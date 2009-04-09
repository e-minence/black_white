//======================================================================
/**
 * @file	sta_act_script.c
 * @brief	ステージ スクリプト処理
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "sta_act_script.h"
#include "sta_act_script_def.h"
#include "script_table.h"
//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define SCRIPT_TCB_TASK_NUM (16)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static BOOL STA_SCRIPT_UpdateScript( STA_SCRIPT_SYS *work , STA_SCRIPT_WORK *scriptWork );

//============================================================================================
/**
 *	VMイニシャライザテーブル
 */
//============================================================================================
static	const VM_INITIALIZER	vm_init={
	16,//BTLV_EFFVM_STACK_SIZE,				//u16 stack_size;	///<使用するスタックのサイズ
	8,//BTLV_EFFVM_REG_SIZE,				//u16 reg_size;		///<使用するレジスタの数
	STA_ScriptFuncArr,			//const VMCMD_FUNC * command_table;	///<使用する仮想マシン命令の関数テーブル
	SCRIPT_ENUM_MAX,			//const u32 command_max;			///<使用する仮想マシン命令定義の最大数
};


//--------------------------------------------------------------
//	
//--------------------------------------------------------------

STA_SCRIPT_SYS* STA_SCRIPT_InitSystem( HEAPID heapId ,ACTING_WORK *actWork)
{
	u8 i;
	STA_SCRIPT_SYS *work;
	work = GFL_HEAP_AllocMemory( heapId , sizeof( STA_SCRIPT_SYS ) );
	
	work->heapId = heapId;
	work->actWork = actWork;
	work->tcbWork = GFL_HEAP_AllocMemory( work->heapId , GFL_TCB_CalcSystemWorkSize(SCRIPT_TCB_TASK_NUM) );
	work->tcbSys = GFL_TCB_Init( SCRIPT_TCB_TASK_NUM , work->tcbWork );
	
	for( i=0;i<SCRIPT_NUM;i++ )
	{
		work->scriptWork[i] = NULL;
	}
	
	return work;
}

void STA_SCRIPT_ExitSystem( STA_SCRIPT_SYS *work )
{
	u8 i;
	for( i=0;i<SCRIPT_NUM;i++ )
	{
		if( work->scriptWork[i] != NULL )
		{
			GFL_HEAP_FreeMemory( work->scriptWork[i] );
			work->scriptWork[i] = NULL;
		}
	}

	GFL_TCB_Exit( work->tcbSys );
	GFL_HEAP_FreeMemory( work->tcbWork );
	GFL_HEAP_FreeMemory( work );
}

void STA_SCRIPT_UpdateSystem( STA_SCRIPT_SYS *work )
{
	u8 i;
	for( i=0;i<SCRIPT_NUM;i++ )
	{
		if( work->scriptWork[i] != NULL )
		{
			const BOOL ret = STA_SCRIPT_UpdateScript( work , work->scriptWork[i] );
			if( ret == TRUE )
			{
				GFL_HEAP_FreeMemory( work->scriptWork[i] );
				work->scriptWork[i] = NULL;
			}
		}
	}
	GFL_TCB_Main( work->tcbSys );
}

void STA_SCRIPT_SetScript( STA_SCRIPT_SYS *work , void *scriptData )
{
	u8 i;
	for( i=0;i<SCRIPT_NUM;i++ )
	{
		if( work->scriptWork[i] == NULL )
		{
			break;
		}
	}

	GF_ASSERT_MSG( i<SCRIPT_NUM , "Stage script is full!!\n" );
	
	work->scriptWork[i] = GFL_HEAP_AllocMemory( work->heapId , sizeof( STA_SCRIPT_WORK ) );
	work->scriptWork[i]->scriptData = scriptData;
	work->scriptWork[i]->loadPos = work->scriptWork[i]->scriptData;
	work->scriptWork[i]->waitCnt = 0;
	work->scriptWork[i]->frame = 0;
	work->scriptWork[i]->isFinish = FALSE;

	work->scriptWork[i]->sysWork = work;
	work->scriptWork[i]->vmHandle = VM_Create( work->heapId , &vm_init );
	VM_Init( work->scriptWork[i]->vmHandle , (void*)work->scriptWork[i] );
	VM_Start( work->scriptWork[i]->vmHandle , work->scriptWork[i]->scriptData );
}

static BOOL STA_SCRIPT_UpdateScript( STA_SCRIPT_SYS *work , STA_SCRIPT_WORK *scriptWork )
{
	
	if( scriptWork->waitCnt > 0 )
	{
		scriptWork->waitCnt--;
	}

	if( scriptWork->waitCnt == 0 )
	{
		VM_Control( scriptWork->vmHandle );
		if( scriptWork->isFinish == TRUE )
		{
			VM_End( scriptWork->vmHandle );
			VM_Delete( scriptWork->vmHandle );
			GFL_HEAP_FreeMemory( scriptWork->scriptData );
			return TRUE;
		}
	}
	scriptWork->frame++;
	return FALSE;
}

const u8 STA_SCRIPT_GetRunningScriptNum( STA_SCRIPT_SYS *work )
{
	u8 i;
	u8 num = 0;
	for( i=0;i<SCRIPT_NUM;i++ )
	{
		if( work->scriptWork[i] != NULL )
		{
			num++;
		}
	}
	return num;	
}
