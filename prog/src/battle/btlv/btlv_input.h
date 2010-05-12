
//============================================================================================
/**
 * @file  btlv_input.h
 * @brief �퓬�����
 * @author  soga
 * @date  2009.06.30
 */
//============================================================================================

#pragma once

#include "..\btl_common.h"
#include "..\btl_client.h"
#include "system/palanm.h"
#include "gamesystem/game_data.h"

#define BTLV_INPUT_POKEMON_MAX ( 3 )            //���͂���|�P����MAX
#define BTLV_INPUT_DIR_MAX  ( TEMOTI_POKEMAX )  //�ΏۑI��MAX
#define BTLV_INPUT_POKEICON_MAX ( 3 )           //�|�P�����A�C�R��MAX

//�퓬�����OBJ�p���b�g����
/*
0:�����X�^�[�{�[��
1:��邠�����{�^��
2:�Z���ΏۑI���{�^���A�j��
3:�V��A�C�R���������
4:�V��A�C�R���x�[�X
5:�V��A�C�R���x�[�X
6:�V��A�C�R���x�[�X
7:�V��A�C�R���x�[�X
8:�Z�^�C�v�A�C�R��
9:�Z�^�C�v�A�C�R��
A:�Z�^�C�v�A�C�R��
B:�|�P�����A�C�R��
C:�|�P�����A�C�R��
D:�|�P�����A�C�R��
E:�ʐM�A�C�R��
F:���[�J���C�Y�p�\��
*/

typedef struct _BTLV_INPUT_WORK BTLV_INPUT_WORK;

///�Z�I����ʍ\���p�̃f�[�^
typedef struct{
  WazaID      wazano[ PTL_WAZA_MAX ];
  u8          pp[ PTL_WAZA_MAX ];
  u8          ppmax[ PTL_WAZA_MAX ];
  BtlvMcssPos pos;
}BTLV_INPUT_WAZA_PARAM;

///�U���ΏۑI����ʍ\���p�̃f�[�^
typedef struct{
  u8                    sex   :2;
  u8                    exist :1;
  u8                          :5;
  u8                    status;
  u16                   dummy;

  s16                   hp;             //���݂�HP
  u16                   hpmax;          //���݂�HPMAX
  const POKEMON_PARAM*  pp;
}BTLV_INPUT_DIR_PARAM;

///�|�P�����I����ʍ\���p��Scene���[�N
typedef struct{
  BTLV_INPUT_DIR_PARAM  bidp[ BTLV_INPUT_DIR_MAX ];
  u8                    pos;
  u8                    waza_target;  ///<�ΏۑI���^�C�v
}BTLV_INPUT_SCENE_PARAM;

///YES/NO�I����ʍ\���p��Scene���[�N
typedef struct{
  STRBUF*   yes_msg;        //YES�ɑ������郁�b�Z�[�W
  STRBUF*   no_msg;         //NO�ɑ������郁�b�Z�[�W
  BOOL      b_cancel_flag;  //B�L�����Z���L�����������H
}BTLV_INPUT_YESNO_PARAM;

enum
{ 
  BTLV_INPUT_BR_SEL_REW = 0,  //�����߂�
  BTLV_INPUT_BR_SEL_FF,       //������
  BTLV_INPUT_BR_SEL_STOP,     //��~
};

typedef enum
{ 
  BTLV_INPUT_BR_STOP_NONE = 0,
  BTLV_INPUT_BR_STOP_KEY,         //�L�[�ɂ�钆�f
  BTLV_INPUT_BR_STOP_BREAK,       //�f�[�^�j��ɂ�钆�f
  BTLV_INPUT_BR_STOP_OVER,        //�^�掞�ԃI�[�o�[�ɂ�钆�f
  BTLV_INPUT_BR_STOP_SKIP,        //�`���v�^�[�X�L�b�v�ɂ�钆�f
}BTLV_INPUT_BR_STOP_FLAG;

//�o�g�����R�[�_�[��ʍ\���p��Scene���[�N
typedef struct{
  int                       play_chapter;   //���ʂōĐ����̃`���v�^
  int                       view_chapter;   //����ʂɕ\������`���v�^
  int                       max_chapter;    //���`���v�^��
  BTLV_INPUT_BR_STOP_FLAG   stop_flag;      //�Đ���~�t���O
}BTLV_INPUT_BATTLE_RECORDER_PARAM;

typedef enum
{
  BTLV_INPUT_CENTER_BUTTON_ESCAPE = FALSE,
  BTLV_INPUT_CENTER_BUTTON_MODORU = TRUE,
}BTLV_INPUT_CENTER_BUTTON_TYPE;

///�R�}���h�I����ʍ\���p�̃��[�N
typedef struct{
  BTLV_INPUT_DIR_PARAM          bidp[ 2 ][ BTLV_INPUT_DIR_MAX ];
  BTLV_INPUT_CENTER_BUTTON_TYPE center_button_type;
  BOOL                          trainer_flag;                         //�ΐ푊�肪�g���[�i�[���ǂ���
  int                           mons_no[ BTLV_INPUT_POKEICON_MAX ];   //�J��o���Ă���|�P�����i���o�[
  int                           form_no[ BTLV_INPUT_POKEICON_MAX ];   //�J��o���Ă���|�P�����̃t�H�����i���o�[
  int                           sex[ BTLV_INPUT_POKEICON_MAX ];       //�J��o���Ă���|�P�����̐���
  BtlvMcssPos                   pos;
  BtlBagMode                    bagMode;                              //�o�b�O���[�h
  u8                            shooterEnergy;                        //�V���[�^�[�G�l���M�[
}BTLV_INPUT_COMMAND_PARAM;

///���[�e�[�V������ʍ\���p�̃��[�N
#ifdef ROTATION_NEW_SYSTEM
typedef struct{
  const BTL_POKEPARAM*  bpp[ BTL_ROTATE_NUM ];
  BOOL  waza_exist[ BTL_ROTATE_NUM ][PTL_WAZA_MAX ];
}BTLV_INPUT_ROTATE_PARAM;
#else
typedef struct{
  const POKEMON_PARAM*  pp[ BTL_ROTATE_NUM ];
  BtlRotateDir          before_select_dir;                  //1�O�ɑI����������
}BTLV_INPUT_ROTATE_PARAM;
#endif

typedef enum
{
  BTLV_INPUT_TYPE_SINGLE = 0,
  BTLV_INPUT_TYPE_DOUBLE,
  BTLV_INPUT_TYPE_TRIPLE,
  BTLV_INPUT_TYPE_ROTATION,
}BTLV_INPUT_TYPE;

typedef enum
{
  BTLV_INPUT_SCRTYPE_STANDBY = 0,
  BTLV_INPUT_SCRTYPE_COMMAND,
  BTLV_INPUT_SCRTYPE_WAZA,
  BTLV_INPUT_SCRTYPE_DIR,
  BTLV_INPUT_SCRTYPE_YES_NO,
  BTLV_INPUT_SCRTYPE_ROTATE,
  BTLV_INPUT_SCRTYPE_BATTLE_RECORDER,
  BTLV_INPUT_SCRTYPE_PDC,

  BTLV_INPUT_SCRTYPE_MAX,
}BTLV_INPUT_SCRTYPE;

//SCRTYPE_COMMAND��status�p�����[�^�̈Ӗ�
enum
{
  BTLV_INPUT_STATUS_NONE = 0, ///<�|�P���������Ȃ�
  BTLV_INPUT_STATUS_ALIVE,    ///<�|�P����������i�����Ă�j
  BTLV_INPUT_STATUS_DEAD,     ///<�|�P����������i�C��j
  BTLV_INPUT_STATUS_NG,       ///<�|�P����������i�X�e�[�^�X�ُ�j
};

typedef struct
{ 
  const GFL_UI_TP_HITTBL* hit_tbl;
  const BOOL*             cancel_flag;
  const int*              button_pltt;
}BTLV_INPUT_HITTBL;

typedef struct
{
  s8 cur_pos[ 6 ];  //�J�[�\���|�W�V����
  s8 up;            //��L�[���������Ƃ��̈ړ���
  s8 down;          //���L�[���������Ƃ��̈ړ���
  s8 left;          //���L�[���������Ƃ��̈ړ���
  s8 right;         //�E�L�[���������Ƃ��̈ړ���
  s8 a_button;      //A�{�^�����������Ƃ��̂ǂ������������Ƃɂ��邩
  s8 b_button;      //�L�����Z���\���ǂ���
}BTLV_INPUT_KEYTBL;

#define BTLV_INPUT_NOMOVE   ( -128 )
#define BTLV_INPUT_MOVETBL  ( 0x40 )    //�J�[�\���ړ��̈ʒu���e�[�u���ŗ�L����t���O

typedef u32 BTLV_INPUT_ROTATE_RESULT; //���16bit:BtlRotateDir  ����16bit:�I�������Z�̈ʒuor�L�����Z��

//BtlRotateDir�擾
static inline u16 BTLV_INPUT_GetBtlRotateDir( BTLV_INPUT_ROTATE_RESULT result )
{
  return ( result & 0xffff0000 ) >> 16;
}

//�I�������Z�̈ʒuor�L�����Z���擾
static inline u16 BTLV_INPUT_GetSelectWaza( BTLV_INPUT_ROTATE_RESULT result )
{
  return ( result & 0xffff );
}

extern  BTLV_INPUT_WORK*  BTLV_INPUT_Init( GAMEDATA* gameData, BTLV_INPUT_TYPE type, BtlCompetitor comp,
                                           PALETTE_FADE_PTR pfd, GFL_FONT* font, u8* cursor_flag, HEAPID heapID );
extern  BTLV_INPUT_WORK*  BTLV_INPUT_InitEx( GAMEDATA* gameData, BTLV_INPUT_TYPE type, PALETTE_FADE_PTR pfd,
                                             GFL_FONT* font, u8* cursor_flag, HEAPID heapID );
extern  void              BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_Main( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_InitBG( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_ExitBG( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFrame( void );
extern  void              BTLV_INPUT_FreeFrame( void );
extern  void              BTLV_INPUT_SetFadeOut( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFadeIn( BTLV_INPUT_WORK* biw );
extern  BOOL              BTLV_INPUT_CheckFadeExecute( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type, void* param );
extern  int               BTLV_INPUT_CheckInput( BTLV_INPUT_WORK* biw, const BTLV_INPUT_HITTBL* tp_tbl,
                                                 const BTLV_INPUT_KEYTBL* key_tbl );
extern  BOOL              BTLV_INPUT_CheckInputDemo( BTLV_INPUT_WORK* biw );
extern  BOOL              BTLV_INPUT_CheckInputRotate( BTLV_INPUT_WORK* biw, BtlRotateDir* dir, int* select );
extern  BOOL              BTLV_INPUT_CheckExecute( BTLV_INPUT_WORK* biw );

