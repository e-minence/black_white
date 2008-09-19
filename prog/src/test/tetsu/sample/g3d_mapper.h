#define MAP_BLOCK_COUNT		(9)

#define	MAPMDL_SIZE		(0x0f000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPTEX_SIZE		(0x4800) 	//�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	MAPATTR_SIZE	(0x6000) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 

#define G3D_MAPPER_ATTR_MAX	(16)

typedef struct _G3D_MAPPER G3D_MAPPER;

enum {
	FILE_MAPEDITER_DATA = 0,
	FILE_CUSTOM_DATA = 1,
};

typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}G3D_MAPPER_INFODATA;

typedef struct {
	G3D_MAPPER_INFODATA		gridData[G3D_MAPPER_ATTR_MAX];	//�O���b�h�f�[�^�擾���[�N
	u16						count;
}G3D_MAPPER_GRIDINFO;

#define G3D_MAPPER_NOMAP	(0xffffffff)
typedef struct {
	u32 datID;
}G3D_MAPPER_DATA;

typedef enum {
	G3D_MAPPER_MODE_SCROLL_NONE = 0,
	G3D_MAPPER_MODE_SCROLL_XZ,
	G3D_MAPPER_MODE_SCROLL_Y,
}G3D_MAPPER_MODE;

#define	NON_LOWQ	(0xffff)
#define	NON_TEX		(0xffff)
typedef struct {
	u16 highQ_ID;
	u16 lowQ_ID;
}G3D_MAPPEROBJ_DATA;

typedef struct {
	u32							arcID;	//�A�[�J�C�u�h�c
	const G3D_MAPPEROBJ_DATA*	data;	//���}�b�v�f�[�^
	u32							count;		//���f����

}G3D_MAPPER_RESIST_OBJ;

typedef struct {
	u32			arcID;	//�A�[�J�C�u�h�c
	const u16*	data;	//���}�b�v�f�[�^
	u32			count;	//�e�N�X�`����

}G3D_MAPPER_RESIST_DDOBJ;

#define NON_GLOBAL_OBJ	(0xffffffff)
#define USE_GLOBAL_OBJSET_TBL	(0)
#define USE_GLOBAL_OBJSET_BIN	(1)
typedef struct {
	u32							objArcID;	//�A�[�J�C�u�h�c
	const G3D_MAPPEROBJ_DATA*	objData;	//���}�b�v�f�[�^
	u32							objCount;	//���f����
	u32							ddobjArcID;	//�A�[�J�C�u�h�c
	const u16*					ddobjData;	//���}�b�v�f�[�^
	u32							ddobjCount;	//���f����
}G3D_MAPPER_GLOBAL_OBJSET_TBL;

typedef struct {
	u32							areaObjArcID;		//�z�u��ރA�[�J�C�u�h�c
	u32							areaObjAnmTblArcID;	//�z�u��ރA�j���t�^�f�[�^�A�[�J�C�u�h�c
	u32							areaObjDatID;		//�z�u��ރf�[�^�h�c
	u32							objArcID;			//���f���A�[�J�C�u�h�c
	u32							objtexArcID;		//�e�N�X�`���A�[�J�C�u�h�c
	u32							objanmArcID;		//�A�j���A�[�J�C�u�h�c
}G3D_MAPPER_GLOBAL_OBJSET_BIN;

#define	NON_GLOBAL_TEX	(0xffffffff)
#define	USE_GLOBAL_TEX	(0)
typedef struct {
	u32 arcID;
	u32 datID;
}G3D_MAPPER_GLOBAL_TEXTURE;

typedef struct {
	u32						g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	u16						sizex;			//���u���b�N��
	u16						sizez;			//�c�u���b�N��
	u32						totalSize;		//�z��T�C�Y
	fx32					width;			//�u���b�N�P�ӂ̕�
	fx32					height;			//�u���b�N����
	G3D_MAPPER_MODE			mode;			//���샂�[�h
	u32						arcID;			//�O���t�B�b�N�A�[�J�C�u�h�c
	u32						gtexType;		//�O���[�o���e�N�X�`���^�C�v
	void*					gtexData;		//�O���[�o���e�N�X�`��
	u32						gobjType;		//�O���[�o���I�u�W�F�N�g�^�C�v
	void*					gobjData;		//�O���[�o���I�u�W�F�N�g

	const G3D_MAPPER_DATA*	data;			//���}�b�v�f�[�^

}G3D_MAPPER_RESIST;

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���쐬
 */
//------------------------------------------------------------------
extern G3D_MAPPER*	Create3Dmapper( HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e�����C��
 */
//------------------------------------------------------------------
extern void	Main3Dmapper( G3D_MAPPER* g3Dmapper );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���f�B�X�v���C
 */
//------------------------------------------------------------------
extern void	Draw3Dmapper( G3D_MAPPER* g3Dmapper, GFL_G3D_CAMERA* g3Dcamera );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�R���g���[���V�X�e���j��
 */
//------------------------------------------------------------------
extern void	Delete3Dmapper( G3D_MAPPER* g3Dmapper );


//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	ResistData3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPER_RESIST* resistData );
extern void	ReleaseData3Dmapper( G3D_MAPPER* g3Dmapper );

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�ʒu�Z�b�g
 */
//------------------------------------------------------------------
extern void SetPos3Dmapper( G3D_MAPPER* g3Dmapper, const VecFx32* pos );


//------------------------------------------------------------------
/**
 * @brief	�O���b�h��񃏁[�N������
 */
//------------------------------------------------------------------
extern void InitGet3DmapperGridInfoData( G3D_MAPPER_INFODATA* gridInfoData );
extern void InitGet3DmapperGridInfo( G3D_MAPPER_GRIDINFO* gridInfo );
//------------------------------------------------------------------
/**
 * @brief	�O���b�h���擾
 */
//------------------------------------------------------------------
extern BOOL Get3DmapperGridInfoData
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, G3D_MAPPER_INFODATA* gridInfoData );
extern BOOL Get3DmapperGridInfo
	( G3D_MAPPER* g3Dmapper, const VecFx32* pos, G3D_MAPPER_GRIDINFO* gridInfo );

//------------------------------------------------------------------
/**
 * @brief	�͈͊O�`�F�b�N
 */
//------------------------------------------------------------------
extern BOOL Check3DmapperOutRange( G3D_MAPPER* g3Dmapper, const VecFx32* pos );
//------------------------------------------------------------------
/**
 * @brief	�T�C�Y�擾
 */
//------------------------------------------------------------------
extern void Get3DmapperSize( G3D_MAPPER* g3Dmapper, fx32* x, fx32* z );


