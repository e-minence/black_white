//======================================================================
/**
 * @file  sta_act_button.c
 * @brief �X�e�[�W�p�@�ꔭ�t�]�{�^��
 * @author  ariizumi
 * @data  09/08/26
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "stage_gra.naix"
#include "dressup_gra.naix"

#include "musical/mus_item_data.h"

#include "sta_acting.h"
#include "sta_local_def.h"
#include "sta_act_button.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

#define STA_BUTTON_PLT (0)
#define STA_BUTTON_BASE_PLT (2)
#define STA_TOUCH_EFFECT_PLT (3)

//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  SBUP_RIGHT,
  SBUP_LEFT,
  SBUP_NONE,
  
  SBUP_MAX,
}STA_BUTTON_USEITEM_POS;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
struct _STA_BUTTON_SYS
{
  HEAPID heapId;
  ACTING_WORK* actWork;

  MUSICAL_POKE_PARAM *musPoke;
  BOOL canUseButton;
  BOOL isUseWait; //�ʐM���A�����܂ł̑҂�
  STA_BUTTON_USEITEM_POS useItemPos;

  GFL_CLUNIT  *cellUnit;
  u32 pltIdx[3];
  u32 ncgIdx[3];
  u32 anmIdx[3];

  BOOL      useButton[2];
  u16       equipItem[2];
  GFL_CLWK  *clwkButton[2];
  GFL_CLWK  *clwkButtonBase[2];
  GFL_CLWK  *clwkTouchEffect;
};


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

static void STA_BUTTON_InitCell( STA_BUTTON_SYS *work );
static void STA_BUTTON_TransTexToCell( STA_BUTTON_SYS *work , const u8 idx , const u16 itemNo , const GFL_BBD_TEXSIZ texType );

//--------------------------------------------------------------
//  ������
//--------------------------------------------------------------
STA_BUTTON_SYS* STA_BUTTON_InitSystem( HEAPID heapId , ACTING_WORK* actWork , MUSICAL_POKE_PARAM *musPoke )
{
  STA_BUTTON_SYS *work = GFL_HEAP_AllocMemory( heapId , sizeof(STA_BUTTON_SYS) );

  work->heapId = heapId;
  work->actWork = actWork;
  work->musPoke = musPoke;
  work->canUseButton = FALSE;
  work->isUseWait = FALSE;
  work->useItemPos = SBUP_NONE;
  
  work->equipItem[0] = work->musPoke->equip[MUS_POKE_EQU_HAND_R].itemNo;
  work->equipItem[1] = work->musPoke->equip[MUS_POKE_EQU_HAND_L].itemNo;

  work->useButton[0] = FALSE;
  work->useButton[1] = FALSE;

  STA_BUTTON_InitCell( work );

  GXS_SetVisibleWnd( GX_WNDMASK_W0 );
  G2S_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , TRUE );
  G2S_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , FALSE );
  G2S_SetWnd0Position( 0 , 128 , 255 , 192 );
  return work;
}

//--------------------------------------------------------------
//  �J��
//--------------------------------------------------------------
void STA_BUTTON_ExitSystem( STA_BUTTON_SYS *work )
{
  GXS_SetVisibleWnd( GX_WNDMASK_NONE );

  if( work->equipItem[0] != MUSICAL_ITEM_INVALID )
  {
    GFL_CLACT_WK_Remove( work->clwkButton[0] );
  }
  if( work->equipItem[1] != MUSICAL_ITEM_INVALID )
  {
    GFL_CLACT_WK_Remove( work->clwkButton[1] );
  }
  GFL_CLACT_WK_Remove( work->clwkTouchEffect );
  
  GFL_CLGRP_PLTT_Release( work->pltIdx[0] );
  GFL_CLGRP_CGR_Release( work->ncgIdx[0] );
  GFL_CLGRP_CELLANIM_Release( work->anmIdx[0] );
  GFL_CLGRP_PLTT_Release( work->pltIdx[1] );
  GFL_CLGRP_CGR_Release( work->ncgIdx[1] );
  GFL_CLGRP_CELLANIM_Release( work->anmIdx[1] );
  GFL_CLGRP_PLTT_Release( work->pltIdx[2] );
  GFL_CLGRP_CGR_Release( work->ncgIdx[2] );
  GFL_CLGRP_CELLANIM_Release( work->anmIdx[2] );
  
  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
void STA_BUTTON_UpdateSystem( STA_BUTTON_SYS *work )
{
  //�{�^���̃^�b�`����
  if( work->canUseButton == TRUE )
  {
    GFL_UI_TP_HITTBL hitTbl[3] =
    {
      {160-16,160+16, 32-16, 32+16},
      {160-16,160+16,224-16,224+16},
      {GFL_UI_TP_HIT_END,0,0,0},
    };
    const int ret = GFL_UI_TP_HitTrg( hitTbl );

    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ , 0 );
    if( ret != GFL_UI_TP_HIT_NONE )
    {
      if( work->equipItem[ret] != MUSICAL_ITEM_INVALID &&
          work->useButton[ret] == FALSE )
      {
        if( ret == 0 )
        {
          STA_ACT_UseItemRequest( work->actWork , MUS_POKE_EQU_HAND_R );
        }
        else
        {
          STA_ACT_UseItemRequest( work->actWork , MUS_POKE_EQU_HAND_L );
        }
        G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_OBJ , -8 );
        work->useButton[ret] = TRUE;
        work->canUseButton = FALSE;
        work->isUseWait = TRUE;
        work->useItemPos = ret;
        {
          GFL_CLACTPOS pos;
          pos.x = (ret == 0 ? 32 : 256-32);
          pos.y = 128;
          GFL_CLACT_WK_ResetAnm( work->clwkTouchEffect );
          GFL_CLACT_WK_SetDrawEnable( work->clwkTouchEffect, TRUE );
          GFL_CLACT_WK_SetPos( work->clwkTouchEffect , &pos , CLSYS_DEFREND_SUB );
        }
        GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
      }
    }
  }
  
  if( work->useItemPos != SBUP_NONE )
  {
    if( STA_ACT_IsUsingItemSelf( work->actWork ) == FALSE )
    {
      if( work->isUseWait == FALSE )
      {
        work->canUseButton = TRUE;
        GFL_CLACT_WK_SetDrawEnable( work->clwkButton[work->useItemPos], FALSE );
        GFL_CLACT_WK_SetDrawEnable( work->clwkButtonBase[work->useItemPos], FALSE );

        work->useItemPos = SBUP_NONE;
      }
    }
    else
    {
      if( work->isUseWait == TRUE )
      {
        work->isUseWait = FALSE;
      }
    }
  }
  //�^�b�`�G�t�F�N�g
  if( GFL_CLACT_WK_GetDrawEnable( work->clwkTouchEffect ) == TRUE )
  {
    if( GFL_CLACT_WK_CheckAnmActive( work->clwkTouchEffect ) == FALSE )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkTouchEffect, FALSE );
    } 
  }
}


//--------------------------------------------------------------
//  �Z��������
//--------------------------------------------------------------
static void STA_BUTTON_InitCell( STA_BUTTON_SYS *work )
{
  u8 i;
  MUS_ITEM_DATA_SYS* itemDataSys = STA_ACT_GetItemDataSys( work->actWork );
  //OBJ�p
  work->cellUnit  = GFL_CLACT_UNIT_Create( 5 , 0, work->heapId );
  GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
  //�e��f�ނ̓ǂݍ���
  {
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );
  
    work->pltIdx[0] = GFL_CLGRP_PLTT_RegisterEx( arcHandle , NARC_stage_gra_button_NCLR , CLSYS_DRAW_SUB , STA_BUTTON_PLT*0x20 , 0 , 2 , work->heapId  );
    work->ncgIdx[0] = GFL_CLGRP_CGR_Register( arcHandle , NARC_stage_gra_button_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->anmIdx[0] = GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_stage_gra_button_NCER , NARC_stage_gra_button_NANR, work->heapId  );
    work->pltIdx[1] = GFL_CLGRP_PLTT_RegisterEx( arcHandle , NARC_stage_gra_button_base_NCLR , CLSYS_DRAW_SUB , STA_BUTTON_BASE_PLT*0x20 , 0 , 1 , work->heapId  );
    work->ncgIdx[1] = GFL_CLGRP_CGR_Register( arcHandle , NARC_stage_gra_button_base_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->anmIdx[1] = GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_stage_gra_button_base_NCER , NARC_stage_gra_button_base_NANR, work->heapId  );
  
    GFL_ARC_CloseDataHandle(arcHandle);
  }
  //�e��f�ނ̓ǂݍ���(�h���X�A�b�v����^�b�`�G�t�F�N�g
  {
    ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_DRESSUP_GRA , work->heapId );
  
    work->pltIdx[2] = GFL_CLGRP_PLTT_Register( arcHandle , NARC_dressup_gra_anime_sita_NCLR , CLSYS_DRAW_SUB , STA_TOUCH_EFFECT_PLT*32 , work->heapId  );
    work->ncgIdx[2] = GFL_CLGRP_CGR_Register( arcHandle , NARC_dressup_gra_anime_stage_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->anmIdx[2] = GFL_CLGRP_CELLANIM_Register( arcHandle , NARC_dressup_gra_anime_stage_NCER , NARC_dressup_gra_anime_stage_NANR, work->heapId  );
  
    GFL_ARC_CloseDataHandle(arcHandle);
  }
  //�Z���쐬
  for( i=0;i<2;i++ )
  {
    if( work->equipItem[i] != MUSICAL_ITEM_INVALID )
    {
      u8 baseAnm;
      GFL_CLWK_DATA cellInitData;
      MUS_ITEM_DATA_WORK* itemDataWork = MUS_ITEM_DATA_GetMusItemData( itemDataSys , work->equipItem[i] );
      const GFL_BBD_TEXSIZ texType = MUS_ITEM_DATA_GetTexType( itemDataWork );
      if( i == 0 )
      {
        cellInitData.pos_x = 32;
      }
      else
      {
        cellInitData.pos_x = 224;
      }
      cellInitData.pos_y = 160;
      baseAnm = 0;

      cellInitData.anmseq = i;
      cellInitData.softpri = 10;
      cellInitData.bgpri = 0;
      work->clwkButton[i] = GFL_CLACT_WK_Create( work->cellUnit ,work->ncgIdx[0],work->pltIdx[0],work->anmIdx[0],
                    &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
      cellInitData.anmseq = baseAnm;
      work->clwkButtonBase[i] = GFL_CLACT_WK_Create( work->cellUnit ,work->ncgIdx[1],work->pltIdx[1],work->anmIdx[1],
                    &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );

      GFL_CLACT_WK_SetDrawEnable( work->clwkButton[i], FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkButtonBase[i], FALSE );

      STA_BUTTON_TransTexToCell( work , i , work->equipItem[i] , texType );
    }
  }
  {
    //�Z���̐���
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 128;
    cellInitData.pos_y = 96;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    work->clwkTouchEffect = GFL_CLACT_WK_Create( work->cellUnit ,
              work->ncgIdx[2],
              work->pltIdx[2],
              work->anmIdx[2],
              &cellInitData ,CLSYS_DEFREND_SUB , work->heapId );
    GFL_CLACT_WK_SetDrawEnable( work->clwkTouchEffect, FALSE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->clwkTouchEffect, TRUE );
  }
}

//--------------------------------------------------------------
//  �e�N�X�`���̉摜��NCG�ENCL�ɕϊ�
//--------------------------------------------------------------
static void STA_BUTTON_TransTexToCell( STA_BUTTON_SYS *work , const u8 idx , const u16 itemNo , const GFL_BBD_TEXSIZ texType )
{
  //�e�N�X�`���]���e�X�g
  void *texRes = GFL_ARC_UTIL_Load( ARCID_MUSICAL_ITEM, itemNo , FALSE , work->heapId );
  NNSG3dResTex *texData = NNS_G3dGetTex( texRes );
  
  //Tex�EPlt�A�h���X
  void *texAdr = (void*)((u32)(texData) + texData->texInfo.ofsTex);
  void *pltAdr = (void*)((u32)(texData) + texData->plttInfo.ofsPlttData);
  //Tex�EPlt�T�C�Y 3bit�V�t�g���ꂽ��Ԃœ����Ă���
  const u32 texSize = (texData->texInfo.sizeTex)<<3;
  const u32 pltSize = (texData->plttInfo.sizePltt)<<3;
  
  const u32 ncgOfs = GFL_CLGRP_CGR_GetAddr( work->ncgIdx[0] , CLSYS_DRAW_SUB ) + idx*0x20*8*8;

  
  //�p���b�g�̓]��
  DC_FlushRange( pltAdr , pltSize );
  GXS_LoadOBJPltt( pltAdr , idx*0x20 , pltSize );

  //�G�̓]���i�G�̕��я����Ⴄ�̂Œ���
  DC_FlushRange( texAdr , texSize );
  {
    u8 cbx; //CharBaseX
    u8 cby;
    u8 cx;  //CharaX
    u8 cy;
    u8 cxs; //CharaXSize
    u8 cys;
    u8 line;
    
    switch(texType)
    {
    case GFL_BBD_TEXSIZ_32x32:
      cbx = 2;
      cby = 2;
      cxs = 4;
      cys = 4;
      break;

    case GFL_BBD_TEXSIZ_32x64:
      cbx = 2;
      cby = 0;
      cxs = 4;
      cys = 8;
      break;

    case GFL_BBD_TEXSIZ_64x32:
      cbx = 0;
      cby = 2;
      cxs = 8;
      cys = 4;
      break;
    
    default:
      GF_ASSERT_MSG( NULL , "" );
      cbx = 2;
      cby = 2;
      cxs = 4;
      cys = 4;
      break;
    }
    
    for( cy = 0 ; cy <cys ; cy++ )
    {
      for( line = 0 ; line <8 ; line++ )
      {
        for( cx = 0 ; cx <cxs ; cx++ )
        {
          const u32 ncgOfs2 = (cy+cby)*0x20*8 + (cx+cbx)*0x20 + line*4;
          const u32 texOfs = cy*0x20*cxs + line*cxs*4 + cx*4;
          GXS_LoadOBJ( (void*)(((u32)texAdr)+texOfs) , ncgOfs+ncgOfs2 , 4 );
        }
      }
    }
  }
  GFL_HEAP_FreeMemory( texRes );  
}

//--------------------------------------------------------------
//  �{�^���g�p�\��
//--------------------------------------------------------------
void STA_BUTTON_SetCanUseButton( STA_BUTTON_SYS *work , const BOOL flg )
{
  if( work->equipItem[0] != MUSICAL_ITEM_INVALID )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkButton[0], flg );
    GFL_CLACT_WK_SetDrawEnable( work->clwkButtonBase[0], flg );
  }
  if( work->equipItem[1] != MUSICAL_ITEM_INVALID )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkButton[1], flg );
    GFL_CLACT_WK_SetDrawEnable( work->clwkButtonBase[1], flg );
  }
  work->canUseButton = flg;
}