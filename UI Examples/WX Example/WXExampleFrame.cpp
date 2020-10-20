#include "WXExampleFrame.h"

#include "wx/filedlg.h"
#include "wx/button.h"
#include "wx/combobox.h"

#include <iostream>

#include "boost/property_tree/xml_parser.hpp"

WXExampleFrame::WXExampleFrame(wxWindow * parent) :
	WXExampleFrameBase(parent)
{
	//Setup element parsing functions
	m_ParseFnMap[BUTTON_ELEMENT]		= [this](boost::property_tree::ptree& pt) { return parseButton(pt); };
	m_ParseFnMap[MENULIST_ELEMENT]		= [this](boost::property_tree::ptree& pt) { return parseMenuList(pt); };
	m_ParseFnMap[RADIO_GROUP_ELEMENT]	= [this](boost::property_tree::ptree& pt) { return parseRadioGroup(pt); };

	//Create the basic sizer needed for WX widgets
	m_BasicSizer = new wxBoxSizer(wxVERTICAL);
	SetSizer(m_BasicSizer);
}

void WXExampleFrame::OnOpenPlugin(wxCommandEvent & /*event*/)
{
	wxFileDialog dlg(this, _("Open File"), "", "", "XUL File (*.xml)|*.xml", wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	//Clear out all of the old UI elements
	while (!m_aPluginObjects.empty())
	{
		delete m_aPluginObjects.top();
		m_aPluginObjects.pop();
	}

	parsePluginUI(std::string(dlg.GetPath()));
}

bool WXExampleFrame::parsePluginUI(const std::string & sFilePath)
{
	boost::property_tree::ptree pt;

	try
	{
		boost::property_tree::read_xml(sFilePath, pt);
		return parseWindow(pt);
	}
	catch (boost::property_tree::ptree_error& e)
	{
		return false;
	}

	return true;
}

bool WXExampleFrame::parseWindow(boost::property_tree::ptree & pt)
{
	if (pt.get_child_optional("window"))
	{
		for (auto& v : pt.get_child("window"))
		{
			std::string sTag = v.first;
			if (sTag == "<xmlattr>")
			{
				std::string sID = v.second.get("id", "");
				std::string sTitle = v.second.get("title", "");
			}
			else
			{
				if (m_ParseFnMap.find(sTag) != m_ParseFnMap.end())
					m_ParseFnMap[sTag](v.second);
			}
		}

		Layout();
		return true;
	}

	return false;
}

bool WXExampleFrame::parseButton(boost::property_tree::ptree & pt)
{
	std::string sLabel = pt.get("<xmlattr>.label", "");
	bool bDisabled = pt.get("<xmlattr>.disabled", false);

	wxButton* pButton = new wxButton(this, wxID_ANY, sLabel);
	pButton->Enable(!bDisabled);

	m_BasicSizer->Add(pButton, 0, wxALL, 5);
	m_aPluginObjects.push(pButton);

	return true;
}

bool WXExampleFrame::parseMenuList(boost::property_tree::ptree & pt)
{
	if (pt.get_child_optional(MENU_POPUP_ELEMENT))
	{
		std::string sLabel = pt.get("<xmlattr>.label", "");
		bool bDisabled = pt.get("<xmlattr>.disabled", false);
		
		wxComboBox* pComboBox = new wxComboBox(this, wxID_ANY, sLabel);
		pComboBox->Enable(!bDisabled);

		m_BasicSizer->Add(pComboBox, 0, wxALL, 5);
		m_aPluginObjects.push(pComboBox);

		for (auto& v : pt.get_child(MENU_POPUP_ELEMENT))
		{
			if (v.first == MENU_ITEM_ELEMENT)
			{
				std::string sChoice = v.second.get("<xmlattr>.label", "");
				bool bSelected = v.second.get("<xmlattr>.selected", false);

				int nCount = pComboBox->Append(sChoice);

				if(bSelected)
					pComboBox->SetSelection(nCount);
			}
		}
	}

	return true;
}

bool WXExampleFrame::parseRadioGroup(boost::property_tree::ptree & pt)
{
	return false;
}
