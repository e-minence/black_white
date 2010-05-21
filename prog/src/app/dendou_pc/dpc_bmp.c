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

#include "message.naix"
#include "ui/print_tool.h"
#include "msg/msg_pc_dendou.h"

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
#define	BMPWIN_TITLE_PAL	( 3 )

/*
// �y�[�W
#define	BMPWIN_PAGE_FRM		( GFL_BG_FRAME3_M )
#define	BMPWIN_PAGE_PX		( 5 )
#define	BMPWIN_PAGE_PY		( 21 )
#define	BMPWIN_PAGE_SX		( 5 )
#define	BMPWIN_PAGE_SY		( 3 )
#define	BMPWIN_PAGE_PAL		( 3 )
*/

// ���
#define	BMPWIN_INFO_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_INFO_PX		( 4 )
#define	BMPWIN_INFO_PY		( 7 )
#define	BMPWIN_INFO_SX		( 24 )
#define	BMPWIN_INFO_SY		( 15 )
#define	BMPWIN_INFO_PAL		( 1 )

// ���ʕ\�����W
#define	NAME_PX				( 0 )
#define	NAME_PY				( 0 )
#define	SEX_PX				( 8*8 )
#define	SEX_PY				( 0 )
#define	LEVEL_PX			( 10*8 )
#define	LEVEL_PY			( 0 )
#define	NICKNAME_PX		( 0 )
#define	NICKNAME_PY		( 3*8 )
#define	OYA_PX				( 0 )
#define	OYA_PY				( 6*8 )
#define	WAZA_PX				( 0 )
#define	WAZA_PY				( 11*8 )
#define	WAZA_SX				( 12*8 )
#define	WAZA_SY				( 2*8 )

// �t�H���g�J���[
#define FCOL_MP03WN		( PRINTSYS_LSB_Make(1,2,0) )		// �t�H���g�J���[�F���p�O�R����
#define FCOL_MP03RN		( PRINTSYS_LSB_Make(5,6,0) )		// �t�H���g�J���[�F���p�O�R�Ԕ�
#define FCOL_SP01WN		( PRINTSYS_LSB_Make(1,2,0) )		// �t�H���g�J���[�F��p�O�P����
#define FCOL_SP01RN		( PRINTSYS_LSB_Make(5,6,0) )		// �t�H���g�J���[�F��p�O�P�Ԕ�
#define FCOL_SP01BLN	( PRINTSYS_LSB_Make(3,4,0) )		// �t�H���g�J���[�F��p�O�P��

#define FCOL_FNTOAM		( PRINTSYS_LSB_Make(6,8,0) )		// �t�H���g�J���[�F�t�H���gOAM


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
/*
	wk->win[DPCBMP_WINID_PAGE].win = GFL_BMPWIN_Create(
																			BMPWIN_PAGE_FRM,
																			BMPWIN_PAGE_PX, BMPWIN_PAGE_PY,
																			BMPWIN_PAGE_SX, BMPWIN_PAGE_SY,
																			BMPWIN_PAGE_PAL, GFL_BMP_CHRAREA_GET_B );
*/
	wk->win[DPCBMP_WINID_INFO].win = GFL_BMPWIN_Create(
																			BMPWIN_INFO_FRM,
																			BMPWIN_INFO_PX, BMPWIN_INFO_PY,
																			BMPWIN_INFO_SX, BMPWIN_INFO_SY,
																			BMPWIN_INFO_PAL, GFL_BMP_CHRAREA_GET_B );
}

void DPCBMP_Exit( DPCMAIN_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_TITLE].win );
//	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_PAGE].win );
	GFL_BMPWIN_Delete( wk->win[DPCBMP_WINID_INFO].win );
	GFL_BMPWIN_Exit();
}

void DPCBMP_PrintUtilTrans( DPCMAIN_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<DPCBMP_WINID_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->win[i], wk->que );
	}
}


static void PrintScreenTrans( PRINT_UTIL * util )
{
	GFL_BMPWIN_MakeScreen( util->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(util->win) );
}

void DPCBMP_PutTitle( DPCMAIN_WORK * wk )
{
	DPC_PARTY_DATA * pt;
	STRBUF * str;

	pt = &wk->party[wk->page];

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[DPCBMP_WINID_TITLE].win), 0 );

	// ����N���A
	if( wk->dat->callMode == DENDOUPC_CALL_CLEAR ){
		str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_01_01 );
		PRINTTOOL_PrintColor(
			&wk->win[DPCBMP_WINID_TITLE], wk->que, 0, 0, str, wk->font, FCOL_MP03WN, PRINTTOOL_MODE_LEFT );
		GFL_STR_DeleteBuffer( str );
	// �a������
	}else{
		str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_01_02 );
		WORDSET_RegisterNumber( wk->wset, 0, pt->recNo, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wk->wset, wk->exp, str );
		PRINTTOOL_PrintColor(
			&wk->win[DPCBMP_WINID_TITLE], wk->que, 0, 0, wk->exp, wk->font, FCOL_MP03WN, PRINTTOOL_MODE_LEFT );
		GFL_STR_DeleteBuffer( str );
	}

	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_01_03 );
	WORDSET_RegisterNumber( wk->wset, 0, pt->date.year, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	WORDSET_RegisterNumber( wk->wset, 1, pt->date.month, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	WORDSET_RegisterNumber( wk->wset, 2, pt->date.day, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTTOOL_PrintColor(
		&wk->win[DPCBMP_WINID_TITLE], wk->que,
		GFL_BMPWIN_GetScreenSizeX(wk->win[DPCBMP_WINID_TITLE].win)*8,
		0,
		wk->exp, wk->font, FCOL_MP03RN, PRINTTOOL_MODE_RIGHT );
	GFL_STR_DeleteBuffer( str );

	PrintScreenTrans( &wk->win[DPCBMP_WINID_TITLE] );
}

void DPCBMP_PutPage( DPCMAIN_WORK * wk )
{
	STRBUF * str;
	u8	px, sx;

	if( wk->pageMax == 1 ){ return; }
/*
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[DPCBMP_WINID_PAGE].win), 0 );

	PRINTTOOL_PrintFractionColor(
		&wk->win[DPCBMP_WINID_PAGE], wk->que, wk->font,
		GFL_BMPWIN_GetScreenSizeX(wk->win[DPCBMP_WINID_PAGE].win)*8/2,
		4,
		FCOL_MP03WN, wk->page+1, wk->pageMax, HEAPID_DENDOU_PC );

	PrintScreenTrans( &wk->win[DPCBMP_WINID_PAGE] );
*/

	GFL_BMP_Clear( wk->fobmp, 0 );

	//�u/�v
	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_01_04 );
	sx  = PRINTSYS_GetStrWidth( str, wk->font, 0 );
	px  = ( DPCBMP_PAGE_SX * 8 - sx ) / 2;
	PRINTSYS_PrintColor(
		wk->fobmp,
		px, 4,
		str, wk->font, FCOL_FNTOAM );
	GFL_STR_DeleteBuffer( str );

	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_01_05 );

	// ���݂̃y�[�W
	WORDSET_RegisterNumber( wk->wset, 0, wk->page+1, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTSYS_PrintColor(
		wk->fobmp,
		px - PRINTSYS_GetStrWidth( wk->exp, wk->font, 0 ), 4,
		wk->exp, wk->font, FCOL_FNTOAM );

	// �y�[�W�ő�
	WORDSET_RegisterNumber( wk->wset, 0, wk->pageMax, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTSYS_PrintColor(
		wk->fobmp,
		px+sx , 4,
		wk->exp, wk->font, FCOL_FNTOAM );

	GFL_STR_DeleteBuffer( str );

	BmpOam_ActorBmpTrans( wk->foact );

/*
	STRBUF * str;
	u32	x;

	str = GFL_STR_CreateBuffer( BOX_TRAYNAME_BUFSIZE, HEAPID_BOX_APP_L );

	BOXDAT_GetBoxName( syswk->dat->sv_box, tray, str );
	GFL_BMP_Clear( syswk->app->fobj[idx].bmp, 0 );
	x = ( BOX2OBJ_FNTOAM_BOXNAME_SX * 8 - PRINTSYS_GetStrWidth( str, syswk->app->font, 0 ) ) / 2;
	PRINTSYS_PrintColor( syswk->app->fobj[idx].bmp, x, 0, str, syswk->app->font, FCOL_FNTOAM );

	GFL_STR_DeleteBuffer( str );

	BmpOam_ActorBmpTrans( syswk->app->fobj[idx].oam );
*/
}

void DPCBMP_PutInfo( DPCMAIN_WORK * wk )
{
	DENDOU_POKEMON_DATA * poke;
	STRBUF * str;
	u32	i;

	poke = &wk->party[wk->page].dat[wk->pokePos];

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[DPCBMP_WINID_INFO].win), 0 );

	// �|�P������
	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_01 );
	WORDSET_RegisterPokeMonsNameNo( wk->wset, 0, poke->monsno );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTTOOL_PrintColor(
		&wk->win[DPCBMP_WINID_INFO], wk->que,
		NAME_PX, NAME_PY, wk->exp, wk->font, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
	GFL_STR_DeleteBuffer( str );

	// ����
	if( poke->sex == PTL_SEX_MALE ){
		if( poke->monsno != MONSNO_NIDORAN_M ){
			str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_02 );
			PRINTTOOL_PrintColor(
				&wk->win[DPCBMP_WINID_INFO], wk->que,
				SEX_PX, SEX_PY, str, wk->font, FCOL_SP01BLN, PRINTTOOL_MODE_LEFT );
			GFL_STR_DeleteBuffer( str );
		}
	}else if( poke->sex == PTL_SEX_FEMALE ){
		if( poke->monsno != MONSNO_NIDORAN_F ){
			str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_03 );
			PRINTTOOL_PrintColor(
				&wk->win[DPCBMP_WINID_INFO], wk->que,
				SEX_PX, SEX_PY, str, wk->font, FCOL_SP01RN, PRINTTOOL_MODE_LEFT );
			GFL_STR_DeleteBuffer( str );
		}
	}

	// ���x��
	{
		u32	npx;

		str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_08 );
		npx = PRINTSYS_GetStrWidth( str, wk->nfnt, 0 );
		PRINTTOOL_PrintColor(
			&wk->win[DPCBMP_WINID_INFO], wk->que,
			LEVEL_PX, LEVEL_PY+4, str, wk->nfnt, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
		GFL_STR_DeleteBuffer( str );

		str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_04 );
		WORDSET_RegisterNumber( wk->wset, 0, poke->level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wk->wset, wk->exp, str );
		PRINTTOOL_PrintColor(
			&wk->win[DPCBMP_WINID_INFO], wk->que,
			LEVEL_PX+npx, LEVEL_PY, wk->exp, wk->font, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
		GFL_STR_DeleteBuffer( str );
	}

	// �j�b�N�l�[��
	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_05 );
	WORDSET_RegisterWord( wk->wset, 0, poke->nickname, 0, TRUE, PM_LANG );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTTOOL_PrintColor(
		&wk->win[DPCBMP_WINID_INFO], wk->que,
		NICKNAME_PX, NICKNAME_PY, wk->exp, wk->font, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
	GFL_STR_DeleteBuffer( str );

	// �e��
	str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_07 );
	WORDSET_RegisterWord( wk->wset, 0, poke->oyaname, 0, TRUE, PM_LANG );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	PRINTTOOL_PrintColor(
		&wk->win[DPCBMP_WINID_INFO], wk->que,
		OYA_PX, OYA_PY, wk->exp, wk->font, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
	GFL_STR_DeleteBuffer( str );

	// �Z��
	for( i=0; i<4; i++ ){
		str = GFL_MSG_CreateString( wk->mman, mes_pc_dendou_02_09+i );
		WORDSET_RegisterWazaName( wk->wset, 0, poke->waza[i] );
		WORDSET_ExpandStr( wk->wset, wk->exp, str );
		PRINTTOOL_PrintColor(
			&wk->win[DPCBMP_WINID_INFO], wk->que,
			WAZA_PX+WAZA_SX*(i&1), WAZA_PY+WAZA_SY*(i/2), wk->exp, wk->font, FCOL_SP01WN, PRINTTOOL_MODE_LEFT );
		GFL_STR_DeleteBuffer( str );
	}

	PrintScreenTrans( &wk->win[DPCBMP_WINID_INFO] );
}

void DPCBMP_ClearInfo( DPCMAIN_WORK * wk )
{
	GFL_BMPWIN_ClearScreen( wk->win[DPCBMP_WINID_INFO].win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->win[DPCBMP_WINID_INFO].win) );
}
