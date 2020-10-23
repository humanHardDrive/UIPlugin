#pragma once

#include "boost/config.hpp"
#include "boost/property_tree/ptree.hpp"

#include <string>
#include <functional>
#include <utility>
#include <map>

class BOOST_SYMBOL_VISIBLE UIPluginBase
{
public:
	UIPluginBase();
	~UIPluginBase();

	std::string name() { return m_sPluginName; }

	virtual std::string resourcePath() { return "."; }

	bool passEvent(std::string sEvent);

protected:
	std::string m_sPluginName;

	void setEventCallback(const std::string& sSource, const std::string& sEventType, std::function<void(std::string)> fn);

private:
	//map<pair<event source, event type>, callback with data>
	std::map<std::pair<std::string, std::string>, std::function<void(std::string)>> m_EventMap;
};