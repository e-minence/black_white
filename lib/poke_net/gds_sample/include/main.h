#ifndef AUTOMATCH_H_
#define AUTOMATCH_H_

#include "utility.h"

extern void NitroMain ();
extern void drawConsole( void );

extern KeyControl		g_KeyCtrl;

// �o�g���f�[�^�r�f�I���A�b�v���[�h���邩�ǂ���
// ���݂͏����`�F�b�N���s���邽�߁A�T���v������ł̓A�b�v���[�h�ł��܂���
// �A�b�v���[�h���j���[��L���ɂ���ꍇ�A�ȉ��̒�`��L���ɂ��܂�
//#define BATTLEVIDEO_UPLOAD_ENABLE


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
    u64  inputValue;  // ���͂��ꂽ�l
}InputElement;

// ���̓V�[�P���X
#define MAX_INPUT_ELEMENTS      12
typedef struct tagInputSequence
{
    const int type;                                     // ���̓^�C�v
    const int numElements;                              // �v�f�̐�(���j���[�p INPUT �ł͖���)
    // ���j���[�̏ꍇ�̓��j���[�A�C�e��������
    // ���͂̏ꍇ�͓��͗v�f�����
    // ���͗v�f�͕����ł͂Ȃ�
    InputElement elements[MAX_INPUT_ELEMENTS];          // �v�f(MAX_INPUT_ELEMENTS �ȓ�)
    int currentElement;                                 // ���j���[�Ō��ݑI������Ă���ԍ�(���͂ł͎g�p���Ȃ�)
    u64 result;                                         // ����(���j���[�Ȃ�I��ԍ�, ���͂Ȃ�l)
}InputSequence;

// �Q�[���V�[�P���X���䃊�X�g�\����
#define MAX_SCENE_ELEMENTS      5
typedef struct tagGameSequence
{
    const char* modeName;                               // ���[�h��
    const int   numElements;                            // �v�f��(MAX_SCENE_ELEMENTS �ȓ�)
    int (*fpResult)(struct tagGameSequence*);           // ���ʂ���������֐�
    InputSequence   inputList[MAX_SCENE_ELEMENTS];      // ���̓��X�g
    u64 result;                                         // ���쌋��
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
#ifdef BATTLEVIDEO_UPLOAD_ENABLE
    state_battle_upload,
#endif
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
