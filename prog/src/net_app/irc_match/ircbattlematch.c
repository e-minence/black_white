//=============================================================================
/**
 * @file	ircbattlematch.c
 * @bfief	赤外線マッチング
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	09/02/24
 */
//=============================================================================

#include <gflib.h>
#include "arc_def.h"
#include "net/network_define.h"

#include "ircbattlematch.h"
#include "system/main.h"
#include "system/wipe.h"

#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"

#include "sound/pm_sndsys.h"

#include "msg/msg_ircbattle.h"
#include "ircbattle.naix"
#include "cg_comm.naix"
#include "net_app/connect_anm.h"
//#include "../../field/event_ircbattle.h"
#include "ir_ani_NANR_LBLDEFS.h"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "net_app/irc_match.h"

#include "ircbattlematch.cdat"


enum _IBMODE_SELECT {
  _SELECTMODE_EXIT,
};


typedef enum
{
  PLT_DS,
  PLT_RESOURCE_MAX,
  CHAR_DS = PLT_RESOURCE_MAX,
  CHAR_RESOURCE_MAX,
  ANM_DS = CHAR_RESOURCE_MAX,
  ANM_RESOURCE_MAX,
  CEL_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;

typedef enum
{
  CELL_IRDS1,
  CELL_IRDS2,
  CELL_IRDS3,
  CELL_IRDS4,
  CELL_IRWAVE1,
  CELL_IRWAVE2,
  CELL_IRWAVE3,
  CELL_IRWAVE4,
  _CELL_DISP_NUM,
} _CELL_WK_ENUM;



#if DEBUG_ONLY_FOR_ohno
#define _NET_DEBUG (1)
#else
#define _NET_DEBUG (0)
#endif
#define _WORK_HEAPSIZE (0x1000)  // 調整が必要
#define _BRIGHTNESS_SYNC (2)  // フェードのＳＹＮＣは要調整



//--------------------------------------------
// 画面構成定義
//--------------------------------------------
#define _WINDOW_MAXNUM (3)   //ウインドウのパターン数

#define _MESSAGE_BUF_NUM	( 100*2 )

#define _BUTTON_WIN_CENTERX (16)   // 真ん中
#define _BUTTON_WIN_CENTERY (13)   //
#define _BUTTON_WIN_WIDTH (30)    // ウインドウ幅
#define _BUTTON_WIN_HEIGHT (5)    // ウインドウ高さ
#define _BUTTON_WIN_PAL   (14)  // ウインドウ
#define _BUTTON_MSG_PAL   (13)  // メッセージフォント
#define _START_PAL   (12)  // スタートウインドウ

#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
#define	_BUTTON_WIN_CGX	( 2 )


#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// 通信システムウィンドウ転送先

#define _SUBLIST_NORMAL_PAL   (9)   //サブメニューの通常パレット


#define	FBMP_COL_WHITE		(15)

#define _FULL_TIMER   (120)  //人数がいっぱいで交換できない場合のメッセージ表示時間


//-------------------------------------------------------------------------
///	文字表示色定義(default)	-> gflib/fntsys.hへ移動
//------------------------------------------------------------------

#define WINCLR_COL(col)	(((col)<<4)|(col))

typedef void (StateFunc)(IRC_BATTLE_MATCH* pState);

//-----------------------------------------------
//static 定義
//-----------------------------------------------
static void _changeState(IRC_BATTLE_MATCH* pWork,StateFunc* state);
static void _changeStateDebug(IRC_BATTLE_MATCH* pWork,StateFunc* state, int line);
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void _endCallBack(void* pWork);
static void _buttonWindowCreate(int num,int* pMsgBuff,IRC_BATTLE_MATCH* pWork);
static void _ircMatchStart(IRC_BATTLE_MATCH* pWork);
static void _fadeInWait(IRC_BATTLE_MATCH* pWork);
static void _ircInitWait(IRC_BATTLE_MATCH* pWork);
static void _ircMatchWait(IRC_BATTLE_MATCH* pWork);
static void _modeSelectEntryNumInit(IRC_BATTLE_MATCH* pWork);
static void _modeSelectEntryNumWait(IRC_BATTLE_MATCH* pWork);
static void _modeReportInit(IRC_BATTLE_MATCH* pWork);
static void _modeReportWait(IRC_BATTLE_MATCH* pWork);
static BOOL _modeSelectEntryNumButtonCallback(int bttnid,IRC_BATTLE_MATCH* pWork);
static void _connectCallBack(void* pWork, int netID);
static void _RecvModeCheckData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _ircPreConnect(IRC_BATTLE_MATCH* pWork);
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work );
static BOOL _cancelButtonCallback(int bttnid,IRC_BATTLE_MATCH* pWork);
static void _RecvMyStatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _RecvPokePartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static u8* _getPokePartyBuff(int netID, void* pWork, int size);
static void _firstConnectMessage(IRC_BATTLE_MATCH* pWork);


///通信コマンド
typedef enum {
  _NETCMD_TYPESEND = GFL_NET_CMD_IRCBATTLE,
  _NETCMD_MYSTATUSSEND,
  _NETCMD_POKEPARTY_SEND,
} _BATTLEIRC_SENDCMD;


#define _TIMINGNO_CS (120)
#define _TIMINGNO_DS (121)
#define _TIMINGNO_POKEP (122)

//--------------------------------------------
// 内部ワーク
//--------------------------------------------



typedef struct{
  int gameNo;   ///< ゲーム種類
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };


//--------------------------------------------------------------
/**
 * @brief   ビーコンデータ取得関数
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------

static void* IrcBattleBeaconGetFunc(void* pWork)
{
  return &_testBeacon;
}

///< ビーコンデータサイズ取得関数
static int IrcBattleBeaconGetSizeFunc(void* pWork)
{
  return sizeof(_testBeacon);
}

///< ビーコンデータ取得関数
static BOOL IrcBattleBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}



///通信コマンドテーブル
static const NetRecvFuncTable _PacketTbl[] = {
  {_RecvModeCheckData,          NULL},  ///_NETCMD_TYPESEND
  {_RecvMyStatusData,          NULL},  ///_NETCMD_MYSTATUSSEND
  {_RecvPokePartyData,       _getPokePartyBuff},  ///_NETCMD_POKEPARTY_SEND
};

#define _MAXNUM   (2)         // 最大接続人数
#define _MAXSIZE  (100)        // 最大送信バイト数
#define _BCON_GET_NUM (16)    // 最大ビーコン収集数

static GFLNetInitializeStruct aGFLNetInit = {
  NULL,//_PacketTbl,  // 受信関数テーブル
  0,//NELEMS(_PacketTbl), // 受信テーブル要素数
  NULL,    ///< ハードで接続した時に呼ばれる
  _connectCallBack,    ///< ネゴシエーション完了時にコール
  NULL,   // ユーザー同士が交換するデータのポインタ取得関数
  NULL,   // ユーザー同士が交換するデータのサイズ取得関数
  IrcBattleBeaconGetFunc,  // ビーコンデータ取得関数
  IrcBattleBeaconGetSizeFunc,  // ビーコンデータサイズ取得関数
  IrcBattleBeaconCompFunc,  // ビーコンのサービスを比較して繋いで良いかどうか判断する
  NULL,            // 普通のエラーが起こった場合 通信終了
  FatalError_Disp,  // 通信不能なエラーが起こった場合呼ばれる 切断するしかない
  NULL, //_endCallBack,  // 通信切断時に呼ばれる関数
  NULL,  // オート接続で親になった場合
  NULL,     ///< wifi接続時に自分のデータをセーブする必要がある場合に呼ばれる関数
  NULL, ///< wifi接続時にフレンドコードの入れ替えを行う必要がある場合呼ばれる関数
  NULL,  ///< wifiフレンドリスト削除コールバック
  NULL,   ///< DWC形式の友達リスト
  NULL,  ///< DWCのユーザデータ（自分のデータ）
  0,   ///< DWCへのHEAPサイズ
  TRUE,        ///< デバック用サーバにつなぐかどうか
  0x532,//0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //元になるheapid
  HEAPID_NETWORK,  //通信用にcreateされるHEAPID
  HEAPID_WIFI,  //wifi用にcreateされるHEAPID
  HEAPID_IRC,   //※check　赤外線通信用にcreateされるHEAPID
  GFL_WICON_POSX, GFL_WICON_POSY,        // 通信アイコンXY位置
  _MAXNUM,     // 最大接続人数
  _MAXSIZE,  //最大送信バイト数
  _BCON_GET_NUM,    // 最大ビーコン収集数
  TRUE,     // CRC計算
  FALSE,     // MP通信＝親子型通信モードかどうか
  GFL_NET_TYPE_IRC,  //wifi通信を行うかどうか
  TRUE,     // 親が再度初期化した場合、つながらないようにする場合TRUE
  WB_NET_COMPATI_CHECK,  //GameServiceID
  0xfffe,	// 赤外線タイムアウト時間
  0,//MP通信親機送信バイト数
  0,//dummy
};


#define _SUBMENU_LISTMAX (2)

typedef BOOL (TouchFunc)(int no, IRC_BATTLE_MATCH* pState);


struct _IRC_BATTLE_MATCH {
  IRC_MATCH_WORK* pBattleWork;
  StateFunc* state;      ///< ハンドルのプログラム状態
  int selectType;   // 接続タイプ
  HEAPID heapID;
  GFL_BMPWIN* buttonWin[_WINDOW_MAXNUM]; /// ウインドウ管理
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// メッセージ展開用ワークマネージャー
  GFL_FONT* pFontHandle;
  STRBUF* pExStrBuf;
  STRBUF* pStrBuf;
  BMPWINFRAME_AREAMANAGER_POS aPos;
  int windowNum;
  BOOL IsIrc;
  BMPMENU_WORK* pYesNoWork;
  //    GAMESYS_WORK *gameSys_;
  //    FIELD_MAIN_WORK *fieldWork_;
  
  GFL_TCB *g3dVintr; //3D用vIntrTaskハンドル
  GFL_CLUNIT	*cellUnit;
  u32 cellRes[CEL_RESOURCE_MAX];
  GFL_CLWK* curIcon[_CELL_DISP_NUM];

  u32 connect_bit;
  BOOL connect_ok;
  BOOL receive_ok;
  u32 receive_result_param;
  u32 receive_first_param;
  GFL_ARCUTIL_TRANSINFO mainchar;
  GFL_ARCUTIL_TRANSINFO bgchar;
  GFL_ARCUTIL_TRANSINFO bgchar2;
  GFL_ARCUTIL_TRANSINFO subchar;
  CONNECT_BG_PALANM cbp;		// Wifi接続画面のBGパレットアニメ制御構造体
  BOOL bParent;
  BOOL ircmatchflg;
  BOOL ircmatchanim;
  int ircmatchanimCount;

  BOOL irccenterflg;

  BOOL ircCenterAnim;
  int ircCenterAnimCount;
  int yoffset;
  BOOL SecondStep;

  GFL_BUTTON_MAN* pButton;
  TouchFunc* touch;
  int timer;
  
  PRINT_QUE*            SysMsgQue;
  APP_TASKMENU_WORK* pAppTask;
	APP_TASKMENU_RES* pAppTaskRes;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_WIN_WORK* pAppWin;

};



enum{
  _START_TIMING=12,
};




#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG




//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @param   state  変えるステートの関数
 * @param   time   ステート保持時間
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(IRC_BATTLE_MATCH* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(IRC_BATTLE_MATCH* pWork,StateFunc state, int line)
{
  NET_PRINT("ircmatch: %d\n",line);
  _changeState(pWork, state);
}
#endif

static void _endCallBack(void* pWork)
{
  IRC_BATTLE_MATCH *commsys = pWork;

  OS_TPrintf("endCallBack終了\n");
  commsys->connect_ok = FALSE;
  commsys->connect_bit = 0;
}


//------------------------------------------------------------------------------
/**
 * @brief   フェードアウト処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(IRC_BATTLE_MATCH* pWork)
{
  if(WIPE_SYS_EndCheck()){
    _CHANGE_STATE(pWork, NULL);        // 終わり
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードアウト処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeoutStart(IRC_BATTLE_MATCH* pWork)
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  _CHANGE_STATE(pWork, _modeFadeout);        // 終わり
}

//----------------------------------------------------------------------------
/**
 *	@brief	波紋OBJ表示
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------


static void _CLACT_SetResource(IRC_BATTLE_MATCH* pWork)
{
  int i=0;

  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );

  pWork->cellRes[CHAR_DS] =
    GFL_CLGRP_CGR_Register( p_handle , NARC_ircbattle_ir_demo_ani_NCGR ,
                            FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
  pWork->cellRes[PLT_DS] =
    GFL_CLGRP_PLTT_RegisterEx(
      p_handle ,NARC_ircbattle_ir_demo_NCLR , CLSYS_DRAW_MAIN, 0, 0, 3, pWork->heapID  );
  pWork->cellRes[ANM_DS] =
    GFL_CLGRP_CELLANIM_Register(
      p_handle , NARC_ircbattle_ir_ani_NCER, NARC_ircbattle_ir_ani_NANR , pWork->heapID  );
  GFL_ARC_CloseDataHandle(p_handle);

}

static void _CLACT_SetAnim(IRC_BATTLE_MATCH* pWork,int x,int y,int no,int anm)
{
  if(pWork->curIcon[no]==NULL){
    GFL_CLWK_DATA cellInitData;

    cellInitData.pos_x = x;
    cellInitData.pos_y = y;
    cellInitData.anmseq = anm;
    cellInitData.softpri = no;
    cellInitData.bgpri = 3;
    pWork->curIcon[no] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                              pWork->cellRes[CHAR_DS],
                                              pWork->cellRes[PLT_DS],
                                              pWork->cellRes[ANM_DS],
                                              &cellInitData ,CLSYS_DRAW_MAIN , pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->curIcon[no] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->curIcon[no], TRUE );
  }
}



static void _CLACT_AddPos(IRC_BATTLE_MATCH* pWork,int x,int y,int no)
{
  GFL_CLACTPOS apos;

  if(pWork->curIcon[no]){
    GFL_CLACT_WK_GetPos(pWork->curIcon[no],&apos,CLSYS_DEFREND_MAIN);
    apos.x+=x;
    apos.y+=y;
    GFL_CLACT_WK_SetPos(pWork->curIcon[no],&apos,CLSYS_DEFREND_MAIN);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLACT開放
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

static void _CLACT_Release(IRC_BATTLE_MATCH* pWork)
{
  int i=0;

  for(i = 0 ; i < _CELL_DISP_NUM ;i++){
    if(pWork->curIcon[i]!=NULL){
      GFL_CLACT_WK_Remove(pWork->curIcon[i]);
      pWork->curIcon[i]=NULL;
    }
  }
  for(i=0;i<PLT_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_PLTT_Release(pWork->cellRes[i] );
    }
  }
  for(;i<CHAR_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CGR_Release(pWork->cellRes[i] );
    }
  }
  for(;i<ANM_RESOURCE_MAX;i++){
    if(pWork->cellRes[i] ){
      GFL_CLGRP_CELLANIM_Release(pWork->cellRes[i] );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   接続完了コールバック
 * @param   pCtl    デバッグワーク
 * @retval  none
 */
//--------------------------------------------------------------
static void _connectCallBack(void* pWk, int netID)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  u32 temp;

  OS_TPrintf("ネゴシエーション完了 netID = %d\n", netID);
  pWork->connect_bit |= 1 << netID;
  temp = pWork->connect_bit;
  if(MATH_CountPopulation(temp) >= 2){
    OS_TPrintf("全員のネゴシエーション完了 人数bit=%x\n", pWork->connect_bit);
    pWork->connect_ok = TRUE;
  }
}



//--------------------------------------------------------------
/**
 * @brief   赤外線終了ワイヤレス開始コールバック
            このコールバックは赤外線の一回のマッチング後に必ず呼ばれる。
 * @param   pCtl    デバッグワーク
 * @retval  none
 */
//--------------------------------------------------------------

static void _ircConnectEndCallback(void* pWk)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  int no;

  OS_TPrintf("_ircConnectEndCallback\n");

  if(pWork->selectType!=EVENTIRCBTL_ENTRYMODE_MULTH){
    pWork->ircmatchflg=TRUE;
    pWork->ircmatchanim=TRUE;
    pWork->SecondStep = TRUE;
  }
  else if(pWork->bParent){
    if(pWork->ircCenterAnimCount!=0){
      pWork->ircmatchflg=TRUE;
      pWork->ircmatchanim=TRUE;
    }
  }
}




//--------------------------------------------------------------
/**
 * @brief   移動コマンド受信関数
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvModeCheckData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;
  const int* pType = pData;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }

  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//自分のデータは無視
  }
  if(pWork->selectType > pType[0] ){
    NET_PRINT("BattleChange %d ->%d\n",pWork->selectType, pType[0]);
    pWork->pBattleWork->selectType = pType[0];
    //pWork->selectType = pType[0];
  }
}

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS受信関数 _NETCMD_MYSTATUSSEND
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvMyStatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  MYSTATUS* pTargetSt;
  const u16 *pRecvData = pData;
  IRC_BATTLE_MATCH* pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return; //自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  //０，１，２，３が通信用に入れても良い
  pTargetSt = GAMEDATA_GetMyStatusPlayer(pWork->pBattleWork->gamedata, netID);  //netIDで代入
  NET_PRINT("MYSTATUS GET\n");
  MyStatus_Copy(pData, pTargetSt);

}

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY受信関数 _NETCMD_POKEPARTY_SEND
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvPokePartyData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  // すでにバッファに入っている
}

static u8* _getPokePartyBuff(int netID, void* pWk, int size)
{
  IRC_BATTLE_MATCH* pWork = pWk;
  if(netID >= 0 && netID < 4){
    return (u8*)pWork->pBattleWork->pNetParty[netID];
  }
  return NULL;
}



static void _modeCheckStart5(IRC_BATTLE_MATCH* pWork)
{
 if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP, WB_NET_IRCBATTLE)){
   _CHANGE_STATE(pWork,_modeFadeoutStart);
  }
}


static void _modeCheckStart4(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS, WB_NET_IRCBATTLE)){
    if(GFL_NET_SendDataEx(GFL_NET_HANDLE_GetCurrentHandle(), GFL_NET_SENDID_ALLUSER, _NETCMD_POKEPARTY_SEND,
                        PokeParty_GetWorkSize(), pWork->pBattleWork->pParty,
                          FALSE,FALSE,TRUE)){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_POKEP,WB_NET_IRCBATTLE);
      _CHANGE_STATE(pWork,_modeCheckStart5);
    }
  }
}


static void _modeCheckStart3(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_MYSTATUSSEND,
                      MyStatus_GetWorkSize(), GAMEDATA_GetMyStatus(pWork->pBattleWork->gamedata))){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_DS,WB_NET_IRCBATTLE);
    _CHANGE_STATE(pWork,_modeCheckStart4);
  }
}

static void _modeCheckStart31(IRC_BATTLE_MATCH* pWork)
{
  switch(pWork->selectType){
  case EVENTIRCBTL_ENTRYMODE_SINGLE:
  case EVENTIRCBTL_ENTRYMODE_DOUBLE:
  case EVENTIRCBTL_ENTRYMODE_TRI:
  case EVENTIRCBTL_ENTRYMODE_ROTATE:
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(), _NETCMD_TYPESEND,
                        sizeof(pWork->selectType), &pWork->selectType)){
      _CHANGE_STATE(pWork,_modeCheckStart3);
    }
    break;
  default:
    _CHANGE_STATE(pWork,_modeCheckStart3);
    break;
  }
}

static void _modeCheckStart2(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_CS,WB_NET_IRCBATTLE)){
    _CHANGE_STATE(pWork,_modeCheckStart31);
  }
}


//--------------------------------------------------------------
/**
 * @brief   接続完了コールバック
 * @param   pCtl    デバッグワーク
 * @retval  none
 */
//--------------------------------------------------------------

static void _modeCheckStart(IRC_BATTLE_MATCH* pWork)
{
  GFL_NET_AddCommandTable(GFL_NET_CMD_IRCBATTLE, _PacketTbl, NELEMS(_PacketTbl), pWork);
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGNO_CS, WB_NET_IRCBATTLE);
  _CHANGE_STATE(pWork,_modeCheckStart2);
}

//--------------------------------------------------------------
/**
 * @brief   接続完了コールバック
 * @param   pCtl    デバッグワーク
 * @retval  none
 */
//--------------------------------------------------------------

static void _wirelessConnectCallback(void* pWk)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  _CHANGE_STATE(pWork, _modeCheckStart);
}


static void _wirelessPreConnectCallback(void* pWk,BOOL bParent)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  pWork->bParent = bParent;
  pWork->irccenterflg=TRUE;
  pWork->ircCenterAnim=TRUE;


  if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MULTH){
    if(!bParent){
      pWork->ircmatchflg=TRUE;
      pWork->ircmatchanim=TRUE;
    }
  }
}


static const GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
  GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
  GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,			// メイン2DエンジンのOBJ拡張パレット
  GX_VRAM_SUB_OBJ_128_D,			// サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
  GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
  GX_VRAM_TEXPLTT_NONE,			// テクスチャパレットスロット
  GX_OBJVRAMMODE_CHAR_1D_128K,	// メインOBJマッピングモード
  GX_OBJVRAMMODE_CHAR_1D_128K,		// サブOBJマッピングモード
};


//------------------------------------------------------------------------------
/**
 * @brief   BG領域設定
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _createBg(IRC_BATTLE_MATCH* pWork)
{
  {
    GFL_DISP_SetBank( &_defVBTbl );
  }
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }
  GFL_DISP_GX_SetVisibleControlDirect(0);		//全BG&OBJの表示OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  {
    int frame = GFL_BG_FRAME0_M;
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl( frame, &bgcntText, GFL_BG_MODE_TEXT );
    //  GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME1_M;
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl( frame, &bgcntText, GFL_BG_MODE_TEXT );
    //    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    // GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    //		GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME1_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    //		GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
    //        GFL_BG_ClearFrame(frame);
  }
  {
    int frame = GFL_BG_FRAME2_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME3_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );

    GFL_BG_FillScreen( frame,	0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }

}


//--------------------------------------------------------------
/**
 * G3D VBlank処理
 * @param TCB GFL_TCB
 * @param work tcb work
 * @retval nothing
 */
//--------------------------------------------------------------
static void	_VBlank( GFL_TCB *tcb, void *work )
{
  GFL_CLACT_SYS_VBlankFunc();	//セルアクターVBlank

}


static void _YesNoStart(IRC_BATTLE_MATCH* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 14;
	appinit.w				 = APP_TASKMENU_PLATE_WIDTH;
	appinit.h				 = APP_TASKMENU_PLATE_HEIGHT;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_27, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_28, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  pWork->appitem[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;


  pWork->pAppTask			= APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);
  G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , -8 );
}


static void _ReturnButtonStart(IRC_BATTLE_MATCH* pWork)
{
  int i;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->pMsgData, IRCBTL_STR_03, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_RETURN;
  pWork->pAppWin =APP_TASKMENU_WIN_Create( pWork->pAppTaskRes,
                                           pWork->appitem, 32-10, 24-4, 10, pWork->heapID);

  
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);

}



//------------------------------------------------------------------------------
/**
 * @brief   受け取った数のウインドウを等間隔に作る 幅は3char
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _msgWindowCreate(int* pMsgBuff,IRC_BATTLE_MATCH* pWork)
{
  int i=0;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);


  pWork->buttonWin[i] = GFL_BMPWIN_Create(
    frame,
    1, 1, _BUTTON_WIN_WIDTH,_BUTTON_WIN_HEIGHT,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
  BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );

  GFL_MSG_GetString(  pWork->pMsgData, pMsgBuff[i], pWork->pStrBuf );
  //    GFL_FONTSYS_SetColor( 1, 1, 1 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 4, 4, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

}


//------------------------------------------------------------------------------
/**
 * @brief   ともだちの人数表示
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _friendNumWindowCreate(int msgno,IRC_BATTLE_MATCH* pWork)
{
  int i=1;
  u32 cgx;
  int frame = GFL_BG_FRAME1_S;

  pWork->buttonWin[i] = GFL_BMPWIN_Create(
    frame,
    2, 8, 16, 4,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_F);
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen(pWork->buttonWin[i]);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);
  BmpWinFrame_Write( pWork->buttonWin[i], WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2), _BUTTON_WIN_PAL );

  GFL_MSG_GetString(  pWork->pMsgData, msgno, pWork->pExStrBuf );
  {
    int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
    int num2 = WIFILIST_FRIEND_MAX;
    WORDSET_RegisterNumber(pWork->pWordSet, 0, num1, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
    WORDSET_RegisterNumber(pWork->pWordSet, 1, num2, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);  
    WORDSET_ExpandStr( pWork->pWordSet, pWork->pStrBuf, pWork->pExStrBuf  );
  }
  
  //    GFL_FONTSYS_SetColor( 1, 1, 1 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->buttonWin[i]), 0, 2, pWork->pStrBuf, pWork->pFontHandle);
  GFL_BMPWIN_TransVramCharacter(pWork->buttonWin[i]);

}

//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------

static void _buttonWindowDelete(IRC_BATTLE_MATCH* pWork)
{
  int i;

  if(pWork->buttonWin[0]){
    GFL_BMPWIN_Delete(pWork->buttonWin[0]);
  }
  if(pWork->buttonWin[1]){
    GFL_BMPWIN_Delete(pWork->buttonWin[1]);
  }
  pWork->buttonWin[0]=NULL;
  pWork->buttonWin[1]=NULL;
}


static void _graphicInit(IRC_BATTLE_MATCH* pWork)
{
  GFL_BG_Init( pWork->heapID );
  GFL_BMPWIN_Init( pWork->heapID );
  GFL_FONTSYS_Init();
}


static void _graphicEnd(IRC_BATTLE_MATCH* pWork)
{
  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

}



static void _firstConnectMessage(IRC_BATTLE_MATCH* pWork)
{
  if( pWork->SecondStep == TRUE){
    int aMsgBuff[]={IRCBTL_STR_12};
    _msgWindowCreate(aMsgBuff, pWork);
  }
  else{
    if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_FRIEND)
    {
      int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
      if(num1==WIFILIST_FRIEND_MAX){
        int aMsgBuff[]={IRCBTL_STR_34};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_33};
        _msgWindowCreate(aMsgBuff, pWork);
        _friendNumWindowCreate(IRCBTL_STR_35, pWork);
      }
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_TRADE)
    {
      int aMsgBuff[]={IRCBTL_STR_17};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_MULTH)
    {
      int aMsgBuff[]={IRCBTL_STR_10};
      _msgWindowCreate(aMsgBuff, pWork);
    }
    else
    {
      int aMsgBuff[]={IRCBTL_STR_09};
      _msgWindowCreate(aMsgBuff, pWork);
    }
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト全体の初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeInit(IRC_BATTLE_MATCH* pWork)
{
  GFL_FONTSYS_SetDefaultColor();
  pWork->pWordSet = WORDSET_CreateEx( 11, 200, pWork->heapID );
  _graphicInit(pWork);
  _createBg(pWork);

  pWork->pStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pExStrBuf = GFL_STR_CreateBuffer( _MESSAGE_BUF_NUM, pWork->heapID );
  pWork->pFontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );
  pWork->pMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_ircbattle_dat, pWork->heapID );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_IRCBATTLE, pWork->heapID );



    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircbattle_ir_demo_NCLR,
                                      PALTYPE_MAIN_BG, 0, 0,  pWork->heapID);
    pWork->mainchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_ircbattle_ir_demo_NCGR,
                                                                   GFL_BG_FRAME0_M, 0, 0, pWork->heapID);

    if(EVENTIRCBTL_ENTRYMODE_MULTH==pWork->selectType){ //1vs1
      GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo1_NSCR,
                                                GFL_BG_FRAME1_M, 0,
                                                GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                                pWork->heapID);
    }

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_ircbattle_ir_demo2_NSCR,
                                              GFL_BG_FRAME0_M, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainchar), 0, 0,
                                              pWork->heapID);
    GFL_ARC_CloseDataHandle( p_handle );
  }

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_CG_COMM, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_cg_comm_comm_bg_NCLR,
                                      PALTYPE_SUB_BG, 0, 0,  pWork->heapID);
    // サブ画面BG0キャラ転送
    pWork->subchar = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_cg_comm_comm_bg_NCGR,
                                                                  GFL_BG_FRAME0_S, 0, 0, pWork->heapID);

    // サブ画面BG0スクリーン転送
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs(   p_handle, NARC_cg_comm_comm_base_NSCR,
                                              GFL_BG_FRAME0_S, 0,
                                              GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subchar), 0, 0,
                                              pWork->heapID);
    GFL_ARC_CloseDataHandle(p_handle);
  }


  pWork->bgchar2 = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME1_S, _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, _BRIGHTNESS_SYNC);

  {
    _firstConnectMessage(pWork);
  }

  GFL_CLACT_SYS_Create(	&GFL_CLSYSINIT_DEF_DIVSCREEN, &_defVBTbl, pWork->heapID );
  pWork->cellUnit = GFL_CLACT_UNIT_Create( 40 , 0 , pWork->heapID );
  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

  pWork->SysMsgQue = PRINTSYS_QUE_Create( pWork->heapID );
  pWork->pAppTaskRes =
    APP_TASKMENU_RES_Create( GFL_BG_FRAME2_S, _SUBLIST_NORMAL_PAL,
                             pWork->pFontHandle, pWork->SysMsgQue, pWork->heapID  );
  


  _CLACT_SetResource(pWork);

  //  if(EVENTIRCBTL_ENTRYMODE_MULTH)

  if(EVENTIRCBTL_ENTRYMODE_MULTH!=pWork->selectType){ //1vs1
    int keisu=4;

    _CLACT_SetAnim(pWork,88,72,CELL_IRDS2,NANR_ir_ani_CellAnime3);  //左
    _CLACT_SetAnim(pWork,88+keisu,72+keisu,CELL_IRWAVE1,NANR_ir_ani_CellAnime0);


    _CLACT_SetAnim(pWork,168,      110,CELL_IRDS1,NANR_ir_ani_CellAnime2); //右
    _CLACT_SetAnim(pWork,168-keisu,110-keisu,CELL_IRWAVE2,NANR_ir_ani_CellAnime1);

  }
  else{ //2vs2
    int keisu=4;
    int kei2=16;

    _CLACT_SetAnim(pWork, kei2+ 88,        -kei2+72,CELL_IRDS2,NANR_ir_ani_CellAnime3);  //左
    _CLACT_SetAnim(pWork, kei2+ 88+keisu,  -kei2+72+keisu,CELL_IRWAVE2,NANR_ir_ani_CellAnime0);
    // _CLACT_SetAnim(pWork, -kei2+ 88,        kei2+72,CELL_IRDS4,NANR_ir_ani_CellAnime3);  //左
    _CLACT_SetAnim(pWork, -kei2+ 88+keisu,  kei2+72+keisu,CELL_IRWAVE3,NANR_ir_ani_CellAnime0);

    //   _CLACT_SetAnim(pWork, kei2+ 168,       -kei2+   110,CELL_IRDS3,NANR_ir_ani_CellAnime2); //右
    _CLACT_SetAnim(pWork, kei2+ 168-keisu, -kei2+   110-keisu,CELL_IRWAVE4,NANR_ir_ani_CellAnime1);
    _CLACT_SetAnim(pWork,-kei2+168,         kei2+   110,CELL_IRDS1,NANR_ir_ani_CellAnime2); //右
    _CLACT_SetAnim(pWork,-kei2+168-keisu,   kei2+   110-keisu,CELL_IRWAVE1,NANR_ir_ani_CellAnime1);
  }


  GFL_DISP_GX_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_OBJ );
  GFL_DISP_GXS_SetVisibleControlDirect( GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2 );


  _CHANGE_STATE(pWork,_fadeInWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   フェードイン待ち
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _fadeInWait(IRC_BATTLE_MATCH* pWork)
{
  // ワイプ終了待ち
  if( GFL_FADE_CheckFade() ){
    _CHANGE_STATE(pWork,_ircMatchStart);
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircMatchStart(IRC_BATTLE_MATCH* pWork)
{

  {
    GFLNetInitializeStruct net_ini_data;
    net_ini_data = aGFLNetInit;
    net_ini_data.bNetType = GFL_NET_TYPE_IRC_WIRELESS;
    switch(pWork->selectType){
    case EVENTIRCBTL_ENTRYMODE_SINGLE:
    case EVENTIRCBTL_ENTRYMODE_DOUBLE:
    case EVENTIRCBTL_ENTRYMODE_TRI:
    case EVENTIRCBTL_ENTRYMODE_ROTATE:
      net_ini_data.gsid = WB_NET_IRCBATTLE;
      break;
    case EVENTIRCBTL_ENTRYMODE_MULTH:
      net_ini_data.gsid = WB_NET_IRCBATTLE_MULTI;
      net_ini_data.maxConnectNum = 4;
      break;
    case EVENTIRCBTL_ENTRYMODE_FRIEND:
      net_ini_data.gsid = WB_NET_IRCFRIEND;
      break;
    case EVENTIRCBTL_ENTRYMODE_TRADE:
      net_ini_data.gsid = WB_NET_IRCTRADE;
      break;
    case EVENTIRCBTL_ENTRYMODE_SUBWAY:
      net_ini_data.gsid = WB_NET_BSUBWAY;
      break;
    default:
//      GF_ASSERT(0);
      break;
    }
    if(pWork->selectType!=EVENTIRCBTL_ENTRYMODE_MUSICAL){
      GFL_NET_Init(&net_ini_data, NULL, pWork);	//通信初期化
    }
  }
  _ReturnButtonStart(pWork);
  
  _CHANGE_STATE(pWork,_ircInitWait);
}

static void _workEnd(IRC_BATTLE_MATCH* pWork)
{
  GFL_FONTSYS_SetDefaultColor();

  _buttonWindowDelete(pWork);
  GFL_BG_FillCharacterRelease( GFL_BG_FRAME1_S, 1, 0);
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_S,GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar2),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar2));
  GFL_BG_FreeBGControl(GFL_BG_FRAME1_S);
  GFL_MSG_Delete( pWork->pMsgData );
  GFL_FONT_Delete(pWork->pFontHandle);
  GFL_STR_DeleteBuffer(pWork->pExStrBuf);
  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
  APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );

  _graphicEnd(pWork);

}

//------------------------------------------------------------------------------
/**
 * @brief   ＩＲＣ接続待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircInitWait(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_NET_IsInit() == TRUE){	//初期化終了待ち
    GFL_NET_ChangeoverConnect_IRCWIRELESS(_wirelessConnectCallback,_wirelessPreConnectCallback,_ircConnectEndCallback); // 専用の自動接続


    pWork->pButton = GFL_BMN_Create( btn_irmain, _BttnCallBack, pWork,  pWork->heapID );
    pWork->touch = &_cancelButtonCallback;


    _CHANGE_STATE(pWork,_ircMatchWait);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ＩＲＣ接続待機
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircExitWait(IRC_BATTLE_MATCH* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    if(selectno == 0)
    { // はいを選択した場合
      pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
//      EVENT_IrcBattleSetType(pWork->pBattleWork,EVENTIRCBTL_ENTRYMODE_EXIT);
      _buttonWindowDelete(pWork);
      GFL_NET_Exit(NULL);
      _CHANGE_STATE(pWork,_modeFadeoutStart);
    }
    else
    {  // いいえを選択した場合
      _buttonWindowDelete(pWork);

      _firstConnectMessage(pWork);

      _ReturnButtonStart(pWork);
      pWork->pButton = GFL_BMN_Create( btn_irmain, _BttnCallBack, pWork,  pWork->heapID );
      pWork->touch = &_cancelButtonCallback;

      _CHANGE_STATE(pWork,_ircMatchWait);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   キーを待って終了
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _ircEndKeyWait(IRC_BATTLE_MATCH* pWork)
{
  if(GFL_UI_KEY_GetTrg() != 0){


    pWork->pBattleWork->selectType = EVENTIRCBTL_ENTRYMODE_RETRY;
//    EVENT_IrcBattleSetType(pWork->pBattleWork,EVENTIRCBTL_ENTRYMODE_RETRY);
    GFL_NET_Exit(NULL);
    _CHANGE_STATE(pWork,_modeFadeoutStart);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   人数選択画面初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
#define	FLD_YESNO_WIN_PX	( 25 )
#define	FLD_YESNO_WIN_PY	( 13 )

static const BMPWIN_YESNO_DAT _yesNoBmpDatSys2 = {
  GFL_BG_FRAME2_S, FLD_YESNO_WIN_PX, FLD_YESNO_WIN_PY+6,
  12, 512
  };



//----------------------------------------------------------------------------
/**
 *	@brief	ボタンイベントコールバック
 *
 *	@param	bttnid		ボタンID
 *	@param	event		イベント種類
 *	@param	p_work		ワーク
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  IRC_BATTLE_MATCH *pWork = p_work;
  u32 friendNo;

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< 触れた瞬間
    if(pWork->touch!=NULL){
      if(pWork->touch(bttnid, pWork)){
        return;
      }
    }
    break;

  default:
    break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   モードセレクト画面タッチ処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeAppWinFlash(IRC_BATTLE_MATCH* pWork)
{
  if(APP_TASKMENU_WIN_IsFinish(pWork->pAppWin)){
    int aMsgBuff[]={IRCBTL_STR_16};
    _buttonWindowDelete(pWork);

    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;

    GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
                                  0x20*12, 0x20, pWork->heapID);
    _msgWindowCreate(aMsgBuff, pWork);

    _YesNoStart(pWork);

    GFL_FONTSYS_SetDefaultColor();

    if(pWork->pButton){
      GFL_BMN_Delete(pWork->pButton);
    }
    pWork->pButton = NULL;

    _CHANGE_STATE(pWork,_ircExitWait);
  }
}


static void _waitFinish(IRC_BATTLE_MATCH* pWork)
{
  pWork->timer--;
  if(pWork->timer!=0){
    return;
  }
  _buttonWindowDelete(pWork);
  GFL_NET_Exit(NULL);
  _CHANGE_STATE(pWork,_modeFadeoutStart);

}

//------------------------------------------------------------------------------
/**
 * @brief   キャンセルボタンタッチ処理
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _cancelButtonCallback(int bttnid, IRC_BATTLE_MATCH* pWork)
{
  BOOL ret = FALSE;

//  pWork->bttnid = bttnid;
  switch( bttnid ){
  case _SELECTMODE_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
    APP_TASKMENU_WIN_SetDecide(pWork->pAppWin, TRUE);
//  pWork->selectType = EVENTIRCBTL_ENTRYMODE_EXIT;
    _CHANGE_STATE(pWork,_modeAppWinFlash);        // 終わり
    break;
  default:
    break;
  }
  return ret;
}





static void _ircMatchWait(IRC_BATTLE_MATCH* pWork)
{
  int num1 = WifiList_GetFriendDataNum( GAMEDATA_GetWiFiList(pWork->pBattleWork->gamedata) ); //WIFILIST_FRIEND_MAX
  if(num1==WIFILIST_FRIEND_MAX){
    _CHANGE_STATE(pWork,_waitFinish);        // 終わり()
    pWork->timer = _FULL_TIMER;
    return;
  }

  
  if(pWork->ircCenterAnim){  //4台の時2台を真ん中に

    _buttonWindowDelete(pWork);
    if(pWork->irccenterflg == TRUE){

      if(pWork->bParent){  //リーダー
        int aMsgBuff[]={IRCBTL_STR_12};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_13};
        _msgWindowCreate(aMsgBuff, pWork);
      }

      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE3]);
      pWork->curIcon[CELL_IRWAVE3]=NULL;
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE4]);
      pWork->curIcon[CELL_IRWAVE4]=NULL;
      pWork->irccenterflg =FALSE;
    }


    pWork->ircCenterAnimCount++;

    _CLACT_AddPos(pWork, -2,+2,CELL_IRDS2);
    _CLACT_AddPos(pWork, 2,-2,CELL_IRDS1);
    _CLACT_AddPos(pWork, -2,+2,CELL_IRWAVE2);
    _CLACT_AddPos(pWork, 2,-2,CELL_IRWAVE1);

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BD , 16-(pWork->ircCenterAnimCount*2), 16);

    if(pWork->ircCenterAnimCount>8){
      pWork->ircCenterAnim=FALSE;
      GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
    }



  }
  else if(pWork->ircmatchanim==TRUE){  //2台の時お互いを引く
    if(pWork->ircmatchflg==TRUE){
      _buttonWindowDelete(pWork);
      if(pWork->selectType==EVENTIRCBTL_ENTRYMODE_FRIEND || pWork->selectType==EVENTIRCBTL_ENTRYMODE_TRADE)
      {
        int aMsgBuff[]={IRCBTL_STR_30};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      else{
        int aMsgBuff[]={IRCBTL_STR_31};
        _msgWindowCreate(aMsgBuff, pWork);
      }
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE1]);
      pWork->curIcon[CELL_IRWAVE1]=NULL;
      GFL_CLACT_WK_Remove(pWork->curIcon[CELL_IRWAVE2]);
      pWork->curIcon[CELL_IRWAVE2]=NULL;
      pWork->ircmatchflg=FALSE;
    }
    pWork->ircmatchanimCount++;
    //if(EVENTIRCBTL_ENTRYMODE_MULTH!=pWork->selectType){ //1vs1
    _CLACT_AddPos(pWork, 2, 1,CELL_IRDS1);
    _CLACT_AddPos(pWork,-2,-1,CELL_IRDS2);
    //    }
    if(pWork->ircmatchanimCount > 25){;
      pWork->ircmatchanim=FALSE;
    }
  }


  if(GFL_NET_IsInit()){
    if(GFL_NET_NEG_TYPE_TYPE_ERROR==GFL_NET_HANDLE_GetNegotiationType(GFL_NET_HANDLE_GetCurrentHandle())){  ///< モードが異なる接続エラー
      int aMsgBuff[]={IRCBTL_STR_25};
      _buttonWindowDelete(pWork);
      _msgWindowCreate(aMsgBuff, pWork);
      _CHANGE_STATE(pWork,_ircEndKeyWait);
      return;
    }
  }


  GFL_BMN_Main( pWork->pButton );

}

//------------------------------------------------------------------------------
/**
 * @brief
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _ircPreConnect(IRC_BATTLE_MATCH* pWork)
{
}

//--------------------------------------------------------------
/**
 * @brief   移動コマンド受信関数
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//自分のデータは無視
  }

  //GFL_STD_MemCopy(pData, pWork->receive_first_param, size);
  pWork->receive_ok = TRUE;
  OS_TPrintf("最初のデータ受信コールバック netID = %d\n", netID);
}

//--------------------------------------------------------------
/**
 * @brief   キー情報コマンド受信関数
 * @param   netID      送ってきたID
 * @param   size       パケットサイズ
 * @param   pData      データ
 * @param   pWork      ワークエリア
 * @param   pHandle    受け取る側の通信ハンドル
 * @retval  none
 */
//--------------------------------------------------------------
static void _RecvResultData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  IRC_BATTLE_MATCH *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//自分のデータは無視
  }

  //GFL_STD_MemCopy(pData, pWork->receive_result_param, size);
  pWork->receive_ok = TRUE;
  OS_TPrintf("最後のデータ受信コールバック netID = %d\n", netID);
}




//------------------------------------------------------------------------------
/**
 * @brief   PROCスタート
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x20000 );

  {
    IRC_BATTLE_MATCH *pWork = GFL_PROC_AllocWork( proc, sizeof( IRC_BATTLE_MATCH ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(IRC_BATTLE_MATCH));
    pWork->pBattleWork = pwk;
    pWork->heapID = HEAPID_IRCBATTLE;
    pWork->selectType =  pWork->pBattleWork->selectType; //EVENT_IrcBattleGetType((EVENT_IRCBATTLE_WORK*) pwk);

    _CHANGE_STATE( pWork, _modeInit);
  }
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MATCH* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Update( pWork->pAppWin );
  }
  GFL_BG_SetScroll( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, pWork->yoffset );
  pWork->yoffset--;
  ConnectBGPalAnm_Main(&pWork->cbp);
  GFL_CLACT_SYS_Main();
  PRINTSYS_QUE_Main(pWork->SysMsgQue);

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT IrcBattleMatchProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  IRC_BATTLE_MATCH* pWork = mywk;


  if(pWork->pAppTask){
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
  }
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  }
	if(pWork->pButton){
		GFL_BMN_Delete(pWork->pButton);
	}
  pWork->pButton = NULL;
  WORDSET_Delete(pWork->pWordSet);

  _workEnd(pWork);
  GFL_TCB_DeleteTask( pWork->g3dVintr );
  GFL_CLACT_UNIT_Delete(pWork->cellUnit);
  GFL_CLACT_SYS_Delete();
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);

  ConnectBGPalAnm_End(&pWork->cbp);
  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);


  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA IrcBattleMatchProcData = {
  IrcBattleMatchProcInit,
  IrcBattleMatchProcMain,
  IrcBattleMatchProcEnd,
};


