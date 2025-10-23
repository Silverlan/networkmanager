// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

module;

#include <chrono>

module pragma.network_manager;

import :transfer_rate;

#define TRANSFER_RATE_INTERVAL_MS 500

#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", off)
#endif
nwm::TransferRate::TransferRate() : sample(0), sampleSize(0) {}

void nwm::TransferRate::AddSample(unsigned long long s)
{
	sampleSize += s * (1000 / TRANSFER_RATE_INTERVAL_MS);
	ChronoTimePoint p = util::Clock::now();
	auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(p - lastUpdate);
	if(delta.count() >= TRANSFER_RATE_INTERVAL_MS) {
		if(sample < 5)
			samples[sample++] = sampleSize;
		else {
			for(auto i = 1; i < sample; i++)
				samples[i - 1] = samples[i];
			samples[sample - 1] = sampleSize;
		}
		sampleSize = 0;
		lastUpdate = p;
	}
}
unsigned long long nwm::TransferRate::GetAverage()
{
	if(sample == 0)
		return 0;
	unsigned long long sum = 0;
	for(auto i = 0; i < sample; i++)
		sum += samples[i];
	return sum / sample;
}

void nwm::TransferRate::Reset() { sample = 0; }
#ifdef NWM_DISABLE_OPTIMIZATION
#pragma optimize("", on)
#endif
