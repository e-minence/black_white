//======================================================================
/**
 * @file	p_sta_info.c
 * @brief	�|�P�����X�e�[�^�X ���y�[�W
 * @author	ariizumi
 * @data	09/07/06
 *
 * ���W���[�����FPSTATUS_INFO
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "print/global_msg.h"
#include "poke_tool/poke_memo.h"
#include "app/app_menu_common.h"
#include "field/zonedata.h"

#include "arc_def.h"
#include "message.naix"
#include "p_status_gra.naix"

#include "msg/msg_trainermemo.h"

#include "p_sta_sys.h"
#include "p_sta_info.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

//�����p���W
#define PSTATUS_INFO_STR_ZUKAN_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_ZUKAN_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_ZUKAN_VAL_X (64+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_ZUKAN_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_NAME_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NAME_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_NAME_VAL_X (64+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NAME_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_TYPE_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_TYPE_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_PARENT_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_PARENT_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_PARENT_VAL_X (64+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_PARENT_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_ID_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_ID_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_ID_VAL_X (64+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_ID_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_NOWEXP_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NOWEXP_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_NOWEXP_VAL_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NOWEXP_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

#define PSTATUS_INFO_STR_NEXTEXP_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NEXTEXP_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_NEXTEXP_ATO_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NEXTEXP_ATO_Y ( 0+ PSTATUS_STR_OFS_Y)
#define PSTATUS_INFO_STR_NEXTEXP_VAL_X (32+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_STR_NEXTEXP_VAL_Y ( 0+ PSTATUS_STR_OFS_Y)

//�^�C�v�A�C�R��
#define PSTATUS_INFO_TYPEICON_1_X  (96)
#define PSTATUS_INFO_TYPEICON_1_Y  (48)
#define PSTATUS_INFO_TYPEICON_2_X  (96+32)
#define PSTATUS_INFO_TYPEICON_2_Y  (48)

//�o���l�o�[
#define PSTATUS_INFO_EXPBAR_TOP  (3)
#define PSTATUS_INFO_EXPBAR_LEFT (0)
#define PSTATUS_INFO_EXPBAR_HEIGHT  (3)
#define PSTATUS_INFO_EXPBAR_WIDTH (64)

#define PSTATUS_INFO_EXPBAR_COL (5) //�t�H���g�̐�

//����
#define PSTATUS_INFO_MEMO_WIN_TOP  (5)
#define PSTATUS_INFO_MEMO_WIN_LEFT (4)
#define PSTATUS_INFO_MEMO_WIN_HEIGHT  (20)
#define PSTATUS_INFO_MEMO_WIN_WIDTH (24)

#define PSTATUS_INFO_MEMO_STR_X ( 0+ PSTATUS_STR_OFS_X)
#define PSTATUS_INFO_MEMO_STR_Y ( 0+ PSTATUS_STR_OFS_Y)


//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
enum STATUS_INFO_BMPWIN
{
  SIB_ZUKAN,
  SIB_NAME,
  SIB_TYPE,
  SIB_PARENT,
  SIB_ID,
  SIB_NOWEXP,
  SIB_NOWEXP_NUM,
  SIB_NEXTEXP,
  SIB_NEXTEXP_NUM,
  SIB_EXP_BAR,
  
  SIB_MAX,
};


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PSTATUS_INFO_WORK
{
  BOOL isUpdateStr;
  BOOL isDisp;

  u16 *localZukanNoList;

  GFL_BMPWIN  *bmpWin[SIB_MAX];
  GFL_BMPWIN  *bmpWinUp;

  GFL_MSGDATA *msgMemo;
  GFL_MSGDATA *msgPlace;
  GFL_MSGDATA *msgPlaceSp;
  GFL_MSGDATA *msgPlaceOut;
  GFL_MSGDATA *msgPlacePerson;

  NNSG2dScreenData *scrDataDown;
  void *scrResDown;
  NNSG2dScreenData *scrDataDownEgg;
  void *scrResDownEgg;
  NNSG2dScreenData *scrDataUp;
  void *scrResUp;
  NNSG2dScreenData *scrDataUpTitle;
  void *scrResUpTitle;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PSTATUS_INFO_DrawState( PSTATUS_WORK *work , PSTATUS_INFO_WORK *subWork , const POKEMON_PASO_PARAM *ppp );
static void PSTATUS_INFO_DrawStateUp( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , const POKEMON_PASO_PARAM *ppp );
static STRBUF* PSTATUS_INFO_GetPlaceStr( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , const u32 place );

static const u8 winPos[SIB_MAX][4] =
{
  {  2,  1, 16,  2},  //�}��
  {  2,  3, 16,  2},  //���O
  {  2,  5,  8,  2},  //�^�C�v
  {  2,  7, 16,  2},  //�e
  {  2,  9, 16,  2},  //ID
  {  2, 11, 17,  2},  //���݂̌o���n
  { 10, 13,  8,  2},  //�����l
  {  2, 15, 16,  2},  //���܂ł̌o���n
  {  6, 17, 12,  2},  //�����l
  { 10, 19,  8,  1},  //�o���n�o�[
};

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
PSTATUS_INFO_WORK* PSTATUS_INFO_Init( PSTATUS_WORK *work )
{
  u32 size;
  PSTATUS_INFO_WORK* infoWork;
  
  infoWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_INFO_WORK) );
  infoWork->isUpdateStr = FALSE;
  infoWork->isDisp = FALSE;
  infoWork->localZukanNoList = POKE_PERSONAL_GetZenkokuToChihouArray( work->heapId, NULL );

//  {
//    int i;
//    for( i=0;i<=MONSNO_END;i++ )
//    {
//      OS_TPrintf("[%3d->%3d]\n",i,infoWork->localZukanNoList[i]);
//    }
//  }

  return infoWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void PSTATUS_INFO_Term( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  GFL_HEAP_FreeMemory( infoWork->localZukanNoList );
  GFL_HEAP_FreeMemory( infoWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PSTATUS_INFO_Main( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{

}

#pragma mark [>Resource
//--------------------------------------------------------------
//	���\�[�X�ǂݍ���
//--------------------------------------------------------------
void PSTATUS_INFO_LoadResource( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , ARCHANDLE *archandle )
{
  //���������p�X�N���[���ǂݍ���
  infoWork->scrResDown = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_info_d_NSCR ,
                    FALSE , &infoWork->scrDataDown , work->heapId );
  infoWork->scrResDownEgg = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_skill_d_NSCR ,
                    FALSE , &infoWork->scrDataDownEgg , work->heapId );
  infoWork->scrResUp = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_info_u_NSCR ,
                    FALSE , &infoWork->scrDataUp , work->heapId );
  infoWork->scrResUpTitle = GFL_ARCHDL_UTIL_LoadScreen( archandle , NARC_p_status_gra_p_st_infotitle_u_NSCR ,
                    FALSE , &infoWork->scrDataUpTitle , work->heapId );

  //�g���[�i�[����
  infoWork->msgMemo = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_trainermemo_dat , work->heapId );
  infoWork->msgPlace   = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_place_name_dat , work->heapId );
  infoWork->msgPlaceSp = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_place_name_spe_dat , work->heapId );
  infoWork->msgPlaceOut= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_place_name_out_dat , work->heapId );
  infoWork->msgPlacePerson = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_place_name_per_dat , work->heapId );

}

//--------------------------------------------------------------
//	���\�[�X�J��
//--------------------------------------------------------------
void PSTATUS_INFO_ReleaseResource( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  //�����ŕ`�敨�̊J�������Ă���
  if( infoWork->isDisp == TRUE )
  {
    PSTATUS_INFO_ClearPage( work , infoWork );
  }

  GFL_MSG_Delete( infoWork->msgMemo );
  GFL_MSG_Delete( infoWork->msgPlace );
  GFL_MSG_Delete( infoWork->msgPlaceSp );
  GFL_MSG_Delete( infoWork->msgPlaceOut );
  GFL_MSG_Delete( infoWork->msgPlacePerson );


  GFL_HEAP_FreeMemory( infoWork->scrResDown );
  GFL_HEAP_FreeMemory( infoWork->scrResDownEgg );
  GFL_HEAP_FreeMemory( infoWork->scrResUp );
  GFL_HEAP_FreeMemory( infoWork->scrResUpTitle );
}

#pragma mark [>Disp
//--------------------------------------------------------------
//	�y�[�W�̕\��
//--------------------------------------------------------------
void PSTATUS_INFO_DispPage( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  u8 i;
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );

  for( i=0;i<SIB_MAX;i++ )
  {
    infoWork->bmpWin[i] = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
                winPos[i][0] , winPos[i][1] , winPos[i][2] , winPos[i][3] ,
                PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  }
  infoWork->bmpWinUp = GFL_BMPWIN_Create( PSTATUS_BG_SUB_STR ,
              PSTATUS_INFO_MEMO_WIN_LEFT , PSTATUS_INFO_MEMO_WIN_TOP ,
              PSTATUS_INFO_MEMO_WIN_WIDTH , PSTATUS_INFO_MEMO_WIN_HEIGHT ,
              PSTATUS_BG_SUB_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  PSTATUS_INFO_DrawState( work , infoWork , ppp );
  PSTATUS_INFO_DrawStateUp( work , infoWork , ppp );

  infoWork->isDisp = TRUE;
}


//--------------------------------------------------------------
//	�y�[�W�̕\��(�]���^�C�~���O
//--------------------------------------------------------------
void PSTATUS_INFO_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  u8 i;
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );

  //Window���n�̒��ւ�
  if( work->isEgg == FALSE )
  {
    GFL_BG_WriteScreenExpand( PSTATUS_BG_PLATE , 
                      0 , 0 , 32 , 24 ,
                      infoWork->scrDataDown->rawData ,
                      0 , 0 , 32 , 32 );
  }
  else
  {
    GFL_BG_WriteScreenExpand( PSTATUS_BG_PLATE , 
                      0 , 0 , 32 , 24 ,
                      infoWork->scrDataDownEgg->rawData ,
                      0 , 0 , 32 , 32 );
  }
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_PLATE );

  GFL_BG_LoadScreenBuffer( PSTATUS_BG_SUB_PLATE, 
                     infoWork->scrDataUp->rawData, 
                     infoWork->scrDataUp->szByte );

  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_PLATE );

  //���ʃ^�C�g��
  GFL_BG_WriteScreenExpand( PSTATUS_BG_SUB_TITLE , 
                    0 , 0 , 32 , PSTATUS_SUB_TITLE_HEIGHT ,
                    infoWork->scrDataUpTitle->rawData ,
                    0 , 0 , 32 , 32 );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_TITLE );

  for( i=0;i<SIB_MAX;i++ )
  {
    GFL_BMPWIN_MakeTransWindow( infoWork->bmpWin[i] );
  }
  GFL_BMPWIN_MakeTransWindow( infoWork->bmpWinUp );

  //�^�C�v�A�C�R��
  if( work->isEgg == FALSE )
  {
    const u32 type1 = PPP_Get( ppp, ID_PARA_type1, NULL );
    const u32 type2 = PPP_Get( ppp, ID_PARA_type2, NULL );
    GFL_CLACTPOS cellPos;
    {
      NNSG2dImageProxy imageProxy;
      GFL_CLGRP_CGR_GetProxy( work->cellResTypeNcg[type1] , &imageProxy );
      GFL_CLACT_WK_SetImgProxy( work->clwkTypeIcon[0] , &imageProxy );
      GFL_CLACT_WK_SetPlttOffs( work->clwkTypeIcon[0] , 
                                APP_COMMON_GetPokeTypePltOffset(type1) , 
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      cellPos.x = PSTATUS_INFO_TYPEICON_1_X;
      cellPos.y = PSTATUS_INFO_TYPEICON_1_Y;
      GFL_CLACT_WK_SetPos( work->clwkTypeIcon[0] , &cellPos , CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , TRUE );
      GFL_CLACT_WK_SetBgPri( work->clwkTypeIcon[0] , 1 );
    }
    
    if( type1 != type2 )
    {
      NNSG2dImageProxy imageProxy;
      GFL_CLGRP_CGR_GetProxy( work->cellResTypeNcg[type2] , &imageProxy );
      GFL_CLACT_WK_SetImgProxy( work->clwkTypeIcon[1] , &imageProxy );
      GFL_CLACT_WK_SetPlttOffs( work->clwkTypeIcon[1] , 
                                APP_COMMON_GetPokeTypePltOffset(type2) , 
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      cellPos.x = PSTATUS_INFO_TYPEICON_2_X;
      cellPos.y = PSTATUS_INFO_TYPEICON_2_Y;
      GFL_CLACT_WK_SetPos( work->clwkTypeIcon[1] , &cellPos , CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , TRUE );
      GFL_CLACT_WK_SetBgPri( work->clwkTypeIcon[1] , 1 );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );
    }
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );
  }
  
  //�o�[�A�C�R������
  GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE1] , SBA_PAGE1_SELECT );
}
//--------------------------------------------------------------
//	�y�[�W�̃N���A
//--------------------------------------------------------------
void PSTATUS_INFO_ClearPage( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  u8 i;
  for( i=0;i<SIB_MAX;i++ )
  {
    GFL_BMPWIN_Delete( infoWork->bmpWin[i] );
  }
  GFL_BMPWIN_Delete( infoWork->bmpWinUp );
  
  infoWork->isDisp = FALSE;
}
//--------------------------------------------------------------
//	�y�[�W�̃N���A(�]���^�C�~���O
//--------------------------------------------------------------
void PSTATUS_INFO_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork )
{
  u8 i;
  GFL_BG_FillScreen( PSTATUS_BG_PARAM , 0 , 0 , 0 , 
                     PSTATUS_MAIN_PAGE_WIDTH , 21 ,
                     GFL_BG_SCRWRT_PALNL );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_PARAM );

  GFL_BG_ClearScreenCodeVReq( PSTATUS_BG_SUB_STR , 0 );
  GFL_BG_LoadScreenV_Req( PSTATUS_BG_SUB_STR );

  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[0] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[1] , FALSE );

  //�o�[�A�C�R������
  GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE1] , SBA_PAGE1_NORMAL );
}

//--------------------------------------------------------------
//	�����̕`��
//--------------------------------------------------------------
static void PSTATUS_INFO_DrawState( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , const POKEMON_PASO_PARAM *ppp )
{
  if( work->isEgg == FALSE )
  {
    //�}��
    PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_ZUKAN] , mes_status_02_02 ,
                              PSTATUS_INFO_STR_ZUKAN_X , PSTATUS_INFO_STR_ZUKAN_Y , PSTATUS_STR_COL_TITLE );
    {
      WORDSET *wordSet = WORDSET_Create( work->heapId );
      u32 no = PPP_Get( ppp , ID_PARA_monsno , NULL );
      u16 col = PSTATUS_STR_COL_VALUE;
      if( work->psData->zukan_mode == FALSE )
      {
        no = infoWork->localZukanNoList[no];
      }
      
      if( PPP_CheckRare( ppp ) == TRUE )
      {
        col = PSTATUS_STR_COL_RED;
      }

      if( no == POKEPER_CHIHOU_NO_NONE )
      {
        PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_ZUKAN] , wordSet , mes_status_02_16 , 
                                       PSTATUS_INFO_STR_ZUKAN_VAL_X , PSTATUS_INFO_STR_ZUKAN_VAL_Y , col );
      }
      else
      {
        WORDSET_RegisterNumber( wordSet , 0 , no , 3 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
        PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_ZUKAN] , wordSet , mes_status_02_03 , 
                                       PSTATUS_INFO_STR_ZUKAN_VAL_X , PSTATUS_INFO_STR_ZUKAN_VAL_Y , col );
      }
      WORDSET_Delete( wordSet );
    }

    //���O
    PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_NAME] , mes_status_02_04 ,
                              PSTATUS_INFO_STR_NAME_X , PSTATUS_INFO_STR_NAME_Y , PSTATUS_STR_COL_TITLE );
    {
      STRBUF *srcStr;
      if( work->isEgg == FALSE )
      {
        u32 no = PPP_Get( ppp , ID_PARA_monsno , NULL );
        srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName , no ); 
      }
      else
      {
        srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName , MONSNO_TAMAGO ); 
      }
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( infoWork->bmpWin[SIB_NAME] ) , 
              PSTATUS_INFO_STR_NAME_VAL_X , PSTATUS_INFO_STR_NAME_VAL_Y , 
              srcStr , work->fontHandle , PSTATUS_STR_COL_VALUE );
      GFL_STR_DeleteBuffer( srcStr );
    }

    //�^�C�v
    PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_TYPE] , mes_status_02_06 ,
                              PSTATUS_INFO_STR_TYPE_X , PSTATUS_INFO_STR_TYPE_Y , PSTATUS_STR_COL_TITLE );

    //�e
    PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_PARENT] , mes_status_02_07 ,
                              PSTATUS_INFO_STR_PARENT_X , PSTATUS_INFO_STR_PARENT_Y , PSTATUS_STR_COL_TITLE );
    {
      STRBUF *parentName  = GFL_STR_CreateBuffer( 32, work->heapId );
      WORDSET *wordSet = WORDSET_Create( work->heapId );
      const u32 sex = PPP_Get( ppp , ID_PARA_oyasex , parentName );
      PRINTSYS_LSB col = PSTATUS_STR_COL_VALUE;
      if( sex == PTL_SEX_MALE )
      {
        col = PSTATUS_STR_COL_BLUE;
      }
      else if( sex == PTL_SEX_FEMALE )
      {
        col = PSTATUS_STR_COL_RED;
      }

      PPP_Get( ppp , ID_PARA_oyaname , parentName );
      WORDSET_RegisterWord( wordSet , 0 , parentName , 0,TRUE,PM_LANG );
      PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_PARENT] , wordSet , mes_status_02_08 , 
                                     PSTATUS_INFO_STR_PARENT_VAL_X , PSTATUS_INFO_STR_PARENT_VAL_Y , col );
      WORDSET_Delete( wordSet );
      GFL_STR_DeleteBuffer( parentName );
    }
    
    //ID
    PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_ID] , mes_status_02_09 ,
                              PSTATUS_INFO_STR_ID_X , PSTATUS_INFO_STR_ID_Y , PSTATUS_STR_COL_TITLE );
    {
      WORDSET *wordSet = WORDSET_Create( work->heapId );
      u32 id = PPP_Get( ppp , ID_PARA_id_no , NULL )&0x0000FFFF;
      WORDSET_RegisterNumber( wordSet , 0 , id , 5 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
      PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_ID] , wordSet , mes_status_02_10 , 
                                     PSTATUS_INFO_STR_ID_VAL_X , PSTATUS_INFO_STR_ID_VAL_Y , PSTATUS_STR_COL_VALUE );
      WORDSET_Delete( wordSet );
    }

    //�o���l
    {
      const u32 lv = PPP_Get( ppp, ID_PARA_level, NULL );
      const u32 exp = PPP_Get( ppp , ID_PARA_exp , NULL );
      const u32 nextLvExp = POKETOOL_GetMinExp(PPP_Get( ppp, ID_PARA_monsno, NULL ),
                                      PPP_Get( ppp, ID_PARA_form_no, NULL ),
                                      lv+1) ;
      const u32 nowLvExp = POKETOOL_GetMinExp(PPP_Get( ppp, ID_PARA_monsno, NULL ),
                                      PPP_Get( ppp, ID_PARA_form_no, NULL ),
                                      lv) ;
      u32 minExp = nextLvExp - exp;
      if( lv == 100 )
      {
        minExp = 0;
      }
      //���݂̌o���l
      PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_NOWEXP] , mes_status_02_11 ,
                                PSTATUS_INFO_STR_NOWEXP_X , PSTATUS_INFO_STR_NOWEXP_Y , PSTATUS_STR_COL_TITLE );
      {
        WORDSET *wordSet = WORDSET_Create( work->heapId );
        WORDSET_RegisterNumber( wordSet , 0 , exp , 7 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
        PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_NOWEXP_NUM] , wordSet , mes_status_02_12 , 
                                       PSTATUS_INFO_STR_NOWEXP_VAL_X , PSTATUS_INFO_STR_NOWEXP_VAL_Y , PSTATUS_STR_COL_VALUE );
        WORDSET_Delete( wordSet );
      }

      //���܂ł̌o���l
      PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_NEXTEXP] , mes_status_02_13 ,
                                PSTATUS_INFO_STR_NEXTEXP_X , PSTATUS_INFO_STR_NEXTEXP_Y , PSTATUS_STR_COL_TITLE );
      PSTATUS_UTIL_DrawStrFunc( work , infoWork->bmpWin[SIB_NEXTEXP_NUM] , mes_status_02_14 ,
                                PSTATUS_INFO_STR_NEXTEXP_ATO_X , PSTATUS_INFO_STR_NEXTEXP_ATO_Y , PSTATUS_STR_COL_VALUE );
      {
        WORDSET *wordSet = WORDSET_Create( work->heapId );
        WORDSET_RegisterNumber( wordSet , 0 , minExp , 6 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
        PSTATUS_UTIL_DrawValueStrFunc( work , infoWork->bmpWin[SIB_NEXTEXP_NUM] , wordSet , mes_status_02_15 , 
                                       PSTATUS_INFO_STR_NEXTEXP_VAL_X , PSTATUS_INFO_STR_NEXTEXP_VAL_Y , PSTATUS_STR_COL_VALUE );
        WORDSET_Delete( wordSet );
      }
      
      //�o���l�o�[
      if( lv != 100 )
      {
        const u32 modLvExp = (nextLvExp-nowLvExp);
        u32 len = PSTATUS_INFO_EXPBAR_WIDTH * (modLvExp-minExp) / modLvExp;
        if( (modLvExp-minExp) != 0 && len == 0 )
        {
          len = 1;
        }
        if( len == PSTATUS_INFO_EXPBAR_WIDTH )
        {
          len--;
        }
        GFL_BMP_Fill( GFL_BMPWIN_GetBmp(infoWork->bmpWin[SIB_EXP_BAR]) , 
                      PSTATUS_INFO_EXPBAR_LEFT , PSTATUS_INFO_EXPBAR_TOP ,
                      len , PSTATUS_INFO_EXPBAR_HEIGHT , PSTATUS_INFO_EXPBAR_COL );

      }
    }
  }
  else
  {
    
  }


  infoWork->isUpdateStr = TRUE;
}

//--------------------------------------------------------------
//	�����̕`��
//--------------------------------------------------------------
static void PSTATUS_INFO_DrawStateUp( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , const POKEMON_PASO_PARAM *ppp )
{
  u32 height = 0;
  BOOL isParent;  //�����̃|�P���H
  u32 isEgg = PPP_Get( ppp , ID_PARA_tamago_flag , NULL );
  u32 seikaku = PPP_Get( ppp , ID_PARA_seikaku , NULL );
  u32 natuki = PPP_Get( ppp , ID_PARA_friend , NULL );
  u32 year1   = PPP_Get( ppp , ID_PARA_get_year , NULL );
  u32 month1  = PPP_Get( ppp , ID_PARA_get_month , NULL );
  u32 day1    = PPP_Get( ppp , ID_PARA_get_day , NULL );
  u32 year2   = PPP_Get( ppp , ID_PARA_birth_year , NULL );
  u32 month2  = PPP_Get( ppp , ID_PARA_birth_month , NULL );
  u32 day2    = PPP_Get( ppp , ID_PARA_birth_day , NULL );
  u32 level   = PPP_Get( ppp , ID_PARA_get_level , NULL );
  u32 isEvent = PPP_Get( ppp , ID_PARA_event_get_flag , NULL );
  u32 rom = PPP_Get( ppp , ID_PARA_get_cassette , NULL );
  const u32 perRand = PPP_Get( ppp , ID_PARA_personal_rnd , NULL );
  //�ꏊ�̓|�P�V�t�^�[�Ή��ŏ���������\������B
  u32 place1  = PPP_Get( ppp , ID_PARA_get_place , NULL );
  u32 place2  = PPP_Get( ppp , ID_PARA_birth_place , NULL );
  //�����̃|�P���`�F�b�N
  {
    MYSTATUS *myStatus = GAMEDATA_GetMyStatus( work->psData->game_data );
    isParent = PPP_IsMatchOya( ppp , myStatus );
  }
  

#if USE_STATUS_DEBUG
  if( work->isDevMemo == TRUE )
  {
    year1  =work->year1;
    month1 =work->month1;
    day1   = work->day1;
    place1 = work->place1;
    year2  = work->year2;
    month2 = work->month2;
    day2   = work->day2;
    place2 = work->place2;
    level = work->level;
    rom = work->devRom;
    natuki = work->natsuki;
    isEgg = work->isDevEgg;
    isParent = work->isDevParent;
    isEvent = work->isDevEvent;
  }
#endif

  //���i
  if( isEgg == 0 )
  {
    STRBUF *srcStr = GFL_MSG_CreateString( infoWork->msgMemo , trmemo_01_01 + seikaku );
    
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( infoWork->bmpWinUp ) , 
            PSTATUS_INFO_MEMO_STR_X , PSTATUS_INFO_MEMO_STR_Y + height, 
            srcStr , work->fontHandle , PSTATUS_STR_COL_VALUE );
    GFL_STR_DeleteBuffer( srcStr );
    
    height = 16;
  }
  
  //����
  {
    u16 memoId;
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 256 , work->heapId );
    STRBUF *placeStr1;
    STRBUF *placeStr2;
    WORDSET *wordSet  = WORDSET_CreateEx( 10 , WORDSET_DEFAULT_BUFLEN , work->heapId );

    //��ޕ���

    if( isEgg == 0 )
    {
      if( place2 == 30001 )
      {
        //�|�P�V�t�^�[
        u16 newRomMsg = trmemo_02_09_01;
        u16 oldRomMsg = trmemo_02_10_01;
        if( isEvent == TRUE )
        {
          newRomMsg = trmemo_02_11_01;
          oldRomMsg = trmemo_02_12_01;
        }

        switch( rom )
        {
        case VERSION_SAPPHIRE:  //1	///<	�o�[�W�����FAGB�T�t�@�C�A
        case VERSION_RUBY:		  //2	///<	�o�[�W�����FAGB���r�[
        case VERSION_EMERALD:	  //3	///<	�o�[�W�����FAGB�G�������h
          place2 = POKE_MEMO_PLACE_HOUEN;
          memoId = oldRomMsg;
          break;
          
        case VERSION_RED:			  //4	///<	�o�[�W�����FAGB�t�@�C�A�[���b�h
        case VERSION_GREEN:		  //5	///<	�o�[�W�����FAGB���[�t�O���[��
          place2 = POKE_MEMO_PLACE_KANTOU;
          memoId = oldRomMsg;
          break;

        case VERSION_GOLD:		  //7	///<	�o�[�W�����F�S�[���h�p�\���`
        case VERSION_SILVER:	  //8	///<	�o�[�W�����F�V���o�[�p�\���`
          place2 = POKE_MEMO_PLACE_JYOUTO;
          memoId = newRomMsg;
          break;

        case VERSION_DIAMOND:	  //10	///<	�o�[�W�����FDS�_�C�������h
        case VERSION_PEARL:		  //11	///<	�o�[�W�����FDS�p�[��
        case VERSION_PLATINUM:  //12	///<	�o�[�W�����FDS�v���`�i
          place2 = POKE_MEMO_PLACE_SHINOU;
          memoId = newRomMsg;
          break;

        case VERSION_COLOSSEUM: //15	///<	�o�[�W�����FGC�R���V�A��
          place2 = POKE_MEMO_PLACE_FAR_PLACE;
          memoId = oldRomMsg;
          break;

        default:
          place1 = POKE_MEMO_PLACE_UNKNOWN;
          memoId = newRomMsg;
          break;
          
        }
      }
      else if( place2 >= POKE_MEMO_PLACE_SEREBIXI_BEFORE && place2 <= POKE_MEMO_PLACE_ENRAISUI_AFTER )
      {
        //2010�f��
        switch( place2 )
        {
        case POKE_MEMO_PLACE_SEREBIXI_BEFORE:
          memoId = trmemo_02_13_01;
          break;
        case POKE_MEMO_PLACE_SEREBIXI_AFTER:
          memoId = trmemo_02_14_01;
          break;
        case POKE_MEMO_PLACE_ENRAISUI_BEFORE:
          memoId = trmemo_02_15_01;
          break;
        case POKE_MEMO_PLACE_ENRAISUI_AFTER:
          memoId = trmemo_02_16_01;
          break;
        }
      }
      else
      {
        //���̑�
        if( isEvent == FALSE )
        {
          if( place1 == 0 )
          {
            if( place2 == POKE_MEMO_PLACE_GAME_TRADE )
            {
              //�Q�[��������
              memoId = trmemo_02_02_01;
            }
            else
            {
              //�ʏ�ߊl
              memoId = trmemo_02_01_01;
            }
          }
          else
          {
            if( place1 <= 60000 )
            {
              //�����^�}�S�z��
              memoId = trmemo_02_03_01;
            }
            else
            {
              //�Q�[�����^�}�S�z��
              memoId = trmemo_02_04_01;
            }
          }
        }
        else
        {
          //�C�x���g�z�z
          if( place1 == 0 )
          {
            //�s�v�c�ȑ��蕨�o�R
            memoId = trmemo_02_05_01;
          }
          else
          {
            //���z�z
            memoId = trmemo_02_06_01;
          }
        }
      }
      
      
      if( isParent == FALSE &&
          !(place2 >= POKE_MEMO_PLACE_SEREBIXI_BEFORE && 
            place2 <= POKE_MEMO_PLACE_ENRAISUI_AFTER) )
      {
        //�^�}�S����Ȃ���΁A���l�̃|�P�̎���Msg��+1�̈ʒu�ɂ���B
        memoId++;
      }
    }
    else
    {
      //�^�}�S
      if( isEvent == FALSE )
      {
        if( isParent == TRUE )
        {
          memoId = trmemo_01_T_01_01;
        }
        else
        {
          memoId = trmemo_01_T_01_02;
        }
      }
      else
      {
        if( isParent == TRUE )
        {
          memoId = trmemo_01_T_01_03;
        }
        else
        {
          memoId = trmemo_01_T_01_04;
        }
      }
    }

    
    //�ꏊ
    placeStr1 = PSTATUS_INFO_GetPlaceStr( work , infoWork , place1 );
    placeStr2 = PSTATUS_INFO_GetPlaceStr( work , infoWork , place2 );
    
    WORDSET_RegisterNumber( wordSet , 0 , year2 , 2 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wordSet , 1 , month2 , 2 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wordSet , 2 , day2 , 2 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wordSet , 3 , level , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterWord(   wordSet , 4 , placeStr2 , 0,TRUE,PM_LANG );
    WORDSET_RegisterNumber( wordSet , 5 , year1 , 2 , STR_NUM_DISP_ZERO , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wordSet , 6 , month1 , 2 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterNumber( wordSet , 7 , day1 , 2 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    WORDSET_RegisterWord(   wordSet , 8 , placeStr1 , 0,TRUE,PM_LANG );

    srcStr = GFL_MSG_CreateString( infoWork->msgMemo , memoId ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( infoWork->bmpWinUp ) , 
            PSTATUS_INFO_MEMO_STR_X , PSTATUS_INFO_MEMO_STR_Y + height, 
            dstStr , work->fontHandle , PSTATUS_STR_COL_VALUE );

    WORDSET_Delete( wordSet );

    //�������̊֐�
    height += PRINTSYS_GetLineCount( dstStr )*16;
    
    GFL_STR_DeleteBuffer( placeStr1 );
    GFL_STR_DeleteBuffer( placeStr2 );
    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
  }

  //�����Q
  if( isEgg == 0 )
  {
    u8 i;
    u8 maxIdx = 0;
    u8 maxRand = 0;
    u32 msgId;
    STRBUF *srcStr;
    const int paraId[6] = 
    {
      ID_PARA_hp_rnd,               //HP����
      ID_PARA_pow_rnd,              //�U���͗���
      ID_PARA_def_rnd,              //�h��͗���
      ID_PARA_agi_rnd,              //�f��������
      ID_PARA_spepow_rnd,             //���U����
      ID_PARA_spedef_rnd,             //���h����
    };
    //�������������ꍇ�̗D�揇�ʗp(�O����`�F�b�N���Ă�����"���傫��"�Ń`�F�b�N
    const u8 priArrIdx = perRand%6;
    const u8 paraPriorityArr[6][6] =
    {
      { 0,1,2,3,4,5 },
      { 1,2,3,4,5,0 },
      { 2,3,4,5,0,1 },
      { 3,4,5,0,1,2 },
      { 4,5,0,1,2,3 },
      { 5,0,1,2,3,4 },
    };
    
    for( i=0;i<6;i++ )
    {
      const u32 rand = PPP_Get( ppp , paraId[ paraPriorityArr[priArrIdx][i] ] , NULL );
      if( maxRand < rand )
      {
        maxRand = rand;
        maxIdx = i;
      }
    }
    
    msgId = trmemo_03_01_00 + maxIdx*5 + perRand%5;
    
    srcStr = GFL_MSG_CreateString( infoWork->msgMemo , msgId );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( infoWork->bmpWinUp ) , 
            PSTATUS_INFO_MEMO_STR_X , PSTATUS_INFO_MEMO_STR_Y + height, 
            srcStr , work->fontHandle , PSTATUS_STR_COL_VALUE );
    GFL_STR_DeleteBuffer( srcStr );
    
  }
  else
  {
    //�^�}�S
    u16 msgId;
    STRBUF *srcStr;
    if( natuki <= 5 )
    {
      msgId = trmemo_01_T_02_01;
    }
    else
    if( natuki <= 10 )
    {
      msgId = trmemo_01_T_02_02;
    }
    else
    if( natuki <= 40 )
    {
      msgId = trmemo_01_T_02_03;
    }
    else
    {
      msgId = trmemo_01_T_02_04;
    }

    srcStr = GFL_MSG_CreateString( infoWork->msgMemo , msgId );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( infoWork->bmpWinUp ) , 
            PSTATUS_INFO_MEMO_STR_X , PSTATUS_INFO_MEMO_STR_Y + height, 
            srcStr , work->fontHandle , PSTATUS_STR_COL_VALUE );
    GFL_STR_DeleteBuffer( srcStr );
  }
}


//�n���ϊ�
static STRBUF* PSTATUS_INFO_GetPlaceStr( PSTATUS_WORK *work , PSTATUS_INFO_WORK *infoWork , const u32 place )
{
  const ARCDATID datId = POKE_PLACE_GetMessageDatId( place );
  const u32 msgId = POKE_PLACE_GetMessageId( place );
  
  switch( datId )
  {
  case NARC_message_place_name_dat:
    return GFL_MSG_CreateString( infoWork->msgPlace , msgId ); 
    break;
  case NARC_message_place_name_spe_dat:
    return GFL_MSG_CreateString( infoWork->msgPlaceSp , msgId ); 
    break;
  case NARC_message_place_name_out_dat:
    return GFL_MSG_CreateString( infoWork->msgPlaceOut , msgId ); 
    break;
  case NARC_message_place_name_per_dat:
    return GFL_MSG_CreateString( infoWork->msgPlacePerson , msgId ); 
    break;
  }
  GF_ASSERT(0);
  return GFL_MSG_CreateString( infoWork->msgPlace , 0 ); 
  
}
