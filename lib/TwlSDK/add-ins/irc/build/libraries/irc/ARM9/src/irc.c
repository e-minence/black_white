/*---------------------------------------------------------------------------*
  Project:  TwlSDK - IRC - libraries
  File:     irc.c

  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-04-20 #$
  $Rev: 10429 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/
#include <nitro.h>
#include <nitro/irc.h>

/*---------------------------------------------------------------------------*/
/* constants */

/*---------------------------------------------------------------------------*/
/* variables */

static u8 myUnitNumber = 0xFF;  // ���j�b�g�ԍ�
static u8 unit_number  = 0xFF;  // �ʐM����̃��j�b�g�ԍ�


static BOOL isTry    = FALSE;   // �e�q�����܂��Ă��邩�ǂ���
static BOOL isSender = FALSE;   // �Z���_���ǂ���

static OSTick timeout;

static u8 sIrcBuf[sizeof(IRCHeader)+IRC_TRNS_MAX] ATTRIBUTE_ALIGN(4);

static u32 cnt_retry;            // ���C�u���������g���C�J�E���g��Ԃ����߂Ɏg�p
static u8 retry_count = 0;
static BOOL flg_restore = FALSE; // �G���[���A�����ǂ���
static IRCStatus sStatus;

// �R�[���o�b�N�֐��Q
static IRCConnectCallback  sConnectCallback;
static IRCRecvCallback     sRecvCallback;
static IRCShutdownCallback sShutdownCallback;

/*---------------------------------------------------------------------------*/
/* declarations */

static u16 IRCi_CalcSum(void *input, u32 length);
static void IRCi_Restore(void);
static void IRC_InitCommon(void);
static void IRCi_Send(const u8 *data, u8 size, u8 command, u8 id);
static void IRCi_Shutdown(BOOL isError);
static void print_connect(void);
static BOOL IRCi_CheckNoise(u32 size);
static BOOL IRCi_CheckTimeout(void);
static BOOL IRCi_CheckSum(u32 size);

/*---------------------------------------------------------------------------*/
/* functions */

static u16 IRCi_CalcSum(void *input, u32 length)
{
    u32 sum;
    u32 i;
    u8  t;

    sum = IRC_SUM_VALUE;  // �`�F�b�N�T�������l

    for (i = 0; i < length; i++)
    {
        t = *((u8*)input)++;
        if (i & 1)
        {
            sum += t;
        }
        else
        {
            sum += t << 8;
        }
    }

    sum  = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return ((u16)sum);
}

static void IRCi_Restore(void)
{
    flg_restore = TRUE;
    cnt_retry++; // IRC �̃J�E���^�A�b�v
}

// ���ʏ���������(�V���b�g�_�E�����ɂ��Ă����Ȃ����̂̂�)
static void IRC_InitCommon(void)
{
    retry_count = 0;

    sStatus.status = IRC_ST_DISCONNECT;
    sStatus.flg_connect = FALSE;
    flg_restore = FALSE;

    isTry = FALSE;
    isSender = FALSE;

    cnt_retry = 0;
    PRINTF("(IRC_InitCommon: isSender = FALSE) shutdown and retry_count cleared\n");
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Init

  Description:  ���C�u�����̏������ƃ��j�b�g�ԍ���ݒ肷�邽�߂̊֐�

  Arguments:    unitId : �ʐM�̎��ʂɎg����@�� ID
                         �C�V�����甭�s���ꂽ�@�� ID ���w�肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Init(u8 unitId)
{
    IRCi_Init();

    {
        u8 size = IRCi_Read(sIrcBuf); // IRC�Ɏc���Ă���f�[�^���N���A����ړI
    }

    IRC_InitCommon(); // �V���b�g�_�E�����ɂ��Ă΂�鋤�ʏ���������
    IRC_SetUnitNumber(unitId); // myUnitNumber = 0xFF;
    unit_number = 0xFF;
    sConnectCallback = NULL;
    sRecvCallback = NULL;
    sShutdownCallback = NULL;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Connect

  Description:  �ڑ��v�����邽�߂̊֐�
                �{�֐����Ă񂾕�(���̗v�����󂯂����Ƌt)���Z���_�ƂȂ�܂�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Connect(void)
{
    if (sStatus.flg_connect) // �ڑ���
    {
        return;
    }
    if (isTry)
    {
        return;
    }
    isTry = TRUE;

    PRINTF("IRC_Connect �̃R�[���ɂ��ڑ��J�n\n");

    {
        u8 command = IR_CONNECT ^ 0xAA;
        IRCi_Write(&command, 1);
    }
    sStatus.status = IRC_ST_WAIT_RESPONSE; // ���X�|���X�҂���Ԃɂ���
    timeout = OS_GetTick(); // �^�C���A�E�g���ԍX�V
}

/*---------------------------------------------------------------------------*
  Name:         IRC_IsSender

  Description:  �ʐM�J�n���s�����[��(�Z���_��)���ǂ������ׂ邽�߂̊֐�
                (�ڑ����̂ݗL��)

  Arguments:    None.

  Returns:      ��ɐڑ��v�����o�������ł���� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_IsSender(void)
{
    return (isSender);
}

/*---------------------------------------------------------------------------*
  Name:         IRC_IsConnect

  Description:  �ʐM�����N������Ă��邩�ǂ����m�F���邽�߂̊֐�

  Arguments:    None.

  Returns:      �R�l�N�V����������A�ʐM�\�ł���� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_IsConnect(void)
{
    return (sStatus.flg_connect);
}


/*---------------------------------------------------------------------------*
  Name:         IRCi_Send

  Description:  �w�b�_�쐬�� ^0xAA ���������邽�߂̊֐�

  Arguments:    data    : ���M�������f�[�^
                size    : ���M����f�[�^�̃o�C�g��
                command : �ʐM���e�����ʂ��邽�߂̃R�}���h�ԍ�
                id      : ���j�b�g�ԍ�

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void IRCi_Send(const u8 *data, u8 size, u8 command, u8 id)
{
    u8  *p;
    u16 sum;
    int i;
    IRCHeader *pHeader;

    pHeader = (IRCHeader*) & sIrcBuf[0];
    pHeader->command = command;
    pHeader->unitId = id;
    pHeader->sumL = 0;
    pHeader->sumH = 0;
    p = &sIrcBuf[sizeof(IRCHeader)];
    for (i = 0; i < size; i++)
    {
        *p++ = *data++;
    }

    size += sizeof(IRCHeader);
    sum   = IRCi_CalcSum(&sIrcBuf[0], size);

    pHeader->sumL = (u8)(sum & 0xFF);
    pHeader->sumH = (u8)((sum >> 8) & 0xFF);

    for (i = 0; i < size; i++) // �Í� // �f�o�C�X�I���R�}���h�͕������̂őS�ĈÍ���
    {
        sIrcBuf[i]= (u8)(sIrcBuf[i] ^ 0xAA);
    }
    IRCi_Write(sIrcBuf, size); // �ԊO�����M
}


/*---------------------------------------------------------------------------*
  Name:         IRCi_Shutdown

  Description:  �ʐM�������I�����邽�߂̊֐�

  Arguments:    isError : TRUE �Ȃ�G���[�ɂ��I������
                          FALSE �Ȃ琳��ȏI������

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void IRCi_Shutdown(BOOL isError)
{
    if (sStatus.flg_connect && (!isSender)) // �ؒf�R�}���h���M�B�R�}���h�̂ݑ���ꍇ�̓T�C�Y��0�ɂ���K�v������
    {
        if (isError)
        {
            // �G���[�I���̏ꍇ�̓V���b�g�_�E���R�}���h���o���� �G���[���o�����ɏI�����Ă��܂����Ƃ�����̂ňӐ}�I�Ƀ^�C���A�E�g���N��������
        }
        else
        {
            IRCi_Send(NULL, 0, IR_SHUTDOWN, myUnitNumber);
        }
    }
    if (sShutdownCallback != NULL)
    {
        sShutdownCallback(isError);
    }
    if (isError)
    {
        PRINTF("IRC_Shutdown(�G���[) called\n");
    }
    else
    {
        PRINTF("IRC_Shutdown(����) called\n");
    }
    IRC_InitCommon();
    PRINTF("�V���b�g�_�E�������I��\n");
    PRINTF("=============================================\n\n");
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Shutdown

  Description:  �ʐM�������I�����邽�߂̊֐�

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Shutdown(void)
{
    IRCi_Shutdown(FALSE); // �G���[�ɂ��V���b�g�_�E���ł͂Ȃ�
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Send

  Description:  �f�[�^�𑗐M���邽�߂̊֐�

  Arguments:    buf     : ���M�������f�[�^�̐擪�A�h���X
                          �R�}���h�̂ݑ��肽���ꍇ�� NULL ���w�肵�Ă�������
                size    : ���M����f�[�^�̃o�C�g��
                command : �ʐM���e�����ʂ��邽�߂̃R�}���h�ԍ�
                          �����̂ݎg�p�\�ł����A���e�̓A�v���P�[�V�����Ŏ��R�ɐݒ肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_Send(const u8 *buf, u8 size, u8 command)
{
    if (sStatus.flg_connect == FALSE)
    {
        return;
    }
    IRCi_Send(buf, size, command, myUnitNumber);
}


static void print_connect(void)
{
    if (sStatus.flg_connect)
    { // IRC �����ُ팟�o���Đڑ����ɍĐڑ��Ƃ��Ă��̃R�}���h�𑗂��Ă����ꍇ
        PRINTF("===========================================================================\n");
        PRINTF("�ʐM���肩��Đڑ��v��(0x%X)��M ID 0x%X\n", sIrcBuf[0], sIrcBuf[1]);
    }
    else
    {
        PRINTF("===========================================================================\n");
        PRINTF("\n�V�K�ڑ��v����M -> ���X�|���X���M ������UnitNumber %d isSender = FALSE\n", myUnitNumber);
    }
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_CheckNoise

  Description:  �m�C�Y��\�����邽�߂̊֐�(�f�o�b�O�p)

  Arguments:    size    : ��M�����f�[�^�̃T�C�Y

  Returns:      �m�C�Y�����������ꍇ�� TRUE ��
                �����łȂ��ꍇ�� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
static BOOL IRCi_CheckNoise(u32 size)
{
    u32 i;

    if (size == 1)
    {
        if (sStatus.flg_connect) // �ڑ�����1�o�C�g��M�͖���
        {
            return (TRUE);
        }
        if (sIrcBuf[0] != IR_CONNECT)
        {  // �ڑ��R�}���h�ȊO��1�o�C�g��M������
            PRINTF("�m�C�Y�擾(1byte) 0x%X(0x%X)\n", sIrcBuf[0], sIrcBuf[0]^0xAA); 
            return TRUE; 
        }
        return FALSE;
    }
    if (size == 2 || size == 3)
    {
        PRINTF("�m�C�Y(2 or 3 bytes)\n");
        for (i = 0; i < 4; i++)
        {
            PRINTF("%02X ", sIrcBuf[i]);
        }
        PRINTF("\n"); // �m�C�Y�����������ꍇ���f�[�^�̐擪��\��
        for (i = 0; i < size; i++)
        {
            PRINTF("%02X ", sIrcBuf[i]);
        }
        PRINTF("\n");

        return TRUE;
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_CheckTimeout

  Description:  �^�C���A�E�g�`�F�b�N�����邽�߂̊֐�

  Arguments:    None.

  Returns:      ���炩�̃^�C���A�E�g���������Ă����Ԃł������ꍇ�� TRUE ��
                �����łȂ��ꍇ�� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
static BOOL IRCi_CheckTimeout(void)
{
    if (sStatus.flg_connect == FALSE && isTry == FALSE) // ��ڑ����̓G���[�ɂ��Ȃ�
    {
        return FALSE;
    }

    // �ʏ펞�̃^�C���A�E�g�`�F�b�N
    if (OS_TicksToMilliSeconds(OS_GetTick() - timeout) < 100) // �^�C���A�E�g���ĂȂ�
    {
        return FALSE;
    }

    if(flg_restore)
    {
        PRINTF("���A���̃^�C���A�E�g!!\n");
    }
    else if (isTry)
    { 
        PRINTF("�ڑ��r���ł̃^�C���A�E�g\n"); 
    }
    else
    {
        PRINTF("�ʏ��Ԃł̃^�C���A�E�g!!\n");
    }
    IRCi_Shutdown(TRUE);
    isTry = FALSE; // �ڑ��g���C���t���O���N���A
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         IRCi_CheckSum

  Description:  �`�F�b�N�T�����m�F���邽�߂̊֐�

  Arguments:    size    : ��M�����f�[�^�̃T�C�Y

  Returns:      �`�F�b�N�T���������������ꍇ�� TRUE ��
                �����łȂ��ꍇ�� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
static BOOL IRCi_CheckSum(u32 size)
{
    u16 checksum;
    u16 mysum;
    IRCHeader *pHeader;

    pHeader = (IRCHeader*)sIrcBuf;

    if (size == 1)
    {
        if (pHeader->command == IR_CONNECT)
        {
            return TRUE;
        }
        else
        {
            // 1�o�C�g�̃R�l�N�g�R�}���h�̓`�F�b�N���Ȃ�
            return FALSE;
        }
    }
    if (size < 4)
    {
        return FALSE;
    }

    pHeader = (IRCHeader*)sIrcBuf;
    checksum = (u16)(pHeader->sumL + (pHeader->sumH << 8)); 
    pHeader->sumL = 0;
    pHeader->sumH = 0; 
    mysum = IRCi_CalcSum(sIrcBuf, size);
    pHeader->sumL = (u8)(checksum & 0xFF);
    pHeader->sumH = (u8)(checksum >> 8); // �߂��Ă���

    if (checksum == mysum)
    {
        if (retry_count)
        {
            PRINTF("retry_count cleared COMMAND 0x%X\n", sIrcBuf[0]);
        }
        retry_count = 0; 
        return TRUE;
    }

    // �`�F�b�N�T���G���[
    if (sStatus.flg_connect)
    {  // ���A�����J�n(�ڑ����̂�)
        PRINTF("ERROR: �`�F�b�N�T���G���[ �f�[�^�T�C�Y %d command 0x%X ID 0x%X\n", size, pHeader->command, pHeader->unitId);
        {
            int i;
            for (i = 0; i < size; i++)
            {
                PRINTF("0x%X ", sIrcBuf[i]);
                if ((i & 0xF) == 0xF)
                {
                    PRINTF("\n");
                }
            }
        }
        if ((pHeader->command & 0xFE) != 0xFE)
        { // 0xFE �̓m�C�Y
            PRINTF("�`�F�b�N�T���G���[�ɂ��Đڑ��J�n ��M�R�}���h 0x%X RETRY-COUNT %d: size %d 0x%X(COM) -> 0x%X(CALC)\n", pHeader->command, retry_count, size, checksum, mysum); ; 
        }
        IRCi_Restore(); 
    }
    size = 0; // �f�[�^���͂��Ȃ��������Ƃɂ���
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Move

  Description:  ���t���[���Ăԃv���Z�X�֐�

  Arguments:    None.

  Returns:      IRCError �^�̃G���[�ԍ���Ԃ��܂�
 *---------------------------------------------------------------------------*/
IRCError IRC_Move(void)
{
    u32 size;
    IRCHeader *pHeader;

    size = (u32)IRCi_Read(sIrcBuf);

    if (IRCi_CheckNoise(size))
    { // �m�C�Y�������̏���
        PRINTF("NOISE!\n");
        size = 0;
    }
    if (size == 0)
    {
        if (IRCi_CheckTimeout())
        {
            PRINTF("TIMEOUT!\n");
            return IRC_ERROR_TIMEOUT;
        }
        else
        {
            return IRC_ERROR_NONE;
        }
    }
    if (IRCi_CheckSum(size) == FALSE)
    {
        // �`�F�b�N�T���G���[
        PRINTF("CHECKSUM ERROR\n");
        return IRC_ERROR_NONE;
    }

    timeout = OS_GetTick(); // �^�C���A�E�g���ԍX�V

    // �`�F�b�N�T���`�F�b�N���܂߂ăf�[�^���͂��Ă����ꍇ(�ڑ��R�}���h�͗�O)
    pHeader = (IRCHeader*)sIrcBuf;

    if (pHeader->command < 0xF0)
    {
        if (sStatus.flg_connect == FALSE)
        {
            // �V�X�e���R�}���h�ȊO�͐ڑ����łȂ���΂͂˂�
            return IRC_ERROR_NONE;
        }
        if (pHeader->unitId != myUnitNumber)
        {
            // ���j�b�gID ����v���Ă��Ȃ���Δ������Ȃ�
            PRINTF("UNIT_ID_ERROR my 0x%02X com 0x%02X\n");
            return IRC_ERROR_NONE;
        }
    }

    switch (pHeader->command)
    {
    case IR_CONNECT: // �ڑ��R�}���h
        switch (sStatus.status)
        {
        case IRC_ST_WAIT_RESPONSE:  // ���X�|���X �𑗂�(�҂��Ă�����ԂƈႤ���A����ɂ��킹��)
            sStatus.status = IRC_ST_WAIT_ACK; // ACK �҂����
            IRCi_Send(NULL, 0, IR_RESPONSE, myUnitNumber);
            break;
        case IRC_ST_WAIT_ACK:
            break;    // ��U�Ԏ������Ă���̂Ŗ�������
        case IRC_ST_SENDER:     // �ڑ����͖�������
        case IRC_ST_RECEIVER:
            break;
        }
        if (isTry)
        {
            break;
        }
        isTry = TRUE;
        sStatus.status = IRC_ST_WAIT_ACK;
        isSender = FALSE;
        if ((size > 1) && (sStatus.flg_connect == 0)) // �ؒf���̍Đڑ��R�}���h�͖���
        {
            break;
        }
        IRCi_Send(NULL, 0, IR_RESPONSE, myUnitNumber);
        if (sStatus.flg_connect) // IRC �����ُ팟�o���Đڑ����ɍĐڑ��Ƃ��Ă��̃R�}���h�𑗂��Ă����ꍇ
        {
            IRCi_Restore();
        }
        print_connect(); // �f�o�b�O�o��
        break;
    case IR_RESPONSE: // �ڑ��v����������M
        if (sStatus.status != IRC_ST_WAIT_RESPONSE)
        {
            break;
        }
        sStatus.status = IRC_ST_SENDER;
        isSender = TRUE;
        unit_number = pHeader->unitId; // ����̃��j�b�g�ԍ�
        if (unit_number != myUnitNumber) // ���j�b�gID ����v���Ă��Ȃ���Δ������Ȃ�
        {
            break;
        }
        IRCi_Send(NULL, 0, IR_ACK, myUnitNumber);
        PRINTF("���X�|���X��M ���j�b�gID 0x%02X -> ACK ���M\n", pHeader->unitId);
        sStatus.flg_connect = TRUE;
        flg_restore = FALSE;
        isTry = FALSE;
        break;
    case IR_ACK: // �ڑ��v���������ACK(���ꂪ�͂��Đڑ�����)
        if (sStatus.status != IRC_ST_WAIT_ACK)
        {
            break;
        }
        PRINTF("RECEIVE_ACK: CONNECT ID 0x%02X\n", pHeader->unitId);
        sStatus.status = IRC_ST_RECEIVER;
        unit_number = pHeader->unitId; // ���j�b�g�ԍ�
        sStatus.flg_connect = TRUE;
        flg_restore = FALSE;
        isTry = FALSE;
        if (sConnectCallback != NULL)
        {
            sConnectCallback(pHeader->unitId, FALSE);
        }
        break;
    case IR_RETRY: // COMMAND_RETRY(IRC �G�~�����[�g�p)
        IRCi_Restore();
        PRINTF("�ʐM���肩�� RETRY �R�}���h����M\n");
        break;
    default:
        if (flg_restore == TRUE)
        { // ���A���ɓ͂����ʏ�R�}���h�͖�������
            break;
        }
        if (sStatus.flg_connect == FALSE)
        {
            break;
        }
        sRecvCallback(&sIrcBuf[sizeof(IRCHeader)], (u8)(size-sizeof(IRCHeader)), sIrcBuf[0], sIrcBuf[1]);
        break;
    }
    return IRC_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_SetConnectCallback

  Description:  �ڑ��R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �ڑ����ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetConnectCallback(IRCConnectCallback callback)
{
    sConnectCallback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_SetRecvCallback

  Description:  �f�[�^��M�R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �f�[�^��M���ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetRecvCallback(IRCRecvCallback callback)
{
    sRecvCallback = callback;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_SetShutdownCallback

  Description:  �ؒf�R�[���o�b�N��ݒ肷�邽�߂̊֐�

  Arguments:    callback : �ؒf���ɌĂяo�����R�[���o�b�N

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetShutdownCallback(IRCShutdownCallback callback)
{
    sShutdownCallback = callback;
}


/*---------------------------------------------------------------------------*
  Name:         IRC_GetRetryCount

  Description:  �g�[�^���̃��g���C�񐔂��擾���邽�߂̊֐�(�f�o�b�O�p)

  Arguments:    None.

  Returns:      ���g���C�񐔂�Ԃ��܂�
 *---------------------------------------------------------------------------*/
u32 IRC_GetRetryCount(void)
{
    return (cnt_retry);
}

/*---------------------------------------------------------------------------*
  Name:         IRC_SetUnitNumber

  Description:  ���j�b�g�ԍ���ݒ肷�邽�߂̊֐�

  Arguments:    unitId : �ʐM�̎��ʂɎg����@�� ID
                         �C�V�����甭�s���ꂽ�@�� ID ���w�肵�Ă�������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void IRC_SetUnitNumber(u8 id)
{
    myUnitNumber = id;
}

/*---------------------------------------------------------------------------*
  Name:         IRC_GetUnitNumber

  Description:  �ʐM����̃��j�b�g�ԍ����m�F���邽�߂̊֐�

  Arguments:    None.

  Returns:      �ʐM����̃��j�b�g�ԍ���Ԃ��܂�(�ؒf���� 0xFF)
 *---------------------------------------------------------------------------*/
u8 IRC_GetUnitNumber(void)
{
    return (unit_number);
}

/*---------------------------------------------------------------------------*
  Name:         IRC_Check

  Description:  �V�^ IRC �J�[�h���𔻕ʂ��邽�߂̊֐�

  Arguments:    None.

  Returns:      �V�^ IRC �J�[�h�Ȃ� TRUE ��
                �����łȂ���� FALSE ��Ԃ��܂�
 *---------------------------------------------------------------------------*/
BOOL IRC_Check(void)
{
    u8 ret = IRCi_Check();
    if (IRCi_Check() == 0xAA)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
