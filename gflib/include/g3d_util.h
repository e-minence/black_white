//=============================================================================================
/**
 * @file	g3d_util.h                                                  
 * @brief	�R�c�f�[�^�Ǘ����[�e�B���e�B�[�v���O����
 */
//=============================================================================================
#ifndef _G3D_UTIL_H_
#define _G3D_UTIL_H_

typedef struct _GFL_G3D_UTIL	GFL_G3D_UTIL;

typedef enum {
	GFL_G3D_UTIL_RESARC = 0,		//�A�[�J�C�u�h�c���w��
	GFL_G3D_UTIL_RESPATH,			//�A�[�J�C�u�p�X���w��

}GFL_G3D_UTIL_RESTYPE;	//�w��A�[�J�C�u�^�C�v

//	���\�[�X�z��ݒ�p�e�[�u���\����
typedef struct {
	const u32						arcive;		//�w��A�[�J�C�u�h�c�������̓A�[�J�C�u�p�X�|�C���^
	const u16						datID;		//�A�[�J�C�u���f�[�^�h�c
	const GFL_G3D_UTIL_RESTYPE		arcType;	//�w��A�[�J�C�u�^�C�v
}GFL_G3D_UTIL_RES;

typedef struct {
	const u16				anmresID;	//�A�j�����\�[�X�h�c
	const u16				anmdatID;	//�A�j���f�[�^�h�c(���\�[�X�����h�m�c�d�w)
}GFL_G3D_UTIL_ANM;

typedef struct {
	const u16				mdlresID;	//���f�����\�[�X�h�c
	const u16				mdldatID;	//���f���f�[�^�h�c(���\�[�X�����h�m�c�d�w)
	const u16				texresID;	//�e�N�X�`�����\�[�X�h�c
	const GFL_G3D_UTIL_ANM*	anmTbl;		//�A�j���e�[�u���i�����ݒ�̂��߁j
	const u16				anmCount;	//�A�j�����\�[�X��
}GFL_G3D_UTIL_OBJ;

typedef struct {
	const GFL_G3D_UTIL_RES*	resTbl;		//���\�[�X�e�[�u��
	const u16				resCount;	//���\�[�X��

	const GFL_G3D_UTIL_OBJ*	objTbl;		//�I�u�W�F�N�g�ݒ�e�[�u��
	const u16				objCount;	//�I�u�W�F�N�g��

}GFL_G3D_UTIL_SETUP;

//=============================================================================================
/**
 *
 *
 * �R�c�֘A�n���h���̊Ǘ����[�e�B���e�B�[
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v
 *
 * @param	scene				�ݒ�f�[�^
 * @param	heapID				�q�[�v�h�c
 *
 * @return	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_UTIL*
	GFL_G3D_UTIL_Create
		( const GFL_G3D_UTIL_SETUP* setup, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_UTIL_Delete
		( GFL_G3D_UTIL* g3Dutil );

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h���擾
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_OBJ*
	GFL_G3D_UTIL_GetObjHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx );

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetObjCount
		( GFL_G3D_UTIL* g3Dutil );

#endif
