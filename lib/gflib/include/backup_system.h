//=============================================================================
/**
 * @file	backup_system.h
 * @brief	バックアップシステム・セーブデータ用ヘッダ
 * @author	tamada	GAME FREAK Inc.
 * @date	2007.06.14
 *
 *
 * このヘッダはセーブデータの生成処理、バックアップ領域へのアクセスなどの
 * 直接セーブデータにかかわる部分にのみ必要。
 *
 * 通常はSAVEDATA型（savedata_def.hで定義）を経由して各データにアクセスできればよいため、
 * このヘッダをインクルードする箇所は限定される。
 */
//=============================================================================
#pragma	once

#include "savedata_def.h"
#include <heap.h>

#ifdef __cplusplus
extern "C" {
#endif

//============================================================================================
//
//			セーブデータ構築用定義
//
//============================================================================================
///---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 (*FUNC_GET_SIZE)(void);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef void (*FUNC_INIT_WORK)(void *);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 GFL_SVDT_ID;

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//セーブ失敗エラーの指定
typedef enum {
	GFL_SAVEERROR_DISABLE_WRITE = 0,		///<書き込み失敗の時
	GFL_SAVEERROR_DISABLE_READ,				///<書き込み前のリードチェック失敗のとき
}GFL_SAVEERROR_DISABLE;

//フラッシュエラー(ロード時)が発生した場合に呼ばれるコールバック関数型
typedef void (*GFL_BACKUP_LOAD_ERROR_FUNC)( void );
//フラッシュエラー(セーブ時)が発生した場合に呼ばれるコールバック関数型
typedef void (*GFL_BACKUP_SAVE_ERROR_FUNC)( GFL_SAVEERROR_DISABLE err_disable );


//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ内容データ用の構造体定義
 */
//---------------------------------------------------------------------------
typedef struct {
	GFL_SVDT_ID		gmdataID;		///<セーブデータ識別ID
	FUNC_GET_SIZE	get_size;		///<セーブデータサイズ取得関数
	FUNC_INIT_WORK	init_work;		///<セーブデータ初期化関数
}GFL_SAVEDATA_TABLE;

//---------------------------------------------------------------------------
///	セーブデータ定義構造体
//---------------------------------------------------------------------------
typedef struct {
	const GFL_SAVEDATA_TABLE * table;	///<セーブデータ定義テーブルのアドレス
	u32 table_max;						      ///<セーブデータ定義テーブルの要素数
	u32 savearea_top_address;		  	///<使用するバックアップ領域の先頭アドレス
	u32 savearea_mirror_address;		///<使用するミラーリング領域の先頭アドレス
	                                ///<ミラーリング無しの場合はsavearea_top_addressと[同じ値]を指定
	u32 savearea_size;					///<使用するバックアップ領域の大きさ
	u32 magic_number;					///<使用するマジックナンバー
}GFL_SVLD_PARAM;



//============================================================================================
//
//				バックアップ制御関数
//
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	バックアップシステム起動
 * @param	heap_save_id
 *
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_Init(u32 heap_save_id, GFL_BACKUP_LOAD_ERROR_FUNC load_error_func, GFL_BACKUP_SAVE_ERROR_FUNC save_error_func);

//---------------------------------------------------------------------------
/**
 * @brief	バックアップシステム終了
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_Exit(void);
//---------------------------------------------------------------------------
/**
 * @brief	バックアップフラッシュ存在チェック
 * @retval	TRUE	フラッシュが存在する
 * @retval	FALSE	フラッシュが存在しない
 */
//---------------------------------------------------------------------------
extern BOOL GFL_BACKUP_IsEnableFlash(void);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：ロード処理
 * @param	sv
 * @param heap_temp_id    この関数内でのみテンポラリとして使用するヒープID
 * @return	LOAD_RESULT
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_BACKUP_Load(GFL_SAVEDATA * sv, u32 heap_temp_id);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：セーブ処理
 * @param	sv
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_Save(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：消去処理
 * @param	sv			        セーブデータ構造へのポインタ
 * @param heap_temp_id    この関数内でのみテンポラリとして使用するヒープID
 */
//---------------------------------------------------------------------------
extern BOOL GFL_BACKUP_Erase(GFL_SAVEDATA * sv, u32 heap_temp_id);

//==================================================================
/**
 * フラッシュを工場出荷状態の値でクリアする
 *
 * @param   heap_id		    テンポラリヒープ
 * @param	  start_address	消去開始位置(フラッシュのアドレス（０〜）※セクタ指定ではない)
 * @param   erase_size		消去するサイズ(byte)
 */
//==================================================================
extern void GFL_BACKUP_FlashErase(HEAPID heap_id, u32 start_address, u32 erase_size);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：分割セーブ：初期化
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_Init(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：分割セーブ：メイン
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_SAVEASYNC_Main(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ：分割セーブ：キャンセル
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_Cancel(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	外部セーブ処理(外部と内部を片面ずつ交互にセーブを行う)
 * @param	sv			      内部セーブデータ構造へのポインタ
 * @param	extra_sv			外部セーブデータ構造へのポインタ
 * @param link_ptr      リンク情報として外部セーブのグローバルカウンタを
 *                      片面の外部セーブが完了した時点でここのポインタの箇所に
 *                      システム側からセットします。
 *                      内部セーブの任意の場所にリンク情報保存用のエリアを用意し
 *                      そのポインタを渡してください
 *                      ※ロード時にシステム側でリンク情報のチェックはしない為、
 *                        ゲーム側でリンク情報が一致しているかチェックしてください
 *                          GFL_BACKUP_GetGlobalCounter( extra_sv ) == *link_ptr
 *
 *                      ※ポケモンのようにセーブがある状態で「さいしょから」ができると、
 *                        グローバルカウンタが一致してしまう場合があります。
 *                        自分のセーブデータと誤認されないようにするため、
 *                        ゲーム側で「外部セーブをしたことがある」
 *                        のようなフラグなりマジックキーなりを持ち、ロード処理そのものを
 *                        しないように回避してください。
 * 
 * @retval	TRUE		書き込み成功
 * @retval	FALSE		書き込み失敗
 *
 * 外部と内部のリンク情報はあらかじめ外側で結んでおく事
 * セーブシステムはリンクの関連付けまではしません
 *
 * Cancelは禁止です
 *
 * PL,GSまでの外部セーブ両面が完了後、内部セーブ、と続くよりも
 * 外部セーブとのリンク情報が切れなくなる為、外部セーブもちゃんと生き残る
 */
//---------------------------------------------------------------------------
extern void GFL_BACKUP_SAVEASYNC_EXTRA_Init(GFL_SAVEDATA * sv, GFL_SAVEDATA * extra_sv, u32 *link_ptr);

//---------------------------------------------------------------------------
/**
 * @brief	外部セーブ：分割セーブメイン処理(外部と内部を片面ずつ交互にセーブを行う)
 * @param	sv			      内部セーブデータ構造へのポインタ
 * @param	extra_sv			外部セーブデータ構造へのポインタ
 * @return	SAVE_RESULT
 */
//---------------------------------------------------------------------------
extern SAVE_RESULT GFL_BACKUP_SAVEASYNC_EXTRA_Main(GFL_SAVEDATA * sv, GFL_SAVEDATA * extra_sv);

//---------------------------------------------------------------------------
/**
 * @brief	外部セーブのロード処理
 * @param	extra_sv			外部セーブデータ構造へのポインタ
 * @param heap_temp_id    この関数内でのみテンポラリとして使用するヒープID
 * @param link          内部セーブとのリンク情報
 * @retval	TRUE		読み込み成功
 * @retval	FALSE		読み込み失敗
 *
 * ロード処理はフラッシュアクセス時間が十分早いため、分割関数は用意していない
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_BACKUP_Extra_Load(GFL_SAVEDATA * sv, u32 heap_temp_id, u32 link);

//==================================================================
/**
 * GFL_BACKUP_SAVEASYNC_Mainで何バイト目までを書き込んだかを調べる
 *
 * @param   sv		
 *
 * @retval  u32		書き込みサイズ(Save_GetActualSizeのactual_size * 2(両面サイズ))
 */
//==================================================================
extern u32 GFL_BACKUP_SAVEASYNC_Main_WritingSize(GFL_SAVEDATA * sv);

//==================================================================
/**
 * 分割セーブ時、カードがロックされているか調べる
 *
 * @param   none		
 *
 * @retval  BOOL		TRUE:カードがロックされている
 * @retval  BOOL		FALSE:ロックされていない
 */
//==================================================================
extern BOOL GFL_BACKUP_SAVEASYNC_CheckCardLock(void);


//============================================================================================
//
//				セーブデータアクセス用関数
//
//============================================================================================
//==================================================================
/**
 * セーブデータシステム作成(セーブワークの実体を渡されたヒープで確保)
 *
 * @param   sv_param		
 * @param   heap_save_id		セーブシステムとセーブワーク実体の両方で使用
 *
 * @retval  GFL_SAVEDATA *		
 */
//==================================================================
extern GFL_SAVEDATA * GFL_SAVEDATA_Create(const GFL_SVLD_PARAM * sv_param, u32 heap_save_id);

//==================================================================
/**
 * セーブデータシステム作成(セーブワークの実体は外側で渡してもらう　※外部セーブでの使用を想定
 *
 * @param   sv_param		
 * @param   heap_save_id		セーブシステムで使用
 * @param   svwk_adrs       セーブワーク実体として使用するワークのポインタ
 * @param   svwk_size       svwk_adrsのサイズ
 * @param   svwk_clear      svwk_adrsがNULLでない場合、
 *                            TRUE:バッファの初期化を行う
 *                            FALSE:バッファの初期化は行わない
 *
 * @retval  GFL_SAVEDATA *		
 */
//==================================================================
extern GFL_SAVEDATA * GFL_SAVEDATA_CreateEx(const GFL_SVLD_PARAM * sv_param, 
  u32 heap_save_id, void *svwk_adrs, u32 svwk_size, BOOL svwk_clear );

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ解放処理
 * @param	sv		セーブデータへのポインタ
 */
//---------------------------------------------------------------------------
extern void GFL_SAVEDATA_Delete(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ初期化
 * @param	sv		セーブデータへのポインタ
 *
 * バックアップへの書き込みは行わず、データの初期化処理のみを行う
 */
//---------------------------------------------------------------------------
extern void GFL_SAVEDATA_Clear(GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	最初の読み込み時のセーブデータ状態取得
 * @param	sv		セーブデータへのポインタ
 * @return	LOAD_RESULT		ロード結果情報
 */
//---------------------------------------------------------------------------
extern LOAD_RESULT GFL_SAVEDATA_GetLoadResult(const GFL_SAVEDATA * sv);

//---------------------------------------------------------------------------
/**
 * @brief	データが存在するかどうかの判別フラグ取得
 * @param	sv		セーブデータへのポインタ
 * @retval	TRUE	存在する
 * @retval	FALSE	存在しない
 */
//---------------------------------------------------------------------------
extern BOOL GFL_SAVEDATA_GetExistFlag(const GFL_SAVEDATA * sv);

//==================================================================
/**
 * セーブデータワークの先頭アドレスを取得
 * @param   sv		セーブデータ構造へのポインタ
 * @param   size  実セーブデータサイズ
 * @retval  const void *		セーブデータワークの先頭アドレス
 */
//==================================================================
extern const void * GFL_SAVEDATA_GetSaveWorkAdrs(GFL_SAVEDATA * sv, u32 *size);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータの各要素取得
 * @param	gmdataID	データＩＤ
 * @return	void * として各要素へのポインタを返す
 */
//---------------------------------------------------------------------------
extern void * GFL_SAVEDATA_Get(GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID);

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータの各要素取得
 * @param	gmdataID	データＩＤ
 * @return	void * として各要素へのポインタを返す
 *
 * 読み込み専用として扱いたい場合にGFL_SAVEDATA_Getと切り分けて使用する
 */
//---------------------------------------------------------------------------
extern const void * GFL_SAVEDATA_GetReadOnly(const GFL_SAVEDATA * sv, GFL_SVDT_ID gmdataID);

//--------------------------------------------------------------
/**
 * セーブを実行した場合のセーブサイズを取得する
 *
 * @param   sv		        セーブデータ構造へのポインタ
 * @param   actual_size		セーブされる実サイズ(CRCテーブルなどのシステム系のデータは除きます)
 * @param   total_size		セーブ全体のサイズ(CRCテーブルなどのシステム系のデータは除きます)
 *
 * 差分比較の為、フラッシュアクセスします。
 */
//--------------------------------------------------------------
extern void GFL_SAVEDATA_GetActualSize(GFL_SAVEDATA *sv, u32 *actual_size, u32 *total_size);

//--------------------------------------------------------------
/**
 * @brief   ブロックのCRCを現在のデータで更新する
 *
 * @param   sv				セーブデータ構造へのポインタ
 * @param   gmdataid		セーブデータID
 *
 * @retval  生成されたCRC値
 */
//--------------------------------------------------------------
extern u16 GFL_BACKUP_BlockCRC_Set(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid);

//==================================================================
/**
 * フラッシュ直接書き込み：一括書き込み
 *
 * @param	flash_address		フラッシュのアドレス（０〜）※セクタ指定ではない
 * @param	src         		書き込むデータのアドレス
 * @param	len		          書き込むデータの長さ
 * @return	BOOL	TRUEで成功、FALSEで失敗
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashSave(u32 flash_address, const void * src, u32 len);

//==================================================================
/**
 * フラッシュ直接書き込み：分割書き込み：初期化
 *
 * @param	flash_address		フラッシュのアドレス（０〜）※セクタ指定ではない
 * @param	src         		書き込むデータのアドレス
 * @param	len		          書き込むデータの長さ
 * @return	u16 lock_id
 */
//==================================================================
extern u16 GFL_BACKUP_DirectFlashSaveAsyncInit(u32 flash_address, const void * src, u32 len);

//==================================================================
/**
 * フラッシュ直接書き込み：分割書き込み：メイン
 *
 * @param   lock_id		    
 * @param   err_result		処理完了後　TRUE:正常に書き込んで終了　FALSE:エラーで終了
 *
 * @retval  BOOL		      TRUE:処理完了　FALSE:処理継続中
 *
 * 戻り値がTRUEが返ってきたのを確認してから、err_resultでエラー判定をしてください
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashSaveAsyncMain(u16 lock_id, BOOL *err_result);

//==================================================================
/**
 * フラッシュ直接読み出し
 *
 * @param	src		フラッシュのアドレス（０〜）※セクタ指定ではない
 * @param	dst		読み込み先アドレス
 * @param	len		読み込むデータの長さ
 * @return	BOOL	TRUEで成功、FALSEで失敗
 */
//==================================================================
extern BOOL GFL_BACKUP_DirectFlashLoad(u32 src, void * dst, u32 len);

//==================================================================
/**
 * 指定IDの部分だけをフラッシュから直接ロードする
 *
 * @param   sv            
 * @param   gmdataid		  ロードする対象のID
 * @param   side_a_or_b		0:A面からロード　　1:B面からロード
 * @param   dst		        データ展開先
 * @param   load_size     ロードするバイトサイズ
 *
 * @retval  BOOL		TRUE:ロード成功　FALSE:ロード失敗
 */
//==================================================================
extern BOOL GFL_BACKUP_Page_FlashLoad(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, BOOL side_a_or_b, void *dst, u32 load_size);




//==============================================================================
//	デバッグ用関数
//==============================================================================
#ifdef PM_DEBUG
extern void DEBUG_GFL_BACKUP_BlockSaveFlagGet(const GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, BOOL crctable, BOOL footer, BOOL *ret_a, BOOL *ret_b);
extern void DEBUG_BACKUP_DataWrite(GFL_SAVEDATA *sv, GFL_SVDT_ID gmdataid, void *data, int write_offset, int size, int save_a, int save_b, int save_crc, int save_footer);
extern BOOL DEBUG_BACKUP_FlashSave(u32 flash_address, const void * src, u32 len);
extern BOOL DEBUG_BACKUP_FlashLoad(u32 src, void * dst, u32 len);
#endif


#ifdef __cplusplus
}/* extern "C" */
#endif


