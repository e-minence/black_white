
//============================================================================================
/**
 * @file	btlv_mcss.c
 * @brief	�퓬MCSS�Ǘ�
 * @author	soga
 * @date	2008.11.14
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"
#include "system/mcss_tool.h"

#include "btlv_effect.h"

#include "battgra/battgra_wb.naix"

//============================================================================================
/**
 *	�萔�錾
 */
//============================================================================================

#define	BTLV_MCSS_MAX	( 6 )	//BTLV_MCSS�ŊǗ�����MCSS��MAX�l

#define	BTLV_MCSS_DEFAULT_SHIFT	( FX32_SHIFT - 4 )					//�|���S���P�ӂ̊�̒����ɂ���V�t�g�l
#define	BTLV_MCSS_DEFAULT_LINE	( 1 << BTLV_MCSS_DEFAULT_SHIFT )	//�|���S���P�ӂ̊�̒���

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTLV_MCSS_WORK
{
	GFL_TCBSYS				*tcb_sys;
	MCSS_SYS_WORK			*mcss_sys;
	MCSS_WORK				*mcss[ BTLV_MCSS_POS_MAX ];

	u8						poke_mcss_ortho_mode			:1;
	u8														:7;
	u8						poke_mcss_tcb_move_execute;
	u8						poke_mcss_tcb_scale_execute;
	u8						poke_mcss_tcb_rotate_execute;

	u8						poke_mcss_tcb_blink_execute;
	u8						dummy;								//padding
	HEAPID					heapID;
};

typedef struct
{
	BTLV_MCSS_WORK		*bmw;
	int					position;
	EFFTOOL_MOVE_WORK	emw;
}BTLV_MCSS_TCB_WORK;

typedef struct	
{
	NNSG2dCharacterData*	pCharData;			//�e�N�X�`���L����
	NNSG2dPaletteData*		pPlttData;			//�e�N�X�`���p���b�g
	void					*pBufChar;			//�e�N�X�`���L�����o�b�t�@
	void					*pBufPltt;			//�e�N�X�`���p���b�g�o�b�t�@
	int						chr_ofs;
	int						pal_ofs;
	BTLV_MCSS_WORK			*bmw;
}TCB_LOADRESOURCE_WORK;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTLV_MCSS_WORK	*BTLV_MCSS_Init( GFL_TCBSYS *tcb_sys, HEAPID heapID );
void			BTLV_MCSS_Exit( BTLV_MCSS_WORK *bmw );
void			BTLV_MCSS_Main( BTLV_MCSS_WORK *bmw );
void			BTLV_MCSS_Draw( BTLV_MCSS_WORK *bmw );
void			BTLV_MCSS_Add( BTLV_MCSS_WORK *bmw, const POKEMON_PARAM *pp, int position );
void			BTLV_MCSS_Del( BTLV_MCSS_WORK *bmw, int position );
void			BTLV_MCSS_SetOrthoMode( BTLV_MCSS_WORK *bmw );
void			BTLV_MCSS_ResetOrthoMode( BTLV_MCSS_WORK *bmw );
void			BTLV_MCSS_SetMepachiFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
void			BTLV_MCSS_SetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
int				BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position );
void			BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, int flag );
void			BTLV_MCSS_GetPokeDefaultPos( VecFx32 *pos, int position );
fx32			BTLV_MCSS_GetPokeDefaultScale( BTLV_MCSS_WORK *bmw, int position );
void			BTLV_MCSS_GetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );
void			BTLV_MCSS_SetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale );

void			BTLV_MCSS_MovePosition( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *pos, int frame, int wait, int count );
void			BTLV_MCSS_MoveScale( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *scale, int frame, int wait, int count );
void			BTLV_MCSS_MoveRotate( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *rotate, int frame, int wait, int count );
void			BTLV_MCSS_MoveBlink( BTLV_MCSS_WORK *bmw, int position, int type, int wait, int count );
BOOL			BTLV_MCSS_CheckTCBExecute( BTLV_MCSS_WORK *bmw, int position );
BOOL			BTLV_MCSS_CheckExistPokemon( BTLV_MCSS_WORK *bmw, int position );

static	void	BTLV_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position );
static	void	BTLV_MCSS_SetDefaultScale( BTLV_MCSS_WORK *bmw, int position );

static	void	BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func );
static	void	TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work );
static	void	TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work );
static	void	TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work );
static	void	TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work );

#ifdef PM_DEBUG
void	BTLV_MCSS_AddDebug( BTLV_MCSS_WORK *bmw, const MCSS_ADD_DEBUG_WORK *madw, int position );
#endif

//============================================================================================
/**
 *	�|�P�����̗����ʒu�e�[�u��
 */
//============================================================================================
/*
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
*/


static	const	VecFx32	poke_pos_table[]={
	{ FX_F32_TO_FX32( -2.5f + 3.000f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(   7.5f - 0.5f ) },		//POS_AA
	{ FX_F32_TO_FX32(  4.5f - 4.200f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -10.0f ) },		//POS_BB
	{ FX_F32_TO_FX32( -3.5f + 3.500f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(   8.5f ) },		//POS_A
	{ FX_F32_TO_FX32(  6.0f - 4.200f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(  -9.0f ) },		//POS_B
	{ FX_F32_TO_FX32( -0.5f + 3.845f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(   9.0f ) },		//POS_C
	{ FX_F32_TO_FX32(  2.0f - 4.964f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -11.0f ) },		//POS_D
	{ FX_F32_TO_FX32( -2.5f + 3.845f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32(  10.0f ) },		//POS_E
	{ FX_F32_TO_FX32(  4.5f - 4.964f ),	FX_F32_TO_FX32( 0.7f ), FX_F32_TO_FX32( -10.0f ) },		//POS_F
};

//============================================================================================
/**
 *	�|�P�����̗����ʒu�ɂ��X�P�[���␳�e�[�u��
 */
//============================================================================================
static	const	fx32	poke_scale_table[ 2 ][ BTLV_MCSS_POS_MAX ]={
	{
		0x0780 * 2,	//POS_AA
		0x1280,		//POS_BB
		0x0873 * 2,	//POS_A
		0x1322,		//POS_B
		0x0831 * 2,	//POS_C
		0x141e,		//POS_D
		0x1000 * 2,	//POS_E
		0x1000,		//POS_F
	},
	{
		FX32_ONE * 16 * 2,
		FX32_ONE * 16,
		FX32_ONE * 16 * 2,
		FX32_ONE * 16,
		FX32_ONE * 16 * 2,
		FX32_ONE * 16,
		FX32_ONE * 16 * 2,
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
BTLV_MCSS_WORK	*BTLV_MCSS_Init( GFL_TCBSYS	*tcb_sys, HEAPID heapID )
{
	BTLV_MCSS_WORK *bmw = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTLV_MCSS_WORK ) );

	bmw->mcss_sys = MCSS_Init( BTLV_MCSS_MAX, heapID );
	bmw->tcb_sys  = tcb_sys;

	return bmw;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bmw	BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_Exit( BTLV_MCSS_WORK *bmw )
{
	MCSS_Exit( bmw->mcss_sys );
	GFL_HEAP_FreeMemory( bmw );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bmw	BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_Main( BTLV_MCSS_WORK *bmw )
{
	MCSS_Main( bmw->mcss_sys );
}

//============================================================================================
/**
 *	�`��
 *
 * @param[in]	bmw	BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_Draw( BTLV_MCSS_WORK *bmw )
{
	MCSS_Draw( bmw->mcss_sys );
}

//============================================================================================
/**
 *	BTLV_MCSS�ǉ�
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pp			POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
void	BTLV_MCSS_Add( BTLV_MCSS_WORK *bmw, const POKEMON_PARAM *pp, int position )
{
	MCSS_ADD_WORK	maw;

	GF_ASSERT( position < BTLV_MCSS_POS_MAX );
	GF_ASSERT( bmw->mcss[ position ] == NULL );

	BTLV_MCSS_MakeMAW( pp, &maw, position );
	bmw->mcss[ position ] = MCSS_Add( bmw->mcss_sys,
									  poke_pos_table[ position ].x,
									  poke_pos_table[ position ].y,
									  poke_pos_table[ position ].z,
									  &maw );

	BTLV_MCSS_SetDefaultScale( bmw, position );
}

//============================================================================================
/**
 *	BTLV_MCSS�폜
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
void	BTLV_MCSS_Del( BTLV_MCSS_WORK *bmw, int position )
{
	MCSS_Del( bmw->mcss_sys, bmw->mcss[ position ] );
	bmw->mcss[ position ] = NULL;
}

//============================================================================================
/**
 *	���ˉe�`�惂�[�hON
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_SetOrthoMode( BTLV_MCSS_WORK *bmw )
{
	int	position;

	MCSS_SetOrthoMode( bmw->mcss_sys );

	bmw->poke_mcss_ortho_mode = 1;

	for( position = 0 ; position < BTLV_MCSS_POS_MAX ; position++ ){
		if( bmw->mcss[ position ] ){
			BTLV_MCSS_SetDefaultScale( bmw, position );
		}
	}
}

//============================================================================================
/**
 *	���ˉe�`�惂�[�hOFF
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_ResetOrthoMode( BTLV_MCSS_WORK *bmw )
{
	int	position;

	MCSS_ResetOrthoMode( bmw->mcss_sys );

	bmw->poke_mcss_ortho_mode = 0;

	for( position = 0 ; position < BTLV_MCSS_POS_MAX ; position++ ){
		if( bmw->mcss[ position ] ){
			BTLV_MCSS_SetDefaultScale( bmw, position );
		}
	}
}

//============================================================================================
/**
 *	���p�`����
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	���p�`���������|�P�����̗����ʒu
 * @param[in]	flag		���p�`�t���O�iBTLV_MCSS_MEPACHI_ON�ABTLV_MCSS_MEPACHI_OFF�j
 */
//============================================================================================
void	BTLV_MCSS_SetMepachiFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
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
 *	�A�j���X�g�b�v����
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�A�j���X�g�b�v���������|�P�����̗����ʒu
 * @param[in]	flag		�A�j���X�g�b�v�t���O�iBTLV_MCSS_ANM_STOP_ON�ABTLV_MCSS_ANM_STOP_OFF�j
 */
//============================================================================================
void	BTLV_MCSS_SetAnmStopFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
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
 *	�o�j�b�V���t���O�擾
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�t���O���삳�������|�P�����̗����ʒu
 */
//============================================================================================
int		BTLV_MCSS_GetVanishFlag( BTLV_MCSS_WORK *bmw, int position )
{
	GF_ASSERT( bmw->mcss[ position ] != NULL );
	return MCSS_GetVanishFlag( bmw->mcss[ position ] );
}

//============================================================================================
/**
 *	�o�j�b�V������
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�t���O���삳�������|�P�����̗����ʒu
 * @param[in]	flag		�o�j�b�V���t���O�iBTLV_MCSS_VANISH_ON�ABTLV_MCSS_VANISH_OFF�j
 */
//============================================================================================
void	BTLV_MCSS_SetVanishFlag( BTLV_MCSS_WORK *bmw, int position, int flag )
{
	GF_ASSERT( bmw->mcss[ position ] != NULL );
	if( flag == BTLV_MCSS_VANISH_ON ){
		MCSS_SetVanishFlag( bmw->mcss[ position ] );
	}
	else{
		MCSS_ResetVanishFlag( bmw->mcss[ position ] );
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
void	BTLV_MCSS_GetPokeDefaultPos( VecFx32 *pos, int position )
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
fx32	BTLV_MCSS_GetPokeDefaultScale( BTLV_MCSS_WORK *bmw, int position )
{
	return poke_scale_table[ bmw->poke_mcss_ortho_mode ][ position ];
}

//============================================================================================
/**
 *	�|�P�����̃X�P�[���l���擾
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�擾����|�P�����̗����ʒu
 * @param[out]	scale		�擾�����X�P�[���l���i�[���郏�[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTLV_MCSS_GetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale )
{
	MCSS_GetScale( bmw->mcss[ position ], scale );
}

//============================================================================================
/**
 *	�|�P�����̃X�P�[���l���i�[
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�i�[����|�P�����̗����ʒu
 * @param[in]	scale		�i�[����X�P�[���l
 */
//============================================================================================
void	BTLV_MCSS_SetScale( BTLV_MCSS_WORK *bmw, int position, VecFx32 *scale )
{
	MCSS_SetScale( bmw->mcss[ position ], scale );
}

//============================================================================================
/**
 *	�|�P�����ړ�
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�ړ�����|�P�����̗����ʒu
 * @param[in]	type		�ړ��^�C�v
 * @param[in]	pos			�ړ��^�C�v�ɂ��Ӗ����ω�
 *							EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION	�ړ���
 *							EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in]	frame		�ړ��t���[�����i�ړI�n�܂ŉ��t���[���œ��B���邩�j
 * @param[in]	wait		�ړ��E�G�C�g
 * @param[in]	count		�����J�E���g�iPOKEMCSS_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void	BTLV_MCSS_MovePosition( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *pos, int frame, int wait, int count )
{
	VecFx32	start;

	MCSS_GetPosition( bmw->mcss[ position ], &start );
	BTLV_MCSS_TCBInitialize( bmw, position, type, &start, pos, frame, wait, count, TCB_BTLV_MCSS_Move );
	bmw->poke_mcss_tcb_move_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 *	�|�P�����g�k
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�g�k����|�P�����̗����ʒu
 * @param[in]	type		�g�k�^�C�v
 * @param[in]	scale		�g�k�^�C�v�ɂ��Ӗ����ω�
 *							EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION	�ŏI�I�ȃX�P�[���l
 *							EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in]	frame		�g�k�t���[�����i�ݒ肵���g�k�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in]	wait		�g�k�E�G�C�g
 * @param[in]	count		�����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void	BTLV_MCSS_MoveScale( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *scale, int frame, int wait, int count )
{
	VecFx32	start;

	MCSS_GetScale( bmw->mcss[ position ], &start );
	BTLV_MCSS_TCBInitialize( bmw, position, type, &start, scale, frame, wait, count, TCB_BTLV_MCSS_Scale );
	bmw->poke_mcss_tcb_scale_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 *	�|�P������]
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	��]����|�P�����̗����ʒu
 * @param[in]	type		��]�^�C�v
 * @param[in]	rotate		��]�^�C�v�ɂ��Ӗ����ω�
 *							EFFTOOL_CALCTYPE_DIRECT EFFTOOL_CALCTYPE_INTERPOLATION	�ŏI�I�ȉ�]�l
 *							EFFTOOL_CALCTYPE_ROUNDTRIP�@�����̒���
 * @param[in]	frame		��]�t���[�����i�ݒ肵����]�l�܂ŉ��t���[���œ��B���邩�j
 * @param[in]	wait		��]�E�G�C�g
 * @param[in]	count		�����J�E���g�iEFFTOOL_CALCTYPE_ROUNDTRIP�ł����Ӗ��̂Ȃ��p�����[�^�j
 */
//============================================================================================
void	BTLV_MCSS_MoveRotate( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *rotate, int frame, int wait, int count )
{
	VecFx32	start;

	MCSS_GetRotate( bmw->mcss[ position ], &start );
	BTLV_MCSS_TCBInitialize( bmw, position, type, &start, rotate, frame, wait, count, TCB_BTLV_MCSS_Rotate );
	bmw->poke_mcss_tcb_rotate_execute |= BTLV_EFFTOOL_Pos2Bit( position );
}

//============================================================================================
/**
 *	�|�P�����܂΂���
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�܂΂�����|�P�����̗����ʒu
 * @param[in]	type		�܂΂����C�v
 * @param[in]	wait		�܂΂����E�G�C�g
 * @param[in]	count		�܂΂����J�E���g
 */
//============================================================================================
void	BTLV_MCSS_MoveBlink( BTLV_MCSS_WORK *bmw, int position, int type, int wait, int count )
{
	BTLV_MCSS_TCB_WORK	*pmtw = GFL_HEAP_AllocMemory( bmw->heapID, sizeof( BTLV_MCSS_TCB_WORK ) );

	pmtw->bmw				= bmw;
	pmtw->position			= position;
	pmtw->emw.move_type		= type;
	pmtw->emw.wait			= 0;
	pmtw->emw.wait_tmp		= wait;
	pmtw->emw.count			= count * 2;	//���ĊJ����1��ƃJ�E���g���邽�߂ɔ{���Ă���

	switch( type ){
	case BTLEFF_MEPACHI_ON:
		BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_ON );
		break;
	case BTLEFF_MEPACHI_OFF:
		BTLV_MCSS_SetMepachiFlag( bmw, position, BTLV_MCSS_MEPACHI_OFF );
		break;
	case BTLEFF_MEPACHI_MABATAKI:
		GFL_TCB_AddTask( bmw->tcb_sys, TCB_BTLV_MCSS_Blink, pmtw, 0 );
		bmw->poke_mcss_tcb_blink_execute |= BTLV_EFFTOOL_Pos2Bit( position );
		break;
	}
}

//============================================================================================
/**
 *	�^�X�N���N�������`�F�b�N
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 *
 * @retval: TRUE:�N�����@FALSE:�I��
 */
//============================================================================================
BOOL	BTLV_MCSS_CheckTCBExecute( BTLV_MCSS_WORK *bmw, int position )
{
	return ( ( bmw->poke_mcss_tcb_move_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
			 ( bmw->poke_mcss_tcb_scale_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
			 ( bmw->poke_mcss_tcb_rotate_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) ||
			 ( bmw->poke_mcss_tcb_blink_execute & BTLV_EFFTOOL_Pos2Bit( position ) ) );
}

//============================================================================================
/**
 *	�w�肳�ꂽ�����ʒu�̃|�P���������݂��邩�`�F�b�N
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 *
 * @retval	TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL	BTLV_MCSS_CheckExistPokemon( BTLV_MCSS_WORK *bmw, int position )
{
	return (bmw->mcss[ position ] != NULL );
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
static	void	BTLV_MCSS_MakeMAW( const POKEMON_PARAM *pp, MCSS_ADD_WORK *maw, int position )
{
	int	dir = ( ( position & 1 ) ) ? MCSS_DIR_FRONT : MCSS_DIR_BACK;

	MCSS_TOOL_MakeMAWPP( pp, maw, dir );
}

//============================================================================================
/**
 *	�|�P�����f�t�H���g�X�P�[���Z�b�g
 */
//============================================================================================
static	void	BTLV_MCSS_SetDefaultScale( BTLV_MCSS_WORK *bmw, int position )
{
	VecFx32			scale;

	GF_ASSERT( position < BTLV_MCSS_POS_MAX );
	GF_ASSERT( bmw->mcss[ position ] );

	VEC_Set( &scale, 
			 poke_scale_table[ bmw->poke_mcss_ortho_mode ][ position ], 
			 poke_scale_table[ bmw->poke_mcss_ortho_mode ][ position ],
			 FX32_ONE );

	MCSS_SetScale( bmw->mcss[ position ], &scale );

	VEC_Set( &scale, 
			 poke_scale_table[ 0 ][ position ], 
			 poke_scale_table[ 0 ][ position ] / 2,
			 FX32_ONE );

	MCSS_SetShadowScale( bmw->mcss[ position ], &scale );
}

//============================================================================================
/**
 *	�|�P��������n�^�X�N����������
 */
//============================================================================================
static	void	BTLV_MCSS_TCBInitialize( BTLV_MCSS_WORK *bmw, int position, int type, VecFx32 *start, VecFx32 *end, int frame, int wait, int count, GFL_TCB_FUNC *func )
{
	BTLV_MCSS_TCB_WORK	*pmtw = GFL_HEAP_AllocMemory( bmw->heapID, sizeof( BTLV_MCSS_TCB_WORK ) );

	pmtw->bmw				= bmw;
	pmtw->position			= position;
	pmtw->emw.move_type		= type;
	pmtw->emw.vec_time		= frame;
	pmtw->emw.vec_time_tmp	= frame;
	pmtw->emw.wait			= 0;
	pmtw->emw.wait_tmp		= wait;
	pmtw->emw.count			= count;
	pmtw->emw.start_value.x	= start->x;
	pmtw->emw.start_value.y	= start->y;
	pmtw->emw.start_value.z	= start->z;
	pmtw->emw.end_value.x	= end->x;
	pmtw->emw.end_value.y	= end->y;
	pmtw->emw.end_value.z	= end->z;

	switch( type ){
	case EFFTOOL_CALCTYPE_DIRECT:			//���ڃ|�W�V�����Ɉړ�
		break;
	case EFFTOOL_CALCTYPE_INTERPOLATION:	//�ړ���܂ł��Ԃ��Ȃ���ړ�
		BTLV_EFFTOOL_CalcMoveVector( &pmtw->emw.start_value, end, &pmtw->emw.vector, FX32_CONST( frame ) );
		break;
	case EFFTOOL_CALCTYPE_ROUNDTRIP_LONG:	//�w�肵����Ԃ������ړ�
		pmtw->emw.vec_time_tmp	*= 2;
	case EFFTOOL_CALCTYPE_ROUNDTRIP:		//�w�肵����Ԃ������ړ�
		pmtw->emw.vector.x = FX_Div( end->x, FX32_CONST( frame ) );
		pmtw->emw.vector.y = FX_Div( end->y, FX32_CONST( frame ) );
		pmtw->emw.vector.z = FX_Div( end->z, FX32_CONST( frame ) );
		break;
	}
	GFL_TCB_AddTask( bmw->tcb_sys, func, pmtw, 0 );
}

//============================================================================================
/**
 *	�|�P�����ړ��^�X�N
 */
//============================================================================================
static	void	TCB_BTLV_MCSS_Move( GFL_TCB *tcb, void *work )
{
	BTLV_MCSS_TCB_WORK	*pmtw = ( BTLV_MCSS_TCB_WORK * )work;
	BTLV_MCSS_WORK *bmw = pmtw->bmw;
	VecFx32	now_pos;
	BOOL	ret;

	MCSS_GetPosition( bmw->mcss[ pmtw->position ], &now_pos );
	ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_pos );
	MCSS_SetPosition( bmw->mcss[ pmtw->position ], &now_pos );
	if( ret == TRUE ){
		bmw->poke_mcss_tcb_move_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ 0xff );
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//============================================================================================
/**
 *	�|�P�����g�k�^�X�N
 */
//============================================================================================
static	void	TCB_BTLV_MCSS_Scale( GFL_TCB *tcb, void *work )
{
	BTLV_MCSS_TCB_WORK	*pmtw = ( BTLV_MCSS_TCB_WORK * )work;
	BTLV_MCSS_WORK *bmw = pmtw->bmw;
	VecFx32	now_scale;
	BOOL	ret;

	MCSS_GetScale( bmw->mcss[ pmtw->position ], &now_scale );
	ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_scale );
	MCSS_SetScale( bmw->mcss[ pmtw->position ], &now_scale );
	if( ret == TRUE ){
		bmw->poke_mcss_tcb_scale_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ 0xff );
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//============================================================================================
/**
 *	�|�P������]�^�X�N
 */
//============================================================================================
static	void	TCB_BTLV_MCSS_Rotate( GFL_TCB *tcb, void *work )
{
	BTLV_MCSS_TCB_WORK	*pmtw = ( BTLV_MCSS_TCB_WORK * )work;
	BTLV_MCSS_WORK *bmw = pmtw->bmw;
	VecFx32	now_rotate;
	BOOL	ret;

	MCSS_GetRotate( bmw->mcss[ pmtw->position ], &now_rotate );
	ret = BTLV_EFFTOOL_CalcParam( &pmtw->emw, &now_rotate );
	MCSS_SetRotate( bmw->mcss[ pmtw->position ], &now_rotate );
	if( ret == TRUE ){
		bmw->poke_mcss_tcb_rotate_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ 0xff );
		GFL_HEAP_FreeMemory( work );
		GFL_TCB_DeleteTask( tcb );
	}
}

//============================================================================================
/**
 *	�|�P�����܂΂����^�X�N
 */
//============================================================================================
static	void	TCB_BTLV_MCSS_Blink( GFL_TCB *tcb, void *work )
{
	BTLV_MCSS_TCB_WORK	*pmtw = ( BTLV_MCSS_TCB_WORK * )work;
	BTLV_MCSS_WORK *bmw = pmtw->bmw;

	if( pmtw->emw.wait == 0 ){
		pmtw->emw.wait = pmtw->emw.wait_tmp;
		BTLV_MCSS_SetMepachiFlag( pmtw->bmw, pmtw->position, BTLV_MCSS_MEPACHI_FLIP );
		if( --pmtw->emw.count == 0 ){
			bmw->poke_mcss_tcb_blink_execute &= ( BTLV_EFFTOOL_Pos2Bit( pmtw->position ) ^ 0xff );
			GFL_HEAP_FreeMemory( work );
			GFL_TCB_DeleteTask( tcb );
		}
	}
	else{
		pmtw->emw.wait--;
	}
}

#ifdef PM_DEBUG
//============================================================================================
/**
 *	BTLV_MCSS�ǉ��i�f�o�b�O�p�j
 *
 * @param[in]	bmw			BTLV_MCSS�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	madw		MCSS_ADD_DEBUG_WORK�\���̂ւ̃|�C���^
 * @param[in]	position	�|�P�����̗����ʒu
 */
//============================================================================================
void	BTLV_MCSS_AddDebug( BTLV_MCSS_WORK *bmw, const MCSS_ADD_DEBUG_WORK *madw, int position )
{
	GF_ASSERT( position < BTLV_MCSS_POS_MAX );
	if( bmw->mcss[ position ] ){
		BTLV_MCSS_Del( bmw, position );
	}

	bmw->mcss[ position ] = MCSS_AddDebug( bmw->mcss_sys,
									  poke_pos_table[ position ].x,
									  poke_pos_table[ position ].y,
									  poke_pos_table[ position ].z,
									  madw );

	BTLV_MCSS_SetDefaultScale( bmw, position );
}
#endif
