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

//======================================================================
//	define
//======================================================================
#pragma mark [> define
#define MB_CAP_DOWN_BOW_X (128)
#define MB_CAP_DOWN_BOW_Y (62)

#define MB_CAP_DOWN_BALL_X (128)
#define MB_CAP_DOWN_BALL_Y (82)

#define MB_CAP_DOWN_DEF_BALL_LEN (MB_CAP_DOWN_BALL_Y-MB_CAP_DOWN_BOW_Y)

//�| ����n���l
#define MB_CAP_DOWN_BOWBEND_SCALE_MAX (FX32_HALF)
#define MB_CAP_DOWN_BOWBEND_SCALE_LEN_MAX (FX32_CONST(100))

//�^�b�`���蔼�a
#define MB_CAP_DOWN_BALL_TOUCH_LEN (8)

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

//�X�e�[�g
typedef enum
{
  MCDS_NONE,        //��������
  MCDS_DRAG,        //��������
  MCDS_SHOT_WAIT,   //���ł�
  MCDS_SUPPLY_BALL, //�{�[����[
}MB_CAP_DOWN_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_CAP_DOWN
{
  BOOL isTrg;
  BOOL isTouch;
  BOOL isUpdateBall;
  
  MB_CAP_DOWN_STATE state;
  
  u32 cellRes[MCDO_MAX];
  GFL_CLUNIT  *cellUnit;
  GFL_CLWK    *clwkBall;
  
  int ballPosX;
  int ballPosY;
  
  //�|�֌W
  int rot;    //��]
  fx32 pull;   //���������
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void MB_CAP_DOWN_UpdateBow( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork );
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
  
  downWork->rot  = 0;
  downWork->pull = 0;
  downWork->state = MCDS_NONE;
  downWork->ballPosX = MB_CAP_DOWN_BALL_X;
  downWork->ballPosY = MB_CAP_DOWN_BALL_Y;
  downWork->isUpdateBall = TRUE;
  
  return downWork;
}

//--------------------------------------------------------------
//	����ʊJ��
//--------------------------------------------------------------
void MB_CAP_POKE_DeleteSystem( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  GFL_CLACT_WK_Remove( downWork->clwkBall );
  GFL_CLGRP_PLTT_Release( downWork->cellRes[MCDO_PLT] );
  GFL_CLGRP_CGR_Release( downWork->cellRes[MCDO_NCG] );
  GFL_CLGRP_CELLANIM_Release( downWork->cellRes[MCDO_ANM] );

  GFL_HEAP_FreeMemory( downWork );
}

//--------------------------------------------------------------
//	����ʍX�V
//--------------------------------------------------------------
void MB_CAP_POKE_UpdateSystem( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
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
  /*
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
  {
    downWork->rot += 4;
    isUpdate = TRUE;
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
  {
    downWork->rot -= 4;
    isUpdate = TRUE;
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  {
    if( downWork->pull > 0 )
    {
      downWork->pull -= FX32_CONST(0.1f);
      isUpdate = TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    downWork->pull += FX32_CONST(0.1f);
    isUpdate = TRUE;
  }
  */
  
  if( downWork->state == MCDS_DRAG )
  {
    const s32 subX = downWork->ballPosX - MB_CAP_DOWN_BOW_X;
    const s32 subY = downWork->ballPosY - MB_CAP_DOWN_BOW_Y;
    const s32 len_mul = subX*subX + subY*subY;
    const fx32 len_fx = FX_Sqrt( FX32_CONST(len_mul) );
    const fx32 len_sub = len_fx - FX32_CONST(MB_CAP_DOWN_DEF_BALL_LEN);
    
    {
      //����v�Z
      if( len_sub > MB_CAP_DOWN_BOWBEND_SCALE_LEN_MAX )
      {
        downWork->pull = MB_CAP_DOWN_BOWBEND_SCALE_MAX;
      }
      else
      {
        const fx32 len_rate = FX_Div(len_sub,MB_CAP_DOWN_BOWBEND_SCALE_LEN_MAX);
        downWork->pull = FX_Mul( MB_CAP_DOWN_BOWBEND_SCALE_MAX , len_rate );
      }
      //��]�v�Z
      {
        const u16 angle = FX_Atan2Idx(FX32_CONST(-subX), FX32_CONST(subY));
        downWork->rot = angle*360/0x10000;
      }
    }
    isUpdate = TRUE;
  }
  else
  {
    downWork->rot = 0;
    downWork->pull = 0;
    isUpdate = TRUE;
  }
  
  
  if( isUpdate == TRUE )
  {
    const u16  rot = (downWork->rot<0?downWork->rot+360:downWork->rot);
    const fx32 xScale = FX32_ONE + downWork->pull;
    const fx32 yScale = FX32_ONE - downWork->pull;
    GFL_BG_SetRadianReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_RADION_SET , rot );
    GFL_BG_SetScaleReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_SCALE_X_SET , xScale );
    GFL_BG_SetScaleReq( MB_CAPTURE_FRAME_SUB_BOW , GFL_BG_SCALE_Y_SET , yScale );
  }
}

//--------------------------------------------------------------
//	�{�[���X�V
//--------------------------------------------------------------
static void MB_CAP_DOWN_UpdateBall( MB_CAPTURE_WORK *capWork , MB_CAP_DOWN *downWork )
{
  if( downWork->isUpdateBall == TRUE )
  {
    GFL_CLACTPOS cellPos;
    cellPos.x = downWork->ballPosX;
    cellPos.y = downWork->ballPosY;
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
      const u32 subX = downWork->ballPosX - tpx;
      const u32 subY = downWork->ballPosY - tpy;
      const u32 ballLen = subX*subX + subY*subY;
      if( ballLen < MB_CAP_DOWN_BALL_TOUCH_LEN*MB_CAP_DOWN_BALL_TOUCH_LEN )
      {
        downWork->state = MCDS_DRAG;
      }
    }
  }
  //else�͖���
  
  if( downWork->state == MCDS_DRAG )
  {
    if( downWork->isTouch == TRUE )
    {
      downWork->ballPosX = tpx;
      downWork->ballPosY = tpy;
      downWork->isUpdateBall = TRUE;

    }
    else
    {
      downWork->state = MCDS_NONE;
      downWork->ballPosX = MB_CAP_DOWN_BALL_X;
      downWork->ballPosY = MB_CAP_DOWN_BALL_Y;
      downWork->isUpdateBall = TRUE;
    }
  }
}
