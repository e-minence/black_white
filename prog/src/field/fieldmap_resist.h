//============================================================================================
/**
 * @file	fieldmap_resist.h
 * @date	2008.12.19
 */
//============================================================================================
#pragma once
//------------------------------------------------------------------
//------------------------------------------------------------------

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

//============================================================================================
//============================================================================================

typedef struct {
	u32						g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	fx32					width;			//�u���b�N�P�ӂ̕�
	fx32					height;			//�u���b�N����
	FLD_G3D_MAPPER_MODE			mode;			//���샂�[�h
	u32						arcID;			//�O���t�B�b�N�A�[�J�C�u�h�c
	u32						gtexType;		//�O���[�o���e�N�X�`���^�C�v
	FLD_G3D_MAPPER_GLOBAL_TEXTURE*	gtexData;		//�O���[�o���e�N�X�`��
	u32						gobjType;		//�O���[�o���I�u�W�F�N�g�^�C�v
	void*					gobjData;		//�O���[�o���I�u�W�F�N�g

	u16						sizex;			//���u���b�N��
	u16						sizez;			//�c�u���b�N��
	u32						totalSize;		//�z��T�C�Y
	const FLD_G3D_MAPPER_DATA*	data;			//���}�b�v�f�[�^

}FLD_G3D_MAPPER_RESIST;

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	ResistDataFieldG3Dmapper( FLD_G3D_MAPPER* g3Dmapper, const FLD_G3D_MAPPER_RESIST* resistData );


