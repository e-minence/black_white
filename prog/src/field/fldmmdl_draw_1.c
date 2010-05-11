//======================================================================
/**
 * @file  fldmmdl_draw_1.c
 * @brief  �t�B�[���h���샂�f�� �`�揈������1�@���f���֘A
 * @author  kagaya
 * @date  05.07.13
 */
//======================================================================
#include "fldmmdl.h"
#include "fldmmdl_procdraw.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/// DRAW_MODEL_WORK
//--------------------------------------------------------------
typedef struct
{
  FLD_G3DOBJ_OBJIDX obj_idx;
}DRAW_MODEL_WORK;

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �`�揈���@���f���`��@�\���A�A�j�����[�v�Đ�
//======================================================================
//--------------------------------------------------------------
/**
 * ���f���`��@������
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawModel_Init( MMDL * mmdl )
{
  DRAW_MODEL_WORK *work;
  
  work = MMDL_InitDrawProcWork( mmdl, sizeof(DRAW_MODEL_WORK) );
  work->obj_idx = MMDL_G3DOBJCONT_AddObject( mmdl );
}

//--------------------------------------------------------------
/**
 * ���f���`��@�`��
 * @param  mmdl    MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawModel_Draw( MMDL * mmdl )
{
  u16 code;
  VecFx32 pos;
  BOOL flag;
  DRAW_MODEL_WORK *work;
  FLD_G3DOBJ_CTRL *objctrl;
  
  work = MMDL_GetDrawProcWork( mmdl );
  objctrl = MMDL_G3DOBJCONT_GetFldG3dObjCtrl( mmdl );
  
  MMDL_GetDrawVectorPos( mmdl, &pos );
  FLD_G3DOBJ_CTRL_SetObjPos( objctrl, work->obj_idx, &pos );
  
  flag = FALSE;
  if( MMDL_CheckStatusBitVanish(mmdl) == TRUE ){
    flag = TRUE;
  }
  FLD_G3DOBJ_CTRL_SetObjVanishFlag( objctrl, work->obj_idx, flag );
  
  if( MMDL_CheckDrawPause(mmdl) == FALSE ){
    FLD_G3DOBJ_CTRL_LoopAnimeObject( objctrl, work->obj_idx, FX32_ONE );
  }
}

//--------------------------------------------------------------
/**
 * ���f���`��@�폜
 * @param  mmdl  MMDL * 
 * @retval  nothing
 */
//--------------------------------------------------------------
static void MMdl_DrawModel_Delete( MMDL * mmdl )
{
  DRAW_MODEL_WORK *work = MMDL_GetDrawProcWork( mmdl );
  MMDL_G3DOBJCONT_DeleteObject( mmdl, work->obj_idx );
}

//--------------------------------------------------------------
/**
 * ���f���`��@�ޔ�
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawModel_Push( MMDL * mmdl )
{
  //����A�폜�Ă�ŏI��
  MMdl_DrawModel_Delete( mmdl );
}

//--------------------------------------------------------------
/**
 * ���f���`��@���A
 * �ޔ������������ɍĕ`��B
 * @param  mmdl    MMDL * 
 * @retval  int      TRUE=����������
 */
//--------------------------------------------------------------
static void MMdl_DrawModel_Pop( MMDL * mmdl )
{
  //����A�������Ă�ŏI��
  MMdl_DrawModel_Init( mmdl );
}

//--------------------------------------------------------------
///  �`�揈���@���f���`��@�܂Ƃ�
//--------------------------------------------------------------
const MMDL_DRAW_PROC_LIST DATA_MMDL_DRAWPROCLIST_Model =
{
  MMdl_DrawModel_Init,
  MMdl_DrawModel_Draw,
  MMdl_DrawModel_Delete,
  MMdl_DrawModel_Push,
  MMdl_DrawModel_Pop,
  NULL,
};
