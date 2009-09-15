/*---------------------------------------------------------------------------*
  Project:  TwlSDK - SPI - demos
  File:     monkey.c

  Copyright 2003-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-08#$
  $Rev: 10700 $
  $Author: yosizaki $
 *---------------------------------------------------------------------------*/

#include    "monkey.h"
#include    <nitro/spi.h>
#include    <nitro/spi/common/pm_common.h>
#include    <nitro/spi/ARM9/pm.h>


/*---------------------------------------------------------------------------*
    �����֐���`
 *---------------------------------------------------------------------------*/
static void MonkeyThread(void *arg);
static void MonkeySamplingAlarm(void *arg);
static void MonkeyMicCallback(MICResult result, void *arg);
static void MonkeyTpCallback(TPRequestCommand command, TPRequestResult result, u16 index);


/*---------------------------------------------------------------------------*
    �����ϐ���`
 *---------------------------------------------------------------------------*/
static MonkeyWork monkey;


/*---------------------------------------------------------------------------*
  Name:         MonkeyInit

  Description:  SPI�f�o�C�X�T���v�����O�p�X���b�h���N������B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyInit(void)
{
    // �T���v�����O�݂̂��s���X���b�h���쐬
    OS_InitMessageQueue(&(monkey.msg_q), monkey.msg_buf, MONKEY_MESSAGE_ARRAY_MAX);
    OS_CreateThread(&(monkey.thread),
                    MonkeyThread,
                    0,
                    (void *)(monkey.stack + (MONKEY_STACK_SIZE / sizeof(u32))),
                    MONKEY_STACK_SIZE, MONKEY_THREAD_PRIORITY);
    OS_WakeupThreadDirect(&(monkey.thread));
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyGetNewTpData

  Description:  �ŐV�̃^�b�`�p�l���T���v�����O�f�[�^���擾����B
                ���łɈ�x�ǂݏo����Ă���f�[�^���ǂ����͍l�����Ă��Ȃ��B

  Arguments:    num   - �擾����f�[�^�����w�肷��B
                array - �f�[�^���擾����z����w�肷��B
                        �z��̐擪����V�������Ƀf�[�^���i�[�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyGetNewTpData(s32 num, TPData *array)
{
    s32     i;
    s32     index;

    index = (s32)(monkey.tpIndex);
    for (i = 0; i < num; i++)
    {
        index = (index + (MONKEY_TP_ARRAY_MAX - 1)) % MONKEY_TP_ARRAY_MAX;
        array[i] = monkey.tpBuf[index];
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyGetNewMicData

  Description:  �ŐV�̃}�C�N�T���v�����O�f�[�^���擾����B
                ���łɈ�x�ǂݏo����Ă���f�[�^���ǂ����͍l�����Ă��Ȃ��B

  Arguments:    num   - �擾����f�[�^�����w�肷��B
                array - �f�[�^���擾����z����w�肷��B
                        �z��̐擪����V�������Ƀf�[�^���i�[�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void MonkeyGetNewMicData(s32 num, u16 *array)
{
    s32     i;
    s32     index;

    index = (s32)(monkey.micIndex);
    for (i = 0; i < num; i++)
    {
        index = (index + (MONKEY_MIC_ARRAY_MAX - 1)) % MONKEY_MIC_ARRAY_MAX;
        array[i] = monkey.micBuf[index];
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyThread

  Description:  �T���v�����O���s���X���b�h�B

  Arguments:    arg - ���g�p�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyThread(void *arg)
{
#pragma unused( arg )

    OSMessage msg;

    // �}�C�NAPI������
    {
        MIC_Init();
        monkey.micIndex = 0;

        // PMIC������
        PM_Init();
        // AMP�I��
        (void)PM_SetAmp(PM_AMP_ON);
        // AMP�Q�C������
        (void)PM_SetAmpGain(PM_AMPGAIN_80);
    }
    // �^�b�`�p�l��API������
    {
        TPCalibrateParam calibrate;

        TP_Init();
        if (TP_GetUserInfo(&calibrate))
        {
            TP_SetCalibrateParam(&calibrate);
        }
        else
        {
            OS_Panic("Can't find TP calibration data.");
        }
        TP_SetCallback(MonkeyTpCallback);
        monkey.tpIndex = 0;
    }

    // �T���v�����O�^�C�~���O�p�^�C�}�[���J�n
    OS_CreateAlarm(&(monkey.alarm));
    monkey.timerCount = 0;
    OS_SetPeriodicAlarm(&(monkey.alarm),
                        OS_GetTick(),
                        (MONKEY_MIC_SPAN_TICK * MONKEY_SAMPLING_SPAN_LINE),
                        MonkeySamplingAlarm, NULL);

    while (TRUE)
    {
        // ���b�Z�[�W���󂯎��܂ŃX���b�h�x�~
        (void)OS_ReceiveMessage(&(monkey.msg_q), &msg, OS_MESSAGE_BLOCK);

        // �^�b�`�p�l���̃T���v�����O
        if ((u32)msg == MONKEY_MESSAGE_TYPE_TP)
        {
            TP_RequestSamplingAsync();
        }
        // �}�C�N�̃T���v�����O
        else if ((u32)msg == MONKEY_MESSAGE_TYPE_MIC)
        {
            if (MIC_RESULT_SUCCESS != MIC_DoSamplingAsync(MIC_SAMPLING_TYPE_12BIT,
                                                          &(monkey.micBuf[monkey.micIndex]),
                                                          MonkeyMicCallback, NULL))
            {
                OS_Printf("Monkey: MIC request failure.\n");
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeySamplingAlarm

  Description:  �X���b�h�Ƀ��b�Z�[�W�𑗂�^�C�~���O�𐧌䂷��A���[���n���h���B

  Arguments:    arg - ���g�p�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
    Periodic�^�C�}�[�ƃT���v�����O�̃^�C�~���O�}

    -+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
MIC   >----           >----           >----           >----           >----
TP          >--------                       >--------                       >--

 *---------------------------------------------------------------------------*/
static void MonkeySamplingAlarm(void *arg)
{
#pragma unused( arg )

    if (0 == (monkey.timerCount % 8))
    {
        (void)OS_SendMessage(&(monkey.msg_q), (void *)MONKEY_MESSAGE_TYPE_MIC, OS_MESSAGE_BLOCK);
    }
    else if (3 == (monkey.timerCount % 16))
    {
        (void)OS_SendMessage(&(monkey.msg_q), (void *)MONKEY_MESSAGE_TYPE_TP, OS_MESSAGE_BLOCK);
    }

    monkey.timerCount++;
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyMicCallback

  Description:  �}�C�N�̃T���v�����O�v���ɑ΂��鉞���R�[���o�b�N�֐��B
                ����ɃT���v�����O�ł����ꍇ�̓f�[�^�i�[��o�b�t�@�ʒu��
                ���ɐi�߂�B

  Arguments:    result - �}�C�N����v���ɑ΂��鏈�����ʁB
                arg    - ���g�p�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyMicCallback(MICResult result, void *arg)
{
#pragma unused( arg )

    if (result == MIC_RESULT_SUCCESS)
    {
        // �f�[�^�i�[�o�b�t�@�ʒu�����֐i�߂�
        monkey.micIndex = (u16)((monkey.micIndex + 1) % MONKEY_MIC_ARRAY_MAX);
    }
    else
    {
        // �T�E���h�̉e���Œ��O�̃T���v�����O�������������Ă��Ȃ�����
        // ����̐V���ȃT���v�����O�����͊J�n����Ȃ������B
        OS_Printf("Monkey: MIC request failure.\n");
    }
}

/*---------------------------------------------------------------------------*
  Name:         MonkeyTpCallback

  Description:  �^�b�`�p�l���̃T���v�����O�v���ɑ΂��鉞���R�[���o�b�N�֐��B
                ����ɃT���v�����O�ł����ꍇ�̓f�[�^�i�[��o�b�t�@�ʒu��
                ���ɐi�߂�B

  Arguments:    command - �������Ή����鑀��v���R�}���h�������B���g�p�B
                result  - �^�b�`�p�l������v���ɑ΂��鏈�����ʁB
                index   - �����T���v�����O���̃C���f�b�N�X�B���g�p�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void MonkeyTpCallback(TPRequestCommand command, TPRequestResult result, u16 index)
{
#pragma unused( command , index )

    if (result == TP_RESULT_SUCCESS)
    {
        // �T���v�����O���l���X�N���[�����W�l�ɕϊ�
        (void)TP_GetCalibratedResult(&(monkey.tpBuf[monkey.tpIndex]));
        // �f�[�^�i�[�o�b�t�@�ʒu�����֐i�߂�
        monkey.tpIndex = (u16)((monkey.tpIndex + 1) % MONKEY_TP_ARRAY_MAX);
    }
    else
    {
        // �T�E���h�̉e���Œ��O�̃T���v�����O�������������Ă��Ȃ�����
        // ����̐V���ȃT���v�����O�����͊J�n����Ȃ������B
        OS_Printf("Monkey: TP request failure.\n");
    }
}

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
