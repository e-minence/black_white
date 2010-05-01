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
	FLDMAPPER_MODE_SCROLL_XZ_LOOP,
	FLDMAPPER_MODE_SCROLL_Y,
}FLDMAPPER_MODE;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef enum {
	FLDMAPPER_TEXTYPE_NONE = 0,
	FLDMAPPER_TEXTYPE_USE,

  FLDMAPPER_TEXTYPE_MAX,
}FLDMAPPER_TEXTYPE;

typedef struct {
	u32 arcID;
	u32 datID;
}FLDMAPPER_RESIST_TEX;

typedef struct {
	u32 ita_datID;				// �����ꍇ��FLDMAPPER_MAPDATA_NULL���w��
	u32 itp_anm_datID;		// �����ꍇ��FLDMAPPER_MAPDATA_NULL���w��
}FLDMAPPER_RESIST_GROUND_ANIME;

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	fx32					blockWidth;			//�u���b�N�P�ӂ̕�
	fx32					blockHeight;		//�u���b�N����
  u16           blockXNum;      //�������Ɉ�x�ɒu���AX�����u���b�N��
  u16           blockZNum;      //�������Ɉ�x�ɒu���AZ���@�u���b�N��
	FLDMAPPER_MODE			mode;			//���샂�[�h
	u32						arcID;			//�O���t�B�b�N�A�[�J�C�u�h�c
	u16						sizex;			//���u���b�N��
	u16						sizez;			//�c�u���b�N��
	u32						totalSize;		//�z��T�C�Y
	const FLDMAPPER_MAPDATA*	blocks;			//���}�b�v�f�[�^

	FLDMAPPER_TEXTYPE	gtexType;		//�O���[�o���e�N�X�`���^�C�v
	FLDMAPPER_RESIST_TEX	gtexData;		//�O���[�o���e�N�X�`��

	FLDMAPPER_RESIST_GROUND_ANIME ground_anime;	// �n�ʃA�j���[�V����

	u32						memsize;				// �P�u���b�N�������T�C�Y

  u8            topWriteBlockNum; // top�i�f�[�^�X�V�t���[���j�ŕ`�悷��u���b�N��
  u8            pad[3];
}FLDMAPPER_RESISTDATA;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	FLDMAPPER_ResistData( FLDMAPPER* g3Dmapper, const FLDMAPPER_RESISTDATA* resistData, u8 *gray_scale );


