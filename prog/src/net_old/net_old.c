//==============================================================================
/**
 * @file    net_old.c
 * @brief   ���ʐM�V�X�e���̃��[�h�A�A�����[�h�Ǘ�
 * @author  matsuda
 * @date    2009.06.04(��)
 */
//==============================================================================
#include <gflib.h>
#include "net_old/comm_system.h"
#include "net_old/net_old.h"


//==============================================================================
//  �O���[�o���ϐ�
//==============================================================================
BOOL NetOldLoad = FALSE;


//==============================================================================
//  �I�[�o�[���CID
//==============================================================================
FS_EXTERN_OVERLAY(netold);
FS_EXTERN_OVERLAY(dev_wifilib);



//==================================================================
/**
 * ���ʐM�V�X�e���̃��[�h
 */
//==================================================================
void NetOld_Load(void)
{
  GF_ASSERT(NetOldLoad == FALSE);
  if(NetOldLoad == FALSE){
    GFL_OVERLAY_Load( FS_OVERLAY_ID( dev_wifilib ) );
    GFL_OVERLAY_Load(FS_OVERLAY_ID(netold));
    NetOldLoad = TRUE;
  }
}

//==================================================================
/**
 * ���ʐM�V�X�e���̃A�����[�h
 */
//==================================================================
void NetOld_Unload(void)
{
  GF_ASSERT(NetOldLoad == TRUE);
  if(NetOldLoad == TRUE){
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(netold));
    GFL_OVERLAY_Unload(FS_OVERLAY_ID(dev_wifilib));
    NetOldLoad = FALSE;
  }
}

//==================================================================
/**
 * ���ʐM�V�X�e���̍X�V����
 */
//==================================================================
void NetOld_Update(void)
{
  if(NetOldLoad == FALSE){
    return;
  }
  
  CommUpdateData();
}
