//======================================================================
/**
 * @file	fieldmap_proc.h
 * @brief �t�B�[���h�}�b�v�@�v���Z�X
 * @date	2008.11.18
 * @author	tamada GAME FREAK inc.
 *
 * @note
 * 2009.09.20 tamada  FIELDMAP_WORK�ւ̎Q�Ƃ�PROC�A�N�Z�X�݂̂��c����
 * ���p�֐���extern��include/field/fieldmap_call.h�Ɉړ�����
 */
//======================================================================
#pragma once
#include <gflib.h>

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	�t�B�[���h�}�b�v�@�R���g���[���^�C�v
//--------------------------------------------------------------
typedef enum
{
	FLDMAP_CTRLTYPE_GRID = 0, ///<�O���b�h�^�C�v
	FLDMAP_CTRLTYPE_NOGRID, ///<��O���b�h�^�C�v
	FLDMAP_CTRLTYPE_HYBRID, ///<�O���b�h�A��O���b�h�n�C�u���b�g
	FLDMAP_CTRLTYPE_MAX,
}FLDMAP_CTRLTYPE;


//--------------------------------------------------------------
///	�t�B�[���h�}�b�v�@�x�[�X�V�X�e���^�C�v
//--------------------------------------------------------------
typedef enum
{
	FLDMAP_BASESYS_GRID = 0,  ///<�x�[�X�V�X�e���@�O���b�h�ړ�
	FLDMAP_BASESYS_RAIL,      ///<�x�[�X�V�X�e���@���[���ړ�
	FLDMAP_BASESYS_MAX,
}FLDMAP_BASESYS_TYPE;

//======================================================================
//	struct
//======================================================================
///FIELDMAP
typedef struct _FIELDMAP_WORK FIELDMAP_WORK;

//======================================================================
//	extern
//======================================================================
extern const GFL_PROC_DATA FieldProcData;

FS_EXTERN_OVERLAY(fieldmap);
