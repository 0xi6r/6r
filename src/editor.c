#include "editor.h"
#include <stdlib.h>
#include <string.h>

Editor* editor_create(void) {
    Editor *editor = (Editor*)malloc(sizeof(Editor));
    if (!editor) return NULL;
    
    editor_new(editor);
    return editor;
}

void editor_destroy(Editor *editor) {
    if (editor) {
        free(editor);
    }
}

void editor_new(Editor *editor) {
    strcpy(editor->filename, "untitled.txt");
    editor->line_count = 1;
    strcpy(editor->lines[0], "");
    editor->cursor_line = 0;
    editor->cursor_col = 0;
    editor->modified = 0;
    editor->scroll_offset = 0;
}

void editor_insert_char(Editor *editor, char c) {
    if (editor->line_count >= MAX_LINES) return;
    
    int line_len = strlen(editor->lines[editor->cursor_line]);
    if (line_len >= MAX_LINE_LENGTH - 1) return;
    
    // Shift characters to the right
    for (int i = line_len; i > editor->cursor_col; i--) {
        editor->lines[editor->cursor_line][i] = editor->lines[editor->cursor_line][i - 1];
    }
    
    editor->lines[editor->cursor_line][editor->cursor_col] = c;
    editor->cursor_col++;
    editor->modified = 1;
}

void editor_delete_char(Editor *editor) {
    int line_len = strlen(editor->lines[editor->cursor_line]);
    if (editor->cursor_col >= line_len) return;
    
    for (int i = editor->cursor_col; i < line_len; i++) {
        editor->lines[editor->cursor_line][i] = editor->lines[editor->cursor_line][i + 1];
    }
    editor->modified = 1;
}

void editor_backspace(Editor *editor) {
    if (editor->cursor_col > 0) {
        editor->cursor_col--;
        editor_delete_char(editor);
    }
}

void editor_new_line(Editor *editor) {
    if (editor->line_count >= MAX_LINES) return;
    
    // Shift lines down
    for (int i = editor->line_count; i > editor->cursor_line; i--) {
        strcpy(editor->lines[i], editor->lines[i - 1]);
    }
    
    // Split current line
    strcpy(editor->lines[editor->cursor_line + 1], 
           &editor->lines[editor->cursor_line][editor->cursor_col]);
    editor->lines[editor->cursor_line][editor->cursor_col] = '\0';
    
    editor->cursor_line++;
    editor->cursor_col = 0;
    editor->line_count++;
    editor->modified = 1;
}

void editor_move_cursor(Editor *editor, int dx, int dy) {
    editor->cursor_col += dx;
    editor->cursor_line += dy;
    
    // Bounds checking
    if (editor->cursor_line < 0) editor->cursor_line = 0;
    if (editor->cursor_line >= editor->line_count) 
        editor->cursor_line = editor->line_count - 1;
    
    int line_len = strlen(editor->lines[editor->cursor_line]);
    if (editor->cursor_col < 0) editor->cursor_col = 0;
    if (editor->cursor_col > line_len) editor->cursor_col = line_len;
}

void editor_move_to_line_start(Editor *editor) {
    editor->cursor_col = 0;
}

void editor_move_to_line_end(Editor *editor) {
    editor->cursor_col = strlen(editor->lines[editor->cursor_line]);
}

void editor_move_page_up(Editor *editor) {
    editor->scroll_offset -= 10;
    if (editor->scroll_offset < 0) editor->scroll_offset = 0;
}

void editor_move_page_down(Editor *editor) {
    editor->scroll_offset += 10;
    if (editor->scroll_offset > editor->line_count - 1)
        editor->scroll_offset = editor->line_count - 1;
}

int editor_get_line_count(const Editor *editor) {
    return editor->line_count;
}

const char* editor_get_line(const Editor *editor, int line_num) {
    if (line_num < 0 || line_num >= editor->line_count) return "";
    return editor->lines[line_num];
}

const char* editor_get_filename(const Editor *editor) {
    return editor->filename;
}

int editor_is_modified(const Editor *editor) {
    return editor->modified;
}

int editor_get_cursor_line(const Editor *editor) {
    return editor->cursor_line;
}

int editor_get_cursor_col(const Editor *editor) {
    return editor->cursor_col;
}

int editor_get_scroll_offset(const Editor *editor) {
    return editor->scroll_offset;
}