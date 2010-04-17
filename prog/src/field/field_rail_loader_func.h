//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_rail_loader_func.h
 *	@brief	�t�B�[���h���[�����[�_�[�@�֐�ID
 *	@author	tomoya takahashi
 *	@date		2009.07.07
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�J�����֐�ID
*/
//-----------------------------------------------------------------------------
typedef enum
{
	// rail_conv.pl�p�R�����g�@�C�����Ȃ��ŁB
	//CAMERA_FUNC_ID_START
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXPOS,				// �Œ�ʒu�J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE,			// �Œ�A���O���J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_OFSANGLE,			// �I�t�Z�b�g�A���O���J����	�i���C����p�j
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXALL,				// ���S�Œ�J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXALL_LINE,	// ���S�Œ�J�������`��ԁi���C����p�j
	FIELD_RAIL_LOADER_CAMERA_FUNC_CIRCLE,				// �~����J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXLENCIRCLE,	// �����Œ�~����J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_TARGETCHANGE_CIRCLE,	// �����Œ�~����^�[�Q�b�g�ύX�J����
	FIELD_RAIL_LOADER_CAMERA_FUNC_PLT_CIRCLE,				// �v���C���[�^�[�Q�b�g�~����J���� 

	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE_LINEWAY,	// �i�s�����ɑ΂���A���O�����Œ�
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE_LINEWAY_XYZ,	// �i�s�����ɑ΂���A���O�����Œ�
	FIELD_RAIL_LOADER_CAMERA_FUNC_FIXANGLE_LINEWAY_CENTER,	// �i�s�����ɑ΂���A���O�����Œ� �^�[�Q�b�g���C���̒��S
	FIELD_RAIL_LOADER_CAMERA_FUNC_STOPSCROLL_FIXANGLE_START,// �J�����X�N���[����~�A�n�_�@FIX�A���O��
	FIELD_RAIL_LOADER_CAMERA_FUNC_STOPSCROLL_FIXANGLE_END,// �J�����X�N���[����~�A�I�_�@FIX�A���O��
	FIELD_RAIL_LOADER_CAMERA_FUNC_OFSANGLE_NOT_TRACE,			// �I�t�Z�b�g�A���O���J����	�@�J����TRACE�Ȃ��i���C����p�j

	FIELD_RAIL_LOADER_CAMERA_FUNC_MAX,	// �ő�l
} FIELD_RAIL_LOADER_CAMERA_FUNC;

//-----------------------------------------------------------------------------
/**
 *					���W�v�Z�֐�ID
*/
//-----------------------------------------------------------------------------
typedef enum
{
	// rail_conv.pl�p�R�����g�@�C�����Ȃ��ŁB
	//LINEPOS_FUNC_ID_START
	FIELD_RAIL_LOADER_LINEPOS_FUNC_STRAIT,			// ����
	FIELD_RAIL_LOADER_LINEPOS_FUNC_CURVE,				// �J�[�u
	FIELD_RAIL_LOADER_LINEPOS_FUNC_YCURVE,		  // Y�J�[�u

	FIELD_RAIL_LOADER_LINEPOS_FUNC_MAX,				// �ő�l
} FIELD_RAIL_LOADER_LINEPOS_FUNC;


//-----------------------------------------------------------------------------
/**
 *					�����v�Z�֐�ID
*/
//-----------------------------------------------------------------------------
typedef enum
{
	// rail_conv.pl�p�R�����g�@�C�����Ȃ��ŁB
	//LINEDIST_FUNC_ID_START
	FIELD_RAIL_LOADER_LINEDIST_FUNC_STRAIT,			// ����
	FIELD_RAIL_LOADER_LINEDIST_FUNC_CURVE,		  // �J�[�u
	FIELD_RAIL_LOADER_LINEDIST_FUNC_YCURVE,		// Y�J�[�u

	FIELD_RAIL_LOADER_LINEDIST_FUNC_MAX,				// �ő�l
} FIELD_RAIL_LOADER_LINEDIST_FUNC;


#ifdef _cplusplus
}	// extern "C"{
#endif



