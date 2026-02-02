#include "tui.h"
#include <stdio.h>
#include <string.h>

void tui_init(TUIState* tui) {
    platform_init_terminal();
    
    platform_get_console_size(&tui->rows, &tui->cols);
    tui->status_height = 2;
    tui->cursor_x = 0;
    tui->cursor_y = 0;
    tui->offset_x = 0;
    tui->offset_y = 0;
    tui->line_num_width = 6;
    
#ifdef PLATFORM_WINDOWS
    tui->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(tui->stdout_handle, &tui->original_info);
#endif
    
    // Hide cursor initially
    platform_hide_cursor();
    
    // Set raw mode for input
    platform_set_raw_mode(1);
}

void tui_cleanup(TUIState* tui) {
    // Restore original console state
#ifdef PLATFORM_WINDOWS
    SetConsoleTextAttribute(tui->stdout_handle, tui->original_info.wAttributes);
#endif
    platform_show_cursor();
    platform_set_raw_mode(0);
    platform_reset_color();
    platform_clear_screen();
}

void tui_clear_screen() {
    platform_clear_screen();
}

void tui_set_color(int foreground, int background) {
    platform_set_color(foreground, background);
}

void tui_reset_color() {
    platform_reset_color();
}

void tui_draw(TUIState* tui, TextBuffer* buffer) {
    platform_set_cursor_position(0, 0);
    
    int max_display_lines = tui_get_max_display_lines(tui);
    int start_line = tui->offset_y;
    int end_line = start_line + max_display_lines;
    
    if (end_line > buffer->line_count) {
        end_line = buffer->line_count;
    }
    
    // Draw text area
    for (int i = start_line; i < end_line; i++) {
        platform_set_cursor_position(0, i - start_line);
        
        // Draw line numbers
        tui_set_color(COLOR_YELLOW, COLOR_BLACK);
        printf("%*d | ", tui->line_num_width - 2, i + 1);
        tui_reset_color();
        
        // Draw line content
        char* line = buffer_get_line(buffer, i);
        if (line) {
            int line_len = strlen(line);
            int start_pos = tui->offset_x;
            int max_chars = tui->cols - tui->line_num_width - 1;
            
            if (start_pos < line_len) {
                int chars_to_show = line_len - start_pos;
                if (chars_to_show > max_chars) {
                    chars_to_show = max_chars;
                }
                for (int j = 0; j < chars_to_show; j++) {
                    putchar(line[start_pos + j]);
                }
            }
        }
        
        // Clear rest of line
        for (int j = 0; j < tui->cols - (tui->line_num_width + (line ? strlen(line) - tui->offset_x : 0)); j++) {
            putchar(' ');
        }
    }
    
    // Clear remaining lines
    for (int i = end_line - start_line; i < max_display_lines; i++) {
        platform_set_cursor_position(0, i);
        for (int j = 0; j < tui->cols; j++) {
            putchar(' ');
        }
    }
    
    tui_draw_status(tui, buffer);
    tui_update_cursor(tui);
}

void tui_draw_status(TUIState* tui, TextBuffer* buffer) {
    int max_display_lines = tui_get_max_display_lines(tui);
    platform_set_cursor_position(0, max_display_lines);
    
    // Status bar background
    tui_set_color(COLOR_WHITE, COLOR_BLACK);
    for (int i = 0; i < tui->cols; i++) {
        putchar(' ');
    }
    
    platform_set_cursor_position(0, max_display_lines);
    
    // File info
    char status[256];
    const char* filename = buffer->filename[0] ? buffer->filename : "[New File]";
    snprintf(status, sizeof(status), " %s %s", 
             filename, buffer->modified ? "(modified)" : "");
    printf("%-30.30s", status);
    
    // Cursor position
    platform_set_cursor_position(tui->cols - 20, max_display_lines);
    printf("Ln %d, Col %d", tui->cursor_y + 1, tui->cursor_x + 1);
    
    // Second status line
    platform_set_cursor_position(0, max_display_lines + 1);
    
    tui_set_color(COLOR_BLACK, COLOR_BLUE);
    for (int i = 0; i < tui->cols; i++) {
        putchar(' ');
    }
    
    platform_set_cursor_position(0, max_display_lines + 1);
    
    printf(" ^S:Save  ^O:Open  ^N:New  ^Q:Quit  F1:Help");
    
    tui_reset_color();
}

void tui_update_cursor(TUIState* tui) {
    int max_display_lines = tui_get_max_display_lines(tui);
    int x = tui->cursor_x - tui->offset_x + tui->line_num_width;
    int y = tui->cursor_y - tui->offset_y;
    
    if (y >= 0 && y < max_display_lines) {
        platform_show_cursor();
        platform_set_cursor_position(x, y);
    }
}

int tui_get_max_display_lines(TUIState* tui) {
    return tui->rows - tui->status_height;
}

void tui_handle_resize(TUIState* tui) {
    int new_cols, new_rows;
    platform_get_console_size(&new_rows, &new_cols);
    
    if (new_cols != tui->cols || new_rows != tui->rows) {
        tui->cols = new_cols;
        tui->rows = new_rows;
        
        // Adjust cursor position if needed
        if (tui->cursor_x >= new_cols - tui->line_num_width) {
            tui->cursor_x = new_cols - tui->line_num_width - 1;
        }
        if (tui->cursor_x < 0) tui->cursor_x = 0;
    }
}