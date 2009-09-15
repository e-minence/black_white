/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_wm_base.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-17#$
  $Rev: 8556 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef MB_WM_BASE_H_
#define MB_WM_BASE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro.h>
#include <nitro/wm.h>

/* �ڑ��ő吔 */
#define MB_NUM_PARENT_INFORMATIONS              16

#define MB_MAX_SEND_BUFFER_SIZE         (0x400)

/*
 * �}���`�u�[�g�e / �q�̑I��.
 * (�q�� IPL �łȂ��Ǝ��s�ł��Ȃ�)
 */
#define MB_MODE_PARENT                          1
#define MB_MODE_CHILD                           2

/*
 * wm_lib �ŗL��`
 */

#define MB_CALLBACK_CHILD_CONNECTED             0
#define MB_CALLBACK_CHILD_DISCONNECTED          1
#define MB_CALLBACK_MP_PARENT_SENT              2
#define MB_CALLBACK_MP_PARENT_RECV              3
#define MB_CALLBACK_PARENT_FOUND                4
#define MB_CALLBACK_PARENT_NOT_FOUND            5
#define MB_CALLBACK_CONNECTED_TO_PARENT         6
#define MB_CALLBACK_DISCONNECTED                7
#define MB_CALLBACK_MP_CHILD_SENT               8
#define MB_CALLBACK_MP_CHILD_RECV               9
#define MB_CALLBACK_DISCONNECTED_FROM_PARENT    10
#define MB_CALLBACK_CONNECT_FAILED              11
#define MB_CALLBACK_DCF_CHILD_SENT              12
#define MB_CALLBACK_DCF_CHILD_SENT_ERR          13
#define MB_CALLBACK_DCF_CHILD_RECV              14
#define MB_CALLBACK_DISCONNECT_COMPLETE         15
#define MB_CALLBACK_DISCONNECT_FAILED           16
#define MB_CALLBACK_END_COMPLETE                17
#define MB_CALLBACK_MP_CHILD_SENT_ERR           18
#define MB_CALLBACK_MP_PARENT_SENT_ERR          19
#define MB_CALLBACK_MP_STARTED                  20
#define MB_CALLBACK_INIT_COMPLETE               21
#define MB_CALLBACK_END_MP_COMPLETE             22
#define MB_CALLBACK_SET_GAMEINFO_COMPLETE       23
#define MB_CALLBACK_SET_GAMEINFO_FAILED         24
#define MB_CALLBACK_MP_SEND_ENABLE              25
#define MB_CALLBACK_PARENT_STARTED              26
#define MB_CALLBACK_BEACON_LOST                 27
#define MB_CALLBACK_BEACON_SENT                 28
#define MB_CALLBACK_BEACON_RECV                 29
#define MB_CALLBACK_MP_SEND_DISABLE             30
#define MB_CALLBACK_DISASSOCIATE                31
#define MB_CALLBACK_REASSOCIATE                 32
#define MB_CALLBACK_AUTHENTICATE                33
#define MB_CALLBACK_SET_LIFETIME                34
#define MB_CALLBACK_DCF_STARTED                 35
#define MB_CALLBACK_DCF_SENT                    36
#define MB_CALLBACK_DCF_SENT_ERR                37
#define MB_CALLBACK_DCF_RECV                    38
#define MB_CALLBACK_DCF_END                     39
#define MB_CALLBACK_MPACK_IND                   40
#define MB_CALLBACK_MP_CHILD_SENT_TIMEOUT       41
#define MB_CALLBACK_SEND_QUEUE_FULL_ERR         42

#define MB_CALLBACK_API_ERROR                   255     // API�R�[���̖߂�l�ŃG���[
#define MB_CALLBACK_ERROR                       256


/*
 * �r�[�R���Ŏ�M�����X�̐e�@���. (�q�@�������ŊǗ�)
 */
typedef struct ParentInfo
{
    union
    {
        WMBssDesc parentInfo[1];
        u8      parentInfo_area[WM_BSS_DESC_SIZE] ATTRIBUTE_ALIGN(32);
    };
    /* ���� mac �� GameInfo �ȊO�S���g���Ă��Ȃ� */
    WMStartScanCallback scan_data;
    u8      reserved1[8];
}
ParentInfo;


/* MB �̃R�[���o�b�N�Ɏg�p����֐��̌`�� */
typedef void (*MBCallbackFunc) (u16 type, void *arg);


/*
 * wm_lib �p�����[�^�\����.
 * wm_lib - wm_tool �̋��n���������ꎞ�I�ɋ����A�������̂�,
 * union �̎g�����Ɋւ��� ANSI-STRICT �������Ă��Ȃ�����������.
 * (���ƂŒu������Ηǂ������̘b�Ȃ̂Ō��)
 */
typedef struct
{
    /* �e�@���ݒ�(�e�@���g�p) */
    union
    {
        WMParentParam parentParam;
        u8      parentParam_area[WM_PARENT_PARAM_SIZE] ATTRIBUTE_ALIGN(32);
    };

    /* StartMP �ɓn�� WM �����o�b�t�@. (SetMP �Ŏg���Ă͂Ȃ�Ȃ�) */
    u16     sendBuf[MB_MAX_SEND_BUFFER_SIZE / sizeof(u16)] ATTRIBUTE_ALIGN(32);

    /* �e�@���擾�o�b�t�@(�q�@���g�p) */
    union
    {
        WMBssDesc parentInfoBuf;
        u8      parentInfoBuf_area[WM_BSS_DESC_SIZE] ATTRIBUTE_ALIGN(32);
    };

    u16     p_sendlen;
    u16     p_recvlen;

    WMMpRecvBuf *recvBuf;              /* ��M�o�b�t�@ */

    /* �e�@ / �q�@ �R�[���o�b�N */
    void    (*callback_ptr) (void *arg);

    u8      mpBusy;                    /* MP���M���t���O */
    u8      mbIsStarted;
    u8      reserved0[10];

    u16     sendBufSize;               // ���M�o�b�t�@�̃T�C�Y
    u16     recvBufSize;               // MP��M�o�b�t�@�̃T�C�Y

    MBCallbackFunc callback;           // WM_lib�p�R�[���o�b�N
    const WMBssDesc *pInfo;            // �ڑ���e�@���|�C���^(�q�@�Ŏg�p)
    u16     mode;                      // MB_MODE_***
    u16     endReq;

    u16     mpStarted;                 // MP�J�n�ς݃t���O
    u16     child_bitmap;              // �q�@�ڑ���

    /* �A�����M�֘A */
    u16     contSend;                  // �A�����M���s�t���O(1:�A�����M, 0:1�t���[�����̒ʐM)
    u8      reserved1[2];

    // gameinfo�֘A
    u8      uname[WM_SIZE_USERNAME] ATTRIBUTE_ALIGN(4);
    u8      gname[WM_SIZE_GAMENAME] ATTRIBUTE_ALIGN(4);
    u16     currentTgid;               // �ڑ���e�@��TGID(BeaconRecv.Ind�Ń`�F�b�N����)
    u8      reserved2[22];

    u16     userGameInfo[((WM_SIZE_USER_GAMEINFO + 32) & ~(32 - 1)) /
                         sizeof(u16)] ATTRIBUTE_ALIGN(32);

    /* �q�@�ŗL�̃f�[�^ */
    struct
    {
        /*
         * �����ςݐe�@��.
         * �����l 0, StartScan ������ ++, StartConnect ���s�� 0.
         * ����͌���N�����Ă��Ȃ���,
         * BeconRecvState �ɂł�����Ă�����΃��[�U�ɂƂ��ĕ֗��ł��傤.
         */
        u16     found_parent_count;
        /*
         * ���g�Ɋ��蓖�Ă�ꂽAID;
         * �����l 0, StartConnect ������ n.
         * ����͌���N�����Ă��Ȃ�.
         */
        u16     my_aid;

        BOOL    scanning_flag;
        BOOL    scan_channel_flag;
        int     last_found_parent_no;

        u8      reserved8[16];

        /* �e�@ 16 �䕪�̏��z�� */
        ParentInfo parent_info[MB_NUM_PARENT_INFORMATIONS];
    };

}
MBiParam;


// ===============================================================================
// function

/* �Ō�Ɍ��������e���擾 */
int     MBi_GetLastFountParent(void);

/* �e�� BSS ���擾 */
WMBssDesc *MBi_GetParentBssDesc(int parent);

/* �ő�X�L�������Ԃ�ݒ� */
void    MBi_SetMaxScanTime(u16 time);

int     MBi_SendMP(const void *buf, int len, int pollbmp);

int     MBi_GetSendBufSize(void);

int     MBi_GetRecvBufSize(void);

int     MBi_CommConnectToParent(const WMBssDesc *bssDescp);

u32     MBi_GetGgid(void);

u16     MBi_GetTgid(void);

u8      MBi_GetAttribute(void);

int     MBi_RestartScan(void);

int     MBi_GetScanChannel(void);

u16     MBi_GetAid(void);

BOOL    MBi_IsStarted(void);

int     MBi_CommEnd(void);

void    MBi_CommParentCallback(u16 type, void *arg);
void    MBi_CommChildCallback(u16 type, void *arg);

#ifdef __cplusplus
}
#endif

#endif /*  MB_WM_BASE_H_    */
