//============================================================================================
/**
 * @file	field_sub_c3.c
 * @brief	�t�B�[���h���C�������T�u�i�O���b�h�����AC3�����p�j
 *
 * ���̃\�[�X��fieldmap.c�ɃC���N���[�h����Ă��܂��B
 * �ŏI�I�ɂ͂����ƕ����Ǘ�����܂����A�������̂��߂�
 * ���΂炭�͂��̌`���ł��B
 */
//============================================================================================

#include "field_easytp.h"


static void CalcPos(VecFx32 * pos, const VecFx32 * center, u16 len, u16 dir);

typedef struct {
	u16 player_len;
	u16 pos_angle;
	s16 df_len;
	s16 df_angle;
}C3_MOVE_WORK;

static C3_MOVE_WORK C3MoveWork;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����������i�O���b�h�����j
 */
//------------------------------------------------------------------
static void TestC3Create( FIELD_MAIN_WORK * fieldWork, VecFx32 * pos, u16 dir)
{
	static const C3_MOVE_WORK init = {
		 0x1f0,
		 0,
		 1,
		 16
	};
	C3MoveWork = init;
#if 0
	fieldWork->pcActCont = CreatePlayerAct( fieldWork, pos, fieldWork->heapID );
	SetPlayerActTrans( fieldWork->pcActCont, pos );
	SetPlayerActDirection( fieldWork->pcActCont, &dir );
#endif
	fieldWork->pcActCont = CreatePlayerActGrid(fieldWork, pos, fieldWork->heapID);
	SetGridPlayerActTrans( fieldWork->pcActCont, pos);
	SetPlayerActDirection( fieldWork->pcActCont, &dir );

}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void TestC3Main( FIELD_MAIN_WORK* fieldWork, VecFx32 * pos )
{
	C3_MOVE_WORK * mwk = &C3MoveWork;

	{
		if (fieldWork->key_cont & PAD_KEY_LEFT) {
			mwk->pos_angle -= mwk->df_angle;
		}
		if (fieldWork->key_cont & PAD_KEY_RIGHT) {
			mwk->pos_angle += mwk->df_angle;
		}
		if (fieldWork->key_cont & PAD_KEY_UP) {
			mwk->player_len -= mwk->df_len;
		}
		if (fieldWork->key_cont & PAD_KEY_DOWN) {
			mwk->player_len += mwk->df_len;
		}
	}

	{
		VecFx32 cam, player_pos;
		FIELD_CAMERA_GetTargetPos( fieldWork->camera_control, &cam);
		CalcPos(&player_pos, &cam, mwk->player_len, mwk->pos_angle);
		//SetPlayerActTrans( fieldWork->pcActCont, &player_pos );
		SetGridPlayerActTrans( fieldWork->pcActCont, &player_pos);
	}
	FIELD_CAMERA_SetDirectionOnXZ(fieldWork->camera_control, mwk->pos_angle);
	MainPlayerAct_C3( fieldWork->pcActCont, fieldWork->key_cont, mwk->pos_angle );
}

//------------------------------------------------------------------
/**
 * @brief	�I�������i�O���b�h�����j
 */
//------------------------------------------------------------------
static void TestC3Delete( FIELD_MAIN_WORK* fieldWork )
{
	DeletePlayerActGrid(fieldWork->pcActCont);
	//DeletePlayerAct( fieldWork->pcActCont );
}

//============================================================================================
//============================================================================================
#define HEIGHT	0x2000
//------------------------------------------------------------------
//------------------------------------------------------------------
static void CalcPos(VecFx32 * pos, const VecFx32 * center, u16 len, u16 dir)
{
	pos->x = center->x + len * FX_SinIdx(dir);
	pos->y = center->y + HEIGHT;
	pos->z = center->z + len * FX_CosIdx(dir);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
