//======================================================================
/**
 * @file	mb_util_msg.c
 * @brief	�}���`�u�[�g�E���b�Z�[�W�\��
 * @author	ariizumi
 * @data	09/11/16
 *
 * ���W���[�����FMB_MSG
 */
//======================================================================
#include <gflib.h>

#include "arc_def.h"
#include "app/app_keycursor.h"
#include "app/app_printsys_common.h"
#include "print/printsys.h"
#include "system/talkmsgwin.h"
#include "system/time_icon.h"

#include "script_message.naix"
#include "multiboot/mb_util_msg.h"
#include "multiboot/mb_local_def.h"

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
#include "system/bmp_menu.h"
#include "msg/script/msg_common_scr.h"
  //talkmsgwin�p��field��ǂ�
  FS_EXTERN_OVERLAY(fieldmap);
#define MB_TYPE_POKE_SHIFTER
#endif //MULTI_BOOT_MAKE


//======================================================================
//	define
//======================================================================
#pragma mark [> define

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_MSG_WORK
{
  HEAPID heapId;
  u8     frame;
  u8     selFrame;
  MB_MSG_WIN_TYPE type;
  BOOL      enableKey;
  
  //���b�Z�[�W�p
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT        *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
  STRBUF          *msgStr;
  WORDSET         *wordSet;
  APP_PRINTSYS_COMMON_WORK printWork;
  
  PRINT_QUE *printQue;
  BOOL      isUpdateQue;
  BOOL      isUseCursor;
  
  APP_TASKMENU_WORK *yesNoWork;
  APP_TASKMENU_RES  *takmenures;
  APP_KEYCURSOR_WORK *cursorWork;
  
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  BMPWIN_YESNO_DAT menuWork;
  BMPMENU_WORK  *yesNoBmpWin;
#else
  void *yesNoBmpWin;
#endif MULTI_BOOT_MAKE  //�ʏ펞����
  
#ifdef MB_TYPE_POKE_SHIFTER
  TALKMSGWIN_SYS    *talkWinSys;
#endif
  BOOL reqDispTimerIcon;
  TIMEICON_WORK *timeIcon;

};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//  ���b�Z�[�W�n ������
//--------------------------------------------------------------
MB_MSG_WORK* MB_MSG_MessageInit( HEAPID heapId , const u8 frame ,const u8 selFrame , const u32 datId , const BOOL useTalkWin , const BOOL enableKey )
{
  
  MB_MSG_WORK* msgWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MB_MSG_WORK ) );
  msgWork->heapId = heapId;
  msgWork->frame = frame;
  msgWork->selFrame = selFrame;
  msgWork->enableKey = enableKey;
  //���b�Z�[�W�p����
  msgWork->msgWin = NULL;
  
  //�t�H���g�ǂݍ���
  msgWork->fontHandle = GFL_FONT_Create( ARCID_FONT_MB , FILE_FONT_LARGE_MB , GFL_FONT_LOADTYPE_FILE , FALSE , msgWork->heapId );
  
  //���b�Z�[�W
  msgWork->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE_MB , datId , msgWork->heapId );

  BmpWinFrame_GraphicSet( frame , MB_MSG_MSGWIN_CGX , MB_MSG_PLT_MAIN_MSGWIN , 0 , msgWork->heapId );
  if( frame < GFL_BG_FRAME0_S )
  {
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , FILE_FONT_PLT_MB , PALTYPE_MAIN_BG , MB_MSG_PLT_MAIN_FONT*0x20, 16*2, msgWork->heapId );
  }
  else
  {
    GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , FILE_FONT_PLT_MB , PALTYPE_SUB_BG , MB_MSG_PLT_MAIN_FONT*0x20, 16*2, msgWork->heapId );
  }

  GFL_FONTSYS_SetDefaultColor();
  
  msgWork->tcblSys = GFL_TCBL_Init( msgWork->heapId , msgWork->heapId , 3 , 0x100 );
  msgWork->printHandle = NULL;
  msgWork->msgStr = NULL;
  msgWork->wordSet = NULL;
  
  //YesNo�p
  msgWork->printQue = PRINTSYS_QUE_Create( msgWork->heapId );
  msgWork->takmenures  = APP_TASKMENU_RES_Create( msgWork->selFrame, MB_MSG_PLT_MAIN_TASKMENU, msgWork->fontHandle, msgWork->printQue, msgWork->heapId );
  msgWork->yesNoWork = NULL;
  msgWork->yesNoBmpWin = NULL;
  
  msgWork->cursorWork = APP_KEYCURSOR_Create( 0x0f , FALSE , TRUE , msgWork->heapId );
  
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  //talkmsgwin�p��field��ǂ�
  GFL_OVERLAY_Load( FS_OVERLAY_ID( fieldmap ) );
  OS_TPrintf("�f�o�b�O�N������ASSERT�o�܂���SKIP���Ă��������B\n");
  OS_TPrintf("������������������������������������������������\n");
#endif //MULTI_BOOT_MAKE

  //��bWin
#ifdef MB_TYPE_POKE_SHIFTER
  if( useTalkWin == TRUE )
  {
    TALKMSGWIN_SYS_SETUP setup;
    setup.heapID = heapId;
    setup.g3Dcamera = NULL;
    setup.fontHandle = msgWork->fontHandle;
    setup.chrNumOffs = MB_MSG_TALKWIN_CGX;
    setup.ini.frameID = frame;
    setup.ini.winPltID = MB_MSG_PLT_MAIN_TALKWIN;
    setup.ini.fontPltID = MB_MSG_PLT_MAIN_FONT;
    msgWork->talkWinSys = TALKMSGWIN_SystemCreate( &setup );
  }
  else
  {
    msgWork->talkWinSys = NULL;
  }
#endif
  
  msgWork->isUpdateQue = FALSE;
  msgWork->isUseCursor = FALSE;
  msgWork->reqDispTimerIcon = FALSE;
  msgWork->timeIcon = NULL;
  return msgWork;
}


//--------------------------------------------------------------
//  ���b�Z�[�W�n �J��
//--------------------------------------------------------------
void MB_MSG_MessageTerm( MB_MSG_WORK *msgWork )
{
#ifdef MB_TYPE_POKE_SHIFTER
  if( msgWork->talkWinSys != NULL )
  {
    TALKMSGWIN_SystemDelete( msgWork->talkWinSys );
  }
#endif
  if( msgWork->yesNoWork != NULL )
  {
    APP_TASKMENU_CloseMenu( msgWork->yesNoWork );
  }
  
  if( msgWork->timeIcon != NULL )
  {
    TILEICON_Exit( msgWork->timeIcon );
    msgWork->timeIcon = NULL;
  }

  APP_KEYCURSOR_Delete( msgWork->cursorWork );
  APP_TASKMENU_RES_Delete( msgWork->takmenures );
  PRINTSYS_QUE_Delete( msgWork->printQue );
  if( msgWork->printHandle != NULL )
  {
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
  }
  if( msgWork->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( msgWork->msgStr );
  }
  if( msgWork->wordSet != NULL )
  {
    MB_MSG_MessageDeleteWordset( msgWork );
  }
  GFL_MSG_Delete( msgWork->msgHandle );
  if( msgWork->msgWin != NULL )
  {
    GFL_BMPWIN_Delete( msgWork->msgWin );
  }
  GFL_FONT_Delete( msgWork->fontHandle );
  GFL_TCBL_Exit( msgWork->tcblSys );
  
  GFL_HEAP_FreeMemory( msgWork );

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
  //talkmsgwin�p��field��ǂ�
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( fieldmap ) );
#endif //MULTI_BOOT_MAKE
}

//--------------------------------------------------------------
//  ���b�Z�[�W�n �X�V
//--------------------------------------------------------------
void MB_MSG_MessageMain( MB_MSG_WORK *msgWork )
{
  GFL_TCBL_Main( msgWork->tcblSys );
  if( msgWork->printHandle != NULL  )
  {
    const BOOL ret = APP_PRINTSYS_COMMON_PrintStreamFunc( &msgWork->printWork , msgWork->printHandle );
    APP_KEYCURSOR_Main( msgWork->cursorWork , msgWork->printHandle , msgWork->msgWin );
    if( ret == TRUE )
    {
      PRINTSYS_PrintStreamDelete( msgWork->printHandle );
      msgWork->printHandle = NULL;
    }
    /*
    //if( msgWork->isUseCursor == TRUE )
    {
    }
    
    if( PRINTSYS_PrintStreamGetState( msgWork->printHandle ) == PRINTSTREAM_STATE_DONE )
    {
      PRINTSYS_PrintStreamDelete( msgWork->printHandle );
      msgWork->printHandle = NULL;
    }
    else
    if( PRINTSYS_PrintStreamGetState( msgWork->printHandle ) == PRINTSTREAM_STATE_PAUSE )
    {
      if( GFL_UI_TP_GetTrg() == TRUE )
      {
        PRINTSYS_PrintStreamReleasePause( msgWork->printHandle );
      }
    }
    else
    {
      if( GFL_UI_TP_GetCont() == TRUE )
      {
        PRINTSYS_PrintStreamShortWait( msgWork->printHandle , 0 );
      }
    }
    */
  }
  PRINTSYS_QUE_Main( msgWork->printQue );
  if( msgWork->isUpdateQue == TRUE &&
      PRINTSYS_QUE_IsFinished( msgWork->printQue ) == TRUE )
  {
    GFL_BMPWIN_TransVramCharacter( msgWork->msgWin );
    msgWork->isUpdateQue = FALSE;
    if( msgWork->reqDispTimerIcon == TRUE )
    {
      msgWork->reqDispTimerIcon = FALSE;
      msgWork->timeIcon = TIMEICON_CreateTcbl( msgWork->tcblSys , msgWork->msgWin , 0x0F , TIMEICON_DEFAULT_WAIT , msgWork->heapId );
    }
  }

  if( msgWork->yesNoWork != NULL )
  {
    APP_TASKMENU_UpdateMenu( msgWork->yesNoWork );
  }

}

//--------------------------------------------------------------
//  �E�B���h�E�̍쐬(�e��
//--------------------------------------------------------------
void MB_MSG_MessageCreateWindow( MB_MSG_WORK *msgWork , MB_MSG_WIN_TYPE type )
{
  if( msgWork->type != type )
  {
    if( msgWork->msgWin != NULL )
    {
      GFL_BMPWIN_Delete( msgWork->msgWin );
      msgWork->msgWin = NULL;
    }
    //���b�Z�[�W�p����
    switch( type )
    {
    case MMWT_NORMAL:  //��ʒ���
      msgWork->msgWin = GFL_BMPWIN_Create( msgWork->frame , 
                                        MB_MSG_MSGWIN_LEFT , 
                                        MB_MSG_MSGWIN_TOP ,
                                        MB_MSG_MSGWIN_WIDTH , 
                                        MB_MSG_MSGWIN_HEIGHT , 
                                        MB_MSG_PLT_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
      break;
    case MMWT_LARGE:  //��ʒ���
      msgWork->msgWin = GFL_BMPWIN_Create( msgWork->frame , 
                                        MB_MSG_MSGWIN_LEFT , 
                                        MB_MSG_MSGWIN_TOP_L,
                                        MB_MSG_MSGWIN_WIDTH , 
                                        MB_MSG_MSGWIN_HEIGHT_L , 
                                        MB_MSG_PLT_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
      break;
    case MMWT_1LINE:   //��ʉ��P�s
      msgWork->msgWin = GFL_BMPWIN_Create( msgWork->frame , 
                                        MB_MSG_MSGWIN_SEL_LEFT , 
                                        MB_MSG_MSGWIN_SEL_TOP ,
                                        MB_MSG_MSGWIN_SEL_WIDTH , 
                                        MB_MSG_MSGWIN_SEL_HEIGHT , 
                                        MB_MSG_PLT_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
      break;
    case MMWT_2LINE:   //��ʉ��Q�s
    case MMWT_2LINE_TALK:   //��ʉ��Q�s(��bWIN
      msgWork->msgWin = GFL_BMPWIN_Create( msgWork->frame , 
                                        MB_MSG_MSGWIN_SEL_L_LEFT , 
                                        MB_MSG_MSGWIN_SEL_L_TOP ,
                                        MB_MSG_MSGWIN_SEL_L_WIDTH , 
                                        MB_MSG_MSGWIN_SEL_L_HEIGHT , 
                                        MB_MSG_PLT_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
      break;

    case MMWT_2LINE_UP:   //��ʏ�Q�s
      msgWork->msgWin = GFL_BMPWIN_Create( msgWork->frame , 
                                        MB_MSG_MSGWIN_SEL_L_LEFT , 
                                        MB_MSG_MSGWIN_SEL_L_TOP_U ,
                                        MB_MSG_MSGWIN_SEL_L_WIDTH , 
                                        MB_MSG_MSGWIN_SEL_L_HEIGHT , 
                                        MB_MSG_PLT_MAIN_FONT ,
                                        GFL_BMP_CHRAREA_GET_B );
      break;

    }
    msgWork->type = type;
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 0x0 );
  GFL_BMPWIN_TransVramCharacter( msgWork->msgWin );
  GFL_BMPWIN_MakeScreen( msgWork->msgWin );
  GFL_BG_LoadScreenReq(msgWork->frame);
}


//--------------------------------------------------------------------------
//  ���b�Z�[�W�\��
//  �q�@��MsgSpeed�̓Z�[�u������Ȃ��̂ŁA�e��������������ϐ��œn��
//--------------------------------------------------------------------------
void MB_MSG_MessageDisp( MB_MSG_WORK *msgWork , const u16 msgId , const int msgSpeed )
{
  if( msgWork->printHandle != NULL )
  {
    MB_TPrintf( "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
    msgWork->printHandle = NULL;
  }
  
  
  if( msgWork->timeIcon != NULL )
  {
    TILEICON_Exit( msgWork->timeIcon );
    msgWork->timeIcon = NULL;
  }
  //�ꉞ�f�t�H�͏����Ă���
  msgWork->isUseCursor = FALSE;
  msgWork->reqDispTimerIcon = FALSE;
  
  {
    if( msgWork->msgStr != NULL )
    {
      GFL_STR_DeleteBuffer( msgWork->msgStr );
      msgWork->msgStr = NULL;
    }
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 0xf );
    msgWork->msgStr = GFL_MSG_CreateString( msgWork->msgHandle , msgId );
    
    if( msgWork->wordSet != NULL )
    {
      STRBUF *msgWorkStr = GFL_STR_CreateBuffer( 256 , msgWork->heapId );
      WORDSET_ExpandStr( msgWork->wordSet , msgWorkStr , msgWork->msgStr );
      GFL_STR_DeleteBuffer( msgWork->msgStr );
      msgWork->msgStr = msgWorkStr;
    }
    
    msgWork->printHandle = PRINTSYS_PrintStream( msgWork->msgWin , 0,0, msgWork->msgStr ,msgWork->fontHandle ,
                        msgSpeed , msgWork->tcblSys , 2 , msgWork->heapId , 0xf );
  }
  if( msgWork->type == MMWT_2LINE_TALK )
  {
#ifdef MB_TYPE_POKE_SHIFTER
    TALKMSGWIN_WriteBmpWindow( msgWork->talkWinSys , msgWork->msgWin , TALKMSGWIN_TYPE_NORMAL );
#endif
  }
  else
  {
    BmpWinFrame_Write( msgWork->msgWin , WINDOW_TRANS_ON_V , MB_MSG_MSGWIN_CGX , MB_MSG_PLT_MAIN_MSGWIN );
  }
  if( msgWork->enableKey == TRUE )
  {
    APP_PRINTSYS_COMMON_PrintStreamInit( &msgWork->printWork , APP_PRINTSYS_COMMON_TYPE_BOTH );
  }
  else
  {
    APP_PRINTSYS_COMMON_PrintStreamInit( &msgWork->printWork , APP_PRINTSYS_COMMON_TYPE_TOUCH );
  }
}


//--------------------------------------------------------------------------
//  ���b�Z�[�W�\��
//--------------------------------------------------------------------------
void MB_MSG_MessageDispNoWait( MB_MSG_WORK *msgWork , const u16 msgId )
{
  if( msgWork->printHandle != NULL )
  {
    MB_TPrintf( "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
    msgWork->printHandle = NULL;
  }
  
  if( msgWork->timeIcon != NULL )
  {
    TILEICON_Exit( msgWork->timeIcon );
    msgWork->timeIcon = NULL;
  }

  {
    if( msgWork->msgStr != NULL )
    {
      GFL_STR_DeleteBuffer( msgWork->msgStr );
      msgWork->msgStr = NULL;
    }
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 0xf );
    msgWork->msgStr = GFL_MSG_CreateString( msgWork->msgHandle , msgId );
    
    if( msgWork->wordSet != NULL )
    {
      STRBUF *msgWorkStr = GFL_STR_CreateBuffer( 256 , msgWork->heapId );
      WORDSET_ExpandStr( msgWork->wordSet , msgWorkStr , msgWork->msgStr );
      GFL_STR_DeleteBuffer( msgWork->msgStr );
      msgWork->msgStr = msgWorkStr;
    }
    PRINTSYS_PrintQue( msgWork->printQue , GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 
            0 , 0 , msgWork->msgStr , msgWork->fontHandle );
    msgWork->isUpdateQue = TRUE;

  }
  if( msgWork->type == MMWT_2LINE_TALK )
  {
#ifdef MB_TYPE_POKE_SHIFTER
    TALKMSGWIN_WriteBmpWindow( msgWork->talkWinSys , msgWork->msgWin , TALKMSGWIN_TYPE_NORMAL );
#endif
    GFL_BG_LoadScreenReq(msgWork->frame);
    }
  else
  {
    BmpWinFrame_Write( msgWork->msgWin , WINDOW_TRANS_ON_V , MB_MSG_MSGWIN_CGX , MB_MSG_PLT_MAIN_MSGWIN );
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W����
//--------------------------------------------------------------------------
void MB_MSG_MessageHide( MB_MSG_WORK *msgWork )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 0 );
  if( msgWork->type == MMWT_2LINE_TALK )
  {
#ifdef MB_TYPE_POKE_SHIFTER
    TALKMSGWIN_ClearBmpWindow( msgWork->talkWinSys , msgWork->msgWin );
    GFL_BG_LoadScreenReq(msgWork->frame);
#endif
  }
  else
  {
    BmpWinFrame_Clear( msgWork->msgWin , WINDOW_TRANS_ON_V );
  }

  if( msgWork->timeIcon != NULL )
  {
    TILEICON_Exit( msgWork->timeIcon );
    msgWork->timeIcon = NULL;
  }

}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET �쐬
//--------------------------------------------------------------------------
void MB_MSG_MessageCreateWordset( MB_MSG_WORK *msgWork )
{
  if( msgWork->wordSet == NULL )
  {
    msgWork->wordSet = WORDSET_Create( msgWork->heapId );
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET �폜
//--------------------------------------------------------------------------
void MB_MSG_MessageDeleteWordset( MB_MSG_WORK *msgWork )
{
  if( msgWork->wordSet != NULL )
  {
    WORDSET_Delete( msgWork->wordSet );
    msgWork->wordSet = NULL;
  }
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W WORDSET ���O�ݒ�
//--------------------------------------------------------------------------
void MB_MSG_MessageWordsetName( MB_MSG_WORK *msgWork , const u32 bufId , MYSTATUS *myStatus)
{
  WORDSET_RegisterPlayerName( msgWork->wordSet , bufId , myStatus );
}
void MB_MSG_MessageWordsetNumber( MB_MSG_WORK *msgWork , const u32 bufId , const u32 num , const u32 keta )
{
  WORDSET_RegisterNumber( msgWork->wordSet , bufId , num , keta , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
}
void MB_MSG_MessageWordsetNumberZero( MB_MSG_WORK *msgWork , const u32 bufId , const u32 num , const u32 keta )
{
  WORDSET_RegisterNumber( msgWork->wordSet , bufId , num , keta , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
}
void MB_MSG_MessageWordsetStrBuf( MB_MSG_WORK *msgWork , const u32 bufId , STRBUF *strBuf )
{
  //DS�{�̖��p
  WORDSET_RegisterWord( msgWork->wordSet , bufId , strBuf , 0 , TRUE , PM_LANG );
}

//--------------------------------------------------------------------------
//  �I�����\��
//--------------------------------------------------------------------------
void MB_MSG_DispYesNo( MB_MSG_WORK *msgWork , const MB_MSG_YESNO_TYPE type )
{
  APP_TASKMENU_ITEMWORK itemWork[2];
  APP_TASKMENU_INITWORK initWork;
  
  itemWork[0].str = GFL_MSG_CreateString( msgWork->msgHandle , MSGID_YES_MB );
  itemWork[1].str = GFL_MSG_CreateString( msgWork->msgHandle , MSGID_NO_MB );
  itemWork[0].msgColor = MB_MSG_YESNO_COLOR;
  itemWork[1].msgColor = MB_MSG_YESNO_COLOR;
  itemWork[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  itemWork[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  initWork.heapId = msgWork->heapId;
  initWork.itemNum = 2;
  initWork.itemWork = itemWork;
  initWork.posType = ATPT_LEFT_UP;
  switch( type )
  {
  case MMYT_UP:
    initWork.charPosX = MB_MSG_YESNO_X;
    initWork.charPosY = MB_MSG_YESNO_Y_UP;
    break;
  case MMYT_MID:
    initWork.charPosX = MB_MSG_YESNO_SEL_X;
    initWork.charPosY = MB_MSG_YESNO_SEL_Y;
    break;
  case MMYT_DOWN:
    initWork.charPosX = MB_MSG_YESNO_X;
    initWork.charPosY = MB_MSG_YESNO_Y_DOWN;
    break;
  }
  initWork.w = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  initWork.h = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  msgWork->yesNoWork = APP_TASKMENU_OpenMenu( &initWork, msgWork->takmenures );
  
  GFL_STR_DeleteBuffer( itemWork[0].str );
  GFL_STR_DeleteBuffer( itemWork[1].str );
}
//--------------------------------------------------------------------------
//  YesNo����
//--------------------------------------------------------------------------
void MB_MSG_ClearYesNo( MB_MSG_WORK *msgWork )
{
  APP_TASKMENU_CloseMenu( msgWork->yesNoWork );
  msgWork->yesNoWork = NULL;
}

//--------------------------------------------------------------------------
//  YesNo�X�V
//--------------------------------------------------------------------------
const MB_MSG_YESNO_RET MB_MSG_UpdateYesNo( MB_MSG_WORK *msgWork )
{
  if( APP_TASKMENU_IsFinish( msgWork->yesNoWork ) == TRUE )
  {
    return MMYR_RET1 + APP_TASKMENU_GetCursorPos( msgWork->yesNoWork );
  }
  
  return MMYR_NONE;
}

#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
//--------------------------------------------------------------------------
//  �I�����\��(���ʗp
//--------------------------------------------------------------------------
#define	FLD_YESNO_WIN_PX	( 24 )
#define	FLD_YESNO_WIN_PY	( 13 )
void MB_MSG_DispYesNoUpper( MB_MSG_WORK *msgWork , const MB_MSG_YESNO_TYPE type )
{
  msgWork->menuWork.frmnum = msgWork->selFrame;
  msgWork->menuWork.pos_x = 24;
  msgWork->menuWork.pos_y = 13;
  msgWork->menuWork.palnum = MB_MSG_PLT_MAIN_FONT;
  msgWork->menuWork.chrnum = 0;  //���g�p(FixPos�̂�)
  msgWork->yesNoBmpWin = BmpMenu_YesNoSelectInit( &msgWork->menuWork , MB_MSG_MSGWIN_CGX , MB_MSG_PLT_MAIN_MSGWIN , 0 , msgWork->heapId );
}
//--------------------------------------------------------------------------
//  YesNo����
//--------------------------------------------------------------------------
void MB_MSG_ClearYesNoUpper( MB_MSG_WORK *msgWork )
{
  //�����R�����g�Ȃ̂́ABmpMenu_YesNoSelectMain�̓����ŁA���肵���u�Ԃ�
  //EXIT����Ă���̂�
  //BmpMenu_YesNoMenuExit( msgWork->yesNoBmpWin );
  msgWork->yesNoBmpWin = NULL;
}

//--------------------------------------------------------------------------
//  YesNo�X�V
//--------------------------------------------------------------------------
const MB_MSG_YESNO_RET MB_MSG_UpdateYesNoUpper( MB_MSG_WORK *msgWork )
{
  const u32 ret = BmpMenu_YesNoSelectMain( msgWork->yesNoBmpWin );
  if( ret == 0 )
  {
    return MMYR_RET1;
  }
  else
  if( ret == BMPMENU_CANCEL )
  {
    return MMYR_RET2;
  }
  return MMYR_NONE;
}

void MB_MSG_MessageDips_CommDisableError( MB_MSG_WORK *msgWork , const int msgSpeed )
{
  GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_SCRIPT_MESSAGE , NARC_script_message_common_scr_dat , msgWork->heapId );

  if( msgWork->printHandle != NULL )
  {
    MB_TPrintf( "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
    msgWork->printHandle = NULL;
  }

  //�ꉞ�f�t�H�͏����Ă���
  msgWork->isUseCursor = FALSE;
  msgWork->reqDispTimerIcon = FALSE;

  if( msgWork->msgStr != NULL )
  {
    GFL_STR_DeleteBuffer( msgWork->msgStr );
    msgWork->msgStr = NULL;
  }

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->msgWin ) , 0xf );
  msgWork->msgStr = GFL_MSG_CreateString( msgHandle , msg_common_wireless_off_keywait );
  
  msgWork->printHandle = PRINTSYS_PrintStream( msgWork->msgWin , 0,0, msgWork->msgStr ,msgWork->fontHandle ,
                      msgSpeed , msgWork->tcblSys , 2 , msgWork->heapId , 0xf );

  BmpWinFrame_Write( msgWork->msgWin , WINDOW_TRANS_ON_V , MB_MSG_MSGWIN_CGX , MB_MSG_PLT_MAIN_MSGWIN );
  
  GFL_MSG_Delete( msgHandle );
}

#endif //MULTI_BOOT_MAKE  //�ʏ펞����

//--------------------------------------------------------------------------
//  MSGDATA�擾
//--------------------------------------------------------------------------
GFL_MSGDATA* MB_MSG_GetMsgHandle( MB_MSG_WORK *msgWork )
{
  return msgWork->msgHandle;
}

//--------------------------------------------------------------------------
//  WORDSET�擾
//--------------------------------------------------------------------------
WORDSET* MB_MSG_GetWordSet( MB_MSG_WORK *msgWork )
{
  return msgWork->wordSet;
}

//--------------------------------------------------------------------------
//  GFL_FONT�擾
//--------------------------------------------------------------------------
GFL_FONT* MB_MSG_GetFont( MB_MSG_WORK *msgWork )
{
  return msgWork->fontHandle;
}
//--------------------------------------------------------------------------
//  PRINT_QUE�擾
//--------------------------------------------------------------------------
PRINT_QUE* MB_MSG_GetPrintQue( MB_MSG_WORK *msgWork )
{
  return msgWork->printQue;
}
//--------------------------------------------------------------------------
//  �I���`�F�b�N
//--------------------------------------------------------------------------
const BOOL MB_MSG_CheckPrintQueIsFinish( MB_MSG_WORK *msgWork )
{
  return PRINTSYS_QUE_IsFinished( msgWork->printQue );
}
const BOOL MB_MSG_CheckPrintStreamIsFinish( MB_MSG_WORK *msgWork )
{
  if( msgWork->printHandle == NULL )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  ���b�Z�[�W�҂��J�[�\���̕\��
//--------------------------------------------------------------------------
void MB_MSG_SetDispKeyCursor( MB_MSG_WORK *msgWork , const BOOL flg )
{
  msgWork->isUseCursor = flg;
  GFL_BMPWIN_MakeScreen( msgWork->msgWin );
  GFL_BG_LoadScreenReq(msgWork->frame);
}

//--------------------------------------------------------------------------
//  ���v�A�C�R���̕\��
//--------------------------------------------------------------------------
void MB_MSG_SetDispTimeIcon( MB_MSG_WORK *msgWork , const BOOL flg )
{
  msgWork->reqDispTimerIcon = flg;
}

