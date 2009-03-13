//============================================================================================
/**
 * @file	waza_tool.c
 * @bfief	�Z�p�����[�^�c�[���Q�iWB�����Łj
 * @author	HisashiSogabe
 * @date	08.11.13
 */
//============================================================================================
#include	<gflib.h>

#include	"arc_def.h"

#include	"waza_tool/waza_tool.h"
#include	"waza_tool/wazano_def.h"
#include	"waza_tool/wazadata.h"

#include	"waza_tool_def.h"


typedef struct{
	u16	monsno;
	s16 hp;

	u16	hpmax;
	u8	eqp;
	u8	atc;

	u32 condition;

	u8	speabino;
	u8	sex;
	u8	type1;
	u8	type2;

}WAZA_DAMAGE_CALC_PARAM;

//============================================================================================
/**
 * �v���g�^�C�v�錾
 */
//============================================================================================
void		WT_WazaTableDataGet( void *buffer );
ARCHANDLE	*WT_WazaDataParaHandleOpen( HEAPID heapID );
void		WT_WazaDataParaHandleClose( ARCHANDLE *handle );
u32			WT_WazaDataParaGet( int waza_no, int id );
u32			WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id );
u8			WT_PPMaxGet( u16 wazano, u8 maxupcnt );
u32			WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id );

static	void	WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd );

//============================================================================================
/**
 *	�Z�f�[�^�e�[�u���f�[�^��Alloc�����������ɓW�J
 *
 * @param[in]	heapID	�f�[�^��W�J���郁�����q�[�vID
 */
//============================================================================================
void	WT_WazaTableDataGet( void *buffer )
{
	GFL_ARC_LoadDataOfs( buffer, ARCID_WAZA_TBL, 0, 0, sizeof( WAZA_TABLE_DATA ) * ( WAZANO_MAX + 1 ) );
}

//============================================================================================
/**
 *	�Z�f�[�^�e�[�u���t�@�C���n���h���I�[�v��
 *
 * @param[in]	heapID	�n���h����Alloc����q�[�vID
 */
//============================================================================================
ARCHANDLE	*WT_WazaDataParaHandleOpen( HEAPID heapID )
{
	return GFL_ARC_OpenDataHandle( ARCID_WAZA_TBL, heapID );
}

//============================================================================================
/**
 *	�Z�f�[�^�e�[�u���t�@�C���n���h���N���[�Y
 *
 * @param[in]	handle	�N���[�Y����Handle
 */
//============================================================================================
void	WT_WazaDataParaHandleClose( ARCHANDLE *handle )
{
	GFL_ARC_CloseDataHandle( handle );
}

//============================================================================================
/**
 *	�Z�f�[�^�e�[�u������l���擾
 *
 * @param[in]	waza_no		�擾�������Z�i���o�[
 * @param[in]	id			�擾�������l�̎��
 */
//============================================================================================
u32	WT_WazaDataParaGet( int waza_no, int id )
{
	WAZA_TABLE_DATA wtd;

	WT_WazaDataGet( waza_no, &wtd );

	return	WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *	�Z�f�[�^�e�[�u������l���擾�i�t�@�C���n���h�����g�p����j
 *
 * @param[in]	waza_no		�擾�������Z�i���o�[
 * @param[in]	id			�擾�������l�̎��
 */
//============================================================================================
u32	WT_WazaDataParaGetHandle( ARCHANDLE *handle, int waza_no, int id )
{
	WAZA_TABLE_DATA wtd;

	GFL_ARC_LoadDataByHandle( handle, waza_no, &wtd );

	return	WT_WazaDataParaGetAct( &wtd, id );
}

//============================================================================================
/**
 *	PPMax���擾
 *
 * @param[in]	wazano		�擾����Z�i���o�[
 * @param[in]	maxupcnt	�}�b�N�X�A�b�v���g�p������
 *
 * @retval	PPMAX
 */
//============================================================================================
u8	WT_PPMaxGet( u16 wazano, u8 maxupcnt )
{
	u8	ppmax;

	if( maxupcnt > 3 ){
		maxupcnt = 3;
	}

	ppmax = WT_WazaDataParaGet( wazano, ID_WTD_pp );
	ppmax += ( ppmax * 20 * maxupcnt ) / 100;

	return	ppmax;
}

//============================================================================================
/**
 *	�Z�e�[�u���\���̃f�[�^����l���擾
 *
 * @param[in]	wtd		�Z�e�[�u���\���̃f�[�^�ւ̃|�C���^
 * @param[in]	id		�擾�������Z�f�[�^��Index
 */
//============================================================================================
u32	WT_WazaDataParaGetAct( WAZA_TABLE_DATA *wtd, int id )
{
	u32	ret;

	switch( id ){
	case ID_WTD_battleeffect:
		ret = wtd->battleeffect;
		break;
	case ID_WTD_kind:
		ret = wtd->kind;
		break;
	case ID_WTD_damage:
		ret = wtd->damage;
		break;
	case ID_WTD_wazatype:
		ret = wtd->wazatype;
		break;
	case ID_WTD_hitprobability:
		ret = wtd->hitprobability;
		break;
	case ID_WTD_pp:
		ret = wtd->pp;
		break;
	case ID_WTD_addeffect:
		ret = wtd->addeffect;
		break;
	case ID_WTD_attackrange:
		ret = wtd->attackrange;
		break;
	case ID_WTD_attackpri:
		ret = wtd->attackpri;
		break;
	case ID_WTD_flag:
		ret = wtd->flag;
		break;
	case ID_WTD_ap_no:				//�R���e�X�g�p�p�����[�^�@�A�s�[��No
		ret = wtd->condata.ap_no;
		break;
	case ID_WTD_contype:			//�R���e�X�g�p�p�����[�^�@�R���e�X�g�Z�^�C�v
		ret = wtd->condata.contype;
		break;
	}
	return ret;
}

//============================================================================================
/**
 *							�O�����J���Ȃ��֐��S
 */
//============================================================================================
//============================================================================================
/**
 *	�Z�e�[�u���\���̃f�[�^���擾
 *
 * @param[in]	waza_no		�擾�������Z�i���o�[
 * @param[out]	wtd			�擾�����Z�f�[�^�̊i�[����w��
 */
//============================================================================================
static	void	WT_WazaDataGet( int waza_no, WAZA_TABLE_DATA *wtd )
{
	GFL_ARC_LoadData( wtd, ARCID_WAZA_TBL, waza_no );
}



//=============================================================================================
/**
 * @file	wazadata.c
 * @brief	�|�P����WB  ���U�f�[�^�A�N�Z�T
 * @author	taya
 *
 * @date	2008.10.14	�쐬
 */
//=============================================================================================

#include "waza_tool/wazadata.h"


enum {
	HITRATIO_MUST = 101,
};

//------------------------------------------------------------------------------
/**
 * �萔�錾
 */
//------------------------------------------------------------------------------
enum {
	WSEQ_SINMPLESICK_NEMURI = 1,
	WSEQ_SINMPLESICK_DOKU = 66,
	WSEQ_SINMPLESICK_MAHI = 67,
	WSEQ_SINMPLESICK_MOUDOKU = 33,
	WSEQ_DAMAGESICK_DOKU = 2,
	WSEQ_DAMAGESICK_YAKEDO = 4,
	WSEQ_DAMAGESICK_KOORI = 5,
	WSEQ_DAMAGESICK_MAHI = 6,
	WSEQ_SIMPLEEFF_ATK = 10,
	WSEQ_SIMPLEEFF_DEF = 11,
	WSEQ_SIMPLEEFF_SPATK = 13,
	WSEQ_SIMPLEEFF_AVOID = 16,
	WSEQ_SHRINK = 31,
	WSEQ_MUST_SHRINK = 158,

};

// @@@ ���o�[�W�����̃��U�f�[�^�V�[�P���X���A�V�o�[�W�����̃f�[�^�e�[�u���Ƃ��Ĉ������߂�
// �ꎞ�I�ȕϊ��p�e�[�u��
typedef struct {
	u16 wseq;
	u16 category;

	WazaSick  sick;
	struct {
		WazaRankEffect	type;
		s8							value;
	}rankEff[2];

	u8 shrinkFlg;

}SEQ_PARAM;


static const SEQ_PARAM* getSeqParam( WazaID waza )
{
	static const SEQ_PARAM SeqParamTable[] = {
		{
			WSEQ_SINMPLESICK_NEMURI, WAZADATA_CATEGORY_SIMPLE_SICK, POKESICK_NEMURI, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SINMPLESICK_DOKU, WAZADATA_CATEGORY_SIMPLE_SICK, POKESICK_DOKU, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SINMPLESICK_MAHI, WAZADATA_CATEGORY_SIMPLE_SICK, POKESICK_MAHI, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_DAMAGESICK_DOKU, WAZADATA_CATEGORY_DAMAGE_SICK, POKESICK_DOKU, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_DAMAGESICK_YAKEDO, WAZADATA_CATEGORY_DAMAGE_SICK, POKESICK_YAKEDO, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_DAMAGESICK_KOORI, WAZADATA_CATEGORY_DAMAGE_SICK, POKESICK_KOORI, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_DAMAGESICK_MAHI, WAZADATA_CATEGORY_DAMAGE_SICK, POKESICK_MAHI, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SIMPLEEFF_ATK, WAZADATA_CATEGORY_SIMPLE_EFFECT, POKESICK_NULL, 
			{ { WAZA_RANKEFF_ATTACK, 1 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SIMPLEEFF_DEF, WAZADATA_CATEGORY_SIMPLE_EFFECT, POKESICK_NULL, 
			{ { WAZA_RANKEFF_DEFENCE, 1 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SIMPLEEFF_SPATK, WAZADATA_CATEGORY_SIMPLE_EFFECT, POKESICK_NULL, 
			{ { WAZA_RANKEFF_SP_ATTACK, 1 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
			WSEQ_SIMPLEEFF_AVOID, WAZADATA_CATEGORY_SIMPLE_EFFECT, POKESICK_NULL, 
			{ { WAZA_RANKEFF_AVOID, 1 }, { WAZA_RANKEFF_NULL, 0 } },
			FALSE,
		},{
				WSEQ_SHRINK, WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKESICK_NULL, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			TRUE,
		},{
				WSEQ_MUST_SHRINK, WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKESICK_NULL, 
			{ { WAZA_RANKEFF_NULL, 0 }, { WAZA_RANKEFF_NULL, 0 } },
			TRUE,
		},
	};
	u16 seq = WT_WazaDataParaGet( waza, ID_WTD_battleeffect );
	u16 i;

	for(i=0; i<NELEMS(SeqParamTable); ++i)
	{
		if( SeqParamTable[i].wseq == seq )
		{
			return &SeqParamTable[i];
		}
	}
	return NULL;
}

// ���U�D��x
s8   WAZADATA_GetPriority( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_attackpri );
}
// ���U�J�e�S��
WazaCategory  WAZADATA_GetCategory( WazaID id )
{
	const SEQ_PARAM* seqparam = getSeqParam( id );

	if( seqparam )
	{
		return seqparam->category;
	}

	if( WAZADATA_IsDamage(id) )
	{
		return WAZADATA_CATEGORY_SIMPLE_DAMAGE;
	}
	else
	{
		return WAZADATA_CATEGORY_OTHERS;
	}
}

PokeType WAZADATA_GetType( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_wazatype );
}

u16 WAZADATA_GetPower( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_damage );
}

WazaDamageType WAZADATA_GetDamageType( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_kind );
}

// ������
u8 WAZADATA_GetHitRatio( WazaID id )
{
	u8 ret = WT_WazaDataParaGet( id, ID_WTD_hitprobability );
	TAYA_Printf("[WAZATL] GetHitRatio : waza=%d, ratio=%d\n", id, ret);
	if( ret == 0 )
	{
		ret = 100;
	}
	return ret;
}

// �K���t���O�B�΂߂������Ȃǂ�TRUE��Ԃ��B
BOOL WAZADATA_IsAlwaysHit( WazaID id )
{
	return WAZADATA_GetHitRatio(id) == HITRATIO_MUST;
}

// �_���[�W���U���ǂ���
BOOL WAZADATA_IsDamage( WazaID id )
{
	return WAZADATA_GetPower(id) != 0;
}

// �N���e�B�J�������N
u8 WAZADATA_GetCriticalRank( WazaID id )
{
	return 0;
}

// �ō��q�b�g��
u8 WAZADATA_GetMaxHitCount( WazaID id )
{
	return 1;
}

// ���ʔ͈�
WazaTarget WAZADATA_GetTarget( WazaID id )
{
	enum {
		OLD_RANGE_NORMAL		= 0,
		OLD_RANGE_NONE			= 1,
		OLD_RANGE_RANDOM		= 2,
		OLD_RANGE_ENEMY2		= 4,
		OLD_RANGE_OTHER_ALL	= 8,
		OLD_RANGE_USER			= 16,
		OLD_RANGE_FRIEND_ALL = 32,
		OLD_RANGE_WEATHER		= 64,
		OLD_RANGE_MAKIBISI	= 128,
		OLD_RANGE_TEDASUKE	= 256,
		OLD_RANGE_TUBO			= 512,
		OLD_RANGE_SAKIDORI	= 1024,
	};

	u32 old_range = WT_WazaDataParaGet( id, ID_WTD_attackrange );

	switch( old_range ){
	case OLD_RANGE_NORMAL:		return WAZA_TARGET_SINGLE;
	case OLD_RANGE_NONE:			return WAZA_TARGET_UNKNOWN;
	case OLD_RANGE_ENEMY2:		return WAZA_TARGET_ENEMY2;
	case OLD_RANGE_OTHER_ALL:	return WAZA_TARGET_OTHER_ALL;
	case OLD_RANGE_USER:			return WAZA_TARGET_ONLY_USER;
	case OLD_RANGE_FRIEND_ALL:return WAZA_TARGET_TEAM_USER;
	case OLD_RANGE_RANDOM:		return WAZA_TARGET_RANDOM;
	case OLD_RANGE_WEATHER:		return WAZA_TARGET_FIELD;
	case OLD_RANGE_MAKIBISI:	return WAZA_TARGET_TEAM_ENEMY;
	case OLD_RANGE_TEDASUKE:	return WAZA_TARGET_OTHER_FRIEND;
	case OLD_RANGE_TUBO:			return WAZA_TARGET_SINGLE_FRIEND;
	case OLD_RANGE_SAKIDORI:	return WAZA_TARGET_SINGLE_ENEMY;
	}

	return WAZA_TARGET_SINGLE;///< �����ȊO�̂P�́i�I���j
}

// �ǉ����ʂ̏�Ԉُ��Ԃ�
PokeSick WAZADATA_GetSick( WazaID id )
{
	const SEQ_PARAM* seq = getSeqParam( id );
	if( seq ){
		return seq->sick;
	}
	return POKESICK_NULL;
}

//=============================================================================================
/**
 * �ǉ��̏�Ԉُ픭������Ԃ�
 *
 * @param   id		���UID
 *
 * @retval  u32		�������i�p�[�Z���e�[�W = 0�`100�j
 */
//=============================================================================================
u32 WAZADATA_GetSickPer( WazaID id )
{
	// @@@ ���͂Ă��Ƃ���
	return WT_WazaDataParaGet( id, ID_WTD_addeffect );
}

//=============================================================================================
/**
 * �ǉ��́u�Ђ�݁v����Ԃ�
 *
 * @param   id		���UID
 *
 * @retval  u32		�������i�p�[�Z���e�[�W = 0�`100�j
 */
//=============================================================================================
u32 WAZADATA_GetShrinkPer( WazaID id )
{
	const SEQ_PARAM* seq = getSeqParam( id );
	if( seq && seq->shrinkFlg ){
		return WT_WazaDataParaGet( id, ID_WTD_addeffect );
	}
	return 0;
}


#ifdef PM_DEBUG
void WAZADATA_PrintDebug( void )
{
	static const struct {
		const char* name;
		u16 id;
	}tbl[] = {
		{ "�݂��ł��ۂ��i�ʏ�j",			WAZANO_MIZUDEPPOU },
		{ "�˂��Ղ��i����Q�́j",			WAZANO_NEPPUU },
		{ "�Ȃ݂̂�i�R�́j",					WAZANO_NAMINORI },
		{ "�����Ԃ񂵂�i�����j",			WAZANO_KAGEBUNSIN },
		{ "���낢����i��j",					WAZANO_KUROIKIRI },
		{ "�������i�����_���j",			WAZANO_KUROIKIRI },
		{ "���܂����i��j",						WAZANO_AMAGOI },
		{ "���΂��i�����_���j",			WAZANO_ABARERU },
		{ "�A���}�Z���s�[�i�����Q�́j",WAZANO_AROMASERAPII },
		{ "�܂��т��i�܂��т��j",				WAZANO_MAKIBISI },
		{ "�Ă������i�Ă������j",				WAZANO_TEDASUKE },
		{ "�ڂ��i�ڂ��j",				WAZANO_TUBOWOTUKU },
		{ "�����ǂ�i�����ǂ�j",				WAZANO_SAKIDORI },
		{ "�I�E���������i�Ȃ��j",		WAZANO_OUMUGAESI },
	};
	u32 i, range;

	OS_TPrintf("***** ���U���ʔ͈͒l�`�F�b�N *****\n");
	for(i=0; i<NELEMS(tbl); ++i)
	{
		range = WT_WazaDataParaGet( tbl[i].id, ID_WTD_attackrange );
		OS_TPrintf("%s\t=%d\n", tbl[i].name, range);
		
	}
}

#endif
