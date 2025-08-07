///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "panel_login_base.h"

///////////////////////////////////////////////////////////////////////////

PANEL_LOGIN_BASE::PANEL_LOGIN_BASE( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name ) : wxPanel( parent, id, pos, size, style, name )
{
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	m_panel1 = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_panel2 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer( wxVERTICAL );

	m_staticText4 = new wxStaticText( m_panel2, wxID_ANY, _("Phone "), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText4->Wrap( -1 );
	bSizer7->Add( m_staticText4, 0, wxALL, 5 );

	m_edit_phone = new wxTextCtrl( m_panel2, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer7->Add( m_edit_phone, 0, wxALL|wxEXPAND, 5 );


	m_panel2->SetSizer( bSizer7 );
	m_panel2->Layout();
	bSizer7->Fit( m_panel2 );
	bSizer6->Add( m_panel2, 1, wxEXPAND | wxALL, 5 );

	m_panel3 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer8;
	bSizer8 = new wxBoxSizer( wxVERTICAL );

	m_staticText5 = new wxStaticText( m_panel3, wxID_ANY, _("Verification Code"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText5->Wrap( -1 );
	bSizer8->Add( m_staticText5, 0, wxALL, 5 );

	wxBoxSizer* bSizer61;
	bSizer61 = new wxBoxSizer( wxHORIZONTAL );

	m_edit_code = new wxTextCtrl( m_panel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_edit_code, 1, wxALL|wxEXPAND, 5 );

	m_btn_get_code = new wxButton( m_panel3, wxID_ANY, _("Get Code"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer61->Add( m_btn_get_code, 0, wxALL, 5 );


	bSizer8->Add( bSizer61, 0, wxEXPAND, 0 );


	m_panel3->SetSizer( bSizer8 );
	m_panel3->Layout();
	bSizer8->Fit( m_panel3 );
	bSizer6->Add( m_panel3, 1, wxEXPAND | wxALL, 5 );

	m_panel4 = new wxPanel( m_panel1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer( wxVERTICAL );

	m_btn_login = new wxButton( m_panel4, wxID_ANY, _("Login"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer9->Add( m_btn_login, 0, wxALL|wxEXPAND, 5 );


	m_panel4->SetSizer( bSizer9 );
	m_panel4->Layout();
	bSizer9->Fit( m_panel4 );
	bSizer6->Add( m_panel4, 1, wxEXPAND | wxALL, 5 );


	m_panel1->SetSizer( bSizer6 );
	m_panel1->Layout();
	bSizer6->Fit( m_panel1 );
	bSizer1->Add( m_panel1, 1, wxEXPAND | wxALL, 24 );


	this->SetSizer( bSizer1 );
	this->Layout();

	// Connect Events
	m_btn_get_code->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_LOGIN_BASE::OnBtnGetCodeClicked ), NULL, this );
	m_btn_login->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( PANEL_LOGIN_BASE::OnLoginClicked ), NULL, this );
}

PANEL_LOGIN_BASE::~PANEL_LOGIN_BASE()
{
}
