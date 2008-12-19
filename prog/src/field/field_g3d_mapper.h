//============================================================================================
/**
 */
//============================================================================================
#pragma once

//------------------------------------------------------------------
//------------------------------------------------------------------

#define MAP_BLOCK_COUNT		(9)

#define	MAPMDL_SIZE		(0x0f000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPTEX_SIZE		(0x4800) 	//�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	MAPATTR_SIZE	(0x6000) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 


//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct _FLD_G3D_MAPPER FLD_G3D_MAPPER;

//------------------------------------------------------------------
//------------------------------------------------------------------
#define FLD_G3D_MAPPER_ATTR_MAX	(16)
enum {
	FILE_MAPEDITER_DATA = 0,
	FILE_CUSTOM_DATA = 1,
};

typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}FLD_G3D_MAPPER_INFODATA;

typedef struct {
	FLD_G3D_MAPPER_INFODATA		gridData[FLD_G3D_MAPPER_ATTR_MAX];	//�O���b�h�f�[�^�擾���[�N
	u16						count;
}FLD_G3D_MAPPER_GRIDINFO;

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
extern FLD_G3D_MAPPER*	CreateFieldG3Dmapper( HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
extern void	MainFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
extern void	DrawFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
extern void	DeleteFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper );

extern void	ReleaseDataFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper );

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�]���҂�
 */
//------------------------------------------------------------------
extern BOOL CheckTransFieldG3Dmapper( const FLD_G3D_MAPPER* g3Dmapper );


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
extern void SetPosFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper, const VecFx32* pos );


//------------------------------------------------------------------
/**
 * @brief	�O���b�h��񃏁[�N������
 */
//------------------------------------------------------------------
extern void InitGetFieldG3DmapperGridInfoData( FLD_G3D_MAPPER_INFODATA* gridInfoData );
extern void InitGetFieldG3DmapperGridInfo( FLD_G3D_MAPPER_GRIDINFO* gridInfo );
//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
extern BOOL GetFieldG3DmapperGridInfoData
	( FLD_G3D_MAPPER* g3Dmapper, const VecFx32* pos, FLD_G3D_MAPPER_INFODATA* gridInfoData );
extern BOOL GetFieldG3DmapperGridInfo
	( const FLD_G3D_MAPPER* g3Dmapper, const VecFx32* pos, FLD_G3D_MAPPER_GRIDINFO* gridInfo );

extern u32 GetFieldG3DmapperFileType( const FLD_G3D_MAPPER *g3Dmapper );
extern BOOL GetFieldG3DmapperGridAttr(
	const FLD_G3D_MAPPER* g3Dmapper, const VecFx32* pos, u16 *attr );

//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
extern BOOL CheckFieldG3DmapperOutRange( const FLD_G3D_MAPPER* g3Dmapper, const VecFx32* pos );
//------------------------------------------------------------------
/**
 * @brief	�T�C�Y�擾
 */
//------------------------------------------------------------------
extern void GetFieldG3DmapperSize( const FLD_G3D_MAPPER* g3Dmapper, fx32* x, fx32* z );

extern void SetFieldG3DmapperDrawOffset( FLD_G3D_MAPPER *g3Dmapper, const VecFx32 *offs );
extern void GetFieldG3DmapperDrawOffset( const FLD_G3D_MAPPER *g3Dmapper, VecFx32 *offs );

