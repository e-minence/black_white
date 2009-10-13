
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

#define BTLV_INPUT_DIR_MAX  ( TEMOTI_POKEMAX )   //�ΏۑI��MAX

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

typedef enum
{ 
  BTLV_INPUT_CENTER_BUTTON_ESCAPE = FALSE,
  BTLV_INPUT_CENTER_BUTTON_MODORU = TRUE,
}BTLV_INPUT_CENTER_BUTTON_TYPE;

///�R�}���h�I����ʍ\���p�̃��[�N
typedef struct{
  BTLV_INPUT_DIR_PARAM          bidp[ BTLV_INPUT_DIR_MAX ];
  BTLV_INPUT_CENTER_BUTTON_TYPE center_button_type;
}BTLV_INPUT_COMMAND_PARAM;

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

extern  BTLV_INPUT_WORK*  BTLV_INPUT_Init( BTLV_INPUT_TYPE type, GFL_FONT* font, HEAPID heapID );
extern	void			        BTLV_INPUT_Exit( BTLV_INPUT_WORK* biw );
extern	void			        BTLV_INPUT_Main( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_InitBG( BTLV_INPUT_WORK* biw );
extern	void			        BTLV_INPUT_ExitBG( BTLV_INPUT_WORK* biw );
extern  void              BTLV_INPUT_SetFrame( void );
extern  void              BTLV_INPUT_FreeFrame( void );
extern  void              BTLV_INPUT_CreateScreen( BTLV_INPUT_WORK* biw, BTLV_INPUT_SCRTYPE type, void* param );
extern  int               BTLV_INPUT_CheckInput( BTLV_INPUT_WORK* biw, const GFL_UI_TP_HITTBL* tp_tbl );

