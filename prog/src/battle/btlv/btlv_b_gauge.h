
//============================================================================================
/**
 * @file	btlv_b_gauge.h
 * @brief	�퓬�{�[���Q�[�W
 * @author	soga
 * @date	2009.07.23
 */
//============================================================================================

#pragma once
#include "battle/btl_common.h"

typedef struct _BTLV_BALL_GAUGE_WORK BTLV_BALL_GAUGE_WORK;

enum
{ 
  BTLV_BALL_GAUGE_STATUS_NONE = 0, ///<�|�P���������Ȃ�
  BTLV_BALL_GAUGE_STATUS_ALIVE,    ///<�|�P����������i�����Ă�j
  BTLV_BALL_GAUGE_STATUS_DEAD,     ///<�|�P����������i�C��j
  BTLV_BALL_GAUGE_STATUS_NG,       ///<�|�P����������i�X�e�[�^�X�ُ�j
  BTLV_BALL_GAUGE_STATUS_ARROW,    ///<���\���p
  BTLV_BALL_GAUGE_STATUS_MAX,
};

typedef enum
{ 
  BTLV_BALL_GAUGE_ANM_NORMAL_E = 0,
  BTLV_BALL_GAUGE_ANM_NG_E,
  BTLV_BALL_GAUGE_ANM_DEAD_E,
  BTLV_BALL_GAUGE_ANM_NORMAL_M,
  BTLV_BALL_GAUGE_ANM_NG_M,
  BTLV_BALL_GAUGE_ANM_DEAD_M,
  BTLV_BALL_GAUGE_ANM_NORMAL,
  BTLV_BALL_GAUGE_ANM_NG,
  BTLV_BALL_GAUGE_ANM_DEAD,
  BTLV_BALL_GAUGE_ANM_NONE,
  BTLV_BALL_GAUGE_ANM_ARROW_E,
  BTLV_BALL_GAUGE_ANM_ARROW_M,
}BTLV_BALL_GAUGE_ANM_NO;

typedef enum
{
  BTLV_BALL_GAUGE_TYPE_MINE = 0,
  BTLV_BALL_GAUGE_TYPE_ENEMY,
  BTLV_BALL_GAUGE_TYPE_MAX,
}BTLV_BALL_GAUGE_TYPE;

typedef struct
{ 
  BTLV_BALL_GAUGE_TYPE  type;
  int                   status[ TEMOTI_POKEMAX ];
}BTLV_BALL_GAUGE_PARAM;


extern  BTLV_BALL_GAUGE_WORK* BTLV_BALL_GAUGE_Create( const BTLV_BALL_GAUGE_PARAM* bbgp, HEAPID heapID );
extern  void                  BTLV_BALL_GAUGE_Delete( BTLV_BALL_GAUGE_WORK* bbgw );
extern  BOOL                  BTLV_BALL_GAUGE_CheckExecute( BTLV_BALL_GAUGE_WORK* bbgw );
