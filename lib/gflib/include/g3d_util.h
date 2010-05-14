#ifdef __cplusplus
extern "C" {
#endif
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
  GFL_G3D_UTIL_RESARC_UTILHEAP,		//�A�[�J�C�u�h�c���w��@���[�e�B���e�B�̃q�[�v���g�p
  GFL_G3D_UTIL_RESPATH_UTILHEAP,			//�A�[�J�C�u�p�X���w�� ���[�e�B���e�B�̃q�[�v���g�p

}GFL_G3D_UTIL_RESTYPE;	//�w��A�[�J�C�u�^�C�v

//	���\�[�X�z��ݒ�p�e�[�u���\����
typedef struct {
	u32						arcive;		//�w��A�[�J�C�u�h�c�������̓A�[�J�C�u�p�X�|�C���^
	u16						datID;		//�A�[�J�C�u���f�[�^�h�c
	GFL_G3D_UTIL_RESTYPE		arcType;	//�w��A�[�J�C�u�^�C�v
}GFL_G3D_UTIL_RES;

typedef struct {
	u16				anmresID;	//�A�j�����\�[�X�h�c
	u16				anmdatID;	//�A�j���f�[�^�h�c(���\�[�X�����h�m�c�d�w)
}GFL_G3D_UTIL_ANM;

typedef struct {
	u16				mdlresID;	//���f�����\�[�X�h�c
	u16				mdldatID;	//���f���f�[�^�h�c(���\�[�X�����h�m�c�d�w)
	u16				texresID;	//�e�N�X�`�����\�[�X�h�c
	const GFL_G3D_UTIL_ANM*	anmTbl;		//�A�j���e�[�u���i�����ݒ�̂��߁j
	u16				anmCount;	//�A�j�����\�[�X��
}GFL_G3D_UTIL_OBJ;

typedef struct {
	const GFL_G3D_UTIL_RES*	resTbl;		//���\�[�X�e�[�u��
	u16				resCount;	//���\�[�X��

	const GFL_G3D_UTIL_OBJ*	objTbl;		//�I�u�W�F�N�g�ݒ�e�[�u��
	u16				objCount;	//�I�u�W�F�N�g��

}GFL_G3D_UTIL_SETUP;

#define GFL_G3D_UTIL_NORESOURCE_ID (0xff)
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
 * @param	resCountMax			�ݒ胊�\�[�X�ő吔��
 * @param	objCountMax			�ݒ�I�u�W�F�N�g�ő吔
 * @param	heapID				�q�[�v�h�c
 *
 * @return	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_UTIL*
	GFL_G3D_UTIL_Create
		( u16 resCountMax, u16 objCountMax, HEAPID heapID );
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
 * ���j�b�g�Z�b�g�A�b�v
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 * @param	setup				�ݒ�f�[�^
 *
 * @return	unitIdx				���j�b�g�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_AddUnit
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup );

extern u16
	GFL_G3D_UTIL_AddUnitResShare
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup );

extern u16
	GFL_G3D_UTIL_AddUnitResShareByHandle
		( GFL_G3D_UTIL* g3Dutil, const GFL_G3D_UTIL_SETUP* setup, ARCHANDLE *handle );
    
//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�j��
 *
 * @param	GFL_G3D_UTIL*		�f�[�^�Z�b�g�n���h��
 * @param	unitIdx				���j�b�g�C���f�b�N�X
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_UTIL_DelUnit
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g���\�[�X�擪�C���f�b�N�X�擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitResIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g���\�[�X���擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitResCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�I�u�W�F�N�g�擪�C���f�b�N�X�擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitObjIdx
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ���j�b�g�I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetUnitObjCount
		( GFL_G3D_UTIL* g3Dutil, u16 unitIdx );
//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�n���h���擾
 */
//--------------------------------------------------------------------------------------------
extern GFL_G3D_RES*
	GFL_G3D_UTIL_GetResHandle
		( GFL_G3D_UTIL* g3Dutil, u16 idx );
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
 * ���I�u�W�F�N�g���擾
 */
//--------------------------------------------------------------------------------------------
extern u16
	GFL_G3D_UTIL_GetObjCount
		( GFL_G3D_UTIL* g3Dutil );
//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�ɃA�j���[�V������ǉ��ݒ肷��i�ʃ��j�b�g�̃A�j����ݒ肷��Ȃǁj
 */
//--------------------------------------------------------------------------------------------
extern BOOL
	GFL_G3D_UTIL_SetObjAnotherUnitAnime
		( GFL_G3D_UTIL* g3Dutil, u16 objUnitIdx, u16 objIdx, 
			u16 anmUnitIdx, const GFL_G3D_UTIL_ANM* anmTbl, u16 anmCount );

#endif
#ifdef __cplusplus
} /* extern "C" */
#endif
