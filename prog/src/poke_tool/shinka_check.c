//============================================================================================
/**
 * @file    shinka_check.c
 * @bfief   �i���`�F�b�N�֘A�֐��Q
 * @author  HisashiSogabe
 * @date    09.12.02
 */
//============================================================================================
#include <gflib.h>
#include "item/item.h"
#include "poke_tool/monsno_def.h"
#include "system/rtc_tool.h"

#include "poke_tool/shinka_check.h"
#include "shinka_def.h"
#include "shinka_check.cdat"

#include "arc_def.h"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================
struct pokemon_shinka_data{
	u16	ShinkaCond;
	u16	ShinkaData;
	u16	ShinkaMons;
};

struct pokemon_shinka_table{
	POKEMON_SHINKA_DATA	psd[7];
};

static	void	PokeShinkaDataGet( int mons_no, POKEMON_SHINKA_TABLE *pst );
static  BOOL  PokeWazaHaveCheck( POKEMON_PARAM* pp, WazaID waza_no );

//============================================================================================
/**
 *	�i���`�F�b�N
 *
 *	@param[in]	ppt			    �`�F�b�N����|�P�p�[�e�B�\���̂̃|�C���^
 *	@param[in]	pp			    �`�F�b�N����|�P�����̍\���̂̃|�C���^
 *	@param[in]	type		    �i���^�C�v
 *	@param[in]	param		    �A�C�e���i���`�F�b�N���͎g�p�A�C�e��
 *	                        �ꏊ�i���̏ꍇ�́AZONEID
 *	                        �ʐM�Z���i���̏ꍇ�́A���������POKEMON_PARAM�̃A�h���X
 *	@param[out]	cond        �i�������i�[���[�N�ւ̃|�C���^
 *	@param[in]	heapID      �q�[�vID
 *
 *	@retval	0 �i������ 0�ȊO �i�������|�P�����i���o�[
 */
//============================================================================================
u16	SHINKA_Check( POKEPARTY *ppt, POKEMON_PARAM *pp, SHINKA_TYPE type, u32 param, SHINKA_COND *cond, HEAPID heapID )
{
	POKEMON_SHINKA_TABLE	*pst;
	u16	mons_no;
	u16	item_no;
	u8	level;
	int	i, j;
	u16	ret=0;
	u16	friend;
	u32	personal_rnd;
	u8	eqp;
	u8	beautiful;
	u16	rnd = 0;
	SHINKA_COND	dummy;

	mons_no       = PP_Get( pp, ID_PARA_monsno, NULL );
	item_no       = PP_Get( pp, ID_PARA_item, NULL );
	personal_rnd  = PP_Get( pp, ID_PARA_personal_rnd, NULL );
	beautiful     = PP_Get( pp, ID_PARA_beautiful, NULL );
	rnd = ( personal_rnd & 0xffff0000 ) >> 16;

	eqp = ITEM_GetParam( item_no, ITEM_PRM_EQUIP, heapID );

	//�����Q���[�͐�ΐi��
	if( mons_no != MONSNO_YUNGERAA )
  {
		//�A�C�e���Ői�����Ȃ��������ʂ͐i�����Ȃ�
		if( ( eqp == SOUBI_SINKASINAI ) && ( type != SHINKA_TYPE_ITEM_CHECK ) )
    {
			return 0;
		}
	}
	//�����݂݃s�`���[�͐i�����Ȃ�
#if 0
	if( ( mons_no == MONSNO_PITYUU ) && ( PP_Get( pp, ID_PARA_form_no, NULL ) == FORMNO_PITYUU_GIZAMIMI ) )
  {
		return 0;
	}		
#endif

	if( cond == NULL )
  {
		cond = &dummy;
	}

	pst = GFL_HEAP_AllocMemory( heapID, sizeof( POKEMON_SHINKA_TABLE ) );
	PokeShinkaDataGet( mons_no, pst );

	switch( type ){
	case SHINKA_TYPE_LEVELUP:
		level   = PP_Get( pp, ID_PARA_level, NULL );
		friend  = PP_Get( pp, ID_PARA_friend, NULL );
		for( i = 0 ; i < SHINKA_MAX ; i++ )
    {
			switch( pst->psd[ i ].ShinkaCond ){
			case SHINKA_COND_FRIEND_HIGH:
				if( SHINKA_FRIEND <= friend )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_FRIEND_HIGH_NOON:
				if( ( GFL_RTC_IsNightTime() == FALSE ) && ( SHINKA_FRIEND <= friend ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_FRIEND_HIGH_NIGHT:
				if( ( GFL_RTC_IsNightTime() == TRUE ) && ( SHINKA_FRIEND <= friend ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_LEVELUP:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_SPECIAL_POW:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) > ( PP_Get( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_SPECIAL_EVEN:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) == ( PP_Get ( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_SPECIAL_DEF:
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( PP_Get( pp, ID_PARA_pow, NULL ) ) < ( PP_Get( pp, ID_PARA_def, NULL ) ) )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_SPECIAL_RND_EVEN:		//��������10�Ŋ������]�肪�T����
				if( pst->psd[ i ].ShinkaData <= level)
        {
					if( ( rnd % 10 ) < 5 )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_SPECIAL_RND_ODD:		//��������10�Ŋ������]�肪�T�ȏ�
				if( pst->psd[ i ].ShinkaData <= level )
        {
					if( ( rnd % 10 ) >= 5 )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_SPECIAL_LEVELUP:		//�莝���ɋ󂫂�����΁A�i�����A�J�����c��
				if( pst->psd[ i ].ShinkaData <= level )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_SPECIAL_NUKENIN:		//�莝���ɋ󂫂������
				(*cond) = SHINKA_COND_SPECIAL_NUKENIN;
				break;
			case SHINKA_COND_SPECIAL_BEAUTIFUL:		//�������Ői��
				if( pst->psd[ i ].ShinkaData <= beautiful )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_SOUBI_NOON:
				if( ( GFL_RTC_IsNightTime() == FALSE ) && ( pst->psd[ i ].ShinkaData == item_no ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_SOUBI_NIGHT:
				if( ( GFL_RTC_IsNightTime() == TRUE ) && ( pst->psd[ i ].ShinkaData == item_no ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_WAZA:
				if( PokeWazaHaveCheck( pp, pst->psd[ i ].ShinkaData ) == TRUE )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_POKEMON:
				if( ppt != NULL )
        {
					if( PokeParty_PokemonCheck( ppt, pst->psd[ i ].ShinkaData ) == TRUE )
          {
						ret = pst->psd[ i ].ShinkaMons;
					}
				}
				break;
			case SHINKA_COND_MALE:
				if( ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_MALE ) && ( pst->psd[ i ].ShinkaData <= level ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_FEMALE:
				if( ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_FEMALE ) && ( pst->psd[ i ].ShinkaData <= level ) )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_PLACE_TENGANZAN:
        for( j = 0 ; j < NELEMS( TenganzanShinkaPlaceTable ) ; j++ )
        { 
          if( TenganzanShinkaPlaceTable[ j ] == param )
          { 
					  ret = pst->psd[ i ].ShinkaMons;
            break;
          }
        }
        break;
			case SHINKA_COND_PLACE_KOKE:
        for( j = 0 ; j < NELEMS( KokeShinkaPlaceTable ) ; j++ )
        { 
          if( KokeShinkaPlaceTable[ j ] == param )
          { 
					  ret = pst->psd[ i ].ShinkaMons;
            break;
          }
        }
        break;
			case SHINKA_COND_PLACE_ICE:
        for( j = 0 ; j < NELEMS( IceShinkaPlaceTable ) ; j++ )
        { 
          if( IceShinkaPlaceTable[ j ] == param )
          { 
					  ret = pst->psd[ i ].ShinkaMons;
            break;
          }
        }
        break;
			case SHINKA_COND_PLACE_DENKIDOUKUTSU:
        for( j = 0 ; j < NELEMS( DenkidoukutsuShinkaPlaceTable ) ; j++ )
        { 
          if( DenkidoukutsuShinkaPlaceTable[ j ] == param )
          { 
					  ret = pst->psd[ i ].ShinkaMons;
            break;
          }
        }
				break;
			}
			//�i������������������A������
			if(ret){
				(*cond) = pst->psd[ i ].ShinkaCond;
				break;
			}
		}
		break;
	case SHINKA_TYPE_TUUSHIN:
		for( i = 0 ; i < SHINKA_MAX ; i++ )
    {
			switch( pst->psd[ i ].ShinkaCond ){
			case SHINKA_COND_TUUSHIN:
				ret = pst->psd[ i ].ShinkaMons;
				break;
			case SHINKA_COND_TUUSHIN_ITEM:
				if( pst->psd[ i ].ShinkaData == item_no )
        {
					ret = pst->psd[ i ].ShinkaMons;
				}
				break;
			case SHINKA_COND_TUUSHIN_YUUGOU:
        { 
          POKEMON_PARAM*  pp_e = ( POKEMON_PARAM* )param;
          int item_no_e = PP_Get( pp_e, ID_PARA_item, NULL );
          int eqp_e = ITEM_GetParam( item_no_e, ITEM_PRM_EQUIP, heapID );

		      //�A�C�e���Ői�����Ȃ��������ʂ͐i�����Ȃ�
		      if( eqp_e == SOUBI_SINKASINAI )
          {
			      break;
		      }
          //���݂��̃|�P�������J�u�����A�J�b�`���Ȃ�i��
          if( ( PP_Get( pp, ID_PARA_monsno, NULL ) == MONSNO_KABURIN ) &&
              ( PP_Get( pp, ID_PARA_monsno, NULL ) == MONSNO_KATTYU ) )
          { 
					  ret = pst->psd[ i ].ShinkaMons;
          }
        }
        break;
			}
			//�i������������������A������
			if( ret )
      {
				(*cond) = pst->psd[ i ].ShinkaCond;
				break;
			}
		}
		break;
	case SHINKA_TYPE_ITEM:
	case SHINKA_TYPE_ITEM_CHECK:
		for( i = 0 ; i < SHINKA_MAX ; i++ )
    {
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_COND_ITEM ) && ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				break;
			}
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_COND_ITEM_MALE ) &&
			    ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_MALE ) &&
			    ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				break;
			}
			if( ( pst->psd[ i ].ShinkaCond == SHINKA_COND_ITEM_FEMALE ) &&
			    ( PP_Get( pp, ID_PARA_sex, NULL ) == PTL_SEX_FEMALE ) &&
			    ( pst->psd[ i ].ShinkaData == param ) )
      {
				ret = pst->psd[ i ].ShinkaMons;
				break;
			}
			//�i������������������A������
			if( ret )
      {
				(*cond) = 0;
				break;
			}
		}
		break;
	}
	GFL_HEAP_FreeMemory( pst );
	return ret;
}

//============================================================================================
/**
 *	�i���e�[�u����ArcHandle���擾
 *
 * @param[in]	heapID	ArcHandle���m�ۂ��邽�߂�heapID
 */
//============================================================================================
ARCHANDLE*  SHINKA_GetArcHandle( HEAPID heapID )
{ 
  return GFL_ARC_OpenDataHandle( ARCID_EVOTBL, heapID );
}

//============================================================================================
/**
 *	�i���e�[�u���̃p�����[�^��ArcHandle����Ď擾
 *
 * @param[in]	handle	ArcHandle
 * @param[in]	mons_no �擾����|�P�����i���o�[
 * @param[in]	index	  ���Ԗڂ̃f�[�^���擾���邩�i�ő�V�j
 * @param[in]	param	  �擾����p�����[�^
 */
//============================================================================================
u16 SHINKA_GetParamByHandle( ARCHANDLE* handle, int mons_no, int index, SHINKA_PARAM param )
{ 
  POKEMON_SHINKA_TABLE  pst;

  GF_ASSERT( index < SHINKA_MAX );

  GFL_ARC_LoadDataByHandle( handle, mons_no, &pst );

  switch( param ){ 
  case SHINKA_PARAM_COND:
    return pst.psd[ index ].ShinkaCond;
  case SHINKA_PARAM_DATA:
    return pst.psd[ index ].ShinkaData;
  case SHINKA_PARAM_MONS:
    return pst.psd[ index ].ShinkaMons;
  }

  return 0;
}

//============================================================================================
/**
 *	�i���e�[�u���\���̃f�[�^���擾
 *
 * @param[in]	mons_no	�擾�������|�P�����i���o�[
 * @param[out]	psd		�擾�����i���f�[�^�̊i�[����w��
 */
//============================================================================================
static	void	PokeShinkaDataGet( int mons_no, POKEMON_SHINKA_TABLE *pst )
{
	GFL_ARC_LoadData( pst, ARCID_EVOTBL, mons_no );
}

//============================================================================================
/**
 *	�|�P�������w�肳�ꂽ�Z�������Ă��邩�`�F�b�N
 *
 * @param[in]	pp	      �`�F�b�N����POKEMON_PAMAM�\����
 * @param[in]	waza_no		�`�F�b�N����Z
 *
 * @retval  FALSE:�����Ă��Ȃ��@TRUE:�����Ă���
 */
//============================================================================================
static  BOOL  PokeWazaHaveCheck( POKEMON_PARAM* pp, WazaID waza_no )
{ 
  int i;
  BOOL  ret = FALSE;

  for( i = ID_PARA_waza1 ; i < ID_PARA_waza4 + 1 ; i++ )
  { 
    if( PP_Get( pp, i, NULL ) == waza_no )
    {
      ret = TRUE;
      break;
    }
  }
  
  return ret;
}

