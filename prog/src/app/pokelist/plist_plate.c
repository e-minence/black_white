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
#define PLIST_PLATE_POKE_POS_X (24)
#define PLIST_PLATE_POKE_POS_Y (16)
#define PLIST_PLATE_BALL_POS_X (16)
#define PLIST_PLATE_BALL_POS_Y (16)
#define PLIST_PLATE_HPBASE_POS_X (84)
#define PLIST_PLATE_HPBASE_POS_Y (28)
#define PLIST_PLATE_ITEM_POS_X (PLIST_PLATE_POKE_POS_X+8)
#define PLIST_PLATE_ITEM_POS_Y (PLIST_PLATE_POKE_POS_Y+8)
#define PLIST_PLATE_CONDITION_POS_X (32)
#define PLIST_PLATE_CONDITION_POS_Y (42)

//�����̈ʒu
#define PLIST_PLATE_STR_NAME_X (40)
#define PLIST_PLATE_STR_NAME_Y ( 8)
#define PLIST_PLATE_STR_SEX_X (104)
#define PLIST_PLATE_STR_SEX_Y ( 8)

#define PLIST_PLATE_NUM_BASAE_Y (34)  //Lv��Hp��Y���W
#define PLIST_PLATE_STR_LEVEL_X (16)
#define PLIST_PLATE_STR_LEVEL_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_HP_X (64)
#define PLIST_PLATE_STR_HP_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_SLASH_X (PLIST_PLATE_STR_HP_X+24)
#define PLIST_PLATE_STR_SLASH_Y (PLIST_PLATE_NUM_BASAE_Y)
#define PLIST_PLATE_STR_HPMAX_X (PLIST_PLATE_STR_SLASH_X+8)
#define PLIST_PLATE_STR_HPMAX_Y (PLIST_PLATE_NUM_BASAE_Y)

//HP�o�[�n
#define PLIST_PLATE_HPBAR_LEN (48)
#define PLIST_PLATE_HPBAR_TOP (PLIST_PLATE_HPBASE_POS_Y-2)
#define PLIST_PLATE_HPBAR_LEFT (PLIST_PLATE_HPBASE_POS_X-23)

#define PLIST_PLATE_CELLNUM (6)
#define PLIST_RENDER_MAIN (0)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//��Ԉُ�A�C�R����IDX
enum
{
  POKE_CONICON_POKERUSU,
  POKE_CONICON_MAHI,
  POKE_CONICON_KOORI,
  POKE_CONICON_NEMURI,
  POKE_CONICON_DOKU,
  POKE_CONICON_YAKEDO,
  POKE_CONICON_HINSI,
};

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
  GFL_CLWK *hpBase;
  GFL_CLWK *itemIcon;
  GFL_CLWK *conditionIcon;
  
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

//�v���[�g�̑f�ނ̍��W(0�p��1�`�p
static const u8 PLATE_SCR_POS_ARR[2][2] ={{0,1},{16,2}};

static void PLIST_PLATE_CreateCell( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
static void PLIST_PLATE_CreatePokeIcon( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
static void PLIST_PLATE_DrawParam( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
static void PLIST_PLATE_DrawHPBar( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork );
static const u8 PLIST_PLATE_GetHPRate( PLIST_PLATE_WORK *plateWork );

//--------------------------------------------------------------
//	�v���[�g�쐬
//--------------------------------------------------------------
PLIST_PLATE_WORK* PLIST_PLATE_CreatePlate( PLIST_WORK *work , const u8 idx , POKEMON_PARAM *pp )
{
  const u8 baseX = PLIST_PLATE_POS_ARR[idx][0];
  const u8 baseY = PLIST_PLATE_POS_ARR[idx][1];
  PLIST_PLATE_WORK *plateWork;
  plateWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PLIST_PLATE_WORK) );
  plateWork->idx = idx;
  plateWork->pp = pp;
  plateWork->isBlank = FALSE;
  plateWork->isUpdateStr = FALSE;
  
  //BG�n
  GFL_BG_WriteScreenExpand( PLIST_BG_PLATE , 
                      baseX+PLIST_BG_SCROLL_X_CHAR ,baseY ,
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
    plateWork->cellUnit = GFL_CLACT_UNIT_Create( PLIST_PLATE_CELLNUM , PLIST_CELLUNIT_PRI_PLATE , work->heapId );
    GFL_CLACT_UNIT_SetUserRend( plateWork->cellUnit , plateWork->cellRender );
  }
  //�Z���쐬
  PLIST_PLATE_CreateCell( work , plateWork );
  PLIST_PLATE_CreatePokeIcon( work , plateWork );
  {
    //�ݒ肪�I������烌���_���𓮂���
    GFL_CLACTPOS surfacePos;
    surfacePos.x = -baseX*8;
    surfacePos.y = -baseY*8;
    GFL_CLACT_USERREND_SetSurfacePos( plateWork->cellRender , PLIST_RENDER_MAIN , &surfacePos );
  }
  
  plateWork->bmpWin = GFL_BMPWIN_Create( PLIST_BG_PARAM , baseX+PLIST_BG_SCROLL_X_CHAR , baseY , 
          PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PLIST_BG_PLT_HP_BAR , GFL_BMP_CHRAREA_GET_B );
  
  PLIST_PLATE_DrawParam( work , plateWork );
  PLIST_PLATE_DrawHPBar( work , plateWork );
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
                      PLIST_PLATE_POS_ARR[idx][0]+PLIST_BG_SCROLL_X_CHAR ,PLIST_PLATE_POS_ARR[idx][1] ,
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
    GFL_CLACT_WK_Remove( plateWork->ballIcon );
    GFL_CLACT_WK_Remove( plateWork->hpBase );
    GFL_CLACT_USERREND_Delete( plateWork->cellRender );
    GFL_CLACT_UNIT_Delete( plateWork->cellUnit );

    GFL_CLGRP_CGR_Release( plateWork->pokeIconNcgRes );
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
    GFL_CLACT_WK_SetAutoAnmFlag( plateWork->ballIcon , TRUE );
  }
  
  //HP�y��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_PLATE_HPBASE_POS_X;
    cellInitData.pos_y = PLIST_PLATE_HPBASE_POS_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    plateWork->hpBase = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              work->cellRes[PCR_NCG_HP_BASE],
              work->cellRes[PCR_PLT_HP_BASE],
              work->cellRes[PCR_ANM_HP_BASE],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
  }
  //�A�C�e���A�C�R��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_PLATE_ITEM_POS_X;
    cellInitData.pos_y = PLIST_PLATE_ITEM_POS_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    plateWork->itemIcon = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              work->cellRes[PCR_NCG_ITEM_ICON],
              work->cellRes[PCR_PLT_ITEM_ICON],
              work->cellRes[PCR_ANM_ITEM_ICON],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( plateWork->itemIcon , FALSE );
  }

  //��Ԉُ�A�C�R��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PLIST_PLATE_CONDITION_POS_X;
    cellInitData.pos_y = PLIST_PLATE_CONDITION_POS_Y;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 2;
    plateWork->conditionIcon = GFL_CLACT_WK_Create( plateWork->cellUnit ,
              work->cellRes[PCR_NCG_CONDITION],
              work->cellRes[PCR_PLT_CONDITION],
              work->cellRes[PCR_ANM_CONDITION],
              &cellInitData ,PLIST_RENDER_MAIN , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( plateWork->conditionIcon , FALSE );
  }
}
//--------------------------------------------------------------
//	�|�P�����A�C�R���쐬
//--------------------------------------------------------------
static void PLIST_PLATE_CreatePokeIcon( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
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
}

//--------------------------------------------------------------
//	������`��
//--------------------------------------------------------------
static void PLIST_PLATE_DrawParam( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
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
            PLIST_PLATE_STR_NAME_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_NAME_Y+PLIST_MSG_STR_OFS_Y , 
            dstStr , work->fontHandle , fontCol);
    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  //����
  {
    STRBUF *srcStr;
    u32 sex = PP_Get( plateWork->pp , ID_PARA_sex , NULL );
    if( sex == PTL_SEX_MALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_28 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
              PLIST_PLATE_STR_SEX_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_SEX_Y+PLIST_MSG_STR_OFS_Y ,
              srcStr , work->fontHandle , fontCol );
      GFL_STR_DeleteBuffer( srcStr );
    }
    else if( sex == PTL_SEX_FEMALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_29 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
              PLIST_PLATE_STR_SEX_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_SEX_Y+PLIST_MSG_STR_OFS_Y , 
              srcStr , work->fontHandle , fontCol );
      GFL_STR_DeleteBuffer( srcStr );
    }
  }

  //���x��
  if( PP_GetSick( plateWork->pp ) == POKESICK_NULL &&
      PP_Get( plateWork->pp , ID_PARA_hp , NULL ) != 0 )
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 lv = PP_CalcLevel( plateWork->pp );
    WORDSET_RegisterNumber( wordSet , 0 , lv , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_03 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_LEVEL_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_LEVEL_Y+PLIST_MSG_STR_OFS_Y , 
            dstStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  else
  {
    //���x���̑���ɏ�Ԉُ�A�C�R��
    if( PP_Get( plateWork->pp , ID_PARA_hp , NULL ) == 0 )
    {
      GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_HINSI );
    }
    else
    {
      switch(PP_GetSick( plateWork->pp ))
      {
      case POKESICK_MAHI:   ///< �܂�
        GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_MAHI );
        break;
      case POKESICK_NEMURI: ///< �˂ނ�
        GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_NEMURI );
        break;
      case POKESICK_KOORI:  ///< ������
        GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_KOORI );
        break;
      case POKESICK_YAKEDO: ///< �₯��
        GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_YAKEDO );
        break;
      case POKESICK_DOKU:   ///< �ǂ�
        GFL_CLACT_WK_SetAnmSeq( plateWork->conditionIcon , POKE_CONICON_DOKU );
        break;
      }
    }
    GFL_CLACT_WK_SetDrawEnable( plateWork->conditionIcon , TRUE );
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
            PLIST_PLATE_STR_HPMAX_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_HPMAX_Y+PLIST_MSG_STR_OFS_Y , 
            dstStr , work->sysFontHandle , fontCol );

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
            PLIST_PLATE_STR_HP_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_HP_Y+PLIST_MSG_STR_OFS_Y , 
            dstStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  //HP�̃X���b�V��
  {
    STRBUF *srcStr;

    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_pokelist_01_27 ); 
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 
            PLIST_PLATE_STR_SLASH_X+PLIST_MSG_STR_OFS_X , PLIST_PLATE_STR_SLASH_Y+PLIST_MSG_STR_OFS_Y , 
            srcStr , work->sysFontHandle , fontCol );

    GFL_STR_DeleteBuffer( srcStr );
  }
  
  //�A�C�e���A�C�R��
  {
    u32 itemId = PP_Get( plateWork->pp , ID_PARA_item , NULL );
    //TODO ���[���`�F�b�N
    if( itemId != 0 )
    {
      GFL_CLACT_WK_SetDrawEnable( plateWork->itemIcon , TRUE );
    }
  }
 
  //�|�P�A�C�R���A�j��
  {
    const u8 rate = PLIST_PLATE_GetHPRate( plateWork );
    u16 anmSeq = POKEICON_ANM_HPMAX;
    if( rate == 0 )
    {
      anmSeq = POKEICON_ANM_DEATH;
    }
    else
    if( PP_GetSick( plateWork->pp ) != POKESICK_NULL )
    {
      anmSeq = POKEICON_ANM_STCHG;
    }
    else
    if( rate <= 25 )
    {
      anmSeq = POKEICON_ANM_HPRED;
    }
    else
    if( rate <= 50 )
    {
      anmSeq = POKEICON_ANM_HPYERROW;
    }
    else
    if( rate < 100 )
    {
      anmSeq = POKEICON_ANM_HPGREEN;
    }
    
    GFL_CLACT_WK_SetAnmSeq( plateWork->pokeIcon , anmSeq );
      
  }

  plateWork->isUpdateStr = TRUE;
  
}

//--------------------------------------------------------------
//	HP�o�[�\��
//--------------------------------------------------------------
static void PLIST_PLATE_DrawHPBar( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  const u8 rate = PLIST_PLATE_GetHPRate( plateWork );
  u8 len = PLIST_PLATE_HPBAR_LEN*rate/100;
  u8 inCol,outCol;
  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( plateWork->bmpWin );
  
  //�F����
  if( rate <= 25 )
  {
    inCol  = PLIST_HPBAR_COL_RED_IN;
    outCol = PLIST_HPBAR_COL_RED_OUT;
  }
  else
  if( rate <= 50 )
  {
    inCol  = PLIST_HPBAR_COL_YELLOW_IN;
    outCol = PLIST_HPBAR_COL_YELLOW_OUT;
  }
  else
  {
    inCol  = PLIST_HPBAR_COL_GREEN_IN;
    outCol = PLIST_HPBAR_COL_GREEN_OUT;
  }
  
  GFL_BMP_Fill( bmp , PLIST_PLATE_HPBAR_LEFT , PLIST_PLATE_HPBAR_TOP ,
                len , 1 , outCol );
  GFL_BMP_Fill( bmp , PLIST_PLATE_HPBAR_LEFT , PLIST_PLATE_HPBAR_TOP+1 ,
                len , 2 , inCol );
  GFL_BMP_Fill( bmp , PLIST_PLATE_HPBAR_LEFT , PLIST_PLATE_HPBAR_TOP+3 ,
                len , 1 , outCol );

}

//--------------------------------------------------------------
//	�v���[�g�I����Ԃ�ON�EOFF
//--------------------------------------------------------------
void PLIST_PLATE_SetActivePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const BOOL isActive )
{
  if( isActive == TRUE )
  {
    if( work->mainSeq == PSMS_CHANGE_POKE )
    {
      PLIST_PLATE_ChangeColor( work , plateWork , PPC_CHANGE );
    }
    else
    {
      PLIST_PLATE_ChangeColor( work , plateWork , PPC_NORMAL_SELECT );
    }
    GFL_CLACT_WK_SetAnmSeq( plateWork->ballIcon , 1 );
  }
  else
  {
    //����ւ����ŁA����ւ��^�[�Q�b�g�̂Ƃ��͐F��߂��Ȃ�
    if( !(work->mainSeq == PSMS_CHANGE_POKE &&
          work->changeTarget == plateWork->idx) )
    {
      PLIST_PLATE_ChangeColor( work , plateWork , PPC_NORMAL );
    }
    GFL_CLACT_WK_SetAnmSeq( plateWork->ballIcon , 0 );
  }
}

//--------------------------------------------------------------
//	�F�ς�
//--------------------------------------------------------------
void PLIST_PLATE_ChangeColor( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , PLIST_PLATE_COLTYPE col )
{
  if( col == PPC_NORMAL && 
      PLIST_PLATE_GetHPRate( plateWork ) == 0 )
  {
    GFL_BG_ChangeScreenPalette( PLIST_BG_PLATE , 
                PLIST_PLATE_POS_ARR[plateWork->idx][0]+PLIST_BG_SCROLL_X_CHAR ,
                PLIST_PLATE_POS_ARR[plateWork->idx][1] ,
                PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PPC_DEATH );
  }
  else
  if( col == PPC_NORMAL_SELECT && 
      PLIST_PLATE_GetHPRate( plateWork ) == 0 )
  {
    GFL_BG_ChangeScreenPalette( PLIST_BG_PLATE , 
                PLIST_PLATE_POS_ARR[plateWork->idx][0]+PLIST_BG_SCROLL_X_CHAR ,
                PLIST_PLATE_POS_ARR[plateWork->idx][1] ,
                PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PPC_DEATH_SELECT );
  }
  else
  {
    GFL_BG_ChangeScreenPalette( PLIST_BG_PLATE , 
                PLIST_PLATE_POS_ARR[plateWork->idx][0]+PLIST_BG_SCROLL_X_CHAR ,
                PLIST_PLATE_POS_ARR[plateWork->idx][1] ,
                PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , col );
  }

  GFL_BG_LoadScreenV_Req( PLIST_BG_PLATE );
}

//--------------------------------------------------------------
//����ւ��A�j���Ńv���[�g���ړ�������(�L�����P��
//--------------------------------------------------------------
void PLIST_PLATE_MovePlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const u8 moveValue )
{
  const u8 baseX = PLIST_PLATE_POS_ARR[plateWork->idx][0];
  const u8 baseY = PLIST_PLATE_POS_ARR[plateWork->idx][1];
  
  const int value = (plateWork->idx%2==0 ? -moveValue : moveValue );
  
  GFL_BMPWIN_SetPosX( plateWork->bmpWin , 
            PLIST_PLATE_POS_ARR[plateWork->idx][0] + PLIST_BG_SCROLL_X_CHAR + value );
  
  {
    //�����_���𓮂���
    GFL_CLACTPOS surfacePos;
    surfacePos.x = -(baseX+value)*8;
    surfacePos.y = -baseY*8;
    GFL_CLACT_USERREND_SetSurfacePos( plateWork->cellRender , PLIST_RENDER_MAIN , &surfacePos );
  }
  //BG�n
  GFL_BG_WriteScreenExpand( PLIST_BG_PLATE , 
                      baseX+PLIST_BG_SCROLL_X_CHAR+value ,baseY ,
                      PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT ,
                      &work->plateScrData->rawData , 
                      PLATE_SCR_POS_ARR[(plateWork->idx==0?0:1)][0] , PLATE_SCR_POS_ARR[(plateWork->idx==0?0:1)][1] ,
                      32 , 32 );  //���O���t�B�b�N�f�[�^�̃T�C�Y
  GFL_BG_ChangeScreenPalette( PLIST_BG_PLATE , 
              baseX+PLIST_BG_SCROLL_X_CHAR+value , baseY ,
              PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PPC_CHANGE );

	GFL_BMPWIN_MakeScreen( plateWork->bmpWin );
  GFL_BG_LoadScreenV_Req( PLIST_BG_PARAM );
  GFL_BG_LoadScreenV_Req( PLIST_BG_PLATE );
}

//--------------------------------------------------------------
//�v���[�g���N���A����(����ւ��̈ړ��O�ɍs��
//--------------------------------------------------------------
void PLIST_PLATE_ClearPlate( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , const u8 moveValue )
{
  const u8 baseX = PLIST_PLATE_POS_ARR[plateWork->idx][0];
  const u8 baseY = PLIST_PLATE_POS_ARR[plateWork->idx][1];
  
  const int value = (plateWork->idx%2==0 ? -moveValue : moveValue );

  GFL_BG_FillScreen( PLIST_BG_PLATE , 0 ,
              baseX+PLIST_BG_SCROLL_X_CHAR+value , baseY ,
              PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PPC_CHANGE );
  GFL_BG_FillScreen( PLIST_BG_PARAM , 0 ,
              baseX+PLIST_BG_SCROLL_X_CHAR+value , baseY ,
              PLIST_PLATE_WIDTH , PLIST_PLATE_HEIGHT , PPC_CHANGE );
}

//--------------------------------------------------------------
//�p�����[�^�č쐬
//--------------------------------------------------------------
void PLIST_PLATE_ResetParam( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , POKEMON_PARAM *pp , const u8 moveValue )
{
  const u8 baseX = PLIST_PLATE_POS_ARR[plateWork->idx][0];
  const u8 baseY = PLIST_PLATE_POS_ARR[plateWork->idx][1];
  
  const int value = (plateWork->idx%2==0 ? -moveValue : moveValue );

  plateWork->pp = pp;
  plateWork->isUpdateStr = FALSE;

  GFL_CLACT_WK_Remove( plateWork->pokeIcon );
  GFL_CLGRP_CGR_Release( plateWork->pokeIconNcgRes );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( plateWork->bmpWin ) , 0 );
  GFL_CLACT_WK_SetDrawEnable( plateWork->itemIcon , FALSE );
  GFL_CLACT_WK_SetDrawEnable( plateWork->conditionIcon , FALSE );
  
  {
    //�ݒ肪�I������烌���_�������߂�
    GFL_CLACTPOS surfacePos;
    surfacePos.x = 0;
    surfacePos.y = 0;
    GFL_CLACT_USERREND_SetSurfacePos( plateWork->cellRender , PLIST_RENDER_MAIN , &surfacePos );
  }
  //�Z���쐬
  PLIST_PLATE_CreatePokeIcon( work , plateWork );
  {
    //�ݒ肪�I������烌���_���𓮂���
    GFL_CLACTPOS surfacePos;
    surfacePos.x = -(baseX+value)*8;
    surfacePos.y = -baseY*8;
    GFL_CLACT_USERREND_SetSurfacePos( plateWork->cellRender , PLIST_RENDER_MAIN , &surfacePos );
  }
  
  PLIST_PLATE_DrawParam( work , plateWork );
  PLIST_PLATE_DrawHPBar( work , plateWork );
  
}

//--------------------------------------------------------------
//�I���ł��邩�H
//--------------------------------------------------------------
const BOOL PLIST_PLATE_CanSelect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork )
{
  if( plateWork->isBlank == TRUE )
  {
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
//�v���[�g�̈ʒu�擾(clact�p
//--------------------------------------------------------------
void PLIST_PLATE_GetPlatePosition( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_CLACTPOS *pos )
{
  pos->x = (PLIST_PLATE_POS_ARR[plateWork->idx][0] + (PLIST_PLATE_WIDTH/2))*8;
  pos->y = (PLIST_PLATE_POS_ARR[plateWork->idx][1] + (PLIST_PLATE_HEIGHT/2))*8;
}

//--------------------------------------------------------------
//�v���[�g�̈ʒu�擾(TP����p
//--------------------------------------------------------------
void PLIST_PLATE_GetPlateRect( PLIST_WORK *work , PLIST_PLATE_WORK *plateWork , GFL_UI_TP_HITTBL *hitTbl )
{
  hitTbl->rect.left   = PLIST_PLATE_POS_ARR[plateWork->idx][0]*8;
  hitTbl->rect.top    = PLIST_PLATE_POS_ARR[plateWork->idx][1]*8;
  hitTbl->rect.right  = (PLIST_PLATE_POS_ARR[plateWork->idx][0]+PLIST_PLATE_WIDTH )*8;
  hitTbl->rect.bottom = (PLIST_PLATE_POS_ARR[plateWork->idx][1]+PLIST_PLATE_HEIGHT)*8;
}

//--------------------------------------------------------------
//HP�̊����̌v�Z0�`100
//--------------------------------------------------------------
static const u8 PLIST_PLATE_GetHPRate( PLIST_PLATE_WORK *plateWork )
{
  const u32 hpmax = PP_Get( plateWork->pp , ID_PARA_hpmax , NULL );
  const u32 hp = PP_Get( plateWork->pp , ID_PARA_hp , NULL );
  u8 rate = 100*hp/hpmax;
  
  if( hp != 0 && rate == 0 )
  {
    rate = 1;
  }
  if( hp != hpmax && rate == 100 )
  {
    rate = 99;
  }
  return rate;
}

