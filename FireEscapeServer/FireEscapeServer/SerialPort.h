#pragma once

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "uutil.h"

class SerialPort {
public:
	enum {
		BUF_SIZE = 256
	};
	const std::string name;
protected:
	boost::asio::io_context io_context;
	boost::asio::serial_port port{ io_context };
	boost::mutex mutex;
	boost::thread loop_thread;

	std::string read_buf;
	char eol = 0x0d;
public:
	SerialPort(const std::string port_name, unsigned int baud_rate);
	virtual ~SerialPort();

	virtual void start();
	virtual void stop();
	
	inline void setEol(char _eol) { eol = _eol; }
	inline char getEol()          { return eol; }

	int write(const std::string &buf);
	int write(const char *buf, const int &size);

protected:
	virtual void readLoop();
};

