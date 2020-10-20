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
#define MENU_POPUP_ELEMENT		"menupopup"
#define MENU_ITEM_ELEMENT		"menuitem"

#define RADIO_GROUP_ELEMENT		"radiogroup"
#define RADIO_BUTTON_ELEMENT	"radio"

#define GRID_ELEMENT			"grid"
#define COLUMN_ELEMENT			"column"
#define ROW_ELEMENT				"row"

class WXExampleFrame : public WXExampleFrameBase
{
public:
	WXExampleFrame(wxWindow* parent);

protected:
	virtual void OnOpenPlugin(wxCommandEvent& event);

private:
	std::stack<wxObject*> m_aPluginObjects;
	std::map<std::string, std::function<bool(boost::property_tree::ptree&, wxSizer*)>> m_ParseFnMap;

	bool parsePluginUI(const std::string& sFilePath);

	bool parseWindow(boost::property_tree::ptree& pt);
	bool parseButton(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseMenuList(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseRadioGroup(boost::property_tree::ptree& pt, wxSizer* pSizer);
};