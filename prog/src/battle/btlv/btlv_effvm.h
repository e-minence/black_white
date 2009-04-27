
//======================================================================
/**
 * @file	btlv_effvm.h
 * @brief	�퓬�G�t�F�N�g
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
//�f�o�b�O�p�p�[�e�B�N���f�[�^�\���̒�`
typedef struct
{
	int	particle_count;
	u8	adrs[1];
}DEBUG_PARTICLE_DATA;
#endif

extern	VMHANDLE	*BTLV_EFFVM_Init( GFL_TCBSYS *tcbsys, HEAPID heapID );
extern	BOOL		BTLV_EFFVM_Main( VMHANDLE *vmh );
extern	void		BTLV_EFFVM_Exit( VMHANDLE *core );
extern	void		BTLV_EFFVM_Start( VMHANDLE *core, BtlvMcssPos from, BtlvMcssPos to, WazaID waza );

#ifdef PM_DEBUG
extern	void		BTLV_EFFVM_StartDebug( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, const VM_CODE *start, const DEBUG_PARTICLE_DATA *dpd );
extern	void		BTLV_EFFVM_DebugParticlePlay( VMHANDLE *vmh, GFL_PTC_PTR ptc, int index, int src, int dst, fx32 ofs_y, fx32 angle );
#endif

#endif //__BTLV_EFFECT_DEF_H_
