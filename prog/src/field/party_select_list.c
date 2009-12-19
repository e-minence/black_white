//==============================================================================
/**
 * @file    party_select_list.c
 * @brief   �莝�� or �o�g���{�b�N�X �I�����X�g
 * @author  matsuda
 * @date    2009.12.18(��)
 */
//==============================================================================
#include <gflib.h>


//==============================================================================
//  �萔��`
//==============================================================================
///�A�N�^�[�ő吔
#define PSL_ACTOR_MAX   (32)
///�Z���A�N�^�[���j�b�g�̃v���C�I���e�B
#define PSL_ACTOR_UNIT_PRI  (10)

enum{
  POKEICON_POS_START_X = 32,    ///<�|�P�����A�C�R���z�u�J�n�ʒuX
  POKEICON_POS_SPACE_X = 32,    ///<�|�P�����A�C�R�����̔z�u�ԊuX
  POKEICON_POS_Y = 64,          ///<�|�P�����A�C�R��Y���W
};

enum{
  ITEMICON_POS_START_X = 32,    ///<�A�C�e���A�C�R���z�u�J�n�ʒuX
  ITEMICON_POS_SPACE_X = 32,    ///<�A�C�e���A�C�R�����̔z�u�ԊuX
  ITEMICON_POS_Y = 64,          ///<�A�C�e���A�C�R��Y���W
};


//==============================================================================
//  �\���̒�`
//==============================================================================
typedef struct _PARTY_SELECT_LIST{
  FIELD_MAIN_WORK *fieldWork;
  const POKEPARTY *temoti_party;  ///<�莝���p�[�e�B
  const POKEPARTY *bbox_party;    ///<�o�g���{�b�N�X�p�[�e�B(�쐬���Ă��Ȃ��ꍇ��NULL)
  HEAPID heap_id;
  
  GFL_CLUNIT *clunit;
  GFL_CLWK *temoti_clwk[TEMOTI_POKEMAX];
  GFL_CLWK *bbox_clwk[TEMOTI_POKEMAX];
}PARTY_SELECT_LIST;


PARTY_SELECT_LIST_PTR PARTY_SELECT_LIST_Setup(FIELD_MAIN_WORK *fieldWork, const POKEPARTY *temoti, const POKEPARTY *bbox, BOOL temoti_reg_fail, BOOL bbox_reg_fail, HEAPID heap_id)
{
  PARTY_SELECT_LIST_PTR psl;
  
  psl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(PARTY_SELECT_LIST));
  psl->fieldWork = fieldWork;
  psl->temoti_party = temoti;
  psl->bbox_party = bbox;
  psl->heap_id = heap_id;

  psl->clunit = GFL_CLACT_UNIT_Create( PSL_ACTOR_MAX, PSL_ACTOR_UNIT_PRI, psl->heap_id );
}

void PARTY_SELECT_LIST_Exit(PARTY_SELECT_LIST_PTR psl)
{
  GFL_CLACT_UNIT_Delete(psl->clunit);
}

//--------------------------------------------------------------
/**
 * �|�P�����A�C�R�����\�[�X�S�o�^
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _PokeIcon_ResourceSet(PARTY_SELECT_LIST_PTR psl)
{
  ARCHANDLE *handle;
  int temoti_max, bbox_max;
  POKEMON_PARAM *pp;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_POKEICON, psl->heap_id);

  psl->pokeicon_pltt_index = GFL_CLGRP_PLTT_RegisterEx( handle, POKEICON_GetPalArcIndex(), 
    CLSYS_DRAW_MAIN, FLDOAM_PALNO_FREE_PSL_POKEICON * 0x20, 0, POKEICON_PAL_MAX, psl->heap_id );
  psl->pokeicon_cell_index = GFL_CLGRP_CELLANIM_Register( handle, POKEICON_GetCellArcIndex(), 
    POKEICON_GetAnmArcIndex(), psl->heap_id );
  
  //�莝��CGX
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    pp = PokeParty_GetMemberPointer(psl->temoti_party, i);
    psl->temoti_cgr_index[i] = GFL_CLGRP_CGR_Register( handle, 
      POKEICON_GetCgxArcIndex( PP_GetPPPPointer(pp) ), FALSE, CLSYS_DRAW_MAIN, psl->heap_id );
  }
  
  //�o�g���{�b�N�XCGX
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      pp = PokeParty_GetMemberPointer(psl->bbox_party, i);
      psl->bbox_cgr_index[i] = GFL_CLGRP_CGR_Register( handle, 
        POKEICON_GetCgxArcIndex( PP_GetPPPPointer(pp) ), FALSE, CLSYS_DRAW_MAIN, psl->heap_id );
    }
  }

  GFL_ARC_CloseDataHandle(handle);
}

//--------------------------------------------------------------
/**
 * �|�P�����A�C�R�����\�[�X�S���
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _PokeIcon_ResourceFree(PARTY_SELECT_LIST_PTR psl)
{
  int i;
  
  //�o�g���{�b�N�XCGX
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      GFL_CLGRP_CGR_Release(psl->bbox_cgr_index[i]);
    }
  }
  
  //�莝��CGX
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    GFL_CLGRP_CGR_Release(psl->temoti_cgr_index[i]);
  }
  
  GFL_CLGRP_CELLANIM_Release(psl->pokeicon_cell_index);
  GFL_CLGRP_PLTT_Release(psl->pokeicon_pltt_index);
}

//--------------------------------------------------------------
/**
 * �|�P�����A�C�R���A�N�^�[�S�o�^
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _PokeIcon_ActorAdd(PARTY_SELECT_LIST_PTR psl)
{
  int i, temoti_max, bbox_max;
  GFL_CLWK_DATA head = {
    0, POKEICON_POS_Y, 
    0,
    SOFTPRI_POKEICON,
    BGPRI_POKEICON,
  };
  
  //�莝��
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    head.pos_x = POKEICON_POS_START_X + POKEICON_POS_SPACE_X * i;
    psl->temoti_clwk[i] = GFL_CLACT_WK_Create( psl->clunit, psl->temoti_cgr_index[i], 
      psl->pokeicon_pltt_index, psl->pokeicon_cell_index, 
      &head, CLSYS_DRAW_MAIN, psl->heap_id);
  }
  
  //�o�g���{�b�N�X
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      head.pos_x = POKEICON_POS_START_X + POKEICON_POS_SPACE_X * i;
      psl->bbox_clwk[i] = GFL_CLACT_WK_Create( psl->clunit, psl->bbox_cgr_index[i], 
        psl->pokeicon_pltt_index, psl->pokeicon_cell_index, 
        &head, CLSYS_DRAW_MAIN, psl->heap_id);
    }
  }
}

//--------------------------------------------------------------
/**
 * �|�P�����A�C�R���A�N�^�[�S�폜
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _PokeIcon_ActorDel(PARTY_SELECT_LIST_PTR psl)
{
  int i, temoti_max, bbox_max;

  //�莝��
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    GFL_CLACT_WK_Remove(psl->temoti_clwk[i]);
  }
  
  //�o�g���{�b�N�X
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      GFL_CLACT_WK_Remove(psl->bbox_clwk[i]);
    }
  }
}

//--------------------------------------------------------------
/**
 * ����A�C�R�����\�[�X�S�o�^
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _Item_ResourceSet(PARTY_SELECT_LIST_PTR psl)
{
  ARCHANDLE *handle;
  
  handle = GFL_ARC_OpenDataHandle(ARCID_APP_MENU_COMMON, psl->heap_id);

  psl->item_pltt_index = GFL_CLGRP_PLTT_RegisterEx( handle, 
    APP_COMMON_GetPokeItemIconPltArcIdx(), 
    CLSYS_DRAW_MAIN, FLDOAM_PALNO_FREE_PSL_ITEM * 0x20, 0, APP_COMMON_ITEMICON_PLT_NUM, 
    psl->heap_id );
  
  psl->item_cell_index = GFL_CLGRP_CELLANIM_Register( handle, 
    APP_COMMON_GetPokeItemIconCellArcIdx(APP_COMMON_MAPPING_64K), 
    APP_COMMON_GetPokeItemIconAnimeArcIdx(APP_COMMON_MAPPING_64K), psl->heap_id );
  
  psl->item_cgr_index = GFL_CLGRP_CGR_Register( handle, 
    APP_COMMON_GetPokeItemIconCharArcIdx(), FALSE, CLSYS_DRAW_MAIN, psl->heap_id );

  GFL_ARC_CloseDataHandle(handle);
}

//--------------------------------------------------------------
/**
 * ����A�C�R�����\�[�X�S���
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _Item_ResourceFree(PARTY_SELECT_LIST_PTR psl)
{
  GFL_CLGRP_CGR_Release(psl->item_cgr_index);
  GFL_CLGRP_CELLANIM_Release(psl->item_cell_index);
  GFL_CLGRP_PLTT_Release(psl->item_pltt_index);
}

//--------------------------------------------------------------
/**
 * ����A�C�R���A�N�^�[�S�o�^
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _Item_ActorAdd(PARTY_SELECT_LIST_PTR psl)
{
  int i, temoti_max, bbox_max, item;
  BOOL enbale;
  GFL_CLWK_DATA head = {
    0, ITEMICON_POS_Y, 
    APP_COMMON_ITEMICON_ITEM,   //anm_seq
    SOFTPRI_ITEMICON,
    BGPRI_ITEMICON,
  };
  
  //�莝��
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    enable = TRUE;
    if(item == 0){
      enable = FALSE;
    }
    else if(ITEM_CheckMail( item ) == TRUE){
      head.anm_seq = APP_COMMON_ITEMICON_MAIL;
    }
    else{
      head.anm_seq = APP_COMMON_ITEMICON_ITEM;
    }
    head.pos_x = ITEMICON_POS_START_X + ITEMICON_POS_SPACE_X * i;
    psl->item_temoti_clwk[i] = GFL_CLACT_WK_Create( psl->clunit, psl->item_cgr_index, 
      psl->item_pltt_index, psl->item_cell_index, 
      &head, CLSYS_DRAW_MAIN, psl->heap_id);
    GFL_CLACT_WK_SetDrawEnable( psl->item_temoti_clwk[i], enable );
  }
  
  //�o�g���{�b�N�X
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      enable = TRUE;
      if(item == 0){
        enable = FALSE;
      }
      else if(ITEM_CheckMail( item ) == TRUE){
        head.anm_seq = APP_COMMON_ITEMICON_MAIL;
      }
      else{
        head.anm_seq = APP_COMMON_ITEMICON_ITEM;
      }
      head.pos_x = ITEMICON_POS_START_X + ITEMICON_POS_SPACE_X * i;
      psl->item_bbox_clwk[i] = GFL_CLACT_WK_Create( psl->clunit, psl->item_cgr_index, 
        psl->item_pltt_index, psl->item_cell_index, 
        &head, CLSYS_DRAW_MAIN, psl->heap_id);
      GFL_CLACT_WK_SetDrawEnable( psl->item_bbox_clwk[i], enable );
    }
  }
}

//--------------------------------------------------------------
/**
 * ����A�C�R���A�N�^�[�S�폜
 *
 * @param   psl		
 */
//--------------------------------------------------------------
static void _Item_ActorDel(PARTY_SELECT_LIST_PTR psl)
{
  int i, temoti_max, bbox_max;

  //�莝��
  temoti_max = PokeParty_GetPokeCount(psl->temoti_party);
  for(i = 0; i < temoti_max; i++){
    GFL_CLACT_WK_Remove(psl->item_temoti_clwk[i]);
  }
  
  //�o�g���{�b�N�X
  if(psl->bbox_party != NULL){
    bbox_max = PokeParty_GetPokeCount(psl->bbox_party);
    for(i = 0; i < bbox_max; i++){
      GFL_CLACT_WK_Remove(psl->item_bbox_clwk[i]);
    }
  }
}

