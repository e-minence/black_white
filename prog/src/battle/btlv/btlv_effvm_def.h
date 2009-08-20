//======================================================================
/**
 * @file	btlv_effvm_def.h
 * @brief	エフェクトシーケンス用のマクロ定義ファイル
 * @author	HisashiSogabe
 * @date	2009.02.12
 */
//======================================================================

//====================================================================================
/**
 *	定数定義
 */
//====================================================================================
#ifndef __BTLV_EFFVM_DEF_H_
#define __BTLV_EFFVM_DEF_H_

#define	BTLV_EFFVM_STACK_SIZE	( 16 )	///<使用するスタックのサイズ
#define	BTLV_EFFVM_REG_SIZE		( 8 )		///<使用するレジスタの数

//スクリプトアドレステーブル用定義
#define	BTLEFF_AA_SAME			( 0xffffffff )		//立ち位置AAと同じ
#define	BTLEFF_AA_REVERSE		( 0xfffffffe )		//立ち位置AAの反転
#define	BTLEFF_BB_SAME			( 0xfffffffd )		//立ち位置BBと同じ
#define	BTLEFF_BB_REVERSE		( 0xfffffffc )		//立ち位置BBの反転
#define	BTLEFF_A_SAME				( 0xfffffffb )		//立ち位置Aと同じ
#define	BTLEFF_A_REVERSE		( 0xfffffffa )		//立ち位置Aの反転
#define	BTLEFF_B_SAME				( 0xfffffff9 )		//立ち位置Bと同じ
#define	BTLEFF_B_REVERSE		( 0xfffffff8 )		//立ち位置Bの反転
#define	BTLEFF_C_SAME				( 0xfffffff7 )		//立ち位置Cと同じ
#define	BTLEFF_C_REVERSE		( 0xfffffff6 )		//立ち位置Cの反転
#define	BTLEFF_D_SAME				( 0xfffffff5 )		//立ち位置Dと同じ
#define	BTLEFF_D_REVERSE		( 0xfffffff4 )		//立ち位置Dの反転

//カメラ移動タイプ
#define	BTLEFF_CAMERA_MOVE_DIRECT										( 0 )		//ダイレクト
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION						( 1 )		//追従
#define	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	( 2 )		//追従（相対指定）

//カメラ移動先
#define	BTLEFF_CAMERA_POS_AA						( 0 )
#define	BTLEFF_CAMERA_POS_BB						( 1 )
#define	BTLEFF_CAMERA_POS_A							( 2 )
#define	BTLEFF_CAMERA_POS_B							( 3 )
#define	BTLEFF_CAMERA_POS_C							( 4 )
#define	BTLEFF_CAMERA_POS_D							( 5 )
#define	BTLEFF_CAMERA_POS_INIT					( 6 )
#define	BTLEFF_CAMERA_POS_ATTACK				( 7 )
#define	BTLEFF_CAMERA_POS_ATTACK_PAIR		( 8 )
#define	BTLEFF_CAMERA_POS_DEFENCE				( 9 )
#define	BTLEFF_CAMERA_POS_DEFENCE_PAIR	( 10 )

#define	BTLEFF_CAMERA_POS_NONE					( 0xffffffff )

//カメラゆれ
#define	BTLEFF_CAMERA_SHAKE_VERTICAL		( 0 )
#define	BTLEFF_CAMERA_SHAKE_HORIZON		  ( 1 )

//射影モード
#define	BTLEFF_CAMERA_PROJECTION_ORTHO					( 0 )
#define	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE		( 1 )

//パーティクル再生
#define	BTLEFF_PARTICLE_PLAY_POS_AA				( BTLEFF_CAMERA_POS_AA )
#define	BTLEFF_PARTICLE_PLAY_POS_BB				( BTLEFF_CAMERA_POS_BB )
#define	BTLEFF_PARTICLE_PLAY_POS_A				( BTLEFF_CAMERA_POS_A )
#define	BTLEFF_PARTICLE_PLAY_POS_B				( BTLEFF_CAMERA_POS_B )
#define	BTLEFF_PARTICLE_PLAY_POS_C				( BTLEFF_CAMERA_POS_C )
#define	BTLEFF_PARTICLE_PLAY_POS_D				( BTLEFF_CAMERA_POS_D )
#define	BTLEFF_PARTICLE_PLAY_SIDE_NONE		( BTLEFF_CAMERA_POS_INIT )
#define	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	( BTLEFF_CAMERA_POS_ATTACK )
#define	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	( BTLEFF_CAMERA_POS_DEFENCE )

//エミッタ移動
#define	BTLEFF_EMITTER_MOVE_NONE							( 0 )
#define	BTLEFF_EMITTER_MOVE_STRAIGHT					( 1 )
#define	BTLEFF_EMITTER_MOVE_CURVE							( 2 )
#define	BTLEFF_EMITTER_MOVE_CURVE_HALF				( 3 )

//ポケモン操作
#define	BTLEFF_POKEMON_SIDE_ATTACK				( 0 )		//攻撃側
#define	BTLEFF_POKEMON_SIDE_ATTACK_PAIR		( 1 )		//攻撃側ペア
#define	BTLEFF_POKEMON_SIDE_DEFENCE				( 2 )		//防御側
#define	BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	( 3 )		//防御側ペア
#define	BTLEFF_POKEMON_POS_AA							( 4 )
#define	BTLEFF_POKEMON_POS_BB							( 5 )
#define	BTLEFF_POKEMON_POS_A							( 6 )
#define	BTLEFF_POKEMON_POS_B							( 7 )
#define	BTLEFF_POKEMON_POS_C							( 8 )
#define	BTLEFF_POKEMON_POS_D							( 9 )
#define	BTLEFF_POKEMON_POS_E					    ( 10 )
#define	BTLEFF_POKEMON_POS_F							( 11 )
#define	BTLEFF_TRAINER_POS_AA					    ( 12 )
#define	BTLEFF_TRAINER_POS_BB					    ( 13 )
#define	BTLEFF_TRAINER_POS_A					    ( 14 )
#define	BTLEFF_TRAINER_POS_B					    ( 15 )
#define	BTLEFF_TRAINER_POS_C					    ( 16 )
#define	BTLEFF_TRAINER_POS_D					    ( 17 )

#define	BTLEFF_POKEMON_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_SCALE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_SCALE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_ROTATE_DIRECT					( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ROTATE_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG	( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_POKEMON_ALPHA_DIRECT					  ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_POKEMON_ALPHA_INTERPOLATION		( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP				( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG	  ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )
#define	BTLEFF_MEPACHI_ON											( MCSS_MEPACHI_ON )
#define	BTLEFF_MEPACHI_OFF										( MCSS_MEPACHI_OFF )
#define	BTLEFF_MEPACHI_MABATAKI								( 2 )
#define	BTLEFF_ANM_STOP												( MCSS_ANM_STOP_ON )
#define	BTLEFF_ANM_START											( MCSS_ANM_STOP_OFF )

//ポケモン円移動
#define BTLEFF_AXIS_X_L                       ( 0 )     //Ｘ軸左
#define BTLEFF_AXIS_X_R                       ( 1 )     //Ｘ軸右
#define BTLEFF_AXIS_Y_L                       ( 2 )     //Ｙ軸左
#define BTLEFF_AXIS_Y_R                       ( 3 )     //Ｙ軸右
#define BTLEFF_AXIS_Z_L                       ( 4 )     //Ｚ軸左
#define BTLEFF_AXIS_Z_R                       ( 5 )     //Ｚ軸右

#define BTLEFF_SHIFT_H_P                      ( 0 )     //シフトＨ＋
#define BTLEFF_SHIFT_H_M                      ( 1 )     //シフトＨ−
#define BTLEFF_SHIFT_V_P                      ( 2 )     //シフトＶ＋
#define BTLEFF_SHIFT_V_M                      ( 3 )     //シフトＶ−

//トレーナー操作
#define	BTLEFF_TRAINER_MOVE_DIRECT						( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_TRAINER_MOVE_INTERPOLATION			( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP					( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG		( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )


//SE再生プレーヤー定義
#define BTLEFF_SEPLAY_DEFAULT                 ( 0 )
#define BTLEFF_SEPLAY_SE1                     ( 1 )
#define BTLEFF_SEPLAY_SE2                     ( 2 )
#define BTLEFF_SEPLAY_PSG                     ( 3 )

//SEストッププレーヤー定義
#define BTLEFF_SESTOP_ALL                     ( 0 )
#define BTLEFF_SESTOP_SE1                     ( 1 )
#define BTLEFF_SESTOP_SE2                     ( 2 )
#define BTLEFF_SESTOP_PSG                     ( 3 )

//エフェクト終了待ち
#define	BTLEFF_EFFENDWAIT_ALL									( 0 )
#define	BTLEFF_EFFENDWAIT_CAMERA							( 1 )
#define	BTLEFF_EFFENDWAIT_PARTICLE						( 2 )
#define	BTLEFF_EFFENDWAIT_POKEMON							( 3 )
#define	BTLEFF_EFFENDWAIT_TRAINER							( 4 )
#define	BTLEFF_EFFENDWAIT_PALFADE_STAGE		  	( 5 )
#define	BTLEFF_EFFENDWAIT_PALFADE_FIELD		  	( 6 )
#define	BTLEFF_EFFENDWAIT_PALFADE_3D			    ( 7 )
#define	BTLEFF_EFFENDWAIT_PALFADE_EFFECT	    ( 8 )
#define BTLEFF_EFFENDWAIT_SEALL               ( 9 )
#define BTLEFF_EFFENDWAIT_SE1                 ( 10 )
#define BTLEFF_EFFENDWAIT_SE2                 ( 11 )

//制御モード
#define	BTLEFF_CONTROL_MODE_CONTINUE					( 0 )
#define	BTLEFF_CONTROL_MODE_SUSPEND						( 1 )

#define BTLEFF_FX32_SHIFT											( 12 )

//BGパレットフェード対象定義
#define BTLEFF_PAL_FADE_STAGE                 ( 0 )
#define BTLEFF_PAL_FADE_FIELD                 ( 1 )
#define BTLEFF_PAL_FADE_3D                    ( 2 )
#define BTLEFF_PAL_FADE_EFFECT                ( 3 )
#define BTLEFF_PAL_FADE_ALL                   ( 4 )

//背景で使用されているパレットをビットで表現
#define BTLEFF_PAL_FADE_EFFECT_BIT            ( 0xff00 )

//BGプライオリティ設定
#define BTLEFF_BG_PRI_FRONT                   ( 0 )
#define BTLEFF_BG_PRI_BACK                    ( 1 )

//BGスクロール
#define	BTLEFF_BG_SCROLL_DIRECT						    ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_BG_SCROLL_INTERPOLATION			  ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_BG_SCROLL_ROUNDTRIP					  ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_BG_SCROLL_ROUNDTRIP_LONG		    ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

//BGラスタースクロール
#define	BTLEFF_BG_RASTER_SCROLL_H						  ( 0 )
#define	BTLEFF_BG_RASTER_SCROLL_V						  ( 1 )

//BGα値
#define	BTLEFF_BG_ALPHA_DIRECT					      ( EFFTOOL_CALCTYPE_DIRECT )
#define	BTLEFF_BG_ALPHA_INTERPOLATION		      ( EFFTOOL_CALCTYPE_INTERPOLATION )
#define	BTLEFF_BG_ALPHA_ROUNDTRIP				      ( EFFTOOL_CALCTYPE_ROUNDTRIP )
#define	BTLEFF_BG_ALPHA_ROUNDTRIP_LONG	      ( EFFTOOL_CALCTYPE_ROUNDTRIP_LONG )

//BG表示/非表示
#define	BTLEFF_STAGE			( 0 )
#define	BTLEFF_FIELD			( 1 )
#define	BTLEFF_EFFECT			( 2 )

#define	BTLEFF_VANISH_OFF	( 0 )
#define	BTLEFF_VANISH_ON	( 1 )

#endif //__BTLV_EFFVM_DEF_H_

//====================================================================================
/**
 *	VisualStudioで作成したエフェクトエディタツールでも使用されるファイルです 
 *	各命令マクロ定義のコメントからツールで表示されるコマンド名や引数の型などを判断します
 */
//====================================================================================

#if 0

ex)
 * #param_num	5
 * @param	type			カメラ移動タイプ
 * @param	move_pos	移動先
 * @param	frame			カメラタイプが追従のとき、何フレームで移動先に到達するかを指定
 * @param	wait			移動ウエイト
 * @param	brake			ブレーキをかけはじめるフレームを指定
 *
 * #param	COMBOBOX_TEXT	ダイレクト	追従
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_DIRECT	BTLEFF_CAMERA_MOVE_INTERPOLATION
 * #param	COMBOBOX_TEXT	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D	初期位置	攻撃側	防御側
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D	BTLEFF_CAMERA_POS_INIT BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_DEFENCE
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT

#param_numを書いて、コマンドの引数の数を宣言をします(@paramの総数になります）
doxygen書式の@paramはそのままツール上のコメントとして表示されます
#paramを書くことで、ツールで読み込むパラメータであることの宣言をします
パラメータの型を記述します
・COMBOBOX_TEXT
　コンボボックス型にして、コンボボックスで表示するリストデータを列記します

・COMBOBOX_VALUE
　先に設定したコンボボックス型のインデックスに対応して実際のシーケンスソースに書き出されるラベル定義などを記述します

・VALUE_FX32
　FX32型の値（ツール上では、小数ありの数値入力を可能にします）
	型の宣言の後ろに、数値入力ダイアログ表示用のラベルを記述できます

・VALUE_INT
	INT型の値（ツール上では、小数なしの数値入力になります）
	型の宣言の後ろに、数値入力ダイアログ表示用のラベルを記述できます

・VALUE_VECFX32
	VecFx32型の値　型の宣言の後ろにタブ区切りで、数値入力ダイアログ表示用のラベルを記述できます
ex)
	VALUE_VECFX32	camPosX	camPosY	camPosZ
	
	こうすることによって、ツールのダイアログに、入力するための値の意味を表示させることができます
 
・VALUE_ANGLE
	角度入力用の値（0〜360）
  コンバータで（0〜0xffff）に変換します

・FILE_DIALOG
　ファイルダイアログを表示してファイル選択を促す　選択を絞る拡張子を指定できます

・FILE_DIALOG_COMBOBOX
　ファイルダイアログで選択したファイルをコンボボックスで表示します　選択を絞る拡張子を指定できます

・COMBOBOX_HEADER
　FILE_DIALOG_COMBOBOXで選択したファイルの拡張子.hファイルを読み込んでコンボボックスを表示します

#endif

//命令シンボル宣言
//COMMAND_START
#define	EC_CAMERA_MOVE							( 0 )
#define	EC_CAMERA_MOVE_COORDINATE		( EC_CAMERA_MOVE              + 1 )
#define	EC_CAMERA_MOVE_ANGLE				( EC_CAMERA_MOVE_COORDINATE   + 1 )
#define	EC_CAMERA_SHAKE				      ( EC_CAMERA_MOVE_ANGLE        + 1 )
#define	EC_CAMERA_PROJECTION				( EC_CAMERA_SHAKE             + 1 )
#define	EC_PARTICLE_LOAD						( EC_CAMERA_PROJECTION        + 1 )
#define	EC_PARTICLE_PLAY						( EC_PARTICLE_LOAD            + 1 )
#define	EC_PARTICLE_PLAY_COORDINATE	( EC_PARTICLE_PLAY            + 1 )
#define	EC_PARTICLE_DELETE					( EC_PARTICLE_PLAY_COORDINATE + 1 )
#define	EC_EMITTER_MOVE							( EC_PARTICLE_DELETE          + 1 )
#define	EC_EMITTER_MOVE_COORDINATE	( EC_EMITTER_MOVE             + 1 )
#define	EC_POKEMON_MOVE							( EC_EMITTER_MOVE_COORDINATE  + 1 )
#define	EC_POKEMON_CIRCLE_MOVE			( EC_POKEMON_MOVE             + 1 )
#define	EC_POKEMON_SCALE						( EC_POKEMON_CIRCLE_MOVE      + 1 )
#define	EC_POKEMON_ROTATE						( EC_POKEMON_SCALE            + 1 )
#define	EC_POKEMON_ALPHA						( EC_POKEMON_ROTATE           + 1 )
#define	EC_POKEMON_SET_MEPACHI_FLAG	( EC_POKEMON_ALPHA            + 1 )
#define	EC_POKEMON_SET_ANM_FLAG			( EC_POKEMON_SET_MEPACHI_FLAG + 1 )
#define	EC_POKEMON_PAL_FADE					( EC_POKEMON_SET_ANM_FLAG     + 1 )
#define	EC_POKEMON_VANISH					  ( EC_POKEMON_PAL_FADE         + 1 )
#define	EC_POKEMON_SHADOW_VANISH	  ( EC_POKEMON_VANISH           + 1 )
#define	EC_TRAINER_SET							( EC_POKEMON_SHADOW_VANISH    + 1 )
#define	EC_TRAINER_MOVE							( EC_TRAINER_SET              + 1 )
#define	EC_TRAINER_ANIME_SET				( EC_TRAINER_MOVE             + 1 )
#define	EC_TRAINER_DEL							( EC_TRAINER_ANIME_SET        + 1 )
#define	EC_BG_LOAD	   				      ( EC_TRAINER_DEL              + 1 )
#define	EC_BG_SCROLL		   				  ( EC_BG_LOAD                  + 1 )
#define	EC_BG_RASTER_SCROLL		   	  ( EC_BG_SCROLL                + 1 )
#define	EC_BG_PAL_ANM		   				  ( EC_BG_RASTER_SCROLL         + 1 )
#define	EC_BG_PRIORITY		   				( EC_BG_PAL_ANM               + 1 )
#define	EC_BG_ALPHA		   				    ( EC_BG_PRIORITY              + 1 )
#define	EC_BG_PAL_FADE		   				( EC_BG_ALPHA                 + 1 )
#define	EC_BG_VANISH								( EC_BG_PAL_FADE              + 1 )
#define	EC_SE_PLAY									( EC_BG_VANISH                + 1 )
#define	EC_SE_STOP									( EC_SE_PLAY                  + 1 )
#define	EC_SE_PITCH									( EC_SE_STOP                  + 1 )
#define	EC_EFFECT_END_WAIT					( EC_SE_PITCH                 + 1 )
#define	EC_WAIT											( EC_EFFECT_END_WAIT          + 1 )
#define	EC_CONTROL_MODE							( EC_WAIT                     + 1 )

//終了コマンドは必ず一番下になるようにする
#define	EC_SEQ_END									( EC_CONTROL_MODE + 1 )

#ifndef __C_NO_DEF_

//勝手にパディングを入れられないようにする
	.option	alignment off

//命令マクロ定義
//======================================================================
/**
 * @brief		カメラ移動
 *
 * #param_num	5
 * @param	type			カメラ移動タイプ
 * @param	move_pos	移動先
 * @param	frame			カメラタイプが追従のとき、何フレームで移動先に到達するかを指定
 * @param	wait			移動ウエイト
 * @param	brake			ブレーキをかけはじめるフレームを指定
 *
 * #param	COMBOBOX_TEXT	追従	ダイレクト
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	初期位置	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_POS_ATTACK BTLEFF_CAMERA_POS_ATTACK_PAIR	BTLEFF_CAMERA_POS_DEFENCE BTLEFF_CAMERA_POS_DEFENCE_PAIR	BTLEFF_CAMERA_POS_INIT	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	VALUE_INT	移動フレーム数
 * #param	VALUE_INT	移動ウエイト
 * #param	VALUE_INT	ブレーキ開始フレーム
 */
//======================================================================
	.macro	CAMERA_MOVE	type, move_pos, frame, wait, brake
	.short	EC_CAMERA_MOVE
	.long		\type
	.long		\move_pos
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		カメラ移動（座標指定）
 *
 * #param_num	6
 * @param	type			カメラ移動タイプ
 * @param	move_pos	移動先位置
 * @param	move_tar	移動先ターゲット
 * @param	frame			カメラタイプが追従のとき、何フレームで移動先に到達するかを指定
 * @param	wait			移動ウエイト
 * @param	brake			ブレーキをかけはじめるフレームを指定
 *
 * #param	COMBOBOX_TEXT	追従	追従（相対指定）	ダイレクト
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_VECFX32	CamPosX	CamPosY	CamPosZ
 * #param	VALUE_VECFX32	CamTarX	CamTarY	CamTarZ
 * #param	VALUE_INT	移動フレーム数
 * #param	VALUE_INT	移動ウエイト
 * #param	VALUE_INT	ブレーキ開始フレーム
 */
//======================================================================
	.macro	CAMERA_MOVE_COODINATE	type, pos_x, pos_y, pos_z, tar_x, tar_y, tar_z, frame, wait, brake
	.short	EC_CAMERA_MOVE_COORDINATE
	.long		\type
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
	.long		\tar_x
	.long		\tar_y
	.long		\tar_z
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		カメラ移動（角度指定）
 *
 * #param_num	6
 * @param	type				カメラ移動タイプ
 * @param	angle_phi		移動先φ（追従のときは、現在値からの相対）
 * @param	angle_theta	移動先θ（追従のときは、現在値からの相対）
 * @param	frame				カメラタイプが追従のとき、何フレームで移動先に到達するかを指定
 * @param	wait				移動ウエイト
 * @param	brake				ブレーキをかけはじめるフレームを指定
 *
 * #param	COMBOBOX_TEXT	追従	ダイレクト
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_MOVE_INTERPOLATION	BTLEFF_CAMERA_MOVE_DIRECT
 * #param	VALUE_INT	移動先φ
 * #param	VALUE_INT	移動先θ
 * #param	VALUE_INT	移動フレーム数
 * #param	VALUE_INT	移動ウエイト
 * #param	VALUE_INT	ブレーキ開始フレーム
 */
//======================================================================
	.macro	CAMERA_MOVE_ANGLE	type, angle_phi, angle_theta, frame, wait, brake
	.short	EC_CAMERA_MOVE_ANGLE
	.long		\type
	.long		\angle_phi
	.long		\angle_theta
	.long		\frame
	.long		\wait
	.long		\brake
	.endm

//======================================================================
/**
 * @brief		カメラゆれ
 *
 * #param_num	6
 * @param	dir     ゆれ方向
 * @param	value   ゆれ幅
 * @param	offset	ゆれ幅オフセット
 * @param	frame   １ゆれにかかるフレーム
 * @param	wait    １フレーム毎のウエイト
 * @param	count   ゆれ回数
 *
 * #param	COMBOBOX_TEXT	縦ゆれ	横ゆれ
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_SHAKE_VERTICAL	BTLEFF_CAMERA_SHAKE_HORIZON
 * #param	VALUE_FX32	ゆれ幅
 * #param	VALUE_FX32	ゆれ幅オフセット（１ゆれごと）
 * #param	VALUE_INT	１ゆれにかかるフレーム
 * #param	VALUE_INT	１フレーム毎のウエイト
 * #param	VALUE_INT	ゆれ回数
 */
//======================================================================
	.macro	CAMERA_SHAKE	dir, value, offset, frame, wait, count
	.short	EC_CAMERA_SHAKE
	.long		\dir
	.long		\value
	.long		\offset
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief		射影モード
 *
 * #param_num	1
 * @param	type				射影タイプ
 *
 * #param	COMBOBOX_TEXT	正射影	透視射影
 * #param	COMBOBOX_VALUE	BTLEFF_CAMERA_PROJECTION_ORTHO	BTLEFF_CAMERA_PROJECTION_PERSPECTIVE
 */
//======================================================================
	.macro	CAMERA_PROJECTION	type
	.short	EC_CAMERA_PROJECTION
	.long		\type
	.endm

//======================================================================
/**
 * @brief	パーティクルデータロード
 *
 * #param_num	1
 * @param	datID	アーカイブデータのdatID
 *
 * #param	FILE_DIALOG_WITH_ADD	.spa	パーティクル再生	パーティクル再生（座標指定）	エミッタ移動
 */
//======================================================================
	.macro	PARTICLE_LOAD	datID
	.short	EC_PARTICLE_LOAD
	.long		\datID
	.endm

//======================================================================
/**
 * @brief	パーティクル再生
 *
 * #param_num	6
 * @param	num				再生パーティクルナンバー
 * @param	index			spa内インデックスナンバー
 * @param	start_pos	パーティクル再生開始立ち位置
 * @param	dir_pos		パーティクル再生方向立ち位置
 * @param	ofs_y			パーティクル再生Y方向オフセット
 * @param	dir_angle	パーティクル再生方向Y角度
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	攻撃側	防御側	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	COMBOBOX_TEXT	方向無し	攻撃側	防御側	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_NONE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_CAMERA_POS_AA	BTLEFF_CAMERA_POS_BB	BTLEFF_CAMERA_POS_A	BTLEFF_CAMERA_POS_B	BTLEFF_CAMERA_POS_C	BTLEFF_CAMERA_POS_D
 * #param	VALUE_FX32	パーティクル再生Y方向オフセット
 * #param	VALUE_FX32	パーティクル再生方向Y角度
 */
//======================================================================
	.macro	PARTICLE_PLAY	num, index, start_pos, dir_pos, ofs_y, dir_angle
	.short	EC_PARTICLE_PLAY
	.long		\num
	.long		\index
	.long		\start_pos
	.long		\dir_pos
	.long		\ofs_y
	.long		\dir_angle
	.endm

//======================================================================
/**
 * @brief	パーティクル再生（座標指定）
 *
 * #param_num	6
 * @param	num				再生パーティクルナンバー
 * @param	index			spa内インデックスナンバー
 * @param	start_pos	パーティクル再生開始位置
 * @param	dir_pos		パーティクル再生方向位置
 * @param	ofs_y			パーティクル再生Y方向オフセット
 * @param	dir_angle	パーティクル再生方向Y角度
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	VALUE_VECFX32	POS_X	POS_Y	POS_Z
 * #param	VALUE_VECFX32	DIR_X	DIR_Y	DIR_Z
 * #param	VALUE_FX32	パーティクル再生Y方向オフセット
 * #param	VALUE_FX32	パーティクル再生方向Y角度
 */
//======================================================================
	.macro	PARTICLE_PLAY_COORDINATE	num, index, start_pos_x, start_pos_y, start_pos_z, dir_pos_x, dir_pos_y, dir_pos_z, ofs_y, dir_angle
	.short	EC_PARTICLE_PLAY_COORDINATE
	.long		\num
	.long		\index
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\dir_pos_x
	.long		\dir_pos_y
	.long		\dir_pos_z
	.long		\ofs_y
	.long		\dir_angle
	.endm

//======================================================================
/**
 * @brief	パーティクルデータ削除
 *
 * #param_num	1
 * @param	num			削除するパーティクルナンバー
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 */
//======================================================================
	.macro	PARTICLE_DELETE	num
	.short	EC_PARTICLE_DELETE
	.long		\num
	.endm

//======================================================================
/**
 * @brief	エミッタ移動
 *
 * #param_num	6
 * @param	num					再生パーティクルナンバー
 * @param	index				spa内インデックスナンバー
 * @param	move_type		移動タイプ（直線、放物線）
 * @param	start_pos		移動開始立ち位置
 * @param	end_pos			移動終了立ち位置
 * @param	move_param	立ち位置Y方向オフセット(ofs_y)	移動フレーム(move_frame)	放物線頂点（放物線時のみ）(top)
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	直線	放物線  放物線（半分）
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
 * #param	COMBOBOX_TEXT	攻撃側	防御側
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	COMBOBOX_TEXT	攻撃側	防御側
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE
 * #param	VALUE_VECFX32	立ち位置Y方向オフセット	移動フレーム	放物線頂点（放物線時のみ）
 */
//======================================================================
	.macro	EMITTER_MOVE	num, index, move_type, start_pos, end_pos, ofs_y, move_frame, top
	.short	EC_EMITTER_MOVE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.endm

//======================================================================
/**
 * @brief	エミッタ移動（座標指定）
 *
 * #param_num	6
 * @param	num					再生パーティクルナンバー
 * @param	index				spa内インデックスナンバー
 * @param	move_type		移動タイプ（直線、放物線）
 * @param	start_pos		移動開始座標
 * @param	end_pos			移動終了立ち位置
 * @param	move_param	立ち位置Y方向オフセット(ofs_y)	移動フレーム(move_frame)	放物線頂点（放物線時のみ）(top)
 *
 * #param	FILE_DIALOG_COMBOBOX .spa
 * #param	COMBOBOX_HEADER
 * #param	COMBOBOX_TEXT	直線	放物線  放物線（半分）
 * #param	COMBOBOX_VALUE	BTLEFF_EMITTER_MOVE_STRAIGHT	BTLEFF_EMITTER_MOVE_CURVE BTLEFF_EMITTER_MOVE_CURVE_HALF
 * #param	VALUE_VECFX32	X座標	Y座標	Z座標
 * #param	COMBOBOX_TEXT	攻撃側	防御側 POS_AA POS_BB POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE	BTLEFF_PARTICLE_PLAY_SIDE_ATTACK	BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE	BTLEFF_PARTICLE_PLAY_POS_AA	BTLEFF_PARTICLE_PLAY_POS_BB	BTLEFF_PARTICLE_PLAY_POS_A	BTLEFF_PARTICLE_PLAY_POS_B	BTLEFF_PARTICLE_PLAY_POS_C	BTLEFF_PARTICLE_PLAY_POS_D
 * #param	VALUE_VECFX32	立ち位置Y方向オフセット	移動フレーム	放物線頂点（放物線時のみ）
 */
//======================================================================
	.macro	EMITTER_MOVE_COORDINATE	num, index, move_type, start_pos_x, start_pos_y, start_pos_z, end_pos, ofs_y, move_frame, top
	.short	EC_EMITTER_MOVE_COORDINATE
	.long		\num
	.long		\index
	.long		\move_type
	.long		\start_pos_x
	.long		\start_pos_y
	.long		\start_pos_z
	.long		\end_pos
	.long		\ofs_y
	.long		\move_frame
	.long		\top
	.endm

//======================================================================
/**
 * @brief	ポケモン移動
 *
 * #param_num	7
 * @param	pos					移動させるポケモンの立ち位置
 * @param	type				移動タイプ
 * @param	move_pos_x	移動先X座標
 * @param	move_pos_y	移動先Y座標
 * @param	frame				移動フレーム数（目的地まで何フレームで到達するか）
 * @param	wait				移動ウエイト
 * @param	count				往復カウント（移動タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_MOVE_DIRECT	BTLEFF_POKEMON_MOVE_INTERPOLATION	BTLEFF_POKEMON_MOVE_ROUNDTRIP	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	移動先X座標
 * #param	VALUE_FX32	移動先Y座標
 * #param	VALUE_INT		移動フレーム数
 * #param	VALUE_INT		移動ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	POKEMON_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_POKEMON_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	ポケモン円移動
 *
 * #param_num	7
 * @param	pos				      	円移動させるポケモンの立ち位置
 * @param	axis			      	回転軸
 * @param	shift			      	回転シフト
 * @param	radius_h	        横方向半径
 * @param	radius_v	        縦方向半径
 * @param	rotate_param			回転フレーム数（1回転何フレームでするか）:移動ウエイト:回転カウント
 * @param	rotate_after_wait 1回転したあとのウエイト（2回転以上で意味のある値です）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	Ｘ軸左  Ｘ軸右  Ｙ軸左  Ｙ軸右  Ｚ軸左  Ｚ軸右
 * #param	COMBOBOX_VALUE	BTLEFF_AXIS_X_L BTLEFF_AXIS_X_R BTLEFF_AXIS_Y_L BTLEFF_AXIS_Y_R BTLEFF_AXIS_Z_L BTLEFF_AXIS_Z_R
 * #param	COMBOBOX_TEXT シフトＨ＋  シフトＨ−  シフトＶ＋  シフトＶ−
 * #param	COMBOBOX_VALUE	BTLEFF_SHIFT_H_P BTLEFF_SHIFT_H_M BTLEFF_SHIFT_V_P BTLEFF_SHIFT_V_M
 * #param	VALUE_FX32	横方向半径
 * #param	VALUE_FX32	縦方向半径
 * #param	VALUE_VECFX32		回転フレーム数  移動ウエイト  回転カウント
 * #param	VALUE_INT 1回転したあとのウエイト（2回転以上で意味のある値です）
 */
//======================================================================
	.macro	POKEMON_CIRCLE_MOVE	pos, axis, shift, radius_h, radius_v, frame, rotate_wait, count, rotate_after_wait
	.short	EC_POKEMON_CIRCLE_MOVE
	.long		\pos
	.long		\axis
	.long		\shift
	.long		\radius_h
	.long		\radius_v
	.long		\frame
	.long		\rotate_wait
	.long		\count
	.long		\rotate_after_wait
	.endm

//======================================================================
/**
 * @brief	ポケモン拡縮
 *
 * #param_num	7
 * @param	pos			拡縮させるポケモンの立ち位置
 * @param	type		拡縮タイプ
 * @param	scale_x	X方向拡縮率
 * @param	scale_y	Y方向拡縮率
 * @param	frame		拡縮フレーム数（設定した拡縮値まで何フレームで到達するか）
 * @param	wait		拡縮ウエイト
 * @param	count		往復カウント（拡縮タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SCALE_DIRECT	BTLEFF_POKEMON_SCALE_INTERPOLATION	BTLEFF_POKEMON_SCALE_ROUNDTRIP	BTLEFF_POKEMON_SCALE_ROUNDTRIP_LONG
 * #param	VALUE_FX32	X方向拡縮率
 * #param	VALUE_FX32	Y方向拡縮率
 * #param	VALUE_INT		拡縮フレーム数
 * #param	VALUE_INT		拡縮ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	POKEMON_SCALE	pos, type, scale_x, scale_y, frame, wait, count
	.short	EC_POKEMON_SCALE
	.long		\pos
	.long		\type
	.long		\scale_x
	.long		\scale_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	ポケモン回転
 *
 * #param_num	6
 * @param	pos			回転させるポケモンの立ち位置
 * @param	type		回転タイプ
 * @param	rotate	回転角度
 * @param	frame		回転フレーム数（設定した回転値まで何フレームで到達するか）
 * @param	wait		回転ウエイト
 * @param	count		往復カウント（回転タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ROTATE_DIRECT	BTLEFF_POKEMON_ROTATE_INTERPOLATION	BTLEFF_POKEMON_ROTATE_ROUNDTRIP	BTLEFF_POKEMON_ROTATE_ROUNDTRIP_LONG
 * #param	VALUE_ANGLE	回転角度
 * #param	VALUE_INT		回転フレーム数（設定した回転値まで何フレームで到達するか）
 * #param	VALUE_INT		回転ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	POKEMON_ROTATE	pos, type, rotate, frame, wait, count
	.short	EC_POKEMON_ROTATE
	.long		\pos
	.long		\type
	.long		\rotate
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	ポケモンα値
 *
 * #param_num	6
 * @param	pos			α値セットするポケモンの立ち位置
 * @param	type		α値タイプ
 * @param	alpha	  α値
 * @param	frame		フレーム数（設定したα値まで何フレームで到達するか）
 * @param	wait		ウエイト
 * @param	count		往復カウント（α値タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A POS_B POS_C POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK  BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR  BTLEFF_POKEMON_POS_AA BTLEFF_POKEMON_POS_BB BTLEFF_POKEMON_POS_A  BTLEFF_POKEMON_POS_B  BTLEFF_POKEMON_POS_C  BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_ALPHA_DIRECT	BTLEFF_POKEMON_ALPHA_INTERPOLATION	BTLEFF_POKEMON_ALPHA_ROUNDTRIP	BTLEFF_POKEMON_ALPHA_ROUNDTRIP_LONG
 * #param	VALUE_INT	  α値
 * #param	VALUE_INT		フレーム数（設定したα値まで何フレームで到達するか）
 * #param	VALUE_INT		ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	POKEMON_ALPHA	pos, type, alpha, frame, wait, count
	.short	EC_POKEMON_ALPHA
	.long		\pos
	.long		\type
	.long		\alpha
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	ポケモンメパチ
 *
 * #param_num	4
 * @param	pos			メパチさせるポケモンの立ち位置
 * @param	type		メパチタイプ
 * @param	wait		まばたきウエイト（メパチタイプがまばたきのときだけ有効）
 * @param	count		まばたきカウント（メパチタイプがまばたきのときだけ有効）
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	閉じる	開ける	まばたき
 * #param	COMBOBOX_VALUE	BTLEFF_MEPACHI_ON	BTLEFF_MEPACHI_OFF	BTLEFF_MEPACHI_MABATAKI
 * #param	VALUE_INT	まばたきウエイト（まばたき時有効）
 * #param	VALUE_INT	まばたきカウント（まばたき時有効）
 */
//======================================================================
	.macro	POKEMON_SET_MEPACHI_FLAG	pos, type, wait, count
	.short	EC_POKEMON_SET_MEPACHI_FLAG
	.long		\pos
	.long		\type
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	ポケモンアニメ
 *
 * #param_num	2
 * @param	pos			アニメ操作するポケモンの立ち位置
 * @param	flag		アニメフラグ
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	アニメストップ	アニメスタート
 * #param	COMBOBOX_VALUE	BTLEFF_ANM_STOP	BTLEFF_ANM_START
 */
//======================================================================
	.macro	POKEMON_SET_ANM_FLAG	pos, flag
	.short	EC_POKEMON_SET_ANM_FLAG
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	ポケモンパレットフェード
 *
 * #param_num	5
 * @param	pos				パレットフェードするポケモンの立ち位置
 * @param	start_evy	START_EVY値
 * @param	end_evy		END_EVY値
 * @param	wait			ウエイト
 * @param	rgb				開始or終了時の色
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	VALUE_INT	START_EVY値
 * #param	VALUE_INT	END_EVY値
 * #param	VALUE_INT	ウエイト
 * #param	VALUE_VECFX32	R値(0-31)	G値(0-31)	B値(0-31)
 */
//======================================================================
	.macro	POKEMON_PAL_FADE	pos,	start_evy,	end_evy,	wait,	r, g, b
	.short	EC_POKEMON_PAL_FADE
	.long		\pos
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	ポケモンバニッシュ
 *
 * #param_num	2
 * @param	pos			バニッシュ操作するポケモンの立ち位置
 * @param	flag		バニッシュフラグ
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	バニッシュオン	バニッシュオフ
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_VANISH	pos, flag
	.short	EC_POKEMON_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	ポケモン影バニッシュ
 *
 * #param_num	2
 * @param	pos			バニッシュ操作するポケモンの立ち位置
 * @param	flag		バニッシュフラグ
 *
 * #param	COMBOBOX_TEXT	攻撃側	攻撃側ペア	防御側	防御側ペア	POS_AA	POS_BB	POS_A	POS_B	POS_C	POS_D POS_E POS_F POS_TR_AA POS_TR_BB POS_TR_A  POS_TR_B  POS_TR_C  POS_TR_D 
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR	BTLEFF_POKEMON_POS_AA	BTLEFF_POKEMON_POS_BB	BTLEFF_POKEMON_POS_A	BTLEFF_POKEMON_POS_B	BTLEFF_POKEMON_POS_C	BTLEFF_POKEMON_POS_D  BTLEFF_POKEMON_POS_E  BTLEFF_POKEMON_POS_F  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A BTLEFF_TRAINER_POS_B BTLEFF_TRAINER_POS_C BTLEFF_TRAINER_POS_D
 * #param	COMBOBOX_TEXT	バニッシュオン	バニッシュオフ
 * #param	COMBOBOX_VALUE	BTLEFF_VANISH_ON	BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	POKEMON_SHADOW_VANISH	pos, flag
	.short	EC_POKEMON_SHADOW_VANISH
	.long		\pos
	.long		\flag
	.endm

//======================================================================
/**
 * @brief	トレーナーセット
 *
 * #param_num	3
 * @param	trtype		セットするトレーナータイプ
 * @param	position	セットするトレーナーの立ち位置
 * @param	pos			  セットする座標(X:Y:Z)
 *
 * #param	VALUE_INT	トレーナータイプ
 * #param	COMBOBOX_TEXT POS_AA  POS_BB  POS_A POS_B POS_C POS_D
 * #param	COMBOBOX_VALUE  BTLEFF_TRAINER_POS_AA BTLEFF_TRAINER_POS_BB BTLEFF_TRAINER_POS_A  BTLEFF_TRAINER_POS_B  BTLEFF_TRAINER_POS_C  BTLEFF_TRAINER_POS_D
 * #param	VALUE_VECFX32	X座標 Y座標 Z座標
 */
//======================================================================
	.macro	TRAINER_SET	index, position, pos_x, pos_y, pos_z
	.short	EC_TRAINER_SET
	.long		\index
	.long		\position
	.long		\pos_x
	.long		\pos_y
	.long		\pos_z
	.endm

//======================================================================
/**
 * @brief	トレーナー移動
 *
 * #param_num	7
 * @param	index				移動させるトレーナーの立ち位置
 * @param	type				移動タイプ
 * @param	move_pos_x	移動先X座標
 * @param	move_pos_y	移動先Y座標
 * @param	frame				移動フレーム数（目的地まで何フレームで到達するか）
 * @param	wait				移動ウエイト
 * @param	count				往復カウント（移動タイプが往復のときだけ有効）
 *
 * #param	VALUE_INT		トレーナーの立ち位置
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_TRAINER_MOVE_DIRECT	BTLEFF_TRAINER_MOVE_INTERPOLATION	BTLEFF_TRAINER_MOVE_ROUNDTRIP	BTLEFF_TRAINER_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_INT		移動先X座標
 * #param	VALUE_INT		移動先Y座標
 * #param	VALUE_INT		移動フレーム数
 * #param	VALUE_INT		移動ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	TRAINER_MOVE	pos, type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_TRAINER_MOVE
	.long		\pos
	.long		\type
	.long		\move_pos_x
	.long		\move_pos_y
	.long		\frame
	.long		\wait
	.long		\count
	.endm

//======================================================================
/**
 * @brief	トレーナーアニメセット
 *
 * #param_num	2
 * @param	pos			アニメセットするトレーナーの立ち位置
 * @param	anm_no	セットするアニメナンバー
 *
 * #param	VALUE_INT		トレーナーの立ち位置
 * #param	VALUE_INT		アニメナンバー
 */
//======================================================================
	.macro	TRAINER_ANIME_SET	pos, anm_no
	.short	EC_TRAINER_ANIME_SET
	.long		\pos
	.long		\anm_no
	.endm

//======================================================================
/**
 * @brief	トレーナー削除
 *
 * #param_num	1
 * @param	index		削除するトレーナーの立ち位置
 *
 * #param	VALUE_INT		トレーナーの立ち位置
 */
//======================================================================
	.macro	TRAINER_DEL	pos
	.short	EC_TRAINER_DEL
	.long		\pos
	.endm

//======================================================================
/**
 * @brief	BGのロード
 *
 * #param_num	1
 * @param	datID	アーカイブデータのdatID
 *
 * #param	FILE_DIALOG	.NCGR
 */
//======================================================================
	.macro	BG_LOAD	datID
	.short	EC_BG_LOAD
	.long		\datID
	.endm

//======================================================================
/**
 * @brief	BGのスクロール
 *
 * #param_num	6
 * @param	type				移動タイプ
 * @param	move_pos_x	移動先X座標
 * @param	move_pos_y	移動先Y座標
 * @param	frame				移動フレーム数（目的地まで何フレームで到達するか）
 * @param	wait				移動ウエイト
 * @param	count				往復カウント（移動タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_BG_SCROLL_DIRECT	BTLEFF_BG_SCROLL_INTERPOLATION	BTLEFF_BG_SCROLL_ROUNDTRIP	BTLEFF_BG_SCROLL_ROUNDTRIP_LONG
 * #param	VALUE_INT		移動先X座標
 * #param	VALUE_INT		移動先Y座標
 * #param	VALUE_INT		移動フレーム数
 * #param	VALUE_INT		移動ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
	.macro	BG_SCROLL	type, move_pos_x, move_pos_y, frame, wait, count
	.short	EC_BG_SCROLL
	.long   \type
  .long   \move_pos_x
  .long   \move_pos_y
  .long   \frame
  .long   \wait
  .long   \count
	.endm

//======================================================================
/**
 * @brief	BGのラスタースクロール
 *
 * #param_num	4
 * @param	type		ラスタータイプ
 * @param	radius  サインカーブの半径
 * @param	line	  サインカーブ1周を何ラインにするか
 * @param	wait		ウエイト
 *
 * #param	COMBOBOX_TEXT 横  縦
 * #param	COMBOBOX_VALUE  BTLEFF_BG_RASTER_SCROLL_H BTLEFF_BG_RASTER_SCROLL_V
 * #param	VALUE_INT サインカーブの半径
 * #param	VALUE_INT サインカーブ1周を何ラインにするか
 * #param	VALUE_INT ウエイト
 */
//======================================================================
  .macro  BG_RASTER_SCROLL  type, radius, line, wait
  .short  EC_BG_RASTER_SCROLL
  .long   \type
  .long   \radius
  .long   \line
  .long   \wait
  .endm

//======================================================================
/**
 * @brief	BGのパレットアニメ
 *
 * #param_num	5
 * @param	datID     アニメ用パレットのアーカイブデータID
 * @param	trans_pal 送り先パレット
 * @param	col_count 送るカラー数
 * @param	offset		送り元パレットの更新オフセット
 * @param	wait		  ウエイト
 *
 * #param	VALUE_INT アニメ用パレットのアーカイブデータID
 * #param	VALUE_INT 送り先パレット
 * #param	VALUE_INT 送るカラー数
 * #param	VALUE_INT 送り元パレットの更新オフセット
 * #param	VALUE_INT ウエイト
 */
//======================================================================
  .macro  BG_PAL_ANM  datID, trans_pal, col_count, offset, wait
  .short  EC_BG_PAL_ANM
  .long   \datID
  .long   \trans_pal
  .long   \col_count
  .long   \offset
  .long   \wait
  .endm

//======================================================================
/**
 * @brief	BGのプライオリティ
 *
 * #param_num	1
 * @param	pri	BGのプライオリティ
 *
 * #param	COMBOBOX_TEXT 前面  背面
 * #param	COMBOBOX_VALUE BTLEFF_BG_PRI_FRONT  BTLEFF_BG_PRI_BACK
 */
//======================================================================
  .macro  BG_PRIORITY pri
  .short  EC_BG_PRIORITY
  .long   \pri
  .endm

//======================================================================
/**
 * @brief	BGα値
 *
 * #param_num	6
 * @param	bg_num	α値セットするBGの種類
 * @param	type		α値タイプ
 * @param	alpha	  α値
 * @param	frame		フレーム数（設定したα値まで何フレームで到達するか）
 * @param	wait		ウエイト
 * @param	count		往復カウント（α値タイプが往復のときだけ有効）
 *
 * #param	COMBOBOX_TEXT お盆  フィールド  お盆＋フィールド  エフェクト面  すべて
 * #param	COMBOBOX_VALUE  BTLEFF_PAL_FADE_STAGE BTLEFF_PAL_FADE_FIELD BTLEFF_PAL_FADE_3D  BTLEFF_PAL_FADE_EFFECT BTLEFF_PAL_FADE_ALL
 * #param	COMBOBOX_TEXT	ダイレクト	追従	往復	往復ロング
 * #param	COMBOBOX_VALUE	BTLEFF_BG_ALPHA_DIRECT	BTLEFF_BG_ALPHA_INTERPOLATION	BTLEFF_BG_ALPHA_ROUNDTRIP	BTLEFF_BG_ALPHA_ROUNDTRIP_LONG
 * #param	VALUE_INT	  α値
 * #param	VALUE_INT		フレーム数（設定したα値まで何フレームで到達するか）
 * #param	VALUE_INT		ウエイト
 * #param	VALUE_INT		往復カウント（往復時有効）
 */
//======================================================================
  .macro  BG_ALPHA  bg_num, type, alpha, frame, wait, count
  .short  EC_BG_ALPHA
  .long   \bg_num
  .long   \type
  .long   \alpha
  .long   \frame
  .long   \wait
  .long   \count
  .endm

//======================================================================
/**
 * @brief	BGのパレットフェード
 *
 * #param_num	5
 * @param	bg_num    操作する背景
 * @param	start_evy	START_EVY値
 * @param	end_evy		END_EVY値
 * @param	wait			ウエイト
 * @param	rgb				開始or終了時の色
 *
 * #param	COMBOBOX_TEXT お盆  フィールド  お盆＋フィールド  エフェクト面  すべて
 * #param	COMBOBOX_VALUE  BTLEFF_PAL_FADE_STAGE BTLEFF_PAL_FADE_FIELD BTLEFF_PAL_FADE_3D  BTLEFF_PAL_FADE_EFFECT BTLEFF_PAL_FADE_ALL
 * #param	VALUE_INT	START_EVY値
 * #param	VALUE_INT	END_EVY値
 * #param	VALUE_INT	ウエイト
 * #param	VALUE_VECFX32	R値(0-31)	G値(0-31)	B値(0-31)
 */
//======================================================================
	.macro	BG_PAL_FADE bg_num, start_evy, end_evy, wait, r, g, b
	.short	EC_BG_PAL_FADE
	.long		\bg_num
	.long		\start_evy
	.long		\end_evy
	.long		\wait
	.long		( ( ( \b >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 10 ) | ( ( ( \g >> BTLEFF_FX32_SHIFT ) & 0x1f ) << 5 ) | ( ( \r >> BTLEFF_FX32_SHIFT ) & 0x1f )
	.endm

//======================================================================
/**
 * @brief	BGの表示/非表示
 *
 * #param_num	2
 * @param	bg_num	操作する背景
 * @param	sw			表示/非表示を指定
 *
 * #param	COMBOBOX_TEXT	お盆  フィールド  エフェクト
 * #param	COMBOBOX_VALUE  BTLEFF_STAGE  BTLEFF_FIELD  BTLEFF_EFFECT
 * #param	COMBOBOX_TEXT 非表示  表示
 * #param	COMBOBOX_VALUE  BTLEFF_VANISH_ON BTLEFF_VANISH_OFF
 */
//======================================================================
	.macro	BG_VISIBLE	bg_num, sw
	.short	EC_BG_VANISH
	.long		\bg_num
	.long		\sw
	.endm

//======================================================================
/**
 * @brief	SE再生
 *
 * #param_num	7
 * @param	se_no	      再生するSEナンバー
 * @param player      再生するPlayerNo
 * @param wait        再生までのウエイト
 * @param pitch       再生ピッチ
 * @param vol         再生ボリューム
 * @param mod_depth   再生モジュレーションデプス
 * @param mod_speed   再生モジュレーションスピード
 *
 * #param	VALUE_INT   再生するSEナンバー
 * #param COMBOBOX_TEXT デフォルト  SE1 SE2 PSG
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_DEFAULT  BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2 BTLEFF_SEPLAY_PSG
 * #param VALUE_INT   再生までのウエイト
 * #param VALUE_INT   再生ピッチ
 * #param VALUE_INT   再生ボリューム
 * #param VALUE_INIT  127
 * #param VALUE_INT   再生モジュレーションデプス
 * #param VALUE_INT   再生モジュレーションスピード
 */
//======================================================================
	.macro	SE_PLAY	se_no, player, wait, pitch, vol, mod_depth, mod_speed
	.short	EC_SE_PLAY
	.long		\se_no
	.long   \player
	.long   \wait
  .long   \pitch
  .long   \vol
  .long   \mod_depth
  .long   \mod_speed
	.endm

//======================================================================
/**
 * @brief	SEストップ
 *
 * #param_num	1
 * @param player  ストップするPlayerNo
 *
 * #param COMBOBOX_TEXT すべて  SE1 SE2 PSG
 * #param COMBOBOX_VALUE BTLEFF_SESTOP_ALL  BTLEFF_SESTOP_SE1 BTLEFF_SESTOP_SE2 BTLEFF_SESTOP_PSG
 */
//======================================================================
	.macro	SE_STOP player
	.short	EC_SE_STOP
  .long   \player
	.endm

//======================================================================
/**
 * @brief	SEピッチ変更
 *
 * #param_num	1
 * @param player  変更するPlayerNo
 * @param start   開始ピッチ
 * @param end     終了ピッチ
 *
 * #param COMBOBOX_TEXT SE1 SE2
 * #param COMBOBOX_VALUE BTLEFF_SEPLAY_SE1 BTLEFF_SEPLAY_SE2
 */
//======================================================================
  .macro  SE_PITCH player, start, end
  .short  EC_SE_PITCH	
  .long   \player
  .long   \start
  .long   \end
  .endm

//======================================================================
/**
 * @brief	エフェクト終了待ち
 *
 * #param_num	1
 * @param	kind	終了待ちする種類
 *
 * #param	COMBOBOX_TEXT	すべて	カメラ	パーティクル	ポケモン	トレーナー  PALFADEお盆  PALFADEフィールド PALFADEお盆＋フィールド  PALFADEエフェクト SEPLAYすべて  SEPLAY1 SEPLAY2  
 * #param COMBOBOX_VALUE	BTLEFF_EFFENDWAIT_ALL	BTLEFF_EFFENDWAIT_CAMERA	BTLEFF_EFFENDWAIT_PARTICLE	BTLEFF_EFFENDWAIT_POKEMON	BTLEFF_EFFENDWAIT_TRAINER BTLEFF_EFFENDWAIT_PALFADE_STAGE  BTLEFF_EFFENDWAIT_PALFADE_FIELD  BTLEFF_EFFENDWAIT_PALFADE_3D BTLEFF_EFFENDWAIT_PALFADE_EFFECT BTLEFF_EFFENDWAIT_SEALL  BTLEFF_EFFENDWAIT_SE1  BTLEFF_EFFENDWAIT_SE2  
 */
//======================================================================
	.macro	EFFECT_END_WAIT	kind
	.short	EC_EFFECT_END_WAIT
	.long		\kind
	.endm

//======================================================================
/**
 * @brief	タイマーによるウエイト
 *
 * #param_num	1
 * @param		wait	ウエイト時間
 *
 * #param	VALUE_INT	ウエイト時間
 */
//======================================================================
	.macro	WAIT	wait
	.short	EC_WAIT
	.long		\wait
	.endm

//======================================================================
/**
 * @brief	制御モード変更
 *
 * #param_num	1
 * @param		mode	制御モード（継続：モードが切り替わるか、エフェクト終了まちまで処理を継続して実行　逐次：1行ごとに実行）
 *
 * #param	COMBOBOX_TEXT	継続	逐次
 * #param	COMBOBOX_VALUE	BTLEFF_CONTROL_MODE_CONTINUE	BTLEFF_CONTROL_MODE_SUSPEND
 */
//======================================================================
	.macro	CONTROL_MODE	mode
	.short	EC_CONTROL_MODE
	.long		\mode
	.endm

//======================================================================
/**
 * 	エフェクト終了はかならず最後になるようにする
 */
//======================================================================
//======================================================================
/**
 * @brief	エフェクト終了
 *
 * #param_num	0
 */
//======================================================================
	.macro	SEQ_END
	.short	EC_SEQ_END
	.endm

#endif
