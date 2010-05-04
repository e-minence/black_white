
//============================================================================================
/**
 * @file  btlv_gauge.c
 * @brief �퓬�Q�[�W
 * @author  soga
 * @date  2009.07.15
 */
//============================================================================================

#include <gflib.h>

#include "poke_tool/monsno_def.h"
#include "print/printsys.h"
#include "font/font.naix"
#include "poke_tool/gauge_tool.h"
#include "app/app_menu_common.h"
#include "print/str_tool.h"
#include "net/net_dev.h"

#include "btlv_effect.h"
#include "btlv_gauge.h"

#include "pm_define.h"
#include "sound/pm_sndsys.h"


#include "arc_def.h"
#include "battle/battgra_wb.naix"


//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

#define BTLV_GAUGE_MAX ( 6 )                                   //��ʂɏo��Q�[�W��MAX�l
#define BTLV_GAUGE_CLWK_MAX ( BTLV_MCSS_POS_MAX )              //�Q�[�WCLWK��MAX�l
#define BTLV_GAUGE_CLUNIT_CLWK_MAX ( BTLV_MCSS_POS_MAX * 5 )   //CLUNIT�𐶐�����Ƃ���CLWK��MAX�l


///��Ɨp���[�N�̏����l�Ƃ��Ďg�p
#define BTLV_GAUGE_HP_WORK_INIT_VALUE (-2147483648)

//���l�̌���
#define BTLV_GAUGE_NUM_MAX ( 3 )

//�_���[�W�Q�[�W����������܂ł̃E�G�C�g
#define BTLV_GAUGE_DAMAGE_WAIT  ( 18 )

//�Q�[�W�p�[�c
enum
{
  GP_HPBAR_GREEN  = 0x00 * 0x20,      //HP�o�[��
  GP_HPBAR_YELLOW = 0x09 * 0x20,      //HP�o�[��
  GP_HPBAR_RED    = 0x12 * 0x20,      //HP�o�[��

  GP_BALL         = 0x1b * 0x20,      //�ߊl�{�[��

  GP_MALE_U       = 0x1c * 0x20,      //����
  GP_MALE_D       = 0x1d * 0x20,      //����
  GP_FEMALE_U     = 0x1e * 0x20,      //����
  GP_FEMALE_D     = 0x1f * 0x20,      //����

  GP_EXPBAR       = 0x20 * 0x20,      //EXP�o�[

  GP_NUM_0        = 0x29 * 0x20,      //�����O
  GP_NUM_1        = 0x2a * 0x20,      //�����P
  GP_SPACE        = 0x33 * 0x20,      //��
  GP_LV           = 0x34 * 0x20,      //LV���x��

  GP_HPBAR_DAMAGE = 0x35 * 0x20,      //HP�o�[�_���[�W
};

enum
{
  BTLV_GAUGE_POS_AA_X = 200,
  BTLV_GAUGE_POS_AA_Y = 120,

  BTLV_GAUGE_POS_BB_X = 56,
  BTLV_GAUGE_POS_BB_Y = 40,

  BTLV_GAUGE_POS_A_X = 192 + 8,
  BTLV_GAUGE_POS_A_Y = 104 - 4,

  BTLV_GAUGE_POS_B_X = 64 - 8,
  BTLV_GAUGE_POS_B_Y = 28,

  BTLV_GAUGE_POS_C_X = 196 + 8,
  BTLV_GAUGE_POS_C_Y = 132 - 4,

  BTLV_GAUGE_POS_D_X = 60 - 8,
  BTLV_GAUGE_POS_D_Y = 52,

  BTLV_GAUGE_POS_E_X = 200 + 8,
  BTLV_GAUGE_POS_F_X = 56 - 8,

  BTLV_GAUGE_POS_A_3vs3_Y =  92 + 6,
  BTLV_GAUGE_POS_C_3vs3_Y = 114 + 3,
  BTLV_GAUGE_POS_E_3vs3_Y = 136,

  BTLV_GAUGE_POS_B_3vs3_Y = 20,
  BTLV_GAUGE_POS_D_3vs3_Y = 42 - 3,
  BTLV_GAUGE_POS_F_3vs3_Y = 64 - 6,

  BTLV_GAUGE_HPNUM_X  = 16,
  BTLV_GAUGE_HPNUM_Y  = 13,
  BTLV_GAUGE_MINE_HP_X   = 8,
  BTLV_GAUGE_MINE_HP_Y   = 7,
  BTLV_GAUGE_ENEMY_HP_X  = 0,
  BTLV_GAUGE_ENEMY_HP_Y  = 7,
  BTLV_GAUGE_EXP_X  =  8,
  BTLV_GAUGE_EXP_Y  = 21,
  BTLV_STATUS_MINE_X     = -30,
  BTLV_STATUS_MINE_Y     = 8,
  BTLV_STATUS_ENEMY_X    = -38,
  BTLV_STATUS_ENEMY_Y    = 8,

  BTLV_GAUGE_BMP_SIZE_X = 8,
  BTLV_GAUGE_BMP_SIZE_Y = 2,
  BTLV_GAUGE_BMP_POS_X = 8,
  BTLV_GAUGE_BMP_POS_Y = 5,

  BTLV_GAUGE_NAME1U_CHARSTART_M = 0x01,
  BTLV_GAUGE_NAME1D_CHARSTART_M = 0x09,
  BTLV_GAUGE_NAME1U_CHARSTART_E = 0x02,
  BTLV_GAUGE_NAME1D_CHARSTART_E = 0x0a,

  BTLV_GAUGE_NAME2U_CHARSTART = 0x20,
  BTLV_GAUGE_NAME2D_CHARSTART = 0x28,

  BTLV_GAUGE_SEXU_CHARSTART_M = 0x21,
  BTLV_GAUGE_SEXD_CHARSTART_M = 0x29,
  BTLV_GAUGE_SEXU_CHARSTART_E = 0x22,
  BTLV_GAUGE_SEXD_CHARSTART_E = 0x2a,

  BTLV_GAUGE_LVU_CHARSTART_M = 0x23,
  BTLV_GAUGE_LVD_CHARSTART_M = 0x2b,
  BTLV_GAUGE_LVU_CHARSTART_E = 0x24,
  BTLV_GAUGE_LVD_CHARSTART_E = 0x2c,


  BTLV_GAUGE_HP_CHARSTART = 0x00,
  BTLV_GAUGE_HP_DAMAGE_CHARSTART = 0x00,
  BTLV_GAUGE_NOWHP_CHARSTART = 0x00,
  BTLV_GAUGE_MAXHP_CHARSTART = 0x04,
  BTLV_GAUGE_EXP_CHARSTART = 0x01,
  BTLV_GAUGE_BALL_CHARSTART = 0x0a,

  BTLV_GAUGE_EXP_SE_TIME = 16,

  MOVE_VALUE = 16,
  MOVE_COUNT = 2,
  MOVE_SPEED = MOVE_VALUE / MOVE_COUNT,

  YURE_OFFSET = 2048,
  YURE_SPEED  = 0x400,

  BTLV_GAUGE_DAMAGE_EFFECT_COUNT = 3,       //�_���[�W�G�t�F�N�g�̓_��MAX��
};

typedef enum
{
  BTLV_GAUGE_REQ_HP = 0,
  BTLV_GAUGE_REQ_EXP,
}BTLV_GAUGE_REQ;

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct _BTLV_GAUGE_CLWK
{
  GFL_CLWK*     base_clwk;
  GFL_CLWK*     hpnum_clwk;
  GFL_CLWK*     hp_clwk;
  GFL_CLWK*     hp_damage_clwk;
  GFL_CLWK*     exp_clwk;
  GFL_CLWK*     status_clwk;

  //�퓬�Q�[�W�{��
  u32           base_charID;
  u32           base_cellID;
  //HP���l
  u32           hpnum_charID;
  u32           hpnum_cellID;
  //HP�Q�[�W
  u32           hp_charID;
  u32           hp_cellID;
  //HP�_���[�W�Q�[�W
  u32           hp_damage_charID;
  u32           hp_damage_cellID;
  //EXP�Q�[�W
  u32           exp_charID;
  u32           exp_cellID;

  int           gauge_type;
  int           gauge_dir;

  s32           hp;
  s32           hpmax;
  s32           hp_work;

  s32           exp;
  s32           exp_max;
  s32           next_exp_max;
  s32           exp_add;
  s32           exp_work;
  s32           damage;

  u8            level;
  u8            sex;
  u8            status;
  u8            getball;

  u32           hp_calc_req       :1;
  u32           exp_calc_req      :1;
  u32           level_up_req      :1;
  u32           gauge_enable      :1;
  u32           seq_no            :4;
  u32           se_wait           :8;
  u32           move_cnt          :4;
  u32           yure_req          :1;
  u32           damage_wait       :5;
  u32           damage_wait_flag  :1;
  u32                             :5;

  u32           add_dec;
  u32           damage_dot;   //�_���[�W�Q�[�W�G�t�F�N�g�p�̏����h�b�g�l
};

struct _BTLV_GAUGE_WORK
{
  GFL_CLUNIT*     clunit;
  ARCHANDLE*      handle;
  GFL_FONT*       font;
  GFL_TCB*        tcb;
  void*           arc_data;
  const u8*       parts_address;

  u32             plttID[ BTLV_GAUGE_MAX ];

  //��Ԉُ�A�C�R��
  u32             status_charID;
  u32             status_cellID;
  u32             status_plttID;

  BTLV_GAUGE_CLWK bgcl[ BTLV_GAUGE_CLWK_MAX ];

  u32             vanish_flag             :1;
  u32             bgm_fade_flag           :1;
  u32             pinch_bgm_flag          :1;
  u32             yure_angle              :16;
  u32             trainer_bgm_change_flag :1;
  u32                                     :13;

  u32             now_bgm_no;

  HEAPID          heapID;
};

typedef struct
{ 
  BTLV_GAUGE_WORK*  bgw;
  int               seq_no;
  int               pltt_bit;
  int               count;
}BTLV_GAUGE_DAMAGE_EFFECT_WORK;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

static  void  Gauge_InitCalcHP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl, int value );
static  void  Gauge_CalcHP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl );
static  void  Gauge_InitCalcEXP( BTLV_GAUGE_CLWK* bgcl, int add_exp );
static  void  Gauge_CalcEXP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl );
static  s32   GaugeProc( s32 MaxHP, s32 NowHP, s32 beHP, s32 *HP_Work, u8 GaugeMax, u16 add_dec );
static  u8    PutGaugeProc( s32 MaxHP, s32 NowHP, s32 beHP, s32 *HP_Work, u8 *gauge_chr, u8 GaugeMax );
static  u32   DottoOffsetCalc( s32 nowHP, s32 beHP, s32 MaxHP, u8 GaugeMax );
static  void  PutNameOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, const POKEMON_PARAM* pp );
static  void  PutSexOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl );
static  void  PutGaugeOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, BTLV_GAUGE_REQ req );
static  void  PutHPNumOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, s32 nowHP );
static  void  PutLVNumOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl );
static  void  PutBallOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl );
static  void  PutGaugeDamageObj( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl, int put_dot );
static  void  Gauge_LevelUp( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl );
static  void  Gauge_Yure( BTLV_GAUGE_WORK* bgw, BtlvMcssPos pos );

static  void  pinch_bgm_check( BTLV_GAUGE_WORK* bgw );

static  void  TCB_BTLV_GAUGE_Move( GFL_TCB* tcb, void* work );
static  void  TCB_BTLV_GAUGE_DamageEffect( GFL_TCB* tcb, void* work );

static  void  gauge_load_resource( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_TYPE type, BtlvMcssPos pos, PokeSick sick );
static  void  gauge_init_view( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_TYPE type, BtlvMcssPos pos, const POKEMON_PARAM* pp );
static  void  init_damage_dot( BTLV_GAUGE_CLWK* bgcl );
static  void  bgm_pause( BOOL flag );
static  u32   get_num_dotto( u32 prm_now, u32 prm_max, u8 dot_max );

//============================================================================================
/**
 *  @brief  �V�X�e��������
 *
 *  @param[in] heapID    �q�[�vID
 *
 *  @retval  bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
BTLV_GAUGE_WORK*  BTLV_GAUGE_Init( GFL_FONT* fontHandle, HEAPID heapID )
{
  BTLV_GAUGE_WORK *bgw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_GAUGE_WORK ) );

  bgw->heapID = heapID;
  bgw->handle = GFL_ARC_OpenDataHandle( ARCID_BATTGRA, bgw->heapID );

  bgw->clunit = GFL_CLACT_UNIT_Create( BTLV_GAUGE_CLUNIT_CLWK_MAX, 0, bgw->heapID );

  //���ʃp���b�g�ǂݍ���
  { 
    int i;

    for( i = 0 ; i < BTLV_GAUGE_MAX ; i++ )
    { 
      bgw->plttID[ i ] = GFL_CLGRP_PLTT_Register( bgw->handle, NARC_battgra_wb_gauge_NCLR,
                                                  CLSYS_DRAW_MAIN, BTLV_OBJ_PLTT_HP_GAUGE + 0x20 * i, bgw->heapID );
      PaletteWorkSet_VramCopy( BTLV_EFFECT_GetPfd(), FADE_MAIN_OBJ,
                               GFL_CLGRP_PLTT_GetAddr( bgw->plttID[ i ], CLSYS_DRAW_MAIN ) / 2, 0x20 * 1 );
    }
  }
  {
    ARCHANDLE*  handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), bgw->heapID );

    bgw->status_charID = GFL_CLGRP_CGR_Register( handle, APP_COMMON_GetStatusIconCharArcIdx(),
                                                 FALSE, CLSYS_DRAW_MAIN, bgw->heapID );
    bgw->status_cellID = GFL_CLGRP_CELLANIM_Register( handle,
                                                      APP_COMMON_GetStatusIconCellArcIdx( APP_COMMON_MAPPING_64K ),
                                                      APP_COMMON_GetStatusIconAnimeArcIdx( APP_COMMON_MAPPING_64K ),
                                                      bgw->heapID );
    bgw->status_plttID = GFL_CLGRP_PLTT_Register( handle, APP_COMMON_GetStatusIconPltArcIdx(),
                                                  CLSYS_DRAW_MAIN, BTLV_OBJ_PLTT_STATUS_ICON, bgw->heapID );
    PaletteWorkSet_VramCopy( BTLV_EFFECT_GetPfd(), FADE_MAIN_OBJ,
                             GFL_CLGRP_PLTT_GetAddr( bgw->status_plttID, CLSYS_DRAW_MAIN ) / 2, 0x20 * 1 );
    GFL_ARC_CloseDataHandle( handle );
  }


  //�p�[�c�f�[�^�ǂݍ���
  {
    NNSG2dCharacterData *char_data;

    bgw->arc_data = GFL_ARCHDL_UTIL_LoadOBJCharacter( bgw->handle, NARC_battgra_wb_gauge_parts_NCGR, NULL,
                                                      &char_data, bgw->heapID );
    bgw->parts_address = ( u8 * )char_data->pRawData;
  }

  bgw->font = fontHandle;

  //�����Ă���BGM��ۑ�
  bgw->now_bgm_no = PMSND_GetBGMsoundNo();

  //�_���[�W�G�t�F�N�g�Ď�TCB�Z�b�g
  { 
    BTLV_GAUGE_DAMAGE_EFFECT_WORK* bgdew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_GAUGE_DAMAGE_EFFECT_WORK ) );

    bgdew->bgw = bgw;

    bgw->tcb = GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), TCB_BTLV_GAUGE_DamageEffect, bgdew, 0 );
  }

  return bgw;
}

//============================================================================================
/**
 *  @brief  �V�X�e���I��
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_GAUGE_Exit( BTLV_GAUGE_WORK *bgw )
{
  BtlvMcssPos pos;

  for( pos = 0 ;  pos < BTLV_GAUGE_CLWK_MAX ; pos++ )
  {
    BTLV_GAUGE_Del( bgw, pos );
  }
  if( bgw->pinch_bgm_flag )
  {
    PMSND_PopBGM();
  }
  { 
    int i;

    for( i = 0 ; i < BTLV_GAUGE_MAX ; i++ )
    { 
      GFL_CLGRP_PLTT_Release( bgw->plttID[ i ] );
    }
  }
  GFL_CLGRP_CGR_Release( bgw->status_charID );
  GFL_CLGRP_CELLANIM_Release( bgw->status_cellID );
  GFL_CLGRP_PLTT_Release( bgw->status_plttID );
  GFL_CLACT_UNIT_Delete( bgw->clunit );
  GFL_HEAP_FreeMemory( bgw->arc_data );
  GFL_ARC_CloseDataHandle( bgw->handle );
  GFL_HEAP_FreeMemory( GFL_TCB_GetWork( bgw->tcb ) );
  GFL_TCB_DeleteTask( bgw->tcb );
  GFL_HEAP_FreeMemory( bgw );
}

//============================================================================================
/**
 *  @brief  �V�X�e�����C��
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_GAUGE_Main( BTLV_GAUGE_WORK *bgw )
{
  int i;

  for( i = 0 ; i < BTLV_GAUGE_CLWK_MAX ; i++ )
  {
    if( bgw->bgcl[ i ].hp_calc_req )
    {
      Gauge_CalcHP( bgw, &bgw->bgcl[ i ] );
    }
    if( bgw->bgcl[ i ].exp_calc_req )
    {
      if( bgw->bgcl[ i ].se_wait == 0 )
      { 
        PMSND_PlaySE( SEQ_SE_EXP );
      }
      Gauge_CalcEXP( bgw, &bgw->bgcl[ i ] );
    }
    if( bgw->bgcl[ i ].se_wait < BTLV_GAUGE_EXP_SE_TIME )
    { 
      bgw->bgcl[ i ].se_wait++;
    }
    if( ( bgw->bgcl[ i ].exp_calc_req == 0 ) && ( bgw->bgcl[ i ].se_wait == BTLV_GAUGE_EXP_SE_TIME ) )
    {
      PMSND_StopSE();
      bgw->bgcl[ i ].se_wait++;
    }
    if( bgw->bgcl[ i ].level_up_req )
    {
      Gauge_LevelUp( bgw, &bgw->bgcl[ i ] );
    }
    if( bgw->bgcl[ i ].yure_req )
    { 
      Gauge_Yure( bgw, i );
    }
    //@todo�@�����Ƃ�������������܂ł̎b�菈��
    if( ( bgw->bgcl[ i ].hp_damage_clwk ) && ( bgw->bgcl[ i ].damage_wait_flag ) )
    { 
      if( bgw->bgcl[ i ].damage_wait == 0 )
      { 
        GFL_CLACT_WK_SetDrawEnable( bgw->bgcl[ i ].hp_damage_clwk, FALSE );
        bgw->bgcl[ i ].damage_wait_flag = 0;
      }
      else
      { 
        bgw->bgcl[ i ].damage_wait--;
      }
    }
  }
  //�s���`BGM�Đ��`�F�b�N
  pinch_bgm_check( bgw );
}

//============================================================================================
/**
 *  @brief  �Q�[�W����
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] type �Q�[�W�^�C�v
 *  @param[in] pos  �����ʒu
 */
//============================================================================================
void  BTLV_GAUGE_Add( BTLV_GAUGE_WORK *bgw, const BTL_MAIN_MODULE* wk, const BTL_POKEPARAM* bpp, BTLV_GAUGE_TYPE type, BtlvMcssPos pos )
{
  GF_ASSERT( bgw->bgcl[ pos ].base_clwk == NULL );

  { 
    PokeSick sick;
    if( ( BPP_SICKCONT_IsMoudokuCont( BPP_GetSickCont( bpp, WAZASICK_DOKU ) ) == TRUE ) &&
        ( BPP_GetPokeSick( bpp ) == POKESICK_DOKU ) )
    { 
      sick = POKESICK_MAX;
    }
    else
    { 
      sick = BPP_GetPokeSick( bpp );
    }
    gauge_load_resource( bgw, type, pos, sick );
  }

  {
    bgw->bgcl[ pos ].hp       = BPP_GetValue( bpp, BPP_HP );
    BTL_Printf("�����łg�o��%d\n", bgw->bgcl[ pos ].hp);
    bgw->bgcl[ pos ].hpmax    = BPP_GetValue( bpp, BPP_MAX_HP );
    bgw->bgcl[ pos ].hp_work  = BTLV_GAUGE_HP_WORK_INIT_VALUE;
    bgw->bgcl[ pos ].damage   = 0;

    bgw->bgcl[ pos ].level    = BPP_GetValue( bpp, BPP_LEVEL );

    if( bgw->bgcl[ pos ].level < 100 )
    {
      int min_exp = POKETOOL_GetMinExp( BPP_GetMonsNo( bpp ),
                                        BPP_GetValue( bpp, BPP_FORM ),
                                        BPP_GetValue( bpp, BPP_LEVEL ) );

      bgw->bgcl[ pos ].exp      = BPP_GetValue( bpp, BPP_EXP ) - min_exp;
      bgw->bgcl[ pos ].exp_max  = POKETOOL_GetMinExp( BPP_GetMonsNo( bpp ),
                                                      BPP_GetValue( bpp, BPP_FORM ),
                                                      BPP_GetValue( bpp, BPP_LEVEL ) + 1 ) -
                                  min_exp;
    }
    else
    {
      bgw->bgcl[ pos ].exp      = 0;
      bgw->bgcl[ pos ].exp_max  = 0;
    }

    bgw->bgcl[ pos ].exp_work = BTLV_GAUGE_HP_WORK_INIT_VALUE;
    bgw->bgcl[ pos ].exp_add  = 0;

    bgw->bgcl[ pos ].status   = 0;
    bgw->bgcl[ pos ].getball  = BTL_MAIN_IsZukanRegistered( wk, bpp );

    {
      u16 mons_no = BPP_GetMonsNo( bpp );

      if( ( mons_no != MONSNO_NIDORAN_M ) && ( mons_no != MONSNO_NIDORAN_F ) )
      {
        bgw->bgcl[ pos ].sex  = BPP_GetValue( bpp, BPP_SEX );
      }
      else
      {
        bgw->bgcl[ pos ].sex  = PTL_SEX_UNKNOWN;
      }
    }
  }
  init_damage_dot( &bgw->bgcl[ pos ] );
  gauge_init_view( bgw, type, pos, BPP_GetViewSrcData( bpp ) );
}

//============================================================================================
/**
 *  @brief  �Q�[�W�����iPOKEMON_PARAM�o�[�W�����j
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] type �Q�[�W�^�C�v
 *  @param[in] pos  �����ʒu
 */
//============================================================================================
void  BTLV_GAUGE_AddPP( BTLV_GAUGE_WORK *bgw, const ZUKAN_SAVEDATA* zs, const POKEMON_PARAM* pp, BTLV_GAUGE_TYPE type, BtlvMcssPos pos )
{
  GF_ASSERT( bgw->bgcl[ pos ].base_clwk == NULL );

  gauge_load_resource( bgw, type, pos, PP_GetSick( pp ) );

  {
    bgw->bgcl[ pos ].hp       = PP_Get( pp, ID_PARA_hp,     NULL );
    bgw->bgcl[ pos ].hpmax    = PP_Get( pp, ID_PARA_hpmax,  NULL );
    bgw->bgcl[ pos ].hp_work  = BTLV_GAUGE_HP_WORK_INIT_VALUE;
    bgw->bgcl[ pos ].damage   = 0;

    bgw->bgcl[ pos ].level    = PP_Get( pp, ID_PARA_level,  NULL );

    if( bgw->bgcl[ pos ].level < 100 )
    {
      int mons_no = PP_Get( pp, ID_PARA_monsno,   NULL );
      int form_no = PP_Get( pp, ID_PARA_form_no,  NULL );
      int min_exp = POKETOOL_GetMinExp( mons_no, form_no, bgw->bgcl[ pos ].level );

      bgw->bgcl[ pos ].exp      = PP_Get( pp, ID_PARA_exp, NULL ) - min_exp;
      bgw->bgcl[ pos ].exp_max  = POKETOOL_GetMinExp( mons_no, form_no, bgw->bgcl[ pos ].level + 1 ) - min_exp;
    }
    else
    {
      bgw->bgcl[ pos ].exp      = 0;
      bgw->bgcl[ pos ].exp_max  = 0;
    }

    bgw->bgcl[ pos ].exp_work = BTLV_GAUGE_HP_WORK_INIT_VALUE;
    bgw->bgcl[ pos ].exp_add  = 0;

    bgw->bgcl[ pos ].status   = 0;

    {
      u16 mons_no = PP_Get( pp, ID_PARA_monsno, NULL );

      bgw->bgcl[ pos ].getball  = ZUKANSAVE_GetPokeGetFlag( zs, mons_no );

      if( ( mons_no != MONSNO_NIDORAN_M ) && ( mons_no != MONSNO_NIDORAN_F ) )
      {
        bgw->bgcl[ pos ].sex  = PP_GetSex( pp );
      }
      else
      {
        bgw->bgcl[ pos ].sex  = PTL_SEX_UNKNOWN;
      }
    }
  }
  init_damage_dot( &bgw->bgcl[ pos ] );
  gauge_init_view( bgw, type, pos, pp );
}

static  void  gauge_load_resource( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_TYPE type, BtlvMcssPos pos, PokeSick sick )
{ 
  u32 arcdatid_char;
  u32 arcdatid_cell;
  u32 arcdatid_anm;

  bgw->bgcl[ pos ].gauge_type = type;
  bgw->bgcl[ pos ].gauge_dir = pos & 1;

  switch( type ){
  default:
  case BTLV_GAUGE_TYPE_1vs1:
  case BTLV_GAUGE_TYPE_2vs2:
    arcdatid_char = ( pos & 1 ) ? NARC_battgra_wb_gauge_1vs1_e_NCGR : NARC_battgra_wb_gauge_1vs1_m_NCGR;
    arcdatid_cell = ( pos & 1 ) ? NARC_battgra_wb_gauge_1vs1_e_NCER : NARC_battgra_wb_gauge_1vs1_m_NCER;
    arcdatid_anm  = ( pos & 1 ) ? NARC_battgra_wb_gauge_1vs1_e_NANR : NARC_battgra_wb_gauge_1vs1_m_NANR;
    break;
  case BTLV_GAUGE_TYPE_3vs3:
  case BTLV_GAUGE_TYPE_ROTATE:
    arcdatid_char = ( pos & 1 ) ? NARC_battgra_wb_gauge_2vs2_e_NCGR : NARC_battgra_wb_gauge_2vs2_m_NCGR;
    arcdatid_cell = ( pos & 1 ) ? NARC_battgra_wb_gauge_2vs2_e_NCER : NARC_battgra_wb_gauge_2vs2_m_NCER;
    arcdatid_anm  = ( pos & 1 ) ? NARC_battgra_wb_gauge_2vs2_e_NANR : NARC_battgra_wb_gauge_2vs2_m_NANR;
    break;
  case BTLV_GAUGE_TYPE_SAFARI:
    break;
  }

  if( type == BTLV_GAUGE_TYPE_ROTATE )
  {
    int       pltt_id = ( pos < BTLV_MCSS_POS_A ) ? pos : pos - BTLV_MCSS_POS_A;
    ARCDATID  pltt = ( pos < BTLV_MCSS_POS_C ) ? NARC_battgra_wb_gauge_NCLR : NARC_battgra_wb_gauge_dark_NCLR;

    GFL_CLGRP_PLTT_Release( bgw->plttID[ pltt_id ] );

    bgw->plttID[ pltt_id ] = GFL_CLGRP_PLTT_RegisterEx( bgw->handle, pltt, CLSYS_DRAW_MAIN,
                                                        BTLV_OBJ_PLTT_HP_GAUGE + 0x20 * pltt_id, 0, 1, bgw->heapID );
    PaletteWorkSet_VramCopy( BTLV_EFFECT_GetPfd(), FADE_MAIN_OBJ,
                             GFL_CLGRP_PLTT_GetAddr( bgw->plttID[ pltt_id ], CLSYS_DRAW_MAIN ) / 2, 0x20 * 1 );
  }

  //���\�[�X�ǂݍ���
  bgw->bgcl[ pos ].base_charID = GFL_CLGRP_CGR_Register( bgw->handle, arcdatid_char, FALSE, CLSYS_DRAW_MAIN, bgw->heapID );
  bgw->bgcl[ pos ].base_cellID = GFL_CLGRP_CELLANIM_Register( bgw->handle, arcdatid_cell, arcdatid_anm, bgw->heapID );

  bgw->bgcl[ pos ].hpnum_charID = GFL_CLGRP_CGR_Register( bgw->handle, NARC_battgra_wb_gauge_num_NCGR, FALSE,
                                                       CLSYS_DRAW_MAIN, bgw->heapID );
  bgw->bgcl[ pos ].hpnum_cellID = GFL_CLGRP_CELLANIM_Register( bgw->handle, NARC_battgra_wb_gauge_num_NCER,
                                                            NARC_battgra_wb_gauge_num_NANR, bgw->heapID );

  bgw->bgcl[ pos ].hp_charID = GFL_CLGRP_CGR_Register( bgw->handle, NARC_battgra_wb_gauge_hp_NCGR, FALSE,
                                                       CLSYS_DRAW_MAIN, bgw->heapID );
  bgw->bgcl[ pos ].hp_cellID = GFL_CLGRP_CELLANIM_Register( bgw->handle, NARC_battgra_wb_gauge_hp_NCER,
                                                            NARC_battgra_wb_gauge_hp_NANR, bgw->heapID );

  bgw->bgcl[ pos ].hp_damage_charID = GFL_CLGRP_CGR_Register( bgw->handle, NARC_battgra_wb_gauge_hp_damage_NCGR, FALSE,
                                                              CLSYS_DRAW_MAIN, bgw->heapID );
  bgw->bgcl[ pos ].hp_damage_cellID = GFL_CLGRP_CELLANIM_Register( bgw->handle, NARC_battgra_wb_gauge_hp_damage_NCER,
                                                                   NARC_battgra_wb_gauge_hp_NANR, bgw->heapID );

  if( ( pos & 1 ) == 0 )
  {
    bgw->bgcl[ pos ].exp_charID = GFL_CLGRP_CGR_Register( bgw->handle, NARC_battgra_wb_gauge_exp_NCGR, FALSE,
                                                          CLSYS_DRAW_MAIN, bgw->heapID );
    bgw->bgcl[ pos ].exp_cellID = GFL_CLGRP_CELLANIM_Register( bgw->handle, NARC_battgra_wb_gauge_exp_NCER,
                                                               NARC_battgra_wb_gauge_exp_NANR, bgw->heapID );
  }

  //CLWK
  {
    GFL_CLWK_DATA gauge = {
      0, 0,     //x, y
      0, 0, 1,  //�A�j���ԍ��A�D�揇�ʁABG�v���C�I���e�B
    };
    int pltt_id = ( pos < BTLV_MCSS_POS_A ) ? pos : pos - BTLV_MCSS_POS_A;
    gauge.softpri = 2;
    bgw->bgcl[ pos ].base_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                      bgw->bgcl[ pos ].base_charID, bgw->plttID[ pltt_id ],
                                                      bgw->bgcl[ pos ].base_cellID,
                                                      &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    gauge.softpri = 1;
    bgw->bgcl[ pos ].hpnum_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                       bgw->bgcl[ pos ].hpnum_charID, bgw->plttID[ pltt_id ],
                                                       bgw->bgcl[ pos ].hpnum_cellID,
                                                       &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    bgw->bgcl[ pos ].hp_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                    bgw->bgcl[ pos ].hp_charID, bgw->plttID[ pltt_id ],
                                                    bgw->bgcl[ pos ].hp_cellID,
                                                    &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    bgw->bgcl[ pos ].status_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                        bgw->status_charID, bgw->status_plttID,
                                                        bgw->status_cellID,
                                                        &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( bgw->bgcl[ pos ].status_clwk, TRUE );
    BTLV_GAUGE_SetStatus( bgw, sick, pos );

    gauge.softpri = 0;
    bgw->bgcl[ pos ].hp_damage_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                           bgw->bgcl[ pos ].hp_damage_charID, bgw->plttID[ pltt_id ],
                                                           bgw->bgcl[ pos ].hp_damage_cellID,
                                                           &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    GFL_CLACT_WK_SetDrawEnable( bgw->bgcl[ pos ].hp_damage_clwk, FALSE );

    if( ( ( pos & 1 ) == 0 ) &&
          ( bgw->bgcl[ pos ].gauge_type != BTLV_GAUGE_TYPE_3vs3 ) &&
          ( bgw->bgcl[ pos ].gauge_type != BTLV_GAUGE_TYPE_ROTATE ) )
    {
      bgw->bgcl[ pos ].exp_clwk = GFL_CLACT_WK_Create( bgw->clunit,
                                                       bgw->bgcl[ pos ].exp_charID, bgw->plttID[ pltt_id ],
                                                       bgw->bgcl[ pos ].exp_cellID,
                                                       &gauge, CLSYS_DEFREND_MAIN, bgw->heapID );
    }
    BTLV_GAUGE_SetPos( bgw, pos, NULL );
  }
}

static  void  gauge_init_view( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_TYPE type, BtlvMcssPos pos, const POKEMON_PARAM* pp )
{ 
  GaugeProc( bgw->bgcl[ pos ].hpmax, bgw->bgcl[ pos ].hp, 0, &bgw->bgcl[ pos ].hp_work, BTLV_GAUGE_HP_CHARMAX, 1 );
  PutGaugeOBJ( bgw, &bgw->bgcl[ pos ], BTLV_GAUGE_REQ_HP );
  if( bgw->bgcl[ pos ].exp_clwk ){
    GaugeProc( bgw->bgcl[ pos ].exp_max, bgw->bgcl[ pos ].exp, 0, &bgw->bgcl[ pos ].exp_work, BTLV_GAUGE_EXP_CHARMAX, 1 );
    PutGaugeOBJ( bgw, &bgw->bgcl[ pos ], BTLV_GAUGE_REQ_EXP );
  }
  PutNameOBJ( bgw, &bgw->bgcl[ pos ], pp );
  PutSexOBJ( bgw, &bgw->bgcl[ pos ] );
  PutHPNumOBJ( bgw, &bgw->bgcl[ pos ], bgw->bgcl[ pos ].hp );
  PutLVNumOBJ( bgw, &bgw->bgcl[ pos ] );
  if( bgw->bgcl[ pos ].gauge_dir )
  { 
    PutBallOBJ( bgw, &bgw->bgcl[ pos ] );
  }
  bgw->bgcl[ pos ].gauge_enable = 1;

  bgw->bgcl[ pos ].move_cnt = MOVE_COUNT;

  GFL_TCB_AddTask( BTLV_EFFECT_GetTCBSYS(), TCB_BTLV_GAUGE_Move, &bgw->bgcl[ pos ], 0 );
}

//============================================================================================
/**
 *  @brief  damage_dot������
 *
 *  @param[in] bgcl  �Q�[�W���[�N�ւ̃|�C���^
 */
//============================================================================================
static  void  init_damage_dot( BTLV_GAUGE_CLWK* bgcl )
{ 
  u8  dot_max = BTLV_GAUGE_HP_CHARMAX * BTLV_GAUGE_DOTTO;

  if( bgcl->hpmax < dot_max )
  { 
    bgcl->damage_dot = get_num_dotto( ( bgcl->hp << 8 ), bgcl->hpmax, dot_max );
    bgcl->damage_dot >>= 8;
  }
  else
  { 
    bgcl->damage_dot = get_num_dotto( bgcl->hp, bgcl->hpmax, dot_max );
  }
}

//============================================================================================
/**
 *  @brief  �Q�[�W�j��
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos  �����ʒu
 */
//============================================================================================
void  BTLV_GAUGE_Del( BTLV_GAUGE_WORK *bgw, BtlvMcssPos pos )
{
  if( bgw->bgcl[ pos ].base_clwk )
  {
    GFL_CLGRP_CGR_Release( bgw->bgcl[ pos ].base_charID );
    GFL_CLGRP_CGR_Release( bgw->bgcl[ pos ].hpnum_charID );
    GFL_CLGRP_CGR_Release( bgw->bgcl[ pos ].hp_charID );
    GFL_CLGRP_CGR_Release( bgw->bgcl[ pos ].hp_damage_charID );

    GFL_CLGRP_CELLANIM_Release( bgw->bgcl[ pos ].base_cellID );
    GFL_CLGRP_CELLANIM_Release( bgw->bgcl[ pos ].hpnum_cellID );
    GFL_CLGRP_CELLANIM_Release( bgw->bgcl[ pos ].hp_cellID );
    GFL_CLGRP_CELLANIM_Release( bgw->bgcl[ pos ].hp_damage_cellID );

    GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].base_clwk );
    GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].hpnum_clwk );
    GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].hp_clwk );
    GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].hp_damage_clwk );
    GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].status_clwk );
    bgw->bgcl[ pos ].base_clwk      = NULL;
    bgw->bgcl[ pos ].hpnum_clwk     = NULL;
    bgw->bgcl[ pos ].hp_clwk        = NULL;
    bgw->bgcl[ pos ].hp_damage_clwk = NULL;
    bgw->bgcl[ pos ].status_clwk    = NULL;

    if( ( pos & 1 ) == 0 )
    { 
      GFL_CLGRP_CGR_Release( bgw->bgcl[ pos ].exp_charID );
      GFL_CLGRP_CELLANIM_Release( bgw->bgcl[ pos ].exp_cellID );
    }

    if( bgw->bgcl[ pos ].exp_clwk )
    {
      GFL_CLACT_WK_Remove( bgw->bgcl[ pos ].exp_clwk );
      bgw->bgcl[ pos ].exp_clwk = NULL;
    }
  }
}

//============================================================================================
/**
 *  @brief  �Q�[�W�ʒu�Z�b�g
 *
 *  @param[in] bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos   �����ʒu
 *  @param[in] ofs   �ʒu�I�t�Z�b�g
 */
//============================================================================================
void  BTLV_GAUGE_SetPos( BTLV_GAUGE_WORK* bgw, BtlvMcssPos pos, GFL_CLACTPOS* ofs )
{
  GFL_CLACTPOS  cl_pos;
  GFL_CLACTPOS  hp_pos_ofs[]={
    { BTLV_GAUGE_MINE_HP_X, BTLV_GAUGE_MINE_HP_Y },
    { BTLV_GAUGE_ENEMY_HP_X, BTLV_GAUGE_ENEMY_HP_Y },
  };
  GFL_CLACTPOS  status_pos_ofs[]={
    { BTLV_STATUS_MINE_X, BTLV_STATUS_MINE_Y },
    { BTLV_STATUS_ENEMY_X, BTLV_STATUS_ENEMY_Y },
  };
  GFL_CLACTPOS  gauge_pos[]={
    { BTLV_GAUGE_POS_AA_X + MOVE_VALUE, BTLV_GAUGE_POS_AA_Y },
    { BTLV_GAUGE_POS_BB_X - MOVE_VALUE, BTLV_GAUGE_POS_BB_Y },
    { BTLV_GAUGE_POS_A_X  + MOVE_VALUE, BTLV_GAUGE_POS_A_Y },
    { BTLV_GAUGE_POS_B_X  - MOVE_VALUE, BTLV_GAUGE_POS_B_Y },
    { BTLV_GAUGE_POS_C_X  + MOVE_VALUE, BTLV_GAUGE_POS_C_Y },
    { BTLV_GAUGE_POS_D_X  - MOVE_VALUE, BTLV_GAUGE_POS_D_Y },
  };
  GFL_CLACTPOS  gauge_pos_3vs3[]={
    { BTLV_GAUGE_POS_AA_X + MOVE_VALUE, BTLV_GAUGE_POS_AA_Y },
    { BTLV_GAUGE_POS_BB_X - MOVE_VALUE, BTLV_GAUGE_POS_BB_Y },
    { BTLV_GAUGE_POS_A_X  + MOVE_VALUE, BTLV_GAUGE_POS_A_3vs3_Y },
    { BTLV_GAUGE_POS_B_X  - MOVE_VALUE, BTLV_GAUGE_POS_B_3vs3_Y },
    { BTLV_GAUGE_POS_C_X  + MOVE_VALUE, BTLV_GAUGE_POS_C_3vs3_Y },
    { BTLV_GAUGE_POS_D_X  - MOVE_VALUE, BTLV_GAUGE_POS_D_3vs3_Y },
    { BTLV_GAUGE_POS_E_X  + MOVE_VALUE, BTLV_GAUGE_POS_E_3vs3_Y },
    { BTLV_GAUGE_POS_F_X  - MOVE_VALUE, BTLV_GAUGE_POS_F_3vs3_Y },
  };
  GFL_CLACTPOS  gauge_pos_rotate[]={
    { BTLV_GAUGE_POS_AA_X + MOVE_VALUE, BTLV_GAUGE_POS_AA_Y },
    { BTLV_GAUGE_POS_BB_X - MOVE_VALUE, BTLV_GAUGE_POS_BB_Y },
    { BTLV_GAUGE_POS_C_X  + MOVE_VALUE, BTLV_GAUGE_POS_C_3vs3_Y },
    { BTLV_GAUGE_POS_D_X  - MOVE_VALUE, BTLV_GAUGE_POS_D_3vs3_Y },
    { BTLV_GAUGE_POS_A_X  + MOVE_VALUE, BTLV_GAUGE_POS_A_3vs3_Y },
    { BTLV_GAUGE_POS_B_X  - MOVE_VALUE, BTLV_GAUGE_POS_B_3vs3_Y },
    { BTLV_GAUGE_POS_E_X  + MOVE_VALUE, BTLV_GAUGE_POS_E_3vs3_Y },
    { BTLV_GAUGE_POS_F_X  - MOVE_VALUE, BTLV_GAUGE_POS_F_3vs3_Y },
  };
  int pos_x, pos_y;

  if( bgw->bgcl[ pos ].gauge_type == BTLV_GAUGE_TYPE_3vs3 )
  {
    pos_x = gauge_pos_3vs3[ pos ].x;
    pos_y = gauge_pos_3vs3[ pos ].y;
  }
  else if( bgw->bgcl[ pos ].gauge_type == BTLV_GAUGE_TYPE_ROTATE )
  {
    pos_x = gauge_pos_rotate[ pos ].x;
    pos_y = gauge_pos_rotate[ pos ].y;
  }
  else
  { 
    pos_x = gauge_pos[ pos ].x;
    pos_y = gauge_pos[ pos ].y;
  }

  if( ofs )
  { 
    pos_x += ofs->x;
    pos_y += ofs->y;
  }

  cl_pos.x = pos_x;
  cl_pos.y = pos_y;
  GFL_CLACT_WK_SetPos( bgw->bgcl[ pos ].base_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  if( ( ( pos & 1 ) == 0 ) && ( bgw->bgcl[ pos ].gauge_type == BTLV_GAUGE_TYPE_1vs1 ) )
  {
    cl_pos.x = pos_x + BTLV_GAUGE_HPNUM_X;
    cl_pos.y = pos_y + BTLV_GAUGE_HPNUM_Y;
    GFL_CLACT_WK_SetPos( bgw->bgcl[ pos ].hpnum_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  }

  cl_pos.x = pos_x + hp_pos_ofs[ pos & 1 ].x;
  cl_pos.y = pos_y + hp_pos_ofs[ pos & 1 ].y;
  GFL_CLACT_WK_SetPos( bgw->bgcl[ pos ].hp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  cl_pos.x = pos_x + status_pos_ofs[ pos & 1 ].x;
  cl_pos.y = pos_y + status_pos_ofs[ pos & 1 ].y;
  GFL_CLACT_WK_SetPos( bgw->bgcl[ pos ].status_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  if( bgw->bgcl[ pos ].exp_clwk )
  {
    cl_pos.x = pos_x + BTLV_GAUGE_EXP_X;
    cl_pos.y = pos_y + BTLV_GAUGE_EXP_Y;
    GFL_CLACT_WK_SetPos( bgw->bgcl[ pos ].exp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  }
}

//============================================================================================
/**
 *  @brief  HP�Q�[�W�v�Z
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos    �����ʒu
 *  @param[in] value  �ŏI�I�ɓ��B����l
 */
//============================================================================================
void  BTLV_GAUGE_CalcHP( BTLV_GAUGE_WORK *bgw, BtlvMcssPos pos, int value )
{
  Gauge_InitCalcHP( bgw, &bgw->bgcl[ pos ], value );
  bgw->bgcl[ pos ].add_dec = 1;
}

//============================================================================================
/**
 *  @brief  HP�Q�[�W�v�Z
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos    �����ʒu
 *  @param[in] value  �ŏI�I�ɓ��B����l
 */
//============================================================================================
void  BTLV_GAUGE_CalcHPAtOnce( BTLV_GAUGE_WORK *bgw, BtlvMcssPos pos, int value )
{
  Gauge_InitCalcHP( bgw, &bgw->bgcl[ pos ], value );
  bgw->bgcl[ pos ].add_dec = 0;
}

//============================================================================================
/**
 *  @brief  EXP�Q�[�W�v�Z
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos    �����ʒu
 *  @param[in] value  ���Z����o���l
 */
//============================================================================================
void  BTLV_GAUGE_CalcEXP( BTLV_GAUGE_WORK *bgw, BtlvMcssPos pos, int value )
{
  bgw->bgcl[ pos ].se_wait = 0;
  Gauge_InitCalcEXP( &bgw->bgcl[ pos ], value );
}

//============================================================================================
/**
 *  @brief  EXP�Q�[�W�v�Z�i���x���A�b�v���j
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] bpp    �v�Z����|�P�����p�����[�^
 *  @param[in] pos    �����ʒu
 */
//============================================================================================
void  BTLV_GAUGE_CalcEXPLevelUp( BTLV_GAUGE_WORK *bgw, const BTL_POKEPARAM* bpp, BtlvMcssPos pos )
{
  int value = bgw->bgcl[ pos ].exp_max - bgw->bgcl[ pos ].exp;

  bgw->bgcl[ pos ].hp       = BPP_GetValue( bpp, BPP_HP );
  bgw->bgcl[ pos ].hpmax    = BPP_GetValue( bpp, BPP_MAX_HP );
  bgw->bgcl[ pos ].hp_work  = BTLV_GAUGE_HP_WORK_INIT_VALUE;
  bgw->bgcl[ pos ].damage   = 0;

  bgw->bgcl[ pos ].level    = BPP_GetValue( bpp, BPP_LEVEL );

  bgw->bgcl[ pos ].next_exp_max  = POKETOOL_GetMinExp( BPP_GetMonsNo( bpp ),
                                                       BPP_GetValue( bpp, BPP_FORM ),
                                                       BPP_GetValue( bpp, BPP_LEVEL ) + 1 ) -
                                   POKETOOL_GetMinExp( BPP_GetMonsNo( bpp ),
                                                       BPP_GetValue( bpp, BPP_FORM ),
                                                       BPP_GetValue( bpp, BPP_LEVEL ) );

  bgw->bgcl[ pos ].seq_no = 0;
  bgw->bgcl[ pos ].level_up_req = 1;
  bgw->bgcl[ pos ].se_wait = 0;

  Gauge_InitCalcEXP( &bgw->bgcl[ pos ], value );
}

//============================================================================================
/**
 *  @brief  �Q�[�W�v�Z�����`�F�b�N
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *
 *  @retval FALSE:�v�Z�I��  TRUE:�v�Z��
 */
//============================================================================================
BOOL  BTLV_GAUGE_CheckExecute( BTLV_GAUGE_WORK *bgw )
{
  int i;

  for( i = 0 ; i < BTLV_GAUGE_CLWK_MAX ; i++ )
  {
    if( ( bgw->bgcl[ i ].hp_calc_req ) || ( bgw->bgcl[ i ].exp_calc_req ) || ( bgw->bgcl[ i ].level_up_req ) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//============================================================================================
/**
 *  @brief  �Q�[�W�\��/��\��
 *
 *  @param[in] bgw    BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] on_off TRUE:�\�� FALSE:��\��
 */
//============================================================================================
void  BTLV_GAUGE_SetDrawEnable( BTLV_GAUGE_WORK* bgw, BOOL on_off )
{
  GFL_CLACT_UNIT_SetDrawEnable( bgw->clunit, on_off );
}

//============================================================================================
/**
 *  @brief  �Q�[�W�����݂��邩�ǂ����`�F�b�N
 *
 *  @param[in] bgw  BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 *  @param[in] pos  �`�F�b�N����|�W�V����
 *
 *  @retval TRUE:���݂��� FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  BTLV_GAUGE_CheckExist( BTLV_GAUGE_WORK* bgw, BtlvMcssPos pos )
{
  return ( bgw->bgcl[ pos ].base_clwk != NULL );
}

//--------------------------------------------------------------
/**
 * @brief   HP�o�[�̌v�Z���J�n����O�ɍ�Ɨp���[�N���̏��������s��
 *
 * @param[in] bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param[in] bgcl  �Q�[�W���[�N�ւ̃|�C���^
 * @param[in] value �ŏI�I�ɓ��B����l
 *
 * Gauge_CalcHP�����s����O�ɕK���Ăяo���Ēu���K�v������܂��B
 */
//--------------------------------------------------------------
static  void  Gauge_InitCalcHP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl, int value )
{
  bgcl->hp_work = BTLV_GAUGE_HP_WORK_INIT_VALUE;

  bgcl->damage = bgcl->hp - value;
  if( bgcl->hp < 0 ){
    bgcl->hp = 0;
  }
  if( bgcl->hp > bgcl->hpmax ){
    bgcl->hp = bgcl->hpmax;
  }
  bgcl->hp_calc_req = 1;
  { 
    GFL_CLACTPOS  pos;

    init_damage_dot( bgcl );
    GFL_CLACT_WK_GetPos( bgcl->hp_clwk, &pos, CLSYS_DEFREND_MAIN );
    pos.x -= ( ( BTLV_GAUGE_HP_CHARMAX * BTLV_GAUGE_DOTTO ) - bgcl->damage_dot );
    GFL_CLACT_WK_SetPos( bgcl->hp_damage_clwk, &pos, CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( bgcl->hp_damage_clwk, TRUE );
    PutGaugeDamageObj( bgw, bgcl, bgcl->damage_dot );
  }
}

//--------------------------------------------------------------
/**
 * @brief   HP�o�[�̌v�Z�Ə������݂��s��
 *
 * @param   bgw   �Q�[�W���[�N�ւ̃|�C���^
 *
 * Gauge_InitCalcHP���ɌĂ�ł���K�v������܂��B
 */
//--------------------------------------------------------------
static  void  Gauge_CalcHP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl )
{
  s32 calc_hp;

  calc_hp = GaugeProc( bgcl->hpmax, bgcl->hp, bgcl->damage, &bgcl->hp_work, BTLV_GAUGE_HP_CHARMAX, bgcl->add_dec );
  PutGaugeOBJ( bgw, bgcl, BTLV_GAUGE_REQ_HP );
  if( calc_hp == -1 ){
    //�v�Z�I�����ɃQ�[�W���[�N��hp�p�����[�^���ŐV�̒l(�_���[�W�v�Z��)�ōX�V���Ă���
    bgcl->hp -= bgcl->damage;
    PutHPNumOBJ( bgw, bgcl, bgcl->hp );
    bgcl->hp_calc_req = 0;
    bgcl->hp_work = 0;
    bgcl->damage_wait = BTLV_GAUGE_DAMAGE_WAIT;
    bgcl->damage_wait_flag = 1;
  }
  else{
    PutHPNumOBJ( bgw, bgcl, calc_hp );
  }
}

//--------------------------------------------------------------
/**
 * @brief   EXP�o�[�̌v�Z���J�n����O�ɍ�Ɨp���[�N���̏��������s��
 *
 * @param   bgw       �Q�[�W���[�N�ւ̃|�C���^
 * @param   add_exp   ���Z����EXP�l
 *
 * Gauge_CalcEXP�����s����O�ɕK���Ăяo���Ēu���K�v������܂��B
 */
//--------------------------------------------------------------
static  void  Gauge_InitCalcEXP( BTLV_GAUGE_CLWK* bgcl, int add_exp )
{
  bgcl->exp_work = BTLV_GAUGE_HP_WORK_INIT_VALUE;

  if( bgcl->exp + add_exp < 0 )
  {
    add_exp -= bgcl->exp + add_exp;
  }
  if( bgcl->exp + add_exp > bgcl->exp_max)
  {
    add_exp -= ( bgcl->exp + add_exp ) - bgcl->exp_max;
  }

  //-- ���Q�[�W�v�Z���[�`���ɍ��킹�邽�߁A�����̋t�]�Ȃǂ��s�� --//
   bgcl->exp_add = -add_exp;
  if( bgcl->exp < 0 )
  {
    bgcl->exp = 0;
  }
  if( bgcl->exp > bgcl->exp_max )
  {
    bgcl->exp = bgcl->exp_max;
  }
  bgcl->exp_calc_req = 1;
}

//--------------------------------------------------------------
/**
 * @brief   EXP�o�[�̌v�Z�Ə������݂��s��
 *
 * @param   bgw   �Q�[�W���[�N�ւ̃|�C���^
 *
 * @retval  -1=�I���B�@-1�ȊO=�v�Z�r���̐���(��ʂɕ\�����鐔�l)
 *
 * Gauge_InitCalcEXP���ɌĂ�ł���K�v������܂��B
 */
//--------------------------------------------------------------
static  void  Gauge_CalcEXP( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl )
{
  s32 calc_exp;
  s32 dotto_offset;

  dotto_offset = DottoOffsetCalc( bgcl->exp, bgcl->exp_add, bgcl->exp_max, BTLV_GAUGE_EXP_CHARMAX );
  if( dotto_offset == 0 )
  {
    dotto_offset = 1;
  }
  dotto_offset = GFL_STD_Abs( bgcl->exp_add / dotto_offset );
  calc_exp = GaugeProc( bgcl->exp_max, bgcl->exp, bgcl->exp_add, &bgcl->exp_work, BTLV_GAUGE_EXP_CHARMAX, dotto_offset );
  PutGaugeOBJ( bgw, bgcl, BTLV_GAUGE_REQ_EXP );
  if(calc_exp == -1)
  {
    //�v�Z�I�����ɃQ�[�W���[�N��exp�p�����[�^���ŐV�̒l(�_���[�W�v�Z��)�ōX�V���Ă���
    bgcl->exp -= bgcl->exp_add;
    bgcl->exp_calc_req = 0;
    bgcl->exp_work = 0;
  }
}


//--------------------------------------------------------------
/**
 *  @brief   �Q�[�W�o�[�v�Z
 *
 *  @param   MaxHP    �ő�HP
 *  @param   NowHP    ����HP(�v�Z�O��HP)
 *  @param   beHP   �_���[�W(-�Ń_���[�W�A+�Ȃ��)
 *  @param   HP_Work    ��Ɨp���[�N
 *  @param   GaugeMax �Q�[�W�̃L�����N�^��
 *  @param   add_dec    �ް�ނ������Z���Ă����l GaugeDotto��GaugeMax�ȉ��̏ꍇ�͖�������
 *
 *  @retval  -1 = �I��  ����ȊO�F�v�Z�r���̐���(��ʂɏo�����l)
 */
//--------------------------------------------------------------
static s32 GaugeProc(s32 MaxHP, s32 NowHP, s32 beHP, s32 *HP_Work, u8 GaugeMax, u16 add_dec)
{
  s32 endHP;
  s32 ret;
  u8  GaugeMaxDot;
  s32 add_hp;

  GaugeMaxDot = GaugeMax * BTLV_GAUGE_DOTTO;

  //-- HP_Work��BTLV_GAUGE_HP_WORK_INIT_VALUE�̎��͏���Ȃ̂Ń��[�N�������� --//
  if( *HP_Work == BTLV_GAUGE_HP_WORK_INIT_VALUE ){
    if( MaxHP < GaugeMaxDot ){
      *HP_Work = NowHP << 8;
    }
    else{
      *HP_Work = NowHP;
    }
  }

  //-- �I������ --//
  endHP = NowHP - beHP;
  if( endHP < 0 )
  {
    endHP = 0;
  }
  else if( endHP > MaxHP )
  {
    endHP = MaxHP;
  }

  if( MaxHP < GaugeMaxDot )           //����������������
  {
    if( endHP == ( *HP_Work >> 8 ) && ( *HP_Work & 0xff ) == 0 )
    {
      return (-1);
    }
  }
  else{
    if( endHP == *HP_Work )
    {
      return (-1);
    }
  }

  //-- �I�������I�� --//
  if( MaxHP < GaugeMaxDot )
  {
    add_hp = MaxHP * 0x100 / GaugeMaxDot;
    if( beHP < 0 ){ //�ް�މ�
      *HP_Work += add_hp;
      ret = *HP_Work >> 8;
      if( ( ret >= endHP ) || ( add_dec == 0 ) ){ //�����������Ă���̂ŵ��ް�۰����
        *HP_Work = endHP << 8;
        ret = endHP;
      }
    }
    else{         //�ް�ތ���
      *HP_Work -= add_hp;
      ret = *HP_Work >> 8;
      if( ( *HP_Work & 0xff ) > 0 ) //��������������Ȃ班���J��グ�ŕ\��
      {
        ret++;
      }
      if( ( ret <= endHP ) || ( add_dec == 0 ) )    //�����������Ă���̂ŵ��ް�۰����
      {
        *HP_Work = endHP << 8;
        ret = endHP;
      }
    }
  }
  else{
    if( beHP < 0 ){   //�ް�މ�
      *HP_Work += add_dec;
      if( ( *HP_Work > endHP ) || ( add_dec == 0 ) )
      {
        *HP_Work = endHP;
      }
    }
    else{       //�ް�ތ���
      *HP_Work -= add_dec;
      if( ( *HP_Work < endHP ) || ( add_dec == 0 ) )
      {
        *HP_Work = endHP;
      }
    }
    ret = *HP_Work;
  }
  return ret;
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W�\���v�Z
 *
 * @param   MaxHP   �ő�HP
 * @param   NowHP   ����HP(�v�Z�O��HP)
 * @param   beHP    �_���[�W(-�Ń_���[�W�A+�Ȃ��)
 * @param   HP_Work   ��Ɨp���[�N
 * @param   gauge_chr �Q�[�W�\�����ʑ����(GaugeMax���̔z��v�f�����K�v)
 * @param   GaugeMax  �Q�[�W�̃L�����N�^��
 *
 * @retval  �\������h�b�g��
 *
 * gauge_chr�ɓ���l�́A0:����ۂ̹ް��  1:1�ޯĂ̹ް�� 2:2�ޯĂ̹ް�� ��� 8:8�ޯ�(����)�̹ް��
 */
//--------------------------------------------------------------
static u8 PutGaugeProc( s32 MaxHP, s32 NowHP, s32 beHP, s32 *HP_Work, u8 *gauge_chr, u8 GaugeMax )
{
  int i;
  u32 GaugeMaxDot;
  u32 put_dot, dot_ret;   //�\�������ޯĐ�
  s32 AfterHP;

  AfterHP = NowHP - beHP;
  if( AfterHP < 0 )
  {
    AfterHP = 0;
  }
  else if( AfterHP > MaxHP )
  {
    AfterHP = MaxHP;
  }

  GaugeMaxDot = BTLV_GAUGE_DOTTO * GaugeMax;
  for( i = 0 ; i < GaugeMax ; i++ )   //�󔒹ް�ނŏ�����
  {
    gauge_chr[ i ] = 0;
  }

  //���������̃h�b�g�擾�̌v�Z����ύX������GetNumDotto�̌v�Z�����ύX���邱��!!
  if( MaxHP < GaugeMaxDot )
  {
    put_dot = ( *HP_Work * GaugeMaxDot / MaxHP ) >> 8;
  }
  else{
    put_dot = *HP_Work * GaugeMaxDot / MaxHP;
  }
  dot_ret = put_dot;
  //�ޯČv�Z�ł�0�ł��A���ۂ�HP��1�ȏ゠��Ȃ�1�ޯĂ̹ް�ނ�\������
  if( put_dot == 0 && AfterHP > 0)
  {
    gauge_chr[ 0 ] = 1;
    dot_ret = 1;
  }
  else
  {
    for( i = 0 ; i < GaugeMax ; i++ )
    {
      if( put_dot >= BTLV_GAUGE_DOTTO )
      {
        gauge_chr[ i ] = 8;
        put_dot -= BTLV_GAUGE_DOTTO;
      }
      else
      {
        gauge_chr[ i ] = put_dot;
        break;  //�ޯĂ��s�����̂Ŕ�����
      }
    }
  }
  return dot_ret;
}

//--------------------------------------------------------------
/**
 * @brief   �ް�ތv�Z�O�ƌv�Z��̍ŏI�I���ޯĂ̍��������߂�
 *
 * @param   nowHP   ����HP(�v�Z�O��HP)
 * @param   beHP    �_���[�W(-�Ń_���[�W�A+�Ȃ��)
 * @param   MaxHP   �ő�HP
 * @param   GaugeMax  �Q�[�W�̃L�����N�^��
 *
 * @retval  �h�b�g����
 */
//--------------------------------------------------------------
static u32 DottoOffsetCalc( s32 nowHP, s32 beHP, s32 MaxHP, u8 GaugeMax )
{
  s8  now_dotto, end_dotto,ret_dotto;
  u8  GaugeMaxDot;
  s32 endHP;

  GaugeMaxDot = GaugeMax * BTLV_GAUGE_DOTTO;

  endHP = nowHP - beHP;
  if( endHP < 0 )
  {
    endHP = 0;
  }
  else if( endHP > MaxHP )
  {
    endHP = MaxHP;
  }
  now_dotto = nowHP * GaugeMaxDot / MaxHP;
  end_dotto = endHP * GaugeMaxDot / MaxHP;
  ret_dotto = now_dotto - end_dotto;
  return ( GFL_STD_Abs( ret_dotto ) );
}

//--------------------------------------------------------------
/**
 * @brief  �j�b�N�l�[���`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 * @param pp    POKEMON_PARAM�\����
 */
//--------------------------------------------------------------
static  void  PutNameOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, const POKEMON_PARAM* pp )
{
  STRBUF* monsname = GFL_STR_CreateBuffer( BUFLEN_POKEMON_NAME, bgw->heapID );
  PRINTSYS_LSB  color = PRINTSYS_LSB_Make( 1, 4, 0 );
  GFL_BMP_DATA* bmp = GFL_BMP_Create( BTLV_GAUGE_BMP_SIZE_X, BTLV_GAUGE_BMP_SIZE_Y, GFL_BMP_16_COLOR, bgw->heapID );
  u8 letter, shadow, back;

  GFL_BMP_Clear( bmp, 0 );

  PP_Get( pp, ID_PARA_nickname, monsname );

  GFL_FONTSYS_GetColor( &letter, &shadow, &back );
  GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( color ), PRINTSYS_LSB_GetS( color ), PRINTSYS_LSB_GetB( color ) );
  PRINTSYS_Print( bmp, BTLV_GAUGE_BMP_POS_X, BTLV_GAUGE_BMP_POS_Y, monsname, bgw->font );
  GFL_FONTSYS_SetColor( letter, shadow, back );

  {
    void *obj_vram;
    NNSG2dImageProxy image;
    u8* bmp_data = GFL_BMP_GetCharacterAdrs( bmp );

    obj_vram = G2_GetOBJCharPtr();
    GFL_CLACT_WK_GetImgProxy( bgcl->base_clwk, &image );

    if( bgcl->gauge_dir )
    { 
      MI_CpuCopy16( bmp_data,
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME1U_CHARSTART_E * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 6 );
      MI_CpuCopy16( &bmp_data[ BTLV_GAUGE_BMP_SIZE_X * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME1D_CHARSTART_E * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 6 );
      MI_CpuCopy16( &bmp_data[ 6 * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME2U_CHARSTART * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 2 );
      MI_CpuCopy16( &bmp_data[ ( BTLV_GAUGE_BMP_SIZE_X + 6 ) * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME2D_CHARSTART * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 2 );
    }
    else
    { 
      MI_CpuCopy16( bmp_data,
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME1U_CHARSTART_M * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 7 );
      MI_CpuCopy16( &bmp_data[ BTLV_GAUGE_BMP_SIZE_X * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME1D_CHARSTART_M * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 7 );
      MI_CpuCopy16( &bmp_data[ 7 * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME2U_CHARSTART * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 1 );
      MI_CpuCopy16( &bmp_data[ ( BTLV_GAUGE_BMP_SIZE_X + 7 ) * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_NAME2D_CHARSTART * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 1 );
    }
  }

  GFL_STR_DeleteBuffer( monsname );
  GFL_BMP_Delete( bmp );
}

//--------------------------------------------------------------
/**
 * @brief  ���ʕ`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static  void  PutSexOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl )
{
  void *obj_vram;
  NNSG2dImageProxy image;

  if( bgcl->sex == PTL_SEX_UNKNOWN )
  {
    return;
  }

  obj_vram = G2_GetOBJCharPtr();
  GFL_CLACT_WK_GetImgProxy( bgcl->base_clwk, &image );

  if( bgcl->gauge_dir )
  { 
    MI_CpuCopy16( &bgw->parts_address[ GP_MALE_U + bgcl->sex * 0x40 ],
                  (void*)( (u32)obj_vram + BTLV_GAUGE_SEXU_CHARSTART_E * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20 );
    MI_CpuCopy16( &bgw->parts_address[ GP_MALE_D + bgcl->sex * 0x40 ],
                  (void*)( (u32)obj_vram + BTLV_GAUGE_SEXD_CHARSTART_E * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20 );
  }
  else
  { 
    MI_CpuCopy16( &bgw->parts_address[ GP_MALE_U + bgcl->sex * 0x40 ],
                  (void*)( (u32)obj_vram + BTLV_GAUGE_SEXU_CHARSTART_M * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20 );
    MI_CpuCopy16( &bgw->parts_address[ GP_MALE_D + bgcl->sex * 0x40 ],
                  (void*)( (u32)obj_vram + BTLV_GAUGE_SEXD_CHARSTART_M * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20 );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W�o�[�`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 * @param req   GAUGE_REQ_???
 */
//--------------------------------------------------------------
static  void  PutGaugeOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, BTLV_GAUGE_REQ req )
{
  u8  i;
  u8  gauge_chr[ BTLV_GAUGE_EXP_CHARMAX ];

  u16 parts_num;
  u8  put_dot;
  u8  level;
  void *obj_vram;
  NNSG2dImageProxy image;

  obj_vram = G2_GetOBJCharPtr();

  switch(req){
  case BTLV_GAUGE_REQ_HP:
    GFL_CLACT_WK_GetImgProxy( bgcl->hp_clwk, &image );
    put_dot = PutGaugeProc( bgcl->hpmax, bgcl->hp, bgcl->damage, &bgcl->hp_work, gauge_chr, BTLV_GAUGE_HP_CHARMAX );

    //�Q�[�W�̕\���h�b�g������F�ƃp�[�c�A�h���X���擾
    {
      int dotto_color;

      dotto_color = GAUGETOOL_GetGaugeDottoColor( put_dot, BTLV_GAUGE_HP_DOTTOMAX );
      switch(dotto_color){
      case GAUGETOOL_HP_DOTTO_GREEN:
        parts_num = GP_HPBAR_GREEN;
        break;
      case GAUGETOOL_HP_DOTTO_YELLOW:
        parts_num = GP_HPBAR_YELLOW;
        break;
      case GAUGETOOL_HP_DOTTO_RED:
      default:
        parts_num = GP_HPBAR_RED;
        break;
      }
    }
    for( i = 0 ; i < BTLV_GAUGE_HP_CHARMAX ; i++ ){
      MI_CpuCopy16( &bgw->parts_address[ parts_num + ( gauge_chr[ i ] << 5 ) ],
                    (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_HP_CHARSTART ) * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 );
    }
    PutGaugeDamageObj( bgw, bgcl, put_dot );
    break;

  case BTLV_GAUGE_REQ_EXP:
    if( bgcl->exp_clwk )
    {
      GFL_CLACT_WK_GetImgProxy( bgcl->exp_clwk, &image );
      PutGaugeProc( bgcl->exp_max, bgcl->exp, bgcl->exp_add, &bgcl->exp_work, gauge_chr, BTLV_GAUGE_EXP_CHARMAX );
      level = bgcl->level;
      if( level == 100 )
      {
        for( i = 0; i < BTLV_GAUGE_EXP_CHARMAX ; i++ )
        {
          gauge_chr[ i ] = 0;
        }
      }

      for(i = 0; i < BTLV_GAUGE_EXP_CHARMAX; i++){
        MI_CpuCopy16( &bgw->parts_address[ GP_EXPBAR + ( gauge_chr[ i ] << 5 ) ],
                      (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_EXP_CHARSTART ) * 0x20 +
                      image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                      0x20 );
      }
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief   HP���l�`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 * @param nowHP HP���ݒl�i�r���o�߂�\������K�v������̂ŁA�������ł��炤�悤�ɂ��Ă܂��j
 */
//--------------------------------------------------------------
static  void  PutHPNumOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl, s32 nowHP )
{
  int i;
  u16 now_hp[ BTLV_GAUGE_NUM_MAX ];
  u16 max_hp[ BTLV_GAUGE_NUM_MAX ];
  s32 hp_max;
  void *obj_vram;
  NNSG2dImageProxy image;

  if( bgcl->hpnum_clwk == NULL )
  {
    return;
  }

  obj_vram = G2_GetOBJCharPtr();
  GFL_CLACT_WK_GetImgProxy( bgcl->hpnum_clwk, &image );

  for( i = 0 ; i < BTLV_GAUGE_NUM_MAX ; i++ )
  {
    now_hp[ i ] = GP_SPACE;
    max_hp[ i ] = GP_SPACE;
  }

  //HP���ݒl�v�Z
  if( nowHP >= 100 )
  {
    now_hp[ 0 ] = GP_NUM_0 + ( nowHP / 100 ) * 0x20;
    nowHP = nowHP % 100;
  }
  if( ( nowHP >= 10 ) || ( now_hp[ 0 ] != GP_SPACE ) )
  {
    now_hp[ 1 ] = GP_NUM_0 + ( nowHP / 10 ) * 0x20;
    nowHP = nowHP % 10;
  }
  now_hp[ 2 ] = GP_NUM_0 + nowHP * 0x20;

  //HPMAX�v�Z
  hp_max = bgcl->hpmax;
  i = 0;

  if( hp_max >= 100 )
  {
    max_hp[ i ] = GP_NUM_0 + ( hp_max / 100 ) * 0x20;
    hp_max = hp_max % 100;
    i++;
  }
  if( ( hp_max >= 10 ) || ( i != 0 ) )
  {
    max_hp[ i ] = GP_NUM_0 + ( hp_max / 10 ) * 0x20;
    hp_max = hp_max % 10;
    i++;
  }
  max_hp[ i ] = GP_NUM_0 + hp_max * 0x20;

  for( i = 0 ; i < BTLV_GAUGE_NUM_MAX ; i++ ){
    MI_CpuCopy16( &bgw->parts_address[ now_hp[ i ] ],
                  (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_NOWHP_CHARSTART ) * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20);
    MI_CpuCopy16( &bgw->parts_address[ max_hp[ i ] ],
                  (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_MAXHP_CHARSTART ) * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20);
  }
}

//--------------------------------------------------------------
/**
 * @brief   LV���l�`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static  void  PutLVNumOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl )
{
#if 0
  int i;
  u16 level[ BTLV_GAUGE_NUM_MAX ];
  int lv;
  void *obj_vram;
  NNSG2dImageProxy image;

  obj_vram = G2_GetOBJCharPtr();
  GFL_CLACT_WK_GetImgProxy( bgcl->base_clwk, &image );

  level[ 0 ] = GP_LV;
  level[ 1 ] = GP_SPACE;
  level[ 2 ] = GP_SPACE;

  //LV�v�Z
  lv = bgcl->level;
  i = 1;
  if( lv >= 100 )
  {
    level[ 0 ] = GP_NUM_1;
    level[ 1 ] = GP_NUM_0;
    level[ 2 ] = GP_NUM_0;
  }
  else
  {
    if( lv >= 10 )
    {
      level[ i ] = GP_NUM_0 + ( lv / 10 ) * 0x20;
      lv = lv % 10;
      i++;
    }
    level[ i ] = GP_NUM_0 + lv * 0x20;
  }

  for( i = 0 ; i < BTLV_GAUGE_NUM_MAX ; i++ ){
    MI_CpuCopy16( &bgw->parts_address[ level[ i ] ],
                  (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_LV_CHARSTART ) * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20);
  }
#else
  STRBUF* num = GFL_STR_CreateBuffer( 4, bgw->heapID );
  PRINTSYS_LSB  color = PRINTSYS_LSB_Make( 1, 4, 0 );
  GFL_BMP_DATA* bmp = GFL_BMP_Create( 3, BTLV_GAUGE_BMP_SIZE_Y, GFL_BMP_16_COLOR, bgw->heapID );
  u8 letter, shadow, back;

  GFL_BMP_Clear( bmp, 0 );

  STRTOOL_SetNumber( num, bgcl->level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );

  GFL_FONTSYS_GetColor( &letter, &shadow, &back );
  GFL_FONTSYS_SetColor( PRINTSYS_LSB_GetL( color ), PRINTSYS_LSB_GetS( color ), PRINTSYS_LSB_GetB( color ) );
  PRINTSYS_Print( bmp, 0, BTLV_GAUGE_BMP_POS_Y, num, bgw->font );
  GFL_FONTSYS_SetColor( letter, shadow, back );

  {
    void *obj_vram;
    NNSG2dImageProxy image;
    u8* bmp_data = GFL_BMP_GetCharacterAdrs( bmp );

    obj_vram = G2_GetOBJCharPtr();
    GFL_CLACT_WK_GetImgProxy( bgcl->base_clwk, &image );

    if( bgcl->gauge_dir )
    { 
      MI_CpuCopy16( bmp_data,
                    (void*)( (u32)obj_vram + BTLV_GAUGE_LVU_CHARSTART_E * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 3 );
      MI_CpuCopy16( &bmp_data[ 3 * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_LVD_CHARSTART_E * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 3 );
    }
    else
    { 
      MI_CpuCopy16( bmp_data,
                    (void*)( (u32)obj_vram + BTLV_GAUGE_LVU_CHARSTART_M * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 3 );
      MI_CpuCopy16( &bmp_data[ 3 * 0x20 ],
                    (void*)( (u32)obj_vram + BTLV_GAUGE_LVD_CHARSTART_M * 0x20 +
                    image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                    0x20 * 3 );
    }
  }

  GFL_STR_DeleteBuffer( num );
  GFL_BMP_Delete( bmp );
#endif
}

//--------------------------------------------------------------
/**
 * @brief  �ߊl�{�[���`��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static  void  PutBallOBJ( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK *bgcl )
{
  void *obj_vram;
  NNSG2dImageProxy image;

  if( bgcl->getball == 0 )
  {
    return;
  }

  obj_vram = G2_GetOBJCharPtr();
  GFL_CLACT_WK_GetImgProxy( bgcl->base_clwk, &image );

  MI_CpuCopy16( &bgw->parts_address[ GP_BALL ],
                (void*)( (u32)obj_vram + BTLV_GAUGE_BALL_CHARSTART * 0x20 +
                image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                0x20 );
}

//--------------------------------------------------------------
/**
 * @brief  �_���[�W�Q�[�W�`��
 *
 * @param bgw     BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl    �Q�[�W���[�N�ւ̃|�C���^
 * @param put_dot �`��h�b�g��
 */
//--------------------------------------------------------------
static  void  PutGaugeDamageObj( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl, int put_dot )
{ 
  int i;
  void *obj_vram;
  NNSG2dImageProxy image;
  u8  gauge_chr[ BTLV_GAUGE_HP_CHARMAX ];
  int dot = bgcl->damage_dot - put_dot;

  obj_vram = G2_GetOBJCharPtr();

  for( i = 0 ; i < BTLV_GAUGE_HP_CHARMAX ; i++ )
  {
    gauge_chr[ i ] = 8;
  }

  for( i = BTLV_GAUGE_HP_CHARMAX - 1 ; i>= 0 ; i-- )
  { 
    if( dot >= BTLV_GAUGE_DOTTO )
    { 
      gauge_chr[ i ] = 0;
      dot -= BTLV_GAUGE_DOTTO;
    }
    else
    { 
      gauge_chr[ i ] = BTLV_GAUGE_DOTTO - dot;
      break;
    }
  }

  GFL_CLACT_WK_GetImgProxy( bgcl->hp_damage_clwk, &image );
  for( i = 0 ; i < BTLV_GAUGE_HP_CHARMAX ; i++ )
  {
    MI_CpuCopy16( &bgw->parts_address[ GP_HPBAR_DAMAGE + ( gauge_chr[ i ] << 5 ) ],
                  (void*)( (u32)obj_vram + ( i + BTLV_GAUGE_HP_DAMAGE_CHARSTART ) * 0x20 +
                  image.vramLocation.baseAddrOfVram[ NNS_G2D_VRAM_TYPE_2DMAIN ] ),
                  0x20 );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W���x���A�b�v����
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param bgcl  �Q�[�W���[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static  void  Gauge_LevelUp( BTLV_GAUGE_WORK* bgw, BTLV_GAUGE_CLWK* bgcl )
{
  if( bgcl->exp_calc_req )
  {
    return;
  }
  switch( bgcl->seq_no ){
  case 0:
    PMSND_PlaySE( SEQ_SE_EXPMAX );
    /** @TODO ���x���A�b�v�G�t�F�N�g����ꂽ�� */
    bgcl->seq_no++;
    break;
  case 1:
    /** @TODO ���x���A�b�v�G�t�F�N�g�I���`�F�b�N����ꂽ�� */
    bgcl->seq_no++;
    break;
  case 2:
    bgcl->exp       = 0;
    bgcl->exp_max   = bgcl->next_exp_max;
    bgcl->exp_work  = BTLV_GAUGE_HP_WORK_INIT_VALUE;
    bgcl->exp_add   = 0;
    GaugeProc( bgcl->hpmax, bgcl->hp, 0, &bgcl->hp_work, BTLV_GAUGE_HP_CHARMAX, 1 );
    GaugeProc( bgcl->exp_max, bgcl->exp, 0, &bgcl->exp_work, BTLV_GAUGE_EXP_CHARMAX, 1 );
    PutGaugeOBJ( bgw, bgcl, BTLV_GAUGE_REQ_HP );
    PutGaugeOBJ( bgw, bgcl, BTLV_GAUGE_REQ_EXP );
    PutHPNumOBJ( bgw, bgcl, bgcl->hp );
    PutLVNumOBJ( bgw, bgcl );
    bgcl->level_up_req = 0;
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W��ꏈ��
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param pos   �Q�[�W�ʒu
 */
//--------------------------------------------------------------
static  void  Gauge_Yure( BTLV_GAUGE_WORK* bgw, BtlvMcssPos pos )
{ 
  GFL_CLACTPOS  yure_ofs;

  yure_ofs.x = -MOVE_VALUE;
  yure_ofs.y = FX_SinIdx( bgw->yure_angle ) / YURE_OFFSET;
  bgw->yure_angle += YURE_SPEED;

  BTLV_GAUGE_SetPos( bgw, pos, &yure_ofs );
}

//--------------------------------------------------------------
/**
 * @brief   ��Ԉُ�A�C�R���Z�b�g
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param sick  �Z�b�g�����Ԉُ�
 * @param pos   �Z�b�g����|�P�����̗����ʒu
 */
//--------------------------------------------------------------
void  BTLV_GAUGE_SetStatus( BTLV_GAUGE_WORK* bgw, PokeSick sick, BtlvMcssPos pos )
{
  int sick_anm[]={
    APP_COMMON_ST_ICON_NONE,        // �Ȃ��i�A�j���ԍ��I�ɂ��Ȃ��j
    APP_COMMON_ST_ICON_MAHI,        // ���
    APP_COMMON_ST_ICON_NEMURI,      // ����
    APP_COMMON_ST_ICON_KOORI,       // �X
    APP_COMMON_ST_ICON_YAKEDO,      // �Ώ�
    APP_COMMON_ST_ICON_DOKU,        // ��
    APP_COMMON_ST_ICON_DOKUDOKU,    // �ǂ��ǂ�
  };
  //�Q�[�W�����݂��Ȃ��Ƃ��͂Ȃɂ������Ƀ��^�[��
  if( bgw->bgcl[ pos ].status_clwk == NULL )
  {
    return;
  }
  if( sick == POKESICK_NULL )
  {
    GFL_CLACT_WK_SetDrawEnable( bgw->bgcl[ pos ].status_clwk, FALSE );
  }
  else
  {
    BTL_Printf("GAUGE[%d] SetStatus(%d)\n", pos, sick);
    GFL_CLACT_WK_SetDrawEnable( bgw->bgcl[ pos ].status_clwk, TRUE );
    GFL_CLACT_WK_SetAnmSeq( bgw->bgcl[ pos ].status_clwk, sick_anm[ sick ] );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W��ꃊ�N�G�X�g
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param pos   ���N�G�X�g����|�P�����̗����ʒu
 */
//--------------------------------------------------------------
void  BTLV_GAUGE_RequestYure( BTLV_GAUGE_WORK* bgw, BtlvMcssPos pos )
{ 
  int i;

  for( i = 0 ; i < BTLV_MCSS_POS_MAX ; i++ )
  { 
    if( ( bgw->bgcl[ i ].base_clwk ) && ( bgw->bgcl[ i ].yure_req ) )
    { 
      GFL_CLACTPOS  ofs = { -MOVE_VALUE, 0 };

      bgw->bgcl[ i ].yure_req = 0;
      BTLV_GAUGE_SetPos( bgw, i, &ofs );
    }
  }
  //�����ʒu�w�肪�͈͊O�Ȃ�Q�[�W�̂����~�߂邾���Ń��^�[��
  if( pos != BTLV_MCSS_POS_MAX )
  { 
    bgw->bgcl[ pos ].yure_req = 1;
    bgw->yure_angle = 0;
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W��ꃊ�N�G�X�g
 *
 * @param bgw   BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 * @param pos   ���N�G�X�g����|�P�����̗����ʒu
 */
//--------------------------------------------------------------
int  BTLV_GAUGE_GetPinchBGMFlag( BTLV_GAUGE_WORK* bgw )
{ 
  return  bgw->pinch_bgm_flag;
}

//--------------------------------------------------------------
/**
 * @brief   �s���`BGM�Đ��`�F�b�N
 *
 * @param bgw     BTLV_GAUGE_WORK�Ǘ��\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------
static  void  pinch_bgm_check( BTLV_GAUGE_WORK* bgw )
{
  int i;
  BOOL  not_check_flag = FALSE;

  if( bgw->bgm_fade_flag )
  {
    if( PMSND_CheckFadeOnBGM() == FALSE )
    {
      bgw->bgm_fade_flag = 0;
      if( bgw->pinch_bgm_flag )
      {
        PMSND_PopBGM();
        if( ( bgw->trainer_bgm_change_flag == 0 ) && ( BTLV_EFFECT_GetTrainerBGMChangeFlag() ) )
        { 
          bgw->now_bgm_no = SEQ_BGM_BATTLESUPERIOR;
          PMSND_PlayBGM( bgw->now_bgm_no );
          bgw->trainer_bgm_change_flag = 1;
        }
        else
        {  
          bgm_pause( FALSE );
        }
        PMSND_FadeInBGM( 24 );
      }
      else
      {
        bgm_pause( TRUE );
        PMSND_PushBGM();
        PMSND_PlayBGM( SEQ_BGM_BATTLEPINCH );
      }
      bgw->pinch_bgm_flag ^= 1;
    }
  }
  else
  {
    BOOL  pinch_flag = FALSE;

    for( i = 0 ; i < BTLV_GAUGE_NUM_MAX ; i++ )
    {
      //�����ꂩ�̌v�Z�Œ��̓`�F�b�N�����Ȃ�
      if( ( bgw->bgcl[ i ].hp_calc_req ) ||
          ( bgw->bgcl[ i ].exp_calc_req ) ||
          ( bgw->bgcl[ i ].level_up_req ) )
      { 
        not_check_flag = TRUE;
        continue;
      }
      if( ( bgw->bgcl[ i ].gauge_dir == 0 ) &&
          ( bgw->bgcl[ i ].gauge_enable ) &&
          ( bgw->bgcl[ i ].gauge_type != BTLV_GAUGE_TYPE_SAFARI ) )
      {
        u8  color = GAUGETOOL_GetHPGaugeDottoColor( bgw->bgcl[ i ].hp, bgw->bgcl[ i ].hpmax, BTLV_GAUGE_HP_DOTTOMAX );

        if( color == GAUGETOOL_HP_DOTTO_RED )
        {
          pinch_flag = TRUE;
        }
      }
    }
    //�����W���O���Ƃ��ɕω����Ă�����A�s���`BGM�`�F�b�N�͂��Ȃ��悤�ɂ���
    if( ( ( bgw->pinch_bgm_flag ) || ( PMSND_GetBGMsoundNo() == bgw->now_bgm_no ) ) && ( not_check_flag == FALSE ) )
    {
      if( ( ( bgw->pinch_bgm_flag ) && ( pinch_flag == FALSE ) ) ||
          ( ( bgw->pinch_bgm_flag == 0 ) && ( pinch_flag == TRUE ) ) )
      {
        bgw->bgm_fade_flag = 1;
        PMSND_FadeOutBGM( 8 );
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W�ړ��^�X�N
 */
//--------------------------------------------------------------
static  void  TCB_BTLV_GAUGE_Move( GFL_TCB* tcb, void* work )
{ 
  BTLV_GAUGE_CLWK*  bgcl = ( BTLV_GAUGE_CLWK* )work;
  GFL_CLACTPOS  cl_pos;
  int           move_value;

  if( bgcl->gauge_dir )
  { 
    move_value = MOVE_SPEED;
  }
  else
  { 
    move_value = -MOVE_SPEED;
  }

  GFL_CLACT_WK_GetPos( bgcl->base_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  cl_pos.x += move_value;
  GFL_CLACT_WK_SetPos( bgcl->base_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  GFL_CLACT_WK_GetPos( bgcl->hp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  cl_pos.x += move_value;
  GFL_CLACT_WK_SetPos( bgcl->hp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  GFL_CLACT_WK_GetPos( bgcl->status_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  cl_pos.x += move_value;
  GFL_CLACT_WK_SetPos( bgcl->status_clwk, &cl_pos, CLSYS_DEFREND_MAIN );

  if( ( bgcl->gauge_dir == 0 ) && ( bgcl->gauge_type == BTLV_GAUGE_TYPE_1vs1 ) )
  {
    GFL_CLACT_WK_GetPos( bgcl->hpnum_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
    cl_pos.x += move_value;
    GFL_CLACT_WK_SetPos( bgcl->hpnum_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  }


  if( bgcl->exp_clwk )
  {
    GFL_CLACT_WK_GetPos( bgcl->exp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
    cl_pos.x += move_value;
    GFL_CLACT_WK_SetPos( bgcl->exp_clwk, &cl_pos, CLSYS_DEFREND_MAIN );
  }

  if( --bgcl->move_cnt == 0 )
  { 
    GFL_TCB_DeleteTask( tcb );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[�W�_���[�W�G�t�F�N�gTCB
 */
//--------------------------------------------------------------
static  void  TCB_BTLV_GAUGE_DamageEffect( GFL_TCB* tcb, void* work )
{ 
  BTLV_GAUGE_DAMAGE_EFFECT_WORK* bgdew = ( BTLV_GAUGE_DAMAGE_EFFECT_WORK* )work;
  int pltt_bit_tbl[] = { 0x01, 0x02, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20 };
  int i;

  switch( bgdew->seq_no ){ 
  case 0:
    bgdew->pltt_bit = 0;
    for( i = 0 ; i < BTLV_GAUGE_CLWK_MAX ; i++ )
    {
      if( bgdew->bgw->bgcl[ i ].hp_calc_req )
      {
        bgdew->pltt_bit |= pltt_bit_tbl[ i ];
      }
    }
    if( bgdew->pltt_bit )
    { 
      PaletteFadeReq( BTLV_EFFECT_GetPfd(), PF_BIT_MAIN_OBJ, bgdew->pltt_bit, 0, 0, 8, 16, BTLV_EFFECT_GetTCBSYS() );
      bgdew->seq_no = 1;
    }
    break;
  case 1:
    if( PaletteFadeCheck( BTLV_EFFECT_GetPfd() ) == 0 )
    { 
      PaletteFadeReq( BTLV_EFFECT_GetPfd(), PF_BIT_MAIN_OBJ, bgdew->pltt_bit, 0, 8, 0, 16, BTLV_EFFECT_GetTCBSYS() );
      bgdew->seq_no = 2;
    }
    break;
  case 2:
    if( PaletteFadeCheck( BTLV_EFFECT_GetPfd() ) == 0 )
    { 
      bgdew->count++;
      if( bgdew->count == BTLV_GAUGE_DAMAGE_EFFECT_COUNT )
      { 
        bgdew->count = 0;
        bgdew->seq_no = 3;
      }
      else
      { 
        bgdew->seq_no = 0;
      }
    }
    break;
  case 3:
    for( i = 0 ; i < BTLV_GAUGE_CLWK_MAX ; i++ )
    {
      if( bgdew->bgw->bgcl[ i ].hp_calc_req )
      {
        break;
      }
    }
    if( i == BTLV_GAUGE_CLWK_MAX )
    { 
      bgdew->seq_no = 0;
    }
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief   �s���`BGM�ڍs���ɐ퓬BGM�̃|�[�Y�ݒ�/����
 */
//--------------------------------------------------------------
static  void  bgm_pause( BOOL flag )
{ 
  //�ԊO���n�̒ʐM�ł̓|�[�Y�����Ȃ�
  if( NET_DEV_IsIrcMode() == TRUE )
  { 
    return;
  }
  PMSND_PauseBGM( flag );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���ݒl�̃Q�[�W�h�b�g�����擾
 *
 * @param		prm_now		���ݒl
 * @param		prm_max		�ő�l
 * @param		dot_max		�ő�h�b�g��
 *
 * @return	�h�b�g��
 */
//--------------------------------------------------------------------------------------------
static  u32 get_num_dotto( u32 prm_now, u32 prm_max, u8 dot_max )
{
	u32 put_dot;
	
	put_dot = prm_now * dot_max / prm_max;
	if( put_dot == 0 && prm_now > 0 )   // �ޯČv�Z�ł�0�ł����ۂ̒l��1�ȏ�Ȃ�1�ޯĂɂ���
  {
		put_dot = 1;
	}
	return put_dot;
}
