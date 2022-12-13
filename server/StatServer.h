#ifndef STAT_SERVER_H_
#define STAT_SERVER_H_
#include <boost/asio.hpp>
#include <memory>

// A simple TCP server that accepts new TCP connections on provided port
// and hands off the actual client handling to class StatConnectionHandler.
class StatServer
{
public:
	StatServer();
	~StatServer();

	// Start listening on provided port
	void start(boost::asio::ip::port_type port);

	// Stop listening and exit the event loop.
	void stop();

	// Run the event loop. Doesn't return until stop() is called.
	void run();

	// Add SIGINT and SIGTERM handlers and stop the event loop if they are received
	void terminateOnSignal();
private:
	void acceptNext();
	void acceptHandler(const boost::system::error_code& error);
private:
	boost::asio::io_service m_io;

	// Main listening socket
	boost::asio::ip::tcp::acceptor m_socket;

	// The next incoming connection object
	std::unique_ptr<boost::asio::ip::tcp::socket> m_nextConnection;

	// Used for SIGINT / SIGTERM handling
	boost::asio::signal_set m_terminateSignals;
};

#endif
