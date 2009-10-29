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
#include "pokemontrade_snd.h"
#include "sound/pm_sndsys.h"

#include "print/wordset.h"

#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "app/app_taskmenu.h"
#include "ircpokemontrade_anim.h"
#include "system/ica_anime.h"
#include "ui/ui_easy_clwk.h"
//�^�b�`�o�[
#include "ui/touchbar.h"
#include "savedata/mail_util.h"


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




typedef enum
{
  _CHANGERAND = 43,    ///< �|�P���������^�C�~���O�h�炬��
  _TIMING_ENDNO=12,
  _TIMING_TRADEDEMO_START,
  _TIMING_SAVEST,
  _TIMING_SAVELAST,
  _TIMING_SAVEEND,
  _TIMING_ANIMEEND,
  
 } NET_TIMING_ENUM;

#define BOX_VERTICAL_NUM (5)
#define BOX_HORIZONTAL_NUM (6)

#define HAND_VERTICAL_NUM (3)
#define HAND_HORIZONTAL_NUM (2)

#define TRADEBOX_LINEMAX (BOX_MAX_TRAY * 6 + 2)

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


#define _OBJPLT_POKEICON_GRAY_OFFSET (0) ///< �O���[�p�|�P�����A�C�R���p���b�g
#define _OBJPLT_POKEICON_GRAY  (3)  //�|�P�����A�C�R���O���C 3�{
#define _OBJPLT_POKEICON_OFFSET (_OBJPLT_POKEICON_GRAY*32)
#define _OBJPLT_POKEICON  (3)  //�|�P�����A�C�R�� 3�{
#define _OBJPLT_COMMON_OFFSET (_OBJPLT_POKEICON_OFFSET + _OBJPLT_POKEICON*32)
#define _OBJPLT_COMMON  (2)  //�T�u���OBJ�p���b�g����ʃo�[ 2�{
#define _OBJPLT_BOX_OFFSET (_OBJPLT_COMMON_OFFSET+_OBJPLT_COMMON*32)
#define _OBJPLT_BOX  (6)  //�T�u���OBJ�p���b�g��{  5�{

#define PLTID_OBJ_POKEITEM_S (_OBJPLT_POKEICON_GRAY+_OBJPLT_COMMON+_OBJPLT_BOX+_OBJPLT_POKEICON+1) //15�{��


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

#define PSTATUS_MCSS_POS_X1 _MCSS_POS_X(-60)            //�����ŏ��̈ʒuX
#define PSTATUS_MCSS_POS_X2 _MCSS_POS_X(60)             //����ŏ��̈ʒuX
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

#define TOTAL_DOT_MAX  ((BOX_MAX_TRAY * 20 * 8) + (12 * 8))



typedef void (StateFunc)(POKEMON_TRADE_WORK* pState);


typedef enum
{
  PLT_POKEICON,   //ARCID_POKEICON
  PAL_SCROLLBAR, //ARCID_POKETRADE
  PLT_COMMON,
  PLT_POKECREATE,
  PLT_POKEICON_GRAY,
  PLT_RESOURCE_MAX,

  CHAR_SCROLLBAR = PLT_RESOURCE_MAX,    //ARCID_POKETRADE
  CHAR_SELECT_POKEICON1, //�I�𒆃|�P�����A�C�R���P
  CHAR_SELECT_POKEICON2, //�I�𒆃|�P�����A�C�R���P
  CHAR_COMMON,
  CHAR_POKECREATE,
  CHAR_RESOURCE_MAX,

  ANM_POKEICON = CHAR_RESOURCE_MAX,  //ARCID_POKEICON
  ANM_SCROLLBAR, //ARCID_POKETRADE
  ANM_COMMON,
  ANM_POKECREATE,
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
  //CELL_CUR_POKE_KEY,
  CELL_DISP_NUM
} _CELL_DISP_TYPE;



// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_BALL,
  SETUP_INDEX_ROUND,
  SETUP_INDEX_MAX
};

// �|�P�����X�e�[�^�X�������Ƒ����؂�ւ���ۂ�CELL�̈ʒu
#define _POKEMON_SELECT1_CELLX  (128-32)
#define _POKEMON_SELECT1_CELLY  (12)
#define _POKEMON_SELECT2_CELLX  (128+32)
#define _POKEMON_SELECT2_CELLY  (12)

// ���{�ꌟ������MAX
#define JAPANESE_SEARCH_INDEX_MAX (44)


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
}_POKEMCSS_MOVE_WORK;


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

typedef enum
{
  PTC_KIND_DEMO1,
  PTC_KIND_DEMO2,
  PTC_KIND_DEMO3,
  PTC_KIND_DEMO4,
  PTC_KIND_DEMO5,
  PTC_KIND_DEMO6,
  PTC_KIND_DEMO7,
  PTC_KIND_DEMO8,
//  PTC_KIND_DEMO9,
//  PTC_KIND_DEMO10,
//  PTC_KIND_DEMO11,
  PTC_KIND_NUM_MAX,
} _TYPE_PTC_KIND;

#define _POKEMARK_MAX  (8)




struct _POKEMON_TRADE_WORK{
  u8 FriendPokemonCol[732];         ///< ����̃|�P����BOX�ɂ���|�P�����F
  GFL_BMPWIN* StatusWin[2];     ///< �X�e�[�^�X�\��
  POKEMON_PARAM* recvPoke[2];  ///< �󂯎�����|�P�������i�[����ꏊ
  StateFunc* state;      ///< �n���h���̃v���O�������
  HEAPID heapID;

  
  GFL_BMPWIN* MessageWin;  ///< ���b�Z�[�W�n�\��

  GFL_ARCUTIL_TRANSINFO bgchar;


  GFL_PTC_PTR ptc[PTC_KIND_NUM_MAX];
  GFL_PTC_PTR ptcOrthogonal;

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
//  ICA_ANIME* icaAnime;
  ICA_ANIME* icaCamera;
  ICA_ANIME* icaTarget;
  //  GFL_G3D_CAMERA* camera;
  ICA_ANIME* icaBallin;
  GFL_EMIT_PTR pBallInPer;
  ICA_ANIME* icaBallout;
  GFL_EMIT_PTR pBallOutPer;
  GFL_CLWK* pPokeCreateCLWK;   //�|�P�����o��G�t�F�N�gCLACT


  BOX_MANAGER* pBox;
  GAMEDATA* pGameData;
  MAIL_BLOCK* pMailBlock;
  MYSTATUS* pMy;
  MYSTATUS* pFriend;
  POKEPARTY* pMyParty;
  GFL_G3D_CAMERA    *camera;

  MCSS_SYS_WORK *mcssSys;
  MCSS_WORK     *pokeMcss[4];
  BOOL     mcssStop[2];
  _POKEMCSS_MOVE_WORK* pMoveMcss[2];

  GFL_ARCUTIL_TRANSINFO subchar;
  GFL_ARCUTIL_TRANSINFO subchar1;
  GFL_ARCUTIL_TRANSINFO subchar2;
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

  u32 pokeIconNcgRes[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* pokeIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  GFL_CLWK* markIcon[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconNo[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u32 pokeIconForm[_LING_LINENO_MAX][BOX_VERTICAL_NUM];
  u8 pokeIconLine[_LING_LINENO_MAX][BOX_VERTICAL_NUM];

  GFL_CLWK* curIcon[CELL_DISP_NUM];

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
  

  int MainObjCursorLine;   //OBJ�J�[�\�����C��
  int MainObjCursorIndex;  //OBJ�J�[�\���C���f�b�N�X

  int workPokeIndex;  // �}�E�X����łɂ������|�P����
  int workBoxno;  //�}�E�X����łɂ������|�P����


  int selectIndex;  //���̃|�P����Index
  int selectBoxno;  //���̃|�P����Box

  int underSelectIndex;  //�܂�����Ɍ����ĂȂ��|�P����Index
  int underSelectBoxno;  //�܂�����Ɍ����ĂȂ��|�P����Box
  int pokemonsetCall;
  int userNetCommand[2];


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

  int modelno;  ///< �\�����Ă��郂�f���̔ԍ�
  int pokemonselectno;  ///< �����Ƒ���̃|�P�����I�𒆂̂ǂ�����w���Ă��邩

  u16* scrTray;
  u16* scrTemoti;
  u8* pCharMem;

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

extern void IRC_POKETRADE_AppMenuOpen(POKEMON_TRADE_WORK* pWork, int *menustr,int num);
extern void IRC_POKETRADE_AppMenuClose(POKEMON_TRADE_WORK* pWork);

extern void IRC_POKETRADE_MessageOpen(POKEMON_TRADE_WORK* pWork, int msgno);
extern void IRC_POKETRADE_MessageWindowOpen(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_MessageWindowClose(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_MessageWindowClear(POKEMON_TRADE_WORK* pWork);
extern BOOL IRC_POKETRADE_MessageEndCheck(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_TrayDisp(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_InitBoxIcon( BOX_MANAGER* boxData ,POKEMON_TRADE_WORK* pWork );
extern void IRC_POKETRADE_AllDeletePokeIconResource(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_G3dDraw(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubdispGraphicDemo(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_GraphicFreeVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_ResetSubDispGraphic(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetSubVram(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetMainVram(POKEMON_TRADE_WORK* pWork);

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
extern GFL_CLWK* IRC_POKETRADE_GetCLACT( POKEMON_TRADE_WORK* pWork , int x, int y, int* trayno, int* pokeindex, int* outline, int* outindex);

extern void IRC_POKETRADEDEMO_SetModel( POKEMON_TRADE_WORK* pWork, int modelno);
extern void IRC_POKETRADEDEMO_RemoveModel( POKEMON_TRADE_WORK* pWork);

extern void IRC_POKMEONTRADE_STEP_ChangeDemo_PokeMove(POKEMON_TRADE_WORK* pWork);

extern void IRCPOKETRADE_PokeDeleteMcss( POKEMON_TRADE_WORK *pWork,int no  );
extern void IRCPOKETRADE_PokeCreateMcss( POKEMON_TRADE_WORK *pWork ,int no, int bFront, const POKEMON_PARAM *pp );
extern POKEMON_PASO_PARAM* IRCPOKEMONTRADE_GetPokeDataAddress(BOX_MANAGER* boxData , int trayNo, int index,POKEMON_TRADE_WORK* pWork);
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
extern void IRC_POKETRADE_PokeStatusIconDisp(POKEMON_TRADE_WORK* pWork, POKEMON_PARAM* pp);
extern void IRC_POKETRADE_PokeStatusIconReset(POKEMON_TRADE_WORK* pWork);
extern void IRC_POKETRADE_SetCursorXY(POKEMON_TRADE_WORK* pWork);
extern BOOL POKETRADE_IsMainCursorDispIn(POKEMON_TRADE_WORK* pWork,int* line);
extern int POKETRADE_Line2RingLineIconGet(int line);
extern void POKETRADE_ToolBarInit(POKEMON_TRADE_WORK* pWork);
extern BOOL POKEMONTRADE_IsPokeLanguageMark(int monsno,int moji);


#if _TRADE_DEBUG

extern void IRC_POKMEONTRADE_changeStateDebug(POKEMON_TRADE_WORK* pWork,StateFunc* state, int line);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeStateDebug(pWork ,state, __LINE__)

#else  //_NET_DEBUG

extern void IRC_POKMEONTRADE_changeState(POKEMON_TRADE_WORK* pWork,StateFunc* state);

#define   _CHANGE_STATE(pWork, state)  IRC_POKMEONTRADE_changeState(pWork ,state)

#endif //_NET_DEBUG


#define _TEMOTITRAY_SCR_MAX (12)
#define _BOXTRAY_SCR_MAX (20)
#define _SRCMAX ((BOX_MAX_TRAY*_BOXTRAY_SCR_MAX)+_TEMOTITRAY_SCR_MAX)

#define _TEMOTITRAY_MAX (8*_TEMOTITRAY_SCR_MAX)
#define _BOXTRAY_MAX (8*_BOXTRAY_SCR_MAX)

#define _DOTMAX ((BOX_MAX_TRAY*_BOXTRAY_MAX)+_TEMOTITRAY_MAX)

#define _DOT_START_POS (_DOTMAX - 80)


extern int IRC_TRADE_LINE2TRAY(int lineno);
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
#define	FBMP_COL_WHITE_SDW		(14)
#define	FBMP_COL_WHITE		(15)
