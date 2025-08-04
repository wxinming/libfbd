#pragma once
#include "../base.h"
#include "../../../Dependent/Lib/ginkgo/include/ControlI2C.h"
#ifdef _WIN64
#pragma comment(lib, "../../Dependent/Lib/ginkgo/lib/x64/Ginkgo_Driver.lib")
#else
#pragma comment(lib, "../../Dependent/Lib/ginkgo/lib/x86/Ginkgo_Driver.lib")
#endif

namespace fbd {
	class LIBFBD_DLL_EXPORT Ginkgo : public Base
	{
	public:
		Ginkgo(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);
		~Ginkgo();
		bool open(int port, int bitrate, int timeout) override;
		bool close() override;
		int i2cRead(uint8_t* data, uint16_t size, uint16_t slave) override;
		int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave) override;
	};
}
