/*---------------------------------------------------------------------------*
  Project:  TwlSDK - library - dsp
  File:     dsp_g711.c

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-21#$
  $Rev: 9387 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/

#include <twl.h>
#include <twl/dsp.h>

#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/g711.h>

#include "dsp_process.h"


/*---------------------------------------------------------------------------*/
/* variables */

static BOOL DSPiG711Available = FALSE;
static u32  DSPiG711CommandSend = 0;
static u32  DSPiG711CommandRecv = 0;
static DSPProcessContext DSPiProcessG711[1];


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DSPi_OpenStaticComponentG711Core

  Description:  G.711�R���|�[�l���g�p�̃������t�@�C�����J���B
                �t�@�C���V�X�e���Ɏ��O�ɗp�ӂ��Ă����K�v���Ȃ��Ȃ邪
                �ÓI�������Ƃ��ă����N����邽�߃v���O�����T�C�Y����������B
  
  Arguments:    file : �������t�@�C�����J��FSFile�\���́B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_OpenStaticComponentG711Core(FSFile *file)
{
    extern const u8 DSPiFirmware_audio[];
    (void)DSPi_CreateMemoryFile(file, DSPiFirmware_audio);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_SendCodecG711

  Description:  G.711�R�[�f�b�N�փR�}���h�𔭍s�B
  
  Arguments:    dst   : �ϊ���o�b�t�@�B (A-law�܂���u-law�̃p�b�N�X�g���[��)
                src   : �ϊ����o�b�t�@�B (PCM 8kHz * 8bits)
                len   : �ϊ��T���v�����B
                flags : �R�[�f�b�N����ѕϊ������̃r�b�g�W���B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_SendCodecG711(void *dst, const void *src, u32 len, DSPAudioCodecMode flags)
{
    DSPAudioCodecCommand    command;
    command.ctrl = DSP_32BIT_TO_DSP(flags);
    command.src = DSP_32BIT_TO_DSP((DSPAddrInARM)src);
    command.dst = DSP_32BIT_TO_DSP((DSPAddrInARM)dst);
    command.len = DSP_32BIT_TO_DSP(len);
    DSP_WriteProcessPipe(DSPiProcessG711, DSP_PIPE_BINARY, &command, sizeof(command));

    ++DSPiG711CommandSend;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_LoadG711Core

  Description:  G.711�������������T�|�[�g����R���|�[�l���g��DSP�փ��[�h�B
  
  Arguments:    file  : G.711�R���|�[�l���g�̃t�@�C���B
                slotB : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B
  
  Returns:      G.711�R���|�[�l���g��������DSP�փ��[�h������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSPi_LoadG711Core(FSFile *file, int slotB, int slotC)
{
    if (!DSPiG711Available)
    {
        DSP_InitProcessContext(DSPiProcessG711, "g711");
        if (DSP_ExecuteProcess(DSPiProcessG711, file, slotB, slotC))
        {
            DSPiG711Available = TRUE;
        }
    }
    return DSPiG711Available;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_UnloadG711Core

  Description:  G.711�������������T�|�[�g����R���|�[�l���g��DSP����A�����[�h�B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_UnloadG711Core(void)
{
    if (DSPiG711Available)
    {
        DSP_QuitProcess(DSPiProcessG711);
        DSPiG711Available = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_EncodeG711Core

  Description:  G.711�G���R�[�h�B
  
  Arguments:    dst  : �ϊ���o�b�t�@�B (A-law�܂���u-law�̃p�b�N�X�g���[��)
                src  : �ϊ����o�b�t�@�B (PCM 8kHz * 8bits)
                len  : �ϊ��T���v�����B
                mode : �R�[�f�b�N��ʁB
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_EncodeG711Core(void *dst, const void *src, u32 len, DSPAudioCodecMode mode)
{
    DSPi_SendCodecG711(dst, src, len,
                       (DSPAudioCodecMode)(DSP_AUDIO_CODEC_TYPE_ENCODE |
                                           (mode & DSP_AUDIO_CODEC_MODE_MASK)));
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_DecodeG711Core

  Description:  G.711�f�R�[�h�B
  
  Arguments:    dst  : �ϊ���o�b�t�@�B (PCM 8kHz * 8bits)
                src  : �ϊ����o�b�t�@�B (A-law�܂���u-law�̃p�b�N�X�g���[��)
                len  : �ϊ��T���v�����B
                mode : �R�[�f�b�N��ʁB
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_DecodeG711Core(void *dst, const void *src, u32 len, DSPAudioCodecMode mode)
{
    DSPi_SendCodecG711(dst, src, len,
                       (DSPAudioCodecMode)(DSP_AUDIO_CODEC_TYPE_DECODE |
                                           (mode & DSP_AUDIO_CODEC_MODE_MASK)));
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_TryWaitForG711Core

  Description:  �Ō�ɔ��s�����R�}���h���������Ă��邩����B
  
  Arguments:    None.
  
  Returns:      �Ō�ɔ��s�����R�}���h���������Ă���Ȃ�TRUE.
 *---------------------------------------------------------------------------*/
BOOL DSPi_TryWaitForG711Core(void)
{
    BOOL    retval = (DSPiG711CommandSend == DSPiG711CommandRecv);
    if (!retval)
    {
        DSPByte32   ack;
        DSPPipe     binin[1];
        (void)DSP_LoadPipe(binin, DSP_PIPE_BINARY, DSP_PIPE_INPUT);
        if (DSP_GetPipeReadableSize(binin) >= sizeof(ack))
        {
            DSP_ReadPipe(binin, &ack, sizeof(ack));
            ++DSPiG711CommandRecv;
            retval = (DSPiG711CommandSend == DSPiG711CommandRecv);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_WaitForG711Core

  Description:  �Ō�ɔ��s�����R�}���h�̊�����҂B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_WaitForG711Core(void)
{
    if (DSPiG711CommandSend != DSPiG711CommandRecv)
    {
        DSPByte32   ack;
        DSPPipe     binin[1];
        (void)DSP_LoadPipe(binin, DSP_PIPE_BINARY, DSP_PIPE_INPUT);
        while (DSPiG711CommandSend != DSPiG711CommandRecv)
        {
            DSP_ReadPipe(binin, &ack, sizeof(ack));
            ++DSPiG711CommandRecv;
        }
    }
}
