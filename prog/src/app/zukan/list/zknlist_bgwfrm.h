//============================================================================================
/**
 * @file		zknlist_bgwfrm.h
 * @brief		�}�Ӄ��X�g��� �a�f�E�B���h�E�t���[���֘A
 * @author	Hiroyuki Nakamura
 * @date		09.12.16
 *
 *	���W���[�����FZKNLISTBGWFRM
 */
//============================================================================================
#pragma	once


//============================================================================================
//	�萔��`
//============================================================================================

// ���X�g�t���[���ԍ�
enum {
	ZKNLISTBGWFRM_POKE_GET = 0,		// �ߊl�ς�
	ZKNLISTBGWFRM_POKE_NONE,			// �ߊl���Ă��Ȃ�
};

enum {
	ZKNLISTBGWFRM_LISTPUT_UP = 0,
	ZKNLISTBGWFRM_LISTPUT_DOWN
};


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

extern void ZKNLISTBGWFRM_Init( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTBGWFRM_Exit( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTBGWFRM_SetNameFrame( ZKNLISTMAIN_WORK * wk, u32 idx, u32 num );

extern void ZKNLISTBGWFRM_PutPokeList( ZKNLISTMAIN_WORK * wk, u32 idx, s32 listPos );

extern void ZKNLISTBGWFRM_PutScrollList( ZKNLISTMAIN_WORK * wk, u32 idx, u32 mv );

extern void ZKNLISTBGWFRM_PutScrollListSub( ZKNLISTMAIN_WORK * wk, u32 idx, u32 mv );
