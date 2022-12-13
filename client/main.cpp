#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>

void printHelp(const char* progname);
void runCommand(boost::asio::ip::tcp::socket& socket, const std::string& command);

int main(int argc, const char** argv)
{
	if (argc != 2)
	{
		printHelp(argv[0]);
		exit(EXIT_FAILURE);
	}

	try
	{
		boost::asio::io_context io_context;
		boost::asio::ip::tcp::resolver resolver(io_context);

		boost::asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "5001");

		boost::asio::ip::tcp::socket socket(io_context);
		boost::asio::connect(socket, endpoints);

		std::cout << "Connected to the server!\n";

		std::string line;

		while (true)
		{
			std::cout << "Which command should I execute (mem/cpu)? ";

			if (!std::getline(std::cin, line))
				break;

			if (line == "cpu" || line == "mem")
				runCommand(socket, line);
			else
				std::cerr << "Unrecognized command!\n";
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
}

void printHelp(const char* progname)
{
	std::cerr << "USAGE:\n"
		<< progname << " <server-ip>\n\n";

	std::cerr << "The server is expected to listen on port 5001.\n";
}

void runCommand(boost::asio::ip::tcp::socket& socket, const std::string& command)
{
	std::string commandAndNL = command + '\n';

	boost::asio::write(socket, boost::asio::buffer(commandAndNL.c_str(), commandAndNL.size()));

	boost::asio::streambuf buf;
	boost::asio::read_until(socket, buf, '\n');

	std::istream is(&buf);
    std::string line;
    std::getline(is, line);

	std::cout << "Reply received: " << line << std::endl;
}
