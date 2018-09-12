#include "SerialPort.h"



SerialPort::SerialPort(const std::string port_name, unsigned int baud_rate)  {
	boost::system::error_code ec;
	port.open(port_name, ec);  // port_name example : "COM7"
	if (ec) {
		// 생성자에서 exception 발생 시 알아서 적절한 delete가 불리며 객체 지워준다.
		throw "[Error] port open fail : " + port_name + '\n'
		    + "        error msg      : " + ec.message();
	}
	
	typedef boost::asio::serial_port_base port_base;
	port.set_option(port_base::baud_rate(baud_rate));
	// 아래는 모두 default 값을 나타냄.
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
	// async 동작은 이 event loop 위에서 돌아간다.
	boost::thread t([&] { io_context.run(); });
	readLoop();
}

/* async_read_some(MutableBuffer, handler);메서드는 호출과 동시에 리턴하므로 readLoop()는 바로 리턴.
arduino 측에서 Serial.write()로 한 번 쓰는 데이터 만큼 읽고 handler 호출. handler는 람다.
근데 write() 한 번이 꼭 IO 끝으로 인식되리라는 보장은 없지않나? 싶음. 만약 1:1로 대응되면 eol을 따로 쓸 필요 없을 듯. */
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
	// write_some(buffer)는 에러 시 exception. write_some(buffer, ec)는 에러 시 0을 리턴.
	return port.write_some(boost::asio::buffer(data, size));
}



