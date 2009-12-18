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


//============================================================================================
//	�萔��`
//============================================================================================

#define	ZKNCOMM_BG_SCROLL_WAIT		( 4 )			// �a�f�X�N���[���E�F�C�g


// �Z���A�N�^�[�f�[�^
typedef struct {
	GFL_CLWK_DATA	dat;

	u32	chrRes;
	u32	palRes;
	u32	celRes;

	u16	pal;
	u16	disp;

}ZKNCOMM_CLWK_DATA;


extern void ZKNCOMM_SetFadeIn( HEAPID heapID );

extern void ZKNCOMM_SetFadeOut( HEAPID heapID );

extern GFL_CLWK * ZKNCOMM_CreateClact( GFL_CLUNIT * unit, const ZKNCOMM_CLWK_DATA * prm, HEAPID heapID );
