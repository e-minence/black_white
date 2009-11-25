//==============================================================================
/**
 * @file	wazatype_icon.c
 * @brief	�Z�^�C�v�����ރA�C�R��
 * @author	matsuda
 * @date	2005.10.26(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/wazatype_icon.h"
#include "arc_def.h"
#include "wazatype_icon.naix"


//==============================================================================
//	�f�[�^
//==============================================================================
//--------------------------------------------------------------
//	�Z�^�C�v
//--------------------------------------------------------------
///�Z�^�C�v����CGRID
ALIGN4 static const ARCDATID WazaTypeCgrNo[] = {
	NARC_wazatype_icon_p_st_type_normal_NCGR,			   //POKETYPE_NORMAL
	NARC_wazatype_icon_p_st_type_fight_NCGR,               //POKETYPE_BATTLE
	NARC_wazatype_icon_p_st_type_flight_NCGR,              //POKETYPE_HIKOU
	NARC_wazatype_icon_p_st_type_poison_NCGR,              //POKETYPE_POISON
	NARC_wazatype_icon_p_st_type_ground_NCGR,              //POKETYPE_JIMEN
	NARC_wazatype_icon_p_st_type_rock_NCGR,                //POKETYPE_IWA
	NARC_wazatype_icon_p_st_type_insect_NCGR,              //POKETYPE_MUSHI
	NARC_wazatype_icon_p_st_type_ghost_NCGR,               //POKETYPE_GHOST
	NARC_wazatype_icon_p_st_type_steel_NCGR,               //POKETYPE_METAL
	NARC_wazatype_icon_p_st_type_fire_NCGR,                //POKETYPE_FIRE
	NARC_wazatype_icon_p_st_type_water_NCGR,               //POKETYPE_WATER
	NARC_wazatype_icon_p_st_type_grass_NCGR,               //POKETYPE_KUSA
	NARC_wazatype_icon_p_st_type_ele_NCGR,                 //POKETYPE_ELECTRIC
	NARC_wazatype_icon_p_st_type_esp_NCGR,                 //POKETYPE_SP
	NARC_wazatype_icon_p_st_type_ice_NCGR,                 //POKETYPE_KOORI
	NARC_wazatype_icon_p_st_type_dragon_NCGR,              //POKETYPE_DRAGON
	NARC_wazatype_icon_p_st_type_evil_NCGR,                //POKETYPE_AKU
	NARC_wazatype_icon_p_st_type_style_NCGR,               //ICONTYPE_STYLE,		///<�i�D�悳
	NARC_wazatype_icon_p_st_type_beautiful_NCGR,           //ICONTYPE_BEAUTIFUL,	///<������
	NARC_wazatype_icon_p_st_type_cute_NCGR,                //ICONTYPE_CUTE,			///<����
	NARC_wazatype_icon_p_st_type_intelli_NCGR,             //ICONTYPE_INTELLI,		///<����
	NARC_wazatype_icon_p_st_type_strong_NCGR,              //ICONTYPE_STRONG,		///<痂���
};

///�Z�^�C�v���̃p���b�g�I�t�Z�b�g�ԍ�
ALIGN4 static const u8 WazaTypePlttOffset[] = {
	0,	//POKETYPE_NORMAL
	0,	//POKETYPE_BATTLE
	1,	//POKETYPE_HIKOU
	1,	//POKETYPE_POISON
	0,	//POKETYPE_JIMEN
	0,	//POKETYPE_IWA
	2,	//POKETYPE_MUSHI
	1,	//POKETYPE_GHOST
	0,	//POKETYPE_METAL
	0,	//POKETYPE_FIRE
	1,	//POKETYPE_WATER
	2,	//POKETYPE_KUSA
	0,	//POKETYPE_ELECTRIC
	1,	//POKETYPE_SP
	1,	//POKETYPE_KOORI
	2,	//POKETYPE_DRAGON
	0,	//POKETYPE_AKU
	0,	//ICONTYPE_STYLE,			///<�i�D�悳
	1,	//ICONTYPE_BEAUTIFUL,		///<������
	1,	//ICONTYPE_CUTE,			///<����
	2,	//ICONTYPE_INTELLI,		///<����
	0,	//ICONTYPE_STRONG,		///<痂���
};

//--------------------------------------------------------------
//	����
//--------------------------------------------------------------
///���ޖ���CGRID
ALIGN4 static const u32 WazaKindCgrNo[] = {
	NARC_wazatype_icon_p_st_bunrui_buturi_NCGR,
	NARC_wazatype_icon_p_st_bunrui_tokusyu_NCGR,
	NARC_wazatype_icon_p_st_bunrui_henka_NCGR,
};

///���ޖ��̃p���b�g�I�t�Z�b�g�ԍ�
ALIGN4 static const u8 WazaKindPlttOffset[] = {
	0,		//KIND_BUTSURI
	1,		//KIND_TOKUSYU
	0,		//KIND_HENNKA
};



//==============================================================================
//
//	�Z�^�C�v�A�C�R��
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �w��Z�^�C�v��CGRID���擾���܂�
 * @param   waza_type		�Z�^�C�v(POKETYPE_NORMAL, POKETYPE_BATTLE..���A���� ICONTYPE_???)
 * @retval  CGRID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaTypeIcon_CgrIDGet(int waza_type)
{
	GF_ASSERT(waza_type < NELEMS(WazaTypeCgrNo));
	return WazaTypeCgrNo[waza_type];
}

//--------------------------------------------------------------
/**
 * @brief   �Z�^�C�v�A�C�R���̃p���b�gID���擾���܂�
 * @retval  �p���b�gID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaTypeIcon_PlttIDGet(void)
{
	return NARC_wazatype_icon_st_type_NCLR;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z�^�C�v�̃Z��ID���擾���܂�
 * @retval  �Z��ID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaTypeIcon_CellIDGet(void)
{
	return NARC_wazatype_icon_p_st_type_normal_NCER;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z�^�C�v�̃Z���A�j��ID���擾���܂�
 * @retval  �Z���A�j��ID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaTypeIcon_CellAnmIDGet(void)
{
	return NARC_wazatype_icon_p_st_type_normal_NANR;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z�^�C�v�̃p���b�g�ԍ��I�t�Z�b�g���擾���܂�
 * @param   waza_type		�Z�^�C�v(NORMAL_TYPE, FIRE_TYPE..���A���� ICONTYPE_???)
 * @retval  �p���b�g�ԍ��̃I�t�Z�b�g
 */
//--------------------------------------------------------------
u8 WazaTypeIcon_PlttOffsetGet(int waza_type)
{
	GF_ASSERT(waza_type < NELEMS(WazaTypePlttOffset));
	return WazaTypePlttOffset[waza_type];
}

//--------------------------------------------------------------
/**
 * @brief   �Z�^�C�v�A�C�R���̃A�[�J�C�uINDEX���擾���܂�
 * @retval  �A�[�J�C�uINDEX
 */
//--------------------------------------------------------------
ARCID WazaTypeIcon_ArcIDGet(void)
{
	return ARCID_WAZATYPE_ICON;
}





//==============================================================================
//
//	���ރA�C�R��
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �w��Z���ނ�CGRID���擾���܂�
 * @param   waza_kind		�Z����(KIND_BUTURI or KIND_TOKUSYU or KIND_HENNKA)
 * @retval  CGRID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaKindIcon_CgrIDGet(int waza_kind)
{
	GF_ASSERT(waza_kind < NELEMS(WazaKindCgrNo));
	return WazaKindCgrNo[waza_kind];
}

//--------------------------------------------------------------
/**
 * @brief   �Z���ރA�C�R���̃p���b�gID���擾���܂�
 * @retval  �p���b�gID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaKindIcon_PlttIDGet(void)
{
	return NARC_wazatype_icon_st_type_NCLR;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z���ނ̃Z��ID���擾���܂�
 * @retval  �Z��ID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaKindIcon_CellIDGet(void)
{
	return NARC_wazatype_icon_p_st_type_normal_NCER;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z���ނ̃Z���A�j��ID���擾���܂�
 * @retval  �Z���A�j��ID(ARC�̃f�[�^INDEX)
 */
//--------------------------------------------------------------
ARCDATID WazaKindIcon_CellAnmIDGet(void)
{
	return NARC_wazatype_icon_p_st_type_normal_NANR;
}

//--------------------------------------------------------------
/**
 * @brief   �w��Z���ނ̃p���b�g�ԍ��I�t�Z�b�g���擾���܂�
 * @param   waza_kind		�Z����(KIND_BUTURI or KIND_TOKUSYU or KIND_HENNKA)
 * @retval  �p���b�g�ԍ��̃I�t�Z�b�g
 */
//--------------------------------------------------------------
u8 WazaKindIcon_PlttOffsetGet(int waza_kind)
{
	GF_ASSERT(waza_kind < NELEMS(WazaKindPlttOffset));
	return WazaKindPlttOffset[waza_kind];
}

//--------------------------------------------------------------
/**
 * @brief   �Z���ރA�C�R���̃A�[�J�C�uINDEX���擾���܂�
 * @retval  �A�[�J�C�uINDEX
 */
//--------------------------------------------------------------
ARCID WazaKindIcon_ArcIDGet(void)
{
	return ARCID_WAZATYPE_ICON;
}


