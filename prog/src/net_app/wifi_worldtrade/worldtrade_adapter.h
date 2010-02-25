//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		worldtrade_adapter.h
 *	@brief	GTS���ڐA����ۂ̃p�b�N�֐��Q
 *	@author	Toru=Nagihashi
 *	@date		2009.08.05
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
//lib
#include <gflib.h>
#include <dwc.h>

//system
#include "print/wordset.h"
#include "print/printsys.h"
#include "system/selbox.h"
#include "system/net_err.h"

#include <dpw_common.h>

//savedata
#include "savedata/config.h"
#include "savedata/wifihistory.h"
#include "savedata/myitem_savedata.h"
#include "savedata/record.h"
#include "field/eventdata_system.h"

#include "gamesystem/game_data.h"

//tool
#include "poke_tool/poke_tool.h"
#include "pokeicon/pokeicon.h"

#include "net_app/net_bugfix.h"

//=============================================================================
/**
 *					�f�o�b�O�}�N��
*/
//=============================================================================
#ifdef PM_DEBUG

#define CHANGE_POKE_RULE_IGNORE	//�����p�|�P���������ʂƈ�v���Ă��Ȃ��Ă�����ׂ�
//#define RESERVE_POKE_GS_BINARY	//�T�[�o�[�ɗa����|�P������GS����쐬�����o�C�i�����g��
//#define DEBUG_SAVE_NONE					//�f�o�b�O�p�ɃZ�[�u���Ȃ��Ői�݂܂�


#ifdef DEBUG_ONLY_FOR_toru_nagihashi	//�S���҂��ς������ς��Ă�������
#define DEBUG_AUTHER_ONLY				//����ON�ɂȂ��Ă����adapter�ȊO�ōs���Ă��Ȃ��Ƃ�������[�j���O�Œm�点�܂�
#endif //PM_DEBUG

//�S���҃v�����g
#ifdef DEBUG_AUTHER_ONLY
#define MORI_PRINT(...)				OS_Printf( __VA_ARGS__ )
#else
#define MORI_PRINT(...)				((void)0)
#endif //DEBUG_AUTHER_ONLY

//�A���Z�E�X�C�x���g�`�F�b�N
//#define ARUCEUSU_EVENT_CHECK

//PHC����`�F�b�N
//#define PHC_EVENT_CHECK


#endif //PM_DEBUG
//=============================================================================
/**
 *					��`
*/
//=============================================================================
#define _MAXNUM   (2)         // �ő�ڑ��l��
#define _MAXSIZE  (80)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��
#define COMM_DMA_NO						(_NETWORK_DMA_NO)
#define COMM_POWERMODE				(_NETWORK_POWERMODE)
#define COMM_SSL_PRIORITY			(_NETWORK_SSL_PRIORITY)
#define COMM_ERROR_RESET_GTS  (3)	//gflib/src/network/net_state.h�ɒ�`������
#define COMM_ERROR_RESET_OTHER (4)
#define COMM_ERRORTYPE_POWEROFF (1)   // �d����؂�Ȃ��Ƃ����Ȃ�
#define TEMOTI_POKEMAX				(6)
#define PARA_UNK							(2)
#define PARA_MALE							(0)
#define PARA_FEMALE						(1)
//=============================================================================
/**
 *					�u��������\�肾���A�܂���`����Ă��Ȃ��}�N��
*/
//=============================================================================

//�T�E���h	worldtrade_local�̒��ɂ�����܂�
#define SE_CANCEL							(SEQ_SE_SELECT1)	
#define SE_GTC_NG							(SEQ_SE_SELECT1)
#define SE_GTC_SEARCH					(SEQ_SE_SELECT1)
#define SE_GTC_PLAYER_IN			(SEQ_SE_SELECT1)
#define SE_GTC_ON							(SEQ_SE_SELECT1)
#define SE_GTC_OFF						(SEQ_SE_SELECT1)
#define SE_GTC_PLAYER_OUT			(SEQ_SE_SELECT1)
#define SE_GTC_APPEAR					(SEQ_SE_SELECT1)

#define FONT_TOUCH						(0)
#define FONT_SYSTEM						(0)
#define FONT_TALK							(0)

//���b�Z�[�W
#define MSG_TP_ON							(0)
#define MSG_TP_OFF						(0)


//NUMFONT
#define NUMFONT_MODE_LEFT			(0)
#define NUMFONT_MARK_SLASH		(0)

//PHC
#define PHC_WIFI_OPEN_COURSE_NO	(0)
typedef void PHC_SVDATA;


//=============================================================================
/**
 *					�u��������\�肾���A�܂���`����Ă��Ȃ��֐�
*/
//=============================================================================
typedef struct
{	
	int dummy;
}ZUKAN_WORK;

typedef struct
{	
	int dummy;
}NUMFONT;

//=============================================================================
/**
 *					�g�p���Ă��Ȃ����A�u������ςȂ̂Œ�`�ł��܂��Ă������
*/
//=============================================================================

#define MSG_NO_PUT						(0)
#define MSG_ALLPUT						(0)

//=============================================================================
/**
 *					�u��������\�肾���A�܂��Ȃ��A���g���`���Ă��Ȃ��֐�
*/
//=============================================================================
//�^�C���E�F�C�g�A�C�R��
static inline void * TimeWaitIconAdd( GFL_BMPWIN *bmpwin, int a ) {	return NULL; }
static inline void TimeWaitIconDel( void *wk ){}

//�T�E���h
static inline void Snd_DataSetByScene( int a, int b, int c ){}

//UI
static inline void MsgPrintTouchPanelFlagSet( int a ){}

//NUMFONT
static inline NUMFONT * NUMFONT_Create( int a, int b, int c, HEAPID heapID ){return NULL;}
static inline void NUMFONT_Delete( NUMFONT *wk ){}
static inline void NUMFONT_WriteNumber( NUMFONT *wk, int a, int b, int c, GFL_BMPWIN *bmpwin, int d, int e ){}
static inline void NUMFONT_WriteMark( NUMFONT *wk, int a, GFL_BMPWIN *bmpwin, int c, int d ){}


//�e��Z�[�u�f�[�^�@PHC
static inline PHC_SVDATA * SaveData_GetPhcSaveData( SAVE_CONTROL_WORK *sv ){	return NULL; }
static inline void PhcSvData_SetCourseOpenFlag( PHC_SVDATA *sv, int a ){}
static inline EVENTWORK* SaveData_GetEventWork( SAVE_CONTROL_WORK *sv ){ return NULL; }

//�A���Z�E�X�C�x���g
static inline int SysWork_AruseusuEventGet( EVENTWORK *ev ){	return 0;}
static inline void SysWork_AruseusuEventSet( EVENTWORK *ev, int a ){}

//�}��
static inline BOOL ZukanWork_GetZenkokuZukanFlag( ZUKAN_WORK *wk ){return TRUE;}
static inline BOOL ZukanWork_GetPokeSeeFlag( ZUKAN_WORK *zukan, int a ){return TRUE;}
static inline void SaveData_GetPokeRegister( SAVE_CONTROL_WORK *sv, POKEMON_PARAM *pp ){}

//=============================================================================
/**
 *					�ȉ��A�P���ɒu���������֐�
*/
//=============================================================================
static inline int PokeIconCgxArcIndexGetByMonsNumber( int pokeno, int tamago, int form )
{
	return POKEICON_GetCgxArcIndexByMonsNumber( pokeno, form, tamago ); 
}
static inline int PokeIconPalNumGet( int pokeno, int form, int tamago )
{
	return POKEICON_GetPalNum( pokeno, form, tamago );
}
static inline int PokePersonalParaGet(int mons_no, int param )
{ 
	u32 ret;

	POKEMON_PERSONAL_DATA *ppd;
		
	ppd	= POKE_PERSONAL_OpenHandle( mons_no, 0, HEAPID_WORLDTRADE );
	ret	= POKE_PERSONAL_GetParam( ppd, param );
	POKE_PERSONAL_CloseHandle( ppd );
	
	return ret;
}
static inline int PokePasoLevelCalc( POKEMON_PASO_PARAM *paso )
{
	return PPP_CalcLevel( paso ); 
}
//�ʐM�A�C�R��
static inline void WirelessIconEasy( void )
{
	//GFL_NET_ReloadIcon();  // �ڑ����Ȃ̂ŃA�C�R���\��
	GFL_NET_WirelessIconEasyXY(GFL_WICON_POSX, GFL_WICON_POSY, TRUE, HEAPID_WORLDTRADE);
	GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_WORLDTRADE );
}
static inline void WirelessIconEasy_SetLevel( int level )
{
	GFL_NET_WirelessIconEasy_SetLevel(level);
}
static inline void WirelessIconEasyEnd( void )
{
	GFL_NET_WirelessIconEasyEnd();
}


//�ʐM�G���[
static inline void CommStateSetError( int error )
{	
  GFL_NET_StateSetError( error );
	NetErr_ErrorSet();
}
static inline void CommFatalErrorFunc_NoNumber( void )
{
	NetErr_ErrorSet();
}
static inline void ComErrorWarningResetCall( int heapID, int type, int code )
{
  //@todo
	NetErr_ErrorSet();
}
static inline void Comm_WifiHistoryDataSet( WIFI_HISTORY *wifiHistory, int Nation, int Area, int langCode )
{
#if (PM_LANG == LANG_JAPAN)
	// ����̃J�[�g���b�W�����{�Ȃ̂�
	if(langCode==LANG_JAPAN){
		// �Ⴄ���̒n���V�R�[�h�����Ă�����o�^���Ȃ�
		if(Nation!=WIFI_NATION_JAPAN){
			return;
		}
	}
#endif
	
	// ���E�n���o�^
//	OS_Printf("no=%d\n", WIFIHISTORY_GetStat(wifiHistory, Nation, Area ));
	if(WIFIHISTORY_GetStat( wifiHistory, Nation, Area )==WIFIHIST_STAT_NODATA){
		WIFIHISTORY_SetStat( wifiHistory, Nation, Area, WIFIHIST_STAT_NEW );
	}
}
//=============================================================================
/**
 *					�ȉ��AWB�ɂ͂Ȃ��������ł���̂ŁA�쐬�����֐�
*/
//=============================================================================
extern void WT_WORDSET_RegisterPokeNickNamePPP( WORDSET* wordset, u32 bufID, const POKEMON_PASO_PARAM * ppp );
extern POKEMON_PARAM *PokemonParam_AllocWork( HEAPID heapID );
extern void WT_PokeCopyPPtoPP( const POKEMON_PARAM *pp_src, POKEMON_PARAM *pp_dest );
extern void BmpMenuWinClear( GFL_BMPWIN * win, u8 trans_sw );
extern STRBUF * MSGDAT_UTIL_AllocExpandString( const WORDSET *wordset, GFL_MSGDATA *MsgManager, u32 strID, HEAPID heapID );
extern void PokeReplace( const POKEMON_PASO_PARAM *ppp, POKEMON_PARAM *pp );

extern POKEMON_PASO_PARAM* PPPPointerGet( POKEMON_PARAM *pp );


//=============================================================================
/**
 *					BGWINFRM
 *
 *		GS�ɂ�����BG�X�N���[�����H�����́A�Ȃ��Ȃ����̂ŁA
 *		WB�̃V�X�e�����g����������
*/
//=============================================================================
#include "system/bgwinfrm.h"


//=============================================================================
/**
 *					PRINT	�y�� FONT
 *					WB��Print��PRINTSYS��FONT���g���悤�ɂ��ꂽ�̂ŁA
 *					�K�v�ȏ���Z�߂Ă������߂ɍ쐬�������́B
 *					�ǉ������ł��B
 *
*/
//=============================================================================
#define WT_PRINT_BUFF_MAX	(24)
#define WT_PRINT_STREAM_MAX	(1)

typedef struct 
{
	PRINT_UTIL		util;
	BOOL					use;
} WT_PRINT_QUE;

typedef struct
{	
	GFL_FONT			*font;
	GFL_TCBLSYS		*tcbsys;
	const CONFIG	*cfg;
	PRINT_QUE			*que;
	PRINT_STREAM	*stream[WT_PRINT_STREAM_MAX];
	WT_PRINT_QUE	one[WT_PRINT_BUFF_MAX];
}WT_PRINT;

extern int FontProc_GetPrintStrWidth( WT_PRINT *wk, u8 font_idx, STRBUF *buf, int magin );

extern void WT_PRINT_Init( WT_PRINT *wk, const CONFIG *cfg );
extern void WT_PRINT_Exit( WT_PRINT *wk );
extern void WT_PRINT_Main( WT_PRINT *wk );

extern BOOL GF_MSG_PrintEndCheck( WT_PRINT *setup );

extern void GF_STR_PrintSimple2( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, WT_PRINT *setup, const char *cp_file, int line );

#define GF_STR_PrintSimple(a,b,c,d,e,f) GF_STR_PrintSimple2(a,b,c,d,e,f,__FILE__,__LINE__)
extern void GF_STR_PrintColor( GFL_BMPWIN *bmpwin, u8 font_idx, STRBUF *str, int x, int y, int put_type, PRINTSYS_LSB color, WT_PRINT *setup );


extern void WT_PRINT_ClearBuffer( WT_PRINT *wk );
