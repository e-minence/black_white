//============================================================================================
/**
 * @file		zknlist_bmp.c
 * @brief		�}�Ӄ��X�g��� �a�l�o�֘A
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZKNLISTBMP
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "ui/print_tool.h"
#include "msg/msg_zukan_list.h"

#include "zknlist_main.h"
#include "zknlist_bmp.h"
#include "zukan_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// �t�H���g�J���[
#define FCOL_WP01WN		( PRINTSYS_LSB_Make(14,15,0) )	// �t�H���g�J���[�F���p�O�P����
#define FCOL_SP00WN		( PRINTSYS_LSB_Make(14,15,0) )	// �t�H���g�J���[�F�T�u�O�O����
#define FCOL_SP00BN		( PRINTSYS_LSB_Make(12,13,0) )	// �t�H���g�J���[�F�T�u�O�O����
#define FCOL_SP15WN		( PRINTSYS_LSB_Make(15,2,0) )		// �t�H���g�J���[�F�T�u�P�T����

// BMPWIN�֘A
// �|�P������
#define	BMPWIN_NAME_M_FRM		( GFL_BG_FRAME2_M )
#define	BMPWIN_NAME_S_FRM		( GFL_BG_FRAME2_S )
#define	BMPWIN_NAME_PX			( 3 )
#define	BMPWIN_NAME_PY			( 0 )
#define	BMPWIN_NAME_SX			( 11 )
#define	BMPWIN_NAME_SY			( 3 )
#define	BMPWIN_NAME_M_PAL		( 1 )
#define	BMPWIN_NAME_S_PAL		( 1 )
// �^�C�g��
#define	BMPWIN_TITLE_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_TITLE_PX			( 1 )
#define	BMPWIN_TITLE_PY			( 0 )
#define	BMPWIN_TITLE_SX			( 17 )
#define	BMPWIN_TITLE_SY			( 3 )
#define	BMPWIN_TITLE_PAL		( 4 )
// ��������
#define	BMPWIN_SEENUM_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_SEENUM_PX		( 2 )
#define	BMPWIN_SEENUM_PY		( 3 )
#define	BMPWIN_SEENUM_SX		( 13 )
#define	BMPWIN_SEENUM_SY		( 3 )
#define	BMPWIN_SEENUM_PAL		( 4 )
// �߂܂�����
#define	BMPWIN_GETNUM_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_GETNUM_PX		( 16 )
#define	BMPWIN_GETNUM_PY		( 3 )
#define	BMPWIN_GETNUM_SX		( 15 )
#define	BMPWIN_GETNUM_SY		( 3 )
#define	BMPWIN_GETNUM_PAL		( 4 )


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static void PutTitleStr( ZKNLISTMAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// BMPWIN�f�[�^
static const u8	BmpWinData[][6] =
{
	{	// �^�C�g��
		BMPWIN_TITLE_FRM, BMPWIN_TITLE_PX, BMPWIN_TITLE_PY,
		BMPWIN_TITLE_SX, BMPWIN_TITLE_SY, BMPWIN_TITLE_PAL
	},
	{	// ��������
		BMPWIN_SEENUM_FRM, BMPWIN_SEENUM_PX, BMPWIN_SEENUM_PY,
		BMPWIN_SEENUM_SX, BMPWIN_SEENUM_SY, BMPWIN_SEENUM_PAL
	},
	{	// �߂܂�����
		BMPWIN_GETNUM_FRM, BMPWIN_GETNUM_PX, BMPWIN_GETNUM_PY,
		BMPWIN_GETNUM_SX, BMPWIN_GETNUM_SY, BMPWIN_GETNUM_PAL
	}
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A������
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_Init( ZKNLISTMAIN_WORK * wk )
{
	u32	i;

	GFL_BMPWIN_Init( HEAPID_ZUKAN_LIST );

	for( i=0; i<ZKNLISTBMP_WINIDX_MAX; i++ ){
		const u8 * dat = BmpWinData[i];
		wk->win[i].win = GFL_BMPWIN_Create(
											dat[0], dat[1], dat[2], dat[3], dat[4], dat[5], GFL_BMP_CHRAREA_GET_B );
	}

	PutTitleStr( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�a�l�o�֘A�폜
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_Exit( ZKNLISTMAIN_WORK * wk )
{
	u32	i;

	for( i=0; i<ZKNLISTBMP_WINIDX_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->win[i].win );
	}

	GFL_BMPWIN_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v�����g���C��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_PrintUtilTrans( ZKNLISTMAIN_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<ZKNLISTBMP_WINIDX_MAX; i++ ){
		PRINT_UTIL_Trans( &wk->win[i], wk->que );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		������\���i�Œ蕶����j
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		winID		BMPWIN ID
 * @param		msgID		������h�c
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		col			�J���[
 * @param		mode		�\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void StrPrint(
							ZKNLISTMAIN_WORK * wk, u32 winID,
							u32 msgID, u32 x, u32 y, PRINTSYS_LSB col, u32 mode )
{
	STRBUF * str = GFL_MSG_CreateString( wk->mman, msgID );

	PRINTTOOL_PrintColor( &wk->win[winID], wk->que, x, y, str, wk->font, col, mode );

	GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		������\���i�ϕ�����j
 *
 * @param		wk			�}�Ӄ��X�g���[�N
 * @param		winID		BMPWIN ID
 * @param		msgID		������h�c
 * @param		x				�\����w���W
 * @param		y				�\����x���W
 * @param		col			�J���[
 * @param		mode		�\�����[�h
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ExStrPrint(
							ZKNLISTMAIN_WORK * wk, u32 winID,
							u32 msgID, u32 x, u32 y, PRINTSYS_LSB col, u32 mode )
{
	STRBUF * str = GFL_MSG_CreateString( wk->mman, msgID );

	WORDSET_ExpandStr( wk->wset, wk->exp, str );

	PRINTTOOL_PrintColor( &wk->win[winID], wk->que, x, y, wk->exp, wk->font, col, mode );

	GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMPWIN�X�N���[���]��
 *
 * @param		util		PRINT_UTIL
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PrintScreenTrans( PRINT_UTIL * util )
{
	GFL_BMPWIN_MakeScreen( util->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(util->win) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�T�u��ʂ̕�����\��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		idx		BMPWIN index
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutSubDispStr( ZKNLISTMAIN_WORK * wk, u32 idx )
{
	ARCHANDLE * ah;

	NNSG2dCharacterData * chr;
	void * chrBuff;
	u8 * chrData;

	NNSG2dScreenData * scrn;
	void * scrnBuff;
	u16 * scrnData;

	u8 * bmpBuff;
	u8	px, py, sx, sy;

	u16	i, j;

	sx = GFL_BMPWIN_GetScreenSizeX( wk->win[idx].win );
	sy = GFL_BMPWIN_GetScreenSizeY( wk->win[idx].win );
	px = GFL_BMPWIN_GetPosX( wk->win[idx].win );
	py = GFL_BMPWIN_GetPosY( wk->win[idx].win );

	bmpBuff = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->win[idx].win) );

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST );

	scrnBuff = GFL_ARCHDL_UTIL_LoadScreen(
							ah, NARC_zukan_gra_list_listtitle_bgu_NSCR, FALSE, &scrn, HEAPID_ZUKAN_LIST_L );
	scrnData = (u16 *)scrn->rawData;

	chrBuff = GFL_ARCHDL_UTIL_LoadBGCharacter(
							ah, NARC_zukan_gra_list_listbase_bgu_NCGR, FALSE, &chr, HEAPID_ZUKAN_LIST_L );
	chrData = (u8 *)chr->pRawData;

	GFL_ARC_CloseDataHandle( ah );

	for( i=0; i<sy; i++ ){
		for( j=0; j<sx; j ++ ){
			GFL_STD_MemCopy32( &chrData[(scrnData[(py+i)*32+px+j]&0x3ff)*0x20], &bmpBuff[(i*sx+j)*0x20], 0x20 );
		}
	}

	GFL_HEAP_FreeMemory( chrBuff );
	GFL_HEAP_FreeMemory( scrnBuff );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�^�C�g���\��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutTitleStr( ZKNLISTMAIN_WORK * wk )
{
	PutSubDispStr( wk, ZKNLISTBMP_WINIDX_TITLE );

	// �S���}��
	if( ZUKANSAVE_GetZukanMode( wk->dat->savedata ) == TRUE ){
		StrPrint( wk, ZKNLISTBMP_WINIDX_TITLE, str_title_01, 0, 4, FCOL_SP00WN, PRINTTOOL_MODE_LEFT );
	// �C�b�V���}��
	}else{
		StrPrint( wk, ZKNLISTBMP_WINIDX_TITLE, str_title_00, 0, 4, FCOL_SP00WN, PRINTTOOL_MODE_LEFT );
	}
	PrintScreenTrans( &wk->win[ZKNLISTBMP_WINIDX_TITLE] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�|�P�����o�^���\���i�ߊl���E�������j
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_PutPokeEntryStr( ZKNLISTMAIN_WORK * wk )
{
	u32	see, get;

	// �S��
	if( ZUKANSAVE_GetZukanMode( wk->dat->savedata ) == TRUE ){
		see = ZUKANSAVE_GetPokeSeeCount( wk->dat->savedata );
		get = ZUKANSAVE_GetPokeGetCount( wk->dat->savedata );
	// �C�b�V��
	}else{
		see = ZUKANSAVE_GetLocalPokeSeeCount( wk->dat->savedata, HEAPID_ZUKAN_LIST_L );
		get = ZUKANSAVE_GetLocalPokeGetCount( wk->dat->savedata, HEAPID_ZUKAN_LIST_L );
	}

	PutSubDispStr( wk, ZKNLISTBMP_WINIDX_SEENUM );
	PutSubDispStr( wk, ZKNLISTBMP_WINIDX_GETNUM );

	StrPrint( wk, ZKNLISTBMP_WINIDX_SEENUM, str_see, 9*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );
	WORDSET_RegisterNumber( wk->wset, 0, see, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	ExStrPrint( wk, ZKNLISTBMP_WINIDX_SEENUM, str_see_get_num, 13*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );

	StrPrint( wk, ZKNLISTBMP_WINIDX_GETNUM, str_get, 11*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );
	WORDSET_RegisterNumber( wk->wset, 0, get, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	ExStrPrint( wk, ZKNLISTBMP_WINIDX_GETNUM, str_see_get_num, 15*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );

	PrintScreenTrans( &wk->win[ZKNLISTBMP_WINIDX_SEENUM] );
	PrintScreenTrans( &wk->win[ZKNLISTBMP_WINIDX_GETNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�������ʐ��\��
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_PutPokeSearchStr( ZKNLISTMAIN_WORK * wk )
{
	PutSubDispStr( wk, ZKNLISTBMP_WINIDX_GETNUM );

	StrPrint( wk, ZKNLISTBMP_WINIDX_GETNUM, str_search, 11*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );
	WORDSET_RegisterNumber(
		wk->wset, 0, wk->dat->listMax, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	ExStrPrint( wk, ZKNLISTBMP_WINIDX_GETNUM, str_see_get_num, 15*8, 4, FCOL_SP00BN, PRINTTOOL_MODE_RIGHT );

	PrintScreenTrans( &wk->win[ZKNLISTBMP_WINIDX_GETNUM] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���X�g��̕�����\���i�|�P�������E�}�Ӕԍ��j
 *
 * @param		wk		�}�Ӄ��X�g���[�N
 * @param		util	PRINT_UTIL
 * @param		name	�|�P������
 * @param		num		�}�Ӕԍ�
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTBMP_PutPokeList( ZKNLISTMAIN_WORK * wk, PRINT_UTIL * util, STRBUF * name, u32 num )
{
	STRBUF * str;
	PRINT_QUE * que;

	que = FRAMELIST_GetPrintQue( wk->lwk );

	// �}�Ӕԍ�
	if( ZUKANSAVE_GetZukanMode( wk->dat->savedata ) == FALSE ){
		num = wk->localNo[num];
	}
	str = GFL_MSG_CreateString( wk->mman, str_poek_num );
	WORDSET_RegisterNumber( wk->wset, 0, num, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->exp, str );
	GFL_STR_DeleteBuffer( str );
	PRINTTOOL_PrintColor( util, que, 0, 4, wk->exp, wk->font, FCOL_WP01WN, PRINTTOOL_MODE_LEFT );

	// ���O
	PRINTTOOL_PrintColor( util, que, 28, 4, name, wk->font, FCOL_WP01WN, PRINTTOOL_MODE_LEFT );
}
