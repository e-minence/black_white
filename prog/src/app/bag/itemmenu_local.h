

//======================================================================
/**
 * @file	  itemmenu.h
 * @brief	  �A�C�e�����j���[
 * @author	k.ohno
 * @date	  2009.06.30
 */
//======================================================================

#pragma once

#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/touch_subwindow.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"
#include "itemmenu.h"
#include "app/itemuse.h"
#include "app/app_taskmenu.h"

extern const GFL_PROC_DATA ItemMenuProcData;


#define ITEM_LIST_NUM (8)
#define _CELLUNIT_NUM (30)

#define _SCROLL_TOP_Y  (8+8)  //�X�N���[���o�[�̍ŏ��̈ʒu
#define _SCROLL_BOTTOM_Y  (8*19)  //�X�N���[���o�[�̍Ō�̈ʒu

#define _SUBMENU_LISTMAX (8)

enum _ITEMLISTCELL_RESOURCE
{
  _PLT_CUR,
  _PLT_BAR,
  _PLT_COMMON,
  _NCG_CUR,
  _NCG_COMMON,
  _ANM_CUR,
  _ANM_LIST,
  _ANM_COMMON,
  _PTL_CHECK,
  _NCG_CHECK,
  _ANM_CHECK,
  _PLT_BAGPOCKET,  
  _NCG_BAGPOCKET,  
  _ANM_BAGPOCKET,  
  SCR_MAX ,
};


//�Z�A�C�R���̃��\�[�X
enum WAZAICON_CELL
{
  SCR_PLT_SUB_POKE_TYPE,
  SCR_ANM_SUB_POKE_TYPE,
  SCR_NCG_SKILL_TYPE_HENKA,
  SCR_NCG_SKILL_TYPE_BUTURI,
  SCR_NCG_SKILL_TYPE_TOKUSHU,
};


typedef struct _DEBUGITEM_PARAM FIELD_ITEMMENU_WORK;
typedef void (StateFunc)(FIELD_ITEMMENU_WORK* wk);
typedef int (MenuFunc)(FIELD_ITEMMENU_WORK* wk);

struct _DEBUGITEM_PARAM {
  // [IN] ���������ɊO������󂯓n����郁���o
  FIELDMAP_WORK       * fieldmap;
  GAMESYS_WORK        * gsys;
  SAVE_CONTROL_WORK   * ctrl;
  MYSTATUS            * mystatus;
  ITEMCHECK_WORK      icwk;             ///< �A�C�e���`�F�b�N���[�N FMENU_EVENT_WORK����R�s�[
  BAG_MODE            mode;             ///< �o�b�O�Ăяo�����[�h
  BOOL                cycle_flg;        ///< ���]�Ԃ��ǂ����H
//  GMEVENT * event;

  // [PRIVATE]
	ITEM_ST ScrollItem[ BAG_MYITEM_MAX ];	///< �X�N���[������ׂ̃A�C�e�����X�g
  StateFunc * state;                    ///< �n���h���̃v���O�������
  MYITEM_PTR  pMyItem;
	GFL_BMPWIN* win;
  GFL_BMPWIN* itemInfoDispWin;
  GFL_BMPWIN* pocketNameWin;
  PRINT_UTIL  SysMsgPrintUtil;      ///< �V�X�e���E�C���h�EPrintUtil
  PRINT_QUE*  SysMsgQue;
  GFL_MSGDATA* MsgManager;		      ///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA* MsgManagerItemInfo;	///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA* MsgManagerPocket;
  WORDSET*    WordSet;							///< ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF*     pStrBuf;
	STRBUF*     pExpStrBuf;
  GFL_FONT*   fontHandle;
  BAG_CURSOR* pBagCursor;
  MenuFunc*   menu_func[BAG_MENUTBL_MAX];
	HEAPID      heapID;

	u32 objRes[3];          ///< CLACT���\�[�X
  u32 cellRes[SCR_MAX];   ///< �A�C�e���J�[�\��

  u32 commonCellTypeNcg[POKETYPE_MAX];
  u32 commonCell[5];

  u32 listRes[ITEM_LIST_NUM];  //�A�C�e�����X�g
  GFL_BMP_DATA* listBmp[ITEM_LIST_NUM];
  GFL_CLWK  *listCell[ITEM_LIST_NUM];     //�A�C�e���̈ꗗOBJ
  u32 nListEnable[ITEM_LIST_NUM];
  GFL_CLWK  *listMarkCell[ITEM_LIST_NUM];  //�`�F�b�N�}�[�N
  
	GFL_BUTTON_MAN* pButton;
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
  GFL_CLUNIT *cellUnit;
  GFL_CLWK  *cellicon;
  GFL_CLWK  *clwkCur;
  GFL_CLWK  *scrollCur;
  GFL_CLWK  *clwkPocketIcon;
  GFL_CLWK  *clwkWazaKind;
  GFL_CLWK  *clwkWazaType;
  GFL_CLWK  *clwkBarIcon[5];
	GFL_TCBLSYS *pMsgTcblSys;
  PRINT_STREAM* pStream;
  TOUCH_SW_SYS* pTouchSWSys;
  GFL_BMPWIN* winWaza;
  GFL_BMPWIN* winItemName;
  GFL_BMPWIN* winItemNum;
  GFL_BMPWIN* winItemReport;
  GFL_BMPWIN* menuWin[5];
  GFL_BMPWIN* winNumFrame;

  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
	APP_TASKMENU_RES* pAppTaskRes;
  
  //���j���[�y��
  NNSG2dCharacterData *ncgData;
  void *ncgRes; 

  int mainbg;
  int subbg;
  int subbg2;
  int barbg;
  int numFrameBg;
  int trashNum;
	int pocketno;  //�������Ă���|�P�b�g�ԍ�
	int curpos;   //�������Ă���J�[�\���ԍ�
  int oamlistpos; //OAMLIST �� �擪�ʒu -1����J�n
  int moveMode;  //�ړ����[�h�ɂȂ�
  int menuNum;          //�T�u���j���[�̍��ڐ�
  int subListCursor;  //�T�u���j���[�̃J�[�\���ʒu
  int submenuList[BAG_MENUTBL_MAX];  //�T�u���j���[�̍���
  u32 NowAttr;
  u32 FrontAttr;
	u32 bgchar;
  BOOL bChange;
  int count;

  int ret_code;  //�o�b�O���j���[���I���ۂ̎��̓���
  int ret_code2;
  int ret_item;  //�I�񂾃A�C�e��
  
};

#define DEBUG_ITEMDISP_FRAME (GFL_BG_FRAME1_M)


#define _OBJPLT_SUB_POKE_TYPE (4) //�T�u��ʋZ�^�C�v�A�C�R���p���b�g�ʒu


#define _DISP_INITX (18)
#define _DISP_INITY (1)
#define _DISP_SIZEX (12)
#define _DISP_SIZEY (20)
#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g
#define _SUBLIST_SELECT_PAL   (10)  //�T�u���j���[�̑I���p���b�g
#define _SUBLIST_CANCEL_PAL   (11)  //�T�u���j���[�̃L�����Z���p���b�g
#define _BUTTON_MSG_PAL   (12)  // ���b�Z�[�W�t�H���g
#define _BUTTON_WIN_PAL   (13)  // �E�C���h�E

#define _PAL_BAG_PARTS_CELL (10)  // �o�b�O�̃p�[�cCELL�̃p���b�g�W�J�ʒu
#define _PAL_MENU_CHECKBOX_CELL (12)  // �`�F�b�N�{�b�N�X�̃p���b�g�W�J�ʒu

#define	FBMP_COL_WHITE		(15)
#define WINCLR_COL(col)	(((col)<<4)|(col))


#define _ITEMUSE_DISP_INITX (17)
#define _ITEMUSE_DISP_INITY (10)
#define _ITEMUSE_DISP_SIZEX (8)
#define _ITEMUSE_DISP_SIZEY (10)


#define _POCKETNAME_DISP_INITX (4)
#define _POCKETNAME_DISP_INITY (21)
#define _POCKETNAME_DISP_SIZEX (12)
#define _POCKETNAME_DISP_SIZEY (3)

//�������͂̃E�C���h�E�̈ʒu
#define _WINNUM_INITX (17)
#define _WINNUM_INITY (13)
#define _WINNUM_SIZEX (11)
#define _WINNUM_SIZEY (2)
#define _WINNUM_PAL (1)

#define _WINNUM_SCR_INITX (16)
#define _WINNUM_SCR_INITY (12)



#define BUTTONID_LEFT    (5)
#define BUTTONID_RIGHT   (6)
#define BUTTONID_EXIT   (7)
#define BUTTONID_RETURN   (8)
#define BUTTONID_ITEM_AREA (9)
#define BUTTONID_CHECK_AREA (15)


#define FLD_SUBSCR_FADE_DIV (1)
#define FLD_SUBSCR_FADE_SYNC (1)

extern void _createSubBg(void);
extern void ITEMDISP_graphicInit(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageRewrite(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageDelete(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageCreate(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_graphicDelete(FIELD_ITEMMENU_WORK* pWork);
extern void _dispMain(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_CellResourceCreate( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellCreate( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellMessagePrint( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellVramTrans( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_scrollCursorMove(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_scrollCursorChangePos(FIELD_ITEMMENU_WORK* pWork, int num);


extern int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork);
extern int ITEMMENU_GetItemPocketNumber(FIELD_ITEMMENU_WORK* pWork);
extern ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork,int no);

extern void ITEMDISP_InitPocketCell( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_ChangePocketCell( FIELD_ITEMMENU_WORK* pWork,int pocketno );
extern void ITEMDISP_MenuWinDisp(  FIELD_ITEMMENU_WORK *work , int *menustr,int num );
extern void ITEMDISP_ListPlateClear( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_ListPlateSelectChange( FIELD_ITEMMENU_WORK* pWork , int selectNo);
extern void ITEMDISP_ItemInfoWindowChange(FIELD_ITEMMENU_WORK *pWork,int pocketno  );
extern void ITEMDISP_WazaInfoWindowChange( FIELD_ITEMMENU_WORK *pWork );

extern void ITEMDISP_ItemInfoWindowDisp( FIELD_ITEMMENU_WORK *pWork );
extern void ITEMDISP_ItemInfoMessageMake( FIELD_ITEMMENU_WORK *pWork,int id );
extern BOOL ITEMDISP_MessageEndCheck(FIELD_ITEMMENU_WORK* pWork);

extern int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern void ITEMDISP_SetVisible(void);
extern void ITEMDISP_NumFrameDisp(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_TrashNumDisp(FIELD_ITEMMENU_WORK* pWork,int num);
extern void ITEMDISP_YesNoStart(FIELD_ITEMMENU_WORK* pWork);


