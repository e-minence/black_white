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

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���f�[�^���e��`
 */
//------------------------------------------------------------------
struct _GAMEDATA{
	PLAYER_WORK playerWork[PLAYER_MAX];
	EVENTDATA_SYSTEM * evdata;
};

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
	
	gd = GFL_HEAP_AllocMemory(heapID, sizeof(GAMEDATA));

	GFL_STD_MemClear(gd, sizeof(GAMEDATA));
	for (i = 0; i < PLAYER_MAX; i++) {
		PLAYERWORK_init(&gd->playerWork[i]);
	}
	gd->evdata = EVENTDATA_SYS_Create(heapID);
	EVENTDATA_SYS_Clear(gd->evdata);

	return gd;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_Delete(GAMEDATA * gamedata)
{
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

