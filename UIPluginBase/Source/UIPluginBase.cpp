#include "UIPluginBase.hpp"

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
		std::string sElementName, sEventType;
		std::stringstream ss;
		ss << sEvent;

		boost::property_tree::json_parser::read_json(ss, pt);
		sElementName = pt.get("ElementName", "");
		sEventType = pt.get("EventType", "");
		
		if (m_ElementEventMap.find(sElementName) != m_ElementEventMap.end())
		{
			if (m_ElementEventMap[sElementName].find(sEventType) != m_ElementEventMap[sElementName].end())
			{
				m_ElementEventMap[sElementName][sEventType](pt.get_child("Data"));
			}
			else
			{
				BOOST_LOG_TRIVIAL(error) << "Unhandled event " << sEventType;
				return false;
			}
		}
		else
		{
			BOOST_LOG_TRIVIAL(error) << "Unknown element " << sElementName;
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
