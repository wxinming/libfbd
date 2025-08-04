#pragma once

#include "../base.h"
#include "../../../Dependent/Lib/FT4222/include/ftd2xx.h"
#include "../../../Dependent/Lib/FT4222/include/LibFT4222.h"

#ifdef _WIN64
#pragma comment(lib, "../../Dependent/Lib/FT4222/lib/x64/ftd2xx.lib")
#pragma comment(lib, "../../Dependent/Lib/FT4222/lib/x64/LibFT4222-64.lib")
#else
#pragma comment(lib, "../../Dependent/Lib/FT4222/lib/x86/ftd2xx.lib")
#pragma comment(lib, "../../Dependent/Lib/FT4222/lib/x86/LibFT4222.lib")
#endif

namespace fbd {
	class LIBFBD_DLL_EXPORT Ft4222 : public Base
	{
	public:
		Ft4222(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);
		~Ft4222();
		bool open(int port, int bitrate, int timeout) override;
		bool close() override;
		int i2cRead(uint8_t* data, uint16_t size, uint16_t slave) override;
		int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave) override;

	private:
		void* handle_ = nullptr;
	};
}
