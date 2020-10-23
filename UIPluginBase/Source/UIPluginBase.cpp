 #include "UIPluginBase.hpp"
#include "Events.h"

#include "boost/property_tree/json_parser.hpp"

#include <boost/log/trivial.hpp>

UIPluginBase::UIPluginBase()
{
}

UIPluginBase::~UIPluginBase()
{
}

bool UIPluginBase::passEvent(std::string sEvent)
{
	boost::property_tree::ptree pt;

	try
	{
		std::pair<std::string, std::string> eventPair;
		std::stringstream ss;
		ss << sEvent;

		boost::property_tree::json_parser::read_json(ss, pt);
		eventPair.first = pt.get(EVENT_SOURCE, "");
		eventPair.second = pt.get(EVENT_TYPE, "");
		
		if (m_EventMap.find(eventPair) != m_EventMap.end())
			m_EventMap[eventPair](pt.get(EVENT_DATA, ""));
		else
		{
			BOOST_LOG_TRIVIAL(error) << "Unhandled event source=" << eventPair.first << " type=" << eventPair.second;
			return false;
		}
	}
	catch (boost::property_tree::ptree_error& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Invalid JSON " << e.what();
		return false;
	}

	return false;
}

void UIPluginBase::setEventCallback(const std::string & sSource, const std::string & sEventType, std::function<void(std::string)> fn)
{
	m_EventMap[std::pair<std::string, std::string>(sSource, sEventType)] = fn;
}
