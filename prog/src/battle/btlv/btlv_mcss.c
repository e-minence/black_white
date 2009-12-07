
//============================================================================================
/**
 * @file  btlv_mcss.c
 * @brief �퓬MCSS�Ǘ�
 * @author  soga
 * @date  2008.11.14
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/mcss_tool.h"

#include "btlv_effect.h"

#include "battle/battgra_wb.naix"
#include "pokegra/pokegra_wb.naix"

#include "data/patch.cdat"

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

#define BTLV_MCSS_STOP_ANIME_TIME ( 64 )

enum{
  REVERSE_FLAG_OFF = 0,
  REVERSE_FLAG_ON,
  ANIME_STOP_FLAG = 0x80000000,
  ANIME_START_FLAG = 0x00000000,
  SEQ_ANIME_STANDBY = 0,
  SEQ_ANIME_STOP,
};

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

struct _BTLV_MCSS_WORK
{
  GFL_TCBSYS*     tcb_sys;
  MCSS_SYS_WORK*  mcss_sys;
  MCSS_WORK*      mcss[ BTLV_MCSS_MAX ];
  GFL_TCB*        tcb[ BTLV_MCSS_MAX ];

  u32             mcss_pos_3vs3   :1;
  u32             mcss_pos_rotate :1;
  u32             mcss_proj_mode  :1;
  u32                             :29;
  u32             mcss_tcb_move_execute;
  u32             mcss_tcb_scale_execute;
  u32             mcss_tcb_rotate_execute;

  u32             mcss_tcb_blink_execute;
  u32             mcss_tcb_alpha_execute;
  u32             callback_work;
  HEAPID          heapID;
};

typedef struct
{
  BTLV_MCSS_WORK*   bmw;
  int               position;
  EFFTOOL_MOVE_WORK emw;
}BTLV_MCSS_TCB_WORK;

typedef struct
{ 
  int seq_no;
  int position;
  int wait;
}BTLV_MCSS_STOP_ANIME;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

static  void  BTLV_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK* maw, int position );
static  void  BTLV_MCSS_MakeMAWTrainer( int tr_type, MCSS_ADD_WORK* maw, int position );
static  void  BTLV_MCSS_SetDefaultScale( BTLV_MCSS_WORK *bmw, int position );

static  void  BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end,
                                       int frame, int wait, int count, GFL_TCB_FUNC *func, int reverse_flag );
static  void  TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_Alpha( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_MoveCircle( GFL_TCB *tcb, void *work );
static  void  TCB_BTLV_MCSS_StopAnime( GFL_TCB *tcb, void *work );

static  void  BTLV_MCSS_CallBackFunctorFrame( u32 data, fx32 currentFrame );
static  BOOL  BTLV_MCSS_CallBackNodes( u32 data, const NNSG2dMultiCellHierarchyData* pNodeData,
                                       NNSG2dCellAnimation* pCellAnim, u16 nodeIdx );

static  void  BTLV_MCSS_GetDefaultPos( BTLV_MCSS_WORK *bmw, VecFx32 *pos, BtlvMcssPos position );
static  fx32  BTLV_MCSS_GetDefaultScale( BTLV_MCSS_WORK* bmw, BtlvMcssPos position, BTLV_MCSS_PROJECTION proj );

static  BOOL  BTLV_MCSS_MakeBuchi( const MCSS_ADD_WORK* maw, TCB_LOADRESOURCE_WORK* tlw, void* work );

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
};

static  const VecFx32 poke_pos_rotate_table[]={
  { 0xffffe500, 0x00000666, 0x00007000 }, //POS_A
  { 0x000026cd, 0x00000666, 0xffff5a00 }, //POS_B
  { 0x00002800, 0x00000666, 0x00007000 }, //POS_C
  { 0xffffdccd, 0x00000666, 0xffff4700 }, //POS_D
  { 0x00000800, 0x00000666, 0x0000a000 }, //POS_E
  { 0x000004cd, 0x00000666, 0xffff3000 }, //POS_F
};

static  const VecFx32 trainer_pos_table[]={
  { FX_F32_TO_FX32( -2.5f + 3.000f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   7.5f - 0.5f ) },    //POS_TR_AA
  { FX_F32_TO_FX32(  4.5f - 4.200f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( -25.0f ) },           //POS_TR_BB
  { FX_F32_TO_FX32( -3.5f + 3.500f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   8.5f ) },           //POS_TR_A
  { FX_F32_TO_FX32(  6.0f - 4.200f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(  -9.0f ) },           //POS_TR_B
  { FX_F32_TO_FX32( -0.5f + 3.845f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   9.0f ) },           //POS_TR_C
  { FX_F32_TO_FX32(  2.0f - 4.964f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( -11.0f ) },           //POS_TR_D
};

//============================================================================================
/**
 *  �|�P�����̗����ʒu�ɂ��X�P�[���␳�e�[�u��
 */
//============================================================================================
static  const fx32 poke_scale_single_table[]={
  0x1030,   //POS_AA
  0x119b,   //POS_BB
};

static  const fx32 poke_scale_double_table[]={
  0x000010f0, //POS_A
  0x00001205, //POS_B
  0x00000f00, //POS_C
  0x00001320, //POS_D
};

static  const fx32 poke_scale_triple_table[]={
  0x00001200, //POS_A
  0x00001200, //POS_B
  0x00001270, //POS_C
  0x000011b0, //POS_D
  0x00000ed4, //POS_E
  0x00001400, //POS_F
};

static  const fx32 poke_scale_rotate_table[]={
  0x000010f0, //POS_A
  0x00001205, //POS_B
  0x00000f00, //POS_C
  0x00001320, //POS_D
  0x00000e00, //POS_E
  0x000010f0, //POS_F
};

static  const fx32 trainer_scale_table[]={
  0x1030,   //POS_TR_AA
  0x119b,   //POS_TR_BB
  0x0f00,   //POS_TR_A
  0x10e0,   //POS_TR_B
  0x0d00,   //POS_TR_C
  0x1320,   //POS_TR_D
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

  bmw->mcss_sys = MCSS_Init( BTLV_MCSS_MAX, heapID );
  bmw->tcb_sys  = tcb_sys;

  bmw->mcss_pos_3vs3 = ( rule == BTL_RULE_TRIPLE ) ? 1 : 0;
  bmw->mcss_pos_rotate = ( rule == BTL_RULE_ROTATION ) ? 1 : 0;

  MCSS_SetCallBackFunc( bmw->mcss_sys, BTLV_MCSS_MakeBuchi, &bmw->callback_work );

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
  MCSS_ADD_WORK maw;
  VecFx32 pos;

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT_MSG( bmw->mcss[ position ] == NULL, "pos=%d", position );

  //���������R�[���o�b�N���[�N�ɑ�����Ă���
  bmw->callback_work = PP_Get( pp, ID_PARA_personal_rnd, NULL );

  BTLV_MCSS_MakeMAW( pp, &maw, position );
  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->mcss[ position ] = MCSS_Add( bmw->mcss_sys, pos.x, pos.y, pos.z, &maw );

  BTLV_MCSS_SetDefaultScale( bmw, position );

  MCSS_SetAnimCtrlCallBack( bmw->mcss[ position ], position, BTLV_MCSS_CallBackFunctorFrame, 1 );
  //MCSS_SetTraverseMCNodesCallBack( bmw->mcss[ position ], position, BTLV_MCSS_CallBackNodes );
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

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT_MSG( bmw->mcss[ position ] == NULL, "pos=%d", position );

  BTLV_MCSS_MakeMAWTrainer( tr_type, &maw, position );
  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->mcss[ position ] = MCSS_Add( bmw->mcss_sys, pos.x, pos.y, pos.z, &maw );

  BTLV_MCSS_SetDefaultScale( bmw, position );

  //MCSS_SetAnimCtrlCallBack( bmw->mcss[ position ], position, BTLV_MCSS_CallBackFunctorFrame, 1 );
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
  MCSS_Del( bmw->mcss_sys, bmw->mcss[ position ] );
  bmw->mcss[ position ] = NULL;
  if( bmw->tcb[ position ] )
  { 
    GFL_HEAP_FreeMemory( GFL_TCB_GetWork( bmw->tcb[ position ] ) );
    GFL_TCB_DeleteTask( bmw->tcb[ position ] );
    bmw->tcb[ position ] = NULL;
  }
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

  pos.x = pos_x;
  pos.y = pos_y;
  pos.z = pos_z;

  MCSS_SetPosition( bmw->mcss[ position ], &pos );
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

  bmw->mcss_proj_mode = BTLV_MCSS_PROJ_ORTHO;

  for( position = 0 ; position < BTLV_MCSS_POS_TOTAL ; position++ ){
    if( bmw->mcss[ position ] ){
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

  bmw->mcss_proj_mode = BTLV_MCSS_PROJ_PERSPECTIVE;

  for( position = 0 ; position < BTLV_MCSS_POS_TOTAL ; position++ ){
    if( bmw->mcss[ position ] ){
      BTLV_MCSS_SetDefaultScale( bmw, position );
    }
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
  GF_ASSERT( bmw->mcss[ position ] != NULL );
  if( flag == BTLV_MCSS_MEPACHI_FLIP ){
    MCSS_FlipMepachiFlag( bmw->mcss[ position ] );
  }
  else if( flag == BTLV_MCSS_MEPACHI_ON ){
    MCSS_SetMepachiFlag( bmw->mcss[ position ] );
  }
  else{
    MCSS_ResetMepachiFlag( bmw->mcss[ position ] );
  }
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
  GF_ASSERT( bmw->mcss[ position ] != NULL );
  if( flag == BTLV_MCSS_ANM_STOP_ON ){
    MCSS_SetAnmStopFlag( bmw->mcss[ position ] );
  }
  else{
    MCSS_ResetAnmStopFlag( bmw->mcss[ position ] );
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
int   BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position )
{
  GF_ASSERT( bmw->mcss[ position ] != NULL );
  return MCSS_GetVanishFlag( bmw->mcss[ position ] );
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
void  BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
{
  GF_ASSERT( bmw->mcss[ position ] != NULL );
  if( flag == BTLV_MCSS_VANISH_FLIP ){
    MCSS_FlipVanishFlag( bmw->mcss[ position ] );
  }
  else if( flag == BTLV_MCSS_VANISH_ON ){
    MCSS_SetVanishFlag( bmw->mcss[ position ] );
  }
  else{
    MCSS_ResetVanishFlag( bmw->mcss[ position ] );
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
  return BTLV_MCSS_GetPokeDefaultScaleEx( bmw, position, bmw->mcss_proj_mode );
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
  MCSS_GetScale( bmw->mcss[ position ], scale );
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

  MCSS_SetScale( bmw->mcss[ position ], scale );
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
  MCSS_SetShadowVanishFlag( bmw->mcss[ position ], flag );
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
  //MCSS_SetAnmSpeed( bmw->mcss[ position ], speed );
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

  MCSS_GetPosition( bmw->mcss[ position ], &start );
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
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, pos, frame, wait, count, TCB_BTLV_MCSS_Move, REVERSE_FLAG_ON );
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

  MCSS_GetOfsScale( bmw->mcss[ position ], &start );
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, scale, frame, wait, count, TCB_BTLV_MCSS_Scale, REVERSE_FLAG_OFF );
  bmw->mcss_tcb_scale_execute |= BTLV_EFFTOOL_Pos2Bit( position );
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

  MCSS_GetRotate( bmw->mcss[ position ], &start );
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, rotate, frame, wait, count, TCB_BTLV_MCSS_Rotate, REVERSE_FLAG_ON );
  bmw->mcss_tcb_rotate_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 * @brief �|�P�����܂΂���
 *
 * @param[in] bmw     BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in] position  �܂΂�����|�P�����̗����ʒu
 * @param[in] type    �܂΂����C�v
 * @param[in] wait    �܂΂����E�G�C�g
 * @param[in] count   �܂΂����J�E���g
 */
//============================================================================================
void  BTLV_MCSS_MoveBlink( BTLV_MCSS_WORK *bmw, int position, int type, int wait, int count )
{
  BTLV_MCSS_TCB_WORK  *pmtw = GFL_HEAP_AllocMemory( bmw->heapID, sizeof( BTLV_MCSS_TCB_WORK ) );

  pmtw->bmw       = bmw;
  pmtw->position      = position;
  pmtw->emw.move_type   = type;
  pmtw->emw.wait      = 0;
  pmtw->emw.wait_tmp    = wait;
  pmtw->emw.count     = count * 2;  //���ĊJ����1��ƃJ�E���g���邽�߂ɔ{���Ă���

  switch( type ){
  case BTLEFF_MEPACHI_ON:
    BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_ON );
    break;
  case BTLEFF_MEPACHI_OFF:
    BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_OFF );
    break;
  case BTLEFF_MEPACHI_MABATAKI:
    GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_Blink, pmtw, 0 );
    bmw->mcss_tcb_blink_execute |= BTLV_EFFTOOL_Pos2Bit( position );
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

  start.x = MCSS_GetAlpha( bmw->mcss[ position ] );
  start.y = 0;
  start.z = 0;
  end.x = alpha;
  end.y = 0;
  end.z = 0;
  BTLV_MCSS_TCBInitialize( bmw, position, type, &start, &end, frame, wait, count, TCB_BTLV_MCSS_Alpha, REVERSE_FLAG_OFF );
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
  BTLV_MCSS_MOVE_CIRCLE_PARAM* bmmcp_p = GFL_HEAP_AllocMemory( bmw->heapID, sizeof( BTLV_MCSS_MOVE_CIRCLE_PARAM ) );

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

  GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_MoveCircle, bmmcp_p, 0 );
  bmw->mcss_tcb_move_execute |= BTLV_EFFTOOL_Pos2Bit( bmmcp_p->position );
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

  if( BTLV_MCSS_CheckExist( bmw, position ) )
  {
    pal_fade_flag = MCSS_CheckExecutePaletteFade( bmw->mcss[ position ] );
  }

  return ( ( bmw->mcss_tcb_move_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_scale_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_rotate_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_blink_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( bmw->mcss_tcb_alpha_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
       ( pal_fade_flag ) );
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
  return (bmw->mcss[ position ] != NULL );
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
  MCSS_SetPaletteFade( bmw->mcss[ position ], start_evy, end_evy, wait, rgb );
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
static  void  BTLV_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position )
{
  int dir = ( ( position & 1 ) ) ? MCSS_DIR_FRONT : MCSS_DIR_BACK;

  MCSS_TOOL_MakeMAWPP( pp, maw, dir );
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

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  GF_ASSERT( bmw->mcss[ position ] );

  def_scale = BTLV_MCSS_GetDefaultScale( bmw, position, bmw->mcss_proj_mode );

  VEC_Set( &scale, def_scale, def_scale, FX32_ONE );

  MCSS_SetScale( bmw->mcss[ position ], &scale );

  def_scale = BTLV_MCSS_GetDefaultScale( bmw, position, BTLV_MCSS_PROJ_PERSPECTIVE );

  //VEC_Set( &scale, def_scale, def_scale / 2, FX32_ONE );
  VEC_Set( &scale, def_scale, def_scale, FX32_ONE );

  MCSS_SetShadowScale( bmw->mcss[ position ], &scale );
}

//============================================================================================
/**
 * @brief �|�P��������n�^�X�N����������
 */
//============================================================================================
static  void  BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func, int reverse_flag )
{
  BTLV_MCSS_TCB_WORK  *pmtw = GFL_HEAP_AllocMemory( bmw->heapID, sizeof( BTLV_MCSS_TCB_WORK ) );

  pmtw->bmw               = bmw;
  pmtw->position          = position;
  pmtw->emw.move_type     = type;
  pmtw->emw.vec_time      = frame;
  pmtw->emw.vec_time_tmp  = frame;
  pmtw->emw.wait          = 0;
  pmtw->emw.wait_tmp      = wait;
  pmtw->emw.count         = count * 2;
  pmtw->emw.start_value.x = start->x;
  pmtw->emw.start_value.y = start->y;
  pmtw->emw.start_value.z = start->z;
  pmtw->emw.end_value.x   = end->x;
  pmtw->emw.end_value.y   = end->y;
  pmtw->emw.end_value.z   = end->z;

  switch( type ){
  case EFFTOOL_CALCTYPE_DIRECT:         //���ڃ|�W�V�����Ɉړ�
    break;
  case EFFTOOL_CALCTYPE_INTERPOLATION:  //�ړ���܂ł��Ԃ��Ȃ���ړ�
    BTLV_EFFTOOL_CalcMoveVector( &pmtw->emw.start_value, end, &pmtw->emw.vector, FX32_CONST( frame ) );
    break;
  case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG: //�w�肵����Ԃ������ړ�
    pmtw->emw.count         *= 2;
    //fall through
  case EFFTOOL_CALCTYPE_ROUNDTRIP:      //�w�肵����Ԃ������ړ�
    pmtw->emw.vector.x = FX_Div( end->x, FX32_CONST( frame ) );
    pmtw->emw.vector.y = FX_Div( end->y, FX32_CONST( frame ) );
    pmtw->emw.vector.z = FX_Div( end->z, FX32_CONST( frame ) );
    if( ( position & 1 ) && ( reverse_flag ) )
    {
      pmtw->emw.vector.x *= -1;
      pmtw->emw.vector.z *= -1;
    }
    break;
  }
  GFL_TCB_AddTask( bmw->tcb_sys, func, pmtw, 0 );
}

//============================================================================================
/**
 * @brief �|�P�����ړ��^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *pmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = pmtw->bmw;
  VecFx32 now_pos;
  BOOL  ret;

  MCSS_GetPosition( bmw->mcss[ pmtw->position ], &now_pos );
  ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_pos );
  MCSS_SetPosition( bmw->mcss[ pmtw->position ], &now_pos );
  if( ret == TRUE ){
    bmw->mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 * @brief �|�P�����g�k�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *pmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = pmtw->bmw;
  VecFx32 now_scale;
  BOOL  ret;

  MCSS_GetOfsScale( bmw->mcss[ pmtw->position ], &now_scale );
  ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_scale );
  MCSS_SetOfsScale( bmw->mcss[ pmtw->position ], &now_scale );
  if( ret == TRUE ){
    bmw->mcss_tcb_scale_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 * @brief �|�P������]�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *pmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = pmtw->bmw;
  VecFx32 now_rotate;
  BOOL  ret;

  MCSS_GetRotate( bmw->mcss[ pmtw->position ], &now_rotate );
  ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_rotate );
  MCSS_SetRotate( bmw->mcss[ pmtw->position ], &now_rotate );
  if( ret == TRUE ){
    bmw->mcss_tcb_rotate_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 * @brief �|�P�����܂΂����^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *pmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = pmtw->bmw;

  if( pmtw->emw.wait == 0 ){
    pmtw->emw.wait = pmtw->emw.wait_tmp;
    BTLV_MCSS_SetMepachiFlag( pmtw->bmw, pmtw->position, BTLV_MCSS_MEPACHI_FLIP );
    if( --pmtw->emw.count == 0 ){
      bmw->mcss_tcb_blink_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
      GFL_HEAP_FreeMemory( work );
      GFL_TCB_DeleteTask( tcb );
    }
  }
  else{
    pmtw->emw.wait--;
  }
}

//============================================================================================
/**
 * @brief �|�P�������l�^�X�N
 */
//============================================================================================
static  void  TCB_BTLV_MCSS_Alpha( GFL_TCB *tcb, void *work )
{
  BTLV_MCSS_TCB_WORK  *pmtw = ( BTLV_MCSS_TCB_WORK * )work;
  BTLV_MCSS_WORK *bmw = pmtw->bmw;
  VecFx32 now_alpha;
  BOOL  ret;

  now_alpha.x = MCSS_GetAlpha( bmw->mcss[ pmtw->position ] );
  now_alpha.y = 0;
  now_alpha.z = 0;
  ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_alpha );
  MCSS_SetAlpha( bmw->mcss[ pmtw->position ], now_alpha.x );
  if( ret == TRUE ){
    bmw->mcss_tcb_alpha_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
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
      MCSS_SetOfsPosition( bmw->mcss[ bmmcp->position ], &ofs );
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
    bmw->mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( bmmcp->position ) ^ BTLV_EFFTOOL_POS2BIT_XOR );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
  }
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

  switch( bmsa->seq_no ){ 
  case 0:
    NNS_G2dStopAnimCtrl( NNS_G2dGetMCAnimAnimCtrl( MCSS_GetAnimCtrl( bmw->mcss[ bmsa->position ] ) ) );
    MCSS_SetTraverseMCNodesCallBack( bmw->mcss[ bmsa->position ], ANIME_STOP_FLAG | bmsa->position, BTLV_MCSS_CallBackNodes );
    bmsa->seq_no++;
    bmsa->wait = BTLV_MCSS_STOP_ANIME_TIME;
    break;
  case 1:
    if( ( --bmsa->wait == 0 ) || 
        ( BTLV_EFFVM_GetExecuteEffectType( BTLV_EFFECT_GetVMHandle() ) == EXECUTE_EFF_TYPE_WAZA ) )
    { 
      MCSS_SetAnimeIndex( bmw->mcss[ bmsa->position ], SEQ_ANIME_STANDBY );
      NNS_G2dStartAnimCtrl( NNS_G2dGetMCAnimAnimCtrl( MCSS_GetAnimCtrl( bmw->mcss[ bmsa->position ] ) ) );
      MCSS_SetTraverseMCNodesCallBack( bmw->mcss[ bmsa->position ], ANIME_START_FLAG | bmsa->position, BTLV_MCSS_CallBackNodes );
      bmsa->seq_no++;
    }
    break;
  case 2:
    MCSS_SetAnimCtrlCallBack( bmw->mcss[ bmsa->position ], bmsa->position, BTLV_MCSS_CallBackFunctorFrame, 1 );
    GFL_HEAP_FreeMemory( work );
    GFL_TCB_DeleteTask( tcb );
    bmw->tcb[ bmsa->position ] = NULL;
    break;
  }
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

  if( BTLV_EFFVM_GetExecuteEffectType( BTLV_EFFECT_GetVMHandle() ) == EXECUTE_EFF_TYPE_WAZA )
  { 
    return;
  }

  if( MCSS_GetStopCellAnms( bmw->mcss[ data ] ) == MCSS_CELL_ANIM_NONSTOP ) 
  { 
    return;
  }

  bmsa = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bmw->heapID ), sizeof( BTLV_MCSS_STOP_ANIME ) );
  bmsa->position = data;

  if( MCSS_GetMCellAnmNum( bmw->mcss[ data ] ) > 1 )
  { 
    MCSS_SetAnimeIndex( bmw->mcss[ data ], SEQ_ANIME_STOP );
  }

  GF_ASSERT( bmw->tcb[ data ] == NULL );
  bmw->tcb[ data ] = GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_StopAnime, bmsa, 0 );
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
  int position = data & 0x0000ffff;

  if( data & ANIME_STOP_FLAG )
  { 
    BOOL  flag = FALSE;
    u8    cnt = MCSS_GetStopCellAnms( bmw->mcss[ data ] );
    int   i;

    for( i = 0 ; i < cnt ; i++ )
    { 
      if( nodeIdx == MCSS_GetStopNode( bmw->mcss[ data ], i ) )
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
    if( position & 1 )
    {
      scale = BTLV_MCSS_ORTHO_SCALE_ENEMY;
    }
    else
    {
      scale = BTLV_MCSS_ORTHO_SCALE_MINE;
    }
  }
  return scale;
}

//============================================================================================
/**
 * @brief �L�����f�[�^�ɂԂ�������
 *
 * @param[in] maw  MCSS_ADD_WORK�\���̂ւ̃|�C���^
 * @param[in] tlw  ���\�[�X�f�[�^���[�N�\����
 * @param[in] work �������������Ă���
 *
 * @retval  TRUE:�R�[���o�b�N����������
 */
//============================================================================================
static  BOOL  BTLV_MCSS_MakeBuchi( const MCSS_ADD_WORK* maw, TCB_LOADRESOURCE_WORK* tlw, void* work )
{ 
  //�p�b�`�[���ɂԂ�������
  if( maw->ncec == NARC_pokegra_wb_pfwb_327_NCEC )
  { 
	  const	PATTIIRU_BUCHI_DATA	*pbd;
	  int i, j;
	  u8	setx, sety, cnt;
	  int	pos[ 2 ];
    u32 *rnd = (u32*)work;
    u8  *buf = tlw->pCharData->pRawData;

	  //1����
	  for( i = 0 ; i < 4 ; i++ )
    {
		  pbd = pbd_table[ i ];
		  cnt=0;
		  while( pbd[ cnt ].posx != 0xff )
      {
			  setx = pbd[ cnt ].posx +   ( ( (*rnd) & 0x0f ) - 8 );
			  sety = pbd[ cnt ].posy + ( ( ( (*rnd) & 0xf0 ) >> 4 ) - 8 );
			  pos[ 0 ] = setx / 2 + sety * 128;
			  pos[ 1 ] = setx / 2 + ( sety + 40 ) * 128;
        for( j = 0 ; j < 2 ; j++ )
        { 
			    if( setx & 1)
          {
				    if( ( ( buf[ pos[ j ] ] & 0xf0 ) >= 0x10 ) && ( ( buf[ pos[ j ] ] & 0xf0 ) <= 0x30) )
            {
					    buf[ pos[ j ] ] += 0x50;
				    }
			    }
			    else
          {
				    if( ( ( buf[ pos[ j ] ] & 0x0f ) >= 0x01 ) && ( ( buf[ pos[ j ] ] & 0x0f ) <= 0x03 ) )
            {
					    buf[ pos[ j ] ] += 0x05;
				    }
			    }
        }
			  cnt++;
		  }
		  (*rnd) = (*rnd) >> 8;
	  }
  }

  return FALSE;
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

  GF_ASSERT( position < BTLV_MCSS_POS_TOTAL );
  if( bmw->mcss[ position ] ){
    BTLV_MCSS_Del( bmw, position );
  }

  BTLV_MCSS_GetDefaultPos( bmw, &pos, position );
  bmw->mcss[ position ] = MCSS_AddDebug( bmw->mcss_sys, pos.x, pos.y, pos.z, madw );

  BTLV_MCSS_SetDefaultScale( bmw, position );
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
 * @brief ���ݎ��s���̃^�X�N�������I���i�f�o�b�O�p�j
 *
 * @param[in] bmw   BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void  BTLV_MCSS_ClearExecute( BTLV_MCSS_WORK *bmw )
{

}

#endif
