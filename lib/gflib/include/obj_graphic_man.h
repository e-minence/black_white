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


//-----------------------------------------------------------------------------------------------------------
/**
 *	CGR�f�[�^�֘A����
 *
 *	�E�o�^
 *		GFL_OBJGRP_RegisterCGR					VRAM�풓�^�f�[�^�p
 *		GFL_OBJGRP_RegisterCGR_VramTransfer		VRAM�]���^�f�[�^�p
 *		GFL_OBJGRP_CGR_CreateAlies_VramTransfer	�o�^�ς�VRAM�]���^�f�[�^�̃G�C���A�X�𐶐�
 *
 *	�E���
 *		GFL_OBJGRP_ReleaseCGR
 *
 *	�EVRAM�]�����f�[�^�̍����ւ�
 *		GFL_OBJGRP_CGR_ReplaceSrc_VramTransfer
 *
 *	�EVRAM�]�����f�[�^�̃|�C���^�擾
 *		GFL_OBJGRP_CGR_GetSrcPointer_VramTransfer
 *
 *	�E�v���L�V�擾
 *		GFL_OBJGRP_CGR_GetProxy
 *
 *
 *	���z���g�͊֐�����������������
 *
 *	GFL_OBJGRP_RegisterCGR -> GFL_OBJGRP_CGR_Register
 *	GFL_OBJGRP_RegisterCGR_VramTransfer -> GFL_OBJGRP_CGR_Register_VramTransfer
 *
 *	xxxx_VramTransfer �̕����͒����̂ŁAxxxx_VT �Ƃ��ɂ��Ă����������B
 *
 */
//-----------------------------------------------------------------------------------------------------------


//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�풓�^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h����VRAM�]�����s���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��͔̂j������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   cgrDataID		[in] �A�[�J�C�u����CGR�f�[�^ID
 * @param   compressedFlag	[in] �f�[�^�����k����Ă��邩
 * @param   targetVram		[in] �]����VRAM�w��
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_OBJGRP_RegisterCGR( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, HEAPID heapID );


//==============================================================================================
/**
 * CGR�f�[�^�̓o�^�iVRAM�]���^OBJ�p�j
 *
 * �A�[�J�C�u����CGR�f�[�^�����[�h���A�v���L�V���쐬���ĕێ�����B
 * CGR�f�[�^�̎��̂��ێ���������B
 * CGR�f�[�^�Ɋ֘A�t����ꂽ�Z���A�j���f�[�^����ɓo�^����Ă���K�v������B
 *
 * @param   arcHandle		�A�[�J�C�u�n���h��
 * @param   cgrDataID		CGR�f�[�^�̃A�[�J�C�u��ID
 * @param   targetVram		�]����VRAM�w��
 * @param   cellIndex		�֘A�t����ꂽ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   heapID			�f�[�^���[�h�p�q�[�v
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_OBJGRP_RegisterCGR_VramTransfer( ARCHANDLE* arcHandle, u32 cgrDataID, BOOL compressedFlag, GFL_VRAM_TYPE targetVram, u32 cellIndex, HEAPID heapID );

//==============================================================================================
/**
 * �o�^����Ă���VRAM�]���^CGR�̃G�C���A�X�𐶐����A�V���ɓo�^����
 *
 * ���łɓo�^����Ă���CGR�f�[�^�Ɠ���̂��Q�Ƃ��A�v���L�V���쐬�E�ێ�����B
 * �o�^����Ă���CGR�f�[�^�Ɠ���̂��̂��Q�Ƃ��邽�߁A�q�[�v�g�p�ʂ�}���邱�Ƃ��ł���B
 * CGR�f�[�^�Ɋ֘A�t����ꂽ�Z���A�j���f�[�^����ɓo�^����Ă���K�v������B
 *
 * ���ŏ��ɓo�^����CGR�f�[�^�ƃG�C���A�X�i���������j�͂ǂ̏��Ԃŉ�����Ă��ǂ��B
 *
 * @param   srcCgrIdx		[in] ���łɓo�^����Ă���CGR�f�[�^�C���f�b�N�X�iVRAM�]���^�j
 * @param   cellAnimIdx		[in] �֘A�Â���ꂽ�Z���A�j���f�[�^�̓o�^�C���f�b�N�X
 * @param   targetVram		[in] �]����VRAM
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//==============================================================================================
extern u32 GFL_OBJGRP_CGR_CreateAliesVramTransfer( u32 srcCgrIdx, u32 cellAnimIdx, GFL_VRAM_TYPE targetVram );

//==============================================================================================
/**
 * �o�^���ꂽCGR�f�[�^�̉��
 *
 * @param   index		�o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_OBJGRP_ReleaseCGR( u32 index );

//==============================================================================================
/**
 * �o�^�ς�VRAM�]���^CGR�̓]�����f�[�^����ʂ̃f�[�^�ɍ����ւ���
 *
 * @param   index			CGR�f�[�^�iVRAM�]���^�j�̓o�^�C���f�b�N�X
 * @param   arc				�����ւ����CGR�f�[�^�����A�[�J�C�u�̃n���h��
 * @param   cgrDatIdx		�����ւ����CGR�f�[�^�̃A�[�J�C�u���C���f�b�N�X
 * @param   compressedFlag	�����ւ����CGR�f�[�^�����k����Ă��邩
 * @param	heapID			��Ɨp�q�[�vID
 *
 */
//==============================================================================================
extern void GFL_OBJGRP_CGR_ReplaceSrc_VramTransfer( u32 index, ARCHANDLE* arc, u32 cgrDatIdx, BOOL compressedFlag, HEAPID heapID );

//==============================================================================================
/**
 * �o�^�ς�VRAM�]���^CGR�̓]�����f�[�^�|�C���^���擾�i��舵���͐T�d�ɁI�j
 *
 * @param   index			CGR�f�[�^�iVRAM�]���^�j�̓o�^�C���f�b�N�X
 *
 * @retval  void*		�f�[�^�|�C���^
 */
//==============================================================================================
extern void* GFL_OBJGRP_CGR_GetSrcPointer_VramTransfer( u32 index );

//==============================================================================================
/**
 * CGR�v���L�V�擾
 *
 * @param   index		[in]  �o�^�C���f�b�N�X
 * @param   proxy		[out] �v���L�V�f�[�^�擾�̂��߂̍\���̃A�h���X
 *
 */
//==============================================================================================
extern void GFL_OBJGRP_CGR_GetProxy( u32 index, NNSG2dImageProxy* proxy );


//-----------------------------------------------------------------------------------------------------------
/**
 *	�Z���A�j���f�[�^�֘A����
 *
 *	�E�o�^
 *		GFL_OBJGRP_RegisterCellAnim
 *
 *	�E���
 *		GFL_OBJGRP_ReleaseCellAnim
 *
 *	�EVRAM�]���^���ǂ�������
 *		GFL_OBJGRP_CELLANIM_IsVramTransfer
 *
 *	���z���g�͊֐�����������������
 *
 *	GFL_OBJGRP_RegisterCellAnim -> GFL_OBJGRP_CELLANIM_Register
 *	GFL_OBJGRP_ReleaseCellAnim  -> GFL_OBJGRP_CELLANIM_Release
 */
//-----------------------------------------------------------------------------------------------------------

//==============================================================================================
/**
 * �Z���A�j���f�[�^�o�^
 *
 * @param   arcHandle		�A�[�J�C�u�n���h��
 * @param   cellDataID		�Z���f�[�^ID
 * @param   animDataID		�A�j���f�[�^ID
 * @param   heapID			�f�[�^�ێ��p�q�[�vID
 *
 * @retval  �o�^�C���f�b�N�X
 *
 */
//==============================================================================================
extern u32 GFL_OBJGRP_RegisterCellAnim( ARCHANDLE* arcHandle, u32 cellDataID, u32 animDataID, HEAPID heapID );

//==============================================================================================
/**
 * �o�^���ꂽ�Z���A�j���f�[�^�̉��
 *
 * @param   index		�o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_OBJGRP_ReleaseCellAnim( u32 index );

//==============================================================================================
/**
 * �w�肳�ꂽ�Z���f�[�^���AVRAM�]���^���ǂ������`�F�b�N
 *
 * @param   index		�o�^�C���f�b�N�X
 *
 * @retval  BOOL		TRUE�Ȃ�VRAM�]���^�f�[�^
 */
//==============================================================================================
extern BOOL GFL_OBJGRP_CELLANIM_IsVramTransfer( u32 index );




//-----------------------------------------------------------------------------------------------------------
/**
 *	�p���b�g�f�[�^�֘A����
 *
 *�E�o�^
 *
 *		GFL_OBJGRP_RegisterPltt		�i�񈳏k�p���b�g�f�[�^��p�j
 *			�]����I�t�Z�b�g���w��\�B�p���b�gVRAM�e�ʂ𒴂��Ȃ��͈͂őS�]�����܂��B
 *
 *		GFL_OBJGRP_RegisterPlttEx	�i�񈳏k�p���b�g�f�[�^��p�j
 *			�]����I�t�Z�b�g�ɉ����A�]�����f�[�^�̓ǂݍ��݊J�n�ʒu�A�]���{�����w��ł��܂��B
 *
 *		GFL_OBJGRP_RegisterCompPltt	�i���k�p���b�g�f�[�^��p�j
 *			NitroCharacter�ŕҏW�����ʂ�̈ʒu�ɓ]�����܂��B
 *			�]����I�t�Z�b�g���w�肵�Ă��炷���Ƃ��o���܂��B
 *
 *�E���
 *		GFL_OBJGRP_ReleasePltt
 *
 *�E�v���L�V�擾
 *		GFL_OBJGRP_GetPlttProxy
 *
 *
 *	���z���g�͊֐�����������������
 *
 *	GFL_OBJGRP_RegisterPltt     -> GFL_OBJGRP_PLTT_Register
 *	GFL_OBJGRP_RegisterPlttEx   -> GFL_OBJGRP_PLTT_RegisterEx
 *	GFL_OBJGRP_RegisterCompPltt -> GFL_OBJGRP_PLTT_RegisterComp
 *	GFL_OBJGRP_ReleasePltt      -> GFL_OBJGRP_PLTT_Release
 *	GFL_OBJGRP_GetPlttProxy     -> GFL_OBJGRP_PLTT_GetProxy
 *
//-----------------------------------------------------------------------------------------------------------


//==============================================================================================
/**
 * �񈳏k�p���b�g�f�[�^�̓o�^�����VRAM�ւ̓]��
 * �]����I�t�Z�b�g��0�ȊO�̒l���w�肷��ƁAVRAM�T�C�Y�𒴂��Ȃ��悤�ɓ]���T�C�Y���K�X�A���������B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * ��OBJ�g���p���b�g���w�肵�����ꍇ�A byteOffs�̒l�� GFL_OBJGRP_EXPLTT_OFFSET + �I�t�Z�b�g�o�C�g�����w�肷��
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 *
 */
//==============================================================================================
extern u32 GFL_OBJGRP_RegisterPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, HEAPID heapID );

//==============================================================================================
/**
 * �񈳏k�p���b�g�f�[�^�̓o�^�����VRAM�ւ̓]���i�g���Łj
 * �]����I�t�Z�b�g�w��̑��ɁA�f�[�^�ǂݏo���J�n�ʒu�Ɠ]���{�����w��ł���B
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   srcStartLine	[in] �f�[�^�ǂݏo���J�n�ʒu�i�p���b�g���{�ڂ���]�����邩�H 1�{=16�F�j
 * @param   numTransLines	[in] �]���{���i���{���]�����邩�H 1�{=16�F�^0���ƑS�ē]���j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * ��OBJ�g���p���b�g���w�肵�����ꍇ�A byteOffs�̒l�� GFL_OBJGRP_EXPLTT_OFFSET �����Z���Ďw�肷��B
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 *
 */
//==============================================================================================
extern u32 GFL_OBJGRP_RegisterPlttEx( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, u16 srcStartLine, u16 numTransLines, HEAPID heapID );

//=============================================================================================
/**
 * ���k�p���b�g�f�[�^�i-pcm�I�v�V�������w�肵�Đ��������f�[�^�j�̓o�^�����VRAM�ւ̓]��
 * NitroCharacter�ŕҏW�����ʂ��VRAM�z�u�����B
 *�i�ꉞ�A�I�t�Z�b�g���w�肷�邱�ƂŃY�������Ƃ��\�ɂ��Ă���j
 *
 * @param   arcHandle		[in] �A�[�J�C�u�n���h��
 * @param   plttDataID		[in] �p���b�g�f�[�^�̃A�[�J�C�u���f�[�^ID
 * @param   vramType		[in] �]����VRAM�w��
 * @param   byteOffs		[in] �]����I�t�Z�b�g�i�o�C�g�j
 * @param   heapID			[in] �f�[�^���[�h�p�q�[�v�i�e���|�����j
 *
 * @retval  u32		�o�^�C���f�b�N�X�i�o�^���s�̏ꍇ, GFL_OBJGRP_REGISTER_FAILED�j
 */
//=============================================================================================
extern u32 GFL_OBJGRP_RegisterCompPltt( ARCHANDLE* arcHandle, u32 plttDataID, GFL_VRAM_TYPE vramType, u16 byteOffs, HEAPID heapID );

//==============================================================================================
/**
 * �o�^���ꂽ�p���b�g�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
extern void GFL_OBJGRP_ReleasePltt( u32 index );

//==============================================================================================
/**
 * �o�^���ꂽ�p���b�g�f�[�^�̉��
 *
 * @param   index		[in] �o�^�C���f�b�N�X
 */
//==============================================================================================
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
