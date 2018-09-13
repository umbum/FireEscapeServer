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

