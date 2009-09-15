/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_private.h

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

/*
    MB���C�u�����̊e�\�[�X�t�@�C���ł́A���̃w�b�_�t�@�C���݂̂��C���N���[�h���Ă��܂��B
*/

#ifndef _MB_PRIVATE_H_
#define _MB_PRIVATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#if	!defined(NITRO_FINALROM)
/*
#define PRINT_DEBUG
*/
#endif

#define CALLBACK_WM_STATE       0

#include <nitro/types.h>
#include <nitro/wm.h>
#include <nitro/mb.h>
#include "mb_wm_base.h"
#include "mb_common.h"
#include "mb_block.h"
#include "mb_rom_header.h"
#include "mb_gameinfo.h"
#include "mb_fileinfo.h"
#include "mb_child.h"

/* �L���b�V�������̂��߂̐V�@�\ */
#include "mb_cache.h"
#include "mb_task.h"

/* �f�o�b�O�o�� */
#ifdef  PRINT_DEBUG
#define MB_OUTPUT( ... )        OS_TPrintf( __VA_ARGS__ )
#define MB_DEBUG_OUTPUT( ... )  MBi_DebugPrint(__FILE__, __LINE__, __FUNCTION__, __VA_ARGS__ )

#else
#define MB_OUTPUT( ... )        ((void)0)
#define MB_DEBUG_OUTPUT( ... )  ((void)0)
#endif

// ----------------------------------------------------------------------------
// definition

/* ----------------------------------------------------------------------------

    �u���b�N�]���֘A��`

   ----------------------------------------------------------------------------*/

#define MB_IPL_VERSION                          (0x0001)
#define MB_MAX_BLOCK                            12288   // max 12288 * 256 = 3.0Mbyte

#define MB_NUM_PARENT_INFORMATIONS              16      /* �ő�e�@�� */

#define MB_SCAN_TIME_NORMAL                     WM_DEFAULT_SCAN_PERIOD
#define MB_SCAN_TIME_NORMAL_FOR_FAKE_CHILD      20
#define MB_SCAN_TIME_LOCKING                    220

//SDK_COMPILER_ASSERT( sizeof(MBDownloadFileInfo) <= MB_COMM_BLOCK_SIZE );


/* ----------------------------------------------------------------------------

    �����g�p�^��`

   ----------------------------------------------------------------------------*/

/*
 * MB ���C�u�����Ŏg�p���郏�[�N�̈�.
 */

typedef struct MB_CommCommonWork
{
    //  --- ����    ---
    //  ����M�o�b�t�@
    u32     sendbuf[MB_MAX_SEND_BUFFER_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    u32     recvbuf[MB_MAX_RECV_BUFFER_SIZE * MB_MAX_CHILD * 2 / sizeof(u32)] ATTRIBUTE_ALIGN(32);
    MBUserInfo user;                   // ���@�̃��[�U�[���
    u16     isMbInitialized;
    int     block_size_max;
    BOOL    start_mp_busy;             /* WM_StartMP() is operating */
    BOOL    is_started_ex;             /* WM_StartParentEX() called */
    u8      padding[28];
}
MB_CommCommonWork;

typedef struct MB_CommPWork
{
    MB_CommCommonWork common;

    //  --- �e�@�p  ---
    MBUserInfo childUser[MB_MAX_CHILD] ATTRIBUTE_ALIGN(4);
    u16     childversion[MB_MAX_CHILD]; // �q�@�̃o�[�W�������(�q�@�䐔��)
    u32     childggid[MB_MAX_CHILD];   // �q�@��GGID(�q�@�䐔��)
    MBCommPStateCallback parent_callback;       // �e�@�̏�ԃR�[���o�b�N�֐��|�C���^
    int     p_comm_state[MB_MAX_CHILD]; // ���ꂼ��̎q�@�ɑ΂���state������
    u8      file_num;                  // �o�^����Ă���t�@�C����
    u8      cur_fileid;                // ���ݑ��M����File�ԍ�
    s8      fileid_of_child[MB_MAX_CHILD];      // �q�@�̃��N�G�X�g���Ă���File�ԍ�(���N�G�X�g�����̏ꍇ��-1)
    u8      child_num;                 // �G���g���[�q�@��
    u16     child_entry_bmp;           // �q�@�̃G���g���[�Ǘ�bitmap
    MbRequestPieceBuf req_data_buf;    // �q�@�̕������N�G�X�g��M�p�̃o�b�t�@
    u16     req2child[MB_MAX_CHILD];   // �q�@�֑��M���郊�N�G�X�g
    MBUserInfo childUserBuf;           // �A�v���P�[�V�����ɓn�����߂́A�q�@���o�b�t�@

    //  �t�@�C�����
    struct
    {
        // DownloadFileInfo�̃o�b�t�@
        MBDownloadFileInfo dl_fileinfo;
        MBGameInfo game_info;
        MB_BlockInfoTable blockinfo_table;
        MBGameRegistry *game_reg;
        void   *src_adr;               //  �e�@��̃u�[�g�C���[�W�擪�A�h���X
        u16     currentb;              // ���݂̃u���b�N
        u16     nextb;                 // ���񑗐M����u���b�N
        u16     pollbmp;               // �f�[�^���M�pPollBitmap
        u16     gameinfo_child_bmp;    // GameInfo�X�V�̂��߂̌��݂̃����obitmap
        u16     gameinfo_changed_bmp;  // GameInfo�X�V�̂��߂̕ύX���������������obitmap
        u8      active;
        u8      update;

        /*
         * �L���b�V���������[�h�̑Ή��̂��߂ɒǉ�.
         * ����炪�w�����e�� src_adr �̒�.
         */
        MBiCacheList *cache_list;      /* �L���b�V�����X�g */
        u32    *card_mapping;          /* �e�Z�O�����g�擪�� CARD �A�h���X */

    }
    fileinfo[MB_MAX_FILE];

    BOOL    useWvrFlag;                // WVR���g�p���Ă��邩�ǂ����̃t���O
    u8      padding2[20];

    /* �^�X�N�X���b�h�̂��߂ɒǉ� */
    u8      task_work[2 * 1024];
    MBiTaskInfo cur_task;

}
MB_CommPWork;


typedef struct MB_CommCWork
{
    MB_CommCommonWork common;

    //  --- �q�@�p  ---
    WMBssDesc bssDescbuf ATTRIBUTE_ALIGN(32);   // WMBssDesc�̃o�b�N�A�b�v
    MBDownloadFileInfo dl_fileinfo;    // �q�@��DownloadFileInfo�o�b�t�@
    MBCommCStateCallbackFunc child_callback;    // �q�@�̏�ԃR�[���o�b�N�֐��|�C���^
    int     c_comm_state;              // �q�@��state
    int     connectTargetNo;           // �ڑ��Ώۂ�MbBeaconRecvStatus���X�gNo.
    u8      fileid;                    // ���N�G�X�g����File�ԍ�
    u8      _padding1[1];
    u16     user_req;
    u16     got_block;                 // Download�ς݃u���b�N��
    u16     total_block;               // Download�t�@�C�����u���b�N
    u8      recvflag[MB_MAX_BLOCK / 8]; // �u���b�N��M��Ԃ�\���t���O
    MB_BlockInfoTable blockinfo_table;
    int     last_recv_seq_no;          // �O���M�����u���b�N�V�[�P���X�ԍ�
    u8      boot_end_flag;             // BOOT_READY�ɂ��MB�I�����������ۂ��̃t���O
    u8      _padding2[15];
}
MB_CommCWork;


/* ----------------------------------------------------------------------------

    �����g�p�ϐ���`

   ----------------------------------------------------------------------------*/

extern MB_CommCommonWork *mbc;
/* �e�@�p���[�N�ւ̃|�C���^�}�N�� */
#define pPwork                                  ((MB_CommPWork*)mbc)
/* �q�@�p���[�N�ւ̃|�C���^�}�N�� */
#define pCwork                                  ((MB_CommCWork*)mbc)


#ifdef __cplusplus
}
#endif

#endif /*  _MB_PRIVATE_H_  */
