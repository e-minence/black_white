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


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static int HitCheckPokeObj( DPCMAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

static const GFL_UI_TP_HITTBL TpHitTbl[] =
{
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1,  8,  31 },		// 06: �y�[�W��
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, 88, 111 },		// 07: �y�[�W�E
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_X_06+TOUCHBAR_ICON_WIDTH-1 },			// 08: �߂�P
	{ TOUCHBAR_ICON_Y, TOUCHBAR_ICON_Y+TOUCHBAR_ICON_HEIGHT-1, TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_X_07+TOUCHBAR_ICON_WIDTH-1 },			// 09: �߂�Q
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};




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

	if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT|PAD_BUTTON_L) ){
		return DPCUI_ID_LEFT;
	}

	if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT|PAD_BUTTON_R) ){
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

	if( GFL_UI_KEY_GetTrg() & (PAD_KEY_LEFT|PAD_BUTTON_L) ){
		return DPCUI_ID_LEFT;
	}

	if( GFL_UI_KEY_GetTrg() & (PAD_KEY_RIGHT|PAD_BUTTON_R) ){
		return DPCUI_ID_RIGHT;
	}

/*
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		return DPCUI_ID_MODE_CHANGE;
	}
*/

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return DPCUI_ID_RETURN;
	}

	return DPCUI_ID_NONE;
}

#define	POKE_LX(a)	( a - POKE2DGRA_POKEMON_CHARA_WIDTH*8/2 )
#define	POKE_RX(a)	( a + POKE2DGRA_POKEMON_CHARA_WIDTH*8/2 )
#define	POKE_UX(a)	( a - POKE2DGRA_POKEMON_CHARA_HEIGHT*8/2 )
#define	POKE_DX(a)	( a + POKE2DGRA_POKEMON_CHARA_HEIGHT*8/2 )

static int HitCheckPokeObj( DPCMAIN_WORK * wk )
{
	u32	i;
	u32	tpx, tpy;
	s16	objx, objy;
	s16	id, tmpy;

	if( GFL_UI_TP_GetPointTrg( &tpx, &tpy ) == FALSE ){
		return DPCUI_ID_NONE;
	}

	id = -1;
	for( i=DPCOBJ_ID_POKE01; i<=DPCOBJ_ID_POKE16; i++ ){
		if( wk->clwk[i] == NULL ){ continue; }
		DPCOBJ_GetPos( wk, i, &objx, &objy );
		if( tpx >= POKE_LX(objx) && tpx < POKE_RX(objx) && tpy >= POKE_UX(objy) && tpy < POKE_DX(objy) ){
			if( id == -1 ){
				id = i - DPCOBJ_ID_POKE01;
				tmpy = objy;
			}else{
				if( objy > tmpy ){
					id = i - DPCOBJ_ID_POKE01;
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
