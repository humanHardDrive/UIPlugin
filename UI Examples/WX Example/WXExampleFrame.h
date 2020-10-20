#pragma once

#include "WXExampleBase.h"
#include "wx/sizer.h"

#include <stack>
#include <string>
#include <map>
#include <functional>

#include "boost/property_tree/ptree.hpp"

/*https://developer.mozilla.org/en-US/docs/Archive/Mozilla/XUL*/

#define WINDOW_ELEMENT			"window"
#define BUTTON_ELEMENT			"button"
#define MENULIST_ELEMENT		"menulist"
#define RADIO_GROUP_ELEMENT		"radiogroup"
#define RADIO_BUTTON_ELEMENT	"radio"

class WXExampleFrame : public WXExampleFrameBase
{
public:
	WXExampleFrame(wxWindow* parent);

protected:
	virtual void OnOpenPlugin(wxCommandEvent& event);

private:
	std::stack<wxObject*> m_aPluginObjects;
	std::map<std::string, std::function<bool(boost::property_tree::ptree&)>> m_ParseFnMap;

	wxBoxSizer* m_BasicSizer;

	bool parsePluginUI(const std::string& sFilePath);

	bool parseWindow(boost::property_tree::ptree& pt);
	bool parseButton(boost::property_tree::ptree& pt);
	bool parseMenuList(boost::property_tree::ptree& pt);
	bool parseRadioGroup(boost::property_tree::ptree& pt);
};