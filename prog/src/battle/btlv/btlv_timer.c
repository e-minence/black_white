
//============================================================================================
/**
 * @file  btlv_timer.c
 * @brief �퓬�^�C�}�[
 * @author  soga
 * @date  2009.10.27
 */
//============================================================================================

#include <gflib.h>

#include "btlv_effect.h"
#include "btlv_timer.h"

#include "arc_def.h"
#include "battle/battgra_wb.naix"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================
enum
{ 
  BTLV_TIMER_GAME_TIME = 0,
  BTLV_TIMER_COMMAND_TIME,
  BTLV_TIMER_SEPARATES,
  BTLV_TIMER_ZERO,
  BTLV_TIMER_ONE,
  BTLV_TIMER_TWO,
  BTLV_TIMER_THREE,
  BTLV_TIMER_FOUR,
  BTLV_TIMER_FIVE,
  BTLV_TIMER_SIX,
  BTLV_TIMER_SEVEN,
  BTLV_TIMER_EIGHT,
  BTLV_TIMER_NINE,

  BTLV_TIMER_ALERT,

  BTLV_TIMER_GAME_TIME_Y    = 128,
  BTLV_TIMER_COMMAND_TIME_Y = 136,

  BTLV_TIMER_LABEL_X      = 0,
  BTLV_TIMER_MINUTE_10_X  = BTLV_TIMER_LABEL_X + 64,
  BTLV_TIMER_MINUTE_01_X  = BTLV_TIMER_MINUTE_10_X + 4,
  BTLV_TIMER_SEPARATES_X  = BTLV_TIMER_MINUTE_01_X + 4,
  BTLV_TIMER_SECOND_10_X  = BTLV_TIMER_SEPARATES_X + 6,
  BTLV_TIMER_SECOND_01_X  = BTLV_TIMER_SECOND_10_X + 4,

  BTLV_TIMER_LABEL = 0,
  BTLV_TIMER_MINUTE_10,
  BTLV_TIMER_MINUTE_01,
  BTLV_TIMER_SEPATATES,
  BTLV_TIMER_SECOND_10,
  BTLV_TIMER_SECOND_01,
  BTLV_TIMER_CLWK_MAX,

  BTLV_TIMER_1SEC_FRAME = 60,

  BTLV_TIMER_MAX = 99 * 60 + 59,
};

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct  _BTLV_TIMER_CLWK
{ 
  GFL_CLWK*     clwk[ BTLV_TIMER_CLWK_MAX ];
  GFL_CLACTPOS  pos;
};

struct  _BTLV_TIMER_WORK
{ 
  GFL_CLUNIT*     clunit[ BTLV_TIMER_TYPE_MAX ];
  BTLV_TIMER_CLWK btcl[ BTLV_TIMER_TYPE_MAX ];

  u32             charID;
  u32             cellID;
  u32             plttID;

  int             second[ BTLV_TIMER_TYPE_MAX ];
  OSTick          tick[ BTLV_TIMER_TYPE_MAX ];

  int             alert[ BTLV_TIMER_TYPE_MAX ];
  
  GFL_TCB*        draw;
  GFL_TCB*        count_down;

  HEAPID          heapID;
};

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_TIMER_Draw( BTLV_TIMER_WORK* btw, BTLV_TIMER_TYPE type );
static  void  TCB_BTLV_TIMER_Draw( GFL_TCB *tcb, void *work );

//============================================================================================
/**
 *  @brief  �V�X�e��������
 *
 *  @param[in]  heapID  �q�[�vID
 */
//============================================================================================
BTLV_TIMER_WORK*  BTLV_TIMER_Init( HEAPID heapID )
{ 
  BTLV_TIMER_WORK *btw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_TIMER_WORK ) );
  ARCHANDLE*      handle;

  btw->heapID = heapID;
  handle = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, btw->heapID );

  btw->clunit[ BTLV_TIMER_TYPE_GAME_TIME ] = GFL_CLACT_UNIT_Create( BTLV_TIMER_CLWK_MAX, 0, btw->heapID );
  btw->clunit[ BTLV_TIMER_TYPE_COMMAND_TIME ] = GFL_CLACT_UNIT_Create( BTLV_TIMER_CLWK_MAX, 0, btw->heapID );
  btw->charID = GFL_CLGRP_CGR_Register( handle, NARC_battgra_wb_battle_time_num_NCGR, FALSE, CLSYS_DRAW_MAIN, btw->heapID );
  btw->cellID = GFL_CLGRP_CELLANIM_Register( handle, NARC_battgra_wb_battle_time_num_NCER,
                                             NARC_battgra_wb_battle_time_num_NANR, btw->heapID );
  btw->plttID = GFL_CLGRP_PLTT_Register( handle, NARC_battgra_wb_gauge_NCLR, CLSYS_DRAW_MAIN,
                                         BTLV_OBJ_PLTT_TIMER, btw->heapID );
  PaletteWorkSet_VramCopy( BTLV_EFFECT_GetPfd(), FADE_MAIN_OBJ,
                           GFL_CLGRP_PLTT_GetAddr( btw->plttID, CLSYS_DRAW_MAIN ) / 2, 0x20 * 1 );

  GFL_ARC_CloseDataHandle( handle );

  OS_SetTick( 0 );

  return btw;
}

//============================================================================================
/**
 *  @brief  �V�X�e���I��
 *
 *  @param[in]  btw  �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void  BTLV_TIMER_Exit( BTLV_TIMER_WORK* btw )
{ 
  BTLV_TIMER_Delete( btw );

  GFL_CLGRP_CGR_Release( btw->charID );
  GFL_CLGRP_CELLANIM_Release( btw->cellID );
  GFL_CLGRP_PLTT_Release( btw->plttID );
  GFL_CLACT_UNIT_Delete( btw->clunit[ BTLV_TIMER_TYPE_GAME_TIME ] );
  GFL_CLACT_UNIT_Delete( btw->clunit[ BTLV_TIMER_TYPE_COMMAND_TIME ] );
  GFL_HEAP_FreeMemory( btw );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[����
 *
 *  @param[in]  btw           �V�X�e���Ǘ��\���̂̃|�C���^
 *  @param[in]  game_time     �����^�C�}�[�l�i�b�j
 *  @param[in]  command_time  �����^�C�}�[�l�i�b�j
 */
//============================================================================================
void  BTLV_TIMER_Create( BTLV_TIMER_WORK* btw, int game_time, int command_time )
{ 
  GFL_CLWK_DATA timer = {
    0, 0,     //x, y
    0, 0, 0,  //�A�j���ԍ��A�D�揇�ʁABG�v���C�I���e�B
  };
  int  timer_pos_x[] = { 
    BTLV_TIMER_LABEL_X,
    BTLV_TIMER_MINUTE_10_X,
    BTLV_TIMER_MINUTE_01_X,
    BTLV_TIMER_SEPARATES_X,
    BTLV_TIMER_SECOND_10_X,
    BTLV_TIMER_SECOND_01_X,
  };
  int  timer_pos_y[] = { 
    BTLV_TIMER_GAME_TIME_Y,
    BTLV_TIMER_COMMAND_TIME_Y,
  };
  int timer_anm[ 2 ][ BTLV_TIMER_CLWK_MAX ] = { 
    { BTLV_TIMER_GAME_TIME,    BTLV_TIMER_ZERO, BTLV_TIMER_ZERO, BTLV_TIMER_SEPARATES, BTLV_TIMER_ZERO, BTLV_TIMER_ZERO },
    { BTLV_TIMER_COMMAND_TIME, BTLV_TIMER_ZERO, BTLV_TIMER_ZERO, BTLV_TIMER_SEPARATES, BTLV_TIMER_ZERO, BTLV_TIMER_ZERO },
  };
  int i,j;

  for( i = 0 ; i < BTLV_TIMER_TYPE_MAX ; i++ )
  { 
    for( j = 0 ; j < BTLV_TIMER_CLWK_MAX ; j++ )
    { 
      timer.pos_x = timer_pos_x[ j ];
      timer.pos_y = timer_pos_y[ i ];
      timer.anmseq = timer_anm[ i ][ j ];
      btw->btcl[ i ].clwk[ j ] = GFL_CLACT_WK_Create( btw->clunit[ i ], btw->charID, btw->plttID, btw->cellID,
                                                      &timer, CLSYS_DEFREND_MAIN, btw->heapID );
      GFL_CLACT_WK_SetAutoAnmFlag( btw->btcl[ i ].clwk[ j ], TRUE );
    }
  }

  if( game_time > BTLV_TIMER_MAX )
  { 
    game_time = BTLV_TIMER_MAX;
  }
  if( command_time > BTLV_TIMER_MAX )
  { 
    command_time = BTLV_TIMER_MAX;
  }

  btw->second[ BTLV_TIMER_TYPE_GAME_TIME ]         = game_time;
  btw->second[ BTLV_TIMER_TYPE_COMMAND_TIME ]      = command_time;
  btw->tick[ BTLV_TIMER_TYPE_GAME_TIME ] = btw->tick[ BTLV_TIMER_TYPE_COMMAND_TIME ] = OS_GetTick();

//  BTLV_TIMER_Draw( btw, BTLV_TIMER_TYPE_GAME_TIME );
//  BTLV_TIMER_Draw( btw, BTLV_TIMER_TYPE_COMMAND_TIME );
  BTLV_TIMER_SetDrawEnable( btw, BTLV_TIMER_TYPE_GAME_TIME, FALSE, FALSE );
  BTLV_TIMER_SetDrawEnable( btw, BTLV_TIMER_TYPE_COMMAND_TIME, FALSE, FALSE );

  btw->draw       = GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), TCB_BTLV_TIMER_Draw, btw, 0 );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�폜
 *
 *  @param[in]  btw  �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void  BTLV_TIMER_Delete( BTLV_TIMER_WORK* btw )
{ 
  int i,j;

  for( i = 0 ; i < BTLV_TIMER_TYPE_MAX ; i++ )
  { 
    for( j = 0 ; j < BTLV_TIMER_CLWK_MAX ; j++ )
    { 
      if( btw->btcl[ i ].clwk[ j ] ) 
      { 
        GFL_CLACT_WK_Remove( btw->btcl[ i ].clwk[ j ] );
        btw->btcl[ i ].clwk[ j ] = NULL; 
      }
    }
  }
  if( btw->draw )
  { 
    GFL_TCB_DeleteTask( btw->draw );
    btw->draw = NULL;
  }
  if( btw->count_down )
  { 
    GFL_TCB_DeleteTask( btw->count_down );
    btw->count_down = NULL;
  }
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�`�拖��
 *
 *  @param[in]  btw     �V�X�e���Ǘ��\���̂̃|�C���^
 *  @param[in]  type    �^�C�}�[�^�C�v
 *  @param[in]  enable  TRUE:�`��@FALSE:��`��
 *  @param[in]  init    TRUE:�J�E���^�����������ĕ`��@FALSE:�����������`��ienable��TRUE�̂Ƃ��ɂ����Ӗ�������܂���j
 *
 */
//============================================================================================
void  BTLV_TIMER_SetDrawEnable( BTLV_TIMER_WORK* btw, BTLV_TIMER_TYPE type, BOOL enable, BOOL init )
{ 
  if( ( enable == TRUE ) && ( init == TRUE ))
  { 
    btw->tick[ type ] = OS_GetTick();
    btw->alert[ type ] = 0;
    BTLV_TIMER_Draw( btw, type );
  }

  GFL_CLACT_UNIT_SetDrawEnable( btw->clunit[ type ], enable );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�`��
 *
 *  @param[in]  btw   �V�X�e���Ǘ��\���̂̃|�C���^
 *  @param[in]  type  �^�C�}�[�^�C�v
 */
//============================================================================================
static  void  BTLV_TIMER_Draw( BTLV_TIMER_WORK* btw, BTLV_TIMER_TYPE type )
{ 
  int alert[ BTLV_TIMER_TYPE_MAX ] = { 300, 15 };
  int label_anm = ( type == BTLV_TIMER_TYPE_GAME_TIME ) ? BTLV_TIMER_GAME_TIME : BTLV_TIMER_COMMAND_TIME;
  int timer = btw->second[ type ] - OS_TicksToSeconds( OS_GetTick() - btw->tick[ type ] );
  int minute;
  int second;
  int minute_10;
  int minute_01;
  int second_10;
  int second_01;

  if( timer < 0 )
  { 
    timer = 0;
  }

  minute = timer / 60;
  second = timer % 60; 
  minute_10 = minute / 10;
  minute_01 = minute % 10;
  second_10 = second / 10;
  second_01 = second % 10;

  if( timer == alert[ type ] )
  { 
    btw->alert[ type ] = BTLV_TIMER_ALERT;
  }

  GFL_CLACT_WK_SetAnmSeq( btw->btcl[ type ].clwk[ BTLV_TIMER_LABEL ], label_anm + btw->alert[ type ] );

  if( minute_10 == 0 )
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_10 ], FALSE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_10 ], TRUE );
    GFL_CLACT_WK_SetAnmSeq( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_10 ], BTLV_TIMER_ZERO + minute_10 + btw->alert[ type ] );
  }

  if( ( minute_10 == 0 ) && ( minute_01 == 0 ) )
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_01 ], FALSE );
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_SEPATATES ], FALSE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_01 ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_SEPATATES ], TRUE );
    GFL_CLACT_WK_SetAnmSeq( btw->btcl[ type ].clwk[ BTLV_TIMER_MINUTE_01 ], BTLV_TIMER_ZERO + minute_01 + btw->alert[ type ] );
  }

  if( ( minute_10 == 0 ) && ( minute_01 == 0 ) && ( second_10 == 0 ) )
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_SECOND_10 ], FALSE );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( btw->btcl[ type ].clwk[ BTLV_TIMER_SECOND_10 ], TRUE );
    GFL_CLACT_WK_SetAnmSeq( btw->btcl[ type ].clwk[ BTLV_TIMER_SECOND_10 ], BTLV_TIMER_ZERO + second_10 + btw->alert[ type ] );
  }

  GFL_CLACT_WK_SetAnmSeq( btw->btcl[ type ].clwk[ BTLV_TIMER_SECOND_01 ], BTLV_TIMER_ZERO + second_01 + btw->alert[ type ] );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�`��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_TIMER_Draw( GFL_TCB *tcb, void *work )
{ 
  BTLV_TIMER_WORK *btw = ( BTLV_TIMER_WORK* )work;

  BTLV_TIMER_Draw( btw, BTLV_TIMER_TYPE_GAME_TIME );
  BTLV_TIMER_Draw( btw, BTLV_TIMER_TYPE_COMMAND_TIME );
}

