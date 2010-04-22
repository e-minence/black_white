//=============================================================================
/**
 * @file	  pokemontrade_local.h
 * @brief	  �|�P�������� ���[�J�����L��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#if PM_DEBUG
#define _TRADE_DEBUG (1)
#else
#define _TRADE_DEBUG (0)
#endif

#include "../../field/event_ircbattle.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"

#include "pokemontrade_snd.h"
#include "sound/pm_sndsys.h"
#include "net/network_define.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p

#include "savedata/mail_util.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "system/time_icon.h"
#include "app/app_taskmenu.h"
#include "pokemontrade_anim.h"
#include "system/ica_anime.h"
#include "ui/ui_easy_clwk.h"
//�^�b�`�o�[
#include "ui/touchbar.h"
#include "savedata/mail_util.h"
#include "progval.h"

#include "app/app_printsys_common.h"
#include "net/net_save.h"
#include "net/nhttp_rap.h"
#include "pm_define.h"
#include "savedata/wifihistory.h"


///3D���f���̃^�C�v
typedef enum
{
  REEL_PANEL_OBJECT,
  TRADE01_OBJECT,
  TRADEIR_OBJECT,
  TRADEUP_OBJECT,
  TRADEMIDDLE_OBJECT,
  TRADEDOWN_OBJECT,
  OBJECT_MODEL_MAX,
} D3_OBJECT_MODEL;


typedef enum
{
  SETUP_TRADE_BG_MODE_NORMAL,
  SETUP_TRADE_BG_MODE_DEMO,
} SETUP_TRADE_BG_MODE;


typedef enum
{
  _DEMO_TYPE_ALL,
  _DEMO_TYPE_UP,
  _DEMO_TYPE_MIDDLE,
  _DEMO_TYPE_DOWN,
}_DEMO_TYPE_;



typedef enum
{
  _CHANGERAND = 43,    ///< �|�P���������^�C�~���O�h�炬��
  _TIMING_ENDNO=12,
  _TIMING_TRADEDEMO_START,
  _TIMING_SAVEST,
  _TIMING_SAVELAST,
  _TIMING_SAVEEND,
  _TIMING_ANIMEEND,
  _TIMING_POKECOLOR,
  _TIMING_RETURN,     //�����Ă��������������Ă��āA���肪��߂������͓������Ƃ��Č��̃V�[�P���X�ɖ߂�
  _TIMING_RETURN2,
  _TIMING_EVIL,
  _TIMING_FIRSTBOTH,
  _TIMING_UNDATA,

  POKETRADE_FACTOR_TIMING_A,   ///->�������POKETRADE_FACTOR_NONE�ɂ���^�C�~���O
  POKETRADE_FACTOR_TIMING_B,   ///->POKETRADE_FACTOR_SINGLECHANGE ��POKETRADE_FACTOR_TRI_SELECT�� POKETRADE_FACTOR_END�̃^�C�~���O
  POKETRADE_FACTOR_TIMING_C,   //  POKETRADE_FACTOR_SINGLE_OK  ��POKETRADE_FACTOR_SINGLE_NG��  POKETRADE_FACTOR_EGG
  POKETRADE_FACTOR_TIMING_D,  ///->POKETRADE_FACTOR_TRI_DECIDE��POKETRADE_FACTOR_TRI_END�̃^�C�~���O
  POKETRADE_FACTOR_TIMING_E,  ///->POKETRADE_FACTOR_TRI_LAST��POKETRADE_FACTOR_TRI_BACK�̃^�C�~���O
  POKETRADE_FACTOR_TIMING_F,  ///->POKETRADE_FACTOR_TRI_DECIDE����POKETRADE_FACTOR_TRI_LAST�ւ̂܂�
} NET_TIMING_ENUM;


typedef enum
{
  POKETRADE_FACTOR_NONE,            //�܂��I��Ŗ���
  POKETRADE_FACTOR_SINGLECHANGE,    //��̑I���݂�����
  POKETRADE_FACTOR_SINGLE_OK,    //��̑I��OK
  POKETRADE_FACTOR_SINGLE_NG,    //��̑I��NG
  POKETRADE_FACTOR_END,            //�I���I��
  POKETRADE_FACTOR_EGG,        // ���܂��ƍŌ�̎莝������
  POKETRADE_FACTOR_MAIL,      //���[���{�b�N�X�������ς�
  POKETRADE_FACTOR_WAZA,      //�Z���܂���
  POKETRADE_FACTOR_TRI_SELECT,   //  �R�̑I���ɂ�����

  POKETRADE_FACTOR_TRI_DECIDE,   //  �R������������
  POKETRADE_FACTOR_TRI_END,     //  �O�ΑI�����߂�

  POKETRADE_FACTOR_TRI_LAST,   //��������
  POKETRADE_FACTOR_TRI_BACK,   //������x3�̑I����

}POKETRADE_FACTOR_ENUM;





#define BOX_VERTICAL_NUM (5)
#define BOX_HORIZONTAL_NUM (6)

#define HAND_VERTICAL_NUM (3)
#define HAND_HORIZONTAL_NUM (2)


//#define BOX_MONS_NUM (30)


#define _BRIGHTNESS_SYNC (2)  // �t�F�[�h�̂r�x�m�b�͗v����



//�p���b�g�̒�`�͂����Ă��ǂ���
#define _FONT_PARAM_LETTER_BLUE (0x5)
#define _FONT_PARAM_SHADOW_BLUE (0x6)
#define _FONT_PARAM_LETTER_RED (0x3)
#define _FONT_PARAM_SHADOW_RED (0x4)
#define	FBMP_COL_WHITE		(15)


#define PLTID_OBJ_TYPEICON_M (8) // //3�{
#define PLTID_OBJ_BALLICON_M (12)  // 1�{�g�p
#define PLTID_OBJ_POKESTATE_M (13) //
#define PLTID_OBJ_POKERUS_M (15) //
#define PLTID_OBJ_DEMO_M (7)
#define _OBJPLT_GTS_POKEICON_OFFSET_M (0)


//#define _OBJPLT_POKEICON_GRAY_OFFSET (0) ///< �O���[�p�|�P�����A�C�R���p���b�g
//#define _OBJPLT_POKEICON_GRAY  (3)  //�|�P�����A�C�R���O���C 3�{

//���͒ʐM�A�C�R���o���Ȃ����������{�\
#define _TOUCHBAR_OBJ_PALPOS  (0)  //�^�b�`�o�[�̃p���b�g
#define _OBJPLT_COMMON_OFFSET (0)   //�A�v���P�[�V�������j��
#define _OBJPLT_COMMON  (TOUCHBAR_OBJ_PLT_NUM)  //�T�u���OBJ�p���b�g����ʃo�[ 3�{
#define PLTID_OBJ_POKEITEM_S (0xc) //
#define PLTID_OBJ_BALLICON_S (0xd)  // 1�{�g�p

#define _OBJPLT_POKEICON_OFFSET (3*0x20)
#define _OBJPLT_POKEICON  (3)  //�|�P�����A�C�R�� 3�{
#define _OBJPLT_BOX_OFFSET (_OBJPLT_POKEICON_OFFSET+_OBJPLT_POKEICON*32)
#define _OBJPLT_BOX  (6)  //�T�u���OBJ�p���b�g��{  5�{
#define _PALETTE_OAMMSG (0x0e) //���b�Z�[�WOAM�p


//PLTID_OBJ_POKEITEM_S

//--- TOUCH2
//--- POKE5
//------  SCROLLb



#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL (5)  //���C����ʁA�������̃p���b�g
#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL_RED (12)  //���C����ʁA�������̃p���b�g
#define _POKEMON_MAIN_FRIENDGIVEMSG_PAL_BLUE (14)  //���C����ʁA�������̃p���b�g

#define _TRADE_BG_PALLETE_NUM (6)  //����BG�̃p���b�g�̖{��   0-4
#define _TOUCHBAR_BG_PALPOS (7)     //�^�b�`�o�[�̐F     TOUCHBAR_BG_PLT_NUM �i�P�j�{

#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g ef0
#define _STATUS_MSG_PAL   (11)  // ���b�Z�[�W�t�H���g

#define _MAINBG_APP_MSG_PAL (13)
//���b�Z�[�W�n�͏㉺���� BG
#define _BUTTON_MSG_PAL   (14)  // ���b�Z�[�W�t�H���g
#define _BUTTON_WIN_PAL   (15)  // �E�C���h�E


//#define PLIST_RENDER_MAIN (1)


#define _POKE_PRJORTH_Y_COEFFICIENT (12)
#define _POKE_PRJORTH_X_COEFFICIENT (16)


#define _POKE_PRJORTH_TOP   (FX32_ONE*_POKE_PRJORTH_Y_COEFFICIENT)    ///< PRJORTH	��top	  :near�N���b�v�ʏ�ӂ�Y���W
#define _POKE_PRJORTH_RIGHT (FX32_ONE*_POKE_PRJORTH_X_COEFFICIENT)  ///< PRJORTH	��right	  :near�N���b�v�ʉE�ӂ�X���W

#define _MCSS_POS_X(x) (x * FX32_ONE)
#define _MCSS_POS_Y(y) (y * FX32_ONE)
#define _MCSS_POS_Z(z) (z * FX32_ONE)

#define PSTATUS_MCSS_POS_X1 _MCSS_POS_X(-55)            //�����ŏ��̈ʒuX
#define PSTATUS_MCSS_POS_X2 _MCSS_POS_X(55)             //����ŏ��̈ʒuX
#define PSTATUS_MCSS_POS_Y  _MCSS_POS_Y(-28)            //�ŏ��̈ʒuY�͋���



#define PSTATUS_MCSS_POS_MYZ   (0)
#define PSTATUS_MCSS_POS_YOUZ  (0)



#define YESNO_CHARA_OFFSET	(21 * 20 )
#define YESNO_CHARA_W		( 8 )
#define YESNO_CHARA_H		( 4 )

#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (5)    // �E�C���h�E����


#define CONTROL_PANEL_Y (TOUCHBAR_ICON_Y+12)


#define CHANGEBUTTON_X (72)
#define CHANGEBUTTON_Y (CONTROL_PANEL_Y)
#define CHANGEBUTTON_WIDTH  (5*8)
#define CHANGEBUTTON_HEIGHT (3*8)
#define WINCLR_COL(col)	(((col)<<4)|(col))




#define YESBUTTON_X (200)
#define YESBUTTON_Y (160)
#define NOBUTTON_X (200)
#define NOBUTTON_Y (180)

#define _SUBMENU_LISTMAX (8)

#define _LING_LINENO_MAX (12)  //�����O�o�b�t�@�ő�
//#define _G3D_MDL_MAX (1)  //���f���̐�

/// �h�b�g�P�ʂňʒu���Ǘ�  8*20��BOX 8*12���Ă��� BOX_MAX_TRAY => 2880+96=2976

//#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))

#define _STARTDOT_OFFSET (-80)

//GTS�����ŃX�e�[�^�X��������b��
#define _SLIDE_END_TIME (120)


typedef void (StateFunc)(POKEMON_TRADE_WORK* pState);






typedef enum
{
  POKEMONTORADE_SEQ_WAIT,  //�ҋ@��
  POKEMONTORADE_SEQ_MISERUOK,  //�������񂵂������Ă���
  POKEMONTORADE_SEQ_CANCEL, //��߂������Ă���
  POKEMONTORADE_SEQ_NONE,
}POKEMONTORADE_SEQ_ENUM;



typedef enum
{
  PLT_POKEICON,   //ARCID_POKEICON
  PAL_SCROLLBAR, //ARCID_POKETRADE
  PAL_SCROLLBAR_UP, //ARCID_POKETRADE
  PLT_COMMON,
  PLT_POKECREATE,
 // PLT_POKEICON_GRAY,
  PLT_GTS_POKEICON,
  PLT_RESOURCE_MAX,

  CHAR_SCROLLBAR = PLT_RESOURCE_MAX,    //ARCID_POKETRADE
  CHAR_SCROLLBAR_UP,    //ARCID_POKETRADE
  CHAR_SELECT_POKEICON1, //�I�𒆃|�P�����A�C�R���P
  CHAR_SELECT_POKEICON2, //�I�𒆃|�P�����A�C�R���P
  CHAR_COMMON,
  CHAR_POKECREATE,
  CHAR_RESOURCE_MAX,

  ANM_POKEICON = CHAR_RESOURCE_MAX,  //ARCID_POKEICON
  ANM_SCROLLBAR, //ARCID_POKETRADE
  ANM_SCROLLBAR_UP, //ARCID_POKETRADE
  ANM_COMMON,
  ANM_POKECREATE,
  ANM_GTS_POKEICON,
  ANM_RESOURCE_MAX,

  CEL_RESOURCE_MAX=ANM_RESOURCE_MAX,
} _CELL_RESOURCE_TYPE;

typedef enum
{
  CELL_CUR_SELECT,
  CELL_CUR_MOJI,
  CELL_CUR_SCROLLBAR,
  CELL_CUR_POKE_BASEPANEL,
  CELL_CUR_POKE_SELECT,
  CELL_CUR_POKE_PLAYER,
  CELL_CUR_POKE_FRIEND,
  CHAR_LEFTPAGE_MARK,
  CHAR_RETURNPAGE_MARK,
  CELL_CUR_POKE_KEY,
  CELL_DISP_NUM
} _CELL_DISP_TYPE;



// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_BALL,
  SETUP_INDEX_ROUND,
  SETUP_INDEX_MAX
};


// �畬�o��
enum{
  GTS_ERUPTED_BUTTON_BASE,
  GTS_ERUPTED_BUTTON_MARK,
  GTS_ERUPTED_BUTTON_NUM,
};

// ��}�[�N�{�^��
enum{
  GTS_FACE_BUTTON_GOOD,
  GTS_FACE_BUTTON_BAD,
  GTS_FACE_BUTTON_HART,
  GTS_FACE_BUTTON_ACK,
  GTS_FACE_BUTTON_NUM,
};

// �U�̌��������̎��̕���
enum{
  GTS_SEL6MSG_MY_NAME,
  GTS_SEL6MSG_MY_POKE1,
  GTS_SEL6MSG_MY_POKE2,
  GTS_SEL6MSG_MY_POKE3,
  GTS_SEL6MSG_FRIEND_NAME,
  GTS_SEL6MSG_FRIEND_POKE1,
  GTS_SEL6MSG_FRIEND_POKE2,
  GTS_SEL6MSG_FRIEND_POKE3,
  GTS_SEL6MSG_NUM,
};

typedef enum{
  PAL_SEL6MSG,
  ANM_SEL6MSG,
} OAMRESOURCE_SEL6MSG;

// �|�P�����X�e�[�^�X�������Ƒ����؂�ւ���ۂ�CELL�̈ʒu
#define _POKEMON_SELECT1_CELLX  (128-32)
#define _POKEMON_SELECT1_CELLY  (12)
#define _POKEMON_SELECT2_CELLX  (128+32)
#define _POKEMON_SELECT2_CELLY  (12)

// ���{�ꌟ������MAX
#define JAPANESE_SEARCH_INDEX_MAX (44)

//GTS�Ō�������|�P����MAX
#define GTS_NEGO_POKESLT_MAX  (3)
#define GTS_NEGO_UPSTATUSMSG_MAX (6)
#define GTS_PLAYER_WORK_NUM  (2)


#define GTSFACEICON_POSX (8)
#define GTSFACEICON_POSY (144)
#define GTSFACEICON_HEIGHT (24)
#define GTSFACEICON_WIDTH (24)

/// @brief 3D���f���J���[�t�F�[�h
typedef struct
{
  GFL_G3D_RES* g3DRES;             //�p���b�g�t�F�[�h  �Ώ�3D���f�����\�[�X�ւ̃|�C���^
  void*        pData_dst;          //�p���b�g�t�F�[�h  �]���p���[�N
  u8            pal_fade_flag;      //�p���b�g�t�F�[�h  �N���t���O
  u8            pal_fade_count;     //�p���b�g�t�F�[�h  �Ώۃ��\�[�X��
  u8            pal_fade_start_evy; //�p���b�g�t�F�[�h�@START_EVY�l
  u8            pal_fade_end_evy;		//�p���b�g�t�F�[�h�@END_EVY�l
  u8            pal_fade_wait;			//�p���b�g�t�F�[�h�@wait�l
  u8            pal_fade_wait_tmp;	//�p���b�g�t�F�[�h�@wait_tmp�l
  u16           pal_fade_rgb;				//�p���b�g�t�F�[�h�@end_evy����rgb�l
}_EFFTOOL_PAL_FADE_WORK;

/// @brief 2D�ʃt�F�[�h
typedef struct
{
  s8   pal_fade_time;			//�p���b�g�t�F�[�h �g�[�^������
  s8   pal_fade_nowcount;	//�p���b�g�t�F�[�h ���݂̐i�s����
  s8   pal_start;
  s8   pal_end;
}_D2_PAL_FADE_WORK;



/// @brief �|�P�������W�ړ�
typedef struct
{
  MCSS_WORK* pMcss;  ///< ����������
  int   time;			   ///< �g�[�^������
  int   nowcount;	   ///<  ���݂̐i�s����
  float   percent;   ///< ��������v�Z
  float   add;   ///< �ω���
  u16 sins;
  u16 wave;
  u16 waveNum;
  VecFx32  start;      ///< �J�n�ʒu
  VecFx32  end;        ///< �I���ʒu
  VecFx32* MoveTbl;  ///< �e�[�u���ړ��̍ۂ̍��W
} _POKEMCSS_MOVE_WORK;


typedef enum
{  // �{�[���A�C�R���^�C�v
  UI_BALL_SUBSTATUS,     ///< �T�u�X�e�[�^�X�p
  UI_BALL_MYSTATUS,      ///< ���C����ʎ�����
  UI_BALL_FRIENDSTATUS,  ///< ���C����ʑ����

  UI_BALL_NUM
} _UI_BALLICON_TYPE;



typedef struct
{
  // �|�P�A�C�R���p�A�C�e���A�C�R��
  UI_EASY_CLWK_RES      clres_poke_item;
  GFL_CLWK                  *clwk_poke_item;
} _ITEMMARK_ICON_WORK;


typedef struct
{
  // �{�[���A�C�R��
  UI_EASY_CLWK_RES      clres_ball;
  GFL_CLWK              *clwk_ball;
} _BALL_ICON_WORK;


typedef struct
{
  // �^�C�v�A�C�R��
  UI_EASY_CLWK_RES      clres_type_icon;
  GFL_CLWK                  *clwk_type_icon;
} _TYPE_ICON_WORK;

#define _POKEMARK_MAX  (8)

#include "pokemontrade_demo_local.h"


typedef struct
{
  u16 bAreaOver;   //���ł��G���A�O�ɏo�����ǂ���
} PENMOVE_WORK;


/// �����߂��悤�o�b�N�A�b�v�\����
typedef struct
{
  MAIL_BLOCK* pMail;     
  RECORD* pRecord;
  WIFI_HISTORY* pWifiHis;
  ZUKAN_SAVEDATA* pZukan;
  WIFI_NEGOTIATION_SAVEDATA* pNego;
  POKEPARTY* pPokeParty;
  BOX_TRAY_DATA* pBoxTray;
  BOOL bVoice;
} SAVEREV_BACKUP;





struct _POKEMON_TRADE_WORK{
  NHTTP_RAP_WORK* pNHTTP;
  PENMOVE_WORK aPanWork;
  POKEMONTRADE_PARAM* pParentWork;
  POKEMONTRADE_DEMO_WORK* pPokemonTradeDemo;
  u8 FriendPokemonCol[2][BOX_POKESET_MAX + TEMOTI_POKEMAX];         ///< ����̃|�P����BOX�ɂ���|�P�����F
  GFL_BMPWIN* StatusWin[2];     ///< �X�e�[�^�X�\��
  GFL_BMPWIN* StatusWin1[2];     ///< �X�e�[�^�X�\��
  GFL_BMPWIN* StatusWin2[2];     ///< �X�e�[�^�X�\��
  GFL_BMPWIN* GTSInfoWindow;     ///< GTS�l�S�V�G�[�V��������
  GFL_BMPWIN* GTSInfoWindow2;     ///< GTS�l�S�V�G�[�V��������
  POKEMON_PARAM* recvPoke[2];  ///< �󂯎�����|�P�������i�[����ꏊ
  StateFunc* state;      ///< �n���h���̃v���O�������
  TIMEICON_WORK* pTimeIcon;
  HEAPID heapID;
  void* pTexBoard[5];///< �{�[�h�e�N�X�`���[
  SAVEREV_BACKUP aBackup;
  
  int BOX_TRAY_MAX;
  int TRADEBOX_LINEMAX;  ///< �{�b�N�X�̃}�b�N�X�����܂�Ȃ��̂ŏc�̗񂪕ϐ�
  int _SRCMAX;    ///< �{�b�N�X�̃}�b�N�X�����܂�Ȃ��̂ŃX�N���[���̐�
  int _DOTMAX;     ///< �{�b�N�X�̃}�b�N�X�����܂�Ȃ��̂őS���h�b�g
//  int _DOT_START_POS;  ///< �{�b�N�X�̃}�b�N�X�����܂�Ȃ��̂Ŏn�܂�ꏊ�̕ϐ�
//#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))

  GFL_BMPWIN* MessageWin;  ///< ���b�Z�[�W�n�\��

  GFL_ARCUTIL_TRANSINFO bgchar;

  BOOL padMode;

  APP_TASKMENU_WIN_WORK* pAppWin;
  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_RES* pAppTaskRes;
  GFL_BMPWIN* MyInfoWin;            // �X�e�[�^�X�\��
  GFL_BMPWIN* BoxNameWin[BOX_MAX_TRAY+1];            // �{�b�N�X���\��
  GFL_BMPWIN* mesWin;               // ��b�E�C���h�E�p
  GFL_MSGDATA *pMsgData;  //
  WORDSET *pWordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  GFL_FONT* pFontHandle;
  PRINT_STREAM* pStream;
  STRBUF* pStrBuf;
  STRBUF* pExStrBuf;
  STRBUF* pMessageStrBuf;
  STRBUF* pMessageStrBufEx;
  PRINT_QUE*            SysMsgQue;
  GFL_TCBLSYS *pMsgTcblSys;

  GFL_BMPWIN* SerchMojiWin[JAPANESE_SEARCH_INDEX_MAX];  //�����������̃E�C���h�E�p
  int selectMoji;                  //�����������őI�񂾕���

  GFL_G3D_RES* pG3dRes;
  GFL_G3D_OBJ* pG3dObj;
  GFL_G3D_OBJSTATUS*	pStatus;
  GFL_G3D_CAMERA		*pCamera;
  _EFFTOOL_PAL_FADE_WORK* pModelFade;
  _D2_PAL_FADE_WORK* pD2Fade;

  //  G3D_MDL_WORK			mdl[_G3D_MDL_MAX];
  //    BMPWINFRAME_AREAMANAGER_POS aPos;
  //3D
  int objCount;

  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex;


  BOX_MANAGER* pBox;
  GAMEDATA* pGameData;
//  MAIL_BLOCK* pMailBlock;
  MYSTATUS* pMy;
  MYSTATUS* pFriend;
  POKEPARTY* pMyParty;
  GFL_G3D_CAMERA    *camera;

  MCSS_SYS_WORK *mcssSys;
  MCSS_WORK     *pokeMcss[4];
  BOOL     mcssStop[2];
  _POKEMCSS_MOVE_WORK* pMoveMcss[4];

  GFL_ARCUTIL_TRANSINFO subchar;
  GFL_ARCUTIL_TRANSINFO subchar1;
  GFL_ARCUTIL_TRANSINFO subchar2;
  GFL_ARCUTIL_TRANSINFO subcharMain;
  u32 cellRes[CEL_RESOURCE_MAX];

  //	GAMESYS_WORK* pGameSys;
  _BALL_ICON_WORK aBallIcon[UI_BALL_NUM];
  _TYPE_ICON_WORK aTypeIcon[2];
  _ITEMMARK_ICON_WORK aItemMark;
  _ITEMMARK_ICON_WORK aPokerusMark;
  _ITEMMARK_ICON_WORK aPokeMark[_POKEMARK_MAX];

  TOUCHBAR_WORK* pTouchWork;

  GFL_CLUNIT	*cellUnit;
  GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��

//  u32 pokeIconNcgResOrg[BOX_POKESET_MAX+ TEMOTI_POKEMAX];

  u32 pokeIconNcgRes[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* pokeIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* markIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* searchIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconNo[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconForm[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconLine[_LING_LINENO_MAX][BOX_VERTICAL_NUM];

  NET_SAVE_WORK* pNetSave;
  GFL_CLWK* curIcon[CELL_DISP_NUM];
  GFL_CLWK* mojiIcon;

  int windowNum;
  int anmCount;
  int saveStep;
  BOOL bTouch;
  BOOL IsIrc;
  u32 connect_bit;
  BOOL connect_ok;
  BOOL receive_ok;
  u32 receive_result_param;
  u32 receive_first_param;

  int nowBoxno;  //���܂̃{�b�N�X
  u32 x;
  u32 y;
  BOOL bUpVec;

  GFL_CLWK* pSelectCLWK;   ///<�I��ł�|�P����CLACT   ������͉���I�����Ă��邩�Ɏg��
  GFL_CLWK* pCatchCLWK;   ///<����ł�|�P����CLACT  ������̓^�b�`���Ɉړ����邽�߂Ɏg��
  GFL_CLACTPOS aCatchOldPos;  //����ł�|�P�����̑O�̈ʒu
  GFL_CLACTPOS aDifferencePos;  //����ł�|�P�����̌덷�ʒu
  GFL_CLACTPOS aVecPos;  //�x�N�g�������ׂɂƂ��Ă����O�̈ʒu
  BOOL bStatusDisp;

  int MainObjCursorLine;   //OBJ�J�[�\�����C��
  int MainObjCursorIndex;  //OBJ�J�[�\���C���f�b�N�X

  int workPokeIndex;       // �}�E�X����łɂ������|�P����
  int workBoxno;           // �}�E�X����łɂ������|�P����
  int selectIndex;  //���̃|�P����Index   ��ɕ\�����Ă���|�P����
  int selectBoxno;  //���̃|�P����Box     ��ɕ\�����Ă���|�P����
  int importanceIndex;  //���̃|�P����Index   ��ɕ\�����Ă���|�P����
  int importanceBoxno;  //���̃|�P����Box     ��ɕ\�����Ă���|�P����

  int underSelectIndex;  //�܂�����Ɍ����ĂȂ��|�P����Index �X�e�[�^�X�\���̃|�P����
  int underSelectBoxno;  //�܂�����Ɍ����ĂȂ��|�P����Box   �X�e�[�^�X�\���̃|�P����
  int pokemonsetCall;
  int pokemonselectnoGTS;

  //GTS�p
  int GTSSelectIndex[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];  //���̃|�P����Index
  int GTSSelectBoxno[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];  //���̃|�P����Box

  GFL_CLWK* pokeSelectSixCur;
  POKEMON_PARAM* GTSSelectPP[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  GFL_CLWK* pokeIconGTS[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  u32 pokeIconNcgResGTS[GTS_PLAYER_WORK_NUM][GTS_NEGO_POKESLT_MAX];
  GFL_BMPWIN* TriStatusWin[ GTS_PLAYER_WORK_NUM*2 ];            // �{�b�N�X���\��
  int GTStype[GTS_PLAYER_WORK_NUM];  //
  int GTSlv[GTS_PLAYER_WORK_NUM];  //
  BOOL bGTSSelect[GTS_PLAYER_WORK_NUM];  //���肵�����ǂ���

  BOOL pokemonThreeSet;  //���葤�̃|�P�������R�̃Z�b�g���ꂽ
  
  POKEMON_PARAM* TempBuffer[GTS_PLAYER_WORK_NUM]; //�|�P������M�p�o�b�t�@

  BOOL bParent;
  BOOL bTouchReset;

  short xspeed;
  short speed;   ///< �X�N���[���������x
  short BoxScrollNum;   ///< �h�b�g�P�ʂňʒu���Ǘ�  8*20��BOX 8*12���Ă��� BOX_MAX_TRAY => 2880+96=2976
  short FriendBoxScrollNum;   ///< ��L�̑��葤�̒l

  short oldLine; //���݂̕`��Line �X�V�g���K

  BOOL bgscrollRenew;
  BOOL touchON;
  int bgscroll;
  int ringLineNo;
  int demoBGM;

  int modelno;  ///< �\�����Ă��郂�f���̔ԍ�
  int pokemonselectno;  ///< �����Ƒ���̃|�P�����I�𒆂̂ǂ�����w���Ă��邩

  GFL_CLWK* eruptedButtonGTS[GTS_ERUPTED_BUTTON_NUM * 2];  //�畬�o��
  GFL_CLWK* faceButtonGTS[GTS_FACE_BUTTON_NUM];  //��}�[�N�{�^��
  GFL_CLWK* select6Msg[GTS_SEL6MSG_NUM];  //����
  u32 select6CellRes[2]; //pal anm
  GFL_BMP_DATA* listBmp[GTS_SEL6MSG_NUM];
  u32 listRes[GTS_SEL6MSG_NUM];
  u16 palette3d[16];
  u16 palTrans[16];
  u16* scrTray;
  u16* scrTemoti;
  u8* pCharMem;
  GFL_CLACTPOS AddPos;
  POKEMONTRADE_TYPE type;
  PROGVAL_PEZIER_WORK aCutMullRom;
  APP_PRINTSYS_COMMON_WORK trgWork;  //�L�[�̐�����s�����[�N
  s16 SuckedCount;
  s16 countNEGOSlideScroll;
  u8 changeFactor[2];
  u8 SlideWindowTimer[2];
  u8 timerErupted[2]; //�當�������^�C�}�[
  u8 evilCheck[2];
  u8 pokemonGTSSeq[2];  //���b�Z�[�W�̕\���ؑ�
  u8 pokemonGTSStateMode;  //��ʓI��pokemonGTSSeq���󂯓���Ă悢�ꍇTRUE
  u8 BGClearFlg;
  u8 DemoBGClearFlg;
  u8 bByebyeMessageEach;  //�����ID�Ƒ��M�|�P������v
  u8 bEncountMessageEach; //�󂯎��ID�Ǝ�M�|�P������v
  u8 bByebyeNoJump;
  u8 bEncountNoJump;
  u8 NegoWaitTime;  //NEGO��x�点��^�C�}�[
  u8 friendBoxNum;  //�Ƃ������̃{�b�N�X�ԍ�
  u8 statusModeOn;  //
  u8 friendMailBoxFULL;  //�Ƃ������̓��[���{�b�N�X���t���ǂ���
  u8 mojiIconEnd;  //�����_�ŃA�C�R�����I��������ǂ���
  u8 zenkoku_flag;
  u8 dummy;
} ;


#define UNIT_NULL		(0xffff)

extern void IRC_POKMEONTRADE_ChangeFinish(POKEMON_TRADE_WORK* pWork);


extern void IRC_POKETRADE_GraphicInitMainDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicInitSubDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_MainGraphicExit(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SubGraphicExit(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_CommonCellInit(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_GraphicExit(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SubStatusInit(POKEMON_TRADE_WORK* pWork,int pokeposx, int type);
extern void IRC_POKETRADE_SubStatusEnd(POKEMON_TRADE_WORK* pWork);


extern BOOL POKEMONTRADEPROC_IsTriSelect(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADEPROC_IsNetworkMode(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_PROC_FadeoutStart(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_TrayDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitBoxIcon( BOX_MANAGER* boxData ,POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADE_AllDeletePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_G3dDraw(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork,int type);
extern void IRC_POKETRADE_ResetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicFreeVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetSubDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainVram(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_TouchStateGTS(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE3D_3DReelPaletteFade(POKEMON_TRADE_WORK *pWork);
extern void IRC_POKETRADE_3DGraphicSetUp( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE type);
extern void IRC_POKETRADE_CreatePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitBoxCursor(POKEMON_TRADE_WORK* pWork);
extern POKEMON_PARAM* IRC_POKEMONTRADE_GetRecvPP(POKEMON_TRADE_WORK *pWork, int index);
extern int POKETRADE_boxScrollNum2Line(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_MainObjCursorDisp(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADEDEMO_Init( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADEDEMO_Main( POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADEDEMO_End( POKEMON_TRADE_WORK* pWork );
//extern GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex);
extern GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex, int* outline, int* outindex, int* outRingLine);

extern void IRC_POKETRADEDEMO_SetModel( POKEMON_TRADE_WORK* pWork, int modelno);
extern void IRC_POKETRADEDEMO_RemoveModel( POKEMON_TRADE_WORK* pWork);
extern void POKE_MAIN_Pokemonset(POKEMON_TRADE_WORK *pWork, int side, POKEMON_PARAM* pp );
extern void POKETRE_MAIN_ChangePokemonSendDataNetwork(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_IRCDEMO_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_SAVE_Init(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_SAVE_TimingStart(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_EVOLUTION_TimingStart(POKEMON_TRADE_WORK* pWork);

extern void IRCPOKETRADE_PokeDeleteMcss( POKEMON_TRADE_WORK *pWork,int no  );
extern void IRCPOKETRADE_PokeCreateMcss( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev );
extern void IRCPOKETRADE_PokeCreateMcssGTS( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp, BOOL bRev,BOOL bGTS );
extern POKEMON_PASO_PARAM* IRCPOKEMONTRADE_GetPokeDataAddress(BOX_MANAGER* boxData , int trayNo, int index,POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_SetSubStatusIcon(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainStatusBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetMainStatusBG(POKEMON_TRADE_WORK* pWork);
extern void IRCPOKEMONTRADE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK* pWork, int side);
extern void IRC_POKETRADE_InitSubMojiBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_EndSubMojiBG(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SendVramBoxNameChar(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SendScreenBoxNameChar(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetBoxNameWindow(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ItemIconDisp(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp);

extern void IRC_POKETRADE_ItemIconReset(_ITEMMARK_ICON_WORK* pIM);

extern void IRC_POKETRADE_PokerusIconDisp(POKEMON_TRADE_WORK* pWork,int side,int bMain, POKEMON_PARAM* pp);
extern void IRC_POKETRADE_PokeStatusIconDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp, BOOL bEgg);
extern void IRC_POKETRADE_PokeStatusIconReset(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetCursorXY(POKEMON_TRADE_WORK* pWork);
extern BOOL POKETRADE_IsMainCursorDispIn(POKEMON_TRADE_WORK* pWork,int* line);
extern int POKETRADE_Line2RingLineIconGet(int line);
extern void POKETRADE_TOUCHBAR_Init(POKEMON_TRADE_WORK* pWork);

extern BOOL POKEMONTRADE_IsPokeLanguageMark(int monsno,int moji);
extern void IRC_POKETRADE_StatusWindowMessagePaletteTrans(POKEMON_TRADE_WORK* pWork, int palno, int palType);
extern void IRC_POKETRADE_GraphicInitSubDispStatusDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicEndSubDispStatusDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_EndIconResource(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_DEMOCLACT_Create(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_CLACT_Create(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE2D_IconGray(POKEMON_TRADE_WORK* pWork, GFL_CLWK* pCL ,BOOL bGray);
extern void IRC_POKETRADE_PosChangeSubStatusIcon(POKEMON_TRADE_WORK* pWork,int sel,BOOL bReset);
extern void POKEMONTRADE_VisibleFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo, BOOL bVisible);
extern void POKE_GTS_VisibleFaceIcon(POKEMON_TRADE_WORK* pWork,BOOL bVisible);
extern BOOL POKE_GTS_BanPokeCheck(POKEMON_TRADE_WORK* pWork, POKEMON_PASO_PARAM* ppp);

extern void POKEMON_TRADE_MaskCommon(POKEMON_TRADE_WORK* pWork);
extern BOOL POKE_GTS_IsMyIn(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowOpenXY(POKEMON_TRADE_WORK* pWork,BOOL bFast,int x,int y,int xm,int ym);
extern int POKEMONTRADE_GetFriendBoxNum(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_NEGOBG_Select6Create(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_NEGOBG_Select6Scroll(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_NEGOBG_SlideInit(POKEMON_TRADE_WORK* pWork,int side, POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGOBG_SlideMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGOBG_SlideMessageDel(POKEMON_TRADE_WORK *pWork,int side);
extern void POKEMONTRADE_NEGO_SlideInit(POKEMON_TRADE_WORK* pWork,int side,POKEMON_PARAM* pp);
extern void POKEMONTRADE_NEGO_SlideMain(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_2D_GTSPokemonIconVisible(POKEMON_TRADE_WORK* pWork,int side, BOOL onoff);
extern void POKEMONTRADE_changeTimingDemoStart(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsEggAndLastBattlePokemonChange(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowTimeIconStart(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_DEMOBGMChange(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKMEONTRADE_changeState(POKEMON_TRADE_WORK* pWork,StateFunc* state);
extern BOOL POKEMONTRADE_IsMailBoxFull(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsMailPokemon(POKEMON_TRADE_WORK* pWork);


#if _TRADE_DEBUG

extern void IRC_POKMEONTRADE_changeStateDebug(POKEMON_TRADE_WORK* pWork,StateFunc* state, int line);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeStateDebug(pWork ,state, __LINE__)

#else  //_NET_DEBUG


#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeState(pWork ,state)

#endif //_NET_DEBUG


#define _TEMOTITRAY_SCR_MAX (12)
#define _BOXTRAY_SCR_MAX (20)
//#define _SRCMAX ((BOX_MAX_TRAY*_BOXTRAY_SCR_MAX)+_TEMOTITRAY_SCR_MAX)

#define _TEMOTITRAY_MAX (8*_TEMOTITRAY_SCR_MAX)
#define _BOXTRAY_MAX (8*_BOXTRAY_SCR_MAX)

//#define _DOTMAX ((BOX_MAX_TRAY*_BOXTRAY_MAX)+_TEMOTITRAY_MAX)

//#define _DOT_START_POS (_DOTMAX - 80)


extern int IRC_TRADE_LINE2TRAY(int lineno,POKEMON_TRADE_WORK* pWork);
extern int IRC_TRADE_LINE2POKEINDEX(int lineno,int verticalindex);


// �ʏ�̃t�H���g�J���[
#define	FBMP_COL_NULL		(0)
#define	FBMP_COL_BLACK		(1)
#define	FBMP_COL_BLK_SDW	(2)
#define	FBMP_COL_RED		(3)
#define	FBMP_COL_RED_SDW	(4)
#define	FBMP_COL_GREEN		(7)
#define	FBMP_COL_GRN_SDW	(8)
#define	FBMP_COL_BLUE		(5)
#define	FBMP_COL_BLUE_SDW	(6)
#define	FBMP_COL_PINK		(9)
#define	FBMP_COL_PNK_SDW	(10)
#define	FBMP_COL_WHITE_SDW		(2)
#define	FBMP_COL_WHITE		(15)



#define _CLACT_SOFTPRI_POKESEL  (11)
#define _CLACT_SOFTPRI_POKESEL_BAR  (12)
#define _CLACT_SOFTPRI_CATCHPOKE  (1)
#define _CLACT_SOFTPRI_SCROLL_BAR  (14)
#define _CLACT_SOFTPRI_SELECT (15)
#define _CLACT_SOFTPRI_POKELIST  (16)
#define _CLACT_SOFTPRI_MARK (19)

// �����������
#define _TIMER_ERUPTED_NUM (60)


///�ʐM�R�}���h
typedef enum {
  _NETCMD_CHANGEFACTOR = GFL_NET_CMD_IRCTRADE,
  _NETCMD_SELECT_POKEMON,
  _NETCMD_CHANGE_POKEMON,
  _NETCMD_POKEMONSELECT_GTS,
  _NETCMD_LOOKATPOKE,
  _NETCMD_CHANGE_CANCEL,
  _NETCMD_END,
  _NETCMD_CANCEL_POKEMON,
  _NETCMD_THREE_SELECT1,
  _NETCMD_THREE_SELECT2,
  _NETCMD_THREE_SELECT3,
  _NETCMD_THREE_SELECT_END,
  _NETCMD_THREE_SELECT_CANCEL,
  _NETCMD_SCROLLBAR,
  _NETCMD_FACEICON,
  _NETCMD_POKEMONCOLOR,
  _NETCMD_GTSSEQNO,
  _NETCMD_FRIENDBOXNUM,
  _NETCMD_EVILCHECK,
  _NETCMD_UN,
} _POKEMON_TRADE_SENDCMD;


extern BOOL POKE_GTS_PokemonsetAndSendData(POKEMON_TRADE_WORK* pWork,int index,int boxno);
extern void POKE_GTS_CreatePokeIconResource(POKEMON_TRADE_WORK* pWork,int mainsub);
extern void POKE_GTS_StatusMessageDisp(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_SelectStatusMessageDisp(POKEMON_TRADE_WORK* pWork, int side, BOOL bSelected);
extern void POKE_GTS_SelectStatusMessageDelete(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_EndWork(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_InitWork(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_Select6Init(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_Select6MessageInit(POKEMON_TRADE_WORK* pWork);
extern int POKE_GTS_IsSelect(POKEMON_TRADE_WORK* pWork,int boxno,int index);
extern BOOL POKEMONTRADEPROC_IsTimeWaitSelect(POKEMON_TRADE_WORK* pWork);




//���b�Z�[�W�֘A
extern void POKETRADE_MESSAGE_WindowOpen(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowOpenCustom(POKEMON_TRADE_WORK* pWork,BOOL bFast, BOOL bUnderPos);
extern void POKETRADE_MESSAGE_WindowClose(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_WindowClear(POKEMON_TRADE_WORK* pWork);
extern BOOL POKETRADE_MESSAGE_EndCheck(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_HeapInit(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_HeapEnd(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_AppMenuOpenCustom(POKEMON_TRADE_WORK* pWork, int *menustr,int num, int x,int y);
extern void POKETRADE_MESSAGE_AppMenuOpen(POKEMON_TRADE_WORK* pWork, int *menustr,int num);
extern void POKETRADE_MESSAGE_AppMenuClose(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_ChangePokemonMyStDisp(POKEMON_TRADE_WORK* pWork,int pageno,int leftright,POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_ChangePokemonStatusDisp(POKEMON_TRADE_WORK* pWork,POKEMON_PARAM* pp, int mcssno, int change);
extern void POKETRADE_MESSAGE_SetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork, int side,POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_ResetPokemonMyStDisp(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_ResetPokemonStatusMessage(POKEMON_TRADE_WORK *pWork);
extern void POKETRADE_MESSAGE_CreatePokemonParamDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp);
extern void POKETRADE_MESSAGE_SixStateDisp(POKEMON_TRADE_WORK* pWork,int frame);
extern void POKETRADE_MESSAGE_ChangeStreamType(POKEMON_TRADE_WORK* pWork,int type);

//�|�P�����Q�c
extern void POKETRADE_2D_GTSPokemonIconSet(POKEMON_TRADE_WORK* pWork, int side,int no, POKEMON_PARAM* pp, int hilow);
extern void POKETRADE_2D_GTSPokemonIconReset(POKEMON_TRADE_WORK* pWork,int side, int no);


extern void POKEMONTRADE_StartFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);
extern void POKEMONTRADE_RemoveFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);


//�|�P�����v���Z�X ir union wifi�p
extern void POKETRADE_PROC_PokemonStatusStart(POKEMON_TRADE_WORK* pWork);
extern void POKE_TRADE_PROC_TouchStateCommon(POKEMON_TRADE_WORK* pWork);

//�|�P�����l�S�V�G�[�V�����p
extern void POKETRADE_NEGO_Select6keywait(POKEMON_TRADE_WORK* pWork);
extern int POKETRADE_NEGO_IsSelect(POKEMON_TRADE_WORK* pWork,int line , int height);
extern BOOL POKETRADE_NEGO_IsStatusLookAt(POKEMON_TRADE_WORK* pWork,int line , int height);

extern void POKETRADE_TOUCHBAR_Init(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_RemoveCoreResource(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_ReleasePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE_MESSAGE_SixStateDelete(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DirectAddPokemon(POKEMON_TRADE_WORK* pWork,int index,const POKEMON_PARAM* pp);
extern void POKE_GTS_DeletePokemonDirect(POKEMON_TRADE_WORK* pWork,int side,int index);
extern void POKETRADE_2D_GTSPokemonIconResetAll(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DeletePokemonState(POKEMON_TRADE_WORK* pWork);

extern void POKETRADE_2D_ObjTrayDispInit(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_StartCatched(POKEMON_TRADE_WORK* pWork,int line, int pos,int x,int y,POKEMON_PASO_PARAM* ppp);
extern void POKEMONTRADE_StartSucked(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_SuckedMain(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_RemovePokemonCursor(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_2D_AlphaSet(POKEMON_TRADE_WORK* pWork);
extern void POKETRADE2D_IconAllGray(POKEMON_TRADE_WORK* pWork,BOOL bGray);


extern void POKE_GTS_InitFaceIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_DeleteFaceIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_FaceIconFunc(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_TouchFaceButtonGTS(POKEMON_TRADE_WORK* pWork, int faceNo);
extern void POKEMONTRADE_StartEruptedGTS(POKEMON_TRADE_WORK* pWork, int faceNo, int index);
extern void POKEMONTRADE_RemoveEruptedGTS(POKEMON_TRADE_WORK* pWork,int index);

extern void POKE_GTS_DeleteEruptedIcon(POKEMON_TRADE_WORK* pWork);
extern void POKE_GTS_InitEruptedIcon(POKEMON_TRADE_WORK* pWork,int faceNo, int index);

extern void POKE_GTS_InitEruptedIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ReleaseMainObj(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitMainObj(POKEMON_TRADE_WORK* pWork);
extern void PokemonTrade_SetMyPokeColor( POKEMON_TRADE_WORK* pWork  );
extern void IRC_POKETRADE3D_SetColorTex( POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_StartPokeSelectSixButton(POKEMON_TRADE_WORK* pWork,int index);
extern void POKEMONTRADE_RemovePokeSelectSixButton(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsInPokemonRecvPoke(POKEMON_PARAM* pp);
extern void POKEMONTRADE_CreatePokeSelectMessage(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_RemovePokeSelectMessage(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_MessageOAMWriteVram(POKEMON_TRADE_WORK* pWork);


extern void POKMEONTRADE_DEMO_GTSUP_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTSDOWN_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTSMID_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKMEONTRADE_DEMO_GTS_ChangeDemo(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE_StartMojiSelect(POKEMON_TRADE_WORK* pWork,int x,int y);
extern void POKEMONTRADE_EndMojiSelect(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_CheckMojiSelect(POKEMON_TRADE_WORK* pWork);
extern void POKEMONTRADE2D_ChangePokemonPalette(POKEMON_TRADE_WORK* pWork, BOOL bGray);
extern void POKEMONTRADE_McssAnmStop( u32 data, fx32 currentFrame );



//�{�^���쐬
extern APP_TASKMENU_WIN_WORK* POKEMONTRADE_MESSAGE_CancelButtonStart(POKEMON_TRADE_WORK* pWork,int msgno);
extern void POKEMONTRADE_MESSAGE_CancelButtonDelete(POKEMON_TRADE_WORK* pWork,int bar,int change);
extern BOOL POKEMONTRADE_MESSAGE_ButtonCheck(POKEMON_TRADE_WORK* pWork);

extern void POKEMONTRADE_pokeMoveFunc(_POKEMCSS_MOVE_WORK* pMove);
extern _POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeTblMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, VecFx32* pTbl, HEAPID heapID);
extern _POKEMCSS_MOVE_WORK* POKEMONTRADE_pokeMoveCreate(MCSS_WORK* pokeMcss, int time, const VecFx32* pPos, HEAPID heapID);
extern void POKEMONTRADE_pokeMoveRenew(_POKEMCSS_MOVE_WORK* pPoke,int time, const VecFx32* pPos);

