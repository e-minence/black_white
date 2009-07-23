

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

#include "message.naix"
#include "msg/msg_d_field.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_nftr
#include "sound/pm_sndsys.h"
#include "itemmenu.h"
#include "app/itemuse.h"

extern const GFL_PROC_DATA ItemMenuProcData;


#define ITEM_LIST_NUM (8)
#define _CELLUNIT_NUM (12)

#define _SCROLL_TOP_Y  (8)  //�X�N���[���o�[�̍ŏ��̈ʒu
#define _SCROLL_BOTTOM_Y  (8*20)  //�X�N���[���o�[�̍Ō�̈ʒu

enum _ITEMLISTCELL_RESOURCE
{
  _PLT_CUR,
  _PLT_BAR,
  _NCG_CUR,
  _ANM_CUR,
  _ANM_LIST,
  SCR_MAX ,
};


typedef struct _DEBUGITEM_PARAM FIELD_ITEMMENU_WORK;
typedef void (StateFunc)(FIELD_ITEMMENU_WORK* wk);
typedef int (MenuFunc)(FIELD_ITEMMENU_WORK* wk);

struct _DEBUGITEM_PARAM {
	ITEM_ST ScrollItem[ BAG_MYITEM_MAX ];	// �X�N���[������ׂ̃A�C�e�����X�g
  StateFunc* state;      ///< �n���h���̃v���O�������
  GMEVENT * event;
  GAMESYS_WORK * gsys;
  FIELD_MAIN_WORK * fieldmap;
  SAVE_CONTROL_WORK *ctrl;
  MYITEM_PTR pMyItem;
  MYSTATUS* mystatus;
	GFL_BMPWIN* win;
  GFL_BMPWIN* itemUseWin;
  PRINT_UTIL            SysMsgPrintUtil;    // �V�X�e���E�C���h�EPrintUtil
  PRINT_QUE*            SysMsgQue;
  GFL_MSGDATA *MsgManager;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA *MsgManagerItemInfo;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  WORDSET			*WordSet;								// ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF*  pStrBuf;
	STRBUF*  pExpStrBuf;
  BMP_MENULIST_DATA* submenulist;
  BMP_MENULIST_DATA* itemUseMenuList;
  GFL_FONT 			*fontHandle;
  BMPMENULIST_WORK* sublw;
  BMPMENULIST_WORK* lwItemUse;
  BAG_CURSOR* pBagCursor;
  MenuFunc* menu_func[BAG_MENUTBL_MAX];
  ITEMCHECK_WORK icwk;
	u32 objRes[3];  //CLACT���\�[�X
  u32 cellRes[SCR_MAX];  //�A�C�e���J�[�\��

  u32 listRes[ITEM_LIST_NUM];  //�A�C�e�����X�g
  GFL_BMP_DATA* listBmp[ITEM_LIST_NUM];
  GFL_CLWK  *listCell[ITEM_LIST_NUM];
  u32 bListEnable[ITEM_LIST_NUM];
  
	GFL_BUTTON_MAN* pButton;
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��
  GFL_CLUNIT *cellUnit;
  GFL_CLWK  *cellicon;
  GFL_CLWK  *clwkCur;
  GFL_CLWK  *scrollCur;

  GFL_BMPWIN* winItemName;
  GFL_BMPWIN* winItemNum;
  GFL_BMPWIN* winItemReport;

  int mainbg;
  int subbg;
  int subbg2;
  int barbg;
  
	int pocketno;  //�������Ă���|�P�b�g�ԍ�
	int curpos;   //�������Ă���J�[�\���ԍ�
  int oamlistpos; //OAMLIST �� �擪�ʒu -1����J�n
  int moveMode;  //�ړ����[�h�ɂȂ�
  
  int mode;
  int ret_item;
  int cycle_flg;
  u32 NowAttr;
  u32 FrontAttr;
	HEAPID heapID;
	u32 bgchar;
  BOOL bChange;
};

#define DEBUG_ITEMDISP_FRAME (GFL_BG_FRAME1_M)


#define _DISP_INITX (18)
#define _DISP_INITY (1)
#define _DISP_SIZEX (12)
#define _DISP_SIZEY (20)
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (11)  // ���b�Z�[�W�t�H���g
#define	FBMP_COL_WHITE		(15)
#define WINCLR_COL(col)	(((col)<<4)|(col))


#define _ITEMUSE_DISP_INITX (17)
#define _ITEMUSE_DISP_INITY (10)
#define _ITEMUSE_DISP_SIZEX (8)
#define _ITEMUSE_DISP_SIZEY (10)



#define FLD_SUBSCR_FADE_DIV (1)
#define FLD_SUBSCR_FADE_SYNC (1)

extern void _createSubBg(void);
extern void _graphicInit(FIELD_ITEMMENU_WORK* pWork);
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

