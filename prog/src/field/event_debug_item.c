//============================================================================================
/**
 * @file    event_debug_item.h
 * @brief   �f�o�b�O�A�C�e���C�x���g
 * @author  k.ohno
 * @date    2009.06.30
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "system/bmp_winframe.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "field/fieldmap.h"
#include "./event_fieldmap_control.h"
#include "event_debug_item.h"
#include "font/font.naix" //NARC_font_large_gftr
#include "sound/pm_sndsys.h"

#include "debug/debug_wordsearch.h"  //DEBUG_WORDSEARCH_sub_search

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "system/main.h"      //GFL_HEAPID_APP�Q��

//============================================================================================
//============================================================================================

#define DEBUG_ITEMDISP_FRAME ( FLDBG_MFRM_MSG )

typedef struct _DEBUGITEM_PARAM EVENT_DEBUGITEM_WORK;
typedef void (StateFunc)(EVENT_DEBUGITEM_WORK* wk);

struct _DEBUGITEM_PARAM {
  StateFunc* state;      ///< �n���h���̃v���O�������
  GMEVENT * event;
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  SAVE_CONTROL_WORK *ctrl;
  GFL_BMPWIN* win;
  GFL_SKB*      skb;
  GFL_MSGDATA *MsgManager;      // ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
  GFL_MSGDATA *CompMsgManager;      //
  WORDSET     *WordSet;               // ���b�Z�[�W�W�J�p���[�N�}�l�[�W���[
  STRBUF*  pStrBuf;
  STRBUF*  pExpStrBuf;
  STRBUF*  pInStr;
  STRBUF*  pCompStr;
  STRBUF* fullword;
  GFL_FONT      *fontHandle;
  int backupDisp;
  int itemid;
  int itemnum;
  int curpos;
  int index;   //�T�u��ʂ̃A�C�e��index
  HEAPID heapID;
  u32 bgchar;
};

#define _DISP_INITX (1)
#define _DISP_INITY (18)
#define _DISP_SIZEX (30)
#define _DISP_SIZEY (4)
#define _BUTTON_WIN_PAL   (12)  // �E�C���h�E
#define _BUTTON_MSG_PAL   (11)  // ���b�Z�[�W�t�H���g
#define FBMP_COL_WHITE    (15)
#define WINCLR_COL(col) (((col)<<4)|(col))

//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(EVENT_DEBUGITEM_WORK* pWork,StateFunc* state);
static void _changeStateDebug(EVENT_DEBUGITEM_WORK* pWork,StateFunc* state, int line);
static void _windowRewrite(EVENT_DEBUGITEM_WORK* wk, int type);
static void _itemNumSelectMenu(EVENT_DEBUGITEM_WORK* wk);
static void _itemKindSelectMenu(EVENT_DEBUGITEM_WORK* wk);


#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(EVENT_DEBUGITEM_WORK* wk,StateFunc state)
{
  wk->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(EVENT_DEBUGITEM_WORK* wk,StateFunc state, int line)
{
  NET_PRINT("EvDebugItem: %d\n",line);
  _changeState(wk, state);
}
#endif


static BOOL sub_strncmp( const STRBUF* str1, const STRBUF* str2, u32 len )
{
  if( GFL_STR_GetBufferLength(str1) < len ){
    return FALSE;
  }
  if( GFL_STR_GetBufferLength(str2) < len ){
    return FALSE;
  }

  {
    const STRCODE *p1 = GFL_STR_GetStringCodePointer( str1 );
    const STRCODE *p2 = GFL_STR_GetStringCodePointer( str2 );
    u32 i;
    for(i=0; i<len; ++i){
      if( *p1++ != *p2++ ){ return FALSE; }
    }
    return TRUE;
  }
  return FALSE;
}
static BOOL sub_search( EVENT_DEBUGITEM_WORK* wk, const STRBUF* word, int dir, int* startidx )
{
  int word_len = GFL_STR_GetBufferLength( word );
  if( word_len )
  {
    int str_cnt, match_cnt, i, allcnt=0;

    str_cnt = GFL_MSG_GetStrCount( wk->CompMsgManager ); //������S��

    i = *startidx;
    while( allcnt < str_cnt )
    {
      i += dir;
      if(i < 0){
        i = str_cnt-1;
      }
      else if(i >= str_cnt){
        i = 0;
      }
      GFL_MSG_GetString( wk->CompMsgManager, i, wk->fullword );
      if( sub_strncmp( word, wk->fullword, GFL_STR_GetBufferLength(word) ) ){
        *startidx = i;
        return TRUE;
      }
      allcnt++;
    }
  }
  return FALSE;
}


#define PRINT_FRAME (GFL_BG_FRAME0_S)

#define PRINT_PALIDX  0
#define SKB_PALIDX1   1
#define SKB_PALIDX2   2
#define GEAR_MAIN_FRAME   (GFL_BG_FRAME2_S)
#define GEAR_BMPWIN_FRAME   (GFL_BG_FRAME1_S)
#define GEAR_BUTTON_FRAME   (GFL_BG_FRAME0_S)

static const GFL_SKB_SETUP setup = {
  100, GFL_SKB_STRTYPE_STRBUF,
  GFL_SKB_MODE_KATAKANA, TRUE, 0,
  PRINT_FRAME, SKB_PALIDX1, SKB_PALIDX2,
};

static void _itemSoftKey(EVENT_DEBUGITEM_WORK* wk)
{
  int dir = 0;
  GflSkbReaction reaction = GFL_SKB_Main(wk->skb);

  switch( reaction ){
  case GFL_SKB_REACTION_QUIT:
    wk->itemid = wk->index;
    GFL_SKB_Delete(wk->skb);
    _windowRewrite(wk,0);

    GFL_BG_SetVisible(GFL_BG_FRAME0_S,VISIBLE_ON);
    GFL_BG_SetVisible(GFL_BG_FRAME1_S,VISIBLE_ON);
    GFL_BG_SetVisible(GFL_BG_FRAME2_S,VISIBLE_ON);
    GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

    G2S_SetBlendAlpha( GEAR_MAIN_FRAME|GEAR_BUTTON_FRAME, GEAR_BMPWIN_FRAME , 3, 16 );


    _CHANGE_STATE(wk, _itemKindSelectMenu);
    return;
  case GFL_SKB_REACTION_INPUT:
    {
      GFL_STR_ClearBuffer(wk->pCompStr);
    }
    break;
  case GFL_SKB_REACTION_PAGE:
    break;
  }


  if(GFL_UI_KEY_GetTrg() && (PAD_BUTTON_R|PAD_BUTTON_L)){
    int idx;
    if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_R){
      dir = 1;
    }
    if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_L){
      dir = -1;
    }
    if(GFL_STR_GetBufferLength(wk->pCompStr) == 0){
      GFL_SKB_PickStr(wk->skb);
      GFL_STR_CopyBuffer(wk->pCompStr, wk->pInStr);
      wk->index = 0;
    }
    idx = wk->index;
    if( DEBUG_WORDSEARCH_sub_search(wk->CompMsgManager, wk->fullword, wk->pCompStr, dir, &idx) == 1 )
    {
      GFL_MSG_GetString( wk->CompMsgManager, idx, wk->fullword );
      GFL_SKB_ReloadStr( wk->skb, wk->fullword );
      wk->index = idx;
    }
  }



}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e����ޕύX
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _itemKindSelectMenu(EVENT_DEBUGITEM_WORK* wk)
{
  int id = wk->itemid;

  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_START){
    wk->skb = GFL_SKB_Create( (void*)(wk->pInStr), &setup, wk->heapID );

//    GFL_BG_SetVisible(GFL_BG_FRAME0_S,VISIBLE_ON);
    GFL_BG_SetVisible(GFL_BG_FRAME1_S,VISIBLE_OFF);
    GFL_BG_SetVisible(GFL_BG_FRAME2_S,VISIBLE_OFF);
    GFL_BG_SetVisible(GFL_BG_FRAME3_S,VISIBLE_OFF);
    GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_OFF);


    _CHANGE_STATE(wk,_itemSoftKey);
    return;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_UP){
    wk->itemid++;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_DOWN){
    wk->itemid--;
  }
  if(GFL_UI_KEY_GetTrg()==PAD_KEY_RIGHT){
    wk->itemid += 10;
  }
  if(GFL_UI_KEY_GetTrg()==PAD_KEY_LEFT){
    wk->itemid -= 10;
  }
  if(wk->itemid > ITEM_DATA_MAX){
    wk->itemid = 0;
  }
  if(wk->itemid < 0){
    wk->itemid = ITEM_DATA_MAX;
  }
  if(id != wk->itemid){
    _windowRewrite(wk,0);
  }
  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_DECIDE){
    _windowRewrite(wk,1);
    _CHANGE_STATE(wk,_itemNumSelectMenu);
  }
  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_CANCEL){
    _CHANGE_STATE(wk,NULL);  //�I��
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�����ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _itemNumSelectMenu(EVENT_DEBUGITEM_WORK* wk)
{
  int id = wk->itemnum;


  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_UP){
    wk->itemnum++;
  }
  if(GFL_UI_KEY_GetRepeat()==PAD_KEY_DOWN){
    wk->itemnum--;
  }
  if(GFL_UI_KEY_GetTrg()==PAD_KEY_RIGHT){
    wk->itemnum += 10;
  }
  if(GFL_UI_KEY_GetTrg()==PAD_KEY_LEFT){
    wk->itemnum -= 10;
  }
  if(wk->itemnum > 999){
    wk->itemnum = 0;
  }
  if(wk->itemnum < 0){
    wk->itemnum = 999;
  }
  if(id != wk->itemnum){
    _windowRewrite(wk,1);
  }

  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_DECIDE){  //�ǉ�
    MYITEM_PTR pMyItem = GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(wk->gsys));
    MYITEM_AddItem(pMyItem,wk->itemid, wk->itemnum,wk->heapID);
    PMSND_PlaySystemSE( SEQ_SE_DECIDE3 );
    _windowRewrite(wk,0);
    _CHANGE_STATE(wk,_itemKindSelectMenu);

  }
  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_START){
    _CHANGE_STATE(wk,NULL);  //�I��
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �G�̏�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _graphicInit(EVENT_DEBUGITEM_WORK* wk)
{
#if 0
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( DEBUG_ITEMDISP_FRAME, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( DEBUG_ITEMDISP_FRAME );
    GFL_BG_LoadScreenReq( DEBUG_ITEMDISP_FRAME );
    GFL_BG_SetPriority( DEBUG_ITEMDISP_FRAME, 0 );
    GFL_BG_SetVisible( DEBUG_ITEMDISP_FRAME, VISIBLE_ON );
  }
#endif

  GFL_FONTSYS_SetDefaultColor();


  wk->bgchar = BmpWinFrame_GraphicSetAreaMan(DEBUG_ITEMDISP_FRAME,
                                             _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, wk->heapID);
  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, wk->heapID);
}


//------------------------------------------------------------------------------
/**
 * @brief   �E�C���h�E�̐���
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _windowCreate(EVENT_DEBUGITEM_WORK* wk)
{

  wk->win = GFL_BMPWIN_Create(
    DEBUG_ITEMDISP_FRAME,
    _DISP_INITX, _DISP_INITY,
    _DISP_SIZEX, _DISP_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_BMPWIN_MakeScreen( wk->win );
  BmpWinFrame_Write( wk->win, WINDOW_TRANS_ON, GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchar), _BUTTON_WIN_PAL );

}



static void _windowRewrite(EVENT_DEBUGITEM_WORK* wk, int type)
{
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(wk->win), WINCLR_COL(FBMP_COL_WHITE) );
  GFL_MSG_GetString(  wk->MsgManager, DEBUG_FIELD_STR35, wk->pStrBuf );
  //id0 �A�C�e���ԍ�
  //id1 �A�C�e����
  //id2 �A�C�e����
  WORDSET_RegisterNumber(wk->WordSet, 0, wk->itemid,
                         3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( wk->WordSet, wk->pExpStrBuf, wk->pStrBuf );

  if(type==1){
    GFL_FONTSYS_SetColor( 1, 2, 15 );
  }
  else{
    GFL_FONTSYS_SetColor( 3, 4, 15 );
  }
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win), 1, 10, wk->pExpStrBuf, wk->fontHandle);


  GFL_MSG_GetString(  wk->MsgManager, DEBUG_FIELD_STR33, wk->pStrBuf );
  //id0 �A�C�e���ԍ�
  //id1 �A�C�e����
  //id2 �A�C�e����
  WORDSET_RegisterItemName( wk->WordSet, 1, wk->itemid);
  WORDSET_ExpandStr( wk->WordSet, wk->pExpStrBuf, wk->pStrBuf  );

  GFL_FONTSYS_SetColor( 1, 2, 15 );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win), 50, 10, wk->pExpStrBuf, wk->fontHandle);



  GFL_MSG_GetString(  wk->MsgManager, DEBUG_FIELD_STR34, wk->pStrBuf );

  WORDSET_RegisterNumber(wk->WordSet, 2, wk->itemnum,
                         3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( wk->WordSet, wk->pExpStrBuf, wk->pStrBuf );

  if(type==1){
    GFL_FONTSYS_SetColor( 3, 4, 15 );
  }
  else{
    GFL_FONTSYS_SetColor( 1, 2, 15 );
  }
  PRINTSYS_Print( GFL_BMPWIN_GetBmp(wk->win), 160, 10, wk->pExpStrBuf, wk->fontHandle);

  GFL_BMPWIN_TransVramCharacter(wk->win);
  //  GFL_BG_LoadScreenV_Req(DEBUG_ITEMDISP_FRAME);
}


//------------------------------------------------------------------------------
/**
 * @brief   �f�o�b�O�v���Z�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT DebugItemMakeProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_DEBUGITEM_WORK* wk = pwk;
  wk->curpos = 0;
  wk->MsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                   NARC_message_d_field_dat, wk->heapID );
  wk->CompMsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
                                       NARC_message_itemname_dat, wk->heapID );
  wk->pStrBuf = GFL_STR_CreateBuffer(100,wk->heapID);
  wk->pExpStrBuf = GFL_STR_CreateBuffer(100,wk->heapID);
  wk->pInStr = GFL_STR_CreateBuffer(100,wk->heapID);
  wk->pCompStr = GFL_STR_CreateBuffer(100,wk->heapID);
  wk->fullword = GFL_STR_CreateBuffer(100,wk->heapID);

  wk->WordSet    = WORDSET_Create( wk->heapID );
  wk->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr ,
                                    GFL_FONT_LOADTYPE_FILE , FALSE , wk->heapID );

  GFL_UI_KEY_SetRepeatSpeed(1,6);
  _graphicInit(wk);
  _windowCreate(wk);
  _windowRewrite(wk,0);
  _CHANGE_STATE(wk, _itemKindSelectMenu);
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�o�b�O�v���Z�XMain
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT DebugItemMakeProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_DEBUGITEM_WORK* wk = pwk;
  int retCode = GFL_PROC_RES_FINISH;
  StateFunc* state = wk->state;

  if(state != NULL){
    state(wk);
    retCode = GFL_PROC_RES_CONTINUE;
  }
  return retCode;

}

//------------------------------------------------------------------------------
/**
 * @brief   �f�o�b�O�v���Z�XEnd
 * @retval  none
 */
//------------------------------------------------------------------------------

static GFL_PROC_RESULT DebugItemMakeProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_DEBUGITEM_WORK* wk = pwk;

  GFL_BG_FreeCharacterArea(DEBUG_ITEMDISP_FRAME,GFL_ARCUTIL_TRANSINFO_GetPos(wk->bgchar),
                           GFL_ARCUTIL_TRANSINFO_GetSize(wk->bgchar));
  GFL_BMPWIN_ClearScreen(wk->win);
  BmpWinFrame_Clear(wk->win, WINDOW_TRANS_OFF);
  GFL_BG_LoadScreenV_Req(DEBUG_ITEMDISP_FRAME);
  GFL_BMPWIN_Delete(wk->win);
  GFL_MSG_Delete(wk->MsgManager);
  GFL_MSG_Delete(wk->CompMsgManager);
  GFL_STR_DeleteBuffer(wk->pCompStr);
  GFL_STR_DeleteBuffer(wk->pStrBuf);
  GFL_STR_DeleteBuffer(wk->pInStr);
  GFL_STR_DeleteBuffer(wk->pExpStrBuf);
  GFL_STR_DeleteBuffer(wk->fullword);
  WORDSET_Delete(wk->WordSet);
  GFL_FONT_Delete(wk->fontHandle);
  return GFL_PROC_RES_FINISH;

}



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA DebugItemMakeProcData = {
  DebugItemMakeProc_Init,
  DebugItemMakeProc_Main,
  DebugItemMakeProc_End,
};




//============================================================================================
//
//    �T�u�C�x���gENUM
//
//============================================================================================

enum{
  _INIT_DEBUGITEM,
  _MAIN_DEBUGITEM,
  _END_DEBUGITEM,


};

//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
static GMEVENT_RESULT EVENT_DebugItemMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_DEBUGITEM_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;
  GFL_PROC_RESULT ret;


  switch (*seq) {
  case _INIT_DEBUGITEM:
    ret = DebugItemMakeProc_Init(NULL,seq,work,work);
    if(GFL_PROC_RES_FINISH == ret){
      (*seq)++;
    }
    break;
  case _MAIN_DEBUGITEM:
    ret = DebugItemMakeProc_Main(NULL,seq,work,work);
    if(GFL_PROC_RES_FINISH == ret){
      (*seq)++;
    }
    break;
  case _END_DEBUGITEM:
    ret = DebugItemMakeProc_End(NULL,seq,work,work);
    if(GFL_PROC_RES_FINISH == ret){
      (*seq)++;
      return GMEVENT_RES_FINISH;
    }
  default:
    break;
  }
  return GMEVENT_RES_CONTINUE;

}


//------------------------------------------------------------------------------
/**
 * @brief   �A�C�e�������
 * @param   heapSize    WIFI�p�̃������m�ۃT�C�Y
 * @retval  none
 */
//------------------------------------------------------------------------------


void EVENT_DebugItemMake(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,GMEVENT * event, HEAPID heapID)
{
  GMEVENT * child_event;
  EVENT_DEBUGITEM_WORK * wk;

  GMEVENT_Change( event, EVENT_DebugItemMain, sizeof(EVENT_DEBUGITEM_WORK) );
  wk = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear(wk, sizeof(EVENT_DEBUGITEM_WORK));
  wk->ctrl = SaveControl_GetPointer();
  wk->gsys = gsys;
  wk->fieldmap = fieldmap;
  wk->event = event;
  wk->heapID = heapID;
  wk->itemnum = 1;
}

