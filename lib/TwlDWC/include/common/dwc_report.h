/*---------------------------------------------------------------------------*
  Project:  TwlDWC

  Copyright 2005-2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.
 *---------------------------------------------------------------------------*/

#ifndef DWC_REPORT_H_
#define DWC_REPORT_H_

#ifdef  __cplusplus
extern "C"
{
#endif

    /**
     *  dwc�̃f�o�b�O���o�͗p�t���O
     *
     */
    typedef enum 
    {
        DWC_REPORTFLAG_NONE            = 0x00000000, ///< �o�͂Ȃ�
        DWC_REPORTFLAG_INFO            = 0x00000001, ///< ���
        DWC_REPORTFLAG_ERROR           = 0x00000002, ///< �G���[
        DWC_REPORTFLAG_DEBUG           = 0x00000004, ///< �f�o�b�O���
        DWC_REPORTFLAG_WARNING         = 0x00000008, ///< �x��
        DWC_REPORTFLAG_ACHECK          = 0x00000010, ///< GameSpy�T�[�o�[�̗��p�\�`�F�b�N
        DWC_REPORTFLAG_LOGIN           = 0x00000020, ///< ���O�C���֌W
        DWC_REPORTFLAG_MATCH_NN        = 0x00000040, ///< �}�b�`���O - NN�֌W
        DWC_REPORTFLAG_MATCH_GT2       = 0x00000080, ///< �}�b�`���O - GT2�֌W
        DWC_REPORTFLAG_TRANSPORT       = 0x00000100, ///< ����M�֌W
        DWC_REPORTFLAG_QR2_REQ         = 0x00000200, ///< QR2�֌W
        DWC_REPORTFLAG_SB_UPDATE       = 0x00000400, ///< SB�֌W
        DWC_REPORTFLAG_SEND_INFO       = 0x00008000, ///< ���M���
        DWC_REPORTFLAG_RECV_INFO       = 0x00010000, ///< ��M���
        DWC_REPORTFLAG_UPDATE_SV       = 0x00020000, ///< �T�[�o�X�V�֌W
        DWC_REPORTFLAG_CONNECTINET     = 0x00040000, ///< �C���^�[�l�b�g�ڑ��֌W
        DWC_REPORTFLAG_AUTH            = 0x01000000, ///< �F�؊֌W
        DWC_REPORTFLAG_AC              = 0x02000000, ///< �A�J�E���g�֌W
        DWC_REPORTFLAG_BM              = 0x04000000, ///< WiFi�ݒ�̃Z�[�u�E���[�h
        DWC_REPORTFLAG_UTIL            = 0x08000000, ///< UTIL�֌W
        DWC_REPORTFLAG_OPTION_CF       = 0x10000000, ///< ���g�p
        DWC_REPORTFLAG_OPTION_CONNTEST = 0x20000000, ///< ���g�p
        DWC_REPORTFLAG_GAMESPY         = 0x80000000, ///< Gamespy�֌W
        DWC_REPORTFLAG_TEST            = 0x00100000,
        DWC_REPORTFLAG_ALL             = 0xffffffff  ///< ���ׂ�

    }DWCReportFlag;

#ifndef SDK_FINALROM
    void        DWC_SetReportLevel      ( u32 level );
    void        DWC_Printf              ( DWCReportFlag level, const char* fmt, ... );
#else
#define     DWC_SetReportLevel( level )         ((void)0)
#define     DWC_Printf( level, ... )        ((void)0)
#endif


#ifdef  __cplusplus
}       /* extern "C" */
#endif

#endif // DWC_REPORT_H_
