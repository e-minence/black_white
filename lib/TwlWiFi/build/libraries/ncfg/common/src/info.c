/*---------------------------------------------------------------------------*
  Project:  TwlWiFi - NCFG - libraries
  File:     info.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 1024 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/
#include <nitroWiFi/ncfg.h>

/*---------------------------------------------------------------------------*
    �萔��`
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
    �����֐��v���g�^�C�v
 *---------------------------------------------------------------------------*/
static s32 checkConfig(NCFGConfig* config, const MATHCRC16Table *tbl);
static s32 checkConfigEx(NCFGConfigEx* configEx, const MATHCRC16Table *tbl);
static int init(NCFGConfig* config);
static void initPage(NCFGConfig* config, int page);
static void initPageEx(NCFGConfigEx* configEx, int page);
static BOOL readAllSlots(NCFGConfig* config);
static BOOL readAllExSlots(NCFGConfigEx* configEx);
static BOOL writeAllSlots(NCFGConfig* config, const MATHCRC16Table *tbl);
static BOOL writeAllExSlots(NCFGConfigEx* configEx, const MATHCRC16Table *tbl);

/*---------------------------------------------------------------------------*
  Name:         NCFG_ReadConfig

  Description:  �l�b�g���[�N�ݒ���擾����B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    config - �f�[�^�i�[�o�b�t�@
                work   - ���[�N������

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_SUCCESS, NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 NCFG_ReadConfig(NCFGConfig* config, void* work)
{
    (void)work;
    return readAllSlots(config) ? NCFG_RESULT_SUCCESS : NCFG_RESULT_FAILURE;
}

/*---------------------------------------------------------------------------*
  Name:         NCFG_WriteConfig

  Description:  �l�b�g���[�N�ݒ��ݒ肷��B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    config - �f�[�^�i�[�o�b�t�@
                work   - ���[�N������

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_SUCCESS, NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
typedef struct MEMWRITE {
    NCFGConfig        config;          // �����o�b�N�A�b�v�������}�b�v�o�b�t�@
    MATHCRC16Table    tbl;             // CRC �v�Z�e�[�u��
}  MEMWRITE;

s32 NCFG_WriteConfig(const NCFGConfig* config, void* work)
{
    MEMWRITE* wk = work;

    SDK_ASSERT(wk);

    MI_CpuCopy8(config, &wk->config, sizeof(NCFGConfig));
    MATH_CRC16InitTable(&wk->tbl);

    return writeAllSlots(&wk->config, &wk->tbl) ? NCFG_RESULT_SUCCESS : NCFG_RESULT_FAILURE;
}

/*---------------------------------------------------------------------------*
  Name:         NCFG_ReadConfigEx

  Description:  �g���l�b�g���[�N�ݒ���擾����B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    configEx - �f�[�^�i�[�o�b�t�@
                work   - ���[�N������

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_SUCCESS, NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
s32 NCFG_ReadConfigEx(NCFGConfigEx* configEx, void* work)
{
    BOOL result;

    result = (NCFG_ReadConfig(&configEx->compat, work) == NCFG_RESULT_SUCCESS);

    if ( NCFG_GetFormatVersion() >= NCFG_FORMAT_VERSION_TWL )
    {
        result = result && readAllExSlots(configEx);
    }
    else
    {
        result = FALSE;
    }

    return result ? NCFG_RESULT_SUCCESS : NCFG_RESULT_FAILURE;
}

/*---------------------------------------------------------------------------*
  Name:         NCFG_WriteConfigEx

  Description:  �g���l�b�g���[�N�ݒ��ݒ肷��B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    configEx - �f�[�^�i�[�o�b�t�@
                work   - ���[�N������

  Returns:      s32     -   �ȉ��̓��̂����ꂩ�̏������ʂ��Ԃ����B
                            NCFG_RESULT_SUCCESS, NCFG_RESULT_FAILURE
 *---------------------------------------------------------------------------*/
typedef struct MEMWRITEEX {
    NCFGConfigEx      configEx;        // �����o�b�N�A�b�v�������}�b�v�o�b�t�@
    MATHCRC16Table    tbl;             // CRC �v�Z�e�[�u��
}  MEMWRITEEX;

s32 NCFG_WriteConfigEx(const NCFGConfigEx* configEx, void* work)
{
    MEMWRITEEX* wk = work;
    BOOL result;

    SDK_ASSERT(wk);

    result = (NCFG_WriteConfig(&configEx->compat, work) == NCFG_RESULT_SUCCESS);

    MI_CpuCopy8(configEx, &wk->configEx, sizeof(NCFGConfigEx));
    MATH_CRC16InitTable(&wk->tbl);

    if ( NCFG_GetFormatVersion() >= NCFG_FORMAT_VERSION_TWL )
    {
        result = result && writeAllExSlots(&wk->configEx, &wk->tbl);
    }
    else
    {
        result = FALSE;
    }

    return result ? NCFG_RESULT_SUCCESS : NCFG_RESULT_FAILURE;
}

/*---------------------------------------------------------------------------*
  Name:         NCFG_CheckConfig

  Description:  �l�b�g���[�N�ݒ�̈�ѐ����m�F���A�C������B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    work   - ���[�N������

  Returns:      s32   -      0 : ����I��
                        -10002 : �ڑ���ݒ肪��������������I��
                        -10003 : ���[�U ID ����������������I��
                        -10000 : �����o�b�N�A�b�v�������̏������݃G���[�I��
                        -10001 : �����o�b�N�A�b�v�������̓ǂݍ��݃G���[�I��
 *---------------------------------------------------------------------------*/
// �`�F�b�N�֐��g�p���[�N������
typedef struct MEMCHECK {
	NCFGConfig        config;          // �����o�b�N�A�b�v�������}�b�v�o�b�t�@
	MATHCRC16Table    tbl;             // CRC �v�Z�e�[�u��
}  MEMCHECK;

s32 NCFG_CheckConfig(void* work)
{
	MEMCHECK* wk = work;

	SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );
	
	// ���[�N��������O�̂��߂Ƀ[���N���A����
	MI_CpuClear8(work, NCFG_CHECKCONFIG_WORK_SIZE);
	
	// �����o�b�N�A�b�v��������������
	if (NCFGi_InitBackupMemory() != NCFG_RESULT_SUCCESS)  return  NCFG_RESULT_INIT_ERROR_READ;

	// CRC ������
	MATH_CRC16InitTable(&wk->tbl);

	// �����o�b�N�A�b�v��������ǂݍ���
	if (! readAllSlots(&wk->config))    return  NCFG_RESULT_INIT_ERROR_READ;

    return checkConfig(&wk->config, &wk->tbl);
}

/*---------------------------------------------------------------------------*
  Name:         NCFG_CheckConfigEx

  Description:  �g���l�b�g���[�N�ݒ�̈�ѐ����m�F���A�C������B
                �� 32 �o�C�g�A���C������Ă���o�b�t�@���w�肵�Ă�������

  Arguments:    work   - ���[�N������

  Returns:      s32   -      0 : ����I��
                        -10002 : �ڑ���ݒ肪��������������I��
                        -10003 : ���[�U ID ����������������I��
                        -10000 : �����o�b�N�A�b�v�������̏������݃G���[�I��
                        -10001 : �����o�b�N�A�b�v�������̓ǂݍ��݃G���[�I��
 *---------------------------------------------------------------------------*/
// �`�F�b�N�֐��g�p���[�N������
typedef struct MEMCHECKEX {
    NCFGConfigEx      configEx;        // �����o�b�N�A�b�v�������}�b�v�o�b�t�@
    MATHCRC16Table    tbl;             // CRC �v�Z�e�[�u��
}  MEMCHECKEX;

s32 NCFG_CheckConfigEx(void* work)
{
    MEMCHECKEX* wk = work;

    SDK_ASSERTMSG( ( (int)work & 0x1f ) == 0, "work must be 32bytes alignment - %p\n", work );

    // ���[�N��������O�̂��߂Ƀ[���N���A����
    MI_CpuClear8(work, NCFG_CHECKCONFIGEX_WORK_SIZE);

    // �����o�b�N�A�b�v��������������
    if (NCFGi_InitBackupMemory() != NCFG_RESULT_SUCCESS)
    {
        return NCFG_RESULT_INIT_ERROR_READ;
    }

    // CRC ������
    MATH_CRC16InitTable(&wk->tbl);

    // �����o�b�N�A�b�v��������ǂݍ���
    if (! readAllSlots(&wk->configEx.compat))
    {
        return NCFG_RESULT_INIT_ERROR_READ;
    }
    if (! readAllExSlots(&wk->configEx))
    {
        return NCFG_RESULT_INIT_ERROR_READ;
    }

    return checkConfigEx(&wk->configEx, &wk->tbl);
}

static s32 checkConfig(NCFGConfig* config, const MATHCRC16Table *tbl)
{
	int       i;
	u16       hash;
	BOOL      c[4];
	BOOL      clear;
    u8        state;
    u8        prevState;

	// CRC ���`�F�b�N
	MI_CpuClear8(c, sizeof(c));
	for (i = 0; i < 3; ++ i) {
		hash = MATH_CalcCRC16(tbl, &config->slot[i], 0xFE);
		if ((hash == config->slot[i].crc) && NCFGi_CheckApInfo(&config->slot[i].ap)) {
			c[i] = TRUE;
		}
	}
	hash = MATH_CalcCRC16(tbl, &config->rsv, 0xFE);
	if (hash == config->rsv.crc)  c[3] = TRUE;

	// ����I��
	if (c[0] && c[1] && c[2] && c[3]) {
		return  NCFG_RESULT_INIT_OK;
	}

    prevState = config->slot[0].ap.state;

    // �o�b�N�A�b�v�������j��̂��߂̕�������
    if (! c[0])
    {
        initPage(config, 0);
        if (c[1])
        {
            prevState = config->slot[1].ap.state;
            MI_CpuCopy8(config->slot[1].wifi, config->slot[0].wifi, 0x0e);
        }
        else
        {
            // ���̃P�[�X�� NCFG_RESULT_INIT_OK_INIT ��Ԃ��K�v������̂ŕʂɃP�A����
            prevState = 0;
        }
    }
    if (! c[1])
    {
        initPage(config, 1);
        if (c[0])
        {
            MI_CpuCopy8(config->slot[0].wifi, config->slot[1].wifi, 0x0e);
        }
    }
    if (! c[2])
    {
        initPage(config, 2);
    }
    if (! c[3])
    {
        MI_CpuClear16(&config->rsv, sizeof(NCFGSlotInfo));
    }

    // �g�p�󋵃t���O�� ap.setType ���畜��
    clear = FALSE;
    state = 0;
    for (i = 0; i < 3; ++ i)
    {
        if (config->slot[i].ap.setType != NCFG_SETTYPE_NOT)
        {
            state |= (1 << i);
        }
        else
        {
            if (prevState & (1 << i))
            {
                // ���݂��Ă����ݒ�f�[�^��������
                clear = TRUE;
            }
        }
    }
    config->slot[0].ap.state = state;
    config->slot[1].ap.state = state;

    if (! writeAllSlots(config, tbl))
    {
        return NCFG_RESULT_INIT_ERROR_WRITE;
    }

    if ((! c[0]) && (! c[1]) && (! c[2]))
    {
        // �S�̈悪 CRC �G���[�������ꍇ��
        // ����N���ƌ��Ȃ��A�G���[��Ԃ��Ȃ�
        return NCFG_RESULT_INIT_OK;
    }
    else if ((! c[0]) && (! c[1]))
    {
        // WiFi ���[�U ID ��������
        // NCFG_RESULT_INIT_OK_INIT �� NCFG_RESULT_INIT_ERASE ���D�悷��
        return NCFG_RESULT_INIT_OK_INIT;
    }
    else
    {
        return (clear ? NCFG_RESULT_INIT_OK_ERASE : NCFG_RESULT_INIT_OK);
    }
}

static s32 checkConfigEx(NCFGConfigEx* configEx, const MATHCRC16Table *tbl)
{
    int       i;
    int       result;
    u16       hash;
    u16       hashEx;
    BOOL      c[3];
    BOOL      clear;
    u8        prevState;
    u8        state;

    // �݊��������`�F�b�N
    result = checkConfig(&configEx->compat, tbl);

    // CRC ���`�F�b�N
    MI_CpuClear8(c, sizeof(c));
    for (i = 0; i < 3; ++ i)
    {
        hash = MATH_CalcCRC16(tbl, &configEx->slotEx[i], 0xFE);
        hashEx = MATH_CalcCRC16(tbl, ((u8*)&configEx->slotEx[i]) + 0x100, 0xFE);
        if ((hash == configEx->slotEx[i].crc) && (hashEx == configEx->slotEx[i].crcEx)
            && NCFGi_CheckApInfoEx(&configEx->slotEx[i].ap, &configEx->slotEx[i].apEx))
        {
            c[i] = TRUE;
        }
    }

    // ����I��
    if (c[0] && c[1] && c[2])
    {
        return result; // �݊������̌��ʂ����̂܂ܕԓ�
    }

    prevState = configEx->slotEx[0].ap.state;

    // �o�b�N�A�b�v�������j��̂��߂̕�������
    if (! c[0])
    {
        initPageEx(configEx, 0);
        if (c[1])
        {
            prevState = configEx->slotEx[1].ap.state;
        }
        else
        {
            // �ݒ肪�S�����݂��Ă����Ɖ��肷��
            prevState = 0x07;
        }
    }
    if (! c[1])
    {
        initPageEx(configEx, 1);
    }
    if (! c[2])
    {
        initPageEx(configEx, 2);
    }

    // �g�p�󋵃t���O�� ap.setType ���畜��
    clear = FALSE;
    state = 0;
    for (i = 0; i < 3; ++ i)
    {
        if (configEx->slotEx[i].ap.setType != NCFG_SETTYPE_NOT)
        {
            state |= (1 << i);
        }
        else
        {
            if (prevState & (1 << i))
            {
                clear = TRUE;
            }
        }
    }
    configEx->slotEx[0].ap.state = state;
    configEx->slotEx[1].ap.state = state;

    if (! writeAllExSlots(configEx, tbl))
    {
        return NCFG_RESULT_INIT_ERROR_WRITE;
    }

    if ((! c[0]) && (! c[1]) && (! c[2]))
    {
        // �S�̈悪 CRC �G���[�������ꍇ��
        // ����N���ƌ��Ȃ��A�G���[�Ƃ��Ȃ�
        clear = FALSE;
    }

    if (result != NCFG_RESULT_INIT_OK)
    {
        // result �� NCFG_RESULT_INIT_OK_INIT �� NCFG_RESULT_INIT_ERASE
        // NCFG_RESULT_INIT_OK_INIT �� NCFG_RESULT_INIT_ERASE ���D�悷��
        return result;
    }
    else if (clear)
    {
        // �ݒ�ς݂̗̈悪�ǂ���������
        return NCFG_RESULT_INIT_OK_ERASE;
    }
    else
    {
        return NCFG_RESULT_INIT_OK;
    }
}

//-----------------------------------------------------------------------------
// �@�\ : ����������
// ���� : config - �����o�b�N�A�b�v������
// �Ԓl : int  - �G���[�R�[�h
//-----------------------------------------------------------------------------
static int  init(NCFGConfig* config)
{
	int          i;

	MI_CpuClear16(config, sizeof(NCFGConfig));
	for (i = 0; i < 3; ++ i) {
		config->slot[i].ap.setType = NCFG_SETTYPE_NOT;
	}

	return  0;
}

//-----------------------------------------------------------------------------
// �@�\ : 1 �y�[�W��������
// ���� : config - �����o�b�N�A�b�v������
//        page  - ����������y�[�W
// �Ԓl :
//-----------------------------------------------------------------------------
static void  initPage(NCFGConfig* config, int page)
{
	MI_CpuClear16(&config->slot[page], sizeof(NCFGSlotInfo));
	config->slot[page].ap.setType = NCFG_SETTYPE_NOT;
}

//-----------------------------------------------------------------------------
// �@�\ : �g���̈�� 1 �y�[�W��������
// ���� : config - �����o�b�N�A�b�v������
//        page  - ����������y�[�W
// �Ԓl :
//-----------------------------------------------------------------------------
static void initPageEx(NCFGConfigEx* configEx, int page)
{
    MI_CpuClear16(&configEx->slotEx[page], sizeof(NCFGSlotInfoEx));
    configEx->slotEx[page].ap.setType = NCFG_SETTYPE_NOT;
}

//*****************************************************************************
// �@�\ : �����o�b�N�A�b�v��������ǂݍ���
// ���� : mem   - �ǂݍ��ݐ�o�b�t�@  <<-- �Ԓl
// �Ԓl : BOOL  - TRUE  : ���� / FALSE : ���s
//*****************************************************************************
static BOOL readAllSlots(NCFGConfig* config)
{
    int i;
    BOOL result = TRUE;

    for (i = 0; i < 3; i++)
    {
        result = result &&
            (NCFG_ReadBackupMemory(&config->slot[i], sizeof(config->slot[i]), NCFG_SLOT_1+i) >= 0);
    }
    result = result &&
        (NCFG_ReadBackupMemory(&config->rsv, sizeof(config->rsv), NCFG_SLOT_RESERVED) >= 0);
    return result;
}

static BOOL readAllExSlots(NCFGConfigEx* configEx)
{
    int i;
    BOOL result = TRUE;

    for (i = 0; i < 3; i++)
    {
        result = result &&
            (NCFG_ReadBackupMemory(&configEx->slotEx[i], sizeof(configEx->slotEx[i]), NCFG_SLOT_EX_1+i) >= 0);
    }
    return result;
}

//*****************************************************************************
// �@�\ : �����o�b�N�A�b�v�������ɏ�������
// ���� : work  - �������֐��g�p���[�N������
// �Ԓl : BOOL  - TRUE  : ���� / FALSE : ���s
//*****************************************************************************
static BOOL writeAllSlots(NCFGConfig* config, const MATHCRC16Table *tbl)
{
    int  i;
    BOOL result = TRUE;

    for (i = 0; i < 3; i++)
    {
        config->slot[i].crc = MATH_CalcCRC16(tbl, &config->slot[i], sizeof(config->slot[i])-2);
        result = result &&
            (NCFG_WriteBackupMemory(NCFG_SLOT_1+i, &config->slot[i], sizeof(config->slot[i])) >= 0);
    }
    config->rsv.crc = MATH_CalcCRC16(tbl, &config->rsv, sizeof(config->rsv)-2);
    result = result &&
        (NCFG_WriteBackupMemory(NCFG_SLOT_RESERVED, &config->rsv, sizeof(config->rsv)) >= 0);
    return result;
}

static BOOL writeAllExSlots(NCFGConfigEx* configEx, const MATHCRC16Table *tbl)
{
    int  i;
    BOOL result = TRUE;

    for (i = 0; i < 3; i++)
    {
        configEx->slotEx[i].crc   = MATH_CalcCRC16(tbl, &configEx->slotEx[i],              0xFE);
        configEx->slotEx[i].crcEx = MATH_CalcCRC16(tbl, (u8*)&configEx->slotEx[i] + 0x100, 0xFE);
        result = result &&
            (NCFG_WriteBackupMemory(NCFG_SLOT_EX_1+i, &configEx->slotEx[i], sizeof(configEx->slotEx[i])) >= 0);
    }
    return result;
}

/*---------------------------------------------------------------------------*
    End of file
 *---------------------------------------------------------------------------*/
