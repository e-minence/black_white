
//============================================================================================
/**
 * @file  btlv_effect.c
 * @brief �퓬�G�t�F�N�g����
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"
#include "system/mcss_tool.h"
#include "sound/pm_voice.h"
#include "sound/pm_sndsys.h"

#include "btlv_effect.h"
#include "btlv_effect_def.h"

#include "field/field_light_status.h"

#include "tr_tool/trtype_def.h"
#include "tr_tool/trtype_sex.h"

#include "arc_def.h"
#include "battle/batt_bg_tbl.h"
#include "batt_bg_tbl.naix"

#include "debug/debug_hudson.h"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

#define BTLV_EFFECT_TCB_MAX ( 32 )    //�g�p����TCB��MAX

#define BTLV_EFFECT_BLINK_TIME    ( 3 )
#define BTLV_EFFECT_BLINK_WAIT    ( 3 )
#define BTLV_EFFECT_BLINK_PHYSIC  ( 0x0842 )
#define BTLV_EFFECT_BLINK_SPECIAL ( 0x6318 )

#define BTLV_EFFECT_TRAINER_INDEX_NONE  ( 0xffffffff )

#define BTLV_EFFECT_CAMERA_WORK_WAIT      ( 15 * 60 )   //�J�������[�N�J�n�܂ł̃E�G�C�g�i15�b�j
#define BTLV_EFFECT_WCS_CAMERA_WORK_WAIT  (  5 * 60 )   //WCS�J�������[�N�J�n�܂ł̃E�G�C�g�i5�b�j

#define TRTYPE_NONE ( 0xffff )

enum
{
  BTLV_EFFECT_FOCUS_OFFSET_X = 0x00004b33,
  BTLV_EFFECT_FOCUS_OFFSET_Y = 0x00002299,
  BTLV_EFFECT_FOCUS_OFFSET_Z = 0x000099cd,
  BTLV_EFFECT_FOCUS_TARGET_Y = 0x00002000,
};

#ifdef PM_DEBUG
#if defined DEBUG_ONLY_FOR_sogabe | defined DEBUG_ONLY_FOR_yoshida
//#define  CAMERA_FOCUS
#endif
#endif

#ifdef  CAMERA_FOCUS
volatile  fx32  camera_focus_offset_x = 0x00004b33; //BTLV_EFFECT_FOCUS_OFFSET_X
volatile  fx32  camera_focus_offset_y = 0x00002299; //BTLV_EFFECT_FOCUS_OFFSET_Y
volatile  fx32  camera_focus_offset_z = 0x000099cd; //BTLV_EFFECT_FOCUS_OFFSET_Z
volatile  fx32  camera_focus_target_y = 0x00002000; //BTLV_EFFECT_FOCUS_OFFSET_Z
#endif

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct _BTLV_EFFECT_SETUP_PARAM
{
  BtlRule                 rule;
  BtlCompetitor           competitor;
  BTL_FIELD_SITUATION     bfs;
  u16                     tr_type[ 4 ];
  BOOL                    multi;
  const BTLV_SCU*         scu;
  const BTL_MAIN_MODULE*  mainModule;
};

struct _BTLV_EFFECT_WORK
{
  GFL_TCBSYS*             tcb_sys;
  void*                   tcb_work;
  GFL_TCB*                tcb[ BTLV_EFFECT_TCB_MAX ];
  BTLV_EFFECT_TCB_CALLBACK_FUNC*  tcb_callback[ BTLV_EFFECT_TCB_MAX ];
  int                     tcb_group[ BTLV_EFFECT_TCB_MAX ];
  VMHANDLE*               vm_core;
  PALETTE_FADE_PTR        pfd;
  BTLV_MCSS_WORK*         bmw;
  BTLV_STAGE_WORK*        bsw;
  BTLV_FIELD_WORK*        bfw;
  BTLV_CAMERA_WORK*       bcw;
  BTLV_CLACT_WORK*        bclw;
  BTLV_GAUGE_WORK*        bgw;
  BTLV_BALL_GAUGE_WORK*   bbgw[ BTLV_BALL_GAUGE_TYPE_MAX ];
  BTLV_TIMER_WORK*        btw;
  BTLV_BG_WORK*           bbw;
  GFL_TCB*                v_tcb;
  BTLV_EFFECT_SETUP_PARAM besp;
  int                     execute_flag;
  u32                     tcb_damage_flag :BTLV_MCSS_POS_MAX;
  u32                     tcb_henge_flag  :BTLV_MCSS_POS_MAX;
  u32                     tcb_rotate_flag :2;
  u32                     bagMode         :8;
  u32                     se_mode         :1;
  u32                                     :5;
  HEAPID                  heapID;

  BOOL                    trainer_bgm_change_flag;

  int                     trainer_index[ BTLV_MCSS_POS_MAX ];

  int                     tokusei[ BTLV_MCSS_POS_MAX ];

  BTLV_EFFECT_CWE         camera_work_execute;
  int                     camera_work_seq;
  int                     camera_work_wait;
  int                     camera_work_wait_tmp;
  int                     camera_work_num;
};

typedef struct
{
  BtlvMcssPos target;
  int         seq_no;
  int         work;
  int         wait;
  int         color;
  BOOL        vanish;   //�o�j�b�V����ԂŃG�t�F�N�g���Ă΂�Ă�����TRUE
}BTLV_EFFECT_DAMAGE_TCB;

typedef struct
{
  BtlvMcssPos   vpos;
  int           seq_no;
  MCSS_ADD_WORK maw;
}BTLV_EFFECT_HENGE_TCB;

typedef struct
{
  int           seq_no;
  BtlRotateDir  dir;
  int           side;
  BOOL          se_on;
}TCB_ROTATION;

typedef struct
{
  int               seq_no;
  BtlvMcssPos       from;
  BtlvMcssPos       to;
  int               eff_no;
  BTLV_EFFVM_PARAM* param;
}TCB_EFFECT_START;

static  BTLV_EFFECT_WORK  *bew = NULL;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Damage( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Damage_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_EFFECT_Henge( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Henge_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_EFFECT_Rotation( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Rotation_CB( GFL_TCB *tcb );
static  void  TCB_BTLV_EFFECT_Start( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_EFFECT_Start_CB( GFL_TCB *tcb );
static  int   BTLV_EFFECT_GetTCBIndex( void );
static  void  BTLV_EFFECT_FreeTCBAll( void );
static  void  camera_work_check( void );

#ifdef PM_DEBUG
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 * @brief �V�X�e���������p�̃Z�b�g�A�b�v�p�����[�^����
 *
 * @param[in] heapID      �q�[�vID
 */
//============================================================================================
BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParam( BtlRule rule, BtlCompetitor competitor, const BTL_FIELD_SITUATION* bfs, BOOL multi, u16* tr_type, const BTL_MAIN_MODULE* mainModule, const BTLV_SCU* scu, HEAPID heapID )
{
  BTLV_EFFECT_SETUP_PARAM* besp = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( heapID ), sizeof( BTLV_EFFECT_SETUP_PARAM ) );
  int i;

  besp->rule        = rule;
  besp->competitor  = competitor;
  besp->bfs         = *bfs;
  besp->multi       = multi;
  besp->scu         = scu;
  besp->mainModule  = mainModule;

  for( i = 0 ; i < 4 ; i++ )
  {
    besp->tr_type[ i ] = tr_type[ i ];
  }

  return besp;
}

//============================================================================================
/**
 * @brief �V�X�e���������p�̃Z�b�g�A�b�v�p�����[�^�����i�o�g���p�j
 *
 * @param[in] mainModule  �퓬���C�����W���[��
 * @param[in] heapID      �q�[�vID
 */
//============================================================================================
BTLV_EFFECT_SETUP_PARAM*  BTLV_EFFECT_MakeSetUpParamBtl( const BTL_MAIN_MODULE* mainModule, const BTLV_SCU* viewSCU, HEAPID heapID )
{
  u16 tr_type[ 4 ];
  int i;

  for(i=0; i<NELEMS(tr_type); ++i){
    tr_type[ i ] = 0;
  }

  if( BTL_MAIN_IsMultiMode( mainModule ) == TRUE )
  {
    u8 myMultiPos = BTL_MAIN_GetPlayerMultiPos( mainModule );
    u8 myClientID = BTL_MAIN_GetPlayerClientID( mainModule );
    u8 friendClientID = BTL_MAIN_GetPlayerFriendCleintID( mainModule );

    tr_type[ myMultiPos*2 ] = BTL_MAIN_GetClientTrainerType( mainModule, myClientID );
    if( friendClientID != BTL_CLIENTID_NULL ){
      tr_type[ (myMultiPos^1)*2 ] = BTL_MAIN_GetClientTrainerType( mainModule, friendClientID );
    }
    TAYA_Printf("@@@@@@myClientID=%d, type=%d, friendClientID=%d, type=%d\n",
            myClientID, tr_type[myClientID], friendClientID, tr_type[friendClientID]);

    tr_type[ 1 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 0 ) );
    tr_type[ 3 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 1 ) );
  }
  else
  {
    tr_type[ 0 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetPlayerClientID( mainModule ) );
    tr_type[ 1 ] = BTL_MAIN_GetClientTrainerType( mainModule, BTL_MAIN_GetEnemyClientID( mainModule, 0 ) );
    tr_type[ 2 ] = 0;
    tr_type[ 3 ] = 0;
  }

  return BTLV_EFFECT_MakeSetUpParam( BTL_MAIN_GetRule( mainModule ), BTL_MAIN_GetCompetitor( mainModule ),
                                     BTL_MAIN_GetFieldSituation( mainModule ), BTL_MAIN_IsMultiMode( mainModule ),
                                     tr_type, mainModule, viewSCU, heapID );
}

//============================================================================================
/**
 * @brief �V�X�e��������
 *
 * @param[in] besp        �Z�b�g�A�b�v�p�����[�^
 * @param[in] fontHandle  �t�H���g�n���h��
 * @param[in] heapID      �q�[�vID
 */
//============================================================================================
void  BTLV_EFFECT_Init( BTLV_EFFECT_SETUP_PARAM* besp, GFL_FONT* fontHandle, HEAPID heapID )
{
  GF_ASSERT( bew == NULL );
  bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_EFFECT_WORK ) );

  bew->besp = *besp;

  bew->heapID = heapID;

  bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTLV_EFFECT_TCB_MAX ) );
  bew->tcb_sys = GFL_TCB_Init( BTLV_EFFECT_TCB_MAX, bew->tcb_work );

  bew->vm_core = BTLV_EFFVM_Init( bew->tcb_sys, heapID );

  //�p���b�g�t�F�[�h������
  bew->pfd = PaletteFadeInit( heapID );
  PaletteTrans_AutoSet( bew->pfd, TRUE );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_BG, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_BG, 0x1e0, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_MAIN_OBJ, 0x200, heapID );
  PaletteFadeWorkAllocSet( bew->pfd, FADE_SUB_OBJ, 0x1e0, heapID );

  bew->bmw  = BTLV_MCSS_Init( besp->rule, bew->tcb_sys, heapID );

  {
    BATT_BG_TBL_ZONE_SPEC_TABLE*  bbtzst = GFL_ARC_LoadDataAlloc( ARCID_BATT_BG_TBL,
                                                                  NARC_batt_bg_tbl_zone_spec_table_bin,
                                                                  GFL_HEAP_LOWID( bew->heapID ) );
    u8  season = 0;

    if( bbtzst[ besp->bfs.bgType ].season )
    {
      season = besp->bfs.season;
    }
    bew->bsw  = BTLV_STAGE_Init( besp->rule, bbtzst[ besp->bfs.bgType ].stage_file[ besp->bfs.bgAttr ], season, heapID );
    if( bew->besp.mainModule )
    {
      bew->bfw  = BTLV_FIELD_Init( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_WCS ),
                                   bbtzst[ besp->bfs.bgType ].bg_file[ besp->bfs.bgAttr ], season, heapID );
    }
    else
    {
      bew->bfw  = BTLV_FIELD_Init( FALSE, bbtzst[ besp->bfs.bgType ].bg_file[ besp->bfs.bgAttr ], season, heapID );
    }

    //���C�g�ݒ�
    if( bbtzst[ besp->bfs.bgType ].time_zone )
    {
      GFL_G3D_LIGHT light;
      FIELD_LIGHT_STATUS  fls;

      FIELD_LIGHT_STATUS_Get( besp->bfs.zoneID, besp->bfs.hour, besp->bfs.minute, besp->bfs.weather, besp->bfs.season, &fls, bew->heapID );

      light.color = fls.light;
      light.vec.x = 0;
      light.vec.y = -FX32_ONE;
      light.vec.z = 0;
      GFL_G3D_SetSystemLight( 0, &light );
    }
    GFL_HEAP_FreeMemory( bbtzst );
  }

  bew->bcw  = BTLV_CAMERA_Init( bew->tcb_sys, heapID );
  bew->bclw = BTLV_CLACT_Init( bew->tcb_sys, heapID );
  bew->bgw  = BTLV_GAUGE_Init( fontHandle, heapID );
  bew->btw  = BTLV_TIMER_Init( heapID );
  bew->bbw  = BTLV_BG_Init( bew->tcb_sys, heapID );


  BTLV_MCSS_SetOrthoMode( bew->bmw );

  //�p�[�e�B�N��������
  GFL_PTC_Init( heapID );

  //�g���[�i�[�C���f�b�N�X�Ǘ��z�񏉊���
  {
    int index;

    for( index = 0 ; index < BTLV_MCSS_POS_MAX ; index++ )
    {
      bew->trainer_index[ index ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
    }
  }

  //�J�������[�N�E�G�C�g�Z�b�g
  if( bew->besp.mainModule )
  {
    if( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_OFF ) )
    {
      bew->camera_work_wait_tmp = BTLV_EFFECT_WCS_CAMERA_WORK_WAIT;
    }
    else
    {
      bew->camera_work_wait_tmp = BTLV_EFFECT_CAMERA_WORK_WAIT;
    }
  }

#ifdef  CAMERA_FOCUS
  OS_TPrintf("camera_focus_offset_x:0x%08x\n",&camera_focus_offset_x);
  OS_TPrintf("camera_focus_offset_y:0x%08x\n",&camera_focus_offset_y);
  OS_TPrintf("camera_focus_offset_z:0x%08x\n",&camera_focus_offset_z);
  OS_TPrintf("camera_focus_target_y:0x%08x\n",&camera_focus_target_y);
#endif

  //VBlank�֐�
  bew->v_tcb = GFUser_VIntr_CreateTCB( BTLV_EFFECT_VBlank, NULL, 1 );

  //3�̓����Ŗ������������悤�Ƀo�b�t�@���m��
  PMVOICE_PlayerHeapReserve( 2, bew->heapID );
}

//============================================================================================
/**
 *  @brief  �V�X�e���I��
 */
//============================================================================================
void  BTLV_EFFECT_Exit( void )
{
  if( bew == NULL ) return;

  PMVOICE_PlayerHeapRelease();
  BTLV_EFFECT_FreeTCBAll();

  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_BG );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_MAIN_OBJ );
  PaletteFadeWorkAllocFree( bew->pfd, FADE_SUB_OBJ );
  PaletteFadeFree( bew->pfd );
  BTLV_MCSS_Exit( bew->bmw );
  BTLV_STAGE_Exit( bew->bsw );
  BTLV_FIELD_Exit( bew->bfw );
  BTLV_CAMERA_Exit( bew->bcw );
  BTLV_CLACT_Exit( bew->bclw );
  BTLV_GAUGE_Exit( bew->bgw );
  BTLV_TIMER_Exit( bew->btw );
  BTLV_BG_Exit( bew->bbw );
  GFL_PTC_Exit();
  BTLV_EFFVM_Exit( bew->vm_core );
  GFL_TCB_DeleteTask( bew->v_tcb );
  GFL_TCB_Exit( bew->tcb_sys );
  GFL_HEAP_FreeMemory( bew->tcb_work );
  GFL_HEAP_FreeMemory( bew );

  bew = NULL;

}

//============================================================================================
/**
 *  @brief  �V�X�e�����C��
 */
//============================================================================================
void  BTLV_EFFECT_Main( void )
{
  if( bew == NULL ) return;

  bew->execute_flag = BTLV_EFFVM_Main( bew->vm_core );

#ifdef DEBUG_ONLY_FOR_hudson
  if( HUDSON_IsTestCode( HUDSON_TESTCODE_ALL_WAZA_CAM ) )
  {
    static int preflag = FALSE;
    static int hitflag = FALSE;

    if( BTLV_EFFVM_GetAttackPos( bew->vm_core ) == BTLV_MCSS_POS_A )
    {
      if( bew->execute_flag )
      {
        if( BTLV_EFFVM_GetExecuteEffectType( bew->vm_core ) == EXECUTE_EFF_TYPE_WAZA )
        {
          VecFx32 pos, target;

          BTLV_CAMERA_GetCameraPosition( BTLV_EFFECT_GetCameraWork(), &pos, &target );

          // �J�����������^�Y�[���A�E�g
          if( ( pos.x == 0x00000b33 && pos.y == 0x00005333 && pos.z == 0x000114cd ) ||
              ( pos.x == 0x00008b33 && pos.y == 0x00007b33 && pos.z == 0x00017ccd )
              )
          {
            hitflag = TRUE;
          }
        }
      }
      else
      {
        if( preflag == TRUE )
        {
          OS_FPrintf( 3 ,"hit=%d ",hitflag);
        }
        else
        {
          hitflag = FALSE;
        }
      }
    }

    preflag = bew->execute_flag && BTLV_EFFVM_GetExecuteEffectType( bew->vm_core ) == EXECUTE_EFF_TYPE_WAZA;
  }
#endif // DEBUG_ONLY_FOR_hudson

  camera_work_check();

  GFL_TCB_Main( bew->tcb_sys );

  BTLV_MCSS_Main( bew->bmw );
  BTLV_STAGE_Main( bew->bsw );
  BTLV_FIELD_Main( bew->bfw );
  BTLV_CAMERA_Main( bew->bcw );
  BTLV_GAUGE_Main( bew->bgw );

  BTLV_CLACT_Main( bew->bclw );

  //3D�`��
  {
    GFL_G3D_DRAW_Start();
    GFL_G3D_DRAW_SetLookAt();
    {
      BTLV_STAGE_Draw( bew->bsw );
      BTLV_FIELD_Draw( bew->bfw );
      //�����̕`�揇�Ԃ́ANitroSDK�ŕ`�悳��Ă�����̂̂��Ƃ�NitroSystem�ŕ`�悳��Ă�����̂�u��
      //���Ԃ����݂�����Ɛ������\�����ꂸ�ň��t���[�Y����
      BTLV_MCSS_Draw( bew->bmw );
      {
        s32 particle_count;

        particle_count = G3X_GetPolygonListRamCount();

        GFL_PTC_Main();

        particle_count = G3X_GetPolygonListRamCount() - particle_count;
      }
    }

    GFL_G3D_DRAW_End();
  }
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�N��
 *
 * @param[in] eff_no  �N������G�t�F�N�g�i���o�[
 */
//============================================================================================
void  BTLV_EFFECT_Add( int eff_no )
{
  if( !BTLV_EFFECT_CheckExecute() )
  {
    BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_ERROR, BTLV_MCSS_POS_ERROR, eff_no, NULL );
    bew->execute_flag = TRUE;
  }
  else
  {
    TCB_EFFECT_START* tes = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bew->heapID ), sizeof( TCB_EFFECT_START ) );

    tes->from   = BTLV_MCSS_POS_ERROR;
    tes->to     = BTLV_MCSS_POS_ERROR;
    tes->eff_no = eff_no;

    BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Start, tes, 0 ),
                                         TCB_BTLV_EFFECT_Start_CB, GROUP_DEFAULT );
  }
}
//=============================================================================================
/**
 * @brief �G�t�F�N�g�N���i�����ʒu�̎w�肪�K�v�Ȃ��� ... �\�̓A�b�v�E�_�E���C����E�ޏꓙ�j
 *
 * @param   pos       �����ʒu
 * @param   eff_no    �G�t�F�N�g�i���o�[
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByPos( BtlvMcssPos pos, int eff_no )
{
  if( !BTLV_EFFECT_CheckExecute() )
  {
    BTLV_EFFVM_Start( bew->vm_core, pos, BTLV_MCSS_POS_ERROR, eff_no, NULL );
    bew->execute_flag = TRUE;
  }
  else
  {
    TCB_EFFECT_START* tes = GFL_HEAP_AllocClearMemory( bew->heapID, sizeof( TCB_EFFECT_START ) );

    tes->from   = pos;
    tes->to     = BTLV_MCSS_POS_ERROR;
    tes->eff_no = eff_no;

    BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Start, tes, 0 ),
                                         TCB_BTLV_EFFECT_Start_CB, GROUP_DEFAULT );
  }
}
//=============================================================================================
/**
 * @brief �G�t�F�N�g�N���i�����ʒu�̎w��ƕ������K�v�Ȃ��� ... �^�[���`�F�b�N���̂�ǂ肬�̃^�l���j
 *
 * @param   from      �����ʒu
 * @param   to        ����
 * @param   eff_no    �G�t�F�N�g�i���o�[
 *
 */
//=============================================================================================
void BTLV_EFFECT_AddByDir( BtlvMcssPos from, BtlvMcssPos to, int eff_no )
{
  if( !BTLV_EFFECT_CheckExecute() )
  {
    BTLV_EFFVM_Start( bew->vm_core, from, to, eff_no, NULL );
    bew->execute_flag = TRUE;
  }
  else
  {
    TCB_EFFECT_START* tes = GFL_HEAP_AllocClearMemory( bew->heapID, sizeof( TCB_EFFECT_START ) );

    tes->from   = from;
    tes->to     = to;
    tes->eff_no = eff_no;

    BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Start, tes, 0 ),
                                         TCB_BTLV_EFFECT_Start_CB, GROUP_DEFAULT );
  }
}
//=============================================================================================
/**
 * @brief �G�t�F�N�g�N���i���U�G�t�F�N�g��p�j
 *
 * @param   param   [in] �G�t�F�N�g�p�����[�^
 */
//=============================================================================================
void BTLV_EFFECT_AddWazaEffect( const BTLV_WAZAEFFECT_PARAM* param )
{
  if( !BTLV_EFFECT_CheckExecute() )
  {
    BTLV_EFFVM_PARAM effvm_param;

    BTLV_EFFVM_ClearParam( &effvm_param );

    effvm_param.waza_range = param->waza_range;
    effvm_param.turn_count = param->turn_count;
    effvm_param.continue_count = param->continue_count;

    BTLV_EFFVM_Start( bew->vm_core, param->from, param->to, param->waza, &effvm_param );
    bew->execute_flag = TRUE;
  }
  else
  {
    TCB_EFFECT_START* tes = GFL_HEAP_AllocClearMemory( bew->heapID, sizeof( TCB_EFFECT_START ) );
    tes->param = GFL_HEAP_AllocClearMemory( bew->heapID, sizeof( BTLV_EFFVM_PARAM ) );

    tes->from   = param->from;
    tes->to     = param->to;
    tes->eff_no = param->waza;

    tes->param->waza_range      = param->waza_range;
    tes->param->turn_count      = param->turn_count;
    tes->param->continue_count  = param->continue_count;

    BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Start, tes, 0 ),
                                         TCB_BTLV_EFFECT_Start_CB, GROUP_DEFAULT );
  }
}

//=============================================================================================
/**
 * @brief �G�t�F�N�g������~
 */
//=============================================================================================
void BTLV_EFFECT_Stop( void )
{
  BTLV_EFFVM_Stop( bew->vm_core );
  bew->execute_flag = FALSE;
}

//=============================================================================================
/**
 * @brief �G�t�F�N�g�ĊJ
 */
//=============================================================================================
void BTLV_EFFECT_Restart( void )
{
  BTLV_EFFVM_Restart( bew->vm_core );
  bew->execute_flag = TRUE;
}

//=============================================================================================
/**
 * @brief �_���[�W�G�t�F�N�g�N��
 *
 * @param   target    �����ʒu
 * @param   wazaID    ���U�i���o�[
 */
//=============================================================================================
void BTLV_EFFECT_Damage( BtlvMcssPos target, WazaID waza )
{
  BTLV_EFFECT_DAMAGE_TCB *bedt = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bew->heapID ), sizeof(BTLV_EFFECT_DAMAGE_TCB) );

  bedt->seq_no = 0;
  bedt->target = target;
  bedt->work = BTLV_EFFECT_BLINK_TIME;
  bedt->wait = 0;
  bedt->vanish = BTLV_MCSS_GetVanishFlag( bew->bmw, target );

  if( WAZADATA_GetDamageType( waza ) == WAZADATA_DMG_PHYSIC )
  {
    //�����_���[�W
    bedt->color = BTLV_EFFECT_BLINK_PHYSIC;
  }
  else
  {
    //����_���[�W
    bedt->color = BTLV_EFFECT_BLINK_SPECIAL;
  }

  bew->tcb_damage_flag |= BTLV_EFFTOOL_Pos2Bit( target );

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Damage, bedt, 0 ),
                                       TCB_BTLV_EFFECT_Damage_CB, GROUP_DEFAULT );
}
//=============================================================================================
/**
 * @brief �m���G�t�F�N�g�N��
 *
 * @param   target    �����ʒu
 */
//=============================================================================================
void BTLV_EFFECT_Hinshi( BtlvMcssPos target )
{
  BTLV_EFFECT_AddByPos( target, BTLEFF_HINSHI );
  //���������������N���A���Ă���
  BTLV_EFFECT_SetLookTokusei( target, TOKUSYU_NULL );
}

//=============================================================================================
/**
 * @brief �ߊl���{�[�������G�t�F�N�g�N��
 *
 * @param   vpos        �Ώۃ|�P�����̕`��ʒu
 * @param   itemNum     �������{�[���̃A�C�e���i���o�[
 * @param   yure_cnt    �{�[���h��񐔁i0�`3�j
 * @param   f_success   �ߊl�����t���O
 * @param   f_critical  �N���e�B�J���t���O
 */
//=============================================================================================
void BTLV_EFFECT_BallThrow( int vpos, u16 item_no, u8 yure_cnt, BOOL f_success, BOOL f_critical )
{
  BTLV_EFFVM_PARAM  effvm_param;

  BTLV_EFFVM_ClearParam( &effvm_param );

  effvm_param.yure_cnt      = yure_cnt;
  effvm_param.get_success   = f_success;
  effvm_param.get_critical  = f_critical;
  effvm_param.item_no       = item_no;

  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_AA, vpos, BTLEFF_BALL_THROW, &effvm_param );
  bew->execute_flag = TRUE;
}

//=============================================================================================
/**
 * @brief �ߊl���{�[�������G�t�F�N�g�N���i�g���[�i�[��p�j
 *
 * @param   vpos        �Ώۃ|�P�����̕`��ʒu
 * @param   itemNum     �������{�[���̃A�C�e���i���o�[
 */
//=============================================================================================
void BTLV_EFFECT_BallThrowTrainer( int vpos, u16 item_no )
{
  BTLV_EFFVM_PARAM  effvm_param;

  BTLV_EFFVM_ClearParam( &effvm_param );

  effvm_param.item_no = item_no;

  BTLV_EFFVM_Start( bew->vm_core, BTLV_MCSS_POS_AA, vpos, BTLEFF_BALL_THROW_TRAINER, &effvm_param );
  bew->execute_flag = TRUE;
}

//=============================================================================================
/**
 * @brief �ω��G�t�F�N�g�N��
 *
 * @param[in] pp    �Ώۃ|�P������POKEMON_PARAM
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void BTLV_EFFECT_Henge( const POKEMON_PARAM* pp, BtlvMcssPos vpos )
{
  BTLV_EFFECT_HENGE_TCB *beht = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bew->heapID ), sizeof(BTLV_EFFECT_HENGE_TCB) );

  beht->seq_no = 0;
  beht->vpos = vpos;

  BTLV_MCSS_SetHengeParam( bew->bmw, vpos, pp );
  BTLV_MCSS_MakeMAW( bew->bmw, pp, &beht->maw, vpos );

  bew->tcb_henge_flag |= BTLV_EFFTOOL_Pos2Bit( vpos );

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Henge, beht, 0 ),
                      TCB_BTLV_EFFECT_Henge_CB, GROUP_DEFAULT );
}

//=============================================================================================
/**
 * @brief �ω��G�t�F�N�g�N���i�^��Đ��p�̃V���[�g�J�b�g�Łj
 *
 * @param[in] pp    �Ώۃ|�P������POKEMON_PARAM
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void BTLV_EFFECT_HengeShortCut( const POKEMON_PARAM* pp, BtlvMcssPos vpos )
{
  MCSS_ADD_WORK maw;
  BTLV_MCSS_SetHengeParam( bew->bmw, vpos, pp );
  BTLV_MCSS_MakeMAW( bew->bmw, pp, &maw, vpos );
  BTLV_MCSS_OverwriteMAW( bew->bmw, vpos, &maw );
}

//=============================================================================================
/**
 * @brief �|�P�����o�j�b�V���t���OON
 *
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void  BTLV_EFFECT_PokemonVanishOn( BtlvMcssPos vpos )
{
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_POKEMON_VANISH_ON );
}

//=============================================================================================
/**
 * @brief �|�P�����o�j�b�V���t���OOFF
 *
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void  BTLV_EFFECT_PokemonVanishOff( BtlvMcssPos vpos )
{
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_POKEMON_VANISH_OFF );
}

//=============================================================================================
/**
 * @brief �g������o��
 *
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void  BTLV_EFFECT_CreateMigawari( BtlvMcssPos vpos )
{
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_MIGAWARI_FALL );
}

//=============================================================================================
/**
 * @brief �g���������
 *
 * @param[in] vpos  �Ώۃ|�P�����̕`��ʒu
 */
//=============================================================================================
void  BTLV_EFFECT_DeleteMigawari( BtlvMcssPos vpos )
{
  BTLV_EFFECT_AddByPos( vpos, BTLEFF_MIGAWARI_DEAD );
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�N�������`�F�b�N
 *
 * @retval FALSE:�N�����Ă��Ȃ��@TRUE:�N����
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecute( void )
{
  return ( ( bew->execute_flag | bew->tcb_damage_flag | bew->tcb_henge_flag | bew->tcb_rotate_flag ) != 0 );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu�Ƀ|�P�������Z�b�g
 *
 * @param[in] pp      �Z�b�g����|�P������POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemon( const POKEMON_PARAM *pp, int position )
{
  BTLV_MCSS_Add( bew->bmw, pp, position );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu�̃|�P�������폜
 *
 * @param[in] position  �폜����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_DelPokemon( int position )
{
  BTLV_MCSS_Del( bew->bmw, position );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu��MCSS�����݂��邩�`�F�b�N
 *
 * @param[in] position  �`�F�b�N���闧���ʒu
 *
 * @retval  TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExist( int position )
{
  if( position < BTLV_MCSS_POS_MAX )
  {
    return BTLV_MCSS_CheckExist( bew->bmw, position );
  }
  else
  {
    return( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE );
  }
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�Ƀg���[�i�[���Z�b�g
 *
 * @param[in] trtype    �Z�b�g����g���[�i�[�^�C�v
 * @param[in] position  �Z�b�g����g���[�i�[�̗����ʒu
 * @param[in] pos_x     �Z�b�g����g���[�i�[��X���W�i�O�ŗ����ʒu��X���W�����j
 * @param[in] pos_y     �Z�b�g����g���[�i�[��Y���W�i�O�ŗ����ʒu��Y���W�����j
 */
//============================================================================================
void  BTLV_EFFECT_SetTrainer( int trtype, int position, int pos_x, int pos_y, int pos_z )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( ( position - BTLV_MCSS_POS_MAX ) < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] == BTLV_EFFECT_TRAINER_INDEX_NONE );

  if( ( position & 1 ) == 0 )
  {
    switch( trtype ){
    case TRTYPE_HERO:
    case TRTYPE_HEROINE:
      if( BTLV_EFFECT_CheckShooterEnable() )
      {
        trtype += 4;
      }
      break;
    case TRTYPE_DOCTOR:
      trtype = 2;
      break;
    case TRTYPE_RIVAL:
      trtype = 3;
      break;
    default:
      if( TrTypeSexTable[ trtype ] == PTL_SEX_MALE )
      {
        trtype = TRTYPE_HERO;
      }
      else
      {
        trtype = TRTYPE_HEROINE;
      }
      break;
    }
  }
  else
  {
    switch( trtype ){
    case TRTYPE_HERO:
    case TRTYPE_HEROINE:
      //�V���[�^�[���[�h�ł͐�p�̊G�ɍ����ւ�
      if( BTLV_EFFECT_CheckShooterEnable() )
      {
        trtype += TRTYPE_HERO_S;
      }
      break;
    default:
      break;
    }
  }
  BTLV_MCSS_AddTrainer( bew->bmw, trtype, position );
  if( ( pos_x != 0 ) || ( pos_y != 0 ) || ( pos_z != 0 ) )
  {
    BTLV_MCSS_SetPosition( bew->bmw, position, pos_x, pos_y, pos_z );
  }
  bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = position;
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu�̃g���[�i�[���폜
 *
 * @param[in] position  �폜����g���[�i�[�̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_DelTrainer( int position )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( position - BTLV_MCSS_POS_MAX < BTLV_MCSS_POS_MAX );
  GF_ASSERT_MSG( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE, "pos=%d", position );

  BTLV_MCSS_Del( bew->bmw, position );
  bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] = BTLV_EFFECT_TRAINER_INDEX_NONE;
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�ɃQ�[�W���Z�b�g
 *
 * @param[in] pp        �Q�[�W�\������|�P������POEKMON_PARAM�\���̂̃|�C���^
 * @param[in] position  �Z�b�g����Q�[�W�ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetGauge( const BTL_MAIN_MODULE* wk, const BTL_POKEPARAM* bpp, int position )
{
  switch( bew->besp.rule ){
  case BTL_RULE_TRIPLE:
    BTLV_GAUGE_Add( bew->bgw, wk, bpp, BTLV_GAUGE_TYPE_3vs3, position );
    break;
  case BTL_RULE_ROTATION:
    BTLV_GAUGE_Add( bew->bgw, wk, bpp, BTLV_GAUGE_TYPE_ROTATE, position );
    break;
  default:
    BTLV_GAUGE_Add( bew->bgw, wk, bpp, BTLV_GAUGE_TYPE_1vs1, position );
    break;
  }
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�ɃQ�[�W���Z�b�g�iPOKEMON_PARAM�o�[�W�����j
 *
 * @param[in] pp        �Q�[�W�\������|�P������POEKMON_PARAM�\���̂̃|�C���^
 * @param[in] position  �Z�b�g����Q�[�W�ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugePP( const ZUKAN_SAVEDATA* zs, const POKEMON_PARAM* pp, int position )
{
  switch( bew->besp.rule ){
  case BTL_RULE_TRIPLE:
    BTLV_GAUGE_AddPP( bew->bgw, zs, pp, BTLV_GAUGE_TYPE_3vs3, position );
    break;
  case BTL_RULE_ROTATION:
    BTLV_GAUGE_AddPP( bew->bgw, zs, pp, BTLV_GAUGE_TYPE_ROTATE, position );
    break;
  default:
    BTLV_GAUGE_AddPP( bew->bgw, zs, pp, BTLV_GAUGE_TYPE_1vs1, position );
    break;
  }
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�̃Q�[�W���폜
 *
 * @param[in] position  �폜����Q�[�W�ʒu
 */
//============================================================================================
void  BTLV_EFFECT_DelGauge( int position )
{
  BTLV_GAUGE_Del( bew->bgw, position );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu��HP�Q�[�W�v�Z
 *
 * @param[in] position  �v�Z����Q�[�W�ʒu
 * @param[in] value     �v�Z��
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeHP( int position, int value )
{
  BTLV_GAUGE_CalcHP( bew->bgw, position, value );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu��HP�Q�[�W�v�Z�i�Q�[�W�̑����G�t�F�N�g�Ȃ��ł������f�j
 *
 * @param[in] position  �v�Z����Q�[�W�ʒu
 * @param[in] value     �v�Z��
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeHPAtOnce( int position, int value )
{
  BTLV_GAUGE_CalcHPAtOnce( bew->bgw, position, value );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu��EXP�Q�[�W�v�Z
 *
 * @param[in] position  �v�Z����Q�[�W�ʒu
 * @param[in] value     �v�Z��
 */
//============================================================================================
void  BTLV_EFFECT_CalcGaugeEXP( int position, int value )
{
  BTLV_GAUGE_CalcEXP( bew->bgw, position, value );
}

//=============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu��EXP�Q�[�W���x���A�b�v
 *
 * @param   position    �v�Z����Q�[�W�ʒu
 * @param   bpp         ���x���A�b�v��̃p�����[�^
 */
//=============================================================================================
void BTLV_EFFECT_CalcGaugeEXPLevelUp( int position, const BTL_POKEPARAM* bpp )
{
  BTLV_GAUGE_CalcEXPLevelUp( bew->bgw, bpp, position );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q�[�W���[�h��؂�ւ���
 */
//-----------------------------------------------------------------------------
void BTLV_EFFECT_SwitchGaugeMode( void )
{
  BTLV_GAUGE_SwitchGaugeMode( bew->bgw );
}

//============================================================================================
/**
 * @brief  �Q�[�W���v�Z�����`�F�b�N
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecuteGauge( void )
{
  return BTLV_GAUGE_CheckExecute( bew->bgw );
}

//============================================================================================
/**
 * @brief  �Q�[�W�\��/��\��
 *
 * @param[in] on_off  �\���^��\���t���O
 * @param[in] side    ���삷�鑤
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeDrawEnable( BOOL on_off, int side )
{
  BTLV_GAUGE_SetDrawEnable( bew->bgw, on_off, side );
}

//============================================================================================
/**
 * @brief  �Q�[�W�\��/��\���i�ꏊ�w��j
 *
 * @param[in] on_off  �\���^��\���t���O
 * @param[in] pos     ���삷�闧���ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeDrawEnableByPos( BOOL on_off, BtlvMcssPos pos )
{
  BTLV_GAUGE_SetDrawEnableByPos( bew->bgw, on_off, pos );
}

//============================================================================================
/**
 * @brief  �Q�[�W�ɃX�e�[�^�X�A�C�R����\��
 *
 * @param[in] sick  �Z�b�g�����Ԉُ�
 * @param[in] pos   �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeStatus( PokeSick sick,  BtlvMcssPos pos )
{
  BTLV_GAUGE_SetStatus( bew->bgw, sick, pos );
}

//============================================================================================
/**
 * @brief  �Q�[�W�����Z�b�g
 *
 * @param[in] pos   �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetGaugeYure( BtlvMcssPos pos )
{
  BTLV_GAUGE_RequestYure( bew->bgw, pos );
}

//============================================================================================
/**
 *  @brief  �Q�[�W�����݂��邩�ǂ����`�F�b�N
 *
 *  @param[in] pos  �`�F�b�N����|�W�V����
 *
 *  @retval TRUE:���݂��� FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExistGauge( BtlvMcssPos pos )
{
  return BTLV_GAUGE_CheckExist( bew->bgw, pos );
}

//============================================================================================
/**
 *  @brief  �Q�[�W�̏�Ԃ��擾
 *
 *  @param[in]  pos       �擾����|�W�V����
 *  @param[out] color     �擾����HP�Q�[�W�J���[
 *  @param[out] sick_anm  �擾������Ԉُ�A�j���i���o�[
 *
 *  @retval TRUE:�擾���� FALSE:�擾���s
 */
//============================================================================================
BOOL  BTLV_EFFECT_GetGaugeStatus( BtlvMcssPos pos, int* color, int* sick_anm )
{
  return BTLV_GAUGE_GetGaugeStatus( bew->bgw, pos, color, sick_anm );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�Ƀ{�[���Q�[�W���Z�b�g
 *
 * @param[in] bbgp  �{�[���Q�[�W�p�������p�����[�^
 */
//============================================================================================
void  BTLV_EFFECT_SetBallGauge( const BTLV_BALL_GAUGE_PARAM* bbgp )
{
  bew->bbgw[ bbgp->type ] = BTLV_BALL_GAUGE_Create( bbgp, bew->heapID );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�ʒu�̃{�[���Q�[�W���폜
 *
 * @param[in] type  �폜����Q�[�W�^�C�v
 */
//============================================================================================
void  BTLV_EFFECT_DelBallGauge( BTLV_BALL_GAUGE_TYPE type )
{
  BTLV_BALL_GAUGE_Delete( bew->bbgw[ type ] );
}

//============================================================================================
/**
 * @brief  �Q�[�W���v�Z�����`�F�b�N
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecuteBallGauge( BTLV_BALL_GAUGE_TYPE type )
{
  return BTLV_BALL_GAUGE_CheckExecute( bew->bbgw[ type ] );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ3D���f���ɑ΂���p���b�g�t�F�[�h���Z�b�g
 *
 * @param[in] model       �ΏۂƂ���3D���f��
 * @param[in] start_evy   �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���J�n����16�i�K�j
 * @param[in] end_evy     �Z�b�g����p�����[�^�i�t�F�[�h������F�ɑ΂���I������16�i�K�j
 * @param[in] wait        �Z�b�g����p�����[�^�i�E�F�C�g�j
 * @param[in] rgb         �Z�b�g����p�����[�^�i�t�F�[�h������F�j
 */
//============================================================================================
void  BTLV_EFFECT_SetPaletteFade( int model, u8 start_evy, u8 end_evy, u8 wait, u16 rgb )
{
  if( ( model == BTLEFF_PAL_FADE_STAGE ) ||
      ( model == BTLEFF_PAL_FADE_3D ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    BTLV_STAGE_SetPaletteFade( bew->bsw, start_evy, end_evy, wait, rgb );
  }
  if( ( model == BTLEFF_PAL_FADE_FIELD ) ||
      ( model == BTLEFF_PAL_FADE_3D ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    BTLV_FIELD_SetPaletteFade( bew->bfw, start_evy, end_evy, wait, rgb );
  }
  if( ( model == BTLEFF_PAL_FADE_EFFECT ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    PaletteFadeReq( bew->pfd, PF_BIT_MAIN_BG, BTLEFF_PAL_FADE_EFFECT_BIT, wait,
                    start_evy, end_evy, rgb, bew->tcb_sys );
  }
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ3D���f���ɑ΂���p���b�g�t�F�[�h�̎��s�`�F�b�N
 *
 * @param[in] model       �ΏۂƂ���3D���f��
 *
 * @retval  TRUE:���s���@FALSE:�I��
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckExecutePaletteFade( int model )
{
  BOOL  ret_stage   = FALSE;
  BOOL  ret_field   = FALSE;
  BOOL  ret_effect  = FALSE;

  if( ( model == BTLEFF_PAL_FADE_STAGE ) ||
      ( model == BTLEFF_PAL_FADE_3D ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    ret_stage = BTLV_STAGE_CheckExecutePaletteFade( bew->bsw );
  }
  if( ( model == BTLEFF_PAL_FADE_FIELD ) ||
      ( model == BTLEFF_PAL_FADE_3D ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    ret_field = BTLV_FIELD_CheckExecutePaletteFade( bew->bfw );
  }
  if( ( model == BTLEFF_PAL_FADE_EFFECT ) ||
      ( model == BTLEFF_PAL_FADE_ALL ) )
  {
    ret_effect = ( PaletteFadeCheck( bew->pfd ) != 0 );
  }

  return ( ( ret_stage == TRUE ) || ( ret_field == TRUE ) || ( ret_effect == TRUE ) );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ3D���f���ɑ΂���o�j�b�V���t���O�Z�b�g
 *
 * @param[in] model  �ΏۂƂ���3D���f��
 * @param[in] flag   �Z�b�g����t���O
 */
//============================================================================================
void  BTLV_EFFECT_SetVanishFlag( int model, int flag )
{
  switch( model )
  {
  case BTLEFF_STAGE:
    BTLV_STAGE_SetVanishFlag( bew->bsw, BTLV_STAGE_MAX, flag );
    break;
  case BTLEFF_FIELD:
    BTLV_FIELD_SetVanishFlag( bew->bfw, flag );
    break;
  case BTLEFF_EFFECT:
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, flag ^ 1 );
    break;
  case BTLEFF_STAGE_MINE:
    BTLV_STAGE_SetVanishFlag( bew->bsw, BTLV_STAGE_MINE, flag );
    break;
  case BTLEFF_STAGE_ENEMY:
    BTLV_STAGE_SetVanishFlag( bew->bsw, BTLV_STAGE_ENEMY, flag );
    break;
  }
}

//============================================================================================
/**
 * @brief  ���[�e�[�V�����A�j���Z�b�g
 *
 * @param[in] dir   ���[�e�[�V�����̌���
 * @param[in] side  �����������葤��
 * @param[in] se_on SE���Đ����邩�ǂ����H(�^��X�L�b�v�΍�)
 */
//============================================================================================
void  BTLV_EFFECT_SetRotateEffect( BtlRotateDir dir, int side ,BOOL se_on )
{
  TCB_ROTATION *tr = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bew->heapID ), sizeof( TCB_ROTATION ) );
  GF_ASSERT( dir != BTL_ROTATEDIR_NONE )
  if( dir == BTL_ROTATEDIR_STAY )
  {
    return;
  }
  tr->seq_no = 0;
  tr->dir = ( dir == BTL_ROTATEDIR_L ) ? 0 : 1;
  tr->side = side;
  tr->se_on = se_on;

  bew->tcb_rotate_flag |= BTLV_EFFTOOL_Pos2Bit( side );

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bew->tcb_sys, TCB_BTLV_EFFECT_Rotation, tr, 0 ),
                      TCB_BTLV_EFFECT_Rotation_CB, GROUP_DEFAULT );
}

//============================================================================================
/**
 * @brief  �T�C�h�`�F���W�Z�b�g�i3vs3��Move������Łj
 *
 * @param[in] pos1  ����ւ���BtlvMcssPos
 * @param[in] pos2  ����ւ���BtlvMcssPos
 */
//============================================================================================
void  BTLV_EFFECT_SetSideChange( BtlvMcssPos pos1, BtlvMcssPos pos2 )
{
  BTLV_MCSS_SetSideChange( bew->bmw, pos1, pos2 );
}

//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu�̃g���[�i�[�C���f�b�N�X���擾
 *
 * @param[in] position  �擾����g���[�i�[�̗����ʒu
 */
//============================================================================================
int BTLV_EFFECT_GetTrainerIndex( int position )
{
  GF_ASSERT_MSG( ( position - BTLV_MCSS_POS_MAX ) >= 0, "position:%d\n", position );
  GF_ASSERT( ( position - BTLV_MCSS_POS_MAX ) < BTLV_MCSS_POS_MAX );
  GF_ASSERT( bew->trainer_index[ position - BTLV_MCSS_POS_MAX ] != BTLV_EFFECT_TRAINER_INDEX_NONE );

  return bew->trainer_index[ position - BTLV_MCSS_POS_MAX ];
}

//============================================================================================
/**
 * @brief  ���Ԑ����^�C�}�[�N���G�C�g
 *
 * @param[in] game_time     �Q�[�����ԁi�b�j
 * @param[in] command_time  �R�}���h���ԁi�b�j
 */
//============================================================================================
void  BTLV_EFFECT_CreateTimer( int game_time, int command_time )
{
  BTLV_TIMER_Create( bew->btw, game_time, command_time );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�`�拖��
 *
 *  @param[in]  type    �^�C�}�[�^�C�v
 *  @param[in]  enable  TRUE:�`��@FALSE:��`��
 *  @param[in]  init    TRUE:�J�E���^�����������ĕ`��@FALSE:�����������`��ienable��TRUE�̂Ƃ��ɂ����Ӗ�������܂���j
 *
 */
//============================================================================================
void  BTLV_EFFECT_DrawEnableTimer( BTLV_TIMER_TYPE type, BOOL enable, BOOL init )
{
  BTLV_TIMER_SetDrawEnable( bew->btw, type, enable, init );
}

//============================================================================================
/**
 *  @brief  �^�C�}�[�[���`�F�b�N
 *
 *  @param[in]  type    �^�C�}�[�^�C�v
 *
 */
//============================================================================================
BOOL  BTLV_EFFECT_IsZero( BTLV_TIMER_TYPE type )
{
  return BTLV_TIMER_IsZero( bew->btw, type );
}

//============================================================================================
/**
 *  @brief  �w�肳�ꂽ�����ʒu��MCSS�̃o�j�b�V���t���O���擾
 *
 *  @param[in]  position  �擾�����������ʒu
 */
//============================================================================================
BTLV_MCSS_VANISH_FLAG  BTLV_EFFECT_GetMcssVanishFlag( BtlvMcssPos position )
{
  return  BTLV_MCSS_GetVanishFlag( bew->bmw, position );
}

//============================================================================================
/**
 *  @brief  �w�肳�ꂽ�����ʒu��MCSS�Ƀo�j�b�V���t���O���Z�b�g
 *
 *  @param[in]  position  �Z�b�g�����������ʒu
 *  @param[in]  flag      �Z�b�g����t���O
 */
//============================================================================================
void    BTLV_EFFECT_SetMcssVanishFlag( BtlvMcssPos position, BTLV_MCSS_VANISH_FLAG flag )
{
  BTLV_MCSS_SetVanishFlag( bew->bmw, position, flag );
}

//============================================================================================
/**
 *  @brief  �w�肳�ꂽ�����ʒu�ɃJ�����̃t�H�[�J�X�����킹��
 *
 *  @param[in]  position  �Z�b�g�����������ʒu
 *  @param[in]  move_type �ړ��^�C�v  BTLEFF_CAMERA_MOVE_DIRECT�F�_�C���N�g BTLEFF_CAMERA_MOVE_INTERPOLATION�F�Ǐ]
 *  @param[in]  frame     �ړ��^�C�v�Ǐ]�̂Ƃ����t���[���ňړ����邩
 *  @param[in]  wait      �ړ��^�C�v�Ǐ]�̂Ƃ��P�t���[���ړ����閈�̃E�F�C�g
 *  @param[in]  brake     �ړ��^�C�v�Ǐ]�̂Ƃ����t���[���ڂŃu���[�L�������邩
 */
//============================================================================================
void    BTLV_EFFECT_SetCameraFocus( BtlvMcssPos position, int move_type, int frame, int wait, int brake )
{
  VecFx32 pos;
  VecFx32 target;

  GF_ASSERT( ( move_type == BTLEFF_CAMERA_MOVE_DIRECT ) || ( move_type == BTLEFF_CAMERA_MOVE_INTERPOLATION ) );

  if( position & 1 )
  {
    BTLV_EFFECT_GetCameraFocus( position, &pos, &target );

    switch( move_type ){
    case BTLEFF_CAMERA_MOVE_DIRECT:   //�_�C���N�g
      BTLV_CAMERA_MoveCameraPosition( bew->bcw, &pos, &target );
      break;
    case BTLEFF_CAMERA_MOVE_INTERPOLATION:  //�Ǐ]
      BTLV_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, frame, wait, brake );
      break;
    }
  }
  else
  {
    switch( bew->besp.rule ){
    case BTL_RULE_SINGLE:
      BTLV_EFFECT_Add( BTLEFF_1vs1_POS_AA_FOCUS );
      break;
    case BTL_RULE_DOUBLE:
      if( position == BTLV_MCSS_POS_A )
      {
        BTLV_EFFECT_Add( BTLEFF_2vs2_POS_A_FOCUS );
      }
      else
      {
        BTLV_EFFECT_Add( BTLEFF_2vs2_POS_C_FOCUS );
      }
      break;
    case BTL_RULE_TRIPLE:
      if( position == BTLV_MCSS_POS_A )
      {
        BTLV_EFFECT_Add( BTLEFF_3vs3_POS_A_FOCUS );
      }
      else if( position == BTLV_MCSS_POS_C )
      {
        BTLV_EFFECT_Add( BTLEFF_1vs1_POS_AA_FOCUS );
      }
      else
      {
        BTLV_EFFECT_Add( BTLEFF_3vs3_POS_E_FOCUS );
      }
      break;
    case BTL_RULE_ROTATION:
      BTLV_EFFECT_Add( BTLEFF_ROTATION_POS_A_FOCUS );
      break;
    }
  }
}

//============================================================================================
/**
 *  @brief  �w�肳�ꂽ�����ʒu�̃J�����̃t�H�[�J�X�ʒu���擾
 *
 *  @param[in]    position  �Z�b�g�����������ʒu
 *  @param[out]   pos       �擾����J�����|�W�V����
 *  @param[out]   target    �擾����J�����^�[�Q�b�g
 */
//============================================================================================
void    BTLV_EFFECT_GetCameraFocus( BtlvMcssPos position, VecFx32* pos, VecFx32* target )
{
  BTLV_MCSS_GetPokeDefaultPos( bew->bmw, target, position );

#ifndef  CAMERA_FOCUS
  pos->x = target->x + BTLV_EFFECT_FOCUS_OFFSET_X;
  pos->y = target->y + BTLV_EFFECT_FOCUS_OFFSET_Y;
  pos->z = target->z + BTLV_EFFECT_FOCUS_OFFSET_Z;
  target->y += BTLV_EFFECT_FOCUS_TARGET_Y;
#else
  pos->x = target->x + camera_focus_offset_x;
  pos->y = target->y + camera_focus_offset_y;
  pos->z = target->z + camera_focus_offset_z;
  target->y += camera_focus_target_y;
#endif
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bew �J�����Ǘ��\����
 */
//============================================================================================
BTLV_EFFECT_WORK* BTLV_EFFECT_GetEffectWork( void )
{
  return bew;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���J�����Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bcw �J�����Ǘ��\����
 */
//============================================================================================
BTLV_CAMERA_WORK  *BTLV_EFFECT_GetCameraWork( void )
{
  return bew->bcw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���MCSS�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bmw MCSS�Ǘ��\����
 */
//============================================================================================
BTLV_MCSS_WORK  *BTLV_EFFECT_GetMcssWork( void )
{
  return bew->bmw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���Stage�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bsw Stage�Ǘ��\����
 */
//============================================================================================
BTLV_STAGE_WORK  *BTLV_EFFECT_GetStageWork( void )
{
  return bew->bsw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���Gauge�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bgw Gauge�Ǘ��\����
 */
//============================================================================================
BTLV_GAUGE_WORK  *BTLV_EFFECT_GetGaugeWork( void )
{
  return bew->bgw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���VMHANDLE�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
VMHANDLE  *BTLV_EFFECT_GetVMHandle( void )
{
  return bew->vm_core;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���GFL_TCBSYS�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
GFL_TCBSYS  *BTLV_EFFECT_GetTCBSYS( void )
{
  return bew->tcb_sys;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���PALETTE_FADE_PTR�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval vm_core VMHANDLE�Ǘ��\����
 */
//============================================================================================
PALETTE_FADE_PTR  BTLV_EFFECT_GetPfd( void )
{
  return bew->pfd;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���CLWK�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bclw CLWK�Ǘ��\����
 */
//============================================================================================
BTLV_CLACT_WORK *BTLV_EFFECT_GetCLWK( void )
{
  return bew->bclw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă���BTLV_BG_WORK�Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bbw BTLV_BG_WORK�Ǘ��\����
 */
//============================================================================================
BTLV_BG_WORK *BTLV_EFFECT_GetBGWork( void )
{
  return bew->bbw;
}

//============================================================================================
/**
 * @brief  �G�t�F�N�g�Ŏg�p����Ă��鎞�Ԑ����Ǘ��\���̂̃|�C���^���擾
 *
 * @retval bbw BTLV_TIMER_WORK�Ǘ��\����
 */
//============================================================================================
BTLV_TIMER_WORK*  BTLV_EFFECT_GetTimerWork( void )
{
  return bew->btw;
}

//============================================================================================
/**
 * @brief  �o�g�����[�����擾
 *
 * @retval BtlRule
 */
//============================================================================================
BtlRule BTLV_EFFECT_GetBtlRule( void )
{
  return bew->besp.rule;
}

//============================================================================================
/**
 * @brief  �o�g���R���y�`�^�[���擾
 *
 * @retval BtlCompetitor
 */
//============================================================================================
BtlCompetitor BTLV_EFFECT_GetBtlCompetitor( void )
{
  return bew->besp.competitor;
}

//============================================================================================
/**
 * @brief  �}���`���ǂ������擾
 *
 * @retval TRUE:�}���`  FALSE:�}���`�ȊO
 */
//============================================================================================
BOOL BTLV_EFFECT_GetMulti( void )
{
  return bew->besp.multi;
}

//============================================================================================
/**
 * @brief  �����ʒu���w�肵��TRTYPE���擾
 *
 * @param[in] pos �����ʒu
 *
 * @retval TRTYPE_
 */
//============================================================================================
int BTLV_EFFECT_GetTrType( int pos )
{
  GF_ASSERT( pos < 4 );
  return bew->besp.tr_type[ pos ];
}

//============================================================================================
/**
 * @brief  mainModule���擾
 *
 * @retval mainModule
 */
//============================================================================================
const BTL_MAIN_MODULE* BTLV_EFFECT_GetMainModule( void )
{
  return bew->besp.mainModule;
}

//============================================================================================
/**
 * @brief  scu���擾
 *
 * @retval scu
 */
//============================================================================================
const BTLV_SCU* BTLV_EFFECT_GetScu( void )
{
  return bew->besp.scu;
}

//============================================================================================
/**
 * @brief  �s���`BGM�t���O���擾
 *
 * @retval 0:�s���`BGM����Ȃ��@1:�s���`BGM
 */
//============================================================================================
int BTLV_EFFECT_GetPinchBGMFlag( void )
{
  return BTLV_GAUGE_GetPinchBGMFlag( bew->bgw );
}

//============================================================================================
/**
 * @brief  �s���`BGM�����Ă��邩�`�F�b�N������BGM��ω�������
 *
 * @retval 0:�s���`BGM����Ȃ��@1:�s���`BGM
 */
//============================================================================================
void BTLV_EFFECT_SwitchBGM( const int bgmNo )
{
  //�s���`BGM�����Ă����Ȃ�Pop���Ă���
  if( BTLV_GAUGE_GetPinchBGMFlag( bew->bgw ) )
  { 
    BTLV_GAUGE_SetPinchBGMFlag( bew->bgw, FALSE );
    PMSND_PopBGM();
  }
  //�s���`BGM�`�F�b�N�͂��Ȃ��悤�ɂ���
  BTLV_GAUGE_SetPinchBGMNoCheck( bew->bgw, TRUE );
  PMSND_PlayBGM( bgmNo );
}

//============================================================================================
/**
 * @brief  �s���`BGM�m�[�`�F�b�N�t���O���Z�b�g
 */
//============================================================================================
void BTLV_EFFECT_SetPinchBGMNoCheck( BOOL flag )
{ 
  BTLV_GAUGE_SetPinchBGMNoCheck( bew->bgw, flag );
}

//============================================================================================
/**
 * @brief  �g���[�i�[�ȃ`�F���W�t���O���Z�b�g
 *
 * @param[in] bgm_no    �`�F���W����g���[�i�[��
 */
//============================================================================================
void  BTLV_EFFECT_SetTrainerBGMChangeFlag( int bgm_no )
{
  bew->trainer_bgm_change_flag = TRUE;

  BTLV_GAUGE_SetNowBGMNo( bew->bgw, bgm_no );

  if( BTLV_GAUGE_GetPinchBGMFlag( bew->bgw ) == 0 )
  {
    BTLV_GAUGE_SetTrainerBGMChangeFlag( bew->bgw, 1 );
  }
}

//============================================================================================
/**
 * @brief  �g���[�i�[�ȃ`�F���W�t���O���Q�b�g
 */
//============================================================================================
BOOL  BTLV_EFFECT_GetTrainerBGMChangeFlag( void )
{
  return bew->trainer_bgm_change_flag;
}

//============================================================================================
/**
 * @brief  BGM��Push��Ԃ��`�F�b�N���ċȂ��Z�b�g
 */
//============================================================================================
void  BTLV_EFFECT_SetBGMNoCheckPush( int bgm_no )
{
  //�s���`SE���Ȃ��Ă���Ȃ�A�Ȃ��v�b�V������Ă���̂ŁAPop����
  if( BTLV_GAUGE_GetPinchBGMFlag( bew->bgw ) == TRUE )
  {
    PMSND_PopBGM();
  }
  BTLV_GAUGE_SetPinchBGMNoCheck( bew->bgw, TRUE );
  BTLV_GAUGE_SetPinchBGMFlag( bew->bgw, FALSE );
  PMSND_PlayBGM( bgm_no );
}

//============================================================================================
/**
 * @brief  �o�b�O���[�h���Z�b�g
 */
//============================================================================================
void  BTLV_EFFECT_SetBagMode( BtlBagMode bagMode )
{
  bew->bagMode = bagMode;
}

//============================================================================================
/**
 * @brief  �o�b�O���[�h���Q�b�g
 */
//============================================================================================
BtlBagMode  BTLV_EFFECT_GetBagMode( void )
{
  return bew->bagMode;
}

//============================================================================================
/**
 * @brief  SE���[�h���Z�b�g
 */
//============================================================================================
void  BTLV_EFFECT_SetSEMode( BTLV_EFFECT_SE_MODE mode )
{
  bew->se_mode = mode;
}

//============================================================================================
/**
 * @brief  SE���[�h���Q�b�g
 */
//============================================================================================
BTLV_EFFECT_SE_MODE  BTLV_EFFECT_GetSEMode( void )
{
  return bew->se_mode;
}

//============================================================================================
/**
 * @brief �V���[�^�[���g�p�ł��邩�`�F�b�N
 *
 * @retval  TRUE:�g�p��
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckShooterEnable( void )
{
  BOOL  ret = FALSE;

  if( bew->besp.mainModule )
  {
    ret = BTL_MAIN_IsShooterEnable( bew->besp.mainModule );
  }

  return ret;
}

//============================================================================================
/**
 * @brief �A�C�e���{�^���i�o�b�O�^�V���[�^�[�j���g�p�ł��邩�`�F�b�N
 *
 * @retval  TRUE:�g�p��
 */
//============================================================================================
BOOL  BTLV_EFFECT_CheckItemEnable( void )
{
  BOOL  ret = FALSE;

  if( bew->besp.mainModule )
  {
    ret = BTL_MAIN_IsItemEnable( bew->besp.mainModule );
  }

  return ret;
}

//============================================================================================
/**
 * @brief MCSS��ReverseDrawFlag���Z�b�g
 *
 * @param[in] draw_flag �Z�b�g����t���O
 */
//============================================================================================
void  BTLV_EFFECT_SetReverseDrawFlag( BTLV_EFFECT_REVERSE_DRAW draw_flag )
{
  BtlvMcssPos pos;

  for( pos = BTLV_MCSS_POS_AA ; pos < BTLV_MCSS_POS_MAX ; pos++ )
  {
    if( BTLV_MCSS_CheckExist( bew->bmw, pos ) )
    {
      BTLV_MCSS_SetReverseDrawFlag( bew->bmw, pos, draw_flag );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �󂢂Ă���TCBIndex���擾����GFL_TCB���Z�b�g
 *
 *  @param[in]  tcb       �Z�b�g����GFL_TCB
 *  @param[in]  callback  ��������Ƃ��ɌĂяo�����R�[���o�b�N�֐�
 *  @param[in]  group     �O���[�v�w��
 */
//-----------------------------------------------------------------------------
void   BTLV_EFFECT_SetTCB( GFL_TCB* tcb, BTLV_EFFECT_TCB_CALLBACK_FUNC* callback_func, BTLV_EFFECT_TCB_GROUP group )
{
  int index = BTLV_EFFECT_GetTCBIndex();

  bew->tcb[ index ] = tcb;
  bew->tcb_callback[ index ] = callback_func;
  bew->tcb_group[ index ] = group;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �w�肳�ꂽGFL_TCB��TCBIndex���擾
 *
 *  @param[in]  tcb �擾����GFL_TCB
 */
//-----------------------------------------------------------------------------
int   BTLV_EFFECT_SearchTCBIndex( GFL_TCB* tcb )
{
  int i;

  for( i = 0 ; i < BTLV_EFFECT_TCB_MAX ; i++ )
  {
    if( bew->tcb[ i ] == tcb )
    {
      break;
    }
  }

  GF_ASSERT( i != BTLV_EFFECT_TCB_MAX );

  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������Ă��Ȃ�TCB�����
 *
 *  @param[in]  tcb �������TCB
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_FreeTCB( GFL_TCB* tcb )
{
  int index = BTLV_EFFECT_SearchTCBIndex( tcb );
  if( tcb )
  {
    void* work = GFL_TCB_GetWork( tcb );
    if( bew->tcb_callback[ index ] )
    {
      bew->tcb_callback[ index ]( tcb );
    }
    if( work )
    {
      GFL_HEAP_FreeMemory( work );
    }
    GFL_TCB_DeleteTask( tcb );
    bew->tcb[ index ] = NULL;
    bew->tcb_callback[ index ] = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �O���[�v���w�肵��TCB�����
 *
 *  @param[in]  group �������TCB�O���[�v
 *
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_FreeTCBGroup( BTLV_EFFECT_TCB_GROUP group )
{
  int i;

  for( i = 0 ; i < BTLV_EFFECT_TCB_MAX ; i++ )
  {
    if( ( bew->tcb[ i ] ) && ( bew->tcb_group[ i ] == group ) )
    {
      BTLV_EFFECT_FreeTCB( bew->tcb[ i ] );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�������[�N�G�t�F�N�g�N���t���OON
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_SetCameraWorkExecute( BTLV_EFFECT_CWE cwe )
{
#if 0
  //WCS�ŃJ�������[�N�I�t�Ȃ牽�����Ȃ�
  if( ( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_WCS ) ) &&
      ( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_OFF ) ) )
  {
    return;
  }
#endif
  bew->camera_work_execute = cwe;
  if( bew->camera_work_execute != BTLV_EFFECT_CWE_NO_STOP )
  {
    bew->camera_work_seq  = 0;
    bew->camera_work_wait = 0;
  }
  if( cwe == BTLV_EFFECT_CWE_COMM_WAIT )
  { 
    BTLV_MCSS_PushVanishFlag( bew->bmw );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�������[�N�G�t�F�N�g���~�߂�
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_SetCameraWorkStop( void )
{
  BTLV_EFFECT_Stop();
  if( ( bew->camera_work_execute == BTLV_EFFECT_CWE_COMM_WAIT ) &&
      ( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_WCS ) ) )
  {
    BTLV_EFFECT_Add( BTLEFF_CAMERA_WORK_WCS_INIT );
  }
  else
  {
    BTLV_EFFECT_Add( BTLEFF_CAMERA_INIT );
  }
  bew->camera_work_execute = BTLV_EFFECT_CWE_NONE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �J�������[�N�G�t�F�N�g���~�߂Ď��s�^�C�v��ω�
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_SetCameraWorkSwitch( BTLV_EFFECT_CWE type )
{
  BTLV_EFFECT_Stop();
  bew->camera_work_execute = type;
  bew->camera_work_seq  = 0;
  bew->camera_work_wait = 0;

  //���̂ɂ���Ă̓J���������ʒu���Ă�
  /*
  if( bew->camera_work_execute == BTLV_EFFECT_CWE_SHIFT_NONE )
  {
    BTLV_EFFECT_Add( BTLEFF_CAMERA_INIT );
  }
  else
  {
    BTLV_EFFECT_Add( BTLEFF_CAMERA_WORK_INIT );
  }
  */
}

//----------------------------------------------------------------------------
/**
 *  @brief  �t�B�[���h�A�j���[�V������1�t���[���~�߂�
 *  �e�N�X�`���]�����̔w�i�̃p�V�����y��
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_SetFieldAnmStopOnce( void )
{ 
  BTLV_FIELD_SetAnmStopOnce( bew->bfw );
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���������������L������
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_SetLookTokusei( BtlvMcssPos pos, int tokusei )
{ 
  bew->tokusei[ pos ] = tokusei;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���������������擾����
 */
//-----------------------------------------------------------------------------
int  BTLV_EFFECT_GetLookTokusei( BtlvMcssPos pos )
{ 
  return bew->tokusei[ pos ];
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����Ă���|�P�����̖������~�߂�
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_StopAllPMVoice( void )
{ 
  BTLV_EFFVM_StopAllPMVoice( bew->vm_core );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �X�e�[�^�X�㏸���~�G�t�F�N�g�̘A���N���}�����N���A
 */
//-----------------------------------------------------------------------------
void  BTLV_EFFECT_ClearOldStatusEffectBuffer( void )
{ 
  BTLV_EFFVM_ClearOldStatusEffectBuffer( bew->vm_core );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �󂢂Ă���TCBIndex���擾
 *
 *  @param[in]  bevw �V�X�e���Ǘ��\����
 */
//-----------------------------------------------------------------------------
static  int   BTLV_EFFECT_GetTCBIndex( void )
{
  int i;

  for( i = 0 ; i < BTLV_EFFECT_TCB_MAX ; i++ )
  {
    if( bew->tcb[ i ] == NULL )
    {
      break;
    }
  }

  GF_ASSERT( i != BTLV_EFFECT_TCB_MAX );
  if( i == BTLV_EFFECT_TCB_MAX )
  {
    BTLV_EFFECT_FreeTCB( bew->tcb[ 0 ] );
    i = 0;
  }

  return i;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������Ă��Ȃ����ׂĂ�TCB�����
 *
 *  @param[in]  bevw �V�X�e���Ǘ��\����
 */
//-----------------------------------------------------------------------------
static  void  BTLV_EFFECT_FreeTCBAll( void )
{
  int i;

  for( i = 0 ; i < BTLV_EFFECT_TCB_MAX ; i++ )
  {
    if( bew->tcb[ i ] )
    {
      BTLV_EFFECT_FreeTCB( bew->tcb[ i ] );
    }
  }
}

//============================================================================================
/**
 *  @brief  VBlank�֐�
 */
//============================================================================================
static  void  BTLV_EFFECT_VBlank( GFL_TCB *tcb, void *work )
{
  GFL_CLACT_SYS_VBlankFunc();
  PaletteFadeTrans( bew->pfd );
}

//============================================================================================
/**
 *  @brief  �_���[�W�G�t�F�N�g�V�[�P���X�i���d�N��������̂�TCB�ō쐬�j
 */
//============================================================================================
static  void  TCB_BTLV_EFFECT_Damage( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_DAMAGE_TCB *bedt = (BTLV_EFFECT_DAMAGE_TCB*)work;

  if( bedt->wait ){
    bedt->wait--;
    return;
  }
  switch( bedt->seq_no ){
  case 0:
    if( bedt->vanish == FALSE )
    {
      BTLV_MCSS_SetPaletteFade( bew->bmw, bedt->target, 16, 16, 0, bedt->color );
    }
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    bedt->seq_no = 1;
    break;
  case 1:
    if( bedt->vanish == FALSE )
    {
      BTLV_MCSS_SetVanishFlag( bew->bmw, bedt->target, BTLV_MCSS_VANISH_ON );
    }
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    bedt->seq_no = 2;
    break;
  case 2:
    bedt->seq_no = 0;
    if( bedt->vanish == FALSE )
    {
      BTLV_MCSS_SetVanishFlag( bew->bmw, bedt->target, BTLV_MCSS_VANISH_OFF );
      BTLV_MCSS_SetPaletteFade( bew->bmw, bedt->target, 0, 0, 0, 0x7fff );
    }
    bedt->wait = BTLV_EFFECT_BLINK_WAIT;
    if( --bedt->work == 0 ){
      BTLV_EFFECT_FreeTCB( tcb );
    }
    break;
  }
}

static  void  TCB_BTLV_EFFECT_Damage_CB( GFL_TCB *tcb )
{
  BTLV_EFFECT_DAMAGE_TCB *bedt = (BTLV_EFFECT_DAMAGE_TCB*)GFL_TCB_GetWork( tcb );

  bew->tcb_damage_flag &= ( BTLV_EFFTOOL_Pos2Bit( bedt->target ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 *  @brief  �ω��G�t�F�N�g�V�[�P���X�i���d�N��������̂�TCB�ō쐬�j
 */
//============================================================================================
static  void  TCB_BTLV_EFFECT_Henge( GFL_TCB *tcb, void *work )
{
  BTLV_EFFECT_HENGE_TCB *beht = (BTLV_EFFECT_HENGE_TCB*)work;

  switch( beht->seq_no ){
  case 0:
    BTLV_EFFVM_Start( bew->vm_core, beht->vpos, BTLV_MCSS_POS_ERROR, BTLEFF_ZOOM_IN_ORTHO, NULL );
    bew->execute_flag = TRUE;
    beht->seq_no++;
    break;
  case 1:
    if( bew->execute_flag == FALSE )
    {
      if( BTLV_MCSS_GetStatusFlag( bew->bmw, beht->vpos ) & BTLV_MCSS_STATUS_FLAG_MIGAWARI )
      {
        BTLV_EFFVM_Start( bew->vm_core, beht->vpos, BTLV_MCSS_POS_ERROR, BTLEFF_MIGAWARI_WAZA_BEFORE, NULL );
        bew->execute_flag = TRUE;
      }
      beht->seq_no++;
    }
    break;
  case 2:
    if( bew->execute_flag == FALSE )
    {
      BTLV_MCSS_MoveAlpha( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_DIRECT, 16, 0, 0, 0 );
      BTLV_MCSS_MoveMosaic( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_INTERPOLATION, 8, 8, 1, 0 );
      BTLV_EFFVM_SePlay( bew->vm_core, SEQ_SE_W048_01, BTLEFF_SEPLAY_SE1, BTLEFF_SEPAN_ATTACK, -500, 127, 0, 0, 0 );
      BTLV_EFFVM_SeEffect( bew->vm_core, BTLEFF_SEPLAY_SE1, BTLEFF_SEEFFECT_INTERPOLATION, BTLEFF_SEEFFECT_VOLUME,
                           127, 0, 30, 10, 0, 0 );
      beht->seq_no++;
    }
    break;
  case 3:
    if( !BTLV_MCSS_CheckTCBExecute( bew->bmw, beht->vpos ) )
    {
      BTLV_MCSS_OverwriteMAW( bew->bmw, beht->vpos, &beht->maw );
      beht->seq_no++;
    }
    break;
  case 4:
    BTLV_MCSS_MoveMosaic( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_INTERPOLATION, 0, 8, 1, 0 );
    beht->seq_no++;
    break;
  case 5:
    if( !BTLV_MCSS_CheckTCBExecute( bew->bmw, beht->vpos ) )
    {
      BTLV_EFFVM_SePlay( bew->vm_core, SEQ_SE_DUMMY5, BTLEFF_SEPLAY_SE1, BTLEFF_SEPAN_ATTACK, 0, 0, 0, 0, 0 );
      BTLV_MCSS_MoveAlpha( bew->bmw, beht->vpos, EFFTOOL_CALCTYPE_DIRECT, 31, 0, 0, 0 );
      if( BTLV_MCSS_GetStatusFlag( bew->bmw, beht->vpos ) & BTLV_MCSS_STATUS_FLAG_MIGAWARI )
      {
        BTLV_EFFVM_Start( bew->vm_core, beht->vpos, BTLV_MCSS_POS_ERROR, BTLEFF_MIGAWARI_WAZA_AFTER, NULL );
        bew->execute_flag = TRUE;
      }
      beht->seq_no++;
    }
    break;
  case 6:
    if( bew->execute_flag == FALSE )
    {
      BTLV_EFFVM_Start( bew->vm_core, beht->vpos, BTLV_MCSS_POS_ERROR, BTLEFF_CAMERA_INIT_ORTHO, NULL );
      bew->execute_flag = TRUE;
      beht->seq_no++;
    }
    break;
  case 7:
    if( bew->execute_flag == FALSE )
    {
      BTLV_EFFECT_FreeTCB( tcb );
    }
    break;
  }
}

static  void  TCB_BTLV_EFFECT_Henge_CB( GFL_TCB *tcb )
{
  BTLV_EFFECT_HENGE_TCB *beht = (BTLV_EFFECT_HENGE_TCB*)GFL_TCB_GetWork( tcb );

  bew->tcb_henge_flag &= ( BTLV_EFFTOOL_Pos2Bit( beht->vpos ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 *  @brief  ���[�e�[�V�����A�j��
 */
//============================================================================================
static  void  TCB_BTLV_EFFECT_Rotation( GFL_TCB *tcb, void *work )
{
  TCB_ROTATION *tr = ( TCB_ROTATION* )work;

  switch( tr->seq_no ){
  case 0:
    if( ( bew->execute_flag | bew->tcb_damage_flag | bew->tcb_henge_flag ) != 0 )
    { 
      break;
    }
    tr->seq_no++;
    /*fallthry*/
  case 1:
    BTLV_MCSS_SetRotation( bew->bmw, tr->side, tr->dir, tr->se_on );
    tr->seq_no++;
    break;
  case 2:
    if( ( BTLV_STAGE_CheckExecuteAnmReq( bew->bsw ) == FALSE ) &&
        ( BTLV_MCSS_CheckTCBExecuteAllPos( bew->bmw ) == FALSE ) )
    {
      BTLV_EFFECT_FreeTCB( tcb );
    }
    break;
  }
}

static  void  TCB_BTLV_EFFECT_Rotation_CB( GFL_TCB *tcb )
{
  TCB_ROTATION *tr = ( TCB_ROTATION* )GFL_TCB_GetWork( tcb );

  bew->tcb_rotate_flag &= ( BTLV_EFFTOOL_Pos2Bit( tr->side ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
}

//============================================================================================
/**
 *  @brief  �Z�G�t�F�N�g�N��
 */
//============================================================================================
static  void  TCB_BTLV_EFFECT_Start( GFL_TCB *tcb, void *work )
{
  TCB_EFFECT_START *tes = ( TCB_EFFECT_START* )work;

  if( !BTLV_EFFECT_CheckExecute() )
  {
    BTLV_EFFVM_Start( bew->vm_core, tes->from, tes->to, tes->eff_no, tes->param );
    bew->execute_flag = TRUE;
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_BTLV_EFFECT_Start_CB( GFL_TCB *tcb )
{
  TCB_EFFECT_START *tes = ( TCB_EFFECT_START* )GFL_TCB_GetWork( tcb );

  if( tes->param )
  {
    GFL_HEAP_FreeMemory( tes->param );
  }
}

//============================================================================================
/**
 * @brief  �J�������[�N�`�F�b�N
 */
//============================================================================================
static  void  camera_work_check( void )
{
  int trg = GFL_UI_KEY_GetTrg();
  int tp = GFL_UI_TP_GetTrg();

  if( bew->camera_work_execute == BTLV_EFFECT_CWE_NONE ) return;

  if( ( bew->camera_work_execute != BTLV_EFFECT_CWE_NO_STOP ) &&
      ( bew->camera_work_execute != BTLV_EFFECT_CWE_COMM_WAIT ) )
  {
    if( ( trg ) || ( tp ) )
    {
      BTLV_EFFECT_Stop();
      BTLV_EFFECT_Add( BTLEFF_CAMERA_WORK_INIT );
      bew->camera_work_seq = 0;
      bew->camera_work_wait = 0;
    }
  }

  switch( bew->camera_work_seq ){
  case 0:
    if( bew->camera_work_wait < bew->camera_work_wait_tmp )
    {
      bew->camera_work_wait++;
      break;
    }
    bew->camera_work_seq = 1;
    /*fallthru*/
  case 1:
    if( !BTLV_EFFECT_CheckExecute() )
    {
      static  int camera_work_num[ 6 ] = {
        BTLEFF_CAMERA_WORK,
        BTLEFF_CAMERA_WORK_ROTATE_R_L,
        BTLEFF_CAMERA_WORK_ROTATE_L_R,
        BTLEFF_CAMERA_WORK_UPDOWN_E_M,
        BTLEFF_CAMERA_WORK_UPDOWN_M_E,
        BTLEFF_CAMERA_WORK_TRIANGLE,
      };
      static  int wcs_camera_work_num[ 2 ] = {
        BTLEFF_WCS_CAMERA_WORK_E_M,
        BTLEFF_WCS_CAMERA_WORK_M_E,
      };
      int eff_no;

      if( ( bew->camera_work_execute == BTLV_EFFECT_CWE_COMM_WAIT ) &&
          ( BTL_MAIN_GetSetupStatusFlag( bew->besp.mainModule, BTL_STATUS_FLAG_CAMERA_WCS ) ) )
      {
        while( 1 )
        {
          int no = GFL_STD_MtRand( NELEMS( wcs_camera_work_num ) );
          if( bew->camera_work_num != no )
          {
            bew->camera_work_num = no;
            break;
          }
        }
        eff_no = wcs_camera_work_num[ bew->camera_work_num ];
      }
      else
      {
        while( 1 )
        {
          int no = GFL_STD_MtRand( NELEMS( camera_work_num ) );
          if( bew->camera_work_num != no )
          {
            bew->camera_work_num = no;
            break;
          }
        }
        eff_no = camera_work_num[ bew->camera_work_num ];
      }
      BTLV_EFFECT_Add( eff_no );
      bew->execute_flag = TRUE;
    }
    break;
  }
}

#ifdef PM_DEBUG
//============================================================================================
/**
 * @brief  �w�肳�ꂽ�����ʒu�Ƀ|�P�������Z�b�g
 *
 * @param[in] pp      �Z�b�g����|�P������POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in] position  �Z�b�g����|�P�����̗����ʒu
 */
//============================================================================================
void  BTLV_EFFECT_SetPokemonDebug( const MCSS_ADD_DEBUG_WORK *madw, int position )
{
  BTLV_MCSS_AddDebug( bew->bmw, madw, position );
}

//============================================================================================
/**
 * @brief  �o�g�����[�����Z�b�g�i�f�o�b�O�p�j
 *
 * @param[in] rule  �Z�b�gBtlRule
 */
//============================================================================================
void  BTLV_EFFECT_SetBtlRule( BtlRule rule )
{
  bew->besp.rule = rule;
}

#endif
