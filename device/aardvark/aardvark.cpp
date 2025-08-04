#include "aardvark.h"

fbd::Aardvark::Aardvark(uint16_t slaveAddr, uint8_t slaveAddrBits)
	:Base(slaveAddr, slaveAddrBits)
{

}

fbd::Aardvark::~Aardvark()
{
	close();
}

bool fbd::Aardvark::open(int port, int bitrate, int timeout)
{
	bool result = false;
	do
	{
		auto handle = aa_open(port);
		if (handle <= 0) {
			setLastError("�򿪶˿�%dʧ��,�������:%d", port, handle);
			break;
		}

		aa_configure(handle, AA_CONFIG_SPI_I2C);
		aa_i2c_pullup(handle, AA_I2C_PULLUP_BOTH);
		aa_target_power(handle, AA_TARGET_POWER_BOTH);
		aa_i2c_slave_disable(handle);
		if (bitrate != aa_i2c_bitrate(handle, bitrate)) {
			setLastError("���ò�����ʧ��,�������:%d", bitrate);
			aa_close(handle);
			break;
		}

		if (timeout != aa_i2c_bus_timeout(handle, timeout)) {
			setLastError("���ó�ʱʧ��,�������:%d", timeout);
			aa_close(handle);
			break;
		}

		handle_ = handle;
		port_ = port;
		is_open_ = true;
		result = true;
	} while (false);
	return result;
}

bool fbd::Aardvark::close()
{
	bool result = false;
	do
	{
		if (is_open_) {
			aa_close(handle_);
			handle_ = -1;
			is_open_ = false;
			port_ = -1;
		}
		result = true;
	} while (false);
	return result;
}

int fbd::Aardvark::i2cRead(uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto oldSlave = slave;
	AardvarkI2cFlags flags = AA_I2C_NO_FLAGS;
	if (slave_addr_bits_ == 0x07) {
		slave >>= 1;
	}
	else if (slave_addr_bits_ == 0x0a) {
		flags = AA_I2C_10_BIT_ADDR;
	}

	auto result = aa_i2c_read(handle_, slave, flags, size, data);
	if (result < 0) {
		setLastError("��ȡʧ��,%s", aa_status_string(result));
	}
	else if (result == 0) {
		setLastError("��ȡʧ��,�ӵ�ַ����ȷ���������");
	}
	else if (result != size) {
		setLastError("��ȡ��ʧ%d���ֽ�,����岻�ȶ�������̫��", size - result);
	}
	else {
#ifdef _DEBUG
		outputDebugString("R", data, size, slave);
#endif

		if (onMessage) {
			onMessage("R", data, size, oldSlave);
		}
	}

	if (delay_.read > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_.read));
	}
	return result;
}

int fbd::Aardvark::i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave)
{
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto oldSlave = slave;
	AardvarkI2cFlags flags = AA_I2C_NO_FLAGS;
	if (slave_addr_bits_ == 0x07) {
		slave >>= 1;
	}
	else if (slave_addr_bits_ == 0x0a) {
		flags = AA_I2C_10_BIT_ADDR;
	}

	auto result = aa_i2c_write(handle_, slave, flags, size, data);
	if (result < 0) {
		setLastError("д��ʧ��,%s", aa_status_string(result));
	}
	else if (result == 0) {
		setLastError("д��ʧ��,�ӵ�ַ����ȷ���������");
	}
	else if (result != size) {
		setLastError("д�붪ʧ%d���ֽ�,����岻�ȶ�������̫��", size - result);
	}

#ifdef _DEBUG
	outputDebugString("W", data, size, slave);
#endif

	if (onMessage) {
		onMessage("W", data, size, oldSlave);
	}

	if (delay_.write > 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(delay_.write));
	}
	return result;
}
