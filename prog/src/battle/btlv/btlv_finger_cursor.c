
//============================================================================================
/**
 * @file  btlv_finger_cursor.c
 * @brief �w�J�[�\��
 * @author  soga
 * @date  2010.01.28
 */
//============================================================================================
#include <gflib.h>

#include "btlv_finger_cursor.h"
#include "btlv_effect.h"

#include "arc_def.h"
#include "battle/battgra_wb.naix"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

enum
{
  BTLV_FINGER_CURSOR_SRC_PALNO      = 0,
  BTLV_FINGER_CURSOR_PAL_TRANS_SIZE = 1,

  BTLV_FINGER_CURSOR_MAX = 1,
};

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

typedef struct
{
  int         seq_no;
  int         count;
  int         frame;
  int         wait;
  BOOL        anm_end_flag;
}BTLV_FINGER_CURSOR_PARAM;

struct _BTLV_FINGER_CURSOR_WORK
{
  GFL_CLUNIT* clunit;
  GFL_CLWK*   clwk;
  GFL_TCB*    tcb;
  u32         charID;
  u32         plttID;
  u32         cellID;

  BTLV_FINGER_CURSOR_PARAM  bfcp;

  HEAPID      heapID;
};

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  void  BTLV_FINGER_CURSOR_CallBackFunc( u32 param, fx32 currentFrame );
static  void  TCB_BTLV_FINGER_CURSOR_WatchAnmEndFlag( GFL_TCB* tcb, void* work );

//============================================================================================
/**
 *  @brief  �V�X�e��������
 *
 *  @param  pfd     PaletteFadePTR
 *  @param  pal     �w�J�[�\���p���b�g�̓ǂݍ��ݐ�
 *  @param  heapID  �q�[�vID
 *
 *  @retval �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
BTLV_FINGER_CURSOR_WORK*  BTLV_FINGER_CURSOR_Init( PALETTE_FADE_PTR pfd, int pal, HEAPID heapID )
{
  BTLV_FINGER_CURSOR_WORK* bfcw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_FINGER_CURSOR_WORK ) );
  ARCHANDLE*  handle  = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, GFL_HEAP_LOWID(heapID) );

  bfcw->heapID  = heapID;

  //���\�[�X�ǂݍ���
  bfcw->charID = GFL_CLGRP_CGR_Register( handle, NARC_battgra_wb_finger_cursor_NCGR, FALSE, CLSYS_DRAW_SUB, heapID );
  bfcw->cellID = GFL_CLGRP_CELLANIM_Register( handle, NARC_battgra_wb_finger_cursor_NCER,
                                              NARC_battgra_wb_finger_cursor_NANR, heapID );
  bfcw->plttID = GFL_CLGRP_PLTT_RegisterEx( handle, NARC_battgra_wb_finger_cursor_NCLR, CLSYS_DRAW_SUB, pal * 0x20,
                                            BTLV_FINGER_CURSOR_SRC_PALNO,
                                            BTLV_FINGER_CURSOR_PAL_TRANS_SIZE,
                                            heapID );
  PaletteWorkSet_VramCopy( pfd, FADE_SUB_OBJ,
                           GFL_CLGRP_PLTT_GetAddr( bfcw->plttID, CLSYS_DRAW_SUB ) / 2, 0x20 );

  //CLACT������
  bfcw->clunit = GFL_CLACT_UNIT_Create( BTLV_FINGER_CURSOR_MAX, 0, heapID );

  GFL_ARC_CloseDataHandle( handle );

  //�A�j���t���O�Ď�TCB�Z�b�g
  bfcw->tcb = GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), TCB_BTLV_FINGER_CURSOR_WatchAnmEndFlag, bfcw, 0 );

  return bfcw;
}

//============================================================================================
/**
 *  @brief  �V�X�e���I��
 *
 *  @retval �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void  BTLV_FINGER_CURSOR_Exit( BTLV_FINGER_CURSOR_WORK* bfcw )
{
  GFL_TCB_DeleteTask( bfcw->tcb );

  BTLV_FINGER_CURSOR_Delete( bfcw );

  GFL_CLGRP_CGR_Release( bfcw->charID );
  GFL_CLGRP_CELLANIM_Release( bfcw->cellID );
  GFL_CLGRP_PLTT_Release( bfcw->plttID );

  GFL_CLACT_UNIT_Delete( bfcw->clunit );
  GFL_HEAP_FreeMemory( bfcw );
}

//============================================================================================
/**
 *  @brief  �V�X�e�����C�����[�v
 *
 *  @param[in]  bfcw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void  BTLV_FINGER_CURSOR_Main( BTLV_FINGER_CURSOR_WORK* bfcw )
{

}

//============================================================================================
/**
 *  @brief  �w�J�[�\������
 *
 *  @param[in]  bfcw    �V�X�e���Ǘ��\���̂̃|�C���^
 *  @param[in]  pos_x   �w�J�[�\��X���W
 *  @param[in]  pos_y   �w�J�[�\��Y���W
 *  @param[in]  count   �����A�j���Ɉڍs����܂ł̃��[�v��
 *  @param[in]  frame   �����A�j�����A����������Ɉڍs����A�j���[�V�����t���[��
 *  @param[in]  wait    �����A�j���̂��Ə�����܂ł̃E�G�C�g
 *
 * @retval  TRUE:Create���� FALSE:Create���s�i�܂��A�j�����I����Ă��Ȃ��j
 */
//============================================================================================
BOOL  BTLV_FINGER_CURSOR_Create( BTLV_FINGER_CURSOR_WORK* bfcw, int pos_x, int pos_y, int count, int frame, int wait )
{
  GFL_CLWK_AFFINEDATA cursor = {
    {
      0, 0,                   //x, y
      0, 0, 0,                //�A�j���ԍ��A�D�揇�ʁABG�v���C�I���e�B
    },
    0, 0,                     // �A�t�B�������W �A�t�B�������W
    FX32_ONE, FX32_ONE,       // �g�傘�l �g�備�l
    0,                        // ��]�p�x(0�`0xffff 0xffff��360�x)
    CLSYS_AFFINETYPE_DOUBLE,  // �㏑���A�t�B���^�C�v�iCLSYS_AFFINETYPE�j
  };

  if( bfcw->bfcp.anm_end_flag )
  {
    return FALSE;
  }

  if( bfcw->clwk )
  {
    BTLV_FINGER_CURSOR_Delete( bfcw );
    bfcw->clwk = NULL;
  }

  cursor.clwkdata.pos_x = pos_x;
  cursor.clwkdata.pos_y = pos_y;

  bfcw->clwk = GFL_CLACT_WK_CreateAffine( bfcw->clunit, bfcw->charID, bfcw->plttID, bfcw->cellID,
                                          &cursor, CLSYS_DEFREND_SUB, bfcw->heapID );

  GFL_CLACT_WK_SetAnmSeq( bfcw->clwk, 0 );
  GFL_CLACT_WK_SetAnmFrame( bfcw->clwk, 0 );
  GFL_CLACT_WK_SetAutoAnmFlag( bfcw->clwk, TRUE );
  GFL_CLACT_WK_SetDrawEnable( bfcw->clwk, TRUE );

  if( count )
  {
    bfcw->bfcp.count  = count;
  }
  else
  {
    bfcw->bfcp.count  = 1;  //�Œ�ł�1��̓��[�v����悤�ɂ���
  }

  if( wait )
  {
    bfcw->bfcp.wait  = wait;
  }
  else
  {
    bfcw->bfcp.wait  = 1;  //�Œ�ł�1�͑҂�
  }

  bfcw->bfcp.frame  = frame;

  bfcw->bfcp.seq_no = 0;
  bfcw->bfcp.anm_end_flag = FALSE;

  {
    GFL_CLWK_ANM_CALLBACK dat;
    dat.param = ( u32 )bfcw;
    dat.callback_type = CLWK_ANM_CALLBACK_TYPE_LAST_FRM;
    dat.frame_idx     = 0;
    dat.p_func        = BTLV_FINGER_CURSOR_CallBackFunc;

    GFL_CLACT_WK_StartAnmCallBack( bfcw->clwk, &dat );
  }

  return TRUE;
}

//============================================================================================
/**
 *  @brief  �w�J�[�\���폜
 *
 *  @param[in]  bfcw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
void  BTLV_FINGER_CURSOR_Delete( BTLV_FINGER_CURSOR_WORK* bfcw )
{
  if( bfcw->clwk )
  {
    GFL_CLACT_WK_Remove( bfcw->clwk );
    bfcw->clwk = NULL;
  }
}

//============================================================================================
/**
 *  @brief  �w�J�[�\���A�j���`�F�b�N
 *
 *  @param[in]  bfcw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
BOOL  BTLV_FINGER_CURSOR_CheckExecute( BTLV_FINGER_CURSOR_WORK* bfcw )
{
  return bfcw->bfcp.anm_end_flag;
}

//============================================================================================
/**
 *  @brief  �w�J�[�\���R�[���o�b�N
 *
 *  @param[in]  bfcw �V�X�e���Ǘ��\���̂̃|�C���^
 */
//============================================================================================
static  void  BTLV_FINGER_CURSOR_CallBackFunc( u32 param, fx32 currentFrame )
{
  BTLV_FINGER_CURSOR_WORK* bfcw = ( BTLV_FINGER_CURSOR_WORK* )param;

  switch( bfcw->bfcp.seq_no ){
  case 0:
    if( --bfcw->bfcp.count == 0 )
    {
      GFL_CLACT_WK_SetAnmSeq( bfcw->clwk, 1 );
      bfcw->bfcp.seq_no++;
      {
        GFL_CLWK_ANM_CALLBACK dat;
        dat.param = param;
        dat.callback_type = CLWK_ANM_CALLBACK_TYPE_SPEC_FRM;
        dat.frame_idx     = bfcw->bfcp.frame;
        dat.p_func        = BTLV_FINGER_CURSOR_CallBackFunc;

        GFL_CLACT_WK_StartAnmCallBack( bfcw->clwk, &dat );
      }
    }
    break;
  case 1:
    bfcw->bfcp.anm_end_flag = TRUE;
    bfcw->bfcp.seq_no++;
    {
      GFL_CLWK_ANM_CALLBACK dat;
      dat.param = param;
      dat.callback_type = CLWK_ANM_CALLBACK_TYPE_EVER_FRM;
      dat.frame_idx     = 0;
      dat.p_func        = BTLV_FINGER_CURSOR_CallBackFunc;

      GFL_CLACT_WK_StartAnmCallBack( bfcw->clwk, &dat );
    }
    break;
  case 2:
    GFL_CLACT_WK_SetDrawEnable( bfcw->clwk, FALSE );
    bfcw->bfcp.anm_end_flag = FALSE;
    bfcw->bfcp.seq_no++;
    break;
  case 3:
  default:
    break;
  }
}

//============================================================================================
/**
 *  @brief  anm_end_flag�Ď�TCB
 */
//============================================================================================
static  void  TCB_BTLV_FINGER_CURSOR_WatchAnmEndFlag( GFL_TCB* tcb, void* work )
{
  BTLV_FINGER_CURSOR_WORK* bfcw = ( BTLV_FINGER_CURSOR_WORK* )work;

  if( bfcw->clwk )
  {
    if( bfcw->bfcp.anm_end_flag )
    {
      if( --bfcw->bfcp.wait == 0 )
      {
        GFL_CLACT_WK_SetDrawEnable( bfcw->clwk, FALSE );
        bfcw->bfcp.anm_end_flag = FALSE;
      }
    }
  }
}

