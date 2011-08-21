#include <wx/wx.h>
#include <wx/glcanvas.h>
#include "EmulatorPanel.h"

wxDEFINE_EVENT(EMULATOR_PANEL_EVENT, EmulatorPanelEvent);

EmulatorPanel::EmulatorPanel(wxWindow *parent,
               wxWindowID id,
               const int *attribList,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxString& name,
               const wxPalette& palette):
wxGLCanvas (parent, id, attribList, pos, size, style, name, palette),
ctx(this),
initialized(false),
imageLoaded(false)
{
    initCanvas();
}
EmulatorPanel::EmulatorPanel(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style,
               const wxString& name,
               const int *attribList,
               const wxPalette& palette):
wxGLCanvas (parent, id, attribList, pos, size, style, name, palette),
ctx(this),
initialized(false),
imageLoaded(false)
{
    initCanvas();
}

void EmulatorPanel::initCanvas(){
    Bind(EMULATOR_PANEL_EVENT, &EmulatorPanel::OnEvent, this);
    Bind(wxEVT_SIZE, &EmulatorPanel::OnSize, this);
    Bind(wxEVT_PAINT, &EmulatorPanel::OnPaint, this);
    Bind(wxEVT_ERASE_BACKGROUND, &EmulatorPanel::OnEraseBackground, this);
}

void EmulatorPanel::initialize()
{
    if(!initialized){
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);
        glMatrixMode(GL_PROJECTION);
        initialized = true;
    }
    glLoadIdentity();
    GetClientSize(&this->screenWidth, &this->screenHeight);
    glViewport(0, 0, (GLint)this->screenWidth, (GLint)this->screenHeight);
    glOrtho(0, (GLint)this->screenWidth, (GLint)this->screenHeight, 0, -1, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

EmulatorPanel::~EmulatorPanel()
{
    if(imageLoaded){
        this->SetCurrent(ctx);
        glDeleteTextures(1,&this->imageTexture);
    }
}

void EmulatorPanel::OnEvent(EmulatorPanelEvent &event)
{
    switch(event.getType()){
        default:
            throw "invalid";
    }
}


void EmulatorPanel::OnPaint(wxPaintEvent &event)
{
	/*
    if(!GetParent()->IsShown()){
        return;
    }
    //wxPaintDC dc(this);
    render();
    */
}


void EmulatorPanel::OnSize(wxSizeEvent &event)
{
	/*
    if(!GetParent()->IsShown()){
        return;
    }
    render();
    */
}
void EmulatorPanel::OnEraseBackground(wxEraseEvent& event)
{
    //何もしない
}

void EmulatorPanel::update(const uint8_t* image, const int aw, const int ah,const int w, const int h, const int bytesPerPixel)
{
    this->imageSurfaceWidth = w;
    this->imageSurfaceHeight = h;
    this->imageWidth = aw;
    this->imageHeight = ah;
    this->SetCurrent(ctx);
    if(!imageLoaded){
        glGenTextures(1, &this->imageTexture);
        imageLoaded = true;
    }
    glBindTexture(GL_TEXTURE_2D, this->imageTexture);
    glTexImage2D(
				GL_TEXTURE_2D, 0, bytesPerPixel, w, h,
				0, GL_RGB ,GL_UNSIGNED_BYTE , image
				);
    render();
}

void EmulatorPanel::render()
{
    this->SetCurrent(ctx);
    initialize();

    if(!imageLoaded){
        glFlush();
        SwapBuffers();
        return;
    }

    int shownWidth;
    int shownHeight;
    if((float)this->screenWidth/this->imageWidth > (float)this->screenHeight/this->imageHeight){
        shownWidth = this->imageWidth * this->screenHeight / this->imageHeight;
        shownHeight = this->screenHeight;
    }else{
        shownWidth = this->screenWidth;
        shownHeight = this->imageHeight * this->screenWidth / this->imageWidth;
    }
    const int shownOffsetX = (this->screenWidth - shownWidth)/2;
    const int shownOffsetY = (this->screenHeight - shownHeight)/2;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, this->imageTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0f, 0.0f);
            glVertex2f(shownOffsetX, shownOffsetY);
		glTexCoord2f(0.0f, (float)imageHeight / imageSurfaceHeight);
            glVertex2f(shownOffsetX,shownOffsetY+shownHeight);
		glTexCoord2f((float)imageWidth/imageSurfaceWidth, (float)imageHeight / imageSurfaceHeight);
            glVertex2f(shownOffsetX + shownWidth, shownOffsetY+shownHeight);
		glTexCoord2f((float)imageWidth/imageSurfaceWidth, 0);
            glVertex2f(shownOffsetX + shownWidth, shownOffsetY);
	glEnd();

    glFlush();
    SwapBuffers();
}

//--------------------------------------------------------------------------------------

EmulatorPanelEvent::EmulatorPanelEvent(enum EventType type, int winid):
    wxEvent(winid, EMULATOR_PANEL_EVENT),
    type(type)
{

}

EmulatorPanelEvent::~EmulatorPanelEvent()
{
}

EmulatorPanelEvent::EmulatorPanelEvent(const EmulatorPanelEvent& event) :
    wxEvent(event),
    type(event.type)
{
}

EmulatorPanelEvent::EventType EmulatorPanelEvent::getType() const
{
    return this->type;
}

wxEvent* EmulatorPanelEvent::Clone() const
{
    return new EmulatorPanelEvent(*this);
}
