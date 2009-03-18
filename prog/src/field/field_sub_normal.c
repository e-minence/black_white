//============================================================================================
/**
 * @file	field_sub_normal.c
 * @brief	�t�B�[���h���C�������T�u�i�ʏ�j
 *
 * ���̃\�[�X��field_main.c�ɃC���N���[�h����Ă��܂��B
 * �ŏI�I�ɂ͂����ƕ����Ǘ�����܂����A�������̂��߂�
 * ���΂炭�͂��̌`���ł��B
 */
//============================================================================================

#define NORMAL_CM_LENGTH (0x98)
#define NORMAL_CM_HEIGHT (0x50000)

//------------------------------------------------------------------
/**
 * @brief	�����������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void NormalCreate( FIELD_MAIN_WORK * fieldWork, VecFx32 * pos, u16 dir)
{
	fieldWork->camera_control = FIELD_CAMERA_Create( fieldWork->gs, fieldWork->heapID );
	fieldWork->fldActCont = FLD_CreateFieldActSys( fieldWork->gs, fieldWork->heapID );
	FLDACT_TestSetup( fieldWork->fldActCont );
	fieldWork->pcActCont = CreatePlayerAct( fieldWork->gs, fieldWork->heapID );
	SetPlayerActTrans( fieldWork->pcActCont, pos );
	SetPlayerActDirection( fieldWork->pcActCont, &dir );

	FLD_SetCameraLength( fieldWork->camera_control, NORMAL_CM_LENGTH );
	FLD_SetCameraHeight( fieldWork->camera_control, NORMAL_CM_HEIGHT );
}

//------------------------------------------------------------------
/**
 * @brief	���C�������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void NormalMain( FIELD_MAIN_WORK* fieldWork, VecFx32 * pos )
{
	MainPlayerAct( fieldWork->pcActCont, fieldWork->key_cont );
	FLD_MainFieldActSys( fieldWork->fldActCont );
	
	GetPlayerActTrans( fieldWork->pcActCont, pos );

	FLD_SetCameraTrans( fieldWork->camera_control, pos );
	//FLD_SetCameraDirection( fieldWork->camera_control, &dir );

	FIELD_CAMERA_Main( fieldWork->camera_control, fieldWork->key_cont );
}

//------------------------------------------------------------------
/**
 * @brief	�I�������i�f�t�H���g�j
 */
//------------------------------------------------------------------
static void NormalDelete( FIELD_MAIN_WORK* fieldWork )
{
	DeletePlayerAct( fieldWork->pcActCont );
	FIELD_CAMERA_Delete( fieldWork->camera_control );
	FLDACT_TestRelease( fieldWork->fldActCont );
	FLD_DeleteFieldActSys( fieldWork->fldActCont );
}

