#include "base.h"
#include <stdarg.h>

fbd::Base::Base(uint16_t slaveAddr, uint8_t slaveAddrBits)
	:slave_addr_(slaveAddr), slave_addr_bits_(slaveAddrBits)
{
	timer_.output.getStartTime();
}

fbd::Base::~Base()
{

}

void fbd::Base::setSlaveAddr(uint16_t addr)
{
	slave_addr_ = addr;
}

uint16_t fbd::Base::getSlaveAddr() const
{
	return slave_addr_;
}

void fbd::Base::setSlaveAddrBits(uint8_t bits)
{
	slave_addr_bits_ = bits;
}

uint8_t fbd::Base::getSlaveAddrBits() const
{
	return slave_addr_bits_;
}

int fbd::Base::i2cWrite(const std::initializer_list<uint8_t>& data, uint16_t slave)
{
	return i2cWrite(data.begin(), static_cast<uint16_t>(data.size()), slave);
}

bool fbd::Base::i2cAddrRead(uint32_t addr, uint8_t addrLen, uint8_t* data, uint16_t dataSize, uint16_t slave)
{
	bool result = false;
	do
	{
		if (!i2cAddrWrite(addr, addrLen, nullptr, 0, false, slave)) {
			break;
		}

		auto count = i2cRead(data, dataSize, slave);
		if (count != dataSize) {
			setLastError("读取地址0x%04x失败,%s", addr, last_error_.c_str());
			break;
		}

		result = true;
	} while (false);
	return result;
}

bool fbd::Base::i2cAddrWrite(uint32_t addr, uint8_t addrLen, const uint8_t* data, uint16_t dataSize, bool check, uint16_t slave)
{
	bool result = false, success = true;
	do
	{
		uint8_t addrData[8] = { 0 };
		for (int i = 0; i < addrLen; i++) {
			addrData[i] = addr >> ((addrLen - i - 1) * 8);
		}

		uint8_t writeData[4096] = { 0 };
		memcpy(writeData, addrData, addrLen);

		auto writeLen = addrLen;
		if (dataSize > 0 && data) {
			writeLen += dataSize;
			memcpy(&writeData[addrLen], data, dataSize);
		}

		auto count = i2cWrite(writeData, writeLen, slave);
		if (count != writeLen) {
			setLastError("写入地址0x%04x失败,%s", addr, last_error_.c_str());
			break;
		}

		if (check) {
			if (!i2cAddrRead(addr, addrLen, writeData, dataSize, slave)) {
				break;
			}

			for (int i = 0; i < dataSize; i++) {
				if (data[i] != writeData[i]) {
					setLastError("写入地址0x%04x校验失败", addr);
					success = false;
					break;
				}
			}
		}

		if (!success) {
			break;
		}
		result = true;
	} while (false);
	return result;
}

bool fbd::Base::i2cAddrWrite(uint32_t addr, uint8_t addrLen, const std::initializer_list<uint8_t>& data, bool check, uint16_t slave)
{
	return i2cAddrWrite(addr, addrLen, data.begin(), static_cast<uint16_t>(data.size()), check, slave);
}

bool fbd::Base::isOpen() const
{
	return is_open_;
}

int fbd::Base::getPort() const
{
	return port_;
}

std::string fbd::Base::getLastError() const
{
	return last_error_;
}

void fbd::Base::setReadDelay(uint32_t delay)
{
	delay_.read = delay;
}

uint32_t fbd::Base::getReadDelay() const
{
	return delay_.read;
}

void fbd::Base::setWriteDelay(uint32_t delay)
{
	delay_.write = delay;
}

uint32_t fbd::Base::getWriteDelay() const
{
	return delay_.write;
}

void fbd::Base::setLastError(const char* fmt, ...)
{
	va_list ap = nullptr;
	va_start(ap, fmt);
	auto length = _vscprintf_p(fmt, ap) + 1;
	std::unique_ptr<char[]> buffer(new char[length]);
	memset(buffer.get(), 0, length);
	_vsprintf_p(buffer.get(), length, fmt, ap);
	va_end(ap);
	last_error_ = buffer.get();
}

void fbd::Base::outputDebugString(const char* type, const uint8_t* data, uint16_t size, uint16_t slave)
{
	auto result = std::format("{:06.5f}> 0x{:04x} {}:[", timer_.output.getEndTime(), slave, type);
	for (int i = 0; i < size; ++i) {
		result.append(std::format("0x{:02x}", data[i]));
		if (i != size - 1) {
			result.append(" ");
		}
	}
	result.append("]\n");
	OutputDebugStringA(result.c_str());
}
