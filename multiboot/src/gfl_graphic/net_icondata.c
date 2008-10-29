//============================================================================================
/**
 * @file	net_icondata.c
 * @brief	�ʐM�A�C�R���f�[�^�����̒�`
 * @author	ohno
 * @date	2007.05.29
 */
//============================================================================================

#include "gflib.h"
#include "arc_def.h"
#include "wmi.naix"


//===============================02030A0C===============================================
/**
 * @brief    �ʐM�A�C�R���O���t�B�b�N�e�[�u���|�C���^�𓾂�
 * @param    none
 * @return   �O���t�B�b�N�e�[�u���|�C���^
 */
//==============================================================================

const u32 NET_ICONDATA_GetTableID(void)
{
    return ARCID_WMI;
}


//==============================================================================
/**
 * @brief    �ʐM�A�C�R���O���t�B�b�NNO�𓾂�
 * @param    pNoBuff  �O���t�B�b�N�ԍ���Ԃ��z��
 * @return   none
 */
//==============================================================================

void NET_ICONDATA_GetNoBuff(int* pNoBuff)
{
    pNoBuff[GFL_NET_ICON_WIFINCGR] = NARC_wmi_wifi_ncgr;
    pNoBuff[GFL_NET_ICON_WMNCGR] = NARC_wmi_wm_ncgr;
    pNoBuff[GFL_NET_ICON_WMNCLR] = NARC_wmi_wm_nclr;
}

