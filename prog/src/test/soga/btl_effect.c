
//============================================================================================
/**
 * @file	btl_effect.c
 * @brief	�퓬�G�t�F�N�g����
 * @author	soga
 * @date	2008.11.21
 */
//============================================================================================

#include <gflib.h>

#include "system/gfl_use.h"

#include "btl_effect.h"
#include "btl_effect_def.h"

#include "btl_efftool.h"

#include "arc_def.h"
#include "spa.naix"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

struct _BTL_EFFECT_WORK
{
	//�{���̓X�N���v�g�G���W�����ڂ��āA���삳���邪�A�b���TCB�𗘗p����
	//�ŏI�I�ɂ̓G�t�F�N�g�Ŏg�p����TCB��POKE_MCSS�ABTL_CAMERA�ABTL_EFFECT�ŃV�F�A����`�ɂ���
	GFL_TCBSYS			*tcb_sys;
	void				*tcb_work;
	POKE_MCSS_WORK		*pmw;
	BTL_STAGE_WORK		*bsw;
	BTL_FIELD_WORK		*bfw;
	BTL_CAMERA_WORK		*bcw;
	GFL_PTC_PTR			ptc;
	u8					spa_work[ PARTICLE_LIB_HEAP_SIZE ];
	int					execute_flag;
	HEAPID				heapID;
};

typedef struct
{
	BTL_EFFECT_WORK *bew;
	void			*resource;
	int				seq_no;
	int				wait;
}BTL_EFFECT_TCB;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BTL_EFFECT_WORK	*BTL_EFFECT_Init( int index, HEAPID heapID );
void			BTL_EFFECT_Exit( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_Main( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_Add( BTL_EFFECT_WORK *bew, int eff_no );
BOOL			BTL_EFFECT_CheckExecute( BTL_EFFECT_WORK *bew );
void			BTL_EFFECT_SetPokemon( BTL_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position );

BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( BTL_EFFECT_WORK *bew );

//�b��ō����TCBVer�̋Z�G�t�F�N�g�V�[�P���X
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work );
static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet );
static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit );
static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit );



static	GFL_TCB_FUNC * const btl_effect_tcb_table[]={
	&BTL_EFFECT_TCB_AA2BBGanseki,
	&BTL_EFFECT_TCB_BB2AAGanseki,
	&BTL_EFFECT_TCB_AA2BBMizudeppou,
	&BTL_EFFECT_TCB_BB2AAMizudeppou,
};

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	index	�w�i�����肷��C���f�b�N�X�i���o�[
 * @param[in]	heapID	�q�[�vID
 */
//============================================================================================
BTL_EFFECT_WORK	*BTL_EFFECT_Init( int index, HEAPID heapID )
{
	BTL_EFFECT_WORK *bew = GFL_HEAP_AllocClearMemory( heapID, sizeof( BTL_EFFECT_WORK ) );

	bew->heapID = heapID;

	//�{���̓X�N���v�g�G���W�����ڂ��āA���삳���邪�A�b���TCB�𗘗p����
	//�ŏI�I�ɂ̓G�t�F�N�g�Ŏg�p����TCB��POKE_MCSS�ABTL_CAMERA�ABTL_EFFECT�ŃV�F�A����`�ɂ���
	bew->tcb_work = GFL_HEAP_AllocClearMemory( heapID, GFL_TCB_CalcSystemWorkSize( BTL_EFFECT_TCB_MAX ) );
	bew->tcb_sys = GFL_TCB_Init( BTL_EFFECT_TCB_MAX, bew->tcb_work );

	bew->pmw = POKE_MCSS_Init( bew->tcb_sys, heapID );
	bew->bsw = BTL_STAGE_Init( index, heapID );
	bew->bfw = BTL_FIELD_Init( index, heapID );
	bew->bcw = BTL_CAMERA_Init( bew->tcb_sys, heapID );

	//�p�[�e�B�N��������
	GFL_PTC_Init( heapID );

	return bew;
}

//============================================================================================
/**
 *	�V�X�e���I��
 *
 * @param[in]	bew	BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_EFFECT_Exit( BTL_EFFECT_WORK *bew )
{
	GFL_TCB_Exit( bew->tcb_sys );
	GFL_HEAP_FreeMemory( bew->tcb_work );
	GFL_HEAP_FreeMemory( bew );
}

//============================================================================================
/**
 *	�V�X�e�����C��
 *
 * @param[in]	bew	BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 */
//============================================================================================
void	BTL_EFFECT_Main( BTL_EFFECT_WORK *bew )
{
	GFL_TCB_Main( bew->tcb_sys );

	POKE_MCSS_Main( bew->pmw );
	BTL_STAGE_Main( bew->bsw );
	BTL_FIELD_Main( bew->bfw );
	BTL_CAMERA_Main( bew->bcw );

	//3D�`��
	{
		GFL_G3D_DRAW_Start();
		GFL_G3D_DRAW_SetLookAt();
		{
			//�����̕`�揇�Ԃ́ANitroSDK�ŕ`�悳��Ă�����̂̂��Ƃ�NitroSystem�ŕ`�悳��Ă�����̂�u��
			//���Ԃ����݂�����Ɛ������\�����ꂸ�ň��t���[�Y����
			POKE_MCSS_Draw( bew->pmw );
			{
				s32	particle_count;

				particle_count = G3X_GetPolygonListRamCount();

				GFL_PTC_Main();

				particle_count = G3X_GetPolygonListRamCount() - particle_count;
			}
			BTL_STAGE_Draw( bew->bsw );
			BTL_FIELD_Draw( bew->bfw );
		}

		GFL_G3D_DRAW_End();
	}
}

//============================================================================================
/**
 *	�G�t�F�N�g�N��
 *
 * @param[in]	bew		BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	eff_no	�N������G�t�F�N�g�i���o�[
 */
//============================================================================================
void	BTL_EFFECT_Add( BTL_EFFECT_WORK *bew, int eff_no )
{
	BTL_EFFECT_TCB	*bet = GFL_HEAP_AllocMemory( bew->heapID, sizeof( BTL_EFFECT_TCB ) );

	bet->bew = bew;
	bet->seq_no = 0;
	bet->wait = 0;

	GFL_TCB_AddTask( bew->tcb_sys, btl_effect_tcb_table[ eff_no ], bet, 0 );

	bew->execute_flag = 1;
}

//============================================================================================
/**
 *	�G�t�F�N�g�N�������`�F�b�N
 *
 * @param[in]	bew		BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval FALSE:�N�����Ă��Ȃ��@TRUE:�N����
 */
//============================================================================================
BOOL	BTL_EFFECT_CheckExecute( BTL_EFFECT_WORK *bew )
{
	return ( bew->execute_flag != 0 );
}

//============================================================================================
/**
 *	�w�肳�ꂽ�����ʒu�Ƀ|�P�������Z�b�g
 *
 * @param[in]	bew			BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 * @param[in]	pp			�Z�b�g����|�P������POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param[in]	position	�Z�b�g����|�P�����̗����ʒu
 *
 * @retval FALSE:�N�����Ă��Ȃ��@TRUE:�N����
 */
//============================================================================================
void	BTL_EFFECT_SetPokemon( BTL_EFFECT_WORK *bew, POKEMON_PARAM *pp, int position )
{
	POKE_MCSS_Add( bew->pmw, pp, position );
}

//============================================================================================
/**
 *	�G�t�F�N�g�Ŏg�p����Ă���J�����Ǘ��\���̂̃|�C���^���擾
 *
 * @param[in]	bew			BTL_EFFECT�Ǘ����[�N�ւ̃|�C���^
 *
 * @retval bcw �J�����Ǘ��\����
 */
//============================================================================================
BTL_CAMERA_WORK	*BTL_EFFECT_GetCameraWork( BTL_EFFECT_WORK *bew )
{
	return bew->bcw;
}

//�{���̓X�N���v�g�G���W�����ڂ��āA���삳���邪�A�b���TCB�𗘗p����
//============================================================================================
/**
 *	�G�t�F�N�g�V�[�P���X
 */
//============================================================================================
static	void	BTL_EFFECT_TCB_AA2BBGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );

	switch( bet->seq_no ){
	//�J����BB�Ɉړ�
	case 0:
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_157_spa, bew->heapID );
		bet->seq_no++;
		break;
	//�p�[�e�B�N���Z�b�g
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;
		scale.x = POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_POS_BB );
		scale.y = POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_POS_BB );

		if( bet->wait < 60 ){
			if( bet->wait > 10 ){
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
				if( (bet->wait % 10 ) < 5 ){
					scale.y = FX_MUL( scale.y, FX_F32_TO_FX32( 0.9f ) );
				}
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		POKE_MCSS_SetScale( bew->pmw, POKE_MCSS_POS_BB, &scale );
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//�J�����f�t�H���g�ʒu�Ɉړ�
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}
static	void	BTL_EFFECT_TCB_BB2AAGanseki( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );

	switch( bet->seq_no ){
	//�J����AA�Ɉړ�
	case 0:
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_157_spa, bew->heapID );
		bet->seq_no++;
		break;
	//�p�[�e�B�N���Z�b�g
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
		GFL_PTC_CreateEmitter( bew->ptc, 2, &target );
		bet->seq_no++;
		bet->wait = 0;
		break;
	case 5:
		bet->wait++;
		scale.x = POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_POS_AA );
		scale.y = POKE_MCSS_GetPokeDefaultScale( POKE_MCSS_POS_AA );

		if( bet->wait < 60 ){
			if( bet->wait > 10 ){
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
				if( (bet->wait % 10 ) < 5 ){
					scale.y = FX_MUL( scale.y, FX_F32_TO_FX32( 0.9f ) );
				}
			}
		}
		else{
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			bet->seq_no++;
		}
		POKE_MCSS_SetScale( bew->pmw, POKE_MCSS_POS_AA, &scale );
		break;
	case 6:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//�J�����f�t�H���g�ʒu�Ɉړ�
	case 7:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_AA2BBMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//�J����AA�Ɉړ�
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//�p�[�e�B�N���Z�b�g
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCAA, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };

				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
				target.y += FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_BB, EFFTOOL_MOVETYPE_ROUNDTRIP, &rt_value, 2, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_BB, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//�J�����f�t�H���g�ʒu�Ɉړ�
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_BB2AAMizudeppou( GFL_TCB *tcb, void *work )
{
	BTL_EFFECT_TCB	*bet = (BTL_EFFECT_TCB *)work;
	BTL_EFFECT_WORK *bew = bet->bew;
	VecFx32			pos,target,scale;

	switch( bet->seq_no ){
	//�J����BB�Ɉړ�
	case 0:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_BB );
		target.x -= FX32_ONE * 2;
		target.z -= FX32_ONE * 10;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 8;
		pos.z = target.z + FX32_ONE * 30;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
		bet->seq_no++;
		break;
	case 1:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bew->ptc = GFL_PTC_Create( bew->spa_work, PARTICLE_LIB_HEAP_SIZE, FALSE, bew->heapID );
			bet->seq_no++;
		}
		break;
	case 2:
		bet->resource = GFL_PTC_LoadArcResource( ARCID_PTC, NARC_spa_w_055_spa, bew->heapID );
		bet->seq_no++;
		break;
	//�p�[�e�B�N���Z�b�g
	case 3:
		GFL_PTC_SetResource( bew->ptc, bet->resource, FALSE, GFUser_VIntr_GetTCBSYS() );
		bet->seq_no++;
		break;
	case 4:
		GFL_PTC_CreateEmitterCallback( bew->ptc, 2, &BTL_EFFECT_InitPTCBB, NULL );
		bet->seq_no++;
		break;
	case 5:
		POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
		target.x += FX32_ONE * 1;
		target.z -= FX32_ONE * 8;
		pos.x = target.x;
		pos.y = target.y + FX32_ONE * 7;
		pos.z = target.z + FX32_ONE * 16;
		BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 30, 28 );
		bet->seq_no++;
		bet->wait = 12;
		break;
	case 6:
		if( bet->wait ){
			bet->wait--;
			if( bet->wait == 0 ){
				VecFx32 rt_value = { FX32_HALF >> 1, 0, 0 };
				POKE_MCSS_GetPokeDefaultPos( &target, POKE_MCSS_POS_AA );
				target.y += FX32_ONE * 3;
				target.z -= FX32_ONE * 3;
				GFL_PTC_CreateEmitter( bew->ptc, 0, &target );
				GFL_PTC_CreateEmitter( bew->ptc, 1, &target );
				POKE_MCSS_MovePosition( bew->pmw, POKE_MCSS_POS_AA, EFFTOOL_MOVETYPE_ROUNDTRIP, &rt_value, 2, 60 );
				POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_ON );
				POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_ON );
			}
		}
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			bet->seq_no++;
		}
		break;
	case 7:
		if( GFL_PTC_GetEmitterNum( bew->ptc ) == 0 ){
			POKE_MCSS_SetMepachiFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_MEPACHI_OFF );
			POKE_MCSS_SetAnmStopFlag( bew->pmw, POKE_MCSS_POS_AA, POKE_MCSS_ANM_STOP_OFF );
			GFL_PTC_Delete( bew->ptc );
			BTL_CAMERA_GetDefaultCameraPosition( &pos, &target );
			BTL_CAMERA_MoveCameraInterpolation( bew->bcw, &pos, &target, 10, 8 );
			bet->seq_no++;
		}
		break;
	//�J�����f�t�H���g�ʒu�Ɉړ�
	case 8:
		if( BTL_CAMERA_CheckExecute( bew->bcw ) == FALSE ){
			BTL_EFFECT_TCB_End( tcb, bet );
		}
		break;
	}
}

static	void	BTL_EFFECT_TCB_End( GFL_TCB *tcb, BTL_EFFECT_TCB *bet )
{
	bet->bew->execute_flag = 0;
	GFL_HEAP_FreeMemory( bet );
	GFL_TCB_DeleteTask( tcb );
}

static	void	BTL_EFFECT_InitPTCAA( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_AA );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_BB );
	src.y += FX32_ONE * 2;
	src.z -= 0x1600;
	dst.y += FX32_ONE * 2;
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}

static	void	BTL_EFFECT_InitPTCBB( GFL_EMIT_PTR emit )
{
	VecFx32			src,dst;
	VecFx16			dir;

	POKE_MCSS_GetPokeDefaultPos( &src, POKE_MCSS_POS_BB );
	POKE_MCSS_GetPokeDefaultPos( &dst, POKE_MCSS_POS_AA );
	src.y += FX32_ONE * 4;
	dst.x -= FX32_ONE * 1;
	dst.y += FX32_HALF;
	VEC_Normalize( &dst, &dst );
	VEC_Fx16Set( &dir, dst.x, dst.y, dst.z );
	GFL_PTC_SetEmitterPosition( emit, &src );
	GFL_PTC_SetEmitterAxis( emit, &dir );
}
