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
#include "gamesystem/playerwork.h"

//============================================================================================
//============================================================================================
enum {
	HEAPID_GAMESYS = HEAPID_TAMADA_DEBUG,

	HEAPSIZE_GAMESYS = 0x2000,
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


//============================================================================================
//============================================================================================
extern const GFL_PROC_DATA DebugFieldProcData;

static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, void * pwk);
static BOOL GameSystem_Main(GAMESYS_WORK * gsys);
static void GameSystem_End(GAMESYS_WORK * gsys);
static u32 GameSystem_GetGameSysWorkSize(void);


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
	u32 work_size = GameSystem_GetGameSysWorkSize();
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_GAMESYS, HEAPSIZE_GAMESYS );
	gsys = GFL_PROC_AllocWork(proc, work_size, HEAPID_GAMESYS);
	GameSysWork = gsys;
	GameSystem_Init(gsys, HEAPID_GAMESYS, pwk);
#if 1		/* �b��I�Ƀv���Z�X�o�^ */
	GameSystem_CallProc(gsys, NO_OVERLAY_ID, &DebugFieldProcData, gsys);
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



//============================================================================================
//============================================================================================
enum {
	PLAYER_MAX = 5,
};
//------------------------------------------------------------------
/**
 * @brief	�Q�[������V�X�e���p���[�N��`
 */
//------------------------------------------------------------------
struct _GAMESYS_WORK {
	HEAPID heapID;			///<�g�p����q�[�v�w��ID
	void * parent_work;		///<�e�i�Ăяo�����j��������p�������[�N�ւ̃|�C���^

	GFL_PROCSYS * procsys;	///<�g�p���Ă���PROC�V�X�e���ւ̃|�C���^
	PLAYER_WORK playerWork[PLAYER_MAX];
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static u32 GameSystem_GetGameSysWorkSize(void)
{
	return sizeof(GAMESYS_WORK);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_Init(GAMESYS_WORK * gsys, HEAPID heapID, void * pwk)
{
	int i;
	gsys->heapID = heapID;
	gsys->parent_work = pwk;
	gsys->procsys = GFL_PROC_LOCAL_boot(gsys->heapID);

	for (i = 0; i < PLAYER_MAX; i++) {
		PLAYERWORK_init(&gsys->playerWork[i]);
	}
}

//------------------------------------------------------------------
/**
 * @return BOOL	TRUE�̂Ƃ��A�v���Z�X�I��
 */
//------------------------------------------------------------------
static BOOL GameSystem_Main(GAMESYS_WORK * gsys)
{
	//Game Server Proccess
	//	PlayerController/Event
	if (GFL_PROC_LOCAL_Main(gsys->procsys)) {
		return FALSE;
	} else {
		return TRUE;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void GameSystem_End(GAMESYS_WORK * gsys)
{
	GFL_PROC_LOCAL_Exit(gsys->procsys);
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
void GameSystem_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_SetNextProc(gsys->procsys, ov_id, procdata, pwk);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GameSystem_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk)
{
	GFL_PROC_LOCAL_CallProc(gsys->procsys, ov_id, procdata, pwk);
}


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	PLAYER_WORK������
 */
//------------------------------------------------------------------
void PLAYERWORK_init(PLAYER_WORK * player)
{
	player->zoneID = 0;
	player->position.x = 0;
	player->position.y = 0;
	player->position.z = 0;
	player->direction = 0;
	player->mystatus.id = 0;
	player->mystatus.sex = 0;
	player->mystatus.region_code = 0;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void PLAYERWORK_setPosition(PLAYER_WORK * player, const VecFx32 * pos)
{
	player->position = *pos;
}

const VecFx32 * PLAYERWORK_getPosition(const PLAYER_WORK * player)
{
	return &player->position;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void PLAYERWORK_setZoneID(PLAYER_WORK * player, ZONEID zoneid)
{
	player->zoneID = zoneid;
}

ZONEID PLAYERWORK_getZoneID(const PLAYER_WORK * player)
{
	return player->zoneID;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void PLAYERWORK_setDirection(PLAYER_WORK * player, u16 direction)
{
	player->direction = direction;
}

u16 PLAYERWORK_getDirection(const PLAYER_WORK * player)
{
	return player->direction;
}

