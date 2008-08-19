//============================================================================================
/**
 * @file	net_icondata.c
 * @brief	�ʐM�A�C�R���f�[�^�����̒�` gflib�ɒu���Ȃ��̂ŊO���ɒ�`���Ă���
 * @author	ohno
 * @date	2007.05.29
 */
//============================================================================================

#include "gflib.h"
#include "wmi.naix"
#include "net_icondata.h"
#include "arc_def.h"


//==============================================================================
/**
 * @brief    �ʐM�A�C�R���O���t�B�b�NARCID�𓾂�
 * @param    none
 * @return   �O���t�B�b�NARCID
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
    pNoBuff[GFL_NET_ICON_WIFINCGR] = NARC_wmi_wifi_NCGR;
    pNoBuff[GFL_NET_ICON_WMNCGR] = NARC_wmi_wm_NCGR;
    pNoBuff[GFL_NET_ICON_WMNCLR] = NARC_wmi_wm_NCLR;
}

