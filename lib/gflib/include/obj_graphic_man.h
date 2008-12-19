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

#include <clact.h>
#include <arc_tool.h>

#ifdef __cplusplus
extern "C" {
#endif

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




//-----------------------------------------------------------------------------------------------------------
/**
 *	�V�X�e�����������I������
 *
 *	�v���O�����N����ɂP�񂾂��R�[�����܂��B
 *	�A�v���P�[�V�������x���ł͌Ăяo���K�v������܂���B
 */
//-----------------------------------------------------------------------------------------------------------
extern void GFL_OBJGRP_Init( void );
extern void GFL_OBJGRP_Exit( void );

//-----------------------------------------------------------------------------------------------------------
/**
 *	�A�v���P�[�V�����P�ʂ̏��������I������
 *
 *	�A�v���P�[�V�������ƂɌĂяo���Ă��������B
 *	�����ŕK�v�ȃ��������m�ہ^������܂��B
 */
//-----------------------------------------------------------------------------------------------------------
extern void GFL_OBJGRP_sysStart( HEAPID heapID, const GFL_DISP_VRAM* vramBank, const GFL_OBJGRP_INIT_PARAM* initParam );
extern void GFL_OBJGRP_sysEnd( void );


extern u32 GFL_OBJGRP_RegisterCGR( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, HEAPID heapID );
extern u32 GFL_OBJGRP_RegisterCGR_VramTransfer( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, u32 cellIndex, HEAPID heapID );
extern u32 GFL_OBJGRP_CopyCGR_VramTransfer( u32 srcCgrIdx, u32 cellAnimIdx, GFL_VRAM_TYPE targetVram );
extern void GFL_OBJGRP_ReloadCGR_VramTransfer( u32 index, ARCHANDLE* arc, u32 cgrDatIdx, BOOL compressedFlag, HEAPID heapID );
extern void* GFL_OBJGRP_GetVramTransCGRPointer( u32 index );
extern void GFL_OBJGRP_ReleaseCGR( u32 index );

extern u32 GFL_OBJGRP_RegisterCellAnim( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, HEAPID heapID );
extern void GFL_OBJGRP_ReleaseCellAnim( u32 index );
extern BOOL GFL_OBJGRP_CellBankHasVramTransferData( u32 index );

//-----------------------------------------------------------------------------------------------------------
//
//	�p���b�g�o�^�֐�
//
//�EGFL_OBJGRP_RegisterPltt
//		�񈳏k�p���b�g�f�[�^��]���B�S�]�����܂��B
//
//�EGFL_OBJGRP_RegisterPlttEx
//		�񈳏k�p���b�g�f�[�^��]���B
//		�]����o�C�g�I�t�Z�b�g�A�]�����ǂݍ��݊J�n�ʒu�A�]���{�����w��ł��܂��B
//
//�EGFL_OBJGRP_RegisterCompPltt
//		���k�p���b�g�f�[�^��]���B
//		NitroCharacter�ŕҏW�����ʂ�̈ʒu�ɓ]�����܂��B
//		�]����o�C�g�I�t�Z�b�g���w�肵�Ă��炷���Ƃ��o���܂��B
//
//-----------------------------------------------------------------------------------------------------------
extern u32 GFL_OBJGRP_RegisterPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, HEAPID heapID );
extern u32 GFL_OBJGRP_RegisterPlttEx( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, u16 srcStartLine, u16 numTransLines, HEAPID heapID );
extern u32 GFL_OBJGRP_RegisterCompPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, HEAPID heapID );

extern void GFL_OBJGRP_ReleasePltt( u32 index );

extern void GFL_OBJGRP_GetCGRProxy( u32 index, NNSG2dImageProxy* proxy );
extern void GFL_OBJGRP_GetPlttProxy( u32 index, NNSG2dImagePaletteProxy* proxy );

//-----------------------------------------------------------------------------------------------------------
//
//	�Z���A�N�^�[�����֐�
//
//-----------------------------------------------------------------------------------------------------------
extern GFL_CLWK* GFL_OBJGRP_CreateClAct( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_DATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_OBJGRP_CreateClActVT( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_DATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_OBJGRP_CreateClActAffine( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex,
	const GFL_CLWK_AFFINEDATA* param, u16 setSerface, HEAPID heapID );

extern GFL_CLWK* GFL_OBJGRP_CreateClActVTAffine( GFL_CLUNIT* actUnit, u32 cgrIndex, u32 plttIndex, u32 cellAnimIndex, 
	  const GFL_CLWK_AFFINEDATA* param, u16 setSerface, HEAPID heapID );



#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* #ifndef __OBJMAN_H__ */
