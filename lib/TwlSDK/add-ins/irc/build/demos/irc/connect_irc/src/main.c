/*---------------------------------------------------------------------------*
  Project:  TwlSDK - IRC - demos - connect_irc
  File:     main.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Log: main.c,v $

  $NoKeywords: $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/card.h>
#include <nitro/fs.h>

#include <nitro/card/backup_ex.h>
#include <nitro/irc.h>


/* ��ʕ`��p�̃��[�`�� */
#include "draw.h"

#define IRC_DEBUG   (1)
#define MY_UNIT_NUMBER  (0x7F)

static void TestWriteAndVerify(void);
static void ShowResult(void);

/* CARD ���C�u�����p�ϐ� */

/*
 * CARD �o�b�N�A�b�v�̃��b�N�p ID.
 * CARD �n�[�h�E�F�A���\�[�X�� FS ���C�u�����ȂǂƔr���g�p���邽��
 * CARD_LockBackup() / CARD_UnlockBackup() �Ŏg�p���܂�.
 */
static u16 card_lock_id;

/*
 * CARD �A�N�Z�X�֐��̃G���[���̌��ʒl.
 * CARD_GetResultCode() �̕Ԃ�l�� CARD_UnlockBackup() ���ŕύX�����̂�,
 * �A�N�Z�X�����ƃG���[����������Ă���ꍇ�ɂ͂��̓_�ɒ��ӂ��Ă�������.
 */
static CARDResult last_result = CARD_RESULT_SUCCESS;

// �������݃e�X�g�����s���ꂽ���̃t���O.
static BOOL is_test_run;

#define MY_TRNS_SIZE        (128)   // (IRC_TRNS_MAX)
#define DATA_CONFIRM_SIZE   (0x80)
/**********************************************************************/
static u32 cnt_success;
u8 data_up[MY_TRNS_SIZE];
u8 data_down[MY_TRNS_SIZE];

static int cnt_userdata = 0;
/**********************************************************************/
static void init_data(void)
{
    int i;

    for (i = 0; i < MY_TRNS_SIZE; i++)
    {
        data_up[i] = (u8)i;
    }
    for (i = 0; i < MY_TRNS_SIZE; i++)
    {
        data_down[i] = (u8)(data_up[MY_TRNS_SIZE-1-i]);
    }
}

// �ڑ��R�[���o�b�N
static void connectCallback(u8 id, BOOL isSender)
{
    if (id != MY_UNIT_NUMBER)
    {
        return;
    }
    cnt_userdata = 0;
#ifdef IRC_DEBUG
    DrawText(0, 8, "CONNECT");
#endif
    if (!isSender) // ���V�[�o����̍ŏ��̑��M
    {
        data_down[0] = (u8)cnt_userdata;
        IRC_Send((u8*)data_down, MY_TRNS_SIZE, 0x02);
    }
}

// �f�[�^��M�R�[���o�b�N(�Z���_�[�ƃ��V�[�o�[�ŋ���)
static void recvCallback(u8 *data, u8 size, u8 command, u8/*id*/)
{
    u8 value;

    if (size != MY_TRNS_SIZE)
    {
        return;
    }

    value = data[0];
    switch (command)
    {
    case 0x02: // �Z���_�[��2����M
        if (value >= DATA_CONFIRM_SIZE)
        {
            break;
        }
        cnt_success++;
      
        data_up[0] = (u8)value;
        IRC_Send((u8*)data_up, MY_TRNS_SIZE, 0x04);

        SetTextColor(RGB555_GREEN);
        FillRect(8, 48, 8*8, 8, RGB555_BLACK);
        DrawText(8, 48, "%02X TIMES", value);
        if (value == 0)
        {
            FillRect(0, (64-8), 8*32, 8*16, RGB555_BLACK);
        }
        DrawText((value&0x7)*16*2, (64-8)+(value/8)*8, "%02X", value);
        break;
    case 0x04: // ���V�[�o�[��4����M
        if (value >= DATA_CONFIRM_SIZE)
        {
            break;
        }
        if (value != cnt_userdata)
        {
            break;
        }
        cnt_success++;

        FillRect(8, 48, 8*8, 8, RGB555_BLACK);
        SetTextColor(RGB555_GREEN);
        DrawText(8, 48, "%02X TIMES", value);
        if (value == 0)
        {
            FillRect(0, (64-8), 8*32, 8*16, RGB555_BLACK);
        }
        DrawText((value&0x7)*16*2, (64-8)+(value/8)*8, "%02X", value);

        cnt_userdata = (cnt_userdata+1) & (DATA_CONFIRM_SIZE-1);
        data_down[0] = (u8)cnt_userdata;
        IRC_Send(data_down, MY_TRNS_SIZE, 0x02);
        break;
    case 0xF4: // COMMAND_SHUTDOWN:
#ifdef IRC_DEBUG
        OS_TPrintf("�R�[���o�b�N��: SHUTDOWN COMMAND ACCEPT\n");
#endif
        cnt_userdata = 0;
        break;
    }
}

/*---------------------------------------------------------------------------*
  Name:         GetPadDown

  Description:  �����^�C�~���O�̃L�[���͂��擾.

  Arguments:    None.

  Returns:      �O��Ăяo�����獡��Ăяo���܂ł̊Ԃɉ������ꂽ�L�[�̃t���O.
 *---------------------------------------------------------------------------*/
static u32 GetPadDown(void)
{
    static u32 btn = 0;
    static u32 btn_bak = 0;

    btn_bak = btn;
    btn = PAD_Read();
    return (u32)(~btn_bak & btn);
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V �u�����N�R�[���o�b�N.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void VBlankIntr(void)
{
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

/*---------------------------------------------------------------------------*
  Name:         InitApp

  Description:  OS ��, �A�v���P�[�V������{�����̏�����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void InitApp(void)
{
    void   *tmp;
    OSHeapHandle hh;

    /* OS �̏����� */
    OS_Init();
    OS_InitThread();
    OS_InitTick();
    /* �A���[�i�̏����� */
    tmp = OS_InitAlloc(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1);
    OS_SetArenaLo(OS_ARENA_MAIN, tmp);
    hh = OS_CreateHeap(OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi());
    if (hh < 0)
    {
        OS_TPanic("ARM9: Fail to create heap...\n");
    }
    hh = OS_SetCurrentHeap(OS_ARENA_MAIN, hh);
    /* ���荞�݂�L���� */
    OS_SetIrqFunction(OS_IE_V_BLANK, VBlankIntr);
    (void)OS_EnableIrqMask(OS_IE_V_BLANK);
    (void)OS_EnableIrq();
}

/*---------------------------------------------------------------------------*
  Name:         GetCardResultString

  Description:  CARD �֐��̌��ʒl�𕶎���Ŏ擾.

  Arguments:    result : CARDResult ���ʒl.

  Returns:      ���ʒl��������镶����ւ̃|�C���^.
 *---------------------------------------------------------------------------*/
static const char *GetCardResultString(CARDResult result)
{
    switch (result)
    {
    case CARD_RESULT_SUCCESS:
        return "success";
    case CARD_RESULT_FAILURE:
        return "failure";
    case CARD_RESULT_INVALID_PARAM:
        return "invalid param";
    case CARD_RESULT_UNSUPPORTED:
        return "unsupported";
    case CARD_RESULT_TIMEOUT:
        return "timeout";
    case CARD_RESULT_CANCELED:
        return "canceled";
    case CARD_RESULT_NO_RESPONSE:
        return "no response";
    case CARD_RESULT_ERROR:
        return "error";
    default:
        return "unknown error";
    }
}

/*---------------------------------------------------------------------------*
  Name:         SelectDevice

  Description:  �f�o�C�X�I�����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static u8 idxCursor;

static BOOL isSender; // �ŏ��ɐԊO�����o����
static void IRCTest(void)
{
    BOOL end = FALSE;

    cnt_success = 0;
    IRC_Init(MY_UNIT_NUMBER);                // �������ƃ��j�b�g�ԍ��̃Z�b�g
    IRC_SetConnectCallback(connectCallback); // �ڑ��R�[���o�b�N���Z�b�g
    IRC_SetRecvCallback(recvCallback);       // �f�[�^��M�R�[���o�b�N���Z�b�g
    isSender = FALSE;
    while (!end)
    {
        u32     btn_down = GetPadDown();

        DrawBegin();
#ifdef IRC_DEBUG
        //�G���[���̕\��
        {
            u32 rate; 
            u32 cnt_retry;

            cnt_retry = IRC_GetRetryCount();
            rate = 10000*cnt_retry / (cnt_success+cnt_retry);
            FillRect(0, 8*3, 8*24, 8, RGB555_BLACK);
            DrawText(0, 8*3, "% 8d/% 8d % 3d.%02d%%", cnt_retry, cnt_success+cnt_retry, rate/100, rate%100);
        }

        FillRect(8*(31-5), 0, 8*2, 8, RGB555_BLACK);
        if (IRC_IsConnect())
        {
            DrawText(8*(31-5), 0, "C");
        }
        //if (flg_restore){ DrawText(8*(31-4), 0, "R"); }

        FillRect(8*(31-2), 0, 8*3, 8, RGB555_BLACK);
        {
            int i;
            for (i = 0; i < (idxCursor>>4); i++)
            {
                DrawText(8*(31-2+i), 0, "*");
            }
        }
        idxCursor = (u8)((idxCursor+1)&63);

        FillRect(0, 0, 8*3, 8, RGB555_BLACK);
        if (IRC_IsSender())
        {
            DrawText(0, 0, "SEND");
        }
        else
        {
            DrawText(0, 0, "RECV");
        }
        FillRect(0, 8*2, 8*12, 8, RGB555_BLACK);
        DrawText(0, 8*2, "UNIT ID %02X-%02X", MY_UNIT_NUMBER, IRC_GetUnitNumber()); // �����ID
#endif
        IRC_Move();

        if ((btn_down & PAD_BUTTON_A) || (isSender == TRUE && IRC_IsConnect() == FALSE)) // �{�^�������������A�Z���_�[�����ؒf�����ꍇ
        {
            cnt_userdata = 0;
            IRC_Connect();
            isSender = TRUE;
            ClearFrame(RGB555_BLACK);
            DrawText(32, 0, "REQ>");
        }
        if (btn_down & PAD_BUTTON_B)
        {
            isSender = FALSE;
            IRC_Shutdown(); 
            cnt_userdata = 0;
            end = TRUE; 
        }
        DrawEnd();
    }
}

static void SelectDevice(void)
{
    static const struct
    {
        CARDBackupType type;
        const char *comment;
    }
    types_table[] =
    {
        { CARD_BACKUP_TYPE_EEPROM_4KBITS_IRC,    "EEPROM    4 kb"},
        { CARD_BACKUP_TYPE_EEPROM_64KBITS_IRC,   "EEPROM   64 kb"},
        { CARD_BACKUP_TYPE_EEPROM_512KBITS_IRC,  "EEPROM  512 kb"},
        { CARD_BACKUP_TYPE_FLASH_2MBITS_IRC,     "FLASH     2 Mb"},
        { CARD_BACKUP_TYPE_FLASH_4MBITS_IRC,     "FLASH     4 Mb"},
        { CARD_BACKUP_TYPE_FLASH_8MBITS_IRC,     "FLASH     8 Mb"},
    };

    enum
    {
        types_table_max = sizeof(types_table) / sizeof(*types_table)
    };

    int     cur = 0;
    BOOL    error = FALSE;
    BOOL    end = FALSE;
    int     i;

    while (!end)
    {

        u32     btn_down = GetPadDown();

        DrawBegin();
        ClearFrame(RGB555_BLACK);
        
        /* �㉺�L�[�ŃJ�[�\���ړ� */
        if (btn_down & PAD_KEY_DOWN)
        {
            error = FALSE;
            if (++cur >= types_table_max)
            {
                cur -= types_table_max;
            }
        }
        if (btn_down & PAD_KEY_UP)
        {
            error = FALSE;
            if (--cur < 0)
            {
                cur += types_table_max;
            }
        }

        /*
         * A �{�^���Ō��݂̑I���f�o�C�X�ɓ���.
         *
         * ���C�u�����͎w��̃f�o�C�X�����ۂɓ��ڂ���Ă���̂���
         * ���f�ł��܂���̂�, �������w�肵�Ă�������.
         */
        if (btn_down & PAD_BUTTON_A)
        {
            CARD_LockBackup((u16)card_lock_id);
            end = CARD_IdentifyBackup(types_table[cur].type);
            if (!end)
            {
                error = TRUE;
                last_result = CARD_GetResultCode();
            }
            CARD_UnlockBackup(card_lock_id);
        }

        /* ������\�� */
        SetTextColor(RGB555_GREEN);
        DrawText(10, 40, "select device!");
        for (i = 0; i < types_table_max; ++i)
        {
            DrawText(20, 60 + 10 * i, types_table[i].comment);
        }
        FillRect(10, 60, 10, i * 10, RGB555_BLACK);
        DrawText(10, 60 + 10 * cur, ">");
        if (error)
        {
            /* �G���[�\�� */
            SetTextColor(RGB555_RED);
            DrawText(10, 120, "error!");
            DrawText(10, 130, "result:\"%s\"", GetCardResultString(last_result));
        }
        DrawEnd();
    }
}

static void test(void)
{
    static int cur = 0;
    u32 btn_down = GetPadDown();

    DrawBegin();
    ClearFrame(RGB555_BLACK);

    /* �㉺�L�[�ŃJ�[�\���ړ� */
    if (btn_down & PAD_KEY_DOWN)
    {
        if (++cur >= 2)
        {
            cur -= 2;
        }
    }
    if (btn_down & PAD_KEY_UP)
    {
        if (--cur < 0)
        {
            cur += 2;
        }
    }
    if (btn_down & PAD_BUTTON_A)
    {
        if(cur == 0)
        {
            IRCTest();
        }
        else
        {
            /* �f�o�C�X�I����� */
            SelectDevice();
            /* �e�X�g�J�n */
            is_test_run = FALSE;
            TestWriteAndVerify();
            /* ���ʕ\�� */
            if (is_test_run)
            {
                ShowResult();
            }
        }
    }

    SetTextColor(RGB555_GREEN);
    DrawText(10, 40, "select device!");
    DrawText(20, 60 + 10 * 0, "IR");
    DrawText(20, 60 + 10 * 1, "BACKUP");

    FillRect(10, 60, 10, 10 * 2, RGB555_BLACK);
    DrawText(10, 60 + 10 * cur, ">");

    DrawEnd();
}
/*---------------------------------------------------------------------------*
  Name:         TestWriteAndVerify

  Description:  �������݃e�X�g���.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void TestWriteAndVerify(void)
{
    const u32 page_size  = CARD_GetBackupPageSize();
    const u32 total_size = CARD_GetBackupTotalSize();

    u32     pos = 0;
    BOOL    end = FALSE;

    /* �e�����\�� */
    DrawBegin();
    ClearFrame(RGB555_BLACK);
    SetTextColor(RGB555_WHITE);
    if (CARD_IsBackupEeprom())
    {
        DrawText(10, 10, "EEPROM");
    }
    else if (CARD_IsBackupFlash())
    {
        DrawText(10, 10, "FLASH");
    }
    else if (CARD_IsBackupFram())
    {
        DrawText(10, 10, "FRAM");
    }
    DrawText(10, 20, "page:%d  total:%d", page_size, total_size);
    FillRect(10, 40, 256, 100, RGB555_BLACK);
    SetTextColor(RGB555_BLUE);
    MoveTo(0 + 8, 35);
    LineTo(256 - 8, 35);
    LineTo(256 - 8, 182 - 8);
    LineTo(0 + 8, 182 - 8);
    LineTo(0 + 8, 35);
    DrawEnd();

    while (!end)
    {
        int mode = 0;
        u32 btn_down = GetPadDown();
        if (btn_down & PAD_KEY_LEFT)
        {
            mode = (mode+(3-1))%3;
        }
        if (btn_down & PAD_KEY_RIGHT)
        {
            mode = (mode+1)%3;
        }
        DrawBegin();

        /* �J�n�O�Ȃ� A �{�^���ŊJ�n */
        if (!is_test_run)
        {
            if (btn_down & PAD_BUTTON_A)
            {
                is_test_run = TRUE;
            }
            SetTextColor(RGB555_WHITE);
            DrawText(10, 40, "press A-BUTTON to test");
        }

        /* B �{�^���Ńe�X�g���~ */
        if (btn_down & PAD_BUTTON_B)
        {
            end = TRUE;
        }

        /* �e�X�g�� */
        if (is_test_run)
        {

            u8      tmp_buf[256];

            FillRect(10, 40, 256 - 10 * 2, 100, RGB555_BLACK);

            SetTextColor(RGB555_GREEN);
            DrawText(10, 40, "now testing...");
            DrawText(10, 50, "address:%d-%d", pos, pos + sizeof(tmp_buf));

            /*
             * �ȍ~�̃o�b�N�A�b�v�A�N�Z�X�̂��߂� CARD ���\�[�X�����b�N����.
             *
             * ������ CARD_UnlockBackup() ���ĂԂ܂ł̊�
             * FS �ȂǑ��̃��W���[������� CARD �A�N�Z�X��
             * �u���b�L���O����邱�ƂɂȂ�܂�.
             * �����̃f�b�h���b�N�ɒ��ӂ��Ă�������.
             */
            CARD_LockBackup((u16)card_lock_id);
            {
                OSTick  tick;

                int     i;

                for (i = 0; i < sizeof(tmp_buf); ++i)
                {
                    tmp_buf[i] = (u8)(pos * 3 + i);
                }
                tick = OS_GetTick();

                /* �񓯊��ŏ������݁A���e�m�F���� */
                if (CARD_IsBackupEeprom())
                {
                    CARD_WriteAndVerifyEepromAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }
                else if (CARD_IsBackupFlash())
                {
                    CARD_WriteAndVerifyFlashAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }
                else if (CARD_IsBackupFram())
                {
                    CARD_WriteAndVerifyFramAsync(pos, tmp_buf, sizeof(tmp_buf), NULL, (void *)0);
                }

                /*
                 * �����҂�.
                 *
                 * �f�o�C�X��ނƎw��T�C�Y�ɂ���Ă�
                 * ���ɒ������ԃu���b�L���O����܂��̂�,
                 * �e���[�U�A�v���P�[�V�����̃t���[�����[�N�ɍ����`��
                 * �҂������H�v���Ă�������.
                 * (1 �t���[���� 1 �� CARD_TryWaitBackupAsync() �Ŕ��肷��, ��)
                 */
                (void)CARD_WaitBackupAsync();
                last_result = CARD_GetResultCode();

                if (last_result != CARD_RESULT_SUCCESS)
                {
                    /* �G���[�Ȃ�I�� */
                    end = TRUE;
                }
                else
                {
                    /* �����Ȃ玞�Ԃ�\�� */
                    tick = OS_GetTick() - tick;
                    DrawText(10, 60, "%6d[ms]", OS_TicksToMilliSeconds(tick));
                    /* �e�X�g�A�h���X�����ֈړ� */
                    pos += sizeof(tmp_buf);
                    if (pos + sizeof(tmp_buf) > total_size)
                    {
                        end = TRUE;
                    }
                }
            }
            CARD_UnlockBackup(card_lock_id);
        }
        DrawEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ShowResult

  Description:  �e�X�g���ʕ\�����.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShowResult(void)
{

    BOOL    end = FALSE;

    while (!end)
    {
        u32     btn_down = GetPadDown();

        DrawBegin();

        /* A / B / START �{�^���Ŗ߂� */
        if (btn_down & (PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_START))
        {
            end = TRUE;
        }

        if (last_result == CARD_RESULT_SUCCESS)
        {
            /* �����\�� */
            SetTextColor(RGB555_WHITE);
            DrawText(10, 70, "done! (success)");
        }
        else
        {
            /* �G���[�\�� */
            SetTextColor(RGB555_RED);
            DrawText(10, 70, "error!");
            DrawText(10, 80, "result:\"%s\"", GetCardResultString(last_result));
        }
        DrawEnd();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  ���C�� �G���g���|�C���g.

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{

    {
        /* �T���v���A�v���P�[�V�����̏����� */
        InitApp();
        InitDraw();
        SetClearColor(RGB555_CLEAR);
    }

    {
        /* CARD ���C�u�����̏����� */
        s32     ret;

        CARD_Init();
        ret = OS_GetLockID();
        if (ret == OS_LOCK_ID_ERROR)
        {
            OS_TPanic("demo fatal error! OS_GetLockID() failed");
        }
        card_lock_id = (u16)ret;
    }
    init_data();
    /* ��ʑJ�� */
    for (;;)
    {
        test();
    }

}
