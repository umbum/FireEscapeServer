#pragma once

#include <iostream>

#include <boost/asio.hpp>
#include <boost/thread.hpp>

class SerialPort
{
public:
	enum {
		BUF_SIZE = 256
	};
protected:
	boost::asio::io_context io_context;
	boost::asio::serial_port port{ io_context };
	boost::mutex mutex;

	char read_buf[BUF_SIZE];
	char eol;

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

