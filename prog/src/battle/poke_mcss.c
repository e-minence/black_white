
//============================================================================================
/**
 * @file	poke_mcss.c
 * @brief	�|�P����MCSS�Ǘ�
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"

#include "btl_efftool.h"
#include "poke_mcss.h"

#include "arc_def.h"
#include "pokegra/pokegra_wb.naix"
#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define	POKE_MCSS_MAX	( 6 )	//POKE_MCSS�ŊǗ�����MCSS��MAX�l

#define	POKE_MCSS_DEFAULT_SHIFT	( FX32_SHIFT - 4 )					//�|���S���P�ӂ̊�̒����ɂ���V�t�g�l
#define	POKE_MCSS_DEFAULT_LINE	( 1 << POKE_MCSS_DEFAULT_SHIFT )	//�|���S���P�ӂ̊�̒���

//�|�P������̂��\������MCSS�p�t�@�C���̍\��
enum{
	POKEGRA_FRONT_M_NCGR = 0,
	POKEGRA_FRONT_F_NCGR,
	POKEGRA_FRONT_M_NCBR,
	POKEGRA_FRONT_F_NCBR,
	POKEGRA_FRONT_NCER,
	POKEGRA_FRONT_NANR,
	POKEGRA_FRONT_NMCR,
	POKEGRA_FRONT_NMAR,
	POKEGRA_FRONT_NCEC,
	POKEGRA_BACK_M_NCGR,
	POKEGRA_BACK_F_NCGR,
	POKEGRA_BACK_M_NCBR,
	POKEGRA_BACK_F_NCBR,
	POKEGRA_BACK_NCER,
	POKEGRA_BACK_NANR,
	POKEGRA_BACK_NMCR,
	POKEGRA_BACK_NMAR,
	POKEGRA_BACK_NCEC,
	POKEGRA_NORMAL_NCLR,
	POKEGRA_RARE_NCLR,

	POKEGRA_FILE_MAX,			//�|�P������̂��\������MCSS�p�t�@�C���̑���

	POKEGRA_M_NCGR = 0,
	POKEGRA_F_NCGR,
	POKEGRA_M_NCBR,
	POKEGRA_F_NCBR,
	POKEGRA_NCER,
	POKEGRA_NANR,
	POKEGRA_NMCR,
	POKEGRA_NMAR,
	POKEGRA_NCEC
};

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _POKE_MCSS_WORK
{
	GFL_TCBSYS				*tcb_sys;
	MCSS_SYS_WORK			*mcss_sys;
	MCSS_WORK				*mcss[ POKE_MCSS_POS_MAX ];
	u32						poke_mcss_ortho_mode	:1;
	u32												:31;
	HEAPID					heapID;
};

typedef struct
{
	POKE_MCSS_WORK		*pmw;
	EFFTOOL_MOVE_WORK	emw;
}POKE_MCSS_TCB_WORK;

typedef struct	
{
	NNSG2dCharacterData*	pCharData;			//�e�N�X�`���L����
	NNSG2dPaletteData*		pPlttData;			//�e�N�X�`���p���b�g
	void					*pBufChar;			//�e�N�X�`���L�����o�b�t�@
	void					*pBufPltt;			//�e�N�X�`���p���b�g�o�b�t�@
	int						chr_ofs;
	int						pal_ofs;
	POKE_MCSS_WORK			*pmw;
}TCB_LOADRESOURCE_WORK;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

POKE_MCSS_WORK	*POKE_MCSS_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID );
void			POKE_MCSS_Exit( POKE_MCSS_WORK *pmw );
void			POKE_MCSS_Main( POKE_MCSS_WORK *pmw );
void			POKE_MCSS_Draw( POKE_MCSS_WORK *pmw );
void			POKE_MCSS_Add( POKE_MCSS_WORK *pmw, const POKEMON_PARAM *pp, int position );
void			POKE_MCSS_Del( POKE_MCSS_WORK *pmw, int position );
void			POKE_MCSS_SetOrthoMode( POKE_MCSS_WORK *pmw );
void			POKE_MCSS_ResetOrthoMode( POKE_MCSS_WORK *pmw );
void			POKE_MCSS_SetMepachiFlag( POKE_MCSS_WORK *pmw, int position, int flag );
void			POKE_MCSS_SetAnmStopFlag( POKE_MCSS_WORK *pmw, int position, int flag );
void			POKE_MCSS_GetPokeDefaultPos( VecFx32 *pos, int position );
fx32			POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_WORK *pmw, int position );
void			POKE_MCSS_GetScale( POKE_MCSS_WORK *pmw, int position, VecFx32 *scale );
void			POKE_MCSS_SetScale( POKE_MCSS_WORK *pmw, int position, VecFx32 *scale );

void			POKE_MCSS_MovePosition( POKE_MCSS_WORK *pmw, int position, int move_type, VecFx32 *pos, int speed, int wait, int count );
void			POKE_MCSS_MoveScale( POKE_MCSS_WORK *pmw, int position, int move_type, VecFx32 *scale, int speed, int wait, int count );

static	void	POKE_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position );
static	void	POKE_MCSS_SetDefaultScale( POKE_MCSS_WORK *pmw, int position );

static	void	TCB_POKE_MCSS_Move( GFL_TCB *tcb, void *work );
static	void	TCB_POKE_MCSS_Scale( GFL_TCB *tcb, void *work );

//============================================================================================
/**
 *	�|�P�����̗����ʒu�e�[�u��
 */
//============================================================================================
static	const	VecFx32	poke_pos_table[]={
	{ FX_F32_TO_FX32( -2.5f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(   8.0f ) },		//POS_AA
	{ FX_F32_TO_FX32(  4.5f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -10.0f ) },		//POS_BB
	{ FX_F32_TO_FX32( -3.5f ),	FX_F32_TO_FX32( 1.2f ), FX_F32_TO_FX32(   8.5f ) },		//POS_A
	{ FX_F32_TO_FX32(  6.0f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(  -9.0f ) },		//POS_B
	{ FX_F32_TO_FX32( -0.5f ),	FX_F32_TO_FX32( 1.2f ), FX_F32_TO_FX32(   9.0f ) },		//POS_C
	{ FX_F32_TO_FX32(  2.0f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -11.0f ) },		//POS_D
	{ FX_F32_TO_FX32( -2.5f ),	FX_F32_TO_FX32( 1.2f ), FX_F32_TO_FX32(  10.0f ) },		//POS_E
	{ FX_F32_TO_FX32(  4.5f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -10.0f ) },		//POS_F
};

//============================================================================================
/**
 *	�|�P�����̗����ʒu�ɂ��X�P�[���␳�e�[�u��
 */
//============================================================================================
static	const	fx32	poke_scale_table[2][POKE_MCSS_POS_MAX]={
	{
		0x08c4,	//POS_AA
		0x1386,	//POS_BB
		0x0873,	//POS_A
		0x1322,	//POS_B
		0x0831,	//POS_C
		0x141e,	//POS_D
		0x1000,	//POS_E
		0x1000,	//POS_F
	},
	{
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
		FX32_ONE * 16,
	}
};

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	tcb_sys	�V�X�e�����Ŏg�p����TCBSYS�\���̂ւ̃|�C���^
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
POKE_MCSS_WORK	*POKE_MCSS_Init( GFL_TCBSYS	*tcb_sys, HEAPID heapID )
{
	POKE_MCSS_WORK *pmw = GFL_HEAP_AllocClearMemory( heapID, sizeof( POKE_MCSS_WORK ) );

	pmw->mcss_sys = MCSS_Init( POKE_MCSS_MAX, heapID );
	pmw->tcb_sys  = tcb_sys;

	return pmw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	pmw	POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_Exit( POKE_MCSS_WORK *pmw )
{
	MCSS_Exit( pmw->mcss_sys );
	GFL_HEAP_FreeMemory( pmw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	pmw	POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_Main( POKE_MCSS_WORK *pmw )
{
	MCSS_Main( pmw->mcss_sys );
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	pmw	POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_Draw( POKE_MCSS_WORK *pmw )
{
	MCSS_Draw( pmw->mcss_sys );
}

//============================================================================================
/**
 *	POKE_MCSS�ǉ�
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pp			POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
void	POKE_MCSS_Add( POKE_MCSS_WORK *pmw, const POKEMON_PARAM *pp, int position )
{
	MCSS_ADD_WORK	maw;

	GF_ASSERT( position < POKE_MCSS_POS_MAX );
	GF_ASSERT( pmw->mcss[ position ] == NULL );

	POKE_MCSS_MakeMAW( pp, &maw, position );
	pmw->mcss[ position ] = MCSS_Add( pmw->mcss_sys,
									  poke_pos_table[ position ].x,
									  poke_pos_table[ position ].y,
									  poke_pos_table[ position ].z,
									  &maw );

	POKE_MCSS_SetDefaultScale( pmw, position );
}

//============================================================================================
/**
 *	POKE_MCSS�폜
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
void	POKE_MCSS_Del( POKE_MCSS_WORK *pmw, int position )
{
	MCSS_Del( pmw->mcss_sys, pmw->mcss[ position ] );
	pmw->mcss[ position ] = NULL;
}

//============================================================================================
/**
 *	���ˉe�`�惂�[�hON
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_SetOrthoMode( POKE_MCSS_WORK *pmw )
{
	int	position;

	MCSS_SetOrthoMode( pmw->mcss_sys );

	pmw->poke_mcss_ortho_mode = 1;

	for( position = 0 ; position < POKE_MCSS_POS_MAX ; position++ ){
		if( pmw->mcss[ position ] ){
			POKE_MCSS_SetDefaultScale( pmw, position );
		}
	}
}

//============================================================================================
/**
 *	���ˉe�`�惂�[�hOFF
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_ResetOrthoMode( POKE_MCSS_WORK *pmw )
{
	int	position;

	MCSS_ResetOrthoMode( pmw->mcss_sys );

	pmw->poke_mcss_ortho_mode = 0;

	for( position = 0 ; position < POKE_MCSS_POS_MAX ; position++ ){
		if( pmw->mcss[ position ] ){
			POKE_MCSS_SetDefaultScale( pmw, position );
		}
	}
}

//============================================================================================
/**
 *	���p�`����
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	���p�`���������|�P�����̗����ʒu
 * @param[in]	flag		���p�`�t���O�iPOKE_MCSS_MEPACHI_ON�APOKE_MCSS_MEPACHI_OFF�j
 */
//============================================================================================
void	POKE_MCSS_SetMepachiFlag( POKE_MCSS_WORK *pmw, int position, int flag )
{
	GF_ASSERT( pmw->mcss[ position ] != NULL );
	if( flag == POKE_MCSS_MEPACHI_ON ){
		MCSS_SetMepachiFlag( pmw->mcss[ position ] );
	}
	else{
		MCSS_ResetMepachiFlag( pmw->mcss[ position ] );
	}
}

//============================================================================================
/**
 *	�A�j���X�g�b�v����
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�A�j���X�g�b�v���������|�P�����̗����ʒu
 * @param[in]	flag		�A�j���X�g�b�v�t���O�iPOKE_MCSS_ANM_STOP_ON�APOKE_MCSS_ANM_STOP_OFF�j
 */
//============================================================================================
void	POKE_MCSS_SetAnmStopFlag( POKE_MCSS_WORK *pmw, int position, int flag )
{
	GF_ASSERT( pmw->mcss[ position ] != NULL );
	if( flag == POKE_MCSS_ANM_STOP_ON ){
		MCSS_SetAnmStopFlag( pmw->mcss[ position ] );
	}
	else{
		MCSS_ResetAnmStopFlag( pmw->mcss[ position ] );
	}
}

//============================================================================================
/**
 *	�|�P�����̏��������ʒu���擾
 *
 * @param[out]	pos			���������ʒu���i�[���郏�[�N�ւ̃|�C���^
 * @param[in]	position	�擾����|�P�����̗����ʒu
 */
//============================================================================================
void	POKE_MCSS_GetPokeDefaultPos( VecFx32 *pos, int position )
{
	pos->x = poke_pos_table[ position ].x;
	pos->y = poke_pos_table[ position ].y;
	pos->z = poke_pos_table[ position ].z;
}

//============================================================================================
/**
 *	�|�P�����̏����g�k�����擾
 *
 * @param[in]	position	�擾����|�P�����̗����ʒu
 */
//============================================================================================
fx32	POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_WORK *pmw, int position )
{
	return poke_scale_table[ pmw->poke_mcss_ortho_mode ][ position ];
}

//============================================================================================
/**
 *	�|�P�����̃X�P�[���l���擾
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�擾����|�P�����̗����ʒu
 * @param[out]	scale		�擾�����X�P�[���l���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	POKE_MCSS_GetScale( POKE_MCSS_WORK *pmw, int position, VecFx32 *scale )
{
	MCSS_GetScale( pmw->mcss[ position ], scale );
}

//============================================================================================
/**
 *	�|�P�����̃X�P�[���l���i�[
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�i�[����|�P�����̗����ʒu
 * @param[in]	scale		�i�[����X�P�[���l
 */
//============================================================================================
void	POKE_MCSS_SetScale( POKE_MCSS_WORK *pmw, int position, VecFx32 *scale )
{
	MCSS_SetScale( pmw->mcss[ position ], scale );
}

//============================================================================================
/**
 *	�|�P�����ړ�
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�ړ�����|�P�����̗����ʒu
 * @param[in]	move_type	�ړ��^�C�v
 * @param[in]	pos			�ړ��^�C�v�ɂ��Ӗ����ω�
 *							EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION	�ړ���
 *							EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in]	speed		�ړ��X�s�[�h
 * @param[in]	count		�����J�E���g�iPOKEMCSS_CALCYUPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void	POKE_MCSS_MovePosition( POKE_MCSS_WORK *pmw, int position, int move_type, VecFx32 *pos, int speed, int wait, int count )
{
	POKE_MCSS_TCB_WORK	*pmtw = GFL_HEAP_AllocMemory( pmw->heapID, sizeof( POKE_MCSS_TCB_WORK ) );

	pmtw->pmw				= pmw;
	pmtw->emw.position		= position;
	pmtw->emw.move_type		= move_type;
	pmtw->emw.vec_time		= speed;
	pmtw->emw.vec_time_tmp	= speed;
	pmtw->emw.wait			= 0;
	pmtw->emw.wait_tmp		= wait;
	pmtw->emw.count			= count;
	pmtw->emw.end_value.x	= pos->x;
	pmtw->emw.end_value.y	= pos->y;
	pmtw->emw.end_value.z	= pos->z;

	MCSS_GetPosition( pmw->mcss[ position ], &pmtw->emw.start_value );

	switch( move_type ){
	case EFFTOOL_CALCTYPE_DIRECT:			//���ڃ|�W�V�����Ɉړ�
		break;
	case EFFTOOL_CALCTYPE_INTERPOLATION:	//�ړ���܂ł��Ԃ��Ȃ���ړ�
		BTL_EFFTOOL_CalcMoveVector( &pmtw->emw.start_value, pos, &pmtw->emw.vector, FX32_CONST( speed ) );
		break;
	case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG:	//�w�肵����Ԃ������ړ�
		pmtw->emw.vec_time_tmp	*= 2;
	case EFFTOOL_CALCTYPE_ROUNDTRIP:		//�w�肵����Ԃ������ړ�
		pmtw->emw.vector.x = FX_Div( pos->x, FX32_CONST( speed ) );
		pmtw->emw.vector.y = FX_Div( pos->y, FX32_CONST( speed ) );
		pmtw->emw.vector.z = FX_Div( pos->z, FX32_CONST( speed ) );
		break;
	}

	GFL_TCB_AddTask( pmw->tcb_sys, TCB_POKE_MCSS_Move, pmtw, 0 );
}

//============================================================================================
/**
 *	�|�P�����g�k
 *
 * @param[in]	pmw			POKE_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�g�k����|�P�����̗����ʒu
 * @param[in]	scale_type	�g�k�^�C�v
 * @param[in]	scale		�g�k�^�C�v�ɂ��Ӗ����ω�
 *							EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION	�ŏI�I�ȃX�P�[���l
 *							EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in]	speed		�g�k�X�s�[�h
 * @param[in]	wait		�g�k�E�G�C�g
 * @param[in]	count		�����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void	POKE_MCSS_MoveScale( POKE_MCSS_WORK *pmw, int position, int move_type, VecFx32 *scale, int speed, int wait, int count )
{
	POKE_MCSS_TCB_WORK	*pmtw = GFL_HEAP_AllocMemory( pmw->heapID, sizeof( POKE_MCSS_TCB_WORK ) );

	pmtw->pmw				= pmw;
	pmtw->emw.position		= position;
	pmtw->emw.move_type		= move_type;
	pmtw->emw.vec_time		= speed;
	pmtw->emw.vec_time_tmp	= speed;
	pmtw->emw.wait			= 0;
	pmtw->emw.wait_tmp		= wait;
	pmtw->emw.count			= count;
	pmtw->emw.end_value.x	= scale->x;
	pmtw->emw.end_value.y	= scale->y;
	pmtw->emw.end_value.z	= scale->z;

	MCSS_GetScale( pmw->mcss[ position ], &pmtw->emw.start_value );

	switch( move_type ){
	case EFFTOOL_CALCTYPE_DIRECT:			//���ڊg�k
		break;
	case EFFTOOL_CALCTYPE_INTERPOLATION:	//�ŏI�I�ȃX�P�[���l�܂ł��Ԃ��Ȃ���g�k
		BTL_EFFTOOL_CalcMoveVector( &pmtw->emw.start_value, scale, &pmtw->emw.vector, FX32_CONST( speed ) );
		break;
	case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG:	//�w�肵����Ԃ������g�k
		pmtw->emw.vec_time_tmp	*= 2;
	case EFFTOOL_CALCTYPE_ROUNDTRIP:		//�w�肵����Ԃ������g�k
		pmtw->emw.vector.x = FX_Div( scale->x, FX32_CONST( speed ) );
		pmtw->emw.vector.y = FX_Div( scale->y, FX32_CONST( speed ) );
		pmtw->emw.vector.z = FX_Div( scale->z, FX32_CONST( speed ) );
		break;
	}

	GFL_TCB_AddTask( pmw->tcb_sys, TCB_POKE_MCSS_Scale, pmtw, 0 );
}

//============================================================================================
/**
 *	POKEMON_PARAM����MCSS_ADD_WORK�𐶐�����
 *
 * @param[in]	pp			POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[out]	maw			MCSS_ADD_WORK���[�N�ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
static	void	POKE_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position )
{
	int	mons_no = PP_Get( pp, ID_PARA_monsno,	NULL ) - 1;
	int	form_no = PP_Get( pp, ID_PARA_form_no, NULL );
	int	sex		= PP_Get( pp, ID_PARA_sex,		NULL );
	int	rare	= PP_CheckRare( pp );

	int	file_start = POKEGRA_FILE_MAX * mons_no;						//�|�P�����i���o�[����t�@�C���̃I�t�Z�b�g���v�Z
	int	file_offset = ( ( position & 1 ) ) ? 0 : POKEGRA_BACK_M_NCGR;	//�����̌v�Z

	//�{���͕ʃt�H��������������
#warning Another Form Nothing

	//���ʂ̃`�F�b�N
	switch( sex ){
	case PTL_SEX_MALE:
		break;
	case PTL_SEX_FEMALE:
		//�I�X���X�����������Ă��邩�`�F�b�N����i�T�C�Y���O�Ȃ珑�������Ȃ��j
		sex = ( GFL_ARC_GetDataSize( ARCID_POKEGRA, file_start + file_offset + 1 ) == 0 ) ? PTL_SEX_MALE : PTL_SEX_FEMALE;
		break;
	case PTL_SEX_UNKNOWN:
		//���ʂȂ��́A�I�X�����ɂ���
		sex = PTL_SEX_MALE;
		break;
	default:
		//���肦�Ȃ�����
		GF_ASSERT(0);
		break;
	}

	maw->arcID = ARCID_POKEGRA;
	maw->ncbr = file_start + file_offset + POKEGRA_M_NCBR + sex;
	maw->nclr = file_start + POKEGRA_NORMAL_NCLR + rare;
	maw->ncer = file_start + file_offset + POKEGRA_NCER;
	maw->nanr = file_start + file_offset + POKEGRA_NANR;
	maw->nmcr = file_start + file_offset + POKEGRA_NMCR;
	maw->nmar = file_start + file_offset + POKEGRA_NMAR;
	maw->ncec = file_start + file_offset + POKEGRA_NCEC;
}

//============================================================================================
/**
 *	�|�P�����f�t�H���g�X�P�[���Z�b�g
 */
//============================================================================================
static	void	POKE_MCSS_SetDefaultScale( POKE_MCSS_WORK *pmw, int position )
{
	VecFx32			scale;

	GF_ASSERT( position < POKE_MCSS_POS_MAX );
	GF_ASSERT( pmw->mcss[ position ] );

	VEC_Set( &scale, 
			 poke_scale_table[ pmw->poke_mcss_ortho_mode ][ position ], 
			 poke_scale_table[ pmw->poke_mcss_ortho_mode ][ position ],
			 FX32_ONE );

	MCSS_SetScale( pmw->mcss[ position ], &scale );

	VEC_Set( &scale, 
			 poke_scale_table[ 0 ][ position ], 
			 poke_scale_table[ 0 ][ position ],
			 FX32_ONE );

	MCSS_SetShadowScale( pmw->mcss[ position ], &scale );
}

//============================================================================================
/**
 *	�|�P�����ړ��^�X�N
 */
//============================================================================================
static	void	TCB_POKE_MCSS_Move( GFL_TCB *tcb, void *work )
{
	POKE_MCSS_TCB_WORK	*pmtw = ( POKE_MCSS_TCB_WORK * )work;
	POKE_MCSS_WORK *pmw = pmtw->pmw;
	VecFx32	now_pos;
	BOOL	ret;

	MCSS_GetPosition( pmw->mcss[ pmtw->emw.position ], &now_pos );
	ret = BTL_EFFTOOL_CalcParam( &pmtw->emw, &now_pos );
	MCSS_SetPosition( pmw->mcss[ pmtw->emw.position ], &now_pos );
	if( ret == TRUE ){
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//============================================================================================
/**
 *	�|�P�����g�k�^�X�N
 */
//============================================================================================
static	void	TCB_POKE_MCSS_Scale( GFL_TCB *tcb, void *work )
{
	POKE_MCSS_TCB_WORK	*pmtw = ( POKE_MCSS_TCB_WORK * )work;
	POKE_MCSS_WORK *pmw = pmtw->pmw;
	VecFx32	now_scale;
	BOOL	ret;

	MCSS_GetScale( pmw->mcss[ pmtw->emw.position ], &now_scale );
	ret = BTL_EFFTOOL_CalcParam( &pmtw->emw, &now_scale );
	MCSS_SetScale( pmw->mcss[ pmtw->emw.position ], &now_scale );
	if( ret == TRUE ){
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

