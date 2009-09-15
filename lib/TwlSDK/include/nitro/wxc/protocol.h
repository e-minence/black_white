/*---------------------------------------------------------------------------*
  Project:  TwlSDK - WXC - include -
  File:     protocol.h

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

#ifndef	NITRO_WXC_PROTOCOL_H_
#define	NITRO_WXC_PROTOCOL_H_


#include <nitro.h>
#include <nitro/wxc/common.h>


/*****************************************************************************/
/* constant */

/* �R�}���h�^�C�v */
#define WXC_BLOCK_COMMAND_IDLE      0  /* �v��/�������Ȃ���� */
#define WXC_BLOCK_COMMAND_INIT      1  /* �f�[�^�T�C�Y�ƃ`�F�b�N�T������M */
#define WXC_BLOCK_COMMAND_SEND      2  /* �f�[�^�u���b�N����M */
#define WXC_BLOCK_COMMAND_STOP      3  /* ����M�L�����Z�� */
#define WXC_BLOCK_COMMAND_DONE      4  /* ����M���� */
#define WXC_BLOCK_COMMAND_QUIT      5  /* �ʐM�I���v�� (�ؒf) */

/* 1 ��� MP �p�P�b�g�T�C�Y */
#define WXC_PACKET_BUFFRER_MIN      (int)(WXC_PACKET_SIZE_MIN - sizeof(WXCBlockHeader))
/* �f�[�^�����Ɏg�p����ő�f�[�^�� (32kB) */
#define WXC_MAX_DATA_SIZE           (32 * 1024)
#define WXC_RECV_BITSET_SIZE        ((((WXC_MAX_DATA_SIZE + WXC_PACKET_BUFFRER_MIN - 1) / WXC_PACKET_BUFFRER_MIN) + 31) / 32)

/* �u���b�N�f�[�^�������̏d���h�~�p�C���f�b�N�X���� */
#define WXC_RECENT_SENT_LIST_MAX    2

/* �o�^�\�ȃf�[�^�u���b�N�̑��� */
#define WXC_REGISTER_DATA_MAX       16


/*****************************************************************************/
/* declaration */

/* �u���b�N�f�[�^�����̑���M��� */
typedef struct WXCBlockInfo
{
    u8      phase;                     /* �u���b�N�f�[�^�����̃t�F�[�Y */
    u8      command;                   /* ����̃t�F�[�Y�ł̃R�}���h */
    u16     index;                     /* �v������V�[�P���X�ԍ� */
}
WXCBlockInfo;

/* �u���b�N�f�[�^�����̃p�P�b�g�w�b�_ */
typedef struct WXCBlockHeader
{
    WXCBlockInfo req;                  /* ���M������̗v�� */
    WXCBlockInfo ack;                  /* ���M��ւ̉��� */
}
WXCBlockHeader;

SDK_STATIC_ASSERT(sizeof(WXCBlockHeader) == 8);


/* �u���b�N�f�[�^���\���� */
typedef struct WXCBlockDataFormat
{
    void   *buffer;                    /* �o�b�t�@�ւ̃|�C���^ */
    u32     length;                    /* �f�[�^�T�C�Y */
    u32     buffer_max;                /* �o�b�t�@�T�C�Y (���M�f�[�^�Ȃ� length �ƈ�v) */
    u16     checksum;                  /* �`�F�b�N�T�� (MATH_CalcChecksum8() ���g�p) */
    u8      padding[2];
}
WXCBlockDataFormat;


struct WXCProtocolContext;


/* WXC �őΉ��\�ȒʐM�v���g�R���̃C���^�t�F�[�X���K�肷��\���� */
typedef struct WXCProtocolImpl
{
    /* �v���g�R����ʂ����������� */
    const char *name;
    /* �r�[�R������M���̃t�b�N�֐� */
    void    (*BeaconSend) (struct WXCProtocolContext *, WMParentParam *);
    BOOL    (*BeaconRecv) (struct WXCProtocolContext *, const WMBssDesc *);
    /* �ڑ��O�ɌĂяo�����t�b�N�֐� */
    void    (*PreConnectHook) (struct WXCProtocolContext *, const WMBssDesc *, u8 ssid[WM_SIZE_CHILD_SSID]);
    /* MP�p�P�b�g����M���̃t�b�N�֐� */
    void    (*PacketSend) (struct WXCProtocolContext *, WXCPacketInfo *);
    BOOL    (*PacketRecv) (struct WXCProtocolContext *, const WXCPacketInfo *);
    /* �ڑ����̏������֐� */
    void    (*Init) (struct WXCProtocolContext *, u16, u16);
    /* �f�[�^�u���b�N�o�^�֐� */
    BOOL    (*AddData) (struct WXCProtocolContext *, const void *, u32, void *, u32);
    /* ���݃f�[�^�����������肷��֐� */
    BOOL    (*IsExecuting) (struct WXCProtocolContext *);
    /* �K�v�ɉ����Ċe�v���g�R���Ŏg�p���郏�[�N�G���A�ւ̃|�C���^ */
    void   *impl_work;
    /* �P�������X�g�Ƃ��ĊǗ����邽�߂̃|�C���^ */
    struct WXCProtocolImpl *next;
}
WXCProtocolImpl;

/* �o�^���ꂽ�v���g�R��������K�肷��\���� */
typedef struct WXCProtocolRegistry
{
    /* �����I�ɒP�������X�g�Ǘ����邽�߂̃|�C���^ */
    struct WXCProtocolRegistry *next;
    /* �v���g�R����� */
    u32     ggid;
    WXCCallback callback;
    WXCProtocolImpl *impl;
    /* ������������M�p�f�[�^ */
    WXCBlockDataFormat send;           /* ���M�o�b�t�@ */
    WXCBlockDataFormat recv;           /* ��M�o�b�t�@ */
}
WXCProtocolRegistry;


/* WXC���C�u�����̒ʐM�v���g�R�������� */
typedef struct WXCProtocolContext
{
    WXCBlockDataFormat send;           /* ���M�o�b�t�@ */
    WXCBlockDataFormat recv;           /* ��M�o�b�t�@ */
    /*
     * �����p����M�f�[�^���.
     * �����͍ŏI�I�ɔz��łȂ����X�g�ɂ���.
     */
    WXCProtocolRegistry *current_block;
    WXCProtocolRegistry data_array[WXC_REGISTER_DATA_MAX];
    u32     target_ggid;
}
WXCProtocolContext;


/*****************************************************************************/
/* function */

#ifdef  __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*
  Name:         WXC_InitProtocol

  Description:  WXC���C�u�����v���g�R����������

  Arguments:    protocol      WXCProtocolContext �\����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_InitProtocol(WXCProtocolContext * protocol);

/*---------------------------------------------------------------------------*
  Name:         WXC_InstallProtocolImpl

  Description:  �I���\�ȐV�����v���g�R����ǉ�

  Arguments:    impl          WXCProtocolImpl�\���̂ւ̃|�C���^.
                              ���̍\���̂̓��C�u�����I�����܂œ����Ŏg�p�����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_InstallProtocolImpl(WXCProtocolImpl * impl);

/*---------------------------------------------------------------------------*
  Name:         WXC_FindProtocolImpl

  Description:  �w��̖��O�����v���g�R��������

  Arguments:    name          �v���g�R����.

  Returns:      �w��̖��O�����v���g�R�������݂���΂��̃|�C���^.
 *---------------------------------------------------------------------------*/
WXCProtocolImpl *WXC_FindProtocolImpl(const char *name);

/*---------------------------------------------------------------------------*
  Name:         WXC_ResetSequence

  Description:  WXC���C�u�����v���g�R�����ď�����

  Arguments:    protocol      WXCProtocolContext �\����.
                send_max      ���M�p�P�b�g�ő�T�C�Y.
                recv_max      ��M�p�P�b�g�ő�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_ResetSequence(WXCProtocolContext * protocol, u16 send_max, u16 recv_max);

/*---------------------------------------------------------------------------*
  Name:         WXC_AddBlockSequence

  Description:  �u���b�N�f�[�^������ݒ�

  Arguments:    protocol      WXCProtocolContext �\����.
                send_buf      ���M�o�b�t�@.
                send_size     ���M�o�b�t�@�T�C�Y.
                recv_buf      ��M�o�b�t�@.
                recv_max      ��M�o�b�t�@�T�C�Y.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_AddBlockSequence(WXCProtocolContext * protocol,
                             const void *send_buf, u32 send_size, void *recv_buf, u32 recv_max);

/*---------------------------------------------------------------------------*
  Name:         WXC_GetCurrentBlock

  Description:  ���ݑI������Ă���u���b�N�f�[�^���擾.

  Arguments:    protocol      WXCProtocolContext �\����.

  Returns:      ���ݑI������Ă���u���b�N�f�[�^.
 *---------------------------------------------------------------------------*/
static inline WXCProtocolRegistry *WXC_GetCurrentBlock(WXCProtocolContext * protocol)
{
    return protocol->current_block;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_GetCurrentBlockImpl

  Description:  ���ݑI������Ă���u���b�N�f�[�^�̃v���g�R���C���^�t�F�[�X���擾.

  Arguments:    protocol      WXCProtocolContext �\����.

  Returns:      ���ݑI������Ă���u���b�N�f�[�^.
 *---------------------------------------------------------------------------*/
static inline WXCProtocolImpl *WXC_GetCurrentBlockImpl(WXCProtocolContext * protocol)
{
    return protocol->current_block->impl;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_SetCurrentBlock

  Description:  �w��̃u���b�N�f�[�^��I������.

  Arguments:    protocol      WXCProtocolContext �\����.
                target        �I������f�[�^�u���b�N.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static inline void WXC_SetCurrentBlock(WXCProtocolContext * protocol, WXCProtocolRegistry * target)
{
    protocol->current_block = target;
}

/*---------------------------------------------------------------------------*
  Name:         WXC_FindNextBlock

  Description:  GGID ���w�肵�ău���b�N�f�[�^������.

  Arguments:    protocol      WXCProtocolContext �\����.
                from          �����J�n�ʒu.
                              NULL �Ȃ�擪���猟����, �����łȂ����
                              �w�肳�ꂽ�u���b�N�̎����猟������.
                ggid          ��������u���b�N�f�[�^�Ɋ֘A�t����ꂽ GGID.
                match         ��������.
                              TRUE �Ȃ� GGID �̍��v������̂������ΏۂƂ�,
                              FALSE �Ȃ� ���v���Ȃ����̂�ΏۂƂ���.

  Returns:      ���v����u���b�N�f�[�^�����݂���΂��̃|�C���^��,
                ������� NULL ��Ԃ�.
 *---------------------------------------------------------------------------*/
WXCProtocolRegistry *WXC_FindNextBlock(WXCProtocolContext * protocol,
                                       const WXCProtocolRegistry * from, u32 ggid, BOOL match);

/*---------------------------------------------------------------------------*
  Name:         WXC_BeaconSendHook

  Description:  �r�[�R���X�V�^�C�~���O�ŌĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_param       ����̃r�[�R���Ɏg�p���� WMParentParam �\����.
                              ���̊֐��̓����ŕK�v�ɉ����ĕύX����.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_BeaconSendHook(WXCProtocolContext * protocol, WMParentParam *p_param);

/*---------------------------------------------------------------------------*
  Name:         WXC_BeaconRecvHook

  Description:  �X�L�������ꂽ�e�r�[�R���ɑ΂��ČĂяo�����t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_desc        �X�L�������ꂽ WMBssDesc �\����

  Returns:      �ڑ��ΏۂƂ݂Ȃ��� TRUE ��, �����łȂ���� FALSE ��Ԃ�.
 *---------------------------------------------------------------------------*/
BOOL    WXC_BeaconRecvHook(WXCProtocolContext * protocol, const WMBssDesc *p_desc);

/*---------------------------------------------------------------------------*
  Name:         WXC_PacketSendHook

  Description:  MP �p�P�b�g���M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ���M�p�P�b�g����ݒ肷�� WXCPacketInfo �|�C���^.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_PacketSendHook(WXCProtocolContext * protocol, WXCPacketInfo * packet);

/*---------------------------------------------------------------------------*
  Name:         WXC_PacketRecvHook

  Description:  MP �p�P�b�g��M�̃^�C�~���O�ŌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                packet        ��M�p�P�b�g�����Q�Ƃ��� WXCPacketInfo �|�C���^.

  Returns:      �v���g�R���I�ɑ��肪�ؒf������ TRUE.
 *---------------------------------------------------------------------------*/
BOOL    WXC_PacketRecvHook(WXCProtocolContext * protocol, const WXCPacketInfo * packet);

/*---------------------------------------------------------------------------*
  Name:         WXC_ConnectHook

  Description:  �ʐM�Ώۂ̐ڑ����m���ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                bitmap        �ڑ����ꂽ�ʐM��� AID �r�b�g�}�b�v.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_ConnectHook(WXCProtocolContext * protocol, u16 bitmap);

/*---------------------------------------------------------------------------*
  Name:         WXC_DisconnectHook

  Description:  �ʐM�Ώۂ̐ؒf���m���ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                bitmap        �ؒf���ꂽ�ʐM��� AID �r�b�g�}�b�v.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_DisconnectHook(WXCProtocolContext * protocol, u16 bitmap);

/*---------------------------------------------------------------------------*
  Name:         WXC_CallPreConnectHook

  Description:  �ʐM�Ώۂւ̐ڑ��O�ɌĂяo���t�b�N.

  Arguments:    protocol      WXCProtocolContext �\����.
                p_desc        �ڑ��Ώۂ� WMBssDesc �\����
                ssid          �ݒ肷�邽�߂�SSID���i�[����o�b�t�@.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_CallPreConnectHook(WXCProtocolContext * protocol, WMBssDesc *p_desc, u8 *ssid);

/*---------------------------------------------------------------------------*
  Name:         WXC_InitProtocolRegistry

  Description:  �w��̓o�^�\���̂� GGID �ƃR�[���o�b�N���֘A�t����

  Arguments:    p_data        �o�^�Ɏg�p���� WXCProtocolRegistry �\����
                ggid          �ݒ肷�� GGID
                callback      ���[�U�[�ւ̃R�[���o�b�N�֐�
                              (NULL �̏ꍇ�͐ݒ����)
                impl          �̗p����ʐM�v���g�R��

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_InitProtocolRegistry(WXCProtocolRegistry * p_data, u32 ggid, WXCCallback callback,
                                 WXCProtocolImpl * impl);

/*---------------------------------------------------------------------------*
  Name:         WXC_SetInitialExchangeBuffers

  Description:  ����̃f�[�^�����Ŗ��񎩓��I�Ɏg�p����f�[�^��ݒ�

  Arguments:    p_data        �o�^�Ɏg�p���� WXCProtocolRegistry �\����
                send_ptr      �o�^�f�[�^�̃|�C���^
                send_size     �o�^�f�[�^�̃T�C�Y
                recv_ptr      ��M�o�b�t�@�̃|�C���^
                recv_size     ��M�o�b�t�@�̃T�C�Y

  Returns:      None.
 *---------------------------------------------------------------------------*/
void    WXC_SetInitialExchangeBuffers(WXCProtocolRegistry * p_data, u8 *send_ptr, u32 send_size,
                                      u8 *recv_ptr, u32 recv_size);

WXCProtocolImpl* WXCi_GetProtocolImplCommon(void);
WXCProtocolImpl* WXCi_GetProtocolImplWPB(void);
WXCProtocolImpl* WXCi_GetProtocolImplWXC(void);


#ifdef  __cplusplus
}       /* extern "C" */
#endif


#endif /* NITRO_WXC_PROTOCOL_H_ */

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
