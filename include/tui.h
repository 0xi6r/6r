#ifndef TUI_H
#define TUI_H

#include "platform.h"
#include "buffer.h"

typedef struct {
    int rows;
    int cols;
    int cursor_x;
    int cursor_y;
    int offset_x;
    int offset_y;
    int status_height;
    int line_num_width;
    int cursor_line;
    int cursor_col;
    PlatformHandle stdout_handle;
#ifdef PLATFORM_WINDOWS
    ConsoleInfo original_info;
#endif
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