#include "SerialPort.h"
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")
#pragma warning(disable:4996)

// e.g., SerialPort serial("COM7", 9600);
SerialPort::SerialPort(const std::string _port_name, unsigned int baud_rate)
	: name(_port_name), read_buf(std::string(BUF_SIZE, 0x00)) {
	boost::system::error_code ec;
	port.open(name, ec);    // open�� ȣ���ϴ� ���� ���� serial port���� �����͸� �о�ٰ� internal system buffer�� �׾Ƶд�.
	if (ec) {
		// �����ڿ��� exception �߻� �� �˾Ƽ� ������ delete�� �Ҹ��� ��ü �����ش�.
		throw "[Error] port open fail : " + name + '\n'
		    + "        error msg      : " + ec.message();
	}
	
	typedef boost::asio::serial_port_base port_base;
	port.set_option(port_base::baud_rate(baud_rate));
	// �Ʒ��� ��� default ���� ��Ÿ��.
	//port.set_option(port_base::character_size(8));
	//port.set_option(port_base::stop_bits(port_base::stop_bits::one));
	//port.set_option(port_base::parity(port_base::parity::none));
	//port.set_option(port_base::flow_control(port_base::flow_control::none));
}

SerialPort::~SerialPort() {
	stop();
}

void SerialPort::stop() {
	boost::mutex::scoped_lock lock(mutex);
	if (port.is_open()) {
		port.cancel();                  // read blocking ���¿� �ִ� async �۾��� ��� blocking ���� �ϰ� handler�� ȣ��.
		port.close();                   // ���������� cancel() �۾��� �������ְ� ��Ʈ�� ����. �׷��� cancel()���� ���ص� �Ǳ� ��.
	}
	lock.unlock();      // ���⼭ Ǯ���ְ� join()�ؾ� thread���� lock�� ȹ���ϱ� ���� ���� ����ϴ� ��Ȳ�� ���� �� ����.
	if (loop_thread.joinable()) {   // thread�� ����� �� ���� ��ٸ���
		loop_thread.join();
	}
	
	io_context.stop();       // thread ������ ���� �ִ� io_context�� stop signal ����.
	io_context.restart();    // Restart the io_context in preparation for a subsequent run() invocation.
}

void SerialPort::start() {
	readLoop();    // async_read_some�� ���� ȣ���� handler�� ����ϰ� ���� run()�ؾ� �Ѵ�.
	loop_thread = boost::thread([&] { io_context.run(); });
}


/** async_read_some(MutableBuffer, handler);�޼���� ȣ��� ���ÿ� �����ϹǷ� readLoop()�� �ٷ� ����.
  * arduino ������ Serial.write()�� �� �� ���� ������ ��ŭ �а� handler ȣ��. handler�� ����.
  * �޴� �ʿ��� ����ϰ� ������, �Ƶ��̳����� write/print ȣ���� �����鼭 ���ÿ� handler�� �ٷ� ȣ�������,
  * �޴� �ʿ��� �ٻڸ�, write/print �����Ͱ� ��� internal system buffer�� �׿��� read �� �̸� �Ѳ����� �������� ������
  * write/print ȣ�� �� ���� �� �������� �� ���� read�� ��������� �ʴ´�. ���� ó������� ��. 
  */
void SerialPort::readLoop() {
	// asio::buffer�� vector�� string�� �Ѱܵ� ������ �ƴϴ�. buffer�� ũ��� �ʱ⿡ vector.size()�� �� ��������.
	port.async_read_some(boost::asio::buffer(read_buf),
		[this] (auto error, auto bytes_transferred) {

		auto splited = cppSplit(read_buf, ",");
		for (auto i = 0; i < splited.size(); i++) {
			printf("%s,", splited[i].c_str());
		}
		std::cout << std::endl;

		boost::mutex::scoped_lock lock(mutex);
		if (port.is_open()) {  // port�� ���� ���� ���� �ٽ� read blocking ���·�.
			readLoop();        // async_read_some�� �ٷ� �����ϸ鼭 readLoop�� �ٷ� �����ϹǷ� ��Ͱ� �ƴϴ�. event�� �������ִ� ��.
		}
	});
}

int SerialPort::write(const std::string &str) {
	return write(str.c_str(), str.size());
}

int SerialPort::write(const char *data, const int &size) {
	if (size == 0)    return 0;
	// write_some(buffer)�� ���� �� exception. write_some(buffer, ec)�� ���� �� 0�� ����.
	return port.write_some(boost::asio::buffer(data, size));
}



//////////////////////////////////////////////////////////////////////////
///////////////////      static methods     //////////////////////////////
//////////////////////////////////////////////////////////////////////////

/*
std::vector<std::string> SerialPort::get_port_names() {
	std::vector<std::string> names;

	BOOL rv;
	DWORD size;
	GUID guid[1];
	HDEVINFO hdevinfo;
	DWORD idx = 0;
	SP_DEVINFO_DATA devinfo_data;
	devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
	int count = 0;

	rv = SetupDiClassGuidsFromName("Ports", (LPGUID)&guid, 1, &size);
	if (!rv) {
		std::cout << "error : SetupDiClassGuidsFromName() failed..." << std::endl;
		return names;
	}

	hdevinfo = SetupDiGetClassDevs(&guid[0], NULL, NULL, DIGCF_PRESENT | DIGCF_PROFILE);
	if (hdevinfo == INVALID_HANDLE_VALUE) {
		std::cout << "error : SetupDiGetClassDevs() failed..." << std::endl;
		return names;
	}

	while (SetupDiEnumDeviceInfo(hdevinfo, idx++, &devinfo_data)) {
		char friendly_name[MAX_PATH];
		char port_name[MAX_PATH];
		DWORD prop_type;
		DWORD type = REG_SZ;
		HKEY hKey = NULL;

		rv = ::SetupDiGetDeviceRegistryProperty(hdevinfo, &devinfo_data, SPDRP_FRIENDLYNAME, &prop_type,
			(LPBYTE)friendly_name, sizeof(friendly_name), &size);
		if (!rv) {
			std::cout << "error : SetupDiGetDeviceRegistryProperty() failed..." << std::endl;
			continue;
		}

		hKey = ::SetupDiOpenDevRegKey(hdevinfo, &devinfo_data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
		if (!hKey) continue;

		size = sizeof(port_name);
		rv = ::RegQueryValueEx(hKey, "PortName", 0, &type, (LPBYTE)&port_name, &size);
		::RegCloseKey(hKey);

		names.push_back(port_name);
	}

	SetupDiDestroyDeviceInfoList(hdevinfo);

	return names;
}

int SerialPort::get_port_number() {
	std::vector<std::string> names = get_port_names();
	return names.size();
}

std::string SerialPort::get_port_name(const unsigned int &idx) {
	std::vector<std::string> names = get_port_names();
	if (idx >= names.size()) return std::string();
	return names[idx];
}

void SerialPort::print_devices() {
	std::cout << "SerialPort::print_devices()" << std::endl;
	int n = SerialPort2::get_port_number();
	for (int i = 0; i < n; ++i) {
		std::string name = SerialPort2::get_port_name(i);
		std::cout << "\t" << name.c_str() << std::endl;
	}
}

*/