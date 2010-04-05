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
#include "system/gf_date.h"

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _CGEAR_SAVEDATA {
#if 0
  u16 CGearPictCRC;
  u8 gearType;
  u8 dummy;
	u8 type[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];
  u8 irMarkx;
  u8 irMarky;
  u8 wirlessMarkx;
  u8 wirlessMarky;
  u8 wifiMarkx;
  u8 wifiMarkx;
  u8 CGearPictureON;
  u8 CGearON;
#else
	u8 type[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];
  u16 CGearPictCRC;
  u8 CGearPictureON;
  u8 CGearON;
#endif
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
	{CGEAR_PANELTYPE_GET_ICON_TYPE(CGEAR_PANELTYPE_IR),
		CGEAR_PANELTYPE_GET_ICON_TYPE(CGEAR_PANELTYPE_WIFI),
		CGEAR_PANELTYPE_GET_ICON_TYPE(CGEAR_PANELTYPE_WIRELESS),
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
 * @brief   CGEAR��ONOFF�������Ȃ�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @param   bON     ON�Ȃ�TRUE OFF�Ȃ�FALSE
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetCGearONOFF(CGEAR_SAVEDATA* pSV,BOOL bON)
{
  pSV->CGearON = bON;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR��ONOFF�𓾂�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @return   ON�Ȃ�TRUE OFF�Ȃ�FALSE
 */
//--------------------------------------------------------------------------------------------
BOOL CGEAR_SV_GetCGearONOFF(CGEAR_SAVEDATA* pSV)
{
  return pSV->CGearON;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�̊G�������Ă��邩�ǂ���ONOFF�������Ȃ�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @param   bON     ON�Ȃ�TRUE OFF�Ȃ�FALSE
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetCGearPictureONOFF(CGEAR_SAVEDATA* pSV,BOOL bON)
{
  pSV->CGearPictureON = bON;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�̊G�����邩ONOFF�𓾂�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @return   ON�Ȃ�TRUE OFF�Ȃ�FALSE
 */
//--------------------------------------------------------------------------------------------
BOOL CGEAR_SV_GetCGearPictureONOFF(CGEAR_SAVEDATA* pSV)
{
  return pSV->CGearPictureON;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    CGEAR�̊G�������Ă��邩�ǂ����m�F�̈�CRC�ۑ�
 * @param    pSV     CGEAR�Z�[�u�\����
 * @param    pictureCRC     CRC
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetCGearPictureCRC(CGEAR_SAVEDATA* pSV,u16 pictureCRC)
{
  pSV->CGearPictCRC = pictureCRC;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    CGEAR�̊G�����邩�m�F�̈�CRCGET
 * @param    pSV     CGEAR�Z�[�u�\����
 * @return   Picture��CRC
 */
//--------------------------------------------------------------------------------------------
u16 CGEAR_SV_GetCGearPictureCRC(CGEAR_SAVEDATA* pSV)
{
  return pSV->CGearPictCRC;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�p�l���^�C�v�𓾂�
 * @param   x     �p�l�����ʒu
 * @param   y     �p�l���c�ʒu
 * @return	type  �p�l���^�C�v CGEAR_PANELTYPE_ENUM
 */
//--------------------------------------------------------------------------------------------
CGEAR_PANELTYPE_ENUM CGEAR_SV_GetPanelType(const CGEAR_SAVEDATA* pSV,int x, int y)
{
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
  if(!(x < C_GEAR_PANEL_WIDTH)){
    return CGEAR_PANELTYPE_NONE;
  }
  GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
  if(!(y < C_GEAR_PANEL_HEIGHT)){
    return CGEAR_PANELTYPE_NONE;
  }
  // ���1�r�b�g�̓J�b�g
	return pSV->type[x][y] & 0x7f;
}

//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR �p�l���^�C�v��ICON�����`�F�b�N
 *
 *	@param	pSV �Z�[�u
 *	@param	x   ��
 *	@param	y�@ ��
 *
 *	@retval TRUE  ICON���̏ꏊ
 *	@retval FALSE ICON������Ȃ��΂���
 */
//-----------------------------------------------------------------------------
BOOL CGEAR_SV_IsPanelTypeIcon(const CGEAR_SAVEDATA* pSV,int x, int y)
{
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
	GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
  if(!(x < C_GEAR_PANEL_WIDTH)){
    return FALSE;
  }
  if(!(y < C_GEAR_PANEL_HEIGHT)){
    return FALSE;
  }
  // ���1�r�b�g�̓J�b�g
  if( pSV->type[x][y] & 0x80 ){
  	return TRUE;
  }

  return FALSE;
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
  u32 org_type;
  org_type = type & 0x7f;
	GF_ASSERT(org_type < CGEAR_PANELTYPE_MAX);
  if(!(org_type < CGEAR_PANELTYPE_MAX)){
    return;
  }
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
  if(!(x < C_GEAR_PANEL_WIDTH)){
    return;
  }
	GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
  if(!(y < C_GEAR_PANEL_HEIGHT)){
    return;
  }
	pSV->type[x][y] = type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �p�l���^�C�v�̃Z�[�u�o�b�t�@���擾
 *
 *	@param	pSV   ���[�N
 *
 *	@return �o�b�t�@
 */
//-----------------------------------------------------------------------------
u8* CGEAR_SV_GetPanelTypeBuff( CGEAR_SAVEDATA* pSV )
{
  return &pSV->type[0][0];
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

