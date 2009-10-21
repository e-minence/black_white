
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

#pragma once

#ifdef PM_DEBUG
//�f�o�b�O�p�p�[�e�B�N���f�[�^�\���̒�`
typedef struct
{
	int	particle_count;
	u8	adrs[1];
}DEBUG_PARTICLE_DATA;
#endif

typedef struct
{ 
  u8    waza_range;       ///<�Z�̌��ʔ͈�
  u8    turn_count;       ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
  u8    continue_count;   ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
  u8    yure_cnt;         ///<�{�[������J�E���g
  BOOL  get_success;      ///<�ߊl�������ǂ���
  u16   item_no;          ///<�{�[���̃A�C�e���i���o�[
}BTLV_EFFVM_PARAM;

extern	VMHANDLE	*BTLV_EFFVM_Init( GFL_TCBSYS *tcbsys, HEAPID heapID );
extern	BOOL  		BTLV_EFFVM_Main( VMHANDLE *vmh );
extern	void  		BTLV_EFFVM_Exit( VMHANDLE *core );
extern	void  		BTLV_EFFVM_Start( VMHANDLE *core, BtlvMcssPos from, BtlvMcssPos to, WazaID waza, BTLV_EFFVM_PARAM* param );
extern  void			BTLV_EFFVM_Stop( VMHANDLE *vmh );

#ifdef PM_DEBUG
extern	void		BTLV_EFFVM_StartDebug( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, const VM_CODE *start, const DEBUG_PARTICLE_DATA *dpd, BTLV_EFFVM_PARAM* param );
extern	void		BTLV_EFFVM_DebugParticlePlay( VMHANDLE *vmh, GFL_PTC_PTR ptc, int index, int src, int dst, fx32 ofs_y, fx32 angle );
extern  void    BTLV_EFFVM_DebugSeEffect( VMHANDLE *vmh, int player, int type, int param, int start, int end, 
                                          int start_wait, int frame, int wait, int count );
extern  void    BTLV_EFFVM_ClearUnpackInfo( VMHANDLE *vmh );
#endif

