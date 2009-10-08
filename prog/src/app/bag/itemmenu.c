//============================================================================================
/**
 * @file	  itemmenu.h
 * @brief	  アイテムメニュー
 * @author	k.ohno
 * @date	  2009.06.30
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "system/bmp_winframe.h"

#include "message.naix"
#include "bag.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_bag.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"
#include "system/wipe.h"

#include "itemmenu.h"
#include "itemmenu_local.h"
#include "app/itemuse.h"
#include "savedata/mystatus.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "system/main.h"			//GFL_HEAPID_APP参照

//============================================================================================
//============================================================================================

#define _1CHAR (8)

// SE再定義
enum
{ 
  SE_BAG_SELL         = SEQ_SE_SYS_22,  ///< 売却音
  SE_BAG_TRASH        = SEQ_SE_SYS_08,  ///< 捨てる音
  SE_BAG_CURSOR_MOVE  = SEQ_SE_SELECT1, ///< バッグカーソル移動(上下キー)
  SE_BAG_POCKET_MOVE  = SEQ_SE_SELECT4, ///< バッグポケット選択(左右キー)
};

//=============================================================================
// 目次
//=============================================================================
//  __TECNIQUE  技マシン シーケンス
//  __TRASH     捨てる シーケンス
//  __SELL      売る シーケンス


//=============================================================================
// データテーブル
//=============================================================================
//ならびは savedata/myitem_savedata.hに準拠
// コールバックは_BttnCallBack
static const GFL_UI_TP_HITTBL bttndata[] = {  //上下左右

#include "bag_btnpos.h"  //バッグの絵のタッチエリア

  {	21*_1CHAR,  24*_1CHAR,   1*_1CHAR,  3*_1CHAR },  //左
  {	21*_1CHAR,  24*_1CHAR,  15*_1CHAR, 18*_1CHAR },  //右

  {	21*_1CHAR,  24*_1CHAR,  26*_1CHAR, 28*_1CHAR },  //x
  {	21*_1CHAR,  24*_1CHAR,  30*_1CHAR, 32*_1CHAR },  //リターン

  {	1*_1CHAR+4,   4*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア1
  {	4*_1CHAR+4,   7*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア2
  {	7*_1CHAR+4,  11*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア3
  {11*_1CHAR+4,  14*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア4
  {14*_1CHAR+4,  17*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア5
  {17*_1CHAR+4,  20*_1CHAR+3,  18*_1CHAR, 28*_1CHAR },  //アイテム一覧エリア6

  {	1*_1CHAR+4,   4*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア1
  {	4*_1CHAR+4,   7*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア2
  {	7*_1CHAR+4,  11*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア3
  {11*_1CHAR+4,  14*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア4
  {14*_1CHAR+4,  17*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア5
  {17*_1CHAR+4,  20*_1CHAR+3,  15*_1CHAR, 18*_1CHAR },  //だいじなものチェックエリア6

  {GFL_UI_TP_HIT_END,0,0,0},		 //終了データ
};



//-----------------------------------------------
//static 定義
//-----------------------------------------------
static void _changeState(FIELD_ITEMMENU_WORK* pWork,StateFunc state);
static void _changeStateDebug(FIELD_ITEMMENU_WORK* pWork,StateFunc state, int line);
static void _windowCreate(FIELD_ITEMMENU_WORK* pWork);
static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork);
ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork, int no);
int ITEMMENU_GetItemPocketNumber(FIELD_ITEMMENU_WORK* pWork);
static void _ItemChangeSingle(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 );
static void _ItemChange(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 );
static void _pocketCursorChange(FIELD_ITEMMENU_WORK* pWork,int oldpocket, int newpocket);
int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork);
static BOOL _posplus(FIELD_ITEMMENU_WORK* pWork, int length);
static BOOL _posminus(FIELD_ITEMMENU_WORK* pWork, int length);
static BOOL _itemScrollCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _keyChangeItemCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _keyMoveCheck(FIELD_ITEMMENU_WORK* pWork);
static BOOL _itemMovePositionTouchItem(FIELD_ITEMMENU_WORK* pWork);
static void _itemMovePosition(FIELD_ITEMMENU_WORK* pWork);
static void _itemSelectWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemSelectState(FIELD_ITEMMENU_WORK* pWork);
static void _itemKindSelectMenu(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseYesNo(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTecniqueUseInit(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashEndWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesNoWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashYesNo(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrashWait(FIELD_ITEMMENU_WORK* pWork);
static void _itemTrash(FIELD_ITEMMENU_WORK* pWork);
static void _itemSellInit( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellInputWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellYesnoInit( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellYesnoInput( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellEndMsgWait( FIELD_ITEMMENU_WORK* pWork );
static void _itemSellExit( FIELD_ITEMMENU_WORK* pWork );
static s32 _get_item_sell_price( int item_no, int input_num, HEAPID heapID );
//static void BAG_ItemUseErrorMsgSet( MYSTATUS * myst, STRBUF * buf, u16 item, u32 err, FIELD_ITEMMENU_WORK * pWork );
//u8 Bag_TalkMsgPrint( BAG_WORK * pWork, int type );
//static int Bag_MenuUse( FIELD_ITEMMENU_WORK * pWork );
static BOOL BAGMAIN_NPlanterUseCheck( u16 pocket, u16 item );
static void ItemMenuMake( FIELD_ITEMMENU_WORK * pWork, u8* tbl );
static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork);
int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work );
static void	_VBlank( GFL_TCB *tcb, void *work );
static void _startState(FIELD_ITEMMENU_WORK* pWork);

#ifdef PM_DEBUG
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

static void _changeState(FIELD_ITEMMENU_WORK* pWork,StateFunc state)
{
  pWork->state = state;
  GFL_UI_KEY_SetRepeatSpeed(8, 15);  //元に戻す
}

//------------------------------------------------------------------------------
/**
 * @brief   通信管理ステートの変更
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef PM_DEBUG
static void _changeStateDebug(FIELD_ITEMMENU_WORK* pWork,StateFunc state, int line)
{
  OS_TPrintf("BAG STATE: %d\n",line);
  _changeState(pWork, state);
}
#endif


#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(5)
#define	FBMP_COL_GRN_SDW	(6)
#define	FBMP_COL_BLUE		(7)
#define	FBMP_COL_BLU_SDW	(8)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE		(15)
#define _BMPMENULIST_FONTSIZE   (16)




///選択メニューのリスト
static BMPMENULIST_HEADER _itemMenuListHeader = {
  NULL,			// 表示文字データポインタ
  NULL,					// カーソル移動ごとのコールバック関数
  NULL,					// 一列表示ごとのコールバック関数
  NULL,                      // GF_BGL_BMPWIN* win
  9,// リスト項目数
  9,// 表示最大項目数
  0,						// ラベル表示Ｘ座標
  16,						// 項目表示Ｘ座標
  0,						// カーソル表示Ｘ座標
  0,						// 表示Ｙ座標
  FBMP_COL_BLACK,			// 文字色
  FBMP_COL_WHITE,			// 背景色
  FBMP_COL_BLK_SDW,		// 文字影色
  0,						// 文字間隔Ｘ
  18,						// 文字間隔Ｙ
  BMPMENULIST_NO_SKIP,		// ページスキップタイプ
  0,//FONT_SYSTEM,				// 文字指定
  0,						// ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  NULL,                   // work
  _BMPMENULIST_FONTSIZE,			//文字サイズX(ドット
  _BMPMENULIST_FONTSIZE,			//文字サイズY(ドット
  NULL,		//表示に使用するメッセージバッフ
  NULL,		//表示に使用するプリントユーティ
  NULL,		//表示に使用するプリントキュー
  NULL,		//表示に使用するフォントハンドル
};

static void _windowCreate(FIELD_ITEMMENU_WORK* pWork)
{
  _windowRewrite(pWork);

}


static void _windowRewrite(FIELD_ITEMMENU_WORK* pWork)
{

  ITEMDISP_upMessageRewrite(pWork);
  ITEMDISP_WazaInfoWindowChange(pWork);
  ITEMDISP_CellMessagePrint(pWork);
  pWork->bChange = TRUE;


}

//------------------------------------------------------------------------------
/**
 * @brief   ポケットにあるアイテムを返す
 * @retval  none
 */
//------------------------------------------------------------------------------

ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  ITEM_ST * item;

  if(pWork->moveMode){
    item = &pWork->ScrollItem[no];
  }
  else{
    item = MYITEM_PosItemGet( pWork->pMyItem, pWork->pocketno, no );
  }
  return item;
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケットにあるアイテムの総数を返す
 * @retval  none
 */
//------------------------------------------------------------------------------

int ITEMMENU_GetItemPocketNumber(FIELD_ITEMMENU_WORK* pWork)
{
  int length = 0;

  if(pWork->moveMode){
    length = MYITEM_GetItemThisPocketNumber(pWork->ScrollItem, BAG_MYITEM_MAX);

  }
  else{
    length = MYITEM_GetItemPocketNumber( pWork->pMyItem, pWork->pocketno);
  }
  return length;
}


static void _ItemChangeSingle(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 )
{
  ITEM_ST temp;
  GFL_STD_MemCopy(&pWork->ScrollItem[no1], &temp, sizeof(ITEM_ST));
  GFL_STD_MemCopy(&pWork->ScrollItem[no2],&pWork->ScrollItem[no1], sizeof(ITEM_ST));
  GFL_STD_MemCopy(&temp, &pWork->ScrollItem[no2], sizeof(ITEM_ST));
}


static void _ItemChange(FIELD_ITEMMENU_WORK* pWork, int no1, int no2 )
{
  int i;

  if(no1 == no2){
    return;
  }
  else if(no1 < no2){
    for(i = no1; i < no2 ; i++ ){
      _ItemChangeSingle(pWork, i, i+1);
    }
  }
  else{
    for(i = no1; i > no2 ; i-- ){
      _ItemChangeSingle(pWork, i, i-1);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケットに応じたカーソル位置を覚えておく 新しいポケットでのカーソル位置を引き出す
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _pocketCursorChange(FIELD_ITEMMENU_WORK* pWork,int oldpocket, int newpocket)
{
  s16 cur,scr;

  MYITEM_FieldBagPocketSet(pWork->pBagCursor, oldpocket);
  MYITEM_FieldBagCursorSet(pWork->pBagCursor, oldpocket, pWork->curpos, pWork->oamlistpos+1 );
  MYITEM_FieldBagCursorGet(pWork->pBagCursor, newpocket, &cur, &scr );
  pWork->curpos = cur;
  pWork->oamlistpos = scr - 1;
  ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
  ITEMDISP_PocketMessage(pWork, newpocket);
  ITEMDISP_ChangePocketCell( pWork,newpocket );
  ITEMDISP_ItemInfoWindowChange(pWork,newpocket);


}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムインデックスをカーソルとリストの位置から計算
 * @retval  none
 */
//------------------------------------------------------------------------------

int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork)
{
  return pWork->curpos + pWork->oamlistpos + 1;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  下キーが押された時の処理
 *
 *	@param	pWork
 *	@param	length
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static BOOL _posplus(FIELD_ITEMMENU_WORK* pWork, int length)
{
  BOOL bChange = FALSE;

  if((pWork->curpos==4) && ((pWork->oamlistpos+7) < length)){
    //カーソルはそのままでリストが移動
    pWork->oamlistpos++;
    bChange = TRUE;
  }
  else if((pWork->curpos==4) && ((pWork->curpos+1) < length)){
    //リストの終端まで来たのでカーソルが移動
    pWork->curpos++;
    bChange = TRUE;
  }
  else if((pWork->curpos!=5) && ((pWork->curpos+1) < length)){
    pWork->curpos++;
    bChange = TRUE;
  }

  GFL_SOUND_PlaySE( SE_BAG_CURSOR_MOVE );

  return bChange;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  上キーが押された時の処理
 *
 *	@param	pWork
 *	@param	length
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static BOOL _posminus(FIELD_ITEMMENU_WORK* pWork, int length)
{
  BOOL bChange = FALSE;

  if((pWork->curpos==1) && (pWork->oamlistpos!=-1)){
    //カーソルはそのままでリストが移動
    pWork->oamlistpos--;
    bChange = TRUE;
  }
  else if((pWork->curpos==1)){
    //リストの終端まで来たのでカーソルが移動
    pWork->curpos--;
    bChange = TRUE;
  }
  else if(pWork->curpos != 0){
    pWork->curpos--;
    bChange = TRUE;
  }

  GFL_SOUND_PlaySE( SE_BAG_CURSOR_MOVE );
  
  return bChange;
}


//------------------------------------------------------------------------------
/**
 * @brief   スクロールの処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _itemScrollCheck(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x,y,i;
  int ymax = _SCROLL_BOTTOM_Y - _SCROLL_TOP_Y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    if((y <= _SCROLL_TOP_Y) || (y >= _SCROLL_BOTTOM_Y)){
      return FALSE;
    }
    if((x >= (32*8)) || (x <= (28*8)) ){
      return FALSE;
    }
    {
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      int num = (length * (y-_SCROLL_TOP_Y)) / ymax;

      pWork->curpos = 0;
      pWork->oamlistpos = -1;
      for(i = 0 ; i < num ; i++){
        _posplus(pWork, length);
      }
    }
    return TRUE;
  }
  return FALSE;
}


//------------------------------------------------------------------------------
/**
 * @brief   キーの動きの処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _keyChangeItemCheck(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange = FALSE;
  int nowno = ITEMMENU_GetItemIndex(pWork);
  {
    int pos = pWork->curpos;
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_DOWN){
      bChange = _posplus(pWork, length);
    }
    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_UP){
      bChange = _posminus(pWork, length);
    }
  }
  if(bChange){
    int newno = ITEMMENU_GetItemIndex(pWork);
    _ItemChange(pWork, nowno, newno);
  }
  return bChange;
}



//------------------------------------------------------------------------------
/**
 * @brief   アイテム交換時のキーの動きの処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _keyMoveCheck(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange = FALSE;
  {
    int pos = pWork->curpos;
    int length = ITEMMENU_GetItemPocketNumber( pWork);

    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_DOWN){
      bChange = _posplus(pWork, length);
    }
    if(GFL_UI_KEY_GetRepeat()== PAD_KEY_UP){
      bChange = _posminus(pWork, length);
    }
  }
  return bChange;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテム移動モード時のアイテム部分のタッチ処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _itemMovePositionTouchItem(FIELD_ITEMMENU_WORK* pWork)
{
  int nowno = ITEMMENU_GetItemIndex(pWork);
  u32 x,y,i;
  int ymax = _SCROLL_BOTTOM_Y - _SCROLL_TOP_Y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    if((y <= _SCROLL_TOP_Y)  || (y >= _SCROLL_BOTTOM_Y)){
      return FALSE;
    }
    if((x >= (30*8)) || (x <= (18*8)) ){
      return FALSE;
    }
    {
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      BOOL bChange=FALSE;
      int num = (6 * (y-_SCROLL_TOP_Y)) / ymax;  //カーソルを移動させたい量

      if(pWork->curpos == num){
        return FALSE;
      }
      else if(pWork->curpos < num){
        for(i = pWork->curpos ; i < num; i++){
          bChange += _posplus(pWork, length);
        }
      }
      else{
        for(i = pWork->curpos ; i > num; i--){
          bChange += _posminus(pWork, length);
        }
      }
    }
    if(0){
      int length = ITEMMENU_GetItemPocketNumber( pWork);
      int num = (length * (y-_SCROLL_TOP_Y)) / ymax;

      pWork->curpos = 0;
      pWork->oamlistpos = -1;
      for(i = 0 ; i < num ; i++){
        _posplus(pWork, length);
      }
    }
    {
      int newno = ITEMMENU_GetItemIndex(pWork);
      _ItemChange(pWork, nowno, newno);
    }
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテム移動モード時のキーの処理
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemMovePosition(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bChange=FALSE;
  u32 trg = GFL_UI_KEY_GetTrg();

 	GFL_UI_KEY_SetRepeatSpeed(1, 6);

  if((trg == PAD_BUTTON_A) || (trg == PAD_BUTTON_SELECT)){  //反映
    MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, FALSE);
  }
  if((trg == PAD_BUTTON_B) || (trg == PAD_BUTTON_A) || (trg == PAD_BUTTON_SELECT) ){  //戻る
    pWork->moveMode = FALSE;
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur , 1 );
    return;
  }

  if( _itemScrollCheck(pWork) ){   // スクロールバーの操作
    ITEMDISP_scrollCursorMove(pWork);
    bChange = TRUE;
  }
  else if(_itemMovePositionTouchItem(pWork)){
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }
  else if(_keyChangeItemCheck(pWork)){   // キーの操作
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }
  if(bChange){
    _windowRewrite(pWork);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムを選択してどういう動作を行うかきいているところ
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemSelectWait(FIELD_ITEMMENU_WORK* pWork)
{
  BOOL bClear=FALSE;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    pWork->ret_code2 = pWork->submenuList[selectno];
    OS_Printf("ret_code2 %d %d\n", pWork->ret_code2,selectno);

    if((BAG_MENU_TSUKAU==pWork->ret_code2)&&(pWork->pocketno==BAG_POKE_WAZA)){ //技マシン
      _CHANGE_STATE(pWork,_itemTecniqueUseInit);
    }
    else if((BAG_MENU_TSUKAU==pWork->ret_code2)&&( 1==ITEM_GetParam( pWork->ret_item, ITEM_PRM_EVOLUTION, pWork->heapID )  )){
      pWork->ret_code = BAG_NEXTPROC_EVOLUTION;  //進化
      _CHANGE_STATE(pWork,NULL);
    }
    else if((ITEM_ZITENSYA == pWork->ret_item) && (BAG_MENU_TSUKAU==pWork->ret_code2)){
      pWork->ret_code = BAG_NEXTPROC_RIDECYCLE;  //のる
      _CHANGE_STATE(pWork,NULL);
    }
    else if((ITEM_ZITENSYA == pWork->ret_item) && (BAG_MENU_ORIRU==pWork->ret_code2)){
      pWork->ret_code = BAG_NEXTPROC_DROPCYCLE;  //おりる
      _CHANGE_STATE(pWork,NULL);
    }
    else if(BAG_MENU_SUTERU==pWork->ret_code2){  //すてる
      _CHANGE_STATE(pWork,_itemTrash);
    }
    else if(pWork->ret_item == ITEM_TAUNMAPPU){
      pWork->ret_code = BAG_NEXTPROC_TOWNMAP;  //タウンマップ
      _CHANGE_STATE(pWork,NULL);
    }
    else if(pWork->ret_item == ITEM_TOMODATITETYOU){
      pWork->ret_code = BAG_NEXTPROC_FRIENDNOTE;  //ともだち手帳
      _CHANGE_STATE(pWork,NULL);
    }
    else if(pWork->ret_item == ITEM_PARESUHEGOO){
      pWork->ret_code = BAG_NEXTPROC_PALACEJUMP;  //@TODO 仮 パレスへゴー
      _CHANGE_STATE(pWork,NULL);
    }
    else if(BAG_MENU_YAMERU==pWork->ret_code2){  //やめる
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
    else if(BAG_MENU_TSUKAU==pWork->ret_code2){
      pWork->ret_code = BAG_NEXTPROC_ITEMUSE;  //つかう
      _CHANGE_STATE(pWork,NULL);
    }
    else if(BAG_MENU_URU==pWork->ret_code2){
      pWork->ret_code = BAG_NEXTPROC_SELL;  //うる
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      pWork->ret_code = BAG_NEXTPROC_HAVE;
      _CHANGE_STATE(pWork,NULL);
    }
    bClear = TRUE;
  }
  else if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_B){
    bClear = TRUE;
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }
  if(bClear){
    ITEMDISP_ListPlateClear( pWork );
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }

}



//------------------------------------------------------------------------------
/**
 * @brief   アイテムを選択してどういう動作を行うかきいているところ
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemSelectState(FIELD_ITEMMENU_WORK* pWork)
{
  {  //選んだアイテムのセット
    ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
    pWork->ret_item = ITEM_DUMMY_DATA;
    if(item){
      pWork->ret_item = item->id;
    }
  }
  if(pWork->mode == BAG_MODE_POKELIST)
  {
    pWork->ret_code = BAG_NEXTPROC_ITEMEQUIP;
    _CHANGE_STATE(pWork,NULL);
  }
  else if( pWork->mode == BAG_MODE_SELL )
  {
    _CHANGE_STATE(pWork,_itemSellInit);
  }
  else
  {
    _itemUseWindowRewrite(pWork);
    ITEMDISP_ItemInfoMessageMake( pWork,pWork->ret_item );
    ITEMDISP_ItemInfoWindowDisp( pWork );
    _CHANGE_STATE(pWork,_itemSelectWait);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   キーの処理
 * @retval  none
 */
//------------------------------------------------------------------------------


//カーソルの位置
//OAMLIST の 先頭位置 -1は表示しない



static void _itemKindSelectMenu(FIELD_ITEMMENU_WORK* pWork)
{
  u32	ret=0;
  BOOL bChange=FALSE;

 	GFL_UI_KEY_SetRepeatSpeed(1, 6);

  GFL_BMN_Main( pWork->pButton );
  if(pWork->state == NULL){
    return;
  }

  // 決定
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE)
  {
    // アイテムに対する動作選択画面へ
    _CHANGE_STATE(pWork,_itemSelectState);
    return;
  }

  // キャンセル
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_B)
  {
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    _CHANGE_STATE(pWork,NULL);
    return;
  }

  // 並び替え
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_SELECT)
  {
    GFL_STD_MemClear(pWork->ScrollItem, sizeof(pWork->ScrollItem));
    MYITEM_ITEM_STCopy(pWork->pMyItem, pWork->ScrollItem, pWork->pocketno, TRUE);  //取得
    pWork->moveMode = TRUE;
    GFL_CLACT_WK_SetAnmSeq( pWork->clwkCur , 2 );

    _CHANGE_STATE(pWork,_itemMovePosition);
    return;
  }

  // スクロール
  if( _itemScrollCheck(pWork) )
  {
    ITEMDISP_scrollCursorMove(pWork);
    bChange = TRUE;
  }
  else if(_keyMoveCheck(pWork))
  {
    ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
    bChange = TRUE;
  }

  {
    int oldpocket = pWork->pocketno;
    if(GFL_UI_KEY_GetTrg() == PAD_KEY_RIGHT)
    {
      GFL_SOUND_PlaySE( SE_BAG_POCKET_MOVE );
      pWork->pocketno++;
    }
    if(GFL_UI_KEY_GetTrg() == PAD_KEY_LEFT)
    {
      GFL_SOUND_PlaySE( SE_BAG_POCKET_MOVE );
      pWork->pocketno--;
    }
    if(pWork->pocketno >= BAG_POKE_MAX)
    {
      pWork->pocketno = 0;
    }
    if(pWork->pocketno < 0)
    {
      pWork->pocketno = BAG_POKE_MAX-1;
    }
    if(oldpocket != pWork->pocketno)
    {
      _pocketCursorChange(pWork, oldpocket, pWork->pocketno);
      bChange = TRUE;
    }
  }

  if(bChange)
  {
    _windowRewrite(pWork);
  }

}

//=============================================================================
//
//
//  __TECNIQUE 技マシン シーケンス
//
//
//=============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   技マシン確認中 はいいえまち
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseYesNo(FIELD_ITEMMENU_WORK* pWork)
{
  u32 ret = 0;//= TOUCH_SW_Main( pWork->pTouchSWSys );

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      ITEMDISP_ListPlateClear( pWork );
      pWork->ret_code = BAG_NEXTPROC_WAZASET;
      _CHANGE_STATE(pWork,NULL);
    }
    else{
      ITEMDISP_ListPlateClear( pWork );
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
    // YESNOの後始末
    ITEMDISP_YesNoExit(pWork);
  }

}

//------------------------------------------------------------------------------
/**
 * @brief   技マシン確認中
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  ITEMDISP_YesNoStart(pWork);

  _CHANGE_STATE(pWork,_itemTecniqueUseYesNo);

}

//------------------------------------------------------------------------------
/**
 * @brief   技マシン起動
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTecniqueUseInit(FIELD_ITEMMENU_WORK* pWork)
{

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_065, pWork->pStrBuf );
  WORDSET_RegisterWazaName(pWork->WordSet, 0, ITEM_GetWazaNo( pWork->ret_item ));
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );
  _CHANGE_STATE(pWork,_itemTecniqueUseWait);
}

//=============================================================================
//
//
//  __TRASH 捨てる シーケンス
//
//
//=============================================================================

//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の操作 はいまち
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashEndWait(FIELD_ITEMMENU_WORK* pWork)
{
  // @TODO タッチ
  if(PAD_BUTTON_DECIDE == GFL_UI_KEY_GetTrg()){
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の操作 はいまち
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  _windowRewrite(pWork);
  _CHANGE_STATE(pWork,_itemTrashEndWait);



}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の操作 はいいいえまち
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesNoWait(FIELD_ITEMMENU_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    
    // YESNOの後始末
    ITEMDISP_YesNoExit(pWork);
    
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    if(selectno==0){
      // 手持ちからアイテム削除
      MYITEM_SubItem(pWork->pMyItem, pWork->ret_item, pWork->InputNum, pWork->heapID );

      // 捨てる音
      GFL_SOUND_PlaySE( SE_BAG_TRASH );

      GFL_MSG_GetString( pWork->MsgManager, msg_bag_055, pWork->pStrBuf );
      WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
      WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->InputNum,
                             3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      ITEMDISP_ItemInfoWindowDisp( pWork );

      // 再描画---

      _CHANGE_STATE(pWork,_itemTrashYesWait);
    }
    else{
      _CHANGE_STATE(pWork,_itemKindSelectMenu);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の操作
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashYesNo(FIELD_ITEMMENU_WORK* pWork)
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  ITEMDISP_YesNoStart(pWork);
  _CHANGE_STATE(pWork,_itemTrashYesNoWait);
}


//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の操作
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrashWait(FIELD_ITEMMENU_WORK* pWork)
{
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  int backup = pWork->InputNum;

  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);

    GFL_MSG_GetString( pWork->MsgManager, msg_bag_056, pWork->pStrBuf );
    WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
    WORDSET_RegisterNumber(pWork->WordSet, 1, pWork->InputNum,
                           3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    ITEMDISP_ItemInfoWindowDisp( pWork );
    _CHANGE_STATE(pWork,_itemTrashYesNo);
  }
  else if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_CANCEL){
    G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_OBJ , 0 );
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
    _CHANGE_STATE(pWork,_itemKindSelectMenu);
  }

  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_UP){
    pWork->InputNum++;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_DOWN){
    pWork->InputNum--;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_RIGHT){
    pWork->InputNum += 10;
  }
  else if(GFL_UI_KEY_GetRepeat() == PAD_KEY_LEFT){
    pWork->InputNum -= 10;
  }


  if(item->no < pWork->InputNum){
    pWork->InputNum = item->no;
  }
  else if(pWork->InputNum < 1){
    pWork->InputNum = 1;
  }
  if(pWork->InputNum != backup){
    ITEMDISP_TrashNumDisp(pWork,pWork->InputNum);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテムを捨てる為の表示
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _itemTrash(FIELD_ITEMMENU_WORK* pWork)
{
  pWork->InputNum = 1;  //初期化

  ITEMDISP_NumFrameDisp(pWork);

  ITEMDISP_TrashNumDisp(pWork,pWork->InputNum);

  GFL_MSG_GetString( pWork->MsgManager, msg_bag_054, pWork->pStrBuf );
  WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );
    
  //@TODO リストとBAR双方にOBJが使われているためバッティングする。OAMの半透明を切替るシステムが必要
//  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 /*| GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1*/ | GX_BLEND_PLANEMASK_OBJ , -8 );

  _CHANGE_STATE(pWork,_itemTrashWait);
}


//=============================================================================
//
//
//  __SELL 売る シーケンス
//
//
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス 初期化
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellInit( FIELD_ITEMMENU_WORK* pWork )
{
  // 買えないもの判定
  {
    s32 val;
    s32 event;

    val   = ITEM_GetParam( pWork->ret_item, ITEM_PRM_PRICE, pWork->heapID );
    event = ITEM_GetParam( pWork->ret_item, ITEM_PRM_EVENT, pWork->heapID );

    // 売値が0、もじくは重要フラグがONの時は売れない
    if( val == 0 || event )
    {
      // 「○○○を　かいとることは　できません」
      GFL_MSG_GetString( pWork->MsgManager, mes_shop_093, pWork->pStrBuf );
      WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf );
      ITEMDISP_ItemInfoWindowDisp( pWork );

      _CHANGE_STATE( pWork, _itemSellEndMsgWait );
      return;
    }
  }

  pWork->InputNum = 1;  //初期化
  
  // おこづかい表示開始
  ITEMDISP_GoldDispIn( pWork );

  // アイテムがひとつしかない場合、入力をスキップ
  {
    ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );

    HOSAKA_Printf("item->no:%d \n", item->no );

    if( item->no == 1 )
    {
      _CHANGE_STATE( pWork, _itemSellYesnoInit );
      return;
    }
  }

  ITEMDISP_NumFrameDisp(pWork);

  ITEMDISP_TrashNumDisp(pWork,pWork->InputNum);

  GFL_MSG_GetString( pWork->MsgManager, mes_shop_094, pWork->pStrBuf );
  WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
  ITEMDISP_ItemInfoWindowDisp( pWork );

  _CHANGE_STATE( pWork, _itemSellInputWait );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス 入力待ち
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellInputWait( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // @TODO タッチ対応

  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
  {
    // 決定
    _CHANGE_STATE( pWork, _itemSellYesnoInit );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL )
  {
    // キャンセル
    _CHANGE_STATE( pWork, _itemSellExit );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス YESNO開始
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellYesnoInit( FIELD_ITEMMENU_WORK* pWork )
{
  // 数値入力フレームクリア
  GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
  
  // YESNO開始
  ITEMDISP_YesNoStart(pWork);

  // それでしたら XXXXXX円で おひきとり　いたしましょう
  {
    s32 val;

    // 売値を取得
    val = _get_item_sell_price( pWork->ret_item, pWork->InputNum, pWork->heapID );
  
    GFL_MSG_GetString( pWork->MsgManager, mes_shop_095, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, val,
                             6, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
    ITEMDISP_ItemInfoWindowDisp( pWork );
  }

    _CHANGE_STATE( pWork, _itemSellYesnoInput );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス YESNO選択まち
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellYesnoInput( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  if(APP_TASKMENU_IsFinish(pWork->pAppTask))
  {
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    // YESNOの後始末
    ITEMDISP_YesNoExit(pWork);

    switch( selectno )
    {
      case 0 :
        // YES
        {
          s32 val;

          // 売値を取得
          val = _get_item_sell_price( pWork->ret_item, pWork->InputNum, pWork->heapID );

          // 手持ちからアイテム削除
          MYITEM_SubItem( pWork->pMyItem, pWork->ret_item, pWork->InputNum, pWork->heapID );

          // 所持金増加
          Mystatus_AddGold( pWork->mystatus, val );

          // 売却音
          GFL_SOUND_PlaySE( SE_BAG_SELL );
  
          // 再描画
          _windowRewrite(pWork);

          // 「○○○を　わたして XXXXXX円 うけとった」
          GFL_MSG_GetString( pWork->MsgManager, mes_shop_096, pWork->pStrBuf );
          WORDSET_RegisterItemName(pWork->WordSet, 0,  pWork->ret_item );
          WORDSET_RegisterNumber(pWork->WordSet, 1, val,
                                   6, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
          WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
          ITEMDISP_ItemInfoWindowDisp( pWork );

          _CHANGE_STATE( pWork, _itemSellEndMsgWait );
        }
        break;

      case 1 :
        // NO
        _CHANGE_STATE( pWork, _itemSellExit );
        break;

      default : GF_ASSERT(0);
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス 売った後のメッセージウィエト
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellEndMsgWait( FIELD_ITEMMENU_WORK* pWork )
{
  if(!ITEMDISP_MessageEndCheck(pWork)){
    return;
  }

  // 入力待ち
  if( ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ) || GFL_UI_TP_GetTrg() )
  {
    _CHANGE_STATE( pWork, _itemSellExit );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  売るシーケンス 終了
 *
 *	@param	FIELD_ITEMMENU_WORK* pWork 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _itemSellExit( FIELD_ITEMMENU_WORK* pWork )
{
  GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
  
  // おこづかい表示終了
  ITEMDISP_GoldDispOut( pWork );

  _CHANGE_STATE( pWork, _itemKindSelectMenu );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  アイテムの売値を取得
 *
 *	@param	int item_no 売るアイテムNO
 *	@param	input_num   売る個数
 *	@param	heapID      ヒープID
 *
 *	@retval s32 売値
 */
//-----------------------------------------------------------------------------
static s32 _get_item_sell_price( int item_no, int input_num, HEAPID heapID )
{
  s32 val;

  val = ITEM_GetParam( item_no, ITEM_PRM_PRICE, heapID );

  // 売り値は半分
  val /= 2;

  // 個数
  val *= input_num;

  return val;
}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * アイテム使用エラーメッセージセット
 *
 * @param	myst	プレーヤーデータ
 * @param	buf		メッセージ展開場所
 * @param	item	アイテム番号
 * @param	err		エラーID
 * @param	heap	ヒープID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BAG_ItemUseErrorMsgSet( MYSTATUS * myst, STRBUF * buf, u16 item, u32 err, FIELD_ITEMMENU_WORK * pWork )
{
  WORDSET * wset;
  STRBUF * str;

  switch( err ){
  case ITEMCHECK_ERR_CYCLE_OFF:		// 自転車を降りれない
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_058, buf );
    break;
  case ITEMCHECK_ERR_COMPANION:		// 使用不可・連れ歩き
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_pair, buf );
    break;
  case ITEMCHECK_ERR_DISGUISE:		// 使用不可・ロケット団変装中
    GFL_MSG_GetString( pWork->MsgManager, msg_bag_err_disguise, buf );
    break;
  default:							// 使用不可・博士の言葉
    wset = WORDSET_Create( pWork->heapID );
    str  = GFL_MSG_CreateString( pWork->MsgManager, msg_bag_059 );
    WORDSET_RegisterPlayerName( wset, 0, myst );
    WORDSET_ExpandStr( wset, buf, str );
    GFL_STR_DeleteBuffer( str );
    WORDSET_Delete( wset );
  }
}


//--------------------------------------------------------------------------------------------
/**
 * 会話（風）メッセージ表示
 *
 * @param	pWork		バッグ画面ワーク
 * @param	type	BAG_TALK_FULL=画面一杯, BAG_TALK_MINI=画面2/3
 *
 * @return	メッセージID
 */
//--------------------------------------------------------------------------------------------
u8 Bag_TalkMsgPrint( BAG_WORK * pWork, int type )
{
  u8	idx;
  GF_BGL_BMPWIN *win;
  if(type==BAG_TALK_FULL){
    win = &pWork->win[WIN_TALK];
  }else{
    // 呼び出せない時がある
    GF_ASSERT( pWork->sub_win[ADD_WIN_MINITALK].ini!=NULL );
    win = &pWork->sub_win[ADD_WIN_MINITALK];
  }

  GF_BGL_BmpWinDataFill( win, 15 );
  BmpTalkWinWrite( win, WINDOW_TRANS_OFF, TALK_SUB_WIN_CGX_NUM, TALKWIN_PAL );
  GF_BGL_BmpWinOnVReq( win );

  MsgPrintSkipFlagSet( MSG_SKIP_ON );
  MsgPrintAutoFlagSet( MSG_AUTO_OFF );

  idx = GF_STR_PrintSimple(	win, FONT_TALK,pWork->expb, 0, 0,
                            CONFIG_GetMsgPrintSpeed( pWork->cfg ), BAG_TalkMsgCallBack );

  return idx;
}

//--------------------------------------------------------------------------------------------
/**
 * メニュー：つかう
 *
 * @param	pWork		バッグ画面のワーク
 *
 * @return	移行するメインシーケンス
 */
//--------------------------------------------------------------------------------------------
static int Bag_MenuUse( FIELD_ITEMMENU_WORK * pWork )
{
  ITEMCHECK_FUNC	check;
  s32	id;

  // どうぐ使用処理取得
  id    = ITEM_GetParam( pWork->ret_item, ITEM_PRM_FIELD, pWork->heapID );
  // 使うチェックをする関数
  check = (ITEMCHECK_FUNC)ITEMUSE_GetFunc( ITEMUSE_PRM_CHECKFUNC, id );

  // どうぐ使用チェック
  if( check != NULL ){
    u32	ret = check( &pWork->icpWork );
    if( ret != ITEMCHECK_TRUE ){
      BAG_ItemUseErrorMsgSet( pWork->mystatus, pWork->expb, pWork->ret_item, ret, pWork );
      pWork->midx = Bag_TalkMsgPrint( pWork, BAG_TALK_FULL );
      return SEQ_ITEM_ERR_WAIT;
    }
  }

  return BAG_ItemUse( pWork );
}
#endif



//--------------------------------------------------------------------------------------------
/**
 * Useメニュー作成
 * @param	pWork		バッグ画面のワーク
 * @return	none
 */
//--------------------------------------------------------------------------------------------
#if 1

int (*MenuFuncTbl[])( FIELD_ITEMMENU_WORK *pWork)={
  //Bag_MenuUse,		// つかう
  //Bag_MenuUse,		// おりる
  //Bag_MenuUse,		// みる
  //Bag_MenuUse,		// うめる
  //Bag_MenuUse,		// ひらく
  //Bag_MenuSub,		// すてる
  //Bag_MenuCnvSet,		// とうろく
  //Bag_MenuCnvDel,		// かいじょ
  //Bag_MenuSet,		// もたせる
  //Bag_MenuTag,		// タグをみる
  //Bag_MenuKettei,		// けってい
  NULL,				// やめる
  //Bag_MenuItemMove,	// いどう
  NULL,				// うる
  //Bag_MenuPlanterUse,	// つかう（木の実プランター用）
  //Bag_MenuUse,		// とめる
};

static BOOL BAGMAIN_NPlanterUseCheck( u16 pocket, u16 item )
{
  if( pocket == BAG_POKE_NUTS ||
      ( item >= COMPOST_START_ITEMNUM && item < (COMPOST_START_ITEMNUM+ITEM_COMPOST_MAX) ) ){
    return TRUE;
  }
  return FALSE;
}

static void ItemMenuMake( FIELD_ITEMMENU_WORK * pWork, u8* tbl )
{
  int	 i;
  void * itemdata;
  u8	pocket;
  ITEM_ST * item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  if((item==NULL) || (item->id==ITEM_DUMMY_DATA)){
    return;
  }

  itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
  pocket   = pWork->pocketno;

  for(i=0;i<BAG_MENUTBL_MAX;i++){
    pWork->menu_func[i] = NULL;
  }

  // フィールド
  if( pWork->mode == BAG_MODE_FIELD ){
    // コロシアム・ユニオンルームでは「みる」のみ
    if( pWork->mode == BAG_MODE_COLOSSEUM || pWork->mode == BAG_MODE_UNION )
    {
      tbl[BAG_MENU_USE] = BAG_MENU_MIRU;
    }
    else
    {
      // つかう
      // おりる
      // みる
      // ひらく
      // うめる
      // とめる
      if( ITEM_GetBufParam( itemdata,  ITEM_PRM_FIELD ) != 0 )
      {
        if( item->id == ITEM_ZITENSYA && pWork->cycle_flg == 1 )
        {
          tbl[BAG_MENU_USE] = BAG_MENU_ORIRU;
        }
        else if( item->id == ITEM_POFINKEESU )
        {
          tbl[BAG_MENU_USE] = BAG_MENU_HIRAKU;
        }
        //			else if( pWork->ret_item == ITEM_gbPUREIYAA && Snd_GameBoyFlagCheck() == 1 )
        //		{
        //		tbl[BAG_MENU_USE] = BAG_MENU_TOMERU;
        //}
        else
        {
          tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU;
        }
      }
    }
    // もたせる
    // すてる
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_EVENT ) == 0 ){
      if( ITEM_CheckPokeAdd( item->id ) == TRUE ){
        tbl[BAG_MENU_GIVE] = BAG_MENU_MOTASERU;
      }
      if( pocket != BAG_POKE_WAZA ){
        // 技マシンじゃない場合は捨てるを登録
        tbl[BAG_MENU_SUB] = BAG_MENU_SUTERU;
      }
    }
    // とうろく
    // かいじょ
    if( ITEM_GetBufParam( itemdata, ITEM_PRM_CNV ) != 0 ){
      if( ITEMMENU_GetPosCnvButtonItem( pWork, item->id  ) != -1 ){
        tbl[BAG_MENU_SUB] = BAG_MENU_KAIZYO;
      }else{
        tbl[BAG_MENU_SUB] = BAG_MENU_TOUROKU;
      }
    }
  }
  // 木の実プランター
  else if( pWork->mode == BAG_MODE_N_PLANTER ){
    if( BAGMAIN_NPlanterUseCheck( pocket, item->id ) == TRUE ){
      tbl[BAG_MENU_USE] = BAG_MENU_TSUKAU_NP;
    }
  }

  // やめる
  tbl[BAG_MENU_CANCEL] = BAG_MENU_YAMERU;

  OS_Printf("pocket=%d\n",   pocket);
  OS_Printf("tbl[BAG_MENU_USE]=%d\n",   tbl[BAG_MENU_USE]);
  OS_Printf("tbl[BAG_MENU_GIVE]=%d\n",  tbl[BAG_MENU_GIVE]);
  OS_Printf("tbl[BAG_MENU_SUB]=%d\n",   tbl[BAG_MENU_SUB]);
  OS_Printf("tbl[BAG_MENU_MOVE]=%d\n",   tbl[BAG_MENU_ITEMMOVE]);

  for(i=0;i<BAG_MENUTBL_MAX;i++){
    if(tbl[i]!=255){
      pWork->menu_func[i] = MenuFuncTbl[tbl[i]];
    }
    OS_Printf("menu_func[%d]=%08x\n", i,pWork->menu_func[i]);
  }


  GFL_HEAP_FreeMemory( itemdata );
}

#endif


static void _itemUseWindowRewrite(FIELD_ITEMMENU_WORK* pWork)
{
  BMPMENULIST_HEADER list_h = _itemMenuListHeader;
  int length,i,j;


  length = BAG_MENUTBL_MAX;


  {
    u8	tbl[BAG_MENUTBL_MAX]={255, 255, 255, 255, 255};
    int strtbl[]={msg_bag_001,msg_bag_007,msg_bag_017,mes_bag_104,mes_bag_105,
      msg_bag_002,msg_bag_003,msg_bag_019,msg_bag_004,msg_bag_005,msg_bag_006,
      msg_bag_009, msg_bag_094, mes_shop_103,msg_bag_001,msg_bag_menu_ex_01};
    int stringbuff[BAG_MENUTBL_MAX];

    ItemMenuMake(pWork, tbl);

    for(i=0,j=0; i< BAG_MENUTBL_MAX ; i++){
      if(tbl[i]==255){
        continue;
      }
      pWork->submenuList[j]=tbl[i];
      stringbuff[j] = strtbl[tbl[i]];
      j++;
    }
    ITEMDISP_MenuWinDisp(pWork,stringbuff,j);


  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	便利登録されていたら順番を返す
 */
//-----------------------------------------------------------------------------

int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i;
  for(i = 0 ; i < DUMMY_SHORTCUT_MAX ; i++){
    if(no == MYITEM_CnvButtonItemGet( pWork->pMyItem, i)){
      return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
/**
 *	@brief	チェックを追加
 */
//-----------------------------------------------------------------------------

BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i;
  for(i = 0 ; i < DUMMY_SHORTCUT_MAX ; i++){
    if(0 == MYITEM_CnvButtonItemGet( pWork->pMyItem, i)){
      MYITEM_CnvButtonItemSet( pWork->pMyItem, i, no);
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	チェックを消す
 */
//-----------------------------------------------------------------------------

void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no)
{
  int i=ITEMMENU_GetPosCnvButtonItem(pWork,no);

  if(i!=-1){
    MYITEM_CnvButtonItemSet( pWork->pMyItem, i, 0);
  }
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
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  FIELD_ITEMMENU_WORK* pWork = p_work;
  int pocketno = -1;
  int length = ITEMMENU_GetItemPocketNumber(pWork);

  if(event!=GFL_BMN_EVENT_TOUCH){
    return;
  }
  if(BAG_POKE_MAX > bttnid){
    int id2no[]={1,2,3,4,0};
    pocketno = id2no[bttnid];  //どうぐのあたりが広い為、順番を最後にしている
  }
  else if(BUTTONID_LEFT == bttnid){
    pocketno = pWork->pocketno;
    pocketno--;
    if(pocketno < 0){
      pocketno = BAG_POKE_MAX-1;
    }
  }
  else if(BUTTONID_RIGHT == bttnid){
    pocketno = pWork->pocketno;
    pocketno++;
    if(pocketno >= BAG_POKE_MAX){
      pocketno = 0;
    }
  }
  else if(BUTTONID_EXIT == bttnid){
    pWork->ret_code = BAG_NEXTPROC_EXIT;
    _CHANGE_STATE(pWork, NULL);
  }
  else if(BUTTONID_RETURN == bttnid){
    pWork->ret_code = BAG_NEXTPROC_RETURN;
    _CHANGE_STATE(pWork, NULL);
  }
  else if((bttnid >= BUTTONID_ITEM_AREA) && (bttnid < BUTTONID_CHECK_AREA)){
    if(_itemKindSelectMenu == pWork->state){
      int backup = pWork->curpos;
      int nowno = ITEMMENU_GetItemIndex(pWork);

      if(pWork->curpos != (bttnid-BUTTONID_ITEM_AREA)){
        pWork->curpos = bttnid - BUTTONID_ITEM_AREA;
        if(length <= ITEMMENU_GetItemIndex(pWork)){
          pWork->curpos = backup;
          return;
        }
      }
      _ItemChange(pWork, nowno, ITEMMENU_GetItemIndex(pWork));
      //      ITEMDISP_scrollCursorChangePos(pWork, ITEMMENU_GetItemIndex(pWork));
      _windowRewrite(pWork);
      _CHANGE_STATE(pWork,_itemSelectState);
    }
    return;
  }
  else if(bttnid >= BUTTONID_CHECK_AREA){
    if(pWork->pocketno == BAG_POKE_EVENT){
      int no = bttnid - BUTTONID_CHECK_AREA + pWork->oamlistpos + 1;
      ITEM_ST * item = ITEMMENU_GetItem( pWork, no );
      if(length <= no){
        return;
      }
      {
        void * itemdata;
        int ret;
        itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
        ret = ITEM_GetBufParam( itemdata, ITEM_PRM_CNV );
        GFL_HEAP_FreeMemory( itemdata );
        if( ret == 0 ){
          return;
        }
      }



      if(ITEMMENU_GetPosCnvButtonItem(pWork,item->id)!=-1){//チェックつき
        ITEMMENU_RemoveCnvButtonItem(pWork,item->id);
      }
      else{
        ITEMMENU_AddCnvButtonItem(pWork,item->id);
      }
      _windowRewrite(pWork);
    }
  }

  if(pocketno != -1){
    _pocketCursorChange(pWork, pWork->pocketno, pocketno);
    pWork->pocketno = pocketno;
    _windowRewrite(pWork);
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
  FIELD_ITEMMENU_WORK* pWork = work;

  GFL_CLACT_SYS_VBlankFunc();	//セルアクターVBlank
  if(pWork->bChange){
    ITEMDISP_CellVramTrans(pWork);
    pWork->bChange = FALSE;
  }
}


static void _startState(FIELD_ITEMMENU_WORK* pWork)
{
  ITEMDISP_SetVisible();
  if (GFL_FADE_CheckFade() == FALSE) {
    _CHANGE_STATE(pWork, _itemKindSelectMenu);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   プロセス初期化
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_Init( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;

  GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0);

// @TODO 保坂のみショップから起動したことにする
#ifdef DEBUG_ONLY_FOR_genya_hosaka
  if( (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG) == 0 )
  {
    pWork->mode = BAG_MODE_SELL;
    HOSAKA_Printf("hook sell\n");
  }
#endif

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ITEMMENU, 0x28000 );
  pWork->heapID = HEAPID_ITEMMENU;
  pWork->pBagCursor = GAMEDATA_GetBagCursor(GAMESYSTEM_GetGameData(pWork->gsys));

  pWork->pocketno = MYITEM_FieldBagPocketGet(pWork->pBagCursor);
  {
    s16 cur,scr;
    MYITEM_FieldBagCursorGet(pWork->pBagCursor, pWork->pocketno, &cur, &scr );
    pWork->curpos = cur;
    pWork->oamlistpos = scr - 1;
  }
  ITEMDISP_graphicInit(pWork);

  pWork->pMyItem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(pWork->gsys));
  pWork->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                      NARC_message_bag_dat, pWork->heapID );

  pWork->MsgManagerItemInfo = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                              NARC_message_iteminfo_dat, pWork->heapID );

  pWork->MsgManagerPocket = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                            NARC_message_itempocket_dat, pWork->heapID );

  pWork->pStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->pExpStrBuf = GFL_STR_CreateBuffer(200,pWork->heapID);
  pWork->WordSet    = WORDSET_Create( pWork->heapID );
  pWork->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr ,
                                       GFL_FONT_LOADTYPE_FILE , FALSE , pWork->heapID );

  ITEMDISP_upMessageCreate(pWork);
  ITEMDISP_CellResourceCreate(pWork);
  ITEMDISP_InitPocketCell( pWork );

  _windowCreate(pWork);
  ITEMDISP_CellCreate(pWork);

  pWork->pButton = GFL_BMN_Create( bttndata, _BttnCallBack, pWork,  pWork->heapID );

  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , FLD_SUBSCR_FADE_DIV , FLD_SUBSCR_FADE_SYNC , pWork->heapID );

  pWork->g3dVintr = GFUser_VIntr_CreateTCB( _VBlank, (void*)pWork, 0 );

  pWork->pMsgTcblSys = GFL_TCBL_Init( pWork->heapID , pWork->heapID , 1 , 0 );

  //  pWork->pTouchSWSys = TOUCH_SW_AllocWork(pWork->heapID);


  ITEMDISP_BarMessageCreate( pWork );

	pWork->pAppTaskRes	= APP_TASKMENU_RES_Create( GFL_BG_FRAME3_M, _SUBLIST_NORMAL_PAL,pWork->fontHandle, pWork->SysMsgQue, pWork->heapID  );

  pWork->count=2;

  GFL_NET_ChangeIconPosition(256-16, 0);
  GFL_NET_ReloadIcon();

  _CHANGE_STATE(pWork, _startState);
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   プロセスMain
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_Main( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;
  StateFunc* state = pWork->state;

  if(state == NULL){
    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
    return GFL_PROC_RES_FINISH;
  }
  if( WIPE_SYS_EndCheck() != TRUE ){
    return GFL_PROC_RES_CONTINUE;
  }
  state(pWork);
  _dispMain(pWork);

  PRINTSYS_QUE_Main(pWork->SysMsgQue);


  GFL_TCBL_Main( pWork->pMsgTcblSys );

  return GFL_PROC_RES_CONTINUE;

}

//------------------------------------------------------------------------------
/**
 * @brief   プロセスEnd
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT FieldItemMenuProc_End( GFL_PROC * proc, int * seq, void * ppWork, void * mypWork )
{
  FIELD_ITEMMENU_WORK* pWork = ppWork;

  if (GFL_FADE_CheckFade() == TRUE) {
    return GFL_PROC_RES_CONTINUE;
  }

  // GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0);
  GFL_TCB_DeleteTask( pWork->g3dVintr );

  MYITEM_FieldBagPocketSet(pWork->pBagCursor, pWork->pocketno);
  MYITEM_FieldBagCursorSet(pWork->pBagCursor, pWork->pocketno, pWork->curpos, pWork->oamlistpos+1 );

	APP_TASKMENU_RES_Delete( pWork->pAppTaskRes );

  ITEMDISP_upMessageDelete(pWork);
  ITEMDISP_graphicDelete(pWork);

  //  TOUCH_SW_FreeWork(pWork->pTouchSWSys);

  GFL_TCBL_Exit(pWork->pMsgTcblSys);// = GFL_TCBL_Init( work->heapId , work->heapId , 1 , 0 );


  GFL_MSG_Delete(pWork->MsgManager);
  GFL_MSG_Delete(pWork->MsgManagerItemInfo);
  GFL_MSG_Delete(pWork->MsgManagerPocket);

  GFL_STR_DeleteBuffer(pWork->pStrBuf);
  GFL_STR_DeleteBuffer(pWork->pExpStrBuf);
  WORDSET_Delete(pWork->WordSet);

  ITEMDISP_BarMessageDelete( pWork );

  if(pWork->itemInfoDispWin){

    GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_M,
                             GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar),
                             GFL_ARCUTIL_TRANSINFO_GetSize(pWork->bgchar));

    GFL_BMPWIN_Delete(pWork->itemInfoDispWin);
  }

  GFL_BG_FillCharacterRelease(GFL_BG_FRAME3_M,1,0);

  GFL_FONT_Delete(pWork->fontHandle);
  PRINTSYS_QUE_Clear(pWork->SysMsgQue);
  PRINTSYS_QUE_Delete(pWork->SysMsgQue);
  GFL_BMN_Delete(pWork->pButton);
  //  ITEMDISP_ListPlateDelete(pWork);

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();


  GFL_HEAP_DeleteHeap(  HEAPID_ITEMMENU );

  return GFL_PROC_RES_FINISH;

}



// プロセス定義データ
const GFL_PROC_DATA ItemMenuProcData = {
  FieldItemMenuProc_Init,
  FieldItemMenuProc_Main,
  FieldItemMenuProc_End,
};
