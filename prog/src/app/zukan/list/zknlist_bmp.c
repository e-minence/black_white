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

#include "zknlist_main.h"
#include "zknlist_bmp.h"


//============================================================================================
//	�萔��`
//============================================================================================

// BMPWIN�֘A
// �|�P������
#define	BMPWIN_NAME_M_FRM		( GFL_BG_FRAME2_M )
#define	BMPWIN_NAME_S_FRM		( GFL_BG_FRAME2_S )
#define	BMPWIN_NAME_PX			( 3 )
#define	BMPWIN_NAME_PY			( 0 )
#define	BMPWIN_NAME_SX			( 11 )
#define	BMPWIN_NAME_SY			( 3 )
#define	BMPWIN_NAME_M_PAL		( 0 )
#define	BMPWIN_NAME_S_PAL		( 0 )

// �^�C�g��
#define	BMPWIN_TITLE_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_TITLE_PX			( 1 )
#define	BMPWIN_TITLE_PY			( 0 )
#define	BMPWIN_TITLE_SX			( 12 )
#define	BMPWIN_TITLE_SY			( 3 )
#define	BMPWIN_TITLE_PAL		( 0 )

// ��������
#define	BMPWIN_SEENUM_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_SEENUM_PX		( 1 )
#define	BMPWIN_SEENUM_PY		( 3 )
#define	BMPWIN_SEENUM_SX		( 15 )
#define	BMPWIN_SEENUM_SY		( 3 )
#define	BMPWIN_SEENUM_PAL		( 0 )

// �߂܂�����
#define	BMPWIN_GETNUM_FRM		( GFL_BG_FRAME1_S )
#define	BMPWIN_GETNUM_PX		( 17 )
#define	BMPWIN_GETNUM_PY		( 3 )
#define	BMPWIN_GETNUM_SX		( 15 )
#define	BMPWIN_GETNUM_SY		( 3 )
#define	BMPWIN_GETNUM_PAL		( 0 )


//============================================================================================
//	�O���[�o��
//============================================================================================

static const u8	BoxBmpWinData[][6] =
{
	{	// �|�P�������i���C����ʁj
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},
	{
		BMPWIN_NAME_M_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_M_PAL
	},

	{	// �|�P�������i�T�u��ʁj
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},
	{
		BMPWIN_NAME_S_FRM, BMPWIN_NAME_PX, BMPWIN_NAME_PY,
		BMPWIN_NAME_SX, BMPWIN_NAME_SY, BMPWIN_NAME_S_PAL
	},

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
		const u8 * dat = BoxBmpWinData[i];
		wk->win[i].win = GFL_BMPWIN_Create(
											dat[0], dat[1], dat[2], dat[3], dat[4], dat[5], GFL_BMP_CHRAREA_GET_B );
	}
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
