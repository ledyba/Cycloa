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
        case EmulatorPanelEvent::UPDATE:
            update(event.getImage(), event.getImageWidth(), event.getImageHeight(), event.getImagebytesPerPixel());
            break;
        default:
            throw "invalid";
    }
}


void EmulatorPanel::OnPaint(wxPaintEvent &event)
{
    if(!GetParent()->IsShown()){
        return;
    }
    wxPaintDC dc(this);
    render();
}


void EmulatorPanel::OnSize(wxSizeEvent &event)
{
    if(!GetParent()->IsShown()){
        return;
    }
    render();
}
void EmulatorPanel::OnEraseBackground(wxEraseEvent& event)
{
    //何もしない
}

GLuint getPowerOfTwo(GLuint size){
    GLuint def = 1;
    while(def < size){
        def <<= 1;
    }
    return def;
}
void EmulatorPanel::update(const uint8_t* image, const int w, const int h, const int bytesPerPixel)
{
    const GLuint texWidth = getPowerOfTwo(w);
    const GLuint texHeight = getPowerOfTwo(h);
    this->imageSurfaceWidth = texWidth;
    this->imageSurfaceHeight = texHeight;
    this->imageWidth = w;
    this->imageHeight = h;
    if(!imageLoaded){
        glGenTextures(1, &this->imageTexture);
        imageLoaded = true;
    }
    glBindTexture(GL_TEXTURE_2D, this->imageTexture);
    if(texWidth != static_cast<GLuint>(w) || texHeight != static_cast<GLuint>(h)){
        const int bytesPerLine = texWidth * bytesPerPixel;
        const int origBytesPerLine = w * bytesPerPixel;
        char bits[texHeight * bytesPerLine];
        for(GLuint y=0;y<static_cast<GLuint>(h);y++){
            memcpy(&bits[y * bytesPerLine], &image[y * origBytesPerLine], origBytesPerLine);
        }
        glTexImage2D(
            GL_TEXTURE_2D, 0, bytesPerPixel, texWidth, texHeight ,
            0, GL_RGB ,GL_UNSIGNED_BYTE , bits
        );
    }else{
        glTexImage2D(
            GL_TEXTURE_2D, 0, bytesPerPixel, texWidth, texHeight ,
            0, GL_RGB ,GL_UNSIGNED_BYTE , image
        );
    }
    wxPaintEvent paintEvt;
    this->ProcessEvent(paintEvt);
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

EmulatorPanelEvent::EmulatorPanelEvent(enum EventType type, int winid):
    wxEvent(winid, EMULATOR_PANEL_EVENT),
    type(type),
    image(NULL)
{

}

EmulatorPanelEvent::~EmulatorPanelEvent()
{
    if(this->image != NULL){
        delete[] this->image;
        this->image = NULL;
    }
}

EmulatorPanelEvent::EmulatorPanelEvent(const EmulatorPanelEvent& event) :
    wxEvent(event),
    type(event.type),
    image(NULL),
    imageBytesPerPixel(event.imageBytesPerPixel),
    imageWidth(event.imageWidth),
    imageHeight(event.imageHeight)
{
    const int imageSize = getImageSize();
    uint8_t* copiedImage = new uint8_t[imageSize];
    memcpy(copiedImage, event.image, imageSize);
    this->image = copiedImage;
}

EmulatorPanelEvent::EventType EmulatorPanelEvent::getType() const
{
    return this->type;
}

void EmulatorPanelEvent::setImage(const uint8_t* const image, const int width, const int height, const int bytesPerPixel)
{
    this->imageBytesPerPixel = bytesPerPixel;
    this->imageWidth = width;
    this->imageHeight = height;
    if(this->image != NULL){
        delete [] this->image;
    }
    const int imageSize = getImageSize();
    uint8_t* copiedImage = new uint8_t[imageSize];
    memcpy(copiedImage, image, imageSize);
    this->image = copiedImage;
}

const uint8_t* EmulatorPanelEvent::getImage() const
{
    return this->image;
}
int EmulatorPanelEvent::getImageSize() const
{
    return this->imageWidth * this->imageHeight * this->imageBytesPerPixel;
}
int EmulatorPanelEvent::getImagebytesPerPixel() const
{
    return this->imageBytesPerPixel;
}
int EmulatorPanelEvent::getImageWidth() const
{
    return this->imageWidth;
}
int EmulatorPanelEvent::getImageHeight() const
{
    return this->imageHeight;
}
wxEvent* EmulatorPanelEvent::Clone() const
{
    return new EmulatorPanelEvent(*this);
}
