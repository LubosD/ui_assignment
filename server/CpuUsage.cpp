#include "CpuUsage.h"
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

template <class OutIt>
void explode(const std::string& input, char sep, OutIt& output)
{ 
	std::istringstream buffer(input);
	std::string temp;

	while (std::getline(buffer, temp, sep))
	{
		if (!temp.empty())
			output.push_back(temp);
	}
}


bool cpuTotalAndIdle(CpuSample& sample)
{
	std::ifstream stat("/proc/stat");

	// Find the 'cpu' row in the stat file
	while (true)
	{
		std::string line;

		if (!std::getline(stat, line))
			return false;
		
		std::vector<std::string> fields;
		explode(line, ' ', fields);

		if (fields.size() < 9)
			return false; // Unexpected format

		if (fields[0] != "cpu")
			continue;

		uint64_t user = std::stoi(fields[1]);
		uint64_t nice = std::stoi(fields[2]);
		uint64_t system = std::stoi(fields[3]);
		uint64_t idle = std::stoi(fields[4]);
		uint64_t iowait = std::stoi(fields[5]);
		uint64_t irq = std::stoi(fields[6]);
		uint64_t softirq = std::stoi(fields[7]);
		uint64_t steal = std::stoi(fields[8]);

		uint64_t nonIdle = user + nice + system + irq + softirq + steal;

		sample.idle = idle + iowait;
		sample.total = nonIdle + sample.idle;

		return true;
	}
}

double cpuUsagePct(const CpuSample& previousSample, const CpuSample& currentSample)
{
	auto totald = currentSample.total - previousSample.total;
	auto idled = currentSample.idle - previousSample.idle;

	return double(totald - idled) / totald;
}
