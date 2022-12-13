#include "StatServer.h"
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <boost/log/trivial.hpp>

constexpr int port = 5001;

int main()
{
	try
	{
		StatServer server;

		server.start(port);

		BOOST_LOG_TRIVIAL(info) << "Startup successful, forking into background...";

		if (daemon(false, false) == -1)
		{
			std::stringstream ss;
			ss << "Error forking to background: " << strerror(errno);
			throw std::runtime_error(ss.str());
		}

		// Allow for clean shutdowns
		server.terminateOnSignal();

		server.run();
		return EXIT_SUCCESS;
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << '\n';
		return EXIT_FAILURE;
	}
	
}
