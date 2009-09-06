//=============================================================================
/**
 * @file	  ircpokemontrade_local.h
 * @bfief	  ポケモン交換 ローカル共有定義
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#include "../../field/event_ircbattle.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "system/touch_subwindow.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //デバッグアイテム生成用

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "app/app_taskmenu.h"


#define _TIMING_ENDNO (12)
#define BOX_MONS_NUM (30)
#define _BRIGHTNESS_SYNC (2)  // フェードのＳＹＮＣは要調整
#define CUR_NUM (5)
#define _BUTTON_WIN_PAL   (15)  // ウインドウ
#define _BUTTON_MSG_PAL   (14)  // メッセージフォント
#define	FBMP_COL_WHITE		(15)

#define _OBJPLT_BOX  (0)  //3本
#define _OBJPLT_POKEICON  (6)  //3本
#define PLIST_RENDER_MAIN (1)
#define PSTATUS_MCSS_POS_X1 (FX32_CONST(( 60 )/16.0f))
#define PSTATUS_MCSS_POS_X2 (FX32_CONST(( 190 )/16.0f))
#define PSTATUS_MCSS_POS_Y (FX32_CONST((192.0f-( 140 ))/16.0f))

#define YESNO_CHARA_OFFSET	(21 * 20 )
#define YESNO_CHARA_W		( 8 )
#define YESNO_CHARA_H		( 4 )

#define _BUTTON_WIN_WIDTH (22)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (5)    // ウインドウ高さ


#define CONTROL_PANEL_Y (120+48)


#define CHANGEBUTTON_X (72)
#define CHANGEBUTTON_Y (CONTROL_PANEL_Y)
#define CHANGEBUTTON_WIDTH  (5*8)
#define CHANGEBUTTON_HEIGHT (3*8)
#define WINCLR_COL(col)	(((col)<<4)|(col))




#define YESBUTTON_X (200)
#define YESBUTTON_Y (160)
#define NOBUTTON_X (200)
#define NOBUTTON_Y (180)

#define _SUBMENU_LISTMAX (8)


typedef void (StateFunc)(IRC_POKEMON_TRADE* pState);


typedef enum
{
	CHAR_BOX,
	PLT_POKEICON,
	PLT_BOX,
	ANM_POKEICON,
	ANM_BOX,
	CEL_RESOURCE_MAX,
} CEL_RESOURCE;


struct _IRC_POKEMON_TRADE {
	POKEMON_PASO_PARAM* recvPoke[2];
	BOOL bPokemonSet[2];
	EVENT_IRCBATTLE_WORK* pParentWork;
	StateFunc* state;      ///< ハンドルのプログラム状態
	int selectType;   // 接続タイプ
	HEAPID heapID;

	//メッセージ系
	GFL_BMPWIN* MessageWin;

  GFL_ARCUTIL_TRANSINFO bgchar;

	GFL_BMPWIN* StatusWin[2*4];

	TOUCH_SW_SYS			*TouchSubWindowSys;

  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
	GFL_BMPWIN* MyInfoWin;
	GFL_BMPWIN* mesWin;
	GFL_MSGDATA *pMsgData;  //
	WORDSET *pWordSet;								// メッセージ展開用ワークマネージャー
	GFL_FONT* pFontHandle;
  PRINT_STREAM* pStream;
	STRBUF* pStrBuf;
	STRBUF* pExStrBuf;
  STRBUF* pMessageStrBuf;
  STRBUF* pMessageStrBufEx;
  PRINT_QUE*            SysMsgQue;
	GFL_TCBLSYS *pMsgTcblSys;
	//    BMPWINFRAME_AREAMANAGER_POS aPos;
	//3D
	BOX_DATA* pBox;
	GFL_G3D_CAMERA    *camera;

	MCSS_SYS_WORK *mcssSys;
	MCSS_WORK     *pokeMcss[2];

	GFL_ARCUTIL_TRANSINFO subchar;
	GFL_ARCUTIL_TRANSINFO subchar1;
	u32 cellRes[CEL_RESOURCE_MAX];

	GAMESYS_WORK* pGameSys;

	GFL_CLUNIT	*cellUnit;
	GFL_TCB *g3dVintr; //3D用vIntrTaskハンドル

	u32 pokeIconNcgRes[BOX_MONS_NUM];
	GFL_CLWK* pokeIcon[BOX_MONS_NUM];

	GFL_CLWK* curIcon[CUR_NUM];

	int windowNum;
	BOOL IsIrc;
	u32 connect_bit;
	BOOL connect_ok;
	BOOL receive_ok;
	u32 receive_result_param;
	u32 receive_first_param;

	int nowBoxno;  //いまのボックス
	u32 x;
	u32 y;
	BOOL bUpVec;
	int catchIndex;   //つかんでるポケモン
	int selectIndex;  //候補のポケモンIndex
	int selectBoxno;  //候補のポケモンBox

  int underSelectIndex;  //まだ相手に見せてないポケモンIndex
  int underSelectBoxno;  //まだ相手に見せてないポケモンBox
  int pokemonsetCall;

  
	BOOL bChangeOK[2];

	BOOL bParent;
	BOOL bTouchReset;
};




extern void IRC_POKETRADE_GraphicInit(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_SubStatusInit(IRC_POKEMON_TRADE* pWork,int pokeposx);
extern void IRC_POKETRADE_SubStatusEnd(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_AppMenuOpen(IRC_POKEMON_TRADE* pWork, int *menustr,int num);
extern void IRC_POKETRADE_MessageOpen(IRC_POKEMON_TRADE* pWork, int msgno);
extern void IRC_POKETRADE_MessageWindowOpen(IRC_POKEMON_TRADE* pWork, int msgno);
extern void IRC_POKETRADE_MessageWindowClose(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_MessageWindowClear(IRC_POKEMON_TRADE* pWork);
extern BOOL IRC_POKETRADE_MessageEndCheck(IRC_POKEMON_TRADE* pWork);

