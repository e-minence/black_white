#pragma once
//============================================================================================
/**
 * @file	  c_gear_data.h
 * @brief	  CGEAR�ŃZ�[�u���Ă����f�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include <gflib.h>
#include "savedata/save_control.h"	//SAVE_CONTROL_WORK


//----------------------------------------------------------
/**
 * @brief	C-GEAR�̕ۑ����
 */
//----------------------------------------------------------
typedef struct _CGEAR_SAVEDATA CGEAR_SAVEDATA;

#define C_GEAR_PANEL_WIDTH   (9)
#define C_GEAR_PANEL_HEIGHT  (4)

/// �ς˂�^�C�v
//���1�r�b�g��TRUE�Ȃ當���̏o��Ƃ���B
typedef enum {
  CGEAR_PANELTYPE_NONE,
  CGEAR_PANELTYPE_IR,
  CGEAR_PANELTYPE_WIRELESS,
  CGEAR_PANELTYPE_WIFI,
  CGEAR_PANELTYPE_BASE,
  CGEAR_PANELTYPE_MAX,
  CGEAR_PANELTYPE_BOOT=CGEAR_PANELTYPE_MAX,

  CGEAR_PANELTYPE_IR_ICON = CGEAR_PANELTYPE_IR | 0x80,
  CGEAR_PANELTYPE_WIRELESS_ICON = CGEAR_PANELTYPE_WIRELESS | 0x80,
  CGEAR_PANELTYPE_WIFI_ICON = CGEAR_PANELTYPE_WIFI | 0x80,
} CGEAR_PANELTYPE_ENUM;

#define CGEAR_PANELTYPE_GET_ICON_TYPE(type) ((type) | 0x80)


//----------------------------------------------------------
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//----------------------------------------------------------
extern int CGEAR_SV_GetWorkSize(void);
extern CGEAR_SAVEDATA* CGEAR_SV_AllocWork(HEAPID heapID);
extern void CGEAR_SV_Init(CGEAR_SAVEDATA* pSV);

extern void CGEAR_SV_SetCGearONOFF(CGEAR_SAVEDATA* pSV,BOOL bON);
extern BOOL CGEAR_SV_GetCGearONOFF(CGEAR_SAVEDATA* pSV);
extern void CGEAR_SV_SetCGearPictureONOFF(CGEAR_SAVEDATA* pSV,BOOL bON);
extern BOOL CGEAR_SV_GetCGearPictureONOFF(CGEAR_SAVEDATA* pSV);
extern void CGEAR_SV_SetCGearPictureCRC(CGEAR_SAVEDATA* pSV,u16 pictureCRC);
extern u16 CGEAR_SV_GetCGearPictureCRC(CGEAR_SAVEDATA* pSV);


extern CGEAR_PANELTYPE_ENUM CGEAR_SV_GetPanelType(const CGEAR_SAVEDATA* pSV,int x, int y);
extern BOOL CGEAR_SV_IsPanelTypeIcon(const CGEAR_SAVEDATA* pSV,int x, int y);
extern void CGEAR_SV_SetPanelType(CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type);
extern u8* CGEAR_SV_GetPanelTypeBuff( CGEAR_SAVEDATA* pSV );

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
extern CGEAR_SAVEDATA* CGEAR_SV_GetCGearSaveData(SAVE_CONTROL_WORK* pSave);


