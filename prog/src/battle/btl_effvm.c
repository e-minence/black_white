
//============================================================================================
/**
 * @file	btl_effvm.c
 * @brief	�퓬�G�t�F�N�g�X�N���v�g�R�}���h���
 * @author	soga
 * @date	2008.11.21
 */
//============================================================================================

#include <gflib.h>
#include "system/vm_cmd.h"

#include "btl_effect.h"
#include "btl_effvm.h"

#include "arc_def.h"
#include "spa.naix"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define	EFFVM_PTCNO_NONE	( 0xffff )		//ptc_no�̖��i�[�̂Ƃ��̒l

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

typedef struct{
	HEAPID		heapID;
	u32			position_reverse_flag	:1;			//�����ʒu���]�t���O
	u32									:31;
	GFL_PTC_PTR	ptc[ PARTICLE_GLOBAL_MAX ];
	u16			ptc_no[ PARTICLE_GLOBAL_MAX ];
}BTL_EFFVM_WORK;

typedef struct{
	VMHANDLE	*vmh;
	int			src;
	int			dst;
	fx32		angle;
}BTL_EFFVM_EMIT_INIT_WORK;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

VMHANDLE	*BTL_EFFVM_Init( HEAPID heapID );
void		BTL_EFFVM_Exit( VMHANDLE *core );

//�G�t�F�N�g�R�}���h
static VMCMD_RESULT EC_CAMERA_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT EC_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT EC_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT EC_PARTICLE_PLAY_WITH_DIR( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT	EC_POKEMON_MOVE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT	EC_POKEMON_SCALE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT	EC_POKEMON_ROTATE( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT	EC_POKEMON_SET_MEPACHI_FLAG( VMHANDLE *vmh, void *context_work );
static VMCMD_RESULT	EC_POKEMON_SET_ANM_FLAG( VMHANDLE *vmh, void *context_work );

static VMCMD_RESULT EC_SEQ_END( VMHANDLE *vmh, void *context_work );

//VM_WAIT_FUNC�Q
static	BOOL	VWF_CAMERA_MOVE_CHECK( VMHANDLE *vmh, void *context_work );

//����J�֐��Q
static	int		EFFVM_ConvPosition( VMHANDLE *vmh, PokeMcssPos position );
static	int		EFFVM_RegistPtcNo( BTL_EFFVM_WORK *bevw, ARCDATID datID );
static	int		EFFVM_GetPtcNo( BTL_EFFVM_WORK *bevw, ARCDATID datID );
static	void	EFFVM_InitEmitterPos( GFL_EMIT_PTR emit );

//============================================================================================
/**
 *	�X�N���v�g�e�[�u��
 */
//============================================================================================
static const VMCMD_FUNC btl_effect_command_table[]={
	EC_CAMERA_MOVE,
	EC_PARTICLE_LOAD,
	EC_PARTICLE_PLAY,
	EC_PARTICLE_PLAY_WITH_DIR,
	EC_POKEMON_MOVE,
	EC_POKEMON_SCALE,
	EC_POKEMON_ROTATE,
	EC_POKEMON_SET_MEPACHI_FLAG,
	EC_POKEMON_SET_ANM_FLAG,

	EC_SEQ_END,
};

//============================================================================================
/**
 *	VM�C�j�V�����C�U�e�[�u��
 */
//============================================================================================
static	const VM_INITIALIZER	vm_init={
	BTL_EFFVM_STACK_SIZE,				//u16 stack_size;	///<�g�p����X�^�b�N�̃T�C�Y
	BTL_EFFVM_REG_SIZE,					//u16 reg_size;		///<�g�p���郌�W�X�^�̐�
	btl_effect_command_table,			//const VMCMD_FUNC * command_table;	///<�g�p���鉼�z�}�V�����߂̊֐��e�[�u��
	NELEMS( btl_effect_command_table ),	//const u32 command_max;			///<�g�p���鉼�z�}�V�����ߒ�`�̍ő吔
};

//============================================================================================
/**
 *	VM������
 *
 * @param[in]	heapID			�q�[�vID
 */
//============================================================================================
VMHANDLE	*BTL_EFFVM_Init( HEAPID heapID )
{
	int			i;
	VMHANDLE	*core;
	BTL_EFFVM_WORK	*bevw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_EFFVM_WORK ) );

	bevw->heapID = heapID;

	for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ ){
		bevw->ptc_no[ i ] = EFFVM_PTCNO_NONE;
	}

	core = VM_Create( heapID, &vm_init );
	VM_Init( core, bevw );

	return core;
}

//============================================================================================
/**
 *	VM�I��
 *
 * @param[in]	core	���z�}�V������\���̂ւ̃|�C���^
 */
//============================================================================================
void	BTL_EFFVM_Exit( VMHANDLE *core )
{
	GFL_HEAP_FreeMemory (VM_GetContext( core ) );
	VM_Delete( core );
}

//============================================================================================
/**
 *	�J�����ړ�
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT EC_CAMERA_MOVE( VMHANDLE *vmh, void *context_work )
{
	VecFx32		cam_pos,cam_target;
	//�J�����^�C�v�ǂݍ���
	int			cam_type = ( int )VMGetU32( vmh );
	int			cam_move_pos = ( int )VMGetU32( vmh );
	int			frame,brake;
	static	VecFx32	cam_pos_table[]={
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_AA	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_BB	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_A
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_B
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_C	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_D
	};
	static	VecFx32	cam_target_table[]={
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_AA	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_BB	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_A
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_B
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_C	
		{ FX32_ONE * 0, FX32_ONE * 0, FX32_ONE * 0 },	//BTLEFF_CAMERA_POS_D
	};

	cam_move_pos = EFFVM_ConvPosition( vmh, cam_move_pos );

	switch( cam_move_pos ){
	case BTLEFF_CAMERA_POS_AA:
	case BTLEFF_CAMERA_POS_BB:
	case BTLEFF_CAMERA_POS_A:
	case BTLEFF_CAMERA_POS_B:
	case BTLEFF_CAMERA_POS_C:
	case BTLEFF_CAMERA_POS_D:
		//�J�����ړ���ʒu��ǂݍ���
		cam_pos.x = cam_pos_table[ cam_move_pos ].x;
		cam_pos.y = cam_pos_table[ cam_move_pos ].y;
		cam_pos.z = cam_pos_table[ cam_move_pos ].z;
		//�J�����ړ���^�[�Q�b�g��ǂݍ���
		cam_target.x = cam_target_table[ cam_move_pos ].x;
		cam_target.y = cam_target_table[ cam_move_pos ].y;
		cam_target.z = cam_target_table[ cam_move_pos ].z;
		break;
	case BTLEFF_CAMERA_POS_INIT:
	default:
		BTL_CAMERA_GetDefaultCameraPosition( &cam_pos, &cam_target );
		break;
	}
	//�ړ��t���[������ǂݍ���
	frame = ( int )VMGetU32( vmh );
	//�u���[�L�t���[������ǂݍ���
	brake = ( int )VMGetU32( vmh );

	//�J�����^�C�v����ړ�����v�Z
	switch( cam_type ){
	case BTLEFF_CAMERA_MOVE_DIRECT:		//�_�C���N�g
		BTL_CAMERA_MoveCameraPosition( BTL_EFFECT_GetCameraWork(), &cam_pos, &cam_target );
		break;
	case BTLEFF_CAMERA_MOVE_INTERPOLATION:	//�Ǐ]
		BTL_CAMERA_MoveCameraInterpolation( BTL_EFFECT_GetCameraWork(), &cam_pos, &cam_target, frame, brake );
		break;
	}
	VMCMD_SetWait( vmh, VWF_CAMERA_MOVE_CHECK );

	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	�p�[�e�B�N�����\�[�X���[�h
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT EC_PARTICLE_LOAD( VMHANDLE *vmh, void *context_work )
{
	BTL_EFFVM_WORK	*bevw = ( BTL_EFFVM_WORK* )context_work;
	void			*heap;
	void			*resource;
	ARCDATID		datID = ( ARCDATID )VMGetU32( vmh );
	int				ptc_no = EFFVM_RegistPtcNo( bevw, datID );

	heap = GFL_HEAP_AllocMemory( bevw->heapID, PARTICLE_LIB_HEAP_SIZE );
	bevw->ptc[ ptc_no ] = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, bevw->heapID );
	resource = GFL_PTC_LoadArcResource( ARCID_PTC, datID, bevw->heapID );
	GFL_PTC_SetResource( bevw->ptc[ ptc_no ], resource, FALSE, GFUser_VIntr_GetTCBSYS() );
	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	�p�[�e�B�N���Đ�
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT EC_PARTICLE_PLAY( VMHANDLE *vmh, void *context_work )
{
	BTL_EFFVM_WORK	*bevw = ( BTL_EFFVM_WORK* )context_work;
	BTL_EFFVM_EMIT_INIT_WORK	*beeiw = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTL_EFFVM_EMIT_INIT_WORK ) );
	ARCDATID	datID = ( ARCDATID )VMGetU32( vmh );
	int			ptc_no = EFFVM_GetPtcNo( bevw, datID );
	int			index = ( int )VMGetU32( vmh );
	
	beeiw->vmh = vmh;
	beeiw->src = beeiw->dst = ( int )VMGetU32( vmh );

	GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	�p�[�e�B�N���Đ��i���W�w�肠��j
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT EC_PARTICLE_PLAY_WITH_DIR( VMHANDLE *vmh, void *context_work )
{
	BTL_EFFVM_WORK	*bevw = ( BTL_EFFVM_WORK* )context_work;
	BTL_EFFVM_EMIT_INIT_WORK	*beeiw = GFL_HEAP_AllocMemory( bevw->heapID, sizeof( BTL_EFFVM_EMIT_INIT_WORK ) );
	ARCDATID	datID = ( ARCDATID )VMGetU32( vmh );
	int			ptc_no = EFFVM_GetPtcNo( bevw, datID );
	int			index = ( int )VMGetU32( vmh );
	
	beeiw->vmh = vmh;
	beeiw->src = ( int )VMGetU32( vmh );
	beeiw->dst = ( int )VMGetU32( vmh );
	beeiw->angle = ( fx32 )VMGetU32( vmh );

	GFL_PTC_CreateEmitterCallback( bevw->ptc[ ptc_no ], index, &EFFVM_InitEmitterPos, beeiw );

	return VMCMD_RESULT_SUSPEND;
}

//============================================================================================
/**
 *	�|�P�����ړ�
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT	EC_POKEMON_MOVE( VMHANDLE *vmh, void *context_work )
{
#if 0
	BTL_EFFVM_WORK	*bevw = ( BTL_EFFVM_WORK* )context_work;
	int	pos = ( int )VMGetU32( vmh );
 * @param	type		�ړ��^�C�v
 * @param	move_pos_x	�ړ���X���W
 * @param	move_pos_y	�ړ���Y���W
 * @param	speed		�ړ��X�s�[�h
 * @param	wait		�ړ��E�G�C�g
 * @param	count		�����J�E���g�i�ړ��^�C�v�������̂Ƃ������L���j
 *
 * #param	COMBOBOX_TEXT	�U����	�U�����y�A�@�h�䑤�@�h�䑤�y�A
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_SIDE_ATTACK BTLEFF_POKEMON_SIDE_ATTACK_PAIR BTLEFF_POKEMON_SIDE_DEFENCE BTLEFF_POKEMON_SIDE_DEFENCE_PAIR
 * #param	COMBOBOX_TEXT	�_�C���N�g	�Ǐ]	����	���������O
 * #param	COMBOBOX_VALUE	BTLEFF_POKEMON_MOVE_DIRECT	BTLEFF_POKEMON_MOVE_INTERPOLATION	BTLEFF_POKEMON_MOVE_ROUNDTRIP	BTLEFF_POKEMON_MOVE_ROUNDTRIP_LONG
 * #param	VALUE_FX32
 * #param	VALUE_FX32
 * #param	VALUE_INT
 * #param	VALUE_INT
 * #param	VALUE_INT
#endif
}

//============================================================================================
/**
 *	�|�P�����g�k
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT	EC_POKEMON_SCALE( VMHANDLE *vmh, void *context_work )
{
}

//============================================================================================
/**
 *	�|�P������]
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT	EC_POKEMON_ROTATE( VMHANDLE *vmh, void *context_work )
{
}

//============================================================================================
/**
 *	�|�P�������p�`�t���O�Z�b�g
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT	EC_POKEMON_SET_MEPACHI_FLAG( VMHANDLE *vmh, void *context_work )
{
}

//============================================================================================
/**
 *	�|�P�����A�j���[�V�����t���O�Z�b�g
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT	EC_POKEMON_SET_ANM_FLAG( VMHANDLE *vmh, void *context_work )
{
}

//============================================================================================
/**
 *	�G�t�F�N�g�V�[�P���X�I��
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static VMCMD_RESULT EC_SEQ_END( VMHANDLE *vmh, void *context_work )
{
	BTL_EFFVM_WORK	*bevw = ( BTL_EFFVM_WORK* )context_work;
	int	i;

	//�������Ă��Ȃ��p�[�e�B�N�����������������Ă���
	for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ ){
		if( bevw->ptc[ i ] ){
			GFL_PTC_Delete( bevw->ptc[ i ] );
			bevw->ptc[ i ] = NULL;
		}
		bevw->ptc_no[ i ] = EFFVM_PTCNO_NONE; 
	}

	//���z�}�V����~
	VM_End( vmh );

	return VMCMD_RESULT_SUSPEND;
}

//VM_WAIT_FUNC�Q
//============================================================================================
/**
 *	�J�����ړ��I���`�F�b�N
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 * @param[in]	context_work	�R���e�L�X�g���[�N�ւ̃|�C���^
 */
//============================================================================================
static	BOOL	VWF_CAMERA_MOVE_CHECK( VMHANDLE *vmh, void *context_work )
{
	return ( BTL_CAMERA_CheckExecute( BTL_EFFECT_GetCameraWork() ) == FALSE );
}

//����J�֐��Q
//============================================================================================
/**
 *	�����ʒu���̕ϊ��i���]�t���O�����ēK�؂ȗ����ʒu��Ԃ��j
 *
 * @param[in]	vmh				���z�}�V������\���̂ւ̃|�C���^
 *
 * @retval	�K�؂ȗ����ʒu
 */
//============================================================================================
static	int		EFFVM_ConvPosition( VMHANDLE *vmh, PokeMcssPos position )
{
	BTL_EFFVM_WORK *bevw = (BTL_EFFVM_WORK *)VM_GetContext( vmh );

	if( bevw->position_reverse_flag ){
		position ^= 1;
	}

	return position;
}

//============================================================================================
/**
 *	�p�[�e�B�N����datID��o�^
 *
 * @param[in]	bevw	�G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in]	datID	�A�[�J�C�udatID
 *
 * @retval	�o�^����ptc�z��̓Y����No
 */
//============================================================================================
static	int	EFFVM_RegistPtcNo( BTL_EFFVM_WORK *bevw, ARCDATID datID )
{
	int	i;

	for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ ){
		if( bevw->ptc_no[ i ] == datID ){
			break;
		}
	}

	if( i == PARTICLE_GLOBAL_MAX ){
		for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ ){
			if( bevw->ptc_no[ i ] == EFFVM_PTCNO_NONE ){
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
 *	�p�[�e�B�N����datID����ptc�z��̓Y����No���擾
 *
 * @param[in]	bevw	�G�t�F�N�g���z�}�V���̃��[�N�\���̂ւ̃|�C���^
 * @param[in]	datID	�A�[�J�C�udatID
 *
 * @retval	ptc�z��̓Y����No
 */
//============================================================================================
static	int	EFFVM_GetPtcNo( BTL_EFFVM_WORK *bevw, ARCDATID datID )
{
	int	i;

	for( i = 0 ; i < PARTICLE_GLOBAL_MAX ; i++ ){
		if( bevw->ptc_no[ i ] == datID ){
			break;
		}
	}

	GF_ASSERT( i != PARTICLE_GLOBAL_MAX );

	return i;
}

//============================================================================================
/**
 *	�G�~�b�^�������ɌĂ΂��G�~�b�^�������p�R�[���o�b�N�֐��i�����ʒu����v�Z�j
 *
 * @param[in]	emit	�G�~�b�^���[�N�\���̂ւ̃|�C���^
 */
//============================================================================================
static	void	EFFVM_InitEmitterPos( GFL_EMIT_PTR emit )
{
	BTL_EFFVM_EMIT_INIT_WORK	*beeiw = ( BTL_EFFVM_EMIT_INIT_WORK* )GFL_PTC_GetTempPtr();
	VecFx32	src,dst;
	VecFx16	dir;

	beeiw->src = EFFVM_ConvPosition( beeiw->vmh, beeiw->src );
	beeiw->dst = EFFVM_ConvPosition( beeiw->vmh, beeiw->dst );

	POKE_MCSS_GetPokeDefaultPos( &src, beeiw->src );
	POKE_MCSS_GetPokeDefaultPos( &dst, beeiw->dst );

	//src��dst���ꏏ�̂Ƃ��́A�����Ȃ�
	if( beeiw->src == beeiw->dst ){
		dst.x -= src.x;
		dst.y -= src.y;
		dst.z -= src.z;

		VEC_Normalize( &dst, &dst );
		VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
		GFL_PTC_SetEmitterAxis( emit, &dir );
	}

	GFL_PTC_SetEmitterPosition( emit, &src );

	GFL_HEAP_FreeMemory( beeiw );
}

