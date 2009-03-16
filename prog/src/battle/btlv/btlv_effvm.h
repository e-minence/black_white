
//======================================================================
/**
 * @file	btlv_effvm.h
 * @brief	戦闘エフェクト
 * @author	HisashiSogabe
 * @date	2009.02.12
 */
//======================================================================

#define __C_NO_DEF_

#include "system/vm.h"
#include "waza_tool/wazadata.h"
#include "btlv_effvm_def.h"

#ifndef __BTLV_EFFVM_H_
#define __BTLV_EFFVM_H_

#ifdef PM_DEBUG
//デバッグ用パーティクルデータ構造体定義
typedef struct
{
	int	particle_count;
	u8	adrs[1];
}DEBUG_PARTICLE_DATA;
#endif

extern	VMHANDLE	*BTLV_EFFVM_Init( HEAPID heapID );
extern	void		BTLV_EFFVM_Exit( VMHANDLE *core );
extern	void		BTLV_EFFVM_Start( VMHANDLE *core, BtlvMcssPos from, BtlvMcssPos to, WazaID waza );

#ifdef PM_DEBUG
extern	void		BTLV_EFFVM_StartDebug( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, const VM_CODE *start, const DEBUG_PARTICLE_DATA *dpd );
#endif

#endif //__BTLV_EFFECT_DEF_H_
