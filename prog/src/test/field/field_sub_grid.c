//============================================================================================
/**
 * @file	field_sub_grid.c
 * @brief	�t�B�[���h���C�������T�u�i�O���b�h�ړ��j
 *
 * ���̃\�[�X��field_main.c�ɃC���N���[�h����Ă��܂��B
 * �ŏI�I�ɂ͂����ƕ����Ǘ�����܂����A�������̂��߂�
 * ���΂炭�͂��̌`���ł��B
 */
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�����������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void GridMoveCreate( FIELD_WORK * fieldWork, VecFx32 * pos, u16 dir)
{
	fieldWork->camera_control = FLD_CreateCamera( fieldWork->gs, fieldWork->heapID );
	fieldWork->fldActCont = FLD_CreateFieldActSys( fieldWork->gs, fieldWork->heapID );
	fieldWork->pcActCont = CreatePlayerAct( fieldWork->gs, fieldWork->heapID );
	SetPlayerActTrans( fieldWork->pcActCont, pos );
	SetPlayerActDirection( fieldWork->pcActCont, &dir );
}

//------------------------------------------------------------------
/**
 * @brief	���C�������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void GridMoveMain( FIELD_WORK* fieldWork, VecFx32 * pos )
{
	MainPlayerAct( fieldWork->pcActCont, fieldWork->key_cont );
	FLD_MainFieldActSys( fieldWork->fldActCont );
	
	GetPlayerActTrans( fieldWork->pcActCont, pos );

	FLD_SetCameraTrans( fieldWork->camera_control, pos );
	//FLD_SetCameraDirection( fieldWork->camera_control, &dir );

	FLD_MainCamera( fieldWork->camera_control, fieldWork->key_cont );
}

//------------------------------------------------------------------
/**
 * @brief	�I�������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void GridMoveDelete( FIELD_WORK* fieldWork )
{
	DeletePlayerAct( fieldWork->pcActCont );
	FLD_DeleteCamera( fieldWork->camera_control );
	FLD_DeleteFieldActSys( fieldWork->fldActCont );
}

