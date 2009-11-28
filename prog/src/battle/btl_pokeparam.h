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
  //--- �����܂Ŕ\�̓����N�A�b�v�^�_�E�����ʂ̈����Ƃ��Ă��p���� ---

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
  BPP_EXP,

  BPP_RANKVALUE_START = BPP_ATTACK_RANK,
  BPP_RANKVALUE_END = BPP_AGILITY_RANK,
  BPP_RANKVALUE_RANGE = (BPP_RANKVALUE_END - BPP_RANKVALUE_START) + 1,


}BppValueID;

//--------------------------------------------------------------
/**
 *  �^�[���I�����ƂɃN���A�����t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

  BPP_TURNFLG_ACTION_DONE,  ///< �s������
  BPP_TURNFLG_DAMAGED,      ///< �_���[�W������
  BPP_TURNFLG_WAZA_EXE,     ///< ���U���o����
  BPP_TURNFLG_SHRINK,       ///< �Ђ�܂��ꂽ
  BPP_TURNFLG_MUST_SHRINK,  ///< �_���[�W�ŕK���Ђ��
  BPP_TURNFLG_MAMORU,       ///< �g�܂���h����
  BPP_TURNFLG_FLYING,       ///< ���ɕ����Ă�

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

  BPP_CONTFLG_KODAWARI_LOCK,    ///< �ŏ��Ɏg�������U�����o���Ȃ��Ȃ�
  BPP_CONTFLG_CANT_CHANGE,      ///< �ɂ���E����ւ����o���Ȃ��Ȃ�
  BPP_CONTFLG_TAME,
  BPP_CONTFLG_SORAWOTOBU,
  BPP_CONTFLG_DIVING,
  BPP_CONTFLG_ANAWOHORU,
  BPP_CONTFLG_MARUKUNARU,
  BPP_CONTFLG_TIISAKUNARU,
  BPP_CONTFLG_KIAIDAME,
  BPP_CONTFLG_POWERTRICK,

  BPP_CONTFLG_MAX,

}BppContFlag;

//--------------------------------------------------------------
/**
 *  �e��J�E���^�l
 */
//--------------------------------------------------------------
typedef enum {

  BPP_COUNTER_TAKUWAERU,          ///< �����킦��i�K��
  BPP_COUNTER_TAKUWAERU_DEF,      ///< �����킦��ɂ���ďオ�����h�䃉���N
  BPP_COUNTER_TAKUWAERU_SPDEF,    ///< �����킦��ɂ���ďオ�������h�����N

  BPP_COUNTER_MAX,
}BppCounter;

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
 *  �u���炦��i���U�_���[�W��HP���Œ�P�c��j�v�P�[�X
 */
//--------------------------------------------------------------
typedef enum {

  BPP_KORAE_NONE = 0,       ///< ���炦�Ȃ�
  BPP_KORAE_WAZA_DEFENDER,  ///< �h�䑤�̃��U�ɂ��i�u���炦��v���g�p�Ȃǁj
  BPP_KORAE_WAZA_ATTACKER,  ///< �U�����̃��U�ɂ��i�u�݂˂����v���g�p�Ȃǁj
  BPP_KORAE_ITEM,           ///< �h�䑤�̑����A�C�e�����ʁi�������̃^�X�L�Ȃǁj

}BppKoraeruCause;

//--------------------------------------------------------------
/**
 *  ���U�_���[�W���R�[�h�֘A�萔
 */
//--------------------------------------------------------------
enum {
  BPP_WAZADMG_REC_TURN_MAX = 3,   ///< ���^�[�����̋L�^����邩�H
  BPP_WAZADMG_REC_MAX = 6,        ///< �P�^�[���ɂ��A�������܂ŋL�^���邩�H
};
//--------------------------------------------------------------
/**
 *  ���U�_���[�W���R�[�h�\����
 */
//--------------------------------------------------------------
typedef struct {
  u16      wazaID;    ///< ���UID
  u16      damage;    ///< �_���[�W��
  u8       wazaType;  ///< �ق̂��A�݂����̃^�C�v
  u8       pokeID;    ///< �U�������|�P����ID
}BPP_WAZADMG_REC;

static inline void BPP_WAZADMG_REC_Setup( BPP_WAZADMG_REC* rec, u8 pokeID, u16 wazaID, u8 wazaType, u16 damage )
{
  rec->wazaID = wazaID;
  rec->damage = damage;
  rec->wazaType = wazaType;
  rec->pokeID = pokeID;
}

//--------------------------------------------------------------
/**
 *  ���x���A�b�v���̊e��p�����[�^�㏸�l
 */
//--------------------------------------------------------------
typedef struct {
  u8  level;
  u8  hp;
  u8  atk;
  u8  def;
  u8  sp_atk;
  u8  sp_def;
  u8  agi;
}BTL_LEVELUP_INFO;

extern BOOL BPP_AddExp( BTL_POKEPARAM* bpp, u32* exp, BTL_LEVELUP_INFO* info );


//--------------------------------------------------------------
/**
 *  �֐��v���g�^�C�v
 */
//--------------------------------------------------------------
extern BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 id, HEAPID heapID );
extern void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp );
extern void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src );

extern u8 BPP_GetID( const BTL_POKEPARAM* pp );
extern u16 BPP_GetMonsNo( const BTL_POKEPARAM* pp );
extern PokeTypePair BPP_GetPokeType( const BTL_POKEPARAM* pp );
extern u32 BPP_GetItem( const BTL_POKEPARAM* pp );
extern BOOL BPP_IsMatchType( const BTL_POKEPARAM* pp, PokeType type );
extern const POKEMON_PARAM* BPP_GetSrcData( const BTL_POKEPARAM* bpp );

extern u8 BPP_WAZA_GetCount( const BTL_POKEPARAM* pp );
extern u8 BPP_WAZA_GetUsedCount( const BTL_POKEPARAM* pp );
extern WazaID BPP_WAZA_GetID( const BTL_POKEPARAM* pp, u8 idx );
extern WazaID BPP_WAZA_GetParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax );
extern u8 BPP_WAZA_SearchIdx( const BTL_POKEPARAM* pp, WazaID waza );
extern u16 BPP_WAZA_GetPP( const BTL_POKEPARAM* pp, u8 wazaIdx );
extern u8 BPP_WAZA_GetPPShort( const BTL_POKEPARAM* bpp, u8 idx );

extern int BPP_GetValue( const BTL_POKEPARAM* pp, BppValueID vid );
extern int BPP_GetValue_Base( const BTL_POKEPARAM* pp, BppValueID vid );
extern int BPP_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid );

extern BOOL BPP_IsDead( const BTL_POKEPARAM* pp );
extern BOOL BPP_IsHPFull( const BTL_POKEPARAM* pp );
extern BOOL BPP_IsPPFull( const BTL_POKEPARAM* pp, u8 wazaIdx );

extern BOOL BPP_TURNFLAG_Get( const BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern BOOL BPP_GetActFlag( const BTL_POKEPARAM* pp, BppActFlag flagID );
extern BOOL BPP_CONTFLAG_Get( const BTL_POKEPARAM* pp, BppContFlag flagID );
extern u16 BPP_GetTurnCount( const BTL_POKEPARAM* pp );
extern u16 BPP_GetAppearTurn( const BTL_POKEPARAM* pp );

extern WazaID  BPP_GetPrevWazaID( const BTL_POKEPARAM* pp );
extern BtlPokePos  BPP_GetPrevTargetPos( const BTL_POKEPARAM* pp );
extern u32  BPP_GetWazaContCounter( const BTL_POKEPARAM* pp );
extern fx32  BPP_GetHPRatio( const BTL_POKEPARAM* pp );

extern void BPP_SetActionAgility( BTL_POKEPARAM* bpp, u16 actionAgility );
extern u16 BPP_GetActionAgility( const BTL_POKEPARAM* bpp );

extern void BPP_COUNTER_Set( BTL_POKEPARAM* bpp, BppCounter cnt, u8 value );
extern u8 BPP_COUNTER_Get( const BTL_POKEPARAM* bpp, BppCounter cnt );


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
extern BppHpBorder BPP_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp );

//=============================================================================================
/**
 * ���݂�HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp
 *
 * @retval  BppHpBorder
 */
//=============================================================================================
extern BppHpBorder BPP_GetHPBorder( const BTL_POKEPARAM* pp );


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
extern BOOL BPP_IsRankEffectValid( const BTL_POKEPARAM* pp, BppValueID rankType, int volume );

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
extern int BPP_RankEffectUpLimit( const BTL_POKEPARAM* pp, BppValueID rankType );

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
extern int BPP_RankEffectDownLimit( const BTL_POKEPARAM* pp, BppValueID rankType );

//-------------------------
typedef void (*BtlSickTurnCheckFunc)( BTL_POKEPARAM* bpp, WazaSick sick, BPP_SICK_CONT oldCont, BOOL fCure, void* work );
typedef void( *BppCureWazaSickDependPokeCallback)( void* arg, BTL_POKEPARAM* bpp, WazaSick sickID, u8 dependPokeID );


extern void BPP_SetWazaSick( BTL_POKEPARAM* pp, WazaSick sick, BPP_SICK_CONT contParam );
extern void BPP_CurePokeSick( BTL_POKEPARAM* pp );
extern void BPP_CureWazaSick( BTL_POKEPARAM* pp, WazaSick sick );
extern void BPP_CureWazaSickDependPoke( BTL_POKEPARAM* pp, u8 depend_pokeID, BppCureWazaSickDependPokeCallback callBackFunc, void* callbackArg );
extern void BPP_WazaSick_TurnCheck( BTL_POKEPARAM* bpp, BtlSickTurnCheckFunc callbackFunc, void* callbackWork );
extern void BPP_GetNickName( const BTL_POKEPARAM* bpp, STRBUF* dst );
extern void BPP_ReflectLevelup( BTL_POKEPARAM* bpp, u8 nextLevel, u8 hpMax, u8 atk, u8 def, u8 spAtk, u8 spDef, u8 agi );
extern void BPP_ReflectExpAdd( BTL_POKEPARAM* bpp );

extern BOOL BPP_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType );
extern PokeSick BPP_GetPokeSick( const BTL_POKEPARAM* pp );
extern u8 BPP_GetSickParam( const BTL_POKEPARAM* pp, WazaSick sick );
extern int  BPP_CalcSickDamage( const BTL_POKEPARAM* pp, WazaSick sickID );
extern BPP_SICK_CONT BPP_GetSickCont( const BTL_POKEPARAM* bpp, WazaSick sick );
extern u8 BPP_GetSickTurnCount( const BTL_POKEPARAM* bpp, WazaSick sick );

//-------------------------
extern u8 BPP_RankUp( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume );
extern u8 BPP_RankDown( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume );
extern void BPP_RankSet( BTL_POKEPARAM* pp, BppValueID rankType, u8 value );
extern void BPP_HpMinus( BTL_POKEPARAM* pp, u16 value );
extern void BPP_HpPlus( BTL_POKEPARAM* pp, u16 value );
extern void BPP_HpZero( BTL_POKEPARAM* pp );
extern void BPP_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value );
extern void BPP_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value );
extern BOOL BPP_Nemuri_CheckWake( BTL_POKEPARAM* pp );
extern void BPP_TURNFLAG_Set( BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern void BPP_TURNFLAG_ForceOff( BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern void BPP_TurnCheck( BTL_POKEPARAM* pp );
extern void BPP_ACTFLAG_Set( BTL_POKEPARAM* pp, BppActFlag flagID );
extern void BPP_ACTFLAG_Clear( BTL_POKEPARAM* pp );
extern void BPP_CONTFLAG_Set( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BPP_CONTFLAG_Clear( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BPP_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok );
extern void BPP_SetAppearTurn( BTL_POKEPARAM* pp, u16 turn );
extern void BPP_ChangePokeType( BTL_POKEPARAM* pp, PokeTypePair type );
extern void BPP_ChangeForm( BTL_POKEPARAM* pp, u8 formNo );
extern void BPP_RemoveItem( BTL_POKEPARAM* pp );
extern u16 BPP_GetConsumedItem( const BTL_POKEPARAM* bpp );
extern void BPP_UpdatePrevWazaID( BTL_POKEPARAM* pp, WazaID waza, BtlPokePos targetPos );
extern void BPP_ResetWazaContConter( BTL_POKEPARAM* pp );
extern void BPP_RankRecover( BTL_POKEPARAM* pp );
extern void BPP_RankReset( BTL_POKEPARAM* pp );
extern void BPP_Clear_ForDead( BTL_POKEPARAM* pp );
extern void BPP_Clear_ForOut( BTL_POKEPARAM* bpp );
extern void BPP_Clear_ForIn( BTL_POKEPARAM* bpp );
extern void BPP_SetItem( BTL_POKEPARAM* pp, u16 itemID );
extern void BPP_WAZA_SetUsedFlag( BTL_POKEPARAM* pp, u8 wazaIdx );
extern void BPP_WAZA_UpdateID( BTL_POKEPARAM* pp, u8 wazaIdx, WazaID waza, u8 ppMax, BOOL fPermenent );
extern void BPP_BatonTouchParam( BTL_POKEPARAM* target, const BTL_POKEPARAM* user );
extern void BPP_ReflectPP( BTL_POKEPARAM* bpp );
extern void BPP_SetSrcPP( BTL_POKEPARAM* bpp, POKEMON_PARAM* pp );
extern u8 BPP_GetCriticalRank( const BTL_POKEPARAM* bpp );
extern BOOL BPP_AddCriticalRank( BTL_POKEPARAM* bpp, int value );
extern void BPP_SetBaseStatus( BTL_POKEPARAM* bpp, BppValueID vid, u8 value );
extern void BPP_SetWeight( BTL_POKEPARAM* bpp, u16 weight );
extern u16 BPP_GetWeight( const BTL_POKEPARAM* bpp );


extern BOOL BPP_HENSIN_Set( BTL_POKEPARAM* bpp, const BTL_POKEPARAM* target );
extern void BPP_HENSIN_Reset( BTL_POKEPARAM* bpp );
extern BOOL BPP_HENSIN_Check( const BTL_POKEPARAM* bpp );

extern void BPP_WAZADMGREC_Add( BTL_POKEPARAM* pp, const BPP_WAZADMG_REC* rec );
extern u8   BPP_WAZADMGREC_GetCount( const BTL_POKEPARAM* pp, u8 turn_ridx );
extern BOOL BPP_WAZADMGREC_Get( const BTL_POKEPARAM* pp, u8 turn_ridx, u8 rec_ridx, BPP_WAZADMG_REC* dst );

extern void BPP_MIGAWARI_Create( BTL_POKEPARAM* bpp, u16 migawariHP );
extern void BPP_MIGAWARI_Delete( BTL_POKEPARAM* bpp );
extern BOOL BPP_MIGAWARI_IsExist( const BTL_POKEPARAM* bpp );
extern BOOL BPP_MIGAWARI_AddDamage( BTL_POKEPARAM* bpp, u16 damage );

extern void BPP_CONFRONT_REC_Set( BTL_POKEPARAM* bpp, u8 pokeID );
extern u8 BPP_CONFRONT_REC_GetCount( const BTL_POKEPARAM* bpp );
extern u8 BPP_CONFRONT_REC_GetPokeID( const BTL_POKEPARAM* bpp, u8 idx );


#endif
