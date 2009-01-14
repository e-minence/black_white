//==============================================================================
/**
 * @file	save_control.c
 * @brief	セーブコントロール関連
 * @author	matsuda
 * @date	2008.08.28(木)
 */
//==============================================================================
#include <gflib.h>
#include <backup_system.h>
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"


//==============================================================================
//	構造体定義
//==============================================================================
///セーブデータ管理ワーク構造体
struct _SAVE_CONTROL_WORK{
	BOOL new_data_flag;			///<TRUE:新規データ
	BOOL data_exists;			///<データが存在するかどうか
	BOOL total_save_flag;		///<TRUE:全体セーブ
	u32 first_status;			///<一番最初のセーブデータチェック結果(bit指定)
	GFL_SAVEDATA *sv_normal;	///<ノーマル用セーブデータへのポインタ
	GFL_SAVEDATA *sv_box;		///<ボックス用セーブデータへのポインタ
};

//==============================================================================
//	ローカル変数
//==============================================================================
///セーブデータへのポインタ
static SAVE_CONTROL_WORK *SaveControlWork = NULL;


//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ構造の初期化
 */
//---------------------------------------------------------------------------
SAVE_CONTROL_WORK * SaveControl_SystemInit(int heap_id)
{
	SAVE_CONTROL_WORK *ctrl;
	GFL_SAVEDATA * sv;
	LOAD_RESULT load_ret;

	ctrl = GFL_HEAP_AllocClearMemory(heap_id, sizeof(SAVE_CONTROL_WORK));
	SaveControlWork = ctrl;
	ctrl->new_data_flag = TRUE;			//新規データ
	ctrl->total_save_flag = TRUE;		//全体セーブ
	ctrl->data_exists = FALSE;			//データは存在しない
	ctrl->sv_normal = GFL_SAVEDATA_Create(&SaveParam_Normal);


	//データ存在チェックを行っている
	load_ret = GFL_BACKUP_Load(ctrl->sv_normal);
	ctrl->first_status = 0;
	switch(load_ret){
	case LOAD_RESULT_OK:				///<データ正常読み込み
		ctrl->total_save_flag = FALSE;	//全体セーブの必要はない
										//NGの場合はTRUEのままなので全体セーブになる
		//break;
		/* FALL THROUGH */
	case LOAD_RESULT_NG:				///<データ異常
		//まともなデータがあるようなので読み込む　OK or NG(正常読み出しかミラー読み出しが出来た)
		ctrl->new_data_flag = FALSE;		//新規データではない
		ctrl->data_exists = TRUE;			//データは存在する
		if(load_ret == LOAD_RESULT_NG){	//OKもNGも両方ここを通るので改めてチェック
			ctrl->first_status |= FST_NORMAL_NG_BIT;
		}

	#if 0	//※check　外部が出来るまで後回し
		//外部セーブの存在チェック
		{
			LOAD_RESULT frontier_result, video_result;
			
			ExtraNewCheckLoadData(sv, &frontier_result, &video_result);
			if(frontier_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= FST_FRONTIER_BREAK_BIT;
			}
			else if(frontier_result == LOAD_RESULT_NG){
				ctrl->first_status |= FST_FRONTIER_NG_BIT;
			}
			if(video_result == LOAD_RESULT_BREAK){
				ctrl->first_status |= FST_VIDEO_BREAK_BIT;
			}
			else if(video_result == LOAD_RESULT_NG){
				ctrl->first_status |= FST_VIDEO_NG_BIT;
			}
		}
	#endif
		break;
	case LOAD_RESULT_BREAK:			///<破壊、復旧不能
		OS_TPrintf("LOAD:データ破壊\n");
		ctrl->first_status |= FST_NORMAL_BREAK_BIT;
		//break;
		/* FALL THROUGH */
	case LOAD_RESULT_NULL:		///<データなし
	case LOAD_RESULT_ERROR:			///<機器故障(Flashが無い)などで読み取り不能
		//新規 or データ破壊なのでクリアする
		OS_TPrintf("LOAD:データが存在しない\n");
		GFL_SAVEDATA_Clear(ctrl->sv_normal);
		break;
	default:
		GF_ASSERT("LOAD:例外エラー！");
		break;
	}

	OS_TPrintf("first_status = %d\n", ctrl->first_status);
	
#if 0	//※check
#ifdef	PM_DEBUG
	{
		int i, rest;
		for (i = 0; i < SVBLK_ID_MAX; i++) {
			OS_TPrintf("[%d] ",i);
			OS_TPrintf("(%04x x %02d) - %05x = %05x\n",
					SEC_DATA_SIZE,sv->blkinfo[i].use_sec,
					sv->blkinfo[i].size,
					SEC_DATA_SIZE * sv->blkinfo[i].use_sec - sv->blkinfo[i].size);
		}
		rest = SECTOR_MAX - GetTotalSector(sv);
		if (rest > 0) {
			OS_TPrintf("%2d sector(0x%05x) left.\n", rest, 0x1000 * rest);
		}
	}
#endif
#endif

	return ctrl;
}

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ管理ワークへのポインタ取得
 * @return	SAVE_CONTROL_WORK	セーブデータ管理ワークへのポインタ
 *
 * 基本的にはセーブデータへのグローバル参照は避けたい。そのため、この関数を
 * 使用する箇所は厳重に制限されなければならない。できればプログラマリーダーの
 * 許可がなければ使用できないようにしたい。
 * 変なアクセスをしたら修正がかかります。使用方法には注意してください。
 */
//---------------------------------------------------------------------------
SAVE_CONTROL_WORK * SaveControl_GetPointer(void)
{
	GF_ASSERT(SaveControlWork != NULL);
	return SaveControlWork;
}

//--------------------------------------------------------------
/**
 * @brief   通常データのロード
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  ロード結果
 */
//--------------------------------------------------------------
LOAD_RESULT SaveControl_Load(SAVE_CONTROL_WORK *ctrl)
{
	LOAD_RESULT result = GFL_BACKUP_Load(ctrl->sv_normal);
	
	switch(result){
	case LOAD_RESULT_OK:
	case LOAD_RESULT_NG:
		ctrl->data_exists = TRUE;
		break;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   通常データのセーブ
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
SAVE_RESULT SaveControl_Save(SAVE_CONTROL_WORK *ctrl)
{
	SAVE_RESULT result;
	
	result = GFL_BACKUP_Save(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
		ctrl->new_data_flag = FALSE;
		ctrl->data_exists = TRUE;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   通常データの分割セーブ初期化
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
void SaveControl_SaveAsyncInit(SAVE_CONTROL_WORK *ctrl)
{
	GFL_BACKUP_SAVEASYNC_Init(ctrl->sv_normal);
}

//--------------------------------------------------------------
/**
 * @brief   通常データの分割セーブメイン処理
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
SAVE_RESULT SaveControl_SaveAsyncMain(SAVE_CONTROL_WORK *ctrl)
{
	SAVE_RESULT result;
	
	result = GFL_BACKUP_SAVEASYNC_Main(ctrl->sv_normal);
	if(result == SAVE_RESULT_OK){
		ctrl->new_data_flag = FALSE;
		ctrl->data_exists = TRUE;
	}
	return result;
}

//--------------------------------------------------------------
/**
 * @brief   通常セーブデータの各セーブワークのポインタを取得する
 *
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 * @param   gmdata_id	取得したいセーブデータのID
 *
 * @retval  セーブ領域へのポインタ
 */
//--------------------------------------------------------------
void * SaveControl_DataPtrGet(SAVE_CONTROL_WORK *ctrl, GFL_SVDT_ID gmdata_id)
{
	return GFL_SAVEDATA_Get(ctrl->sv_normal, gmdata_id);
}

//--------------------------------------------------------------
/**
 * @brief   新規データフラグを取得する
 *
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 *
 * @retval  TRUE:新規データである
 */
//--------------------------------------------------------------
BOOL SaveControl_NewDataFlagGet(SAVE_CONTROL_WORK *ctrl)
{
	return ctrl->new_data_flag;
}

//---------------------------------------------------------------------------
/**
 * @brief	最初の読み込み結果を返す
 * @param	sv			セーブデータ構造へのポインタ
 * @return	LOAD_RESULT	読み込み結果（savedata_def.h参照）
 */
//---------------------------------------------------------------------------
u32 SaveControl_GetLoadResult(const SAVE_CONTROL_WORK * sv)
{
	return sv->first_status;
}

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータ存在フラグを取得
 * @param	sv			セーブデータ構造へのポインタ
 * @return	BOOL		TRUEのとき、セーブデータが存在する
 */
//---------------------------------------------------------------------------
BOOL SaveData_GetExistFlag(const SAVE_CONTROL_WORK * sv)
{
	return sv->data_exists;
}

//---------------------------------------------------------------------------
/**
 * @brief	セーブデータの初期化
 * @param	sv			セーブデータ構造へのポインタ
 *
 * SaveControl_Eraseと違い、フラッシュに書き込まない。
 * セーブデータがある状態で「さいしょから」遊ぶ場合などの初期化処理
 */
//---------------------------------------------------------------------------
void SaveControl_ClearData(SAVE_CONTROL_WORK * ctrl)
{
	ctrl->new_data_flag = TRUE;				//新規データである
	ctrl->total_save_flag = TRUE;			//全体セーブする必要がある
	GFL_SAVEDATA_Clear(ctrl->sv_normal);
}

//--------------------------------------------------------------
/**
 * @brief   セーブデータの消去
 * @param   ctrl		セーブデータ管理ワークへのポインタ
 */
//--------------------------------------------------------------
void SaveControl_Erase(SAVE_CONTROL_WORK *ctrl)
{
	GFL_BACKUP_Erase(ctrl->sv_normal);
}


//==============================================================================
//	デバッグ用関数
//==============================================================================
#ifdef PM_DEBUG
GFL_SAVEDATA * DEBUG_SaveData_PtrGet(void)
{
	SAVE_CONTROL_WORK *ctrl = SaveControl_GetPointer();
	return ctrl->sv_normal;
}
#endif

