#include "StatServer.h"
#include "StatConnectionHandler.h"
#include <boost/log/trivial.hpp>
#include <cstdlib>
#include <signal.h>
#include <stdexcept>

StatServer::StatServer()
: m_socket(m_io), m_terminateSignals(m_io, SIGINT, SIGTERM)
{

}

StatServer::~StatServer()
{
	stop();
}

void StatServer::start(boost::asio::ip::port_type port)
{
	if (m_socket.is_open())
		throw std::logic_error("Server already started");

	// NOTE: Assignment didn't specify whether the server should be IPv6 enabled or not
	m_socket.open(boost::asio::ip::tcp::v4());

	// Make our life easier by enabling address reuse
	boost::asio::socket_base::reuse_address option(true);
	m_socket.set_option(option);

	m_socket.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port));

	m_socket.listen(5);

	acceptNext();
}

void StatServer::stop()
{
	if (m_io.stopped())
		return;

	BOOST_LOG_TRIVIAL(info) << "Shutting down...";
	m_io.stop();
}

void StatServer::terminateOnSignal()
{
	m_terminateSignals.async_wait([=](const boost::system::error_code& error, int signalNumber) {
		if (!error)
			stop();
	});
}

void StatServer::acceptNext()
{
	m_nextConnection.reset(new boost::asio::ip::tcp::socket(m_io));
    m_socket.async_accept(*m_nextConnection.get(), [=](auto ec) {
		acceptHandler(ec);
	});
}

void StatServer::acceptHandler(const boost::system::error_code& error)
{
	if (error)
	{
		BOOST_LOG_TRIVIAL(fatal) << "Failed to accept new connection: " << error;

		// Let's consider the error fatal for the purpose of this assignment
		exit(EXIT_FAILURE);

		return;
	}

	BOOST_LOG_TRIVIAL(info) << "New incoming connection from " << m_nextConnection->remote_endpoint();

	// Fire off a handler for this client...
	auto handler = std::make_shared<StatConnectionHandler>(m_io, std::move(m_nextConnection));
	handler->run();

	// ...and get ready to accept the next one.
	acceptNext();
}

void StatServer::run()
{
	m_io.run();
}
