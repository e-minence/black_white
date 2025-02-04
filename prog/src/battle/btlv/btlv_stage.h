
//============================================================================================
/**
 * @file	btlv_stage.h
 * @brief	�퓬��ʕ`��p�֐��i���~�j
 * @author	soga
 * @date	2008.11.18
 */
//============================================================================================

#pragma once

#include "battle/battle.h"

typedef enum{
	BTLV_STAGE_MINE = 0,	//���������~
	BTLV_STAGE_ENEMY,			//���葤���~
	BTLV_STAGE_MAX				//��ʂɏo�����~�̐�
}BTLV_STAGE_DIR;

typedef enum
{ 
  BTLV_STAGE_VANISH_OFF = 0,
  BTLV_STAGE_VANISH_ON,
  BTLV_STAGE_VANISH_ALL_ON = 3,
}BTLV_STAGE_VANISH;

enum
{ 
  BTLV_STAGE_POS_Z_E = FX32_ONE * -15,
  BTLV_STAGE_POS_Z_M = FX32_ONE *  10,
};

typedef struct _BTLV_STAGE_WORK BTLV_STAGE_WORK;

extern  BTLV_STAGE_WORK*  BTLV_STAGE_Init( BtlRule rule, int index, u8 season, HEAPID heapID );
extern	void			        BTLV_STAGE_Exit( BTLV_STAGE_WORK *bsw );
extern	void			        BTLV_STAGE_Main( BTLV_STAGE_WORK *bsw );
extern	void			        BTLV_STAGE_Draw( BTLV_STAGE_WORK *bsw );
extern  void          	  BTLV_STAGE_SetPaletteFade( BTLV_STAGE_WORK *bsw, u8 start_evy, u8 end_evy, u8 wait, u16 rgb );
extern  BOOL              BTLV_STAGE_CheckExecutePaletteFade( BTLV_STAGE_WORK* bsw );
extern  void	            BTLV_STAGE_SetVanishFlag( BTLV_STAGE_WORK* bsw, BTLV_STAGE_DIR dir, BTLV_STAGE_VANISH flag );
extern  void	            BTLV_STAGE_SetAnmReq( BTLV_STAGE_WORK* bsw, int side, int index, fx32 speed, int distance );
extern  BOOL	            BTLV_STAGE_CheckExecuteAnmReq( BTLV_STAGE_WORK* bsw );

