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

#include	"waza_tool_def.h"



//============================================================================================
/**
 * �萔�錾
 */
//============================================================================================

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


typedef struct {
	u16  id;

	u8   category;
	u8   type;

	u8   dmgType;
	u8   hitRatio;
	u8   power;
	u8   numMaxHit;

	u8   sick1;
	u8   sick1per;

	u8   influ1;
	u8   influ1per;
	s8   influ1volume;

}WAZA_DATA;

static const WAZA_DATA WazaDataTbl[] = {
	{
		WAZANO_NAMINORI,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_WATER,
		WAZADATA_DMG_SPECIAL, 100, 80, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_NULL, 0, 0,
	},{
		WAZANO_KOORINOKIBA,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_KOORI,
		WAZADATA_DMG_PHYSIC, 100, 65, 1,
		WAZASICK_KOORI, 20,
		WAZA_INFLU_NULL, 0, 0,
	},{
		WAZANO_AMAERU,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_NORMAL,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_ATTACK, 0, -2,
	},{
		WAZANO_KAGEBUNSIN,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_NORMAL,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_AVOID, 0, 1,
	},{
		WAZANO_HIKARINOKABE,
		WAZADATA_CATEGORY_SIMPLE_EFFECT, POKETYPE_SP,
		WAZADATA_DMG_NONE, 0, 0, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_AVOID, 0, 1,
	},{
		0,
		WAZADATA_CATEGORY_SIMPLE_DAMAGE, POKETYPE_WATER,
		WAZADATA_DMG_SPECIAL, 100, 80, 1,
		WAZASICK_NULL, 0,
		WAZA_INFLU_NULL, 0, 0,
	},
};


static const WAZA_DATA* getData( WazaID id )
{
	int i;
	for(i=0; WazaDataTbl[i].id != 0; i++)
	{
		if( WazaDataTbl[i].id == id )
		{
			break;
		}
	}

	return &WazaDataTbl[i];
}



s8   WAZADATA_GetPriority( WazaID id )
{
	return WT_WazaDataParaGet( id, ID_WTD_attackpri );
}


WazaCategory  WAZADATA_GetCategory( WazaID id )
{
	const WAZA_DATA* dat = getData( id );
	return dat->category;
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
BOOL WAZADATA_IsAleadyHit( WazaID id )
{
	return FALSE;
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

#if 0
BOOL      WAZADATA_IsDamage( WazaID id );
WazaDamageType  WAZADATA_GetDamageType( WazaID id );
u16  WAZADATA_GetPower( WazaID id );
u16  WAZADATA_GetHitRatio( WazaID id );


// ������A���U�����U�̍ő�񐔁B
extern u16  WAZADATA_GetContiniusCount( WazaID id );

// ���イ����ɓ����郉���N
extern u16  WAZADATA_GetCriticalRatio( WazaID id );

// �ʏ�U�����U�ł���A�ǉ����ʂ��܂ޏꍇ�͒ǉ����ʒl���Ԃ�B
// ��Ԉُ�݂̂��N�������U�i�Ⴆ�΍Ö��p�Ȃǁj�́AWAZASICK_NULL ���Ԃ�B
extern WazaSick  WAZADATA_GetAdditionalSick( WazaID id );

// ����B������͒ǉ����ʂ̊m�����Ԃ�B
extern u8  WAZADATA_GetAdditionalSickRatio( WazaID id );

#endif
