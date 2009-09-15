 /*---------------------------------------------------------------------------*
  Project:  TwlSDK - MB - include
  File:     mb_common.h

  Copyright 2007-2008 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2008-09-18#$
  $Rev: 8573 $
  $Author: okubata_ryoma $
 *---------------------------------------------------------------------------*/

#ifndef __MB_COMMON_H__
#define __MB_COMMON_H__

/* macro definition -------------------------------------------------------- */

// for debug
#ifdef	PRINT_DEBUG
#define MB_COMM_TYPE_OUTPUT             MBi_comm_type_output
#define MB_COMM_WMEVENT_OUTPUT          MBi_comm_wmevent_output
#else
#define MB_COMM_TYPE_OUTPUT( ... )      ((void)0)
#define MB_COMM_WMEVENT_OUTPUT( ... )   ((void)0)
#endif

/* ����M�T�C�Y��` */
#define MB_COMM_P_SENDLEN_DEFAULT       (256)
#define MB_COMM_P_RECVLEN_DEFAULT       (8)

#define MB_COMM_P_SENDLEN_MAX           MB_COMM_PARENT_SEND_MAX
#define MB_COMM_P_RECVLEN_MAX           MB_COMM_PARENT_RECV_MAX

#define MB_COMM_P_SENDLEN_MIN           (sizeof(MBDownloadFileInfo))    // min size
#define MB_COMM_P_RECVLEN_MIN           MB_COMM_PARENT_RECV_MIN

#define MB_COMM_REQ_DATA_SIZE           (29)

/* �f�Љ����N�G�X�g�f�[�^�Ɋւ���v�Z�}�N�� */
#define MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )       (( __P_RECVLEN__ ) - 2)
#define MB_COMM_CALC_REQ_DATA_PIECE_NUM( __P_RECVLEN__ )        \
                                ( (MB_COMM_REQ_DATA_SIZE + 1) / (MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )) )
#define MB_COMM_CALC_REQ_DATA_BUF_SIZE( __P_RECVLEN__ )     (MB_COMM_REQ_DATA_SIZE + 1)
/*
 * ���_���I�ɊԈႢ������܂����AIPL�����̎d�l�œ��삵�Ă��܂��Ă���̂ŁA
 *   �C���͂ł��܂���B �{���Ȃ��
 * #define MB_COMM_CALC_REQ_DATA_PICE_NUM( __P_RECV_LEN__)                                                            \
 *                                  ( (MB_COMM_REQ_DATA_SIZE + MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ ) - 1) \
 *                                    / MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ ) )
 * #define MB_COMM_CALC_REQ_DATA_BUF_SIZE( __P_RECV_LEN__ )                                 \
 *                                  ( MB_COMM_CALC_REQ_DATA_PIECE_SIZE( __P_RECVLEN__ )     \
 *                                    * MB_COMM_CALC_REQ_DATA_PIECE_NUM( __P_RECVLEN__ ) )
 */


/* �u���b�N�w�b�_�̃T�C�Y */
#define MB_COMM_PARENT_HEADER_SIZE      (6)     // MBCommParentBlockHeader�̃T�C�Y(padding����)
#define MB_COMM_CHILD_HEADER_SIZE       (8)     // MBCommChildBlockHeader�̃T�C�Y(padding����)

#define MB_COMM_CALC_BLOCK_SIZE( __P_SENDLEN__ )                (( __P_SENDLEN__ ) - MB_COMM_PARENT_HEADER_SIZE)

/* �f�[�^���M�֐����Ԃ��G���[��`
   WM��ERRCODE�Ɣ��Ȃ��l�Œ�`���Ă��܂��B */
#define MB_SENDFUNC_STATE_ERR           (WM_ERRCODE_MAX + 1)

/* �u���b�N�]���f�[�^�^�C�v */
typedef enum MBCommType
{
    MB_COMM_TYPE_DUMMY = 0,            //  0

    MB_COMM_TYPE_PARENT_SENDSTART,     //  1
    MB_COMM_TYPE_PARENT_KICKREQ,       //  2
    MB_COMM_TYPE_PARENT_DL_FILEINFO,   //  3
    MB_COMM_TYPE_PARENT_DATA,          //  4
    MB_COMM_TYPE_PARENT_BOOTREQ,       //  5
    MB_COMM_TYPE_PARENT_MEMBER_FULL,   //  6

    MB_COMM_TYPE_CHILD_FILEREQ,        //  7
    MB_COMM_TYPE_CHILD_ACCEPT_FILEINFO, //  8
    MB_COMM_TYPE_CHILD_CONTINUE,       //  9
    MB_COMM_TYPE_CHILD_STOPREQ,        //  10
    MB_COMM_TYPE_CHILD_BOOTREQ_ACCEPTED //  11
}
MBCommType;

/* ���[�U�[����̃��N�G�X�g�^ */
typedef enum MBCommUserReq
{
    MB_COMM_USER_REQ_NONE = 0,         //  0
    MB_COMM_USER_REQ_DL_START,         //  1
    MB_COMM_USER_REQ_SEND_START,       //  2
    MB_COMM_USER_REQ_ACCEPT,           //  3
    MB_COMM_USER_REQ_KICK,             //  4
    MB_COMM_USER_REQ_BOOT              //  5
}
MBCommUserReq;

/* structure definition ---------------------------------------------------- */

/*
 * �G���[�ʒm�R�[���o�b�N
 */
typedef struct
{
    u16     apiid;                     // API�R�[�h
    u16     errcode;                   // �G���[�R�[�h
}
MBErrorCallback;


/* functions --------------------------------------------------------------- */

/*
 * �u���b�N�w�b�_����������, �^�C�v�����ݒ肷��.
 * ���̂��� MBi_BlockHeaderEnd() �ő��M����܂ł̊Ԃ�
 * ���܂��܂Ƃ����t�B�[���h�𖄂߂Ă���.
 * �������Ȃ���΂��̂܂܂ŗǂ�.
 */
void    MBi_BlockHeaderBegin(u8 type, u32 *sendbuf);

/*
 * �u���b�N�w�b�_�ݒ��������, �`�F�b�N�T�����v�Z����.
 * ���̌�, pollbmp �Ŏw�肵������֎��ۂɑ��M����.
 */
int     MBi_BlockHeaderEnd(int body_len, u16 pollbmp, u32 *sendbuf);


/*
 * �`�F�b�N�T���v�Z
 */
u16     MBi_calc_cksum(const u16 *buf, int length);

// --- for debug
void    MBi_DebugPrint(const char *file, int line, const char *func, const char *fmt, ...);
void    MBi_comm_type_output(u16 type);
void    MBi_comm_wmevent_output(u16 type, void *arg);

#endif /* __MB_COMMON_H__ */
