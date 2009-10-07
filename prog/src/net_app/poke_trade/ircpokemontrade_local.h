//=============================================================================
/**
 * @file	  ircpokemontrade_local.h
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
#include "system/touch_subwindow.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "app/app_taskmenu.h"
#include "ircpokemontrade_anim.h"

typedef enum
{
  REEL_PANEL_OBJECT,
  TRADE01_OBJECT,
  TRADE_TRADE_OBJECT,
  TRADE_RETURN_OBJECT,
  TRADE_SPLASH_OBJECT,
  TRADE_END_OBJECT,
  OBJECT_MODEL_MAX,
} CELL_OBJECT_MODEL;


typedef enum
{
  SETUP_TRADE_BG_MODE_NORMAL,
  SETUP_TRADE_BG_MODE_DEMO,
} SETUP_TRADE_BG_MODE;


#define _TIMING_ENDNO (12)

#define BOX_VERTICAL_NUM (5)
#define BOX_HORIZONTAL_NUM (6)

#define HAND_VERTICAL_NUM (3)
#define HAND_HORIZONTAL_NUM (2)

//#define BOX_MONS_NUM (30)


#define _BRIGHTNESS_SYNC (2)  // �t�F�[�h�̂r�x�m�b�͗v����


typedef enum
{
  CELL_CUR_SELECT,
  CELL_CUR_MOJI,
  CELL_CUR_SCROLLBAR,
  CELL_CUR_POKE_SELECT,
  CELL_DISP_NUM
} _CUR_RESOURCE;


//#define CUR_NUM (3)
#define _BUTTON_WIN_PAL   (15)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (14)  // ���b�Z�[�W�t�H���g
#define	FBMP_COL_WHITE		(15)

#define _OBJPLT_BOX  (0)  //3�{
#define _OBJPLT_BAR  (3)  //3�{
#define _OBJPLT_POKEICON  (6)  //3�{
#define PLIST_RENDER_MAIN (1)


#define _POKE_PRJORTH_Y_COEFFICIENT (12)
#define _POKE_PRJORTH_X_COEFFICIENT (16)


#define _POKE_PRJORTH_TOP   (FX32_ONE*_POKE_PRJORTH_Y_COEFFICIENT)    ///< PRJORTH	��top	  :near�N���b�v�ʏ�ӂ�Y���W
#define _POKE_PRJORTH_RIGHT (FX32_ONE*_POKE_PRJORTH_X_COEFFICIENT)  ///< PRJORTH	��right	  :near�N���b�v�ʉE�ӂ�X���W

#define _MCSS_POS_X(x) ((x*FX32_ONE)/_POKE_PRJORTH_X_COEFFICIENT)
#define _MCSS_POS_Y(y) (((192-y)*FX32_ONE)/_POKE_PRJORTH_Y_COEFFICIENT)


#define PSTATUS_MCSS_POS_X1 _MCSS_POS_X(60)
#define PSTATUS_MCSS_POS_X2 _MCSS_POS_X(190)
#define PSTATUS_MCSS_POS_Y  _MCSS_POS_Y(140)



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

/// �h�b�g�P�ʂňʒu���Ǘ�  8*20��BOX 8*12���Ă��� BOX_MAX_TRAY => 2880+96=2976

#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))



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
  SETUP_INDEX_ROUND,
  SETUP_INDEX_MAX
};


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
  fx32   xstart;      ///< �J�n�ʒu
  fx32   xend;        ///< �I���ʒu
  fx32   zstart;      ///< �J�n�ʒu
  fx32   zend;        ///< �I���ʒu
}_POKEMCSS_MOVE_WORK;




struct _IRC_POKEMON_TRADE {
  u8 FriendPokemonCol[732];         ///< ����̃|�P����BOX�ɂ���|�P�����F
  POKEMON_PARAM* recvPoke[2];  ///< �󂯎�����|�P�������i�[����ꏊ
  BOOL bPokemonSet[2];              ///<
  //	EVENT_IRCBATTLE_WORK* pParentWork;
  StateFunc* state;      ///< �n���h���̃v���O�������
  HEAPID heapID;

  //���b�Z�[�W�n
  GFL_BMPWIN* MessageWin;

  GFL_ARCUTIL_TRANSINFO bgchar;

  GFL_BMPWIN* StatusWin[2*4];

  TOUCH_SW_SYS			*TouchSubWindowSys;
  GFL_PTC_PTR ptc;

  APP_TASKMENU_WORK* pAppTask;
  APP_TASKMENU_ITEMWORK appitem[_SUBMENU_LISTMAX];
  APP_TASKMENU_RES* pAppTaskRes;
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
  //  ICA_ANIME* icaAnime;
  //  GFL_G3D_CAMERA* camera;

  BOX_DATA* pBox;
  MYSTATUS* pMy;
  POKEPARTY* pMyParty;
  GFL_G3D_CAMERA    *camera;

  MCSS_SYS_WORK *mcssSys;
  MCSS_WORK     *pokeMcss[2];
  _POKEMCSS_MOVE_WORK* pMoveMcss[2];

  GFL_ARCUTIL_TRANSINFO subchar;
  GFL_ARCUTIL_TRANSINFO subchar1;
  GFL_ARCUTIL_TRANSINFO subchar2;
  u32 cellRes[CEL_RESOURCE_MAX];

  //	GAMESYS_WORK* pGameSys;

  GFL_CLUNIT	*cellUnit;
  GFL_TCB *g3dVintr; //3D�pvIntrTask�n���h��

  u32 pokeIconNcgRes[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* pokeIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconNo[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconForm[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconLine[_LING_LINENO_MAX][BOX_VERTICAL_NUM];

  GFL_CLWK* curIcon[CELL_DISP_NUM];

  int windowNum;
  int anmCount;
  int anmStep;
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

  int modelno;  ///< �\�����Ă��郂�f���̔ԍ�

  u16* scrTray;
  u16* scrTemoti;
  u8* pCharMem;

};


#define UNIT_NULL		(0xffff)

extern void IRC_POKMEONTRADE_ChangeFinish(IRC_POKEMON_TRADE* pWork);


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
extern void IRC_POKETRADE_SetSubdispGraphicDemo(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_GraphicFreeVram(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_SetMainDispGraphic(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_ResetSubDispGraphic(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_SetSubDispGraphic(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_3DGraphicSetUp( IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADE_SetBgMode(SETUP_TRADE_BG_MODE type);
extern void IRC_POKETRADE_CreatePokeIconResource(IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_InitBoxCursor(IRC_POKEMON_TRADE* pWork);


extern void IRC_POKETRADEDEMO_Init( IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADEDEMO_Main( IRC_POKEMON_TRADE* pWork );
extern void IRC_POKETRADEDEMO_End( IRC_POKEMON_TRADE* pWork );
extern GFL_CLWK* IRC_POKETRADE_GetCLACT( IRC_POKEMON_TRADE* pWork , int x, int y, int* trayno, int* pokeindex);
extern void IRC_POKETRADEDEMO_SetModel( IRC_POKEMON_TRADE* pWork, int modelno);
extern void IRC_POKETRADEDEMO_RemoveModel( IRC_POKEMON_TRADE* pWork);

extern void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(IRC_POKEMON_TRADE* pWork);

extern void IRCPOKETRADE_PokeDeleteMcss( IRC_POKEMON_TRADE *pWork,int no  );
extern void IRCPOKETRADE_PokeCreateMcss( IRC_POKEMON_TRADE *pWork ,int no, int bFront, const POKEMON_PARAM *pp );
extern POKEMON_PASO_PARAM* IRCPOKEMONTRADE_GetPokeDataAddress(BOX_DATA* boxData , int trayNo, int index,IRC_POKEMON_TRADE* pWork);
extern void IRC_POKETRADE_SetMainStatusBG(IRC_POKEMON_TRADE* pWork);



#if _TRADE_DEBUG

extern void IRC_POKMEONTRADE_changeStateDebug(IRC_POKEMON_TRADE* pWork,StateFunc* state, int line);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeStateDebug(pWork ,state, __LINE__)

#else  //_NET_DEBUG

extern void IRC_POKMEONTRADE_changeState(IRC_POKEMON_TRADE* pWork,StateFunc* state);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeState(pWork ,state)

#endif //_NET_DEBUG
