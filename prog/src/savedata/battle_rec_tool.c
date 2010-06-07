//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file     battle_rec_tool.c
 *  @brief    battle_rec_save�n
 *  @author   Toru=Nagihashi
 *  @data     2010.05.06
 *
 *  battle_rec.c�̃I�[�o�[���C���傫���Ȃ��Ă����̂ŁA
 * BattleRec_LoadToolModule�`BattleRec_UnloadToolModule���g�����W���[����Ɨ�
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>


#include "system/main.h"
#include "savedata/save_tbl.h"
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "savedata/battle_rec.h"
#include "battle/battle.h"

#include "../battle/btl_common.h"
#include "../battle/btl_net.h"
#include "../poke_tool/poke_tool_def.h"

#include "mystatus_local.h"
#include "net_app/gds/gds_profile_local.h"

#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/gds_profile.h"
#include "battle/btl_common.h"
#include "net_app/gds/gds_profile_local.h"
#include "battle_rec_local.h"
#include "net_app/gds/gds_battle_mode.h"

//=============================================================================
/**
 *          �萔�錾
*/
//=============================================================================

extern BATTLE_REC_SAVEDATA * brs;

//=============================================================================
/**
 *          �\���̐錾
*/
//=============================================================================

//=============================================================================
/**
 *          �v���g�^�C�v�錾
*/
//=============================================================================
static void PokeParty_to_RecPokeParty( const POKEPARTY *party, REC_POKEPARTY *rec_party );
static void RecPokeParty_to_PokeParty( REC_POKEPARTY *rec_party, POKEPARTY *party, HEAPID heapID );
static void store_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static void restore_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec, HEAPID heapID );
static void store_ClientStatus( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static void restore_ClientStatus( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );
static void store_TrainerData( const BSP_TRAINER_DATA* bspTrainer, BTLREC_TRAINER_STATUS* recTrainer );
static void restore_TrainerData( BSP_TRAINER_DATA* bspTrainer, const BTLREC_TRAINER_STATUS* recTrainer );
static BOOL store_OperationBuffer( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static BOOL restore_OperationBuffer( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );
static BOOL store_SetupSubset( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static BOOL restore_SetupSubset( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );


//=============================================================================================
/**
 * �o�g���Z�b�g�A�b�v�p�����[�^��^��Z�[�u�f�[�^�ɕϊ����Ę^��Z�[�u�o�b�t�@�Ɋi�[����
 *
 * �����O�� BattleRec_LoadToolModule �̌Ăяo�����K�v�B
 *    �g���I������� BattleRec_UnoadToolModule ���Ăяo���Ă��������B
 *
 * @param   setup   �o�g���Z�b�g�A�b�v�p�����[�^
 */
//=============================================================================================
void BattleRec_StoreSetupParam( const BATTLE_SETUP_PARAM* setup )
{
  BATTLE_REC_WORK  *rec = &brs->rec;

  store_Party( setup, rec );
  store_ClientStatus( setup, rec );
  store_OperationBuffer( setup, rec );
  store_SetupSubset( setup, rec );
}

//=============================================================================================
/**
 * �^��Z�[�u�o�b�t�@����o�g���Z�b�g�A�b�v�p�����[�^�𕜌�����
 *
 * �����O�� BattleRec_LoadToolModule �̌Ăяo�����K�v�B
 *    �g���I������� BattleRec_UnoadToolModule ���Ăяo���Ă��������B
 *
 * @param   setup   [out] ������
 */
//=============================================================================================
void BattleRec_RestoreSetupParam( BATTLE_SETUP_PARAM* setup, HEAPID heapID )
{
  BATTLE_REC_WORK  *rec = &brs->rec;

  TAYA_Printf("*** Rec Info ***\n");
  TAYA_Printf(" recWorkSize = %d byte\n", sizeof(BATTLE_REC_WORK));
  TAYA_Printf("   ->setupSubset  = %d byte\n", sizeof(rec->setupSubset) );
  TAYA_Printf("   ->opBuffer     = %d byte\n", sizeof(rec->opBuffer) );
  TAYA_Printf("   ->clientStatus = %d byte\n", sizeof(rec->clientStatus) );
  TAYA_Printf("   ->rec_party    = %d byte\n", sizeof(rec->rec_party) );


  restore_Party( setup, rec, heapID );
  restore_ClientStatus( setup, rec );
  restore_OperationBuffer( setup, rec );
  restore_SetupSubset( setup, rec );
}

//=============================================================================
/**
 *    �v���g�^�C�v
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY��REC_POKEPARTY�ɕϊ�����
 *
 * @param   party     �ϊ����f�[�^�ւ̃|�C���^
 * @param   rec_party   �ϊ���̃f�[�^�����
 */
//--------------------------------------------------------------
static void PokeParty_to_RecPokeParty( const POKEPARTY *party, REC_POKEPARTY *rec_party )
{
  int i;
  POKEMON_PARAM *pp;

  GFL_STD_MemClear(rec_party, sizeof(REC_POKEPARTY));

  if(PokeParty_GetPokeCount(party) == 0){  //�s���`�F�b�N�p�ɋ�̎��͑S��0�ŃN���A���ꂽ�܂܏I��
    return;
  }

  rec_party->PokeCountMax = PokeParty_GetPokeCountMax(party);
  rec_party->PokeCount = PokeParty_GetPokeCount(party);

  for(i=0; i < rec_party->PokeCount; i++){
    pp = PokeParty_GetMemberPointer(party, i);
    POKETOOL_PokePara_to_RecPokePara(pp, &rec_party->member[i]);
    NAGI_Printf( "poke[%d]=%d\n", i, PP_Get( pp, ID_PARA_monsno, NULL ) );
  }
}

//--------------------------------------------------------------
/**
 * @brief   REC_POKEPARTY��POKEPARTY�ɕϊ�����
 *
 * @param   rec_party   �ϊ����f�[�^�ւ̃|�C���^
 * @param   party     �ϊ���̃f�[�^�����
 */
//--------------------------------------------------------------
static void RecPokeParty_to_PokeParty( REC_POKEPARTY *rec_party, POKEPARTY *party, HEAPID heapID )
{
  int i;
  POKEMON_PARAM *pp;
  u8 cb_id_para = 0;

  pp = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), POKETOOL_GetWorkSize() );
  TAYA_Printf("RecParty pokeCnt=%d\n", rec_party->PokeCount);

  PokeParty_Init(party, rec_party->PokeCountMax);
  for(i = 0; i < rec_party->PokeCount; i++){
    POKETOOL_RecPokePara_to_PokePara(&rec_party->member[i], pp);
    PP_Put(pp, ID_PARA_cb_id, cb_id_para);  //�J�X�^���{�[���͏o�Ȃ��悤�ɂ���
    PokeParty_Add(party, pp);
  }

  GFL_HEAP_FreeMemory( pp );
}


//----------------------------------------------------------------------------------
/**
 * �p�[�e�B�f�[�^�F�^��Z�[�u�f�[�^�����Ċi�[
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void store_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  u32 i;

  //�s���`�F�b�N�p�ɖ��g�p�̈�͑S��0�Ŗ��߂�K�v������ׁA��U�o�b�t�@�S�̂�0�N���A���Ă���
  GFL_STD_MemClear(rec->rec_party, sizeof(REC_POKEPARTY) * BTL_CLIENT_MAX);

  for(i=0; i<BTL_CLIENT_NUM; ++i)
  {
    if( setup->party[i] ){
      PokeParty_to_RecPokeParty( setup->party[i], &(rec->rec_party[i]) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �p�[�e�B�f�[�^�F�^��Z�[�u�f�[�^����
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void restore_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec, HEAPID heapID )
{
  u32 i;
  for(i=0; i<BTL_CLIENT_NUM; ++i)
  {
    if( setup->party[i] ){
      RecPokeParty_to_PokeParty( &(rec->rec_party[i]), setup->party[i], heapID );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �N���C�A���g�X�e�[�^�X�F�^��Z�[�u�f�[�^�����Ċi�[
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void store_ClientStatus( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  u32 i;
  for(i=0; i<NELEMS(setup->playerStatus); ++i)
  {
    // �����̃`�F�b�N���Ԃ͕ς��Ă͂����܂���
    // �ibtl_setup.c�ŃT�u�E�F�C�g���[�i�[��playerStatus��ݒ肵�Ă���j
    if( setup->tr_data[i] ){
      store_TrainerData( setup->tr_data[i], &rec->clientStatus[i].trainer );
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_TRAINER;
    }
    else if( setup->playerStatus[i] ){
      MyStatus_Copy( setup->playerStatus[i], &rec->clientStatus[i].player );
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_PLAYER;
    }
    else{
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_NONE;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �N���C�A���g�X�e�[�^�X�F�^��Z�[�u�f�[�^���畜��
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void restore_ClientStatus( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  u32 i;
  for(i=0; i<NELEMS(setup->playerStatus); ++i)
  {
    switch( rec->clientStatus[i].type ){
    case BTLREC_CLIENTSTATUS_PLAYER:
      GF_ASSERT(setup->playerStatus[i]!=NULL);
      MyStatus_Copy( &rec->clientStatus[i].player, (MYSTATUS*)setup->playerStatus[i] );
      break;

    case BTLREC_CLIENTSTATUS_TRAINER:
      GF_ASSERT(setup->tr_data[i]!=NULL);
      restore_TrainerData( setup->tr_data[i], &rec->clientStatus[i].trainer );
      break;
    }
  }
}

/**
 *  �g���[�i�[�f�[�^�F�^��Z�[�u�f�[�^�p�ɕϊ����Ċi�[
 */
static void store_TrainerData( const BSP_TRAINER_DATA* bspTrainer, BTLREC_TRAINER_STATUS* recTrainer )
{
  u32 i;

  recTrainer->tr_id     = bspTrainer->tr_id;
  recTrainer->tr_type   = bspTrainer->tr_type;
  recTrainer->ai_bit    = bspTrainer->ai_bit;
  recTrainer->win_word  = bspTrainer->win_word;
  recTrainer->lose_word = bspTrainer->lose_word;

  for(i=0; i<NELEMS(recTrainer->use_item); ++i){
    recTrainer->use_item[i] = bspTrainer->use_item[i];
  }

  GFL_STR_GetStringCode( bspTrainer->name, recTrainer->name, NELEMS(recTrainer->name) );
}
/**
 *  �g���[�i�[�f�[�^�F�^��Z�[�u�f�[�^���畜��
 */
static void restore_TrainerData( BSP_TRAINER_DATA* bspTrainer, const BTLREC_TRAINER_STATUS* recTrainer )
{
  u32 i;

  bspTrainer->tr_id     = recTrainer->tr_id;
  bspTrainer->tr_type   = recTrainer->tr_type;
  bspTrainer->ai_bit    = recTrainer->ai_bit;
  bspTrainer->win_word  = recTrainer->win_word;
  bspTrainer->lose_word = recTrainer->lose_word;

  for(i=0; i<NELEMS(bspTrainer->use_item); ++i){
    bspTrainer->use_item[i] = recTrainer->use_item[i];
  }

  GFL_STR_SetStringCode( bspTrainer->name, recTrainer->name );

  {
    const STRCODE* sp = GFL_STR_GetStringCodePointer( bspTrainer->name );
    OS_TPrintf("Rec -> Btl  trID=%d, trType=%d, name= ", bspTrainer->tr_id, bspTrainer->tr_type);
    while( (*sp) != GFL_STR_GetEOMCode() ){
      OS_TPrintf( "%04x,", (*sp) );
      ++sp;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ����o�b�t�@�F�^��Z�[�u�f�[�^�p�ɕϊ����Ċi�[
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL store_OperationBuffer( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  if( (setup->recBuffer != NULL)
  &&  (setup->recDataSize < sizeof(rec->opBuffer.buffer))
  ){
    rec->opBuffer.size = setup->recDataSize;
    GFL_STD_MemCopy( setup->recBuffer, rec->opBuffer.buffer, setup->recDataSize );
    TAYA_Printf("Store Operation Buffer %dbyte\n", rec->opBuffer.size);
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ����o�b�t�@�F�^��Z�[�u�f�[�^���畜��
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL restore_OperationBuffer( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  setup->recDataSize = rec->opBuffer.size;
  TAYA_Printf("Restore Operation Buffer %dbyte\n", setup->recDataSize );
  GFL_STD_MemCopy( rec->opBuffer.buffer, setup->recBuffer, setup->recDataSize );
  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�p�����[�^�����f�[�^��^��Z�[�u�f�[�^�ϊ����Ċi�[
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL store_SetupSubset( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  rec->setupSubset.fieldSituation = setup->fieldSituation;
  rec->setupSubset.randomContext = setup->recRandContext;
  rec->setupSubset.musicDefault = setup->musicDefault;
  rec->setupSubset.musicWin = setup->musicWin;

  rec->setupSubset.competitor = setup->competitor;
  rec->setupSubset.rule = setup->rule;
  rec->setupSubset.fMultiMode = setup->multiMode;
  rec->setupSubset.debugFlagBit = setup->DebugFlagBit;
  rec->setupSubset.myCommPos = setup->commPos;

  CONFIG_Copy( setup->configData, &rec->setupSubset.config );

  {
    u32 i;
    const u8* p = (u8*)(&(rec->setupSubset.randomContext));
    TAYA_Printf("*** �����߂���RandomContext ***\n");
    for(i=0; i<sizeof(setup->recRandContext); ++i)
    {
      TAYA_Printf("%02x ", p[i]);
      if( (i+1)%8 == 0 ){
        TAYA_Printf("\n");
      }
    }
  }

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�p�����[�^����
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL restore_SetupSubset( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  setup->fieldSituation = rec->setupSubset.fieldSituation;
  setup->recRandContext = rec->setupSubset.randomContext;
  setup->musicDefault = rec->setupSubset.musicDefault;
  setup->musicWin = rec->setupSubset.musicWin;

  setup->competitor = rec->setupSubset.competitor;
  setup->rule = rec->setupSubset.rule;
  setup->multiMode = rec->setupSubset.fMultiMode;
  setup->DebugFlagBit = rec->setupSubset.debugFlagBit;
  setup->commPos = rec->setupSubset.myCommPos;

  CONFIG_Copy( &rec->setupSubset.config, (CONFIG*)(setup->configData) );

  return TRUE;
}

