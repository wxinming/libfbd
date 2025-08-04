#include "Timer.h"

using namespace fbd;

Timer::Timer()
{
	QueryPerformanceFrequency(&m_large);
	m_freq = static_cast<double>(m_large.QuadPart);
	getStartTime();
}

Timer::~Timer()
{

}

void Timer::getStartTime()
{
	QueryPerformanceCounter(&m_large);
	m_start = m_large.QuadPart;
}

double Timer::getEndTime()
{
	QueryPerformanceCounter(&m_large);
	return static_cast<double>(m_large.QuadPart - m_start) * 1000 / m_freq;
}

void Timer::sleep(double ms)
{
	getStartTime();
	while (getEndTime() < ms);
}
