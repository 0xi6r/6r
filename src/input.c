#include "input.h"
#include "display.h"
#include "file_ops.h"
#include <conio.h>
#include <stdio.h>
#include <string.h>

void input_handle_key(Editor *editor, int *running) {
    int key = _getch();
    
    if (key == 224) { // Extended key
        key = _getch();
        switch (key) {
            case 72: // Up arrow
                editor_move_cursor(editor, 0, -1);
                break;
            case 80: // Down arrow
                editor_move_cursor(editor, 0, 1);
                break;
            case 75: // Left arrow
                editor_move_cursor(editor, -1, 0);
                break;
            case 77: // Right arrow
                editor_move_cursor(editor, 1, 0);
                break;
            case 71: // Home
                editor_move_to_line_start(editor);
                break;
            case 79: // End
                editor_move_to_line_end(editor);
                break;
            case 73: // Page Up
                editor_move_page_up(editor);
                break;
            case 81: // Page Down
                editor_move_page_down(editor);
                break;
            case 83: // Delete
                editor_delete_char(editor);
                break;
        }
    } else if (key == 8) { // Backspace
        editor_backspace(editor);
    } else if (key == 13) { // Enter
        editor_new_line(editor);
    } else if (key == 19) { // Ctrl+S
        file_save(editor);
    } else if (key == 15) { // Ctrl+O
        char filename[MAX_FILENAME];
        if (input_get_filename(filename, MAX_FILENAME)) {
            file_open(editor, filename);
        }
    } else if (key == 14) { // Ctrl+N
        editor_new(editor);
    } else if (key == 8) { // Ctrl+H
        display_show_help();
    } else if (key == 17) { // Ctrl+Q
        *running = 0;
    } else if (key >= 32 && key < 127) { // Printable character
        editor_insert_char(editor, key);
    }
}

int input_get_filename(char *buffer, int size) {
    display_clear_screen();
    printf("Enter filename: ");
    fgets(buffer, size, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';
    
    return strlen(buffer) > 0;
}