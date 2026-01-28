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

#endif