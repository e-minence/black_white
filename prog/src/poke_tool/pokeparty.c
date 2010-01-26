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
#include	"poke_tool/pokeparty.h"
#include	"pokeparty_local.h"
#include     "savedata/save_tbl.h"  //save GMDATA_ID_MYPOKE


//WIFI���p�ɁA�T�[�o�[��POKEPARTY�̃f�[�^���������Ă���̂ŁA�T�C�Y�Œ�̕K�v������B
//�قȂ�����S���҂̖��؋��̂݃R���p�C���G���[���ł�悤�ɂ��Ă���B
#if defined( DEBUG_ONLY_FOR_toru_nagihashi )
SDK_COMPILER_ASSERT( sizeof(POKEPARTY) == 1332 );
#endif

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
static BOOL poke_CheckBattleEnable( const POKEMON_PARAM* pp );
static BOOL poke_CheckNotEgg( const POKEMON_PARAM* pp );
static int pokeParty_getTopIndex( const POKEPARTY* party, BOOL(*check_func)(const POKEMON_PARAM*) );

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
	party->statusView =0;
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
	PokeParty_SetStatusViewIsFront( party , party->PokeCount , TRUE );
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

//--------------------------------------------------------------
/*
 * @brief �^�}�S�������莝���|�P�����̐����擾����
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�^�}�S�������莝����
 */
//--------------------------------------------------------------
int PokeParty_GetPokeCountNotEgg( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;

  // �^�}�S�ȊO�̃|�P���������J�E���g
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    if( tamago_flag != TRUE ) num++;
  }
  return num;
}

//--------------------------------------------------------------
/*
 * @brief �킦��(�^�}�S�ƕm����������)�|�P���������擾����
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�킦��莝����
 */
//--------------------------------------------------------------
int PokeParty_GetPokeCountBattleEnable( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 hp;

  // �킦��|�P���������J�E���g
  for( i=0; i<max; i++ )
  {
    param       = PokeParty_GetMemberPointer( party, i );
    if( poke_CheckBattleEnable( PokeParty_GetMemberPointer( party, i ) ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
/*
 * @brief �^�}�S�̐�(�ʖڃ^�}�S������)���擾����
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�^�}�S�̐�(�ʖڃ^�}�S����)
 */
//--------------------------------------------------------------
int PokeParty_GetPokeCountOnlyEgg( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 tamago_flag;
  u32 fusei_tamago_flag;

  // �ʖڂ���Ȃ��^�}�S�̐����J�E���g
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    tamago_flag       = PP_Get( param, ID_PARA_tamago_flag, NULL );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( ( tamago_flag == TRUE ) && ( fusei_tamago_flag == FALSE ) ) num++;
  }
  return num;
}

//--------------------------------------------------------------
/*
 * @brief �ʖڃ^�}�S�̐����擾����
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�ʖڃ^�}�S�̐�
 */
//--------------------------------------------------------------
int PokeParty_GetPokeCountOnlyDameEgg( const POKEPARTY* party )
{
  int i;
  int num = 0;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������
  POKEMON_PARAM* param;
  u32 fusei_tamago_flag;

  // �ʖڃ^�}�S�̐����J�E���g
  for( i=0; i<max; i++ )
  {
    param             = PokeParty_GetMemberPointer( party, i );
    fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );
    if( fusei_tamago_flag == TRUE ) num++;
  }
  return num;
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����퓬�\�ȃ����o��TopIndex���擾
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�퓬�\�ȃ����o��TopIndex
 */
//----------------------------------------------------------
int PokeParty_GetMemberTopIdxBattleEnable( const POKEPARTY * party )
{
  return pokeParty_getTopIndex( party, poke_CheckBattleEnable );
}

//----------------------------------------------------------
/**
 * @brief	POKEPARTY����^�}�S�łȂ������o��TopIndex���擾
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @retval	�^�}�S�łȂ������o��TopIndex
 */
//----------------------------------------------------------
int PokeParty_GetMemberTopIdxNotEgg( const POKEPARTY * party )
{
  return pokeParty_getTopIndex( party, poke_CheckNotEgg );
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
	{
    const BOOL isFront1 = PokeParty_GetStatusViewIsFront( party , pos1 );
    const BOOL isFront2 = PokeParty_GetStatusViewIsFront( party , pos2 );
    PokeParty_SetStatusViewIsFront( party , pos1 , isFront2 );
    PokeParty_SetStatusViewIsFront( party , pos2 , isFront1 );
  }

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

//----------------------------------------------------------
/**
 * @brief	POKEPARTY���ɐ킦�郂���X�^�[�����邩�ǂ����H
 * @param	ppt		�`�F�b�N����POKEPARTY�\���̂ւ̃|�C���^
 * @return	�o�g���o���鐔
 */
//----------------------------------------------------------
int PokeParty_GetBattlePokeNum(const POKEPARTY * ppt)
{
    POKEMON_PARAM* poke;
    int max = PokeParty_GetPokeCount(ppt);
    int i,num = 0;

	for(i = 0 ; i < max ; i++){
		poke = PokeParty_GetMemberPointer(ppt, i);
        if (PP_Get(poke, ID_PARA_hp, NULL) == 0) {
            continue;
        }
        if (PP_Get(poke, ID_PARA_tamago_flag, NULL)) {
            continue;
        }
        num++;
	}
    return num;
}

//----------------------------------------------------------
/**
 * @brief	�X�e�[�^�X��ʂőO��������������
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos	�ʒu
 * @retval	TRUE	�O����
 * @retval	FALSE	������
 */
//----------------------------------------------------------
BOOL PokeParty_GetStatusViewIsFront( POKEPARTY* party, int pos )
{
  if( (party->statusView & (1<<pos)) != 0 )
  {
    return TRUE;
  }
  return FALSE;

}

//----------------------------------------------------------
/**
 * @brief	�X�e�[�^�X��ʂőO��������������
 * @param	party	POKEPARTY�\���̂ւ̃|�C���^
 * @param	pos	�ʒu
 * @param isFront	TRUE	�O����
 * @param isFront	FALSE	������
 */
//----------------------------------------------------------
void PokeParty_SetStatusViewIsFront( POKEPARTY* party, int pos , const BOOL isFront)
{
  if( isFront == TRUE )
  {
    party->statusView = (party->statusView | (1<<pos));
  }
  else
  {
    party->statusView = (party->statusView&(0xFF-(1<<pos)));
  }
}


//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�莝���|�P�����f�[�^�̎擾
 * @param	sv	�Z�[�u�f�[�^�ւ̃|�C���^
 * @return	POKEPARTY	�莝���|�P�����f�[�^�ւ̃|�C���^
 */
//----------------------------------------------------------
POKEPARTY * SaveData_GetTemotiPokemon(SAVE_CONTROL_WORK * sv)
{
	POKEPARTY * party;
	party = (POKEPARTY *)SaveControl_DataPtrGet(sv, GMDATA_ID_MYPOKE);
	return party;
}

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
}

/*
 *  @brief  �w��̃|�P�������퓬�\���ǂ����Ԃ�
 */
static BOOL poke_CheckBattleEnable( const POKEMON_PARAM* pp )
{
  u32 tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
  u32 hp          = PP_Get( pp, ID_PARA_hp, NULL );
  
  if( ( tamago_flag != TRUE ) && ( 0 < hp ) ) return TRUE;
  return FALSE;
}

/**
 * @brief �w��̃|�P�������^�}�S�łȂ��ꍇTRUE��Ԃ�
 */
static BOOL poke_CheckNotEgg( const POKEMON_PARAM* pp )
{
  u32 tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
  return (tamago_flag != TRUE);
}

/**
 * @brief �����Ɏ�����`�F�b�N�ɓ��Ă͂܂�擪�̃|�P�����ʒu��Ԃ�
 */
static int pokeParty_getTopIndex( const POKEPARTY* party, BOOL(*check_func)(const POKEMON_PARAM*) )
{
  int i;
  int max = PokeParty_GetPokeCount( party );  // �S�|�P������

  // �����ɓ��Ă͂܂�擪�̃����o�[index���T�[�`
  for( i=0; i<max; i++ )
  {
    if( check_func( PokeParty_GetMemberPointer( party, i ) ) ) return i; 
  }
  return max;
}

// �O���Q�ƃC���f�b�N�X����鎞�̂ݗL��(�Q�[�����͖���)
#ifdef CREATE_INDEX
void *Index_Get_PokeCount_Offset(POKEPARTY *pt){ return &pt->PokeCount; }
#endif
