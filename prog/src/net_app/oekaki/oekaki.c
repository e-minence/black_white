//============================================================================================
/**
 * @file  oekaki.c
 * @brief お絵かきボード画面処理
 * @author  Akito Mori(移植）
 * @date    09.01.20
 *
 *
 * ・TOUCH_INFOの配列を２に減らす
 * ・親の送信FIFOを作る
 * ・描画情報を１頂点にするから途切れる。だから２頂点にして線分をかならず引かせるようにすれば
 * 　いいんじゃないか？
 * 　↑描いてない時のクライアントの判定が全部作り直しのような気がする…
 *
 * 乱入者が来て送信し始めたときに誰かが電プチしていなくなる。
 * 離脱メンバーがいなくなる処理が終了した時にされる事で親が乱入OKにしてしまう事で
 * 途中乱入がOKになる状態が確認できたのでココをおさえることにする
 *   ChangeConnectMax(wk, 1) とConnectLimitSet(がほとんと同じ機能なのに混在しているので
 * 再チェック。
 */
//============================================================================================
#define DEBUGPLAY_ONE ( 0 )

#include <gflib.h>
#include <calctool.h>

#include "system/main.h"
#include "print/wordset.h"
#include "font/font.naix"
#include "arc/arc_def.h"
#include "arc/message.naix"
#include "msg/msg_oekaki.h"
#include "system/wipe.h"
#include "system/bmp_winframe.h"
#include "system/net_err.h"
#include "system/gfl_use.h"
#include "gamesystem/msgspeed.h"
#include "print/printsys.h"
#include "sound/pm_sndsys.h"
#include "app/app_menu_common.h"
#include "savedata/etc_save.h"

#include "net_app/oekaki.h"
#include "oekaki_local.h"
#include "comm_command_oekaki.h"

//#include "msgdata/msg_ev_win.h"


#include "arc/oekaki.naix"      // グラフィックアーカイブ定義

//============================================================================================
//  定数定義
//============================================================================================

// 文字列描画用のパレット定義
#define NAME_COL_MINE   ( PRINTSYS_LSB_Make(  3,  4,  0) )  // 自分の名前
#define NAME_COL_NORMAL ( PRINTSYS_LSB_Make(0x1,0x2,0xf) )  // 参加メンバーの名前
#define STRING_COL_MSG  ( PRINTSYS_LSB_Make(0x1,0x2,0xf) )  // 「やめる」

#define MESFRAME_PAL      ( 10 )   // メッセージウインドウ
#define MENUFRAME_PAL     ( 11 )   // メニューウインドウ

// SE定義
#define OEKAKI_DECIDE_SE     ( SEQ_SE_SELECT1 )
#define OEKAKI_BS_SE         ( SEQ_SE_SELECT4 )
#define OEKAKI_NEWMEMBER_SE  ( SEQ_SE_DECIDE1 )
#define OEKAKI_PEN_CHANGE_SE ( SEQ_SE_SELECT1 )
#define OEKAKI_CANCEL_SE     ( SEQ_SE_CANCEL1 )

// はい・いいえ
#define YESNO_RET_YES   ( 0 )
#define YESNO_RET_NO    ( 1 )
#define YESNO_RET_NONE  ( 2 )

//============================================================================================
//  プロトタイプ宣言
//============================================================================================

/*** 関数プロトタイプ ***/
static void VBlankFunc( GFL_TCB *tcb, void * work );
static void VramBankSet(void);
static void BgInit( void );
static void InitWork( OEKAKI_WORK *wk );
static void FreeWork( OEKAKI_WORK *wk );
static void BgExit(void);
static void BgGraphicSet( OEKAKI_WORK * wk, ARCHANDLE* p_handle );
static void InitCellActor(OEKAKI_WORK *wk, ARCHANDLE* p_handle);
static void SetCellActor(OEKAKI_WORK *wk);
static void BmpWinInit(OEKAKI_WORK *wk, GFL_PROC* proc);
static void BmpWinDelete( OEKAKI_WORK *wk );
static void SetCursor_Pos( GFL_CLWK *act, int x, int y );
static void NormalTouchFunc(OEKAKI_WORK *wk);
static  int Oekaki_MainNormal( OEKAKI_WORK *wk, int seq );
static void EndSequenceCommonFunc( OEKAKI_WORK *wk );
static  int Oekaki_EndSelectPutString( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectWait( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectAnswerWait( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectParentCall( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectAnswerOK( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectSendOK( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectAnswerNG( OEKAKI_WORK *wk, int seq );
static  int  Oekaki_EndChild( OEKAKI_WORK *wk, int seq );
static  int  Oekaki_EndChildWait( OEKAKI_WORK *wk, int seq );
static  int  Oekaki_EndChildWait2( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectParent( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectParentWait( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndSelectParentSendEnd( OEKAKI_WORK *wk, int seq );
static  int Oekaki_ForceEnd( OEKAKI_WORK *wk, int seq );
static  int Oekaki_ForceEndWait( OEKAKI_WORK *wk, int seq );
static  int Oekaki_ForceEndSynchronize( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndParentOnly( OEKAKI_WORK *wk, int seq );
static  int Oekaki_EndParentOnlyWait( OEKAKI_WORK *wk, int seq );
static void DrawPoint_to_Line(  GFL_BMPWIN *win,   const u8 *brush,  int px, int py, int *sx, int *sy,   int count, int flag );
static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock );
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw );
static void MoveCommCursor( OEKAKI_WORK *wk );
static void DebugTouchDataTrans( OEKAKI_WORK *wk );
static void CursorColTrans(u16 *CursorCol);
static void NameCheckPrint( GFL_BMPWIN *win[], PRINTSYS_LSB color, OEKAKI_WORK *wk );
static  int ConnectCheck( OEKAKI_WORK *wk );
static void LineDataSendRecv( OEKAKI_WORK *wk );
static  int MyStatusCheck( OEKAKI_WORK *wk );
static void EndMessagePrint( OEKAKI_WORK *wk, int msgno, int wait );
static  int EndMessageWait( OEKAKI_WORK *wk );
static void EndMessageWindowOff( OEKAKI_WORK *wk );
static  int OnlyParentCheck( OEKAKI_WORK *wk );
static  int Oekaki_LogoutChildMes( OEKAKI_WORK *wk, int seq );
static  int Oekaki_LogoutChildClose( OEKAKI_WORK *wk, int seq );
static  int Oekaki_LogoutChildMesWait( OEKAKI_WORK *wk, int seq );
static  int MyStatusGetNum( OEKAKI_WORK *wk );
static  int Oekaki_NewMemberWait( OEKAKI_WORK *wk, int seq );
static  int Oekaki_NewMember( OEKAKI_WORK *wk, int seq );
static  int Oekaki_NewMemberEnd( OEKAKI_WORK *wk, int seq );
static void PalButtonAppearChange( GFL_CLWK *act[], int no);
static void EndButtonAppearChange( GFL_CLWK *act, BOOL flag );
static void _BmpWinPrint_Rap(
             GFL_BMPWIN * win, void * src,
             int src_x, int src_y, int src_dx, int src_dy,
             int win_x, int win_y, int win_dx, int win_dy );
static void ChangeConnectMax( OEKAKI_WORK *wk, int plus );
static  int ConnectNumControl( OEKAKI_WORK *wk );
static  int Oekaki_ForceEndWaitNop( OEKAKI_WORK *wk, int seq );
static void SetNextSequence( OEKAKI_WORK *wk, int nextSequence );
static void OekakiSequenceControl( OEKAKI_WORK *wk, int proc_seq );

static  int FakeEndYesNoSelect( OEKAKI_WORK  *wk );
static void SetTouchpanelData( TOUCH_INFO *touchResult, TP_ONE_DATA *tpData, int brush_color, int brush );
static  int _get_connect_bit( OEKAKI_WORK *wk );
static  int _get_connect_num( OEKAKI_WORK *wk );
static void _disp_on( void );
static void OEKAKI_entry_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork);
static void OEKAKI_leave_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork);
static APP_TASKMENU_WORK  *YesNoMenuInit( OEKAKI_WORK *wk );
static  u32 YesNoMenuMain( OEKAKI_WORK *wk );
static void YesNoMenuDelete( OEKAKI_WORK *wk );
static void OekakiResetYesNoWin(OEKAKI_WORK *wk);
static void Oekaki_PrintFunc( OEKAKI_WORK *wk );
static void _comm_friend_add( OEKAKI_WORK *wk );
static void Oekaki_SendFunc( OEKAKI_WORK *wk );
static void _wordset_username( OEKAKI_WORK *wk, int word_id, int comm_id );

//====================================================
/// シーケンス管理構造体
//====================================================
typedef struct{
  int (*func)(OEKAKI_WORK *wk, int seq);  // 関数
  int execOn_SeqLeave;                    // 実行可能フラグ
}OEKAKI_FUNC_TABLE;

static OEKAKI_FUNC_TABLE FuncTable[]={
  {NULL,                      1,},  // OEKAKI_MODE_INIT                       = 0,
  {Oekaki_NewMember,          0,},  // OEKAKI_MODE_NEWMEMBER,
  {Oekaki_NewMemberWait,      0,},  // OEKAKI_MODE_NEWMEMBER_WAIT,
  {Oekaki_NewMemberEnd,       0,},  // OEKAKI_MODE_NEWMEMBER_END,
  {Oekaki_MainNormal,         0,},  // OEKAKI_MODE,
  {Oekaki_EndSelectPutString, 1,},  // OEKAKI_MODE_END_SELECT,                5
  {Oekaki_EndSelectWait,      1,},  // OEKAKI_MODE_END_SELECT_WAIT,
  {Oekaki_EndSelectAnswerWait,1,},  // OEKAKI_MODE_END_SELECT_ANSWER_WAIT
  {Oekaki_EndSelectParentCall,1,},  // OEKAKI_MODE_END_SELECT_PARENT_CALL
  {Oekaki_EndSelectAnswerOK,  1,},  // OEKAKI_MODE_END_SELECT_ANSWER_OK
  {Oekaki_EndSelectSendOK,    1,},  // OEKAKI_MODE_END_SELECT_SEND_OK         10
  {Oekaki_EndSelectAnswerNG,  1,},  // OEKAKI_MODE_END_SELECT_ANSWER_NG
  {Oekaki_EndChild,           1,},  // OEKAKI_MODE_END_CHILD
  {Oekaki_EndChildWait,       1,},  // OEKAKI_MODE_END_CHILD_WAIT
  {Oekaki_EndChildWait2,      1,},  // OEKAKI_MODE_END_CHILD_WAIT2
  {Oekaki_EndSelectParent,    1,},  // OEKAKI_MODE_END_SELECT_PARENT          15
  {Oekaki_EndSelectParentWait,1,},  // OEKAKI_MODE_END_SELECT_PARENT_WAIT
  {Oekaki_EndSelectParentSendEnd,1,},  // OEKAKI_MODE_END_SELECT_PARENT_SEND_END
  {Oekaki_ForceEnd,           1,},  // OEKAKI_MODE_FORCE_END
  {Oekaki_ForceEndWait,       1,},  // OEKAKI_MODE_FORCE_END_WAIT
  {Oekaki_ForceEndSynchronize,1,},  // OEKAKI_MODE_FORCE_END_SYNCHRONIZE      20
  {Oekaki_ForceEndWaitNop,    1,},  // OEKAKI_MODE_FORCE_END_WAIT_NOP
  {Oekaki_EndParentOnly,      1,},  // OEKAKI_MODE_END_PARENT_ONLY
  {Oekaki_EndParentOnlyWait,  1,},  // OEKAKI_MODE_END_PARENT_ONLY_WAIT
  {Oekaki_LogoutChildMes,     1,},  // OEKAKI_MODE_LOGOUT_CHILD
  {Oekaki_LogoutChildMesWait, 1,},  // OEKAKI_MODE_LOGOUT_CHILD_WAIT          25
  {Oekaki_LogoutChildClose,   1,},  // OEKAKI_MODE_LOGOUT_CHILD_CLOSE
};


//--------------------------------------------------------------------------------------------
/**
 * お絵かきPROCDATA
 */
//--------------------------------------------------------------------------------------------
const GFL_PROC_DATA OekakiProcData = {
  OekakiProc_Init,
  OekakiProc_Main,
  OekakiProc_End,
};



//----------------------------------------------------------------------------------
/**
 * @brief 乱入可能人数を設定（お絵かきでは0か1しか設定しない）
 *
 * @param   wk    OEKAKI_WORK
 * @param   num   0:乱入禁止  1:一人だけ乱入可能
 */
//----------------------------------------------------------------------------------
static void ConnectLimitSet( OEKAKI_WORK *wk, int num)
{
  Union_App_Parent_EntryBlockNum( wk->param->uniapp, num );
  OS_Printf("乱入人数を%d人に設定\n", num);
}


//============================================================================================
//  プロセス関数
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：初期化
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT OekakiProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK * wk;
  ARCHANDLE* p_handle;

  switch(*seq){
  case 0:
    GFL_DISP_GX_InitVisibleControl();
    GFL_DISP_GXS_InitVisibleControl();
  
    OS_Printf("OEKAKI_HEAP Cretae, before remain = %08x\n", GFL_HEAP_GetHeapFreeSize(GFL_HEAPID_APP));
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OEKAKI, 0x50000 );

    wk = GFL_PROC_AllocWork( proc, sizeof(OEKAKI_WORK), HEAPID_OEKAKI );
    GFL_STD_MemFill( wk, 0, sizeof(OEKAKI_WORK) );
    GFL_BG_Init( HEAPID_OEKAKI );
    
    // 文字列マネージャー生成
    wk->WordSet    = WORDSET_Create( HEAPID_OEKAKI );
    wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                     NARC_message_oekaki_dat, HEAPID_OEKAKI );
    wk->font       = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                      GFL_FONT_LOADTYPE_FILE , FALSE , HEAPID_OEKAKI );

    // VRAM バンク設定
    VramBankSet();
    
    // BGシステム初期化
    BgInit();          

    // 輝度変更セット
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEIN, WIPE_TYPE_HOLEIN, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    
    // パラメータ取得
    {
      PICTURE_PARENT_WORK *param = (PICTURE_PARENT_WORK*)pwk;
      wk->param = param;
    }

    // ハンドルオープン
    p_handle = GFL_ARC_OpenDataHandle( ARCID_OEKAKI, HEAPID_OEKAKI );

    //BGグラフィックセット
    BgGraphicSet( wk, p_handle );

    // VBlank関数セット
    wk->vblankTcb = GFUser_VIntr_CreateTCB( VBlankFunc, wk, 0 );  
  
    // ワーク初期化
    InitWork( wk );

    // CellActorシステム初期化
    InitCellActor(wk, p_handle);
    
    // CellActro表示登録
    SetCellActor(wk);

    // BMPWIN登録・描画
    BmpWinInit(wk,proc);

    // 画面ON（出力の上下入れ替えも）
    _disp_on();

    // 通信コマンドをおえかきボード用に変更
    OEKAKIBOARD_CommandInitialize( wk );

        // お絵かき時には接続切断でエラー扱いしない
//        CommStateSetErrorCheck(FALSE,TRUE);
    
    // 接続している反対側のIDのメンバーを通信友達登録
    _comm_friend_add(wk);

    //無線アイコン表示
    GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_OEKAKI );

    // ハンドルクローズ
    GFL_ARC_CloseDataHandle( p_handle );
    (*seq)++;
    break;
  case 1:
    (*seq) = SEQ_IN;
    return GFL_PROC_RES_FINISH;
    break;
  }
  return GFL_PROC_RES_CONTINUE;
}



#ifdef PM_DEBUG
static int debugseq,debugsubseq;
#endif

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：メイン
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------

GFL_PROC_RESULT OekakiProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK * wk  = (OEKAKI_WORK *)mywk;

  // 親機は接続中bitテーブルと離脱申告bitテーブルを&比較して0になるのを待つ
  if(GFL_NET_SystemGetCurrentID() == 0 && wk->ridatu_bit != 0){
    wk->ridatu_bit &= _get_connect_bit(wk);
  }
  
  // ワークに現在のprocシーケンスを保存
  wk->proc_seq = *seq;

  // サブシーケンス遷移制御を行う
  OekakiSequenceControl( wk, *seq );
  
  switch( *seq ){
  case SEQ_IN:
    // ワイプ処理待ち
    if( WIPE_SYS_EndCheck() ){
      // 乱入・退出コールバック登録
      Union_App_SetCallback( wk->param->uniapp, OEKAKI_entry_callback, OEKAKI_leave_callback, wk);
      // 1人まで乱入可能の設定に(開始した時は２人でここにくるのであと一人だけ入れるようにしておく）
      if(GFL_NET_SystemGetCurrentID()==0){
        ConnectLimitSet(wk,1);
      }

      //親はメインへ
      *seq = SEQ_MAIN;

    }
    break;

  case SEQ_MAIN:
    // カーソル移動

    // シーケンス毎の動作
    if(FuncTable[wk->seq].func!=NULL){
      *seq = FuncTable[wk->seq].func( wk, *seq );
    }
    // 接続人数を確認して上画面に名前を描画
    NameCheckPrint( wk->TrainerNameWin, NAME_COL_NORMAL ,wk);

    if(GFL_NET_SystemGetCurrentID()==0){        // 親は
      int temp = ConnectNumControl(wk);
      if(*seq == SEQ_MAIN){
        // 既に終了シーケンスなどに入っている場合は書き換えない
        *seq = temp;
      }
    }
    CursorColTrans(&wk->CursorPal);

    break;
  case SEQ_LEAVE:
    if(FuncTable[wk->seq].func!=NULL && FuncTable[wk->seq].execOn_SeqLeave){
      *seq = FuncTable[wk->seq].func( wk, *seq );
    }
    break;

  case SEQ_OUT:
    if( WIPE_SYS_EndCheck() ){
      OS_Printf("ワイプ終了確認\n");
      // 通信コマンドテーブル解放
      GFL_NET_DelCommandTable( GFL_NET_CMD_PICTURE );
      // 通信切断
      Union_App_Shutdown( wk->param->uniapp );
      *seq = SEQ_DISCONNECT_WAIT;
    }
    break;
  case SEQ_DISCONNECT_WAIT:
    // ユニオン通信切断待ち
    if(Union_App_WaitShutdown(wk->param->uniapp)){
      return GFL_PROC_RES_FINISH;
    }
    break;
  }
  
  // データ送信リクエスト処理
  Oekaki_SendFunc( wk );

#ifdef PM_DEBUG
  if(debugseq!=*seq || debugsubseq!=wk->seq){
    OS_Printf("seq=%d, subseq=%d sharebit=%d ridatu_bit=%d\n", *seq, wk->seq, wk->shareBit, wk->ridatu_bit);
    debugseq = *seq;
    debugsubseq = wk->seq;
  }
#endif
  GFL_CLACT_SYS_Main();             // セルアクター常駐関数
  GFL_TCBL_Main( wk->pMsgTcblSys );
  Oekaki_PrintFunc(wk);

  // フェード中じゃない時に通信エラーチェック
  if( WIPE_SYS_EndCheck() ){
    if(NetErr_App_CheckError()!=NET_ERR_STATUS_NULL){
      OS_Printf("------------------------通信エラー--------------------\n");
      return ( GFL_PROC_RES_FINISH );
    }
  }

  
  return GFL_PROC_RES_CONTINUE;
}



#define DEFAULT_NAME_MAX    18

// ダイヤ・パールで変わるんだろう
#define MALE_NAME_START     0
#define FEMALE_NAME_START   18

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：終了
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
GFL_PROC_RESULT OekakiProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk )
{
  OEKAKI_WORK  *wk    = (OEKAKI_WORK  *)mywk;
  PICTURE_PARENT_WORK *param = (PICTURE_PARENT_WORK*)pwk;
  int i;

  switch(*seq){
  case 0:
    OS_Printf("おえかきワーク解放処理突入\n");

    // Vblank期間中のBG転送終了
    GFL_TCB_DeleteTask( wk->vblankTcb );

    // セルアクターリソース解放
    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_S_CHR] );     // サブ面
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_S_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_S_CELL] );
    
    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_SYS_CHR] );     // システムアイコン
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_SYS_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_SYS_CELL] );

    GFL_CLGRP_CGR_Release(  wk->resObjTbl[CLACT_RES_M_CHR] );     // メイン面
    GFL_CLGRP_PLTT_Release( wk->resObjTbl[CLACT_RES_M_PLTT] );
    GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[CLACT_RES_M_CELL] );
    
    
    // セルアクターユニット破棄
    GFL_CLACT_UNIT_Delete( wk->clUnit );

    //OAMレンダラー破棄
    GFL_CLACT_SYS_Delete();
    
    // はい・いいえが残っている時用の解放処理
    if(wk->app_menuwork){
      YesNoMenuDelete( wk );
    }
    
    // BMPウィンドウ開放
    BmpWinDelete( wk );

    // BGL削除
    BgExit();

    // メッセージマネージャー・ワードセットマネージャー解放
    GFL_MSG_Delete( wk->MsgManager );
    WORDSET_Delete( wk->WordSet );
    GFL_FONT_Delete( wk->font );

    (*seq)++;
    break;
  case 1:
    // 入れ替わっていた上下画面出力を元に戻す
//    GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);


    (*seq)++;
    break;
  case 2:
    if (wk->ireagalJoin){
      OS_Printf("親機だけになるまで待機\n");  
      if (_get_connect_bit(wk) == 1){
        (*seq)++;
      }
    }else{
      (*seq)++;
    }
    break;
  case 3:
    // ワーク解放
    FreeWork( wk );

    GFL_PROC_FreeWork( proc );        // GFL_PROCワーク開放

    GFL_HEAP_DeleteHeap( HEAPID_OEKAKI );


    return GFL_PROC_RES_FINISH;
  }
  return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * VBlank関数
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *tcb, void * work )
{
  // セルアクター
  GFL_CLACT_SYS_VBlankFunc();

  // BG転送  
  GFL_BG_VBlankFunc();
  
}


//----------------------------------------------------------------------------------
/**
 * @brief 【ユニオン乱入コールバック】乱入処理
 *         乱入コールバックは３台目からの子機しか発生しない
 *
 * @param   net_id      接続ＩＤ
 * @param   mystatus    MYSTATUS
 * @param   userwork    OEKAKI_WORK*
 */
//----------------------------------------------------------------------------------
static void OEKAKI_entry_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  OEKAKI_WORK  *wk    = (OEKAKI_WORK  *)userwork;
  if(net_id==0){
    OS_Printf("親は乱入扱いにしない net_id=%d\n", net_id);
  }
  
  // 別な子機の乱入に対処
  if(GFL_NET_SystemGetCurrentID()==0){
      // 2台目以降の子機の乱入
      // 全台に「これから絵を送るので止まってください」と送信する
      Oekaki_SendDataRequest( wk, CO_OEKAKI_STOP, net_id );
  }

  // 接続したメンバーを通信友達登録
  _comm_friend_add(wk);


}

//----------------------------------------------------------------------------------
/**
 * @brief 【ユニオン退出コールバック】退出時処理
 *
 * @param   net_id      接続ＩＤ
 * @param   mystatus    MYSTATUS
 * @param   userwork    OEKAKI_WORK*
 */
//----------------------------------------------------------------------------------
static void OEKAKI_leave_callback(NetID net_id, const MYSTATUS *mystatus, void *userwork)
{
  OEKAKI_WORK  *wk    = (OEKAKI_WORK  *)userwork;

  OS_Printf("離脱コールバック net_id=%d 人数=%d\n", net_id,_get_connect_num(wk));
}

//--------------------------------------------------------------------------------------------
// VRAM設定data
//--------------------------------------------------------------------------------------------
static const GFL_DISP_VRAM OekakiDispVramDat = {
    GX_VRAM_BG_128_A,           // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット

    GX_VRAM_SUB_BG_128_C,       // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // サブ2DエンジンのBG拡張パレット

    GX_VRAM_OBJ_128_B,          // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット

    GX_VRAM_SUB_OBJ_16_I,       // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// サブ2DエンジンのOBJ拡張パレット

    GX_VRAM_TEX_NONE,           // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_NONE,       // テクスチャパレットスロット

    GX_OBJVRAMMODE_CHAR_1D_32K, // メイン面OBJVRAMサイズ
    GX_OBJVRAMMODE_CHAR_1D_32K, // サブ面OBJVRAMサイズ
    
  };


//--------------------------------------------------------------------------------------------
/**
 * VRAM設定
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet(void)
{
  GFL_DISP_SetBank( &OekakiDispVramDat );
}

//--------------------------------------------------------------------------------------------
/**
 * BG設定
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( void )
{
  // BG SYSTEM
  { 
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  // メイン画面文字版0
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME0_M );


  }

  // メイン画面ラクガキ面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME1_M );
  }

  // メイン画面背景
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }


  // サブ画面テキスト面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(  GFL_BG_FRAME0_S );
  }

  // サブ画面背景面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x128,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl(  GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
  }

  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_OEKAKI );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 32, 0, HEAPID_OEKAKI );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 32, 0, HEAPID_OEKAKI ); ///< 文字面以外もキャラクリアした。080422
  GFL_BG_SetClearCharacter( GFL_BG_FRAME1_S, 32, 0, HEAPID_OEKAKI );

}


#define TALK_MESSAGE_BUF_NUM  ( 40*2 )

//------------------------------------------------------------------
/**
 * お絵かきワーク初期化
 *
 * @param   wk    OEKAKI_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitWork( OEKAKI_WORK *wk )
{
  int i;

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->TrainerName[i] = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE, HEAPID_OEKAKI );
    wk->AllTouchResult[i].size = 0;
    wk->OldTouch[i].size = 0;
    wk->TrainerStatus[i][0] = NULL;
    wk->TrainerStatus[i][1] = NULL;

  }
  // 「やめる」文字列バッファ作成
  wk->EndString  = GFL_STR_CreateBuffer( 10, HEAPID_OEKAKI );
  wk->TalkString = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_OEKAKI );

  // ブラシ初期化
  wk->brush_color  = 0;
  wk->brush        = MIDDLE_BRUSH;
  wk->banFlag      = OEKAKI_BAN_OFF;
  wk->yesno_flag   = 0;
  wk->shareNum     = 2;
  wk->shareBit     = _get_connect_bit(wk);
  wk->firstChild   = 0;
  wk->ireagalJoin  = 0;
  wk->seq          = 0;
  wk->bookJoin     = 0;
  wk->joinBit      = 0;
  wk->app_menuwork = NULL;

  // 親は通信で絵を受け取る必要が無い
  if( GFL_NET_SystemGetCurrentID()==0 ){
    SetNextSequence( wk, OEKAKI_MODE );
  }else{
    // 絵を受け取らないといけない
    SetNextSequence( wk, OEKAKI_MODE );
  }
  
  // 「やめる」取得
  GFL_MSG_GetString( wk->MsgManager, msg_oekaki_08, wk->EndString );

  // お絵かき画像圧縮データ領域
  wk->lz_buf = GFL_HEAP_AllocMemory( HEAPID_OEKAKI, 30*16*32 );

  // 接続人数監視用ワーク初期化
  wk->connectBackup = 0;
}

//------------------------------------------------------------------
/**
 * @brief   ワーク解放
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void FreeWork( OEKAKI_WORK *wk )
{
  int i;

  GFL_HEAP_FreeMemory(  wk->lz_buf );


  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_STR_DeleteBuffer( wk->TrainerName[i] );
  }
  GFL_STR_DeleteBuffer( wk->TalkString ); 
  GFL_STR_DeleteBuffer( wk->EndString  ); 

}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param ini   BGLデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{
//  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
  GX_SetMasterBrightness(-16);
  GXS_SetMasterBrightness(-16);

  GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
  GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

  GFL_BG_Exit();
}


#define TALKFONT_PAL_ROW      ( 13 )
#define TALKFONT_PAL_OFFSET   ( TALKFONT_PAL_ROW*0x20 )
//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param wk    ポケモンリスト画面のワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( OEKAKI_WORK * wk, ARCHANDLE* p_handle )
{

  // 上下画面ＢＧパレット転送
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_oekaki_oekaki_m_NCLR, PALTYPE_MAIN_BG, 0, 16*2*12,  HEAPID_OEKAKI);
  GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_oekaki_oekaki_s_NCLR, PALTYPE_SUB_BG,  0, 16*2*12,  HEAPID_OEKAKI);
  
  // 会話フォントパレット転送
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_OEKAKI );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 
                                 TALKFONT_PAL_OFFSET, 32, HEAPID_OEKAKI );

  // メイン画面BG2キャラ転送
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_oekaki_oekaki_m_NCGR, GFL_BG_FRAME2_M, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

  // メイン画面BG2スクリーン転送
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_oekaki_oekaki_m_NSCR, GFL_BG_FRAME2_M, 0, 32*24*2, 0, HEAPID_OEKAKI);



  // サブ画面BG1キャラ転送
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_oekaki_oekaki_s_NCGR, GFL_BG_FRAME1_S, 0, 32*8*0x20, 0, HEAPID_OEKAKI);

  // サブ画面BG1スクリーン転送
  GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_oekaki_oekaki_s_NSCR, GFL_BG_FRAME1_S, 0, 32*24*2, 0, HEAPID_OEKAKI);

  // サブ画面会話ウインドウグラフィック転送
  BmpWinFrame_GraphicSet(
        GFL_BG_FRAME0_M, 1, MESFRAME_PAL,  0, HEAPID_OEKAKI );

  BmpWinFrame_GraphicSet(
        GFL_BG_FRAME0_M, 1+TALK_WIN_CGX_SIZ, MENUFRAME_PAL, 0, HEAPID_OEKAKI );

}


//------------------------------------------------------------------
/**
 * レーダー画面用セルアクター初期化
 *
 * @param   wk    レーダー構造体のポインタ
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitCellActor(OEKAKI_WORK *wk, ARCHANDLE* p_handle)
{
  int i;
  
  // 共通素材ファイルハンドルオープン
  ARCHANDLE *c_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_OEKAKI );
  // セルアクター初期化
  GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &OekakiDispVramDat, HEAPID_OEKAKI );
  
  
  // セルアクター初期化
  wk->clUnit = GFL_CLACT_UNIT_Create( 50+3, 1,  HEAPID_OEKAKI );
  
  
  
  //---------メイン画面用-------------------
  // お絵かき素材
  //chara読み込み
  wk->resObjTbl[CLACT_RES_M_CHR] = GFL_CLGRP_CGR_Register( p_handle, NARC_oekaki_oekaki_m_obj_NCGR, 0, 
                                                           CLSYS_DRAW_MAIN, HEAPID_OEKAKI );

  //pal読み込み
  wk->resObjTbl[CLACT_RES_M_PLTT] = GFL_CLGRP_PLTT_Register( p_handle, NARC_oekaki_oekaki_m_NCLR, 
                                                             CLSYS_DRAW_MAIN, 0, HEAPID_OEKAKI );

  //cell読み込み
  wk->resObjTbl[CLACT_RES_M_CELL] = GFL_CLGRP_CELLANIM_Register( p_handle, 
                                                                 NARC_oekaki_oekaki_m_obj_NCER, 
                                                                 NARC_oekaki_oekaki_m_obj_NANR, 
                                                                 HEAPID_OEKAKI );

  //---------メイン画面用-------------------
  // 共通メニュー素材
  //chara読み込み
  wk->resObjTbl[CLACT_RES_SYS_CHR] = GFL_CLGRP_CGR_Register( c_handle, APP_COMMON_GetBarIconCharArcIdx(), 0, 
                                                           CLSYS_DRAW_MAIN, HEAPID_OEKAKI );

  //pal読み込み
  wk->resObjTbl[CLACT_RES_SYS_PLTT] = GFL_CLGRP_PLTT_RegisterEx( c_handle, APP_COMMON_GetBarIconPltArcIdx(), 
                                                                 CLSYS_DRAW_MAIN, 11*32, 0, 3, HEAPID_OEKAKI );

  //cell読み込み
  wk->resObjTbl[CLACT_RES_SYS_CELL] = GFL_CLGRP_CELLANIM_Register( c_handle, 
                                                                 APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K), 
                                                                 APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K), 
                                                                 HEAPID_OEKAKI );

  //---------サブ画面用-------------------

  //chara読み込み
  wk->resObjTbl[CLACT_RES_S_CHR] = GFL_CLGRP_CGR_Register( p_handle, NARC_oekaki_oekaki_m_obj_NCGR, 0, 
                                                           CLSYS_DRAW_SUB, HEAPID_OEKAKI );

  //pal読み込み
  wk->resObjTbl[CLACT_RES_S_PLTT] = GFL_CLGRP_PLTT_Register( p_handle, NARC_oekaki_oekaki_m_NCLR, 
                                                             CLSYS_DRAW_SUB, 0, HEAPID_OEKAKI );

  //cell読み込み
  wk->resObjTbl[CLACT_RES_S_CELL] = GFL_CLGRP_CELLANIM_Register( p_handle, 
                                                                 NARC_oekaki_oekaki_m_obj_NCER, 
                                                                 NARC_oekaki_oekaki_m_obj_NANR, 
                                                                 HEAPID_OEKAKI );
  // 共通素材ファイルハンドルクローズ
  GFL_ARC_CloseDataHandle( c_handle );

  

}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define END_ICON_X  ( 212 )
#define END_ICON_Y  ( 160 )


static const u16 pal_button_oam_table[][3]={
  {12     , 172, 5},    // 黒
  {12+24*1, 172, 7},    // 白
  {12+24*2, 172, 9},    // 赤
  {12+24*3, 172,11},    // 紫
  {12+24*4, 172,13},    // 青
  {12+24*5, 172,15},    // 水色
  {12+24*6, 172,17},    // 緑
  {12+24*7, 172,19},    // 黄色
  {12+24*8+20, 171,21}, // 「やめる」

  // ペン先アイコン
  {20,    9,   29, },
  {36,      9,   32, }, // 初期値で決定状態になっている
  {52,      9,   33, },
};

//------------------------------------------------------------------
/**
 * セルアクター登録
 *
 * @param   wk      OEKAKI_WORK*
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCellActor(OEKAKI_WORK *wk)
{
  int i;
  // セルアクターヘッダ作成

  {
    //登録情報格納
    GFL_CLWK_DATA add;

    add.pos_x = 0;
    add.pos_y = 0;
    add.anmseq    = 0;
    add.softpri   = 1;
    add.bgpri     = 1;

    //セルアクター表示開始

    // メイン画面用(矢印の登録）
    for(i=0;i<5;i++){
      add.pos_x = TRAINER_NAME_POS_X + i * 40;
      add.pos_y = TRAINER_NAME_POS_Y + TRAINER_NAME_POS_SPAN;
      wk->MainActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_M_CHR],
        wk->resObjTbl[CLACT_RES_M_PLTT],
        wk->resObjTbl[CLACT_RES_M_CELL],
        &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );

      GFL_CLACT_WK_SetAutoAnmFlag( wk->MainActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq(      wk->MainActWork[i], i );
      GFL_CLACT_WK_SetDrawEnable(  wk->MainActWork[i], 0 );

    }

    // メイン画面パレット・「やめる」ボタンの登録
    for(i=0;i<9+3;i++){
      add.pos_x = pal_button_oam_table[i][0];
      add.pos_y = pal_button_oam_table[i][1];
      wk->ButtonActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_M_CHR ],
        wk->resObjTbl[CLACT_RES_M_PLTT],
        wk->resObjTbl[CLACT_RES_M_CELL],
        &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );

      GFL_CLACT_WK_SetAutoAnmFlag( wk->ButtonActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[i], pal_button_oam_table[i][2] );
      if(i >= 8){
        GFL_CLACT_WK_SetBgPri( wk->ButtonActWork[i], 2 );
      }
    }

    // 黒色パレットはへこませておく
    GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[0], 6 );


    // サブ画面用(矢印の登録）
    for(i=0;i<5;i++){
      add.pos_x = TRAINER_NAME_POS_X;
      add.pos_y = (TRAINER_NAME_POS_Y+TRAINER_NAME_POS_SPAN*i)+NAMEIN_SUB_ACTOR_DISTANCE;
      wk->SubActWork[i] = GFL_CLACT_WK_Create( wk->clUnit,
        wk->resObjTbl[CLACT_RES_S_CHR],
        wk->resObjTbl[CLACT_RES_S_PLTT],
        wk->resObjTbl[CLACT_RES_S_CELL],
        &add, CLSYS_DEFREND_SUB, HEAPID_OEKAKI );


      GFL_CLACT_WK_SetAutoAnmFlag( wk->SubActWork[i], 1 );
      GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[i],      i );
      GFL_CLACT_WK_SetSoftPri( wk->SubActWork[i],     1 );  // それぞれのアクターのプライオリティ設定
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i],  0 );
      
    }

    // アプリ共通素材戻るボタンの表示
    add.pos_x = END_ICON_X;
    add.pos_y = END_ICON_Y;
    wk->EndIconActWork = GFL_CLACT_WK_Create( wk->clUnit,
                                              wk->resObjTbl[CLACT_RES_SYS_CHR],
                                              wk->resObjTbl[CLACT_RES_SYS_PLTT],
                                              wk->resObjTbl[CLACT_RES_SYS_CELL],
                                              &add, CLSYS_DEFREND_MAIN, HEAPID_OEKAKI );
    GFL_CLACT_WK_SetAutoAnmFlag( wk->EndIconActWork, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->EndIconActWork,      1 );
    GFL_CLACT_WK_SetDrawEnable( wk->EndIconActWork,  TRUE );
    
  } 

  //メイン・サブ画面OBJ面ＯＮ
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); 
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON ); 
 
  
}

// おえかきボードBMP（下画面）
#define OEKAKI_BOARD_POSX  ( 1 )
#define OEKAKI_BOARD_POSY  ( 2 )
#define OEKAKI_BOARD_W   ( 30 )
#define OEKAKI_BOARD_H   ( 15 )


// 名前表示BMP（上画面）
#define OEKAKI_NAME_BMP_W  ( 10 )
#define OEKAKI_NAME_BMP_H  (  2 )
#define OEKAKI_NAME_BMP_SIZE (OEKAKI_NAME_BMP_W * OEKAKI_NAME_BMP_H)


// 「やめる」文字列BMP（下画面）
#define OEKAKI_END_BMP_X  ( 26 )
#define OEKAKI_END_BMP_Y  ( 21 )
#define OEKAKI_END_BMP_W  ( 6  )
#define OEKAKI_END_BMP_H  ( 2  )


// 会話ウインドウ表示位置定義
#define OEKAKI_TALK_X   (  2 )
#define OEKAKI_TALK_Y   (  1 )

#define MSG_WIN_W      ( 27 )
#define MSG_WIN_H      (  4 )
//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinInit( OEKAKI_WORK *wk, GFL_PROC* proc )
{
  // ---------- メイン画面 ------------------

  // BMPWINシステム開始
  GFL_BMPWIN_Init( HEAPID_OEKAKI );

  // BG0面BMP（会話ウインドウ）ウインドウ確保
  wk->MsgWin = GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
                                  OEKAKI_TALK_X, OEKAKI_TALK_Y, 
                                  MSG_WIN_W, MSG_WIN_H, 13, GFL_BMP_CHRAREA_GET_B );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0f0f );
  
  PRINT_UTIL_Setup( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->MsgWin );

  // BG1面用BMP（お絵かき画像）ウインドウ確保
  wk->OekakiBoard = GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
                                   OEKAKI_BOARD_POSX, OEKAKI_BOARD_POSY, 
                                   OEKAKI_BOARD_W, OEKAKI_BOARD_H, 0, GFL_BMP_CHRAREA_GET_B);
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->OekakiBoard), 0x0202 );

  // メッセージ表示システム用初期化 
  wk->pMsgTcblSys = GFL_TCBL_Init( HEAPID_OEKAKI, HEAPID_OEKAKI, 32 , 32 );
  wk->printQueName = PRINTSYS_QUE_Create( HEAPID_OEKAKI );
  wk->printQue    = PRINTSYS_QUE_Create( HEAPID_OEKAKI );

  
  // ----------- サブ画面名前表示BMP確保 ------------------
  {
    int i;
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      wk->TrainerNameWin[i] = GFL_BMPWIN_Create( GFL_BG_FRAME0_S,  
                                             TRAINER_NAME_POS_X/8+2, TRAINER_NAME_POS_Y/8+i*4-1, 
                                             10, 2, 13,  GFL_BMP_CHRAREA_GET_B);
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( wk->TrainerNameWin[i] ), 0 );
      PRINT_UTIL_Setup( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->TrainerNameWin[i] );
      GFL_BMPWIN_MakeTransWindow( wk->TrainerNameWin[i] );
    }

    //最初に見えている面なので文字パネル描画と転送も行う
    NameCheckPrint( wk->TrainerNameWin, NAME_COL_NORMAL, wk );
  }
  
} 

// はい・いいえ用定義（下画面）
#define YESNO_CHARA_OFFSET  (1 + TALK_WIN_CGX_SIZ + MENU_WIN_CGX_SIZ + MSG_WIN_W*MSG_WIN_H )
#define YESNO_CHARA_W   ( 8 )
#define YESNO_CHARA_H   ( 4 )





//------------------------------------------------------------------
/**
 * @brief   確保したBMPWINを解放
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinDelete( OEKAKI_WORK *wk )
{
  int i;

  // メッセージ表示用システム解放
  GFL_TCBL_DeleteAll( wk->pMsgTcblSys );
  GFL_TCBL_Exit( wk->pMsgTcblSys );
  PRINTSYS_QUE_Clear( wk->printQue );
  PRINTSYS_QUE_Delete( wk->printQue );
  PRINTSYS_QUE_Clear( wk->printQueName );
  PRINTSYS_QUE_Delete( wk->printQueName );

  
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_BMPWIN_Delete( wk->TrainerNameWin[i] );
  }
  GFL_BMPWIN_Delete( wk->OekakiBoard );
  GFL_BMPWIN_Delete( wk->MsgWin );

  // BMPWINシステム終了
  GFL_BMPWIN_Exit();
}








//------------------------------------------------------------------
/**
 * @brief   カーソル位置を変更する
 *
 * @param   act   アクターのポインタ
 * @param   x   
 * @param   y   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCursor_Pos( GFL_CLWK *act, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y-8;
  GFL_CLACT_WK_SetWldPos( act, &pos );

}


// ---------------------------------------------------------
// 通常時タッチ検出テーブル
// ---------------------------------------------------------

#define PAL_BUTTON_X  (  0  )
#define PAL_BUTTON_Y  ( 150 )
#define PAL_BUTTON_W  ( 3*8 )
#define PAL_BUTTON_H  ( 5*8 )
#define PAL_BUTTON_RECT ( 3*8 -1 )
#define PAL_BUTTON_RECT ( 3*8 -1 )
#define END_BUTTON_RECT ( 8*8 -1 )

#define PAL_BUTTON0_X ( PAL_BUTTON_X+PAL_BUTTON_W*0 )
#define PAL_BUTTON1_X ( PAL_BUTTON_X+PAL_BUTTON_W*1 )
#define PAL_BUTTON2_X ( PAL_BUTTON_X+PAL_BUTTON_W*2 )
#define PAL_BUTTON3_X ( PAL_BUTTON_X+PAL_BUTTON_W*3 )
#define PAL_BUTTON4_X ( PAL_BUTTON_X+PAL_BUTTON_W*4 )
#define PAL_BUTTON5_X ( PAL_BUTTON_X+PAL_BUTTON_W*5 )
#define PAL_BUTTON6_X ( PAL_BUTTON_X+PAL_BUTTON_W*6 )
#define PAL_BUTTON7_X ( PAL_BUTTON_X+PAL_BUTTON_W*7 )
#define END_BUTTON_X  ( PAL_BUTTON_X+PAL_BUTTON_W*8 )

#define BRUSH_BUTTON0_X ( 14 ) // 16,5 23,12
#define BRUSH_BUTTON1_X ( 30 ) // 16,5 23,12
#define BRUSH_BUTTON2_X ( 46 ) // 16,5 23,12
#define BRUSH_BUTTON_Y  (  1 ) // 16,5 23,12
#define BRUSH_BUTTON_W  ( 12 ) // 16,5 23,12
#define BRUSH_BUTTON_H  ( 12 ) // 16,5 23,12


static const GFL_UI_TP_HITTBL sub_button_hittbl[]={
// パレット
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON0_X,PAL_BUTTON0_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON1_X,PAL_BUTTON1_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON2_X,PAL_BUTTON2_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON3_X,PAL_BUTTON3_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON4_X,PAL_BUTTON4_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON5_X,PAL_BUTTON5_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON6_X,PAL_BUTTON6_X+PAL_BUTTON_RECT},
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,PAL_BUTTON7_X,PAL_BUTTON7_X+PAL_BUTTON_RECT},

// 終了ボタン
{PAL_BUTTON_Y,PAL_BUTTON_Y+PAL_BUTTON_H-1,END_BUTTON_X,END_BUTTON_X+END_BUTTON_RECT},

// ペン先ボタン
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON0_X,BRUSH_BUTTON0_X+BRUSH_BUTTON_W},
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON1_X,BRUSH_BUTTON1_X+BRUSH_BUTTON_W},
{BRUSH_BUTTON_Y,BRUSH_BUTTON_Y+BRUSH_BUTTON_H, BRUSH_BUTTON2_X,BRUSH_BUTTON2_X+BRUSH_BUTTON_W},

{GFL_UI_TP_HIT_END,0,0,0},   // 終了データ
};


// ---------------------------------------------------------
// 離脱選択禁止中の際のYESNOボタン検出テーブル
// ---------------------------------------------------------
#define OEKAKI_YESNO_BUTTON_X ( 25 )
#define OEKAKI_YESNO_BUTTON_Y ( 6  )

#define FAKE_YESNO_X  ( OEKAKI_YESNO_BUTTON_X*8 )
#define FAKE_YESNO_Y  ( OEKAKI_YESNO_BUTTON_Y*8 )
#define FAKE_YESNO_W  ( 6*8 )
#define FAKE_YESNO_H  ( 8*8 )

static const GFL_UI_TP_HITTBL fake_yesno_hittbl[]={
  {FAKE_YESNO_Y, FAKE_YESNO_Y+FAKE_YESNO_H, FAKE_YESNO_X, FAKE_YESNO_X+FAKE_YESNO_W},
  {GFL_UI_TP_HIT_END,0,0,0},
};

// ---------------------------------------------------------
// 描画領域検出用テーブル
// ---------------------------------------------------------
#define DRAW_AREA_X (  8  )
#define DRAW_AREA_Y ( 16  )
#define DRAW_AREA_W ( OEKAKI_BOARD_W*8 )
#define DRAW_AREA_H ( OEKAKI_BOARD_H*8)

static const GFL_UI_TP_HITTBL sub_canvas_touchtbl[]={
  {DRAW_AREA_Y,DRAW_AREA_Y+DRAW_AREA_H,DRAW_AREA_X,DRAW_AREA_X+DRAW_AREA_W},
  {GFL_UI_TP_HIT_END,0,0,0},   // 終了データ
};


// 空でーたを格納
static void _set_blank_data( TOUCH_INFO *result, int color, int brush )
{
  TP_ONE_DATA tpData;

  tpData.Size = 0;
  tpData.TPDataTbl[0].x = 0;
  tpData.TPDataTbl[0].y = 0;
  SetTouchpanelData( result, &tpData, color, brush );

}


//------------------------------------------------------------------
/**
 * タッチパネルで押したボタンがへこむ
 *
 * @param   wk    OEKAKI_WORKのポインタ
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void NormalTouchFunc(OEKAKI_WORK *wk)
{
  int button=-1,touch;
  int decide = FALSE;
  
  // 入力処理

  // カラー切り替え
  button=GFL_UI_TP_HitTrg( sub_button_hittbl );
  if( button != GFL_UI_TP_HIT_NONE ){
    switch(button){
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
      // 色変更
      if(wk->brush_color!=button){
        wk->brush_color = button;
        PalButtonAppearChange( wk->ButtonActWork, button);
        PMSND_PlaySE( OEKAKI_DECIDE_SE );
      }
      break;
    case 8:
      // 「やめる」を押したらウインドウ描画開始
      if(wk->seq==OEKAKI_MODE){
        // 親機の場合は接続拒否
        if(GFL_NET_SystemGetCurrentID()==0){
          //コネクト数と表示人数が異なる場合は、やめるボタンを無反応にする
          if (wk->shareBit != _get_connect_bit(wk)){
            PMSND_PlaySE(OEKAKI_BS_SE);
            break;
          }
          wk->banFlag = OEKAKI_BAN_ON;
          // 「おえかきをやめますか？」
          EndMessagePrint( wk, msg_oekaki_02, 1 );
          SetNextSequence( wk, OEKAKI_MODE_END_SELECT );
          EndButtonAppearChange( wk->EndIconActWork, TRUE );
          decide = TRUE;
          PMSND_PlaySE(OEKAKI_CANCEL_SE);
        }else{
          if(wk->AllTouchResult[0].banFlag == OEKAKI_BAN_ON ){
            // 親機に禁止されているときはＳＥのみ
            PMSND_PlaySE(OEKAKI_BS_SE);
          }else{
            // 「おえかきをやめますか？」
            EndMessagePrint( wk, msg_oekaki_02, 1 );
            SetNextSequence( wk, OEKAKI_MODE_END_SELECT );
            EndButtonAppearChange( wk->EndIconActWork, TRUE );
            decide = TRUE;
            PMSND_PlaySE(OEKAKI_CANCEL_SE);
          }
        }

      }
      break;
    case 9: case 10: case 11:
      // ペン先変更
      {
        int i;
        for(i=0;i<3;i++){
          if((button-9)==i){
            GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[CELL_BRUSH_NO+i], 29+2*i+1 );   // ON
          }else{
            GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[CELL_BRUSH_NO+i], 29+2*i ); // OFF
          }
        }
        if(wk->brush!=(SMALL_BRUSH + button-9)){
          wk->brush = SMALL_BRUSH + button-9;
          PMSND_PlaySE(OEKAKI_PEN_CHANGE_SE);
        }
      }
      break;
    }
  }

  // カーソル位置変更
  touch = GFL_UI_TP_HitCont( sub_canvas_touchtbl );

  GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[GFL_NET_SystemGetCurrentID()], 0 );
  if(touch!=GFL_UI_TP_HIT_NONE){
    // 自分のカーソルはタッチパネルの座標を反映させる
    u32 x,y;
    GFL_UI_TP_GetPointCont( &x, &y );
    SetCursor_Pos( wk->MainActWork[GFL_NET_SystemGetCurrentID()], x, y );
    GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[GFL_NET_SystemGetCurrentID()], 1 );
  }

  // サンプリング情報を取得して格納
  {
    TP_ONE_DATA tpData;
    TOUCH_INFO  tmpResult;
    u32 x=0,y=0;
    int i;
    if(GFL_UI_TP_GetCont()==TRUE){
      GFL_UI_TP_GetPointCont( &x, &y );
      tpData.Size = 2;
      tpData.TPDataTbl[0].x = wk->oldTpData.TPDataTbl[1].x;
      tpData.TPDataTbl[0].y = wk->oldTpData.TPDataTbl[1].y;
      tpData.TPDataTbl[1].x = x;
      tpData.TPDataTbl[1].y = y;
      SetTouchpanelData( &tmpResult, &tpData, wk->brush_color, wk->brush );

      if(decide == TRUE){
        tmpResult.size = 0;
      }
    }else{
      tpData.TPDataTbl[1].x=0;
      tpData.TPDataTbl[1].y=0;
      _set_blank_data( &tmpResult, wk->brush_color, wk->brush );
    }
    // FIFOに積む
    OekakiTouchFifo_Set( &tmpResult, &wk->TouchFifo, 0 );

    // FIFOのENDポインタを進める
    if(GFL_NET_SystemGetCurrentID()==0){
      OekakiTouchFifo_AddEndParent( &wk->TouchFifo ); // 親機
    }else{
      OekakiTouchFifo_AddEnd( &wk->TouchFifo );       // 子機
    }

    // 今回のタッチデータを保存
    wk->oldTpData = tpData;
  }


}


//------------------------------------------------------------------
/**
 * @brief   パレットとやめるボタンのセルアクター制御
 *
 * @param   act   アクターのポインタ
 * @param   button    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void PalButtonAppearChange( GFL_CLWK *act[], int no )
{
  int i;
  
  for(i=0;i<8;i++){
    // 押されたボタンはへこむ。ほかのボタンは戻る
    if(i==no){
      GFL_CLACT_WK_SetAnmSeq( act[i], pal_button_oam_table[i][2]+1 );
    }else{
      GFL_CLACT_WK_SetAnmSeq( act[i], pal_button_oam_table[i][2] );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   やめるボタンのオン・オフ
 *
 * @param   act   アクターのポインタ
 * @param   flag    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndButtonAppearChange( GFL_CLWK *act, BOOL flag )
{
  if(flag==TRUE){
    GFL_CLACT_WK_SetAnmSeq( act, 9 );
  }else{
    GFL_CLACT_WK_SetAnmSeq( act, 1 );
  }
}

//------------------------------------------------------------------
/**
 * @brief   お絵かきボード通常処理
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_MainNormal( OEKAKI_WORK *wk, int seq )
{
  NormalTouchFunc(wk);      //  タッチパネル処理
  
  if(GFL_NET_SystemGetCurrentID()==0){      // 親機の時
    if(OnlyParentCheck(wk)!=1){     // 一人じゃないか？
      LineDataSendRecv( wk );
    }
  }else{
      LineDataSendRecv( wk );
  }
  MoveCommCursor( wk );
  DrawBrushLine( wk->OekakiBoard, wk->AllTouchResult, wk->OldTouch, 1 );

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   終了選択処理の共通処理関数(後ろで描画等）
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndSequenceCommonFunc( OEKAKI_WORK *wk )
{
  MoveCommCursor( wk );
  DrawBrushLine( wk->OekakiBoard, wk->AllTouchResult, wk->OldTouch, 0 );
  
}


//------------------------------------------------------------------
/**
 * @brief   新しい子機がきたので親機が画像を送信しはじめる
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int Oekaki_NewMember( OEKAKI_WORK *wk, int seq )
{
  // ●●●さんがはいってきました
  OS_Printf("newMemberは%d\n",wk->newMemberId);
  if(GFL_NET_SystemGetCurrentID()==wk->newMemberId){
    EndMessagePrint(wk, msg_oekaki_14, 0);
  }else{
    EndMessagePrint(wk, msg_oekaki_01, 0);
  }
  SetNextSequence( wk, OEKAKI_MODE_NEWMEMBER_WAIT );
  // 選択肢表示中の場合は強制的に非表示に
  OekakiResetYesNoWin(wk);

  PMSND_PlaySE(OEKAKI_NEWMEMBER_SE);

  // 画像転送状態になったら輝度ダウン
  G2_SetBlendBrightness(  GX_BLEND_PLANEMASK_BG1|
              GX_BLEND_PLANEMASK_BG2|
              GX_BLEND_PLANEMASK_BG3,  -6);

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   新しい子機用の画像受信待ち
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int Oekaki_NewMemberWait( OEKAKI_WORK *wk, int seq )
{
  int i;

  // 座標データが入っている時はカーソル座標を反映させる
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 0 );
  }
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   子機乱入受信終了
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_NewMemberEnd( OEKAKI_WORK *wk, int seq )
{
  // 輝度ダウン解除
  G2_BlendNone();

  OS_Printf("乱入人数変更01\n");
  ChangeConnectMax(wk, 1);

  SetNextSequence( wk, OEKAKI_MODE );
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理

  //乱入済み
  wk->bookJoin = 0;
  OS_Printf("乱入成功\n");
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   お絵かきボード「やめる」を選択した時
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectPutString( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){

    // はい・いいえ開始
    if(wk->app_menuwork==NULL){
      wk->app_menuwork = YesNoMenuInit( wk );
    }

    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_WAIT );
  }
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   「やめますか？」はい・いいえ選択待ち
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectWait( OEKAKI_WORK *wk, int seq )
{
  u32 result;

  // 誤送信を防ぐ
  wk->MyTouchResult.size = 0;

  // 親機から操作禁止が着ている間は子機は操作禁止
  if(wk->AllTouchResult[0].banFlag==OEKAKI_BAN_ON && GFL_NET_SystemGetCurrentID()!=0){
    EndSequenceCommonFunc( wk );    //終了選択時の共通処理
    APP_TASKMENU_ShowOnly( wk->app_menuwork );
    if(FakeEndYesNoSelect(wk)){
      // 親機に禁止されているときはＳＥのみ
      PMSND_PlaySE(OEKAKI_BS_SE);
    }

    return seq;
  }

  if(GFL_NET_SystemGetCurrentID() == 0 && wk->ridatu_bit != 0){
    EndSequenceCommonFunc( wk );    //終了選択時の共通処理
    APP_TASKMENU_ShowOnly( wk->app_menuwork );
    OS_Printf("メニュー操作禁止 ridatu_bit=%d, union_bit=%d \n", wk->ridatu_bit, _get_connect_bit(wk));
    return seq;
  }
  
  result = YesNoMenuMain( wk );
  switch(result){       //やめますか？
  case YESNO_RET_YES:
      if(GFL_NET_SystemGetCurrentID()==0){    
        SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT );
        EndMessagePrint( wk, msg_oekaki_05, 1 );    // リーダーがやめると…
      }else{
        SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT_CALL );
        BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
      }
  
      //おえかき再描画
      GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
  
    break;
  case YESNO_RET_NO:
    SetNextSequence( wk, OEKAKI_MODE );
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );
  
    //おえかき再描画
    GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
      
    // 親機は接続拒否を解除
    if(GFL_NET_SystemGetCurrentID()==0){
      ConnectLimitSet(wk,1);
      wk->banFlag = OEKAKI_BAN_OFF;
    }

    break;
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理

  return seq;
}

//----------------------------------------------------------------------------------
/**
 * @brief 親機が一旦お絵かき終了を選択したのでまずは子機に操作禁止を伝える
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int Oekaki_EndSelectParentCall( OEKAKI_WORK *wk, int seq )
{
  COMM_OEKAKI_END_CHILD_WORK coec;
  
  MI_CpuClear8(&coec, sizeof(COMM_OEKAKI_END_CHILD_WORK));
  coec.request = COEC_REQ_RIDATU_CHECK;
  coec.ridatu_id = GFL_NET_SystemGetCurrentID();
  
  if(GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(),CO_OEKAKI_END_CHILD, 
                    sizeof(COMM_OEKAKI_END_CHILD_WORK), &coec ))
  {
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_ANSWER_WAIT );
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   「やめますか？」＞「はい」、で親から離脱許可待ち
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerWait( OEKAKI_WORK *wk, int seq )
{
  // 誤送信を防ぐ
  wk->MyTouchResult.size = 0;
  
  wk->ridatu_wait = 0;
  
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   「やめますか？」＞「はい」、で親から離脱許可待ち＞OK!
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerOK( OEKAKI_WORK *wk, int seq )
{
  // 誤送信を防ぐ
  wk->MyTouchResult.size = 0;

/*
  if((wk->oya_share_num != _get_connect_num(wk))
      || (wk->oya_share_num != MyStatusGetNum(wk))){
    OS_TPrintf("share_nuM = %d, Comm = %d, My = %d, Bit = %d\n", wk->oya_share_num, _get_connect_num(wk), MyStatusGetNum(wk), _get_connect_bit(wk));
    wk->status_end = TRUE;
    wk->ridatu_wait = 0;
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_ANSWER_NG );
    EndSequenceCommonFunc( wk );    //終了選択時の共通処理
    // 本当にやめる事にしたので送信する
    Oekaki_SendDataRequest( wk, CO_OEKAKI_END_CHILD_DECIDE, GFL_NET_SystemGetCurrentID() );
    OS_Printf("wk->status_endを立てた\n");
    return seq;
  }
*/
  wk->ridatu_wait++;
  OS_TPrintf("ridatu_wait = %d\n", wk->ridatu_wait);
  if(wk->ridatu_wait > 30){
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_SEND_OK );
    wk->ridatu_wait = 0;
  }


  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}

//----------------------------------------------------------------------------------
/**
 * @brief 子機離脱を送信
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static  int Oekaki_EndSelectSendOK( OEKAKI_WORK *wk, int seq )
{
  COMM_OEKAKI_END_CHILD_WORK coec;
  
  MI_CpuClear8(&coec, sizeof(COMM_OEKAKI_END_CHILD_WORK));
  coec.request = COEC_REQ_RIDATU_EXE;
  coec.ridatu_id = GFL_NET_SystemGetCurrentID();

  if(GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CO_OEKAKI_END_CHILD, 
                    sizeof(COMM_OEKAKI_END_CHILD_WORK), &coec ))
  {
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD );
  }
  
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   「やめますか？」＞「はい」、で親から離脱許可待ち＞NG!
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectAnswerNG( OEKAKI_WORK *wk, int seq )
{
  // 誤送信を防ぐ
  wk->MyTouchResult.size = 0;

  wk->status_end = FALSE;
  SetNextSequence( wk, OEKAKI_MODE );
  EndButtonAppearChange( wk->EndIconActWork, FALSE );
  
  OS_TPrintf("==========離脱強制キャンセル！===========\n");
  
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}

// OEKAKI_MODE_END_CHILD
//------------------------------------------------------------------
/**
 * @brief   子機終了メッセージ開始
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChild( OEKAKI_WORK *wk, int seq )
{
  // おえかきをやめました
  EndMessagePrint( wk, msg_oekaki_07, 1 );  

  SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT );

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}
        

// OEKAKI_MODE_END_CHILD_WAIT
//------------------------------------------------------------------
/**
 * @brief   子機終了メッセージ表示終了待ち
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChildWait( OEKAKI_WORK *wk, int seq )
{     

  if( EndMessageWait( wk ) ){
    wk->wait = 0;
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT2 );
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   メッセージ表示終了後少し待つ
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_EndChildWait2( OEKAKI_WORK *wk, int seq )
{     

  wk->wait++;
  if( wk->wait > OEKAKI_MESSAGE_END_WAIT ){
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    seq = SEQ_OUT;            //終了シーケンスへ
  }

  EndSequenceCommonFunc( wk );      //終了選択時の共通処理
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   親機が終了するときはもう一度「リーダーがやめると…」と質問する
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectParent( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){

    // はい・いいえ開始
    if(wk->app_menuwork==NULL){
      wk->app_menuwork = YesNoMenuInit( wk );
    }
    
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT_WAIT );

  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   親機の２回目の「はい・いいえ」
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndSelectParentWait( OEKAKI_WORK *wk, int seq )
{
  u32 result = YesNoMenuMain( wk );

  if(wk->shareNum != MyStatusGetNum(wk) //一致していないなら「やめる」許可しない
      || wk->ridatu_bit != 0){  //離脱しようとしている子がいるなら許可しない
    EndSequenceCommonFunc( wk );    //終了選択時の共通処理
    return seq;
  }

  switch(result){       //やめますか？
  case YESNO_RET_YES:            //はい
    SetNextSequence( wk, OEKAKI_MODE_END_SELECT_PARENT_SEND_END );
    Union_App_Parent_EntryBlock(wk->param->uniapp);
    break;
  case YESNO_RET_NO:           //いいえ
    SetNextSequence( wk, OEKAKI_MODE );
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_OFF );

    // 親機は接続拒否を解除
    if(GFL_NET_SystemGetCurrentID()==0){
      ConnectLimitSet(wk,1);
      wk->banFlag = OEKAKI_BAN_OFF;
    }
    break;
  }


  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
  
}

//----------------------------------------------------------------------------------
/**
 * @brief MYSTATUSを通信データから取得し、WORDSETに登録する
 *        (取れなかったら登録しない。ハングするよりはマシ）
 *
 * @param   wk        OEKAKI_WORK
 * @param   word_id   WORDSET指定のバッファ
 * @param   comm_id   通信ID
 */
//----------------------------------------------------------------------------------
static void _wordset_username( OEKAKI_WORK *wk, int word_id, int comm_id )
{
  const MYSTATUS *mystatus = Union_App_GetMystatus(wk->param->uniapp, comm_id);
  if(mystatus!=NULL){
    WORDSET_RegisterPlayerName( wk->WordSet, word_id, mystatus); 
  }else{
    OS_Printf("MyStatus[%d]が取得できなかったのでWORDSET[%d]に格納しませんでした\n", 
               comm_id, word_id);
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief 親が終了を送信
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static  int Oekaki_EndSelectParentSendEnd( OEKAKI_WORK *wk, int seq )
{
  //終了通知
  if(GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), 
                    CO_OEKAKI_END, NULL, 0 ))
  {
    SetNextSequence( wk, OEKAKI_MODE_FORCE_END );
    // 親機（自分）の名前をWORDSET
    _wordset_username( wk, 0, 0 );
    seq = SEQ_LEAVE;
    OS_Printf("OEKAKI_MODE_FORCE_ENDにかきかえ\n");
  }
  
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
}


//------------------------------------------------------------------
/**
 * @brief   親機が終わりと言ったので終わる
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEnd( OEKAKI_WORK *wk, int seq )
{
  // 親機（自分）の名前をWORDSET
  _wordset_username( wk, 0, 0 );
  EndMessagePrint( wk, msg_oekaki_04, 1 );        // リーダーが抜けたので解散します。
  SetNextSequence( wk, OEKAKI_MODE_FORCE_END_WAIT );

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;

}


//------------------------------------------------------------------
/**
 * @brief   親機が終わりと言ったので終わる
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndWait( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){
    SetNextSequence( wk, OEKAKI_MODE_FORCE_END_SYNCHRONIZE );
    // 画像共有済みのメンバーとだけ通信同期を行う
    // （こうしないと、新規乱入メンバーとも同期を行ってハマってしまう）
    GFL_NET_HANDLE_TimeSyncBitStart(GFL_NET_HANDLE_GetCurrentHandle(), 
                                    OEKAKI_SYNCHRONIZE_END, WB_NET_PICTURE, 
                                    wk->shareBit);
    OS_Printf("同期開始 =%d\n",Union_App_GetMemberNetBit(wk->param->uniapp));
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
 return seq;

}

//------------------------------------------------------------------
/**
 * @brief   全員の通信同期を待つ
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndSynchronize( OEKAKI_WORK *wk, int seq )
{
  GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();
  int net_id, now_bit = 0;
  
  // 途中で電源を切った子機がいないか監視
  for(net_id = 0; net_id < 5; net_id++){
    if(GFL_NET_IsConnectMember(net_id) == TRUE){
      now_bit |= 1 << net_id;
    }
  }
  
  if(now_bit != wk->shareBit){
    OS_Printf("強制終了抜け\n");
  }
  
  // 通信同期待ち
  if(GFL_NET_HANDLE_IsTimeSync( pNet,OEKAKI_SYNCHRONIZE_END, WB_NET_PICTURE) 
  || _get_connect_num(wk) == 1 || now_bit != wk->shareBit){
    OS_Printf("終了時同期成功  seq = %d\n", seq);
    OS_Printf("コネクト人数%d\n",_get_connect_num(wk));
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_HOLEOUT, WIPE_TYPE_HOLEOUT, 
                    WIPE_FADE_BLACK, 16, 1, HEAPID_OEKAKI );
    seq = SEQ_OUT;            //終了シーケンスへ
  }
  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;
  
}

//------------------------------------------------------------------
/**
 * @brief   何もしない
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_ForceEndWaitNop( OEKAKI_WORK *wk, int seq )
{
  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   親一人になったので終了
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndParentOnly( OEKAKI_WORK *wk, int seq )
{
  EndMessagePrint( wk, msg_oekaki_06, 1 );        // リーダーが抜けたので解散します。
  SetNextSequence( wk, OEKAKI_MODE_END_PARENT_ONLY_WAIT );

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理

  return seq;
}

//------------------------------------------------------------------
/**
 * @brief   一人になった文章を表示して終了シーケンスへ
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_EndParentOnlyWait( OEKAKI_WORK *wk, int seq )
{
  if( EndMessageWait( wk ) ){
    SetNextSequence( wk, OEKAKI_MODE_END_CHILD_WAIT2 );
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   ●●●さんがかえりました
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_LogoutChildMes( OEKAKI_WORK *wk, int seq )
{
  // ●●●さんがかえりました
  EndMessagePrint( wk, msg_oekaki_03, 1 );  
  // 選択肢表示中の場合は強制的に非表示に
  OekakiResetYesNoWin(wk);
  
  SetNextSequence( wk, OEKAKI_MODE_LOGOUT_CHILD_WAIT );
  PMSND_PlaySE(OEKAKI_NEWMEMBER_SE);

  // 接続可能人数を一旦現在の接続人数に落とす
  if(GFL_NET_SystemGetCurrentID()==0){
    OS_Printf("乱入人数変更02\n");
    ChangeConnectMax( wk, 0 );
  }
  wk->err_num = _get_connect_num(wk);

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理

  return seq;
}

#define OEKAKI_RIDATU_FUNC

//------------------------------------------------------------------
/**
 * @brief   子機か抜けた文章終了待ち
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int Oekaki_LogoutChildMesWait( OEKAKI_WORK *wk, int seq )
{
  // 接続人数が１減るかチェック
#ifdef OEKAKI_RIDATU_FUNC
  if((wk->err_num != 0 && _get_connect_num(wk) != wk->err_num) ||   // 離脱した子機が減る
     (wk->ridatu_bit & _get_connect_bit(wk))==0){                   // 離脱した子機が既にいなくなっている
#else
  if(wk->err_num != 0 && _get_connect_num(wk) != wk->err_num){
#endif
    wk->err_num = 0;
  }

  if( EndMessageWait( wk ) ){
    SetNextSequence( wk, OEKAKI_MODE_LOGOUT_CHILD_CLOSE );
    wk->wait = 0;
  }

  EndSequenceCommonFunc( wk );    //終了選択時の共通処理
  return seq;

}

//------------------------------------------------------------------
/**
 * @brief   表示終了ウェイト
 *
 * @param   wk    
 * @param   seq   
 *
 * @retval  int       
 */
//------------------------------------------------------------------
static int  Oekaki_LogoutChildClose( OEKAKI_WORK *wk, int seq )
{     
  // 接続人数が１減るまでは待つ
#ifdef OEKAKI_RIDATU_FUNC
  if((wk->err_num != 0 && _get_connect_num(wk) != wk->err_num) || 
     (wk->ridatu_bit & _get_connect_bit(wk))==0){
#else
  if(wk->err_num != 0 && _get_connect_num(wk) != wk->err_num){
#endif
    wk->err_num = 0;
  }

  wk->wait++;
  if( wk->wait > OEKAKI_MESSAGE_END_WAIT && wk->err_num == 0 ){
    EndMessageWindowOff( wk );
    SetNextSequence( wk, OEKAKI_MODE );
    if(GFL_NET_SystemGetCurrentID() == 0){
      wk->banFlag = OEKAKI_BAN_OFF;
      OS_Printf("乱入人数変更03\n");
      ChangeConnectMax(wk, 1);
    }
    // 絵を共有した接続人数を更新
    wk->shareNum = Union_App_GetMemberNum( wk->param->uniapp );
    wk->shareBit = Union_App_GetMemberNetBit(wk->param->uniapp);

  }else{
    OS_Printf("wait=%d, err_num=%d, connect_num=%d ridatu=%d\n", 
              wk->wait, wk->err_num, _get_connect_num(wk), wk->ridatu_bit);
  }

//  OS_Printf("err_num=%d, connect_num=%d wait=%d \n ", wk->err_num, _get_connect_num(wk), wk->wait);

  EndSequenceCommonFunc( wk );      //終了選択時の共通処理
  return seq;
  
}





//==============================================================================
/**
 * @brief   どんな状態であっても強制的にメインシーケンスチェンジ
 *
 * @param   wk    
 * @param   seq   
 * @param   id    通信ID
 *
 * @retval  none    
 */
//==============================================================================
void OekakiBoard_MainSeqForceChange( OEKAKI_WORK *wk, int seq, u8 id  )
{
  const MYSTATUS *mystatus=NULL;
  switch(seq){
  case OEKAKI_MODE_NEWMEMBER: 
    // 既に離脱中ならシーケンス変更は行わない
    if(wk->status_end == TRUE){
      return;
    }
    // 「はい・いいえ」表示中だったらクリアする
    if(wk->seq==OEKAKI_MODE_END_SELECT_WAIT || wk->seq==OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      OekakiResetYesNoWin(wk);
    }
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    // 指定の子機の名前をWORDSETに登録（離脱・乱入時)
    _wordset_username( wk, 0, id );
    wk->newMemberId = id;
    wk->ridatu_bit = 0;
    OS_Printf("新しい人のID %d\n",id);
    break;
  case OEKAKI_MODE_NEWMEMBER_END:
    EndMessageWindowOff( wk );
    break;
  case OEKAKI_MODE_LOGOUT_CHILD:
    if(wk->status_end == TRUE){
      return; //自分自身が離脱処理中
    }
    // 離脱する子機の名前をWORDSETに登録
    _wordset_username( wk, 0, id );
    if(id==GFL_NET_SystemGetCurrentID()){
      // 自分が離脱する子機だった場合は「子機がいなくなたよ」とは言わない
      return;
    }

    if(GFL_NET_SystemGetCurrentID() == 0){
      wk->ridatu_bit &= 0xffff ^ id;
    }
    if(wk->seq==OEKAKI_MODE_END_SELECT_WAIT || wk->seq==OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      OekakiResetYesNoWin(wk);

      //おえかき再描画
      GFL_BMPWIN_MakeTransWindow( wk->OekakiBoard );
    }
    EndButtonAppearChange( wk->EndIconActWork, FALSE );
    break;
  case OEKAKI_MODE_FORCE_END:
    break;
  case OEKAKI_MODE_END_SELECT_ANSWER_OK:
    Oekaki_SendDataRequest( wk, CO_OEKAKI_END_CHILD_DECIDE, GFL_NET_SystemGetCurrentID() );
    break;
  case OEKAKI_MODE_END_SELECT_ANSWER_NG:
    break;
  default:
    GF_ASSERT( "指定以外のシーケンスチェンジが来た" );
    return;
  }
  SetNextSequence( wk, seq );

}

//==============================================================================
/**
 * @brief   何か処理をしてからメインループ内シーケンスチェンジ(通常状態であること）
 *
 * @param   wk    
 * @param   seq   このメインシーケンスにチェンジしたい
 *
 * @retval  none    
 */
//==============================================================================
void OekakiBoard_MainSeqCheckChange( OEKAKI_WORK *wk, int seq, u8 id  )
{
  // 通常状態なら
  if(wk->seq == OEKAKI_MODE){
    OS_Printf("子機%dの名前を登録\n",id);
    switch(seq){
    case OEKAKI_MODE_LOGOUT_CHILD:
      WORDSET_RegisterPlayerName( wk->WordSet, 0, Union_App_GetMystatus(wk->param->uniapp, id) );  
      if(id==GFL_NET_SystemGetCurrentID()){
        // 自分が離脱する子機だった場合は「子機がいなくなたよ」とは言わない
        return;
      }
      if(GFL_NET_SystemGetCurrentID() == 0){
        wk->ridatu_bit &= 0xffff ^ id;
      }
      SetNextSequence( wk, seq );
      break;
    
    default:
      GF_ASSERT( 0&&"指定以外のシーケンスチェンジが来た" );
      return;
    }
  }

}



//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
//11520
//---------------------------------------------------------
// ブラシパターン
//---------------------------------------------------------

// どうしてもパレットデータを3bitに縮めてしまいたいので、透明色を8にして
// 色指定自体は0-7に当てている。0は透明色だがBD面を黒にして黒く見えるようにしている

// BMPデータは最低横8dot分必要なので、4x4のドットデータを作りたい時は
// ２バイトごとに参照されないデータがもう２バイト必要
static const u8 oekaki_brush[3][8][24]={

{ /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x11,  0x00,0x00,  0x00,0x11, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x22,  0x00,0x00,  0x00,0x22, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x33,  0x00,0x00,  0x00,0x33, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x44,  0x00,0x00,  0x00,0x44, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x55,  0x00,0x00,  0x00,0x55, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x66,  0x00,0x00,  0x00,0x66, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x77,  0x00,0x00,  0x00,0x77, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,  0x00,0x88,  0x00,0x00,  0x00,0x88, 0x00,0x00,  0x00,0x00, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
},
{ /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x00,0x00, 0x00,0x00, 0x00,0x11, 0x00,0x00,  0x10,0x11,  0x01,0x00,  0x10,0x11, 0x01,0x00,  0x00,0x11, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x22, 0x00,0x00,  0x20,0x22,  0x02,0x00,  0x20,0x22, 0x02,0x00,  0x00,0x22, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x33, 0x00,0x00,  0x30,0x33,  0x03,0x00,  0x30,0x33, 0x03,0x00,  0x00,0x33, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x44, 0x00,0x00,  0x40,0x44,  0x04,0x00,  0x40,0x44, 0x04,0x00,  0x00,0x44, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x55, 0x00,0x00,  0x50,0x55,  0x05,0x00,  0x50,0x55, 0x05,0x00,  0x00,0x55, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x66, 0x00,0x00,  0x60,0x66,  0x06,0x00,  0x60,0x66, 0x06,0x00,  0x00,0x66, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x77, 0x00,0x00,  0x70,0x77,  0x07,0x00,  0x70,0x77, 0x07,0x00,  0x00,0x77, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
  {0x00,0x00, 0x00,0x00, 0x00,0x88, 0x00,0x00,  0x80,0x88,  0x08,0x00,  0x80,0x88, 0x08,0x00,  0x00,0x88, 0x00,0x00, 0x00,0x00, 0x00,0x00,},
},                                                                     
{   /*  0  */              /*  1  */               /*  2  */              /*  3  */
  {0x10,0x11, 0x01,0x00, 0x11,0x11, 0x11,0x00,  0x11,0x11,  0x11,0x00,  0x11,0x11, 0x11,0x00,  0x11,0x11, 0x11,0x00, 0x10,0x11, 0x01,0x00,},
  {0x20,0x22, 0x02,0x00, 0x22,0x22, 0x22,0x00,  0x22,0x22,  0x22,0x00,  0x22,0x22, 0x22,0x00,  0x22,0x22, 0x22,0x00, 0x20,0x22, 0x02,0x00,},
  {0x30,0x33, 0x03,0x00, 0x33,0x33, 0x33,0x00,  0x33,0x33,  0x33,0x00,  0x33,0x33, 0x33,0x00,  0x33,0x33, 0x33,0x00, 0x30,0x33, 0x03,0x00,},
  {0x40,0x44, 0x04,0x00, 0x44,0x44, 0x44,0x00,  0x44,0x44,  0x44,0x00,  0x44,0x44, 0x44,0x00,  0x44,0x44, 0x44,0x00, 0x40,0x44, 0x04,0x00,},
  {0x50,0x55, 0x05,0x00, 0x55,0x55, 0x55,0x00,  0x55,0x55,  0x55,0x00,  0x55,0x55, 0x55,0x00,  0x55,0x55, 0x55,0x00, 0x50,0x55, 0x05,0x00,},
  {0x60,0x66, 0x06,0x00, 0x66,0x66, 0x66,0x00,  0x66,0x66,  0x66,0x00,  0x66,0x66, 0x66,0x00,  0x66,0x66, 0x66,0x00, 0x60,0x66, 0x06,0x00,},
  {0x70,0x77, 0x07,0x00, 0x77,0x77, 0x77,0x00,  0x77,0x77,  0x77,0x00,  0x77,0x77, 0x77,0x00,  0x77,0x77, 0x77,0x00, 0x70,0x77, 0x07,0x00,},
  {0x80,0x88, 0x08,0x00, 0x88,0x88, 0x88,0x00,  0x88,0x88,  0x88,0x00,  0x88,0x88, 0x88,0x00,  0x88,0x88, 0x88,0x00, 0x80,0x88, 0x08,0x00,},
},
};

/*


















*/



//==============================================================================
/**
 * @brief   描画開始位置がマイナス方向にあっても描画できるBmpWinPrintラッパー
 * @retval  none    
 */
//==============================================================================
static void _BmpWinPrint_Rap(
      GFL_BMPWIN * win, void * src,
      int src_x, int src_y, int src_dx, int src_dy,
      int win_x, int win_y, int win_dx, int win_dy )
{
  GFL_BMP_DATA *src_win;
  // X描画開始ポイントがマイナスか
  if(win_x < 0){
    int diff;
    diff = win_x*-1;
    if(diff>win_dx){  // そのマイナス分は転送幅を超えてないか
      diff = win_dx;  // 超えてたら転送幅と同じにする
    }

    // 転送参照開始ポイントと、転送幅を引く
    win_x   = 0;
    src_x  += diff;
    src_dx -= diff;
    win_dx -= diff;
  }

  // Y描画開始ポイントがマイナスか
  if(win_y < 0){
    int diff;
    diff = win_y*-1;
    if(diff>win_dy){  // そのマイナス分は転送幅を超えてないか
      diff = win_dy;  // 超えてたら転送幅と同じにする
    }

    // 転送参照開始ポイントと、転送幅を引く
    win_y   = 0;
    src_y  += diff;
    src_dy -= diff;
    win_dy -= diff;
  }

//  GFL_BMPWINPrint( win, src, src_x, src_y, src_dx, src_dy, win_x, win_y, win_dx, win_dy );
  src_win =  GFL_BMP_CreateWithData( src, 8, 8, GFL_BMP_16_COLOR, HEAPID_OEKAKI );
  GFL_BMP_Print( src_win, GFL_BMPWIN_GetBmp(win), src_x, src_y, win_x, win_y, src_dx, src_dy, 0 );
  GFL_BMP_Delete( src_win );
}


#define POINT_W ( 6 )
#define POINT_H ( 6 )
//------------------------------------------------------------------
/**
 * @brief   ライン描画
 *
 * @param   win   
 * @param   brush   
 * @param   px    
 * @param   py    
 * @param   sx    
 * @param   sy    
 * @param   count   
 * @param   flag    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void DrawPoint_to_Line( 
  GFL_BMPWIN *win, 
  const u8 *brush, 
  int px, int py, int *sx, int *sy, 
  int count, int flag )
{
  int dx, dy, s, step;
  int x1 = *sx;
  int y1 = *sy;
  int x2 = px;
  int y2 = py;

  // 初回は原点保存のみ
  if(count==0 && flag == 0){
    *sx = px;   *sy = py;
    return;
  }
  

  dx = MATH_IAbs(x2 - x1);  dy = MATH_IAbs(y2 - y1);
  if (dx > dy) {
    if (x1 > x2) {
      step = (y1 > y2) ? 1 : -1;
      s = x1;  x1 = x2;  x2 = s;  y1 = y2;
    } else step = (y1 < y2) ? 1: -1;
    _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    s = dx >> 1;
    while (++x1 <= x2) {
      if ((s -= dy) < 0) {
        s += dx;  y1 += step;
      };
      _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    }
  } else {
    if (y1 > y2) {
      step = (x1 > x2) ? 1 : -1;
      s = y1;  y1 = y2;  y2 = s;  x1 = x2;
    } else step = (x1 < x2) ? 1 : -1;
    _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    s = dy >> 1;
    while (++y1 <= y2) {
      if ((s -= dx) < 0) {
        s += dy;  x1 += step;
      }
      _BmpWinPrint_Rap( win, (void*)brush,  0, 0, POINT_W, POINT_H, x1, y1, POINT_W, POINT_H );
    }
  }
  
  
  *sx = px;     *sy = py;

}

//----------------------------------------------------------------------------------
/**
 * @brief 描画終了後のタッチ座標を保存用ワークに格納する
 *
 * @param   all   
 * @param   stock 
 */
//----------------------------------------------------------------------------------
static void Stock_OldTouch( TOUCH_INFO *all, OLD_TOUCH_INFO *stock )
{
  int i;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    stock[i].size = all[i].size;
    if(all[i].size!=0){
      stock[i].x = all[i].x[all[i].size-1];
      stock[i].y = all[i].y[all[i].size-1];
    }
  }
}

// タッチされた座標をセンタリングしている感覚にするためにズラす値
#define OEKAKI_PEN_OFFSET_X ( 9 )
#define OEKAKI_PEN_OFFSET_Y ( 17 )

//------------------------------------------------------------------
/**
 * @brief  通信で受信したタッチパネルの結果データを下に描画する
 *
 * @param   win   
 * @param   all   
 * @param   draw  メモリ上で行ったCGX変更を転送するか？(0:しない  1:する）
 *
 * @retval  none    
 *
 * WBで通信方式が変わったため、大幅な変更を行う。
 * 通信データはDPでは頂点データを送信して、前回の頂点との線を引いていたが、
 * 今回のお絵かきは送信データは必ず「線分」を送信する。なので2頂点。
 * 両方に有効な頂点を持たないデータは描画しない
 */
//------------------------------------------------------------------
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw )
{
  int px,py,i,flag=0, sx, sy;

//  OS_Printf("id0=%d,id1=%d,id2=%d,id3=%d,id4=%d\n",all[0].size,all[1].size,all[2].size,all[3].size,all[4].size);

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(all[i].size==2){
        sx = all[i].x[0] - OEKAKI_PEN_OFFSET_X;
        sy = all[i].y[0] - OEKAKI_PEN_OFFSET_Y;
        px = all[i].x[1] - OEKAKI_PEN_OFFSET_X;
        py = all[i].y[1] - OEKAKI_PEN_OFFSET_Y;
        // BG1面用BMP（お絵かき画像）ウインドウ確保
        DrawPoint_to_Line(win, oekaki_brush[all[i].brush][all[i].color], px, py, &sx, &sy, 1, old[i].size);
        flag = 1;
    }
  }
  if(flag && draw){
    GFL_BMPWIN_MakeTransWindow( win );
  }
  
  // 今回の最終座標のバックアップを取る   
  Stock_OldTouch(all, old);
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    all[i].size = 0;    // 一度描画したら座標情報は捨てる
  }
  
}

#if 0
static void DrawBrushLine( GFL_BMPWIN *win, TOUCH_INFO *all, OLD_TOUCH_INFO *old, int draw )
{
  int px,py,i,r,flag=0, sx, sy;

//  OS_Printf("id0=%d,id1=%d,id2=%d,id3=%d,id4=%d\n",all[0].size,all[1].size,all[2].size,all[3].size,all[4].size);

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(all[i].size!=0){
      if(old[i].size){
        sx = old[i].x-OEKAKI_PEN_OFFSET_X;
        sy = old[i].y-OEKAKI_PEN_OFFSET_Y;
      }
      for(r=0;r<all[i].size;r++){
        px = all[i].x[r] - OEKAKI_PEN_OFFSET_X;
        py = all[i].y[r] - OEKAKI_PEN_OFFSET_Y;
        // BG1面用BMP（お絵かき画像）ウインドウ確保
        DrawPoint_to_Line(win, oekaki_brush[all[i].brush][all[i].color], px, py, &sx, &sy, r, old[i].size);
        flag = 1;
      }
    }
  }
  if(flag && draw){
    GFL_BMPWIN_MakeTransWindow( win );
  }
  
  // 今回の最終座標のバックアップを取る   
  Stock_OldTouch(all, old);
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    all[i].size = 0;    // 一度描画したら座標情報は捨てる
  }
  
}
#endif

//------------------------------------------------------------------
/**
 * @brief   通信データからカーソル位置を移動させる
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void MoveCommCursor( OEKAKI_WORK *wk )
{
  int i;
  TOUCH_INFO *all = wk->AllTouchResult;
  
  // 座標データが入っている時はカーソル座標を反映させる
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(i!=GFL_NET_SystemGetCurrentID()){    // 自分のカーソルはタッチパネルから直接とる
      if(all[i].size!=0 && Union_App_GetMystatus(wk->param->uniapp,i)!=NULL){
        GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 1 );
        SetCursor_Pos( wk->MainActWork[i], all[i].x[all[i].size-1],  all[i].y[all[i].size-1]);
      }else{
        GFL_CLACT_WK_SetDrawEnable( wk->MainActWork[i], 0 );
      }
    }
  }
  
}
//------------------------------------------------------------------
/**
 * @brief   デバッグ用に自分で取得した情報を受信バッファにコピーする
 *
 * @param   wk    
 *
 * @retval  static    
 */
//------------------------------------------------------------------
static void DebugTouchDataTrans( OEKAKI_WORK *wk )
{
  wk->AllTouchResult[0] = wk->MyTouchResult;
}


//------------------------------------------------------------------
/**
 * カーソルのパレット変更（点滅）
 *
 * @param   CursorCol sinに渡すパラメータ（360まで）
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void CursorColTrans(u16 *CursorCol)
{
  fx32  sin;
  GXRgb tmp;
  int   r,g,b;

  *CursorCol+=20;
  if(*CursorCol>360){
    *CursorCol = 0;
  }

  sin = GFL_CALC_Sin360R(*CursorCol);
  g   = 15 +( sin * 10 ) / FX32_ONE;
  tmp = GX_RGB(29,g,0);


  GX_LoadOBJPltt((u16*)&tmp, ( 12 )*2, 2);
}

//------------------------------------------------------------------
/**
 * @brief   現在何人接続しているか？
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int OnlyParentCheck( OEKAKI_WORK *wk )
{
  int i,result;
  const MYSTATUS *status;

  result = 0;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    status = Union_App_GetMystatus(wk->param->uniapp,i);
    if(status!=NULL){
      result++;
    }
  }

  return result;
}


//------------------------------------------------------------------
/**
 * @brief   オンライン状況を確認して上画面に名前を表示する
 *
 * @param   win   
 * @param   frame   
 * @param   color   
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void NameCheckPrint( GFL_BMPWIN *win[], PRINTSYS_LSB color, OEKAKI_WORK *wk )
{
  int i,id = GFL_NET_SystemGetCurrentID();
  int num;

  // 名前取得の状況に変化が無い場合は書き換えない
  if(!MyStatusCheck(wk)){
    return;
  }

  // ログイン状況に応じて上画面の矢印をON・OFFする
  for(i=0;i<5;i++){
    if(wk->TrainerStatus[i][0]==NULL){
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i], 0 );
    }else{
      GFL_CLACT_WK_SetDrawEnable( wk->SubActWork[i], 1 );
    }
  }

  // 上画面名前リストのプリントキューをクリア
  PRINTSYS_QUE_Clear( wk->printQueName );

  // それぞれの文字パネルの背景色でクリア
  for(i=0;i<5;i++){
    GFL_BMP_Fill( GFL_BMPWIN_GetBmp(win[i]), 0,0,OEKAKI_NAME_BMP_W*8, OEKAKI_NAME_BMP_H*8, 0 );
  }

  // 描画
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->TrainerStatus[i][0]!=NULL){
      MyStatus_CopyNameString( wk->TrainerStatus[i][0], wk->TrainerName[i] );
      OS_Printf("name print id=%d\n", i);
      if(id==i){
          PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->printQueName, 
                                  0, 0, wk->TrainerName[i], wk->font, NAME_COL_MINE );

      }else{
          PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_NAME_WIN0+i], wk->printQueName, 
                                  0, 0, wk->TrainerName[i], wk->font, color );

      }
    }
    GFL_BMPWIN_MakeTransWindow( win[i] );
  }
  OS_Printf("名前かきかえしますよ\n");

}









#define PLATE_CHARA_OFFSET1 ( 12 )
#define PLATE_CHARA_OFFSET2 ( 16 )

static const u8 plate_num[4]={2,2,3,2};
static const u8 plate_table[4][3]={
  {8*8,17*8,0,},
  {8*8,17*8,0,},
  {5*8,12*8,18*8,},
  {8*8,17*8,0,},

};
static const u8 plate_chara_no[][5]={
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
  {PLATE_CHARA_OFFSET1,PLATE_CHARA_OFFSET1,},
};

//------------------------------------------------------------------
/**
 * @brief   接続が確認された場合は名前を取得する
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static int ConnectCheck( OEKAKI_WORK *wk )
{
  int i,result=0;
  const MYSTATUS *status;
  STRCODE  *namecode;

  // 接続チェック
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->ConnectCheck[i][0] = wk->ConnectCheck[i][1];

    if(Union_App_GetMemberNetBit(wk->param->uniapp)&(1<<i)){
      wk->ConnectCheck[i][0] = 1;
    }
//    wk->ConnectCheck[i][0] = CommIsConnect(i);

  }

  // 接続がいたら名前を反映させる
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->ConnectCheck[i][0]){       // 接続しているか？

      status = Union_App_GetMystatus(wk->param->uniapp,i);
      if(status!=NULL){         // MYSTATUSは取得できているか？
        namecode = (STRCODE*)MyStatus_GetMyName(status);
        GFL_STR_SetStringCode( wk->TrainerName[i], namecode );
      }

    }
  }
  return 0;
}

static int _get_fifo_diff( TOUCH_FIFO *touchFifo )
{
  if(touchFifo->end>=touchFifo->start){
    return touchFifo->end-touchFifo->start;
  }
  
  return OEKAKI_FIFO_MAX-touchFifo->start-1 + touchFifo->end;
}
//------------------------------------------------------------------
/**
 * @brief   タッチパネル情報の送受信を行う
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void LineDataSendRecv( OEKAKI_WORK *wk )
{
  int ret;


  // 親機か子機か
  if( GFL_NET_SystemGetCurrentID()==0 ){
    GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();

    // 親機は自分のタッチパネル情報を追加して送信する
    if(GFL_NET_IsEmptySendData(pNet)){  // パケットが空いてるなら
      // FIFOから親機用の頂点情報を取り出す
      OekakiTouchFifo_GetParent( wk->ParentTouchResult, &wk->TouchFifo );

      wk->ParentTouchResult[0].banFlag = wk->banFlag;
      ret=GFL_NET_SendData( pNet, CO_OEKAKI_LINEPOS_SERVER, 
                        COMM_SEND_5TH_PACKET_MAX*OEKAKI_MEMBER_MAX, wk->ParentTouchResult );
      if(ret==TRUE){
        // 送信成功であればFIFOを進める
        OekakiTouchFifo_AddStart( &wk->TouchFifo );
//        MORI_Printf("fifo start=%d, end=%d diff=%d\n", 
//                    wk->TouchFifo.start, wk->TouchFifo.end,_get_fifo_diff(&wk->TouchFifo));
      }else{
//        MORI_Printf("親機送信失敗  x=%03d, y=%03d\n", wk->MyTouchResult.x[0], wk->MyTouchResult.y[0]);
      }
    }else{
//      MORI_Printf("送信バッファに積むことができない\n");
    }
  }else{

    GFL_NETHANDLE *pNet = GFL_NET_HANDLE_GetCurrentHandle();

    // 子機用の頂点情報をFIFOから取り出す
    OekakiTouchFifo_Get( &wk->MyTouchResult, &wk->TouchFifo );
    if(GFL_NET_IsEmptySendData(pNet)){
      // 子機は自分のタッチパネル情報を親機に送信する
      ret=GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), CO_OEKAKI_LINEPOS, 
                        COMM_SEND_5TH_PACKET_MAX, &wk->MyTouchResult );
      if(ret==TRUE){
        // 送信成功であればFIFOを進める
        OekakiTouchFifo_AddStart( &wk->TouchFifo );
//        MORI_Printf("fifo start=%d, end=%d diff=%d\n", 
//                    wk->TouchFifo.start, wk->TouchFifo.end,_get_fifo_diff(&wk->TouchFifo));
      }else{
//        MORI_Printf("子機送信失敗 x=%03d, y=%03d\n", wk->MyTouchResult.x[0], wk->MyTouchResult.y[0]);
      }
    }else{
//      MORI_Printf("送信バッファに積むことができない\n");
    }
  }

}


//------------------------------------------------------------------
/**
 * @brief   現在のオンライン数を取得
 *
 * @param   none    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int MyStatusGetNum( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNum( wk->param->uniapp );

}


//------------------------------------------------------------------
/**
 * @brief   MYSTATUSの取得状況に変化があったか？
 *
 * @param   wk
 *
 * @retval  int   あったら1,  無い場合は0
 */
//------------------------------------------------------------------
static int MyStatusCheck( OEKAKI_WORK *wk )
{
  int i,result=0;
  // 接続がいたら名前を反映させる
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    wk->TrainerStatus[i][1] = wk->TrainerStatus[i][0];
    wk->TrainerStatus[i][0] = Union_App_GetMystatus(wk->param->uniapp,i);
  }

  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(wk->TrainerStatus[i][1] != wk->TrainerStatus[i][0]){
      result = 1;
    }
  }

  return result;
}


//------------------------------------------------------------------
/**
 * @brief   会話ウインドウ表示
 *
 * @param   wk    
 *
 * @retval  none  
 */
//------------------------------------------------------------------
static void EndMessagePrint( OEKAKI_WORK *wk, int msgno, int wait )
{
  // 文字列取得
  STRBUF *tempbuf;
  
  tempbuf = GFL_STR_CreateBuffer(TALK_MESSAGE_BUF_NUM,HEAPID_OEKAKI);
  GFL_MSG_GetString(  wk->MsgManager, msgno, tempbuf );
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
  GFL_STR_DeleteBuffer(tempbuf);

  // 会話ウインドウ枠描画
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, 1, MESFRAME_PAL );


  // printStreamする瞬間に他の描画を積んでいるようであればそのタスクを消す（後勝ち）
  if(wk->printStream!=NULL){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    wk->printStream=NULL;
  }

  // メッセージスピードを指定
  if(wait==0){
    // 一括描画
    PRINT_UTIL_PrintColor( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->printQue, 
                           0, 0, wk->TalkString, wk->font, STRING_COL_MSG );
  }else{
    // 文字列描画タスク登録
    wk->printStream = PRINTSYS_PrintStream( wk->MsgWin, 0, 0, wk->TalkString, wk->font,
                                            MSGSPEED_GetWait(), wk->pMsgTcblSys, 
                                            1, HEAPID_OEKAKI, 0x0f0f );
  }

  // 文字列描画開始
//  wk->MsgIndex = GF_STR_PrintSimple( &wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, wait, NULL);
  GFL_BMPWIN_MakeTransWindow( wk->MsgWin );
}

//------------------------------------------------------------------
/**
 * @brief   会話表示ウインドウ終了待ち
 *
 * @param   msg_index   
 *
 * @retval  int   0:表示中  1:終了
 */
//------------------------------------------------------------------
static int EndMessageWait( OEKAKI_WORK *wk )
{
  PRINTSTREAM_STATE state;
  state = PRINTSYS_PrintStreamGetState( wk->printStream );
  if(state==PRINTSTREAM_STATE_PAUSE){
    if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
      PRINTSYS_PrintStreamReleasePause( wk->printStream );
    }
    return 0;
  }else if(state==PRINTSTREAM_STATE_DONE){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    wk->printStream = NULL;
    return 1;
  }

  // 強制的にprintStreamを消されたようであればシーケンスを次に進めるために１に
  if(wk->printStream==NULL){
    return 1;
  }
  
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void EndMessageWindowOff( OEKAKI_WORK *wk )
{
  BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
}


//------------------------------------------------------------------
/**
 * @brief   接続人数がその時接続人数＋１になるように再設定する
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void ChangeConnectMax( OEKAKI_WORK *wk, int plus )
{
  if(GFL_NET_SystemGetCurrentID()==0){
    int num = _get_connect_num(wk)+plus;
    if(num>5){
      num = 5;
    }
    ConnectLimitSet(wk,plus);

//    OS_Printf("接続人数を %d人に変更\n",num);
  }

}

//------------------------------------------------------------------
/**
 * @brief  接続人数を監視して制御を変える 
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int ConnectNumControl( OEKAKI_WORK *wk )
{
  int num;
  // ビーコンを書き換える
  num = MyStatusGetNum(wk);

  switch(num){
  case 1:
    if(wk->seq<=OEKAKI_MODE_END_SELECT_PARENT_WAIT){
      if (_get_connect_bit(wk) != 1){
        OS_Printf("一人ではなくなりました。\n");
        OS_Printf("bit:%d\n",_get_connect_bit(wk));
        wk->ireagalJoin = 1;
      }
      OS_Printf("OEKAKI_MODE_END_PARENT_ONLYにかきかえ\n");
//      wk->seq = OEKAKI_MODE_END_PARENT_ONLY;
      wk->next_seq = OEKAKI_MODE_END_PARENT_ONLY; //予約
      OS_Printf("接続制限を１にする");
      // やめる選択中だったら強制リセット
      if(wk->yesno_flag){
        OekakiResetYesNoWin(wk);
        GFL_CLACT_WK_SetAnmSeq( wk->ButtonActWork[8], pal_button_oam_table[8][2] );
      }
      return SEQ_LEAVE;
    }
    break;
  case 2:case 3:case 4:
    // まだ入れるよ
    // 接続人数が減った場合は接続最大人数も減らす
    if(num<wk->connectBackup){
      if(wk->banFlag==OEKAKI_BAN_ON){
        OS_Printf("乱入人数変更04\n");
        ChangeConnectMax( wk, 0 );
      }else{
        // 人は減ったけど今新しい子機にデータ送信しているんだったら乱入OKにしない
        if(wk->bookJoin==0){
          OS_Printf("乱入人数変更05\n");
          ChangeConnectMax( wk, 1 );
        }else{
          OS_Printf("乱入人数変更06\n");
          ChangeConnectMax( wk, 0 );
          OS_Printf("乱入子機に送信中なのでOKにしない\n");
        }
      }
    }
    break;
  case 5:
    // 満員です。
    break;
  }

  // 画像共有人数が減ったときは更新
  if(num<wk->connectBackup){
    wk->shareNum = _get_connect_num(wk);
    wk->shareBit = _get_connect_bit(wk);
    OS_TPrintf("接続人数が減ったのでshareNumを%d人に変更 bit=%x\n", _get_connect_num(wk), wk->shareBit);
    /*乱入中フラグ成立している状態で、人数が減った*/
    if(wk->bookJoin){ /*乱入待機ビットと比較し、それが落ちていた場合は、乱入者が電源を切ったとみなす*/
      if (!(wk->shareBit&wk->joinBit)){
        //リミット制限再設定
        OS_Printf("乱入人数変更07\n");
        ChangeConnectMax( wk, 1 );
        //離脱解除
        wk->banFlag = OEKAKI_BAN_OFF;
        //乱入予約キャンセル
        wk->bookJoin = 0;
        wk->joinBit = 0;
        OS_Printf("乱入キャンセルされました\n");
      }
    }
  }

  // 接続人数を保存
  wk->connectBackup = MyStatusGetNum(wk);

  // 通信接続人数が画像共有人数よりも多くなった場合は離脱禁止フラグを立てる
  if(wk->shareNum < _get_connect_num(wk)){
//    OS_Printf("ban_flag_on:%d,%d\n",wk->shareNum, _get_connect_num(wk));
//    OS_Printf("乱入待機\n");
    wk->banFlag = OEKAKI_BAN_ON;
    //乱入を期待する
    wk->bookJoin = 1;
    //乱入しようとしている人のビットを取得
    wk->joinBit = wk->shareBit^_get_connect_bit(wk);
    
  }

  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   次に移行させたいシーケンス番号を入れる
 *
 * @param   wk    
 * @param   nextSequence    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetNextSequence( OEKAKI_WORK *wk, int nextSequence )
{
  wk->next_seq = nextSequence;
}


//------------------------------------------------------------------
/**
 * @brief   wk->seqのコントロールを行う
 *
 * @param   wk    
 *
 * @retval  void      
 */
//------------------------------------------------------------------
static void OekakiSequenceControl( OEKAKI_WORK *wk, int proc_seq )
{
  // シーケンスが違うなら
  if(wk->seq!=wk->next_seq){
    // 終了シーケンスじゃないことを確認して
    if(proc_seq!=SEQ_LEAVE){ 
      wk->seq = wk->next_seq;
    }else{
      // 一部実行制限のシーケンスじゃないことを確認して
      if(FuncTable[wk->next_seq].execOn_SeqLeave){
        OS_Printf("now_next:%d,%d\n",wk->seq,wk->next_seq);
        // シーケンス書き換え
        wk->seq = wk->next_seq;
      }
    }
  }
}


//------------------------------------------------------------------
/**
 * @brief   タッチパネルのはい・いいえウィンドウの消去関数
 *
 * @param   wk    
 *
 * @retval  void      
 */
//------------------------------------------------------------------
static void OekakiResetYesNoWin(OEKAKI_WORK *wk)
{
  if(wk->yesno_flag){
    if(wk->app_menuwork!=NULL){
      YesNoMenuDelete( wk );
    }
    wk->yesno_flag = 0;
  }
}


//------------------------------------------------------------------
/**
 * @brief   YESNOボタンを禁止している間、嘘の検出を行う
 *
 * @param   wk    
 *
 * @retval  int   
 */
//------------------------------------------------------------------
static int FakeEndYesNoSelect( OEKAKI_WORK  *wk )
{

  int button=GFL_UI_TP_HitTrg( fake_yesno_hittbl );
  if( button != GFL_UI_TP_HIT_NONE ){
    return 1;
  }
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   タッチパネルデータの格納処理
 *
 * @param   touchResult   
 * @param   tpData    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetTouchpanelData( TOUCH_INFO *touchResult, TP_ONE_DATA *tpData, int brush_color, int brush )
{
  int i,n;

  // 最初にサンプリングされた点を格納
  if(tpData->Size!=0){
    touchResult->x[0] = tpData->TPDataTbl[0].x;
    touchResult->y[0] = tpData->TPDataTbl[0].y;

    // 最後にサンプリングされた点を格納
    n = tpData->Size-1;
    touchResult->x[1] = tpData->TPDataTbl[n].x;
    touchResult->y[1] = tpData->TPDataTbl[n].y;

    // タッチ座標が0,0なら設定しない
    for( i=0; i<2; i++ ){
      if( (touchResult->x[i] + touchResult->y[i]) == 0 ){
        tpData->Size = 0; // なかったことにする
      }
    }
  }

  // 最大４回サンプリングされるはずだが、それでも送信データは２と格納する
  if(tpData->Size>=2){
    touchResult->size  = 2;
  }else{
    touchResult->size  = tpData->Size;
  }
  
  // 色・大きさ設定
  touchResult->color = brush_color;
  touchResult->brush = brush;

  

}


//----------------------------------------------------------------------------------
/**
 * @brief 接続人数取得
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_connect_num( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNum( wk->param->uniapp );
}

//----------------------------------------------------------------------------------
/**
 * @brief 接続状態をBITMAPで表現
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _get_connect_bit( OEKAKI_WORK *wk )
{
  return Union_App_GetMemberNetBit( wk->param->uniapp );
}


//----------------------------------------------------------------------------------
/**
 * @brief 画面表示ON、上下入れ替え設定
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void _disp_on( void )
{
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );   // メイン画面OBJ面ＯＮ
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );   // サブ画面OBJ面OFF

  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_ON );
  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
}


static const int yn_item[][2]={
  { msg_oekaki_yes,  APP_TASKMENU_WIN_TYPE_NORMAL },
  { msg_oekaki_no,   APP_TASKMENU_WIN_TYPE_NORMAL },
};


//----------------------------------------------------------------------------------
/**
 * @brief はい・いいえ開始
 *
 * @param   wk    
 *
 * @retval  APP_TASKMENU_WORK  *    
 */
//----------------------------------------------------------------------------------
static APP_TASKMENU_WORK  *YesNoMenuInit( OEKAKI_WORK *wk )
{
  APP_TASKMENU_INITWORK init;
  APP_TASKMENU_WORK     *menuwork;
  int i;

  // APPMENUリソース読み込み
  wk->app_res = APP_TASKMENU_RES_Create( GFL_BG_FRAME0_M, 2, wk->font, wk->printQue, HEAPID_OEKAKI );

  for(i=0;i<2;i++){
    wk->yn_menuitem[i].str      = GFL_MSG_CreateString( wk->MsgManager, yn_item[i][0] ); //メニューに表示する文字列
    wk->yn_menuitem[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;   //文字色。デフォルトでよいならばAPP_TASKMENU_ITEM_MSGCOLOR
    wk->yn_menuitem[i].type     = yn_item[i][1];
  }


  init.heapId   = HEAPID_OEKAKI;
  init.itemNum  = 2;
  init.itemWork = wk->yn_menuitem;
  init.posType  = ATPT_LEFT_UP;
  init.charPosX = 21; //ウィンドウ開始位置(キャラ単位
  init.charPosY =  7;
  init.w = 10;  //キャラ単位
  init.h =  3;  //キャラ単位

  // はい・いいえメニュー開始
  menuwork = APP_TASKMENU_OpenMenu( &init, wk->app_res );
  
  OS_Printf("はい・いいえ開始\n");
  
  wk->yesno_flag = TRUE;
  return menuwork;
}


//----------------------------------------------------------------------------------
/**
 * @brief はい・いいえ選択待ち
 *
 * @param   wk    
 *
 * @retval  u32   
 */
//----------------------------------------------------------------------------------
static u32 YesNoMenuMain( OEKAKI_WORK *wk )
{
  u32 ret=YESNO_RET_NONE;

  // メニューメイン
  APP_TASKMENU_UpdateMenu( wk->app_menuwork );

  if(APP_TASKMENU_IsFinish( wk->app_menuwork )){
    if(APP_TASKMENU_GetCursorPos(wk->app_menuwork)==0){
      ret = YESNO_RET_YES;
    }else{
      ret = YESNO_RET_NO;
    }
    // はい・いいえメニュー解放
    YesNoMenuDelete( wk );
  }

  return ret;
  
}

//----------------------------------------------------------------------------------
/**
 * @brief はい・いいえ解放処理
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void YesNoMenuDelete( OEKAKI_WORK *wk )
{
  // 終了処理
  APP_TASKMENU_CloseMenu( wk->app_menuwork );
  GFL_STR_DeleteBuffer( wk->yn_menuitem[1].str );
  GFL_STR_DeleteBuffer( wk->yn_menuitem[0].str );
  APP_TASKMENU_RES_Delete( wk->app_res );
  wk->app_menuwork = NULL;
  OS_Printf("はい・いいえ終了\n");

}


//----------------------------------------------------------------------------------
/**
 * @brief ぐるぐる交換を行ったメンバーを通信友達として登録
 *
 * @param   g2m   
 */
//----------------------------------------------------------------------------------
static void _comm_friend_add( OEKAKI_WORK *wk )
{
  ETC_SAVE_WORK  *etc_save  = SaveData_GetEtc( 
                                GAMEDATA_GetSaveControlWork( wk->param->gamedata));
  int i;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    const MYSTATUS *mystatus = Union_App_GetMystatus(wk->param->uniapp,i);
    if(mystatus!=NULL){
      if(GFL_NET_SystemGetCurrentID()!=i){
        EtcSave_SetAcquaintance( etc_save, MyStatus_GetID(mystatus) );
        OS_Printf("id=%dを友達登録\n", i);
      }
    }
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief 文字列描画関連呼び出し処理
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void Oekaki_PrintFunc( OEKAKI_WORK *wk )
{
  int i;
  PRINTSYS_QUE_Main( wk->printQueName );
  PRINTSYS_QUE_Main( wk->printQue );
  for(i=0;i<OEKAKI_PRINT_UTIL_NAME_WIN4+1;i++){
    PRINT_UTIL_Trans( &wk->printUtil[i], wk->printQueName );
  }
  PRINT_UTIL_Trans( &wk->printUtil[OEKAKI_PRINT_UTIL_MSG], wk->printQue );


}

//----------------------------------------------------------------------------------
/**
 * @brief 成功するまで送信処理を呼び出す関数（ループにつき一回）
 *
 * @param   wk    OEKAKI_WORK *wk
 *
 * @retval  none
 */
//----------------------------------------------------------------------------------
static void Oekaki_SendFunc( OEKAKI_WORK *wk )
{

  switch(wk->send_req.command){
  // 2台目以降の子機の乱入
  // 全台に「これから絵を送るので止まってください」と送信する（親から）
  case CO_OEKAKI_STOP:
    if(GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), 
                         CO_OEKAKI_STOP, 1, &wk->send_req.id))
    {
      wk->send_req.command = 0;
    }
    break;
  // 画像送信が終わったので元に戻っていいよ命令（親から）
  case CO_OEKAKI_RESTART:
    if(GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_RESTART, 0, NULL))
    {
      wk->send_req.command = 0;
    }
    break;
  // 親機が終了するので子機も強制終了と告知（親から）
  case CO_OEKAKI_END_CHILD:
    if(GFL_NET_SendData( GFL_NET_GetNetHandle( GFL_NET_NETID_SERVER), CO_OEKAKI_END_CHILD,
                            sizeof(COMM_OEKAKI_END_CHILD_WORK), &wk->send_req.trans_work))
    {
      wk->send_req.command = 0;
    }
    break;
  //子機が親機から離脱許可をもらったので本当に抜けるという表明を送信
  case CO_OEKAKI_END_CHILD_DECIDE:
    if(GFL_NET_SendData( GFL_NET_HANDLE_GetCurrentHandle(), 
                         CO_OEKAKI_END_CHILD_DECIDE, 0, NULL))
    {
      wk->send_req.command = 0;
    }
    break;
  default:
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief データ送信リクエスト
 *
 * @param   wk    
 * @param   command   
 * @param   id    
 */
//----------------------------------------------------------------------------------
void Oekaki_SendDataRequest( OEKAKI_WORK *wk, int command, int id )
{
  if(wk->send_req.command==0){
    wk->send_req.command = command;
    wk->send_req.id = id;
  }else{
//    GF_ASSERT_MSG(0,"送信リクエストが重複した now=%d, new=%d\n", wk->send_req.command, command);
    OS_Printf("送信リクエストが重複した now=%d, new=%d\n", wk->send_req.command, command);
  }
}




//----------------------------------------------------------------------------------
/**
 * @brief FIFO初期化
 *
 * @param   touchFifo   
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_Init( TOUCH_FIFO *touchFifo )
{
  touchFifo->start = 0;
  touchFifo->end   = 0;
}

static int _get_fifo_previous_end(TOUCH_FIFO *touchFifo)
{
  int end = touchFifo->end;
  
  end--;
  if(end<0){
    end = OEKAKI_FIFO_MAX-1;
  }
 return end;
}

//----------------------------------------------------------------------------------
/**
 * @brief FIFOセット位置＋１
 *
 * @param   touchFifo   
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_AddEnd( TOUCH_FIFO *touchFifo )
{

  // 今回格納されているのが０で、前回も０だった場合は積まない
  if(touchFifo->fifo[touchFifo->end][0].size==0){
    if(touchFifo->fifo[_get_fifo_previous_end(touchFifo)][0].size==0){
//      OS_Printf("FIFOを進めなかった touchFifo->end=%d\n",touchFifo->end);
      return;
    }
  }

  // 頂点情報が格納されていたら進める
  touchFifo->end++;

  // リングバッファが1周したら０に
  if(touchFifo->end>=OEKAKI_FIFO_MAX){
    touchFifo->end=0;
  }
  
}

//=============================================================================================
/**
 * @brief 親機用のリングバッファ＋１
 *
 * @param   touchFifo   
 */
//=============================================================================================
void OekakiTouchFifo_AddEndParent( TOUCH_FIFO *touchFifo )
{
  int i;
  int flag=0,flag2=0;

  // 今回の5人分のデータが全部０
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    if(touchFifo->fifo[touchFifo->end][i].size==0){
      flag++;
    }
  }
  // 一つ前に戻ってみても5人分のデータが全部０
  if(flag==5){
    int previous=_get_fifo_previous_end(touchFifo);
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      if(touchFifo->fifo[previous][i].size==0){
        flag2++;
      }
    }
    
  }
  // 上の2つの条件を全部満たしていたら積まない
  if(flag2==5){
    return;
  }

  // 終端ポイントを進める
  touchFifo->end++;
  // リングバッファが1周したら０に
  if(touchFifo->end>=OEKAKI_FIFO_MAX){
    touchFifo->end=0;
  }
  
}

static void _fifo_size_clear( TOUCH_INFO *pack )
{
  int i;
  for(i=0;i<OEKAKI_MEMBER_MAX;i++){
    pack[i].size = 0;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief FIFO取り出し位置＋１
 *
 * @param   touchFifo   
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_AddStart( TOUCH_FIFO *touchFifo )
{
  // startはendを抜かない
  if(touchFifo->start!=touchFifo->end){
    _fifo_size_clear( touchFifo->fifo[touchFifo->start] );
    touchFifo->start++;

    // リングバッファが1周したら０に
    if(touchFifo->start>=OEKAKI_FIFO_MAX){
      touchFifo->start=0;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief  頂点情報をFIFOにセット
 *
 * @param   myResult    
 * @param   touchFifo   
 * @param   id
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_Set( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo, int id )
{
  touchFifo->fifo[touchFifo->end][id] = *myResult;
}

//----------------------------------------------------------------------------------
/**
 * @brief FIFOから頂点情報を取得（子機用）
 *
 * @param   myResult    
 * @param   touchFifo   
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_Get( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo )
{
  if(touchFifo->start!=touchFifo->end){
    *myResult = touchFifo->fifo[touchFifo->start][0];
  }else{
    myResult->size = 0;
  }
  
}

//----------------------------------------------------------------------------------
/**
 * @brief FIFOから頂点情報をまとめて取得（親機用）
 *
 * @param   myResult    
 * @param   touchFifo   
 */
//----------------------------------------------------------------------------------
void OekakiTouchFifo_GetParent( TOUCH_INFO *myResult, TOUCH_FIFO *touchFifo )
{
  int i;
  if(touchFifo->start!=touchFifo->end){
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      myResult[i] = touchFifo->fifo[touchFifo->start][i];
    }
  }else{
    for(i=0;i<OEKAKI_MEMBER_MAX;i++){
      myResult[i].size = 0;
    }
  }
}

