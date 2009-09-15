/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - dsp
  File:     dsp_util.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-11-26#$
  $Rev: 9412 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/
#ifndef TWL_DSP_UTIL_H_
#define TWL_DSP_UTIL_H_


#include <twl/types.h>


#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*/
/* constants */

#define DSP_SLOT_B_COMPONENT_G711           1
#define DSP_SLOT_C_COMPONENT_G711           2

#define DSP_SLOT_B_COMPONENT_JPEGENCODER    3
#define DSP_SLOT_C_COMPONENT_JPEGENCODER    4

#define DSP_SLOT_B_COMPONENT_JPEGDECODER    2
#define DSP_SLOT_C_COMPONENT_JPEGDECODER    4

#define DSP_SLOT_B_COMPONENT_GRAPHICS       1
#define DSP_SLOT_C_COMPONENT_GRAPHICS       4

#define DSP_SLOT_B_COMPONENT_AACDECODER     2
#define DSP_SLOT_C_COMPONENT_AACDECODER     4

/*---------------------------------------------------------------------------*/
/* functions */

void DSPi_PlaySoundCore(const void *src, u32 len, BOOL stereo);
BOOL DSPi_PlayShutterSoundCore(const void *src, u32 len);
void DSPi_StopSoundCore(void);
BOOL DSPi_IsSoundPlayingCore(void);
BOOL DSPi_IsShutterSoundPlayingCore(void);
void DSPi_StartSamplingCore(void *buffer, u32 length);
void DSPi_StopSamplingCore(void);
void DSPi_SyncSamplingBufferCore(void);
const u8* DSPi_GetLastSamplingAddressCore(void);


/*---------------------------------------------------------------------------*
  Name:         DSP_PlaySound

  Description:  DSP�T�E���h�o�͂��璼�ڃT�E���h���Đ��B
  
  Arguments:    src    : ���f�[�^�ƂȂ�T���v�����O�f�[�^�B
                         PCM16bit32768kHz��4�o�C�g���E�������Ă���K�v������B
                len    : �T���v�����O�f�[�^�̃o�C�g�T�C�Y�B
                stereo : �X�e���I�Ȃ�TRUE�A���m�����Ȃ�FALSE�B

  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_PlaySound(const void *src, u32 len, BOOL stereo)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_PlaySoundCore(src, len, stereo);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_PlayShutterSound

  Description:  DSP�T�E���h�o�͂���V���b�^�[�����Đ��B
  
  Arguments:    src    : SDK�t����WAVE�`���̃V���b�^�[���f�[�^�B
                len    : �V���b�^�[���f�[�^�̃o�C�g�T�C�Y�B

  
  Returns:      SDNEX�֐������������Ȃ��TRUE��Ԃ�.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSP_PlayShutterSound(const void *src, u32 len)
{
    if (OS_IsRunOnTwl())
    {
        return DSPi_PlayShutterSoundCore(src, len);
    }
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_IsSoundPlaying

  Description:  DSP�T�E���h�o�͂����ݍĐ���������B
  
  Arguments:    None.
  
  Returns:      DSP�T�E���h�o�͂����ݍĐ����Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSP_IsSoundPlaying(void)
{
    BOOL    retval = FALSE;
    if (OS_IsRunOnTwl())
    {
        retval = DSPi_IsSoundPlayingCore();
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_IsShutterSoundPlaying

  Description:  DSP�T�E���h�o�͂����݃V���b�^�[�����Đ���������B
  
  Arguments:    None.
  
  Returns:      DSP�T�E���h�o�͂����݃V���b�^�[�����Đ����Ȃ�TRUE�B
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSP_IsShutterSoundPlaying(void)
{
    BOOL    retval = FALSE;
    if (OS_IsRunOnTwl())
    {
        retval = DSPi_IsShutterSoundPlayingCore();
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StopSound

  Description:  DSP�T�E���h�o�͂��璼�ڃT�E���h���Đ��B
  
  Arguments:    None.
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_StopSound(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_StopSoundCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StartSampling

  Description:  DSP�ɂ��}�C�N�T���v�����O���J�n�B

  Arguments:    buffer : �T���v�����O�Ɏg�p���郊���O�o�b�t�@�B
                         16bit�̐����{�ɋ��E�������Ă���K�v������B
                length : �����O�o�b�t�@�̃T�C�Y�B
                         16bit�̐����{�ɋ��E�������Ă���K�v������B

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_StartSampling(void *buffer, u32 length)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_StartSamplingCore(buffer, length);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_StopSampling

  Description:  DSP�ɂ��}�C�N�T���v�����O���~�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_StopSampling(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_StopSamplingCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SyncSamplingBuffer

  Description:  DSP�����̃����O�o�b�t�@����X�V����������ARM9���֓ǂݍ��݁B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE void DSP_SyncSamplingBuffer(void)
{
    if (OS_IsRunOnTwl())
    {
        DSPi_SyncSamplingBufferCore();
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetLastSamplingAddress

  Description:  ARM9���Ƀ��[�h����Ă��郍�[�J�������O�o�b�t�@��
                �ŐV�̃T���v���ʒu���擾�B

  Arguments:    None.

  Returns:      �ŐV�̃T���v���ʒu�B
 *---------------------------------------------------------------------------*/
SDK_INLINE const u8* DSP_GetLastSamplingAddress(void)
{
    const u8   *retval = NULL;
    if (OS_IsRunOnTwl())
    {
        retval = DSPi_GetLastSamplingAddressCore();
    }
    return retval;
}


/*---------------------------------------------------------------------------*
 * �ȉ��͓����֐��B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSP_HookPostStartProcess

  Description:  DSP�v���Z�X�C���[�W�����[�h��������̃t�b�N�B
                DSP�R���|�[�l���g�J���҂��f�o�b�K���N�����邽�߂ɕK�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_HookPostStartProcess(void);


/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif


#endif /* TWL_DSP_UTIL_H_ */
