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

#ifndef NITRO_MB_MB_GAMEINFO_H_
#define NITRO_MB_MB_GAMEINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <nitro/types.h>
#include <nitro/mb/mb.h>

#define MB_USER_VOLAT_DATA_SIZE     8


typedef void (*MBSendVolatCallbackFunc) (u32 ggid);

/*
 * �e�@�Q�[�����̂����A�ω����Ȃ����.
 * (seqNo���ω�������A��M�������B�i�A���A���ʂ͌Œ�̂܂܁j)
 */
typedef struct MBGameInfoFixed
{
    MBIconInfo icon;                   // 544B     �A�C�R���f�[�^
    MBUserInfo parent;                 //  22B     �e�@���[�U�[���
    u8      maxPlayerNum;              //   1B     �ő�v���C���[�l��
    u8      pad[1];
    u16     gameName[MB_GAME_NAME_LENGTH];      //  96B     �Q�[���^�C�g��
    u16     gameIntroduction[MB_GAME_INTRO_LENGTH];     // 192B     �Q�[�����e����
}
MBGameInfoFixed, MbGameInfoFixed;


/*
 * �e�@�Q�[�����̂����A�����I�ɕω�������B
 *�iseqNo�l���ω�������A��M�������j
 */
typedef struct MBGameInfoVolatile
{
    u8      nowPlayerNum;              //   1B     ���݂̃v���C���[��
    u8      pad[1];
    u16     nowPlayerFlag;             //   2B     ���݂̑S�v���C���[�̃v���C���[�ԍ����r�b�g�Ŏ����B
    u16     changePlayerFlag;          //   2B     ����̍X�V�ŕύX���ꂽ�v���C���[���̔ԍ����t���O�Ŏ����B
    MBUserInfo member[MB_MEMBER_MAX_NUM];       // 330B     �����o�[���
    u8      userVolatData[MB_USER_VOLAT_DATA_SIZE];     //   8B     ���[�U���Z�b�g�ł���f�[�^
}
MBGameInfoVolatile, MbGameInfoVolatile;


/* �r�[�R������ */
typedef enum MbBeaconDataAttr
{
    MB_BEACON_DATA_ATTR_FIXED_NORMAL = 0,       /* �A�C�R���f�[�^����̌Œ�f�[�^ */
    MB_BEACON_DATA_ATTR_FIXED_NO_ICON, /* �A�C�R���f�[�^�Ȃ��̌Œ�f�[�^ */
    MB_BEACON_DATA_ATTR_VOLAT          /* �����o�[��񓙂̗����I�ȃf�[�^ */
}
MBBeaconDataAttr, MbBeaconDataAttr;

/*  dataAttr = FIXED_NORMAL �̎��́AMbGameInfoFixed�̃f�[�^��擪  ����MB_BEACON_DATA_SIZE�P�ʂŕ������đ��M�B
    dataAttr = FIXED_NO_ICON            �V                   MBUserInfo ����@�@�V                                �i�A�C�R���f�[�^�ȗ��j
    dataAttr = VOLAT�̎��́A�u����̑��M�l���~���[�U�[���v�ő��M
*/

/*
 * �e�@�Q�[�����r�[�R��
 */
typedef struct MBGameInfo
{
    MBGameInfoFixed fixed;             // �Œ�f�[�^
    MBGameInfoVolatile volat;          // �����f�[�^
    u16     broadcastedPlayerFlag;     // �@�@�V�@�@�@�@�@�@�̔z�M�ς݃v���C���[�����r�b�g�Ŏ����B
    u8      dataAttr;                  // �f�[�^����
    u8      seqNoFixed;                // �Œ�̈�̃V�[�P���X�ԍ�
    u8      seqNoVolat;                // �����̈�̃V�[�P���X�ԍ�
    u8      fileNo;                    // �t�@�C��No.
    u8      pad[2];
    u32     ggid;                      // GGID
    struct MBGameInfo *nextp;          // ����GameInfo�ւ̃|�C���^�i�Е������X�g�j
}
MBGameInfo, MbGameInfo;


enum
{
    MB_SEND_VOLAT_CALLBACK_TIMING_BEFORE,
    MB_SEND_VOLAT_CALLBACK_TIMING_AFTER,
    // compatibility for typo. (not recommended)
    MB_SEND_VOLAT_CALLBACK_TIMMING_BEFORE = MB_SEND_VOLAT_CALLBACK_TIMING_BEFORE,
    MB_SEND_VOLAT_CALLBACK_TIMMING_AFTER  = MB_SEND_VOLAT_CALLBACK_TIMING_AFTER
};

void    MB_SetSendVolatCallback(MBSendVolatCallbackFunc callback, u32 timing);
void    MB_SetUserVolatData(u32 ggid, const u8 *userData, u32 size);
void   *MB_GetUserVolatData(const WMGameInfo *gameInfo);



#ifdef __cplusplus
}
#endif


#endif // NITRO_MB_MB_GAMEINFO_H_
