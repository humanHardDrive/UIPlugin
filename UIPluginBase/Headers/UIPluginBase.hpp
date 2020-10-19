#pragma once

#include "boost/config.hpp"
#include "boost/property_tree/ptree.hpp"

#include <string>
#include <functional>
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

private:
	//map<element_name, map<event_name, event_handler_fn>>
	std::map<std::string, std::map<std::string, std::function<void(boost::property_tree::ptree& pt)>>> m_ElementEventMap;
};