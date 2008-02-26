
//============================================================================================
/**
 * @file	fallchr.h
 * @brief	DS版ヨッシーのたまご落下キャラヘッダー
 * @date	2007.02.06
 */
//============================================================================================

#ifndef	__FALL_CHR_H__
#define	__FALL_CHR_H__

#undef GLOBAL
#ifdef __FALL_CHR_H_GLOBAL__
#define GLOBAL /*	*/
#else
#define GLOBAL extern
#endif

//----------------------------------------------------------------------------
/**
 *	@brief	落下キャラクタパラメータ構造体
 */
//-----------------------------------------------------------------------------
struct fall_chr_param{
	GAME_PARAM		*gp;
	GFL_CLWK		*clwk;
	GFL_CLACTPOS	now_pos;
	GFL_CLACTPOS	offset_pos;

	u8			seq_no;			//シーケンスナンバー
	u8			push_seq_no;	//シーケンスナンバー
	u8			player_no;		//プレーヤーナンバー
	u8			type;			//キャラの種類

	u8			line_no;		//現在いるライン
	u8			dir;			//キャラクタの向き
	u8			clact_no;		//セルアクターナンバー
	u8			fall_wait;

	int			speed_value;	//移動スピード格納ワーク（ROTATE、EGG_MAKEなど）
	int			height;			//高さ情報（タマゴで送られてきたとき用）

	u8			wait_value;		//ウエイト系の格納ワーク
	u8			fall_wait_tmp;
	u8			rotate_flag		:2;		//回転フラグ
	u8			overturn_flag	:1;		//ひっくり返しフラグ
	u8			egg_make_flag	:1;
	u8			ikari_flag		:1;		//いかりキャラフラグ
	u8							:3;		
	u8			chr_count;		//タマゴキャラの時のはさんでいるタマゴの数
	int			birth_wait;		//生まれるまでのウエイト
};

//----------------------------------------------------------------------------
/**
 *	@brief	落下キャラ初期化
 *	
 *	@param	gp			ゲームパラメータポインタ
 *	@param	player_no	1P or 2P
 *	@param	type		キャラタイプ
 *	@param	line_no		落下ラインナンバー
 *	@param	height		高さ情報
 *
 *	@retval FALL_CHR_PARAM
 */
//-----------------------------------------------------------------------------
GLOBAL	FALL_CHR_PARAM	*YT_InitFallChr(GAME_PARAM *gp,u8 player_no,u8 type,u8 line_no,int height);

//----------------------------------------------------------------------------
/**
 *	@brief	Network用落下キャラ生成
 *	
 *	@param	gp			ゲームパラメータポインタ
 *	@param	player_no	1P or 2P
 *	@param	type		キャラタイプ
 *	@param	line_no		落下ラインナンバー
 *	@param	height		高さ情報
 *
 *	@retval GFL_CLWK*
 */
//-----------------------------------------------------------------------------
GLOBAL GFL_CLWK* YT_InitNetworkFallChr(GAME_PARAM *gp,u8 player_no,u8 type,u8 line_no,int height);

//----------------------------------------------------------------------------
/**
 *	@brief	タマゴ生成チェック
 *	
 *	@param	ps		プレーヤーステータス
 */
//-----------------------------------------------------------------------------
GLOBAL	void	YT_EggMakeCheck(YT_PLAYER_STATUS *ps);


//----------------------------------------------------------------------------
/**
 *	@brief	ヨッシーの生まれるアニメーションタスクをセット
 *	
 *	@param	gs		ゲームパラメータポインタ
 *	@param	ps		プレーヤーステータス
 *	@param	pos_x		
 *	@param	pos_y		
 *	@param	count		
 */
//-----------------------------------------------------------------------------
GLOBAL void YT_YossyBirthAnimeTaskSet(GAME_PARAM *gp,YT_PLAYER_STATUS *ps,u8 pos_x,u8 pos_y,u8 count);

//----------------------------------------------------------------------------
/**
 *	@brief	ヨッシーが相手にキャラを送ってきた
 *	
 *	@param  player_no 相手側番号
 *	@param  num   追加数
 */
//-----------------------------------------------------------------------------
GLOBAL void YT_NetFallchrAddChar(GAME_PARAM *gp,u8 player_no, u8 num);

#endif	__FALL_CHR_H__
