#pragma once
//============================================================================================
/**
 * @file	  c_gear_picture.h
 * @brief	  CGEAR�ŃZ�[�u���Ă����G�̃f�[�^
 * @author	k.ohno
 * @date	  2009.11.17
 */
//============================================================================================

#include <gflib.h>
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK


//----------------------------------------------------------
/**
 * @brief	C-GEAR�̕ۑ����
 */
//----------------------------------------------------------
typedef struct _CGEAR_PICTURE_SAVEDATA CGEAR_PICTURE_SAVEDATA;


#define CGEAR_PICTURTE_WIDTH   (17)
#define CGEAR_PICTURTE_HEIGHT  (15)

#define CGEAR_PICTURTE_CHAR_SIZE (CGEAR_PICTURTE_WIDTH*CGEAR_PICTURTE_HEIGHT*32)
#define CGEAR_PICTURTE_PAL_SIZE  (16*2)
#define CGEAR_PICTURTE_SCR_SIZE (2*32*24)
#define CGEAR_PICTURTE_ALL_SIZE (CGEAR_PICTURTE_CHAR_SIZE+CGEAR_PICTURTE_PAL_SIZE+CGEAR_PICTURTE_SCR_SIZE)


//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------

extern int CGEAR_PICTURE_SAVE_GetWorkSize(void);
extern CGEAR_PICTURE_SAVEDATA* CGEAR_PICTURE_SAVE_AllocWork(HEAPID heapID);
extern void CGEAR_PICTURE_SAVE_Init(CGEAR_PICTURE_SAVEDATA* pSV);
extern void* CGEAR_PICTURE_SAVE_GetPicture(CGEAR_PICTURE_SAVEDATA* pSV);
extern void* CGEAR_PICTURE_SAVE_GetPalette(CGEAR_PICTURE_SAVEDATA* pSV);
extern BOOL CGEAR_PICTURE_SAVE_IsPalette(CGEAR_PICTURE_SAVEDATA* pSV);

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern CGEAR_PICTURE_SAVEDATA* CGEAR_PICTURE_SAVE_GetCGearSaveData(SAVE_CONTROL_WORK* pSave);




//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _CGEAR_PICTURE_SAVEDATA {
	u8 picture[CGEAR_PICTURTE_CHAR_SIZE]; //�L�����N�^�[
  u8 palette[CGEAR_PICTURTE_PAL_SIZE];     //�p���b�g
  u8 scr[CGEAR_PICTURTE_SCR_SIZE];   //�X�N���[��
};

