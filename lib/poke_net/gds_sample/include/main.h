#ifndef AUTOMATCH_H_
#define AUTOMATCH_H_

#include "utility.h"

extern void NitroMain ();
extern void drawConsole( void );

extern KeyControl		g_KeyCtrl;

// ���j���[�^�C�v
enum {
    TYPE_NORMAL,        // �ʏ폈��
    TYPE_SELECT,        // �I��
    TYPE_INPUT          // ����
};

// ���͊֘A
enum {
    INPUT_CONFIRM,    // ���͊m��
    INPUT_NOW,        // ���͒�
    INPUT_CANCEL      // �L�����Z��
};
// ���͐���\����
// ���j���[�̏ꍇ�̓��b�Z�[�W�����̂ݎQ�Ƃ����
// �����͊܂܂Ȃ��B�����̂݁B
typedef struct tagInputElement
{
    const char* head; // ���̓��b�Z�[�W
    char buf[20];     // ���̓o�b�t�@
    u8   inputKeta;   // �������ڂ���͂��Ă��邩�H
    u32  inputValue;  // ���͂��ꂽ�l
}InputElement;

// ���̓V�[�P���X
#define MAX_INPUT_ELEMENTS      10
typedef struct tagInputSequence
{
    const int type;                                     // ���̓^�C�v
    const int numElements;                              // �v�f�̐�(���j���[�p INPUT �ł͖���)
    // ���j���[�̏ꍇ�̓��j���[�A�C�e��������
    // ���͂̏ꍇ�͓��͗v�f�����
    // ���͗v�f�͕����ł͂Ȃ�
    InputElement elements[MAX_INPUT_ELEMENTS];          // �v�f(MAX_INPUT_ELEMENTS �ȓ�)
    int currentElement;                                 // ���j���[�Ō��ݑI������Ă���ԍ�(���͂ł͎g�p���Ȃ�)
    int result;                                         // ����(���j���[�Ȃ�I��ԍ�, ���͂Ȃ�l)
}InputSequence;

// �Q�[���V�[�P���X���䃊�X�g�\����
#define MAX_SCENE_ELEMENTS      5
typedef struct tagGameSequence
{
    const char* modeName;                               // ���[�h��
    const int   numElements;                            // �v�f��(MAX_SCENE_ELEMENTS �ȓ�)
    int (*fpResult)(struct tagGameSequence*);           // ���ʂ���������֐�
    InputSequence   inputList[MAX_SCENE_ELEMENTS];      // ���̓��X�g
    int result;                                         // ���쌋��
    int currentElement;                                 // ���ݎ��s���̏����ԍ�
} GameSequence;

// �V�[����`
enum
{
    state_select_connect,
    state_init,
    state_connect,
    state_select_pid,
    state_input_pid,
    state_gds_init,
    state_menu,
    state_debug_message,
    state_musical_update,
    state_musical_download,
    state_battle_upload,
    state_battle_search,
    state_battle_rank_search,
    state_battle_download,
    state_event_battle_download,
    state_event_battle_bookmark,
    state_error,
    state_query_disconnect,
    state_disconnect,
    state_end
};

#endif // AUTOMATCH_H_
