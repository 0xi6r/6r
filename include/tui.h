#ifndef TUI_H
#define TUI_H

#include <windows.h>
#include "buffer.h"

#define COLOR_BLACK 0
#define COLOR_WHITE 15
#define COLOR_BLUE 1
#define COLOR_GREEN 10
#define COLOR_RED 4
#define COLOR_YELLOW 14

typedef struct {
    int rows;
    int cols;
    int cursor_x;
    int cursor_y;
    int offset_x;
    int offset_y;
    int status_height;
    int line_num_width;
    HANDLE stdout_handle;
    CONSOLE_SCREEN_BUFFER_INFO original_info;
} TUIState;

void tui_init(TUIState* tui);
void tui_cleanup(TUIState* tui);
void tui_draw(TUIState* tui, TextBuffer* buffer);
void tui_draw_status(TUIState* tui, TextBuffer* buffer);
void tui_draw_line_numbers(TUIState* tui, TextBuffer* buffer, int start_line);
void tui_update_cursor(TUIState* tui);
void tui_clear_screen();
void tui_set_color(int foreground, int background);
void tui_reset_color();
int tui_get_max_display_lines(TUIState* tui);
void tui_handle_resize(TUIState* tui);

#endif