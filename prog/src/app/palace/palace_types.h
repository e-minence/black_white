//==============================================================================
/**
 * @file	palace_types.h
 * @brief	�p���X�F���ʒ�`
 * @author	matsuda
 * @date	2009.02.20(��)
 */
//==============================================================================
#ifndef __PALACE_TYPES_H__
#define __PALACE_TYPES_H__


//==============================================================================
//	�萔��`
//==============================================================================
///�G�f�B�b�g�T�C�YX
#define PALACE_EDIT_X		(16)
///�G�f�B�b�g�T�C�YY
#define PALACE_EDIT_Y		(16)
///�G�f�B�b�g�T�C�YZ
#define PALACE_EDIT_Z		(16)

///�u���b�N�̃|���S������
#define PALACE_POLYGON_MAX	((PALACE_EDIT_X + PALACE_EDIT_Y + PALACE_EDIT_Z) * 2)
///�p���X�̃u���b�N�ő吔
#define PALACE_BLOCK_MAX	(PALACE_EDIT_X * PALACE_EDIT_Y * PALACE_EDIT_Z)
///1byte�ɂ����̃u���b�N�f�[�^�������邩
#define PALACE_ONE_BLOCK_NUM	(2)

#define ONE_GRID		(0x100)	//fx16
#define HALF_GRID		(ONE_GRID/2)	//fx16


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	u8 block[PALACE_BLOCK_MAX / PALACE_ONE_BLOCK_NUM];
}PALACE_SAVEDATA;

#endif	//__PALACE_TYPES_H__
