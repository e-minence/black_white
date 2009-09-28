//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		worldtrade_adapter.c
 *	@brief	GTS���ڐA����ۂ̃p�b�N�֐��Q
 *	@author	Toru=Nagihashi
 *	@data		2009.08.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>
//system
#include "system/main.h"
#include "system/bmp_winframe.h"
#include "libdpw/dpw_tr.h"

//mine
#include "worldtrade_adapter.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
#define POKEMON_DATA_SIZE		(236)
//POKETOOL_GetWorkSize()
//savedata\worldtrade\worldtrade_data.c  15

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PPP����j�b�N�l�[����o�^����֐�
 *
 *	@param	WORDSET* wordset	���[�h�Z�b�g
 *	@param	bufID							�o�b�t�@ID
 *	@param	POKEMON_PASO_PARAM * ppp	PPP
 */
//-----------------------------------------------------------------------------
void WT_WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM * ppp )
{	
	POKEMON_PARAM *pp	= PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
	WORDSET_RegisterPokeNickName( wordset, bufID, pp );
	GFL_HEAP_FreeMemory( pp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����p�����[�^���̃������m��
 *	
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	PP�\���̕��̃�����
 */
//-----------------------------------------------------------------------------
POKEMON_PARAM *PokemonParam_AllocWork( HEAPID heapID )
{	
	return GFL_HEAP_AllocMemory( heapID, POKEMON_DATA_SIZE );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����p�����[�^�̃R�s�[
 *
 *	@param	const POKEMON_PARAM *pp_src
 *	@param	*pp_dest 
 */
//-----------------------------------------------------------------------------
void WT_PokeCopyPPtoPP( const POKEMON_PARAM *pp_src, POKEMON_PARAM *pp_dest )
{	
	GFL_STD_MemCopy( pp_src, pp_dest, POKEMON_DATA_SIZE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���A
 *
 *	@param	GFL_BMPWIN * winGFLBMPWIN
 *	@param	trans_sw				�]���t���O
 */
//-----------------------------------------------------------------------------
void BmpMenuWinClear( GFL_BMPWIN * win, u8 trans_sw )
{	
	GFL_BMPWIN_ClearScreen( win );
	switch( trans_sw )
	{	
	case WINDOW_TRANS_ON:
		GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(win) );
		break;
	case WINDOW_TRANS_ON_V:
		GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(win) );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	������W�J����MSG���擾
 *
 *	@param	WORDSET *wordset	���[�h�Z�b�g
 *	@param	*MsgManager				���b�Z�[�W
 *	@param	strID							����ID
 *	@param	heapID						�e���|�����q�[�vID
 *
 *	@return	������W�J���STRBUF
 */
//-----------------------------------------------------------------------------
STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID )
{	
	STRBUF	*src;
	STRBUF	*dst;
	src	= GFL_MSG_CreateString( MsgManager, strID );
	dst	= GFL_STR_CreateBuffer( GFL_STR_GetBufferLength( src ), heapID );

	WORDSET_ExpandStr( wordset, src, dst );

	GFL_STR_DeleteBuffer( src );

	return dst;
}

//----------------------------------------------------------------------------
/**
 *	@brief	ppp����pp���쐬���A�R�s�[����
 *
 *	@param	const POKEMON_PASO_PARAM *ppp
 *	@param	*pp 
 */
//-----------------------------------------------------------------------------
void PokeReplace( const POKEMON_PASO_PARAM *ppp, POKEMON_PARAM *pp )
{	
	POKEMON_PARAM*  src	= PP_CreateByPPP( ppp, HEAPID_WORLDTRADE );
	WT_PokeCopyPPtoPP( src, pp );
	GFL_HEAP_FreeMemory( src );
}


//----------------------------------------------------------------------------
/**
 *	@brief	PP2PPP
 *
 *	@param	POKEMON_PARAM *pp 
 *
 *	@return	PPP
 */
//-----------------------------------------------------------------------------
POKEMON_PASO_PARAM* PPPPointerGet( POKEMON_PARAM *pp )
{	
	return (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst( pp );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�v���t�@�C�����쐬
 *
 *	@param	SAVE_CONTROL_WORK * savedata	�Z�[�u�f�[�^
 *	@param	* profile											�v���t�@�C��
 *
 */
//-----------------------------------------------------------------------------
static void EMAILSAVE_DCProfileCreateCommon( SAVE_CONTROL_WORK *sv, Dpw_Common_Profile *dc_profile )
{
	WIFI_HISTORY *wh = SaveData_GetWifiHistory(sv);
	MYSTATUS *my = SaveData_GetMyStatus(sv);
	//char *email_address = EMAILSAVE_AddressGet(sv);
	
	GFL_STD_MemClear(dc_profile, sizeof(Dpw_Common_Profile));
	
	dc_profile->version = PM_VERSION;
	dc_profile->language = PM_LANG;
	dc_profile->countryCode = WIFIHISTORY_GetMyNation(wh);
	dc_profile->localCode = WIFIHISTORY_GetMyArea(wh);
	dc_profile->playerId = MyStatus_GetID(my);
	
	STRTOOL_Copy( MyStatus_GetMyName(my), dc_profile->playerName, DPW_TR_NAME_SIZE );
	
	dc_profile->flag = 0;	//�n���O��������\���ł��邩
//	dc_profile->macAddr		���C�u�������Ŋi�[����̂ŃZ�b�g�̕K�v�Ȃ�
	
	//strcpy(dc_profile->mailAddr, email_address);

#if 0
	dc_profile->mailRecvFlag = FALSE;////EMAILSAVE_ParamGet(sv, EMAIL_PARAM_RECV_FLAG);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EMAIL)
	SVLD_SetCrc(GMDATA_ID_EMAIL);
#endif //CRC_LOADCHECK
#endif
}
void EMAILSAVE_DCProfileCreate_Update( SAVE_CONTROL_WORK * savedata, Dpw_Common_Profile * profile )
{
	EMAILSAVE_DCProfileCreateCommon( savedata, profile );
}

//=============================================================================
/**
 *					�v�����g�t�H���g����
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	������̕���Ԃ�
 *
 *	@param	WT_PRINT *wk	�v�����g���
 *	@param	*buf		������
 *	@param	magin		�}�[�W��
 *
 *	@return	��
 */
//-----------------------------------------------------------------------------
int FontProc_GetPrintStrWidth( WT_PRINT *wk, u8 font_idx, STRBUF *buf, int magin )
{ 
	return PRINTSYS_GetStrWidth( buf, wk->font, magin );
}

//----------------------------------------------------------------------------
/**
 *	@brief	������\���ɕK�v�ȏ���������
 *
 *	@param	WT_PRINT *wk	���[�N
 *	@param	CONFIG *cfg		�R���t�B�O
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Init( WT_PRINT *wk, const CONFIG *cfg )
{	
	GFL_STD_MemClear( wk, sizeof(WT_PRINT) );

	wk->tcbsys	= GFL_TCBL_Init( HEAPID_WORLDTRADE, HEAPID_WORLDTRADE, 32, 32 );
	wk->cfg			= cfg;
	wk->font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WORLDTRADE );


	{	
		int i;
		WT_PRINT_QUE	*p_one;
		for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
		{	
			p_one	= &wk->one[i];
			p_one->que		= PRINTSYS_QUE_Create( HEAPID_WORLDTRADE );
			GFL_STD_MemClear( &p_one->util, sizeof(PRINT_UTIL) );
			p_one->use		= FALSE;
		}
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	��������ɕK�v�ȏ���j��
 *
 *	@param	WT_PRINT *wk	���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Exit( WT_PRINT *wk )
{	
	{	
		int i;
		WT_PRINT_QUE	*p_one;
		for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
		{	
			p_one	= &wk->one[i];
			PRINTSYS_QUE_Delete( p_one->que );
		}
	}
	
	GFL_FONT_Delete( wk->font );
	GFL_TCBL_Exit( wk->tcbsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	������\��ɕK�v�ȏ��	���C������
 *
 *	@param	WT_PRINT *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WT_PRINT_Main( WT_PRINT *wk )
{	
	{	
		int i;
		WT_PRINT_QUE	*p_one;
		for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
		{	
			p_one	= &wk->one[i];
			if( p_one->use )
			{
				if( PRINT_UTIL_Trans( &p_one->util, p_one->que ) )
				{	
					p_one->use	= FALSE;
				}
			}

			if( wk->stream[i] != NULL )
			{	
				if( PRINTSYS_PrintStreamGetState(wk->stream[i]) == PRINTSTREAM_STATE_DONE )
				{	
					PRINTSYS_PrintStreamDelete( wk->stream[i] );
					wk->stream[i]	= NULL;
				}
			}
		}
	}


	GFL_TCBL_Main( wk->tcbsys );
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PRINT_STREAM *stream	�X�g���[��
 *
 *	@return	FALSE�Ȃ�Ε�����`��I��	TRUE�Ȃ�Ώ�����
 */
//-----------------------------------------------------------------------------
BOOL GF_MSG_PrintEndCheck( WT_PRINT *setup )
{
	int i;
	for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
	{	

		if( setup->stream[i] != NULL )
		{	
			if( PRINTSYS_PrintStreamGetState(setup->stream[i]) == PRINTSTREAM_STATE_DONE )
			{	
				return FALSE;
			}
			else
			{	
				return TRUE;
			}
		}
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	GFL_BMPWIN *bmpwin	BMPWIN
 *	@param	*str	������
 *	@param	x			�\���ʒuX
 *	@param	y			�\���ʒuY
 *	@param	WT_PRINT *setup		������ɕK�v�ȏ��
 *
 *	@return	�v�����g�X�g���[��
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintSimple( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, WT_PRINT *setup )
{
	int i;
	{
		for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
		{	
			if( setup->stream[i] == NULL )
			{
				break;
			}
		}
		GF_ASSERT( i < WT_PRINT_BUFF_MAX );
	}

	setup->stream[i]	= PRINTSYS_PrintStream(
    bmpwin, x, y, str, setup->font,
    CONFIG_GetMsgPrintSpeed( setup->cfg ), setup->tcbsys, 0, HEAPID_WORLDTRADE, 0x0f );
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	�F���ꊇ�\��
 *
 *	@param	GFL_BMPWIN *bmpwin	��������BMPWIN
 *	@param	*str	������
 *	@param	x			X
 *	@param	y			Y
 *	@param	color	�F
 *	@param	*setup	������p���
 */
//-----------------------------------------------------------------------------
void GF_STR_PrintColor( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, int put_type, PRINTSYS_LSB color, WT_PRINT *setup )
{
	WT_PRINT_QUE	*p_one	= NULL;
	
	{	
		int i;
		for( i = 0; i < WT_PRINT_BUFF_MAX; i++ )
		{	
			if( !setup->one[i].use )
			{	
				p_one = &setup->one[i];
			}
		}
	}

	GF_ASSERT( p_one != NULL );

	PRINT_UTIL_Setup( &p_one->util, bmpwin );
	PRINT_UTIL_PrintColor( &p_one->util, p_one->que, x, y, str, setup->font, color );

	p_one->use	= TRUE;
}

//=============================================================================
/**
 *		BGWINFRM
 */
//=============================================================================
#if 0
//-------------------------------------
///		WIN_FRM
//=====================================
typedef struct {
	u16 * scr;			// �X�N���[���f�[�^
	u16	sx:6;			// �X�N���[���̂w�T�C�Y
	u16	sy:6;			// �X�N���[���̂x�T�C�Y
	u16	vanish:1;		// �\���t���O�@1 = ON, 0 = OFF
	u16	auto_flg:3;		// �����ړ��t���O�@0 = ON, 1 = OFF
	// 2bit
	s8	px;				// �\���w���W
	s8	py;				// �\���x���W

	s8	mvx;			// �P�t���[���̂w�ړ���
	s8	mvy;			// �P�t���[���̂x�ړ���
	u8	frm;			// �a�f�t���[��
	u8	cnt;			// ����J�E���^

	s8	area_lx;		// �`��͈�
	s8	area_rx;
	s8	area_uy;
	s8	area_dy;

}BGWINFRM;

//-------------------------------------
///		���C�����[�N
//=====================================
struct _BGWINFRM_WORK
{
	BGWINFRM * win;		// �t���[���f�[�^
	u16 max;			// �t���[����
	u16	trans_mode:15;	// �]�����[�h
	u16	auto_master:1;	// �����ړ��}�X�^�[�t���O�@0 = ON, 1 = OFF
	u32	heapID;			// �q�[�v�h�c
};

//----------------------------------------------------------------------------
/**
 *	@brief	BGWINFRM�̍쐬
 *
 *	@param	int mode	�g�p���Ȃ�	�iVBlankOnly�j
 *	@param	max				�t���[���̐�
 *	@param	heapID		�q�[�vID
 *
 *	@return	BGWINFRM_WORK
 */
//-----------------------------------------------------------------------------
BGWINFRM_WORK * BGWINFRM_Create( int mode, int max, HEAPID heapID )
{
	BGWINFRM_WORK *p_wk;
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BGWINFRM_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(BGWINFRM_WORK) );
	p_wk->max	= max;
	p_wk->trans_mode	= mode;
	p_wk->auto_master	= 0;

	p_wk->win	= GFL_HEAP_AllocMemory( heapID, sizeof(BGWINFRM) * max );
	GFL_STD_MemClear( p_wk->win, sizeof(BGWINFRM) * max );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	BGWINFRM_WORK�̔j��
 *
 *	@param	BGWINFRM_WORK *wk		���[�N
 */
//-----------------------------------------------------------------------------
void BGWINFRM_Exit( BGWINFRM_WORK *wk )
{
	int i;
	for( i = 0; i < wk->max; i++ )
	{	
		if( wk->win[i].scr != NULL )
		{	
			GFL_HEAP_FreeMemory( wk->win[i].scr );
		}
	}

	GFL_HEAP_FreeMemory( wk->win );
	GFL_HEAP_FreeMemory( wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t���[���ǉ�
 *
 *	@param	BGWINFRM_WORK *wk	���[�N
 *	@param	index							�C���f�b�N�X
 *	@param	frm								�t���[��
 *	@param	sx								�T�C�YX
 *	@param	sy								�T�C�YY
 */
//-----------------------------------------------------------------------------
void BGWINFRM_Add( BGWINFRM_WORK *wk, int index, int frm, int sx, int sy )
{
	BGWINFRM * win;


	win = &wk->win[index];

	GF_ASSERT( win->scr == NULL );

	win->scr = GFL_HEAP_AllocMemory( wk->heapID, sx * sy * 2 );
	win->sx  = sx;
	win->sy  = sy;

	win->frm = frm;

	win->px  = 0;
	win->py  = 0;

	win->auto_flg = 0;

	win->vanish = FALSE;

	win->area_lx = 0;
	win->area_rx = 32;
	win->area_uy = 0;
	win->area_dy = 24;
}
//--------------------------------------------------------------------------------------------
/**
 * BMPWIN����������
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	win			BMPWIN
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_BmpWinOn( BGWINFRM_WORK *wk, int index, GFL_BMPWIN *win )
{
	u16 * scr;
	u32	sx;
	u16	i, j;
	u16	chr, pal;

	scr = wk->win[index].scr;
	sx  = wk->win[index].sx;

	chr = GFL_BMPWIN_GetChrNum( win );

	//pal = win->palnum << 12;
	pal	= GFL_BMPWIN_GetPalette(win) << 12;

	for( i=0; i<GFL_BMPWIN_GetSizeY(win); i++ ){
		if( ( GFL_BMPWIN_GetPosY(win) + i ) >= wk->win[index].sy ){
			break;
		}
		for( j=0; j<GFL_BMPWIN_GetSizeX(win); j++ ){
			if( ( GFL_BMPWIN_GetPosX(win) + j ) >= wk->win[index].sx ){
				break;
			}
			scr[ ( GFL_BMPWIN_GetPosY(win) + i ) * sx + GFL_BMPWIN_GetPosX(win) + j ] = chr + j + pal;
		}
		chr += GFL_BMPWIN_GetSizeX( win );
	}
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[���̕\���ʒu���擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	px			�w���W�i�[�ꏊ
 * @param	py			�x���W�i�[�ꏊ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_PosGet( BGWINFRM_WORK *wk, int index, s8 *px, s8 *py  )
{
	if( px != NULL ){ *px = wk->win[index].px; }
	if( py != NULL ){ *py = wk->win[index].py; }
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ����C���i�S�́j
 *
 * @param	wk			�t���[�����[�N
 *
 * @return	none
 *
 * @li	BGWINFRM_MoveOne() �Ɠ����Ɏg�p����� 2move/1frame �ɂȂ�̂Œ���
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_MoveMain( BGWINFRM_WORK *wk )
{
	u32	i;

	if( wk->auto_master == 0 ){ return; }

	wk->auto_master = 0;

	for( i=0; i<wk->max; i++ ){
		if( BGWINFRM_MoveOne( wk, i ) == 1 ){
			wk->auto_master = 1;
		}
	}
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ����C���i�ʁj
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @retval	"0 = ��~��"
 * @retval	"1 = �ړ���"
 *
 * @li	BGWINFRM_MoveMain() �Ɠ����Ɏg�p����� 2move/1frame �ɂȂ�̂Œ���
 */
//--------------------------------------------------------------------------------------------
u32 BGWINFRM_MoveOne( BGWINFRM_WORK * wk, u32 index )
{
	if( wk->win[index].auto_flg == 0 ){
		return 0;
	}

//	wk->auto_master = 0;

	BGWINFRM_FrameOff( wk, index );
	wk->win[index].px += wk->win[index].mvx;
	wk->win[index].py += wk->win[index].mvy;
	BGWINFRM_FrameOn( wk, index );
	wk->win[index].cnt--;
//	if( wk->win[index].cnt == 0 || wk->win[index].vanish == BGWINFRM_OFF ){
	if( wk->win[index].cnt == 0 ){
		wk->win[index].auto_flg = 0;
		return 0;
	}

//	wk->auto_master = 1;

	return 1;
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ��ݒ�
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	mvx			�w�ړ��ʁi�P�t���[���j
 * @param	mvy			�x�ړ��ʁi�P�t���[���j
 * @param	cnt			�ړ��t���[����
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_MoveInit( BGWINFRM_WORK *wk, int index, int add_x, int add_y, int cnt )
{
	BGWINFRM * win = &wk->win[index];

	win->mvx = add_x;
	win->mvy = add_y;
	win->cnt = cnt;

	win->auto_flg = 1;

	wk->auto_master = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[�������ړ��`�F�b�N
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @retval	"0 = ��~��"
 * @retval	"1 = �ړ���"
 */
//--------------------------------------------------------------------------------------------
u32 BGWINFRM_MoveCheck( BGWINFRM_WORK *wk, int index )
{
	return wk->win[index].auto_flg;
}

//--------------------------------------------------------------------------------------------
/**
 * �t���[���ɃX�N���[�����Z�b�g
 *
 * @param	wk		�t���[�����[�N
 * @param	index	�t���[���C���f�b�N�X
 * @param	scr		�X�N���[���f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameSet( BGWINFRM_WORK * wk, u32 index, u16 * scr )
{
	BGWINFRM * win = &wk->win[index];

	GFL_STD_MemCopy16( scr, win->scr, win->sx*win->sy*2 );
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[���ɃA�[�J�C�u�X�N���[���f�[�^���Z�b�g
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 * @param	fileIdx		�A�[�J�C�u�t�@�C���C���f�b�N�X
 * @param	dataIdx		�f�[�^�C���f�b�N�X
 * @param	comp		���k�t���O
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameSetArc( BGWINFRM_WORK *wk, int index, ARCID arcID, ARCDATID datID, int comp )
{
	NNSG2dScreenData * scrn;
	void * buf;

	buf = GFL_ARC_UTIL_LoadScreen( arcID, datID, comp, &scrn, wk->heapID );

	BGWINFRM_FrameSet( wk, index, (void *)scrn->rawData );

	GFL_HEAP_FreeMemory( buf );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t���[���v�b�g
 *
 *	@param	BGWINFRM_WORK *wk	���[�N
 *	@param	index							�C���f�b�N�X
 *	@param	px								�ʒuX
 *	@param	py								�ʒuY
 */
//-----------------------------------------------------------------------------
void BGWINFRM_FramePut( BGWINFRM_WORK *wk, int index, int px, int py )
{
	BGWINFRM * win;
	u8	wpx, wpy, wsx, wsy, bpx, bpy;

	win = &wk->win[index];

	win->px = px;
	win->py = py;

	if( px >= win->area_rx ||
		py >= win->area_dy ||
		px + win->sx < win->area_lx ||
		py + win->sy < win->area_uy ){
		return;
	}

	wpx = px;
	wsx = win->sx;
	bpx = 0;

	if( px < win->area_lx ){
		wpx = win->area_lx;
		wsx -= ( win->area_lx - px );
		bpx = win->area_lx - px;
	}
	if( px + win->sx >= win->area_rx ){
		wsx -= ( px + win->sx - win->area_rx );
	}

	wpy = py;
	wsy = win->sy;
	bpy = 0;

	if( py < win->area_uy ){
		wpy = win->area_uy;
		wsy -= ( win->area_uy - py );
		bpy = win->area_uy - py;
	}
	if( py + win->sy >= win->area_dy ){
		wsy -= ( py + win->sy - win->area_dy );
	}

	GFL_BG_WriteScreenExpand( win->frm, wpx, wpy, wsx, wsy, win->scr, bpx, bpy, win->sx, win->sy );
	GFL_BG_LoadScreenV_Req( win->frm );

	win->vanish = TRUE;
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[���̃X�N���[�����擾
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 *
 * @return	�X�N���[���̃|�C���^
 */
//--------------------------------------------------------------------------------------------
u16 * BGWINFRM_FrameBufGet( BGWINFRM_WORK *wk, int index )
{
	return wk->win[index].scr;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�t���[���\��
 *
 *	@param	BGWINFRM_WORK *wk	���[�N
 *	@param	index							�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
void BGWINFRM_FrameOn( BGWINFRM_WORK *wk, int index )
{
	BGWINFRM_FramePut( wk, index, wk->win[index].px, wk->win[index].py );
}
//--------------------------------------------------------------------------------------------
/**
 * �t���[�����\��
 *
 * @param	wk			�t���[�����[�N
 * @param	index		�t���[���C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
void BGWINFRM_FrameOff( BGWINFRM_WORK *wk, int index )
{
	BGWINFRM * win;
	s8	px, py, sx, sy;

	win = &wk->win[index];

	win->vanish = FALSE;

	if( win->px >= win->area_rx ||
		win->py >= win->area_dy ||
		win->px + win->sx < win->area_lx ||
		win->py + win->sy < win->area_uy ){
		return;
	}

	px = win->px;
	sx = win->sx;

	if( win->px < win->area_lx ){
		px = win->area_lx;
		sx -= ( win->area_lx - win->px );
	}
	if( win->px + win->sx >= win->area_rx ){
		sx -= ( win->px + win->sx - win->area_rx );
	}

	py = win->py;
	sy = win->sy;

	if( win->py < win->area_uy ){
		py = win->area_uy;
		sy -= ( win->area_uy - win->py );
	}
	if( win->py + win->sy >= win->area_dy ){
		sy -= ( win->py + win->sy - win->area_dy );
	}

	GFL_BG_FillScreen( win->frm, 0, px, py, sx, sy, GFL_BG_SCRWRT_PALNL );
	GFL_BG_LoadScreenV_Req( win->frm );
}
#endif
