
#ifdef PM_DEBUG

#include "common.h"
#include "system/wipe.h"

#include "net_app/balance_ball.h"

#include "bb_common.h"
#include "bb_comm_cmd.h"

#include "bb_server.h"
#include "bb_client.h"

#include "bb_game.h"

typedef struct {
	
	GFL_BMPWIN*	win;
	BB_SYS*			sys;
	int				seq;
	
} DEBUG_WORK;

static DEBUG_WORK	dwk = { 0 };
static DEBUG_WORK*  wk = &dwk;

//--------------------------------------------------------------
/**
 * @brief	Game Setup
 *
 * @param	work	
 *
 * @retval	none	
 *
 */
//--------------------------------------------------------------
BOOL Debug_GameSetup( BB_WORK* work )
{
	wk->sys = &work->sys;
	
	if ( wk->seq == 0 ) {
		
		GF_BGL_BmpWinInit( wk->win );
		wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME3_M, 1, 1, 30, 22, 14, GFL_BMP_CHRAREA_GET_B );
		GFL_BMPWIN_MakeScreen(wk->win);
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), 0xFF );
		GF_BGL_BmpWinOn( wk->win );
		
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG3, VISIBLE_ON );
		
		wk->seq++;
	}
	
	if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		ArcUtil_HDL_BgCharSet( wk->sys->p_handle_bb, NARC_balance_ball_gra_manene_bottom_NCGR, GFL_BG_FRAME1_M, 0, 0, 0, HEAPID_BB );	
		ArcUtil_HDL_ScrnSet(   wk->sys->p_handle_bb, NARC_balance_ball_gra_manene_bottom_NSCR, GFL_BG_FRAME1_M, 0, 0, 0, HEAPID_BB );
		
	    GF_BGL_BmpWinOff( wk->win );
		GFL_BMPWIN_Delete( wk->win );
		return TRUE;
	}
	
	return FALSE;
}


#endif
