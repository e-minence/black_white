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
#include "gamesystem/game_data.h"
#include "system/gf_date.h"


#define CGEAR_PANELTYPE_GET_ICON_TYPE(type) ((type) | 0x80)
#define CGEAR_PANELTYPE_GET_LAST_IR(type) ((type) | 0x40)
#define CGEAR_PANELTYPE_GET_LAST_WIFI(type) ((type) | 0x20)
#define CGEAR_PANELTYPE_GET_LAST_WIRELESS(type) ((type) | 0x10)

//----------------------------------------------------------
/**
 * @brief  �\���̒�`
 */
//----------------------------------------------------------

struct _CGEAR_SAVEDATA {
	u8 type[C_GEAR_PANEL_WIDTH][C_GEAR_PANEL_HEIGHT];
  u16 CGearPictCRC;
  u8 CGearPictureON;
  u8 CGearON;
  u8 gearType;
  u8 CGearPowerON;
  u8 dummy2;
  u8 dummy3;
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
 * @brief   CGEAR�̊G���̋L�^�������Ȃ�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @param   type    �^�C�v
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetCGearType(CGEAR_SAVEDATA* pSV,u8 type)
{
  pSV->gearType = type;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief   CGEAR�̊G���𓾂�
 * @param   pSV     CGEAR�Z�[�u�\����
 * @return  type    �^�C�v
 */
//--------------------------------------------------------------------------------------------
u8 CGEAR_SV_GetCGearType(CGEAR_SAVEDATA* pSV)
{
  return pSV->gearType;
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
  // ���4�r�b�g�̓J�b�g
	return (pSV->type[x][y] & 0x0f);
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

//----------------------------------------------------------------------------
/**
 *	@brief  �P�܂����H
 *
 *	@param	pSV �Z�[�u
 *	@param	x   ��
 *	@param	y   ��
 *
 *	@retval TRUE
 *	@retval FALSE
 */
//-----------------------------------------------------------------------------
BOOL CGEAR_SV_IsPanelTypeLast(const CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type )
{
  u32 bit;
	GF_ASSERT(x < C_GEAR_PANEL_WIDTH);
	GF_ASSERT(y < C_GEAR_PANEL_HEIGHT);
  if(!(x < C_GEAR_PANEL_WIDTH)){
    return FALSE;
  }
  if(!(y < C_GEAR_PANEL_HEIGHT)){
    return FALSE;
  }

  bit = 0;
  if( type == CGEAR_PANELTYPE_IR ){
    bit = 0x40;
  }else if( type == CGEAR_PANELTYPE_WIFI ){
    bit = 0x20;
  }else if( type == CGEAR_PANELTYPE_WIRELESS ){
    bit = 0x10;
  }

  // ���1�r�b�g�̓J�b�g
  if( pSV->type[x][y] & bit ){
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
 * @param   icon  ICON������ꏊ
 * @param   last  �P�O��ICON���������ꏊ
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void CGEAR_SV_SetPanelType(CGEAR_SAVEDATA* pSV,int x, int y, CGEAR_PANELTYPE_ENUM type, BOOL icon, BOOL last)
{
  u32 set_type;
	GF_ASSERT(type < CGEAR_PANELTYPE_MAX);
  if(!(type < CGEAR_PANELTYPE_MAX)){
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
  set_type = type;
  if( (icon == TRUE) && 
      ( (type == CGEAR_PANELTYPE_IR) ||
      (type == CGEAR_PANELTYPE_WIFI) ||
      (type == CGEAR_PANELTYPE_WIRELESS) ) ){

    set_type = CGEAR_PANELTYPE_GET_ICON_TYPE(set_type);
  }
  if( last ){
    if( type == CGEAR_PANELTYPE_IR ){
      set_type = CGEAR_PANELTYPE_GET_LAST_IR(set_type);
    }else if( type == CGEAR_PANELTYPE_WIFI ){
      set_type = CGEAR_PANELTYPE_GET_LAST_WIFI(set_type);
    }else if( type == CGEAR_PANELTYPE_WIRELESS ){
      set_type = CGEAR_PANELTYPE_GET_LAST_WIRELESS(set_type);
    }
  }
	pSV->type[x][y] = set_type;
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


//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR �d���@�ݒ�
 *
 *	@param	pSV     �Z�[�u�f�[�^
 *	@param	flag    TRUE�FON�@�@FALSE�FOFF
 */
//-----------------------------------------------------------------------------
void CGEAR_SV_SetPowerFlag( CGEAR_SAVEDATA* pSV, BOOL flag )
{
  pSV->CGearPowerON = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  CGEAR�̓d��ON�EOFF���擾
 *
 *	@param	cpSV  �Z�[�u�f�[�^
 *
 *	@retval TRUE    ON
 *	@retval FALSE   OFF
 */
//-----------------------------------------------------------------------------
BOOL CGEAR_SV_GetPowerFlag( const CGEAR_SAVEDATA* cpSV )
{
  return cpSV->CGearPowerON;
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

CGEAR_SAVEDATA* GAMEDATA_GetCGearSaveData(GAMEDATA* pGameData)
{
	return CGEAR_SV_GetCGearSaveData(GAMEDATA_GetSaveControlWork(pGameData));
}

