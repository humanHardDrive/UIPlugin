#include "WXExampleFrame.h"

#include "wx/filedlg.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/radiobut.h"
#include "wx/msgdlg.h"

#include <iostream>

#include "boost/property_tree/xml_parser.hpp"
#include "boost/property_tree/json_parser.hpp"

WXExampleFrame::WXExampleFrame(wxWindow * parent) :
	WXExampleFrameBase(parent)
{
	Bind(wxEVT_BUTTON, &WXExampleFrame::handleButtonPress, this, wxID_ANY);

	//Setup element parsing functions
	m_ParseFnMap[BUTTON_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer) { return parseButton(pt, pSizer); };
	m_ParseFnMap[MENULIST_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer) { return parseMenuList(pt, pSizer); };
	m_ParseFnMap[RADIO_GROUP_ELEMENT]	= [this](boost::property_tree::ptree& pt, wxSizer* pSizer) { return parseRadioGroup(pt, pSizer); };
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
		//Create a basic sizer for initial layout
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);

		for (auto& v : pt.get_child("window"))
		{
			std::string sTag = v.first;
			if (sTag == "<xmlattr>") //Window data
			{
				std::string sID = v.second.get("id", "");
				std::string sTitle = v.second.get("title", "");
			}
			else //Any other UI element
			{
				if (m_ParseFnMap.find(sTag) != m_ParseFnMap.end())
					m_ParseFnMap[sTag](v.second, pSizer);
			}
		}

		//This call handles cleaning up any old sizer
		SetSizer(pSizer);
		//Do the layout
		Layout();
		return true;
	}

	return false;
}

bool WXExampleFrame::parseButton(boost::property_tree::ptree & pt, wxSizer* pSizer)
{
	std::string sLabel = pt.get("<xmlattr>.label", "");
	std::string sID = pt.get("<xmlattr>.id", "");
	bool bDisabled = pt.get("<xmlattr>.disabled", false);

	wxButton* pButton = new wxButton(this, wxID_ANY, sLabel);
	pButton->Enable(!bDisabled);
	pButton->SetName(sID);
	Bind(wxEVT_BUTTON, &WXExampleFrame::handleButtonPress, this, pButton->GetId());

	pSizer->Add(pButton, 0, wxALL, 5);
	m_aPluginObjects.push(pButton);

	return true;
}

bool WXExampleFrame::parseMenuList(boost::property_tree::ptree & pt, wxSizer* pSizer)
{
	if (pt.get_child_optional(MENU_POPUP_ELEMENT))
	{
		std::string sLabel = pt.get("<xmlattr>.label", "");
		std::string sID = pt.get("<xmlattr>.id", "");
		bool bDisabled = pt.get("<xmlattr>.disabled", false);
		
		wxComboBox* pComboBox = new wxComboBox(this, wxID_ANY, sLabel);
		pComboBox->Enable(!bDisabled);
		pComboBox->SetName(sID);
		Bind(wxEVT_COMBOBOX, &WXExampleFrame::handleComboboxChange, this, pComboBox->GetId());

		pSizer->Add(pComboBox, 0, wxALL, 5);
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

bool WXExampleFrame::parseRadioGroup(boost::property_tree::ptree & pt, wxSizer* pSizer)
{
	bool bFirst = true;
	for (auto& v : pt.get_child(""))
	{
		if (v.first == RADIO_BUTTON_ELEMENT)
		{
			std::string sLabel = v.second.get("<xmlattr>.label", "");
			bool bDisabled = v.second.get("<xmlattr>.disabled", false);
			bool bSelected = v.second.get("<xmlattr>.selected", false);

			wxRadioButton* pRadioButton = new wxRadioButton(this, wxID_ANY, sLabel, wxDefaultPosition, wxDefaultSize, bFirst ? wxRB_GROUP : 0);
			pRadioButton->Enable(!bDisabled);
			pRadioButton->SetValue(bSelected);

			pSizer->Add(pRadioButton, 0, wxALL, 5);
			m_aPluginObjects.push(pRadioButton);

			bFirst = false;
		}
	}
	return true;
}

void WXExampleFrame::handleButtonPress(wxCommandEvent & event)
{
	boost::property_tree::ptree pt;
	std::stringstream ss;

	wxButton* pButton = static_cast<wxButton*>(FindWindowById(event.GetId()));
	pt.add("event", BUTTON_ELEMENT);
	pt.add("id", pButton->GetName());

	boost::property_tree::write_json(ss, pt);

	if (m_pCurrentPlugin)
		m_pCurrentPlugin->passEvent(ss.str());
}

void WXExampleFrame::handleComboboxChange(wxCommandEvent & event)
{
	boost::property_tree::ptree pt;
	std::stringstream ss;

	wxComboBox* pComboBox = static_cast<wxComboBox*>(FindWindowById(event.GetId()));
	pt.add("event", MENULIST_ELEMENT);
	pt.add("id", pComboBox->GetName());
	pt.add("selection", pComboBox->GetValue());

	boost::property_tree::write_json(ss, pt);

	if (m_pCurrentPlugin)
		m_pCurrentPlugin->passEvent(ss.str());
}
