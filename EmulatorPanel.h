#ifndef EMULATORPANEL_H
#define EMULATORPANEL_H

#include <wx/wx.h>
#include <wx/glcanvas.h>

class EmulatorPanelEvent : public wxEvent {
    public:
        enum EventType {UPDATE};
        EmulatorPanelEvent(enum EventType type, int winid = 0);
        EmulatorPanelEvent(const EmulatorPanelEvent& event);
        ~EmulatorPanelEvent();
        void setImage(const uint8_t* const image, const int width, const int height, const int bytesPerPixel);
        EventType getType() const;
        const uint8_t* getImage() const;
        int getImagebytesPerPixel() const;
        int getImageSize() const;
        int getImageWidth() const;
        int getImageHeight() const;
        virtual wxEvent *Clone() const;
    private:
        enum EventType type;
        const uint8_t* image;
        int imageBytesPerPixel;
        int imageWidth;
        int imageHeight;
};

wxDECLARE_EVENT(EMULATOR_PANEL_EVENT, EmulatorPanelEvent);

class EmulatorPanel : public wxGLCanvas
{
    public:
        wxEXPLICIT
        EmulatorPanel(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const int *attribList = NULL,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const wxPalette& palette = wxNullPalette);
        wxEXPLICIT
        EmulatorPanel(wxWindow *parent,
               wxWindowID id = wxID_ANY,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxGLCanvasName,
               const int *attribList = NULL,
               const wxPalette& palette = wxNullPalette);
        virtual ~EmulatorPanel();
    protected:
    private:
        const wxGLContext ctx;
        GLuint imageTexture;
        int screenWidth;
        int screenHeight;
        int imageWidth;
        int imageHeight;
        int imageSurfaceWidth;
        int imageSurfaceHeight;
        bool initialized;
        bool imageLoaded;
        void initCanvas();
        void initialize();
        void OnEvent(EmulatorPanelEvent &event);
        void OnPaint(wxPaintEvent &event);
        void OnSize(wxSizeEvent &event);
        void OnEraseBackground(wxEraseEvent& event);
        void render();
        void update(const uint8_t* image, const int w, const int h, const int bytesPerPixel);
};
#endif // EMULATORPANEL_H
