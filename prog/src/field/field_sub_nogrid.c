//============================================================================================
/**
 * @file	field_sub_nogrid.c
 * @brief	�t�B�[���h���C�������T�u�i�O���b�h�����p�j
 *
 * ���̃\�[�X��field_main.c�ɃC���N���[�h����Ă��܂��B
 * �ŏI�I�ɂ͂����ƕ����Ǘ�����܂����A�������̂��߂�
 * ���΂炭�͂��̌`���ł��B
 */
//============================================================================================

#include "field_easytp.h"
//------------------------------------------------------------------
/**
 * @brief	�����������i�O���b�h�����j
 */
//------------------------------------------------------------------
static void NoGridCreate( FIELD_MAIN_WORK * fieldWork, VecFx32 * pos, u16 dir)
{
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
static void NoGridMain( FIELD_MAIN_WORK* fieldWork, VecFx32 * pos )
{
	MainPlayerAct_NoGrid( fieldWork->pcActCont, fieldWork->key_cont );
	
	GetPlayerActTrans( fieldWork->pcActCont, pos );
	SetGridPlayerActTrans( fieldWork->pcActCont, pos);
}

//------------------------------------------------------------------
/**
 * @brief	�I�������i�O���b�h�����j
 */
//------------------------------------------------------------------
static void NoGridDelete( FIELD_MAIN_WORK* fieldWork )
{
	DeletePlayerActGrid(fieldWork->pcActCont);
	//DeletePlayerAct( fieldWork->pcActCont );
}

