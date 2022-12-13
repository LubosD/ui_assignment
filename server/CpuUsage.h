#ifndef _CPU_USAGE_H
#define _CPU_USAGE_H
#include <stdint.h>

// Contains collected CPU usage data
struct CpuSample
{
	uint64_t total, idle;
};

// Sample the current cpu usage statistics
bool cpuTotalAndIdle(CpuSample& sample);

// After acquiring two samples one second apart, call this function to get the CPU usage percentage
double cpuUsagePct(const CpuSample& previousSample, const CpuSample& currentSample);

#endif
