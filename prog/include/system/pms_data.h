//============================================================================================
/**
 * @file  pms_data.h
 * @bfief 簡易文章（簡易会話データと定型文を組み合わせた文章）
 * @author  taya
 * @date  05.12.27
 */
//============================================================================================
#ifndef __PMS_DATA_H__
#define __PMS_DATA_H__

//------------------------------------------------------
/**
 *  関連定数
 */
//------------------------------------------------------
#define  PMS_WORD_MAX     (2)       ///< 文中に含まれる単語の最大数
#define  PMS_WORD_NULL    (0xffff)  ///< 単語として無効な値
#define  PMS_DECO_WIDTH   ( 84 )  ///< デコ文字の最大幅（ドット）

//全開放デバッグ
#ifdef PM_DEBUG
#define  PMS_ALLOPEN (1) 
#endif


//------------------------------------------------------
/**
 *  文章タイプ
 */
//------------------------------------------------------
enum  PMS_TYPE {
  PMS_TYPE_BATTLE_READY,  ///< 対戦開始前
  PMS_TYPE_BATTLE_WON,  ///< 対戦勝ったとき
  PMS_TYPE_BATTLE_LOST, ///< 対戦負けた時
  PMS_TYPE_MAIL,      ///< メールあいさつ
  PMS_TYPE_UNION,     ///< ユニオンあいさつ


  PMS_TYPE_MAX,
};

//------------------------------------------------------
/**
 *  デコ文字種類
 */
//------------------------------------------------------
typedef enum {
  PMS_DECOID_NULL,
  PMS_DECOID_ORG,

  PMS_DECOID_HERO = PMS_DECOID_ORG, ///< はじめまして
  PMS_DECOID_HERO2, ///< こんにちは
  PMS_DECOID_LOVE,  ///< だいすき
  PMS_DECOID_YELL,  ///< がんばれ！ 
  PMS_DECOID_FUN,   ///< たのしい
  PMS_DECOID_HAPPY, ///< うれしい
  PMS_DECOID_TANKS, ///< ありがとう
  PMS_DECOID_HIGHEST, ///< さいこう
  PMS_DECOID_SORRY,   ///< ごめんね
  PMS_DECOID_BYEBYE,  ///< バイバイ

  PMS_DECOID_MAX,
}PMS_DECO_ID;

//------------------------------------------------------
/**
 *  単語型定義
 */
//------------------------------------------------------
typedef u16   PMS_WORD; 
// 上位から 
//   4 bit -> ローカライズ用
//   1 bit -> 単語／デコメフラグ (1ならデコメ)
//  11 bit -> 単語／デコメナンバー

#define PMS_WORD_DECO_MASK ( 0x1 )    ///< デコメ判定用ビットマスク
#define PMS_WORD_DECO_BITSHIFT ( 11 )  ///< デコメ判定用ビット オフセット
#define PMS_WORD_NUM_MASK ( 0x7FF )    ///< 単語・デコメナンバー ビットマスク

//------------------------------------------------------
/**
 *  文章型定義
 *
 *  構造体内容が公開されていますが、
 *  勝手にメンバを参照したり書き換えたりしてはいけません。
 *  必要な処理はインターフェイス関数を通してのみ行ってください。
 *
 *（公開されているのは、この構造体を含む構造体（メール等）を
 * セーブデータ用に定義しなければならないためです）
 *
 */
//------------------------------------------------------
typedef struct {
  u16       sentence_type;      ///< 文章タイプ
  u16       sentence_id;      ///< タイプ内ID
  PMS_WORD  word[PMS_WORD_MAX];   ///< 単語ID
}PMS_DATA;






//------------------------------------------------------------------
/**
 * 構造体を空文章としてクリアする
 *
 * @param   pms
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Clear( PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * 状態初期化
 *
 * @param   pms       文章型領域へのポインタ
 * @param   sentence_type 文章タイプ
 *
 * ※ここでの文章タイプは、入力画面遷移後のデフォルト値として参照される。
 *   入力画面での編集後、異なる文章タイプになっていることはあり得る。
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Init( PMS_DATA* pms, u32 sentence_type );

//------------------------------------------------------------------
/**
 * ユニオンルームデフォルトあいさつとして初期化する
 *
 * @param   pms   文章型へのポインタ
 */
//------------------------------------------------------------------
extern void PMSDAT_SetupDefaultUnionMessage( PMS_DATA* pms );


//------------------------------------------------------------------
/**
 * バトルタワーデフォルトあいさつとして初期化する
 *
 * @param   pms     文章型へのポインタ
 * @param   msgType   あいさつタイプ
 *
 */
//------------------------------------------------------------------
//extern void PMSDAT_SetupDefaultBattleTowerMessage( PMS_DATA* pms, BTWR_PLAYER_MSG_ID msgType );



//------------------------------------------------------------------
/**
 * 文章型から文字列を生成。この文字列はそのままSTR_Printに回して表示できる
 *
 * @param   pms     文章型へのポインタ
 * @param   heapID    バッファ生成用ヒープID
 *
 * @retval  STRBUF*   生成された文字列を含むバッファ
 */
//------------------------------------------------------------------
extern STRBUF* PMSDAT_ToString( const PMS_DATA* pms, u32 heapID );

//------------------------------------------------------------------
/**
 * 文章型から、タグ展開前の文字列を取得。そのまま表示は不可。
 * これを使うのは、おそらく簡易会話入力画面のみ。
 *
 * @param   pms
 * @param   heapID
 *
 * @retval  STRBUF*
 */
//------------------------------------------------------------------
extern STRBUF*  PMSDAT_GetSourceString( const PMS_DATA* pms, u32 heapID );

//------------------------------------------------------------------
/**
 * 空文章になっていないかチェック
 *
 * @param   pms   文章型へのポインタ
 *
 * @retval  BOOL  TRUEで有効
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_IsEnabled( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * 文章が完成しているかチェック
 *
 * @param   pms
 *
 * @retval  BOOL
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_IsComplete( const PMS_DATA* pms , const HEAPID heapID );


//------------------------------------------------------------------
/**
 * 設定されている文章タイプを取得
 *
 * @param   pms   文章型へのポインタ
 *
 * @retval  u32   文章タイプ（enum PMS_TYPE）
 */
//------------------------------------------------------------------
extern u32  PMSDAT_GetSentenceType( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * 設定されている文章インデックスを返す
 *
 * @param   pms   文章型へのポインタ
 *
 * @retval  u32   文章インデックス（このままでは使い道がないハズ。入力画面用。）
 */
//------------------------------------------------------------------
extern u32 PMSDAT_GetSentenceID( const PMS_DATA* pms );

//------------------------------------------------------------------
/**
 * 設定されている単語ナンバーを取得
 *
 * @param   pms   文章型へのポインタ
 * @param   pos   何番目の単語か？
 *
 * @retval  PMS_WORD  簡易会話単語ナンバー
 */
//------------------------------------------------------------------
extern PMS_WORD  PMSDAT_GetWordNumber( const PMS_DATA* pms, int pos );

extern BOOL PMSDAT_IsDecoID( const PMS_DATA* pms, int pos );

//------------------------------------------------------------------
/**
 * 内容比較
 *
 * @param   pms1
 * @param   pms2
 *
 * @retval  BOOL    一致していたらTRUEを返す
 */
//------------------------------------------------------------------
extern BOOL PMSDAT_Compare( const PMS_DATA* pms1, const PMS_DATA* pms2 );

//------------------------------------------------------------------
/**
 * 文章データのコピー
 *
 * @param   src   コピー元ポインタ
 * @param   dst   コピー先ポインタ
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_Copy( PMS_DATA* dst, const PMS_DATA* src  );


//------------------------------------------------------------------
/**
 * 文章タイプから、そのタイプに含まれる文章パターンの最大数を返す
 *
 * @param   sentence_type
 *
 * @retval  u32
 */
//------------------------------------------------------------------
extern u32  PMSDAT_GetSentenceIdMax( u32 sentence_type );

//------------------------------------------------------------------
/**
 * 文章タイプ、パターンをセット
 *
 * @param   pms
 * @param   sentence_type
 * @param   sentence_id
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_SetSentence( PMS_DATA* pms, u32 sentence_type, u32 sentence_id );

//-----------------------------------------------------------------------------
/**
 *	@brief  単語ナンバーセット
 *
 *	@param	pms       文章型へのポインタ
 *	@param	pos       文章内にセットする位置
 *	@param	word      単語ナンバー
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
extern void PMSDAT_SetWord( PMS_DATA* pms, u32 pos, PMS_WORD word );

//-----------------------------------------------------------------------------
/**
 *	@brief  デコメナンバーセット
 *
 *	@param	pms       文章型へのポインタ
 *	@param	pos       文章内にセットする位置
 *	@param	deco_id   デコメナンバー
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
extern void PMSDAT_SetDeco( PMS_DATA* pms, u32 pos, PMS_DECO_ID deco_id );

//------------------------------------------------------------------
/**
 * 文章タイプ＆IDから、使わない単語領域をクリアしておく
 *
 * @param   pms
 *
 */
//------------------------------------------------------------------
extern void PMSDAT_ClearUnnecessaryWord( PMS_DATA* pms , const HEAPID heapID );

#ifdef PM_DEBUG
//------------------------------------------------------------------
/**
 * テスト用に適当な内容をセットする
 *
 * @param   pms
 */
//------------------------------------------------------------------
extern void PMSDAT_SetDebug( PMS_DATA* pms );   // 固定
extern void PMSDAT_SetDebugRandomDeco( PMS_DATA* pms, HEAPID heapID ); //ランダムデコメ
extern void PMSDAT_SetDebugRandom( PMS_DATA* pms ); // ランダム

#endif

#endif
