///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "WXExampleBase.h"

///////////////////////////////////////////////////////////////////////////

WXExampleFrameBase::WXExampleFrameBase( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_MenuBar = new wxMenuBar( 0 );
	m_FileMenuBtn = new wxMenu();
	wxMenuItem* m_FileMenuOpen;
	m_FileMenuOpen = new wxMenuItem( m_FileMenuBtn, wxID_ANY, wxString( wxT("Open Plugin") ) , wxEmptyString, wxITEM_NORMAL );
	m_FileMenuBtn->Append( m_FileMenuOpen );

	m_MenuBar->Append( m_FileMenuBtn, wxT("File") );

	this->SetMenuBar( m_MenuBar );


	this->Centre( wxBOTH );

	// Connect Events
	m_FileMenuBtn->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( WXExampleFrameBase::OnOpenPlugin ), this, m_FileMenuOpen->GetId());
}

WXExampleFrameBase::~WXExampleFrameBase()
{
	// Disconnect Events

}
