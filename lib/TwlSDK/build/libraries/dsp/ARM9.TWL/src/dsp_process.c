/*---------------------------------------------------------------------------*
  Project:  TwlSDK - library - dsp
  File:     dsp_process.c

  Copyright 2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009-06-19#$
  $Rev: 10786 $
  $Author: okajima_manabu $
 *---------------------------------------------------------------------------*/


#include <twl.h>
#include <twl/dsp.h>
#include <twl/dsp/common/pipe.h>

#include "dsp_coff.h"


/*---------------------------------------------------------------------------*/
/* constants */

#define DSP_DPRINTF(...)    ((void) 0)
//#define DSP_DPRINTF OS_TPrintf

// �v���Z�X�C���[�W���̃Z�N�V�����񋓃R�[���o�b�N�B
typedef BOOL (*DSPSectionEnumCallback)(DSPProcessContext *, const COFFFileHeader *, const COFFSectionTable *);


/*---------------------------------------------------------------------------*/
/* variables */

// ���ݎ��s���̃v���Z�X�R���e�L�X�g�B
static DSPProcessContext   *DSPiCurrentComponent = NULL;
static PMExitCallbackInfo   DSPiShutdownCallbackInfo[1];
static BOOL                 DSPiShutdownCallbackIsRegistered = FALSE;
static PMSleepCallbackInfo  DSPiPreSleepCallbackInfo[1];
static BOOL                 DSPiPreSleepCallbackIsRegistered = FALSE;


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DSP_EnumSections

  Description:  �v���Z�X�C���[�W���̃Z�N�V������񋓂��ď���̏��������s�B

  Arguments:    context  : �R�[���o�b�N�����ɓn��DSPProcessContext�\���́B
                callback : �e�Z�N�V�����ɑ΂��ČĂяo�����R�[���o�b�N�B

  Returns:      �S�ẴZ�N�V�������񋓂�����TRUE�A�r���ŏI��������FALSE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_EnumSections(DSPProcessContext *context, DSPSectionEnumCallback callback);

/*---------------------------------------------------------------------------*
  Name:         DSP_StopDSPComponent

  Description:  DSP�̏I���������s���B
                ���݂�DSP��DMA���~�����Ă��邾���B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_StopDSPComponent(void)
{
    DSPProcessContext  *context = DSPiCurrentComponent;
    context->hookFactors = 0;
    DSP_SendData(DSP_PIPE_COMMAND_REGISTER, DSP_PIPE_FLAG_EXIT_OS);
    (void)DSP_RecvData(DSP_PIPE_COMMAND_REGISTER);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_ShutdownCallback

  Description:  �V�X�e���V���b�g�_�E���̋����I���R�[���o�b�N�B

  Arguments:    name : �v���Z�X���B
                       NULL���w�肵����Ō�ɓo�^�����v���Z�X�������B

  Returns:      �w�肵�����O�ɍ��v����DSPProcessContext�\���́B
 *---------------------------------------------------------------------------*/
static void DSPi_ShutdownCallback(void *arg)
{
    (void)arg;
    for (;;)
    {
        DSPProcessContext  *context = DSP_FindProcess(NULL);
        if (!context)
        {
            break;
        }
//        OS_TWarning("force-exit %s component.\n", context->name);
        DSP_QuitProcess(context);
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_PreSleepCallback

  Description:  �X���[�v�x���R�[���o�b�N�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_PreSleepCallback(void *arg)
{
#pragma unused( arg )
    OS_TPanic("Could not sleep while DSP is processing.\n");
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_MasterInterrupts

  Description:  DSP���荞�ݏ���

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_MasterInterrupts(void)
{
    DSPProcessContext  *context = DSPiCurrentComponent;
    if (context)
    {
        // DSP���荞�ݗv���̂ǂꂩ�����߂Ĕ��������u�Ԃ���
        // OS�ւ̊��荞�ݗv���t���O(IF)�͔������Ȃ����߁A
        // ������DSP���荞�ݗv���𒀎��������Ă���Ԃ�
        // �V����DSP���荞�ݗv�������������\��������Ȃ�
        // ���荞�݃n���h���ɗ��炸���̏�ŌJ��Ԃ��K�v������B
        for (;;)
        {
            // DSP���荞�ݗv���ɂȂ肤�郁�b�Z�[�W���ꊇ�Ŕ���B
            int     ready = (reg_DSP_SEM | (((reg_DSP_PSTS >> REG_DSP_PSTS_RRI0_SHIFT) & 7) << 16));
            int     factors = (ready & context->hookFactors);
            if (factors == 0)
            {
                break;
            }
            else
            {
                // �Y������t�b�N���������ɏ����B
                // �O���[�v�o�^����Ă���Ȃ�܂Ƃ߂�1�񂾂��ʒm�����B
                while (factors != 0)
                {
                    int     index = (int)MATH_CTZ((u32)factors);
                    factors &= ~context->hookGroup[index];
                    (*context->hookFunction[index])(context->hookUserdata[index]);
                }
            }
        }
    }
    OS_SetIrqCheckFlag(OS_IE_DSP);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_ReadProcessImage

  Description:  �v���Z�X�C���[�W����f�[�^��ǂݏo���B

  Arguments:    context : DSPProcessContext�\���́B
                offset  : �C���[�W���̃I�t�Z�b�g�B
                buffer  : �]����o�b�t�@�B
                length  : �]���T�C�Y�B

  Returns:      �w��̃T�C�Y�𐳂����ǂݎ�ꂽ��TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_ReadProcessImage(DSPProcessContext *context, int offset, void *buffer, int length)
{
    return FS_SeekFile(context->image, offset, FS_SEEK_SET) &&
           (FS_ReadFile(context->image, buffer, length) == length);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_CommitWram

  Description:  �w�肵���Z�O�����g�����̃v���Z�b�T�֊��蓖�āB

  Arguments:    context : DSPProcessContext�\���́B
                wram    : Code/Data�B
                segment : �؂�ւ���Z�O�����g�ԍ��B
                to      : �؂�ւ���v���Z�b�T�B

  Returns:      �S�ẴX���b�g���������؂�ւ������TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_CommitWram(DSPProcessContext *context, MIWramPos wram, int segment, MIWramProc to)
{
    BOOL    retval = FALSE;
    int     slot = DSP_GetProcessSlotFromSegment(context, wram, segment);
    // ���݊��蓖�Ă��Ă���v���Z�b�T����ؒf�B
    if (!MI_IsWramSlotUsed(wram, slot) ||
        MI_FreeWramSlot(wram, slot, MI_WRAM_SIZE_32KB, MI_GetWramBankMaster(wram, slot)) > 0)
    {
        // �w��̃v���Z�b�T�֊��蓖�āB
        void   *physicalAddr = (void *)MI_AllocWramSlot(wram, slot, MI_WRAM_SIZE_32KB, to);
        if (physicalAddr != 0)
        {
            // �v���Z�b�T���Ƃɏ���̃I�t�Z�b�g�ֈړ��B
            vu8    *bank = &((vu8*)((wram == MI_WRAM_B) ? REG_MBK_B0_ADDR  : REG_MBK_C0_ADDR))[slot];
            if (to == MI_WRAM_ARM9)
            {
                *bank = (u8)((*bank & ~MI_WRAM_OFFSET_MASK_B) | (slot << MI_WRAM_OFFSET_SHIFT_B));
            }
            else if (to == MI_WRAM_DSP)
            {
                *bank = (u8)((*bank & ~MI_WRAM_OFFSET_MASK_B) | (segment << MI_WRAM_OFFSET_SHIFT_B));
            }
            retval = TRUE;
        }
    }
    return retval;
}


/*---------------------------------------------------------------------------*
  Name:         DSPi_MapAndLoadProcessImageCallback

  Description:  �v���Z�X�C���[�W��1�p�X�Ń}�b�v�����[�h����R�[���o�b�N�B

  Arguments:    context : DSPProcessContext�\���́B
                header  : COFF�t�@�C���w�b�_���B
                section : �񋓂��ꂽ�Z�N�V�����B

  Returns:      �񋓂��p������Ȃ�TRUE�A�I�����Ă悢�Ȃ�FALSE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_MapAndLoadProcessImageCallback(DSPProcessContext *context,
                                                const COFFFileHeader *header,
                                                const COFFSectionTable *section)
{
    BOOL        retval = TRUE;

    // �P��Z�N�V������CODE/DATA���킹�čő�4�ӏ��ɔz�u�����\�������邽�߁A
    // �K�v�Ȕz�u��������X�g�A�b�v���Ă���܂Ƃ߂Ĕz�u����B
    enum
    {
        placement_kind_max = 2, // { CODE, DATA }
        placement_code_page_max = 2,
        placement_data_page_max = 2,
        placement_max = placement_code_page_max + placement_data_page_max
    };
    MIWramPos   wrams[placement_max];
    int         addrs[placement_max];
    BOOL        nolds[placement_max];
    int         placement = 0;

    // �z�u�����ׂ��y�[�W�ԍ��̓Z�N�V�������̐ڔ��q���画�肷�邱�Ƃ��ł���B
    // Name�t�B�[���h��������𒼐ڂ���킷�ꍇ�͈�ӏ��ɂ̂ݔz�u�����悤����
    // ������e�[�u������Q�Ƃ��ׂ��������O�Ȃ畡���ӏ��֔z�u�����\��������B
    const char *name = (const char *)section->Name;
    char        longname[128];
    if (*(u32*)name == 0)
    {
        u32     stringtable = header->PointerToSymbolTable + 0x12 * header->NumberOfSymbols;
        if(!DSPi_ReadProcessImage(context,
                              (int)(stringtable + *(u32*)&section->Name[4]),
                              longname, sizeof(longname)))
        {
            retval = FALSE;
            return retval;
        }
        name = longname;
    }

    // ����Ȗڈ�ƂȂ�Z�N�V�����̑��݂������Ŕ��肷��B
    if (STD_CompareString(name, "SDK_USING_OS@d0") == 0)
    {
        context->flags |= DSP_PROCESS_FLAG_USING_OS;
    }

    // CODE�Z�N�V������WRAM-B�B
    if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0)
    {
        int         baseaddr = (int)(section->s_paddr * 2);
        BOOL        noload = ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_CODE_MEMORY) != 0);
        if (STD_StrStr(name, "@c0") != NULL)
        {
            wrams[placement] = MI_WRAM_B;
            addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 0;
            nolds[placement] = noload;
            ++placement;
        }
        if (STD_StrStr(name, "@c1") != NULL)
        {
            wrams[placement] = MI_WRAM_B;
            addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 1;
            nolds[placement] = noload;
            ++placement;
        }
    }

    // DATA�Z�N�V������WRAM-C�B
    if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) != 0)
    {
        int         baseaddr = (int)(section->s_vaddr * 2);
        BOOL        noload = ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_DATA_MEMORY) != 0);
        if (STD_StrStr(name, "@d0") != NULL)
        {
            wrams[placement] = MI_WRAM_C;
            addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 0;
            nolds[placement] = noload;
            ++placement;
        }
        if (STD_StrStr(name, "@d1") != NULL)
        {
            wrams[placement] = MI_WRAM_C;
            addrs[placement] = baseaddr + DSP_WRAM_SLOT_SIZE * 4 * 1;
            nolds[placement] = noload;
            ++placement;
        }
    }

    // �z�u���񂪃��X�g�A�b�v�ł����̂Ŋe�A�h���X�֔z�u�B
    {
        int     i;
        for (i = 0; i < placement; ++i)
        {
            MIWramPos   wram = wrams[i];
            int     dstofs = addrs[i];
            int     length = (int)section->s_size;
            int     srcofs = (int)section->s_scnptr;
            // �X���b�g���E���܂����Ȃ��悤�������[�h�B
            while (length > 0)
            {
                // �X���b�g���E�ŃT�C�Y���N���b�v����B
                int     ceil = MATH_ROUNDUP(dstofs + 1, DSP_WRAM_SLOT_SIZE);
                int     curlen = MATH_MIN(length, ceil - dstofs);
                BOOL    newmapped = FALSE;
                // �Y������Z�O�����g���܂��}�b�v����Ă��Ȃ���΂����Ń}�b�v���ď������B
                if (DSP_GetProcessSlotFromSegment(context, wram, dstofs / DSP_WRAM_SLOT_SIZE) == -1)
                {
                    int     segment = (dstofs / DSP_WRAM_SLOT_SIZE);
                    u16    *slots = (wram == MI_WRAM_B) ? &context->slotB : &context->slotC;
                    int    *segbits = (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
                    int    *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode : context->slotOfSegmentData;
                    int     slot = (int)MATH_CountTrailingZeros((u32)*slots);
                    if (slot >= MI_WRAM_B_MAX_NUM)
                    {
                        retval = FALSE;
                        break;
                    }
                    else
                    {
                        newmapped = TRUE;
                        map[segment] = slot;
                        *slots &= ~(1 << slot);
                        *segbits |= (1 << segment);
                        if (!DSPi_CommitWram(context, wram, segment, MI_WRAM_ARM9))
                        {
                            retval = FALSE;
                            break;
                        }
                    }
                    MI_CpuFillFast(DSP_ConvertProcessAddressFromDSP(context, wram,
                                                                    segment * (DSP_WRAM_SLOT_SIZE / 2)),
                                   0, DSP_WRAM_SLOT_SIZE);
                }
                // noload�w��Ȃ疳���B
                if (nolds[i])
                {
                    DSP_DPRINTF("$%04X (noload)\n", dstofs);
                }
                else
                {
                    // ����̓]���͈͂ɊY������I�t�Z�b�g���v�Z�B
                    u8     *dstbuf = (u8*)DSP_ConvertProcessAddressFromDSP(context, wram, dstofs / 2);
                    if (!DSPi_ReadProcessImage(context, srcofs, dstbuf, curlen))
                    {
                        retval = FALSE;
                        break;
                    }
                    DSP_DPRINTF("$%04X -> mem:%08X\n", dstofs, dstbuf);
                }
                srcofs += curlen;
                dstofs += curlen;
                length -= curlen;
            }
        }
    }

    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_InitProcessContext

  Description:  �v���Z�X���\���̂��������B

  Arguments:    context : DSPProcessContext�\���́B
                name    : �v���Z�X���܂���NULL�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_InitProcessContext(DSPProcessContext *context, const char *name)
{
    int     i;
    int     segment;
    MI_CpuFill8(context, 0, sizeof(*context));
    context->next = NULL;
    context->flags = 0;
    (void)STD_CopyString(context->name, name ? name : "");
    context->image = NULL;
    // �Z�O�����g�}�b�v����ɁB
    context->segmentCode = 0;
    context->segmentData = 0;
    // �X���b�g�}�b�v����ɁB
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        context->slotOfSegmentCode[segment] = -1;
    }
    for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment)
    {
        context->slotOfSegmentData[segment] = -1;
    }
    // �t�b�N�o�^�󋵂���ɁB
    context->hookFactors = 0;
    for (i = 0; i < DSP_HOOK_MAX; ++i)
    {
        context->hookFunction[i] = NULL;
        context->hookUserdata[i] = NULL;
        context->hookGroup[i] = 0;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_EnumSections

  Description:  �v���Z�X�C���[�W���̃Z�N�V������񋓂��ď���̏��������s�B

  Arguments:    context  : �R�[���o�b�N�����ɓn��DSPProcessContext�\���́B
                callback : �e�Z�N�V�����ɑ΂��ČĂяo�����R�[���o�b�N�B

  Returns:      �S�ẴZ�N�V�������񋓂�����TRUE�A�r���ŏI��������FALSE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_EnumSections(DSPProcessContext *context, DSPSectionEnumCallback callback)
{
    BOOL            retval = FALSE;
    // �C���[�W�o�b�t�@�̃��������E���l�����Ĉ�x�e���|�����փR�s�[�B
    COFFFileHeader  header[1];
    if (DSPi_ReadProcessImage(context, 0, header, sizeof(header)))
    {
        int     base = (int)(sizeof(header) + header->SizeOfOptionalHeader);
        int     index;
        for (index = 0; index < header->NumberOfSections; ++index)
        {
            COFFSectionTable    section[1];
            if (!DSPi_ReadProcessImage(context, (int)(base + index * sizeof(section)), section, (int)sizeof(section)))
            {
                break;
            }
            // BLOCK-HEADER �����������T�C�Y1�ȏ�̃Z�N�V����������I�ʂ���B
            if (((section->s_flags & COFF_SECTION_ATTR_BLOCK_HEADER) == 0) && (section->s_size != 0))
            {
                if (callback && !(*callback)(context, header, section))
                {
                    break;
                }
            }
        }
        retval = (index >= header->NumberOfSections);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_AttachProcessMemory

  Description:  �v���Z�X�̖��g�p�̈�ɘA��������������Ԃ����蓖�Ă�B

  Arguments:    context : DSPProcessContext�\���́B
                wram    : ���蓖�Ă郁������ԁB (MI_WRAM_B/MI_WRAM_C)
                slots   : ���蓖�Ă�WRAM�X���b�g�B

  Returns:      ���蓖�Ă�ꂽDSP��������Ԃ̐擪�A�h���X�܂���0�B
 *---------------------------------------------------------------------------*/
u32 DSP_AttachProcessMemory(DSPProcessContext *context, MIWramPos wram, int slots)
{
    u32     retval = 0;
    int    *segbits = (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
    int    *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode : context->slotOfSegmentData;
    // �K�v�ȕ������̘A���������g�p�̈�������B
    int     need = (int)MATH_CountPopulation((u32)slots);
    u32     region = (u32)((1 << need) - 1);
    u32     space = (u32)(~*segbits & 0xFF);
    int     segment = 0;
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        // DSP�̃y�[�W���E(4�Z�O�����g)�ɂ͗l�X�Ȑ���������̂�
        // ������܂����Ȃ��悤�Ɋm�ۂ���悤���ӁB
        if ((((segment ^ (segment + need - 1)) & 4) == 0) &&
            (((space >> segment) & region) == region))
        {
            // �[���Ȗ��g�p�̈悪���݂���Ή��ʏ��ɃX���b�g�����蓖�Ă�B
            retval = (u32)(DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * segment);
            while (slots)
            {
                int     slot = (int)MATH_CountTrailingZeros((u32)slots);
                map[segment] = slot;
                slots &= ~(1 << slot);
                *segbits |= (1 << segment);
                segment += 1;
            }
            break;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_DetachProcessMemory

  Description:  �v���Z�X�̎g�p�̈�Ɋ��蓖�Ă�ꂽWRAM�X���b�g���������B

  Arguments:    context : DSPProcessContext�\���́B
                slots   : ���蓖�čς݂�WRAM�X���b�g�B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_DetachProcessMemory(DSPProcessContext *context, MIWramPos wram, int slots)
{
    int    *segbits = (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
    int    *map = (wram == MI_WRAM_B) ? context->slotOfSegmentCode : context->slotOfSegmentData;
    int     segment;
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        if ((((1 << segment) & *segbits) != 0) && (((1 << map[segment]) & slots) != 0)) 
        {
            *segbits &= ~(1 << segment);
            map[segment] = -1;
        }
    }
}

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
BOOL DSP_SwitchProcessMemory(DSPProcessContext *context, MIWramPos wram, u32 address, u32 length, MIWramProc to)
{
    address = DSP_ADDR_TO_ARM(address);
    length = DSP_ADDR_TO_ARM(MATH_MAX(length, 1));
    {
        int    *segbits = (wram == MI_WRAM_B) ? &context->segmentCode : &context->segmentData;
        int     lower = (int)(address / DSP_WRAM_SLOT_SIZE);
        int     upper = (int)((address + length - 1) / DSP_WRAM_SLOT_SIZE);
        int     segment;
        for (segment = lower; segment <= upper; ++segment)
        {
            if ((*segbits & (1 << segment)) != 0)
            {
                if (!DSPi_CommitWram(context, wram, segment, to))
                {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_MapAndLoadProcessImage

  Description:  �w�肳�ꂽ�v���Z�X�C���[�W��1�p�X�Ń}�b�v�����[�h�B
                �Z�O�����g�}�b�v�͎Z�o����Ă��Ȃ��Ă��悢�B

  Arguments:    context : DSPProcessContext�\���́B
                image   : �v���Z�X�C���[�W���w���t�@�C���n���h���B
                          ���̊֐����ł̂ݎQ�Ƃ����B
                slotB   : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC   : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B

  Returns:      �S�ẴC���[�W�����������[�h������TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL DSP_MapAndLoadProcessImage(DSPProcessContext *context, FSFile *image, int slotB, int slotC)
{
    BOOL    retval = FALSE;
    const u32   dspMemSize = DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * MI_WRAM_B_MAX_NUM;
    // �v���Z�X�C���[�W�����ۂɃ��[�h���Ȃ���WRAM���}�b�s���O�B
    context->image = image;
    context->slotB = (u16)slotB;
    context->slotC = (u16)slotC;

    if (DSP_EnumSections(context, DSPi_MapAndLoadProcessImageCallback))
    {
        DC_FlushRange((const void*)MI_GetWramMapStart_B(), MI_WRAM_B_SIZE);
        DC_FlushRange((const void*)MI_GetWramMapStart_C(), MI_WRAM_C_SIZE);
        // ���蓖�Ă�ꂽWRAM�X���b�g��S��DSP�ɐ؂�ւ��B
        if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize, MI_WRAM_DSP) &&
            DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize, MI_WRAM_DSP))
        {
            retval = TRUE;
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetProcessHook

  Description:  �v���Z�X�ւ�DSP���荞�ݗv���ɑ΂��ăt�b�N��ݒ�B

  Arguments:    context  : DSPProcessContext�\���́B
                factors  : �w�肷�銄�荞�ݗv���̃r�b�g�W���B
                           �r�b�g0-15���Z�}�t�H�A�r�b�g16-18�����v���C�B
                function : �Ăяo���ׂ��t�b�N�֐��B
                userdata : �t�b�N�֐��ɗ^����C�ӂ̃��[�U��`�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SetProcessHook(DSPProcessContext *context, int factors, DSPHookFunction function, void *userdata)
{
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    int     i;
    for (i = 0; i < DSP_HOOK_MAX; ++i)
    {
        int     bit = (1 << i);
        if ((bit & factors) != 0)
        {
            context->hookFunction[i] = function;
            context->hookUserdata[i] = userdata;
            context->hookGroup[i] = factors;
        }
    }
    {
        // ���荞�ݗv����ύX�B
        int         modrep = (((factors >> 16) & 0x7) << REG_DSP_PCFG_PRIE0_SHIFT);
        int         modsem = ((factors >> 0) & 0xFFFF);
        int         currep = reg_DSP_PCFG;
        int         cursem = reg_DSP_PMASK;
        if (function != NULL)
        {
            reg_DSP_PCFG = (u16)(currep | modrep);
            reg_DSP_PMASK = (u16)(cursem & ~modsem);
            context->hookFactors |= factors;
        }
        else
        {
            reg_DSP_PCFG = (u16)(currep & ~modrep);
            reg_DSP_PMASK = (u16)(cursem | modsem);
            context->hookFactors &= ~factors;
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_HookPostStartProcess

  Description:  DSP�v���Z�X�C���[�W�����[�h��������̃t�b�N�B
                DSP�R���|�[�l���g�J���҂��f�o�b�K���N�����邽�߂ɕK�v�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
SDK_WEAK_SYMBOL void DSP_HookPostStartProcess(void)
{
}

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
BOOL DSP_ExecuteProcess(DSPProcessContext *context, FSFile *image, int slotB, int slotC)
{
    BOOL    retval = FALSE;
    if (!FS_IsAvailable())
    {
        OS_TWarning("FS is not initialized yet.\n");
        FS_Init(FS_DMA_NOT_USE);
    }
    // ���ׂẴR���|�[�l���g�ɋ��ʂ�DSP�V�X�e�������������B
    DSP_InitPipe();
    OS_SetIrqFunction(OS_IE_DSP, DSPi_MasterInterrupts);
    DSP_SetProcessHook(context,
                       DSP_HOOK_SEMAPHORE_(15) | DSP_HOOK_REPLY_(2),
                       (DSPHookFunction)DSP_HookPipeNotification, NULL);
    (void)OS_EnableIrqMask(OS_IE_DSP);
    // �v���Z�X�C���[�W���������փ}�b�v���ă��[�h�B
    if (!DSP_MapAndLoadProcessImage(context, image, slotB, slotC))
    {
        OS_TWarning("you should check wram\n");
    }
    else
    {
        OSIntrMode  bak_cpsr = OS_DisableInterrupts();
        // �v���Z�X���X�g�ɃR���e�L�X�g��o�^�B
        DSPProcessContext  **pp = &DSPiCurrentComponent;
        for (pp = &DSPiCurrentComponent; *pp && (*pp != context); pp = &(*pp)->next)
        {
        }
        *pp = context;
        context->image = NULL;
        // �V���b�g�_�E�����̋����I���R�[���o�b�N��ݒ�B
        if (!DSPiShutdownCallbackIsRegistered)
        {
            PM_SetExitCallbackInfo(DSPiShutdownCallbackInfo, DSPi_ShutdownCallback, NULL);
            PMi_InsertPostExitCallbackEx(DSPiShutdownCallbackInfo, PM_CALLBACK_PRIORITY_DSP);
            DSPiShutdownCallbackIsRegistered = TRUE;
        }
        // �X���[�v���̌x���R�[���o�b�N��ݒ�B
        if (!DSPiPreSleepCallbackIsRegistered)
        {
            PM_SetSleepCallbackInfo(DSPiPreSleepCallbackInfo, DSPi_PreSleepCallback, NULL);
            PMi_InsertPreSleepCallbackEx(DSPiPreSleepCallbackInfo, PM_CALLBACK_PRIORITY_DSP);
            DSPiPreSleepCallbackIsRegistered = TRUE;
        }
        // �N������ɂł��邾�������A�K�v�Ȋ��荞�ݗv��������ݒ�B
        DSP_PowerOn();
        DSP_ResetOffEx((u16)(context->hookFactors >> 16));
        DSP_MaskSemaphore((u16)~(context->hookFactors >> 0));
        // �N������̂��̃^�C�~���O��ARM�����ꎞ��~���Ă�����
        // DSP�f�o�b�K�œ����R���|�[�l���g���ă��[�h���ăg���[�X�\�B
        DSP_HookPostStartProcess();
        // ARM9����̃R�}���h��҂���DSP�����玩���I��
        // �R�}���h�𑗐M����悤�ȃR���|�[�l���g�̏ꍇ�A
        // DSP�f�o�b�K�ɂ���čă��[�h���Ă��܂���
        // �R�}���h�l��0�ɏ����������ARM9���ɖ��ǃr�b�g���c���Ă��܂��B
        // ����0�����C�u���������œǂݎ̂Ă�B
        if ((context->flags & DSP_PROCESS_FLAG_USING_OS) != 0)
        {
            u16     id;
            for (id = 0; id < 3; ++id)
            {
                vu16    dummy;
                while (dummy = DSP_RecvDataCore(id), dummy != 1)
                {
                }
            }
        }
        // �N������ݒ�܂ł̊ԂɎ�肱�ڂ������荞�݂��Ȃ����O�̂��ߊm�F�B
        DSPi_MasterInterrupts();
        retval = TRUE;
        (void)OS_RestoreInterrupts(bak_cpsr);
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_QuitProcess

  Description:  DSP�v���Z�X���I�����ă�����������B

  Arguments:    context : ��ԊǗ��Ɏg�p����DSPProcessContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_QuitProcess(DSPProcessContext *context)
{
    OSIntrMode  bak_cpsr;
    
    // �܂��� TEAK �� DMA ���~������
    DSP_StopDSPComponent();
    
    bak_cpsr = OS_DisableInterrupts();
    // DSP���~�B
    DSP_ResetOn();
    DSP_PowerOff();
    // ���荞�݂𖳌����B
    (void)OS_DisableIrqMask(OS_IE_DSP);
    OS_SetIrqFunction(OS_IE_DSP, NULL);
    // �g�p���Ă����S�Ă�WRAM��ԋp�B
    (void)MI_FreeWram(MI_WRAM_B, MI_WRAM_DSP);
    (void)MI_FreeWram(MI_WRAM_C, MI_WRAM_DSP);
    {
        // �v���Z�X���X�g����R���e�L�X�g������B
        DSPProcessContext  **pp = &DSPiCurrentComponent;
        for (pp = &DSPiCurrentComponent; *pp; pp = &(*pp)->next)
        {
            if (*pp == context)
            {
                *pp = (*pp)->next;
                break;
            }
        }
        context->next = NULL;
    }
    (void)context;
    (void)OS_RestoreInterrupts(bak_cpsr);

    // �X���[�v���̌x���R�[���o�b�N���폜�B
    PM_DeletePreSleepCallback(DSPiPreSleepCallbackInfo);
    DSPiPreSleepCallbackIsRegistered = FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_FindProcess

  Description:  ���s���̃v���Z�X�������B

  Arguments:    name : �v���Z�X���B
                       NULL���w�肵����Ō�ɓo�^�����v���Z�X�������B

  Returns:      �w�肵�����O�ɍ��v����DSPProcessContext�\���́B
 *---------------------------------------------------------------------------*/
DSPProcessContext* DSP_FindProcess(const char *name)
{
    DSPProcessContext  *ptr = NULL;
    OSIntrMode  bak_cpsr = OS_DisableInterrupts();
    ptr = DSPiCurrentComponent;
    if (name)
    {
        for (; ptr && (STD_CompareString(ptr->name, name) != 0); ptr = ptr->next)
        {
        }
    }
    (void)OS_RestoreInterrupts(bak_cpsr);
    return ptr;
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
void DSP_ReadProcessPipe(DSPProcessContext *context, int port, void *buffer, u32 length)
{
    DSPPipe input[1];
    (void)DSP_LoadPipe(input, port, DSP_PIPE_INPUT);
    DSP_ReadPipe(input, buffer, (DSPByte)length);
    (void)context;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_WriteProcessPipe

  Description:  �v���Z�X�̏���|�[�g�Ɋ֘A�t����ꂽ�p�C�v�֑��M�B

  Arguments:    context : DSPProcessContext�\���́B
                port    : ���M��̃|�[�g�B
                buffer  : ���M�f�[�^�B
                length  : ���M�T�C�Y�B(�o�C�g�P��)

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_WriteProcessPipe(DSPProcessContext *context, int port, const void *buffer, u32 length)
{
    DSPPipe output[1];
    (void)DSP_LoadPipe(output, port, DSP_PIPE_OUTPUT);
    DSP_WritePipe(output, buffer, (DSPByte)length);
    (void)context;
}


#if defined(DSP_SUPPORT_OBSOLETE_LOADER)
/*---------------------------------------------------------------------------*
 * �ȉ��͌��ݎg�p����Ă��Ȃ��Ǝv����p�~���C���^�t�F�[�X�B
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
  Name:         DSPi_MapProcessSegmentCallback

  Description:  �v���Z�X����L����Z�O�����g�}�b�v���Z�o����R�[���o�b�N�B

  Arguments:    context : DSPProcessContext�\���́B
                header  : COFF�t�@�C���w�b�_���B
                section : �񋓂��ꂽ�Z�N�V�����B

  Returns:      �񋓂��p������Ȃ�TRUE�A�I�����Ă悢�Ȃ�FALSE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_MapProcessSegmentCallback(DSPProcessContext *context,
                                           const COFFFileHeader *header,
                                           const COFFSectionTable *section)
{
    (void)header;
    // TODO: ���Z�������Ƃ�荂���ɂȂ邩������Ȃ��B
    if((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0)
    {
        u32     addr = DSP_ADDR_TO_ARM(section->s_paddr);
        int     lower = (int)(addr / DSP_WRAM_SLOT_SIZE);
        int     upper = (int)((addr + section->s_size - 1) / DSP_WRAM_SLOT_SIZE);
        int     segment;
        for(segment = lower; segment <= upper; ++segment)
        {
            context->segmentCode |= (1 << segment);
        }
    }
    else if((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) != 0)
    {
        u32     addr = DSP_ADDR_TO_ARM(section->s_vaddr);
        int     lower = (int)(addr / DSP_WRAM_SLOT_SIZE);
        int     upper = (int)((addr + section->s_size - 1) / DSP_WRAM_SLOT_SIZE);
        int     segment;
        for(segment = lower; segment <= upper; ++segment)
        {
            context->segmentData |= (1 << segment);
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_MapProcessSlotDefault

  Description:  �󂫔ԍ������ʂ��珇�Ɏg�p���ăX���b�g�}�b�v���������B

  Arguments:    context : DSPProcessContext�\���́B
                slotB   : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC   : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B

  Returns:      �����𖞂����悤�ɃX���b�g�}�b�v���m�ۂł����TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_MapProcessSlotDefault(DSPProcessContext *context, int slotB, int slotC)
{
    BOOL    retval = TRUE;
    int     segment;
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        if ((context->segmentCode & (1 << segment)) != 0)
        {
            int     slot = (int)MATH_CountTrailingZeros((u32)slotB);
            if (slot >= MI_WRAM_B_MAX_NUM)
            {
                retval = FALSE;
                break;
            }
            context->slotOfSegmentCode[segment] = slot;
            slotB &= ~(1 << slot);
        }
    }
    for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment)
    {
        if ((context->segmentData & (1 << segment)) != 0)
        {
            int     slot = (int)MATH_CountTrailingZeros((u32)slotC);
            if (slot >= MI_WRAM_C_MAX_NUM)
            {
                retval = FALSE;
                break;
            }
            context->slotOfSegmentData[segment] = slot;
            slotC &= ~(1 << slot);
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_IsProcessMemoryReady

  Description:  �v���Z�X�Ɋ��蓖�Ă�ꂽ�͂���WRAM�X���b�g���g�p���łȂ����m�F�B

  Arguments:    context : DSPProcessContext�\���́B

  Returns:      �S�Ă�WRAM�X���b�g���g�p���ꂸ�ɏ�����Ԃł����TRUE�B
 *---------------------------------------------------------------------------*/
static BOOL DSP_IsProcessMemoryReady(DSPProcessContext *context)
{
    BOOL    retval = TRUE;
    int     segment;
    for (segment = 0; segment < MI_WRAM_B_MAX_NUM; ++segment)
    {
        if ((context->segmentCode & (1 << segment)) != 0)
        {
            int     slot = context->slotOfSegmentCode[segment];
            if (MI_IsWramSlotUsed(MI_WRAM_B, slot))
            {
                OS_TWarning("slot:%d for DSP:%05X is now used by someone.\n", slot, segment * DSP_WRAM_SLOT_SIZE);
                retval = FALSE;
                break;
            }
        }
    }
    for (segment = 0; segment < MI_WRAM_C_MAX_NUM; ++segment)
    {
        if ((context->segmentData & (1 << segment)) != 0)
        {
            int     slot = context->slotOfSegmentData[segment];
            if (MI_IsWramSlotUsed(MI_WRAM_C, slot))
            {
                OS_TWarning("slot:%d for DSP:%05X is now used by someone.\n", slot, segment * DSP_WRAM_SLOT_SIZE);
                retval = FALSE;
                break;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_LoadProcessImageCallback

  Description:  �v���Z�X�C���[�W���O���[�v�����Ń��[�h�B

  Arguments:    context : DSPProcessContext�\���́B
                header  : COFF�t�@�C���w�b�_���B
                section : �񋓂��ꂽ�Z�N�V�����B

  Returns:      �񋓂��p������Ȃ�TRUE�A�I�����Ă悢�Ȃ�FALSE�B
 *---------------------------------------------------------------------------*/
static BOOL DSPi_LoadProcessImageCallback(DSPProcessContext *context,
                                          const COFFFileHeader *header,
                                          const COFFSectionTable *section)
{
    BOOL        retval = TRUE;
    MIWramPos   wram = MI_WRAM_A;
    int         dstofs = 0;
    BOOL        noload = FALSE;
    (void)header;
    // CODE�Z�N�V������WRAM-B�B
    if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_CODE_MEMORY) != 0)
    {
        wram = MI_WRAM_B;
        dstofs = (int)(section->s_paddr * 2);
        if ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_CODE_MEMORY) != 0)
        {
            noload = TRUE;
        }
    }
    // DATA�Z�N�V������WRAM-C�B
    else if ((section->s_flags & COFF_SECTION_ATTR_MAPPED_IN_DATA_MEMORY) != 0)
    {
        wram = MI_WRAM_C;
        dstofs = (int)(section->s_vaddr * 2);
        if ((section->s_flags & COFF_SECTION_ATTR_NOLOAD_FOR_DATA_MEMORY) != 0)
        {
            noload = TRUE;
        }
    }
    // �Z�O�����g���ƂɓK�؂�WRAM�X���b�g��I���B
    // (�u�ǂ�WRAM�ł��Ȃ��v�Ƃ����萔�����݂��Ȃ��̂�WRAM-A���p)
    if (wram != MI_WRAM_A)
    {
        // noload�w��Ȃ疳���B
        if (noload)
        {
            DSP_DPRINTF("$%04X (noload)\n", dstofs);
        }
        else
        {
            // �X���b�g���E���܂����Ȃ��悤�������[�h�B
            int     length = (int)section->s_size;
            int     srcofs = (int)section->s_scnptr;
            while (length > 0)
            {
                // �X���b�g���E�ŃT�C�Y���N���b�v����B
                int     ceil = MATH_ROUNDUP(dstofs + 1, DSP_WRAM_SLOT_SIZE);
                int     curlen = MATH_MIN(length, ceil - dstofs);
                // ����̓]���͈͂ɊY������I�t�Z�b�g���v�Z�B
                u8     *dstbuf = (u8*)DSP_ConvertProcessAddressFromDSP(context, wram, dstofs/2);
                if (!DSPi_ReadProcessImage(context, srcofs, dstbuf, length))
                {
                    retval = FALSE;
                    break;
                }
                DSP_DPRINTF("$%04X -> mem:%08X\n", dstofs, dstbuf);
                srcofs += curlen;
                dstofs += curlen;
                length -= curlen;
            }
        }
    }
    return retval;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_MapProcessSegment

  Description:  �v���Z�X����L����Z�O�����g�}�b�v���Z�o�B

  Arguments:    context : DSPProcessContext�\���́B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_MapProcessSegment(DSPProcessContext *context)
{
    (void)DSP_EnumSections(context, DSPi_MapProcessSegmentCallback);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadProcessImage

  Description:  �w�肳�ꂽ�v���Z�X�C���[�W�����[�h����B
                �Z�O�����g�}�b�v�͂��łɎZ�o�ς݂łȂ���΂Ȃ�Ȃ��B

  Arguments:    context  : DSPProcessContext�\���́B

  Returns:      �S�ẴC���[�W�����������[�h������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSP_LoadProcessImage(DSPProcessContext *context)
{
    BOOL    retval = FALSE;
    // DSP�Ɋ��蓖�Ă�ꂽ�͂���WRAM�X���b�g���{���ɗ��p�\���m�F�B
    if (DSP_IsProcessMemoryReady(context))
    {
        const u32   dspMemSize = DSP_ADDR_TO_DSP(DSP_WRAM_SLOT_SIZE) * MI_WRAM_B_MAX_NUM;
        // ���蓖�Ă�ꂽWRAM�X���b�g��S��ARM9�ɐ؂�ւ��B
        if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize, MI_WRAM_ARM9) &&
            DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize, MI_WRAM_ARM9))
        {
            // �v���Z�X�C���[�W�����ۂɃ��[�h����WRAM��Flush�B
            if (DSP_EnumSections(context, DSPi_LoadProcessImageCallback))
            {
                DC_FlushRange((const void*)MI_GetWramMapStart_B(), MI_WRAM_B_SIZE);
                DC_FlushRange((const void*)MI_GetWramMapStart_C(), MI_WRAM_C_SIZE);
                // ���蓖�Ă�ꂽWRAM�X���b�g��S��DSP�ɐ؂�ւ��B
                if (DSP_SwitchProcessMemory(context, MI_WRAM_B, 0, dspMemSize, MI_WRAM_DSP) &&
                    DSP_SwitchProcessMemory(context, MI_WRAM_C, 0, dspMemSize, MI_WRAM_DSP))
                {
                    retval = TRUE;
                }
            }
        }
    }
    return retval;
}

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
                        int slotB, int slotC, BOOL (*slotMapper)(DSPProcessContext *, int, int))
{
    BOOL    retval = FALSE;
    if (!slotMapper)
    {
        slotMapper = DSPi_MapProcessSlotDefault;
    }
    if (!FS_IsAvailable())
    {
        OS_TWarning("FS is not initialized yet.\n");
        FS_Init(FS_DMA_NOT_USE);
    }
    context->image = image;
    DSP_MapProcessSegment(context);
    if (!(*slotMapper)(context, slotB, slotC) ||
        !DSP_LoadProcessImage(context))
    {
        OS_TWarning("you should check wram\n");
    }
    else
    {
        retval = TRUE;
    }
    context->image = NULL;
    return retval;
}


#endif

/*---------------------------------------------------------------------------*
  End of file
 *---------------------------------------------------------------------------*/
