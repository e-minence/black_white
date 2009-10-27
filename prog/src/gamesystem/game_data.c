//=============================================================================
/**
 * @file	game_data.c
 * @brief	�Q�[���i�s�f�[�^�Ǘ�
 * @author	tamada
 * @date	2008.11.04
 *
 * @note
 * ��{�I�ɃQ�[���i�s�f�[�^�̓Z�[�u�f�[�^�Ƃ��đ��݂��邪�A
 * �ʐM���̕����v���C���[����t�B�[���h�}�b�v���ȂǃZ�[�u�f�[�^��
 * �̂�Ƃ͌���Ȃ���������B
 * ���̂悤�ȃZ�[�u�f�[�^�E��Z�[�u�f�[�^�֕���ɃA�N�Z�X����C���^�[�t�F�C�X��
 * �e�p�[�g���Ƃɍ\������ƃp�[�g���ƂɃA�N�Z�X��@���������A�܂������@�\��
 * ���̂��������݂���Ȃǃv���O�����S�̂̕��G���������Ă��܂��\���������B
 * ���������邽�߁A���ʃC���^�[�t�F�C�X����邱�Ƃɂ���Ċȗ�������B
 */
//=============================================================================

#include <gflib.h>

#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "poke_tool/pokeparty.h"
#include "savedata/mystatus.h"
#include "savedata/situation.h"
#include "savedata/player_data.h"
#include "gamesystem/pm_season.h"		//�G�ߒ�`�Q��
#include "field/field_subscreen.h" //FIELD_SUBSCREEN_ACTION
#include "field/rail_location.h"    //RAIL_LOCATION
#include "field/field_rail_loader.h"    //FIELD_RAIL_LOADER
#include "sound/bgm_info.h"
#include "field/map_matrix.h"     //MAP_MATRIX
#include "field/field_status.h"   //FIELD_STATUS
#include "field/field_encount.h"
#include "field/field_dir.h"
#include "savedata/shortcut.h"		//SHORTCUT_SetRegister

#include "field/fldmmdl.h"      //MMDLSYS
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
  LOCATION *special_loc;    ///<����ڑ���ʒu
  LOCATION *escape_loc;     ///<�E�o��ʒu

  BAG_CURSOR* bagcursor;  ///< �o�b�O�J�[�\���̊Ǘ��\���̃|�C���^
  MYITEM_PTR myitem;			///<�莝���A�C�e���Z�[�u�f�[�^�ւ̃|�C���^
  POKEPARTY *my_pokeparty;	///<�莝���|�P�����Z�[�u�f�[�^�ւ̃|�C���^
	CONFIG		*config;				///<�R���t�B�O�Z�[�u�f�[�^�ւ̃|�C���^
  BOX_MANAGER *boxMng;      ///<�{�b�N�X�Ǘ��\���̂ւ̃|�C���^

  BGM_INFO_SYS* bgm_info_sys;	// BGM���擾�V�X�e��

  FIELD_RAIL_LOADER * railLoader;   ///<�t�B�[���h���[���f�[�^�Ǘ��ւ̃|�C���^
  MMDLSYS *mmdlsys;
  EVENTWORK *eventwork;
  MAP_MATRIX * mapMatrix;     ///<�}�b�v�}�g���b�N�X�Ǘ�
  
  FIELD_SOUND *field_sound; ///<�t�B�[���h�T�E���h�Ǘ�

  FIELD_STATUS * fldstatus;  ///<�t�B�[���h�}�b�v���

  u32 last_battle_result;   ///<�ŐV�̃o�g���̌���

  int fieldmap_walk_count; ///<�t�B�[���h�}�b�v�����J�E���g
  u8 season_id;				///<�G�ߎw��ID
  u8 subscreen_mode; ///< �t�B�[���h����ʂ̏��
  u8 subscreen_type;
  u8 frameSpritcount;    ///< �t���[����������œ��삷��ꍇ�̃J�E���g
  u8 frameSpritEnable;   ///< �t���[����������œ��삷��ꍇ�̋���

  u8 intrude_num;         ///<�N�����Ă��鎞�̐ڑ��l��
  u8 intrude_my_id;       ///<�N�����Ă��鎩����NetID
  OCCUPY_INFO occupy[OCCUPY_ID_MAX];    ///<�苒���
  
  FIELD_BEACON_MSG_DATA *fbmData; //�t�B�[���h�r�[�R�����b�Z�[�W�f�[�^

  ENCOUNT_WORK* enc_work; ///<�G���J�E���g�֘A�f�[�^���[�N
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

  gd = GFL_HEAP_AllocClearMemory(heapID, sizeof(GAMEDATA));

  gd->sv_control_ptr = SaveControl_GetPointer();

  //�G�ߐݒ�F�Q�[�����ł�1�������P�V�[�Y��
  {
    RTCDate date;
    GFL_RTC_GetDate(&date);
    gd->season_id = PMSEASON_SPRING;
    //gd->season_id = date.month % PMSEASON_TOTAL;
  }

  //
  gd->subscreen_mode = FIELD_SUBSCREEN_NORMAL;

  //�󋵃f�[�^
  st = SaveData_GetSituation(gd->sv_control_ptr);
  gd->start_loc = Situation_GetStartLocation(st);
  gd->entrance_loc = Situation_GetStartLocation(st);
  gd->special_loc = Situation_GetStartLocation(st);
  gd->escape_loc = Situation_GetEscapeLocation(st);

  // BGM���擾�V�X�e�����쐬(BGM����ǂݍ���)
  gd->bgm_info_sys = BGM_INFO_CreateSystem( heapID );

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
  gd->eventwork = SaveControl_DataPtrGet( gd->sv_control_ptr, GMDATA_ID_EVENT_WORK );
  
  //�}�b�v�}�g���b�N�X
  gd->mapMatrix = MAP_MATRIX_Create( heapID );

	// rail�f�[�^�ǂݍ��݃V�X�e��
  gd->railLoader = FIELD_RAIL_LOADER_Create( heapID );
  
  //���샂�f��
  gd->mmdlsys = MMDLSYS_CreateSystem( heapID, MMDL_MDL_MAX, SaveControl_DataPtrGet(gd->sv_control_ptr,GMDATA_ID_ROCKPOS) );
  
  //�t�B�[���h�T�E���h�Ǘ�
  gd->field_sound = FIELD_SOUND_Create( heapID );

  gd->fldstatus = FIELD_STATUS_Create( heapID );

  gd->bagcursor = MYITEM_BagCursorAlloc( heapID );
  gd->myitem = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYITEM);
  gd->my_pokeparty = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYPOKE);
	gd->config	= SaveData_GetConfig( gd->sv_control_ptr );
  gd->boxMng	= BOX_DAT_InitManager( heapID , gd->sv_control_ptr );

  //�苒���
  for(i = 0; i < OCCUPY_ID_MAX; i++){
    OccupyInfo_WorkInit(&gd->occupy[i]);
  }
  
  //�����J�E���g
  gd->fieldmap_walk_count = 0;

  //��ɃR�R�̎��_�Ŗ��O�������Ă���Ə����Ă������̂ŁA�������Ɏg�p�B
  gd->fbmData = FIELD_BEACON_MSG_CreateData( heapID , GAMEDATA_GetMyStatus(gd) );

  if(SaveControl_NewDataFlagGet(gd->sv_control_ptr) == FALSE){
  }
  
  //�G���J�E���g���[�N������
  gd->enc_work = ENCOUNT_WORK_Create( heapID );

  GAMEDATA_SaveDataLoad(gd);	//�Z�[�u�f�[�^����K�v�Ȃ��̂����[�h���Ă���
  return gd;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_Delete(GAMEDATA * gamedata)
{
  ENCOUNT_WORK_Delete( gamedata->enc_work );
  FIELD_BEACON_MSG_DeleteData( gamedata->fbmData );
	BGM_INFO_DeleteSystem(gamedata->bgm_info_sys);
  BOX_DAT_ExitManager( gamedata->boxMng );
  GFL_HEAP_FreeMemory(gamedata->bagcursor);
  MMDLSYS_FreeSystem(gamedata->mmdlsys);
	FIELD_RAIL_LOADER_Delete( gamedata->railLoader );
  MAP_MATRIX_Delete( gamedata->mapMatrix );
  EVENTDATA_SYS_Delete(gamedata->evdata);
  FIELD_STATUS_Delete( gamedata->fldstatus );
  FIELD_SOUND_Delete( gamedata->field_sound );
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
  return &gamedata->playerWork[PLAYER_ID_MINE];
}

//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFO�ւ̃|�C���^�擾(�v���C���[ID�w��)
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   player_id   �v���C���[ID
 * @retval  OCCUPY_INFO�ւ̃|�C���^
 */
//--------------------------------------------------------------
OCCUPY_INFO * GAMEDATA_GetOccupyInfo(GAMEDATA * gamedata, u32 player_id)
{
  GF_ASSERT_MSG(player_id < OCCUPY_ID_MAX, "player_id = %d\n", player_id);
  return &gamedata->occupy[player_id];
}

//--------------------------------------------------------------
/**
 * @brief   OCCUPY_INFO�ւ̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  OCCUPY_INFO�ւ̃|�C���^
 */
//--------------------------------------------------------------
OCCUPY_INFO * GAMEDATA_GetMyOccupyInfo(GAMEDATA * gamedata)
{
  return GAMEDATA_GetOccupyInfo(gamedata, OCCUPY_ID_MINE);
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
//------------------------------------------------------------------
//------------------------------------------------------------------
const LOCATION * GAMEDATA_GetEscapeLocation(const GAMEDATA * gamedata)
{
  return gamedata->escape_loc;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_SetEscapeLocation(GAMEDATA * gamedata, const LOCATION * loc)
{
  *(gamedata->escape_loc) = *loc;
}

//--------------------------------------------------------------
/**
 * @brief ���[�v��ѐ�ID�̃Z�b�g
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @param warp_id   ���[�v��ѐ�ID�w��
 */
//--------------------------------------------------------------
void GAMEDATA_SetWarpID(GAMEDATA * gamedata, u16 warp_id)
{
  u16 * warp = Situation_GetWarpID( SaveData_GetSituation(gamedata->sv_control_ptr) );
  *warp = warp_id;
}

//--------------------------------------------------------------
/**
 * @brief  ���[�v��ѐ�ID�̎擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @return  u16 ���[�v��ѐ�ID
 */
//--------------------------------------------------------------
u16 GAMEDATA_GetWarpID(GAMEDATA * gamedata)
{
  u16 * warp = Situation_GetWarpID( SaveData_GetSituation(gamedata->sv_control_ptr) );
  return *warp;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�J�[�\���̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �o�b�O�J�[�\���|�C���^
 */
//--------------------------------------------------------------
BAG_CURSOR* GAMEDATA_GetBagCursor(const GAMEDATA * gamedata)
{
  return gamedata->bagcursor;
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
 * @brief   �{�b�N�X�Ǘ��\���̂̃|�C���^�擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @retval  �{�b�N�X�Ǘ��\���̂ւ̃|�C���^
 */
//--------------------------------------------------------------
BOX_MANAGER * GAMEDATA_GetBoxManager(const GAMEDATA * gamedata)
{
  return gamedata->boxMng;
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
 * @brief   MYSTATUS�ւ̃|�C���^�擾(�v���C���[ID�w��)
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @param   player_id   �v���C���[ID
 * @retval  MYSTATUS�ւ̃|�C���^
 */
//--------------------------------------------------------------
MYSTATUS * GAMEDATA_GetMyStatusPlayer(GAMEDATA * gamedata, u32 player_id)
{
  GF_ASSERT(player_id < PLAYER_MAX);
  return &gamedata->playerWork[player_id].mystatus;
}

//--------------------------------------------------------------
/**
 * @brief FIELD_RAIL_LOADER�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @return  FIELD_RAIL_LOADER ���[���f�[�^���[�_�[�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
FIELD_RAIL_LOADER * GAMEDATA_GetFieldRailLoader(GAMEDATA * gamedata)
{
  return gamedata->railLoader;
}

//--------------------------------------------------------------
/**
 * @brief BGM_INFO_SYS�ւ̃|�C���^�擾
 * @param gamedata    GAMEDATA�ւ̃|�C���^
 * preturn BGM_INFO_SYS BGM���擾�V�X�e���ւ̃|�C���^
 */
//--------------------------------------------------------------
extern BGM_INFO_SYS * GAMEDATA_GetBGMInfoSys(GAMEDATA * gamedata)
{
	return gamedata->bgm_info_sys;
}

//--------------------------------------------------------------
/**
 * @brief	MMDLSYS�ւ̃|�C���^�擾
 * @param	gamedata	GAMEDATA�ւ̃|�C���^
 * @retval	MMDLSYS�ւ̃|�C���^
 */
//--------------------------------------------------------------
MMDLSYS * GAMEDATA_GetMMdlSys(GAMEDATA *gamedata)
{
  return gamedata->mmdlsys;
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
 * @brief   MAP_MATRIX�ւ̃|�C���^�擾
 * @param	  gamedata	GAMEDATA�ւ̃|�C���^
 * @retval  MAP_MATRIX�ւ̃|�C���^
 */
//--------------------------------------------------------------
MAP_MATRIX * GAMEDATA_GetMapMatrix(GAMEDATA * gamedata)
{
  return gamedata->mapMatrix;
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

//--------------------------------------------------------------
/**
 * @brief	�G�߂̃Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	season_id	�G��ID�igamesystem/pm_season.h�Q�Ɓj
 *
 * ���ۂ̋@�\�Ƃ��ĕK�v���ǂ����͋^�₾���f�o�b�O�p�Ƃ��Ēǉ�
 */
//--------------------------------------------------------------
void GAMEDATA_SetSeasonID(GAMEDATA *gamedata, u8 season_id)
{
  GF_ASSERT(season_id < PMSEASON_TOTAL);
  gamedata->season_id = season_id;
}

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʂ̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	subscreen_mode
 */
//--------------------------------------------------------------
u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata)
{
  return gamedata->subscreen_mode;
}

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʂ̐ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  subscreen_mode
 */
//--------------------------------------------------------------
void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode)
{
  GF_ASSERT(subscreen_mode < FIELD_SUBSCREEN_MODE_MAX);
  gamedata->subscreen_mode = subscreen_mode;
}
//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʃ^�C�v�̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return	subscreen_type
 */
//--------------------------------------------------------------
const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata)
{
  return gamedata->subscreen_type;
}

//--------------------------------------------------------------
/**
 * @brief	  �t�B�[���h����ʎ�ނ̐ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  subscreen_type
 */
//--------------------------------------------------------------
void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type)
{
  gamedata->subscreen_type = subscreen_type;
}

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̋��t���O�ݒ�
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param	  bEnable ��������ꍇTRUE
 */
//------------------------------------------------------------------
void GAMEDATA_SetFrameSpritEnable(GAMEDATA *gamedata,BOOL bEnable)
{
  gamedata->frameSpritEnable = bEnable;
  if(bEnable){
    gamedata->frameSpritcount = 0;    ///< �t���[����������œ��삷��ꍇ�̃J�E���g
  }
}

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̃J�E���^���Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata)
{
  gamedata->frameSpritcount = 0;    ///< �t���[����������œ��삷��ꍇ�̃J�E���g
}

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삷��ꍇ�̃t���[��
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
u8 GAMEDATA_GetAndAddFrameSpritCount(GAMEDATA *gamedata)
{
  if(gamedata->frameSpritEnable){
    u8 ret = gamedata->frameSpritcount;
    gamedata->frameSpritcount++;
    return ret;
  }
  return 0;
}

//------------------------------------------------------------------
/**
 * @brief	  �t���[����������œ��삵�Ă��邩�ǂ���
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  ���삵�Ă���ꍇTRUE
 */
//------------------------------------------------------------------
BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata)
{
  return gamedata->frameSpritEnable;
}

//------------------------------------------------------------------
/**
 * @brief	  �t�B�[���h�}�b�v�����J�E���g�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  int �����J�E���g
 */
//------------------------------------------------------------------
int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata)
{
  return gamedata->fieldmap_walk_count;
}

//------------------------------------------------------------------
/**
 * @brief	  �t�B�[���h�}�b�v�����J�E���g�Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param   count �Z�b�g����J�E���g
 * @return  nothing
 */
//------------------------------------------------------------------
void GAMEDATA_SetFieldMapWalkCount(GAMEDATA *gamedata,int count)
{
  gamedata->fieldmap_walk_count = count;
}

//--------------------------------------------------------------
/**
 * @brief FIELD_SOUND�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  FIELD_SOUND*
 */
//--------------------------------------------------------------
FIELD_SOUND * GAMEDATA_GetFieldSound( GAMEDATA *gamedata )
{
  return( gamedata->field_sound );
}

//--------------------------------------------------------------
/**
 * @brief   ENCOUNT_WORK�擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  ENCOUNT_WORK*
 */
//--------------------------------------------------------------
ENCOUNT_WORK* GAMEDATA_GetEncountWork( GAMEDATA *gamedata )
{
  return( gamedata->enc_work );
}
//--------------------------------------------------------------
/**
 * @brief �ŐV�̐퓬���ʂ��擾
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @return  u32 �ŐV�̐퓬���ʁiinclude/battle/battle.h��BtlResult�j
 */
//--------------------------------------------------------------
u32 GAMEDATA_GetLastBattleResult( const GAMEDATA * gamedata )
{
  return gamedata->last_battle_result;
}

//--------------------------------------------------------------
/**
 * @brief �ŐV�̐퓬���ʂ��Z�b�g
 * @param   gamedata	GAMEDATA�ւ̃|�C���^
 * @param btl_result  �퓬���ʁiinclude/battle/battle.h��BtlResult�j
 */
//--------------------------------------------------------------
void GAMEDATA_SetLastBattleResult( GAMEDATA * gamedata, u32 btl_result )
{
  gamedata->last_battle_result = btl_result;
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
  player->pos_type = 0;
  player->mystatus.id = 0;
  player->mystatus.sex = 0;
  player->mystatus.region_code = 0;
  player->railposition.type       = 0;
  player->railposition.key        = 0;
  player->railposition.rail_index = 0;
  player->railposition.line_grid  = 0;
  player->railposition.width_grid = 0;
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

//--------------------------------------------------------------
//--------------------------------------------------------------
void PLAYERWORK_setRailPosition(PLAYER_WORK * player, const RAIL_LOCATION * railpos)
{
  player->railposition = *railpos;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
const RAIL_LOCATION * PLAYERWORK_getRailPosition(const PLAYER_WORK * player)
{
  return &player->railposition;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void PLAYERWORK_setPosType(PLAYER_WORK * player, LOCATION_POS_TYPE pos_type)
{
  player->pos_type = pos_type;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
LOCATION_POS_TYPE PLAYERWORK_getPosType(const PLAYER_WORK * player)
{
  return player->pos_type;
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

//--------------------------------------------------------------
/**
 * @brief	�����^�C�v�ŕ����p�x��ݒ肷��
 * @param	player		PLAYER_WORK�ւ̃|�C���^
 * @param	dir_type	�������w�肷��l(DIR_�`)
 */
//--------------------------------------------------------------
void PLAYERWORK_setDirection_Type(PLAYER_WORK * player, u16 dir_type)
{
  static const u16 DIR_ROT[DIR_MAX4] = 
  {
    0,
    0x8000,
    0x4000,
    0xC000,
  };
  
  GF_ASSERT( dir_type < DIR_MAX4 );

  player->direction = DIR_ROT[ dir_type ];
}

//--------------------------------------------------------------
/**
 * @brief	�����p�x�̕����^�C�v���擾����
 * @param	player	PLAYER_WORK�ւ̃|�C���^
 * @return	�������w�肷��l(DIR_�`)
 */
//--------------------------------------------------------------
u16 PLAYERWORK_getDirection_Type(const PLAYER_WORK * player)
{
  u16 dir = DIR_UP;
  
	if( (player->direction>0x2000) && (player->direction<0x6000) ){
		dir = DIR_LEFT;
	}else if( (player->direction >= 0x6000) && (player->direction <= 0xa000) ){
		dir = DIR_DOWN;
	}else if( (player->direction > 0xa000)&&(player->direction < 0xe000) ){
		dir = DIR_RIGHT;
	}
	return( dir );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
//��check�@�p���X�e�X�g�ׂ̈̎b��֐�
void PLAYERWORK_setPalaceArea(PLAYER_WORK * player, u8 palace_area)
{
  player->palace_area = palace_area;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
//��check�@�p���X�e�X�g�ׂ̈̎b��֐�
u8 PLAYERWORK_getPalaceArea(const PLAYER_WORK * player)
{
  return player->palace_area;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
PLAYER_MOVE_FORM PLAYERWORK_GetMoveForm( const PLAYER_WORK *player )
{
  return player->move_form;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void PLAYERWORK_SetMoveForm( PLAYER_WORK *player, PLAYER_MOVE_FORM form )
{
  player->move_form = form;
}

//==================================================================
/**
 * @brief   FIELD_STATUS�̎擾
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 * @return  FIELD_STATUS
 */
//==================================================================
FIELD_STATUS * GAMEDATA_GetFieldStatus(GAMEDATA * gamedata)
{
  return gamedata->fldstatus;
}

//==================================================================
/**
 * �N���ڑ��l���擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  int		�ڑ��l��
 */
//==================================================================
int GAMEDATA_GetIntrudeNum(const GAMEDATA *gamedata)
{
  return gamedata->intrude_num;
}

//==================================================================
/**
 * �N�����̎�����NetID���擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  int		������NetID
 */
//==================================================================
int GAMEDATA_GetIntrudeMyID(const GAMEDATA *gamedata)
{
  return gamedata->intrude_my_id;
}

//==================================================================
/**
 * �N�����̎�����NetID���Z�b�g
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  int		������NetID
 */
//==================================================================
void GAMEDATA_SetIntrudeMyID(GAMEDATA *gamedata, int intrude_my_id)
{
  gamedata->intrude_my_id = intrude_my_id;
  OS_TPrintf("�����̐N��MyNetID=%d\n", intrude_my_id);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�펞�ʐM���[�h�t���O�擾
 *
 *	@param	gamedata		GAMEDATA�ւ̃|�C���^
 *	
 *	@return	TRUE�펞�ʐM���[�hON FALSE�펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
BOOL GAMEDATA_GetAlwaysNetFlag( const GAMEDATA *gamedata )
{	
	return CONFIG_GetNetworkSearchMode( gamedata->config ) == NETWORK_SEARCH_ON;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�펞�ʐM���[�h�t���O�ݒ�
 *
 *	@param	gamedata		GAMEDATA�ւ̃|�C���^
 *	@param	is_on				TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
void GAMEDATA_SetAlwaysNetFlag( GAMEDATA *gamedata, BOOL is_on )
{	
	NETWORK_SEARCH_MODE mode	= is_on? NETWORK_SEARCH_ON: NETWORK_SEARCH_OFF;
	CONFIG_SetNetworkSearchMode( gamedata->config, mode );
}

//==================================================================
/**
 * �t�B�[���h�r�[�R�����b�Z�[�W�f�[�^�擾
 *
 * @param   gamedata		GAMEDATA�ւ̃|�C���^
 *
 * @retval  
 */
//==================================================================
FIELD_BEACON_MSG_DATA* GAMEDATA_GetFieldBeaconMessageData( GAMEDATA *gamedata )
{
  return gamedata->fbmData;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Y�{�^���o�^�ݒ�
 *
 *  @param  gamedata		GAMEDATA�ւ̃|�C���^
 *  @param	shortcutID	Y�{�^���o�^����ID
 *  @param	is_on				TRUE�Ȃ��Y�{�^���o�^	FALSE�Ȃ�Ή���
 */
//----------------------------------------------------------------------------
void GAMEDATA_SetShortCut( GAMEDATA *gamedata, SHORTCUT_ID shortcutID, BOOL is_on )
{	
	SAVE_CONTROL_WORK*	p_sv	= GAMEDATA_GetSaveControlWork(gamedata);

	SHORTCUT *p_shortcut	=  SaveData_GetShortCut( p_sv );

	SHORTCUT_SetRegister( p_shortcut, shortcutID, is_on );	
}

//----------------------------------------------------------------------------
/**
 *	@brief	Y�{�^���o�^�ݒ�
 *
 *  @param  gamedata		GAMEDATA�ւ̃|�C���^
 *  @param	shortcutID	Y�{�^���o�^����ID
 *  @retval	TRUE�Ȃ��YBTN�ɓo�^���Ă���	FALSE�Ȃ�Γo�^���Ă��Ȃ�
 */
//----------------------------------------------------------------------------
extern BOOL GAMEDATA_GetShortCut( const GAMEDATA *gamedata, SHORTCUT_ID shortcutID )
{	
	const SAVE_CONTROL_WORK*	cp_sv	= GAMEDATA_GetSaveControlWorkConst(gamedata);

	const SHORTCUT *cp_shortcut	=  SaveData_GetShortCutConst( cp_sv );

	return SHORTCUT_GetRegister( cp_shortcut, shortcutID );	
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

  {	//MMDL
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys(gamedata);
    MMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_MMDL);
    MMDL_SAVEDATA_Load( mmdlsys, pw );
  }
  
  { //OCCUPY_INFO
    OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
    SaveData_OccupyInfoLoad(gamedata->sv_control_ptr, occupy);
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

  {	//MMDL
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys(gamedata);
    MMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_MMDL);
    MMDL_SAVEDATA_Save( mmdlsys, pw );
  }
  
  { //OCCUPY_INFO
    OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
    SaveData_OccupyInfoUpdate(gamedata->sv_control_ptr, occupy);
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

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u���J�n
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
void GAMEDATA_SaveAsyncStart(GAMEDATA *gamedata)
{
  //�Z�[�u���[�N�̏����X�V
  GAMEDATA_SaveDataUpdate(gamedata);

  //�Z�[�u�J�n
  SaveControl_SaveAsyncInit(gamedata->sv_control_ptr);
}


//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u�����s
 *
 * @param   gamedata		�Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT GAMEDATA_SaveAsyncMain(GAMEDATA *gamedata)
{
  return SaveControl_SaveAsyncMain(gamedata->sv_control_ptr);
}

//--------------------------------------------------------------
/**
 * @brief	  �Z�[�u�R���g���[�����[�N���擾����
 * @param	  gamedata			GAMEDATA�ւ̃|�C���^
 * @return	SAVE_CONTROL_WORK	�̃|�C���^
 */
//--------------------------------------------------------------

SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata)
{
  return gamedata->sv_control_ptr;
}

//--------------------------------------------------------------
/**
 * @brief	  �Z�[�u�R���g���[�����[�N���擾����
 * @param	  gamedata			GAMEDATA�ւ̃|�C���^
 * @return	SAVE_CONTROL_WORK	�̃|�C���^
 */
//--------------------------------------------------------------
const SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWorkConst(const GAMEDATA * gamedata)
{
  return gamedata->sv_control_ptr;
}
