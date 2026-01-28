#include "input.h"
#include <stdio.h>
#include <windows.h>

int input_get_key(KeyEvent* event) {
    INPUT_RECORD input_record;
    DWORD events_read;
    
    while (1) {
        ReadConsoleInput(GetStdHandle(STD_INPUT_HANDLE), &input_record, 1, &events_read);
        
        if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
            event->key = input_record.Event.KeyEvent.wVirtualKeyCode;
            event->ctrl = (input_record.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;
            event->alt = (input_record.Event.KeyEvent.dwControlKeyState & (LEFT_ALT_PRESSED | RIGHT_ALT_PRESSED)) != 0;
            event->shift = (input_record.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED) != 0;
            
            // For printable characters, also get the actual character
            if (event->key >= 0x30 && event->key <= 0x5A) {
                event->key = input_record.Event.KeyEvent.uChar.AsciiChar;
                if (event->key == 0) {
                    continue;  // Skip non-character keys
                }
            }
            
            return 1;
        }
    }
    
    return 0;
}

void input_handle_key(TUIState* tui, TextBuffer* buffer, KeyEvent event) {
    switch (event.key) {
        case VK_UP:
            input_move_cursor(tui, buffer, 0, -1);
            break;
        case VK_DOWN:
            input_move_cursor(tui, buffer, 0, 1);
            break;
        case VK_LEFT:
            input_move_cursor(tui, buffer, -1, 0);
            break;
        case VK_RIGHT:
            input_move_cursor(tui, buffer, 1, 0);
            break;
        case VK_HOME:
            tui->cursor_x = 0;
            break;
        case VK_END: {
            char* line = buffer_get_line(buffer, tui->cursor_y);
            tui->cursor_x = line ? strlen(line) : 0;
            break;
        }
        case VK_PRIOR:  // Page Up
            tui->cursor_y -= tui_get_max_display_lines(tui);
            if (tui->cursor_y < 0) tui->cursor_y = 0;
            break;
        case VK_NEXT:   // Page Down
            tui->cursor_y += tui_get_max_display_lines(tui);
            if (tui->cursor_y >= buffer->line_count) {
                tui->cursor_y = buffer->line_count - 1;
            }
            break;
        case KEY_BACKSPACE:
            if (tui->cursor_x > 0) {
                char* line = buffer_get_line(buffer, tui->cursor_y);
                if (line) {
                    int len = strlen(line);
                    for (int i = tui->cursor_x - 1; i < len; i++) {
                        line[i] = line[i + 1];
                    }
                    tui->cursor_x--;
                    buffer->modified = 1;
                }
            } else if (tui->cursor_y > 0) {
                // Merge with previous line
                char* prev_line = buffer_get_line(buffer, tui->cursor_y - 1);
                char* curr_line = buffer_get_line(buffer, tui->cursor_y);
                if (prev_line && curr_line) {
                    int prev_len = strlen(prev_line);
                    int curr_len = strlen(curr_line);
                    
                    if (prev_len + curr_len < MAX_LINE_LENGTH) {
                        strcat(prev_line, curr_line);
                        buffer_delete_line(buffer, tui->cursor_y);
                        tui->cursor_x = prev_len;
                        tui->cursor_y--;
                    }
                }
            }
            break;
        case KEY_ENTER: {
            char* line = buffer_get_line(buffer, tui->cursor_y);
            if (line) {
                buffer_split_line(buffer, tui->cursor_y, tui->cursor_x);
                tui->cursor_y++;
                tui->cursor_x = 0;
            }
            break;
        }
        default:
            if (event.key >= 32 && event.key <= 126) {  // Printable ASCII
                input_insert_char(tui, buffer, (char)event.key);
            } else if (event.ctrl) {
                // Handle Ctrl+key combinations
                // These will be processed in main loop
                break;
            }
            break;
    }
    
    input_scroll_to_cursor(tui);
}

void input_insert_char(TUIState* tui, TextBuffer* buffer, char ch) {
    char* line = buffer_get_line(buffer, tui->cursor_y);
    if (!line) return;
    
    int len = strlen(line);
    if (len >= MAX_LINE_LENGTH - 1) return;
    
    // Make space for new character
    for (int i = len; i >= tui->cursor_x; i--) {
        line[i + 1] = line[i];
    }
    
    line[tui->cursor_x] = ch;
    tui->cursor_x++;
    buffer->modified = 1;
}

void input_delete_char(TUIState* tui, TextBuffer* buffer) {
    char* line = buffer_get_line(buffer, tui->cursor_y);
    if (!line) return;
    
    int len = strlen(line);
    if (tui->cursor_x < len) {
        for (int i = tui->cursor_x; i < len; i++) {
            line[i] = line[i + 1];
        }
        buffer->modified = 1;
    }
}

void input_move_cursor(TUIState* tui, TextBuffer* buffer, int dx, int dy) {
    int new_x = tui->cursor_x + dx;
    int new_y = tui->cursor_y + dy;
    
    if (new_y >= 0 && new_y < buffer->line_count) {
        tui->cursor_y = new_y;
        
        char* line = buffer_get_line(buffer, tui->cursor_y);
        int line_len = line ? strlen(line) : 0;
        
        if (new_x < 0) new_x = 0;
        if (new_x > line_len) new_x = line_len;
        
        tui->cursor_x = new_x;
    }
}

void input_scroll_to_cursor(TUIState* tui) {
    int max_display_lines = tui_get_max_display_lines(tui);
    
    // Vertical scrolling
    if (tui->cursor_y < tui->offset_y) {
        tui->offset_y = tui->cursor_y;
    } else if (tui->cursor_y >= tui->offset_y + max_display_lines) {
        tui->offset_y = tui->cursor_y - max_display_lines + 1;
    }
    
    // Horizontal scrolling
    int max_visible_chars = tui->cols - tui->line_num_width - 1;
    if (tui->cursor_x < tui->offset_x) {
        tui->offset_x = tui->cursor_x;
    } else if (tui->cursor_x >= tui->offset_x + max_visible_chars) {
        tui->offset_x = tui->cursor_x - max_visible_chars + 1;
    }
    
    if (tui->offset_x < 0) tui->offset_x = 0;
}