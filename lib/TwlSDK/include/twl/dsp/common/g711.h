/*---------------------------------------------------------------------------*
  Project:  TwlSDK - dspcomponents - include - dsp - audio
  File:     g711.h

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
#ifndef DSP_AUDIO_G711_H__
#define DSP_AUDIO_G711_H__


#include <twl/dsp/common/pipe.h>
#include <twl/dsp/common/audio.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* functions */

#ifdef SDK_TWL

/*---------------------------------------------------------------------------*
 * internal core functions placed on LTDMAIN.
 *---------------------------------------------------------------------------*/

void DSPi_OpenStaticComponentG711Core(FSFile *file);
BOOL DSPi_LoadG711Core(FSFile *file, int slotB, int slotC);
void DSPi_UnloadG711Core(void);
void DSPi_EncodeG711Core(void *dst, const void *src, u32 len, DSPAudioCodecMode mode);
void DSPi_DecodeG711Core(void *dst, const void *src, u32 len, DSPAudioCodecMode mode);
BOOL DSPi_TryWaitForG711Core(void);
void DSPi_WaitForG711Core(void);

/*---------------------------------------------------------------------------*
  Name:         DSP_OpenStaticComponentG711

  Description:  G.711�R���|�[�l���g�p�̃������t�@�C�����J���B
                �t�@�C���V�X�e���Ɏ��O�ɗp�ӂ��Ă����K�v���Ȃ��Ȃ邪
                �ÓI�������Ƃ��ă����N����邽�߃v���O�����T�C�Y����������B
  
  Arguments:    file : �������t�@�C�����J��FSFile�\���́B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_OpenStaticComponentG711(FSFile *file)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_OpenStaticComponentG711Core(file);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadG711

  Description:  G.711�������������T�|�[�g����R���|�[�l���g��DSP�փ��[�h�B
  
  Arguments:    file  : G.711�R���|�[�l���g�̃t�@�C���B
                slotB : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B
  
  Returns:      G.711�R���|�[�l���g��������DSP�փ��[�h������TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSP_LoadG711(FSFile *file, int slotB, int slotC)
{
    if (OS_IsRunOnTwl())
    {
        return DSPi_LoadG711Core(file, slotB, slotC);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_UnloadG711

  Description:  G.711�������������T�|�[�g����R���|�[�l���g��DSP����A�����[�h�B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_UnloadG711(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_UnloadG711Core();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EncodeG711

  Description:  G.711�G���R�[�h�B
  
  Arguments:    dst  : �ϊ���o�b�t�@�B (A-law/u-law 8bit)
                src  : �ϊ����o�b�t�@�B (PCM 16bit)
                len  : �ϊ��T���v�����B
                mode : �R�[�f�b�N��ʁB
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_EncodeG711(void *dst, const void *src, u32 len, DSPAudioCodecMode mode)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_EncodeG711Core(dst, src, len, mode);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DecodeG711

  Description:  G.711�f�R�[�h�B
  
  Arguments:    dst  : �ϊ���o�b�t�@�B (PCM 16bit)
                src  : �ϊ����o�b�t�@�B (A-law/u-law 8bit)
                len  : �ϊ��T���v�����B
                mode : �R�[�f�b�N��ʁB
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_DecodeG711(void *dst, const void *src, u32 len, DSPAudioCodecMode mode)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_DecodeG711Core(dst, src, len, mode);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_TryWaitForG711

  Description:  �Ō�ɔ��s�����R�}���h���������Ă��邩����B
  
  Arguments:    None.
  
  Returns:      �Ō�ɔ��s�����R�}���h���������Ă���Ȃ�TRUE.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSP_TryWaitForG711(void)
{
    if (OS_IsRunOnTwl())
    {
        return DSPi_TryWaitForG711Core();
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_WaitForG711

  Description:  �Ō�ɔ��s�����R�}���h�̊�����҂B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_WaitForG711(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_WaitForG711Core();
    }
}

#else

void DSP_EncodeG711(void *dst, const void *src, u32 len, DSPAudioCodecMode mode);
void DSP_DecodeG711(void *dst, const void *src, u32 len, DSPAudioCodecMode mode);

#endif // SDK_TWL


#ifdef __cplusplus
}
#endif


#endif // DSP_AUDIO_G711_H__
