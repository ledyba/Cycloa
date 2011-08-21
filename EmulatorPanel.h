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
        EventType getType() const;
        virtual wxEvent *Clone() const;
    private:
        enum EventType type;
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
        void render();
        void update(const uint8_t* image, const int aw, const int ah,const int w, const int h, const int bytesPerPixel);
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
};
#endif // EMULATORPANEL_H
