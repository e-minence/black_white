//=============================================================================================
/**
 * @file  btl_server_flow_sub.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h���������i�T�u���[�`���Q�j
 * @author  taya
 *
 *  ���X btl_server_flow.c ���ɂ������֐����A���܂�Ɍ��ʂ��������Ȃ��Ă����̂Ŗ�����ɕ����������̂ł��B
 *  ���������킯�Ń\�[�X��ǂނƂ��� btl_server_flow.c �ƍs���߂���Ȃ��ƂȂ�Ȃ��_�����育���f�����������܂��c�B
 *
 * @date  2010.05.28  �쐬
 */
//=============================================================================================
#include <gflib.h>


#include "sound\pm_sndsys.h"
#include "waza_tool\wazadata.h"
#include "waza_tool\wazano_def.h"
#include "item\item.h"
#include "item\itemtype_def.h"
#include "item\itemuse_def.h"
#include "poke_tool\shinka_check.h"
#include "gamesystem\g_power.h"

#include "btl_event.h"
#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_sick.h"
#include "btl_pospoke_state.h"
#include "btl_pokeset.h"
#include "btl_shooter.h"
#include "btl_calc.h"
#include "btl_tables.h"
#include "btlv\btlv_effect.h"

#include "handler\hand_tokusei.h"
#include "handler\hand_item.h"
#include "handler\hand_waza.h"
#include "handler\hand_side.h"
#include "handler\hand_pos.h"
#include "btl_server_local.h"
#include "btl_server.h"
#include "btl_hem.h"

#include "btl_server_flow_def.h"
#include "btl_server_flow_sub.h"




/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec );
static BOOL correctTargetDead( BTL_SVFLOW_WORK* wk, BtlRule rule, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, BTL_POKESET* rec );
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx );
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
static u8 checkTemptPokeID( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 targetPokeID );
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam );
static u8 scEvent_GetWazaTargetTempt( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 defaultTargetPokeID );
static u32 getexp_calc_adjust_level( const BTL_POKEPARAM* bpp, u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv );
static inline u32 _calc_adjust_level_sub( u32 val );
static void PutDoryokuExp( BTL_POKEPARAM* bpp, const BTL_POKEPARAM* deadPoke );
static BOOL scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID );
static BOOL CalcCapturePokemon( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
    u8* yure_cnt, u8* fCritical );
static fx32 GetKusamuraCaptureRatio( BTL_SVFLOW_WORK* wk );
static fx32 CalcBallCaptureRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID );
static BOOL CheckCaptureCritical( BTL_SVFLOW_WORK* wk, fx32 capture_value );
static u8 ItemEff_SleepRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
static u8 ItemEff_Mental_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID );
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType );
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam );
static BtlResult CheckResult_AliveBoth( BTL_SVFLOW_WORK* wk, const u8* alivePokeCnt, const u8* totalPokeCnt, u8 playerSide );
static void calcHPRatio( BTL_SVFLOW_WORK* wk, int* HP_sum, int* HP_ratio, u8 side );



/*====================================================================================================*/
/*                                                                                                    */
/*  ���U�Ώۃ|�P�����i�[                                                                              */
/*                                                                                                    */
/*====================================================================================================*/

//--------------------------------------------------------------------------
/**
 * �Ώۃ|�P�����̃f�[�^�|�C���^���ABTL_POKESET�\���̂Ɋi�[
 *
 * @param   wk
 * @param   atPos
 * @param   waza
 * @param   action
 * @param   rec       [out]
 *
 * @retval  u8        �Ώۃ|�P������
 */
//--------------------------------------------------------------------------
u8 BTL_SVFSUB_RegisterTargets( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, BTL_POKESET* rec )
{
  BtlRule rule = BTL_MAIN_GetRule( wk->mainModule );

  u8 intrPokeID = BTL_POKEID_NULL;
  u8 numTarget;

  if( !(wazaParam->fMagicCoat) ){
    intrPokeID = scEvent_GetWazaTargetIntr( wk, attacker, wazaParam );
  }

  /*
  if( BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, intrPokeID) == BTL_POS_NULL ){
    intrPokeID = BTL_POKEID_NULL;
  }else{
    BTL_N_Printf( DBGSTR_SVFL_IntrTargetPoke, intrPokeID);
  }
  */

  BTL_POKESET_Clear( rec );
  wk->defaultTargetPos = BTL_POS_NULL;

  switch( rule ){
  case BTL_RULE_SINGLE:
  default:
    numTarget = registerTarget_single( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;

  case BTL_RULE_DOUBLE:
    numTarget = registerTarget_double( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;

  case BTL_RULE_TRIPLE:
    numTarget = registerTarget_triple( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    break;

  case BTL_RULE_ROTATION:
    #ifdef ROTATION_NEW_SYSTEM
    numTarget = registerTarget_single( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    #else
    numTarget = registerTarget_double( wk, attacker, targetPos, wazaParam, intrPokeID, rec );
    #endif
    break;
  }

  if( numTarget == 1 )
  {
    BTL_POKEPARAM* bpp = BTL_POKESET_Get( rec, 0 );
    if( bpp ){
      wk->defaultTargetPos = BTL_POSPOKE_GetPokeLastPos( &wk->pospokeWork, BPP_GetID(bpp) );
//      TAYA_Printf(" �P�̃^�[�Q�b�g���U�̓f�t�H���g�ʒu���L�^:�ʒu=%d, poke=%d\n", wk->defaultTargetPos, BPP_GetID(bpp) );
    }

    if( correctTargetDead(wk, rule, attacker, wazaParam, targetPos, rec) == FALSE ){
      numTarget = 0;
    }
  }

  return numTarget;
}
/**
 *  ���U�Ώۊi�[�F�V���O���p
 */
static u8 registerTarget_single( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  switch( wazaParam->targetType ){
  case WAZA_TARGET_OTHER_SELECT:  ///< �����ȊO�̂P�́i�I���j
  case WAZA_TARGET_ENEMY_SELECT:  ///< �G�P�́i�I���j
  case WAZA_TARGET_ENEMY_RANDOM:  ///< �G�����_��
  case WAZA_TARGET_ENEMY_ALL:     ///< �G���Q��
  case WAZA_TARGET_OTHER_ALL:     ///< �����ȊO�S��
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_USER:                ///< �����P�̂̂�
  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P��
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, attacker );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_ALL:
    BTL_POKESET_Add( rec, attacker );
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    return 2;

  case WAZA_TARGET_UNKNOWN:
    if( intrPokeID != BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
      return 1;
    }else{
      return 0;
    }
    break;

  default:
    return 0;
  }
}
/**
 *  ���U�Ώۊi�[�F�_�u���p
 */
static u8 registerTarget_double( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );

  BTL_POKEPARAM* bpp = NULL;

  BTL_N_Printf( DBGSTR_SVFS_RegTargetDouble, wazaParam->targetType, BPP_GetID(attacker), targetPos);

  switch( wazaParam->targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< �����ȊO�̂P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< �G�P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< �G�����_���ɂP��
    bpp = get_opponent_pokeparam( wk, atPos, BTL_CALC_GetRand(2) );
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< �G���S��
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 0) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam(wk, atPos, 1) );
    return 2;
  case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�S��
    BTL_POKESET_Add( rec, get_next_pokeparam( wk, atPos ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
    return 3;
  case WAZA_TARGET_ALL:                ///< �S��
    BTL_POKESET_Add( rec, attacker );
    BTL_POKESET_Add( rec, get_next_pokeparam( wk, atPos ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 0 ) );
    BTL_POKESET_Add( rec, get_opponent_pokeparam( wk, atPos, 1 ) );
    return 3;

  case WAZA_TARGET_USER:      ///< �����P�̂̂�
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, attacker );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P�́i�I���j
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����P��
    BTL_POKESET_Add( rec, get_next_pokeparam(wk, atPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    if( intrPokeID != BTL_POKEID_NULL )
    {
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    else{
      return 0;
    }
    break;

  default:
    return 0;
  }

  // �Ώۂ��G�܂ނP�̂̃��U�Ȃ炱���܂ŗ���
  if( bpp )
  {
    u8 targetPokeID = BPP_GetID( bpp );
    u8 temptPokeID = checkTemptPokeID( wk, attacker, wazaParam, targetPokeID );
    if( temptPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, temptPokeID );
    }
    BTL_N_Printf( DBGSTR_SVFL_DoubleTargetRegister, BPP_GetID(bpp) );
    BTL_POKESET_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
  }
}
/**
 *  ���U�Ώۊi�[�F�g���v���p
 */
static u8 registerTarget_triple( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* attacker, BtlPokePos targetPos,
  const SVFL_WAZAPARAM* wazaParam, u8 intrPokeID, BTL_POKESET* rec )
{
  BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
  const BTL_TRIPLE_ATTACK_AREA* area = BTL_MAINUTIL_GetTripleAttackArea( atPos );
  u32 i, cnt;

  BTL_POKEPARAM* bpp = NULL;

  BTL_N_Printf( DBGSTR_SVFS_RegTargetDouble, wazaParam->targetType, BPP_GetID(attacker), targetPos);


  switch( wazaParam->targetType ){
  case WAZA_TARGET_OTHER_SELECT:        ///< �����ȊO�̂P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_SELECT:        ///< �G�P�́i�I���j
    bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, targetPos );
    break;
  case WAZA_TARGET_ENEMY_RANDOM:        ///< �G�����_���ɂP��
    {
      u8 r = BTL_CALC_GetRand( area->numEnemys );
      bpp = BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[r] );
    }
    break;

  case WAZA_TARGET_ENEMY_ALL:           ///< �G���S��
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData( wk->pokeCon, area->enemyPos[i]) );
    }
    return area->numEnemys;

  case WAZA_TARGET_OTHER_ALL:           ///< �����ȊO�S��
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      if( area->friendPos[i] == atPos ){ continue; }
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_ALL:                ///< �S��
    cnt = 0;
    for(i=0; i<area->numEnemys; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->enemyPos[i]) );
      ++cnt;
    }
    for(i=0; i<area->numFriends; ++i){
      BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, area->friendPos[i]) );
      ++cnt;
    }
    return cnt;

  case WAZA_TARGET_USER:      ///< �����P�̂̂�
    if( intrPokeID == BTL_POKEID_NULL ){
      BTL_POKESET_Add( rec, attacker );
    }else{
      BTL_POKESET_Add( rec, BTL_POKECON_GetPokeParam(wk->pokeCon, intrPokeID) );
    }
    return 1;

  case WAZA_TARGET_FRIEND_USER_SELECT:  ///< �������܂ޖ����P�́i�I���j
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;
  case WAZA_TARGET_FRIEND_SELECT:       ///< �����ȊO�̖����P��
    BTL_POKESET_Add( rec, BTL_POKECON_GetFrontPokeData(wk->pokeCon, targetPos) );
    return 1;

  case WAZA_TARGET_UNKNOWN:
    if( intrPokeID != BTL_POKEID_NULL )
    {
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, intrPokeID );
    }
    else{
      return 0;
    }
    break;

  default:
    return 0;
  }

  // �Ώۂ��G�܂ނP�̂̃��U�Ȃ炱���܂ŗ���
  if( bpp )
  {
    u8 targetPokeID = BPP_GetID( bpp );
    u8 temptPokeID = checkTemptPokeID( wk, attacker, wazaParam, targetPokeID );
    if( temptPokeID != BTL_POKEID_NULL ){
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, temptPokeID );
    }
    BTL_N_Printf( DBGSTR_SVFL_DoubleTargetRegister, BPP_GetID(bpp) );
    BTL_POKESET_Add( rec, bpp );
    return 1;
  }
  else
  {
    return 0;
  }
}
/**
 *  �Ώۃ|�P����������ł������̑ΏۏC��
 *
 *  @retval   BOOL �␳�ł�����TRUE�^�␳�Ώی��ɂ��|�P���������Ȃ��ꍇFALSE
 */
static BOOL correctTargetDead( BTL_SVFLOW_WORK* wk, BtlRule rule, const BTL_POKEPARAM* attacker,
  const SVFL_WAZAPARAM* wazaParam, BtlPokePos targetPos, BTL_POKESET* rec )
{
  // �퓬�ʒu���w�c������Q�ȏ�i�_�u���A�g���v���A���[�e�j
  u8 numFrontPos = BTL_RULE_GetNumFrontPos( rule );
  if( numFrontPos > 1 )
  {
    // ���X�̑Ώۂ��P�̂ŁA
    if( BTL_POKESET_GetCount(rec) == 1 )
    {
      // �U���|�P�̓G�ŁA������ł���
      BTL_POKEPARAM* defaulTarget = BTL_POKESET_Get( rec, 0 );
      if( BPP_IsDead(defaulTarget)
      &&  !BTL_MAINUTIL_IsFriendPokeID( BPP_GetID(attacker), BPP_GetID(defaulTarget) )
      ){
        u8  nextTargetPos[ BTL_POS_MAX ];
        BTL_POKEPARAM* nextTarget[ BTL_POSIDX_MAX ];
        BtlPokePos  atPos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(attacker) );
        BtlExPos    exPos;
        u8 nextTargetCnt, aliveCnt, i;

        BTL_N_Printf( DBGSTR_SVFL_CorrectTarget_Info,
          BPP_GetID(attacker), atPos, BPP_GetID(defaulTarget), targetPos);

      // ���X�̑Ώۂ͎���ł�̂ŏ��O���Ă���
        BTL_POKESET_Remove( rec, defaulTarget );

      // ���X�̑Ώ۔͈͂��
        if( WAZADATA_GetFlag(wazaParam->wazaID, WAZAFLAG_TripleFar) ){
          exPos = EXPOS_MAKE( BTL_EXPOS_FULL_ENEMY, atPos );
          BTL_N_Printf( DBGSTR_SVFL_CorrectHitFarOn, wazaParam->wazaID);
        }else{
          exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, atPos );
          BTL_N_Printf( DBGSTR_SVFL_CorrectHitFarOff, wazaParam->wazaID);
        }
        nextTargetCnt = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, nextTargetPos );

      // ���X�̑Ώ۔͈͂��琶���Ă�|�P���
        for(i=0; i<NELEMS(nextTarget); ++i){
          nextTarget[i] = NULL;
        }
        aliveCnt = 0;
        for(i=0; i<nextTargetCnt; ++i)
        {
          nextTarget[aliveCnt] = BTL_POKECON_GetFrontPokeData( wk->pokeCon, nextTargetPos[i] );
          if( !BPP_IsDead(nextTarget[aliveCnt]) ){
            nextTargetPos[ aliveCnt ] = nextTargetPos[i]; // �ʒu����Index�𑵂��Ă���
            ++aliveCnt;
          }
        }

        BTL_N_Printf( DBGSTR_SVFL_CorrectTargetNum, aliveCnt, nextTargetCnt );
        if( aliveCnt == 0 ){
          return FALSE;
        }

        // �Ώۂ��Q�̂���Ȃ���΂P�̂̃n�Y�Ȃ̂ŁA��⃊�X�g�̐擪��_��
        if( aliveCnt != 2 ){
          i = 0;
        }
        // ��₪�Q�̂̏ꍇ�i�g���v���̏ꍇ�̂݋N���肤��j
        else
        {
          // ��{�́A���X�̑Ώۂɋ߂�����_��
          int diff_0 = BTL_CALC_ABS( (int)(nextTargetPos[0]) - (int)targetPos );
          int diff_1 = BTL_CALC_ABS( (int)(nextTargetPos[1]) - (int)targetPos );

          BTL_N_Printf( DBGSTR_SVFL_CorrectTargetDiff,
            BPP_GetID(nextTarget[0]), diff_0, BPP_GetID(nextTarget[1]), diff_1);

          // ���X�̑ΏۂƂ̋����������ꍇ�i�^�񒆂�_�����ꍇ�̂݋N���肤��j�AHP�̏��Ȃ�����_��
          if( diff_0 == diff_1 )
          {
            diff_0 = BPP_GetValue( nextTarget[0], BPP_HP );
            diff_1 = BPP_GetValue( nextTarget[1], BPP_HP );
            BTL_N_Printf( DBGSTR_SVFL_CorrectTargetHP,
              BPP_GetID(nextTarget[0]), diff_0, BPP_GetID(nextTarget[1]), diff_1);

          }

          if( diff_0 < diff_1 ){
            i = 0;
          }else if( diff_1 < diff_0 ){
            i = 1;
          }else{
            // HP�������Ȃ烉���_��
            i = BTL_CALC_GetRand( 2 );
          }
        }

        if( nextTarget[i] != NULL )
        {
          BTL_N_Printf( DBGSTR_SVFL_CorrectResult,
            nextTargetPos[i], BPP_GetID(nextTarget[i]) );
          BTL_POKESET_Add( rec, nextTarget[ i ] );
          return TRUE;
        }
      }
    }
  }
  return FALSE;
}
/**
 * �w��ʒu���猩�đΐ푊��̃|�P�����f�[�^��Ԃ�
 */
static BTL_POKEPARAM* get_opponent_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos, u8 pokeSideIdx )
{
  SVCL_WORK* clwk;
  BtlPokePos targetPos;
  u8 clientID, posIdx;

  targetPos = BTL_MAIN_GetOpponentPokePos( wk->mainModule, pos, pokeSideIdx );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, targetPos, &clientID, &posIdx );

  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
}
/**
 * �w��ʒu���猩�ėׂ̃|�P�����f�[�^��Ԃ�
 */
static BTL_POKEPARAM* get_next_pokeparam( BTL_SVFLOW_WORK* wk, BtlPokePos pos )
{
  SVCL_WORK* clwk;
  BtlPokePos nextPos;
  u8 clientID, posIdx;

  nextPos = BTL_MAIN_GetNextPokePos( wk->mainModule, pos );
  BTL_MAIN_BtlPosToClientID_and_PosIdx( wk->mainModule, nextPos, &clientID, &posIdx );
  clwk = BTL_SERVER_GetClientWork( wk->server, clientID );

  return BTL_PARTY_GetMemberData( clwk->party, posIdx );
}

static u8 checkTemptPokeID( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 targetPokeID )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  u8 temptPokeID = scEvent_GetWazaTargetTempt( wk, attacker, wazaParam, targetPokeID );
  BTL_SVF_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
  return temptPokeID;
}

//----------------------------------------------------------------------------------
/**
 * [Event] ���U�^�[�Q�b�g����������i�U�����̃n���h���ɂ��j
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaTargetIntr( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam )
{
  u8 pokeID = BTL_POKEID_NULL;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_POKEID_DEF, pokeID );
    BTL_EVENTVAR_SetRewriteOnceValue( BTL_EVAR_FIX_FLAG, FALSE );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_DECIDE_TARGET );
    pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();
  return pokeID;
}
//----------------------------------------------------------------------------------
/**
 * [Event] ���U�^�[�Q�b�g���������������i��т��ӂ铙�A�h�䑤�̃n���h���ɂ��j
 *
 * @param   wk
 * @param   attacker
 * @param   wazaParam
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 scEvent_GetWazaTargetTempt( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* attacker, const SVFL_WAZAPARAM* wazaParam, u8 defaultTargetPokeID )
{
  u8 pokeID;
  BTL_EVENTVAR_Push();
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_POKEID_ATK, BPP_GetID(attacker) );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZA_TYPE, wazaParam->wazaType );
    BTL_EVENTVAR_SetConstValue( BTL_EVAR_WAZAID, wazaParam->wazaID );
    BTL_EVENTVAR_SetValue( BTL_EVAR_POKEID_DEF, defaultTargetPokeID );
    BTL_EVENT_CallHandlers( wk, BTL_EVENT_TEMPT_TARGET );
    pokeID = BTL_EVENTVAR_GetValue( BTL_EVAR_POKEID_DEF );
  BTL_EVENTVAR_Pop();
  if( pokeID != defaultTargetPokeID ){
    return pokeID;
  }
  return BTL_POKEID_NULL;
}




/*====================================================================================================*/
/*                                                                                                    */
/*  �o���l�v�Z����                                                                                    */
/*                                                                                                    */
/*====================================================================================================*/

//----------------------------------------------------------------------------------
/**
 * �o���l�v�Z���ʂ��p���[�N�ɏo��
 *
 * @param   wk
 * @param   party     �v���C���[���p�[�e�B
 * @param   deadPoke  ���񂾃|�P����
 * @param   result    [out] �v�Z���ʊi�[��
 */
//----------------------------------------------------------------------------------
void BTL_SVFSUB_CalcExp( BTL_SVFLOW_WORK* wk, BTL_PARTY* party, const BTL_POKEPARAM* deadPoke, CALC_EXP_WORK* result )
{
  u32 baseExp = BTL_CALC_CalcBaseExp( deadPoke );
  u16 memberCount  = BTL_PARTY_GetMemberCount( party );
  u16 gakusyuCount = 0;

  const BTL_POKEPARAM* bpp;
  u16 i;

  // �g���[�i�[���1.5�{
  if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_TRAINER ){
    baseExp = (baseExp * 15) / 10;
  }

  // ���[�N�N���A
  for(i=0; i<BTL_PARTY_MEMBER_MAX; ++i){
    GFL_STD_MemClear( &result[i], sizeof(CALC_EXP_WORK) );
  }

  // �������イ���������̌o���l�v�Z
  for(i=0; i<memberCount; ++i)
  {
    // �������イ�������𑕔����Ă���|�P�������Ώ�
    bpp = BTL_PARTY_GetMemberDataConst( party, i );
    if( !BPP_IsDead(bpp)
    &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
    ){
      ++gakusyuCount;
    }
  }
  if( gakusyuCount )
  {
    u32 gakusyuExp = (baseExp / 2);
    baseExp -= gakusyuExp;

    gakusyuExp /= gakusyuCount;
    if( gakusyuExp == 0 ){
      gakusyuExp = 1;
    }

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp)
      &&  (BPP_GetItem(bpp) == ITEM_GAKUSYUUSOUTI)
      ){
        result[i].exp = gakusyuExp;
      }
    }
  }

  BTL_N_Printf( DBGSTR_SVFL_ExpCalc_Base, baseExp );


  // �ΐ�o���l�擾
  {
    u8 confrontCnt = BPP_CONFRONT_REC_GetCount( deadPoke );
    u8 aliveCnt = 0;
    u8 pokeID;

    u32 addExp;

    for(i=0; i<confrontCnt; ++i)
    {
      pokeID = BPP_CONFRONT_REC_GetPokeID( deadPoke, i );
      bpp = BTL_POKECON_GetPokeParam( wk->pokeCon, pokeID );
      if( !BPP_IsDead(bpp) ){
        ++aliveCnt;
      }
    }
    BTL_N_Printf( DBGSTR_SVFL_ExpCalc_MetInfo, BPP_GetID(deadPoke), confrontCnt, aliveCnt);

    addExp = baseExp / aliveCnt;
    if( addExp == 0 ){
      addExp = 1;
    }

    for(i=0; i<memberCount; ++i)
    {
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      if( !BPP_IsDead(bpp) )
      {
        u8 j;
        pokeID = BPP_GetID( bpp );
        for(j=0; j<confrontCnt; ++j)
        {
          if( BPP_CONFRONT_REC_GetPokeID(deadPoke, j) == pokeID ){
            BTL_N_Printf( DBGSTR_SVFL_ExpCalc_DivideInfo, i, addExp);
            result[i].exp += addExp;
          }
        }
      }
    }
  }

  // �{�[�i�X�v�Z
  for(i=0; i<memberCount; ++i)
  {
    if( result[i].exp )
    {
      const MYSTATUS* status = BTL_MAIN_GetPlayerStatus( wk->mainModule );
      const POKEMON_PARAM* pp;
      u16 myLv, enemyLv;
      bpp = BTL_PARTY_GetMemberDataConst( party, i );
      pp = BPP_GetSrcData( bpp );

      // �|�����|�P�Ƃ̃��x�����ɂ���Čo���l����������iWB����ǉ����ꂽ�d�l�j
      myLv = BPP_GetValue( bpp, BPP_LEVEL );
      enemyLv = BPP_GetValue( deadPoke, BPP_LEVEL );

      result[i].exp = getexp_calc_adjust_level( bpp, result[i].exp, myLv, enemyLv );

      // ���l���e�Ȃ�{�[�i�X
      if( !PP_IsMatchOya(pp, status) )
      {
        // �O���̐e�Ȃ�x1.7�C��������Ȃ����x1.5
        fx32 ratio;
        ratio = ( PP_Get(pp, ID_PARA_country_code, NULL) != MyStatus_GetRegionCode(status) )?
            FX32_CONST(1.7f) : FX32_CONST(1.5f);

        result[i].exp = BTL_CALC_MulRatio( result[i].exp, ratio );
        result[i].fBonus = TRUE;
      }

      // �����킹�^�}�S�����Ă���{�[�i�X x1.5
      if( BPP_GetItem(bpp) == ITEM_SIAWASETAMAGO ){
        result[i].exp = BTL_CALC_MulRatio( result[i].exp, FX32_CONST(1.5f) );
        result[i].fBonus = TRUE;
      }

      // �f�p���[�␳
      result[i].exp = GPOWER_Calc_Exp( result[i].exp );

      BTL_N_Printf( DBGSTR_SVFL_ExpCalc_Result, i, result[i].exp);
    }
  }

  // �w�͒l�v�Z
  for(i=0; i<memberCount; ++i)
  {
    if( result[i].exp )
    {
      BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberData( party, i );
      PutDoryokuExp( bpp, deadPoke );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �|���ꂽ�|�P�����ƌo���l���擾����|�P�����̃��x�����ɉ����Čo���l��␳����i�v�a���j
 *
 * @param   base_exp      ��{�o���l�iGS�܂ł̎d�l�ɂ��v�Z���ʁj
 * @param   getpoke_lv    �o���l���擾����|�P�����̃��x��
 * @param   deadpoke_lv   �|���ꂽ�|�P�����̃��x��
 *
 * @retval  u32   �␳��o���l
 */
//----------------------------------------------------------------------------------
static u32 getexp_calc_adjust_level( const BTL_POKEPARAM* bpp, u32 base_exp, u16 getpoke_lv, u16 deadpoke_lv )
{
  u32 denom, numer;
  u32  denom_int, numer_int, result, expMargin;

  numer_int = deadpoke_lv * 2 + 10;
  denom_int = deadpoke_lv + getpoke_lv + 10;

  numer = _calc_adjust_level_sub( numer_int );
  denom = _calc_adjust_level_sub( denom_int );

  result = ((base_exp * numer) / denom) + 1;

  expMargin = BPP_GetExpMargin( bpp );
  if( result > expMargin ){
    result = expMargin;
  }

  BTL_N_Printf( DBGSTR_SVFL_ExpAdjustCalc,
      getpoke_lv, deadpoke_lv, numer, denom, base_exp, result );

  return result;
}
/**
 *  �o���l�␳�v�Z�T�u(2.5��j
 */
static inline u32 _calc_adjust_level_sub( u32 val )
{
  fx32  fx_val, fx_sqrt;
  u64 result;

  fx_val = FX32_CONST( val );
  fx_sqrt = FX_Sqrt( fx_val );
  val *= val;
  result = (val * fx_sqrt) >> FX32_SHIFT;

  return result;
}
/**
 *  �w�͒l�v�Z�A���ʂ�Src�f�[�^�ɔ��f������
 */
static void PutDoryokuExp( BTL_POKEPARAM* bpp, const BTL_POKEPARAM* deadPoke )
{
  /**
   *  �X�e�[�^�XIndex
   */
  enum {
    _HP = 0,
    _POW,
    _DEF,
    _AGI,
    _SPOW,
    _SDEF,
    _PARAM_MAX,
  };

  /**
   *  �֘A�p�����[�^�i��L�X�e�[�^�XIndex���j
   */
  static const struct {
    u8   personalParamID;   ///< �p�[�\�i�����^�w�͒lID
    u16  pokeParamID;       ///< POKEMON_PARAM �w�͒lID
    u16  boostItemID;       ///< �w�͒l���Z�A�C�e��ID
  }RelationTbl[] = {

    { POKEPER_ID_pains_hp,      ID_PARA_hp_exp,     ITEM_PAWAAUEITO   },  // HP
    { POKEPER_ID_pains_pow,     ID_PARA_pow_exp,    ITEM_PAWAARISUTO  },  // �U��
    { POKEPER_ID_pains_def,     ID_PARA_def_exp,    ITEM_PAWAABERUTO  },  // �h��
    { POKEPER_ID_pains_agi,     ID_PARA_agi_exp,    ITEM_PAWAAANKURU  },  // �f����
    { POKEPER_ID_pains_spepow,  ID_PARA_spepow_exp, ITEM_PAWAARENZU   },  // ���U
    { POKEPER_ID_pains_spedef,  ID_PARA_spedef_exp, ITEM_PAWAABANDO   },  // ���h

  };

  u16 mons_no = BPP_GetMonsNo( deadPoke );
  u16 form_no = BPP_GetValue( deadPoke, BPP_FORM );

  u8  exp[ _PARAM_MAX ];
  u8  i;

  // ��{�w�͒l���p�[�\�i������擾
  for(i=0; i<_PARAM_MAX; ++i){
    exp[ i ] = POKETOOL_GetPersonalParam( mons_no, form_no, RelationTbl[i].personalParamID );
  }

  // ���傤�����M�v�X�Ŕ{
  if( BPP_GetItem(bpp) == ITEM_KYOUSEIGIPUSU )
  {
    for(i=0; i<_PARAM_MAX; ++i){
      exp[ i ] *= 2;
    }
  }

  // �e��A�w�͒l�����A�C�e���������Z
  for(i=0; i<_PARAM_MAX; ++i)
  {
    if( BPP_GetItem(bpp) == RelationTbl[i].boostItemID ){
      exp[ i ] += BTL_CALC_ITEM_GetParam( RelationTbl[i].boostItemID, ITEM_PRM_ATTACK );
    }
  }

  // Src�f�[�^�ɔ��f
  {
    POKEMON_PARAM* pp = (POKEMON_PARAM*)BPP_GetSrcData( bpp );
    BOOL fFastMode = PP_FastModeOn( pp );

      // �|�P���X�Ŕ{
      if( PP_Get(pp, ID_PARA_pokerus, NULL) )
      {
        for(i=0; i<_PARAM_MAX; ++i){
          exp[ i ] *= 2;
        }
      }

      // �w�͒l���v���擾
      for(i=0; i<_PARAM_MAX; ++i)
      {
        if( exp[i] )
        {
          u32 sum = exp[i] + PP_Get( pp, RelationTbl[i].pokeParamID, NULL );
          if( sum > PARA_EXP_MAX ){
            sum = PARA_EXP_MAX;
          }
          PP_Put( pp, RelationTbl[i].pokeParamID, sum );
        }
      }

    PP_FastModeOff( pp, fFastMode );
  }
}


/*====================================================================================================*/
/*                                                                                                    */
/*  �g���[�i�[�A�C�e���g�p                                                                            */
/*                                                                                                    */
/*====================================================================================================*/
TrItemResult BTL_SVFSUB_TrainerItemProc( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, u8 actParam, u8 targetIdx )
{
  typedef u8 (*pItemEffFunc)( BTL_SVFLOW_WORK*, BTL_POKEPARAM*, u16, int, u8 );

  static const struct {
    u16            effect;
    pItemEffFunc   func;
  }ItemParallelEffectTbl[] = {
    { ITEM_PRM_SLEEP_RCV,     ItemEff_SleepRcv      },   // �����
    { ITEM_PRM_POISON_RCV,    ItemEff_PoisonRcv     },   // �ŉ�
    { ITEM_PRM_BURN_RCV,      ItemEff_YakedoRcv     },   // �Ώ���
    { ITEM_PRM_ICE_RCV,       ItemEff_KooriRcv      },   // �X��
    { ITEM_PRM_PARALYZE_RCV,  ItemEff_MahiRcv       },   // ��჉�
    { ITEM_PRM_PANIC_RCV,     ItemEff_KonranRcv     },   // ������
    { ITEM_PRM_MEROMERO_RCV,  ItemEff_MeromeroRcv   },   // ����������
    { ITEM_PRM_ABILITY_GUARD, ItemEff_EffectGuard   },   // �\�̓K�[�h
    { ITEM_PRM_DEATH_RCV,     ItemEff_Relive        },   // �m����
    { ITEM_PRM_ATTACK_UP,     ItemEff_AttackRank    },   // �U���̓A�b�v
    { ITEM_PRM_DEFENCE_UP,    ItemEff_DefenceRank   },   // �h��̓A�b�v
    { ITEM_PRM_SP_ATTACK_UP,  ItemEff_SPAttackRank  },   // ���U�A�b�v
    { ITEM_PRM_SP_DEFENCE_UP, ItemEff_SPDefenceRank },   // ���h�A�b�v
    { ITEM_PRM_AGILITY_UP,    ItemEff_AgilityRank   },   // �f�����A�b�v
    { ITEM_PRM_HIT_UP,        ItemEff_HitRank       },   // �������A�b�v
    { ITEM_PRM_CRITICAL_UP,   ItemEff_CriticalUp    },   // �N���e�B�J�����A�b�v
    { ITEM_PRM_PP_RCV,        ItemEff_PP_Rcv        },   // PP��
    { ITEM_PRM_ALL_PP_RCV,    ItemEff_AllPP_Rcv     },   // PP�񕜁i�S�Ă̋Z�j
    { ITEM_PRM_HP_RCV,        ItemEff_HP_Rcv        },   // HP��
  };

  static const struct {
    u16           itemID;
    pItemEffFunc  func;
  }ShooterItemParam[] = {
    { ITEM_AITEMUKOORU,     ShooterEff_ItemCall  },
    { ITEM_SUKIRUKOORU,     ShooterEff_SkillCall },
    { ITEM_AITEMUDOROPPU,   ShooterEff_ItemDrop  },
    { ITEM_HURATTOKOORU,    ShooterEff_FlatCall  },
  };

  u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( BPP_GetID(bpp) );
  u8 targetPokeID, fShooterEffective;
  BTL_POKEPARAM* target = NULL;
  int i, itemParam;
  u32 hem_state;
  BtlPokePos targetPos;

  // @todo targetIdx = ���p�[�e�B�̉��Ԗڂ̃|�P�����Ɏg�����H�Ƃ����Ӗ������A�}���`�ł��L�����Ƙb������Ă��܂�
  if( targetIdx != BTL_PARTY_MEMBER_MAX ){
    BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, clientID );
    target = BTL_PARTY_GetMemberData( party, targetIdx );
    targetPokeID = BPP_GetID( target );
    BTL_N_Printf( DBGSTR_SVFL_TrainerItemTarget, targetIdx, targetPokeID );
  }

  // �����́~�~���g�����I  ���b�Z�[�W
  fShooterEffective = FALSE;
  {
    u8 pokeID = BPP_GetID( bpp );
    u8 clientID = BTL_MAINUTIL_PokeIDtoClientID( pokeID );

    // �ʏ�o�b�O�p
    if( wk->bagMode != BBAG_MODE_SHOOTER )
    {
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_UseItem_Self, clientID, itemID );

      if( clientID == BTL_MAIN_GetPlayerClientID(wk->mainModule) ){
        BTL_MAIN_RECORDDATA_Inc( wk->mainModule, RECID_USE_SHOOTER_COUNT );
      }
    }else
    {
      // �V���[�^�[�p
      BtlPokePos pos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, targetPokeID );
      if( BTL_MAIN_GetRule(wk->mainModule) == BTL_RULE_ROTATION ){
        if( (pos != BTL_POS_NULL) && (pos > BTL_POS_2ND_0) ){
          pos = BTL_POS_NULL;
        }
      }
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_UseItem_Shooter, clientID, targetPokeID, itemID );
      if( pos != BTL_POS_NULL ){
        SCQUE_PUT_ACT_EffectByPos( wk->que, pos, BTLEFF_SHOOTER_EFFECT );
        fShooterEffective = TRUE;
      }
    }
  }

  // �{�[�������Ȃ�{�[�������V�[�P���X��
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_ITEM_TYPE) == ITEMTYPE_BALL )
  {
    if( scproc_TrainerItem_BallRoot( wk, bpp, itemID ) ){
      BTL_MAIN_DecrementPlayerItem( wk->mainModule, clientID, itemID );
    }
    return TRITEM_RESULT_NORMAL;
  }

  // �����A�C�e���Ȃ瓦���V�[�P���X��
  if( BTL_CALC_ITEM_GetParam(itemID, ITEM_PRM_BATTLE) == ITEMUSE_BTL_ESCAPE )
  {
//    BTL_SVFRET_UseEscapeItem( wk, bpp );
    return TRITEM_RESULT_ESCAPE;
  }

  // �ΏۈʒuID
  targetPos = BTL_POSPOKE_GetPokeExistPos( &wk->pospokeWork, targetPokeID );

  // �V���[�^�[�Ǝ��̃A�C�e������
  for(i=0; i<NELEMS(ShooterItemParam); ++i)
  {
    if( itemID == ShooterItemParam[i].itemID )
    {
      if( fShooterEffective )
      {
        if( ShooterItemParam[i].func( wk, target, itemID, 0, actParam ) )
        {
          if( targetPos != BTL_POS_NULL ){
            SCQUE_PUT_ACT_EffectByPos( wk->que, targetPos, BTLEFF_USE_ITEM );
          }
        }
        else{
          SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_UseItem_NoEffect );
        }
      }
      else
      {
        SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_PokeNotExist, targetPokeID );
      }
      return TRITEM_RESULT_NORMAL;
    }
  }

  // �ʏ�̃A�C�e������
  {
    BOOL fEffective = FALSE;
    hem_state = BTL_Hem_PushState( &wk->HEManager );
    for(i=0; i<NELEMS(ItemParallelEffectTbl); ++i)
    {
      itemParam = BTL_CALC_ITEM_GetParam( itemID, ItemParallelEffectTbl[i].effect );
      if( itemParam ){
        if( ItemParallelEffectTbl[i].func(wk, target, itemID, itemParam, actParam) ){
          fEffective = TRUE;
        }
      }
    }
    // �g�p���ʂ���
    if( fEffective )
    {
      if( targetPos != BTL_POS_NULL ){
        SCQUE_PUT_ACT_EffectByPos( wk->que, targetPos, BTLEFF_USE_ITEM );
      }

      BTL_SVF_HandEx_Root( wk, ITEM_DUMMY_DATA );
      if( wk->bagMode != BBAG_MODE_SHOOTER )
      {
        BTL_MAIN_DecrementPlayerItem( wk->mainModule, clientID, itemID );
        BTL_MAIN_CalcNatsukiItemUse( wk->mainModule, target, itemID );
      }
    }
    else{
      SCQUE_PUT_MSG_STD( wk->que, BTL_STRID_STD_UseItem_NoEffect );
    }
    BTL_Hem_PopState( &wk->HEManager, hem_state );
  }

  return TRITEM_RESULT_NORMAL;
}

//----------------------------------------------------------------------------------
/**
 * �{�[���g�p���[�g
 *
 * @param   wk
 * @param   bpp
 * @param   itemID
 *
 * @retval  �{�[���������ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL scproc_TrainerItem_BallRoot( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID )
{
  BtlPokePos  targetPos = BTL_POS_NULL;
  BTL_POKEPARAM* targetPoke = NULL;

  // ������ʒu�i�����Ă���|�P�������V�[�N���čŏ��̃q�b�g�ʒu�j�����߂�
  {
    u8 posAry[ BTL_POS_MAX ];
    u8 basePos, posCnt, i;
    BtlExPos exPos;

    basePos = BTL_MAIN_PokeIDtoPokePos( wk->mainModule, wk->pokeCon, BPP_GetID(bpp) );
    exPos = EXPOS_MAKE( BTL_EXPOS_AREA_ENEMY, basePos );
    posCnt = BTL_MAIN_ExpandBtlPos( wk->mainModule, exPos, posAry );

    for(i=0; i<posCnt; ++i)
    {
      targetPoke = BTL_POKECON_GetFrontPokeData( wk->pokeCon, posAry[i] );
      if( !BPP_IsDead(targetPoke) ){
        targetPos = posAry[i];
        break;
      }
    }
  }

  // �쐶�� = �ʏ폈��
  if( BTL_MAIN_GetCompetitor(wk->mainModule) == BTL_COMPETITOR_WILD )
  {
    if( targetPos != BTL_POS_NULL )
    {
      u8 yure_cnt, fSuccess, fCritical, fZukanRegister;

      fSuccess = CalcCapturePokemon( wk, bpp, targetPoke, itemID, &yure_cnt, &fCritical );
//      OS_TPrintf("�ߊl .. critical=%d\n", fCritical);

      if( fSuccess )
      {
        wk->flowResult = SVFLOW_RESULT_POKE_GET;
        wk->getPokePos = targetPos;
        fZukanRegister = !BTL_MAIN_IsZukanRegistered( wk->mainModule, targetPoke );
        BPP_SetCaptureBallID( targetPoke, itemID );
      }
      else{
        fZukanRegister = FALSE;
      }

      SCQUE_PUT_ACT_BallThrow( wk->que, targetPos, yure_cnt, fSuccess, fZukanRegister, fCritical, itemID );
    }

    return TRUE;
  }
  // �g���[�i�[��ł͎��s�i�{�[���͌���Ȃ��j
  else
  {
    if( targetPos != BTL_POS_NULL ){
      SCQUE_PUT_ACT_BallThrowTrainer( wk->que, targetPos, itemID );
    }
    return FALSE;
  }
}
//----------------------------------------------------------------------------------
/**
 * �{�[���������ʌv�Z
 *
 * @param   wk
 * @param   myPoke
 * @param   targetPoke
 * @param   ballID
 * @param   yure_cnt    [out] �h�ꉉ�o�̔�����
 * @param   fCritical   [out] �N���e�B�J���t���O
 *
 * @retval  BOOL    �ߊl�ł���ꍇ��TRUE
 */
//----------------------------------------------------------------------------------
static BOOL CalcCapturePokemon( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke,u16 ballID,
    u8* yure_cnt, u8* fCritical )
{
  *fCritical = FALSE;

  if( ballID == ITEM_MASUTAABOORU ){
    *yure_cnt = BTL_CAPTURE_BALL_YURE_CNT_MAX;
    return TRUE;
  }
  else
  {
    u32 hp_base = BPP_GetValue( targetPoke, BPP_MAX_HP ) * 3;
    u32 hp_value = ( hp_base - BPP_GetValue(targetPoke, BPP_HP) * 2 );
    fx32 capture_value = FX32_CONST( hp_value );

//    OS_TPrintf("�ߊl�l�F���� %08x(%d) ..MaxHPx3=%d, HPVal=%d\n", capture_value, capture_value>>FX32_SHIFT, hp_base, hp_value );

    // ���ނ�a�␳
    if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_HIGH_LV_ENC) )
    {
      fx32 kusa_ratio = GetKusamuraCaptureRatio( wk );
      capture_value = FX_Mul( capture_value, kusa_ratio );
    }

    // �푰���Ƃ̕ߊl�l�v�Z
    {
      u16 mons_no = BPP_GetMonsNo( targetPoke );
      u16 form_no = BPP_GetValue( targetPoke, BPP_FORM );
      u16 ratio = POKETOOL_GetPersonalParam( mons_no, form_no, POKEPER_ID_get_rate );
      capture_value *= ratio;
//      OS_TPrintf("�ߊl�l�F�푰�␳ %08x(%d) ..�imonsno=%d, ratio=%d)\n",
//          capture_value, capture_value>>FX32_SHIFT, mons_no, ratio);
    }

    // �{�[���ɂ��ߊl���␳
    {
      fx32 ball_ratio = CalcBallCaptureRatio( wk, myPoke, targetPoke, ballID );
      capture_value = FX_Mul( capture_value, ball_ratio );

//      OS_TPrintf("�ߊl�l�F�{�[���␳ %08x(%d) ..�iBallID=%d, ratio=%08x)\n",
//          capture_value, capture_value>>FX32_SHIFT, ballID, ball_ratio);
    }

    capture_value /= hp_base;
//    OS_TPrintf("�ߊl�l�F��{ %08x(%d) .. (MaxHPx3 = %d)\n",
//          capture_value, capture_value>>FX32_SHIFT, hp_base);


    // ��Ԉُ�ɂ��␳
    switch( BPP_GetPokeSick(targetPoke) ){
    case POKESICK_NEMURI:
    case POKESICK_KOORI:
      capture_value = FX_Mul( capture_value, FX32_CONST(2.5) );
      break;
    case POKESICK_DOKU:
    case POKESICK_MAHI:
    case POKESICK_YAKEDO:
      capture_value = FX_Mul( capture_value, FX32_CONST(1.5) );
      break;
    }
  //  OS_TPrintf("�ߊl�l�F�ŏI %08x(%d)\n",
//          capture_value, capture_value>>FX32_SHIFT);

    // �f�p���[�␳
    capture_value = GPOWER_Calc_Capture( capture_value );

    // �N���e�B�J���`�F�b�N
    *fCritical = CheckCaptureCritical( wk, capture_value );

    if( capture_value >= FX32_CONST(255) ){
      *yure_cnt = (*fCritical)? 1 : BTL_CAPTURE_BALL_YURE_CNT_MAX;
      return TRUE;
    }
    else
    {
      s32 check_value;
      u32 check_count, captured_count, i;

      check_count = (*fCritical)? 1 : BTL_CAPTURE_BALL_YURE_CNT_MAX;

      capture_value = FX_Div( FX32_CONST(255), capture_value );
      capture_value = FX_Sqrt( capture_value );
      capture_value = FX_Sqrt( capture_value );

      capture_value = FX_Div( FX32_CONST(65536), capture_value );
      check_value = FX_Whole( capture_value );
//      OS_TPrintf("CheckValue=%08x(%d)\n", capture_value, check_value);
      *yure_cnt = 0;
      for(i=0; i<check_count; ++i)
      {
        if( BTL_CALC_GetRand(65536) < check_value ){
          ++(*yure_cnt);
        }else{
          return FALSE;
        }
      }
      return TRUE;
    }
  }
}
/**
 *  ���ނ�␳�����擾
 */
static fx32 GetKusamuraCaptureRatio( BTL_SVFLOW_WORK* wk )
{
  u32 capturedPokeCnt = BTL_MAIN_GetZukanCapturedCount( wk->mainModule );

  if( capturedPokeCnt > 600 ){
    return FX32_CONST(1);
  }
  if( capturedPokeCnt > 450 ){
    return FX32_CONST(0.9f);
  }
  if( capturedPokeCnt > 300 ){
    return FX32_CONST(0.8f);
  }
  if( capturedPokeCnt > 150 ){
    return FX32_CONST(0.7f);
  }
  if( capturedPokeCnt > 30 ){
    return FX32_CONST(0.5f);
  }

  return FX32_CONST(0.3f);

}
/**
 *  �{�[���ɂ��ߊl���v�Z
 */
static fx32 CalcBallCaptureRatio( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* myPoke, const BTL_POKEPARAM* targetPoke, u16 ballID )
{
  switch( ballID ){
  case ITEM_SUUPAABOORU:  return FX32_CONST(1.5f);
  case ITEM_HAIPAABOORU:  return FX32_CONST(2.0f);

  case ITEM_NETTOBOORU: // �l�b�g�{�[��
    if( BPP_IsMatchType(targetPoke, POKETYPE_MIZU) || BPP_IsMatchType(targetPoke, POKETYPE_MUSHI) ){
      return FX32_CONST(3);
    }
    break;

  case ITEM_DAIBUBOORU: // �l�b�g�{�[��
    if( BTL_MAIN_GetSetupStatusFlag(wk->mainModule, BTL_STATUS_FLAG_FISHING) ){
      return FX32_CONST(3.5f);
    }
    {
      const BTL_FIELD_SITUATION* fldSit = BTL_MAIN_GetFieldSituation( wk->mainModule );
      if( fldSit->bgAttr == BATTLE_BG_ATTR_WATER ){
        return FX32_CONST(3.5f);
      }
    }
    break;

  case ITEM_NESUTOBOORU: // �l�X�g�{�[��
    {
      u16 targetLevel = BPP_GetValue( targetPoke, BPP_LEVEL );

      if( targetLevel < 30 )
      {
        u16 levelDiff = 41 - targetLevel;
        if( levelDiff > 40 ){
          levelDiff = 40;
        }
        return FX32_CONST(levelDiff) / 10;
      }
    }
    break;

  case ITEM_RIPIITOBOORU: // ���s�[�g�{�[��
    if( BTL_MAIN_IsZukanRegistered(wk->mainModule, targetPoke) ){
      return FX32_CONST(3);
    }
    break;

  case ITEM_TAIMAABOORU:  // �^�C�}�[�{�[��
    {
      fx32 ratio = FX32_CONST(1) + FX32_CONST(0.3f) * wk->turnCount;
      if( ratio > FX32_CONST(4) ){
        ratio = FX32_CONST(4);
      }
      return ratio;
    }
    break;

  case ITEM_DAAKUBOORU:  // �_�[�N�{�[��
    {
      const BTL_FIELD_SITUATION* fldSit = BTL_MAIN_GetFieldSituation( wk->mainModule );
      if( (fldSit->bgType == BATTLE_BG_TYPE_CAVE) || (fldSit->bgType == BATTLE_BG_TYPE_CAVE_DARK) ){
        return FX32_CONST(3.5);
      }
      // @ todo ���ԑтɂ��Â����Ώۂ��H
//      if( (fldSit->bgType != BATTLE_BG_TYPE_ROOM) ||
    }
    break;

  case ITEM_KUIKKUBOORU:  // �N�C�b�N�{�[��
    if( wk->turnCount == 0 )
    {
      return FX32_CONST(5);
    }
    break;

  }
  return FX32_CONST(1);
}
//----------------------------------------------------------------------------------
/**
 * �ߊl�N���e�B�J�������`�F�b�N
 *
 * @param   wk
 * @param   capture_value   �v�Z��̕ߊl�ϐ��i�Œ菬�� 0�`255�j
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL CheckCaptureCritical( BTL_SVFLOW_WORK* wk, fx32 capture_value )
{
  u32 capturedPokeCnt = BTL_MAIN_GetZukanCapturedCount( wk->mainModule );

  fx32 ratio = 0;

  if( capturedPokeCnt > 600 ){
    ratio = FX32_CONST(2.5);
  }else if(  capturedPokeCnt > 450 ){
    ratio = FX32_CONST(2.0f);
  }else if(  capturedPokeCnt > 300 ){
    ratio = FX32_CONST(1.5f);
  }else if(  capturedPokeCnt > 150 ){
    ratio = FX32_CONST(1.0f);
  }else if(  capturedPokeCnt > 30 ){
    ratio = FX32_CONST(0.5f);
  }else{
    return FALSE;
  }

  if( capture_value > FX32_CONST(255) ){
    capture_value = FX32_CONST(255);
  }
  capture_value = FX_Mul( capture_value, ratio ) / 6;
  if( BTL_CALC_GetRand(256) < FX_Whole(capture_value) )
  {
    return TRUE;
  }
  return FALSE;
}



// �A�C�e�����ʁF�˂ނ��
static u8 ItemEff_SleepRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_NEMURI );
}
static u8 ItemEff_PoisonRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_DOKU );
}
static u8 ItemEff_YakedoRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_YAKEDO );
}
static u8 ItemEff_KooriRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KOORI );
}
static u8 ItemEff_MahiRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_MAHI );
}
static u8 ItemEff_KonranRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Cure( wk, bpp, itemID, itemParam, WAZASICK_KONRAN );
}
static u8 ItemEff_MeromeroRcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Mental_Cure( wk, bpp, itemID, itemParam, WAZASICK_MEROMERO );
}
static u8 ItemEff_EffectGuard( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BtlSide side = BTL_MAINUTIL_PokeIDtoSide( BPP_GetID(bpp) );
  BPP_SICK_CONT cont = BPP_SICKCONT_MakeTurn( 5 );
  if( BTL_HANDLER_SIDE_Add(side, BTL_SIDEEFF_SIROIKIRI, cont) )
  {
    BTL_HANDEX_PARAM_MESSAGE* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, BTL_POKEID_NULL );
    HANDEX_STR_Setup( &param->str, BTL_STRTYPE_STD, BTL_STRID_STD_SiroiKiri );
    HANDEX_STR_AddArg( &param->str, side );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_Relive( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_IsDead(bpp))
  {
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RELIVE* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RELIVE, pokeID );
    param->pokeID = pokeID;

    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 2 ); break;
		case ITEM_RECOVER_HP_QUOT:
      param->recoverHP = BTL_CALC_QuotMaxHP( bpp, 4 ); break;
    default:
      param->recoverHP = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT ); break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Relive );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

//    TAYA_Printf("�|�P����(ID:%d)�����Ԃ� -> HP=%d\n", pokeID, param->recoverHP);

    // ���^�[���ɐ����Ԃ����|�P�������L�^
    BTL_SVFRET_AddRelivePokeRecord( wk, pokeID );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_AttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_ATTACK_RANK );
}
static u8 ItemEff_DefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_DEFENCE_RANK );
}
static u8 ItemEff_SPAttackRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_ATTACK_RANK );
}
static u8 ItemEff_SPDefenceRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_SP_DEFENCE_RANK );
}
static u8 ItemEff_AgilityRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_AGILITY_RANK );
}
static u8 ItemEff_HitRank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  return ItemEff_Common_Rank( wk, bpp, itemID, itemParam, BPP_HIT_RATIO );
}
static u8 ItemEff_CriticalUp( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u8 result = FALSE;
  if( !BPP_CONTFLAG_Get(bpp, BPP_CONTFLG_KIAIDAME) )
  {
    BPP_CONTFLAG_Set( bpp, BPP_CONTFLG_KIAIDAME );
    SCQUE_PUT_OP_SetContFlag( wk->que, BPP_GetID(bpp), BPP_CONTFLG_KIAIDAME );

    result = TRUE;
  }
  if( itemParam > 1 ){
    --itemParam;
    if( BPP_AddCriticalRank(bpp, itemParam) ){
      SCQUE_PUT_OP_AddCritical( wk->que, BPP_GetID(bpp), itemParam );
      result = TRUE;
    }
  }
  if( result )
  {
    SCQUE_PUT_MSG_SET( wk->que, BTL_STRID_SET_KiaiDame, BPP_GetID(bpp) );
    return TRUE;
  }
  return FALSE;
}
static u8 ItemEff_PP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( BPP_WAZA_GetCount_Org(bpp) > actParam )
  {
    u8 pokeID = BPP_GetID( bpp );
    u8 volume = BPP_WAZA_GetPPShort_Org( bpp, actParam );
    u8 ppValue = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_PP_RCV_POINT );
    if( ppValue != ITEM_RECOVER_PP_FULL ){
      if( volume > ppValue ){
        volume = ppValue;
      }
    }
    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->volume = volume;
      param->pokeID = pokeID;
      param->wazaIdx = actParam;
      param->fDeadPokeEnable = TRUE;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_PP_Recover );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, BPP_WAZA_GetID_Org(bpp, actParam) );
      return 1;
    }
  }
  return 0;
}
static u8 ItemEff_AllPP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  BTL_HANDEX_PARAM_MESSAGE* msg_param;
  u32 cnt, volume, i;
  u8 pokeID = BPP_GetID( bpp );
  u8 ppValue = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_PP_RCV_POINT );
  u8 fEffective = FALSE;

  cnt = BPP_WAZA_GetCount_Org( bpp );

  for(i=0; i<cnt; ++i)
  {
    volume = BPP_WAZA_GetPPShort_Org( bpp, i );
    if( volume > ppValue ){
      volume = ppValue;
    }

    if( volume )
    {
      BTL_HANDEX_PARAM_PP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_PP, pokeID );
      param->wazaIdx = i;
      param->pokeID = pokeID;
      param->volume = volume;
      param->fDeadPokeEnable = TRUE;
      fEffective = TRUE;
    }
  }

  if( fEffective ){
    msg_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, pokeID );
    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_PP_AllRecover );
    HANDEX_STR_AddArg( &msg_param->str, pokeID );
  }

  return fEffective;
}
static u8 ItemEff_HP_Rcv( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( !BPP_IsHPFull(bpp)
  &&  !BPP_IsDead(bpp)
  ){
    u8 pokeID = BPP_GetID( bpp );
    BTL_HANDEX_PARAM_RECOVER_HP* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RECOVER_HP, pokeID );
    param->pokeID = pokeID;
    param->fFailCheckThru = TRUE;
    itemParam = BTL_CALC_ITEM_GetParam( itemID, ITEM_PRM_HP_RCV_POINT );
    switch( itemParam ){
    case ITEM_RECOVER_HP_FULL:
      param->recoverHP = BPP_GetValue( bpp, BPP_MAX_HP ); break;
    case ITEM_RECOVER_HP_HALF:
      param->recoverHP = BTL_CALC_QuotMaxHP(bpp, 2 ); break;
		case ITEM_RECOVER_HP_QUOT:
      param->recoverHP = BTL_CALC_QuotMaxHP(bpp, 4 ); break;
    default:
      param->recoverHP = itemParam; break;
    }

    HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_HP_Recover );
    HANDEX_STR_AddArg( &param->exStr, pokeID );

    return 1;
  }
  return 0;
}

/**
 *  ��Ԉُ펡���n����
 */
static u8 ItemEff_Common_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  if( !BPP_IsDead(bpp) )
  {
    if( BPP_CheckSick(bpp, sickID) )
    {
      BTL_HANDEX_PARAM_CURE_SICK* param;
      u8 pokeID = BPP_GetID( bpp );
      param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_CURE_SICK, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->sickCode = sickID;
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �����^���n�[�u��p
 */
static u8 ItemEff_Mental_Cure( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, WazaSick sickID )
{
  if( !BPP_IsDead(bpp) && (!BTL_POSPOKE_IsExist(&wk->pospokeWork, BPP_GetID(bpp))) )
  {
    WazaSick sickID;
    u8  result = 0;
    u32 idx = 0;
    while( 1 )
    {
      sickID = BTL_TABLES_GetMentalSickID( idx++ );
      if( sickID == WAZASICK_NULL ){
        break;
      }
      if( BPP_CheckSick(bpp, sickID) )
      {
        u8 pokeID = BPP_GetID( bpp );
        BTL_HANDEX_PARAM_CURE_SICK* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_CURE_SICK, pokeID );
        param->poke_cnt = 1;
        param->pokeID[0] = pokeID;
        param->sickCode = sickID;
        result = 1;
      }
    }
    return result;
  }
  return FALSE;

}
/**
 *  �����N�A�b�v�n����
 */
static u8 ItemEff_Common_Rank( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, BppValueID rankType )
{
  u8 pokeID = BPP_GetID( bpp );

  if( (BTL_POSPOKE_GetPokeExistPos(&wk->pospokeWork, pokeID) != BTL_POS_NULL )
  &&  !BPP_IsDead(bpp)
  ){
    if( BPP_IsRankEffectValid(bpp, rankType, itemParam) )
    {
      BTL_HANDEX_PARAM_RANK_EFFECT* param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RANK_EFFECT, pokeID );
      param->poke_cnt = 1;
      param->pokeID[0] = pokeID;
      param->rankType = rankType;
      param->rankVolume = itemParam;
      param->fAlmost = TRUE;
      return 1;
    }
  }
  else
  {
    BTL_Printf("��ɏo�Ă��Ȃ��̂Ń����N���ʂȂ�\n");
  }

  return 0;
}
/**
 *  �V���[�^�[��p�F�A�C�e���R�[��
 */
static u8 ShooterEff_ItemCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  if( !BTL_TABLES_CheckItemCallNoEffect(itemID) )
  {
    u8 pokeID = BPP_GetID( bpp );

    BTL_N_Printf( DBGSTR_SVFL_UseItemCall, BPP_GetID(bpp) );

    return BTL_SVFRET_UseItemEquip( wk, bpp );
  }
  return FALSE;
}
/**
 *  �V���[�^�[��p�F�X�L���R�[��
 */
static u8 ShooterEff_SkillCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  HandExResult  result;

  BTL_SVFRET_Event_AfterMemberIn( wk, bpp );
  result = BTL_SVF_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );

  return (result != HandExResult_NULL);
}
/**
 *  �V���[�^�[��p�F�A�C�e���h���b�v
 */
static u8 ShooterEff_ItemDrop( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u16 equipItemID = BPP_GetItem( bpp );
  if( equipItemID != ITEM_DUMMY_DATA )
  {
    u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
    {
      BTL_HANDEX_PARAM_SET_ITEM* param;
      u8 pokeID = BPP_GetID( bpp );

      param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_SET_ITEM, pokeID );
      param->pokeID = pokeID;
      param->itemID = ITEM_DUMMY_DATA;
      HANDEX_STR_Setup( &param->exStr, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_ItemDrop );
      HANDEX_STR_AddArg( &param->exStr, pokeID );
      HANDEX_STR_AddArg( &param->exStr, equipItemID );

      BTL_SVF_HandEx_Root( wk, ITEM_DUMMY_DATA );
    }
    BTL_Hem_PopState( &wk->HEManager, hem_state );
    return TRUE;
  }
  return FALSE;
}
/**
 *  �V���[�^�[��p�F�t���b�g�R�[��
 */
static u8 ShooterEff_FlatCall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u16 itemID, int itemParam, u8 actParam )
{
  u32 hem_state = BTL_Hem_PushState( &wk->HEManager );
  {
    BTL_HANDEX_PARAM_RESET_RANK* reset_param;
    BTL_HANDEX_PARAM_MESSAGE* msg_param;
    u8 pokeID = BPP_GetID( bpp );

    reset_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_RESET_RANK, pokeID );
    msg_param = BTL_SVF_HANDEX_Push( wk, BTL_HANDEX_MESSAGE, pokeID );

    reset_param->poke_cnt = 1;
    reset_param->pokeID[0] = pokeID;

    HANDEX_STR_Setup( &msg_param->str, BTL_STRTYPE_SET, BTL_STRID_SET_Shooter_FlatCall );
  }
  BTL_SVF_HandEx_Root( wk, ITEM_DUMMY_DATA );
  BTL_Hem_PopState( &wk->HEManager, hem_state );
  return TRUE;
}



/*====================================================================================================*/
/*                                                                                                    */
/*  ���s����                                                                                          */
/*                                                                                                    */
/*====================================================================================================*/

//=============================================================================================
/**
 * ���s����
 *
 * @param   wk
 *
 * @retval  BtlResult   ���s�R�[�h
 */
//=============================================================================================
BtlResult BTL_SVFSUB_CheckBattleResult( BTL_SVFLOW_WORK* wk )
{
  u8 alivePokeCnt[2], totalPokeCnt[2];
  u8 playerClientID;
  u32 i;

// ���w�c�̐����c��|�P�������|�P���J�E���g
  alivePokeCnt[0] = alivePokeCnt[1] = 0;
  totalPokeCnt[0] = totalPokeCnt[1] = 0;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( BTL_MAIN_IsExistClient(wk->mainModule, i) )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, i );
      if( BTL_PARTY_GetMemberCount(party) )
      {
        u8 side = BTL_MAIN_GetClientSide( wk->mainModule, i );
        alivePokeCnt[ side ] += BTL_PARTY_GetAliveMemberCount( party );
        totalPokeCnt[ side ] += BTL_PARTY_GetMemberCount( party );
      }
    }
  }
  BTL_N_Printf( DBGSTR_SVFS_Result_RestCnt, alivePokeCnt[0], alivePokeCnt[1] );

  {
    u8 playerClientID = BTL_MAIN_GetPlayerClientID( wk->mainModule );
    u8 playerSide = BTL_MAIN_GetClientSide( wk->mainModule, playerClientID );

    // �ǂ��炩�S�ŁA��������͐����c��̃P�[�X -> �ȒP
    if( (alivePokeCnt[0] == 0) && (alivePokeCnt[1] != 0) )
    {
      return (playerSide == 1)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
    }
    if( (alivePokeCnt[1] == 0) && (alivePokeCnt[0] != 0) )
    {
      return (playerSide == 0)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
    }

    // �������肪�s�v�Ȃ��������
    if( !BTL_MAIN_IsResultStrictMode(wk->mainModule) ){
      return BTL_RESULT_DRAW;
    }

    // �����S�� -> �Ō�ɓ|�ꂽ���̏���
    if( alivePokeCnt[0] == 0 )
    {
      u8 lastPokeID = BTL_DEADREC_GetLastDeadPokeID( &wk->deadRec );
      if( lastPokeID != BTL_POKEID_NULL )
      {
        u8 lastPokeSide = BTL_MAINUTIL_PokeIDtoSide( lastPokeID );

        BTL_N_Printf( DBGSTR_SVFS_LastPokeID, lastPokeID );

        return (lastPokeSide == playerSide)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
      }
      else{
        // �i�L�蓾�Ȃ����ǂ˂�̂��߁j
        return BTL_RESULT_DRAW;
      }
    }
    // ���������c��i�^�C���A�E�g���j
    else
    {
      return CheckResult_AliveBoth( wk, alivePokeCnt, totalPokeCnt, playerSide );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * ���w�c�����c�莞�̌����ȏ��s����
 *
 * @param   wk
 * @param   playerSide
 *
 * @retval  BtlResult
 */
//----------------------------------------------------------------------------------
static BtlResult CheckResult_AliveBoth( BTL_SVFLOW_WORK* wk, const u8* alivePokeCnt, const u8* totalPokeCnt, u8 playerSide )
{
  int deadPoke[ 2 ];
  u8 winSide;

  // �|�����|�P���ŗD���ȕ�������
  deadPoke[0] = totalPokeCnt[0] - alivePokeCnt[0];
  deadPoke[1] = totalPokeCnt[1] - alivePokeCnt[1];
  BTL_N_Printf( DBGSTR_SVFL_DeadPokeCount, deadPoke[0], deadPoke[1]);

  if( deadPoke[0] != deadPoke[1] ){
    winSide = (deadPoke[0] > deadPoke[1])? 0 : 1;
    return (winSide == playerSide)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
  }

  // �c����ł����������Ȃ�c��HP�����ŗD���ȕ��̏���
  {
    int HP_total[ 2 ];
    int HP_ratio[ 2 ];

    calcHPRatio( wk, &HP_total[0], &HP_ratio[0], 0 );
    calcHPRatio( wk, &HP_total[1], &HP_ratio[1], 1 );

    BTL_N_Printf( DBGSTR_SVFL_HPRatio, 0, HP_total[0], HP_ratio[0] );
    BTL_N_Printf( DBGSTR_SVFL_HPRatio, 1, HP_total[1], HP_ratio[1] );

    if( HP_ratio[0] != HP_ratio[1] ){
      winSide = (HP_ratio[0] > HP_ratio[1])? 0 : 1;
      return (winSide == playerSide)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
    }
    // �c�܂����������Ȃ�c��HP���a�ŗD���ȕ��̏���
    if( HP_total[0] != HP_total[1] ){
      winSide = (HP_total[0] > HP_total[1])? 0 : 1;
      return (winSide == playerSide)? BTL_RESULT_WIN : BTL_RESULT_LOSE;
    }

    return BTL_RESULT_DRAW;
  }
}


//----------------------------------------------------------------------------------
/**
 * �w��w�c���p�[�e�B��HP�c���a�������v�Z
 *
 * @param   wk
 * @param   HP_sum      [out] ���a���i�[
 * @param   HP_ratio    [out] �������i�[
 * @param   side
 */
//----------------------------------------------------------------------------------
static void calcHPRatio( BTL_SVFLOW_WORK* wk, int* HP_sum, int* HP_ratio, u8 side )
{
  u32 i;
  u32 HPMax_sum = 0;

  *HP_sum = 0;
  *HP_ratio = 0;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( (BTL_MAIN_IsExistClient(wk->mainModule, i))
    &&  (BTL_MAIN_GetClientSide(wk->mainModule, i) == side)
    ){
      BTL_PARTY* party = BTL_POKECON_GetPartyData( wk->pokeCon, i );
      u32 member_count = BTL_PARTY_GetMemberCount( party );
      u32 p;
      for(p=0; p<member_count; ++p)
      {
        const BTL_POKEPARAM* bpp = BTL_PARTY_GetMemberDataConst( party, p );
        HPMax_sum += BPP_GetValue( bpp, BPP_MAX_HP );
        (*HP_sum) += BPP_GetValue( bpp, BPP_HP );
      }
    }
  }


  *HP_ratio = ((*HP_sum)*100) / HPMax_sum;
}

