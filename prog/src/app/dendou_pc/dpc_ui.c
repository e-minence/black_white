//============================================================================================
/**
 * @file		dpc_ui.c
 * @brief		�a������m�F��� �t�h�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	���W���[�����FDPCUI
 */
//============================================================================================
#include <gflib.h>

#include "system/poke2dgra.h"
#include "ui/touchbar.h"

#include "dpc_main.h"
#include "dpc_obj.h"
#include "dpc_ui.h"


//============================================================================================
//	�萔��`
//============================================================================================
// �|�P�����n�a�i�\���͈͎擾�}�N��
#define	POKE_LX(a)	( a - POKE2DGRA_POKEMON_CHARA_WIDTH*8/2 )
#define	POKE_RX(a)	( a + POKE2DGRA_POKEMON_CHARA_WIDTH*8/2 )
#define	POKE_UY(a)	( a - POKE2DGRA_POKEMON_CHARA_HEIGHT*8/2 )
#define	POKE_DY(a)	( a + POKE2DGRA_POKEMON_CHARA_HEIGHT*8/2 )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int HitCheckPokeObj( DPCMAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================
// �^�b�`�e�[�u��
static const GFL_UI_TP_HITTBL TpHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,  8,  31 },		// 06: �y�[�W��
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, 88, 111 },		// 07: �y�[�W�E
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 },			// 08: �߂�P
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },			// 09: �߂�Q
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�y�[�W�I���C���^�[�t�F�[�X���C��
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
int DPCUI_PageMain( DPCMAIN_WORK * wk )
{
	int	ret;
		
	ret = GFL_UI_TP_HitTrg( TpHitTbl );
	if( ret != GFL_UI_TP_HIT_NONE ){
		return ret;
	}

	ret = HitCheckPokeObj( wk );
	if( ret != DPCUI_ID_NONE ){
		return ret;
	}

	if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_LEFT|PAD_BUTTON_L) ){
		return DPCUI_ID_LEFT;
	}

	if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_RIGHT|PAD_BUTTON_R) ){
		return DPCUI_ID_RIGHT;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		return DPCUI_ID_MODE_CHANGE;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return DPCUI_ID_RETURN;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		return DPCUI_ID_EXIT;
	}

	return DPCUI_ID_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����I���C���^�[�t�F�[�X���C��
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
int DPCUI_PokeMain( DPCMAIN_WORK * wk )
{
	int	ret;
		
	ret = GFL_UI_TP_HitTrg( TpHitTbl );
	if( ret == DPCUI_ID_RETURN ){
		return ret;
	}

	ret = HitCheckPokeObj( wk );
	if( ret != DPCUI_ID_NONE ){
		return ret;
	}

	if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_LEFT|PAD_BUTTON_L) ){
		return DPCUI_ID_LEFT;
	}

	if( GFL_UI_KEY_GetRepeat() & (PAD_KEY_RIGHT|PAD_BUTTON_R) ){
		return DPCUI_ID_RIGHT;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return DPCUI_ID_RETURN;
	}

	return DPCUI_ID_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����n�a�i�^�b�`�`�F�b�N
 *
 * @param		wk		�a������o�b��ʃ��[�N
 *
 * @return	���͌���
 */
//--------------------------------------------------------------------------------------------
static int HitCheckPokeObj( DPCMAIN_WORK * wk )
{
	u32	i;
	u32	tpx, tpy;
	s16	objx, objy;
	s16	id, def;
	s16	tmpy;
	s16	lx, rx, uy, dy;

	if( GFL_UI_TP_GetPointTrg( &tpx, &tpy ) == FALSE ){
		return DPCUI_ID_NONE;
	}

	def = DPCOBJ_GetDefaultPoke( wk );
	id = -1;
	for( i=def; i<def+6; i++ ){
		if( wk->clwk[i] == NULL ){ continue; }
		DPCOBJ_GetPos( wk, i, &objx, &objy );
		lx = POKE_LX(objx);
		rx = POKE_RX(objx);
		uy = POKE_UY(objy);
		dy = POKE_DY(objy);
		if( lx < 0 ){ lx = 0; }
		if( rx > 255 ){ rx = 255; }
		if( uy < 0 ){ uy = 0; }
		if( dy > 255 ){ dy = 255; }
		if( tpx >= lx && tpx < rx && tpy >= uy && tpy < dy ){
			if( id == -1 ){
				id = i - def;
				tmpy = objy;
			}else{
				if( objy > tmpy ){
					id = i - def;
					tmpy = objy;
				}
			}
		}
	}

	if( id != -1 ){
		return ( ( id % 6 ) + DPCUI_ID_POKE1 );
	}

	return DPCUI_ID_NONE;
}
