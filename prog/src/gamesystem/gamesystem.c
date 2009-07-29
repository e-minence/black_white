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
#ifdef PM_DEBUG
#include "item/item.h"  //�f�o�b�O�A�C�e�������p
#include "savedata/box_savedata.h"  //�f�o�b�O�A�C�e�������p
#endif

//============================================================================================
//============================================================================================
enum {
	HEAPID_GAMESYS = HEAPID_PROC,
	
	HEAPSIZE_GAMESYS = 0x6000,
	//x200000
};

///HEAPID_UNION�̃q�[�v�T�C�Y
#define HEAPSIZE_APP_CONTROL      (0x1000)

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
static void DEBUG_MYITEM_MakeBag(MYITEM_PTR myitem, int heapID);


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
	case GAMEINIT_MODE_FIRST:
	case GAMEINIT_MODE_DEBUG:
		{
#ifdef PM_DEBUG
			GMEVENT * event = DEBUG_EVENT_SetFirstMapIn(gsys, game_init);
			GAMESYSTEM_SetEvent(gsys, event);
			//�K���Ɏ莝���|�P������Add
			DEBUG_MyPokeAdd(gsys);
			//�f�o�b�O�A�C�e���ǉ�
			DEBUG_MYITEM_MakeBag(GAMEDATA_GetMyItem(GAMESYSTEM_GetGameData(gsys)), GFL_HEAPID_APP);
#endif //PM_DEBUG
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
  MYSTATUS *myStatus;
	POKEPARTY *party;
	POKEMON_PARAM *pp;
	const STRCODE *name;
	
	party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(gsys));
  myStatus = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));
  name = MyStatus_GetMyName( myStatus );
  
	pp = PP_Create(MONSNO_WANIBAAN, 100, 123456, GFL_HEAPID_APP);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);

	PP_Setup(pp, MONSNO_ONOKKUSU, 100, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);

	PP_Setup(pp, MONSNO_BANBIINA, 100, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);

	{
		int i,j;
		BOX_DATA* pBox = SaveData_GetBoxData(GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys)));

		for(i=0;i<18;i++){
			for(j=0;j<5;j++){
				POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle(MONSNO_ZENIGAME+i+j, 0, GFL_HEAPID_APP);
				u32 ret = POKE_PERSONAL_GetParam(ppd,POKEPER_ID_sex);

				PP_SetupEx(pp, MONSNO_ZENIGAME+i+j, i+j, 123456,PTL_SETUP_POW_AUTO, ret);
				PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
				PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );

				BOXDAT_PutPokemonBox(pBox, i, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst(pp));

				POKE_PERSONAL_CloseHandle(ppd);
			}
		}
	}
	
	GFL_HEAP_FreeMemory(pp);
}

#ifdef PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p�F�K���Ɏ莝���𐶐�����
 * @param	myitem	�莝���A�C�e���\���̂ւ̃|�C���^
 */
//------------------------------------------------------------------
static const ITEM_ST DebugItem[] = {
	{ ITEM_MASUTAABOORU,	111 },
	{ ITEM_MONSUTAABOORU,	222 },
	{ ITEM_SUUPAABOORU,		333 },
	{ ITEM_HAIPAABOORU,		444 },
	{ ITEM_PUREMIABOORU,	555 },
	{ ITEM_DAIBUBOORU,		666 },
	{ ITEM_TAIMAABOORU,		777 },
	{ ITEM_RIPIITOBOORU,	888 },
	{ ITEM_NESUTOBOORU,		999 },
	{ ITEM_GOOZYASUBOORU,	100 },
	{ ITEM_KIZUGUSURI,		123 },
	{ ITEM_NEMUKEZAMASI,	456 },
	{ ITEM_BATORUREKOODAA,  1},  // �o�g�����R�[�_�[
	{ ITEM_TAUNMAPPU,		1 },
	{ ITEM_ZITENSYA,		1 },
	{ ITEM_NANDEMONAOSI,	18 },
	{ ITEM_PIIPIIRIKABAA,	18 },
	{ ITEM_PIIPIIMAKKUSU,	18 },
	{ ITEM_ANANUKENOHIMO, 50 },
	{ ITEM_GOORUDOSUPUREE, 50 },
	{ ITEM_DOKUKESI,		18 },		// �ǂ�����
	{ ITEM_YAKEDONAOSI,		19 },		// �₯�ǂȂ���
	{ ITEM_KOORINAOSI,		20 },		// ������Ȃ���
	{ ITEM_MAHINAOSI,		22 },		// �܂ЂȂ���
	{ ITEM_EFEKUTOGAADO,	54 },		// �G�t�F�N�g�K�[�h
	{ ITEM_KURITHIKATTAA,	55 },		// �N���e�B�J�b�^�[
	{ ITEM_PURASUPAWAA,		56 },		// �v���X�p���[
	{ ITEM_DHIFENDAA,		57 },		// �f�B�t�F���_�[
	{ ITEM_SUPIIDAA,		58 },		// �X�s�[�_�[
	{ ITEM_YOKUATAARU,		59 },		// ���N�A�^�[��
	{ ITEM_SUPESYARUAPPU,	60 },		// �X�y�V�����A�b�v
	{ ITEM_SUPESYARUGAADO,	61 },		// �X�y�V�����K�[�h
	{ ITEM_PIPPININGYOU,	62 },		// �s�b�s�ɂ񂬂傤
	{ ITEM_ENEKONOSIPPO,	63 },		// �G�l�R�̃V�b�|
	{ ITEM_GENKINOKAKERA,	28 },		// ���񂫂̂�����
	{ ITEM_KAIHUKUNOKUSURI,	28 },		// ���񂫂̂�����
	{ ITEM_PIIPIIEIDO,	28 },
	{ ITEM_PIIPIIEIDAA,	28 },
	{ ITEM_DAAKUBOORU,	13 },		// �_�[�N�{�[��
	{ ITEM_HIIRUBOORU,  14 },		// �q�[���{�[��
	{ ITEM_KUIKKUBOORU,	15 },		// �N�C�b�N�{�[��
	{ ITEM_PURESYASUBOORU,	16 },	// �v���V�A�{�[��
	{ ITEM_TOMODATITETYOU,  1},  // �Ƃ������蒠
	{ ITEM_POFINKEESU,  1},  // �|���g�P�[�X
	{ ITEM_MOKOSINOMI,	50},	//���R�V�̂�
	{ ITEM_GOSUNOMI,	50},	//�S�X�̂�
	{ ITEM_RABUTANOMI,	50},	//���u�^�̂�

  { ITEM_WAZAMASIN01,  328},		// �킴�}�V���O�P
  {  ITEM_WAZAMASIN02	, 329 },		// �킴�}�V���O�Q
  {  ITEM_WAZAMASIN03	, 330 },		// �킴�}�V���O�R
  {  ITEM_WAZAMASIN04	, 331 },		// �킴�}�V���O�S
  {  ITEM_WAZAMASIN05	, 332 },		// �킴�}�V���O�T
  
};

static void DEBUG_MYITEM_MakeBag(MYITEM_PTR myitem, int heapID)
{
	u32	i;

	MYITEM_Init( myitem );
	for( i=0; i<NELEMS(DebugItem); i++ ){
		MYITEM_AddItem( myitem, DebugItem[i].id, DebugItem[i].no, heapID );
	}
}

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
