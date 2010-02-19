//======================================================================
/**
 * @file	  mb_cap_poke.c
 * @brief	  �ߊl�Q�[���E�|�P����
 * @author	ariizumi
 * @data	  09/11/24
 *
 * ���W���[�����FMB_CAP_POKE
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "poke_tool/monsno_def.h"

#include "mb_capture_gra.naix"

#include "multiboot/mb_poke_icon.h"

#include "./mb_cap_local_def.h"
#include "./mb_cap_poke.h"
#include "./mb_cap_obj.h"
#include "./mb_cap_effect.h"

#include "./mb_cap_snd_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MB_CAP_POKE_OFS_Y (8)
#define MB_CAP_POKE_MOVE_OFS_Y (4)

#define MB_CAP_POKE_HIDE_LOOK_OFS (5)

#define MB_CAP_POKE_ANIM_SPEED (8)
#define MB_CAP_POKE_ANIM_FRAME (2)

#define MB_CAP_POKE_FALL_SPEED (FX32_HALF)
#define MB_CAP_POKE_DOWN_EFF_OFS (FX32_CONST(16))

#define MB_CAP_POKE_ESCAPE_TIME (600)   //��ʊO�ɓ����Ă��鎞��

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef void (*MB_CAP_POKE_STATE_FUNC)(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );

struct _MB_CAP_POKE
{
  MB_CAP_POKE_STATE state;
  MB_CAP_POKE_STATE_FUNC stateFunc;
  u16 cnt;  //�e�X�e�[�g���Ŏg���܂킹��J�E���^

  u16 anmFrame;
  u16 anmIdx;
  
  s8 posXidx;
  s8 posYidx;
  MB_CAP_POKE_DIR dir;
  VecFx32 startPos;
  VecFx32 pos;
  fx32 height;
  
  int resIdx;
  int resEdgeIdx;
  int objIdx;
  int objShadowIdx;
  int objEdgeIdx;
  
  MB_CAP_EFFECT *downEff;
  
  BOOL isGround;  //�f�B�O�_
  BOOL isNoFlip;  //���]�s��
};


//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void MB_CAP_POKE_SetEscape( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );

static void MB_CAP_POKE_StateNone(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );
static void MB_CAP_POKE_StateHide(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );
static void MB_CAP_POKE_StateRun(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );
static void MB_CAP_POKE_StateDown(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork );

//--------------------------------------------------------------
//	�I�u�W�F�N�g�쐬
//--------------------------------------------------------------
MB_CAP_POKE* MB_CAP_POKE_CreateObject( MB_CAPTURE_WORK *capWork , MB_CAP_POKE_INIT_WORK *initWork )
{
  const HEAPID heapId = MB_CAPTURE_GetHeapId( capWork );
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  ARCHANDLE *arcHandle = MB_CAPTURE_GetArcHandle( capWork );
  const DLPLAY_CARD_TYPE cardType = MB_CAPTURE_GetCardType( capWork );
  const int resShadowIdx = MB_CAPTURE_GetBbdResIdx( capWork , MCBR_SHADOW );

  MB_CAP_POKE *pokeWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MB_CAP_POKE ) );

  GFL_G3D_RES* res = GFL_G3D_CreateResourceHandle( arcHandle , NARC_mb_capture_gra_cap_poke_dummy_nsbtx );
  const BOOL flg = TRUE;
  pokeWork->pos.x = 0;
  pokeWork->pos.y = 0;
  pokeWork->pos.z = 0;
  GFL_G3D_TransVramTexture( res );
  pokeWork->resIdx = GFL_BBD_AddResource( bbdSys , 
                                       res , 
                                       GFL_BBD_TEXFMT_PAL16 ,
                                       GFL_BBD_TEXSIZDEF_32x64 ,
                                       32 , 32 );
  GFL_BBD_CutResourceData( bbdSys , pokeWork->resIdx );
  
  //�������o�̂��߃T�C�Y��2�{�ɂ��Ă�̂Ńf�t�H�͔���
  pokeWork->objIdx = GFL_BBD_AddObject( bbdSys , 
                                     pokeWork->resIdx ,
                                     FX32_HALF , 
                                     FX32_HALF , 
                                     &pokeWork->pos ,
                                     31 ,
                                     GFL_BBD_LIGHT_NONE );
  pokeWork->objShadowIdx = GFL_BBD_AddObject( bbdSys , 
                                     resShadowIdx ,
                                     FX32_HALF , 
                                     FX32_HALF , 
                                     &pokeWork->pos ,
                                     31 ,
                                     GFL_BBD_LIGHT_NONE );
  //3D�ݒ�̊֌W�Ŕ��]������E�E�E
  GFL_BBD_SetObjectFlipT( bbdSys , pokeWork->objIdx , &flg );

  //�ӂ�
#if MB_CAPTURE_USE_EDGE
  {
    NNSGfdTexKey texKey;
    GFL_BBD_GetResourceTexKey( bbdSys , pokeWork->resIdx , &texKey );
    pokeWork->resEdgeIdx = GFL_BBD_AddResourceKey(  bbdSys , 
                                       texKey , 
                                       initWork->edgePlttKey ,
                                       GFL_BBD_TEXFMT_PAL16 ,
                                       GFL_BBD_TEXSIZDEF_32x64 ,
                                       32 , 32 );
    pokeWork->objEdgeIdx = GFL_BBD_AddObject( bbdSys , 
                                       pokeWork->resEdgeIdx ,
                                       FX32_CONST(0.6f) , 
                                       FX32_CONST(0.6f) , 
                                       &pokeWork->pos ,
                                       31 ,
                                       GFL_BBD_LIGHT_NONE );
    GFL_BBD_SetObjectFlipT( bbdSys , pokeWork->objEdgeIdx , &flg );
  }
#endif //MB_CAPTURE_USE_EDGE

  //�A�C�R���f�[�^��VRAM�֑���
  {
    const u32 pltResNo = MB_ICON_GetPltResId( cardType );
    const u32 ncgResNo = MB_ICON_GetCharResId( initWork->ppp , cardType );
    const u8  palNo = MB_ICON_GetPalNumber( initWork->ppp );
    NNSG2dPaletteData *pltData;
    NNSG2dCharacterData *charData;
    void *pltRes = GFL_ARCHDL_UTIL_LoadPalette( initWork->pokeArcHandle , pltResNo , &pltData , GFL_HEAP_LOWID( heapId ) );
    void *ncgRes = GFL_ARCHDL_UTIL_LoadOBJCharacter( initWork->pokeArcHandle , ncgResNo , FALSE , &charData , GFL_HEAP_LOWID( heapId ) );
    u32 texAdr,pltAdr,pltDataAdr;
    GFL_BMP_DATA *bmpData = GFL_BMP_CreateWithData( charData->pRawData , 4 , 8 , GFL_BMP_16_COLOR , GFL_HEAP_LOWID( heapId ) );
    
    GFL_BMP_DataConv_to_BMPformat( bmpData , FALSE , GFL_HEAP_LOWID( heapId ) );
    
    GFL_BBD_GetResourceTexPlttAdrs( bbdSys , pokeWork->resIdx , &pltAdr );
    GFL_BBD_GetResourceTexDataAdrs( bbdSys , pokeWork->resIdx , &texAdr );
    
    GX_BeginLoadTex();
    DC_FlushRange( charData->pRawData , 32*64*0x20 );
    GX_LoadTex( charData->pRawData , texAdr , 32*64*0x20 );
    GX_EndLoadTex();

    pltDataAdr = (u32)pltData->pRawData + palNo*32;
    GX_BeginLoadTexPltt();
    DC_FlushRange( (void*)pltDataAdr , 32 );
    GX_LoadTexPltt( (void*)pltDataAdr , pltAdr , 32 );
    GX_EndLoadTexPltt();
    
    GFL_HEAP_FreeMemory( pltRes );
    GFL_HEAP_FreeMemory( ncgRes );
    GFL_BMP_Delete( bmpData );
  }

  pokeWork->state = MCPS_NONE;
  pokeWork->stateFunc = MB_CAP_POKE_StateNone;
  pokeWork->anmFrame = 0;
  pokeWork->anmIdx = 0;

  pokeWork->posXidx = 0xFF;
  pokeWork->posYidx = 0xFF;
  pokeWork->height = 0;
  
  pokeWork->downEff = NULL;
  
  pokeWork->isGround = FALSE;
  pokeWork->isNoFlip = FALSE;
  
  //�f�B�O�_�E���]�`�F�b�N
  {
    const u32 monsno = PPP_Get( initWork->ppp , ID_PARA_monsno , NULL );
    if( monsno == MONSNO_DHIGUDA ||
        monsno == MONSNO_DAGUTORIO )
    {
      pokeWork->isGround = TRUE;
    }
  }
  

  return pokeWork;
}

//--------------------------------------------------------------
//	�I�u�W�F�N�g�J��
//--------------------------------------------------------------
void MB_CAP_POKE_DeleteObject( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );

#if MB_CAPTURE_USE_EDGE
  GFL_BBD_RemoveObject( bbdSys , pokeWork->objEdgeIdx );
  GFL_BBD_RemoveResource( bbdSys , pokeWork->resEdgeIdx );
#endif //MB_CAPTURE_USE_EDGE
  GFL_BBD_RemoveObject( bbdSys , pokeWork->objShadowIdx );
  GFL_BBD_RemoveObject( bbdSys , pokeWork->objIdx );
  GFL_BBD_RemoveResource( bbdSys , pokeWork->resIdx );

  GFL_HEAP_FreeMemory( pokeWork );
}


//--------------------------------------------------------------
//	�I�u�W�F�N�g�X�V
//--------------------------------------------------------------
void MB_CAP_POKE_UpdateObject( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  pokeWork->stateFunc( capWork , pokeWork);
  
  //�A�j���̍X�V
  pokeWork->anmFrame++;
  if( pokeWork->anmFrame >= MB_CAP_POKE_ANIM_SPEED )
  {
    pokeWork->anmFrame = 0;
    pokeWork->anmIdx++;
    if( pokeWork->anmIdx >= MB_CAP_POKE_ANIM_FRAME )
    {
      pokeWork->anmIdx = 0;
    }
    GFL_BBD_SetObjectCelIdx( bbdSys , pokeWork->objIdx , &pokeWork->anmIdx );
  }
  
  //�g�̒Ǐ]
#if MB_CAPTURE_USE_EDGE
  {
    VecFx32 pos;
    BOOL isDisp = GFL_BBD_GetObjectDrawEnable( bbdSys , pokeWork->objIdx );
    BOOL isFlip = GFL_BBD_GetObjectFlipS( bbdSys , pokeWork->objIdx );
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objEdgeIdx , &isDisp );
    GFL_BBD_SetObjectFlipS( bbdSys , pokeWork->objEdgeIdx , &isFlip );

    GFL_BBD_GetObjectTrans( bbdSys , pokeWork->objIdx , &pos );
    //pos.z -= FX32_CONST(0.1f);
    GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objEdgeIdx , &pos );
    GFL_BBD_SetObjectCelIdx( bbdSys , pokeWork->objEdgeIdx , &pokeWork->anmIdx );
  }
#endif //MB_CAPTURE_USE_EDGE
  
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(�J�n���B��p
//--------------------------------------------------------------
void MB_CAP_POKE_SetHide( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork , const u8 idxX , const u8 idxY )
{
  pokeWork->cnt = GFUser_GetPublicRand0(MB_CAP_POKE_HIDE_TOTAL_TIME);
  pokeWork->posXidx = idxX;
  pokeWork->posYidx = idxY;
  
  pokeWork->state = MCPS_HIDE;
  pokeWork->stateFunc = MB_CAP_POKE_StateHide;
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(������
//--------------------------------------------------------------
void MB_CAP_POKE_SetRun( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  if( pokeWork->state == MCPS_LOOK )
  {
    if( pokeWork->cnt < MB_CAP_POKE_HIDE_LOOK_TIME )
    {
      pokeWork->dir = MCPD_LEFT;
    }
    else
    {
      pokeWork->dir = MCPD_RIGHT;
    }
    pokeWork->cnt = 0;
    pokeWork->stateFunc = MB_CAP_POKE_StateRun;
    //������̊�ʒu
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->startPos );
    pokeWork->startPos.y -= FX32_CONST( MB_CAP_POKE_MOVE_OFS_Y );
    pokeWork->startPos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );
  }
  else
  if( pokeWork->state == MCPS_ESCAPE )
  {
    //��ʊO��������Ԃ���̕��A
    pokeWork->cnt = 0;
    pokeWork->stateFunc = MB_CAP_POKE_StateRun;
    //������̊�ʒu
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->startPos );
    pokeWork->startPos.y -= FX32_CONST( MB_CAP_POKE_MOVE_OFS_Y );
    pokeWork->startPos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );
  }
  else
  {
    //�B��Ă��鎞�͂����ړ����n�߂�
    pokeWork->cnt = 0;
  }
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(������E���͂Ƀ{�[�����������ꍇ
//--------------------------------------------------------------
void MB_CAP_POKE_SetRunChangeDir( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork , const MB_CAP_POKE_DIR dir )
{
  pokeWork->dir = dir;
  pokeWork->cnt = 0;
  pokeWork->stateFunc = MB_CAP_POKE_StateRun;
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(�ߊl
//--------------------------------------------------------------
void MB_CAP_POKE_SetCapture( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  const BOOL isDisp = FALSE;
  GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &isDisp );
  GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objShadowIdx , &isDisp );

  pokeWork->state = MCPS_CAPTURE;
  pokeWork->stateFunc = MB_CAP_POKE_StateNone;
  
  if( pokeWork->downEff != NULL )
  {
    MB_CAP_EFFECT_SetIsFinish( pokeWork->downEff , TRUE );
    pokeWork->downEff = NULL;
  }

}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(�_�E��
//--------------------------------------------------------------
void MB_CAP_POKE_SetDown( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  pokeWork->state = MCPS_DOWN_MOVE;
  pokeWork->stateFunc = MB_CAP_POKE_StateDown;
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(����E�_�E���ƃX�e�[�g����
//--------------------------------------------------------------
void MB_CAP_POKE_SetSleep( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  if( pokeWork->state == MCPS_HIDE ||
      pokeWork->state == MCPS_LOOK )
  {
    VecFx32 effPos = pokeWork->pos;
    effPos.y -= MB_CAP_POKE_DOWN_EFF_OFS;
    effPos.z = FX32_CONST(MB_CAP_EFFECT_ZZZ_Z);
    pokeWork->downEff = MB_CAPTURE_CreateEffect( capWork , &effPos , MCET_ZZZ );
    MB_CAP_EFFECT_SetIsLoop( pokeWork->downEff , TRUE );
    pokeWork->state = MCPS_SLEEP_WAIT_GRASS;
  }
  else
  {
    pokeWork->state = MCPS_SLEEP_FALL;
  }
  pokeWork->cnt = 0;
  pokeWork->stateFunc = MB_CAP_POKE_StateDown;
}

//--------------------------------------------------------------
//	�|�P�����Z�b�g(������
//--------------------------------------------------------------
static void MB_CAP_POKE_SetEscape( MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  pokeWork->state = MCPS_ESCAPE;
  pokeWork->stateFunc = MB_CAP_POKE_StateNone;
  pokeWork->cnt = MB_CAP_POKE_ESCAPE_TIME;
}

#pragma mark [>state func
//--------------------------------------------------------------
//	�X�e�[�g�F�����E�ߊl�E�����Ă���
//--------------------------------------------------------------
static void MB_CAP_POKE_StateNone(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  if( pokeWork->state == MCPS_ESCAPE )
  {
    if( pokeWork->cnt > 0 )
    {
      pokeWork->cnt--;
      if( GFL_UI_KEY_GetCont() & PAD_BUTTON_A )
      {
        pokeWork->cnt = 0;
      }
    }
    else
    {
      if( GFUser_GetPublicRand0(2) == 0 )
      {
        pokeWork->posXidx = -1;
        pokeWork->dir = MCPD_RIGHT;
      }
      else
      {
        pokeWork->posXidx = MB_CAP_OBJ_X_NUM;
        pokeWork->dir = MCPD_LEFT;
      }
      pokeWork->posYidx = GFUser_GetPublicRand0(MB_CAP_OBJ_Y_NUM);
      MB_CAP_POKE_SetRun( capWork , pokeWork );
      MB_TPrintf("ReturnPoke!\n");
    }
  }
}

//--------------------------------------------------------------
//	�X�e�[�g�F�B��Ă���
//--------------------------------------------------------------
static void MB_CAP_POKE_StateHide(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  //�X�e�[�g�̕ύX�� == �ɂ��Ă���̂́A�����ݒ�l�ł����Ȃ�o�Ȃ��悤��
  pokeWork->cnt++;
  if( pokeWork->cnt >= MB_CAP_POKE_HIDE_TOTAL_TIME )
  {
    //���ɏo��
    const BOOL flg = TRUE;
    const BOOL flipFlg = FALSE;
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->pos );
    pokeWork->pos.x -= FX32_CONST( MB_CAP_POKE_HIDE_LOOK_OFS );
    pokeWork->pos.y -= FX32_CONST( MB_CAP_POKE_OFS_Y );
    pokeWork->pos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &flg );
    GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objIdx , &pokeWork->pos );
    GFL_BBD_SetObjectFlipS( bbdSys , pokeWork->objIdx , &flipFlg );

    pokeWork->cnt = 0;
    pokeWork->state = MCPS_LOOK;
  }
  else
  if( pokeWork->cnt == MB_CAP_POKE_HIDE_LOOK_TIME+MB_CAP_POKE_HIDE_HIDE_TIME )
  {
    //�E�ɏo��
    const BOOL flg = TRUE;
    const BOOL flipFlg = ( pokeWork->isNoFlip == FALSE ? TRUE : FALSE );
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->pos );
    pokeWork->pos.x += FX32_CONST( MB_CAP_POKE_HIDE_LOOK_OFS );
    pokeWork->pos.y -= FX32_CONST( MB_CAP_POKE_OFS_Y );
    pokeWork->pos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &flg );
    GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objIdx , &pokeWork->pos );
    GFL_BBD_SetObjectFlipS( bbdSys , pokeWork->objIdx , &flipFlg );

    pokeWork->state = MCPS_LOOK;

  }
  else
  if( pokeWork->cnt == MB_CAP_POKE_HIDE_LOOK_TIME ||
      pokeWork->cnt == MB_CAP_POKE_HIDE_LOOK_TIME*2+MB_CAP_POKE_HIDE_HIDE_TIME )
  {
    //�B���
    const BOOL flg = FALSE;
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->pos );
    pokeWork->pos.y -= FX32_CONST( MB_CAP_POKE_OFS_Y );
    pokeWork->pos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &flg );
    GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objIdx , &pokeWork->pos );

    pokeWork->state = MCPS_HIDE;
  }
}

//--------------------------------------------------------------
//	�X�e�[�g�F������
//--------------------------------------------------------------
static void MB_CAP_POKE_StateRun(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  pokeWork->cnt++;
  if( pokeWork->cnt == 1)
  {
    PMSND_PlaySE( MB_SND_POKE_JUMP );
  }
  if( pokeWork->cnt < MB_CAP_POKE_RUN_LOOK_TIME )
  {
    //���
    const BOOL isDisp = TRUE;
    const BOOL isDispShadow = (pokeWork->isGround==FALSE?TRUE:FALSE);
    BOOL flipFlg = FALSE;
    VecFx32 dispPos;
    
    {
      //�W�����v�̌v�Z
      const u16 rot = 0x8000*pokeWork->cnt/MB_CAP_POKE_RUN_LOOK_TIME;
      fx32 ofs;
      VecFx32 nextPos;
      pokeWork->height = FX_SinIdx( rot ) * MB_CAP_POKE_JUMP_HEIGHT;
      switch( pokeWork->dir )
      {
      case MCPD_LEFT:
        //������̊�ʒu
        MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx-1 , pokeWork->posYidx , &nextPos );
        ofs = nextPos.x - pokeWork->startPos.x;
        pokeWork->pos.x = pokeWork->startPos.x + ofs*pokeWork->cnt/MB_CAP_POKE_RUN_LOOK_TIME;
        pokeWork->pos.y = pokeWork->startPos.y;
        pokeWork->pos.z = pokeWork->startPos.z;
        break;
      case MCPD_RIGHT:
        //������̊�ʒu
        MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx+1 , pokeWork->posYidx , &nextPos );
        ofs = nextPos.x - pokeWork->startPos.x;
        pokeWork->pos.x = pokeWork->startPos.x + ofs*pokeWork->cnt/MB_CAP_POKE_RUN_LOOK_TIME;
        pokeWork->pos.y = pokeWork->startPos.y;
        pokeWork->pos.z = pokeWork->startPos.z;
        flipFlg = ( pokeWork->isNoFlip == FALSE ? TRUE : FALSE );
        break;
      case MCPD_UP:
        MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx-1 , &nextPos );
        if( pokeWork->posYidx <= 0 )
        {
          //�ŏ�i�␳�I�I�I
          nextPos.y = FX32_CONST(MB_CAP_OBJ_MAIN_TOP-8);
        }
        ofs = (nextPos.y - FX32_CONST( MB_CAP_POKE_MOVE_OFS_Y )) - pokeWork->startPos.y;
        pokeWork->pos.x = pokeWork->startPos.x;
        pokeWork->pos.y = pokeWork->startPos.y + ofs*pokeWork->cnt/MB_CAP_POKE_RUN_LOOK_TIME;

        if( pokeWork->posYidx <= 0 &&
            pokeWork->cnt > MB_CAP_POKE_RUN_LOOK_TIME/2 )
        {
          //�ŏ�i�␳�I�I�I
          pokeWork->pos.z = FX32_CONST(MB_CAP_OBJ_BASE_Z - MB_CAP_OBJ_LINEOFS_Z - MB_CAP_OBJ_LINEOFS_Z/2);
        }
        else
        {
          pokeWork->pos.z = pokeWork->startPos.z;
        }
        flipFlg = ( pokeWork->isNoFlip == FALSE ? TRUE : FALSE );
        break;
      case MCPD_DOWN:
        MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx+1 , &nextPos );
        ofs = (nextPos.y - FX32_CONST( MB_CAP_POKE_MOVE_OFS_Y )) - pokeWork->startPos.y;
        pokeWork->pos.x = pokeWork->startPos.x;
        pokeWork->pos.y = pokeWork->startPos.y + ofs*pokeWork->cnt/MB_CAP_POKE_RUN_LOOK_TIME;
        pokeWork->pos.z = pokeWork->startPos.z;
        //���ړ��̂Ƃ�����Z�␳
        pokeWork->pos.z += FX32_CONST( MB_CAP_OBJ_LINEOFS_Z );
        break;
      }
      //�f�B�O�_
      if( pokeWork->isGround == TRUE )
      {
        if( pokeWork->dir == MCPD_UP && 
            pokeWork->posYidx <= 0 )
        {
          //�ŏ�i�␳(�e�������Ă���̂ŏ�ړ��Ɍ���������
          pokeWork->height = FX_SinIdx( rot ) * (MB_CAP_POKE_JUMP_HEIGHT-8);
        }
        else
        {
          pokeWork->height = 0;
        }
      }
    
      dispPos.x = pokeWork->pos.x;
      dispPos.y = pokeWork->pos.y+FX32_CONST(6.0f);
      dispPos.z = pokeWork->pos.z;

      GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objShadowIdx , &isDispShadow );
      GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objShadowIdx , &dispPos );
      
      dispPos.y = pokeWork->pos.y-pokeWork->height;

      GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &isDisp );
      GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objIdx , &dispPos );
      GFL_BBD_SetObjectFlipS( bbdSys , pokeWork->objIdx , &flipFlg );
    }

    pokeWork->state = MCPS_RUN_LOOK;
    
  }
  else
  if( pokeWork->cnt == MB_CAP_POKE_RUN_LOOK_TIME )
  {
    BOOL isCrossPoke = FALSE;
    const BOOL flg = FALSE;
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objShadowIdx , &flg );
    GFL_BBD_SetObjectDrawEnable( bbdSys , pokeWork->objIdx , &flg );
    switch( pokeWork->dir )
    {
    case MCPD_LEFT:
      pokeWork->posXidx--;
      break;
    case MCPD_RIGHT:
      pokeWork->posXidx++;
      break;
    case MCPD_UP:
      pokeWork->posYidx--;
      break;
    case MCPD_DOWN:
      pokeWork->posYidx++;
      break;
    }
    
    //OBJ�A�j���̃��N�G�X�g
    {
      MB_CAP_OBJ *objWork;
      u8 objIdx;
      if( pokeWork->posXidx == -1 )
      {
        objIdx = MB_CAP_OBJ_SUB_L_START+pokeWork->posYidx;
      }
      else
      if( pokeWork->posXidx == MB_CAP_OBJ_X_NUM )
      {
        objIdx = MB_CAP_OBJ_SUB_R_START+pokeWork->posYidx;
      }
      else
      if( pokeWork->posYidx == -1 )
      {
        //�؂͗h�炳�Ȃ�
        objIdx = 0xFF;
      }
      else
      if( pokeWork->posYidx == MB_CAP_OBJ_Y_NUM )
      {
        objIdx = MB_CAP_OBJ_SUB_D_START+pokeWork->posXidx;
      }
      else
      {
        objIdx = pokeWork->posXidx + pokeWork->posYidx*MB_CAP_OBJ_X_NUM;
      }
      if( objIdx != 0xFF )
      {
        objWork = MB_CAPTURE_GetObjWork( capWork , objIdx );
        if( objWork != NULL )
        {
          MB_CAP_OBJ_StartAnime( objWork );
        }
      }
    }
  
    
    //������Ƀ|�P�����邩�`�F�b�N
    {
      u8 i;
      for( i=0;i<MB_CAP_POKE_NUM;i++ )
      {
        MB_CAP_POKE *checkPoke = MB_CAPTURE_GetPokeWork( capWork , i );
        if( checkPoke != pokeWork )
        {
          if( pokeWork->posXidx == checkPoke->posXidx &&
              pokeWork->posYidx == checkPoke->posYidx )
          {
            if( checkPoke->state == MCPS_HIDE ||
                checkPoke->state == MCPS_LOOK ||
                checkPoke->state == MCPS_RUN_HIDE )
            {
              isCrossPoke = TRUE;
            }
          }
        }
      }
    }
    
    if( isCrossPoke == FALSE )
    {
      pokeWork->state = MCPS_RUN_HIDE;
      
      if( pokeWork->posXidx <= -1 ||
          pokeWork->posXidx >= MB_CAP_OBJ_X_NUM ||
          pokeWork->posYidx <= -1 ||
          pokeWork->posYidx >= MB_CAP_OBJ_Y_NUM )
      {
        MB_CAP_POKE_SetEscape( capWork , pokeWork );
      }
    }
    else
    {
      //���v���ɕ����]��
      pokeWork->cnt = 0;
      switch( pokeWork->dir )
      {
      case MCPD_LEFT:
        pokeWork->dir = MCPD_UP;
        break;
      case MCPD_RIGHT:
        pokeWork->dir = MCPD_DOWN;
        break;
      case MCPD_UP:
        pokeWork->dir = MCPD_RIGHT;
        break;
      case MCPD_DOWN:
        pokeWork->dir = MCPD_LEFT;
        break;
      }
    }
    
    
    MB_CAPTURE_GetGrassObjectPos( pokeWork->posXidx , pokeWork->posYidx , &pokeWork->startPos );
    pokeWork->startPos.y -= FX32_CONST( MB_CAP_POKE_MOVE_OFS_Y );
    pokeWork->startPos.z -= FX32_CONST( MB_CAP_OBJ_BASE_Z-MB_CAP_POKE_BASE_Z );

    PMSND_PlaySE_byPlayerID( MB_SND_GRASS_SHAKE , SEPLAYER_SE2 );
  }
  else
  if( pokeWork->cnt >= MB_CAP_POKE_RUN_LOOK_TIME+MB_CAP_POKE_RUN_HIDE_TIME )
  {
    pokeWork->cnt = 0;
  }
}

//--------------------------------------------------------------
//	�X�e�[�g�F�_�E���I(����
//--------------------------------------------------------------
static void MB_CAP_POKE_StateDown(MB_CAPTURE_WORK *capWork , MB_CAP_POKE *pokeWork )
{
  GFL_BBD_SYS *bbdSys = MB_CAPTURE_GetBbdSys( capWork );
  if( pokeWork->state == MCPS_DOWN_MOVE ||
      pokeWork->state == MCPS_SLEEP_FALL )
  {
    VecFx32 pos;
    if( pokeWork->height <= MB_CAP_POKE_FALL_SPEED )
    {
      VecFx32 effPos = pokeWork->pos;
      effPos.y -= MB_CAP_POKE_DOWN_EFF_OFS;

      pokeWork->height = 0;
      pokeWork->cnt = 0;
      if( pokeWork->state == MCPS_DOWN_MOVE )
      {
        effPos.z = FX32_CONST(MB_CAP_EFFECT_Z);
        pokeWork->downEff = MB_CAPTURE_CreateEffect( capWork , &effPos , MCET_DOWN );
        MB_CAP_EFFECT_SetIsLoop( pokeWork->downEff , TRUE );
        pokeWork->state = MCPS_DOWN_WAIT;
      }
      else
      {
        effPos.z = FX32_CONST(MB_CAP_EFFECT_ZZZ_Z);
        pokeWork->downEff = MB_CAPTURE_CreateEffect( capWork , &effPos , MCET_ZZZ );
        MB_CAP_EFFECT_SetIsLoop( pokeWork->downEff , TRUE );
        pokeWork->state = MCPS_SLEEP_WAIT;
      }
    }
    else
    {
      pokeWork->height -= MB_CAP_POKE_FALL_SPEED;
    }
    pos.x = pokeWork->pos.x;
    pos.y = pokeWork->pos.y - pokeWork->height;
    pos.z = pokeWork->pos.z + FX32_ONE;
    GFL_BBD_SetObjectTrans( bbdSys , pokeWork->objIdx , &pos );
  }
  else
  {
    const u16 downTime = (pokeWork->state == MCPS_DOWN_WAIT ? MB_CAP_POKE_DOWN_TIME : MB_CAP_BONUSTIME );
    pokeWork->cnt++;
    if( pokeWork->cnt > downTime )
    {
      if( pokeWork->state == MCPS_DOWN_WAIT )
      {
        //�Ԃ����ă_�E���������̂�
        //�����]�����Ĉړ�
        switch( pokeWork->dir )
        {
        case MCPD_LEFT:
          pokeWork->dir = MCPD_RIGHT;
          break;
        case MCPD_RIGHT:
          pokeWork->dir = MCPD_LEFT;
          break;
        case MCPD_UP:
          pokeWork->dir = MCPD_DOWN;
          break;
        case MCPD_DOWN:
          pokeWork->dir = MCPD_UP;
          break;
        }
      }

      if( pokeWork->state == MCPS_DOWN_WAIT ||
          pokeWork->state == MCPS_SLEEP_WAIT )
      {
        //�_�E���ƈړ����̐����̂�
        pokeWork->startPos = pokeWork->pos;
        pokeWork->cnt = 0;
        pokeWork->stateFunc = MB_CAP_POKE_StateRun;
      }
      else
      {
        MB_CAP_POKE_SetHide( capWork , pokeWork , pokeWork->posXidx , pokeWork->posYidx );
      }

      MB_CAP_EFFECT_SetIsFinish( pokeWork->downEff , TRUE );
      pokeWork->downEff = NULL;
    }
  }
}

#pragma mark [>outer func
//--------------------------------------------------------------
//�w����W�ɂ��邩�H
//--------------------------------------------------------------
const BOOL MB_CAP_POKE_CheckPos( const MB_CAP_POKE *pokeWork , const u8 idxX , const u8 idxY )
{
  if( pokeWork->posXidx == idxX &&
      pokeWork->posYidx == idxY )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//
//--------------------------------------------------------------
const MB_CAP_POKE_STATE MB_CAP_POKE_GetState( const MB_CAP_POKE *pokeWork )
{
  return pokeWork->state;
}

//--------------------------------------------------------------
//  �����蔻��쐬
//--------------------------------------------------------------
void MB_CAP_POKE_GetHitWork( MB_CAP_POKE *pokeWork , MB_CAP_HIT_WORK *hitWork )
{
  hitWork->pos = &pokeWork->pos;
  hitWork->height = pokeWork->height;
  hitWork->size.x = FX32_CONST(MB_CAP_POKE_HITSIZE_X);
  hitWork->size.y = FX32_CONST(MB_CAP_POKE_HITSIZE_Y);
  hitWork->size.z = FX32_CONST(MB_CAP_POKE_HITSIZE_Z);
}

//--------------------------------------------------------------
//  �ߊl�\�ȃX�e�[�g���H
//--------------------------------------------------------------
const BOOL MB_CAP_POKE_CheckCanCapture( MB_CAP_POKE *pokeWork )
{
  if( pokeWork->state == MCPS_RUN_LOOK ||
      pokeWork->state == MCPS_DOWN_MOVE ||
      pokeWork->state == MCPS_DOWN_WAIT ||
      pokeWork->state == MCPS_SLEEP_FALL ||
      pokeWork->state == MCPS_SLEEP_WAIT ||
      pokeWork->state == MCPS_SLEEP_WAIT_GRASS )
  {
    return TRUE;
  }
  return FALSE;
}
