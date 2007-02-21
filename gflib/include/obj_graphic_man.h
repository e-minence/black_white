//==============================================================================
/**
 *
 *@file		obj_graphic_man.h
 *@brief	OBJ�p�O���t�B�b�N�f�[�^�]���A�Ǘ��V�X�e��
 *@author	taya
 *@data		2006.11.28
 *
 */
//==============================================================================
#ifndef __OBJ_GRAPHIC_MAN_H__
#define __OBJ_GRAPHIC_MAN_H__

#include "clact.h"
#include "arc_tool.h"

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
//	�萔
//==============================================================
enum {
	GFL_OBJGRP_REGISTER_FAILED = 0xffffffff,
	GFL_OBJGRP_EXPLTT_OFFSET = 0x200,
};


//==============================================================
//	VRAM�w��p�V���{��
//==============================================================
typedef enum {
	GFL_VRAM_2D_MAIN = 1,
	GFL_VRAM_2D_SUB = 2,
	GFL_VRAM_2D_BOTH = GFL_VRAM_2D_MAIN | GFL_VRAM_2D_SUB,
}GFL_VRAM_TYPE;




extern void GFL_OBJGRP_sysInit( void );
extern void GFL_OBJGRP_sysExit( void );

extern void GFL_OBJGRP_sysStart(	u16 heapID, const GFL_BG_DISPVRAM* vramBank,
									const GFL_OBJGRP_INIT_PARAM* initParam );
extern void GFL_OBJGRP_sysEnd( void );

extern u32 GFL_OBJGRP_RegisterCGR( ARCHANDLE* arcHandle, u32 dataID, BOOL compressedFlag, 
	GFL_VRAM_TYPE targetVram, HEAPID heapID );

extern u32 GFL_OBJGRP_RegisterCGR_VramTransfer( ARCHANDLE* arcHandle, u32 dataID, BOOL compressedFlag, 
			GFL_VRAM_TYPE targetVram,  u32 cellIndex, HEAPID heapID );

extern void GFL_OBJGRP_ReleaseCGR( u32 index );

extern u32 GFL_OBJGRP_RegisterCellAnim( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, HEAPID heapID );
extern BOOL GFL_OBJGRP_CellBankHasVramTransferData( u32 index );
extern void GFL_OBJGRP_ReleaseCellAnim( u32 index );

extern u32 GFL_OBJGRP_RegisterPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u32 byteOffs, u16 heapID );
extern void GFL_OBJGRP_ReleasePltt( u32 index );

extern CLWK* GFL_OBJGRP_CreateClAct
	( CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const CLWK_DATA* param, u16 setSerface, u16 heapID );

extern CLWK* GFL_OBJGRP_CreateClActVT
	( CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const CLWK_DATA* param, u16 setSerface, u16 heapID );


#endif /* #ifndef __OBJMAN_H__ */
