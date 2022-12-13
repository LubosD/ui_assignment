#include "MemUsage.h"
#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>
#include <boost/log/trivial.hpp>

static const std::regex g_reMeminfoLine("^([^:]+): +(\\d+) kB");

uint64_t getMemoryUsage()
{
	std::ifstream proc("/proc/meminfo");

	if (!proc.is_open())
	{
		BOOST_LOG_TRIVIAL(error) << "Cannot read /proc/meminfo";
		return 0;
	}

	std::unordered_map<std::string, uint64_t> memFields;
	std::string line;

	while (std::getline(proc, line))
	{
		std::smatch match;

		if (!std::regex_match(line, match, g_reMeminfoLine))
			continue;

		memFields.insert({ match[1], std::stoul(match[2]) * 1024 });
	}

	uint64_t memtotal, memfree, buffers, cached;

	try
	{
		memtotal = memFields.at("MemTotal");
		memfree = memFields.at("MemFree");
		buffers = memFields.at("Buffers");
		cached = memFields.at("Cached");

		return memtotal - memfree - buffers - cached;
	}
	catch (const std::out_of_range& e)
	{
		// Some required field was not found
		BOOST_LOG_TRIVIAL(error) << "Expected data not found in /proc/meminfo";
		return 0;
	}
}
