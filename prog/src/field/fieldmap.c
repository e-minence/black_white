//============================================================================================
/**
 * @file	fieldmap.c
 * @brief	�t�B�[���h�}�b�v�`�惁�C��
 * @author	
 * @date	
 *
 */
//============================================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"		//GFL_HEAPID_APP
#include "net/network_define.h"
#include "textprint.h"
#include "arc_def.h"
#include "system/g3d_tool.h"
#include "sound/snd_strm.h"
#include "arc/snd_strm.naix"

#include "print/gf_font.h"

#include "fieldmap_local.h"
#include "field_common.h"
#include "field_actor.h"
#include "field_player.h"
#include "field_camera.h"
#include "field_data.h"
#include "field_subscreen.h"
#include "field/field_msgbg.h"

#include "weather.h"
#include "field_fog.h"
#include "field_light.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"

#include "event_mapchange.h"
#include "event_debug_menu.h"
#include "event_fieldmap_menu.h"
#include "event_battle.h"
#include "event_fieldtalk.h"

#include "field_comm_actor.h"
#include "field_comm/field_comm_main.h"
#include "field_comm/field_comm_event.h"

#include "fldmmdl.h"

#include "field_debug.h"

#include "map_matrix.h"

extern FLDMMDL * Player_GetFldMMdl( PC_ACTCONT *pcActCont );

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	���C������
 *
 *
 *
 *
 *
 */
//============================================================================================

static void				SetupGameSystem(FIELD_MAIN_WORK * fieldWork, HEAPID heapID );
static void				RemoveGameSystem( FIELD_MAIN_WORK * fieldWork );
static void				MainGameSystem( FIELD_MAIN_WORK * fieldWork );


typedef enum {
	GAMEMODE_BOOT = 0,
	GAMEMODE_NORMAL,
	GAMEMODE_FINISH,
}GAMEMODE;

//------------------------------------------------------------------
/**
 * @brief	�\���̒�`
 */
//------------------------------------------------------------------
struct _FIELD_MAIN_WORK
{
	GAMESYS_WORK*	gsys;
    VecFx32         recvWork;
	HEAPID			heapID;
	GAMEMODE		gamemode;
	int				seq;
	int				timer;
	
	u16				map_id;
	const DEPEND_FUNCTIONS * ftbl;
	FIELD_CAMERA*	camera_control;
	PC_ACTCONT*		pcActCont;
	FLD_ACTCONT*	fldActCont;
	VecFx32			now_pos;


	FIELD_LIGHT*	light;
	FIELD_FOG_WORK*	fog;
	FIELD_WEATHER*	weather_sys;
	
	int				key_cont;
	
	void *pGridCont;
	MAP_MATRIX *pMapMatrix;
	FLDMAPPER_RESISTDATA map_res;
	
	FIELD_COMM_MAIN *commSys;
	FLD_COMM_ACTOR *commActorTbl[FLD_COMM_ACTOR_MAX];
	
	FLDMSGBG *fldMsgBG;
	
	FLDMMDLSYS *fldMMdlSys;
	
	FIELD_DEBUG_WORK *debugWork;


	/* �ȉ���FIELD_SETUP�̃����o�[ */

	GFL_G3D_UTIL*			g3Dutil;		//g3Dutil Lib �n���h��
	u16						g3DutilUnitIdx;	//g3Dutil Unit�C���f�b�N�X
	GFL_G3D_SCENE*			g3Dscene;		//g3Dscene Lib �n���h��
	GFL_G3D_CAMERA*			g3Dcamera;		//g3Dcamera Lib �n���h��
	GFL_G3D_LIGHTSET*		g3Dlightset;	//g3Dlight Lib �n���h��
	GFL_TCB*				g3dVintr;		//3D�pvIntrTask�n���h��
	GFL_BBDACT_SYS*			bbdActSys;		//�r���{�[�h�A�N�g�V�X�e���ݒ�n���h��

	FLDMAPPER*				g3Dmapper;
};

//------------------------------------------------------------------
//------------------------------------------------------------------
struct _DEPEND_FUNCTIONS{
	void (*create_func)(FIELD_MAIN_WORK*, VecFx32*, u16);
	void (*main_func)(FIELD_MAIN_WORK*, VecFx32*);
	void (*delete_func)(FIELD_MAIN_WORK*);
};

//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
static GMEVENT * FieldEventCheck(GAMESYS_WORK * gsys, void * work);

FIELD_MAIN_WORK* fieldWork;

static void fieldMainCommActorFree( FIELD_MAIN_WORK *fieldWork );
static void fieldMainCommActorProc( FIELD_MAIN_WORK *fieldWork );
//���o�[�̏������ƊJ��


//------------------------------------------------------------------
//------------------------------------------------------------------
static int GetSceneID(GAMESYS_WORK * gsys)
{
	PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
	ZONEID id = PLAYERWORK_getZoneID(pw);
	return id;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const VecFx32 * GetStartPos(GAMESYS_WORK * gsys)
{
	PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
	return PLAYERWORK_getPosition(pw);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static const u16 GetStartDirection(GAMESYS_WORK * gsys)
{
	PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
	OS_Printf("Start Dir = %d\n",pw->direction);
	return pw->direction;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void SetMapperData(FIELD_MAIN_WORK * fieldWork)
{
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(fieldWork->gsys);
	FIELDDATA_SetMapperData(fieldWork->map_id,
			GAMEDATA_GetSeasonID(gamedata),
			&fieldWork->map_res,
			fieldWork->pMapMatrix );
}


//------------------------------------------------------------------
/**
 * @brief	���[�N�̊m�ۂƔj��
 */
//------------------------------------------------------------------
FIELD_MAIN_WORK *	FIELDMAP_Create(GAMESYS_WORK * gsys, HEAPID heapID )
{
	fieldWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_MAIN_WORK) );
	fieldWork->heapID = heapID;
	fieldWork->gamemode = GAMEMODE_BOOT;
	fieldWork->gsys = gsys;
	fieldWork->map_id = GetSceneID(gsys);
	fieldWork->ftbl = FIELDDATA_GetFieldFunctions(fieldWork->map_id);
	//�T�C�Y�͎b��BDP�ł̍ő�T�C�Y��30x30
	fieldWork->pMapMatrix = MAP_MATRIX_CreateWork( heapID );

	//�ʐM�p����
	fieldWork->commSys = FIELD_COMM_MAIN_InitSystem( heapID , GFL_HEAPID_APP );
#if 0
    //�T�E���h�p����
	SND_STRM_SetUp( ARCID_SNDSTRM, NARC_snd_strm_Firestarter_swav, SND_STRM_PCM8, SND_STRM_8KHZ, GFL_HEAPID_APP );
    SND_STRM_Play();
#endif
	return fieldWork;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void* FieldMain_GetCommSys( const FIELD_MAIN_WORK *fieldWork )
{
	return (void*)fieldWork->commSys;
}

FLDMSGBG * FIELDMAP_GetFLDMSGBG( FIELD_MAIN_WORK *fieldWork )
{
	return( fieldWork->fldMsgBG );
}

FIELD_CAMERA * FIELDMAP_GetFieldCamera( FIELD_MAIN_WORK *fieldWork )
{
	return( fieldWork->camera_control );
}

// ���C�g�Ǘ����[�N
FIELD_LIGHT * FIELDMAP_GetFieldLight( FIELD_MAIN_WORK *fieldWork )
{
	return ( fieldWork->light );
}

// �t�H�O�Ǘ����[�N
FIELD_FOG_WORK * FIELDMAP_GetFieldFog( FIELD_MAIN_WORK *fieldWork )
{
	return ( fieldWork->fog );
}

// �V�C�Ǘ����[�N
FIELD_WEATHER * FIELDMAP_GetFieldWeather( FIELD_MAIN_WORK *fieldWork )
{
	return ( fieldWork->weather_sys );
}


FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELD_MAIN_WORK *fieldWork );

FLDMMDLSYS * FIELDMAP_GetFldMMdlSys( FIELD_MAIN_WORK *fieldWork )
{
	return fieldWork->fldMMdlSys;
}

FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELD_MAIN_WORK *fieldWork );

FIELD_DEBUG_WORK * FIELDMAP_GetDebugWork( FIELD_MAIN_WORK *fieldWork )
{
	return fieldWork->debugWork;
}

GAMESYS_WORK * FIELDMAP_GetGameSysWork( FIELD_MAIN_WORK *fieldWork );

GAMESYS_WORK * FIELDMAP_GetGameSysWork( FIELD_MAIN_WORK *fieldWork )
{
	return fieldWork->gsys;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void	FIELDMAP_Delete( FIELD_MAIN_WORK * fldWork )
{
#if 0
    SND_STRM_Stop();
    SND_STRM_Release();
#endif
	MAP_MATRIX_Delete( fldWork->pMapMatrix );

	//FIXME:�t�B�[���h�𔲂���Ƃ������AComm�̃f�[�^�̈�̊J����������
	FIELD_COMM_MAIN_TermSystem( fldWork->commSys , FALSE );
	GFL_HEAP_FreeMemory( fldWork );
	fieldWork = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
BOOL	FIELDMAP_Main( GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork )
{
	BOOL return_flag = FALSE,bSkip = FALSE;
	int i;

	fieldWork->timer++;

	switch( fieldWork->seq ){

	case 0:
		//��{�V�X�e���Z�b�g�A�b�v
		SetupGameSystem( fieldWork, fieldWork->heapID );
		fieldWork->seq++;
        break;

	case 1:

		//�Z�b�g�A�b�v
		fieldWork->fldMsgBG = FLDMSGBG_Setup( fieldWork->heapID );

		fieldWork->camera_control = FIELD_CAMERA_Create(
				fieldWork, fieldWork->g3Dcamera, fieldWork->heapID );

		SetMapperData(fieldWork);
		FLDMAPPER_ResistData(
			 fieldWork->g3Dmapper, &fieldWork->map_res );

		//�o�^�e�[�u�����ƂɌʂ̏������������Ăяo��
		{
			u16		dir;

			fieldWork->now_pos = *GetStartPos(gsys);
			TAMADA_Printf("start X,Y,Z=%d,%d,%d\n",
					FX_Whole(fieldWork->now_pos.x),
					FX_Whole(fieldWork->now_pos.y),
					FX_Whole(fieldWork->now_pos.z));
			dir = GetStartDirection(gsys);
			fieldWork->ftbl->create_func( fieldWork, &fieldWork->now_pos, dir );
			FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
		}


		// �t�H�O�V�X�e������
		fieldWork->fog	= FIELD_FOG_Create( fieldWork->heapID );

		// ���C�g�V�X�e������
		fieldWork->light = FIELD_LIGHT_Create( 0, 0, 0, fieldWork->fog, fieldWork->g3Dlightset, fieldWork->heapID );

		// �V�C�V�X�e������
		fieldWork->weather_sys = FIELD_WEATHER_Init( fieldWork->camera_control, fieldWork->light, fieldWork->fog, fieldWork->heapID );
		// �V�C����
		FIELD_WEATHER_Set( fieldWork->weather_sys, WEATHER_NO_SUNNY, fieldWork->heapID );
		
		//���o�[�̏�����
		FIELD_SUBSCREEN_Init(fieldWork->heapID);
		
		//�t�B�[���h�f�o�b�O������
		fieldWork->debugWork = FIELD_DEBUG_Init( fieldWork, fieldWork->heapID );
		fieldWork->seq++;
		break;

	case 2:
		MainGameSystem( fieldWork );
		FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
		FIELD_DEBUG_UpdateProc( fieldWork->debugWork );
		if( fieldWork->fldMMdlSys != NULL ){
			FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
		}

		if (FLDMAPPER_CheckTrans(fieldWork->g3Dmapper) == FALSE) {
			break;
		}

		//�t�B�[���h�}�b�v�p�C�x���g�N���`�F�b�N���Z�b�g����
		GAMESYSTEM_EVENT_EntryCheckFunc(gsys, FieldEventCheck, fieldWork);

		fieldWork->gamemode = GAMEMODE_NORMAL;
		fieldWork->seq++;
		break;

	case 3:
		if( GAMESYSTEM_GetEvent(gsys) == FALSE) {
		
			fieldWork->key_cont = GFL_UI_KEY_GetCont();
			
			//�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
			fieldWork->ftbl->main_func( fieldWork, &fieldWork->now_pos );
			
			//�ʐM�p�A�N�^�[�X�V
			//fieldMainCommActorProc( fieldWork );

			//Map�V�X�e���Ɉʒu��n���Ă���B
			//���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
			FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
		}
		//�ʐM�p�A�N�^�[�X�V
		fieldMainCommActorProc( fieldWork );

		//�ʐM�p����(�v���C���[�̍��W�̐ݒ�Ƃ�
		FIELD_COMM_MAIN_UpdateCommSystem( fieldWork , fieldWork->gsys , fieldWork->pcActCont , fieldWork->commSys );

		
		FIELD_CAMERA_Main( fieldWork->camera_control );
		MainGameSystem( fieldWork );
		FIELD_SUBSCREEN_Main();
		FIELD_DEBUG_UpdateProc( fieldWork->debugWork );
		
		if( fieldWork->fldMMdlSys != NULL ){
			FLDMMDLSYS_UpdateProc( fieldWork->fldMMdlSys );
		}
		break;
	case 4:
		//�C�x���g�N���`�F�b�N���~����
		GAMESYSTEM_EVENT_EntryCheckFunc(gsys, NULL, NULL);
		{
			//�A�N�^�[�����v���C���[���݈ʒu��PLAYER_WORK�ɔ��f����
			VecFx32 player_pos;
			PLAYER_WORK * pw = GAMESYSTEM_GetMyPlayerWork(gsys);
			GetPlayerActTrans(fieldWork->pcActCont, &player_pos);
			PLAYERWORK_setPosition(pw, &player_pos);
		}
		//�ʐM�p�A�N�^�[�폜
		fieldMainCommActorFree( fieldWork );
		
		//���o�[�̊J��
		FIELD_SUBSCREEN_Exit();

		// �V�C�V�X�e���j��
		FIELD_WEATHER_Exit( fieldWork->weather_sys );
		fieldWork->weather_sys = NULL;

		// ���C�g�V�X�e���j��
		FIELD_LIGHT_Delete( fieldWork->light );

		// �t�H�O�V�X�e���j��
		FIELD_FOG_Delete( fieldWork->fog );

		FIELD_CAMERA_Delete( fieldWork->camera_control );

		//�o�^�e�[�u�����ƂɌʂ̏I���������Ăяo��
		fieldWork->ftbl->delete_func(fieldWork);

        FLDMAPPER_ReleaseData( fieldWork->g3Dmapper );
		
		FLDMSGBG_Delete( fieldWork->fldMsgBG );
		
		FIELD_DEBUG_Delete( fieldWork->debugWork );

		fieldWork->seq ++;
		break;

	case 5:
		RemoveGameSystem( fieldWork );
		return_flag = TRUE;
		break;
	}
	return return_flag;
}

//------------------------------------------------------------------
/**
 * @brief	�I�����N�G�X�g
 */
//------------------------------------------------------------------
void FIELDMAP_Close( FIELD_MAIN_WORK * fieldWork )
{
	fieldWork->gamemode = GAMEMODE_FINISH;
	fieldWork->seq = 4;
}

//============================================================================================
//============================================================================================
#include "field/eventdata_sxy.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
static void PrintDebugInfo(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork)
{
	VecFx32 pos;
	int i;
	static const VecFx32 pos_array[] = {
		{-1, 0, -1},	{ 0,  0, -1},	{+1, 0, -1},
		{-1, 0,  0},	{ 0,  0,  0},	{+1, 0,  0},
		{-1, 0, +1},	{ 0,  0, +1},	{+1, 0, +1},
	};
	static char limit[] = "  \n  \n  \n";
	for (i = 0; i < NELEMS(pos_array); i++) {
		u32 attr = 0;
		FLDMAPPER_GRIDINFO gridInfo;
		//pos = pos_array[i] * FX32_ONE * 16 + fieldWork->now_pos;
		VEC_Set(&pos,
				pos_array[i].x * 16 * FX32_ONE,
				pos_array[i].y * 16 * FX32_ONE,
				pos_array[i].z * 16 * FX32_ONE);
		VEC_Add(&pos, &fieldWork->now_pos, &pos);
		TAMADA_Printf("(x=%08x, z=%08x)",pos.x,pos.z);
		if( FLDMAPPER_GetGridInfo( fieldWork->g3Dmapper, &pos, &gridInfo ) == TRUE ){
			attr = gridInfo.gridData[0].attr;
		}
		TAMADA_Printf("%04x%c", attr, limit[i]);
	}
	TAMADA_Printf("X,Y,Z=%d,%d,%d\n",
			FX_Whole(fieldWork->now_pos.x),
			FX_Whole(fieldWork->now_pos.y),
			FX_Whole(fieldWork->now_pos.z));
	DEBUG_GFL_G3D_DumpVramInfo();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT * ConnectCheck(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork, const VecFx32 * now_pos)
{
	GAMEDATA * gamedata = GAMESYSTEM_GetGameData(gsys);
	EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData(gamedata);
	const CONNECT_DATA * cnct;
	int idx;
	idx = EVENTDATA_SearchConnectIDByPos(evdata, now_pos);
	if (idx == EXIT_ID_NONE) return NULL;

	//�}�b�v�J�ڔ����̏ꍇ�A�o�������L�����Ă���
	{
		LOCATION ent_loc;
		LOCATION_Set(&ent_loc, fieldWork->map_id, idx, 0, now_pos->x, now_pos->y, now_pos->z);
		GAMEDATA_SetEntranceLocation(gamedata, &ent_loc);
	}

	cnct = EVENTDATA_GetConnectByID(evdata, idx);
	if (CONNECTDATA_IsSpecialExit(cnct)) {
		//����ڑ��悪�w�肳��Ă���ꍇ�A�L�����Ă������ꏊ�ɔ��
		const LOCATION * sp = GAMEDATA_GetSpecialLocation(gamedata);
		return EVENT_ChangeMap(gsys, fieldWork, sp);
	} else {
		LOCATION loc_req;
		GMEVENT * event;
		CONNECTDATA_SetNextLocation(cnct, &loc_req);
		return EVENT_ChangeMap(gsys, fieldWork, &loc_req);
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static GMEVENT * PushConnectCheck(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldWork)
{
	VecFx32 now_pos = fieldWork->now_pos;
	switch (GFL_UI_KEY_GetCont()) {
	case PAD_KEY_UP:	now_pos.z -= FX32_ONE * 16; break;
	case PAD_KEY_DOWN:	now_pos.z += FX32_ONE * 16; break;
	case PAD_KEY_LEFT:	now_pos.x -= FX32_ONE * 16; break;
	case PAD_KEY_RIGHT:	now_pos.x += FX32_ONE * 16; break;
	default:
		return NULL;
	}
	{
		u32 attr = 0;
		FLDMAPPER_GRIDINFO gridInfo;
		//pos = pos_array[i] * FX32_ONE * 16 + fieldWork->now_pos;
		if( FLDMAPPER_GetGridInfo( fieldWork->g3Dmapper, &now_pos, &gridInfo ) == TRUE ){
			attr = gridInfo.gridData[0].attr;
		}
		if (attr == 0) return NULL;
	}

	return ConnectCheck(gsys, fieldWork, &now_pos);
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�N���`�F�b�N�i�b��j
 */
//------------------------------------------------------------------
static GMEVENT * FieldEventCheck(GAMESYS_WORK * gsys, void * work)
{
	enum {
		resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
		chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT
	};
	FIELD_MAIN_WORK * fieldWork = work;
	GMEVENT * event;
	int	trg = GFL_UI_KEY_GetTrg();
	int cont = GFL_UI_KEY_GetCont();

	//���W�ڑ��`�F�b�N
	event = ConnectCheck(gsys, fieldWork, &fieldWork->now_pos);
	if (event != NULL) return event;
	
	//�L�[���͐ڑ��`�F�b�N
	event = PushConnectCheck(gsys, fieldWork);
	if (event != NULL) return event;
	
	//�\�t�g���Z�b�g�`�F�b�N
	if( (cont&resetCont) == resetCont ){
		return DEBUG_EVENT_GameEnd(gsys, fieldWork);
		//return DEBUG_EVENT_FieldSample(gsys, fieldWork);
	}
	
	//�}�b�v�ύX�`�F�b�N
	if( (cont&chgCont) == chgCont ){
		return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
	}
	
	//�퓬�ڍs�`�F�b�N
	if( trg == PAD_BUTTON_START ){
		return DEBUG_EVENT_Battle(gsys, fieldWork);
	}
	
	//���j���[�N���`�F�b�N
	if( trg == PAD_BUTTON_X ){
		return EVENT_FieldMapMenu( gsys, fieldWork, fieldWork->heapID );
	}
	
	//�f�o�b�O���j���[�N���`�F�b�N
	if( trg == PAD_BUTTON_SELECT ){
		PrintDebugInfo(gsys, fieldWork);
		return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
				fieldWork->heapID, ZONEDATA_GetMapRscID(fieldWork->map_id));
	}
	
	///�ʐM�p��b����(��
	//�b�������鑤
	if( trg == PAD_BUTTON_A ){
		if( FIELD_COMM_MAIN_CanTalk( fieldWork->commSys ) == TRUE ){
			return FIELD_COMM_EVENT_StartTalk( gsys , fieldWork , fieldWork->commSys );
		}
	}
	//�b���������鑤(���ňꏏ�ɘb�����Ԃ��̃`�F�b�N�����Ă��܂�
	if( FIELD_COMM_MAIN_CheckReserveTalk( fieldWork->commSys ) == TRUE ){
		return FIELD_COMM_EVENT_StartTalkPartner( gsys , fieldWork , fieldWork->commSys );
	}
	
	//�t�B�[���h�b���|���`�F�b�N
	if( trg == PAD_BUTTON_A && FIELDMAP_CheckGridControl(fieldWork) ){
		int gx,gy,gz;
		FLDMMDL *fmmdl_talk;
		PLAYER_GRID_GetFrontGridPos( fieldWork->pcActCont, &gx, &gy, &gz );
		fmmdl_talk = FLDMMDLSYS_SearchGridPos( fieldWork->fldMMdlSys, gx, gz, FALSE );
		if( fmmdl_talk != NULL ){
			u32 scr_id = FLDMMDL_GetEventID( fmmdl_talk );
			FLDMMDL *fmmdl_player = Player_GetFldMMdl( fieldWork->pcActCont );
			return EVENT_FieldTalk( gsys, fieldWork,
				scr_id, fmmdl_player, fmmdl_talk, fieldWork->heapID );
		}
	}
	
	return NULL;
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�Z�b�g�A�b�v
 *
 *
 *
 *
 *
 */
//============================================================================================
//#define BACKGROUND_COL	(GX_RGB(23,29,31))		//�w�i�F
//#define FOG_COL			(GX_RGB(31,31,31))		//�t�H�O�F
#define BACKGROUND_COL	(GX_RGB(30,31,31))		//�w�i�F
#define FOG_COL			(GX_RGB(31,31,31))		//�t�H�O�F
#define DTCM_SIZE		(0x1000)				//DTCM�G���A�̃T�C�Y

//------------------------------------------------------------------
/**
 * @brief	�f�B�X�v���C���f�[�^
 */
//------------------------------------------------------------------
///�u�q�`�l�o���N�ݒ�\����
//�ʐM�̂��� ���C��2D�@D��C�@�e�N�X�`���C���[�W�X���b�gABC��AB	Ari081113
static const GFL_DISP_VRAM dispVram = {
	GX_VRAM_BG_128_C,				//���C��2D�G���W����BG�Ɋ��蓖�� 
	GX_VRAM_BGEXTPLTT_NONE,			//���C��2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_SUB_BG_32_H,			//�T�u2D�G���W����BG�Ɋ��蓖��
	GX_VRAM_SUB_BGEXTPLTT_NONE,		//�T�u2D�G���W����BG�g���p���b�g�Ɋ��蓖��
	GX_VRAM_OBJ_64_E,				//���C��2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_OBJEXTPLTT_NONE,		//���C��2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_SUB_OBJ_16_I,			//�T�u2D�G���W����OBJ�Ɋ��蓖��
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	//�T�u2D�G���W����OBJ�g���p���b�g�ɂɊ��蓖��
	GX_VRAM_TEX_01_AB,				//�e�N�X�`���C���[�W�X���b�g�Ɋ��蓖��
	GX_VRAM_TEXPLTT_0_G,			//�e�N�X�`���p���b�g�X���b�g�Ɋ��蓖��
	GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
	GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
};


//-----------------------------------------------------------------------------
/**
 *			�Z���A�N�^�[�V�X�e��
 */
//-----------------------------------------------------------------------------
#define FIELD_CLSYS_RESOUCE_MAX		( 64 )
static const GFL_CLSYS_INIT CLSYS_Init = {
	0, 0,
	0, 512,
	0, 128,
	0, 128,
	0,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX,
	FIELD_CLSYS_RESOUCE_MAX
};


//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�e�[�u��
 */
//------------------------------------------------------------------
#include "test_graphic/fld_act.naix"

#if 0
static	const	char	*GraphicFileTable[]={
	"test_graphic/test3d.narc",
	"test_graphic/fld_act.narc",
	"test_graphic/fld_map.narc",
	"test_graphic/sample_map.narc",
};
#endif

//static const char font_path[] = {"gfl_font.dat"};

//------------------------------------------------------------------
/**
 * @brief	�R�c�O���t�B�b�N���f�[�^
 */
//------------------------------------------------------------------
//�J���������ݒ�f�[�^
static const VecFx32 cameraTarget	= { 0, 0, 0 };
static const VecFx32 cameraPos	= { 0, (FX32_ONE * 64), (FX32_ONE * 128) };

//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };
//------------------------------------------------------------------
/**
 * @brief	�R�c���\�[�X�f�[�^
 */
//------------------------------------------------------------------
#define G3D_FRM_PRI			(1)			//�R�c�ʂ̕`��v���C�I���e�B�[
#define G3D_UTIL_RESCOUNT	(512)		//g3Dutil�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_UTIL_OBJCOUNT	(128)		//g3Dutil�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��
#define G3D_SCENE_OBJCOUNT	(256)		//g3Dscene�Ŏg�p����sceneObj�̍ő�ݒ�\��
#define G3D_OBJWORK_SZ		(64)		//g3Dscene�Ŏg�p����sceneObj�̃��[�N�T�C�Y
#define G3D_ACC_COUNT		(32)		//g3Dscene�Ŏg�p����sceneObjAccesary�̍ő�ݒ�\��
#define G3D_BBDACT_RESMAX	(64)		//billboardAct�Ŏg�p���郊�\�[�X�̍ő�ݒ�\��
#define G3D_BBDACT_ACTMAX	(256)		//billboardAct�Ŏg�p����I�u�W�F�N�g�̍ő�ݒ�\��

static const GXRgb edgeColorTable[8] = {
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
	GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),GX_RGB(10,10,10),
};
//------------------------------------------------------------------
/**
 * @brief	���[�J���錾
 */
//------------------------------------------------------------------
//�a�f�ݒ�֐�
static void	bg_init( FIELD_MAIN_WORK * fieldWork );
static void	bg_exit( FIELD_MAIN_WORK * fieldWork );
//�R�c�֐�
static void g3d_load( FIELD_MAIN_WORK * fieldWork );
static void g3d_control( FIELD_MAIN_WORK * fieldWork );
static void g3d_draw( FIELD_MAIN_WORK * fieldWork );
static void g3d_unload( FIELD_MAIN_WORK * fieldWork );
static void	g3d_vblank( GFL_TCB* tcb, void* work );

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static void	SetupGameSystem(FIELD_MAIN_WORK * fieldWork, HEAPID heapID )
{
	//����������
	GFL_STD_MtRandInit(0);

	//VRAM�N���A
	GFL_DISP_ClearVRAM( GX_VRAM_D );
	//VRAM�ݒ�
	GFL_DISP_SetBank( &dispVram );

	//BG������
	bg_init( fieldWork );
	
	// CLACT������
	GFL_CLACT_SYS_Create( &CLSYS_Init, &dispVram, heapID );
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//BMP������
	GFL_BMPWIN_Init( heapID );
	
	//FONT������
	GFL_FONTSYS_Init();
	
	//�R�c�f�[�^�̃��[�h
	g3d_load( fieldWork );
	fieldWork->g3dVintr = GFUser_VIntr_CreateTCB( g3d_vblank, (void*)fieldWork, 0 );
}

//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v�֐�
 */
//------------------------------------------------------------------
static void	RemoveGameSystem( FIELD_MAIN_WORK * fieldWork )
{
	GFL_TCB_DeleteTask( fieldWork->g3dVintr );
	g3d_unload( fieldWork );	//�R�c�f�[�^�j��


	// CLACT�j��
	GFL_CLACT_SYS_Delete();
	
	GFL_BMPWIN_Exit();

	bg_exit( fieldWork );

	//GFL_HEAP_FreeMemory( fieldWork->gs );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e�����C���֐�
 */
//------------------------------------------------------------------
static void		MainGameSystem( FIELD_MAIN_WORK * fieldWork )
{
	g3d_control( fieldWork );

	FIELD_WEATHER_Main( fieldWork->weather_sys, fieldWork->heapID );
	FIELD_FOG_Main( fieldWork->fog );
	{
		static int time;
		time += 30;
		time %= 24*3600;
		FIELD_LIGHT_Main( fieldWork->light, time );
	}
	FLDMSGBG_PrintMain( fieldWork->fldMsgBG );
	
	g3d_draw( fieldWork );

	// CLSYS���C��
	GFL_CLACT_SYS_Main();
}

//------------------------------------------------------------------
/**
 * @brief		�a�f�ݒ聕�f�[�^�]��
 */
//------------------------------------------------------------------
static void G3DsysSetup( void );
static const GFL_BG_SYS_HEADER bgsysHeader = {
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_3D
};	

static void	bg_init( FIELD_MAIN_WORK * fieldWork )
{
	//�a�f�V�X�e���N��
	GFL_BG_Init( fieldWork->heapID );

	//�w�i�F�p���b�g�쐬���]��
	{
		u16* plt = GFL_HEAP_AllocClearMemoryLo( fieldWork->heapID, 16*2 );
		plt[0] = BACKGROUND_COL;
		GFL_BG_LoadPalette( GFL_BG_FRAME0_M, plt, 16*2, 0 );	//���C����ʂ̔w�i�F�]��
		GFL_BG_LoadPalette( GFL_BG_FRAME0_S, plt, 16*2, 0 );	//�T�u��ʂ̔w�i�F�]��
		GFL_HEAP_FreeMemory( plt );
	}
	//�a�f���[�h�ݒ�
	GFL_BG_SetBGMode( &bgsysHeader );

	//�a�f�R���g���[���ݒ�
	G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2, GX_BLEND_PLANEMASK_BG3, 16, 8 );
	
	//�R�c�V�X�e���N��
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT64K,
						DTCM_SIZE, fieldWork->heapID, G3DsysSetup );
	DEBUG_GFL_G3D_SetVManSize( GFL_G3D_TEX256K, GFL_G3D_PLT64K );
	GFL_BG_SetBGControl3D( G3D_FRM_PRI );

	//�f�B�X�v���C�ʂ̑I��
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();
}

static void	bg_exit( FIELD_MAIN_WORK * fieldWork )
{
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );

	GFL_G3D_Exit();
	GFL_BG_Exit();
}

// �R�c�Z�b�g�A�b�v�R�[���o�b�N
static void G3DsysSetup( void )
{
	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_TOON );
	G3X_AntiAlias( TRUE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );
#if 1
	// �t�H�O�Z�b�g�A�b�v
    {
        u32     fog_table[8];
        int     i;

        //G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x1000, 0x01000 );
        G3X_SetFog(TRUE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x0400, 0x00e00 );

        G3X_SetFogColor(FOG_COL, 0);

        for ( i=0; i<8; i++ ){
            fog_table[i] = (u32)(((i*16)<<0) | ((i*16+4)<<8) | ((i*16+8)<<16) | ((i*16+12)<<24));
        }
        G3X_SetFogTable(&fog_table[0]);
	}

#else
	G3X_SetFog( FALSE, 0, 0, 0 );
#endif
	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),0,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog

	G3X_SetEdgeColorTable( edgeColorTable ); 
	//G3X_EdgeMarking( TRUE );
	G3X_EdgeMarking( FALSE );

	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
}


//------------------------------------------------------------------
/**
 * @brief		�R�c�f�[�^���[�h
 */
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz );
//------------------------------------------------------------------
//�쐬
static void g3d_load( FIELD_MAIN_WORK * fieldWork )
{
	//�z�u���ݒ�

	//g3Dutil���g�p���z��Ǘ�������
	fieldWork->g3Dutil = GFL_G3D_UTIL_Create( G3D_UTIL_RESCOUNT, G3D_UTIL_OBJCOUNT, fieldWork->heapID );
	//g3Dscene���g�p���Ǘ�������
	fieldWork->g3Dscene = GFL_G3D_SCENE_Create( fieldWork->g3Dutil, 
						G3D_SCENE_OBJCOUNT, G3D_OBJWORK_SZ, G3D_ACC_COUNT, TRUE, fieldWork->heapID );

	fieldWork->g3Dmapper = FLDMAPPER_Create( fieldWork->heapID );
	fieldWork->bbdActSys = GFL_BBDACT_CreateSys
					( G3D_BBDACT_RESMAX, G3D_BBDACT_ACTMAX, g3d_trans_BBD, fieldWork->heapID );

	//�J�����쐬
	fieldWork->g3Dcamera = GFL_G3D_CAMERA_CreateDefault( &cameraPos, &cameraTarget, fieldWork->heapID );
	//���C�g�쐬
	fieldWork->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, fieldWork->heapID );

	//�J�������C�g0���f
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
	OS_Printf("TEX:%06x PLT:%04x\n",
			DEBUG_GFL_G3D_GetBlankTextureSize(), DEBUG_GFL_G3D_GetBlankPaletteSize());
}
	
//����
static void g3d_control( FIELD_MAIN_WORK * fieldWork )
{
	GFL_G3D_SCENE_Main( fieldWork->g3Dscene ); 
	FLDMAPPER_Main( fieldWork->g3Dmapper );
	GFL_BBDACT_Main( fieldWork->bbdActSys );
}

//�`��
static void g3d_draw( FIELD_MAIN_WORK * fieldWork )
{
	GFL_G3D_CAMERA_Switching( fieldWork->g3Dcamera );
	GFL_G3D_LIGHT_Switching( fieldWork->g3Dlightset );
	FLDMAPPER_Draw( fieldWork->g3Dmapper, fieldWork->g3Dcamera );
	GFL_BBDACT_Draw( fieldWork->bbdActSys, fieldWork->g3Dcamera, fieldWork->g3Dlightset );
	FIELD_WEATHER_3DWrite( fieldWork->weather_sys );	// �V�C�`�揈��

	GFL_G3D_SCENE_Draw( fieldWork->g3Dscene );  
}

//�j��
static void g3d_unload( FIELD_MAIN_WORK * fieldWork )
{
	GFL_G3D_LIGHT_Delete( fieldWork->g3Dlightset );
	GFL_G3D_CAMERA_Delete( fieldWork->g3Dcamera );

	GFL_BBDACT_DeleteSys( fieldWork->bbdActSys );
	FLDMAPPER_Delete( fieldWork->g3Dmapper );

	GFL_G3D_SCENE_Delete( fieldWork->g3Dscene );  
	GFL_G3D_UTIL_Delete( fieldWork->g3Dutil );
}
	
static void	g3d_vblank( GFL_TCB* tcb, void* work )
{
	FIELD_MAIN_WORK * fieldWork = (FIELD_MAIN_WORK*)work;

	// �Z���A�N�^�[VBlank
	GFL_CLACT_SYS_VBlankFunc();	
}

//BBD�pVRAM�]���֐�
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;

	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	} else {
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���擾
 */
//------------------------------------------------------------------
GFL_G3D_CAMERA* GetG3Dcamera( FIELD_MAIN_WORK * fieldWork )
{
	return fieldWork->g3Dcamera;
}

FLDMAPPER* GetFieldG3Dmapper( FIELD_MAIN_WORK * fieldWork )
{
	return fieldWork->g3Dmapper;
}

GFL_BBDACT_SYS* GetBbdActSys( FIELD_MAIN_WORK * fieldWork )
{
	return fieldWork->bbdActSys;
}

	
//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�R���g���[��
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�ړ������̒n�`�ɉ������x�N�g���擾
 */
//------------------------------------------------------------------
#define WALK_LIMIT_HEIGHT ( 16 * FX32_ONE )

static void GetGroundMoveVec
		( const VecFx16* vecN, const VecFx32* pos, const VecFx32* vecMove, VecFx32* result )
{
	VecFx32	vecN32, posNext;
	fx32	by, valD;

	VEC_Add( pos, vecMove, &posNext );

	VEC_Set( &vecN32, vecN->x, vecN->y, vecN->z );
	valD = -( FX_Mul(vecN32.x,pos->x) + FX_Mul(vecN32.y,pos->y) + FX_Mul(vecN32.z,pos->z) ); 
	by = -( FX_Mul( vecN32.x, posNext.x ) + FX_Mul( vecN32.z, posNext.z ) + valD );
	posNext.y = FX_Div( by, vecN32.y );

	VEC_Subtract( &posNext, pos, result );
	VEC_Normalize( result, result );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL CalcSetGroundMove( const FLDMAPPER* g3Dmapper, FLDMAPPER_GRIDINFODATA* gridInfoData, 
								VecFx32* pos, VecFx32* vecMove, fx32 speed )
{
	FLDMAPPER_GRIDINFO gridInfo;
	VecFx32	posNext, vecGround;
	fx32	height = 0;
	BOOL	initSw = FALSE;

	//VEC_Normalize( &vecMove, &vecMove );
	//
	if( (gridInfoData->vecN.x|gridInfoData->vecN.y|gridInfoData->vecN.z) == 0 ){
		//vecN = {0,0,0}�̏ꍇ�͏������
		VecFx16	vecNinit = { 0, FX16_ONE, 0 };

		GetGroundMoveVec( &vecNinit, pos, vecMove, &vecGround );
		initSw = TRUE;
	} else {
		GetGroundMoveVec( &gridInfoData->vecN, pos, vecMove, &vecGround );
	}
	VEC_MultAdd( speed, &vecGround, pos, &posNext );
	if( posNext.y < 0 ){
		posNext.y = 0;	//�x�[�X���C��
	}
	if( FLDMAPPER_CheckOutRange( g3Dmapper, &posNext ) == TRUE ){
	//	OS_Printf("�}�b�v�͈͊O�ňړ��s��\n");
		return FALSE;
	}

	//�v���[���[�p����B���̈ʒu���S�ɍ����f�[�^�����݂��邽�߁A���ׂĎ擾���Đݒ�
	if( FLDMAPPER_GetGridInfo( g3Dmapper, &posNext, &gridInfo ) == FALSE ){
		return FALSE;
	}

	if( gridInfo.count ){
		int		i = 0, p;
		fx32	h_tmp, diff1, diff2;

		height = gridInfo.gridData[0].height;
		p = 0;
		i++;
		for( ; i<gridInfo.count; i++ ){
			h_tmp = gridInfo.gridData[i].height;

			diff1 = height - pos->y;
			diff2 = h_tmp - pos->y;

			if( FX_Mul( diff2, diff2 ) < FX_Mul( diff1, diff1 ) ){
				height = h_tmp;
				p = i;
			}
		}
#if 0
		if( initSw == FALSE ){
			//�ړ������e�X�g
			if(FX_Mul((height-pos->y),(height-pos->y))
					>=FX_Mul(WALK_LIMIT_HEIGHT,WALK_LIMIT_HEIGHT)){
				return FALSE;
			}
		} 
#endif
		*gridInfoData = gridInfo.gridData[p];	//�O���b�h�f�[�^�X�V
		VEC_Set( pos, posNext.x, gridInfoData->height, posNext.z );		//�ʒu���X�V
	}
	return TRUE;
}
	


//============================================================================================
//============================================================================================
#include "field_sub_nogrid.c"
#include "field_sub_grid.c"
#include "field_sub_c3.c"

const DEPEND_FUNCTIONS FieldGridFunctions = {
	GridMoveCreate,
	GridMoveMain,
	GridMoveDelete,
};

const DEPEND_FUNCTIONS FieldNoGridFunctions = {
	NoGridCreate,
	NoGridMain,
	NoGridDelete,
};

const DEPEND_FUNCTIONS FieldTestC3Functions = {
	TestC3Create,
	TestC3Main,
	TestC3Delete,
};

//======================================================================
//	comm actor
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�p�A�N�^�[�̒ǉ�
 * @param	fieldWork	FIELD_MAIN_WORK
 * @param	player		�Q�Ƃ���PLAYER_WORK
 * @retval	nothing
 */
//--------------------------------------------------------------
void FieldMain_AddCommActor(
	FIELD_MAIN_WORK *fieldWork, const PLAYER_WORK *player )
{
	int i;
	GFL_BBDACT_SYS *bbdActSys;
	GFL_BBDACT_RESUNIT_ID unitID;
	
	bbdActSys = fieldWork->bbdActSys;
	unitID = GetPlayerBBdActResUnitID( fieldWork->pcActCont );
	
	for( i = 0; i < FLD_COMM_ACTOR_MAX; i++ ){
		if( fieldWork->commActorTbl[i] == NULL ){
			fieldWork->commActorTbl[i] = FldCommActor_Init(
				player, bbdActSys, unitID, fieldWork->heapID, i );
			return;
		}
	}
}

//--------------------------------------------------------------
///	�ʐM�A�N�^�[�S�폜
//--------------------------------------------------------------
static void fieldMainCommActorFree( FIELD_MAIN_WORK *fieldWork )
{
	int i;
	
	for( i = 0; i < FLD_COMM_ACTOR_MAX; i++ ){
		if( fieldWork->commActorTbl[i] != NULL ){
			FldCommActor_Delete( fieldWork->commActorTbl[i] );
			fieldWork->commActorTbl[i] = NULL;
		}
	}
}

//--------------------------------------------------------------
///	�ʐM�A�N�^�[�X�V
//--------------------------------------------------------------
static void fieldMainCommActorProc( FIELD_MAIN_WORK *fieldWork )
{
	int i;
	
	for( i = 0; i < FLD_COMM_ACTOR_MAX; i++ ){
		if( fieldWork->commActorTbl[i] != NULL ){
			FldCommActor_Update( fieldWork->commActorTbl[i] );
		}
	}
}

//--------------------------------------------------------------
///	�t�B�[���h���X�V�\��Ԃɓ��������H
//--------------------------------------------------------------
const BOOL FIELDMAP_IsReady( const FIELD_MAIN_WORK *fieldWork )
{
	return (fieldWork->gamemode == GAMEMODE_NORMAL);
	//return (fieldWork->seq==3);
}

//--------------------------------------------------------------
///	�t�B�[���h�������I�ɍX�V����
//--------------------------------------------------------------
void FIELDMAP_ForceUpdate( FIELD_MAIN_WORK *fieldWork )
{
	//�L�[���͖͂������̂Ƃ���
	fieldWork->key_cont = 0;
	
	//�o�^�e�[�u�����ƂɌʂ̃��C���������Ăяo��
	fieldWork->ftbl->main_func( fieldWork, &fieldWork->now_pos );
	
	//Map�V�X�e���Ɉʒu��n���Ă���B
	//���ꂪ�Ȃ��ƃ}�b�v�ړ����Ȃ��̂Œ���
	FLDMAPPER_SetPos( fieldWork->g3Dmapper, &fieldWork->now_pos );
}


