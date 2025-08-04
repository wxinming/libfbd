#pragma once
#include "device/base.h"

// firmware burning device
namespace fbd {
	/*
	* @brief �Զ��ͷ�new
	* @param[in] type �豸����
	* @param[in] slaveAddr �ӵ�ַ
	* @param[in] slaveAddrBits �ӵ�ַ���
	* @return fbd::Base
	*/
	LIBFBD_DLL_EXPORT std::shared_ptr<Base> autoReleaseNew(DeviceType type, uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);

	/*
	* @brief �����豸
	* @param[in] type �豸����
	* @param[out] devices ���������豸
	* @param[in] maxDevices ����豸��
	* @param[out] msg ������Ϣ
	* @return bool
	*/
	LIBFBD_DLL_EXPORT bool searchDevices(DeviceType type, std::map<int, std::string>& devices, int maxDevices = 16, std::string* msg = nullptr);

	/*
	* @brief ��������
	* @param[in] deviceName �豸����
	* @return �豸����
	*/
	LIBFBD_DLL_EXPORT DeviceType translateType(const std::string& deviceName);

	/*
	* @brief ��ȡ֧�ֵ��豸����
	* @return �豸����
	*/
	LIBFBD_DLL_EXPORT std::vector<std::string> getSupportDeviceType();
}

