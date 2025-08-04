#include "ginkgo.h"

fbd::Ginkgo::Ginkgo(uint16_t slaveAddr, uint8_t slaveAddrBits)
	:Base(slaveAddr, slaveAddrBits)
{

}

fbd::Ginkgo::~Ginkgo()
{
	close();
}

bool fbd::Ginkgo::open(int port, int bitrate, int timeout)
{
	bool result = false;
	do 
	{
		auto count = VII_ScanDevice();
		if (0 == count) {
			setLastError("请插入烧录器设备");
			break;
		}

		if (port > count - 1) {
			setLastError("未找到设备端口:%d", port);
			break;
		}

		auto code = VII_OpenDevice(VII_USBI2C, port, 0);
		if (0 != code) {
			setLastError("打开设备失败,错误代码:%d", code);
			break;
		}

		VII_INIT_CONFIG init = { 0 };
		init.AddrType = slave_addr_bits_ == 0x07 ? VII_ADDR_7BIT : VII_ADDR_10BIT;
		init.ClockSpeed = bitrate * 1000;
		init.ControlMode = VII_HCTL_MODE;
		init.MasterMode = VII_MASTER;
		init.SubAddrWidth = VII_SUB_ADDR_NONE;
		code = VII_InitI2C(VII_USBI2C, 0, 0, &init);
		if (0 != code) {
			setLastError("配置设备I2C失败,错误代码:%d", code);
			break;
		}

		VII_TIME_CONFIG time = { 0 };
		time.tSU_STA = 5;
		time.tHD_STA = 4;
		time.tLOW = 5;
		time.tHIGH = 5;
		time.tSU_DAT = 1;
		time.tSU_STO = 4;
		time.tBuf = 5;
		code = VII_TimeConfig(VII_USBI2C, port, 0, &time);
		if (0 != code) {
			setLastError("配置设备时序失败,错误代码:%d", code);
			break;
		}

		is_open_ = true;
		port_ = port;
		result = true;
	} while (false);
	return result;
}

bool fbd::Ginkgo::close()
{
	if (is_open_) {
		VII_CloseDevice(VII_USBI2C, port_);
		is_open_ = false;
		port_ = -1;
	}
	return true;
}

int fbd::Ginkgo::i2cRead(uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto code = VII_ReadBytes(VII_USBI2C, port_, 0, slave, 0, data, size);
	if (0 != code) {
		setLastError("读取失败,从地址不正确或解码板故障,错误代码:%d", code);
		size = 0;
	}
	else {
#ifdef _DEBUG
		outputDebugString("R", data, size, slave);
#endif

		if (onMessage) {
			onMessage("R", data, size, slave);
		}
	}

	if (delay_.read > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_.read));
	}
	return size;
}

int fbd::Ginkgo::i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto code = VII_WriteBytes(VII_USBI2C, port_, 0, slave, 0, const_cast<uint8_t*>(data), size);
	if (0 != code) {
		setLastError("写入失败,从地址不正确或解码板故障,错误代码:%d", code);
		size = 0;
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

