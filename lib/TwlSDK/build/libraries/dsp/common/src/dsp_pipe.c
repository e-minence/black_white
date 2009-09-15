/*---------------------------------------------------------------------------*
  Project:  TwlSDK - library - dsp
  File:     dsp_pipe.c

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


/*---------------------------------------------------------------------------*/
/* variables */

// �p�C�v����DSP���x�[�X�A�h���X�B
static DSPAddrInARM     DSPiPipeMonitorAddress = 0;
static OSThreadQueue    DSPiBlockingQueue[1];
static DSPPipe          DSPiDefaultPipe[DSP_PIPE_PORT_MAX][DSP_PIPE_PEER_MAX];
static DSPPipeCallback  DSPiCallback[DSP_PIPE_PORT_MAX];
static void*           (DSPiCallbackArgument[DSP_PIPE_PORT_MAX]);

// �t�@�C��IO�����X���b�h�\���́B
typedef struct DSPFileIOContext
{
    BOOL            initialized;
    OSThread        th[1];
    OSMessage       msga[1];
    OSMessageQueue  msgq[1];
    volatile int    pollbits;
    FSFile          file[DSP_PIPE_PORT_MAX][1];
    u8              stack[4096];
}
DSPFileIOContext;

static DSPFileIOContext DSPiThread[1];


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DSPi_FileIOProc

  Description:  �t�@�C��IO�����v���V�[�W���B

  Arguments:    arg : DSPFileIOThread�\����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_FileIOProc(void *arg)
{
    DSPFileIOContext   *ctx = (DSPFileIOContext *)arg;
    for (;;)
    {
        // �K�v�Ȃ�ʒm��҂��A�v������Ă���|�[�g��1�擾����B
        OSIntrMode  bak = OS_DisableInterrupts();
        int         port;
        for (;;)
        {
            port = (int)MATH_CTZ((u32)ctx->pollbits);
            if (port < DSP_PIPE_PORT_MAX)
            {
                break;
            }
            else
            {
                OSMessage   msg[1];
                (void)OS_ReceiveMessage(ctx->msgq, msg, OS_MESSAGE_BLOCK);
            }
        }
        ctx->pollbits &= ~(1 << port);
        (void)OS_RestoreInterrupts(bak);
        // �Ή�����p�C�v����R�}���h�����[�h����B
        {
            FSFile     *file = ctx->file[port];
            DSPPipe     in[1], out[1];
            u16         command;
            (void)DSP_LoadPipe(in, port, DSP_PIPE_INPUT);
            (void)DSP_LoadPipe(out, port, DSP_PIPE_OUTPUT);
            while (DSP_GetPipeReadableSize(in) >= sizeof(command))
            {
                DSP_ReadPipe(in, &command, sizeof(command));
                switch (command)
                {
                case DSP_PIPE_IO_COMMAND_OPEN:
                    // fopen�R�}���h�B
                    {
                        u16     mode;
                        u16     len;
                        char    path[FS_ENTRY_LONGNAME_MAX + 1];
                        u16     result;
                        DSP_ReadPipe(in, &mode, sizeof(mode));
                        DSP_ReadPipe(in, &len, sizeof(len));
                        len = DSP_WORD_TO_ARM(len);
                        DSP_ReadPipe(in, path, len);
                        path[len] = '\0';
                        (void)FS_OpenFileEx(file, path, mode);
                        result = (u16)(FS_IsFile(file) ? 1 : 0);
                        DSP_WritePipe(out, &result, sizeof(result));
                    }
                    break;
                case DSP_PIPE_IO_COMMAND_MEMMAP:
                    // memmap�R�}���h�B
                    {
                        DSPAddrInARM    addr;
                        DSPAddrInARM    length;
                        u16     result;
                        DSP_ReadPipe(in, &addr, sizeof(addr));
                        DSP_ReadPipe(in, &length, sizeof(length));
                        addr = DSP_32BIT_TO_ARM(addr);
                        length = DSP_32BIT_TO_ARM(length);
                        length = DSP_ADDR_TO_ARM(length);
                        (void)FS_CreateFileFromMemory(file, (void*)addr, length);
                        result = (u16)(FS_IsFile(file) ? 1 : 0);
                        DSP_WritePipe(out, &result, sizeof(result));
                    }
                    break;
                case DSP_PIPE_IO_COMMAND_CLOSE:
                    // fclose�R�}���h�B
                    {
                        u16     result;
                        (void)FS_CloseFile(file);
                        result = 1;
                        DSP_WritePipe(out, &result, sizeof(result));
                    }
                    break;
                case DSP_PIPE_IO_COMMAND_SEEK:
                    // fseek�R�}���h�B
                    {
                        s32     position;
                        u16     whence;
                        s32     result;
                        DSP_ReadPipe(in, &position, sizeof(position));
                        DSP_ReadPipe(in, &whence, sizeof(whence));
                        position = (s32)DSP_32BIT_TO_ARM(position);
                        (void)FS_SeekFile(file, position, (FSSeekFileMode)whence);
                        result = (s32)FS_GetFilePosition(file);
                        result = (s32)DSP_32BIT_TO_DSP(result);
                        DSP_WritePipe(out, &result, sizeof(result));
                    }
                    break;
                case DSP_PIPE_IO_COMMAND_READ:
                    // fread�R�}���h�B
                    {
                        DSPWord length;
                        u32     rest;
                        u16     result;
                        DSP_ReadPipe(in, &length, sizeof(length));
                        length = DSP_WORD_TO_ARM(length);
                        // ���ۂ̃T�C�Y���擾���Ď��O�ɐ�������B
                        rest = FS_GetFileLength(file) - FS_GetFilePosition(file);
                        length = (DSPWord)MATH_MIN(length, rest);
                        result = DSP_WORD_TO_DSP(length);
                        DSP_WritePipe(out, &result, sizeof(result));
                        while (length > 0)
                        {
                            u8      tmp[256];
                            u16     n = (u16)MATH_MIN(length, 256);
                            (void)FS_ReadFile(file, tmp, (s32)n);
                            DSP_WritePipe(out, tmp, n);
                            length -= n;
                        }
                    }
                    break;
                case DSP_PIPE_IO_COMMAND_WRITE:
                    // fwrite�R�}���h�B
                    {
                        DSPWord length;
                        DSP_ReadPipe(in, &length, sizeof(length));
                        length = DSP_WORD_TO_ARM(length);
                        while (length > 0)
                        {
                            u8      tmp[256];
                            u16     n = (u16)MATH_MIN(length, 256);
                            DSP_ReadPipe(in, tmp, (u16)n);
                            (void)FS_WriteFile(file, tmp, (s32)n);
                            length -= n;
                        }
                    }
                    break;
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_NotifyFileIOUpdation

  Description:  �t�@�C��IO�̍X�V�ʒm�B

  Arguments:    port : �X�V�̂������|�[�g�ԍ�

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_NotifyFileIOUpdation(int port)
{
    DSPFileIOContext   *ctx = DSPiThread;
    OSIntrMode          bak = OS_DisableInterrupts();
    // �K�v�Ȃ�X���b�h�𐶐��B
    if (!ctx->initialized)
    {
        int                 i;
        for (i = 0; i < DSP_PIPE_PORT_MAX; ++i)
        {
            FS_InitFile(ctx->file[i]);
        }
        OS_InitMessageQueue(ctx->msgq, ctx->msga, 1);
        ctx->pollbits = 0;
        ctx->initialized = TRUE;
        OS_CreateThread(ctx->th, DSPi_FileIOProc, ctx,
                        &ctx->stack[sizeof(ctx->stack)], sizeof(ctx->stack), 13);
        OS_WakeupThreadDirect(ctx->th);
    }
    // �ʒm�p�̃��b�Z�[�W�𓊂���B
    // ���łɓ��l�̒ʒm�����܂��Ă���΃L���[�ɐς܂Ȃ��Ă����܂�Ȃ��̂�NOBLOCK�B
    ctx->pollbits |= (1 << port);
    (void)OS_SendMessage(ctx->msgq, (OSMessage)port, OS_MESSAGE_NOBLOCK);
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_PipeCallbackToConsole

  Description:  DSP_PIPE_CONSOLE����M�n���h���B

  Arguments:    userdata : �C�ӂ̃��[�U��`����
                port     : �|�[�g�ԍ�
                peer     : ����M����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_PipeCallbackToConsole(void *userdata, int port, int peer)
{
    (void)userdata;
    if (peer == DSP_PIPE_INPUT)
    {
        DSPPipe pipe[1];
        (void)DSP_LoadPipe(pipe, port, peer);
        {
            u16     max = DSP_GetPipeReadableSize(pipe);
            u16     pos = 0;
            while (pos < max)
            {
                enum { tmplen = 128 };
                char    buffer[tmplen + 2];
                u16     length = (u16)((max - pos < tmplen) ? (max - pos) : tmplen);
                DSP_ReadPipe(pipe, buffer, length);
                pos += length;
                // DSP����̃R���\�[���o�͂��f�o�b�O�o�́B
                // �X��printf�o�͂�u16�P�ʂɃp�f�B���O����ē͂�����
                // �s����� '\0' ���}�������_�ɒ��ӁB
                {
                    const char *str = (const char *)buffer;
                    int     current = 0;
                    int     pos;
                    for (pos = 0; pos < length; ++pos)
                    {
                        if (str[pos] == '\0')
                        {
                            OS_TPrintf("%s", &str[current]);
                            current = pos + 1;
                        }
                    }
                    if (current < length)
                    {
                        OS_TPrintf("%.*s", (length - current), &str[current]);
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_PipeCallbackForDMA

  Description:  DSP_PIPE_DMA����M�n���h���B

  Arguments:    userdata : �C�ӂ̃��[�U��`����
                port     : �|�[�g�ԍ�
                peer     : ����M����

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_PipeCallbackForDMA(void *userdata, int port, int peer)
{
    (void)userdata;
    if (peer == DSP_PIPE_INPUT)
    {
        DSPPipe pipe[1];
        (void)DSP_LoadPipe(pipe, port, peer);
        {
            u16     max = DSP_GetPipeReadableSize(pipe);
            u16     pos = 0;
            while (pos < max)
            {
                enum { tmplen = 128 };
                char    buffer[tmplen + 2];
                u16     length = (u16)((max - pos < tmplen) ? (max - pos) : tmplen);
                DSP_ReadPipe(pipe, buffer, length);
                pos += length;
                // DSP����̃R���\�[���o�͂��f�o�b�O�o�́B
                // �X��printf�o�͂�u16�P�ʂɃp�f�B���O����ē͂�����
                // �s����� '\0' ���}�������_�ɒ��ӁB
                {
                    const char *str = (const char *)buffer;
                    int     current = 0;
                    int     pos;
                    for (pos = 0; pos < length; ++pos)
                    {
                        if (str[pos] == '\0')
                        {
                            OS_TPrintf("%s", &str[current]);
                            current = pos + 1;
                        }
                    }
                    if (current < length)
                    {
                        OS_TPrintf("%.*s", (length - current), &str[current]);
                    }
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
  Name:         DSPi_WaitForPipe

  Description:  �w���DSP�p�C�v���ɑ΂���X�V��ҋ@�B

  Arguments:    pipe : �p�C�v���

  Returns:      None.
 *---------------------------------------------------------------------------*/
static void DSPi_WaitForPipe(DSPPipe *pipe)
{
    OS_SleepThread(DSPiBlockingQueue);
    (void)DSP_SyncPipe(pipe);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_InitPipe

  Description:  DSP�p�C�v�ʐM�̏������B
                DSP�R�}���h���v���C���W�X�^2���L����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_InitPipe(void)
{
    int     i;
    DSPiPipeMonitorAddress = 0;
    OS_InitThreadQueue(DSPiBlockingQueue);
    for (i = 0; i < DSP_PIPE_PORT_MAX; ++i)
    {
        DSPiCallback[i] = NULL;
        DSPiCallbackArgument[i] = NULL;
    }
    // DSP_PIPE_CONSOLE �� DSP ���̃f�o�b�O�R���\�[��
    DSPiCallback[DSP_PIPE_CONSOLE] = DSPi_PipeCallbackToConsole;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SetPipeCallback

  Description:  DSP�p�C�v�ʐM�̃R�[���o�b�N��ݒ�B

  Arguments:    port     : �p�C�v�̃|�[�g�ԍ��B
                callback : Readable/Writable�C�x���g���̃R�[���o�b�N�B
                userdata : �C�ӂ̃��[�U��`�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SetPipeCallback(int port, void (*callback)(void *, int, int), void *userdata)
{
    DSPiCallback[port] = callback;
    DSPiCallbackArgument[port] = userdata;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadPipe

  Description:  DSP�p�C�v�������[�h�B

  Arguments:    pipe : �p�C�v���̊i�[�� (DSP���ł�NULL�ł悢)
                port : �p�C�v�̃|�[�g�ԍ�
                peer : DSP_PIPE_INPUT �܂��� DSP_PIPE_OUTPUT

  Returns:      ���[�h�����p�C�v���̃|�C���^
 *---------------------------------------------------------------------------*/
DSPPipe* DSP_LoadPipe(DSPPipe *pipe, int port, int peer)
{
    // ���j�^�A�h���X���ʒm�����܂őҋ@�B
    OSIntrMode  bak = OS_DisableInterrupts();
    while (!DSPiPipeMonitorAddress)
    {
        OS_SleepThread(DSPiBlockingQueue);
    }
    (void)OS_RestoreInterrupts(bak);
    {
        DSPPipeMonitor *monitor = (DSPPipeMonitor *)DSPiPipeMonitorAddress;
        DSPPipe        *target = &monitor->pipe[port][peer];
        if (!pipe)
        {
            pipe = &DSPiDefaultPipe[port][peer];
        }
        DSP_LoadData((DSPAddrInARM)target, pipe, sizeof(*pipe));
    }
    return pipe;
}

/*---------------------------------------------------------------------------*
  Name:         DSP_SyncPipe

  Description:  DSP�p�C�v�����ŐV�̓��e�֍X�V�B

  Arguments:    pipe : �p�C�v���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SyncPipe(DSPPipe *pipe)
{
    (void)DSP_LoadPipe(pipe,
                 ((pipe->flags & DSP_PIPE_FLAG_PORTMASK) >> 1),
                 (pipe->flags & 1));
}

/*---------------------------------------------------------------------------*
  Name:         DSP_FlushPipe

  Description:  DSP�p�C�v�̃X�g���[�����t���b�V���B

  Arguments:    pipe : �p�C�v���
                port : �p�C�v�̃|�[�g�ԍ�
                peer : DSP_PIPE_INPUT �܂��� DSP_PIPE_OUTPUT

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_FlushPipe(DSPPipe *pipe)
{
    // ARM������X�V�����|�C���^������DSP���փ��C�g�o�b�N����B
    int     port = ((pipe->flags & DSP_PIPE_FLAG_PORTMASK) >> 1);
    int     peer = (pipe->flags & 1);
    DSPPipeMonitor *monitor = (DSPPipeMonitor *)DSPiPipeMonitorAddress;
    DSPPipe        *target = &monitor->pipe[port][peer];
    if (peer == DSP_PIPE_INPUT)
    {
        DSP_StoreData((DSPAddrInARM)&target->rpos, &pipe->rpos, sizeof(target->rpos));
    }
    else
    {
        DSP_StoreData((DSPAddrInARM)&target->wpos, &pipe->wpos, sizeof(target->wpos));
    }
    // DSP���֍X�V�ʒm�B
    DSP_SendData(DSP_PIPE_COMMAND_REGISTER, (u16)(pipe->flags & DSP_PIPE_FLAG_PORTMASK));
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetPipeReadableSize

  Description:  �w���DSP�p�C�v���猻�ݓǂݏo���\�ȍő�T�C�Y���擾�B

  Arguments:    pipe : �p�C�v���

  Returns:      ���ݓǂݏo���\�ȍő�T�C�Y�B
 *---------------------------------------------------------------------------*/
u16 DSP_GetPipeReadableSize(const DSPPipe *pipe)
{
    return DSP_BYTE_TO_UNIT(((pipe->wpos - pipe->rpos) +
           (((pipe->rpos ^ pipe->wpos) < 0x8000) ? 0 : pipe->length)) & ~0x8000);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_GetPipeWritableSize

  Description:  �w���DSP�p�C�v�֌��ݏ������݉\�ȍő�T�C�Y���擾�B

  Arguments:    pipe : �p�C�v���

  Returns:      ���ݏ������݉\�ȍő�T�C�Y�B
 *---------------------------------------------------------------------------*/
u16 DSP_GetPipeWritableSize(const DSPPipe *pipe)
{
    return DSP_BYTE_TO_UNIT(((pipe->rpos - pipe->wpos) +
           (((pipe->wpos ^ pipe->rpos) < 0x8000) ? 0 : pipe->length)) & ~0x8000);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_ReadPipe

  Description:  DSP�p�C�v�ʐM�|�[�g����f�[�^��ǂݍ��݁B

  Arguments:    pipe   : �p�C�v���
                buffer : �]����o�b�t�@
                length : �]���T�C�Y�B(�������P�ʂ͂��̊��̃��[�h�T�C�Y)
                         ARM����1�o�C�g�P�ʁADSP����2�o�C�g�P�ʂƂȂ�_�ɒ��ӁB

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_ReadPipe(DSPPipe *pipe, void *buffer, u16 length)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    u8         *dst = (u8 *)buffer;
    BOOL        modified = FALSE;
    length = DSP_UNIT_TO_BYTE(length);
    DSP_SyncPipe(pipe);
    while (length > 0)
    {
        u16     rpos = pipe->rpos;
        u16     wpos = pipe->wpos;
        u16     phase = (u16)(rpos ^ wpos);
        // read-empty�Ȃ犮���҂��B
        if (phase == 0x0000)
        {
            if (modified)
            {
                DSP_FlushPipe(pipe);
                modified = FALSE;
            }
            DSPi_WaitForPipe(pipe);
        }
        else
        {
            // �����łȂ���Έ��S�Ȕ͈͂���ǂݍ��݁B
            u16     pos = (u16)(rpos & ~0x8000);
            u16     end = (u16)((phase < 0x8000) ? (wpos & ~0x8000) : pipe->length);
            u16     len = (u16)((length < end - pos) ? length : (end - pos));
            len = (u16)(len & ~(DSP_WORD_UNIT - 1));
            DSP_LoadData(DSP_ADDR_TO_ARM(pipe->address) + pos, dst, len);
            length -= len;
            dst += DSP_BYTE_TO_UNIT(len);
            pipe->rpos = (u16)((pos + len < pipe->length) ? (rpos + len) : (~rpos & 0x8000));
            modified = TRUE;
        }
    }
    // �ǋL������΂����Œʒm�B
    if (modified)
    {
        DSP_FlushPipe(pipe);
        modified = FALSE;
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_WritePipe

  Description:  DSP�p�C�v�ʐM�|�[�g�փf�[�^���������݁B

  Arguments:    pipe   : �p�C�v���
                buffer : �]�����o�b�t�@
                length : �]���T�C�Y�B(�������P�ʂ͂��̊��̃��[�h�T�C�Y)
                         ARM����1�o�C�g�P�ʁADSP����2�o�C�g�P�ʂƂȂ�_�ɒ��ӁB

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_WritePipe(DSPPipe *pipe, const void *buffer, u16 length)
{
    OSIntrMode  bak = OS_DisableInterrupts();
    const u8   *src = (const u8 *)buffer;
    BOOL        modified = FALSE;
    length = DSP_UNIT_TO_BYTE(length);
    DSP_SyncPipe(pipe);
    while (length > 0)
    {
        u16     rpos = pipe->rpos;
        u16     wpos = pipe->wpos;
        u16     phase = (u16)(rpos ^ wpos);
        // write-full�Ȃ犮���҂��B
        if (phase == 0x8000)
        {
            if (modified)
            {
                DSP_FlushPipe(pipe);
                modified = FALSE;
            }
            DSPi_WaitForPipe(pipe);
        }
        else
        {
            // �����łȂ���Έ��S�Ȕ͈͂֏������݁B
            u16     pos = (u16)(wpos & ~0x8000);
            u16     end = (u16)((phase < 0x8000) ? pipe->length : (rpos & ~0x8000));
            u16     len = (u16)((length < end - pos) ? length : (end - pos));
            len = (u16)(len & ~(DSP_WORD_UNIT - 1));
            DSP_StoreData(DSP_ADDR_TO_ARM(pipe->address) + pos, src, len);
            length -= len;
            src += DSP_BYTE_TO_UNIT(len);
            pipe->wpos = (u16)((pos + len < pipe->length) ? (wpos + len) : (~wpos & 0x8000));
            modified = TRUE;
        }
    }
    // �ǋL������΂����Œʒm�B
    if (modified)
    {
        DSP_FlushPipe(pipe);
        modified = FALSE;
    }
    (void)OS_RestoreInterrupts(bak);
}

/*---------------------------------------------------------------------------*
  Name:         DSP_HookPipeNotification

  Description:  DSP���荞�ݓ��ŌĂяo���ׂ��p�C�v�ʒm�t�b�N�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_HookPipeNotification(void)
{
    // CR2���p�p�C�v�ʐM�Ƃ��Ďg�p�B
    // ���������̂�CR���荞�݂��������Ȃ����߁A
    // ����ł̓Z�}�t�H��1�r�b�g������p�B
    while (((DSP_GetSemaphore() & 0x8000) != 0) || DSP_RecvDataIsReady(DSP_PIPE_COMMAND_REGISTER))
    {
        DSP_ClearSemaphore(0x8000);
        while (DSP_RecvDataIsReady(DSP_PIPE_COMMAND_REGISTER))
        {
            // �����DSP�����狤�L�\���̂̃A�h���X���ʒm�����B
            if (DSPiPipeMonitorAddress == 0)
            {
                DSPiPipeMonitorAddress = DSP_ADDR_TO_ARM(DSP_RecvData(DSP_PIPE_COMMAND_REGISTER));
            }
            // ����ȍ~�́A�X�V���ꂽ�p�C�v�̂ݒʒm�����B
            else
            {
                u16     recvdata = DSP_RecvData(DSP_PIPE_COMMAND_REGISTER);
                int     port = (recvdata >> 1);
                int     peer = (recvdata & 1);
                // ARM���ɊĎ��������݂���Ȃ�p�C�v�����m�F�B
                if (DSPiCallback[port])
                {
                    (*DSPiCallback[port])(DSPiCallbackArgument[port], port, peer);
                }
                else
                {
                    DSPPipe pipe[1];
                    (void)DSP_LoadPipe(pipe, port, peer);
                    // DSP���ŊJ�����t�@�C���̍X�V�Ȃ�X���b�h�֒ʒm�B
                    if ((peer == DSP_PIPE_INPUT) && ((pipe->flags & DSP_PIPE_FLAG_BOUND) != 0))
                    {
                        DSPi_NotifyFileIOUpdation(port);
                    }
                }
            }
            // �u���b�L���O���̃X���b�h�֍X�V�ʒm�B
            OS_WakeupThread(DSPiBlockingQueue);
        }
    }
}
