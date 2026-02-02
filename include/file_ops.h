#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "editor.h"

int editor_file_open(Editor *editor, const char *filename);
int editor_file_save(Editor *editor);
int editor_file_save_as(Editor *editor, const char *filename);

#endif