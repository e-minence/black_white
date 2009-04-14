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
	fieldWork->fldActCont = FLD_CreateFieldActSys( fieldWork, fieldWork->heapID );
	fieldWork->pcActCont = CreatePlayerAct( fieldWork, fieldWork->heapID );
	SetPlayerActTrans( fieldWork->pcActCont, pos );
	SetPlayerActDirection( fieldWork->pcActCont, &dir );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void NoGridMain( FIELD_MAIN_WORK* fieldWork, VecFx32 * pos )
{
	MainPlayerAct_NoGrid( fieldWork->pcActCont, fieldWork->key_cont );
	FLD_MainFieldActSys( fieldWork->fldActCont );
	
	GetPlayerActTrans( fieldWork->pcActCont, pos );

	if (FieldEasyTP_TouchDirGet() == FLDEASYTP_TCHDIR_DOWN) {
		VecFx32 trans;
		FLDMAPPER_GRIDINFO gridInfo;
		int i;
		GetPlayerActTrans(fieldWork->pcActCont, &trans);
		FLDMAPPER_GetGridInfo( GetFieldG3Dmapper(fieldWork), &trans, &gridInfo);
		OS_Printf("gridInfo.count = %d\n", gridInfo.count);
		for (i = 0; i < gridInfo.count; i++) {
			OS_Printf("[%02d]%08x\n",i, gridInfo.gridData[i].height);
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�I�������i�O���b�h�����j
 */
//------------------------------------------------------------------
static void NoGridDelete( FIELD_MAIN_WORK* fieldWork )
{
	DeletePlayerAct( fieldWork->pcActCont );
	FLD_DeleteFieldActSys( fieldWork->fldActCont );
}

