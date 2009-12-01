//======================================================================
/**
 * @file	  mb_cap_down.h
 * @brief	  �ߊl�Q�[���E�����
 * @author	ariizumi
 * @data	  09/11/26
 *
 * ���W���[�����FMB_CAP_DOWN
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "mb_capture_gra.naix"

#include "multiboot/mb_poke_icon.h"

#include "./mb_cap_down.h"
#include "./mb_cap_snd_def.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CAP_DOWN_BOW_X (128)
#define MB_CAP_DOWN_BOW_Y (62)

#define MB_CAP_DOWN_BALL_X (128)
#define MB_CAP_DOWN_BALL_Y (88)

#define MB_CAP_DOWN_BALL_SUPPLY_X (128)
#define MB_CAP_DOWN_BALL_SUPPLY_Y (-32)

//�|�̒��S���猷�̊J�n�ʒu�܂ł̋���
#define MB_CAP_DOWN_BOW_LINE_X (84)
#define MB_CAP_DOWN_BOW_LINE_Y (26)

#define MB_CAP_DOWN_DEF_BALL_LEN (MB_CAP_DOWN_BALL_Y-MB_CAP_DOWN_BOW_Y)

//�| ����n���l
#define MB_CAP_DOWN_BOWBEND_SCALE_MAX (FX32_HALF)

//�^�b�`���蔼�a
#define MB_CAP_DOWN_BALL_TOUCH_LEN (8)

//�{�[�����x
#define MB_CAP_DOWN_BALL_SHOT_SPD (16)
#define MB_CAP_DOWN_BALL_SUPPLY_SPD (12)
#define MB_CAP_DOWN_BOW_SUPPLY_ROT (0x600)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
//OBJ���\�[�X�̎��
typedef enum
{
  MCDO_PLT,
  MCDO_NCG,
  MCDO_ANM,
  
  MCDO_MAX,
}MB_CAP_DOWN_OBJRES;

//OBJ�A�j�����
typedef enum
{
  MCDA_NUMBER,
  MCDA_BALL,
  MCDA_BALL_BONUS,
  MCDA_BALL_GET,
  MCDA_BALL_PEN,
  
  MCDA_MAX,
}MB_CAP_DOWN_ANMTYPE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_CAP_DOWN
{
  BOOL isTrg;
  BOOL isTouch;
  BOOL isUpdateBall;
  BOOL isPlayBowPullSnd;
  
  MB_CAP_DOWN_STATE state;
  
  u32 cellRes[MCDO_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkBall;
  
  //�{�[�����W�͐��x�m�ۂ̂���FX32��
  fx32 ballPosX;
  fx32 ballPosY;
  fx32 pullLen;
  u16  rotAngle;
  fx32 shotLen; //�������u�Ԃ̒l
  u16  shotAngle;
  
  fx32 shotBallSpdX;
  fx32 shotBallSpdY;
  
  //�|���痣�ꂽ���H
  BOOL isFlying;
  
  //�|�֌W
  fx32 pullBow;   //���������
  GFL_BMP_DATA *lineBmp;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void MB_CAP_DOWN_UpdateBow( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork );
static void MB_CAP_DOWN_UpdateBow_DrawLine( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork );
static void MB_CAP_DOWN_UpdateBow_DrawLine_Func( const u8* vramAdr , const int x1 , const int y1 , const int x2 , const int y2 );
static void MB_CAP_DOWN_UpdateBow_DrawDot( const u8* vramAdr , const int x , const int y );
static void MB_CAP_DOWN_UpdateBall( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork );
static void MB_CAP_DOWN_UpdateTP( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork , u32 tpx , u32 tpy );

//--------------------------------------------------------------
//	����ʍ쐬
//--------------------------------------------------------------
MB_CAP_DOWN* MB_CAP_DOWN_InitSystem( MB_CAPTURE_WORK *capWork )
{
  const HEAPID heapId = MB_CAPTURE_GetHeapId( capWork );
  ARCHANDLE *arcHandle = MB_CAPTURE_GetArcHandle( capWork );
  MB_CAP_DOWN *downWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MB_CAP_DOWN ) );
  //�����
  //�p���b�g�͋|����W�J���邱��
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_mb_capture_gra_cap_bow_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_capture_gra_cap_bow_NCGR ,
                    MB_CAPTURE_FRAME_SUB_BOW , 0 , 0, FALSE , heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_capture_gra_cap_bow_NSCR , 
                    MB_CAPTURE_FRAME_SUB_BOW ,  0 , 0, FALSE , heapId );

  GFL_ARCHDL_UTIL_TransVramPaletteEx( arcHandle , NARC_mb_capture_gra_cap_bgd_NCLR , 
                    PALTYPE_SUB_BG , 0 , 32*MB_CAPTURE_PAL_SUB_BG, 32 , heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_mb_capture_gra_cap_bgd_NCGR ,
                    MB_CAPTURE_FRAME_SUB_BG , 0 , 0, FALSE , heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_mb_capture_gra_cap_bgd_NSCR , 
                    MB_CAPTURE_FRAME_SUB_BG ,  0 , 0, FALSE , heapId );

  GFL_BG_ChangeScreenPalette( MB_CAPTURE_FRAME_SUB_BG , 0 , 0 , 32 , 24 , MB_CAPTURE_PAL_SUB_BG );

  GFL_BG_LoadScreenV_Req( MB_CAPTURE_FRAME_SUB_BG );
  GFL_BG_LoadScreenV_Req( MB_CAPTURE_FRAME_SUB_BOW );
  
  GFL_BG_SetRotateCenterReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_CENTER_X_SET , MB_CAP_DOWN_BOW_X );
  GFL_BG_SetRotateCenterReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_CENTER_Y_SET , MB_CAP_DOWN_BOW_Y );
  
  //�Z��
  {
    downWork->cellRes[MCDO_PLT] = GFL_CLGRP_PLTT_RegisterEx( arcHandle , 
          NARC_mb_capture_gra_cap_objd_NCLR , CLSYS_DRAW_SUB , 
          MB_CAPTURE_PAL_SUB_OBJ_MAIN*32 , 0 , MB_CAPTURE_PAL_SUB_OBJ_MAIN_NUM+1 , heapId  );
    downWork->cellRes[MCDO_NCG] = GFL_CLGRP_CGR_Register( arcHandle , 
          NARC_mb_capture_gra_cap_objd_NCGR , FALSE , CLSYS_DRAW_SUB , heapId  );
    downWork->cellRes[MCDO_ANM] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
          NARC_mb_capture_gra_cap_objd_NCER , NARC_mb_capture_gra_cap_objd_NANR, heapId  );

    //TODO ���͓K��
    downWork->cellUnit  = GFL_CLACT_UNIT_Create( 96 , 0, heapId );
    GFL_CLACT_UNIT_SetDefaultRend( downWork->cellUnit );

  }
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 0;
    cellInitData.pos_x = MB_CAP_DOWN_BALL_X;
    cellInitData.pos_y = MB_CAP_DOWN_BALL_Y;
    cellInitData.anmseq = MCDA_BALL;

    downWork->clwkBall = GFL_CLACT_WK_Create( downWork->cellUnit ,
              downWork->cellRes[MCDO_NCG],
              downWork->cellRes[MCDO_PLT],
              downWork->cellRes[MCDO_ANM],
              &cellInitData ,CLSYS_DEFREND_SUB , heapId );
  }
  
  downWork->pullBow = 0;
  downWork->state = MCDS_NONE;
  downWork->ballPosX = MB_CAP_DOWN_BALL_X;
  downWork->ballPosY = MB_CAP_DOWN_BALL_Y;
  downWork->isUpdateBall = TRUE;
  downWork->isPlayBowPullSnd = FALSE;
  
  downWork->lineBmp = GFL_BMP_CreateInVRAM( G2S_GetBG1CharPtr() , 32 , 24 , GFL_BMP_16_COLOR , heapId );
  {
    u8 x,y;
    for( x=0;x<32;x++ )
    {
      for(y=0;y<24;y++)
      {
        u16 data = x+y*32;
        GFL_BG_WriteScreen( MB_CAPTURE_FRAME_SUB_BOW_LINE , &data , x,y,1,1 );
      }
    }
  }
  GFL_BG_LoadScreenV_Req( MB_CAPTURE_FRAME_SUB_BOW_LINE );
  
  return downWork;
}

//--------------------------------------------------------------
//	����ʊJ��
//--------------------------------------------------------------
void MB_CAP_DOWN_DeleteSystem( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  GFL_BMP_Delete( downWork->lineBmp );
  
  GFL_CLACT_WK_Remove( downWork->clwkBall );
  GFL_CLGRP_PLTT_Release( downWork->cellRes[MCDO_PLT] );
  GFL_CLGRP_CGR_Release( downWork->cellRes[MCDO_NCG] );
  GFL_CLGRP_CELLANIM_Release( downWork->cellRes[MCDO_ANM] );

  GFL_CLACT_UNIT_Delete( downWork->cellUnit );

  GFL_HEAP_FreeMemory( downWork );
}

//--------------------------------------------------------------
//	����ʍX�V
//--------------------------------------------------------------
void MB_CAP_DOWN_UpdateSystem( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  u32 tpx,tpy;
  downWork->isTrg = GFL_UI_TP_GetTrg();
  downWork->isTouch = GFL_UI_TP_GetPointCont( &tpx,&tpy );
  MB_CAP_DOWN_UpdateTP( capWork , downWork , tpx , tpy );
  MB_CAP_DOWN_UpdateBall( capWork , downWork );
  MB_CAP_DOWN_UpdateBow( capWork , downWork );
  
}

//--------------------------------------------------------------
//	�|�̍X�V
//--------------------------------------------------------------
static void MB_CAP_DOWN_UpdateBow( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  BOOL isUpdate = FALSE;

  if( downWork->state == MCDS_DRAG ||
      downWork->state == MCDS_SHOT_WAIT )
  {
    //����v�Z
    if( downWork->pullLen > MB_CAP_DOWN_BOW_PULL_LEN_MAX )
    {
      downWork->pullBow = MB_CAP_DOWN_BOWBEND_SCALE_MAX;
    }
    else
    {
      const fx32 len_rate = FX_Div(downWork->pullLen,MB_CAP_DOWN_BOW_PULL_LEN_MAX);
      downWork->pullBow = FX_Mul( MB_CAP_DOWN_BOWBEND_SCALE_MAX , len_rate );
    }
    isUpdate = TRUE;
  }
  else
  if( downWork->state == MCDS_SUPPLY_BALL )
  {
    if( downWork->rotAngle < 0x8000 )
    {
      downWork->rotAngle -= MB_CAP_DOWN_BOW_SUPPLY_ROT;
      if( downWork->rotAngle > 0x8000 )
      {
        downWork->rotAngle = 0;
      }
    }
    else
    if( downWork->rotAngle > 0x8000 )
    {
      downWork->rotAngle += MB_CAP_DOWN_BOW_SUPPLY_ROT;
      if( downWork->rotAngle < 0x8000 )
      {
        downWork->rotAngle = 0;
      }
    }
    isUpdate = TRUE;
  }
  else
  {
    downWork->rotAngle = 0;
    downWork->pullLen = 0;
    downWork->pullBow = 0;
    isUpdate = TRUE;
  }
  
  
  if( isUpdate == TRUE )
  {
    const int rotBowTemp = downWork->rotAngle*360/0x10000;
    const u16 rotBow = (rotBowTemp<0 ? rotBowTemp+360 : rotBowTemp);
    const fx32 xScale = FX32_ONE - downWork->pullBow;
    const fx32 yScale = FX32_ONE + downWork->pullBow;
/*
    MtxFx22 mtx;
    mtx._00 = FX_Inv(xScale);
    mtx._01 = 0;
    mtx._10 = 0;
    mtx._11 = FX_Inv(yScale);
    
    G2S_SetBG3Affine(&mtx,MB_CAP_DOWN_BOW_X,MB_CAP_DOWN_BOW_Y,0,0);
*/
    
    GFL_BG_SetRadianReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_RADION_SET , rotBow );
    GFL_BG_SetScaleReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_SCALE_X_SET , xScale );
    GFL_BG_SetScaleReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_SCALE_Y_SET , yScale );
  }
  MB_CAP_DOWN_UpdateBow_DrawLine( capWork , downWork );
}

//--------------------------------------------------------------
//	��������
//--------------------------------------------------------------
static void MB_CAP_DOWN_UpdateBow_DrawLine( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  u8 *vramAdr = GFL_BMP_GetCharacterAdrs( downWork->lineBmp );
  GFL_BMP_Clear( downWork->lineBmp , 0 );
  
  {
    //�g�k�Ή�
    const u8 subBowX = (MB_CAP_DOWN_BOW_LINE_X * (FX32_ONE - downWork->pullBow))>>FX32_SHIFT;
    const u8 subBowY = (MB_CAP_DOWN_BOW_LINE_Y * (FX32_ONE + downWork->pullBow))>>FX32_SHIFT;
    const fx16 sin = FX_SinIdx( downWork->rotAngle );
    const fx16 cos = FX_CosIdx( downWork->rotAngle );

    const int subX1 = ((subBowX*cos)-(subBowY*sin))>>FX16_SHIFT;
    const int subY1 = ((subBowX*sin)+(subBowY*cos))>>FX16_SHIFT;
    const int subX2 = ((-subBowX*cos)-(subBowY*sin))>>FX16_SHIFT;
    const int subY2 = ((-subBowX*sin)+(subBowY*cos))>>FX16_SHIFT;
    
    if( downWork->isFlying == FALSE )
    {
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr ,
                                           MB_CAP_DOWN_BOW_X + subX1 ,
                                           MB_CAP_DOWN_BOW_Y + subY1 ,
                                           F32_CONST(downWork->ballPosX) , 
                                           F32_CONST(downWork->ballPosY) +8);
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr ,
                                           MB_CAP_DOWN_BOW_X + subX2 ,
                                           MB_CAP_DOWN_BOW_Y + subY2 ,
                                           F32_CONST(downWork->ballPosX) , 
                                           F32_CONST(downWork->ballPosY) +8);
    }
    else
    {
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr ,
                                           MB_CAP_DOWN_BOW_X + subX1 ,
                                           MB_CAP_DOWN_BOW_Y + subY1 ,
                                           MB_CAP_DOWN_BOW_X + subX2 ,
                                           MB_CAP_DOWN_BOW_Y + subY2 );
/*
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr ,
                                           MB_CAP_DOWN_BOW_X + subX1 ,
                                           MB_CAP_DOWN_BOW_Y + subY1 ,
                                           MB_CAP_DOWN_BALL_X , 
                                           MB_CAP_DOWN_BALL_Y );
      
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr ,
                                           MB_CAP_DOWN_BOW_X + subX2 ,
                                           MB_CAP_DOWN_BOW_Y + subY2 ,
                                           MB_CAP_DOWN_BALL_X , 
                                           MB_CAP_DOWN_BALL_Y );
*/
    }
  }
  
  
  //�J�n�n�_���v�Z
  #if 0
  {
    static u8 posX = 128;
    static u8 posY = 96;
    BOOL isUpdateDot = FALSE;
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
    {
      posX--;
      isUpdateDot = TRUE;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
    {
      posX++;
      isUpdateDot = TRUE;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
    {
      posY--;
      isUpdateDot = TRUE;
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
    {
      posY++;
      isUpdateDot = TRUE;
    }
    if( isUpdateDot == TRUE )
    {
      MB_CAP_DOWN_UpdateBow_DrawLine_Func( vramAdr , posX , posY );
    }
  }
  #endif
}

static void MB_CAP_DOWN_UpdateBow_DrawLine_Func( const u8* vramAdr , const int x1 , const int y1 , const int x2 , const int y2 )
{
  const int xLen = MATH_ABS( x1-x2 );
  const int yLen = MATH_ABS( y1-y2 );
  const int loopNum = ( xLen > yLen ? xLen : yLen )+1;
  fx32 addX = FX32_CONST(x2-x1) / loopNum;
  fx32 addY = FX32_CONST(y2-y1) / loopNum;
  fx32 subX = 0;
  fx32 subY = 0;
  
  u8 i;
  
  for( i=0;i<loopNum;i++ )
  {
    const int posX = x1 + (subX>>FX32_SHIFT);
    const int posY = y1 + (subY>>FX32_SHIFT);
    
    MB_CAP_DOWN_UpdateBow_DrawDot( vramAdr , posX , posY );
    MB_CAP_DOWN_UpdateBow_DrawDot( vramAdr , posX , posY+1 );
    
    subX += addX;
    subY += addY;
  }
  
}

static void MB_CAP_DOWN_UpdateBow_DrawDot( const u8* vramAdr , const int x , const int y )
{
  const u8 drawCol = 1;

  const int charX = x/8;
  const int charY = y/8;
  const u16 charNo = charX+charY*32;
  const u8 ofsX = x%8;
  const u8 ofsY = y%8;
  const u8 ofsXmod = ofsX%4;
  const u32 ofsAdr = (ofsX/4) + ofsY*2;
  u16 *trgAdr = (u16*)((u32)vramAdr + charNo*0x20 + ofsAdr*2);

  static const u16 shiftArr[4]={0x0001*drawCol,
                                0x0010*drawCol,
                                0x0100*drawCol,
                                0x1000*drawCol};
  if( (u32)trgAdr <  (u32)vramAdr+0x6000 &&
      (u32)trgAdr >= (u32)vramAdr )
  {
    *trgAdr |= shiftArr[ofsXmod];
  }
}
//--------------------------------------------------------------
//	�{�[���X�V
//--------------------------------------------------------------
static void MB_CAP_DOWN_UpdateBall( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  if( downWork->state == MCDS_NONE )
  {
    downWork->ballPosX = FX32_CONST(MB_CAP_DOWN_BALL_X);
    downWork->ballPosY = FX32_CONST(MB_CAP_DOWN_BALL_Y);
    downWork->isFlying = FALSE;
  }
  else
  if( downWork->state == MCDS_DRAG )
  {
    downWork->isFlying = FALSE;
  }
  else
  if( downWork->state == MCDS_SHOT_WAIT )
  {
    //�{�[�����
    if( downWork->ballPosY > FX32_CONST(-32) )
    {
      downWork->ballPosX -= downWork->shotBallSpdX;
      downWork->ballPosY -= downWork->shotBallSpdY;
      downWork->isUpdateBall = TRUE;
      {
        if( downWork->ballPosY < FX32_CONST(MB_CAP_DOWN_BALL_Y) )
        {
          downWork->pullLen = 0;
          downWork->isFlying = TRUE;
        }
        else
        {
          const s32 subX = F32_CONST(downWork->ballPosX) - MB_CAP_DOWN_BOW_X;
          const s32 subY = F32_CONST(downWork->ballPosY) - MB_CAP_DOWN_BOW_Y;
          const s32 len_mul = subX*subX + subY*subY;
          const fx32 len_fx = FX_Sqrt( FX32_CONST(len_mul) );
          const fx32 len_sub = len_fx - FX32_CONST(MB_CAP_DOWN_DEF_BALL_LEN);
          
          downWork->pullLen = len_sub;
          downWork->isFlying = FALSE;
        }
      }
    }
    else
    {
      downWork->pullLen = 0;
      downWork->isFlying = TRUE;
    }
  }
  else
  if( downWork->state == MCDS_SUPPLY_BALL )
  {
    downWork->ballPosY += FX32_CONST(MB_CAP_DOWN_BALL_SUPPLY_SPD);
    if( downWork->ballPosY > FX32_CONST(MB_CAP_DOWN_BALL_Y) )
    {
      downWork->ballPosY = FX32_CONST(MB_CAP_DOWN_BALL_Y);
      downWork->state = MCDS_NONE;
    }
    downWork->isFlying = TRUE;
    downWork->isUpdateBall = TRUE;
  }

  if( downWork->isUpdateBall == TRUE )
  {
    GFL_CLACTPOS cellPos;
    cellPos.x = F32_CONST(downWork->ballPosX);
    cellPos.y = F32_CONST(downWork->ballPosY);
    GFL_CLACT_WK_SetPos( downWork->clwkBall , &cellPos , CLSYS_DEFREND_SUB );
    downWork->isUpdateBall = FALSE;
  }
}

//--------------------------------------------------------------
//	TP����(�f�����l�����č��W�͊O�n��
//--------------------------------------------------------------
static void MB_CAP_DOWN_UpdateTP( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork , u32 tpx , u32 tpy )
{
  if( downWork->isTrg == TRUE )
  {
    if( downWork->state == MCDS_NONE )
    {
      const u32 subX = F32_CONST(downWork->ballPosX) - tpx;
      const u32 subY = F32_CONST(downWork->ballPosY) - tpy;
      const u32 ballLen = subX*subX + subY*subY;
      if( ballLen < MB_CAP_DOWN_BALL_TOUCH_LEN*MB_CAP_DOWN_BALL_TOUCH_LEN )
      {
        downWork->state = MCDS_DRAG;
        downWork->isPlayBowPullSnd = FALSE;
      }
    }
  }
  //else�͖���
  
  if( downWork->state == MCDS_DRAG )
  {
    if( downWork->isTouch == TRUE )
    {
      downWork->ballPosX = FX32_CONST(tpx);
      downWork->ballPosY = FX32_CONST(tpy);
      downWork->isUpdateBall = TRUE;
      //�����E�p�x�v�Z
      {
        const s32 subX = F32_CONST(downWork->ballPosX) - MB_CAP_DOWN_BOW_X;
        const s32 subY = F32_CONST(downWork->ballPosY) - MB_CAP_DOWN_BOW_Y;
        const s32 len_mul = subX*subX + subY*subY;
        const fx32 len_fx = FX_Sqrt( FX32_CONST(len_mul) );
        const fx32 len_sub = len_fx - FX32_CONST(MB_CAP_DOWN_DEF_BALL_LEN);
        
        downWork->pullLen = len_sub;
        downWork->rotAngle = FX_Atan2Idx(FX32_CONST(-subX), FX32_CONST(subY));;
      }
      if( downWork->pullLen > (MB_CAP_DOWN_BOW_PULL_LEN_MAX/4) &&
          downWork->isPlayBowPullSnd == FALSE )
      {
        PMSND_PlaySE( MB_SND_BOW_PULL );
        downWork->isPlayBowPullSnd = TRUE;
      }
      //TODO�����Ŕ͈͊O�̊p�x�E�������������ꍇ�́A�␳�������ďC������
    }
    else
    {
      //TODO�����Ȃǂ����Ĕ�΂����̔���
      const fx32 subX = downWork->ballPosX - FX32_CONST(MB_CAP_DOWN_BOW_X);
      const fx32 subY = downWork->ballPosY - FX32_CONST(MB_CAP_DOWN_BOW_Y);
      
      downWork->shotBallSpdX = FX_Div(subX*MB_CAP_DOWN_BALL_SHOT_SPD,downWork->pullLen);
      downWork->shotBallSpdY = FX_Div(subY*MB_CAP_DOWN_BALL_SHOT_SPD,downWork->pullLen);
      
      downWork->shotLen = downWork->pullLen;
      downWork->shotAngle = downWork->rotAngle;
      
      downWork->state = MCDS_SHOT_WAIT;
      PMSND_PlaySE( MB_SND_BOW_SHOT );
    }
  }
}

#pragma mark [>outer func
//--------------------------------------------------------------
//  �O���񋟊֐��Q
//--------------------------------------------------------------
const MB_CAP_DOWN_STATE MB_CAP_DOWN_GetState( const MB_CAP_DOWN *downWork )
{
  return downWork->state;
}
const fx32 MB_CAP_DOWN_GetPullLen( const MB_CAP_DOWN *downWork )
{
  return downWork->pullLen;
}
const u16  MB_CAP_DOWN_GetRotAngle( const MB_CAP_DOWN *downWork )
{
  return downWork->rotAngle;
}
const fx32 MB_CAP_DOWN_GetShotLen( const MB_CAP_DOWN *downWork )
{
  return downWork->shotLen;
}
const u16  MB_CAP_DOWN_GetShotAngle( const MB_CAP_DOWN *downWork )
{
  return downWork->shotAngle;
}

//--------------------------------------------------------------
//  �{�[�����[����
//--------------------------------------------------------------
void MB_CAP_DOWN_ReloadBall( MB_CAP_DOWN *downWork )
{
  downWork->state = MCDS_SUPPLY_BALL;
  downWork->ballPosX = FX32_CONST(MB_CAP_DOWN_BALL_SUPPLY_X);
  downWork->ballPosY = FX32_CONST(MB_CAP_DOWN_BALL_SUPPLY_Y);
}

