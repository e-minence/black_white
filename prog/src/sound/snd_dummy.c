//=============================================================================
/**
 * @file	sound_dummy.c
 * @brief	�T�E���h�p�̃������m�ۂ�\�񂵂��t�@�C��
            �T�E���h�����ۂɂ͍폜���Ȃ���΂����Ȃ�
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	2009.01.20
 */
//=============================================================================

#include <gflib.h>

static u8 dummySoundData[0xB0000];
extern void SoundDummyInit(void);

void SoundDummyInit(void)
{
    GFL_STD_MemClear(dummySoundData,sizeof(dummySoundData));
}

