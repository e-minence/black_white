
//=============================================================================================
/**
 * @file	sound.h
 * @brief	サウンドルーチン
 * @author	Hisashi Sogabe
 * @date	2007/04/17
 */
//=============================================================================================
#ifndef __SOUND_H_
#define __SOUND_H_

//--------------------------------------------------------------------------------------------
/**
 * サウンド初期化
 *
 * @param	sndHeap		サウンドヒープへのポインタ（静的に確保することを推奨）
 * @param	sndHeapSize	サウンドヒープのサイズ
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_Init( u8 *sndHeap,int sndHeapSize );

//--------------------------------------------------------------------------------------------
/**
 * サウンドメイン処理
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_Main( void );

//--------------------------------------------------------------------------------------------
/**
 * サウンド終了（呼ばれることはないだろうが、一応作成）
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_Exit( void );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータロード（アーカイブデータ）
 *
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadArchiveData( int arcID, int datID );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータロード（シーケンスデータ）
 *
 * @param	seq_no	シーケンスデータナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadSequenceData( int seq_no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータロード（シーケンスアーカイブデータ）
 *
 * @param	seqarc_no	シーケンスアーカイブデータナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadSequenceArchiveData( int seqarc_no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータロード（バンクデータ）
 *
 * @param	bank_no	バンクデータナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadBankData( int bank_no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータロード（グループデータ）
 *
 * @param	group_no	グループデータナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadGroupData( int group_no );

//--------------------------------------------------------------------------------------------
/**
 * シーケンスナンバーからプレイヤーナンバーを取得
 *
 * @param	no	シーケンスナンバー
 *
 * @retval	プレイヤーナンバー or 0xff=取得失敗
 */
//--------------------------------------------------------------------------------------------
extern	u8	GFL_SOUND_GetPlayerNo( u16 no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータ再生（BGM）
 *
 * @param	bgm_no	BGMナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_PlayBGM( int bgm_no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータStop（BGM）
 *
 * 今鳴っているBGMを止める
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_StopBGM( void );

//--------------------------------------------------------------------------------------------
/**
 * サウンドデータ再生（SE）
 *
 * @param	se_no	BGMナンバー
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_PlaySE( int se_no );

//--------------------------------------------------------------
/**
 * @brief	player_noを指定して再生中のシーケンスを止める
 *
 * @param	player_no	playerナンバー
 */
//--------------------------------------------------------------
extern	void	GFL_SOUND_StopPlayerNo( u8 player_no );

//--------------------------------------------------------------
/**
 * @brief	player_noを指定して再生中のシーケンスの数を返す
 *
 * @param	player_no	playerナンバー
 *
 * @retval	"再生中のシーケンスの数"
 */
//--------------------------------------------------------------
extern	int		GFL_SOUND_CheckPlaySeq( u8 player_no );

//--------------------------------------------------------------------------------------------
/**
 * サウンドヒープ階層ロード
 *
 * @param	level	ロードする階層レベル
 */
//--------------------------------------------------------------------------------------------
extern	void	GFL_SOUND_LoadHeapState(int level);

//--------------------------------------------------------------------------------------------
/**
 * サウンドヒープ階層セーブ
 *
 * @retval	保存した階層レベル（-1でエラー）
 */
//--------------------------------------------------------------------------------------------
extern	int	GFL_SOUND_SaveHeapState(void);

//--------------------------------------------------------------------------------------------
/**
 * プレイヤーナンバーを指定して一時停止
 *
 * @param	playerNo	プレイヤー番号
 * @param	flg			一時停止フラグ ( TRUE = PAUSE )
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_SOUND_PausePlayerNo( int playerNo, BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * 音程を変更
 *
 * @param	playerNo	プレイヤー番号
 * @param	trbitmask	トラックビットマスク
 * @param	pitch		音程変化の値（-32768〜32767）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_SOUND_SetPitch( int playerNo, u16 trbitmask, int pitch );

//--------------------------------------------------------------------------------------------
/**
 * 音程を変更
 *
 * @param	playerNo	プレイヤー番号
 * @param	trbitmask	トラックビットマスク
 * @param	pan			定位変化の値（-128〜127）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern	void GFL_SOUND_SetPan( int playerNo, u16 trbitmask, int pan );

//--------------------------------------------------------------------------------------------
/**
 * ボリュームを変更
 *
 * @param	playerNo	プレイヤー番号
 * @param	volume		ボリュームの値（0〜127）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_SOUND_SetVolume( int playerNo, int volume);

//--------------------------------------------------------------------------------------------
/**
 * ボリュームを取得
 *
 * @param	playerNo	プレイヤー番号
 * @param	volume		取得したボリュームの値を格納するポインタ（0〜127）
 *
 * @return	TRUE:取得成功　FALSE:取得失敗
 */
//--------------------------------------------------------------------------------------------
BOOL GFL_SOUND_GetVolume( int playerNo, int *volume);

#endif
