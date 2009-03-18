//=============================================================================
/**
 * @file	game_data.c
 * @brief	�Q�[���i�s�f�[�^�Ǘ�
 * @author	tamada
 * @date	2008.11.04
 *
 * ��{�I�ɃQ�[���i�s�f�[�^�̓Z�[�u�f�[�^�Ƃ��đ��݂��邪�A
 * �ʐM���̕����v���C���[����t�B�[���h�}�b�v���ȂǃZ�[�u�f�[�^��
 * �̂�Ƃ͌���Ȃ���������B
 * ���̂悤�ȃZ�[�u�f�[�^�E��Z�[�u�f�[�^�֕���ɃA�N�Z�X����C���^�[�t�F�C�X��
 * �e�p�[�g���Ƃɍ\������ƃp�[�g���ƂɃA�N�Z�X��@���������A�܂������@�\��
 * ���̂��������݂���Ȃǃv���O�����S�̂̕��G���������Ă��܂��\���������B
 * ���������邽�߁A���ʃC���^�[�t�F�C�X����邱�Ƃɂ���Ċȗ������鎎��
 * �c���݂Ȃ̂œr���ŕ��j��ς��邩���B
 */
//=============================================================================

#include <gflib.h>

#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/myitem_savedata.h"
#include "poke_tool/pokeparty.h"
#include "savedata/mystatus.h"
#include "savedata/situation.h"
#include "savedata/player_data.h"
#include "gamesystem/pm_season.h"		//�G�ߒ�`�Q��

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���f�[�^���e��`
 */
//------------------------------------------------------------------
struct _GAMEDATA{
	SAVE_CONTROL_WORK *sv_control_ptr;		///<�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
	PLAYER_WORK playerWork[PLAYER_MAX];
	EVENTDATA_SYSTEM * evdata;
	LOCATION *start_loc;
	LOCATION *entrance_loc;
	LOCATION *special_loc;
	MYITEM_PTR myitem;			///<�莝���A�C�e���Z�[�u�f�[�^�ւ̃|�C���^
	POKEPARTY *my_pokeparty;	///<�莝���|�P�����Z�[�u�f�[�^�ւ̃|�C���^
	FLDMMDLSYS *fldmmdlsys;
	EVENTWORK *eventwork;
	u8 season_id;				///<�G�ߎw��ID
};

//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
static void GAMEDATA_SaveDataLoad(GAMEDATA *gamedata);
static void GAMEDATA_SaveDataUpdate(GAMEDATA *gamedata);


//============================================================================================
//
//
//		GAMEDATA�S��
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
GAMEDATA * GAMEDATA_Create(HEAPID heapID)
{
	int i;
	GAMEDATA * gd;
	SITUATION *st;
	
	gd = GFL_HEAP_AllocMemory(heapID, sizeof(GAMEDATA));
	GFL_STD_MemClear(gd, sizeof(GAMEDATA));
	
	gd->sv_control_ptr = SaveControl_GetPointer();
	
	//�G�ߐݒ�F�Q�[�����ł�1�������P�V�[�Y��
	{
		RTCDate date;
		GFL_RTC_GetDate(&date);
		gd->season_id = date.month % PMSEASON_TOTAL;
	}

	//�󋵃f�[�^
	st = SaveData_GetSituation(gd->sv_control_ptr);
	gd->start_loc = Situation_GetStartLocation(st);
	gd->entrance_loc = Situation_GetStartLocation(st);
	gd->special_loc = Situation_GetStartLocation(st);
	
	//�v���C���[���[�N
	for (i = 0; i < PLAYER_MAX; i++) {
		PLAYERWORK_init(&gd->playerWork[i]);
	}
	//�������g�̃v���C���[���[�N�̃}�C�X�e�[�^�X�ɃZ�[�u�f�[�^�̃}�C�X�e�[�^�X���R�s�[���Ă���
	//�u�ŏ�����v�Ŏn�߂��Ƃ��ł������ɗ���܂łɖ��O���͂�����Ă���̂�
	//�Z�[�u�̗L���Ɋ֌W�Ȃ��R�s�[
	GFL_STD_MemCopy(SaveData_GetMyStatus(gd->sv_control_ptr), 
		GAMEDATA_GetMyStatus(gd), MyStatus_GetWorkSize());
	
	//�C�x���g�f�[�^
	gd->evdata = EVENTDATA_SYS_Create(heapID);
	EVENTDATA_SYS_Clear(gd->evdata);
	
	//�C�x���g���[�N
	gd->eventwork = EVENTWORK_AllocWork( heapID );
	EVENTWORK_Init( gd->eventwork );

	//���샂�f��
	gd->fldmmdlsys = FLDMMDLSYS_CreateSystem( heapID, FLDMMDL_MDL_MAX );
	
	gd->myitem = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYITEM);
	gd->my_pokeparty = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYPOKE);
	
	if(SaveControl_NewDataFlagGet(gd->sv_control_ptr) == FALSE){
	}
	
	GAMEDATA_SaveDataLoad(gd);	//�Z�[�u�f�[�^����K�v�Ȃ��̂����[�h���Ă���
	return gd;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_Delete(GAMEDATA * gamedata)
{
	FLDMMDLSYS_FreeSystem(gamedata->fldmmdlsys);
	EVENTWORK_FreeWork(gamedata->eventwork);
	EVENTDATA_SYS_Delete(gamedata->evdata);
	GFL_HEAP_FreeMemory(gamedata);
}
//------------------------------------------------------------------
//------------------------------------------------------------------
PLAYER_WORK * GAMEDATA_GetPlayerWork(GAMEDATA * gamedata, u32 player_id)
{
	GF_ASSERT(player_id < PLAYER_MAX);
	return &gamedata->playerWork[player_id];
}

//------------------------------------------------------------------
//------------------------------------------------------------------
PLAYER_WORK * GAMEDATA_GetMyPlayerWork(GAMEDATA * gamedata)
{
	return &gamedata->playerWork[0];
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^���擾����
 * @param	gamedata			GAMEDATA�ւ̃|�C���^
 * @return	EVENTDATA_SYSTEM	�C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^
 */
//------------------------------------------------------------------
EVENTDATA_SYSTEM * GAMEDATA_GetEventData(GAMEDATA * gamedata)
{
	return gamedata->evdata;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
const LOCATION * GAMEDATA_GetStartLocation(const GAMEDATA * gamedata)
{
	return gamedata->start_loc;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_SetStartLocation(GAMEDATA * gamedata, const LOCATION * loc)
{
	*(gamedata->start_loc) = *loc;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const LOCATION * GAMEDATA_GetEntranceLocation(const GAMEDATA * gamedata)
{
	return gamedata->entrance_loc;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_SetEntranceLocation(GAMEDATA * gamedata, const LOCATION * loc)
{
	*(gamedata->entrance_loc) = *loc;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const LOCATION * GAMEDATA_GetSpecialLocation(const GAMEDATA * gamedata)
{
	return gamedata->special_loc;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_SetSpecialLocation(GAMEDATA * gamedata, const LOCATION * loc)
{
	*(gamedata->special_loc) = *loc;
}

//--------------------------------------------------------------
/**
 * @brief   �莝���A�C�e���ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �莝���A�C�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
MYITEM_PTR GAMEDATA_GetMyItem(const GAMEDATA * gamedata)
{
	return gamedata->myitem;
}

//--------------------------------------------------------------
/**
 * @brief   �莝���|�P�����ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �莝���|�P�����ւ̃|�C���^
 */
//--------------------------------------------------------------
POKEPARTY * GAMEDATA_GetMyPokemon(const GAMEDATA * gamedata)
{
	return gamedata->my_pokeparty;
}

//--------------------------------------------------------------
/**
 * @brief   MYSTATUS�ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
MYSTATUS * GAMEDATA_GetMyStatus(GAMEDATA * gamedata)
{
	return &((GAMEDATA_GetMyPlayerWork(gamedata))->mystatus);
}

//--------------------------------------------------------------
/**
 * @brief	FLDMMDLSYS�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @retval	FLDMMDLSYS�ւ̃|�C���^
 */
//--------------------------------------------------------------
FLDMMDLSYS * GAMEDATA_GetFldMMdlSys(GAMEDATA *gamedata)
{
	return gamedata->fldmmdlsys;
}

//--------------------------------------------------------------
/**
 * @brief	EVENTWORK�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @retval	EVENTWORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata)
{
	return gamedata->eventwork;
}

//--------------------------------------------------------------
/**
 * @brief	�G�߂̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	u8	�G��ID�igamesystem/pm_season.h�Q�Ɓj
 */
//--------------------------------------------------------------
u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata)
{
	return gamedata->season_id;
}

//============================================================================================
//
//
//	PLAYER_WORK�p�֐�
//	��player_work.c���쐬���Ĉړ������邩��
//
//
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

//------------------------------------------------------------------
//------------------------------------------------------------------
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

//------------------------------------------------------------------
//------------------------------------------------------------------
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

//------------------------------------------------------------------
//------------------------------------------------------------------
u16 PLAYERWORK_getDirection(const PLAYER_WORK * player)
{
	return player->direction;
}

//==============================================================================
//	�Q�[���f�[�^�����������ɃZ�[�u
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^���[�N����Q�[���f�[�^�����������[�h����
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void GAMEDATA_SaveDataLoad(GAMEDATA *gamedata)
{
	if(SaveControl_NewDataFlagGet(gamedata->sv_control_ptr) == TRUE){
		return;	//�Z�[�u�f�[�^�������̂ŉ����ǂݍ��܂Ȃ�
	}
	
	{	//PLAYER_WORK
		PLAYER_WORK *pw;
		pw = GAMEDATA_GetMyPlayerWork(gamedata);
		SaveData_PlayerDataLoad(gamedata->sv_control_ptr, pw);
		SaveData_SituationDataLoad(gamedata->sv_control_ptr, pw);
	}
	
	{	//FLDMMDL
		FLDMMDLSYS *fldmmdlsys = GAMEDATA_GetFldMMdlSys(gamedata);
		FLDMMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_FLDMMDL);
		FLDMMDL_SAVEDATA_Load( fldmmdlsys, pw );
	}
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�f�[�^�̃��[�N���X�V����
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * �Z�[�u����f�[�^���A�Z�[�u���[�N���璼�ڎg�p�����ɁA�ʃ��[�N�ɃR�s�[���Ďg�p���Ă�����̂��Ώ�
 */
//--------------------------------------------------------------
static void GAMEDATA_SaveDataUpdate(GAMEDATA *gamedata)
{
	{	//PLAYER_WORK
		PLAYER_WORK *pw = GAMEDATA_GetMyPlayerWork(gamedata);
		SaveData_PlayerDataUpdate(gamedata->sv_control_ptr, pw);
		SaveData_SituationDataUpdate(gamedata->sv_control_ptr, pw);
	}

	{	//FLDMMDL
		FLDMMDLSYS *fldmmdlsys = GAMEDATA_GetFldMMdlSys(gamedata);
		FLDMMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_FLDMMDL);
		FLDMMDL_SAVEDATA_Save( fldmmdlsys, pw );
	}
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�����s
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata)
{
	//�Z�[�u���[�N�̏����X�V
	GAMEDATA_SaveDataUpdate(gamedata);
	
	//�Z�[�u���s
	return SaveControl_Save(gamedata->sv_control_ptr);
}
