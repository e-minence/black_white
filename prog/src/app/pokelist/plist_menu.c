//======================================================================
/**
 * @file	plist_menu.c
 * @brief	�|�P�������X�g ���j���[
 * @author	ariizumi
 * @data	09/06/18
 *
 * ���W���[�����FPLIST_MENU
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "pokelist_gra.naix"
#include "message.naix"
#include "msg/msg_pokelist.h"
#include "msg/msg_wazaname.h"

#include "print/wordset.h"
#include "system/bmp_winframe.h"

#include "plist_sys.h"
#include "plist_menu.h"
#include "plist_plate.h"
#include "plist_snd_def.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PLIST_MENU_ITEM_MAX (8)

#define PLIST_MENU_PLATE_LEFT (19)
#define PLIST_MENU_PLATE_WIDTH (13)
#define PLIST_MENU_PLATE_HEIGHT (3)

//���莞�_�ŃA�j��
#define PLIST_MENU_ANM_CNT (16)
#define PLIST_MENU_ANM_INTERVAL (4)

//�v���[�g�̃A�j���Bsin�g���̂�0�`0xFFFF�̃��[�v
#define PLIST_MENU_ANIME_VALUE (0x400)
#define PLIST_MENU_ANIME_S_R (5)
#define PLIST_MENU_ANIME_S_G (10)
#define PLIST_MENU_ANIME_S_B (13)
#define PLIST_MENU_ANIME_E_R (12)
#define PLIST_MENU_ANIME_E_G (25)
#define PLIST_MENU_ANIME_E_B (30)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PLIST_MENU_WORK
{
  u8 itemNum;
  u8 cursorPos;
  u8 anmCnt;
  u16 transAnmCnt;
  GXRgb transCol;
  BOOL isUpdateMsg;
  BOOL isDecide;

  GFL_BMPWIN *menuWin[PLIST_MENU_ITEM_MAX];
  u16        itemArr[PLIST_MENU_ITEM_MAX];
  //���j���[�y��
  NNSG2dCharacterData *ncgData;
  void *ncgRes; 

};
//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void PLIST_MENU_CreateItem( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr );
static void PLIST_MENU_CreateMenuWin( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
static STRBUF* PLIST_MENU_CreateMenuStr( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , const PLIST_MENU_ITEM_TYPE type );

static void PLIST_MENU_UpdateKey( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );
static void PLIST_MENU_UpdateTP( PLIST_WORK *work , PLIST_MENU_WORK *menuWork );

static void PLIST_MENU_SetActiveItem( PLIST_MENU_WORK *menuWork , const u8 idx , const BOOL isActive );

//--------------------------------------------------------------
//	���j���[�V�X�e��������
//--------------------------------------------------------------
PLIST_MENU_WORK* PLIST_MENU_CreateSystem( PLIST_WORK *work )
{
  u8 i;
  PLIST_MENU_WORK* menuWork = GFL_HEAP_AllocMemory( work->heapId , sizeof( PLIST_MENU_WORK ) );

  //�v���[�g�̓y��̊G
  menuWork->ncgRes = GFL_ARC_UTIL_LoadBGCharacter( ARCID_POKELIST , NARC_pokelist_gra_list_sel_NCGR , FALSE , 
                    &menuWork->ncgData , work->heapId );
  
  menuWork->isUpdateMsg = FALSE;
  
  for(i=0;i<8;i++)
  {
    menuWork->menuWin[i] = NULL;
  }

  return menuWork;
}

//--------------------------------------------------------------
//	���j���[�V�X�e���J��
//--------------------------------------------------------------
void PLIST_MENU_DeleteSystem( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  GFL_HEAP_FreeMemory( menuWork->ncgRes );
  GFL_HEAP_FreeMemory( menuWork );
}

//--------------------------------------------------------------
//	���j���[�J��
//--------------------------------------------------------------
void PLIST_MENU_OpenMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr )
{
  u8 i;
  
  PLIST_MENU_CreateItem( work,menuWork,itemArr );
  PLIST_MENU_CreateMenuWin( work,menuWork );

  menuWork->isDecide = FALSE;
  menuWork->cursorPos = menuWork->itemNum-1;
  menuWork->anmCnt = 0;
  menuWork->transAnmCnt = 0;
  
  if( work->ktst == GFL_APP_KTST_KEY )
  {
    PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , TRUE );
  }
  
  menuWork->isUpdateMsg = TRUE;
}

//--------------------------------------------------------------
//	���j���[����
//--------------------------------------------------------------
void PLIST_MENU_CloseMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  for(i=0;i<menuWork->itemNum;i++)
  {
    GFL_BMPWIN_Delete( menuWork->menuWin[i] );
    menuWork->menuWin[i] = NULL;
  }
}


//--------------------------------------------------------------
//	���j���[�X�V
//--------------------------------------------------------------
void PLIST_MENU_UpdateMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  
  //�����̍X�V
  if( menuWork->isUpdateMsg == TRUE )
  {
    BOOL isFinish = TRUE;
    for(i=0;i<menuWork->itemNum;i++)
    {
      if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( menuWork->menuWin[i] ) ) == TRUE )
      {
        isFinish = FALSE;
      }
    }
    if( isFinish == TRUE )
    {
      for(i=0;i<menuWork->itemNum;i++)
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( menuWork->menuWin[i] );
      }
      menuWork->isUpdateMsg = FALSE;
    }
  }
  
  if( menuWork->isDecide == FALSE )
  {
    PLIST_MENU_UpdateKey( work , menuWork );
    if( menuWork->isDecide == FALSE )
    {
      PLIST_MENU_UpdateTP( work , menuWork );
    }
  }
  else
  {
    //���莞�A�j��
    const u8 isBlink = (menuWork->anmCnt/PLIST_MENU_ANM_INTERVAL)%2;
    if( isBlink == 0 )
    {
      PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , TRUE );
    }
    else
    {
      PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , FALSE );
    }
    menuWork->anmCnt++;
  }
  
  //�v���[�g�A�j��
  if( menuWork->transAnmCnt + PLIST_MENU_ANIME_VALUE >= 0x10000 )
  {
    menuWork->transAnmCnt = menuWork->transAnmCnt+PLIST_MENU_ANIME_VALUE-0x10000;
  }
  else
  {
    menuWork->transAnmCnt += PLIST_MENU_ANIME_VALUE;
  }
  {
    //1�`0�ɕϊ�
    const fx16 cos = (FX_CosIdx(menuWork->transAnmCnt)+FX16_ONE)/2;
    const u8 r = PLIST_MENU_ANIME_S_R + (((PLIST_MENU_ANIME_E_R-PLIST_MENU_ANIME_S_R)*cos)>>FX16_SHIFT);
    const u8 g = PLIST_MENU_ANIME_S_G + (((PLIST_MENU_ANIME_E_G-PLIST_MENU_ANIME_S_G)*cos)>>FX16_SHIFT);
    const u8 b = PLIST_MENU_ANIME_S_B + (((PLIST_MENU_ANIME_E_B-PLIST_MENU_ANIME_S_B)*cos)>>FX16_SHIFT);
    
    menuWork->transCol = GX_RGB(r, g, b);
    
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        PLIST_BG_PLT_MENU_ACTIVE * 32 + PLIST_MENU_ANIME_COL*2 ,
                                        &menuWork->transCol , 2 );
  }

}

const PLIST_MENU_ITEM_TYPE PLIST_MENU_IsFinish( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  if( menuWork->anmCnt < PLIST_MENU_ANM_CNT )
  {
    return PMIT_NONE;
  }
  else
  {
    return menuWork->itemArr[menuWork->cursorPos];
  }
}

#pragma mark [>init func
//--------------------------------------------------------------
//	���j���[�̍��ڂ����
//--------------------------------------------------------------
static void PLIST_MENU_CreateItem(  PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr )
{
  int i;
  int arrNum = 0;
  
  //���`�F�b�N
  while( itemArr[arrNum] != PMIT_END_LIST )
  {
    arrNum++;
    
    if( arrNum >= PLIST_MENU_ITEM_MAX )
    {
      GF_ASSERT_MSG(NULL,"PLIST_MENU Item num is over!!\n")
      break;
    }
  }
  
  menuWork->itemNum = 0;
  for( i=0;i<arrNum;i++ )
  {
    switch( itemArr[arrNum-(i+1)] )
    {
    case PMIT_STATSU:  //����������
      menuWork->itemArr[menuWork->itemNum] = PMIT_STATSU;
      menuWork->itemNum += 1;
      break;

    case PMIT_HIDEN:   //��`�Z�B�����Ō����ǉ����܂�
      {
        u8 idx;
        for( idx=0;idx<4;idx++ )
        {
          const u32 ret = PLIST_UTIL_CheckFieldWaza( work->selectPokePara , idx );
          if( ret != 0 )
          {
            menuWork->itemArr[menuWork->itemNum] = PMIT_WAZA_1+idx;
            menuWork->itemNum += 1;
          }
        }
      }
      break;

    case PMIT_CHANGE:  //����ւ�
      menuWork->itemArr[menuWork->itemNum] = PMIT_CHANGE;
      menuWork->itemNum += 1;
      break;

    case PMIT_ITEM:    //������
      menuWork->itemArr[menuWork->itemNum] = PMIT_ITEM;
      menuWork->itemNum += 1;
      break;

    case PMIT_CLOSE:   //����
      menuWork->itemArr[menuWork->itemNum] = PMIT_CLOSE;
      menuWork->itemNum += 1;
      break;

    case PMIT_LEAVE:   //�a����(��Ă�
      menuWork->itemArr[menuWork->itemNum] = PMIT_LEAVE;
      menuWork->itemNum += 1;
      break;

    case PMIT_SET_JOIN:    //�Q������E�Q�����Ȃ�
      //FIXME �����������`�F�b�N
      {
        const battleOrder = PLIST_PLATE_GetBattleOrder( work->plateWork[ work->pokeCursor ] );
        if( battleOrder == PPBO_JOIN_OK )
        {
          menuWork->itemArr[menuWork->itemNum] = PMIT_RET_JOIN;
          menuWork->itemNum += 1;
        }
        else
        if( battleOrder <= PPBO_JOIN_6 )
        {
          menuWork->itemArr[menuWork->itemNum] = PMIT_JOIN_CANCEL;
          menuWork->itemNum += 1;
        }
        //���̑��͑����Ȃ��I
      }
      break;
      
    case PMIT_GIVE:    //��������
      menuWork->itemArr[menuWork->itemNum] = PMIT_GIVE;
      menuWork->itemNum += 1;
      break;

    case PMIT_TAKE:   //�a����
      menuWork->itemArr[menuWork->itemNum] = PMIT_TAKE;
      menuWork->itemNum += 1;
      break;

    
    default:
      GF_ASSERT_MSG(0,"PLIST_MENU Invalid item type!![%d]\n",itemArr[arrNum-(i+1)] );
      break;
    }
  }
}

//--------------------------------------------------------------
//	���j���[��BmpWin�ƕ��������
//--------------------------------------------------------------
static void PLIST_MENU_CreateMenuWin(  PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  
  for(i=0;i<menuWork->itemNum;i++)
  {
    PRINTSYS_LSB col;
    STRBUF *str = PLIST_MENU_CreateMenuStr( work , menuWork , menuWork->itemArr[i] );

    menuWork->menuWin[i] = GFL_BMPWIN_Create( PLIST_BG_MENU ,
                        PLIST_MENU_PLATE_LEFT , 24-((i+1)*PLIST_MENU_PLATE_HEIGHT) , 
                        PLIST_MENU_PLATE_WIDTH , PLIST_MENU_PLATE_HEIGHT , 
                        PLIST_BG_PLT_MENU_NORMAL , GFL_BMP_CHRAREA_GET_B );
    //�v���[�g�̊G�𑗂�
    GFL_STD_MemCopy32( menuWork->ncgData->pRawData , GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( menuWork->menuWin[i] )) ,
                     0x20*PLIST_MENU_PLATE_WIDTH*PLIST_MENU_PLATE_HEIGHT );
    if( menuWork->itemArr[i] >= PMIT_WAZA_1 && menuWork->itemArr[i] <= PMIT_WAZA_4 )
    {
      col = PRINTSYS_LSB_Make( PLIST_FONT_MENU_WAZA_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
    }
    else
    {
      col = PRINTSYS_LSB_Make( PLIST_FONT_MENU_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
    }
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( menuWork->menuWin[i] ), 
                        8+PLIST_MSG_STR_OFS_X , 4+PLIST_MSG_STR_OFS_Y , str , work->fontHandle , col );
    GFL_STR_DeleteBuffer( str );
    GFL_BMPWIN_MakeTransWindow_VBlank( menuWork->menuWin[i] );
  }
}

//--------------------------------------------------------------
//	�A�C�e�����Ƃ̕����̍쐬
//--------------------------------------------------------------
static STRBUF* PLIST_MENU_CreateMenuStr( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , const PLIST_MENU_ITEM_TYPE type )
{
  static const u32 msgArr[PMIT_MAX] =
  {
    mes_pokelist_05_02 ,  //PMIT_STATSU,  //����������
    0 ,          //PMIT_HIDEN,   //��`�Z�B�����Ō����ǉ����܂�
    mes_pokelist_05_01 ,  //PMIT_CHANGE,  //����ւ�
    mes_pokelist_05_03 ,  //PMIT_ITEM,    //������
    mes_pokelist_05_08 ,  //PMIT_CLOSE,   //����
    mes_pokelist_05_07 ,  //PMIT_LEAVE,   //�a����(��Ă�
    0 ,          //PMIT_SET_JOIN,    //�Q������(�Q�����Ȃ�)
    mes_pokelist_05_16 ,  //  PMIT_GIVE,    //��������
    mes_pokelist_05_17 ,  //  PMIT_TAKE,    //�a����
    0 ,          //PMIT_WAZA_1,   //��`�p�Z�X���b�g�P
    0 ,          //PMIT_WAZA_2,   //��`�p�Z�X���b�g�Q
    0 ,          //PMIT_WAZA_3,   //��`�p�Z�X���b�g�R
    0 ,          //PMIT_WAZA_4,   //��`�p�Z�X���b�g�S
    mes_pokelist_05_10 ,  //PMIT_RET_JOIN,      //�Q������
    mes_pokelist_05_11 ,  //PMIT_JOIN_CANCEL,   //�Q�����Ȃ�
  };
  STRBUF *str;
  
  if( type >= PMIT_WAZA_1 && type <= PMIT_WAZA_4 )
  {
    const u32 wazaID = PP_Get( work->selectPokePara , ID_PARA_waza1+(type-PMIT_WAZA_1) , NULL );
    GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_wazaname_dat , work->heapId );
    
    //��`�Z�͓��ꏈ��
    str = GFL_MSG_CreateString( msgHandle , wazaID );
  }
  else
  {
    GF_ASSERT_MSG( msgArr[type] != 0 , "PLIST_MENU CreateMenuStr type error!![%d]\n",type );
    str = GFL_MSG_CreateString( work->msgHandle , msgArr[type] );
  }
  return str;
}

#pragma mark [>main func
static void PLIST_MENU_UpdateKey( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  const int trg = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();

  if( work->ktst == GFL_APP_KTST_TOUCH )
  {
    if( trg != 0 )
    {
      PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , TRUE );
      work->ktst = GFL_APP_KTST_KEY;
    }
  }
  else
  {
    const u8 befPos = menuWork->cursorPos;
    if( repeat & PAD_KEY_UP )
    {
      if( menuWork->cursorPos == menuWork->itemNum-1 )
      {
        menuWork->cursorPos = 0;
      }
      else
      {
        menuWork->cursorPos++;
      }
      PMSND_PlaySystemSE( PLIST_SND_CURSOR );
    }
    else
    if( repeat & PAD_KEY_DOWN )
    {
      if( menuWork->cursorPos == 0 )
      {
        menuWork->cursorPos = menuWork->itemNum-1;
      }
      else
      {
        menuWork->cursorPos--;
      }
      PMSND_PlaySystemSE( PLIST_SND_CURSOR );
    }
    else
    if( trg & PAD_BUTTON_A )
    {
      menuWork->isDecide = TRUE;
      PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    }
    else
    if( trg & PAD_BUTTON_B )
    {
      menuWork->cursorPos = 0;
      menuWork->isDecide = TRUE;
      PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    }
    
    if( befPos != menuWork->cursorPos )
    {
      PLIST_MENU_SetActiveItem( menuWork , befPos , FALSE );
      PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , TRUE );
    }
  }
}

static void PLIST_MENU_UpdateTP( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  int ret;
  GFL_UI_TP_HITTBL hitTbl[PLIST_MENU_ITEM_MAX+1];
  
  //�e�[�u���̍쐬
  for( i=0 ; i<menuWork->itemNum ; i++ )
  {
    hitTbl[i].rect.top    = 192 - (PLIST_MENU_PLATE_HEIGHT*8*(i+1));
    hitTbl[i].rect.bottom = 192 - (PLIST_MENU_PLATE_HEIGHT*8*i) - 1;
    hitTbl[i].rect.left   = PLIST_MENU_PLATE_LEFT*8;
    hitTbl[i].rect.right  = (PLIST_MENU_PLATE_LEFT+PLIST_MENU_PLATE_WIDTH)*8 - 1;
  }
  hitTbl[i].circle.code = GFL_UI_TP_HIT_END;
  
  ret = GFL_UI_TP_HitTrg( hitTbl );
  
  if( ret != GFL_UI_TP_HIT_NONE )
  {
    work->ktst = GFL_APP_KTST_TOUCH;
    PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , FALSE );
    menuWork->cursorPos = ret;
    menuWork->isDecide = TRUE;
    PLIST_MENU_SetActiveItem( menuWork , menuWork->cursorPos , TRUE );
    if( menuWork->cursorPos == 0 )
    {
      PMSND_PlaySystemSE( PLIST_SND_DECIDE );
    }
    else
    {
      PMSND_PlaySystemSE( PLIST_SND_CANCEL );
    }
  }
}


#pragma mark [>util
static void PLIST_MENU_SetActiveItem( PLIST_MENU_WORK *menuWork , const u8 idx , const BOOL isActive )
{
  if( isActive == TRUE )
  {
    GFL_BMPWIN_SetPalette( menuWork->menuWin[idx] , PLIST_BG_PLT_MENU_ACTIVE );
  }
  else if( isActive == FALSE )
  {
    GFL_BMPWIN_SetPalette( menuWork->menuWin[idx] , PLIST_BG_PLT_MENU_NORMAL );
  }
	GFL_BMPWIN_MakeScreen( menuWork->menuWin[idx] );
  GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(menuWork->menuWin[idx]) );
}



//--------------------------------------------------------------
//	�o�g���I��p�ɍ��W�w��ŏo����悤��
//--------------------------------------------------------------
GFL_BMPWIN* PLIST_MENU_CreateMenuWin_BattleMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , u32 strId , u8 charX , u8 charY )
{
  GFL_BMPWIN* bmpWin;
  
  PRINTSYS_LSB col;
  STRBUF *str = GFL_MSG_CreateString( work->msgHandle , strId );

  bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU ,
                  charX , charY , 
                  PLIST_MENU_PLATE_WIDTH , PLIST_MENU_PLATE_HEIGHT , 
                  PLIST_BG_PLT_MENU_NORMAL , GFL_BMP_CHRAREA_GET_B );
  //�v���[�g�̊G�𑗂�
  GFL_STD_MemCopy32( menuWork->ncgData->pRawData , GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( bmpWin )) ,
                   0x20*PLIST_MENU_PLATE_WIDTH*PLIST_MENU_PLATE_HEIGHT );
  col = PRINTSYS_LSB_Make( PLIST_FONT_MENU_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ), 
                      8+PLIST_MSG_STR_OFS_X , 4+PLIST_MSG_STR_OFS_Y , str , work->fontHandle , col );
  GFL_STR_DeleteBuffer( str );
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpWin );

  return bmpWin;
}

