//======================================================================
/**
 * @file  bmp_taskmenu.c
 * @brief �^�X�N�o�[�p ���ʃ��j���[
 * @author  ariizumi toru_nagihashi
 * @data  09/07/27
 *
 * ���W���[�����FBMP_TASKMENU
 *
 *  taskmenu�������O�ł����A
 *  task�V�X�e���͎g���Ă���܂���
 *
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "app/app_menu_common.h"
#include "print/wordset.h"

#include "arc_def.h"

//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE
//�ʏ펞����
#include "app_menu_common.naix"
#else
//DL�q�@������
#include "multiboot/wb_sound_palpark.sadl"
#include "app_menu_common_dl.naix"
#endif //MULTI_BOOT_MAKE
#include "sound/pm_sndsys.h"

#include "app/app_taskmenu.h"


//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define APP_TASKMENU_PLATE_LEFT (19)

#define APP_TASKMENU_PLATE_NCG_NUM  (3*8)
#define APP_TASKMENU_PLATE_NCG_LEFTTOP  (0)
#define APP_TASKMENU_PLATE_NCG_TOP      (1)
#define APP_TASKMENU_PLATE_NCG_RIGHTTOP (2)
#define APP_TASKMENU_PLATE_NCG_LEFT     (3)
#define APP_TASKMENU_PLATE_NCG_CENTER   (4)
#define APP_TASKMENU_PLATE_NCG_RIGHT    (5)
#define APP_TASKMENU_PLATE_NCG_LEFTDOWN (6)
#define APP_TASKMENU_PLATE_NCG_DOWN     (7)
#define APP_TASKMENU_PLATE_NCG_RIGHTDOWN  (8)
#define APP_TASKMENU_PLATE_NCG_MARK_RET   (9)
#define APP_TASKMENU_PLATE_NCG_WIDTH    (3)

//�v���[�g�̃A�j��
#define APP_TASKMENU_ANIME_S_R (5)
#define APP_TASKMENU_ANIME_S_G (10)
#define APP_TASKMENU_ANIME_S_B (13)
#define APP_TASKMENU_ANIME_E_R (12)
#define APP_TASKMENU_ANIME_E_G (25)
#define APP_TASKMENU_ANIME_E_B (30)
//�v���[�g�̃A�j������F
#define APP_TASKMENU_ANIME_COL (0x6)

#define APP_TASKMENU_SND_CURSOR (SEQ_SE_SELECT1)
#define APP_TASKMENU_SND_DECIDE (SEQ_SE_DECIDE1)
#define APP_TASKMENU_SND_CANCEL (SEQ_SE_CANCEL1)
#define APP_TASKMENU_SND_OPENMENU (SEQ_SE_OPEN1)
#define APP_TASKMENU_SND_CLOSEMENU (SEQ_SE_CLOSE1)

#define APP_TASKMENU_SND_CHANGE (SEQ_SE_OPEN1)


//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
//���\�[�X�p���[�N
struct _APP_TASKMENU_RES
{ 
  //���\�[�X�֌W
  u16 frame;
  u16 plt;
  NNSG2dCharacterData *ncg_data;
  void *ncg_buf; 

  //���b�Z�[�W�֌W
  GFL_MSGDATA *msgHandle;
  GFL_FONT    *fontHandle;
  PRINT_QUE   *printQue;
};

//�c���j���[�p���[�N
struct _APP_TASKMENU_WORK
{
  u8 cursorPos;
  u8 anmCnt;
  u16 transAnmCnt;
  GXRgb transCol;
  BOOL isUpdateMsg;
  BOOL isDecide;
  BOOL disableKey;  //�L�[���얳��
  u32 anmDouble;

  GFL_BMPWIN **menuWin;
  APP_TASKMENU_ITEMWORK *itemWork;
  //���j���[�y��
  
  APP_TASKMENU_INITWORK initWork;

  const APP_TASKMENU_RES *res;
};

//�����j���[�p���[�N
struct _APP_TASKMENU_WIN_WORK
{
  GFL_BMPWIN *bmpwin;
  BOOL isActive;
  BOOL isDecide;
  BOOL isUpdateMsg;

  u16 anmCnt;
  u16 anmDouble;
  u16 transAnmCnt;
  GXRgb transCol;
  const APP_TASKMENU_RES *res;
 
  // �v���[�g�̃A�j���̐F
  GXRgb animeColS;  // �J�n�F
  GXRgb animeColE;  // �I���F
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static void APP_TASKMENU_CreateMenuWin( APP_TASKMENU_WORK *work, const APP_TASKMENU_RES *res );
static void APP_TASKMENU_TransFrame( GFL_BMPWIN *bmpwin, const APP_TASKMENU_RES *res, APP_TASKMENU_WIN_TYPE type );
static void APP_TASKMENU_SetActiveItem( GFL_BMPWIN *bmpwin, const APP_TASKMENU_RES *res, const BOOL isActive );
static void APP_TASKMENU_UpdatePalletAnime( u16 *anmCnt , u16 *transBuf , u8 bgFrame , u8 pltNo );
static void APP_TASKMENU_UpdatePalletAnimeEx( u16 *anmCnt , u16 *transBuf , u8 bgFrame , u8 pltNo, GXRgb colS, GXRgb colE );
static void APP_TASKMENU_ResetPallet( u16 *transBuf, u8 bgFrame , u8 pltNo );
static void APP_TASKMENU_UpdateKey( APP_TASKMENU_WORK *work );
static void APP_TASKMENU_UpdateTP( APP_TASKMENU_WORK *work );

static APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_CreateExCore( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, u8 h, BOOL anmDouble, const BOOL isCenter , HEAPID heapID );
static void APP_TASKMENU_WIN_DeleteCore( APP_TASKMENU_WIN_WORK *wk );


//--------------------------------------------------------------
//  ���j���[�J��
//--------------------------------------------------------------
APP_TASKMENU_WORK* APP_TASKMENU_OpenMenu( APP_TASKMENU_INITWORK *initWork, const APP_TASKMENU_RES *res )
{
  APP_TASKMENU_WORK *work = GFL_HEAP_AllocMemory( initWork->heapId , sizeof( APP_TASKMENU_WORK ) );
  PALTYPE palType;

  work->initWork  = *initWork;
  work->res       = res;

  work->menuWin = GFL_HEAP_AllocMemory( work->initWork.heapId , sizeof( GFL_BMPWIN* ) * work->initWork.itemNum );
  work->itemWork = GFL_HEAP_AllocMemory( work->initWork.heapId , sizeof( APP_TASKMENU_ITEMWORK ) * work->initWork.itemNum );
  GFL_STD_MemCopy16( work->initWork.itemWork , work->itemWork , sizeof(APP_TASKMENU_ITEMWORK) * work->initWork.itemNum );

  work->isDecide = FALSE;
  work->disableKey = FALSE;
  work->cursorPos = 0;
  work->anmCnt = 0;
  work->transAnmCnt = 0;

  work->anmDouble = 1;

  { 
    work->initWork.w  = initWork->w == 0 ?  APP_TASKMENU_PLATE_WIDTH:  initWork->w;
    work->initWork.h  = initWork->h == 0 ?  APP_TASKMENU_PLATE_HEIGHT: initWork->h;
    APP_TASKMENU_CreateMenuWin( work, res );
  }

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
  {
    APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, TRUE );
  }
  
  work->isUpdateMsg = TRUE;
  
  return work;
}

// 1/30f�p�ɃA�j�����{���ɂȂ�
APP_TASKMENU_WORK* APP_TASKMENU_OpenMenuEx( APP_TASKMENU_INITWORK *initWork, const APP_TASKMENU_RES *res )
{
  APP_TASKMENU_WORK * work = APP_TASKMENU_OpenMenu( initWork, res );
  work->anmDouble = 2;
  return work;
}

//--------------------------------------------------------------
//  ���j���[����
//--------------------------------------------------------------
void APP_TASKMENU_CloseMenu( APP_TASKMENU_WORK *work )
{
  u8 i;
  u8 frm;

  frm = GFL_BMPWIN_GetFrame(work->menuWin[0]);

  //�L���[���c�����܂�BMPWIN���N���A���Ă��܂��̂�h������
  //�������c���Ă����ꍇ�͏����������߂�
  for(i=0;i<work->initWork.itemNum;i++)
  {
    while( PRINTSYS_QUE_IsExistTarget( work->res->printQue, GFL_BMPWIN_GetBmp(work->menuWin[i] ) ) )
    {
      PRINTSYS_QUE_Main( work->res->printQue );
    }
  }

  for(i=0;i<work->initWork.itemNum;i++)
  {
    GFL_BMPWIN_ClearScreen( work->menuWin[i] );
    GFL_BMPWIN_Delete( work->menuWin[i] );
  }
  GFL_BG_LoadScreenV_Req( frm );
  
  GFL_HEAP_FreeMemory( work->menuWin );
  GFL_HEAP_FreeMemory( work->itemWork );
  GFL_HEAP_FreeMemory( work );
}

//=============================================================================================
/**
 * @brief �����`��̂݃��N�G�X�g������ꍇ�͍s��
 *
 * @param   work    
 */
//=============================================================================================
void APP_TASKMENU_ShowOnly( APP_TASKMENU_WORK *work )
{
  u8 i;
  //�����̍X�V
  if( work->isUpdateMsg == TRUE )
  {
    BOOL isFinish = TRUE;
    for(i=0;i<work->initWork.itemNum;i++)
    {
      if( PRINTSYS_QUE_IsExistTarget( work->res->printQue , GFL_BMPWIN_GetBmp( work->menuWin[i] ) ) == TRUE )
      {
        isFinish = FALSE;
      }
    }
    if( isFinish == TRUE )
    {
      for(i=0;i<work->initWork.itemNum;i++)
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->menuWin[i] );
      }
      work->isUpdateMsg = FALSE;
    }
  }
  
}

//--------------------------------------------------------------
//  ���j���[�X�V
//--------------------------------------------------------------
void APP_TASKMENU_UpdateMenu( APP_TASKMENU_WORK *work )
{
  u8 i;
  //�����̍X�V�E�\������
  APP_TASKMENU_ShowOnly( work );
  
  if( work->isDecide == FALSE )
  {
    APP_TASKMENU_UpdateKey( work );
    if( work->isDecide == FALSE )
    {
      APP_TASKMENU_UpdateTP( work );
    }
  }
  else
  {
    //���莞�A�j��
    const u8 isBlink = (work->anmCnt/(APP_TASKMENU_ANM_INTERVAL/work->anmDouble))%2;
    if( isBlink == 0 )
    {
      APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, TRUE );
    }
    else
    {
      APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, FALSE );
    }
    work->anmCnt++;
  }
  
  APP_TASKMENU_UpdatePalletAnime( &work->transAnmCnt , &work->transCol , work->res->frame , work->res->plt );
}

//--------------------------------------------------------------
//  �I���`�F�b�N
//--------------------------------------------------------------
const BOOL APP_TASKMENU_IsFinish( APP_TASKMENU_WORK *work )
{
  if( work->anmCnt < (APP_TASKMENU_ANM_CNT/work->anmDouble) )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}

//--------------------------------------------------------------
//  �J�[�\���ʒu�̎擾
//--------------------------------------------------------------
const u8 APP_TASKMENU_GetCursorPos( APP_TASKMENU_WORK *work )
{
  return work->cursorPos;
}

//--------------------------------------------------------------
//  �A�N�e�B�u�̐ݒ�
//--------------------------------------------------------------
const void APP_TASKMENU_SetActive( APP_TASKMENU_WORK *work, BOOL isActive )
{ 
  APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, isActive );
}

//=============================================================================================
/**
 * @brief �^�X�N���j���[�̌����Ԃ��擾
 *
 * @param   work
 * @return  BOOL       TRUE :����ς݁@FALSE �F����܂�
 */
//=============================================================================================
const BOOL APP_TASKMENU_IsDecide( APP_TASKMENU_WORK *work )
{
  return work->isDecide;
}

//--------------------------------------------------------------
//  ���j���[��BmpWin�ƕ��������
//--------------------------------------------------------------
static void APP_TASKMENU_CreateMenuWin( APP_TASKMENU_WORK *work, const APP_TASKMENU_RES *res )
{
  u8 i;
  
  u8 menuTop;
  u8 menuLeft;
  
  if( work->initWork.posType == ATPT_LEFT_UP )
  {
    menuLeft = work->initWork.charPosX;
    menuTop = work->initWork.charPosY;
  }
  else
  {
    menuLeft = work->initWork.charPosX - work->initWork.w;
    menuTop = work->initWork.charPosY - (work->initWork.h*work->initWork.itemNum);
  }
  
  for(i=0;i<work->initWork.itemNum;i++)
  {
    u32 charAdr;
    work->menuWin[i] = GFL_BMPWIN_Create( work->res->frame ,
                        menuLeft , menuTop+(i*work->initWork.h) , 
                        work->initWork.w, work->initWork.h, 
                        work->res->plt+1 , GFL_BMP_CHRAREA_GET_B );

    //�v���[�g�̊G�𑗂�
    APP_TASKMENU_TransFrame( work->menuWin[i], res, work->itemWork[i].type );
    PRINTSYS_PrintQueColor( res->printQue , GFL_BMPWIN_GetBmp( work->menuWin[i] ), 
                        8+2 , 4+2 , work->itemWork[i].str , res->fontHandle , work->itemWork[i].msgColor );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->menuWin[i] );
  }
}

//--------------------------------------------------------------
//  ���ڂ̃A�N�e�B�u�E��A�N�e�B�u�؂�ւ�
//--------------------------------------------------------------
static void APP_TASKMENU_SetActiveItem( GFL_BMPWIN *bmpwin, const APP_TASKMENU_RES *res, const BOOL isActive )
{
  if( isActive == TRUE )
  {
    GFL_BMPWIN_SetPalette( bmpwin , res->plt );
  }
  else if( isActive == FALSE )
  {
    GFL_BMPWIN_SetPalette( bmpwin , res->plt+1 );
  }
  GFL_BMPWIN_MakeScreen( bmpwin );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(bmpwin) );
}

//--------------------------------------------------------------
//  �p���b�g�A�j���[�V�����̍X�V
//--------------------------------------------------------------
static void APP_TASKMENU_UpdatePalletAnime( u16 *anmCnt , u16 *transBuf , u8 bgFrame , u8 pltNo )
{
  //�v���[�g�A�j��
  if( *anmCnt + APP_TASKMENU_ANIME_VALUE >= 0x10000 )
  {
    *anmCnt = *anmCnt+APP_TASKMENU_ANIME_VALUE-0x10000;
  }
  else
  {
    *anmCnt += APP_TASKMENU_ANIME_VALUE;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*anmCnt)+FX16_ONE)/2;
    const u8 r = APP_TASKMENU_ANIME_S_R + (((APP_TASKMENU_ANIME_E_R-APP_TASKMENU_ANIME_S_R)*cos)>>FX16_SHIFT);
    const u8 g = APP_TASKMENU_ANIME_S_G + (((APP_TASKMENU_ANIME_E_G-APP_TASKMENU_ANIME_S_G)*cos)>>FX16_SHIFT);
    const u8 b = APP_TASKMENU_ANIME_S_B + (((APP_TASKMENU_ANIME_E_B-APP_TASKMENU_ANIME_S_B)*cos)>>FX16_SHIFT);
    
    *transBuf = GX_RGB(r, g, b);
    
    if( bgFrame <= GFL_BG_FRAME3_M )
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
    else
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
  }
}

//--------------------------------------------------------------
//  �p���b�g�A�j���[�V�����̍X�V(�J�n�F�A�I���F�w���)
//--------------------------------------------------------------
static void APP_TASKMENU_UpdatePalletAnimeEx( u16 *anmCnt , u16 *transBuf , u8 bgFrame , u8 pltNo, GXRgb colS, GXRgb colE )
{
  u8 s_r = ( colS & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  u8 s_g = ( colS & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  u8 s_b = ( colS & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
  u8 e_r = ( colE & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  u8 e_g = ( colE & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  u8 e_b = ( colE & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

  //�v���[�g�A�j��
  if( *anmCnt + APP_TASKMENU_ANIME_VALUE >= 0x10000 )
  {
    *anmCnt = *anmCnt+APP_TASKMENU_ANIME_VALUE-0x10000;
  }
  else
  {
    *anmCnt += APP_TASKMENU_ANIME_VALUE;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(*anmCnt)+FX16_ONE)/2;
    const u8 r = s_r + (((e_r-s_r)*cos)>>FX16_SHIFT);
    const u8 g = s_g + (((e_g-s_g)*cos)>>FX16_SHIFT);
    const u8 b = s_b + (((e_b-s_b)*cos)>>FX16_SHIFT);
    
    *transBuf = GX_RGB(r, g, b);
    
    if( bgFrame <= GFL_BG_FRAME3_M )
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
    else
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �p���b�g�A�j����������A���\�[�X���g��������ۂ́A�p���b�g������
 *
 *  @param  u8 bgFrame  BG��
 *  @param  pltNo       �p���b�gNO
 */
//-----------------------------------------------------------------------------
static void APP_TASKMENU_ResetPallet( u16 *transBuf, u8 bgFrame , u8 pltNo )
{ 
    if( bgFrame <= GFL_BG_FRAME3_M )
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
    else
    {
      NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                          pltNo * 32 + APP_TASKMENU_ANIME_COL*2 ,
                                          transBuf , 2 );
    }
}

void APP_TASKMENU_SetDisableKey( APP_TASKMENU_WORK *work , const BOOL flg )
{
  work->disableKey = flg;

  //�����L�[�𕷂��Ȃ�����ꍇ�͋����^�b�`���[�h
  if( flg == TRUE )
  { 
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, FALSE );
  }
}

void APP_TASKMENU_SetCursorPos( APP_TASKMENU_WORK *work, int pos )
{
  work->cursorPos = pos;

  //�L�[���[�h
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_KEY )
  {
    int i;
    for( i = 0; i < work->initWork.itemNum; i++ )
    {
      if( i == pos )
      {
        APP_TASKMENU_SetActiveItem( work->menuWin[i], work->res, TRUE );
      }
      else
      {
        APP_TASKMENU_SetActiveItem( work->menuWin[i], work->res, FALSE );
      }
    }
  }
}

#pragma mark [>main func
//--------------------------------------------------------------
//  �L�[����X�V
//--------------------------------------------------------------
static void APP_TASKMENU_UpdateKey( APP_TASKMENU_WORK *work )
{
  const int trg = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();
  
  if( work->disableKey == TRUE )
  {
    return;
  }

  //B�͒��ڑI�΂��i�L�[���[�h�ւ̕ϊ��͉��L�Łj
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH && !(trg&PAD_BUTTON_B)   )
  {
    //���E���g���ӏ�������̂�
    if( trg & (PAD_KEY_UP|PAD_KEY_DOWN|PAD_BUTTON_A|PAD_BUTTON_B) )
    {
      APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, TRUE );
      GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
      PMSND_PlaySE( SEQ_SE_SELECT1 );
    }
  }
  else
  {
    const u8 befPos = work->cursorPos;
    if( repeat & PAD_KEY_UP )
    {
      if( work->cursorPos == 0 )
      {
        work->cursorPos = work->initWork.itemNum-1;
      }
      else
      {
        work->cursorPos--;
      }
      work->transAnmCnt = 0;
      PMSND_PlaySystemSE( APP_TASKMENU_SND_CURSOR );
    }
    else
    if( repeat & PAD_KEY_DOWN )
    {
      if( work->cursorPos >= work->initWork.itemNum-1 )
      {
        work->cursorPos = 0;
      }
      else
      {
        work->cursorPos++;
      }
      work->transAnmCnt = 0;
      PMSND_PlaySystemSE( APP_TASKMENU_SND_CURSOR );
    }
    else
    if( trg & PAD_BUTTON_A )
    {
      work->isDecide = TRUE;
      
      if( work->itemWork[ work->cursorPos ].type == APP_TASKMENU_WIN_TYPE_RETURN )
      { 
        PMSND_PlaySystemSE( APP_TASKMENU_SND_CANCEL );
      }
      else
      { 
        PMSND_PlaySystemSE( APP_TASKMENU_SND_DECIDE );
      }
    }
    else
    if( trg & PAD_BUTTON_B )
    {
      work->cursorPos = work->initWork.itemNum-1;
      work->isDecide = TRUE;
      PMSND_PlaySystemSE( APP_TASKMENU_SND_CANCEL );
      GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
    }
    
    if( befPos != work->cursorPos )
    {
      APP_TASKMENU_SetActiveItem( work->menuWin[befPos], work->res, FALSE );
      APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, TRUE );
    }
  }
}

static void APP_TASKMENU_UpdateTP( APP_TASKMENU_WORK *work )
{
  u8 i;
  int ret;
  GFL_UI_TP_HITTBL hitTbl[9];

  u8 menuTop;
  u8 menuLeft;
  if( work->initWork.posType == ATPT_LEFT_UP )
  {
    menuLeft = work->initWork.charPosX;
    menuTop = work->initWork.charPosY;
  }
  else
  {
    menuLeft = work->initWork.charPosX - work->initWork.w;
    menuTop = work->initWork.charPosY - (work->initWork.h*work->initWork.itemNum);
  }
  
  //�e�[�u���̍쐬
  for( i=0 ; i<work->initWork.itemNum ; i++ )
  {
    hitTbl[i].rect.top    = menuTop*8 + (work->initWork.h*8*i);
    hitTbl[i].rect.bottom = menuTop*8 + (work->initWork.h*8*(i+1));
    hitTbl[i].rect.left   = menuLeft*8;
    hitTbl[i].rect.right  = (menuLeft+work->initWork.w)*8 - 1;
  }
  hitTbl[i].circle.code = GFL_UI_TP_HIT_END;
  
  ret = GFL_UI_TP_HitTrg( hitTbl );
  
  if( ret != GFL_UI_TP_HIT_NONE )
  {
    GFL_UI_SetTouchOrKey( GFL_APP_KTST_TOUCH );
    APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, FALSE );
    work->cursorPos = ret;
    work->isDecide = TRUE;
    APP_TASKMENU_SetActiveItem( work->menuWin[work->cursorPos], work->res, TRUE );
    if( work->cursorPos == 0 )
    {
      PMSND_PlaySystemSE( APP_TASKMENU_SND_DECIDE );
    }
    else
    {
      if( work->itemWork[ work->cursorPos ].type == APP_TASKMENU_WIN_TYPE_RETURN )
      { 
        PMSND_PlaySystemSE( APP_TASKMENU_SND_CANCEL );
      }
      else
      { 
        PMSND_PlaySystemSE( APP_TASKMENU_SND_DECIDE );
      }
    }
  }
}

//=============================================================================
/**
 *          RES
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  APPTASK���j���[�p���\�[�X�ǂݍ���
 *
 *  @param  u8 frame  �t���[��
 *  @param  plt       �p���b�g�ԍ�
 *  @param  heapID    �q�[�vID
 *
 *  @return         ���\�[�X�ێ����[�N
 */
//-----------------------------------------------------------------------------
APP_TASKMENU_RES* APP_TASKMENU_RES_Create( u8 frame, u8 plt, GFL_FONT *fontHandle, PRINT_QUE *printQue, HEAPID heapID )
{ 
  APP_TASKMENU_RES *wk;
  PALTYPE palType;

  //���[�N�쐬
  wk  = GFL_HEAP_AllocMemory( heapID ,sizeof(APP_TASKMENU_RES) );
  GFL_STD_MemClear( wk, sizeof(APP_TASKMENU_RES) );
  wk->frame = frame;
  wk->plt   = plt;
  wk->fontHandle  = fontHandle;
  wk->printQue    = printQue;

  //�㉺���
  if( frame <= GFL_BG_FRAME3_M )
  {
    palType = PALTYPE_MAIN_BG;
  }
  else
  {
    palType = PALTYPE_SUB_BG;
  }

  //�ǂݍ���
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE
//�ʏ펞����
  wk->ncg_buf = GFL_ARC_UTIL_LoadBGCharacter( APP_COMMON_GetArcId() ,
      NARC_app_menu_common_task_menu_NCGR, FALSE, &wk->ncg_data, heapID );  
  GFL_ARC_UTIL_TransVramPalette( APP_COMMON_GetArcId() , 
      NARC_app_menu_common_task_menu_NCLR , palType , plt*32 , 32*2 , heapID ); 
#else
//DL�q�@������
  wk->ncg_buf = GFL_ARC_UTIL_LoadBGCharacter( APP_COMMON_GetArcId() ,
      NARC_app_menu_common_dl_task_menu_NCGR, FALSE, &wk->ncg_data, heapID ); 
  GFL_ARC_UTIL_TransVramPalette( APP_COMMON_GetArcId() , 
      NARC_app_menu_common_dl_task_menu_NCLR , palType , plt*32 , 32*2 , heapID );  
#endif //MULTI_BOOT_MAKE

  return wk;
}

//----------------------------------------------------------------------------
/**
 *  @brief  APPTASKMENU�p���\�[�X�ǂݍ���
 *
 *  @param  APP_TASKMENU_RES *wk ���[�N
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_RES_Delete( APP_TASKMENU_RES *wk )
{ 
  GFL_HEAP_FreeMemory( wk->ncg_buf );
  GFL_HEAP_FreeMemory( wk );
}


//=============================================================================
/**
 *            WIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  �P�����쐬
 *
 *  @param  const APP_TASKMENU_WIN_WORK *res  ���̃��\�[�X
 *  @param  APP_TASKMENU_ITEMWORK *item �����
 *  @param  x     X���W�i�L�����P�ʁj
 *  @param  y     Y���W�i�L�����P�ʁj
 *  @param  w     ���i�L�����P�ʁj
 *
 *  @return �P���p���[�N
 */
//-----------------------------------------------------------------------------
APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_Create( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, HEAPID heapID )
{ 
  return APP_TASKMENU_WIN_CreateEx( res, item, x, y, w, APP_TASKMENU_PLATE_HEIGHT, 0, FALSE ,heapID );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �P�����쐬����  �w��ł����
 *
 *  @param  const APP_TASKMENU_WIN_WORK *res  ���̃��\�[�X
 *  @param  APP_TASKMENU_ITEMWORK *item �����
 *  @param  x     X���W�i�L�����P�ʁj
 *  @param  y     Y���W�i�L�����P�ʁj
 *  @param  w     ���i�L�����P�ʁj
 *  @param  h     �����i�L�����P�ʁj
 *  @param  anmDouble     BOOL:30f�ŉғ�������ꍇ�ŃA�j�����x��{�ɂ������ꍇTRUE
 *  @param  isCenter      BOOL:�Z���^�����O
 *
 *  @return �P���p���[�N
 */
//-----------------------------------------------------------------------------
APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_CreateEx( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, u8 h, BOOL anmDouble, const BOOL isCenter , HEAPID heapID )
{ 
  APP_TASKMENU_WIN_WORK *wk = APP_TASKMENU_WIN_CreateExCore( res, item, x, y, w, h, anmDouble, isCenter, heapID );

  GFL_BMPWIN_MakeTransWindow_VBlank( wk->bmpwin );

  return wk;
}

APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_CreateExNotTransCharacter( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, u8 h, BOOL anmDouble, const BOOL isCenter , HEAPID heapID )
{ 
  APP_TASKMENU_WIN_WORK *wk = APP_TASKMENU_WIN_CreateExCore( res, item, x, y, w, h, anmDouble, isCenter, heapID );

  GFL_BMPWIN_MakeScreen( wk->bmpwin );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->bmpwin) );

  return wk;
}

static APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_CreateExCore( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, u8 h, BOOL anmDouble, const BOOL isCenter , HEAPID heapID )
{
  APP_TASKMENU_WIN_WORK *wk;
  u8 drawX = 2;

  //���[�N�쐬
  wk  = GFL_HEAP_AllocMemory( heapID ,sizeof(APP_TASKMENU_WIN_WORK) );
  GFL_STD_MemClear( wk, sizeof(APP_TASKMENU_WIN_WORK) );
  wk->res = res;
  wk->isUpdateMsg = TRUE;
  wk->anmDouble = anmDouble+1;

  //BMPWIN
  wk->bmpwin  = GFL_BMPWIN_Create( res->frame, x, y , w, h, 
                        res->plt+1, GFL_BMP_CHRAREA_GET_B ); 

  //�ǂݍ���  
  APP_TASKMENU_TransFrame( wk->bmpwin, res, item->type );
  if( isCenter == TRUE )
  {
    const u32 len = PRINTSYS_GetStrWidth( item->str , res->fontHandle , 0 );
    drawX = ((w*8)-len-16)/2;
  }
  
  PRINTSYS_PrintQueColor( res->printQue , GFL_BMPWIN_GetBmp( wk->bmpwin ), 
                8+drawX , 4+2 , item->str , res->fontHandle , item->msgColor );

  // �v���[�g�̃A�j���̐F
  wk->animeColS = GX_RGB( APP_TASKMENU_ANIME_S_R, APP_TASKMENU_ANIME_S_G, APP_TASKMENU_ANIME_S_B );
  wk->animeColE = GX_RGB( APP_TASKMENU_ANIME_E_R, APP_TASKMENU_ANIME_E_G, APP_TASKMENU_ANIME_E_B );

  return wk;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �P���E�B���h�E�j��
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_Delete( APP_TASKMENU_WIN_WORK *wk )
{ 
  //BMPWIN�폜
  GFL_BMPWIN_ClearScreen( wk->bmpwin );
  
  APP_TASKMENU_WIN_DeleteCore( wk );
}

void APP_TASKMENU_WIN_DeleteNotClearScreen( APP_TASKMENU_WIN_WORK *wk )
{ 
  APP_TASKMENU_WIN_DeleteCore( wk );
}

static void APP_TASKMENU_WIN_DeleteCore( APP_TASKMENU_WIN_WORK *wk )
{
  //BMPWIN�폜
  GFL_BMPWIN_Delete( wk->bmpwin );
  //���[�N�폜
  GFL_HEAP_FreeMemory( wk );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �P���E�B���h�E�A�b�v�f�[�g
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_Update( APP_TASKMENU_WIN_WORK *wk )
{ 
  //�����̍X�V
  if( wk->isUpdateMsg )
  {
    if( PRINTSYS_QUE_IsExistTarget( wk->res->printQue , GFL_BMPWIN_GetBmp( wk->bmpwin ) ) == FALSE )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( wk->bmpwin );
      wk->isUpdateMsg = FALSE;
    }
  }
  
  if( wk->isDecide )
  {
    //���莞�A�j��
    const u8 isBlink = (wk->anmCnt/(APP_TASKMENU_ANM_INTERVAL/wk->anmDouble))%2;
    if( isBlink == 0 )
    {
      APP_TASKMENU_SetActiveItem( wk->bmpwin, wk->res, TRUE );
    }
    else
    {
      APP_TASKMENU_SetActiveItem( wk->bmpwin, wk->res, FALSE );
    }
    wk->anmCnt++;
  }
 
  //APP_TASKMENU_UpdatePalletAnime( &wk->transAnmCnt , &wk->transCol , wk->res->frame, wk->res->plt );
  APP_TASKMENU_UpdatePalletAnimeEx( &wk->transAnmCnt , &wk->transCol , wk->res->frame, wk->res->plt , wk->animeColS , wk->animeColE );
}
//----------------------------------------------------------------------------
/**
 *  @brief  �A�N�e�B�uON,OFF�@�i�A�N�e�B�u�Ȃ�ΑI�𒆂�PLTANM���o�@�m���A�N�e�B�u�Ȃ�΂��Ȃ��j
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 *  @param  isActive                  TRUE�A�N�e�B�u  FALSE�m���A�N�e�B�u
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_SetActive( APP_TASKMENU_WIN_WORK *wk, BOOL isActive )
{ 
  wk->transAnmCnt = 0;
  APP_TASKMENU_SetActiveItem( wk->bmpwin, wk->res, isActive );
}
//----------------------------------------------------------------------------
/**
 *  @brief  ����ON,OFF
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 *  @param  isDecide  TRUE�Ō���A�j��  FALSE�łȂ�
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_SetDecide( APP_TASKMENU_WIN_WORK *wk, BOOL isDecide )
{ 
  wk->isDecide  = isDecide;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �����Ԃ��擾
 *
 *  @param  const APP_TASKMENU_WIN_WORK *wk   ���[�N
 *
 *  @return TRUE�Ō�����  FALSE�łȂ�
 */
//-----------------------------------------------------------------------------
BOOL APP_TASKMENU_WIN_IsDecide( const APP_TASKMENU_WIN_WORK *wk )
{ 
  return wk->isDecide;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �I���`�F�b�N
 *
 *  @param  APP_TASKMENU_WIN_WORK *work ���[�N
 *
 *  @retval TRUE:�A�j���J�E���g�I��
 */
//-----------------------------------------------------------------------------
const BOOL APP_TASKMENU_WIN_IsFinish( const APP_TASKMENU_WIN_WORK *work )
{
  if( work->anmCnt < (APP_TASKMENU_ANM_CNT/work->anmDouble) )
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
//----------------------------------------------------------------------------
/**
 *  @brief  �^�b�`�`�F�b�N
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 *
 *  @return
 */
//-----------------------------------------------------------------------------
const BOOL APP_TASKMENU_WIN_IsTrg( const APP_TASKMENU_WIN_WORK *wk )
{ 
  u32 x, y;
  GFL_RECT  rect;

  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  { 
    rect.left   = GFL_BMPWIN_GetPosX( wk->bmpwin ) * 8;
    rect.top    = GFL_BMPWIN_GetPosY( wk->bmpwin ) * 8;
    rect.right  = rect.left + GFL_BMPWIN_GetSizeX( wk->bmpwin ) * 8;
    rect.bottom = rect.top + GFL_BMPWIN_GetSizeX( wk->bmpwin ) * 8;

    if( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
        & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)))
    {
      return TRUE;
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����OFF��ԂɃ��Z�b�g����
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_ResetDecide( APP_TASKMENU_WIN_WORK *wk )
{
  wk->isDecide  = 0;
  wk->anmCnt    = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �p���b�g�ƃv���[�g�̃A�j���̐F��ݒ肷��
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk         ���[�N
 *  @param  APP_TASKMENU_WIN_WORK *res        ���̃��\�[�X
 *  @param                        plt         �p���b�g�ԍ�

 *  @param                        animeColS   �v���[�g�̃A�j���̊J�n�F
 *  @param                        animeColE   �v���[�g�̃A�j���̏I���F
 
 
 *  @note   res��wk�𐶐�����ۂɓn�������̂Ɠ������̂�n���ĉ������B
 *  @note   plt��plt+1���g�p���܂��B
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_SetPaletteAndAnimeColor( APP_TASKMENU_WIN_WORK *wk, APP_TASKMENU_RES *res,
                                               u8 plt, GXRgb animeColS, GXRgb animeColE )
{
  res->plt = plt;

  wk->animeColS = animeColS;
  wk->animeColE = animeColE;

  GFL_BMPWIN_SetPalette( wk->bmpwin , plt+1 );  // +1���Ă���̂�APP_TASKMENU_WIN_CreateEx�Q�l
  GFL_BMPWIN_MakeScreen( wk->bmpwin );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->bmpwin) );
}


//----------------------------------------------------------------------------
/**
 *  @brief  �t���[����BMPWIN�ɓ]��
 *
 *  @param  GFL_BMPWIN *bmpwin          BMPWIN
 *  @param  APP_TASKMENU_WIN_WORK *res  ���\�[�X
 *  @param  type                        ���̎��
 */
//-----------------------------------------------------------------------------
static void APP_TASKMENU_TransFrame( GFL_BMPWIN *bmpwin, const APP_TASKMENU_RES *res, APP_TASKMENU_WIN_TYPE type )
{ 
  u32 dstAdr;
  u32 srcAdr;
  u8 w;
  u8 h;


  for( h = 0; h < GFL_BMPWIN_GetSizeY(bmpwin); h++ )
  { 
    for( w = 0; w < GFL_BMPWIN_GetSizeX(bmpwin); w++ )
    { 
      //�]����A�h���X�͘A��
      dstAdr = (u32)(GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( bmpwin )))
        + 0x20*(h*GFL_BMPWIN_GetSizeX(bmpwin)+w);

      //�]�����͏ꏊ�ɂ��ω�
      if( h == 0 && w == 0 )
      { 
        //����
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_LEFTTOP;
      }
      else if( h == 0 && w == GFL_BMPWIN_GetSizeX(bmpwin)-1 )
      { 
        //�E��
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_RIGHTTOP;
      }
      else if( h == 0 )
      { 
        //��
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_TOP;
      }
      else if( h == GFL_BMPWIN_GetSizeY(bmpwin)-1 && w == 0 )
      { 
        //����
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_LEFTDOWN;
      }
      else if( h == GFL_BMPWIN_GetSizeY(bmpwin)-1 && w == GFL_BMPWIN_GetSizeX(bmpwin)-1 )
      { 
        //�E��
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_RIGHTDOWN;
      }
      else if( h == GFL_BMPWIN_GetSizeY(bmpwin)-1 )
      { 
        //��
        srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_DOWN;
      }
      else
      { 
        //���̑�
        //�ꏊ���L���̏ꏊ�ŋL������ON�Ȃ�΋L����
        //����ȊO�͕��ʂɓh��
        if( type != APP_TASKMENU_WIN_TYPE_NORMAL
          && w == GFL_BMPWIN_GetSizeX(bmpwin)-3 )
        { 
          srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*(APP_TASKMENU_PLATE_NCG_MARK_RET
                                                  + (type-1) * APP_TASKMENU_PLATE_NCG_WIDTH);
        }
        else if( type != APP_TASKMENU_WIN_TYPE_NORMAL
          && w == GFL_BMPWIN_GetSizeX(bmpwin)-2 )
        { 
          srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*(APP_TASKMENU_PLATE_NCG_MARK_RET
                                                  + (type-1) * APP_TASKMENU_PLATE_NCG_WIDTH + 1);
        }
        else if( w == 0 )
        { 
          srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_LEFT;
        }
        else if( w == GFL_BMPWIN_GetSizeX(bmpwin)-1 )
        { 
          srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_RIGHT;
        }
        else
        { 
          srcAdr = (u32)(res->ncg_data->pRawData) + 0x20*APP_TASKMENU_PLATE_NCG_CENTER;
        }
      }
    
      //�]��
      GFL_STD_MemCopy32( (void*)srcAdr, (void*)dstAdr, 0x20 );
    }
  }

}


//----------------------------------------------------------------------------
/**
 *  @brief  ��\��
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_Hide( APP_TASKMENU_WIN_WORK *wk )
{
  GFL_BMPWIN_ClearScreen( wk->bmpwin );
  GFL_BG_LoadScreenV_Req(wk->res->frame);
}
//----------------------------------------------------------------------------
/**
 *  @brief  �\��
 *
 *  @param  APP_TASKMENU_WIN_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void APP_TASKMENU_WIN_Show( APP_TASKMENU_WIN_WORK *wk )
{
  GFL_BMPWIN_MakeScreen( wk->bmpwin );
  GFL_BG_LoadScreenV_Req(wk->res->frame);
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����`�撆�t���O��Ԃ�
 *
 *	@param	const APP_TASKMENU_WIN_WORK *wk   ���[�N
 *
 *	@return TRUE�Ȃ�Ε����`�撆  FALSE�Ȃ�Ώ����I����
 */
//-----------------------------------------------------------------------------
BOOL APP_TASKMENU_WIN_IsUpdateMsg( const APP_TASKMENU_WIN_WORK *wk )
{
  return wk->isUpdateMsg;
}
