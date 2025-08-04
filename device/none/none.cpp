#include "none.h"

fbd::None::None(uint16_t slaveAddr, uint8_t slaveAddrBits)
	:Base(slaveAddr, slaveAddrBits)
{

}

fbd::None::~None()
{
	close();
}

bool fbd::None::open(int port, int bitrate, int timeout)
{
	return true;
}

bool fbd::None::close()
{
	return true;
}

int fbd::None::i2cRead(uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

#ifdef _DEBUG
	outputDebugString("R", data, size, slave);
#endif

	if (onMessage) {
		onMessage("R", data, size, slave);
	}

	if (delay_.read > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_.read));
	}
	return size;
}

int fbd::None::i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

#ifdef _DEBUG
	outputDebugString("W", data, size, slave);
#endif

	if (onMessage) {
		onMessage("W", data, size, slave);
	}

	if (delay_.write > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_.write));
	}
	return size;
}

