#pragma once
#include "device/base.h"

// firmware burning device
namespace fbd {
	/*
	* @brief 自动释放new
	* @param[in] type 设备类型
	* @param[in] slaveAddr 从地址
	* @param[in] slaveAddrBits 从地址宽度
	* @return fbd::Base
	*/
	LIBFBD_DLL_EXPORT std::shared_ptr<Base> autoReleaseNew(DeviceType type, uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);

	/*
	* @brief 搜索设备
	* @param[in] type 设备类型
	* @param[out] devices 搜索到的设备
	* @param[in] maxDevices 最大设备数
	* @param[out] msg 错误消息
	* @return bool
	*/
	LIBFBD_DLL_EXPORT bool searchDevices(DeviceType type, std::map<int, std::string>& devices, int maxDevices = 16, std::string* msg = nullptr);

	/*
	* @brief 翻译类型
	* @param[in] deviceName 设备名称
	* @return 设备类型
	*/
	LIBFBD_DLL_EXPORT DeviceType translateType(const std::string& deviceName);

	/*
	* @brief 获取支持的设备类型
	* @return 设备类型
	*/
	LIBFBD_DLL_EXPORT std::vector<std::string> getSupportDeviceType();
}

