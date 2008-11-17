#ifndef __BTL_MAIN_H__
#define __BTL_MAIN_H__

#include "battle/battle.h"

#include "btl_common.h"
#include "btl_pokeparam.h"

typedef struct _BTL_MAIN_MODULE		BTL_MAIN_MODULE;

typedef struct _BTL_PARTY		BTL_PARTY;


extern BtlRule BTL_MAIN_GetRule( const BTL_MAIN_MODULE* wk );
extern u8 BTL_MAIN_GetFrontMemberIdx( const BTL_MAIN_MODULE* wk, u8 clientID );
extern void BTL_MAIN_SetFrontMemberIdx( BTL_MAIN_MODULE* wk, u8 clientID, u8 memberIdx );
extern BTL_PARTY* BTL_MAIN_GetPartyData( BTL_MAIN_MODULE* wk, u32 clientID );
extern const BTL_PARTY* BTL_MAIN_GetPartyDataConst( const BTL_MAIN_MODULE* wk, u32 clientID );
extern u8 BTL_MAIN_GetOpponentClientID( const BTL_MAIN_MODULE* wk, u8 clientID, u8 idx );
extern u8 BTL_MAIN_GetFriendClientID( const BTL_MAIN_MODULE* wk, u8 cliendID, u8 idx );
extern const BTL_POKEPARAM* BTL_MAIN_GetPokeParam( const BTL_MAIN_MODULE* wk, u8 pokeID );
extern const BTL_POKEPARAM* BTL_MAIN_GetFrontPokeDataConst( const BTL_MAIN_MODULE* wk, u8 clientID );
extern BTL_POKEPARAM* BTL_MAIN_GetFrontPokeData( BTL_MAIN_MODULE* wk, u8 clientID );
extern void BTL_MAIN_SyncServerCalcData( BTL_MAIN_MODULE* wk );


//-------------------------
extern u8 BTL_PARTY_GetMemberCount( const BTL_PARTY* party );
extern u8 BTL_PARTY_GetAliveMemberCount( const BTL_PARTY* party );
extern BTL_POKEPARAM* BTL_PARTY_GetMemberData( BTL_PARTY* party, u8 idx );
extern const BTL_POKEPARAM* BTL_PARTY_GetMemberDataConst( const BTL_PARTY* party, u8 idx );


//-------------------------------------------------------------------------------
/**
 *	����N���C�A���gID�w��q
 */
//-------------------------------------------------------------------------------
typedef enum {

	BTL_EXCLIENT_DEFAULT = 0,		///< ��������
	BTL_EXCLIENT_ENEMY_ALL,			///< ����S��
	BTL_EXCLIENT_ENEMY_RANDOM,		///< ���胉���_���P��
	BTL_EXCLIENT_WITHOUT_ME,		///< �����ȊO�S��

}BtlExClientType;

typedef u16 BtlExClientID;

static inline BtlExClientID  EXID_MAKE( BtlExClientType type, u8 clientID )
{
	return (type<<8) | clientID;
}

static inline u8 EXID_GET_TYPE( BtlExClientID exID )
{
	return (exID >> 8) & 0xff;
}

static inline u8 EXID_GET_CLIENTID( BtlExClientID exID )
{
	return exID & 0xff;
}



//=============================================================================================
/**
 * ����N���C�A���gID�w��q���A���ۂ̑ΏۃN���C�A���gID�ɕϊ�
 *
 * @param   wk						���C�����W���[���n���h��
 * @param   exID					����N���C�A���gID�w��q
 * @param   dst						[out] �ΏۃN���C�A���gID���i�[����o�b�t�@�iBTL_CLIENT_MAX�����m�ۂ���Ă��邱�Ɓj
 *
 * @retval  u8		�ΏۃN���C�A���g��
 */
//=============================================================================================
extern u8 BTL_MAIN_ExpandClientID( const BTL_MAIN_MODULE* wk, BtlExClientID exID, u8* dst );




#endif
