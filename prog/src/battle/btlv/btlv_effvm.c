
//============================================================================================
/**
 * @file  btlv_effvm.c
 * @brief �퓬�G�t�F�N�g�X�N���v�g�R�}���h���
 * @author  soga
 * @date  2008.11.21
 */
//============================================================================================

#include <gflib.h>
#include "system/vm_cmd.h"
#include "sound/pm_sndsys.h"

#include "btlv_effect.h"

#include "arc_def.h"

//============================================================================================
/**
 *  �萔�錾
 */
//============================================================================================

enum{
  EFFVM_PTCNO_NONE = 0xffffffff,    //ptc_no�̖��i�[�̂Ƃ��̒l
  EFFVM_OBJNO_NONE = 0xffffffff,    //obj_no�̖��i�[�̂Ƃ��̒l
  TEMP_WORK_SIZE = 16,          //�e���|�������[�N�̃T�C�Y
  BTLV_EFFVM_BG_PAL = 8,        //�G�t�F�N�gBG�̃p���b�g�J�n�ʒu
  EFFVM_OBJ_MAX = 4,            //�G�t�F�N�g���Ɏg�p�ł���OBJMAX

	ORTHO_WIDTH = 4,
	ORTHO_HEIGHT = 3,
};

#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_sogabe
//#define DEBUG_OS_PRINT
#endif
#endif

//============================================================================================
/**
 *  �\���̐錾
 */
//============================================================================================

typedef struct{
  u32               position_reverse_flag :1;     //�����ʒu���]�t���O
  u32               camera_ortho_on_flag  :1;     //�J�����ړ���A���ˉe�ɖ߂��t���O
  u32               se_play_wait_flag     :1;     //SE�Đ��E�G�C�g��
  u32               se_effect_enable_flag :1;     //SEEFFECT�t���O
  u32               set_priority_flag     :1;     //PRIORITY���삳�ꂽ���H
  u32               set_alpha_flag        :1;     //ALPHA���삳�ꂽ���H
  u32               execute_effect_type   :1;     //�N�����Ă���G�t�F�N�g�^�C�v�i0:�Z�G�t�F�N�g�@1:�퓬�G�t�F�N�g�j
  u32                                     :25;
  GFL_TCBSYS*       tcbsys;
  GFL_PTC_PTR       ptc[ PARTICLE_GLOBAL_MAX ];
  int               ptc_no[ PARTICLE_GLOBAL_MAX ];
  int               obj[ EFFVM_OBJ_MAX ];
  BtlvMcssPos       attack_pos;
  BtlvMcssPos       defence_pos;
  int               wait;
  VM_CODE*          sequence;
  void*             temp_work[ TEMP_WORK_SIZE ];
  int               temp_work_count;
  HEAPID            heapID;
  VMCMD_RESULT      control_mode;
  int               effect_end_wait_kind;
  int               camera_projection;
  BTLV_EFFVM_PARAM  param;
  WazaID            waza;
#ifdef PM_DEBUG
  const DEBUG_PARTICLE_DATA*  dpd;
  BOOL                        debug_flag;
  ARCDATID                    dat_id[ PARTICLE_GLOBAL_MAX ];
  u32                         dat_id_cnt;
  void*                       unpack_info[ PARTICLE_GLOBAL_MAX ];
#endif
}BTLV_EFFVM_WORK;

typedef struct{
  VMHANDLE* vmh;
  int       src;
  int       dst;
  VecFx32   ofs;
  fx32      angle;
  fx32      top;
  int       move_type;
  int       move_frame; //�ړ�����t���[����
  VecFx32   src_pos;
  VecFx32   dst_pos;
  int       ortho_mode;
}BTLV_EFFVM_EMIT_INIT_WORK;

//�G�~�b�^�ړ��p�p�����[�^�\����
typedef struct{
  MtxFx43 mtx43;      //�ړ����W�ϊ��p�s��
  fx32    radius_x;   //�������ړ�����X�������a
  fx32    radius_y;   //�������ړ�����Y�������a�i�O���ƒ����ɂȂ�͂��j
  fx32    angle;      //���������݊p�x
  fx32    speed;      //�X�s�[�h
  int     move_frame; //�ړ�����t���[����
  int     move_type;  //�ړ��^�C�v
  int     wait;
  fx32    wait_tmp;
  fx32    wait_tmp_plus;
}BTLV_EFFVM_EMITTER_MOVE_WORK;

//�G�~�b�^�~�ړ��p�p�����[�^�\����
typedef struct{
  int       center;       //��]���S
  VecFx32   center_pos;   //��]���S���W
  fx32      radius_h;     //���������a
  fx32      radius_v;     //�c�������a
  int       angle;        //���݊p�x
  int       speed;        //�X�s�[�h
  int       frame;        //�ړ�����t���[����
  int       frame_tmp;  
  int       wait;         //1�t���[���ړ����邲�Ƃ̃E�G�C�g
  int       wait_tmp;
  int       count;        //�J�E���g
  int       after_wait;   //1��]���Ƃ̃E�G�C�g
  int       after_wait_tmp;
  BOOL      ortho_mode;   //���ˉe���[�h���ǂ���
}BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK;

//SE�Đ��p�p�����[�^�\����
typedef struct
{ 
  BTLV_EFFVM_WORK*  bevw;
	int               se_no;
	int               player;
	int               wait;
  int               pitch;
  int               vol;
  int               mod_depth;
  int               mod_speed;
}BTLV_EFFVM_SEPLAY;

//SE�G�t�F�N�g�p�p�����[�^�\����
typedef struct
{ 
  BTLV_EFFVM_WORK*  bevw;
  int               player;
  int               type;
  int               param;
  int               start;
  int               end;
  fx32              value;
  fx32              vec_value;
  int               start_wait;
  int               frame;
  int               frame_tmp;
  int               wait;
  int               wait_tmp;
  int               count;
}BTLV_EFFVM_SEEFFECT;

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================

VMHANDLE* BTLV_EFFVM_Init( GFL_TCBSYS *tcbsys, HEAPID heapID );
BOOL      BTLV_EFFVM_Main( VMHANDLE *vmh );
void      BTLV_EFFVM_Exit( VMHANDLE *vmh );
void      BTLV_EFFVM_Start( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, WazaID waza, BTLV_EFFVM_PARAM* param );
void      BTLV_EFFVM_Stop( VMHANDLE *vmh );

//�G�t�F�N�g�R�}���h
static VMCMD_RESULT VMEC_CAMERA_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CAMERA_MOVE_COODINATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CAMERA_MOVE_ANGLE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CAMERA_SHAKE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CAMERA_PROJECTION( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_PLAY_COORDINATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_PLAY_ORTHO( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_DELETE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_MOVE_COORDINATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE_ORTHO( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SCALE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_ROTATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_ALPHA( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SET_MEPACHI_FLAG( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SET_ANM_FLAG( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_PAL_FADE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_VANISH( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SHADOW_VANISH( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_DEL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_TRAINER_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_TRAINER_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_TRAINER_ANIME_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_TRAINER_DEL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_LOAD( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_SCROLL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_RASTER_SCROLL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_PAL_ANM( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_PRIORITY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_ALPHA( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_PAL_FADE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BG_VANISH( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_SCALE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_ANIME_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_DEL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_PLAY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_STOP( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_PAN( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_EFFECT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EFFECT_END_WAIT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_WAIT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CONTROL_MODE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_IF( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_MCSS_POS_CHECK( VMHANDLE *vmh, void *context_work );

static VMCMD_RESULT VMEC_SEQ_END( VMHANDLE *vmh, void *context_work );

//VM_WAIT_FUNC�Q
static  BOOL  VWF_EFFECT_END_CHECK( VMHANDLE *vmh, void *context_work );
static  BOOL  VWF_WAIT_CHECK( VMHANDLE *vmh, void *context_work );

//����J�֐��Q
static  int           EFFVM_GetPosition( VMHANDLE *vmh, int pos_flag );
static  int           EFFVM_ConvPosition( VMHANDLE *vmh, BtlvMcssPos position );
static  int           EFFVM_RegistPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID );
static  int           EFFVM_GetPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID );
static  void          EFFVM_InitEmitterPos( GFL_EMIT_PTR emit );
static  void          EFFVM_MoveEmitter( GFL_EMIT_PTR emit, unsigned int flag );
static  void          EFFVM_InitEmitterCircleMove( GFL_EMIT_PTR emit );
static  void          EFFVM_CircleMoveEmitter( GFL_EMIT_PTR emit, unsigned int flag );
static  void          EFFVM_DeleteEmitter( GFL_PTC_PTR ptc );
static  void          EFFVM_ChangeCameraProjection( BTLV_EFFVM_WORK *bevw );
static  void          EFFVM_SePlay( int se_no, int player, int pitch, int vol, int mod_depth, int mod_speed );
static  int           EFFVM_GetWork( BTLV_EFFVM_WORK* bevw, int param );
static  VMCMD_RESULT  EFFVM_INIT_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work, BOOL ortho_mode );
static  void          EFFVM_CalcPosOrtho( VecFx32 *pos, VecFx32 *ofs );
static	void          MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w );

//TCB�֐�
static  void  TCB_EFFVM_SEPLAY( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_SEEFFECT( GFL_TCB* tcb, void* work );

#ifdef PM_DEBUG
typedef enum
{ 
  DPD_TYPE_PARTICLE = 0,
  DPD_TYPE_BG,
}DPD_TYPE;

//�f�o�b�O�p�֐�
static  u32   BTLV_EFFVM_GetDPDNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID, DPD_TYPE type );
#endif

//============================================================================================
/**
 *  �f�[�^�e�[�u��
 */
//============================================================================================
#define TBL_AA2BB ( 4 * 0 )
#define TBL_BB2AA ( 4 * 1 )
#define TBL_A2B   ( 4 * 2 )
#define TBL_A2C   ( 4 * 3 )
#define TBL_A2D   ( 4 * 4 )
#define TBL_B2A   ( 4 * 5 )
#define TBL_B2C   ( 4 * 6 )
#define TBL_B2D   ( 4 * 7 )
#define TBL_C2A   ( 4 * 8 )
#define TBL_C2B   ( 4 * 9 )
#define TBL_C2D   ( 4 * 10 )
#define TBL_D2A   ( 4 * 11 )
#define TBL_D2B   ( 4 * 12 )
#define TBL_D2C   ( 4 * 13 )
#define TBL_ERROR ( 0xffffffff )

static const int  script_table[ BTLV_MCSS_POS_MAX ][ BTLV_MCSS_POS_MAX ]={
  //  AA          BB          A           B           C           D
  { TBL_AA2BB,  TBL_AA2BB,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR },  //AA
  { TBL_BB2AA,  TBL_BB2AA,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR },  //BB
  { TBL_ERROR,  TBL_ERROR,  TBL_AA2BB,  TBL_A2B,    TBL_A2C,    TBL_A2D   },  //A
  { TBL_ERROR,  TBL_ERROR,  TBL_B2A,    TBL_AA2BB,  TBL_B2C,    TBL_B2D   },  //B
  { TBL_ERROR,  TBL_ERROR,  TBL_C2A,    TBL_C2B,    TBL_AA2BB,  TBL_C2D   },  //C
  { TBL_ERROR,  TBL_ERROR,  TBL_D2A,    TBL_D2B,    TBL_D2C,    TBL_AA2BB },  //D
};

//============================================================================================
/**
 *  �X�N���v�g�e�[�u��
 */
//============================================================================================
static const VMCMD_FUNC btlv_effect_command_table[]={
  VMEC_CAMERA_MOVE,
  VMEC_CAMERA_MOVE_COODINATE,
  VMEC_CAMERA_MOVE_ANGLE,
  VMEC_CAMERA_SHAKE,
  VMEC_CAMERA_PROJECTION,
  VMEC_PARTICLE_LOAD,
  VMEC_PARTICLE_PLAY,
  VMEC_PARTICLE_PLAY_COORDINATE,
  VMEC_PARTICLE_PLAY_ORTHO,
  VMEC_PARTICLE_DELETE,
  VMEC_EMITTER_MOVE,
  VMEC_EMITTER_MOVE_COORDINATE,
  VMEC_EMITTER_CIRCLE_MOVE,
  VMEC_EMITTER_CIRCLE_MOVE_ORTHO,
  VMEC_POKEMON_MOVE,
  VMEC_POKEMON_CIRCLE_MOVE,
  VMEC_POKEMON_SCALE,
  VMEC_POKEMON_ROTATE,
  VMEC_POKEMON_ALPHA,
  VMEC_POKEMON_SET_MEPACHI_FLAG,
  VMEC_POKEMON_SET_ANM_FLAG,
  VMEC_POKEMON_PAL_FADE,
  VMEC_POKEMON_VANISH,
  VMEC_POKEMON_SHADOW_VANISH,
  VMEC_POKEMON_DEL,
  VMEC_TRAINER_SET,
  VMEC_TRAINER_MOVE,
  VMEC_TRAINER_ANIME_SET,
  VMEC_TRAINER_DEL,
  VMEC_BG_LOAD,
  VMEC_BG_SCROLL,
  VMEC_BG_RASTER_SCROLL,
  VMEC_BG_PAL_ANM,
  VMEC_BG_PRIORITY,
  VMEC_BG_ALPHA,
  VMEC_BG_PAL_FADE,
  VMEC_BG_VANISH,
  VMEC_OBJ_SET,
  VMEC_OBJ_MOVE,
  VMEC_OBJ_SCALE,
  VMEC_OBJ_ANIME_SET,
  VMEC_OBJ_DEL,
  VMEC_SE_PLAY,
  VMEC_SE_STOP,
  VMEC_SE_PAN,
  VMEC_SE_EFFECT,
  VMEC_EFFECT_END_WAIT,
  VMEC_WAIT,
  VMEC_CONTROL_MODE,
  VMEC_IF,
  VMEC_MCSS_POS_CHECK,

  VMEC_SEQ_END,
};

//============================================================================================
/**
 *  VM�C�j�V�����C�U�e�[�u��
 */
//============================================================================================
static  const VM_INITIALIZER  vm_init={
  BTLV_EFFVM_STACK_SIZE,                //u16 stack_size; ///<�g�p����X�^�b�N�̃T�C�Y
  BTLV_EFFVM_REG_SIZE,                  //u16 reg_size;   ///<�g�p���郌�W�X�^�̐�
  btlv_effect_command_table,            //const VMCMD_FUNC * command_table; ///<�g�p���鉼�z�}�V�����߂̊֐��e�[�u��
  NELEMS( btlv_effect_command_table ),  //const u32 command_max;      ///<�g�p���鉼�z�}�V�����ߒ�`�̍ő吔
};

//============================================================================================
/**
 * @brief VM������
 *
 * @param[in] heapID      �q�[�vID
 */
//============================================================================================
VMHANDLE  *BTLV_EFFVM_Init( GFL_TCBSYS *tcbsys, HEAPID heapID )
{
  int     i;
  VMHANDLE  *vmh;
  BTLV_EFFVM_WORK *bevw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_EFFVM_WORK ) );

  //VMCMD_RESULT��enum��`�Ȃ̂ŁA.s�ŃC���N���[�h�ł��Ȃ�����define��`���Ă��郉�x���ƈ�v�`�F�b�N�����Ă���
  GF_ASSERT( VMCMD_RESULT_SUSPEND == BTLEFF_CONTROL_MODE_SUSPEND );

  bevw->heapID = heapID;
  bevw->tcbsys = tcbsys;
  bevw->control_mode = BTLEFF_CONTROL_MODE_SUSPEND;
  bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;

  //�p�[�e�B�N���C���f�b�N�X���[�N������
  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    bevw->ptc_no[ i ] = EFFVM_PTCNO_NONE;
  }

  //OBJ�C���f�b�N�X���[�N������
  for( i = 0 ; i < EFFVM_OBJ_MAX ; i++ )
  {
    bevw->obj[ i ] = EFFVM_OBJNO_NONE;
  }

  vmh = VM_Create( heapID, &vm_init );
  VM_Init( vmh, bevw );

  return vmh;
}

//============================================================================================
/**
 * @brief VM������
 *
 * @param[in] heapID      �q�[�vID
 */
//============================================================================================
BOOL    BTLV_EFFVM_Main( VMHANDLE *vmh )
{
  BOOL  ret = VM_Control( vmh );
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  if( ( ret == FALSE ) && ( bevw->sequence ) )
  {
    if( bevw->waza < BTLEFF_SINGLE_ENCOUNT_1 )
    { 
      //HP�Q�[�W�\��
      BTLV_EFFECT_SetGaugeDrawEnable( TRUE );
  
      GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  
      G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                        GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                        31, 3 );
    }

    GFL_HEAP_FreeMemory( bevw->sequence );
    bevw->sequence = NULL;
  }

  return ret;
}

//============================================================================================
/**
 * @brief VM�I��
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_Exit( VMHANDLE *vmh )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  GFL_HEAP_FreeMemory ( bevw );
  VM_Delete( vmh );
}

//============================================================================================
/**
 * @brief VM�N��
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_Start( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, WazaID waza, BTLV_EFFVM_PARAM* param )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  int *start_ofs;
  int table_ofs;

  bevw->sequence = NULL;

  if( param != NULL )
  { 
    bevw->param = *param;
  }
  
  bevw->attack_pos = from;
  bevw->defence_pos = to;
  bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;
  bevw->waza = waza;
  if( bevw->waza < BTLEFF_SINGLE_ENCOUNT_1 )
  { 
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_WAZAEFF_SEQ, waza, bevw->heapID );
    //HP�Q�[�W��\��
    BTLV_EFFECT_SetGaugeDrawEnable( FALSE );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0,
                      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      0, 0 );
    bevw->execute_effect_type = EXECUTE_EFF_TYPE_WAZA;
  }
  else
  { 
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_BATTLEEFF_SEQ, waza - BTLEFF_SINGLE_ENCOUNT_1, bevw->heapID );
    bevw->execute_effect_type = EXECUTE_EFF_TYPE_BATTLE;
  }
  if( ( from != BTLV_MCSS_POS_ERROR ) && ( to != BTLV_MCSS_POS_ERROR ) )
  {
    table_ofs = script_table[ from ][ to ];
    GF_ASSERT( table_ofs != TBL_ERROR );
    if( table_ofs == TBL_ERROR )
    {
      table_ofs = TBL_AA2BB;
    }
  }
  else
  {
    table_ofs = TBL_AA2BB;
  }

  start_ofs = (int *)&bevw->sequence[ table_ofs ];

  VM_Start( vmh, &bevw->sequence[ start_ofs[ 0 ] ] );
}

//============================================================================================
/**
 * @brief VM������~
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void      BTLV_EFFVM_Stop( VMHANDLE *vmh )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  VMEC_SEQ_END( vmh, VM_GetContext( vmh ) );
  if( bevw->sequence )
  { 
    GFL_HEAP_FreeMemory( bevw->sequence );
    bevw->sequence = NULL;
  }
}

//============================================================================================
/**
 * @brief �N�����Ă���G�t�F�N�g�̃^�C�v���擾
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
BTLV_EFFVM_EXECUTE_EFF_TYPE BTLV_EFFVM_GetExecuteEffectType( VMHANDLE *vmh )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  BTLV_EFFVM_EXECUTE_EFF_TYPE type = EXECUTE_EFF_TYPE_NONE;

  if( bevw->sequence )
  { 
    type = bevw->execute_effect_type;
  }

  return type;
}

//============================================================================================
/**
 * @brief �J�����ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CAMERA_MOVE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)context_work;
  VecFx32   cam_pos,cam_target;
  //�J�����^�C�v�ǂݍ���
  int     cam_type = ( int )VMGetU32( vmh );
  //�J�����ړ���ʒu��ǂݍ���
  int     cam_move_pos = ( int )VMGetU32( vmh );
  int     frame, wait, brake;
  static  VecFx32 cam_pos_table[]={
    { 0x00005ca6, 0x00005f33, 0x00013cc3 },       //BTLEFF_CAMERA_POS_AA
    { 0x00006994, 0x00006f33, 0x00006e79 },       //BTLEFF_CAMERA_POS_BB
    { 0x00005ca6, 0x00005f33, 0x00013cc3 },       //BTLEFF_CAMERA_POS_A
    { 0x00006994, 0x00006f33, 0x00006e79 },       //BTLEFF_CAMERA_POS_B
    { 0x00005ca6, 0x00005f33, 0x00013cc3 },       //BTLEFF_CAMERA_POS_C
    { 0x00006994, 0x00006f33, 0x00006e79 },       //BTLEFF_CAMERA_POS_D
    { 0x00005ca6, 0x00005f33, 0x00013cc3 },       //BTLEFF_CAMERA_POS_E
    { 0x00006994, 0x00006f33, 0x00006e79 },       //BTLEFF_CAMERA_POS_F
  };
  static  VecFx32 cam_target_table[]={
    { 0xfffff173, 0x00001d9a, 0x000027f6 },       //BTLEFF_CAMERA_POS_AA
    { 0xfffffe61, 0x00002d9a, 0xffff59ac },       //BTLEFF_CAMERA_POS_BB
    { 0xfffff173, 0x00001d9a, 0x000027f6 },       //BTLEFF_CAMERA_POS_A
    { 0xfffffe61, 0x00002d9a, 0xffff59ac },       //BTLEFF_CAMERA_POS_B
    { 0xfffff173, 0x00001d9a, 0x000027f6 },       //BTLEFF_CAMERA_POS_C
    { 0xfffffe61, 0x00002d9a, 0xffff59ac },       //BTLEFF_CAMERA_POS_D
    { 0xfffff173, 0x00001d9a, 0x000027f6 },       //BTLEFF_CAMERA_POS_E
    { 0xfffffe61, 0x00002d9a, 0xffff59ac },       //BTLEFF_CAMERA_POS_F
  };

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CAMERA_MOVE\n");
#endif DEBUG_OS_PRINT

  //�ړ��t���[������ǂݍ���
  frame = ( int )VMGetU32( vmh );
  //�ړ��E�G�C�g��ǂݍ���
  wait = ( int )VMGetU32( vmh );
  //�u���[�L�t���[������ǂݍ���
  brake = ( int )VMGetU32( vmh );

  //�f�t�H���g�ʒu�ɓ����Ȃ��Ȃ�A�����ˉe���[�h�ɂ���
  if( cam_move_pos != BTLEFF_CAMERA_POS_INIT )
  {
    EFFVM_ChangeCameraProjection( bevw );
  }
  else
  {
    bevw->camera_ortho_on_flag = 1; //�J�����ړ���A���ˉe�ɖ߂��t���O
  }

  switch( cam_move_pos ){
  case BTLEFF_CAMERA_POS_AA:
  case BTLEFF_CAMERA_POS_BB:
  case BTLEFF_CAMERA_POS_A:
  case BTLEFF_CAMERA_POS_B:
  case BTLEFF_CAMERA_POS_C:
  case BTLEFF_CAMERA_POS_D:
    cam_move_pos = EFFVM_ConvPosition( vmh, cam_move_pos );
    cam_pos.x = cam_pos_table[ cam_move_pos ].x;
    cam_pos.y = cam_pos_table[ cam_move_pos ].y;
    cam_pos.z = cam_pos_table[ cam_move_pos ].z;
    cam_target.x = cam_target_table[ cam_move_pos ].x;
    cam_target.y = cam_target_table[ cam_move_pos ].y;
    cam_target.z = cam_target_table[ cam_move_pos ].z;
    break;
  case BTLEFF_CAMERA_POS_ATTACK:
  case BTLEFF_CAMERA_POS_ATTACK_PAIR:
  case BTLEFF_CAMERA_POS_DEFENCE:
  case BTLEFF_CAMERA_POS_DEFENCE_PAIR:
    cam_move_pos = EFFVM_GetPosition( vmh, cam_move_pos - BTLEFF_CAMERA_POS_ATTACK + BTLEFF_POKEMON_SIDE_ATTACK );
    if( cam_move_pos == BTLV_MCSS_POS_ERROR )
    {
      return bevw->control_mode;
    }
    cam_pos.x = cam_pos_table[ cam_move_pos ].x;
    cam_pos.y = cam_pos_table[ cam_move_pos ].y;
    cam_pos.z = cam_pos_table[ cam_move_pos ].z;
    cam_target.x = cam_target_table[ cam_move_pos ].x;
    cam_target.y = cam_target_table[ cam_move_pos ].y;
    cam_target.z = cam_target_table[ cam_move_pos ].z;
    break;
  case BTLEFF_CAMERA_POS_INIT:
  default:
    BTLV_CAMERA_GetDefaultCameraPosition( &cam_pos, &cam_target );
    break;
  }

  //�J�����^�C�v����ړ�����v�Z
  switch( cam_type ){
  case BTLEFF_CAMERA_MOVE_DIRECT:   //�_�C���N�g
    BTLV_CAMERA_MoveCameraPosition( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target );
    break;
  case BTLEFF_CAMERA_MOVE_INTERPOLATION:  //�Ǐ]
    BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target, frame, wait, brake );
    break;
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �J�����ړ��i���W�w��j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CAMERA_MOVE_COODINATE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)context_work;
  VecFx32   cam_pos,cam_target;
  //�J�����^�C�v�ǂݍ���
  int     cam_type = ( int )VMGetU32( vmh );
  int     frame, wait, brake;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CAMERA_MOVE_COODINATE\n");
#endif DEBUG_OS_PRINT

  //�J�����ړ���ʒu��ǂݍ���
  cam_pos.x = ( fx32 )VMGetU32( vmh );
  cam_pos.y = ( fx32 )VMGetU32( vmh );
  cam_pos.z = ( fx32 )VMGetU32( vmh );
  cam_target.x = ( fx32 )VMGetU32( vmh );
  cam_target.y = ( fx32 )VMGetU32( vmh );
  cam_target.z = ( fx32 )VMGetU32( vmh );

  //�ړ��t���[������ǂݍ���
  frame = ( int )VMGetU32( vmh );
  //�ړ��E�G�C�g��ǂݍ���
  wait = ( int )VMGetU32( vmh );
  //�u���[�L�t���[������ǂݍ���
  brake = ( int )VMGetU32( vmh );

  //�����ˉe���[�h�ɂ���
  EFFVM_ChangeCameraProjection( bevw );

  //�J�����^�C�v����ړ�����v�Z
  switch( cam_type ){
  case BTLEFF_CAMERA_MOVE_DIRECT:   //�_�C���N�g
    BTLV_CAMERA_MoveCameraPosition( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target );
    break;
  case BTLEFF_CAMERA_MOVE_INTERPOLATION:  //�Ǐ]
    BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target, frame, wait, brake );
    break;
  case BTLEFF_CAMERA_MOVE_INTERPOLATION_RELATIVITY: //�Ǐ]�i���Ύw��j
    {
      VecFx32 now_pos, now_target;

      BTLV_CAMERA_GetCameraPosition( BTLV_EFFECT_GetCameraWork(), &now_pos, &now_target );
      VEC_Add( &now_pos, &cam_pos, &cam_pos );
      VEC_Add( &now_target, &cam_target, &cam_target );
      BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target, frame, wait, brake );
    }
    break;
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �J�����ړ��i�p�x�w��j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CAMERA_MOVE_ANGLE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)context_work;
  //�J�����^�C�v�ǂݍ���
  int   cam_type = ( int )VMGetU32( vmh );
  //�J�����p�x�ǂݍ���
  int   phi = ( int )VMGetU32( vmh );
  int   theta = ( int )VMGetU32( vmh );
  //�ړ��t���[������ǂݍ���
  int   frame = ( int )VMGetU32( vmh );
  //�ړ��E�G�C�g��ǂݍ���
  int   wait = ( int )VMGetU32( vmh );
  //�u���[�L�t���[������ǂݍ���
  int   brake = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CAMERA_MOVE_ANGLE\n");
#endif DEBUG_OS_PRINT

  //�����ˉe���[�h�ɂ���
  EFFVM_ChangeCameraProjection( bevw );

  //�J�����^�C�v����ړ�����v�Z
  switch( cam_type ){
  case BTLEFF_CAMERA_MOVE_DIRECT:   //�_�C���N�g
    BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), phi, theta );
    break;
  case BTLEFF_CAMERA_MOVE_INTERPOLATION:  //�Ǐ]
    {
      VecFx32 cam_pos, cam_target;
      BTLV_CAMERA_GetCameraPositionAngle( BTLV_EFFECT_GetCameraWork(), phi, theta, &cam_pos, &cam_target );
      BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target, frame, wait, brake );
    }
    break;
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �J�������
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CAMERA_SHAKE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)context_work;
  int dir     = ( int )VMGetU32( vmh );
  fx32 value  = ( fx32 )VMGetU32( vmh );
  fx32 offset = ( fx32 )VMGetU32( vmh );
  int frame   = ( int )VMGetU32( vmh );
  int wait    = ( int )VMGetU32( vmh );
  int count   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CAMERA_SHAKE\n");
#endif DEBUG_OS_PRINT

  BTLV_CAMERA_Shake( BTLV_EFFECT_GetCameraWork(), dir, value, offset, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �ˉe���[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CAMERA_PROJECTION( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CAMERA_PROJECTION\n");
#endif DEBUG_OS_PRINT

  //�ˉe���[�h��ǂݍ���
  bevw->camera_projection = ( int )VMGetU32( vmh );

  EFFVM_ChangeCameraProjection( bevw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N�����\�[�X���[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  void      *heap;
  void      *resource;
  ARCDATID  datID = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no = EFFVM_RegistPtcNo( bevw, datID );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_LOAD\n");
#endif DEBUG_OS_PRINT

#ifdef PM_DEBUG
  //�f�o�b�O�ǂݍ��݂̏ꍇ�͐�p�̃o�b�t�@���烍�[�h����
  if( bevw->debug_flag == TRUE )
  {
    u32   ofs;
    u32*  ofs_p;

    heap = GFL_HEAP_AllocMemory( bevw->heapID, PARTICLE_LIB_HEAP_SIZE );
    bevw->ptc[ ptc_no ] = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, bevw->heapID );
    ofs_p = (u32*)&bevw->dpd->adrs[ 0 ];
    ofs = ofs_p[ BTLV_EFFVM_GetDPDNo( bevw, datID, DPD_TYPE_PARTICLE ) ];
    resource = (void *)&bevw->dpd->adrs[ ofs ];
    GFL_PTC_SetResourceEx( bevw->ptc[ ptc_no ], resource, FALSE, GFUser_VIntr_GetTCBSYS() );
    return bevw->control_mode;
  }
#endif
  heap = GFL_HEAP_AllocMemory( bevw->heapID, PARTICLE_LIB_HEAP_SIZE );
  bevw->ptc[ ptc_no ] = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, bevw->heapID );
  resource = GFL_PTC_LoadArcResource( ARCID_PTC, datID, bevw->heapID );
  GFL_PTC_SetResource( bevw->ptc[ ptc_no ], resource, FALSE, GFUser_VIntr_GetTCBSYS() );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Đ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY\n");
#endif DEBUG_OS_PRINT

  beeiw->vmh = vmh;
  beeiw->src = ( int )VMGetU32( vmh );
  beeiw->dst = ( int )VMGetU32( vmh );
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.z = 0;
  beeiw->angle = ( fx32 )VMGetU32( vmh );

  if( beeiw->dst == BTLEFF_PARTICLE_PLAY_SIDE_NONE )
  {
    beeiw->dst = beeiw->src;
  }

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Đ��i���W�w��j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_PLAY_COORDINATE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY_COORDINATE\n");
#endif DEBUG_OS_PRINT

  beeiw->vmh = vmh;
  beeiw->src = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
  beeiw->dst = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
  beeiw->src_pos.x = ( fx32 )VMGetU32( vmh );
  beeiw->src_pos.y = ( fx32 )VMGetU32( vmh );
  beeiw->src_pos.z = ( fx32 )VMGetU32( vmh );
  beeiw->dst_pos.x = ( fx32 )VMGetU32( vmh );
  beeiw->dst_pos.y = ( fx32 )VMGetU32( vmh );
  beeiw->dst_pos.z = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.z = 0;
  beeiw->angle = ( fx32 )VMGetU32( vmh );

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Đ��i���ˉe�j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_PLAY_ORTHO( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY\n");
#endif DEBUG_OS_PRINT

  { 
    GFL_G3D_PROJECTION  proj;
	  VecFx32 Eye    = { 0, 0, 0 };          // Eye position
	  VecFx32 vUp    = { 0, FX32_ONE, 0 };  // Up
	  VecFx32 at     = { 0, 0, -FX32_ONE }; // Viewpoint

    proj.type = GFL_G3D_PRJORTH;
    proj.param1 = FX32_CONST( ORTHO_HEIGHT );
    proj.param2 = -FX32_CONST( ORTHO_HEIGHT );
    proj.param3 = -FX32_CONST( ORTHO_WIDTH );
    proj.param4 = FX32_CONST( ORTHO_WIDTH );
	  proj.near		= FX32_ONE * 1;
    proj.far    = FX32_ONE * 1024;
	  proj.scaleW	= FX32_ONE;

    if( GFL_PTC_GetCameraPtr( bevw->ptc[ ptc_no ] ) == NULL )
    { 
      GFL_PTC_PersonalCameraCreate( bevw->ptc[ ptc_no ], &proj, DEFAULT_PERSP_WAY, &Eye, &vUp, &at, bevw->heapID );
    }
  }

  beeiw->vmh = vmh;
  beeiw->src = ( int )VMGetU32( vmh );
  beeiw->dst = beeiw->src;
  beeiw->ofs.x = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.y = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.z = ( fx32 )VMGetU32( vmh );
  beeiw->ortho_mode = 1;

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���폜
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_DELETE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_DELETE\n");
#endif DEBUG_OS_PRINT

  EFFVM_DeleteEmitter( bevw->ptc[ ptc_no ] );
  bevw->ptc[ ptc_no ] = NULL;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �G�~�b�^�ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_EMITTER_MOVE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_MOVE\n");
#endif DEBUG_OS_PRINT

  beeiw->vmh = vmh;
  beeiw->move_type = ( int )VMGetU32( vmh );
  beeiw->src = ( int )VMGetU32( vmh );
  beeiw->dst = ( int )VMGetU32( vmh );
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.z = 0;
  beeiw->move_frame = ( int )VMGetU32( vmh );
  beeiw->top = ( fx32 )VMGetU32( vmh );

  //�ړ����ƈړ��悪����̂Ƃ��́A�A�T�[�g�Ŏ~�߂�
  GF_ASSERT( beeiw->dst != beeiw->src );

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �G�~�b�^�ړ��i���W�w��j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_EMITTER_MOVE_COORDINATE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_MOVE_COORDINATE\n");
#endif DEBUG_OS_PRINT

  beeiw->vmh = vmh;
  beeiw->move_type = ( int )VMGetU32( vmh );
  beeiw->src = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
  beeiw->src_pos.x = ( fx32 )VMGetU32( vmh );
  beeiw->src_pos.y = ( fx32 )VMGetU32( vmh );
  beeiw->src_pos.z = ( fx32 )VMGetU32( vmh );
  beeiw->dst = ( int )VMGetU32( vmh );
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ( fx32 )VMGetU32( vmh );
  beeiw->ofs.z = 0;
  beeiw->move_frame = ( int )VMGetU32( vmh );
  beeiw->top = ( fx32 )VMGetU32( vmh );

  //�ړ����ƈړ��悪����̂Ƃ��́A�A�T�[�g�Ŏ~�߂�
  GF_ASSERT( beeiw->dst != beeiw->src );

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �G�~�b�^�~�ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work )
{ 
  return EFFVM_INIT_EMITTER_CIRCLE_MOVE( vmh, context_work, FALSE );
}

//============================================================================================
/**
 * @brief �G�~�b�^�~�ړ��i���ˉe�j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE_ORTHO( VMHANDLE *vmh, void *context_work )
{ 
  return EFFVM_INIT_EMITTER_CIRCLE_MOVE( vmh, context_work, TRUE );
}

//============================================================================================
/**
 * @brief �|�P�����ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_MOVE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position;
  int   type;
  VecFx32 move_pos;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_MOVE\n");
#endif DEBUG_OS_PRINT

  position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );

  type        = ( int )VMGetU32( vmh );
  move_pos.x  = ( fx32 )VMGetU32( vmh );
  move_pos.y  = ( fx32 )VMGetU32( vmh );
  move_pos.z  = 0;
  frame       = ( int )VMGetU32( vmh );
  wait        = ( int )VMGetU32( vmh );
  count       = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_MovePosition( BTLV_EFFECT_GetMcssWork(), position, type, &move_pos, frame, wait, count );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����~�^��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_MCSS_MOVE_CIRCLE_PARAM bmmcp;
  bmmcp.position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_CIRCLE_MOVE\n");
#endif DEBUG_OS_PRINT

  bmmcp.axis              = ( int )VMGetU32( vmh );
  bmmcp.shift             = ( int )VMGetU32( vmh );
  bmmcp.radius_h          = ( fx32 )VMGetU32( vmh );
	bmmcp.radius_v          = ( fx32 )VMGetU32( vmh );
	bmmcp.frame             = ( int )VMGetU32( vmh ) >> FX32_SHIFT;
  bmmcp.rotate_wait       = ( int )VMGetU32( vmh ) >> FX32_SHIFT;
  bmmcp.count             = ( int )VMGetU32( vmh ) >> FX32_SHIFT;
	bmmcp.rotate_after_wait = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( bmmcp.position != BTLV_MCSS_POS_ERROR )
  {
    if( bmmcp.count )
    { 
      BTLV_MCSS_MoveCircle( BTLV_EFFECT_GetMcssWork(), &bmmcp );
    }
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����g�k
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_SCALE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position;
  int   type;
  VecFx32 scale;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SCALE\n");
#endif DEBUG_OS_PRINT

  position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  type    = ( int )VMGetU32( vmh );
  scale.x = ( fx32 )VMGetU32( vmh );
  scale.y = ( fx32 )VMGetU32( vmh );
  scale.z = FX32_ONE;
  frame   = ( int )VMGetU32( vmh );
  wait    = ( int )VMGetU32( vmh );
  count   = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_MoveScale( BTLV_EFFECT_GetMcssWork(), position, type, &scale, frame, wait, count );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P������]
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_ROTATE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position;
  int   type;
  VecFx32 rotate;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_ROTATE\n");
#endif DEBUG_OS_PRINT

  position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  type     = ( int )VMGetU32( vmh );
  rotate.x   = 0;
  rotate.y   = 0;
  rotate.z   = ( fx32 )VMGetU32( vmh );
  frame    = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_MoveRotate( BTLV_EFFECT_GetMcssWork(), position, type, &rotate, frame, wait, count );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�������l
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_ALPHA( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int position;
  int type;
  int alpha;
  int frame;
  int wait;
  int count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_ALPHA\n");
#endif DEBUG_OS_PRINT

  position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  type  = ( int )VMGetU32( vmh );
  alpha = ( int )VMGetU32( vmh );
  frame = ( int )VMGetU32( vmh );
  wait  = ( int )VMGetU32( vmh );
  count = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_MoveAlpha( BTLV_EFFECT_GetMcssWork(), position, type, alpha, frame, wait, count );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�������p�`�t���O�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_SET_MEPACHI_FLAG( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position;
  int   type;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SET_MEPACHI_FLAG\n");
#endif DEBUG_OS_PRINT

  position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  type     = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    if( count == 0 )
    {
      count = 1;
    }
    BTLV_MCSS_MoveBlink( BTLV_EFFECT_GetMcssWork(), position, type, wait, count );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����A�j���[�V�����t���O�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_SET_ANM_FLAG( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position  = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int   flag      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SET_ANM_FLAG\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_SetAnmStopFlag( BTLV_EFFECT_GetMcssWork(), position, flag );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����p���b�g�t�F�[�h�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_PAL_FADE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position        = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int   start_evy       = ( int )VMGetU32( vmh );
  int   end_evy         = ( int )VMGetU32( vmh );
  int   wait            = ( int )VMGetU32( vmh );
  int   rgb             = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_PAL_FADE\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_SetPaletteFade( BTLV_EFFECT_GetMcssWork(), position, start_evy, end_evy, wait, rgb );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����o�j�b�V���t���O�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_VANISH( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position  = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int   flag      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_VANISH\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_SetVanishFlag( BTLV_EFFECT_GetMcssWork(), position, flag );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����e�o�j�b�V���t���O�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_SHADOW_VANISH( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position  = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int   flag      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SHADOW_VANISH\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪃G���[�̂Ƃ��́A�R�}���h���s���Ȃ�
  if( position != BTLV_MCSS_POS_ERROR )
  {
    BTLV_MCSS_SetShadowVanishFlag( BTLV_EFFECT_GetMcssWork(), position, flag );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�����폜
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_DEL( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_DEL\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_DelPokemon( position );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �g���[�i�[�Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_TRAINER_SET( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   index = ( int )VMGetU32( vmh );
  int   position = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int   pos_x = ( int )VMGetU32( vmh );
  int   pos_y = ( int )VMGetU32( vmh );
  int   pos_z = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetTrainer( index, position , pos_x, pos_y, pos_z );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �g���[�i�[�ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_TRAINER_MOVE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int           index;
  int           type;
  GFL_CLACTPOS  move_pos;
  int           frame;
  int           wait;
  int           count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_MOVE\n");
#endif DEBUG_OS_PRINT

  index = BTLV_EFFECT_GetTrainerIndex( ( int )VMGetU32( vmh ) );
  type     = ( int )VMGetU32( vmh );
  move_pos.x = ( int )VMGetU32( vmh );
  move_pos.y = ( int )VMGetU32( vmh );
  frame    = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  BTLV_CLACT_MovePosition( BTLV_EFFECT_GetCLWK(), index, type, &move_pos, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �g���[�i�[�A�j���Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_TRAINER_ANIME_SET( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index = BTLV_EFFECT_GetTrainerIndex( ( int )VMGetU32( vmh ) );
  int anm_no = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_ANIME_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_CLACT_SetAnime( BTLV_EFFECT_GetCLWK(), index, anm_no );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �g���[�i�[�폜
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_TRAINER_DEL( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int position = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_DEL\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_DelTrainer( position );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG�̃��[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_LOAD( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  ARCDATID  datID = ( ARCDATID )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_LOAD\n");
#endif DEBUG_OS_PRINT

#ifdef PM_DEBUG
  //�f�o�b�O�ǂݍ��݂̏ꍇ�͐�p�̃o�b�t�@���烍�[�h����
  if( bevw->debug_flag == TRUE )
  {
    u32   ofs;
    u32*  ofs_p;
    void* resource;
    int   dpd_no = BTLV_EFFVM_GetDPDNo( bevw, datID, DPD_TYPE_BG );
	  NNSG2dCharacterData* charData;
	  NNSG2dScreenData* scrnData;
		NNSG2dPaletteData* palData;

    ofs_p = (u32*)&bevw->dpd->adrs[ 0 ];

    ofs = ofs_p[ dpd_no + 0 ];
    resource = (void *)&bevw->dpd->adrs[ ofs ];

    if( bevw->unpack_info[ dpd_no + 0 ] == NULL )
    { 
      NNS_G2dGetUnpackedScreenData( resource, &scrnData );
      bevw->unpack_info[ dpd_no + 0 ] = scrnData;
    }
    else
    { 
      scrnData = (NNSG2dScreenData*)bevw->unpack_info[ dpd_no + 0 ];
    }
    GFL_BG_LoadScreen( GFL_BG_FRAME3_M, scrnData->rawData, 0x2000, 0 );

    ofs = ofs_p[ dpd_no + 1 ];
    resource = (void *)&bevw->dpd->adrs[ ofs ];

    if( bevw->unpack_info[ dpd_no + 1 ] == NULL )
    { 
	    NNS_G2dGetUnpackedBGCharacterData( resource, &charData );
      bevw->unpack_info[ dpd_no + 1 ] = charData;
    }
    else
    { 
      charData = (NNSG2dCharacterData*)bevw->unpack_info[ dpd_no + 1 ];
    }
    GFL_BG_LoadCharacter( GFL_BG_FRAME3_M, charData->pRawData, 0x8000, 0 );


    ofs = ofs_p[ dpd_no + 2 ];
    resource = (void *)&bevw->dpd->adrs[ ofs ];
    if( bevw->unpack_info[ dpd_no + 2 ] == NULL )
    { 
		  NNS_G2dGetUnpackedPaletteData( resource, &palData );
      bevw->unpack_info[ dpd_no + 2 ] = palData;
    }
    else
    { 
      palData = (NNSG2dPaletteData*)bevw->unpack_info[ dpd_no + 2 ];
    }
    PaletteWorkSet( BTLV_EFFECT_GetPfd(), palData->pRawData, FADE_MAIN_BG, BTLV_EFFVM_BG_PAL * 16, palData->szByte );

    return bevw->control_mode;
  }
#endif

  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WAZAEFF_GRA, datID + 1, GFL_BG_FRAME3_M, 0, 0, 0, bevw->heapID );
  GFL_ARC_UTIL_TransVramScreen( ARCID_WAZAEFF_GRA, datID, GFL_BG_FRAME3_M, 0, 0, 0, bevw->heapID );
  PaletteWorkSetEx_Arc( BTLV_EFFECT_GetPfd(), ARCID_WAZAEFF_GRA, datID + 2, bevw->heapID, FADE_MAIN_BG, 0,
                        BTLV_EFFVM_BG_PAL * 16, 0 );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG�̃X�N���[��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_SCROLL( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int type  = ( int )VMGetU32( vmh );
  int scr_x = ( int )VMGetU32( vmh );
  int scr_y = ( int )VMGetU32( vmh );
  int frame = ( int )VMGetU32( vmh );
  int wait  = ( int )VMGetU32( vmh );
  int count = ( int )VMGetU32( vmh );
  int position = EFFVM_GetPosition( vmh, BTLEFF_POKEMON_SIDE_ATTACK );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_SCROLL\n");
#endif DEBUG_OS_PRINT

  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, 0 );
  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, 0 );

  BTLV_BG_MovePosition( BTLV_EFFECT_GetBGWork(), position, type, scr_x, scr_y, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG�̃��X�^�[�X�N���[��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_RASTER_SCROLL( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int type    = ( int )VMGetU32( vmh );
  int radius  = ( int )VMGetU32( vmh );
  int line    = ( int )VMGetU32( vmh );
  int wait    = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_RASTER_SCROLL\n");
#endif DEBUG_OS_PRINT

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG�̃p���b�g�A�j��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_PAL_ANM( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int datID     = ( int )VMGetU32( vmh );
  int trans_pal = ( int )VMGetU32( vmh );
  int col_count = ( int )VMGetU32( vmh );
  int offset    = ( int )VMGetU32( vmh );
  int wait      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_PAL_ANM\n");
#endif DEBUG_OS_PRINT

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG�̃v���C�I���e�B
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_PRIORITY( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int pri = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_PRIORITY\n");
#endif DEBUG_OS_PRINT

  GFL_BG_SetPriority( GFL_BG_FRAME3_M, pri );

  bevw->set_priority_flag = 1;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	BG���l
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_ALPHA( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int bg_num  = ( int )VMGetU32( vmh );
  int type    = ( int )VMGetU32( vmh );
  int alpha   = ( int )VMGetU32( vmh );
  int frame   = ( int )VMGetU32( vmh );
  int wait    = ( int )VMGetU32( vmh );
  int count   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_ALPHA\n");
#endif DEBUG_OS_PRINT

  if( alpha == 31 )
  { 
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, 3 );
  }
  else
  { 
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, alpha );
  }

  bevw->set_alpha_flag = 1;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief BG�p���b�g�t�F�[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_PAL_FADE( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   flag      = ( int )VMGetU32( vmh );
  int   start_evy = ( int )VMGetU32( vmh );
  int   end_evy   = ( int )VMGetU32( vmh );
  int   wait      = ( int )VMGetU32( vmh );
  int   rgb       = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_PAL_FADE\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetPaletteFade( flag, start_evy, end_evy, wait, rgb );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief BG�̕\��/��\��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BG_VANISH( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int model = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_VANISH\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetVanishFlag( model, flag );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief OBJ�̃Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_SET( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index     = ( int )VMGetU32( vmh );
  int datID     = ( int )VMGetU32( vmh );
  int position  = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  int ofs_x     = ( int )VMGetU32( vmh );
  int ofs_y     = ( int )VMGetU32( vmh );
  fx32 scalex   = ( fx32 )VMGetU32( vmh );
  fx32 scaley   = ( fx32 )VMGetU32( vmh );
  VecFx32 pos;
  int pos_x,pos_y;

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] == EFFVM_OBJNO_NONE );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_OBJ_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &pos, position );
  pos.x += ofs_x;
  pos.y += ofs_y;
  NNS_G3dWorldPosToScrPos( &pos, &pos_x, &pos_y );

  bevw->obj[ index ] = BTLV_CLACT_AddAffine( BTLV_EFFECT_GetCLWK(), ARCID_WAZAEFF_GRA, datID, pos_x, pos_y, scalex, scaley );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief OBJ�ړ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_MOVE( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int           index;
  int           type;
  GFL_CLACTPOS  move_pos;
  int           frame;
  int           wait;
  int           count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_OBJ_MOVE\n");
#endif DEBUG_OS_PRINT

  index       = ( int )VMGetU32( vmh );
  type        = ( int )VMGetU32( vmh );
  move_pos.x  = ( int )VMGetU32( vmh );
  move_pos.y  = ( int )VMGetU32( vmh );
  frame       = ( int )VMGetU32( vmh );
  wait        = ( int )VMGetU32( vmh );
  count       = ( int )VMGetU32( vmh );

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] != EFFVM_OBJNO_NONE );

  BTLV_CLACT_MovePosition( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ], type, &move_pos, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief OBJ�g�k
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_SCALE( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   index;
  int   type;
  VecFx32 scale;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_OBJ_SCALE\n");
#endif DEBUG_OS_PRINT

  index   = ( int )VMGetU32( vmh );
  type    = ( int )VMGetU32( vmh );
  scale.x = ( fx32 )VMGetU32( vmh );
  scale.y = ( fx32 )VMGetU32( vmh );
  scale.z = FX32_ONE;
  frame   = ( int )VMGetU32( vmh );
  wait    = ( int )VMGetU32( vmh );
  count   = ( int )VMGetU32( vmh );

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] != EFFVM_OBJNO_NONE );

  BTLV_CLACT_MoveScale( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ], type, &scale, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief OBJ�A�j���Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_ANIME_SET( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index   = ( int )VMGetU32( vmh );
  int anm_no  = ( int )VMGetU32( vmh );

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] != EFFVM_OBJNO_NONE );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_OBJ_ANIME_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_CLACT_SetAnime( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ], anm_no );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief OBJ�폜
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_DEL( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_DEL\n");
#endif DEBUG_OS_PRINT

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] != EFFVM_OBJNO_NONE );

  BTLV_CLACT_Delete( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ] );
  bevw->obj[ index ] = EFFVM_OBJNO_NONE;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief SE�Đ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SE_PLAY( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int se_no     = ( int )VMGetU32( vmh );
  int player    = ( int )VMGetU32( vmh );
  int wait      = ( int )VMGetU32( vmh );
  int pitch     = ( int )VMGetU32( vmh );
  int vol       = ( int )VMGetU32( vmh );
  int mod_depth = ( int )VMGetU32( vmh );
  int mod_speed = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SE_PLAY\n");
#endif DEBUG_OS_PRINT

  if( mod_depth > 255 ) { mod_depth = 255; }
  if( mod_speed > 255 ) { mod_speed = 255; }

  if( wait == 0 )
  { 
    EFFVM_SePlay( se_no, player, pitch, vol, mod_depth, mod_speed );
  }
  else
  { 
    BTLV_EFFVM_SEPLAY*  bes = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_SEPLAY ) );
    bes->bevw       = bevw;
    bes->se_no      = se_no;
    bes->player     = player;
    bes->wait       = wait;
    bes->pitch      = pitch;
    bes->vol        = vol;
    bes->mod_depth  = mod_depth;
    bes->mod_speed  = mod_speed;

    bevw->se_play_wait_flag = 1;

    GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEPLAY, bes, 0 );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief SE�X�g�b�v
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SE_STOP( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int player      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SE_STOP\n");
#endif DEBUG_OS_PRINT

  if( player == BTLEFF_SESTOP_ALL )
  { 
    PMSND_StopSE();
  }
  else
  { 
    PMSND_StopSE_byPlayerID( player );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief SE�p��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SE_PAN( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_SEEFFECT ) );
  int start, end;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SE_PAN\n");
#endif DEBUG_OS_PRINT

  bes->bevw     = bevw;

  bes->player     = ( int )VMGetU32( vmh );
  bes->type       = ( int )VMGetU32( vmh );
  bes->param      = BTLEFF_SEEFFECT_PAN;
  start           = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  end             = EFFVM_GetPosition( vmh, ( int )VMGetU32( vmh ) );
  bes->start      = ( start & 1 ) ? 127 : -128;
  bes->end        = ( end & 1 ) ? 127 : -128;
  bes->start_wait = ( int )VMGetU32( vmh );
  bes->frame      = ( int )VMGetU32( vmh );
  bes->frame_tmp  = bes->frame;
  bes->wait       = 0;
  bes->wait_tmp   = ( int )VMGetU32( vmh );
  bes->count      = ( int )VMGetU32( vmh ) * 2;

  if( ( bes->type == BTLEFF_SEPAN_ROUNDTRIP ) && ( bes->count == 0 ) )
  { 
    bes->count = 2;
  }

	bes->value = FX32_CONST( bes->start );
	bes->vec_value = FX_Div( FX32_CONST( bes->end - bes->start ) , FX32_CONST( bes->frame ) );

  GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 );

  bevw->se_effect_enable_flag = 1;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief SE���I�ω�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SE_EFFECT( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_SEEFFECT ) );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SE_EFFECT\n");
#endif DEBUG_OS_PRINT

  bes->bevw     = bevw;

  bes->player     = ( int )VMGetU32( vmh );
  bes->type       = ( int )VMGetU32( vmh );
  bes->param      = ( int )VMGetU32( vmh );
  bes->start      = ( int )VMGetU32( vmh );
  bes->end        = ( int )VMGetU32( vmh );
  bes->start_wait = ( int )VMGetU32( vmh );
  bes->frame      = ( int )VMGetU32( vmh );
  bes->frame_tmp  = bes->frame;
  bes->wait       = 0;
  bes->wait_tmp   = ( int )VMGetU32( vmh );
  bes->count      = ( int )VMGetU32( vmh ) * 2;

  if( ( bes->type == BTLEFF_SEEFFECT_ROUNDTRIP ) && ( bes->count == 0 ) )
  { 
    bes->count = 2;
  }

	bes->value = FX32_CONST( bes->start );
	bes->vec_value = FX_Div( FX32_CONST( bes->end - bes->start ) , FX32_CONST( bes->frame ) );

  GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 );

  bevw->se_effect_enable_flag = 1;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �G�t�F�N�g�I���҂�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_EFFECT_END_WAIT( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EFFECT_END_WAIT\n");
#endif DEBUG_OS_PRINT

  bevw->effect_end_wait_kind = ( int )VMGetU32( vmh );

  VMCMD_SetWait( vmh, VWF_EFFECT_END_CHECK );

  //�G�t�F�N�g�҂��͕K��SUSPEND���[�h�ɐ؂�ւ���
  bevw->control_mode = VMCMD_RESULT_SUSPEND;

  return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 * @brief �^�C�}�[�E�G�C�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_WAIT( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_WAIT\n");
#endif DEBUG_OS_PRINT

  bevw->wait = ( int )VMGetU32( vmh );

  VMCMD_SetWait( vmh, VWF_WAIT_CHECK );

  //�E�G�C�g�͕K��SUSPEND���[�h�ɐ؂�ւ���
  bevw->control_mode = VMCMD_RESULT_SUSPEND;

  return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 * @brief �R���g���[�����[�h�ύX
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CONTROL_MODE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_CONTROL_MODE\n");
#endif DEBUG_OS_PRINT

  bevw->control_mode = ( VMCMD_RESULT )VMGetU32( vmh );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	�w�肳�ꂽ���[�N�����ď�������
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_IF( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int work  = EFFVM_GetWork( bevw, ( int )VMGetU32( vmh ) );
  int cond  = ( int )VMGetU32( vmh );
  int value = ( int )VMGetU32( vmh );
  int adrs  = ( int )VMGetU32( vmh );
  BOOL  flag = FALSE;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_IF\n");
#endif DEBUG_OS_PRINT

  switch( cond ){ 
  case BTLEFF_COND_EQUAL:       // ==
    if( work == value )
    { 
      flag = TRUE;
    }
    break;
  case BTLEFF_COND_NOT_EQUAL:   // !=
    if( work != value )
    { 
      flag = TRUE;
    }
    break;
  case BTLEFF_COND_MIMAN:       // <
    if( work < value )
    { 
      flag = TRUE;
    }
    break;
  case BTLEFF_COND_KOERU:       // >
    if( work > value )
    { 
      flag = TRUE;
    }
    break;
  case BTLEFF_COND_IKA:         // <=
    if( work <= value )
    { 
      flag = TRUE;
    }
    break;
  case BTLEFF_COND_IJOU:        // >=
    if( work >= value )
    { 
      flag = TRUE;
    }
    break;
  }

  if( flag == TRUE )
  { 
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �����ʒu�`�F�b�N
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_MCSS_POS_CHECK( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int pos =  ( int )VMGetU32( vmh );
  int cond = ( int )VMGetU32( vmh );
  int adrs = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_MCSS_POS_CHECK:\npos:%d cond:%d adrs:%d\n",pos,cond,adrs);
#endif DEBUG_OS_PRINT
  if( BTLV_EFFECT_CheckExist( pos ) == cond )
  { 
    VMCMD_Jump( vmh, vmh->adrs + adrs );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �G�t�F�N�g�V�[�P���X�I��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SEQ_END( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int i;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SEQ_END\n");
#endif DEBUG_OS_PRINT

  //�������Ă��Ȃ��p�[�e�B�N�����������������Ă���
  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    if( bevw->ptc[ i ] )
    {
      EFFVM_DeleteEmitter( bevw->ptc[ i ] );
      bevw->ptc[ i ] = NULL;
    }
    bevw->ptc_no[ i ] = EFFVM_PTCNO_NONE;
  }

  //�������Ă��Ȃ�OBJ���������������Ă���
  for( i = 0 ; i < EFFVM_OBJ_MAX ; i++ )
  {
    if( bevw->obj[ i ] != EFFVM_OBJNO_NONE )
    { 
      BTLV_CLACT_Delete( BTLV_EFFECT_GetCLWK(), bevw->obj[ i ] );
      bevw->obj[ i ] = EFFVM_OBJNO_NONE;
    }
  }

  //������Ă��Ȃ��e���|�������[�N�����
  for( i = 0 ; i < bevw->temp_work_count ; i++ )
  {
    GFL_HEAP_FreeMemory( bevw->temp_work[ i ] );
  }
  bevw->temp_work_count = 0;

  //���z�}�V����~
  VM_End( vmh );

#ifdef PM_DEBUG
  //�f�o�b�O�t���O�𗎂Ƃ��Ă���
  bevw->debug_flag = FALSE;
#endif

  //BG����̐ݒ���f�t�H���g�ɖ߂��Ă���
  if( bevw->set_priority_flag )
  { 
    GFL_BG_SetPriority( GFL_BG_FRAME3_M, 1 );
    bevw->set_priority_flag = 0;
  }
  if( bevw->set_alpha_flag )
  { 
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, 3 );
    bevw->set_alpha_flag = 0;
  }

  //SUSPEND���[�h�ɐ؂�ւ��Ă���
  bevw->control_mode = VMCMD_RESULT_SUSPEND;

  return VMCMD_RESULT_SUSPEND;
}

//VM_WAIT_FUNC�Q
//============================================================================================
/**
 * @brief �G�t�F�N�g�I���`�F�b�N
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�G�t�F�N�g�I���@FALSE:�G�t�F�N�g���쒆
 */
//============================================================================================
static  BOOL  VWF_EFFECT_END_CHECK( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_CAMERA ) )
  {
    //�J�����ړ��I���H
    if( BTLV_CAMERA_CheckExecute( BTLV_EFFECT_GetCameraWork() ) == TRUE )
    {
      return FALSE;
    }
    if( bevw->camera_ortho_on_flag )
    {
      bevw->camera_ortho_on_flag = 0; //�J�����ړ���A���ˉe�ɖ߂��t���O
      bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;
      BTLV_MCSS_SetOrthoMode( BTLV_EFFECT_GetMcssWork() );
    }
  }
  //�|�P�����ړ��I���H
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_POKEMON ) )
  {
    BtlvMcssPos pos;

    for( pos = 0 ; pos < BTLV_MCSS_POS_TOTAL ; pos++ )
    {
      if( BTLV_MCSS_CheckTCBExecute( BTLV_EFFECT_GetMcssWork(), pos ) == TRUE )
      {
        return FALSE;
      }
    }
  }

  //�g���[�i�[�ړ��I���H
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_TRAINER ) )
  {
    int index;

    for( index = 0 ; index < BTLV_CLACT_CLWK_MAX ; index++ )
    {
      if( BTLV_CLACT_CheckTCBExecute( BTLV_EFFECT_GetCLWK(), index ) == TRUE )
      {
        return FALSE;
      }
    }
  }

  //BG�ړ��I���H
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_BG ) )
  {
    if( BTLV_BG_CheckTCBExecute( BTLV_EFFECT_GetBGWork() ) == TRUE )
    {
      return FALSE;
    }
  }

  //�p�[�e�B�N���`��I���H
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PARTICLE ) )
  {
    int ptc_no;

    for( ptc_no = 0 ; ptc_no < PARTICLE_GLOBAL_MAX ; ptc_no++ )
    {
      if( bevw->ptc[ ptc_no ] != NULL )
      {
        if( GFL_PTC_GetEmitterNum( bevw->ptc[ ptc_no ] ) != 0 )
        {
          return FALSE;
        }
      }
    }
  }
  //�p���b�g�t�F�[�h�I���H
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PALFADE_STAGE ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PALFADE_3D ) )
  { 
    if( BTLV_EFFECT_CheckExecutePaletteFade( BTLEFF_PAL_FADE_STAGE ) )
    { 
      return FALSE;
    }
  }
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PALFADE_FIELD ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PALFADE_3D ) )
  { 
    if( BTLV_EFFECT_CheckExecutePaletteFade( BTLEFF_PAL_FADE_FIELD ) )
    { 
      return FALSE;
    }
  }
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PALFADE_EFFECT ) )
  { 
    if( BTLV_EFFECT_CheckExecutePaletteFade( BTLEFF_PAL_FADE_EFFECT ) )
    { 
      return FALSE;
    }
  }
  //SE�Đ�
//  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
//      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SEALL ) )
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SEALL )
  { 
    if( ( PMSND_CheckPlaySE() ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
    { 
      return FALSE;
    }
  }
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SE1 )
  { 
    if( ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE1 ) ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
    { 
      return FALSE;
    }
  }
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SE2 )
  { 
    if( ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE2 ) ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
    { 
      return FALSE;
    }
  }
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_PSG )
  { 
    if( ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE_PSG ) ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
    { 
      return FALSE;
    }
  }
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SYSTEM )
  { 
    if( ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SYS ) ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
    { 
      return FALSE;
    }
  }

  return TRUE;
}

//============================================================================================
/**
 * @brief �^�C�}�[�E�G�C�g�I���`�F�b�N
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�^�C�}�[�E�G�C�g�I���@FALSE:�^�C�}�[�E�G�C�g���쒆
 */
//============================================================================================
static  BOOL  VWF_WAIT_CHECK( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

  bevw->wait--;

  return ( bevw->wait <= 0 );
}

//����J�֐��Q
//============================================================================================
/**
 * @brief �����ʒu���̎擾
 *
 * @param[in] vmh     ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] pos_flag  �擾�������|�W�V�����t���O
 *
 * @retval  �K�؂ȗ����ʒu
 */
//============================================================================================
static  int   EFFVM_GetPosition( VMHANDLE *vmh, int pos_flag )
{
  int position;
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  switch( pos_flag ){
  case BTLEFF_POKEMON_POS_AA:
  case BTLEFF_POKEMON_POS_BB:
  case BTLEFF_POKEMON_POS_A:
  case BTLEFF_POKEMON_POS_B:
  case BTLEFF_POKEMON_POS_C:
  case BTLEFF_POKEMON_POS_D:
  case BTLEFF_POKEMON_POS_E:
  case BTLEFF_POKEMON_POS_F:
  case BTLEFF_TRAINER_POS_AA:
  case BTLEFF_TRAINER_POS_BB:
  case BTLEFF_TRAINER_POS_A:
  case BTLEFF_TRAINER_POS_B:
  case BTLEFF_TRAINER_POS_C:
  case BTLEFF_TRAINER_POS_D:
    position = pos_flag;
    break;
  case BTLEFF_POKEMON_SIDE_ATTACK:    //�U����
    position = bevw->attack_pos;
    break;
  case BTLEFF_POKEMON_SIDE_ATTACK_PAIR: //�U�����y�A
    if( bevw->attack_pos > BTLV_MCSS_POS_BB ){
      position = bevw->attack_pos ^ BTLV_MCSS_POS_PAIR_BIT;
    }
    else{
      position = BTLV_MCSS_POS_ERROR;
    }
    break;
  case BTLEFF_POKEMON_SIDE_DEFENCE:   //�h�䑤
    position = bevw->defence_pos;
    if( position == BTLV_MCSS_POS_ERROR )
    { 
      if( bevw->attack_pos & 1 )
      { 
        if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_AA ) == TRUE )
        { 
          return BTLV_MCSS_POS_AA;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_A ) == TRUE )
        { 
          return BTLV_MCSS_POS_A;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_C ) == TRUE )
        { 
          return BTLV_MCSS_POS_C;
        }
        else
        { 
          return BTLV_MCSS_POS_E;
        }
      }
      else
      { 
        if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_BB ) == TRUE )
        { 
          return BTLV_MCSS_POS_BB;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_B ) == TRUE )
        { 
          return BTLV_MCSS_POS_B;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_D ) == TRUE )
        { 
          return BTLV_MCSS_POS_D;
        }
        else
        { 
          return BTLV_MCSS_POS_F;
        }
      }
    }
    break;
  case BTLEFF_POKEMON_SIDE_DEFENCE_PAIR:  //�h�䑤�y�A
    if( bevw->defence_pos > BTLV_MCSS_POS_BB ){
      position = bevw->defence_pos ^ BTLV_MCSS_POS_PAIR_BIT;
    }
    else{
      position = BTLV_MCSS_POS_ERROR;
    }
    break;
  default:
    OS_TPrintf("��`����Ă��Ȃ��t���O���w�肳��Ă��܂�\n");
    GF_ASSERT( 0 );
    break;
  }

  if( ( position != BTLV_MCSS_POS_ERROR ) && ( position < BTLV_MCSS_POS_MAX ) )
  {
    if( BTLV_EFFECT_CheckExist( position ) == TRUE )
    {
      position = EFFVM_ConvPosition( vmh, position );
    }
    else
    {
      position = BTLV_MCSS_POS_ERROR;
    }
  }

  return position;
}

//============================================================================================
/**
 * @brief �����ʒu���̕ϊ��i���]�t���O�����ēK�؂ȗ����ʒu��Ԃ��j
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval  �K�؂ȗ����ʒu
 */
//============================================================================================
static  int   EFFVM_ConvPosition( VMHANDLE *vmh, BtlvMcssPos position )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  if( bevw->position_reverse_flag )
  {
    position ^= 1;
  }

  return position;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N����datID��o�^
 *
 * @param[in] bevw  �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in] datID �A�[�J�C�udatID
 *
 * @retval  �o�^����ptc�z��̓Y����No
 */
//============================================================================================
static  int EFFVM_RegistPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID )
{
  int i;

  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    if( bevw->ptc_no[ i ] == datID )
    {
      break;
    }
  }

  if( i == PARTICLE_GLOBAL_MAX )
  {
    for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
    {
      if( bevw->ptc_no[ i ] == EFFVM_PTCNO_NONE )
      {
        bevw->ptc_no[ i ] = datID;
        break;
      }
    }
  }

  GF_ASSERT( i != PARTICLE_GLOBAL_MAX );

  return i;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N����datID����ptc�z��̓Y����No���擾
 *
 * @param[in] bevw  �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in] datID �A�[�J�C�udatID
 *
 * @retval  ptc�z��̓Y����No
 */
//============================================================================================
static  int EFFVM_GetPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID )
{
  int i;

  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    if( bevw->ptc_no[ i ] == datID )
    {
      break;
    }
  }

  GF_ASSERT( i != PARTICLE_GLOBAL_MAX );

  return i;
}

//============================================================================================
/**
 * @brief �G�~�b�^�������ɌĂ΂��G�~�b�^�������p�R�[���o�b�N�֐��i�����ʒu����v�Z�j
 *
 * @param[in] emit  �G�~�b�^���[�N�\���̂ւ̃|�C���^
 */
//============================================================================================
static  void  EFFVM_InitEmitterPos( GFL_EMIT_PTR emit )
{
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = ( BTLV_EFFVM_EMIT_INIT_WORK* )GFL_PTC_GetTempPtr();
  VecFx32 src,dst;
  BOOL  minus_flag = FALSE;

  switch( beeiw->src ){
  case BTLEFF_PARTICLE_PLAY_POS_AA:
  case BTLEFF_PARTICLE_PLAY_POS_BB:
  case BTLEFF_PARTICLE_PLAY_POS_A:
  case BTLEFF_PARTICLE_PLAY_POS_B:
  case BTLEFF_PARTICLE_PLAY_POS_C:
  case BTLEFF_PARTICLE_PLAY_POS_D:
  case BTLEFF_PARTICLE_PLAY_POS_E:
  case BTLEFF_PARTICLE_PLAY_POS_F:
    beeiw->src = EFFVM_ConvPosition( beeiw->vmh, beeiw->src );
    break;
  case BTLEFF_PARTICLE_PLAY_SIDE_ATTACK:
  case BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE:
    beeiw->src = EFFVM_GetPosition( beeiw->vmh, beeiw->src - BTLEFF_PARTICLE_PLAY_SIDE_ATTACK + BTLEFF_POKEMON_SIDE_ATTACK);
    GF_ASSERT( beeiw->src != BTLV_MCSS_POS_ERROR );
    break;
  case BTLEFF_PARTICLE_PLAY_SIDE_NONE:
    src.x = beeiw->src_pos.x;
    src.y = beeiw->src_pos.y;
    src.z = beeiw->src_pos.z;
    break;
  }

  if( ( beeiw->src != BTLEFF_CAMERA_POS_NONE ) && ( beeiw->src != BTLEFF_PARTICLE_PLAY_SIDE_NONE ) )
  {
    BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &src, beeiw->src );
  }

  switch( beeiw->dst ){
  case BTLEFF_PARTICLE_PLAY_POS_AA:
  case BTLEFF_PARTICLE_PLAY_POS_BB:
  case BTLEFF_PARTICLE_PLAY_POS_A:
  case BTLEFF_PARTICLE_PLAY_POS_B:
  case BTLEFF_PARTICLE_PLAY_POS_C:
  case BTLEFF_PARTICLE_PLAY_POS_D:
  case BTLEFF_PARTICLE_PLAY_POS_E:
  case BTLEFF_PARTICLE_PLAY_POS_F:
    beeiw->dst = EFFVM_ConvPosition( beeiw->vmh, beeiw->dst );
    break;
  case BTLEFF_PARTICLE_PLAY_SIDE_ATTACK:
  case BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE:
    beeiw->dst = EFFVM_GetPosition( beeiw->vmh, beeiw->dst - BTLEFF_PARTICLE_PLAY_SIDE_ATTACK + BTLEFF_POKEMON_SIDE_ATTACK );
    GF_ASSERT( beeiw->dst != BTLV_MCSS_POS_ERROR );
    break;
  case BTLEFF_PARTICLE_PLAY_SIDE_ATTACK_MINUS:
  case BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS:
    minus_flag = TRUE;
    beeiw->dst = EFFVM_GetPosition( beeiw->vmh, beeiw->dst - BTLEFF_PARTICLE_PLAY_SIDE_ATTACK + BTLEFF_POKEMON_SIDE_ATTACK - 1 );
    GF_ASSERT( beeiw->dst != BTLV_MCSS_POS_ERROR );
    break;
  case BTLEFF_PARTICLE_PLAY_SIDE_NONE:
    dst.x = beeiw->dst_pos.x;
    dst.y = beeiw->dst_pos.y;
    dst.z = beeiw->dst_pos.z;
    break;
  }

  if( ( beeiw->dst != BTLEFF_CAMERA_POS_NONE ) && ( beeiw->dst != BTLEFF_PARTICLE_PLAY_SIDE_NONE ) )
  {
    BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &dst, beeiw->dst );
  }

  if( beeiw->ortho_mode )
  {
    EFFVM_CalcPosOrtho( &src, &beeiw->ofs );
  }
  else
  { 
    src.y += beeiw->ofs.y;
  }

  dst.y += beeiw->ofs.y;

  if( beeiw->move_type )
  {
    BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( beeiw->vmh );
    BTLV_EFFVM_EMITTER_MOVE_WORK  *beemw;
    VecFx32 rot_axis,line_vec,std_vec;
    u16     angle;

    bevw->temp_work[ bevw->temp_work_count ] = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMITTER_MOVE_WORK ) );
    beemw = ( BTLV_EFFVM_EMITTER_MOVE_WORK *)bevw->temp_work[ bevw->temp_work_count ];
    //�G�~�b�^�Ƀe���|�������[�N��ݒ�
    GFL_PTC_SetUserData( emit, bevw->temp_work[ bevw->temp_work_count ] );
    //�G�~�b�^�ɃR�[���o�b�N�֐���ݒ�
    GFL_PTC_SetCallbackFunc( emit, &EFFVM_MoveEmitter );
    bevw->temp_work_count++;

    //�T�C�Y�I�[�o�[�̃A�T�[�g
    GF_ASSERT( bevw->temp_work_count < TEMP_WORK_SIZE );

    beemw->angle = 0;
    beemw->wait = 0;
    beemw->wait_tmp = 0;
    beemw->wait_tmp_plus = 0;
    beemw->move_type = beeiw->move_type;
    beemw->move_frame = beeiw->move_frame >> FX32_SHIFT;
    if( beeiw->move_type == BTLEFF_EMITTER_MOVE_CURVE_HALF )
    { 
      beemw->speed = FX_Div( ( 0x6000 << FX32_SHIFT ), beeiw->move_frame );
    }
    else
    { 
      beemw->speed = FX_Div( ( 0x8000 << FX32_SHIFT ), beeiw->move_frame );
    }
    beemw->radius_x = VEC_Distance( &src, &dst ) / 2;
    if( beeiw->move_type == BTLEFF_EMITTER_MOVE_STRAIGHT )
    {
      beemw->radius_y = 0;
    }
    else
    {
      beemw->radius_y = beeiw->top;
    }

    line_vec.x = dst.x - src.x;
    line_vec.y = dst.y - src.y;
    line_vec.z = dst.z - src.z;
    VEC_Normalize( &line_vec, &line_vec );

    std_vec.x = FX32_ONE;
    std_vec.y = 0;
    std_vec.z = 0;

    //���ς����߂Ċp�x���o��
    angle = FX_AcosIdx( VEC_DotProduct( &std_vec, &line_vec ) );

    //�O�ς����߂ĉ�]�����o��
    VEC_CrossProduct( &std_vec, &line_vec, &rot_axis );
    VEC_Normalize( &rot_axis, &rot_axis );

    //��]�s��𐶐�����
    MTX_RotAxis43( &beemw->mtx43, &rot_axis, FX_SinIdx( angle ), FX_CosIdx( angle ) );

    //���s�ړ��s��𐶐�
    beemw->mtx43._30 = src.x;
    beemw->mtx43._31 = src.y;
    beemw->mtx43._32 = src.z;
  }

  //src��dst���ꏏ�̂Ƃ��́A�����Ȃ�
  if( beeiw->src != beeiw->dst )
  {
    MtxFx43 mtx43;
    VecFx32 src_vec, dst_vec, rot_axis;
    VecFx16 dir;
    u16     angle;
    u16     spin_axis = SPL_FLD_SPIN_AXIS_TYPE_Z;

    dst.x -= src.x;
    dst.y -= src.y;
    dst.z -= src.z;

    //�}�O�l�b�g�ƃR���o�[�W�F���X�̍��W��dst����
    GFL_PTC_SetEmitterMagnetPos( emit, &dst );
    GFL_PTC_SetEmitterConvergencePos( emit, &dst );

    dst_vec.x = dst.x;
    dst_vec.y = 0;
    dst_vec.z = dst.z;

    GFL_PTC_GetEmitterAxis( emit, &dir );
    VEC_Set( &src_vec, dir.x, 0, dir.z );

    VEC_Normalize( &src_vec, &src_vec );
    VEC_Normalize( &dst_vec, &dst_vec );

    //���ς����߂Ċp�x���o��
    angle = FX_AcosIdx( VEC_DotProduct( &src_vec, &dst_vec ) );

    //�O�ς����߂ĉ�]�����o��
    VEC_CrossProduct( &src_vec, &dst_vec, &rot_axis );
    VEC_Normalize( &rot_axis, &rot_axis );

    //��]�s��𐶐�����
    MTX_RotAxis43( &mtx43, &rot_axis, FX_SinIdx( angle ), FX_CosIdx( angle ) );

    VEC_Set( &src_vec, dir.x, dir.y, dir.z );
    MTX_MultVec43( &src_vec, &mtx43, &src_vec );
    VEC_Normalize( &src_vec, &src_vec );
    if( minus_flag == TRUE )
    { 
      src_vec.x *= -1;
      src_vec.z *= -1;
    }
    VEC_Fx16Set( &dir, src_vec.x, src_vec.y, src_vec.z );

    if( angle < 0x2000 || ( angle > 0x6000 && angle < 0xa000 ) || angle > 0xe000 )
    { 
      spin_axis = SPL_FLD_SPIN_AXIS_TYPE_X;
    }
    GFL_PTC_SetEmitterSpinAxisType( emit, &spin_axis );

    GFL_PTC_SetEmitterAxis( emit, &dir );
  }

  GFL_PTC_SetEmitterPosition( emit, &src );

  GFL_HEAP_FreeMemory( beeiw );
}

//============================================================================================
/**
 * @brief �G�~�b�^�ړ��p�R�[���o�b�N�֐�
 *
 * @param[in] emit  �G�~�b�^���[�N�\���̂ւ̃|�C���^
 * @param[in] flag  �R�[���o�b�N���Ă΂ꂽ�^�C�~���O�������t���O
 *                  SPL_EMITTER_CALLBACK_FRONT:�G�~�b�^�v�Z���s�����O
 *                  SPL_EMITTER_CALLBACK_BACK:�G�~�b�^�v�Z���s��������
 */
//============================================================================================
static  void  EFFVM_MoveEmitter( GFL_EMIT_PTR emit, unsigned int flag )
{
  BTLV_EFFVM_EMITTER_MOVE_WORK *beemw = ( BTLV_EFFVM_EMITTER_MOVE_WORK *)GFL_PTC_GetUserData( emit );
  VecFx32 emit_pos;
  int     angle;

  if( ( beemw->move_frame == 0 ) || ( flag == SPL_EMITTER_CALLBACK_FRONT ) )
  {
    return;
  }

  if( beemw->move_type == BTLEFF_EMITTER_MOVE_CURVE_HALF )
  { 
    if( beemw->wait )
    { 
      beemw->wait--;
      return;
    }
    else
    { 
      beemw->wait_tmp_plus += 0x0c;
      beemw->wait_tmp += beemw->wait_tmp_plus;
      beemw->wait = ( beemw->wait_tmp >> FX32_SHIFT );
    }
  }

  beemw->move_frame--;
  beemw->angle += beemw->speed;

  angle = ( ( beemw->angle >> FX32_SHIFT ) + 0xc000 ) & 0xffff;

  emit_pos.x = FX_SinIdx( angle );
  emit_pos.x = FX_Mul( emit_pos.x, beemw->radius_x );
  emit_pos.x += beemw->radius_x;

  emit_pos.y = FX_CosIdx( angle );
  emit_pos.y = FX_Mul( emit_pos.y, beemw->radius_y );

  emit_pos.z = 0;

  MTX_MultVec43( &emit_pos, &beemw->mtx43, &emit_pos );

  GFL_PTC_SetEmitterPosition( emit, &emit_pos );
}

//============================================================================================
/**
 * @brief �G�~�b�^�������ɌĂ΂��G�~�b�^�������p�R�[���o�b�N�֐��i�G�~�b�^�~�ړ��j
 *
 * @param[in] emit  �G�~�b�^���[�N�\���̂ւ̃|�C���^
 */
//============================================================================================
static  void  EFFVM_InitEmitterCircleMove( GFL_EMIT_PTR emit )
{ 
  BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK *beecmw = ( BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK* )GFL_PTC_GetTempPtr();

  //�G�~�b�^�Ƀe���|�������[�N��ݒ�
  GFL_PTC_SetUserData( emit, beecmw );
  //�G�~�b�^�ɃR�[���o�b�N�֐���ݒ�
  GFL_PTC_SetCallbackFunc( emit, &EFFVM_CircleMoveEmitter );
}

//============================================================================================
/**
 * @brief �G�~�b�^�~�ړ��p�R�[���o�b�N�֐�
 *
 * @param[in] emit  �G�~�b�^���[�N�\���̂ւ̃|�C���^
 * @param[in] flag  �R�[���o�b�N���Ă΂ꂽ�^�C�~���O�������t���O
 *                  SPL_EMITTER_CALLBACK_FRONT:�G�~�b�^�v�Z���s�����O
 *                  SPL_EMITTER_CALLBACK_BACK:�G�~�b�^�v�Z���s��������
 */
//============================================================================================
static  void  EFFVM_CircleMoveEmitter( GFL_EMIT_PTR emit, unsigned int flag )
{
  BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK *beecmw = ( BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK *)GFL_PTC_GetUserData( emit );
  VecFx32 emit_pos;

  if( flag == SPL_EMITTER_CALLBACK_FRONT )
  {
    return;
  }

  if( beecmw->after_wait == 0 )
  { 
    if( beecmw->wait == 0 )
    { 
      beecmw->wait = beecmw->wait_tmp;
      if( beecmw->center & 1 )
      { 
        beecmw->angle -= beecmw->speed;
      }
      else
      { 
        beecmw->angle += beecmw->speed;
      }
  
      beecmw->angle &= 0xffff;
  
      emit_pos.x = FX_SinIdx( beecmw->angle );
      emit_pos.x = FX_Mul( emit_pos.x, beecmw->radius_h );
      emit_pos.x += beecmw->center_pos.x;
  
      emit_pos.y = beecmw->center_pos.y;
  
      emit_pos.z = FX_CosIdx( beecmw->angle );
      emit_pos.z = FX_Mul( emit_pos.z, beecmw->radius_v );
      emit_pos.z += beecmw->center_pos.z;

      if( beecmw->ortho_mode )
      { 
        EFFVM_CalcPosOrtho( &emit_pos, NULL );
      }

      GFL_PTC_SetEmitterPosition( emit, &emit_pos );
  
      if( beecmw->frame )
      { 
        beecmw->frame--;
      }
      else 
      { 
        beecmw->frame = beecmw->frame_tmp;
        beecmw->after_wait = beecmw->after_wait_tmp;
      }
    }
    else
    { 
      beecmw->wait--;
    }
  }
  else
  { 
    beecmw->after_wait--;
  }
}

//============================================================================================
/**
 * @brief �G�~�b�^�폜
 *
 * @param[in] ptc   �폜����G�~�b�^��GFL_PTC_PTR�\����
 */
//============================================================================================
static  void  EFFVM_DeleteEmitter( GFL_PTC_PTR ptc )
{
  void  *heap;

  heap = GFL_PTC_GetHeapPtr( ptc );
  GFL_HEAP_FreeMemory( heap );
  GFL_PTC_Delete( ptc );
}

//============================================================================================
/**
 * @brief �J�����̎ˉe���[�h��ύX
 *
 * @param[in] bevw  �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 */
//============================================================================================
static  void  EFFVM_ChangeCameraProjection( BTLV_EFFVM_WORK *bevw )
{
  if( bevw->camera_projection == BTLEFF_CAMERA_PROJECTION_PERSPECTIVE )
  {
    BTLV_MCSS_ResetOrthoMode( BTLV_EFFECT_GetMcssWork() );
  }
  else
  {
    BTLV_MCSS_SetOrthoMode( BTLV_EFFECT_GetMcssWork() );
  }
}

//============================================================================================
/**
 * @brief SE�Đ�
 *
 * @param[in]	se_no	      �Đ�����SE�i���o�[
 * @param[in] player      �Đ�����PlayerNo
 * @param[in] pitch       �Đ��s�b�`
 * @param[in] vol         �Đ��{�����[��
 * @param[in] mod_depth   �Đ����W�����[�V�����f�v�X
 * @param[in] mod_speed   �Đ����W�����[�V�����X�s�[�h
 */
//============================================================================================
static  void  EFFVM_SePlay( int se_no, int player, int pitch, int vol, int mod_depth, int mod_speed )
{ 
  if( player == BTLEFF_SEPLAY_DEFAULT )
  { 
    PMSND_PlaySE( se_no );
    player = PMSND_GetSE_DefaultPlayerID( se_no );
  }
  else
  { 
    PMSND_PlaySE_byPlayerID( se_no, player );
  }
	NNS_SndPlayerSetVolume( PMSND_GetSE_SndHandle( player ), vol );
  PMSND_SetStatusSE_byPlayerID( player, PMSND_NOEFFECT, pitch, PMSND_NOEFFECT );
  if( mod_depth != 0 )
  { 
    NNS_SndPlayerSetTrackModDepth( PMSND_GetSE_SndHandle( player ), 0xffff, mod_depth );
    NNS_SndPlayerSetTrackModSpeed( PMSND_GetSE_SndHandle( player ), 0xffff, mod_speed );
  }
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ�p�����[�^�̃��[�N��Ԃ�
 *
 * @param[in] bevw  �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in] param �擾�������p�����[�^
 */
//============================================================================================
static  int  EFFVM_GetWork( BTLV_EFFVM_WORK* bevw, int param )
{ 
  int ret;

  switch( param ){ 
  case BTLEFF_WORK_WAZA_RANGE:    ///<�Z�̌��ʔ͈�
    ret = bevw->param.waza_range;
    break;
  case BTLEFF_WORK_TURN_COUNT:    ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
    ret = bevw->param.turn_count;
    break;
  case BTLEFF_WORK_CONTINUE_COUNT:   ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
    ret = bevw->param.continue_count;
    break;
  case BTLEFF_WORK_YURE_CNT:   ///<�{�[������J�E���g
    ret = bevw->param.yure_cnt;
    break;
  case BTLEFF_WORK_GET_SUCCESS:   ///<�ߊl�������ǂ���
    ret = bevw->param.get_success;
    break;
  case BTLEFF_WORK_ITEM_NO:   ///<�{�[���̃A�C�e���i���o�[
    ret = bevw->param.item_no;
    break;
  }
  return ret;
}

//============================================================================================
/**
 * @brief �G�~�b�^�~�^��������
 *
 * @param[in] vmh           ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 * @param[in] ortho_mode    ���ˉe���[�h���ǂ���(FALSE:�����ˉe TRUE:���ˉe�j
 */
//============================================================================================
static VMCMD_RESULT EFFVM_INIT_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work, BOOL ortho_mode )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK *beecmw;
  ARCDATID  datID   = ( ARCDATID )VMGetU32( vmh );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       position = 0;
  fx32      offset_y;

  if( ortho_mode == TRUE )
  { 
    GFL_G3D_PROJECTION  proj;
	  VecFx32 Eye    = { 0, 0, 0 };          // Eye position
	  VecFx32 vUp    = { 0, FX32_ONE, 0 };  // Up
	  VecFx32 at     = { 0, 0, -FX32_ONE }; // Viewpoint

    proj.type = GFL_G3D_PRJORTH;
    proj.param1 = FX32_CONST( ORTHO_HEIGHT );
    proj.param2 = -FX32_CONST( ORTHO_HEIGHT );
    proj.param3 = -FX32_CONST( ORTHO_WIDTH );
    proj.param4 = FX32_CONST( ORTHO_WIDTH );
	  proj.near		= FX32_ONE * 1;
    proj.far    = FX32_ONE * 1024;
	  proj.scaleW	= FX32_ONE;

    GFL_PTC_PersonalCameraCreate( bevw->ptc[ ptc_no ], &proj, DEFAULT_PERSP_WAY, &Eye, &vUp, &at, bevw->heapID );
  }

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_CIRCLE_MOVE\n");
#endif DEBUG_OS_PRINT

  bevw->temp_work[ bevw->temp_work_count ] = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK ) );
  beecmw = ( BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK *)bevw->temp_work[ bevw->temp_work_count ];
  bevw->temp_work_count++;

  //�T�C�Y�I�[�o�[�̃A�T�[�g
  GF_ASSERT( bevw->temp_work_count < TEMP_WORK_SIZE );

  beecmw->center          = ( int )VMGetU32( vmh );
  beecmw->radius_h        = ( fx32 )VMGetU32( vmh );
  beecmw->radius_v        = ( fx32 )VMGetU32( vmh );
  offset_y                = ( fx32 )VMGetU32( vmh );
  beecmw->angle           = 0;
  beecmw->frame           = ( int )VMGetU32( vmh );
  beecmw->frame_tmp       = beecmw->frame;
  beecmw->wait            = 0;
  beecmw->wait_tmp        = ( int )VMGetU32( vmh );
  beecmw->count           = ( int )VMGetU32( vmh );
  beecmw->after_wait      = 0;
  beecmw->after_wait_tmp  = ( int )VMGetU32( vmh );

  beecmw->speed = 0x10000 / beecmw->frame;

  beecmw->ortho_mode = ortho_mode;

  switch( beecmw->center ){ 
  case BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_L:
  case BTLEFF_EMITTER_CIRCLE_MOVE_ATTACK_R:
    position = EFFVM_GetPosition( vmh, BTLEFF_POKEMON_SIDE_ATTACK );
    BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &beecmw->center_pos, position );
    break;
  case BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_L:
  case BTLEFF_EMITTER_CIRCLE_MOVE_DEFENCE_R:
    position = EFFVM_GetPosition( vmh, BTLEFF_POKEMON_SIDE_DEFENCE );
    BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &beecmw->center_pos, position );
    break;
  case BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_L:
  case BTLEFF_EMITTER_CIRCLE_MOVE_CENTER_R:
    beecmw->center_pos.x = 0;
    beecmw->center_pos.y = 0;
    beecmw->center_pos.z = 0;
    break;
  }

  if( position & 1 )
  { 
    beecmw->angle = 0x8000;
  }

  beecmw->center_pos.y += offset_y;

  GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterCircleMove, beecmw );

  return bevw->control_mode;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ˉe�ł̍��W���琳�ˉe�ł̍��W�ɕϊ�����
 *
 *	@param[in/out]  pos ���W�ϊ����s�����W
 *	@param[in]      ofs �I�t�Z�b�g���W�iNULL�̏ꍇ�̓I�t�Z�b�g�v�Z�����Ȃ��j
 */
//-----------------------------------------------------------------------------
static  void  EFFVM_CalcPosOrtho( VecFx32 *pos, VecFx32 *ofs )
{ 
  MtxFx44	mtx;
	fx32	w;

	MTX_Copy43To44( NNS_G3dGlbGetCameraMtx(), &mtx );
	MTX_Concat44( &mtx, NNS_G3dGlbGetProjectionMtx(), &mtx );

  if( ofs )
  { 
    pos->x += ofs->x;
    pos->y += ofs->y;
    pos->z += ofs->z;
  }

	MTX_MultVec44( pos, &mtx, pos, &w );

	pos->x = FX_Mul( FX_Div( pos->x, w ), ORTHO_WIDTH * FX32_ONE );
	pos->y = FX_Mul( FX_Div( pos->y, w ), ORTHO_HEIGHT * FX32_ONE );
  pos->z = -pos->z;
}

//�_��峂��炢������
//----------------------------------------------------------------------------
/**
 *	@brief	4x4�s��ɍ��W���|�����킹��
 *			Vec��x,y,z,1�Ƃ��Čv�Z���A�v�Z���Vec��w��Ԃ��܂��B
 *
 *	@param	*cp_src	Vector���W
 *	@param	*cp_m	4*4�s��
 *	@param	*p_dst	Vecror�v�Z����
 *	@param	*p_w	4�ڂ̗v�f�̒l
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static	void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w )
{

	fx32 x = cp_src->x;
    fx32 y = cp_src->y;
    fx32 z = cp_src->z;
	fx32 w = FX32_ONE;

	GF_ASSERT( cp_src );
	GF_ASSERT( cp_m );
	GF_ASSERT( p_dst );
	GF_ASSERT( p_w );

    p_dst->x = (fx32)(((fx64)x * cp_m->_00 + (fx64)y * cp_m->_10 + (fx64)z * cp_m->_20) >> FX32_SHIFT);
    p_dst->x += cp_m->_30;	//	W=1�Ȃ̂ő�������

    p_dst->y = (fx32)(((fx64)x * cp_m->_01 + (fx64)y * cp_m->_11 + (fx64)z * cp_m->_21) >> FX32_SHIFT);
    p_dst->y += cp_m->_31;//	W=1�Ȃ̂ő�������

    p_dst->z = (fx32)(((fx64)x * cp_m->_02 + (fx64)y * cp_m->_12 + (fx64)z * cp_m->_22) >> FX32_SHIFT);
    p_dst->z += cp_m->_32;//	W=1�Ȃ̂ő�������

	*p_w	= (fx32)(((fx64)x * cp_m->_03 + (fx64)y * cp_m->_13 + (fx64)z * cp_m->_23) >> FX32_SHIFT);
    *p_w	+= cp_m->_33;//	W=1�Ȃ̂ő�������
}

//TCB�֐�
//============================================================================================
/**
 * @brief SE�Đ�
 */
//============================================================================================
static  void  TCB_EFFVM_SEPLAY( GFL_TCB* tcb, void* work )
{ 
  BTLV_EFFVM_SEPLAY*  bes = ( BTLV_EFFVM_SEPLAY* )work;

  if( --bes->wait == 0 )
  { 
    bes->bevw->se_play_wait_flag = 0;
    EFFVM_SePlay( bes->se_no, bes->player, bes->pitch, bes->vol, bes->mod_depth, bes->mod_speed );
    GFL_HEAP_FreeMemory( bes );
    GFL_TCB_DeleteTask( tcb );
  }
}

//============================================================================================
/**
 * @brief SE�G�t�F�N�g
 */
//============================================================================================
static  void  TCB_EFFVM_SEEFFECT( GFL_TCB* tcb, void* work )
{ 
  BTLV_EFFVM_SEEFFECT*  bes = ( BTLV_EFFVM_SEEFFECT* )work;
  BOOL  flag = FALSE;

  if( bes->start_wait )
  { 
    bes->start_wait--;
    return;
  }

  if( bes->wait == 0 )
  { 
    int value;

    bes->wait = bes->wait_tmp;
    bes->value += bes->vec_value;
    value = bes->value >> FX32_SHIFT;

    if( bes->count & 1 )
    { 
      if( bes->vec_value < 0 )
      { 
        if( value < bes->start )
        { 
          value = bes->start;
        }
      }
      else
      { 
        if( value > bes->start )
        { 
          value = bes->start;
        }
      }
    }
    else
    { 
      if( bes->vec_value < 0 )
      { 
        if( value < bes->end )
        { 
          value = bes->end;
        }
      }
      else
      { 
        if( value > bes->end )
        { 
          value = bes->end;
        }
      }
    }
    bes->value = FX32_CONST( value );

    switch( bes->param )
    { 
    case BTLEFF_SEEFFECT_PITCH:
      PMSND_SetStatusSE_byPlayerID( bes->player, PMSND_NOEFFECT, value, PMSND_NOEFFECT );
      break;
    case BTLEFF_SEEFFECT_VOLUME:
      NNS_SndPlayerSetVolume( PMSND_GetSE_SndHandle( bes->player ), value );
      break;
    case BTLEFF_SEEFFECT_PAN:
      PMSND_SetStatusSE_byPlayerID( bes->player, PMSND_NOEFFECT, PMSND_NOEFFECT, value );
      break;
    }
    if( bes->frame == 0 )
    { 
      bes->frame = bes->frame_tmp;
      switch( bes->type ) { 
      case BTLEFF_SEEFFECT_INTERPOLATION:
        flag = TRUE;
        break;
      case BTLEFF_SEEFFECT_ROUNDTRIP:
        if( --bes->count )
        { 
          bes->vec_value *= -1;
        }
        else
        { 
          flag = TRUE;
        }
        break;
      }
    }
    else
    { 
      bes->frame--;
    }
  }
  else
  { 
    bes->wait--;
  }

  if( flag == TRUE )
  { 
    bes->bevw->se_effect_enable_flag = 0;
    GFL_HEAP_FreeMemory( bes );
    GFL_TCB_DeleteTask( tcb );
  }
}

#ifdef PM_DEBUG

//�f�o�b�O�p�֐�
//============================================================================================
/**
 * @brief VM�N���i�f�o�b�O�p�A�h���X�w��o�[�W�����j
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_StartDebug( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, const VM_CODE *start, const DEBUG_PARTICLE_DATA *dpd, BTLV_EFFVM_PARAM *param )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  int *start_ofs = (int *)&start[ script_table[ from ][ to ] ] ;
  int i;

  if( param != NULL )
  { 
    bevw->param = *param;
  }

  bevw->dat_id_cnt = 0;
  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    bevw->dat_id[ i ] = 0xffffffff;
  }

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG0,
                    GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                    GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                    0, 0 );

  bevw->attack_pos = from;
  bevw->defence_pos = to;
  bevw->dpd = dpd;
  bevw->debug_flag = TRUE;
  bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;
  VM_Start( vmh, &start[ start_ofs[ 0 ] ] );
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Đ��i�f�o�b�O�p�j
 *
 * @param[in] vmh   ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] ptc   GFL_PTC_PTR
 * @param[in] index �Đ�����G�~�b�^�ԍ�
 * @param[in] src   �U����
 * @param[in] dst   �h�䑤
 * @param[in] ofs_y �G�~�b�^�̍��������l
 * @param[in] angle �G�~�b�^�̃p�[�e�B�N���ˏo�p�x
 *
 */
//============================================================================================
void  BTLV_EFFVM_DebugParticlePlay( VMHANDLE *vmh, GFL_PTC_PTR ptc, int index, int src, int dst, fx32 ofs_y, fx32 angle )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( bevw->heapID, sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );

  beeiw->vmh = vmh;
  beeiw->src = src;
  beeiw->dst = dst;
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ofs_y;
  beeiw->ofs.z = 0;
  beeiw->angle = angle;

  GFL_PTC_CreateEmitterCallback( ptc, index, &EFFVM_InitEmitterPos, beeiw );
}

//============================================================================================
/**
 * @brief SE���I�ω��i�f�o�b�O�p�j
 *
 * @param[in] vmh         ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] player      �ω�������PlayerNo
 * @param[in] type        �ω��^�C�v
 * @param[in] param       �ω�������p�����[�^
 * @param[in] start       �J�n�l
 * @param[in] end         �I���l
 * @param[in] start_wait  �J�n�E�G�C�g
 * @param[in] frame       �J�n�l����I���l�ɕω�����܂ł̃t���[����
 * @param[in] wait        1�t���[�����Ƃ̃E�G�C�g
 * @param[in] count       �ω��^�C�v�������������Ƃ��̉�����
 *
 */
//============================================================================================
void  BTLV_EFFVM_DebugSeEffect( VMHANDLE *vmh, int player, int type, int param, int start, int end, int start_wait, int frame, int wait, int count )
{ 
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTLV_EFFVM_SEEFFECT ) );

  bes->bevw     = bevw;

  bes->player     = player;
  bes->type       = type;
  bes->param      = param;
  bes->start      = start;
  bes->end        = end;
  bes->start_wait = start_wait;
  bes->frame      = frame;
  bes->frame_tmp  = bes->frame;
  bes->wait       = 0;
  bes->wait_tmp   = wait;
  bes->count      = count * 2;

  if( ( bes->type == BTLEFF_SEEFFECT_ROUNDTRIP ) && ( bes->count == 0 ) )
  { 
    bes->count = 2;
  }

	bes->value = FX32_CONST( bes->start );
	bes->vec_value = FX_Div( FX32_CONST( bes->end - bes->start ) , FX32_CONST( bes->frame ) );

  GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 );
}

//============================================================================================
/**
 * @brief DEBUG_PARTICLE_DATA�̔z��̓Y������ARCDATID����擾����
 *
 * @param[in] bevw  �G�t�F�N�g�Ǘ��\���̂ւ̃|�C���^
 * @param[in] datID �擾����ARCDATID
 * @param[in] type  �ǂݍ���ARCDATID�̃^�C�v�i�^�C�v�ɂ���āA�ǂݍ��ރt�@�C�������Ⴄ�j
 */
//============================================================================================
static  u32   BTLV_EFFVM_GetDPDNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID, DPD_TYPE type )
{
  int i;

  for( i = 0 ; i < bevw->dat_id_cnt ; i++ )
  {
    if( bevw->dat_id[ i ] == datID )
    {
      break;
    }
  }
  if( i == bevw->dat_id_cnt )
  {
    GF_ASSERT( bevw->dat_id_cnt < PARTICLE_GLOBAL_MAX );
    switch( type ){ 
    case DPD_TYPE_PARTICLE:
      bevw->dat_id[ i ] = datID;
      bevw->dat_id_cnt++;
      break;
    case DPD_TYPE_BG:
      { 
        int j;
        for( j = 0 ; j < 3 ; j++ )
        { 
          bevw->dat_id[ i + j ] = datID;
          bevw->dat_id_cnt++;
        }
      }
      break;
    }
  }
  return i;
}

//============================================================================================
/**
 * @brief ���\�[�X�A���p�b�N���N���A
 *        NCGR�n�̓A���p�b�N��1�񂵂����Ă͂����Ȃ��̂ŁA�A���p�b�N����ێ����Ă���
 *        ���\�[�X��ǂݍ��񂾃^�C�~���O�ŃA���p�b�N�����N���A����
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_ClearUnpackInfo( VMHANDLE *vmh )
{ 
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  int i;

  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    bevw->unpack_info[ i ] = NULL;
  }
}

#endif PM_DEBUG

