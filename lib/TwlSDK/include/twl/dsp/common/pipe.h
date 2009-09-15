/*---------------------------------------------------------------------------*
  Project:  TwlSDK - include - dsp - common
  File:     pipe.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-10-21#$
  $Rev: 9018 $
  $Author: kitase_hirotake $
 *---------------------------------------------------------------------------*/
#ifndef TWL_DSP_PIPE_H_
#define TWL_DSP_PIPE_H_


#include <twl/dsp/common/byteaccess.h>


#ifdef __cplusplus
extern "C" {
#endif


/*---------------------------------------------------------------------------*/
/* constants */

// IN/OUT��`�p�̒萔�B(�����I�� 0 : DSP->ARM, 1 : ARM->DSP �ƒ�߂Ă���)
#ifdef SDK_TWL
#define DSP_PIPE_INPUT      0
#define DSP_PIPE_OUTPUT     1
#else
#define DSP_PIPE_INPUT      1
#define DSP_PIPE_OUTPUT     0
#endif
#define DSP_PIPE_PEER_MAX   2

// �V�X�e�����\�[�X
#define DSP_PIPE_PORT_MAX               8   // �g�p�\�ȃp�C�v�̍ő�|�[�g��
#define DSP_PIPE_DEFAULT_BUFFER_LENGTH  256 // �f�t�H���g�̃����O�o�b�t�@�T�C�Y

// ��`�ς݃|�[�g
#define DSP_PIPE_CONSOLE    0   // DSP->ARM: �f�o�b�O�R���\�[��
#define DSP_PIPE_DMA        1   // DSP<->ARM: �[��DMA
#define DSP_PIPE_AUDIO      2   // DSP<->ARM: �I�[�f�B�I�ėp�ʐM
#define DSP_PIPE_BINARY     3   // DSP<->ARM: �ėp�o�C�i��
#define DSP_PIPE_EPHEMERAL  4   // DSP_CreatePipe()�Ŋm�ۉ\�ȋ󂫗̈�

#define DSP_PIPE_FLAG_INPUT     0x0000  // ���͑�
#define DSP_PIPE_FLAG_OUTPUT    0x0001  // �o�͑�
#define DSP_PIPE_FLAG_PORTMASK  0x00FF  // �|�[�g�ԍ����t�B�[���h
#define DSP_PIPE_FLAG_BOUND     0x0100  // Open����Ă���
#define DSP_PIPE_FLAG_EOF       0x0200  // EOF

#define DSP_PIPE_FLAG_EXIT_OS   0x8000  // DSP��AHB�}�X�^�I������

#define DSP_PIPE_COMMAND_REGISTER   2

// DSP�t�@�C��IO�p�̃R�}���h�\���́B
#define DSP_PIPE_IO_COMMAND_OPEN    0
#define DSP_PIPE_IO_COMMAND_CLOSE   1
#define DSP_PIPE_IO_COMMAND_SEEK    2
#define DSP_PIPE_IO_COMMAND_READ    3
#define DSP_PIPE_IO_COMMAND_WRITE   4
#define DSP_PIPE_IO_COMMAND_MEMMAP  5

#define DSP_PIPE_IO_MODE_R          0x0001
#define DSP_PIPE_IO_MODE_W          0x0002
#define DSP_PIPE_IO_MODE_RW         0x0004
#define DSP_PIPE_IO_MODE_TRUNC      0x0008
#define DSP_PIPE_IO_MODE_CREATE     0x0010

#define DSP_PIPE_IO_SEEK_SET        0
#define DSP_PIPE_IO_SEEK_CUR        1
#define DSP_PIPE_IO_SEEK_END        2


/*---------------------------------------------------------------------------*/
/* declarations */

// �p�C�v�\���́B
// DSP����ARM�ւ̒��ڃA�N�Z�X��i�����݂��Ȃ�����
// ��{�I��ARM����APBP-FIFO��DSP���̃o�b�t�@�𐧌䂷��B
// ���Ɏw�肪������΃����O�o�b�t�@�Ƃ��Ďg�p����B
typedef struct DSPPipe
{
    DSPAddr address;    // �o�b�t�@�̐擪�A�h���X
    DSPByte length;     // �o�b�t�@�T�C�Y
    DSPByte rpos;       // ���ǐ擪�̈�
    DSPByte wpos;       // �ǋL�I�[�̈�
    u16     flags;      // �����t���O
}
DSPPipe;

// DSP���ێ�����ARM���Q�Ƃ���p�C�v���B
typedef struct DSPPipeMonitor
{
    DSPPipe pipe[DSP_PIPE_PORT_MAX][DSP_PIPE_PEER_MAX];
}
DSPPipeMonitor;

// DSP�p�C�v�ʐM�n���h���B
typedef void (*DSPPipeCallback)(void *userdata, int port, int peer);


/*---------------------------------------------------------------------------*/
/* functions */

/*---------------------------------------------------------------------------*
  Name:         DSP_InitPipe

  Description:  DSP�p�C�v�ʐM�̏������B
                DSP�R�}���h���v���C���W�X�^2���L����B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_InitPipe(void);

/*---------------------------------------------------------------------------*
  Name:         DSP_SetPipeCallback

  Description:  DSP�p�C�v�ʐM�̃R�[���o�b�N��ݒ�B

  Arguments:    port     : �p�C�v�̃|�[�g�ԍ��B
                callback : Readable/Writable�C�x���g���̃R�[���o�b�N�B
                userdata : �C�ӂ̃��[�U��`�����B

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SetPipeCallback(int port, void (*callback)(void *, int, int), void *userdata);

/*---------------------------------------------------------------------------*
  Name:         DSP_LoadPipe

  Description:  DSP�p�C�v�������[�h�B

  Arguments:    pipe : �p�C�v���̊i�[�� (DSP���ł�NULL�ł悢)
                port : �p�C�v�̃|�[�g�ԍ�
                peer : DSP_PIPE_INPUT �܂��� DSP_PIPE_OUTPUT

  Returns:      ���[�h�����p�C�v���̃|�C���^
 *---------------------------------------------------------------------------*/
DSPPipe* DSP_LoadPipe(DSPPipe *pipe, int port, int peer);

/*---------------------------------------------------------------------------*
  Name:         DSP_SyncPipe

  Description:  DSP�p�C�v�����ŐV�̓��e�֍X�V�B

  Arguments:    pipe : �p�C�v���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_SyncPipe(DSPPipe *pipe);

/*---------------------------------------------------------------------------*
  Name:         DSP_FlushPipe

  Description:  DSP�p�C�v�̃X�g���[�����t���b�V���B

  Arguments:    pipe : �p�C�v���

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_FlushPipe(DSPPipe *pipe);

/*---------------------------------------------------------------------------*
  Name:         DSP_GetPipeReadableSize

  Description:  �w���DSP�p�C�v���猻�ݓǂݏo���\�ȍő�T�C�Y���擾�B

  Arguments:    pipe : �p�C�v���

  Returns:      ���ݓǂݏo���\�ȍő�T�C�Y�B
 *---------------------------------------------------------------------------*/
u16 DSP_GetPipeReadableSize(const DSPPipe *pipe);

/*---------------------------------------------------------------------------*
  Name:         DSP_GetPipeWritableSize

  Description:  �w���DSP�p�C�v�֌��ݏ������݉\�ȍő�T�C�Y���擾�B

  Arguments:    pipe : �p�C�v���

  Returns:      ���ݏ������݉\�ȍő�T�C�Y�B
 *---------------------------------------------------------------------------*/
u16 DSP_GetPipeWritableSize(const DSPPipe *pipe);

/*---------------------------------------------------------------------------*
  Name:         DSP_ReadPipe

  Description:  DSP�p�C�v�ʐM�|�[�g����f�[�^��ǂݍ��݁B

  Arguments:    pipe   : �p�C�v���
                buffer : �]����o�b�t�@
                length : �]���T�C�Y�B(�������P�ʂ͂��̊��̃��[�h�T�C�Y)
                         ARM����1�o�C�g�P�ʁADSP����2�o�C�g�P�ʂƂȂ�_�ɒ��ӁB

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_ReadPipe(DSPPipe *pipe, void *buffer, u16 length);

/*---------------------------------------------------------------------------*
  Name:         DSP_WritePipe

  Description:  DSP�p�C�v�ʐM�|�[�g�փf�[�^���������݁B

  Arguments:    pipe   : �p�C�v���
                buffer : �]�����o�b�t�@
                length : �]���T�C�Y�B(�������P�ʂ͂��̊��̃��[�h�T�C�Y)
                         ARM����1�o�C�g�P�ʁADSP����2�o�C�g�P�ʂƂȂ�_�ɒ��ӁB
  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_WritePipe(DSPPipe *pipe, const void *buffer, u16 length);

/*---------------------------------------------------------------------------*
  Name:         DSP_HookPipeNotification

  Description:  DSP���荞�ݓ��ŌĂяo���ׂ��p�C�v�ʒm�t�b�N�B

  Arguments:    None.

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_HookPipeNotification(void);

#ifdef SDK_TWL

#else

/*---------------------------------------------------------------------------*
  Name:         DSP_Printf

  Description:  DSP�p�C�v�̃R���\�[���|�[�g�֕�������o�́B

  Arguments:    format : ����������

  Returns:      None.
 *---------------------------------------------------------------------------*/
void DSP_Printf(const char *format, ...);

int DSP_OpenFile(const char *path, int mode);
int DSP_OpenMemoryFile(DSPAddrInARM address, DSPWord32 length);
void DSP_CloseFile(int port);
s32 DSP_GetFilePosition(int port);
s32 DSP_GetFileLength(int port);
s32 DSP_SeekFile(int port, s32 offset, int whence);
s32 DSP_ReadFile(int port, void *buffer, DSPWord length);
s32 DSP_WriteFile(int port, const void *buffer, DSPWord length);

// C�W���֐��̒u��������B
#if 0
typedef void FILE;
#define fopen(path, mode)           (FILE*)DSP_OpenFile(path, mode)
#define fclose(f)                   DSP_CloseFile((int)f)
#define fseek(f, ofs, whence)       DSP_SeekFile((int)f, ofs, whence)
#define fread(buf, len, unit, f)    DSP_ReadFile((int)f, buf, (len) * (unit))
#define fwrite(buf, len, unit, f)   DSP_WriteFile((int)f, buf, (len) * (unit))
#define rewind(f)                   (void)DSP_SeekFile((int)f, 0, DSP_PIPE_IO_SEEK_SET)
#define ftell(f)                    (void)DSP_SeekFile((int)f, 0, DSP_PIPE_IO_SEEK_CUR)
#define fgetpos(f, ppos)            (((*(ppos) = ftell((int)f)) != -1) ? 0 : -1)
#define fsetpos(f, ppos)            fseek((int)f, *(ppos), DSP_PIPE_IO_SEEK_SET)
#endif

#endif


/*===========================================================================*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TWL_DSP_PIPE_H_ */
