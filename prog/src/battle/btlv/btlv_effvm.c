
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
#include "item/item.h"

#include "net/dwc_rap.h"

#include "btlv_scu.h"
#include "btlv_effect.h"

#include "btlv_effvm_dat.h"

#include "arc_def.h"
#include "particle/wazaeffect/spa.naix"
#include "wazaeffect/waza_eff_gra.naix"

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

  EFFVM_PTCNO_NO_FIND = PARTICLE_GLOBAL_MAX,

  ORTHO_WIDTH = 4,
  ORTHO_HEIGHT = 3,

  EFFVM_CHANGE_VOLUME = ( 127 * EFFVM_VOLUME_DOWN_RATIO / 100 ) << FX32_SHIFT,
  EFFVM_CHANGE_VOLUME_DOWN_FRAME = EFFVM_VOLUME_DOWN_FRAME,
  EFFVM_CHANGE_VOLUME_UP_FRAME = EFFVM_VOLUME_UP_FRAME,
  EFFVM_CHANGE_VOLUME_PV = ( 127 * EFFVM_VOLUME_DOWN_RATIO_PV / 100 ) << FX32_SHIFT,
  EFFVM_CHANGE_VOLUME_DOWN_FRAME_PV = EFFVM_VOLUME_DOWN_FRAME_PV,
  EFFVM_CHANGE_VOLUME_UP_FRAME_PV = EFFVM_VOLUME_UP_FRAME_PV,

  EFFVM_VOICEPLAYER_INDEX_NONE = 0xffffffff,
};

#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_sogabe
#define DEBUG_OS_PRINT
#endif
#endif

#ifdef PM_DEBUG
#ifdef DEBUG_ONLY_FOR_kageyama_shota
#define KAGEYAMA_DEBUG
vu32  volume_down_ratio = EFFVM_VOLUME_DOWN_RATIO;
vu32  volume_down_frame = EFFVM_CHANGE_VOLUME_DOWN_FRAME;
vu32  volume_up_frame   = EFFVM_CHANGE_VOLUME_UP_FRAME;
vu32  volume_down_ratio_pv = EFFVM_VOLUME_DOWN_RATIO_PV;
vu32  volume_down_frame_pv = EFFVM_CHANGE_VOLUME_DOWN_FRAME_PV;
vu32  volume_up_frame_pv   = EFFVM_CHANGE_VOLUME_UP_FRAME_PV;
#endif
#endif

#ifdef PM_DEBUG
#if defined(DEBUG_ONLY_FOR_kageyama_shota)|\
    defined(DEBUG_ONLY_FOR_adachi_minako)|\
    defined(DEBUG_ONLY_FOR_sato_hitomi)|\
    defined(DEBUG_ONLY_FOR_ichinose)
//#define DEBUG_SE_END_PRINT
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
  u32               pause_flag            :1;     //�ꎞ��~�t���O
  u32               window_move_flag      :1;     //�E�C���h�E����t���O
  u32               volume_down_flag      :1;     //�{�����[���_�E�������ǂ����t���O
  u32               volume_already_down   :1;     //���łɃ{�����[���_�E���������t���O
  u32               nakigoe_wait_flag     :1;     //�����Đ��E�G�C�g��
  u32               se_play_flag          :1;     //SEPLAY���ꂽ���t���O
  u32                                     :19;
  u32               sequence_work;                //�V�[�P���X�Ŏg�p����ėp���[�N

  GFL_TCBSYS*       tcbsys;
  GFL_PTC_PTR       ptc[ PARTICLE_GLOBAL_MAX ];
  int               ptc_no[ PARTICLE_GLOBAL_MAX ];
  int               spr_max[ PARTICLE_GLOBAL_MAX ];
  int               obj[ EFFVM_OBJ_MAX ];
  BtlvMcssPos       attack_pos;
  BtlvMcssPos       defence_pos;
  BtlvMcssPos       push_attack_pos[ BTLV_EFFVM_STACK_SIZE ];     //VMEC_CALL�����s���ꂽ�Ƃ��ɑޔ�����attack_pos
  BtlvMcssPos       push_defence_pos[ BTLV_EFFVM_STACK_SIZE ];    //VMEC_CALL�����s���ꂽ�Ƃ��ɑޔ�����defence_pos
  VM_CODE*          push_sequence[ BTLV_EFFVM_STACK_SIZE ];       //VMEC_CALL�����s���ꂽ�Ƃ��ɑޔ�����sequence
  u32               push_sequence_work[ BTLV_EFFVM_STACK_SIZE ];  //VMEC_CALL�����s���ꂽ�Ƃ��ɑޔ�����sequence_work
  int               wait;
  VM_CODE*          sequence;
  VM_CODE*          migawari_sequence;
  int               migawari_table_ofs;
  void*             temp_work[ TEMP_WORK_SIZE ];
  int               temp_work_count;
  HEAPID            heapID;
  VMCMD_RESULT      control_mode;
  int               effect_end_wait_kind;
  int               camera_projection;
  BTLV_EFFVM_PARAM  param;
  WazaID            waza;
  int               ball_mode;
  int               call_count;   //�T�u���[�`���R�[��������
  int               temp_scr_x;
  int               temp_scr_y;
  u32               voiceplayerIndex[ TEMOTI_POKEMAX ];  //�����v���C���[�C���f�b�N�X�i�O�̂���6�̕��j
#ifdef PM_DEBUG
  const DEBUG_PARTICLE_DATA*  dpd;
  BOOL                        debug_flag;
  BOOL                        debug_return_through;
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
  fx32      radius;
  fx32      life;
  fx32      scale;
  fx32      speed;
  BOOL      minus_flag;
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
  int               pan;
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

//�{�����[���ω�
typedef struct
{
  BTLV_EFFVM_WORK*  bevw;
  fx32              start_vol;
  fx32              end_vol;
  fx32              vec_vol;
}BTLV_EFFVM_CHANGE_VOLUME;

//�����Đ�
typedef struct
{ 
  BTLV_EFFVM_WORK*  bevw;
  BtlvMcssPos       pos;
  int               pitch;
  int               volume;
  int               chorus_vol;
  int               chorus_speed;
  BOOL              play_dir;
  int               wait;
}BTLV_EFFVM_NAKIGOE;

//SPA�f�[�^�iSPA�Ɋ܂܂��spr�̐�����肽���j
typedef struct
{
  u8  head[ 8 ];    //�w�b�_�[�f�[�^
  u16 size;         //spa�Ɋ܂܂��spr�̐��炵��
  u8  data[ 1 ];    //���f�[�^�H�i��肽���̂̓T�C�Y�����Ȃ̂ŁA�Ƃ肠�����P�����j
}SPA_DATA;

//�E�C���h�E����
typedef struct
{
  BTLV_EFFVM_WORK*  bevw;
  int               seq_no;
	int               type;
	int               horizon;
	int               vertical;
  int               in_out;
	int               frame;
	int               wait;
	int               wait_tmp;
  BOOL              flag;
}BTLV_EFFVM_WINDOW_MOVE;

typedef struct
{ 
  BTLV_EFFVM_WORK*  bevw;
}TCB_NONE_WORK;

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
void      BTLV_EFFVM_Restart( VMHANDLE *vmh );

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
static VMCMD_RESULT VMEC_PARTICLE_PLAY_ALL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PARTICLE_DELETE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_MOVE_COORDINATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EMITTER_CIRCLE_MOVE_ORTHO( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SIN_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_SCALE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_ROTATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_ALPHA( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_POKEMON_MOSAIC( VMHANDLE *vmh, void *context_work );
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
static VMCMD_RESULT VMEC_BG_VISIBLE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_WINDOW_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_SCALE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_ANIME_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_PAL_FADE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_OBJ_DEL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_GAUGE_VANISH( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_PLAY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_STOP( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_PAN( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SE_EFFECT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_EFFECT_END_WAIT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_WAIT( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CONTROL_MODE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_IF( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_MCSS_POS_CHECK( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SET_WORK( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_GET_WORK( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_SET_PARAM( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_MIGAWARI( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_HENSHIN( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_NAKIGOE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BALL_MODE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_BALLOBJ_SET( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_CALL( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_RETURN( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_JUMP( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT VMEC_PAUSE( VMHANDLE *vmh, void *context_work );

static VMCMD_RESULT VMEC_SEQ_END( VMHANDLE *vmh, void *context_work );

//VM_WAIT_FUNC�Q
static  BOOL  VWF_EFFECT_END_CHECK( VMHANDLE *vmh, void *context_work );
static  BOOL  VWF_WAIT_CHECK( VMHANDLE *vmh, void *context_work );
static  BOOL  VWF_PAUSE_CHECK( VMHANDLE *vmh, void *context_work );

//����J�֐��Q
static  int           EFFVM_GetPokePosition( BTLV_EFFVM_WORK* bevw, int pos_flag, BtlvMcssPos* pos );
static  int           EFFVM_GetPosition( VMHANDLE *vmh, int pos_flag );
static  int           EFFVM_ConvPokePosition( BTLV_EFFVM_WORK* bevw, BtlvMcssPos position );
static  int           EFFVM_ConvPosition( VMHANDLE *vmh, BtlvMcssPos position );
static  BOOL          EFFVM_RegistPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID, int* ptc_no );
static  void          EFFVM_RegistSprMax( BTLV_EFFVM_WORK *bevw, int ptc_no, void* resource );
static  int           EFFVM_GetPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID );
static  int           EFFVM_GetSprMax( BTLV_EFFVM_WORK *bevw, int ptc_no );
static  void          EFFVM_InitEmitterPos( GFL_EMIT_PTR emit );
static  void          EFFVM_MoveEmitter( GFL_EMIT_PTR emit, unsigned int flag );
static  void          EFFVM_InitEmitterCircleMove( GFL_EMIT_PTR emit );
static  void          EFFVM_CircleMoveEmitter( GFL_EMIT_PTR emit, unsigned int flag );
static  void          EFFVM_DeleteEmitter( GFL_PTC_PTR ptc );
static  void          EFFVM_ChangeCameraProjection( BTLV_EFFVM_WORK *bevw );
static  void          EFFVM_SePlay( int se_no, int player, int pan, int pitch, int vol, int mod_depth, int mod_speed );
static  int           EFFVM_GetWork( BTLV_EFFVM_WORK* bevw, int param );
static  void          EFFVM_INIT_EMITTER_POS( BTLV_EFFVM_WORK* bevw, BTLV_EFFVM_EMIT_INIT_WORK* beeiw_src, int ptc_no, int index );
static  VMCMD_RESULT  EFFVM_INIT_EMITTER_CIRCLE_MOVE( VMHANDLE *vmh, void *context_work, BOOL ortho_mode );
static  void          EFFVM_CalcPosOrtho( VecFx32 *pos, VecFx32 *ofs );
static  void          MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w );
static  ARCDATID      EFFVM_ConvDatID( BTLV_EFFVM_WORK* bevw, ARCDATID datID );
static  void          EFFVM_ChangeVolume( BTLV_EFFVM_WORK* bevw, fx32 start_vol, fx32 end_vol, int frame );
static  int           EFFVM_GetVoicePlayerIndex( BTLV_EFFVM_WORK* bevw );

//TCB�֐�
static  void  TCB_EFFVM_SEPLAY( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_SEPLAY_CB( GFL_TCB* tcb );
static  void  TCB_EFFVM_SEEFFECT( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_SEEFFECT_CB( GFL_TCB* tcb );
static  void  TCB_EFFVM_NAKIGOE( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_NAKIGOE_CB( GFL_TCB* tcb );
static  void  TCB_EFFVM_ChangeVolume( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_WINDOW_MOVE( GFL_TCB* tcb, void* work );
static  void  TCB_EFFVM_WINDOW_MOVE_CB( GFL_TCB* tcb );
static  void  TCB_EFFVM_NakigoeEndCheck( GFL_TCB* tcb, void* work );

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
#define TBL_CNT   ( 4 * 0 )
#define TBL_AA2BB ( 4 * 1 )
#define TBL_BB2AA ( 4 * 2 )
#define TBL_A2B   ( 4 * 3 )
#define TBL_A2C   ( 4 * 4 )
#define TBL_A2D   ( 4 * 5 )
#define TBL_B2A   ( 4 * 6 )
#define TBL_B2C   ( 4 * 7 )
#define TBL_B2D   ( 4 * 8 )
#define TBL_C2A   ( 4 * 9 )
#define TBL_C2B   ( 4 * 10 )
#define TBL_C2D   ( 4 * 11 )
#define TBL_D2A   ( 4 * 12 )
#define TBL_D2B   ( 4 * 13 )
#define TBL_D2C   ( 4 * 14 )
#define TBL_MAX   ( 4 * 15 )
#define TBL_ERROR ( 0xffffffff )

static const int  script_table[ BTLV_MCSS_POS_MAX ][ BTLV_MCSS_POS_MAX ]={
  //  AA          BB          A           B           C           D           E           F
  { TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  },  //AA
  { TBL_BB2AA,  TBL_BB2AA,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  TBL_ERROR,  },  //BB
  { TBL_ERROR,  TBL_ERROR,  TBL_AA2BB,  TBL_A2B,    TBL_A2C,    TBL_A2D,    TBL_AA2BB,  TBL_AA2BB,  },  //A
  { TBL_ERROR,  TBL_ERROR,  TBL_B2A,    TBL_AA2BB,  TBL_B2C,    TBL_B2D,    TBL_AA2BB,  TBL_AA2BB,  },  //B
  { TBL_ERROR,  TBL_ERROR,  TBL_C2A,    TBL_C2B,    TBL_AA2BB,  TBL_C2D,    TBL_AA2BB,  TBL_AA2BB,  },  //C
  { TBL_ERROR,  TBL_ERROR,  TBL_D2A,    TBL_D2B,    TBL_D2C,    TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  },  //D
  { TBL_ERROR,  TBL_ERROR,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  },  //E
  { TBL_ERROR,  TBL_ERROR,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  TBL_AA2BB,  },  //F
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
  VMEC_PARTICLE_PLAY_ALL,
  VMEC_PARTICLE_DELETE,
  VMEC_EMITTER_MOVE,
  VMEC_EMITTER_MOVE_COORDINATE,
  VMEC_EMITTER_CIRCLE_MOVE,
  VMEC_EMITTER_CIRCLE_MOVE_ORTHO,
  VMEC_POKEMON_MOVE,
  VMEC_POKEMON_CIRCLE_MOVE,
  VMEC_POKEMON_SIN_MOVE,
  VMEC_POKEMON_SCALE,
  VMEC_POKEMON_ROTATE,
  VMEC_POKEMON_ALPHA,
  VMEC_POKEMON_MOSAIC,
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
  VMEC_BG_VISIBLE,
  VMEC_WINDOW_MOVE,
  VMEC_OBJ_SET,
  VMEC_OBJ_MOVE,
  VMEC_OBJ_SCALE,
  VMEC_OBJ_ANIME_SET,
  VMEC_OBJ_PAL_FADE,
  VMEC_OBJ_DEL,
  VMEC_GAUGE_VANISH,
  VMEC_SE_PLAY,
  VMEC_SE_STOP,
  VMEC_SE_PAN,
  VMEC_SE_EFFECT,
  VMEC_EFFECT_END_WAIT,
  VMEC_WAIT,
  VMEC_CONTROL_MODE,
  VMEC_IF,
  VMEC_MCSS_POS_CHECK,
  VMEC_SET_WORK,
  VMEC_GET_WORK,
  VMEC_SET_PARAM,
  VMEC_MIGAWARI,
  VMEC_HENSHIN,
  VMEC_NAKIGOE,
  VMEC_BALL_MODE,
  VMEC_BALLOBJ_SET,
  VMEC_CALL,
  VMEC_RETURN,
  VMEC_JUMP,
  VMEC_PAUSE,

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

  //voiceplayerIndex������
  for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
  {
    bevw->voiceplayerIndex[ i ] = EFFVM_VOICEPLAYER_INDEX_NONE;
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

  if( bevw->pause_flag )
  {
    return FALSE;
  }
  if( ( ret == FALSE ) && ( bevw->sequence ) )
  {
    //HP�Q�[�W�\��
    BTLV_EFFECT_SetGaugeDrawEnable( TRUE, BTLEFF_GAUGE_ALL );

    if( bevw->execute_effect_type == EXECUTE_EFF_TYPE_WAZA )
    {
#ifdef KAGEYAMA_DEBUG
      fx32  start_vol = ( 127 * volume_down_ratio / 100 ) << FX32_SHIFT;
      int   frame = volume_up_frame;
#else
      fx32  start_vol = EFFVM_CHANGE_VOLUME;
      int   frame = EFFVM_CHANGE_VOLUME_UP_FRAME;
#endif
      fx32  end_vol = 127 << FX32_SHIFT;
      EFFVM_ChangeVolume( bevw, start_vol, end_vol, frame );
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

  GF_ASSERT_MSG( bevw->sequence == NULL, "���łɃG�t�F�N�g���N�����ł�\neffno:%d\n", bevw->waza );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("EFFVM_Start:\nEFFNO:%d\n",waza);
#endif

  bevw->sequence = NULL;
  BTLV_EFFECT_FreeTCBGroup( GROUP_EFFVM );

  if( param != NULL )
  {
    bevw->param = *param;
  }
  else
  {
    MI_CpuClear16( &bevw->param, sizeof( BTLV_EFFVM_PARAM ) );
  }

  bevw->attack_pos = from;
  bevw->defence_pos = to;
  bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;
  bevw->waza = waza;
  if( bevw->waza < BTLEFF_SINGLE_ENCOUNT_1 )
  {
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_WAZAEFF_SEQ, waza, GFL_HEAP_LOWID( bevw->heapID ) );
    //HP�Q�[�W��\��
    BTLV_EFFECT_SetGaugeDrawEnable( FALSE, BTLEFF_GAUGE_ALL );

    bevw->execute_effect_type = EXECUTE_EFF_TYPE_WAZA;
    //BGM�̃}�X�^�[�{�����[����������
    {
      fx32  start_vol = 127 << FX32_SHIFT;
#ifdef KAGEYAMA_DEBUG
      fx32  end_vol = ( 127 * volume_down_ratio / 100 ) << FX32_SHIFT;
      int   frame = volume_down_frame;
#else
      fx32  end_vol = EFFVM_CHANGE_VOLUME;
      int   frame = EFFVM_CHANGE_VOLUME_DOWN_FRAME;
#endif
      EFFVM_ChangeVolume( bevw, start_vol, end_vol, EFFVM_CHANGE_VOLUME_DOWN_FRAME );
    }
  }
  else
  {
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_BATTLEEFF_SEQ, waza - BTLEFF_SINGLE_ENCOUNT_1, GFL_HEAP_LOWID( bevw->heapID ) );
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

  if( param != NULL )
  { 
    table_ofs += ( TBL_MAX * param->turn_count );
  }

  //�݂���肪�o�Ă���Ƃ��ɋZ�G�t�F�N�g���N������Ȃ�A�݂������������߂�G�t�F�N�g����������
  if( ( bevw->execute_effect_type == EXECUTE_EFF_TYPE_WAZA ) &&
      ( BTLV_MCSS_GetStatusFlag( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos ) & BTLV_MCSS_STATUS_FLAG_MIGAWARI ) )
  {
    bevw->migawari_sequence = bevw->sequence;
    bevw->migawari_table_ofs = table_ofs;
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_BATTLEEFF_SEQ, BTLEFF_MIGAWARI_WAZA_BEFORE - BTLEFF_SINGLE_ENCOUNT_1,
                                            GFL_HEAP_LOWID( bevw->heapID ) );
    bevw->execute_effect_type = EXECUTE_EFF_TYPE_BATTLE;
    table_ofs = TBL_AA2BB;
  }

  start_ofs = (int *)&bevw->sequence[ table_ofs ];

  //�ėp���[�N��������
  bevw->sequence_work = 0;

  //SE�Đ��t���O��������
  bevw->se_play_flag = 0;

  //�{�[�����[�h��������
  bevw->ball_mode = BTLEFF_USE_BALL;

  VM_Start( vmh, &bevw->sequence[ (*start_ofs) ] );

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

  if( bevw->sequence )
  {
    VMEC_SEQ_END( vmh, VM_GetContext( vmh ) );
    GFL_HEAP_FreeMemory( bevw->sequence );
    bevw->sequence = NULL;
  }
}

//============================================================================================
/**
 * @brief VM�ĊJ
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void      BTLV_EFFVM_Restart( VMHANDLE *vmh )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );

  GF_ASSERT( bevw->pause_flag );

  bevw->pause_flag = 0;
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
 * @brief BTLV_EFFVM_PARAM��������
 *
 * @param[out]  param �N���A����BTLV_EFFVM_PARAM�\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_ClearParam( BTLV_EFFVM_PARAM* param )
{ 
  param->waza_range     = 0;      ///<�Z�̌��ʔ͈�
  param->turn_count     = 0;      ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
  param->continue_count = 0;      ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
  param->yure_cnt       = 0;      ///<�{�[������J�E���g
  param->get_success    = 0;      ///<�ߊl�������ǂ���
  param->get_critical   = 0;      ///<�N���e�B�J�����ǂ���
  param->item_no        = 0;      ///<�{�[���̃A�C�e���i���o�[
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
  ARCDATID  datID = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_LOAD\n");
#endif DEBUG_OS_PRINT

  if( EFFVM_RegistPtcNo( bevw, datID, &ptc_no ) == TRUE )
  {
#ifdef PM_DEBUG
    //�f�o�b�O�ǂݍ��݂̏ꍇ�͐�p�̃o�b�t�@���烍�[�h����
    if( bevw->debug_flag == TRUE )
    {
      u32   ofs;
      u32*  ofs_p;

      heap = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), PARTICLE_LIB_HEAP_SIZE );
      bevw->ptc[ ptc_no ] = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, GFL_HEAP_LOWID( bevw->heapID ) );
      ofs_p = (u32*)&bevw->dpd->adrs[ 0 ];
      ofs = ofs_p[ BTLV_EFFVM_GetDPDNo( bevw, datID, DPD_TYPE_PARTICLE ) ];
      resource = (void *)&bevw->dpd->adrs[ ofs ];
      GFL_PTC_SetResourceEx( bevw->ptc[ ptc_no ], resource, FALSE, GFUser_VIntr_GetTCBSYS() );
      EFFVM_RegistSprMax( bevw, ptc_no, resource );
      return bevw->control_mode;
    }
#endif
    heap = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), PARTICLE_LIB_HEAP_SIZE );
    bevw->ptc[ ptc_no ] = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, GFL_HEAP_LOWID( bevw->heapID ) );
    resource = GFL_PTC_LoadArcResource( ARCID_PTC, datID, GFL_HEAP_LOWID( bevw->heapID ) );
    GFL_PTC_SetResource( bevw->ptc[ ptc_no ], resource, FALSE, GFUser_VIntr_GetTCBSYS() );
    EFFVM_RegistSprMax( bevw, ptc_no, resource );
  }

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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw_src = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                    sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       dummy;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    beeiw_src->vmh    = vmh;
    beeiw_src->src    = ( int )VMGetU32( vmh );
    beeiw_src->dst    = ( int )VMGetU32( vmh );
    beeiw_src->ofs.x  = 0;
    beeiw_src->ofs.y  = ( fx32 )VMGetU32( vmh );
    beeiw_src->ofs.z  = 0;
    beeiw_src->angle  = ( fx32 )VMGetU32( vmh );
    //�_�~�[�f�[�^������̂ŋ�ǂ݁G
    dummy = VMGetU32( vmh );
    beeiw_src->radius = ( fx32 )VMGetU32( vmh );
    beeiw_src->life   = ( fx32 )VMGetU32( vmh );
    beeiw_src->scale  = ( fx32 )VMGetU32( vmh );
    beeiw_src->speed  = ( fx32 )VMGetU32( vmh );

    if( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_NONE )
    {
      beeiw_src->dst = beeiw_src->src;
    }

    EFFVM_INIT_EMITTER_POS( bevw, beeiw_src, ptc_no, index );
  }

  GFL_HEAP_FreeMemory( beeiw_src );

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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       dummy;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY_COORDINATE\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    beeiw->vmh        = vmh;
    beeiw->src        = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
    beeiw->dst        = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
    beeiw->src_pos.x  = ( fx32 )VMGetU32( vmh );
    beeiw->src_pos.y  = ( fx32 )VMGetU32( vmh );
    beeiw->src_pos.z  = ( fx32 )VMGetU32( vmh );
    beeiw->dst_pos.x  = ( fx32 )VMGetU32( vmh );
    beeiw->dst_pos.y  = ( fx32 )VMGetU32( vmh );
    beeiw->dst_pos.z  = ( fx32 )VMGetU32( vmh );
    beeiw->ofs.x      = 0;
    beeiw->ofs.y      = ( fx32 )VMGetU32( vmh );
    beeiw->ofs.z      = 0;
    beeiw->angle      = ( fx32 )VMGetU32( vmh );
    //�_�~�[�f�[�^������̂ŋ�ǂ݁G
    dummy = VMGetU32( vmh );
    beeiw->radius     = ( fx32 )VMGetU32( vmh );
    beeiw->life       = ( fx32 )VMGetU32( vmh );
    beeiw->scale      = ( fx32 )VMGetU32( vmh );
    beeiw->speed      = ( fx32 )VMGetU32( vmh );

    if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
    {
      GFL_HEAP_FreeMemory( beeiw );
    }
  }

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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw_src = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                    sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
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
    proj.near   = FX32_ONE * 1;
    proj.far    = FX32_ONE * 1024;
    proj.scaleW = FX32_ONE;

    if( GFL_PTC_GetCameraPtr( bevw->ptc[ ptc_no ] ) == NULL )
    {
      GFL_PTC_PersonalCameraCreate( bevw->ptc[ ptc_no ], &proj, DEFAULT_PERSP_WAY, &Eye, &vUp, &at,
                                    GFL_HEAP_LOWID( bevw->heapID ) );
    }

    beeiw_src->vmh      = vmh;
    beeiw_src->src      = ( int )VMGetU32( vmh );
    beeiw_src->dst      = ( int )VMGetU32( vmh );
    beeiw_src->ofs.x    = ( fx32 )VMGetU32( vmh );
    beeiw_src->ofs.y    = ( fx32 )VMGetU32( vmh );
    beeiw_src->ofs.z    = ( fx32 )VMGetU32( vmh );
    beeiw_src->radius   = ( fx32 )VMGetU32( vmh );
    beeiw_src->life     = ( fx32 )VMGetU32( vmh );
    beeiw_src->scale    = ( fx32 )VMGetU32( vmh );
    beeiw_src->speed    = ( fx32 )VMGetU32( vmh );
    beeiw_src->ortho_mode = 1;

    if( beeiw_src->src == BTLEFF_PARTICLE_PLAY_SIDE_ATTACKOFS )
    {
      beeiw_src->src = BTLEFF_PARTICLE_PLAY_SIDE_ATTACK;
      beeiw_src->ortho_mode = 2;
    }

    if( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_NONE )
    {
      beeiw_src->dst = beeiw_src->src;
    }

    EFFVM_INIT_EMITTER_POS( bevw, beeiw_src, ptc_no, index );
  }

  GFL_HEAP_FreeMemory( beeiw_src );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Đ����ׂ�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PARTICLE_PLAY_ALL( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw_src = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                    sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       spr_max = EFFVM_GetSprMax( bevw, ptc_no );
  int       dummy, index;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_PLAY_ALL\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    beeiw_src->vmh    = vmh;
    beeiw_src->src    = ( int )VMGetU32( vmh );
    beeiw_src->dst    = ( int )VMGetU32( vmh );
    beeiw_src->ofs.x  = 0;
    beeiw_src->ofs.y  = ( fx32 )VMGetU32( vmh );
    beeiw_src->ofs.z  = 0;
    beeiw_src->angle  = ( fx32 )VMGetU32( vmh );
    //�_�~�[�f�[�^������̂ŋ�ǂ݁G
    dummy = VMGetU32( vmh );
    beeiw_src->radius = ( fx32 )VMGetU32( vmh );
    beeiw_src->life   = ( fx32 )VMGetU32( vmh );
    beeiw_src->scale  = ( fx32 )VMGetU32( vmh );
    beeiw_src->speed  = ( fx32 )VMGetU32( vmh );

    if( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_NONE )
    {
      beeiw_src->dst = beeiw_src->src;
    }

    for( index = 0 ; index < spr_max ; index++ )
    {
      EFFVM_INIT_EMITTER_POS( bevw, beeiw_src, ptc_no, index );
    }
  }

  GFL_HEAP_FreeMemory( beeiw_src );

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
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PARTICLE_DELETE\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    EFFVM_DeleteEmitter( bevw->ptc[ ptc_no ] );
    bevw->ptc[ ptc_no ] = NULL;
    bevw->ptc_no[ ptc_no ] = EFFVM_PTCNO_NONE;
  }

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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       dummy;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_MOVE\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    beeiw->vmh        = vmh;
    beeiw->move_type  = ( int )VMGetU32( vmh );
    beeiw->src        = ( int )VMGetU32( vmh );
    beeiw->dst        = ( int )VMGetU32( vmh );
    beeiw->ofs.x      = 0;
    beeiw->ofs.y      = ( fx32 )VMGetU32( vmh );
    beeiw->ofs.z      = 0;
    beeiw->move_frame = ( int )VMGetU32( vmh );
    beeiw->top        = ( fx32 )VMGetU32( vmh );
    beeiw->radius     = FX32_ONE;
    beeiw->life       = ( fx32 )VMGetU32( vmh );
    beeiw->scale      = FX32_ONE;
    beeiw->speed      = ( fx32 )VMGetU32( vmh );
    //�_�~�[�f�[�^������̂ŋ�ǂ݁G
    dummy = VMGetU32( vmh );

    //�ړ����ƈړ��悪����̂Ƃ��́A�A�T�[�g�Ŏ~�߂�
    GF_ASSERT( beeiw->dst != beeiw->src );

    if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
    {
      GFL_HEAP_FreeMemory( beeiw );
    }
  }

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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       dummy;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_MOVE_COORDINATE\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    beeiw->vmh        = vmh;
    beeiw->move_type  = ( int )VMGetU32( vmh );
    beeiw->src        = BTLEFF_PARTICLE_PLAY_SIDE_NONE;
    beeiw->src_pos.x  = ( fx32 )VMGetU32( vmh );
    beeiw->src_pos.y  = ( fx32 )VMGetU32( vmh );
    beeiw->src_pos.z  = ( fx32 )VMGetU32( vmh );
    beeiw->dst        = ( int )VMGetU32( vmh );
    beeiw->ofs.x      = 0;
    beeiw->ofs.y      = ( fx32 )VMGetU32( vmh );
    beeiw->ofs.z      = 0;
    beeiw->move_frame = ( int )VMGetU32( vmh );
    beeiw->top        = ( fx32 )VMGetU32( vmh );
    beeiw->radius     = FX32_ONE;
    beeiw->life       = ( fx32 )VMGetU32( vmh );
    beeiw->scale      = FX32_ONE;
    beeiw->speed      = ( fx32 )VMGetU32( vmh );
    //�_�~�[�f�[�^������̂ŋ�ǂ݁G
    dummy = VMGetU32( vmh );

    //�ړ����ƈړ��悪����̂Ƃ��́A�A�T�[�g�Ŏ~�߂�
    GF_ASSERT( beeiw->dst != beeiw->src );

    if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
    {
      GFL_HEAP_FreeMemory( beeiw );
    }
  }

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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   type;
  VecFx32 move_pos;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_MOVE\n");
#endif DEBUG_OS_PRINT

  type        = ( int )VMGetU32( vmh );
  move_pos.x  = ( fx32 )VMGetU32( vmh );
  move_pos.y  = ( fx32 )VMGetU32( vmh );
  move_pos.z  = 0;
  frame       = ( int )VMGetU32( vmh );
  wait        = ( int )VMGetU32( vmh );
  count       = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MovePosition( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, &move_pos, frame, wait, count );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );

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

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    if( bmmcp.count )
    {
      int i;

      for( i = 0 ; i < pos_cnt ; i++ )
      {
        bmmcp.position = pos[ i ];
        BTLV_MCSS_MoveCircle( BTLV_EFFECT_GetMcssWork(), &bmmcp );
      }
    }
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P����SIN�^��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_SIN_MOVE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_MCSS_MOVE_SIN_PARAM bmmsp;
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   dir           = ( int )VMGetU32( vmh );
  fx32  start_angle   = ( fx32 )VMGetU32( vmh );
  fx32  end_angle     = ( fx32 )VMGetU32( vmh );
  fx32  radius        = ( fx32 )VMGetU32( vmh );
  int   frame         = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SIN_MOVE\n");
#endif DEBUG_OS_PRINT

  bmmsp.dir     = dir;
  bmmsp.angle   = start_angle;
  bmmsp.radius  = radius;
  bmmsp.frame   = frame;
  bmmsp.speed   = FX_Div( ( end_angle - start_angle ), ( frame << FX32_SHIFT ) );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      bmmsp.position = pos[ i ];
      BTLV_MCSS_MoveSin( BTLV_EFFECT_GetMcssWork(), &bmmsp );
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
  int   type;
  VecFx32 scale;
  int   frame;
  int   wait;
  int   count;
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SCALE\n");
#endif DEBUG_OS_PRINT

  type    = ( int )VMGetU32( vmh );
  scale.x = ( fx32 )VMGetU32( vmh );
  scale.y = ( fx32 )VMGetU32( vmh );
  scale.z = FX32_ONE;
  frame   = ( int )VMGetU32( vmh );
  wait    = ( int )VMGetU32( vmh );
  count   = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MoveScale( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, &scale, frame, wait, count );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   type;
  VecFx32 rotate;
  int   frame;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_ROTATE\n");
#endif DEBUG_OS_PRINT

  type     = ( int )VMGetU32( vmh );
  rotate.x   = 0;
  rotate.y   = 0;
  rotate.z   = ( fx32 )VMGetU32( vmh );
  frame    = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MoveRotate( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, &rotate, frame, wait, count );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int type;
  int alpha;
  int frame;
  int wait;
  int count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_ALPHA\n");
#endif DEBUG_OS_PRINT

  type  = ( int )VMGetU32( vmh );
  alpha = ( int )VMGetU32( vmh );
  frame = ( int )VMGetU32( vmh );
  wait  = ( int )VMGetU32( vmh );
  count = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MoveAlpha( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, alpha, frame, wait, count );
    }
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P�������U�C�N
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_POKEMON_MOSAIC( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int type;
  int mosaic;
  int frame;
  int wait;
  int count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_MOSAIC\n");
#endif DEBUG_OS_PRINT

  type    = ( int )VMGetU32( vmh );
  mosaic  = ( int )VMGetU32( vmh );
  frame   = ( int )VMGetU32( vmh );
  wait    = ( int )VMGetU32( vmh );
  count   = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MoveMosaic( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, mosaic, frame, wait, count );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   type;
  int   wait;
  int   count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SET_MEPACHI_FLAG\n");
#endif DEBUG_OS_PRINT

  type     = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;
    if( count == 0 )
    {
      count = 1;
    }
    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_MoveBlink( BTLV_EFFECT_GetMcssWork(), pos[ i ], type, wait, count );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int flag    = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SET_ANM_FLAG\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_SetAnmStopFlag( BTLV_EFFECT_GetMcssWork(), pos[ i ], flag );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt         = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   start_evy       = ( int )VMGetU32( vmh );
  int   end_evy         = ( int )VMGetU32( vmh );
  int   wait            = ( int )VMGetU32( vmh );
  int   rgb             = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_PAL_FADE\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_SetPaletteFade( BTLV_EFFECT_GetMcssWork(), pos[ i ], start_evy, end_evy, wait, rgb );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt   = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   flag      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_VANISH\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;
    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_SetVanishFlag( BTLV_EFFECT_GetMcssWork(), pos[ i ], flag );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt   = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int   flag      = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_SHADOW_VANISH\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_SetShadowVanishFlag( BTLV_EFFECT_GetMcssWork(), pos[ i ], flag );
    }
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
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int   pos_cnt = EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_POKEMON_DEL\n");
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_EFFECT_DelPokemon( pos[ i ] );
    }
  }

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

  if( index == BTLEFF_TRTYPE_FROM_WORK )
  {
    index = bevw->sequence_work;
  }
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
#if 0
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
#else
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int           position = BTLV_EFFECT_GetTrainerIndex( ( int )VMGetU32( vmh ) );
  int           type;
  VecFx32       move_pos;
  int           frame;
  int           wait;
  int           count;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_MOVE\n");
#endif DEBUG_OS_PRINT

  type     = ( int )VMGetU32( vmh );
  move_pos.x = ( int )VMGetU32( vmh );
  move_pos.y = ( int )VMGetU32( vmh );
  move_pos.z  = 0;
  frame    = ( int )VMGetU32( vmh );
  wait     = ( int )VMGetU32( vmh );
  count    = ( int )VMGetU32( vmh );

  BTLV_MCSS_MovePosition( BTLV_EFFECT_GetMcssWork(), position, type, &move_pos, frame, wait, count );

  return bevw->control_mode;
#endif
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
#if 0
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index = BTLV_EFFECT_GetTrainerIndex( ( int )VMGetU32( vmh ) );
  int anm_no = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_ANIME_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_CLACT_SetAnime( BTLV_EFFECT_GetCLWK(), index, anm_no );

  return bevw->control_mode;
#else
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int position  = ( int )VMGetU32( vmh );
  int anm_no    = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_TRAINER_ANIME_SET\n");
#endif DEBUG_OS_PRINT

  if( BTLV_MCSS_SetAnime( BTLV_EFFECT_GetMcssWork(), position, anm_no ) == TRUE )
  { 
    BTLV_MCSS_SetAnimeEndCheck( BTLV_EFFECT_GetMcssWork(), position );
  }

  return bevw->control_mode;
#endif
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
 * @brief BG�̃��[�h
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

    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
    GFL_BG_SetPriority( GFL_BG_FRAME3_M, 1 );

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

    bevw->temp_scr_x = GFL_BG_GetScrollX( GFL_BG_FRAME3_M );
    bevw->temp_scr_y = GFL_BG_GetScrollY( GFL_BG_FRAME3_M );
    GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, 0 );
    GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, 0 );

    bevw->set_priority_flag = 1;

    return bevw->control_mode;
  }
#endif
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
  GFL_BG_SetPriority( GFL_BG_FRAME3_M, 1 );

  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WAZAEFF_GRA, datID + 1, GFL_BG_FRAME3_M, 0, 0, 0, GFL_HEAP_LOWID( bevw->heapID ) );
  GFL_ARC_UTIL_TransVramScreen( ARCID_WAZAEFF_GRA, datID, GFL_BG_FRAME3_M, 0, 0, 0, GFL_HEAP_LOWID( bevw->heapID ) );
  PaletteWorkSetEx_Arc( BTLV_EFFECT_GetPfd(), ARCID_WAZAEFF_GRA, datID + 2, GFL_HEAP_LOWID( bevw->heapID ), FADE_MAIN_BG, 0,
                        BTLV_EFFVM_BG_PAL * 16, 0 );

  bevw->temp_scr_x = GFL_BG_GetScrollX( GFL_BG_FRAME3_M );
  bevw->temp_scr_y = GFL_BG_GetScrollY( GFL_BG_FRAME3_M );
  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, 0 );
  GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, 0 );

  bevw->set_priority_flag = 1;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief BG�̃X�N���[��
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

  BTLV_BG_MovePosition( BTLV_EFFECT_GetBGWork(), position, type, scr_x, scr_y, frame, wait, count );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief BG�̃��X�^�[�X�N���[��
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
 * @brief BG�̃p���b�g�A�j��
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
 * @brief BG�̃v���C�I���e�B
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
 * @brief BG���l
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
                      31, 7 );
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
static VMCMD_RESULT VMEC_BG_VISIBLE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int model = ( int )VMGetU32( vmh );
  int flag = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_BG_VISIBLE\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetVanishFlag( model, flag );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	�E�C���h�E����
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_WINDOW_MOVE( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BTLV_EFFVM_WINDOW_MOVE *bevwm = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                             sizeof( BTLV_EFFVM_WINDOW_MOVE ) );

  bevwm->bevw     = bevw;
	bevwm->type     = ( int )VMGetU32( vmh );
	bevwm->horizon  = ( int )VMGetU32( vmh );
	bevwm->vertical = ( int )VMGetU32( vmh );
	bevwm->in_out   = ( int )VMGetU32( vmh );
	bevwm->frame    = ( int )VMGetU32( vmh );
	bevwm->wait_tmp = ( int )VMGetU32( vmh );
	bevwm->flag     = ( BOOL )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_WINDOW_MOVE\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_WINDOW_MOVE, bevwm, 0 ), TCB_EFFVM_WINDOW_MOVE_CB, GROUP_EFFVM );
  bevw->window_move_flag = 1;

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
 * @brief OBJ�p���b�g�t�F�[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_OBJ_PAL_FADE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int   index     = ( int )VMGetU32( vmh );
  int   start_evy = ( int )VMGetU32( vmh );
  int   end_evy   = ( int )VMGetU32( vmh );
  int   wait      = ( int )VMGetU32( vmh );
  int   rgb       = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_OBJ_PAL_FADE\n");
#endif DEBUG_OS_PRINT

  BTLV_CLACT_SetPaletteFade( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ], start_evy, end_evy, wait, rgb );

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
  OS_TPrintf("VMEC_OBJ_DEL\n");
#endif DEBUG_OS_PRINT

  GF_ASSERT( index < EFFVM_OBJ_MAX );
  GF_ASSERT( bevw->obj[ index ] != EFFVM_OBJNO_NONE );

  BTLV_CLACT_Delete( BTLV_EFFECT_GetCLWK(), bevw->obj[ index ] );
  bevw->obj[ index ] = EFFVM_OBJNO_NONE;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief	HP�Q�[�W�\���^��\��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_GAUGE_VANISH( VMHANDLE *vmh, void *context_work )
{ 
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BOOL flag = ( BOOL )VMGetU32( vmh );
  BOOL side = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_GAUGE_VANISH\n");
#endif DEBUG_OS_PRINT

  BTLV_EFFECT_SetGaugeDrawEnable( flag, side );

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
  int pan       = ( int )VMGetU32( vmh );
  int wait      = ( int )VMGetU32( vmh );
  int pitch     = ( int )VMGetU32( vmh );
  int vol       = ( int )VMGetU32( vmh );
  int mod_depth = ( int )VMGetU32( vmh );
  int mod_speed = ( int )VMGetU32( vmh );
  int dummy     = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SE_PLAY\n");
#endif DEBUG_OS_PRINT

  //SE�Đ��t���O���Z�b�g
  bevw->se_play_flag = 1;

  if( pan == BTLEFF_SEPAN_FLAT )
  {
    pan = 0;
  }
  else
  {
    pan = ( EFFVM_GetPosition( vmh, pan ) & 1 ) ? 127 : -128;
  }

  if( mod_depth > 255 ) { mod_depth = 255; }
  if( mod_speed > 255 ) { mod_speed = 255; }

  if( wait == 0 )
  {
    EFFVM_SePlay( se_no, player, pan, pitch, vol, mod_depth, mod_speed );
  }
  else
  {
    BTLV_EFFVM_SEPLAY*  bes = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_SEPLAY ) );
    bes->bevw       = bevw;
    bes->se_no      = se_no;
    bes->player     = player;
    bes->pan        = pan;
    bes->wait       = wait;
    bes->pitch      = pitch;
    bes->vol        = vol;
    bes->mod_depth  = mod_depth;
    bes->mod_speed  = mod_speed;

    bevw->se_play_wait_flag = 1;

    BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEPLAY, bes, 0 ), TCB_EFFVM_SEPLAY_CB, GROUP_EFFVM );
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
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_SEEFFECT ) );
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

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 ), TCB_EFFVM_SEEFFECT_CB, GROUP_EFFVM );

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
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_SEEFFECT ) );

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

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 ), TCB_EFFVM_SEEFFECT_CB, GROUP_EFFVM );

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
 * @brief �w�肳�ꂽ���[�N�����ď�������
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
 * @brief �ėp���[�N�ɒl���Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SET_WORK( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int value =  ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SET_WORK:\nvalue:%d\n",value);
#endif DEBUG_OS_PRINT

  bevw->sequence_work = value;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ���[�N����l���Q�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_GET_WORK( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_GET_WORK:\n");
#endif DEBUG_OS_PRINT

  bevw->sequence_work = EFFVM_GetWork( bevw, ( int )VMGetU32( vmh ) );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �w�肳�ꂽ���[�N�ɒl���Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_SET_PARAM( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int work = ( int )VMGetU32( vmh );
  int param = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_SET_WORK:\n");
#endif DEBUG_OS_PRINT

  switch( work ){
  case BTLEFF_WORK_WAZA_RANGE:    ///<�Z�̌��ʔ͈�
    bevw->param.waza_range = param;
    break;
  case BTLEFF_WORK_TURN_COUNT:    ///< �^�[���ɂ���ĈقȂ�G�t�F�N�g���o���ꍇ�̃^�[���w��B�iex.������Ƃԁj
    bevw->param.turn_count = param;
    break;
  case BTLEFF_WORK_CONTINUE_COUNT:   ///< �A�����ďo���ƃG�t�F�N�g���قȂ�ꍇ�̘A���J�E���^�iex. ���낪��j
    bevw->param.continue_count = param;
    break;
  case BTLEFF_WORK_YURE_CNT:   ///<�{�[������J�E���g
    bevw->param.yure_cnt = param;
    break;
  case BTLEFF_WORK_GET_SUCCESS:   ///<�ߊl�������ǂ���
    bevw->param.get_success = param;
    break;
  case BTLEFF_WORK_GET_CRITICAL:   ///<�N���e�B�J�����ǂ���
    bevw->param.get_critical = param;
    break;
  case BTLEFF_WORK_ITEM_NO:   ///<�{�[���̃A�C�e���i���o�[
    bevw->param.item_no = param;
    break;
  default:
    //���m�̃p�����[�^�ł�
    GF_ASSERT( 0 );
    break;
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �݂���菈��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_MIGAWARI( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int sw =  ( int )VMGetU32( vmh );
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt =  EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int flag =  ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_MIGAWARI:\nsw:%d\n",sw);
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    int i;

    for( i = 0 ; i < pos_cnt ; i++ )
    {
      BTLV_MCSS_SetMigawari( BTLV_EFFECT_GetMcssWork(), pos[ i ], sw, flag );
    }
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �ւ񂵂񏈗�
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_HENSHIN( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_HENSHIN:\n");
#endif DEBUG_OS_PRINT

  BTLV_MCSS_CopyMAW( BTLV_EFFECT_GetMcssWork(), bevw->defence_pos, bevw->attack_pos );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �|�P��������
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_NAKIGOE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
  int pos_cnt       =  EFFVM_GetPokePosition( bevw, ( int )VMGetU32( vmh ), pos );
  int pitch         = ( int )VMGetU32( vmh );
  int volume        = ( int )VMGetU32( vmh );
  int chorus_vol    = ( int )VMGetU32( vmh );
  int chorus_speed  = ( int )VMGetU32( vmh );
  BOOL play_dir     = ( BOOL )VMGetU32( vmh );
  int wait          = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_NAKIGOE:\npos:%d\n",pos_cnt);
#endif DEBUG_OS_PRINT

  //�����ʒu��񂪂Ȃ��Ƃ��́A�R�}���h���s���Ȃ�
  if( pos_cnt )
  {
    if( wait == 0 )
    { 
      int i;

      for( i = 0 ; i < pos_cnt ; i++ )
      {
        int index = EFFVM_GetVoicePlayerIndex( bevw );
        bevw->voiceplayerIndex[ index ] = BTLV_MCSS_PlayVoice( BTLV_EFFECT_GetMcssWork(), pos[ i ], pitch, volume,
                                                               chorus_vol, chorus_speed, play_dir );
      }
      //BGM�̃}�X�^�[�{�����[����������
      {
        fx32  start_vol = 127 << FX32_SHIFT;
#ifdef KAGEYAMA_DEBUG
        fx32  end_vol = ( 127 * volume_down_ratio_pv / 100 ) << FX32_SHIFT;
        int   frame = volume_down_frame_pv;
#else
        fx32  end_vol = EFFVM_CHANGE_VOLUME_PV;
        int   frame = EFFVM_CHANGE_VOLUME_DOWN_FRAME_PV;
#endif
        EFFVM_ChangeVolume( bevw, start_vol, end_vol, frame );
      }
      if( bevw->volume_already_down == 0 )
      { 
        TCB_NONE_WORK*  tnw = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( TCB_NONE_WORK ) );
        tnw->bevw = bevw;
        BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_NakigoeEndCheck, tnw, 0 ), NULL, GROUP_EFFVM );
      }
    }
    else
    { 
      int i;

      for( i = 0 ; i < pos_cnt ; i++ )
      { 
        BTLV_EFFVM_NAKIGOE*  ben = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_NAKIGOE ) );
        ben->bevw         = bevw;
        ben->pos          = pos[ i ];
        ben->pitch        = pitch;
        ben->volume       = volume;
        ben->chorus_vol   = chorus_vol;
        ben->chorus_speed = chorus_speed;
        ben->play_dir     = play_dir;
        ben->wait         = wait;

        bevw->nakigoe_wait_flag = 1;

        BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_NAKIGOE, ben, 0 ), TCB_EFFVM_NAKIGOE_CB, GROUP_EFFVM );
      }
    }
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �{�[�����[�h
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BALL_MODE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

  bevw->ball_mode = ( int )VMGetU32( vmh );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �{�[��OBJ�̃Z�b�g
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_BALLOBJ_SET( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int index     = ( int )VMGetU32( vmh );
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
  OS_TPrintf("VMEC_BALLOBJ_SET\n");
#endif DEBUG_OS_PRINT

  BTLV_MCSS_GetPokeDefaultPos( BTLV_EFFECT_GetMcssWork(), &pos, position );
  pos.x += ofs_x;
  pos.y += ofs_y;
  NNS_G3dWorldPosToScrPos( &pos, &pos_x, &pos_y );

  {
    BALL_ID ballID = ITEM_GetBallID( bevw->param.item_no );
    ARCDATID  ncgrID, nclrID;

    //�s���ȃ{�[��ID�̓����X�^�[�{�[���ɂ���
    if( ( ballID == BALLID_NULL ) || ( ballID > BALLID_MAX ) )
    {
      ballID = BALLID_MONSUTAABOORU;
    }
    ncgrID = NARC_waza_eff_gra_ball_01_anim_NCER + ballID * 2;
    nclrID = NARC_waza_eff_gra_ball_01_anim_NANR + ballID * 2;
    bevw->obj[ index ] = BTLV_CLACT_AddEx( BTLV_EFFECT_GetCLWK(), ARCID_WAZAEFF_GRA,
                                           ncgrID, nclrID,
                                           NARC_waza_eff_gra_ball_01_anim_NCER, NARC_waza_eff_gra_ball_01_anim_NANR,
                                           pos_x, pos_y, scalex, scaley );
  }

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �T�u���[�`���R�[��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_CALL( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int seq_no = ( int )VMGetU32( vmh );
  int *start_ofs;
  int attack  = ( int )VMGetU32( vmh );
  int defence = ( int )VMGetU32( vmh );

  GF_ASSERT( bevw->call_count < BTLV_EFFVM_STACK_SIZE );
  //�Z�G�t�F�N�g�̃T�u���[�`���R�[���͋����Ȃ�
  GF_ASSERT( seq_no >= BTLEFF_SINGLE_ENCOUNT_1 );

  bevw->push_attack_pos   [ bevw->call_count ]  = bevw->attack_pos;
  bevw->push_defence_pos  [ bevw->call_count ]  = bevw->defence_pos;
  bevw->push_sequence     [ bevw->call_count ]  = bevw->sequence;
  bevw->push_sequence_work[ bevw->call_count ]  = bevw->sequence_work;

  if( attack != BTLEFF_POKEMON_SIDE_ATTACK )
  {
    bevw->attack_pos  = attack;
  }
  if( defence != BTLEFF_POKEMON_SIDE_DEFENCE )
  {
    bevw->defence_pos  = defence;
  }
  bevw->sequence_work = 0;

  bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_BATTLEEFF_SEQ, seq_no - BTLEFF_SINGLE_ENCOUNT_1, GFL_HEAP_LOWID( bevw->heapID ) );
  start_ofs = (int *)&bevw->sequence[ TBL_AA2BB ];

  VMCMD_Call( vmh, &bevw->sequence[ (*start_ofs) ] );

  bevw->call_count++;

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �T�u���[�`�����^�[��
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_RETURN( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

//�f�o�b�O�̂Ƃ��̓T�u���[�`���R�[�����Ȃ��ꍇ�ł��A�T�[�g�Ŏ~�߂���VMEC_SEQ_END�����ɂ���
#ifdef PM_DEBUG
  if( bevw->debug_flag == TRUE )
  {
    int i;

    bevw->debug_flag = FALSE;
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

    return VMCMD_RESULT_SUSPEND;
  }

  if( bevw->debug_return_through )
  {
    bevw->debug_return_through = FALSE;

    //���z�}�V����~
    VM_End( vmh );

    return VMCMD_RESULT_SUSPEND;
  }
#endif

  GF_ASSERT( bevw->call_count );

  bevw->call_count--;

  GFL_HEAP_FreeMemory( bevw->sequence );

  bevw->attack_pos    = bevw->push_attack_pos   [ bevw->call_count ];
  bevw->defence_pos   = bevw->push_defence_pos  [ bevw->call_count ];
  bevw->sequence      = bevw->push_sequence     [ bevw->call_count ];
  bevw->sequence_work = bevw->push_sequence_work[ bevw->call_count ];

  VMCMD_Return( vmh );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �W�����v
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_JUMP( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;
  int adrs  = ( int )VMGetU32( vmh );

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_JUMP\n");
#endif DEBUG_OS_PRINT

  VMCMD_Jump( vmh, vmh->adrs + adrs );

  return bevw->control_mode;
}

//============================================================================================
/**
 * @brief �V�[�P���X�ꎞ��~
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT VMEC_PAUSE( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_PAUSE\n");
#endif DEBUG_OS_PRINT

  bevw->pause_flag = 1;

  VMCMD_SetWait( vmh, VWF_PAUSE_CHECK );

  //�E�G�C�g�͕K��SUSPEND���[�h�ɐ؂�ւ���
  bevw->control_mode = VMCMD_RESULT_SUSPEND;

  return VMCMD_RESULT_SUSPEND;
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

  BTLV_EFFECT_FreeTCBGroup( GROUP_EFFVM );

  //���z�}�V����~
  VM_End( vmh );

  //BG����̐ݒ���f�t�H���g�ɖ߂��Ă���
  if( bevw->set_priority_flag )
  {
    const BTLV_SCU* scu = BTLV_EFFECT_GetScu();
    if( scu != NULL )
    { 
      BTLV_SCU_RestoreDefaultScreen( scu );
    }
    GFL_BG_SetPriority( GFL_BG_FRAME3_M, 0 );
    bevw->set_priority_flag = 0;
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
    GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_X_SET, bevw->temp_scr_x );
    GFL_BG_SetScroll( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, bevw->temp_scr_y );
  }
  if( bevw->set_alpha_flag )
  {

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                      31, 7 );

    bevw->set_alpha_flag = 0;
  }

#ifdef PM_DEBUG
  { 
    const BTLV_SCU* scu = BTLV_EFFECT_GetScu();
    if( scu == NULL )
    { 
      GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );
    }
    //�f�o�b�O�t���O�𗎂Ƃ��Ă���
    bevw->debug_flag = FALSE;
  }
#endif


  //SUSPEND���[�h�ɐ؂�ւ��Ă���
  bevw->control_mode = VMCMD_RESULT_SUSPEND;

  //�݂����������ރG�t�F�N�g�N�����Ă����Ȃ�A�ޔ����Ă����Z�G�t�F�N�g���N��
  if( bevw->migawari_sequence )
  {
    int* start_ofs;

    GFL_HEAP_FreeMemory( bevw->sequence );
    bevw->sequence = bevw->migawari_sequence;
    bevw->migawari_sequence = NULL;
    bevw->execute_effect_type = EXECUTE_EFF_TYPE_WAZA;

    start_ofs = (int *)&bevw->sequence[ bevw->migawari_table_ofs ];

    //�ėp���[�N��������
    bevw->sequence_work = 0;

    VM_Start( vmh, &bevw->sequence[ (*start_ofs) ] );
  }
  //�݂���肪�o�Ă���Ƃ��ɋZ�G�t�F�N�g���N�����Ă����Ȃ�A�݂�����߂��G�t�F�N�g����������
  else if( ( bevw->execute_effect_type == EXECUTE_EFF_TYPE_WAZA ) &&
           ( BTLV_MCSS_GetStatusFlag( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos ) & BTLV_MCSS_STATUS_FLAG_MIGAWARI ) )
  {
    int* start_ofs;

    GFL_HEAP_FreeMemory( bevw->sequence );
    bevw->sequence = GFL_ARC_LoadDataAlloc( ARCID_BATTLEEFF_SEQ, BTLEFF_MIGAWARI_WAZA_AFTER - BTLEFF_SINGLE_ENCOUNT_1,
                                            GFL_HEAP_LOWID( bevw->heapID ) );
    bevw->execute_effect_type = EXECUTE_EFF_TYPE_BATTLE;

    start_ofs = (int *)&bevw->sequence[ TBL_AA2BB ];

    //�ėp���[�N��������
    bevw->sequence_work = 0;

    VM_Start( vmh, &bevw->sequence[ (*start_ofs) ] );
  }

  //�T�u���[�`���R�[�����c���Ă��Ă͂����Ȃ�
  GF_ASSERT( bevw->call_count == 0 );

  //SE�������I�ɃX�g�b�v
  if( bevw->se_play_flag )
  { 
    PMSND_StopSE();
  }
#ifdef DEBUG_SE_END_PRINT
  { 
    BOOL  se_end_flag = FALSE;

    OS_TPrintf("WazaNo:%d\n",bevw->waza);
    //SE�I���`�F�b�N�������Ȃ�
    if( bevw->se_play_wait_flag )
    { 
      OS_TPrintf("�J�n�҂���SE���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( bevw->se_effect_enable_flag )
    { 
      OS_TPrintf("���I�ω�����SE���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE1 ) )
    { 
      OS_TPrintf("SE1���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE2 ) )
    { 
      OS_TPrintf("SE2���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE3 ) )
    { 
      OS_TPrintf("SE3���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE_PSG ) )
    { 
      OS_TPrintf("PSG���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    if( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SYS ) )
    { 
      OS_TPrintf("SYSTEM���I�����Ă��܂���\n");
      se_end_flag = TRUE;
    }
    GF_ASSERT( se_end_flag == FALSE );
    if( se_end_flag == TRUE )
    { 
      OS_TPrintf("se_end_flag�̃A�T�[�g�����܂���\n\n");
    }
    else
    { 
      OS_TPrintf("����I�����܂���\n\n");
    }
  }
#endif

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

#if 0
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
#endif
  //MCSS�A�j���I���H
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ANIME )
  {
    BtlvMcssPos pos;

    for( pos = BTLV_MCSS_POS_TR_AA ; pos < BTLV_MCSS_POS_TOTAL ; pos++ )
    {
      if( BTLV_MCSS_CheckExist( BTLV_EFFECT_GetMcssWork(), pos ) )
      { 
        if( BTLV_MCSS_CheckAnimeExecute( BTLV_EFFECT_GetMcssWork(), pos ) == TRUE )
        {
          return FALSE;
        }
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
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_SE3 )
  {
    if( ( PMSND_CheckPlaySE_byPlayerID( SEPLAYER_SE3 ) ) || ( bevw->se_play_wait_flag ) || ( bevw->se_effect_enable_flag ) )
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
  if( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_VOICE )
  {
    int i;
    for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
    {
      if( bevw->nakigoe_wait_flag )
      { 
        return FALSE;
      }
      if( bevw->voiceplayerIndex[ i ] != EFFVM_VOICEPLAYER_INDEX_NONE )
      { 
        if( PMVOICE_CheckPlay( bevw->voiceplayerIndex[ i ] ) )
        { 
          return FALSE;
        }
        else
        { 
          bevw->voiceplayerIndex[ i ] = EFFVM_VOICEPLAYER_INDEX_NONE;
        }
      }
    }
  }
  if( ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_ALL ) ||
      ( bevw->effect_end_wait_kind == BTLEFF_EFFENDWAIT_WINDOW ) )
  { 
    if( bevw->window_move_flag )
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

//============================================================================================
/**
 * @brief �ꎞ��~�I���`�F�b�N
 *
 * @param[in] vmh       ���z�}�V������\���̂ւ̃|�C���^
 * @param[in] context_work  �R���e�L�X�g���[�N�ւ̃|�C���^
 *
 * @retval  TRUE:�ꎞ��~�I���@FALSE:�ꎞ��~���쒆
 */
//============================================================================================
static  BOOL  VWF_PAUSE_CHECK( VMHANDLE *vmh, void *context_work )
{
  BTLV_EFFVM_WORK *bevw = ( BTLV_EFFVM_WORK* )context_work;

  return ( bevw->pause_flag == 0 );
}

//����J�֐��Q
//============================================================================================
/**
 * @brief �����ʒu���̎擾�i�|�P��������֘A��p�j
 *
 * @param[in]   bevw      �G�t�F�N�g�Ǘ��\����
 * @param[in]   pos_flag  �擾�������|�W�V�����t���O
 * @param[out]  pos       �擾�����|�W�V�����̊i�[���[�N
 *
 * @retval  �擾�����|�W�V������
 */
//============================================================================================
static  int   EFFVM_GetPokePosition( BTLV_EFFVM_WORK* bevw, int pos_flag, BtlvMcssPos* pos )
{
  int pos_cnt = 1;

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
    pos[ 0 ] = pos_flag;
    break;
  case BTLEFF_POKEMON_SIDE_ATTACK:    //�U����
    pos[ 0 ] = bevw->attack_pos;
    if( pos[ 0 ] == BTLV_MCSS_POS_ERROR )
    {
      if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_AA ) == TRUE )
      {
        pos[ 0 ] = BTLV_MCSS_POS_AA;
      }
      else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_A ) == TRUE )
      {
        pos[ 0 ] = BTLV_MCSS_POS_A;
      }
      else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_C ) == TRUE )
      {
        pos[ 0 ] = BTLV_MCSS_POS_C;
      }
      else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_C ) == TRUE )
      {
        pos[ 0 ] = BTLV_MCSS_POS_E;
      }
    }
    break;
  case BTLEFF_POKEMON_SIDE_ATTACK_PAIR: //�U�����y�A
    if( bevw->attack_pos > BTLV_MCSS_POS_BB ){
      pos[ 0 ] = bevw->attack_pos ^ BTLV_MCSS_POS_PAIR_BIT;
    }
    else{
      pos[ 0 ] = BTLV_MCSS_POS_ERROR;
    }
    break;
  case BTLEFF_POKEMON_SIDE_DEFENCE:   //�h�䑤
    pos[ 0 ] = bevw->defence_pos;
    if( pos[ 0 ] == BTLV_MCSS_POS_ERROR )
    {
      BtlRule rule = BTLV_EFFECT_GetBtlRule();
      pos_cnt = 0;
      if( rule == BTL_RULE_SINGLE )
      {
        break;
      }
      switch( bevw->param.waza_range ){
      case WAZA_TARGET_OTHER_SELECT:        ///< �ʏ�|�P�i�P�̑I���j
      case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����|�P�i�P�̑I���j
      case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����|�P�i�P�̑I���j
      case WAZA_TARGET_ENEMY_SELECT:        ///< ���葤�|�P�i�P�̑I���j
      //case WAZA_TARGET_LONG_SELECT:         ///<�ΏۑI��\���p�ɒ�`
        //�{�����̑I��͈͂�BTLV_MCSS_POS_ERROR�ɂȂ邱�Ƃ͂Ȃ��͂�
        GF_ASSERT( 0 );
        break;
      case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�̑S�|�P
        if( ( rule == BTL_RULE_TRIPLE ) &&
            ( bevw->attack_pos != BTLV_MCSS_POS_C ) &&
            ( bevw->attack_pos != BTLV_MCSS_POS_D ) )
        {
          BtlvMcssPos check_pos[ 4 ][ 3 ] = {
            //BTLV_MCSS_POS_A
            {
              BTLV_MCSS_POS_C,
              BTLV_MCSS_POS_D,
              BTLV_MCSS_POS_F,
            },
            //BTLV_MCSS_POS_B
            {
              BTLV_MCSS_POS_C,
              BTLV_MCSS_POS_D,
              BTLV_MCSS_POS_E,
            },
            //BTLV_MCSS_POS_E
            {
              BTLV_MCSS_POS_B,
              BTLV_MCSS_POS_C,
              BTLV_MCSS_POS_D,
            },
            //BTLV_MCSS_POS_F
            {
              BTLV_MCSS_POS_A,
              BTLV_MCSS_POS_C,
              BTLV_MCSS_POS_D,
            },
          };
          int src = ( ( bevw->attack_pos > BTLV_MCSS_POS_B ) ? 2 : 0 ) + ( bevw->attack_pos & 1 );
          int i;

          for( i = 0 ; i < 3 ; i++ )
          {
            if( BTLV_EFFECT_CheckExist( check_pos[ src ][ i ] ) )
            {
              pos[ pos_cnt ] = check_pos[ src ][ i ];
              pos_cnt++;
            }
          }
        }
        else
        {
          BtlvMcssPos check_pos;
          BtlvMcssPos end_pos = ( rule == BTL_RULE_TRIPLE ) ? BTLV_MCSS_POS_F : BTLV_MCSS_POS_D;

          for( check_pos = BTLV_MCSS_POS_A ; check_pos <= end_pos ; check_pos++ )
          {
            if( bevw->attack_pos != check_pos )
            {
              if( BTLV_EFFECT_CheckExist( check_pos ) )
              {
                pos[ pos_cnt ] = check_pos;
                pos_cnt++;
              }
            }
          }
        }
        break;
      case WAZA_TARGET_UNKNOWN:             ///< ��т��ӂ�ȂǓ���^
      case WAZA_TARGET_USER:                ///< �����̂�
      case WAZA_TARGET_ENEMY_RANDOM:        ///< ����|�P�P�̃����_��
        //�G�t�F�N�g�ɂ���đ��葤���w������Ӗ��Ŗh�䑤�������Ă���̂ŁA
        //�S�|�P�w���Ƃ���
      case WAZA_TARGET_ENEMY_ALL:           ///< ���葤�S�|�P
        {
          BtlvMcssPos check_pos;
          BtlvMcssPos start_pos = ( bevw->attack_pos & 1 ) ? BTLV_MCSS_POS_A : BTLV_MCSS_POS_B;
          BtlvMcssPos end_pos = ( rule == BTL_RULE_TRIPLE ) ? BTLV_MCSS_POS_F : BTLV_MCSS_POS_D;

          if( rule == BTL_RULE_TRIPLE )
          {
            if( bevw->attack_pos <= BTLV_MCSS_POS_B )
            {
              start_pos += 2;
            }
            if( bevw->attack_pos >= BTLV_MCSS_POS_E )
            {
              end_pos -= 2;
            }
          }

          for( check_pos = start_pos ; check_pos <= end_pos ; check_pos += 2 )
          {
            if( BTLV_EFFECT_CheckExist( check_pos ) )
            {
              pos[ pos_cnt ] = check_pos;
              pos_cnt++;
            }
          }
        }
        break;
      case WAZA_TARGET_SIDE_ENEMY:          ///< �G���w�c
        {
          BtlvMcssPos check_pos;
          BtlvMcssPos start_pos = ( bevw->attack_pos & 1 ) ? BTLV_MCSS_POS_A : BTLV_MCSS_POS_B;
          BtlvMcssPos end_pos = ( rule == BTL_RULE_TRIPLE ) ? BTLV_MCSS_POS_F : BTLV_MCSS_POS_D;

          for( check_pos = start_pos ; check_pos <= end_pos ; check_pos += 2 )
          {
            if( BTLV_EFFECT_CheckExist( check_pos ) )
            {
              pos[ pos_cnt ] = check_pos;
              pos_cnt++;
            }
          }
        }
        break;
      case WAZA_TARGET_FRIEND_ALL:          ///< �������S�|�P
      case WAZA_TARGET_SIDE_FRIEND:         ///< �������w�c
        {
          BtlvMcssPos check_pos;
          BtlvMcssPos start_pos = ( bevw->attack_pos & 1 ) ? BTLV_MCSS_POS_B : BTLV_MCSS_POS_A;
          BtlvMcssPos end_pos = ( rule == BTL_RULE_TRIPLE ) ? BTLV_MCSS_POS_F : BTLV_MCSS_POS_D;

          for( check_pos = start_pos ; check_pos <= end_pos ; check_pos += 2 )
          {
            if( BTLV_EFFECT_CheckExist( check_pos ) )
            {
              pos[ pos_cnt ] = check_pos;
              pos_cnt++;
            }
          }
        }
        break;
      case WAZA_TARGET_ALL:                 ///< ��ɏo�Ă���S�|�P
      case WAZA_TARGET_FIELD:               ///< ��S�́i�V��Ȃǁj
        {
          BtlvMcssPos check_pos;
          BtlvMcssPos end_pos = ( rule == BTL_RULE_TRIPLE ) ? BTLV_MCSS_POS_F : BTLV_MCSS_POS_D;

          for( check_pos = BTLV_MCSS_POS_A ; check_pos <= end_pos ; check_pos++ )
          {
            if( BTLV_EFFECT_CheckExist( check_pos ) )
            {
              pos[ pos_cnt ] = check_pos;
              pos_cnt++;
            }
          }
        }
        break;
      }
    }
    break;
  case BTLEFF_POKEMON_SIDE_DEFENCE_PAIR:  //�h�䑤�y�A
    //@todo �Z��RANGE�ɂ��킹�đΏۂƂȂ闧���ʒu��Ԃ��悤�ɂ��Ȃ���΂Ȃ�Ȃ�
    if( bevw->defence_pos > BTLV_MCSS_POS_BB ){
      pos[ 0 ] = bevw->defence_pos ^ BTLV_MCSS_POS_PAIR_BIT;
    }
    else{
      pos[ 0 ] = BTLV_MCSS_POS_ERROR;
    }
    break;
  default:
    OS_TPrintf("��`����Ă��Ȃ��t���O���w�肳��Ă��܂�\n");
    GF_ASSERT( 0 );
    break;
  }

  if( ( pos[ 0 ] != BTLV_MCSS_POS_ERROR ) && ( pos[ 0 ] < BTLV_MCSS_POS_MAX ) )
  {
    if( BTLV_EFFECT_CheckExist( pos[ 0 ] ) == TRUE )
    {
      pos[ 0 ] = EFFVM_ConvPokePosition( bevw, pos[ 0 ] );
    }
    else
    {
      pos[ 0 ] = BTLV_MCSS_POS_ERROR;
      pos_cnt = 0;
    }
  }

  return pos_cnt;
}
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
    if( position == BTLV_MCSS_POS_ERROR )
    {
      if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_A ) == TRUE )
      {
        return BTLV_MCSS_POS_A;
      }
      else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_C ) == TRUE )
      {
        return BTLV_MCSS_POS_C;
      }
      else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_E ) == TRUE )
      {
        return BTLV_MCSS_POS_E;
      }
      else
      {
        return BTLV_MCSS_POS_AA;
      }
    }
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
        if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_A ) == TRUE )
        {
          return BTLV_MCSS_POS_A;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_C ) == TRUE )
        {
          return BTLV_MCSS_POS_C;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_E ) == TRUE )
        {
          return BTLV_MCSS_POS_E;
        }
        else
        {
          return BTLV_MCSS_POS_AA;
        }
      }
      else
      {
        if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_B ) == TRUE )
        {
          return BTLV_MCSS_POS_B;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_D ) == TRUE )
        {
          return BTLV_MCSS_POS_D;
        }
        else if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_F ) == TRUE )
        {
          return BTLV_MCSS_POS_F;
        }
        else
        {
          return BTLV_MCSS_POS_BB;
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

  position = EFFVM_ConvPosition( vmh, position );
#if 0
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
#endif

  return position;
}

//============================================================================================
/**
 * @brief �����ʒu���̕ϊ��i���]�t���O�����ēK�؂ȗ����ʒu��Ԃ��F�|�P������p�j
 *
 * @param[in] bevw
 * @param[in] position
 *
 * @retval  �K�؂ȗ����ʒu
 */
//============================================================================================
static  int   EFFVM_ConvPokePosition( BTLV_EFFVM_WORK* bevw, BtlvMcssPos position )
{
  if( BTLV_EFFECT_CheckExist( position ) == TRUE )
  {
    if( bevw->position_reverse_flag )
    {
      position ^= 1;
    }
  }
  else
  {
    position = BTLV_MCSS_POS_ERROR;
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
 * @param[in]   bevw    �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in]   datID   �A�[�J�C�udatID
 * @param[out]  ptc_no  datID��o�^����ptc_no
 *
 * @retval  TRUE�F�o�^����  FALSE:���łɓo�^���Ă���
 */
//============================================================================================
static  BOOL  EFFVM_RegistPtcNo( BTLV_EFFVM_WORK *bevw, ARCDATID datID, int *ptc_no )
{
  int i;
  BOOL  ret = TRUE;

  for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ )
  {
    if( bevw->ptc_no[ i ] == datID )
    {
      ret = FALSE;
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

  *ptc_no = i;

  return ret;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Ɋ܂܂��spr��max��o�^
 *
 * @param[in]   bevw      �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[out]  ptc_no    datID��o�^����ptc_no
 * @param[out]  resource  spa���\�[�X
 */
//============================================================================================
static  void  EFFVM_RegistSprMax( BTLV_EFFVM_WORK *bevw, int ptc_no, void* resource )
{
  SPA_DATA* sd = ( SPA_DATA* )resource;

  bevw->spr_max[ ptc_no ] = sd->size;
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

  return i;
}

//============================================================================================
/**
 * @brief �p�[�e�B�N���Ɋ܂܂��spr��max���擾
 *
 * @param[in] bevw    �G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in] ptc_no  �擾����p�[�e�B�N���f�[�^�̃C���f�b�N�X
 *
 * @retval  spr��max
 */
//============================================================================================
static  int EFFVM_GetSprMax( BTLV_EFFVM_WORK *bevw, int ptc_no )
{
  return bevw->spr_max[ ptc_no ];
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
  BOOL  minus_flag = beeiw->minus_flag;

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
    EFFVM_CalcPosOrtho( &dst, &beeiw->ofs );
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
    VecFx32 line_vec_xz, line_vec_y;
    MtxFx43 rot_xz, rot_y;
    u16     angle, angle_xz, angle_y;

    bevw->temp_work[ bevw->temp_work_count ] = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                     sizeof( BTLV_EFFVM_EMITTER_MOVE_WORK ) );
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
    if( ( beeiw->move_type == BTLEFF_EMITTER_MOVE_STRAIGHT ) ||
        ( beeiw->move_type == BTLEFF_EMITTER_MOVE_OFFSET ) )
    {
      beemw->radius_y = 0;
    }
    else
    {
      beemw->radius_y = beeiw->top;
    }

    if( beeiw->move_type == BTLEFF_EMITTER_MOVE_OFFSET )
    {
      fx32  work;

      work  = src.x;
      src.x = dst.x;
      dst.x = work;
      work  = src.y;
      src.y = dst.y;
      dst.y = work;
      work  = src.z;
      src.z = dst.z;
      dst.z = work;
      dst.x += src.x;
      dst.y += src.y;
      dst.z += src.z;
    }

    line_vec.x = dst.x - src.x;
    line_vec.y = dst.y - src.y;
    line_vec.z = dst.z - src.z;
    VEC_Normalize( &line_vec, &line_vec );

    std_vec.x = FX32_ONE;
    std_vec.y = 0;
    std_vec.z = 0;

#if 0
    line_vec_xz.x = line_vec.x;
    line_vec_xz.y = 0;
    line_vec_xz.z = line_vec.z;

    VEC_Normalize( &line_vec_xz, &line_vec_xz );

    line_vec_y.x = FX32_ONE;
    line_vec_y.y = line_vec.y;
    line_vec_y.z = 0;
    VEC_Normalize( &line_vec_y, &line_vec_y );

    angle_xz = FX_AcosIdx( VEC_DotProduct( &std_vec, &line_vec_xz ) );
    angle_y = FX_AcosIdx( VEC_DotProduct( &std_vec, &line_vec_y ) );

    MTX_RotY43( &rot_xz, FX_SinIdx( angle_xz ), FX_CosIdx( angle_xz ) );
    MTX_RotZ43( &rot_y, FX_SinIdx( angle_y ), FX_CosIdx( angle_y ) );
    MTX_Concat43( &rot_xz, &rot_y, &beemw->mtx43 );

#else
    //���ς����߂Ċp�x���o��
    angle = FX_AcosIdx( VEC_DotProduct( &std_vec, &line_vec ) );

    //�O�ς����߂ĉ�]�����o��
    VEC_CrossProduct( &std_vec, &line_vec, &rot_axis );
    VEC_Normalize( &rot_axis, &rot_axis );

    //��]�s��𐶐�����
    MTX_RotAxis43( &beemw->mtx43, &rot_axis, FX_SinIdx( angle ), FX_CosIdx( angle ) );
#endif

    //���s�ړ��s��𐶐�
    beemw->mtx43._30 = src.x;
    beemw->mtx43._31 = src.y;
    beemw->mtx43._32 = src.z;
  }

  //src��dst���ꏏ�̂Ƃ��́A�����Ȃ�
  if( ( ( beeiw->src != BTLEFF_PARTICLE_PLAY_SIDE_NONE ) && ( beeiw->dst != BTLEFF_PARTICLE_PLAY_SIDE_NONE ) ) &&
        ( beeiw->src != beeiw->dst ) )
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
//  else
  {
    //���ˉe�ł�Z�Ŋg�k���Ȃ��̂ŁA���������̍Đ�������������␳������
    if( ( ( beeiw->ortho_mode ) && ( beeiw->src & 1 ) ) ||
          ( beeiw->ortho_mode == 0 ) ||
          ( beeiw->ortho_mode == 2 ) )
    {
      fx32  radius  = GFL_PTC_GetEmitterRadius( emit );
      fx32  life    = ( GFL_PTC_GetEmitterParticleLife( emit ) << FX32_SHIFT );
      fx32  scale   = GFL_PTC_GetEmitterBaseScale( emit );
      if( beeiw->radius )
      {
        GFL_PTC_SetEmitterRadius( emit, FX_Mul( radius, beeiw->radius ) );
      }
      if( beeiw->life )
      {
        u16 ptcl_life = FX_Mul( life, beeiw->life ) >> FX32_SHIFT;
        GFL_PTC_SetEmitterParticleLife( emit, ptcl_life );
      }
      if( beeiw->scale )
      {
        GFL_PTC_SetEmitterBaseScale( emit, (fx16)FX_Mul( scale, beeiw->scale ) );
      }
      if( beeiw->speed )
      {
        VecFx32  vel32;
        fx16  vel;

        GFL_PTC_GetEmitterVelocity( emit, &vel32 );
        vel32.x = FX_Mul( vel32.x, beeiw->speed );
        vel32.y = FX_Mul( vel32.y, beeiw->speed );
        vel32.z = FX_Mul( vel32.z, beeiw->speed );
        GFL_PTC_SetEmitterVelocity( emit, &vel32 );
        vel = GFL_PTC_GetEmitterInitVelocityPos( emit );
        GFL_PTC_SetEmitterInitVelocityPos( emit, FX_Mul( vel, beeiw->speed ) );
        vel = GFL_PTC_GetEmitterInitVelocityAxis( emit );
        GFL_PTC_SetEmitterInitVelocityAxis( emit, FX_Mul( vel, beeiw->speed ) );
      }
    }
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

  //OS_Printf( "pos_x:%x pos_y:%x pos_z:%x\n", emit_pos.x, emit_pos.y, emit_pos.z );

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

  if( ( beecmw->ortho_mode ) && ( beecmw->center_pos.z < 0 ) )
  {
    GFL_PTC_SetEmitterBaseScale( emit, FX16_HALF );
  }

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
 * @param[in] se_no       �Đ�����SE�i���o�[
 * @param[in] player      �Đ�����PlayerNo
 * @param[in] pan         �Đ��p��
 * @param[in] pitch       �Đ��s�b�`
 * @param[in] vol         �Đ��{�����[��
 * @param[in] mod_depth   �Đ����W�����[�V�����f�v�X
 * @param[in] mod_speed   �Đ����W�����[�V�����X�s�[�h
 */
//============================================================================================
static  void  EFFVM_SePlay( int se_no, int player, int pan, int pitch, int vol, int mod_depth, int mod_speed )
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
  PMSND_SetStatusSE_byPlayerID( player, PMSND_NOEFFECT, PMSND_NOEFFECT, pan );
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
  case BTLEFF_WORK_POS_AA_WEIGHT: ///<POS_AA�̑̏d
  case BTLEFF_WORK_POS_BB_WEIGHT: ///<POS_BB�̑̏d
  case BTLEFF_WORK_POS_A_WEIGHT:  ///<POS_A�̑̏d
  case BTLEFF_WORK_POS_B_WEIGHT:  ///<POS_B�̑̏d
  case BTLEFF_WORK_POS_C_WEIGHT:  ///<POS_C�̑̏d
  case BTLEFF_WORK_POS_D_WEIGHT:  ///<POS_D�̑̏d
  case BTLEFF_WORK_POS_E_WEIGHT:  ///<POS_E�̑̏d
  case BTLEFF_WORK_POS_F_WEIGHT:  ///<POS_F�̑̏d
    ret = BTLV_MCSS_GetWeight( BTLV_EFFECT_GetMcssWork(), param );
    break;
  case BTLEFF_WORK_ATTACK_WEIGHT:  ///<�U�����̑̏d
    ret = BTLV_MCSS_GetWeight( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos );
    break;
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
  case BTLEFF_WORK_GET_CRITICAL:   ///<�N���e�B�J�����ǂ���
    ret = bevw->param.get_critical;
    break;
  case BTLEFF_WORK_ITEM_NO:   ///<�{�[���̃A�C�e���i���o�[
    ret = bevw->param.item_no;
    break;
  case BTLEFF_WORK_SEQUENCE_WORK:   ///<�ėp���[�N
    ret = bevw->sequence_work;
    break;
  case BTLEFF_WORK_ATTACK_POKEMON:
    ret = bevw->attack_pos;
    break;
  case BTLEFF_WORK_ATTACK_POKEMON_VANISH:
    ret = BTLV_MCSS_GetVanishFlag( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos );
    break;
  case BTLEFF_WORK_ATTACK_POKEMON_DIR:
    ret = bevw->attack_pos & 1;
    break;
  case BTLEFF_WORK_POS_AA_RARE:
  case BTLEFF_WORK_POS_BB_RARE:
  case BTLEFF_WORK_POS_A_RARE:
  case BTLEFF_WORK_POS_B_RARE:
  case BTLEFF_WORK_POS_C_RARE:
  case BTLEFF_WORK_POS_D_RARE:
  case BTLEFF_WORK_POS_E_RARE:
  case BTLEFF_WORK_POS_F_RARE:
    ret = ( ( BTLV_MCSS_GetStatusFlag( BTLV_EFFECT_GetMcssWork(), param - BTLEFF_WORK_POS_AA_RARE ) & BTLV_MCSS_STATUS_FLAG_RARE ) != 0 );
    break;
  case BTLEFF_WORK_ATTACK_RARE:
    ret = ( ( BTLV_MCSS_GetStatusFlag( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos ) & BTLV_MCSS_STATUS_FLAG_RARE ) != 0 );
    break;
  case BTLEFF_WORK_POS_AA_JUMP:
  case BTLEFF_WORK_POS_BB_JUMP:
  case BTLEFF_WORK_POS_A_JUMP:
  case BTLEFF_WORK_POS_B_JUMP:
  case BTLEFF_WORK_POS_C_JUMP:
  case BTLEFF_WORK_POS_D_JUMP:
  case BTLEFF_WORK_POS_E_JUMP:
  case BTLEFF_WORK_POS_F_JUMP:
    ret = BTLV_MCSS_GetNoJump( BTLV_EFFECT_GetMcssWork(), param - BTLEFF_WORK_POS_AA_JUMP );
    break;
  case BTLEFF_WORK_ATTACK_JUMP:
    ret = BTLV_MCSS_GetNoJump( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos );
    break;
  case BTLEFF_WORK_MULTI:
    ret = BTLV_EFFECT_GetMulti();
    break;
  case BTLEFF_WORK_RULE:
    ret = BTLV_EFFECT_GetBtlRule();
    break;
  case BTLEFF_WORK_TRTYPE_A:
  case BTLEFF_WORK_TRTYPE_B:
  case BTLEFF_WORK_TRTYPE_C:
  case BTLEFF_WORK_TRTYPE_D:
    ret = BTLV_EFFECT_GetTrType( param - BTLEFF_WORK_TRTYPE_A );
    break;
  case BTLEFF_WORK_POS_AA_FLY:
  case BTLEFF_WORK_POS_BB_FLY:
  case BTLEFF_WORK_POS_A_FLY:
  case BTLEFF_WORK_POS_B_FLY:
  case BTLEFF_WORK_POS_C_FLY:
  case BTLEFF_WORK_POS_D_FLY:
  case BTLEFF_WORK_POS_E_FLY:
  case BTLEFF_WORK_POS_F_FLY:
    ret = BTLV_MCSS_GetFlyFlag( BTLV_EFFECT_GetMcssWork(), param - BTLEFF_WORK_POS_AA_FLY );
    break;
  case BTLEFF_WORK_ATTACK_FLY:
    ret = BTLV_MCSS_GetFlyFlag( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos );
    break;
  default:
    //���m�̃p�����[�^�ł�
    GF_ASSERT( 0 );
    break;
  }
  return ret;
}

//============================================================================================
/**
 * @brief �����l����̃G�~�b�^�����i�h�䑤�w����l�����������j
 *
 * @param[in] bevw
 * @param[in] beeiw_src
 * @param[in] ptc_no
 * @param[in] index
 */
//============================================================================================
static void EFFVM_INIT_EMITTER_POS( BTLV_EFFVM_WORK* bevw, BTLV_EFFVM_EMIT_INIT_WORK* beeiw_src, int ptc_no, int index )
{
  if( ( beeiw_src->dst == beeiw_src->src ) &&
      ( ( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE ) ||
        ( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS ) ) )
  {
    if( beeiw_src->dst == BTLEFF_PARTICLE_PLAY_SIDE_DEFENCE_MINUS )
    {
      beeiw_src->minus_flag = TRUE;
    }
    {
      BtlvMcssPos pos[ BTLV_MCSS_POS_MAX ];
      int   pos_cnt = EFFVM_GetPokePosition( bevw, BTLEFF_POKEMON_SIDE_DEFENCE, pos );
      if( pos_cnt )
      {
        int i;

        for( i = 0 ; i < pos_cnt ; i++ )
        {
          BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                        sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );

          *beeiw = *beeiw_src;
          beeiw->src = pos[ i ];
          beeiw->dst = pos[ i ];

          if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
          {
            GFL_HEAP_FreeMemory( beeiw );
          }
        }
      }
    }
  }
  else
  {
    BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                  sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );

    *beeiw = *beeiw_src;

    if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
    {
      GFL_HEAP_FreeMemory( beeiw );
    }
  }
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
  ARCDATID  datID   = EFFVM_ConvDatID( bevw, ( ARCDATID )VMGetU32( vmh ) );
  int       ptc_no  = EFFVM_GetPtcNo( bevw, datID );
  int       index   = ( int )VMGetU32( vmh );
  int       position = 0;
  fx32      offset_y;

#ifdef DEBUG_OS_PRINT
  OS_TPrintf("VMEC_EMITTER_CIRCLE_MOVE\n");
#endif DEBUG_OS_PRINT

  if( ptc_no != EFFVM_PTCNO_NO_FIND )
  {
    if( ( ortho_mode == TRUE ) && ( GFL_PTC_GetCameraPtr( bevw->ptc[ ptc_no ] ) == NULL ) )
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
      proj.near   = FX32_ONE * 1;
      proj.far    = FX32_ONE * 1024;
      proj.scaleW = FX32_ONE;

      GFL_PTC_PersonalCameraCreate( bevw->ptc[ ptc_no ], &proj, DEFAULT_PERSP_WAY, &Eye, &vUp, &at,
                                    GFL_HEAP_LOWID( bevw->heapID ) );
    }

    bevw->temp_work[ bevw->temp_work_count ] = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                     sizeof( BTLV_EFFVM_EMITTER_CIRCLE_MOVE_WORK ) );
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

    if( GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterCircleMove, beecmw ) == PTC_NON_CREATE_EMITTER )
    {
      GFL_HEAP_FreeMemory( beecmw );
    }
  }

  return bevw->control_mode;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �����ˉe�ł̍��W���琳�ˉe�ł̍��W�ɕϊ�����
 *
 *  @param[in/out]  pos ���W�ϊ����s�����W
 *  @param[in]      ofs �I�t�Z�b�g���W�iNULL�̏ꍇ�̓I�t�Z�b�g�v�Z�����Ȃ��j
 */
//-----------------------------------------------------------------------------
static  void  EFFVM_CalcPosOrtho( VecFx32 *pos, VecFx32 *ofs )
{
  MtxFx44 mtx;
  fx32  w;

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
 *  @brief  4x4�s��ɍ��W���|�����킹��
 *      Vec��x,y,z,1�Ƃ��Čv�Z���A�v�Z���Vec��w��Ԃ��܂��B
 *
 *  @param  *cp_src Vector���W
 *  @param  *cp_m 4*4�s��
 *  @param  *p_dst  Vecror�v�Z����
 *  @param  *p_w  4�ڂ̗v�f�̒l
 *
 *  @return
 */
//-----------------------------------------------------------------------------
static  void MTX_MultVec44( const VecFx32 *cp_src, const MtxFx44 *cp_m, VecFx32 *p_dst, fx32 *p_w )
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
    p_dst->x += cp_m->_30;  //  W=1�Ȃ̂ő�������

    p_dst->y = (fx32)(((fx64)x * cp_m->_01 + (fx64)y * cp_m->_11 + (fx64)z * cp_m->_21) >> FX32_SHIFT);
    p_dst->y += cp_m->_31;//  W=1�Ȃ̂ő�������

    p_dst->z = (fx32)(((fx64)x * cp_m->_02 + (fx64)y * cp_m->_12 + (fx64)z * cp_m->_22) >> FX32_SHIFT);
    p_dst->z += cp_m->_32;//  W=1�Ȃ̂ő�������

  *p_w  = (fx32)(((fx64)x * cp_m->_03 + (fx64)y * cp_m->_13 + (fx64)z * cp_m->_23) >> FX32_SHIFT);
    *p_w  += cp_m->_33;// W=1�Ȃ̂ő�������
}

//----------------------------------------------------------------------------
/**
 *  @brief  �{�[���̃��\�[�X���Q�Ƃ��Ă���Ȃ�Aball_mode�ɂ��I�t�Z�b�g�v�Z����
 *
 *  @param[in]  bevw  �V�X�e���Ǘ��\����
 *  @param[in]  datID ���\�[�XID
 *
 *  @retval ARCDATID  �I�t�Z�b�g�v�Z������ARCDATID
 */
//-----------------------------------------------------------------------------
static  ARCDATID  EFFVM_ConvDatID( BTLV_EFFVM_WORK* bevw, ARCDATID datID )
{
  ARCDATID  ofs = 0;
#ifdef PM_DEBUG
  //�f�o�b�O�ǂݍ��݂̏ꍇ�͂��̂܂ܕԂ�
  if( bevw->debug_flag == TRUE )
  {
    return datID;
  }
#endif

  if( bevw->ball_mode == BTLEFF_CAPTURE_BALL_ATTACK )
  {
    ofs = BTLV_MCSS_GetCaptureBall( BTLV_EFFECT_GetMcssWork(), bevw->attack_pos );
  }
  else if( bevw->ball_mode != BTLEFF_USE_BALL )
  {
    ofs = BTLV_MCSS_GetCaptureBall( BTLV_EFFECT_GetMcssWork(), bevw->ball_mode );
  }
  else
  {
    ofs = ITEM_GetBallID( bevw->param.item_no );
  }

  //�s���ȃ{�[��ID�̓����X�^�[�{�[���ɂ���
  if( ( ofs == BALLID_NULL ) || ( ofs > BALLID_MAX ) )
  {
    ofs = BALLID_MONSUTAABOORU;
  }

  ofs -= 1;

  switch( datID ){
  case NARC_spa_be_ball_001_1_spa:
  case NARC_spa_be_capture_01_spa:
    datID += ofs;
    break;
  case NARC_spa_be_m_ball_01_close_spa:
  case NARC_spa_be_m_ball_01_fix_spa:
  case NARC_spa_be_m_ball_01_kaiten_spa:
  case NARC_spa_be_m_ball_01_open1_spa:
    datID += ofs * 4;
    break;
  default:
    break;
  }

  return datID;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ���C���{�����[������
 *
 *  @param[in]  bevw      �V�X�e���Ǘ��\����
 *  @param[in]  start_vol �J�n�{�����[��
 *  @param[in]  end_vol   �I���{�����[��
 *  @param[in]  frame     ����t���[����
 *
 *  @retval ARCDATID  �I�t�Z�b�g�v�Z������ARCDATID
 */
//-----------------------------------------------------------------------------
static  void  EFFVM_ChangeVolume( BTLV_EFFVM_WORK* bevw, fx32 start_vol, fx32 end_vol, int frame )
{
  BTLV_EFFVM_CHANGE_VOLUME* becv;

  if( start_vol > end_vol )
  { 
    //���łɃ{�����[���_�E�����Ă���Ȃ�A�Ȃɂ����Ȃ�
    if( bevw->volume_down_flag )
    { 
      bevw->volume_already_down = 1;
      return;
    }
    bevw->volume_down_flag = 1;
  }
  else
  { 
    bevw->volume_down_flag = 0;
  }

  becv = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_CHANGE_VOLUME ) );

  becv->start_vol = start_vol;
  becv->end_vol = end_vol;
  becv->bevw = bevw;
  BTLV_EFFTOOL_CalcMove( becv->start_vol, becv->end_vol, &becv->vec_vol, FX32_CONST( frame ) );
  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_ChangeVolume, becv, 0 ), NULL, GROUP_EFFVM );
}

//----------------------------------------------------------------------------
/**
 *  @brief  voicePlayIndex�̋󂢂Ă���INDEX��Ԃ�
 *
 *  @param[in]  bevw      �V�X�e���Ǘ��\����
 *
 *  @retval ARCDATID  �I�t�Z�b�g�v�Z������ARCDATID
 */
//-----------------------------------------------------------------------------
static  int EFFVM_GetVoicePlayerIndex( BTLV_EFFVM_WORK* bevw )
{ 
  int i;

  for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
  { 
    if( bevw->voiceplayerIndex[ i ] == EFFVM_VOICEPLAYER_INDEX_NONE )
    { 
      break;
    }
  }

  //�}�b�N�X�I�[�o�[
  GF_ASSERT( i != TEMOTI_POKEMAX );
  if( i == TEMOTI_POKEMAX )
  { 
    i = 0;
  }

  return i;
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
    EFFVM_SePlay( bes->se_no, bes->player, bes->pan, bes->pitch, bes->vol, bes->mod_depth, bes->mod_speed );
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_EFFVM_SEPLAY_CB( GFL_TCB* tcb )
{ 
  BTLV_EFFVM_SEPLAY*  bes = ( BTLV_EFFVM_SEPLAY* )GFL_TCB_GetWork( tcb );
  bes->bevw->se_play_wait_flag = 0;
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
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_EFFVM_SEEFFECT_CB( GFL_TCB* tcb )
{ 
  BTLV_EFFVM_SEEFFECT*  bes = ( BTLV_EFFVM_SEEFFECT* )GFL_TCB_GetWork( tcb );
  bes->bevw->se_effect_enable_flag = 0;
}

//============================================================================================
/**
 * @brief �����Đ�
 */
//============================================================================================
static  void  TCB_EFFVM_NAKIGOE( GFL_TCB* tcb, void* work )
{
  BTLV_EFFVM_NAKIGOE* ben = ( BTLV_EFFVM_NAKIGOE* )work;

  if( --ben->wait == 0 )
  {
    int index = EFFVM_GetVoicePlayerIndex( ben->bevw );
    ben->bevw->voiceplayerIndex[ index ] = BTLV_MCSS_PlayVoice( BTLV_EFFECT_GetMcssWork(), ben->pos, ben->pitch, ben->volume,
                                                                ben->chorus_vol, ben->chorus_speed, ben->play_dir );
    //BGM�̃}�X�^�[�{�����[����������
    {
      fx32  start_vol = 127 << FX32_SHIFT;
#ifdef KAGEYAMA_DEBUG
      fx32  end_vol = ( 127 * volume_down_ratio_pv / 100 ) << FX32_SHIFT;
      int   frame = volume_down_frame_pv;
#else
      fx32  end_vol = EFFVM_CHANGE_VOLUME_PV;
      int   frame = EFFVM_CHANGE_VOLUME_DOWN_FRAME_PV;
#endif
      EFFVM_ChangeVolume( ben->bevw, start_vol, end_vol, frame );
    }
    if( ben->bevw->volume_already_down == 0 )
    { 
      TCB_NONE_WORK*  tnw = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( ben->bevw->heapID ), sizeof( TCB_NONE_WORK ) );
      tnw->bevw = ben->bevw;
      BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( ben->bevw->tcbsys, TCB_EFFVM_NakigoeEndCheck, tnw, 0 ), NULL, GROUP_EFFVM );
    }
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

static  void  TCB_EFFVM_NAKIGOE_CB( GFL_TCB* tcb )
{ 
  BTLV_EFFVM_NAKIGOE* ben = ( BTLV_EFFVM_NAKIGOE* )GFL_TCB_GetWork( tcb );
  ben->bevw->nakigoe_wait_flag = 0;
}

//============================================================================================
/**
 * @brief �{�����[���ω�
 */
//============================================================================================
static  void  TCB_EFFVM_ChangeVolume( GFL_TCB* tcb, void* work )
{
  BTLV_EFFVM_CHANGE_VOLUME* becv = ( BTLV_EFFVM_CHANGE_VOLUME* )work;
  BOOL  ret = TRUE;

  BTLV_EFFTOOL_CheckMove( &becv->start_vol, &becv->vec_vol, &becv->end_vol, &ret );
  //PMSND_ChangeBGMVolume( 0xffff, becv->start_vol >> FX32_SHIFT );
  GFL_NET_DWC_PMSND_ChangeBGMVolume( 0xffff, becv->start_vol >> FX32_SHIFT );

  if( ret )
  {
    BTLV_EFFECT_FreeTCB( tcb );
  }
}

//============================================================================================
/**
 * @brief �E�C���h�E����
 */
//============================================================================================
static  void  TCB_EFFVM_WINDOW_MOVE( GFL_TCB* tcb, void* work )
{ 
  BTLV_EFFVM_WINDOW_MOVE* bevwm = ( BTLV_EFFVM_WINDOW_MOVE* )work;

  switch( bevwm->seq_no ){ 
  case 0:

    G2_SetWnd0InsidePlane( bevwm->in_out & 0x00ff, FALSE );
    G2_SetWnd1InsidePlane( bevwm->in_out & 0x00ff, FALSE );
    G2_SetWndOutsidePlane( ( bevwm->in_out & 0xff00 ) >> 8, FALSE );
    GX_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );
    G2_SetWnd1Position( 0, 0, 0, 0 );
    bevwm->seq_no++;
    /* fallthru */
  case 1:
    if( bevwm->wait == 0 )
    { 
      bevwm->wait = bevwm->wait_tmp;
      switch( bevwm->type ){ 
      case BTLEFF_WINDOW_VERTICAL_OPEN:
        bevwm->vertical -= 0x100;
        bevwm->vertical++;
        break;
      case BTLEFF_WINDOW_VERTICAL_CLOSE:
        bevwm->vertical += 0x100;
        bevwm->vertical--;
        break;
      case BTLEFF_WINDOW_HORIZON_OPEN:
        bevwm->horizon -= 0x100;
        bevwm->horizon++;
        break;
      case BTLEFF_WINDOW_HORIZON_CLOSE:
        bevwm->horizon += 0x100;
        bevwm->horizon--;
        break;
      }
      G2_SetWnd0Position( ( bevwm->horizon & 0xff00 ) >> 8,
                          ( bevwm->vertical & 0xff00 ) >> 8,
                          ( bevwm->horizon & 0x00ff ),
                          ( bevwm->vertical & 0x00ff ) );
      if( ( bevwm->horizon & 0xff ) == 0xff )
      { 
        G2_SetWnd1Position( 1,
                            ( bevwm->vertical & 0xff00 ) >> 8,
                            0,
                            ( bevwm->vertical & 0x00ff ) );
      }
      else
      { 
        G2_SetWnd1Position( 0, 0, 0, 0 );
      }
      if( --bevwm->frame == 0 )
      {
        BTLV_EFFECT_FreeTCB( tcb );
      }
    }
    else
    {
      bevwm->wait--;
    }
    break;
  }
}

static  void  TCB_EFFVM_WINDOW_MOVE_CB( GFL_TCB* tcb )
{ 
  BTLV_EFFVM_WINDOW_MOVE* bevwm = ( BTLV_EFFVM_WINDOW_MOVE* )GFL_TCB_GetWork( tcb );
  if( bevwm->flag == FALSE )
  {  
    GX_SetVisibleWnd( GX_WNDMASK_NONE );
  }
  bevwm->bevw->window_move_flag = 0;
}

//============================================================================================
/**
 * @brief �����I���`�F�b�N
 */
//============================================================================================
static  void  TCB_EFFVM_NakigoeEndCheck( GFL_TCB* tcb, void* work )
{ 
  TCB_NONE_WORK*  tnw = ( TCB_NONE_WORK* )work;
  int i;
  for( i = 0 ; i < TEMOTI_POKEMAX ; i++ )
  {
    if( tnw->bevw->voiceplayerIndex[ i ] != EFFVM_VOICEPLAYER_INDEX_NONE )
    { 
      if( PMVOICE_CheckPlay( tnw->bevw->voiceplayerIndex[ i ] ) )
      { 
        return;
      }
      else
      { 
        tnw->bevw->voiceplayerIndex[ i ] = EFFVM_VOICEPLAYER_INDEX_NONE;
      }
    }
  }
  {
#ifdef KAGEYAMA_DEBUG
    fx32  start_vol = ( 127 * volume_down_ratio_pv / 100 ) << FX32_SHIFT;
    int   frame = volume_up_frame_pv;
#else
    fx32  start_vol = EFFVM_CHANGE_VOLUME_PV;
    int   frame = EFFVM_CHANGE_VOLUME_UP_FRAME_PV;
#endif
    fx32  end_vol = 127 << FX32_SHIFT;
    EFFVM_ChangeVolume( tnw->bevw, start_vol, end_vol, frame );
  }
  tnw->bevw->volume_already_down = 0;
  BTLV_EFFECT_FreeTCB( tcb );
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
void  BTLV_EFFVM_StartDebug( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, const VM_CODE *start, const DEBUG_PARTICLE_DATA *dpd, BTLV_EFFVM_PARAM* param, BTLV_EFFVM_EXECUTE_EFF_TYPE type )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  int *start_ofs = (int *)&start[ script_table[ from ][ to ] ] ;
  int i;

  BTLV_EFFECT_FreeTCBGroup( GROUP_EFFVM );

  if( bevw->pause_flag )
  {
    BTLV_EFFVM_Restart( vmh );
    return;
  }

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

  bevw->execute_effect_type = type;

  //BGM�̃}�X�^�[�{�����[����������
  {
    fx32  start_vol = 127 << FX32_ONE;
#ifdef KAGEYAMA_DEBUG
    fx32  end_vol = ( 127 * volume_down_ratio / 100 ) << FX32_SHIFT;
    int   frame = volume_down_frame;
#else
    fx32  end_vol = EFFVM_CHANGE_VOLUME;
    int   frame = EFFVM_CHANGE_VOLUME_DOWN_FRAME;
#endif
    EFFVM_ChangeVolume( bevw, start_vol, end_vol, frame );
  }

  bevw->attack_pos = from;
  bevw->defence_pos = to;
  bevw->dpd = dpd;
  bevw->debug_flag = TRUE;
  bevw->camera_projection = BTLEFF_CAMERA_PROJECTION_PERSPECTIVE;
  bevw->ball_mode = BTLEFF_USE_BALL;
  VM_Start( vmh, &start[ start_ofs[ 0 ] ] );
}

//============================================================================================
/**
 * @brief VM�N���i�f�o�b�O�p�R�[�����^�[���R�}���h�X���[�o�[�W�����j
 *
 * @param[in] vmh ���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void  BTLV_EFFVM_StartThrough( VMHANDLE *vmh, BtlvMcssPos from, BtlvMcssPos to, WazaID waza, BTLV_EFFVM_PARAM* param )
{
  BTLV_EFFVM_WORK *bevw = (BTLV_EFFVM_WORK *)VM_GetContext( vmh );
  bevw->debug_return_through = TRUE;
  BTLV_EFFVM_Start( vmh, from, to, waza, param );
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
  BTLV_EFFVM_EMIT_INIT_WORK *beeiw = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID( bevw->heapID ),
                                                                sizeof( BTLV_EFFVM_EMIT_INIT_WORK ) );

  beeiw->vmh = vmh;
  beeiw->src = src;
  beeiw->dst = dst;
  beeiw->ofs.x = 0;
  beeiw->ofs.y = ofs_y;
  beeiw->ofs.z = 0;
  beeiw->angle = angle;

  if( GFL_PTC_CreateEmitterCallback( ptc, index, &EFFVM_InitEmitterPos, beeiw ) == PTC_NON_CREATE_EMITTER )
  {
    GFL_HEAP_FreeMemory( beeiw );
  }
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
  BTLV_EFFVM_SEEFFECT*  bes = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( bevw->heapID ), sizeof( BTLV_EFFVM_SEEFFECT ) );

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

  BTLV_EFFECT_SetTCB( GFL_TCB_AddTask( bevw->tcbsys, TCB_EFFVM_SEEFFECT, bes, 0 ), TCB_EFFVM_SEEFFECT_CB, GROUP_EFFVM );
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

