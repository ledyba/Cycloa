/***************************************************************
 * Name:      CycloaMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    PSI (nasda60@hotmail.com)
 * Created:   2011-08-03
 * Copyright: PSI (http://ledyba.ddo.jp/)
 * License:
 **************************************************************/

#include "CycloaMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(CycloaFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(CycloaFrame)
const long CycloaFrame::ID_GLCANVAS1 = wxNewId();
const long CycloaFrame::idMenuQuit = wxNewId();
const long CycloaFrame::idMenuAbout = wxNewId();
const long CycloaFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(CycloaFrame,wxFrame)
    //(*EventTable(CycloaFrame)
    //*)
END_EVENT_TABLE()

CycloaFrame::CycloaFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(CycloaFrame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenuBar* MenuBar1;
    wxMenu* Menu2;
    
    Create(parent, id, _("Cycloa"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("id"));
    int GLCanvasAttributes_1[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    screenCanvas = new wxGLCanvas(this, ID_GLCANVAS1, wxPoint(64,336), wxDefaultSize, 0, _T("ID_GLCANVAS1"), GLCanvasAttributes_1);
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);
    
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CycloaFrame::OnQuit);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&CycloaFrame::OnAbout);
    //*)
}

CycloaFrame::~CycloaFrame()
{
    //(*Destroy(CycloaFrame)
    //*)
}

void CycloaFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void CycloaFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}
