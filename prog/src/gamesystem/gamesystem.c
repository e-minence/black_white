//============================================================================================
/**
 * @file	gamemsystem.c
 * @brief	�|�P�����Q�[���V�X�e���i�������ʏ�Q�[���̃g�b�v���x���j
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.20
 *
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"		//HEAPID�̎Q�Ɨp

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "gamesystem/iss_sys.h"

#include "gamesystem/game_init.h"

#include "src/field/event_mapchange.h"
#include "system/net_err.h"

#ifdef PM_DEBUG
#include "debug_data.h"
#endif




//============================================================================================
//============================================================================================
enum {
  /**
   * @brief �i�����pHEAP�̈�
   */
  HEAPSIZE_WORLD = 0x18000,

  /**
   * @brief �C�x���g�pHEAP�̈�
   *
   * �ʂ̃C�x���g���ƂɊm�ۂ��A�C�x���g�I���ŉ������郁������
   * ����HEAP����m�ۂ���
   */
	HEAPSIZE_PROC = 0x8000,

  /**
   * @brief �C�x���g�R���g���[���[�pHEAP�̈�
   * ��ɂł͂Ȃ����A�C�x���g�ƃC�x���g���܂����悤�Ȍ`��
   * ���݂��郁�����i���j�I�����[����o�g���^���[�̃R���g���[���j��
   * ����HEAP����擾����
   */
  HEAPSIZE_APP_CONTROL  =      (0x1000),

  HEAPID_GAMESYS = HEAPID_WORLD,
	//HEAPID_GAMESYS = HEAPID_PROC,
	
};

//------------------------------------------------------------------
/**
 * @brief	�Q�[������V�X�e���p���[�N�ێ��ϐ�
 *
 * �����Ԃ�f�o�b�O�p�����莞�̃O���[�o���Q�Ɨp�ɂ����g��Ȃ��͂�
 */
//------------------------------------------------------------------
static GAMESYS_WORK * GameSysWork;

//------------------------------------------------------------------
/**
 * @brief	�Q�[���������p�����[�^�ێ��ϐ�
 *
 */
//------------------------------------------------------------------
static GAME_INIT_WORK TestGameInitWork;

//============================================================================================
//============================================================================================

static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param);
static BOOL GameSystem_Main(GAMESYS_WORK * gsys);
static void GameSystem_End(GAMESYS_WORK * gsys);
static u32 GAMESYS_WORK_GetSize(void);


//============================================================================================
/**
 * @brief	�Q�[���V�X�e���p�v���Z�X
 *
 * NitroMain->GFLUser_Main->GFL_PROC_Main()�ɓo�^�����B
 * ��{�I�ɂ̓Q�[���{�ҊJ�n��͂��̃v���Z�X�����삵������͂�
 */
//============================================================================================
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk);
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk);

//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys;
	u32 work_size = GAMESYS_WORK_GetSize();
	GAME_INIT_WORK *game_init = pwk;
	
  //�Q�[�����A�i���I�ɕێ�����K�v�̂���f�[�^�͂���HEAP��
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WORLD, HEAPSIZE_WORLD );
  //�Q�[���C�x���g���ɐ�������ׂ��������i�C�x���g�I���ŉ���j�̏ꍇ�͂���HEAP��
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PROC, HEAPSIZE_PROC );
	//�p���X���Ȃ���퓬���t�B�[���h������̂ł����Ŋm��
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_APP_CONTROL, HEAPSIZE_APP_CONTROL );
	
	gsys = GFL_PROC_AllocWork(proc, work_size, HEAPID_GAMESYS);
	GFL_STD_MemClear(gsys, work_size);
	GameSysWork = gsys;
	GameSystem_Init(gsys, HEAPID_GAMESYS, game_init);
#if 1		/* �b��I�Ƀv���Z�X�o�^ */
	switch(game_init->mode){
	case GAMEINIT_MODE_CONTINUE:
		{
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, game_init);
			GAMESYSTEM_SetEvent(gsys, event);
		}
		break;
	case GAMEINIT_MODE_DEBUG:
#ifdef PM_DEBUG
    //�K���Ɏ莝���|�P������Add
    DEBUG_MyPokeAdd( GAMESYSTEM_GetGameData(gsys), GFL_HEAPID_APP );
    //�f�o�b�O�A�C�e���ǉ�
    DEBUG_MYITEM_MakeBag( GAMESYSTEM_GetGameData(gsys), GFL_HEAPID_APP );
#endif //PM_DEBUG
    /* FALL THROUGH */
	case GAMEINIT_MODE_FIRST:
    {
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, game_init);
			GAMESYSTEM_SetEvent(gsys, event);
    }
		break;
	}
#endif
	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys = mywk;

	if (GameSystem_Main(gsys)) {
		return GFL_PROC_RES_FINISH;
	} else {
		return GFL_PROC_RES_CONTINUE;
	}
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static GFL_PROC_RESULT GameMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAMESYS_WORK * gsys = mywk;
	GameSystem_End(gsys);
	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_GAMESYS );
	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��e�[�u��
 */
//------------------------------------------------------------------
const GFL_PROC_DATA GameMainProcData = {
	GameMainProcInit,
	GameMainProcMain,
	GameMainProcEnd,
};


//------------------------------------------------------------------
/**
 * @file	�Q�[���J�n�������p���[�N�擾
 */
//------------------------------------------------------------------
GAME_INIT_WORK * DEBUG_GetGameInitWork(GAMEINIT_MODE mode, u16 mapid, VecFx32 *pos, s16 dir )
{
  GFL_STD_MemClear(&TestGameInitWork, sizeof(GAME_INIT_WORK));
	TestGameInitWork.mode = mode;
	TestGameInitWork.mapid = mapid;
	TestGameInitWork.pos = *pos;
	TestGameInitWork.dir = dir;
	return &TestGameInitWork;
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[������V�X�e���p���[�N��`
 */
//------------------------------------------------------------------
struct _GAMESYS_WORK {
	HEAPID heapID;			///<�g�p����q�[�v�w��ID
	GAME_INIT_WORK * init_param;		///<�e�i�Ăяo�����j��������p�������[�N�ւ̃|�C���^

	GFL_PROCSYS * procsys;	///<�g�p���Ă���PROC�V�X�e���ւ̃|�C���^

	GFL_PROC_MAIN_STATUS proc_result;

	EVCHECK_FUNC evcheck_func;
	void * evcheck_context;
	GMEVENT * event;

	GAMEDATA * gamedata;
	FIELDMAP_WORK * fieldmap;
	GAME_COMM_SYS_PTR game_comm;    ///<�Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
	void * comm_infowin;		///<INFOWIN�ʐM���[�N

	ISS_SYS * iss_sys;		// ISS�V�X�e��
	
	u8 field_comm_error_req;      ///<TRUE:�t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g
	u8 padding[3];
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static u32 GAMESYS_WORK_GetSize(void)
{
	return sizeof(GAMESYS_WORK);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
	gsys->heapID = heapID;
	gsys->init_param = init_param;
	gsys->procsys = GFL_PROC_LOCAL_boot(gsys->heapID);
	gsys->proc_result = GFL_PROC_MAIN_NULL;
	gsys->evcheck_func = NULL;
	gsys->evcheck_context = NULL;
	gsys->event = NULL;

	gsys->gamedata = GAMEDATA_Create(gsys->heapID);
	gsys->fieldmap = NULL;
	gsys->game_comm = GameCommSys_Alloc(gsys->heapID, gsys->gamedata);
	gsys->comm_infowin = NULL;
	gsys->iss_sys = ISS_SYS_Create( gsys->gamedata, heapID );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Delete(GAMESYS_WORK * gsys)
{
	ISS_SYS_Delete(gsys->iss_sys);
	GAMEDATA_Delete(gsys->gamedata);
	GameCommSys_Free(gsys->game_comm);
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param)
{
	GAMESYS_WORK_Init(gsys, heapID, init_param);
}

//------------------------------------------------------------------
/**
 * @return BOOL	TRUE�̂Ƃ��A�Q�[���I��
 */
//------------------------------------------------------------------
static BOOL GameSystem_Main(GAMESYS_WORK * gsys)
{
	//Game Server Proccess

	if(GAMEDATA_IsFrameSpritMode(gsys->gamedata)) //�t���[��������Ԃɂ���ꍇ
	{
		//��������R�O�t���[���p�̃L�[��Ԃ��悤�ɂ��܂�
		GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_30);
	}

	// ���̃t���[�����O�ȊO�̏ꍇ�A�����������s���Ă���
	// �����������ɂ͏��������̍ŏ��̃t���[�������A�����K�v������
	// �����łȂ��ꍇ�A�p���̃t���O�����Ԃ�
	if(!GAMEDATA_GetAndAddFrameSpritCount(gsys->gamedata))
	{
		//PlayerController/Event Trigger
		//�C�x���g�N���`�F�b�N�����i�V�`���G�[�V�����ɂ�蕪��j
		GAMESYSTEM_EVENT_CheckSet(gsys, gsys->evcheck_func, gsys->evcheck_context);
		//�C�x���g���s����
		GAMESYSTEM_EVENT_Main(gsys);
		//�ʐM�C�x���g���s����
		GameCommSys_Main(gsys->game_comm);
	}
	//���C���v���Z�X
	gsys->proc_result = GFL_PROC_LOCAL_Main(gsys->procsys);

	// ISS�V�X�e�����C��
	ISS_SYS_Update( gsys->iss_sys );

	if(GAMEDATA_IsFrameSpritMode(gsys->gamedata)) //�t���[��������Ԃɂ���ꍇ
	{
		//��������U�O�t���[���p�̃L�[��Ԃ��悤�ɂ��܂�
		GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_60);
	}

  //�ʐM�G���[��ʌĂяo���`�F�b�N
  if(gsys->proc_result == GFL_PROC_MAIN_CHANGE || gsys->proc_result == GFL_PROC_MAIN_NULL){
    NetErr_DispCall();
  }
  
	if (gsys->proc_result == GFL_PROC_MAIN_NULL && gsys->event == NULL)
	{
		//�v���Z�X���C�x���g�����݂��Ȃ��Ƃ��A�Q�[���I��
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_End(GAMESYS_WORK * gsys)
{
	GFL_PROC_LOCAL_Exit(gsys->procsys);
	GAMESYS_WORK_Delete(gsys);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_EVENT_EntryCheckFunc(GAMESYS_WORK * gsys, EVCHECK_FUNC evcheck_func, void * context)
{
	gsys->evcheck_func = evcheck_func;
	gsys->evcheck_context = context;
}

//============================================================================================
//
//		�O���C���^�[�t�F�C�X�֐�
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_SetNextProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_CallProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMESYSTEM_CallFieldProc(GAMESYS_WORK * gsys)
{
	GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(fieldmap), &FieldProcData, gsys);
}

//------------------------------------------------------------------
//	�v���Z�X���݃`�F�b�N
//------------------------------------------------------------------
GFL_PROC_MAIN_STATUS GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys)
{
	return gsys->proc_result;
}
//------------------------------------------------------------------
//	�Q�[���f�[�^�擾
//------------------------------------------------------------------
GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys)
{
	return gsys->gamedata;
}

//------------------------------------------------------------------
//	�����v���C���[���[�N�擾
//------------------------------------------------------------------
PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys)
{
	return GAMEDATA_GetMyPlayerWork(gsys->gamedata);
}
//------------------------------------------------------------------
//	�q�[�vID�擾
//------------------------------------------------------------------
HEAPID GAMESYSTEM_GetHeapID(GAMESYS_WORK * gsys)
{
	return gsys->heapID;
}

//------------------------------------------------------------------
//	�C�x���g�擾
//------------------------------------------------------------------
GMEVENT * GAMESYSTEM_GetEvent(GAMESYS_WORK * gsys)
{
	return gsys->event;
}

//------------------------------------------------------------------
//	�C�x���g�ݒ�
//------------------------------------------------------------------
void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT * event)
{
	gsys->event = event;
}

//------------------------------------------------------------------
//	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^�擾
//------------------------------------------------------------------
FIELDMAP_WORK * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys)
{
	//GF_ASSERT(gsys->fieldmap != NULL);
	return gsys->fieldmap;
}

//------------------------------------------------------------------
//	�t�B�[���h�}�b�v���[�N�̃|�C���^�Z�b�g
//------------------------------------------------------------------
void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap)
{
	gsys->fieldmap = fieldmap;
}

//--------------------------------------------------------------
//  �t�B�[���h�}�b�v���[�N�|�C���^�L���`�F�b�N
//--------------------------------------------------------------
BOOL GAMESYSTEM_CheckFieldMapWork( const GAMESYS_WORK *gsys )
{
  if( gsys->fieldmap != NULL ){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�ݒ�
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @param   error_req		TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
void GAMESYSTEM_SetFieldCommErrorReq( GAMESYS_WORK *gsys, BOOL error_req )
{
  gsys->field_comm_error_req = error_req;
}

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�擾
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @retval  TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
BOOL GAMESYSTEM_GetFieldCommErrorReq( const GAMESYS_WORK *gsys )
{
  return gsys->field_comm_error_req;
}

//--------------------------------------------------------------
//	ISS�V�X�e���擾
//--------------------------------------------------------------
ISS_SYS * GAMESYSTEM_GetIssSystem( GAMESYS_WORK * gsys )
{
	return gsys->iss_sys;
}

//==================================================================
/**
 * �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^���擾����
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @retval  GAME_COMM_SYS_PTR		�Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
 */
//==================================================================
GAME_COMM_SYS_PTR GAMESYSTEM_GetGameCommSysPtr(GAMESYS_WORK *gsys)
{
  return gsys->game_comm;
}

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̎擾
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @retval  TRUE:�펞�ʐMON�A�@FALSE:�펞�ʐMOFF
 */
//--------------------------------------------------------------
BOOL GAMESYSTEM_GetAlwaysNetFlag(const GAMESYS_WORK * gsys)
{
	return GAMEDATA_GetAlwaysNetFlag(gsys->gamedata);
}

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̐ݒ�
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @param	is_on			TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//--------------------------------------------------------------
void GAMESYSTEM_SetAlwaysNetFlag( GAMESYS_WORK * gsys, BOOL is_on )
{	
	GAMEDATA_SetAlwaysNetFlag(gsys->gamedata, is_on);
}

#ifdef PM_DEBUG
//==================================================================
/**
 * �f�o�b�O�F�Q�[������V�X�e���p���[�N�擾
 *
 * @retval  GAMESYS_WORK *		
 */
//==================================================================
GAMESYS_WORK * DEBUG_GameSysWorkPtrGet(void)
{
  return GameSysWork;
}

#endif //PM_DEBUG
