// SPDX-FileCopyrightText: (c) 2019 Silverlan <opensource@pragma-engine.com>
// SPDX-License-Identifier: MIT

#ifndef __NWM_TRANSFERRATE_H__
#define __NWM_TRANSFERRATE_H__

#include <sharedutils/chronoutil.h>

namespace nwm {
	struct TransferRate {
	  private:
		char sample;
		ChronoTimePoint lastUpdate;
		unsigned long long sampleSize;
	  public:
		TransferRate();
		unsigned long long samples[5];
		void AddSample(unsigned long long s);
		unsigned long long GetAverage();
		void Reset();
	};
};

#endif
