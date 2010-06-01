//=============================================================================================
/**
 * @file  btl_server_flow_sub.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o�R�}���h���������i�T�u���[�`���Q�j
 * @author  taya
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
