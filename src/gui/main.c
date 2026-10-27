#include <windows.h>
#include <commctrl.h>

#include "app.h"
#include "image.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev, PWSTR cmd_line, int show)
{
    INITCOMMONCONTROLSEX controls = {sizeof controls, ICC_STANDARD_CLASSES};
    int code = 1;

    (void)prev;
    (void)cmd_line;
    InitCommonControlsEx(&controls);
    if (FAILED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE))) return 1;
    if (image_startup() == IMAGE_OK) {
        code = window_run(instance, show);
        image_shutdown();
    } else {
        MessageBoxW(NULL, L"Windows Imaging Component is not available.", APP_TITLE, MB_ICONERROR);
    }
    CoUninitialize();
    return code;
}
