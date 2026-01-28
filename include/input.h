#ifndef INPUT_H
#define INPUT_H

#include "buffer.h"
#include "tui.h"

#define KEY_ESC 27
#define KEY_ENTER 13
#define KEY_BACKSPACE 8
#define KEY_TAB 9
#define KEY_CTRL_S 19
#define KEY_CTRL_O 15
#define KEY_CTRL_Q 17
#define KEY_CTRL_N 14
#define KEY_F1 0x70

typedef struct {
    int key;
    int ctrl;
    int alt;
    int shift;
} KeyEvent;

int input_get_key(KeyEvent* event);
void input_handle_key(TUIState* tui, TextBuffer* buffer, KeyEvent event);
void input_insert_char(TUIState* tui, TextBuffer* buffer, char ch);
void input_delete_char(TUIState* tui, TextBuffer* buffer);
void input_move_cursor(TUIState* tui, TextBuffer* buffer, int dx, int dy);
void input_scroll_to_cursor(TUIState* tui);

#endif