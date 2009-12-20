//==============================================================================
/**
 * @file    party_select_list.h
 * @brief   �莝�� or �o�g���{�b�N�X �I�����X�g
 * @author  matsuda
 * @date    2009.12.19(�y)
 */
//==============================================================================
#pragma once

#include "field/fieldmap_proc.h"


//==============================================================================
//  �萔��`
//==============================================================================
typedef enum{
  SELECT_PARTY_TEMOTI,    ///<�莝�����I�����ꂽ
  SELECT_PARTY_BBOX,      ///<�o�g���{�b�N�X���I�����ꂽ
  SELECT_PARTY_CANCEL,    ///<�L�����Z���ŏI��
}SELECT_PARTY;


//==============================================================================
//  �^��`
//==============================================================================
///�莝��or�o�g���{�b�N�X�I�����X�g����V�X�e���̕s��`�|�C���^
typedef struct _PARTY_SELECT_LIST * PARTY_SELECT_LIST_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern PARTY_SELECT_LIST_PTR PARTY_SELECT_LIST_Setup(FIELDMAP_WORK *fieldWork, 
  const POKEPARTY *temoti, const POKEPARTY *bbox, 
  BOOL temoti_reg_fail, BOOL bbox_reg_fail, HEAPID heap_id);
extern SELECT_PARTY PARTY_SELECT_LIST_Exit(PARTY_SELECT_LIST_PTR psl);
extern BOOL PARTY_SELECT_LIST_Main(PARTY_SELECT_LIST_PTR psl);
extern SELECT_PARTY PARTY_SELECT_LIST_GetSelect(PARTY_SELECT_LIST_PTR psl);
