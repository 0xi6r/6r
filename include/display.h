#ifndef DISPLAY_H
#define DISPLAY_H

#include "editor.h"

typedef struct {
    int width;
    int height;
} ScreenSize;

// Screen operations
void display_init(void);
void display_cleanup(void);
void display_clear_screen(void);
void display_set_cursor_position(int x, int y);
ScreenSize display_get_screen_size(void);
void display_show_cursor(void);
void display_hide_cursor(void);

// Rendering with optimization
void display_render_editor(const Editor *editor);
void display_show_status_bar(const Editor *editor);
void display_show_help(void);
void display_show_message(const char *message, int wait_for_key);
void display_show_save_dialog(void);

// Utility
void display_flush(void);

#endif // DISPLAY_H