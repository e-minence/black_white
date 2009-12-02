//============================================================================================
/**
 * @file    pokerus.c
 * @bfief   �|�P���X�֘A�֐��Q
 * @author  HisashiSogabe
 * @date    09.12.02
 */
//============================================================================================
#include <gflib.h>

#include "poke_tool/pokerus.h"

//============================================================================================
/**
 *  �v���g�^�C�v�錾
 */
//============================================================================================
static  inline  u32 No2Bit( u32 no );

//==============================================================================
/**
 *	�|�P�����Ƀ|�P���X���������邩�`�F�b�N
 *
 * @param	ppt		POKEPARTY�\���̂ւ̃|�C���^
 *
 * @retval  none		
 */
//==============================================================================
void	POKERUS_CheckCatchPokerus( POKEPARTY *ppt )
{
	POKEMON_PARAM	*pp;
	u16				rnd;
	u8				pokerus;
	int				count;
	int				pos;
  u8        infect;

	count = PokeParty_GetPokeCount( ppt );
	rnd = GFL_STD_MtRand( 0x10000 );
	infect = POKERUS_CheckInfected( ppt );

	if( ( rnd == 16384 ) || ( rnd == 32768 ) || ( rnd == 49152 ) )
  {
		do{
			pos = GFL_STD_MtRand( count );
			pp = PokeParty_GetMemberPointer( ppt, pos );
			if( ( PP_Get( pp, ID_PARA_monsno, NULL ) ) && ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == 0 ) )
      {
				break;
			}
			else
      {
				pos = count;
			}
		}while( pos == count );
		if( ( infect & No2Bit( pos ) ) == 0 )
    {
			do{
				pokerus = GFL_STD_MtRand( 0x100 );
			}while( ( pokerus & 0x07 ) == 0 );
			if( pokerus & 0xf0 )
      {
				pokerus &= 0x07;
			}
			pokerus |= ( pokerus << 4 );
			pokerus &= 0xf3;
			pokerus++;
			PP_Put( pp, ID_PARA_pokerus, pokerus );
		}
	}
}

//============================================================================================
/**
 *	�|�P���X�̓����J�E���^�[���f�N�������g����
 *
 * @param[in]	ppt		�`�F�b�N����POKEPARTY�\���̂̃|�C���^
 * @param[in]	day		�o�ߓ���
 *
 * @retval	none
 */
//============================================================================================
void	POKERUS_DecCounter( POKEPARTY *ppt, s32 day )
{
	POKEMON_PARAM	*pp;
	int	pos;
	int	count;
	u8	pokerus;

	count = PokeParty_GetPokeCount( ppt );

	for( pos = 0 ; pos < count ; pos++ )
  {
		pp = PokeParty_GetMemberPointer( ppt, pos );
		if( PP_Get( pp, ID_PARA_monsno, NULL ) )
    {
			pokerus = PP_Get( pp, ID_PARA_pokerus, NULL );
			if( pokerus & 0x0f )
      {
				if( ( ( pokerus & 0x0f ) < day ) || ( day > 4 ) )
        {
					pokerus &= 0xf0;
				}
				else
        {
					pokerus -= day;
				}
				if( pokerus == 0 )
        {
					pokerus = 0x10;
				}
				PP_Put( pp, ID_PARA_pokerus, pokerus );
			}
		}
	}
}

//============================================================================================
/**
 *	�|�P���X�̓`���`�F�b�N���[�`���i�|�P��������|�P�����ւ̊����j
 *
 * @param[in]	ppt	�`�����`�F�b�N����POKEPARTY�\���̂̃|�C���^
 */
//============================================================================================
void	POKERUS_CheckContagion( POKEPARTY *ppt )
{
	POKEMON_PARAM	*pp;
	int	pos;
	int	count;
	u8	pokerus;

	count = PokeParty_GetPokeCount( ppt );

	if( GFL_STD_MtRand( 3 ) == 0 )
  { 
		for( pos = 0 ; pos < count ; pos++ )
    {
			pp = PokeParty_GetMemberPointer( ppt, pos );
			if( PP_Get( pp, ID_PARA_monsno, NULL ) )
      {
				pokerus = PP_Get( pp, ID_PARA_pokerus, NULL );
				if( pokerus & 0x0f )
        {
					if( pos != 0 )
          {
						pp = PokeParty_GetMemberPointer( ppt, pos - 1 );
						if( ( PP_Get( pp, ID_PARA_pokerus, NULL ) & 0xf0 ) == 0 )
            {
							PP_Put( pp, ID_PARA_pokerus, pokerus );
						}
					}
					if( pos < count - 1 )
          {
						pp = PokeParty_GetMemberPointer( ppt, pos + 1 );
						if( ( PP_Get( pp, ID_PARA_pokerus, NULL ) & 0xf0 ) == 0 )
            {
							PP_Put( pp, ID_PARA_pokerus, pokerus );
							pos++;
						}
					}
				}
			}
		}
	}
}

//============================================================================================
/**
 *	�|�P�������|�P���X�Ɋ������Ă��邩�`�F�b�N
 *
 * @param[in]	ppt �������`�F�b�N����POKEPARTY�\���̂̃|�C���^
 *
 * @retval	�������Ă���|�P�������r�b�g�ŕԂ�
 */
//============================================================================================
u8	POKERUS_CheckInfect( POKEPARTY *ppt )
{
	POKEMON_PARAM	*pp;
	int	pos;
	int	count;
	u8	infect = 0;

	count = PokeParty_GetPokeCount( ppt );

	for( pos = 0 ; pos < count ; pos++)
  {
		pp = PokeParty_GetMemberPointer( ppt, pos );
		if( POKERUS_CheckInfectPP( pp ) )
    { 
			infect |= No2Bit( pos );
		}
	}

	return infect;
}

//============================================================================================
/**
 *	�|�P�������|�P���X�Ɋ������Ă��邩�`�F�b�N�i�ߋ��Ɋ������Ă������Ƃ����݁j
 *
 * @param[in]	ppt �������`�F�b�N����POKEPARTY�\���̂̃|�C���^
 *
 * @retval	�������Ă���|�P�������r�b�g�ŕԂ�
 */
//============================================================================================
u8	POKERUS_CheckInfected( POKEPARTY *ppt )
{
	POKEMON_PARAM	*pp;
	int	pos;
	int	count;
	u8	infect = 0;

	count = PokeParty_GetPokeCount( ppt );

	for( pos = 0 ; pos < count ; pos++)
  {
		pp = PokeParty_GetMemberPointer( ppt, pos );
		if( PP_Get( pp, ID_PARA_pokerus, NULL ) )
    { 
			infect |= No2Bit( pos );
		}
	}

	return infect;
}

//============================================================================================
/**
 *	�|�P���X�Ɋ������Ă��邩�`�F�b�N
 *
 * @param[in]	pp	�������`�F�b�N����POKEMON_PARAM�\���̂̃|�C���^
 *
 * @retval FALSE:�������Ă��Ȃ��@TRUE:�������Ă���
 */
//============================================================================================
BOOL	POKERUS_CheckInfectPP( POKEMON_PARAM *pp )
{
	return POKERUS_CheckInfectPPP( PP_GetPPPPointer( pp ) );
}

//============================================================================================
/**
 *	�|�P���X�Ɋ������Ă��邩�`�F�b�N
 *
 * @param[in]	ppp	�������`�F�b�N����POKEMON_PASO_PARAM�\���̂̃|�C���^
 *
 * @retval FALSE:�������Ă��Ȃ��@TRUE:�������Ă���
 */
//============================================================================================
BOOL	POKERUS_CheckInfectPPP( POKEMON_PASO_PARAM *ppp )
{
	return ( ( PPP_Get( ppp, ID_PARA_pokerus, NULL ) & 0x0f ) != 0 );
}

//============================================================================================
/**
 *	�ߋ��Ƀ|�P���X�Ɋ������Ă������`�F�b�N
 *
 * @param[in]	pp	�������`�F�b�N����POKEMON_PARAM�\���̂̃|�C���^
 *
 * @retval FALSE:�������Ă��Ȃ��@TRUE:�������Ă���
 */
//============================================================================================
BOOL	POKERUS_CheckInfectedPP( POKEMON_PARAM *pp )
{ 
	return POKERUS_CheckInfectedPPP( PP_GetPPPPointer( pp ) );
}

//============================================================================================
/**
 *	�ߋ��Ƀ|�P���X�Ɋ������Ă������`�F�b�N
 *
 * @param[in]	ppp	�������`�F�b�N����POKEMON_PASO_PARAM�\���̂̃|�C���^
 *
 * @retval FALSE:�������Ă��Ȃ��@TRUE:�������Ă���
 */
//============================================================================================
BOOL	POKERUS_CheckInfectedPPP( POKEMON_PASO_PARAM *ppp )
{
	u8	pokerus;

	pokerus = PPP_Get( ppp, ID_PARA_pokerus, NULL );
	
	//���A�������Ă���ꍇ�́AFALSE
	if( pokerus & 0x0f )
  {
		return FALSE;
	}

	return ( ( pokerus & 0xf0 ) != 0 );
}

//============================================================================================
/**
 *	�i���o�[���r�b�g�ɕϊ�
 */
//============================================================================================
static  inline  u32 No2Bit( u32 no )
{ 
  GF_ASSERT( no < 32 );

  return ( 1 << ( no ) );
}

