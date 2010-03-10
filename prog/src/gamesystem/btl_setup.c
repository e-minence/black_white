/*
 *  @file   btl_setup.c
 *  @brief  �o�g���p�����[�^�Z�b�g�A�b�v
 *  @author Taya & Iwasawa
 *  @date   09.10.07
 */

#include <gflib.h>
#include "system/main.h"
#include "gamesystem/pm_season.h"
#include "savedata/battle_rec.h"
#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��
#include "tr_tool/tr_tool.h"
#include "tr_tool/trno_def.h"
#include "tr_tool/trtype_def.h"
#include "buflen.h"
#include "field/eventwork.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "gamesystem/btl_setup.h"

//------------------------------------------------------------
//------------------------------------------------------------


///�v���g�^�C�v
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

static BSP_TRAINER_DATA* BSP_TRAINER_DATA_Create( HEAPID heapID );
static void BSP_TRAINER_DATA_Delete( BSP_TRAINER_DATA* tr_data );
static void setup_common_situation( BTL_FIELD_SITUATION* sit );

/*
 *  @brief  �퓬�p�����[�^���[�N����
 *  @param  heapID  ���[�N���������m�ۂ���heapID
 *
 *  ��BATTLE_SETUP_PARAM�\���̗̈���A���P�[�g���A���������܂�
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���K�� BATTLE_PARAM_Delete()�ŉ�����Ă�������
 */
BATTLE_SETUP_PARAM* BATTLE_PARAM_Create( HEAPID heapID )
{
  BATTLE_SETUP_PARAM* bp;

  bp = GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
  BATTLE_PARAM_Init( bp );
  return bp;
}

/*
 *  @brief  �퓬�p�����[�^���[�N���
 *  @param  bp  BATTLE_PARAM_Create()�Ő������ꂽBATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
void BATTLE_PARAM_Delete( BATTLE_SETUP_PARAM* bp )
{
  BATTLE_PARAM_Release( bp );
  GFL_HEAP_FreeMemory( bp );
  bp = NULL;
}

/**
 *  @brief  �퓬�p�����[�^���[�N�̓�������������
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 *
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���g���I�������K��BATTLE_PARAM_Release()�֐��ŉ�����������Ă�������
 */
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp )
{
  MI_CpuClear8( bp, sizeof(BATTLE_SETUP_PARAM) );
  setup_common_situation( &bp->fieldSituation );
}

/*
 *  @brief  �퓬�p�����[�^���[�N�̓����A���P�[�V��������������ƃN���A
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp )
{
  int i;

  for( i = 0;i < BTL_CLIENT_NUM;i++)
  {
    if( bp->party[i] ){
      GFL_HEAP_FreeMemory( bp->party[i] );
    }
    if(bp->tr_data[i] != NULL){
      BSP_TRAINER_DATA_Delete( bp->tr_data[i]);
    }
    if( (i != BTL_CLIENT_PLAYER) && (bp->playerStatus[i] != NULL) ){
      GFL_HEAP_FreeMemory( bp->playerStatus[i] );
    }
  }
  if( bp->recBuffer ){
    GFL_HEAP_FreeMemory( bp->recBuffer );
  }

  MI_CpuClear8(bp,sizeof(BATTLE_SETUP_PARAM));
}

/*
 *  @brief  �o�g���p�����@PokeParty�f�[�^�Z�b�g
 *
 *  �n���ꂽ�|�C���^�̓��e���R�s�[���܂�
 */
void BATTLE_PARAM_SetPokeParty( BATTLE_SETUP_PARAM* bp, const POKEPARTY* party, BTL_CLIENT_ID client )
{
  GF_ASSERT(client<BTL_CLIENT_NUM);
  GF_ASSERT( party );

  PokeParty_Copy( party, bp->party[ client ] );
}

/*
 *  @brief  �o�g���p�����@PokeParty�|�C���^�擾
 */
POKEPARTY* BATTLE_PARAM_GetPokePartyPointer( BATTLE_SETUP_PARAM* bp, BTL_CLIENT_ID client )
{
  GF_ASSERT(client<BTL_CLIENT_NUM);
  return bp->party[ client ];
}

/*
 *  @brief  �o�g���p�����@�o�g���X�e�[�^�X�t���O�Z�b�g
 */
void BATTLE_PARAM_SetBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f)
{
  bp->btl_status_flag |= status_f;
}

/*
 *  @brief  �o�g���p���� �o�g���X�e�[�^�X�t���O�`�F�b�N
 */
BOOL BATTLE_PARAM_CheckBtlStatusFlag( BATTLE_SETUP_PARAM* bp, BTL_STATUS_FLAG status_f )
{
  return (bp->btl_status_flag & status_f);
}

/*
 *  @brief  �퓬�t�B�[���h�V�`���G�[�V�����f�[�^�f�t�H���g�������i�����ݒ�E���C�g�Ȃ��j
 */
void BTL_FIELD_SITUATION_Init( BTL_FIELD_SITUATION* sit )
{
  sit->bgType = BATTLE_BG_TYPE_ROOM;
  sit->bgAttr = BATTLE_BG_ATTR_LAWN;
  sit->weather = BTL_WEATHER_NONE;
  sit->zoneID = 0;
  sit->hour   = 0;
  sit->minute = 0;
  sit->season = 0;
}

static void setup_common_situation( BTL_FIELD_SITUATION* sit )
{
  // @todo ���݂͉��쐬
  sit->bgType = BATTLE_BG_TYPE_GRASS;
  sit->bgAttr = BATTLE_BG_ATTR_LAWN;
  sit->season = PMSEASON_SPRING;
  sit->weather = BTL_WEATHER_NONE;

  sit->zoneID = 0;
  sit->hour   = 10;
  sit->minute = 0;
}

/*
 *  @brief  �퓬�g���[�i�[�f�[�^���[�N������
 *
 *  �����Ń��������A���P�[�g���邽�߁A�K��BSP_TRAINER_DATA_Create�ŉ�����Ă�������
 */
static BSP_TRAINER_DATA* BSP_TRAINER_DATA_Create( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );

  return tr_data;
}

/*
 *  @brief  �퓬�g���[�i�[�f�[�^���[�N���
 *
 *  �����Ń��������A���P�[�g���邽�߁A�K��������Ă�������
 */
static void BSP_TRAINER_DATA_Delete( BSP_TRAINER_DATA* tr_data )
{
  GFL_STR_DeleteBuffer( tr_data->name );

  GFL_HEAP_FreeMemory( tr_data );
}

/*
 *  @brief  �g���[�i�[�p�����[�^�Z�b�g
 */
static void setup_trainer_param( BATTLE_SETUP_PARAM* dst, BTL_CLIENT_ID client, POKEPARTY** party, TrainerID tr_id, HEAPID heapID )
{
  *party = PokeParty_AllocPartyWork( heapID );
  dst->tr_data[client] = BSP_TRAINER_DATA_Create( heapID );

  if(tr_id != TRID_NULL){
    TT_EncountTrainerPersonalDataMake( tr_id, dst->tr_data[client], heapID );
    TT_EncountTrainerPokeDataMake( tr_id, *party, heapID );
  }
}

/*
 *  @brief  �v���C���[�p�����[�^�Z�b�g
 */
static void setup_player_param( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data;
  PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork( gameData );

  dst->party[ BTL_CLIENT_PLAYER ] = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon( gameData ), dst->party[ BTL_CLIENT_PLAYER ] );

  dst->tr_data[BTL_CLIENT_PLAYER] = BSP_TRAINER_DATA_Create( heapID );
  tr_data = dst->tr_data[BTL_CLIENT_PLAYER];

  MyStatus_CopyNameString( (const MYSTATUS*)&player->mystatus , tr_data->name );
  tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( (const MYSTATUS*)&player->mystatus );
}

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BTL_FIELD_SITUATION* sit )
{
  u32 i;

  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->multiMode = BTL_MULTIMODE_NONE;
  dst->recBuffer = NULL;
  dst->fRecordPlay = FALSE;

  for(i=0; i<BTL_CLIENT_NUM; ++i){
    dst->party[i] = NULL;
    dst->playerStatus[i] = NULL;
  }

  dst->playerStatus[BTL_CLIENT_PLAYER] = GAMEDATA_GetMyStatus( gameData );
  dst->itemData     = GAMEDATA_GetMyItem( gameData );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gameData );
  dst->zukanData    = GAMEDATA_GetZukanSave( gameData );
  dst->commSupport  = GAMEDATA_GetCommPlayerSupportPtr( gameData );

  {
    // @todo ������SAVE_CONTROL_WORK�ɒ��A�N�Z�X���Ȃ��悤�ɏ���������H
    SAVE_CONTROL_WORK* saveCtrl = GAMEDATA_GetSaveControlWork( gameData );
    const MISC* miscData = SaveData_GetMiscConst( saveCtrl );

    dst->configData = SaveData_GetConfig( saveCtrl );
    dst->badgeCount = MISC_GetBadgeCount( miscData );
  }
  MI_CpuCopy8( sit, &dst->fieldSituation, sizeof(BTL_FIELD_SITUATION) );

  dst->musicDefault = SEQ_BGM_VS_NORAPOKE;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;

  dst->result = BTL_RESULT_WIN;

  //�{�b�N�X�J���҂ɉ���Ă��邩�t���O
  if( EVENTWORK_CheckEventFlag( GAMEDATA_GetEventWork( gameData ), SYS_FLAG_PCNAME)){
    BATTLE_PARAM_SetBtlStatusFlag( dst, BTL_STATUS_FLAG_PCNAME_OPEN );
  }
}

//=============================================================================================
/**
 * @brief �쐶��R�����p�����[�^�Z�b�g
 *
 * @param   dst
 * @param   gameData
 * @param   heapID
 * @param   rule
 * @param   partyEnemy
 * @param   sit
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gameData,
  const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, const BtlRule rule, HEAPID heapID )
{
  BATTLE_PARAM_Init( bp );
  setup_common( bp, gameData, (BTL_FIELD_SITUATION*)sit );
  setup_player_param( bp, gameData, heapID );

  bp->party[ BTL_CLIENT_ENEMY1 ] = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, bp->party[ BTL_CLIENT_ENEMY1 ] );
  }

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;
}

//=============================================================================================
/**
 * �Q�[�����g���[�i�[�ΐ�R����
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
static void setup_common_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BtlRule rule, BTL_FIELD_SITUATION* sit, HEAPID heapID )
{
  BATTLE_PARAM_Init( dst );
  setup_common( dst, gameData, sit );
  setup_player_param( dst, gameData, heapID );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = rule;
}

//=============================================================================================
/**
 * �ʐM�ΐ�R����
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
static void setup_common_CommTrainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BtlRule rule, BtlMultiMode multiMode, GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos, HEAPID heapID )
{
  BTL_FIELD_SITUATION sit;

  BATTLE_PARAM_Init( dst );

  BTL_FIELD_SITUATION_Init(&sit);
  sit.bgType = BATTLE_BG_TYPE_ROOM;
  sit.bgAttr = BATTLE_BG_ATTR_NORMAL_GROUND;

  setup_common( dst, gameData, &sit );
  setup_player_param( dst, gameData, heapID );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = rule;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->multiMode = multiMode;
  dst->commPos = commPos;

  // �^��f�[�^�����̂��߁A�ΐ푊���MYSTATUS, POKEPARTY���󂯎��o�b�t�@�Ƃ��Ċm�ۂ��܂� taya
  {
    u32 i;
    for(i=0; i<BTL_CLIENT_NUM; ++i)
    {
      if( i != BTL_CLIENT_PLAYER )
      {
        if( dst->playerStatus[i] == NULL ){
          dst->playerStatus[i] = MyStatus_AllocWork( heapID );
        }
        if( dst->party[i] == NULL ){
          dst->party[i] = PokeParty_AllocPartyWork( heapID );
        }
      }
    }
  }
}

//=============================================================================================
/**
 * @brief �ߊl�f���R�����p�����[�^�Z�b�g
 *
 * @param   dst
 * @param   gameData
 * @param   partyMine
 * @param   partyEnemy
 * @param   sit
 * @param   heapID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_CaptureDemo( BATTLE_SETUP_PARAM* bp, GAMEDATA* gameData,
  const POKEPARTY* partyMine, const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, HEAPID heapID )
{
  BATTLE_PARAM_Init( bp );
  setup_common( bp, gameData, (BTL_FIELD_SITUATION*)sit );

  setup_player_param( bp, gameData, heapID );

  if( partyMine != NULL ){
    PokeParty_Copy( partyMine, bp->party[ BTL_CLIENT_PLAYER ] );
  }
  bp->party[ BTL_CLIENT_ENEMY1 ] = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, bp->party[ BTL_CLIENT_ENEMY1 ] );
  }

  bp->competitor = BTL_COMPETITOR_DEMO_CAPTURE;
  bp->rule = BTL_RULE_SINGLE;
}

///////////////////////////////////////////////////////////////////////////////
//  ���[�J���g���[�i�[�ΐ�֘A
///////////////////////////////////////////////////////////////////////////////

//=============================================================================================
/**
 * �V���O�� �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   sit
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_SINGLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], trID, heapID );
}

//=============================================================================================
/**
 * �_�u�� �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], trID, heapID );
}
//=============================================================================================
/**
 * �g���v�� �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_TRIPLE, sit, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], trID, heapID );
}

//=============================================================================================
/**
 * ���[�e�[�V���� �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_ROTATION, sit, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], trID, heapID );
}

//=============================================================================================
/**
 * �^�b�O �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Tag_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], tr_id0, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY2, &dst->party[ BTL_CLIENT_ENEMY2 ], tr_id1, heapID );
  dst->multiMode = BTL_MULTIMODE_P_AA;
}

//=============================================================================================
/**
 * AI�}���` �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_AIMulti_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  BTL_FIELD_SITUATION* sit, TrainerID partner, TrainerID tr_id0, TrainerID tr_id1, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, sit, heapID );

  setup_trainer_param( dst, BTL_CLIENT_PARTNER, &dst->party[ BTL_CLIENT_PARTNER ], partner, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], tr_id0, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY2, &dst->party[ BTL_CLIENT_ENEMY2 ], tr_id1, heapID );
  dst->multiMode = BTL_MULTIMODE_PA_AA;
}

///////////////////////////////////////////////////////////////////////////////
//  �ʐM�ΐ�֘A
///////////////////////////////////////////////////////////////////////////////
//=============================================================================================
/**
 * �V���O�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  u8 myCommPos = GFL_NET_GetNetID( netHandle );

  setup_common_CommTrainer( dst, gameData, BTL_RULE_SINGLE, BTL_MULTIMODE_NONE,
    netHandle, commMode, myCommPos, heapID );
}

//=============================================================================================
/**
 * �_�u�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  u8 myCommPos = GFL_NET_GetNetID( netHandle );

  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, BTL_MULTIMODE_NONE,
    netHandle, commMode, myCommPos, heapID );
}


//=============================================================================================
/**
 * �_�u���i�}���`�F�v���C���[ 2 vs 2�j�ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 * @param   commPos
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos, HEAPID heapID )
{
  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, BTL_MULTIMODE_PP_PP,
    netHandle, commMode, commPos, heapID );
}

//=============================================================================================
/**
 * �_�u���i�}���`�F�v���C���[2 vs AI�g���[�i�[2�j�ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 * @param   commPos
 * @param   tr_id1
 * @param   tr_id2
 * @param   heapID
 */
//=============================================================================================
void BTL_SETUP_AIMulti_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos, TrainerID tr_id1, TrainerID tr_id2, HEAPID heapID )
{
  GF_ASSERT((commPos==BTL_CLIENT_PLAYER)||(commPos==BTL_CLIENT_PARTNER));

  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, BTL_MULTIMODE_PP_AA,
      netHandle, commMode, commPos, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY1, &dst->party[ BTL_CLIENT_ENEMY1 ], tr_id1, heapID );
  setup_trainer_param( dst, BTL_CLIENT_ENEMY2, &dst->party[ BTL_CLIENT_ENEMY2 ], tr_id2, heapID );
}

//=============================================================================================
/**
 * �g���v�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  u8 myCommPos = GFL_NET_GetNetID( netHandle );

  setup_common_CommTrainer( dst, gameData, BTL_RULE_TRIPLE, BTL_MULTIMODE_NONE,
      netHandle, commMode, myCommPos, heapID );
}


//=============================================================================================
/**
 * ���[�e�[�V���� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 *
 * @li  �����Ń������̊m�ۂ��s���܂��B
 *      �o�g���I����ABATTLE_PARAM_Release()��������BATTLE_PARAM_Delete()�ŕK������������s���Ă�������
 */
//=============================================================================================
void BTL_SETUP_Rotation_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  u8 myCommPos = GFL_NET_GetNetID( netHandle );
  setup_common_CommTrainer( dst, gameData, BTL_RULE_ROTATION, BTL_MULTIMODE_NONE,
    netHandle, commMode, myCommPos, heapID );
}


//=============================================================================================
/**
 * �^��f�[�^�Đ��p�̏���������
 *
 * @param   dst
 * @param   gameData
 * @param   heapID
 */
//=============================================================================================
void BTL_SETUP_InitForRecordPlay( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, HEAPID heapID )
{
  GFL_STD_MemClear( dst, sizeof(BATTLE_SETUP_PARAM) );
  {
    u32 i;
    for(i=0; i<BTL_CLIENT_NUM; ++i)
    {
      dst->party[i] = PokeParty_AllocPartyWork( heapID );
      dst->playerStatus[i] = MyStatus_AllocWork( heapID );
      dst->tr_data[i] = BSP_TRAINER_DATA_Create( heapID );
    }
  }
  // config�f�[�^��alloc���Ă���̂ŕʓr������K�v�ɂȂ�
  dst->configData = CONFIG_AllocWork( heapID );
  dst->zukanData    = GAMEDATA_GetZukanSave( gameData );
  dst->fRecordPlay = TRUE;
  BTL_SETUP_AllocRecBuffer( dst, heapID );
}

//=============================================================================================
/**
 * �^��f�[�^�Đ������������f�[�^�̏I������
 *
 * @param   bsp
 */
//=============================================================================================
void BTL_SETUP_QuitForRecordPlay( BATTLE_SETUP_PARAM* bsp )
{
  if( bsp->configData ){
    GFL_HEAP_FreeMemory( (void*)(bsp->configData) );
    bsp->configData = NULL;
  }
  BATTLE_PARAM_Release( bsp );
}



/*
 *  @brief  �Z�b�g�A�b�v�ς݃p�����[�^���o�g���T�u�E�F�C���[�h�p�ɐ؂�ւ�
 */
void BTL_SETUP_SetSubwayMode( BATTLE_SETUP_PARAM* dst )
{
  if( dst->competitor == BTL_COMPETITOR_TRAINER ){
    dst->competitor = BTL_COMPETITOR_SUBWAY;
  }
}
/*
 *  @brief  �Z�b�g�A�b�v�ς݃p�����[�^�ɘ^��p�o�b�t�@�𐶐�
 */
void BTL_SETUP_AllocRecBuffer( BATTLE_SETUP_PARAM* dst, HEAPID heapID )
{
  if( dst->recBuffer == NULL ){
    dst->recBuffer = GFL_HEAP_AllocMemory( heapID,  BTLREC_OPERATION_BUFFER_SIZE );
  }
}
/*
 *  @brief �Z�b�g�A�b�v�p�����[�^��^��Đ����[�h�ɐ؂�ւ�
 */
void BTL_SETUP_SetRecordPlayMode( BATTLE_SETUP_PARAM* dst )
{
  dst->fRecordPlay = TRUE;
}

/*
 *  @brief �f�o�b�O�p�t���O�Z�b�g
 */
void BTL_SETUP_SetDebugFlag( BATTLE_SETUP_PARAM* dst, BtlDebugFlag flag )
{
  GF_ASSERT(flag < BTL_DEBUGFLAG_MAX);

  dst->DebugFlagBit |= (1 << flag);
}
/*
 *  @brief �f�o�b�O�p�t���O�擾
 */
BOOL BTL_SETUP_GetDebugFlag( const BATTLE_SETUP_PARAM* dst, BtlDebugFlag flag )
{
  GF_ASSERT(flag < BTL_DEBUGFLAG_MAX);

  return (dst->DebugFlagBit & (1 << flag)) != 0;
}


