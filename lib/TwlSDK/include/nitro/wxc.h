/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     wxc.h

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef	NITRO_WXC_H_
#define	NITRO_WXC_H_


#include <nitro.h>

#include <nitro/wxc/common.h>
#include <nitro/wxc/protocol.h>
#include <nitro/wxc/driver.h>
#include <nitro/wxc/scheduler.h>

#include <nitro/wxc/wxc_protocol_impl_wxc.h>


/*****************************************************************************/
/* constant */

/* ���C�u�����ɕK�v�ȓ������[�N�������̃T�C�Y */
#define WXC_WORK_SIZE              0xA000


/*****************************************************************************/
/* function */

#ifdef  __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         WXC_Init

  Description:  ���C�u����������������.

  Arguments:    work          ���C�u���������Ŏg�p���郏�[�N������.
                              WXC_WORK_SIZE �o�C�g�ȏ��, ����
                              32 BYTE �A���C�������g����Ă���K�v������.
                callback      ���C�u�����̊e��V�X�e����Ԃ�ʒm����
                              �R�[���o�b�N�ւ̃|�C���^.
                dma           ���C�u���������Ŏg�p���� DMA �`�����l��.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_Init(void *work, WXCCallback callback, u32 dma);

/*---------------------------------------------------------------------------*
  Name:         WXC_Start

  Description:  ���C�u�����̃��C�����X�쓮���J�n����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_Start(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_Stop

  Description:  ���C�u�����̃��C�����X�쓮���~����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_Stop(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_End

  Description:  ���C�u�������I������.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_End(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetStateCode

  Description:  ���݂̃��C�u������Ԃ��擾.

  Arguments:    None.

  Returns:      ���݂̃��C�u������Ԃ����� WXCStateCode �^.
 *---------------------------------------------------------------------------*/
WXCStateCode WXC_GetStateCode(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_IsParentMode

  Description:  ���݂̃��C�����X��Ԃ��e�@���[�h������.
                WXC_STATE_ACTIVE �̏�ԂɌ���L��.

  Arguments:    None.

  Returns:      ���C�����X��Ԃ��e�@���[�h�Ȃ� TRUE, �q�@���[�h�Ȃ� FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WXC_IsParentMode(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetParentParam

  Description:  ���݂̃��C�����X��Ԃ� WMParentParam �\���̂��Q��.
                WXC_STATE_ACTIVE ���� WXC_IsParentMode() ��
                TRUE �̏�ԂɌ���L��.

  Arguments:    None.

  Returns:      WMParentParam �\����.
 *---------------------------------------------------------------------------*/
const WMParentParam *WXC_GetParentParam(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetParentBssDesc

  Description:  ���ݐڑ��Ώۂ� WMBssDesc �\���̂��Q��.
                WXC_STATE_ACTIVE ���� WXC_IsParentMode() ��
                FALSE �̏�ԂɌ���L��.

  Arguments:    None.

  Returns:      WMBssDesc �\����.
 *---------------------------------------------------------------------------*/
const WMBssDesc *WXC_GetParentBssDesc(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetUserBitmap

  Description:  ���ݐڑ����̃��[�U�󋵂��r�b�g�}�b�v�Ŏ擾.
                �ʐM���m������Ă��Ȃ��󋵂ł� 0 ��Ԃ�.

  Arguments:    None.

  Returns:      WMBssDesc �\����.
 *---------------------------------------------------------------------------*/
u16     WXC_GetUserBitmap(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetCurrentGgid

  Description:  ���݂̐ڑ��őI������Ă���GGID���擾.
                �ʐM���m������Ă��Ȃ��󋵂ł� 0 ��Ԃ�.
                ���݂̒ʐM��Ԃ� WXC_GetUserBitmap() �֐��̕Ԃ�l�Ŕ���\.

  Arguments:    None.

  Returns:      ���݂̐ڑ��őI������Ă���GGID.
 *---------------------------------------------------------------------------*/
u32     WXC_GetCurrentGgid(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetOwnAid

  Description:  ���݂̐ڑ��ɂ����鎩�g��AID���擾.
                �ʐM���m������Ă��Ȃ��󋵂ł͕s��Ȓl��Ԃ�.
                ���݂̒ʐM��Ԃ� WXC_GetUserBitmap() �֐��̕Ԃ�l�Ŕ���\.

  Arguments:    None.

  Returns:      ���݂̐ڑ��ɂ����鎩�g��AID.
 *---------------------------------------------------------------------------*/
u16     WXC_GetOwnAid(void);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetUserInfo

  Description:  ���ݐڑ����̃��[�U�����擾.
                �Ԃ����|�C���^���w���Ă�����̓��e��ύX���Ă͂Ȃ�Ȃ�.

  Arguments:    aid           �����擾���郆�[�U��AID.

  Returns:      �w�肳�ꂽAID���L���Ȃ�΃��[�U���, �����łȂ���� NULL
 *---------------------------------------------------------------------------*/
const WXCUserInfo *WXC_GetUserInfo(u16 aid);

/*---------------------------------------------------------------------------*
  Name:         WXC_SetChildMode

  Description:  ���C�����X���q�@���Ƃ��Ă����N�������Ȃ��悤�ݒ�.
                ���̊֐��� WXC_Start �֐����O�ɌĂяo���K�v������.

  Arguments:    enable        �q�@���ł����N�������Ȃ��ꍇ�� TRUE

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_SetChildMode(BOOL enable);

/*---------------------------------------------------------------------------*
  Name:         WXC_SetParentParameter

  Description:  ���C�����X�̒ʐM�ݒ���w�肷��.
                �ݒ���e�͐e�@���[�h�̏�Ԏ��ł̂ݎg�p����,
                �q�@���[�h�Őڑ������ꍇ�͐e�@���̐ݒ�ɏ]��.
                ���̊֐��̓I�v�V�����ł���, �K�v�ȏꍇ�Ɍ���
                WXC_Start �֐����O�ɌĂяo��.

  Arguments:    sendSize      �e�@���M�T�C�Y.
                              WXC_PACKET_SIZE_MIN �ȏォ��
                              WXC_PACKET_SIZE_MAX �ȉ��ł���K�v������.
                recvSize      �e�@��M�T�C�Y.
                              WXC_PACKET_SIZE_MIN �ȏォ��
                              WXC_PACKET_SIZE_MAX �ȉ��ł���K�v������.
                maxEntry      �ő�ڑ��l��.
                              ���݂̎����ł͂��̎w��� 1 �̂݃T�|�[�g�����.

  Returns:      �ݒ肪�L���ł���Γ����ɔ��f���� TRUE,
                �����łȂ���Όx�������f�o�b�O�o�͂��� FALSE.
 *---------------------------------------------------------------------------*/
BOOL    WXC_SetParentParameter(u16 sendSize, u16 recvSize, u16 maxEntry);

/*---------------------------------------------------------------------------*
  Name:         WXC_RegisterDataEx

  Description:  �����p�̃f�[�^��o�^����

  Arguments:    ggid          �o�^�f�[�^��GGID
                callback      ���[�U�[�ւ̃R�[���o�b�N�֐��i�f�[�^�����������ɃR�[���o�b�N�����j
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y
                type          �v���g�R��������������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_RegisterDataEx(u32 ggid, WXCCallback callback, u8 *send_ptr, u32 send_size,
                           u8 *recv_ptr, u32 recv_size, const char *type);

#define WXC_RegisterData(...) SDK_OBSOLETE_FUNCTION("WXC_RegisterData() is obsolete. use WXC_RegisterCommonData()")

/*---------------------------------------------------------------------------*
  Name:         WXC_RegisterCommonData

  Description:  �����p�̃f�[�^�����ʂ��ꂿ�����ʐM�d�l�œo�^����

  Arguments:    ggid          �o�^�f�[�^��GGID
                callback      ���[�U�[�ւ̃R�[���o�b�N�֐��i�f�[�^�����������ɃR�[���o�b�N�����j
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXC_RegisterCommonData(u32 ggid, WXCCallback callback,
                                          u8 *send_ptr, u32 send_size,
                                          u8 *recv_ptr, u32 recv_size)
{
    WXC_RegisterDataEx((u32)(ggid | WXC_GGID_COMMON_BIT), callback, send_ptr,
                             send_size, recv_ptr, recv_size, "COMMON");
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetInitialData

  Description:  ����f�[�^�����Ŗ���g�p����f�[�^�u���b�N���w�肷��

  Arguments:    ggid          �o�^�f�[�^��GGID
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_SetInitialData(u32 ggid, u8 *send_ptr, u32 send_size, u8 *recv_ptr, u32 recv_size);

/*---------------------------------------------------------------------------*
  Name:         WXC_AddData

  Description:  ���������u���b�N�f�[�^�����ɒǉ��Ńf�[�^��ݒ�

  Arguments:    send_buf      ���M�o�b�t�@.
                send_size     ���M�o�b�t�@�T�C�Y.
                recv_buf      ��M�o�b�t�@.
                recv_max      ��M�o�b�t�@�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_AddData(const void *send_buf, u32 send_size, void *recv_buf, u32 recv_max);

/*---------------------------------------------------------------------------*
  Name:         WXC_UnregisterData

  Description:  �����p�̃f�[�^��o�^����j������

  Arguments:    ggid          �j������u���b�N�Ɋ֘A�t����ꂽ GGID

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_UnregisterData(u32 ggid);

/*---------------------------------------------------------------------------*
  Name:         WXCi_SetSsid

  Description:  �q�@�Ƃ��Đڑ�����ۂ� SSID ��ݒ肷��.
                �����֐�.

  Arguments:    buffer        �ݒ肷SSID�f�[�^.
                length        SSID�f�[�^��.
                              WM_SIZE_CHILD_SSID �ȉ��ł���K�v������.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXCi_SetSsid(const void *buffer, u32 length);


#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
