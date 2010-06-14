//======================================================================
/**
 * @file	mb_sel_poke.h
 * @brief	�}���`�u�[�g�F�{�b�N�X�E�|�P����
 * @author	ariizumi
 * @data	09/12/03
 *
 * ���W���[�����FMB_SEL_POKE
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "mb_select_gra.naix"
#include "poke_tool/monsno_def.h"

#include "multiboot/mb_poke_icon.h"
#include "multiboot/mb_util.h"
#include "mb_sel_poke.h"

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
struct _MB_SEL_POKE
{
  POKEMON_PASO_PARAM *ppp;
  MB_SEL_POKE_INIT_WORK *initWork;
  
  MB_SEL_POKE_TYPE type;
  u8       idx;
  u32      cellResIdx;
  GFL_CLWK *pokeIcon;
  
  BOOL isMove;
  BOOL isAutoDelete;
  int startX,startY;
  int moveOfsX,moveOfsY;
  u8 endCnt;
  u8 cnt;
  u8 alphaRetCnt; //Alpha��߂��܂ł̎���
  
  BOOL isValid;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
MB_SEL_POKE* MB_SEL_POKE_CreateWork( MB_SELECT_WORK *selWork , MB_SEL_POKE_INIT_WORK *initWork )
{
  GFL_CLWK_DATA cellInitData;
  MB_SEL_POKE* pokeWork = GFL_HEAP_AllocClearMemory( initWork->heapId , sizeof(MB_SEL_POKE) );
  
  pokeWork->initWork = initWork;
  pokeWork->type = initWork->iconType;
  pokeWork->idx  = initWork->idx;

  //���̎��_�ł�ppp�͖���Ƃ��A�_�~�[�Z��������
  pokeWork->cellResIdx = GFL_CLGRP_CGR_Register( initWork->arcHandle , 
                      NARC_mb_select_gra_icon_dummy_NCGR , 
                      FALSE , CLSYS_DRAW_MAIN , initWork->heapId  );

  cellInitData.pos_x = MB_SEL_POKE_GetPosX( pokeWork );
  cellInitData.pos_y = MB_SEL_POKE_GetPosY( pokeWork );
  cellInitData.anmseq = 0;
  cellInitData.softpri = 0;
  if( pokeWork->type == MSPT_BOX )
  {
    cellInitData.bgpri = 2;
  }
  else
  {
    cellInitData.bgpri = 1;
  }

  pokeWork->pokeIcon = GFL_CLACT_WK_Create( initWork->cellUnit ,
            pokeWork->cellResIdx,
            initWork->palResIdx,
            initWork->anmResIdx,
            &cellInitData ,CLSYS_DRAW_MAIN , initWork->heapId );

  GFL_CLACT_WK_SetDrawEnable( pokeWork->pokeIcon , FALSE );
  GFL_CLACT_WK_SetAutoAnmFlag( pokeWork->pokeIcon , TRUE );

  pokeWork->ppp = NULL;
  pokeWork->isValid = FALSE;
  pokeWork->isMove = FALSE;
  pokeWork->alphaRetCnt = 0;
  return pokeWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void MB_SEL_POKE_DeleteWork( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork )
{
  GFL_CLACT_WK_Remove( pokeWork->pokeIcon );
  GFL_HEAP_FreeMemory( pokeWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
const BOOL MB_SEL_POKE_UpdateWork( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork )
{
  if( pokeWork->isMove == TRUE )
  {
    GFL_CLACTPOS cellPos;
    pokeWork->cnt++;
    
    cellPos.x = pokeWork->startX + pokeWork->moveOfsX*pokeWork->cnt/pokeWork->endCnt;
    cellPos.y = pokeWork->startY + pokeWork->moveOfsY*pokeWork->cnt/pokeWork->endCnt;
    GFL_CLACT_WK_SetPos( pokeWork->pokeIcon , &cellPos , CLSYS_DRAW_MAIN );
    
    if( pokeWork->cnt == pokeWork->endCnt )
    {
      if( pokeWork->isAutoDelete == TRUE )
      {
        MB_SEL_POKE_SetPPP( selWork , pokeWork , NULL );
      }
      pokeWork->isMove = FALSE;
      //�ŏI�m�F�L�����Z�����A�g���[�߂��̂ݗ�O����������̂ł��̑Ή�
      MB_SEL_POKE_SetPri( selWork , pokeWork , pokeWork->type );
    }
  }
  //�ړ������̂��߁A���ԍ��ŃA���t�@��߂��悤
  if( pokeWork->alphaRetCnt > 0 )
  {
    pokeWork->alphaRetCnt--;
    if( pokeWork->alphaRetCnt == 0 )
    {
      MB_SEL_POKE_SetAlpha( selWork , pokeWork , FALSE );
    }
    
  }
  return pokeWork->isMove;
}

//--------------------------------------------------------------
//	�A�C�R���쐬
//  NULL���s����ppp��n���Ə���
//--------------------------------------------------------------
void MB_SEL_POKE_SetPPP( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , POKEMON_PASO_PARAM *ppp )
{
  ARCHANDLE *iconArcHandle = MB_SELECT_GetIconArcHandle( selWork );
  const DLPLAY_CARD_TYPE type = MB_SELECT_GetCardType( selWork );
  const HEAPID heapId = MB_SELECT_GetHeapId( selWork );

  pokeWork->isValid = TRUE;
  pokeWork->ppp = ppp;
  
  if( ppp == NULL )
  {
    pokeWork->isValid = FALSE;
  }
  else
  if( PPP_Get( pokeWork->ppp , ID_PARA_poke_exist , NULL ) == FALSE )
  {
    pokeWork->isValid = FALSE;
  }
  
  if( pokeWork->isValid == TRUE )
  {
    const MB_UTIL_CHECK_PLAY_RET ret = MB_UTIL_CheckPlay_PalGate( pokeWork->ppp , type );
    NNSG2dImagePaletteProxy tempPlt;
    NNSG2dCharacterData *tempNcg;
    u32 pltAdr;
    u32 resId = MB_ICON_GetCharResId(pokeWork->ppp,type);
    
    //�M���e�B�i�Ή�
    if( PPP_Get( pokeWork->ppp , ID_PARA_monsno , NULL ) == MONSNO_GIRATHINA && 
        PPP_Get( pokeWork->ppp , ID_PARA_item , NULL ) == 112 ) //������
    {
      if( type != CARD_TYPE_DUMMY )
      {
        resId = 540;  //NARC_poke_icon_poke_icon_509_01_NCGR
      }
    }
    {
      void *tempRes = GFL_ARCHDL_UTIL_LoadOBJCharacter( iconArcHandle ,
                                            resId ,
                                            FALSE ,
                                            &tempNcg ,
                                            heapId );
      GFL_CLGRP_CGR_Replace( pokeWork->cellResIdx , tempNcg );
      NNS_G2dInitImagePaletteProxy( &tempPlt );
      if( ret == MUCPR_OK )
      {
        pltAdr = GFL_CLGRP_PLTT_GetAddr( pokeWork->initWork->palResIdx , CLSYS_DRAW_MAIN );
      }
      else
      {
        pltAdr = GFL_CLGRP_PLTT_GetAddr( pokeWork->initWork->palNoneResIdx , CLSYS_DRAW_MAIN );
      }
      NNS_G2dSetImagePaletteLocation( &tempPlt , NNS_G2D_VRAM_TYPE_2DMAIN , pltAdr );
      GFL_CLACT_WK_SetPlttProxy( pokeWork->pokeIcon , &tempPlt );
      GFL_CLACT_WK_SetPlttOffs( pokeWork->pokeIcon , 
                                MB_ICON_GetPalNumber(pokeWork->ppp) ,
                                CLWK_PLTTOFFS_MODE_PLTT_TOP );
      GFL_CLACT_WK_SetDrawEnable( pokeWork->pokeIcon , TRUE );
      GFL_HEAP_FreeMemory( tempRes );
      
      MB_SEL_POKE_SetPri( selWork , pokeWork , pokeWork->type );
    }
  }
  else
  {
    //���Ȃ�!
    GFL_CLACT_WK_SetDrawEnable( pokeWork->pokeIcon , FALSE );
  }
}

//--------------------------------------------------------------
//	���W�Z�b�g
//--------------------------------------------------------------
void MB_SEL_POKE_SetPos( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , GFL_CLACTPOS *pos )
{
  GFL_CLACT_WK_SetPos( pokeWork->pokeIcon , pos , CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------
//	�ړ��Z�b�g
//--------------------------------------------------------------
void MB_SEL_POKE_SetMove( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , int startX , int startY , int endX , int endY , const u8 cnt, const BOOL autoDel )
{
  GFL_CLACTPOS cellPos;

  pokeWork->startX = startX;
  pokeWork->startY = startY;
  pokeWork->moveOfsX = endX - startX;
  pokeWork->moveOfsY = endY - startY;
  pokeWork->endCnt = cnt;
  pokeWork->isMove = TRUE;
  pokeWork->cnt = 0;
  pokeWork->isAutoDelete = autoDel;
  
  cellPos.x = startX;
  cellPos.y = startY;
  GFL_CLACT_WK_SetPos( pokeWork->pokeIcon , &cellPos , CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------
//	�v���C�I���e�B�ݒ�
//--------------------------------------------------------------
void MB_SEL_POKE_SetPri( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , const MB_SEL_POKE_TYPE type )
{
  switch( type )
  {
  case MSPT_BOX:
    GFL_CLACT_WK_SetBgPri( pokeWork->pokeIcon , MB_SEL_POKE_BG_PRI_BOX );
    GFL_CLACT_WK_SetSoftPri( pokeWork->pokeIcon , MB_SEL_POKE_SOFT_PRI_BOX );
    break;

  case MSPT_TRAY:
    GFL_CLACT_WK_SetBgPri( pokeWork->pokeIcon , MB_SEL_POKE_BG_PRI_TRAY );
    GFL_CLACT_WK_SetSoftPri( pokeWork->pokeIcon , MB_SEL_POKE_SOFT_PRI_TRAY );
    break;
  
  case MSPT_HOLD:
    GFL_CLACT_WK_SetBgPri( pokeWork->pokeIcon , MB_SEL_POKE_BG_PRI_HOLD );
    GFL_CLACT_WK_SetSoftPri( pokeWork->pokeIcon , MB_SEL_POKE_SOFT_PRI_HOLD );
    break;
  }
}

//--------------------------------------------------------------
//	�A���t�@�ݒ�
//--------------------------------------------------------------
void MB_SEL_POKE_SetAlpha( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , const BOOL flg )
{
  if( flg == TRUE )
  {
    GFL_CLACT_WK_SetObjMode( pokeWork->pokeIcon , GX_OAM_MODE_XLU );
  }
  else
  {
    GFL_CLACT_WK_SetObjMode( pokeWork->pokeIcon , GX_OAM_MODE_NORMAL );
  }
}
void MB_SEL_POKE_ResetAlphaCnt( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork , const u8 cnt)
{
  pokeWork->alphaRetCnt = cnt;
}
const BOOL MB_SEL_POKE_GetAlpha( MB_SELECT_WORK *selWork , MB_SEL_POKE *pokeWork )
{
  if( GFL_CLACT_WK_GetObjMode( pokeWork->pokeIcon ) == GX_OAM_MODE_NORMAL )
  {
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
//	�ʒu�v�Z
//--------------------------------------------------------------
const int MB_SEL_POKE_GetPosX( MB_SEL_POKE *pokeWork )
{
  if( pokeWork->type == MSPT_BOX )
  {
    return (pokeWork->idx % MB_SEL_POKE_BOX_X_NUM)*MB_SEL_POKE_BOX_WIDTH + MB_SEL_POKE_BOX_LEFT;
  }
  else
  {
    return (pokeWork->idx % MB_SEL_POKE_TRAY_X_NUM)*MB_SEL_POKE_TRAY_WIDTH + MB_SEL_POKE_TRAY_LEFT;
  }
}
const int MB_SEL_POKE_GetPosY( MB_SEL_POKE *pokeWork )
{
  if( pokeWork->type == MSPT_BOX )
  {
    return (pokeWork->idx / MB_SEL_POKE_BOX_X_NUM)*MB_SEL_POKE_BOX_HEIGHT + MB_SEL_POKE_BOX_TOP;
  }
  else
  {
    if( pokeWork->idx%MB_SEL_POKE_TRAY_X_NUM == 0 )
    {
      return (pokeWork->idx / MB_SEL_POKE_TRAY_X_NUM)*MB_SEL_POKE_TRAY_HEIGHT + MB_SEL_POKE_TRAY_TOP ;
    }
    else
    {
      return (pokeWork->idx / MB_SEL_POKE_TRAY_X_NUM)*MB_SEL_POKE_TRAY_HEIGHT + MB_SEL_POKE_TRAY_TOP + MB_SEL_POKE_TRAY_OFS;
    }
  }
}

const BOOL MB_SEL_POKE_isValid( const MB_SEL_POKE *pokeWork )
{
  return pokeWork->isValid;
}
const MB_SEL_POKE_TYPE MB_SEL_POKE_GetType( const MB_SEL_POKE *pokeWork )
{
  return pokeWork->type;
}
const u8 MB_SEL_POKE_GetIdx( const MB_SEL_POKE *pokeWork )
{
  return pokeWork->idx;
}
void MB_SEL_POKE_SetIdx( MB_SEL_POKE *pokeWork , u8 idx )
{
  pokeWork->idx = idx;
}
POKEMON_PASO_PARAM* MB_SEL_POKE_GetPpp( MB_SEL_POKE *pokeWork )
{
  return pokeWork->ppp;
}
