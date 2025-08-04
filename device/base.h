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
	* @brief 设备类型
	*/
	enum class DeviceType {
		AARDVARK,
		FT4222,
		GINKGO,
		NONE,
	};

	/*
	* @brief 烧录器设备基类
	*/
	class LIBFBD_DLL_EXPORT Base
	{
	public:
		/*
		* @brief 构造
		* @param[in] slaveAddr 从地址
		* @param[in] slaveAddrBits 从地址位数(目前仅支持7位,10位尚未支持)
		*/
		Base(uint16_t slaveAddr = 0, uint8_t slaveAddrBits = 7);

		/*
		* @brief 析构
		*/
		virtual ~Base();

		/*
		* @brief 拷贝构造删除
		*/
		Base(const Base&) = delete;

		/*
		* @brief 复制构造删除
		*/
		Base& operator=(const Base&) = delete;

		/*
		* @brief 打开
		* @param[in] port 端口
		* @param[in] bitrate 比特率
		* @param[in] timeout 超时
		* @return bool
		*/
		virtual	bool open(int port, int bitrate = 400, int timeout = 150) = 0;

		/*
		* @brief 关闭
		* @return bool
		*/
		virtual bool close() = 0;

		/*
		* @brief 设置从地址
		* @param[in] addr 从地址
		*/
		void setSlaveAddr(uint16_t addr);

		/*
		* @brief 获取从地址
		* @return 从地址
		*/
		uint16_t getSlaveAddr() const;

		/*
		* @brief 设置从地址位数
		* @param[in] bits 位数
		*/
		void setSlaveAddrBits(uint8_t bits);

		/*
		* @brief 获取从地址位数
		* @return 位数
		*/
		uint8_t getSlaveAddrBits() const;

		/*
		* @brief 读取数据
		* @param[out] data 读取数据的缓存区
		* @param[in] size 读取数据的大小
		* @param[in] slave 从地址
		* @return int
		*/
		virtual int i2cRead(uint8_t* data, uint16_t size, uint16_t slave = 0) = 0;

		/*
		* @brief 写入数据
		* @param[in] data 写入数据的缓存区
		* @param[in] size 写入数据的大小
		* @param[in] slave 从地址
		* @return int
		*/
		virtual int i2cWrite(const uint8_t* data, uint16_t size, uint16_t slave = 0) = 0;

		/*
		* @brief i2c写
		* @param[in] data 写入的数据列表
		* @param[in] slave 从地址
		* @return 实际写入的大小
		*/
		int i2cWrite(const std::initializer_list<uint8_t>& data, uint16_t slave = 0);

		/*
		* @brief i2c地址读取
		* @param[in] addr 起始地址
		* @param[in] addrLen 地址长度
		* @param[out] data 数据缓存区
		* @param[in] dataSize 读取数据大小
		* @param[in] slave 从地址
		* @return bool
		*/
		bool i2cAddrRead(uint32_t addr, uint8_t addrLen, uint8_t* data, uint16_t dataSize, uint16_t slave = 0);

		/*
		* @brief i2c地址写入
		* @param[in] addr 起始地址
		* @param[in] addrLen 地址长度
		* @param[in] data 数据缓存区
		* @param[in] dataSize 写入数据大小
		* @param[in] check 是否校验
		* @param[in] slave 从地址
		* @return bool
		*/
		bool i2cAddrWrite(uint32_t addr, uint8_t addrLen, const uint8_t* data, uint16_t dataSize, bool check = false, uint16_t slave = 0);

		/*
		* @brief i2c地址写入
		* @param[in] addr 起始地址
		* @param[in] addrLen 地址长度
		* @param[in] data 初始化数据列表
		* @param[in] check 是否校验
		* @param[in] slave 从地址
		* @return bool
		*/
		bool i2cAddrWrite(uint32_t addr, uint8_t addrLen, const std::initializer_list<uint8_t>& data, bool check = false, uint16_t slave = 0);

		/*
		* @brief 是否打开
		* @return bool
		*/
		bool isOpen() const;

		/*
		* @brief 获取端口
		* @return int
		*/
		int getPort() const;

		/*
		* @brief 获取最终错误
		* @return std::string
		*/
		std::string getLastError() const;

		/*
		* @brief 设置读延时
		* @param[in] delay 延时ms
		*/
		void setReadDelay(uint32_t delay);

		/*
		* @brief 获取读延时
		* @return 延时时间
		*/
		uint32_t getReadDelay() const;

		/*
		* @brief 设置写延时
		* @param[in] delay 延时ms
		*/
		void setWriteDelay(uint32_t delay);

		/*
		* @brief 获取写延时
		* @return 延时时间
		*/
		uint32_t getWriteDelay() const;

		//消息回调
		std::function<void(const char* action, const uint8_t* data, uint16_t size, uint16_t slave)> onMessage;

	protected:
		/*
		* @brief 设置最终错误
		* @param[in] fmt 格式化字符串
		* @param[in] ... 可变参数
		* @return void
		*/
		void setLastError(const char* fmt, ...);

		/*
		* @brief 控制台输出调试信息
		* @param[in] type 日志类型[R,W]
		* @param[in] data 日志数据
		* @param[in] size 数据大小
		* @param[in] slave 从地址
		* @return void
		*/
		void outputDebugString(const char* type, const uint8_t* data, uint16_t size, uint16_t slave);

	protected:
		//是否打开
		bool is_open_ = false;

		//端口
		int port_ = -1;

		//从地址
		uint16_t slave_addr_;

		//从地址位数
		uint8_t slave_addr_bits_;

		//延时
		struct {
			uint32_t read = 0;
			uint32_t write = 0;
		} delay_;

	private:
		//最终错误
		std::string last_error_ = "未知错误";

		//计时器
		struct {
			Timer output;
		} timer_;
	};
}
