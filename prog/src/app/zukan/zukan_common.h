//============================================================================================
/**
 * @file		zukan_common.h
 * @brief		�}�Ӊ�� ���ʏ���
 * @author	Hiroyuki Nakamura
 * @date		09.12.15
 *
 *	���W���[�����FZKNCOMM
 */
//============================================================================================
#pragma	once

#include "savedata/zukan_savedata.h"
#include "search/zukan_search_engine.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �㉺�o�[�̃X�N���[���X�s�[�h
#define ZKNCOMM_BAR_SPEED   (8)     // ���t���[��ZKNCOMM_BAR_SPEED�����ړ�����
// �t�F�[�h�C��/�A�E�g�̃X�s�[�h
#define ZKNCOMM_FADE_WAIT   (0)     // PaletteFadeReqWrite��wait�ɓn����l


// �Z���A�N�^�[�f�[�^
typedef struct {
	GFL_CLWK_DATA	dat;

	u32	chrRes;
	u32	palRes;
	u32	celRes;

	u16	pal;
	u16	disp;

}ZKNCOMM_CLWK_DATA;

// ���X�g�f�[�^
enum {
	ZUKAN_LIST_MONS_NONE = 0,		// ���m�F
	ZUKAN_LIST_MONS_SEE,				// ����
	ZUKAN_LIST_MONS_GET,				// �ߊl����
};


extern u16 ZKNCOMM_MakeDefaultList( ZUKAN_SAVEDATA * sv, u16 ** list, HEAPID heapID );

extern BOOL ZKNCOMM_CheckLocalListNumberZero( ZUKAN_SAVEDATA * sv );

extern void ZKNCOMM_ResetSortData( const ZUKAN_SAVEDATA * sv, ZKNCOMM_LIST_SORT * sort );

extern void ZKNCOMM_SetFadeIn( HEAPID heapID );

extern void ZKNCOMM_SetFadeOut( HEAPID heapID );

extern void ZKNCOMM_ScrollBaseBG( u8 mainBG, u8 subBG, u32 * cnt );


extern GFL_CLWK * ZKNCOMM_CreateClact( GFL_CLUNIT * unit, const ZKNCOMM_CLWK_DATA * prm, HEAPID heapID );
