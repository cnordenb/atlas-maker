#ifndef APP_H
#define APP_H

#include <windows.h>

#define APP_TITLE L"Atlas Maker"

enum {
    ID_LIST = 100,
    ID_ADD,
    ID_REMOVE,
    ID_CLEAR,
    ID_RESIZE,
    ID_WIDTH,
    ID_BROWSE,
    ID_CREATE
};

/* The state behind the one main window. */
typedef struct {
    HWND hwnd;
    HWND list, info, resize, width, height, output, summary, status, create;
    HFONT font;
    const wchar_t **paths; /* input files, owned, kept in atlas order */
    int count;
    int src_w, src_h;      /* the inputs' shared resolution, or 0 while they can't be used */
} App;

extern App app;

/* window.c */
int window_run(HINSTANCE instance, int show);

/* actions.c: one function per user action, plus the refreshes they trigger. */
void inputs_changed(void);
void settings_changed(void);
void inputs_free(void);
void on_add(void);
void on_remove(void);
void on_clear(void);
void on_browse(void);
void on_create(void);

#endif
