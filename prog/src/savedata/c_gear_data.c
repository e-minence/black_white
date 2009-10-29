//============================================================================================
/**
 * @file	  c_gear_data.c
 * @brief	  CGEAR�ŃZ�[�u���Ă����f�[�^
 * @author	k.ohno
 * @date	  2009.05.14
 */
//============================================================================================

#include "gflib.h"

#include "savedata/save_tbl.h"
#include "savedata/c_gear_data.h"

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _CGEAR_SAVEDATA {
	u8 type[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];
};


static u8 _inittable[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT]={
	{CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_NONE} ,
	{CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE},
	{CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE},
	{CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE},
	{CGEAR_PANELTYPE_IR,
		CGEAR_PANELTYPE_WIFI,
		CGEAR_PANELTYPE_WIRELESS,
		CGEAR_PANELTYPE_NONE},
	//--------------------------------------
	{CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE},
	{CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE},
	{CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE},
	{CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_BASE,
		CGEAR_PANELTYPE_NONE,
		CGEAR_PANELTYPE_NONE},
};



//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̃T�C�Y�𓾂�
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------
int CGEAR_SV_GetWorkSize(void)
{
	return sizeof(CGEAR_SAVEDATA);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   �\���̂𓾂�
 * @return	�\����
 */
//--------------------------------------------------------------------------------------------

CGEAR_SAVEDATA* CGEAR_SV_AllocWork(HEAPID heapID)
{
	return GFL_HEAP_AllocClearMemory(heapID, CGEAR_SV_GetWorkSize());
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   ���g������������
 * @return	�T�C�Y
 */
//--------------------------------------------------------------------------------------------

void CGEAR_SV_Init(CGEAR_SAVEDATA* pSV)
{
	GFL_STD_MemClear(pSV, CGEAR_SV_GetWorkSize());
	GFL_STD_MemCopy(_inittable,pSV->type,sizeof(_inittable));
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�p�l���^�C�v�𓾂�
 * @param   x     �p�l�����ʒu
 * @param   y     �p�l���c�ʒu
 * @return	type  �p�l���^�C�v CGEAR_PANELTYPE_ENUM
 */
//--------------------------------------------------------------------------------------------
CGEAR_PANELTYPE_ENUM CGEAR_SV_GetPanelType(CGEAR_SAVEDATA* pSV,int x, int y)
{
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
	GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
	return pSV->type[x][y];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�p�l���^�C�v��ݒ�
 * @param   x     �p�l�����ʒu
 * @param   y     �p�l���c�ʒu
 * @param   type  �p�l���^�C�v CGEAR_PANELTYPE_ENUM
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetPanelType(CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type)
{
	GF_ASSERT(type < CGEAR_PANELTYPE_MAX);
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
	GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
	pSV->type[x][y] = type;
}

//----------------------------------------------------------
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//----------------------------------------------------------
CGEAR_SAVEDATA* CGEAR_SV_GetCGearSaveData(SAVE_CONTROL_WORK* pSave)
{
	CGEAR_SAVEDATA* pData;
	pData = SaveControl_DataPtrGet(pSave, GMDATA_ID_CGEAR);
	return pData;

}

