/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - dsp
  File:     dsp_process.h

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-21#$
  $Rev: 9018 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/
#ifndef TWL_DSP_PROCESS_H_
#define TWL_DSP_PROCESS_H_


/*---------------------------------------------------------------------------*
 * ���̃��W���[����TWL����B
 *---------------------------------------------------------------------------*/
#ifdef SDK_TWL


#include <twl/dsp.h>
#include <twl/dsp/common/byteaccess.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// WRAM-B/C�X���b�g�T�C�Y�B
#define DSP_WRAM_SLOT_SIZE (32 * 1024)

// �t�b�N�o�^�Ɏg�p����r�b�g��`�}�N���B
#define DSP_HOOK_SEMAPHORE_(id) (1 << (id))
#define DSP_HOOK_REPLY_(id)     (1 << (16 + (id)))
#define DSP_HOOK_MAX            (16 + 3)

// �N������SDK�ŗL�̓����������g�p���邩�ǂ����̃t���O�B
#define DSP_PROCESS_FLAG_USING_OS   0x0001

/*---------------------------------------------------------------------------*/
/* declarations */

// DSP���荞�݃t�b�N�֐��v���g�^�C�v�B
typedef void (*DSPHookFunction)(void *userdata);

// �v���Z�X���������\���́B
// DSP�v���Z�X�̃��[�h�菇�͈ȉ��̒ʂ�B
//   (1) �v���Z�X�C���[�W�̃Z�N�V�����e�[�u����񋓂���
//       ��L�����Z�O�����g�z�u��(�Z�O�����g�}�b�v)���Z�o�B
//   (2) DSP�Ɏg�p�������ꂽWRAM-B/C�̃X���b�g�Q�̂���
//       ���ۂ�DSP�Z�O�����g�Ƃ��Ďg�p������̂�I�肵�A
//       �e�Z�O�����g�Ƃ̑Ή��\(�X���b�g�}�b�v)������B
//   (3) �Ăуv���Z�X�C���[�W�̃Z�N�V�����e�[�u����񋓂���
//       ���x�͎��ۂɃv���Z�X�C���[�W��WRAM-B/C��֔z�u����B
typedef struct DSPProcessContext
{
    // �}���`�v���Z�X�Ή��̂��߂̒P�������X�g�ƃv���Z�X���B
    struct DSPProcessContext   *next;
    char                        name[15 + 1];
    // �v���Z�X�C���[�W�̊i�[���ꂽ�t�@�C���n���h���Ɨ��p�\��WRAM�B
    // ���[�h�����̊Ԃ����ꎞ�I�Ɏg�p����B
    FSFile                     *image;
    u16                         slotB;
    u16                         slotC;
    int                         flags;
    // �v���Z�X����L����DSP���̃Z�O�����g�}�b�v�B
    // DSP_MapProcessSegment�֐����Z�o����B
    int                         segmentCode;
    int                         segmentData;
    // �e�Z�O�����g�Ɋ��蓖�Ă�ꂽ�X���b�g�}�b�v�B
    // DSP_StartupProcess�֐��Ń}�b�p�[���z�u����B
    int                         slotOfSegmentCode[8];
    int                         slotOfSegmentData[8];
    // DSP�̊e�튄�荞�ݗv���ɑ΂��ēo�^���ꂽ�t�b�N�֐��B
    // DSP_SetProcessHook�֐��ŗv���ʂɓo�^����B
    int                         hookFactors;
    DSPHookFunction             hookFunction[DSP_HOOK_MAX];
    void                       *hookUserdata[DSP_HOOK_MAX];
    int                         hookGroup[DSP_HOOK_MAX];
}
DSPProcessContext;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DSP_StopDSPComponent

  Description:  DSP�̏I���������s���B
                ���݂�DSP��DMA���~�����Ă��邾���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_StopDSPComponent(void);

/*---------------------------------------------------------------------------*
  Name:         DSP_InitProcessContext

  Description:  �v���Z�X���\���̂��������B

  Arguments:    context : DSPProcessContext�\���́B
                name    : �v���Z�X���܂���NULL�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_InitProcessContext(DSPProcessContext *context, const char *name);

/*---------------------------------------------------------------------------*
  Name:         DSP_ExecuteProcess

  Description:  DSP�v���Z�X�C���[�W�����[�h���ċN���B

  Arguments:    context : ��ԊǗ��Ɏg�p����DSPProcessContext�\���́B
                          �v���Z�X��j������܂ŃV�X�e���ɂ���ĎQ�Ƃ����B
                image   : �v���Z�X�C���[�W���w���t�@�C���n���h���B
                          ���̊֐����ł̂ݎQ�Ƃ����B
                slotB   : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC   : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
BOOL DSP_ExecuteProcess(DSPProcessContext *context, FSFile *image, int slotB, int slotC);

/*---------------------------------------------------------------------------*
  Name:         DSP_QuitProcess

  Description:  DSP�v���Z�X���I�����ă�����������B

  Arguments:    context : ��ԊǗ��Ɏg�p����DSPProcessContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_QuitProcess(DSPProcessContext *context);

/*---------------------------------------------------------------------------*
  Name:         DSP_FindProcess

  Description:  ���s���̃v���Z�X�������B

  Arguments:    name : �v���Z�X���B
                       NULL���w�肵����Ō�ɓo�^�����v���Z�X�������B

  Returns:      �w�肵�����O�ɍ��v����DSPProcessContext�\���́B
 *---------------------------------------------------------------------------*/
DSPProcessContext* DSP_FindProcess(const char *name);

/*---------------------------------------------------------------------------*
  Name:         DSP_GetProcessSlotFromSegment

  Description:  �w�肵���Z�O�����g�ԍ��ɊY������WRAM�X���b�g�ԍ����擾�B

  Arguments:    context : DSPProcessContext�\���́B
                wram    : MI_WRAM_B/MI_WRAM_C�B
                segment : �Z�O�����g�ԍ��B

  Returns:      �w�肵���Z�O�����g�ԍ��ɊY������WRAM�X���b�g�ԍ��B
 *---------------------------------------------------------------------------*/
SDK_INLINE int DSP_GetProcessSlotFromSegment(const DSPProcessContext *context, MIWramPos wram, int segment)
{
    return (wram == MI_WRAM_B) ? context->slotOfSegmentCode[segment] : context->slotOfSegmentData[segment];
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ConvertProcessAddressFromDSP

  Description:  DSP��������Ԃ̃A�h���X����WRAM�̃A�h���X�֕ϊ��B

  Arguments:    context : DSPProcessContext�\���́B
                wram    : MI_WRAM_B/MI_WRAM_C�B
                address : DSP��������Ԃ̃A�h���X�B(DSP��������Ԃ̃��[�h�P��)

  Returns:      �w�肵��DSP�A�h���X�ɊY������WRAM�A�h���X�B
 *---------------------------------------------------------------------------*/
SDK_INLINE void* DSP_ConvertProcessAddressFromDSP(const DSPProcessContext *context, MIWramPos wram, int address)
{
    int     segment = (address / (DSP_WRAM_SLOT_SIZE/2));
    int     mod = (address - segment * (DSP_WRAM_SLOT_SIZE/2));
    int     slot = DSP_GetProcessSlotFromSegment(context, wram, segment);
    SDK_ASSERT((slot >= 0) && (slot < MI_WRAM_B_MAX_NUM));
    return (u8*)MI_GetWramMapStart(wram) + slot * DSP_WRAM_SLOT_SIZE + mod * 2;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_AttachProcessMemory

  Description:  �v���Z�X�̖��g�p�̈�ɘA��������������Ԃ����蓖�Ă�B

  Arguments:    context : DSPProcessContext�\���́B
                wram    : ���蓖�Ă郁������ԁB (MI_WRAM_B/MI_WRAM_C)
                slots   : ���蓖�Ă�WRAM�X���b�g�B

  Returns:      ���蓖�Ă�ꂽDSP��������Ԃ̐擪�A�h���X�܂���0�B
 *---------------------------------------------------------------------------*/
u32 DSP_AttachProcessMemory(DSPProcessContext *context, MIWramPos wram, int slots);

/*---------------------------------------------------------------------------*
  Name:         DSP_DetachProcessMemory

  Description:  �v���Z�X�̎g�p�̈�Ɋ��蓖�Ă�ꂽWRAM�X���b�g���������B

  Arguments:    context : DSPProcessContext�\���́B
                slots   : ���蓖�čς݂�WRAM�X���b�g�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_DetachProcessMemory(DSPProcessContext *context, MIWramPos wram, int slots);

/*---------------------------------------------------------------------------*
  Name:         DSP_SwitchProcessMemory

  Description:  �w��̃v���Z�X���g�p����DSP�A�h���X�Ǘ���؂�ւ���B

  Arguments:    context : DSPProcessContext�\���́B
                wram    : �؂�ւ��郁������ԁB (MI_WRAM_B/MI_WRAM_C)
                address : �؂�ւ���擪�A�h���X�B(DSP��������Ԃ̃��[�h�P��)
                length  : �؂�ւ��郁�����T�C�Y�B(DSP��������Ԃ̃��[�h�P��)
                to      : �V�����}�X�^�[�v���Z�b�T�B

  Returns:      �S�ẴX���b�g���������؂�ւ������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_SwitchProcessMemory(DSPProcessContext *context, MIWramPos wram, u32 address, u32 length, MIWramProc to);

/*---------------------------------------------------------------------------*
  Name:         DSP_SetProcessHook

  Description:  �v���Z�X�ւ�DSP���荞�ݗv���ɑ΂��ăt�b�N��ݒ�B

  Arguments:    context  : DSPProcessContext�\���́B
                factors  : �w�肷�銄�荞�ݗv���̃r�b�g�W���B
                function : �Ăяo���ׂ��t�b�N�֐��B
                userdata : �t�b�N�֐��ɗ^����C�ӂ̃��[�U��`�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SetProcessHook(DSPProcessContext *context, int factors, DSPHookFunction function, void *userdata);

/*---------------------------------------------------------------------------*
  Name:         DSPi_CreateMemoryFile

  Description:  �ÓI��DSP�v���Z�X�C���[�W���������t�@�C���֕ϊ��B

  Arguments:    memfile : �������t�@�C���ƂȂ�FSFile�\���́B
                image   : DSP�v���Z�X�C���[�W��ێ�����o�b�t�@�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_INLINE BOOL DSPi_CreateMemoryFile(FSFile *memfile, const void *image)
{
    if (!FS_IsAvailable())
    {
        OS_TWarning("FS is not initialized yet.\n");
        FS_Init(FS_DMA_NOT_USE);
    }
    FS_InitFile(memfile);
    return FS_CreateFileFromMemory(memfile, (void *)image, (u32)(16 * 1024 * 1024));
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ReadProcessPipe

  Description:  �v���Z�X�̏���|�[�g�Ɋ֘A�t����ꂽ�p�C�v�����M�B

  Arguments:    context : DSPProcessContext�\���́B
                port    : ��M���̃|�[�g�B
                buffer  : ��M�f�[�^�B
                length  : ��M�T�C�Y�B(�o�C�g�P��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_ReadProcessPipe(DSPProcessContext *context, int port, void *buffer, u32 length);

/*---------------------------------------------------------------------------*
  Name:         DSP_WriteProcessPipe

  Description:  �v���Z�X�̏���|�[�g�Ɋ֘A�t����ꂽ�p�C�v�֑��M�B

  Arguments:    context : DSPProcessContext�\���́B
                port    : ���M��̃|�[�g�B
                buffer  : ���M�f�[�^�B
                length  : ���M�T�C�Y�B(�o�C�g�P��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_WriteProcessPipe(DSPProcessContext *context, int port, const void *buffer, u32 length);

#if defined(DSP_SUPPORT_OBSOLETE_LOADER)
/*---------------------------------------------------------------------------*
 * �ȉ��͌��ݎg�p����Ă��Ȃ��Ǝv����p�~���C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSP_MapProcessSegment

  Description:  �v���Z�X����L����Z�O�����g�}�b�v���Z�o�B

  Arguments:    context : DSPProcessContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_MapProcessSegment(DSPProcessContext *context);

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadProcessImage

  Description:  �w�肳�ꂽ�v���Z�X�C���[�W�����[�h����B
                �Z�O�����g�}�b�v�͂��łɎZ�o�ς݂łȂ���΂Ȃ�Ȃ��B

  Arguments:    context  : DSPProcessContext�\���́B

  Returns:      �S�ẴC���[�W�����������[�h������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_LoadProcessImage(DSPProcessContext *context);

/*---------------------------------------------------------------------------*
  Name:         DSP_StartupProcess

  Description:  �v���Z�X�C���[�W�̃Z�O�����g�}�b�v���Z�o����WRAM�փ��[�h����B
                DSP_MapProcessSegment�֐���DSP_LoadProcessImage�֐��̕����ŁB

  Arguments:    context    : DSPProcessContext�\���́B
                image      : �v���Z�X�C���[�W���w���t�@�C���n���h���B
                             ���̊֐����ł̂ݎQ�Ƃ����B
                slotB      : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC      : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B
                slotMapper : �Z�O�����g��WRAM�X���b�g�����蓖�Ă�A���S���Y���B
                             NULL���w�肷��΃f�t�H���g�̍œK�ȏ������I�������B

  Returns:      �S�ẴC���[�W�����������[�h������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_StartupProcess(DSPProcessContext *context, FSFile *image,
                        int slotB, int slotC, BOOL (*slotMapper)(DSPProcessContext *, int, int));


#endif


#ifdef __cplusplus
} // extern "C"
#endif


#endif // SDK_TWL


/*---------------------------------------------------------------------------*/
#endif // TWL_DSP_PROCESS_H_
