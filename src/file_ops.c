#include "file_ops.h"
#include "display.h"
#include "fileio.h"
#include <stdio.h>
#include <string.h>

int editor_file_open(Editor *editor, const char *filename) {
    return file_open(editor->buffer, filename);
}

int editor_file_save(Editor *editor) {
    if (editor->buffer->filename[0]) {
        return file_save(editor->buffer, editor->buffer->filename);
    } else {
        return file_save_as(editor->buffer);
    }
}

int editor_file_save_as(Editor *editor, const char *filename) {
    return file_save(editor->buffer, filename);
}