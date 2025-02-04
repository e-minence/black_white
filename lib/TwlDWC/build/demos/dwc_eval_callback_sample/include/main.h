#ifndef AUTOMATCH_H_
#define AUTOMATCH_H_

#include "utility.h"

extern void NitroMain ();
extern void drawConsole( void );

extern KeyControl		g_KeyCtrl;

typedef struct stMatchKey
{
    char	key[16];      // マッチング用キー文字列
    char	value[16];    // キーに対応する値（文字列）
    int     ivalue;       // キーに対応する値（int型）
} MatchKey;

enum
{
    state_init,
    state_connect,
    state_login,
    state_online,
    state_matching,
    state_matched,
    state_disconnect,
    state_save,
    state_error,
    state_last
};

#endif // AUTOMATCH_H_
