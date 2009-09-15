/*---------------------------------------------------------------------------*
  Project:  TwlSDK - demos - CARD - backup
  File:     main.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-27#$
  $Rev: 9430 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/


#include <nitro.h>

#include "DEMO.h"


/*---------------------------------------------------------------------------*/
/* variables */

// CARD �o�b�N�A�b�v�̃��b�N�p ID�B
// CARD �n�[�h�E�F�A���\�[�X�� FS ���C�u�����ȂǂƔr���I�Ɏg�p���邽��
// CARD_LockBackup() / CARD_UnlockBackup() �Ŏg�p���܂��B
static u16  card_lock_id;

// CARD �A�N�Z�X�֐��̃G���[���̌��ʒl�B
// CARD_GetResultCode() �̕Ԃ�l�� CARD_UnlockBackup() ���ŕύX�����̂�
// �A�N�Z�X�����ƃG���[����������Ă���ꍇ�ɂ͂��̓_�ɒ��ӂ��Ă��������B
static CARDResult last_result = CARD_RESULT_SUCCESS;

// �������݃e�X�g�����s���ꂽ���̃t���O�B
static BOOL is_test_run;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         GetCardResultString

  Description:  CARD �֐��̌��ʒl�𕶎���Ŏ擾

  Arguments:    result           CARDResult ���ʒl

  Returns:      ���ʒl��������镶����ւ̃|�C���^
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

  Description:  �f�o�C�X�I�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void SelectDevice(void)
{
    /* *INDENT-OFF* */
    static const struct
    {
        CARDBackupType type;
        const char *comment;
    }
    types_table[] =
    {
        { CARD_BACKUP_TYPE_EEPROM_4KBITS,   "EEPROM    4 kb"},
        { CARD_BACKUP_TYPE_EEPROM_64KBITS,  "EEPROM   64 kb"},
        { CARD_BACKUP_TYPE_EEPROM_512KBITS, "EEPROM  512 kb"},
        { CARD_BACKUP_TYPE_EEPROM_1MBITS,   "EEPROM    1 Mb"},
        { CARD_BACKUP_TYPE_FLASH_2MBITS,    "FLASH     2 Mb"},
        { CARD_BACKUP_TYPE_FLASH_4MBITS,    "FLASH     4 Mb"},
        { CARD_BACKUP_TYPE_FLASH_8MBITS,    "FLASH     8 Mb"},
        { CARD_BACKUP_TYPE_FLASH_16MBITS,   "FLASH    16 Mb"},
        { CARD_BACKUP_TYPE_FLASH_64MBITS,   "FLASH    64 Mb"},
    };
    /* *INDENT-ON* */
    enum
    { types_table_max = sizeof(types_table) / sizeof(*types_table) };

    int     cur = 0;
    BOOL    error = FALSE;
    BOOL    end = FALSE;
    int     i;

    while (!end)
    {
        DEMOReadKey();
        // �㉺�L�[�ŃJ�[�\���ړ�
        if (DEMO_IS_TRIG(PAD_KEY_DOWN))
        {
            error = FALSE;
            if (++cur >= types_table_max)
            {
                cur -= types_table_max;
            }
        }
        if (DEMO_IS_TRIG(PAD_KEY_UP))
        {
            error = FALSE;
            if (--cur < 0)
            {
                cur += types_table_max;
            }
        }

        // A �{�^���Ō��݂̑I���f�o�C�X�ɓ���B
        // ���C�u�����͎w��̃f�o�C�X�����ۂɓ��ڂ���Ă��邩
        // ���f�ł��܂���̂�, �������w�肵�Ă��������B
        if (DEMO_IS_TRIG(PAD_BUTTON_A))
        {
            CARD_LockBackup(card_lock_id);
            end = CARD_IdentifyBackup(types_table[cur].type);
            if (!end)
            {
                error = TRUE;
                last_result = CARD_GetResultCode();
            }
            CARD_UnlockBackup(card_lock_id);
        }

        // ��ʕ\��
        DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
        DEMOSetBitmapTextColor(GX_RGBA(0, 31, 0, 1));
        DEMODrawText(10, 40, "select device!");
        for (i = 0; i < types_table_max; ++i)
        {
            DEMODrawText(10, 60 + 10 * i, "%c%s",
                         (cur == i) ? '>' : ' ', types_table[i].comment);
        }
        if (error)
        {
            DEMOSetBitmapTextColor(GX_RGBA(31, 0, 0, 1));
            DEMODrawText(10, 160, "error!");
            DEMODrawText(10, 170, "result:\"%s\"", GetCardResultString(last_result));
        }

        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         TestWriteAndVerify

  Description:  �������݃e�X�g���

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void TestWriteAndVerify(void)
{
    const u32   page_size = CARD_GetBackupPageSize();
    const u32   sector_size = CARD_GetBackupSectorSize();
    const u32   total_size = CARD_GetBackupTotalSize();

    OSTick      erase_time = 0;
    u32         pos = 0;

    BOOL        end = FALSE;

    // ��ʂ̏�����
    {
        DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
        DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
        if (CARD_IsBackupEeprom())
        {
            DEMODrawText(10, 10, "EEPROM");
        }
        else if (CARD_IsBackupFlash())
        {
            DEMODrawText(10, 10, "FLASH");
        }
        DEMODrawText(10, 20, "page:%d  sector:%d", page_size, sector_size);
        DEMODrawText(10, 30, "total:%d", total_size);
        DEMOFillRect(10, 50, GX_LCD_SIZE_X - 10 * 2, 100, GX_RGBA(0, 0, 0, 1));
        DEMODrawFrame(8, 45, GX_LCD_SIZE_X - 8 * 2, 182 - 8 - 45, GX_RGBA(0, 0, 31, 1));
        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }

    while (!end)
    {
        DEMOReadKey();
        // �J�n�O�Ȃ�A�{�^���ŊJ�n
        if (!is_test_run)
        {
            if (DEMO_IS_TRIG(PAD_BUTTON_A))
            {
                is_test_run = TRUE;
            }
        }
        //B�{�^���Ńe�X�g���~
        else if (DEMO_IS_TRIG(PAD_BUTTON_B))
        {
            end = TRUE;
        }

        // �������݃e�X�g�Ɖ�ʕ\��
        if (!is_test_run)
        {
            DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
            DEMODrawText(10, 50, "press A-BUTTON to test");
        }
        else
        {
            static u8 tmp_buf[65536];
            SDK_ASSERT(sizeof(tmp_buf) >= sector_size);

            DEMOFillRect(10, 50, 256 - 10 * 2, 100, GX_RGBA(0, 0, 0, 1));
            DEMOSetBitmapTextColor(GX_RGBA(0, 31, 0, 1));
            DEMODrawText(10, 50, "now testing...");
            DEMODrawText(10, 60, "address:%d-%d", pos, pos + page_size);

            // �ȍ~�̃o�b�N�A�b�v�A�N�Z�X�̂��߂� CARD ���\�[�X�����b�N���܂��B
            // ������ CARD_UnlockBackup() ���ĂԂ܂ł̊Ԃ�
            // FS �ȂǑ��̃��W���[������� CARD �A�N�Z�X�̓u���b�L���O����܂��B
            // �����̃f�b�h���b�N�ɒ��ӂ��Ă��������B
            CARD_LockBackup(card_lock_id);
            {
                OSTick  tick;
                int     i;
                for (i = 0; i < page_size; ++i)
                {
                    tmp_buf[i] = (u8)(pos * 3 + i);
                }
                tick = OS_GetTick();
                // �񓯊��ŏ������� + ���e�m�F����
                if (CARD_IsBackupEeprom())
                {
                    CARD_WriteAndVerifyEepromAsync(pos, tmp_buf, page_size, NULL, NULL);
                }
                else if (CARD_IsBackupFlash())
                {
                    // �قƂ�ǂ�FLASH�f�o�C�X��Write������g�p�\�ł��B
                    if (CARD_GetCurrentBackupType() != CARD_BACKUP_TYPE_FLASH_64MBITS)
                    {
                        CARD_WriteAndVerifyFlashAsync(pos, tmp_buf, page_size, NULL, NULL);
                    }
                    // �ꕔ�̑�e��FLASH��EraseSector�����Program����̂ݎg�p�\�ł��B
                    // ��������̓Z�N�^�T�C�Y�̐����{�P�ʂ݂̂Ƃ����_�ɒ��ӂ��Ă��������B
                    else
                    {
                        BOOL    programmable = FALSE;
                        if ((pos % sector_size) == 0)
                        {
                            // ���ꂩ�珑�����ޗ̈�͎��O�ɃZ�N�^���Ə�������K�v������܂��B
                            // ���̃T���v���f���ł̓Z�N�^�S�̂̓��e�����������Ă��܂����A
                            // �Z�N�^�̈ꕔ���݂̂��������������ꍇ�ɂ͏����O�̓��e��ޔ�����
                            // ������ɍĂѓ������e���������݂Ȃ����K�v������܂��B
                            erase_time = tick;
                            programmable = CARD_EraseFlashSector(pos, sector_size);
                            tick = OS_GetTick();
                            erase_time = tick - erase_time;
                            last_result = CARD_GetResultCode();
                        }
                        else
                        {
                            // ���łɏ����ς݂̗̈�͂��̂܂܏������݉\�ł��B
                            programmable = TRUE;
                        }
                        if (programmable)
                        {
                            CARD_ProgramAndVerifyFlashAsync(pos, tmp_buf, page_size, NULL, NULL);
                        }
                    }
                }

                // ���̃T���v���ł́A���̏�ŏ���������҂��Č��ʂ��m�F���܂��B
                // �f�o�C�X��ނƎw��T�C�Y�ɂ���Ă͔��ɒ������ԃu���b�L���O����܂��̂�
                // �e���[�U�A�v���P�[�V�����̃t���[�����[�N�ɍ����`�ő҂������H�v���Ă��������B
                // (�Ⴆ�΁A1�t���[����1�񂾂�CARD_TryWaitBackupAsync()�Ŋm�F����Ȃ�)
                (void)CARD_WaitBackupAsync();
                last_result = CARD_GetResultCode();
                if (last_result != CARD_RESULT_SUCCESS)
                {
                    // �G���[�Ȃ�ЂƂ܂��I��
                    end = TRUE;
                }
                else
                {
                    // �����Ȃ玞�Ԃ�\��
                    tick = OS_GetTick() - tick;
                    DEMODrawText(10, 70, "write:%6d[ms]/%d[BYTE]", (int)OS_TicksToMilliSeconds(tick), page_size);
                    if (erase_time != 0)
                    {
                        DEMODrawText(10, 80, "erase:%6d[ms]/%d[BYTE]", (int)OS_TicksToMilliSeconds(erase_time), sector_size);
                    }
                    // �e�X�g�A�h���X�����ֈړ�
                    pos += page_size;
                    if (pos + page_size > total_size)
                    {
                        end = TRUE;
                    }
                }
            }
            CARD_UnlockBackup(card_lock_id);
        }

        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         ShowResult

  Description:  �e�X�g���ʕ\�����

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void ShowResult(void)
{
    BOOL    end = FALSE;

    while (!end)
    {
        DEMOReadKey();
        // A / B / START �{�^���Ŗ߂�
        if (DEMO_IS_TRIG(PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_START))
        {
            end = TRUE;
        }

        // ��ʕ\��
        if (last_result == CARD_RESULT_SUCCESS)
        {
            DEMOSetBitmapTextColor(GX_RGBA(31, 31, 31, 1));
            DEMODrawText(10, 100, "done! (success)");
        }
        else
        {
            DEMOSetBitmapTextColor(GX_RGBA(0, 31, 0, 1));
            DEMODrawText(10, 100, "error!");
            DEMODrawText(10, 110, "result:\"%s\"", GetCardResultString(last_result));
        }
        DEMO_DrawFlip();
        OS_WaitVBlankIntr();
    }
}

/*---------------------------------------------------------------------------*
  Name:         NitroMain

  Description:  ���C�� �G���g���|�C���g

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void NitroMain(void)
{
    // SDK�̏�����
    OS_Init();
    OS_InitTick();
    (void)OS_EnableIrq();
    (void)OS_EnableInterrupts();

    // �f���\���̏�����
    DEMOInitCommon();
    DEMOInitVRAM();
    DEMOInitDisplayBitmap();
    DEMOHookConsole();
    DEMOSetBitmapTextColor(GX_RGBA(31, 31, 0, 1));
    DEMOSetBitmapGroundColor(DEMO_RGB_CLEAR);
    DEMOStartDisplay();

    // �J�[�h�A�v���ȊO��ROM��o�b�N�A�b�v�ɃA�N�Z�X����ꍇ�A
    // �}������Ă���DS�J�[�h�����Ѓ^�C�g���ł��邩���肵�����
    // �����I��CARD_Enable�֐����Ăяo���ėL��������K�v������܂��B
    // �ڍׂ̓v���O���~���O�K�C�h���C�������Q�Ƃ��������B
    if (OS_GetBootType() != OS_BOOTTYPE_ROM)
    {
        const CARDRomHeader *own_header = (const CARDRomHeader *)HW_ROM_HEADER_BUF;
        const CARDRomHeader *rom_header = (const CARDRomHeader *)CARD_GetRomHeader();
        if (own_header->maker_code != rom_header->maker_code)
        {
            // ���̃T���v���̓o�b�N�A�b�v�f�o�C�X�ɃA�N�Z�X���邱�Ƃ��ړI�Ȃ̂ŁA
            // DS�J�[�h���}������Ă��Ȃ��Ȃ炱���Œ�~���܂��B
            static const char *message = "cannot detect own-maker title DS-CARD!";
            DEMOFillRect(0, 0, GX_LCD_SIZE_X, GX_LCD_SIZE_Y, DEMO_RGB_CLEAR);
            DEMOSetBitmapTextColor(GX_RGBA(31, 0, 0, 1));
            DEMODrawText(10, 40, message);
            DEMO_DrawFlip();
            OS_WaitVBlankIntr();
            OS_TPanic(message);
        }
        else
        {
            CARD_Enable(TRUE);
        }
    }

    {
        // CARD���C�u�����̃f�[�^�o�X�����b�N���邽�߂�ID���m�ہB
        s32     ret = OS_GetLockID();
        if (ret == OS_LOCK_ID_ERROR)
        {
            OS_TPanic("demo fatal error! OS_GetLockID() failed");
        }
        card_lock_id = (u16)ret;
    }

    // ��ʑJ��
    for (;;)
    {
        // �f�o�C�X�I�����
        SelectDevice();
        // �e�X�g�J�n
        is_test_run = FALSE;
        TestWriteAndVerify();
        // ���ʕ\��
        if (is_test_run)
        {
            ShowResult();
        }
    }

}
