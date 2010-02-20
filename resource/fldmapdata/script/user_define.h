//======================================================================
/**
 * @file  user_define.h
 * @author  GAMEFREAK inc.
 * @date  2009.11.11
 */
//======================================================================

#pragma once


//======================================================================
//  最初の3体識別用定義
//======================================================================
#define FIRST_POKETYPE_KUSA   ( 0 )	//草タイプを選んだ
#define FIRST_POKETYPE_HONOO	( 1 )	//炎タイプを選んだ
#define FIRST_POKETYPE_MIZU   ( 2 )	//水タイプを選んだ

//======================================================================
//  マップ書き換えの指定ID
//======================================================================
#define MAPREPLACE_ID_MUSEUM    MAPREPLACE_ID_01    /* 博物館 */
#define MAPREPLACE_ID_CHAMPROOM MAPREPLACE_ID_02    /* チャンピオン部屋 */
#define MAPREPLACE_ID_WFBC      MAPREPLACE_ID_03    /* WF/BC制御用 */
#define MAPREPLACE_ID_NCASTLE   MAPREPLACE_ID_04    /* Nの城の謁見の間 */
#define MAPREPLACE_ID_DUN08     MAPREPLACE_ID_05    /* りゅうらせんの塔 屋外 */
#define MAPREPLACE_ID_DUN04     MAPREPLACE_ID_06    /* 古代の城 */
//#define MAPREPLACE_ID_07  未使用
//#define MAPREPLACE_ID_08  未使用
//#define MAPREPLACE_ID_09  未使用
//#define MAPREPLACE_ID_10  未使用

//======================================================================
//  BMPメニューの表示位置用定義	※所持金表示もこの定義を使います
//======================================================================

//100220 ３月には右上表示へ。プログラム側で指定位置が右上基準になったら値を変更します。
#define MENU_X1   ( 1 )	//画面左上に表示する場合
#define MENU_Y1   ( 1 )	//画面左上に表示する場合
#define MENU_X2   ( 1 )	//所持金表示の下にメニューを表示する場合
#define MENU_Y2   ( 5 )	//所持金表示の下にメニューを表示する場合


