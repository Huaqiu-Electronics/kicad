#include <dialogs/dialog_promote_mall.h>

DIALOG_PROMOTE_MALL::DIALOG_PROMOTE_MALL(wxWindow* aParent, wxWindowID id, const wxString& title, const wxString& settingKey, const wxSize& size, const wxPoint& pos)
                            : DIALOG_WEBVIEW(aParent, id, title, settingKey, size)
                            {
                                SetPosition(pos);
                                Bind(wxEVT_KILL_FOCUS, &DIALOG_PROMOTE_MALL::OnKillFocus, this);
                            }

DIALOG_PROMOTE_MALL::~DIALOG_PROMOTE_MALL()
{

}

void DIALOG_PROMOTE_MALL::LoadUrl( wxString const& aUrl )
{
    GetBrowser().LoadURL( aUrl );
}

void DIALOG_PROMOTE_MALL::onClose(wxCloseEvent& event)
{
    event.Skip();
    Show(false);
}

void DIALOG_PROMOTE_MALL::OnKillFocus(wxFocusEvent& event)
{
    Show(false);
    event.Skip();
}