#pragma once

#include "WXExampleBase.h"

#include <stack>
#include <string>
#include <map>
#include <functional>
#include <tuple>

#include "boost/property_tree/ptree.hpp"
#include "boost/dll/import.hpp"

#include "UIPluginBase.hpp"

class WXExampleFrame : public WXExampleFrameBase
{
public:
	WXExampleFrame(wxWindow* parent);

protected:
	virtual void OnOpenPlugin(wxCommandEvent& event);

private:
	typedef std::tuple<bool, wxSizer*, wxWindow*> parseReturn;

	std::stack<wxObject*> m_aPluginObjects;
	std::map<std::string, std::function<parseReturn(boost::property_tree::ptree&, wxSizer*, wxWindow*)>> m_ParseFnMap;

	boost::shared_ptr<UIPluginBase> m_pCurrentPlugin;

	bool parsePluginUI(const std::string& sFilePath);
	parseReturn parseElements(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);

	parseReturn parseWindow(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);

	parseReturn parseButton(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);

	parseReturn parseMenuList(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);
	parseReturn parseMenuPopUp(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);
	parseReturn parseMenuItem(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);

	parseReturn parseRadioGroup(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);
	parseReturn parseRadioButton(boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement);

	void handleButtonPress(wxCommandEvent& event);
	void handleComboboxChange(wxCommandEvent& event);
};