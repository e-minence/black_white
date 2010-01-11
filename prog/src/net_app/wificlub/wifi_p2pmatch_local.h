//=============================================================================
/**
 * @file	wifi_p2pmatch_local.h
 * @brief	WIFICLUB���[�J����`
 * @author	k.ohno
 * @date    2006.4.5
 */
//=============================================================================


#pragma once


#include <gflib.h>
#include "wifi_status.h"
#include "savedata/wifilist.h"
#include "savedata/config.h"
#include "print/wordset.h"
#include "wifi_p2pmatchroom.h"
#include "system/bmp_oam.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "net_app/connect_anm.h"
#include "poke_tool/pokeparty.h"


#define WIFIP2PMATCH_MEMBER_MAX  (WIFILIST_FRIEND_MAX)
#define WIFIP2PMATCH_DISP_MEMBER_MAX  (5)

// �@�\�{�^���p��`
#define FUNCBUTTON_NUM	( 7 ) 		// �@�\�{�^���̐�
#define START_WORDPANEL ( 0 )		// �ŏ��̕������̓p�l���̔ԍ��i�O���Ђ炪�ȁj

// CLACT�Œ�`���Ă���Z�����傫�����ăT�u��ʂɉe�����łĂ��܂��̂ŗ����Ă݂�
#define NAMEIN_SUB_ACTOR_DISTANCE 	(256)

// CellActor�ɏ��������郊�\�[�X�}�l�[�W���̎�ނ̐��i���}���`�Z���E�}���`�Z���A�j���͎g�p���Ȃ��j
#define CLACT_RESOURCE_NUM		(  4 )
#define _OAM_NUM			( 5 )

#define FRIENDCODE_MAXLEN     (12)

// �}�b�`���O���[���ɂ͂��Ă���l�̐��i����������āj
#define MATCHROOM_IN_NPCNUM	(32)
#define MATCHROOM_IN_OBJNUM	(MATCHROOM_IN_NPCNUM+1)

// ���b�Z�[�W�\�����WAIT
#define WIFIP2PMATCH_CORNER_MESSAGE_END_WAIT	( 60 )


// �����p�l���̑J�ڗp
enum{
  WIFIP2PMATCH_MODE_INIT  = 0,
  WIFIP2PMATCH_NORMALCONNECT_YESNO,
  WIFIP2PMATCH_NORMALCONNECT_WAIT,
  WIFIP2PMATCH_DIFFER_MACHINE_INIT,
  WIFIP2PMATCH_DIFFER_MACHINE_NEXT,
  WIFIP2PMATCH_DIFFER_MACHINE_ONEMORE,
  WIFIP2PMATCH_FIRST_YESNO,
  WIFIP2PMATCH_POWEROFF_INIT,
  WIFIP2PMATCH_POWEROFF_YESNO,
  WIFIP2PMATCH_POWEROFF_WAIT,
  WIFIP2PMATCH_RETRY_INIT,        //10
  WIFIP2PMATCH_RETRY_YESNO,
  WIFIP2PMATCH_RETRY_WAIT,
  WIFIP2PMATCH_RETRY,
  WIFIP2PMATCH_CONNECTING_INIT,
  WIFIP2PMATCH_CONNECTING,
  WIFIP2PMATCH_FIRST_ENDMSG,
  WIFIP2PMATCH_FIRST_ENDMSG_WAIT,
  WIFIP2PMATCH_FRIENDLIST_INIT,
  WIFIP2PMATCH_MODE_FRIENDLIST,
  WIFIP2PMATCH_VCHATWIN_WAIT,           //20
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2,
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT,
  WIFIP2PMATCH_MODE_VCT_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_DISCONNECT,
  WIFIP2PMATCH_MODE_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT,  //30
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT,
  WIFIP2PMATCH_MODE_MAIN_MENU,
  WIFIP2PMATCH_MODE_END_WAIT,
  WIFIP2PMATCH_MODE_CHECK_AND_END,
  WIFIP2PMATCH_MODE_SELECT_INIT,
  WIFIP2PMATCH_MODE_SELECT_WAIT,
  WIFIP2PMATCH_MODE_SUBBATTLE_WAIT,
  WIFIP2PMATCH_MODE_SELECT_REL_INIT,
  WIFIP2PMATCH_MODE_SELECT_REL_YESNO,       //40
  WIFIP2PMATCH_MODE_SELECT_REL_WAIT,
  WIFIP2PMATCH_MODE_MATCH_INIT,
  WIFIP2PMATCH_MODE_MATCH_INIT2,
  WIFIP2PMATCH_MODE_MATCH_WAIT,
  WIFIP2PMATCH_MODE_MATCH_LOOP,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT,
  WIFIP2PMATCH_MODE_CALL_INIT,
  WIFIP2PMATCH_MODE_CALL_YESNO,
  WIFIP2PMATCH_MODE_CALL_SEND,
  WIFIP2PMATCH_MODE_CALL_CHECK,             //50
  WIFIP2PMATCH_MODE_MYSTATUS_WAIT,
  WIFIP2PMATCH_MODE_CALL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_INIT,
  WIFIP2PMATCH_MODE_PERSONAL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_END,
  WIFIP2PMATCH_MODE_EXIT_YESNO,
  WIFIP2PMATCH_MODE_EXIT_WAIT,
  WIFIP2PMATCH_MODE_EXITING,
  WIFIP2PMATCH_MODE_EXIT_END,
  WIFIP2PMATCH_NEXTBATTLE_YESNO,         //60
  WIFIP2PMATCH_NEXTBATTLE_WAIT,
  WIFIP2PMATCH_MODE_VCHAT_NEGO,
  WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT,
  WIFIP2PMATCH_RECONECTING_WAIT,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT,
  WIFIP2PMATCH_PARENT_RESTART,
  WIFIP2PMATCH_FIRST_SAVING,
  WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT,
  WIFIP2PMATCH_FIRST_SAVING2,       //70
  WIFIP2PMATCH_MODE_CALLGAME_INIT, 

};


enum{
	WIFIP2PMATCH_SYNCHRONIZE_END=201,
};

// �㉺��ʎw���`
#define BOTH_LCD	( 2 )
#define MAIN_LCD	( GF_BGL_MAIN_DISP )	// �v�͂O��
#define SUB_LCD		( GF_BGL_SUB_DISP )		// �P�Ȃ�ł����B

// BMPWIN�w��
enum{
	BMP_NAME1_S_BG0,
	BMP_NAME2_S_BG0,
	BMP_NAME3_S_BG0,
	BMP_NAME4_S_BG0,
	BMP_NAME5_S_BG0,
	BMP_WIFIP2PMATCH_MAX,
};


typedef enum{
  _REGULATION_BATTLE_TOWER     // �o�g���^���[�ΐ����
} REGULATION_BATTLE_e;


//�g�[�^��189�o�C�g���M�ł��邪�Ƃ肠�������ʃv���O�����͔͈͕���������
#define _POKEMON_NUM   (6)

#define _CANCELENABLE_TIMER (60*30)   // �L�����Z���ɂȂ�ׂ̃^�C�}�[60min


typedef enum{
	WF_VIEW_STATUS,
	WF_VIEW_VCHAT,
	WF_VIEW_STATUS_NUM,
} WF_VIEW_STATUS_e;
/*
typedef enum{
	WF_USERDISPTYPE_NRML,	// �ʏ�
	WF_USERDISPTYPE_MINI,	// �~�j�Q�[��
	WF_USERDISPTYPE_BLTW,	// �o�g���^���[
	WF_USERDISPTYPE_BLFT,	// �o�g���t�����e�B�A
	WF_USERDISPTYPE_BLKS,	// �o�g���L���b�X��
	WF_USERDISPTYPE_BTST,	// �o�g���X�e�[�W
	WF_USERDISPTYPE_BTRT,	// �o�g�����[���b�g
	WF_USERDISPTYPE_NUM,
} WF_USERDISPTYPE__e;
*/
// ���[�U�\���{�^����
typedef enum{
	MCV_USERD_BTTN_LEFT = 0,
	MCV_USERD_BTTN_BACK,
	MCV_USERD_BTTN_RIGHT,
	MCV_USERD_BTTN_NUM,
} MCV_USERD_BTTN_e;

typedef struct _WIFI_MACH_STATUS_tag{
  u16 pokemonType[_POKEMON_NUM];
  u16 hasItemType[_POKEMON_NUM];
  u8 version;
  u8 regionCode;
  u8 pokemonZukan;
  u8 status;
  u8 regulation;
  u8 trainer_view;
  u8 sex;
  u8 nation;
  u8 area;
  u8 vchat;
  u8 vchat_org;
} _WIFI_MACH_STATUS;


//============================================================================================
//	�\���̒�`
//============================================================================================

typedef struct {
  _WIFI_MACH_STATUS myMatchStatus;   // �����̃}�b�`���O��ԃf�[�^
  //_WIFI_MACH_STATUS friendMatchStatus[WIFIP2PMATCH_MEMBER_MAX]; // ����̃}�b�`���O��ԃf�[�^
} TEST_MATCH_WORK;

//-------------------------------------
///	Icon�O���t�B�b�N
//=====================================
typedef struct {
	void* p_buff;
	NNSG2dScreenData* p_scrn;

	void* p_charbuff;
	NNSG2dCharacterData* p_char;
} WIFIP2PMATCH_ICON;


//-------------------------------------
///	�f�[�^�r���[�A�[�f�[�^
//=====================================
typedef struct {
	// ���[�h�Z�b�g
	WORDSET*	p_wordset;			// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
	
	// �{�^���O���t�B�b�N
	void* p_bttnbuff;
	NNSG2dScreenData* p_bttnscrn;

	void* p_userbuff[1];
	NNSG2dScreenData* p_userscrn[1];

	void* p_useretcbuff;
	NNSG2dScreenData* p_useretcscrn;
	
	GFL_BUTTON_MAN* p_bttnman;	// �{�^���Ǘ��V�X�e��
	u8 bttnfriendNo[ MATCHROOM_IN_NPCNUM ];
	u8 frame_no;	// ���\�����Ă��鏰�̃i���o�[
	u8 touch_friendNo;	// �G��Ă���F�B�ԍ�+1
	u8 touch_frame;	// �t���[����
	u8 user_disp;		// ���[�U�[�f�[�^�\��ONOFF
	s8 user_dispno;	// ���[�U�[DISP���e
	u8 button_on;		// �{�^���\���X�V
	u8 button_count;	// �{�^���t���[���J�E���^
	u8 bttn_chg_friendNo;	// �{�^�����X�V���Ăق����F�B�ԍ�
	BOOL bttn_allchg;	// �{�^�����ׂĂ��X�V���邩 

	// �\���r�b�g�}�b�v��
	GFL_BMPWIN*	  nameWin[ WCR_MAPDATA_1BLOCKOBJNUM ];
	GFL_BMPWIN*	  statusWin[ WCR_MAPDATA_1BLOCKOBJNUM ][ WF_VIEW_STATUS_NUM ];
	GFL_BMPWIN*	  userWin;

	// �{�^��
	u32	button_res[ 4 ];
  u32 btnCGRid;
  u32 btnCLRid;
  u32 btnCERid;

//	CLACT_WORK_PTR button_act[MCV_USERD_BTTN_NUM];
   GFL_CLWK* button_act[MCV_USERD_BTTN_NUM];
#if 1
//  CHAR_MANAGER_ALLOCDATA back_fontoam_cg;
  BMPOAM_SYS_PTR back_fontoam;	// ���ǂ�pFONTOAM
  BMPOAM_ACT_PTR BmpOamAct;
  GFL_BMP_DATA*  BmpOamBmp;
#endif
  u32 buttonact_on;			// �{�^�����샂�[�h
	u32 touch_button;
	u32 touch_button_event;
	GFL_BUTTON_MAN* p_oambttnman;	// �{�^���Ǘ��V�X�e��
} WIFIP2PMATCH_VIEW;



struct _WIFIP2PMATCH_WORK{
	GFL_TCB					*vblankFunc;
  WIFI_LIST* pList;				// �Z�[�u�f�[�^���̃��[�U�[�ƃt�����h�f�[�^
  WIFI_STATUS* pMatch;		// �T�[�o�[�Ƒ���M���鎩���̏��
//  TEST_MATCH_WORK* pMatch;		// �T�[�o�[�Ƒ���M���鎩���̏��

  int friendMatchReadCount;		// �r�[�R������M�����F�B��
  u8 index2No[WIFIP2PMATCH_MEMBER_MAX];	// �t�����h�i���o�[�z��
  u8 index2NoBackUp[WIFIP2PMATCH_MEMBER_MAX];	// �t�����h�i���o�[�z��o�b�N�A�b�v
  int matchStatusBackup[WIFIP2PMATCH_MEMBER_MAX];// �F�B�r�[�R���f�[�^���ς�����������p
  int matchVchatBackup[WIFIP2PMATCH_MEMBER_MAX];// �F�B�r�[�R���f�[�^���ς�����������p
//  NAMEIN_PARAM*		nameinParam;
  
  // ���C�����X�g�p���[�N
  BMP_MENULIST_DATA*   menulist;
  BMPMENULIST_WORK* lw;		// BMP���j���[���[�N
  
  BMP_MENULIST_DATA*   submenulist;
  BMPMENULIST_WORK* sublw;		// BMP���j���[���[�N
//  GFL_BG_INI		*bgl;									// GF_BGL_INI
  SAVE_CONTROL_WORK*  pSaveData;
GAMEDATA* pGameData;
  POKEPARTY* pMyPoke;
  void* pEmail;
  void* pFrontier;
  CONFIG* pConfig;
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_MSGDATA *MsgManager;							// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA *SysMsgManager;  //
  GFL_FONT 			*fontHandle;
 // STRBUF			*TrainerName[WIFIP2PMATCH_MEMBER_MAX];		// ���O
//  STRBUF			*MsgString;								// ���b�Z�[�W
  STRBUF*         pExpStrBuf;
  STRBUF			*TalkString;							// ��b���b�Z�[�W�p
  STRBUF			*TitleString;							// �^�C�g�����b�Z�[�W�p
//  STRBUF			*MenuString[4];							// ���j���[���b�Z�[�W�p
  STRBUF*         pTemp;        // ���͓o�^���̈ꎞ�o�b�t�@
  
  int				MsgIndex;								// �I�����o�p���[�N
  BMPMENU_WORK* pYesNoWork;
  void* timeWaitWork;			// �^�C���E�G�C�g�A�C�R�����[�N
//  CLACT_SET_PTR 			clactSet;								// �Z���A�N�^�[�Z�b�g
  GFL_CLUNIT* clactSet;								// �Z���A�N�^�[�Z�b�g
  GFL_CLSYS_REND*          renddata;						// �ȈՃ����_�[�f�[�^
  //CLACT_U_EASYRENDER_DATA	renddata;								// �ȈՃ����_�[�f�[�^
  //CLACT_U_RES_MANAGER_PTR	resMan[CLACT_RESOURCE_NUM];				// ���\�[�X�}�l�[�W��
//  FONTOAM_SYS_PTR			fontoam;								// �t�H���gOAM�V�X�e��  @@OO
/*
  CLACT_U_RES_OBJ_PTR 	resObjTbl[BOTH_LCD][CLACT_RESOURCE_NUM];// ���\�[�X�I�u�W�F�e�[�u��
  CLACT_HEADER			clActHeader_m;							// �Z���A�N�^�[�w�b�_�[
  CLACT_HEADER			clActHeader_s;							// �Z���A�N�^�[�w�b�_�[
  CLACT_WORK_PTR			MainActWork[_OAM_NUM];				// �Z���A�N�^�[���[�N�|�C���^�z��
	//CLACT_WORK_PTR			SubActWork[_OAM_NUM];				// �Z���A�N�^�[���[�N�|�C���^�z��
//*/

  GFL_BMPWIN*			MsgWin;									// ��b�E�C���h�E
  GFL_BMPWIN*			MyInfoWin;								// �����̏�ԕ\��
  GFL_BMPWIN*			MyInfoWinBack;								// �^�C�g��
  GFL_BMPWIN*			SysMsgWin;								// �V�X�e���E�B���h�E�ŕ`�悷�����	����[��ADWC���[���̃��b�Z�[�W
  GFL_BMPWIN*			ListWin;									// �t�����h���X�g
  GFL_BMPWIN*			SubListWin;									// ��W����^�C�v�Ȃǂ�`�悷�郊�X�g
  GFL_BMPWIN*			MyWin;									// �F�B�̌l���\��
  PRINT_UTIL            SysMsgPrintUtil;    // �V�X�e���E�C���h�EPrintUtil
  PRINT_QUE*            SysMsgQue;

   int cancelEnableTimer;   // �L�����Z�����ɂȂ�ׂ̃^�C�}�[
  int localTime;
  int seq;									// ���݂̕������͏�ԁi����OK/�A�j�����j�Ȃ�
  int       endSeq;
  int						nextseq;
  int initSeq;
  int						mode;									// ���ݍőO�ʂ̕����p�l��
  int						timer;			// �҂����Ԃ�A�t���O�ȂǂɎg�p
  MYSTATUS                *pMyStatus;		// �����̃X�e�[�^�X
  u8     mainCur;
  u8     subCur;
  BOOL bInitMessage;		// ����ڑ���
  GFL_PROC*		subProc;
   u16 matchState[WIFIP2PMATCH_MEMBER_MAX];   ///<CNM_WFP2PMF_STATUS
  int preConnect;			// �V���������F�B(-1=�Ȃ�)
  u16 battleCur;			// �o�g���^�C�v�I�����j���[�J�[�\��
  u16 singleCur[3];			// �o�g���̏ڍו����̃��j���[�J�[�\��
  u16 bSingle;				// SINGLE�o�g���@�_�u���o�g���X�C�b�`

 WIFI_STATUS targetStatus;  //�ڑ����悤�Ƃ��Ă���l�̃X�e�[�^�X

//  u16 keepStatus;			// �ڑ����悤�Ƃ����瑊��̃X�e�[�^�X���ς�����Ƃ��������p
//  u16 keepVChat;			// �ڑ����悤�Ƃ�����{�C�X�`���b�g��Ԃ��ς�����Ƃ��������p
  u16 friendNo;			// ���Ȃ����Ă���F�B�i���o�[
  BOOL bRetryBattle;
   int vctEnc;
	WIFI_MATCHROOM matchroom;	// �}�b�`���O���[�����[�N
	MCR_MOVEOBJ*	p_matchMoveObj[ MATCHROOM_IN_OBJNUM ];
	WIFIP2PMATCH_ICON icon;	// �A�C�R���O���t�B�b�N
	WIFIP2PMATCH_VIEW view;	// �F�B�f�[�^�r���[�A�[

	BOOL friend_request_wait;	// �F�B��W����TRUE�ɂȂ�t���O
	
	CONNECT_BG_PALANM cbp;		// Wifi�ڑ���ʂ�BG�p���b�g�A�j������\����

	u16 brightness_reset;	// _retry�Ń}�X�^�[�P�x��VBlank�Ń��Z�b�g���邽��
	u16 friend_num;			// P2Pmatch��ʏ������^�C�~���O�̗F�B��
};


extern void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);




