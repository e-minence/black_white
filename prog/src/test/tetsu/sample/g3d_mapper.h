#define MAP_BLOCK_COUNT		(9)

#define	MAPDATA_SIZE	(0x0f000)	//���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPTEX_SIZE		(0x4800) 	//�e�N�X�`���f�[�^�p�u�q�`�l���������m�ۃT�C�Y 
#define	MAPPLTT_SIZE	(0x200) 	//�e�N�X�`���p���b�g�p�u�q�`�l�m�ۃT�C�Y 
#define	MAPATTR_SIZE	(0x6000) 	//�A�g���r���[�g�i�������j�p�������m�ۃT�C�Y 
#define	MAPOBJ_SIZE		(0x18000) 	//�u���b�N���I�u�W�F�N�g���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPOBJTEX_SIZE	(0x4000) 	//�u���b�N���I�u�W�F�N�g�e�N�X�`���f�[�^�p�u�q�`�l�m�ۃT�C�Y 

#define	MAPLOAD_SIZE	(0x800)		//�����f�[�^���[�h�T�C�Y(ROM��RAM) 
#define	MAPTRANS_SIZE	(0x2000) 	//�����f�[�^�]���T�C�Y(RAM��VRAM) 

#define MAP_GRIDCOUNT	(32)		//�}�b�v���c���O���b�h��

typedef struct _G3D_MAPPER G3D_MAPPER;

typedef struct {
	fx16	vecN1_x;
	fx16	vecN1_y;
	fx16	vecN1_z;

	fx16	vecN2_x;
	fx16	vecN2_y;
	fx16	vecN2_z;

	fx32	vecN1_D;
	fx32	vecN2_D;

	u32		attr:31;
	u32		tryangleType:1;
}NormalVtxSt;

typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}G3D_MAPPER_INFODATA;

typedef struct {
	G3D_MAPPER_INFODATA		gridData[MAP_BLOCK_COUNT];	//�O���b�h�f�[�^�擾���[�N
	u16						count;
}G3D_MAPPER_GRIDINFO;

#define	NON_ATTR	(0xffff)
typedef struct {
	u16 datID;
	u16 texID;
	u16 attrID;
}G3D_MAPPER_DATA;

typedef enum {
	G3D_MAPPER_MODE_SCROLL_NONE = 0,
	G3D_MAPPER_MODE_SCROLL_XZ,
	G3D_MAPPER_MODE_SCROLL_Y,
}G3D_MAPPER_MODE;

typedef struct {
	u16						sizex;		//���u���b�N��
	u16						sizez;		//�c�u���b�N��
	u32						totalSize;	//�z��T�C�Y
	fx32					width;		//�u���b�N�P�ӂ̕�
	fx32					height;		//�u���b�N����
	G3D_MAPPER_MODE			mode;		//���샂�[�h
	u32						arcID;		//�O���t�B�b�N�A�[�J�C�u�h�c
	const G3D_MAPPER_DATA*	data;		//���}�b�v�f�[�^

}G3D_MAPPER_RESIST;

#define	NON_LOWQ	(0xffff)
typedef struct {
	u16 highQ_ID;
	u16 lowQ_ID;
}G3D_MAPPEROBJ_DATA;

typedef struct {
	u32							arcID;	//�A�[�J�C�u�h�c
	const G3D_MAPPEROBJ_DATA*	data;	//���}�b�v�f�[�^
	u32							count;		//���f����

}G3D_MAPPEROBJ_RESIST;

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
//------------------------------------------------------------------
/**
 * @brief	�I�u�W�F�N�g���\�[�X�Z�b�g
 */
//------------------------------------------------------------------
extern void ResistObjRes3Dmapper( G3D_MAPPER* g3Dmapper, const G3D_MAPPEROBJ_RESIST* resistData );
extern void ReleaseObjRes3Dmapper( G3D_MAPPER* g3Dmapper );

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


