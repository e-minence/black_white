//=============================================================================================
/**
 * @file	btl_pokeparam.c
 * @brief	�|�P����WB �o�g���V�X�e��  �Q���|�P�����퓬�p�f�[�^
 * @author	taya
 *
 * @date	2008.10.08	�쐬
 */
//=============================================================================================
#include <gflib.h>
#include "poke_tool/poke_tool.h"
#include "waza_tool/wazadata.h"

#include "btl_common.h"
#include "btl_calc.h"
#include "btl_pokeparam.h"


/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	RANK_STATUS_MIN = 0,
	RANK_STATUS_MAX = 13,
	RANK_STATUS_DEF = 6,

	RANK_CRITICAL_MIN = 0,
	RANK_CRITICAL_MAX = 4,
	RANK_CRITICAL_DEF = 0,

	TURNFLG_BUF_SIZE = (BPP_TURNFLG_MAX/8)+(BPP_TURNFLG_MAX%8!=0),
	CONTFLG_BUF_SIZE = (BPP_CONTFLG_MAX/8)+(BPP_CONTFLG_MAX%8!=0),
};



//--------------------------------------------------------------
/**
 *	��{�p�����[�^  --�o�g�����A�s�ςȗv�f--
 */
//--------------------------------------------------------------
typedef struct {

	u16	monsno;				///< �|�P�����i���o�[

	u8	hpMax;				///< �ő�HP
	u8	attack;				///< ��������
	u8	defence;			///< �ڂ�����
	u8	sp_attack;		///< �Ƃ�����
	u8	sp_defence;		///< �Ƃ��ڂ�
	u8	agility;			///< ���΂₳

	u8  level;				///< ���x��
	u8  type1;				///< �^�C�v�P
	u8  type2;				///< �^�C�v�Q
	u8	sex;					///< ����

}BPP_BASE_PARAM;

//--------------------------------------------------------------
/**
 *	�����N����
 */
//--------------------------------------------------------------
typedef struct {

	s8	attack;			///< ��������
	s8	defence;		///< �ڂ�����
	s8	sp_attack;	///< �Ƃ�����
	s8	sp_defence;	///< �Ƃ��ڂ�
	s8	agility;		///< ���΂₳
	s8	hit;				///< ������
	s8	avoid;			///< ���
	s8	critical;		///< �N���e�B�J����

}BPP_VARIABLE_PARAM;

//--------------------------------------------------------------
/**
 *	���ۂɌv�Z�Ɏg����p�����[�^�l�i�����l�~�����N���ʁj
 */
//--------------------------------------------------------------
typedef struct {

	u16	attack;				///< ��������
	u16	defence;			///< �ڂ�����
	u16	sp_attack;		///< �Ƃ�����
	u16	sp_defence;		///< �Ƃ��ڂ�
	u16	agility;			///< ���΂₳
	u16	dmy;

}BPP_REAL_PARAM;

//--------------------------------------------------------------
/**
 *	���U
 */
//--------------------------------------------------------------
typedef struct {
	u16  number;
	u8   pp;
	u8   ppMax;
}BPP_WAZA;


struct _BTL_POKEPARAM {

	BPP_BASE_PARAM			baseParam;
	BPP_VARIABLE_PARAM	varyParam;
	BPP_REAL_PARAM			realParam;
	BPP_WAZA						waza[ PTL_WAZA_MAX ];
	const POKEMON_PARAM*	ppSrc;

	u16  item;
	u16  tokusei;
	u16  hp;

	u8	myID;
	u8	wazaCnt;
	u8	pokeSick;
	u8	pokeSickCounter;

	BPP_SICK_CONT		sickCont[ WAZASICK_MAX ];
	u8	wazaSickCounter[ WAZASICK_MAX ];
	u8	turnFlag[ TURNFLG_BUF_SIZE ];
	u8	contFlag[ CONTFLG_BUF_SIZE ];

	u32 dmy;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void Effrank_Init( BPP_VARIABLE_PARAM* rank );
static void update_RealParam( BTL_POKEPARAM* pp, BppValueID rankType );
static inline void flgbuf_clear( u8* buf, u32 size );
static inline void flgbuf_set( u8* buf, u32 flagID );
static inline void flgbuf_reset( u8* buf, u32 flagID );
static inline BOOL flgbuf_get( const u8* buf, u32 flagID );



//=============================================================================================
/**
 * �o�g���p�|�P�����p�����[�^����
 *
 * @param   pp			�Q�[�����ėp�|�P�����p�����[�^
 * @param   pokeID		�o�g���Q���|�PID�i�o�g���ɎQ�����Ă���|�P�����Ɋ���U���ӂ̐��l�j
 * @param   heapID		
 *
 * @retval  BTL_POKEPARAM*		
 */
//=============================================================================================
BTL_POKEPARAM*  BTL_POKEPARAM_Create( const POKEMON_PARAM* pp, u8 pokeID, HEAPID heapID )
{
	BTL_POKEPARAM* bpp = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_POKEPARAM) );
	int i;
	u16 monsno;

	// ��{�p�����^������

	monsno = PP_Get( pp, ID_PARA_monsno, 0 );

	bpp->baseParam.monsno = monsno;
	bpp->baseParam.level = PP_Get( pp, ID_PARA_level, 0 );
	bpp->baseParam.hpMax = PP_Get( pp, ID_PARA_hpmax, 0 );
	bpp->baseParam.attack = PP_Get( pp, ID_PARA_pow, 0 );
	bpp->baseParam.defence = PP_Get( pp, ID_PARA_def, 0 );
	bpp->baseParam.sp_attack = PP_Get( pp, ID_PARA_spepow, 0 );
	bpp->baseParam.sp_defence = PP_Get( pp, ID_PARA_spedef, 0 );
	bpp->baseParam.agility = PP_Get( pp, ID_PARA_agi, 0 );
	bpp->baseParam.type1 = PP_Get( pp, ID_PARA_type1, 0 );
	bpp->baseParam.type2 = PP_Get( pp, ID_PARA_type2, 0 );
	bpp->baseParam.sex = PP_GetSex( pp );

	// �����N���ʏ�����
	Effrank_Init( &bpp->varyParam );

	// ���p�����[�^������
	bpp->realParam.attack = bpp->baseParam.attack;
	bpp->realParam.defence = bpp->baseParam.defence;
	bpp->realParam.sp_attack = bpp->baseParam.sp_attack;
	bpp->realParam.sp_defence = bpp->baseParam.sp_defence;
	bpp->realParam.agility = bpp->baseParam.agility;

	// ���L���U�f�[�^������
	bpp->wazaCnt = 0;
	for(i=0; i<PTL_WAZA_MAX; i++)
	{
		bpp->waza[i].number = PP_Get( pp, ID_PARA_waza1+i, 0 );
		if( bpp->waza[i].number )
		{
			bpp->waza[i].pp = PP_Get( pp, ID_PARA_pp1+i, 0 );
			bpp->waza[i].ppMax = PP_Get( pp, ID_PARA_pp_max1+i, 0 );
			bpp->wazaCnt++;
		}
		else
		{
			bpp->waza[i].pp = 0;
			bpp->waza[i].ppMax = 0;
		}
	}

	// ���U�n��Ԉُ�̃J�E���^�[���N���A
	for(i=0; i<NELEMS(bpp->sickCont); ++i){
		bpp->sickCont[i].raw = 0;
		bpp->sickCont[i].type = WAZASICK_CONT_NONE;
	}
	GFL_STD_MemClear( bpp->wazaSickCounter, sizeof(bpp->wazaSickCounter) );

	bpp->item = PP_Get( pp, ID_PARA_item, NULL );
	bpp->tokusei = PP_Get( pp, ID_PARA_speabino, 0 );
	bpp->hp = PP_Get( pp, ID_PARA_hp, 0 );
	bpp->myID = pokeID;
	bpp->pokeSick = PP_GetSick( pp );
	if( bpp->pokeSick != POKESICK_NEMURI )
	{
		bpp->pokeSickCounter = 0;
	}
	else
	{
		bpp->pokeSickCounter = BTL_CALC_RandRange( BTL_NEMURI_TURN_MIN, BTL_NEMURI_TURN_MIN );
	}

	bpp->ppSrc = pp;

	flgbuf_clear( bpp->turnFlag, sizeof(bpp->turnFlag) );
	flgbuf_clear( bpp->contFlag, sizeof(bpp->contFlag) );

	return bpp;
}

//=============================================================================================
/**
 * �o�g���p�|�P�����p�����[�^�폜
 *
 * @param   bpp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Delete( BTL_POKEPARAM* bpp )
{
	GFL_HEAP_FreeMemory( bpp );
}


//=============================================================================================
/**
 * �f�[�^�R�s�[
 *
 * @param   dst		
 * @param   src		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_Copy( BTL_POKEPARAM* dst, const BTL_POKEPARAM* src )
{
	*dst = *src;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

static void Effrank_Init( BPP_VARIABLE_PARAM* rank )
{
	rank->attack = RANK_STATUS_DEF;
	rank->defence = RANK_STATUS_DEF;
	rank->sp_attack = RANK_STATUS_DEF;
	rank->sp_defence = RANK_STATUS_DEF;
	rank->agility = RANK_STATUS_DEF;
	rank->hit = RANK_STATUS_DEF;
	rank->avoid = RANK_STATUS_DEF;
	rank->critical = RANK_CRITICAL_DEF;
}




//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------

u8 BTL_POKEPARAM_GetID( const BTL_POKEPARAM* pp )
{
	return pp->myID;
}

u16 BTL_POKEPARAM_GetMonsNo( const BTL_POKEPARAM* pp )
{
	return pp->baseParam.monsno;
}

u8 BTL_POKEPARAM_GetWazaCount( const BTL_POKEPARAM* pp )
{
	return pp->wazaCnt;
}

WazaID BTL_POKEPARAM_GetWazaNumber( const BTL_POKEPARAM* pp, u8 idx )
{
	GF_ASSERT(idx < pp->wazaCnt);
	return pp->waza[idx].number;
}

WazaID BTL_POKEPARAM_GetWazaParticular( const BTL_POKEPARAM* pp, u8 idx, u8* PP, u8* PPMax )
{
	GF_ASSERT(idx < pp->wazaCnt);
	*PP = pp->waza[idx].pp;
	*PPMax = pp->waza[idx].ppMax;

	return pp->waza[idx].number;
}


PokeTypePair BTL_POKEPARAM_GetPokeType( const BTL_POKEPARAM* pp )
{
	return PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
}

BOOL BTL_POKEPARAM_IsMatchType( const BTL_POKEPARAM* pp, PokeType type )
{
	PokeTypePair pair = PokeTypePair_Make( pp->baseParam.type1, pp->baseParam.type2 );
	return PokeTypePair_IsMatch( pair, type );
}

const POKEMON_PARAM* BTL_POKEPARAM_GetSrcData( const BTL_POKEPARAM* bpp )
{
	return bpp->ppSrc;
}


//=============================================================================================
/**
 * �e��p�����[�^�擾
 *
 * @param   pp		
 * @param   vid		
 *
 * @retval  int		
 */
//=============================================================================================
int BTL_POKEPARAM_GetValue( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:		return pp->realParam.attack;
	case BPP_DEFENCE:		return pp->realParam.defence;
	case BPP_SP_ATTACK:		return pp->realParam.sp_attack;
	case BPP_SP_DEFENCE:	return pp->realParam.sp_defence;
	case BPP_AGILITY:		return pp->realParam.agility;

	case BPP_HIT_RATIO:			return pp->varyParam.hit;
	case BPP_AVOID_RATIO:		return pp->varyParam.avoid;
	case BPP_CRITICAL_RATIO:	return pp->varyParam.critical;

	case BPP_LEVEL:			return pp->baseParam.level;
	case BPP_HP:				return pp->hp;
	case BPP_MAX_HP:		return pp->baseParam.hpMax;
	case BPP_SEX:				return pp->baseParam.sex;

	case BPP_TOKUSEI:		return pp->tokusei;

	default:
		GF_ASSERT(0);
		return 0;
	};
}
//=============================================================================================
/**
 * �����N�␳�t���b�g�ȏ�Ԃ̃p�����[�^�擾
 *
 * @param   pp		
 * @param   vid		
 *
 * @retval  int		
 */
//=============================================================================================
int BTL_POKEPARAM_GetValue_Base( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:		return pp->baseParam.attack;
	case BPP_DEFENCE:		return pp->baseParam.defence;
	case BPP_SP_ATTACK:		return pp->baseParam.sp_attack;
	case BPP_SP_DEFENCE:	return pp->baseParam.sp_defence;
	case BPP_AGILITY:		return pp->baseParam.agility;

	case BPP_HIT_RATIO:			return RANK_STATUS_DEF;
	case BPP_AVOID_RATIO:		return RANK_STATUS_DEF;
	case BPP_CRITICAL_RATIO:	return RANK_CRITICAL_DEF;

	default:
		return BTL_POKEPARAM_GetValue( pp, vid );
	};
}
//=============================================================================================
/**
 * �N���e�B�J���q�b�g���̃p�����[�^�擾�i�U�����ɕs���ȃ����N�␳���t���b�g�ɂ���j
 *
 * @param   pp		
 * @param   vid		
 *
 * @retval  int
 */
//=============================================================================================
int BTL_POKEPARAM_GetValue_Critical( const BTL_POKEPARAM* pp, BppValueID vid )
{
	switch( vid ){
	case BPP_ATTACK:	return (pp->varyParam.attack < 0)? pp->baseParam.attack : pp->realParam.attack;
	case BPP_SP_ATTACK:	return (pp->varyParam.sp_attack < 0)? pp->baseParam.sp_attack : pp->realParam.sp_attack;
	case BPP_DEFENCE:	return (pp->varyParam.defence > 0)? pp->baseParam.defence : pp->realParam.defence;
	case BPP_SP_DEFENCE:return (pp->varyParam.sp_defence > 0)? pp->baseParam.sp_defence : pp->realParam.sp_defence;

	default:
		return BTL_POKEPARAM_GetValue( pp, vid );
	}
}
//=============================================================================================
/**
 * ���L�A�C�e��ID��Ԃ�
 *
 * @param   pp		
 *
 * @retval  u32		
 */
//=============================================================================================
u32 BTL_POKEPARAM_GetItem( const BTL_POKEPARAM* pp )
{
	return pp->item;
}


//=============================================================================================
/**
 * �Ђ񂵏�Ԃ��`�F�b�N
 *
 * @param   pp				
 *
 * @retval  BOOL			
 */
//=============================================================================================
BOOL BTL_POKEPARAM_IsDead( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_GetValue( pp, BPP_HP ) == 0;
}
//=============================================================================================
/**
 * 
 *
 * @param   pp		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTL_POKEPARAM_IsFullHP( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_GetValue(pp, BPP_HP) == BTL_POKEPARAM_GetValue(pp, BPP_MAX_HP);
}
//=============================================================================================
/**
 * �|�P������Ԉُ�ɂ������Ă��邩�`�F�b�N
 *
 * @param   pp		
 *
 * @retval  PokeSick		�������Ă����Ԉُ�̎��ʎq�i�������Ă��Ȃ��ꍇ POKESICK_NULL�j
 */
//=============================================================================================
PokeSick BTL_POKEPARAM_GetPokeSick( const BTL_POKEPARAM* pp )
{
	return pp->pokeSick;
}
//=============================================================================================
/**
 * ����̏�Ԉُ�ɂ������Ă��邩�`�F�b�N
 *
 * @param   pp				
 * @param   sickType		
 *
 * @retval  BOOL			
 */
//=============================================================================================
BOOL BTL_POKEPARAM_CheckSick( const BTL_POKEPARAM* pp, WazaSick sickType )
{
	GF_ASSERT(sickType < NELEMS(pp->sickCont));

	return pp->sickCont[ sickType ].type != WAZASICK_CONT_NONE;
}
//=============================================================================================
/**
 * ��Ԉُ�̃^�[���`�F�b�N�Ō���HP�̗ʂ��v�Z
 *
 * @param   pp		
 *
 * @retval  int		
 */
//=============================================================================================
int BTL_POKEPARAM_CalcSickDamage( const BTL_POKEPARAM* pp )
{
	switch( pp->pokeSick ){
	case POKESICK_DOKU:
		// �J�E���^��0�Ȃ�ʏ�́u�ǂ��v
		if( pp->pokeSickCounter == 0 )
		{
			return BTL_CALC_QuotMaxHP( pp, 8 );;
		}
		// �J�E���^��1�`�Ȃ�u�ǂ��ǂ��v
		else
		{
			return (pp->baseParam.hpMax / 16) * pp->pokeSickCounter;
		}
		break;

	case POKESICK_YAKEDO:
			return BTL_CALC_QuotMaxHP( pp, 8 );

	default:
		return 0;
	}
}
//=============================================================================================
/**
 * �^�[���t���O�l�擾
 *
 * @param   pp		
 * @param   flagID		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTL_POKEPARAM_GetTurnFlag( const BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
	return flgbuf_get( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�l�擾
 *
 * @param   pp			
 * @param   flagID		
 *
 * @retval  BOOL		
 */
//=============================================================================================
BOOL BTL_POKEPARAM_GetContFlag( const BTL_POKEPARAM* pp, BppContFlag flagID )
{
	return flgbuf_get( pp->contFlag, flagID );
}


//=============================================================================================
/**
 * HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp		
 *
 * @retval  BppHpBorder		
 */
//=============================================================================================
BppHpBorder BTL_POKEPARAM_CheckHPBorder( const BTL_POKEPARAM* pp, u32 hp )
{
	if( hp <= (pp->baseParam.hpMax / 8) )
	{
		return BPP_HPBORDER_RED;
	}
	if( hp <= (pp->baseParam.hpMax / 3) )
	{
		return BPP_HPBORDER_YELLOW;
	}
	return BPP_HPBORDER_GREEN;
}
//=============================================================================================
/**
 * HP�c�ʂ̂߂₷�i���ʁE�����E�s���`�Ƃ��j��Ԃ�
 *
 * @param   pp		
 *
 * @retval  BppHpBorder		
 */
//=============================================================================================
BppHpBorder BTL_POKEPARAM_GetHPBorder( const BTL_POKEPARAM* pp )
{
	return BTL_POKEPARAM_CheckHPBorder( pp, pp->hp );
}

//-----------------------------

//=============================================================================================
/**
 * �����N�A�b�v����
 *
 * @param   pp			
 * @param   rankType	
 * @param   volume		
 *
 * @retval  BOOL		�����N���オ�����ꍇTRUE�^�����オ��Ȃ��ꍇFALSE
 */
//=============================================================================================
BOOL BTL_POKEPARAM_RankUp( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
	s8 *ptr;
	s8 max = RANK_STATUS_MAX;

	switch( rankType ){
	case BPP_ATTACK:		ptr = &pp->varyParam.attack; break;
	case BPP_DEFENCE:		ptr = &pp->varyParam.defence; break;
	case BPP_SP_ATTACK:		ptr = &pp->varyParam.sp_attack; break;
	case BPP_SP_DEFENCE:	ptr = &pp->varyParam.sp_defence; break;
	case BPP_AGILITY:		ptr = &pp->varyParam.agility; break;
	case BPP_HIT_RATIO:		ptr = &pp->varyParam.hit; break;
	case BPP_AVOID_RATIO:	ptr = &pp->varyParam.avoid; break;
	case BPP_CRITICAL_RATIO:
		ptr = &pp->varyParam.critical;
		max = RANK_CRITICAL_MAX;
		break;
	default:
		GF_ASSERT(0);
		return FALSE;
	}

	if( *ptr < max )
	{
		*ptr += volume;
		if( *ptr > max )
		{
			*ptr = max;
		}
		update_RealParam( pp, rankType );
		return TRUE;
	}

	return FALSE;
}

//=============================================================================================
/**
 * �����N�_�E������
 *
 * @param   pp			
 * @param   rankType	
 * @param   volume		
 *
 * @retval  BOOL		�����N�����������ꍇTRUE�^����������Ȃ��ꍇFALSE
 */
//=============================================================================================
BOOL BTL_POKEPARAM_RankDown( BTL_POKEPARAM* pp, BppValueID rankType, u8 volume )
{
	s8 *ptr;
	s8 min = RANK_STATUS_MIN;

	switch( rankType ){
	case BPP_ATTACK:		ptr = &pp->varyParam.attack; break;
	case BPP_DEFENCE:		ptr = &pp->varyParam.defence; break;
	case BPP_SP_ATTACK:		ptr = &pp->varyParam.sp_attack; break;
	case BPP_SP_DEFENCE:	ptr = &pp->varyParam.sp_defence; break;
	case BPP_AGILITY:		ptr = &pp->varyParam.agility; break;
	case BPP_HIT_RATIO:		ptr = &pp->varyParam.hit; break;
	case BPP_AVOID_RATIO:	ptr = &pp->varyParam.avoid; break;
	case BPP_CRITICAL_RATIO:
		ptr = &pp->varyParam.critical;
		min = RANK_CRITICAL_MIN;
		break;
	default:
		GF_ASSERT(0);
		return FALSE;
	}

	if( *ptr > min )
	{
		*ptr -= volume;
		if( *ptr < min )
		{
			*ptr = min;
		}
		update_RealParam( pp, rankType );
		return TRUE;
	}

	return FALSE;
}

//=============================================================================================
/**
 * HP�l������
 *
 * @param   pp		
 * @param   value		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_HpMinus( BTL_POKEPARAM* pp, u16 value )
{
	if( pp->hp > value )
	{
		pp->hp -= value;
	}
	else
	{
		pp->hp = 0;
	}
}
//=============================================================================================
/**
 * HP�l�𑝉�
 *
 * @param   pp		
 * @param   value		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_HpPlus( BTL_POKEPARAM* pp, u16 value )
{
	pp->hp += value;
	if( pp->hp > pp->baseParam.hpMax )
	{
		pp->hp = pp->baseParam.hpMax;
	}
}
//=============================================================================================
/**
 * HP�l���[���ɂ���
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_HpZero( BTL_POKEPARAM* pp )
{
	pp->hp = 0;
}
//=============================================================================================
/**
 * ���UPP�l������
 *
 * @param   pp		
 * @param   wazaIdx		
 * @param   value		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_PPMinus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
	GF_ASSERT(wazaIdx < pp->wazaCnt);

	if( pp->waza[wazaIdx].pp >= value )
	{
		pp->waza[wazaIdx].pp -= value;
	}
	else
	{
		pp->waza[wazaIdx].pp = 0;
	}
}
//=============================================================================================
/**
 * ���UPP�l�𑝉�
 *
 * @param   pp		
 * @param   wazaIdx		
 * @param   value		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_PPPlus( BTL_POKEPARAM* pp, u8 wazaIdx, u8 value )
{
	GF_ASSERT(wazaIdx < pp->wazaCnt);

	pp->waza[wazaIdx].pp += value;
	if( pp->waza[wazaIdx].pp > pp->waza[wazaIdx].ppMax )
	{
		pp->waza[wazaIdx].pp = pp->waza[wazaIdx].ppMax;
	}
}
//=============================================================================================
/**
 * ��Ԉُ��ݒ�
 *
 * @param   pp			
 * @param   sick		
 * @param   contParam		�p���p�����[�^
 *
 */
//=============================================================================================
void BTL_POKEPARAM_SetWazaSick( BTL_POKEPARAM* pp, WazaSick sick, BPP_SICK_CONT contParam )
{
	if( sick < POKESICK_MAX )
	{
		GF_ASSERT(pp->pokeSick == POKESICK_NULL);
		pp->pokeSick = sick;
		pp->pokeSickCounter = contParam.turn.count;
	}
	else
	{
		pp->wazaSickCounter[sick] = contParam.turn.count;
		pp->sickCont[ sick ] = contParam;
	}
}
//=============================================================================================
/**
 * �|�P�����n��Ԉُ���񕜂�����
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_CurePokeSick( BTL_POKEPARAM* pp )
{
	if( pp->pokeSick != POKESICK_NULL )
	{
		pp->sickCont[ pp->pokeSick ].type = WAZASICK_CONT_NONE;
	}
	pp->pokeSick = POKESICK_NULL;
	pp->pokeSickCounter = 0;
}
//=============================================================================================
/**
 * ���U�n��Ԉُ���񕜂�����
 *
 * @param   pp		
 * @param   sick		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_CureWazaSick( BTL_POKEPARAM* pp, WazaSick sick )
{
	if( sick < POKESICK_MAX )
	{
		BTL_POKEPARAM_CurePokeSick( pp );
	}
	else
	{
		pp->sickCont[ sick ].type = WAZASICK_CONT_NONE;
	}
}
//=============================================================================================
/**
 * �P�^�[���L���t���O�̃Z�b�g
 *
 * @param   pp		
 * @param   flagID		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_SetTurnFlag( BTL_POKEPARAM* pp, BppTurnFlag flagID )
{
	flgbuf_set( pp->turnFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�̃Z�b�g
 *
 * @param   pp		
 * @param   flagID		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_SetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID )
{
	flgbuf_set( pp->contFlag, flagID );
}
//=============================================================================================
/**
 * �i���t���O�̃��Z�b�g
 *
 * @param   pp		
 * @param   flagID		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_ResetContFlag( BTL_POKEPARAM* pp, BppContFlag flagID )
{
	flgbuf_reset( pp->contFlag, flagID );
}

//=============================================================================================
/**
 * �u�˂ނ�v�^�[���i�s�@���|�P������Ԉُ�`�F�b�N�́A�u���������v��I�񂾏ꍇ�ɂ̂ݍs��
 *
 * @param   pp			
 *
 * @retval  BOOL		�ڂ��o�߂��ꍇ��True
 */
//=============================================================================================
BOOL BTL_POKEPARAM_Nemuri_CheckWake( BTL_POKEPARAM* pp )
{
	GF_ASSERT(pp->pokeSick == POKESICK_NEMURI);

	if( pp->sickCont[POKESICK_NEMURI].type == WAZASICK_CONT_TURN )
	{
		if( pp->sickCont[POKESICK_NEMURI].turn.count == 0 )
		{
			pp->sickCont[POKESICK_NEMURI].type = WAZASICK_CONT_NONE;
			return TRUE;
		}
	}
	return FALSE;
}
//=============================================================================================
/**
 * �S��Ԉُ�̃^�[���`�F�b�N����
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_WazaSick_TurnCheck( BTL_POKEPARAM* pp )
{
	u32 i;
	for(i=0; i<NELEMS(pp->sickCont); ++i)
	{
		if( pp->sickCont[i].type == WAZASICK_CONT_TURN )
		{
			u8 n = 1;
			if( (i == WAZASICK_NEMURI)
			&&	(pp->tokusei == POKETOKUSEI_HAYAOKI )
			){
				n = 2;
			}

			if( pp->sickCont[i].turn.count > n ){
				pp->sickCont[i].turn.count--;
			}else{
				pp->sickCont[i].turn.count = 0;
				// ����̂݁A�ڊo�߃`�F�b�N�ŃI�t�ɂ���
				if( i != WAZASICK_NEMURI )
				{
					pp->sickCont[i].type = WAZASICK_CONT_NONE;
				}
			}
		}
	}

	if( pp->pokeSick == POKESICK_DOKU )
	{
		if( (pp->pokeSickCounter!=0) && (pp->pokeSickCounter < BTL_MOUDOKU_COUNT_MAX) )
		{
			pp->pokeSickCounter++;
		}
	}
}
//=============================================================================================
/**
 * �P�^�[���L���t���O�̃N���A
 *
 * @param   pp		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_ClearTurnFlag( BTL_POKEPARAM* pp )
{
	flgbuf_clear( pp->turnFlag, sizeof(pp->turnFlag) );
}
//=============================================================================================
/**
 * �Ƃ�������������
 *
 * @param   pp		
 * @param   tok		
 *
 */
//=============================================================================================
void BTL_POKEPARAM_ChangeTokusei( BTL_POKEPARAM* pp, PokeTokusei tok )
{
	pp->tokusei = tok;
}


//--------------------------------------------------------------------------
/**
 * �����N���ʂƃf�t�H���g�l���������킹�Ď��ےl���Z�o���Ă���
 *
 * @param   pp		
 * @param   rankType		
 *
 */
//--------------------------------------------------------------------------
static void update_RealParam( BTL_POKEPARAM* pp, BppValueID rankType )
{
	switch( rankType ){
	case BPP_ATTACK:
		pp->realParam.attack = BTL_CALC_StatusRank(pp->baseParam.attack, pp->varyParam.attack);
		break;
	case BPP_DEFENCE:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_SP_ATTACK:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_SP_DEFENCE:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	case BPP_AGILITY:
		pp->realParam.defence = BTL_CALC_StatusRank(pp->baseParam.defence, pp->varyParam.defence);
		break;
	default:
		return;
	}

}
//---------------------------------------------------------------------------------------------
// bit�t���O�o�b�t�@����
//---------------------------------------------------------------------------------------------
static inline void flgbuf_clear( u8* buf, u32 size )
{
	GFL_STD_MemClear( buf, size );
}
static inline void flgbuf_set( u8* buf, u32 flagID )
{
	{
		u8 byte = flagID / 8;
		u8 bit = (1 << (flagID%8));
		buf[ byte ] |= bit;
	}
}
static inline void flgbuf_reset( u8* buf, u32 flagID )
{
	{
		u8 byte = flagID / 8;
		u8 bit = (1 << (flagID%8));
		buf[ byte ] &= (~bit);
	}
}
static inline BOOL flgbuf_get( const u8* buf, u32 flagID )
{
//	GF_ASSERT(flagID<BPP_TURNFLG_MAX);
	{
		u8 byte = flagID / 8;
		u8 bit = (1 << (flagID%8));
		return (buf[ byte ] & bit) != 0;
	}
}


	