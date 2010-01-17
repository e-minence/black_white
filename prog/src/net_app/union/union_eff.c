//==============================================================================
/**
 * @file    union_eff.c
 * @brief   ���j�I�����[���F�G�t�F�N�g��
 * @author  matsuda
 * @date    2010.01.17(��)
 */
//==============================================================================
#include <gflib.h>
#include "union_types.h"
#include "union_local.h"
#include "net_app/union/union_main.h"
#include "field/field_player.h"
#include "field/fieldmap.h"
#include "unionroom.naix"
#include "net_app/union_eff.h"


//==============================================================================
//  �萔��`
//==============================================================================
///�����Ƀt�H�[�J�X�o����ő吔
#define FOCUS_MAX     (4)
///�t�H�[�J�X�G�t�F�N�g�̃A�j�����\�[�X��
#define FOCUS_ANM_RESOURCE_MAX    (1)
///�t�H�[�J�X�G�t�F�N�g�̎���
#define FOCUS_LIFE    (30 * 3)


//==============================================================================
//  �\���̒�`
//==============================================================================
//--------------------------------------------------------------
//  �t�H�[�J�X�G�t�F�N�g
//--------------------------------------------------------------
///�t�H�[�J�X�G�t�F�N�g���[�N
typedef struct _UNION_EFF_FOCUS{
  MMDL *mmdl;           ///�t�H�[�J�X�Ώۂ̓��샂�f���ւ̃|�C���^
  GFL_G3D_RND *rnder;   //G3D�����_�[
  GFL_G3D_ANM *anm[FOCUS_ANM_RESOURCE_MAX];     //G3D�A�j���[�V�����n���h��
  GFL_G3D_OBJ *g3dobj;  //G3D�I�u�W�F�N�g
  u16 eff_no;           ///<���Ԗڂɐ������ꂽ�G�t�F�N�g��
  u16 timer;            ///<��������Ă���̃t���[�������J�E���g
}UNION_EFF_FOCUS;

///�t�H�[�J�X�G�t�F�N�g���䃏�[�N
typedef struct _UNION_EFF_FOCUS_MANAGER{
  UNION_EFF_FOCUS focus[FOCUS_MAX];   ///�t�H�[�J�X�G�t�F�N�g���[�N
  GFL_G3D_RES *res_nsbmd;
  GFL_G3D_RES *res_anm[FOCUS_ANM_RESOURCE_MAX];
  u16 eff_no_count;                   ///<���G�t�F�N�g���쐬���Ă��������J�E���g
  u16 padding;
}UNION_EFF_FOCUS_MANAGER;

//--------------------------------------------------------------
//  �G�t�F�N�g�S�̐���
//--------------------------------------------------------------
///���j�I���G�t�F�N�g����V�X�e�����[�N
struct _UNION_EFF_SYSTEM{
  UNION_EFF_FOCUS_MANAGER focus_man;    ///<�t�H�[�J�X�G�t�F�N�g���䃏�[�N
};


//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void UnionEff_Focus_ResourceSet(UNION_EFF_FOCUS_MANAGER *focus_man, HEAPID heap_id);
static void UnionEff_Focus_ResourceDelete(UNION_EFF_FOCUS_MANAGER *focus_man);
static void UnionEff_Focus_Setup(UNION_EFF_FOCUS_MANAGER *focus_man, MMDL *mmdl);
static void UnionEff_Focus_Create(UNION_EFF_FOCUS_MANAGER *focus_man, UNION_EFF_FOCUS *focus, MMDL *mmdl, u16 eff_no);
static void UnionEff_Focus_Delete(UNION_EFF_FOCUS *focus);
static void UnionEff_Focus_Update(UNION_EFF_FOCUS *focus);
static void UnionEff_Focus_Draw( UNION_EFF_FOCUS *focus, GFL_G3D_OBJSTATUS *status );
static void UnionEff_Focus_DeleteAll(UNION_EFF_FOCUS_MANAGER *focus_man);
static void UnionEff_Focus_UpdateAll(UNION_EFF_FOCUS_MANAGER *focus_man);
static void UnionEff_Focus_DrawAll( UNION_EFF_FOCUS_MANAGER *focus_man);
static UNION_EFF_FOCUS * _SearchFocusWork(UNION_EFF_FOCUS_MANAGER *focus_man);



//==============================================================================
//
//  �V�X�e���n
//
//==============================================================================
//==================================================================
/**
 * ���j�I���G�t�F�N�g����V�X�e���Z�b�g�A�b�v
 *
 * @param   heap_id		���j�I���G�t�F�N�g����V�X�e����Alloc����q�[�vID
 *
 * @retval  UNION_EFF_SYSTEM *		�쐬���ꂽ���j�I���G�t�F�N�g����V�X�e���ւ̃|�C���^
 */
//==================================================================
UNION_EFF_SYSTEM * UNION_EFF_SystemSetup(HEAPID heap_id)
{
  UNION_EFF_SYSTEM *unieff;
  
  unieff = GFL_HEAP_AllocClearMemory(heap_id, sizeof(UNION_EFF_SYSTEM));

  UnionEff_Focus_ResourceSet(&unieff->focus_man, heap_id);

  return unieff;
}

//==================================================================
/**
 * ���j�I���G�t�F�N�g����V�X�e���폜
 *
 * @param   unieff		���j�I���G�t�F�N�g����V�X�e���ւ̃|�C���^
 */
//==================================================================
void UNION_EFF_SystemExit(UNION_EFF_SYSTEM *unieff)
{
  if(unieff == NULL){
    return;
  }

  UnionEff_Focus_DeleteAll(&unieff->focus_man);
  UnionEff_Focus_ResourceDelete(&unieff->focus_man);
  
  GFL_HEAP_FreeMemory(unieff);
}

//==================================================================
/**
 * ���j�I���G�t�F�N�g����V�X�e���X�V����
 *
 * @param   unieff		���j�I���G�t�F�N�g����V�X�e���ւ̃|�C���^
 */
//==================================================================
void UNION_EFF_SystemUpdate(UNION_EFF_SYSTEM *unieff)
{
  if(unieff == NULL){
    return;
  }
  
  UnionEff_Focus_UpdateAll(&unieff->focus_man);
}

//==================================================================
/**
 * ���j�I���G�t�F�N�g����V�X�e���`�揈��
 *
 * @param   unieff		���j�I���G�t�F�N�g����V�X�e���ւ̃|�C���^
 */
//==================================================================
void UNION_EFF_SystemDraw(UNION_EFF_SYSTEM *unieff)
{
  if(unieff == NULL){
    return;
  }
  
  UnionEff_Focus_DrawAll(&unieff->focus_man);
}




//==============================================================================
//
//  �A�v���n
//
//==============================================================================
//==================================================================
/**
 * �Ώۂ̓��샂�f���Ƀt�H�[�J�X�G�t�F�N�g�����N�G�X�g����
 *
 * @param   unieff		
 * @param   mmdl		  �t�H�[�J�X����Ώۓ��샂�f���ւ̃|�C���^
 */
//==================================================================
void UnionEff_App_ReqFocus(UNION_EFF_SYSTEM *unieff, MMDL *mmdl)
{
  if(unieff == NULL){
    return;
  }
  
  UnionEff_Focus_Setup(&unieff->focus_man, mmdl);
}




//==============================================================================
//
//  ���[�J��
//
//==============================================================================

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�̃��\�[�X�o�^
 *
 * @param   focus_man		
 * @param   heap_id
 */
//--------------------------------------------------------------
static void UnionEff_Focus_ResourceSet(UNION_EFF_FOCUS_MANAGER *focus_man, HEAPID heap_id)
{
  ARCHANDLE *handle;
  BOOL ret;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_UNION, heap_id);

  focus_man->res_nsbmd = GFL_G3D_CreateResourceHandle( handle, NARC_unionroom_sentaku_nsbmd );
  focus_man->res_anm[0] = GFL_G3D_CreateResourceHandle( handle, NARC_unionroom_sentaku_nsbta );

  ret = GFL_G3D_TransVramTexture( focus_man->res_nsbmd );
  GF_ASSERT( ret );
  
  GFL_ARC_CloseDataHandle(handle);
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�̃��\�[�X�j��
 *
 * @param   focus_man		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_ResourceDelete(UNION_EFF_FOCUS_MANAGER *focus_man)
{
  int i;
  
  GFL_G3D_FreeVramTexture(focus_man->res_nsbmd);
  
  for(i = 0; i < FOCUS_ANM_RESOURCE_MAX; i++){
    GFL_G3D_DeleteResource(focus_man->res_anm[i]);
    focus_man->res_anm[i] = NULL;
  }
  GFL_G3D_DeleteResource(focus_man->res_nsbmd);
  focus_man->res_nsbmd = NULL;
}

//--------------------------------------------------------------
/**
 * �󂫗̈���������ăt�H�[�J�X�G�t�F�N�g���쐬����
 *
 * @param   focus_man		
 * @param   mmdl		    �t�H�[�J�X�Ώۂ̓��샂�f���ւ̃|�C���^
 */
//--------------------------------------------------------------
static void UnionEff_Focus_Setup(UNION_EFF_FOCUS_MANAGER *focus_man, MMDL *mmdl)
{
  UNION_EFF_FOCUS *focus;
  
  GF_ASSERT(focus_man->res_nsbmd != NULL);
  
  focus = _SearchFocusWork(focus_man);
  UnionEff_Focus_Create(focus_man, focus, mmdl, focus_man->eff_no_count);
  focus_man->eff_no_count++;
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�쐬
 *
 * @param   focus		  �t�H�[�J�X�G�t�F�N�g���[�N�ւ̃|�C���^
 * @param   mmdl		  �t�H�[�J�X�Ώۂ̓��샂�f���ւ̃|�C���^
 * @param   eff_no		�G�t�F�N�g�ԍ�
 */
//--------------------------------------------------------------
static void UnionEff_Focus_Create(UNION_EFF_FOCUS_MANAGER *focus_man, UNION_EFF_FOCUS *focus, MMDL *mmdl, u16 eff_no)
{
  int i;

  GFL_STD_MemClear(focus, sizeof(UNION_EFF_FOCUS));

  focus->mmdl = mmdl;
  focus->eff_no = eff_no;
  
  focus->rnder = GFL_G3D_RENDER_Create( focus_man->res_nsbmd, 0, focus_man->res_nsbmd );
  for(i = 0; i < FOCUS_ANM_RESOURCE_MAX; i++){
    focus->anm[i] = GFL_G3D_ANIME_Create( focus->rnder, focus_man->res_anm[i], 0 );
  }
  
  focus->g3dobj = GFL_G3D_OBJECT_Create( focus->rnder, focus->anm, FOCUS_ANM_RESOURCE_MAX );
  
  for(i = 0; i < FOCUS_ANM_RESOURCE_MAX; i++){
    GFL_G3D_OBJECT_EnableAnime( focus->g3dobj, i );
  }
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�폜
 *
 * @param   focus		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_Delete(UNION_EFF_FOCUS *focus)
{
  int i;
  
  for(i = 0; i < FOCUS_ANM_RESOURCE_MAX; i++){
    GFL_G3D_ANIME_Delete( focus->anm[i] );
  }
  GFL_G3D_OBJECT_Delete( focus->g3dobj );
	GFL_G3D_RENDER_Delete( focus->rnder );

  GFL_STD_MemClear(focus, sizeof(UNION_EFF_FOCUS));
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�X�V
 *
 * @param   focus		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_Update(UNION_EFF_FOCUS *focus)
{
  GFL_G3D_OBJECT_LoopAnimeFrame(focus->g3dobj, 0, FX32_ONE);
  focus->timer++;
  if(focus->timer > FOCUS_LIFE){
    UnionEff_Focus_Delete(focus);
  }
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g�`��
 *
 * @param   unieff		
 * @param   status		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_Draw( UNION_EFF_FOCUS *focus, GFL_G3D_OBJSTATUS *status )
{
  MMDL_GetVectorPos( focus->mmdl, &status->trans );
  GFL_G3D_DRAW_DrawObjectCullingON( focus->g3dobj, status );
}

//--------------------------------------------------------------
/**
 * �S�t�H�[�J�X�G�t�F�N�g�폜
 *
 * @param   focus_man		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_DeleteAll(UNION_EFF_FOCUS_MANAGER *focus_man)
{
  int i;
  
  for(i = 0; i < FOCUS_MAX; i++){
    if(focus_man->focus[i].g3dobj != NULL){
      UnionEff_Focus_Delete(&focus_man->focus[i]);
    }
  }
}

//--------------------------------------------------------------
/**
 * �S�t�H�[�J�X�G�t�F�N�g�X�V
 *
 * @param   focus_man		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_UpdateAll(UNION_EFF_FOCUS_MANAGER *focus_man)
{
  int i;
  
  for(i = 0; i < FOCUS_MAX; i++){
    if(focus_man->focus[i].g3dobj != NULL){
      if(MMDL_CheckStatusBitUse(focus_man->focus[i].mmdl) == TRUE){
        UnionEff_Focus_Update(&focus_man->focus[i]);
      }
      else{
        UnionEff_Focus_Delete(&focus_man->focus[i]);
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �S�t�H�[�J�X�G�t�F�N�g�`��
 *
 * @param   focus_man		
 */
//--------------------------------------------------------------
static void UnionEff_Focus_DrawAll( UNION_EFF_FOCUS_MANAGER *focus_man)
{
  int i;
  GFL_G3D_OBJSTATUS status = {{0},{FX32_ONE,FX32_ONE,FX32_ONE},{0}};
  
  MTX_Identity33( &status.rotate );
  
  for(i = 0; i < FOCUS_MAX; i++){
    if(focus_man->focus[i].g3dobj != NULL 
        && MMDL_CheckStatusBitUse(focus_man->focus[i].mmdl) == TRUE){
      UnionEff_Focus_Draw(&focus_man->focus[i], &status);
    }
  }
}

//--------------------------------------------------------------
/**
 * �t�H�[�J�X�G�t�F�N�g���[�N�̋󂫗̈������
 *
 * @param   focus_man		
 *
 * @retval  UNION_EFF_FOCUS *		�󂢂Ă����t�H�[�J�X�G�t�F�N�g���[�N�ւ̃|�C���^
 *
 * �󂫂������ꍇ�͍ł��Â��G�t�F�N�g���[�N���폜���āA�����̃��[�N�|�C���^��Ԃ��܂�
 */
//--------------------------------------------------------------
static UNION_EFF_FOCUS * _SearchFocusWork(UNION_EFF_FOCUS_MANAGER *focus_man)
{
  int i, old_no = 0;
  
  for(i = 0; i < FOCUS_MAX; i++){
    if(focus_man->focus[i].g3dobj == NULL){
      return &focus_man->focus[i];
    }
    if(focus_man->focus[i].eff_no < focus_man->focus[old_no].eff_no){
      old_no = i;
    }
  }
  
  //�󂫂������ꍇ�͍ł��Â��t�H�[�J�X�G�t�F�N�g���폜���āA������Ԃ�
  UnionEff_Focus_Delete(&focus_man->focus[old_no]);
  return &focus_man->focus[old_no];
}

