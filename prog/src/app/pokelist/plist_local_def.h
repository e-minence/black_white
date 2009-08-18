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

#define PLIST_LIST_MAX (6)

//Vram�]���^�X�N
#define PLIST_VTRANS_TASK_NUM (3)

//BG�ʒ�`(�A���t�@�ݒ�̂Ƃ���͒�`���Ⴄ�̂ŕς�����ύX���邱��
#define PLIST_BG_MAIN_BG (GFL_BG_FRAME3_M)
#define PLIST_BG_PLATE   (GFL_BG_FRAME2_M)
#define PLIST_BG_PARAM   (GFL_BG_FRAME1_M)
#define PLIST_BG_MENU    (GFL_BG_FRAME0_M)

#define PLIST_BG_SUB_BG (GFL_BG_FRAME3_S)

//�y��ƃp�����[�^�̃X�N���[����(�L�����P��
#define PLIST_BG_SCROLL_X_CHAR (16)

//BG�p���b�g
#define PLIST_BG_PLT_MENU_ACTIVE (0x01)
#define PLIST_BG_PLT_MENU_NORMAL (0x02)
#define PLIST_BG_PLT_BMPWIN (0x0c)
#define PLIST_BG_PLT_HP_BAR (0x0d)
#define PLIST_BG_PLT_FONT (0x0e)


//BG�L����(BmpWin
#define PLIST_BG_WINCHAR_TOP (1)

//�����F
#define PLIST_FONT_MSG_BACK (0xF)
#define PLIST_FONT_MSG_LETTER (0x1)
#define PLIST_FONT_MSG_SHADOW (0x2)
#define PLIST_FONT_COLOR_BLACK (PRINTSYS_LSB_Make(1,2,0))

//�v���[�g�̕�����HP�o�[�Ɠ����p���b�g
#define PLIST_FONT_PARAM_LETTER (0xF)
#define PLIST_FONT_PARAM_SHADOW (0x1)

//���j���[�̕����̓t�H���g�p���b�g�ł͂Ȃ����j���[�v���[�g�̃p���b�g���g��
#define PLIST_FONT_MENU_BACK (0x0)
#define PLIST_FONT_MENU_LETTER (0xe)
#define PLIST_FONT_MENU_WAZA_LETTER (0xd)
#define PLIST_FONT_MENU_SHADOW (0xf)

//�����\���ʒu
#define PLIST_MSG_STR_OFS_X (1)
#define PLIST_MSG_STR_OFS_Y (1)

//HP�F
#define PLIST_HPBAR_COL_GREEN_IN  (0x5)
#define PLIST_HPBAR_COL_GREEN_OUT (0x6)
#define PLIST_HPBAR_COL_YELLOW_IN  (0x7)
#define PLIST_HPBAR_COL_YELLOW_OUT (0x8)
#define PLIST_HPBAR_COL_RED_IN  (0x9)
#define PLIST_HPBAR_COL_RED_OUT (0xa)


//�v���[�g�̃A�j������F
#define PLIST_MENU_ANIME_COL (0x7)


//OBJ���\�[�XIdx
typedef enum
{
  PCR_PLT_OBJ_COMMON,
  PCR_PLT_BAR_ICON,
  PCR_PLT_ITEM_ICON,
  PCR_PLT_CONDITION,
  PCR_PLT_HP_BASE,
  PCR_PLT_POKEICON,
  
  PCR_NCG_CURSOR,
  PCR_NCG_BALL,
  PCR_NCG_BAR_ICON,
  PCR_NCG_ITEM_ICON,
  PCR_NCG_CONDITION,
  PCR_NCG_HP_BASE,
  //�|�P�A�C�R����NCG�͊e�v���[�g�Ŏ���
  
  PCR_ANM_CURSOR,
  PCR_ANM_BALL,
  PCR_ANM_BAR_ICON,
  PCR_ANM_ITEM_ICON,
  PCR_ANM_CONDITION,
  PCR_ANM_HP_BASE,
  PCR_ANM_POKEICON,
  
  PCR_MAX,
  
  PCR_PLT_START = PCR_PLT_OBJ_COMMON,
  PCR_PLT_END = PCR_PLT_POKEICON,
  PCR_NCG_START = PCR_NCG_CURSOR,
  PCR_NCG_END = PCR_NCG_HP_BASE,
  PCR_ANM_START = PCR_ANM_CURSOR,
  PCR_ANM_END = PCR_ANM_POKEICON,
  
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
  PSMS_CHANGE_ANM,
  PSMS_MENU,        //���j���[������
  PSMS_MSG_WAIT,    //���b�Z�[�W���X
  PSMS_YESNO_WAIT,    //�͂��E�������҂�
  PSMS_INIT_HPANIME,
  PSMS_HPANIME,     //HP�o�[������
  PSMS_FADEOUT,
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

typedef struct _PLIST_WORK PLIST_WORK;
typedef struct _PLIST_PLATE_WORK PLIST_PLATE_WORK;
typedef struct _PLIST_MSG_WORK   PLIST_MSG_WORK;
typedef struct _PLIST_MENU_WORK  PLIST_MENU_WORK;
typedef struct _PLIST_DEBUG_WORK PLIST_DEBUG_WORK;

typedef void (*PSTATUS_CallbackFunc)(PLIST_WORK *work);
typedef void (*PSTATUS_CallbackFuncYesNo)(PLIST_WORK *work,const int retVal);

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
  
  PL_SELECT_POS pokeCursor;
  PL_SELECT_POS changeTarget;
  POKEMON_PARAM *selectPokePara;
  int  menuRet; //PLIST_MENU_ITEM_TYPE
  
  u16 platePalAnm[16];
  u16 platePalTrans[16];
  u8  platePalAnmCnt;

  //����ւ��A�j���p
  u8  anmCnt;

  //�o�g�����j���[�p
  u8  btlJoinNum;   //�o�g���Q���l��
  GFL_BMPWIN *btlMenuWin[2];  //�o�g���p����E�߂�
  u16 btlMenuAnm;       //���̃A�j���p
  u16 btlMenuTransBuf;

  //HP�A�j������
  u16 befHp;  //�񕜑O��HP
  PSTATUS_CallbackFunc hpAnimeCallBack;
  
  //MSG�n
  GFL_MSGDATA *msgHandle;
  GFL_FONT *fontHandle;
  GFL_FONT *sysFontHandle;
  PRINT_QUE *printQue;
  BOOL isMsgWaitKey;    //���b�Z�[�W�̊����̃L�[�҂������邩�H
  PSTATUS_CallbackFunc msgCallBack;  //���b�Z�[�W�I�����̃R�[���o�b�N
  PSTATUS_CallbackFuncYesNo yesNoCallBack;  //�͂��E�������R�[���o�b�N
  
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

  PLIST_DATA *plData;
#if USE_DEBUGWIN_SYSTEM
  PLIST_DEBUG_WORK *debWork;
#endif
};

