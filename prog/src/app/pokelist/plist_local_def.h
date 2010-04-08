//======================================================================
/**
 * @file  plist_local_def.h
 * @brief �|�P�������X�g ���[�J����`
 * @author  ariizumi
 * @data  09/06/10
 */
//======================================================================
#pragma once

#include "app/pokelist.h"
#include "print/gf_font.h"
#include "print/printsys.h"
#include "sound/pm_sndsys.h"
#include "debug/debugwin_sys.h"
#include "app/app_taskmenu.h"

#define PLIST_LIST_MAX (6)

//Vram�]���^�X�N
#define PLIST_VTRANS_TASK_NUM (3)

//BG�ʒ�`(�A���t�@�ݒ�̂Ƃ���͒�`���Ⴄ�̂ŕς�����ύX���邱��
#define PLIST_BG_MAIN_BG (GFL_BG_FRAME3_M)
#define PLIST_BG_PLATE   (GFL_BG_FRAME2_M)
#define PLIST_BG_PARAM   (GFL_BG_FRAME1_M)
#define PLIST_BG_MENU    (GFL_BG_FRAME0_M)
#define PLIST_BG_3D      (GFL_BG_FRAME0_M)

#define PLIST_BG_SUB_BG (GFL_BG_FRAME3_S)
//�o�g����p
#define PLIST_BG_SUB_BATTLE_WIN (GFL_BG_FRAME2_S)
#define PLIST_BG_SUB_BATTLE_BAR (GFL_BG_FRAME1_S)
#define PLIST_BG_SUB_BATTLE_STR (GFL_BG_FRAME0_S)

//�y��ƃp�����[�^�̃X�N���[����(�L�����P��
#define PLIST_BG_SCROLL_X_CHAR (16)

//BG�p���b�g
#define PLIST_BG_PLT_MENU_ACTIVE (0x01)
#define PLIST_BG_PLT_MENU_NORMAL (0x02)
#define PLIST_BG_PLT_BAR (0x0b)
#define PLIST_BG_PLT_BMPWIN (0x0c)
#define PLIST_BG_PLT_HP_BAR (0x0d)
#define PLIST_BG_PLT_FONT (0x0e)

#define PLIST_BG_SUB_PLT_BACK (0x00)  //1�{
#define PLIST_BG_SUB_PLT_PLATE (0x01) //1�{
#define PLIST_BG_SUB_PLT_BAR (0x02) //1�{
#define PLIST_BG_SUB_PLT_HP_BAR (0x0d)  //1�{ �����p
#define PLIST_BG_SUB_PLT_FONT (0x0e)      //1�{


//BG�L����(BmpWin
#define PLIST_BG_WINCHAR_TOP (1)

//�����F
#define PLIST_FONT_MSG_BACK (0xF)
#define PLIST_FONT_MSG_LETTER (0x1)
#define PLIST_FONT_MSG_SHADOW (0x2)
#define PLIST_FONT_COLOR_BLACK (PRINTSYS_LSB_Make(1,2,0))

//�v���[�g�̕�����HP�o�[�Ɠ����p���b�g
#define PLIST_FONT_PARAM_LETTER (0x1)
#define PLIST_FONT_PARAM_SHADOW (0x2)
#define PLIST_FONT_PARAM_LETTER_BLUE (0x5)
#define PLIST_FONT_PARAM_SHADOW_BLUE (0x6)
#define PLIST_FONT_PARAM_LETTER_RED (0x3)
#define PLIST_FONT_PARAM_SHADOW_RED (0x4)

//���j���[�̕����̓t�H���g�p���b�g�ł͂Ȃ����j���[�v���[�g�̃p���b�g���g��
#define PLIST_FONT_MENU_BACK (0x0)
#define PLIST_FONT_MENU_LETTER (0xe)
#define PLIST_FONT_MENU_WAZA_LETTER (0xd)
#define PLIST_FONT_MENU_SHADOW (0xf)

//�o�g���p�̃A�C�R���͕��ʂ̃t�H���g
#define PLIST_FONT_BATTLE_PARAM PRINTSYS_LSB_Make( 0xF , 2 , 0 )
#define PLIST_FONT_BATTLE_BLUE PRINTSYS_LSB_Make( 5 , 6 , 0 )
#define PLIST_FONT_BATTLE_RED  PRINTSYS_LSB_Make( 3 , 0xE , 0 )

//�����\���ʒu
#define PLIST_MSG_STR_OFS_X (1)
#define PLIST_MSG_STR_OFS_Y (1)

//HP�F
#define PLIST_HPBAR_COL_GREEN_IN  (0xa)
#define PLIST_HPBAR_COL_GREEN_OUT (0xb)
#define PLIST_HPBAR_COL_YELLOW_IN  (0xc)
#define PLIST_HPBAR_COL_YELLOW_OUT (0xd)
#define PLIST_HPBAR_COL_RED_IN  (0xe)
#define PLIST_HPBAR_COL_RED_OUT (0xf)


//�v���[�g�̃A�j������{��
#define PLIST_MENU_ANIME_NUM (3)


//OBJ���\�[�XIdx
typedef enum
{
  PCR_PLT_OBJ_COMMON,
  PCR_PLT_BAR_ICON,
  PCR_PLT_ITEM_ICON,
  PCR_PLT_ITEM_ICON_SUB,
  PCR_PLT_CONDITION,
  PCR_PLT_HP_BASE,
  PCR_PLT_POKEICON,
  PCR_PLT_POKEICON_SUB,
  
  PCR_NCG_CURSOR,
  PCR_NCG_BALL,
  PCR_NCG_BAR_ICON,
  PCR_NCG_ITEM_ICON,
  PCR_NCG_ITEM_ICON_SUB,
  PCR_NCG_CONDITION,
  PCR_NCG_HP_BASE,
  //�|�P�A�C�R����NCG�͊e�ӏ��Ŏ���
  
  PCR_ANM_CURSOR,
  PCR_ANM_BALL,
  PCR_ANM_BAR_ICON,
  PCR_ANM_ITEM_ICON,
  PCR_ANM_ITEM_ICON_SUB,
  PCR_ANM_CONDITION,
  PCR_ANM_HP_BASE,
  PCR_ANM_POKEICON,
  
  PCR_MAX,
  
  PCR_PLT_START = PCR_PLT_OBJ_COMMON,
  PCR_NCG_START = PCR_NCG_CURSOR,
  PCR_ANM_START = PCR_ANM_CURSOR,
  
  PCR_NONE = 0xFFFFFFFF,  //��
  
}PLIST_CEL_RESOURCE;

//CELL_UNIT�̕\����
#define PLIST_CELLUNIT_PRI_MAIN (8)
#define PLIST_CELLUNIT_PRI_PLATE (0)

//OBJ�p���b�g�̊J�n�ʒu
#define PLIST_OBJPLT_COMMON (0)     //3�{
#define PLIST_OBJPLT_ITEM_ICON (3)  //1�{
#define PLIST_OBJPLT_CONDITION (4)  //1�{
#define PLIST_OBJPLT_HP_BASE   (5)  //1�{
#define PLIST_OBJPLT_POKEICON  (6)  //3�{
#define PLIST_OBJPLT_BAR_ICON  (9)  //3�{

//�o�[�̃A�C�R���̎��
enum PLIST_BARICON_TYPE
{
  PBT_RETURN, //�߂�(���
  PBT_EXIT,   //������(�~�}�[�N
  
  PBT_MAX,
};
//�o�[�̃A�C�R���̃A�j��
enum PLIST_BARICON_ANIME
{
  PBA_RETURN_NORMAL,  //�߂�(�ʏ�
  PBA_EXIT_NORMAL,    //������(�ʏ�
};

//���C���ƂȂ�V�[�P���X(work->mainSeq
typedef enum
{
  PSMS_FADEIN,
  PSMS_FADEIN_WAIT,
  PSMS_SELECT_POKE, //�|�P�����I��
  PSMS_CHANGE_POKE,
  PSMS_USE_POKE,    //�g�p�ΏۑI��
  PSMS_CHANGE_ANM,
  PSMS_MENU,        //���j���[������
  PSMS_MSG_WAIT,    //���b�Z�[�W���X
  PSMS_MSG_SE,      //���b�Z�[�W���SE����
  PSMS_YESNO_WAIT,    //�͂��E�������҂�
  PSMS_INIT_HPANIME,
  PSMS_HPANIME,     //HP�o�[������
  PSMS_BATTLE_ANM_WAIT, //�o�g�����j���[���I���A�j���҂�
  PSMS_DISP_PARAM,      //���x���A�b�v���E�p�����[�^�\��

  PSMS_FORM_CHANGE_INIT,
  PSMS_FORM_CHANGE_MAIN,
  PSMS_FORM_CHANGE_TERM,

  PSMS_FINISH_SYNC_INIT,  //Wifi�o�g���I���ʐM����
  PSMS_FINISH_SYNC_WAIT,
  
  PSMS_FADEOUT,
  PSMS_FADEOUT_FORCE, //�����I��
  PSMS_FADEOUT_WAIT,

  PSMS_MAX,
}PLIST_SYS_MAIN_SEQ;

//Proc�؂�ւ��p�V�[�P���X(work->changeProcSeq
typedef enum
{
  PSCS_INIT,
  PSCS_MAIN,
  PSCS_TERM,
  
  PSCS_MAX,
}PLIST_SYS_CHANGEPROC_SEQ;

//DEMO�p
#define PLIST_DEMO_EMMITER_MAX (3)
#define PLIST_DEMO_SCALE (38)
typedef enum
{
  PDT_NONE,
  PDT_GIRATHINA_TO_ORIGIN,
  PDT_GIRATHINA_TO_ANOTHER,
  PDT_SHEIMI_TO_SKY,
}PLIST_DEMO_TYPE;

typedef struct _PLIST_WORK PLIST_WORK;
typedef struct _PLIST_PLATE_WORK PLIST_PLATE_WORK;
typedef struct _PLIST_MSG_WORK   PLIST_MSG_WORK;
typedef struct _PLIST_MENU_WORK  PLIST_MENU_WORK;
typedef struct _PLIST_BATTLE_PARAM_WORK PLIST_BATTLE_PARAM_WORK;
typedef struct _PLIST_DEBUG_WORK PLIST_DEBUG_WORK;

typedef void (*PLIST_CallbackFunc)(PLIST_WORK *work);
typedef void (*PLIST_CallbackFuncYesNo)(PLIST_WORK *work,const int retVal);

struct _PLIST_WORK
{
  HEAPID heapId;
  GFL_TCB *vBlankTcb;
  
  int ktst;     //KeyToushState
  u8  mainSeq;
  u8  nextMainSeq;
  u8  subSeq;
  u8  selectState;
  BOOL isActiveWindowMask;  //���j���[�o���Ƃ���WndMask��ON/OFF��VBlank�؂�ւ��悤
  BOOL canExit;   //X�{�^�����������H
  BOOL isSetWazaMode;     //�Z�p��MODE_SELECT���H
  BOOL isSetMusicalMode;  //�~���[�W�J���p��MODE_SELECT���H
  BOOL isFinishSync;      //�I�����ɒʐM�������Ƃ�(WifiBattle
  BOOL reqPlaySe;         //�t�F�[�h���SE��炷����(�S�̕����A�C�e��
  
  PL_SELECT_POS pokeCursor;
  PL_SELECT_POS changeTarget;
  PL_SELECT_POS useTarget;
  POKEMON_PARAM *selectPokePara;
  int  menuRet; //PLIST_MENU_ITEM_TYPE
  
  //�I�����t���b�V��+�g�_��
  u16 platePalAnm[PLIST_MENU_ANIME_NUM][16];
  u16 platePalTrans[PLIST_MENU_ANIME_NUM][16];
  u8  platePalAnmCnt;
  u16 platePalWakuAnmCnt;

  //����ւ��A�j���p
  u8  anmCnt;

  //�o�g�����j���[�p
  u8  btlJoinNum;   //�o�g���Q���l��
//  GFL_BMPWIN *btlMenuWin[2];  //�o�g���p����E�߂�
  APP_TASKMENU_WIN_WORK *btlMenuWin[2]; //�o�g���p����E�߂�
  u16 btlMenuAnmCnt;       //���̃A�j���p
  u16 btlMenuTransBuf;
  u8  btlTermAnmCnt;
  BOOL isDecideParty;

  //HP�A�j������
  u16 befHp;  //�񕜑O��HP
  s8  anmSpd; //1�t���[���̑���
  PLIST_CallbackFunc hpAnimeCallBack;
  
  //LvUp�pbmpWin
  u16          befParam[6]; //���x���A�b�v�O�̐��l
  GFL_BMPWIN   *paramWin;

  
  //MSG�n
  GFL_MSGDATA *msgHandle;
  GFL_FONT *fontHandle;
  GFL_FONT *sysFontHandle;
  PRINT_QUE *printQue;
  BOOL isMsgWaitKey;    //���b�Z�[�W�̊����̃L�[�҂������邩�H
  PLIST_CallbackFunc msgCallBack;  //���b�Z�[�W�I�����̃R�[���o�b�N
  PLIST_CallbackFuncYesNo yesNoCallBack;  //�͂��E�������R�[���o�b�N
  
  //��ʉ��E�B���h�E�Ǘ�
  PLIST_MSG_WORK  *msgWork;

  //���j���[�Ǘ�
  PLIST_MENU_WORK *menuWork;
  
  //�v���[�gScr
  void  *plateScrRes;
  NNSG2dScreenData *plateScrData;

  //�v���[�g�f�[�^
  PLIST_PLATE_WORK  *plateWork[PLIST_LIST_MAX];
  
  //Cell�n
  u32 cellRes[PCR_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkCursor[2];
  GFL_CLWK    *clwkBarIcon[PBT_MAX];
  GFL_CLWK    *clwkExitButton;    //�I�����A�j���҂�������Ƃ��Ƀ|�C���^�����Ă���

  //�o�g�����ʗp
  GFL_CLUNIT  *cellUnitBattle;
  PLIST_BATTLE_PARAM_WORK *battleParty[PL_COMM_PLAYER_TYPE_MAX];
  GFL_BMPWIN   *timeLimitStr;
  GFL_BMPWIN   *timeLimitNumStr;
  BOOL  isUpdateLimitStr;
  BOOL  isUpdateLimitNum;
  u8    befSelectedNum;
  u8    befTimeLimit;
  u16   barPalletAnm[16];
  u16   barPalletTrans[16];
  u16   barPalletAnmCnt;
  BOOL  isCallForceExit;

	//taskmenu���\�[�X
	APP_TASKMENU_RES *taskres;
	
	//3D�p(�t�H�����`�F���W���o�p
  GFL_G3D_CAMERA    *camera;
	void*         effTempWork;
	GFL_PTC_PTR		ptcWork;
	PLIST_DEMO_TYPE demoType;
	u8   demoCnt;
	u8   demoChangeTimming;
	BOOL demoIsChange;

  //�ʐM�p
  BOOL isComm;
  u8   commState;

  PLIST_DATA *plData;
#if USE_DEBUGWIN_SYSTEM
  PLIST_DEBUG_WORK *debWork;
#endif
};

