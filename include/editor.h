#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"
#include "tui.h"
#include "input.h"
#include "fileio.h"

typedef struct {
    TextBuffer* buffer;
    TUIState tui;
    int running;
} Editor;

void editor_init(Editor* editor);
void editor_run(Editor* editor);
void editor_cleanup(Editor* editor);
void editor_show_help();

// Additional helper functions for display compatibility
const char* editor_get_filename(const Editor* editor);
int editor_get_cursor_line(const Editor* editor);
int editor_get_cursor_col(const Editor* editor);
int editor_is_modified(const Editor* editor);
int editor_get_line_count(const Editor* editor);
const char* editor_get_line(const Editor* editor, int index);
void editor_new(Editor* editor);

#endif