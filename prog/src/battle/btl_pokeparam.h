//=============================================================================================
/**
 * @file  btl_pokeparam.h
 * @brief �|�P����WB �o�g���V�X�e��  �Q���|�P�����퓬�p�f�[�^
 * @author  taya
 *
 * @date  2008.10.08  �쐬
 */
//=============================================================================================
#ifndef __BTL_POKEPARAM_H__
#define __BTL_POKEPARAM_H__

#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_personal.h"
#include "waza_tool/wazadata.h"

#include "btl_util.h"

//--------------------------------------------------------------
/**
 *  �n���h���^��`
 */
//--------------------------------------------------------------
typedef struct _BTL_POKEPARAM BTL_POKEPARAM;

//--------------------------------------------------------------
/**
 *  �擾�ł���\�͒l
 */
//--------------------------------------------------------------
typedef enum {

  BPP_VALUE_NULL = 0,

  BPP_ATTACK,
  BPP_DEFENCE,
  BPP_SP_ATTACK,
  BPP_SP_DEFENCE,
  BPP_AGILITY,
  BPP_HIT_RATIO,
  BPP_AVOID_RATIO,
  BPP_CRITICAL_RATIO,
  //--- �����܂Ń����N�A�b�v�^�_�E�����ʂ̈����Ƃ��Ă��p���� ---

  BPP_HP,
  BPP_MAX_HP,
  BPP_LEVEL,
  BPP_TOKUSEI,
  BPP_SEX,
  BPP_FORM,
  BPP_ATTACK_RANK,
  BPP_DEFENCE_RANK,
  BPP_SP_ATTACK_RANK,
  BPP_SP_DEFENCE_RANK,
  BPP_AGILITY_RANK,

  BPP_RANKTYPE_MAX = BPP_AVOID_RATIO+1,

}BppValueID;

//--------------------------------------------------------------
/**
 *  �^�[���I�����ƂɃN���A�����t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

  BPP_TURNFLG_ACTION_DONE,  ///< �s������
  BPP_TURNFLG_WAZA_EXE,     ///< ���U���o����
  BPP_TURNFLG_SHRINK,       ///< �Ђ�܂��ꂽ
  BPP_TURNFLG_MAMORU,       ///< �g�܂���h����
  BPP_TURNFLG_FLYING,       ///< ���ɕ����Ă�t���O

  BPP_TURNFLG_MAX,

}BppTurnFlag;

//--------------------------------------------------------------
/**
 *  �A�N�V�����I�����ƂɃN���A�����t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

  BPP_ACTFLG_CANT_ACTION,   ///< �A�N�V�����I��s��

  BPP_ACTFLG_MAX,

}BppActFlag;


//--------------------------------------------------------------
/**
 *  �i���I�ɕێ������t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

  BPP_CONTFLG_MEMBERIN_EFFECT,  ///< ���꒼��
  BPP_CONTFLG_KODAWARI_LOCK,    ///< �ŏ��Ɏg�������U�����o���Ȃ��Ȃ�
  BPP_CONTFLG_CANT_CHANGE,      ///< �ɂ���E����ւ����o���Ȃ��Ȃ�
  BPP_CONTFLG_TAME,
  BPP_CONTFLG_SORAWOTOBU,
  BPP_CONTFLG_DIVING,
  BPP_CONTFLG_ANAWOHORU,
  BPP_CONTFLG_MARUKUNARU,
  BPP_CONTFLG_TIISAKUNARU,

  BPP_CONTFLG_MAX,

}BppContFlag;

//--------------------------------------------------------------
/**
 *  HP�c�ʂ߂₷
 */
//--------------------------------------------------------------
typedef enum {

  BPP_HPBORDER_GREEN = 0,   ///< �ʏ�
  BPP_HPBORDER_YELLOW,      ///< 1/3�ȉ�
  BPP_HPBORDER_RED,         ///< 1/8�ȉ�

}BppHpBorder;

//--------------------------------------------------------------
/**
 *  ��Ԉُ�p���p�����[�^
 */
//--------------------------------------------------------------
typedef struct  {

  union {
    u16    raw;
    struct {
      u16  type : 3;
      u16  _0   : 12;
    };
    struct {
      u16  type_turn : 3;
      u16  count     : 5;
      u16  param     : 8;
    }turn;
    struct {
      u16  type_poke : 3;
      u16  ID        : 6;
      u16  _2        : 6;
    }poke;
    struct {
      u16  type_perm : 3;
      u16  count_max : 5;
      u16  _2        : 8;
    }permanent;
  };

}BPP_SICK_CONT;

//--------------------------------------------------------------
/**
 *  �u���炦��i���U�_���[�W��HP���Œ�P�c��j�v���R
 */
//--------------------------------------------------------------
typedef enum {

  BPP_KORAE_NONE = 0,       ///< ���炦�Ȃ�
  BPP_KORAE_WAZA_DEFENDER,  ///< �h�䑤�̃��U�ɂ��i�u���炦��v���g�p�Ȃǁj
  BPP_KORAE_WAZA_ATTACKER,  ///< �U�����̃��U�ɂ��i�u�݂˂����v���g�p�Ȃǁj
  BPP_KORAE_ITEM,           ///< �h�䑤�̑����A�C�e�����ʁi�������̃^�X�L�Ȃǁj

}BppKoraeruCause;

static inline BPP_SICK_CONT BPP_SICKCONT_MakeTurn( u8 turns )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_TURN;
  cont.turn.count = turns;
  cont.turn.param = 0;
  return cont;
}
static inline BPP_SICK_CONT BPP_SICKCONT_Set_TurnParam( u8 turns, u8 param )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_TURN;
  cont.turn.count = turns;
  cont.turn.param = param;
  return cont;
}
static inline BPP_SICK_CONT BPP_SICKCONT_Set_Poke( u8 pokeID )
{
  BPP_SICK_CONT  cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_POKE;
  cont.poke.ID = pokeID;
  return cont;
}
static inline BPP_SICK_CONT BPP_SICKCONT_MakePermanent( void )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  return cont;
}
static inline BPP_SICK_CONT BPP_SICKCONT_MakePermanentInc( u8 count_max )
{
  BPP_SICK_CONT cont;
  cont.raw = 0;
  cont.type = WAZASICK_CONT_PERMANENT;
  cont.permanent.count_max = count_max;
  return cont;
}

static inline BOOL BPP_SICKCONT_IsMoudokuCont( BPP_SICK_CONT cont )
{
  return ((cont.type == WAZASICK_CONT_PERMANENT) && (cont.permanent.count_max > 0));
}

extern BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 id, HEAPID heapID );
extern void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp );
extern void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src );


extern u8 BTL_POKEPARAM_GetID( const BTL_POKEPARAM* pp );
extern const POKEMON_PARAM* BTL_POKEPARAM_GetSrcData( const BTL_POKEPARAM* bpp );
extern u16 BTL_POKEPARAM_GetPP( const BTL_POKEPARAM* pp, u8 wazaIdx );

extern u16 BTL_POKEPARAM_GetMonsNo( const BTL_POKEPARAM* pp );
extern u8 BTL_POKEPARAM_GetWazaCount( const BTL_POKEPARAM* pp );
extern WazaID BTL_POKEPARAM_GetWazaNumber( const BTL_POKEPARAM* pp, u8 idx );
extern WazaID BTL_POKEPARAM_GetWazaParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax );

extern PokeTypePair BTL_POKEPARAM_GetPokeType( const BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_IsMatchType( const BTL_POKEPARAM* pp, PokeType type );

extern int BTL_POKEPARAM_GetValue( const BTL_POKEPARAM* pp, BppValueID vid );
extern int BTL_POKEPARAM_GetValue_Base( const BTL_POKEPARAM* pp, BppValueID vid );
extern int BTL_POKEPARAM_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid );
extern u32 BTL_POKEPARAM_GetItem( const BTL_POKEPARAM* pp );

extern BOOL BTL_POKEPARAM_IsDead( const BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_IsHPFull( const BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType );
extern BOOL BTL_POKEPARAM_IsPPFull( const BTL_POKEPARAM* pp, u8 wazaIdx );

extern PokeSick BTL_POKEPARAM_GetPokeSick( const BTL_POKEPARAM* pp );
extern int BTL_POKEPARAM_CalcSickDamage( const BTL_POKEPARAM* pp, WazaSick sickID );
extern BOOL BTL_POKEPARAM_GetTurnFlag( const BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern BOOL BTL_POKEPARAM_GetActFlag( const BTL_POKEPARAM* pp, BppActFlag flagID );
extern BOOL BTL_POKEPARAM_GetContFlag( const BTL_POKEPARAM* pp, BppContFlag flagID );
extern u16 BTL_POKEPARAM_GetTurnCount( const BTL_POKEPARAM* pp );
extern u16 BTL_POKEPARAM_GetAppearTurn( const BTL_POKEPARAM* pp );

extern WazaID BTL_POKEPARAM_GetPrevWazaNumber( const BTL_POKEPARAM* pp );
extern BtlPokePos BTL_POKEPARAM_GetPrevTargetPos( const BTL_POKEPARAM* pp );
extern u32 BTL_POKEPARAM_GetSameWazaUsedCounter( const BTL_POKEPARAM* pp );
extern fx32 BTL_POKEPARAM_GetHPRatio( const BTL_POKEPARAM* pp );
extern u8 BTL_POKEPARAM_GetWazaIdx( const BTL_POKEPARAM* pp, WazaID waza );
extern u8 BTL_POKEPARAM_GetSickParam( const BTL_POKEPARAM* pp, WazaSick sick );



//=============================================================================================
/**
 * �w��HP�̒l����AHP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp
 * @param   hp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
extern BppHpBorder BTL_POKEPARAM_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp );

//=============================================================================================
/**
 * ���݂�HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
extern BppHpBorder BTL_POKEPARAM_GetHPBorder( const BTL_POKEPARAM* pp );


//=============================================================================================
/**
 * �����N�������ʂ������i����E�����ɒB���Ă��Ȃ��j���ǂ�������
 *
 * @param   pp
 * @param   rankType
 * @param   volume
 *
 * @retval  BOOL    �����ꍇTRUE
 */
//=============================================================================================
extern BOOL BTL_POKEPARAM_IsRankEffectValid( const BTL_POKEPARAM* pp, BppValueID rankType, int volume );

//=============================================================================================
/**
 * �����N�������ʂ����Ɖ��i�K����������
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   �i�K��
 */
//=============================================================================================
extern int BTL_POKEPARAM_RankEffectUpLimit( const BTL_POKEPARAM* pp, BppValueID rankType );

//=============================================================================================
/**
 * �����N�������ʂ����Ɖ��i�K����������
 *
 * @param   pp
 * @param   rankType
 *
 * @retval  int   �i�K���i�}�C�i�X�j
 */
//=============================================================================================
extern int BTL_POKEPARAM_RankEffectDownLimit( const BTL_POKEPARAM* pp, BppValueID rankType );


//-------------------------
extern u8 BTL_POKEPARAM_RankUp( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume );
extern u8 BTL_POKEPARAM_RankDown( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume );
extern void BTL_POKEPARAM_HpMinus( BTL_POKEPARAM* pp, u16 value );
extern void BTL_POKEPARAM_HpPlus( BTL_POKEPARAM* pp, u16 value );
extern void BTL_POKEPARAM_HpZero( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value );
extern void BTL_POKEPARAM_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value );
extern void BTL_POKEPARAM_SetWazaSick( BTL_POKEPARAM* pp, WazaSick sick, BPP_SICK_CONT contParam );
extern void BTL_POKEPARAM_CurePokeSick( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_CureWazaSick( BTL_POKEPARAM* pp, WazaSick sick );
extern BOOL BTL_POKEPARAM_Nemuri_CheckWake( BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_WazaSick_TurnCheck( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetShrink( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetTurnFlag( BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern void BTL_POKEPARAM_ForceOffTurnFlag( BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern void BTL_POKEPARAM_ClearTurnFlag( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetActFlag( BTL_POKEPARAM* pp, BppActFlag flagID );
extern void BTL_POKEPARAM_ClearActFlag( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BTL_POKEPARAM_ResetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BTL_POKEPARAM_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok );
extern void BTL_POKEPARAM_SetAppearTurn( BTL_POKEPARAM* pp, u16 turn );
extern void BTL_POKEPARAM_ChangePokeType( BTL_POKEPARAM* pp, PokeTypePair type );
extern void BTL_POKEPARAM_ChangeForm( BTL_POKEPARAM* pp, u8 formNo );
extern void BTL_POKEPARAM_RemoveItem( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_UpdateUsedWazaNumber( BTL_POKEPARAM* pp, WazaID waza, BtlPokePos targetPos );
extern void BTL_POKEPARAM_ResetUsedWazaNumber( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_RankRecover( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_RankReset( BTL_POKEPARAM* pp );
extern void BTL_POKEPARM_DeadClear( BTL_POKEPARAM* pp );



#endif
