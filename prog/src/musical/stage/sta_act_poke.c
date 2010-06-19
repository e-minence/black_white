//======================================================================
/**
 * @file  sta_act_poke.h
 * @brief �X�e�[�W�p�@�|�P����
 * @author  ariizumi
 * @data  09/03/06
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "stage_gra.naix"

#include "sta_local_def.h"
#include "sta_snd_def.h"
#include "sta_acting.h"
#include "sta_act_poke.h"
#include "script/sta_act_script_def.h"

#include "test/ariizumi/ari_debug.h"


//======================================================================
//  define
//======================================================================
#pragma mark [> define

//�A�C�e���g�p�֌W
#define STA_POKE_ITEMUSE_TIME (60*3)
#define STA_POKE_ITEMUSE_TIME_SPIN (64*3) //��]�p�x�v�Z��L���̂���������
#define STA_POKE_ITEMUSE_FLYING_SPD (FX32_CONST(0.75f)) //�㏸���x

#define STA_POKE_ITEMUSE_THROW_START_ACC (FX32_CONST(-3.0f)) //���������x(�����
#define STA_POKE_ITEMUSE_THROW_ADD_ACC (FX32_CONST(0.04f)) //�����x
#define STA_POKE_ITEMUSE_THROW_SPD (FX32_CONST(0.7f)) //���ړ����x
#define STA_POKE_ITEMUSE_THROW_SIZE_ADD (FX32_CONST(0.003f)) 

#define STA_POKE_EYE_BLINK_CNT (120)
#define STA_POKE_EYE_BLINK_CNT_OFS (5)


//======================================================================
//  enum
//======================================================================
#pragma mark [> enum


typedef void (*STA_POKE_InitItemUseFunc)( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
typedef void (*STA_POKE_UpdateItemUseFunc)( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  u16 cnt;
  MUS_POKE_EQUIP_POS equipPos;

  STA_EFF_WORK   *effWork;

  //�����p�d�͉����x
  fx32        graAcc;
}STA_POKE_ITEMUSE_WORK;


struct _STA_POKE_WORK
{
  BOOL    isEnable;
  BOOL    isUpdate; //���W�v�Z�������������H
  BOOL    isFront;
  BOOL    isDrawItem;
  
  BOOL    isGround; //�W�����v�֎~
  BOOL    isNoFlip; //���]�֎~
  
  VecFx32         pokePos;
  VecFx32         scale;
  VecFx32         posOfs; //�ړ�����(�A�N�V�����p��
  VecFx32         rotOfs; //��]����(�O�b�Y�ە␳�p��
  u16             rotate;
  BOOL            mepachiFlg;
  u16             mepathiCnt;

  STA_POKE_DIR      dir;
  MUS_POKE_DRAW_WORK    *drawWork;

  int           shadowBbdIdx;

  GFL_G3D_RES     *itemRes[MUS_POKE_EQUIP_MAX];
  MUS_ITEM_DRAW_WORK  *itemWork[MUS_POKE_EQUIP_MAX];
  MUSICAL_POKE_EQUIP  *pokeEquip[MUS_POKE_EQUIP_MAX];
  BOOL                isEnableItem[MUS_POKE_EQUIP_MAX];
  
  //�A�C�e���g�p�n
  STA_POKE_ITEMUSE_WORK itemUseWork;
  
  MUSICAL_ITEM_USETYPE   itemUseType;
  STA_POKE_UpdateItemUseFunc updateItemUseFunc;
};

struct _STA_POKE_SYS
{
  HEAPID heapId;
  ACTING_WORK *actWork;

  GFL_BBD_SYS       *bbdSys;
  MUS_POKE_DRAW_SYSTEM  *drawSys;
  MUS_ITEM_DRAW_SYSTEM  *itemDrawSys;
  STA_POKE_WORK     pokeWork[MUSICAL_POKE_MAX];
  
  u16   scrollOffset;

  int   shadowResIdx;
  u8    useItemSeCnt;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

static void STA_POKE_UpdatePokeFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
static void STA_POKE_UpdateItemFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
static void STA_POKE_DrawShadow( STA_POKE_SYS *work ,  STA_POKE_WORK *pokeWork );

void STA_POKE_InitItemUse_Spin( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_UpdateItemUse_Spin( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_InitItemUse_Flash( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_UpdateItemUse_Flash( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_InitItemUse_Flying( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_UpdateItemUse_Flying( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_InitItemUse_Throw( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_UpdateItemUse_Throw( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_InitItemUse_Use( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );
void STA_POKE_UpdateItemUse_Use( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork );

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
STA_POKE_SYS* STA_POKE_InitSystem( HEAPID heapId , ACTING_WORK *actWork , MUS_POKE_DRAW_SYSTEM* drawSys , MUS_ITEM_DRAW_SYSTEM *itemDrawSys , GFL_BBD_SYS* bbdSys )
{
  u8 i;
  STA_POKE_SYS *work = GFL_HEAP_AllocMemory( heapId , sizeof(STA_POKE_SYS) );
  
  work->heapId = heapId;
  work->actWork = actWork;
  work->drawSys = drawSys;
  work->itemDrawSys = itemDrawSys;
  work->scrollOffset = 0;
  work->bbdSys = bbdSys;
  work->useItemSeCnt = 0;

  for( i=0 ; i<MUSICAL_POKE_MAX ; i++ )
  {
    work->pokeWork[i].isEnable = FALSE;
    work->pokeWork[i].isUpdate = FALSE;
  }
  
  
  //�e�|���̓ǂݍ���
  work->shadowResIdx = GFL_BBD_AddResourceArc( work->bbdSys , ARCID_STAGE_GRA , 
                      NARC_stage_gra_shadow_tex_nsbtx , GFL_BBD_TEXFMT_A3I5 ,
                      GFL_BBD_TEXSIZ_64x64 , 64 , 64 );
  
  return work;
}

void  STA_POKE_ExitSystem( STA_POKE_SYS *work )
{
  int idx;
  for( idx=0 ; idx<MUSICAL_POKE_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == TRUE )
    {
      STA_POKE_DeletePoke( work , &work->pokeWork[idx] );
    }
  }
  GFL_BBD_RemoveResource( work->bbdSys , work->shadowResIdx );

  GFL_HEAP_FreeMemory( work );
}

void  STA_POKE_UpdateSystem( STA_POKE_SYS *work )
{
  int idx;
  for( idx=0 ; idx<MUSICAL_POKE_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == TRUE )
    {
      STA_POKE_UpdatePokeFunc( work , &work->pokeWork[idx] );
    }
  }
}

void  STA_POKE_UpdateSystem_Item( STA_POKE_SYS *work )
{
  int idx,i;
  for( idx=0 ; idx<MUSICAL_POKE_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == TRUE )
    {
      STA_POKE_UpdateItemFunc( work , &work->pokeWork[idx] );
      if( work->actWork != NULL )
      {
        for( i=0 ; i<STA_ACT_GetDelayVCnt(work->actWork) ; i++ )
        {
          if( work->pokeWork[idx].updateItemUseFunc != NULL )
          {
            work->pokeWork[idx].updateItemUseFunc( work , &work->pokeWork[idx] );
          }
        }
      }

    }
  }
  MUS_ITEM_DRAW_UpdateSystem( work->itemDrawSys ); 
}

static void STA_POKE_UpdatePokeFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  //if( pokeWork->isUpdate == TRUE )
  {
    VecFx32 musPos;
    VecFx32 musScale;
    //�e����
    VecFx32 *shadowOfs = MUS_POKE_DRAW_GetShadowOfs( pokeWork->drawWork );
    //��]����
    VecFx32 *rotOfsTemp = MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
    VecFx32 rotOfsBase;
    
    //�|�P�����̍��W
    VEC_Add( &pokeWork->pokePos , &pokeWork->posOfs , &musPos );

    //��]�␳
    {
      u16 rotZ = pokeWork->rotate;
      MtxFx33 rotWork;
      rotOfsBase.x = 0;//rotOfsTemp->x - shadowOfs->x;
      rotOfsBase.y = rotOfsTemp->y;
      MTX_RotZ33( &rotWork , -FX_SinIdx( rotZ ) , FX_CosIdx( rotZ ) );
      MTX_MultVec33( &rotOfsBase , &rotWork , &pokeWork->rotOfs );
      VEC_Subtract( &rotOfsBase , &pokeWork->rotOfs , &pokeWork->rotOfs );
    }
      
    //�e�̍��W�����
    musPos.x = ACT_POS_X_FX(musPos.x - shadowOfs->x - pokeWork->rotOfs.x );
    musPos.y = ACT_POS_Y_FX(musPos.y - shadowOfs->y + pokeWork->rotOfs.y );
//    musPos.z = musPos.z;
    
    MUS_POKE_DRAW_SetPosition( pokeWork->drawWork , &musPos);

    //��]�̓K�p
    MUS_POKE_DRAW_SetRotation( pokeWork->drawWork , pokeWork->rotate);
    
    //�X�P�[���̒����ƌ����̊m�F
    musScale.x = pokeWork->scale.x * 16;
    musScale.y = pokeWork->scale.y * 16;
    musScale.z = pokeWork->scale.z * 16;
    if( pokeWork->dir == SPD_RIGHT )
    {
      musScale.x *= -1;
    }
    MUS_POKE_DRAW_SetScale( pokeWork->drawWork , &musScale );
    
    //�A�C�e���̍X�V�v��H
    pokeWork->isUpdate = FALSE;
  }
  
  //�ڃp�`
  if( pokeWork->mepachiFlg == TRUE )
  {
    pokeWork->mepathiCnt += GFUser_GetPublicRand(2);
    if( pokeWork->mepathiCnt > STA_POKE_EYE_BLINK_CNT )
    {
      MUS_POKE_DRAW_SetMepachiFlg( pokeWork->drawWork , TRUE );
    }
    else
    {
      MUS_POKE_DRAW_SetMepachiFlg( pokeWork->drawWork , FALSE );
    }
    if( pokeWork->mepathiCnt > STA_POKE_EYE_BLINK_CNT+STA_POKE_EYE_BLINK_CNT_OFS )
    {
      pokeWork->mepathiCnt = GFUser_GetPublicRand(STA_POKE_EYE_BLINK_CNT)/3;
    }
  }
}

static void STA_POKE_UpdateItemFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  int ePosTemp;
  int ePos;
  VecFx32 pos;
  for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
  {
    ePosTemp = ePos;
    //���]���Ă��E�͉E�Ɏ��I
    if( pokeWork->dir == SPD_RIGHT )
    {
      if( ePos == MUS_POKE_EQU_HAND_R )
      {
        ePosTemp = MUS_POKE_EQU_HAND_L;
      }
      else
      if( ePos == MUS_POKE_EQU_HAND_L )
      {
        ePosTemp = MUS_POKE_EQU_HAND_R;
      }
      else
      if( ePos == MUS_POKE_EQU_EAR_R )
      {
        ePosTemp = MUS_POKE_EQU_EAR_L;
      }
      else
      if( ePos == MUS_POKE_EQU_EAR_L )
      {
        ePosTemp = MUS_POKE_EQU_EAR_R;
      }
    }

    if( pokeWork->itemWork[ePos] != NULL &&
        pokeWork->isEnableItem[ePos] == TRUE )
    {
      if( pokeWork->isDrawItem == TRUE &&
          pokeWork->isFront == TRUE )
      {
        //������FALSE�ł�������Ƃ͌���Ȃ��̂ŏ��if����Ɨ����Ă�
        //if( pokeWork->isEnableItem[ePos] == TRUE )
        {
          MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( pokeWork->drawWork , ePosTemp );
          if( equipData->isEnable == TRUE )
          {
            const BOOL flipS = ( equipData->scale.x < 0 ? TRUE : FALSE);
            const u16 rotZ = 0x10000-equipData->rot;//( flipS==TRUE ? 0x10000-equipData->rot : equipData->rot);
            u16 itemRot = (0x10000+equipData->itemRot+pokeWork->pokeEquip[ePos]->angle)%0x10000;          
            VecFx32 *shadowOfs = MUS_POKE_DRAW_GetShadowOfs( pokeWork->drawWork );
            MtxFx33 rotWork;
            VecFx32 rotOfs;
            VecFx32 ofs;
            fx32 tempScaleX = equipData->scale.x;
            MTX_RotZ33( &rotWork , -FX_SinIdx( rotZ ) , FX_CosIdx( rotZ ) );
            MTX_MultVec33( &equipData->ofs , &rotWork , &ofs );
            
            if( flipS == TRUE )
            {
              itemRot = 0x10000-itemRot;
            }
            
            if( MUS_ITEM_DRAW_CanReverseItem( pokeWork->itemWork[ePos] ) == FALSE &&
                flipS == TRUE )
            {
              tempScaleX = -tempScaleX;
            }
            {
              MTX_MultVec33( &equipData->rotOfs , &rotWork , &rotOfs );
              VEC_Subtract( &equipData->rotOfs , &rotOfs , &rotOfs );
            }
/*

            pos.x = (equipData->pos.x+ofs.x+FX32_CONST(128.0f) + rotOfs.x + pokeWork->rotOfs.x - shadowOfs->x) + FX32_CONST(work->scrollOffset);
            pos.y = (equipData->pos.y+ofs.y+FX32_CONST(96.0f) + rotOfs.y - pokeWork->rotOfs.y);
*/
/*
            if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
            {
              VEC_Add( &pokeWork->pokePos , &pokeWork->posOfs , &pos );

              pos.x += ofs.x + FX32_CONST(128.0f) + FX32_CONST(work->scrollOffset);
              pos.y += ofs.y + FX32_CONST(96.0f);
            }
            else
*/
            {
              pos.x = equipData->pos.x + ofs.x + FX32_CONST(128.0f) + FX32_CONST(work->scrollOffset);
              pos.y = equipData->pos.y + ofs.y + FX32_CONST(96.0f);
            }
            if( MUS_ITEM_DRAW_IsBackItem( pokeWork->itemWork[ePos] ) == TRUE )
            {
              //�w�ʗp�A�C�e��
              pos.z = pokeWork->pokePos.z-FX32_CONST(20.0f);
            }
            else
            if( ePosTemp == MUS_POKE_EQU_HAND_R ||
                ePosTemp == MUS_POKE_EQU_HAND_L )
            {
              //�O�ʗp�A�C�e��
              pos.z = pokeWork->pokePos.z+FX32_CONST(10.0f) - (pokeWork->pokeEquip[ePos]->priority*FX32_CONST(0.5f));
            }
            else
            {
              //�|�P�̑O�ɏo��
              pos.z = pokeWork->pokePos.z+FX32_CONST(5.0f) - (pokeWork->pokeEquip[ePos]->priority*FX32_CONST(0.5f));
            }

            //OS_Printf("[%.2f][%.2f]\n",F32_CONST(equipData->pos.z),F32_CONST(pokePos.z));
            MUS_ITEM_DRAW_SetPosition(  work->itemDrawSys , 
                          pokeWork->itemWork[ePos] ,
                          &pos );
            MUS_ITEM_DRAW_SetRotation(  work->itemDrawSys , 
                          pokeWork->itemWork[ePos] ,
                          itemRot-rotZ );
            MUS_ITEM_DRAW_SetSize(    work->itemDrawSys , 
                          pokeWork->itemWork[ePos] ,
                          tempScaleX /16 /4,
                          equipData->scale.y /16 /4);
            MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                            pokeWork->itemWork[ePos] , TRUE );
            /*
            MUS_ITEM_DRAW_SetFlipS( work->itemDrawSys , 
                        pokeWork->itemWork[ePos] ,
                        flipS );
            */
          }
        }
      }
      else
      {
        MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                        pokeWork->itemWork[ePos] , FALSE );
      }
    }
  }

  //�Ǐ]�Ȃ̂ł����ňꏏ�ɂ��
  //�e�̍��W(�e������W�ɂ���̂ŁA�e�̍����͌��Ȃ��B
  {
    VecFx32 *shadowOfs; //�e����
    shadowOfs = MUS_POKE_DRAW_GetShadowOfs( pokeWork->drawWork );
//    pos.x = ACT_POS_X_FX(pokeWork->pokePos.x + shadowOfs->x );
//    pos.y = ACT_POS_Y_FX(pokeWork->pokePos.y + shadowOfs->y );
    pos.x = ACT_POS_X_FX(pokeWork->pokePos.x );
    pos.y = ACT_POS_Y_FX(pokeWork->pokePos.y );
    pos.z = 50.0f;
    GFL_BBD_SetObjectTrans( work->bbdSys , pokeWork->shadowBbdIdx , &pos );
  }
}


void  STA_POKE_DrawSystem( STA_POKE_SYS *work )
{
  //�ۉe
  int idx;
  for( idx=0 ; idx<MUSICAL_POKE_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == TRUE )
    {
      if( STA_POKE_GetShowFlg( work , &work->pokeWork[idx] ) == TRUE )
      {
        STA_POKE_DrawShadow( work , &work->pokeWork[idx] );
      }
    }
  }
}
#if 0 //�|���S��������Ver
static void STA_POKE_DrawShadow( STA_POKE_SYS *work ,  STA_POKE_WORK *pokeWork )
{
  u32 i;
  u16 add = 0x1000;
  VecFx32 shadowPos;
  const fx32 sizeX = FX32_HALF;
  const fx32 sizeY = FX32_CONST(0.25f);
  
  shadowPos.x = ACT_POS_X_FX(pokeWork->pokePos.x - FX32_CONST(work->scrollOffset)) /4;
  shadowPos.y = ACT_POS_Y_FX(pokeWork->pokePos.y)/4;
  shadowPos.z = FX32_CONST(10.0f)/4;

  G3_PushMtx();
  G3_PolygonAttr(GX_LIGHTMASK_NONE,  // no lights
           GX_POLYGONMODE_MODULATE,   // modulation mode
           GX_CULL_NONE,     // cull none
           ACT_POLYID_SHADOW, // polygon ID(0 - 63)
           4,    // alpha(0 - 31)
           0           // OR of GXPolygonAttrMisc's value
    );

//  G3_Translate( 0,0,FX32_CONST(199.0f));
  G3_Scale( FX32_ONE*4,FX32_ONE*4,FX32_ONE);
  G3_Begin(GX_BEGIN_TRIANGLES);
  {
    G3_Color(GX_RGB(0,0,0));
    for( i=0;i<0x10000;i+=add)
    {
      G3_Vtx( shadowPos.x,shadowPos.y,shadowPos.z);
      G3_Vtx( shadowPos.x + FX_Mul(FX_SinIdx((u16)i),sizeX) ,
          shadowPos.y + FX_Mul(FX_CosIdx((u16)i),sizeY) ,
          shadowPos.z);
      G3_Vtx( shadowPos.x + FX_Mul(FX_SinIdx((u16)(i+add)),sizeX) ,
          shadowPos.y + FX_Mul(FX_CosIdx((u16)(i+add)),sizeY) ,
          shadowPos.z);
      
    }
  }
  G3_End();
  G3_PopMtx(1); 
}
#else
//�|��Ver
static void STA_POKE_DrawShadow( STA_POKE_SYS *work ,  STA_POKE_WORK *pokeWork )
{
  //Update�Ń|�P�����ƈꏏ�ɍ��W�ݒ肵�Ă�
}

#endif

void  STA_POKE_System_SetScrollOffset( STA_POKE_SYS *work , const u16 scrOfs )
{
  u8 idx;
  work->scrollOffset = scrOfs;

  for( idx=0 ; idx<MUS_POKE_EQUIP_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == TRUE )
    {
      work->pokeWork[idx].isUpdate = TRUE;
    }
  }
}


STA_POKE_WORK* STA_POKE_CreatePoke( STA_POKE_SYS *work , MUSICAL_POKE_PARAM *musPoke )
{
  u8 idx;
  u8 ePos;  //equipPos
  STA_POKE_WORK *pokeWork;
  
  for( idx=0 ; idx<MUS_POKE_EQUIP_MAX ; idx++ )
  {
    if( work->pokeWork[idx].isEnable == FALSE )
    {
      break;
    }
  }
  GF_ASSERT_MSG( idx < MUS_POKE_EQUIP_MAX , "Stage acting poke work is full!\n" );
  
  pokeWork = &work->pokeWork[idx];

  pokeWork->isEnable = TRUE;
  pokeWork->isUpdate = TRUE;
  pokeWork->isFront = TRUE;
  pokeWork->isDrawItem = FALSE;
  pokeWork->isGround = FALSE;
  pokeWork->isNoFlip = FALSE;
  pokeWork->updateItemUseFunc = NULL;
  pokeWork->drawWork = MUS_POKE_DRAW_Add( work->drawSys , musPoke , TRUE );

  //�W�����v�E���]�̃`�F�b�N
  {
    POKEMON_PERSONAL_DATA *personalData = POKE_PERSONAL_OpenHandle( musPoke->mcssParam.monsno , musPoke->mcssParam.form ,work->heapId );
    if( POKE_PERSONAL_GetParam( personalData ,  POKEPER_ID_no_jump ) == TRUE )
    {
      pokeWork->isGround = TRUE;
    }
    if( POKE_PERSONAL_GetParam( personalData ,  POKEPER_ID_reverse ) == TRUE )
    {
      pokeWork->isNoFlip = TRUE;
    }
    POKE_PERSONAL_CloseHandle( personalData );
  }

  VEC_Set( &pokeWork->pokePos , 0,0,0 );
  VEC_Set( &pokeWork->posOfs , 0,0,0 );
  VEC_Set( &pokeWork->scale , FX32_ONE,FX32_ONE,FX32_ONE );
  pokeWork->rotate = 0;
//  MUS_POKE_DRAW_StartAnime( pokeWork->drawWork );
  //���\�[�X�ǂݍ��݂�TCB��TRUE�ɂ����̂ŁE�E�E
//  MUS_POKE_DRAW_SetShowFlg( pokeWork->drawWork , FALSE );

  for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
  {
    const u16 itemNo = musPoke->equip[ePos].itemNo;
    pokeWork->pokeEquip[ePos] = &musPoke->equip[ePos];
    if( itemNo != MUSICAL_ITEM_INVALID )
    {
      pokeWork->isEnableItem[ePos] = TRUE;
      pokeWork->itemRes[ePos] = MUS_ITEM_DRAW_LoadResource( itemNo );
      pokeWork->itemWork[ePos] = MUS_ITEM_DRAW_AddResource( work->itemDrawSys , itemNo , pokeWork->itemRes[ePos] , &pokeWork->pokePos );
      MUS_ITEM_DRAW_SetSize( work->itemDrawSys , pokeWork->itemWork[ePos] , FX16_CONST(0.25f) , FX16_CONST(0.25f) );
      MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , pokeWork->itemWork[ePos] , FALSE );
      //ACT_BBD_SIZE(32.0f)
    }
    else
    {
      pokeWork->isEnableItem[ePos] = FALSE;
      pokeWork->itemRes[ePos] = NULL;
      pokeWork->itemWork[ePos] = NULL;
    }
  }

  //�e�|���̍쐬
  //�G�b�W�}�[�L���O�h�~�̂��ߔ������ɂ���
  {
    fx16 sizX = FX16_CONST(1.0f)/4;
    fx16 sizY = FX16_CONST(0.75f)/4;
//    pokeWork->shadowBbdIdx = GFL_BBD_AddObject( work->bbdSys , work->shadowResIdx 
//                                                , sizX ,  sizY, 
//                                                &pokeWork->pokePos , 12 , GFL_BBD_LIGHT_NONE );
    pokeWork->shadowBbdIdx = GFL_BBD_AddObject( work->bbdSys , work->shadowResIdx 
                                                , sizX ,  sizY, 
                                                &pokeWork->pokePos , 30 , GFL_BBD_LIGHT_NONE );
  }
  {
    BOOL flg = TRUE;
    GFL_BBD_SetObjectDrawEnable( work->bbdSys , pokeWork->shadowBbdIdx , &flg );
  }
  pokeWork->mepathiCnt = GFUser_GetPublicRand(150)/2;
  pokeWork->mepachiFlg = TRUE;
  return pokeWork;
}

void STA_POKE_DeletePoke( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  u8 ePos;  //equipPos
  GFL_BBD_RemoveObject( work->bbdSys , pokeWork->shadowBbdIdx );
  for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
  {
    if( pokeWork->itemWork[ePos] != NULL )
    {
      MUS_ITEM_DRAW_DelItem( work->itemDrawSys , pokeWork->itemWork[ePos] );
      MUS_ITEM_DRAW_DeleteResource( pokeWork->itemRes[ePos] );

      pokeWork->itemRes[ePos] = NULL;
      pokeWork->itemWork[ePos] = NULL;
    }
  }
  MUS_POKE_DRAW_Del( work->drawSys , pokeWork->drawWork );
  pokeWork->isEnable = FALSE;
}

#pragma mark [>use item func
void STA_POKE_UseItemFunc( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , MUS_POKE_EQUIP_POS ePos )
{
  if( pokeWork->itemWork[ePos] != NULL &&
      pokeWork->updateItemUseFunc == NULL )
  {
    static const u32 UseSeArr[MUT_MAX] = { 
      STA_SE_USEITEM_SPIN ,
      STA_SE_USEITEM_FLASH ,
      STA_SE_USEITEM_FLYING ,
      STA_SE_USEITEM_THROW ,
      STA_SE_USEITEM_USE };
    
    MUS_ITEM_DATA_SYS *itemDataSys = MUS_ITEM_DRAW_GetItemDataSys ( work->itemDrawSys );
    const u16 itemNo = pokeWork->pokeEquip[ePos]->itemNo;
    const MUSICAL_ITEM_USETYPE useType = MUS_ITEM_DATA_GetItemUseType( itemDataSys , itemNo );
//    const MUSICAL_ITEM_USETYPE useType = GFL_STD_MtRand0(5);
//    const MUSICAL_ITEM_USETYPE useType = MUT_USE;
    
    //���ʏ�����
    pokeWork->itemUseWork.cnt = 0;
    pokeWork->itemUseWork.equipPos = ePos;
    pokeWork->itemUseWork.effWork = NULL;
    
    
    //SE�Đ�
    PMSND_PlaySE_byPlayerID( UseSeArr[useType] , SEPLAYER_SE2 );
    work->useItemSeCnt++;
    
    //�ʏ�����
    switch(useType)
    {
    case MUT_SPIN:
      STA_POKE_InitItemUse_Spin( work , pokeWork );
      pokeWork->updateItemUseFunc = STA_POKE_UpdateItemUse_Spin;
      break;
    case MUT_FLASH:
      STA_POKE_InitItemUse_Flash( work , pokeWork );
      pokeWork->updateItemUseFunc = STA_POKE_UpdateItemUse_Flash;
      break;
    case MUT_FLYING:
      STA_POKE_InitItemUse_Flying( work , pokeWork );
      pokeWork->updateItemUseFunc = STA_POKE_UpdateItemUse_Flying;
      break;
    case MUT_THROW:
      STA_POKE_InitItemUse_Throw( work , pokeWork );
      pokeWork->updateItemUseFunc = STA_POKE_UpdateItemUse_Throw;
      break;
    case MUT_USE:
      STA_POKE_InitItemUse_Use( work , pokeWork );
      pokeWork->updateItemUseFunc = STA_POKE_UpdateItemUse_Use;
      break;
    }
    
  }
}

const BOOL STA_POKE_IsUsingItem( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  if( pokeWork->updateItemUseFunc == NULL )
  {
    return FALSE;
  }
  return TRUE; 
}

void STA_POKE_InitItemUse_Spin( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
}

void STA_POKE_UpdateItemUse_Spin( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  pokeWork->pokeEquip[itemUseWork->equipPos]->angle += 0x1000;
  
  itemUseWork->cnt++;
  if( itemUseWork->cnt >= STA_POKE_ITEMUSE_TIME_SPIN )
  {
    pokeWork->updateItemUseFunc = NULL;
    work->useItemSeCnt--;
    if( work->useItemSeCnt == 0 )
    {
      PMSND_StopSE_byPlayerID(SEPLAYER_SE2);
    }
  }
}

void STA_POKE_InitItemUse_Flash( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  VecFx32 pos;
  STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );
  MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( pokeWork->drawWork , pokeWork->itemUseWork.equipPos );

  itemUseWork->effWork = STA_EFF_CreateEffect( effSys , NARC_stage_gra_mus_eff_itemuse_spa );

  //�w�ʑΉ��E�A�b�p�[�����ӏ������Ƃ��Ή�
  if( pokeWork->isFront == TRUE &&
      equipData->isEnable == TRUE )
  {
    MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] ,
                  &pos );
    pos.x = ACT_POS_X_FX(pos.x);
    pos.y = ACT_POS_Y_FX(pos.y);
    pos.z += FX32_ONE;
  }
  else
  {
    VecFx32 *rotOfs = MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
    VEC_Add( &pokeWork->pokePos , rotOfs , &pos );
    pos.x = ACT_POS_X_FX(pos.x);
    pos.y = ACT_POS_Y_FX(pos.y);
    pos.z -= (FX32_ONE*28); //���ɏo������
  }
  
  OS_FPrintf(3,"[%6.2f][%6.2f][%6.2f]\n",F32_CONST(pos.x),F32_CONST(pos.y),F32_CONST(pos.z));
  STA_EFF_CreateEmitter( itemUseWork->effWork , 0 , &pos );
  
}

void STA_POKE_UpdateItemUse_Flash( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( pokeWork->drawWork , pokeWork->itemUseWork.equipPos );
  VecFx32 pos;
  
  //�w�ʑΉ��E�A�b�p�[�����ӏ������Ƃ��Ή�
  if( pokeWork->isFront == TRUE &&
      equipData->isEnable == TRUE )
  {
    MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] ,
                  &pos );
    pos.x = ACT_POS_X_FX(pos.x);
    pos.y = ACT_POS_Y_FX(pos.y);
    pos.z += FX32_ONE;
  }
  else
  {
    VecFx32 *rotOfs = MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
    VEC_Add( &pokeWork->pokePos , rotOfs , &pos );
    pos.x = ACT_POS_X_FX(pos.x);
    pos.y = ACT_POS_Y_FX(pos.y);
    pos.z -= (FX32_ONE*28); //���ɏo������
  }
  
  STA_EFF_SetPosition( itemUseWork->effWork , 0 , &pos );

  itemUseWork->cnt++;
  if( itemUseWork->cnt >= STA_POKE_ITEMUSE_TIME )
  {
    STA_EFF_SYS  *effSys = STA_ACT_GetEffectSys( work->actWork );

    STA_EFF_DeleteEmitter( itemUseWork->effWork , 0 );
    STA_EFF_DeleteEffect( effSys , itemUseWork->effWork );
    pokeWork->updateItemUseFunc = NULL;
    work->useItemSeCnt--;
    if( work->useItemSeCnt == 0 )
    {
      PMSND_StopSE_byPlayerID(SEPLAYER_SE2);
    }
  }
  
}

void STA_POKE_InitItemUse_Flying( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( pokeWork->drawWork , pokeWork->itemUseWork.equipPos );

  pokeWork->isEnableItem[pokeWork->itemUseWork.equipPos] = FALSE;

  //�w�ʑΉ��E�A�b�p�[�����ӏ������Ƃ��Ή�
  if( pokeWork->isFront == FALSE ||
      equipData->isEnable == FALSE )
  {
    VecFx32 pos;
    VecFx32 *rotOfs = MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
    VEC_Add( &pokeWork->pokePos , rotOfs , &pos );
    pos.z -= (FX32_ONE*28); //���ɏo������
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] , &pos );
  }

  MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] , TRUE );
  MUS_ITEM_DRAW_SetRotation(  work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                0 );
  MUS_ITEM_DRAW_SetSize(    work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                FX32_ONE/4 , FX32_ONE/4);
  
}

void STA_POKE_UpdateItemUse_Flying( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  VecFx32 pos;
  const fx32 curveRate = /*FX32_CONST(0.2f)+*/(itemUseWork->cnt/3)*FX32_CONST(0.05f);
  const fx32 curveSin = FX_SinIdx( (itemUseWork->cnt * 0x200)%0x10000 );
  
  MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                &pos );

  pos.x += FX_Mul(curveRate,curveSin);
  pos.y -= STA_POKE_ITEMUSE_FLYING_SPD;

  MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                &pos );
  MUS_ITEM_DRAW_SetRotation(  work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                (0x800*curveSin)>>FX32_SHIFT );

  itemUseWork->cnt++;
  if( itemUseWork->cnt >= STA_POKE_ITEMUSE_TIME )
  {
    MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                    pokeWork->itemWork[itemUseWork->equipPos] , FALSE );
    pokeWork->updateItemUseFunc = NULL;
    work->useItemSeCnt--;
    if( work->useItemSeCnt == 0 )
    {
      PMSND_StopSE_byPlayerID(SEPLAYER_SE2);
    }
  }
}

void STA_POKE_InitItemUse_Throw( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  MUS_POKE_EQUIP_DATA *equipData = MUS_POKE_DRAW_GetEquipData( pokeWork->drawWork , pokeWork->itemUseWork.equipPos );

  pokeWork->isEnableItem[pokeWork->itemUseWork.equipPos] = FALSE;
  
  //�w�ʑΉ��E�A�b�p�[�����ӏ������Ƃ��Ή�
  if( pokeWork->isFront == FALSE ||
      equipData->isEnable == FALSE )
  {
    VecFx32 pos;
    VecFx32 *rotOfs = MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
    VEC_Add( &pokeWork->pokePos , rotOfs , &pos );
    //�����͌��ɏo���Ȃ��H
    //pos.z -= (FX32_ONE*28); //���ɏo������
    MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] , &pos );
  }

  itemUseWork->graAcc = STA_POKE_ITEMUSE_THROW_START_ACC;
  MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] , TRUE );
  MUS_ITEM_DRAW_SetRotation(  work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                0 );
  MUS_ITEM_DRAW_SetSize(    work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                FX32_ONE/4 , FX32_ONE/4);
}

void STA_POKE_UpdateItemUse_Throw( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;

  VecFx32 pos;
  fx16 sizeX,sizeY;
  
  MUS_ITEM_DRAW_GetPosition( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                &pos );
  
  itemUseWork->graAcc += STA_POKE_ITEMUSE_THROW_ADD_ACC;
  pos.x -= STA_POKE_ITEMUSE_THROW_SPD;
  pos.y += itemUseWork->graAcc;
  pos.z = FX32_CONST(200);

  MUS_ITEM_DRAW_SetPosition( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                &pos );

  MUS_ITEM_DRAW_GetSize( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                &sizeX , &sizeY );
  sizeX += STA_POKE_ITEMUSE_THROW_SIZE_ADD;
  sizeY += STA_POKE_ITEMUSE_THROW_SIZE_ADD;
  MUS_ITEM_DRAW_SetSize( work->itemDrawSys , 
                pokeWork->itemWork[itemUseWork->equipPos] ,
                sizeX , sizeY );

  itemUseWork->cnt++;
  if( itemUseWork->cnt >= STA_POKE_ITEMUSE_TIME )
  {
    MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , 
                    pokeWork->itemWork[itemUseWork->equipPos] , FALSE );
    pokeWork->updateItemUseFunc = NULL;
    work->useItemSeCnt--;
    if( work->useItemSeCnt == 0 )
    {
      PMSND_StopSE_byPlayerID(SEPLAYER_SE2);
    }
  }
}

void STA_POKE_InitItemUse_Use( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
}

void STA_POKE_UpdateItemUse_Use( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  STA_POKE_ITEMUSE_WORK *itemUseWork = &pokeWork->itemUseWork;
  const u16 rad = (0x800*itemUseWork->cnt)%0x10000;
  fx16 sizeX,sizeY;

  itemUseWork->cnt++;
  if( itemUseWork->cnt >= STA_POKE_ITEMUSE_TIME )
  {
    pokeWork->updateItemUseFunc = NULL;
    work->useItemSeCnt--;
    if( work->useItemSeCnt == 0 )
    {
      PMSND_StopSE_byPlayerID(SEPLAYER_SE2);
    }
  }
  else
  {
    sizeX = FX16_CONST(0.25f) + FX_SinIdx(rad)/8;
    sizeY = FX16_CONST(0.25f) + FX_CosIdx(rad)/8;
    //OS_Printf("[%.3f][%.3f]\n",FX_FX32_TO_F32(sizeX),FX_FX32_TO_F32(sizeY));
    MUS_ITEM_DRAW_SetSize( work->itemDrawSys , 
                  pokeWork->itemWork[itemUseWork->equipPos] ,
                  sizeX , sizeY );
  }
}




#pragma mark [>outer func
void STA_POKE_GetPosition( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *pos )
{
  pos->x = pokeWork->pokePos.x;
  pos->y = pokeWork->pokePos.y;
  pos->z = pokeWork->pokePos.z;
}

void STA_POKE_SetPosition( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const VecFx32 *pos )
{
  pokeWork->pokePos.x = pos->x;
  pokeWork->pokePos.y = pos->y;
  pokeWork->pokePos.z = pos->z;
  
  pokeWork->isUpdate = TRUE;
}

void STA_POKE_GetRotate( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , u16 *rotate )
{
  *rotate = pokeWork->rotate;
}

void STA_POKE_SetRotate( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const u16 rotate )
{
  pokeWork->rotate = rotate;
  
  if( pokeWork->isGround == TRUE )
  {
    pokeWork->rotate = 0;
  }

  pokeWork->isUpdate = TRUE;
}

void STA_POKE_GetScale( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *scale )
{
  scale->x = pokeWork->scale.x;
  scale->y = pokeWork->scale.y;
  scale->z = pokeWork->scale.z;
}

void STA_POKE_SetScale( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *scale )
{
  pokeWork->scale.x = scale->x;
  pokeWork->scale.y = scale->y;
  pokeWork->scale.z = scale->z;
  
  pokeWork->isUpdate = TRUE;
}
void STA_POKE_GetPositionOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , VecFx32 *pos )
{
  pos->x = pokeWork->posOfs.x;
  pos->y = pokeWork->posOfs.y;
  pos->z = pokeWork->posOfs.z;
}

void STA_POKE_SetPositionOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const VecFx32 *pos )
{
  pokeWork->posOfs.x = pos->x;
  pokeWork->posOfs.y = pos->y;
  pokeWork->posOfs.z = pos->z;

  if( pokeWork->isGround == TRUE )
  {
    pokeWork->posOfs.y = 0;
  }
  
  pokeWork->isUpdate = TRUE;
}

void STA_POKE_StartAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  MUS_POKE_DRAW_StartAnime( pokeWork->drawWork );
  pokeWork->mepachiFlg = TRUE;
}

void STA_POKE_StopAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  MUS_POKE_DRAW_StopAnime( pokeWork->drawWork );
  pokeWork->mepachiFlg = FALSE;
  MUS_POKE_DRAW_SetMepachiFlg( pokeWork->drawWork , FALSE );
}

void STA_POKE_ChangeAnime( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const u8 anmIdx )
{
  MUS_POKE_DRAW_ChangeAnime( pokeWork->drawWork , anmIdx );
}

void STA_POKE_SetShowFlg( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const BOOL flg )
{
  int ePos;
  MUS_POKE_DRAW_SetShowFlg( pokeWork->drawWork , flg );
  for( ePos=0;ePos<MUS_POKE_EQUIP_MAX;ePos++ )
  {
    if( pokeWork->itemWork[ePos] != NULL &&
        pokeWork->isEnableItem[ePos] == TRUE )
    {
      MUS_ITEM_DRAW_SetDrawEnable( work->itemDrawSys , pokeWork->itemWork[ePos] , flg );
    }
  }
  GFL_BBD_SetObjectDrawEnable( work->bbdSys , pokeWork->shadowBbdIdx , &flg );

}
BOOL STA_POKE_GetShowFlg( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  return MUS_POKE_DRAW_GetShowFlg( pokeWork->drawWork );
}

STA_POKE_DIR STA_POKE_GetPokeDir( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  return pokeWork->dir;
}

void STA_POKE_SetPokeDir( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const STA_POKE_DIR dir )
{
  if( pokeWork->isNoFlip == TRUE )
  {
    pokeWork->dir = SPD_LEFT;
  }
  else
  {
    pokeWork->dir = dir;
  }
  pokeWork->isUpdate = TRUE;
}

void STA_POKE_SetFrontBack( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const BOOL isFront )
{
  pokeWork->isFront = isFront;
  MUS_POKE_DRAW_SetFrontBack( pokeWork->drawWork , pokeWork->isFront );
  pokeWork->isUpdate = TRUE;
}

void STA_POKE_SetDrawItem( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork , const BOOL isDrawItem )
{
  pokeWork->isDrawItem = isDrawItem;
  pokeWork->isUpdate = TRUE;
}

const VecFx32* STA_POKE_GetRotOffset( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  return MUS_POKE_DRAW_GetRotateOfs( pokeWork->drawWork );
}


MUS_POKE_DRAW_WORK* STA_POKE_GetMusPokeWork( STA_POKE_SYS *work , STA_POKE_WORK *pokeWork )
{
  return pokeWork->drawWork;
}
MUS_POKE_DRAW_SYSTEM* STA_POKE_GetMusPokeSys( STA_POKE_SYS *work )
{
  return work->drawSys;
}
