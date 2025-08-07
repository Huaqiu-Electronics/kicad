///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version 4.2.1-0-g80c4cb6)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#pragma once

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/intl.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class PANEL_LOGIN_BASE
///////////////////////////////////////////////////////////////////////////////
class PANEL_LOGIN_BASE : public wxPanel
{
	private:

	protected:
		wxPanel* m_panel1;
		wxPanel* m_panel2;
		wxStaticText* m_staticText4;
		wxTextCtrl* m_edit_phone;
		wxPanel* m_panel3;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_edit_code;
		wxButton* m_btn_get_code;
		wxPanel* m_panel4;
		wxButton* m_btn_login;

		// Virtual event handlers, override them in your derived class
		virtual void OnBtnGetCodeClicked( wxCommandEvent& event ) { event.Skip(); }
		virtual void OnLoginClicked( wxCommandEvent& event ) { event.Skip(); }


	public:

		PANEL_LOGIN_BASE( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 302,373 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString );

		~PANEL_LOGIN_BASE();

};

