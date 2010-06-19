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
#include "net_app/wificlub/wifi_p2pmatch.h"

#include "wifi_status.h"
#include "savedata/wifilist.h"
#include "savedata/config.h"

#include "print/wordset.h"

#include "system/bmp_oam.h"
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"

#include "net_app/connect_anm.h"
#include "app/app_keycursor.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_regulation.h"

#include "wifi_p2pmatchroom.h"
#include "ui/touchbar.h"
#include "system/time_icon.h"
#include "app/app_printsys_common.h"


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


//�オ�߂�ۂ�����A�C�R���p���b�g
#define _OBJPLT_POKEICON_OFFSET (11*32)

































// �����p�l���̑J�ڗp
enum{
  WIFIP2PMATCH_MODE_INIT  = 0,
  WIFIP2PMATCH_RETRY_INIT,
  WIFIP2PMATCH_RETRY_YESNO,
  WIFIP2PMATCH_RETRY_WAIT,
  WIFIP2PMATCH_RETRY,
  WIFIP2PMATCH_CONNECTING_INIT,
  WIFIP2PMATCH_CONNECTING,
  WIFIP2PMATCH_FIRST_ENDMSG,
  WIFIP2PMATCH_FIRST_ENDMSG_WAIT,
  WIFIP2PMATCH_FRIENDLIST_INIT,
  WIFIP2PMATCH_FRIENDLIST_INIT2,
  WIFIP2PMATCH_MODE_FRIENDLIST,
  WIFIP2PMATCH_VCHATWIN_WAIT,           
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT2,
  WIFIP2PMATCH_MODE_VCT_CONNECT_INIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_VCT_CONNECT,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_YESNO,
  WIFIP2PMATCH_MODE_VCT_CONNECTEND_WAIT,
  WIFIP2PMATCH_MODE_VCT_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_DISCONNECT,
  WIFIP2PMATCH_MODE_DISCONNECT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_INIT, 
  WIFIP2PMATCH_MODE_BATTLE_CONNECT_WAIT,
  WIFIP2PMATCH_MODE_BATTLE_CONNECT,
  WIFIP2PMATCH_MODE_MAIN_MENU,
  WIFIP2PMATCH_MODE_END_WAIT,
  WIFIP2PMATCH_MODE_CHECK_AND_END,
  WIFIP2PMATCH_MODE_SELECT_INIT,
  WIFIP2PMATCH_MODE_SELECT_WAIT,
  WIFIP2PMATCH_MODE_SUBBATTLE_WAIT,
  WIFIP2PMATCH_MODE_SELECT_REL_INIT,
  WIFIP2PMATCH_MODE_SELECT_REL_YESNO,   
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
  WIFIP2PMATCH_MODE_CALL_CHECK,     
  WIFIP2PMATCH_MODE_MYSTATUS_WAIT,
  WIFIP2PMATCH_MODE_CALL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_INIT,
  WIFIP2PMATCH_MODE_PERSONAL_WAIT,
  WIFIP2PMATCH_MODE_PERSONAL_END,
  WIFIP2PMATCH_MODE_EXIT_YESNO,
  WIFIP2PMATCH_MODE_EXIT_WAIT,
  WIFIP2PMATCH_MODE_EXITING,
  WIFIP2PMATCH_MODE_EXIT_END,
  WIFIP2PMATCH_NEXTBATTLE_YESNO,     
  WIFIP2PMATCH_NEXTBATTLE_WAIT,
  WIFIP2PMATCH_MODE_VCHAT_NEGO,
  WIFIP2PMATCH_MODE_VCHAT_NEGO_WAIT,
  WIFIP2PMATCH_RECONECTING_WAIT,
  WIFIP2PMATCH_MODE_BCANCEL_YESNO_VCT,
  WIFIP2PMATCH_MODE_BCANCEL_WAIT_VCT,
  WIFIP2PMATCH_FIRST_SAVING,
  WIFIP2PMATCH_MODE_CANCEL_ENABLE_WAIT,
  WIFIP2PMATCH_FIRST_SAVING2,   
  WIFIP2PMATCH_MODE_CALLGAME_INIT,
  WIFIP2PMATCH_MODE_PLAYERDIRECT_INIT,
  WIFIP2PMATCH_PLAYERDIRECT_INIT1,
  WIFIP2PMATCH_PLAYERDIRECT_INIT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT3,
  WIFIP2PMATCH_PLAYERDIRECT_INIT5,
  WIFIP2PMATCH_PLAYERDIRECT_INIT6,
  WIFIP2PMATCH_PLAYERDIRECT_INIT7,
  WIFIP2PMATCH_PLAYERDIRECT_RETURN,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END,  
  WIFIP2PMATCH_PLAYERDIRECT_VCT,
  WIFIP2PMATCH_PLAYERDIRECT_TVT,
  WIFIP2PMATCH_PLAYERDIRECT_TRADE,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE1,
  WIFIP2PMATCH_PLAYERDIRECT_SUB1,
  WIFIP2PMATCH_PLAYERDIRECT_SUB2,
  WIFIP2PMATCH_PLAYERDIRECT_SUBSTART,
  WIFIP2PMATCH_PLAYERDIRECT_SUBSTARTCALL,
  WIFIP2PMATCH_PLAYERDIRECT_ENDCALL,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_MODE2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE,   //���[��
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_RULE2,   //���[��
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER,  //�V���[�^�[
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_SHOOTER2,  //�V���[�^�[
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_DECIDE,  //����
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH,  //���[�������� 
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_WATCH2,  //���[�������� 
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO1,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO4,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START3,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START4,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START5,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START6,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START7,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_TEMOTI,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_BBOX,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_NOREG_SELECT_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED2,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_FAILED3,
  WIFIP2PMATCH_MESSAGEEND_RETURNLIST,
  WIFIP2PMATCH_RETURNLIST,
  WIFIP2PMATCH_PLAYERMACHINE_INIT1,
  WIFIP2PMATCH_PLAYERMACHINE_NOREG_PARENT,
  WIFIP2PMATCH_PLAYERMACHINE_BATTLE_DECIDE,
  WIFIP2PMATCH_MODE_CHILD_CONNECT,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT1,
  WIFIP2PMATCH_PLAYERDIRECT_NOREG_PARENT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT1,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_12,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_13,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO_14,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE1,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE2,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE3,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE4,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE5,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE6,
  WIFIP2PMATCH_MODE_SELECT_INIT2,
  WIFIP2PMATCH_PLAYERDIRECT_END2,
  WIFIP2PMATCH_PLAYERDIRECT_ENDCALL2,
  WIFIP2PMATCH_PLAYERDIRECT_VCTCHANGE_E,
  WIFIP2PMATCH_MODE_CALL_CHECK_D,
  WIFIP2PMATCH_MODE_CONNECTWAIT,
  WIFIP2PMATCH_MODE_CONNECTWAIT2,
  WIFIP2PMATCH_PLAYERDIRECT_WAIT_COMMAND,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_GO3_KEYWAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END_KEYWAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END3,
  WIFIP2PMATCH_PLAYERDIRECT_SUB3,
  WIFIP2PMATCH_PLAYERDIRECT_SUB_FAILED,
  WIFIP2PMATCH_MODE_DISCONNECT2,
  WIFIP2PMATCH_VCTEND_COMMSEND1,
  WIFIP2PMATCH_VCTEND_COMMSEND2,
  WIFIP2PMATCH_VCTEND_COMMSEND3,
  WIFIP2PMATCH_MODE_VCT_CONNECT_MAIN,
  WIFIP2PMATCH_PLAYERDIRECT_SUB23,
  WIFIP2PMATCH_PLAYERMACHINE_TALKEND,
  WIFIP2PMATCH_PLAYERDIRECT_BATTLE_START42,
  WIFIP2PMATCH_PLAYERDIRECT_END_NEXT,
  WIFIP2PMATCH_VCTEND_COMMSEND2_ENDCHK,
  WIFIP2PMATCH_PLAYERDIRECT_CANCELEND,
  WIFIP2PMATCH_PLAYERDIRECT_CANCELEND_NEXT,
  WIFIP2PMATCH_PLAYERDIRECT_SUB45,
  WIFIP2PMATCH_MODE_FRIENDLIST_MW,
  WIFIP2PMATCH_RECONECTING_WAIT_PRE,
  WIFIP2PMATCH_MODE_TVTMESSAGE1_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE1_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE2_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE2_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE3_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE3_WAIT,
  WIFIP2PMATCH_MODE_TVTMESSAGE4_YESNO,
  WIFIP2PMATCH_MODE_TVTMESSAGE4_WAIT,
  WIFIP2PMATCH_PLAYERDIRECT_END_CHILD,
  WIFIP2PMATCH_PLAYERDIRECT_END_CHILD_NEXT,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT2,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT3,
  WIFIP2PMATCH_PLAYERDIRECT_INIT_NEXT0,
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

///���M�����[�V�����F�m�[�}���F
#define REG_NORMAL_COLOR      PRINTSYS_MACRO_LSB(1, 2, 0)
///���M�����[�V�����FNG�F
#define REG_FAIL_COLOR        PRINTSYS_MACRO_LSB(4, 2, 0)


#define MSG_WHITE_COLOR      PRINTSYS_MACRO_LSB(15, 2, 0)


typedef enum{
  REGWIN_TYPE_RULE,        ///<���[��������
  REGWIN_TYPE_RULE_SHORT,
  REGWIN_TYPE_NG_TEMOTI,   ///<�莝��NG�`�F�b�N
  REGWIN_TYPE_NG_BBOX,     ///<�o�g���{�b�N�XNG�`�F�b�N
} REGWIN_TYPE;

//u16 singleCur[_MENUTYPE_MAX];			// �o�g���^�C�v�I�����j���[�J�[�\���̎��
typedef enum{
  _MENUTYPE_BATTLE_CUSTOM,
  _MENUTYPE_BATTLE_MODE,
  _MENUTYPE_BATTLE_RULE,
  _MENUTYPE_BATTLE_SHOOTER,
  _MENUTYPE_GAME,
  _MENUTYPE_POKEPARTY,
  _MENUTYPE_MAX
} _MENUTYPE_;


#define _CANCELENABLE_TIMER (60*60)   // �L�����Z���ɂȂ�ׂ̃^�C�}�[60min


typedef enum{
	WF_VIEW_STATUS,
	WF_VIEW_VCHAT,
	WF_VIEW_STATUS_NUM,
} WF_VIEW_STATUS_e;

// ���[�U�\���{�^����
typedef enum{
	MCV_USERD_BTTN_LEFT = 0,
	MCV_USERD_BTTN_BACK,
	MCV_USERD_BTTN_RIGHT,
	MCV_USERD_BTTN_NUM,
} MCV_USERD_BTTN_e;

//�v���C���[�̏�
typedef enum
{
  WIFIP2PMATCH_STATE_NONE,     //�����s���Ă��Ȃ�
  WIFIP2PMATCH_STATE_MACHINE,  //�}�V�[���ɂĕ�W
  WIFIP2PMATCH_STATE_MACHINE_RECV,  //��W�ɎQ������
  WIFIP2PMATCH_STATE_TALK,     //�b���������i�e�j
  WIFIP2PMATCH_STATE_RECV,     //�b��������ꂽ�i�q�j
} WIFIP2PMATCH_STATE;

//���M�����[�V�����쐬�^�C�v
typedef enum{
  REG_CREATE_TYPE_DECIDE, ///<���肵�Ă��郋�[��������
  REG_CREATE_TYPE_SELECT, ///<�I��ł��郋�[��������
}REG_CREATE_TYPE;

//-------------------------------------
///	Icon�O���t�B�b�N
//=====================================
typedef struct {
	void* p_buff;
	NNSG2dScreenData* p_scrn;

	void* p_charbuff;
	NNSG2dCharacterData* p_char;
  u32 wf_match_all_iconcgx1m;
  u32 wf_match_all_iconcgx2s;

} WIFIP2PMATCH_ICON;


typedef struct {
  u8 bgmVol;        ///< BGM�{�����[���ύX
  u8 bgmVolStart;    ///< BGM�{�����[���ύX�l
  u8 bgmVolEnd;    ///< BGM�{�����[���Ō�̒l
  u8 bgmVolCount;    ///< BGM�{�����[���ύX�J�E���g
} WIFIP2PMATCH_VOL;


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
//	NNSG2dScreenData* p_useretcscrn;
	
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
	GFL_BMPWIN*	  userWinStatus;
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
  u8 matchStatusBackup[WIFIP2PMATCH_MEMBER_MAX];// �F�B�r�[�R���f�[�^���ς�����������p
  u8 matchGamemodeBackup[WIFIP2PMATCH_MEMBER_MAX];// �F�B�r�[�R���f�[�^���ς�����������p
  u8 matchVchatBackup[WIFIP2PMATCH_MEMBER_MAX];// �F�B�r�[�R���f�[�^���ς�����������p
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
  POKEPARTY* bb_party;
  void* pEmail;
  void* pFrontier;
  CONFIG* pConfig;
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_MSGDATA *MsgManager;							// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA *SysMsgManager;  //
  GFL_FONT 			*fontHandle;
  GFL_FONT 			*font_handle_num;
 // STRBUF			*TrainerName[WIFIP2PMATCH_MEMBER_MAX];		// ���O
//  STRBUF			*MsgString;								// ���b�Z�[�W
  STRBUF*         pExpStrBuf;
  STRBUF			*TalkString;							// ��b���b�Z�[�W�p
  STRBUF			*TitleString;							// �^�C�g�����b�Z�[�W�p
//  STRBUF			*MenuString[4];							// ���j���[���b�Z�[�W�p
  STRBUF*         pTemp;        // ���͓o�^���̈ꎞ�o�b�t�@
  TIMEICON_WORK* pTimeIcon;
  
  int				MsgIndex;								// �I�����o�p���[�N
  BMPMENU_WORK* pYesNoWork;
  void* timeWaitWork;			// �^�C���E�G�C�g�A�C�R�����[�N

  GFL_CLUNIT* clactSet;								// �Z���A�N�^�[�Z�b�g
  GFL_CLSYS_REND*          renddata;						// �ȈՃ����_�[�f�[�^

  u32 cellRes[12];
  u32 cellResPal;
  u32 cellResAnm;
  GFL_CLWK* pokemonIcon[12];
  
  GFL_CLWK* pTrgra;   //�g���[�i�[�̑傫���G
  u32 trRes[3];       //�g���[�i�[�̃��\�[�X

  GFL_BMPWIN*			MsgWin;									// ��b�E�C���h�E
  GFL_BMPWIN*			MyInfoWin;								// �����̏�ԕ\��
  GFL_BMPWIN*			MyInfoWinBack;								// �^�C�g��
  GFL_BMPWIN*			SysMsgWin;								// �V�X�e���E�B���h�E�ŕ`�悷�����	����[��ADWC���[���̃��b�Z�[�W
  GFL_BMPWIN*			ListWin;									// �t�����h���X�g
  GFL_BMPWIN*			SubListWin;									// ��W����^�C�v�Ȃǂ�`�悷�郊�X�g
  GFL_BMPWIN*			MyWin;									// �F�B�̌l���\��
  PRINT_UTIL            SysMsgPrintUtil;    // �V�X�e���E�C���h�EPrintUtil
  PRINT_QUE*            SysMenuQue;
  PRINT_QUE*            SysMsgQue;
  PRINT_STREAM* pStream;
	GFL_TCBLSYS *pMsgTcblSys;

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
  u16 matchGameMode[WIFIP2PMATCH_MEMBER_MAX];   ///<CNM_WFP2PMF_STATUS
  int preConnect;			// �V���������F�B(-1=�Ȃ�)
  u16 singleCur[_MENUTYPE_MAX];			// �o�g���^�C�v�I�����j���[�J�[�\��
//  u16 singleCur[3];			// �o�g���̏ڍו����̃��j���[�J�[�\��
  u16 bSingle;				// SINGLE�o�g���@�_�u���o�g���X�C�b�`
  WIFIP2PMATCH_PROC_PARAM* pParentWork;
  WIFI_STATUS targetStatus;  //�ڑ����悤�Ƃ��Ă���l�̃X�e�[�^�X
  APP_PRINTSYS_COMMON_WORK trgWork;  //�L�[�̐�����s�����[�N

  u16 friendNo;			// ���Ȃ����Ă���F�B�i���o�[
  BOOL bRetryBattle;
  int vctEnc;
	WIFI_MATCHROOM matchroom;	// �}�b�`���O���[�����[�N
	MCR_MOVEOBJ*	p_matchMoveObj[ MATCHROOM_IN_OBJNUM ];
	WIFIP2PMATCH_ICON icon;	// �A�C�R���O���t�B�b�N
	WIFIP2PMATCH_VIEW view;	// �F�B�f�[�^�r���[�A�[
  u32 bgchrSubButton;
  u32 bgchrSubBack;
  TOUCHBAR_WORK* pTouchWork;

  APP_KEYCURSOR_WORK* pKeyCursor;
  
	BOOL friend_request_wait;	// �F�B��W����TRUE�ɂȂ�t���O
	
	CONNECT_BG_PALANM cbp;		// Wifi�ڑ���ʂ�BG�p���b�g�A�j������\����

	u16 brightness_reset;	// _retry�Ń}�X�^�[�P�x��VBlank�Ń��Z�b�g���邽��
	u16 friend_num;			// P2Pmatch��ʏ������^�C�~���O�̗F�B��
 	u16 directmode;			// ���ډ�b���̃Q�[�����[�h

  u32 talkwin_m2;
  u32 menuwin_m2;
  u32 menuwin_m1;
  u32 matchRegulation;  //�ʐM�ŖႤ���葤�̑I���������M�����[�V����
	WIFIP2PMATCH_VOL aVol;
  REGULATION_PRINT_MSG* rpm;  // ���M�����[�V�����v�����g���b�Z�[�W
  REGULATION* pRegulation;
  u16 battleMode;  //�o�g���̃��[�h
  u16 battleRule;  //�o�g���̃��[��
  u8 DirectModeNo;    // �_�C���N�g�ɂ͂Ȃ��������Ă���ꍇ�A�b���Ă������NO
  u8 DirectMacSet;  //�b��������ׂ�MAC���Z�b�g�����l
  u8 keepGameMode;
  u8 command;   ///< direct�i�s�ׂ̈̃R�}���h
  u8 backupCursor;
  u8 VChatModeOff;  //VCTOFF���Ă΂ꂽ
  u8 vchatrev;
  u8 dummy;

  WIFIP2PMATCH_STATE state; //�v���C���[�̏�
};

//�ʐM�֘A�̊֐�
extern void WifiP2PMatchRecvGameStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvMyStatus(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvDirectMode(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvBattleRegulation(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvPokeParty(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
extern void WifiP2PMatchRecvVctOff(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);

extern u8* WifiP2PMatchGetPokePartyWork(int netID, void* pWk, int size);
extern void WifiP2PMatchMessage_TimeIconStart(WIFIP2PMATCH_WORK* wk);
extern void WIFIP2PMatch_pokePartyMenuDelete( WIFIP2PMATCH_WORK* wk );



