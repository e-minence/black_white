//============================================================================================
/**
 * @file		zukan_common.c
 * @brief		�}�Ӊ�� ���ʏ���
 * @author	Hiroyuki Nakamura
 * @date		09.12.15
 *
 *	���W���[�����FZKNCOMM
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "poke_tool/monsnum_def.h"

#include "zukan_common.h"


//============================================================================================
//	�萔��`
//============================================================================================
#define	BASEBG_SCROLL_WAIT	( 4 )			// �a�f�X�N���[���E�F�C�g
#define	BASEBG_SCROLL_VAL		( 1 )			// �a�f�X�N���[���l


//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�t�H���g���X�g�쐬
 *
 * @param		sv			�}�ӃZ�[�u�f�[�^
 * @param		list		���X�g�쐬�ꏊ
 * @param		heapID	�q�[�v�h�c
 *
 * @return	���X�g�ɓo�^������
 */
//--------------------------------------------------------------------------------------------
u16 ZKNCOMM_MakeDefaultList( ZUKAN_SAVEDATA * sv, u16 ** list, HEAPID heapID )
{
	u16 * buf;
	u16	siz, max;
	u32	i;

	if( ZUKANSAVE_GetZukanMode( sv ) == TRUE ){
		siz = ZUKAN_GetNumberRow( ZKNCOMM_LIST_SORT_MODE_ZENKOKU, heapID, &buf );
	}else{
		siz = ZUKAN_GetNumberRow( ZKNCOMM_LIST_SORT_MODE_LOCAL, heapID, &buf );
	}
	max = 0;

	for( i=0; i<siz; i++ ){
		if( ZUKANSAVE_GetPokeGetFlag( sv, buf[i] ) == TRUE ||
				ZUKANSAVE_GetPokeSeeFlag( sv, buf[i] ) == TRUE ){
			max = i+1;
		}
	}

	*list = buf;

	return max;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�r�N�e�B�`�F�b�N
 *
 * @param		sv		�}�ӃZ�[�u�f�[�^
 *
 * @retval	"TRUE = �\������"
 * @retval	"FALSE = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNCOMM_CheckLocalListNumberZero( ZUKAN_SAVEDATA * sv )
{
	// �\�����[�h���S��
	if( ZUKANSAVE_GetZukanMode( sv ) == TRUE ){
		return TRUE;
	}

	// �����E�ߊl�����ꍇ
	if( ZUKANSAVE_GetPokeGetFlag( sv, MONSNO_657 ) == TRUE ||
			ZUKANSAVE_GetPokeSeeFlag( sv, MONSNO_657 ) == TRUE ){
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�\�[�g�f�[�^���Z�b�g
 *
 * @param		sv		�}�ӃZ�[�u�f�[�^
 * @param		sort	�\�[�g�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNCOMM_ResetSortData( const ZUKAN_SAVEDATA * sv, ZKNCOMM_LIST_SORT * sort )
{
	if( ZUKANSAVE_GetZukanMode( sv ) == TRUE ){
		sort->mode = ZKNCOMM_LIST_SORT_MODE_ZENKOKU;
	}else{
		sort->mode = ZKNCOMM_LIST_SORT_MODE_LOCAL;
	}
	sort->row   = ZKNCOMM_LIST_SORT_ROW_NUMBER;
	sort->name  = ZKNCOMM_LIST_SORT_NONE;
	sort->type1 = ZKNCOMM_LIST_SORT_NONE;
	sort->type2 = ZKNCOMM_LIST_SORT_NONE;
	sort->color = ZKNCOMM_LIST_SORT_NONE;
	sort->form  = ZKNCOMM_LIST_SORT_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�C���Z�b�g
 *
 * @param		heapID		�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNCOMM_SetFadeIn( HEAPID heapID )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�t�F�[�h�A�E�g�Z�b�g
 *
 * @param		heapID		�q�[�v�h�c
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNCOMM_SetFadeOut( HEAPID heapID )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, heapID );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�w�i�X�N���[��
 *
 * @param		mainBG		���C����ʂ̂a�f�t���[��
 * @param		subBG			�T�u��ʂ̂a�f�t���[��
 * @param		cnt				�J�E���^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNCOMM_ScrollBaseBG( u8 mainBG, u8 subBG, u32 * cnt )
{
	*cnt += 1;
	if( *cnt == BASEBG_SCROLL_WAIT ){
		GFL_BG_SetScrollReq( mainBG, GFL_BG_SCROLL_X_INC, BASEBG_SCROLL_VAL );
		GFL_BG_SetScrollReq( subBG, GFL_BG_SCROLL_X_INC, BASEBG_SCROLL_VAL );
		*cnt = 0;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�Z���A�N�^�[�ǉ�
 *
 * @param		unit		GFL_CLUNIT
 * @param		prm			�Z���A�N�^�[�f�[�^
 * @param		heapID	�q�[�v�h�c
 *
 * @return	GFL_CLWK
 */
//--------------------------------------------------------------------------------------------
GFL_CLWK * ZKNCOMM_CreateClact( GFL_CLUNIT * unit, const ZKNCOMM_CLWK_DATA * prm, HEAPID heapID )
{
	return GFL_CLACT_WK_Create( unit, prm->chrRes, prm->palRes, prm->celRes, &prm->dat, prm->disp, heapID );
}
