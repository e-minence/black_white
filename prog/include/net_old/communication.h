//=============================================================================
/**
 * @file	communication.h
 * @brief	�ʐM�֘A�ŊO���Q�Ƃ��K�v�Ȋ�{�I�Ȃ��̂��`
 * @author	Katsumi Ohno
 * @date    2005.11.14
 */
//=============================================================================
#pragma once


//==============================================================================
//	�T�[�o�[�̐��i�ŁA�J���łւ̐ڑ��؂�ւ�
//==============================================================================
///�F�؃T�[�o�[�̊J���p�T�[�o�[�A���i�ŃT�[�o�[�̐؂�ւ�
#ifdef PM_DEBUG
///�L���ɂ��Ă���ƃf�o�b�O�T�[�o�[�֐ڑ�
#define DEBUG_SERVER
#else

#endif

#ifdef DEBUG_SERVER
//�J���p�T�[�o�[
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_TEST)
//�s�v�c�ȑ��蕨�@��`��L���ɂ���Ɩ{�ԃT�[�o�֐ڑ�
//#define USE_AUTHSERVER_RELEASE			// �{�ԃT�[�o�֐ڑ�
// dpw_common.h ��DPW_SERVER_PUBLIC�̒�`�͒��ڃt�@�C�����ŕύX����K�v������܂��B
//DPW_SERVER_PUBLIC

//Wi-Fi���r�[�T�[�o
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(PPW_LOBBY_CHANNEL_PREFIX_DEBUG)

#else	//---------- DEBUG_SERVER

//���i�ŗp�T�[�o�[
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_RELEASE)
//�s�v�c�ȑ��蕨�@��`��L���ɂ���Ɩ{�ԃT�[�o�֐ڑ�
#define USE_AUTHSERVER_RELEASE			// �{�ԃT�[�o�֐ڑ�
// dpw_common.h ��DPW_SERVER_PUBLIC�̒�`�͒��ڃt�@�C�����ŕύX����K�v������܂��B
//DPW_SERVER_PUBLIC
//Wi-Fi���r�[�T�[�o
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(DWC_LOBBY_CHANNEL_PREFIX_RELEASE)

#endif	//---------- DEBUG_SERVER


#include "comm_def.h"
#include "system/pms_data.h"


//ggid��`��wh.h�Ɉړ����܂���

#ifndef OHNO_PRINT
#if defined(DEBUG_ONLY_FOR_ohno) | defined(DEBUG_ONLY_FOR_tomoya_takahashi)//(defined(DEBUG_ONLY_FOR_ohno) | defined(DEBUG_ONLY_FOR_tomoya_takahashi) | defined(DEBUG_ONLY_FOR_mituhara))
//#ifdef  SDK_DEBUG

#if 0
#define OHNO_SP_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#define OHNO_PRINT(...)           ((void) 0)

#else
#define OHNO_PRINT(...) \
  (void) ((OS_Printf(__VA_ARGS__)))
#define OHNO_SP_PRINT(...)           ((void) 0)
#endif

#else   //DEBUG_ONLY_FOR_ohno

#define OHNO_PRINT(...)           ((void) 0)
#define OHNO_SP_PRINT(...)           ((void) 0)

#endif  // DEBUG_ONLY_FOR_ohno
#endif  //OHNO_PRINT

	
#ifndef TOMOYA_PRINT
#if (defined(DEBUG_ONLY_FOR_tomoya_takahashi))
	
/*// OS_Printf�̎��������o����
#define TOMOYA_PRINT(...) \
	(void) ((OS_TPrintf(__VA_ARGS__)))//*/

#define TOMOYA_PRINT(...)           ((void) 0)
	
#else

#define TOMOYA_PRINT(...)           ((void) 0)
	
#endif  // (defined(DEBUG_ONLY_FOR_tomoya_takahashi))
#endif // #ifndef TOMOYA_PRINT

#define MYSTERY_BEACON_DATA_SIZE   (84)

typedef struct {
	u16				sentence_type;			///< ���̓^�C�v
	u16				sentence_id;			///< �^�C�v��ID
	u16		word[2];		///< �P��ID
} old_PMS_DATA;

// WMBssDesc��gameInfo.userGameInfo���g�p���Ă���󋵂̍\����
// WM_SIZE_USER_GAMEINFO  �ő� 112byte
// ����̉�c�ŕύX�ɂȂ�\��
// ���ł����Ɉ��z��
typedef struct{
  u32 		pokeID;     	// ID												8
  u8  		serviceNo;   	// �ʐM�T�[�r�X�ԍ�									9
  u8  		regulationNo;  	// �퓬���M�����[�V�������R���e�X�g�^�C�v�̎���		10
  u8  		connectNum;    	// �Ȃ����Ă���䐔  --> �{�e���ǂ�������			11
  u8 		soloDebugNo;  	// ���̐l�ƒʐM���Ȃ��悤�ɂ��邽�߂̔ԍ�			12
  old_PMS_DATA	pmsData;									// �ȈՉ�b�f�[�^		20
  u8  		myStatusBuff[COMM_SEND_MYSTATUS_SIZE];  	// ���ۂ�mystatus�̑傫���͎��s���łȂ��Ƃ킩��Ȃ��̂ŏ�ɐ؂��Ă���
  u8  		regulationBuff[COMM_SEND_REGULATION_SIZE];  // ���ۂ�reg�̑傫���͎��s���łȂ��Ƃ킩��Ȃ��̂ŏ�ɐ؂��Ă���
  // �퓬�̎��ȊO�͋󂢂Ă�̂ŁA���j�I�����[���̎q�@ID������鎖�ɂ���
  u32		UnionRoomTime;								// ���j�I�����[���ɓ����Ă��鎞�Ԃ�����������
  u8        pause;   // �ڑ����֎~���������Ɏg�p����
} _GF_BSS_DATA_INFO;

// ���p�̂ɕς��������������܂��UNION�̃\�[�X�ύX�������̂ŁA�ʍ\����
typedef struct{
  u32 		pokeID;     	// ID												8
  u8  		serviceNo;   	// �ʐM�T�[�r�X�ԍ�									9
  u8  		regulationNo;  	// �퓬���M�����[�V�������R���e�X�g�^�C�v�̎���		10
  u8  		connectNum;    	// �Ȃ����Ă���䐔  --> �{�e���ǂ�������			11
  u8 		soloDebugNo;  	// ���̐l�ƒʐM���Ȃ��悤�ɂ��邽�߂̔ԍ�			12
  u8 mysteryData[MYSTERY_BEACON_DATA_SIZE];
} _GF_BSS_MYSTERY;


#include "net_old/comm_command.h"
#include "net_old/comm_info.h"
//#include "net_old/comm_mp.h"
#include "net_old/comm_system.h"
#include "net_old/comm_recv.h"
#include "net_old/comm_message.h"
#include "net_old/comm_tool.h"
//#include "net_old/comm_wm_icon.h"
#include "net_old/comm_state.h"

// ���̐l�ƒʐM�ڑ����Ȃ��悤�ɂ��邽�߂̔ԍ�
#ifdef PM_DEBUG		//DebugROM
#if (defined(DEBUG_ONLY_FOR_ohno) | defined(DEBUG_ONLY_FOR_mituhara))
#define   SOLO_DEBUG_NO   (0)
#else
#define   SOLO_DEBUG_NO   (0)
#endif
#endif //PM_DEBUG

// �_�C���N�g�f�o�b�O�ʐM�����̂��̂ƍ������Ȃ��悤��
#define COMMDIRECT_DEBUG_NO  (2)



