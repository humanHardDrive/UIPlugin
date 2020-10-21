#pragma once

#include "WXExampleBase.h"

#include <stack>
#include <string>
#include <map>
#include <functional>

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
	std::stack<wxObject*> m_aPluginObjects;
	std::map<std::string, std::function<bool(boost::property_tree::ptree&, wxSizer*)>> m_ParseFnMap;

	boost::shared_ptr<UIPluginBase> m_pCurrentPlugin;

	bool parsePluginUI(const std::string& sFilePath);
	bool parseElements(boost::property_tree::ptree& pt, wxSizer* pSizer);

	bool parseWindow(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseButton(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseMenuList(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseRadioGroup(boost::property_tree::ptree& pt, wxSizer* pSizer);
	bool parseRadioButton(boost::property_tree::ptree& pt, wxSizer* pSizer);

	void handleButtonPress(wxCommandEvent& event);
	void handleComboboxChange(wxCommandEvent& event);
};