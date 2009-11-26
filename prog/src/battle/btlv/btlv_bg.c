
//============================================================================================
/**
 * @file  btlv_bg.c
 * @brief �퓬�G�t�F�N�gBG����
 * @author  soga
 * @date  2009.11.06
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"

#include "btlv_effect.h"
#include "btlv_bg.h"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================
struct _BTLV_BG_WORK
{
  GFL_TCBSYS*     tcb_sys;
  u32             bg_tcb_move_execute :1;
  u32                                 :31;
  HEAPID          heapID;
};

typedef struct
{
  BTLV_BG_WORK*     bbw;
  VecFx32           now_value;
  EFFTOOL_MOVE_WORK emw;
}BTLV_BG_TCB_WORK;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_BG_TCBInitialize( BTLV_BG_WORK *bbw, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func );
static  void  TCB_BTLV_BG_Move( GFL_TCB *tcb, void *work );

//============================================================================================
/**
 *  �V�X�e��������
 *
 * @param[in] vramBank  �������ɕK�v��VRAMBANK�}�b�v
 * @param[in] heapID    �q�[�vID
 *
 * @retval  bclw  BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
BTLV_BG_WORK*  BTLV_BG_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID )
{
  BTLV_BG_WORK *bbw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_BG_WORK ) );

  bbw->tcb_sys = tcb_sys;
  bbw->heapID  = heapID;

  return bbw;
}

//============================================================================================
/**
 *  �V�X�e���I��
 *
 * @param[in] bclw  BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_BG_Exit( BTLV_BG_WORK *bbw )
{
  GFL_HEAP_FreeMemory( bbw );
}

//============================================================================================
/**
 *  ���W�ړ�
 *
 * @param[in] bbw       BTLV_BG_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �U�����̗����ʒu�i�X�N���[���̌��������߂�̂Ɏg�p�j
 * @param[in] type      �ړ��^�C�v
 * @param[in] scr_x     �ړ��^�C�v�ɂ��Ӗ����ω�
 * @param[in] scr_y     �ړ��^�C�v�ɂ��Ӗ����ω�
 *                      EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ړ���
 *                      EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame     �ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait      �ړ��E�G�C�g
 * @param[in] count     �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_BG_MovePosition( BTLV_BG_WORK *bbw, int position, int type, int scr_x, int scr_y, int frame, int wait, int count )
{
  VecFx32       start_fx32;
  VecFx32       pos_fx32;

  pos_fx32.x = scr_x << FX32_SHIFT;
  pos_fx32.y = scr_y << FX32_SHIFT;
  pos_fx32.z = 0;

  start_fx32.x = GFL_BG_GetScrollX( GFL_BG_FRAME3_M ) << FX32_SHIFT;
  start_fx32.y = GFL_BG_GetScrollY( GFL_BG_FRAME3_M ) << FX32_SHIFT;
  start_fx32.z = 0;

  //��ɃX�N���[��������Ƃ��̓|�P�����̗����ʒu�ŕ��������肷��
  if( type == BTLEFF_BG_SCROLL_EVERY )
  { 
    //�|�P�����̎����̕����Ł{�|�����肷��
    if( position & 1 )
    {
      pos_fx32.x *= -1;
    }
  }

  //�ړ��̕�Ԃ͑��Ύw��Ƃ���
  if( type == EFFTOOL_CALCTYPE_INTERPOLATION )
  { 
    pos_fx32.x += start_fx32.x;
    pos_fx32.y += start_fx32.y;
    pos_fx32.z += start_fx32.z;
  }

  BTLV_BG_TCBInitialize( bbw, type, &start_fx32, &pos_fx32, frame, wait, count, TCB_BTLV_BG_Move );
  bbw->bg_tcb_move_execute = 1;
}

//============================================================================================
/**
 *  �^�X�N���N�������`�F�b�N
 *
 * @param[in] bbw    BTLV_CLACT_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in] index   CLWK�̊Ǘ��C���f�b�N�X
 *
 * @retval: TRUE:�N�����@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_BG_CheckTCBExecute( BTLV_BG_WORK *bbw )
{
  return ( bbw->bg_tcb_move_execute != 0 );
}

//============================================================================================
/**
 *  BG����n�^�X�N����������
 */
//============================================================================================
static  void  BTLV_BG_TCBInitialize( BTLV_BG_WORK *bbw, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func )
{
  BTLV_BG_TCB_WORK *bbtw = GFL_HEAP_AllocMemory( bbw->heapID, sizeof( BTLV_BG_TCB_WORK ) );

  bbtw->bbw               = bbw;
  bbtw->now_value.x       = start->x;
  bbtw->now_value.y       = start->y;
  bbtw->now_value.z       = start->z;
  bbtw->emw.move_type     = type;
  bbtw->emw.vec_time      = frame;
  bbtw->emw.vec_time_tmp  = frame;
  bbtw->emw.wait          = 0;
  bbtw->emw.wait_tmp      = wait;
  bbtw->emw.count         = count * 2;
  bbtw->emw.start_value.x = start->x;
  bbtw->emw.start_value.y = start->y;
  bbtw->emw.start_value.z = start->z;
  bbtw->emw.end_value.x   = end->x;
  bbtw->emw.end_value.y   = end->y;
  bbtw->emw.end_value.z   = end->z;

  switch( type ){
  case EFFTOOL_CALCTYPE_DIRECT:         //���ڃ|�W�V�����Ɉړ�
    break;
  case EFFTOOL_CALCTYPE_INTERPOLATION:  //�ړ���܂ł��Ԃ��Ȃ���ړ�
    BTLV_EFFTOOL_CalcMoveVector( &bbtw->emw.start_value, end, &bbtw->emw.vector, FX32_CONST( frame ) );
    break;
  case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG: //�w�肵����Ԃ������ړ�
    bbtw->emw.vec_time_tmp  *= 2;
    bbtw->emw.count         *= 2;
    //fall through
  case EFFTOOL_CALCTYPE_ROUNDTRIP:      //�w�肵����Ԃ������ړ�
    bbtw->emw.vector.x = FX_Div( end->x, FX32_CONST( frame ) );
    bbtw->emw.vector.y = FX_Div( end->y, FX32_CONST( frame ) );
    bbtw->emw.vector.z = FX_Div( end->z, FX32_CONST( frame ) );
    break;
  }
  GFL_TCB_AddTask( bbw->tcb_sys, func, bbtw, 0 );
}

//============================================================================================
/**
 *  BG�ړ��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_BG_Move( GFL_TCB *tcb, void *work )
{
  BTLV_BG_TCB_WORK *bbtw = ( BTLV_BG_TCB_WORK * )work;
  BTLV_BG_WORK *bbw = bbtw->bbw;
  int   scr_x, scr_y;
  BOOL  ret;

  if( bbtw->emw.move_type == BTLEFF_BG_SCROLL_EVERY )
  { 
    bbtw->now_value.x += bbtw->emw.end_value.x;
    bbtw->now_value.y += bbtw->emw.end_value.y;
    if( bbtw->emw.vec_time == 0 )
    { 
      ret = TRUE;
    }
    else
    { 
      bbtw->emw.vec_time--;
      ret = FALSE;
    }
  }
  else
  { 
    ret = BTLV_EFFTOOL_CalcParam( &bbtw->emw, &bbtw->now_value );
  }
  scr_x = bbtw->now_value.x >> FX32_SHIFT;
  scr_y = bbtw->now_value.y >> FX32_SHIFT;
  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, scr_x );
  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, scr_y );
  if( ret == TRUE )
  {
    bbw->bg_tcb_move_execute = 0;
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

