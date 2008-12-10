//============================================================================================
/**
 * @file	pokeparty.c
 * @brief	�|�P�����p�[�e�B����
 * @author	tamada	GAME FREAK Inc.
 * @date	2005.10.13
 */
//============================================================================================
#include	<gflib.h>

#include	"pm_define.h"
#include	"poke_tool/poke_tool.h"
#include	"poke_tool_def.h"

//�Z�[�u�n�����Ȃ��ł�
#ifdef DEBUG_ONLY_FOR_sogabe
#warning	SaveData Nothing
#endif
#include	"poke_tool/pokeparty.h"
#include	"pokeparty_local.h"

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�w��ʒu�����������ǂ����̔���
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos		�|�P�����̈ʒu�i�O�I���W���j
 */
//----------------------------------------------------------
#define	PARTY_POS_ASSERT(party, pos)	{\
	GF_ASSERT((pos) >= 0);				\
	GF_ASSERT_MSG((pos) < (party)->PokeCount, "pos(%d) >= PokeCount(%d)\n",pos, (party)->PokeCount); \
	GF_ASSERT((pos) < (party)->PokeCountMax) \
}

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	POKEPARTY�\���̂̃T�C�Y�擾
 * @return	int	POKEPARTY�\���̂̑傫��
 */
//----------------------------------------------------------
int PokeParty_GetWorkSize( void )
{
	return sizeof( POKEPARTY );
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY�\���̂̃��[�N�m��
 * @param	heapID		���������m�ۂ���q�[�v�ւ̃|�C���^
 * @return	POKEPARTY�ւ̃|�C���^
 */
//----------------------------------------------------------
POKEPARTY * PokeParty_AllocPartyWork( HEAPID heapID )
{
	POKEPARTY * party;

	party = GFL_HEAP_AllocMemory( heapID, sizeof( POKEPARTY ) );
	PokeParty_InitWork( party );

	return party;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY�̏���������
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 */
//----------------------------------------------------------
void PokeParty_InitWork( POKEPARTY * party )
{
	PokeParty_Init( party, TEMOTI_POKEMAX );
}

//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	POKEPARTY�̏���������
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	max		POKEPARTY���ێ��ł���ő�̃|�P������
 */
//----------------------------------------------------------
void PokeParty_Init( POKEPARTY * party, int max )
{
	int i;
	GF_ASSERT( max <= TEMOTI_POKEMAX );
	MI_CpuClearFast( party, sizeof( POKEPARTY ) );
	party->PokeCount = 0;
	party->PokeCountMax = max;
	for (i = 0; i < TEMOTI_POKEMAX; i++) {
		PP_Clear( &party->member[i] );
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY�Ƀ|�P������������
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	poke	������|�P�����ւ̃f�[�^
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//----------------------------------------------------------
BOOL PokeParty_Add( POKEPARTY * party, const POKEMON_PARAM * poke )
{
	if ( party->PokeCount >= party->PokeCountMax ) {
		//�莝���������ς�
		return FALSE;
	}
	party->member[ party->PokeCount ] = *poke;
	party->PokeCount ++;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
	return TRUE;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����|�P��������菜��
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos		��菜���|�P�����̈ʒu�i�O�I���W���j
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//----------------------------------------------------------
BOOL PokeParty_Delete( POKEPARTY * party, int pos )
{
	int i;
	PARTY_POS_ASSERT( party, pos );
	GF_ASSERT( party->PokeCount > 0 );

	for ( i = pos ;i < party->PokeCount - 1; i ++ ) {
		party->member[i] = party->member[i + 1];
	}
	PP_Clear( &party->member[i] );
	party->PokeCount--;

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
	return TRUE;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����ێ��ł���|�P�������̍ő���擾
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�莝����
 */
//----------------------------------------------------------
int	PokeParty_GetPokeCountMax( const POKEPARTY * party )
{
	return party->PokeCountMax;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����莝�������擾
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�莝����
 */
//----------------------------------------------------------
int	PokeParty_GetPokeCount( const POKEPARTY * party )
{
	return party->PokeCount;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����|�P�����ւ̃|�C���^���擾
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos		�Q�Ƃ������|�P�����̈ʒu�i�O�I���W���j
 * @retval	POKEMON_PARAM	�w�肵���|�P�����ւ̃|�C���^
 */
//----------------------------------------------------------
POKEMON_PARAM * PokeParty_GetMemberPointer( const POKEPARTY * party, int pos )
{
	PARTY_POS_ASSERT( party, pos );
	return (POKEMON_PARAM*)&party->member[pos];
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY�փ|�P�����f�[�^�������Z�b�g
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos		�Z�b�g�������|�P�����̈ʒu�i�O�I���W���j
 * @param	pp		�Z�b�g�������|�P�����f�[�^
 *
 * �{�b�N�X����E���Ă����|�P�����Ɓu����ւ���v�����ɕ֗��Ȃ̂ō쐬�����B
 * �ʏ�A�����o�[�̒ǉ��ɂ� PokeParty_Add ���g���Ă��������B  taya
 *
 */
//----------------------------------------------------------
void PokeParty_SetMemberData( POKEPARTY* party, int pos, POKEMON_PARAM* pp )
{
	int cnt;

	PARTY_POS_ASSERT(party, pos);

	cnt = PP_Get(&(party->member[pos]), ID_PARA_poke_exist, NULL) - PP_Get(pp, ID_PARA_poke_exist, NULL);
	party->member[pos] = *pp;
	party->PokeCount += cnt;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY���Ń|�P�����̈ʒu�����ւ���
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos1	�|�P�����̈ʒu�i�O�I���W���j
 * @param	pos2	�|�P�����̈ʒu�i�O�I���W���j
 * @retval	TRUE	����
 * @retval	FALSE	���s
 */
//----------------------------------------------------------
BOOL PokeParty_ExchangePosition( POKEPARTY * party, int pos1, int pos2, HEAPID heapID )
{
	struct pokemon_param * temp;

	PARTY_POS_ASSERT(party, pos1);
	PARTY_POS_ASSERT(party, pos2);

	temp = GFL_HEAP_AllocMemory( heapID, sizeof(struct pokemon_param) );
	*temp = party->member[pos1];
	party->member[pos1] = party->member[pos2];
	party->member[pos2] = *temp;
	GFL_HEAP_FreeMemory( temp );

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
	return FALSE;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY�̃R�s�[
 * @param	src		POKEPARTY�\���̂ւ̃|�C���^
 * @param	dst		POKEPARTY�\���̂ւ̃|�C���^
 */
//----------------------------------------------------------
void PokeParty_Copy(const POKEPARTY * src, POKEPARTY * dst)
{
	*dst = *src;
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY���Ƀ����X�^�[�����邩�ǂ����H
 * @param	ppt		�`�F�b�N����POKEPARTY�\���̂ւ̃|�C���^
 * @param	mons_no	���邩�`�F�b�N���郂���X�^�[�i���o�[
 * @return	FALSE:���Ȃ��@TRUE:����
 */
//----------------------------------------------------------
BOOL PokeParty_PokemonCheck(const POKEPARTY * ppt, int mons_no)
{
	int				i;

	for(i=0;i<ppt->PokeCount;i++){
		if(PP_Get((POKEMON_PARAM*)&ppt->member[i],ID_PARA_monsno,NULL)==mons_no){
			break;
		}
	}

	return (i!=ppt->PokeCount);
}

//�Z�[�u�n�����Ȃ��ł�
#ifdef DEBUG_ONLY_FOR_sogabe
#warning	SaveData Nothing
#endif

#if 0
//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�莝���|�P�����f�[�^�̎擾
 * @param	sv	�Z�[�u�f�[�^�ւ̃|�C���^
 * @return	POKEPARTY	�莝���|�P�����f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
POKEPARTY * SaveData_GetTemotiPokemon(SAVEDATA * sv)
{
	POKEPARTY * party;
	party = (POKEPARTY *)SaveData_Get(sv, GMDATA_ID_TEMOTI_POKE);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_CheckCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
	return party;
}
#endif

//----------------------------------------------------------
/**
 * @brief	�f�o�b�O�F�_�~�[�p�[�e�B�̐���
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 */
//----------------------------------------------------------
void Debug_PokeParty_MakeParty(POKEPARTY * party)
{
	struct pokemon_param poke;
	int i;
	PokeParty_Init(party, TEMOTI_POKEMAX);
	for (i = 0; i < 3; i++) {
		PP_Clear(&poke);
		PP_Setup( &poke, 392+i, 99, 0 );
		PokeParty_Add(party, &poke);
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_TEMOTI_POKE)
	SVLD_SetCrc(GMDATA_ID_TEMOTI_POKE);
#endif //CRC_LOADCHECK
}

// �O���Q�ƃC���f�b�N�X����鎞�̂ݗL��(�Q�[�����͖���)
#ifdef CREATE_INDEX
void *Index_Get_PokeCount_Offset(POKEPARTY *pt){ return &pt->PokeCount; }
#endif
