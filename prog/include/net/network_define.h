
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


#define GAME_NAME					"syachi2ds"		// �g�p����Q�[����
#define GAME_SECRET_KEY		"tXH2sN"					// �g�p����V�[�N���b�g�L�[
#define GAME_PRODUCTID		12230							// �g�p����v���_�N�gID
#define GAME_ID           2911              // �Q�[��ID


#define WIFI_ND_TIMEOUT_SYNC   (60*60*2)     //�Q���̃^�C���A�E�g


#if 0

��GameSpy�ݒ���iIRAJ/IRBJ ���o�[�W�������ʂł��j
Game Name     syachi2ds 
Secret Key    tXH2sN 
Product ID    12230 
Game ID       2911 
Master Index  10

���_�E�����[�h�T�[�o�i���o�[�W�������ʂł��j
�_�E�����[�h�T�[�o�ڑ��p�Q�[���R�[�h�E
�_�E�����[�h�T�[�o�Ǘ���ʃ��[�U�[�� 
IRAJ 

�|�J���p�_�E�����[�h�T�[�o
�Ǘ����URL   https://dls1ctl.test.nintendowifi.net/control
�Ǘ���ʃp�X���[�h  wG7Q2xL4
�Q�[���p�p�X���[�h  JDXNcp79mr54RUga 

�|���i�p�_�E�����[�h�T�[�o
�Ǘ���ʁF�i���i�p�T�[�o�փA�b�v����ۂ͋g���܂ł����k�������j
�Q�[���p�p�X���[�h  fZrUY6SVMLp4tdmB 


���G���[�V�~�����[�V�����T�[�o
�EIRAJ
�G���[�V�~�����[�V�����T�[�o�[��ID TWL-IRA 
�G���[�V�~�����[�V�����T�[�o�[�̃p�X���[�h cmMWFX 

�EIRBJ
�G���[�V�~�����[�V�����T�[�o�[��ID TWL-IRB 
�G���[�V�~�����[�V�����T�[�o�[�̃p�X���[�h NvT4rI 

//�܂��AGGID�ɂ��܂��Ă����s�������܂����B
(1)0x00001380�@�ʏ�ʐM�p
(2)0x00001381�@�f�[�^�r�[�R���z�M�p
(3)0x00001382�@WCS���M�����[�V�����z�M�p








/*
��GDS�̐؂�ւ��ɂ���

poke_net���C�u�����̏������֐�"POKE_NET_GDS_Initialize"�̈����ɓn���Ă���URL���A���i�^�f�o�b�O�Ő؂�ւ��Ē������ɂȂ�܂��B
���L���Q�ƒ����܂��ł��傤���B

�i�e�X�g�T�[�o�F�f�o�b�O�Łj
�A�h���X���pkgdstest.nintendo.co.jp
�|�[�g���12401

�i�{�ԃT�[�o�F���i�Łj
�A�h���X���pkgdsprod.nintendo.co.jp
�|�[�g���12401


���s�������T�[�o�̐��i��URL

https://pkvldtprod.nintendo.co.jp/pokemon/validate
*/

#endif

/// �������������̂ł����ACGEAR�̎d�l�ɂ��GGID�𓝈ꂵ�܂�
/// 2010.01.08 k.ohno
#define SYACHI_NETWORK_GGID  (0x00001380)    //�ʏ�͂���
#define SYACHI_DOWNLOAD_GGID  (0x00001381)   //�f��Q�O�P�O��p MB�s��p
//#define SYACHI_LEG_GGID (0x00001380)         //���M�����[�V�����z�M��p

//#define SYACHI_NETWORK_GGID  (0x00001511)  //syachi���V���GGID 2010.06.29



#define NET_DREAMWORLD_VER (1)  //�h���[�����[���h�ʐM�o�[�V�����i���Ȃ炸�����j


/*
>�S�DIRC_Init��ID
>����͌��O�_�ł͂������܂���B
>IRC_Init�̃}�j���A���Ɍ�Ђ��璸���R�[�h�����Ă���������
>�����Ă���̂ł����A���s���Ă��������܂��ł��傤���H

�@������ł����ASyachi��
3
�ł��肢�v���܂��B
*/
#define POKEMON_IRC_CONNECTCODE (3)  //�ԊO���ڑ��R�[�h �m��

//==============================================================================
//  �T�[�o�[�ڑ����`
//==============================================================================
///�F�؃T�[�o�[�̊J���p�T�[�o�[�A���i�ŃT�[�o�[�̐؂�ւ�
#ifdef PM_DEBUG //--
///�L���ɂ��Ă���ƃf�o�b�O�T�[�o�[�֐ڑ�
#define DEBUG_SERVER
#endif  //-- PM_DEBUG

//DWC_Init�`�Ŏd�l����AGameCode
#if ( PM_VERSION == VERSION_WHITE )
#define GF_DWC_GAMECODE		('IRAJ')
#define GF_DWC_ND_LOGIN		"IRAJ"
#else //defined(VERSION_WHITE)
#define GF_DWC_GAMECODE		('IRBJ')
#define GF_DWC_ND_LOGIN		"IRAJ"
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

//���|�[�g���M
#define GF_DWC_GDB_AUTH_TYPE          (DWC_GDB_SSL_TYPE_NONE)

#define MATCHINGKEY (0)

///GDS�T�[�o�[(�f�o�b�O��)
#define GF_GDS_SERVER_URL     "pkgdstest.nintendo.co.jp"
#define GF_GDS_SERVER_PORT    (12401)

#else //---------- DEBUG_SERVER

//���i�ŗp�T�[�o�[
#define GF_DWC_CONNECTINET_AUTH_TYPE  (DWC_CONNECTINET_AUTH_RELEASE)
//�s�v�c�ȑ��蕨�@��`��L���ɂ���Ɩ{�ԃT�[�o�֐ڑ�
#define USE_AUTHSERVER_RELEASE      // �{�ԃT�[�o�֐ڑ�
// dpw_common.h ��DPW_SERVER_PUBLIC�̒�`�͒��ڃt�@�C�����ŕύX����K�v������܂��B
//DPW_SERVER_PUBLIC
//Wi-Fi���r�[�T�[�o
#define GF_DWC_LOBBY_CHANNEL_PREFIX   (DWC_LOBBY_CHANNEL_PREFIX_RELEASE)

//���|�[�g���M
#define GF_DWC_GDB_AUTH_TYPE          (DWC_GDB_SSL_TYPE_SERVER_AUTH)

#define MATCHINGKEY (1)

///GDS�T�[�o�[(���i��)
#define GF_GDS_SERVER_URL     ("pkgdsprod.nintendo.co.jp")
#define GF_GDS_SERVER_PORT    (12401)

#endif  //---------- DEBUG_SERVER

//�s�v�c�ȑ��蕨�ł��悤����p�X���[�h
#ifdef USE_AUTHSERVER_RELEASE
#define WIFI_ND_LOGIN_PASSWD		"fZrUY6SVMLp4tdmB"		// �p�X���[�h(�{�ԃT�[�o)
#else
#define WIFI_ND_LOGIN_PASSWD		"JDXNcp79mr54RUga"    // �p�X���[�h(�e�X�g�T�[�o)
#endif


#ifdef DEBUG_SERVER
#define LIBDPW_SERVER_TYPE ( DPW_SERVER_DEBUG )   ///< PDW�ڑ���(�f�o�b�O��)
#else
#define LIBDPW_SERVER_TYPE ( DPW_SERVER_RELEASE )   ///< PDW�ڑ��� �i�����[�X�p�j
#endif


//==============================================================================
//  ��`
//==============================================================================

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork);
extern u32 GFLR_NET_GetGGID(void);

//==============================================================================
//  �^�錾
//==============================================================================

/// �Q�[���̎�ނ���ʂ���ׂ̒�`   �������\���̂�gsid�Ƃ��ēn���Ă�������
typedef enum  {
  WB_NET_NOP_SERVICEID = 0,
  WB_NET_BATTLE_SERVICEID,       ///< �o�g���pID
  WB_NET_PALACE_SERVICEID,       ///< �p���X
  WB_NET_FIELDMOVE_SERVICEID,  ///< �T���v���̃t�B�[���h�ړ�
  WB_NET_MULTIBOOT_SERVICEID,  ///< �_�E�����[�h�ʐM�̃{�b�N�X�擾
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_BATTLE,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_TRADE_SERVICEID,     ///<�|�P��������
  WB_NET_MYSTERY,         ///<�s�v�c�ȑ��蕨
  WB_NET_WIFICLUB,                 ///<WIFICLUB
  WB_NET_COMPATI_CHECK,       ///<�����`�F�b�N�i�e�X�g�Łj
  WB_NET_IRCBATTLE,               ///< IRC>WIRELESS>�ڑ�
  WB_NET_IRCCOMPATIBLE,     ///�����`�F�b�N
  WB_NET_IRCAURA,         ///�����`�F�b�N �I�[���`�F�b�N
  WB_NET_IRCRHYTHM,       ///�����`�F�b�N�@RHYTHM�`�F�b�N
  WB_NET_IRCMENU,       ///�����`�F�b�N�@RHYTHM�`�F�b�N
  WB_NET_MUSICAL,       ///�~���[�W�J��      �iIRC�e�@�{�������ʁj        ���ԕύX�֎~
  WB_NET_MUSICAL_CHILD,       ///�~���[�W�J��  (IRC�q�@)         ���ԕύX�֎~
  WB_NET_MUSICAL_LEADER,       ///�~���[�W�J��  (IRC�F��)         ���ԕύX�֎~
  WB_NET_UNION,                             ///<���j�I�����[��
  WB_NET_COLOSSEUM,       ///<�R���V�A��
  WB_NET_UNION_PICTURE,                     ///<���G����
  WB_NET_UNION_BATTLE_MULTI_FREE_SHOOTER,   ///<�}���`(�V���[�^�[�L)
  WB_NET_UNION_BATTLE_MULTI_FREE,           ///<�}���`
  WB_NET_UNION_BATTLE_MULTI_FLAT_SHOOTER,   ///<�}���`�t���b�g(�V���[�^�[�L)
  WB_NET_UNION_BATTLE_MULTI_FLAT,           ///<�}���`�t���b�g
  WB_NET_UNION_GURUGURU,                    ///<���邮�����
  WB_NET_IRCTRADE,       ///<�ԊO���ۂ�����
  WB_NET_IRCFRIEND,       ///<�ԊO���Ƃ���������
  WB_NET_WIFIGTS,         ///<GTS
  WB_NET_IRC_BATTLE,      ///<�ԊO���o�g���E(���p)
  WB_NET_COMM_TVT,         ///<�ʐMTVT
  WB_NET_GDS,              ///<GDS
  WB_NET_GTSNEGO,          ///< GTS�l�S�V�G�[�V����
	WB_NET_DEBUG_BEACON_FRIENDCODE,	///<�f�o�b�O�@�r�[�R���ł̗F�B�R�[�h����
  WB_NET_WIFIMATCH,         ///<WIFI���E�ΐ�
  WB_NET_IRCBATTLE_MULTI,  ///< �ԊO���o�g���}���`
  WB_NET_IRCBATTLE_MULTI_IRC,  ///< �ԊO���o�g���}���`�ԊO��
  WB_NET_COMM_ENTRY_MENU,   ///<�ʐM�G���g���[���j���[
  WB_NET_GURUGURU,        ///<���邮�����
  WB_NET_PICTURE,         ///<���G����
  WB_NET_BATTLE_ADD_CMD,  ///< �ʐM�o�g�� �R�}���h���M
  WB_NET_BTL_REC_SEL,      ///<�ʐM�ΐ��̘^��I�����
  WB_NET_BSUBWAY,         ///<�o�g���T�u�E�F�C
  WB_NET_POKELIST,         ///<�|�P�������X�g(�I���������R�}���h�p
  WB_NET_BATTLE_EXAMINATION,  ///<�o�g������z�M
  WB_NET_WIFIP2P_SUB,  ///< WIFI�o�g���̒���
  WB_NET_IRC_REGULATION,  ///<���C�u���M�����[�V�����z�M
  WB_NET_GAMESYNC,  ///<GAMESYNC
  WB_NET_PDW_ACC,  ///<GAMESYNC�A�J�E���g
  WB_NET_GTS_NEGO,  ///<GTS�l�S�V�G�[�V����
  WB_NET_WIFIRNDMATCH,  ///<WIFI�����_���}�b�`�i�Q�[���T�[�r�XID�����R�[�h�J�E���g�̂���WIFIMATCH�Ƃ킯�邱�ƂɂȂ����j
  
  WB_NET_SERVICEID_MAX   // �I�[
} NetworkServiceID_e;

///�ʐM�R�}���h����ʂ���ׂ̒�`  �R�[���o�b�N�e�[�u����n���Ƃ���ID�Ƃ��Ē�`���Ă�������
/// ��̒�`�Ɠ������тŏ����Ă�������
enum NetworkCommandHeaderNo_e {
  GFL_NET_CMD_BASE = (WB_NET_NOP_SERVICEID<<8),  ///< �x�[�X�R�}���h�J�n�ԍ�
  GFL_NET_CMD_BATTLE = (WB_NET_BATTLE_SERVICEID<<8),   ///< �o�g���J�n�ԍ�
  GFL_NET_CMD_PALACE = (WB_NET_PALACE_SERVICEID<<8),   ///< �p���X�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_FIELD = (WB_NET_FIELDMOVE_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_MULTIBOOT = (WB_NET_MULTIBOOT_SERVICEID<<8),   ///< �t�B�[���h�̃x�[�X�J�n�ԍ�
  GFL_NET_CMD_DEBUG_OHNO = (WB_NET_DEBUG_OHNO_SERVICEID<<8),
  GFL_NET_CMD_DEBUG_BATTLE = (WB_NET_SERVICEID_DEBUG_BATTLE<<8),
  GFL_NET_CMD_DEBUG_MATSUDA = (WB_NET_DEBUG_MATSUDA_SERVICEID<<8),
  GFL_NET_CMD_TRADE = (WB_NET_TRADE_SERVICEID<<8),      ///<�|�P���������̊J�n�ԍ�
  GFL_NET_CMD_MYSTERY = (WB_NET_MYSTERY<<8),        ///<�s�v�c�ȑ��蕨
  GFL_NET_CMD_WIFICLUB = (WB_NET_WIFICLUB<<8),        ///<WIFICLUB
  GFL_NET_CMD_COMPATI_CHECK = (WB_NET_COMPATI_CHECK<<8),  ///<�����`�F�b�N�i�e�X�g�Łj
  GFL_NET_CMD_IRCBATTLE = (WB_NET_IRCBATTLE<<8),  ///< IRC>WIRELESS>�ڑ�
  GFL_NET_CMD_IRCCOMPATIBLE = (WB_NET_IRCCOMPATIBLE<<8),  ///<�����`�F�b�N
  GFL_NET_CMD_IRCAURA = (WB_NET_IRCAURA<<8),  ///<�����`�F�b�N�@�I�[���`�F�b�N
  GFL_NET_CMD_IRCRHYTHM = (WB_NET_IRCRHYTHM<<8),  ///<�����`�F�b�N�@���Y���`�F�b�N
  GFL_NET_CMD_IRCMENU = (WB_NET_IRCMENU<<8),  ///<�����`�F�b�N�@���j���[
  GFL_NET_CMD_MUSICAL = (WB_NET_MUSICAL<<8),  ///�~���[�W�J��
  GFL_NET_CMD_UNION = (WB_NET_UNION<<8),      ///<���j�I�����[��
  GFL_NET_CMD_COLOSSEUM = (WB_NET_COLOSSEUM<<8),      ///<�R���V�A��
  GFL_NET_CMD_UNION_PICTURE = (WB_NET_UNION_PICTURE),                     ///<���G����
  GFL_NET_CMD_UNION_BATTLE_MULTI_FREE_SHOOTER = (WB_NET_UNION_BATTLE_MULTI_FREE_SHOOTER), ///<�}���`(�V���[�^�[�L)
  GFL_NET_CMD_UNION_BATTLE_MULTI_FREE = (WB_NET_UNION_BATTLE_MULTI_FREE),  ///<�}���`
  GFL_NET_CMD_UNION_BATTLE_MULTI_FLAT_SHOOTER = (WB_NET_UNION_BATTLE_MULTI_FLAT_SHOOTER), ///<�}���`�t���b�g(�V���[�^�[�L)
  GFL_NET_CMD_UNION_BATTLE_MULTI_FLAT = (WB_NET_UNION_BATTLE_MULTI_FLAT),  ///<�}���`�t���b�g
  GFL_NET_CMD_UNION_GURUGURU = (WB_NET_UNION_GURUGURU),                    ///<���邮�����
  GFL_NET_CMD_IRCTRADE = (WB_NET_IRCBATTLE<<8),  ///< IRC�ۂ���������
  GFL_NET_CMD_IRCFRIEND = (WB_NET_IRCFRIEND<<8),  ///< IRC�Ƃ�������������
  GFL_NET_CMD_WIFIGTS   = (WB_NET_WIFIGTS<<8),    ///< GTS
  GFL_NET_CMD_IRC_BATTLE    = (WB_NET_IRC_BATTLE<<8),   ///< IRC
  GFL_NET_CMD_COMM_TVT   = (WB_NET_COMM_TVT<<8),    ///< �ʐMTVT
  GFL_NET_CMD_GDS   = (WB_NET_GDS<<8),    ///< GDS
  GFL_NET_CMD_GTSNEGO   = (WB_NET_GTSNEGO<<8),    ///< GTS�l�S�V�G�[�V����
	GFL_NET_CMD_DEBUG_BEACON_FRIENDCODE	= (WB_NET_DEBUG_BEACON_FRIENDCODE<<8),	///<�f�o�b�O�@�r�[�R���ł̗F�B�R�[�h����
  GFL_NET_CMD_WIFIMATCH = (WB_NET_WIFIMATCH<<8),         ///<WIFI���E�ΐ�
  GFL_NET_CMD_IRCBATTLE_MULTI = (WB_NET_IRCBATTLE_MULTI<<8),         ///<�ԊO���o�g���}���`
  GFL_NET_CMD_COMM_ENTRY_MENU = (WB_NET_COMM_ENTRY_MENU<<8),  ///<�ʐM�G���g���[���j���[
  GFL_NET_CMD_GURUGURU = (WB_NET_GURUGURU << 8),      ///<���邮�����
  GFL_NET_CMD_PICTURE = (WB_NET_PICTURE << 8),        ///<���G����
  GFL_NET_CMD_BSUBWAY = (WB_NET_BSUBWAY << 8),  ///<�o�g���T�u�E�F�C
  GFL_NET_CMD_POKELIST = (WB_NET_POKELIST << 8),///<�|�P�������X�g(�I���������R�}���h�p
  GFL_NET_CMD_BATTLE_EXAMINATION = (WB_NET_BATTLE_EXAMINATION<<8),  ///<�o�g������z�M
  GFL_NET_CMD_WIFIP2P_SUB = (WB_NET_WIFIP2P_SUB<<8), ///< WIFI�o�g���̒���
  GFL_NET_CMD_IRC_REGULATION  = (WB_NET_IRC_REGULATION<<8),  ///<���C�u���M�����[�V�����z�M
};


/// �ʐM���M��{�T�C�Y �����Q�[������������ꍇ�ɓ��ꂵ�����x�ɂȂ�悤�Ɏw�肵�Ă���
/// �S�l�ł̃Q�[���̑z��
#define NET_SEND_SIZE_STANDARD (96)


#if 0
//>��������������������������������������������������������������������������������������
//>�����[�h��                    ������f�[�^�`��        ������|�P�����ő吔���o�C�g����
//>��������������������������������������������������������������������������������������
//>��0     �������_���}�b�`      ��POKEMON_PARAM         ��6                 ��220     ��
//>��������������������������������������������������������������������������������������
//>��1     ��GTS                 ��POKEMON_PARAM         ��1                 ��220     ��
//>��������������������������������������������������������������������������������������
//>��2     ��GTS�l�S�V�G�[�V���� ��POKEMON_PARAM         ��3                 ��220     ��
//>��������������������������������������������������������������������������������������
//>��3     ���o�g���r�f�I        ��POKE_BATTLEVIDEO_PARAM��12                ��108     ��
//>��������������������������������������������������������������������������������������
//>��4     ���o�g���T�u�E�F�C    ��_B_TOWER_POKEMON      ��3                 ��60      ��
//>��������������������������������������������������������������������������������������
#endif

typedef enum{
  NHTTP_POKECHK_RANDOMMATCH = 0,//�����_���}�b�`      ��POKEMON_PARAM         ��6                 ��220     ��
  NHTTP_POKECHK_GTS,   //GTS                 ��POKEMON_PASO_PARAM    ��1                 ��136     ��
  NHTTP_POKECHK_GTSNEGO,       // GTS�l�S�V�G�[�V���� ��POKEMON_PASO_PARAM    ��3                 ��136     ��
  NHTTP_POKECHK_VIDIO,   //     ���o�g���r�f�I        ��POKE_BATTLEVIDEO_PARAM��12                ��108     ��
  NHTTP_POKECHK_SUBWAY,  // ���o�g���T�u�E�F�C    ��_B_TOWER_POKEMON      ��3                 ��56      ��
} NHTTP_POKECHK_ENUM;

#include "net_irc.h"

/// WiFi�Ŏg��Heap�̃T�C�Y
#define GFL_NET_DWC_HEAPSIZE        (0x45400)                           //WIFI�N���u��HEAP��
#define GFL_NET_DWCLOBBY_HEAPSIZE   (0x60000)                           //WIFI�L���HEAP��
#define GFL_NET_DWC_BSUBWAY_HEAPSIZE  (0x50000)                        //�o�g���T�u�E�F�C��HEAP��
#define GFL_NET_DWC_RNDMATCH_HEAPSIZE (0x55000)                       //�����_���}�b�`���O��HEAP��


#define GAMESYNC_CGEAR_DOWNLOAD "CGEAR_J"
#define GAMESYNC_MUS_DOWNLOAD "MUSICAL_J"
#define GAMESYNC_ZUK_DOWNLOAD "ZUKAN_J"

