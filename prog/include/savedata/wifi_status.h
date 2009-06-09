#pragma once
//============================================================================================
/**
 * @file	  wifi_status.h
 * @brief	  WIFI�ɗ������̊Ǘ�
            �Z�[�u����f�[�^�ł͂Ȃ��� mystatus�\���̂̓���̂��߂����ɂ���
 * @author	k.ohno
 * @date	  2009.6.6
 */
//============================================================================================
#include "savedata/mystatus.h"

typedef struct _WIFI_STATUS WIFI_STATUS;


//---------------WIFISTATUS�f�[�^
typedef enum{
  WIFI_STATUS_NONE,   // ��������	NONE�̂Ƃ��͏o�������܂���
  WIFI_STATUS_VCT,      // VCT��
  WIFI_STATUS_SBATTLE50,      // �V���O���ΐ풆
  WIFI_STATUS_SBATTLE100,      // �V���O���ΐ풆
  WIFI_STATUS_SBATTLE_FREE,      // �V���O���ΐ풆
  WIFI_STATUS_DBATTLE50,      // �_�u���ΐ풆
  WIFI_STATUS_DBATTLE100,      // �_�u���ΐ풆
  WIFI_STATUS_DBATTLE_FREE,      // �_�u���ΐ풆
  WIFI_STATUS_TRADE,          // ������
  WIFI_STATUS_SBATTLE50_WAIT,   // �V���O��Lv50�ΐ��W��
  WIFI_STATUS_SBATTLE100_WAIT,   // �V���O��Lv100�ΐ��W��
  WIFI_STATUS_SBATTLE_FREE_WAIT,   // �V���O��Free�ΐ��W��
  WIFI_STATUS_DBATTLE50_WAIT,   // �_�u��Lv50�ΐ��W��
  WIFI_STATUS_DBATTLE100_WAIT,   // �_�u��Lv100�ΐ��W��
  WIFI_STATUS_DBATTLE_FREE_WAIT,   // �_�u��Free�ΐ��W��
  WIFI_STATUS_TRADE_WAIT,    // ������W��
  WIFI_STATUS_LOGIN_WAIT,    // �ҋ@���@���O�C������͂���
  
  WIFI_STATUS_DP_UNK,        // DP��UNKNOWN
  
  // �v���`�i�Œǉ�
  WIFI_STATUS_FRONTIER,          // �t�����e�B�A��
  WIFI_STATUS_FRONTIER_WAIT,    // �t�����e�B�A��W��

  WIFI_STATUS_BUCKET,				// �o�P�b�g�Q�[��
  WIFI_STATUS_BUCKET_WAIT,			// �o�P�b�g�Q�[����W��
  WIFI_STATUS_BALANCEBALL,		    // �ʏ��Q�[��
  WIFI_STATUS_BALANCEBALL_WAIT,		// �ʏ��Q�[����W��
  WIFI_STATUS_BALLOON,				// �΂�[��Q�[��
  WIFI_STATUS_BALLOON_WAIT,			// �΂�[��[����W��

#ifdef WFP2P_DEBUG_EXON
  WIFI_STATUS_BATTLEROOM,     // �o�g�����[����
  WIFI_STATUS_BATTLEROOM_WAIT,// �o�g�����[����W��
  WIFI_STATUS_MBATTLE_FREE,     // �}���`�o�g����
  WIFI_STATUS_MBATTLE_FREE_WAIT,// �}���`�o�g����W��
#endif

  WIFI_STATUS_PLAY_OTHER,	// WiFi�N���u�ɈȊO�ŗV�ђ�
  WIFI_STATUS_UNKNOWN,   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�

#if 0  //��������\��
	WIFI_STATUS_NONE,         // ��������	NONE�̂Ƃ��͏o�������܂���
  WIFI_STATUS_VCT,            // VCT��
  WIFI_STATUS_SBATTLE50,      // �V���O���ΐ풆����W��
  WIFI_STATUS_SBATTLE100,      // �V���O���ΐ풆����W��
  WIFI_STATUS_SBATTLE_FREE,      // �V���O���ΐ풆����W��
  WIFI_STATUS_DBATTLE50,      // �_�u���ΐ풆����W��
  WIFI_STATUS_DBATTLE100,      // �_�u���ΐ풆����W��
  WIFI_STATUS_DBATTLE_FREE,      // �_�u���ΐ풆����W��
  WIFI_STATUS_TRADE,          // ����������W��
  WIFI_STATUS_LOGIN,           // �ҋ@�� ���O�C������͂���
  WIFI_STATUS_FRONTIER,        // �t�����e�B�A������W��
  WIFI_STATUS_BATTLEROOM,      // �o�g�����[����
  WIFI_STATUS_MBATTLE_FREE,     // �}���`�o�g����
  WIFI_STATUS_UNKNOWN,   // �V���ɍ�����炱�̔ԍ��ȏ�ɂȂ�
#endif
} WIFI_STATUS_e;



extern int WIFI_STATUS_GetSize(void);
extern const MYSTATUS* WIFI_STATUS_GetMyStatus(const WIFI_STATUS* pStatus);
extern u8 WIFI_STATUS_GetWifiMode(const WIFI_STATUS* pStatus);
extern u8 WIFI_STATUS_GetVChatStatus(const WIFI_STATUS* pStatus);
extern u8 WIFI_STATUS_GetActive(const WIFI_STATUS* pStatus);
extern BOOL WIFI_STATUS_IsVChatMac(const WIFI_STATUS* pStatus, const u8* SearchMacAddress);

