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
#include "app_menu_common.naix"
#include "message.naix"
#include "msg/msg_pokelist.h"
#include "msg/msg_wazaname.h"
#include "app/app_menu_common.h"

#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "app/app_taskmenu.h"

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
  u8  itemNum;
  u16 itemArr[PLIST_MENU_ITEM_MAX];
  APP_TASKMENU_ITEMWORK itemWork[PLIST_MENU_ITEM_MAX];

  APP_TASKMENU_WORK *taskMenuWork;
  APP_TASKMENU_INITWORK *initTaskMenu;

  //���j���[�y��
  NNSG2dCharacterData *ncgData;
  void *ncgRes; 
};
//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void PLIST_MENU_CreateItem( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , PLIST_MENU_ITEM_TYPE *itemArr );
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
  GFL_ARC_UTIL_TransVramPalette( APP_COMMON_GetArcId() , NARC_app_menu_common_task_menu_NCLR , 
                    PALTYPE_MAIN_BG , PLIST_BG_PLT_MENU_ACTIVE*32 , 32*2 , work->heapId );
  menuWork->ncgRes = GFL_ARC_UTIL_LoadBGCharacter( APP_COMMON_GetArcId() , NARC_app_menu_common_task_menu_NCGR , FALSE , 
                    &menuWork->ncgData , work->heapId );
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
  APP_TASKMENU_INITWORK taskInitWork;
  
  PLIST_MENU_CreateItem( work,menuWork,itemArr );

  taskInitWork.heapId = work->heapId;
  taskInitWork.itemNum = menuWork->itemNum;
  taskInitWork.itemWork = menuWork->itemWork;
  taskInitWork.bgFrame = PLIST_BG_MENU;
  taskInitWork.palNo = PLIST_BG_PLT_MENU_ACTIVE;
  taskInitWork.msgHandle = work->msgHandle;
  taskInitWork.fontHandle = work->fontHandle;
  taskInitWork.printQue = work->printQue;
  
  menuWork->taskMenuWork = APP_TASKMENU_OpenMenu( &taskInitWork );
}

//--------------------------------------------------------------
//	���j���[����
//--------------------------------------------------------------
void PLIST_MENU_CloseMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  APP_TASKMENU_CloseMenu(menuWork->taskMenuWork);

  for( i=0;i<menuWork->itemNum;i++ )
  {
    GFL_STR_DeleteBuffer( menuWork->itemWork[i].str );
  }
}


//--------------------------------------------------------------
//	���j���[�X�V
//--------------------------------------------------------------
void PLIST_MENU_UpdateMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  u8 i;
  APP_TASKMENU_UpdateMenu(menuWork->taskMenuWork);
}

const PLIST_MENU_ITEM_TYPE PLIST_MENU_IsFinish( PLIST_WORK *work , PLIST_MENU_WORK *menuWork )
{
  if( APP_TASKMENU_IsFinish( menuWork->taskMenuWork ) == FALSE )
  {
    return PMIT_NONE;
  }
  else
  {
    const u8 ret = APP_TASKMENU_GetCursorPos( menuWork->taskMenuWork );
    return menuWork->itemArr[ret];
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
    switch( itemArr[i] )
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

  for( i=0;i<menuWork->itemNum;i++ )
  {
    menuWork->itemWork[i].str = PLIST_MENU_CreateMenuStr( work,menuWork,menuWork->itemArr[i] );
    if( menuWork->itemArr[i] >= PMIT_WAZA_1 && menuWork->itemArr[i] <= PMIT_WAZA_4 )
    {
      menuWork->itemWork[i].msgColor = PRINTSYS_LSB_Make( PLIST_FONT_MENU_WAZA_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
    }
    else
    {
      menuWork->itemWork[i].msgColor = PRINTSYS_LSB_Make( PLIST_FONT_MENU_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
    }
    
    if( menuWork->itemArr[i] == PMIT_CLOSE )
    {
      menuWork->itemWork[i].isReturn = TRUE;
    }
    else
    {
      menuWork->itemWork[i].isReturn = FALSE;
    }
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

//--------------------------------------------------------------
//	�o�g���I��p�ɍ��W�w��ŏo����悤��
//--------------------------------------------------------------
GFL_BMPWIN* PLIST_MENU_CreateMenuWin_BattleMenu( PLIST_WORK *work , PLIST_MENU_WORK *menuWork , u32 strId , u8 charX , u8 charY , const BOOL isReturn )
{
  
  //FIXME �����printQue��Update�������̂Ō�ő�������
  GFL_BMPWIN* bmpWin;
  
  PRINTSYS_LSB col;
  STRBUF *str = GFL_MSG_CreateString( work->msgHandle , strId );

  bmpWin = GFL_BMPWIN_Create( PLIST_BG_MENU ,
                  charX , charY , 
                  PLIST_MENU_PLATE_WIDTH , PLIST_MENU_PLATE_HEIGHT , 
                  PLIST_BG_PLT_MENU_NORMAL , GFL_BMP_CHRAREA_GET_B );
  //�v���[�g�̊G�𑗂�
  if( isReturn == FALSE )
  {
    GFL_STD_MemCopy32( menuWork->ncgData->pRawData , GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( bmpWin )) ,
                    0x20*PLIST_MENU_PLATE_WIDTH*PLIST_MENU_PLATE_HEIGHT );
  }
  else
  {
    GFL_STD_MemCopy32( (void*)((u32)menuWork->ncgData->pRawData + 0x20*PLIST_MENU_PLATE_WIDTH*PLIST_MENU_PLATE_HEIGHT) , 
                    GFL_BMP_GetCharacterAdrs(GFL_BMPWIN_GetBmp( bmpWin )) ,
                    0x20*PLIST_MENU_PLATE_WIDTH*PLIST_MENU_PLATE_HEIGHT );
  }
  col = PRINTSYS_LSB_Make( PLIST_FONT_MENU_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
//  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ), 
//                      8+PLIST_MSG_STR_OFS_X , 4+PLIST_MSG_STR_OFS_Y , str , work->fontHandle , col );
  GFL_FONTSYS_SetColor(PLIST_FONT_MENU_LETTER,PLIST_FONT_MENU_SHADOW,PLIST_FONT_MENU_BACK);
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( bmpWin ),8+PLIST_MSG_STR_OFS_X , 4+PLIST_MSG_STR_OFS_Y ,
                  str , work->fontHandle );
  GFL_FONTSYS_SetDefaultColor();
  GFL_STR_DeleteBuffer( str );
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpWin );

  return bmpWin;
}

