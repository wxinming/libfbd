#pragma once
#include "../Base.h"
#include "aardvark-core.h"

namespace fbd  {
	class LIBFBD_DLL_EXPORT Aardvark : public Base
	{
	public:
		Aardvark(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);
		~Aardvark();
		bool open(int port, int bitrate, int timeout) override;
		bool close() override;
		int i2cRead(uint8_t* data, uint16_t size, uint16_t slave) override;
		int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave) override;

	private:
		int handle_ = -1;
	};
}
