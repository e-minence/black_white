//=============================================================================
/**
 * @file	net_devirc.c
 * @bfief	IRC�ʐM�h���C�o�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/22
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"

extern GFLNetDevTable *NET_GetIrcDeviceTable(void);


//--------------------------------------------
// �\����
//--------------------------------------------

static GFLNetDevTable netDevTbl={
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,

};


//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�e�[�u����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
GFLNetDevTable *NET_GetIrcDeviceTable(void)
{
    return &netDevTbl;
}
