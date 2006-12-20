//==============================================================================
/**
 *
 *@file		obj_graphic.h
 *@brief	OBJ�p�O���t�B�b�N�f�[�^�]���A�Ǘ��V�X�e��
 *@author	taya
 *@data		2006.11.28
 *
 */
//==============================================================================
#ifndef __OBJ_GRAPHIC_H__
#define __OBJ_GRAPHIC_H__

#include "gflib/display.h"	// VRAM�o���N�ݒ�iGF_BGL_DISPVRAM�j�Q�Ɨp�B
#include "gflib/vram.h"

#include "gflib/clact.h"

//==============================================================
//	�������p�\����
//==============================================================
typedef struct {
	u16  CGR_RegisterMax;			///< �o�^�ł���L�����f�[�^��
	u16  PLTT_RegisterMax;			///< �o�^�ł���p���b�g�f�[�^��
	u16  CELL_RegisterMax;			///< �o�^�ł���Z���A�j���p�^�[����
	u16  MULTICELL_RegisterMax;		///< �o�^�ł���}���`�Z���A�j���p�^�[�����i�����󖢑Ή��j
}GFL_OBJGRP_INIT_PARAM;


//==============================================================
//	�A�N�^�[�ǉ��p�\����
//==============================================================
typedef struct {
	s16	pos_x;				// �����W
	s16 pos_y;				// �����W
	u16 anmseq;				// �A�j���[�V�����V�[�P���X
	u8	softpri;			// �\�t�g�D�揇��	0>0xff
	u8	bgpri;				// BG�D�揇��
}GFL_OBJGRP_CLWKDAT;

//==============================================================
//	�萔
//==============================================================
enum {
	GFL_OBJGRP_REGISTER_FAILED = 0xffffffff,
};





extern void GFL_OBJGRP_sysInit( void );
extern void GFL_OBJGRP_sysExit( void );

extern void GFL_OBJGRP_sysStart( u32 heapID, const GF_BGL_DISPVRAM* vramBank, const GFL_OBJGRP_INIT_PARAM* initParam );
extern void GFL_OBJGRP_sysEnd( void );

extern u32 GFL_OBJGRP_RegisterCGR( void* dataPtr, GFL_VRAM_TYPE targetVram );
extern u32 GFL_OBJGRP_RegisterCGR_VramTransfer( void* dataPtr, GFL_VRAM_TYPE targetVram, u32 cellIndex );
extern void GFL_OBJGRP_ReleaseCGR( u32 index );

extern u32 GFL_OBJGRP_RegisterCellAnim( void* cellDataPtr, void* animDataPtr );
extern BOOL GFL_OBJGRP_CellBankHasVramTransferData( u32 index );
extern void GFL_OBJGRP_ReleaseCellAnim( u32 index );

extern u32 GFL_OBJGRP_RegisterPltt( void* plttData, GFL_VRAM_TYPE vramType, u32 byteOffs );
extern void GFL_OBJGRP_ReleasePltt( u32 index );

extern CLACT_WORK_PTR GFL_OBJGRP_CreateClAct( CLACT_SET_PTR actset, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex,
	const GFL_OBJGRP_CLWKDAT* param, u8 drawArea, u32 heapID );

#endif /* #ifndef __OBJMAN_H__ */
