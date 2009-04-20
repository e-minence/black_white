//=============================================================================================
/**
 * @file	btl_pokeparam.h
 * @brief	�|�P����WB �o�g���V�X�e��  �Q���|�P�����퓬�p�f�[�^
 * @author	taya
 *
 * @date	2008.10.08	�쐬
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
 *	�n���h���^��`
 */
//--------------------------------------------------------------
typedef struct _BTL_POKEPARAM	BTL_POKEPARAM;

//--------------------------------------------------------------
/**
 *	�擾�ł���\�͒l
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

	//--- �����܂Ń����N�A�b�v�^�_�E�����ʂ̈����Ƃ��Ă��p���� ---
	BPP_CRITICAL_RATIO,

	BPP_HP,
	BPP_MAX_HP,
	BPP_LEVEL,
	BPP_TOKUSEI,
	BPP_SEX,
	BPP_FORM,

	BPP_RANKTYPE_MAX = BPP_AVOID_RATIO+1,

}BppValueID;

//--------------------------------------------------------------
/**
 *	�^�[�����ƂɃN���A�����t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

	BPP_TURNFLG_ACTION_EXE,	///< �s������
	BPP_TURNFLG_SHRINK,			///< �Ђ�܂��ꂽ
	BPP_TURNFLG_DEAD,				///< ���̃^�[���Ɏ���

	BPP_TURNFLG_MAX,

}BppTurnFlag;

//--------------------------------------------------------------
/**
 *	�i���I�ɕێ������t���O�Z�b�g
 */
//--------------------------------------------------------------
typedef enum {

	BPP_CONTFLG_DEAD_IGNORE,	///< ���Ɏ���ł���̂ŏ�ɂ��Ȃ����̂Ƃ��Ĉ���
	BPP_CONTFLG_MAX,

}BppContFlag;

//--------------------------------------------------------------
/**
 *	HP�c�ʂ߂₷
 */
//--------------------------------------------------------------
typedef enum {

	BPP_HPBORDER_GREEN = 0,		///< �ʏ�
	BPP_HPBORDER_YELLOW,			///< 1/3�ȉ�
	BPP_HPBORDER_RED,					///< 1/8�ȉ�

}BppHpBorder;

//--------------------------------------------------------------
/**
 *	��Ԉُ�p���p�����[�^
 */
//--------------------------------------------------------------
typedef struct  {

	union {
		u16    raw;
		struct {
			u16  type : 4;
			u16  _0   : 12;
		};
		struct {
			u16  type_turn : 4;
			u16  count     : 6;
			u16  _1        : 6;
		}turn;
		struct {
			u16  type_poke : 4;
			u16  ID        : 6;
			u16  _2        : 6;
		}poke;
	};

}BPP_SICK_CONT;

static inline void BPP_SICKCONT_Set_Turn( BPP_SICK_CONT* cont, u8 turns )
{
	cont->raw = 0;
	cont->type = WAZASICK_CONT_TURN;
	cont->turn.count = turns;
}
static inline void BPP_SICKCONT_Set_Poke( BPP_SICK_CONT* cont, u8 pokeID )
{
	cont->raw = 0;
	cont->type = WAZASICK_CONT_POKE;
	cont->poke.ID = pokeID;
}
static inline void BPP_SICKCONT_Set_Permanent( BPP_SICK_CONT* cont )
{
	cont->raw = 0;
	cont->type = WAZASICK_CONT_PERMANENT;
}


extern BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 id, HEAPID heapID );
extern void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp );
extern void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src );


extern u8 BTL_POKEPARAM_GetID( const BTL_POKEPARAM* pp );
extern const POKEMON_PARAM* BTL_POKEPARAM_GetSrcData( const BTL_POKEPARAM* bpp );

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
extern BOOL BTL_POKEPARAM_IsFullHP( const BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType );

extern PokeSick BTL_POKEPARAM_GetPokeSick( const BTL_POKEPARAM* pp );
extern int BTL_POKEPARAM_CalcSickDamage( const BTL_POKEPARAM* pp );
extern BOOL BTL_POKEPARAM_GetTurnFlag( const BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern BOOL BTL_POKEPARAM_GetContFlag( const BTL_POKEPARAM* pp, BppContFlag flagID );
extern u16 BTL_POKEPARAM_GetTurnCount( const BTL_POKEPARAM* pp );
extern u16 BTL_POKEPARAM_GetAppearTurn( const BTL_POKEPARAM* pp );


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
 * @retval  BOOL		�����ꍇTRUE
 */
//=============================================================================================
extern BOOL BTL_POKEPARAM_IsRankEffectValid( const BTL_POKEPARAM* pp, BppValueID rankType, int volume );

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
extern void BTL_POKEPARAM_WazaSick_TurnCheck( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetShrink( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetTurnFlag( BTL_POKEPARAM* pp, BppTurnFlag flagID );
extern void BTL_POKEPARAM_ClearTurnFlag( BTL_POKEPARAM* pp );
extern void BTL_POKEPARAM_SetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BTL_POKEPARAM_ResetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID );
extern void BTL_POKEPARAM_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok );
extern void BTL_POKEPARAM_SetAppearTurn( BTL_POKEPARAM* pp, u16 turn );
extern void BTL_POKEPARAM_ChangePokeType( BTL_POKEPARAM* pp, PokeTypePair type );
extern void BTL_POKEPARAM_ChangeForm( BTL_POKEPARAM* pp, u8 formNo );






#endif
