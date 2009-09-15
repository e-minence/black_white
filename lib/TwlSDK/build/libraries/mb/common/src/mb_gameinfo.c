/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - libraries
  File:     mb_gameinfo.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-04#$
  $Rev: 10698 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#include <nitro.h>
#include "mb_gameinfo.h"
#include "mb_common.h"
#include "mb_private.h"                // for MB_SCAN_TIME_NORMAL & MB_SCAN_TIME_LOKING & MB_OUTPUT, MB_DEBUG_OUTPUT


/*

�@���������������ӎ�����������������
�@�E�X�L�������b�N���́A�X�L�������Ԃ�e�@�̃r�[�R���Ԋu�ȏ�ɂ������������Ƃ̂��ƁB�i200ms�ȏ�j
�@�@�A���A���������ꍇ�A���I�ɃX�L�������Ԃ�ς��邱�ƂɂȂ�̂ŁALifetimeCount��LockTimeCount�̍ő�l
    ����ɃJ�E���g����Ă���l���ǂ����邩�H

*/


/*
 * �g�p���Ă��� wmDEMOlib �֐�:
 *  �E����
 *
 * �g�p���Ă��� WM API:
 *  �EWM_SetGameInfo (MBi_SendFixedBeacon, MBi_SendVolatBeacon)
 *
 *
 *
 *
 */

// define data---------------------------------------------

#define MB_GAME_INFO_LIFETIME_SEC       60      // ��M�����Q�[�����̎����i����sec�l�̊ԃr�[�R����M���Ȃ���΍폜�����j
#define MB_SCAN_LOCK_SEC                8       // ����e�@�����b�N���ăX�L��������ő厞��
#define MB_SAME_BEACON_RECV_MAX_COUNT   3       // �e�@�������r�[�R�����o�������Ă����ꍇ�A���̐e�@�̃r�[�R������M��������ő�J�E���g���B

#define MB_BEACON_DATA_SIZE         (WM_SIZE_USER_GAMEINFO - 8)
#define MB_BEACON_FIXED_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 6) // �r�[�R�����̐e�@�Q�[�����Œ�̈�̃f�[�^�T�C�Y
#define MB_BEACON_VOLAT_DATA_SIZE   (MB_BEACON_DATA_SIZE   - 8) // �r�[�R�����̐e�@�Q�[����񗬓��̈�̃f�[�^�T�C�Y
#define MB_SEND_MEMBER_MAX_NUM      (MB_BEACON_VOLAT_DATA_SIZE / sizeof(MBUserInfo))    // �P��̃r�[�R���ő��M�ł��郁���o�[���̍ő吔

#define FIXED_NORMAL_SIZE            sizeof(MBGameInfoFixed)    // �Q�[�����Œ�̈�̃m�[�}���T�C�Y
#define FIXED_NO_ICON_SIZE          (sizeof(MBGameInfoFixed) - sizeof(MBIconInfo))      // �Q�[�����Œ�̈�̃A�C�R���Ȃ��T�C�Y
#define FIXED_FRAGMENT_MAX(size)    ( ( size / MB_BEACON_FIXED_DATA_SIZE) + ( size % MB_BEACON_FIXED_DATA_SIZE ? 1 : 0 ) )
                                                                                    // �Q�[�����Œ�̈���r�[�R��������

#define MB_LIFETIME_MAX_COUNT       ( MB_GAME_INFO_LIFETIME_SEC * 1000 / MB_SCAN_TIME_NORMAL + 1 )
#define MB_LOCKTIME_MAX_COUNT       ( MB_SCAN_LOCK_SEC          * 1000 / MB_SCAN_TIME_NORMAL + 1 )
                                                                                    // �e�@�Q�[����񐶑����Ԃ�ScanTime�P�ʂɊ��Z��������
                                                                                    // �X�L�������b�N���ԁ@�@��ScanTime�P�ʂɊ��Z��������
#define MB_SCAN_COUNT_UNIT_NORMAL   ( 1 )
#define MB_SCAN_COUNT_UNIT_LOCKING  ( MB_SCAN_TIME_LOCKING / MB_SCAN_TIME_NORMAL )



typedef enum MbBeaconState
{
    MB_BEACON_STATE_STOP = 0,
    MB_BEACON_STATE_READY,
    MB_BEACON_STATE_FIXED_START,
    MB_BEACON_STATE_FIXED,
    MB_BEACON_STATE_VOLAT_START,
    MB_BEACON_STATE_VOLAT,
    MB_BEACON_STATE_NEXT_GAME
}
MbBeaconState;


/* �r�[�R���t�H�[�}�b�g�\���� */
typedef struct MbBeacon
{
    u32     ggid;                      // GGID
    u8      dataAttr:2;                // �f�[�^�����iMbBeaconDataAttr�Ŏw��j
    u8      fileNo:6;                  // �t�@�C��No.
    u8      seqNoFixed;                // Fixed�f�[�^�V�[�P���X�ԍ��i�f�[�^���e�ɍX�V���������ꍇ�ɉ��Z�����B�j
    u8      seqNoVolat;                // Volat�f�[�^�V�[�P���X�ԍ��i�@�V�@�j
    u8      beaconNo;                  // �r�[�R���i���o�[�i�r�[�R�����M���ƂɃC���N�������g�j
    /* �� MbBeaconData */
    union
    {
        struct
        {                              // MBGameInfoFixed���M��
            u16     sum;               // 16bit checksum
            u8      fragmentNo;        // �f�Љ��f�[�^�̌��ݔԍ�
            u8      fragmentMaxNum;    // �f�Љ��f�[�^�̍ő吔
            u8      size;              // �f�[�^�T�C�Y
            u8      rsv;
            u8      data[MB_BEACON_FIXED_DATA_SIZE];    // �f�[�^����
        }
        fixed;
        struct
        {                              // MBGameInfoVolatile���M��
            u16     sum;               // 16bit checksum
            u8      nowPlayerNum;      // ���݂̃v���C���[��
            u8      pad[1];
            u16     nowPlayerFlag;     // ���݂̑S�v���C���[�̃v���C���[�ԍ����r�b�g�Ŏ����B
            u16     changePlayerFlag;  // ����̍X�V�ŕύX���ꂽ�v���C���[���̔ԍ����r�b�g�Ŏ����B�i�V�[�P���X�ԍ����ς�����u�Ԃɂ̂ݔ���Ɏg�p�j
            MBUserInfo member[MB_SEND_MEMBER_MAX_NUM];  // �e�q�@�̃��[�U�[����MB_SEND_MEMBER_MAX_NUM�����M�iPlayerNo == 15�i�e�@�j �Ȃ�I�[�B�j
            u8      userVolatData[MB_USER_VOLAT_DATA_SIZE];     // ���[�U���Z�b�g�ł���f�[�^
        }
        volat;
    }
    data;

}
MbBeacon;

/* �e�@���r�[�R�����M�X�e�[�^�X�\���� */
typedef struct MbBeaconSendStatus
{
    MBGameInfo *gameInfoListTop;       // �Q�[����񃊃X�g�擪�ւ̃|�C���^�i�Е������X�g�j
    MBGameInfo *nowGameInfop;          // ���ݑ��M���̃Q�[�����ւ̃|�C���^
    u8     *srcp;                      // ���݂̑��M���̃Q�[�����\�[�X�ւ̃|�C���^
    u8      state;                     // �r�[�R�����M�X�e�[�g�i�Œ�f�[�^���A�����f�[�^�����M�B�e�f�[�^��S���M������X�e�[�g�ύX�B�j
    u8      seqNoFixed;                // �Œ�̈�̃V�[�P���X�ԍ�
    u8      seqNoVolat;                // �����̈�̃V�[�P���X�ԍ�
    u8      fragmentNo;                // �@�@�V�@�@�@�@�@�@�̃t���O�����g�ԍ��i�Œ�̈�̏ꍇ�j
    u8      fragmentMaxNum;            // �@�@�V�@�@�@�@�@�@�̃t���O�����g���@�i�V�j
    u8      beaconNo;
    u8      pad[2];
}
MbBeaconSendStatus;



// function's prototype------------------------------------
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag);
static void MBi_ClearSendStatus(void);
static BOOL MBi_ReadyBeaconSendStatus(void);
static void MBi_InitSendFixedBeacon(void);
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute);
static void MBi_InitSendVolatBeacon(void);
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute);

static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid);
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp);
static void MBi_CheckCompleteGameInfoFragments(int index, MBBeaconMsgCallback Callbackp);
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel);
static void MBi_CheckTGID(WMBssDesc *bssDescp, int inex);
static void MBi_CheckSeqNoFixed(int index);
static void MBi_CheckSeqNoVolat(int index);
static void MBi_InvalidateGameInfoBssID(u8 *bssidp);
static void MBi_RecvFixedBeacon(int index);
static void MBi_RecvVolatBeacon(int index);

static void MBi_LockScanTarget(int index);
static void MBi_UnlockScanTarget(void);
static int mystrlen(u16 *str);


// const data----------------------------------------------

// global variables----------------------------------------

static MbBeaconSendStatus mbss;        // �r�[�R�����M�X�e�[�^�X

static MbBeaconRecvStatus mbrs;        // �r�[�R����M�X�e�[�^�X
static MbBeaconRecvStatus *mbrsp = NULL;        // �r�[�R����M�X�e�[�^�X

// static variables----------------------------------------
static MbScanLockFunc sLockFunc = NULL; // �X�L�������b�N�ݒ�֐��|�C���^
static MbScanUnlockFunc sUnlockFunc = NULL;     // �X�L�������b�N�����p�֐��|�C���^

static MbBeacon bsendBuff ATTRIBUTE_ALIGN(32);  // �r�[�R�����M�o�b�t�@
static MbBeacon *brecvBuffp;           // �r�[�R����M�o�b�t�@
static WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);        // BssDesc�̃e���|�����o�b�t�@

static MBSendVolatCallbackFunc sSendVolatCallback = NULL;       // ���[�U�f�[�^���M�R�[���o�b�N
static u32 sSendVolatCallbackTiming;  // ���M�R�[���o�b�N�����^�C�~���O

// function's description-----------------------------------------------


//=========================================================
// �r�[�R����M���[�N�o�b�t�@����
//=========================================================
// �ݒ肳��Ă��郏�[�N�o�b�t�@���擾
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void)
{
    return mbrsp;
}

// static�ϐ����烏�[�N�o�b�t�@��ݒ�(���݊��p)
void MBi_SetBeaconRecvStatusBufferDefault(void)
{
    mbrsp = &mbrs;
}

// �r�[�R����M���[�N�o�b�t�@�̐ݒ�
void MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf)
{
    mbrsp = buf;
}


// �X�L�������b�N�p�֐���ݒ�
void MBi_SetScanLockFunc(MbScanLockFunc lock, MbScanUnlockFunc unlock)
{
    sLockFunc = lock;
    sUnlockFunc = unlock;
}



//=========================================================
// �e�@�ɂ��Q�[�����̃r�[�R�����M
//=========================================================

// �r�[�R���ő��M����e�@�Q�[�����̐���
void MBi_MakeGameInfo(MBGameInfo *gameInfop,
                      const MBGameRegistry *mbGameRegp, const MBUserInfo *parent)
{
    BOOL    icon_disable;

    // �Ƃ肠�����S�N���A
    MI_CpuClear16(gameInfop, sizeof(MBGameInfo));

    // �A�C�R���f�[�^�̓o�^
    gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NORMAL;
    icon_disable = !MBi_ReadIconInfo(mbGameRegp->iconCharPathp, &gameInfop->fixed.icon, TRUE);
    icon_disable |= !MBi_ReadIconInfo(mbGameRegp->iconPalettePathp, &gameInfop->fixed.icon, FALSE);

    if (icon_disable)
    {
        gameInfop->dataAttr = MB_BEACON_DATA_ATTR_FIXED_NO_ICON;
        MI_CpuClearFast(&gameInfop->fixed.icon, sizeof(MBIconInfo));
    }

    // GGID
    gameInfop->ggid = mbGameRegp->ggid;

    // �e�̃��[�U�[���o�^
    if (parent != NULL)
    {
        MI_CpuCopy16(parent, &gameInfop->fixed.parent, sizeof(MBUserInfo));
    }

    // �ő�v���C�l���o�^
    gameInfop->fixed.maxPlayerNum = mbGameRegp->maxPlayerNum;

    // �Q�[�������Q�[�����e�����o�^
    {
        int     len;

#define COPY_GAME_INFO_STRING   MI_CpuCopy16

        len = mystrlen(mbGameRegp->gameNamep) << 1;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameNamep, gameInfop->fixed.gameName, (u16)len);
        // �Q�[�����e�����̌�Ƀf�[�^�𖄂ߍ��߂�悤�ɁANUL�����ȍ~�̃f�[�^�����M����B
        len = MB_GAME_INTRO_LENGTH * 2;
        COPY_GAME_INFO_STRING((u8 *)mbGameRegp->gameIntroductionp,
                              gameInfop->fixed.gameIntroduction, (u16)len);
    }

    // �e�@���̃v���C���[����o�^
    gameInfop->volat.nowPlayerNum = 1;
    gameInfop->volat.nowPlayerFlag = 0x0001;    // �e�@�̃v���C���[�ԍ���0
    gameInfop->broadcastedPlayerFlag = 0x0001;

    // ���}���`�u�[�g�J�n���ɂ̓v���C�����o�[�͂��Ȃ��̂ŁA�o�^�͂Ȃ��B
}


// �A�C�R���f�[�^�̃��[�h
static BOOL MBi_ReadIconInfo(const char *filePathp, MBIconInfo *iconp, BOOL char_flag)
{
    FSFile  file;
    s32     size = char_flag ? MB_ICON_DATA_SIZE : MB_ICON_PALETTE_SIZE;
    u16    *dstp = char_flag ? iconp->data : iconp->palette;

    if (filePathp == NULL)
    {                                  // �t�@�C���w�肪�Ȃ����FALSE���^�[��
        return FALSE;
    }

    FS_InitFile(&file);

    if (FS_OpenFileEx(&file, filePathp, FS_FILEMODE_R) == FALSE)
    {                                  // �t�@�C�����J���Ȃ�������FALSE���^�[��
        MB_DEBUG_OUTPUT("\t%s : file open error.\n", filePathp);
        return FALSE;
    }
    else if ((u32)size != FS_GetFileLength(&file))
    {
        MB_DEBUG_OUTPUT("\t%s : different file size.\n", filePathp);
        (void)FS_CloseFile(&file);
        return FALSE;
    }

    (void)FS_ReadFile(&file, dstp, size);
    (void)FS_CloseFile(&file);
    return TRUE;
}


// �Q�[�����̗����������X�V����B
void MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                             const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag)
{
    int     i;
    u8      playerNum = 1;

    MI_CpuCopy16(member, &gameInfop->volat.member[0], sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
    /* �q�@�����J�E���g */
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if (nowPlayerFlag & (0x0002 << i))
        {
            playerNum++;
        }
    }
    gameInfop->volat.nowPlayerNum = playerNum;
    gameInfop->volat.nowPlayerFlag = (u16)(nowPlayerFlag | 0x0001);
    gameInfop->volat.changePlayerFlag = changePlayerFlag;
    gameInfop->seqNoVolat++;
}


// �����񒷂̎Z�o
static int mystrlen(u16 *str)
{
    int     len = 0;
    while (*str++)
        len++;
    return len;
}


// �Q�[�����𑗐M���X�g�ɒǉ�
void MB_AddGameInfo(MBGameInfo *newGameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;

    if (!gInfop)
    {                                  // mbss�̐擪��NULL�Ȃ�擪�ɓo�^�B
        mbss.gameInfoListTop = newGameInfop;
    }
    else
    {                                  // �����Ȃ��΁A���X�g��H���čs���A�Ō�ɒǉ��B
        while (gInfop->nextp != NULL)
        {
            gInfop = gInfop->nextp;
        }
        gInfop->nextp = newGameInfop;
    }
    newGameInfop->nextp = NULL;        // �ǉ������Q�[������next���I�[�Ƃ���B
}


// �Q�[�����𑗐M���X�g����폜
BOOL MB_DeleteGameInfo(MBGameInfo *gameInfop)
{
    MBGameInfo *gInfop = mbss.gameInfoListTop;
    MBGameInfo *before;

    while (gInfop != NULL)
    {
        if (gInfop != gameInfop)
        {
            before = gInfop;
            gInfop = gInfop->nextp;
            continue;
        }

        // ��v�����̂Ń��X�g����폜
        if ((u32)gInfop == (u32)mbss.gameInfoListTop)
        {                              // ���X�g�擪���폜����ꍇ
            mbss.gameInfoListTop = mbss.gameInfoListTop->nextp;
        }
        else
        {
            before->nextp = gInfop->nextp;
        }

        if ((u32)gameInfop == (u32)mbss.nowGameInfop)   // �폜����Q�[����񂪌��ݑ��M���̏ꍇ�A���ݑ��M����
        {                              // �Q�[�������č\�z���ꂽ���X�g����Đݒ肷��B
            mbss.nowGameInfop = NULL;
            if (!MBi_ReadyBeaconSendStatus())
            {
                mbss.state = MB_BEACON_STATE_READY;     // �Q�[����񂪑S�č폜����Ă����Ȃ�΁A�X�e�[�^�X��READY�ɁB
            }
        }
        return TRUE;
    }

    // �Q�[����񂪂Ȃ��ꍇ
    return FALSE;
}


// �Q�[����񑗐M�ݒ�̏�����
void MB_InitSendGameInfoStatus(void)
{
    mbss.gameInfoListTop = NULL;       // �Q�[����񃊃X�g��S�폜�B
    mbss.nowGameInfop = NULL;          // �����M�Q�[�����폜�B
    mbss.state = MB_BEACON_STATE_READY;
    sSendVolatCallback = NULL;
    MBi_ClearSendStatus();             // ���M�X�e�[�^�X���N���A�B
}


// ���M�X�e�[�^�X�̃N���A
static void MBi_ClearSendStatus(void)
{
    mbss.seqNoFixed = 0;
    mbss.seqNoVolat = 0;
    mbss.fragmentNo = 0;
    mbss.fragmentMaxNum = 0;
    mbss.beaconNo = 0;
}


// �r�[�R�����M
void MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    while (1)
    {
        switch (mbss.state)
        {
        case MB_BEACON_STATE_STOP:
        case MB_BEACON_STATE_READY:
            if (!MBi_ReadyBeaconSendStatus())
            {                          // �Q�[�����̑��M����
                return;
            }
            break;
        case MB_BEACON_STATE_FIXED_START:
            MBi_InitSendFixedBeacon();
            break;
        case MB_BEACON_STATE_FIXED:
            MBi_SendFixedBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_VOLAT_START:
            MBi_InitSendVolatBeacon();
            break;
        case MB_BEACON_STATE_VOLAT:
            MBi_SendVolatBeacon(ggid, tgid, attribute);
            return;
        case MB_BEACON_STATE_NEXT_GAME:
            break;
        }
    }
}


// �r�[�R�����M�X�e�[�^�X�𑗐M�\�ɏ�������B
static BOOL MBi_ReadyBeaconSendStatus(void)
{
    MBGameInfo *nextGameInfop;

    // �Q�[����񂪓o�^����Ă��Ȃ��Ȃ�A�G���[���^�[���B
    if (!mbss.gameInfoListTop)
    {
        /* MB �t���O�� ENTRY �t���O�͂����ŗ��Ƃ� */
        (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, MBi_GetGgid(),
                             MBi_GetTgid(), WM_ATTR_FLAG_CS);
        return FALSE;
    }

    // ���ɑ��M����GameInfo�̑I��
    if (!mbss.nowGameInfop || !mbss.nowGameInfop->nextp)
    {
        // �܂����M���̃Q�[����񂪂Ȃ����A���X�g�̍Ō�̏ꍇ�́A���X�g�̐擪�𑗐M����悤�����B
        nextGameInfop = mbss.gameInfoListTop;
    }
    else
    {
        // �����Ȃ��΁A���X�g�̎��̃Q�[���𑗐M����悤�����B
        nextGameInfop = mbss.nowGameInfop->nextp;
    }

    mbss.nowGameInfop = nextGameInfop;

    MBi_ClearSendStatus();
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;

    mbss.state = MB_BEACON_STATE_FIXED_START;

    return TRUE;
}


// �Q�[�����̌Œ�f�[�^���̑��M������
static void MBi_InitSendFixedBeacon(void)
{
    if (mbss.state != MB_BEACON_STATE_FIXED_START)
    {
        return;
    }

    if (mbss.nowGameInfop->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        mbss.fragmentMaxNum = FIXED_FRAGMENT_MAX(FIXED_NORMAL_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed;
    }
    else
    {
        mbss.fragmentMaxNum = FIXED_FRAGMENT_MAX(FIXED_NO_ICON_SIZE);
        mbss.srcp = (u8 *)&mbss.nowGameInfop->fixed.parent;
    }
    mbss.state = MB_BEACON_STATE_FIXED;
}


// �Q�[�����̌Œ�f�[�^���𕪊����ăr�[�R�����M
static void MBi_SendFixedBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    u32     lastAddr = (u32)mbss.nowGameInfop + sizeof(MBGameInfoFixed);

    if ((u32)mbss.srcp + MB_BEACON_FIXED_DATA_SIZE <= lastAddr)
    {
        bsendBuff.data.fixed.size = MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        bsendBuff.data.fixed.size = (u8)(lastAddr - (u32)mbss.srcp);
        MI_CpuClear16((void *)((u8 *)bsendBuff.data.fixed.data + bsendBuff.data.fixed.size),
                      (u32)(MB_BEACON_FIXED_DATA_SIZE - bsendBuff.data.fixed.size));
    }

    // �r�[�R�����M�o�b�t�@�ɃZ�b�g
    MB_DEBUG_OUTPUT("send fragment= %2d  adr = 0x%x\n", mbss.fragmentNo, mbss.srcp);
    MI_CpuCopy16(mbss.srcp, bsendBuff.data.fixed.data, bsendBuff.data.fixed.size);
    bsendBuff.data.fixed.fragmentNo = mbss.fragmentNo;
    bsendBuff.data.fixed.fragmentMaxNum = mbss.fragmentMaxNum;
    bsendBuff.dataAttr = mbss.nowGameInfop->dataAttr;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;
    bsendBuff.data.fixed.sum = 0;
    bsendBuff.data.fixed.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);

    // ���M�X�e�[�^�X�̍X�V
    mbss.fragmentNo++;
    if (mbss.fragmentNo < mbss.fragmentMaxNum)
    {
        mbss.srcp += MB_BEACON_FIXED_DATA_SIZE;
    }
    else
    {
        mbss.state = MB_BEACON_STATE_VOLAT_START;
    }

    /* �e�@�v���O�������g�̂�GGID��o�^���� */
    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // �}���`�u�[�g�t���O�͂����ŗ��Ă�B
}


// �Q�[�����̗����f�[�^���̑��M������
static void MBi_InitSendVolatBeacon(void)
{
    mbss.nowGameInfop->broadcastedPlayerFlag = 0x0001;
    mbss.seqNoVolat = mbss.nowGameInfop->seqNoVolat;
    mbss.state = MB_BEACON_STATE_VOLAT;
}


// �Q�[�����̗����f�[�^���𕪊����ăr�[�R�����M
static void MBi_SendVolatBeacon(u32 ggid, u16 tgid, u8 attribute)
{
    int     i;
    int     setPlayerNum;
    u16     remainPlayerFlag;

    if (mbss.seqNoVolat != mbss.nowGameInfop->seqNoVolat)
    {                                  // ���M���Ƀf�[�^�X�V���������瑗�蒼���B
        MBi_InitSendVolatBeacon();
    }

    // �r�[�R�������Z�b�g
    bsendBuff.dataAttr = MB_BEACON_DATA_ATTR_VOLAT;
    bsendBuff.seqNoFixed = mbss.nowGameInfop->seqNoFixed;
    bsendBuff.seqNoVolat = mbss.seqNoVolat;
    bsendBuff.ggid = mbss.nowGameInfop->ggid;
    bsendBuff.fileNo = mbss.nowGameInfop->fileNo;
    bsendBuff.beaconNo = mbss.beaconNo++;

    // ���݂̃v���C���[���̏�Ԃ��Z�b�g
    bsendBuff.data.volat.nowPlayerNum = mbss.nowGameInfop->volat.nowPlayerNum;
    bsendBuff.data.volat.nowPlayerFlag = mbss.nowGameInfop->volat.nowPlayerFlag;
    bsendBuff.data.volat.changePlayerFlag = mbss.nowGameInfop->volat.changePlayerFlag;

    // �A�v���ݒ�f�[�^���Z�b�g
    if (sSendVolatCallbackTiming == MB_SEND_VOLAT_CALLBACK_TIMING_BEFORE
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        bsendBuff.data.volat.userVolatData[i] = mbss.nowGameInfop->volat.userVolatData[i];
    }

    MB_DEBUG_OUTPUT("send PlayerFlag = %x\n", mbss.nowGameInfop->volat.nowPlayerFlag);

    // ���񑗐M����v���C���[���̃Z�b�g
    MI_CpuClear16(&bsendBuff.data.volat.member[0], sizeof(MBUserInfo) * MB_SEND_MEMBER_MAX_NUM);
    setPlayerNum = 0;
    remainPlayerFlag =
        (u16)(mbss.nowGameInfop->broadcastedPlayerFlag ^ mbss.nowGameInfop->volat.nowPlayerFlag);
    for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
    {
        if ((remainPlayerFlag & (0x0002 << i)) == 0)
        {
            continue;
        }

        MB_DEBUG_OUTPUT("  member %d set.\n", i);

        MI_CpuCopy16(&mbss.nowGameInfop->volat.member[i],
                     &bsendBuff.data.volat.member[setPlayerNum], sizeof(MBUserInfo));
        mbss.nowGameInfop->broadcastedPlayerFlag |= 0x0002 << i;
        if (++setPlayerNum == MB_SEND_MEMBER_MAX_NUM)
        {
            break;
        }
    }
    if (setPlayerNum < MB_SEND_MEMBER_MAX_NUM)
    {                                  // �ő呗�M���ɒB���Ȃ��������ɂ͏I�[����B
        bsendBuff.data.volat.member[setPlayerNum].playerNo = 0;
    }

    // �`�F�b�N�T���̃Z�b�g
    bsendBuff.data.volat.sum = 0;
    bsendBuff.data.volat.sum = MBi_calc_cksum((u16 *)&bsendBuff.data, MB_BEACON_DATA_SIZE);


    // ���M�I���`�F�b�N
    if (mbss.nowGameInfop->broadcastedPlayerFlag == mbss.nowGameInfop->volat.nowPlayerFlag)
    {
        mbss.state = MB_BEACON_STATE_READY;     // �S���𑗐M���I�������A���̃Q�[�����𑗐M���邽�߂̏����X�e�[�g�ցB
    }

    /* �e�@�v���O�������g�̂�GGID��o�^���� */

    (void)WM_SetGameInfo(NULL, (u16 *)&bsendBuff, WM_SIZE_USER_GAMEINFO, ggid, tgid,
                         (u8)(attribute | WM_ATTR_FLAG_MB | WM_ATTR_FLAG_ENTRY));
    // �}���`�u�[�g�t���O�͂����ŗ��Ă�B

    if (sSendVolatCallbackTiming == MB_SEND_VOLAT_CALLBACK_TIMING_AFTER
        && sSendVolatCallback != NULL)
    {
        sSendVolatCallback(mbss.nowGameInfop->ggid);
    }

}


//=========================================================
// �q�@�ɂ��Q�[�����̃r�[�R����M
//=========================================================

// �Q�[������M�X�e�[�^�X�̏�����
void MB_InitRecvGameInfoStatus(void)
{
    MI_CpuClearFast(mbrsp, sizeof(MbBeaconRecvStatus));

    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
}


// �r�[�R����M
BOOL MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp)
{
    int     index;

    // ����擾�����r�[�R�����A�}���`�u�[�g�e�@���ǂ����𔻒�B
    if (!MBi_CheckMBParent(bssDescp))
    {
        return FALSE;
    }

    /* �擾����bssDesc���e���|�����o�b�t�@�փR�s�[ */
    MI_CpuCopy16(bssDescp, &bssDescbuf, sizeof(WMBssDesc));

    /* bssDescp�����[�J���o�b�t�@�ɐ؂�ւ���B */
    bssDescp = &bssDescbuf;

    brecvBuffp = (MbBeacon *) bssDescp->gameInfo.userGameInfo;

    // �`�F�b�N�T���m�F�B
    if (MBi_calc_cksum((u16 *)&brecvBuffp->data, MB_BEACON_DATA_SIZE))
    {
        MB_DEBUG_OUTPUT("Beacon checksum error!\n");
        return FALSE;
    }

    // �e�@�Q�[�����̊i�[�ꏊ�𔻒�i���ɓ���e�@�̃f�[�^����M���Ă���Ȃ�A�����ɑ����Ċi�[����j
    index = MBi_GetStoreElement(bssDescp, Callbackp);
    if (index < 0)
    {
        return FALSE;                  // �i�[�ꏊ���Ȃ��̂ŃG���[���^�[��
    }
    MB_DEBUG_OUTPUT("GameInfo Index:%6d\n", index);

    // �r�[�R�����
    MBi_AnalyzeBeacon(bssDescp, index, linkLevel);

    // �e�@�Q�[�����̑S�Ă̒f�Ђ������������肵�A�R�[���o�b�N�Œʒm
    MBi_CheckCompleteGameInfoFragments(index, Callbackp);

    return TRUE;
}


// ����擾�����r�[�R�����}���`�u�[�g�e�@���ǂ����𔻒肷��B
BOOL MBi_CheckMBParent(WMBssDesc *bssDescp)
{
    // �}���`�u�[�g�e�@���ǂ����𔻒肷��B
    if ((bssDescp->gameInfo.magicNumber != WM_GAMEINFO_MAGIC_NUMBER)
        || !(bssDescp->gameInfo.attribute & WM_ATTR_FLAG_MB))
    {
        MB_DEBUG_OUTPUT("not MB parent : %x%x\n",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return FALSE;
    }
    else
    {

        MB_DEBUG_OUTPUT("MB parent     : %x%x",
                        *(u16 *)(&bssDescp->bssid[4]), *(u32 *)bssDescp->bssid);
        return TRUE;
    }
}


// bssDesc��SSID���Z�b�g����B
static void MBi_SetSSIDToBssDesc(WMBssDesc *bssDescp, u32 ggid)
{
    /* 
       SSID�̐ݒ� 

       �_�E�����[�h�A�v���ŗL��GGID�ƁA�}���`�u�[�g�e�@��TGID����
       SSID�𐶐�����B
       �q�@�́A����SSID��p���Đe�@�A�v���P�[�V�����ƍĐڑ����s���B
     */
    bssDescp->ssidLength = 32;
    ((u16 *)bssDescp->ssid)[0] = (u16)(ggid & 0x0000ffff);
    ((u16 *)bssDescp->ssid)[1] = (u16)((ggid & 0xffff0000) >> 16);
    ((u16 *)bssDescp->ssid)[2] = bssDescp->gameInfo.tgid;
}


// bssDesc�����ƂɎ�M���X�g�v�f�̂ǂ��Ɋi�[���邩���擾����B
static int MBi_GetStoreElement(WMBssDesc *bssDescp, MBBeaconMsgCallback Callbackp)
{
    int     i;

    // ���ɂ��̐e�@�̓���Q�[��������M���Ă��邩�ǂ����𔻒�i "GGID", "BSSID", "fileNo"�̂R����v����Ȃ�A����Q�[�����Ɣ��f����j
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];

        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }
        // GGID����v���邩�H
        if (info->gameInfo.ggid != brecvBuffp->ggid)
        {
            continue;
        }
        // MAC�A�h���X����v���邩�H
        if (!WM_IsBssidEqual(info->bssDesc.bssid, bssDescp->bssid))
        {
            continue;
        }
        // �t�@�C��No.����v���邩�H
        if (mbrsp->list[i].gameInfo.fileNo != brecvBuffp->fileNo)
        {
            continue;
        }

        // =========================================
        // ���̐e�@���ɑ΂��āA���Ɏ�M�ꏊ���m�ۂ���Ă���Ɣ��f�B
        // =========================================
        if (!(mbrsp->validGameInfoFlag & (0x01 << i)))
        {
            MBi_LockScanTarget(i);     // �܂��Y���e�@�̏�񂪑����Ă��Ȃ���΁A�X�L�������b�N����B
        }
        return i;
    }

    // �܂���M���Ă��Ȃ��̂ŁA���X�g��NULL�ʒu��T���āA�������i�[�ꏊ�ɂ���B
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if (mbrsp->usingGameInfoFlag & (0x01 << i))
        {
            continue;
        }

        MI_CpuCopy16(bssDescp, &mbrsp->list[i].bssDesc, sizeof(WMBssDesc));
        // BssDesc���R�s�[
        mbrsp->list[i].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
        mbrsp->usingGameInfoFlag |= (u16)(0x01 << i);

        MB_DEBUG_OUTPUT("\n");
        // ���̐e�@�݂̂�Scan�ΏۂɃ��b�N����B
        MBi_LockScanTarget(i);
        return i;
    }

    // �i�[�ꏊ���S�Ė��܂��Ă����ꍇ�̓R�[���o�b�N�Œʒm���ăG���[���^�[��
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_LIST_FULL, NULL, 0);
    }
    return -1;
}


// �e�@�Q�[�����̑S�Ă̒f�Ђ������������肵�A�R�[���o�b�N�Œʒm
static void MBi_CheckCompleteGameInfoFragments(int index, MBBeaconMsgCallback Callbackp)
{
    MBGameInfoRecvList *info = &mbrsp->list[index];

    /* ������ɂ���A�v���P�[�V�����֐e�@������ʒm���� */
    if (Callbackp != NULL)
    {
        Callbackp(MB_BC_MSG_GINFO_BEACON, info, index);
    }
    if ((info->getFragmentFlag == info->allFragmentFlag) && (info->getFragmentFlag)     // �V�����e�@�Q�[����񂪑������ꍇ
        && (info->gameInfo.volat.nowPlayerFlag)
        && (info->getPlayerFlag == info->gameInfo.volat.nowPlayerFlag))
    {
        if (mbrsp->validGameInfoFlag & (0x01 << index))
        {
            return;
        }
        mbrsp->validGameInfoFlag |= 0x01 << index;
        mbrsp->usefulGameInfoFlag |= 0x01 << index;
        MBi_UnlockScanTarget();        // Scan�Ώۂ̃��b�N����������B
        MB_DEBUG_OUTPUT("validated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_VALIDATED, info, index);
        }
    }
    else
    {                                  // ���Ɏ擾���Ă����e�@�Q�[����񂪍X�V����āA�s���S�ȏ�ԂɂȂ����ꍇ
        if ((mbrsp->validGameInfoFlag & (0x01 << index)) == 0)
        {
            return;
        }
        mbrsp->validGameInfoFlag ^= (0x01 << index);
        MB_DEBUG_OUTPUT("Invaldated ParentInfo = %d\n", index);
        if (Callbackp != NULL)
        {
            Callbackp(MB_BC_MSG_GINFO_INVALIDATED, info, index);
        }
    }
}


// �e�@�̃Q�[�����̎����J�E���g�i���łɃX�L�������b�N���Ԃ̃J�E���g���s���j
void MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent)
{
    int     i;
    BOOL    unlock = FALSE;

    // �Q�[������M���X�g�̎�������@���@�V�K���b�N�Ώۃ^�[�Q�b�g�̗L�����m�F
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        MBGameInfoRecvList *info = &mbrsp->list[i];
        u16     mask = (u16)(0x0001 << i);
        if ((mbrsp->usingGameInfoFlag & mask) == 0)
        {
            continue;
        }
        // ��������
        info->lifetimeCount -= mbrsp->scanCountUnit;
        if (info->lifetimeCount >= 0)
        {
            continue;
        }
        info->lifetimeCount = 0;
        if (mbrsp->validGameInfoFlag & mask)
        {
            if (Callbackp != NULL)
            {
                Callbackp(MB_BC_MSG_GINFO_LOST, info, i);
            }
        }
        if (mbrsp->nowScanTargetFlag & mask)
        {
            unlock = TRUE;
        }
        mbrsp->usingGameInfoFlag &= ~mask;
        MB_DeleteRecvGameInfo(i);      // �R�[���o�b�N�ʒm��ɃQ�[�������폜����B
        MB_DEBUG_OUTPUT("gameInfo %2d : lifetime end.\n", i);
    }

    // �X�L�������b�N���Ԃ̃J�E���g
    if (mbrsp->nowScanTargetFlag && mbrsp->nowLockTimeCount > 0)
    {
        mbrsp->nowLockTimeCount -= mbrsp->scanCountUnit;        // �X�L�������b�N���Ԃ��^�C���A�E�g������A���b�N���������Ď��̃��b�N�Ώۂ�T���B
        if (mbrsp->nowLockTimeCount < 0)
        {
            MB_DEBUG_OUTPUT("scan lock time up!\n");
            unlock = TRUE;
        }
        else if (!found_parent)
        {
            if (++mbrsp->notFoundLockTargetCount > 4)
            {
                MB_DEBUG_OUTPUT("scan lock target not found!\n");
                unlock = TRUE;
            }
        }
        else
        {
            mbrsp->notFoundLockTargetCount = 0;
        }
    }

    // �X�L�����A�����b�N�����B
    if (unlock)
    {
        mbrsp->nowLockTimeCount = 0;
        MBi_UnlockScanTarget();
    }
}


// �r�[�R�����
static void MBi_AnalyzeBeacon(WMBssDesc *bssDescp, int index, u16 linkLevel)
{
    MBi_CheckTGID(bssDescp, index);    // TGID�̃`�F�b�N
    MBi_CheckSeqNoFixed(index);        // seqNoFixed�̃`�F�b�N
    MBi_CheckSeqNoVolat(index);        // seqNoVolat�̃`�F�b�N

    // ��M�r�[�R���̋��ʕ����̃f�[�^�擾
    {
        MBGameInfoRecvList *info = &mbrsp->list[index];

        // �����r�[�R�����o��������ُ�ȏ�Ԃ̐e�@�����O����B
        if (info->beaconNo == brecvBuffp->beaconNo)
        {
            if (++info->sameBeaconRecvCount > MB_SAME_BEACON_RECV_MAX_COUNT)
            {
                info->lifetimeCount = 0;        // �e�@���̎������[���ɂ��āA�폜������B
                MB_OUTPUT("The parent broadcast same beacon.: %d\n", index);
                MBi_InvalidateGameInfoBssID(&info->bssDesc.bssid[0]);
                return;                // ���̐e�@�����ɂ��Q�[������z�M���Ă�����A������폜������B
            }
        }
        else
        {
            info->sameBeaconRecvCount = 0;
        }
        // ����ȏ�Ԃ̐e�@�̃f�[�^��M
        info->beaconNo = brecvBuffp->beaconNo;
        info->lifetimeCount = MB_LIFETIME_MAX_COUNT;    // �e�@���̉����B
        info->gameInfo.ggid = brecvBuffp->ggid; // ggid    �̎擾�B
        info->gameInfo.fileNo = brecvBuffp->fileNo;     // fileNo  �̎擾�B
        info->linkLevel = linkLevel;   // �d�g���x�̎擾�B
        // bssDesc��SSID���Z�b�g����B
        MBi_SetSSIDToBssDesc(&info->bssDesc, info->gameInfo.ggid);
    }

    // ��M�r�[�R���̃f�[�^��ޖ��̃f�[�^�擾
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_VOLAT)
    {
        MBi_RecvVolatBeacon(index);
    }
    else
    {
        MBi_RecvFixedBeacon(index);
    }
}


// tgid�̃`�F�b�N
static void MBi_CheckTGID(WMBssDesc *bssDescp, int index)
{
    if (mbrsp->list[index].bssDesc.gameInfo.tgid == bssDescp->gameInfo.tgid)
    {
        return;
    }

    // tgid���A�b�v���Ă�����A���̐e�@�͗����グ������Ă���Ƃ݂Ȃ��āA�S�f�[�^���N���A���Ď�蒼���B
    MB_DEBUG_OUTPUT("\ntgid updated! : %x%x", *(u16 *)(&bssDescp->bssid[4]),
                    *(u32 *)bssDescp->bssid);
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MI_CpuCopy16(bssDescp, &mbrsp->list[index].bssDesc, sizeof(WMBssDesc));
    // tgid�X�V�̏ꍇ�́A�V����bssDesc���R�s�[����B
    MBi_LockScanTarget(index);         // �Y���e�@���X�L�������b�N����B
}


// Fixed�f�[�^��SeqNo�`�F�b�N
static void MBi_CheckSeqNoFixed(int index)
{
    // �V�[�P���X�ԍ����X�V����Ă����ꍇ�̑Ή�
    if (mbrsp->list[index].gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        return;
    }
    // �V�[�P���X�ԍ����X�V����Ă�����A����܂łɎ�M�����f�[�^���N���A����B
    MB_DEBUG_OUTPUT("\n seqNoFixed updated!");
    MB_DeleteRecvGameInfoWithoutBssdesc(index);
    MBi_LockScanTarget(index);         // �Y���e�@���X�L�������b�N����B
    mbrsp->list[index].gameInfo.seqNoFixed = brecvBuffp->seqNoFixed;
}


// Volat�f�[�^��SeqNo�`�F�b�N
static void MBi_CheckSeqNoVolat(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    // �V�[�P���X�ԍ����X�V����Ă����ꍇ�̑Ή�
    if (mbrsp->list[index].gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        MB_DEBUG_OUTPUT("\n seqNoVolat updated!");
        MBi_LockScanTarget(index);     // �X�L�������b�N�\�����ׂă��b�N����B
    }
}


// �Ώ�BSSID�̐e�@�Q�[������S�Ė����ɂ���B
static void MBi_InvalidateGameInfoBssID(u8 *bssidp)
{
    int     i;
    for (i = 0; i < MB_GAME_INFO_RECV_LIST_NUM; i++)
    {
        if ((mbrsp->usingGameInfoFlag & (0x01 << i)) == 0)
        {
            continue;
        }

        if (!WM_IsBssidEqual(bssidp, mbrsp->list[i].bssDesc.bssid))
        {
            continue;
        }

        // �폜�Ώۂ̃f�[�^�Ɣ���B
        mbrsp->list[i].lifetimeCount = 0;       // �e�@���̎������[���ɂ��āA�폜������B
        MB_OUTPUT("The parent broadcast same beacon.: %d\n", i);
    }
}


// �Q�[�����̌Œ�f�[�^���𕪊����ăr�[�R����M
static void MBi_RecvFixedBeacon(int index)
{
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];
    u32     lastAddr = (u32)&grecvp->gameInfo + sizeof(MBGameInfoFixed);
    u8     *dstp;

    // �V�[�P���X�ԍ����X�V����Ă��炸�A�����Ɏ擾�ς݂̃r�[�R���Ȃ��M���Ȃ��B
    if (grecvp->gameInfo.seqNoFixed == brecvBuffp->seqNoFixed)
    {
        if (grecvp->getFragmentFlag & (0x01 << brecvBuffp->data.fixed.fragmentNo))
        {
            return;
        }
    }

    // ��M�r�[�R������M�o�b�t�@���I�[�o�[���Ȃ����`�F�b�N�B
    if (brecvBuffp->dataAttr == MB_BEACON_DATA_ATTR_FIXED_NORMAL)
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed;
    }
    else
    {
        dstp = (u8 *)&grecvp->gameInfo.fixed.parent;
    }
    dstp += MB_BEACON_FIXED_DATA_SIZE * brecvBuffp->data.fixed.fragmentNo;
    // �Q�[�����o�b�t�@�̎�M�A�h���X���Z�o�B

    if ((u32)dstp + brecvBuffp->data.fixed.size > lastAddr)
    {
        MB_DEBUG_OUTPUT("recv beacon gInfoFixed Buffer over!\n");
        // �o�b�t�@���I�[�o�[����r�[�R���f�[�^�͖�������B
        return;
    }

    // ��M�r�[�R����ΏۃQ�[�����o�b�t�@�ɃZ�b�g
    MB_DEBUG_OUTPUT("recv fragment= %2d  adr = 0x%x", brecvBuffp->data.fixed.fragmentNo, dstp);
    MI_CpuCopy16(brecvBuffp->data.fixed.data, dstp, brecvBuffp->data.fixed.size);
    grecvp->gameInfo.dataAttr = brecvBuffp->dataAttr;
    grecvp->getFragmentFlag |= 0x01 << brecvBuffp->data.fixed.fragmentNo;
    grecvp->allFragmentFlag = (u32)((0x01 << brecvBuffp->data.fixed.fragmentMaxNum) - 1);
    MB_DEBUG_OUTPUT("\t now fragment = 0x%x \t all fragment = 0x%x\n",
                    grecvp->getFragmentFlag, grecvp->allFragmentFlag);
}


// �Q�[�����̗����f�[�^���𕪊����ăr�[�R����M
static void MBi_RecvVolatBeacon(int index)
{
    int     i;
    MBGameInfoRecvList *grecvp = &mbrsp->list[index];

    /* ���[�U��`�f�[�^�͏�Ɏ�M���� */
    for (i = 0; i < MB_USER_VOLAT_DATA_SIZE; i++)
    {
        grecvp->gameInfo.volat.userVolatData[i] = brecvBuffp->data.volat.userVolatData[i];
    }
    MI_CpuCopy16(brecvBuffp, &grecvp->bssDesc.gameInfo.userGameInfo, WM_SIZE_USER_GAMEINFO);

    // �����o�[���X�V�����o�����ۂ̏���
    if (grecvp->gameInfo.seqNoVolat != brecvBuffp->seqNoVolat)
    {
        if ((u8)(grecvp->gameInfo.seqNoVolat + 1) == brecvBuffp->seqNoVolat)
        {                              // seqNoVolat���P����Ȃ�A�ύX����Ă��Ȃ������o�[���̈��p��
            for (i = 0; i < MB_MEMBER_MAX_NUM; i++)
            {
                if (brecvBuffp->data.volat.changePlayerFlag & (0x02 << i))
                {
                    MI_CpuClear16(&grecvp->gameInfo.volat.member[i], sizeof(MBUserInfo));
                }
            }
            grecvp->getPlayerFlag &= ~brecvBuffp->data.volat.changePlayerFlag;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        else
        {                              // seqNoVolat�������Ƃ���Ă�����A����܂Ŏ�M�����S�����o�[�����N���A
            MI_CpuClear16(&grecvp->gameInfo.volat.member[0],
                          sizeof(MBUserInfo) * MB_MEMBER_MAX_NUM);
            grecvp->getPlayerFlag = 0;
            mbrsp->validGameInfoFlag &= ~(0x0001 << index);
        }
        grecvp->gameInfo.seqNoVolat = brecvBuffp->seqNoVolat;
    }
    else if (grecvp->getPlayerFlag == brecvBuffp->data.volat.nowPlayerFlag)
    {
        return;                        // �V�[�P���X�ԍ����X�V����Ă��炸�A�����Ɏ擾�ς݂̃r�[�R���Ȃ��M���Ȃ��B
    }

    // �v���C���[���̓ǂݏo��
    grecvp->gameInfo.volat.nowPlayerNum = brecvBuffp->data.volat.nowPlayerNum;
    grecvp->gameInfo.volat.nowPlayerFlag = brecvBuffp->data.volat.nowPlayerFlag;
    grecvp->gameInfo.volat.changePlayerFlag = brecvBuffp->data.volat.changePlayerFlag;
    grecvp->getPlayerFlag |= 0x0001;

    // �e�����o�[�̃��[�U�[���̓ǂݏo��
    for (i = 0; i < MB_SEND_MEMBER_MAX_NUM; i++)
    {
        int     playerNo = (int)brecvBuffp->data.volat.member[i].playerNo;
        if (playerNo == 0)
        {
            continue;
        }
        MB_DEBUG_OUTPUT("member %d recv.\n", playerNo);
        MI_CpuCopy16(&brecvBuffp->data.volat.member[i],
                     &grecvp->gameInfo.volat.member[playerNo - 1], sizeof(MBUserInfo));
        grecvp->getPlayerFlag |= 0x01 << playerNo;
    }
}


// �X�L�����Ώۂ�P��e�@�Ƀ��b�N����
static void MBi_LockScanTarget(int index)
{
    /* ���Ƀ��b�N���̃^�[�Q�b�g������Ȃ疳�� */
    if (mbrsp->nowScanTargetFlag)
    {
        return;
    }

    if (sLockFunc != NULL)
    {
        sLockFunc(mbrsp->list[index].bssDesc.bssid);
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_LOCKING;

    mbrsp->nowScanTargetFlag = (u16)(0x01 << index);    // �V�����X�L�������b�N�Ώۂ��r�b�g�Ŏ����B
    mbrsp->nowLockTimeCount = MB_LOCKTIME_MAX_COUNT;    // �X�L�������b�N���Ԃ�������
    MB_DEBUG_OUTPUT("scan target locked. : %x %x %x %x %x %x\n",
                    mbrsp->list[index].bssDesc.bssid[0],
                    mbrsp->list[index].bssDesc.bssid[1],
                    mbrsp->list[index].bssDesc.bssid[2],
                    mbrsp->list[index].bssDesc.bssid[3],
                    mbrsp->list[index].bssDesc.bssid[4], mbrsp->list[index].bssDesc.bssid[5]);
}


// �X�L�����Ώۂ̃��b�N����������B
static void MBi_UnlockScanTarget(void)
{
    if (mbrsp->nowScanTargetFlag == 0)
    {
        return;
    }

    if (sUnlockFunc != NULL)
    {
        sUnlockFunc();
    }
    mbrsp->scanCountUnit = MB_SCAN_COUNT_UNIT_NORMAL;
    mbrsp->nowScanTargetFlag = 0;
    mbrsp->notFoundLockTargetCount = 0;

    MB_DEBUG_OUTPUT(" unlock target\n");
}


// ��M�����Q�[���������S�ɍ폜�i�Q�[�����L���t���O���폜�j
void MB_DeleteRecvGameInfo(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    MI_CpuClear16(&mbrsp->list[index], sizeof(MBGameInfoRecvList));
}


// ��M�����Q�[������bssDesc�������č폜����B
void MB_DeleteRecvGameInfoWithoutBssdesc(int index)
{
    mbrsp->usefulGameInfoFlag &= ~(0x0001 << index);
    mbrsp->validGameInfoFlag &= ~(0x0001 << index);
    mbrsp->list[index].getFragmentFlag = 0;
    mbrsp->list[index].allFragmentFlag = 0;
    mbrsp->list[index].getPlayerFlag = 0;
    mbrsp->list[index].linkLevel = 0;
    MI_CpuClear16(&(mbrsp->list[index].gameInfo), sizeof(MBGameInfo));
}

// ��M�����e�@���\���̂ւ̃|�C���^���擾����
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index)
{
    // �L���ȃf�[�^���Ȃ����NULL��Ԃ�
    if ((mbrsp->usefulGameInfoFlag & (0x01 << index)) == 0)
    {
        return NULL;
    }

    return &mbrsp->list[index];
}


//=========================================================
// ���[�UVolatile�f�[�^�̐ݒ�
//=========================================================

/*---------------------------------------------------------------------------*
  Name:         MB_SetSendVolatileCallback

  Description:  �}���`�u�[�g�̃r�[�R�����M�R�[���o�b�N��ݒ肵�܂��B
  
  Arguments:    callback    ���M�����̃R�[���o�b�N�֐��ł��B
                            �f�[�^�����M�����x�ɃR�[���o�b�N���Ăяo����܂��B
                timing      �R�[���o�b�N�����^�C�~���O�B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetSendVolatCallback(MBSendVolatCallbackFunc callback, u32 timing)
{
    OSIntrMode enabled = OS_DisableInterrupts();

    sSendVolatCallback = callback;
    sSendVolatCallbackTiming = timing;

    (void)OS_RestoreInterrupts(enabled);
}



/*---------------------------------------------------------------------------*
  Name:         MB_SetUserVolatData

  Description:  �}���`�u�[�g�̃r�[�R�����̋󂫗̈�Ƀ��[�U�f�[�^��ݒ肵�܂��B
  
  Arguments:    ggid        MB_RegisterFile�Őݒ肵���v���O������ggid���w�肵�āA
                            ���̃t�@�C���̃r�[�R���Ƀ��[�U�f�[�^��t�����܂��B
                userData    �ݒ肷�郆�[�U�f�[�^�ւ̃|�C���^�B
                size        �ݒ肷�郆�[�U�f�[�^�̃T�C�Y (�ő�8�o�C�g�܂�)
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void MB_SetUserVolatData(u32 ggid, const u8 *userData, u32 size)
{
    MBGameInfo *gameInfo;

    SDK_ASSERT(size <= MB_USER_VOLAT_DATA_SIZE);
    SDK_NULL_ASSERT(userData);

    gameInfo = mbss.gameInfoListTop;
    if (gameInfo == NULL)
    {
        return;
    }

    while (gameInfo->ggid != ggid)
    {
        if (gameInfo == NULL)
        {
            return;
        }
        gameInfo = gameInfo->nextp;
    }

    {
        u32     i;

        OSIntrMode enabled = OS_DisableInterrupts();

        for (i = 0; i < size; i++)
        {
            gameInfo->volat.userVolatData[i] = userData[i];
        }

        (void)OS_RestoreInterrupts(enabled);
    }
}

/*---------------------------------------------------------------------------*
  Name:         MB_GetUserVolatData

  Description:  �r�[�R�����̋󂫗̈�ɂ��郆�[�U�f�[�^���擾���܂��B
  
  Arguments:    gameInfo    Scan���Ɏ擾����gameInfo�p�����[�^�ւ̃|�C���^�B
  
  Returns:      ���[�U�f�[�^�ւ̃|�C���^.
 *---------------------------------------------------------------------------*/
void   *MB_GetUserVolatData(const WMGameInfo *gameInfo)
{
    MbBeacon *beacon;

    SDK_NULL_ASSERT(gameInfo);

    if (!(gameInfo->attribute & WM_ATTR_FLAG_MB))
    {
        return NULL;
    }

    beacon = (MbBeacon *) (gameInfo->userGameInfo);

    if (beacon->dataAttr != MB_BEACON_DATA_ATTR_VOLAT)
    {
        return NULL;
    }

    return beacon->data.volat.userVolatData;
}

