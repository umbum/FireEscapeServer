#include "SerialPort.h"
#include <Setupapi.h>
#pragma comment(lib, "Setupapi.lib")
#pragma warning(disable:4996)

// e.g., SerialPort serial("COM7", 9600);
SerialPort::SerialPort(const std::string _port_name, unsigned int baud_rate)
	: name(_port_name), read_buf(std::string(BUF_SIZE, 0x00)) {
	boost::system::error_code ec;
	port.open(name, ec);    // open을 호출하는 시점 부터 serial port에서 데이터를 읽어다가 internal system buffer에 쌓아둔다.
	if (ec) {
		// 생성자에서 exception 발생 시 알아서 적절한 delete가 불리며 객체 지워준다.
		throw "[Error] port open fail : " + name + '\n'
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

SerialPort::~SerialPort() {
	stop();
}

void SerialPort::stop() {
	boost::mutex::scoped_lock lock(mutex);
	if (port.is_open()) {
		port.cancel();                  // read blocking 상태에 있는 async 작업을 즉시 blocking 해제 하고 handler를 호출.
		port.close();                   // 내부적으로 cancel() 작업을 수행해주고 포트를 닫음. 그래서 cancel()따로 안해도 되긴 함.
	}
	lock.unlock();      // 여기서 풀어주고 join()해야 thread에서 lock을 획득하기 위해 무한 대기하는 상황을 피할 수 있음.
	if (loop_thread.joinable()) {   // thread가 종료될 때 까지 기다리고
		loop_thread.join();
	}
	
	io_context.stop();       // thread 위에서 돌고 있는 io_context에 stop signal 전송.
	io_context.restart();    // Restart the io_context in preparation for a subsequent run() invocation.
}

void SerialPort::start() {
	readLoop();    // async_read_some을 먼저 호출해 handler를 등록하고 나서 run()해야 한다.
	loop_thread = boost::thread([&] { io_context.run(); });
}


/** async_read_some(MutableBuffer, handler);메서드는 호출과 동시에 리턴하므로 readLoop()는 바로 리턴.
  * arduino 측에서 Serial.write()로 한 번 쓰는 데이터 만큼 읽고 handler 호출. handler는 람다.
  * 받는 쪽에서 대기하고 있으면, 아두이노측의 write/print 호출이 끝나면서 동시에 handler가 바로 호출되지만,
  * 받는 쪽에서 바쁘면, write/print 데이터가 계속 internal system buffer에 쌓여서 read 시 이를 한꺼번에 가져오기 때문에
  * write/print 호출 한 번이 곧 서버에서 한 번의 read로 연결되지는 않는다. 직접 처리해줘야 함. 
  */
void SerialPort::readLoop() {
	// asio::buffer에 vector나 string을 넘겨도 가변이 아니다. buffer의 크기는 초기에 vector.size()로 딱 정해진다.
	port.async_read_some(boost::asio::buffer(read_buf),
		[this] (auto error, auto bytes_transferred) {

		auto splited = cppSplit(read_buf, ",");
		for (auto i = 0; i < splited.size(); i++) {
			printf("%s,", splited[i].c_str());
		}
		std::cout << std::endl;

		boost::mutex::scoped_lock lock(mutex);
		if (port.is_open()) {  // port가 열려 있을 때만 다시 read blocking 상태로.
			readLoop();        // async_read_some이 바로 리턴하면서 readLoop도 바로 리턴하므로 재귀가 아니다. event를 재등록해주는 것.
		}
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

