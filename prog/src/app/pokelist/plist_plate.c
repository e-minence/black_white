//======================================================================
/**
 * @file	plist_plate.c
 * @brief	�|�P�������X�g �v���[�g
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPLIST_PLATE
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "msg/msg_pokelist.h"

#include "pokeicon/pokeicon.h"
#include "print/wordset.h"

#include "plist_plate.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PLIST_PLATE_WIDTH  (16)
#define PLIST_PLATE_HEIGHT (6)

//�e�\�����̈ʒu
//�v���[�g�̍��ォ��̑��΍��W
#define PLIST_PLATE_POKE_POS_X (32)
#define PLIST_PLATE_POKE_POS_Y (16)
#define PLIST_PLATE_BALL_POS_X (16)
#define PLIST_PLATE_BALL_POS_Y (16)
#define PLIST_PLATE_HPBASE_POS_X (88)
#define PLIST_PLATE_HPBASE_POS_Y (28)

//�����̈ʒu
#define PLIST_PLATE_NUM_BASAE_Y (34)  //
#define PLIST_PLATE_STR_NAME_X (52)
#define PLIST_PLATE_STR_NAME_Y ( 8)
#define PLIST_PLATE_STR_LEVEL_X (16)
#define PLIST_PLATE_STR_LEVEL_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_HPMAX_X (96)
#define PLIST_PLATE_STR_HPMAX_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_HP_X (64)
#define PLIST_PLATE_STR_HP_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_SLASH_X (88)
#define PLIST_PLATE_STR_SLASH_Y (PLIST_PLATE_NUM_BASAE_Y)

#define PLIST_PLATE_CELLNUM (6)
#define PLIST_RENDER_MAIN (0)
//======================================================================
//	enum
//======================================================================
#pragma mark [> enum


//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _PLIST_PLATE_WORK
{
  POKEMON_PARAM *pp;
  u8 idx;
  BOOL isBlank;
  BOOL isUpdateStr;

  GFL_BMPWIN *bmpWin;

  //Obj�n  
  GFL_CLSYS_REND *cellRender;
  GFL_CLUNIT  *cellUnit;
  u32      pokeIconNcgRes;

  GFL_CLWK *pokeIcon;
  GFL_CLWK *ballIcon;
  
};
//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//�v���[�g�̍���ʒu(�L�����P��)
static const u8 PLIST_PLATE_POS_ARR[6][2] =
{
  {0, 1},{16, 2},
  {0, 7},{16, 8},
  {0,13},{16,14},
};

static void PLIST_PLATE_CreateCell( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
static void PLIST_PLATE_DrawStr( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );

//--------------------------------------------------------------
//	�v���[�g�쐬
//--------------------------------------------------------------
PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate( PLIST_WORK *work , const u8 idx , POKEMON_PARAM *pp )
{
  const u8 baseX = PLIST_PLATE_POS_ARR[idx][0];
  const u8 baseY = PLIST_PLATE_POS_ARR[idx][1];
  PLIST_PLATE_WORK *plateWork;
  static const u8 PLATE_SCR_POS_ARR[2][2] ={{0,1},{16,2}};
  plateWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_PLATE_WORK) );
  plateWork->idx = idx;
  plateWork->pp = pp;
  plateWork->isBlank = FALSE;
  plateWork->isUpdateStr = FALSE;
  
  //BG�n
  GFL_BG_WriteScreenExpand( PLIST_BG_PLATE , 
                      baseX ,baseY ,
                      PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT ,
                      &work->plateScrData->rawData , 
                      PLATE_SCR_POS_ARR[(idx==0?0:1)][0] , PLATE_SCR_POS_ARR[(idx==0?0:1)][1] ,
                      32 , 32 );  //���O���t�B�b�N�f�[�^�̃T�C�Y
  PLIST_PLATE_ChangeColor( work , plateWork , PPC_NORMAL );
  
  //Obj�n
  {
    //���j�b�g�ƃ����_�[�����ꂼ����
    GFL_REND_SURFACE_INIT renderInit;
    renderInit.lefttop_x = 0;
    renderInit.lefttop_y = 0;
    renderInit.width  = 256;
    renderInit.height = 192;
    renderInit.type = CLSYS_DRAW_MAIN;
    plateWork->cellRender = GFL_CLACT_USERREND_Create( &renderInit , 1 , work->heapId );
    plateWork->cellUnit = GFL_CLACT_UNIT_Create( PLIST_PLATE_CELLNUM , 0 , work->heapId );
    GFL_CLACT_UNIT_SetUserRend( plateWork->cellUnit , plateWork->cellRender );
  }
  //�Z���쐬
  PLIST_PLATE_CreateCell( work , plateWork );
  {
    //�ݒ肪�I������烌���_���𓮂���
    GFL_CLACTPOS surfacePos;
    surfacePos.x = -baseX*8;
    surfacePos.y = -baseY*8;
    GFL_CLACT_USERREND_SetSurfacePos( plateWork->cellRender , PLIST_RENDER_MAIN , &surfacePos );
  }
  
  plateWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_PARAM , baseX , baseY , 
          PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PLIST_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  
  PLIST_PLATE_DrawStr( work , plateWork );
  
  return plateWork;
}

//--------------------------------------------------------------
//	�v���[�g�쐬(��
//--------------------------------------------------------------
PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate_Blank( PLIST_WORK *work , const u8 idx )
{
  PLIST_PLATE_WORK *plateWork;
  static const u8 PLATE_SCR_POS_ARR[2][2] ={{0,1},{16,2}};
  plateWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_PLATE_WORK) );
  
  //BG�n
  GFL_BG_WriteScreenExpand( PLIST_BG_PLATE , 
                      PLIST_PLATE_POS_ARR[idx][0] ,PLIST_PLATE_POS_ARR[idx][1] ,
                      PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT ,
                      &work->plateScrData->rawData , 0 , 19 , 32 , 32 ); 
  
  plateWork->idx = idx;
  plateWork->pp = NULL;
  plateWork->isBlank = TRUE;
  GFL_BG_LoadScreenV_Req( PLIST_BG_PLATE );

  return plateWork;
}

//--------------------------------------------------------------
//	�v���[�g�폜
//--------------------------------------------------------------
void PLIST_PLATE_DeletePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  if( plateWork->isBlank == FALSE )
  {
    GFL_BMPWIN_Delete( plateWork->bmpWin );
    
    GFL_CLACT_WK_Remove( plateWork->pokeIcon );
    GFL_CLACT_USERREND_Delete( plateWork->cellRender );
    GFL_CLACT_UNIT_Delete( plateWork->cellUnit );
  }
  GFL_HEAP_FreeMemory( plateWork );
}

//--------------------------------------------------------------
//	�v���[�g�X�V
//--------------------------------------------------------------
void PLIST_PLATE_UpdatePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  if( plateWork->isBlank == FALSE )
  {
    if( plateWork->isUpdateStr == TRUE )
    {
      if( PRINTSYS_QUE_IsExistTarget( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin )) == FALSE )
      {
        plateWork->isUpdateStr = FALSE;
        GFL_BMPWIN_MakeTransWindow_VBlank( plateWork->bmpWin );
      }
    }
  }
}

//--------------------------------------------------------------
//	�Z���쐬
//--------------------------------------------------------------
static void PLIST_PLATE_CreateCell( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  
  //�|�P�A�C�R��
  {
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_POKEICON , work->heapId );
    const POKEMON_PARAM *pokePara = PokeParty_GetMemberPointer( work->plData->pp , plateWork->idx );
    const POKEMON_PASO_PARAM *ppp = PP_GetPPPPointerConst( pokePara );
    GFL_CLWK_DATA cellInitData;
    u8 pltNum;

    plateWork->pokeIconNcgRes = GFL_CLGRP_CGR_Register( arcHandle , 
        POKEICON_GetCgxArcIndex(ppp) , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    
    GFL_ARC_CloseDataHandle(arcHandle);

    pltNum = POKEICON_GetPalNumGetByPPP( ppp );
    cellInitData.pos_x = PLIST_PLATE_POKE_POS_X;
    cellInitData.pos_y = PLIST_PLATE_POKE_POS_Y;
    cellInitData.anmseq = POKEICON_ANM_HPMAX;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    plateWork->pokeIcon = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              plateWork->pokeIconNcgRes,
              work->cellRes[PCR_PLT_POKEICON],
              work->cellRes[PCR_ANM_POKEICON],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
    GFL_CLACT_WK_SetPlttOffs( plateWork->pokeIcon , pltNum , CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetAutoAnmFlag( plateWork->pokeIcon , TRUE );
  }
  //�{�[���A�C�R��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_PLATE_BALL_POS_X;
    cellInitData.pos_y = PLIST_PLATE_BALL_POS_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 1;
    cellInitData.bgpri = 2;
    plateWork->ballIcon = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              work->cellRes[PCR_NCG_BALL],
              work->cellRes[PCR_PLT_OBJ_COMMON],
              work->cellRes[PCR_ANM_BALL],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
  }
  
  //HP�y��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_PLATE_HPBASE_POS_X;
    cellInitData.pos_y = PLIST_PLATE_HPBASE_POS_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    plateWork->ballIcon = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              work->cellRes[PCR_NCG_HP_BASE],
              work->cellRes[PCR_PLT_HP_BASE],
              work->cellRes[PCR_ANM_HP_BASE],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
    GFL_CLACT_WK_SetAutoAnmFlag( plateWork->ballIcon , TRUE );
  }
    
}

//--------------------------------------------------------------
//	������`��
//--------------------------------------------------------------
static void PLIST_PLATE_DrawStr( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  const PRINTSYS_LSB fontCol = PRINTSYS_LSB_Make( PLIST_FONT_PARAM_LETTER , PLIST_FONT_PARAM_SHADOW , 0 );
  //���O
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 32, work->heapId );

    WORDSET_RegisterPokeNickName( wordSet , 0 , plateWork->pp );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_09 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_NAME_X , PLIST_PLATE_STR_NAME_Y , dstStr , work->fontHandle , fontCol);
    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }

  //���x��
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 lv = PP_CalcLevel( plateWork->pp );
    WORDSET_RegisterNumber( wordSet , 0 , lv , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_03 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_LEVEL_X , PLIST_PLATE_STR_LEVEL_Y , dstStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  
  //HP�ő�
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 hpMax = PP_Get( plateWork->pp , ID_PARA_hpmax , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , hpMax , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_15 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_HPMAX_X , PLIST_PLATE_STR_HPMAX_Y , dstStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  //HP����
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 hp = PP_Get( plateWork->pp , ID_PARA_hp , NULL );
    WORDSET_RegisterNumber( wordSet , 0 , hp , 3 , STR_NUM_DISP_SPACE , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_21 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_HP_X , PLIST_PLATE_STR_HP_Y , dstStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  //HP�̃X���b�V��
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;

    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_27 ); 
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_SLASH_X , PLIST_PLATE_STR_SLASH_Y , srcStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    WORDSET_Delete( wordSet );
  }
  
  
  
  plateWork->isUpdateStr = TRUE;
  
}

//--------------------------------------------------------------
//	�F�ς�
//--------------------------------------------------------------
void PLIST_PLATE_ChangeColor( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , PLIST_PLATE_COLTYPE col )
{
  GFL_BG_ChangeScreenPalette( PLIST_BG_PLATE , 
              PLIST_PLATE_POS_ARR[plateWork->idx][0] ,PLIST_PLATE_POS_ARR[plateWork->idx][1] ,
              PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , col );

  GFL_BG_LoadScreenV_Req( PLIST_BG_PLATE );
}
