#include "panel_login.h"
#include <wx/msgdlg.h>
#include <wx/timer.h>
#include <nlohmann/json.hpp>
#include <utils/httplib_wrapper.h>
#include <regex>

constexpr auto AWSC_URL = "https://passport.elecfans.com";

#if !defined( IS_RC_BUILD )
constexpr auto API_BASE_URL = "https://www.eda.cn";
#else
constexpr auto API_BASE_URL = "http://47.116.118.182";
#endif

// FIXME : Cannot by pass Alibaba Cloud’s sliding verification
constexpr auto kDefaultForm = R"(
{
    "sig": "05a1C7nT4bR5hcbZlAujcdyUhL5f0xTnwuXsBOhw4p3yrDJrozWRmx3N11iL1QTZdmFtf04wPqHwuQAVHKkLxoXqxp9seTh6Gp36wKRzRt9sdiGp4-RSlo2wwzH2kqN58cemMv1O1Tl8TPVuavnRjknbOI5pVSNOjD8Xf8qHgkVHhsi0TvSHDNkVSeYMyD7vJGBmgsoc78Zs-9K-Twvn2Kys6mkxK0-d2rv6v5MtV9uKufmHLdzTzNOfb7RP9uEYRL6bX8cJCHtTkg0zHpy0KNkQjtxCNbsXsJnYpjgWVD3aURGjoSGtT4TQdoNBevxCuQPRkVLWbPjPgc_kP0-Qd1BAsCh2wx9tO-BMfrowW7sF5K2AsK82rrvcRlU7hnOXU93-F18EHCtNdce7G32HI2aA",
    "sessionId": "0152JIZgtMjy7iQLwB8JakWVRO_6q2wsVkoGxcokKqJGZZwM6LwVqkHOr4zXdTIRhZ4bIttMqbB2GB3a_oIpaiIKzc4bFLhdPOXkW8hsASX1l_qkDqYXD0zAl-Bh-46HvlQwttU8q4L5aX6wAjaDDj6Q",
    "token": "FFFF0000000001784D08:1755236418915:0.6512899670250883",
    "scene": "quick",
    "csessionid": "0152JIZgtMjy7iQLwB8JakWVRO_6q2wsVkoGxcokKqJGZZwM6LwVqkHOr4zXdTIRhZ4bIttMqbB2GB3a_oIpaiIKzc4bFLhdPOXkW8hsASX1l_qkDqYXD0zAl-Bh-46HvlQwttU8q4L5aX6wAjaDDj6Q",
    "aliscene": "login",
    "siteid": 35,
    "account": "17317499234",
    "smscode": "",
    "areacode": "86"
}
)";


class CountdownTimer : public wxTimer
{
public:
    CountdownTimer( wxButton* btn, int* counter ) : m_btn( btn ), m_counter( counter ) {}

    void Notify() override
    {
        if( *m_counter > 0 )
        {
            ( *m_counter )--;
            m_btn->SetLabel( wxString::Format( "%d秒后重试", *m_counter ) );
        }
        else
        {
            Stop();
            m_btn->Enable( true );
            m_btn->SetLabel( "获取验证码" );
        }
    }

private:
    wxButton* m_btn;
    int*      m_counter;
};

PANEL_LOGIN::PANEL_LOGIN( wxWindow* parent ) : PANEL_LOGIN_BASE( parent ), m_countdown( 0 ), m_timer( nullptr )
{
}

PANEL_LOGIN::~PANEL_LOGIN()
{
    if( m_timer )
        delete m_timer;
}

bool PANEL_LOGIN::ValidatePhone( const wxString& phone )
{
    std::regex reg( "^1[3-9]\\d{9}$" );
    return std::regex_match( std::string( phone.mb_str() ), reg );
}

void PANEL_LOGIN::OnBtnGetCodeClicked( wxCommandEvent& event )
{
    wxString phone = m_edit_phone->GetValue().Trim();
    if( !ValidatePhone( phone ) )
    {
        wxMessageBox( "请输入正确的手机号", "错误", wxICON_ERROR );
        return;
    }

    m_btn_get_code->Enable( false );
    m_countdown = 60;
    m_btn_get_code->SetLabel( wxString::Format( "%d秒后重试", m_countdown ) );

    if( !m_timer )
        m_timer = new CountdownTimer( m_btn_get_code, &m_countdown );

    m_timer->Start( 1000 ); // 1-second interval

    try
    {
        httplib::SSLClient cli( AWSC_URL, 443 );
        nlohmann::json     obj{ nlohmann::json::parse( kDefaultForm ) };
        obj["account"] = phone.ToStdString();

        httplib::Headers headers = { { "Content-Type", "application/json" } };
        auto             res = cli.Post( "/register/regsms", headers, obj.dump(), "application/json" );

        if( !res || res->status != 200 )
        {
            wxQueueEvent( this, new wxCommandEvent( wxEVT_BUTTON, m_btn_get_code->GetId() ) );
            wxMessageBox( "验证码请求失败", "错误", wxICON_ERROR );
            return;
        }

        auto body = res->body;
        auto jsonRes = nlohmann::json::parse( body, nullptr, false );
        if( jsonRes.is_discarded() || jsonRes["error_code"].get<int>() > 0 )
        {
            wxMessageBox( wxString::FromUTF8( jsonRes.value( "msg", "未知错误" ) ), "错误", wxICON_ERROR );
        }
    }
    catch( const std::exception& e )
    {
        wxMessageBox( wxString::Format( "请求异常: %s", e.what() ), "错误", wxICON_ERROR );
    }
}

void PANEL_LOGIN::OnLoginClicked( wxCommandEvent& event )
{
    wxString phone = m_edit_phone->GetValue().Trim();
    wxString code = m_edit_code->GetValue().Trim();

    if( !ValidatePhone( phone ) )
    {
        wxMessageBox( "请输入正确的手机号", "错误", wxICON_ERROR );
        return;
    }
    if( code.Length() != 6 )
    {
        wxMessageBox( "验证码应为6位数字", "错误", wxICON_ERROR );
        return;
    }

    m_btn_login->Enable( false );
    m_btn_login->SetLabel( "登录中..." );

    try
    {
#if !defined( IS_RC_BUILD )
        httplib::SSLClient cli( API_BASE_URL, 443 );
#else
        httplib::Client cli( API_BASE_URL );
#endif


        cli.set_connection_timeout( 5, 0 );

        httplib::MultipartFormDataItems items = { { "csessionid", "" },
                                                  { "aliscene", "login" },
                                                  { "siteid", "35" },
                                                  { "account", std::string( phone.mb_str() ) },
                                                  { "smscode", std::string( code.mb_str() ) },
                                                  { "areacode", "86" },
                                                  { "referer", "http://localhost" } };

        auto res = cli.Post( "/api/chiplet/u/login", items );

        if( !res || res->status != 200 )
        {
            wxMessageBox( "登录请求失败", "错误", wxICON_ERROR );
            m_btn_login->Enable( true );
            m_btn_login->SetLabel( "登录" );
            return;
        }

        auto jsonRes = nlohmann::json::parse( res->body, nullptr, false );
        if( jsonRes.is_discarded() || jsonRes["error_code"] != "0" )
        {
            wxMessageBox( wxString::FromUTF8( jsonRes.value( "msg", "登录失败" ) ), "错误", wxICON_ERROR );
            m_btn_login->Enable( true );
            m_btn_login->SetLabel( "登录" );
            return;
        }

        wxMessageBox( "登录成功!", "提示", wxICON_INFORMATION );
        m_btn_login->Enable( true );
        m_btn_login->SetLabel( "登录" );
    }
    catch( const std::exception& e )
    {
        wxMessageBox( wxString::Format( "请求异常: %s", e.what() ), "错误", wxICON_ERROR );
        m_btn_login->Enable( true );
        m_btn_login->SetLabel( "登录" );
    }
}
