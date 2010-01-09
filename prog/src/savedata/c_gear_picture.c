//============================================================================================
/**
 * @file	  c_gear_picture.c
 * @brief	  CGEAR�̉摜���Z�[�u����ꏊ
 * @author	k.ohno
 * @date	  2009.11.17
 */
//============================================================================================

#include "gflib.h"

#include "savedata/save_tbl.h"
#include "savedata/c_gear_picture.h"

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̃T�C�Y�𓾂�
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------
int CGEAR_PICTURE_SAVE_GetWorkSize(void)
{
	return sizeof(CGEAR_PICTURE_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̂𓾂�
 * @return	�\����
 */
//--------------------------------------------------------------------------------------------

CGEAR_PICTURE_SAVEDATA* CGEAR_PICTURE_SAVE_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, CGEAR_PICTURE_SAVE_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void CGEAR_PICTURE_SAVE_Init(CGEAR_PICTURE_SAVEDATA* pSV)
{
  //�f�t�H���g�͖��F
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�̃L�����N�^�[�𓾂�
 * @param   CGEAR_PICTURE_SAVEDATA  
 * @return  �G�̃A�h���X
 */
//--------------------------------------------------------------------------------------------
void* CGEAR_PICTURE_SAVE_GetPicture(CGEAR_PICTURE_SAVEDATA* pSV)
{
  return &pSV->picture[0];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR��ONOFF�𓾂�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @return   ON�Ȃ�TRUE OFF�Ȃ�FALSE
 */
//--------------------------------------------------------------------------------------------
void* CGEAR_PICTURE_SAVE_GetPalette(CGEAR_PICTURE_SAVEDATA* pSV)
{
  return &pSV->palette[0];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �p���b�g�f�[�^�������Ă��邩�ǂ���
 * @param   pSV     CGEAR�Z�[�u�\����
 * @return   TRUE�Ȃ�͂����Ă���
 */
//--------------------------------------------------------------------------------------------
BOOL CGEAR_PICTURE_SAVE_IsPalette(CGEAR_PICTURE_SAVEDATA* pSV)
{
  u8 compbuff[CGEAR_PICTURTE_PAL_SIZE];

  GFL_STD_MemClear(compbuff,CGEAR_PICTURTE_PAL_SIZE);

  if(0==GFL_STD_MemComp(compbuff,pSV->palette,CGEAR_PICTURTE_PAL_SIZE)){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�p�l���^�C�v�ݒ肷��
 * @param   picture     �L�����N�^�[�f�[�^
 * @param   palette     �p���b�g�f�[�^
 * @return	
 */
//--------------------------------------------------------------------------------------------
void CGEAR_PICTURE_SAVE_SetData(CGEAR_PICTURE_SAVEDATA* pSV, void* picture, void* palette)
{
  GFL_STD_MemCopy(picture, pSV->picture, CGEAR_PICTURTE_CHAR_SIZE);
  GFL_STD_MemCopy(palette, pSV->palette, CGEAR_PICTURTE_PAL_SIZE);
}

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
/*
CGEAR_PICTURE_SAVEDATA* CGEAR_PICTURE_SAVE_GetCGearSaveData(SAVE_CONTROL_WORK* pSave)
{
	CGEAR_PICTURE_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_CGEAR);
	return pData;

}
*/
