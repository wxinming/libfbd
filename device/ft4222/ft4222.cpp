#include "Ft4222.h"

fbd::Ft4222::Ft4222(uint16_t slaveAddr, uint8_t slaveAddrBits)
	:Base(slaveAddr, slaveAddrBits)
{

}

fbd::Ft4222::~Ft4222()
{
	close();
}

bool fbd::Ft4222::open(int port, int bitrate, int timeout)
{
	bool result = false;
	do
	{
		DWORD deviceNumber = 0;
		if (FT_CreateDeviceInfoList(&deviceNumber) != FT_OK) {
			setLastError("�����豸��Ϣ�б�ʧ��");
			break;
		}

		if (deviceNumber <= 0) {
			setLastError("�������¼���豸");
			break;
		}

		std::map<int, DWORD> nodes;
		auto index = 0;
		for (DWORD i = 0; i < deviceNumber; ++i) {
			FT_DEVICE_LIST_INFO_NODE node;
			memset(&node, 0, sizeof(FT_DEVICE_LIST_INFO_NODE));
			if (FT_GetDeviceInfoDetail(i, &node.Flags, &node.Type, &node.ID,
				&node.LocId, node.SerialNumber, node.Description, &node.ftHandle) != FT_OK) {
				setLastError("��ȡ�豸��Ϣ����ʧ��");
				break;
			}

			if (!strcmp(node.Description, "FT4222") ||
				!strcmp(node.Description, "FT4222 A")) {
				nodes[index++] = node.LocId;
			}
		}

		if (nodes.size() - 1 < port) {
			setLastError("δ�ҵ��豸�˿�:%d", port);
			break;
		}

		if (FT_OpenEx(reinterpret_cast<void*>(static_cast<ULONG_PTR>(nodes[port])), FT_OPEN_BY_LOCATION, &handle_) != FT_OK) {
			setLastError("���豸ʧ��");
			break;
		}

		if (FT4222_I2CMaster_Init(handle_, bitrate) != FT_OK) {
			setLastError("��������ʧ��");
			FT4222_UnInitialize(handle_);
			FT_Close(handle_);
			handle_ = nullptr;
			break;
		}

		if (FT_SetTimeouts(handle_, timeout, timeout) != FT_OK) {
			setLastError("���ó�ʱʧ��");
			FT4222_UnInitialize(handle_);
			FT_Close(handle_);
			handle_ = nullptr;
			break;
		}

		port_ = port;
		is_open_ = true;
		result = true;
	} while (false);
	return result;
}

bool fbd::Ft4222::close()
{
	bool result = false;
	do
	{
		if (is_open_) {
			FT4222_UnInitialize(handle_);
			FT_Close(handle_);
			handle_ = nullptr;
			is_open_ = false;
			port_ = -1;
		}
		result = true;
	} while (false);
	return result;
}

int fbd::Ft4222::i2cRead(uint8_t* data, uint16_t size, uint16_t slave)
{
	uint16_t result = 0;
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto oldSlave = slave;
	if (slave_addr_bits_ == 0x07) {
		slave >>= 1;
	}

	if (FT4222_I2CMaster_Read(handle_, slave, data, size, &result) != FT_OK) {
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

int fbd::Ft4222::i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave)
{
	uint16_t result = 0;
	if (slave == 0x00) {
		slave = slave_addr_;
	}

	auto oldSlave = slave;
	if (slave_addr_bits_ == 0x07) {
		slave >>= 1;
	}

	if (FT4222_I2CMaster_Write(handle_, slave, const_cast<uint8_t*>(data), size, &result) != FT_OK) {
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
