#pragma once

#define MAP_BLOCK_COUNT		(9)

#define	MAPMDL_SIZE		(0x0f000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPTEX_SIZE		(0x4800) 	//�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	MAPATTR_SIZE	(0x6000) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 

#define FLD_G3D_MAPPER_ATTR_MAX	(16)

typedef struct _FLD_G3D_MAPPER FLD_G3D_MAPPER;

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

#define FLD_G3D_MAPPER_NOMAP	(0xffffffff)
typedef struct {
	u32 datID;
}FLD_G3D_MAPPER_DATA;

typedef enum {
	FLD_G3D_MAPPER_MODE_SCROLL_NONE = 0,
	FLD_G3D_MAPPER_MODE_SCROLL_XZ,
	FLD_G3D_MAPPER_MODE_SCROLL_Y,
}FLD_G3D_MAPPER_MODE;

#define	NON_LOWQ	(0xffff)
#define	NON_TEX		(0xffff)
typedef struct {
	u16 highQ_ID;
	u16 lowQ_ID;
}FLD_G3D_MAPPEROBJ_DATA;

typedef struct {
	u32							arcID;	//�A�[�J�C�u�h�c
	const FLD_G3D_MAPPEROBJ_DATA*	data;	//���}�b�v�f�[�^
	u32							count;		//���f����

}FLD_G3D_MAPPER_RESIST_OBJ;

typedef struct {
	u32			arcID;	//�A�[�J�C�u�h�c
	const u16*	data;	//���}�b�v�f�[�^
	u32			count;	//�e�N�X�`����

}FLD_G3D_MAPPER_RESIST_DDOBJ;

#define NON_GLOBAL_OBJ	(0xffffffff)
#define USE_GLOBAL_OBJSET_TBL	(0)
#define USE_GLOBAL_OBJSET_BIN	(1)
typedef struct {
	u32							objArcID;	//�A�[�J�C�u�h�c
	const FLD_G3D_MAPPEROBJ_DATA*	objData;	//���}�b�v�f�[�^
	u32							objCount;	//���f����
	u32							ddobjArcID;	//�A�[�J�C�u�h�c
	const u16*					ddobjData;	//���}�b�v�f�[�^
	u32							ddobjCount;	//���f����
}FLD_G3D_MAPPER_GLOBAL_OBJSET_TBL;

typedef struct {
	u32							areaObjArcID;		//�z�u��ރA�[�J�C�u�h�c
	u32							areaObjAnmTblArcID;	//�z�u��ރA�j���t�^�f�[�^�A�[�J�C�u�h�c
	u32							areaObjDatID;		//�z�u��ރf�[�^�h�c
	u32							objArcID;			//���f���A�[�J�C�u�h�c
	u32							objtexArcID;		//�e�N�X�`���A�[�J�C�u�h�c
	u32							objanmArcID;		//�A�j���A�[�J�C�u�h�c
}FLD_G3D_MAPPER_GLOBAL_OBJSET_BIN;

#define	NON_GLOBAL_TEX	(0xffffffff)
#define	USE_GLOBAL_TEX	(0)
typedef struct {
	u32 arcID;
	u32 datID;
}FLD_G3D_MAPPER_GLOBAL_TEXTURE;

typedef struct {
	u32						g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	fx32					width;			//�u���b�N�P�ӂ̕�
	fx32					height;			//�u���b�N����
	FLD_G3D_MAPPER_MODE			mode;			//���샂�[�h
	u32						arcID;			//�O���t�B�b�N�A�[�J�C�u�h�c
	u32						gtexType;		//�O���[�o���e�N�X�`���^�C�v
	void*					gtexData;		//�O���[�o���e�N�X�`��
	u32						gobjType;		//�O���[�o���I�u�W�F�N�g�^�C�v
	void*					gobjData;		//�O���[�o���I�u�W�F�N�g

	u16						sizex;			//���u���b�N��
	u16						sizez;			//�c�u���b�N��
	u32						totalSize;		//�z��T�C�Y
	const FLD_G3D_MAPPER_DATA*	data;			//���}�b�v�f�[�^

}FLD_G3D_MAPPER_RESIST;

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

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�]���҂�
 */
//------------------------------------------------------------------
extern BOOL CheckTransFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper );

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	ResistDataFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper, const FLD_G3D_MAPPER_RESIST* resistData );
extern void	ReleaseDataFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper );

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
extern void GetFieldG3DmapperSize( FLD_G3D_MAPPER* g3Dmapper, fx32* x, fx32* z );

extern void SetFieldG3DmapperDrawOffset( FLD_G3D_MAPPER *g3Dmapper, const VecFx32 *offs );
extern void GetFieldG3DmapperDrawOffset( FLD_G3D_MAPPER *g3Dmapper, VecFx32 *offs );
