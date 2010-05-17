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
//FS_EXTERN_OVERLAY(zukan_search);


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

// ���������G���W���̏��
typedef enum
{
  ZKN_SCH_EGN_DIV_STATE_CONTINUE,  // �����p����
  ZKN_SCH_EGN_DIV_STATE_FINISH,    // ��������
}
ZKN_SCH_EGN_DIV_STATE;


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

// ���������G���W�����[�N
typedef struct  _ZKN_SCH_EGN_DIV_WORK  ZKN_SCH_EGN_DIV_WORK;


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

//-------------------------------------
/// ������������(1�t���[���ɏ�������������)
//=====================================
extern  ZKN_SCH_EGN_DIV_WORK*  ZUKAN_SEARCH_ENGINE_DivInit(
               const ZUKAN_SAVEDATA*      zkn_sv,
               const ZKNCOMM_LIST_SORT*   term,      // �Ăяo�����͂�����폜���Ȃ�����
               HEAPID                     heap_id
);
extern  void                   ZUKAN_SEARCH_ENGINE_DivExit(
               ZKN_SCH_EGN_DIV_WORK*      work
);
extern  ZKN_SCH_EGN_DIV_STATE  ZUKAN_SEARCH_ENGINE_DivSearch(  // 1�t���[����1��Ăяo��
               ZKN_SCH_EGN_DIV_WORK*      work,
               HEAPID                     heap_id,
               u16*                       num,      // �߂�l��ZKN_SCH_EGN_DIV_STATE_FINISH�ɂȂ�����L��
               u16**                      list      // �߂�l��ZKN_SCH_EGN_DIV_STATE_FINISH�ɂȂ�����L��
);

//-------------------------------------
/// �S��/�n���}�ӂ̔ԍ������X�g�𓾂�
//=====================================
extern u16 ZUKAN_GetNumberRow(
               u8       mode,  // ZKNCOMM_LIST_SORT_MODE_ZENKOKU / ZKNCOMM_LIST_SORT_MODE_LOCAL
               HEAPID   heap_id,
               u16**    list );

