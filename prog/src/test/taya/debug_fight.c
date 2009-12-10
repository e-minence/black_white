//============================================================================================
/**
 * @file  debug_fight.c
 * @brief �f�o�b�O�o�g���ݒ���
 * @author  taya
 * @date  2009.10.20
 */
//============================================================================================

// lib includes -----------------------
#include <gflib.h>
#include <assert_default.h>
#include <procsys.h>
#include <tcbl.h>
#include <msgdata.h>
#include <net.h>


// global includes --------------------
#include "system\main.h"
#include "print\printsys.h"
#include "print\gf_font.h"
#include "poke_tool\pokeparty.h"
#include "poke_tool\poke_tool.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazadata.h"
#include "item\itemsym.h"
#include "net\network_define.h"
#include "battle\battle.h"
#include "gamesystem\btl_setup.h"
#include "sound\pm_sndsys.h"
#include "gamesystem\game_data.h"
#include "gamesystem\btl_setup.h"
#include "savedata\save_tbl.h"
#include "savedata\config.h"
#include "debug\debug_makepoke.h"
#include "poke_tool\monsno_def.h"
#include "savedata\battle_rec.h"
#include "battle\battle.h"


// local includes ---------------------
#include "msg\msg_debug_fight.h"

// archive includes -------------------
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  POKEPARA_MAX = 24,    // �ő�S�p�[�e�B�~�U�̂�24�̂�ۑ�
  POKEPARA_SIZE = 228,  // �|�P�����p�����[�^�z��T�C�Y
  POKEPARA_SAVEAREA_SIZE = POKEPARA_SIZE * POKEPARA_MAX,

  PAGE_MAX = 2,

  SAVEAREA_SIZE = 0x2000,

};

/*--------------------------------------------------------------------------*/
/* Enums                                                                    */
/*--------------------------------------------------------------------------*/
typedef enum {
  BTLTYPE_SINGLE_WILD,
  BTLTYPE_SINGLE_TRAINER,
  BTLTYPE_SINGLE_COMM,
  BTLTYPE_DOUBLE_WILD,
  BTLTYPE_DOUBLE_TRAINER1,
  BTLTYPE_DOUBLE_TRAINER2,
  BTLTYPE_DOUBLE_COMM,
  BTLTYPE_DOUBLE_COMM_MULTI,
  BTLTYPE_TRIPLE_TRAINER,
  BTLTYPE_TRIPLE_COMM,
  BTLTYPE_ROTATION_TRAINER,
  BTLTYPE_ROTATION_COMM,

  BTLTYPE_MAX,
}BtlType;

typedef enum {
  POKEIDX_SELF_1,
  POKEIDX_SELF_2,
  POKEIDX_SELF_3,
  POKEIDX_SELF_4,
  POKEIDX_SELF_5,
  POKEIDX_SELF_6,
  POKEIDX_ENEMY1_1,
  POKEIDX_ENEMY1_2,
  POKEIDX_ENEMY1_3,
  POKEIDX_ENEMY1_4,
  POKEIDX_ENEMY1_5,
  POKEIDX_ENEMY1_6,
  POKEIDX_FRIEND_1,
  POKEIDX_FRIEND_2,
  POKEIDX_FRIEND_3,
  POKEIDX_FRIEND_4,
  POKEIDX_FRIEND_5,
  POKEIDX_FRIEND_6,
  POKEIDX_ENEMY2_1,
  POKEIDX_ENEMY2_2,
  POKEIDX_ENEMY2_3,
  POKEIDX_ENEMY2_4,
  POKEIDX_ENEMY2_5,
  POKEIDX_ENEMY2_6,
}PokeIdx;

typedef enum {
  DBF_RECMODE_NONE=0,
  DBF_RECMODE_REC,
  DBF_RECMODE_PLAY,

  DBF_RECMODE_MAX,
}RecMode;

typedef enum {
  SELITEM_POKE_SELF_1,
  SELITEM_POKE_SELF_2,
  SELITEM_POKE_SELF_3,
  SELITEM_POKE_SELF_4,
  SELITEM_POKE_SELF_5,
  SELITEM_POKE_SELF_6,
  SELITEM_POKE_ENEMY1_1,
  SELITEM_POKE_ENEMY1_2,
  SELITEM_POKE_ENEMY1_3,
  SELITEM_POKE_ENEMY1_4,
  SELITEM_POKE_ENEMY1_5,
  SELITEM_POKE_ENEMY1_6,
  SELITEM_POKE_FRIEND_1,
  SELITEM_POKE_FRIEND_2,
  SELITEM_POKE_FRIEND_3,
  SELITEM_POKE_FRIEND_4,
  SELITEM_POKE_FRIEND_5,
  SELITEM_POKE_FRIEND_6,
  SELITEM_POKE_ENEMY2_1,
  SELITEM_POKE_ENEMY2_2,
  SELITEM_POKE_ENEMY2_3,
  SELITEM_POKE_ENEMY2_4,
  SELITEM_POKE_ENEMY2_5,
  SELITEM_POKE_ENEMY2_6,

  SELITEM_BTL_TYPE,
  SELITEM_SAVE,
  SELITEM_LOAD,
  SELITEM_MSGSPEED,
  SELITEM_WAZAEFF,
  SELITEM_SUBWAYMODE,
  SELITEM_REC_MODE,
  SELITEM_REC_BUF,

  SELITEM_MUST_TUIKA,
  SELITEM_MUST_TOKU,
  SELITEM_MUST_ITEM,
  SELITEM_MUST_CRITICAL,
  SELITEM_HP_CONST,
  SELITEM_PP_CONST,

  SELITEM_MAX,
  SELITEM_NULL = SELITEM_MAX,

}SelectItem;

/*--------------------------------------------------------------------------*/
/* Layout                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  LAYOUT_PARTY_LABEL_LINE_Y = 8,
  LAYOUT_PARTY_DATA_LINE_Y  = 28,
  LAYOUT_PARTY_DATA_LINE_HEIGHT = 15,
  LAYOUT_PARTY_LINE_OX = 10,
  LAYOUT_PARTY_LINE_WIDTH = 60,

  LAYOUT_PARTY_LINE1_X = LAYOUT_PARTY_LINE_OX,
  LAYOUT_PARTY_LINE2_X = LAYOUT_PARTY_LINE_OX+LAYOUT_PARTY_LINE_WIDTH,
  LAYOUT_PARTY_LINE3_X = LAYOUT_PARTY_LINE_OX+LAYOUT_PARTY_LINE_WIDTH*2,
  LAYOUT_PARTY_LINE4_X = LAYOUT_PARTY_LINE_OX+LAYOUT_PARTY_LINE_WIDTH*3,

  LAYOUT_PARTY_LINE_Y1 = LAYOUT_PARTY_DATA_LINE_Y,
  LAYOUT_PARTY_LINE_Y2 = LAYOUT_PARTY_DATA_LINE_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT,
  LAYOUT_PARTY_LINE_Y3 = LAYOUT_PARTY_DATA_LINE_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*2,
  LAYOUT_PARTY_LINE_Y4 = LAYOUT_PARTY_DATA_LINE_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*3,
  LAYOUT_PARTY_LINE_Y5 = LAYOUT_PARTY_DATA_LINE_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*4,
  LAYOUT_PARTY_LINE_Y6 = LAYOUT_PARTY_DATA_LINE_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*5,

  LAYOUT_PARAM_LINE_HEIGHT = 16,
  LAYOUT_PARAM_LINE_Y1 = 128,
  LAYOUT_PARAM_LINE_Y2 = 128 + LAYOUT_PARAM_LINE_HEIGHT,
  LAYOUT_PARAM_LINE_Y3 = 128 + LAYOUT_PARAM_LINE_HEIGHT*2,
  LAYOUT_PARAM_LINE_Y4 = 128 + LAYOUT_PARAM_LINE_HEIGHT*3,

  LAYOUT_LABEL_BTLTYPE_X  = 8,
  LAYOUT_LABEL_MSGSPEED_X = 144,
  LAYOUT_LABEL_WAZAEFF_X  = 144,

  LAYOUT_LABEL_SUBWAY_X = 144,
  LAYOUT_LABEL_SUBWAY_Y = LAYOUT_PARAM_LINE_Y3,

  LAYOUT_LABEL_REC_X = 144,
  LAYOUT_LABEL_REC_Y = LAYOUT_PARAM_LINE_Y4,

  LAYOUT_CRIPMARK_OX = 240,
  LAYOUT_CRIPMARK_OY = 4,
  LAYOUT_CRIPMARK_WIDTH = 12,
  LAYOUT_CRIPMARK_HEIGHT = 12,


  LAYOUT_LABEL_MUST_TUIKA_X     = 4,
  LAYOUT_LABEL_MUST_TOKU_X      = 4,
  LAYOUT_LABEL_MUST_ITEM_X      = 4,
  LAYOUT_LABEL_MUST_CRITICAL_X  = 4,
  LAYOUT_LABEL_HP_CONST_X       = 4,
  LAYOUT_LABEL_PP_CONST_X       = 4,

  LAYOUT_LABEL_MUST_TUIKA_Y     = 8,
  LAYOUT_LABEL_MUST_TOKU_Y      = LAYOUT_LABEL_MUST_TUIKA_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT,
  LAYOUT_LABEL_MUST_ITEM_Y      = LAYOUT_LABEL_MUST_TUIKA_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*2,
  LAYOUT_LABEL_MUST_CRITICAL_Y  = LAYOUT_LABEL_MUST_TUIKA_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*3,
  LAYOUT_LABEL_HP_CONST_Y       = LAYOUT_LABEL_MUST_TUIKA_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*4,
  LAYOUT_LABEL_PP_CONST_Y       = LAYOUT_LABEL_MUST_TUIKA_Y+LAYOUT_PARTY_DATA_LINE_HEIGHT*5,

};

//======================================================================
/*
 *  ���x�����C�A�E�g��`�p�\����
 */
//======================================================================
typedef struct {
  u16  strID;
  u8   x;
  u8   y;
}LABEL_LAYOUT;

//------------------------------------------------------
/*
 *  ���x�����C�A�E�g�i�P�y�[�W�ځj
 */
//------------------------------------------------------
static const LABEL_LAYOUT LabelLayout_Page1[] = {
  { DBGF_LABEL_FRIEND1, LAYOUT_PARTY_LINE1_X,    LAYOUT_PARTY_LABEL_LINE_Y },
  { DBGF_LABEL_ENEMY1,  LAYOUT_PARTY_LINE2_X,    LAYOUT_PARTY_LABEL_LINE_Y },
  { DBGF_LABEL_FRIEND2, LAYOUT_PARTY_LINE3_X,    LAYOUT_PARTY_LABEL_LINE_Y },
  { DBGF_LABEL_ENEMY2,  LAYOUT_PARTY_LINE4_X,    LAYOUT_PARTY_LABEL_LINE_Y },
  { DBGF_LABEL_TYPE,    LAYOUT_LABEL_BTLTYPE_X,  LAYOUT_PARAM_LINE_Y1      },
  { DBGF_LABEL_MSGSPD,  LAYOUT_LABEL_MSGSPEED_X, LAYOUT_PARAM_LINE_Y1      },
  { DBGF_LABEL_WAZAEFF, LAYOUT_LABEL_WAZAEFF_X,  LAYOUT_PARAM_LINE_Y2      },
  { DBGF_LABEL_SUBWAY,  LAYOUT_LABEL_SUBWAY_X,   LAYOUT_LABEL_SUBWAY_Y     },
  { DBGF_LABEL_RECORD,  LAYOUT_LABEL_REC_X,      LAYOUT_LABEL_REC_Y        },

  { DBGF_LABEL_LAND,    8, LAYOUT_PARAM_LINE_Y2 },
  { DBGF_LABEL_WEATHER, 8, LAYOUT_PARAM_LINE_Y3 },
};
//------------------------------------------------------
/*
 *  ���x�����C�A�E�g�i�Q�y�[�W�ځj
 */
//------------------------------------------------------
static const LABEL_LAYOUT LabelLayout_Page2[] = {
  { DBGF_LABEL_MUST_TUIKA,    LAYOUT_LABEL_MUST_TUIKA_X,    LAYOUT_LABEL_MUST_TUIKA_Y    },
  { DBGF_LABEL_MUST_TOKU,     LAYOUT_LABEL_MUST_TOKU_X,     LAYOUT_LABEL_MUST_TOKU_Y     },
  { DBGF_LABEL_MUST_ITEM,     LAYOUT_LABEL_MUST_ITEM_X,     LAYOUT_LABEL_MUST_ITEM_Y     },
  { DBGF_LABEL_MUST_CRITICAL, LAYOUT_LABEL_MUST_CRITICAL_X, LAYOUT_LABEL_MUST_CRITICAL_Y },
  { DBGF_LABEL_HP_CONST,      LAYOUT_LABEL_HP_CONST_X,      LAYOUT_LABEL_HP_CONST_Y      },
  { DBGF_LABEL_PP_CONST,      LAYOUT_LABEL_PP_CONST_X,      LAYOUT_LABEL_PP_CONST_Y      },
};

//======================================================================
/*
 *  �A�C�e�����C�A�E�g��`�p�\����
 */
//======================================================================
typedef struct {
  u16  itemID;
  u8   x;
  u8   y;
  int  min;
  int  max;
}ITEM_LAYOUT;

/**
 *  �A�C�e�����C�A�E�g�i�P�y�[�W�ځj
 */
static const ITEM_LAYOUT ItemLayout_Page1[] = {
  { SELITEM_POKE_SELF_1,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y1,  0,0,   },
  { SELITEM_POKE_SELF_2,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y2,  0,0,   },
  { SELITEM_POKE_SELF_3,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y3,  0,0,   },
  { SELITEM_POKE_SELF_4,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y4,  0,0,   },
  { SELITEM_POKE_SELF_5,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y5,  0,0,   },
  { SELITEM_POKE_SELF_6,    LAYOUT_PARTY_LINE1_X, LAYOUT_PARTY_LINE_Y6,  0,0,   },

  { SELITEM_POKE_ENEMY1_1,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y1,  0,0,   },
  { SELITEM_POKE_ENEMY1_2,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y2,  0,0,   },
  { SELITEM_POKE_ENEMY1_3,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y3,  0,0,   },
  { SELITEM_POKE_ENEMY1_4,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y4,  0,0,   },
  { SELITEM_POKE_ENEMY1_5,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y5,  0,0,   },
  { SELITEM_POKE_ENEMY1_6,  LAYOUT_PARTY_LINE2_X, LAYOUT_PARTY_LINE_Y6,  0,0,   },

  { SELITEM_POKE_FRIEND_1,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y1,  0,0,   },
  { SELITEM_POKE_FRIEND_2,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y2,  0,0,   },
  { SELITEM_POKE_FRIEND_3,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y3,  0,0,   },
  { SELITEM_POKE_FRIEND_4,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y4,  0,0,   },
  { SELITEM_POKE_FRIEND_5,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y5,  0,0,   },
  { SELITEM_POKE_FRIEND_6,  LAYOUT_PARTY_LINE3_X, LAYOUT_PARTY_LINE_Y6,  0,0,   },

  { SELITEM_POKE_ENEMY2_1,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y1,  0,0,   },
  { SELITEM_POKE_ENEMY2_2,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y2,  0,0,   },
  { SELITEM_POKE_ENEMY2_3,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y3,  0,0,   },
  { SELITEM_POKE_ENEMY2_4,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y4,  0,0,   },
  { SELITEM_POKE_ENEMY2_5,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y5,  0,0,   },
  { SELITEM_POKE_ENEMY2_6,  LAYOUT_PARTY_LINE4_X, LAYOUT_PARTY_LINE_Y6,  0,0,   },

  { SELITEM_BTL_TYPE,       LAYOUT_LABEL_BTLTYPE_X  +30, LAYOUT_PARAM_LINE_Y1,  0, BTLTYPE_MAX-1     },
  { SELITEM_MSGSPEED,       LAYOUT_LABEL_MSGSPEED_X +52, LAYOUT_PARAM_LINE_Y1,  0, MSGSPEED_FAST_EX  },
  { SELITEM_WAZAEFF,        LAYOUT_LABEL_WAZAEFF_X  +64, LAYOUT_PARAM_LINE_Y2,  0, 1                 },
  { SELITEM_SUBWAYMODE,     LAYOUT_LABEL_SUBWAY_X   +48, LAYOUT_LABEL_SUBWAY_Y, 0, 1                 },
  { SELITEM_REC_MODE,       LAYOUT_LABEL_REC_X      +32, LAYOUT_LABEL_REC_Y,    0, DBF_RECMODE_MAX-1 },
  { SELITEM_REC_BUF,        LAYOUT_LABEL_REC_X      +64, LAYOUT_LABEL_REC_Y,    0, 3                 },

  { SELITEM_LOAD,        8, LAYOUT_PARAM_LINE_Y4 },
  { SELITEM_SAVE,       38, LAYOUT_PARAM_LINE_Y4 },
};

/**
 *  �A�C�e�����C�A�E�g�i�Q�y�[�W�ځj
 */
static const ITEM_LAYOUT ItemLayout_Page2[] = {
  { SELITEM_MUST_TUIKA,     LAYOUT_LABEL_MUST_TUIKA_X    +68, LAYOUT_LABEL_MUST_TUIKA_Y,    0, 1 },
  { SELITEM_MUST_TOKU,      LAYOUT_LABEL_MUST_TOKU_X     +68, LAYOUT_LABEL_MUST_TOKU_Y,     0, 1 },
  { SELITEM_MUST_ITEM,      LAYOUT_LABEL_MUST_ITEM_X     +68, LAYOUT_LABEL_MUST_ITEM_Y,     0, 1 },
  { SELITEM_MUST_CRITICAL,  LAYOUT_LABEL_MUST_CRITICAL_X +68, LAYOUT_LABEL_MUST_CRITICAL_Y, 0, 1 },
  { SELITEM_HP_CONST,       LAYOUT_LABEL_HP_CONST_X      +68, LAYOUT_LABEL_HP_CONST_Y,      0, 1 },
  { SELITEM_PP_CONST,       LAYOUT_LABEL_PP_CONST_X      +68, LAYOUT_LABEL_PP_CONST_Y,      0, 1 },

};

//----------------------------------------------------------------------
/*
 *  ���x���E�A�C�e�����C�A�E�g�e�[�u���{��
 */
//----------------------------------------------------------------------
static const struct {
  const LABEL_LAYOUT*   labelLayout;
  const ITEM_LAYOUT*    itemLayout;
  u16                   labelElems;
  u16                   itemElems;

}LayoutTable[] = {
  { LabelLayout_Page1, ItemLayout_Page1,  NELEMS(LabelLayout_Page1), NELEMS(ItemLayout_Page1) },
  { LabelLayout_Page2, ItemLayout_Page2,  NELEMS(LabelLayout_Page2), NELEMS(ItemLayout_Page2) },
};


/**
 * BtlRule -> �ΐ�p�����[�^�ϊ��e�[�u��
 */
static const struct {
  u8  commFlag     : 1;
  u8  wildFlag     : 1;
  u8  enemyPokeReg : 6;   // �G�|�P�K�萔�i0�Ȃ玩�R�j
  u8  rule;
}BtlTypeParams[] = {
  { 0, 1, 1, BTL_RULE_SINGLE   },   // BTLTYPE_SINGLE_WILD
  { 0, 0, 0, BTL_RULE_SINGLE   },   // BTLTYPE_SINGLE_TRAINER
  { 1, 0, 0, BTL_RULE_SINGLE   },   // BTLTYPE_SINGLE_COMM
  { 0, 1, 2, BTL_RULE_DOUBLE   },   // BTLTYPE_DOUBLE_WILD
  { 0, 0, 0, BTL_RULE_DOUBLE   },   // BTLTYPE_DOUBLE_TRAINER1,
  { 0, 0, 0, BTL_RULE_DOUBLE   },   // BTLTYPE_DOUBLE_TRAINER2,
  { 1, 0, 0, BTL_RULE_DOUBLE   },   // BTLTYPE_DOUBLE_COMM
  { 1, 0, 0, BTL_RULE_DOUBLE   },   // BTLTYPE_DOUBLE_COMM_DOUBLE
  { 0, 0, 0, BTL_RULE_TRIPLE   },   // BTLTYPE_TRIPLE_TRAINER
  { 1, 0, 0, BTL_RULE_TRIPLE   },   // BTLTYPE_TRIPLE_COMM
  { 0, 0, 3, BTL_RULE_ROTATION },   // BTLTYPE_ROTATION_TRAINER
  { 1, 0, 0, BTL_RULE_ROTATION },   // BTLTYPE_ROTATION_COMM
};

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
typedef struct _DEBUG_BTL_WORK  DEBUG_BTL_WORK;
typedef BOOL (*pMainProc)( DEBUG_BTL_WORK*, int* );

/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
 *  SaveData
 */
typedef struct {
  u32  btlType  : 5;
  u32  weather  : 3;
  u32  msgSpeed : 3;
  u32  fWazaEff : 1;
  u32  fSubway  : 1;
  u32  recMode  : 2;
  u32  recBufID : 2;

  u32  fMustTuika    : 1;
  u32  fMustToku     : 1;
  u32  fMustItem     : 1;
  u32  fMustCritical : 1;
  u32  fHPConst      : 1;
  u32  fPPConst      : 1;

  u32  dmy      : 7;

  u8  pokeParaArea[ POKEPARA_SAVEAREA_SIZE ];

}DEBUG_BTL_SAVEDATA;

/**
 *  Main Work
 */
struct _DEBUG_BTL_WORK {
  GAMEDATA*       gameData;
  GFL_BMPWIN*     win;
  GFL_BMP_DATA*   bmp;
  GFL_MSGDATA*    mm;
  GFL_MSGDATA*    mmMonsName;
  STRBUF*         strbuf;
  GFL_FONT*       fontHandle;
  PRINT_QUE*      printQue;
  PRINT_UTIL      printUtil[1];
  SelectItem      selectItem;
  POKEPARTY*      partyPlayer;
  POKEPARTY*      partyFriend;
  POKEPARTY*      partyEnemy1;
  POKEPARTY*      partyEnemy2;
  const POKEMON_PARAM*  clipPoke;
  HEAPID          heapID;
  u8              fNetConnect;
  u8              fPlayerPartyAllocated;
  u8              pageNum;
  pMainProc       mainProc;
  int             mainSeq;
  u8              prevItemStrWidth[ SELITEM_MAX ];
  PROCPARAM_DEBUG_MAKEPOKE  makePokeParam;
  BATTLE_SETUP_PARAM  setupParam;
  SAVE_CONTROL_WORK*  saveCtrl;
  u16                 saveSeq0;
  u16                 saveSeq1;


  DEBUG_BTL_SAVEDATA  saveData;

  u8   recBuffer[ 4096 ];
  u32  recDataSize;
  GFL_STD_RandContext  recRand;

};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static GFL_PROC_RESULT DebugFightProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DebugFightProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT DebugFightProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void initGraphicSystems( HEAPID heapID );
static void quitGraphicSystems( void );
static void createTemporaryModules( DEBUG_BTL_WORK* wk, HEAPID heapID );
static void deleteTemporaryModules( DEBUG_BTL_WORK* wk );
static void changeScene_start( DEBUG_BTL_WORK* wk );
static void changeScene_recover( DEBUG_BTL_WORK* wk );
static void savework_Init( DEBUG_BTL_SAVEDATA* saveData );
static POKEMON_PARAM* savework_GetPokeParaArea( DEBUG_BTL_SAVEDATA* saveData, u32 pokeIdx );
static void savework_SetParty( DEBUG_BTL_SAVEDATA* saveData, DEBUG_BTL_WORK* wk );
static void setMainProc( DEBUG_BTL_WORK* wk, pMainProc nextProc );
static inline BOOL selItem_IsPoke( u16 itemID );
static void selItem_Increment( DEBUG_BTL_WORK* wk, u16 itemID, int incValue );
static int loopValue( int value, int min, int max );
static BOOL btltype_IsComm( BtlType type );
static BOOL btltype_IsMulti( BtlType type );
static BOOL btltype_IsWild( BtlType type );
static BtlRule btltype_GetRule( BtlType type );
static void PrintItem( DEBUG_BTL_WORK* wk, u16 itemID, BOOL fSelect );
static void printItem_Poke( DEBUG_BTL_WORK* wk, u16 itemID, STRBUF* buf );
static void printItem_BtlType( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_MsgSpeed( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_WazaEff( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_SubwayMode( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_RecMode( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_RecBuf( DEBUG_BTL_WORK* wk, STRBUF* buf );
static void printItem_DirectStr( DEBUG_BTL_WORK* wk, u16 strID, STRBUF* buf );
static void printClipMark( DEBUG_BTL_WORK* wk );
static void clearClipMark( DEBUG_BTL_WORK* wk );
static void printItem_Flag( DEBUG_BTL_WORK* wk, BOOL flag, STRBUF* buf );
static BOOL mainProc_Setup( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_ChangePage( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_Root( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_MakePokePara( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_Save( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_Load( DEBUG_BTL_WORK* wk, int* seq );
static BOOL mainProc_StartBattle( DEBUG_BTL_WORK* wk, int* seq );
static void SaveRecordStart( DEBUG_BTL_WORK* wk, const BATTLE_SETUP_PARAM* setupParam );
static BOOL SaveRecordWait( DEBUG_BTL_WORK* wk, u8 bufID );
static void LoadRecord( DEBUG_BTL_WORK* wk, u8 bufID, BATTLE_SETUP_PARAM* dst );
static void setDebugParams( const DEBUG_BTL_SAVEDATA* save, BATTLE_SETUP_PARAM* setup );
static void printPartyInfo( POKEPARTY* party );
static void cutoff_wildParty( POKEPARTY* party, BtlRule rule );
static void* testBeaconGetFunc( void* pWork );
static int testBeaconGetSizeFunc( void* pWork );
static BOOL testBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo );
static void comm_dummy_callback(void* pWork);
static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle );
static void btlAutoConnectCallback( void* pWork );

/*--------------------------------------------------------------------------*/
/* Menu Table                                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* Proc Table                                                               */
/*--------------------------------------------------------------------------*/
const GFL_PROC_DATA   DebugFightProcData = {
  DebugFightProcInit,
  DebugFightProcMain,
  DebugFightProcQuit,
};


//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugFightProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  DEBUG_BTL_WORK* wk;

  // �|�P�p���A�Z�[�u�̈�T�C�Y���z����z������ASSERT�Ŏ~�߂�
  GF_ASSERT_MSG( sizeof(DEBUG_BTL_SAVEDATA) <= SAVEAREA_SIZE, "SaveAreaSize=%d bytes", sizeof(DEBUG_BTL_SAVEDATA) );
  {
    u32 pp_size = POKETOOL_GetWorkSize();
    GF_ASSERT_MSG( pp_size <= POKEPARA_SIZE, "PPSize=%d bytes", pp_size );
  }

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTL_DEBUG_SYS,    0x11000 );
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTL_DEBUG_VIEW,   0xb0000 );

  BattleRec_Init(HEAPID_BTL_DEBUG_SYS );

  wk = GFL_PROC_AllocWork( proc, sizeof(DEBUG_BTL_WORK), HEAPID_BTL_DEBUG_SYS );
  GFL_STD_MemClear( wk, sizeof(DEBUG_BTL_WORK) );

  wk->heapID = HEAPID_BTL_DEBUG_SYS;
  wk->gameData = pwk;
  wk->partyPlayer = PokeParty_AllocPartyWork( HEAPID_BTL_DEBUG_SYS );
  wk->partyEnemy1 = PokeParty_AllocPartyWork( HEAPID_BTL_DEBUG_SYS );
  wk->partyFriend = PokeParty_AllocPartyWork( HEAPID_BTL_DEBUG_SYS );
  wk->partyEnemy2 = PokeParty_AllocPartyWork( HEAPID_BTL_DEBUG_SYS );
  wk->clipPoke = NULL;

  initGraphicSystems( HEAPID_BTL_DEBUG_VIEW );
  createTemporaryModules( wk, HEAPID_BTL_DEBUG_VIEW );

  savework_Init( &wk->saveData );
  wk->pageNum = 0;
  wk->selectItem = SELITEM_POKE_SELF_1;

  setMainProc( wk, mainProc_Setup );

  return GFL_PROC_RES_FINISH;
}
//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugFightProcQuit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  DEBUG_BTL_WORK* wk = mywk;

  deleteTemporaryModules( wk );
  quitGraphicSystems();
  BattleRec_Exit();

  GFL_HEAP_DeleteHeap( HEAPID_BTL_DEBUG_VIEW );
  GFL_HEAP_DeleteHeap( HEAPID_BTL_DEBUG_SYS );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT DebugFightProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  DEBUG_BTL_WORK* wk = mywk;

  if( wk->mainProc(wk, &wk->mainSeq) ){
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


//----------------------------------------------------------------------------------
/**
 * ��ʕ`��p�V�X�e��������
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void initGraphicSystems( HEAPID heapID )
{
  static const GFL_DISP_VRAM vramBank = {
    GX_VRAM_BG_128_A,           // ���C��2D�G���W����BG
    GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
    GX_VRAM_SUB_BG_128_C,       // �T�u2D�G���W����BG
    GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
    GX_VRAM_OBJ_128_B,          // ���C��2D�G���W����OBJ
    GX_VRAM_OBJEXTPLTT_0_F,     // ���C��2D�G���W����OBJ�g���p���b�g
    GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
    GX_VRAM_TEX_NONE,           // �e�N�X�`���C���[�W�X���b�g
    GX_VRAM_TEXPLTT_0123_E,     // �e�N�X�`���p���b�g�X���b�g
    GX_OBJVRAMMODE_CHAR_1D_128K,// ���C��OBJ�}�b�s���O���[�h
    GX_OBJVRAMMODE_CHAR_1D_32K, // �T�uOBJ�}�b�s���O���[�h
  };

  GFL_DISP_SetBank( &vramBank );

  // �e����ʃ��W�X�^������
  G2_BlendNone();
  G2S_BlendNone();

  // �㉺��ʐݒ�
  GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

  GFL_BG_Init( heapID );
  GFL_BMPWIN_Init( heapID );
  GFL_FONTSYS_Init();

  //�a�f���[�h�ݒ�
  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }

  // �ʃt���[���ݒ�
  {
    static const GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5800, GX_BG_CHARBASE_0x10000, 0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    /*
    static const GFL_BG_BGCNT_HEADER bgcntText2 = {
      0, 0, 0x800, 0,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x4000, GX_BG_CHARBASE_0x18000, 0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    */

    GFL_BG_SetBGControl( GFL_BG_FRAME0_M,   &bgcntText,   GFL_BG_MODE_TEXT );
    GFL_BG_SetBGControl( GFL_BG_FRAME0_S,   &bgcntText,   GFL_BG_MODE_TEXT );

//    GFL_BG_SetBGControl( GFL_BG_FRAME1_M,   &bgcntText2,   GFL_BG_MODE_TEXT );

    GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

    GFL_BG_SetVisible( GFL_BG_FRAME0_S,   VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );

//    GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0x22, wk->heapID );
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0, heapID );
//    void GFL_BG_FillScreen( u8 frmnum, u16 dat, u8 px, u8 py, u8 sx, u8 sy, u8 mode )
    GFL_BG_FillCharacter( GFL_BG_FRAME0_M, 0xff, 1, 0 );
    GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );

    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, 0, 0, heapID );
    GFL_BG_FillCharacter( GFL_BG_FRAME0_S, 0xff, 1, 0 );
    GFL_BG_FillScreen( GFL_BG_FRAME0_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );
  }
}
//----------------------------------------------------------------------------------
/**
 * ��ʕ`��p�V�X�e���j��
 */
//----------------------------------------------------------------------------------
static void quitGraphicSystems( void )
{
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}
//----------------------------------------------------------------------------------
/**
 * �ꎞ�I���W���[���i�o�g�����s���͏����Ă���j���쐬�E�\�z
 *
 * @param   wk
 * @param   heapID
 */
//----------------------------------------------------------------------------------
static void createTemporaryModules( DEBUG_BTL_WORK* wk, HEAPID heapID )
{
  wk->win = GFL_BMPWIN_Create( GFL_BG_FRAME0_S, 0, 0, 32, 24, 0, GFL_BMP_CHRAREA_GET_F );
  wk->bmp = GFL_BMPWIN_GetBmp( wk->win );
  wk->mm  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debug_fight_dat, heapID );
  wk->mmMonsName  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat, heapID );
  wk->strbuf = GFL_STR_CreateBuffer( 1024, heapID );

  GFL_BMP_Clear( wk->bmp, 0xff );

  GFL_HEAP_CheckHeapSafe( heapID );
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT,
    NARC_font_small_gftr,
    GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

  wk->printQue = PRINTSYS_QUE_Create( heapID );
  PRINT_UTIL_Setup( wk->printUtil, wk->win );
}
//----------------------------------------------------------------------------------
/**
 * �ꎞ�I���W���[���i�o�g�����s���͏����Ă���j��j��
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void deleteTemporaryModules( DEBUG_BTL_WORK* wk )
{
  PRINTSYS_QUE_Delete( wk->printQue );
  GFL_FONT_Delete( wk->fontHandle );

  GFL_STR_DeleteBuffer( wk->strbuf );
  GFL_MSG_Delete( wk->mmMonsName );
  GFL_MSG_Delete( wk->mm );
  GFL_BMPWIN_Delete( wk->win );
}
//----------------------------------------------------------------------------------
/**
 * �V�[���J�ڑO�̏����i�`��p���\�[�X�E��������������j
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void changeScene_start( DEBUG_BTL_WORK* wk )
{
  deleteTemporaryModules( wk );
  quitGraphicSystems();
  GFL_HEAP_DeleteHeap( HEAPID_BTL_DEBUG_VIEW );
}
//----------------------------------------------------------------------------------
/**
 * �V�[�����A��̏����i�`��p���\�[�X�E�����������Ċm�ہj
 *
 * @param   wk
 */
//----------------------------------------------------------------------------------
static void changeScene_recover( DEBUG_BTL_WORK* wk )
{
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BTL_DEBUG_VIEW,  0xb0000 );
  initGraphicSystems( HEAPID_BTL_DEBUG_VIEW );
  createTemporaryModules( wk, HEAPID_BTL_DEBUG_VIEW );
}

//----------------------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^������
 *
 * @param   saveData
 */
//----------------------------------------------------------------------------------
static void savework_Init( DEBUG_BTL_SAVEDATA* saveData )
{
  u32 ppSize = POKETOOL_GetWorkSize();
  u32 i;

  POKEMON_PARAM* pp;

  GFL_STD_MemClear( saveData, sizeof(DEBUG_BTL_SAVEDATA) );

  saveData->btlType  = BTLTYPE_SINGLE_WILD;
  saveData->weather  = BTL_WEATHER_NONE;
  saveData->msgSpeed = MSGSPEED_FAST;
  saveData->fWazaEff = 0;
  saveData->fSubway = 0;

  for(i=0; i<POKEPARA_MAX; ++i){
    pp = savework_GetPokeParaArea( saveData, i );
    PP_Clear( pp );
  }

  pp = savework_GetPokeParaArea( saveData, POKEIDX_SELF_1 );
  PP_Setup( pp, MONSNO_GORIDARUMA, 5, 0 );

  pp = savework_GetPokeParaArea( saveData, POKEIDX_ENEMY1_1 );
  PP_Setup( pp, MONSNO_GORIDARUMA, 5, 0 );

  pp = savework_GetPokeParaArea( saveData, POKEIDX_FRIEND_1 );
  PP_Setup( pp, MONSNO_MAMANBOU, 5, 0 );

  pp = savework_GetPokeParaArea( saveData, POKEIDX_ENEMY2_1 );
  PP_Setup( pp, MONSNO_MAMANBOU, 5, 0 );

}

static POKEMON_PARAM* savework_GetPokeParaArea( DEBUG_BTL_SAVEDATA* saveData, u32 pokeIdx )
{
  GF_ASSERT(pokeIdx < POKEPARA_MAX);
  {
    u32 ppSize = POKETOOL_GetWorkSize();
    return (POKEMON_PARAM*)&saveData->pokeParaArea[ pokeIdx*ppSize ];
  }
}
static void savework_SetParty( DEBUG_BTL_SAVEDATA* saveData, DEBUG_BTL_WORK* wk )
{
  POKEMON_PARAM* pp;
  u32 i, max;

  PokeParty_InitWork( wk->partyPlayer );
  PokeParty_InitWork( wk->partyFriend );
  PokeParty_InitWork( wk->partyEnemy1 );
  PokeParty_InitWork( wk->partyEnemy2 );

  for(i=POKEIDX_SELF_1; i<=POKEIDX_SELF_6; ++i)
  {
    pp = savework_GetPokeParaArea( saveData, i );
    if( PP_Get(pp, ID_PARA_monsno, NULL) ){
      PokeParty_Add( wk->partyPlayer, pp );
    }
  }
  for(i=POKEIDX_FRIEND_1; i<=POKEIDX_FRIEND_6; ++i)
  {
    pp = savework_GetPokeParaArea( saveData, i );
    if( PP_Get(pp, ID_PARA_monsno, NULL) ){
      PokeParty_Add( wk->partyFriend, pp );
    }
  }

  for(i=POKEIDX_ENEMY1_1; i<=POKEIDX_ENEMY1_6; ++i)
  {
    pp = savework_GetPokeParaArea( saveData, i );
    if( PP_Get(pp, ID_PARA_monsno, NULL) ){
      u32 monsno = PP_Get(pp, ID_PARA_monsno, NULL);
      PokeParty_Add( wk->partyEnemy1, pp );
    }
  }
  for(i=POKEIDX_ENEMY2_1; i<=POKEIDX_ENEMY2_6; ++i)
  {
    pp = savework_GetPokeParaArea( saveData, i );
    if( PP_Get(pp, ID_PARA_monsno, NULL) ){
      PokeParty_Add( wk->partyEnemy2, pp );
    }
  }


}

//----------------------------------------------------------------------------------
/**
 * ���C���֐������ւ�
 *
 * @param   wk
 * @param   nextProc
 */
//----------------------------------------------------------------------------------
static void setMainProc( DEBUG_BTL_WORK* wk, pMainProc nextProc )
{
  wk->mainProc = nextProc;
  wk->mainSeq  = 0;
}

//----------------------------------------------------------------------------------
/**
 * �I���A�C�e��ID���|�P�����p�����[�^�����`�F�b�N
 *
 * @param   itemID
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static inline BOOL selItem_IsPoke( u16 itemID )
{
  return ((itemID >= SELITEM_POKE_SELF_1) && (itemID <= SELITEM_POKE_ENEMY2_6) );
}
//----------------------------------------------------------------------------------
/**
 * �I���A�C�e���v�f���C���N�������g
 *
 * @param   wk
 * @param   itemID
 */
//----------------------------------------------------------------------------------
static void selItem_Increment( DEBUG_BTL_WORK* wk, u16 itemID, int incValue )
{
  DEBUG_BTL_SAVEDATA* save = &wk->saveData;

  switch( itemID ){
  case SELITEM_BTL_TYPE:
    save->btlType = loopValue( save->btlType+incValue, 0, BTLTYPE_MAX-1 );
    break;

  case SELITEM_MSGSPEED:
    save->msgSpeed = loopValue( save->msgSpeed+incValue, 0, MSGSPEED_FAST_EX );
    break;

  case SELITEM_WAZAEFF:
    save->fWazaEff ^= 1;
    break;

  case SELITEM_SUBWAYMODE:
    save->fSubway ^= 1;
    break;

  case SELITEM_REC_MODE:
    save->recMode = loopValue( save->recMode+incValue, 0, DBF_RECMODE_MAX-1 );
    break;

  case SELITEM_REC_BUF:
    save->recBufID = loopValue( save->recBufID+incValue, 0, 3 );
    break;


  case SELITEM_MUST_TUIKA:
    save->fMustTuika ^= 1;
    break;
  case SELITEM_MUST_TOKU:
    save->fMustToku ^= 1;
    break;
  case SELITEM_MUST_ITEM:
    save->fMustItem ^= 1;
    break;
  case SELITEM_MUST_CRITICAL:
    save->fMustCritical ^= 1;
    break;
  case SELITEM_HP_CONST:
    save->fHPConst ^= 1;
    break;
  case SELITEM_PP_CONST:
    save->fPPConst ^= 1;
    break;
  }
}
//----------------------------------------------------------------------------------
/**
 * ���[�v����ݒ�l�v�Z
 *
 * @param   value
 * @param   min
 * @param   max
 *
 * @retval  int
 */
//----------------------------------------------------------------------------------
static int loopValue( int value, int min, int max )
{
  if( min > max ){
    int tmp = min;
    min = max;
    max = tmp;
  }

  if( value > max ){
    value = min;
  }
  else if( value < min ){
    value = max;
  }
  return value;
}

//----------------------------------------------------------------------------------
/**
 * �ʐM���s���K�v�̂���o�g���^�C�v������
 *
 * @param   type
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL btltype_IsComm( BtlType type )
{
  return BtlTypeParams[ type ].commFlag;
}
//----------------------------------------------------------------------------------
/**
 * �}���`���[�h������
 *
 * @param   type
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL btltype_IsMulti( BtlType type )
{
  return ( type == BTLTYPE_DOUBLE_COMM_MULTI );
}
static BOOL btltype_IsWild( BtlType type )
{
  return BtlTypeParams[ type ].wildFlag;
}
static BtlRule btltype_GetRule( BtlType type )
{
  return BtlTypeParams[ type ].rule;
}

//----------------------------------------------------------------------------------
/**
 * �I���A�C�e���`��
 *
 * @param   wk
 * @param   itemID
 * @param   fSelect   �I�𒆃t���O
 */
//----------------------------------------------------------------------------------
static void PrintItem( DEBUG_BTL_WORK* wk, u16 itemID, BOOL fSelect )
{
  const ITEM_LAYOUT*  itemTable;
  u32 i, elems;

  itemTable = LayoutTable[ wk->pageNum ].itemLayout;
  elems = LayoutTable[ wk->pageNum ].itemElems;

  for(i=0; i<elems; ++i)
  {
    if( itemTable[i].itemID == itemID )
    {
      u16 x = itemTable[i].x;
      u16 y = itemTable[i].y;

      if( fSelect ){
        GFL_FONTSYS_SetColor( 3, 4, 0x0f );
      }else{
        GFL_FONTSYS_SetColor( 1, 2, 0x0f );
      }

      if( wk->prevItemStrWidth[itemID] ){
        GFL_BMP_Fill( wk->bmp, x, y, wk->prevItemStrWidth[itemID], GFL_FONT_GetLineHeight(wk->fontHandle), 0xff );
      }

      if( selItem_IsPoke(itemID) ){
        printItem_Poke( wk, itemID, wk->strbuf );
      }else{
        switch( itemID ){
        case SELITEM_BTL_TYPE:    printItem_BtlType( wk, wk->strbuf ); break;
        case SELITEM_MSGSPEED:    printItem_MsgSpeed( wk, wk->strbuf ); break;
        case SELITEM_WAZAEFF:     printItem_WazaEff( wk, wk->strbuf ); break;
        case SELITEM_SUBWAYMODE:  printItem_SubwayMode( wk, wk->strbuf ); break;
        case SELITEM_REC_MODE:    printItem_RecMode( wk, wk->strbuf ); break;
        case SELITEM_REC_BUF:     printItem_RecBuf( wk, wk->strbuf ); break;
        case SELITEM_SAVE:        printItem_DirectStr( wk, DBGF_ITEM_SAVE, wk->strbuf ); break;
        case SELITEM_LOAD:        printItem_DirectStr( wk, DBGF_ITEM_LOAD, wk->strbuf ); break;

        case SELITEM_MUST_TUIKA:    printItem_Flag( wk, wk->saveData.fMustTuika,    wk->strbuf ); break;
        case SELITEM_MUST_TOKU:     printItem_Flag( wk, wk->saveData.fMustToku,     wk->strbuf ); break;
        case SELITEM_MUST_ITEM:     printItem_Flag( wk, wk->saveData.fMustItem,     wk->strbuf ); break;
        case SELITEM_MUST_CRITICAL: printItem_Flag( wk, wk->saveData.fMustCritical, wk->strbuf ); break;
        case SELITEM_HP_CONST:      printItem_Flag( wk, wk->saveData.fHPConst, wk->strbuf ); break;
        case SELITEM_PP_CONST:      printItem_Flag( wk, wk->saveData.fPPConst, wk->strbuf ); break;

        default:
          GFL_STR_ClearBuffer( wk->strbuf );
          break;
        }
      }
      PRINTSYS_Print( wk->bmp, x, y, wk->strbuf, wk->fontHandle );
      wk->prevItemStrWidth[ itemID ] = PRINTSYS_GetStrWidth( wk->strbuf, wk->fontHandle, 0 );
      break;
    }
  }
}
static void printItem_Poke( DEBUG_BTL_WORK* wk, u16 itemID, STRBUF* buf )
{
  POKEMON_PARAM* pp;
  u16 monsno;
  u16 pokeIdx = itemID - SELITEM_POKE_SELF_1;

  pp = savework_GetPokeParaArea( &wk->saveData, pokeIdx );
  monsno = PP_Get( pp, ID_PARA_monsno, NULL );
  GFL_MSG_GetString( wk->mmMonsName, monsno, buf );
}
static void printItem_BtlType( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_TYPE01+wk->saveData.btlType, buf );
}
static void printItem_MsgSpeed( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_MSGSPD_SLOW+wk->saveData.msgSpeed, buf );
}
static void printItem_WazaEff( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_WAZAEFF_ON+wk->saveData.fWazaEff, buf );
}
static void printItem_SubwayMode( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_SUBWAY_OFF+wk->saveData.fSubway, buf );
}
static void printItem_RecMode( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_REC_OFF+wk->saveData.recMode, buf );
}
static void printItem_RecBuf( DEBUG_BTL_WORK* wk, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_RECBUF_0+wk->saveData.recBufID, buf );
}

static void printItem_DirectStr( DEBUG_BTL_WORK* wk, u16 strID, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, strID, buf );
}
static void printClipMark( DEBUG_BTL_WORK* wk )
{
  GFL_STR_ClearBuffer( wk->strbuf );
  GFL_STR_AddCode( wk->strbuf, 0xff23 ); // '�b'
  PRINTSYS_Print( wk->bmp, LAYOUT_CRIPMARK_OX, LAYOUT_CRIPMARK_OY, wk->strbuf, wk->fontHandle );
  GFL_BMPWIN_TransVramCharacter( wk->win );
}
static void clearClipMark( DEBUG_BTL_WORK* wk )
{
  GFL_BMP_Fill( wk->bmp, LAYOUT_CRIPMARK_OX, LAYOUT_CRIPMARK_OY,
              LAYOUT_CRIPMARK_WIDTH, LAYOUT_CRIPMARK_HEIGHT, 0xff );
  GFL_BMPWIN_TransVramCharacter( wk->win );
}
static void printItem_Flag( DEBUG_BTL_WORK* wk, BOOL flag, STRBUF* buf )
{
  GFL_MSG_GetString( wk->mm, DBGF_ITEM_SUBWAY_OFF+flag, buf );
}



//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F��ʏ����\�z
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_Setup( DEBUG_BTL_WORK* wk, int* seq )
{
  const LABEL_LAYOUT* labelTable;
  const ITEM_LAYOUT*  itemTable;
  u32 i, elems;

  GFL_FONTSYS_SetColor( 1, 2, 0x0f );

  labelTable = LayoutTable[ wk->pageNum ].labelLayout;
  elems = LayoutTable[ wk->pageNum ].labelElems;
  for(i=0; i<elems; ++i)
  {
    GFL_MSG_GetString( wk->mm, labelTable[i].strID, wk->strbuf );
    PRINTSYS_Print( wk->bmp, labelTable[i].x, labelTable[i].y, wk->strbuf, wk->fontHandle );
  }

  itemTable = LayoutTable[ wk->pageNum ].itemLayout;
  elems = LayoutTable[ wk->pageNum ].itemElems;
  for(i=0; i<elems; ++i)
  {
    PrintItem( wk, itemTable[i].itemID, (wk->selectItem == itemTable[i].itemID));
  }

  GFL_BMPWIN_MakeScreen( wk->win );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->win) );
  GFL_BMPWIN_TransVramCharacter( wk->win );

  setMainProc( wk, mainProc_Root );

  GX_SetMasterBrightness( 0 );
  GXS_SetMasterBrightness( 0 );


  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F�y�[�W�؂�ւ�
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_ChangePage( DEBUG_BTL_WORK* wk, int* seq )
{
  GFL_BMP_Clear( wk->bmp, 0xff );
  wk->selectItem = LayoutTable[ wk->pageNum ].itemLayout[0].itemID;
  setMainProc( wk, mainProc_Setup );
  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F���[�g
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_Root( DEBUG_BTL_WORK* wk, int* seq )
{
  u16 key = GFL_UI_KEY_GetTrg();
  if( key & PAD_PLUS_KEY_MASK )
  {
    static const struct {
      u8  currentItem;
      u8  itemU;
      u8  itemD;
      u8  itemR;
      u8  itemL;
    }nextItemTbl[] = {
      { SELITEM_POKE_SELF_1,   SELITEM_SAVE,          SELITEM_POKE_SELF_2,   SELITEM_POKE_ENEMY1_1, SELITEM_POKE_ENEMY2_1 },
      { SELITEM_POKE_SELF_2,   SELITEM_POKE_SELF_1,   SELITEM_POKE_SELF_3,   SELITEM_POKE_ENEMY1_2, SELITEM_POKE_ENEMY2_2 },
      { SELITEM_POKE_SELF_3,   SELITEM_POKE_SELF_2,   SELITEM_POKE_SELF_4,   SELITEM_POKE_ENEMY1_3, SELITEM_POKE_ENEMY2_3 },
      { SELITEM_POKE_SELF_4,   SELITEM_POKE_SELF_3,   SELITEM_POKE_SELF_5,   SELITEM_POKE_ENEMY1_4, SELITEM_POKE_ENEMY2_4 },
      { SELITEM_POKE_SELF_5,   SELITEM_POKE_SELF_4,   SELITEM_POKE_SELF_6,   SELITEM_POKE_ENEMY1_5, SELITEM_POKE_ENEMY2_5 },
      { SELITEM_POKE_SELF_6,   SELITEM_POKE_SELF_5,   SELITEM_BTL_TYPE,      SELITEM_POKE_ENEMY1_6, SELITEM_POKE_ENEMY2_6 },
      { SELITEM_POKE_ENEMY1_1, SELITEM_SAVE,          SELITEM_POKE_ENEMY1_2, SELITEM_POKE_FRIEND_1, SELITEM_POKE_SELF_1   },
      { SELITEM_POKE_ENEMY1_2, SELITEM_POKE_ENEMY1_1, SELITEM_POKE_ENEMY1_3, SELITEM_POKE_FRIEND_2, SELITEM_POKE_SELF_2   },
      { SELITEM_POKE_ENEMY1_3, SELITEM_POKE_ENEMY1_2, SELITEM_POKE_ENEMY1_4, SELITEM_POKE_FRIEND_3, SELITEM_POKE_SELF_3   },
      { SELITEM_POKE_ENEMY1_4, SELITEM_POKE_ENEMY1_3, SELITEM_POKE_ENEMY1_5, SELITEM_POKE_FRIEND_4, SELITEM_POKE_SELF_4   },
      { SELITEM_POKE_ENEMY1_5, SELITEM_POKE_ENEMY1_4, SELITEM_POKE_ENEMY1_6, SELITEM_POKE_FRIEND_5, SELITEM_POKE_SELF_5   },
      { SELITEM_POKE_ENEMY1_6, SELITEM_POKE_ENEMY1_5, SELITEM_BTL_TYPE,      SELITEM_POKE_FRIEND_6, SELITEM_POKE_SELF_6   },
      { SELITEM_POKE_FRIEND_1, SELITEM_SAVE,          SELITEM_POKE_FRIEND_2, SELITEM_POKE_ENEMY2_1, SELITEM_POKE_ENEMY1_1 },
      { SELITEM_POKE_FRIEND_2, SELITEM_POKE_FRIEND_1, SELITEM_POKE_FRIEND_3, SELITEM_POKE_ENEMY2_2, SELITEM_POKE_ENEMY1_2 },
      { SELITEM_POKE_FRIEND_3, SELITEM_POKE_FRIEND_2, SELITEM_POKE_FRIEND_4, SELITEM_POKE_ENEMY2_3, SELITEM_POKE_ENEMY1_3 },
      { SELITEM_POKE_FRIEND_4, SELITEM_POKE_FRIEND_3, SELITEM_POKE_FRIEND_5, SELITEM_POKE_ENEMY2_4, SELITEM_POKE_ENEMY1_4 },
      { SELITEM_POKE_FRIEND_5, SELITEM_POKE_FRIEND_4, SELITEM_POKE_FRIEND_6, SELITEM_POKE_ENEMY2_5, SELITEM_POKE_ENEMY1_5 },
      { SELITEM_POKE_FRIEND_6, SELITEM_POKE_FRIEND_5, SELITEM_MSGSPEED,      SELITEM_POKE_ENEMY2_6, SELITEM_POKE_ENEMY1_6 },
      { SELITEM_POKE_ENEMY2_1, SELITEM_SAVE,          SELITEM_POKE_ENEMY2_2, SELITEM_POKE_SELF_1,   SELITEM_POKE_FRIEND_1 },
      { SELITEM_POKE_ENEMY2_2, SELITEM_POKE_ENEMY2_1, SELITEM_POKE_ENEMY2_3, SELITEM_POKE_SELF_2,   SELITEM_POKE_FRIEND_2 },
      { SELITEM_POKE_ENEMY2_3, SELITEM_POKE_ENEMY2_2, SELITEM_POKE_ENEMY2_4, SELITEM_POKE_SELF_3,   SELITEM_POKE_FRIEND_3 },
      { SELITEM_POKE_ENEMY2_4, SELITEM_POKE_ENEMY2_3, SELITEM_POKE_ENEMY2_5, SELITEM_POKE_SELF_4,   SELITEM_POKE_FRIEND_4 },
      { SELITEM_POKE_ENEMY2_5, SELITEM_POKE_ENEMY2_4, SELITEM_POKE_ENEMY2_6, SELITEM_POKE_SELF_5,   SELITEM_POKE_FRIEND_5 },
      { SELITEM_POKE_ENEMY2_6, SELITEM_POKE_ENEMY2_5, SELITEM_MSGSPEED,      SELITEM_POKE_SELF_6,   SELITEM_POKE_FRIEND_6 },
      { SELITEM_BTL_TYPE,      SELITEM_POKE_SELF_6,   SELITEM_SAVE,          SELITEM_MSGSPEED,      SELITEM_MSGSPEED,     },
      { SELITEM_MSGSPEED,      SELITEM_POKE_ENEMY2_6, SELITEM_WAZAEFF,       SELITEM_BTL_TYPE,      SELITEM_BTL_TYPE      },
      { SELITEM_WAZAEFF,       SELITEM_MSGSPEED,      SELITEM_SUBWAYMODE,    SELITEM_BTL_TYPE,      SELITEM_BTL_TYPE      },
      { SELITEM_SUBWAYMODE,    SELITEM_WAZAEFF,       SELITEM_REC_MODE,      SELITEM_LOAD,          SELITEM_SAVE          },
      { SELITEM_REC_MODE,      SELITEM_SUBWAYMODE,    SELITEM_POKE_ENEMY2_1, SELITEM_REC_BUF,       SELITEM_SAVE          },
      { SELITEM_REC_BUF,       SELITEM_SUBWAYMODE,    SELITEM_POKE_ENEMY2_1, SELITEM_LOAD,          SELITEM_REC_MODE      },
      { SELITEM_SAVE,          SELITEM_BTL_TYPE,      SELITEM_POKE_SELF_1,   SELITEM_REC_MODE,      SELITEM_LOAD          },
      { SELITEM_LOAD,          SELITEM_BTL_TYPE,      SELITEM_POKE_SELF_1,   SELITEM_SAVE,          SELITEM_REC_BUF       },
  /*    CurrentItem,           Up-Item,               Down-Item,             Right-Item,            Left-Item */
      { SELITEM_MUST_TUIKA,    SELITEM_MUST_CRITICAL, SELITEM_MUST_TOKU,     SELITEM_NULL,          SELITEM_NULL          },
      { SELITEM_MUST_TOKU,     SELITEM_MUST_TUIKA,    SELITEM_MUST_ITEM,     SELITEM_NULL,          SELITEM_NULL          },
      { SELITEM_MUST_ITEM,     SELITEM_MUST_TOKU,     SELITEM_MUST_CRITICAL, SELITEM_NULL,          SELITEM_NULL          },
      { SELITEM_MUST_CRITICAL, SELITEM_MUST_ITEM,     SELITEM_HP_CONST,      SELITEM_NULL,          SELITEM_NULL          },
      { SELITEM_HP_CONST,      SELITEM_MUST_CRITICAL, SELITEM_PP_CONST,      SELITEM_NULL,          SELITEM_NULL          },
      { SELITEM_PP_CONST,      SELITEM_HP_CONST,      SELITEM_MUST_TUIKA,    SELITEM_NULL,          SELITEM_NULL          },
  /*    CurrentItem,           Up-Item,               Down-Item,             Right-Item,            Left-Item */
    };

    u32 nextItem = SELITEM_NULL, i;
    for(i=0; i<NELEMS(nextItemTbl); ++i)
    {
      if( nextItemTbl[i].currentItem == wk->selectItem )
      {
        if( key & PAD_KEY_UP    ){ nextItem = nextItemTbl[i].itemU; break; }
        if( key & PAD_KEY_DOWN  ){ nextItem = nextItemTbl[i].itemD; break; }
        if( key & PAD_KEY_RIGHT ){ nextItem = nextItemTbl[i].itemR; break; }
        if( key & PAD_KEY_LEFT  ){ nextItem = nextItemTbl[i].itemL; break; }
        break;
      }
    }
    if( nextItem != SELITEM_NULL )
    {
      PrintItem( wk, wk->selectItem, FALSE );
      PrintItem( wk, nextItem, TRUE );
      GFL_BMPWIN_TransVramCharacter( wk->win );
      wk->selectItem = nextItem;
    }
    return FALSE;
  }

  // �|�P�I�𒆂ɂx�{�^���ŊȈՃR�s�y�@�\
  if( key & PAD_BUTTON_Y && (wk->selectItem <= SELITEM_POKE_ENEMY2_6) ){
    // �R�s�[
    if( wk->clipPoke == NULL ){
      wk->clipPoke = savework_GetPokeParaArea( &wk->saveData, wk->selectItem );
      if( wk->clipPoke != NULL )
      {
        printClipMark( wk );
      }
    }
    // �y�[�X�g
    else
    {
      POKEMON_PARAM* dst = savework_GetPokeParaArea( &wk->saveData, wk->selectItem );
      if( dst != wk->clipPoke )
      {
        GFL_STD_MemCopy32( wk->clipPoke, dst, POKETOOL_GetWorkSize() );
        PrintItem( wk, wk->selectItem, TRUE );
        clearClipMark( wk );
        wk->clipPoke = NULL;
      }
    }
  }

  if( key & PAD_BUTTON_L ){
    wk->pageNum = loopValue( wk->pageNum-1, 0, (PAGE_MAX-1) );
    setMainProc( wk, mainProc_ChangePage );
  }
  if( key & PAD_BUTTON_R ){
    wk->pageNum = loopValue( wk->pageNum+1, 0, (PAGE_MAX-1) );
    setMainProc( wk, mainProc_ChangePage );
  }

  if( key & PAD_BUTTON_A )
  {
    if( selItem_IsPoke(wk->selectItem) ){
      setMainProc( wk, mainProc_MakePokePara );
    }
    else if( wk->selectItem == SELITEM_SAVE ){
      setMainProc( wk, mainProc_Save );
    }
    else if( wk->selectItem == SELITEM_LOAD ){
      setMainProc( wk, mainProc_Load );
    }
    else{
      selItem_Increment( wk, wk->selectItem, 1 );
      PrintItem( wk, wk->selectItem, TRUE );
      GFL_BMPWIN_TransVramCharacter( wk->win );
    }
  }
  else if( key & PAD_BUTTON_B )
  {
    if( !selItem_IsPoke(wk->selectItem) ){
      selItem_Increment( wk, wk->selectItem, -1 );
      PrintItem( wk, wk->selectItem, TRUE );
      GFL_BMPWIN_TransVramCharacter( wk->win );
    }
  }
  else if( key & PAD_BUTTON_SELECT ){
    setMainProc( wk, mainProc_Load );
  }
  else if( key & PAD_BUTTON_START )
  {
    setMainProc( wk, mainProc_StartBattle );
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F�|�P�p���쐬
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_MakePokePara( DEBUG_BTL_WORK* wk, int* seq )
{
FS_EXTERN_OVERLAY(debug_makepoke);

  switch( *seq ){
  case 0:
    changeScene_start( wk );
    (*seq)++;
    break;
  case 1:
    wk->makePokeParam.dst = savework_GetPokeParaArea( &wk->saveData, wk->selectItem-SELITEM_POKE_SELF_1 );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(debug_makepoke), &ProcData_DebugMakePoke, &wk->makePokeParam );
    (*seq)++;
    break;
  case 2:
    changeScene_recover( wk );
    setMainProc( wk, mainProc_Setup );
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F�Z�[�u
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_Save( DEBUG_BTL_WORK* wk, int* seq )
{
  DEBUG_BACKUP_FlashSave(DEBUG_FIGHT_SAVE, &wk->saveData, sizeof(wk->saveData));
  setMainProc( wk, mainProc_Root );
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F���[�h
 */
//----------------------------------------------------------------------------------
static BOOL mainProc_Load( DEBUG_BTL_WORK* wk, int* seq )
{
  DEBUG_BACKUP_FlashLoad(DEBUG_FIGHT_SAVE, &wk->saveData, sizeof(wk->saveData));
  setMainProc( wk, mainProc_Setup );
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ���C���v���Z�X�F�o�g����
 */
//----------------------------------------------------------------------------------
static const GFLNetInitializeStruct NetInitParamNormal;
static const GFLNetInitializeStruct NetInitParamMulti;

static BOOL mainProc_StartBattle( DEBUG_BTL_WORK* wk, int* seq )
{
FS_EXTERN_OVERLAY(battle);

  enum {
    SEQ_INIT = 0,
    SEQ_COMM_START_1,
    SEQ_COMM_START_2,
    SEQ_COMM_START_3,
    SEQ_COMM_START_4,
    SEQ_SETUP_START,
    SEQ_BTL_START,
    SEQ_BTL_RETURN,
    SEQ_BTL_SAVING,
    SEQ_BTL_RELEASE,
  };

  enum {
    SYNC_ID = 2929, // �Ă��Ƃ�
  };

  switch( *seq ){
  case SEQ_INIT:
    {
      CONFIG* config = SaveData_GetConfig( GAMEDATA_GetSaveControlWork(wk->gameData) );
      CONFIG_SetMsgSpeed( config, wk->saveData.msgSpeed );
      CONFIG_SetWazaEffectMode( config, wk->saveData.fWazaEff );
    }
    BATTLE_PARAM_Init( &wk->setupParam );
    savework_SetParty( &wk->saveData, wk );

    changeScene_start( wk );
    if( btltype_IsComm(wk->saveData.btlType) ){
      (*seq) = SEQ_COMM_START_1;
    }else{
      (*seq) = SEQ_SETUP_START;
    }
    break;

  // �ʐM�J�n
  case SEQ_COMM_START_1:
    {
      const GFLNetInitializeStruct* initParam;
      initParam = (btltype_IsMulti(wk->saveData.btlType))? &NetInitParamMulti : &NetInitParamNormal;
      GFL_NET_Init( initParam, comm_dummy_callback, (void*)wk );
      (*seq) = SEQ_COMM_START_2;
    }
    break;
  case SEQ_COMM_START_2:
    if( GFL_NET_IsInit() )
    {
      wk->fNetConnect = FALSE;
      GFL_NET_ChangeoverConnect( btlAutoConnectCallback ); // �����ڑ�
      (*seq) = SEQ_COMM_START_3;
    }
    break;
  case SEQ_COMM_START_3:
    if( wk->fNetConnect ){
      GFL_NET_TimingSyncStart( GFL_NET_HANDLE_GetCurrentHandle(), SYNC_ID );
      (*seq) = SEQ_COMM_START_4;
    }
    break;
  case SEQ_COMM_START_4:
    if( GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), SYNC_ID) ){
      (*seq) = SEQ_SETUP_START;
    }
    break;

  // �o�g���p�����[�^�Z�b�g
  case SEQ_SETUP_START:
    BATTLE_PARAM_Init( &wk->setupParam );
    wk->fPlayerPartyAllocated = FALSE;

    // �쐶
    if( btltype_IsWild(wk->saveData.btlType) )
    {
      BTL_FIELD_SITUATION sit;
      BtlRule rule = btltype_GetRule( wk->saveData.btlType );

      cutoff_wildParty( wk->partyEnemy1, rule );

      BTL_FIELD_SITUATION_Init(&sit);
      BTL_SETUP_Wild( &wk->setupParam, wk->gameData, wk->partyEnemy1, &sit, rule, HEAPID_BTL_DEBUG_SYS );
    }
    // �ʐM�ΐ�
    else if( btltype_IsComm(wk->saveData.btlType) )
    {
      BtlRule rule = btltype_GetRule( wk->saveData.btlType );
      GFL_NETHANDLE* netHandle = GFL_NET_HANDLE_GetCurrentHandle();
      switch( rule ){
      case BTL_RULE_SINGLE:
        BTL_SETUP_Single_Comm( &wk->setupParam, wk->gameData, netHandle, BTL_COMM_DS, HEAPID_BTL_DEBUG_SYS );
        break;
      case BTL_RULE_DOUBLE:
        if( !btltype_IsMulti(wk->saveData.btlType) ){
          BTL_SETUP_Double_Comm( &wk->setupParam, wk->gameData, netHandle, BTL_COMM_DS, HEAPID_BTL_DEBUG_SYS );
        }else{
          BTL_SETUP_Multi_Comm( &wk->setupParam, wk->gameData, netHandle, BTL_COMM_DS,
            GFL_NET_GetNetID(netHandle), HEAPID_BTL_DEBUG_SYS );
        }
        break;
      case BTL_RULE_TRIPLE:
        BTL_SETUP_Triple_Comm( &wk->setupParam, wk->gameData, netHandle, BTL_COMM_DS, HEAPID_BTL_DEBUG_SYS );
        break;
      case BTL_RULE_ROTATION:
        BTL_SETUP_Rotation_Comm( &wk->setupParam, wk->gameData, netHandle, BTL_COMM_DS, HEAPID_BTL_DEBUG_SYS );
        break;
      }
    }
    // VS�Q�[�����g���[�i�[
    else
    {
      BTL_FIELD_SITUATION sit;
      BtlRule rule = btltype_GetRule( wk->saveData.btlType );
      TrainerID  trID = 1 + GFL_STD_MtRand( 50 ); // �Ă��Ɓ[�Ƀ����_����

      BTL_FIELD_SITUATION_Init(&sit);
      switch( rule ){
      case BTL_RULE_SINGLE:
        BTL_SETUP_Single_Trainer( &wk->setupParam, wk->gameData,
          &sit, trID, HEAPID_BTL_DEBUG_SYS );
        BATTLE_PARAM_SetPokeParty( &wk->setupParam, wk->partyEnemy1, BTL_CLIENT_ENEMY1 );
        break;
      case BTL_RULE_DOUBLE:
        BTL_SETUP_Double_Trainer( &wk->setupParam, wk->gameData,
          &sit, trID, HEAPID_BTL_DEBUG_SYS );
        BATTLE_PARAM_SetPokeParty( &wk->setupParam, wk->partyEnemy1, BTL_CLIENT_ENEMY1 );
        break;
      case BTL_RULE_TRIPLE:
        BTL_SETUP_Triple_Trainer( &wk->setupParam, wk->gameData,
          &sit, trID, HEAPID_BTL_DEBUG_SYS );
        BATTLE_PARAM_SetPokeParty( &wk->setupParam, wk->partyEnemy1, BTL_CLIENT_ENEMY1 );
        break;
      case BTL_RULE_ROTATION:
        BTL_SETUP_Rotation_Trainer( &wk->setupParam, wk->gameData,
          &sit, trID, HEAPID_BTL_DEBUG_SYS );
        BATTLE_PARAM_SetPokeParty( &wk->setupParam, wk->partyEnemy1, BTL_CLIENT_ENEMY1 );
        break;
      }
      if( wk->saveData.fSubway ){
        BTL_SETUP_SetSubwayMode( &wk->setupParam );
      }
    }
    BATTLE_PARAM_SetPokeParty( &wk->setupParam, wk->partyPlayer, BTL_CLIENT_PLAYER );
    if( wk->saveData.recMode == DBF_RECMODE_REC ){
      BTL_SETUP_AllocRecBuffer( &wk->setupParam, HEAPID_BTL_DEBUG_SYS );
    }
    else if( wk->saveData.recMode == DBF_RECMODE_PLAY ){
      BTL_SETUP_AllocRecBuffer( &wk->setupParam, HEAPID_BTL_DEBUG_SYS );
      LoadRecord( wk, wk->saveData.recBufID, &wk->setupParam );
      BTL_SETUP_SetRecordPlayMode( &wk->setupParam );
      TAYA_Printf("�o�g���p�����[�^��^��Đ����[�h�ɂ���\n");
    }
    (*seq) = SEQ_BTL_START;
    break;

  case SEQ_BTL_START:
    setDebugParams( &wk->saveData, &wk->setupParam );
    PMSND_PlayBGM( wk->setupParam.musicDefault );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(battle), &BtlProcData, &wk->setupParam );
    (*seq) = SEQ_BTL_RETURN;
    break;

  case SEQ_BTL_RETURN:
    if( wk->saveData.recMode == DBF_RECMODE_REC )
    {
      SaveRecordStart( wk, &(wk->setupParam) );
      (*seq) = SEQ_BTL_SAVING;
    }
    else{
      (*seq) = SEQ_BTL_RELEASE;
    }
    break;

  case SEQ_BTL_SAVING:
    if( SaveRecordWait(wk, wk->saveData.recBufID) ){
      (*seq) = SEQ_BTL_RELEASE;
    }
    break;

  case SEQ_BTL_RELEASE:
    BATTLE_PARAM_Release( &wk->setupParam );
    changeScene_recover( wk );
    PMSND_StopBGM();
    setMainProc( wk, mainProc_Setup );
    break;
  }
  return FALSE;
}

//static void Record_SaveParty_Start( DEBUG_BTL_WORK* wk,

/**
 *  BATTLE_SETUP_PARAM �̘^��f�[�^�Z�[�u�J�n
 */
static void SaveRecordStart( DEBUG_BTL_WORK* wk, const BATTLE_SETUP_PARAM* setupParam )
{
  u8* dataPtr = (u8*)BattleRec_GetOperationBufferPtr();

  *((u32*)dataPtr) = setupParam->recDataSize;
  TAYA_Printf("�^��f�[�^�Z�[�u����:WorkPtr=%p, dataSize=%dbyte, ptr=%p, dump=%08x\n", dataPtr, *dataPtr, dataPtr, *dataPtr);
  dataPtr += sizeof(u32);
  GFL_STD_MemCopy( &setupParam->recRandContext, dataPtr, sizeof(setupParam->recRandContext) );
  dataPtr += sizeof(setupParam->recRandContext);
  GFL_STD_MemCopy( setupParam->recBuffer, dataPtr, setupParam->recDataSize );

  wk->saveCtrl = GAMEDATA_GetSaveControlWork( wk->gameData );
  wk->saveSeq0 = 0;
  wk->saveSeq1 = 0;

  wk->recDataSize = setupParam->recDataSize;
  GFL_STD_MemCopy( setupParam->recBuffer, wk->recBuffer, wk->recDataSize );
  wk->recRand = setupParam->recRandContext;

  // �ۑ����ׂ��f�[�^���_���v�o�͂��Ă���i���f�[�^�z��쐬�̂��߁j
  {
    u32 i;
    u8* rcp;

    TAYA_Printf("**** Kari Rec Data ****\n");
    for(i=0; i<setupParam->recDataSize; ++i){
      TAYA_Printf("0x%02x,", setupParam->recBuffer[i]);
      if((i+1)%16 == 0){
        TAYA_Printf("\n");
      }
    }
    TAYA_Printf("\n");

    TAYA_Printf("**** Kari RAND Data ****\n");
    rcp = (u8*)(&setupParam->recRandContext);
    for(i=0; i<sizeof(setupParam->recRandContext); ++i){
      TAYA_Printf("0x%02x,", rcp[i]);
      if((i+1)%16 == 0){
        TAYA_Printf("\n");
      }
    }
    TAYA_Printf("\n");
  }

}
/**
 *  BATTLE_SETUP_PARAM �̘^��f�[�^�Z�[�u�I���҂�
 */
static BOOL SaveRecordWait( DEBUG_BTL_WORK* wk, u8 bufID )
{
  SAVE_RESULT result;
  result = BattleRec_Save(wk->saveCtrl, wk->heapID, 0, 0, bufID, &wk->saveSeq0, &wk->saveSeq1);
  switch( result ){
  case SAVE_RESULT_OK:
    {
      u32* dataPtr = (u32*)BattleRec_GetOperationBufferPtr();
      TAYA_Printf("�^��f�[�^����ɃZ�[�u���� (bufID=%d, dataPtr=%p, dump=%08x)\n", bufID, dataPtr, *dataPtr);
    }
    return TRUE;
  case SAVE_RESULT_NG:
    {
      u32* dataPtr = (u32*)BattleRec_GetOperationBufferPtr();
      TAYA_Printf("�^��f�[�^�Z�[�u���s ... (bufID=%d, dataPtr=%p, dump=%08x)\n", bufID, dataPtr, *dataPtr);
    }
    return TRUE;

  default:
    return FALSE;
  }
}
/**
 *  BATTLE_SETUP_PARAM �ɘ^��f�[�^�����[�h���ăZ�b�e�B���O
 */
static void LoadRecord( DEBUG_BTL_WORK* wk, u8 bufID, BATTLE_SETUP_PARAM* dst )
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork( wk->gameData );
  LOAD_RESULT result;

  TAYA_Printf("���[�h����\n");
  BattleRec_Load( sv, wk->heapID, &result, bufID );
  if( result == LOAD_RESULT_OK )
  {
    u8* dataPtr = (u8*)BattleRec_GetOperationBufferPtr();


    dst->recDataSize = *((u32*)dataPtr);
    TAYA_Printf("���[�h�ł��� ptr=%p, size=%d\n", dataPtr, dst->recDataSize);
    dataPtr += sizeof(u32);
    GFL_STD_MemCopy( dataPtr, &dst->recRandContext, sizeof(dst->recRandContext) );
    dataPtr += sizeof(dst->recRandContext);
    GFL_STD_MemCopy( dataPtr, dst->recBuffer, dst->recDataSize );

    {
      u32* p = (u32*)BattleRec_GetOperationBufferPtr();
      TAYA_Printf("�^��f�[�^�ǂݍ��݊��� (bufID=%d, %dbyte, ptr=%p, dump=%08x)\n", bufID, dst->recDataSize, p, *p);
    }
  }
  else
  {
//    #define _TEST_REC_1ST
    static const u8 kariRecData[] = {
      0x12,0x01,0xf1,0x64,0x00,0x00,0x21,0x41,0x17,0x00,0x00,0x12,0x01,0xb1,0x25,0x00,
      0x00,0x21,0x41,0x08,0x00,0x00,0x12,0x01,0xf1,0x01,0x00,0x00,0x21,0x41,0x08,0x00,
      0x00,
    };
    static const u8 kariRecRand[] = {
      0x46, 0x11, 0x82, 0x3e, 0xda, 0x39, 0xd2, 0x50,
      0x65, 0x89, 0x07, 0x6c, 0x65, 0x8b, 0x58, 0x5d,
      0xc3, 0x9e, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    dst->recDataSize = sizeof(kariRecData);
    GFL_STD_MemCopy( kariRecData, dst->recBuffer, sizeof(kariRecData) );
    GFL_STD_MemCopy( kariRecRand, &(dst->recRandContext), sizeof(kariRecRand) );

    {
      u32* dataPtr = (u32*)BattleRec_GetOperationBufferPtr();
      TAYA_Printf("�^��f�[�^�ǂݍ��ݎ��s: bufID=%d,  ResultCode=%d, dataPtr=%p, dump=%08x\n", bufID, result, dataPtr, *dataPtr );
    }
  }
}
/**
 *  �f�o�b�O�@�\�̃Z�b�g�A�b�v
 */
static void setDebugParams( const DEBUG_BTL_SAVEDATA* save, BATTLE_SETUP_PARAM* setup )
{
  if( save->fMustTuika )    { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_MUST_TUIKA );    }
  if( save->fMustToku )     { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_MUST_TOKUSEI );  }
  if( save->fMustItem )     { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_MUST_ITEM );     }
  if( save->fMustCritical ) { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_MUST_CRITICAL ); }
  if( save->fHPConst )      { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_HP_CONST );      }
  if( save->fPPConst )      { BTL_SETUP_SetDebugFlag( setup, BTL_DEBUGFLAG_PP_CONST );      }
}

/**
 *  �p�[�e�B�����o�[���e��Print
 */
static void printPartyInfo( POKEPARTY* party )
{
  POKEMON_PARAM* pp;
  u32 cnt, i;

  TAYA_Printf("*** PartyInfo : adrs(%p)\n", party);

  cnt = PokeParty_GetPokeCount( party );
  TAYA_Printf("Member Count=%d\n", cnt);
  for(i=0; i<cnt; ++i)
  {
    pp = PokeParty_GetMemberPointer( party, i );
    TAYA_Printf(" member[%d] : monsno=%d\n", i, PP_Get(pp, ID_PARA_monsno, NULL) );
  }
}
//----------------------------------------------------------------------------------
/**
 * ���[���ɉ����Ė쐶��̃p�[�e�B�����o�[�]�����J�b�g����
 *
 * @param   party
 * @param   rule
 */
//----------------------------------------------------------------------------------
static void cutoff_wildParty( POKEPARTY* party, BtlRule rule )
{
  u32 max;

  switch( rule ){
  case BTL_RULE_SINGLE:  max = 1; break;
  case BTL_RULE_DOUBLE:  max = 2; break;
  case BTL_RULE_TRIPLE:  max = 3; break;
  default:
       max = 1;
       break;
  }

  if( PokeParty_GetPokeCount(party) > max )
  {
    POKEPARTY* tmpParty = PokeParty_AllocPartyWork( GFL_HEAP_LOWID(HEAPID_BTL_DEBUG_SYS) );
    u32 i;
    for(i=0; i<max; ++i){
      PokeParty_Add( tmpParty, PokeParty_GetMemberPointer(party, i) );
    }

    PokeParty_InitWork( party );
    PokeParty_Copy( tmpParty, party );

    GFL_HEAP_FreeMemory( tmpParty );
  }
}

/*----------------------------------------------------------------------------------------------------*/
/*  �ʐM�ݒ�                                                                                          */
/*----------------------------------------------------------------------------------------------------*/
enum {
  TEST_GGID       = 0x3594,
  TEST_COMM_MEMBER_MAX  = 2,
  TEST_MULTI_MEMBER_MAX = 4,
  TEST_COMM_SEND_SIZE_MAX = 100,
  TEST_COMM_BCON_MAX    = 1,

  TEST_MULTI_BCON_MAX = 4,

  TEST_TIMINGID_INIT    = 11,
  TEST_TIMINGID_PRINT,
};

// �o�g���p��M�֐��e�[�u��
extern const NetRecvFuncTable BtlRecvFuncTable[];


typedef struct{
    int gameNo;   ///< �Q�[�����
}BTL_BCON;

static BTL_BCON btlBcon = { WB_NET_BATTLE_SERVICEID };

///< �r�[�R���f�[�^�擾�֐�
static void* testBeaconGetFunc( void* pWork )
{
  return &btlBcon;
}
///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int testBeaconGetSizeFunc( void* pWork )
{
  return sizeof(btlBcon);
}

///< �r�[�R���f�[�^��r�֐�
static BOOL testBeaconCompFunc( GameServiceID myNo, GameServiceID beaconNo )
{
    if(myNo != beaconNo ){
        return FALSE;
    }
    return TRUE;
}


/**
 *  �ʐM�������p�����[�^
 */
static const GFLNetInitializeStruct NetInitParamNormal = {
  BtlRecvFuncTable,        // ��M�֐��e�[�u��
  BTL_NETFUNCTBL_ELEMS,    // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,           // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,           // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  testBeaconGetFunc,      // �r�[�R���f�[�^�擾�֐�
  testBeaconGetSizeFunc,    // �r�[�R���f�[�^�T�C�Y�擾�֐�
  testBeaconCompFunc,     // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  FatalError_Disp,      // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL,           // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,           // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
  NULL, ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL, ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL, ///< DWC�`���̗F�B���X�g
  NULL, ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,    ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  TEST_GGID,          // ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,       //���ɂȂ�heapid
  HEAPID_NETWORK,       //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,        //wifi�p��create�����HEAPID
  HEAPID_NETWORK,       //IRC�p��create�����HEAPID
  GFL_WICON_POSX,       // �ʐM�A�C�R��XY�ʒu
  GFL_WICON_POSY,
  TEST_COMM_MEMBER_MAX,   // �ő�ڑ��l��
  TEST_COMM_SEND_SIZE_MAX,  // �P�t���[��������̍ő呗�M�o�C�g��
  TEST_COMM_BCON_MAX,     // �ő�r�[�R�����W��
  TRUE,           // CRC�v�Z
  FALSE,            // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  FALSE,            // wifi�ʐM���s�����ǂ���
  TRUE,           // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_BATTLE_SERVICEID,//GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};
/**
 *  �ʐM�������p�����[�^�i�}���`�j
 */
static const GFLNetInitializeStruct NetInitParamMulti = {
  BtlRecvFuncTable,     ///< ��M�֐��e�[�u��
  BTL_NETFUNCTBL_ELEMS, ///< ��M�e�[�u���v�f��
  NULL,                 ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,                 ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,                 // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,                 // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  testBeaconGetFunc,     // �r�[�R���f�[�^�擾�֐�
  testBeaconGetSizeFunc, // �r�[�R���f�[�^�T�C�Y�擾�֐�
  testBeaconCompFunc,    // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,                 // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  FatalError_Disp,      // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL,           // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,           // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif
  TEST_GGID,            // ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,       //���ɂȂ�heapid
  HEAPID_NETWORK,       //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,          //wifi�p��create�����HEAPID
  HEAPID_NETWORK,       //IRC�p��create�����HEAPID
  GFL_WICON_POSX,       // �ʐM�A�C�R��XY�ʒu
  GFL_WICON_POSY,
  TEST_MULTI_MEMBER_MAX,    // �ő�ڑ��l��
  TEST_COMM_SEND_SIZE_MAX,  // �ő呗�M�o�C�g��
  TEST_MULTI_BCON_MAX,      // �ő�r�[�R�����W��
  TRUE,                     // CRC�v�Z
  FALSE,                    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,    // �g�p����ʐM���w��
  TRUE,                     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_BATTLE_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  0,//MP�e�ő�T�C�Y 512�܂�
  0,//dummy
};



static void comm_dummy_callback(void* pWork)
{
}


static void testPacketFunc( const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle )
{
  DEBUG_BTL_WORK* wk = pWork;
//  wk->netTestSeq = 1;
}

static void btlAutoConnectCallback( void* pWork )
{
  DEBUG_BTL_WORK* wk = pWork;
  wk->fNetConnect = TRUE;
}

