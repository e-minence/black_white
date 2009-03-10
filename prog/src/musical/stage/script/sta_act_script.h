//======================================================================
/**
 * @file	sta_act_script.h
 * @brief	ステージ スクリプト処理
 * @author	ariizumi
 * @data	09/03/06
 */
//======================================================================

#ifndef STA_ACT_SCRIPT_H__
#define STA_ACT_SCRIPT_H__

#include "../sta_acting.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

typedef struct _STA_SCRIPT_SYS STA_SCRIPT_SYS;

//======================================================================
//	proto
//======================================================================

STA_SCRIPT_SYS *STA_SCRIPT_InitSystem( HEAPID heapId ,ACTING_WORK *actWork);
void STA_SCRIPT_ExitSystem( STA_SCRIPT_SYS *work );
void STA_SCRIPT_UpdateSystem( STA_SCRIPT_SYS *work );

void STA_SCRIPT_SetScript( STA_SCRIPT_SYS *work , void *scriptData );

#endif //STA_ACT_SCRIPT_H__
