
//============================================================================================
/**
 * @file	btlv_input.h
 * @brief	�퓬�����
 * @author	soga
 * @date	2009.06.30
 */
//============================================================================================

#pragma once

#include "battle/btl_common.h"

#define BTLV_INPUT_DIR_MAX  ( TEMOTI_POKEMAX )  //�ΏۑI��MAX
#define BTLV_INPUT_POKEICON_MAX ( 3 )           //�|�P�����A�C�R��MAX

typedef struct _BTLV_INPUT_WORK BTLV_INPUT_WORK;

///�Z�I����ʍ\���p�̃f�[�^
typedef struct{
  u16         wazano[ PTL_WAZA_MAX ];
  u16         pp[ PTL_WAZA_MAX ];
  u16         ppmax[ PTL_WAZA_MAX ];
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
  ARCDATID  msg_datID;        //���b�Z�[�W���i�[����Ă���ARCDATID
  int       yes_msg_index;    //YES�ɑ������郁�b�Z�[�W�̃C���f�b�N�X
  int       no_msg_index;     //NO�ɑ������郁�b�Z�[�W�̃C���f�b�N�X
}BTLV_INPUT_YESNO_PARAM;

typedef enum
{ 
  BTLV_INPUT_CENTER_BUTTON_ESCAPE = FALSE,
  BTLV_INPUT_CENTER_BUTTON_MODORU = TRUE,
}BTLV_INPUT_CENTER_BUTTON_TYPE;

///�R�}���h�I����ʍ\���p�̃��[�N
typedef struct{
  BTLV_INPUT_DIR_PARAM          bidp[ 2 ][ BTLV_INPUT_DIR_MAX ];
  BTLV_INPUT_CENTER_BUTTON_TYPE center_button_type;
  BOOL                          trainer_flag;                    //�ΐ푊�肪�g���[�i�[���ǂ���
  int                           mons_no[ BTLV_INPUT_POKEICON_MAX ];   //�J��o���Ă���|�P�����i���o�[
  int                           form_no[ BTLV_INPUT_POKEICON_MAX ];   //�J��o���Ă���|�P�����̃t�H�����i���o�[
  BtlvMcssPos                   pos;
}BTLV_INPUT_COMMAND_PARAM;

///���[�e�[�V������ʍ\���p�̃��[�N
typedef struct{
  const POKEMON_PARAM*  pp[ BTL_ROTATE_NUM ];
  BtlRotateDir          before_select_dir;                  //1�O�ɑI����������
}BTLV_INPUT_ROTATE_PARAM;

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
  s8 cur_pos[ 6 ];  //�J�[�\���|�W�V����
  s8 up;            //��L�[���������Ƃ��̈ړ���
  s8 down;          //���L�[���������Ƃ��̈ړ���
  s8 left;          //���L�[���������Ƃ��̈ړ���
  s8 right;         //�E�L�[���������Ƃ��̈ړ���
  s8 a_button;      //A�{�^�����������Ƃ��̂ǂ������������Ƃɂ��邩
  s8 b_button;      //�L�����Z���\���ǂ���
}BTLV_INPUT_KEYTBL;

#define BTLV_INPUT_NOMOVE ( -128 )

extern  BTLV_INPUT_WORK*  BTLV_INPUT_Init( BTLV_INPUT_TYPE type, GFL_FONT* font, u8* cursor_flag, HEAPID heapID );
extern	void			        BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw );
extern	void			        BTLV_INPUT_Main( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_InitBG( BTLV_INPUT_WORK* biw );
extern	void			        BTLV_INPUT_ExitBG( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFrame( void );
extern  void              BTLV_INPUT_FreeFrame( void );
extern  void              BTLV_INPUT_SetFadeOut( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFadeIn( BTLV_INPUT_WORK* biw );
extern  BOOL              BTLV_INPUT_CheckFadeExecute( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type, void* param );
extern  int               BTLV_INPUT_CheckInput( BTLV_INPUT_WORK* biw, const GFL_UI_TP_HITTBL* tp_tbl,
                                                 const BTLV_INPUT_KEYTBL* key_tbl );

