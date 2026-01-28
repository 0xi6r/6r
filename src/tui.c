#include "tui.h"
#include <stdio.h>
#include <string.h>

void tui_init(TUIState* tui) {
    tui->stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(tui->stdout_handle, &tui->original_info);
    
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(tui->stdout_handle, &csbi);
    
    tui->cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    tui->rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    tui->status_height = 2;
    tui->cursor_x = 0;
    tui->cursor_y = 0;
    tui->offset_x = 0;
    tui->offset_y = 0;
    tui->line_num_width = 6;
    
    // Hide cursor initially
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(tui->stdout_handle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(tui->stdout_handle, &cursorInfo);
    
    // Set console mode for input
    HANDLE stdin_handle = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode = 0;
    GetConsoleMode(stdin_handle, &mode);
    SetConsoleMode(stdin_handle, mode & ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT));
}

void tui_cleanup(TUIState* tui) {
    // Restore original console state
    SetConsoleTextAttribute(tui->stdout_handle, tui->original_info.wAttributes);
    
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(tui->stdout_handle, &cursorInfo);
    cursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(tui->stdout_handle, &cursorInfo);
    
    tui_clear_screen();
    SetConsoleCursorPosition(tui->stdout_handle, tui->original_info.dwCursorPosition);
}

void tui_clear_screen() {
    system("cls");
}

void tui_set_color(int foreground, int background) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                          foreground | (background << 4));
}

void tui_reset_color() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 
                          7 | (0 << 4));  // Default white on black
}

void tui_draw(TUIState* tui, TextBuffer* buffer) {
    COORD coord = {0, 0};
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    
    int max_display_lines = tui_get_max_display_lines(tui);
    int start_line = tui->offset_y;
    int end_line = start_line + max_display_lines;
    
    if (end_line > buffer->line_count) {
        end_line = buffer->line_count;
    }
    
    // Draw text area
    for (int i = start_line; i < end_line; i++) {
        coord.Y = i - start_line;
        coord.X = 0;
        SetConsoleCursorPosition(tui->stdout_handle, coord);
        
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
        coord.Y = i;
        coord.X = 0;
        SetConsoleCursorPosition(tui->stdout_handle, coord);
        for (int j = 0; j < tui->cols; j++) {
            putchar(' ');
        }
    }
    
    tui_draw_status(tui, buffer);
    tui_update_cursor(tui);
}

void tui_draw_status(TUIState* tui, TextBuffer* buffer) {
    int max_display_lines = tui_get_max_display_lines(tui);
    COORD coord = {0, max_display_lines};
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    
    // Status bar background
    tui_set_color(COLOR_BLACK, COLOR_WHITE);
    for (int i = 0; i < tui->cols; i++) {
        putchar(' ');
    }
    
    coord.Y = max_display_lines;
    coord.X = 0;
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    
    // File info
    char status[256];
    const char* filename = buffer->filename[0] ? buffer->filename : "[New File]";
    snprintf(status, sizeof(status), " %s %s", 
             filename, buffer->modified ? "(modified)" : "");
    printf("%-30.30s", status);
    
    // Cursor position
    coord.X = tui->cols - 20;
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    printf("Ln %d, Col %d", tui->cursor_y + 1, tui->cursor_x + 1);
    
    // Second status line
    coord.Y = max_display_lines + 1;
    coord.X = 0;
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    
    tui_set_color(COLOR_BLACK, COLOR_BLUE);
    for (int i = 0; i < tui->cols; i++) {
        putchar(' ');
    }
    
    coord.Y = max_display_lines + 1;
    coord.X = 0;
    SetConsoleCursorPosition(tui->stdout_handle, coord);
    
    printf(" ^S:Save  ^O:Open  ^N:New  ^Q:Quit  F1:Help");
    
    tui_reset_color();
}

void tui_update_cursor(TUIState* tui) {
    int max_display_lines = tui_get_max_display_lines(tui);
    COORD coord;
    coord.X = tui->cursor_x - tui->offset_x + tui->line_num_width;
    coord.Y = tui->cursor_y - tui->offset_y;
    
    if (coord.Y >= 0 && coord.Y < max_display_lines) {
        SetConsoleCursorPosition(tui->stdout_handle, coord);
    }
}

int tui_get_max_display_lines(TUIState* tui) {
    return tui->rows - tui->status_height;
}

void tui_handle_resize(TUIState* tui) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(tui->stdout_handle, &csbi);
    
    int new_cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int new_rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    
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