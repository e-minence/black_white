//======================================================================
/**
 * @file	sta_act_script.h
 * @brief	ステージ スクリプト処理 ローカル定義
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================

#ifndef STA_ACT_SCRIPT_DEF_H__
#define STA_ACT_SCRIPT_DEF_H__

#include "system/vm_cmd.h"

#include "sta_act_script.h"
#include "../sta_acting.h"
#include "../sta_act_effect.h"
#include "../sta_act_poke.h"
#include "../sta_act_obj.h"
#include "../sta_act_light.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define SCRIPT_NUM (8)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
	SFT_CONTINUE = VMCMD_RESULT_CONTINUE,	//継続
	SFT_SUSPEND  = VMCMD_RESULT_SUSPEND,	//次フレームへ
	
}SCRIPT_FINISH_TYPE;

typedef enum
{
	SFB_IS_FINISH		= 1<<0,		//終了したか？
	SFB_IS_TARGET_SYNC	= 1<<1,		//同期待ちの対象にするか？
	SFB_WAIT_SYNC		= 1<<2,		//同期待ち中か？
}SCRIPT_FLAG_BIT;

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//スクリプト１本のデータ
typedef struct
{
	STA_SCRIPT_SYS *sysWork;
	VMHANDLE	*vmHandle;
	
	void	*scriptData;
	void	*loadPos;
	
	u32		frame;
	u16		waitCnt;
	u16		isFlag;
	
	u8		trgPokeFlg;
	
}STA_SCRIPT_WORK;

//スクリプト管理
struct _STA_SCRIPT_SYS
{
	HEAPID heapId;
	
	
	GFL_TCBSYS	*tcbSys;
	void		*tcbWork;
	STA_SCRIPT_WORK *scriptWork[SCRIPT_NUM];
	ACTING_WORK *actWork;
};


typedef SCRIPT_FINISH_TYPE(*STA_SCRIPT_FUNC)(STA_SCRIPT_SYS *work,STA_SCRIPT_WORK *scriptWork);


//--------------------------------------------------------------
//	スクリプト用機能関数
//--------------------------------------------------------------
void	STA_ACT_LoadBg( ACTING_WORK *work , const u8 bgNo );
void STA_ACT_ShowMessage( ACTING_WORK *work , const u16 msgNo , const u8 msgSpd );
void STA_ACT_HideMessage( ACTING_WORK *work );

//--------------------------------------------------------------
//	スクリプト用に外部提供関数
//--------------------------------------------------------------
STA_POKE_SYS*	STA_ACT_GetPokeSys( ACTING_WORK *work );
STA_POKE_WORK*	STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx );
STA_OBJ_SYS*	STA_ACT_GetObjectSys( ACTING_WORK *work );
STA_OBJ_WORK*	STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx );
STA_EFF_SYS*	STA_ACT_GetEffectSys( ACTING_WORK *work );
STA_EFF_WORK*	STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx );
void			STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx );
STA_LIGHT_SYS* 	STA_ACT_GetLightSys( ACTING_WORK *work );
STA_LIGHT_WORK* STA_ACT_GetLightWork( ACTING_WORK *work , const u8 idx );
void 			STA_ACT_SetLightWork( ACTING_WORK *work , STA_LIGHT_WORK *lightWork , const u8 idx );

u16		STA_ACT_GetCurtainHeight( ACTING_WORK *work );
void	STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height );

#endif STA_ACT_SCRIPT_DEF_H__
