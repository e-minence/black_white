//============================================================================================
/**
 * @file	fieldmap_resist.h
 * @date	2008.12.19
 */
//============================================================================================
#pragma once

//------------------------------------------------------------------
//
//	�}�b�v�f�[�^
//
//------------------------------------------------------------------

#define FLDMAPPER_MAPDATA_NULL	(0xffffffff)
typedef struct {
	u32 datID;
}FLDMAPPER_MAPDATA;

//------------------------------------------------------------------
///	����^�C�v�̎w��
//------------------------------------------------------------------
typedef enum {
	FLDMAPPER_MODE_SCROLL_NONE = 0,
	FLDMAPPER_MODE_SCROLL_XZ,
	FLDMAPPER_MODE_SCROLL_Y,
}FLDMAPPER_MODE;

//------------------------------------------------------------------
///	�ǂݍ��݃u���b�N�t�@�C���̎�ރR�[�h
///	field_g3d_mapper.c��mapFileFuncTbl�Ɠ�������K�v������
//------------------------------------------------------------------
typedef enum {
	FLDMAPPER_FILETYPE_NORMAL = 0,
	FLDMAPPER_FILETYPE_PKGS = 1,
}FLDMAPPER_FILETYPE;

//------------------------------------------------------------------
//
//		�o�^���f�����O�f�[�^
//
//------------------------------------------------------------------
#define	NON_LOWQ	(0xffff)
#define	NON_TEX		(0xffff)
typedef struct {
	u16 highQ_ID;
	u16 lowQ_ID;
}FLDMAPPER_RESISTOBJDATA;

typedef struct {
	u32							arcID;	//�A�[�J�C�u�h�c
	const FLDMAPPER_RESISTOBJDATA*	data;	//���f�[�^
	u32							count;		//���f����

}FLDMAPPER_RESISTDATA_OBJ;

typedef struct {
	u32			arcID;	//�A�[�J�C�u�h�c
	const u16*	data;	//���f�[�^
	u32			count;	//�e�N�X�`����

}FLDMAPPER_RESISTDATA_DDOBJ;

typedef enum {
	FLDMAPPER_RESIST_OBJTYPE_NONE	= 0xffffffff,
	FLDMAPPER_RESIST_OBJTYPE_TBL	= 0,
	FLDMAPPER_RESIST_OBJTYPE_BIN	= 1,
}FLDMAPPER_RESIST_OBJTYPE;
typedef struct {
	u32							objArcID;	//�A�[�J�C�u�h�c
	const FLDMAPPER_RESISTOBJDATA*	objData;	//���f�[�^
	u32							objCount;	//���f����
	u32							ddobjArcID;	//�A�[�J�C�u�h�c
	const u16*					ddobjData;	//���f�[�^
	u32							ddobjCount;	//���f����
}FLDMAPPER_RESISTDATA_OBJTBL;

typedef struct {
	u32							areaObjArcID;		//�z�u��ރA�[�J�C�u�h�c
	u32							areaObjAnmTblArcID;	//�z�u��ރA�j���t�^�f�[�^�A�[�J�C�u�h�c
	u32							areaObjDatID;		//�z�u��ރf�[�^�h�c
	u32							objArcID;			//���f���A�[�J�C�u�h�c
	u32							objtexArcID;		//�e�N�X�`���A�[�J�C�u�h�c
	u32							objanmArcID;		//�A�j���A�[�J�C�u�h�c
}FLDMAPPER_RESISTDATA_OBJBIN;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
	FLDMAPPER_RESIST_TEXTYPE_NONE = 0xffffffff,
	FLDMAPPER_RESIST_TEXTYPE_USE = 0,
}FLDMAPPER_RESIST_TEXTYPE;

typedef struct {
	u32 arcID;
	u32 datID;
}FLDMAPPER_RESIST_TEX;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	FLDMAPPER_FILETYPE		g3DmapFileType;	//g3Dmap�t�@�C�����ʃ^�C�v�i���j
	fx32					width;			//�u���b�N�P�ӂ̕�
	fx32					height;			//�u���b�N����
	FLDMAPPER_MODE			mode;			//���샂�[�h
	u32						arcID;			//�O���t�B�b�N�A�[�J�C�u�h�c
	FLDMAPPER_RESIST_TEXTYPE	gtexType;		//�O���[�o���e�N�X�`���^�C�v
	const FLDMAPPER_RESIST_TEX*	gtexData;		//�O���[�o���e�N�X�`��
	FLDMAPPER_RESIST_OBJTYPE	gobjType;		//�O���[�o���I�u�W�F�N�g�^�C�v
	void *						gobjData;		//�O���[�o���I�u�W�F�N�g

	u16						sizex;			//���u���b�N��
	u16						sizez;			//�c�u���b�N��
	u32						totalSize;		//�z��T�C�Y
	const FLDMAPPER_MAPDATA*	blocks;			//���}�b�v�f�[�^
}FLDMAPPER_RESISTDATA;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_ResistData( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData );


