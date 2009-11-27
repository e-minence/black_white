/*
 *  @file   btl_setup.c
 *  @brief  �o�g���p�����[�^�Z�b�g�A�b�v
 *  @author Taya & Iwasawa
 *  @date   09.10.07
 */

#include <gflib.h>
#include "system/main.h"
#include "gamesystem/pm_season.h"
#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��

#include "gamesystem/btl_setup.h"

///�v���g�^�C�v
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

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
  if(bp->partyPlayer){
    GFL_HEAP_FreeMemory(bp->partyPlayer);
  }
  if(bp->partyPartner){
    GFL_HEAP_FreeMemory(bp->partyPartner);
  }
  if(bp->partyEnemy1){
    GFL_HEAP_FreeMemory(bp->partyEnemy1);
  }
  if(bp->partyEnemy2){
    GFL_HEAP_FreeMemory(bp->partyEnemy2);
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
  GF_ASSERT( party );

  switch( client ){
  case BTL_CLIENT_PLAYER:
    PokeParty_Copy( party, bp->partyPlayer );
    break;
  case BTL_CLIENT_PARTNER:
    PokeParty_Copy( party, bp->partyPartner );
    break;
  case BTL_CLIENT_ENEMY1:
    PokeParty_Copy( party, bp->partyEnemy1 );
    break;
  case BTL_CLIENT_ENEMY2:
    PokeParty_Copy( party, bp->partyEnemy2 );
    break;
  }
}

/*
 *  @brief  �퓬�t�B�[���h�V�`���G�[�V�����f�[�^�f�t�H���g������
 */
void BTL_FIELD_SITUATION_Init( BTL_FIELD_SITUATION* sit )
{
  setup_common_situation( sit );
}


static void setup_common_situation( BTL_FIELD_SITUATION* sit )
{
  // @todo ���݂͉��쐬
  sit->bgType = BATTLE_BG_TYPE_GRASS;
  sit->bgAttr = BATTLE_BG_ATTR_LAWN;
  sit->timeZone = TIMEZONE_MORNING;
  sit->season = PMSEASON_SPRING;
  sit->weather = BTL_WEATHER_NONE; 
}

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BTL_FIELD_SITUATION* sit )

{
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->netID = 0;
  dst->multiMode = 0;

  dst->partyPlayer = NULL;
  dst->partyEnemy1 = NULL;
  dst->partyPartner = NULL;
  dst->partyEnemy2 = NULL;

  dst->statusPlayer = GAMEDATA_GetMyStatus( gameData );
  dst->itemData     = GAMEDATA_GetMyItem( gameData );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gameData );
  dst->zukanData    = GAMEDATA_GetZukanSave( gameData );
  {
    // @todo ������SAVE_CONTROL_WORK�ɒ��A�N�Z�X���Ȃ��悤�ɏ���������H
    SAVE_CONTROL_WORK* saveCtrl = GAMEDATA_GetSaveControlWork( gameData );
    dst->configData = SaveData_GetConfig( saveCtrl );
  }
  MI_CpuCopy8( sit, &dst->fieldSituation, sizeof( BTL_FIELD_SITUATION ) );

  dst->musicDefault = SEQ_BGM_VS_NORAPOKE;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;

  dst->result = BTL_RESULT_WIN;
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
void BTL_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gdata,
  const POKEPARTY* partyEnemy, const BTL_FIELD_SITUATION* sit, const BtlRule rule, HEAPID heapID )
{
  BATTLE_PARAM_Init( bp );
  setup_common( bp, gdata, (BTL_FIELD_SITUATION*)sit );

  bp->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gdata), bp->partyPlayer );

  bp->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, bp->partyEnemy1 );
  }

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;

  bp->trID = TRID_NULL;
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
static void setup_common_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BtlRule rule,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  BATTLE_PARAM_Init( dst );
  setup_common( dst, gameData, sit );

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gameData), dst->partyPlayer );
  
  dst->partyEnemy1 = PokeParty_AllocPartyWork( heapID );
  if( partyEnemy != NULL ){
    PokeParty_Copy( partyEnemy, dst->partyEnemy1 );
  }

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = rule;

  dst->trID = trID;
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
  BtlRule rule, BOOL multi, GFL_NETHANDLE* netHandle, BtlCommMode commMode, HEAPID heapID )
{
  BTL_FIELD_SITUATION sit;

  BATTLE_PARAM_Init( dst );

  BTL_FIELD_SITUATION_Init(&sit);
  sit.bgType = BATTLE_BG_TYPE_ROOM;
  sit.bgAttr = BATTLE_BG_ATTR_NORMAL_GROUND;

  setup_common( dst, gameData, &sit );

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gameData), dst->partyPlayer );
  
  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = rule;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = multi;

  dst->trID = TRID_NULL;
}

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
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_SINGLE, partyEnemy, sit, trID, heapID );
}

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
  setup_common_CommTrainer( dst, gameData, BTL_RULE_SINGLE, FALSE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * �_�u�� �Q�[�����g���[�i�[�ΐ�
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
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_DOUBLE, partyEnemy, sit, trID, heapID );
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
  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, FALSE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * �_�u���i�}���`�j�ʐM�ΐ�
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
  setup_common_CommTrainer( dst, gameData, BTL_RULE_DOUBLE, TRUE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * �g���v�� �Q�[�����g���[�i�[�ΐ�
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
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_TRIPLE, partyEnemy, sit, trID, heapID );
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
  setup_common_CommTrainer( dst, gameData, BTL_RULE_TRIPLE, FALSE, netHandle, commMode, heapID );
}

//=============================================================================================
/**
 * ���[�e�[�V���� �Q�[�����g���[�i�[�ΐ�
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
void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BTL_FIELD_SITUATION* sit, TrainerID trID, HEAPID heapID )
{
  setup_common_Trainer( dst, gameData, BTL_RULE_ROTATION, partyEnemy, sit, trID, heapID );
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
  setup_common_CommTrainer( dst, gameData, BTL_RULE_ROTATION, FALSE, netHandle, commMode, heapID );
}
