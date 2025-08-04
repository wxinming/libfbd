#pragma once
#include "../base.h"

namespace fbd {
	class None : public Base
	{
	public:
		None(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);
		~None();
		bool open(int port, int bitrate, int timeout) override;
		bool close() override;
		int i2cRead(uint8_t* data, uint16_t size, uint16_t slave) override;
		int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave) override;
	};
}
