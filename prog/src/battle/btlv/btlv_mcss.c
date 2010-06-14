
//============================================================================================
/**
 * @file  btlv_mcss.c
 * @brief �퓬MCSS�Ǘ�
 * @author  soga
 * @date  2008.11.14
 */
//============================================================================================

#include <gflib.h>

#include "app/app_menu_common.h"
#include "poke_tool/gauge_tool.h"
#include "system/gfl_use.h"
#include "system/mcss_tool.h"
#include "sound/pm_voice.h"
#include "sound/pm_wb_voice.h"

#include "btlv_effect.h"

#include "battle/btl_common.h"
#include "battle/battgra_wb.naix"
#include "pokegra/pokegra_wb.naix"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

#define BTLV_MCSS_MAX ( BTLV_MCSS_POS_TOTAL ) //BTLV_MCSS�ŊǗ�����MCSS��MAX�l

#define BTLV_MCSS_DEFAULT_SHIFT ( FX32_SHIFT - 4 )          //�|���S���P�ӂ̊�̒����ɂ���V�t�g�l
#define BTLV_MCSS_DEFAULT_LINE  ( 1 << BTLV_MCSS_DEFAULT_SHIFT )  //�|���S���P�ӂ̊�̒���

#define BTLV_MCSS_ORTHO_SCALE_MINE  ( FX32_ONE * 16 * 2 )
#define BTLV_MCSS_ORTHO_SCALE_ENEMY ( FX32_ONE * 16 * 1 )

#define BTLV_MCSS_STOP_ANIME_TIME1   ( 48 )
#define BTLV_MCSS_STOP_ANIME_TIME2   ( 56 )
#define BTLV_MCSS_STOP_ANIME_TIME3   ( 64 )
#define BTLV_MCSS_STOP_ANIME_COUNT  ( 3 )

#define BTLV_MCSS_NO_INDEX  ( -1 )

#define BTLV_MCSS_PINCH_PITCH ( -2560 )

#define BTLV_MCSS_EVY_WAIT ( 24 )
#define BTLV_MCSS_EVY_MAX  ( 12 )

enum{
  REVERSE_FLAG_OFF = 0,
  REVERSE_FLAG_ON,

  ANIME_STOP_FLAG = 0x80000000,
  ANIME_START_FLAG = 0x00000000,

  SEQ_ANIME_STANDBY = 0,
  SEQ_ANIME_STOP,

  BTLV_MCSS_ROTATION_Z_E = BTLV_STAGE_POS_Z_E + FX32_ONE * 2,
  BTLV_MCSS_ROTATION_Z_M = BTLV_STAGE_POS_Z_M + FX32_ONE * 2,
};

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

typedef struct
{
  int mons_no;
  int form_no;
  u32 personal_rnd;
  u16 weight;
  u16 appear_hp_color;    //�o�ꎞ��HP�o�[�J���[
}BTLV_MCSS_PARAM;

struct  _BTLV_MCSS
{
  MCSS_WORK*      mcss;
  MCSS_ADD_WORK   maw;
  GFL_TCB*        tcb;
  BTLV_MCSS_PARAM param;
  int             capture_ball;
  u32             status_flag;
  BOOL            anm_execute_flag;
  BtlvMcssPos     position;
  u32             mepachi_always_flag :1;
  u32             sick_set_flag       :1;
  u32             mcss_proj_mode      :1;
  u32             stop_anime_count    :2;
  u32             effect_vanish_flag  :1;     //�Z�G�t�F�N�g����Ă΂ꂽvanish��ێ�
  u32             set_init_pos        :1;     //�Z�G�t�F�N�g��A�����ʒu����͂���Ă����`�F�b�N�Ɉ�����������
  u32                                 :25;
};

struct _BTLV_MCSS_WORK
{
  GFL_TCBSYS*     tcb_sys;
  MCSS_SYS_WORK*  mcss_sys;
  BTLV_MCSS       btlv_mcss[ BTLV_MCSS_MAX ];

  u32             mcss_pos_3vs3             :1;
  u32             mcss_pos_rotate           :1;
  u32             mcss_tcb_rotation_execute :1;
  u32                                       :29;
  u32             mcss_tcb_move_execute;
  u32             mcss_tcb_scale_execute;
  u32             mcss_tcb_rotate_execute;
  u32             mcss_tcb_default_scale_execute;

  u32             mcss_tcb_blink_execute;
  u32             mcss_tcb_alpha_execute;
  u32             mcss_tcb_mosaic_execute;
  int             evy;
  int             evy_dir;
  int             evy_wait;

  int             proj;

  fx32            scale_ofs_m;          //�f�t�H���g�X�P�[���ɑ΂���I�t�Z�b�g
  fx32            scale_ofs_e;          //�f�t�H���g�X�P�[���ɑ΂���I�t�Z�b�g

#ifdef  PM_DEBUG
  BOOL            anm_1_loop_flag;
#endif

  HEAPID          heapID;
};

typedef struct
{
  BTLV_MCSS_WORK*   bmw;
  int               position;
  VecFx32           now_value;
  EFFTOOL_MOVE_WORK emw;
}BTLV_MCSS_TCB_WORK;

typedef struct
{
  int seq_no;
  int index;
  int wait;
}BTLV_MCSS_STOP_ANIME;

typedef struct
{
  BTLV_MCSS_WORK*   bmw;
  int               seq_no;
  int               side;
  int               dir;
  int               angle[ 3 ];
  int               frame;
  int               speed;
}BTLV_MCSS_ROTATION_WORK;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

static  void  BTLV_MCSS_MakeMAWTrainer( int tr_type, MCSS_ADD_WORK* maw, int position );
static  void  BTLV_MCSS_SetDefaultScale( BTLV_MCSS_WORK *bmw, int position );

static  void  BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end,
                                       int frame, int wait, int count, GFL_TCB_FUNC* func,
                                       BTLV_EFFECT_TCB_CALLBACK_FUNC* cb_func, int reverse_flag );
static  void  TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Move_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Scale_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_DefaultScale( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_DefaultScale_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Rotate_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Blink_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_Alpha( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Alpha_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_MoveCircle( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_MoveCircle_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_MoveSin( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_MoveSin_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_Mosaic( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Mosaic_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_MCSS_StopAnime( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Rotation( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Rotation_CB( GFL_TCB *tcb );

static  void  BTLV_MCSS_CallBackFunctorFrame( u32 data, fx32 currentFrame );
static  BOOL  BTLV_MCSS_CallBackNodes( u32 data, const NNSG2dMultiCellHierarchyData* pNodeData,
                                       NNSG2dCellAnimation* pCellAnim, u16 nodeIdx );
static  void  BTLV_MCSS_AnmEndCheck( u32 data, fx32 currentFrame );

static  void  BTLV_MCSS_GetDefaultPos( BTLV_MCSS_WORK *bmw, VecFx32 *pos, BtlvMcssPos position );
static  fx32  BTLV_MCSS_GetDefaultScale( BTLV_MCSS_WORK* bmw, BtlvMcssPos position, BTLV_MCSS_PROJECTION proj );

static  int   BTLV_MCSS_GetIndex( BTLV_MCSS_WORK* bmw, BtlvMcssPos position );

#ifdef PM_DEBUG
void  BTLV_MCSS_AddDebug( BTLV_MCSS_WORK *bmw, const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 *  �|�P�����̗����ʒu�e�[�u��
 */
//============================================================================================
static  const VecFx32 poke_pos_single_table[]={
  { 0x00000800, 0x00000666, 0x00007000 }, //POS_AA
  { 0x000004cd, 0x00000666, 0xffff6000 }, //POS_BB
};

static  const VecFx32 poke_pos_double_table[]={
  { 0xffffe500, 0x00000666, 0x00007000 }, //POS_A
  { 0x000026cd, 0x00000666, 0xffff5a00 }, //POS_B
  { 0x00002800, 0x00000666, 0x00007000 }, //POS_C
  { 0xffffdccd, 0x00000666, 0xffff4700 }, //POS_D
};

static  const VecFx32 poke_pos_triple_table[]={
  { 0xffffc000, 0x00000666, 0x00007000 }, //POS_A
  { 0x00004800, 0x00000666, 0xffff4000 }, //POS_B
  { 0x00000c00, 0x00000666, 0x00004c00 }, //POS_C
  { 0x00000bcd, 0x00000666, 0xffff7000 }, //POS_D
  { 0x00005300, 0x00000666, 0x00006300 }, //POS_E
  { 0xffffb9cd, 0x00000666, 0xffff4000 }, //POS_F
};

static  const VecFx32 poke_pos_rotate_table[]={
#ifdef ROTATION_NEW_SYSTEM
  { 0x00000000, 0x00000500, BTLV_MCSS_ROTATION_Z_M + 0xffffb000 }, //POS_A
  { 0x00000000, 0x00000500, BTLV_MCSS_ROTATION_Z_E + 0x00005000 }, //POS_B
  { 0xffffbab4, 0x00000500, BTLV_MCSS_ROTATION_Z_M + 0x00002801 }, //POS_C
  { 0x0000454a, 0x00000500, BTLV_MCSS_ROTATION_Z_E + 0xffffd7ff }, //POS_D
  { 0x0000454a, 0x00000500, BTLV_MCSS_ROTATION_Z_M + 0x00002801 }, //POS_E
  { 0xffffbab4, 0x00000500, BTLV_MCSS_ROTATION_Z_E + 0xffffd7ff }, //POS_F
#else
  { 0xffffe500, 0x00000666, 0x00007000 }, //POS_A
  { 0x000026cd, 0x00000666, 0xffff5a00 }, //POS_B
  { 0x00002800, 0x00000666, 0x00007000 }, //POS_C
  { 0xffffdccd, 0x00000666, 0xffff4700 }, //POS_D
  { 0x00000800, 0x00000666, 0x0000a000 }, //POS_E
  { 0x000004cd, 0x00000666, 0xffff3000 }, //POS_F
#endif
};

static  const VecFx32 trainer_pos_table[]={
  { FX_F32_TO_FX32( -2.5f + 3.000f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   7.5f - 0.5f ) },    //POS_TR_AA
  { 0, 0x0666, FX_F32_TO_FX32( -12.0 ) }, //���~�̏�
  //{ FX_F32_TO_FX32(  4.5f - 4.200f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( -25.0f ) },           //POS_TR_BB
  { FX_F32_TO_FX32( -3.5f + 3.500f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   8.5f ) },           //POS_TR_A
  { 0x000026cd, 0x0666, FX_F32_TO_FX32( -12.0 ) }, //���~�̏�
  { FX_F32_TO_FX32( -0.5f + 3.845f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   9.0f ) },           //POS_TR_C
  { 0xffffdccd, 0x0666, FX_F32_TO_FX32( -12.0 ) }, //���~�̏�
};

//============================================================================================
/**
 *  �|�P�����̗����ʒu�ɂ��X�P�[���␳�e�[�u��
 */
//============================================================================================
static  const fx32 poke_scale_single_table[]={
  0x1030,   //POS_AA
  0x11bf,   //POS_BB
};

static  const fx32 poke_scale_double_table[]={
  0x000010e8, //POS_A
  0x0000118e, //POS_B
  0x00000f1a, //POS_C
  0x00001320, //POS_D
};

static  const fx32 poke_scale_triple_table[]={
  0x000011f0, //POS_A
  0x000011f0, //POS_B
  0x00001270, //POS_C
  0x0000110e, //POS_D
  0x00000ec4, //POS_E
  0x000013f0, //POS_F
};

static  const fx32 poke_scale_rotate_table[]={
#ifdef ROTATION_NEW_SYSTEM
  0x00001040, //POS_A
  0x0000109c, //POS_B
  0x000009a1, //POS_C
  0x00001440, //POS_D
  0x000004b3, //POS_E
  0x00001610, //POS_F
#else
  0x000010f0, //POS_A
  0x00001205, //POS_B
  0x00000f00, //POS_C
  0x00001320, //POS_D
  0x00000e00, //POS_E
  0x000010f0, //POS_F
#endif
};

static  const fx32 trainer_scale_table[]={
  0x1030,   //POS_TR_AA
  0x1300,   //POS_TR_BB
  0x0f00,   //POS_TR_A
  0x1300,   //POS_TR_B
  0x0d00,   //POS_TR_C
  0x1300,   //POS_TR_D
};

//============================================================================================
/**
 *  �|�P�����̗����ʒu�ɂ������p���e�[�u��
 */
//============================================================================================
static  const int pokevoice_pan_single_table[]={
   20, //POS_AA
  107, //POS_BB
};

static  const int pokevoice_pan_double_table[]={
   12, //POS_A
  115, //POS_B
   28, //POS_C
   99, //POS_D
};

static  const int pokevoice_pan_triple_table[]={
    0, //POS_A
  127, //POS_B
   20, //POS_C
  107, //POS_D
   40, //POS_E
   87, //POS_F
};

static  const int pokevoice_pan_rotate_table[]={
   12, //POS_A
  115, //POS_B
   28, //POS_C
   99, //POS_D
   20, //POS_E
  107, //POS_F
};

//============================================================================================
/**
 * @brief �V�X�e��������
 *
 * @param[in] tcb_sys �V�X�e�����Ŏg�p����TCBSYS�\���̂ւ̃|�C���^
 * @param[in] heapID  �q�[�vID
 */
//============================================================================================
BTLV_MCSS_WORK  *BTLV_MCSS_Init( BtlRule rule, GFL_TCBSYS *tcb_sys, HEAPID heapID )
{
  BTLV_MCSS_WORK *bmw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_MCSS_WORK ) );
  int i;

  bmw->mcss_sys = MCSS_Init( BTLV_MCSS_MAX, heapID );
  bmw->tcb_sys  = tcb_sys;
  bmw->heapID   = heapID;

  bmw->proj = BTLV_MCSS_PROJ_ORTHO;

  bmw->scale_ofs_m = FX32_ONE;          //�f�t�H���g�X�P�[���ɑ΂���I�t�Z�b�g
  bmw->scale_ofs_e = FX32_ONE;          //�f�t�H���g�X�P�[���ɑ΂���I�t�Z�b�g

  bmw->mcss_pos_3vs3 = ( rule == BTL_RULE_TRIPLE ) ? 1 : 0;
  bmw->mcss_pos_rotate = ( rule == BTL_RULE_ROTATION ) ? 1 : 0;

  for( i = 0 ; i < BTLV_MCSS_MAX ; i++ )
  {
    bmw->btlv_mcss[ i ].position = BTLV_MCSS_POS_ERROR;
  }

  return bmw;
}

//============================================================================================
/**
 * @brief �V�X�e���I��
 *
 * @param[in] bmw BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_Exit( BTLV_MCSS_WORK *bmw )
{
  BtlvMcssPos pos;

  for( pos = 0 ; pos < BTLV_MCSS_POS_TOTAL ; pos++ )
  { 
    if( BTLV_MCSS_CheckExist( bmw, pos ) )
    {
      BTLV_MCSS_Del( bmw, pos );
    }
  }
  MCSS_Exit( bmw->mcss_sys );
  GFL_HEAP_FreeMemory( bmw );
}

//============================================================================================
/**
 * @brief �V�X�e�����C��
 *
 * @param[in] bmw BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_Main( BTLV_MCSS_WORK *bmw )
{
  MCSS_Main( bmw->mcss_sys );
  {
    BtlvMcssPos pos;

    if( bmw->mcss_tcb_rotation_execute == 0 )
    {
      for( pos = BTLV_MCSS_POS_AA ; pos < BTLV_MCSS_POS_MAX ; pos++ )
      {
        //�܂΂���
        if( ( BTLV_MCSS_CheckExist( bmw, pos ) ) && ( GFL_STD_MtRand( 100 ) == 0 ) )
        {
          if( ( ( bmw->mcss_tcb_blink_execute & BTLV_EFFTOOL_Pos2Bit( pos ) ) == 0 ) &&
                ( BTLV_MCSS_GetAnmStopFlag( bmw, pos ) == BTLV_MCSS_ANM_STOP_OFF ) )
          {
            BTLV_MCSS_MoveBlink( bmw, pos, BTLEFF_MEPACHI_MABATAKI, 4, 1 );
          }
        }
        if( BTLV_EFFECT_CheckExecute() ) continue;
        //�ڂ��҂違�A�j���[�V�������x����
        if( BTLV_MCSS_CheckExist( bmw, pos ) )
        { 
          int color;
          int sick_anm;
          int index = BTLV_MCSS_GetIndex( bmw, pos );

          //�g���肪�łĂ���Ȃ牽�����Ȃ�
          if( BTLV_MCSS_GetStatusFlag( bmw, pos ) & BTLV_MCSS_STATUS_FLAG_MIGAWARI ) continue;

          if( bmw->btlv_mcss[ index ].sick_set_flag == 1 )
          { 
            if( bmw->evy_dir == 0 )
            { 
              bmw->evy_dir = 1;
            }
            if( bmw->evy_wait++ > BTLV_MCSS_EVY_WAIT )
            { 
              bmw->evy_wait = 0;
              bmw->evy += bmw->evy_dir;
              if( ( bmw->evy == 0 ) || ( bmw->evy == BTLV_MCSS_EVY_MAX ) )
              { 
                bmw->evy_dir *= -1;
              }
            }
          }

          if( BTLV_EFFECT_GetGaugeStatus( pos, &color, &sick_anm ) )
          { 
            switch( sick_anm ){ 
            case APP_COMMON_ST_ICON_NEMURI:      // ����
              BTLV_MCSS_ResetPaletteFadeBaseColor( bmw, pos );
              BTLV_MCSS_SetMepachiFlag( bmw, pos, BTLV_MCSS_MEPACHI_ALWAYS_ON );
              BTLV_MCSS_SetAnmSpeed( bmw, pos, FX32_ONE / 3 );
              bmw->btlv_mcss[ index ].sick_set_flag = 1;
              break;
            case APP_COMMON_ST_ICON_MAHI:        // ���
              BTLV_MCSS_SetPaletteFadeBaseColor( bmw, pos, bmw->evy, GX_RGB( 15, 15, 0 ) );
              bmw->btlv_mcss[ index ].sick_set_flag = 1;
              break;
            case APP_COMMON_ST_ICON_KOORI:       // �X
              BTLV_MCSS_SetPaletteFadeBaseColor( bmw, pos, 8, GX_RGB( 15, 15, 31 ) );
              BTLV_MCSS_SetAnmStopFlag( bmw, pos, BTLV_MCSS_ANM_STOP_ALWAYS_ON );
              bmw->btlv_mcss[ index ].sick_set_flag = 1;
              break;
            case APP_COMMON_ST_ICON_YAKEDO:      // �Ώ�
              BTLV_MCSS_SetPaletteFadeBaseColor( bmw, pos, bmw->evy, GX_RGB( 15, 0, 0 ) );
              bmw->btlv_mcss[ index ].sick_set_flag = 1;
              break;
            case APP_COMMON_ST_ICON_DOKU:        // ��
            case APP_COMMON_ST_ICON_DOKUDOKU:    // �ǂ��ǂ�
              BTLV_MCSS_SetPaletteFadeBaseColor( bmw, pos, bmw->evy, GX_RGB( 15, 0, 15 ) );
              bmw->btlv_mcss[ index ].sick_set_flag = 1;
              break;
            case APP_COMMON_ST_ICON_NONE:        // �Ȃ��i�A�j���ԍ��I�ɂ��Ȃ��j
            default:
              if( bmw->btlv_mcss[ index ].sick_set_flag == 1 )
              { 
                BTLV_MCSS_ResetPaletteFadeBaseColor( bmw, pos );
                BTLV_MCSS_SetMepachiFlag( bmw, pos, BTLV_MCSS_MEPACHI_ALWAYS_OFF );
                BTLV_MCSS_SetAnmStopFlag( bmw, pos, BTLV_MCSS_ANM_STOP_ALWAYS_OFF );
                BTLV_MCSS_SetAnmSpeed( bmw, pos, FX32_ONE );
                bmw->btlv_mcss[ index ].sick_set_flag = 0;
                bmw->evy_dir = 0;
                bmw->evy = 0;
                bmw->evy_wait = 0;
              }
              break;
            }
          }
          if( sick_anm == APP_COMMON_ST_ICON_NONE )
          { 
            if( ( color == GAUGETOOL_HP_DOTTO_RED ) || ( color == GAUGETOOL_HP_DOTTO_NULL ) )
            { 
              BTLV_MCSS_SetAnmSpeed( bmw, pos, FX32_ONE / 3 );
            }
            else
            { 
              BTLV_MCSS_SetAnmSpeed( bmw, pos, FX32_ONE );
            }
          }
        }
      }
    }
  }
}

//============================================================================================
/**
 * @brief �`��
 *
 * @param[in] bmw BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_Draw( BTLV_MCSS_WORK *bmw )
{
  MCSS_Draw( bmw->mcss_sys );
}

//============================================================================================
/**
 * @brief BTLV_MCSS�ǉ�
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] pp      POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_Add( BTLV_MCSS_WORK *bmw, const POKEMON_PARAM *pp, int position )
{
  VecFx32 pos;
  int index;

  for( index = 0 ; index < BTLV_MCSS_POS_TOTAL ; index++ )
  {
    if( bmw->btlv_mcss[ index ].mcss == NULL )
    {
      break;
    }
  }

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT( index < BTLV_MCSS_POS_TOTAL );
  if( position >= BTLV_MCSS_POS_TOTAL ) { return; }
  if( index >= BTLV_MCSS_POS_TOTAL ) { return; }
  GF_ASSERT_MSG( BTLV_MCSS_GetIndex( bmw, position ) == BTLV_MCSS_NO_INDEX, "pos=%d", position );

  bmw->btlv_mcss[ index ].status_flag = 0;
  bmw->btlv_mcss[ index ].mepachi_always_flag = 0;
  bmw->btlv_mcss[ index ].sick_set_flag       = 0;
  bmw->btlv_mcss[ index ].effect_vanish_flag  = 0;
  bmw->btlv_mcss[ index ].mcss_proj_mode = bmw->proj;

  //�����ʒu��ۑ�
  bmw->btlv_mcss[ index ].position = position;

  //���������擾���Ă���
  bmw->btlv_mcss[ index ].param.personal_rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );

  //�ߊl�{�[�����擾���Ă���
  bmw->btlv_mcss[ index ].capture_ball = PP_Get( pp, ID_PARA_get_ball, NULL );

  //���A���ǂ����H
  if( PP_CheckRare( pp ) == TRUE )
  {
    bmw->btlv_mcss[ index ].status_flag |= BTLV_MCSS_STATUS_FLAG_RARE;
  }

  BTLV_MCSS_MakeMAW( pp, &bmw->btlv_mcss[ index ].maw, position );
  MCSS_TOOL_SetMakeBuchiCallback( bmw->mcss_sys, bmw->btlv_mcss[ index ].param.personal_rnd );
  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->btlv_mcss[ index ].mcss = MCSS_Add( bmw->mcss_sys, pos.x, pos.y, pos.z, &bmw->btlv_mcss[ index ].maw );

  //�e�̃I�t�Z�b�g�𒲐�
  { 
    VecFx32 ofs;

    VEC_Set( &ofs, 0, 0, -1024 );
    MCSS_SetShadowOffset( bmw->btlv_mcss[ index ].mcss, &ofs );
  }

  { 
    MCSS_REVERSE_DRAW flag = MCSS_REVERSE_DRAW_OFF;

    switch( BTLV_EFFECT_GetBtlRule() ){ 
    case BTL_RULE_SINGLE:
    case BTL_RULE_ROTATION:
      break;
    case BTL_RULE_DOUBLE:
      switch( position ){ 
      case BTLV_MCSS_POS_B:
      case BTLV_MCSS_POS_C:
        flag = MCSS_REVERSE_DRAW_ON;
        break;
      }
      break;
    case BTL_RULE_TRIPLE:
      switch( position ){ 
      case BTLV_MCSS_POS_A:
      case BTLV_MCSS_POS_D:
      case BTLV_MCSS_POS_E:
        flag = MCSS_REVERSE_DRAW_ON;
        break;
      }
      break;
    }
    MCSS_SetReverseDraw( bmw->btlv_mcss[ index ].mcss, flag );
  }

  //�|�P�����̃i���o�[�A�t�H�����A�̏d�f�[�^���擾���Ă���
  {
    bmw->btlv_mcss[ index ].param.mons_no = PP_Get( pp, ID_PARA_monsno, NULL );
    bmw->btlv_mcss[ index ].param.form_no = PP_Get( pp, ID_PARA_form_no, NULL );
    bmw->btlv_mcss[ index ].param.weight = POKETOOL_GetPersonalParam( bmw->btlv_mcss[ index ].param.mons_no,
                                                                bmw->btlv_mcss[ index ].param.form_no, POKEPER_ID_weight );
  }
  //�o�ꎞ��HP�o�[�J���[���擾
  { 
    int hp    = PP_Get( pp, ID_PARA_hp, NULL );
    int hpmax = PP_Get( pp, ID_PARA_hpmax, NULL );
    bmw->btlv_mcss[ index ].param.appear_hp_color = GAUGETOOL_GetGaugeDottoColor( hp, hpmax );
  }

  bmw->btlv_mcss[ index ].stop_anime_count = BTLV_MCSS_STOP_ANIME_COUNT;

  BTLV_MCSS_SetDefaultScale( bmw, position );

  MCSS_SetAnimCtrlCallBack( bmw->btlv_mcss[ index ].mcss, index, BTLV_MCSS_CallBackFunctorFrame, 1 );
}

//============================================================================================
/**
 * @brief BTLV_MCSS�ǉ�
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] pp      POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_AddTrainer( BTLV_MCSS_WORK *bmw, int tr_type, int position )
{
  MCSS_ADD_WORK maw;
  VecFx32 pos;
  int index;

  for( index = 0 ; index < BTLV_MCSS_POS_TOTAL ; index++ )
  {
    if( bmw->btlv_mcss[ index ].mcss == NULL )
    {
      break;
    }
  }

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT( index < BTLV_MCSS_POS_TOTAL );
  if( position >= BTLV_MCSS_POS_TOTAL ) { return; }
  if( index >= BTLV_MCSS_POS_TOTAL ) { return; }
  GF_ASSERT_MSG( BTLV_MCSS_GetIndex( bmw, position ) == BTLV_MCSS_NO_INDEX, "pos=%d", position );

  bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_ORTHO;

  //�����ʒu��ۑ�
  bmw->btlv_mcss[ index ].position = position;

  BTLV_MCSS_MakeMAWTrainer( tr_type, &maw, position );
  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->btlv_mcss[ index ].mcss = MCSS_Add( bmw->mcss_sys, pos.x, pos.y, pos.z, &maw );

  BTLV_MCSS_SetDefaultScale( bmw, position );
}

//============================================================================================
/**
 * @brief BTLV_MCSS�폜
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_Del( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );

  MCSS_Del( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss );
  bmw->btlv_mcss[ index ].mcss = NULL;
  if( bmw->btlv_mcss[ index ].tcb )
  {
    GFL_HEAP_FreeMemory( GFL_TCB_GetWork( bmw->btlv_mcss[ index ].tcb ) );
    GFL_TCB_DeleteTask( bmw->btlv_mcss[ index ].tcb );
    bmw->btlv_mcss[ index ].tcb = NULL;
  }
}

//============================================================================================
/**
 * @brief �X�P�[���I�t�Z�b�g�Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] scale_ofs �Z�b�g����X�P�[���I�t�Z�b�g
 */
//============================================================================================
void  BTLV_MCSS_SetScaleOffset( BTLV_MCSS_WORK *bmw, fx32 scale_ofs_m, fx32 scale_ofs_e )
{
  bmw->scale_ofs_m = scale_ofs_m;
  bmw->scale_ofs_e = scale_ofs_e;
}

//============================================================================================
/**
 * @brief ���W�Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  ���W�Z�b�g����MCSS�̗����ʒu
 * @param[in] pos_x     X���W
 * @param[in] pos_y     Y���W
 * @param[in] pos_z     Z���W
 */
//============================================================================================
void  BTLV_MCSS_SetPosition( BTLV_MCSS_WORK *bmw, int position, fx32 pos_x, fx32 pos_y, fx32 pos_z )
{
  VecFx32 pos;
  int index = BTLV_MCSS_GetIndex( bmw, position );

  pos.x = pos_x;
  pos.y = pos_y;
  pos.z = pos_z;

  MCSS_SetPosition( bmw->btlv_mcss[ index ].mcss, &pos );
}

//============================================================================================
/**
 * @brief ���ˉe�`�惂�[�hON
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_SetOrthoMode( BTLV_MCSS_WORK *bmw )
{
  int position;

  MCSS_SetOrthoMode( bmw->mcss_sys );
  bmw->proj = BTLV_MCSS_PROJ_ORTHO;

  for( position = 0 ; position < BTLV_MCSS_POS_TOTAL ; position++ )
  {
    int index = BTLV_MCSS_GetIndex( bmw, position );
    if( index != BTLV_MCSS_NO_INDEX )
    {
      bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_ORTHO;
      BTLV_MCSS_SetDefaultScale( bmw, position );
    }
  }
}

//============================================================================================
/**
 * @brief ���ˉe�`�惂�[�hOFF
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_ResetOrthoMode( BTLV_MCSS_WORK *bmw )
{
  int position;

  MCSS_ResetOrthoMode( bmw->mcss_sys );
  bmw->proj = BTLV_MCSS_PROJ_PERSPECTIVE;

  for( position = 0 ; position < BTLV_MCSS_POS_TOTAL ; position++ )
  {
    int index = BTLV_MCSS_GetIndex( bmw, position );
    if( index != BTLV_MCSS_NO_INDEX )
    {
      bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_PERSPECTIVE;
      BTLV_MCSS_SetDefaultScale( bmw, position );
    }
  }
}

//============================================================================================
/**
 * @brief ���ˉe�`�惂�[�hON
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_SetOrthoModeByPos( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos )
{
  int index = BTLV_MCSS_GetIndex( bmw, pos );
  if( index != BTLV_MCSS_NO_INDEX )
  {
    bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_ORTHO;
    MCSS_SetOrthoModeMcss( bmw->btlv_mcss[ index ].mcss );
    BTLV_MCSS_SetDefaultScale( bmw, pos );
  }
}

//============================================================================================
/**
 * @brief ���ˉe�`�惂�[�hOFF
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_ResetOrthoModeByPos( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos )
{
  int index = BTLV_MCSS_GetIndex( bmw, pos );
  if( index != BTLV_MCSS_NO_INDEX )
  {
    bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_PERSPECTIVE;
    MCSS_ResetOrthoModeMcss( bmw->btlv_mcss[ index ].mcss );
    BTLV_MCSS_SetDefaultScale( bmw, pos );
  }
}

//============================================================================================
/**
 * @brief ���p�`����
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  ���p�`���������|�P�����̗����ʒu
 * @param[in] flag    ���p�`�t���O�iBTLV_MCSS_MEPACHI_ON�ABTLV_MCSS_MEPACHI_OFF�j
 */
//============================================================================================
void  BTLV_MCSS_SetMepachiFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  if( flag == BTLV_MCSS_MEPACHI_ALWAYS_OFF )
  { 
    MCSS_ResetMepachiFlag( bmw->btlv_mcss[ index ].mcss );
    bmw->btlv_mcss[ index ].mepachi_always_flag = 0;
  }

  //��ɖڂ��Ԃ�t���O�������Ă���Ƃ��͉��������Ƀ��^�[��
  if( bmw->btlv_mcss[ index ].mepachi_always_flag )
  { 
    return;
  }

  if( flag == BTLV_MCSS_MEPACHI_FLIP )
  {
    MCSS_FlipMepachiFlag( bmw->btlv_mcss[ index ].mcss );
  }
  else if( flag == BTLV_MCSS_MEPACHI_ON ){
    MCSS_SetMepachiFlag( bmw->btlv_mcss[ index ].mcss );
  }
  else if( flag == BTLV_MCSS_MEPACHI_ALWAYS_ON )
  {
    MCSS_SetMepachiFlag( bmw->btlv_mcss[ index ].mcss );
    bmw->btlv_mcss[ index ].mepachi_always_flag = 1;
  }
  else
  {
    MCSS_ResetMepachiFlag( bmw->btlv_mcss[ index ].mcss );
  }
}

//============================================================================================
/**
 * @brief �A�j���X�g�b�v�t���O�擾
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �A�j���X�g�b�v���������|�P�����̗����ʒu
 * @param[in] flag    �A�j���X�g�b�v�t���O�iBTLV_MCSS_ANM_STOP_ON�ABTLV_MCSS_ANM_STOP_OFF�j
 */
//============================================================================================
int  BTLV_MCSS_GetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position )
{ 
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return  MCSS_GetAnmStopFlag( bmw->btlv_mcss[ index ].mcss );
}

//============================================================================================
/**
 * @brief �A�j���X�g�b�v����
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �A�j���X�g�b�v���������|�P�����̗����ʒu
 * @param[in] flag    �A�j���X�g�b�v�t���O�iBTLV_MCSS_ANM_STOP_ON�ABTLV_MCSS_ANM_STOP_OFF�j
 */
//============================================================================================
void  BTLV_MCSS_SetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  if( flag == BTLV_MCSS_ANM_STOP_ON ){
    MCSS_SetAnmStopFlag( bmw->btlv_mcss[ index ].mcss );
  }
  else if( flag == BTLV_MCSS_ANM_STOP_ALWAYS_ON )
  {
    MCSS_SetAnmStopFlagAlways( bmw->btlv_mcss[ index ].mcss );
  }
  else if( flag == BTLV_MCSS_ANM_STOP_ALWAYS_OFF )
  {
    MCSS_ResetAnmStopFlagAlways( bmw->btlv_mcss[ index ].mcss );
  }
  else{
    MCSS_ResetAnmStopFlag( bmw->btlv_mcss[ index ].mcss );
  }
}

//============================================================================================
/**
 * @brief �o�j�b�V���t���O�擾
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �t���O���삳�������|�P�����̗����ʒu
 */
//============================================================================================
BTLV_MCSS_VANISH_FLAG   BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return MCSS_GetVanishFlag( bmw->btlv_mcss[ index ].mcss );
}

//============================================================================================
/**
 * @brief �o�j�b�V������
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �t���O���삳�������|�P�����̗����ʒu
 * @param[in] flag    �o�j�b�V���t���O�iBTLV_MCSS_VANISH_ON�ABTLV_MCSS_VANISH_OFF�j
 */
//============================================================================================
void  BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, BTLV_MCSS_VANISH_FLAG flag )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  switch( flag ){ 
  case BTLV_MCSS_VANISH_FLIP:
    MCSS_FlipVanishFlag( bmw->btlv_mcss[ index ].mcss );
    break;
  case BTLV_MCSS_VANISH_ON:
    MCSS_SetVanishFlag( bmw->btlv_mcss[ index ].mcss );
    break;
  case BTLV_MCSS_VANISH_OFF:
    MCSS_ResetVanishFlag( bmw->btlv_mcss[ index ].mcss );
    break;
  case BTLV_MCSS_EFFECT_VANISH_ON:
    //���łɏ����Ă���Ȃ�A�t���O�𗧂Ă�
    if( MCSS_GetVanishFlag( bmw->btlv_mcss[ index ].mcss ) )
    { 
      bmw->btlv_mcss[ index ].effect_vanish_flag = 1;
    }
    MCSS_SetVanishFlag( bmw->btlv_mcss[ index ].mcss );
    break;
  case BTLV_MCSS_EFFECT_VANISH_OFF:
    //���łɏ����Ă����t���O�������Ă�����A�t���O�𗎂Ƃ������ɂ���
    if( bmw->btlv_mcss[ index ].effect_vanish_flag )
    { 
      bmw->btlv_mcss[ index ].effect_vanish_flag = 0;
    }
    else
    { 
      MCSS_ResetVanishFlag( bmw->btlv_mcss[ index ].mcss );
    }
    break;
  }
}

//============================================================================================
/**
 * @brief �|�P�����̏��������ʒu���擾
 *
 * @param[out]  pos     ���������ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in] position  �擾����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_GetPokeDefaultPos( BTLV_MCSS_WORK *bmw, VecFx32 *pos, int position )
{
  BTLV_MCSS_GetDefaultPos( bmw, pos, position );
}

//============================================================================================
/**
 * @brief �|�P�����̏����g�k�����擾
 *
 * @param[in] position  �擾����|�P�����̗����ʒu
 */
//============================================================================================
fx32  BTLV_MCSS_GetPokeDefaultScale( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return FX32_ONE; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return FX32_ONE; }

  return BTLV_MCSS_GetPokeDefaultScaleEx( bmw, position, bmw->btlv_mcss[ index ].mcss_proj_mode );
}

//============================================================================================
/**
 * @brief �|�P�����̏����g�k�����擾
 *
 * @param[in] position  �擾����|�P�����̗����ʒu
 * @param[in] proj      �擾����ˉe���@
 */
//============================================================================================
fx32  BTLV_MCSS_GetPokeDefaultScaleEx( BTLV_MCSS_WORK *bmw, int position, BTLV_MCSS_PROJECTION proj )
{
  return BTLV_MCSS_GetDefaultScale( bmw, position, proj );
}

//============================================================================================
/**
 * @brief �|�P�����̃X�P�[���l���擾
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �擾����|�P�����̗����ʒu
 * @param[out]  scale   �擾�����X�P�[���l���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_GetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_GetScale( bmw->btlv_mcss[ index ].mcss, scale );
}

//============================================================================================
/**
 * @brief �|�P�����̃X�P�[���l���i�[
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �i�[����|�P�����̗����ʒu
 * @param[in] scale   �i�[����X�P�[���l
 */
//============================================================================================
void  BTLV_MCSS_SetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale )
{
  VecFx32 shadow_scale;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetScale( bmw->btlv_mcss[ index ].mcss, scale );
}

//============================================================================================
/**
 * @brief �|�P�����̉e�o�j�b�V���t���O���i�[
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �i�[����|�P�����̗����ʒu
 * @param[in] flag      �i�[����X�P�[���l
 */
//============================================================================================
void  BTLV_MCSS_SetShadowVanishFlag( BTLV_MCSS_WORK *bmw, int position, u8 flag )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetShadowVanishFlag( bmw->btlv_mcss[ index ].mcss, flag );
}

//============================================================================================
/**
 * @brief �A�j���[�V�����t���[���X�s�[�h�Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �Z�b�g����MCSS�̗����ʒu
 * @param[in] speed     �Z�b�g����X�s�[�h
 */
//============================================================================================
void  BTLV_MCSS_SetAnmSpeed( BTLV_MCSS_WORK *bmw, int position, fx32 speed )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetAnimeFrame( bmw->btlv_mcss[ index ].mcss, speed );
}

//============================================================================================
/**
 * @brief �|�P�����ړ�
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �ړ�����|�P�����̗����ʒu
 * @param[in] type    �ړ��^�C�v
 * @param[in] pos     �ړ��^�C�v�ɂ��Ӗ����ω�
 *              EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ړ���
 *              EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   �ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    �ړ��E�G�C�g
 * @param[in] count   �����J�E���g�iPOKEMCSS_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MovePosition( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *pos, int frame, int wait, int count )
{
  VecFx32 start;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_GetPosition( bmw->btlv_mcss[ index ].mcss, &start );
  //�ړ��̕�Ԃ͑��Ύw��Ƃ���
  if( type == EFFTOOL_CALCTYPE_INTERPOLATION )
  {
    //�|�P�����̎����̕����Ł{�|�����肷��
    if( position & 1 )
    {
      pos->x = start.x - pos->x;
      pos->y += start.y;
      pos->z += start.z;
    }
    else
    {
      pos->x += start.x;
      pos->y += start.y;
      pos->z += start.z;
    }
  }
  //�����ʒu�ړ�
  if( type == BTLEFF_POKEMON_MOVE_INIT )
  {
    BTLV_MCSS_GetDefaultPos( bmw, pos, position );
    type = EFFTOOL_CALCTYPE_INTERPOLATION;
  }
  if( type == EFFTOOL_CALCTYPE_DIRECT )     //���ڒl����
  {
    MCSS_SetPosition( bmw->btlv_mcss[ index ].mcss, pos );
    return;
  }
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, pos, frame, wait, count,
                           TCB_BTLV_MCSS_Move, TCB_BTLV_MCSS_Move_CB, REVERSE_FLAG_ON );
  bmw->mcss_tcb_move_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �|�P�����g�k
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �g�k����|�P�����̗����ʒu
 * @param[in] type    �g�k�^�C�v
 * @param[in] scale   �g�k�^�C�v�ɂ��Ӗ����ω�
 *              EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȃX�P�[���l
 *              EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   �g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    �g�k�E�G�C�g
 * @param[in] count   �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MoveScale( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *scale, int frame, int wait, int count )
{
  VecFx32 start;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_GetOfsScale( bmw->btlv_mcss[ index ].mcss, &start );
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, scale, frame, wait, count,
                           TCB_BTLV_MCSS_Scale, TCB_BTLV_MCSS_Scale_CB, REVERSE_FLAG_OFF );
  bmw->mcss_tcb_scale_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �|�P�����g�k�i�f�t�H���g�l�𑀍�j
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �g�k����|�P�����̗����ʒu
 * @param[in] type    �g�k�^�C�v
 * @param[in] scale   �g�k�^�C�v�ɂ��Ӗ����ω�
 *              EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȃX�P�[���l
 *              EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   �g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    �g�k�E�G�C�g
 * @param[in] count   �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MoveDefaultScale( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *scale, int frame, int wait, int count )
{
  VecFx32 start;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_GetScale( bmw->btlv_mcss[ index ].mcss, &start );
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, scale, frame, wait, count,
                           TCB_BTLV_MCSS_DefaultScale, TCB_BTLV_MCSS_DefaultScale_CB, REVERSE_FLAG_OFF );
  bmw->mcss_tcb_default_scale_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �|�P������]
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  ��]����|�P�����̗����ʒu
 * @param[in] type    ��]�^�C�v
 * @param[in] rotate    ��]�^�C�v�ɂ��Ӗ����ω�
 *              EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȉ�]�l
 *              EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame   ��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait    ��]�E�G�C�g
 * @param[in] count   �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MoveRotate( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *rotate, int frame, int wait, int count )
{
  VecFx32 start;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_GetRotate( bmw->btlv_mcss[ index ].mcss, &start );
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, rotate, frame, wait, count,
                           TCB_BTLV_MCSS_Rotate, TCB_BTLV_MCSS_Rotate_CB, REVERSE_FLAG_ON );
  bmw->mcss_tcb_rotate_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu�̃|�P���������ւ�
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] pos1  ����ւ���BtlvMcssPos
 * @param[in] pos2  ����ւ���BtlvMcssPos
 */
//============================================================================================
void  BTLV_MCSS_SetSideChange( BTLV_MCSS_WORK* bmw, BtlvMcssPos pos1, BtlvMcssPos pos2 )
{ 
  int position[ 2 ];

  position[ 0 ] = pos1;
  position[ 1 ] = pos2;

  { 
    int i;
    int index[ 2 ];

    for( i = 0 ; i < 2 ; i++ )
    { 
      index[ i ] = BTLV_MCSS_GetIndex( bmw, position[ i ] );
      if( BTLV_MCSS_CheckExist( bmw, position[ i ] ) )
      {
        VecFx32 pos;
        BTLV_MCSS_GetDefaultPos( bmw, &pos, position[ i ^ 1 ] );
        BTLV_MCSS_MovePosition( bmw, position[ i ], EFFTOOL_CALCTYPE_DIRECT, &pos, 0, 0, 0 );
        if( bmw->btlv_mcss[ index[ i ] ].set_init_pos )
        { 
          bmw->btlv_mcss[ index[ i ] ].set_init_pos = 0;
          BTLV_MCSS_SetVanishFlag( bmw, position[ i ], BTLV_MCSS_VANISH_OFF );
        }
      }
    }
    for( i = 0 ; i < 2 ; i++ )
    { 
      if( index[ i ] != BTLV_MCSS_NO_INDEX )
      { 
        bmw->btlv_mcss[ index[ i ] ].position = position[ i ^ 1 ];
      }
    }
  }
}

//============================================================================================
/**
 * @brief �|�P�����܂΂���
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �܂΂�����|�P�����̗����ʒu
 * @param[in] type    �܂΂����^�C�v
 * @param[in] wait    �܂΂����E�G�C�g
 * @param[in] count   �܂΂����J�E���g
 */
//============================================================================================
void  BTLV_MCSS_MoveBlink( BTLV_MCSS_WORK *bmw, int position, int type, int wait, int count )
{
  switch( type ){
  case BTLEFF_MEPACHI_ON:
    BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_ON );
    break;
  case BTLEFF_MEPACHI_OFF:
    BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_OFF );
    break;
  case BTLEFF_MEPACHI_MABATAKI:
    { 
      BTLV_MCSS_TCB_WORK  *bmtw = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bmw->heapID ), sizeof( BTLV_MCSS_TCB_WORK ) );

      bmtw->bmw           = bmw;
      bmtw->position      = position;
      bmtw->emw.move_type = type;
      bmtw->emw.wait      = 0;
      bmtw->emw.wait_tmp  = wait;
      bmtw->emw.count     = count * 2;  //���ĊJ����1��ƃJ�E���g���邽�߂ɔ{���Ă���

      BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_Blink, bmtw, 0 ), TCB_BTLV_MCSS_Blink_CB, GROUP_MCSS );
      bmw->mcss_tcb_blink_execute |= BTLV_EFFTOOL_Pos2Bit( position );
    }
    break;
  }
}

//============================================================================================
/**
 * @brief �|�P�������l
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  ���l�Z�b�g����|�P�����̗����ʒu
 * @param[in] type      ���l�^�C�v
 * @param[in] alpha     ���l�^�C�v�ɂ��Ӗ����ω�
 *                      EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȃ��l
 *                      EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame     �t���[�����i�ݒ肵�����l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait      �E�G�C�g
 * @param[in] count     �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MoveAlpha( BTLV_MCSS_WORK *bmw, int position, int type, int alpha, int frame, int wait, int count )
{
  VecFx32 start;
  VecFx32 end;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  start.x = FX32_CONST( MCSS_GetAlpha( bmw->btlv_mcss[ index ].mcss ) );
  start.y = 0;
  start.z = 0;
  end.x = FX32_CONST( alpha );
  end.y = 0;
  end.z = 0;
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, &end, frame, wait, count,
                           TCB_BTLV_MCSS_Alpha, TCB_BTLV_MCSS_Alpha_CB, REVERSE_FLAG_OFF );
  bmw->mcss_tcb_alpha_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �|�P�����~�^��
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] bmmcp     �~�^���p�����[�^�\����
 */
//============================================================================================
void  BTLV_MCSS_MoveCircle( BTLV_MCSS_WORK *bmw, BTLV_MCSS_MOVE_CIRCLE_PARAM* bmmcp )
{
  BTLV_MCSS_MOVE_CIRCLE_PARAM* bmmcp_p = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bmw->heapID ),
                                                               sizeof( BTLV_MCSS_MOVE_CIRCLE_PARAM ) );

  bmmcp_p->bmw                      = bmw;
  bmmcp_p->position                 = bmmcp->position;
  bmmcp_p->axis                     = bmmcp->axis;
  bmmcp_p->shift                    = bmmcp->shift;
  bmmcp_p->radius_h                 = bmmcp->radius_h;
  bmmcp_p->radius_v                 = bmmcp->radius_v;
  bmmcp_p->frame                    = bmmcp->frame;
  bmmcp_p->rotate_wait              = bmmcp->rotate_wait;
  bmmcp_p->count                    = bmmcp->count;
  bmmcp_p->rotate_after_wait        = bmmcp->rotate_after_wait;
  bmmcp_p->rotate_wait_count        = 0;
  bmmcp_p->rotate_after_wait_count  = 0;

  //�|�P�����̌����ɂ���āA�{�|������
  if( bmmcp_p->position & 1 )
  {
    if( ( ( bmmcp_p->axis == BTLEFF_AXIS_X_L ) || ( bmmcp_p->axis == BTLEFF_AXIS_X_R ) ||
          ( bmmcp_p->axis == BTLEFF_AXIS_Z_L ) || ( bmmcp_p->axis == BTLEFF_AXIS_Z_R ) ) &&
        ( ( bmmcp->shift == BTLEFF_SHIFT_V_P ) || ( bmmcp->shift == BTLEFF_SHIFT_V_M ) ) )
    {
      ;
    }
    else
    {
      bmmcp_p->shift ^= 1;
    }
  }
  if( bmmcp_p->axis & 1 )
  {
    bmmcp_p->angle  = 0x10000;
  }
  else
  {
    bmmcp_p->angle  = 0;
  }
  bmmcp_p->speed  = 0x10000 / bmmcp->frame;

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_MoveCircle, bmmcp_p, 0 ),
                      TCB_BTLV_MCSS_MoveCircle_CB, GROUP_MCSS );
  bmw->mcss_tcb_move_execute |= BTLV_EFFTOOL_Pos2Bit( bmmcp_p->position );
}

//============================================================================================
/**
 * @brief �|�P�����~�^��
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] bmmcp     �~�^���p�����[�^�\����
 */
//============================================================================================
void  BTLV_MCSS_MoveSin( BTLV_MCSS_WORK *bmw, BTLV_MCSS_MOVE_SIN_PARAM* bmmsp )
{
  BTLV_MCSS_MOVE_SIN_PARAM* bmmsp_p = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bmw->heapID ),
                                                            sizeof( BTLV_MCSS_MOVE_SIN_PARAM ) );
  int index = BTLV_MCSS_GetIndex( bmw, bmmsp->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  *bmmsp_p      = *bmmsp;
  bmmsp_p->bmw  = bmw;
  MCSS_GetPosition( bmw->btlv_mcss[ index ].mcss, &bmmsp_p->pos );

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_MoveSin, bmmsp_p, 0 ),
                      TCB_BTLV_MCSS_MoveSin_CB, GROUP_MCSS );
  bmw->mcss_tcb_move_execute |= BTLV_EFFTOOL_Pos2Bit( bmmsp_p->position );
}

//============================================================================================
/**
 * @brief �|�P�������U�C�N
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  ���U�C�N����|�P�����̗����ʒu
 * @param[in] type      ���U�C�N�^�C�v
 * @param[in] alpha     ���U�C�N�^�C�v�ɂ��Ӗ����ω�
 *                      EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION  �ŏI�I�ȃ��U�C�N�l
 *                      EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in] frame     �t���[�����i�ݒ肵�����U�C�N�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in] wait      �E�G�C�g
 * @param[in] count     �����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void  BTLV_MCSS_MoveMosaic( BTLV_MCSS_WORK *bmw, int position, int type, int mosaic, int frame, int wait, int count )
{
  VecFx32 start;
  VecFx32 end;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  start.x = FX32_CONST( MCSS_GetMosaic( bmw->btlv_mcss[ index ].mcss ) );
  start.y = 0;
  start.z = 0;
  end.x = FX32_CONST( mosaic );
  end.y = 0;
  end.z = 0;
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, &end, frame, wait, count,
                           TCB_BTLV_MCSS_Mosaic, TCB_BTLV_MCSS_Mosaic_CB, REVERSE_FLAG_OFF );
  bmw->mcss_tcb_mosaic_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �^�X�N���N�������`�F�b�N
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 *
 * @retval: TRUE:�N�����@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_MCSS_CheckTCBExecute( BTLV_MCSS_WORK *bmw, int position )
{
  BOOL  pal_fade_flag = FALSE;
  BOOL  anm_execute_flag = FALSE;

  if( BTLV_MCSS_CheckExist( bmw, position ) )
  {
    int index = BTLV_MCSS_GetIndex( bmw, position );
    pal_fade_flag = MCSS_CheckExecutePaletteFade( bmw->btlv_mcss[ index ].mcss );
  }

  return ( ( bmw->mcss_tcb_move_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_scale_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_default_scale_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_rotate_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_blink_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_alpha_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_mosaic_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_rotation_execute ) ||
       ( pal_fade_flag ) );
}

//============================================================================================
/**
 * @brief �^�X�N���N�������`�F�b�N�i���ׂĂ̗����ʒu�j
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval: TRUE:�N�����@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_MCSS_CheckTCBExecuteAllPos( BTLV_MCSS_WORK *bmw )
{
  BtlvMcssPos pos;

  for( pos = BTLV_MCSS_POS_AA ; pos < BTLV_MCSS_POS_MAX ; pos++ )
  {
    if( BTLV_MCSS_CheckExist( bmw, pos ) )
    {
      if( BTLV_MCSS_CheckTCBExecute( bmw, pos ) )
      {
        return TRUE;
      }
    }
  }
  return FALSE;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu�ɑ��݂��邩�`�F�b�N
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �`�F�b�N���闧���ʒu
 *
 * @retval  TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  BTLV_MCSS_CheckExist( BTLV_MCSS_WORK *bmw, int position )
{
  return ( BTLV_MCSS_GetIndex( bmw, position ) != BTLV_MCSS_NO_INDEX );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�Ƀp���b�g�t�F�[�h���Z�b�g����
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 * @param[in] start_evy �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in] end_evy   �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in] wait      �Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in] rgb       �Z�b�g����p�����[�^�i�t�F�[�h������F�j
 *
 */
//============================================================================================
void  BTLV_MCSS_SetPaletteFade( BTLV_MCSS_WORK *bmw, int position, u8 start_evy, u8 end_evy, u8 wait, u32 rgb )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetPaletteFade( bmw->btlv_mcss[ index ].mcss, start_evy, end_evy, wait, rgb );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�Ƀp���b�g�t�F�[�h�x�[�X�J���[���Z�b�g����
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 * @param[in] evy       �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in] rgb       �Z�b�g����p�����[�^�i�t�F�[�h������F�j
 *
 */
//============================================================================================
void  BTLV_MCSS_SetPaletteFadeBaseColor( BTLV_MCSS_WORK *bmw, int position, u8 evy, u32 rgb )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetPaletteFadeBaseColor( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, evy, rgb );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�Ƀp���b�g�t�F�[�h�x�[�X�J���[�����Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
void  BTLV_MCSS_ResetPaletteFadeBaseColor( BTLV_MCSS_WORK *bmw, int position )
{ 
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_ResetPaletteFadeBaseColor( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�̃|�P����No�f�[�^���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
int  BTLV_MCSS_GetMonsNo( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].param.mons_no;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�Ƀ|�P����No�f�[�^���Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 * @param[in] mons_no   �Z�b�g����|�P����No
 *
 */
//============================================================================================
void  BTLV_MCSS_SetMonsNo( BTLV_MCSS_WORK *bmw, int position, int mons_no )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  bmw->btlv_mcss[ index ].param.mons_no = mons_no;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�̃t�H����No�f�[�^���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
int  BTLV_MCSS_GetFormNo( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].param.form_no;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�̑̏d�f�[�^���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
u16  BTLV_MCSS_GetWeight( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].param.weight;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS��HP�����J���[�f�[�^���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
u16  BTLV_MCSS_GetHPColor( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].param.appear_hp_color;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��status_flag���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
u32  BTLV_MCSS_GetStatusFlag( BTLV_MCSS_WORK *bmw, int position )
{
  int index;
  if( position == BTLV_MCSS_POS_ERROR )
  {
    return 0;
  }
  index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].status_flag;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��no_jump���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
BOOL  BTLV_MCSS_GetNoJump( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return FALSE; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return FALSE; }

  return ( POKETOOL_GetPersonalParam( bmw->btlv_mcss[ index ].param.mons_no, bmw->btlv_mcss[ index ].param.form_no,
                                      POKEPER_ID_no_jump ) != 0 );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��fly_flag���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 *
 */
//============================================================================================
u8  BTLV_MCSS_GetFlyFlag( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return MCSS_GetFlyFlag( bmw->btlv_mcss[ index ].mcss );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu��MCSS�ɂ݂����Z�b�g
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  MCSS�̗����ʒu
 * @param[in] sw        �݂����Z�b�gor���Z�b�g
 * @param[in] flag      BTLV_MCSS�ŊǗ����Ă���t���O�̗����������ǂ����邩�iTRUE:������������@FALSE:���Ȃ��j
 *
 */
//============================================================================================
void  BTLV_MCSS_SetMigawari( BTLV_MCSS_WORK* bmw, int position, int sw, BOOL flag )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  if( sw == BTLEFF_MIGAWARI_OFF )
  {
    //�ēx�Ԃ������Ȃ���΂����Ȃ��̂ŃR�[���o�b�N��ݒ肵�Ă���
    MCSS_TOOL_SetMakeBuchiCallback( bmw->mcss_sys, bmw->btlv_mcss[ index ].param.personal_rnd );
    MCSS_ReloadResource( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, &bmw->btlv_mcss[ index ].maw );
    if( flag )
    {
      bmw->btlv_mcss[ index ].status_flag &= BTLV_MCSS_STATUS_FLAG_MIGAWARI_OFF;
    }
  }
  else
  {
    MCSS_ADD_WORK maw;
    maw.arcID = POKEGRA_GetArcID();
    maw.ncbr = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NCBR : NARC_pokegra_wb_pbwb_migawari_NCBR;
    maw.nclr = NARC_pokegra_wb_pmwb_migawari_n_NCLR;
    maw.ncer = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NCER : NARC_pokegra_wb_pbwb_migawari_NCER;
    maw.nanr = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NANR : NARC_pokegra_wb_pbwb_migawari_NANR;
    maw.nmcr = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NMCR : NARC_pokegra_wb_pbwb_migawari_NMCR;
    maw.nmar = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NMAR : NARC_pokegra_wb_pbwb_migawari_NMAR;
    maw.ncec = position & 1 ? NARC_pokegra_wb_pfwb_migawari_NCEC : NARC_pokegra_wb_pbwb_migawari_NCEC;
    MCSS_ReloadResource( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, &maw );
    BTLV_MCSS_ResetPaletteFadeBaseColor( bmw, position );
    if( flag )
    {
      bmw->btlv_mcss[ index ].status_flag |= BTLV_MCSS_STATUS_FLAG_MIGAWARI;
    }
  }
}

//============================================================================================
/**
 * @brief ���U�C�N
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �Z�b�g����MCSS�̗����ʒu
 * @param[in] speed     �Z�b�g����X�s�[�h
 */
//============================================================================================
void  BTLV_MCSS_SetMosaic( BTLV_MCSS_WORK *bmw, int position, int mosaic )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetMosaic( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, mosaic );
}

//============================================================================================
/**
 * @brief MAW�\���̂��R�s�[���ă��\�[�X�������[�h
 *
 * @param[in] bmw BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] src �R�s�[��MCSS�̗����ʒu
 * @param[in] dst �R�s�[��MCSS�̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_CopyMAW( BTLV_MCSS_WORK *bmw, int src, int dst )
{
  int src_index = BTLV_MCSS_GetIndex( bmw, src );
  int dst_index = BTLV_MCSS_GetIndex( bmw, dst );
  bmw->btlv_mcss[ dst_index ].maw.arcID = bmw->btlv_mcss[ src_index ].maw.arcID;
  bmw->btlv_mcss[ dst_index ].maw.nclr = bmw->btlv_mcss[ src_index ].maw.nclr;
  //���ʁ��w��
  if( src & 1 )
  {
    bmw->btlv_mcss[ dst_index ].maw.ncbr = bmw->btlv_mcss[ src_index ].maw.ncbr + POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.ncer = bmw->btlv_mcss[ src_index ].maw.ncer + POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nanr = bmw->btlv_mcss[ src_index ].maw.nanr + POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nmcr = bmw->btlv_mcss[ src_index ].maw.nmcr + POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nmar = bmw->btlv_mcss[ src_index ].maw.nmar + POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.ncec = bmw->btlv_mcss[ src_index ].maw.ncec + POKEGRA_MAX;
  }
  //�w�ʁ�����
  else
  {
    bmw->btlv_mcss[ dst_index ].maw.ncbr = bmw->btlv_mcss[ src_index ].maw.ncbr - POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.ncer = bmw->btlv_mcss[ src_index ].maw.ncer - POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nanr = bmw->btlv_mcss[ src_index ].maw.nanr - POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nmcr = bmw->btlv_mcss[ src_index ].maw.nmcr - POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.nmar = bmw->btlv_mcss[ src_index ].maw.nmar - POKEGRA_MAX;
    bmw->btlv_mcss[ dst_index ].maw.ncec = bmw->btlv_mcss[ src_index ].maw.ncec - POKEGRA_MAX;
  }
  bmw->btlv_mcss[ dst_index ].param = bmw->btlv_mcss[ src_index ].param;
  MCSS_ReloadResource( bmw->mcss_sys, bmw->btlv_mcss[ dst_index ].mcss, &bmw->btlv_mcss[ dst_index ].maw );
}

//============================================================================================
/**
 * @brief MAW�\���̂��㏑�����ă��\�[�X�������[�h
 *
 * @param[in] bmw BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] pos �㏑�����MCSS�̗����ʒu
 * @param[in] maw �㏑������MAW�\����
 */
//============================================================================================
void  BTLV_MCSS_OverwriteMAW( BTLV_MCSS_WORK *bmw, BtlvMcssPos pos, MCSS_ADD_WORK* maw )
{
  int index = BTLV_MCSS_GetIndex( bmw, pos );
  bmw->btlv_mcss[ index ].maw = *maw;
  MCSS_ReloadResource( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, &bmw->btlv_mcss[ index ].maw );
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�����ʒu�̃|�P�����̖������Đ�
 *
 * @param[in] bmw           BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] pos           �����ʒu
 * @param[in] pitch         �����̃s�b�`
 * @param[in] volume        �����̃{�����[��
 * @param[in] chorus_vol    �����̃R�[���X�ݒ�i�{�����[�����j
 * @param[in] chorus_speed  �����̃R�[���X�ݒ�i�Đ����x���j
 * @param[in] play_dir      �Đ�����
 *
 * @retval  �����v���C���[INDEX
 */
//============================================================================================
u32 BTLV_MCSS_PlayVoice( BTLV_MCSS_WORK *bmw, int position, int pitch, int volume, int chorus_vol, int chorus_speed, BOOL play_dir )
{
  int pan;
  u32 playerIndex;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  switch( position ){
  case BTLV_MCSS_POS_AA:
  case BTLV_MCSS_POS_BB:
    pan = pokevoice_pan_single_table[ position ];
    break;
  case BTLV_MCSS_POS_A:
  case BTLV_MCSS_POS_B:
  case BTLV_MCSS_POS_C:
  case BTLV_MCSS_POS_D:
    if( bmw->mcss_pos_rotate )
    {
      pan = pokevoice_pan_rotate_table[ position - BTLV_MCSS_POS_A ];
    }
    else if( bmw->mcss_pos_3vs3 )
    {
      pan = pokevoice_pan_triple_table[ position - BTLV_MCSS_POS_A ];
    }
    else
    {
      pan = pokevoice_pan_double_table[ position - BTLV_MCSS_POS_A ];
    }
    break;
  case BTLV_MCSS_POS_E:
  case BTLV_MCSS_POS_F:
    GF_ASSERT( bmw->mcss_pos_3vs3 == 1 || bmw->mcss_pos_rotate == 1 );
    if( bmw->mcss_pos_rotate )
    {
      pan = pokevoice_pan_rotate_table[ position - BTLV_MCSS_POS_A ];
    }
    else
    {
      pan = pokevoice_pan_triple_table[ position - BTLV_MCSS_POS_A ];
    }
    break;
  default:
    //��`����Ă��Ȃ��|�W�V�������w�肳��Ă��܂�
    GF_ASSERT( 0 );
    break;
  }


  // �y���b�v�{�C�X����mainModule����擾
  {
    const BTL_MAIN_MODULE* mainModule = BTLV_EFFECT_GetMainModule();
    PMV_REF  refBody;
    PMV_REF* pRef = &refBody;
    BOOL  chorus = FALSE;

    //�G�t�F�N�g�G�f�B�^�Ŋm�F����ꍇ�AmainModule��NULL�Ȃ̂ŁA�������
#ifdef PM_DEBUG
    if( mainModule ){
#endif
    if( !BTL_MAIN_SetPmvRef(mainModule, position, &refBody) ){
      pRef = NULL;
    }
#ifdef PM_DEBUG
    }
    else
    {
      pRef = NULL;
    }
#endif

    if( ( chorus_vol ) || ( chorus_speed ) )
    {
      chorus = TRUE;
    }

    //�s���`���ǂ����`�F�b�N���ăs�b�`��ω�
    if( pitch == BTLEFF_NAKIGOE_PINCH_PITCH )
    { 
      if( bmw->btlv_mcss[ index ].param.appear_hp_color == GAUGETOOL_HP_DOTTO_YELLOW )
      { 
        pitch = BTLV_MCSS_PINCH_PITCH;
      }
      else
      { 
        pitch = 0;
      }
    }

    playerIndex = PMVOICE_Play( bmw->btlv_mcss[ index ].param.mons_no, bmw->btlv_mcss[ index ].param.form_no,
                                pan, chorus, chorus_vol, chorus_speed, play_dir, (u32)pRef );
    PMVOICE_SetVolume( playerIndex, volume );
    PMVOICE_SetSpeed( playerIndex, pitch );
  }

  return playerIndex;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ���̃|�P���������[�e�[�V����
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] side  �����ʒu��
 * @param[in] dir   ���[�e�[�V��������
 */
//============================================================================================
void  BTLV_MCSS_SetRotation( BTLV_MCSS_WORK* bmw, int side, int dir )
{
  BTLV_MCSS_ROTATION_WORK* bmrw = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bmw->heapID ), sizeof( BTLV_MCSS_ROTATION_WORK ) );

  bmrw->bmw     = bmw;
  bmrw->seq_no  = 0;
  bmrw->side    = side;
  bmrw->dir     = dir;
  bmw->mcss_tcb_rotation_execute = 1;

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_Rotation, bmrw, 0 ), TCB_BTLV_MCSS_Rotation_CB, GROUP_MCSS );
}

//============================================================================================
/**
 * @brief �ߊl�{�[���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �擾�������|�P�����̗����ʒu
 */
//============================================================================================
int   BTLV_MCSS_GetCaptureBall( BTLV_MCSS_WORK *bmw, int position )
{
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return bmw->btlv_mcss[ index ].capture_ball;
}

//============================================================================================
/**
 * @brief �Z�������擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �擾�������|�P�����̗����ʒu
 */
//============================================================================================
u32 BTLV_MCSS_GetCells( BTLV_MCSS_WORK *bmw, int position )
{ 
  //���Ȃ������ʒu���w�肵�Ď擾���邱�Ƃ�����̂ŁA�A�T�[�g�ł͂Ȃ�
  //���ʂ�0��Ԃ��̂𐳏퓮��ɂ���
  int index = BTLV_MCSS_GetIndex( bmw, position );
  //GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return 0; }
  //GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return 0; }

  return MCSS_GetCells( bmw->btlv_mcss[ index ].mcss );
}

//============================================================================================
/**
 * @brief POKEMON_PARAM����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]   pp        POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[out]  maw       MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]   position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position )
{
  int dir = ( ( position & 1 ) ) ? MCSS_DIR_FRONT : MCSS_DIR_BACK;

  MCSS_TOOL_MakeMAWPP( pp, maw, dir );
}

//============================================================================================
/**
 * @brief �A�j���[�V�������Z�b�g
 *
 * @param[in]   position  �|�P�����̗����ʒu
 *
 * @retval  TRUE:�Z�b�g����   FALSE:�Z�b�g���s
 */
//============================================================================================
BOOL  BTLV_MCSS_SetAnime( BTLV_MCSS_WORK* bmw, int position, int anm_no )
{
  BOOL  ret = FALSE;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return FALSE; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return FALSE; }

  if( anm_no < MCSS_GetAnimeNum( bmw->btlv_mcss[ index ].mcss ) )
  { 
    MCSS_SetAnimeIndex( bmw->btlv_mcss[ index ].mcss, anm_no );
    bmw->btlv_mcss[ index ].anm_execute_flag = TRUE;
    ret = TRUE;
  }
  return ret;
}

//============================================================================================
/**
 * @brief �A�j���[�V�����I���`�F�b�N���Z�b�g
 *
 * @param[in]   position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_SetAnimeEndCheck( BTLV_MCSS_WORK* bmw, int position )
{ 
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  MCSS_SetAnimCtrlCallBack( bmw->btlv_mcss[ index ].mcss, index, BTLV_MCSS_AnmEndCheck, 1 );
}

//============================================================================================
/**
 * @brief �A�j���[�V�������N�������`�F�b�N
 *
 * @param[in]   position  �|�P�����̗����ʒu
 *
 * @retval  TRUE:�N���� FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_MCSS_CheckAnimeExecute( BTLV_MCSS_WORK* bmw, int position )
{ 
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return FALSE; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return FALSE; }

  return bmw->btlv_mcss[ index ].anm_execute_flag;
}

//============================================================================================
/**
 *  @brief  �����ʒu�ɂ��Ȃ�MCSS�̏ꍇ�́A�o�j�b�V�����ď����ʒu�ɖ߂�
 *
 * @param[in]   position  �����ʒu
 */
//============================================================================================
void  BTLV_MCSS_CheckPositionSetInitPos( BTLV_MCSS_WORK* bmw, int position )
{ 
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  { 
    VecFx32 def_pos, pos, ofs_pos;

    BTLV_MCSS_GetDefaultPos( bmw, &def_pos, position );
    MCSS_GetPosition( bmw->btlv_mcss[ index ].mcss, &pos );
    MCSS_GetOfsPosition( bmw->btlv_mcss[ index ].mcss, &ofs_pos );
    bmw->btlv_mcss[ index ].set_init_pos = 0;
    if( ( def_pos.x != pos.x ) ||
        ( def_pos.y != pos.y ) ||
        ( def_pos.z != pos.z ) ||
        ( ofs_pos.x ) ||
        ( ofs_pos.y ) ||
        ( ofs_pos.z ) )
    { 
      bmw->btlv_mcss[ index ].set_init_pos = 1;
      ofs_pos.x = 0;
      ofs_pos.y = 0;
      ofs_pos.z = 0;
      MCSS_SetPosition( bmw->btlv_mcss[ index ].mcss, &def_pos );
      MCSS_SetOfsPosition( bmw->btlv_mcss[ index ].mcss, &ofs_pos );
      MCSS_SetVanishFlag( bmw->btlv_mcss[ index ].mcss );
    }
  }
}

//============================================================================================
/**
 * @brief tr_type����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]   pp        POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[out]  maw       MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]   position  �g���[�i�[�̗����ʒu
 */
//============================================================================================
static  void  BTLV_MCSS_MakeMAWTrainer( int tr_type, MCSS_ADD_WORK* maw, int position )
{
  int dir = ( ( position & 1 ) ) ? MCSS_DIR_FRONT : MCSS_DIR_BACK;

  MCSS_TOOL_MakeMAWTrainer( tr_type, maw, dir );
}

//============================================================================================
/**
 * @brief �|�P�����f�t�H���g�X�P�[���Z�b�g
 */
//============================================================================================
static  void  BTLV_MCSS_SetDefaultScale( BTLV_MCSS_WORK *bmw, int position )
{
  VecFx32 scale;
  fx32    def_scale;
  int index = BTLV_MCSS_GetIndex( bmw, position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  def_scale = BTLV_MCSS_GetDefaultScale( bmw, position, bmw->btlv_mcss[ index ].mcss_proj_mode );
  if( position & 1 )
  { 
    def_scale = FX_Mul( def_scale, bmw->scale_ofs_e );
  }
  else
  { 
    def_scale = FX_Mul( def_scale, bmw->scale_ofs_m );
  }

  VEC_Set( &scale, def_scale, def_scale, FX32_ONE );

  MCSS_SetScale( bmw->btlv_mcss[ index ].mcss, &scale );

  def_scale = BTLV_MCSS_GetDefaultScale( bmw, position, BTLV_MCSS_PROJ_PERSPECTIVE );

  //VEC_Set( &scale, def_scale, def_scale / 2, FX32_ONE );
  VEC_Set( &scale, def_scale, def_scale, FX32_ONE );

  MCSS_SetShadowScale( bmw->btlv_mcss[ index ].mcss, &scale );
}

//============================================================================================
/**
 * @brief �|�P��������n�^�X�N����������
 */
//============================================================================================
static  void  BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func, BTLV_EFFECT_TCB_CALLBACK_FUNC* cb_func, int reverse_flag )
{
  BTLV_MCSS_TCB_WORK  *bmtw = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bmw->heapID ), sizeof( BTLV_MCSS_TCB_WORK ) );

  bmtw->bmw               = bmw;
  bmtw->position          = position;
  bmtw->emw.move_type     = type;
  bmtw->emw.vec_time      = frame;
  bmtw->emw.vec_time_tmp  = frame;
  bmtw->emw.wait          = 0;
  bmtw->emw.wait_tmp      = wait;
  bmtw->emw.count         = count * 2;
  bmtw->emw.start_value.x = start->x;
  bmtw->emw.start_value.y = start->y;
  bmtw->emw.start_value.z = start->z;
  bmtw->emw.end_value.x   = end->x;
  bmtw->emw.end_value.y   = end->y;
  bmtw->emw.end_value.z   = end->z;
  bmtw->now_value.x       = start->x;
  bmtw->now_value.y       = start->y;
  bmtw->now_value.z       = start->z;

  switch( type ){
  case EFFTOOL_CALCTYPE_DIRECT:               //���ڃ|�W�V�����Ɉړ�
    break;
  case EFFTOOL_CALCTYPE_INTERPOLATION:        //�ړ���܂ł��Ԃ��Ȃ���ړ�
  case EFFTOOL_CALCTYPE_INTERPOLATION_DIRECT: //�ړ���܂ł��Ԃ��Ȃ���ړ�
    BTLV_EFFTOOL_CalcMoveVector( &bmtw->emw.start_value, end, &bmtw->emw.vector, FX32_CONST( frame ) );
    break;
  case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG:       //�w�肵����Ԃ������ړ�
    bmtw->emw.count         *= 2;
    //fall through
  case EFFTOOL_CALCTYPE_ROUNDTRIP:            //�w�肵����Ԃ������ړ�
    bmtw->emw.vector.x = FX_Div( end->x, FX32_CONST( frame ) );
    bmtw->emw.vector.y = FX_Div( end->y, FX32_CONST( frame ) );
    bmtw->emw.vector.z = FX_Div( end->z, FX32_CONST( frame ) );
    if( ( position & 1 ) && ( reverse_flag ) )
    {
      bmtw->emw.vector.x *= -1;
      bmtw->emw.vector.z *= -1;
    }
    break;
  }
  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bmw->tcb_sys, func, bmtw, 0 ), cb_func, GROUP_MCSS );
}

//============================================================================================
/**
 * @brief �|�P�����ړ��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  VecFx32 now_pos;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  MCSS_GetPosition( bmw->btlv_mcss[ index ].mcss, &now_pos );
  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &now_pos );
  MCSS_SetPosition( bmw->btlv_mcss[ index ].mcss, &now_pos );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_Move_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�����g�k�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  VecFx32 now_scale;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  MCSS_GetOfsScale( bmw->btlv_mcss[ index ].mcss, &now_scale );
  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &now_scale );
  MCSS_SetOfsScale( bmw->btlv_mcss[ index ].mcss, &now_scale );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_Scale_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_scale_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�����g�k�^�X�N�i�f�t�H���g�l�𑀍�j
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_DefaultScale( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  VecFx32 now_scale;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  MCSS_GetScale( bmw->btlv_mcss[ index ].mcss, &now_scale );
  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &now_scale );
  MCSS_SetScale( bmw->btlv_mcss[ index ].mcss, &now_scale );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_DefaultScale_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_default_scale_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}
//============================================================================================
/**
 * @brief �|�P������]�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  VecFx32 now_rotate;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  MCSS_GetRotate( bmw->btlv_mcss[ index ].mcss, &now_rotate );
  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &now_rotate );
  MCSS_SetRotate( bmw->btlv_mcss[ index ].mcss, &now_rotate );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_Rotate_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_rotate_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�����܂΂����^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;

  //�����ʒu�Ƀ|�P���������݂��Ȃ�������t���[����
  if( !BTLV_MCSS_CheckExist( bmw, bmtw->position ) )
  {
    BTLV_EFFECT_FreeTCB( tcb );
    return;
  }

  if( bmtw->emw.wait == 0 )
  {
    bmtw->emw.wait = bmtw->emw.wait_tmp;
    BTLV_MCSS_SetMepachiFlag( bmtw->bmw, bmtw->position, BTLV_MCSS_MEPACHI_FLIP );
    if( --bmtw->emw.count == 0 )
    {
      BTLV_MCSS_SetMepachiFlag( bmtw->bmw, bmtw->position, BTLV_MCSS_MEPACHI_OFF );
      BTLV_EFFECT_FreeTCB( tcb );
    }
  }
  else
  {
    bmtw->emw.wait--;
  }
}

static  void  TCB_BTLV_MCSS_Blink_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_blink_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�������l�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Alpha( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &bmtw->now_value );
  MCSS_SetAlpha( bmw->btlv_mcss[ index ].mcss, bmtw->now_value.x >> FX32_SHIFT );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_Alpha_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw->bmw->mcss_tcb_alpha_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�����~�^���^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_MoveCircle( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_MOVE_CIRCLE_PARAM*  bmmcp = ( BTLV_MCSS_MOVE_CIRCLE_PARAM * )work;
  BTLV_MCSS_WORK *bmw = bmmcp->bmw;
  VecFx32 ofs = { 0, 0, 0 };
  fx32  sin, cos;
  int index = BTLV_MCSS_GetIndex( bmw, bmmcp->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  if( bmmcp->rotate_after_wait_count ==0 )
  {
    if( bmmcp->rotate_wait_count == bmmcp->rotate_wait )
    {
      bmmcp->rotate_wait_count = 0;
      if( bmmcp->axis & 1 )
      {
        bmmcp->angle -= bmmcp->speed;
      }
      else
      {
        bmmcp->angle += bmmcp->speed;
      }
      if( bmmcp->angle & 0xffff0000 )
      {
        bmmcp->angle &= 0x0000ffff;
        bmmcp->count--;
        bmmcp->rotate_after_wait_count = bmmcp->rotate_after_wait;
      }
      if( bmmcp->count )
      {
        switch( bmmcp->shift ){
        case BTLEFF_SHIFT_H_P:    //�V�t�g�g�{
          sin = FX_Mul( FX_SinIdx( ( bmmcp->angle + 0x4000 ) & 0xffff ), bmmcp->radius_h ) * -1;
          cos = FX_Mul( FX_CosIdx( ( bmmcp->angle + 0x4000 ) & 0xffff ), bmmcp->radius_v ) * -1;
          sin += bmmcp->radius_h;
          break;
        case BTLEFF_SHIFT_H_M:    //�V�t�g�g�|
          sin = FX_Mul( FX_SinIdx( ( bmmcp->angle + 0x4000 ) & 0xffff ), bmmcp->radius_h ) - bmmcp->radius_h;
          cos = FX_Mul( FX_CosIdx( ( bmmcp->angle + 0x4000 ) & 0xffff ), bmmcp->radius_v );
          break;
        case BTLEFF_SHIFT_V_P:    //�V�t�g�u�{
          sin = FX_Mul( FX_SinIdx( bmmcp->angle ), bmmcp->radius_h ) * -1;
          cos = FX_Mul( FX_CosIdx( bmmcp->angle ), bmmcp->radius_v ) * -1;
          cos += bmmcp->radius_v;
          break;
        case BTLEFF_SHIFT_V_M:    //�V�t�g�u�|
          sin = FX_Mul( FX_SinIdx( bmmcp->angle ), bmmcp->radius_h );
          cos = FX_Mul( FX_CosIdx( bmmcp->angle ), bmmcp->radius_v ) - bmmcp->radius_v;
          break;
        }
        switch( ( bmmcp->axis & 7 ) >> 1 ){
        default:
        case 0:   //X��
          ofs.z = sin;
          ofs.y = cos;
          break;
        case 1:   //Y��
          ofs.x = sin;
          ofs.z = cos;
          break;
        case 2:   //Z��
          ofs.x = sin;
          ofs.y = cos;
          break;
        }
      }
      MCSS_SetOfsPosition( bmw->btlv_mcss[ index ].mcss, &ofs );
    }
    else
    {
      bmmcp->rotate_wait_count++;
    }
  }
  else
  {
    bmmcp->rotate_after_wait_count--;
  }
  if( bmmcp->count == 0 ){
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_MoveCircle_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_MOVE_CIRCLE_PARAM*  bmmcp = ( BTLV_MCSS_MOVE_CIRCLE_PARAM * )GFL_TCB_GetWork( tcb );

  bmmcp->bmw->mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmmcp->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P����SIN�ړ��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_MoveSin( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_MOVE_SIN_PARAM*  bmmsp = ( BTLV_MCSS_MOVE_SIN_PARAM * )work;
  BTLV_MCSS_WORK *bmw = bmmsp->bmw;
  VecFx32 pos;
  int   idx;
  fx32  value;
  int   index = BTLV_MCSS_GetIndex( bmw, bmmsp->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  bmmsp->angle += bmmsp->speed;
  idx = ( bmmsp->angle & 0x0ffff000 ) >> FX32_SHIFT;
  value = FX_F32_TO_FX32( FX_FX16_TO_F32( FX_SinIdx( idx ) ) );
  value = FX_Mul( value, bmmsp->radius );

  pos.x = bmmsp->pos.x;
  pos.y = bmmsp->pos.y;
  pos.z = bmmsp->pos.z;

  if( bmmsp->dir )
  {
    pos.y += value;
  }
  else
  {
    pos.x += value;
  }

  MCSS_SetPosition( bmw->btlv_mcss[ index ].mcss, &pos );

  if( --bmmsp->frame == 0 )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_MoveSin_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_MOVE_SIN_PARAM*  bmmsp = ( BTLV_MCSS_MOVE_SIN_PARAM * )GFL_TCB_GetWork( tcb );

  bmmsp->bmw->mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmmsp->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �|�P�������U�C�N�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Mosaic( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = bmtw->bmw;
  BOOL  ret;
  int index = BTLV_MCSS_GetIndex( bmw, bmtw->position );
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { BTLV_EFFECT_FreeTCB( tcb ); return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { BTLV_EFFECT_FreeTCB( tcb ); return; }

  ret = BTLV_EFFTOOL_CalcParam( &bmtw->emw, &bmtw->now_value );
  MCSS_SetMosaic( bmw->mcss_sys, bmw->btlv_mcss[ index ].mcss, bmtw->now_value.x >> FX32_SHIFT );
  if( ret == TRUE )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_MCSS_Mosaic_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_TCB_WORK  *bmtw = ( BTLV_MCSS_TCB_WORK * )GFL_TCB_GetWork( tcb );

  bmtw-> bmw->mcss_tcb_mosaic_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 * @brief �w�肵���t���[���ŌĂ΂��R�[���o�b�N�֐�
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_StopAnime( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_WORK *bmw = BTLV_EFFECT_GetMcssWork();
  BTLV_MCSS_STOP_ANIME* bmsa = ( BTLV_MCSS_STOP_ANIME* )work;
  int index = bmsa->index;
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX )
  {
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
    return;
  }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL )
  { 
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
    return;
  }

  switch( bmsa->seq_no ){
  case 0:
    { 
      static  u8  time[ 3 ] = { BTLV_MCSS_STOP_ANIME_TIME1, BTLV_MCSS_STOP_ANIME_TIME2, BTLV_MCSS_STOP_ANIME_TIME3 };
      NNS_G2dStopAnimCtrl( NNS_G2dGetMCAnimAnimCtrl( MCSS_GetAnimCtrl( bmw->btlv_mcss[ index ].mcss ) ) );
      MCSS_SetTraverseMCNodesCallBack( bmw->btlv_mcss[ index ].mcss, ANIME_STOP_FLAG | index, BTLV_MCSS_CallBackNodes );
      bmsa->seq_no++;
      bmsa->wait = time[ GFL_STD_MtRand( 3 ) ];
    }
    break;
  case 1:
    if( ( --bmsa->wait == 0 ) ||
        ( bmw->mcss_tcb_rotate_execute ) ||
        ( BTLV_EFFVM_GetExecuteEffectType( BTLV_EFFECT_GetVMHandle() ) == EXECUTE_EFF_TYPE_WAZA ) )
    {
      MCSS_SetAnimeIndex( bmw->btlv_mcss[ index ].mcss, SEQ_ANIME_STANDBY );
      NNS_G2dStartAnimCtrl( NNS_G2dGetMCAnimAnimCtrl( MCSS_GetAnimCtrl( bmw->btlv_mcss[ index ].mcss ) ) );
      MCSS_SetTraverseMCNodesCallBack( bmw->btlv_mcss[ index ].mcss, ANIME_START_FLAG | index, BTLV_MCSS_CallBackNodes );
      bmsa->seq_no++;
    }
    break;
  case 2:
    MCSS_SetAnimCtrlCallBack( bmw->btlv_mcss[ index ].mcss, index, BTLV_MCSS_CallBackFunctorFrame, 1 );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
    bmw->btlv_mcss[ index ].tcb = NULL;
    break;
  }
}

//============================================================================================
/**
 * @brief ���[�e�[�V��������
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Rotation( GFL_TCB *tcb, void *work )
{
//�~�ʂœ����o�[�W����
  BTLV_MCSS_ROTATION_WORK* bmrw = ( BTLV_MCSS_ROTATION_WORK* )work;
  int src_pos[ 2 ][ 3 ] = {
    { BTLV_MCSS_POS_A, BTLV_MCSS_POS_C, BTLV_MCSS_POS_E },
    { BTLV_MCSS_POS_B, BTLV_MCSS_POS_D, BTLV_MCSS_POS_F },
  };
  int src_angle[ 2 ][ 3 ] = {
    { 0x8000, 0xd555, 0x2aab },
    { 0x0000, 0x5555, 0xaaaa },
  };
#ifdef ROTATION_NEW_SYSTEM
  int dst_pos[ 2 ][ 2 ][ 3 ] = {
    {
      { BTLV_MCSS_POS_C, BTLV_MCSS_POS_E, BTLV_MCSS_POS_A },
      { BTLV_MCSS_POS_D, BTLV_MCSS_POS_F, BTLV_MCSS_POS_B },
    },
    {
      { BTLV_MCSS_POS_E, BTLV_MCSS_POS_A, BTLV_MCSS_POS_C },
      { BTLV_MCSS_POS_F, BTLV_MCSS_POS_B, BTLV_MCSS_POS_D },
    },
  };
#else
  int dst_pos[ 2 ][ 2 ][ 3 ] = {
    {
      { BTLV_MCSS_POS_E, BTLV_MCSS_POS_A, BTLV_MCSS_POS_C },
      { BTLV_MCSS_POS_F, BTLV_MCSS_POS_B, BTLV_MCSS_POS_D },
    },
    {
      { BTLV_MCSS_POS_C, BTLV_MCSS_POS_E, BTLV_MCSS_POS_A },
      { BTLV_MCSS_POS_D, BTLV_MCSS_POS_F, BTLV_MCSS_POS_B },
    },
  };
#endif

  switch( bmrw->seq_no ){
  case 0:
    {
      int i;
      for( i = 0 ; i < 3 ; i++ )
      {
        if( bmrw->bmw->btlv_mcss[ src_pos[ bmrw->side ][ i ] ].tcb )
        {
          break;
        }
      }
      if( i == 3 )
      {
        for( i = 0 ; i < 3 ; i++ )
        {
          bmrw->angle[ i ] = src_angle[ bmrw->side ][ i ];
        }
        BTLV_STAGE_SetAnmReq( BTLV_EFFECT_GetStageWork(), bmrw->side, 0, ( ( bmrw->dir == 0 ) ? FX32_ONE : -FX32_ONE ), 60 );
        bmrw->speed = ( ( bmrw->dir == 0 ) ? ( 0x5555 / 60 ) : ( -0x5555 / 60 ) );
        bmrw->frame = 60;
        bmrw->seq_no++;
      }
    }
    break;
  case 1:
    {
      int i;

      for( i = 0 ; i < 3 ; i++ )
      {
        VecFx32 pos;
        if( BTLV_MCSS_CheckExist( bmrw->bmw, src_pos[ bmrw->side ][ i ] ) )
        {
          pos.x = FX_Mul( FX_SinIdx( bmrw->angle[ i ] & 0xffff ), FX32_ONE * 5 );
          pos.y = poke_pos_single_table[ 0 ].y;
          pos.z = ( bmrw->side == 0 ) ? BTLV_MCSS_ROTATION_Z_M : BTLV_MCSS_ROTATION_Z_E;
          pos.z += FX_Mul( FX_CosIdx( bmrw->angle[ i ] & 0xffff ), FX32_ONE * 5 );
          BTLV_MCSS_MovePosition( bmrw->bmw, src_pos[ bmrw->side ][ i ],
                                  EFFTOOL_CALCTYPE_DIRECT, &pos, 0, 0, 0 );
        }
        bmrw->angle[ i ] += bmrw->speed;
      }
      if( --bmrw->frame == 0 )
      {
        for( i = 0 ; i < 3 ; i++ )
        {
          VecFx32 pos;
          if( BTLV_MCSS_CheckExist( bmrw->bmw, src_pos[ bmrw->side ][ i ] ) )
          {
            BTLV_MCSS_GetDefaultPos( bmrw->bmw, &pos, dst_pos[ bmrw->dir ][ bmrw->side ][ i ] );
            BTLV_MCSS_MovePosition( bmrw->bmw, src_pos[ bmrw->side ][ i ],
                                    EFFTOOL_CALCTYPE_DIRECT, &pos, 0, 0, 0 );
          }
        }
        bmrw->seq_no++;
      }
    }
    break;
  case 2:
    if( bmrw->bmw->mcss_tcb_move_execute == 0 )
    {
      int i;
      int index[ 3 ];

      for( i = 0 ; i < 3 ; i++ )
      {
        index[ i ] = BTLV_MCSS_GetIndex( bmrw->bmw, src_pos[ bmrw->side ][ i ] );
      }
      for( i = 0 ; i < 3 ; i++ )
      {
        if( index[ i ] != BTLV_MCSS_NO_INDEX )
        {
          bmrw->bmw->btlv_mcss[ index[ i ] ].position = dst_pos[ bmrw->dir ][ bmrw->side ][ i ];
        }
      }
      BTLV_EFFECT_FreeTCB( tcb );
    }
    break;
  }
}

static  void  TCB_BTLV_MCSS_Rotation_CB( GFL_TCB *tcb )
{ 
  BTLV_MCSS_ROTATION_WORK* bmrw = ( BTLV_MCSS_ROTATION_WORK* )GFL_TCB_GetWork( tcb );

  bmrw->bmw->mcss_tcb_rotation_execute = 0;
}

//============================================================================================
/**
 * @brief �w�肵���t���[���ŌĂ΂��R�[���o�b�N�֐�
 */
//============================================================================================
static  void  BTLV_MCSS_CallBackFunctorFrame( u32 data, fx32 currentFrame )
{
  BTLV_MCSS_WORK *bmw = BTLV_EFFECT_GetMcssWork();
  BTLV_MCSS_STOP_ANIME* bmsa;
  int index = data;
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return; }

  if( BTLV_EFFVM_GetExecuteEffectType( BTLV_EFFECT_GetVMHandle() ) == EXECUTE_EFF_TYPE_WAZA )
  {
    return;
  }

  if( bmw->mcss_tcb_rotation_execute )
  {
    return;
  }

  if( MCSS_GetStopCellAnms( bmw->btlv_mcss[ index ].mcss ) == MCSS_CELL_ANIM_NONSTOP )
  {
    //NONSTOP�A�j������1���Ԃ��炢���u�����Ƃ��ɃY�����ł�̂ŁARestart���Ă�ł���
    MCSS_RestartAnime( bmw->btlv_mcss[ index ].mcss );
    return;
  }

#ifdef PM_DEBUG
  if( ( GFL_STD_MtRand0( bmw->btlv_mcss[ index ].stop_anime_count-- ) != 0 ) &&
      ( bmw->anm_1_loop_flag == FALSE ) )
#else
  if( GFL_STD_MtRand0( bmw->btlv_mcss[ index ].stop_anime_count-- ) != 0 )
#endif
  { 
    return;
  }
  bmw->btlv_mcss[ index ].stop_anime_count = BTLV_MCSS_STOP_ANIME_COUNT;

  bmsa = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bmw->heapID ), sizeof( BTLV_MCSS_STOP_ANIME ) );
  bmsa->index = data;

  if( MCSS_GetMCellAnmNum( bmw->btlv_mcss[ index ].mcss ) > 1 )
  {
    MCSS_SetAnimeIndex( bmw->btlv_mcss[ index ].mcss, SEQ_ANIME_STOP );
  }

  GF_ASSERT( bmw->btlv_mcss[ index ].tcb == NULL );
  bmw->btlv_mcss[ index ].tcb = GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_StopAnime, bmsa, 0 );
}

//============================================================================================
/**
 * @brief �m�[�h���񂵂ČĂ΂��R�[���o�b�N�֐�
 */
//============================================================================================
static  BOOL  BTLV_MCSS_CallBackNodes( u32 data, const NNSG2dMultiCellHierarchyData* pNodeData,
                                       NNSG2dCellAnimation* pCellAnim, u16 nodeIdx )
{
  BTLV_MCSS_WORK *bmw = BTLV_EFFECT_GetMcssWork();
  int index = data & 0x0000ffff;
  GF_ASSERT( index != BTLV_MCSS_NO_INDEX );
  if( index == BTLV_MCSS_NO_INDEX ) { return FALSE; }
  GF_ASSERT( bmw->btlv_mcss[ index ].mcss != NULL );
  if( bmw->btlv_mcss[ index ].mcss == NULL ) { return FALSE; }

  if( data & ANIME_STOP_FLAG )
  {
    BOOL  flag = FALSE;
    u8    cnt = MCSS_GetStopCellAnms( bmw->btlv_mcss[ index ].mcss );
    int   i;

    for( i = 0 ; i < cnt ; i++ )
    {
      if( nodeIdx == MCSS_GetStopNode( bmw->btlv_mcss[ index ].mcss, i ) )
      {
        flag = TRUE;
      }
    }
    if( flag == FALSE )
    {
      NNS_G2dStopAnimCtrl( NNS_G2dGetCellAnimAnimCtrl( pCellAnim ) );
    }
    else
    {
      NNS_G2dStartAnimCtrl( NNS_G2dGetCellAnimAnimCtrl( pCellAnim ) );
    }
  }
  else
  {
    NNS_G2dStartAnimCtrl( NNS_G2dGetCellAnimAnimCtrl( pCellAnim ) );
  }
  return TRUE;
}

//============================================================================================
/**
 * @brief �A�j���I���`�F�b�N
 */
//============================================================================================
static  void  BTLV_MCSS_AnmEndCheck( u32 data, fx32 currentFrame )
{ 
  BTLV_MCSS_WORK *bmw = BTLV_EFFECT_GetMcssWork();

  bmw->btlv_mcss[ data ].anm_execute_flag = FALSE;
}

//============================================================================================
/**
 * @brief MCSS�̗����ʒu���Ƃ̃f�t�H���g���W���擾
 *
 * @param[in]   bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[out]  pos       ���W���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]   position  �|�P�����̗����ʒu
 */
//============================================================================================
static  void  BTLV_MCSS_GetDefaultPos( BTLV_MCSS_WORK *bmw, VecFx32 *pos, BtlvMcssPos position )
{
  const VecFx32 *pos_table;
  switch( position ){
  case BTLV_MCSS_POS_AA:
  case BTLV_MCSS_POS_BB:
    pos_table = &poke_pos_single_table[ position ];
    break;
  case BTLV_MCSS_POS_A:
  case BTLV_MCSS_POS_B:
  case BTLV_MCSS_POS_C:
  case BTLV_MCSS_POS_D:
    if( bmw->mcss_pos_rotate )
    {
      pos_table = &poke_pos_rotate_table[ position - BTLV_MCSS_POS_A ];
    }
    else if( bmw->mcss_pos_3vs3 )
    {
      pos_table = &poke_pos_triple_table[ position - BTLV_MCSS_POS_A ];
    }
    else
    {
      pos_table = &poke_pos_double_table[ position - BTLV_MCSS_POS_A ];
    }
    break;
  case BTLV_MCSS_POS_E:
  case BTLV_MCSS_POS_F:
    GF_ASSERT( bmw->mcss_pos_3vs3 == 1 || bmw->mcss_pos_rotate == 1 );
    if( bmw->mcss_pos_rotate )
    {
      pos_table = &poke_pos_rotate_table[ position - BTLV_MCSS_POS_A ];
    }
    else
    {
      pos_table = &poke_pos_triple_table[ position - BTLV_MCSS_POS_A ];
    }
    break;
  case BTLV_MCSS_POS_TR_AA:
  case BTLV_MCSS_POS_TR_BB:
  case BTLV_MCSS_POS_TR_A:
  case BTLV_MCSS_POS_TR_B:
  case BTLV_MCSS_POS_TR_C:
  case BTLV_MCSS_POS_TR_D:
    pos_table = &trainer_pos_table[ position - BTLV_MCSS_POS_TR_AA ];
    break;
  default:
    //��`����Ă��Ȃ��|�W�V�������w�肳��Ă��܂�
    GF_ASSERT( 0 );
    break;
  }
  pos->x = pos_table->x;
  pos->y = pos_table->y;
  pos->z = pos_table->z;
}

//============================================================================================
/**
 * @brief MCSS�̗����ʒu���Ƃ̃f�t�H���g�X�P�[�����擾
 *
 * @param[in]   bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]   position  MCSS�̗����ʒu
 * @param[in]   proj      �ˉe���[�h
 */
//============================================================================================
static  fx32  BTLV_MCSS_GetDefaultScale( BTLV_MCSS_WORK* bmw, BtlvMcssPos position, BTLV_MCSS_PROJECTION proj )
{
  fx32 scale;

  if( proj == BTLV_MCSS_PROJ_PERSPECTIVE )
  {
    switch( position ){
    case BTLV_MCSS_POS_AA:
    case BTLV_MCSS_POS_BB:
      scale = poke_scale_single_table[ position ];
      break;
    case BTLV_MCSS_POS_A:
    case BTLV_MCSS_POS_B:
    case BTLV_MCSS_POS_C:
    case BTLV_MCSS_POS_D:
      if( bmw->mcss_pos_rotate )
      {
        scale = poke_scale_rotate_table[ position - BTLV_MCSS_POS_A ];
      }
      else if( bmw->mcss_pos_3vs3 )
      {
        scale = poke_scale_triple_table[ position - BTLV_MCSS_POS_A ];
      }
      else
      {
        scale = poke_scale_double_table[ position - BTLV_MCSS_POS_A ];
      }
      break;
    case BTLV_MCSS_POS_E:
    case BTLV_MCSS_POS_F:
      GF_ASSERT( bmw->mcss_pos_3vs3 == 1 || bmw->mcss_pos_rotate == 1 );
      if( bmw->mcss_pos_rotate )
      {
        scale = poke_scale_rotate_table[ position - BTLV_MCSS_POS_A ];
      }
      else
      {
        scale = poke_scale_triple_table[ position - BTLV_MCSS_POS_A ];
      }
      break;
    case BTLV_MCSS_POS_TR_AA:
    case BTLV_MCSS_POS_TR_BB:
    case BTLV_MCSS_POS_TR_A:
    case BTLV_MCSS_POS_TR_B:
    case BTLV_MCSS_POS_TR_C:
    case BTLV_MCSS_POS_TR_D:
      scale = trainer_scale_table[ position - BTLV_MCSS_POS_TR_AA ];
      break;
    default:
      //��`����Ă��Ȃ��|�W�V�������w�肳��Ă��܂�
      GF_ASSERT( 0 );
      break;
    }
  }
  else
  {
    if( position < BTLV_MCSS_POS_TR_AA )
    {
      if( position & 1 )
      {
        scale = BTLV_MCSS_ORTHO_SCALE_ENEMY;
      }
      else
      {
        scale = BTLV_MCSS_ORTHO_SCALE_MINE;
      }
    }
    else
    {
      scale = BTLV_MCSS_ORTHO_SCALE_ENEMY;
    }
  }
  return scale;
}

//============================================================================================
/**
 * @brief �����ʒu�̃C���f�b�N�X���擾
 *
 * @param[in] bmw       BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 */
//============================================================================================
static  int   BTLV_MCSS_GetIndex( BTLV_MCSS_WORK* bmw, BtlvMcssPos position )
{
  int i;

  for( i = 0 ; i < BTLV_MCSS_POS_TOTAL ; i++ )
  {
    if( bmw->btlv_mcss[ i ].mcss )
    {
      if( bmw->btlv_mcss[ i ].position == position )
      {
        return i;
      }
    }
  }
  return BTLV_MCSS_NO_INDEX;
}

#ifdef PM_DEBUG
//============================================================================================
/**
 * @brief BTLV_MCSS�ǉ��i�f�o�b�O�p�j
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] madw    MCSS_ADD_DEBUG_WORK�\���̂ւ̃|�C���^
 * @param[in] position  �|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_MCSS_AddDebug( BTLV_MCSS_WORK *bmw, const MCSS_ADD_DEBUG_WORK *madw, int position )
{
  VecFx32 pos;
  int index;

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );

  if( BTLV_MCSS_CheckExist( bmw, position ) )
  {
    BTLV_MCSS_Del( bmw, position );
  }

  for( index = 0 ; index < BTLV_MCSS_POS_TOTAL ; index++ )
  {
    if( bmw->btlv_mcss[ index ].mcss == NULL )
    {
      break;
    }
  }

  bmw->btlv_mcss[ index ].mcss_proj_mode = BTLV_MCSS_PROJ_ORTHO;

  GF_ASSERT( index < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT_MSG( BTLV_MCSS_GetIndex( bmw, position ) == BTLV_MCSS_NO_INDEX, "pos=%d", position );

  //�����ʒu��ۑ�
  bmw->btlv_mcss[ index ].position = position;

  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->btlv_mcss[ index ].mcss = MCSS_AddDebug( bmw->mcss_sys, pos.x, pos.y, pos.z, madw );

  BTLV_MCSS_SetDefaultScale( bmw, position );

  MCSS_SetAnimCtrlCallBack( bmw->btlv_mcss[ index ].mcss, index, BTLV_MCSS_CallBackFunctorFrame, 1 );
}

//============================================================================================
/**
 * @brief mcss_pos_3vs3�ɒl���Z�b�g�i�f�o�b�O�p�j
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] flag  �Z�b�g����l
 */
//============================================================================================
void  BTLV_MCSS_SetMcss3vs3( BTLV_MCSS_WORK *bmw, int flag )
{
  bmw->mcss_pos_3vs3 = flag;
}

//============================================================================================
/**
 * @brief mcss_pos_rotate�ɒl���Z�b�g�i�f�o�b�O�p�j
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] flag  �Z�b�g����l
 */
//============================================================================================
void  BTLV_MCSS_SetMcssRotate( BTLV_MCSS_WORK *bmw, int flag )
{
  bmw->mcss_pos_rotate = flag;
}

//============================================================================================
/**
 * @brief anm_1_loop_flag�ɒl���Z�b�g�i�f�o�b�O�p�j
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] flag  �Z�b�g����l
 */
//============================================================================================
void  BTLV_MCSS_SetAnm1LoopFlag( BTLV_MCSS_WORK *bmw, int flag )
{
  bmw->anm_1_loop_flag = flag;
}

//============================================================================================
/**
 * @brief ���ݎ��s���̃^�X�N�������I���i�f�o�b�O�p�j
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_ClearExecute( BTLV_MCSS_WORK *bmw )
{

}

#endif
