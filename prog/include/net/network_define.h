
//=============================================================================
/**
 * @file  network_define.h
 * @brief WB�̃l�b�g���[�N��`�������w�b�_�[�t�@�C��
 * @author  Katsumi Ohno
 * @date    2008.07.29
 */
//=============================================================================

#pragma once

#define NET_WORK_ON (1)   // �T���v���v���O������ŒʐM�𓮂����ꍇ��`�@�������Ȃ��ꍇ����`�ɂ���


//==============================================================================
//  �T�[�o�[�ڑ����`
//==============================================================================
///�F�؃T�[�o�[�̊J���p�T�[�o�[�A���i�ŃT�[�o�[�̐؂�ւ�
#ifdef PM_DEBUG //--
///�L���ɂ��Ă���ƃf�o�b�O�T�[�o�[�֐ڑ�
#define DEBUG_SERVER
#endif  //-- PM_DEBUG

//DWC_Init�`�Ŏd�l����AGameCode
#ifdef VERSION_BLACK
#define GF_DWC_GAMECODE		('IRAJ')
#elif defined(VERSION_WHITE)
#define GF_DWC_GAMECODE		('IRBJ')
#endif

//DWC_Init�`�Ŏd�l����AGameName
#define GF_DWC_GAMENAME		("syachi2ds")

#ifdef DEBUG_SERVER
//�J���p�T�[�o�[
#define GF_DWC_CONNECTINET_AUTH_TYPE  (DWC_CONNECTINET_AUTH_TEST)
//�s�v�c�ȑ��蕨�@��`��L���ɂ���Ɩ{�ԃT�[�o�֐ڑ�
//#define USE_AUTHSERVER_RELEASE      // �{�ԃT�[�o�֐ڑ�
// dpw_common.h ��DPW_SERVER_PUBLIC�̒�`�͒��ڃt�@�C�����ŕύX����K�v������܂��B
//DPW_SERVER_PUBLIC

//Wi-Fi���r�[�T�[�o
//#define GF_DWC_LOBBY_CHANNEL_PREFIX   (PPW_LOBBY_CHANNEL_PREFIX_DEBUG)
#define GF_DWC_LOBBY_CHANNEL_PREFIX   (PPW_LOBBY_CHANNEL_PREFIX_DEBUG5) //����f�o�b�O���ׁ̈AWB��9

#else //---------- DEBUG_SERVER

//���i�ŗp�T�[�o�[
#define GF_DWC_CONNECTINET_AUTH_TYPE  (DWC_CONNECTINET_AUTH_RELEASE)
//�s�v�c�ȑ��蕨�@��`��L���ɂ���Ɩ{�ԃT�[�o�֐ڑ�
#define USE_AUTHSERVER_RELEASE      // �{�ԃT�[�o�֐ڑ�
// dpw_common.h ��DPW_SERVER_PUBLIC�̒�`�͒��ڃt�@�C�����ŕύX����K�v������܂��B
//DPW_SERVER_PUBLIC
//Wi-Fi���r�[�T�[�o
#define GF_DWC_LOBBY_CHANNEL_PREFIX   (DWC_LOBBY_CHANNEL_PREFIX_RELEASE)

#endif  //---------- DEBUG_SERVER


//==============================================================================
//  ��`
//==============================================================================

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork);
extern u32 GFLR_NET_GetGGID(void);

//==============================================================================
//  �^�錾
//==============================================================================

/// �Q�[���̎�ނ���ʂ���ׂ̒�`   �������\���̂�gsid�Ƃ��ēn���Ă�������
enum NetworkServiceID_e {
  WB_NET_NOP_SERVICEID = 0,
  WB_NET_BATTLE_SERVICEID,       ///< �o�g���pID
  WB_NET_FIELDMOVE_SERVICEID,  ///< �T���v���̃t�B�[���h�ړ�
  WB_NET_BOX_DOWNLOAD_SERVICEID,  ///< �_�E�����[�h�ʐM�̃{�b�N�X�擾
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_BATTLE,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_TRADE_SERVICEID,     ///<�|�P��������
  WB_NET_MINIGAME_TOOL,       ///<WIFI�L��~�j�Q�[���c�[��
  WB_NET_MYSTERY,         ///<�s�v�c�ȑ��蕨
  WB_NET_BALLOON,         ///<���D����(�\�[�i���X)
  WB_NET_BUCKET,          ///<������(�}���m�[��)
  WB_NET_BALANCEBALL,       ///<�o�����X�{�[��(�}�l�l)
  WB_NET_WIFICLUB,                 ///<WIFICLUB
  WB_NET_COMPATI_CHECK,       ///<�����`�F�b�N�i�e�X�g�Łj
  WB_NET_IRCBATTLE,               ///< IRC>WIRELESS>�ڑ�
  WB_NET_WIFILOBBY,                 ///<WIFI�L��
  WB_NET_IRCCOMPATIBLE,     ///�����`�F�b�N
  WB_NET_IRCAURA,         ///�����`�F�b�N �I�[���`�F�b�N
  WB_NET_IRCRHYTHM,       ///�����`�F�b�N�@RHYTHM�`�F�b�N
  WB_NET_IRCMENU,       ///�����`�F�b�N�@RHYTHM�`�F�b�N
  WB_NET_MUSICAL,       ///�~���[�W�J��
  WB_NET_UNION,           ///<���j�I�����[��
  WB_NET_COLOSSEUM,       ///<�R���V�A��
  WB_NET_IRCTRADE,       ///<�ԊO���ۂ�����
  WB_NET_IRCFRIEND,       ///<�ԊO���Ƃ���������
  WB_NET_WIFIGTS,         ///<GTS
  WB_NET_IRC_BATTLE,      ///<�ԊO���o�g���E(���p)
  WB_NET_CAMERA_TEST,         ///<�J�����e�X�g
  WB_NET_GDS,              ///<GDS

  WB_NET_SERVICEID_MAX   // �I�[
};

///�ʐM�R�}���h����ʂ���ׂ̒�`  �R�[���o�b�N�e�[�u����n���Ƃ���ID�Ƃ��Ē�`���Ă�������
/// ��̒�`�Ɠ������тŏ����Ă�������
enum NetworkCommandHeaderNo_e {
  GFL_NET_CMD_BASE = (WB_NET_NOP_SERVICEID<<8),  ///< �x�[�X�R�}���h�J�n�ԍ�
  GFL_NET_CMD_BATTLE = (WB_NET_BATTLE_SERVICEID<<8),   ///< �o�g���J�n�ԍ�
  GFL_NET_CMD_FIELD = (WB_NET_FIELDMOVE_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_BOX_DOWNLOAD = (WB_NET_BOX_DOWNLOAD_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_DEBUG_OHNO = (WB_NET_DEBUG_OHNO_SERVICEID<<8),
  GFL_NET_CMD_DEBUG_BATTLE = (WB_NET_SERVICEID_DEBUG_BATTLE<<8),
  GFL_NET_CMD_DEBUG_MATSUDA = (WB_NET_DEBUG_MATSUDA_SERVICEID<<8),
  GFL_NET_CMD_TRADE = (WB_NET_TRADE_SERVICEID<<8),      ///<�|�P���������̊J�n�ԍ�
  GFL_NET_CMD_MINIGAME_TOOL = (WB_NET_MINIGAME_TOOL<<8),  ///<WIFI�L��~�j�Q�[���c�[��
  GFL_NET_CMD_MYSTERY = (WB_NET_MYSTERY<<8),        ///<�s�v�c�ȑ��蕨
  GFL_NET_CMD_BALLOON = (WB_NET_BALLOON<<8),        ///<���D����
  GFL_NET_CMD_BUCKET = (WB_NET_BUCKET<<8),          ///<������
  GFL_NET_CMD_BALANCEBALL = (WB_NET_BALANCEBALL<<8),    ///<�o�����X�{�[��
  GFL_NET_CMD_WIFICLUB = (WB_NET_WIFICLUB<<8),        ///<WIFICLUB
  GFL_NET_CMD_COMPATI_CHECK = (WB_NET_COMPATI_CHECK<<8),  ///<�����`�F�b�N�i�e�X�g�Łj
  GFL_NET_CMD_IRCBATTLE = (WB_NET_IRCBATTLE<<8),  ///< IRC>WIRELESS>�ڑ�
  GFL_NET_CMD_WIFILOBBY = (WB_NET_WIFILOBBY<<8),      ///<WIFI�L��
  GFL_NET_CMD_IRCCOMPATIBLE = (WB_NET_IRCCOMPATIBLE<<8),  ///<�����`�F�b�N
  GFL_NET_CMD_IRCAURA = (WB_NET_IRCAURA<<8),  ///<�����`�F�b�N�@�I�[���`�F�b�N
  GFL_NET_CMD_IRCRHYTHM = (WB_NET_IRCRHYTHM<<8),  ///<�����`�F�b�N�@���Y���`�F�b�N
  GFL_NET_CMD_IRCMENU = (WB_NET_IRCMENU<<8),  ///<�����`�F�b�N�@���j���[
  GFL_NET_CMD_MUSICAL = (WB_NET_MUSICAL<<8),  ///�~���[�W�J��
  GFL_NET_CMD_UNION = (WB_NET_UNION<<8),      ///<���j�I�����[��
  GFL_NET_CMD_COLOSSEUM = (WB_NET_COLOSSEUM<<8),      ///<�R���V�A��
  GFL_NET_CMD_IRCTRADE = (WB_NET_IRCBATTLE<<8),  ///< IRC�ۂ���������
  GFL_NET_CMD_IRCFRIEND = (WB_NET_IRCFRIEND<<8),  ///< IRC�Ƃ�������������
  GFL_NET_CMD_WIFIGTS   = (WB_NET_WIFIGTS<<8),    ///< GTS
  GFL_NET_CMD_IRC_BATTLE    = (WB_NET_IRC_BATTLE<<8),   ///< IRC
  GFL_NET_CMD_CAMERA_TEST   = (WB_NET_CAMERA_TEST<<8),    ///< �J�����e�X�g
  GFL_NET_CMD_GDS   = (WB_NET_GDS<<8),    ///< GDS
};

#include "net_irc.h"



