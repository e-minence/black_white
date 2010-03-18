//======================================================================
/**
 * @file    itemmenu.h
 * @brief   �A�C�e�����j���[
 * @author  k.ohno
 * @date    2009.06.30
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
#include "system/palanm.h"
#include "system/blink_palanm.h"

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
#include "app/app_keycursor.h"  // APP_KEYCURSOR_WORK

// SE�Ē�`
enum
{ 
  SE_BAG_DECIDE       = SEQ_SE_DECIDE1, ///< ���艹
  SE_BAG_CANCEL       = SEQ_SE_CANCEL1, ///< �L�����Z����
  SE_BAG_CLOSE        = SEQ_SE_CLOSE1,	///< �L�����Z�����i�w�{�^���j
  SE_BAG_REGIST_Y     = SEQ_SE_SYS_07,  ///< �o�b�OY�{�^���o�^��
  SE_BAG_SELL         = SEQ_SE_SYS_22,  ///< ���p��
  SE_BAG_TRASH        = SEQ_SE_SYS_08,  ///< �̂Ă鉹
  SE_BAG_CURSOR_MOVE  = SEQ_SE_SELECT1, ///< �o�b�O�J�[�\���ړ�(�㉺�L�[)
  SE_BAG_POCKET_MOVE  = SEQ_SE_SELECT4, ///< �o�b�O�|�P�b�g�I��(���E�L�[)
  SE_BAG_SRIDE        = SEQ_SE_SELECT1, ///< �X���C�h�o�[
  SE_BAG_WAZA         = SEQ_SE_PC_LOGIN,  ///< ���U�}�V���N����
  SE_BAG_SORT         = SEQ_SE_SYS_36,    ///< �\�[�g��
  SE_BAG_SPRAY        = SEQ_SE_DECIDE1,   ///< �X�v���[�g�p��
  SE_BAG_RAITOSUTOON  = SEQ_SE_DECIDE2,   ///< ���C�g�X�g�[���E�_�[�N�X�g�[��
};

#define ITEMMENU_SCROLLBAR_ENABLE_NUM (7) // �X�N���[���o�[���L���ɂȂ�A�C�e����

#define ITEM_LIST_NUM (8)
#define _CELLUNIT_NUM (30)

#define _SCROLL_TOP_Y  (8+8)  //�X�N���[���o�[�̍ŏ��̈ʒu
#define _SCROLL_BOTTOM_Y  (8*19)  //�X�N���[���o�[�̍Ō�̈ʒu

#define _SUBMENU_LISTMAX (8)

// ���l����
enum
{ 
  BAG_UI_TP_CUR_REPEAT_SYNC = 30,          ///< ���̃t���[���^�b�`��������Ƃ���ȍ~���t���[���^�b�`����
  BAG_UI_TP_CUR_HIGHSPEED_SYNC = 60 * 2,   ///< �^�b�`���Ă��鎞�Ԃ����̃t���[���𒴂���ƈړ���UP
};

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
  _PLT_SORT,
  _NCG_SORT,
  _PLT_LIST,
  // _NCG_LIST >> listRes�Ń��X�g�̍��ڕ��p�ӂ���Ă���
  _ANM_SORT,
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

// �o�[�A�C�R����CLWK�z��C���f�b�N�X
enum
{ 
  BAR_ICON_LEFT = 0,
  BAR_ICON_RIGHT,
  BAR_ICON_CHECK_BOX,
  BAR_ICON_EXIT,
  BAR_ICON_RETURN,
  BAR_ICON_INPUT_U, ///< ���l���͏�
  BAR_ICON_INPUT_D, ///< ���l���͉�
  BAR_ICON_MAX,

  BAR_ICON_ANM_EXIT     = 0,
  BAR_ICON_ANM_RETURN   = 1,
  BAR_ICON_ANM_LEFT     = 4,
  BAR_ICON_ANM_RIGHT    = 5,
  BAR_ICON_ANM_CHECKBOX = 6,
  BAR_ICON_ANM_EXIT_OFF   = 14,
  BAR_ICON_ANM_LEFT_OFF   = 18,
  BAR_ICON_ANM_RIGHT_OFF  = 19,
  BAR_ICON_ANM_CHECKBOX_OFF  = 20,
  BAR_ICON_ANM_MAX,
};

//--------------------------------------------------------------
/// ���l���̓��[�h
//==============================================================
typedef enum
{ 
  BAG_INPUT_MODE_NULL = 0,
  BAG_INPUT_MODE_TRASH,   ///< ���Ă郂�[�h
  BAG_INPUT_MODE_SELL,    ///< ���郂�[�h�i���z�\������j
} BAG_INPUT_MODE;


typedef struct _FIELD_ITEMMENU_PARAM FIELD_ITEMMENU_WORK;
typedef void (StateFunc)(FIELD_ITEMMENU_WORK* wk);
typedef int (MenuFunc)(FIELD_ITEMMENU_WORK* wk);

struct _FIELD_ITEMMENU_PARAM {
  // [IN] ���������ɊO������󂯓n����郁���o
  GAMEDATA            * gamedata;       ///< Y�{�^���o�^�Ɏg�p
  CONFIG              * config;              
  MYSTATUS            * mystatus;
  ITEMCHECK_WORK      * icwk;           ///< �A�C�e���`�F�b�N���[�N FMENU_EVENT_WORK����R�s�[
  BAG_MODE            mode;             ///< �o�b�O�Ăяo�����[�h
  BAG_CURSOR*         pBagCursor;
  MYITEM_PTR          pMyItem;
  BOOL                cycle_flg;        ///< ���]�Ԃ��ǂ����H
//  GMEVENT             * event;
//  FIELDMAP_WORK       * fieldmap;

  // [PRIVATE]
  ITEM_ST ScrollItem[ BAG_MYITEM_MAX ]; ///< �X�N���[������ׂ̃A�C�e�����X�g
  StateFunc * state;                    ///< �n���h���̃v���O�������
  GFL_BMPWIN* win;
  GFL_BMPWIN* itemInfoDispWin;
  GFL_BMPWIN* pocketNameWin;
  PRINT_UTIL  SysMsgPrintUtil;      ///< �V�X�e���E�C���h�EPrintUtil
  PRINT_QUE*  SysMsgQue;
  PRINT_STREAM* pStream;
  APP_KEYCURSOR_WORK *MsgCursorWork;///< ���b�Z�[�W�L�[�҂��J�[�\���\�����[�N
  GFL_MSGDATA* MsgManager;          ///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA* MsgManagerItemInfo;  ///< ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA* MsgManagerPocket;
  GFL_MSGDATA* MsgManagerItemName;
  WORDSET*    WordSet;              ///< ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF*     pStrBuf;
  STRBUF*     pExpStrBuf;
  STRBUF*     pItemNameBuf;
  GFL_FONT*   fontHandle;
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
  GFL_CLWK  *clwkScroll;
  GFL_CLWK  *clwkPocketIcon;
  GFL_CLWK  *clwkWazaKind;
  GFL_CLWK  *clwkWazaType;
  GFL_CLWK  *clwkBarIcon[ BAR_ICON_MAX ];
  GFL_CLWK  *clwkSort;
  GFL_TCBLSYS *pMsgTcblSys;
  TOUCH_SW_SYS* pTouchSWSys;
  GFL_BMPWIN* winWaza;
  GFL_BMPWIN* winItemName;
  GFL_BMPWIN* winItemNum;
  GFL_BMPWIN* winItemReport;
  GFL_BMPWIN* menuWin[5];
  GFL_BMPWIN* winNumFrame;
  GFL_BMPWIN* winGoldCap;
  GFL_BMPWIN* winGold;
  GFL_BMPWIN* winSellGold;
  GFL_BMPWIN* winPocketNone; // �|�P�b�g����̎��̃��b�Z�[�W�\��

  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_RES* pAppTaskRes;
  
  //���j���[�y��
  NNSG2dCharacterData *ncgData;
  void *ncgRes; 

  BAG_INPUT_MODE InputMode;
  int countTouch; ///< �^�b�`�J�E���^
  int InputNum;
  int mainbg;
  int subbg;
  int subbg2;
  int barbg;
  int numFrameBg;
  int pocketno;  //�������Ă���|�P�b�g�ԍ�
  int curpos;   //�������Ă���J�[�\���ԍ�
  int oamlistpos; //OAMLIST �� �擪�ʒu -1����J�n
  int moveMode;  //�ړ����[�h�ɂȂ�
  int menuNum;          //�T�u���j���[�̍��ڐ�
  int subListCursor;  //�T�u���j���[�̃J�[�\���ʒu
  int submenuList[BAG_MENUTBL_MAX];  //�T�u���j���[�̍���
  u32 bgchar;
  BOOL bChange;   ///< CELL�X�V�t���O
  BOOL bDispUpReq; ///< ����DISP�ؑ�(VBLANK���ɔ���A����)

  enum BAG_NEXTPROC_ENUM ret_code;  //�o�b�O���j���[���I���ۂ̎��̓���
  int ret_code2;
  int ret_item;       ///< �I�񂾃A�C�e��
  
  BOOL sort_mode;     ///< �\�[�g���[�h


	BLINKPALANM_WORK * blwk;
	PALETTE_FADE_PTR	pfd;
	BOOL	active;		///< �A�N�e�B�u/�p�b�V�u
	u16	tmpSeq;			///< �ėp�V�[�P���X
	u16	tmpCnt;			///< �ėp�J�E���^
	StateFunc * chgState;
};

#define _OBJPLT_SUB_POKE_TYPE (4) //�T�u��ʋZ�^�C�v�A�C�R���p���b�g�ʒu


#define _DISP_INITX (18)
#define _DISP_INITY (1)
#define _DISP_SIZEX (12)
#define _DISP_SIZEY (20)
#define _SUBLIST_NORMAL_PAL   (9)   //�T�u���j���[�̒ʏ�p���b�g
#define _SUBLIST_SELECT_PAL   (10)  //�T�u���j���[�̑I���p���b�g
#define _SUBLIST_CANCEL_PAL   (11)  //�T�u���j���[�̃L�����Z���p���b�g
#define _BUTTON_MSG_PAL   (12)  // ���b�Z�[�W�t�H���g
#define _SUBBUTTON_MSG_PAL   (0)  // ���b�Z�[�W�t�H���g
#define _BUTTON_WIN_PAL   (13)  // �E�C���h�E

#define _PAL_WIN01_CELL					(0)     // ���X�g�E�B���h�E�p�p���b�g�]���ʒu
#define _PAL_WIN01_CELL_NUM			(3)     // ���X�g�E�B���h�E�p�p���b�g�]���ʒu
#define _PAL_CUR_CELL						(_PAL_WIN01_CELL+_PAL_WIN01_CELL_NUM)	// �X���C�h�o�[�܂� �J�[�\���p�p���b�g�]���ʒu
#define _PAL_CUR_CELL_NUM				(1)
#define _PAL_SORT_CELL					(_PAL_CUR_CELL+_PAL_CUR_CELL_NUM)			// �\�[�g�{�^���̃p���b�g�W�J�ʒu
#define _PAL_SORT_CELL_NUM			(2)																		// �\�[�g�{�^���̃p���b�g�{��
#define _PAL_BAG_PARTS_CELL			(_PAL_SORT_CELL+_PAL_SORT_CELL_NUM)		// �o�b�O�̃p�[�cCELL�̃p���b�g�W�J�ʒu
#define _PAL_BAG_PARTS_CELL_NUM	(1)						// �o�b�O�̃p�[�cCELL�̃p���b�g�W�J�ʒu
#define _PAL_COMMON_CELL				(_PAL_BAG_PARTS_CELL+_PAL_BAG_PARTS_CELL_NUM)	// ���ʃp���b�g�W�J�ʒu
#define _PAL_COMMON_CELL_NUM		(APP_COMMON_BARICON_PLT_NUM)
#define _PAL_MENU_CHECKBOX_CELL			(_PAL_COMMON_CELL+_PAL_COMMON_CELL_NUM)	// �`�F�b�N�{�b�N�X�̃p���b�g�W�J�ʒu
#define _PAL_MENU_CHECKBOX_CELL_NUM	(1)  // �`�F�b�N�{�b�N�X�̃p���b�g�W�J�ʒu

#define	_PAL_FADE_OBJ_BIT				( 0x044f )	// �p���b�g�t�F�[�h��K�p����OBJ�r�b�g [ 0000_0100_0100_1111 ]



// �|�P�b�g���̕����F
#define _POCKETNAME_FONT_PAL_L (0xF)
#define _POCKETNAME_FONT_PAL_S (0x2)
#define _POCKETNAME_FONT_PAL_B (0x0)


#define FBMP_COL_WHITE    (15)
#define WINCLR_COL(col) (((col)<<4)|(col))


#define _ITEMUSE_DISP_INITX (17)
#define _ITEMUSE_DISP_INITY (10)
#define _ITEMUSE_DISP_SIZEX (8)
#define _ITEMUSE_DISP_SIZEY (10)

// �u�����Â����v
#define _GOLD_CAP_DISP_INITX (1)
#define _GOLD_CAP_DISP_INITY (21)
#define _GOLD_CAP_DISP_SIZEX (8)
#define _GOLD_CAP_DISP_SIZEY (3)

// �����Â��� XXXXXX�~
#define _GOLD_DISP_INITX (9)
#define _GOLD_DISP_INITY (21)
#define _GOLD_DISP_SIZEX (9)
#define _GOLD_DISP_SIZEY (3)

// ���l���̓E�B���h�E ���p���z
#define _SELL_GOLD_DISP_INITX (17)
#define _SELL_GOLD_DISP_INITY (15)
#define _SELL_GOLD_DISP_SIZEX (11)
#define _SELL_GOLD_DISP_SIZEY (2)

#define _POCKETNAME_DISP_INITX (3)
#define _POCKETNAME_DISP_INITY (21)
#define _POCKETNAME_DISP_SIZEX (12)
#define _POCKETNAME_DISP_SIZEY (3)

//�������͂̃E�C���h�E�̈ʒu
#define _WINNUM_INITX (17)
#define _WINNUM_INITY (13)
#define _WINNUM_SIZEX (11)
#define _WINNUM_SIZEY (2)
#define _WINNUM_PAL (3)

//�u�Ȃɂ�����܂���v�E�C���h�E�̈ʒu
#define _WIN_POCKETNONE_INITX (18)
#define _WIN_POCKETNONE_INITY (9)
#define _WIN_POCKETNONE_SIZEX (12)
#define _WIN_POCKETNONE_SIZEY (3)
#define _WIN_POCKETNONE_POSX (0)  // PRINT���̃h�b�g���炵�w�� X
#define _WIN_POCKETNONE_POSY (4)  // PRINT���̃h�b�g���炵�w�� Y
#define _WIN_POCKETNONE_PAL (3)

#define _WINNUM_SCR_INITX (16)
#define _WINNUM_SCR_INITY (12)
#define _WINNUM_SCR_TP_END_X (29)
#define _WINNUM_SCR_TP_END_Y (22)

#define _BAR_CELL_CURSOR_EXIT (200-8)    //EXIT x�{�^��
#define _BAR_CELL_CURSOR_RETURN (232-8) //RETURN Enter�{�^��

enum
{ 
  BUTTONID_ITEM_AREA_NUM = 6,   ///< �A�C�e���G���A�̍��ڐ�
  BUTTONID_CHECK_AREA_NUM = 6,  ///< �`�F�b�N�G���A�̍��ڐ�
  // 0-4 : �o�b�O�̃|�P�b�g
  BUTTONID_LEFT = 5,
  BUTTONID_RIGHT,
  BUTTONID_SORT,
  BUTTONID_CHECKBOX,
  BUTTONID_EXIT,
  BUTTONID_RETURN,
  BUTTONID_ITEM_AREA,
  BUTTONID_CHECK_AREA = BUTTONID_ITEM_AREA + BUTTONID_ITEM_AREA_NUM,
  BUTTONID_MAX = BUTTONID_CHECK_AREA + BUTTONID_CHECK_AREA_NUM,
};

#define FLD_SUBSCR_FADE_DIV (1)
#define FLD_SUBSCR_FADE_SYNC (1)


extern void _createSubBg(void);
extern void ITEMDISP_graphicInit(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageSetDispVBlank( FIELD_ITEMMENU_WORK* pWork, BOOL on_off );
extern void ITEMDISP_upMessageRewrite(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageClean(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageDelete(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_upMessageCreate(FIELD_ITEMMENU_WORK* pWork);
//extern void ITEMDISP_RemoveSubDispItemIcon(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_graphicDelete(FIELD_ITEMMENU_WORK* pWork);
extern void _dispMain(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_CellResourceCreate( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellCreate( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellMessagePrint( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_CellVramTrans( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_ScrollCursorMove(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_ScrollCursorChangePos(FIELD_ITEMMENU_WORK* pWork, int num);

extern s32 ITEMMENU_SellPrice( int item_no, int input_num, HEAPID heapID );
extern int ITEMMENU_GetItemIndex(FIELD_ITEMMENU_WORK* pWork);
extern int ITEMMENU_GetItemPocketNumber(FIELD_ITEMMENU_WORK* pWork);
extern ITEM_ST* ITEMMENU_GetItem(FIELD_ITEMMENU_WORK* pWork,int no);
extern void ITEMMENU_WordsetItemName( FIELD_ITEMMENU_WORK* pWork, u32 bufID, u32 itemID );

extern void ITEMDISP_InitPocketCell( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_ChangePocketCell( FIELD_ITEMMENU_WORK* pWork,int pocketno );
extern void ITEMDISP_MenuWinDisp(  FIELD_ITEMMENU_WORK *work , int *menustr,int num );
extern void ITEMDISP_ListPlateClear( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_ListPlateSelectChange( FIELD_ITEMMENU_WORK* pWork , int selectNo);
extern void ITEMDISP_ItemInfoWindowChange(FIELD_ITEMMENU_WORK *pWork,int pocketno  );
extern void ITEMDISP_WazaInfoWindowChange( FIELD_ITEMMENU_WORK *pWork );

extern void ITEMDISP_ItemInfoWindowDispEx( FIELD_ITEMMENU_WORK *pWork, BOOL is_stream );
extern void ITEMDISP_ItemInfoWindowDisp( FIELD_ITEMMENU_WORK *pWork );
extern void ITEMDISP_ItemInfoMessageMake( FIELD_ITEMMENU_WORK *pWork,int id );
extern BOOL ITEMDISP_MessageEndCheck(FIELD_ITEMMENU_WORK* pWork);

extern int ITEMMENU_GetPosCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern BOOL ITEMMENU_AddCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern void ITEMMENU_RemoveCnvButtonItem(FIELD_ITEMMENU_WORK* pWork, int no);
extern void ITEMDISP_SetVisible(void);
extern void ITEMDISP_BarMessageCreate( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_BarMessageDelete( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_PocketMessage(FIELD_ITEMMENU_WORK* pWork,int newpocket);
extern void ITEMDISP_GoldDispWrite( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_GoldDispIn( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_GoldDispOut( FIELD_ITEMMENU_WORK* pWork );
extern void ITEMDISP_YesNoStart(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_YesNoExit(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_NumFrameDisp(FIELD_ITEMMENU_WORK* pWork);
extern void ITEMDISP_InputNumDisp(FIELD_ITEMMENU_WORK* pWork,int num);


extern SHORTCUT_ID ITEMDISP_GetPocketShortcut( int pocketno );
extern void ITEMDISP_ChangeCursorPosPalette( FIELD_ITEMMENU_WORK * wk, u32 pal );
extern void ITEMDISP_ChangeActive( FIELD_ITEMMENU_WORK * wk, BOOL flg );
extern void ITEMDISP_ChangeRetButtonActive( FIELD_ITEMMENU_WORK * wk, BOOL flg );
