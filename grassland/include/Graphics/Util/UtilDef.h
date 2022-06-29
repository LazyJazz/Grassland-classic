#pragma once
#pragma once
#ifdef _WIN32
#include <Windows.h>
#endif

namespace Grassland {
#define GRL_WM_CREATE 0x0001
#define GRL_WM_KEYDOWN 0x0100
#define GRL_WM_KEYLAST 0x0109
#define GRL_WM_KEYUP 0x0101
#define GRL_WM_CLOSE 0x0010
#define GRL_WM_DESTROY 0x0002
#define GRL_WM_MOUSEMOVE 0x0200
#define GRL_WM_MOUSEWHEEL 0x020E
#define GRL_WM_SIZE 0x0005

#define GRL_KEY_A 65
#define GRL_KEY_B 66
#define GRL_KEY_C 67
#define GRL_KEY_D 68
#define GRL_KEY_E 69
#define GRL_KEY_F 70
#define GRL_KEY_G 71
#define GRL_KEY_H 72
#define GRL_KEY_I 73
#define GRL_KEY_J 74
#define GRL_KEY_K 75
#define GRL_KEY_L 76
#define GRL_KEY_M 77
#define GRL_KEY_N 78
#define GRL_KEY_O 79
#define GRL_KEY_P 80
#define GRL_KEY_Q 81
#define GRL_KEY_R 82
#define GRL_KEY_S 83
#define GRL_KEY_T 84
#define GRL_KEY_U 85
#define GRL_KEY_V 86
#define GRL_KEY_W 87
#define GRL_KEY_X 88
#define GRL_KEY_Y 89
#define GRL_KEY_Z 90
#define GRL_KEY_0 48
#define GRL_KEY_1 49
#define GRL_KEY_2 50
#define GRL_KEY_3 51
#define GRL_KEY_4 52
#define GRL_KEY_5 53
#define GRL_KEY_6 54
#define GRL_KEY_7 55
#define GRL_KEY_8 56
#define GRL_KEY_9 57
#define GRL_KEY_F1 0x70
#define GRL_KEY_F2 0x71
#define GRL_KEY_F3 0x72
#define GRL_KEY_F4 0x73
#define GRL_KEY_F5 0x74
#define GRL_KEY_F6 0x75
#define GRL_KEY_F7 0x76
#define GRL_KEY_F8 0x77
#define GRL_KEY_F9 0x78
#define GRL_KEY_F10 0x79
#define GRL_KEY_F11 0x7A
#define GRL_KEY_F12 0x7B
#define GRL_KEY_MINUS
#define GRL_KEY_EQUAL
#define GRL_KEY_LSHIFT 0xA0
#define GRL_KEY_LCONTROL 0xA2
#define GRL_KEY_LALT 0xA4
#define GRL_KEY_LWINDOWS 0x5B
#define GRL_KEY_RSHIFT 0xA1
#define GRL_KEY_RCONTROL 0xA3
#define GRL_KEY_RALT 0xA5
#define GRL_KEY_RWINDOWS 0x5C
#define GRL_KEY_TAB 0x09
#define GRL_KEY_ESCAPE 0x1B
#define GRL_KEY_ENTER 0x0D
#define GRL_KEY_LBUTTON 0x01
#define GRL_KEY_RBUTTON 0x02
#define GRL_KEY_MBUTTON 0x04
#define GRL_KEY_UP 0x26
#define GRL_KEY_DOWN 0x28
#define GRL_KEY_LEFT 0x25
#define GRL_KEY_RIGHT 0x27
}  // namespace Grassland
