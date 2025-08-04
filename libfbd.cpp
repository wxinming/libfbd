#include "libfbd.h"
#include "device/aardvark/aardvark.h"
#include "device/ft4222/ft4222.h"
#include "device/none/none.h"
#include "device/ginkgo/ginkgo.h"

std::shared_ptr<fbd::Base> fbd::autoReleaseNew(DeviceType type, uint16_t slaveAddr, uint8_t slaveAddrBits)
{
	std::shared_ptr<fbd::Base> base;
	switch (type)
	{
	case DeviceType::AARDVARK:
		base = std::make_shared<Aardvark>(slaveAddr, slaveAddrBits);
		break;
	case DeviceType::FT4222:
		base = std::make_shared<Ft4222>(slaveAddr, slaveAddrBits);
		break;
	case DeviceType::GINKGO:
		base = std::make_shared<Ginkgo>(slaveAddr, slaveAddrBits);
		break;
	case DeviceType::NONE:
		base = std::make_shared<None>(slaveAddr, slaveAddrBits);
		break;
	default:
		break;
	}

	return base;
}

bool fbd::searchDevices(DeviceType type, std::map<int, std::string>& devices, int maxDevices, std::string* msg)
{
	bool result = false, success = true;
	do
	{
		if (!devices.empty()) {
			devices.clear();
		}

		if (type == DeviceType::AARDVARK) {
			auto devs = std::make_unique<uint16_t[]>(maxDevices);
			auto sns = std::make_unique<uint32_t[]>(maxDevices);
			auto size = aa_find_devices_ext(maxDevices, devs.get(), maxDevices, sns.get());
			if (size < 0) {
				if (msg) {
					*msg = std::format("搜索设备失败,错误代码:{}", size);
				}
				break;
			}

			for (int i = 0; i < std::min<int>(size, maxDevices); ++i) {
				devices[devs[i]] = std::to_string(sns[i]);
			}
		}
		else if (type == DeviceType::FT4222) {
			DWORD number = 0;
			FT_STATUS status = FT_CreateDeviceInfoList(&number);
			if (status != FT_OK) {
				if (msg) {
					*msg = "创建设备信息列表失败";
				}
				break;
			}

			auto nodes = std::make_unique<FT_DEVICE_LIST_INFO_NODE[]>(maxDevices);
			DWORD size = maxDevices;
			status = FT_GetDeviceInfoList(nodes.get(), &size);
			if (status != FT_OK) {
				if (msg) {
					*msg= "获取设备信息列表失败";
				}
				break;
			}

			auto index = 0;
			for (auto i = 0; i < std::min<int>(size, maxDevices); ++i) {
				if (!strcmp(nodes[i].Description, "FT4222") ||
					!strcmp(nodes[i].Description, "FT4222 A")) {
					devices[index++] = nodes[i].SerialNumber;
				}
			}
		}
		else if (type == DeviceType::GINKGO) {
			auto count = VII_ScanDevice();
			for (int i = 0; i < count; ++i) {
				VII_BOARD_INFO info;
				auto code = VII_ReadBoardInfo(i, &info);
				if (0 != code) {
					if (msg) {
						*msg = std::format("读取设备{}信息失败,错误代码:{}", i, code);
					}
					success = false;
					break;
				}

				auto size = sizeof(info.SerialNumber) / sizeof(*info.SerialNumber);
				auto sn = std::make_unique<char[]>(size * 2 + 1);
				for (int j = 0; j < size; ++j) {
					sprintf_s(&sn[j * 2], 3, "%02X", info.SerialNumber[j]);
				}
				sn[size * 2] = '\0';
				devices[i] = sn.get();
			}

			if (!success) {
				break;
			}
		}
		else if (type == DeviceType::NONE) {
			for (int i = 0; i < maxDevices; ++i) {
				devices[i] = std::to_string(0xabcdef1 + i);
			}
		}
		else {
			if (msg) {
				*msg = "未知的设备类型";
			}
			break;
		}

		result = true;
	} while (false);
	return result;
}

fbd::DeviceType fbd::translateType(const std::string& deviceName)
{
	fbd::DeviceType type;
	if (0 == _stricmp(deviceName.c_str(), "aardvark")) {
		type = DeviceType::AARDVARK;
	}
	else if (0 == _stricmp(deviceName.c_str(), "ft4222")) {
		type = DeviceType::FT4222;
	}
	else if (0 == _stricmp(deviceName.c_str(), "ginkgo")) {
		type = DeviceType::GINKGO;
	}
	else {
		type = DeviceType::NONE;
	}
	return type;
}

std::vector<std::string> fbd::getSupportDeviceType()
{
	return { "AARDVARK", "FT4222", "GINKGO", "NONE" };
}

