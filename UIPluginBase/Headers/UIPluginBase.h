#pragma once

#include <thread>
#include <string>
#include <mutex>

class UIPluginBase
{
public:
	UIPluginBase();

protected:
	std::string m_sPluginName;

private:
	std::thread m_EventThread;
	std::mutex m_EventThreadMutex;
	bool m_bEventThreadRunning;
};