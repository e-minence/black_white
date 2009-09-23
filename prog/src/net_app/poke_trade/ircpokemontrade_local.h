//=============================================================================
/**
 * @file	  ircpokemontrade_local.h
 * @bfief	  �|�P�������� ���[�J�����L��`
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	  09/08/28
 */
//=============================================================================

#pragma once

#include "../../field/event_ircbattle.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "system/touch_subwindow.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "app/app_taskmenu.h"


#define _TIMING_ENDNO (12)

#define BOX_VERTICAL_NUM (5)
#define BOX_HORIZONTAL_NUM (6)

#define HAND_VERTICAL_NUM (3)
#define HAND_HORIZONTAL_NUM (2)

//#define BOX_MONS_NUM (30)


#define _BRIGHTNESS_SYNC (2)  // �t�F�[�h�̂r�x�m�b�͗v����
#define CUR_NUM (3)
#define _BUTTON_WIN_PAL   (15)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (14)  // ���b�Z�[�W�t�H���g
#define	FBMP_COL_WHITE		(15)

#define _OBJPLT_BOX  (0)  //3�{
#define _OBJPLT_BAR  (3)  //3�{
#define _OBJPLT_POKEICON  (6)  //3�{
#define PLIST_RENDER_MAIN (1)
#define PSTATUS_MCSS_POS_X1 (FX32_CONST(( 60 )/16.0f))
#define PSTATUS_MCSS_POS_X2 (FX32_CONST(( 190 )/16.0f))
#define PSTATUS_MCSS_POS_Y (FX32_CONST((192.0f-( 140 ))/16.0f))

#define YESNO_CHARA_OFFSET	(21 * 20 )
#define YESNO_CHARA_W		( 8 )
#define YESNO_CHARA_H		( 4 )

#define _BUTTON_WIN_WIDTH (22)    // �E�C���h�E��
#define _BUTTON_WIN_HEIGHT (5)    // �E�C���h�E����


#define CONTROL_PANEL_Y (120+48)


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


typedef void (StateFunc)(IRC_POKEMON_TRADE* pState);


typedef enum
{
	CHAR_BOX,
	PLT_POKEICON,
	PLT_BOX,
	ANM_POKEICON,
	ANM_BOX,
  CHAR_SCROLLBAR,
  PAL_SCROLLBAR,
  ANM_SCROLLBAR,

  CEL_RESOURCE_MAX,
} CEL_RESOURCE;

// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_BALL,
  SETUP_INDEX_MAX
};


struct _IRC_POKEMON_TRADE {
	POKEMON_PASO_PARAM* recvPoke[2];
	BOOL bPokemonSet[2];
	EVENT_IRCBATTLE_WORK* pParentWork;
	StateFunc* state;      ///< �n���h���̃v���O�������
	int selectType;   // �ڑ��^�C�v
	HEAPID heapID;

	//���b�Z�[�W�n
	GFL_BMPWIN* MessageWin;

  GFL_ARCUTIL_TRANSINFO bgchar;

	GFL_BMPWIN* StatusWin[2*4];

	TOUCH_SW_SYS			*TouchSubWindowSys;

  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
	GFL_BMPWIN* MyInfoWin;
	GFL_BMPWIN* mesWin;
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

  GFL_G3D_RES* pG3dRes;
  GFL_G3D_OBJ* pG3dObj;
	GFL_G3D_OBJSTATUS	status;
	GFL_G3D_CAMERA		*p_camera;

  //  G3D_MDL_WORK			mdl[_G3D_MDL_MAX];
	//    BMPWINFRAME_AREAMANAGER_POS aPos;
	//3D

  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex[ SETUP_INDEX_MAX ];
//  ICA_ANIME* icaAnime;
//  GFL_G3D_CAMERA* camera;


  BOX_DATA* pBox;
	GFL_G3D_CAMERA    *camera;

	MCSS_SYS_WORK *mcssSys;
	MCSS_WORK     *pokeMcss[2];

	GFL_ARCUTIL_TRANSINFO subchar;
	GFL_ARCUTIL_TRANSINFO subchar1;
	u32 cellRes[CEL_RESOURCE_MAX];

	GAMESYS_WORK* pGameSys;

	GFL_CLUNIT	*cellUnit;
	GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��

	u32 pokeIconNcgRes[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
	GFL_CLWK* pokeIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
	u32 pokeIconNo[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
	u32 pokeIconForm[_LING_LINENO_MAX][BOX_VERTICAL_NUM];

  GFL_CLWK* curIcon[CUR_NUM];

	int windowNum;
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
	GFL_CLWK* pCatchCLWK;   //����ł�|�P����CLACT


  int workPokeIndex;  // �}�E�X����łɂ������|�P����
  int workBoxno;  //�}�E�X����łɂ������|�P����


  int selectIndex;  //���̃|�P����Index
	int selectBoxno;  //���̃|�P����Box

  int underSelectIndex;  //�܂�����Ɍ����ĂȂ��|�P����Index
  int underSelectBoxno;  //�܂�����Ɍ����ĂȂ��|�P����Box
  int pokemonsetCall;

  
	BOOL bChangeOK[2];

	BOOL bParent;
	BOOL bTouchReset;

  short BoxScrollNum;   ///< �h�b�g�P�ʂňʒu���Ǘ�  8*20��BOX 8*12���Ă��� BOX_MAX_TRAY => 2880+96=2976
  short FriendBoxScrollNum;   ///< ��L�̑��葤�̒l
  
  short oldLine; //���݂̕`��Line �X�V�g���K
  
  BOOL bgscrollRenew;
  BOOL touckON;
  int bgscroll;
  int ringLineNo;
  
  u16* scrTray;
  u16* scrTemoti;
  u8* pCharMem;

};




extern void IRC_POKETRADE_GraphicInit(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_GraphicExit(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_SubStatusInit(IRC_POKEMON_TRADE* pWork,int pokeposx);
extern void IRC_POKETRADE_SubStatusEnd(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_AppMenuOpen(IRC_POKEMON_TRADE* pWork, int *menustr,int num);
extern void IRC_POKETRADE_MessageOpen(IRC_POKEMON_TRADE* pWork, int msgno);
extern void IRC_POKETRADE_MessageWindowOpen(IRC_POKEMON_TRADE* pWork, int msgno);
extern void IRC_POKETRADE_MessageWindowClose(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_MessageWindowClear(IRC_POKEMON_TRADE* pWork);
extern BOOL IRC_POKETRADE_MessageEndCheck(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_TrayDisp(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_InitBoxIcon( BOX_DATA* boxData ,IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADE_AllDeletePokeIconResource(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_PokeIcomPosSet(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_G3dDraw(IRC_POKEMON_TRADE* pWork);

extern void IRC_POKETRADEDEMO_Init( IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADEDEMO_Main( IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADEDEMO_End( IRC_POKEMON_TRADE* pWork );
extern GFL_CLWK* IRC_POKETRADE_GetCLACT( IRC_POKEMON_TRADE* pWork , int x, int y, int* trayno, int* pokeindex);


