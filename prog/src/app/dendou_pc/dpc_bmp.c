//============================================================================================
/**
 * @file		dpc_bmp.c
 * @brief		�a������m�F��� �a�l�o�֘A
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	���W���[�����FDPCBMP
 */
//============================================================================================
#include <gflib.h>

#include "dpc_main.h"
#include "dpc_bmp.h"


//============================================================================================
//	�萔��`
//============================================================================================

// �^�C�g��
#define	BMPWIN_TITLE_FRM	( GFL_BG_FRAME3_M )
#define	BMPWIN_TITLE_PX		( 1 )
#define	BMPWIN_TITLE_PY		( 0 )
#define	BMPWIN_TITLE_SX		( 30 )
#define	BMPWIN_TITLE_SY		( 2 )
#define	BMPWIN_TITLE_PAL	( DPCMAIN_MBG_PAL_FONT )

// �y�[�W
#define	BMPWIN_PAGE_FRM		( GFL_BG_FRAME3_M )
#define	BMPWIN_PAGE_PX		( 5 )
#define	BMPWIN_PAGE_PY		( 21 )
#define	BMPWIN_PAGE_SX		( 5 )
#define	BMPWIN_PAGE_SY		( 3 )
#define	BMPWIN_PAGE_PAL		( DPCMAIN_MBG_PAL_FONT )

// ���
#define	BMPWIN_INFO_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_INFO_PX		( 4 )
#define	BMPWIN_INFO_PY		( 8 )
#define	BMPWIN_INFO_SX		( 24 )
#define	BMPWIN_INFO_SY		( 14 )
#define	BMPWIN_INFO_PAL		( DPCMAIN_SBG_PAL_FONT )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================


//============================================================================================
//	�O���[�o��
//============================================================================================




void DPCBMP_Init( DPCMAIN_WORK * wk )
{
	GFL_BMPWIN_Init( HEAPID_DENDOU_PC );

	wk->win[DPCBMP_WINID_TITLE].win = GFL_BMPWIN_Create(
																			BMPWIN_TITLE_FRM,
																			BMPWIN_TITLE_PX, BMPWIN_TITLE_PY,
																			BMPWIN_TITLE_SX, BMPWIN_TITLE_SY,
																			BMPWIN_TITLE_PAL, GFL_BMP_CHRAREA_GET_B );
	wk->win[DPCBMP_WINID_PAGE].win = GFL_BMPWIN_Create(
																			BMPWIN_PAGE_FRM,
																			BMPWIN_PAGE_PX, BMPWIN_PAGE_PY,
																			BMPWIN_PAGE_SX, BMPWIN_PAGE_SY,
																			BMPWIN_PAGE_PAL, GFL_BMP_CHRAREA_GET_B );
	wk->win[DPCBMP_WINID_INFO].win = GFL_BMPWIN_Create(
																			BMPWIN_INFO_FRM,
																			BMPWIN_INFO_PX, BMPWIN_INFO_PY,
																			BMPWIN_INFO_SX, BMPWIN_INFO_SY,
																			BMPWIN_INFO_PAL, GFL_BMP_CHRAREA_GET_B );
}

void DPCBMP_Exit( DPCMAIN_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_TITLE].win );
	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_PAGE].win );
	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_INFO].win );
	GFL_BMPWIN_Exit();
}

void DPCBMP_PutTitle( DPCMAIN_WORK * wk )
{
}

void DPCBMP_PutPage( DPCMAIN_WORK * wk )
{
}

void DPCBMP_PutInfo( DPCMAIN_WORK * wk )
{
}
