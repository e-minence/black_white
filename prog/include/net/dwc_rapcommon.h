#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 * @file	dwc_rapcommon.h
 * @brief	Wi-Fi�f�o�C�X�Ƃ̃A�N�Z�X�֐� �ėp�̏���
 * @author	GAME FREAK Inc.
 * @date    2007.2.20
 */
//=============================================================================

#pragma once

#if GFL_NET_WIFI
#include "gamesystem/game_comm.h"  //GAME_COMM_STATUS
#include <dwc.h>


//==============================================================================
/**
 * DWC���C�u����������
 * @param   heapID   �q�[�vID
 * @retval  DS�{�̂ɕۑ����郆�[�UID�̃`�F�b�N�E�쐬���ʁB
 */
//==============================================================================
extern int mydwc_init(HEAPID heapID);

//==============================================================================
/**
 * @brief   DWC  UserData�����
 * @param   DWCUserData  ���[�U�[�f�[�^���Ȃ��ꍇ�쐬
 * @retval  none
 */
//==============================================================================
extern void mydwc_createUserData( DWCUserData *userdata );

//==============================================================================
/**
 * ������GSID���擾����
 * @param   GFL_WIFI_FRIENDLIST  
 * @retval  �O�Ɓ|�P�͎��s   ���̒l�͐���
 */
//==============================================================================

//extern int mydwc_getMyGSID(void);

//==============================================================================
/**
 * ������GSID���������̂���������
 * @retval  TRUE������
 */
//==============================================================================
extern BOOL mydwc_checkMyGSID(void);

//==============================================================================
/**
 * @brief   WIFI�Ŏg��HEAPID���Z�b�g����
 * @param   id     �ύX����HEAPID
 * @retval  none
 */
//==============================================================================
extern void DWC_RAPCOMMON_SetHeapID(HEAPID id,int size);
extern void DWC_RAPCOMMON_ResetHeapID(void);
extern void* DWC_RAPCOMMON_Alloc( DWCAllocType name, u32 size, int align );
extern void DWC_RAPCOMMON_Free( DWCAllocType name, void *ptr, u32 size );

extern void DWC_RAPCOMMON_SetSubHeapID( DWCAllocType SubAllocType, u32 size, HEAPID heapID );
extern void DWC_RAPCOMMON_SetSubHeapIDEx( DWCAllocType SubAllocType, u32 size, u32 limit_size, HEAPID heapID );

#define DWC_RAPCOMMON_AUTORELEASE_HEAPID_NONE 0
extern void DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID heapID );
extern void DWC_RAPCOMMON_ResetSubHeapID(void);

//------------------------------------------------------------------------------
/**
 * @brief   �r�[�R���̃^�C�v�𓾂�
 * @param   �r�[�R����
 * @param   �q�[�vID
 * @retval  GAME_COMM_STATUS
 */
//------------------------------------------------------------------------------
extern GAME_COMM_STATUS DWC_RAPCOMMON_GetBeaconType(int num, HEAPID heap);

#endif //

#ifdef __cplusplus
} /* extern "C" */
#endif
