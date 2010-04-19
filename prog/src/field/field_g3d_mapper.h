//============================================================================================
/**
 */
//============================================================================================
#pragma once

#include "field/map_matrix.h"

#include "height_ex.h"

#include "field/fieldmap_proc.h"


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
#define FLD_MAPPER_CROSSBLOCK_MEMSIZE	( FLD_MAPPER_MAPMDL_SIZE + FLD_MAPPER_MAPTEX_SIZE + (FLD_MAPPER_MAPATTR_SIZE*2) )


//-------------------------------------
///	�`��u���b�N�w��
//=====================================
typedef enum{
  FLDMAPPER_DRAW_TOP,   // �g�b�v�t���[���`��
  FLDMAPPER_DRAW_TAIL,  // �e�C���t���[���`��

  FLDMAPPER_DRAW_TYPE_MAX,
} FLDMAPPER_DRAW_TYPE;

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
extern FLDMAPPER*	FLDMAPPER_Create( HEAPID heapID, u16 season );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
extern BOOL	FLDMAPPER_Main( FLDMAPPER* g3Dmapper );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C���㔼
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_MainTail( FLDMAPPER* g3Dmapper );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_Draw( const FLDMAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera, FLDMAPPER_DRAW_TYPE type );
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
 * @brief	�}�b�v�ʒu����J�����g�u���b�N�A�N�Z�Xindex���擾
 */
//------------------------------------------------------------------
extern int FLDMAPPER_GetCurrentBlockAccessIdx( const FLDMAPPER* g3Dmapper );


//------------------------------------------------------------------
/**
 * @brief	�O���b�h��񃏁[�N������
 */
//------------------------------------------------------------------
extern void FLDMAPPER_GRIDINFODATA_Init( FLDMAPPER_GRIDINFODATA* gridInfoData );
extern void FLDMAPPER_GRIDINFO_Init( FLDMAPPER_GRIDINFO* gridInfo );
//------------------------------------------------------------------
/**
 * @brief	�O���b�h�̑S�K�w�A�g���r���[�g���擾
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_GetGridInfo
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFO* gridInfo );

//------------------------------------------------------------------
/**
 * @brief	�O���b�h�̌��݂̍����ɍł��߂��K�w�̃A�g���r���[�g���擾
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_GetGridData
	( const FLDMAPPER* g3Dmapper, const VecFx32* pos, FLDMAPPER_GRIDINFODATA* pGridData );

//------------------------------------------------------------------
/**
 * @brief	�O���b�h�A�g���r���[�g���擾(�G�t�F�N�g�G���J�E���g��p�I�@���ł͎g��Ȃ��ŁI)
 *
 * @param g3Dmapper 
 * @param pos       x,z�Ƀf�[�^���擾������pos�̍��W���i�[
 * @param gridInfo  �O���b�h�f�[�^���擾����FLDMAPPER_GRIDINFO�\���̌^�ϐ��ւ̃|�C���^
 *
 * @li  ���w�f�[�^�y�ъg�������f�[�^�𖳎����x�[�X�K�w�f�[�^�̂݁��J�����g�u���b�N�݂̂���f�[�^���擾���܂�
 */
//------------------------------------------------------------------
extern BOOL FLDMAPPER_GetGridDataForEffectEncount
	( const FLDMAPPER* g3Dmapper, int blockIdx, const VecFx32* pos, FLDMAPPER_GRIDINFODATA* pGridData );

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
/**
 * @brief �u���b�N�ʒu�擾
 */
//------------------------------------------------------------------
extern void FLDMAPPER_GetBlockXZPos( const FLDMAPPER * g3Dmapper, u32 * blockx, u32 * blockz );

//--------------------------------------------------------------
//�e�N�X�`�����\�[�X��i���p�̃J���[�ɕύX����
//--------------------------------------------------------------
extern BOOL FLDMAPPER_Field_Grayscale(GFL_G3D_RES *g3Dres, int gray_scale);


//--------------------------------------------------------------
/**
 * @brief �}�b�v�ڑ�
 *
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param g3Dmapper �ڑ��Ώۃ}�b�p�[
 * @param matrix    �ڑ�����}�b�v�̃}�b�v�}�g���b�N�X
 *
 * @return �ڑ��ł�����TRUE
 */
//--------------------------------------------------------------
BOOL FLDMAPPER_Connect( FIELDMAP_WORK* fieldmap, FLDMAPPER* g3Dmapper, const MAP_MATRIX* matrix ); 

//--------------------------------------------------------------
/**
 * @brief �f�o�b�O�o�͊֐�
 *
 * @param g3Dmapper ��Ԃ��o�͂������}�b�p�[
 */
//--------------------------------------------------------------
extern void FLDMAPPER_DebugPrint( const FLDMAPPER* g3Dmapper );

//--------------------------------------------------------------
/**
 * @brief �g�������f�[�^���X�g�|�C���^�擾
 *
 * @param g3Dmapper ��Ԃ��o�͂������}�b�p�[
 *
 * @return  �g�������f�[�^���X�g�|�C���^
 */
//--------------------------------------------------------------
extern EHL_PTR	FLDMAPPER_GetExHegihtPtr( FLDMAPPER* g3Dmapper );
