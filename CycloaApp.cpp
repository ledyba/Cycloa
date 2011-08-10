/***************************************************************
 * Name:      CycloaApp.cpp
 * Purpose:   Code for Application Class
 * Author:    PSI (nasda60@hotmail.com)
 * Created:   2011-08-03
 * Copyright: PSI (http://ledyba.ddo.jp/)
 * License:
 **************************************************************/

#include "CycloaApp.h"

//(*AppHeaders
#include "CycloaMain.h"
#include <wx/image.h>
//*)
#include <iostream>

IMPLEMENT_APP(CycloaApp);

bool CycloaApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	CycloaFrame* Frame = new CycloaFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;
}
