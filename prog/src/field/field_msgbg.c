//======================================================================
/**
 * @file  field_msgbg.c
 * @brief �t�B�[���h�@���b�Z�[�WBG�֘A
 * @authaor kagaya
 * @date  2008.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"

#include "arc_def.h"
#include "font/font.naix"

#include "message.naix"
#include "msg/msg_yesnomenu.h"

#include "field_bg_def.h"
#include "field/field_msgbg.h"

#include "sound/pm_sndsys.h"

#include "gamesystem\msgspeed.h"

#include "winframe.naix"

//======================================================================
//  define
//======================================================================
#ifdef DEBUG_ONLY_FOR_yoshida
#define TEST_TALKMSGWIN_TYPE (TALKMSGWIN_TYPE_GIZA)
#else
#define TEST_TALKMSGWIN_TYPE (TALKMSGWIN_TYPE_NORMAL)
#endif

#ifdef PM_DEBUG
#define DEBUG_FLDMSGBG
#endif

#define FLDMSGBG_BGFRAME (FLDBG_MFRM_MSG) ///<�g�pBG�t���[��
#define FLDMSGBG_BGFRAME_BLD (FLDBG_MFRM_EFF1) ///<�g�pBG�t���[�� �������p

#define FLDMSGBG_PRINT_MAX (4)        ///<PRINT�֘A�v�f���ő�
#define FLDMSGBG_PRINT_STREAM_MAX (1) ///<PRINT_STREAM�ғ���

#define FLDMSGBG_STRLEN (128)       ///<�����񒷂��W��

#define SPWIN_HEIGHT_FONT_SPACE (2)

#define MSG_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)
#define MSG_LAST_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

#define BGWIN_SCROLL_SPEED (12)
#define BGWIN_MSG_SCROLL_SPEED (4)

#define PAD_KEY_ALL (PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT)

#define BGFRAME_ERROR (0xff)

///< �ʐM���ȂǂɎg�p����A�������b�Z�[�W
#define AUTO_MSG_WAIT      ( MSGSPEED_NORMAL ) ///<�������̃��b�Z�[�W�X�s�[�h
#define AUTO_MSG_ONE_KEY_WAIT  ( 25 ) ///<�L�[�E�G�C�g
#define AUTO_MSG_ALLCLEAR_KEY_WAIT  ( 50 ) ///<�L�[�E�G�C�g

#define GIZA_SHAKE_Y (8) ///<�M�U�E�B���h�E�����h�ꕝ

///�V�X�e���E�B���h�E�f�t�H���g�ʒu�ƕ�
#define SYSWIN_DEF_PX (2)
#define SYSWIN_DEF_SX (27)
#define SYSWIN_DEF_SY (4)

//--------------------------------------------------------------
//  ���b�Z�[�W�E�B���h�E�A�L�����I�t�Z�b�g
//--------------------------------------------------------------
enum
{
  CHARNO_CLEAR = 0, ///<�N���A�L���� 0
  CHARNO_WINFRAME = 1, ///<�E�B���h�E�t���[�� 1-10 (chara 9
  CHARNO_BALLOONWIN = 11, ///<�����o���E�B���h�E 11-30 (chara 18
  CHARNO_SYSWIN = 31, ///<�V�X�e���E�B���h�E 31-50 (chara 18
  CHARNO_OTHERSWIN = 51, ///<���̑��E�B���h�E 46- (�g�p���ɏ�����
};

//--------------------------------------------------------------
//  ���b�Z�[�W�E�B���h�E�@�g�p�p���b�g
//--------------------------------------------------------------
enum
{
  PANO_BGWIN = 9, //BG�E�B���h�E
  PANO_SPWIN = 9, //����E�B���h�E
  PANO_MENU_W = 10, //���j���[�p���b�gNo �w�i�� �s�������j���[�p
  PANO_FONT_TALKMSGWIN = 11, ///<�����o���t�H���g�p���b�gNo
  PANO_TALKMSGWIN = 12, ///<�����o���E�B���h�E�p���b�gNo
  PANO_MENU_B = 13, ///<���j���[�p���b�gNo �w�i�� �������p
  PANO_FONT = 14, ///<�t�H���g�p���b�gNo �������p
};

//======================================================================
//  typedef struct
//======================================================================
//--------------------------------------------------------------
/// FLDPRINT_CONTROL
//  ���b�Z�[�W�Đ��̊e��cont�Ǘ�
//--------------------------------------------------------------
typedef struct
{
  u8 auto_msg_flag;     ///<�������b�Z�[�W����
  u8 key_wait;          ///<�L�[�E�G�C�g
  u8 key_skip;          ///<�X�L�b�v �g���K�`�F�b�N
  u8 pad[1];
} FLDPRINT_CONTROL;


//--------------------------------------------------------------
/// FLDKEYWAITCURSOR
//--------------------------------------------------------------
struct _FLDKEYWAITCURSOR
{
  u8 cursor_state;
  u8 cursor_anm_no;
  u8 cursor_anm_frame;
  u8 padding;
  GFL_BMP_DATA *bmp_cursor;

  void *pArcChara;
  NNSG2dCharacterData *pChara;
};

//--------------------------------------------------------------
/// FLDMSGPRINT
//--------------------------------------------------------------
struct _TAG_FLDMSGPRINT
{
  FLDMSGBG *fmb;
  
  BOOL printTransFlag;
  GFL_FONT *fontHandle; //FLDMSGBG����
  PRINT_QUE *printQue;  //FLDMSGBG����
  GFL_MSGDATA *msgData; //���[�U�[����
  GFL_BMPWIN *bmpwin;   //���[�U�[����
  
  STRBUF *strBuf;
  PRINT_UTIL printUtil;
};

//--------------------------------------------------------------
/// FLDTALKMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDTALKMSGWIN
{
  u8 padding;
  u8 talkMsgWinIdx;
  s16 shake_y;
  
  STRBUF *strBuf;
  TALKMSGWIN_SYS *talkMsgWinSys; //FLDMSGBG���
  FLDKEYWAITCURSOR cursor_work;
  FLDMSGBG *fmb;
  
  //debug
  const VecFx32 *watch_pos;
  VecFx32 d_pos;
  VecFx32 d_offs;
};

//--------------------------------------------------------------
/// FLDMSGBG
//--------------------------------------------------------------
struct _TAG_FLDMSGBG
{
  HEAPID heapID; //u16
  u16 bgFrame;
  
  u16 bgFrameBld;
  u16 deriveFont_plttNo;
  
  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  FLDMSGPRINT msgPrintTbl[FLDMSGBG_PRINT_MAX];
  
  FLDSUBMSGWIN *subMsgWinTbl[FLDSUBMSGWIN_MAX];
  
  FLDTALKMSGWIN balloonMsgWinTbl[FLDTALKMSGWIN_MAX];
  
  TALKMSGWIN_SYS *talkMsgWinSys;

  FLDPRINT_CONTROL print_cont; ///<���b�Z�[�W�\�� �ݒ�
  
  GFL_TCBLSYS *printTCBLSys;
  GFL_G3D_CAMERA *g3Dcamera;
  
  u8 req_reset_bg2_control;
  u8 padding[3];
  
#ifdef DEBUG_FLDMSGBG
  int d_printTCBcount;
#endif
};

//--------------------------------------------------------------
/// FLDMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDMSGWIN
{
  GFL_BMPWIN *bmpwin;   //���[�U�[����
  FLDMSGPRINT *msgPrint;
  FLDMSGBG *fmb;
};

//--------------------------------------------------------------
/// FLDSYSWIN
//--------------------------------------------------------------
struct _TAG_FLDSYSWIN
{
  GFL_BMPWIN *bmpwin;   //���[�U�[����
  FLDMSGPRINT *msgPrint;
  FLDMSGBG *fmb;
};

//--------------------------------------------------------------
/// FLDMENUFUNC
//--------------------------------------------------------------
struct _TAG_FLDMENUFUNC
{
  FLDMSGBG *fmb;
  FLDMSGPRINT *msgPrint;
  GFL_BMPWIN *bmpwin;
  
  BMP_MENULIST_DATA *pMenuListData;
  BMPMENULIST_WORK *pMenuListWork;
};

//--------------------------------------------------------------
/// FLDMSGPRINT_STREAM
//--------------------------------------------------------------
struct _TAG_FLDMSGPRINT_STREAM
{
  u16 pad;
  s16 msg_wait;
  PRINT_STREAM *printStream;
  FLDMSGBG *fmb;
};

//--------------------------------------------------------------
/// FLDMSGWIN_STREAM
//--------------------------------------------------------------
struct _TAG_FLDMSGWIN_STREAM
{
  GFL_BMPWIN *bmpwin;
  FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
  GFL_MSGDATA *msgData; //���[�U�[����
  STRBUF *strBuf;
  FLDMSGBG *fmb;
   
  FLDKEYWAITCURSOR cursor_work;
};

//--------------------------------------------------------------
/// FLDSYSWIN_STREAM
//--------------------------------------------------------------
struct _TAG_FLDSYSWIN_STREAM
{
  GFL_BMPWIN *bmpwin;
  FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
  GFL_MSGDATA *msgData; //���[�U�[����
  STRBUF *strBuf;
  FLDMSGBG *fmb;
   
  FLDKEYWAITCURSOR cursor_work;
};

//--------------------------------------------------------------
/// FLDBGWIN
//--------------------------------------------------------------
struct _TAG_FLDBGWIN
{
  FLDMSGBG *fmb;
  
  GFL_BMPWIN *bmpwin;
  GFL_BMP_DATA *bmp_new;
  GFL_BMP_DATA *bmp_old;
  
  const STRBUF *strBuf; //���[�U�[����
  STRBUF *strTemp;
  
  int seq_no;
  int y;
  int scroll_y;
  u32 line;
  
  FLDKEYWAITCURSOR cursor_work;

  GFL_FONT * useFontHandle; // �g�p����t�H���g�n���h��
};

//--------------------------------------------------------------
/// FLDSPWIN
//--------------------------------------------------------------
struct _TAG_FLDSPWIN
{
  GFL_BMPWIN *bmpwin;
  STRBUF *strBuf;
  FLDMSGBG *fmb;
  
  GFL_BMP_DATA *bmp_bg;
  FLDKEYWAITCURSOR cursor_work;
};

//======================================================================
//  proto
//======================================================================
static void FldMenuFuncH_BmpMenuListH(
  BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH );

static PRINTSTREAM_STATE fldMsgPrintStream_ProcPrint(
    FLDMSGPRINT_STREAM *stm );

static BOOL fldSubMsgWin_Print( FLDSUBMSGWIN *subwin );

static void bgwin_InitGraphic(
    u32 bgframe, u32 plttno, u32 type, HEAPID heapID );
static void bgwin_WriteWindow(
    u8 frm, u8 px, u8 py, u8 sx, u8 sy, u16 cgx, u8 pal );
static void bgwin_CleanWindow(
    u8 frm, u8 px, u8 py, u8 sx, u8 sy, u16 cgx, u8 pal );
static BOOL bgwin_CheckStrLineEOM(
    const STRBUF *str_src, STRBUF *str_temp, u32 line );
static BOOL bgwin_PrintStr(
    GFL_BMP_DATA *bmp, GFL_FONT *font,
    const STRBUF *str_src, STRBUF *str_temp, u32 *line, u8 col );
static BOOL bgwin_ScrollBmp(
    GFL_BMP_DATA *bmp, GFL_BMP_DATA *old, GFL_BMP_DATA *new,
    int y, u16 n_col );

static void syswin_InitGraphic( HEAPID heapID );
static void syswin_WriteWindow( const GFL_BMPWIN *bmpwin );
static GFL_BMPWIN * syswin_InitBmp( u8 pos_x, u8 pos_y, u8 sx, u8 sy, HEAPID heapID );
static void syswin_ClearBmp( GFL_BMPWIN *bmpwin );
static void syswin_DeleteBmp( GFL_BMPWIN *bmpwin );

static void setBGResource( FLDMSGBG *fmb, const BOOL trans );
static void setBG1Resource( FLDMSGBG *fmb );
static void resetBG2Control( BOOL cnt_set );
static void resetBG2ControlProc( FLDMSGBG *fmb );
static void transBGResource( int bgFrame, HEAPID heapID );
static void transBGResourceParts( int bgFrame, HEAPID heapID, MSGBG_TRANS_RES res );
static void setBlendAlpha( BOOL on );

static GFL_BMPWIN * winframe_InitBmp( u32 bgFrame, HEAPID heapID,
  u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no );
static void winframe_DeleteBmp( GFL_BMPWIN *bmpwin );
static void winframe_SetPaletteBlack( u32 heapID );
static void winframe_SetPaletteWhith( u32 heapID );

static int FldMsgBG_SetFldSubMsgWin( FLDMSGBG *fmb, FLDSUBMSGWIN *subwin );
static FLDSUBMSGWIN * FldMsgBG_DeleteFldSubMsgWin( FLDMSGBG *fmb, int id );

static const FLDMENUFUNC_HEADER DATA_MenuHeader_YesNo;
//static const u8 ALIGN4 skip_cursor_Character[128];

static void Control_Init( FLDPRINT_CONTROL* cont );
static void Control_StartPrint( FLDPRINT_CONTROL* cont );
static void Control_SetAutoPrintFlag( FLDPRINT_CONTROL* cont, BOOL flag );
static BOOL Control_GetAutoPrintFlag( const FLDPRINT_CONTROL* cont );
static int Control_GetMsgWait( const FLDPRINT_CONTROL* cont );
static BOOL Control_GetSkipKey( FLDPRINT_CONTROL* cont );
static BOOL Control_GetWaitKey( FLDPRINT_CONTROL* cont, PRINTSTREAM_PAUSE_TYPE type );
static void set_printStreamTempSpeed( PRINT_STREAM *printStream, BOOL skip );

#ifdef DEBUG_FLDMSGBG
static void DEBUG_AddCountPrintTCB( FLDMSGBG *fmb );
static void DEBUG_SubCountPrintTCB( FLDMSGBG *fmb );
static void DEBUG_CheckCountPrintTCB( FLDMSGBG *fmb );
#else
#define DEBUG_AddCountPrintTCB( fmb ) ((void)0)
#define DEBUG_SubCountPrintTCB( fmb ) ((void)0)
#define DEBUG_CheckCountPrintTCB( fmb ) ((void)0)
#endif

//======================================================================
//  FLDMSGBG  �t�B�[���h���b�Z�[�WBG�֘A
//======================================================================
//--------------------------------------------------------------
//  10��ROM�p ���Ώ� �E�B���h�E�̉��J���[�𖳌��� 0x50001a0
//--------------------------------------------------------------
#if 0
static void debug_ROM091030_WindowColorOFF( HEAPID heapID )
{
  u16 *buf = GFL_HEAP_AllocMemoryLo( heapID, 32 );
  u32 offs = HW_BG_PLTT + (32*PANO_MENU);
  DC_FlushRange( (void*)offs, 32 );
  MI_CpuCopy( (void*)offs, buf, 32 );
  buf[1] = 0;
  DC_FlushRange( (void*)buf, 32 );
  GX_LoadBGPltt( (void*)buf, offs-HW_BG_PLTT, 32 );
  GFL_HEAP_FreeMemory( buf );
}
#endif

#if 0
static void debug_ROM091030_WindowColorON( HEAPID heapID )
{
  u16 *buf = GFL_HEAP_AllocMemoryLo( heapID, 32 );
  u32 offs = HW_BG_PLTT + (32*PANO_MENU);
  DC_FlushRange( (void*)offs, 32 );
  MI_CpuCopy( (void*)offs, buf, 32 );
  buf[1] = 0x7fff;
  DC_FlushRange( (void*)buf, 32 );
  GX_LoadBGPltt( (void*)buf, offs-HW_BG_PLTT, 32 );
  GFL_HEAP_FreeMemory( buf );
}
#endif

//--------------------------------------------------------------
/**
 * FLDMSGBG �쐬
 * @param heapID  HEAPID
 * @param g3Dcamera GFL_G3D_CAMERA* �����o���E�B���h�E�Ŏg�p NULL=�g�p���Ȃ�
 * @retval  FLDMAPBG  FLDMAPBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FLDMSGBG_Create( HEAPID heapID, GFL_G3D_CAMERA *g3Dcamera )
{
  FLDMSGBG *fmb;
  
  fmb = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMSGBG) );
  fmb->heapID = heapID;

#if 1  
  fmb->bgFrame = BGFRAME_ERROR;
  fmb->bgFrameBld = BGFRAME_ERROR;
#else
  fmb->bgFrame = FLDMSGBG_BGFRAME;
  fmb->bgFrameBld = FLDMSGBG_BGFRAME_BLD;
#endif
  
  fmb->g3Dcamera = g3Dcamera;
  
  { //font
    fmb->fontHandle = GFL_FONT_Create(
      ARCID_FONT,
      NARC_font_large_gftr, //�V�t�H���gID
      GFL_FONT_LOADTYPE_FILE, FALSE, fmb->heapID );
  }
  
  { //print
    fmb->printQue = PRINTSYS_QUE_Create( fmb->heapID );
  }
  
  { //PRINT_STREAM TCB
    fmb->printTCBLSys = GFL_TCBL_Init(
        fmb->heapID, fmb->heapID, FLDMSGBG_PRINT_STREAM_MAX, 4 );
  }

  { // OPTION
    Control_Init( &fmb->print_cont );
  }
  
  //FLDMSGBG_SetupResource( fmb );
  return( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG ���\�[�X�Z�b�g�A�b�v
 * @param fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_SetupResource( FLDMSGBG *fmb )
{
  setBGResource( fmb, TRUE );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG ���\�[�X�Z�b�g�A�b�v�]���͌�Ŏ��O�ōs��
 * @param fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_SetupResourceNoTrans( FLDMSGBG *fmb )
{
  setBGResource( fmb, FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG ��ʏ������͍s�킸VRAM���\�[�X�̂ݕ������邽�߂̓]��
 * @param int     bgFrame
 * @param HEAPID  heapID
 * @retval none
 */
//--------------------------------------------------------------
void FLDMSGBG_TransResource( int bgFrame, HEAPID heapID )
{
  transBGResource( bgFrame, heapID );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG ���\�[�X�]��
 * @param fmb FLDMSGBG
 * @param res  �w�胊�\�[�X MSGBG_TRANS_RES�`
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_TranceResourceParts( FLDMSGBG *fmb, MSGBG_TRANS_RES res )
{
  // �p���b�g�E�V�X�e���E�C���h�E
  transBGResourceParts( fmb->bgFrame, fmb->heapID, res );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �폜
 * @param fmb FLDMSGBG
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_Delete( FLDMSGBG *fmb )
{
  int i = 0;
  FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
  
  if( fmb->talkMsgWinSys != NULL ){
    TALKMSGWIN_SystemDelete( fmb->talkMsgWinSys );
  }
  
  if( fmb->printTCBLSys != NULL ){
    DEBUG_CheckCountPrintTCB( fmb );
    GFL_TCBL_Exit( fmb->printTCBLSys );
  }
  
  if( fmb->bgFrame != BGFRAME_ERROR ){
    GFL_BG_FillCharacterRelease( fmb->bgFrame, 1, 0 );
    GFL_BG_FreeBGControl( fmb->bgFrame );
  }
  
  if( fmb->bgFrameBld != BGFRAME_ERROR ){
    GFL_BG_FreeBGControl( fmb->bgFrameBld );
  }
  
  do{
    #if 0
    if( msgPrint->msgData != NULL ){
      GFL_MSG_Delete( msgPrint->msgData );
    }
    #endif
    if( msgPrint->strBuf != NULL ){
      GFL_STR_DeleteBuffer( msgPrint->strBuf );
    }
    msgPrint++;
    i++;
  }while( i < FLDMSGBG_PRINT_MAX );
  
  if( fmb->printQue != NULL ){
    PRINTSYS_QUE_Delete( fmb->printQue );
  }
  
  if( fmb->fontHandle != NULL ){
    GFL_FONT_Delete( fmb->fontHandle );
  }
  
  GFL_HEAP_FreeMemory( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG BG���\�[�X�̂ݔj��
 * @param fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_ReleaseBGResouce( FLDMSGBG *fmb )
{
  if( fmb->talkMsgWinSys != NULL ){
    TALKMSGWIN_SystemDelete( fmb->talkMsgWinSys );
    fmb->talkMsgWinSys = NULL;
  }
  
  if( fmb->bgFrame != BGFRAME_ERROR ){
    GFL_BG_FillCharacterRelease( fmb->bgFrame, 1, 0 );
    GFL_BG_FreeBGControl( fmb->bgFrame );
    fmb->bgFrame = BGFRAME_ERROR;
  }
  
#if 0
  if( fmb->bgFrameBld != BGFRAME_ERROR ){
    GFL_BG_FreeBGControl( fmb->bgFrameBld );
    fmb->bgFrameBld = BGFRAME_ERROR;
  }
#endif
}

//--------------------------------------------------------------
/**
 * FLDMSGBG BG2���\�[�X��j�����čĐݒ�t���O���Z�b�g����B
 * �j���ς݂̏ꍇ�͂Ȃɂ����Ȃ��B
 * @param fmb FLDMSGBG *
 * @retval BOOL TRUE=���\�[�X��j���BFALSE=�j���ς݂Ȃ̂ŉ������Ȃ������B
 */
//--------------------------------------------------------------
BOOL FLDMSGBG_ReleaseBG2Resource( FLDMSGBG *fmb )
{
  if( fmb->bgFrameBld != BGFRAME_ERROR ){
    GFL_BG_FreeBGControl( fmb->bgFrameBld );
    fmb->bgFrameBld = BGFRAME_ERROR;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG BG���\�[�X�����Z�b�g
 * @param *fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_ResetBGResource( FLDMSGBG *fmb )
{
  if( fmb->talkMsgWinSys != NULL ){
    TALKMSGWIN_SystemDelete( fmb->talkMsgWinSys );
  }
  
  setBG1Resource( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g����
 * @param fmb FLDMSGBG
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_PrintMain( FLDMSGBG *fmb )
{
  int i;
  PRINTSYS_QUE_Main( fmb->printQue );
  
  {
    FLDSUBMSGWIN **subWin = fmb->subMsgWinTbl;
    
    for( i = 0; i < FLDSUBMSGWIN_MAX; i++, subWin++ ){
      if( (*subWin) != NULL ){
        fldSubMsgWin_Print( *subWin );
      }
    }
  }
  
  {
    FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
    
    for( i = 0; i < FLDMSGBG_PRINT_MAX; i++, msgPrint++ ){
      if( msgPrint->printQue != NULL ){
        msgPrint->printTransFlag = PRINT_UTIL_Trans(
          &msgPrint->printUtil, msgPrint->printQue );
      }
    }
  }
  
  GFL_TCBL_Main( fmb->printTCBLSys );
  
  if( fmb->talkMsgWinSys != NULL ){
    FLDTALKMSGWIN *bWin = fmb->balloonMsgWinTbl;
    
    for( i = 0; i < FLDTALKMSGWIN_MAX; i++, bWin++ ){
      if( bWin->talkMsgWinSys != NULL ){
#ifndef DEBUG_ONLY_FOR_kagaya //debug
        {
          int trg = GFL_UI_KEY_GetTrg();
          int cont = GFL_UI_KEY_GetCont();
          int rep = GFL_UI_KEY_GetRepeat();
          
          if( rep & PAD_KEY_UP ){
            bWin->d_offs.z -= 0x800;
            OS_Printf( "�o���[���E�B���h�E Z=%d(0x%xH)\n", 
                bWin->d_offs.z/FX32_ONE, bWin->d_offs.z );
          }else if( rep & PAD_KEY_DOWN ){
            bWin->d_offs.z += 0x800;
            OS_Printf( "�o���[���E�B���h�E Z=%d(0x%xH)\n", 
                bWin->d_offs.z/FX32_ONE, bWin->d_offs.z );
          }
          
          bWin->d_pos = *bWin->watch_pos;
          bWin->d_pos.x += bWin->d_offs.x;
          bWin->d_pos.y += bWin->d_offs.y;
          bWin->d_pos.z += bWin->d_offs.z;
        }
#endif
      }
    }
    
    TALKMSGWIN_SystemMain( fmb->talkMsgWinSys );
    TALKMSGWIN_SystemDraw2D( fmb->talkMsgWinSys );
  }
}

//--------------------------------------------------------------
/**
 * FLDMSGBG G3D�`�揈��
 * @param fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_PrintG3D( FLDMSGBG *fmb )
{
  if( fmb->talkMsgWinSys != NULL ){
    TALKMSGWIN_SystemDraw3D( fmb->talkMsgWinSys );
  }
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g�L���[�ɒ��܂��Ă��鏈����S�ăN���A
 * @param fmb FLDMSGBG
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb )
{
  PRINTSYS_QUE_Clear( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g�L���[�ɒ��܂��Ă��鏈����S�Ď��s
 * @param fmb FLDMSGBG
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_AllPrint( FLDMSGBG *fmb )
{
  while( FLDMSGBG_CheckFinishPrint(fmb) != TRUE ){
    FLDMSGBG_PrintMain( fmb );
  }
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g�L���[�̏������S�Ċ������Ă��邩�`�F�b�N
 * @param fmb FLDMSGBG
 * @retval  BOOL TRUE=����
 */
//--------------------------------------------------------------
BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb )
{
  return( PRINTSYS_QUE_IsFinished(fmb->printQue) );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �g�p���Ă���PRINT_QUE��Ԃ�
 * @param fmb FLDMSGBG
 * @retval  PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb )
{
  return( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �g�p���Ă���GFL_FONT��Ԃ�
 * @param fmb FLDMSGBG
 * @retval  GFL_FONT*
 */
//--------------------------------------------------------------
GFL_FONT * FLDMSGBG_GetFontHandle( FLDMSGBG *fmb )
{
  return( fmb->fontHandle );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG FLDMSGBG�Ŏg�p���Ă���HEAPID��GFL_MSGDATA�������B
 * �߂�lGFL_MSGDATA�̍폜�͊e���ōs�����B
 * @param fmb FLDMSGBG
 * @param arcDatIDMsg ���b�Z�[�W���܂܂��A�[�J�C�u�f�[�^ID
 * @retval  GFL_MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * FLDMSGBG_CreateMSGDATA( FLDMSGBG *fmb, u32 arcDatIDMsg )
{
  GFL_MSGDATA *msgData;
  msgData = GFL_MSG_Create(
    GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, arcDatIDMsg, fmb->heapID );
  return( msgData );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG MSGDATA�̍폜�B
 * @param msgData GFL_MSGDATA
 * @retval nothing
 * @note Create()�Ƒ΂ɂȂ�֐����~�����Ƃ̗v�]�ɂ��B
 * ���g��GFL_MSGDATA���Ă�ł��邾���B
 */
//--------------------------------------------------------------
void FLDMSGBG_DeleteMSGDATA( GFL_MSGDATA *msgData )
{
  GFL_MSG_Delete( msgData );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG BG2�̃��Z�b�g��v��
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FLDMSGBG_ReqResetBG2( FLDMSGBG *fmb )
{
  fmb->req_reset_bg2_control = TRUE;
}

//======================================================================
//  FLDPRINT_CONTROL �����L�[����{���b�Z�[�W�X�s�[�h���ݒ�
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �����L�[����{���b�Z�[�W�X�s�[�h���@�ݒ�
 *
 *  @param  fmb       ���[�N
 *  @param  flag      TRUE�FON    FALSE�FOFF
 */
//-----------------------------------------------------------------------------
void FLDMSGBG_SetAutoPrintFlag( FLDMSGBG *fmb, BOOL flag )
{
  Control_SetAutoPrintFlag( &fmb->print_cont, flag );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����L�[����@�{�@���b�Z�[�W�X�s�[�h���@�ݒ�擾
 *
 *  @param  fmb     ���[�N
 *
 *  @retval TRUE    �ݒ�ON
 *  @retval FALSE   �ݒ�OFF
 */
//-----------------------------------------------------------------------------
BOOL FLDMSGBG_GetAutoPrintFlag( const FLDMSGBG *fmb )
{
  return Control_GetAutoPrintFlag( &fmb->print_cont );
}


//======================================================================
//  FLDMSGPRINT �t�B�[���h�����\���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�ݒ�
 * @param fmb FLDMSGBG
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmpwin  �\�����鏉�����ς݂�GFL_BMPWIN
 * @retval  FLDMSGPRINT*
 */
//--------------------------------------------------------------
FLDMSGPRINT * FLDMSGPRINT_SetupPrint(
  FLDMSGBG *fmb, GFL_MSGDATA *msgData, GFL_BMPWIN *bmpwin )
{
  int i = 0;
  FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
  
  do{
    if( msgPrint->printQue == NULL ){
      msgPrint->fmb = fmb;
      msgPrint->bmpwin = bmpwin;
      msgPrint->printTransFlag = TRUE;
      msgPrint->fontHandle = fmb->fontHandle;
      msgPrint->printQue = fmb->printQue;
      msgPrint->strBuf = GFL_STR_CreateBuffer(
          FLDMSGBG_STRLEN, fmb->heapID );
      #if 0
      msgPrint->msgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
          ARCID_MESSAGE, arcDatIDMsg, fmb->heapID );
      #else
      msgPrint->msgData = msgData;
      #endif
      PRINT_UTIL_Setup( &msgPrint->printUtil, bmpwin );
      return( msgPrint );
    }
    msgPrint++;
    i++;
  }while( i < FLDMSGBG_PRINT_MAX );
  
  GF_ASSERT( 0 );
  return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�폜
 * @param msgPrint  FLDMSGPRINT FLDMSGPRINT_SetupPrint()�̖߂�l
 * @retval  nothing 
 * FLDMSGPRINT_SetupPrint()�Ŏw�肵��msgData,bmpwin�̍폜�͊e���ōs�����B
 */
//--------------------------------------------------------------
void FLDMSGPRINT_Delete( FLDMSGPRINT *msgPrint )
{
  msgPrint->fontHandle = NULL;
  msgPrint->printQue = NULL;
  GFL_STR_DeleteBuffer( msgPrint->strBuf );
  msgPrint->strBuf = NULL;
  #if 0
  GFL_MSG_Delete( msgPrint->msgData );
  #endif
  msgPrint->msgData = NULL;
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g
 * @param msgPrint  FLDMSGPRINT
 * @param x   �\��X���W
 * @param y   �\��Y���W
 * @param strID ���b�Z�[�W�f�[�^ ������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_Print( FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID )
{
  GF_ASSERT( msgPrint->msgData );
  GFL_MSG_GetString( msgPrint->msgData, strID, msgPrint->strBuf );
  PRINT_UTIL_Print( &msgPrint->printUtil, msgPrint->printQue,
    x, y, msgPrint->strBuf, msgPrint->fontHandle );   
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g �J���[�w��L��
 * @param msgPrint  FLDMSGPRINT
 * @param x   �\��X���W
 * @param y   �\��Y���W
 * @param strID ���b�Z�[�W�f�[�^ ������ID
 * @param color PRINTSYS_LSB
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_PrintColor(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID, PRINTSYS_LSB color )
{
  GF_ASSERT( msgPrint->msgData );
  GFL_MSG_GetString( msgPrint->msgData, strID, msgPrint->strBuf );
  PRINT_UTIL_PrintColor( &msgPrint->printUtil, msgPrint->printQue,
    x, y, msgPrint->strBuf, msgPrint->fontHandle, color );    
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�@STRBUF�w��
 * @param msgPrint  FLDMSGPRINT
 * @param x   �\��X���W
 * @param y   �\��Y���W
 * @param strBuf  �\������STRBUF
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_PrintStrBuf(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, STRBUF *strBuf )
{
  PRINT_UTIL_Print( &msgPrint->printUtil, msgPrint->printQue,
    x, y, strBuf, msgPrint->fontHandle );   
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�@STRBUF�w��A�J���[�w�肠��AGFL_FONT�O���n��
 * @param msgPrint  FLDMSGPRINT
 * @param x   �\��X���W
 * @param y   �\��Y���W
 * @param strBuf  �\������STRBUF
 * @param color PRINTSYS_LSB
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_PrintStrBufColorFontHandle( FLDMSGPRINT *msgPrint,
    u32 x, u32 y, STRBUF *strBuf, PRINTSYS_LSB color, GFL_FONT *fontHandle )
{
  PRINT_UTIL_PrintColor( &msgPrint->printUtil, msgPrint->printQue,
    x, y, strBuf, fontHandle, color );    
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�@STRBUF�w��A�J���[�w�肠��
 * @param msgPrint  FLDMSGPRINT
 * @param x   �\��X���W
 * @param y   �\��Y���W
 * @param strBuf  �\������STRBUF
 * @param color PRINTSYS_LSB
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_PrintStrBufColor( FLDMSGPRINT *msgPrint,
    u32 x, u32 y, STRBUF *strBuf, PRINTSYS_LSB color )
{
  FLDMSGPRINT_PrintStrBufColorFontHandle(msgPrint, x, y, strBuf, color, msgPrint->fontHandle);
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@�v�����g�]���`�F�b�N
 * @param msgPrint  FLDMSGPRINT
 * @retval  BOOL  TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint )
{
  return( msgPrint->printTransFlag );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �\������r�b�g�}�b�v�E�B���h�E��ύX����
 * @param msgPrint  FLDMSGPRINT
 * @param bmpwin  �\�����鏉�����ς݂�GFL_BMPWIN
 * @retval  nothing
 * �ȑO�Ɏw�肵��bmpwin�̍폜�͊e���ōs�����B
 */
//--------------------------------------------------------------
void FLDMSGPRINT_ChangeBmpWin( FLDMSGPRINT *msgPrint, GFL_BMPWIN *bmpwin )
{
  msgPrint->bmpwin = bmpwin;
  PRINT_UTIL_Setup( &msgPrint->printUtil, bmpwin );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �r�b�g�}�b�v�N���A
 * @param msgPrint
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgPrint->bmpwin );
  GFL_BMP_Clear( bmp, 0xff );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(msgPrint->bmpwin) );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �r�b�g�}�b�v��`�N���A
 * @param msgPrint
 * @param x �������ݐ揑�����݊J�nX���W�i�h�b�g�j
 * @param y �������ݐ揑�����݊J�nY���W�i�h�b�g�j
 * @param size_x  �`��͈�X�T�C�Y�i�h�b�g�j
 * @param size_y  �`��͈�Y�T�C�Y�i�h�b�g�j
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_FillClearBmp(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 size_x, u32 size_y )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgPrint->bmpwin );
  GFL_BMP_Fill( bmp, x, y, size_x, size_y, 0xff );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(msgPrint->bmpwin) );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@PRINT_QUE�擾
 * @param msgPrint  FLDMSGPRINT
 * @retval  PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGPRINT_GetPrintQue( FLDMSGPRINT *msgPrint )
{
  return( msgPrint->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@GFL_FONT�擾
 * @param msgPrint  FLDMSGPRINT
 * @retval  GFL_FONT*
 */
//--------------------------------------------------------------
GFL_FONT * FLDMSGPRINT_GetFontHandle( FLDMSGPRINT *msgPrint )
{
  return( msgPrint->fontHandle );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@PRINT_UTIL�擾
 * @param msgPrint  FLDMSGPRINT
 * @retval  PRINT_UTIL*
 */
//--------------------------------------------------------------
PRINT_UTIL * FLDMSGPRINT_GetPrintUtil( FLDMSGPRINT *msgPrint )
{
  return( &msgPrint->printUtil );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@STRBUF�擾
 * @param msgPrint  FLDMSGPRINT
 * @retval  STRBUF*
 */
//--------------------------------------------------------------
STRBUF * FLDMSGPRINT_GetStrBuf( FLDMSGPRINT *msgPrint )
{
  return( msgPrint->strBuf );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT MSGDATA�擾
 * @param msgPrint  FLDMSGPRINT
 * @retval  MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * FLDMSGPRINT_GetMsgData( FLDMSGPRINT *msgPrint )
{
  return( msgPrint->msgData );
}

//======================================================================
//  FLDMSGWIN ���b�Z�[�W�E�B���h�E�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�ǉ��@���C��
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @param pltt_no     �g�p����p���b�g�ԍ�
 * @retval  FLDMSGWIN*
 */
//--------------------------------------------------------------
static FLDMSGWIN * fldmsgwin_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y, u16 pltt_no )
{
  FLDMSGWIN *msgWin;
  u8 frame = fmb->bgFrame;
  
  resetBG2ControlProc( fmb );
  
  if( pltt_no == PANO_FONT ){
    frame = fmb->bgFrameBld;
  }
  
  msgWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMSGWIN) );
  msgWin->fmb = fmb;
  msgWin->bmpwin = winframe_InitBmp( frame, fmb->heapID,
    bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, pltt_no );
  msgWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, msgWin->bmpwin );
  
  if( pltt_no == PANO_FONT ){
    winframe_SetPaletteBlack( fmb->heapID );
    setBlendAlpha( TRUE );
  }else{
    winframe_SetPaletteWhith( fmb->heapID );
  }
  
  return( msgWin );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�ǉ�
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval  FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDMSGWIN *msgWin;
  fmb->deriveFont_plttNo = PANO_FONT;
  msgWin = fldmsgwin_Add( fmb, msgData,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, fmb->deriveFont_plttNo );
  return( msgWin );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E �폜�B
 * FLDMSGWIN_Add()����msgData�͊e���ōs�����B
 * @param msgWin  FLDMSGWIN*
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_Delete( FLDMSGWIN *msgWin )
{
  winframe_DeleteBmp( msgWin->bmpwin );
  FLDMSGPRINT_Delete( msgWin->msgPrint );
  GFL_HEAP_FreeMemory( msgWin );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\��
 * @param msgWin  FLDMSGWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^ ������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_Print( FLDMSGWIN *msgWin, u16 x, u16 y, u32 strID )
{
  FLDMSGPRINT_Print( msgWin->msgPrint, x, y, strID );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(msgWin->bmpwin) );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��
 * @param msgWin  FLDMSGWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_PrintStrBuf( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf )
{
  FLDMSGPRINT_PrintStrBuf( msgWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��A�J���[�w�肠��
 * @param msgWin  FLDMSGWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @param color PRINTSYS_LSB
 * @retval  nothing
 */
//-------------------------------------------------------------
void FLDMSGWIN_PrintStrBufColor( FLDMSGWIN *msgWin,
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color )
{
  FLDMSGPRINT_PrintStrBufColor( msgWin->msgPrint, x, y, strBuf, color );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\�� GFL_FONT�w��
 * @param msgWin  FLDMSGWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_PrintStrBufColorFontHandle( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color, GFL_FONT *font_handle )
{
  FLDMSGPRINT_PrintStrBufColorFontHandle( msgWin->msgPrint, x, y, strBuf, color, font_handle );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�]���I���`�F�b�N
 * @param msgWin  FLDMSGWIN
 * @retval  BOOL  TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin )
{
  return( FLDMSGPRINT_CheckPrintTrans(msgWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\���N���A
 * @param msgWin  FLDMSGWIN
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_ClearWindow( FLDMSGWIN *msgWin )
{
  FLDMSGPRINT_ClearBmp( msgWin->msgPrint );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�w��̈�N���A
 * @param msgWin FLDMSGWIN
 * @param x �������ݐ揑�����݊J�nX���W�i�h�b�g�j
 * @param y �������ݐ揑�����݊J�nY���W�i�h�b�g�j
 * @param size_x  �`��͈�X�T�C�Y�i�h�b�g�j
 * @param size_y  �`��͈�Y�T�C�Y�i�h�b�g�j
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_FillClearWindow(
    FLDMSGWIN *msgWin, u32 x, u32 y, u32 size_x, u32 size_y )
{
  FLDMSGPRINT_FillClearBmp( msgWin->msgPrint, x, y, size_x, size_y );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�ǉ��@��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval  FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
  return( FLDMSGWIN_Add(fmb,msgData,1,19,30,4) );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@GFL_BMPWIN�擾
 * @param msgWin FLDMSGWIN
 * @retval  GFL_BMPWIN*
 */
//--------------------------------------------------------------
GFL_BMPWIN * FLDMSGWIN_GetBmpWin( FLDMSGWIN * fldmsgwin )
{
  return fldmsgwin->bmpwin;
}

//======================================================================
//  FLDSYSWIN �V�X�e���E�B���h�E�֘A
//======================================================================

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�ǉ�(�T�C�Y�͕W�����b�Z�[�W�g�d�l�Œ�B�\��Y�ʒu�̂ݎw��\)
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_y    //�\�����WY �L�����P��
 * @retval  FLDSYSWIN*
 */
//--------------------------------------------------------------
FLDSYSWIN * FLDSYSWIN_Add(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_y )
{
  return FLDSYSWIN_AddEx(
          fmb, msgData, SYSWIN_DEF_PX, bmppos_y, SYSWIN_DEF_SX, SYSWIN_DEF_SY );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�ǉ�(�ʒu/�T�C�Y�t���w���)
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval  FLDSYSWIN*
 */
//--------------------------------------------------------------
FLDSYSWIN * FLDSYSWIN_AddEx(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_x, u16 bmppos_y, u16 bmpsiz_x, u16 bmpsiz_y )
{
  FLDSYSWIN *sysWin;
  fmb->deriveFont_plttNo = PANO_FONT;
  resetBG2ControlProc( fmb );

  sysWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDSYSWIN) );
  sysWin->fmb = fmb;
  sysWin->bmpwin = syswin_InitBmp( bmppos_x, bmppos_y, bmpsiz_x, bmpsiz_y, fmb->heapID );
  sysWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, sysWin->bmpwin );
  
  setBlendAlpha( TRUE );

  return( sysWin );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E �폜�B
 * FLDSYSWIN_Add()����msgData�͊e���ōs�����B
 * @param sysWin  FLDSYSWIN*
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_Delete( FLDSYSWIN *sysWin )
{
  syswin_DeleteBmp( sysWin->bmpwin );
  FLDMSGPRINT_Delete( sysWin->msgPrint );
  GFL_HEAP_FreeMemory( sysWin );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\��
 * @param sysWin  FLDSYSWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^ ������ID
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_Print( FLDSYSWIN *sysWin, u16 x, u16 y, u32 strID )
{
  FLDMSGPRINT_Print( sysWin->msgPrint, x, y, strID );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(sysWin->bmpwin) );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��
 * @param sysWin  FLDSYSWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_PrintStrBuf( FLDSYSWIN *sysWin, u16 x, u16 y, STRBUF *strBuf )
{
  FLDMSGPRINT_PrintStrBuf( sysWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��A�J���[�w�肠��
 * @param sysWin  FLDSYSWIN
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @param color PRINTSYS_LSB
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_PrintStrBufColor( FLDSYSWIN *sysWin,
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color )
{
  FLDMSGPRINT_PrintStrBufColor( sysWin->msgPrint, x, y, strBuf, color );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�]���I���`�F�b�N
 * @param sysWin  FLDSYSWIN
 * @retval  BOOL  TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDSYSWIN_CheckPrintTrans( FLDSYSWIN *sysWin )
{
  return( FLDMSGPRINT_CheckPrintTrans(sysWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\���N���A
 * @param sysWin  FLDSYSWIN
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_ClearWindow( FLDSYSWIN *sysWin )
{
  FLDMSGPRINT_ClearBmp( sysWin->msgPrint );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�w��̈�N���A
 * @param sysWin FLDSYSWIN
 * @param x �������ݐ揑�����݊J�nX���W�i�h�b�g�j
 * @param y �������ݐ揑�����݊J�nY���W�i�h�b�g�j
 * @param size_x  �`��͈�X�T�C�Y�i�h�b�g�j
 * @param size_y  �`��͈�Y�T�C�Y�i�h�b�g�j
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_FillClearWindow(
    FLDSYSWIN *sysWin, u32 x, u32 y, u32 size_x, u32 size_y )
{
  FLDMSGPRINT_FillClearBmp( sysWin->msgPrint, x, y, size_x, size_y );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�ǉ��@��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval  FLDSYSWIN*
 */
//--------------------------------------------------------------
FLDSYSWIN * FLDSYSWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
  return( FLDSYSWIN_Add(fmb,msgData,19) );
}

//======================================================================
//  FLDMENUFUNC�@�t�B�[���h���j���[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ� ���C��
 * @param fmb FLDMSGBG
 * @param pMenuHead FLDMENUFUNC_HEADER
 * @param pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param pltt_no �g�p����p���b�g�ԍ�
 * @param call_back    BMPMENULIST_HEADER�̃J�[�\���ړ����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param work    BMPMENULIST_HEADER�ɃZ�b�g���郏�[�N(BmpMenuList_GetWorkPtr�Ŏ��o���BNULL��)
 * @retval  FLDMENUFUNC*
 */
//--------------------------------------------------------------
static FLDMENUFUNC * fldmenufunc_AddMenuList( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos, u16 pltt_no,
  BMPMENULIST_CURSOR_CALLBACK callback,
  BMPMENULIST_PRINT_CALLBACK icon,
  void *work )
{
  FLDMENUFUNC *menuFunc;
  BMPMENULIST_HEADER menuH;
  u8 frame = fmb->bgFrame;
  
  resetBG2ControlProc( fmb );

  if( pltt_no == PANO_FONT ){
    frame = fmb->bgFrameBld;
  }
  
  menuFunc = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMENUFUNC) );
  FldMenuFuncH_BmpMenuListH( &menuH, pMenuHead );
  
  menuFunc->fmb = fmb;
  menuFunc->pMenuListData = (BMP_MENULIST_DATA *)pMenuListData;
  
  menuFunc->bmpwin = winframe_InitBmp(
    frame, fmb->heapID,
    pMenuHead->bmppos_x, pMenuHead->bmppos_y,
    pMenuHead->bmpsize_x, pMenuHead->bmpsize_y, pltt_no );
  
  menuFunc->msgPrint = FLDMSGPRINT_SetupPrint(
      fmb, NULL, menuFunc->bmpwin );
  
  menuH.msgdata = NULL;
  menuH.print_util = FLDMSGPRINT_GetPrintUtil( menuFunc->msgPrint );
  menuH.print_que = FLDMSGPRINT_GetPrintQue( menuFunc->msgPrint );
  menuH.font_handle = fmb->fontHandle;
  menuH.win = menuFunc->bmpwin;
  menuH.list = (const BMP_MENULIST_DATA *)pMenuListData;
  menuH.icon = icon;
  menuH.call_back = callback;
  menuH.work = work;
  
  menuFunc->pMenuListWork =
    BmpMenuList_Set( &menuH, list_pos, cursor_pos, fmb->heapID );
//  BmpMenuList_SetCursorString( menuFunc->pMenuListWork, 0 );
  BmpMenuList_SetCursorBmp( menuFunc->pMenuListWork, fmb->heapID );
  
  if( pltt_no == PANO_FONT ){
    winframe_SetPaletteBlack( fmb->heapID );
    setBlendAlpha( TRUE );
  }else{
    winframe_SetPaletteWhith( fmb->heapID );
  }
  
  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ��@BMP���X�g��1��\����Ƃ̃R�[���o�b�N�w��L���
 * @param fmb FLDMSGBG
 * @param pMenuHead FLDMENUFUNC_HEADER
 * @param pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param work    BMPMENULIST_HEADER�ɃZ�b�g���郏�[�N(BmpMenuList_GetWorkPtr�Ŏ��o���BNULL��)
 * @retval  FLDMENUFUNC*
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddMenuListEx( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos,
  BMPMENULIST_CURSOR_CALLBACK callback,
  BMPMENULIST_PRINT_CALLBACK icon,
  void *work )
{
  FLDMENUFUNC *menuFunc;
  fmb->deriveFont_plttNo = PANO_FONT;
  menuFunc = fldmenufunc_AddMenuList( fmb, pMenuHead, pMenuListData,
    list_pos, cursor_pos, fmb->deriveFont_plttNo, callback, icon, work );
  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ�
 * @param fmb FLDMSGBG
 * @param pMenuHead FLDMENUFUNC_HEADER
 * @param pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @retval  FLDMENUFUNC*
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddMenuList( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos )
{
  return FLDMENUFUNC_AddMenuListEx(fmb, pMenuHead, pMenuListData, list_pos, cursor_pos, NULL, pMenuHead->icon, NULL);
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ��@���X�g�ʒu�A�J�[�\���ʒu�ȗ���
 * @param fmb FLDMSGBG
 * @param pMenuHead FLDMENUFUNC_HEADER
 * @param pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @retval  FLDMENUFUNC*
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddMenu( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData )
{
  return( FLDMENUFUNC_AddMenuList(fmb,pMenuHead,pMenuListData,0,0) );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ��@�C�x���g�p
 * @param fmb FLDMSGBG
 * @param pMenuHead FLDMENUFUNC_HEADER
 * @param pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param cancel TRUE=�L�����Z����
 * @retval  FLDMENUFUNC*
 * @note ���j���[�J���[�͔h���E�B���h�E�̃J���[���p������
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddEventMenuList( FLDMSGBG *fmb,
  const FLDMENUFUNC_HEADER *pMenuHead,
  FLDMENUFUNC_LISTDATA *pMenuListData,
  BMPMENULIST_CURSOR_CALLBACK callback, void* cb_work,
  u16 list_pos, u16 cursor_pos, BOOL cancel )
{
  FLDMENUFUNC *menuFunc;
  
  GF_ASSERT( fmb != NULL );
  GF_ASSERT( pMenuHead != NULL );
  GF_ASSERT( pMenuListData != NULL );
  
  menuFunc = fldmenufunc_AddMenuList( fmb, pMenuHead, pMenuListData,
    list_pos, cursor_pos, fmb->deriveFont_plttNo, callback, NULL, cb_work );
  
  if( cancel == FALSE ){
    BmpMenuList_SetCancelMode(
        menuFunc->pMenuListWork, BMPMENULIST_CANCELMODE_NOT );
  }

  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC  �폜
 * @param menuFunc  FLDMENUFUNC*
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc )
{
  BmpMenuList_Exit( menuFunc->pMenuListWork, NULL, NULL );
  BmpMenuWork_ListDelete( menuFunc->pMenuListData );
  winframe_DeleteBmp( menuFunc->bmpwin );
  FLDMSGPRINT_Delete( menuFunc->msgPrint );
  GFL_HEAP_FreeMemory( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[����
 * @param menuFunc  FLDMENUFUNC*
 * @retval  u32   ���X�g�I���ʒu�̃p�����[�^
 * FLDMENUFUNC_NULL = �I�𒆁BFLDMENUFUNC_CANCEL = �L�����Z��
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_ProcMenu( FLDMENUFUNC *menuFunc )
{
  u32 ret = BmpMenuList_Main( menuFunc->pMenuListWork );
  
  if( ret == BMPMENULIST_NULL ){
    return( FLDMENUFUNC_NULL );
  }
  
  if( ret == BMPMENULIST_CANCEL ){
    return( FLDMENUFUNC_CANCEL );
  }
  
  return( ret );
}

//==================================================================
/**
 * FLDMENUFUNC ���j���[�ĕ`��
 *
 * @param   menuFunc    FLDMENUFUNC*
 */
//==================================================================
void FLDMENUFUNC_Rewrite( FLDMENUFUNC *menuFunc )
{
  BmpMenuList_Rewrite( menuFunc->pMenuListWork );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA���쐬
 * @param max ���X�g��
 * @param heapID  �쐬�p�q�[�vID
 * @retval  FLDMENUFUNC_LISTDATA*
 */
//--------------------------------------------------------------
FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateListData( int max, HEAPID heapID )
{
  BMP_MENULIST_DATA *listData;
  listData = BmpMenuWork_ListCreate( max, heapID );
  return( (FLDMENUFUNC_LISTDATA*)listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA���폜
 * @param listData  FLDMENUFUNC_LISTDATA
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_DeleteListData( FLDMENUFUNC_LISTDATA *listData )
{
  BmpMenuWork_ListDelete( (BMP_MENULIST_DATA*)listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA���쐬��
 * FLDMENUFUNC_LIST���烁�j���[�\���p�������ǉ�����
 * @param menuList  FLDMENUFUNC_LIST
 * @param max     menuList�v�f��
 * @param msgData   �������
 * @param heapID    FLDMENUFUNC_LISTDATA�쐬�p�q�[�vID
 * @retval  FLDMENUFUNC_LISTDATA
 */
//--------------------------------------------------------------
FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateMakeListData(
  const FLDMENUFUNC_LIST *menuList,
  int max, GFL_MSGDATA *msgData, HEAPID heapID )
{
  int i;
  FLDMENUFUNC_LISTDATA *listData;
  
  listData = FLDMENUFUNC_CreateListData( max, heapID );
  
  for( i = 0; i < max; i++ ){
    BmpMenuWork_ListAddArchiveString(
      (BMP_MENULIST_DATA*)listData, msgData,
      menuList[i].str_id, (u32)menuList[i].selectParam, heapID );
  }
  
  return( listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�ɕ�����&�p�����[�^���Z�b�g
 * @param listData  FLDMENUFUNC_LISTDATA
 * @param strBuf    STRBUF
 * @param param   �p�����^
 * @param heapID    �q�[�vID
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_AddStringListData( FLDMENUFUNC_LISTDATA *listData,
    const STRBUF *strBuf, u32 param, HEAPID heapID  )
{ 
  BmpMenuWork_ListAddString(
    (BMP_MENULIST_DATA*)listData, strBuf, param, heapID );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�ɕ�����&�p�����[�^���Z�b�g
 * @param listData  FLDMENUFUNC_LISTDATA
 * @param strBuf    STRBUF
 * @param param   �p�����^
 * @param heapID    �q�[�vID
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_AddArcStringListData( FLDMENUFUNC_LISTDATA *listData,
    GFL_MSGDATA *msgData, u32 strID, u32 param, HEAPID heapID )
{ 
  BmpMenuWork_ListAddArchiveString(
    (BMP_MENULIST_DATA*)listData, msgData, strID, param, heapID );
}

//==================================================================
/**
 * FLDMENUFUNC_LISTDATA���烊�X�g�o�b�t�@��STRBUF��j������
 *
 * @param   listData    
 */
//==================================================================
void FLDMENUFUNC_ListSTRBUFDelete(FLDMENUFUNC_LISTDATA *listData)
{
  BmpMenuWork_ListSTRBUFDelete( (BMP_MENULIST_DATA*)listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�Ɋi�[����Ă��郊�X�g�̍ő啶�������擾
 * @param listData FLDMENUFUNC_LISTDATA
 * @retval u32 ���X�g���A�ő啶���� �h�b�g
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListLengthMax( FLDMSGBG *fmb,
    const FLDMENUFUNC_LISTDATA *listData, int *no_buf, u32 space )
{
  u32 len, max = 0, no = 0, max_no = 0;
  const BMP_MENULIST_DATA* list = (const BMP_MENULIST_DATA*)listData;
  
  while( list->str != LIST_ENDCODE )
  {
    if( list->str == NULL )
    {
      break;
    }
    
    len = PRINTSYS_GetStrWidth(
        (const STRBUF*)list->str, fmb->fontHandle, space );
    
    if( len > max )
    {
      max = len;
      max_no = no;
    }
  
    list++;
    no++;
  }
  
  *no_buf = max_no;
  return( max );
}

#if 0 //old
//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�Ɋi�[����Ă��郊�X�g�̍ő啶�������擾
 * @param listData FLDMENUFUNC_LISTDATA
 * @retval u32 ���X�g���A�ő啶����
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListLengthMax( const FLDMENUFUNC_LISTDATA *listData, int *no_buf )
{
  u32 len;
  len = BmpMenuWork_GetListMaxLength(
      (const BMP_MENULIST_DATA*)listData, no_buf );
  return( len );
}
#endif

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�Ɋi�[����Ă��郊�X�g�����擾
 * @param listData FLDMENUFUNC_LISTDATA
 * @retval u32 ���X�g���A�ő啶����
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListMax( const FLDMENUFUNC_LISTDATA *listData )
{
  u32 len;
  len = BmpMenuWork_GetListMax( (const BMP_MENULIST_DATA*)listData );
  return( len );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�Ɋi�[����Ă��郊�X�g�̍ő啶��������
 * �K�v�ȃ��j���[�������擾�B
 * @param fmb FLDMSGBG
 * @param listData FLDMENUFUNC_LISTDATA
 * @param font_size �t�H���g���T�C�Y <-miteinai
 * @param space ������̕\���Ԋu
 * @retval u32 ���j���[���� �L�����P��
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListMenuWidth( FLDMSGBG *fmb,
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 space )
{
  int no;
  u32 c;
  u32 len = FLDMENUFUNC_GetListLengthMax( fmb, listData, &no, space );
  
  len += 16 + space; //�J�[�\����
  c = len / 8;
  if( (len & 0x07) ){ c++; }
  return( c );
}

#if 0
u32 FLDMENUFUNC_GetListMenuWidth(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 space )
{
  int no;
  u32 c,len;
  u32 num = FLDMENUFUNC_GetListLengthMax( listData, &no );
  num++; //�J�[�\����
  len = num * font_size;
  
  c = len / 8;
  if( (len & 0x07) ){ c++; }
  if( space ){ c += ((num*space)/8)+1; }
  return( c );
}
#endif

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA�Ɋi�[����Ă��郊�X�g������
 * �K�v�ȃ��j���[�c�����擾�B
 * @param listData FLDMENUFUNC_LISTDATA
 * @param font_size �t�H���g�c�T�C�Y
 * @param space ��̕\���Ԋu
 * @retval u32 ���j���[�c�� �L�����P��
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListMenuHeight(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 space )
{
  u32 c,len;
  u32 num = FLDMENUFUNC_GetListMax( listData );
  len = num * font_size;
  c = len / 8;
  if( (len & 0x07) ){ c++; }
  if( space ){ c += ((num*space)/8)+1; }
  return( c );
}

//--------------------------------------------------------------
/**
 * �w�荀�ڐ�����
 * �K�v�ȃ��j���[�����擾�B
 * @param num
 * @param font_size �t�H���g�c�T�C�Y
 * @param space ��̕\���Ԋu
 * @retval u32 ���j���[�� �L�����P��
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListMenuLen( u32 num, u32 font_size, u32 space )
{
  u32 c,len;
  len = num * font_size;
  c = len / 8;
  if( (len & 0x07) ){ c++; }
  if( space ){ c += ((num*space)/8)+1; }
  return( c );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_HEADER�Ƀ��X�g���A���W�n����ǉ�
 * @param head  FLDMENUFUNC_HEADER
 * @param list_count  ���X�g��
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval  nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_InputHeaderListSize(
  FLDMENUFUNC_HEADER *head, u16 count,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  head->count = count;
  head->bmppos_x = bmppos_x;
  head->bmppos_y = bmppos_y;
  head->bmpsize_x = bmpsize_x;
  head->bmpsize_y = bmpsize_y;
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_HEADER -> BMPMENULIST_HEADER
 * @param menuH BMPMENULIST_HEADER
 * @param fmenuH  FLDMENUFUNC_HEADER
 * @retval  nothing
 */
//--------------------------------------------------------------
static void FldMenuFuncH_BmpMenuListH(
  BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH )
{
  GFL_STD_MemClear( menuH, sizeof(BMPMENULIST_HEADER) );
  menuH->count = fmenuH->count;     //���X�g���ڐ�
  menuH->line = fmenuH->line;       //�\���ő區�ڐ�
  menuH->label_x = fmenuH->label_x;   //���x���\���w���W
  menuH->data_x = fmenuH->data_x;     //���ڕ\���w���W
  menuH->cursor_x = fmenuH->cursor_x;   //�J�[�\���\���w���W
  menuH->line_y = fmenuH->line_y;     //�\���x���W
  menuH->f_col = fmenuH->f_col;     //�\�������F
  menuH->b_col = fmenuH->b_col;     //�\���w�i�F
  menuH->s_col = fmenuH->s_col;     //�\�������e�F
  menuH->msg_spc = fmenuH->msg_spc;   //�����Ԋu�w
  menuH->line_spc = fmenuH->line_spc;   //�����Ԋu�x
  menuH->page_skip = fmenuH->page_skip; //�y�[�W�X�L�b�v�^�C�v
  menuH->font_size_x = fmenuH->font_size_x; //�����T�C�YX(�h�b�g
  menuH->font_size_y = fmenuH->font_size_y; //�����T�C�YY(�h�b�g
  menuH->icon = fmenuH->icon;
}

//======================================================================
//  �͂��A�������I�����j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �͂��A�������I�����j���[�@�ǉ�
 * @param fmb FLDMSGBG
 * @param pos FLDMENUFUNC_YESNO �J�[�\�������ʒu
 * @retval FLDMENUFUNC*
 * ���j���[�폜�̍ۂ�FLDMENUFUNC_DeleteMenu()���ĂԎ��B
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddYesNoMenu(
    FLDMSGBG *fmb, FLDMENUFUNC_YESNO cursor_pos )
{
  u32 max = 2;
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  FLDMENUFUNC_LISTDATA *listData;
  FLDMENUFUNC_HEADER menuH = DATA_MenuHeader_YesNo;
  const FLDMENUFUNC_LIST menuList[2] = {
    { msgid_yesno_yes, (void*)0 }, { msgid_yesno_no, (void*)1 }, };
  
  KAGAYA_Printf( "fmb HEAPID = %d\n", fmb->heapID );

  msgData = FLDMSGBG_CreateMSGDATA( fmb, NARC_message_yesnomenu_dat );
  listData = FLDMENUFUNC_CreateMakeListData(
            menuList, max, msgData, fmb->heapID );
  GFL_MSG_Delete( msgData );
  
  FLDMENUFUNC_InputHeaderListSize( &menuH, max, 24, 10, 7, 4 );
  menuFunc = fldmenufunc_AddMenuList( fmb, &menuH, listData,
      0, cursor_pos, fmb->deriveFont_plttNo, NULL, NULL, NULL );
  
#if 0 //B�L�����Z������
  BmpMenuList_SetCancelMode(
      menuFunc->pMenuListWork, BMPMENULIST_CANCELMODE_NOT );
#else //B�L�����Z���L��
  BmpMenuList_SetCancelMode(
      menuFunc->pMenuListWork, BMPMENULIST_CANCELMODE_USE );
#endif

  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * �͂��A�������I�����j���[ ����
 * @param menuFunc FLDMENUFUNC*
 * @retval u32 FLDMENUFUNC_YESNO
 */
//--------------------------------------------------------------
FLDMENUFUNC_YESNO FLDMENUFUNC_ProcYesNoMenu( FLDMENUFUNC *menuFunc )
{
  u32 ret = FLDMENUFUNC_ProcMenu( menuFunc );
  
  if( ret == FLDMENUFUNC_NULL ){
    return( FLDMENUFUNC_YESNO_NULL ); //����
  }
  
  if( ret == FLDMENUFUNC_CANCEL ){ //�L�����Z����NO��
    return( FLDMENUFUNC_YESNO_NO );
  }
  
  return( ret );
}

//======================================================================
//  FLDMSGPRINT_STREAM ���b�Z�[�W�@�v�����g�X�g���[���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGPRINT_STREAM �v�����g�ݒ�@�J���[�w�肠��
 * @param fmb FLDMSGBG
 * @param strbuf �\������STRBUF
 * @param bmpwin �\�����鏉�����ς݂�GFL_BMPWIN
 * @param x �`��J�nX���W(�h�b�g)
 * @param y �`��J�nY���W(�h�b�g)
 * @param   wait    �P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @retval FLDMSGPRINT_STREAM
 */
//--------------------------------------------------------------
FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrintColor(
  FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait, u16 n_color )
{
  FLDMSGPRINT_STREAM *stm = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGPRINT_STREAM) );
  
  stm->fmb = fmb;
  stm->msg_wait = wait;
  
  DEBUG_AddCountPrintTCB( fmb );
  
  stm->printStream = PRINTSYS_PrintStream(
      bmpwin, x, y,
      strbuf, fmb->fontHandle,
      wait,
      fmb->printTCBLSys, 0,
      fmb->heapID, n_color );

  // �L�[����Ǘ��J�n
  Control_StartPrint( &stm->fmb->print_cont );

  return( stm );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT_STREAM �v�����g�ݒ�
 * @param fmb FLDMSGBG
 * @param strbuf �\������STRBUF
 * @param bmpwin �\�����鏉�����ς݂�GFL_BMPWIN
 * @param x �`��J�nX���W(�h�b�g)
 * @param y �`��J�nY���W(�h�b�g)
 * @param   wait    �P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @retval FLDMSGPRINT_STREAM
 */
//--------------------------------------------------------------
FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrint(
  FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait )
{
  return( FLDMSGPRINT_STREAM_SetupPrintColor(
        fmb,strbuf,bmpwin,x,y,wait,0x0f) );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT_STREAM �폜
 * @param stm FLDMSGPRINT_STREAM*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_STREAM_Delete( FLDMSGPRINT_STREAM *stm )
{
  DEBUG_SubCountPrintTCB( stm->fmb );
  PRINTSYS_PrintStreamDelete( stm->printStream );
  GFL_HEAP_FreeMemory( stm );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT_STREAM �v�����g
 * @param stm 
 * @retval PRINTSTREAM_STATE
 */
//--------------------------------------------------------------
static PRINTSTREAM_STATE fldMsgPrintStream_ProcPrint(
    FLDMSGPRINT_STREAM *stm )
{
  PRINTSTREAM_STATE state;
  
  state = PRINTSYS_PrintStreamGetState( stm->printStream );
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( Control_GetSkipKey( &stm->fmb->print_cont ) ){
      set_printStreamTempSpeed( stm->printStream, TRUE );
    }else{
      set_printStreamTempSpeed( stm->printStream, FALSE );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��

    if( Control_GetWaitKey( &stm->fmb->print_cont, PRINTSYS_PrintStreamGetPauseType( stm->printStream ) ) ){
      if( Control_GetAutoPrintFlag(&stm->fmb->print_cont) == FALSE ){//�I�[�g�\�����͉����o���Ȃ��B
        PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      }
      PRINTSYS_PrintStreamReleasePause( stm->printStream );
      set_printStreamTempSpeed( stm->printStream, FALSE );
      state = PRINTSTREAM_STATE_RUNNING; //�� RUNNING�ŕԂ�
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    break;
  }
  
  return( state );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT_STREAM �v�����g
 * @param stm 
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FLDMSGPRINT_STREAM_ProcPrint( FLDMSGPRINT_STREAM *stm )
{
  PRINTSTREAM_STATE state;
  state = fldMsgPrintStream_ProcPrint( stm );
  
  if( state == PRINTSTREAM_STATE_DONE ){ //�I��
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  FLDMSGWIN_STREAM
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�ǉ�
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval FLDMSGWIN_STREAM*
 */
//--------------------------------------------------------------
FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_Add(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDMSGWIN_STREAM *msgWin;
  
  fmb->deriveFont_plttNo = PANO_FONT;
  
  resetBG2ControlProc( fmb );

  msgWin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGWIN_STREAM) );
  msgWin->fmb = fmb;
  msgWin->msgData = msgData;
  msgWin->bmpwin = winframe_InitBmp( fmb->bgFrameBld, fmb->heapID,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, fmb->deriveFont_plttNo );
  msgWin->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
  
  FLDKEYWAITCURSOR_Init( &msgWin->cursor_work, fmb->heapID );

  winframe_SetPaletteBlack( fmb->heapID );
  setBlendAlpha( TRUE );
  return( msgWin );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E �폜
 * FLDMSGWIN_STREAM_Add()����msgData�͊e���ōs�����B
 * @param msgWin FLDMSGWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_Delete( FLDMSGWIN_STREAM *msgWin )
{
  winframe_DeleteBmp( msgWin->bmpwin );
  
  if( msgWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( msgWin->msgPrintStream );
  }
  
  if( msgWin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( msgWin->msgPrint );
  }
  
  GFL_STR_DeleteBuffer( msgWin->strBuf );
  FLDKEYWAITCURSOR_Finish( &msgWin->cursor_work );
  GFL_HEAP_FreeMemory( msgWin );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�\���J�n
 * @param msgWin FLDMSGWIN_STREAM*
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^�@������ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_PrintStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, u32 strID )
{
  if( msgWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( msgWin->msgPrintStream );
  }
  
  GF_ASSERT( msgWin->msgData );
  GFL_MSG_GetString( msgWin->msgData, strID, msgWin->strBuf );
  
  msgWin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    msgWin->fmb, msgWin->strBuf, msgWin->bmpwin, x, y, Control_GetMsgWait( &msgWin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param msgWin FLDMSGWIN_STREAM*
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_PrintStrBufStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, const STRBUF *strBuf )
{
  if( msgWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( msgWin->msgPrintStream );
  }
  
  msgWin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    msgWin->fmb, strBuf, msgWin->bmpwin, x, y, Control_GetMsgWait( &msgWin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�\��
 * @param msgWin FLDMSGWIN_STREAM*
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL FLDMSGWIN_STREAM_Print( FLDMSGWIN_STREAM *msgWin )
{
  int trg,cont;
  PRINTSTREAM_STATE state;
  state = fldMsgPrintStream_ProcPrint( msgWin->msgPrintStream );
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( FLDKEYWAITCURSOR_GetState(&msgWin->cursor_work) == CURSOR_STATE_WRITE ){
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgWin->bmpwin );
      FLDKEYWAITCURSOR_Clear( &msgWin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( msgWin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    {
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgWin->bmpwin );
      FLDKEYWAITCURSOR_Write( &msgWin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( msgWin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    return( TRUE );
  }

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�ǉ� ��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval FLDMSGWIN_STREAM
 */
//--------------------------------------------------------------
FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_AddTalkWin(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
  return( FLDMSGWIN_STREAM_Add(fmb,msgData,1,19,30,4) );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�N���A
 * @param msgWin FLDMSGWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_ClearMessage( FLDMSGWIN_STREAM *msgWin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgWin->bmpwin );
  GFL_BMP_Clear( bmp, 0xff);
  GFL_BG_LoadScreenReq( msgWin->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E �E�B���h�E�N���A
 * @param msgWin FLDMSGWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_ClearWindow( FLDMSGWIN_STREAM *msgWin )
{
  BmpWinFrame_Clear( msgWin->bmpwin, 0 );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E �E�B���h�E�`��
 * @param msgWin FLDMSGWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_WriteWindow( FLDMSGWIN_STREAM *msgWin )
{
  BmpWinFrame_Write( msgWin->bmpwin,
      WINDOW_TRANS_ON, 1, PANO_MENU_B );
  FLDMSGWIN_STREAM_ClearMessage( msgWin );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�ꊇ�\��
 * @param msgWin FLDMSGWIN_STREAM
 * @param x X�\�����W
 * @param y Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_STREAM_AllPrintStrBuf(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, STRBUF *strBuf )
{
  if( msgWin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( msgWin->msgPrint );
  }
  
  msgWin->msgPrint = FLDMSGPRINT_SetupPrint(
      msgWin->fmb, NULL, msgWin->bmpwin );
  
  FLDMSGPRINT_PrintStrBuf( msgWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�ꊇ�\���@�]���`�F�b�N
 * @param msgWin FLDMSGWIN_STREAM
 * @retval BOOL TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDMSGWIN_STREAM_CheckAllPrintTrans( FLDMSGWIN_STREAM *msgWin )
{
  return( FLDMSGPRINT_CheckPrintTrans(msgWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�@BMPWIN�擾
 * @param msgWin FLDMSGWIN_STREAM
 * @retval GFL_BMPWIN
 */
//--------------------------------------------------------------
GFL_BMPWIN * FLDMSGWIN_STREAM_GetBmpWin( FLDMSGWIN_STREAM *msgWin )
{
	return msgWin->bmpwin;
}

//======================================================================
//  FLDSYSWIN_STREAM
//======================================================================
//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�ǉ�
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_y    //�\�����WY �L�����P��
 * @retval FLDSYSWIN_STREAM*
 */
//--------------------------------------------------------------
FLDSYSWIN_STREAM * FLDSYSWIN_STREAM_Add(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_y )
{
  FLDSYSWIN_STREAM *sysWin;
  
  fmb->deriveFont_plttNo = PANO_FONT;
  resetBG2ControlProc( fmb );
  
  sysWin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDSYSWIN_STREAM) );
  sysWin->fmb = fmb;
  sysWin->msgData = msgData;
  sysWin->bmpwin = syswin_InitBmp( SYSWIN_DEF_PX, bmppos_y, SYSWIN_DEF_SX, SYSWIN_DEF_SY, fmb->heapID );
  sysWin->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
  
  FLDKEYWAITCURSOR_Init( &sysWin->cursor_work, fmb->heapID );
  
  winframe_SetPaletteBlack( fmb->heapID );
  setBlendAlpha( TRUE );
  return( sysWin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E �폜
 * FLDSYSWIN_STREAM_Add()����msgData�͊e���ōs�����B
 * @param sysWin FLDSYSWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_Delete( FLDSYSWIN_STREAM *sysWin )
{
  syswin_DeleteBmp( sysWin->bmpwin );
  
  if( sysWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( sysWin->msgPrintStream );
  }
  
  if( sysWin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( sysWin->msgPrint );
  }
  
  GFL_STR_DeleteBuffer( sysWin->strBuf );
  FLDKEYWAITCURSOR_Finish( &sysWin->cursor_work );
  GFL_HEAP_FreeMemory( sysWin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�\���J�n
 * @param sysWin FLDSYSWIN_STREAM*
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^�@������ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_PrintStart(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, u32 strID )
{
  if( sysWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( sysWin->msgPrintStream );
  }
  
  GF_ASSERT( sysWin->msgData );
  GFL_MSG_GetString( sysWin->msgData, strID, sysWin->strBuf );
  
  sysWin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    sysWin->fmb, sysWin->strBuf, sysWin->bmpwin, x, y, Control_GetMsgWait( &sysWin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param sysWin FLDSYSWIN_STREAM*
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_PrintStrBufStart(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, const STRBUF *strBuf )
{
  if( sysWin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( sysWin->msgPrintStream );
  }
  
  sysWin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    sysWin->fmb, strBuf, sysWin->bmpwin, x, y, Control_GetMsgWait( &sysWin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�\��
 * @param sysWin FLDSYSWIN_STREAM*
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL FLDSYSWIN_STREAM_Print( FLDSYSWIN_STREAM *sysWin )
{
  int trg,cont;
  PRINTSTREAM_STATE state;
  state = fldMsgPrintStream_ProcPrint( sysWin->msgPrintStream );
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( FLDKEYWAITCURSOR_GetState(&sysWin->cursor_work) == CURSOR_STATE_WRITE ){
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( sysWin->bmpwin );
      FLDKEYWAITCURSOR_Clear( &sysWin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( sysWin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    {
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( sysWin->bmpwin );
      FLDKEYWAITCURSOR_Write( &sysWin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( sysWin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    return( TRUE );
  }

  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E �J�[�\���\���̂�
 * @param sysWin FLDSYSWIN_STREAM*
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
void FLDSYSWIN_WriteKeyWaitCursor( FLDSYSWIN_STREAM *sysWin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( sysWin->bmpwin );
  FLDKEYWAITCURSOR_Write( &sysWin->cursor_work, bmp, 0x0f );
  GFL_BMPWIN_TransVramCharacter( sysWin->bmpwin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�ǉ� ��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval FLDSYSWIN_STREAM
 */
//--------------------------------------------------------------
FLDSYSWIN_STREAM * FLDSYSWIN_STREAM_AddTalkWin(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
  return( FLDSYSWIN_STREAM_Add(fmb,msgData,19) );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�N���A
 * @param sysWin FLDSYSWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_ClearMessage( FLDSYSWIN_STREAM *sysWin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( sysWin->bmpwin );
  GFL_BMP_Clear( bmp, 0xff);
  GFL_BG_LoadScreenReq( sysWin->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E �E�B���h�E�N���A
 * @param sysWin FLDSYSWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_ClearWindow( FLDSYSWIN_STREAM *sysWin )
{
  syswin_ClearBmp( sysWin->bmpwin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E �E�B���h�E�`��
 * @param sysWin FLDSYSWIN_STREAM
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_WriteWindow( FLDSYSWIN_STREAM *sysWin )
{
  syswin_WriteWindow( sysWin->bmpwin );
  FLDSYSWIN_STREAM_ClearMessage( sysWin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�@���b�Z�[�W�ꊇ�\��
 * @param sysWin FLDSYSWIN_STREAM
 * @param x X�\�����W
 * @param y Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_STREAM_AllPrintStrBuf(
    FLDSYSWIN_STREAM *sysWin, u16 x, u16 y, STRBUF *strBuf )
{
  if( sysWin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( sysWin->msgPrint );
  }
  
  sysWin->msgPrint = FLDMSGPRINT_SetupPrint(
      sysWin->fmb, NULL, sysWin->bmpwin );
  
  FLDMSGPRINT_PrintStrBuf( sysWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�@���b�Z�[�W�ꊇ�\���@�]���`�F�b�N
 * @param sysWin FLDSYSWIN_STREAM
 * @retval BOOL TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDSYSWIN_STREAM_CheckAllPrintTrans( FLDSYSWIN_STREAM *sysWin )
{
  return( FLDMSGPRINT_CheckPrintTrans(sysWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�@GFL_BMPWIN�擾
 * @param sysWin FLDSYSWIN_STREAM
 * @retval GFL_BMPWIN*
 */
//--------------------------------------------------------------
GFL_BMPWIN * FLDSYSWIN_STREAM_GetBmpWin( FLDSYSWIN_STREAM *sysWin )
{
  return( sysWin->bmpwin );
}

//======================================================================
//  FLDTALKMSGWIN
//======================================================================
//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param idx FLDTALKMSGWIN_IDX
 * @param strBuf �\������STRBUF
 * @param pos �����o�������W
 * @retval FLDTALKMSGWIN*
 * @note ����pos,strBuf�͕\�����A��ɎQ�Ƃ����̂�
 * �폜�����܂ŕێ����ĉ�����
 */
//--------------------------------------------------------------
static void fldTalkMsgWin_Add(
    FLDMSGBG *fmb, FLDTALKMSGWIN *tmsg,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos, STRBUF *strBuf,
    TALKMSGWIN_TYPE type, TAIL_SETPAT tail )
{
  GF_ASSERT( fmb->talkMsgWinSys != NULL );
  
  //--- debug
  tmsg->watch_pos = pos;
  tmsg->d_pos = *pos;
  //---

  fmb->deriveFont_plttNo = PANO_FONT_TALKMSGWIN;

  tmsg->fmb = fmb;
  
  tmsg->talkMsgWinSys = fmb->talkMsgWinSys;
  tmsg->talkMsgWinIdx = idx;
  
  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );
#ifndef DEBUG_ONLY_FOR_kagaya
  switch( idx ){
  case FLDTALKMSGWIN_IDX_UPPER:
    TALKMSGWIN_CreateFixWindowUpper( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_UPPER, (VecFx32*)pos, strBuf, 15,
        type, tail, Control_GetMsgWait( &fmb->print_cont ) );
    break;
  case FLDTALKMSGWIN_IDX_LOWER:
    TALKMSGWIN_CreateFixWindowLower( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_LOWER, (VecFx32*)pos, strBuf, 15,
        type, tail, Control_GetMsgWait( &fmb->print_cont ) );
    break;
  default:
    TALKMSGWIN_CreateFixWindowAuto( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_AUTO, (VecFx32*)pos, strBuf, 15, type, 
        Control_GetMsgWait( &fmb->print_cont ) );
  }
#else //debug
  switch( idx ){
  case FLDTALKMSGWIN_IDX_UPPER:
    TALKMSGWIN_CreateFixWindowUpper( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_UPPER, &tmsg->d_pos, strBuf, 15,
        type, tail, Control_GetMsgWait( &fmb->print_cont ) );
    break;
  case FLDTALKMSGWIN_IDX_LOWER:
    TALKMSGWIN_CreateFixWindowLower( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_LOWER, &tmsg->d_pos, strBuf, 15,
        type, tail, Control_GetMsgWait( &fmb->print_cont ) );
    break;
  default:
    TALKMSGWIN_CreateFixWindowAuto( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_AUTO, &tmsg->d_pos, strBuf, 15, type, 
        Control_GetMsgWait( &fmb->print_cont ) );
  }
#endif
  Control_StartPrint( &fmb->print_cont );
  
  TALKMSGWIN_OpenWindow( tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  
  FLDKEYWAITCURSOR_Init( &tmsg->cursor_work, fmb->heapID );
  
  if( type == TALKMSGWIN_TYPE_GIZA ){
    tmsg->shake_y = GIZA_SHAKE_Y;
    PMSND_PlaySystemSE( SEQ_SE_SYS_58 );
  }
  
  KAGAYA_Printf( "�E�B���h�E %d,%d,%d\n", pos->x, pos->y, pos->z );
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param idx FLDTALKMSGWIN_TYPE
 * @param pos �����o�������W
 * @param msgData �g�p����GFL_MSGDATA
 * @param msgID ���b�Z�[�W �A�[�J�C�u�f�[�^ID
 * @retval FLDTALKMSGWIN*
 * @note ����pos,strBuf�͕\�����A��ɎQ�Ƃ����̂�
 * �폜�����܂ŕێ����ĉ�����
 */
//--------------------------------------------------------------
FLDTALKMSGWIN * FLDTALKMSGWIN_Add( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    GFL_MSGDATA *msgData, u32 msgID,
    TALKMSGWIN_TYPE type, TAIL_SETPAT tail )
{
  if( idx < FLDTALKMSGWIN_MAX ){
    FLDTALKMSGWIN *tmsg = &fmb->balloonMsgWinTbl[idx];
    
    if( tmsg->talkMsgWinSys == NULL ){
      tmsg->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
      GFL_MSG_GetString( msgData, msgID, tmsg->strBuf );
      fldTalkMsgWin_Add( fmb, tmsg, idx, pos, tmsg->strBuf, type, tail );
      return( tmsg );
    }
  }
  
  GF_ASSERT( 0 );
  return( NULL );
}


#if 0 //old
FLDTALKMSGWIN * FLDTALKMSGWIN_Add( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    GFL_MSGDATA *msgData, u32 msgID,
    TALKMSGWIN_TYPE type, TAIL_SETPAT tail )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  tmsg->strBuf = GFL_STR_CreateBuffer(
          FLDMSGBG_STRLEN, fmb->heapID );
  
  GFL_MSG_GetString( msgData, msgID, tmsg->strBuf );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, tmsg->strBuf, type, tail );
  return( tmsg );
}
#endif

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param idx FLDTALKMSGWIN_TYPE
 * @param strBuf �\������STRBUF
 * @param pos �����o�������W
 * @retval FLDTALKMSGWIN*
 * @note ����pos,strBuf�͕\�����A��ɎQ�Ƃ����̂�
 * �폜�����܂ŕێ����ĉ�����
 */
//--------------------------------------------------------------
FLDTALKMSGWIN * FLDTALKMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    STRBUF *strBuf, TALKMSGWIN_TYPE type, TAIL_SETPAT tail )
{
  if( idx < FLDTALKMSGWIN_MAX ){
    FLDTALKMSGWIN *tmsg = &fmb->balloonMsgWinTbl[idx];
    
    if( tmsg->talkMsgWinSys == NULL ){
      fldTalkMsgWin_Add( fmb, tmsg, idx, pos, strBuf, type, tail );
      return( tmsg );
    }
  }
  
  GF_ASSERT( 0 );
  return( NULL );
}

#if 0 //old
FLDTALKMSGWIN * FLDTALKMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    STRBUF *strBuf, TALKMSGWIN_TYPE type, TAIL_SETPAT tail )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, strBuf, type, tail );
  return( tmsg );
}
#endif

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E����J�n
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDTALKMSGWIN_StartClose( FLDTALKMSGWIN *tmsg )
{ 
  if( tmsg->strBuf != NULL ){
    GFL_STR_DeleteBuffer( tmsg->strBuf );
  }
  
  FLDKEYWAITCURSOR_Finish( &tmsg->cursor_work );
  TALKMSGWIN_CloseWindow( tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E����@�҂�
 * @param tmsg FLDTALKMSGWIN
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
BOOL FLDTALKMSGWIN_WaitClose( FLDTALKMSGWIN *tmsg )
{
#if 0
  if( TALKMSGWIN_CheckCloseStatus(
        tmsg->talkMsgWinSys,tmsg->talkMsgWinIdx) ){
    GFL_HEAP_FreeMemory( tmsg );
    return( TRUE );
  }
  
  return( FALSE );
#else
  if( TALKMSGWIN_CheckCloseStatus(
        tmsg->talkMsgWinSys,tmsg->talkMsgWinIdx) ){
    MI_CpuClear8( tmsg, sizeof(FLDTALKMSGWIN) );
    return( TRUE );
  }
  
  return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E�폜
 * @param tmsg FLDTALKMSGWIN
 * @retval BOOL TRUE=����
 */
//--------------------------------------------------------------
void FLDTALKMSGWIN_Delete( FLDTALKMSGWIN *tmsg )
{
  if( tmsg->strBuf != NULL ){
    GFL_STR_DeleteBuffer( tmsg->strBuf );
  }
  
  TALKMSGWIN_DeleteWindow( tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  FLDKEYWAITCURSOR_Finish( &tmsg->cursor_work );
  MI_CpuClear8( tmsg, sizeof(FLDTALKMSGWIN) );
}

#if 0
//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E ���b�Z�[�W�N���A
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDTALKMSGWIN_ClearMessage( FLDTALKMSGWIN *tmsg, STRBUF *strBuf )
{
  GFL_BMPWIN *bmpwin = TALKMSGWIN_GetBmpWin(
      tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  TALKMSGWIN_ClearBmpWindow( tmsg->talkMsgWinSys, bmpwin );
}
#endif

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E ���b�Z�[�W���Z�b�g
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDTALKMSGWIN_ResetMessageStrBuf( FLDTALKMSGWIN *tmsg, STRBUF *strbuf )
{
  GFL_BMPWIN *bmpwin = TALKMSGWIN_GetBmpWin(
      tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( bmpwin );
  GFL_BMP_Clear( bmp, 0xff );
  TALKMSGWIN_ResetMessage(
    tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx, strbuf, Control_GetMsgWait( &tmsg->fmb->print_cont ) );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(bmpwin) );
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E �\��
 * @param tmsg FLDTALKMSGWIN
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL FLDTALKMSGWIN_Print( FLDTALKMSGWIN *tmsg )
{
  PRINTSTREAM_STATE state;
  PRINT_STREAM *stream;
  PRINTSTREAM_PAUSE_TYPE pause_type;

  if( TALKMSGWIN_CheckPrintOn(
        tmsg->talkMsgWinSys,tmsg->talkMsgWinIdx) == FALSE ){
    return( FALSE );
  }

  stream = TALKMSGWIN_GetPrintStream(
      tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  state = PRINTSYS_PrintStreamGetState( stream );
  
  { //shake
    GFL_BG_SetScroll(
        FLDMSGBG_BGFRAME, GFL_BG_SCROLL_Y_SET, tmsg->shake_y );
    
    if( tmsg->shake_y < 0 ){
      tmsg->shake_y += 2;
      
      if( tmsg->shake_y > 0 ){
        tmsg->shake_y = 0;
      }
    }

    tmsg->shake_y = -tmsg->shake_y;
  }
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( FLDKEYWAITCURSOR_GetState(&tmsg->cursor_work) == CURSOR_STATE_WRITE ){
      GFL_BMPWIN *twin_bmp = TALKMSGWIN_GetBmpWin(
          tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( twin_bmp );
      FLDKEYWAITCURSOR_Clear( &tmsg->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( twin_bmp );
    }
    
    if( Control_GetSkipKey( &tmsg->fmb->print_cont ) ){
      set_printStreamTempSpeed( stream, TRUE );
    }else{
      set_printStreamTempSpeed( stream, FALSE );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    { 
      GFL_BMPWIN *twin_bmp = TALKMSGWIN_GetBmpWin(
          tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( twin_bmp );
      
      if( Control_GetAutoPrintFlag(&tmsg->fmb->print_cont) == FALSE ){//�I�[�g�\�����̓L�[�J�[�\���������Ȃ��B
        FLDKEYWAITCURSOR_Write( &tmsg->cursor_work, bmp, 0x0f );
      }

      if( Control_GetWaitKey( &tmsg->fmb->print_cont, PRINTSYS_PrintStreamGetPauseType( stream ) ) ){
        if( Control_GetAutoPrintFlag(&tmsg->fmb->print_cont) == FALSE ){//�I�[�g�\�����͉����o���Ȃ��B
          PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
        }
        PRINTSYS_PrintStreamReleasePause( stream );
        FLDKEYWAITCURSOR_Clear( &tmsg->cursor_work, bmp, 0x0f );
        set_printStreamTempSpeed( stream, FALSE );
      }
      
      GFL_BMPWIN_TransVramCharacter( twin_bmp );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    if( tmsg->shake_y ){ //shake
      tmsg->shake_y = 0;
      GFL_BG_SetScroll(
          FLDMSGBG_BGFRAME, GFL_BG_SCROLL_Y_SET, tmsg->shake_y );
    }
    
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E �L�[�҂��J�[�\���̂ݕ\��
 * @param tmsg FLDTALKMSGWIN
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
void FLDTALKMSGWIN_WriteKeyWaitCursor( FLDTALKMSGWIN *tmsg )
{
  GFL_BMPWIN *twin_bmp = TALKMSGWIN_GetBmpWin(
    tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( twin_bmp );
  FLDKEYWAITCURSOR_Write( &tmsg->cursor_work, bmp, 0x0f );
  GFL_BMPWIN_TransVramCharacter( twin_bmp );
}

//======================================================================
//  �v���[���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/// FLDPLAINMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDPLAINMSGWIN
{
  FLDKEYWAITCURSOR cursor_work;
  
  STRBUF *strBuf;
  FLDMSGBG *fmb;
  TALKMSGWIN_SYS *talkMsgWinSys; //FLDMSGBG���
  GFL_MSGDATA *msgData; //���[�U�[����
  
  GFL_BMPWIN *bmpwin;
  FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
  
  s16 shake_y;
  u8 padding[2];
};

/*
if( up_down == 0 ){
    plnwin->bmpwin = TALKMSGWIN_CreateBmpWindow(
        plnwin->talkMsgWinSys, 1, 1, 30, 4, type );
  }else{
    plnwin->bmpwin = TALKMSGWIN_CreateBmpWindow(
        plnwin->talkMsgWinSys, 1, 18, 30, 4, type );
  }
*/

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param up_down 0=up,1=down
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldPlainMsgWin_Add( FLDMSGBG *fmb,
    FLDPLAINMSGWIN *plnwin,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y,
    TALKMSGWIN_TYPE type )
{
  GF_ASSERT( fmb->talkMsgWinSys != NULL );
  
  fmb->deriveFont_plttNo = PANO_FONT_TALKMSGWIN;
  
  plnwin->talkMsgWinSys = fmb->talkMsgWinSys;

  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );
  
  plnwin->bmpwin = TALKMSGWIN_CreateBmpWindow(
      plnwin->talkMsgWinSys,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y,
      type );
  
  FLDKEYWAITCURSOR_Init( &plnwin->cursor_work, fmb->heapID );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E�ǉ�
 * @param fmb FLDMSGBG
 * @param msgData �\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval FLDPLAINMSGWIN*
 */
//--------------------------------------------------------------
FLDPLAINMSGWIN * FLDPLAINMSGWIN_Add(
    FLDMSGBG *fmb,  GFL_MSGDATA *msgData, TALKMSGWIN_TYPE type,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDPLAINMSGWIN *plnwin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDPLAINMSGWIN) );
  plnwin->fmb = fmb;
  plnwin->msgData = msgData;
  plnwin->strBuf = GFL_STR_CreateBuffer(
          FLDMSGBG_STRLEN, fmb->heapID );
  fldPlainMsgWin_Add( fmb, plnwin, bmppos_x, bmppos_y,
      bmpsize_x, bmpsize_y, type );
  
  if( type == TALKMSGWIN_TYPE_GIZA ){
    plnwin->shake_y = GIZA_SHAKE_Y;
  }
  
  return( plnwin );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E�@�폜
 * FLDPLAINMSGWIN_Add()����msgData�͊e���ōs�����B
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_Delete( FLDPLAINMSGWIN *plnwin )
{
  if( plnwin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( plnwin->msgPrintStream );
  }
  
  if( plnwin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( plnwin->msgPrint );
  }

  if( plnwin->strBuf != NULL ){
    GFL_STR_DeleteBuffer( plnwin->strBuf );
  }
  
  TALKMSGWIN_DeleteBmpWindow( plnwin->talkMsgWinSys, plnwin->bmpwin );
  FLDKEYWAITCURSOR_Finish( &plnwin->cursor_work );
  GFL_HEAP_FreeMemory( plnwin );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�N���A
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_ClearMessage( FLDPLAINMSGWIN *plnwin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plnwin->bmpwin );
  GFL_BMP_Clear( bmp, 0xff);
  GFL_BG_LoadScreenReq( plnwin->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �E�B���h�E�N���A
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_ClearWindow( FLDPLAINMSGWIN *plnwin )
{
  TALKMSGWIN_ClearBmpWindow(
    plnwin->talkMsgWinSys, plnwin->bmpwin );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �E�B���h�E�`��
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_WriteWindow( FLDPLAINMSGWIN *plnwin )
{
  TALKMSGWIN_TYPE type = TEST_TALKMSGWIN_TYPE;
  TALKMSGWIN_WriteBmpWindow(
      plnwin->talkMsgWinSys, plnwin->bmpwin, type );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�\��
 * @param msgWin FLDPLAINMSGWIN *
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^�@������ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_Print(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, u32 strID )
{
  if( plnwin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( plnwin->msgPrint );
  }
  
  GF_ASSERT( plnwin->msgData );
  
  plnwin->msgPrint = FLDMSGPRINT_SetupPrint(
      plnwin->fmb, plnwin->msgData, plnwin->bmpwin );
  
  FLDMSGPRINT_Print( plnwin->msgPrint, x, y, strID );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�\�� STRBUF�w��
 * @param msgWin FLDPLAINMSGWIN *
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_PrintStrBuf(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, STRBUF *strBuf )
{
  if( plnwin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( plnwin->msgPrint );
  }
  
  plnwin->msgPrint = FLDMSGPRINT_SetupPrint(
      plnwin->fmb, NULL, plnwin->bmpwin );
  
  FLDMSGPRINT_PrintStrBuf( plnwin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�\�� STRBUF�A�J���[�w��
 * @param msgWin FLDPLAINMSGWIN *
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf  STRBUF
 * @param color PRINTSYS_LSB
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_PrintStrBufColor(
    FLDPLAINMSGWIN *plnwin, 
    u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color )
{
  if( plnwin->msgPrint != NULL ){
    FLDMSGPRINT_Delete( plnwin->msgPrint );
  }
  
  plnwin->msgPrint = FLDMSGPRINT_SetupPrint(
      plnwin->fmb, NULL, plnwin->bmpwin );
  
  FLDMSGPRINT_PrintStrBufColor( plnwin->msgPrint, x, y, strBuf, color );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�X�g���[���\���J�n
 * @param msgWin FLDPLAINMSGWIN *
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strID ���b�Z�[�W�f�[�^�@������ID
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_PrintStreamStart(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, u32 strID )
{
  if( plnwin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( plnwin->msgPrintStream );
  }
  
  GF_ASSERT( plnwin->msgData );
  GFL_MSG_GetString( plnwin->msgData, strID, plnwin->strBuf );
  
  plnwin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    plnwin->fmb, plnwin->strBuf, plnwin->bmpwin, x, y, Control_GetMsgWait( &plnwin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�X�g���[���\���J�n�@STRBUF�w��
 * @param msgWin FLDPLAINMSGWIN *
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_PrintStreamStartStrBuf(
    FLDPLAINMSGWIN *plnwin, u16 x, u16 y, const STRBUF *strBuf )
{
  if( plnwin->msgPrintStream != NULL ){
    FLDMSGPRINT_STREAM_Delete( plnwin->msgPrintStream );
  }
  
  plnwin->msgPrintStream = FLDMSGPRINT_STREAM_SetupPrint(
    plnwin->fmb, strBuf, plnwin->bmpwin, x, y, Control_GetMsgWait( &plnwin->fmb->print_cont ) );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E ���b�Z�[�W�X�g���[���\��
 * @param plnwin FLDTALKMSGWIN
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL FLDPLAINMSGWIN_PrintStream( FLDPLAINMSGWIN *plnwin )
{
  int trg,cont;
  PRINTSTREAM_STATE state;
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  state = fldMsgPrintStream_ProcPrint( plnwin->msgPrintStream );
  
  { //shake
    GFL_BG_SetScroll(
        FLDMSGBG_BGFRAME, GFL_BG_SCROLL_Y_SET, plnwin->shake_y );
    
    if( plnwin->shake_y < 0 ){
      plnwin->shake_y += 2;
      
      if( plnwin->shake_y > 0 ){
        plnwin->shake_y = 0;
      }
    }
    
    plnwin->shake_y = -plnwin->shake_y;
  }
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( FLDKEYWAITCURSOR_GetState(&plnwin->cursor_work) == CURSOR_STATE_WRITE ){
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plnwin->bmpwin );
      FLDKEYWAITCURSOR_Clear( &plnwin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( plnwin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    {
      GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plnwin->bmpwin );
      FLDKEYWAITCURSOR_Write( &plnwin->cursor_work, bmp, 0x0f );
      GFL_BMPWIN_TransVramCharacter( plnwin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    if( plnwin->shake_y ){ //shake
      plnwin->shake_y = 0;
      GFL_BG_SetScroll(
          FLDMSGBG_BGFRAME, GFL_BG_SCROLL_Y_SET, plnwin->shake_y );
    }
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E �L�[�҂��J�[�\���̂ݕ`��
 * @param plnwin FLDTALKMSGWIN
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
void FLDPLAINMSGWIN_WriteKeyWaitCursor( FLDPLAINMSGWIN *plnwin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plnwin->bmpwin );
  FLDKEYWAITCURSOR_Write( &plnwin->cursor_work, bmp, 0x0f );
  GFL_BMPWIN_TransVramCharacter( plnwin->bmpwin );
}

//======================================================================
//  �T�u�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/// FLDSUBMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDSUBMSGWIN
{
  STRBUF *strBuf;
  int id; //�E�B���h�EID
  int talkMsgWinIdx;
  TALKMSGWIN_SYS *talkMsgWinSys; //FLDMSGBG���
  FLDKEYWAITCURSOR cursor_work;
};

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldSubMsgWin_Add(
    FLDMSGBG *fmb, FLDSUBMSGWIN *subwin,
    STRBUF *strBuf, u16 idx, u8 x, u8 y, u8 sx, u8 sy, int id )
{
  TALKMSGWIN_TYPE type = TEST_TALKMSGWIN_TYPE;

  GF_ASSERT( fmb->talkMsgWinSys != NULL );
  
  fmb->deriveFont_plttNo = PANO_FONT_TALKMSGWIN;
  
  subwin->talkMsgWinSys = fmb->talkMsgWinSys;
  subwin->talkMsgWinIdx = idx;
  subwin->id = id;
  
  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );
  
  TALKMSGWIN_CreateWindowAlone(
    subwin->talkMsgWinSys, subwin->talkMsgWinIdx,
    strBuf, x, y, sx, sy, GX_RGB(31,31,31), type, Control_GetMsgWait( &fmb->print_cont ) );
  
  TALKMSGWIN_OpenWindow( subwin->talkMsgWinSys, subwin->talkMsgWinIdx );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �T�u�E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param msgData �g�p����GFL_MSGDATA
 * @param msgID ���b�Z�[�W �A�[�J�C�u�f�[�^ID
 * @retval FLDPLAINMSGWIN*
 */
//--------------------------------------------------------------
void FLDSUBMSGWIN_Add( FLDMSGBG *fmb,
    GFL_MSGDATA *msgData, u32 msgID,
    int id, u8 x, u8 y, u8 sx, u8 sy )
{
  int idx;
  
  FLDSUBMSGWIN *subwin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDSUBMSGWIN) );
  
  idx = FldMsgBG_SetFldSubMsgWin( fmb, subwin );
  idx += FLDTALKMSGWIN_IDX_SUBWIN0;
  
  subwin->strBuf = GFL_STR_CreateBuffer(
          FLDMSGBG_STRLEN_SUBWIN, fmb->heapID );
  
  GFL_MSG_GetString( msgData, msgID, subwin->strBuf );
  fldSubMsgWin_Add( fmb, subwin, subwin->strBuf,
      idx, x, y, sx, sy, id );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �T�u�E�B���h�E�Z�b�g
 * @param fmb FLDMSGBG
 * @param up_down 0=up,1=down
 * @param strBuf �\������STRBUF 
 * @retval nothing
 * @note strBuf�̓R�s�[���Ċi�[���܂��̂ŁA
 * �Z�b�g��A�J�����Ă���肠��܂���B
 */
//--------------------------------------------------------------
void FLDSUBMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    const STRBUF *strBuf, int id, u8 x, u8 y, u8 sx, u8 sy )
{
  int idx;
  
  FLDSUBMSGWIN *subwin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDSUBMSGWIN) );
  idx = FldMsgBG_SetFldSubMsgWin( fmb, subwin );
  idx += FLDTALKMSGWIN_IDX_SUBWIN0;
  
  {
    u32 len = GFL_STR_GetBufferLength( strBuf ) + 1;
    subwin->strBuf = GFL_STR_CreateBuffer( len, fmb->heapID );
    GFL_STR_CopyBuffer( subwin->strBuf, strBuf );
  }
  
  fldSubMsgWin_Add(
      fmb, subwin, subwin->strBuf, idx, x, y, sx, sy, id );
}

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E�폜�@���C��
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
static void fldSubMsgWin_Delete( FLDSUBMSGWIN *subwin )
{
  if( subwin->strBuf != NULL ){
    GFL_STR_DeleteBuffer( subwin->strBuf );
  }
  
  TALKMSGWIN_DeleteWindow( subwin->talkMsgWinSys, subwin->talkMsgWinIdx );
  GFL_HEAP_FreeMemory( subwin );
}

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E�폜
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSUBMSGWIN_Delete( FLDMSGBG *fmb, int id )
{
  FLDSUBMSGWIN *subwin = FldMsgBG_DeleteFldSubMsgWin( fmb, id );
  fldSubMsgWin_Delete( subwin );
}

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E�S�폜
 * @param
 * @retval
 */
//--------------------------------------------------------------
void FLDSUBMSGWIN_DeleteAll( FLDMSGBG *fmb )
{
  int i;
  for( i = 0; i < FLDSUBMSGWIN_MAX; i++ ){
    if( fmb->subMsgWinTbl[i] != NULL ){
      FLDSUBMSGWIN *subwin = fmb->subMsgWinTbl[i] = NULL;
      fmb->subMsgWinTbl[i] = NULL;
      fldSubMsgWin_Delete( subwin );
    }
  }
}

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E�����݂��邩�`�F�b�N
 * @param
 * @retval BOOL TRUE=���݂���
 */
//--------------------------------------------------------------
BOOL FLDSUBMSGWIN_CheckExistWindow( FLDMSGBG *fmb )
{
  int i;
  for( i = 0; i < FLDSUBMSGWIN_MAX; i++ ){
    if( fmb->subMsgWinTbl[i] != NULL ){
      return( TRUE );
    }
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDSUBMSGWIN �T�u�E�B���h�E �\��
 * @param subwin FLDTALKMSGWIN
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
static BOOL fldSubMsgWin_Print( FLDSUBMSGWIN *subwin )
{
  int trg,cont;
  PRINTSTREAM_STATE state;
  PRINT_STREAM *stream;
  PRINTSTREAM_PAUSE_TYPE pause_type;
  
  if( TALKMSGWIN_CheckPrintOn(
        subwin->talkMsgWinSys,subwin->talkMsgWinIdx) == FALSE ){
    return( FALSE );
  }

  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  stream = TALKMSGWIN_GetPrintStream(
      subwin->talkMsgWinSys, subwin->talkMsgWinIdx );
  state = PRINTSYS_PrintStreamGetState( stream );
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  BG�E�B���h�E
//======================================================================
#define BGWIN_NCOL (0x0f)

//--------------------------------------------------------------
/**
 * �t�B�[���hBG�E�B���h�E�@�ǉ�
 * @param fmb FLDMSGBG*
 * @param type FLDBGWIN_TYPE
 * @retval FLDBGWIN*
 */
//--------------------------------------------------------------
FLDBGWIN * FLDBGWIN_Add( FLDMSGBG *fmb, FLDBGWIN_TYPE type )
{
  return FLDBGWIN_AddEx( fmb, type, fmb->fontHandle );
}

//----------------------------------------------------------------------------
/**
 * �t�B�[���hBG�E�B���h�E�@�ǉ� �@�ڍאݒ�
 *
 * @param fmb FLDMSGBG*
 * @param type FLDBGWIN_TYPE
 * @param	useFontHandle �t�H���g�n���h���O���w��
 * @retval FLDBGWIN*
 *
 * *10/04/27 �C��_�a�Í����b�Z�[�W�p�ɍ쐬 takahashi tomoya 
 */
//-----------------------------------------------------------------------------
FLDBGWIN * FLDBGWIN_AddEx( FLDMSGBG *fmb, FLDBGWIN_TYPE type, GFL_FONT* useFontHandle )
{
  FLDBGWIN *bgWin;
  
  bgWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDBGWIN) );
  bgWin->fmb = fmb;
  bgWin->useFontHandle = useFontHandle;
  
  {
    bgWin->y = -48;
    GFL_BG_SetScroll( bgWin->fmb->bgFrame, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScroll( bgWin->fmb->bgFrame, GFL_BG_SCROLL_Y_SET, bgWin->y );
  }
  
  {
    GFL_BMP_DATA *bmp;
    
    bgWin->bmpwin = GFL_BMPWIN_Create( fmb->bgFrame,
        2, 19, 27, 4, PANO_BGWIN, GFL_BMP_CHRAREA_GET_B );
    bgWin->bmp_new = GFL_BMP_Create(
        27, 4, GFL_BMP_16_COLOR, fmb->heapID );
    bgWin->bmp_old = GFL_BMP_Create(
        27, 4, GFL_BMP_16_COLOR, fmb->heapID );
    
    bmp = GFL_BMPWIN_GetBmp( bgWin->bmpwin );
    GFL_BMP_Clear( bmp, BGWIN_NCOL );
    GFL_BMPWIN_MakeScreen( bgWin->bmpwin );
    GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
    
    bgwin_InitGraphic(
      fmb->bgFrame, PANO_BGWIN, type, fmb->heapID );
    
    bgwin_WriteWindow( fmb->bgFrame, 
      GFL_BMPWIN_GetPosX( bgWin->bmpwin ),
      GFL_BMPWIN_GetPosY( bgWin->bmpwin ),
      GFL_BMPWIN_GetSizeX( bgWin->bmpwin ),
      GFL_BMPWIN_GetSizeY( bgWin->bmpwin ), CHARNO_OTHERSWIN, PANO_BGWIN );
    
    GFL_BG_LoadScreenReq( fmb->bgFrame );
  }
  
  FLDKEYWAITCURSOR_Init( &bgWin->cursor_work, bgWin->fmb->heapID );
  
  fmb->deriveFont_plttNo = PANO_FONT_TALKMSGWIN;
  setBlendAlpha( FALSE );
  return( bgWin );
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBG�E�B���h�E�@�폜
 * @param FLDBGWIN *bgWin
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDBGWIN_Delete( FLDBGWIN *bgWin )
{
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( bgWin->bmpwin );
  
  bgwin_CleanWindow( bgWin->fmb->bgFrame, 
      GFL_BMPWIN_GetPosX( bgWin->bmpwin ),
      GFL_BMPWIN_GetPosY( bgWin->bmpwin ),
      GFL_BMPWIN_GetSizeX( bgWin->bmpwin ),
      GFL_BMPWIN_GetSizeY( bgWin->bmpwin ), 0, BGWIN_NCOL );
  
  GFL_BMPWIN_ClearScreen( bgWin->bmpwin );
  
  GFL_BG_FillScreen( bgWin->fmb->bgFrame, 0,
      GFL_BMPWIN_GetPosX( bgWin->bmpwin ),
      GFL_BMPWIN_GetPosY( bgWin->bmpwin ),
      GFL_BMPWIN_GetSizeX( bgWin->bmpwin ), 
      GFL_BMPWIN_GetSizeY( bgWin->bmpwin ), GFL_BG_SCRWRT_PALIN );
  
  GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
  GFL_BMPWIN_Delete( bgWin->bmpwin );
  
  GFL_BG_LoadScreenReq( bgWin->fmb->bgFrame );
  GFL_BG_SetScroll( bgWin->fmb->bgFrame, GFL_BG_SCROLL_Y_SET, 0 );
  
  GFL_BMP_Delete( bgWin->bmp_old );
  GFL_BMP_Delete( bgWin->bmp_new );
  
  if( bgWin->strTemp != NULL ){
    GFL_STR_DeleteBuffer( bgWin->strTemp );
  }
  
  FLDKEYWAITCURSOR_Finish( &bgWin->cursor_work );
  GFL_HEAP_FreeMemory( bgWin ); 
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBG�E�B���h�E�@�v�����g
 * @param FLDBGWIN *bgWin
 * @param strID ���b�Z�[�W�f�[�^ ������ID
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
BOOL FLDBGWIN_PrintStrBuf( FLDBGWIN *bgWin, const STRBUF *strBuf )
{
  switch( bgWin->seq_no ){
  case 0: //������
    bgWin->strBuf = strBuf;
#if 0
    bgWin->strTemp = GFL_STR_CreateCopyBuffer( strBuf, bgWin->fmb->heapID );
#else
    bgWin->strTemp = GFL_STR_CreateBuffer( 
        GFL_STR_GetBufferLength(strBuf) + (EOM_CODESIZE*2),
        bgWin->fmb->heapID );
#endif
    
    bgwin_PrintStr(
        GFL_BMPWIN_GetBmp(bgWin->bmpwin), bgWin->useFontHandle,
        bgWin->strBuf, bgWin->strTemp, &bgWin->line,
        BGWIN_NCOL );
    
    GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
    GFL_BG_LoadScreenReq( bgWin->fmb->bgFrame );
    bgWin->seq_no++;
    break;
  case 1: //�X�N���[��
    bgWin->y += BGWIN_SCROLL_SPEED;
    
    if( bgWin->y >= 0 ){
      bgWin->y = 0;
      bgWin->seq_no++;
    }
    
    GFL_BG_SetScroll( bgWin->fmb->bgFrame, GFL_BG_SCROLL_Y_SET, bgWin->y );
    break;
  case 2: //�I�[�`�F�b�N
    if( bgwin_CheckStrLineEOM(
          bgWin->strBuf,bgWin->strTemp,bgWin->line) == TRUE ){
      bgWin->seq_no = 5;
      break;
    }
    
    bgWin->seq_no++;
  case 3: //�y�[�W����҂�
    if( !(GFL_UI_KEY_GetTrg()&(PAD_BUTTON_A|PAD_BUTTON_B)) ){
      FLDKEYWAITCURSOR_Write( &bgWin->cursor_work,
          GFL_BMPWIN_GetBmp(bgWin->bmpwin), BGWIN_NCOL );
      GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
      break;
    }
    
    FLDKEYWAITCURSOR_Clear( &bgWin->cursor_work,
          GFL_BMPWIN_GetBmp(bgWin->bmpwin), BGWIN_NCOL );
    
    GFL_BMP_Copy( GFL_BMPWIN_GetBmp(bgWin->bmpwin), bgWin->bmp_old );
    
    bgwin_PrintStr(
        bgWin->bmp_new, bgWin->useFontHandle,
        bgWin->strBuf, bgWin->strTemp, &bgWin->line,
        BGWIN_NCOL );
    bgWin->scroll_y = 0;
    
    bgWin->seq_no++;
  case 4: //�X�N���[��
    if( bgwin_ScrollBmp(GFL_BMPWIN_GetBmp(bgWin->bmpwin),
          bgWin->bmp_old,bgWin->bmp_new,
          bgWin->scroll_y,BGWIN_NCOL) == TRUE ){
      bgWin->seq_no = 2;
    }else{
      bgWin->scroll_y += BGWIN_MSG_SCROLL_SPEED;
    }
    
    GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
    GFL_BG_LoadScreenReq( bgWin->fmb->bgFrame );
    break;
  case 5: //�I���ց@�L�[�҂�
    if( !(GFL_UI_KEY_GetTrg() & (MSG_LAST_BTN|PAD_KEY_ALL)) ){
      break;
    }
    bgWin->seq_no++;
  case 6: //�X�N���[��
    bgWin->y -= BGWIN_SCROLL_SPEED;
    
    if( bgWin->y <= -48 ){
      bgWin->y = -48;
    }
    
    GFL_BG_SetScroll( bgWin->fmb->bgFrame, GFL_BG_SCROLL_Y_SET, bgWin->y );
    
    if( bgWin->y == -48 ){
      bgWin->seq_no++;
      return( TRUE );
    }  
    break;
  case 7:
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@�O���t�B�b�N������
 * @param bgframe BG�t���[��
 * @param plttno �p���b�g�i���o�[
 * @param type wind
 * @retval nothing
 */
//--------------------------------------------------------------
static void bgwin_InitGraphic(
    u32 bgframe, u32 plttno, u32 type, HEAPID heapID )
{
  if( type >= FLDBGWIN_TYPE_MAX ){
    GF_ASSERT( 0 );
    type = FLDBGWIN_TYPE_INFO;
  }
  
  GFL_ARC_UTIL_TransVramBgCharacter(
      ARCID_FLDMAP_WINFRAME, NARC_winframe_bgwin_NCGR,
      bgframe, CHARNO_OTHERSWIN, 0, FALSE, heapID );
  
  GFL_ARC_UTIL_TransVramPaletteEx(
      ARCID_FLDMAP_WINFRAME, NARC_winframe_bgwin_NCLR,
      PALTYPE_MAIN_BG, type*0x20, plttno*0x20, 0x20, heapID );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@�`��
 * @param frm BG�t���[��
 * @param px ����X���W�@�L�����P��
 * @param py ����Y���W�@�L�����P��
 * @param sx �����@�L�����P��
 * @param sy �c���@�L�����P��
 * @param cgx �������ސ擪�L�����i���o�[
 * @param pal �p���b�g
 * @retval nothing
 */
//--------------------------------------------------------------
static void bgwin_WriteWindow(
    u8 frm, u8 px, u8 py, u8 sx, u8 sy, u16 cgx, u8 pal )
{
  GFL_BG_FillScreen( frm, cgx, px-2, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+1, px-1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+2, px, py-1, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx+3, px+sx, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+4, px+sx+1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+5, px+sx+2, py-1, 1, 1, pal );
  
  GFL_BG_FillScreen( frm, cgx+6, px-2, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+7, px-1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+9, px+sx, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+10, px+sx+1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+11, px+sx+2, py, 1, sy, pal );
  
  GFL_BG_FillScreen( frm, cgx+12, px-2, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+13, px-1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+14, px, py+sy, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx+15, px+sx, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+16, px+sx+1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+17, px+sx+2, py+sy, 1, 1, pal );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@����
 * @param frm BG�t���[��
 * @param px ����X���W�@�L�����P��
 * @param py ����Y���W�@�L�����P��
 * @param sx �����@�L�����P��
 * @param sy �c���@�L�����P��
 * @param cgx �������ރL�����i���o�[
 * @param pal �p���b�g
 * @retval nothing
 */
//--------------------------------------------------------------
static void bgwin_CleanWindow(
    u8 frm, u8 px, u8 py, u8 sx, u8 sy, u16 cgx, u8 pal )
{
  GFL_BG_FillScreen( frm, cgx, px-2, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px-1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px, py-1, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+2, py-1, 1, 1, pal );
  
  GFL_BG_FillScreen( frm, cgx, px-2, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx, px-1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+2, py, 1, sy, pal );
  
  GFL_BG_FillScreen( frm, cgx, px-2, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px-1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px, py+sy, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx, px+sx+2, py+sy, 1, 1, pal );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@�I�[�s�`�F�b�N
 * @param str_src ������\�[�X
 * @param str_temp �`�F�b�N�p�e���|����
 * @param line �`�F�b�N�s
 * @retval BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL bgwin_CheckStrLineEOM(
    const STRBUF *str_src, STRBUF *str_temp, u32 line )
{
  if( PRINTSYS_CopyLineStr(str_src,str_temp,line) == FALSE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@BMP�Ɉ�ʕ��̃t�H���g��������
 * @param bmp �������ݐ�r�b�g�}�b�v
 * @param font GFL_FONT
 * @param str_src �\�[�X������
 * @param str_temp �������ݗp�e���|����
 * @param line �������ݍs
 * @param col �J���[�i���o�[
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL bgwin_PrintStr(
    GFL_BMP_DATA *bmp, GFL_FONT *font,
    const STRBUF *str_src, STRBUF *str_temp, u32 *line, u8 col )
{
  int x = 1, y = 1;
  GFL_BMP_Clear( bmp, col );
  
  if( PRINTSYS_CopyLineStr(str_src,str_temp,*line) == FALSE ){
    return( TRUE );
  }
  
  PRINTSYS_Print( bmp, x, y, str_temp, font );
  
  y += PRINTSYS_GetStrHeight(str_temp, font ) + 1;
  (*line)++;
  
  if( PRINTSYS_CopyLineStr(str_src,str_temp,*line) == FALSE ){
    return( TRUE );
  }
  
  PRINTSYS_Print( bmp, x, y, str_temp, font );
  (*line)++;
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * BG�E�B���h�E�@BMP�X�N���[��
 * @param bmp ���f��r�b�g�}�b�v
 * @param old �X�N���[�����ď����r�b�g�}�b�v
 * @param new �X�N���[�����ĐV�K�ɏo���r�b�g�}�b�v
 * @param y�@�X�N���[���l
 * @param n_col �J���[�i���o�[
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL bgwin_ScrollBmp(
    GFL_BMP_DATA *bmp, GFL_BMP_DATA *old, GFL_BMP_DATA *new,
    int y, u16 n_col )
{
  int sizeX = GFL_BMP_GetSizeX( bmp );
  int sizeY = GFL_BMP_GetSizeY( bmp );
  
  GFL_BMP_Clear( bmp, n_col );
  
  if( y > sizeY ){
    y = sizeY;
  }

  GFL_BMP_Print( old, bmp,
        0, y,
        0, 0,
        sizeX, sizeY-y,
        n_col );
  
  GFL_BMP_Print( new, bmp,
        0, 0,
        0, sizeY-y,
        sizeX, y,
        n_col );
  
  if( y >= sizeY ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  ����E�B���h�E 
//======================================================================
#define SPWIN_CHROFFS_SPACE (9)

//--------------------------------------------------------------
/**
 * ����E�B���h�E�@�O���t�B�b�N������
 * @param bgframe BG�t���[��
 * @param plttno �p���b�g�i���o�[
 * @param type wind
 * @retval  nothing
 */
//--------------------------------------------------------------
static void spwin_InitPalette(
    u32 bgframe, u32 plttno, HEAPID heapID )
{
  GFL_ARC_UTIL_TransVramPaletteEx(
      ARCID_FLDMAP_WINFRAME, NARC_winframe_spwin_NCLR,
      PALTYPE_MAIN_BG, 0, plttno*0x20, 0x20, heapID );
}

//--------------------------------------------------------------
/**
 * ����E�B���h�E �w�i�L�����r�b�g�}�b�v�쐬
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void spwin_CreateBmpBG(
    GFL_BMP_DATA *bmp_bg,
    FLDSPWIN_TYPE type, HEAPID heapID, u32 charOffs )
{
  u8 *buf;
  void *pData;
  NNSG2dCharacterData *pCharData;
  
  if( type >= FLDSPWIN_TYPE_MAX ){
    GF_ASSERT( 0 );
    type = FLDSPWIN_TYPE_LETTER;
  }
  
  type = FLDSPWIN_TYPE_LETTER; //����A�G�����ނ����Ȃ��̂�
  
  buf = GFL_BMP_GetCharacterAdrs( bmp_bg );
  
  pData = GFL_ARC_UTIL_Load(
      ARCID_FLDMAP_WINFRAME, 
      NARC_winframe_spwin_NCGR, 0, heapID );
  
  charOffs *= 0x20;

  NNS_G2dGetUnpackedBGCharacterData( pData, &pCharData );
  MI_CpuCopy( (u8*)(pCharData->pRawData)+(0x20*type)+charOffs, buf, 0x20 );
  GFL_HEAP_FreeMemory( pData );
}

//--------------------------------------------------------------
/**
 * ����E�B���h�E�@BMP�w�i��������
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void spwin_WriteBmpBG(
    GFL_BMP_DATA *bmp, FLDSPWIN_TYPE type, HEAPID heapID )
{
  u16 x,y;
  GFL_BMP_DATA *bmp_bg;
  u16 size_x = GFL_BMP_GetSizeX( bmp ) >> 3;
  u16 size_y = GFL_BMP_GetSizeY( bmp ) >> 3;

  KAGAYA_Printf( "size x = %d, y = %d\n", size_x, size_y );
  bmp_bg = GFL_BMP_Create( 1, 1, GFL_BMP_16_COLOR, heapID );
  
  spwin_CreateBmpBG( bmp_bg, type, heapID, SPWIN_CHROFFS_SPACE );
  
  for( y = 0; y < size_y; y++ ){
    for( x = 0; x < size_x; x++ ){
      GFL_BMP_Print( bmp_bg, bmp,
          0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
    }
  }
  
  //top
  spwin_CreateBmpBG( bmp_bg, type, heapID, 1 );
  
  for( x = 0, y = 0; x < size_x; x++ ){
    GFL_BMP_Print( bmp_bg, bmp,
      0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  }

  //bottom
  spwin_CreateBmpBG( bmp_bg, type, heapID, 17 );
  
  for( x = 0, y = size_y - 1; x < size_x; x++ ){
    GFL_BMP_Print( bmp_bg, bmp,
      0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  }

  //left
  spwin_CreateBmpBG( bmp_bg, type, heapID, 8 );
  
  for( x = 0, y = 0; y < size_y; y++ ){
    GFL_BMP_Print( bmp_bg, bmp,
      0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  }

  //right
  spwin_CreateBmpBG( bmp_bg, type, heapID, 10 );
  
  for( x = size_x - 1, y = 0; y < size_y; y++ ){
    GFL_BMP_Print( bmp_bg, bmp,
      0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  }
  
  //left top
  x = 0;
  y = 0;
  spwin_CreateBmpBG( bmp_bg, type, heapID, 0 );
  GFL_BMP_Print( bmp_bg, bmp, 0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  
  //right top
  x = size_x - 1;
  y = 0;
  spwin_CreateBmpBG( bmp_bg, type, heapID, 2 );
  GFL_BMP_Print( bmp_bg, bmp, 0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );

  //left bottom
  x = 0;
  y = size_y - 1;
  spwin_CreateBmpBG( bmp_bg, type, heapID, 16 );
  GFL_BMP_Print( bmp_bg, bmp, 0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );

  //right bottom
  x = size_x - 1;
  y = size_y - 1;
  spwin_CreateBmpBG( bmp_bg, type, heapID, 18 );
  GFL_BMP_Print( bmp_bg, bmp, 0, 0, x<<3, y<<3, 8, 8, GF_BMPPRT_NOTNUKI );
  
  GFL_BMP_Delete( bmp_bg );
}

//--------------------------------------------------------------
/**
 * ����E�B���h�E�@�����񏑂�����
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void spwin_Print( u16 x, u16 y,
    GFL_BMP_DATA *bmp, GFL_FONT *font, const STRBUF *strbuf, HEAPID heapID )
{
  STRBUF *buf;
  int line = 0;
  int height = GFL_FONT_GetLineHeight( font ) + SPWIN_HEIGHT_FONT_SPACE;
  
  line = GFL_STR_GetBufferLength( strbuf ) + 1;
  buf = GFL_STR_CreateBuffer( line, heapID );
  
  line = 0;
  
  while( PRINTSYS_CopyLineStr(strbuf,buf,line) == TRUE ){
    KAGAYA_Printf( "Print x = %d, y = %d, line = %d\n", x, y, line );
    PRINTSYS_Print( bmp, x, y, buf, font );
    y += height;
    line++;
  }
  
  GFL_STR_DeleteBuffer( buf );
}

//--------------------------------------------------------------
/**
 * FDLSPWIN ����E�B���h�E�@������
 * @param fmb FLDMSGBG*
 * @param bmppos_x    //�\�����WX �L�����P��
 * @param bmppos_y    //�\�����WY �L�����P��
 * @param bmpsize_x   //�\���T�C�YX �L�����P��
 * @param bmpsize_y   //�\���T�C�YY �L�����P��
 * @retval FLDSPWIN*
 */
//--------------------------------------------------------------
FLDSPWIN * FLDSPWIN_Add( FLDMSGBG *fmb, FLDSPWIN_TYPE type,
  u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDSPWIN *spWin;
  
  KAGAYA_Printf( "SPWIN x = %d, y = %d, w = %d, h = %d, type = %d\n",
    bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, type );
  
  spWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDSPWIN) );
  spWin->fmb = fmb;
  
  spWin->bmpwin = GFL_BMPWIN_Create( fmb->bgFrame,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y,
      PANO_SPWIN, GFL_BMP_CHRAREA_GET_B );
  
  spWin->bmp_bg = GFL_BMP_Create( 1, 1, GFL_BMP_16_COLOR, fmb->heapID );
  spwin_CreateBmpBG( spWin->bmp_bg, type, fmb->heapID, SPWIN_CHROFFS_SPACE );
  spwin_WriteBmpBG( GFL_BMPWIN_GetBmp(spWin->bmpwin), type, fmb->heapID );
  spwin_InitPalette( fmb->bgFrame, PANO_SPWIN, fmb->heapID );
  
  GFL_BMPWIN_MakeScreen( spWin->bmpwin );
  GFL_BMPWIN_TransVramCharacter( spWin->bmpwin );
  GFL_BG_LoadScreenReq( fmb->bgFrame );
  
  FLDKEYWAITCURSOR_Init( &spWin->cursor_work, fmb->heapID );

  fmb->deriveFont_plttNo = PANO_FONT_TALKMSGWIN;
  setBlendAlpha( FALSE );
  return( spWin );
}

//--------------------------------------------------------------
/**
 * FDLSPWIN ����E�B���h�E�@�폜
 * @param fmb FLDMSGBG*
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSPWIN_Delete( FLDSPWIN *spWin )
{
  GFL_BMPWIN_ClearScreen( spWin->bmpwin );
  GFL_BG_FillScreen( spWin->fmb->bgFrame, 0,
      GFL_BMPWIN_GetPosX( spWin->bmpwin ),
      GFL_BMPWIN_GetPosY( spWin->bmpwin ),
      GFL_BMPWIN_GetSizeX( spWin->bmpwin ), 
      GFL_BMPWIN_GetSizeY( spWin->bmpwin ), GFL_BG_SCRWRT_PALIN );
  GFL_BMPWIN_TransVramCharacter( spWin->bmpwin );
  GFL_BG_LoadScreenReq( spWin->fmb->bgFrame );
  
  FLDKEYWAITCURSOR_Finish( &spWin->cursor_work );

  GFL_BMP_Delete( spWin->bmp_bg );
  GFL_BMPWIN_Delete( spWin->bmpwin );
  GFL_HEAP_FreeMemory( spWin );
}

//--------------------------------------------------------------
/**
 * FLDSPWIN ����E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param msgWin FLDMSGWIN_STREAM*
 * @param x   X�\�����W
 * @param y   Y�\�����W
 * @param strBuf �\������STRBUF
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDSPWIN_PrintStrBufStart(
    FLDSPWIN *spWin, u16 x, u16 y, const STRBUF *strBuf )
{
  spwin_Print( x, y,
      GFL_BMPWIN_GetBmp(spWin->bmpwin),
      spWin->fmb->fontHandle,
      strBuf,
      spWin->fmb->heapID );
  
  GFL_BMPWIN_TransVramCharacter( spWin->bmpwin );
  GFL_BG_LoadScreenReq( spWin->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDSPWIN ����E�B���h�E ���b�Z�[�W�\��
 * @param msgWin FLDMSGWIN_STREAM*
 * @retval BOOL TRUE=�\���I��,FALSE=�\����
 */
//--------------------------------------------------------------
BOOL FLDSPWIN_Print( FLDSPWIN *spWin )
{
  int trg,cont;
  PRINTSTREAM_STATE state;
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  
  if( (trg & MSG_LAST_BTN) ){
    return( TRUE );
  }
  
  FLDKEYWAITCURSOR_WriteBmpBG( &spWin->cursor_work,
      GFL_BMPWIN_GetBmp(spWin->bmpwin), spWin->bmp_bg );
  GFL_BMPWIN_TransVramCharacter( spWin->bmpwin );
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * FLDSPWIN ����E�B���h�E ���b�Z�[�W����K�v�ȃE�B���h�E�������擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u32 FLDSPWIN_GetNeedWindowWidthCharaSize(
    FLDMSGBG *fmb, const STRBUF *strbuf, u32 margin )
{
  u32 w = PRINTSYS_GetStrWidth(strbuf,fmb->fontHandle,0) + (margin<<1);
  u32 c = w;
  w >>= 3;
  if( (c&0x07) ){
    w++;
  }
  return( w );
}

//--------------------------------------------------------------
/**
 * FLDSPWIN ����E�B���h�E ���b�Z�[�W����K�v�ȃE�B���h�E�c�����擾
 * @param
 * @retval
 */
//--------------------------------------------------------------
u32 FLDSPWIN_GetNeedWindowHeightCharaSize(
    FLDMSGBG *fmb, const STRBUF *strbuf, u32 margin )
{
  u32 line;
  u32 h = PRINTSYS_GetStrHeight(strbuf,fmb->fontHandle) + (margin<<1);
  STRBUF *tmpbuf = GFL_STR_CreateCopyBuffer( strbuf, fmb->heapID );
  
  line = 0;
  while( PRINTSYS_CopyLineStr(strbuf,tmpbuf,line) == TRUE ){
    line++;
    if( line >= 24 ){
      break;
    }
  }
  OS_Printf( "�E�s���@�n%d\n", line );
  line--;

  GFL_STR_DeleteBuffer( tmpbuf );
  
  line *= SPWIN_HEIGHT_FONT_SPACE;
  h += line;
  
  if( (h&0x07) ){
    h += 8;
  }
  
  h >>= 3;
  return( h );
}

//======================================================================
//  �L�[����J�[�\��
//======================================================================
//--------------------------------------------------------------
/**
 * �L�[����J�[�\���@����
 * @param heapID HEAPID
 * @retval FLDKEYWAITCURSOR
 */
//--------------------------------------------------------------
FLDKEYWAITCURSOR * FLDKEYWAITCURSOR_Create( HEAPID heapID )
{
  FLDKEYWAITCURSOR * work = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDKEYWAITCURSOR) );
  FLDKEYWAITCURSOR_Init( work, heapID );
  return work;
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �폜
 * @param work FLDKEYWAITCURSOR
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_Delete( FLDKEYWAITCURSOR * work )
{
  FLDKEYWAITCURSOR_Finish( work );
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� ������
 * @param work FLDKEYWAITCURSOR
 * @param heapID HEAPID
 * @retval nothing
 *
 * ���ł�FLDKEYWAITCURSOR�p�̃��[�N���m�ۂ��Ă���ꍇ�́A
 * FLDKEYWAITCURSOR_Create�łȂ���������g�p����
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_Init( FLDKEYWAITCURSOR *work, HEAPID heapID )
{
  MI_CpuClear8( work, sizeof(FLDKEYWAITCURSOR) );
  
#if 0
  work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)skip_cursor_Character,
        2, 2, GFL_BMP_16_COLOR, heapID );
  
#else
  {
    work->pArcChara = GFL_ARC_UTIL_Load(
        ARCID_FLDMAP_WINFRAME, NARC_winframe_talk_cursor_NCGR,
        FALSE, heapID );
    NNS_G2dGetUnpackedBGCharacterData( work->pArcChara, &work->pChara );
    
    work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)work->pChara->pRawData, 1, 1, GFL_BMP_16_COLOR, heapID );
  }
#endif
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �I��
 * @param work FLDKEYWAITCURSOR
 * @retval nothing
 *
 * FLDKEYWAITCURSOR�p�̃��[�N���m�ۍρiFLDKEYWAITCURSOR_Init�ŏ������j�̏ꍇ
 * FLDKEYWAITCURSOR_Delete�łȂ���������g�p����
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_Finish( FLDKEYWAITCURSOR *work )
{
  work->cursor_state = CURSOR_STATE_NONE;
  GFL_BMP_Delete( work->bmp_cursor );
  GFL_HEAP_FreeMemory( work->pArcChara );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\���@��Ԏ擾
 * @param work FLDKEYWAITCURSOR
 * @retval KEYWAITCURSOR_STATE
 */
//--------------------------------------------------------------
KEYWAITCURSOR_STATE FLDKEYWAITCURSOR_GetState( FLDKEYWAITCURSOR *work )
{
  return( work->cursor_state );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �N���A
 * @param work FLDKEYWAITCURSOR
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI 
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_Clear(
    FLDKEYWAITCURSOR *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 8;
  y = GFL_BMP_GetSizeY( bmp ) - 8 - tbl[work->cursor_anm_no];
  GFL_BMP_Fill( bmp, x, y, 8, 8, n_col );
  
  work->cursor_state = CURSOR_STATE_NONE;
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �`�敔��
 * @param work FLDKEYWAITCURSOR
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI  
 * @retval nothing
 */
//--------------------------------------------------------------
static void FLDKEYWAITCURSOR_WriteCore(
    FLDKEYWAITCURSOR *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 8;
  y = GFL_BMP_GetSizeY( bmp ) - 8 - tbl[work->cursor_anm_no];
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 0, x, y, 8, 8, 0x00 );
  
  work->cursor_state = CURSOR_STATE_WRITE;
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\��
 * @param work FLDKEYWAITCURSOR
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI 
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_Write(
    FLDKEYWAITCURSOR *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  FLDKEYWAITCURSOR_Clear( work, bmp, n_col );
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }
  
  FLDKEYWAITCURSOR_WriteCore( work, bmp, n_col );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\���@�w�iBITMAP�w��
 * @param work FLDKEYWAITCURSOR
 * @param bmp �\����GFL_BMP_DATA
 * @param bmp_bg �w�i�ɒ���1�L��������GFL_BMP_DATA
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDKEYWAITCURSOR_WriteBmpBG(
    FLDKEYWAITCURSOR *work, GFL_BMP_DATA *bmp,
    GFL_BMP_DATA *bmp_bg )
{
  s16 x,y;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 8;
  y = GFL_BMP_GetSizeY( bmp ) - 8 - tbl[work->cursor_anm_no];
  
  { //�N���A
    s16 dy = y & 0x07;
    
    if( dy ){
      GFL_BMP_Print( bmp_bg, bmp,
          0, 8-dy, x, y, 8, dy, GF_BMPPRT_NOTNUKI );
    }
    
    GFL_BMP_Print( bmp_bg, bmp,
        0, 0, x, y+dy, 8, 8-dy, GF_BMPPRT_NOTNUKI );
  }
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }
  
  x = GFL_BMP_GetSizeX( bmp ) - 8;
  y = GFL_BMP_GetSizeY( bmp ) - 8 - tbl[work->cursor_anm_no];
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 0, x, y, 8, 8, 0x00 );
  
  work->cursor_state = CURSOR_STATE_WRITE;
}

//======================================================================
//  �V�X�e���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�O���t�B�b�N������
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void syswin_InitGraphic( HEAPID heapID )
{
  GFL_ARC_UTIL_TransVramBgCharacter(
    ARCID_FLDMAP_WINFRAME, NARC_winframe_syswin_NCGR,
    FLDMSGBG_BGFRAME, CHARNO_SYSWIN, 0, FALSE, heapID );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�`��
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void syswin_WriteWindow( const GFL_BMPWIN *bmpwin )
{
  u16 cgx = CHARNO_SYSWIN;
  u8 px = GFL_BMPWIN_GetPosX( bmpwin );
  u8 py = GFL_BMPWIN_GetPosY( bmpwin );
  u8 sx = GFL_BMPWIN_GetSizeX( bmpwin );
  u8 sy = GFL_BMPWIN_GetSizeY( bmpwin );
  u8 pal = PANO_MENU_B;
  u8 frm = GFL_BMPWIN_GetFrame( bmpwin );
  
  GFL_BG_FillScreen( frm, cgx, px-2, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+1, px-1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+2, px, py-1, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx+3, px+sx, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+4, px+sx+1, py-1, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+5, px+sx+2, py-1, 1, 1, pal );
  
  GFL_BG_FillScreen( frm, cgx+6, px-2, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+7, px-1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+9, px+sx, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+10, px+sx+1, py, 1, sy, pal );
  GFL_BG_FillScreen( frm, cgx+11, px+sx+2, py, 1, sy, pal );
  
  GFL_BG_FillScreen( frm, cgx+12, px-2, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+13, px-1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+14, px, py+sy, sx, 1, pal );
  GFL_BG_FillScreen( frm, cgx+15, px+sx, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+16, px+sx+1, py+sy, 1, 1, pal );
  GFL_BG_FillScreen( frm, cgx+17, px+sx+2, py+sy, 1, 1, pal );
  
  GFL_BG_LoadScreenReq( frm );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�r�b�g�}�b�v�E�B���h�E�t���[���@������
 * @param bgframe BG�t���[��
 * @param heapID  HEAPID
 * @param pos_y �r�b�g�}�b�v����Y �L�����P��
 * @retval  GFL_BMPWIN  �쐬���ꂽGFL_BMPWIN
 */
//--------------------------------------------------------------
static GFL_BMPWIN * syswin_InitBmp( u8 pos_x, u8 pos_y, u8 sx, u8 sy, HEAPID heapID )
{
  GFL_BMP_DATA *bmp;
  GFL_BMPWIN *bmpwin;
  
  bmpwin = GFL_BMPWIN_Create( FLDMSGBG_BGFRAME_BLD,
    pos_x, pos_y, sx, sy,
    PANO_FONT, GFL_BMP_CHRAREA_GET_B );
  
  bmp = GFL_BMPWIN_GetBmp( bmpwin );
  
  GFL_BMP_Clear( bmp, 0xff );
  GFL_BMPWIN_MakeScreen( bmpwin );
  GFL_BMPWIN_TransVramCharacter( bmpwin );
  syswin_WriteWindow( bmpwin );

  return( bmpwin );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@�N���A
 * @param bmpwin  syswin_InitBmp()�̖߂�l
 * @retval  nothing
 */
//--------------------------------------------------------------
static void syswin_ClearBmp( GFL_BMPWIN *bmpwin )
{
  u8 frm = GFL_BMPWIN_GetFrame( bmpwin );
  
  GFL_BG_FillScreen(
    frm, CHARNO_CLEAR,
    GFL_BMPWIN_GetPosX( bmpwin ) - 2,
    GFL_BMPWIN_GetPosY( bmpwin ) - 1,
    GFL_BMPWIN_GetSizeX( bmpwin ) + 3 + 2,
    GFL_BMPWIN_GetSizeY( bmpwin ) + 2,
    0 );
  
  GFL_BG_LoadScreenReq( frm );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@�폜
 * @param bmpwin  syswin_InitBmp()�̖߂�l
 * @retval  nothing
 */
//--------------------------------------------------------------
static void syswin_DeleteBmp( GFL_BMPWIN *bmpwin )
{
  syswin_ClearBmp( bmpwin );
  GFL_BMPWIN_Delete( bmpwin );
}

//======================================================================
//  ���̑�
//======================================================================
//--------------------------------------------------------------
/**
 * BG���\�[�X������
 * @param fmb FLDMSGBG*
 * @param trans   �A�[�J�C�u�����[�h���ē]�����邩�H
 * @retval nothing
 */
//--------------------------------------------------------------
static void setBGResource( FLDMSGBG *fmb, const BOOL trans )
{
  fmb->bgFrame = FLDMSGBG_BGFRAME; //�s����BG
  
  { //BG������
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, FLDBG_MFRM_MSG_SCRSIZE, 0,
      GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_MSG_COLORMODE,
      FLDBG_MFRM_MSG_SCRBASE,
      FLDBG_MFRM_MSG_CHARBASE, FLDBG_MFRM_MSG_CHARSIZE,
      GX_BG_EXTPLTT_01, FLDBG_MFRM_MSG_PRI, 0, 0, FALSE
    };
    
    GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
  }
  
  fmb->bgFrameBld = FLDMSGBG_BGFRAME_BLD; //������BG
  
  { //BG������
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, FLDBG_MFRM_EFF1_SCRSIZE, 0,
      GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_EFF1_COLORMODE,
      FLDBG_MFRM_EFF1_SCRBASE,
      FLDBG_MFRM_MSG_CHARBASE, FLDBG_MFRM_MSG_CHARSIZE,
      GX_BG_EXTPLTT_01, FLDBG_MFRM_EFF1_PRI, 0, 0, FALSE
    };
    
    GFL_BG_SetBGControl( fmb->bgFrameBld, &bgcntText, GFL_BG_MODE_TEXT );
  }
  
  { //BG�L�����A�X�N���[��������
    GFL_BG_FillCharacter( //�N���A�L����
        fmb->bgFrame, CHARNO_CLEAR, 1, 0 );
    
    GFL_BG_FillScreen( fmb->bgFrame,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_FillScreen( fmb->bgFrameBld,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

    GFL_BG_LoadScreenReq( fmb->bgFrame );
    GFL_BG_LoadScreenReq( fmb->bgFrameBld );
  }
 
  if (trans){
    // �p���b�g�E�V�X�e���E�C���h�E
    transBGResource( fmb->bgFrame, fmb->heapID );
  }

  { //TALKMSGWIN
    if( fmb->g3Dcamera != NULL ){
      TALKMSGWIN_SYS_SETUP setup;
      setup.heapID = fmb->heapID;
      setup.g3Dcamera = fmb->g3Dcamera;
      setup.fontHandle = fmb->fontHandle;
      setup.chrNumOffs = CHARNO_BALLOONWIN;
      setup.ini.frameID = FLDMSGBG_BGFRAME;
      setup.ini.winPltID = PANO_TALKMSGWIN;
      setup.ini.fontPltID = PANO_FONT_TALKMSGWIN;
      fmb->talkMsgWinSys = TALKMSGWIN_SystemCreate( &setup );
    }
  }
  
  { //�u�����h
    int plane1 = GX_BLEND_PLANEMASK_BG2; 
    int plane2 = GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ;
    G2_SetBlendAlpha( plane1, plane2, 16, 4 );
  }
  
#if 0
  GFL_BG_SetPriority( fmb->bgFrame, FLDBG_MFRM_MSG_PRI );
  GFL_BG_SetPriority( fmb->bgFrameBld, FLDBG_MFRM_EFF1_PRI );
#endif
  
  GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
  GFL_BG_SetVisible( fmb->bgFrameBld, VISIBLE_ON );
  
  fmb->deriveFont_plttNo = PANO_FONT;
}

//--------------------------------------------------------------
/**
 * BG���\�[�X������ BG1�ƃL�����A�p���b�g���\�[�X�̂�
 * @param fmb FLDMSGBG*
 * @retval nothing
 */
//--------------------------------------------------------------
static void setBG1Resource( FLDMSGBG *fmb )
{
  fmb->bgFrame = FLDMSGBG_BGFRAME; //�s����BG
  
  { //BG������
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, FLDBG_MFRM_MSG_SCRSIZE, 0,
      GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_MSG_COLORMODE,
      FLDBG_MFRM_MSG_SCRBASE,
      FLDBG_MFRM_MSG_CHARBASE, FLDBG_MFRM_MSG_CHARSIZE,
      GX_BG_EXTPLTT_01, FLDBG_MFRM_MSG_PRI, 0, 0, FALSE
    };
    
    GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
  }
  
  { //BG�L�����A�X�N���[��������
    GFL_BG_FillCharacter( //�N���A�L����
        fmb->bgFrame, CHARNO_CLEAR, 1, 0 );
    
    GFL_BG_FillScreen( fmb->bgFrame,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

    GFL_BG_LoadScreenReq( fmb->bgFrame );
  }
  
  // �p���b�g�E�V�X�e���E�C���h�E
  transBGResource( fmb->bgFrame, fmb->heapID );
  
  { //TALKMSGWIN
    if( fmb->g3Dcamera != NULL ){
      TALKMSGWIN_SYS_SETUP setup;
      setup.heapID = fmb->heapID;
      setup.g3Dcamera = fmb->g3Dcamera;
      setup.fontHandle = fmb->fontHandle;
      setup.chrNumOffs = CHARNO_BALLOONWIN;
      setup.ini.frameID = FLDMSGBG_BGFRAME;
      setup.ini.winPltID = PANO_TALKMSGWIN;
      setup.ini.fontPltID = PANO_FONT_TALKMSGWIN;
      fmb->talkMsgWinSys = TALKMSGWIN_SystemCreate( &setup );
    }
  }
  
  GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
  
  fmb->deriveFont_plttNo = PANO_FONT;
}

//--------------------------------------------------------------
/**
 * @brief BG2�R���g���[���̂ݍĐݒ�
 * @param   bgFrame   
 * @param   heapID
 */
//--------------------------------------------------------------
static void resetBG2Control( BOOL cont_set )
{
  u8 frame = FLDMSGBG_BGFRAME_BLD;
  
  GFL_BG_SetVisible( frame, VISIBLE_OFF );
   
  if( cont_set == TRUE ){ //BG������
    GFL_BG_BGCNT_HEADER bgcntText = {
      0, 0, FLDBG_MFRM_EFF1_SCRSIZE, 0,
      GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_EFF1_COLORMODE,
      FLDBG_MFRM_EFF1_SCRBASE,
      FLDBG_MFRM_MSG_CHARBASE, FLDBG_MFRM_MSG_CHARSIZE,
      GX_BG_EXTPLTT_01, FLDBG_MFRM_EFF1_PRI, 0, 0, FALSE
    };
    
    GFL_BG_SetBGControl( frame, &bgcntText, GFL_BG_MODE_TEXT );
  }else{
    G2_SetBG2ControlText(
      GX_BG_SCRSIZE_TEXT_256x256,
      FLDBG_MFRM_EFF1_COLORMODE,
      FLDBG_MFRM_EFF1_SCRBASE,
      FLDBG_MFRM_MSG_CHARBASE );
  }
  
  { //BG�L�����A�X�N���[��������
    GFL_BG_FillScreen( frame,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( frame );
  }
  
  { //�u�����h
    int plane1 = GX_BLEND_PLANEMASK_BG2; 
    int plane2 = GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 |
      GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ;
    G2_SetBlendAlpha( plane1, plane2, 16, 4 );
  }
  
  GFL_BG_SetPriority( frame, FLDBG_MFRM_EFF1_PRI );
  GFL_BG_SetVisible( frame, VISIBLE_ON );
}

//--------------------------------------------------------------
/**
 * BG2�R���g���[���Đݒ�
 * @param
 * @retval
 */
//--------------------------------------------------------------
static void resetBG2ControlProc( FLDMSGBG *fmb )
{
  if( fmb->req_reset_bg2_control != FALSE ){
    fmb->req_reset_bg2_control = FALSE;
    
    if( fmb->bgFrameBld == BGFRAME_ERROR ){
      GFL_BG_FreeBGControl( FLDMSGBG_BGFRAME_BLD );
      resetBG2Control( TRUE );
      fmb->bgFrameBld = FLDMSGBG_BGFRAME_BLD;
    }else{
      resetBG2Control( FALSE );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief �E�C���h�E�p���\�[�X�̂ݓ]��
 *
 * @param   bgFrame   
 * @param   heapID
 */
//--------------------------------------------------------------
static void transBGResource( int bgFrame, HEAPID heapID )
{
  { //�t�H���g�p���b�g
    GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT, NARC_font_default_nclr, //��
      PALTYPE_MAIN_BG, PANO_FONT_TALKMSGWIN*32, 32, heapID );
    GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT, NARC_font_systemwin_nclr, //��
      PALTYPE_MAIN_BG, PANO_FONT*32, 32, heapID );
  }
  
  { //window frame
    BmpWinFrame_GraphicSet( bgFrame, CHARNO_WINFRAME,
      PANO_MENU_B, MENU_TYPE_SYSTEM, heapID );
  }
  
  //SYSWIN
  {
    syswin_InitGraphic( heapID );
  }
}

//--------------------------------------------------------------
/**
 * @brief �E�C���h�E�p���\�[�X�̂ݓ]���@���\�[�X�w��^�]��
 *
 * @param   bgFrame   
 * @param   heapID
 * @param   �w�胊�\�[�X
 */
//--------------------------------------------------------------
static void transBGResourceParts( int bgFrame, HEAPID heapID, MSGBG_TRANS_RES res )
{
  if (res == MSGBG_TRANS_RES_FONTPAL)
  { //�t�H���g�p���b�g
    GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT, NARC_font_default_nclr, //��
      PALTYPE_MAIN_BG, PANO_FONT_TALKMSGWIN*32, 32, heapID );
    GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT, NARC_font_systemwin_nclr, //��
      PALTYPE_MAIN_BG, PANO_FONT*32, 32, heapID );    
  }else if( res == MSGBG_TRANS_RES_WINFRM )  
  { //window frame
    BmpWinFrame_GraphicSet( bgFrame, CHARNO_WINFRAME,
      PANO_MENU_B, MENU_TYPE_SYSTEM, heapID );
  }else if (res == MSGBG_TRANS_RES_SYSWIN)
  //SYSWIN
  {
    syswin_InitGraphic( heapID );
  }
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���b�Z�[�WBG �u�����h�A���t�@�Z�b�g
 * @param on TRUE=�������I�� FALSE=�������I�t
 * @retval nothing
 */
//--------------------------------------------------------------
static void setBlendAlpha( BOOL on )
{
  if( on == TRUE ){
    int plane1 = GX_BLEND_PLANEMASK_BG2; 
    int plane2 = GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|
      GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ;
    G2_SetBlendAlpha( plane1, plane2, 16, 4 );
  }
}

#if 0 //old
static void setBlendAlpha( BOOL on )
{
  if( on == TRUE ){
    int plane1 = GX_BLEND_PLANEMASK_BG1; 
    int plane2 = GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG2|
      GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ;
    G2_SetBlendAlpha( plane1, plane2, 31, 8 );
  }else{
    int plane1 = GX_BLEND_PLANEMASK_NONE; 
    int plane2 = GX_BLEND_PLANEMASK_NONE; 
    G2_SetBlendAlpha( plane1, plane2, 0, 0 );
  }
}
#endif

//--------------------------------------------------------------
/**
 * �t�B�[���h���b�Z�[�WBG �u�����h�A���t�@�Z�b�g
 * @param set TRUE=������ON
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_SetBlendAlpha( BOOL set )
{
  setBlendAlpha( set );
}

//--------------------------------------------------------------
/**
 * �L�����X�N���[���f�[�^���A
 * @param 
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_RecoveryBG( FLDMSGBG *fmb )
{
  HEAPID heapID = fmb->heapID;
  
  {
    GFL_BG_FillCharacterRelease( fmb->bgFrame, 1, 0 );
    GFL_BG_FillCharacter( fmb->bgFrame, 0x00, 1, 0 );
    GFL_BG_FillScreen( fmb->bgFrame,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    
    GFL_BG_LoadScreenReq( fmb->bgFrame );
  }
#if 0  
  {
    GFL_BG_FillScreen( fmb->bgFrameBld,
      0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
    GFL_BG_LoadScreenReq( fmb->bgFrameBld );
  }
#endif  

  transBGResource( fmb->bgFrame, fmb->heapID );

  { //�o���[���E�B���h�E
    TALKMSGWIN_ReTransWindowBG( fmb->talkMsgWinSys );
  }
}

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------

//======================================================================
//  �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@������
 * @param bgframe BG�t���[��
 * @param heapID  HEAPID
 * @param pos_x �r�b�g�}�b�v����X �L�����P��
 * @param pos_y �r�b�g�}�b�v����Y �L�����P��
 * @param size_x�@�r�b�g�}�b�v�T�C�YX �L�����P��
 * @param size_y  �r�b�g�}�b�v�T�C�YY �L�����P��
 * @retval  GFL_BMPWIN  �쐬���ꂽGFL_BMPWIN
 */
//--------------------------------------------------------------
static GFL_BMPWIN * winframe_InitBmp( u32 bgFrame, HEAPID heapID,
  u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no )
{
  GFL_BMP_DATA *bmp;
  GFL_BMPWIN *bmpwin;
  
  bmpwin = GFL_BMPWIN_Create( bgFrame,
    pos_x, pos_y, size_x, size_y,
    pltt_no, GFL_BMP_CHRAREA_GET_B );

  bmp = GFL_BMPWIN_GetBmp( bmpwin );
  
  GFL_BMP_Clear( bmp, 0xff );
  GFL_BMPWIN_MakeScreen( bmpwin );
  GFL_BMPWIN_TransVramCharacter( bmpwin );
  GFL_BG_LoadScreenReq( bgFrame );
  
  if( pltt_no == PANO_FONT ){ //������
    pltt_no = PANO_MENU_B;
  }else{ //�s����
    pltt_no = PANO_MENU_W;
  }

  BmpWinFrame_Write( bmpwin, WINDOW_TRANS_ON, 1, pltt_no );
  
  return( bmpwin );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@�폜
 * @param bmpwin  winframe_InitBmp()�̖߂�l
 * @retval  nothing
 */
//--------------------------------------------------------------
static void winframe_DeleteBmp( GFL_BMPWIN *bmpwin )
{
  BmpWinFrame_Clear( bmpwin, 0 );
  GFL_BMPWIN_Delete( bmpwin );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@���w�i�p���b�g�Z�b�g
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void winframe_SetPaletteBlack( u32 heapID )
{
  u32 pal = PANO_MENU_B;
  u32 arc = BmpWinFrame_WinPalArcGet(MENU_TYPE_SYSTEM);
  GFL_ARC_UTIL_TransVramPaletteEx(
      ARCID_FLDMAP_WINFRAME,
      arc, PALTYPE_MAIN_BG, 0x20*1, pal*0x20, 0x20, heapID );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@���w�i�p���b�g�Z�b�g
 * @param nothing
 * @retval nothing
 */
//--------------------------------------------------------------
static void winframe_SetPaletteWhith( u32 heapID )
{
  u32 pal = PANO_MENU_W;
  u32 arc = BmpWinFrame_WinPalArcGet(MENU_TYPE_SYSTEM);
  GFL_ARC_UTIL_TransVramPaletteEx(
      ARCID_FLDMAP_WINFRAME,
      arc, PALTYPE_MAIN_BG, 0x20*0, pal*0x20, 0x20, heapID );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG FLDSUBMSGWIN�o�^
 * @param fmb FLDMSGBG
 * @param subwin �o�^����FLDSUBMSGWIN
 * @retval int �E�B���h�E�C���f�b�N�X
 */
//--------------------------------------------------------------
static int FldMsgBG_SetFldSubMsgWin( FLDMSGBG *fmb, FLDSUBMSGWIN *subwin )
{
  int i;
  for( i = 0; i < FLDSUBMSGWIN_MAX; i++ ){
    if( fmb->subMsgWinTbl[i] == NULL ){
      fmb->subMsgWinTbl[i] = subwin;
      return( i );
    }
  }
  
  GF_ASSERT( 0 );
  return( i );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG FLDSUBMSGWIN�폜
 * @param fmb FLDMSGBG
 * @param id �o�^���Ă����E�B���h�EID
 * @retval FLDSUBMSGWIN id�Ŏg�p���Ă���FLDSUBMSGWIN
 */
//--------------------------------------------------------------
static FLDSUBMSGWIN * FldMsgBG_DeleteFldSubMsgWin( FLDMSGBG *fmb, int id )
{
  int i;
  for( i = 0; i < FLDSUBMSGWIN_MAX; i++ ){
    if( fmb->subMsgWinTbl[i] != NULL && fmb->subMsgWinTbl[i]->id == id ){
      FLDSUBMSGWIN *subwin = fmb->subMsgWinTbl[i];
      fmb->subMsgWinTbl[i] = NULL;
      return( subwin );
    }
  }
  
  GF_ASSERT( 0 );
  return( NULL );
}



//----------------------------------------------------------------------------
/**
 *  @brief  Print�ݒ� ������
 *
 *  @param  cont  ���[�N
 */
//-----------------------------------------------------------------------------
static void Control_Init( FLDPRINT_CONTROL* cont )
{
  GFL_STD_MemClear( cont, sizeof(FLDPRINT_CONTROL) );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Đ��J�n
 *
 *  @param  cont  ���[�N
 */
//-----------------------------------------------------------------------------
static void Control_StartPrint( FLDPRINT_CONTROL* cont )
{
  cont->key_wait = 0;
  cont->key_skip = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����L�[����{���b�Z�[�W�X�s�[�h���t���O�̐ݒ�
 *
 *  @param  cont    ���[�N
 *  @param  flag      �t���O
 */
//-----------------------------------------------------------------------------
static void Control_SetAutoPrintFlag( FLDPRINT_CONTROL* cont, BOOL flag )
{
  cont->auto_msg_flag = flag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����L�[����{���b�Z�[�W�X�s�[�h���t���O�̎擾
 *
 *  @param  cont  ���[�N
 */
//-----------------------------------------------------------------------------
static BOOL Control_GetAutoPrintFlag( const FLDPRINT_CONTROL* cont )
{
  return cont->auto_msg_flag;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�Đ��X�s�[�h�̎擾
 *
 *  @param  cont  ���[�N
 *
 *  @return �Đ��X�s�[�h
 */
//-----------------------------------------------------------------------------
static int Control_GetMsgWait( const FLDPRINT_CONTROL* cont )
{
  if( cont->auto_msg_flag ){
    return MSGSPEED_GetWaitByConfigParam( AUTO_MSG_WAIT );
  }
  return MSGSPEED_GetWait();
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W�X�L�b�v�@�`�F�b�N
 *
 *  @param  cont  ���[�N
 *
 *  @retval TRUE    �X�L�b�v
 *  @retval FALSE   �ʏ�\��
 */
//-----------------------------------------------------------------------------
static BOOL Control_GetSkipKey( FLDPRINT_CONTROL* cont )
{
  int trg = GFL_UI_KEY_GetTrg();
  int ct = GFL_UI_KEY_GetCont();
  
  // �����Đ����̃X�L�b�v�͕s�\
  if( cont->auto_msg_flag ){
    return FALSE;
  }

  if( trg & MSG_SKIP_BTN ){
    cont->key_skip = TRUE;
  }

  if( cont->key_skip && (ct & MSG_SKIP_BTN) ){
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���b�Z�[�W����@�`�F�b�N
 *
 *  @param  cont  ���[�N
 *
 *  @retval TRUE    ���b�Z�[�W����
 *  @retval FALSE   �ҋ@
 */
//-----------------------------------------------------------------------------
static BOOL Control_GetWaitKey( FLDPRINT_CONTROL* cont, PRINTSTREAM_PAUSE_TYPE type )
{
  int trg = GFL_UI_KEY_GetTrg();
  int wait;

  if( type == PRINTSTREAM_PAUSE_LINEFEED ){
    wait = AUTO_MSG_ONE_KEY_WAIT;
  }else{
    wait = AUTO_MSG_ALLCLEAR_KEY_WAIT;
  }

  // �������蒆�́A�E�G�C�g�チ�b�Z�[�W����
  if( cont->auto_msg_flag ){
    cont->key_wait ++;
    if( cont->key_wait >= wait  ){
      cont->key_wait = 0;
      return TRUE;
    }
    return FALSE;
  }

  if( trg & MSG_LAST_BTN ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �v�����g�X�g���[���̃E�F�C�g�Z�擾
 * @param printStream PRINT_STREAM
 * @param skip TRUE=���b�Z�[�W�X�L�b�v FLASE=�X�L�b�v�؂�
 * @retval nothing
 */
//--------------------------------------------------------------
static void set_printStreamTempSpeed( PRINT_STREAM *printStream, BOOL skip )
{
  if( skip == TRUE ){
    int wait = MSGSPEED_GetWaitByConfigParam( MSGSPEED_FAST );
    PRINTSYS_PrintStream_StartTempSpeedMode( printStream, wait ); 
  }else{
    PRINTSYS_PrintStream_StopTempSpeedMode( printStream ); 
  }
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  �͂��A�������I�����j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_MenuHeader_YesNo =
{
  1,    //���X�g���ڐ�
  10,   //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
//  0,    //�\���x���W
  3,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  1,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_NON, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/// ����J�[�\���f�[�^ ���@PL���玝���Ă���
//--------------------------------------------------------------
#if 0
static const u8 ALIGN4 skip_cursor_Character[128] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x22,0x22,0x22,0x22,
0x21,0x22,0x22,0x22,0x10,0x22,0x22,0x12, 0x00,0x21,0x22,0x11,0x00,0x10,0x12,0x01,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x12,0x00,0x00,0x00,
0x11,0x00,0x00,0x00,0x01,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x11,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
#endif

/*
�p���b�g 0=300H
�p���b�g 1=7fffH
�p���b�g 2=62f5H
�p���b�g 3=3d89H
�p���b�g 4=3126H
�p���b�g 5=0H
�p���b�g 6=6f93H
�p���b�g 7=4e6cH
�p���b�g 8=3126H
�p���b�g 9=77dbH
�p���b�g 10=7fffH
�p���b�g 11=0H
�p���b�g 12=0H
�p���b�g 13=0H
�p���b�g 14=0H
�p���b�g 15=0H
*/

//======================================================================
//  debug
//======================================================================
#ifdef DEBUG_FLDMSGBG

//--------------------------------------------------------------
/**
 * �f�o�b�O�@�v�����g�X�g���[���Ŏg�p���Ă���TCB���J�E���g
 */
//--------------------------------------------------------------
static void DEBUG_AddCountPrintTCB( FLDMSGBG *fmb )
{
  fmb->d_printTCBcount++;
  
  if( fmb->d_printTCBcount > FLDMSGBG_PRINT_STREAM_MAX ){
    GF_ASSERT( 0 && "�t�B�[���h�@���b�Z�[�W��������t�ł�" );
  }
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�@�v�����g�X�g���[���Ŏg�p���Ă���TCB���J�E���g
 */
//--------------------------------------------------------------
static void DEBUG_SubCountPrintTCB( FLDMSGBG *fmb )
{
  fmb->d_printTCBcount--;
  
  if( fmb->d_printTCBcount < 0 ){
    GF_ASSERT( 0 && "�t�B�[���h�@���b�Z�[�W�����𑽂��폜���Ă��܂�" );
  }
}

//--------------------------------------------------------------
/**
 * �f�o�b�O�@�v�����g�X�g���[���Ŏg�p���Ă���TCB���J�E���g
 */
//--------------------------------------------------------------
static void DEBUG_CheckCountPrintTCB( FLDMSGBG *fmb )
{
  if( fmb->d_printTCBcount ){
    GF_ASSERT( 0 && "�t�B�[���h�@���b�Z�[�W�������c���Ă��܂�" );
  }
}

#endif //DEBUG_FLDMSGBG
