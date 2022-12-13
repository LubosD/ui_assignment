#ifndef STAT_CONNECTION_HANDLER_H_
#define STAT_CONNECTION_HANDLER_H_
#include <boost/asio.hpp>
#include <memory>

/// Handles an incoming connection.
class StatConnectionHandler : public std::enable_shared_from_this<StatConnectionHandler>
{
public:
	/// Construct the handler of a new connection
	StatConnectionHandler(boost::asio::io_service& io, std::unique_ptr<boost::asio::ip::tcp::socket> socket);

	/// Call to activate the handler
	void run();
private:
	/// Initiates the next asynchronous read
	void readMore();

	/// Callback handler for when we're done reading the next line from the client
	void readHandler(const boost::system::error_code& error, size_t bytesRead);

	/// Process an incoming command
	void processCommand(const std::string& command);

	/// Handles the "cpu" command
	void processCommandCpu();

	/// Handles the "mem" command
	void processCommandMem();

	/// Sends a reply to the client
	void sendReply(const std::string& reply);
private:
	/// ASIO's IO loop
	boost::asio::io_service& m_io;

	/// The socket we're handling in our instance
	std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;

	/// Read buffer for incoming requests
	boost::asio::streambuf m_buffer;
};

#endif
