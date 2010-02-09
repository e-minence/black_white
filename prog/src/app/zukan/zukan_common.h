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


//============================================================================================
//	�萔��`
//============================================================================================

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


extern void ZKNCOMM_MakeDefaultList( ZUKAN_SAVEDATA * sv, u16 * list );

extern void ZKNCOMM_SetFadeIn( HEAPID heapID );

extern void ZKNCOMM_SetFadeOut( HEAPID heapID );

extern void ZKNCOMM_ScrollBaseBG( u8 mainBG, u8 subBG, u32 * cnt );


extern GFL_CLWK * ZKNCOMM_CreateClact( GFL_CLUNIT * unit, const ZKNCOMM_CLWK_DATA * prm, HEAPID heapID );
