//======================================================================
/**
 * @file	name_input.h
 * @brief	名前入力
 * @author	Toru=Nagihashi
 * @data	09/10/07
 */
//======================================================================

#ifndef NAME_INPUT_H__
#define NAME_INPUT_H__

#include "savedata\config.h"
#include "savedata\box_savedata.h"
#include "savedata/save_control_intr.h"
#include "savedata/misc.h"

//=============================================================================
/**
 *	定義
 */
//=============================================================================

//モードがNAMEIN_FRIENDNAMEのときparam1へいれると、友達手帳の初期入力用のはてなOBJが出ます
#define NAMEIN_TRAINER_VIEW_UNKNOWN   (0xFFFF)

#define NAMEIN_STR_BUFFER_LENGTH	    ( 10 )

//-------------------------------------
///	名前入力モード  
//=====================================
typedef enum 
{
	NAMEIN_MYNAME = 0,	// 自分の名前	param1自分の性別
	NAMEIN_POKEMON,			// ポケモンの名前 param1モンスター番号 param2フォルム番号
	NAMEIN_BOX,					// ボックスの名前
	NAMEIN_RIVALNAME,		// ライバルネーム WBでは使わなくなりました
	NAMEIN_FRIENDNAME,  // WIFIともだち手帳に書き込むともだちの名前 param1 ユニオンの見た目(NAMEIN_TRAINERVIEW_UNKNOWNであればともだち手帳の初期入力用の？が出ます)
  NAMEIN_GREETING_WORD, // すれちがい挨拶言葉 param1自分の性別
  NAMEIN_THANKS_WORD,   // すれちがいお礼言葉 param1自分の性別
  NAMEIN_FREE_WORD,     // すれちがいフリーワードparam1自分の性別

	NAMEIN_MAX,
}NAMEIN_MODE;

//-------------------------------------
///	入力文字数
//    以下EOM含まない長さです
//=====================================
// ポケモン名前入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_POKEMON_LENGTH	(  5 )
#else
#define NAMEIN_POKEMON_LENGTH	( 10 )
#endif

// トレーナー名入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_PERSON_LENGTH	(  5 )
#else
#define NAMEIN_PERSON_LENGTH	( 7 )
#endif


// グループ名入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_GROUP_LENGTH		(  5 )
#else
#define NAMEIN_GROUP_LENGTH		( 7 )
#endif

// ボックス入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_BOX_LENGTH		( 8 )
#else
#define NAMEIN_BOX_LENGTH		( 8 )
#endif

// すれ違い「挨拶」入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_GREETING_WORD_LENGTH		( 8 )
#else
#define NAMEIN_GREETING_WORD_LENGTH		( 8 )
#endif

// すれ違い「お礼」入力の長さ
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_THANKS_WORD_LENGTH		( 8 )
#else
#define NAMEIN_THANKS_WORD_LENGTH		( 8 )
#endif

// すれ違い　フリーワード
#if (PM_LANG==LANG_JAPAN)
#define NAMEIN_FREE_WORD_LENGTH		( 8 )
#else
#define NAMEIN_FREE_WORD_LENGTH		( 8 )
#endif

//=============================================================================
/**
 *	構造体
 */
//=============================================================================
//-------------------------------------
///	PROCに渡すパラメータ
//=====================================
typedef struct
{
	NAMEIN_MODE mode;		//[in]入力モード(enum参照）

	union								//[in]種類ごとに違う引数１
	{	
		u16 param1;				//各モードにて必要
		u16 hero_sex;			//NAMEIN_MYNAME			…主人公の性別
		u16 mons_no;			//NAMEIN_POKEMON		…モンスター番号
		u16 fld_person_id;//NAMEIN_FRIENDNAME	…友達の姿（フィールド３DOBJ）
	};

	union								//[in]種類ごとに違う引数２
	{	
		u16 param2;				//NAMEIN_POKEMON以外は必要なし
		u16 form;					//NAMEIN_POKEMON	…フォルム番号
	};
	const POKEMON_PARAM *pp;	//[in]ニックネーム登録をPPから作成する場合

  const STRBUF       *box_strbuf;   // [in] ポケモン捕獲でボックス転送になった時に必要
                                    // ( mode==NAMEIN_POKEMON && box_strbuf!=NULL ) のとき、ポケモン捕獲時
  const BOX_MANAGER  *box_manager;  // [in] box_strbuf!=NULLのときのみ有効
  u32                box_tray;      // [in] box_strbuf!=NULLのときのみ有効

	u32 wordmax;		// [in]入力文字最大数

	BOOL cancel;		// [out]名前入力が終了した時に反映されるフラグ。
									// 入力文字が０文字だった。もしくは最初と同じだった場合はこのフラグがTRUE

	STRBUF *strbuf; // [in/out]このSTRBUFには２つの意味が存在する。
									// １つは名前入力画面からデータを受け取るワークであること。
									// 名前入力が終わったらここからコピーする
									// もう１つは、名前入力に行くときにここに文字列を格納しておくと,
									// 名前入力画面はその文字列を表示しながら開始する。引数としても機能する

  INTR_SAVE_CONTROL *p_intr_sv;  // [in]内部で、セーブしながら名前入力する(イントロ画面用)

  MISC              *p_misc;    //  [in]内部でキーボードタイプを保存するためのセーブデータ NULLを渡すとセーブしません
                                                                                          //※NULLを渡して良いのはイントロだけです

} NAMEIN_PARAM;

//=============================================================================
/**
 *	PROC
 */
//=============================================================================
//-------------------------------------
///	NAMEINオーバーレイ関係
//=====================================
FS_EXTERN_OVERLAY(namein);

//-------------------------------------
///	PROCデータ
//=====================================
extern const GFL_PROC_DATA NameInputProcData;

//=============================================================================
/**
 *	外部参照  以下関数はnotwifiにおいております
 */
//=============================================================================
//-------------------------------------
///	PARAM作成、破棄
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAMを作成
 *					解放は下記、NameIn_FreeParamで行ってください
 *
 *	@param	HEAPID		ヒープID
 *	@param	mode			入力の種類
 *	@param	param1		種類による引数1
 *	@param	param2		種類による引数2
 *	@param	wordmax		文字入力数
 *	@param	default_str	初期に入力されている文字。いらない場合はNULL
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParam( HEAPID heapId, NAMEIN_MODE mode, int param1, int param2, int wordmax, const STRBUF *default_str, MISC *p_misc );
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PARAM解放
 *
 *	@param		NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern void NAMEIN_FreeParam( NAMEIN_PARAM *param );
//-------------------------------------
///	ポケモンニックネーム専用パラメータ作成、破棄
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンニックネーム登録をPPから行う場合のNAMEIN_PARAM作成
 *					解放はNAMEIN_FreeParamで行ってください
 *
 *	@param	HEAPID		ヒープID
 *	@param	wordmax		文字入力数
 *	@param	default_str	初期に入力されている文字。いらない場合はNULL
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParamPokemonByPP( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str, MISC *p_misc );

//----------------------------------------------------------------------------
/**
 *	@brief	ポケモンニックネーム登録をPPから行う場合のNAMEIN_PARAM作成
 *	        ポケモン捕獲時はこちらを使用してください
 *					解放はNAMEIN_FreeParamで行ってください
 *
 *	@param	HEAPID		ヒープID
 *	@param	wordmax		文字入力数
 *	@param	default_str	初期に入力されている文字。いらない場合はNULL
 *	@param  box_strbuf   [ポケモンニックネーム]は○○のパソコンの[ボックス名]に転送された！
 *	@param  box_manager  ボックスマネージャ(box_strbuf!=NULLのときしか使われない) 
 *	@param  box_tray     ボックストレイナンバー(box_strbuf!=NULLのときしか使われない) 
 *
 *	@return	NAMEIN_PARAM
 */
//-----------------------------------------------------------------------------
extern NAMEIN_PARAM *NAMEIN_AllocParamPokemonCapture( HEAPID heapId, const POKEMON_PARAM *pp, int wordmax, const STRBUF *default_str,
                                                      const STRBUF *box_strbuf, const BOX_MANAGER *box_manager, u32 box_tray, MISC *p_misc );

//------------------------------------
///	PARAMからの取得
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief		NAMEIN_PARAMからの文字列取得
 *
 *	@param		param		NAMEIN_PARAM
 *	@param		strbuf	受け取り文字列バッファ
 */
//-----------------------------------------------------------------------------
extern void NAMEIN_CopyStr( const NAMEIN_PARAM *param, STRBUF *strbuf );
//----------------------------------------------------------------------------
/**
 *	@brief		NAMEIN_PARAMからの文字列コピー作成
 *
 *	@param		param		NAMEIN_PARAM
 *	@param		HEAPID	文字列作成ヒープID
 *
 *	@return		コピーして作成したSTRBUF
 */
//-----------------------------------------------------------------------------
extern STRBUF* NAMEIN_CreateCopyStr( const NAMEIN_PARAM *param, HEAPID heapID );
//----------------------------------------------------------------------------
/**
 *	@brief	NAMEIN_PROCの終了取得
 *
 *	@param	param	NAMEIN_PARAM
 *
 *	@retval	TRUEでデフォルト文字と同じ文字列or何も入力せず終了
 *	@retval	FALSEで通常入力で終了
 */
//-----------------------------------------------------------------------------
extern BOOL NAMEIN_IsCancel( const NAMEIN_PARAM *param );


//=============================================================================
/**
 *    SEプリセット
 */
//=============================================================================
extern const u32 NAMEIN_SE_PresetData[];
extern const u32 NAMEIN_SE_PresetNum;


#endif	//NAME_INPUT_H__
