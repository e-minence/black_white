
//============================================================================================
/**
 * @file  btlv_clact.c
 * @brief �퓬�G�t�F�N�g����
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"

#include "btlv_effect.h"
#include "btlv_clact.h"

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

struct _BTLV_CLACT_CLWK
{
  GFL_CLWK* clwk;
  u32       charID;
  u32       plttID;
  u32       cellID;
};

struct _BTLV_CLACT_WORK
{
  GFL_TCBSYS*     tcb_sys;
  GFL_CLUNIT*     clunit;
  BTLV_CLACT_CLWK bccl[ BTLV_CLACT_CLWK_MAX ];
  u32             clact_tcb_move_execute;
  u32             clact_tcb_scale_execute;
  HEAPID          heapID;
};

typedef struct
{
  BTLV_CLACT_WORK*  bclw;
  VecFx32           now_value;
  int               index;
  EFFTOOL_MOVE_WORK emw;
}BTLV_CLACT_TCB_WORK;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_CLACT_TCBInitialize( BTLV_CLACT_WORK *bclw, int index, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func );
static  void  TCB_BTLV_CLACT_Move( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_CLACT_Scale( GFL_TCB *tcb, void *work );

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
BTLV_CLACT_WORK*  BTLV_CLACT_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID )
{
  BTLV_CLACT_WORK *bclw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_CLACT_WORK ) );

  bclw->tcb_sys = tcb_sys;
  bclw->heapID  = heapID;

  bclw->clunit = GFL_CLACT_UNIT_Create( BTLV_CLACT_CLWK_MAX, 0, heapID );

  return bclw;
}

//============================================================================================
/**
 *  �V�X�e���I��
 *
 * @param[in] bclw  BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_CLACT_Exit( BTLV_CLACT_WORK *bclw )
{
  int index;

  for( index = 0 ; index < BTLV_CLACT_CLWK_MAX ; index++ )
  {
    if( bclw->bccl[ index ].clwk )
    {
      BTLV_CLACT_Delete( bclw, index );
    }
  }
  GFL_CLACT_UNIT_Delete( bclw->clunit );
  GFL_HEAP_FreeMemory( bclw );
}

//============================================================================================
/**
 *  �V�X�e�����C��
 *
 * @param[in] bclw  BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_CLACT_Main( BTLV_CLACT_WORK *bclw )
{
  GFL_CLACT_SYS_Main();
}

//============================================================================================
/**
 *  CLWK����
 *
 *  ARCDATID�́Anaix�̕��т��ANCGR�ANCLR�ANCER�ANANR�ł��邱�Ƃ����҂��Ă��܂�
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] arcID   �O���t�B�b�N���\�[�X��ARCID
 * @param[in] ncgrID  �O���t�B�b�N���\�[�X�incgr�j��ARCDATID
 * @param[in] posx    ����X���W
 * @param[in] posy    ����Y���W
 *
 * @retval  �o�^����INDEX�i���o�[
 */
//============================================================================================
int BTLV_CLACT_Add( BTLV_CLACT_WORK *bclw, ARCID arcID, ARCDATID ncgrID, s16 posx, s16 posy )
{
  return BTLV_CLACT_AddEx( bclw, arcID, ncgrID, ncgrID + 1, ncgrID + 2, ncgrID + 3, posx, posy, 0, 0 );
}

//============================================================================================
/**
 *  CLWK�����iAffine�l���ݒ�j
 *
 *  ARCDATID�́Anaix�̕��т��ANCGR�ANCLR�ANCER�ANANR�ł��邱�Ƃ����҂��Ă��܂�
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] arcID   �O���t�B�b�N���\�[�X��ARCID
 * @param[in] ncgrID  �O���t�B�b�N���\�[�X�incgr�j��ARCDATID
 * @param[in] posx    ����X���W
 * @param[in] posy    ����Y���W
 * @param[in] scalex  ����ScaleX�l
 * @param[in] scaley  ����ScaleY�l
 *
 * @retval  �o�^����INDEX�i���o�[
 */
//============================================================================================
int BTLV_CLACT_AddAffine( BTLV_CLACT_WORK *bclw, ARCID arcID, ARCDATID ncgrID, s16 posx, s16 posy, fx32 scalex, fx32 scaley )
{
  return BTLV_CLACT_AddEx( bclw, arcID, ncgrID, ncgrID + 1, ncgrID + 2, ncgrID + 3, posx, posy, scalex, scaley );
}

//============================================================================================
/**
 *  CLWK����
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] arcID   �O���t�B�b�N���\�[�X��ARCID
 * @param[in] ncgrID  �O���t�B�b�N���\�[�X�incgr�j��ARCDATID
 * @param[in] nclrID  �O���t�B�b�N���\�[�X�inclr�j��ARCDATID
 * @param[in] ncerID  �O���t�B�b�N���\�[�X�incer�j��ARCDATID
 * @param[in] nanrID  �O���t�B�b�N���\�[�X�inanr�j��ARCDATID
 * @param[in] posx    ����X���W
 * @param[in] posy    ����Y���W
 * @param[in] scalex  ����X���W
 * @param[in] scaley  ����Y���W
 *
 * @retval  index �o�^����INDEX�i���o�[
 */
//============================================================================================
int BTLV_CLACT_AddEx( BTLV_CLACT_WORK *bclw, ARCID arcID,
                      ARCDATID ncgrID, ARCDATID nclrID, ARCDATID ncerID, ARCDATID nanrID,
                      s16 posx, s16 posy, fx32 scalex, fx32 scaley )
{
  ARCHANDLE     *hdl;
  int           index;
  GFL_CLWK_AFFINEDATA CLWKAffineParam = { 
    {
      0, 0,                   //x, y
      0, 0, 1,                //�A�j���ԍ��A�D�揇�ʁABG�v���C�I���e�B
    },
		0,		                    // �A�t�B�������W
	  0,		                    // �A�t�B�������W
	  FX32_ONE,			            // �g�傘�l
	  FX32_ONE,			            // �g�備�l
	  0,                        // ��]�p�x(0�`0xffff 0xffff��360�x)
	  CLSYS_AFFINETYPE_NORMAL,  // �㏑���A�t�B���^�C�v�iCLSYS_AFFINETYPE�j
  };

  hdl = GFL_ARC_OpenDataHandle( arcID, bclw->heapID );

  for( index = 0 ; index < BTLV_CLACT_CLWK_MAX ; index++ )
  {
    if( bclw->bccl[ index ].clwk == NULL )
    {
      break;
    }
  }

  GF_ASSERT( index < BTLV_CLACT_CLWK_MAX );

  bclw->bccl[ index ].charID = GFL_CLGRP_CGR_Register( hdl, ncgrID, FALSE, CLSYS_DRAW_MAIN, bclw->heapID );
  bclw->bccl[ index ].plttID = GFL_CLGRP_PLTT_RegisterComp( hdl, nclrID, CLSYS_DRAW_MAIN, 0, bclw->heapID );
  bclw->bccl[ index ].cellID = GFL_CLGRP_CELLANIM_Register( hdl, ncerID, nanrID, bclw->heapID );

  //�p���b�g��PaletteWork�Ƀ��[�h
  PaletteWorkSet_Arc( BTLV_EFFECT_GetPfd(), arcID, nclrID, bclw->heapID, FADE_MAIN_OBJ, 0x20,
    ( GFL_CLGRP_PLTT_GetAddr( bclw->bccl[ index ].plttID, CLSYS_DRAW_MAIN ) & 0x3ff ) / 0x20 );

  CLWKAffineParam.clwkdata.pos_x = posx;
  CLWKAffineParam.clwkdata.pos_y = posy;

  if( ( scalex == 0 ) && ( scaley == 0 ) )
  { 
    bclw->bccl[ index ].clwk = GFL_CLACT_WK_Create( bclw->clunit,
                                                    bclw->bccl[ index ].charID,
                                                    bclw->bccl[ index ].plttID,
                                                    bclw->bccl[ index ].cellID,
                                                    &CLWKAffineParam.clwkdata, CLSYS_DEFREND_MAIN, bclw->heapID );
  }
  else
  { 
    CLWKAffineParam.scale_x = scalex;
    CLWKAffineParam.scale_y = scaley;
    bclw->bccl[ index ].clwk = GFL_CLACT_WK_CreateAffine( bclw->clunit,
                                                          bclw->bccl[ index ].charID,
                                                          bclw->bccl[ index ].plttID,
                                                          bclw->bccl[ index ].cellID,
                                                          &CLWKAffineParam, CLSYS_DEFREND_MAIN, bclw->heapID );
  }

  GFL_CLACT_WK_SetAutoAnmFlag( bclw->bccl[ index ].clwk, TRUE );

  GFL_ARC_CloseDataHandle( hdl );

  return index;
}

//============================================================================================
/**
 *  CLWK�̔j��
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] index   �j������CLWK�̊Ǘ��C���f�b�N�X
 */
//============================================================================================
void  BTLV_CLACT_Delete( BTLV_CLACT_WORK *bclw, int index )
{
  GF_ASSERT( bclw );
  GF_ASSERT( bclw->bccl[ index ].clwk != NULL );

  GFL_CLGRP_CGR_Release( bclw->bccl[ index ].charID );
  GFL_CLGRP_PLTT_Release( bclw->bccl[ index ].plttID );
  GFL_CLGRP_CELLANIM_Release( bclw->bccl[ index ].cellID );

  GFL_CLACT_WK_Remove( bclw->bccl[ index ].clwk );

  bclw->bccl[ index ].clwk = NULL;
}

//============================================================================================
/**
 *  ���W�ړ�
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in] index   �ړ�����CLWK�̃C���f�b�N�X
 * @param[in] type    �ړ��^�C�v
 * @param[in] pos     �ړ��^�C�v�ɂ��Ӗ����ω�
 *                    EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ړ���
 *                    EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   �ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    �ړ��E�G�C�g
 * @param[in] count   �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_CLACT_MovePosition( BTLV_CLACT_WORK *bclw, int index, int type, GFL_CLACTPOS *pos, int frame, int wait, int count )
{
  GFL_CLACTPOS  start;
  VecFx32       start_fx32;
  VecFx32       pos_fx32;

  pos_fx32.x = pos->x << FX32_SHIFT;
  pos_fx32.y = pos->y << FX32_SHIFT;
  pos_fx32.z = 0;

  GFL_CLACT_WK_GetPos( bclw->bccl[ index ].clwk, &start, CLSYS_DEFREND_MAIN );
  start_fx32.x = start.x << FX32_SHIFT;
  start_fx32.y = start.y << FX32_SHIFT;
  start_fx32.z = 0;

  //�ړ��̕�Ԃ͑��Ύw��Ƃ���
  if( type == EFFTOOL_CALCTYPE_INTERPOLATION )
  { 
    pos_fx32.x += start_fx32.x;
    pos_fx32.y += start_fx32.y;
    pos_fx32.z += start_fx32.z;
  }

  BTLV_CLACT_TCBInitialize( bclw, index, type, &start_fx32, &pos_fx32, frame, wait, count, TCB_BTLV_CLACT_Move );
  bclw->clact_tcb_move_execute |= BTLV_EFFTOOL_No2Bit( index );
}

//============================================================================================
/**
 * @brief OBJ�g�k
 *
 * @param[in] bclw    BTLV_CLACT�Ǘ����[�N�ւ̃|�C���^
 * @param[in] index   �g�k����CLWK�̃C���f�b�N�X
 * @param[in] type    �g�k�^�C�v
 * @param[in] scale   �g�k�^�C�v�ɂ��Ӗ����ω�
 *                    EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȃX�P�[���l
 *                    EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   �g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    �g�k�E�G�C�g
 * @param[in] count   �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_CLACT_MoveScale( BTLV_CLACT_WORK *bclw, int index, int type, VecFx32 *scale, int frame, int wait, int count )
{
  VecFx32 start;

  start.x = FX32_ONE;
  start.y = FX32_ONE;
  start.z = FX32_ONE;

  BTLV_CLACT_TCBInitialize( bclw, index, type, &start, scale, frame, wait, count, TCB_BTLV_CLACT_Scale );
  bclw->clact_tcb_scale_execute |= BTLV_EFFTOOL_No2Bit( index );
}


//============================================================================================
/**
 *  �A�j���V�[�P���X�Z�b�g
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] index   CLWK�̊Ǘ��C���f�b�N�X
 * @param[in] anm_no  �Z�b�g����A�j���V�[�P���X�i���o�[
 */
//============================================================================================
void  BTLV_CLACT_SetAnime( BTLV_CLACT_WORK *bclw, int index, int anm_no )
{
  GFL_CLACT_WK_SetAnmSeq( bclw->bccl[ index ].clwk, anm_no );
}

//============================================================================================
/**
 *  �^�X�N���N�������`�F�b�N
 *
 * @param[in] bclw    BTLV_CLACT_WORK�Ǘ����[�N�ւ̃|�C���^
 * @param[in] index   CLWK�̊Ǘ��C���f�b�N�X
 *
 * @retval: TRUE:�N�����@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_CLACT_CheckTCBExecute( BTLV_CLACT_WORK *bclw, int index )
{
  if( bclw->bccl[ index ].clwk == NULL )
  {
    return FALSE;
  }
  return ( ( bclw->clact_tcb_move_execute & BTLV_EFFTOOL_No2Bit( index ) ) ||
           ( GFL_CLACT_WK_CheckAnmActive( bclw->bccl[ index ].clwk ) ) );
}


//============================================================================================
/**
 *  CLWK����n�^�X�N����������
 */
//============================================================================================
static  void  BTLV_CLACT_TCBInitialize( BTLV_CLACT_WORK *bclw, int index, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func )
{
  BTLV_CLACT_TCB_WORK *bctw = GFL_HEAP_AllocMemory( bclw->heapID, sizeof( BTLV_CLACT_TCB_WORK ) );

  GF_ASSERT( index < BTLV_CLACT_CLWK_MAX );

  bctw->bclw              = bclw;
  bctw->index             = index;
  bctw->now_value.x       = start->x;
  bctw->now_value.y       = start->y;
  bctw->now_value.z       = start->z;
  bctw->emw.move_type     = type;
  bctw->emw.vec_time      = frame;
  bctw->emw.vec_time_tmp  = frame;
  bctw->emw.wait          = 0;
  bctw->emw.wait_tmp      = wait;
  bctw->emw.count         = count * 2;
  bctw->emw.start_value.x = start->x;
  bctw->emw.start_value.y = start->y;
  bctw->emw.start_value.z = start->z;
  bctw->emw.end_value.x   = end->x;
  bctw->emw.end_value.y   = end->y;
  bctw->emw.end_value.z   = end->z;

  switch( type ){
  case EFFTOOL_CALCTYPE_DIRECT:         //���ڃ|�W�V�����Ɉړ�
    break;
  case EFFTOOL_CALCTYPE_INTERPOLATION:  //�ړ���܂ł��Ԃ��Ȃ���ړ�
    BTLV_EFFTOOL_CalcMoveVector( &bctw->emw.start_value, end, &bctw->emw.vector, FX32_CONST( frame ) );
    break;
  case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG: //�w�肵����Ԃ������ړ�
    bctw->emw.vec_time_tmp  *= 2;
    bctw->emw.count         *= 2;
    //fall through
  case EFFTOOL_CALCTYPE_ROUNDTRIP:      //�w�肵����Ԃ������ړ�
    bctw->emw.vector.x = FX_Div( end->x, FX32_CONST( frame ) );
    bctw->emw.vector.y = FX_Div( end->y, FX32_CONST( frame ) );
    bctw->emw.vector.z = FX_Div( end->z, FX32_CONST( frame ) );
    break;
  }
  GFL_TCB_AddTask( bclw->tcb_sys, func, bctw, 0 );
}

//============================================================================================
/**
 *  CLWK�ړ��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_CLACT_Move( GFL_TCB *tcb, void *work )
{
  BTLV_CLACT_TCB_WORK *bctw = ( BTLV_CLACT_TCB_WORK * )work;
  BTLV_CLACT_WORK *bclw = bctw->bclw;
  GFL_CLACTPOS  now_pos;
  BOOL          ret;

  ret = BTLV_EFFTOOL_CalcParam( &bctw->emw, &bctw->now_value );
  now_pos.x = bctw->now_value.x >> FX32_SHIFT;
  now_pos.y = bctw->now_value.y >> FX32_SHIFT;
  GFL_CLACT_WK_SetPos( bclw->bccl[ bctw->index ].clwk, &now_pos, CLSYS_DEFREND_MAIN );
  if( ret == TRUE )
  {
    bclw->clact_tcb_move_execute &= ( BTLV_EFFTOOL_No2Bit( bctw->index ) ^ 0xffffffff );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 * @brief CLWK�g�k�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_CLACT_Scale( GFL_TCB *tcb, void *work )
{
  BTLV_CLACT_TCB_WORK *bctw = ( BTLV_CLACT_TCB_WORK * )work;
  BTLV_CLACT_WORK *bclw = bctw->bclw;
  GFL_CLSCALE now_scale;
  BOOL  ret;

  ret = BTLV_EFFTOOL_CalcParam( &bctw->emw, &bctw->now_value );
  now_scale.x = bctw->now_value.x;
  now_scale.y = bctw->now_value.y;
  GFL_CLACT_WK_SetAffineParam( bclw->bccl[ bctw->index ].clwk, CLSYS_AFFINETYPE_NORMAL );
  GFL_CLACT_WK_SetScale( bclw->bccl[ bctw->index ].clwk, &now_scale );
  if( ret == TRUE ){
    bclw->clact_tcb_scale_execute &= ( BTLV_EFFTOOL_No2Bit( bctw->index ) ^ 0xffffffff );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

