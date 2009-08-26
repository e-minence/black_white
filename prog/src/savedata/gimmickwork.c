//============================================================================================
/**
 * @file	gimmickwork.c
 * @brief	マップ固有の仕掛け用ワークをあつかうためのソース
 * @author	saito
 * @date	2009.08.26
 *
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "gimmickwork_local.h"
#include "savedata/gimmickwork.h"

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークのサイズ取得
 * @return	int		サイズ
 */
//---------------------------------------------------------------------------
int GIMMICKWORK_GetWorkSize(void)
{
	return sizeof(GIMMICKWORK);
}

//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークの初期化
 * @param	gimmick		仕掛けワークへのポインタ
 */
//---------------------------------------------------------------------------
void GIMMICKWORK_Init(GIMMICKWORK * gimmick)
{
	MI_CpuClear8(gimmick, sizeof(GIMMICKWORK));
}

//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークの最初の認証処理
 * @param	gimmick		仕掛けワークへのポインタ
 * @param	gimmick_id		仕掛けのID
 */
//---------------------------------------------------------------------------
void * GIMMICKWORK_Assign(GIMMICKWORK * gimmick, int gimmick_id)
{
	GIMMICKWORK_Init(gimmick);
	gimmick->id = gimmick_id;
	return gimmick->work;
}

//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークの取得
 * @param	gimmick		仕掛けワークへのポインタ
 * @param	gimmick_id		使用するサイズ
 */
//---------------------------------------------------------------------------
void * GIMMICKWORK_Get(GIMMICKWORK * gimmick, int gimmick_id)
{
	GF_ASSERT(gimmick->id == gimmick_id);
	return gimmick->work;
}

//---------------------------------------------------------------------------
/**
 * @brief	仕掛け用ワークのID取得
 * @param	gimmick		仕掛けワークへのポインタ
 * @return	int			ID
 */
//---------------------------------------------------------------------------
int GIMMICKWORK_GetAssignID(const GIMMICKWORK * gimmick)
{
	return gimmick->id;
}

//----------------------------------------------------------
/**
 * @brief	データへのポインタ取得
 * @param	sv			セーブデータ保持ワークへのポインタ
 * @return	ギミックセーブデータへのポインタ
 */
//----------------------------------------------------------
GIMMICKWORK * SaveData_GetGimmickWork(SAVE_CONTROL_WORK * sv)
{
	return SaveControl_DataPtrGet(sv, GMDATA_ID_GIMMICK_WORK);
}


