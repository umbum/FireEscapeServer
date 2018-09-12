#include "SerialPort.h"



SerialPort::SerialPort(const std::string port_name, unsigned int baud_rate)  {
	boost::system::error_code ec;
	port.open(port_name, ec);  // port_name example : "COM7"
	if (ec) {
		// �����ڿ��� exception �߻� �� �˾Ƽ� ������ delete�� �Ҹ��� ��ü �����ش�.
		throw "[Error] port open fail : " + port_name + '\n'
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

SerialPort::~SerialPort()
{
	stop();
}

void SerialPort::stop()
{
	boost::mutex::scoped_lock lock(mutex);
	if (port.is_open()) {
		port.cancel();
		port.close();
	}
	io_context.stop();
	io_context.restart();
}

void SerialPort::start() {
	// async ������ �� event loop ������ ���ư���.
	boost::thread t([&] { io_context.run(); });
	readLoop();
}

/* async_read_some(MutableBuffer, handler);�޼���� ȣ��� ���ÿ� �����ϹǷ� readLoop()�� �ٷ� ����.
arduino ������ Serial.write()�� �� �� ���� ������ ��ŭ �а� handler ȣ��. handler�� ����.
�ٵ� write() �� ���� �� IO ������ �νĵǸ���� ������ �����ʳ�? ����. ���� 1:1�� �����Ǹ� eol�� ���� �� �ʿ� ���� ��. */
void SerialPort::readLoop() {
	
	port.async_read_some(boost::asio::buffer(read_buf, BUF_SIZE),
		[this](auto error, auto bytes_transferred) {
		boost::mutex::scoped_lock lock(mutex);
		for (int i = 0; i < bytes_transferred; i++) {
			printf("%02x", read_buf[i]);
		}
		printf("\n");
		readLoop();
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



