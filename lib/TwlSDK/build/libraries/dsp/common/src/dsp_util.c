/*---------------------------------------------------------------------------*
  Project:  TwlSDK - library - dsp
  File:     dsp_util.c

  Copyright 2007-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-07-16#$
  $Rev: 10913 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/
#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/audio.h>

#include "dsp_process.h"

/*---------------------------------------------------------------------------*/
/* constants */

// �T�E���h�Đ��D��x�B
#define DSP_SOUND_PRIORITY_SHUTTER          0
#define DSP_SOUND_PRIORITY_NORMAL           16
#define DSP_SOUND_PRIORITY_NONE             32

// WAVE �t�@�C���̉�͗p
#define MAKE_FOURCC(cc1, cc2, cc3, cc4) (u32)((cc1) | (cc2 << 8) | (cc3 << 16) | (cc4 << 24))
#define MAKE_TAG_DATA(ca) (u32)((*(ca)) | (*(ca+1) << 8) | (*(ca+2) << 16) | (*(ca+3) << 24))
#define FOURCC_RIFF MAKE_FOURCC('R', 'I', 'F', 'F')
#define FOURCC_WAVE MAKE_FOURCC('W', 'A', 'V', 'E')
#define FOURCC_fmt  MAKE_FOURCC('f', 'm', 't', ' ')
#define FOURCC_data MAKE_FOURCC('d', 'a', 't', 'a')

// �V���b�^�[���̍Đ��O�ɓ����E�F�C�g(msec)
#define DSP_WAIT_SHUTTER                    60

/*---------------------------------------------------------------------------*/
/* variables */

// DSP�T�E���h�Đ����t���O�B
static BOOL DSPiSoundPlaying = FALSE;
static OSAlarm DSPiSoundAlarm;

// ���݂�DSP�T�E���h�Đ��D��x�B
static int DSPiSoundPriority = DSP_SOUND_PRIORITY_NONE;

// DSP�̃T���v�����O������B
typedef struct DSPAudioCaptureInfo
{
    DSPAddr bufferAddress;
    DSPWord bufferLength;
    DSPWord currentPosition;
}
DSPAudioCaptureInfo;
static DSPAudioCaptureInfo  DSPiAudioCapture;
extern DSPAddr              DSPiAudioCaptureAddress;
static DSPAddr              DSPiReadPosition = 0;

// ARM9���̃��[�J���o�b�t�@�B
static u32  DSPiLocalRingLength = 0;
static u8  *DSPiLocalRingBuffer = NULL;
static int  DSPiLocalRingOffset = 0;
static DSPAddr DSPiAudioCaptureAddress = 0;

static BOOL DSPiPlayingShutter = FALSE;

// �V���b�^�[���Đ��������ɃT�E���h�ݒ��߂��֐�
static void DSPiShutterPostProcessCallback( SNDEXResult result, void* arg )
{
#pragma unused(arg)
    if(result == SNDEX_RESULT_EXCLUSIVE)
    {
        // �V���b�^�[�̃T�E���h�ݒ��߂��֐��͕K����������
        OS_TPanic("SNDEXi_PostProcessForShutterSound SNDEX_RESULT_EXCLUSIVE ... DSP_PlayShutterSound\n");
    }
    if(result != SNDEX_RESULT_SUCCESS)
    {
        // �V���b�^�[�̃T�E���h�ݒ��߂��֐��͕K����������
        OS_TPanic("SNDEXi_PostProcessForShutterSound Error ... DSP_PlayShutterSound\n");
    }
    DSPiPlayingShutter = FALSE;
}

/*---------------------------------------------------------------------------*/
/* functions */

// DSP����̃T�E���h�Đ���̃����O�o�b�t�@�̋�Đ��҂���ɔ�������֐�
static void sound_handler(void* arg)
{
#pragma unused(arg)
    DSPiSoundPlaying = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_PipeCallbackForSound

  Description:  �T�E���h�Đ������R�[���o�b�N�B

  Arguments:    userdata : �C�ӂ̃��[�U��`����
                port     : �|�[�g�ԍ�
                peer     : ����M����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_PipeCallbackForSound(void *userdata, int port, int peer)
{
    (void)userdata;
    if (peer == DSP_PIPE_INPUT)
    {
        // DSP����̃R�}���h���X�|���X����M�B
        DSPAudioDriverResponse  response;
        DSPPipe                 pipe[1];
        (void)DSP_LoadPipe(pipe, port, peer);
        if (DSP_GetPipeReadableSize(pipe) >= sizeof(response))
        {
            DSP_ReadPipe(pipe, &response, sizeof(response));
            response.ctrl = DSP_32BIT_TO_DSP(response.ctrl);
            response.result = DSP_32BIT_TO_DSP(response.result);
            // �I�[�f�B�I�o�̓R�}���h�B
            if ((response.ctrl & DSP_AUDIO_DRIVER_TARGET_MASK) == DSP_AUDIO_DRIVER_TARGET_OUTPUT)
            {
                // ��~�R�}���h�B
                if ((response.ctrl & DSP_AUDIO_DRIVER_CONTROL_MASK) == DSP_AUDIO_DRIVER_CONTROL_STOP)
                {
                    //DSPiSoundPlaying = FALSE;
                    OS_CreateAlarm(&DSPiSoundAlarm);
                    OS_SetAlarm(&DSPiSoundAlarm, OS_MilliSecondsToTicks(30), sound_handler, NULL);
                    if(DSPiSoundPriority == DSP_SOUND_PRIORITY_SHUTTER)
                    {
                          // �V���b�^�[�̃T�E���h�ݒ��߂��֐��͕K����������
                          (void)SNDEXi_PostProcessForShutterSound(DSPiShutterPostProcessCallback, 0);
                    }
                    DSPiSoundPriority = DSP_SOUND_PRIORITY_NONE;
                }
            }
            // �I�[�f�B�I���̓R�}���h�B
            if ((response.ctrl & DSP_AUDIO_DRIVER_TARGET_MASK) == DSP_AUDIO_DRIVER_TARGET_INPUT)
            {
                // �J�n�R�}���h�B
                if ((response.ctrl & DSP_AUDIO_DRIVER_CONTROL_MASK) == DSP_AUDIO_DRIVER_CONTROL_START)
                {
                    DSPiAudioCaptureAddress = (DSPAddr)response.result;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_PlaySoundEx

  Description:  DSP�T�E���h�o�͂��璼�ڃT�E���h���Đ��B
  
  Arguments:    src      : ���f�[�^�ƂȂ�T���v�����O�f�[�^�B
                           PCM16bit32768kHz��4�o�C�g���E�������Ă���K�v������B
                len      : �T���v�����O�f�[�^�̃o�C�g�T�C�Y�B
                ctrl     : DSP_AUDIO_DRIVER_MODE_* �̑g�ݍ��킹�B
                priority : �Đ��D��x�B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_PlaySoundEx(const void *src, u32 len, u32 ctrl, int priority)
{
    // �R���|�[�l���g�������N�����Ă��Ȃ���Ζ����B
    DSPProcessContext  *context = DSP_FindProcess(NULL);
    if (context)
    {
        // ���ݍĐ����̃T�E���h���Ⴂ�D��x�Ȃ�Đ����Ȃ��B
        if (DSPiSoundPriority < priority)
        {
            OS_TWarning("still now playing high-priority sound.\n");
        }
        else
        {
            DSPiSoundPriority = priority;
            ctrl |= DSP_AUDIO_DRIVER_TARGET_OUTPUT;
            ctrl |= DSP_AUDIO_DRIVER_CONTROL_START;
            // �o�C�g�T�C�Y����n�[�t���[�h�T�C�Y�ɕύX�B
            len >>= 1;
            // �����ʒm�p�ɃR�[���o�b�N��ݒ�B
            DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
            DSPiSoundPlaying = TRUE;
            {
                DSPAudioDriverCommand   command;
                command.ctrl = DSP_32BIT_TO_DSP(ctrl);
                command.buf = DSP_32BIT_TO_DSP(src);
                command.len = DSP_32BIT_TO_DSP(len);
                command.opt = DSP_32BIT_TO_DSP(0);
                DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_PlaySound

  Description:  DSP�T�E���h�o�͂��璼�ڃT�E���h���Đ��B
  
  Arguments:    src    : ���f�[�^�ƂȂ�T���v�����O�f�[�^�B
                         PCM16bit32768kHz��4�o�C�g���E�������Ă���K�v������B
                len    : �T���v�����O�f�[�^�̃o�C�g�T�C�Y�B
                stereo : �X�e���I�Ȃ�TRUE�A���m�����Ȃ�FALSE�B

  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_PlaySoundCore(const void *src, u32 len, BOOL stereo)
{
    u32     ctrl = (stereo != FALSE) ? DSP_AUDIO_DRIVER_MODE_STEREO : DSP_AUDIO_DRIVER_MODE_MONAURAL;
    DSPi_PlaySoundEx(src, len, ctrl, DSP_SOUND_PRIORITY_NORMAL);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_PlayShutterSound

  Description:  DSP�T�E���h�o�͂��璼�ڃT�E���h���Đ��B
  
  Arguments:    src    : ���f�[�^�ƂȂ�T���v�����O�f�[�^�B
                         PCM16bit32768kHz��4�o�C�g���E�������Ă���K�v������B
                len    : �T���v�����O�f�[�^�̃o�C�g�T�C�Y�B

  
  Returns:      SDNEX�֐������������Ȃ��TRUE��Ԃ�.
 *---------------------------------------------------------------------------*/
#if 0 // �����܂ł������肵�� WAVE �̃`�F�b�N�͍s��Ȃ�
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len)
{
    u8* wave_data = (u8*)src;
    u32 cur = 0;
    u32 tag;
    u32 wave_len;
    u32 raw_len;
    BOOL    fFmt = FALSE, fData = FALSE;
    
    static SNDEXFrequency freq;
    u32 sampling;
    u32 chunkSize;

    // �^����ꂽ�f�[�^�� WAVE �t�@�C�����m�F����
    if(len < cur+12)
        return FALSE;
    tag = MAKE_TAG_DATA(wave_data+cur);
    if(tag != FOURCC_RIFF)
        return FALSE;
    cur+=4;

    wave_len = MAKE_TAG_DATA(wave_data+cur);
    cur+=4;

    tag = MAKE_TAG_DATA(wave_data+cur);
    if(tag != FOURCC_WAVE)
        return FALSE;
    cur+=4;

    while (wave_len > 0)
    {
        if(len < cur+8)
            return FALSE;
        tag = MAKE_TAG_DATA(wave_data+cur);
        cur+=4;
        chunkSize = MAKE_TAG_DATA(wave_data+cur);
        cur+=4;
        
        if(len < cur+chunkSize)
            return FALSE;
        
        switch (tag)
        {
        case FOURCC_fmt:
            // �t�H�[�}�b�g��񂩂�T���v�����O���[�g���`�F�b�N����
            // �V���b�^�[���Đ����łȂ���΁A���g�����擾
            if(!DSPi_IsShutterSoundPlayingCore)
            {
                if(SNDEX_GetI2SFrequency(&freq) != SNDEX_RESULT_SUCCESS)
                    return FALSE;
            }
            sampling = MAKE_TAG_DATA(wave_data+cur+4);
            cur+=chunkSize;
            if( ((freq == SNDEX_FREQUENCY_32730)&&(sampling != 32730))||((freq == SNDEX_FREQUENCY_47610)&&(sampling != 47610)) )
                return FALSE;
            fFmt = TRUE;
            break;
        case FOURCC_data:
            raw_len = chunkSize;
            fData = TRUE;
            break;
        default:
            cur+=chunkSize;
            break;
        }
        if(fFmt && fData)
            break;
        wave_len -= chunkSize;
    }
    if(!(fFmt && fData))
        return FALSE;

    // �w�b�h�t�H���ڑ����A�{�̃X�s�[�J�[�͐؂��Ă���B
    // �o�͂����肳���邽�߂ɃV���b�^�[���Đ��O�� ��60msec �̃E�F�C�g������
    // ���̃E�F�C�g�́A���슴�𓝈ꂷ�邽�߂ɂ����Ȃ�ꍇ������邱�ƂƂ���
    OS_SpinWait(67 * DSP_WAIT_SHUTTER * 1000);	// �� 60msec
    
    // �֐������ŃT�E���h�o�̓��[�h��ύX
    while(SNDEXi_PreProcessForShutterSound() != SNDEX_RESULT_SUCCESS)
    {
        OS_Sleep(1); // ��������܂Ń��g���C
    }

    {
        u32     ctrl = DSP_AUDIO_DRIVER_MODE_MONAURAL;
        // �V���b�^�[���͍��E��50%�������B
        ctrl |= DSP_AUDIO_DRIVER_MODE_HALFVOL;
        DSPi_PlaySoundEx((wave_data+cur), raw_len, ctrl, DSP_SOUND_PRIORITY_SHUTTER);
        DSPiPlayingShutter = TRUE;
    }

    return TRUE;
}
#else // �`�F�b�N�̖ړI�͂����܂Ŏ��g���Ⴂ�̃V���b�^�[����炳�Ȃ��Ƃ����_�̂�
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len)
{
#pragma unused(len)
    u32 cur;
    u32 sampling;
    u32 raw_len;
    u8* wave_data = (u8*)src;
    static SNDEXFrequency freq;

    if(len < 44)
        return FALSE;
    
    if(MAKE_TAG_DATA(wave_data) != FOURCC_RIFF)
        return FALSE;

    if(MAKE_TAG_DATA(wave_data+8) != FOURCC_WAVE)
        return FALSE;

    cur = 24;
    sampling = MAKE_TAG_DATA(wave_data+cur);
    
    // �t�H�[�}�b�g��񂩂�T���v�����O���[�g���`�F�b�N����
    // �V���b�^�[���Đ����łȂ���΁A���g�����擾
    if(!DSPi_IsShutterSoundPlayingCore)
    {
        if(SNDEX_GetI2SFrequency(&freq) != SNDEX_RESULT_SUCCESS)
            return FALSE;
    }
    if( ((freq == SNDEX_FREQUENCY_32730)&&(sampling != 32730))||((freq == SNDEX_FREQUENCY_47610)&&(sampling != 47610)) )
        return FALSE;
    
    cur += 16;
    raw_len = MAKE_TAG_DATA(wave_data+cur);
    cur += 4;

    if(len < cur+raw_len)
        return FALSE;
       
    // �w�b�h�t�H���ڑ����A�{�̃X�s�[�J�[�͐؂��Ă���B
    // �o�͂����肳���邽�߂ɃV���b�^�[���Đ��O�� ��60msec �̃E�F�C�g������
    // ���̃E�F�C�g�́A���슴�𓝈ꂷ�邽�߂ɂ����Ȃ�ꍇ������邱�ƂƂ���
    OS_SpinWait(67 * DSP_WAIT_SHUTTER * 1000);	// �� 60msec

    // �֐������ŃT�E���h�o�̓��[�h��ύX
    while(SNDEXi_PreProcessForShutterSound() != SNDEX_RESULT_SUCCESS)
    {
        OS_Sleep(1); // ��������܂Ń��g���C
    }

    {
        u32     ctrl = DSP_AUDIO_DRIVER_MODE_MONAURAL;
        // �V���b�^�[���͍��E��50%�������B
        ctrl |= DSP_AUDIO_DRIVER_MODE_HALFVOL;
        DSPi_PlaySoundEx((wave_data+cur), raw_len, ctrl, DSP_SOUND_PRIORITY_SHUTTER);
        DSPiPlayingShutter = TRUE;
    }

    return TRUE;
}
#endif

/*---------------------------------------------------------------------------*
  Name:         DSP_StopSound

  Description:  DSP�T�E���h�o�͂���̍Đ����~�B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_StopSoundCore(void)
{
    // �R���|�[�l���g�������N�����Ă��Ȃ���Ζ����B
    DSPProcessContext  *context = DSP_FindProcess(NULL);
    if (context && DSPiSoundPlaying)
    {
        int     ctrl = 0;
        ctrl |= DSP_AUDIO_DRIVER_TARGET_OUTPUT;
        ctrl |= DSP_AUDIO_DRIVER_CONTROL_STOP;
        {
            DSPAudioDriverCommand   command;
            command.ctrl = DSP_32BIT_TO_DSP(ctrl);
            command.buf = DSP_32BIT_TO_DSP(0);
            command.len = DSP_32BIT_TO_DSP(0);
            command.opt = DSP_32BIT_TO_DSP(0);
            DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_IsSoundPlaying

  Description:  DSP�T�E���h�o�͂����ݍĐ���������B
  
  Arguments:    None.
  
  Returns:      DSP�T�E���h�o�͂����ݍĐ����Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSPi_IsSoundPlayingCore(void)
{
    return DSPiSoundPlaying;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_IsShutterSoundPlaying

  Description:  DSP�T�E���h�o�͂����݃V���b�^�[�����Đ���������B
  
  Arguments:    None.
  
  Returns:      DSP�T�E���h�o�͂����݃V���b�^�[�����Đ����Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSPi_IsShutterSoundPlayingCore(void)
{
    return (DSPiSoundPlaying | DSPiPlayingShutter);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StartSampling

  Description:  DSP�T�E���h���͂��璼�ڃ}�C�N�������T���v�����O�B
  
  Arguments:    buffer : �T���v�����O�Ɏg�p���郊���O�o�b�t�@�B
                         16bit�̐����{�ɋ��E�������Ă���K�v������B
                length : �����O�o�b�t�@�̃T�C�Y�B
                         16bit�̐����{�ɋ��E�������Ă���K�v������B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_StartSamplingCore(void *buffer, u32 length)
{
    SDK_ALIGN2_ASSERT(buffer);
    SDK_ALIGN2_ASSERT(length);
    {
        // �R���|�[�l���g�������N�����Ă��Ȃ���Ζ����B
        DSPProcessContext  *context = DSP_FindProcess(NULL);
        if (context)
        {
            int     ctrl = 0;
            ctrl |= DSP_AUDIO_DRIVER_TARGET_INPUT;
            ctrl |= DSP_AUDIO_DRIVER_CONTROL_START;
            DSPiLocalRingLength = length;
            DSPiLocalRingBuffer = (u8 *)buffer;
            DSPiLocalRingOffset = 0;
            DSPiAudioCaptureAddress = 0;
            // �����ʒm�p�ɃR�[���o�b�N��ݒ�B
            DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
            {
                DSPAudioDriverCommand   command;
                command.ctrl = DSP_32BIT_TO_DSP(ctrl);
                command.buf = DSP_32BIT_TO_DSP(0);
                command.len = DSP_32BIT_TO_DSP(0);
                command.opt = DSP_32BIT_TO_DSP(0);
                DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StopSampling

  Description:  DSP�T�E���h���͂���̃T���v�����O���~�B
  
  Arguments:    None.

  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_StopSamplingCore(void)
{
    // �R���|�[�l���g�������N�����Ă��Ȃ���Ζ����B
    DSPProcessContext  *context = DSP_FindProcess(NULL);
    if (context)
    {
        int     ctrl = 0;
        ctrl |= DSP_AUDIO_DRIVER_TARGET_INPUT;
        ctrl |= DSP_AUDIO_DRIVER_CONTROL_STOP;
        // �����ʒm�p�ɃR�[���o�b�N��ݒ�B
        DSP_SetPipeCallback(DSP_PIPE_AUDIO, DSPi_PipeCallbackForSound, NULL);
        {
            DSPAudioDriverCommand   command;
            command.ctrl = DSP_32BIT_TO_DSP(ctrl);
            command.buf = DSP_32BIT_TO_DSP(0);
            command.len = DSP_32BIT_TO_DSP(0);
            command.opt = DSP_32BIT_TO_DSP(0);
            DSP_WriteProcessPipe(context, DSP_PIPE_AUDIO, &command, sizeof(command));
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SyncSamplingBuffer

  Description:  DSP�����̃����O�o�b�t�@����X�V����������ARM9���֓ǂݍ��݁B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_SyncSamplingBufferCore(void)
{
    // DSP���̃��j�^�����O�A�h���X�𖢎�M�Ȃ牽�����Ȃ��B
    if (DSPiAudioCaptureAddress != 0)
    {
        // DSP�����̃L���v�`�������擾���A���ǂ�����Γǂݏo���B
        DSP_LoadData(DSP_ADDR_TO_ARM(DSPiAudioCaptureAddress), &DSPiAudioCapture, sizeof(DSPiAudioCapture));
        if (DSPiAudioCapture.currentPosition != DSPiReadPosition)
        {
            // DSP����ARM9���Ń����O�o�b�t�@�̃T�C�Y���قȂ邽��
            // �I�[�ɒ��ӂ��R�s�[���Ă����B
            int     cur = DSPiAudioCapture.currentPosition;
            int     end = (DSPiReadPosition > cur) ? DSPiAudioCapture.bufferLength : cur;
            int     len = end - DSPiReadPosition;
            while (len > 0)
            {
                int     segment = (int)MATH_MIN(len, DSP_WORD_TO_DSP32(DSPiLocalRingLength - DSPiLocalRingOffset));
                DSP_LoadData(DSP_ADDR_TO_ARM(DSPiAudioCapture.bufferAddress + DSPiReadPosition),
                             &DSPiLocalRingBuffer[DSPiLocalRingOffset], DSP_WORD_TO_ARM(segment));
                len -= segment;
                DSPiReadPosition += segment;
                if (DSPiReadPosition >= DSPiAudioCapture.bufferLength)
                {
                    DSPiReadPosition -= DSPiAudioCapture.bufferLength;
                }
                DSPiLocalRingOffset += (int)DSP_WORD_TO_ARM32(segment);
                if (DSPiLocalRingOffset >= DSPiLocalRingLength)
                {
                    DSPiLocalRingOffset -= DSPiLocalRingLength;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetLastSamplingAddress

  Description:  ARM9���Ƀ��[�h����Ă��郍�[�J�������O�o�b�t�@��
                �ŐV�̃T���v���ʒu���擾�B

  Arguments:    None.

  Returns:      �ŐV�̃T���v���ʒu�B
 *---------------------------------------------------------------------------*/
const u8* DSPi_GetLastSamplingAddressCore(void)
{
    int     offset = DSPiLocalRingOffset - (int)sizeof(u16);
    if (offset < 0)
    {
        offset += DSPiLocalRingLength;
    }
    return &DSPiLocalRingBuffer[offset];
}

