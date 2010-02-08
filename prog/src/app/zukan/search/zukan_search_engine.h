//============================================================================
/**
 *  @file   zukan_search_engine.h
 *  @brief  �}�ӌ����G���W��
 *  @author Koji Kawada
 *  @data   2010.02.05
 *  @note   
 *
 *  ���W���[�����FZUKAN_SEARCH_ENGINE
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>


// �I�[�o�[���C
FS_EXTERN_OVERLAY(zukan_search);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define	ZKNCOMM_LIST_SORT_NONE	( 0xff )

enum {
	// �}�Ӄ��[�h
	ZKNCOMM_LIST_SORT_MODE_ZENKOKU = 0,
	ZKNCOMM_LIST_SORT_MODE_LOCAL,
	ZKNCOMM_LIST_SORT_MODE_MAX,

	// ����
	ZKNCOMM_LIST_SORT_ROW_NUMBER = 0,
	ZKNCOMM_LIST_SORT_ROW_NAME,
	ZKNCOMM_LIST_SORT_ROW_WEIGHT_HI,
	ZKNCOMM_LIST_SORT_ROW_WEIGHT_LOW,
	ZKNCOMM_LIST_SORT_ROW_HEIGHT_HI,
	ZKNCOMM_LIST_SORT_ROW_HEIGHT_LOW,
	ZKNCOMM_LIST_SORT_ROW_MAX,

	// ���O
	// �^�C�v
	// �`
	/* ���l�ŁB */
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
typedef struct {
	u8	mode;				// �}�Ӄ��[�h
	u8	row;				// ���сi�U��j
	u8	name;				// ���O
	u8	type1;			// �^�C�v�P�i�P�V��j  // prog/include/poke_tool/poketype_def.h
	u8	type2;			// �^�C�v�Q�i�P�V��j
  u8  color;      // �F�i�P�O��j  // prog/include/poke_tool/poke_personal.h
	u8	form;				// �`�i�P�S��j
}ZKNCOMM_LIST_SORT;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
/// ��������
//=====================================
extern u16 ZUKAN_SEARCH_ENGINE_Search(
               const ZUKAN_SAVEDATA*      zkn_sv,
               const ZKNCOMM_LIST_SORT*   term,
               HEAPID                     heap_id,
               u16**                      list );

