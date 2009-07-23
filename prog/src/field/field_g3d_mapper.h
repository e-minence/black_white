//============================================================================================
/**
 */
//============================================================================================
#pragma once

#include "field_hit_check.h"
//------------------------------------------------------------------
/**
 * @brief	�}�b�p�[����\���̂̕s���S�^��`
 */
//------------------------------------------------------------------
typedef struct _FLD_G3D_MAPPER FLDMAPPER;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}FLDMAPPER_GRIDINFODATA;

#define FLDMAPPER_GRIDINFO_MAX	(16)

typedef struct {
	FLDMAPPER_GRIDINFODATA	gridData[FLDMAPPER_GRIDINFO_MAX];	//�O���b�h�f�[�^�擾���[�N
	u16						count;
}FLDMAPPER_GRIDINFO;



//-------------------------------------
///	1�u���b�N�������T�C�Y�̊
//=====================================
#define	FLD_MAPPER_MAPMDL_SIZE		(0xf000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	FLD_MAPPER_MAPTEX_SIZE		(0) 	    //�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	FLD_MAPPER_MAPATTR_SIZE	(0x6004) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 
#define FLD_MAPPER_BLOCK_MEMSIZE	( FLD_MAPPER_MAPMDL_SIZE + FLD_MAPPER_MAPTEX_SIZE + FLD_MAPPER_MAPATTR_SIZE )


//============================================================================================
//
//
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
extern FLDMAPPER*	FLDMAPPER_Create( HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_Main( FLDMAPPER* g3Dmapper );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_Draw( const FLDMAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_Delete( FLDMAPPER* g3Dmapper );

extern void	FLDMAPPER_ReleaseData( FLDMAPPER* g3Dmapper );

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�]���҂�
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_CheckTrans( const FLDMAPPER* g3Dmapper );


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
extern void FLDMAPPER_SetPos( FLDMAPPER* g3Dmapper, const VecFx32* pos );


//------------------------------------------------------------------
/**
 * @brief	�O���b�h��񃏁[�N������
 */
//------------------------------------------------------------------
extern void FLDMAPPER_GRIDINFODATA_Init( FLDMAPPER_GRIDINFODATA* gridInfoData );
extern void FLDMAPPER_GRIDINFO_Init( FLDMAPPER_GRIDINFO* gridInfo );
//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_GetGridInfo
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFO* gridInfo );


//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_CheckOutRange( const FLDMAPPER* g3Dmapper, const VecFx32* pos );
//------------------------------------------------------------------
/**
 * @brief	�T�C�Y�擾
 */
//------------------------------------------------------------------
extern void FLDMAPPER_GetSize( const FLDMAPPER* g3Dmapper, fx32* x, fx32* z );

extern void FLDMAPPER_SetDrawOffset( FLDMAPPER *g3Dmapper, const VecFx32 *offs );
extern void FLDMAPPER_GetDrawOffset( const FLDMAPPER *g3Dmapper, VecFx32 *offs );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern const GFL_G3D_MAP_GLOBALOBJ_ST * FLDMAPPER_CreateObjStatusList
( const FLDMAPPER* g3Dmapper, const FLDHIT_RECT * rect, HEAPID heapID, u32 * num );

//--------------------------------------------------------------
//--------------------------------------------------------------
extern BOOL DEBUG_Field_Grayscale(GFL_G3D_RES *g3Dres);

