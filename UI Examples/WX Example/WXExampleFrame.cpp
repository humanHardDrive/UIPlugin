#include "WXExampleFrame.h"

#include "wx/dirdlg.h"
#include "wx/msgdlg.h"
#include "wx/sizer.h"
#include "wx/statbox.h"
#include "wx/button.h"
#include "wx/combobox.h"
#include "wx/radiobut.h"
#include "wx/stattext.h"

#include <iostream>

#include "Elements.h"
#include "Events.h"

#include "boost/property_tree/xml_parser.hpp"
#include "boost/property_tree/json_parser.hpp"

WXExampleFrame::WXExampleFrame(wxWindow * parent) :
	WXExampleFrameBase(parent)
{
	//Setup event mapping
	m_EventEventMap[wxEVT_BUTTON]			= BUTTON_ELEMENT;
	m_EventEventMap[wxEVT_COMBOBOX]			= MENULIST_ELEMENT;
	m_EventEventMap[wxEVT_RADIOBUTTON]		= RADIO_BUTTON_ELEMENT;

	m_EventDataFnMap[wxEVT_BUTTON]			= [this](int ID) { return ""; };
	m_EventDataFnMap[wxEVT_COMBOBOX]		= [this](int ID) {	return std::string(static_cast<wxComboBox*>(FindWindowById(ID))->GetStringSelection().c_str()); };
	m_EventDataFnMap[wxEVT_RADIOBUTTON]		= [this](int ID) { return ""; };

	//Setup element parsing functions
	m_ParseFnMap[WINDOW_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseWindow(pt, pSizer, pElement); };
	
	m_ParseFnMap[BUTTON_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseButton(pt, pSizer, pElement); };

	m_ParseFnMap[MENULIST_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseMenuList(pt, pSizer, pElement); };
	m_ParseFnMap[MENU_POPUP_ELEMENT]	= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseMenuPopUp(pt, pSizer, pElement); };
	m_ParseFnMap[MENU_ITEM_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) {return parseMenuItem(pt, pSizer, pElement); };

	m_ParseFnMap[RADIO_GROUP_ELEMENT]	= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseRadioGroup(pt, pSizer, pElement); };
	m_ParseFnMap[RADIO_BUTTON_ELEMENT]	= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseRadioButton(pt, pSizer, pElement); };

	m_ParseFnMap[SPACER_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseSpacer(pt, pSizer, pElement); };

	m_ParseFnMap[BOX_ELEMENT]			= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseBox(pt, pSizer, pElement); };
	m_ParseFnMap[HBOX_ELEMENT]			= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseHBox(pt, pSizer, pElement); };
	m_ParseFnMap[VBOX_ELEMENT]			= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseVBox(pt, pSizer, pElement); };

	m_ParseFnMap[LABEL_ELEMENT]			= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseLabel(pt, pSizer, pElement); };

	m_ParseFnMap[TEXTBOX_ELEMENT]		= [this](boost::property_tree::ptree& pt, wxSizer* pSizer, wxWindow* pElement) { return parseTextBox(pt, pSizer, pElement); };
}

void WXExampleFrame::OnOpenPlugin(wxCommandEvent & /*event*/)
{
	wxDirDialog dlg(this, _("Choose plugin folder"), "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	if (m_pCurrentPlugin)
		m_pCurrentPlugin.reset();

	//Clear out all of the old UI elements
	while (!m_aPluginObjects.empty())
	{
		delete m_aPluginObjects.top();
		m_aPluginObjects.pop();
	}

	std::string sPluginPath(dlg.GetPath());
	std::string sFileName, sPluginName, sPluginDesc;
	if (getPluginInfo(sPluginPath, sFileName, sPluginName, sPluginDesc))
	{
		try
		{
			m_pCurrentPlugin = boost::dll::import<UIPluginBase>(sPluginPath + std::string("\\") + sFileName, sPluginName);
		}
		catch (boost::system::system_error& e) 
		{
			wxMessageBox(std::string("Error: ") + e.what(), "Plugin Load Error", wxICON_ERROR | wxOK | wxCENTRE);
			return;
		}

		parsePluginUI(sPluginPath + "\\" + m_pCurrentPlugin->resourcePath()); 
	}
}

bool WXExampleFrame::getPluginInfo(std::string & sPluginPath, std::string& sPluginFileName, std::string & sPluginName, std::string & sPluginDesc)
{
	boost::property_tree::ptree pt;
	std::string sInfoPath = sPluginPath + "\\info.xml";

	try
	{
		boost::property_tree::read_xml(sInfoPath, pt);

		sPluginFileName = pt.get<std::string>("Filename", "");
		sPluginName = pt.get<std::string>("Name", "");
		sPluginDesc = pt.get<std::string>("Description", "");
	}
	catch (boost::property_tree::ptree_error& e)
	{
		return false;
	}

	return true;
}

bool WXExampleFrame::parsePluginUI(const std::string & sFilePath)
{
	boost::property_tree::ptree pt;
	WXExampleFrame::parseReturn parseResult;

	try
	{
		boost::property_tree::read_xml(sFilePath, pt);
		parseResult = parseElements(pt, nullptr, nullptr);

		if (std::get<1>(parseResult))
		{
			SetSizer(std::get<1>(parseResult));
			Layout();
		}
	}
	catch (boost::property_tree::ptree_error& e)
	{
		return false;
	}

	return std::get<0>(parseResult);
}

WXExampleFrame::parseReturn WXExampleFrame::parseElements(boost::property_tree::ptree & pt, wxSizer* pSizer, wxWindow* pElement)
{
	if (pt.get_child_optional(""))
	{
		WXExampleFrame::parseReturn parseResult;

		for (auto& v : pt.get_child(""))
		{
			if (m_ParseFnMap.find(v.first) != m_ParseFnMap.end())
			{
				parseResult = m_ParseFnMap[v.first](v.second, pSizer, pElement);
				if(std::get<0>(parseResult))
					parseElements(v.second, std::get<1>(parseResult), std::get<2>(parseResult));
			}
		}

		return std::make_tuple(true, std::get<1>(parseResult), std::get<2>(parseResult));
	}

	return std::make_tuple(false, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseWindow(boost::property_tree::ptree & pt, wxSizer* pSizer, wxWindow* pElement)
{
	std::string sID = pt.get("<xmlattr>.id", "");
	std::string sTitle = pt.get("<xmlattr>.title", "");

	if (sID == "plugin-ui")
	{
		wxBoxSizer* pNewSizer = new wxBoxSizer(wxVERTICAL);
		return std::make_tuple(true, pNewSizer, pElement);
	}

	return std::make_tuple(false, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseButton(boost::property_tree::ptree & pt, wxSizer* pSizer, wxWindow* pElement)
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

	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseMenuList(boost::property_tree::ptree & pt, wxSizer* pSizer, wxWindow* pElement)
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

	return std::make_tuple(true, pSizer, pComboBox);
}

WXExampleFrame::parseReturn WXExampleFrame::parseMenuPopUp(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseMenuItem(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	wxComboBox* pComboBox = static_cast<wxComboBox*>(pElement);
	std::string sChoice = pt.get("<xmlattr>.label", "");
	bool bSelected = pt.get("<xmlattr>.selected", false);

	int nCount = pComboBox->Append(sChoice);

	if (bSelected)
		pComboBox->SetSelection(nCount);

	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseRadioGroup(boost::property_tree::ptree & pt, wxSizer* pSizer, wxWindow* pElement)
{
	std::string sLabel = pt.get("<xmlattr>.label", "");

	wxStaticBoxSizer* pStaticSizer = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, sLabel), wxVERTICAL);
	pSizer->Add(pStaticSizer, 0, 0, 5);

	return std::make_tuple(true, pStaticSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseRadioButton(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	std::string sLabel = pt.get("<xmlattr>.label", "");
	std::string sID = pt.get("<xmlattr>.id", "");
	bool bDisabled = pt.get("<xmlattr>.disabled", false);
	bool bSelected = pt.get("<xmlattr>.selected", false);

	wxRadioButton* pRadioButton = new wxRadioButton(this, wxID_ANY, sLabel);
	pRadioButton->SetName(sID);
	pRadioButton->Enable(!bDisabled);
	pRadioButton->SetValue(bSelected);
	Bind(wxEVT_RADIOBUTTON, &WXExampleFrame::handleRadioButton, this, pRadioButton->GetId());

	pSizer->Add(pRadioButton, 0, wxALL, 5);
	m_aPluginObjects.push(pRadioButton);

	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseSpacer(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	pSizer->Add(0, 0, 1, wxEXPAND, 5);
	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseBox(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	std::string sOrientation = pt.get("<xmlattr>.orient", "vertical");

	if (sOrientation == "vertical")
		return parseVBox(pt, pSizer, pElement);
	else if(sOrientation == "horizontal")
		return parseHBox(pt, pSizer, pElement);

	return std::make_tuple(false, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseHBox(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	wxBoxSizer* pBoxSizer = new wxBoxSizer(wxHORIZONTAL);
	pSizer->Add(pBoxSizer);

	return std::make_tuple(true, pBoxSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseVBox(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	wxBoxSizer* pBoxSizer = new wxBoxSizer(wxVERTICAL);
	pSizer->Add(pBoxSizer);

	return std::make_tuple(true, pBoxSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseLabel(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	std::string sLabel = pt.get("<xmlattr>.value", "");
	bool bDisabled = pt.get("<xmlattr>.disabled", false);

	wxStaticText* pStaticText = new wxStaticText(this, wxID_ANY, sLabel);
	pStaticText->Enable(!bDisabled);

	pSizer->Add(pStaticText);
	m_aPluginObjects.push(pStaticText);

	return std::make_tuple(true, pSizer, pElement);
}

WXExampleFrame::parseReturn WXExampleFrame::parseTextBox(boost::property_tree::ptree & pt, wxSizer * pSizer, wxWindow * pElement)
{
	std::string sValue = pt.get("<xmlattr>.value", "");
	std::string sID = pt.get("<xmlattr>.id", "");
	bool bDisabled = pt.get("<xmlattr>.disabled", false);

	wxTextCtrl* pTextBox = new wxTextCtrl(this, wxID_ANY, sValue);
	pTextBox->Enable(!bDisabled);
	pTextBox->SetName(sID);

	pSizer->Add(pTextBox);
	m_aPluginObjects.push(pTextBox);

	return std::make_tuple(true, pSizer, pElement);
}

void WXExampleFrame::handleButtonPress(wxCommandEvent & event)
{
	boost::property_tree::ptree pt;
	std::stringstream ss;

	wxButton* pButton = static_cast<wxButton*>(FindWindowById(event.GetId()));
	pt.add(EVENT_TYPE, BUTTON_ELEMENT);
	pt.add(EVENT_SOURCE, pButton->GetName());
	pt.add(EVENT_DATA, "");

	boost::property_tree::write_json(ss, pt);

	if (m_pCurrentPlugin)
		m_pCurrentPlugin->passEvent(ss.str());
}

void WXExampleFrame::handleComboboxChange(wxCommandEvent & event)
{
	boost::property_tree::ptree pt;
	std::stringstream ss;

	wxComboBox* pComboBox = static_cast<wxComboBox*>(FindWindowById(event.GetId()));
	pt.add(EVENT_TYPE, MENULIST_ELEMENT);
	pt.add(EVENT_SOURCE, pComboBox->GetName());
	pt.add(EVENT_DATA, pComboBox->GetValue());

	boost::property_tree::write_json(ss, pt);

	if (m_pCurrentPlugin)
		m_pCurrentPlugin->passEvent(ss.str());
}

void WXExampleFrame::handleRadioButton(wxCommandEvent & event)
{
	boost::property_tree::ptree pt;
	std::stringstream ss;

	wxRadioButton* pRadioButton = static_cast<wxRadioButton*>(FindWindowById(event.GetId()));
	pt.add(EVENT_TYPE, RADIO_BUTTON_ELEMENT);
	pt.add(EVENT_SOURCE, pRadioButton->GetName());
	pt.add(EVENT_DATA, "");

	boost::property_tree::write_json(ss, pt);

	if (m_pCurrentPlugin)
		m_pCurrentPlugin->passEvent(ss.str());
}
