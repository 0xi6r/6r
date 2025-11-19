#ifndef FILE_OPS_H
#define FILE_OPS_H

#include "editor.h"

// File operations
int file_open(Editor *editor, const char *filename);
int file_save(Editor *editor);
int file_save_as(Editor *editor, const char *filename);

#endif // FILE_OPS_H