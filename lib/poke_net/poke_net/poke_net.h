/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net.h

  $NoKeywords: $
 *===========================================================================*/
//===========================================================================

#ifndef ___POKE_NET___
#define ___POKE_NET___

//==============================================
//                SSL ��`
//==============================================
#define ___COMMUNICATES_BY_USING_SSL___	 // SSL�ʐM�𗘗p���Ȃ��ꍇ�́A�R�����g�A�E�g���Ă��������B

// �A�h���X�ɂ�炸 SSL ���g�p����
// �R�����g�A�E�g�����ꍇ�A�A�h���X�悪 <https> �Ŏn�܂�
// �ꍇ�� SSL ���g�p����
// ___COMMUNICATES_BY_USING_SSL___ ������`�̏ꍇ�͖���
#define ___ALWAYS_ENABLE_SSL___


//==============================================
//                �C���N���[�h
//==============================================
//------------------------------------
//               ����
//------------------------------------
#include "poke_net_dscomp.h"					// LINUX/WINDOWS/DS��typedef�z��

//------------------------------------
//             WINDOW���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_WINDOWS___
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <windowsx.h>
#include <process.h>
#include <locale.h>
#include "../openssl\openssl\ssl.h"
#include "../openssl\openssl\err.h"
#include "../openssl\openssl\x509.h"

#pragma comment(lib, "../openssl/lib/libeay32MTd.lib")
#pragma comment(lib, "../openssl/lib/ssleay32MTd.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

//------------------------------------
//               DS���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_DS___
#include <nitro.h>
#include <nitro/misc.h>
#include <nitro/types.h>
#include <nitro/os/common/thread.h>
//#include <ninet/ip.h>
#include <twlWiFi.h>
#include <dwc.h>
// �ȉ��� NITRO �ł̓��C�u�������ɒ�`����Ă��邪�A
// TWL �ł͒�`����Ă��Ȃ��̂ŁA�����ɒ�`���Ă���
// ��`���@�� NITRO �Ɠ���
// NitroDWC/include/gs/common/gsPlatformSocket.h ���Q�l
typedef int SOCKET;
#ifndef INVALID_SOCKET 
	#define INVALID_SOCKET (-1)
#endif
#endif

//------------------------------------
//            LINUX���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_LINUX___
/*
#include "gds_header/attr_def.h"				// �o�g���f�[�^��`
#include "gds_header/battle_type.h"				// �o�g���f�[�^�퓬�^�C�v
#include "gds_header/trtype_def.h"				// �g���[�i�[�^�C�v��`
#include "gds_header/trtype_sex.h"				// �g���[�i�[���ʒ�`
*/
#include <openssl/ssl.h>
#include <openssl/err.h>

#endif

//------------------------------------
//               ����
//------------------------------------
#include "poke_net_common.h"					// Windows/Linux/DS���ʃw�b�_
#include "poke_net_opt.h"						// �f�[�^�̈Í����A������


//==============================================
//                 �u����������
//==============================================
//------------------------------------
//             WINDOW���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_WINDOWS___
#define POKE_NET_CONNECT_TIMEOUT	15			// �R�l�N�g�^�C���A�E�g(sec)
#define POKE_NET_SEND_TIMEOUT		30			// ���M�^�C���A�E�g(sec)
#define POKE_NET_RECV_TIMEOUT		30			// ��M�^�C���A�E�g(sec)
#endif

//------------------------------------
//               DS���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_DS___
#define POKE_NET_CONNECT_TIMEOUT	15			// �R�l�N�g�^�C���A�E�g(sec)
#define POKE_NET_SEND_TIMEOUT		30			// ���M�^�C���A�E�g(sec)
#define POKE_NET_RECV_TIMEOUT		30			// ��M�^�C���A�E�g(sec)
#endif
//------------------------------------
//             LINUX���L
//------------------------------------
#ifdef ___POKE_NET_BUILD_LINUX___
#define POKE_NET_RECV_TIMEOUT		40			// ��M�^�C���A�E�g(sec)
#define POKE_NET_SEND_TIMEOUT		40			// ���M�^�C���A�E�g(sec)
#endif

//------------------------------------
//               ����
//------------------------------------
#ifndef ___POKE_NET_BUILD_LINUX___

//! �|�P���� WiFi ���C�u�����̌��݂̏��
typedef enum {
	POKE_NET_STATUS_INACTIVE ,					//!< ��ғ���
	POKE_NET_STATUS_INITIALIZED ,				//!< �������ς�
	POKE_NET_STATUS_REQUEST ,					//!< ���N�G�X�g���s
	POKE_NET_STATUS_NETSETTING ,				//!< �l�b�g���[�N�ݒ蒆
	POKE_NET_STATUS_CONNECTING ,				//!< �ڑ���
	POKE_NET_STATUS_SENDING ,					//!< ���M��
	POKE_NET_STATUS_RECEIVING ,					//!< ��M��
	POKE_NET_STATUS_ABORTED ,					//!< ���f
	POKE_NET_STATUS_FINISHED ,					//!< ����I��
	POKE_NET_STATUS_ERROR ,						//!< �G���[�I��
	POKE_NET_STATUS_COUNT
} POKE_NET_STATUS;

//! �|�P���� WiFi ���C�u�����̍ŏI�G���[�R�[�h
typedef enum {
	POKE_NET_LASTERROR_NONE ,					//!< ���ɂȂ�
	POKE_NET_LASTERROR_NOTINITIALIZED ,			//!< ����������Ă��Ȃ�
	POKE_NET_LASTERROR_ILLEGALREQUEST ,			//!< ���N�G�X�g���ُ�
	POKE_NET_LASTERROR_CREATESOCKET ,			//!< �\�P�b�g�����Ȃ�����
	POKE_NET_LASTERROR_IOCTRLSOCKET ,			//!< �\�P�b�g��񓯊��ɂł��Ȃ�����
	POKE_NET_LASTERROR_NETWORKSETTING ,			//!< �ڑ���Ȃǎ擾�G���[
	POKE_NET_LASTERROR_CREATETHREAD ,			//!< �X���b�h�����Ȃ�����
	POKE_NET_LASTERROR_CONNECT ,				//!< �ڑ��G���[
	POKE_NET_LASTERROR_SENDREQUEST ,			//!< ���N�G�X�g�G���[
	POKE_NET_LASTERROR_RECVRESPONSE ,			//!< ���X�|���X�G���[
	POKE_NET_LASTERROR_CONNECTTIMEOUT ,			//!< �ڑ��^�C���A�E�g
	POKE_NET_LASTERROR_SENDTIMEOUT ,			//!< ���M�^�C���A�E�g
	POKE_NET_LASTERROR_RECVTIMEOUT ,			//!< ��M�^�C���A�E�g
	POKE_NET_LASTERROR_ABORTED ,				//!< ���f�I��
	POKE_NET_LASTERROR_GETSVL ,					//!< SVL�擾�G���[
	POKE_NET_LASTERROR_SSL_SETTING ,			//!< SSL �Z�b�g�A�b�v�֘A�̃G���[
	POKE_NET_LASTERROR_COUNT
} POKE_NET_LASTERROR;

//! �|�P���� WiFi ���C�u�����̃f�o�b�O���b�Z�[�W���[�h
typedef enum 
{
	POKE_NET_DEBUGLEVEL_NOT,					//!< ��f�o�b�O���[�h
	POKE_NET_DEBUGLEVEL_NORMAL,					//!< �ʏ�f�o�b�O���[�h
	POKE_NET_DEBUGLEVEL_LEVEL1,					//!< �ڍ׃f�o�b�O���[�h
	POKE_NET_DEBUGLEVEL_LEVEL2					//!< ���ڍ׃f�o�b�O���[�h
}
POKE_NET_DEBUGLEVEL;

#define POKE_NET_CURRENT_DEBUGLEVEL	POKE_NET_DEBUGLEVEL_LEVEL1		// �f�o�b�O���x���ݒ�

#endif // ___POKE_NET_BUILD_LINUX___

//==============================================
//              �N���X/�\���̒�`
//==============================================
#ifndef ___POKE_NET_BUILD_LINUX___
typedef struct poke_net_control {
	POKE_NET_REQUESTCOMMON_AUTH Auth;		// �N���C�A���g���
	BOOL SvlToken;							// �T�[�r�X���P�[�V�����g�[�N�����Z�b�g����Ă��邩
	POKE_NET_STATUS Status;					// �X�e�[�^�X(POKE_NET_STATUS_xxxx)
	char URL[256];							// �ڑ���T�[�o�[�t�q�k
	unsigned short PortNo;					// �|�[�g�ԍ�
	long ConnectTimeOut;					// �R�l�N�g�^�C���A�E�g����(sec)
	long SendTimeOut;						// ���M�^�C���A�E�g����(sec)
	long RecvTimeOut;						// ��M�^�C���A�E�g����(sec)
	POKE_NET_LASTERROR LastErrorCode;		// �ŏI�G���[�R�[�h(POKE_NET_LASTERROR_xxxx)
	long SendSize;							// ���M�T�C�Y
	char *SendBuffer;						// ���M�o�b�t�@(���C�u�����ŗp��)
	long RecvSize;							// ��M�T�C�Y
	char *RecvBuffer;						// ��M�o�b�t�@(���[�U�[�ŗp��)
	unsigned long IP;						// �ڑ���h�o
	BOOL AbortFlag;							// �A�{�[�g�v��
	SOCKET Socket;							// �\�P�b�g
	unsigned long ThreadLevel;				// �X���b�h���x��
#ifdef ___POKE_NET_BUILD_WINDOWS___			// == WINDOWS���L ==
	int ThreadID;							// �X���b�h�h�c
	HANDLE MutexHandle;						// �~���[�e�N�X�n���h��
		// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___
	// �m�������ڑ����ɃT�[�o���N���C�A���g�ɂ���č������ SSL/TLS �\���ł��B
	// ����͎��ۂɂ� SSL API �ł͒��j�ƂȂ�\���ł��B
	SSL* m_pSSL;
	// �v���O������ life-time ���Ɉ�x�A�T�[�o�ƃN���C�A���g�ɂ���č���A
	// �ڑ��̂��߂Ɍ�ō���� SSL �\���̂��߂̃f�t�H���g�l����Ɏ��S�̓I�ȃR���e�N�X�g�\���ł��B
	SSL_CTX* m_pSSL_CTX;
#endif	// ___COMMUNICATES_BY_USING_SSL___
#endif  // ___POKE_NET_BUILD_WINDOWS___
#ifdef ___POKE_NET_BUILD_DS___				// == DS���L ==
	OSThread ThreadID;						// �X���b�h�h�c
	OSMutex MutexHandle;					// �~���[�e�N�X�n���h��
#ifdef ___COMMUNICATES_BY_USING_SSL___
    SOCSslConnection sslConnection;         // SSL �R�l�N�V����
#endif // ___COMMUNICATES_BY_USING_SSL___
#endif // ___POKE_NET_BUILD_DS___
} POKE_NET_CONTROL;
#endif // ___POKE_NET_BUILD_LINUX___

#ifndef	___POKE_NET_BUILD_LINUX___
	
#endif //___POKE_NET_BUILD_LINUX___

//==============================================
//              �֐���`
//==============================================
#ifndef ___POKE_NET_BUILD_LINUX___
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

// -- �������m�� --
void *POKE_NET_AllocFunc( long _size );										// �������m��
void POKE_NET_FreeFunc( void *_ptr  );										// ���

// -- �~���[�e�N�X���� --
BOOL POKE_NET_CreateMutex();												// �~���[�e�N�X�쐬
BOOL POKE_NET_ReleaseMutex();												// �~���[�e�N�X���
void POKE_NET_LockMutex();													// �~���[�e�N�X���b�N
void POKE_NET_UnlockMutex();												// �~���[�e�N�X�A�����b�N

// -- �X���b�h�n���h�� --
BOOL POKE_NET_CreateThread();												// �X���b�h�쐬
void POKE_NET_ThreadFunction();												// �X���b�h����


// -- �l�b�g���[�N�֌W --
void POKE_NET_ControlInitialize();											// �����񏉊���
void POKE_NET_ControlRelease();												// ������J��

BOOL POKE_NET_Initialize(const char *_url ,unsigned short _portno ,POKE_NET_REQUESTCOMMON_AUTH *_auth);	// ������
void POKE_NET_SetThreadLevel(unsigned long _level);							// �X���b�h���x���ݒ�
void POKE_NET_Clear();														// �ēx���N�G�X�g���o����悤�N���A
void POKE_NET_Release();													// ���
BOOL POKE_NET_ExInitialize();												// �@����L������
void POKE_NET_ExRelease();													// �@����L���

BOOL POKE_NET_Request(unsigned short _reqno ,void *_param ,void *_response ,long _size);// ���N�G�X�g���s
BOOL POKE_NET_Abort();														// �ʐM���f

BOOL POKE_NET_GetSvl();														// SVL�擾
BOOL POKE_NET_Setting();													// �Z�b�e�B���O��
BOOL POKE_NET_Connecting();													// �ڑ���
BOOL POKE_NET_Sending();													// ���M��
BOOL POKE_NET_Receiving();													// ��M��
void POKE_NET_CloseSocket();												// �\�P�b�g���

void *POKE_NET_GetResponse();												// �ԓ��擾
long POKE_NET_GetResponseSize();											// �ԓ��T�C�Y�擾
POKE_NET_STATUS POKE_NET_GetStatus();										// ���݂̃X�e�[�^�X�擾
POKE_NET_LASTERROR POKE_NET_GetLastErrorCode();								// �ŏI�G���[�R�[�h�擾

// -- SSL �֌W --
void POKE_NET_SSL_Initialize();
void POKE_NET_SSL_Clear();

void POKE_NET_Sleep(long _ms);												// �X���[�v

#ifdef SDK_FINALROM
#define POKE_NET_DebugPrintf(_level , ...)		(void)(0)
#define POKE_NET_DebugPrintfW(_level , ...)		(void)(0)
#else
void POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL _level ,char *_format ,...);	// �f�o�b�O�v�����g
void POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL _level ,wchar_t *_format ,...);	// �f�o�b�O�v�����g(���C�h�����Ή�)
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ___POKE_NET_BUILD_LINUX___

#endif // ___POKE_NET___
