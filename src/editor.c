#include "editor.h"
#include <stdio.h>

void editor_init(Editor* editor) {
    editor->buffer = buffer_create();
    tui_init(&editor->tui);
    editor->running = 1;
}

void editor_show_help() {
    system("cls");
    printf("6r - Simple TUI Editor - Help\n");
    printf("============================\n");
    printf("Navigation:\n");
    printf("  Arrow Keys    Move cursor\n");
    printf("  Home/End      Beginning/End of line\n");
    printf("  Page Up/Down  Scroll page\n");
    printf("\nEditing:\n");
    printf("  Type          Insert text\n");
    printf("  Backspace     Delete character\n");
    printf("  Enter         New line\n");
    printf("\nCommands:\n");
    printf("  Ctrl+S        Save file\n");
    printf("  Ctrl+O        Open file\n");
    printf("  Ctrl+N        New file\n");
    printf("  Ctrl+Q        Quit\n");
    printf("  F1            This help\n");
    printf("\nPress any key to return to editor...");
    getchar();
}

void editor_run(Editor* editor) {
    KeyEvent event;
    
    // Check for filename argument
    if (__argc > 1) {
        file_open(editor->buffer, __argv[1]);
    }
    
    while (editor->running) {
        tui_handle_resize(&editor->tui);
        tui_draw(&editor->tui, editor->buffer);
        
        if (input_get_key(&event)) {
            if (event.ctrl) {
                switch (event.key) {
                    case 's':  // Ctrl+S
                    case 'S':
                        if (editor->buffer->filename[0]) {
                            file_save(editor->buffer, editor->buffer->filename);
                        } else {
                            // Temporarily exit TUI for file dialog
                            tui_cleanup(&editor->tui);
                            file_save_as(editor->buffer);
                            tui_init(&editor->tui);
                        }
                        break;
                    case 'o':  // Ctrl+O
                    case 'O':
                        tui_cleanup(&editor->tui);
                        char filename[256];
                        printf("Open file: ");
                        if (scanf("%255s", filename) == 1) {
                            file_open(editor->buffer, filename);
                        }
                        tui_init(&editor->tui);
                        break;
                    case 'n':  // Ctrl+N
                    case 'N':
                        tui_cleanup(&editor->tui);
                        file_new(editor->buffer);
                        tui_init(&editor->tui);
                        break;
                    case 'q':  // Ctrl+Q
                    case 'Q':
                        if (editor->buffer->modified) {
                            tui_cleanup(&editor->tui);
                            printf("Save changes before quitting? (y/n): ");
                            char ch = getchar();
                            if (ch == 'y' || ch == 'Y') {
                                if (editor->buffer->filename[0]) {
                                    file_save(editor->buffer, editor->buffer->filename);
                                } else {
                                    file_save_as(editor->buffer);
                                }
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