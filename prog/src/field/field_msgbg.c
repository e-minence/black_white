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

#include "field/field_msgbg.h"

#include "sound/pm_sndsys.h"
#include "sound/wb_sound_data.sadl"

/*
extern GFL_BMPWIN * TALKMSGWIN_GetBmpWin( TALKMSGWIN_SYS* tmsgwinSys, int tmsgwinIdx );
*/

//======================================================================
//	define
//======================================================================
#define FLDMSGBG_BGFRAME (GFL_BG_FRAME1_M)	///<�g�pBG�t���[��

#define FLDMSGBG_PANO_FONT_TALKMSGWIN (11) ///<�����o���t�H���g�p���b�gNo
#define FLDMSGBG_PANO_TALKMSGWIN (12) ///<�����o���E�B���h�E�p���b�gNo
#define FLDMSGBG_PANO_MENU (13) ///<���j���[�p���b�gNo
#define FLDMSGBG_PANO_FONT (14) ///<�t�H���g�p���b�gNo

#define FLDMSGBG_PRINT_MAX (4)				///<PRINT�֘A�v�f���ő�
#define FLDMSGBG_PRINT_STREAM_MAX (1) ///<PRINT_STREAM�ғ���

#define FLDMSGBG_STRLEN (128)				///<�����񒷂��W��

#define FLDMSGBG_CHAROFFS_TALKMSGWIN (32) ///<TALKWINMSG�L�����I�t�Z�b�g

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
 
//======================================================================
//	proto
//======================================================================
static void FldMenuFuncH_BmpMenuListH(
	BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH );

static PRINTSTREAM_STATE fldMsgPrintStream_ProcPrint(
    FLDMSGPRINT_STREAM *stm );

static void keyCursor_Init( KEYCURSOR_WORK *work, HEAPID heapID );
static void keyCursor_Delete( KEYCURSOR_WORK *work );
static void keyCursor_Clear(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );
static void keyCursor_Write(
    KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );

static void setBlendAlpha( BOOL on );

static GFL_BMPWIN * FldBmpWinFrame_Init( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no );
static void FldBmpWinFrame_Delete( GFL_BMPWIN *bmpwin );

static const FLDMENUFUNC_HEADER DATA_MenuHeader_YesNo;
static const u8 ALIGN4 skip_cursor_Character[128];

//======================================================================
//	FLDMSGBG	�t�B�[���h���b�Z�[�WBG�֘A
//======================================================================
//--------------------------------------------------------------
//  10��ROM�p ���Ώ� �E�B���h�E�̉��J���[�𖳌��� 0x50001a0
//--------------------------------------------------------------
static void debug_ROM091030_WindowColorOFF( HEAPID heapID )
{
  u16 *buf = GFL_HEAP_AllocMemoryLo( heapID, 32 );
  u32 offs = HW_BG_PLTT + (32*FLDMSGBG_PANO_MENU);
  DC_FlushRange( (void*)offs, 32 );
  MI_CpuCopy( (void*)offs, buf, 32 );
  buf[1] = 0;
  DC_FlushRange( (void*)buf, 32 );
  GX_LoadBGPltt( (void*)buf, offs-HW_BG_PLTT, 32 );
  GFL_HEAP_FreeMemory( buf );
}

static void debug_ROM091030_WindowColorON( HEAPID heapID )
{
  u16 *buf = GFL_HEAP_AllocMemoryLo( heapID, 32 );
  u32 offs = HW_BG_PLTT + (32*FLDMSGBG_PANO_MENU);
  DC_FlushRange( (void*)offs, 32 );
  MI_CpuCopy( (void*)offs, buf, 32 );
  buf[1] = 0x7fff;
  DC_FlushRange( (void*)buf, 32 );
  GX_LoadBGPltt( (void*)buf, offs-HW_BG_PLTT, 32 );
  GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG �Z�b�g�A�b�v
 * @param	heapID	HEAPID
 * @param g3Dcamera GFL_G3D_CAMERA* �����o���E�B���h�E�Ŏg�p NULL=�g�p���Ȃ�
 * @retval	FLDMAPBG	FLDMAPBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FLDMSGBG_Setup( HEAPID heapID, GFL_G3D_CAMERA *g3Dcamera )
{
	FLDMSGBG *fmb;
	
	fmb = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMSGBG) );
	fmb->heapID = heapID;
	fmb->bgFrame = FLDMSGBG_BGFRAME;
	fmb->g3Dcamera = g3Dcamera;

  KAGAYA_Printf( "FLDMAPBG ����HEAPID = %d\n", fmb->heapID );
  
	{	//BG������
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( fmb->bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 3 );
		
		GFL_BG_FillCharacter( fmb->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( fmb->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		
		GFL_BG_LoadScreenReq( fmb->bgFrame );
	}
	
	{	//�t�H���g�p���b�g
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr, //��
			PALTYPE_MAIN_BG, FLDMSGBG_PANO_FONT_TALKMSGWIN*32, 32, fmb->heapID );
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_systemwin_nclr, //��
			PALTYPE_MAIN_BG, FLDMSGBG_PANO_FONT*32, 32, fmb->heapID );
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( fmb->bgFrame, 1,
			FLDMSGBG_PANO_MENU, MENU_TYPE_SYSTEM, heapID );
    debug_ROM091030_WindowColorOFF( heapID );
	}
	
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
  
  { //TALKMSGWIN
    if( fmb->g3Dcamera != NULL ){
      TALKMSGWIN_SYS_SETUP setup;
      setup.heapID = fmb->heapID;
      setup.g3Dcamera = g3Dcamera;
      setup.fontHandle = fmb->fontHandle;
      setup.chrNumOffs = FLDMSGBG_CHAROFFS_TALKMSGWIN;
      setup.ini.frameID = FLDMSGBG_BGFRAME;
      setup.ini.winPltID = FLDMSGBG_PANO_TALKMSGWIN;
      setup.ini.fontPltID = FLDMSGBG_PANO_FONT_TALKMSGWIN;
      fmb->talkMsgWinSys = TALKMSGWIN_SystemCreate( &setup );
    }
  }
  
  fmb->deriveWin_plttNo = FLDMSGBG_PANO_FONT;
//  FLDMSGBG_SetBlendAlpha();
	return( fmb );
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
  
  GFL_TCBL_Exit( fmb->printTCBLSys );
  
//GFL_BG_FreeCharacterArea( fmb->bgFrame, 0x00, 0x20 );
	GFL_BG_FillCharacterRelease( fmb->bgFrame, 0x00, 0x20 );

	GFL_BG_FreeBGControl( fmb->bgFrame );
	
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
	
	PRINTSYS_QUE_Delete( fmb->printQue );
	GFL_FONT_Delete( fmb->fontHandle );
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
  
	GFL_BG_FillCharacterRelease( fmb->bgFrame, 1, 0 );
  GFL_BG_FreeBGControl( fmb->bgFrame );
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
	{	//BG������
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( fmb->bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 3 );
		
		GFL_BG_FillCharacter( fmb->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( fmb->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		
		GFL_BG_LoadScreenReq( fmb->bgFrame );
	}
	
  {	//�t�H���g�p���b�g
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr, //��
			PALTYPE_MAIN_BG, FLDMSGBG_PANO_FONT_TALKMSGWIN*32, 32, fmb->heapID );
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_systemwin_nclr, //��
			PALTYPE_MAIN_BG, FLDMSGBG_PANO_FONT*32, 32, fmb->heapID );
	}
	
  {	//window frame
		BmpWinFrame_GraphicSet( fmb->bgFrame, 1,
			FLDMSGBG_PANO_MENU, MENU_TYPE_SYSTEM, fmb->heapID );
    debug_ROM091030_WindowColorOFF( fmb->heapID );
	}
  
  { //TALKMSGWIN
    if( fmb->g3Dcamera != NULL ){
      TALKMSGWIN_SYS_SETUP setup;
      setup.heapID = fmb->heapID;
      setup.g3Dcamera = fmb->g3Dcamera;
      setup.fontHandle = fmb->fontHandle;
      setup.chrNumOffs = FLDMSGBG_CHAROFFS_TALKMSGWIN;
      setup.ini.frameID = FLDMSGBG_BGFRAME;
      setup.ini.winPltID = FLDMSGBG_PANO_TALKMSGWIN;
      setup.ini.fontPltID = FLDMSGBG_PANO_FONT_TALKMSGWIN;
      fmb->talkMsgWinSys = TALKMSGWIN_SystemCreate( &setup );
    }
  }

//  FLDMSGBG_SetBlendAlpha();
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
	FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
	
	PRINTSYS_QUE_Main( fmb->printQue );
	
	for( i = 0; i < FLDMSGBG_PRINT_MAX; i++, msgPrint++ ){
		if( msgPrint->printQue != NULL ){
			msgPrint->printTransFlag = PRINT_UTIL_Trans(
				&msgPrint->printUtil, msgPrint->printQue );
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
	msgWin->bmpwin = FldBmpWinFrame_Init( fmb->bgFrame, fmb->heapID,
    bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, pltt_no );
	msgWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, msgWin->bmpwin );
	
  if( pltt_no == FLDMSGBG_PANO_FONT ){
    debug_ROM091030_WindowColorOFF( fmb->heapID );
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
  fmb->deriveWin_plttNo = FLDMSGBG_PANO_FONT;
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
	FldBmpWinFrame_Delete( msgWin->bmpwin );
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
 * @retval	FLDMENUFUNC*
 */
//--------------------------------------------------------------
static FLDMENUFUNC * fldmenufunc_AddMenuList( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos, u16 pltt_no )
{
	FLDMENUFUNC *menuFunc;
	BMPMENULIST_HEADER menuH;
	
	menuFunc = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMENUFUNC) );
	FldMenuFuncH_BmpMenuListH( &menuH, pMenuHead );
	
	menuFunc->fmb = fmb;
	menuFunc->pMenuListData = (BMP_MENULIST_DATA *)pMenuListData;
	
	menuFunc->bmpwin = FldBmpWinFrame_Init(
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
	
	menuFunc->pMenuListWork =
		BmpMenuList_Set( &menuH, list_pos, cursor_pos, fmb->heapID );
//	BmpMenuList_SetCursorString( menuFunc->pMenuListWork, 0 );
	BmpMenuList_SetCursorBmp( menuFunc->pMenuListWork, fmb->heapID );
  
  if( pltt_no == FLDMSGBG_PANO_FONT ){
    debug_ROM091030_WindowColorOFF( fmb->heapID );
    setBlendAlpha( TRUE );
  }
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
	FLDMENUFUNC *menuFunc;
  fmb->deriveWin_plttNo = FLDMSGBG_PANO_FONT;
  menuFunc = fldmenufunc_AddMenuList( fmb, pMenuHead, pMenuListData,
    list_pos, cursor_pos, fmb->deriveWin_plttNo );
  return( menuFunc );
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
 * FLDMENUFUNC	�폜
 * @param	menuFunc	FLDMENUFUNC*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc )
{
	BmpMenuList_Exit( menuFunc->pMenuListWork, NULL, NULL );
	BmpMenuWork_ListDelete( menuFunc->pMenuListData );
	FldBmpWinFrame_Delete( menuFunc->bmpwin );
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
	menuH->rabel_x = fmenuH->rabel_x;		//���x���\���w���W
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
      0, cursor_pos, fmb->deriveWin_plttNo );
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
    return( FLDMENUFUNC_YESNO_NULL );
  }
  
  if( ret == FLDMENUFUNC_CANCEL ){
    return( FLDMENUFUNC_YESNO_NO );
  }
  
  return( ret );
}

//======================================================================
//  FLDMSGPRINT_STREAM ���b�Z�[�W�@�v�����g�X�g���[���֘A
//======================================================================
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
  FLDMSGPRINT_STREAM *stm = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGPRINT_STREAM) );
  
  stm->printStream = PRINTSYS_PrintStream(
      bmpwin, x, y,
      strbuf, fmb->fontHandle,
      wait,
      fmb->printTCBLSys, 0,
      fmb->heapID, 0x0f );

  return( stm );
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
    if( (trg & PAD_BUTTON_A) ){
      stm->flag_key_trg = TRUE;
    }
    
    if( stm->flag_key_trg == TRUE && (cont & PAD_BUTTON_A) ){
      PRINTSYS_PrintStreamShortWait( stm->printStream, 0 );
    }else{
      PRINTSYS_PrintStreamShortWait( stm->printStream, 2 );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( (trg & (PAD_BUTTON_A|PAD_BUTTON_B)) ){
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
	
  fmb->deriveWin_plttNo = FLDMSGBG_PANO_FONT;
  
	msgWin = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDMSGWIN_STREAM) );
	msgWin->fmb = fmb;
  msgWin->msgData = msgData;
	msgWin->bmpwin = FldBmpWinFrame_Init( fmb->bgFrame, fmb->heapID,
      bmppos_x, bmppos_y, bmpsize_x, bmpsize_y, fmb->deriveWin_plttNo );
  msgWin->strBuf = GFL_STR_CreateBuffer( FLDMSGBG_STRLEN, fmb->heapID );
  
  keyCursor_Init( &msgWin->cursor_work, fmb->heapID );

  debug_ROM091030_WindowColorOFF( fmb->heapID );
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
	FldBmpWinFrame_Delete( msgWin->bmpwin );
  
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
  	msgWin->fmb, msgWin->strBuf, msgWin->bmpwin, x, y, 4 );
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
  	msgWin->fmb, strBuf, msgWin->bmpwin, x, y, 2 );
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
      
      if( (trg & PAD_BUTTON_A) ){
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
      WINDOW_TRANS_ON, 1, FLDMSGBG_PANO_MENU );
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
//  FLDTALKMSGWIN
//======================================================================
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
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos, STRBUF *strBuf )
{
  GF_ASSERT( fmb->talkMsgWinSys != NULL );
  
  fmb->deriveWin_plttNo = FLDMSGBG_PANO_FONT_TALKMSGWIN;

  tmsg->talkMsgWinSys = fmb->talkMsgWinSys;
  tmsg->talkMsgWinIdx = idx;
  
  debug_ROM091030_WindowColorON( fmb->heapID );
  setBlendAlpha( FALSE );

  switch( idx ){
  case FLDTALKMSGWIN_IDX_UPPER:
    TALKMSGWIN_CreateFixWindowUpper( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_UPPER, (VecFx32*)pos, strBuf, 15 );
  case FLDTALKMSGWIN_IDX_LOWER:
    TALKMSGWIN_CreateFixWindowLower( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_LOWER, (VecFx32*)pos, strBuf, 15 );
    break;
  default:
    TALKMSGWIN_CreateFixWindowAuto( tmsg->talkMsgWinSys,
        FLDTALKMSGWIN_IDX_AUTO, (VecFx32*)pos, strBuf, 15 );
  }
  
  TALKMSGWIN_OpenWindow( tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
  
  keyCursor_Init( &tmsg->cursor_work, fmb->heapID );
  
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
    const GFL_MSGDATA *msgData, u32 msgID )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  tmsg->strBuf = GFL_STR_CreateBuffer(
					FLDMSGBG_STRLEN, fmb->heapID );
  GFL_MSG_GetString( msgData, msgID, tmsg->strBuf );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, tmsg->strBuf );
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
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos, STRBUF *strBuf )
{
  FLDTALKMSGWIN *tmsg = GFL_HEAP_AllocClearMemory(
      fmb->heapID, sizeof(FLDTALKMSGWIN) );
  fldTalkMsgWin_Add( fmb, tmsg, idx, pos, strBuf );
  return( tmsg );
}

//--------------------------------------------------------------
/**
 * FLDTALKMSGWIN �����o���E�B���h�E�폜
 * @param tmsg FLDTALKMSGWIN
 * @retval nothing
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
  
  switch( state ){
  case PRINTSTREAM_STATE_RUNNING: //���s��
    tmsg->flag_cursor = CURSOR_FLAG_NONE;
    tmsg->flag_key_pause_clear = FALSE;
    
    if( (trg & PAD_BUTTON_A) ){
      tmsg->flag_key_trg = TRUE;
    }
    
    if( tmsg->flag_key_trg == TRUE && (cont & PAD_BUTTON_A) ){
      PRINTSYS_PrintStreamShortWait( stream, 0 );
    }else{
      PRINTSYS_PrintStreamShortWait( stream, 2 );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE: //�ꎞ��~��
    if( tmsg->flag_key_pause_clear == FALSE ){ //���Ƀ|�[�Y�N���A�ς݂��H
      GFL_BMPWIN *twin_bmp = TALKMSGWIN_GetBmpWin(
          tmsg->talkMsgWinSys, tmsg->talkMsgWinIdx );
		  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( twin_bmp );
      
      if( (trg & PAD_BUTTON_A) ){
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
    return( TRUE );
  }
  
  return( FALSE );
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

//======================================================================
//  ���̑�
//======================================================================
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
static GFL_BMPWIN * FldBmpWinFrame_Init( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y, u16 pltt_no )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;

	bmpwin = GFL_BMPWIN_Create( bgFrame,
		pos_x, pos_y, size_x, size_y,
//		FLDMSGBG_PANO_FONT, GFL_BMP_CHRAREA_GET_B );
		pltt_no, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( bmpwin );
		
	GFL_BMP_Clear( bmp, 0xff );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BG_LoadScreenReq( bgFrame );
	
	BmpWinFrame_Write( bmpwin, WINDOW_TRANS_ON, 1, FLDMSGBG_PANO_MENU );
	
	return( bmpwin );
}

//--------------------------------------------------------------
/**
 * �r�b�g�}�b�v�E�B���h�E�t���[���@�폜
 * @param	bmpwin	FldBmpWinFrame_Init()�̖߂�l
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldBmpWinFrame_Delete( GFL_BMPWIN *bmpwin )
{
	BmpWinFrame_Clear( bmpwin, 0 );
	GFL_BMPWIN_Delete( bmpwin );
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
