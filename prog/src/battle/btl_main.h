#ifndef __BTL_MAIN_H__
#define __BTL_MAIN_H__

#include "battle/battle.h"

#include "btl_common.h"
#include "btl_pokeparam.h"

typedef struct _BTL_MAIN_MODULE		BTL_MAIN_MODULE;

typedef struct _BTL_PARTY		BTL_PARTY;


extern BtlRule BTL_MAIN_GetRule( const BTL_MAIN_MODULE* wk );
extern BtlCompetitor BTL_MAIN_GetCompetitor( const BTL_MAIN_MODULE* wk );
extern BtlCommMode BTL_MAIN_GetCommMode( const BTL_MAIN_MODULE* wk );
extern const BTL_PARTY* BTL_MAIN_GetPartyDataConst( const BTL_MAIN_MODULE* wk, u32 clientID );
extern BtlPokePos BTL_MAIN_GetClientPokePos( const BTL_MAIN_MODULE* wk, u8 clientID, u8 posIdx );
extern BtlPokePos BTL_MAIN_GetOpponentPokePos( const BTL_MAIN_MODULE* wk, BtlPokePos basePos, u8 idx );
extern BtlPokePos BTL_MAIN_GetNextPokePos( const BTL_MAIN_MODULE* wk, BtlPokePos basePos );
extern BtlPokePos BTL_MAIN_PokeIDtoPokePos( const BTL_MAIN_MODULE* wk, u8 pokeID );
extern BOOL BTL_MAIN_IsOpponentClientID( const BTL_MAIN_MODULE* wk, u8 clientID1, u8 clientID2 );
extern const BTL_POKEPARAM* BTL_MAIN_GetPokeParamConst( const BTL_MAIN_MODULE* wk, u8 pokeID );
extern const BTL_POKEPARAM* BTL_MAIN_GetFrontPokeDataConst( const BTL_MAIN_MODULE* wk, BtlPokePos pos );
extern BTL_POKEPARAM* BTL_MAIN_GetFrontPokeData( BTL_MAIN_MODULE* wk, BtlPokePos pos );
extern BTL_POKEPARAM* BTL_MAIN_GetClientPokeData( BTL_MAIN_MODULE* wk, u8 clientID, u8 memberIdx );
extern u8 BTL_MAIN_BtlPosToClientID( const BTL_MAIN_MODULE* wk, BtlPokePos pos );
extern u8 BTL_MAIN_BtlPosToViewPos( const BTL_MAIN_MODULE* wk, BtlPokePos pos );
extern BtlSide BTL_MAIN_GetClientSide( const BTL_MAIN_MODULE* wk, u8 clientID );
//-------------------------
extern void BTL_MAIN_CLIENTDATA_SwapPartyMembers( BTL_MAIN_MODULE* wk, u8 clientID, u8 idx1, u8 idx2 );
extern void BTL_MAIN_SyncServerCalcData( BTL_MAIN_MODULE* wk );
extern const BTL_POKEPARAM* BTL_MAIN_GetClientPokeDataConst( const BTL_MAIN_MODULE* wk, u8 clientID, u8 memberIdx );
extern BTL_POKEPARAM* BTL_MAIN_GetPokeParam( BTL_MAIN_MODULE* wk, u8 pokeID );

//=========================================================
// for server only
//=========================================================
extern void BTL_MAIN_BtlPosToClientID_and_PokeIdx( const BTL_MAIN_MODULE* wk, BtlPokePos pos, u8* clientID, u8* pokeIdx );

//=========================================================

extern u8 BTL_PARTY_GetMemberCount( const BTL_PARTY* party );
extern u8 BTL_PARTY_GetAliveMemberCount( const BTL_PARTY* party );
extern const BTL_POKEPARAM* BTL_PARTY_GetMemberDataConst( const BTL_PARTY* party, u8 idx );
extern s16 BTL_PARTY_FindMember( const BTL_PARTY* party, const BTL_POKEPARAM* param );
//-------------------------
extern BTL_POKEPARAM* BTL_PARTY_GetMemberData( BTL_PARTY* party, u8 idx );
extern void BTL_PARTY_SwapMembers( BTL_PARTY* party, u8 idx1, u8 idx2 );


//-------------------------------------------------------------------------------
/**
 *	�T�C�hID�֘A
 */
//-------------------------------------------------------------------------------

static inline BtlSide BTL_MAINUTIL_GetOpponentSide( BtlSide side )
{
	GF_ASSERT(side < BTL_SIDE_MAX);
	return !side;
}

static inline BtlPokePos BTL_MAINUTIL_GetSidePos( BtlSide side, u8 idx )
{
	GF_ASSERT(side < BTL_SIDE_MAX);
	GF_ASSERT(idx < BTL_POSIDX_MAX);

	return (side&1) + idx*2;
}



//-------------------------------------------------------------------------------
/**
 *	����ʒuID�w��q
 */
//-------------------------------------------------------------------------------
typedef enum {

	BTL_EXPOS_DEFAULT = 0,		///< ��������
	BTL_EXPOS_ENEMY_ALL,			///< ����S��
	BTL_EXPOS_WITHOUT_ME,			///< �����ȊO�S��
	BTL_EXPOS_MYSIDE_ALL,			///< �����S��
	BTL_EXPOS_ALL,						///< �G�E�����S��

}BtlExPosType;

typedef u16 BtlExPos;

static inline BtlExPos  EXPOS_MAKE( BtlExPosType type, u8 basePos )
{
	return (type<<8) | basePos;
}

static inline u8 EXPOS_GET_TYPE( BtlExPos exPos )
{
	return (exPos >> 8) & 0xff;
}

static inline u8 EXPOS_GET_BASEPOS( BtlExPos exPos )
{
	return exPos & 0xff;
}



//=============================================================================================
/**
 * ����N���C�A���gID�w��q���A���ۂ̑ΏۃN���C�A���gID�ɕϊ�
 *
 * @param   wk						���C�����W���[���n���h��
 * @param   exPos					����|�P�����ʒu�w��q
 * @param   dst						[out] �Ώۃ|�P�����ʒu���i�[����o�b�t�@�iBTL_POSIDX_MAX�����m�ۂ���Ă��邱�Ɓj
 *
 * @retval  u8		�Ώۃ|�P������
 */
//=============================================================================================
extern u8 BTL_MAIN_ExpandBtlPos( const BTL_MAIN_MODULE* wk, BtlExPos exPos, u8* dst );




#endif
