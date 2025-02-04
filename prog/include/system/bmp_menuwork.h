//======================================================================
/**
 * @file	bmp_menu_list.h
 * @brief	bmp_menu と bmp_list で使用する構造体の定義
 * @author	taya
 * @date	2005.12.02
 *
 * bmp_menu と bmp_list で全く同じ型・同じ用途の構造体を
 * 別個に定義しているのでまとめておく。
 *
 * こうしておけば、この構造体を動的に生成する関数など、
 * 共通のものを呼び出せる。
 *
 * さらに、生の文字配列（STRCODE = u16）を使用している箇所が
 * STRBUF型に置き換わったら、ここの定義のみを書き換えれば良い。
 */
//======================================================================
#ifndef __BMP_MENU_LIST_H__
#define __BMP_MENU_LIST_H__

#include <gflib.h>
#include <msgdata.h>

//======================================================================
//  定数
//======================================================================
/// リストの終端コード
#define LIST_ENDCODE		((const void*)0xffffffff)

//======================================================================
//	構造体
//======================================================================
//--------------------------------------------------------------
//	メニュー表示用の文字列と、それが選択された時のパラメータ値を
//	定義するための構造体。
//--------------------------------------------------------------
struct _BMP_MENULIST_DATA {
	const void * str;
	u32	param;
};

// どっちも同じ型として定義しておくことで、以下の共通関数群を呼び出せる
typedef	struct _BMP_MENULIST_DATA BMP_MENULIST_DATA, BMPLIST_DATA, BMPMENU_DATA;

//======================================================================
//	関数
//======================================================================
//------------------------------------------------------------------
/**
 * リストバッファを作成
 *
 * @param   maxElems		リストに登録する文字列の最大数
 * @param   heapID			リスト作成先ヒープID
 *
 * @retval  BMP_MENULIST_DATA*		作成されたリストバッファ
 */
//------------------------------------------------------------------
extern BMP_MENULIST_DATA * BmpMenuWork_ListCreate( u32 maxElems, u32 heapID );

//------------------------------------------------------------------
/**
 * リストバッファを破棄する
 * ※BMP_MENULIST_Createで作成したもの以外を渡してはダメ。
 *
 * @param   list		リストバッファ
 */
//------------------------------------------------------------------
extern void BmpMenuWork_ListDelete( BMP_MENULIST_DATA* list_top );

//------------------------------------------------------------------
/**
 * リストバッファに文字列＆パラメータをセットする（マネージャを介してアーカイブデータから読み込み）
 *
 * @param   list		[in] リストバッファ
 * @param   man			[in] メッセージデータマネージャ
 * @param   strID		文字列ID
 * @param   param		文字列パラメータ
 */
//------------------------------------------------------------------
extern void BmpMenuWork_ListAddArchiveString( BMP_MENULIST_DATA* list,
	GFL_MSGDATA *msgdata, u32 strID, u32 param, HEAPID heap_id );

#if 0
extern void BmpMenuList_AddArchiveString( BMP_MENULIST_DATA *list );
		const MSGDATA_MANAGER *man, u32 strID, u32 param );
#endif

//------------------------------------------------------------------
/**
 * リストバッファに文字列＆パラメータをセットする（文字列を直接指定する）
 *
 * @param   list		[in] リストバッファ
 * @param   str			[in] 文字列
 * @param   param		文字列パラメータ
 *
 */
//------------------------------------------------------------------
extern void BmpMenuWork_ListAddString( BMP_MENULIST_DATA* list,
		const STRBUF *str, u32 param, HEAPID heapID  );

#if 0
extern void BmpMenuList_AddString( BMP_MENULIST_DATA* list,
		const STRBUF* str, u32 param )
#endif

//------------------------------------------------------------------
/**
 * リストバッファに他のリストのデータをLinkする
 * @param   list		[inout] リストバッファ
 * @param   insList		[in] リストポインタ
 *
 */
//------------------------------------------------------------------
extern void BmpMenuWork_ListAddLinkList(
	BMP_MENULIST_DATA* list, const BMP_MENULIST_DATA* insList );

//------------------------------------------------------------------
/**
 * リストバッファのSTRBUFを破棄する
 * ※BMP_MENULIST_Createで作成したもの以外を渡してはダメ。
 *
 * @param   list		リストバッファ
 *
 */
//------------------------------------------------------------------
extern void BmpMenuWork_ListSTRBUFDelete( BMP_MENULIST_DATA *list_top );

//--------------------------------------------------------------
/**
 * リストバッファ、リスト最大文字数を取得
 * @param list_top
 * @retval u32 
 */
//--------------------------------------------------------------
extern u32 BmpMenuWork_GetListMaxLength(
    const BMP_MENULIST_DATA *list_top, int *no_buf );

//--------------------------------------------------------------
/**
 * リストバッファ　リスト最大数を取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
extern u32 BmpMenuWork_GetListMax( const BMP_MENULIST_DATA *list_top );

#endif	//__BMP_MENU_LIST_H__
