#include "editor.h"
#include "platform.h"
#include <stdio.h>
#include <string.h>

void editor_init(Editor* editor) {
    editor->buffer = buffer_create();
    tui_init(&editor->tui);
    editor->running = 1;
}

void editor_show_help() {
    int rows, cols;
    platform_get_console_size(&rows, &cols);
    platform_clear_screen();
    
    // Help content lines
    const char* help_lines[] = {
        "6r - Simple TUI Editor - Help",
        "",
        "Navigation:",
        "  Arrow Keys    Move cursor",
        "  Home/End      Beginning/End of line",
        "  Page Up/Down  Scroll page",
        "",
        "Editing:",
        "  Type          Insert text",
        "  Backspace     Delete character",
        "  Enter         New line",
        "",
        "Commands:",
        "  Ctrl+S        Save file",
        "  Ctrl+O        Open file",
        "  Ctrl+N        New file",
        "  Ctrl+Q        Quit",
        "  F1            This help",
        "  ESC           Exit from help",
        "",
        "Press ESC to exit or any other key to return to editor..."
    };
    
    int num_lines = sizeof(help_lines) / sizeof(help_lines[0]);
    int start_row = (rows - num_lines) / 2;
    if (start_row < 0) start_row = 0;
    
    for (int i = 0; i < num_lines; i++) {
        platform_set_cursor_position(0, start_row + i);
        
        // Calculate center position for each line
        int line_len = strlen(help_lines[i]);
        int start_col = (cols - line_len) / 2;
        if (start_col < 0) start_col = 0;
        
        // Set colors for different sections
        if (i == 0) {
            // Title - bright white on blue
            platform_set_color(COLOR_WHITE, COLOR_BLUE);
        } else if (strstr(help_lines[i], "Navigation:") || strstr(help_lines[i], "Editing:") || strstr(help_lines[i], "Commands:")) {
            // Section headers - yellow on black
            platform_set_color(COLOR_YELLOW, COLOR_BLACK);
        } else if (strstr(help_lines[i], "Ctrl+") || strstr(help_lines[i], "F1") || strstr(help_lines[i], "ESC")) {
            // Commands - green on black
            platform_set_color(COLOR_GREEN, COLOR_BLACK);
        } else if (strstr(help_lines[i], "Press ESC") || strstr(help_lines[i], "Exit")) {
            // Exit instructions - red on black
            platform_set_color(COLOR_RED, COLOR_BLACK);
        } else {
            // Regular text - white on black
            platform_set_color(COLOR_WHITE, COLOR_BLACK);
        }
        
        // Add padding for centering
        for (int j = 0; j < start_col; j++) {
            putchar(' ');
        }
        
        printf("%s", help_lines[i]);
        platform_reset_color();
    }
    
    // Wait for key input and handle ESC for exit
    KeyEvent event;
    if (platform_get_key(&event)) {
        if (event.key == KEY_ESC) {
            return; // Signal to exit
        }
    }
}

void editor_run(Editor* editor) {
    KeyEvent event;
    
    while (editor->running) {
        tui_handle_resize(&editor->tui);
        tui_draw(&editor->tui, editor->buffer);
        
        if (input_get_key(&event)) {
            if (event.ctrl) {
                switch (event.key) {
                    case 's':  // Ctrl+S
                    case 'S':
                        if (editor_file_save(editor)) {
                            // File saved successfully
                        } else {
                            // Show error message
                        }
                        break;
                    case 'o':  // Ctrl+O
                    case 'O':
                        tui_cleanup(&editor->tui);
                        char filename[256];
                        printf("Open file: ");
                        if (scanf("%255s", filename) == 1) {
                            editor_file_open(editor, filename);
                        }
                        tui_init(&editor->tui);
                        break;
                    case 'n':  // Ctrl+N
                    case 'N':
                        tui_cleanup(&editor->tui);
                        editor_new(editor);
                        tui_init(&editor->tui);
                        break;
                    case 'q':  // Ctrl+Q
                    case 'Q':
                        if (editor->buffer->modified) {
                            tui_cleanup(&editor->tui);
                            printf("Save changes before quitting? (y/n): ");
                            char ch = getchar();
                            if (ch == 'y' || ch == 'Y') {
                                editor_file_save(editor);
                            }
                            tui_init(&editor->tui);
                        }
                        editor->running = 0;
                        break;
                }
            } else if (event.key == KEY_F1) {
                tui_cleanup(&editor->tui);
                editor_show_help();
                tui_init(&editor->tui);
            } else if (event.key == KEY_ESC) {
                // Handle ESC key for exit with confirmation
                int rows, cols;
                platform_get_console_size(&rows, &cols);
                platform_clear_screen();
                
                // Center the exit confirmation dialog
                const char* confirm_lines[] = {
                    "Are you sure you want to exit?",
                    "",
                    "Press 'y' to exit, any other key to continue"
                };
                
                int num_lines = sizeof(confirm_lines) / sizeof(confirm_lines[0]);
                int start_row = (rows - num_lines) / 2;
                if (start_row < 0) start_row = 0;
                
                for (int i = 0; i < num_lines; i++) {
                    platform_set_cursor_position(0, start_row + i);
                    
                    int line_len = strlen(confirm_lines[i]);
                    int start_col = (cols - line_len) / 2;
                    if (start_col < 0) start_col = 0;
                    
                    // Set colors for exit confirmation
                    if (i == 0) {
                        platform_set_color(COLOR_RED, COLOR_BLACK);  // Warning in red
                    } else if (i == 2) {
                        platform_set_color(COLOR_YELLOW, COLOR_BLACK);  // Instructions in yellow
                    } else {
                        platform_set_color(COLOR_WHITE, COLOR_BLACK);  // Normal text
                    }
                    
                    for (int j = 0; j < start_col; j++) {
                        putchar(' ');
                    }
                    
                    printf("%s", confirm_lines[i]);
                    platform_reset_color();
                }
                
                KeyEvent confirm_event;
                if (platform_get_key(&confirm_event)) {
                    if (confirm_event.key == 'y' || confirm_event.key == 'Y') {
                        if (editor->buffer->modified) {
                            platform_clear_screen();
                            platform_set_cursor_position(0, rows/2);
                            platform_set_color(COLOR_YELLOW, COLOR_BLACK);
                            
                            int start_col = (cols - 40) / 2;
                            for (int j = 0; j < start_col; j++) putchar(' ');
                            
                            printf("Save changes before exiting? (y/n): ");
                            platform_reset_color();
                            
                            char ch = getchar();
                            if (ch == 'y' || ch == 'Y') {
                                editor_file_save(editor);
                            }
                        }
                        editor->running = 0;
                    }
                }
                tui_init(&editor->tui);
            } else {
                input_handle_key(&editor->tui, editor->buffer, event);
            }
        }
    }
}

void editor_cleanup(Editor* editor) {
    tui_cleanup(&editor->tui);
    buffer_destroy(editor->buffer);
}

// Helper functions for display compatibility
const char* editor_get_filename(const Editor* editor) {
    return editor->buffer->filename[0] ? editor->buffer->filename : "Untitled";
}

int editor_get_cursor_line(const Editor* editor) {
    return editor->tui.cursor_line;
}

int editor_get_cursor_col(const Editor* editor) {
    return editor->tui.cursor_col;
}

int editor_is_modified(const Editor* editor) {
    return editor->buffer->modified;
}

int editor_get_line_count(const Editor* editor) {
    return editor->buffer->line_count;
}

const char* editor_get_line(const Editor* editor, int index) {
    return buffer_get_line(editor->buffer, index);
}

void editor_new(Editor* editor) {
    buffer_clear(editor->buffer);
    strcpy(editor->buffer->filename, "");
    editor->buffer->modified = 0;
}