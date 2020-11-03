#pragma once

#include "boost/config.hpp"
#include "boost/property_tree/ptree.hpp"

#include <string>
#include <functional>
#include <utility>
#include <map>
#include <queue>
#include <tuple>

class BOOST_SYMBOL_VISIBLE UIPluginBase
{
public:
	static const int EVENT_TYPE_ID = 0;
	static const int EVENT_SOURCE_ID = 1;
	static const int EVENT_DATA_ID = 2;
	typedef std::tuple<std::string, std::string, std::string> UIevent;

	UIPluginBase();
	~UIPluginBase();

	std::string name() { return m_sPluginName; }

	virtual std::string resourcePath() { return "."; }

	bool passEvent(std::string sEvent);

	void setEventCallback(std::function<void(UIevent)> fn);
	std::string poll();

protected:
	std::string m_sPluginName;

	void setEventHandler(const std::string& sSource, const std::string& sEventType, std::function<void(std::string)> fn);
	void generateEvent(const std::string& sEventType, const std::string& sSource, const std::string& sData);

private:
	//map<pair<event source, event type>, callback with data>
	std::map<std::pair<std::string, std::string>, std::function<void(std::string)>> m_EventMap;

	std::queue<std::string> m_EventQueue;
};