/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __NWM_TRANSFERRATE_H__
#define __NWM_TRANSFERRATE_H__

#include <sharedutils/chronoutil.h>

namespace nwm
{
	struct TransferRate
	{
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