//=============================================================================================
/**
 * @file	btl_string.c
 * @brief	�|�P����WB �o�g�� �����񐶐�����
 * @author	taya
 *
 * @date	2008.10.06	�쐬
 */
//=============================================================================================
#include <gflib.h>

#include "btl_str.h"

#include "btl_common.h"
#include "btl_string.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
	MSGDATA_1 = 0,
	MSGDATA_MAX,
};


static struct {

	const BTL_MAIN_MODULE*	mainModule;		///< ���C�����W���[��
	const BTL_CLIENT*		client;			///< UI�N���C�A���g
	GFL_MSGDATA*			msg[ MSGDATA_MAX ];	///< ���b�Z�[�W�f�[�^�n���h��
	HEAPID					heapID;			///< �q�[�vID
	

}SysWork;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void ms_encount( STRBUF* dst );
static void ms_select_action_ready( STRBUF* dst );
static void ms_escape( STRBUF* dst, const int* args );
static void ms_sp_waza_announce( STRBUF* dst, const int* args );
static void ms_sp_waza_avoid( STRBUF* dst, const int* args );
static void ms_sp_waza_dead( STRBUF* dst, const int* args );
static void ms_rankup( STRBUF* dst, const int* args );
static void ms_rankdown( STRBUF* dst, const int* args );
static void ms_rankdown_fail( STRBUF* dst, const int* args );




void BTL_STR_InitSystem( const BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, HEAPID heapID )
{
	SysWork.mainModule = mainModule;
	SysWork.client = client;
	SysWork.heapID = heapID;
}

void BTL_STR_MakeStringGeneric( STRBUF* buf, BtlGenStrID strID )
{
	static void (* const funcTbl[])( STRBUF* buf ) = {
		ms_encount,
		ms_select_action_ready,
	};

	GF_ASSERT(strID < NELEMS(funcTbl));

	funcTbl[strID]( buf );
}

void BTL_STR_MakeStringSpecific( STRBUF* buf, BtlSpStrID strID, const int* args )
{
	static void (* const funcTbl[])( STRBUF*, const int* ) = {
		ms_escape,
		NULL,
		NULL,
		ms_sp_waza_dead,
		NULL,
		ms_sp_waza_avoid,
		NULL,
		ms_sp_waza_announce,
		ms_rankdown,
		ms_rankup,
		ms_rankdown_fail,
	};

	GF_ASSERT(strID < NELEMS(funcTbl));

	if( funcTbl[strID ] != NULL )
	{
		funcTbl[strID]( buf, args );
	}
	else
	{
		sprintf( buf, "�y�p�ӂ���Ă��Ȃ������� %d �z\n", strID );
	}
}


static void ms_encount( STRBUF* dst )
{
	const BTL_PARTY* party;
	const BTL_POKEPARAM* poke;
	u16 monsno;
	u8 clientID;

	clientID = BTL_CLIENT_GetOpponentClientID( SysWork.client, 0 );
	party = BTL_MAIN_GetPartyDataConst( SysWork.mainModule, clientID );
	poke = BTL_PARTY_GetMemberDataConst( party, 0 );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );

	sprintf( dst, "�₹���� %s ����яo�����I\n", BTRSTR_GetMonsName(monsno) );
}

static void ms_select_action_ready( STRBUF* dst )
{
	const BTL_PARTY* party;
	const BTL_POKEPARAM* poke;
	u16 monsno;
	u8 clientID;

	clientID = BTL_CLIENT_GetClientID( SysWork.client );
	poke = BTL_MAIN_GetFrontPokeDataConst( SysWork.mainModule, clientID );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );

	sprintf( dst, "%s �͂ǂ�����I�H\n", BTRSTR_GetMonsName(monsno) );
}

//--------------------------------------------------------------
/**
 *	���܂��ɂ����ꂽ
 */
//--------------------------------------------------------------
static void ms_escape( STRBUF* dst, const int* args )
{
	sprintf( dst, "���܂��ɂ����ꂽ�I\n" );
}

//--------------------------------------------------------------
/**
 *	�����́~�~���������I  ��
 */
//--------------------------------------------------------------
static void ms_sp_waza_announce( STRBUF* dst, const int* args )
{
	const BTL_WAZA_EXE_PARAM* wep;
	const BTL_POKEPARAM* poke;
	WazaID  waza;
	u16 monsno;

	wep = BTL_CLIENT_GetWazaExeParam( SysWork.client, args[0] );
	poke = BTL_CLIENT_WEP_GetUserPokeParam( wep );
	waza = BTL_CLIENT_WEP_GetWazaNumber( wep );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );

	sprintf( dst, "%s��%s�I\n", BTRSTR_GetMonsName(monsno), BTRSTR_GetWazaName(waza) );
}

//--------------------------------------------------------------
/**
 *	���U���͂��ꂽ�I
 */
//--------------------------------------------------------------
static void ms_sp_waza_avoid( STRBUF* dst, const int* args )
{
	sprintf( dst, "�������@�͂��ꂽ�I\n");
}

//--------------------------------------------------------------
/**
 *	�~�~�͓|�ꂽ�I
 */
//--------------------------------------------------------------
static void ms_sp_waza_dead( STRBUF* dst, const int* args )
{
	const BTL_POKEPARAM* poke;
	u16 monsno;

	poke = BTL_MAIN_GetFrontPokeDataConst( SysWork.mainModule, args[0] );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );

	sprintf( dst, "%s�͂����ꂽ�I\n", BTRSTR_GetMonsName(monsno) );
}

//--------------------------------------------------------------
/**
 *	�����́~�~�����������I
 */
//--------------------------------------------------------------
static void ms_rankup( STRBUF* dst, const int* args )
{
	const BTL_POKEPARAM* poke;
	u16 monsno;
	u8 statusType;

	poke = BTL_MAIN_GetFrontPokeDataConst( SysWork.mainModule, args[0] );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );
	statusType = args[1];

	sprintf( dst, "%s��%s�����������I\n", BTRSTR_GetMonsName(monsno), BTRSTR_GetStatusTypeName(statusType) );

}
//--------------------------------------------------------------
/**
 *	�����́~�~�����������I
 */
//--------------------------------------------------------------
static void ms_rankdown( STRBUF* dst, const int* args )
{
	const BTL_POKEPARAM* poke;
	u16 monsno;
	u8 statusType;

	poke = BTL_MAIN_GetFrontPokeDataConst( SysWork.mainModule, args[0] );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );
	statusType = args[1];

	sprintf( dst, "%s��%s�����������I\n", BTRSTR_GetMonsName(monsno), BTRSTR_GetStatusTypeName(statusType) );
}

//--------------------------------------------------------------
/**
 *	�����̂̂���傭�͂�����Ȃ��I
 */
//--------------------------------------------------------------
static void ms_rankdown_fail( STRBUF* dst, const int* args )
{
	const BTL_POKEPARAM* poke;
	u16 monsno;

	poke = BTL_MAIN_GetFrontPokeDataConst( SysWork.mainModule, args[0] );
	monsno = BTL_POKEPARAM_GetMonsNo( poke );

	sprintf( dst, "%s�́@�̂���傭�́@������Ȃ��I\n", BTRSTR_GetMonsName(monsno) );
}

