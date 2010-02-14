//======================================================================
/**
 * @file	field_msgbg.c
 * @brief	�t�B�[���h�@���b�Z�[�WBG�֘A
 * @authaor	kagaya
 * @date	2008.12.11
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
//	define
//======================================================================
#ifdef DEBUG_ONLY_FOR_yoshida
#define TEST_TALKMSGWIN_TYPE (TALKMSGWIN_TYPE_GIZA)
#else
#define TEST_TALKMSGWIN_TYPE (TALKMSGWIN_TYPE_NORMAL)
#endif

#define FLDMSGBG_BGFRAME ( FLDBG_MFRM_MSG )	///<�g�pBG�t���[��

#define FLDMSGBG_PRINT_MAX (4)				///<PRINT�֘A�v�f���ő�
#define FLDMSGBG_PRINT_STREAM_MAX (1) ///<PRINT_STREAM�ғ���

#define FLDMSGBG_STRLEN (128)				///<�����񒷂��W��

#define SPWIN_HEIGHT_FONT_SPACE (2)

#define MSG_SKIP_BTN (PAD_BUTTON_A|PAD_BUTTON_B)
#define MSG_LAST_BTN (PAD_BUTTON_A|PAD_BUTTON_B)

#define BGWIN_SCROLL_SPEED (12)
#define BGWIN_MSG_SCROLL_SPEED (4)

#define PAD_KEY_ALL (PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT)

#define BGFRAME_ERROR (0xff)

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
  PANO_BGWIN = 10, //BG�E�B���h�E
  PANO_SPWIN = 10, //����E�B���h�E
  PANO_FONT_TALKMSGWIN = 11, ///<�����o���t�H���g�p���b�gNo
  PANO_TALKMSGWIN = 12, ///<�����o���E�B���h�E�p���b�gNo
  PANO_MENU = 13, ///<���j���[�p���b�gNo
  PANO_FONT = 14, ///<�t�H���g�p���b�gNo
};

//--------------------------------------------------------------
/// �J�[�\���t���O
//--------------------------------------------------------------
enum
{
  CURSOR_FLAG_NONE = 0,
  CURSOR_FLAG_WRITE,
  CURSOR_FLAG_END,
};

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
/// KEYCURSOR_WORK
//--------------------------------------------------------------
typedef struct
{
  u8 cursor_anm_no;
  u8 cursor_anm_frame;
  GFL_BMP_DATA *bmp_cursor;
}KEYCURSOR_WORK;

//--------------------------------------------------------------
///	FLDMSGPRINT
//--------------------------------------------------------------
struct _TAG_FLDMSGPRINT
{
	FLDMSGBG *fmb;
	
	BOOL printTransFlag;
	GFL_FONT *fontHandle;	//FLDMSGBG����
	PRINT_QUE *printQue;	//FLDMSGBG����
	GFL_MSGDATA *msgData;	//���[�U�[����
	GFL_BMPWIN *bmpwin;		//���[�U�[����
	
	STRBUF *strBuf;
	PRINT_UTIL printUtil;
};

//--------------------------------------------------------------
///	FLDMSGBG
//--------------------------------------------------------------
struct _TAG_FLDMSGBG
{
	HEAPID heapID;
	u16 bgFrame;
	u16 deriveWin_plttNo;
  
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	FLDMSGPRINT msgPrintTbl[FLDMSGBG_PRINT_MAX];
  
  FLDSUBMSGWIN *subMsgWinTbl[FLDSUBMSGWIN_MAX];
  
  TALKMSGWIN_SYS *talkMsgWinSys;
  
  GFL_TCBLSYS *printTCBLSys;
  GFL_G3D_CAMERA *g3Dcamera;
};

//--------------------------------------------------------------
///	FLDMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDMSGWIN
{
	GFL_BMPWIN *bmpwin;		//���[�U�[����
	FLDMSGPRINT *msgPrint;
	FLDMSGBG *fmb;
};

//--------------------------------------------------------------
///	FLDSYSWIN
//--------------------------------------------------------------
struct _TAG_FLDSYSWIN
{
	GFL_BMPWIN *bmpwin;		//���[�U�[����
	FLDMSGPRINT *msgPrint;
	FLDMSGBG *fmb;
};

//--------------------------------------------------------------
///	FLDMENUFUNC
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
  u8 flag_key_trg;
  u8 flag_key_cont;
  int msg_wait;
  PRINT_STREAM *printStream;
};

//--------------------------------------------------------------
///	FLDMSGWIN_STREAM
//--------------------------------------------------------------
struct _TAG_FLDMSGWIN_STREAM
{
	GFL_BMPWIN *bmpwin;
	FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
  const GFL_MSGDATA *msgData; //���[�U�[����
  STRBUF *strBuf;
	FLDMSGBG *fmb;
   
  u8 flag_cursor;
  u8 flag_key_pause_clear;
  KEYCURSOR_WORK cursor_work;
};

//--------------------------------------------------------------
///	FLDSYSWIN_STREAM
//--------------------------------------------------------------
struct _TAG_FLDSYSWIN_STREAM
{
	GFL_BMPWIN *bmpwin;
	FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
  const GFL_MSGDATA *msgData; //���[�U�[����
  STRBUF *strBuf;
	FLDMSGBG *fmb;
   
  u8 flag_cursor;
  u8 flag_key_pause_clear;
  KEYCURSOR_WORK cursor_work;
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
  
  KEYCURSOR_WORK cursor_work;
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
  KEYCURSOR_WORK cursor_work;
};

//======================================================================
//	proto
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

static void keyCursor_Init( KEYCURSOR_WORK *work, HEAPID heapID );
static void keyCursor_Delete( KEYCURSOR_WORK *work );
static void keyCursor_Clear(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );
static void keyCursor_Write(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );
static void keyCursor_WriteBmpBG(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, GFL_BMP_DATA *bmp_bg );

static void syswin_InitGraphic( HEAPID heapID );
static void syswin_WriteWindow( const GFL_BMPWIN *bmpwin );
static GFL_BMPWIN * syswin_InitBmp( u8 pos_y, HEAPID heapID );
static void syswin_ClearBmp( GFL_BMPWIN *bmpwin );
static void syswin_DeleteBmp( GFL_BMPWIN *bmpwin );

static void setBGResource( FLDMSGBG *fmb );
static void setBlendAlpha( BOOL on );

static GFL_BMPWIN * winframe_InitBmp( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no );
static void winframe_DeleteBmp( GFL_BMPWIN *bmpwin );
static void winframe_SetPaletteBlack( u32 heapID );
static void winframe_SetPaletteWhith( u32 heapID );

static int FldMsgBG_SetFldSubMsgWin( FLDMSGBG *fmb, FLDSUBMSGWIN *subwin );
static FLDSUBMSGWIN * FldMsgBG_DeleteFldSubMsgWin( FLDMSGBG *fmb, int id );

static const FLDMENUFUNC_HEADER DATA_MenuHeader_YesNo;
static const u8 ALIGN4 skip_cursor_Character[128];

//======================================================================
//	FLDMSGBG	�t�B�[���h���b�Z�[�WBG�֘A
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
 * @param	heapID	HEAPID
 * @param g3Dcamera GFL_G3D_CAMERA* �����o���E�B���h�E�Ŏg�p NULL=�g�p���Ȃ�
 * @retval	FLDMAPBG	FLDMAPBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FLDMSGBG_Create( HEAPID heapID, GFL_G3D_CAMERA *g3Dcamera )
{
	FLDMSGBG *fmb;
	
	fmb = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMSGBG) );
	fmb->heapID = heapID;
//	fmb->bgFrame = BGFRAME_ERROR;
	fmb->bgFrame = FLDMSGBG_BGFRAME;
	fmb->g3Dcamera = g3Dcamera;
  
  {	//font
		fmb->fontHandle = GFL_FONT_Create(
			ARCID_FONT,
//    NARC_font_large_nftr, //���t�H���gID
      NARC_font_large_gftr, //�V�t�H���gID
			GFL_FONT_LOADTYPE_FILE, FALSE, fmb->heapID );
	}
	
	{	//print
		fmb->printQue = PRINTSYS_QUE_Create( fmb->heapID );
	}
	
  { //PRINT_STREAM TCB
    fmb->printTCBLSys = GFL_TCBL_Init(
        fmb->heapID, fmb->heapID, FLDMSGBG_PRINT_STREAM_MAX, 4 );
  }
  
  //FLDMSGBG_SetupResource( fmb );
	return( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG ���\�[�X�Z�b�g�A�b�v
 * @param	fmb FLDMSGBG
 * @retval nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_SetupResource( FLDMSGBG *fmb )
{
  setBGResource( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �폜
 * @param	fmb	FLDMSGBG
 * @retval	nothing
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
    GFL_TCBL_Exit( fmb->printTCBLSys );
  }
	
  if( fmb->bgFrame != BGFRAME_ERROR ){
    GFL_BG_FillCharacterRelease( fmb->bgFrame, 1, 0 );
	  GFL_BG_FreeBGControl( fmb->bgFrame );
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
  
  setBGResource( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g����
 * @param	fmb	FLDMSGBG
 * @retval	nothing
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
 * @param	fmb	FLDMSGBG
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb )
{
	PRINTSYS_QUE_Clear( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �v�����g�L���[�ɒ��܂��Ă��鏈����S�Ď��s
 * @param	fmb	FLDMSGBG
 * @retval	nothing
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
 * @param	fmb	FLDMSGBG
 * @retval	BOOL TRUE=����
 */
//--------------------------------------------------------------
BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb )
{
	return( PRINTSYS_QUE_IsFinished(fmb->printQue) );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �g�p���Ă���PRINT_QUE��Ԃ�
 * @param	fmb	FLDMSGBG
 * @retval	PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb )
{
	return( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �g�p���Ă���GFL_FONT��Ԃ�
 * @param	fmb	FLDMSGBG
 * @retval	GFL_FONT*
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
 * @param	fmb	FLDMSGBG
 * @param	arcDatIDMsg	���b�Z�[�W���܂܂��A�[�J�C�u�f�[�^ID
 * @retval	GFL_MSGDATA*
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

//======================================================================
//	FLDMSGPRINT	�t�B�[���h�����\���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGPRINT �v�����g�ݒ�
 * @param	fmb	FLDMSGBG
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmpwin	�\�����鏉�����ς݂�GFL_BMPWIN
 * @retval	FLDMSGPRINT*
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
 * @param	msgPrint	FLDMSGPRINT FLDMSGPRINT_SetupPrint()�̖߂�l
 * @retval	nothing 
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
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		�\��X���W
 * @param	y		�\��Y���W
 * @param	strID	���b�Z�[�W�f�[�^ ������ID
 * @retval	nothing
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
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		�\��X���W
 * @param	y		�\��Y���W
 * @param	strID	���b�Z�[�W�f�[�^ ������ID
 * @param color PRINTSYS_LSB
 * @retval	nothing
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
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		�\��X���W
 * @param	y		�\��Y���W
 * @param	strBuf	�\������STRBUF
 * @retval	nothing
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
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		�\��X���W
 * @param	y		�\��Y���W
 * @param	strBuf	�\������STRBUF
 * @param color PRINTSYS_LSB
 * @retval	nothing
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
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		�\��X���W
 * @param	y		�\��Y���W
 * @param	strBuf	�\������STRBUF
 * @param color PRINTSYS_LSB
 * @retval	nothing
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
 * @param	msgPrint	FLDMSGPRINT
 * @retval	BOOL	TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->printTransFlag );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �\������r�b�g�}�b�v�E�B���h�E��ύX����
 * @param	msgPrint	FLDMSGPRINT
 * @param	bmpwin	�\�����鏉�����ς݂�GFL_BMPWIN
 * @retval	nothing
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
 * @param	msgPrint
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint )
{
	GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgPrint->bmpwin );
	GFL_BMP_Clear( bmp, 0xff );
	GFL_BG_LoadScreenReq( msgPrint->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT �r�b�g�}�b�v��`�N���A
 * @param	msgPrint
 * @param x �������ݐ揑�����݊J�nX���W�i�h�b�g�j
 * @param y �������ݐ揑�����݊J�nY���W�i�h�b�g�j
 * @param size_x  �`��͈�X�T�C�Y�i�h�b�g�j
 * @param size_y  �`��͈�Y�T�C�Y�i�h�b�g�j
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_FillClearBmp(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 size_x, u32 size_y )
{
	GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgPrint->bmpwin );
  GFL_BMP_Fill( bmp, x, y, size_x, size_y, 0xff );
	GFL_BG_LoadScreenReq( msgPrint->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@PRINT_QUE�擾
 * @param	msgPrint	FLDMSGPRINT
 * @retval	PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGPRINT_GetPrintQue( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@GFL_FONT�擾
 * @param	msgPrint	FLDMSGPRINT
 * @retval	GFL_FONT*
 */
//--------------------------------------------------------------
GFL_FONT * FLDMSGPRINT_GetFontHandle( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->fontHandle );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@PRINT_UTIL�擾
 * @param	msgPrint	FLDMSGPRINT
 * @retval	PRINT_UTIL*
 */
//--------------------------------------------------------------
PRINT_UTIL * FLDMSGPRINT_GetPrintUtil( FLDMSGPRINT *msgPrint )
{
	return( &msgPrint->printUtil );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT�@STRBUF�擾
 * @param	msgPrint	FLDMSGPRINT
 * @retval	STRBUF*
 */
//--------------------------------------------------------------
STRBUF * FLDMSGPRINT_GetStrBuf( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->strBuf );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT MSGDATA�擾
 * @param	msgPrint	FLDMSGPRINT
 * @retval	MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * FLDMSGPRINT_GetMsgData( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->msgData );
}

//======================================================================
//	FLDMSGWIN	���b�Z�[�W�E�B���h�E�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�ǉ��@���C��
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @param pltt_no     �g�p����p���b�g�ԍ�
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
static FLDMSGWIN * fldmsgwin_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y, u16 pltt_no )
{
	FLDMSGWIN *msgWin;
  
	msgWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMSGWIN) );
	msgWin->fmb = fmb;
	msgWin->bmpwin = winframe_InitBmp( fmb->bgFrame, fmb->heapID,
    bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, pltt_no );
	msgWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, msgWin->bmpwin );
	
  if( pltt_no == PANO_FONT ){
    winframe_SetPaletteBlack( fmb->heapID );
    setBlendAlpha( TRUE );
  }
  
	return( msgWin );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�ǉ�
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDMSGWIN *msgWin;
  fmb->deriveWin_plttNo = PANO_FONT;
  msgWin = fldmsgwin_Add( fmb, msgData,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, fmb->deriveWin_plttNo );
  return( msgWin );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E �폜�B
 * FLDMSGWIN_Add()����msgData�͊e���ōs�����B
 * @param	msgWin	FLDMSGWIN*
 * @retval	nothing
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
 * @param	msgWin	FLDMSGWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strID	���b�Z�[�W�f�[�^ ������ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_Print( FLDMSGWIN *msgWin, u16 x, u16 y, u32 strID )
{
	FLDMSGPRINT_Print( msgWin->msgPrint, x, y, strID );
	GFL_BG_LoadScreenReq( FLDMSGBG_BGFRAME );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��
 * @param	msgWin	FLDMSGWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_PrintStrBuf( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf )
{
	FLDMSGPRINT_PrintStrBuf( msgWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��A�J���[�w�肠��
 * @param	msgWin	FLDMSGWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
 * @param color PRINTSYS_LSB
 * @retval	nothing
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
 * @param	msgWin	FLDMSGWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_PrintStrBufColorFontHandle( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf, PRINTSYS_LSB color, GFL_FONT *font_handle )
{
	FLDMSGPRINT_PrintStrBufColorFontHandle( msgWin->msgPrint, x, y, strBuf, color, font_handle );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@�]���I���`�F�b�N
 * @param	msgWin	FLDMSGWIN
 * @retval	BOOL	TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin )
{
	return( FLDMSGPRINT_CheckPrintTrans(msgWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * ���b�Z�[�W�E�B���h�E�@���b�Z�[�W�\���N���A
 * @param	msgWin	FLDMSGWIN
 * @retval	nothing
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
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
	return( FLDMSGWIN_Add(fmb,msgData,1,19,30,4) );
}

//======================================================================
//	FLDSYSWIN	�V�X�e���E�B���h�E�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�ǉ��@���C��
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval	FLDSYSWIN*
 */
//--------------------------------------------------------------
static FLDSYSWIN * syswin_Add(
  FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_y )
{
	FLDSYSWIN *sysWin;
  
	sysWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDSYSWIN) );
	sysWin->fmb = fmb;
	sysWin->bmpwin = syswin_InitBmp( bmppos_y, fmb->heapID );
	sysWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, sysWin->bmpwin );
	
  setBlendAlpha( TRUE );
	return( sysWin );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@�ǉ�
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @retval	FLDSYSWIN*
 */
//--------------------------------------------------------------
FLDSYSWIN * FLDSYSWIN_Add(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData, u16 bmppos_y )
{
  FLDSYSWIN *sysWin;
  fmb->deriveWin_plttNo = PANO_FONT;
  sysWin = syswin_Add( fmb, msgData, bmppos_y );
  return( sysWin );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E �폜�B
 * FLDSYSWIN_Add()����msgData�͊e���ōs�����B
 * @param	sysWin	FLDSYSWIN*
 * @retval	nothing
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
 * @param	sysWin	FLDSYSWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strID	���b�Z�[�W�f�[�^ ������ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_Print( FLDSYSWIN *sysWin, u16 x, u16 y, u32 strID )
{
	FLDMSGPRINT_Print( sysWin->msgPrint, x, y, strID );
	GFL_BG_LoadScreenReq( FLDMSGBG_BGFRAME );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��
 * @param	sysWin	FLDSYSWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDSYSWIN_PrintStrBuf( FLDSYSWIN *sysWin, u16 x, u16 y, STRBUF *strBuf )
{
	FLDMSGPRINT_PrintStrBuf( sysWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\�� STRBUF�w��A�J���[�w�肠��
 * @param	sysWin	FLDSYSWIN
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
 * @param color PRINTSYS_LSB
 * @retval	nothing
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
 * @param	sysWin	FLDSYSWIN
 * @retval	BOOL	TRUE=�]���I��
 */
//--------------------------------------------------------------
BOOL FLDSYSWIN_CheckPrintTrans( FLDSYSWIN *sysWin )
{
	return( FLDMSGPRINT_CheckPrintTrans(sysWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * �V�X�e���E�B���h�E�@���b�Z�[�W�\���N���A
 * @param	sysWin	FLDSYSWIN
 * @retval	nothing
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
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @retval	FLDSYSWIN*
 */
//--------------------------------------------------------------
FLDSYSWIN * FLDSYSWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
	return( FLDSYSWIN_Add(fmb,msgData,19) );
}

//======================================================================
//	FLDMENUFUNC�@�t�B�[���h���j���[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ� ���C��
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param pltt_no �g�p����p���b�g�ԍ�
 * @param call_back    BMPMENULIST_HEADER�̃J�[�\���ړ����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param work    BMPMENULIST_HEADER�ɃZ�b�g���郏�[�N(BmpMenuList_GetWorkPtr�Ŏ��o���BNULL��)
 * @retval	FLDMENUFUNC*
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
	
	menuFunc = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMENUFUNC) );
	FldMenuFuncH_BmpMenuListH( &menuH, pMenuHead );
	
	menuFunc->fmb = fmb;
	menuFunc->pMenuListData = (BMP_MENULIST_DATA *)pMenuListData;
	
	menuFunc->bmpwin = winframe_InitBmp(
		fmb->bgFrame, fmb->heapID,
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
//	BmpMenuList_SetCursorString( menuFunc->pMenuListWork, 0 );
	BmpMenuList_SetCursorBmp( menuFunc->pMenuListWork, fmb->heapID );
  
  if( pltt_no == PANO_FONT ){
    winframe_SetPaletteBlack( fmb->heapID );
    setBlendAlpha( TRUE );
  }
	return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ��@BMP���X�g��1��\����Ƃ̃R�[���o�b�N�w��L���
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param icon    BMPMENULIST_HEADER�̈��\�����Ƃ̃R�[���o�b�N�֐��w��(NULL��)
 * @param work    BMPMENULIST_HEADER�ɃZ�b�g���郏�[�N(BmpMenuList_GetWorkPtr�Ŏ��o���BNULL��)
 * @retval	FLDMENUFUNC*
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
  fmb->deriveWin_plttNo = PANO_FONT;
  menuFunc = fldmenufunc_AddMenuList( fmb, pMenuHead, pMenuListData,
    list_pos, cursor_pos, fmb->deriveWin_plttNo, callback, icon, work );
  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC ���j���[�ǉ�
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @retval	FLDMENUFUNC*
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
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @retval	FLDMENUFUNC*
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
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete���Ɏ����J�������
 * @param list_pos ���X�g�����ʒu
 * @param cursor_pos �J�[�\�������ʒu
 * @param cancel TRUE=�L�����Z����
 * @retval	FLDMENUFUNC*
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
    list_pos, cursor_pos, fmb->deriveWin_plttNo, callback, NULL, cb_work );
  
  if( cancel == FALSE ){
    BmpMenuList_SetCancelMode(
        menuFunc->pMenuListWork, BMPMENULIST_CANCELMODE_NOT );
  }

  return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC	�폜
 * @param	menuFunc	FLDMENUFUNC*
 * @retval	nothing
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
 * @param	menuFunc	FLDMENUFUNC*
 * @retval	u32		���X�g�I���ʒu�̃p�����[�^
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
 * @param   menuFunc		FLDMENUFUNC*
 */
//==================================================================
void FLDMENUFUNC_Rewrite( FLDMENUFUNC *menuFunc )
{
  BmpMenuList_Rewrite( menuFunc->pMenuListWork );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATA���쐬
 * @param	max	���X�g��
 * @param	heapID	�쐬�p�q�[�vID
 * @retval	FLDMENUFUNC_LISTDATA*
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
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @retval	nothing
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
 * @param	menuList	FLDMENUFUNC_LIST
 * @param	max			menuList�v�f��
 * @param	msgData		�������
 * @param	heapID		FLDMENUFUNC_LISTDATA�쐬�p�q�[�vID
 * @retval	FLDMENUFUNC_LISTDATA
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
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @param	strBuf		STRBUF
 * @param	param		�p�����^
 * @param	heapID		�q�[�vID
 * @retval	nothing
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
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @param	strBuf		STRBUF
 * @param	param		�p�����^
 * @param	heapID		�q�[�vID
 * @retval	nothing
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
 * @retval u32 ���X�g���A�ő啶����
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListLengthMax( const FLDMENUFUNC_LISTDATA *listData )
{
  u32 len;
  len = BmpMenuWork_GetListMaxLength( (const BMP_MENULIST_DATA*)listData );
  return( len );
}

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
 * @param listData FLDMENUFUNC_LISTDATA
 * @param font_size �t�H���g���T�C�Y
 * @param space ������̕\���Ԋu
 * @retval u32 ���j���[���� �L�����P��
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_GetListMenuWidth(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size, u32 space )
{
  u32 c,len;
  u32 num = FLDMENUFUNC_GetListLengthMax( listData );
  num++; //�J�[�\����
  len = num * font_size;
  c = len / 8;
  if( (len & 0x07) ){ c++; }
  if( space ){ c += ((num*space)/8)+1; }
  return( c );
}

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
 * @param	head	FLDMENUFUNC_HEADER
 * @param	list_count	���X�g��
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @retval	nothing
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
 * @param	menuH	BMPMENULIST_HEADER
 * @param	fmenuH	FLDMENUFUNC_HEADER
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMenuFuncH_BmpMenuListH(
	BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH )
{
	GFL_STD_MemClear( menuH, sizeof(BMPMENULIST_HEADER) );
	menuH->count = fmenuH->count;			//���X�g���ڐ�
	menuH->line = fmenuH->line;				//�\���ő區�ڐ�
	menuH->label_x = fmenuH->label_x;		//���x���\���w���W
	menuH->data_x = fmenuH->data_x;			//���ڕ\���w���W
	menuH->cursor_x = fmenuH->cursor_x;		//�J�[�\���\���w���W
	menuH->line_y = fmenuH->line_y;			//�\���x���W
	menuH->f_col = fmenuH->f_col;			//�\�������F
	menuH->b_col = fmenuH->b_col;			//�\���w�i�F
	menuH->s_col = fmenuH->s_col;			//�\�������e�F
	menuH->msg_spc = fmenuH->msg_spc;		//�����Ԋu�w
	menuH->line_spc = fmenuH->line_spc;		//�����Ԋu�x
	menuH->page_skip = fmenuH->page_skip;	//�y�[�W�X�L�b�v�^�C�v
	menuH->font_size_x = fmenuH->font_size_x;	//�����T�C�YX(�h�b�g
	menuH->font_size_y = fmenuH->font_size_y;	//�����T�C�YY(�h�b�g
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
      0, cursor_pos, fmb->deriveWin_plttNo, NULL, NULL, NULL );
  
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
 * @param   wait		�P�����`�悲�Ƃ̃E�F�C�g�t���[����
 * @retval FLDMSGPRINT_STREAM
 */
//--------------------------------------------------------------
FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrintColor(
	FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait, u16 n_color )
{
  FLDMSGPRINT_STREAM *stm = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGPRINT_STREAM) );
  
  stm->msg_wait = wait;
  
  stm->printStream = PRINTSYS_PrintStream(
      bmpwin, x, y,
      strbuf, fmb->fontHandle,
      wait,
      fmb->printTCBLSys, 0,
      fmb->heapID, n_color );

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
 * @param   wait		�P�����`�悲�Ƃ̃E�F�C�g�t���[����
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
  int trg,cont;
  PRINTSTREAM_STATE state;
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
  state = PRINTSYS_PrintStreamGetState( stm->printStream );
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    if( (trg & MSG_SKIP_BTN) ){
      stm->flag_key_trg = TRUE;
    }
    
    if( stm->flag_key_trg == TRUE && (cont & MSG_SKIP_BTN) ){
      PRINTSYS_PrintStreamShortWait( stm->printStream, 0 );
    }
#if 0
    else{
      PRINTSYS_PrintStreamShortWait( stm->printStream, stm->msg_wait );
    }
#endif
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( (trg & MSG_LAST_BTN) ){
      PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
      PRINTSYS_PrintStreamReleasePause( stm->printStream );
      stm->flag_key_trg = FALSE;
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
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @retval FLDMSGWIN_STREAM*
 */
//--------------------------------------------------------------
FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_Add(
    FLDMSGBG *fmb, const GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  FLDMSGWIN_STREAM *msgWin;
	
  fmb->deriveWin_plttNo = PANO_FONT;
  
	msgWin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGWIN_STREAM) );
	msgWin->fmb = fmb;
  msgWin->msgData = msgData;
	msgWin->bmpwin = winframe_InitBmp( fmb->bgFrame, fmb->heapID,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, fmb->deriveWin_plttNo );
  msgWin->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
  
  keyCursor_Init( &msgWin->cursor_work, fmb->heapID );

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
  keyCursor_Delete( &msgWin->cursor_work );
	GFL_HEAP_FreeMemory( msgWin );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�\���J�n
 * @param msgWin FLDMSGWIN_STREAM*
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	msgWin->fmb, msgWin->strBuf, msgWin->bmpwin, x, y, MSGSPEED_GetWait() );
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param msgWin FLDMSGWIN_STREAM*
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	msgWin->fmb, strBuf, msgWin->bmpwin, x, y, MSGSPEED_GetWait() );
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
#if 0
  if( FLDMSGPRINT_STREAM_ProcPrint(msgWin->msgPrintStream) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
#else
  int trg,cont;
  PRINTSTREAM_STATE state;
  state = fldMsgPrintStream_ProcPrint( msgWin->msgPrintStream );
  
  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();

  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    msgWin->flag_cursor = CURSOR_FLAG_NONE;
    msgWin->flag_key_pause_clear = FALSE;
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( msgWin->flag_key_pause_clear == FALSE ){ //���Ƀ|�[�Y�N���A�ς݂��H
		  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgWin->bmpwin );
      
      if( (trg & MSG_LAST_BTN) ){
        if( msgWin->flag_cursor == CURSOR_FLAG_WRITE ){
          keyCursor_Clear( &msgWin->cursor_work, bmp, 0x0f );
          
          msgWin->flag_key_pause_clear = TRUE;
          msgWin->flag_cursor = CURSOR_FLAG_END;
        }
      }
      
      if( msgWin->flag_cursor != CURSOR_FLAG_END ){
        keyCursor_Write( &msgWin->cursor_work, bmp, 0x0f );
        msgWin->flag_cursor = CURSOR_FLAG_WRITE;
      }
      
		  GFL_BMPWIN_TransVramCharacter( msgWin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    return( TRUE );
  }

  return( FALSE );
#endif
}

//--------------------------------------------------------------
/**
 * FLDMSGWIN_STREAM ���b�Z�[�W�E�B���h�E�ǉ� ��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
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
      WINDOW_TRANS_ON, 1, PANO_MENU );
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

//======================================================================
//  FLDSYSWIN_STREAM
//======================================================================
//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�ǉ�
 * @param	fmb	FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @retval FLDSYSWIN_STREAM*
 */
//--------------------------------------------------------------
FLDSYSWIN_STREAM * FLDSYSWIN_STREAM_Add(
    FLDMSGBG *fmb, const GFL_MSGDATA *msgData, u16 bmppos_y )
{
  FLDSYSWIN_STREAM *sysWin;
	
  fmb->deriveWin_plttNo = PANO_FONT;
  
	sysWin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDSYSWIN_STREAM) );
	sysWin->fmb = fmb;
  sysWin->msgData = msgData;
	sysWin->bmpwin = syswin_InitBmp( bmppos_y, fmb->heapID );
  sysWin->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
  
  keyCursor_Init( &sysWin->cursor_work, fmb->heapID );
  
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
  keyCursor_Delete( &sysWin->cursor_work );
	GFL_HEAP_FreeMemory( sysWin );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�\���J�n
 * @param sysWin FLDSYSWIN_STREAM*
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	sysWin->fmb, sysWin->strBuf, sysWin->bmpwin, x, y, MSGSPEED_GetWait() );
}

//--------------------------------------------------------------
/**
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param sysWin FLDSYSWIN_STREAM*
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	sysWin->fmb, strBuf, sysWin->bmpwin, x, y, MSGSPEED_GetWait() );
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
    sysWin->flag_cursor = CURSOR_FLAG_NONE;
    sysWin->flag_key_pause_clear = FALSE;
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( sysWin->flag_key_pause_clear == FALSE ){ //���Ƀ|�[�Y�N���A�ς݂��H
		  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( sysWin->bmpwin );
      
      if( (trg & MSG_LAST_BTN) ){
        if( sysWin->flag_cursor == CURSOR_FLAG_WRITE ){
          keyCursor_Clear( &sysWin->cursor_work, bmp, 0x0f );
          
          sysWin->flag_key_pause_clear = TRUE;
          sysWin->flag_cursor = CURSOR_FLAG_END;
        }
      }
      
      if( sysWin->flag_cursor != CURSOR_FLAG_END ){
        keyCursor_Write( &sysWin->cursor_work, bmp, 0x0f );
        sysWin->flag_cursor = CURSOR_FLAG_WRITE;
      }
      
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
 * FLDSYSWIN_STREAM �V�X�e���E�B���h�E�ǉ� ��b�E�B���h�E�^�C�v
 * @param fmb FLDMSGBG*
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
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

//======================================================================
//  FLDTALKMSGWIN
//======================================================================
#define GIZA_SHAKE_Y (8)

//--------------------------------------------------------------
/// FLDTALKMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDTALKMSGWIN
{
  u8 flag_key_trg;
  u8 flag_key_cont;
  u8 flag_key_pause_clear;
  u8 flag_cursor;
  STRBUF *strBuf;
  int talkMsgWinIdx;
  TALKMSGWIN_SYS *talkMsgWinSys; //FLDMSGBG���
  KEYCURSOR_WORK cursor_work;
  
  int shake_y;
};

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
    TALKMSGWIN_TYPE type )
{
  GF_ASSERT( fmb->talkMsgWinSys != NULL );
  
  fmb->deriveWin_plttNo = PANO_FONT_TALKMSGWIN;

  tmsg->talkMsgWinSys = fmb->talkMsgWinSys;
  tmsg->talkMsgWinIdx = idx;
  
  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );

  switch( idx ){
  case FLDTALKMSGWIN_IDX_UPPER:
    TALKMSGWIN_CreateFixWindowUpper( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_UPPER, (VecFx32*)pos, strBuf, 15, type );
    break;
  case FLDTALKMSGWIN_IDX_LOWER:
    TALKMSGWIN_CreateFixWindowLower( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_LOWER, (VecFx32*)pos, strBuf, 15, type );
    break;
  default:
    TALKMSGWIN_CreateFixWindowAuto( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_AUTO, (VecFx32*)pos, strBuf, 15, type );
  }
  
  TALKMSGWIN_OpenWindow( tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  
  keyCursor_Init( &tmsg->cursor_work, fmb->heapID );
  
  if( type == TALKMSGWIN_TYPE_GIZA ){
    tmsg->shake_y = GIZA_SHAKE_Y;
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
    const GFL_MSGDATA *msgData, u32 msgID, TALKMSGWIN_TYPE type )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  tmsg->strBuf = GFL_STR_CreateBuffer(
					FLDMSGBG_STRLEN, fmb->heapID );
  
  GFL_MSG_GetString( msgData, msgID, tmsg->strBuf );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, tmsg->strBuf, type );
  return( tmsg );
}

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
    STRBUF *strBuf, TALKMSGWIN_TYPE type )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, strBuf, type );
  return( tmsg );
}

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
  
  keyCursor_Delete( &tmsg->cursor_work );
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
  if( TALKMSGWIN_CheckCloseStatus(tmsg->talkMsgWinSys,tmsg->talkMsgWinIdx) ){
    GFL_HEAP_FreeMemory( tmsg );
    return( TRUE );
  }
  
  return( FALSE );
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
  keyCursor_Delete( &tmsg->cursor_work );
  GFL_HEAP_FreeMemory( tmsg );
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
  int trg,cont;
  PRINTSTREAM_STATE state;
  PRINT_STREAM *stream;
  PRINTSTREAM_PAUSE_TYPE pause_type;

  if( TALKMSGWIN_CheckPrintOn(
        tmsg->talkMsgWinSys,tmsg->talkMsgWinIdx) == FALSE ){
    return( FALSE );
  }

  trg = GFL_UI_KEY_GetTrg();
  cont = GFL_UI_KEY_GetCont();
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
    tmsg->flag_cursor = CURSOR_FLAG_NONE;
    tmsg->flag_key_pause_clear = FALSE;
    
    if( (trg & MSG_SKIP_BTN) ){
      tmsg->flag_key_trg = TRUE;
    }
    
    if( tmsg->flag_key_trg == TRUE && (cont & MSG_SKIP_BTN) ){
      PRINTSYS_PrintStreamShortWait( stream, 0 );
    }
#if 0
    else{
      PRINTSYS_PrintStreamShortWait( stream, MSGSPEED_GetWait() );
    }
#endif
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( tmsg->flag_key_pause_clear == FALSE ){ //���Ƀ|�[�Y�N���A�ς݂��H
      GFL_BMPWIN *twin_bmp = TALKMSGWIN_GetBmpWin(
          tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
		  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( twin_bmp );
      
      if( (trg & MSG_LAST_BTN) ){
        PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
        PRINTSYS_PrintStreamReleasePause( stream );
        
        if( tmsg->flag_cursor == CURSOR_FLAG_WRITE ){
          keyCursor_Clear( &tmsg->cursor_work, bmp, 0x0f );
        }
        
        tmsg->flag_key_trg = FALSE;
        tmsg->flag_key_pause_clear = TRUE;
        tmsg->flag_cursor = CURSOR_FLAG_END;
      }
      
      if( tmsg->flag_cursor != CURSOR_FLAG_END ){
        keyCursor_Write( &tmsg->cursor_work, bmp, 0x0f );
        tmsg->flag_cursor = CURSOR_FLAG_WRITE;
      }
      
		  GFL_BMPWIN_TransVramCharacter( twin_bmp );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    { //shake
      GFL_BG_SetScroll( FLDMSGBG_BGFRAME, GFL_BG_SCROLL_Y_SET, 0 );
    }
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  �v���[���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/// FLDPLAINMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDPLAINMSGWIN
{
  u8 flag_key_trg;
  u8 flag_key_cont;
  u8 flag_key_pause_clear;
  u8 flag_cursor;
  KEYCURSOR_WORK cursor_work;
  
  STRBUF *strBuf;
  FLDMSGBG *fmb;
  TALKMSGWIN_SYS *talkMsgWinSys; //FLDMSGBG���
  GFL_MSGDATA *msgData;	//���[�U�[����
  
  GFL_BMPWIN *bmpwin;
	FLDMSGPRINT_STREAM *msgPrintStream;
  FLDMSGPRINT *msgPrint;
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
  
  fmb->deriveWin_plttNo = PANO_FONT_TALKMSGWIN;
  
  plnwin->talkMsgWinSys = fmb->talkMsgWinSys;

  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );
  
  plnwin->bmpwin = TALKMSGWIN_CreateBmpWindow(
      plnwin->talkMsgWinSys,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y,
      type );
  
  keyCursor_Init( &plnwin->cursor_work, fmb->heapID );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN �v���[���E�B���h�E�ǉ�
 * @param fmb FLDMSGBG
 * @param	msgData	�\�����鏉�����ς݂�GFL_MSGDATA NULL=�g�p���Ȃ�
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
 * @retval FLDPLAINMSGWIN*
 */
//--------------------------------------------------------------
FLDPLAINMSGWIN * FLDPLAINMSGWIN_Add(
    FLDMSGBG *fmb,  const GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
  TALKMSGWIN_TYPE type = TEST_TALKMSGWIN_TYPE;
  FLDPLAINMSGWIN *plnwin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDPLAINMSGWIN) );
  plnwin->fmb = fmb;
  plnwin->strBuf = GFL_STR_CreateBuffer(
					FLDMSGBG_STRLEN, fmb->heapID );
  fldPlainMsgWin_Add( fmb, plnwin, bmppos_x, bmppos_y,
      bmpsize_x, bmpsize_y, type );
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
  keyCursor_Delete( &plnwin->cursor_work );
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
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
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
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
 * @param	strBuf	STRBUF
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
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	plnwin->fmb, plnwin->strBuf, plnwin->bmpwin, x, y, MSGSPEED_GetWait() );
}

//--------------------------------------------------------------
/**
 * FLDPLAINMSGWIN ���b�Z�[�W�X�g���[���\���J�n�@STRBUF�w��
 * @param msgWin FLDPLAINMSGWIN *
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  	plnwin->fmb, strBuf, plnwin->bmpwin, x, y, MSGSPEED_GetWait() );
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
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    plnwin->flag_cursor = CURSOR_FLAG_NONE;
    plnwin->flag_key_pause_clear = FALSE;
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( plnwin->flag_key_pause_clear == FALSE ){ //���Ƀ|�[�Y�N���A�ς݂��H
		  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plnwin->bmpwin );
      
      if( (trg & MSG_LAST_BTN) ){
//        PMSND_PlaySystemSE( SEQ_SE_MESSAGE );
//        PRINTSYS_PrintStreamReleasePause( stream );
        if( plnwin->flag_cursor == CURSOR_FLAG_WRITE ){
          keyCursor_Clear( &plnwin->cursor_work, bmp, 0x0f );
          plnwin->flag_key_pause_clear = TRUE;
          plnwin->flag_cursor = CURSOR_FLAG_END;
        }
      }
      
      if( plnwin->flag_cursor != CURSOR_FLAG_END ){
        keyCursor_Write( &plnwin->cursor_work, bmp, 0x0f );
        plnwin->flag_cursor = CURSOR_FLAG_WRITE;
      }
      
		  GFL_BMPWIN_TransVramCharacter( plnwin->bmpwin );
    }
    break;
  case PRINTSTREAM_STATE_DONE: //�I��
    return( TRUE );
  }
  
  return( FALSE );
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
  KEYCURSOR_WORK cursor_work;
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
  
  fmb->deriveWin_plttNo = PANO_FONT_TALKMSGWIN;
  
  subwin->talkMsgWinSys = fmb->talkMsgWinSys;
  subwin->talkMsgWinIdx = idx;
  subwin->id = id;
  
  winframe_SetPaletteWhith( fmb->heapID );
  setBlendAlpha( FALSE );
  
  TALKMSGWIN_CreateWindowAlone(
    subwin->talkMsgWinSys, subwin->talkMsgWinIdx,
    strBuf, x, y, sx, sy, GX_RGB(31,31,31), type );
  
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
    const GFL_MSGDATA *msgData, u32 msgID,
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
  FLDBGWIN *bgWin;
  
  bgWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDBGWIN) );
  bgWin->fmb = fmb;
  
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
  
  keyCursor_Init( &bgWin->cursor_work, bgWin->fmb->heapID );
  
  fmb->deriveWin_plttNo = PANO_FONT_TALKMSGWIN;
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
  
  keyCursor_Delete( &bgWin->cursor_work );
  GFL_HEAP_FreeMemory( bgWin ); 
}

//--------------------------------------------------------------
/**
 * �t�B�[���hBG�E�B���h�E�@�v�����g
 * @param FLDBGWIN *bgWin
 * @param	strID	���b�Z�[�W�f�[�^ ������ID
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
        GFL_BMPWIN_GetBmp(bgWin->bmpwin), bgWin->fmb->fontHandle,
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
    if( !(GFL_UI_KEY_GetTrg()&PAD_BUTTON_A) ){
      keyCursor_Write( &bgWin->cursor_work,
          GFL_BMPWIN_GetBmp(bgWin->bmpwin), BGWIN_NCOL );
		  GFL_BMPWIN_TransVramCharacter( bgWin->bmpwin );
      break;
    }
    
    keyCursor_Clear( &bgWin->cursor_work,
          GFL_BMPWIN_GetBmp(bgWin->bmpwin), BGWIN_NCOL );
    
    GFL_BMP_Copy( GFL_BMPWIN_GetBmp(bgWin->bmpwin), bgWin->bmp_old );
    
    bgwin_PrintStr(
        bgWin->bmp_new, bgWin->fmb->fontHandle,
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
	GFL_BG_FillScreen( frm, cgx+3, px+sx,	py-1, 1, 1, pal );
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
	GFL_BG_FillScreen( frm, cgx, px+sx,	py-1, 1, 1, pal );
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
 * @param	bmppos_x		//�\�����WX �L�����P��
 * @param	bmppos_y		//�\�����WY �L�����P��
 * @param	bmpsize_x		//�\���T�C�YX �L�����P��
 * @param	bmpsize_y		//�\���T�C�YY �L�����P��
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
  
  keyCursor_Init( &spWin->cursor_work, fmb->heapID );

  fmb->deriveWin_plttNo = PANO_FONT_TALKMSGWIN;
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
  
  keyCursor_Delete( &spWin->cursor_work );

  GFL_BMP_Delete( spWin->bmp_bg );
  GFL_BMPWIN_Delete( spWin->bmpwin );
  GFL_HEAP_FreeMemory( spWin );
}

//--------------------------------------------------------------
/**
 * FLDSPWIN ����E�B���h�E ���b�Z�[�W�\���J�n STRBUF�w��
 * @param msgWin FLDMSGWIN_STREAM*
 * @param	x		X�\�����W
 * @param	y		Y�\�����W
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
  
  keyCursor_WriteBmpBG( &spWin->cursor_work,
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
 * �L�[����J�[�\�� ������
 * @param work KEYCURSOR_WORK
 * @param heapID HEAPID
 * @retval nothing
 */
//--------------------------------------------------------------
static void keyCursor_Init( KEYCURSOR_WORK *work, HEAPID heapID )
{
  MI_CpuClear8( work, sizeof(KEYCURSOR_WORK) );
  work->bmp_cursor = GFL_BMP_CreateWithData(
        (u8*)skip_cursor_Character,
        2, 2, GFL_BMP_16_COLOR, heapID );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �폜
 * @param work KEYCURSOR_WORK
 * @retval nothing
 */
//--------------------------------------------------------------
static void keyCursor_Delete( KEYCURSOR_WORK *work )
{
  GFL_BMP_Delete( work->bmp_cursor );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �N���A
 * @param work KEYCURSOR_WORK
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
static void keyCursor_Clear(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  GFL_BMP_Fill( bmp, x, y+offs, 10, 7, n_col );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\��
 * @param work KEYCURSOR_WORK
 * @param bmp �\����GFL_BMP_DATA
 * @param n_col �����F�w�� 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
static void keyCursor_Write(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col )
{
  u16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  keyCursor_Clear( work, bmp, n_col );
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }

  x = GFL_BMP_GetSizeX( bmp ) - 11;
  y = GFL_BMP_GetSizeY( bmp ) - 9;
  offs = tbl[work->cursor_anm_no];
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 2, x, y+offs, 10, 7, 0x00 );
}

//--------------------------------------------------------------
/**
 * �L�[����J�[�\�� �\���@�w�iBITMAP�w��
 * @param work KEYCURSOR_WORK
 * @param bmp �\����GFL_BMP_DATA
 * @param bmp_bg �w�i�ɒ���1�L��������GFL_BMP_DATA
 * @retval nothing
 */
//--------------------------------------------------------------
static void keyCursor_WriteBmpBG(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp,
    GFL_BMP_DATA *bmp_bg )
{
  s16 x,y,offs;
  u16 tbl[3] = { 0, 1, 2 };
  
  x = GFL_BMP_GetSizeX( bmp ) - (8 - 16);
  y = GFL_BMP_GetSizeY( bmp ) - (8 - 16);
  
  if( x >= 0 && y >= 0 ){ //BG�𒣂�̈悪����
    s16 ix,iy;
    for( iy = y; iy < (y+16); iy += 4 ){
      for( ix = x; ix < (x+16); ix += 4 ){
        GFL_BMP_Print( bmp_bg, bmp, 0, 0, ix, iy, 4, 4, GF_BMPPRT_NOTNUKI );
      }
    }
  }
  
  work->cursor_anm_frame++;
  
  if( work->cursor_anm_frame >= 4 ){
    work->cursor_anm_frame = 0;
    work->cursor_anm_no++;
    work->cursor_anm_no %= 3;
  }
  
  x = GFL_BMP_GetSizeX( bmp ) - (8 - 10);
  y = GFL_BMP_GetSizeY( bmp ) - (8 - 7 - 3);
  offs = tbl[work->cursor_anm_no];
  
  GFL_BMP_Print( work->bmp_cursor, bmp, 0, 2, x, y+offs, 10, 7, 0x00 );
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
  u8 pal = PANO_MENU;
  u8 frm = FLDMSGBG_BGFRAME;
  
	GFL_BG_FillScreen( frm, cgx, px-2, py-1, 1, 1, pal );
	GFL_BG_FillScreen( frm, cgx+1, px-1, py-1, 1, 1, pal );
	GFL_BG_FillScreen( frm, cgx+2, px, py-1, sx, 1, pal );
	GFL_BG_FillScreen( frm, cgx+3, px+sx,	py-1, 1, 1, pal );
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
 * @param	bgframe	BG�t���[��
 * @param	heapID	HEAPID
 * @param	pos_y	�r�b�g�}�b�v����Y �L�����P��
 * @retval	GFL_BMPWIN	�쐬���ꂽGFL_BMPWIN
 */
//--------------------------------------------------------------
static GFL_BMPWIN * syswin_InitBmp( u8 pos_y, HEAPID heapID )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;
  
	bmpwin = GFL_BMPWIN_Create( FLDMSGBG_BGFRAME,
		2, pos_y, 27, 4,
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
 * @param	bmpwin	syswin_InitBmp()�̖߂�l
 * @retval	nothing
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
 * @param	bmpwin	syswin_InitBmp()�̖߂�l
 * @retval	nothing
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
 * @retval nothing
 */
//--------------------------------------------------------------
static void setBGResource( FLDMSGBG *fmb )
{
	fmb->bgFrame = FLDMSGBG_BGFRAME;

	{	//BG������
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, FLDBG_MFRM_MSG_SCRSIZE, 0,
			GFL_BG_SCRSIZ_256x256, FLDBG_MFRM_MSG_COLORMODE,
			FLDBG_MFRM_MSG_SCRBASE, FLDBG_MFRM_MSG_CHARBASE, FLDBG_MFRM_MSG_CHARSIZE,
			GX_BG_EXTPLTT_01, FLDBG_MFRM_MSG_PRI, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		
		GFL_BG_FillCharacter( fmb->bgFrame, CHARNO_CLEAR, 1, 0 );
		GFL_BG_FillScreen( fmb->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		
		GFL_BG_LoadScreenReq( fmb->bgFrame );
	}
	
	{	//�t�H���g�p���b�g
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr, //��
			PALTYPE_MAIN_BG, PANO_FONT_TALKMSGWIN*32, 32, fmb->heapID );
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_systemwin_nclr, //��
			PALTYPE_MAIN_BG, PANO_FONT*32, 32, fmb->heapID );
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( fmb->bgFrame, CHARNO_WINFRAME,
			PANO_MENU, MENU_TYPE_SYSTEM, fmb->heapID );
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
	
  { //SYSWIN
    syswin_InitGraphic( fmb->heapID );
  }

  GFL_BG_SetPriority( GFL_BG_FRAME0_M, FLDBG_MFRM_3D_PRI );
  GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
  
  fmb->deriveWin_plttNo = PANO_FONT;
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
	
	{	//�t�H���g�p���b�g
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr, //��
			PALTYPE_MAIN_BG, PANO_FONT_TALKMSGWIN*32, 32, fmb->heapID );
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_systemwin_nclr, //��
			PALTYPE_MAIN_BG, PANO_FONT*32, 32, fmb->heapID );
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( fmb->bgFrame, CHARNO_WINFRAME,
			PANO_MENU, MENU_TYPE_SYSTEM, heapID );
	}

  { //�o���[���E�B���h�E
    TALKMSGWIN_ReTransWindowBG( fmb->talkMsgWinSys );
  }

  { //SYSWIN
    syswin_InitGraphic( heapID );
  }
}

//======================================================================
//	�p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@������
 * @param	bgframe	BG�t���[��
 * @param	heapID	HEAPID
 * @param	pos_x	�r�b�g�}�b�v����X �L�����P��
 * @param	pos_y	�r�b�g�}�b�v����Y �L�����P��
 * @param	size_x�@�r�b�g�}�b�v�T�C�YX �L�����P��
 * @param	size_y	�r�b�g�}�b�v�T�C�YY �L�����P��
 * @retval	GFL_BMPWIN	�쐬���ꂽGFL_BMPWIN
 */
//--------------------------------------------------------------
static GFL_BMPWIN * winframe_InitBmp( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;

	bmpwin = GFL_BMPWIN_Create( bgFrame,
		pos_x, pos_y, size_x, size_y,
//		PANO_FONT, GFL_BMP_CHRAREA_GET_B );
		pltt_no, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( bmpwin );
	
	GFL_BMP_Clear( bmp, 0xff );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BG_LoadScreenReq( bgFrame );
	
	BmpWinFrame_Write( bmpwin, WINDOW_TRANS_ON, 1, PANO_MENU );
	
	return( bmpwin );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@�폜
 * @param	bmpwin	winframe_InitBmp()�̖߂�l
 * @retval	nothing
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
  u32 pal = PANO_MENU;
  u32 arc = BmpWinFrame_WinPalArcGet();
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
  u32 pal = PANO_MENU;
  u32 arc = BmpWinFrame_WinPalArcGet();
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

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
//  �͂��A�������I�����j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_MenuHeader_YesNo =
{
	1,		//���X�g���ڐ�
	10,		//�\���ő區�ڐ�
	0,		//���x���\���w���W
	13,		//���ڕ\���w���W
	0,		//�J�[�\���\���w���W
//	0,		//�\���x���W
	3,		//�\���x���W
	1,		//�\�������F
	15,		//�\���w�i�F
	2,		//�\�������e�F
	0,		//�����Ԋu�w
	1,		//�����Ԋu�x
	FLDMENUFUNC_SKIP_NON,	//�y�[�W�X�L�b�v�^�C�v
	12,		//�����T�C�YX(�h�b�g
	12,		//�����T�C�YY(�h�b�g
	0,		//�\�����WX �L�����P��
	0,		//�\�����WY �L�����P��
	0,		//�\���T�C�YX �L�����P��
	0,		//�\���T�C�YY �L�����P��
};

//--------------------------------------------------------------
/// ����J�[�\���f�[�^ ���@PL���玝���Ă���
//--------------------------------------------------------------
#if 0
static const u8 ALIGN4 skip_cursor_Character[128] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x44,0x44,0x44,0x44,
0x42,0x44,0x44,0x44,0x20,0x44,0x44,0x24, 0x00,0x42,0x44,0x22,0x00,0x20,0x24,0x02,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,
0x22,0x00,0x00,0x00,0x02,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
#else
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
