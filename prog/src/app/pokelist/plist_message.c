//======================================================================
/**
 * @file	plist_message.c
 * @brief	�|�P�������X�g ���b�Z�[�W
 * @author	ariizumi
 * @data	09/06/17
 *
 * ���W���[�����FPLIST_MSG
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "msg/msg_pokelist.h"

#include "gamesystem\msgspeed.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/time_icon.h"
#include "app/app_keycursor.h"
#include "app/app_printsys_common.h"

#include "plist_message.h"
#include "plist_snd_def.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
//�ebmpwin�̃T�C�Y
#define PLIST_MSG_BARWIN_TOP    (21)
#define PLIST_MSG_BARWIN_LEFT   ( 1)
#define PLIST_MSG_BARWIN_WIDTH  (21)
#define PLIST_MSG_BARWIN_HEIGHT ( 2)

#define PLIST_MSG_BARWIN_BATTLE_TOP    (21)
#define PLIST_MSG_BARWIN_BATTLE_LEFT   ( 1)
#define PLIST_MSG_BARWIN_BATTLE_WIDTH  (20)
#define PLIST_MSG_BARWIN_BATTLE_HEIGHT ( 2)

#define PLIST_MSG_MENUWIN_TOP    (19)
#define PLIST_MSG_MENUWIN_LEFT   ( 2)
#define PLIST_MSG_MENUWIN_WIDTH  (14)
#define PLIST_MSG_MENUWIN_HEIGHT ( 4)

#define PLIST_MSG_FULLWIN_TOP    (19)
#define PLIST_MSG_FULLWIN_LEFT   ( 2)
#define PLIST_MSG_FULLWIN_WIDTH  (28)
#define PLIST_MSG_FULLWIN_HEIGHT ( 4)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PLIST_MSG_WORK
{
  PLIST_MSGWIN_TYPE winType;
  
  GFL_BMPWIN *bmpWin;

  //Stream�p(��b���b�Z�[�W
  GFL_TCBLSYS *tcblSys;
  PRINT_STREAM *printHandle;
  STRBUF       *streamStr;
  APP_KEYCURSOR_WORK *cursorWork;
  APP_PRINTSYS_COMMON_WORK printCtrlWork;
  
  BOOL isWaitKey;
  
  //Que�p(�u�ԃ��b�Z�[�W
  BOOL  isUpdateMsg;
  PRINT_QUE *printQueMsg;
  
  WORDSET *wordSet;
  BOOL reqDispTimerIcon;
  TIMEICON_WORK *timeIcon;
};
//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PLIST_MSG_ClearWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork  );
static BOOL PLSIT_MSG_PrintStreamCallBack(u32 value);

//--------------------------------------------------------------
//	���b�Z�[�W�V�X�e��������
//  ���̎��_�ł̃��b�Z�[�W�E�B���h�E�̍쐬�͂��܂���
//--------------------------------------------------------------
PLIST_MSG_WORK* PLIST_MSG_CreateSystem( PLIST_WORK *work )
{
  PLIST_MSG_WORK* msgWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( PLIST_MSG_WORK ) );
  
  BmpWinFrame_GraphicSet( PLIST_BG_MENU , PLIST_BG_WINCHAR_TOP , PLIST_BG_PLT_BMPWIN ,
                          0 , work->heapId );
  
  msgWork->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 1 , 0 );
  msgWork->printHandle = NULL;
  msgWork->winType = PMT_NONE;
  msgWork->isUpdateMsg = FALSE;
  msgWork->isWaitKey = FALSE;
  msgWork->wordSet = NULL;
  msgWork->timeIcon = NULL;
  msgWork->reqDispTimerIcon = FALSE;
  msgWork->cursorWork = APP_KEYCURSOR_Create( 0x0f , FALSE , TRUE , work->heapId );
  msgWork->printQueMsg = PRINTSYS_QUE_Create( work->heapId );
  return msgWork;
}

//--------------------------------------------------------------
//	���b�Z�[�W�V�X�e���J��
//--------------------------------------------------------------
void PLIST_MSG_DeleteSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  if( msgWork->winType != PMT_NONE )
  {
    ARI_TPrintf("PLIST_MSG MessageWindow is open by delete system!!\n");
    PLIST_MSG_CloseWindow( work,msgWork );
  }
  if( msgWork->timeIcon != NULL )
  {
    TIMEICON_Exit( msgWork->timeIcon );
  }
  if( msgWork->printHandle != NULL )
  {
    PRINTSYS_PrintStreamDelete(msgWork->printHandle);
    GFL_STR_DeleteBuffer( msgWork->streamStr );
  }
  PRINTSYS_QUE_Clear( msgWork->printQueMsg );
  PRINTSYS_QUE_Delete( msgWork->printQueMsg );
  APP_KEYCURSOR_Delete( msgWork->cursorWork );
  GFL_TCBL_Exit( msgWork->tcblSys );
  GFL_HEAP_FreeMemory( msgWork );
}

//--------------------------------------------------------------
//	���b�Z�[�W�V�X�e���X�V
//--------------------------------------------------------------
void PLIST_MSG_UpdateSystem( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  //Stream�̍X�V
  GFL_TCBL_Main( msgWork->tcblSys );
  
  if( msgWork->printHandle != NULL )
  {
    BOOL ret;
    APP_KEYCURSOR_Main( msgWork->cursorWork , msgWork->printHandle , msgWork->bmpWin );
    ret = APP_PRINTSYS_COMMON_PrintStreamFunc( &msgWork->printCtrlWork , msgWork->printHandle );
    if( ret == TRUE )
    {
      if( msgWork->isWaitKey == FALSE ||
          GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY ||
          GFL_UI_TP_GetTrg() == TRUE )
      {
        if( msgWork->isWaitKey == TRUE )
        {
          PMSND_PlaySystemSE( APP_PRINTSYS_COMMON_SE_TRG );
        }
        PRINTSYS_PrintStreamDelete( msgWork->printHandle );
        msgWork->printHandle = NULL;
        GFL_STR_DeleteBuffer( msgWork->streamStr );
      }
      else
      {
        APP_KEYCURSOR_Write( msgWork->cursorWork , GFL_BMPWIN_GetBmp( msgWork->bmpWin ) , 0xF );
        GFL_BMPWIN_TransVramCharacter( msgWork->bmpWin );
      }
    }
    /*
    if( PRINTSYS_PrintStreamGetState( msgWork->printHandle ) == PRINTSTREAM_STATE_DONE )
    {
      if( msgWork->isWaitKey == FALSE ||
          GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ||
          GFL_UI_TP_GetTrg() == TRUE )
      {
        PRINTSYS_PrintStreamDelete( msgWork->printHandle );
        msgWork->printHandle = NULL;
        GFL_STR_DeleteBuffer( msgWork->streamStr );
      }
      else
      {
        APP_KEYCURSOR_Write( msgWork->cursorWork , GFL_BMPWIN_GetBmp( msgWork->bmpWin ) , 0xF );
        GFL_BMPWIN_TransVramCharacter( msgWork->bmpWin );
      }
    }
    else
    if( PRINTSYS_PrintStreamGetState( msgWork->printHandle ) == PRINTSTREAM_STATE_PAUSE )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ||
          GFL_UI_TP_GetTrg() == TRUE )
      {
        PRINTSYS_PrintStreamReleasePause( msgWork->printHandle );
      }
    }
    else
    {
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A ||
          GFL_UI_TP_GetCont() == TRUE )
      {
        PRINTSYS_PrintStreamShortWait( msgWork->printHandle , 0 );
      }
    }
    */
  }
  
  //printQue�̍X�V
  if( msgWork->isUpdateMsg == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( msgWork->printQueMsg , GFL_BMPWIN_GetBmp( msgWork->bmpWin )) == FALSE )
    {
      msgWork->isUpdateMsg = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( msgWork->bmpWin );
      if( msgWork->reqDispTimerIcon == TRUE )
      {
        msgWork->reqDispTimerIcon = FALSE;
        msgWork->timeIcon = TIMEICON_CreateTcbl( msgWork->tcblSys , msgWork->bmpWin , 0x0F , TIMEICON_DEFAULT_WAIT , work->heapId );
      }
    }
  }
  PRINTSYS_QUE_Main( msgWork->printQueMsg );

}

//--------------------------------------------------------------
//	���b�Z�[�W �E�B���h�J��
//--------------------------------------------------------------
void PLIST_MSG_OpenWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const PLIST_MSGWIN_TYPE winType)
{
  GF_ASSERT_MSG( msgWork->winType == PMT_NONE , "PLIST_MSG Open window yet!!\n");
  
  msgWork->winType = winType;
  
  switch( msgWork->winType )
  {
  case PMT_BAR:      //�o�[��1�s���b�Z�[�W
    msgWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU , 
              PLIST_MSG_BARWIN_LEFT , PLIST_MSG_BARWIN_TOP , 
              PLIST_MSG_BARWIN_WIDTH , PLIST_MSG_BARWIN_HEIGHT ,
              PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    break;

  case PMT_BAR_BATTLE:      //�o�[��1�s���b�Z�[�W
    msgWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU , 
              PLIST_MSG_BARWIN_BATTLE_LEFT , PLIST_MSG_BARWIN_BATTLE_TOP , 
              PLIST_MSG_BARWIN_BATTLE_WIDTH , PLIST_MSG_BARWIN_BATTLE_HEIGHT ,
              PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    break;

  case PMT_MENU:     //���j���[�̏�����2�s
    msgWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU , 
              PLIST_MSG_MENUWIN_LEFT , PLIST_MSG_MENUWIN_TOP , 
              PLIST_MSG_MENUWIN_WIDTH , PLIST_MSG_MENUWIN_HEIGHT ,
              PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    break;

  case PMT_MESSAGE:  //�t���T�C�Y
    msgWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU , 
              PLIST_MSG_FULLWIN_LEFT , PLIST_MSG_FULLWIN_TOP , 
              PLIST_MSG_FULLWIN_WIDTH , PLIST_MSG_FULLWIN_HEIGHT ,
              PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    break;
  }
  BmpWinFrame_Write( msgWork->bmpWin , WINDOW_TRANS_ON_V , 
                      PLIST_BG_WINCHAR_TOP , PLIST_BG_PLT_BMPWIN );

  PLIST_MSG_ClearWindow( work , msgWork );
}

//--------------------------------------------------------------
//	���b�Z�[�W �E�B���h����
//--------------------------------------------------------------
void PLIST_MSG_CloseWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  if( msgWork->winType == PMT_NONE )
  {
    //�J���ĂȂ����Ƃ����肦��̂ŁE�E�E
    return;
  }
  GF_ASSERT_MSG( msgWork->winType != PMT_NONE , "PLIST_MSG Close window yet!!\n");
  
  if( msgWork->timeIcon != NULL )
  {
    TIMEICON_Exit( msgWork->timeIcon );
    msgWork->timeIcon = NULL;
  }
  
  BmpWinFrame_Clear( msgWork->bmpWin , WINDOW_TRANS_ON_V );
  GFL_BMPWIN_Delete( msgWork->bmpWin );
#ifdef BUGFIX_AF_GFBTS2015_100806
  //���̌�Ɏ��ԍX�V��bmpWin�̂������ʒu������ƁA�]�����Ɏ~�܂�
  PRINTSYS_QUE_Clear( msgWork->printQueMsg );
#endif
  msgWork->winType = PMT_NONE;
}

//--------------------------------------------------------------
//	���b�Z�[�W �E�B���h�E�J�m�F
//--------------------------------------------------------------
const BOOL PLIST_MSG_IsOpenWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  if( msgWork->winType == PMT_NONE )
  {
    return FALSE;
  }

  return TRUE;
}

//--------------------------------------------------------------
//	���b�Z�[�W �`��(�u�ԕ\��
//--------------------------------------------------------------
void PLIST_MSG_DrawMessageNoWait( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const u32 msgId )
{
  STRBUF *str = GFL_MSG_CreateString( work->msgHandle , msgId ); 
  if( msgWork->wordSet != NULL )
  {
    STRBUF *workStr = GFL_STR_CreateBuffer( 128 , work->heapId );
    WORDSET_ExpandStr( msgWork->wordSet , workStr , str );
    GFL_STR_DeleteBuffer( str );
    str = workStr;
  }

  PLIST_MSG_ClearWindow( work , msgWork );
  PRINTSYS_QUE_Clear( msgWork->printQueMsg );

  PRINTSYS_PrintQueColor( msgWork->printQueMsg , GFL_BMPWIN_GetBmp( msgWork->bmpWin ) , 
          PLIST_MSG_STR_OFS_X , PLIST_MSG_STR_OFS_Y , str , work->fontHandle , PLIST_FONT_COLOR_BLACK );

  msgWork->isUpdateMsg = TRUE;
  GFL_STR_DeleteBuffer( str );
  
/*
  PLIST_MSG_ClearWindow( work , msgWork );

  if( msgWork->printHandle != NULL )
  {
    ARI_TPrintf("PLIST_MSG PrintHandle is busy!!\n");
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
    GFL_STR_DeleteBuffer( msgWork->streamStr );
  }
  msgWork->streamStr = GFL_MSG_CreateString( work->msgHandle , msgId ); 
  
  msgWork->printHandle = PRINTSYS_PrintStream( msgWork->bmpWin , 0 , 0 , msgWork->streamStr ,
            work->fontHandle , -64 , msgWork->tcblSys , 0 , work->heapId , PLIST_FONT_MSG_BACK );
*/
}

//--------------------------------------------------------------
//	���b�Z�[�W �`��(�X�g���[��
//--------------------------------------------------------------
void PLIST_MSG_DrawMessageStream( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , const u32 msgId , const BOOL isWait )
{
  STRBUF *str = GFL_MSG_CreateString( work->msgHandle , msgId ); 
  if( msgWork->wordSet != NULL )
  {
    STRBUF *workStr = GFL_STR_CreateBuffer( 128 , work->heapId );
    WORDSET_ExpandStr( msgWork->wordSet , workStr , str );
    GFL_STR_DeleteBuffer( str );
    str = workStr;
  }

  PLIST_MSG_ClearWindow( work , msgWork );
#ifdef BUGFIX_AF_BTS7924_100914
  //Que�������ǉ�
  PRINTSYS_QUE_Clear( msgWork->printQueMsg );
#endif

  if( msgWork->printHandle != NULL )
  {
    ARI_TPrintf("PLIST_MSG PrintHandle is busy!!\n");
    PRINTSYS_PrintStreamDelete( msgWork->printHandle );
    GFL_STR_DeleteBuffer( msgWork->streamStr );
  }
  msgWork->streamStr = str; 
  msgWork->isWaitKey = isWait;

  APP_PRINTSYS_COMMON_PrintStreamInit( &msgWork->printCtrlWork , APP_PRINTSYS_COMMON_TYPE_BOTH );
  msgWork->printHandle = PRINTSYS_PrintStreamCallBack( msgWork->bmpWin , 0 , 0 , msgWork->streamStr ,
            work->fontHandle , MSGSPEED_GetWait() , msgWork->tcblSys , 0 , work->heapId , PLIST_FONT_MSG_BACK , PLSIT_MSG_PrintStreamCallBack );
}

//--------------------------------------------------------------
//	���b�Z�[�W�I���҂�
//--------------------------------------------------------------
const BOOL PLIST_MSG_IsFinishMessage( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  if( msgWork->printHandle == NULL && 
      msgWork->isUpdateMsg == FALSE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//	�X�g���[���p�R�[���o�b�N
//--------------------------------------------------------------
static BOOL PLSIT_MSG_PrintStreamCallBack(u32 value)
{
  switch( value )
  {
  case 2:  // "�e�e�e�e��"Lv�A�b�v�E�Z���o����
    {
      PMSND_PauseBGM( TRUE );
      PMSND_PushBGM();
      PMSND_PlayBGM(PLIST_SND_WAZA_LEARN_ME);
      //PMSND_PlaySystemSE( PLIST_SND_WAZA_LEARN );
    }
    break;
  case 3:  // "�|�J��"
    {
      PMSND_PlaySystemSE( PLIST_SND_WAZA_WASURE );
    }
    break;
  case 5:  // "�e�e�e�e��"��ME�I���҂�
    {
      if( PMSND_CheckPlayBGM() == FALSE )
      {
        PMSND_PopBGM();
        PMSND_PauseBGM( FALSE );
        return FALSE;
      }
      else
      {
        return TRUE;
      }
    }
    break;
  case 6:  // "�|�J��"��SE�I���҂�
    {
      return PMSND_CheckPlaySE();
    }
    break;
  }
  return FALSE;
  
}

#pragma mark [> wordset
void PLIST_MSG_CreateWordSet( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  GF_ASSERT( msgWork->wordSet == NULL );

  msgWork->wordSet = WORDSET_Create( work->heapId );
}

void PLIST_MSG_DeleteWordSet( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_Delete( msgWork->wordSet );
  msgWork->wordSet = NULL;
}

void PLIST_MSG_AddWordSet_PokeName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , POKEMON_PARAM *pp )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterPokeNickName( msgWork->wordSet , wordSetIdx , pp );
}

void PLIST_MSG_AddWordSet_ItemName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 itemIdx )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterItemName( msgWork->wordSet , wordSetIdx , itemIdx );
}

void PLIST_MSG_AddWordSet_SkillName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 skillNo )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterWazaName( msgWork->wordSet , wordSetIdx , skillNo );
}

void PLIST_MSG_AddWordSet_StatusName( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 statusId )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterPokeStatusName( msgWork->wordSet , wordSetIdx , statusId );
}

void PLIST_MSG_AddWordSet_Value( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , u16 value , u8 keta )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterNumber( msgWork->wordSet , wordSetIdx , value , keta , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
}  

void PLIST_MSG_AddWordSet_Word( PLIST_WORK *work , PLIST_MSG_WORK *msgWork , u8 wordSetIdx , STRBUF *str , u32 sex )
{
  GF_ASSERT( msgWork->wordSet != NULL );

  WORDSET_RegisterWord( msgWork->wordSet , wordSetIdx , str , sex , TRUE , PM_LANG );
}  

#pragma mark [> util
//--------------------------------------------------------------
//	�E�B���h�E�N���A
//--------------------------------------------------------------
static void PLIST_MSG_ClearWindow( PLIST_WORK *work , PLIST_MSG_WORK *msgWork  )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( msgWork->bmpWin ) , PLIST_FONT_MSG_BACK );
  
  GFL_BMPWIN_MakeTransWindow_VBlank( msgWork->bmpWin );
}


//--------------------------------------------------------------
//	�E�B���h�E�g�̍ēǂݍ���
//--------------------------------------------------------------
void PLIST_MSG_ReloadWinFrame( PLIST_WORK *work , PLIST_MSG_WORK *msgWork  )
{
  BmpWinFrame_GraphicSet( PLIST_BG_MENU , PLIST_BG_WINCHAR_TOP , PLIST_BG_PLT_BMPWIN ,
                          0 , work->heapId );
}

//--------------------------------------------------------------
//	�^�C�}�[�A�C�R���\��
//--------------------------------------------------------------
void PLIST_MSG_DispTimerIcon( PLIST_WORK *work , PLIST_MSG_WORK *msgWork )
{
  msgWork->reqDispTimerIcon = TRUE;
}

//--------------------------------------------------------------
//	�L�[����N���A
//--------------------------------------------------------------
void PLIST_MSG_ClearKeyCursor( PLIST_WORK *work , PLIST_MSG_WORK *msgWork  )
{
  APP_KEYCURSOR_Clear( msgWork->cursorWork , GFL_BMPWIN_GetBmp( msgWork->bmpWin ) , 0xF );
  GFL_BMPWIN_TransVramCharacter( msgWork->bmpWin );
}

