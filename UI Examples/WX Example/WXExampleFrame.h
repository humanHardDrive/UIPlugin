#pragma once

#include "WXExampleBase.h"

#include <stack>
#include <string>
#include <map>
#include <functional>

#include "boost/property_tree/ptree.hpp"
#include "boost/dll/import.hpp"

#include "UIPluginBase.hpp"

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

#define LABEL_ELEMENT			"label"

#define PROGRESS_BAR_ELEMENT	"progressmeter"

#define TEXTBOX_ELEMENT			"textbox"

#define SPACER_ELEMENT			"spacer"

class WXExampleFrame : public WXExampleFrameBase
{
public:
	WXExampleFrame(wxWindow* parent);

protected:
	virtual void OnOpenPlugin(wxCommandEvent& event);

private:
	std::stack<wxObject*> m_aPluginObjects;
	std::map<std::string, std::function<bool(boost::property_tree::ptree&, wxSizer*)>> m_ParseFnMap;

	boost::shared_ptr<UIPluginBase> m_pCurrentPlugin;

	bool parsePluginUI(const std::string& sFilePath);

	bool parseWindow(boost::property_tree::ptree& pt);
	bool parseButton(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseMenuList(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseRadioGroup(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseRadioButton(boost::property_tree::ptree& pt, wxSizer* pSizer);

	void handleButtonPress(wxCommandEvent& event);
	void handleComboboxChange(wxCommandEvent& event);
};