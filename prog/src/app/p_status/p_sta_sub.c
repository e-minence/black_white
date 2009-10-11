//======================================================================
/**
 * @file	p_sta_sub.c
 * @brief	�|�P�����X�e�[�^�X �E�������
 * @author	ariizumi
 * @data	09/06/10
 *
 * ���W���[�����FPSTATUS_SUB
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "print/printsys.h"
#include "print/wordset.h"

#include "arc_def.h"

#include "p_sta_sys.h"
#include "p_sta_sub.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define


#define PSTATUS_POS_X(val) (FX32_CONST((val)/16.0f))
#define PSTATUS_POS_Y(val) (FX32_CONST((192.0f-( val ))/16.0f))

//#define PSTATUS_MCSS_POS_X (FX32_CONST(( 200+16 )/16.0f))
//#define PSTATUS_MCSS_POS_Y (FX32_CONST(( 192.0f - (120) )/16.0f))

#define PSTATUS_MCSS_POS_X (PSTATUS_POS_X( 200+16 ))
#define PSTATUS_MCSS_POS_Y (PSTATUS_POS_Y( 120 ))

//���W�n
//��pWin
#define PSTATUS_SUB_UPWIN_LEFT (19)
#define PSTATUS_SUB_UPWIN_TOP  ( 0)
#define PSTATUS_SUB_UPWIN_WIDTH  (13)
#define PSTATUS_SUB_UPWIN_HEIGHT ( 4)

//���pWin
#define PSTATUS_SUB_DOWNWIN_LEFT (19)
#define PSTATUS_SUB_DOWNWIN_TOP  (17)
#define PSTATUS_SUB_DOWNWIN_WIDTH  (13)
#define PSTATUS_SUB_DOWNWIN_HEIGHT ( 4)

//�����񋤒ʃI�t�Z�b�g
#define PSTATUS_STR_OFS_X (2)
#define PSTATUS_STR_OFS_Y (2)

//��Win�p�������W
#define PSTATUS_SUB_STR_NAME_X (24 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_NAME_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_SEX_X (88 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_SEX_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_LV_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_LV_Y (16 + PSTATUS_STR_OFS_Y)

//��Win�p�������W
#define PSTATUS_SUB_STR_ITEM_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_ITEM_Y ( 0 + PSTATUS_STR_OFS_Y)
#define PSTATUS_SUB_STR_ITEMNAME_X ( 8 + PSTATUS_STR_OFS_X)
#define PSTATUS_SUB_STR_ITEMNAME_Y (16 + PSTATUS_STR_OFS_Y)

//�Z���n���W
#define PSTATUS_SUB_CELL_BALL_X ( 168 )
#define PSTATUS_SUB_CELL_BALL_Y (   8 )

//�|�P�̃^�b�`�͈�
#define PSTATUS_SUB_TOUCH_TOP ( 32-32)
#define PSTATUS_SUB_TOUCH_BOTTOM ( 120+32)
#define PSTATUS_SUB_TOUCH_LEFT ( 152-32)
#define PSTATUS_SUB_TOUCH_RIGHT ( 255)

//�^�b�`�y������ł���ȏ㓮�����瓮�����Ƃ݂Ȃ�
#define PSTATUS_SUB_TP_MOVE_LENGTH ( 16 )
//�����̗L������
#define PSTATUS_SUB_TP_MOVE_TIME ( 20 )
//�A�N�V�������m�肷��J�E���g��
#define PSTATUS_SUB_ACTION_COUNT ( 20 )
//�A�N�V�������I������J�E���g
#define PSTATUS_SUB_ACTION_END_COUNT ( 24 )

//TP���쑬�x���L�^���鐔
#define PSTATUS_SUB_TP_SPEED_CNT (5)

//�J������`
#define PSTATUS_SUB_CAMERA_FRONT_X (FX32_CONST(-41.0f))
#define PSTATUS_SUB_CAMERA_BACK_X (FX32_CONST(-66.0f))

//�U������n��`
#define PSTATUS_SUB_TURN_TIME (10)
#define PSTATUS_SUB_TURN_HEIGHT (2)  //FX16�ɂ�����̂Ő�����OK

//�O�����A�j����`
#define PSTATUS_SUB_FLIP_TIME (3)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

typedef enum
{
  PSS_DISP_FRONT,
  PSS_FRONT_TO_BACK,
  PSS_DISP_BACK,
  PSS_BACK_TO_FRONT,
}PSTATUS_SUB_STATE;

enum
{
  SSMT_BOX_DISC,
  SSMT_BOX_TRIANGLE,
  SSMT_BOX_SQUARE,
  SSMT_BOX_HEART,
  SSMT_BOX_STAR,
  SSMT_BOX_DIAMOND,
  SSMT_MARK_RARE,
  SSMT_MARK_POKERUS,
  
  SSMT_MAX,
}PSTAUTS_SUB_MARK_TYPE;

typedef enum
{
  PSSS_NONE,
  PSSS_STEP,
  PSSS_JUMP,
  PSSS_FLOAT,
  PSSS_STEP_END,
  PSSS_JUMP_END,
  PSSS_FLOAT_END,
  
}PSTAUTS_SUB_SUBACTION_STATE;

//TP�A�N�V������TP�ړ�����
typedef enum
{
  SSMD_NONE,  //����
  SSMD_RIGHT, //�E�ɍs����
  SSMD_DOWN,  //���ɍs����
  SSMD_LEFT,  //���ɍs����
  SSMD_UP,    //��ɍs����
}PSTATUS_SUB_MOVE_DIR;

//TP�A�N�V������TP��]����
typedef enum
{
  SSRD_NONE,  //����
  SSRD_RIGHT, //�E����(���v���
  SSRD_LEFT,  //������
}PSTATUS_SUB_ROT_DIR;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  u8  count;
  u8  time;
  u8  befX;
  u8  befY;
  PSTATUS_SUB_MOVE_DIR  dir;
}PSTATUS_TP_ACTION_WORK;

struct _PSTATUS_SUB_WORK
{
  MCSS_WORK     *pokeMcss;
  MCSS_WORK     *pokeMcssBack;
  
  PSTATUS_SUB_STATE state;

  BOOL  isDispFront;
  
  GFL_BMPWIN  *bmpWinUpper;
  GFL_BMPWIN  *bmpWinDown;
  
  GFL_CLWK    *clwkBall;
  GFL_CLWK    *clwkMark[SSMT_MAX];
  

  //�T�u�A�N�V�����p
  VecFx32 shadowOffset; //�����ɂ��e�I�t�Z�b�g
  BOOL isStartAction; //�U��Ԃ�`�F�b�N�Ɏg��
  PSTAUTS_SUB_SUBACTION_STATE subActState;
  
  u16 continueCount;
  u16 subActCount;
  u16 subActStep;
  u16 subActValue;  //��]�p ����
  u16 subActTarget; //��]�p �ڕW�l

  //TP����p
  BOOL        IsHoldTp;
  u32         befTpx;
  u32         befTpy;
  
  //�������蔻��
  PSTATUS_TP_ACTION_WORK tpWorkH;
  //�c�����蔻��
  PSTATUS_TP_ACTION_WORK tpWorkV;
  //��]�����蔻��
  PSTATUS_TP_ACTION_WORK tpWorkR;
  PSTATUS_SUB_ROT_DIR  tpRotDir;
  u8  tpRotSpeed[PSTATUS_SUB_TP_SPEED_CNT];
  u8  tpRotSpeedCnt;
  u8  tpRotSpeedPos;
  
  //�U��������o�Ǘ�
  u8          turnAnimeCnt;

};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
static void PSTATUS_SUB_DrawStr( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp );

static void PSTATUS_SUB_SubActionUpdate( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static void PSTATUS_SUB_SubActionInitAction( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static const BOOL PSTATUS_SUB_SubActionContinueCheck( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , PSTATUS_TP_ACTION_WORK *tpWork );

static const BOOL PSTATUS_SUB_SubActionUpdateStep( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static const BOOL PSTATUS_SUB_SubActionUpdateJump( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static const BOOL PSTATUS_SUB_SubActionUpdateFloat( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const BOOL isUpdate );


static void PSTATUS_SUB_UpdateTP( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static void PSTATUS_SUB_UpdateTPAction( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static void PSTATUS_SUB_ResetTPActionWork( PSTATUS_TP_ACTION_WORK *tpWork , u32 tpx , u32 tpy );
static void PSTATUS_SUB_AddCountTPActionWork( PSTATUS_TP_ACTION_WORK *tpWork , u32 tpx , u32 tpy , PSTATUS_SUB_MOVE_DIR dir );
static void PSTATUS_SUB_TPCheckRotSpeed( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const BOOL isRound );

static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp );
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork );
static void PSTATUS_SUB_SetShadowHeight( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , fx32 height );
static void PSTATUS_SUB_SetShadowOffset( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , VecFx32 *offset );

//--------------------------------------------------------------
//	������
//--------------------------------------------------------------
PSTATUS_SUB_WORK* PSTATUS_SUB_Init( PSTATUS_WORK *work )
{
  PSTATUS_SUB_WORK* subWork;
  
  subWork = GFL_HEAP_AllocMemory( work->heapId , sizeof(PSTATUS_SUB_WORK) );
  subWork->pokeMcss = NULL;
  subWork->pokeMcssBack = NULL;
  subWork->IsHoldTp = FALSE;
  subWork->isDispFront = TRUE;
  subWork->state = PSS_DISP_FRONT;
  subWork->subActState = PSSS_NONE;
  return subWork;
}

//--------------------------------------------------------------
//	�J��
//--------------------------------------------------------------
void PSTATUS_SUB_Term( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //PSTATUS_SUB_PokeDeleteMcss( work,subWork );
  GFL_HEAP_FreeMemory( subWork );
}

//--------------------------------------------------------------
//	�X�V
//--------------------------------------------------------------
void PSTATUS_SUB_Main( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  if( subWork->state == PSS_DISP_FRONT ||
      subWork->state == PSS_DISP_BACK )
  {
    PSTATUS_SUB_UpdateTP( work , subWork );
    PSTATUS_SUB_SubActionUpdate( work , subWork );
  }
  else
  if( subWork->state == PSS_BACK_TO_FRONT )
  {
    //�|�P�����U���������
    VecFx32 cam_pos = {PSTATUS_SUB_CAMERA_FRONT_X,FX32_CONST(0.0f),FX32_CONST(101.0f)};
    VecFx32 ofs={0,0,0};
    
    subWork->turnAnimeCnt++;
    
    if( subWork->turnAnimeCnt > PSTATUS_SUB_TURN_TIME/2 &&
        subWork->isDispFront == FALSE )
    {
      MCSS_ResetVanishFlag( subWork->pokeMcss );
      MCSS_SetVanishFlag( subWork->pokeMcssBack );
      subWork->isDispFront = TRUE;
    }

    if( subWork->turnAnimeCnt >= PSTATUS_SUB_TURN_TIME )
    {
      PSTATUS_SUB_SetShadowHeight( work , subWork , 0 );
      subWork->state = PSS_DISP_FRONT;
    }
    else
    {
      u16 rad;
      fx32 sin;
      const fx32 camSubX = (PSTATUS_SUB_CAMERA_FRONT_X-PSTATUS_SUB_CAMERA_BACK_X);
      const fx32 camOfsX = camSubX*subWork->turnAnimeCnt/PSTATUS_SUB_TURN_TIME;
      cam_pos.x = PSTATUS_SUB_CAMERA_BACK_X+camOfsX;
      
      rad = subWork->turnAnimeCnt*0x8000/PSTATUS_SUB_TURN_TIME;
      sin = FX_SinIdx( rad );
      ofs.y = (sin*PSTATUS_SUB_TURN_HEIGHT);
      PSTATUS_SUB_SetShadowHeight( work , subWork , ofs.y );
    }
    MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
    MCSS_SetOfsPosition( subWork->pokeMcssBack , &ofs );
    GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
    GFL_G3D_CAMERA_Switching( work->camera );
  }
  else
  if( subWork->state == PSS_FRONT_TO_BACK )
  {
    VecFx32 cam_pos = {PSTATUS_SUB_CAMERA_BACK_X,FX32_CONST(0.0f),FX32_CONST(101.0f)};
    VecFx32 shadowScale = {PSTATUS_SUB_SHADOW_SCALE_BACK_X , PSTATUS_SUB_SHADOW_SCALE_BACK_Y , PSTATUS_SUB_SHADOW_SCALE_BACK_Z};
    
    subWork->turnAnimeCnt++;

    if( subWork->turnAnimeCnt < PSTATUS_SUB_FLIP_TIME )
    {
      //���l�w��
      const fx32 scareXArr[2] = { FX32_CONST(1.2f),FX32_CONST(1.4f) };
      const fx32 scareYArr[2] = { FX32_CONST(2.2f),FX32_CONST(2.3f) };
      const fx32 camXArr[2] = { FX32_CONST(-57.0f),FX32_CONST(-60.0f) };
      const u8 arrIdx = subWork->turnAnimeCnt-1;
      
      shadowScale.x = scareXArr[arrIdx];
      shadowScale.y = scareYArr[arrIdx];
      cam_pos.x = camXArr[arrIdx];
      
      GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
      GFL_G3D_CAMERA_Switching( work->camera );
      MCSS_SetShadowScale( subWork->pokeMcss , &shadowScale );
    }
    else
    {
      //�Ђ�����Ԃ�I
      
      MCSS_SetVanishFlag( subWork->pokeMcss );
      MCSS_ResetVanishFlag( subWork->pokeMcssBack );
      MCSS_SetShadowScale( subWork->pokeMcssBack , &shadowScale );
      GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
      GFL_G3D_CAMERA_Switching( work->camera );
      subWork->isDispFront = FALSE;
      subWork->state = PSS_DISP_BACK;
      subWork->IsHoldTp = FALSE;
    }
  }
}

//--------------------------------------------------------------
//	�`��
//--------------------------------------------------------------
void PSTATUS_SUB_Draw( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //���g�p
}

#pragma mark [>Resource
//--------------------------------------------------------------
//	���\�[�X�ǂݍ���
//--------------------------------------------------------------
void PSTATUS_SUB_LoadResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , ARCHANDLE *archandle )
{
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_bg_d_NSCR , 
                    PSTATUS_BG_PLATE ,  0 , 0, FALSE , work->heapId );
  
}

//--------------------------------------------------------------
//	���\�[�X�J��
//--------------------------------------------------------------
void PSTATUS_SUB_ReleaseResource( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //�����ŕ`�敨�̊J�������Ă���
  PSTATUS_SUB_ClearPage( work , subWork );

}

#pragma mark [>Cell
//--------------------------------------------------------------
//	�Z��������
//--------------------------------------------------------------
void PSTATUS_SUB_InitCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //�I���J�[�\��
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = PSTATUS_SUB_CELL_BALL_X;
    cellInitData.pos_y = PSTATUS_SUB_CELL_BALL_Y;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    cellInitData.anmseq = 0;
    
    subWork->clwkBall = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellRes[SCR_NCG_BALL],
              work->cellRes[SCR_PLT_BALL],
              work->cellRes[SCR_ANM_BALL],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

    GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , FALSE );
  }
  {
    static const u8 markPosX[SSMT_MAX] = 
    { 208 , 216 , 224 , 232 , 240 , 248 , 184 , 176 };
    static const u8 markPosY = 128;
    static const u8 markAnmSeq[SSMT_MAX] = 
    { 0 , 2 , 4 , 6 , 8 , 10 , 12 , 13 };
    u8 i;
    //�}�[�N
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_y = markPosY;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    
    for( i=0;i<SSMT_MAX;i++ )
    {
      cellInitData.pos_x = markPosX[i];
      cellInitData.anmseq = markAnmSeq[i];
      subWork->clwkMark[i] = GFL_CLACT_WK_Create( work->cellUnit ,
                work->cellRes[SCR_NCG_MARK],
                work->cellRes[SCR_PLT_MARK],
                work->cellRes[SCR_ANM_MARK],
                &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
      GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[i] , TRUE );
    }
    
  }
}

//--------------------------------------------------------------
//	�Z���J��
//--------------------------------------------------------------
void PSTATUS_SUB_TermCell( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  u8 i;
  for( i=0;i<SSMT_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( subWork->clwkMark[i] );
  }
  GFL_CLACT_WK_Remove( subWork->clwkBall );
}

#pragma mark [>Disp
//--------------------------------------------------------------
//	�y�[�W�̕\��
//--------------------------------------------------------------
void PSTATUS_SUB_DispPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  
  if( work->psData->ppt == PST_PP_TYPE_POKEPARTY )
  {
    subWork->isDispFront = PokeParty_GetStatusViewIsFront( work->psData->ppd , work->dataPos );
  }
  else
  {
    subWork->isDispFront = TRUE;
  }

//  PSTATUS_SUB_PokeCreateMcss( work , subWork , ppp );

  //BmpWin
  subWork->bmpWinUpper = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
              PSTATUS_SUB_UPWIN_LEFT , PSTATUS_SUB_UPWIN_TOP ,
              PSTATUS_SUB_UPWIN_WIDTH , PSTATUS_SUB_UPWIN_HEIGHT ,
              PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  subWork->bmpWinDown  = GFL_BMPWIN_Create( PSTATUS_BG_PARAM ,
              PSTATUS_SUB_DOWNWIN_LEFT , PSTATUS_SUB_DOWNWIN_TOP ,
              PSTATUS_SUB_DOWNWIN_WIDTH , PSTATUS_SUB_DOWNWIN_HEIGHT ,
              PSTATUS_BG_PLT_FONT , GFL_BMP_CHRAREA_GET_B );

  PSTATUS_SUB_DrawStr( work , subWork , ppp );
}
//--------------------------------------------------------------
//	�y�[�W�̕\��
//--------------------------------------------------------------
void PSTATUS_SUB_DispPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  PSTATUS_SUB_PokeCreateMcss( work , subWork , ppp );
  GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinUpper );
  GFL_BMPWIN_MakeTransWindow_VBlank( subWork->bmpWinDown );
  
  //�{�[���̍X�V
  {
    u32 ballIdx = PPP_Get( ppp , ID_PARA_get_ball , NULL );
    if( ballIdx != 0 )
    {
      ballIdx--;
    }
    {
      NNSG2dPaletteData *pltData;
      void *pltRes = GFL_ARC_UTIL_LoadPalette( ARCID_P_STATUS , 
                          NARC_p_status_gra_ball00_NCLR + ballIdx ,
                          &pltData , work->heapId );
      GFL_CLGRP_PLTT_Replace( work->cellRes[SCR_PLT_BALL] , pltData , 1 );
      GFL_HEAP_FreeMemory( pltRes );
    }
    {
      NNSG2dCharacterData *ncgData;
      void *ncgRes = GFL_ARC_UTIL_LoadBGCharacter( ARCID_P_STATUS , 
                          NARC_p_status_gra_ball00_NCGR + ballIdx ,
                          FALSE , &ncgData , work->heapId );
      GFL_CLGRP_CGR_Replace( work->cellRes[SCR_NCG_BALL] , ncgData );
      GFL_HEAP_FreeMemory( ncgRes );
    }
    
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , TRUE );
  }
  //�}�[�N�̍X�V
  {
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[SSMT_MARK_RARE] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( subWork->clwkMark[SSMT_MARK_POKERUS] , FALSE );
    
  }
  
  if( subWork->isDispFront == TRUE )
  {
    VecFx32 cam_pos = {PSTATUS_SUB_CAMERA_FRONT_X,FX32_CONST(0.0f),FX32_CONST(101.0f)};
    VecFx32 shadowScale = {PSTATUS_SUB_SHADOW_SCALE_X , PSTATUS_SUB_SHADOW_SCALE_Y , PSTATUS_SUB_SHADOW_SCALE_Z};
    VecFx32 ofs={0,0,0};
    
    MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
    PSTATUS_SUB_SetShadowOffset( work , subWork , &ofs );
    MCSS_SetShadowScale( subWork->pokeMcss , &shadowScale );
    
    GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
    GFL_G3D_CAMERA_Switching( work->camera );

    subWork->state = PSS_DISP_FRONT;
  }
  else
  {
    VecFx32 cam_pos = {PSTATUS_SUB_CAMERA_BACK_X,FX32_CONST(0.0f),FX32_CONST(101.0f)};
    VecFx32 shadowScale = {PSTATUS_SUB_SHADOW_SCALE_BACK_X , PSTATUS_SUB_SHADOW_SCALE_BACK_Y , PSTATUS_SUB_SHADOW_SCALE_BACK_Z};
    VecFx32 ofs={0,0,0};

    MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
    PSTATUS_SUB_SetShadowOffset( work , subWork , &ofs );
    MCSS_SetShadowScale( subWork->pokeMcssBack , &shadowScale );
    GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
    GFL_G3D_CAMERA_Switching( work->camera );

    subWork->state = PSS_DISP_BACK;
  }

  /*
  //�؂�ւ����O�����Œ�
  subWork->isDispFront = TRUE;
  subWork->isJumpPoke = FALSE;
  {
    VecFx32 ofs={0,0,0};
    static const VecFx32 cam_pos = {PSTATUS_SUB_CAMERA_FRONT_X,FX32_CONST(0.0f),FX32_CONST(101.0f)};
    MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
    MCSS_SetOfsPosition( subWork->pokeMcssBack , &ofs );
    GFL_G3D_CAMERA_SetPos( work->camera , &cam_pos );
    GFL_G3D_CAMERA_Switching( work->camera );
  }
  */

}

//--------------------------------------------------------------
//	�y�[�W�̃N���A
//--------------------------------------------------------------
void PSTATUS_SUB_ClearPage( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  if( work->psData->ppt == PST_PP_TYPE_POKEPARTY )
  {
    PokeParty_SetStatusViewIsFront( work->psData->ppd , work->befDataPos , subWork->isDispFront );
  }

  //����
  GFL_BMPWIN_Delete( subWork->bmpWinDown );
  GFL_BMPWIN_Delete( subWork->bmpWinUpper );

  //MCSS
//  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
}
//--------------------------------------------------------------
//	�y�[�W�̃N���A
//--------------------------------------------------------------
void PSTATUS_SUB_ClearPage_Trans( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  //MCSS
  PSTATUS_SUB_PokeDeleteMcss( work,subWork );
  GFL_CLACT_WK_SetDrawEnable( subWork->clwkBall , FALSE );
}

//--------------------------------------------------------------
//	�����̕`��
//--------------------------------------------------------------
void PSTATUS_SUB_DrawStr( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp )
{
  //���O
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 32, work->heapId );
    STRBUF *nameStr = GFL_STR_CreateBuffer( 32, work->heapId );
    PPP_Get( ppp, ID_PARA_nickname, nameStr );

    WORDSET_RegisterWord( wordSet , 0 , nameStr , 0,TRUE,PM_LANG );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_01 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );

    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
            PSTATUS_SUB_STR_NAME_X , PSTATUS_SUB_STR_NAME_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK);

    GFL_STR_DeleteBuffer( nameStr );
    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  
  //����
  {
    STRBUF *srcStr;
    u32 sex = PPP_Get( ppp , ID_PARA_sex , NULL );
    if( sex == PTL_SEX_MALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_02 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
              PSTATUS_SUB_STR_SEX_X , PSTATUS_SUB_STR_SEX_Y ,
              srcStr , work->fontHandle , PSTATUS_STR_COL_BLUE );
      GFL_STR_DeleteBuffer( srcStr );
    }
    else if( sex == PTL_SEX_FEMALE )
    {
      srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_03 ); 
      PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
              PSTATUS_SUB_STR_SEX_X , PSTATUS_SUB_STR_SEX_Y ,
              srcStr , work->fontHandle , PSTATUS_STR_COL_RED );
      GFL_STR_DeleteBuffer( srcStr );
    }
  }

  //���x��
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 lv = PPP_CalcLevel( ppp );
    WORDSET_RegisterNumber( wordSet , 0 , lv , 3 , STR_NUM_DISP_LEFT , STR_NUM_CODE_DEFAULT );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_04 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinUpper ) , 
            PSTATUS_SUB_STR_LV_X , PSTATUS_SUB_STR_LV_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }

  //"��������"
  {
    STRBUF *srcStr;
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_05 ); 
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinDown ) , 
            PSTATUS_SUB_STR_ITEM_X , PSTATUS_SUB_STR_ITEM_Y ,
            srcStr , work->fontHandle , PSTATUS_STR_COL_BLACK );
    GFL_STR_DeleteBuffer( srcStr );
  }

  //�����A�C�e����
  {
    WORDSET *wordSet = WORDSET_Create( work->heapId );
    STRBUF *srcStr;
    STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

    u32 itemId = PPP_Get( ppp , ID_PARA_item , NULL );
    WORDSET_RegisterItemName( wordSet , 0 , itemId );
    srcStr = GFL_MSG_CreateString( work->msgHandle , mes_status_01_06 ); 
    WORDSET_ExpandStr( wordSet , dstStr , srcStr );
    PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( subWork->bmpWinDown ) , 
            PSTATUS_SUB_STR_ITEMNAME_X , PSTATUS_SUB_STR_ITEMNAME_Y , 
            dstStr , work->fontHandle , PSTATUS_STR_COL_BLACK );

    GFL_STR_DeleteBuffer( srcStr );
    GFL_STR_DeleteBuffer( dstStr );
    WORDSET_Delete( wordSet );
  }
  
}

#pragma mark [>Action
static void PSTATUS_SUB_SubActionUpdate( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  switch( subWork->subActState )
  {
  case PSSS_NONE:
    if( subWork->isStartAction == FALSE )
    {
      if( subWork->tpWorkH.count > PSTATUS_SUB_ACTION_COUNT )
      {
        subWork->subActState = PSSS_STEP;
        PSTATUS_SUB_SubActionInitAction( work , subWork );
      }
      if( subWork->tpWorkV.count > PSTATUS_SUB_ACTION_COUNT )
      {
        subWork->subActState = PSSS_JUMP;
        PSTATUS_SUB_SubActionInitAction( work , subWork );
      }
      if( subWork->tpWorkR.count > PSTATUS_SUB_ACTION_COUNT )
      {
        subWork->subActState = PSSS_FLOAT;
        PSTATUS_SUB_SubActionInitAction( work , subWork );
      }
    }
    break;

  case PSSS_STEP:
    PSTATUS_SUB_SubActionUpdateStep( work , subWork );
    if( PSTATUS_SUB_SubActionContinueCheck( work , subWork , &subWork->tpWorkH ) == FALSE )
    {
      subWork->subActState = PSSS_STEP_END;
    }
    break;
    
  case PSSS_STEP_END:
    if( PSTATUS_SUB_SubActionUpdateStep( work , subWork ) == TRUE )
    {
      subWork->subActState = PSSS_NONE;
    }
    break;

  case PSSS_JUMP:
    PSTATUS_SUB_SubActionUpdateJump( work , subWork );
    if( PSTATUS_SUB_SubActionContinueCheck( work , subWork , &subWork->tpWorkV ) == FALSE )
    {
      subWork->subActState = PSSS_JUMP_END;
    }
    break;

  case PSSS_JUMP_END:
    if( PSTATUS_SUB_SubActionUpdateJump( work , subWork ) == TRUE )
    {
      subWork->subActState = PSSS_NONE;
    }
    break;

  case PSSS_FLOAT:
    PSTATUS_SUB_SubActionUpdateFloat( work , subWork , TRUE );
    if( PSTATUS_SUB_SubActionContinueCheck( work , subWork , &subWork->tpWorkR ) == FALSE )
    {
      subWork->subActState = PSSS_FLOAT_END;
    }
    break;

  case PSSS_FLOAT_END:
    if( PSTATUS_SUB_SubActionUpdateFloat( work , subWork , FALSE ) == TRUE )
    {
      MCSS_ResetAnmStopFlag( subWork->pokeMcss );
      subWork->subActState = PSSS_NONE;
    }
    break;

  }
}

static void PSTATUS_SUB_SubActionInitAction( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  subWork->isStartAction = TRUE;
  subWork->subActStep = 0;
  subWork->subActTarget = 0;
  subWork->subActCount = 0;
  subWork->subActValue = 0;
  subWork->continueCount = 0;
}

static const BOOL PSTATUS_SUB_SubActionContinueCheck( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , PSTATUS_TP_ACTION_WORK *tpWork )
{
  if( tpWork->count < PSTATUS_SUB_ACTION_COUNT )
  {
    subWork->continueCount++;
    if( subWork->continueCount > PSTATUS_SUB_ACTION_END_COUNT )
    {
      return FALSE;
    }
  }
  else
  {
    subWork->continueCount = 0;
  }
  return TRUE;
}

#define PSTATUS_SUB_STEP_TIME (15)
#define PSTATUS_SUB_STEP_WIDTH (16)
#define PSTATUS_SUB_STEP_HEIGHT (16) 
static const BOOL PSTATUS_SUB_SubActionUpdateStep( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  VecFx32 pos = {PSTATUS_MCSS_POS_X,PSTATUS_MCSS_POS_Y,0};
  VecFx32 ofs = {0,0,0};
  
  {
    //����
    const u16 rad = subWork->subActCount*0x8000/PSTATUS_SUB_STEP_TIME;
    const fx32 sin = FX_SinIdx( rad );
    ofs.y = (sin*PSTATUS_SUB_STEP_HEIGHT+FX32_CONST(work->friend/64))/16;
    PSTATUS_SUB_SetShadowHeight( work , subWork , ofs.y );
  }
  {
    //���ʒu
    const u8 moveMax = PSTATUS_SUB_STEP_WIDTH + (work->friend/64);
    int moveVal = subWork->subActCount*moveMax/PSTATUS_SUB_STEP_TIME;
    switch( subWork->subActStep )
    {
    case 0:
      //������E
      pos.x += FX32_CONST( moveVal )/16;
      break;
    case 1:
      //�E���璆
      pos.x += FX32_CONST( moveMax-moveVal )/16;
      break;
    case 2:
      //�����獶
      pos.x -= FX32_CONST( moveVal )/16;
      break;
    case 3:
      //�����璆
      pos.x -= FX32_CONST( moveMax-moveVal )/16;
      break;
    }
  }
  MCSS_SetPosition( subWork->pokeMcss , &pos );
  MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
  {
    //�J�E���g�X�V
    subWork->subActCount++;
    if( subWork->subActCount > PSTATUS_SUB_STEP_TIME )
    {
      subWork->subActCount = 0;
      subWork->subActStep++;
      if( subWork->subActStep > 3 )
      {
        subWork->subActStep = 0;
        return TRUE;
      }
    }
  }
  return FALSE;
}

#define PSTATUS_SUB_JUMP_TIME (20)
#define PSTATUS_SUB_JUMP_HEIGHT (32) 
static const BOOL PSTATUS_SUB_SubActionUpdateJump( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  VecFx32 ofs = {0,0,0};
  
  {
    //����
    const u16 rad = subWork->subActCount*0x8000/PSTATUS_SUB_JUMP_TIME;
    const fx32 sin = FX_SinIdx( rad );
    ofs.y = (sin*PSTATUS_SUB_JUMP_HEIGHT+FX32_CONST(work->friend/64))/16;
    PSTATUS_SUB_SetShadowHeight( work , subWork , ofs.y );
  }
  MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
  {
    //�J�E���g�X�V
    subWork->subActCount++;
    if( subWork->subActCount > PSTATUS_SUB_JUMP_TIME )
    {
      subWork->subActCount = 0;
      return TRUE;
    }
  }
  return FALSE;
}

#define PSTATUS_SUB_FLOAT_RATE (16)  //x�{���Ă�̂ŁAx�Ŋ���
#define PSTATUS_SUB_FLOAT_MAX ( 32*PSTATUS_SUB_FLOAT_RATE )
#define PSTATUS_SUB_FLOAT_MIN (  8*PSTATUS_SUB_FLOAT_RATE )
#define PSTATUS_SUB_FLOAT_VAL ( PSTATUS_SUB_FLOAT_MAX-PSTATUS_SUB_FLOAT_MIN)
#define PSTATUS_SUB_FLOAT_SPEED_MAX (10)  //�����̂�MAX�̂ق����������̂Œ���
#define PSTATUS_SUB_FLOAT_SPEED_MIN (20)
#define PSTATUS_SUB_FLOAT_SPEED_VAL (PSTATUS_SUB_FLOAT_SPEED_MIN-PSTATUS_SUB_FLOAT_SPEED_MAX)
#define PSTATUS_SUB_YURE_SPEED (120)
#define PSTATUS_SUB_YURE_VALUE (4)
static const BOOL PSTATUS_SUB_SubActionUpdateFloat( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const BOOL isUpdate )
{
  //Step = �ڕW����
  //Count = ���ݍ���
  VecFx32 ofs = {0,0,0};
  u32 aveSpeed = 0;
  if( isUpdate == TRUE )
  {
    u8 i;
    for( i=0;i<PSTATUS_SUB_TP_SPEED_CNT;i++ )
    {
      aveSpeed += subWork->tpRotSpeed[i];
    }
    aveSpeed /= PSTATUS_SUB_TP_SPEED_CNT;
    if( aveSpeed < PSTATUS_SUB_FLOAT_SPEED_MAX )
    {
      aveSpeed = PSTATUS_SUB_FLOAT_SPEED_MAX;
    }
    if( aveSpeed > PSTATUS_SUB_FLOAT_SPEED_MIN )
    {
      aveSpeed = PSTATUS_SUB_FLOAT_SPEED_MIN;
    }
    
    //�ڕW����
    {
      const u16 heightOfs = PSTATUS_SUB_FLOAT_MAX-PSTATUS_SUB_FLOAT_MIN;
      const u16 speedVal = PSTATUS_SUB_FLOAT_SPEED_VAL-(aveSpeed-PSTATUS_SUB_FLOAT_SPEED_MAX);
      subWork->subActTarget = PSTATUS_SUB_FLOAT_MIN + ( speedVal*PSTATUS_SUB_FLOAT_VAL/PSTATUS_SUB_FLOAT_SPEED_VAL );
    }
    {
      //���
      
      //ofs.x += FX_CosIdx( subWork->subActCount*(0x10000/PSTATUS_SUB_YURE_SPEED) )*PSTATUS_SUB_YURE_VALUE/16;
      ofs.y += FX_SinIdx( subWork->subActCount*(0x10000/PSTATUS_SUB_YURE_SPEED) )*PSTATUS_SUB_YURE_VALUE/16;
      
      subWork->subActCount++;
      if( subWork->subActCount > PSTATUS_SUB_YURE_SPEED )
      {
        subWork->subActCount = 0;
      }
    }
  }
  else
  {
    subWork->subActTarget = 0;
  }
  {
    //����
    if( subWork->subActValue < subWork->subActTarget )
    {
      subWork->subActValue++;
    }
    if( subWork->subActValue > subWork->subActTarget )
    {
      if( isUpdate == FALSE )
      {
        //�I���������͑���
        if( subWork->subActValue >= PSTATUS_SUB_FLOAT_RATE )
        {
          subWork->subActValue -= PSTATUS_SUB_FLOAT_RATE;
        }
        else
        {
          subWork->subActValue = 0;
        }
      }
      else
      {
        subWork->subActValue--;
      }
    }
    
    ofs.y += FX32_CONST(subWork->subActValue)/(16*PSTATUS_SUB_FLOAT_RATE);
    PSTATUS_SUB_SetShadowHeight( work , subWork , ofs.y );
  }
  MCSS_SetOfsPosition( subWork->pokeMcss , &ofs );
  MCSS_SetAnmStopFlag( subWork->pokeMcss );
  
  if( subWork->subActValue == 0 &&
      subWork->subActTarget == 0 )
  {
    return TRUE;
  }
  return FALSE;
}

#pragma mark [>UI
static void PSTATUS_SUB_UpdateTP( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  u32 tpx,tpy;
  //�����蔻��쐬
  GFL_UI_TP_HITTBL hitTbl[2] =
  {
    { PSTATUS_SUB_TOUCH_TOP , PSTATUS_SUB_TOUCH_BOTTOM , PSTATUS_SUB_TOUCH_LEFT , PSTATUS_SUB_TOUCH_RIGHT },
    { GFL_UI_TP_HIT_END ,0,0,0 },
  };
  
  GFL_UI_TP_GetPointCont( &tpx,&tpy );
  if( work->isEgg == FALSE )
  {
    const BOOL isTrg = GFL_UI_TP_GetTrg();
    const BOOL isTouch = GFL_UI_TP_GetCont();
    const int  retCont = GFL_UI_TP_HitCont( hitTbl );
    if( subWork->state == PSS_DISP_FRONT )
    {
      if( retCont == 0 )
      {
        if( isTrg == TRUE )
        {
          u8 i;
          subWork->IsHoldTp = TRUE;
          subWork->isStartAction = FALSE;
          PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkH , tpx , tpy );
          PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkV , tpx , tpy );
          PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkR , tpx , tpy );
          for( i=0;i<PSTATUS_SUB_TP_SPEED_CNT;i++ )
          {
            subWork->tpRotSpeed[i] = 0;
          }
          subWork->tpRotSpeedCnt = 0;
          subWork->tpRotSpeedPos = 0;
          subWork->tpRotDir = SSRD_NONE;
        }
        else
        if( isTouch == TRUE && subWork->IsHoldTp == TRUE )
        {
          PSTATUS_SUB_UpdateTPAction( work , subWork );
        }
        subWork->befTpx = tpx;
        subWork->befTpy = tpy;
      }
      else
      {
        if( subWork->IsHoldTp == TRUE )
        {
          if( subWork->isStartAction == FALSE && 
              subWork->subActState == PSSS_NONE )
          {
            //�������]����
            subWork->state = PSS_FRONT_TO_BACK;
            subWork->turnAnimeCnt = 0;
          }
        }
        PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkH , tpx , tpy );
        PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkV , tpx , tpy );
        PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkR , tpx , tpy );
        subWork->IsHoldTp = FALSE;
      }
    }
    else
    if( subWork->state == PSS_DISP_BACK )
    {
      //�����ĐU�����
      if( retCont == 0 )
      {
        if( isTrg == TRUE )
        {
          subWork->IsHoldTp = TRUE;
        }
      }
      else
      {
        if( subWork->IsHoldTp == TRUE )
        {
          subWork->IsHoldTp = FALSE;
          subWork->state = PSS_BACK_TO_FRONT;
          subWork->turnAnimeCnt = 0;
        }
      }
    }
  }
  else
  {
    //�^�}�S�͕ʏ���
  }
}

static void PSTATUS_SUB_UpdateTPAction( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  u32 tpx,tpy;
  GFL_UI_TP_GetPointCont( &tpx,&tpy );
  //�������蔻��
  {
    const s8 subX = tpx - subWork->tpWorkH.befX;
    const s8 subY = tpy - subWork->tpWorkH.befY;
    if( subWork->tpWorkH.dir == SSMD_NONE || subWork->tpWorkH.dir == SSMD_LEFT )
    {
      if( subX > PSTATUS_SUB_TP_MOVE_LENGTH )
      {
        PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkH , tpx , tpy , SSMD_RIGHT );
      }
    }
    if( subWork->tpWorkH.dir == SSMD_NONE || subWork->tpWorkH.dir == SSMD_RIGHT )
    {
      if( subX < -PSTATUS_SUB_TP_MOVE_LENGTH )
      {
        PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkH , tpx , tpy , SSMD_LEFT );
      }
    }
    if( subY > PSTATUS_SUB_TP_MOVE_LENGTH || subY < -PSTATUS_SUB_TP_MOVE_LENGTH ||
       subWork->tpWorkH.time > PSTATUS_SUB_TP_MOVE_TIME )
    {
      PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkH , tpx , tpy );
    }
    subWork->tpWorkH.time++;
  }
  //�c�����蔻��
  {
    const s8 subX = tpx - subWork->tpWorkV.befX;
    const s8 subY = tpy - subWork->tpWorkV.befY;
    if( subWork->tpWorkV.dir == SSMD_NONE || subWork->tpWorkV.dir == SSMD_UP )
    {
      if( subY > PSTATUS_SUB_TP_MOVE_LENGTH )
      {
        PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkV , tpx , tpy , SSMD_DOWN );
      }
    }
    if( subWork->tpWorkV.dir == SSMD_NONE || subWork->tpWorkV.dir == SSMD_DOWN )
    {
      if( subY < -PSTATUS_SUB_TP_MOVE_LENGTH )
      {
        PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkV , tpx , tpy , SSMD_UP );
      }
    }
    if( subX > PSTATUS_SUB_TP_MOVE_LENGTH || subX < -PSTATUS_SUB_TP_MOVE_LENGTH ||
       subWork->tpWorkV.time > PSTATUS_SUB_TP_MOVE_TIME )
    {
      PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkV , tpx , tpy );
    }
    subWork->tpWorkV.time++;
  }
  //��]�����蔻��
  {
    const s8 subX = tpx - subWork->tpWorkR.befX;
    const s8 subY = tpy - subWork->tpWorkR.befY;
    u8 moveDir = SSMD_NONE;
    BOOL isRound = FALSE;
    if( subX > PSTATUS_SUB_TP_MOVE_LENGTH )
    {
      moveDir = SSMD_RIGHT;
    }
    else
    if( subX < -PSTATUS_SUB_TP_MOVE_LENGTH )
    {
      moveDir = SSMD_LEFT;
    }
    else
    if( subY > PSTATUS_SUB_TP_MOVE_LENGTH )
    {
      moveDir = SSMD_DOWN;
    }
    else
    if( subY < -PSTATUS_SUB_TP_MOVE_LENGTH )
    {
      moveDir = SSMD_UP;
    }
    
    if( moveDir != SSMD_NONE && moveDir != subWork->tpWorkR.dir )
    {
      if( subWork->tpWorkR.dir + 1 == moveDir || 
         (subWork->tpWorkR.dir == SSMD_UP && moveDir == SSMD_RIGHT) )
      {
        //���v���
        if( subWork->tpRotDir == SSRD_RIGHT || subWork->tpRotDir == SSRD_NONE )
        {
          subWork->tpRotDir = SSRD_RIGHT;
          PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkR , tpx , tpy , moveDir );
          if( subWork->tpWorkR.dir == SSMD_UP )
          {
            isRound = TRUE;
          } 
        }
        else
        {
          subWork->tpRotDir = SSRD_NONE;
          PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkR , tpx , tpy );
        }
      }
      else
      if( subWork->tpWorkR.dir - 1 == moveDir || 
         (subWork->tpWorkR.dir == SSMD_RIGHT && moveDir == SSMD_UP) )
      {
        //�����v���
        if( subWork->tpRotDir == SSRD_LEFT || subWork->tpRotDir == SSRD_NONE )
        {
          subWork->tpRotDir = SSRD_LEFT;
          PSTATUS_SUB_AddCountTPActionWork( &subWork->tpWorkR , tpx , tpy , moveDir );
          if( subWork->tpWorkR.dir == SSMD_UP )
          {
            isRound = TRUE;
          } 
        }
        else
        {
          subWork->tpRotDir = SSRD_NONE;
          PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkR , tpx , tpy );
        }
      }
      else
      if( subWork->tpWorkR.dir == SSMD_NONE )
      {
        subWork->tpWorkR.dir = moveDir;
      }
      
    }
    if( subWork->tpWorkR.time > PSTATUS_SUB_TP_MOVE_TIME )
    {
      subWork->tpRotDir = SSRD_NONE;
      PSTATUS_SUB_ResetTPActionWork( &subWork->tpWorkR , tpx , tpy );
    }
    PSTATUS_SUB_TPCheckRotSpeed( work , subWork , isRound );
    subWork->tpWorkR.time++;
  }
  //OS_TPrintf("[%2d][%2d][%2d]\n",subWork->tpWorkH.count,subWork->tpWorkV.count,subWork->tpWorkR.count);
}

static void PSTATUS_SUB_ResetTPActionWork( PSTATUS_TP_ACTION_WORK *tpWork , u32 tpx , u32 tpy )
{
  tpWork->count = 0;
  tpWork->dir = SSMD_NONE;
  tpWork->time = 0;
  tpWork->befX = tpx;
  tpWork->befY = tpy;
}

static void PSTATUS_SUB_AddCountTPActionWork( PSTATUS_TP_ACTION_WORK *tpWork , u32 tpx , u32 tpy , PSTATUS_SUB_MOVE_DIR dir )
{
  tpWork->count++;
  tpWork->dir = dir;
  tpWork->time = 0;
  tpWork->befX = tpx;
  tpWork->befY = tpy;
}

static void PSTATUS_SUB_TPCheckRotSpeed( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const BOOL isRound )
{
  if( isRound == TRUE )
  {
    subWork->tpRotSpeed[ subWork->tpRotSpeedPos ] = subWork->tpRotSpeedCnt;
    subWork->tpRotSpeedCnt = 0;
    subWork->tpRotSpeedPos++;
    if( subWork->tpRotSpeedPos >= PSTATUS_SUB_TP_SPEED_CNT )
    {
      subWork->tpRotSpeedPos = 0;
    }
  }
  subWork->tpRotSpeedCnt++;
}

#pragma mark [>Poke
//--------------------------------------------------------------
//	�|�P����MCSS�쐬
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeCreateMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , const POKEMON_PASO_PARAM *ppp )
{
  MCSS_ADD_WORK addWork;
  VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};
  VecFx32 shadowScale = {PSTATUS_SUB_SHADOW_SCALE_X , PSTATUS_SUB_SHADOW_SCALE_Y , PSTATUS_SUB_SHADOW_SCALE_Z};
  VecFx32 shadowScaleBack = {PSTATUS_SUB_SHADOW_SCALE_BACK_X , PSTATUS_SUB_SHADOW_SCALE_BACK_Y , PSTATUS_SUB_SHADOW_SCALE_BACK_Z};
  VecFx32 shadowOffset= {PSTATUS_SUB_SHADOW_OFFSET_X , PSTATUS_SUB_SHADOW_OFFSET_Y , PSTATUS_SUB_SHADOW_OFFSET_Z};
  
  work->shadowRotate = 302*65536/360;
  
  GF_ASSERT( subWork->pokeMcss == NULL );
  {
    MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_FRONT );
    subWork->pokeMcss = MCSS_Add( work->mcssSys , PSTATUS_MCSS_POS_X , PSTATUS_MCSS_POS_Y ,0 , &addWork );
    MCSS_SetScale( subWork->pokeMcss , &scale );
    MCSS_SetShadowAlpha( subWork->pokeMcss , 2 );
    MCSS_SetShadowRotate( subWork->pokeMcss , PSTATUS_SUB_SHADOW_ROTATE );
    MCSS_SetShadowScale( subWork->pokeMcss , &shadowScale );
  }
  {
    MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_BACK );
    subWork->pokeMcssBack = MCSS_Add( work->mcssSys , PSTATUS_MCSS_POS_X , PSTATUS_MCSS_POS_Y ,0 , &addWork );
    MCSS_SetScale( subWork->pokeMcssBack , &scale );
    MCSS_SetShadowAlpha( subWork->pokeMcssBack , 2 );
    MCSS_SetShadowRotate( subWork->pokeMcssBack , PSTATUS_SUB_SHADOW_ROTATE );
    MCSS_SetShadowScale( subWork->pokeMcssBack , &shadowScaleBack );
  }
  PSTATUS_SUB_SetShadowHeight( work , subWork , 0 );
  PSTATUS_SUB_SetShadowOffset( work , subWork , &shadowOffset );

  if( subWork->isDispFront == TRUE )
  {
    MCSS_ResetVanishFlag( subWork->pokeMcss );
    MCSS_SetVanishFlag( subWork->pokeMcssBack );
  }
  else
  {
    MCSS_ResetVanishFlag( subWork->pokeMcssBack );
    MCSS_SetVanishFlag( subWork->pokeMcss );
  }
}

//--------------------------------------------------------------
//	�|�P����MCSS�폜
//--------------------------------------------------------------
static void PSTATUS_SUB_PokeDeleteMcss( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  GF_ASSERT( subWork->pokeMcss != NULL );
  
  MCSS_SetVanishFlag( subWork->pokeMcss );
  MCSS_SetVanishFlag( subWork->pokeMcssBack );
  MCSS_Del(work->mcssSys,subWork->pokeMcss);
  MCSS_Del(work->mcssSys,subWork->pokeMcssBack);
  subWork->pokeMcss = NULL;
  subWork->pokeMcssBack = NULL;
}

//�e�������ł��炷���߁A�I�t�Z�b�g�ݒ莞�Ɍv�Z���Ă��炷
static void PSTATUS_SUB_SetShadowHeight( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , fx32 height )
{
  VecFx32 shadowOffset= {PSTATUS_SUB_SHADOW_OFFSET_X , PSTATUS_SUB_SHADOW_OFFSET_Y , PSTATUS_SUB_SHADOW_OFFSET_Z};
  subWork->shadowOffset.x = FX_Mul(FX_SinIdx( PSTATUS_SUB_SHADOW_ROTATE ) , height);
  subWork->shadowOffset.y = FX_Mul(FX_CosIdx( PSTATUS_SUB_SHADOW_ROTATE ) , height);
  subWork->shadowOffset.z = 0;
  PSTATUS_SUB_SetShadowOffset( work , subWork , &shadowOffset );
}

static void PSTATUS_SUB_SetShadowOffset( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork , VecFx32 *offset )
{
  VecFx32 calcOfs;
  VEC_Add( offset , &subWork->shadowOffset , &calcOfs );
  MCSS_SetShadowOffset( subWork->pokeMcss , &calcOfs );
  MCSS_SetShadowOffset( subWork->pokeMcssBack , &calcOfs );
}

//�f�o�O�p
void PSTATUS_SUB_SetShadowScale( PSTATUS_WORK *work , PSTATUS_SUB_WORK *subWork )
{
  MCSS_SetShadowScale( subWork->pokeMcss , &work->shadowScale );
  MCSS_SetShadowRotate( subWork->pokeMcss , work->shadowRotate );
  MCSS_SetShadowScale( subWork->pokeMcssBack , &work->shadowScale );
  MCSS_SetShadowRotate( subWork->pokeMcssBack , work->shadowRotate );
  PSTATUS_SUB_SetShadowOffset( work , subWork , &work->shadowOfs );
}
