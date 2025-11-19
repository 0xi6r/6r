#ifndef EDITOR_H
#define EDITOR_H

#define MAX_LINES 1000
#define MAX_LINE_LENGTH 256
#define MAX_FILENAME 256

typedef struct {
    char lines[MAX_LINES][MAX_LINE_LENGTH];
    int line_count;
    int cursor_line;
    int cursor_col;
    char filename[MAX_FILENAME];
    int modified;
    int scroll_offset;
} Editor;

// Lifecycle
Editor* editor_create(void);
void editor_destroy(Editor *editor);
void editor_new(Editor *editor);

// Text editing
void editor_insert_char(Editor *editor, char c);
void editor_delete_char(Editor *editor);
void editor_backspace(Editor *editor);
void editor_new_line(Editor *editor);

// Navigation
void editor_move_cursor(Editor *editor, int dx, int dy);
void editor_move_to_line_start(Editor *editor);
void editor_move_to_line_end(Editor *editor);
void editor_move_page_up(Editor *editor);
void editor_move_page_down(Editor *editor);

// Getters
int editor_get_line_count(const Editor *editor);
const char* editor_get_line(const Editor *editor, int line_num);
const char* editor_get_filename(const Editor *editor);
int editor_is_modified(const Editor *editor);
int editor_get_cursor_line(const Editor *editor);
int editor_get_cursor_col(const Editor *editor);
int editor_get_scroll_offset(const Editor *editor);

#endif // EDITOR_H