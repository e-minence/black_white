
//============================================================================================
/**
 * @file	btl_effect.c
 * @brief	戦闘エフェクト制御
 * @author	soga
 * @date	2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "btl_effect.h"
#include "btl_efftool.h"
#include "btl_effect_def.h"

#include "arc_def.h"
#include "spa.naix"

//============================================================================================
/**
 *	定数宣言
 */
//============================================================================================

#define	BTL_EFFECT_TCB_MAX	( 10 )		//使用するTCBのMAX

//============================================================================================
/**
 *	構造体宣言
 */
//============================================================================================

struct _BTL_EFFECT_WORK
{
	//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
	//最終的にはエフェクトで使用するTCBをPOKE_MCSS、BTL_CAMERA、BTL_EFFECTでシェアする形にする
	GFL_TCBSYS			*tcb_sys;
	void				*tcb_work;
	VMHANDLE			*vm_core;
	POKE_MCSS_WORK		*pmw;
	BTL_STAGE_WORK		*bsw;
	BTL_FIELD_WORK		*bfw;
	BTL_CAMERA_WORK		*bcw;
	GFL_PTC_PTR			ptc[ PARTICLE_GLOBAL_MAX ];
	u16					ptc_no[ PARTICLE_GLOBAL_MAX ];
	int					execute_flag;
	HEAPID				heapID;
};

//============================================================================================
/**
 *	プロトタイプ宣言
 */
//============================================================================================

void			BTL_EFFECT_Init( int index, HEAPID heapID );
void			BTL_EFFECT_Exit( void );
void			BTL_EFFECT_Main( void );
void			BTL_EFFECT_Add( int eff_no );
BOOL			BTL_EFFECT_CheckExecute( void );
void			BTL_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position );
void			BTL_EFFECT_DelPokemon( int position );

BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( void );

static	BTL_EFFECT_WORK	*bew = NULL;

static	VMHANDLE	*BTL_EFFVM_Init( HEAPID heapID );
static	void		BTL_EFFVM_Exit( VMHANDLE *core );

//============================================================================================
/**
 *	システム初期化
 *
 * @param[in]	index	背景を決定するインデックスナンバー
 * @param[in]	heapID	ヒープID
 */
//============================================================================================
void	BTL_EFFECT_Init( int index, HEAPID heapID )
{
	GF_ASSERT( bew == NULL );
	bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_EFFECT_WORK ) );

	bew->heapID = heapID;

	//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
	//最終的にはエフェクトで使用するTCBをPOKE_MCSS、BTL_CAMERA、BTL_EFFECTでシェアする形にする
	bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTL_EFFECT_TCB_MAX ) );
	bew->tcb_sys = GFL_TCB_Init( BTL_EFFECT_TCB_MAX, bew->tcb_work );

	bew->vm_core = BTL_EFFVM_Init( heapID );

	bew->pmw = POKE_MCSS_Init( bew->tcb_sys, heapID );
	bew->bsw = BTL_STAGE_Init( index, heapID );
	bew->bfw = BTL_FIELD_Init( index, heapID );
	bew->bcw = BTL_CAMERA_Init( bew->tcb_sys, heapID );

	POKE_MCSS_SetOrthoMode( bew->pmw );

	//パーティクル初期化
	GFL_PTC_Init( heapID );
}

//============================================================================================
/**
 *	システム終了
 */
//============================================================================================
void	BTL_EFFECT_Exit( void )
{
	GF_ASSERT( bew != NULL );
	POKE_MCSS_Exit( bew->pmw );
	BTL_STAGE_Exit( bew->bsw );
	BTL_FIELD_Exit( bew->bfw );
	BTL_CAMERA_Exit( bew->bcw );
	GFL_PTC_Exit();
	BTL_EFFVM_Exit( bew->vm_core );
	GFL_TCB_Exit( bew->tcb_sys );
	GFL_HEAP_FreeMemory( bew->tcb_work );
	GFL_HEAP_FreeMemory( bew );
	bew = NULL;
}

//============================================================================================
/**
 *	システムメイン
 */
//============================================================================================
void	BTL_EFFECT_Main( void )
{
	if( bew == NULL ) return;

	GFL_TCB_Main( bew->tcb_sys );

	POKE_MCSS_Main( bew->pmw );
	BTL_STAGE_Main( bew->bsw );
	BTL_FIELD_Main( bew->bfw );
	BTL_CAMERA_Main( bew->bcw );

	//3D描画
	{
		GFL_G3D_DRAW_Start();
		GFL_G3D_DRAW_SetLookAt();
		{
			//ここの描画順番は、NitroSDKで描画されているもののあとにNitroSystemで描画されているものを置く
			//順番を混在させると正しく表示されず最悪フリーズする
			POKE_MCSS_Draw( bew->pmw );
			{
				s32	particle_count;

				particle_count = G3X_GetPolygonListRamCount();

				GFL_PTC_Main();

				particle_count = G3X_GetPolygonListRamCount() - particle_count;
			}
			BTL_STAGE_Draw( bew->bsw );
			BTL_FIELD_Draw( bew->bfw );
		}

		GFL_G3D_DRAW_End();
	}
}

//=============================================================================================
/**
 * エフェクト起動（発動位置、受動位置、ワザナンバーからエフェクトナンバーを自動判定）
 *
 * @param   from		発動位置
 * @param   to			受動位置
 * @param   waza		エフェクトナンバー
 *
 */
//=============================================================================================
void BTL_EFFECT_AddByPos( PokeMcssPos from, PokeMcssPos to, WazaID waza )
{
}

//============================================================================================
/**
 *	エフェクト起動
 *
 * @param[in]	eff_no	起動するエフェクトナンバー
 */
//============================================================================================
void	BTL_EFFECT_Add( int eff_no )
{
}

//============================================================================================
/**
 *	エフェクト起動中かチェック
 *
 * @retval FALSE:起動していない　TRUE:起動中
 */
//============================================================================================
BOOL	BTL_EFFECT_CheckExecute( void )
{
	return ( bew->execute_flag != 0 );
}

//============================================================================================
/**
 *	指定された立ち位置にポケモンをセット
 *
 * @param[in]	pp			セットするポケモンのPOKEMON_PARAM構造体へのポインタ
 * @param[in]	position	セットするポケモンの立ち位置
 */
//============================================================================================
void	BTL_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position )
{
	POKE_MCSS_Add( bew->pmw, pp, position );
}

//============================================================================================
/**
 *	指定された立ち位置のポケモンを削除
 *
 * @param[in]	position	削除するポケモンの立ち位置
 */
//============================================================================================
void	BTL_EFFECT_DelPokemon( int position )
{
	POKE_MCSS_Del( bew->pmw, position );
}

//============================================================================================
/**
 *	エフェクトで使用されているカメラ管理構造体のポインタを取得
 *
 * @retval bcw カメラ管理構造体
 */
//============================================================================================
BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( void )
{
	return bew->bcw;
}

//============================================================================================
/**
 *	スクリプト解析
 */
//============================================================================================
//============================================================================================
/**
 *	プロトタイプ宣言
 */
//============================================================================================
//スクリプトコマンド
static VMCMD_RESULT WS_CAMERA_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT WS_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT WS_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT WS_PARTICLE_PLAY_WITH_POS( VMHANDLE *vmh, void *context_work );

//VM_WAIT_FUNC群
static	BOOL	VWF_CAMERA_MOVE_CHECK( VMHANDLE *vmh, void *context_work );

//============================================================================================
/**
 *	スクリプトテーブル
 */
//============================================================================================
static const VMCMD_FUNC btl_effect_command_table[]={
	WS_CAMERA_MOVE,
	WS_PARTICLE_LOAD,
	WS_PARTICLE_PLAY,
	WS_PARTICLE_PLAY_WITH_POS,
};

//============================================================================================
/**
 *	VMイニシャライザーテーブル
 */
//============================================================================================
static	const VM_INITIALIZER	vm_init={
	BTL_EFFVM_STACK_SIZE,				//u16 stack_size;	///<使用するスタックのサイズ
	BTL_EFFVM_REG_SIZE,					//u16 reg_size;		///<使用するレジスタの数
	btl_effect_command_table,			//const VMCMD_FUNC * command_table;	///<使用する仮想マシン命令の関数テーブル
	NELEMS( btl_effect_command_table ),	//const u32 command_max;			///<使用する仮想マシン命令定義の最大数
};

//============================================================================================
/**
 *	VM初期化
 *
 * @param[in]	heapID			ヒープID
 */
//============================================================================================
VMHANDLE	*BTL_EFFVM_Init( HEAPID heapID )
{
	VMHANDLE	*core;

	core = VM_Create( heapID, &vm_init );
	VM_Init( core, NULL );

	return core;
}

//============================================================================================
/**
 *	VM終了
 *
 * @param[in]	core	仮想マシン制御構造体へのポインタ
 */
//============================================================================================
void	BTL_EFFVM_Exit( VMHANDLE *core )
{
	VM_Delete( core );
}

//============================================================================================
/**
 *	カメラ移動
 *
 * @param[in]	vmh				仮想マシン制御構造体へのポインタ
 * @param[in]	context_work	コンテキストワークへのポインタ
 */
//============================================================================================
static VMCMD_RESULT WS_CAMERA_MOVE( VMHANDLE *vmh, void *context_work )
{
	VecFx32		cam_pos,cam_target;
	//カメラタイプ読み込み
	int			cam_type = ( int )VMGetU32( vmh );
	int			frame,brake;

	//カメラ移動先位置を読み込み
	cam_pos.x = ( fx32 )VMGetU32( vmh );
	cam_pos.y = ( fx32 )VMGetU32( vmh );
	cam_pos.z = ( fx32 )VMGetU32( vmh );
	//カメラ移動先ターゲットを読み込み
	cam_target.x = ( fx32 )VMGetU32( vmh );
	cam_target.y = ( fx32 )VMGetU32( vmh );
	cam_target.z = ( fx32 )VMGetU32( vmh );
	//移動フレーム数を読み込み
	frame = ( int )VMGetU32( vmh );
	//ブレーキフレーム数を読み込み
	brake = ( int )VMGetU32( vmh );

	//カメラタイプから移動先を計算
	switch( cam_type ){
	case BTL_CAMERA_MOVE_INIT:			//初期位置
		BTL_CAMERA_GetDefaultCameraPosition( &cam_pos, &cam_target );
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &cam_pos, &cam_target, frame, brake );
		break;
	case BTL_CAMERA_MOVE_DIRECT:		//ダイレクト
		BTL_CAMERA_MoveCameraPosition( bew->bcw, &cam_pos, &cam_target );
		break;
	case BTL_CAMERA_MOVE_INTERPOLATION:	//追従
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &cam_pos, &cam_target, frame, brake );
		break;
	}
	VMCMD_SetWait( vmh, VWF_CAMERA_MOVE_CHECK );

	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	パーティクルリソースロード
 *
 * @param[in]	vmh				仮想マシン制御構造体へのポインタ
 * @param[in]	context_work	コンテキストワークへのポインタ
 */
//============================================================================================
static VMCMD_RESULT WS_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work )
{
	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	パーティクル再生
 *
 * @param[in]	vmh				仮想マシン制御構造体へのポインタ
 * @param[in]	context_work	コンテキストワークへのポインタ
 */
//============================================================================================
static VMCMD_RESULT WS_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work )
{
	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	パーティクル再生（座標指定あり）
 *
 * @param[in]	vmh				仮想マシン制御構造体へのポインタ
 * @param[in]	context_work	コンテキストワークへのポインタ
 */
//============================================================================================
static VMCMD_RESULT WS_PARTICLE_PLAY_WITH_POS( VMHANDLE *vmh, void *context_work )
{
	return VMCMD_RESULT_SUSPEND;
}

//VM_WAIT_FUNC群
//============================================================================================
/**
 *	カメラ移動終了チェック
 *
 * @param[in]	vmh				仮想マシン制御構造体へのポインタ
 * @param[in]	context_work	コンテキストワークへのポインタ
 */
//============================================================================================
static	BOOL	VWF_CAMERA_MOVE_CHECK( VMHANDLE *vmh, void *context_work )
{
	return ( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE );
}

//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
#if 0
//============================================================================================
/**
 *	エフェクトシーケンス
 */
//============================================================================================
static	void	BTL_EFFECT_TCB_A2BGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		target.y += FX32_ONE * 3;
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };

				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_B, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_D, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}
static	void	BTL_EFFECT_TCB_B2AGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_A, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_C, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_A2BMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target;

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCA, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };

				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
				target.y += FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_B, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_D, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_B2AMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target;

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCB, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };
				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
				target.y += FX32_ONE * 3;
				target.z -= FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_A, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_C, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}

//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
//============================================================================================
/**
 *	エフェクトシーケンス
 */
//============================================================================================
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };

				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_BB, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target,scale;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;
		scale.x = POKE_MCSS_GetPokeDefaultScale( bew->pmw, POKE_MCSS_POS_AA );
		scale.y = POKE_MCSS_GetPokeDefaultScale( bew->pmw, POKE_MCSS_POS_AA );

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_AA, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			POKE_MCSS_SetOrthoMode( bew->pmw );
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCAA, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };

				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
				target.y += FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_BB, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		POKE_MCSS_ResetOrthoMode( bew->pmw );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCBB, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };
				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
				target.y += FX32_ONE * 3;
				target.z -= FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_AA, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			TAYA_Printf("エフェクト終了直前シーケンス\n");
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			TAYA_Printf("エフェクト終了シーケンス\n");
			BTL_EFFECT_TCB_End( tcb, bet );
			POKE_MCSS_SetOrthoMode( bew->pmw );
		}
		break;
	}
}

static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_AA );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_BB );
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.y += FX32_ONE * 2;
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_BB );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_AA );
	src.y += FX32_ONE * 3;
	dst.x -= FX32_ONE * 1;
	dst.y += FX32_HALF;
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCA( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_A );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_BB );
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
#if 0
	dst.y += FX32_ONE * 2;
#else
	dst.x = FX_CosIdx( 0xd480 );
	dst.y = FX_SinIdx( 0x0a00 );
	dst.z = FX_SinIdx( 0xd480 );
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCB( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_B );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_AA );
	src.y += FX32_ONE * 2;
#if 0
	dst.x -= FX32_ONE * 2;
	dst.y += FX32_ONE + FX32_HALF;
#else
	dst.x = FX_CosIdx( 0x5500 );
	dst.y = FX_SinIdx( 0x0800 );
	dst.z = FX_SinIdx( 0x5500 );
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet )
{
	if( bew->execute_flag != 0 ){
		GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_ON );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_ON );
	}
	bew->execute_flag = 0;
	GFL_HEAP_FreeMemory( bet );
	GFL_TCB_DeleteTask( tcb );
}
#endif

//管理構造体のポインタを返すバージョン
#if 0
//============================================================================================
/**
 *	構造体宣言
 */
//============================================================================================

struct _BTL_EFFECT_WORK
{
	//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
	//最終的にはエフェクトで使用するTCBをPOKE_MCSS、BTL_CAMERA、BTL_EFFECTでシェアする形にする
	GFL_TCBSYS			*tcb_sys;
	void				*tcb_work;
	POKE_MCSS_WORK		*pmw;
	BTL_STAGE_WORK		*bsw;
	BTL_FIELD_WORK		*bfw;
	BTL_CAMERA_WORK		*bcw;
	GFL_PTC_PTR			ptc;
	u8					spa_work[ PARTICLE_LIB_HEAP_SIZE ];
	int					execute_flag;
	HEAPID				heapID;
};

typedef struct
{
	BTL_EFFECT_WORK *bew;
	void			*resource;
	int				seq_no;
	int				wait;
}BTL_EFFECT_TCB;

//============================================================================================
/**
 *	プロトタイプ宣言
 */
//============================================================================================

BTL_EFFECT_WORK	*BTL_EFFECT_Init( int index, HEAPID heapID );
void			BTL_EFFECT_Exit( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_Main( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_Add( BTL_EFFECT_WORK *bew, int eff_no );
BOOL			BTL_EFFECT_CheckExecute( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_SetPokemon( BTL_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position );

BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( BTL_EFFECT_WORK *bew );

//暫定で作ったTCBVerの技エフェクトシーケンス
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet );
static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit );
static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit );



static	GFL_TCB_FUNC * const btl_effect_tcb_table[]={
	&BTL_EFFECT_TCB_AA2BBGanseki,
	&BTL_EFFECT_TCB_BB2AAGanseki,
	&BTL_EFFECT_TCB_AA2BBMizudeppou,
	&BTL_EFFECT_TCB_BB2AAMizudeppou,
};

//============================================================================================
/**
 *	システム初期化
 *
 * @param[in]	index	背景を決定するインデックスナンバー
 * @param[in]	heapID	ヒープID
 */
//============================================================================================
BTL_EFFECT_WORK	*BTL_EFFECT_Init( int index, HEAPID heapID )
{
	BTL_EFFECT_WORK *bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_EFFECT_WORK ) );

	bew->heapID = heapID;

	//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
	//最終的にはエフェクトで使用するTCBをPOKE_MCSS、BTL_CAMERA、BTL_EFFECTでシェアする形にする
	bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTL_EFFECT_TCB_MAX ) );
	bew->tcb_sys = GFL_TCB_Init( BTL_EFFECT_TCB_MAX, bew->tcb_work );

	bew->pmw = POKE_MCSS_Init( bew->tcb_sys, heapID );
	bew->bsw = BTL_STAGE_Init( index, heapID );
	bew->bfw = BTL_FIELD_Init( index, heapID );
	bew->bcw = BTL_CAMERA_Init( bew->tcb_sys, heapID );

	//パーティクル初期化
	GFL_PTC_Init( heapID );

	return bew;
}

//============================================================================================
/**
 *	システム終了
 *
 * @param[in]	bew	BTL_EFFECT管理ワークへのポインタ
 */
//============================================================================================
void	BTL_EFFECT_Exit( BTL_EFFECT_WORK *bew )
{
	POKE_MCSS_Exit( bew->pmw );
	BTL_STAGE_Exit( bew->bsw );
	BTL_FIELD_Exit( bew->bfw );
	BTL_CAMERA_Exit( bew->bcw );
	GFL_PTC_Exit();
	GFL_TCB_Exit( bew->tcb_sys );
	GFL_HEAP_FreeMemory( bew->tcb_work );
	GFL_HEAP_FreeMemory( bew );
}

//============================================================================================
/**
 *	システムメイン
 *
 * @param[in]	bew	BTL_EFFECT管理ワークへのポインタ
 */
//============================================================================================
void	BTL_EFFECT_Main( BTL_EFFECT_WORK *bew )
{
	GFL_TCB_Main( bew->tcb_sys );

	POKE_MCSS_Main( bew->pmw );
	BTL_STAGE_Main( bew->bsw );
	BTL_FIELD_Main( bew->bfw );
	BTL_CAMERA_Main( bew->bcw );

	//3D描画
	{
		GFL_G3D_DRAW_Start();
		GFL_G3D_DRAW_SetLookAt();
		{
			//ここの描画順番は、NitroSDKで描画されているもののあとにNitroSystemで描画されているものを置く
			//順番を混在させると正しく表示されず最悪フリーズする
			POKE_MCSS_Draw( bew->pmw );
			{
				s32	particle_count;

				particle_count = G3X_GetPolygonListRamCount();

				GFL_PTC_Main();

				particle_count = G3X_GetPolygonListRamCount() - particle_count;
			}
			BTL_STAGE_Draw( bew->bsw );
			BTL_FIELD_Draw( bew->bfw );
		}

		GFL_G3D_DRAW_End();
	}
}

//============================================================================================
/**
 *	エフェクト起動
 *
 * @param[in]	bew		BTL_EFFECT管理ワークへのポインタ
 * @param[in]	eff_no	起動するエフェクトナンバー
 */
//============================================================================================
void	BTL_EFFECT_Add( BTL_EFFECT_WORK *bew, int eff_no )
{
	BTL_EFFECT_TCB	*bet = GFL_HEAP_AllocMemory( bew->heapID, sizeof( BTL_EFFECT_TCB ) );

	bet->bew = bew;
	bet->seq_no = 0;
	bet->wait = 0;

	GFL_TCB_AddTask( bew->tcb_sys, btl_effect_tcb_table[ eff_no ], bet, 0 );

	bew->execute_flag = 1;
}

//============================================================================================
/**
 *	エフェクト起動中かチェック
 *
 * @param[in]	bew		BTL_EFFECT管理ワークへのポインタ
 *
 * @retval FALSE:起動していない　TRUE:起動中
 */
//============================================================================================
BOOL	BTL_EFFECT_CheckExecute( BTL_EFFECT_WORK *bew )
{
	return ( bew->execute_flag != 0 );
}

//============================================================================================
/**
 *	指定された立ち位置にポケモンをセット
 *
 * @param[in]	bew			BTL_EFFECT管理ワークへのポインタ
 * @param[in]	pp			セットするポケモンのPOKEMON_PARAM構造体へのポインタ
 * @param[in]	position	セットするポケモンの立ち位置
 *
 * @retval FALSE:起動していない　TRUE:起動中
 */
//============================================================================================
void	BTL_EFFECT_SetPokemon( BTL_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position )
{
	POKE_MCSS_Add( bew->pmw, pp, position );
}

//============================================================================================
/**
 *	エフェクトで使用されているカメラ管理構造体のポインタを取得
 *
 * @param[in]	bew			BTL_EFFECT管理ワークへのポインタ
 *
 * @retval bcw カメラ管理構造体
 */
//============================================================================================
BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( BTL_EFFECT_WORK *bew )
{
	return bew->bcw;
}

//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
//============================================================================================
/**
 *	エフェクトシーケンス
 */
//============================================================================================
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };

				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_B, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_D, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_waza_126_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 3, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 4, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 5, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 6, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 7, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_A, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_C, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target;

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCAA, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };

				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
				target.y += FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_B, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_D, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_B, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_D, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target;

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCBB, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };
				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
				target.y += FX32_ONE * 3;
				target.z -= FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_A, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_C, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_A, POKE_MCSS_ANM_STOP_OFF );
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_C, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet )
{
	bet->bew->execute_flag = 0;
	GFL_HEAP_FreeMemory( bet );
	GFL_TCB_DeleteTask( tcb );
}

static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_A );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_BB );
#if 0
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.y += FX32_ONE * 2;
#else
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.x = FX_SinIdx( 0 );
	dst.y = 0;
	dst.z = FX_CosIdx( 0 );
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_B );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_AA );
#if 0
	src.y += FX32_ONE * 2;
	dst.x -= FX32_ONE * 2;
	dst.y += FX32_ONE + FX32_HALF;
#else
	src.y += FX32_ONE * 2;
	dst.x = FX_SinIdx( 0 );
	dst.y = 0;
	dst.z = FX_CosIdx( 0 );
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}
//1vs1
#if 0
//本来はスクリプトエンジンを載せて、動作させるが、暫定でTCBを利用する
//============================================================================================
/**
 *	エフェクトシーケンス
 */
//============================================================================================
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_157_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };

				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_BB, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_157_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;
		scale.x = POKE_MCSS_GetPokeDefaultScale( bew->pmw, POKE_MCSS_POS_AA );
		scale.y = POKE_MCSS_GetPokeDefaultScale( bew->pmw, POKE_MCSS_POS_AA );

		if( bet->wait < 60 ){
			if( bet->wait == 10 ){
				VecFx32	rt_value = { 0, FX_F32_TO_FX32( -0.1f ), 0 };
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
				POKE_MCSS_MoveScale( bew->pmw, POKE_MCSS_POS_AA, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 1, 2, 50 );
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//カメラAAに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCAA, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };

				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
				target.y += FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_BB, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//カメラBBに移動
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//パーティクルセット
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCBB, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };
				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
				target.y += FX32_ONE * 3;
				target.z -= FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_AA, EFFTOOL_CALCTYPE_ROUNDTRIP, &rt_value, 2, 0, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//カメラデフォルト位置に移動
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet )
{
	bet->bew->execute_flag = 0;
	GFL_HEAP_FreeMemory( bet );
	GFL_TCB_DeleteTask( tcb );
}

static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_AA );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_BB );
#if 0
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.y += FX32_ONE * 2;
#else
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.x -= src.x;
	dst.y -= src.y;
	dst.z -= src.z;
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_BB );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_AA );
#if 0
	src.y += FX32_ONE * 4;
	dst.x -= FX32_ONE * 1;
	dst.y += FX32_HALF;
#else
	src.y += FX32_ONE * 4;
	dst.x -= src.x;
	dst.y -= src.y;
	dst.z -= src.z;
#endif
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}
#endif
#endif
