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

#include "gamesystem/game_init.h"

#include "src/field/event_mapchange.h"

#include "poke_tool/monsno_def.h"

//============================================================================================
//============================================================================================
enum {
	HEAPID_GAMESYS = HEAPID_PROC,
	
	HEAPSIZE_GAMESYS = 0x4000,
	//x200000
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
extern const GFL_PROC_DATA FieldProcData;
FS_EXTERN_OVERLAY(fieldmap);

static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, GAME_INIT_WORK * init_param);
static BOOL GameSystem_Main(GAMESYS_WORK * gsys);
static void GameSystem_End(GAMESYS_WORK * gsys);
static u32 GAMESYS_WORK_GetSize(void);
static void DEBUG_MyPokeAdd(GAMESYS_WORK * gsys);


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
	
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMESYS, HEAPSIZE_GAMESYS );
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
	case GAMEINIT_MODE_FIRST:
	case GAMEINIT_MODE_DEBUG:
		{
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, game_init);
			GAMESYSTEM_SetEvent(gsys, event);
			//�K���Ɏ莝���|�P������Add
			DEBUG_MyPokeAdd(gsys);
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
GAME_INIT_WORK * DEBUG_GetGameInitWork(GAMEINIT_MODE mode, u16 mapid, VecFx32 *pos, s16 dir, BOOL always_net)
{
  GFL_STD_MemClear(&TestGameInitWork, sizeof(GAME_INIT_WORK));
	TestGameInitWork.mode = mode;
	TestGameInitWork.mapid = mapid;
	TestGameInitWork.pos = *pos;
	TestGameInitWork.dir = dir;
	TestGameInitWork.always_net = always_net;
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

	BOOL proc_result;

	EVCHECK_FUNC evcheck_func;
	void * evcheck_context;
	GMEVENT * event;

	GAMEDATA * gamedata;
	void * fieldmap;
	GAME_COMM_SYS_PTR game_comm;    ///<�Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
	void * comm_infowin;		///<INFOWIN�ʐM���[�N
	
	u8 always_net;          ///<TRUE:�펞�ʐM
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
	gsys->proc_result = FALSE;
	gsys->evcheck_func = NULL;
	gsys->evcheck_context = NULL;
	gsys->event = NULL;

	gsys->gamedata = GAMEDATA_Create(gsys->heapID);
	gsys->fieldmap = NULL;
	gsys->game_comm = GameCommSys_Alloc(gsys->heapID, gsys->gamedata);
	gsys->comm_infowin = NULL;
	gsys->always_net = init_param->always_net;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GAMESYS_WORK_Delete(GAMESYS_WORK * gsys)
{
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

	if(GAMEDATA_IsFrameSpritMode(gsys->gamedata)) //�t���[��������Ԃɂ���ꍇ
	{
		//��������U�O�t���[���p�̃L�[��Ԃ��悤�ɂ��܂�
		GFL_UI_ChangeFrameRate(GFL_UI_FRAMERATE_60);
	}

	if (gsys->proc_result == FALSE && gsys->event == NULL)
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
BOOL GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys)
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
void * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys)
{
	GF_ASSERT(gsys->fieldmap != NULL);
	return gsys->fieldmap;
}
//------------------------------------------------------------------
//	�t�B�[���h�}�b�v���[�N�̃|�C���^�Z�b�g
//------------------------------------------------------------------
void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, void * fieldmap)
{
	gsys->fieldmap = fieldmap;
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
BOOL GAMESYSTEM_GetAlwaysNetFlag(GAMESYS_WORK * gsys)
{
	return gsys->always_net;
}


//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p�ɓK���Ɏ莝���|�P������Add
 * @param   gsys		
 */
//--------------------------------------------------------------
static void DEBUG_MyPokeAdd(GAMESYS_WORK * gsys)
{
	POKEPARTY *party;
	POKEMON_PARAM *pp;
	
	party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys));

	pp = PP_Create(MONSNO_WANIBAAN, 100, 123456, GFL_HEAPID_APP);
	
	PokeParty_Add(party, pp);
	PP_Setup(pp, MONSNO_ONOKKUSU, 100, 123456);
	PokeParty_Add(party, pp);
	PP_Setup(pp, MONSNO_BANBIINA, 100, 123456);
	PokeParty_Add(party, pp);
	
	GFL_HEAP_FreeMemory(pp);
}
