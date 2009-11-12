/*--------------------------------------------------------------------------
  Project:  dwc_sc
  File:     dwci_sc.h
 
  Copyright 2009 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Date:: 2009/09/28#$
  $Rev:  $
  $Author: niwa_kazutomo $
  *-------------------------------------------------------------------------*/

#ifndef DWC_SC_H_
#define DWC_SC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <dwc.h>
#include <nonport/dwc_nonport.h>

/* -------------------------------------------------------------------------
        define
   ------------------------------------------------------------------------- */

#define DWC_SC_AUTHDATA_SIZE 16
#define DWC_SC_SESSION_GUID_SIZE 40
#define DWC_SC_CONNECTION_GUID_SIZE 40

/* -------------------------------------------------------------------------
        enum
   ------------------------------------------------------------------------- */

/**
 * �G���[����ю��s�ɂ��Ď����܂��B
 */
typedef enum
{
    DWC_SC_RESULT_NO_ERROR = 0,              ///< �G���[�͔������܂���ł���
    DWC_SC_RESULT_NO_AVAILABILITY_CHECK,     ///< �������̑O�ɁA�W���� GameSpy �p���`�F�b�N�����s����܂���ł���
    DWC_SC_RESULT_INVALID_PARAMETERS,        ///< �C���^�[�t�F�[�X�֐��ɓn���ꂽ�p�����[�^�������ł�
    DWC_SC_RESULT_NOT_INITIALIZED,           ///< SDK ������������܂���ł���
    DWC_SC_RESULT_CORE_NOT_INITIALIZED,      ///< �R�A������������܂���ł���
    DWC_SC_RESULT_OUT_OF_MEMORY,             ///< SDK �̓��\�[�X�Ƀ����������蓖�Ă��܂���ł���
    DWC_SC_RESULT_CALLBACK_PENDING,          ///< �A�v���P�[�V�����ɑ΂��A���삪�ۗ����ł��邱�Ƃ�`���܂�

    DWC_SC_RESULT_HTTP_ERROR,                ///< �o�b�N�G���h���K���� HTTP �ɂ�郌�X�|���X�Ɏ��s�����ꍇ�̃G���[����
    DWC_SC_RESULT_UNKNOWN_RESPONSE,          ///< SDK �����ʂ𗝉��ł��Ȃ��ꍇ�̃G���[����
    DWC_SC_RESULT_RESPONSE_INVALID,          ///< SDK ���o�b�N�G���h����̃��X�|���X��ǂݎ��Ȃ��ꍇ�̃G���[����
    DWC_SC_RESULT_INVALID_DATATYPE,          ///< �����ȃf�[�^�^�C�v

    DWC_SC_RESULT_REPORT_INCOMPLETE,         ///< ���|�[�g���s���S�ł�
    DWC_SC_RESULT_REPORT_INVALID,            ///< ���|�[�g�̈ꕔ�܂��͂��ׂĂ������ł�
    DWC_SC_RESULT_SUBMISSION_FAILED,         ///< ���|�[�g�̑��M�Ɏ��s���܂���

    DWC_SC_RESULT_UNKNOWN_ERROR,             ///< SDK ���F���ł��Ȃ��G���[

    DWC_SC_RESULT_MAX                        ///< �Ԃ����\���̂��錋�ʃR�[�h�̍��v��
} DWCScResult;

/**
 * �ΐ팋�ʂ��ǂ̂悤�ɏI����������\���܂��B
 */
typedef enum
{
    DWC_SC_GAME_RESULT_WIN,                  ///< ����
    DWC_SC_GAME_RESULT_LOSS,                 ///< �s�k
    DWC_SC_GAME_RESULT_DRAW,                 ///< ��������
    DWC_SC_GAME_RESULT_DISCONNECT,           ///< �ؒf
    DWC_SC_GAME_RESULT_DESYNC,               ///< �����~�X
    DWC_SC_GAME_RESULT_NONE,                 ///< ����`

    DWC_SC_GAME_RESULT_MAX                   ///< �Q�[���̌��ʃR�[�h�̍��v��
} DWCScGameResult;

/**
 * �Q�[���Z�b�V�������ǂ̂悤�ɏI�����Ă��邩�������܂��B
 */
typedef enum
{
    DWC_SC_GAME_STATUS_COMPLETE,             ///< �Q�[�� �Z�b�V�������A���f�i�ؒf�A�񓯊����Ȃǁj����邱�ƂȂ��\��ʂ�ɏI�����܂����B ���̃X�e�[�^�X�́A���ׂẴv���C���[���Q�[�����ʂ����ł��邱�Ƃ������Ă��܂�
    DWC_SC_GAME_STATUS_PARTIAL,              ///< �Q�[�� �Z�b�V�����͗\��ʂ�ɏI�����܂������A1 �l�ȏ�̃v���C���[���\�z�O�ɒ��~�܂��͐ؒf���܂����B �ڑ��ؒf���ꂽ�v���C���[���Q�[�����ʂŖ��m�Ƀ��|�[�g����A���K�����A�\���̂���y�i���e�B ���g���b�N�Ɏg�p����܂�
    DWC_SC_GAME_STATUS_BROKEN,               ///< �Q�[�� �Z�b�V�����͗\�z�ʂ�̃G���h �|�C���g�ɒB�����A�������܂���ł����B ����́A�I�����ʂ��m��ł��Ȃ��C�x���g�����o���ꂽ�ꍇ�Ƀ��|�[�g�����K�v������܂�

    DWC_SC_GAME_STATUS_MAX                   ///< �Q�[�� �X�e�[�^�X �R�[�h�̍��v��
} DWCScGameStatus;

/* -------------------------------------------------------------------------
        struct
   ------------------------------------------------------------------------- */

/**
 * ���|�[�g���i�[�p�\����
 */
typedef void* DWCScReportPtr;

/**
 * ���O�C���F�؏��i�[�p�\����
 */
typedef void*	DWCScLoginCertificatePtr;

/* -------------------------------------------------------------------------
        typedef - callback
   ------------------------------------------------------------------------- */

/**
 * �Z�b�V�����쐬���������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * �Z�b�V�����쐬���������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * Param:  theResult ��������
 * Param:  theUserData ���[�U��`�f�[�^
 */
typedef void (*DWCScCreateSessionCallback)(DWCScResult theResult, void* theUserData);

/**
 * ���|�[�g�C���e���V�������������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * ���|�[�g�C���e���V�������������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * Param:  theResult ��������
 * Param:  theUserData ���[�U��`�f�[�^
 */
typedef void (*DWCScSetReportIntentionCallback)(DWCScResult theResult, void* theUserData);

/**
 * ���|�[�g���M���������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * ���|�[�g���M���������������Ƃ��ɌĂяo�����R�[���o�b�N�֐��B
 *
 * Param:  theResult ��������
 * Param:  theUserData ���[�U��`�f�[�^
 */
typedef void (*DWCScSubmitReportCallback)(DWCScResult theResult, void* theUserData);

/* -------------------------------------------------------------------------
        function - external
   ------------------------------------------------------------------------- */

extern DWCScResult DWC_ScInitialize(int theGameId);
extern DWCScResult DWC_ScShutdown();
extern DWCScResult DWC_ScProcess();
extern DWCScResult DWC_ScCreateSessionAsync(DWCScCreateSessionCallback theCallback, u32 theTimeoutMs, void* theUserData);
extern DWCScResult DWC_ScCreateMatchlessSessionAsync(DWCScCreateSessionCallback theCallback, u32 theTimeoutMs, void* theUserData);
extern DWCScResult DWC_ScSetReportIntentionAsync(BOOL isAuthoritative, DWCScSetReportIntentionCallback theCallback, u32 theTimeoutMs, void* theUserData);
extern DWCScResult DWC_ScSubmitReportAsync(const DWCScReportPtr theReport, BOOL isAuthoritative, DWCScSubmitReportCallback theCallback, u32 theTimeoutMs, void * theUserData);
extern DWCScResult DWC_ScSetSessionId(const u8 theSessionId[DWC_SC_SESSION_GUID_SIZE]);
const u8* DWC_ScGetSessionId();
const u8* DWC_ScGetConnectionId();
extern DWCScResult DWC_ScCreateReport(u32 theHeaderVersion, u32 thePlayerCount, u32 theTeamCount, DWCScReportPtr * theReportOut);
extern DWCScResult DWC_ScReportBeginGlobalData(DWCScReportPtr theReportData);
extern DWCScResult DWC_ScReportBeginPlayerData(DWCScReportPtr theReportData);
extern DWCScResult DWC_ScReportBeginTeamData(DWCScReportPtr theReportData);
extern DWCScResult DWC_ScReportBeginNewPlayer(DWCScReportPtr theReportData);
extern DWCScResult DWC_ScReportSetPlayerData(DWCScReportPtr theReport, u32 thePlayerIndex, const u8 thePlayerConnectionId[DWC_SC_CONNECTION_GUID_SIZE], u32 thePlayerTeamId, DWCScGameResult theResult, DWCScLoginCertificatePtr theCertificate, const u8 theAuthData[DWC_SC_AUTHDATA_SIZE]);
extern DWCScResult DWC_ScReportBeginNewTeam(DWCScReportPtr theReportData);
extern DWCScResult DWC_ScReportSetTeamData(DWCScReportPtr theReport, u32 theTeamId, DWCScGameResult theResult);
extern DWCScResult DWC_ScReportEnd(DWCScReportPtr theReport, BOOL isAuth, DWCScGameStatus theStatus);
extern DWCScResult DWC_ScReportSetAsMatchless(DWCScReportPtr theReport);
extern DWCScResult DWC_ScReportAddIntValue(DWCScReportPtr theReportData, u16 theKeyId, s32 theValue);
extern DWCScResult DWC_ScReportAddShortValue(DWCScReportPtr theReportData, u16 theKeyId, s16 theValue);
extern DWCScResult DWC_ScReportAddByteValue(DWCScReportPtr theReportData, u16 theKeyId, s8 theValue);
extern DWCScResult DWC_ScReportAddFloatValue(DWCScReportPtr theReportData, u16 theKeyId, f32 theValue);
extern DWCScResult DWC_ScReportAddStringValue(DWCScReportPtr theReportData, u16 theKeyId, const char* theValue);
extern DWCScResult DWC_ScDestroyReport(DWCScReportPtr theReport);

extern DWCScResult DWC_ScGetLoginCertificate( DWCScLoginCertificatePtr* theCertificate );

#ifdef __cplusplus
} /* extern "C" */
#endif

/* DWC_SC_H_ */
#endif
