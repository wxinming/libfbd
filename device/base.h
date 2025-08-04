#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <map>
#include <format>
#include <functional>
#include <thread>
#include <chrono>
#include "../misc/timer.h"

namespace fbd {
	/*
	* @brief �豸����
	*/
	enum class DeviceType {
		AARDVARK,
		FT4222,
		GINKGO,
		NONE,
	};

	/*
	* @brief ��¼���豸����
	*/
	class LIBFBD_DLL_EXPORT Base
	{
	public:
		/*
		* @brief ����
		* @param[in] slaveAddr �ӵ�ַ
		* @param[in] slaveAddrBits �ӵ�ַλ��(Ŀǰ��֧��7λ,10λ��δ֧��)
		*/
		Base(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);

		/*
		* @brief ����
		*/
		virtual ~Base();

		/*
		* @brief ��������ɾ��
		*/
		Base(const Base&) = delete;

		/*
		* @brief ���ƹ���ɾ��
		*/
		Base& operator=(const Base&) = delete;

		/*
		* @brief ��
		* @param[in] port �˿�
		* @param[in] bitrate ������
		* @param[in] timeout ��ʱ
		* @return bool
		*/
		virtual	bool open(int port, int bitrate = 400, int timeout = 150) = 0;

		/*
		* @brief �ر�
		* @return bool
		*/
		virtual bool close() = 0;

		/*
		* @brief ���ôӵ�ַ
		* @param[in] addr �ӵ�ַ
		*/
		void setSlaveAddr(uint16_t addr);

		/*
		* @brief ��ȡ�ӵ�ַ
		* @return �ӵ�ַ
		*/
		uint16_t getSlaveAddr() const;

		/*
		* @brief ���ôӵ�ַλ��
		* @param[in] bits λ��
		*/
		void setSlaveAddrBits(uint8_t bits);

		/*
		* @brief ��ȡ�ӵ�ַλ��
		* @return λ��
		*/
		uint8_t getSlaveAddrBits() const;

		/*
		* @brief ��ȡ����
		* @param[out] data ��ȡ���ݵĻ�����
		* @param[in] size ��ȡ���ݵĴ�С
		* @param[in] slave �ӵ�ַ
		* @return int
		*/
		virtual int i2cRead(uint8_t* data, uint16_t size, uint16_t slave = 0) = 0;

		/*
		* @brief д������
		* @param[in] data д�����ݵĻ�����
		* @param[in] size д�����ݵĴ�С
		* @param[in] slave �ӵ�ַ
		* @return int
		*/
		virtual int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave = 0) = 0;

		/*
		* @brief i2cд
		* @param[in] data д��������б�
		* @param[in] slave �ӵ�ַ
		* @return ʵ��д��Ĵ�С
		*/
		int i2cWrite(const std::initializer_list<uint8_t>& data, uint16_t slave = 0);

		/*
		* @brief i2c��ַ��ȡ
		* @param[in] addr ��ʼ��ַ
		* @param[in] addrLen ��ַ����
		* @param[out] data ���ݻ�����
		* @param[in] dataSize ��ȡ���ݴ�С
		* @param[in] slave �ӵ�ַ
		* @return bool
		*/
		bool i2cAddrRead(uint32_t addr, uint8_t addrLen, uint8_t* data, uint16_t dataSize, uint16_t slave = 0);

		/*
		* @brief i2c��ַд��
		* @param[in] addr ��ʼ��ַ
		* @param[in] addrLen ��ַ����
		* @param[in] data ���ݻ�����
		* @param[in] dataSize д�����ݴ�С
		* @param[in] check �Ƿ�У��
		* @param[in] slave �ӵ�ַ
		* @return bool
		*/
		bool i2cAddrWrite(uint32_t addr, uint8_t addrLen, const uint8_t* data, uint16_t dataSize, bool check = false, uint16_t slave = 0);

		/*
		* @brief i2c��ַд��
		* @param[in] addr ��ʼ��ַ
		* @param[in] addrLen ��ַ����
		* @param[in] data ��ʼ�������б�
		* @param[in] check �Ƿ�У��
		* @param[in] slave �ӵ�ַ
		* @return bool
		*/
		bool i2cAddrWrite(uint32_t addr, uint8_t addrLen, const std::initializer_list<uint8_t>& data, bool check = false, uint16_t slave = 0);

		/*
		* @brief �Ƿ��
		* @return bool
		*/
		bool isOpen() const;

		/*
		* @brief ��ȡ�˿�
		* @return int
		*/
		int getPort() const;

		/*
		* @brief ��ȡ���մ���
		* @return std::string
		*/
		std::string getLastError() const;

		/*
		* @brief ���ö���ʱ
		* @param[in] delay ��ʱms
		*/
		void setReadDelay(uint32_t delay);

		/*
		* @brief ��ȡ����ʱ
		* @return ��ʱʱ��
		*/
		uint32_t getReadDelay() const;

		/*
		* @brief ����д��ʱ
		* @param[in] delay ��ʱms
		*/
		void setWriteDelay(uint32_t delay);

		/*
		* @brief ��ȡд��ʱ
		* @return ��ʱʱ��
		*/
		uint32_t getWriteDelay() const;

		//��Ϣ�ص�
		std::function<void(const char* action, const uint8_t* data, uint16_t size, uint16_t slave)> onMessage;

	protected:
		/*
		* @brief �������մ���
		* @param[in] fmt ��ʽ���ַ���
		* @param[in] ... �ɱ����
		* @return void
		*/
		void setLastError(const char* fmt, ...);

		/*
		* @brief ����̨���������Ϣ
		* @param[in] type ��־����[R,W]
		* @param[in] data ��־����
		* @param[in] size ���ݴ�С
		* @param[in] slave �ӵ�ַ
		* @return void
		*/
		void outputDebugString(const char* type, const uint8_t* data, uint16_t size, uint16_t slave);

	protected:
		//�Ƿ��
		bool is_open_ = false;

		//�˿�
		int port_ = -1;

		//�ӵ�ַ
		uint16_t slave_addr_;

		//�ӵ�ַλ��
		uint8_t slave_addr_bits_;

		//��ʱ
		struct {
			uint32_t read = 0;
			uint32_t write = 0;
		} delay_;

	private:
		//���մ���
		std::string last_error_ = "δ֪����";

		//��ʱ��
		struct {
			Timer output;
		} timer_;
	};
}
