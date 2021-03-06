//-----------------------------------------------------------------------------
// Our main() function for the headless (no OpenGL) test runner.
//
// Copyright 2016 whitequark
//-----------------------------------------------------------------------------
#include "solvespace.h"
#include <cairo.h>

namespace SolveSpace {

//-----------------------------------------------------------------------------
// Settings
//-----------------------------------------------------------------------------

class Setting {
public:
    enum class Type {
        Undefined,
        Int,
        Float,
        String
    };

    Type        type;
    int         valueInt;
    float       valueFloat;
    std::string valueString;

    void CheckType(Type expectedType) {
        ssassert(type == Setting::Type::Undefined ||
                 type == expectedType, "Wrong setting type");
        type = expectedType;
    }
};

std::map<std::string, Setting> settings;

void CnfFreezeInt(uint32_t val, const std::string &key) {
    Setting &setting = settings[key];
    setting.CheckType(Setting::Type::Int);
    setting.valueInt = val;
}
uint32_t CnfThawInt(uint32_t val, const std::string &key) {
    if(settings.find(key) != settings.end()) {
        Setting &setting = settings[key];
        setting.CheckType(Setting::Type::Int);
        val = setting.valueInt;
    }
    return val;
}

void CnfFreezeFloat(float val, const std::string &key) {
    Setting &setting = settings[key];
    setting.CheckType(Setting::Type::Float);
    setting.valueFloat = val;
}
float CnfThawFloat(float val, const std::string &key) {
    if(settings.find(key) != settings.end()) {
        Setting &setting = settings[key];
        setting.CheckType(Setting::Type::Float);
        val = setting.valueFloat;
    }
    return val;
}

void CnfFreezeString(const std::string &val, const std::string &key) {
    Setting &setting = settings[key];
    setting.CheckType(Setting::Type::String);
    setting.valueString = val;
}
std::string CnfThawString(const std::string &val, const std::string &key) {
    std::string ret = val;
    if(settings.find(key) != settings.end()) {
        Setting &setting = settings[key];
        setting.CheckType(Setting::Type::String);
        ret = setting.valueString;
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Timers
//-----------------------------------------------------------------------------

void SetTimerFor(int milliseconds) {
}
void SetAutosaveTimerFor(int minutes) {
}
void ScheduleLater() {
}

//-----------------------------------------------------------------------------
// Graphics window
//-----------------------------------------------------------------------------

void GetGraphicsWindowSize(int *w, int *h) {
    *w = *h = 600;
}
double GetScreenDpi() {
    return 72;
}

void InvalidateGraphics() {
}

std::shared_ptr<Pixmap> framebuffer;
bool antialias = true;
void PaintGraphics() {
    const Camera &camera = SS.GW.GetCamera();

    std::shared_ptr<Pixmap> pixmap = std::make_shared<Pixmap>();
    pixmap->format = Pixmap::Format::BGRA;
    pixmap->width  = camera.width;
    pixmap->height = camera.height;
    pixmap->stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, camera.width);
    pixmap->data   = std::vector<uint8_t>(pixmap->stride * pixmap->height);
    cairo_surface_t *surface =
        cairo_image_surface_create_for_data(&pixmap->data[0], CAIRO_FORMAT_RGB24,
                                            pixmap->width, pixmap->height, pixmap->stride);
    cairo_t *context = cairo_create(surface);

    CairoRenderer canvas;
    canvas.camera = camera;
    canvas.lighting = SS.GW.GetLighting();
    canvas.chordTolerance = SS.chordTol;
    canvas.context = context;
    canvas.antialias = antialias;

    SS.GW.Draw(&canvas);
    canvas.CullOccludedStrokes();
    canvas.OutputInPaintOrder();

    pixmap->ConvertTo(Pixmap::Format::RGBA);
    framebuffer = pixmap;

    canvas.Clear();

    cairo_surface_destroy(surface);
    cairo_destroy(context);
}

void SetCurrentFilename(const std::string &filename) {
}
void ToggleFullScreen() {
}
bool FullScreenIsActive() {
    return false;
}
void ShowGraphicsEditControl(int x, int y, int fontHeight, int minWidthChars,
                             const std::string &val) {
    ssassert(false, "Not implemented");
}
void HideGraphicsEditControl() {
}
bool GraphicsEditControlIsVisible() {
    return false;
}
void ToggleMenuBar() {
}
bool MenuBarIsVisible() {
    return false;
}
void AddContextMenuItem(const char *label, ContextCommand cmd) {
    ssassert(false, "Not implemented");
}
void CreateContextSubmenu() {
    ssassert(false, "Not implemented");
}
ContextCommand ShowContextMenu() {
    ssassert(false, "Not implemented");
}
void EnableMenuByCmd(Command cmd, bool enabled) {
}
void CheckMenuByCmd(Command cmd, bool checked) {
}
void RadioMenuByCmd(Command cmd, bool selected) {
}
void RefreshRecentMenus() {
}

//-----------------------------------------------------------------------------
// Text window
//-----------------------------------------------------------------------------

void ShowTextWindow(bool visible) {
}
void GetTextWindowSize(int *w, int *h) {
    *w = *h = 100;
}
void InvalidateText() {
}
void MoveTextScrollbarTo(int pos, int maxPos, int page) {
}
void SetMousePointerToHand(bool is_hand) {
}
void ShowTextEditControl(int x, int y, const std::string &val) {
    ssassert(false, "Not implemented");
}
void HideTextEditControl() {
}
bool TextEditControlIsVisible() {
    return false;
}

//-----------------------------------------------------------------------------
// Dialogs
//-----------------------------------------------------------------------------

bool GetOpenFile(std::string *filename, const std::string &activeOrEmpty,
                 const FileFilter filters[]) {
    ssassert(false, "Not implemented");
}
bool GetSaveFile(std::string *filename, const std::string &activeOrEmpty,
                 const FileFilter filters[]) {
    ssassert(false, "Not implemented");
}
DialogChoice SaveFileYesNoCancel() {
    ssassert(false, "Not implemented");
}
DialogChoice LoadAutosaveYesNo() {
    ssassert(false, "Not implemented");
}
DialogChoice LocateImportedFileYesNoCancel(const std::string &filename,
                                           bool canCancel) {
    ssassert(false, "Not implemented");
}
void DoMessageBox(const char *message, int rows, int cols, bool error) {
    dbp("%s box: %s", error ? "error" : "message", message);
    ssassert(false, "Not implemented");
}
void OpenWebsite(const char *url) {
    ssassert(false, "Not implemented");
}

//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------

std::vector<std::string> fontFiles;
std::vector<std::string> GetFontFiles() {
    return fontFiles;
}

std::string resourceDir;
const void *LoadResource(const std::string &name, size_t *size) {
    static std::map<std::string, std::vector<uint8_t>> cache;

    auto it = cache.find(name);
    if(it == cache.end()) {
        std::string path = resourceDir + "/" + name;
        std::vector<uint8_t> data;

        FILE *f = ssfopen(PathSepUnixToPlatform(path).c_str(), "rb");
        ssassert(f != NULL, "Cannot open resource");
        fseek(f, 0, SEEK_END);
        data.resize(ftell(f));
        fseek(f, 0, SEEK_SET);
        fread(&data[0], 1, data.size(), f);
        fclose(f);

        cache.emplace(name, std::move(data));
        it = cache.find(name);
    }

    *size = (*it).second.size();
    return &(*it).second[0];
}

//-----------------------------------------------------------------------------
// Application lifecycle
//-----------------------------------------------------------------------------

void ExitNow() {
    ssassert(false, "Not implemented");
}

}
