#include "StatConnectionHandler.h"
#include <string_view>
#include <boost/log/trivial.hpp>
#include <algorithm>
#include "CpuUsage.h"
#include "MemUsage.h"

// Maximum read buffer size. All commands must fit within this limit, including the newline.
constexpr size_t MAX_COMMAND_LEN = 32;

StatConnectionHandler::StatConnectionHandler(boost::asio::io_service& io, std::unique_ptr<boost::asio::ip::tcp::socket> socket)
: m_io(io), m_socket(std::move(socket)), m_buffer(MAX_COMMAND_LEN)
{

}

void StatConnectionHandler::run()
{
	readMore();

	// TODO: As a possible improvement, we could start a timer here to add timeout functionality, if desired
}

void StatConnectionHandler::readMore()
{
	// Initiate the next asynchronous read.
	// Pass a shared reference to this into the closure to preserve our instance.
	boost::asio::async_read_until(*m_socket.get(), m_buffer, '\n', [=, self = shared_from_this()](const boost::system::error_code& error, size_t bytesRead) {
		readHandler(error, bytesRead);
	});
}

void StatConnectionHandler::readHandler(const boost::system::error_code& error, size_t bytesRead)
{
	if (error || !bytesRead)
	{
		if (error)
			BOOST_LOG_TRIVIAL(error) << "Error reading from client " << m_socket->remote_endpoint() << ": " << error;
		return;
	}

	// Look into what we have in the buffer
	std::istream is(&m_buffer);
    std::string line;
    std::getline(is, line);

	// Not present in the assignment, but makes testing easier via telnet
	if (line.length() > 0 && line[line.length()-1] == '\r')
		line.resize(line.length() - 1);

	processCommand(line);

	// Await further data from the client
	readMore();
}

void StatConnectionHandler::processCommand(const std::string& command)
{
	BOOST_LOG_TRIVIAL(info) << "Handling command " << command;

	// TODO: We could have a nice map of handlers instead of an if/else
	if (command == "cpu")
	{
		processCommandCpu();
	}
	else if (command == "mem")
	{
		processCommandMem();
	}
	else
	{
		BOOST_LOG_TRIVIAL(error) << "Unknown command received from client " << m_socket->remote_endpoint() << ": " << command;
	}
}

void StatConnectionHandler::processCommandCpu()
{
	std::shared_ptr<CpuSample> firstSample(new CpuSample);
	if (!cpuTotalAndIdle(*firstSample.get()))
	{
		BOOST_LOG_TRIVIAL(error) << "Failed to sample CPU usage";

		// TODO: what to do here? Not defined in the assignment.
		sendReply("error\n");
		return;
	}
	
	// We need to take 2 CPU stats samples, one second apart, to know the CPU usage over the past second
	auto timer = std::make_shared<boost::asio::deadline_timer>(m_io, boost::posix_time::seconds(1));

	// Capture this and the timer in the lambda
	timer->async_wait([=, t = timer, self = shared_from_this()](const boost::system::error_code& ec) {
		if (ec)
			return; // This normally happens only if the timer is being canceled

		CpuSample secondSample;

		if (!cpuTotalAndIdle(secondSample))
		{
			BOOST_LOG_TRIVIAL(error) << "Failed to sample CPU usage";

			// TODO: what to do here? Not defined in the assignment.
			sendReply("error\n");
			return;
		}

		auto cpuUsage = cpuUsagePct(*firstSample.get(), secondSample);
		std::stringstream ss;

		ss << (cpuUsage * 100) << "%\n";

		// Send the calculated CPU usage back to the client
		sendReply(ss.str());
	});
}

void StatConnectionHandler::processCommandMem()
{
	uint64_t bytes = getMemoryUsage();
	if (bytes == 0)
	{
		sendReply("error\n");
		return;
	}

	std::stringstream ss;

	// The assignment asks for memory usage in kilobytes
	ss << (bytes / 1024) << " kB\n";

	sendReply(ss.str());
}

void StatConnectionHandler::sendReply(const std::string& reply)
{
	// Need to make a copy that lives until writing is complete
	std::string* buf = new std::string(reply);

	boost::asio::async_write(*m_socket.get(), boost::asio::buffer(buf->c_str(), buf->length()), [=, self = shared_from_this()](const boost::system::error_code& ec, size_t written) {
		if (ec)
			BOOST_LOG_TRIVIAL(error) << "Error sending reply to " << m_socket->remote_endpoint() << ": " << ec;

		delete buf;
	});
}
