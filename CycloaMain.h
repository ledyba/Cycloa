/***************************************************************
 * Name:      CycloaMain.h
 * Purpose:   Defines Application Frame
 * Author:    PSI (nasda60@hotmail.com)
 * Created:   2011-08-03
 * Copyright: PSI (http://ledyba.ddo.jp/)
 * License:
 **************************************************************/

#ifndef CYCLOAMAIN_H
#define CYCLOAMAIN_H

//(*Headers(CycloaFrame)
#include <wx/menu.h>
#include <wx/glcanvas.h>
#include <wx/frame.h>
#include <wx/statusbr.h>
//*)

class CycloaFrame: public wxFrame
{
    public:

        CycloaFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~CycloaFrame();

    private:

        //(*Handlers(CycloaFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(CycloaFrame)
        static const long ID_GLCANVAS1;
        static const long idMenuQuit;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        //(*Declarations(CycloaFrame)
        wxGLCanvas* screenCanvas;
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // CYCLOAMAIN_H
