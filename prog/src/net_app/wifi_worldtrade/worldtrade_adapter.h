//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		worldtrade_adapter.h
 *	@brief	GTSを移植する際のパック関数群
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
#include "pm_define.h"

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
 *					デバッグマクロ
*/
//=============================================================================
#ifdef PM_DEBUG

#define CHANGE_POKE_RULE_IGNORE	//交換用ポケモンが結果と一致していなくてもえらべる
//#define RESERVE_POKE_GS_BINARY	//サーバーに預けるポケモンはGSから作成したバイナリを使う
//#define DEBUG_SAVE_NONE					//デバッグ用にセーブしないで進みます


#ifdef DEBUG_ONLY_FOR_toru_nagihashi	//担当者が変わったら変えてください
#define DEBUG_AUTHER_ONLY				//現在ONになっているとadapter以外で行っていないところをワーニングで知らせます
#endif //PM_DEBUG

//担当者プリント
#ifdef DEBUG_AUTHER_ONLY
#define MORI_PRINT(...)				OS_Printf( __VA_ARGS__ )
#else
#define MORI_PRINT(...)				((void)0)
#endif //DEBUG_AUTHER_ONLY

//アルセウスイベントチェック
//#define ARUCEUSU_EVENT_CHECK

//PHC解放チェック
//#define PHC_EVENT_CHECK


#endif //PM_DEBUG
//=============================================================================
/**
 *					定義
*/
//=============================================================================
#define _MAXNUM   (2)         // 最大接続人数
#define _MAXSIZE  (80)        // 最大送信バイト数
#define _BCON_GET_NUM (16)    // 最大ビーコン収集数
#define COMM_DMA_NO						(_NETWORK_DMA_NO)
#define COMM_POWERMODE				(_NETWORK_POWERMODE)
#define COMM_SSL_PRIORITY			(_NETWORK_SSL_PRIORITY)
#define COMM_ERROR_RESET_GTS  (3)	//gflib/src/network/net_state.hに定義がある
#define COMM_ERROR_RESET_OTHER (4)
#define COMM_ERRORTYPE_POWEROFF (1)   // 電源を切らないといけない
#define PARA_UNK							(2)
#define PARA_MALE							(0)
#define PARA_FEMALE						(1)
//=============================================================================
/**
 *					置き換える予定だが、まだ定義されていないマクロ
*/
//=============================================================================


#define FONT_TOUCH						(0)
#define FONT_SYSTEM						(0)
#define FONT_TALK							(0)

//メッセージ
#define MSG_TP_ON							(0)
#define MSG_TP_OFF						(0)

//PHC
#define PHC_WIFI_OPEN_COURSE_NO	(0)
typedef void PHC_SVDATA;


//=============================================================================
/**
 *					置き換える予定だが、まだ定義されていない関数
*/
//=============================================================================
typedef struct
{	
	int dummy;
}ZUKAN_WORK;

//=============================================================================
/**
 *					使用していないが、置換が大変なので定義でだましているもの
*/
//=============================================================================

#define MSG_NO_PUT						(0)
#define MSG_ALLPUT						(0)

//=============================================================================
/**
 *					置き換える予定だが、まだなく、中身を定義していない関数
*/
//=============================================================================
//タイムウェイトアイコン
static inline void * TimeWaitIconAdd( GFL_BMPWIN *bmpwin, int a ) {	return NULL; }
static inline void TimeWaitIconDel( void *wk ){}

//サウンド
static inline void Snd_DataSetByScene( int a, int b, int c ){}

//UI
static inline void MsgPrintTouchPanelFlagSet( int a ){}

//各種セーブデータ　PHC
static inline PHC_SVDATA * SaveData_GetPhcSaveData( SAVE_CONTROL_WORK *sv ){	return NULL; }
static inline void PhcSvData_SetCourseOpenFlag( PHC_SVDATA *sv, int a ){}
static inline EVENTWORK* SaveData_GetEventWork( SAVE_CONTROL_WORK *sv ){ return NULL; }

//アルセウスイベント
static inline int SysWork_AruseusuEventGet( EVENTWORK *ev ){	return 0;}
static inline void SysWork_AruseusuEventSet( EVENTWORK *ev, int a ){}

//図鑑
static inline BOOL ZukanWork_GetZenkokuZukanFlag( ZUKAN_WORK *wk ){return TRUE;}
static inline BOOL ZukanWork_GetPokeSeeFlag( ZUKAN_WORK *zukan, int a ){return TRUE;}
static inline void SaveData_GetPokeRegister( SAVE_CONTROL_WORK *sv, POKEMON_PARAM *pp ){}

//=============================================================================
/**
 *					以下、単純に置き換えた関数
*/
//=============================================================================
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
//通信アイコン
static inline void WirelessIconEasy( void )
{
	//GFL_NET_ReloadIcon();  // 接続中なのでアイコン表示
	GFL_NET_WirelessIconEasyXY(GFL_WICON_POSX, GFL_WICON_POSY, TRUE, HEAPID_WORLDTRADE);
	GFL_NET_WirelessIconEasy_HoldLCD( FALSE, HEAPID_WORLDTRADE );
}
static inline void WirelessIconEasy_SetLevel( int level )
{
	GFL_NET_WirelessIconEasy_SetLevel(level);
}
static inline void WirelessIconEasyEnd( void )
{
	GFL_NET_WirelessIconEasyEnd();
}


//通信エラー
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
	// 相手のカートリッジが日本なのに
	if(langCode==LANG_JAPAN){
		// 違う国の地球儀コードを入れてきたら登録しない
		if(Nation!=WIFI_NATION_JAPAN){
			return;
		}
	}
#endif
	
	// 国・地域を登録
//	OS_Printf("no=%d\n", WIFIHISTORY_GetStat(wifiHistory, Nation, Area ));
	if(WIFIHISTORY_GetStat( wifiHistory, Nation, Area )==WIFIHIST_STAT_NODATA){
		WIFIHISTORY_SetStat( wifiHistory, Nation, Area, WIFIHIST_STAT_NEW );
	}
}
//=============================================================================
/**
 *					以下、WBにはないが実装できるので、作成した関数
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
 *		GSにあったBGスクリーン加工処理は、なくなったので、
 *		WBのシステムを使い実装する->移植したので置き換え
*/
//=============================================================================
#include "system/bgwinfrm.h"


//=============================================================================
/**
 *					PRINT	及び FONT
 *					WBのPrintはPRINTSYSとFONTを使うようにされたので、
 *					必要な情報を纏めておくために作成したもの。
 *					追加部分です。
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

//=============================================================================
/**
 *      NUMFONT
 *        WBにNUMFONTはないので、smallfontを使い実装する
*/
//=============================================================================

//NUMFONT
#define NUMFONT_MODE_LEFT			(STR_NUM_DISP_LEFT)
#define NUMFONT_MARK_SLASH		(0)

typedef struct _NUMFONT NUMFONT;

NUMFONT * NUMFONT_Create( int a, int b, int c, HEAPID heapID );
void NUMFONT_Delete( NUMFONT *wk );
void NUMFONT_Main( NUMFONT *wk );
void NUMFONT_WriteNumber( NUMFONT *wk, int num, int keta, int mode, GFL_BMPWIN *bmpwin, int x, int y );
void NUMFONT_WriteMark( NUMFONT *wk, int mark, GFL_BMPWIN *bmpwin, int x, int y );

