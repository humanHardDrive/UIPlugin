#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#define <wx/wx.h>
#endif

#include "WXExampleFrame.h"

class WXExampleApp : public wxApp
{
public:
	virtual bool OnInit();
};
wxIMPLEMENT_APP(WXExampleApp);

bool WXExampleApp::OnInit()
{
	WXExampleFrame* frame = new WXExampleFrame(nullptr);
	frame->Show(true);
	return true;
}