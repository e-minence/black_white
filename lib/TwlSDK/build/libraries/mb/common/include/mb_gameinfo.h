/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_gameinfo.h

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

/*
 * ���̃w�b�_�t�@�C���͓��������y�юq�@���ł̂ݎg�p���܂�.
 * �ʏ�̃}���`�u�[�g�e�@���쐬�����ł͕K�v����܂���.
 */


#ifndef MB_GAME_INFO_H_
#define MB_GAME_INFO_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb/mb_gameinfo.h>


//=============================================================================
// 
// �f�[�^�^�@��`
//
//=============================================================================

#define MB_GAMEINFO_PARENT_FLAG                     (0x0001)
#define MB_GAMEINFO_CHILD_FLAG( __child_aid__ )     ( 1 << ( __child_aid__ ) )

//---------------------------------------------------------
// �e�@����q�@�Ƀr�[�R���Ŕz�M����Q�[�����
//---------------------------------------------------------

/*
 * �q�@���Q�[������M���X�g�\����.
 */
typedef struct MBGameInfoRecvList
{
    MBGameInfo gameInfo;               // �e�@�Q�[�����
    WMBssDesc bssDesc;                 // �e�@�ڑ��p���
    u32     getFragmentFlag;           // ���ݎ�M�����f�Ѓr�[�R�����r�b�g�Ŏ����B
    u32     allFragmentFlag;           // fragmentMaxNum���r�b�g�Ɋ��Z�����l
    u16     getPlayerFlag;             // ���ݎ�M�ς݂̃v���C���[�t���O���r�b�g�Ŏ����B
    s16     lifetimeCount;             // ���̏��ɑ΂�������J�E���^�i���̐e�@�̃r�[�R������M������A���������т�j
    u16     linkLevel;                 /* �e�@�����Beacon��M���x�������A4�i�K�̒l. */
    u8      beaconNo;                  // �Ō�Ɏ�M�����r�[�R���ԍ�
    u8      sameBeaconRecvCount;       // �����r�[�R���ԍ���A�����Ď�M������
}
MBGameInfoRecvList, MbGameInfoRecvList;


/*
 * �q�@���r�[�R����M�X�e�[�^�X�\����
 */
typedef struct MbBeaconRecvStatus
{
    u16     usingGameInfoFlag;         // �Q�[�����̎�M�Ɏg�p����gameInfo�z��v�f���r�b�g�Ŏ����B
    u16     usefulGameInfoFlag;        // ��U�͑S�Ẵr�[�R������M���āAvalidGameInfoFlag���������Q�[�����������B
    // �i�ʐM�����o�[�̍X�V���ŁAvalidGameInfoFlag�͈ꎞ�I�ɗ�����ꍇ������̂ŁA�\����ڑ��ɂ͂�����̃t���O���g�p���Ĕ��f����B�j
    u16     validGameInfoFlag;         // �Q�[���������S�Ɏ�M���Ă���gameInfo�z��v�f���r�b�g�Ŏ����B
    u16     nowScanTargetFlag;         // ���݂�Scan�^�[�Q�b�g���r�b�g�Ŏ����B
    s16     nowLockTimeCount;          // ���݂�Scan�^�[�Q�b�g�̎c�胍�b�N���ԁB
    s16     notFoundLockTargetCount;   // ���݂�ScanTarget���A���Ō�����Ȃ�������
    u16     scanCountUnit;             // ���݂̃X�L�������Ԃ��J�E���g���Ɋ��Z�����l
    u8      pad[2];
    MBGameInfoRecvList list[MB_GAME_INFO_RECV_LIST_NUM];        // �Q�[������M���X�g
}
MbBeaconRecvStatus;
/*
 * ���q�@���́A
 * getFragmentFlag == allFragmentFlag�@���@
 * getPlayerFlag   == gameInfo.volat.nowPlayerFlag
 * �ŁA�e�@���擾�����ƂȂ�B
 */


/*
 * MB_RecvGameInfoBeacon, MB_CountGameInfoLifetime ��
 * �R�[���o�b�N�֐��ŕԂ����msg.
 */
typedef enum MbBeaconMsg
{
    MB_BC_MSG_GINFO_VALIDATED = 1,
    MB_BC_MSG_GINFO_INVALIDATED,
    MB_BC_MSG_GINFO_LOST,
    MB_BC_MSG_GINFO_LIST_FULL,
    MB_BC_MSG_GINFO_BEACON
}
MbBeaconMsg;

typedef void (*MBBeaconMsgCallback) (MbBeaconMsg msg, MBGameInfoRecvList * gInfop, int index);

typedef void (*MbScanLockFunc) (u8 *macAddress);
typedef void (*MbScanUnlockFunc) (void);

/******************************************************************************/
/* �ȉ��͓����g�p */


//------------------
// �e�@���֐�
//------------------

    // ���M�X�e�[�^�X�̏������B
void    MB_InitSendGameInfoStatus(void);

    // MbGameRegistry����MbGameInfo���쐬����B
void    MBi_MakeGameInfo(MBGameInfo *gameInfop,
                         const MBGameRegistry *mbGameRegp, const MBUserInfo *parent);

    // MBGameInfo�̎q�@�����o�[�����X�V����B
void    MB_UpdateGameInfoMember(MBGameInfo *gameInfop,
                                const MBUserInfo *member, u16 nowPlayerFlag, u16 changePlayerFlag);

    // ��������MBGameInfo���r�[�R���Ŕ��M����悤���M���X�g�ɒǉ�����B
void    MB_AddGameInfo(MBGameInfo *newGameInfop);

    // ���M���X�g�ɒǉ����Ă���MBGameInfo���폜����B
BOOL    MB_DeleteGameInfo(MBGameInfo *gameInfop);

    // ���M���X�g�ɓo�^����Ă���MBGameInfo���r�[�R���ɏ悹�Ĕ��M����B
void    MB_SendGameInfoBeacon(u32 ggid, u16 tgid, u8 attribute);


//------------------
// �q�@���֐�
//------------------
    // �r�[�R����M�X�e�[�^�X�o�b�t�@��static�Ɋm��
void    MBi_SetBeaconRecvStatusBufferDefault(void);
    // �r�[�R����M�X�e�[�^�X�o�b�t�@��ݒ�
void    MBi_SetBeaconRecvStatusBuffer(MbBeaconRecvStatus * buf);

    // �擾�����r�[�R�����}���`�u�[�g�e�@���ǂ����𔻒�
BOOL    MBi_CheckMBParent(WMBssDesc *bssDescp);

    // ��M�X�e�[�^�X�̏�����
void    MB_InitRecvGameInfoStatus(void);

    // ��M�����r�[�R������MBGameInfo�����o���B
BOOL    MB_RecvGameInfoBeacon(MBBeaconMsgCallback Callbackp, u16 linkLevel, WMBssDesc *bssDescp);

    // �e�@��񃊃X�g�̎����J�E���g
void    MB_CountGameInfoLifetime(MBBeaconMsgCallback Callbackp, BOOL found_parent);

    // �X�L�������b�N�֐���ݒ�
void    MBi_SetScanLockFunc(MbScanLockFunc lockFunc, MbScanUnlockFunc unlockFunc);

    // ��M�����e�@���\���̂ւ̃|�C���^���擾����
MBGameInfoRecvList *MB_GetGameInfoRecvList(int index);

/* �r�[�R����M��Ԃ��擾 */
const MbBeaconRecvStatus *MB_GetBeaconRecvStatus(void);

/* �w�肳�ꂽ�Q�[�����̍폜 */
void    MB_DeleteRecvGameInfo(int index);
void    MB_DeleteRecvGameInfoWithoutBssdesc(int index);


#ifdef __cplusplus
}
#endif

#endif // MB_GAME_INFO_H_
