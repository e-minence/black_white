//======================================================================
/**
 * @file	plist_battle.c
 * @brief	�|�P�������X�g �o�g���\���n����(����p�[�e�B�[�E��������
 * @author	ariizumi
 * @data	09/10/15
 *
 * ���W���[�����FPLIST_BATTLE
 */
//======================================================================
#include <gflib.h>
#include "buflen.h"
#include "app/app_menu_common.h"
#include "pokeicon/pokeicon.h"
#include "system/main.h"
#include "system/gfl_use.h"
#include "debug/debug_flg.h"

#include "arc_def.h"
#include "pokelist_gra.naix"
#include "msg/msg_pokelist.h"

#include "plist_sys.h"
#include "plist_message.h"
#include "plist_battle.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define PLIST_BATTLE_ICON_LEFT  (0)
#define PLIST_BATTLE_ICON_TOP   (4)
#define PLIST_BATTLE_ICON_WIDTH  (5)
#define PLIST_BATTLE_ICON_HEIGHT (6)  //�X�y�[�X����

#define PLIST_BATTLE_TIME_STR_LEFT   ( 7)
#define PLIST_BATTLE_TIME_STR_TOP    (21)
#define PLIST_BATTLE_TIME_STR_WIDTH  (11)
#define PLIST_BATTLE_TIME_STR_HEIGHT ( 3)
#define PLIST_BATTLE_TIME_NUM_LEFT   (20)
#define PLIST_BATTLE_TIME_NUM_TOP    (21)
#define PLIST_BATTLE_TIME_NUM_WIDTH  ( 5)
#define PLIST_BATTLE_TIME_NUM_HEIGHT ( 3)

#define PLIST_BATTLE_PAL_ANM_MAX (60)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct 
{
  BOOL isUpdateStr;
  GFL_BMPWIN  *sexStrWin;
  GFL_BMPWIN  *lvStrWin;
  u32       pokeIconRes;
  GFL_CLWK *pokeIcon;
  GFL_CLWK *itemIcon;
}PLIST_BATTLE_POKE_ICON;

struct _PLIST_BATTLE_PARAM_WORK
{
  
  BOOL isUpdateStr;
  GFL_BMPWIN  *nameStr;
  
  PLIST_BATTLE_POKE_ICON *icon[PLIST_LIST_MAX];
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PLIST_BATTLE_InitCell( PLIST_WORK *work );
static void PLIST_BATTLE_TermCell( PLIST_WORK *work );
static void PLIST_BATTLE_InitResource( PLIST_WORK *work );
static void PLIST_BATTLE_TermResource( PLIST_WORK *work );

static PLIST_BATTLE_PARAM_WORK* PLIST_BATTLE_CreateBattleParam( PLIST_WORK *work , 
                        PL_COMM_BATTLE_PARAM *initParam , u8 charX, u8 charY , u8 space );
static void PLIST_BATTLE_DeleteBattleParam( PLIST_WORK *work , PLIST_BATTLE_PARAM_WORK *param );
static void PLIST_BATTLE_UpdateBattleParam( PLIST_WORK *work , PLIST_BATTLE_PARAM_WORK *param );

static PLIST_BATTLE_POKE_ICON* PLIST_BATTLE_CreateBattleParamIcon( PLIST_WORK *work , POKEMON_PARAM *pp , const u8 posX , const u8 posY );
static void PLIST_BATTLE_DeleteBattleParamIcon( PLIST_WORK *work , PLIST_BATTLE_POKE_ICON *iconWork );
static void PLIST_BATTLE_UpdateBattleParamIcon( PLIST_WORK *work , PLIST_BATTLE_POKE_ICON *iconWork );

static void PLIST_BATTLE_DispWaitingMessage( PLIST_WORK *work );

static void PLIST_BATTLE_InitTimeLimit( PLIST_WORK *work );
static void PLIST_BATTLE_TermTimeLimit( PLIST_WORK *work );
static void PLIST_BATTLE_UpdateTimeLimit( PLIST_WORK *work );

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
void PLIST_BATTLE_InitBattle( PLIST_WORK *work )
{
  PLIST_BATTLE_InitCell( work );
  PLIST_BATTLE_InitResource( work );

  if( work->plData->is_disp_party == TRUE )
  {
    switch( work->plData->comm_type )
    {
    case PL_COMM_SINGLE:  //�GA�̂�
      work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] = PLIST_BATTLE_CreateBattleParam( work , 
                  &work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A] , 10 , 0 , 2 );
      break;

    case PL_COMM_MULTI:   //�����E�GA�E�GB
      work->battleParty[PL_COMM_PLAYER_TYPE_ALLY] = PLIST_BATTLE_CreateBattleParam( work , 
                  &work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ALLY] , 0 , 0 , 0 );
      work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] = PLIST_BATTLE_CreateBattleParam( work , 
                  &work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A] , 11 , 0 , 0 );
      work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_B] = PLIST_BATTLE_CreateBattleParam( work , 
                  &work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_B] , 22 , 0 , 0 );
      break;
    }
  }
  work->befSelectedNum = work->plData->comm_selected_num;
  //���񋭐��X�V�̂���0�ɂ��Ă���
  work->befTimeLimit = 0;
  
  if(  work->plData->use_tile_limit == TRUE  )
  {
    PLIST_BATTLE_InitTimeLimit( work );
  }
  
  work->barPalletAnmCnt = 0;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void PLIST_BATTLE_TermBattle( PLIST_WORK *work )
{
  if(  work->plData->use_tile_limit == TRUE  )
  {
    PLIST_BATTLE_TermTimeLimit( work );
  }
  if( work->plData->is_disp_party == TRUE )
  {
    switch( work->plData->comm_type )
    {
    case PL_COMM_SINGLE:  //�GA�̂�
      PLIST_BATTLE_DeleteBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] );
      break;

    case PL_COMM_MULTI:   //�����E�GA�E�GB
      PLIST_BATTLE_DeleteBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ALLY] );
      PLIST_BATTLE_DeleteBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] );
      PLIST_BATTLE_DeleteBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_B] );
      break;
    }
  }

  PLIST_BATTLE_TermResource( work );
  PLIST_BATTLE_TermCell( work );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PLIST_BATTLE_UpdateBattle( PLIST_WORK *work )
{
  if( work->plData->is_disp_party == TRUE )
  {
    switch( work->plData->comm_type )
    {
    case PL_COMM_SINGLE:  //�GA�̂�
      PLIST_BATTLE_UpdateBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] );
      break;

    case PL_COMM_MULTI:   //�����E�GA�E�GB
      PLIST_BATTLE_UpdateBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ALLY] );
      PLIST_BATTLE_UpdateBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_A] );
      PLIST_BATTLE_UpdateBattleParam( work , work->battleParty[PL_COMM_PLAYER_TYPE_ENEMY_B] );
      break;
    }
  }
  
  //�ҋ@�l�����b�Z�[�W�̍X�V
  if( work->befSelectedNum != work->plData->comm_selected_num &&
      work->mainSeq == PSMS_SELECT_POKE )
  {
    PLIST_BATTLE_DispWaitingMessage( work );
    work->befSelectedNum = work->plData->comm_selected_num;
  }
  if( work->plData->use_tile_limit == TRUE )
  {
    PLIST_BATTLE_UpdateTimeLimit( work );
  }
  if( work->plData->isNetErr == TRUE )
  {
    PLIST_ForceExit_Timeup( work );
  }

#if PM_DEBUG
  if( DEBUG_FLG_GetFlg( DEBUG_FLG_ListTimeUp ) == TRUE )
  {
    PLIST_ForceExit_Timeup( work );
  }
#endif
}

#pragma mark graphic
//--------------------------------------------------------------
//	cell�̐ݒ�
//--------------------------------------------------------------
static void PLIST_BATTLE_InitCell( PLIST_WORK *work )
{
  const u32 cellNum = PL_COMM_PLAYER_TYPE_MAX * PLIST_LIST_MAX * 2;
  work->cellUnitBattle = GFL_CLACT_UNIT_Create( cellNum , PLIST_CELLUNIT_PRI_MAIN, work->heapId );
  
}

//--------------------------------------------------------------
//	cell�J��
//--------------------------------------------------------------
static void PLIST_BATTLE_TermCell( PLIST_WORK *work )
{
  GFL_CLACT_UNIT_Delete( work->cellUnitBattle );
}

//--------------------------------------------------------------
//	���\�[�X�ǂݍ���
//--------------------------------------------------------------
static void PLIST_BATTLE_InitResource( PLIST_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKELIST , work->heapId );

  GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_pokelist_gra_list_battle_sub_NCLR , 
                    PALTYPE_SUB_BG , PLIST_BG_SUB_PLT_PLATE*16*2 , PLIST_BG_SUB_PLT_PLATE*16*2 , 
                    16*2 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_pokelist_gra_hp_bar_NCLR , 
                    PALTYPE_SUB_BG , PLIST_BG_SUB_PLT_HP_BAR*16*2 , 16*2 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_pokelist_gra_list_battle_sub_NCGR ,
                    PLIST_BG_SUB_BATTLE_WIN , 0 , 0, FALSE , work->heapId );

  if( work->plData->is_disp_party == TRUE )
  {
    switch( work->plData->comm_type )
    {
    case PL_COMM_SINGLE:  //�GA�̂�
      GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_list_battle_sub_s_NSCR , 
                        PLIST_BG_SUB_BATTLE_WIN ,  0 , 0, FALSE , work->heapId );
      break;
    case PL_COMM_MULTI:   //�����E�GA�E�GB
      GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_pokelist_gra_list_battle_sub_m_NSCR , 
                        PLIST_BG_SUB_BATTLE_WIN ,  0 , 0, FALSE , work->heapId );
      break;
    }
  }

  GFL_ARC_CloseDataHandle(arcHandle);
  
  //���ʑf��
  {
    ARCHANDLE *arcHandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );

    //�A�C�R���p�A�C�e��
    work->cellRes[PCR_PLT_ITEM_ICON_SUB] = GFL_CLGRP_PLTT_Register( arcHandle , 
          APP_COMMON_GetPokeItemIconPltArcIdx() , CLSYS_DRAW_SUB , 
          PLIST_OBJPLT_ITEM_ICON*32 , work->heapId  );
    work->cellRes[PCR_NCG_ITEM_ICON_SUB] = GFL_CLGRP_CGR_Register( arcHandle , 
          APP_COMMON_GetPokeItemIconCharArcIdx() , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellRes[PCR_ANM_ITEM_ICON_SUB] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
          APP_COMMON_GetPokeItemIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetPokeItemIconAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    
    //�������ԃo�[
    if( work->plData->use_tile_limit == TRUE )
    {
      //���ʃo�[  
      GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , APP_COMMON_GetBarPltArcIdx() , 
                        PALTYPE_SUB_BG , PLIST_BG_SUB_PLT_BAR*32 , 32 , work->heapId );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , APP_COMMON_GetBarCharArcIdx() ,
                    PLIST_BG_SUB_BATTLE_BAR , 0 , 0, FALSE , work->heapId );
      GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , APP_COMMON_GetBarScrnArcIdx() , 
                        PLIST_BG_SUB_BATTLE_BAR ,  0 , 0, FALSE , work->heapId );
      GFL_BG_ChangeScreenPalette( PLIST_BG_SUB_BATTLE_BAR , 0,0,32,32,PLIST_BG_SUB_PLT_BAR );
      GFL_BG_LoadScreenV_Req( PLIST_BG_SUB_BATTLE_BAR );
    }
    
    GFL_ARC_CloseDataHandle(arcHandleCommon);
  }
  //�|�P�A�C�R���p���\�[�X
  //�L�����N�^�͊e����
  {
    ARCHANDLE *arcHandlePoke = GFL_ARC_OpenDataHandle( ARCID_POKEICON , work->heapId );
    work->cellRes[PCR_PLT_POKEICON_SUB] = GFL_CLGRP_PLTT_RegisterComp( arcHandlePoke , 
          POKEICON_GetPalArcIndex() , CLSYS_DRAW_SUB , 
          PLIST_OBJPLT_POKEICON*32 , work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandlePoke);
  }
  //�A�j���p�ɁA�v���[�g�̑I��F�̃p���b�g��ޔ�
  GFL_STD_MemCopy16( (void*)(HW_BG_PLTT+0x400+PLIST_BG_SUB_PLT_BAR*32) , work->barPalletAnm , 16*2 );
}

//--------------------------------------------------------------
//	���\�[�X�J��
//--------------------------------------------------------------
static void PLIST_BATTLE_TermResource( PLIST_WORK *work )
{
  //Cell�n���\�[�X�͊O�ł܂Ƃ߂ĊJ��
}

#pragma mark [>battle param
//--------------------------------------------------------------
//	�o�g���p�\����(1�p�[�e�B�[��)
//--------------------------------------------------------------
static PLIST_BATTLE_PARAM_WORK* PLIST_BATTLE_CreateBattleParam( PLIST_WORK *work , 
                        PL_COMM_BATTLE_PARAM *initParam , u8 charX, u8 charY , u8 space )
{
  PLIST_BATTLE_PARAM_WORK* param = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_BATTLE_PARAM_WORK) );

  {
    u8 i;
    const u8 partyNum = PokeParty_GetPokeCount( initParam->pp );
    for( i=0;i<PLIST_LIST_MAX;i++ )
    {
      if( i<partyNum )
      {
        const u8 iconX = charX + (i%2) * (PLIST_BATTLE_ICON_WIDTH + space ) + PLIST_BATTLE_ICON_LEFT;
        const u8 iconY = charY + (i/2) * PLIST_BATTLE_ICON_HEIGHT + PLIST_BATTLE_ICON_TOP;
        //4�s�N�Z���߂�
        const u8 ofsX = ( (i%2) == 0 ? 4 : 0 );
        POKEMON_PARAM *pp = PokeParty_GetMemberPointer( initParam->pp , i );
        param->icon[i] = PLIST_BATTLE_CreateBattleParamIcon( work , pp , iconX*8+ofsX , iconY*8 );
      }
      else
      {
        param->icon[i] = NULL;
      }
    }
  }
  //�g���[�i�[��
  {
    STRBUF *srcStr;
    param->nameStr = GFL_BMPWIN_Create( PLIST_BG_SUB_BATTLE_STR , charX+1+(space/2) , charY + 1 , 
          10 , 2 , PLIST_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
    
    srcStr = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME , work->heapId ); 
    GFL_STR_SetStringCode( srcStr , initParam->name );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( param->nameStr ) , 
            0 , 0 , srcStr , work->fontHandle , PLIST_FONT_BATTLE_PARAM );
    GFL_STR_DeleteBuffer( srcStr );
    param->isUpdateStr = TRUE;
  }
  return param;
}

//--------------------------------------------------------------
//	�o�g���p�\����(1�p�[�e�B�[��)
//--------------------------------------------------------------
static void PLIST_BATTLE_DeleteBattleParam( PLIST_WORK *work , PLIST_BATTLE_PARAM_WORK *param )
{
  u8 i;
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    if( param->icon[i] != NULL )
    {
      PLIST_BATTLE_DeleteBattleParamIcon( work , param->icon[i] );
    }
  }
  GFL_BMPWIN_Delete( param->nameStr );
  GFL_HEAP_FreeMemory( param );
}


//--------------------------------------------------------------
//	�o�g���p�\����(1�p�[�e�B�[��)
//--------------------------------------------------------------
static void PLIST_BATTLE_UpdateBattleParam( PLIST_WORK *work , PLIST_BATTLE_PARAM_WORK *param )
{
  u8 i;
  for( i=0;i<PLIST_LIST_MAX;i++ )
  {
    if( param->icon[i] != NULL )
    {
      PLIST_BATTLE_UpdateBattleParamIcon( work , param->icon[i] );
    }
  }
  
  if( param->isUpdateStr == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( param->nameStr )) == FALSE )
    {
      param->isUpdateStr = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( param->nameStr );
    }
    
  }
}

//--------------------------------------------------------------
//	�A�C�R���̍쐬
//--------------------------------------------------------------
static PLIST_BATTLE_POKE_ICON* PLIST_BATTLE_CreateBattleParamIcon( PLIST_WORK *work , POKEMON_PARAM *pp , const u8 posX , const u8 posY )
{
  PLIST_BATTLE_POKE_ICON *iconWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_BATTLE_POKE_ICON) );
  const u32 isEgg = PP_Get( pp , ID_PARA_tamago_flag , NULL );
  //�|�P�A�C�R��
  {
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKEICON , work->heapId );
    const POKEMON_PASO_PARAM *ppp = PP_GetPPPPointerConst( pp );
    GFL_CLWK_DATA cellInitData;
    u8 pltNum;

    iconWork->pokeIconRes = GFL_CLGRP_CGR_Register( arcHandle , 
        POKEICON_GetCgxArcIndex(ppp) , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandle);

    pltNum = POKEICON_GetPalNumGetByPPP( ppp );
    cellInitData.pos_x = posX+16;
    cellInitData.pos_y = posY+8;
    cellInitData.anmseq = POKEICON_ANM_HPMAX;
    cellInitData.softpri = 16;
    cellInitData.bgpri = 2;
    iconWork->pokeIcon = GFL_CLACT_WK_Create( work->cellUnitBattle ,
              iconWork->pokeIconRes,
              work->cellRes[PCR_PLT_POKEICON_SUB],
              work->cellRes[PCR_ANM_POKEICON],
              &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
    GFL_CLACT_WK_SetPlttOffs( iconWork->pokeIcon , pltNum , CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetAutoAnmFlag( iconWork->pokeIcon , TRUE );
  }
  //�A�C�e���A�C�R��
  {
    const u32 itemId = PP_Get( pp , ID_PARA_item , NULL );
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = posX+32;
    cellInitData.pos_y = posY+16;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 8;
    cellInitData.bgpri = 2;
    iconWork->itemIcon = GFL_CLACT_WK_Create( work->cellUnitBattle ,
              work->cellRes[PCR_NCG_ITEM_ICON_SUB],
              work->cellRes[PCR_PLT_ITEM_ICON_SUB],
              work->cellRes[PCR_ANM_ITEM_ICON_SUB],
              &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
    if( itemId != 0 )
    {
      GFL_CLACT_WK_SetDrawEnable( iconWork->itemIcon , TRUE );
      if( ITEM_CheckMail( itemId ) == TRUE )
      {
        GFL_CLACT_WK_SetAnmSeq( iconWork->itemIcon , 1 );
      }
      else
      {
        GFL_CLACT_WK_SetAnmSeq( iconWork->itemIcon , 0 );
      }
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( iconWork->itemIcon , FALSE );
    }
  }
  //���ʃ}�[�N
  {
    const u32 sex = PP_Get( pp , ID_PARA_sex , NULL );
    const u8 charX = posX/8;
    const u8 charY = posY/8;
    const u8 modX = posX%8;

    //HP�o�[�̐F���g���I
    iconWork->sexStrWin = GFL_BMPWIN_Create( PLIST_BG_SUB_BATTLE_STR , charX + 3 , charY, 
          3 , 2 , PLIST_BG_SUB_PLT_HP_BAR , GFL_BMP_CHRAREA_GET_B );
    if( isEgg == FALSE )
    {
      if( sex == PTL_SEX_MALE )
      {
        const PRINTSYS_LSB fontColBlue = PRINTSYS_LSB_Make( PLIST_FONT_PARAM_LETTER_BLUE , PLIST_FONT_PARAM_SHADOW_BLUE , 0 );
        PLIST_UTIL_DrawStrFunc( work , iconWork->sexStrWin , mes_pokelist_01_28 ,
                        modX+4 , 0 , fontColBlue );
      }
      else if( sex == PTL_SEX_FEMALE )
      {
        const PRINTSYS_LSB fontColRed = PRINTSYS_LSB_Make( PLIST_FONT_PARAM_LETTER_RED , PLIST_FONT_PARAM_SHADOW_RED , 0 );
        PLIST_UTIL_DrawStrFunc( work , iconWork->sexStrWin , mes_pokelist_01_29 ,
                        modX+4 , 0 , fontColRed );
      }
    }
  }
  //���x��
  {
    const u32 lv = PP_CalcLevel( pp );
    const u8 charX = posX/8;
    const u8 charY = posY/8;
    const u8 modX = posX%8;

    iconWork->lvStrWin = GFL_BMPWIN_Create( PLIST_BG_SUB_BATTLE_STR , charX , charY + 3 , 
          5 , 1 , PLIST_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

    if( isEgg == FALSE )
    {
      WORDSET *wordSet = WORDSET_Create( work->heapId );
      WORDSET_RegisterNumber( wordSet , 0 , lv , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );

      PLIST_UTIL_DrawValueStrFuncSys( work , iconWork->lvStrWin , 
                        wordSet , mes_pokelist_01_03 , 
                        modX , 0 , PLIST_FONT_BATTLE_PARAM );

      WORDSET_Delete( wordSet );
    }
  }
  iconWork->isUpdateStr = TRUE;
  
  return iconWork;
}

//--------------------------------------------------------------
//	�A�C�R���̊J��
//--------------------------------------------------------------
static void PLIST_BATTLE_DeleteBattleParamIcon( PLIST_WORK *work , PLIST_BATTLE_POKE_ICON *iconWork )
{
  GFL_BMPWIN_Delete( iconWork->sexStrWin );
  GFL_BMPWIN_Delete( iconWork->lvStrWin );
  GFL_CLACT_WK_Remove( iconWork->itemIcon );
  GFL_CLACT_WK_Remove( iconWork->pokeIcon );
  GFL_CLGRP_CGR_Release( iconWork->pokeIconRes );
  GFL_HEAP_FreeMemory( iconWork );
  
}

//--------------------------------------------------------------
//	�A�C�R���̍X�V
//--------------------------------------------------------------
static void PLIST_BATTLE_UpdateBattleParamIcon( PLIST_WORK *work , PLIST_BATTLE_POKE_ICON *iconWork )
{
  if( iconWork->isUpdateStr == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( iconWork->sexStrWin )) == FALSE &&
        PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( iconWork->lvStrWin )) == FALSE )
    {
      iconWork->isUpdateStr = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( iconWork->sexStrWin );
      GFL_BMPWIN_MakeTransWindow_VBlank( iconWork->lvStrWin );
    }
  }
}

#pragma mark [>waiting message
//--------------------------------------------------------------
//	�ҋ@���b�Z�[�W�̕\��
//--------------------------------------------------------------
void PLIST_BATTLE_OpenWaitingMessage( PLIST_WORK *work )
{
  PLIST_BATTLE_DispWaitingMessage( work );

  work->befSelectedNum = work->plData->comm_selected_num;
}

static void PLIST_BATTLE_DispWaitingMessage( PLIST_WORK *work )
{
  if( work->plData->comm_selected_num == 0 )
  {
    //�N�����߂ĂȂ�
    //�\���͖�����OK
  }
  else
  {
    if( PLIST_MSG_IsOpenWindow( work , work->msgWork ) == FALSE )
    {
      PLIST_MSG_OpenWindow( work , work->msgWork , PMT_BAR_BATTLE );
    }
    
    if( work->plData->comm_type == PL_COMM_SINGLE )
    {
      //�����͑ҋ@��
      STRBUF *srcStr;
      srcStr = GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME , work->heapId ); 
      GFL_STR_SetStringCode( srcStr , work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].name );
      PLIST_MSG_CreateWordSet( work , work->msgWork );
      PLIST_MSG_AddWordSet_Word( work , work->msgWork , 0 , srcStr , 
                  work->plData->comm_battle[PL_COMM_PLAYER_TYPE_ENEMY_A].sex );

      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_13_01 );

      PLIST_MSG_DeleteWordSet( work , work->msgWork );
      GFL_STR_DeleteBuffer( srcStr );
    }
    else
    {
      //�~�~�ɂ�@���������イ(�A���r�A�����ł͂Ȃ��̂Ń��b�Z�[�W����
      PLIST_MSG_DrawMessageNoWait( work , work->msgWork , mes_pokelist_13_02+(work->plData->comm_selected_num-1) );
    }
  }
}

#pragma mark [>time limit
//--------------------------------------------------------------
//	�������ԏ�����
//--------------------------------------------------------------
static void PLIST_BATTLE_InitTimeLimit( PLIST_WORK *work )
{
  work->timeLimitStr = GFL_BMPWIN_Create( PLIST_BG_SUB_BATTLE_STR , 
              PLIST_BATTLE_TIME_STR_LEFT , PLIST_BATTLE_TIME_STR_TOP , 
              PLIST_BATTLE_TIME_STR_WIDTH , PLIST_BATTLE_TIME_STR_HEIGHT ,
              PLIST_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  work->timeLimitNumStr = GFL_BMPWIN_Create( PLIST_BG_SUB_BATTLE_STR , 
              PLIST_BATTLE_TIME_NUM_LEFT , PLIST_BATTLE_TIME_NUM_TOP , 
              PLIST_BATTLE_TIME_NUM_WIDTH , PLIST_BATTLE_TIME_NUM_HEIGHT ,
              PLIST_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  {
    STRBUF *str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_13_05 ); 

    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->timeLimitStr ) , 
            0 , 4 , str , work->fontHandle , PLIST_FONT_BATTLE_PARAM );

    GFL_STR_DeleteBuffer( str );
    work->isUpdateLimitStr = TRUE;
  }

}

//--------------------------------------------------------------
//	�������ԊJ��
//--------------------------------------------------------------
static void PLIST_BATTLE_TermTimeLimit( PLIST_WORK *work )
{
  GFL_BMPWIN_Delete( work->timeLimitStr );
  GFL_BMPWIN_Delete( work->timeLimitNumStr );
}

//--------------------------------------------------------------
//	�������ԍX�V
//--------------------------------------------------------------
static void PLIST_BATTLE_UpdateTimeLimit( PLIST_WORK *work )
{
  if( work->befTimeLimit != work->plData->time_limit )
  {
    const u8 min = work->plData->time_limit/60;
    const u8 sec = work->plData->time_limit%60;
    STRBUF *str;
    STRBUF *workStr = GFL_STR_CreateBuffer( 32 , work->heapId );
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    u32 strLen;
    if( min == 0 )
    {
      str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_13_06_01 ); 
      //�b�����Ȃ�X�y�[�X�l��
      WORDSET_RegisterNumber( wordSet , 1 , sec , 2 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    }
    else
    {
      str = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_13_06 ); 
      WORDSET_RegisterNumber( wordSet , 0 , min , 2 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
      WORDSET_RegisterNumber( wordSet , 1 , sec , 2 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
    }
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->timeLimitNumStr ) , 0 );

    WORDSET_ExpandStr( wordSet , workStr , str );

    strLen = PRINTSYS_GetStrWidth(workStr,work->fontHandle,0);
    OS_TFPrintf(3,"Len[%d]\n",strLen);
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( work->timeLimitNumStr ) , 
            0+(36-strLen) , 4 , workStr , work->fontHandle , PLIST_FONT_BATTLE_PARAM );

    WORDSET_Delete( wordSet );
    GFL_STR_DeleteBuffer( workStr );
    GFL_STR_DeleteBuffer( str );
    work->isUpdateLimitNum = TRUE;

    work->befTimeLimit = work->plData->time_limit;
  }
  if( work->plData->time_limit == 0 )
  {
    PLIST_ForceExit_Timeup( work );
  }
  
  if( work->isUpdateLimitNum == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( work->timeLimitNumStr )) == FALSE )
    {
      work->isUpdateLimitNum = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( work->timeLimitNumStr );
    }
  }

  if( work->isUpdateLimitStr == TRUE )
  {
    if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( work->timeLimitStr )) == FALSE )
    {
      work->isUpdateLimitStr = FALSE;
      GFL_BMPWIN_MakeTransWindow_VBlank( work->timeLimitStr );
    }
  }
  
  if( work->plData->time_limit <= 10 )
  {
    //�p���b�g�A�j��
    u8 i;
    const fx32 cos = FX_CosIdx( work->barPalletAnmCnt*0x10000/PLIST_BATTLE_PAL_ANM_MAX );
    const u8 addVal = (cos+FX32_ONE) * 32 / (FX32_ONE*2);
    
    for( i=0;i<16;i++ )
    {
      u8 r = (work->barPalletAnm[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      u8 g = (work->barPalletAnm[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      u8 b = (work->barPalletAnm[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      r = (r+addVal > 31?31:r+addVal);
      work->barPalletTrans[i] = GX_RGB(r,g,b);
    }
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_SUB ,
                                        PLIST_BG_SUB_PLT_BAR * 32 ,
                                        work->barPalletTrans , 2*16 );
    

    work->barPalletAnmCnt++;
    if( work->barPalletAnmCnt >= PLIST_BATTLE_PAL_ANM_MAX )
    {
      work->barPalletAnmCnt = 0;
    }
  }
}

