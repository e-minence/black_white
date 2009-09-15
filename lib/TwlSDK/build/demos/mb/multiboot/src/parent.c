/*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - demos - multiboot
  File:     parent.c

  Copyright 2006-2009 Nintendo.  All rights reserved.

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
#include <nitro/wm.h>
#include <nitro/mb.h>

#include "common.h"
#include "dispfunc.h"


/*
 * �e�@���̊�{�I�ȏ���.
 *
 * MB���C�u�����̃T���v���̓}���`�u�[�g�@�\���g�����߁A
 * �����ʐM��(�������邢�͗L��)�ǂ����̊J���@�ނ𕡐��K�v�Ƃ��܂��B
 * $TwlSDK/bin/ARM9-TS/Rom/ �f�B���N�g���ɂ���
 * mb_child_NITRO.srl, mb_child_TWL.srl �v���O�������ŏI���@�ɂ�����}���`�u�[�g�q�@�Ɠ�����
 * �@�\��񋟂���T���v���ɂȂ��Ă��܂��̂ŁA
 * ���̃o�C�i�����T���v���v���O�����Ɠ������@�ő��̋@�ނɓǂݍ��݁A
 * �ꏏ�Ɏ��s���Ă��������B
 */


/******************************************************************************/
/* variables */

/*
 * �_�E�����[�h�v���O�����̃Z�O�����g����ǂݍ��ރ�����.
 */

static u8 *p_segbuf[GAME_PROG_MAX];
static u8 cursor = 0;
static MBUserInfo myUser;
static u16 parent_channel = PARENT_CHANNEL;
static u16 parent_send_size = 256;
static u8 entry_max = 15;

/*
 * ���̃T���v�����Őe�@������q�@�ւ̉������������䂷�邽�߂̃X�C�b�`.
 *
 * �q�@����̗v���͐e�@�̉�����҂��܂��̂�,
 * ���̉����^�C�~���O��S�q�@�ł��낦����ʂɂ����肷�邱�Ƃ�
 * �}���`�u�[�g�A�v���P�[�V�����̓����ݒ�ł��܂�.
 *
 * ��Ƃ���, �P�ɑ̌��ł�z�M���邾���Ȃ�Όʂɉ�����,
 * �����l�ΐ�ł���΃G���g���[�l�����B���Ɉꊇ��������, �����l�����܂�.
 */

static u8 auto_accept = 0;             /* �ڑ����������� */
static u8 auto_send = 0;               /* �_�E�����[�h���������� */
static u8 auto_boot = 0;               /* �u�[�g���������� */

/******************************************************************************/
/* function */

/* �t�@�C���o�b�t�@�|�C���^�������� */
void InitFileBuffer(void)
{
    int     i;

    for (i = 0; i < GAME_PROG_MAX; i++)
    {
        p_segbuf[i] = NULL;
    }

}

/* �m�ۂ��݂̃t�@�C���o�b�t�@���J�� */
static void FreeFileBuffer(void)
{
    int     i;

    for (i = 0; i < GAME_PROG_MAX; i++)
    {
        if (p_segbuf[i])
        {
            OS_Free(p_segbuf[i]);
            p_segbuf[i] = NULL;
        }

    }

}

/* MBUserInfo�̐��� */
static void MakeUserInfo(MBUserInfo *user, u8 favoriteColor, u8 playerNo, u16 *name,
                         u16 name_length)
{
    SDK_ASSERT(user != NULL);
    user->favoriteColor = favoriteColor;
    user->nameLength = (u8)(name_length);
    MI_CpuCopy8((char *)name, (char *)user->name, (u32)(name_length * 2));
    user->playerNo = playerNo;
}

/* Channel ������ύX���Ă��� */
static BOOL changeChannel(u16 *p_channel)
{
    u16     channel_bmp, channel, i;

    /* channel bitmap �擾 */
    channel_bmp = WM_GetAllowedChannel();

    /* �g�p�\��channel�������ꍇ�́AOS_Panic���R�[���D */
    if (channel_bmp == 0)
    {
        OS_Panic("No Available Parent channel\n");
        return FALSE;
    }

    /* �g�p�\��channel�����݂����ꍇ�D */
    for (i = 0, channel = *p_channel;
         i < 16; i++, channel = (u16)((channel == 16) ? 1 : channel + 1))
    {
        if (channel_bmp & (1 << (channel - 1)))
        {
            /* ���o���ꂽchannel���O�̂��̂�
               �����������ꍇ�́A�ʂ�channel��
               ��������B
             */
            if (*p_channel != channel)
            {
                *p_channel = channel;
                break;
            }
        }

    }

    return TRUE;

}

/* �e�@�t�@�C���_�E�����[�h��Ԓʒm�R�[���o�b�N */
static void ParentStateCallback(u16 aid, u32 status, void *arg)
{

    switch (status)
    {
    case MB_COMM_PSTATE_INIT_COMPLETE:
        /* ���������� ���� */
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;
            BgSetMessage(WHITE, "MB lib Init complete");
        }
        break;

    case MB_COMM_PSTATE_CONNECTED:
        /* �q�@����ڑ��������u�Ԃ̒ʒm */
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;

            BgSetMessage(YELLOW, "Connected [%2d]", aid);
            BgSetMessage(YELLOW, "MACAddress %04x %04x %04x",
                         p->macAddress[0], p->macAddress[1], p->macAddress[2]);
        }
        break;

    case MB_COMM_PSTATE_DISCONNECTED:
        /* �q�@���ڑ���؂������̒ʒm */
        {
            WMStartParentCallback *p = (WMStartParentCallback *)arg;

            BgSetMessage(RED, "Disconnected [%2d]", aid);
            /* �ؒf����ReasonCode�\�� */
            //                  BgSetMessage( WHITE, "reason code : %2d", p->reason);
        }
        break;

    case MB_COMM_PSTATE_KICKED:
        /* �q�@���L�b�N�������̒ʒm */
        BgSetMessage(RED, "Entry Refused [%2d]", aid);
        break;

    case MB_COMM_PSTATE_REQ_ACCEPTED:
        /* �q�@����̃_�E�����[�h�v�����󂯂����̒ʒm */
        BgSetMessage(YELLOW, "Download Request [%2d]", aid);
        break;

    case MB_COMM_PSTATE_SEND_PROCEED:
        /* �q�@�փo�C�i�����M���J�n���鎞�̒ʒm */
        BgSetMessage(CYAN, "Start Sending [%2d]", aid);
        break;

    case MB_COMM_PSTATE_SEND_COMPLETE:
        /* �q�@�ւ̃o�C�i�����M���I���������̒ʒm */
        BgSetMessage(CYAN, "Send Completed [%2d]", aid);
        if (auto_boot == 1)
        {
            /* �����u�[�g�v�� */
            (void)MB_CommBootRequest(aid);
            BgSetMessage(WHITE, "-->Boot Request [%2d]", aid);
        }
        break;

    case MB_COMM_PSTATE_BOOT_REQUEST:
        /* �q�@�ւ̃u�[�g�v�����M���̒ʒm */
        BgSetMessage(CYAN, "Send boot request [%2d]", aid);
        break;

    case MB_COMM_PSTATE_BOOT_STARTABLE:
        /* ��A�̃_�E�����[�h�������I���A�}���`�u�[�g�Ή��A�v�����N���ɂȂ������̒ʒm */
        BgSetMessage(YELLOW, "Boot ready [%2d]", aid);
        break;

    case MB_COMM_PSTATE_REQUESTED:
        /* �q�@����G���g���[���N�G�X�g�������u�Ԃ̒ʒm */
        BgSetMessage(YELLOW, "Entry Requested [%2d]", aid);

        if (auto_accept == 1)
        {
            /* �����G���g���[���� */
            (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_ACCEPT);
            BgSetMessage(WHITE, "-->Entry Accept [%2d]", aid);
        }
        break;

    case MB_COMM_PSTATE_MEMBER_FULL:
        /* ����ɒB�����Q�[���Ɏq�@���ڑ����Ă������̒ʒm */
        BgSetMessage(RED, "Entry Member full [%2d]", aid);
        break;

    case MB_COMM_PSTATE_END:
        /* �e�@���I���������̒ʒm */
        BgSetMessage(WHITE, "MB lib End");
        prog_state = STATE_MENU;
        break;

    case MB_COMM_PSTATE_WAIT_TO_SEND:
        /* �q�@�ւ̑��M�ҋ@��Ԃ��n�܂������̒ʒm */
        BgSetMessage(CYAN, "Waiting to send [%2d]", aid);
        if (auto_send == 1)
        {
            /* �������M�J�n */
            (void)MB_CommStartSending(aid);
            BgSetMessage(WHITE, "-->Start Sending [%2d]", aid);
        }
        break;

    }
}



/* �e�@������ */
void ParentInit(void)
{
    int     i;

    OSOwnerInfo MyInfo;

    OS_GetOwnerInfo(&MyInfo);
    MakeUserInfo(&myUser, MyInfo.favoriteColor, 0, (u16 *)MyInfo.nickName, MyInfo.nickNameLength);

    BgClear();
    BgSetMessage(WHITE, "Initializing Parent.");

    /*
     * MB �e�@������J�n���܂�.
     * �����ł͑S�Ă̏����������������̑ҋ@��ԂɂȂ�܂�.
     */

    /* �ď��������ɁA�ȑO�g�p���Ă����q�[�v�����. */
    FreeFileBuffer();

    /* �}���`�u�[�g���C�u���������� */
    (void)MB_Init(cwork, &myUser, MY_GGID, MB_TGID_AUTO, MB_DMA_NO);

    /* ����M�f�[�^�T�C�Y���w�� */
    (void)MB_SetParentCommParam(parent_send_size, entry_max);
    (void)MB_CommSetParentStateCallback(ParentStateCallback);
    // MB_StartParent �͓����� WM_Initialize ���Ă�
    if(MB_StartParent(parent_channel) != MB_SUCCESS)
    {
        OS_Panic("MB_StartParent() failed.");
    }

    BgSetMessage(WHITE, "** Parameters **");
    BgSetMessage(WHITE, "channel      : %2d", parent_channel);
    BgSetMessage(WHITE, "send size    : %3d", parent_send_size);
    BgSetMessage(WHITE, "max children : %2d", entry_max);
    BgSetMessage(WHITE, "GGID:%08x TGID:%04x", MY_GGID, MB_GetTgid());

    prog_state = STATE_MB_PARENT;
    cursor = 0;

    /*
     * ������, �_�E�����[�h�v���O�����t�@�C������o�^���܂�.
     *
     * �v���O�����t�@�C���S�̂̂���, �}���`�u�[�g�N�����ɕK�v�Ȃ̂�
     * ARM9-static �Z�O�����g����� ARM7-static �Z�O�����g�݂̂ł�.
     * (����ȊO�̓I�[�o�[���C��f�[�^�t�@�C�����ɂȂ�܂�)
     *
     * �v���O�������炱���̏�񂾂���ǂݏo�����߂�,
     * �܂� MB_ReadSegment() �֐����g�p���܂�.
     * MB_GetSegmentLength() �֐��ŃT�C�Y���擾���邱�Ƃɂ��
     * ���̃T���v���ł̓������𓮓I�Ɋm�ۂ��Ă��܂�.
     *
     * �v���O�����t�@�C���� FS_ReadFile() �œǂݏo����K�v������܂�.
     * �ʏ�̓v���O������ CARD-ROM ���Ƀt�@�C���Ƃ��ĕێ�����͂��Ȃ̂�
     * ���͂���܂���, ��������ȃ}���`�u�[�g�V�X�e����z�肷��ꍇ,
     * FSArchive �œƎ��A�[�J�C�u���\�z���đΏ����Ă�������.
     */
    for (i = 0; i < GAME_PROG_MAX; ++i)
    {
        FSFile  file[1];
        FS_InitFile(file);

        /*
         * �q�@�v���O�����t�@�C�����J��.
         * ���̃t�@�C���͈ȉ��� MB_ReadSegment() �Ŏg�p���܂�.
         */
        if (!FS_OpenFileEx(file, mbGameList[i].romFilePathp, FS_FILEMODE_R))
        {
            OS_TPrintf("ParentInit : file cannot open (mbGameList[i].romFilePathp=\"%s)\"\n",
                       mbGameList[i].romFilePathp ? mbGameList[i].romFilePathp : "(NULL)");
        }
        else
        {
            /*
             * �Z�O�����g���̃T�C�Y���擾.
             * �����ȃ_�E�����[�h�v���O�����łȂ��ꍇ,
             * ���̃T�C�Y�� 0 ���Ԃ��Ă��܂�.
             */
            u32     length = MB_GetSegmentLength(file);
            if (length == 0)
            {
                OS_TPrintf
                    ("ParentInit : specified file may be invalid format. (mbGameList[i].romFilePathp=\"%s)\"\n",
                     mbGameList[i].romFilePathp ? mbGameList[i].romFilePathp : "(NULL)");
            }
            else
            {
                /*
                 * �Z�O�����g���̂��߂̃��������m��.
                 * ���̃������̓T�C�Y�����[���ł����
                 * �ÓI�ɗp�ӂ���Ă��Ă��\���܂���.
                 */
                p_segbuf[i] = (u8 *)OS_Alloc(length);
                if (!p_segbuf[i])
                {
                    OS_TPrintf("ParentInit : memory allocation failed. (%d BYTE)\n", length);
                }
                else
                {
                    /*
                     * �Z�O�����g�����t�@�C�����璊�o.
                     * ���o�����Z�O�����g����, �_�E�����[�h�v���O�����z�M��
                     * ���C����������ɏ풓�����Ă����K�v������܂�.
                     *
                     * �T�C�Y�擾�����������ɂ��ւ�炸
                     * �����Œ��o���������s����̂�,
                     * �t�@�C���n���h���ɉ����ύX��^�����ꍇ�Ȃǂł�.
                     * (�t�@�C�������, �ʒu���V�[�N����, ...)
                     */
                    if (!MB_ReadSegment(file, p_segbuf[i], length))
                    {
                        OS_TPrintf("ParentInit : failed to extract segment\n");
                    }
                    else
                    {
                        /*
                         * ���o�����Z�O�����g���� MBGameRegistry ��
                         * �_�E�����[�h�v���O������o�^���܂�.
                         *
                         */

                        if (MB_RegisterFile(&mbGameList[i], p_segbuf[i]))
                        {
                            BgSetMessage(LIGHT_GREEN, "Registered\"%s\"",
                                         mbGameList[i].romFilePathp);
                        }
                        else
                        {
                            OS_TPrintf("ParentInit : failed to register file No %d\n", i);
                        }
                    }
                }
            }
        }
        if (FS_IsFile(file))
            (void)FS_CloseFile(file);
    }

}


/* 1 �t���[�����Ƃ̐e�@���C������ */
void ParentMain(void)
{
    char    userName[MB_USER_NAME_LENGTH * 2 + 1];
    const int children_num = MB_CommGetChildrenNumber();
    const u16 keyData = ReadKeySetTrigger(PAD_Read());
    enum
    { DISP_OX = 2, DISP_OY = 3 };
    u16     i;
    OSIntrMode enabled;
    enum
    { PSTATE_NUM = 16 };
    const char *pstate_string[] = {
        "NONE        ",                //      STATE_NONE
        "INIT OK     ",                //      STATE_INIT_COMPLETE
        "CONNECTED   ",                //      STATE_CONNECTED
        "DISCONNECTED",                //      STATE_DISCONNECTED
        "KICKED      ",                //      STATE_KICKED
        "ENTRY OK    ",                //      STATE_REQ_ACCEPTED
        "SENDING     ",                //      STATE_SEND_PROCEED
        "SEND END    ",                //      STATE_SEND_COMPLETE
        "BOOT REQ    ",                //      STATE_BOOT_REQUEST
        "BOOT READY  ",                //      STATE_BOOT_STARTABLE
        "ENTRY REQ   ",                //      STATE_REQUESTED
        "MEMBER FULL ",                //      STATE_MEMBER_FULL
        "END         ",                //      STATE_END
        "ERROR       ",                //      STATE_ERROR
        "WAIT TO SEND",                //      STATE_WAIT_TO_SEND
    };

    BgPrintf(DISP_OX, DISP_OY - 2, WHITE, "CH:%2d  Max Children:%2d", parent_channel, entry_max);
    BgPutString(DISP_OX, DISP_OY - 1, WHITE, "AID USERNAME STATE        ");
    BgPutString(DISP_OX, DISP_OY + MB_MAX_CHILD + 1, WHITE, "A:operate B:refuse SEL:restart");
    BgPutString(DISP_OX, DISP_OY + MB_MAX_CHILD + 2, WHITE, "START:operate all children");

    enabled = OS_DisableInterrupts();  /* ���荞�݋֎~ */

    /* �q�@���X�g�̕\�� */
    for (i = 0; i < entry_max; ++i)
    {
        const u16 aid = (u16)(i + 1);
        const MBUserInfo *p_child = MB_CommGetChildUser(aid);
        int     p_state;

        BgPutString(DISP_OX, (s16)(DISP_OY + i), WHITE, "                         ");

        BgPrintf(DISP_OX + 0, (s16)(DISP_OY + i), WHITE, "%2d  ", aid);

        if (p_child && p_child->nameLength)
        {
            u8      palette = p_child->favoriteColor;
            MI_CpuCopy8(p_child->name, userName, (u32)(p_child->nameLength * 2));
            BgPutStringN((s16)(DISP_OX + 4), (s16)(DISP_OY + i), palette, userName, 8);
        }

        p_state = MB_CommGetParentState(aid);

        if (p_state < PSTATE_NUM)
        {
            BgPrintf((s16)(DISP_OX + 13), (s16)(DISP_OY + i), WHITE,
                     "%s  ", (char *)pstate_string[p_state]);
        }

        BgPutChar((s16)(DISP_OX - 2), (s16)(DISP_OY + i), RED, (char)((i == cursor) ? '*' : ' '));
    }

    (void)OS_RestoreInterrupts(enabled);        /* ���荞�݋֎~���� */

    /* �S���̎q�@���Ώ� */
    if (keyData & PAD_BUTTON_START)
    {
        BOOL    result = FALSE;

        if (!auto_accept)
        {
            for (i = 1; i <= entry_max; i++)
            {
                /* �G���g���[���� */
                if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(i))
                {
                    result = MB_CommResponseRequest(i, MB_COMM_RESPONSE_REQUEST_ACCEPT);
                }
            }
            if (TRUE == result)
                return;
        }

        if (!auto_send)
        {
            result = MB_CommStartSendingAll();
            if (TRUE == result)
                return;
        }

        if (!auto_boot)
        {
            result = MB_CommBootRequestAll();
            if (TRUE == result)
                return;
        }
    }

    /* �w��q�@�֌ʂɑ��� */
    if (keyData & PAD_BUTTON_A)
    {
        const u16 aid = (u16)(cursor + 1);

        BgSetMessage(WHITE, "AID%2d State:%s", aid, pstate_string[MB_CommGetParentState(aid)]);

        /* �G���g���[���� */
        if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(aid))
        {
            if (!auto_accept)
            {
                (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_ACCEPT);
                BgSetMessage(WHITE, "-->Entry Accept [%2d]", aid);
            }
        }
        /* ���M�J�n */
        else if (MB_COMM_PSTATE_WAIT_TO_SEND == MB_CommGetParentState(aid))
        {
            if (!auto_send)
            {
                (void)MB_CommStartSending(aid);
                BgSetMessage(WHITE, "-->Start Sending [%2d]", aid);
            }
        }
        else
            /* �u�[�g�v�� */
        if (TRUE == MB_CommIsBootable(aid))
        {
            if (!auto_boot)
            {
                (void)MB_CommBootRequest(aid);
                BgSetMessage(WHITE, "-->Boot Request [%2d]", aid);
            }
        }
    }
    else
        /* �w��q�@�փG���g���[���� */
    if (keyData & PAD_BUTTON_B)
    {
        const u16 aid = (u16)(cursor + 1);

        BgSetMessage(WHITE, "AID%2d State:%s", aid, pstate_string[MB_CommGetParentState(aid)]);
        if (MB_COMM_PSTATE_REQUESTED == MB_CommGetParentState(aid))
        {
            if (!auto_accept)
            {
                (void)MB_CommResponseRequest(aid, MB_COMM_RESPONSE_REQUEST_KICK);
                BgSetMessage(CYAN, "Entry Refuse [%2d]", aid);
            }
        }
    }

    /* SELECT ����������I�� -> �ď����� */
    if (keyData & PAD_BUTTON_SELECT)
    {
        /* �`�����l���ύX */
        (void)changeChannel(&parent_channel);
        MB_End();
    }

    /* �J�[�\���ԍ��ύX */
    if (keyData & PAD_KEY_DOWN)
    {
        (void)RotateU8(&cursor, 0, (u8)(entry_max - 1), 1);
    }
    else if (keyData & PAD_KEY_UP)
    {
        (void)RotateU8(&cursor, 0, (u8)(entry_max - 1), -1);
    }

}

/* �e�@MP���M�T�C�Y���Z�b�g */
void SetParentSendSize(u16 p_size)
{
    parent_send_size = p_size;
}

/* �e�@MP���M�T�C�Y���擾 */
u16 GetParentSendSize(void)
{
    return parent_send_size;
}

/* �q�@�ő�ڑ������Z�b�g */
void SetMaxEntry(u8 num)
{
    entry_max = num;
}

/* �q�@�ő�ڑ������擾 */
u8 GetMaxEntry(void)
{
    return entry_max;
}

/* �e�@�̃I�[�g����ݒ� */
void SetAutoOperation(u8 fAccept, u8 fSend, u8 fBoot)
{
    auto_accept = fAccept;
    auto_send = fSend;
    auto_boot = fBoot;
}
