//============================================================================================
/**
 * @file    natsuki.c
 * @bfief   �Ȃ��x�v�Z
 * @author  HisashiSogabe
 * @date    10.02.05
 */
//============================================================================================
#include    <gflib.h>

#include  "poke_tool/natsuki.h"
#include  "item/item.h"
#include  "poke_tool/monsno_def.h"
#include  "gamesystem/g_power.h"

//============================================================================================
/**
 *	�Ȃ��x�v�Z�e�[�u��
 */
//============================================================================================
static	const	s8	CalcNatsukiTable[][3]={
	{  5,  3,  2 },		//���x���A�b�v
	{  0,  0,  0 },		//�A�C�e���g�p�i�A�C�e���f�[�^����擾�j
	{  3,  2,  1 },		//�{�X��ɎQ���i�W�����[�_�[�A�l�V���A�`�����v�j
	{  1,  1,  1 },		//�A�����
	{ -1, -1, -1 },		//�m��
	{ -5, -5,-10 },		//���x���R�O���ȏ�̕m��
	{  3,  2,  1 },		//�~���[�W�J���ɎQ��
};

static  void	NATSUKI_CalcAct( POKEMON_PARAM *pp, const s8* natsuki_table, ZONEID zoneID, HEAPID heapID );

//============================================================================================
/**
 *	�Ȃ��x�v�Z�i�ėp�j
 *
 * @param[in]	pp		        �Ȃ��x���v�Z����|�P�����\���̂̃|�C���^
 * @param[in]	calcID	      �v�Z���邽�߂�ID
 * @param[in]	zoneID	      ���ݒnID
 * @param[in]	heapID	      �q�[�vID
 */
//============================================================================================
void  NATSUKI_Calc( POKEMON_PARAM* pp, CALC_NATSUKI calcID, ZONEID zoneID, HEAPID heapID )
{ 
  NATSUKI_CalcAct( pp, CalcNatsukiTable[ calcID ], zoneID, heapID );
}

//============================================================================================
/**
 *	�Ȃ��x�v�Z�i�A�C�e���g�p�j
 *
 * @param[in]	pp		        �Ȃ��x���v�Z����|�P�����\���̂̃|�C���^
 * @param[in]	item_no	      �g�p�����A�C�e��
 * @param[in]	zoneID	      ���ݒnID
 * @param[in]	heapID	      �q�[�vID
 */
//============================================================================================
void  NATSUKI_CalcUseItem( POKEMON_PARAM* pp, u16 item_no, ZONEID zoneID, HEAPID heapID )
{ 
  int i;
  s8  natsuki_table[ 3 ];

  for( i = 0 ; i < 3 ; i++ )
  { 
    natsuki_table[ i ] = ITEM_GetParam( item_no, ITEM_PRM_FRIEND1_POINT + i, heapID );
  }

  NATSUKI_CalcAct( pp, natsuki_table, zoneID, heapID );
}

//============================================================================================
/**
 *	�Ȃ��x�v�Z�i�A������j
 *
 * @param[in]	pp		        �Ȃ��x���v�Z����|�P�����\���̂̃|�C���^
 * @param[in]	zoneID	      ���ݒnID
 * @param[in]	heapID	      �q�[�vID
 */
//============================================================================================
void  NATSUKI_CalcTsurearuki( POKEMON_PARAM* pp, ZONEID zoneID, HEAPID heapID )
{ 
	//�A������̂Ȃ��x�v�Z�̊m����1/2
	if( GFL_STD_MtRand( 2 ) & 1 )
  {
  	return;
	}

  NATSUKI_CalcAct( pp, CalcNatsukiTable[ CALC_NATSUKI_TSUREARUKI ], zoneID, heapID );
}

//============================================================================================
/**
 *	�Ȃ��x�v�Z�i�{�X��ɎQ���j
 *
 * @param[in]	ppt		        �Ȃ��x���v�Z����|�P�p�[�e�B�\���̂̃|�C���^
 * @param[in]	zoneID	      ���ݒnID
 * @param[in]	heapID	      �q�[�vID
 */
//============================================================================================
void  NATSUKI_CalcBossBattle( POKEPARTY* ppt, ZONEID zoneID, HEAPID heapID )
{ 
  int i;
  POKEMON_PARAM* pp;

  for( i = 0 ; i < PokeParty_GetPokeCount( ppt ) ; i++ )
  { 
    pp = PokeParty_GetMemberPointer( ppt, i );
    NATSUKI_CalcAct( pp, CalcNatsukiTable[ CALC_NATSUKI_BOSS_BATTLE ], zoneID, heapID );
  }
}

//============================================================================================
/**
 *	�Ȃ��x�v�Z
 *
 * @param[in]	pp		        �Ȃ��x���v�Z����|�P�����\���̂̃|�C���^
 * @param[in]	natsuki_table �Ȃ��x�v�Z�e�[�u��
 * @param[in]	zoneID	      ���ݒnID
 * @param[in]	heapID	      �q�[�vID
 */
//============================================================================================
static  void	NATSUKI_CalcAct( POKEMON_PARAM *pp, const s8* natsuki_table, ZONEID zoneID, HEAPID heapID )
{
	u16	monsno;
	u16	itemno;
	int	natsuki;
	int	natsukitmp;
	u8	ofs;
	u8	eqp;

	monsno = PP_Get( pp, ID_PARA_monsno_egg , NULL );
	if( ( monsno == 0 ) || ( monsno == MONSNO_TAMAGO ) )
  {
		return;
	}

	itemno  = PP_Get( pp, ID_PARA_item, NULL );
	eqp     = ITEM_GetParam( itemno, ITEM_PRM_EQUIP, heapID );

	ofs = 0;
	natsuki = PP_Get( pp, ID_PARA_friend, NULL );

	if( natsuki >= 100 )
  {
		ofs++;
	}
	if( natsuki >= 200 )
  {
		ofs++;
	}

	natsukitmp = natsuki_table[ ofs ];

	// �S�[�W���X�{�[�����Ƃ���1�オ��
	if( ( natsukitmp > 0 ) && ( PP_Get( pp, ID_PARA_get_ball, NULL ) == ITEM_GOOZYASUBOORU ) )
  {
		natsukitmp++;
	}

	// �������ߊl���ꂽ�ꏊ���Ƃ����P�オ��
  if( zoneID )
  { 
	  if( ( natsukitmp > 0 ) && ( PP_Get( pp, ID_PARA_get_place, NULL ) == zoneID ) )
    {
		  natsukitmp++;
	  }
  }

	// �Ȃ��x�A�b�v�̓���������Ă����Ƃ���1.5�{�A�b�v
	if( ( natsukitmp > 0 ) && ( eqp == SOUBI_NATUKIDOUP ) )
  {
	  natsukitmp = natsukitmp * 150 / 100;
	}

  //G�p���[�v�Z����
  natsukitmp = GPOWER_Calc_Natsuki( natsukitmp );

	// 0�ȉ��ɂ�255�ȏ�ɂ����Ȃ�
	natsuki += natsukitmp;

	if( natsuki < 0 )
  {
		natsuki = 0;
	}
	if( natsuki > 255 )
  {
		natsuki = 255;
	}

	PP_Put( pp, ID_PARA_friend, natsuki );
}

