/*---------------------------------------------------------------------------*
  Project:  TwlSDK - dsp
  File:     dsp_graphics.c

  Copyright 2008-2009 Nintendo.  All rights reserved.

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
#include <twl/dsp/common/graphics.h>

#include "dsp_process.h"

extern const u8 DSPiFirmware_graphics[];

/*---------------------------------------------------------------------------*/
/* variables */

static DSPPipe binOut[1];
static BOOL DSPiGraphicsAvailable = FALSE;
static DSPProcessContext DSPiProcessGraphics[1];

static u16 replyReg;

/* ��������𖞂����Ă��܂��Ă��邩���肷��i�������Ă���Ȃ�΋U�j */
static BOOL CheckLimitation(f32 rx, f32 ry, DSPGraphicsScalingMode mode, u16 src_width);

/*---------------------------------------------------------------------------*/
/* functions */
/*---------------------------------------------------------------------------*
  Name:         CheckLimitation

  Description:  DSP_Scaling*() �̎d�l�ɂ�鐧������𖞂����Ă��邩�ǂ����𔻒肷��
  
  Arguments:    mode : �g��i�k���j����
                rx, ry : ���{���A�c�{��
                src_width : ���͉摜�̕�
  
  Returns:      ��������𖞂����Ă���ꍇ�� FALSE
 *---------------------------------------------------------------------------*/
static BOOL CheckLimitation(f32 rx, f32 ry, DSPGraphicsScalingMode mode, u16 src_width)
{
    // ������̃f�[�^�T�C�Y�����f�[�^�T�C�Y�ȏ�ɂȂ�ꍇ
    if (rx * ry >= 1.0f)
    {
        switch(mode)
        {
        case DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR:
        case DSP_GRAPHICS_SCALING_MODE_BILINEAR:
            if (DSP_CALC_SCALING_SIZE(src_width, rx) * ry >= 8192)
            {
                return FALSE;
            }
            break;
        case DSP_GRAPHICS_SCALING_MODE_BICUBIC:
            if (DSP_CALC_SCALING_SIZE(src_width, rx) * ry >= 4096)
            {
                return FALSE;
            }
            break;
        }
    }
    else    // ������̃f�[�^�T�C�Y�����f�[�^�T�C�Y�����������Ȃ�ꍇ
    {
        switch(mode)
        {
        case DSP_GRAPHICS_SCALING_MODE_N_NEIGHBOR:
        case DSP_GRAPHICS_SCALING_MODE_BILINEAR:
            if (src_width >= 8192)
            {
                return FALSE;
            }
            break;
        case DSP_GRAPHICS_SCALING_MODE_BICUBIC:
            if (src_width >= 4096)
            {
                return FALSE;
            }
            break;
        }
    }
    return TRUE;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_OpenStaticComponentGraphicsCore

  Description:  �O���t�B�b�N�X�R���|�[�l���g�p�̃������t�@�C�����J���B
                �t�@�C���V�X�e���Ɏ��O�ɗp�ӂ��Ă����K�v���Ȃ��Ȃ邪
                �ÓI�������Ƃ��ă����N����邽�߃v���O�����T�C�Y����������B
  
  Arguments:    file : �������t�@�C�����J��FSFile�\���́B
  
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSPi_OpenStaticComponentGraphicsCore(FSFile *file)
{
    extern const u8 DSPiFirmware_graphics[];
    (void)DSPi_CreateMemoryFile(file, DSPiFirmware_graphics);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_GraphicsEvents(viod *userdata)

  Description:  DSP �O���t�B�b�N�X�R���|�[�l���g�̃C�x���g�n���h��

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
static void DSPi_GraphicsEvents(void *userdata)
{
    (void)userdata;
    
    // �񓯊��������s���Ă���Ƃ��́A�I���ʒm�̗L���𒲂ׂ�
    if ( isAsync )
    {
        replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
        if (replyReg == DSP_STATE_SUCCESS)
        {
            isBusy = FALSE;
            isAsync = FALSE;
                
            if ( callBackFunc != NULL)
            {
                callBackFunc();
            }
        }
        else if (replyReg == DSP_STATE_FAIL)
        {
            OS_TWarning("a process on DSP is failed.\n");
            isBusy = FALSE;
            isAsync = FALSE;
        }
        else
        {
            OS_TWarning("unknown error occured.\n");
            isBusy = FALSE;
            isAsync = FALSE;
        }
    }
    else    // ������
    {
//        replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
//        isBusy = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadGraphicsCore

  Description:  �O���t�B�b�N�X�R���|�[�l���g��DSP�փ��[�h�B
  
  Arguments:    file  : �O���t�B�b�N�X�R���|�[�l���g�̃t�@�C���B
                slotB : �R�[�h�������̂��߂Ɏg�p�������ꂽWRAM-B�B
                slotC : �f�[�^�������̂��߂Ɏg�p�������ꂽWRAM-C�B
  
  Returns:      �O���t�B�b�N�X�R���|�[�l���g��������DSP�փ��[�h������TRUE�B
 *---------------------------------------------------------------------------*/
BOOL DSPi_LoadGraphicsCore(FSFile *file, int slotB, int slotC)
{
    if (!DSPiGraphicsAvailable)
    {
        isBusy = FALSE;
        isAsync = FALSE;
        DSP_InitProcessContext(DSPiProcessGraphics, "graphics");
        // (DSP���̃����J�X�N���v�g�t�@�C���Ɏw�肪����΂��̖����ݒ�͕s�v)
        DSPiProcessGraphics->flags |= DSP_PROCESS_FLAG_USING_OS;
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           DSPi_GraphicsEvents, NULL);
        if (DSP_ExecuteProcess(DSPiProcessGraphics, file, slotB, slotC))
        {
            DSPiGraphicsAvailable = TRUE;
        }
        
        (void)DSP_LoadPipe(binOut, DSP_PIPE_BINARY, DSP_PIPE_OUTPUT);
    }
    
    return DSPiGraphicsAvailable;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_UnloadGraphicsCore

  Description:  DSP�̃O���t�B�b�N�X�R���|�[�l���g���I������B

  Arguments:    �Ȃ�

  Returns:      �Ȃ�
 *---------------------------------------------------------------------------*/
void DSPi_UnloadGraphicsCore(void)
{
    if(DSPiGraphicsAvailable)
    {
        DSP_QuitProcess(DSPiProcessGraphics);
        DSPiGraphicsAvailable = FALSE;
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_YuvToRgbConvertCore

  Description:  YUV->RGB �ϊ����s���܂�

  Arguments:    src       : �������f�[�^�A�h���X
                dest      : ������f�[�^�i�[��A�h���X
                size      : src �f�[�^�T�C�Y
                callback  : �ϊ��I����Ɏ��s�����R�[���o�b�N�֐��̃|�C���^
                async     : �񓯊��ŏ��������s����Ȃ� TRUE

  Returns:      �����Ȃ�TRUE
 *---------------------------------------------------------------------------*/
BOOL DSPi_ConvertYuvToRgbCore(const void* src, void* dst, u32 size, DSP_GraphicsCallback callback, BOOL async)
{
    DSPYuv2RgbParam yr_param;
    u32 offset = 0;
    u16 command;
    
    // ������x DSP ���r�W�[��Ԃ��ǂ����m�F����
    if (isBusy)
    {
        OS_TPrintf("dsp is busy.\n");
        return FALSE;
    }
    isBusy = TRUE;
    
    // �R�[���o�b�N�֐��̓o�^
    callBackFunc = callback;
    isAsync = async;
    
    if (async)
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           DSPi_GraphicsEvents, NULL);
    }
    else
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           NULL, NULL);
    }
    
    // DSP �֊J�n���鏈���̓��e��ʒm����
    command = DSP_G_FUNCID_YUV2RGB;
    DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);
    
    // DSP �փp�����[�^���M
    yr_param.size = size;
    yr_param.src = (u32)((u32)src + offset);
    yr_param.dst = (u32)((u32)dst + offset);
    
    DSP_WritePipe(binOut, &yr_param, sizeof(DSPYuv2RgbParam));
    
    if (async)
    {
        return TRUE;
    }
    else
    {
        // DSP ����̃��v���C��҂�
        while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
            OS_Sleep(1);
        }
        replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
        isBusy = FALSE;
        
        if ( replyReg == DSP_STATE_SUCCESS)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    return FALSE;
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_ScalingCore

  Description:  �摜�f�[�^�̊g��k�����������s

  Returns:      �����Ȃ�� TRUE ��Ԃ�
 *---------------------------------------------------------------------------*/
BOOL DSPi_ScalingCore(const void* src, void* dst, u16 img_width, u16 img_height, f32 rx, f32 ry, DSPGraphicsScalingMode mode,
                      u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback, BOOL async)
{
    DSPScalingParam sc_param;
    
    u16 command;
    
    // ��������`�F�b�N
    SDK_TASSERTMSG(CheckLimitation(rx, ry, mode, width), "DSP_Scaling: arguments exceed the limit.");
    
    // ������x DSP ���r�W�[��Ԃ��ǂ����m�F����
    if (isBusy)
    {
        OS_TPrintf("dsp is busy.\n");
        return FALSE;
    }
    isBusy = TRUE;
    
    // �R�[���o�b�N�֐��̓o�^
    callBackFunc = callback;
    isAsync = async;
    if (async)
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           DSPi_GraphicsEvents, NULL);
    }
    else
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           NULL, NULL);
    }
    
    // DSP �֊J�n���鏈���̓��e��ʒm����
    command = DSP_G_FUNCID_SCALING;
    DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);

    // �p�����[�^�� DSP �֓]��
    sc_param.src = (u32)src;
    sc_param.dst = (u32)dst;
    sc_param.mode = mode;
    sc_param.img_width = img_width;
    sc_param.img_height = img_height;
    sc_param.rate_w = (u16)(rx * 1000);
    sc_param.rate_h = (u16)(ry * 1000);
    sc_param.block_x = x;
    sc_param.block_y = y;
    sc_param.width = width;
    sc_param.height = height;

    DSP_WritePipe(binOut, &sc_param, sizeof(DSPScalingParam));
    
    // �񓯊��ŏ��������s���邩�ŕ���
    if(isAsync)
    {
        return TRUE;
    }
    else
    {
        // DSP ����̃��v���C��҂�
        while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
            OS_Sleep(1);
        }
        
        replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
        isBusy = FALSE;
        
        if ( replyReg == DSP_STATE_SUCCESS )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    return FALSE;
}

BOOL DSPi_ScalingFxCore(const void* src, void* dst, u16 img_width, u16 img_height, fx32 rx, fx32 ry, DSPGraphicsScalingMode mode,
                      u16 x, u16 y, u16 width, u16 height, DSP_GraphicsCallback callback, BOOL async)
{
    DSPScalingParam sc_param;
    
    u16 command;
    
    // ��������`�F�b�N
    SDK_TASSERTMSG(CheckLimitation(FX_FX32_TO_F32(rx), FX_FX32_TO_F32(ry), mode, width), "DSP_Scaling: arguments exceed the limit.");
    
    // ������x DSP ���r�W�[��Ԃ��ǂ����m�F����
    if (isBusy)
    {
        OS_TPrintf("dsp is busy.\n");
        return FALSE;
    }
    isBusy = TRUE;
    
    // �R�[���o�b�N�֐��̓o�^
    callBackFunc = callback;
    isAsync = async;
    
    if (async)
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           DSPi_GraphicsEvents, NULL);
    }
    else
    {
        DSP_SetProcessHook(DSPiProcessGraphics,
                           DSP_HOOK_REPLY_(DSP_GRAPHICS_REP_REGISTER),
                           NULL, NULL);
    }
    
    // DSP �֊J�n���鏈���̓��e��ʒm����
    command = DSP_G_FUNCID_SCALING;
    DSP_SendData(DSP_GRAPHICS_COM_REGISTER, command);
    
    // �p�����[�^�� DSP �֓]��
    sc_param.src = (u32)src;
    sc_param.dst = (u32)dst;
    sc_param.mode = mode;
    sc_param.img_width = img_width;
    sc_param.img_height = img_height;
    sc_param.rate_w = (u16)(rx / 4096.0f * 1000);
    sc_param.rate_h = (u16)(ry / 4096.0f * 1000);
    sc_param.block_x = x;
    sc_param.block_y = y;
    sc_param.width = width;
    sc_param.height = height;

    DSP_WritePipe(binOut, &sc_param, sizeof(DSPScalingParam));
    
    // �񓯊��ŏ��������s���邩�ŕ���
    if(isAsync)
    {
        return TRUE;
    }
    else
    {
        // DSP ����̃��v���C��҂�
        while (!DSP_RecvDataIsReady(DSP_GRAPHICS_REP_REGISTER)) {
            OS_Sleep(1);
        }
        replyReg = DSP_RecvData(DSP_GRAPHICS_REP_REGISTER);
        isBusy = FALSE;
        
        if ( replyReg == DSP_STATE_SUCCESS )
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }
    
    return FALSE;
}

