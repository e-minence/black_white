/*
 *  @file   btl_setup.c
 *  @brief  �o�g���p�����[�^�Z�b�g�A�b�v
 *  @author Taya & Iwasawa
 *  @date   09.10.07
 */

#include <gflib.h>
#include "system/main.h"
#include "sound/wb_sound_data.sadl" //�T�E���h���x���t�@�C��

#include "gamesystem/btl_setup.h"

///�v���g�^�C�v
void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );
void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

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

static void setup_common( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData, BtlBgType bgType, BtlBgAttr bgAttr, BtlWeather weather )
{
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->netID = 0;
  dst->multiMode = 0;

  dst->partyPlayer = NULL;  //GAMEDATA_GetMyPokemon( gameData );
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

  // @todo ���݂͉��쐬
  dst->fieldSituation.bgType = bgType;
  dst->fieldSituation.bgAttr = bgAttr;
  dst->fieldSituation.timeZone = TIMEZONE_MORNING;
  dst->fieldSituation.season = GAMEDATA_GetSeasonID( gameData );;
  dst->fieldSituation.weather = weather;


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
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 */
//=============================================================================================
void BP_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gdata, HEAPID heapID, const BtlRule rule,
  const POKEPARTY* partyEnemy, const BtlBgType bgType, const BtlBgAttr bgAttr, const BtlWeather weather )
{
  setup_common( bp, gdata, bgType, bgAttr, weather );

  bp->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gdata), bp->partyPlayer );

  bp->partyEnemy1 = (POKEPARTY*)partyEnemy;

  bp->competitor = BTL_COMPETITOR_WILD;
  bp->rule = rule;

  bp->trID = TRID_NULL;
}


//=============================================================================================
/**
 * �V���O�� �Q�[�����g���[�i�[�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   partyEnemy
 * @param   landForm
 * @param   weather
 * @param   trID
 */
//=============================================================================================
void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlBgType bgType, BtlBgAttr bgAttr, BtlWeather weather, TrainerID trID, HEAPID heapID )
{
  setup_common( dst, gameData, bgType, bgAttr, weather );

  dst->partyPlayer = PokeParty_AllocPartyWork( heapID );
  PokeParty_Copy( GAMEDATA_GetMyPokemon(gameData), dst->partyPlayer );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_SINGLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * �V���O�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_BG_ROOM, 0, BTL_WEATHER_NONE );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_SINGLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = 0;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlBgType bgType, BtlBgAttr bgAttr, BtlWeather weather, TrainerID trID, HEAPID heapID )
{
  setup_common( dst, gameData, bgType, bgAttr, weather );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_DOUBLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * �_�u�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_BG_ROOM, 0, BTL_WEATHER_NONE );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_DOUBLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = FALSE;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos )
{
  setup_common( dst, gameData, BTL_BG_ROOM, 0, BTL_WEATHER_NONE );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_DOUBLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = TRUE;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlBgType bgType, BtlBgAttr bgAttr, BtlWeather weather, TrainerID trID, HEAPID heapID )
{
  setup_common( dst, gameData, bgType, bgAttr, weather );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_TRIPLE;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}

//=============================================================================================
/**
 * �g���v�� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_BG_ROOM, 0, BTL_WEATHER_NONE );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_TRIPLE;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = 0;

  dst->trID = TRID_NULL;
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
 */
//=============================================================================================
void BTL_SETUP_Rotation_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlBgType bgType, BtlBgAttr bgAttr, BtlWeather weather, TrainerID trID, HEAPID heapID )
{
  setup_common( dst, gameData, bgType, bgAttr, weather );

  dst->competitor = BTL_COMPETITOR_TRAINER;
  dst->rule = BTL_RULE_ROTATION;

  dst->partyEnemy1 = partyEnemy;
  dst->trID = trID;
}
//=============================================================================================
/**
 * ���[�e�[�V���� �ʐM�ΐ�
 *
 * @param   dst
 * @param   gameData
 * @param   netHandle
 * @param   commMode
 */
//=============================================================================================
void BTL_SETUP_Rotation_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode )
{
  setup_common( dst, gameData, BTL_BG_ROOM, 0, BTL_WEATHER_NONE );

  dst->competitor = BTL_COMPETITOR_COMM;
  dst->rule = BTL_RULE_ROTATION;

  dst->netHandle = netHandle;
  dst->commMode = commMode;
  dst->netID = GFL_NET_GetNetID( netHandle );
  dst->multiMode = 0;

  dst->trID = TRID_NULL;
}
