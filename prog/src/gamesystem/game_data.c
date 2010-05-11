//=============================================================================
/**
 * @file  game_data.c
 * @brief �Q�[���i�s�f�[�^�Ǘ�
 * @author  tamada
 * @date  2008.11.04
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
#include "system/main.h"

#include "field/eventdata_system.h"
#include "savedata/save_control.h"
#include "savedata/save_tbl.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "poke_tool/pokeparty.h"
#include "savedata/mystatus.h"
#include "savedata/situation.h"
#include "savedata/player_data.h"
#include "gamesystem/pm_season.h"   //�G�ߒ�`�Q��
#include "gamesystem/pm_season_local.h"   //
#include "field/field_subscreen.h" //FIELD_SUBSCREEN_ACTION
#include "field/rail_location.h"    //RAIL_LOCATION
#include "sound/bgm_info.h"
#include "field/map_matrix.h"     //MAP_MATRIX
#include "field/field_status.h"   //FIELD_STATUS
#include "field/field_encount.h"
#include "field/field_dir.h"
#include "field/field_wfbc_data.h"  // WF�EBC
#include "savedata/intrude_save.h"  //
#include "savedata/randommap_save.h"  //WF�EBC
#include "savedata/shortcut.h"    //SHORTCUT_SetRegister
#include "savedata/wifilist.h"
#include "savedata/playtime.h"
#include "gamesystem/comm_player_support.h"
#include "gamesystem/pm_weather.h"
#include "net/wih_dwc.h"
#include "sound/pm_sndsys.h"

#include "field/fldmmdl.h"      //MMDLSYS
#include "gamesystem/beacon_status.h"
#include "savedata/gimmickwork_local.h"
#include "savedata/gimmickwork.h"
#include "savedata/un_savedata_local.h"
#include "savedata/un_savedata.h"
#include "savedata/bsubway_savedata.h"
#include "savedata/tradepoke_after_save.h"

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �Q�[���f�[�^���e��`
 */
//------------------------------------------------------------------
struct _GAMEDATA{
  SAVE_CONTROL_WORK *sv_control_ptr;    ///<�Z�[�u�f�[�^�Ǘ����[�N�ւ̃|�C���^
  PLAYER_WORK playerWork[PLAYER_MAX];
  EVENTDATA_SYSTEM * evdata;
  SITUATION * situation;
  LOCATION *start_loc;
  LOCATION *entrance_loc;
  LOCATION *special_loc;    ///<����ڑ���ʒu
  LOCATION *escape_loc;     ///<�E�o��ʒu
  LOCATION palace_return_loc; ///<�p���X����̖߂��ʒu

  BAG_CURSOR* bagcursor;  ///< �o�b�O�J�[�\���̊Ǘ��\���̃|�C���^
  MYITEM_PTR myitem;      ///<�莝���A�C�e���Z�[�u�f�[�^�ւ̃|�C���^
  POKEPARTY *my_pokeparty;  ///<�莝���|�P�����Z�[�u�f�[�^�ւ̃|�C���^
  CONFIG    *config;        ///<�R���t�B�O�Z�[�u�f�[�^�ւ̃|�C���^
  BOX_MANAGER *boxMng;      ///<�{�b�N�X�Ǘ��\���̂ւ̃|�C���^
  WIFI_LIST* WiFiList;
  BGM_INFO_SYS* bgm_info_sys; // BGM���擾�V�X�e��

  MMDLSYS *mmdlsys;
  EVENTWORK *eventwork;
  MAP_MATRIX * mapMatrix;     ///<�}�b�v�}�g���b�N�X�Ǘ�
  
  FIELD_SOUND *field_sound; ///<�t�B�[���h�T�E���h�Ǘ�

  FIELD_STATUS * fldstatus;  ///<�t�B�[���h�}�b�v���

  u32 last_battle_result;   ///<�ŐV�̃o�g���̌���

  int fieldmap_walk_count; ///<�t�B�[���h�}�b�v�����J�E���g
  u8 season_id;       ///<�G�ߎw��ID
  u8 weather_id;      ///<�V�C�w��ID
  u8 subscreen_mode; ///< �t�B�[���h����ʂ̏��
  u8 subscreen_type;
  u8 frameSpritcount;    ///< �t���[����������œ��삷��ꍇ�̃J�E���g
  u8 frameSpritEnable;   ///< �t���[����������œ��삷��ꍇ�̋���

  u8 intrude_num;         ///<�N�����Ă��鎞�̐ڑ��l��
  u8 intrude_my_id;       ///<�N�����Ă��鎩����NetID
  u8 intrude_reverse_area;  ///<TRUE:���t�B�[���h�N����
  
  u8 is_save;             ///<TRUE:�Z�[�u���s��
  u8 symbol_map_id;       ///<�V���{���}�b�v�̌��݈ʒu
  u8 pad;
  
  OCCUPY_INFO occupy[OCCUPY_ID_MAX];    ///<�苒���
  FIELD_WFBC_CORE wfbc[GAMEDATA_WFBC_ID_MAX];  ///<WhiteForest BlackCity
  
  ENCOUNT_WORK* enc_work; ///<�G���J�E���g�֘A�f�[�^���[�N
  
  SHORTCUT_CURSOR shortcut_cursor;  ///<�V���[�g�J�b�g��ʂ̃J�[�\��
  
  GIMMICKWORK GimmickWork;      //�M�~�b�N���[�N
  PL_BOAT_WORK_PTR PlBoatWorkPtr;   //�V���D���[�N

  COMM_PLAYER_SUPPORT *comm_player_support;   ///<�ʐM�v���C���[�T�|�[�g
  
  BSUBWAY_SCRWORK *bsubway_scrwork; //�o�g���T�u�E�F�C���[�N

  BEACON_STATUS* beacon_status; ///<�r�[�R���֘A���O��

  CALENDER* calender;  // �J�����_�[
  UNSV_WORK UnsvWork;      //���A���[�N
  TRIAL_HOUSE_WORK_PTR  TrialHouseWorkPtr;    //�g���C�A���n�E�X���[�N
  MUSICAL_SCRIPT_WORK *musicalScrWork;  //�~���[�W�J���X�N���v�g�p���[�N

  PLAYTIME* playtime; // �v���C����
};

//==============================================================================
//  �v���g�^�C�v�錾
//==============================================================================
static void GAMEDATA_SaveDataLoad(GAMEDATA *gamedata);
static void GAMEDATA_SaveDataUpdate(GAMEDATA *gamedata);


//============================================================================================
//
//
//    GAMEDATA�S��
//
//
//============================================================================================
//------------------------------------------------------------------
/*
 *  @brief  
 * 
 *  ��GAMEDATA�̏������ɕK�v�ȃe���|�����m�ۂɎg���q�[�v�́A
 *  �@���[�J���ϐ�tmpHeapID�ɐݒ肳��Ă�����̂��g�p���Ă�������
 *    10.03.26���_�� GFL_HEAPID_APP��Low�ł��B
 */
//------------------------------------------------------------------
GAMEDATA * GAMEDATA_Create(HEAPID heapID)
{
  int i;
  GAMEDATA * gd;

  //GAMEDATA�̏������ɕK�v�ȃe���|�����m�ۂɎg���q�[�v�͂���tmpHeapID�ɓ��ꂵ�Ă�������
  HEAPID  tmpHeapID = GFL_HEAP_LOWID( GFL_HEAPID_APP );

  gd = GFL_HEAP_AllocClearMemory(heapID, sizeof(GAMEDATA));

  gd->sv_control_ptr = SaveControl_GetPointer();

  //�G�ߐݒ�F�Q�[�����ł�1�������P�V�[�Y��
  gd->season_id = PMSEASON_GetRealTimeSeasonID();

  //
  gd->subscreen_mode = FIELD_SUBSCREEN_NORMAL;

  //�󋵃f�[�^
  gd->situation = SaveData_GetSituation(gd->sv_control_ptr);

  gd->start_loc = Situation_GetStartLocation(gd->situation);
  gd->entrance_loc = Situation_GetStartLocation(gd->situation);
  gd->special_loc = Situation_GetSpecialLocation(gd->situation);
  gd->escape_loc = Situation_GetEscapeLocation(gd->situation);

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
  gd->eventwork = SaveData_GetEventWork( gd->sv_control_ptr );
  
  //�}�b�v�}�g���b�N�X
  gd->mapMatrix = MAP_MATRIX_Create( heapID );

  //���샂�f��
  gd->mmdlsys = MMDLSYS_CreateSystem( heapID, MMDL_MDL_MAX, SaveControl_DataPtrGet(gd->sv_control_ptr,GMDATA_ID_ROCKPOS) );
  
  //�t�B�[���h�T�E���h�Ǘ�
  gd->field_sound = FIELD_SOUND_Create( gd, heapID );

  gd->fldstatus = FIELD_STATUS_Create( heapID );

  gd->bagcursor = MYITEM_BagCursorAlloc( heapID );
  gd->myitem = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYITEM);
  gd->my_pokeparty = SaveControl_DataPtrGet(gd->sv_control_ptr, GMDATA_ID_MYPOKE);
  gd->config  = SaveData_GetConfig( gd->sv_control_ptr );
  gd->boxMng  = BOX_DAT_InitManager( heapID , gd->sv_control_ptr );
  //WifiList
  gd->WiFiList = WifiList_AllocWork(heapID);

  //�苒���
  for(i = 0; i < OCCUPY_ID_MAX; i++){
    OccupyInfo_WorkInit(&gd->occupy[i]);
  }

  //�X�����
  for(i = 0; i < GAMEDATA_WFBC_ID_MAX; i++){
    FIELD_WFBC_CORE_Clear(&gd->wfbc[i]);
  }

  // �v���C�^�C��
  gd->playtime = PLAYTIME_Allock( heapID );
  
  //�����J�E���g
  gd->fieldmap_walk_count = 0;

  if(SaveControl_NewDataFlagGet(gd->sv_control_ptr) == FALSE){
  }
  
  //�G���J�E���g���[�N������
  gd->enc_work = ENCOUNT_WORK_Create( heapID );

  //�V���[�g�J�b�g�p�J�[�\��������
  SHORTCUT_CURSOR_Init( &gd->shortcut_cursor );

  GAMEDATA_SaveDataLoad(gd);  //�Z�[�u�f�[�^����K�v�Ȃ��̂����[�h���Ă���

  //�V���D�|�C���^���m�t�k�k������
  gd->PlBoatWorkPtr = NULL;
  
  //�o�g���T�u�E�F�C NULL������
  gd->bsubway_scrwork = NULL;
  
  //�ʐM���肩��̃T�|�[�g�f�[�^
  gd->comm_player_support = COMM_PLAYER_SUPPORT_Alloc(heapID);

  //�r�[�R���X�e�[�^�X���[�N����
  gd->beacon_status = BEACON_STATUS_Create( heapID, tmpHeapID );

  // �J�����_�[����
  gd->calender = CALENDER_Create( gd, heapID );

  //�g���C�A���n�E�X���[�N�|�C���^���m�t�k�k������
  gd->TrialHouseWorkPtr = NULL;

  //�~���[�W�J���p�X�N���v�g���[�N NULL������
  gd->musicalScrWork = NULL;

  return gd;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void GAMEDATA_Delete(GAMEDATA * gamedata)
{
  GFL_HEAP_FreeMemory( gamedata->playtime );
  CALENDER_Delete( gamedata->calender );
  BEACON_STATUS_Delete( gamedata->beacon_status );
  COMM_PLAYER_SUPPORT_Free(gamedata->comm_player_support);
  ENCOUNT_WORK_Delete( gamedata->enc_work );
  BGM_INFO_DeleteSystem(gamedata->bgm_info_sys);
  GFL_HEAP_FreeMemory( gamedata->WiFiList );
  BOX_DAT_ExitManager( gamedata->boxMng );
  GFL_HEAP_FreeMemory(gamedata->bagcursor);
  MMDLSYS_FreeSystem(gamedata->mmdlsys);
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  OCCUPY_INFO�ւ̃|�C���^
 */
//--------------------------------------------------------------
OCCUPY_INFO * GAMEDATA_GetMyOccupyInfo(GAMEDATA * gamedata)
{
  return GAMEDATA_GetOccupyInfo(gamedata, OCCUPY_ID_MINE);
}


//----------------------------------------------------------------------------
/**
 *  @brief  ������WFBC�X���̎擾
 *
 *  @param  gamedata  �Q�[���f�[�^
 *
 *  @return WFBC�X���̃|�C���^
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE* GAMEDATA_GetMyWFBCCoreData( GAMEDATA * gamedata )
{
  return GAMEDATA_GetWFBCCoreData( gamedata, GAMEDATA_WFBC_ID_MINE );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �v���C���[��WFBC�X���̎擾
 *
 *  @param  gamedata      �Q�[���f�[�^
 *  @param  player_id     �v���C���[ID
 *
 *  @return WFBC�X���̃|�C���^
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE* GAMEDATA_GetWFBCCoreData( GAMEDATA * gamedata, GAMEDATA_WFBC_ID id )
{
  GF_ASSERT_MSG(id < GAMEDATA_WFBC_ID_MAX, "wfbc data data_id = %d\n", id);
  return &gamedata->wfbc[id];
}

//----------------------------------------------------------------------------
/**
 *  @brief  WFBC�̃A�C�e���z�u�����擾����
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  
 *  @return �A�C�e��
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_CORE_ITEM* GAMEDATA_GetWFBCItemData( GAMEDATA * gamedata )
{
  RANDOMMAP_SAVE* save = RANDOMMAP_SAVE_GetRandomMapSave( gamedata->sv_control_ptr );
  return RANDOMMAP_SAVE_GetItemData( save );
}

//----------------------------------------------------------------------------
/**
 *  @brief  WFBC�̃C�x���g�f�[�^���擾����
 *
 *  @param  gamedata  �Q�[���f�[�^
 *
 *  @return �C�x���g
 */
//-----------------------------------------------------------------------------
FIELD_WFBC_EVENT* GAMEDATA_GetWFBCEventData( GAMEDATA * gamedata )
{
  RANDOMMAP_SAVE* save = RANDOMMAP_SAVE_GetRandomMapSave( gamedata->sv_control_ptr );
  return RANDOMMAP_SAVE_GetEventData( save );
}


//----------------------------------------------------------------------------
/**
 *  @brief  Palace�̃R�A���ݒ�
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @param  cp_core   Palace�̃R�A���
 */ 
//-----------------------------------------------------------------------------
void GAMEDATA_SetUpPalaceWFBCCoreData( GAMEDATA * gamedata, const FIELD_WFBC_CORE* cp_core )
{
  GF_ASSERT( gamedata );
  GF_ASSERT( cp_core );
  GFL_STD_MemCopy( cp_core, &gamedata->wfbc[ GAMEDATA_WFBC_ID_COMM ], sizeof(FIELD_WFBC_CORE) );

  // Palace�}�b�v��ZONE�ύX����
  FIELD_WFBC_CORE_SetUpZoneData( &gamedata->wfbc[ GAMEDATA_WFBC_ID_COMM ] );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Palace�̃R�A����j��
 *
 *  @param  gamedata �Q�[���f�[�^
 */
//-----------------------------------------------------------------------------
void GAMEDATA_ClearPalaceWFBCCoreData( GAMEDATA * gamedata )
{
  GF_ASSERT( gamedata );
  GFL_STD_MemClear( &gamedata->wfbc[ GAMEDATA_WFBC_ID_COMM ], sizeof(FIELD_WFBC_CORE) );

  // �����̃}�b�v��ZONE�ύX����
  FIELD_WFBC_CORE_SetUpZoneData( &gamedata->wfbc[ GAMEDATA_WFBC_ID_MINE ] );
}


//----------------------------------------------------------------------------
/**
 *  @brief  GAMEDATA�����WIFIList�̎擾
 *  @param  gamedata      �Q�[���f�[�^
 *  @return WIFIList�̃|�C���^
 */
//-----------------------------------------------------------------------------
WIFI_LIST * GAMEDATA_GetWiFiList(GAMEDATA * gamedata)
{
  return gamedata->WiFiList;
}



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^���擾����
 * @param gamedata      GAMEDATA�ւ̃|�C���^
 * @return  EVENTDATA_SYSTEM  �C�x���g�N���f�[�^�V�X�e���ւ̃|�C���^
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
//--------------------------------------------------------------
const LOCATION * GAMEDATA_GetPalaceReturnLocation(const GAMEDATA * gamedata)
{
  return &gamedata->palace_return_loc;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void GAMEDATA_SetPalaceReturnLocation(GAMEDATA * gamedata, const LOCATION * loc)
{
  gamedata->palace_return_loc = * loc;
}

//--------------------------------------------------------------
/**
 * @brief ���[�v��ѐ�ID�̃Z�b�g
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @param warp_id   ���[�v��ѐ�ID�w��
 */
//--------------------------------------------------------------
void GAMEDATA_SetWarpID(GAMEDATA * gamedata, u16 warp_id)
{
  u16 * warp = Situation_GetWarpID( gamedata->situation );
  *warp = warp_id;
}

//--------------------------------------------------------------
/**
 * @brief  ���[�v��ѐ�ID�̎擾
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @return  u16 ���[�v��ѐ�ID
 */
//--------------------------------------------------------------
u16 GAMEDATA_GetWarpID(GAMEDATA * gamedata)
{
  u16 * warp = Situation_GetWarpID( gamedata->situation );
  return *warp;
}

//--------------------------------------------------------------
/**
 * @brief   �o�b�O�J�[�\���̃|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @brief MMDLSYS�ւ̃|�C���^�擾
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @retval  MMDLSYS�ւ̃|�C���^
 */
//--------------------------------------------------------------
MMDLSYS * GAMEDATA_GetMMdlSys(GAMEDATA *gamedata)
{
  return gamedata->mmdlsys;
}

//--------------------------------------------------------------
/**
 * @brief EVENTWORK�ւ̃|�C���^�擾
 * @param gamedata  GAMEDATA�ւ̃|�C���^
 * @retval  EVENTWORK�ւ̃|�C���^
 */
//--------------------------------------------------------------
EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata)
{
  return gamedata->eventwork;
}

//--------------------------------------------------------------
/**
 * @brief   MAP_MATRIX�ւ̃|�C���^�擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @retval  MAP_MATRIX�ւ̃|�C���^
 */
//--------------------------------------------------------------
MAP_MATRIX * GAMEDATA_GetMapMatrix(GAMEDATA * gamedata)
{
  return gamedata->mapMatrix;
}

//--------------------------------------------------------------
/**
 * @brief �J�����_�[�̎擾
 *
 * @param  gamedata GAMEDATA�ւ̃|�C���^
 * @return CALENDER
 */
//--------------------------------------------------------------
CALENDER* GAMEDATA_GetCalender( GAMEDATA * gamedata )
{
  return gamedata->calender;
}

//--------------------------------------------------------------
/**
 * @brief �G�߂̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  u8  �G��ID�igamesystem/pm_season.h�Q�Ɓj
 */
//--------------------------------------------------------------
u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata)
{
  return gamedata->season_id;
}

//--------------------------------------------------------------
/**
 * @brief �G�߂̃Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param season_id �G��ID�igamesystem/pm_season.h�Q�Ɓj
 *
 * ���ۂ̋@�\�Ƃ��ĕK�v���ǂ����͋^�₾���f�o�b�O�p�Ƃ��Ēǉ�
 */
//--------------------------------------------------------------
void GAMEDATA_SetSeasonID(GAMEDATA *gamedata, u8 season_id)
{
  GF_ASSERT(season_id < PMSEASON_TOTAL);
  gamedata->season_id = season_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�CID���擾
 *
 *  @param  cp_data   �f�[�^
 *
 *  @return �V�CID
 */
//-----------------------------------------------------------------------------
u32 GAMEDATA_GetWeatherNo( const GAMEDATA* cp_data )
{
  return cp_data->weather_id;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �V�CID��o�^
 *
 *  @param  p_data
 *  @param  weather_no 
 */
//-----------------------------------------------------------------------------
void GAMEDATA_SetWeatherNo( GAMEDATA* p_data, u8 weather_no )
{
  p_data->weather_id = weather_no;
}



//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʂ̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  subscreen_mode
 */
//--------------------------------------------------------------
u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata)
{
  return gamedata->subscreen_mode;
}

//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʂ̐ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   subscreen_mode
 */
//--------------------------------------------------------------
void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode)
{
  GF_ASSERT(subscreen_mode < FIELD_SUBSCREEN_MODE_MAX);
  gamedata->subscreen_mode = subscreen_mode;
}
//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʃ^�C�v�̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @return  subscreen_type
 */
//--------------------------------------------------------------
const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata)
{
  return gamedata->subscreen_type;
}

//--------------------------------------------------------------
/**
 * @brief   �t�B�[���h����ʎ�ނ̐ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   subscreen_type
 */
//--------------------------------------------------------------
void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type)
{
  gamedata->subscreen_type = subscreen_type;
}

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삷��ꍇ�̋��t���O�ݒ�
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   bEnable ��������ꍇTRUE
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
 * @brief   �t���[����������œ��삷��ꍇ�̃J�E���^���Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 */
//------------------------------------------------------------------
void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata)
{
  gamedata->frameSpritcount = 0;    ///< �t���[����������œ��삷��ꍇ�̃J�E���g
}

//------------------------------------------------------------------
/**
 * @brief   �t���[����������œ��삷��ꍇ�̃t���[��
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
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
 * @brief   �t���[����������œ��삵�Ă��邩�ǂ���
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  ���삵�Ă���ꍇTRUE
 */
//------------------------------------------------------------------
BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata)
{
  return gamedata->frameSpritEnable;
}

//------------------------------------------------------------------
/**
 * @brief   �t�B�[���h�}�b�v�����J�E���g�擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @return  int �����J�E���g
 */
//------------------------------------------------------------------
int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata)
{
  return gamedata->fieldmap_walk_count;
}

//------------------------------------------------------------------
/**
 * @brief   �t�B�[���h�}�b�v�����J�E���g�Z�b�g
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param btl_result  �퓬���ʁiinclude/battle/battle.h��BtlResult�j
 */
//--------------------------------------------------------------
void GAMEDATA_SetLastBattleResult( GAMEDATA * gamedata, u32 btl_result )
{
  gamedata->last_battle_result = btl_result;
}

//--------------------------------------------------------------
/**
 * @brief �r�[�R���X�e�[�^�X���[�N�擾
 * @param   gamedata  GAMEDATA�ւ̃|�C���^
 * @param   BEACON_STATUS*
 */
//--------------------------------------------------------------
BEACON_STATUS* GAMEDATA_GetBeaconStatus( GAMEDATA * gamedata )
{
  return gamedata->beacon_status;
}

//============================================================================================
//
//
//  PLAYER_WORK�p�֐�
//  ��player_work.c���쐬���Ĉړ������邩��
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief PLAYER_WORK������
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
  player->objcode_fix = OBJCODEMAX;
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
 * @brief �����^�C�v�ŕ����p�x��ݒ肷��
 * @param player    PLAYER_WORK�ւ̃|�C���^
 * @param dir_type  �������w�肷��l(DIR_�`)
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
 * @brief �����p�x�̕����^�C�v���擾����
 * @param player  PLAYER_WORK�ւ̃|�C���^
 * @return  �������w�肷��l(DIR_�`)
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

//--------------------------------------------------------------
//--------------------------------------------------------------
void PLAYERWORK_SetOBJCodeFix( PLAYER_WORK *player, u16 code )
{
  player->objcode_fix = code;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
u16 PLAYERWORK_GetOBJCodeFix( const PLAYER_WORK *player )
{
  return player->objcode_fix;
}

//==================================================================
/**
 * @brief   FIELD_STATUS�̎擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   �ڑ��l��
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   ������NetID
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
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  int   ������NetID
 */
//==================================================================
void GAMEDATA_SetIntrudeMyID(GAMEDATA *gamedata, int intrude_my_id)
{
  gamedata->intrude_my_id = intrude_my_id;
  OS_TPrintf("Intrude MyNetID=%d\n", intrude_my_id);
}

//==================================================================
/**
 * ���t�B�[���h�N���t���O���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  u8  TRUE:���t�B�[���h�Z�����@FALSE:�\�t�B�[���h
 */
//==================================================================
int GAMEDATA_GetIntrudeReverseArea(const GAMEDATA *gamedata)
{
  return gamedata->intrude_reverse_area;
}

//==================================================================
/**
 * ���t�B�[���h�N���t���O���Z�b�g
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @param   reverse_flag  TRUE:���t�B�[���h�N����
 */
//==================================================================
void GAMEDATA_SetIntrudeReverseArea(GAMEDATA *gamedata, u8 reverse_flag)
{
  gamedata->intrude_reverse_area = reverse_flag;
  OS_TPrintf("gamedata ���t�B�[���h�N���t���O�Z�b�g =%d\n", reverse_flag);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �펞�ʐM���[�h�t���O�擾
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  
 *  @return TRUE�펞�ʐM���[�hON FALSE�펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
BOOL GAMEDATA_GetAlwaysNetFlag( const GAMEDATA *gamedata )
{ 
  return CONFIG_GetNetworkSearchMode( gamedata->config ) == NETWORK_SEARCH_ON;
}
//----------------------------------------------------------------------------
/**
 *  @brief  �펞�ʐM���[�h�t���O�ݒ�
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  is_on       TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//-----------------------------------------------------------------------------
void GAMEDATA_SetAlwaysNetFlag( GAMEDATA *gamedata, BOOL is_on )
{ 
  NETWORK_SEARCH_MODE mode;
  if( is_on ){
    mode = NETWORK_SEARCH_ON;
  }else{
    mode = NETWORK_SEARCH_OFF;
  }
  CONFIG_SetNetworkSearchMode( gamedata->config, mode );
}

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���o�^�ݒ�
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  shortcutID  Y�{�^���o�^����ID
 *  @param  is_on       TRUE�Ȃ��Y�{�^���o�^ FALSE�Ȃ�Ή���
 */
//----------------------------------------------------------------------------
void GAMEDATA_SetShortCut( GAMEDATA *gamedata, SHORTCUT_ID shortcutID, BOOL is_on )
{ 
  SAVE_CONTROL_WORK*  p_sv  = GAMEDATA_GetSaveControlWork(gamedata);

  SHORTCUT *p_shortcut  =  SaveData_GetShortCut( p_sv );

  SHORTCUT_SetRegister( p_shortcut, shortcutID, is_on );  
}

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���o�^�ݒ�
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *  @param  shortcutID  Y�{�^���o�^����ID
 *  @retval TRUE�Ȃ��YBTN�ɓo�^���Ă���  FALSE�Ȃ�Γo�^���Ă��Ȃ�
 */
//----------------------------------------------------------------------------
BOOL GAMEDATA_GetShortCut( const GAMEDATA *gamedata, SHORTCUT_ID shortcutID )
{ 
  const SAVE_CONTROL_WORK*  cp_sv = GAMEDATA_GetSaveControlWorkConst(gamedata);

  const SHORTCUT *cp_shortcut =  SaveData_GetShortCutConst( cp_sv );

  return SHORTCUT_GetRegister( cp_shortcut, shortcutID ); 
}


//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���p���j���[�̃J�[�\���Z�b�g
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @param  cursor    �J�[�\��
 */
//-----------------------------------------------------------------------------
void GAMEDATA_SetShortCutCursor( GAMEDATA *gamedata, SHORTCUT_CURSOR *cursor )
{ 
  gamedata->shortcut_cursor = *cursor;
}

//----------------------------------------------------------------------------
/**
 *  @brief  Y�{�^���p���j���[�̃J�[�\���擾
 *
 *  @param  gamedata  �Q�[���f�[�^
 *  @retval cursor    �J�[�\��
 */
//-----------------------------------------------------------------------------
SHORTCUT_CURSOR * GAMEDATA_GetShortCutCursor( GAMEDATA *gamedata )
{ 
  return &gamedata->shortcut_cursor;
}

//==============================================================================
//  �Q�[���f�[�^�����������ɃZ�[�u
//==============================================================================
///���̊֐��͂����ł����g�p���Ă͂����Ȃ��̂Œ��ڒ�`
extern WIFI_LIST* SaveData_GetWifiListData(SAVE_CONTROL_WORK* pSave);

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^���[�N����Q�[���f�[�^�����������[�h����
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void GAMEDATA_SaveDataLoad(GAMEDATA *gamedata)
{
  if(SaveControl_NewDataFlagGet(gamedata->sv_control_ptr) == TRUE){
    return; //�Z�[�u�f�[�^�������̂ŉ����ǂݍ��܂Ȃ�
  }

  { //PLAYER_WORK
    PLAYER_WORK *pw;
    pw = GAMEDATA_GetMyPlayerWork(gamedata);
    SaveData_PlayerDataLoad(gamedata->sv_control_ptr, pw);
    SaveData_SituationDataLoad(gamedata->sv_control_ptr, pw);
  }

  { //MMDL
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys(gamedata);
    MMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_MMDL);
    MMDL_SAVEDATA_Load( mmdlsys, pw );
  }
  
  { //OCCUPY_INFO
    OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
    SaveData_OccupyInfoLoad(gamedata->sv_control_ptr, occupy);
  }

  { //WFBC_CORE
    FIELD_WFBC_CORE *wfbc = GAMEDATA_GetMyWFBCCoreData(gamedata);
    RANDOMMAP_SAVE* save = RANDOMMAP_SAVE_GetRandomMapSave( gamedata->sv_control_ptr );
    RANDOMMAP_SAVE_GetCoreWork(save, wfbc);
  }

  { //GIMMICK_WORK
    SaveData_LoadGimmickWork(gamedata->sv_control_ptr, &gamedata->GimmickWork);
  }

  { //FIELD_STATUS
    SITUATION * situation = gamedata->situation;
    SaveData_SituationDataLoadStatus( situation, gamedata->fldstatus );
    SaveData_SituationLoadSeasonID( situation, &gamedata->season_id );
  }
  { //WIFILIST
    WIFI_LIST* wifilist = GAMEDATA_GetWiFiList(gamedata);
    const WIFI_LIST* swifilist = SaveData_GetWifiListData( gamedata->sv_control_ptr );
    WifiList_Copy(swifilist, wifilist);
  }
  { //WEATHER_ID
    SITUATION* sv = gamedata->situation;
    SaveData_SituationLoadWeatherID( sv, &gamedata->weather_id );
  }
  { //UNSV_WORK
    SaveData_LoadUnsvWork(gamedata->sv_control_ptr, &gamedata->UnsvWork);
  }
  { //PlayTime
    PLAYTIME* sv = SaveData_GetPlayTime( gamedata->sv_control_ptr );
    PLAYTIME_SetAllData( gamedata->playtime, sv );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�f�[�^�̃��[�N���X�V����
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * �Z�[�u����f�[�^���A�Z�[�u���[�N���璼�ڎg�p�����ɁA�ʃ��[�N�ɃR�s�[���Ďg�p���Ă�����̂��Ώ�
 */
//--------------------------------------------------------------
static void GAMEDATA_SaveDataUpdate(GAMEDATA *gamedata)
{
  { //PLAYER_WORK
    PLAYER_WORK *pw = GAMEDATA_GetMyPlayerWork(gamedata);
    SaveData_PlayerDataUpdate(gamedata->sv_control_ptr, pw);
    SaveData_SituationDataUpdate(gamedata->sv_control_ptr, pw);
  }

  { //MMDL
    MMDLSYS *mmdlsys = GAMEDATA_GetMMdlSys(gamedata);
    MMDL_SAVEDATA *pw = SaveControl_DataPtrGet(gamedata->sv_control_ptr,GMDATA_ID_MMDL);
    MMDL_SAVEDATA_Save( mmdlsys, pw );
  }
  
  { //OCCUPY_INFO
    OCCUPY_INFO *occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
    SaveData_OccupyInfoUpdate(gamedata->sv_control_ptr, occupy);
  }

  { //WFBC_CORE
    FIELD_WFBC_CORE *wfbc = GAMEDATA_GetMyWFBCCoreData(gamedata);
    RANDOMMAP_SAVE* save = RANDOMMAP_SAVE_GetRandomMapSave( gamedata->sv_control_ptr );
    RANDOMMAP_SAVE_SetCoreWork(save, wfbc);
  }

  { //GIMMICK_WORK
    SaveData_SaveGimmickWork(&gamedata->GimmickWork, gamedata->sv_control_ptr);
  }

  { //FIELD_STATUS
    SITUATION * situation = gamedata->situation;
    SaveData_SituationDataUpdateStatus( situation, gamedata->fldstatus );
    SaveData_SituationUpdateSeasonID( situation, gamedata->season_id );
  }
  { //WIFILIST
    const WIFI_LIST* wifilist = GAMEDATA_GetWiFiList(gamedata);
    WIFI_LIST* swifilist = SaveData_GetWifiListData( gamedata->sv_control_ptr );
    WifiList_Copy(wifilist, swifilist);
  }
  { //WEATHER_ID
    SITUATION* sv = gamedata->situation;
    SaveData_SituationUpdateWeatherID( sv, gamedata->weather_id );
  }

  { //UNSV_WORK
    SaveData_SaveUnsvWork(&gamedata->UnsvWork, gamedata->sv_control_ptr);
  }
  { //PlayTime
    PLAYTIME* sv = SaveData_GetPlayTime( gamedata->sv_control_ptr );
    PLAYTIME_SetSaveTime( gamedata->playtime );
    PLAYTIME_SetAllData( sv, gamedata->playtime );
  }
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɃZ�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata)
{
  SAVE_RESULT sr;
  
  //�Z�[�u���[�N�̏����X�V
  GAMEDATA_SaveDataUpdate(gamedata);

  //�r�[�R���̃X�L�����𑦎���~
  WIH_DWC_Stop();

  //���Z�b�g�֎~
  GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_SVLD);

  //�Z�[�u���s
  sr = SaveControl_Save(gamedata->sv_control_ptr);

  //���Z�b�g����
  GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);
  
  //�r�[�R���̃X�L�������ĊJ
  WIH_DWC_Restart();

  return sr;
}

//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u���J�n
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
void GAMEDATA_SaveAsyncStart(GAMEDATA *gamedata)
{
  //�Z�[�u���[�N�̏����X�V
  GAMEDATA_SaveDataUpdate(gamedata);

  //�r�[�R���̃X�L�����𑦎���~
  WIH_DWC_Stop();

  //�Z�[�u���t���OON
  gamedata->is_save = TRUE;

  //���Z�b�g�֎~
  GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_SVLD);
  
  //�Z�[�u�J�n
  SaveControl_SaveAsyncInit(gamedata->sv_control_ptr);
}


//--------------------------------------------------------------
/**
 * @brief   �Q�[���f�[�^�����������ɕ����Z�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT GAMEDATA_SaveAsyncMain(GAMEDATA *gamedata)
{
  SAVE_RESULT sr;

  if(PMSND_IsLoading() == TRUE){
    return SAVE_RESULT_CONTINUE;
  }

  sr = SaveControl_SaveAsyncMain(gamedata->sv_control_ptr);

  if(sr==SAVE_RESULT_OK || sr==SAVE_RESULT_NG){

    //���Z�b�g����
    GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);
    //�r�[�R���̃X�L�������ĊJ
    WIH_DWC_Restart();
    //�Z�[�u���t���OOFF
    gamedata->is_save = FALSE;
  }

  return sr;
}

//==================================================================
/**
 * @brief   �����Z�[�u���L�����Z������
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//==================================================================
void GAMEDATA_SaveAsyncCancel(GAMEDATA *gamedata)
{
  if( gamedata->is_save == TRUE )
  {
    //���Z�b�g����
    GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);
    //�r�[�R���̃X�L�������ĊJ
    WIH_DWC_Restart();
    //�Z�[�u���t���OOFF
    gamedata->is_save = FALSE;

    SaveControl_SaveAsyncCancel( gamedata->sv_control_ptr );
  }
}

//==================================================================
/**
 * �Z�[�u���s�������ׂ�
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  BOOL        TRUE:�Z�[�u���s���@�@FALSE:�Z�[�u���Ă��Ȃ�
 */
//==================================================================
BOOL GAMEDATA_GetIsSave(const GAMEDATA *gamedata)
{
  return gamedata->is_save;
}

//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�̕����Z�[�u�J�n
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 *
 * @retval  none
 */
//--------------------------------------------------------------
void GAMEDATA_ExtraSaveAsyncStart(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id)
{
  //�Z�[�u���[�N�̏����X�V
  GAMEDATA_SaveDataUpdate(gamedata);

  //�r�[�R���̃X�L�����𑦎���~
  WIH_DWC_Stop();

  //�Z�[�u���t���OON
  gamedata->is_save = TRUE;

  //���Z�b�g�֎~
  GFL_UI_SoftResetDisable(GFL_UI_SOFTRESET_SVLD);
  
  //�Z�[�u�J�n
  SaveControl_Extra_SaveAsyncInit(gamedata->sv_control_ptr, extra_id);
}


//--------------------------------------------------------------
/**
 * @brief   �O���Z�[�u�̕����Z�[�u�����s
 *
 * @param   gamedata    �Q�[���f�[�^�ւ̃|�C���^
 * @param   extra_id    �O���Z�[�u�f�[�^�ԍ�
 *
 * @retval  �Z�[�u����
 */
//--------------------------------------------------------------
SAVE_RESULT GAMEDATA_ExtraSaveAsyncMain(GAMEDATA *gamedata, SAVE_EXTRA_ID extra_id)
{
  SAVE_RESULT sr;

  if(PMSND_IsLoading() == TRUE){
    return SAVE_RESULT_CONTINUE;
  }

  sr = SaveControl_Extra_SaveAsyncMain(gamedata->sv_control_ptr, extra_id);

  if(sr==SAVE_RESULT_OK || sr==SAVE_RESULT_NG){

    //���Z�b�g����
    GFL_UI_SoftResetEnable(GFL_UI_SOFTRESET_SVLD);
    //�r�[�R���̃X�L�������ĊJ
    WIH_DWC_Restart();
    //�Z�[�u���t���OOFF
    gamedata->is_save = FALSE;
  }

  return sr;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�R���g���[�����[�N���擾����
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  SAVE_CONTROL_WORK �̃|�C���^
 */
//--------------------------------------------------------------

SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata)
{
  return gamedata->sv_control_ptr;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�R���g���[�����[�N���擾����
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  SAVE_CONTROL_WORK �̃|�C���^
 */
//--------------------------------------------------------------
const SAVE_CONTROL_WORK* GAMEDATA_GetSaveControlWorkConst(const GAMEDATA * gamedata)
{
  return gamedata->sv_control_ptr;
}

//----------------------------------------------------------
/**
 * @brief   �M�~�b�N�f�[�^�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �M�~�b�N�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
GIMMICKWORK * GAMEDATA_GetGimmickWork(GAMEDATA * gamedata)
{
  return &gamedata->GimmickWork;
}

//----------------------------------------------------------
/**
 * @brief   �V���D�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �V���D���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
PL_BOAT_WORK_PTR *GAMEDATA_GetPlBoatWorkPtr(GAMEDATA * gamedata)
{
  return &gamedata->PlBoatWorkPtr;
}

//----------------------------------------------------------
/**
 * @brief   �o�g���T�u�E�F�C���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  BSUBWAY_SCRWORK*
 */
//----------------------------------------------------------
BSUBWAY_SCRWORK * GAMEDATA_GetBSubwayScrWork(GAMEDATA * gamedata)
{
  return gamedata->bsubway_scrwork;
}

//----------------------------------------------------------
/**
 * @brief   �o�g���T�u�E�F�C���[�N�Z�b�g
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  nothing
 */
//----------------------------------------------------------
void GAMEDATA_SetBSubwayScrWork(GAMEDATA * gamedata, BSUBWAY_SCRWORK *bsw_scr )
{
  gamedata->bsubway_scrwork = bsw_scr;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�@�v���C�f�[�^�@���擾
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *
 *  @return�@BSUBWAY_PLAYDATA
 */
//-----------------------------------------------------------------------------
BSUBWAY_PLAYDATA * GAMEDATA_GetBSubwayPlayData( GAMEDATA* gamedata )
{
  return (BSUBWAY_PLAYDATA*)SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_BSUBWAY_PLAYDATA );

}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�@�X�R�A�f�[�^�@���擾
 *
 *  @param  gamedata    GAMEDATA�ւ̃|�C���^
 *
 *  @return BSUBWAY_SCOREDATA
 */
//-----------------------------------------------------------------------------
BSUBWAY_SCOREDATA * GAMEDATA_GetBSubwayScoreData( GAMEDATA* gamedata )
{
  return (BSUBWAY_SCOREDATA*)SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_BSUBWAY_SCOREDATA );
}

//----------------------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�@WiFi�f�[�^�@���擾
 *
 *  @param  gamedata  GAMEDATA�ւ̃|�C���^
 *
 *  @return BSUBWAY_WIFIDATA
 */
//-----------------------------------------------------------------------------
BSUBWAY_WIFI_DATA * GAMEDATA_GetBSubwayWifiData( GAMEDATA* gamedata )
{
  return (BSUBWAY_WIFI_DATA*)SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_BSUBWAY_WIFIDATA );
}


//----------------------------------------------------------
/**
 * @brief   MISC���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  MISC*
 */
//----------------------------------------------------------
MISC * GAMEDATA_GetMiscWork(GAMEDATA * gamedata)
{
  return SaveData_GetMisc(gamedata->sv_control_ptr);
}

//----------------------------------------------------------
/**
 * @brief   WIFI_NEGOTIATION_SAVEDATA���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  WIFI_NEGOTIATION_SAVEDATA*
 */
//----------------------------------------------------------
WIFI_NEGOTIATION_SAVEDATA * GAMEDATA_GetWifiNegotiation(GAMEDATA * gamedata)
{
  return WIFI_NEGOTIATION_SV_GetSaveData(gamedata->sv_control_ptr);
}

//==================================================================
/**
 * �T�|�[�g�f�[�^�ւ̃|�C���^�擾
 *
 * @param   gamedata    
 *
 * @retval  COMM_PLAYER_SUPPORT *   
 */
//==================================================================
COMM_PLAYER_SUPPORT * GAMEDATA_GetCommPlayerSupportPtr(GAMEDATA * gamedata)
{
  return gamedata->comm_player_support;
}


//=============================================================================================
/**
 * @brief RECORD���[�N�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  RECORD *    
 */
//=============================================================================================
RECORD *GAMEDATA_GetRecordPtr(GAMEDATA * gamedata)
{
  return SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_RECORD);
}

//=============================================================================================
/**
 * @brief ���A���[�N�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  UNSV_WORK *    
 */
//=============================================================================================
UNSV_WORK *GAMEDATA_GetUnsvWorkPtr(GAMEDATA * gamedata)
{
  return &gamedata->UnsvWork;
}

//----------------------------------------------------------
/**
 * @brief �~���[�W�J���Z�[�u�|�C���^�擾
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 * @retval  UNSV_WORK *    
 */
//----------------------------------------------------------
MUSICAL_SAVE *GAMEDATA_GetMusicalSavePtr(GAMEDATA * gamedata)
{
  return SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_MUSICAL);
}

//----------------------------------------------------------
/**
 * @brief   �g���C�A���n�E�X���[�N�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �g���C�A���n�E�X���[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
TRIAL_HOUSE_WORK_PTR *GAMEDATA_GetTrialHouseWorkPtr(GAMEDATA * gamedata)
{
  return &gamedata->TrialHouseWorkPtr;
}

//----------------------------------------------------------
/**
 * @brief   �g���[�i�[�J�[�h�f�[�^�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �g���[�i�[�J�[�h�f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
TR_CARD_SV_PTR GAMEDATA_GetTrainerCardPtr(GAMEDATA * gamedata)
{
  return TRCSave_GetSaveDataPtr( gamedata->sv_control_ptr );
}

//----------------------------------------------------------
/**
 * @brief   ���N���A�|�P�����f�[�^�ւ̃|�C���^�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  �a������f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
DENDOU_RECORD *GAMEDATA_GetDendouRecord(GAMEDATA * gamedata)
{
  return SaveControl_DataPtrGet( gamedata->sv_control_ptr, GMDATA_ID_GAMECLEAR_POKE);
}


//----------------------------------------------------------
/**
 * @brief   �~���[�W�J���p�X�N���v�g���[�N�Z�b�g
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  nothing
 */
//----------------------------------------------------------
void GAMEDATA_SetMusicalScrWork(GAMEDATA * gamedata, MUSICAL_SCRIPT_WORK* musScrWork )
{
  gamedata->musicalScrWork = musScrWork;
}

//----------------------------------------------------------
/**
 * @brief   �~���[�W�J���p�X�N���v�g���[�N�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  MUSICAL_SCRIPT_WORK*
 */
//----------------------------------------------------------
MUSICAL_SCRIPT_WORK* GAMEDATA_GetMusicalScrWork(GAMEDATA * gamedata)
{
  GF_ASSERT_MSG( gamedata->musicalScrWork != NULL , "�~���[�W�J�����[�N��NULL�I" );
  return gamedata->musicalScrWork;
}

//----------------------------------------------------------
/**
 * @brief   PDW�̃Z�[�u�擾
 * @param   gamedata      GAMEDATA�ւ̃|�C���^
 * @return  DREAMWORLD_SAVEDATA*
 */
//----------------------------------------------------------
DREAMWORLD_SAVEDATA* GAMEDATA_GetDreamWorkdSaveWork(GAMEDATA * gamedata)
{
  return DREAMWORLD_SV_GetDreamWorldSaveData( gamedata->sv_control_ptr );
}


//==================================================================
/**
 * @brief �y���b�v�^���f�[�^�̃|�C���^���擾
 *
 * @param   gamedata    GAMEDATA�ւ̃|�C���^
 *
 * @retval  extern PERAPVOICE *   �y���b�v�^���f�[�^�ւ̃|�C���^
 */
//==================================================================
PERAPVOICE *GAMEDATA_GetPerapVoice( GAMEDATA *gamedata )
{
  return SaveData_GetPerapVoice( gamedata->sv_control_ptr );
}

//==================================================================
/**
 * �V���{���}�b�v�̌��݈ʒu���Z�b�g����
 *
 * @param   gamedata    
 * @param   symbol_map_id   �V���{���}�b�vID
 */
//==================================================================
void GAMEDATA_SetSymbolMapID(GAMEDATA *gamedata, u8 symbol_map_id)
{
  gamedata->symbol_map_id = symbol_map_id;
}

//==================================================================
/**
 * �V���{���}�b�v�̌��݈ʒu���擾����
 *
 * @param   gamedata    
 *
 * @retval  u8    �V���{���}�b�vID
 */
//==================================================================
u8 GAMEDATA_GetSymbolMapID(const GAMEDATA *gamedata)
{
  return gamedata->symbol_map_id;
}

//==================================================================
/**
 * PLAYTIME �J�E���g����
 *
 * @param   gamedata    
 * @param   value       �J�E���g�l
 */
//==================================================================
void GAMEDATA_PlayTimeCountUp(GAMEDATA *gamedata, u32 value)
{
  PLAYTIME_CountUp( gamedata->playtime, value );
}

//==================================================================
/**
 * PLAYTIME �擾 
 *
 * @param   gamedata    
 *
 * @return PLAYTIME (GAMEDATA��������alloc�������[�N)
 */
//==================================================================
PLAYTIME* GAMEDATA_GetPlayTimeWork(GAMEDATA *gamedata)
{
  return gamedata->playtime;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �����|�P�����@���̌�@�́@�Z�[�u�f�[�^�擾
 *
 *  @param  gdata     �Q�[���f�[�^
 *    
 *  @return �Z�[�u�f�[�^
 */
//-----------------------------------------------------------------------------
TRPOKE_AFTER_SAVE* GAMEDATA_GetTrPokeAfterSaveData( GAMEDATA* gdata )
{
  return SaveControl_DataPtrGet( gdata->sv_control_ptr, GMDATA_ID_TRPOKE_AFTER);
 
}


//--------------------------------------------------------------
/**
 * @brief �C�x���g�t���O�E���[�N�Ǘ��Z�[�u�f�[�^�̎擾
 * @param sv_control_ptr  �Z�[�u�f�[�^���䃏�[�N�ւ̃|�C���^
 * @return  EVENTWORK 
 *
 * @note
 * �{����GAMEDATA�o�R�ł��ׂẴZ�[�u�f�[�^���A�N�Z�X�ł���͂���������
 * WB�ł͖����ɏI���A�J���I�Ղł��̊֐���ǉ�����K�v���ł����̂ō쐬�B
 * �o�������SAVE_CONTROL_WORK�o�R�̃Z�[�u�f�[�^�A�N�Z�X�͎g�������Ȃ����A
 * �`�����X������΍폜�������̂�GMDATA_ID_EVENT_WORK���Ǐ��Q�ƂƂȂ�悤
 * ���̊֐���game_data.c�̒��ɒu���Ă����B
 */
//--------------------------------------------------------------
EVENTWORK * SaveData_GetEventWork( SAVE_CONTROL_WORK * sv_control_ptr )
{
  return SaveControl_DataPtrGet( sv_control_ptr, GMDATA_ID_EVENT_WORK );
}


