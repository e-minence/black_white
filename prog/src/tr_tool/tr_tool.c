
//============================================================================================
/**
 * @file  tr_tool.c
 * @bfief �g���[�i�[�f�[�^�c�[���S
 * @author  HisashiSogabe
 * @date  05.12.14
 */
//============================================================================================
#include  <gflib.h>

#include  "arc_def.h"

#include  "waza_tool/wazano_def.h"
#include  "tr_tool/tr_tool.h"
#include  "tr_tool/trno_def.h"
#include  "tr_tool/trtype_def.h"
#include  "tr_tool/trtype_sex.h"
#include  "poke_tool/poke_tool.h"
#include  "poke_tool/pokeparty.h"

#include  "pm_define.h"

#include  "message.naix"

//============================================================================================
/**
 * �\���̐錾
 */
//============================================================================================

/*
 *  @brief  �g���[�i�[�^�C�v�Ɉˑ����Č��肷��e��p�����[�^��`
 */
typedef struct _TRTYPE_GRP_PARAM{
  u16  tr_type:7;      ///<�g���[�i�[�^�C�v 7
  u16  type_grp:4;     ///<�g���[�i�[�^�C�v�O���[�v 4
  u16  btl_bg_attr:5;  ///<�퓬�w�iBG�A�g���r���[�g 5
}TRTYPE_GRP_PARAM;

//============================================================================================
/**
 * �萔�錾
 */
//============================================================================================
/*
 *  @brief  �g���[�i�[�^�C�v���O���[�v�����邽�߂̑Ή��e�[�u��
 *
 *  �g������ꍇ�A���V�Ɋm�F�����肢���܂�
 */
static const TRTYPE_GRP_PARAM DATA_TrTypeGrpParam[TRTYPE_GRP_IDX_MAX] = {
 { TRTYPE_RIVAL,    TRTYPE_GRP_RIVAL,	  BATTLE_BG_ATTR_MAX },
 { TRTYPE_SUPPORT,	TRTYPE_GRP_SUPPORT,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER1A,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER1B,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER1C,	TRTYPE_GRP_LEADER,  BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER2,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER3,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER4,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER5,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER6,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER7,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER8A,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_LEADER8B,	TRTYPE_GRP_LEADER,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BIGFOUR1,	TRTYPE_GRP_BIGFOUR,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BIGFOUR2,	TRTYPE_GRP_BIGFOUR,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BIGFOUR3,	TRTYPE_GRP_BIGFOUR,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BIGFOUR4,	TRTYPE_GRP_BIGFOUR,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_CHAMPION, TRTYPE_GRP_CHAMPION,BATTLE_BG_ATTR_MAX	},
 { TRTYPE_BOSS,	    TRTYPE_GRP_TRAINER_BOSS,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BOSS_1,   TRTYPE_GRP_PLASMA_BOSS,	  BATTLE_BG_ATTR_MAX },
 { TRTYPE_BOSS_2,   TRTYPE_GRP_LAST_BOSS,	    BATTLE_BG_ATTR_MAX },
 { TRTYPE_SAGE1,	  TRTYPE_GRP_SAGE,	  BATTLE_BG_ATTR_SAGE },
 { TRTYPE_HAKAIM1,	TRTYPE_GRP_PLASMA,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_HAKAIW1,	TRTYPE_GRP_PLASMA,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_DPCHAMPION,TRTYPE_GRP_DPCHAMP,	BATTLE_BG_ATTR_MAX },
 { TRTYPE_BCHAMP,	  TRTYPE_GRP_BCHAMP,	BATTLE_BG_ATTR_MAX },
};
#ifdef _NITRO
// �\���̂��z��̃T�C�Y�ƂȂ��Ă��邩�`�F�b�N
SDK_COMPILER_ASSERT(sizeof(TRTYPE_GRP_PARAM) == 2);
#endif


//============================================================================================
/**
 * �v���g�^�C�v�錾
 */
//============================================================================================
static  void  rnd_tmp_get( int mons_no, int form_no, int para, u32* rnd_tmp, HEAPID heapID );
static  void  pp_set_param( TrainerID tr_id, POKEMON_PARAM *pp, int form_no, int para );
static inline u32 TTL_SETUP_POW_PACK( u8 pow );

//============================================================================================
/**
 *  @brief  �g���[�i�[�f�[�^���擾����
 *
 *  @param[in]  tr_id   �擾�������g���[�i�[ID
 *  @param[in]  id      �擾�������l�̎��
 *
 *  @retval �擾�����l
 */
//============================================================================================
u32 TT_TrainerDataParaGet( TrainerID tr_id, int id )
{
  u32           ret;
  TRAINER_DATA  td;

  TT_TrainerDataGet( tr_id, &td );

  switch( id ){
  case ID_TD_data_type:   //�f�[�^�^�C�v
    ret = td.data_type;
    break;
  case ID_TD_tr_type:     //�g���[�i�[����
    ret = td.tr_type;
    break;
  case ID_TD_fight_type:
    ret = td.fight_type;
    break;
  case ID_TD_poke_count:  //�����|�P������
    ret = td.poke_count;
    break;
  case ID_TD_use_item1:   //�g�p����1
  case ID_TD_use_item2:   //�g�p����2
  case ID_TD_use_item3:   //�g�p����3
  case ID_TD_use_item4:   //�g�p����4
    ret = td.use_item[ id - ID_TD_use_item1 ];
    break;
  case ID_TD_aibit:       //AI�p�^�[��
    ret = td.aibit;
    break;
  case ID_TD_hp_recover_flag: //�퓬��񕜂��邩�H
    ret = td.hp_recover_flag;
    break;
  case ID_TD_gold:            //�����Â����W��
    ret = td.gold;
    break;
  case ID_TD_gift_item:       //���悷��A�C�e��
    ret = td.gift_item;
    break;
  default:
    //��`����Ă��Ȃ�ID�ł�
    GF_ASSERT( 0 );
    break;
  }
  return ret;
}

//============================================================================================
/**
 *  @brief�g���[�i�[���b�Z�[�W�f�[�^�����݂��邩�`�F�b�N
 *
 *  @param[in]  tr_id   �`�F�b�N�������g���[�i�[ID
 *  @param[in]  kindID  �`�F�b�N���������b�Z�[�W�̎��
 *  @param[in]  heapID  �e���|�����̈�̐�����q�[�vID
 *
 * @retval  TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//============================================================================================
BOOL  TT_TrainerMessageCheck( TrainerID tr_id, int kindID, HEAPID heapID )
{
  ARCHANDLE*  handle;
  int         size;
  u16         ofs;
  u16         data[ 2 ];
  BOOL        ret;

  ret=FALSE;

  size = GFL_ARC_GetDataSize( ARCID_TRMSGTBL, 0 );
  GFL_ARC_LoadDataOfs( &ofs, ARCID_TRMSGTBL_OFS, 0, tr_id * 2, 2 );
  handle = GFL_ARC_OpenDataHandle( ARCID_TRMSGTBL, heapID );

  while( ofs != size ){
    GFL_ARC_LoadDataOfsByHandle( handle, 0, ofs, 4, &data[ 0 ] );
    if( ( data[ 0 ] == tr_id ) && ( data[ 1 ] == kindID ) ){
      ret = TRUE;
      break;
    }
    if( data[ 0 ] != tr_id ){
      break;
    }
    ofs += 4;
  }
  GFL_ARC_CloseDataHandle( handle );

  return ret;
}

//============================================================================================
/**
 *  @brief  �g���[�i�[���b�Z�[�W�f�[�^���擾
 *
 *  @param[in]  tr_id   �擾�������g���[�i�[ID
 *  @param[in]  kindID  �擾���������b�Z�[�W�̎��
 *  @param[out] msg     �擾�������b�Z�[�W�̊i�[��ւ̃|�C���^
 *  @param[in]  heapID  �e���|�����̈�̐�����q�[�vID
 */
//============================================================================================
void  TT_TrainerMessageGet( TrainerID tr_id, int kindID, STRBUF* str, HEAPID heapID )
{
  ARCHANDLE*    handle;
  int           size;
  int           ofs_size;
  u16           ofs;
  u16           data[ 2 ];
  GFL_MSGDATA*  msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_trmsg_dat, heapID );

  size = GFL_ARC_GetDataSize( ARCID_TRMSGTBL, 0 );
#ifdef PM_DEBUG
  ofs_size = GFL_ARC_GetDataSize( ARCID_TRMSGTBL_OFS, 0 );
  if( ofs_size < tr_id * 2 )
  {
    GFL_STR_ClearBuffer( str );
    return;
  }
#endif
  GFL_ARC_LoadDataOfs( &ofs, ARCID_TRMSGTBL_OFS, 0, tr_id * 2, 2 );
  handle = GFL_ARC_OpenDataHandle( ARCID_TRMSGTBL, heapID );

  while( ofs != size ){
    GFL_ARC_LoadDataOfsByHandle( handle, 0, ofs, 4, &data[ 0 ] );
    if( ( data[ 0 ] == tr_id ) && ( data[ 1 ] == kindID ) ){
      GFL_MSG_GetString( msg, ofs / 4, str );
      break;
    }
    ofs += 4;
  }
  GFL_ARC_CloseDataHandle( handle );
  GFL_MSG_Delete( msg );

//  GF_ASSERT_MSG(ofs!=size,"TrainerMessage Not Found");
  //���b�Z�[�W��������Ȃ�������ASTRBUF���N���A���ĕԂ�
  if( ofs == size )
  {
    GFL_STR_ClearBuffer( str );
  }
}

//============================================================================================
/**
 *  @brief�g���[�i�[�\���̃f�[�^���擾
 *
 *  @param[in]  tr_id   �擾�������g���[�i�[ID
 *  @param[in]  td      �擾�����g���[�i�[�f�[�^�̊i�[����w��
 */
//============================================================================================
void  TT_TrainerDataGet( TrainerID tr_id, TRAINER_DATA* td )
{
  GFL_ARC_LoadData( td, ARCID_TRDATA, tr_id );
}

//============================================================================================
/**
 *  @brief  �g���[�i�[�����|�P�����\���̃f�[�^���擾
 *
 *  @param[in]  tr_id   �擾�������g���[�i�[ID
 *  @param[in]  td      �擾�����g���[�i�[�����|�P�����f�[�^�̊i�[����w��
 */
//============================================================================================
void  TT_TrainerPokeDataGet( TrainerID tr_id, void* tpd )
{
  GFL_ARC_LoadData( tpd, ARCID_TRPOKE, tr_id );
}

//============================================================================================
/**
 *  @brief  �g���[�i�[�^�C�v���琫�ʂ��擾����
 *
 *  @param[in]  trtype    �擾�������g���[�i�[�^�C�v
 */
//============================================================================================
u8  TT_TrainerTypeSexGet( int trtype )
{
  return TrTypeSexTable[ trtype ];
}

//----------------------------------------------------------
/*
 *  @brief  �g���[�i�[�^�C�v���g���[�i�[�^�C�v�O���[�vIdx�ɕϊ�
 */
//----------------------------------------------------------
u8 TT_TrainerTypeGrpEntryIdxGet( int trtype )
{
  int i;
  const TRTYPE_GRP_PARAM* prm;

  for(i = 0;i < TRTYPE_GRP_IDX_MAX; i++){
    if( DATA_TrTypeGrpParam[i].tr_type == trtype ){
      return i;
    }
  }
  return TRTYPE_GRP_IDX_MAX;
}
//----------------------------------------------------------
/*
 *  @brief  �g���[�i�[�^�C�v���g���[�i�[�^�C�v�O���[�v�ɕϊ�
 */
//----------------------------------------------------------
TRTYPE_GRP_ID TT_TrainerTypeGrpGet( int trtype )
{
  u8 idx = TT_TrainerTypeGrpEntryIdxGet( trtype );

  if(idx >= TRTYPE_GRP_IDX_MAX){
    return TRTYPE_GRP_NONE;
  }
  return DATA_TrTypeGrpParam[idx].type_grp;
}
//----------------------------------------------------------
/*
 *  @brief  �g���[�i�[�^�C�v����BtlBgAttr���擾
 */
//----------------------------------------------------------
BtlBgAttr TT_TrainerTypeBtlBgAttrGet( int trtype )
{
  u8 idx = TT_TrainerTypeGrpEntryIdxGet( trtype );

  if(idx >= TRTYPE_GRP_IDX_MAX){
    return BATTLE_BG_ATTR_MAX;
  }
  return DATA_TrTypeGrpParam[idx].btl_bg_attr;
}

//============================================================================================
/**
 *  @brief  �G���J�E���g�g���[�i�[�f�[�^���擾����
 *
 *  @param[in/out]  bp      �퓬�V�X�e���Ɉ����n���\���̂̃|�C���^
 *  @param[in]      num     ���Ԗڂ̃g���[�i�[ID�H�i�^�b�O�̎��̂��߁j
 *  @param[in]      heapID  �������m�ۂ��邽�߂̃q�[�vID
 */
//============================================================================================
void TT_EncountTrainerPersonalDataMake( TrainerID tr_id, BSP_TRAINER_DATA* data, HEAPID heapID )
{
  TRAINER_DATA*   td;

  td = GFL_HEAP_AllocMemory( heapID, sizeof( TRAINER_DATA ) );

  TT_TrainerDataGet( tr_id, td );

  data->tr_id = tr_id;
  data->tr_type = td->tr_type;
  data->ai_bit = td->aibit;

  {
    GFL_MSGDATA* man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_trname_dat, heapID );
    GFL_MSG_GetString( man, tr_id, data->name );
    GFL_MSG_Delete(man);
  }

  MI_CpuCopy16( td->use_item, data->use_item, sizeof(u16)*4);
  PMSDAT_Clear( &data->win_word );
  PMSDAT_Clear( &data->lose_word );

  GFL_HEAP_FreeMemory( td );
}

//============================================================================================
/**
 *  @brief  �G���J�E���g�g���[�i�[�����|�P�����f�[�^�𐶐�����
 *
 *  @param[in/out]  bp      �퓬�V�X�e���Ɉ����n���\���̂̃|�C���^
 *  @param[in]      num     ���Ԗڂ̃g���[�i�[ID�H�i�^�b�O�̎��̂��߁j
 *  @param[in]      heapID  �������m�ۂ��邽�߂̃q�[�vID
 */
//============================================================================================
void TT_EncountTrainerPokeDataMake( TrainerID tr_id, POKEPARTY* pparty, HEAPID heapID )
{
  TRAINER_DATA*   td;
  void*           buf;
  int             i, j;
  u32             rnd_tmp, rnd;
  u8              pow;
  POKEMON_PARAM*  pp;
  GFL_STD_RandContext rand_c;

  PokeParty_Init( pparty, TEMOTI_POKEMAX );

  td = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID(heapID), sizeof( TRAINER_DATA ) );
  buf = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID(heapID), sizeof( POKEDATA_TYPE_MULTI ) * TEMOTI_POKEMAX );
  pp = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID(heapID), POKETOOL_GetWorkSize() );

  TT_TrainerDataGet( tr_id, td );
  TT_TrainerPokeDataGet( tr_id, buf );

  if( TT_TrainerTypeSexGet( td->tr_type ) == PTL_SEX_FEMALE )
  {
    rnd_tmp = 120;
  }
  else
  {
    rnd_tmp = 136;
  }

  switch( td->data_type ){
  case DATATYPE_NORMAL:
    {
      POKEDATA_TYPE_NORMAL* ptn;

      ptn = ( POKEDATA_TYPE_NORMAL * )buf;
      for( i = 0 ; i < td->poke_count ; i++ )
      {
        rnd_tmp_get( ptn[ i ].monsno, ptn[ i ].form_no, ptn[ i ].para, &rnd_tmp, heapID );
        //�����|�P�����f�[�^�̃p�����[�^���g�p���āA�|�P�����̌������𐶐�
        rnd = ptn[ i ].pow + ptn[ i ].level + ptn[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
        for( j = 0 ; j < td->tr_type ; j++ )
        {
          rnd = GFL_STD_Rand( &rand_c, 0x10000 );
        }
        rnd = ( rnd << 8 ) + rnd_tmp;
        pow = ptn[ i ].pow * 31 / 255;
        PP_SetupEx( pp,
                    ptn[ i ].monsno,
                    ptn[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
        pp_set_param( tr_id, pp, ptn[ i ].form_no, ptn[ i ].para );
        PokeParty_Add( pparty, pp );
      }
    }
    break;
  case DATATYPE_WAZA:
    {
      POKEDATA_TYPE_WAZA* ptw;

      ptw = ( POKEDATA_TYPE_WAZA * )buf;
      for( i = 0 ; i < td->poke_count ; i++ )
      {
        rnd_tmp_get( ptw[ i ].monsno, ptw[ i ].form_no, ptw[ i ].para, &rnd_tmp, heapID );
        //�����|�P�����f�[�^�̃p�����[�^���g�p���āA�|�P�����̌������𐶐�
        rnd = ptw[ i ].pow + ptw[ i ].level + ptw[i].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
        for( j = 0 ; j < td->tr_type ; j++ )
        {
          rnd = GFL_STD_Rand( &rand_c, 0x10000 );
        }
        rnd = ( rnd << 8 ) + rnd_tmp;
        pow = ptw[ i ].pow * 31 / 255;
        PP_SetupEx( pp,
                    ptw[ i ].monsno,
                    ptw[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
        for( j = 0 ; j < 4 ; j++ )
        {
          PP_SetWazaPos( pp, ptw[ i ].waza[ j ], j );
        }
        pp_set_param( tr_id, pp, ptw[ i ].form_no, ptw[ i ].para );
        PokeParty_Add( pparty, pp );
      }
    }
    break;
  case DATATYPE_ITEM:
    {
      POKEDATA_TYPE_ITEM* pti;

      pti = ( POKEDATA_TYPE_ITEM * )buf;
      for( i = 0 ; i < td->poke_count ; i++ )
      {
        rnd_tmp_get( pti[ i ].monsno, pti[ i ].form_no, pti[ i ].para, &rnd_tmp, heapID );
        //�����|�P�����f�[�^�̃p�����[�^���g�p���āA�|�P�����̌������𐶐�
        rnd = pti[ i ].pow + pti[ i ].level + pti[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
        for( j = 0 ; j < td->tr_type ; j++ )
        {
          rnd = GFL_STD_Rand( &rand_c, 0x10000 );
        }
        rnd = ( rnd << 8 ) + rnd_tmp;
        pow = pti[ i ].pow * 31 / 255;
        PP_SetupEx( pp,
                    pti[ i ].monsno,
                    pti[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
        PP_Put( pp, ID_PARA_item, pti[ i ].itemno );
        pp_set_param( tr_id, pp, pti[ i ].form_no, pti[ i ].para );
        PokeParty_Add( pparty, pp );
      }
    }
    break;
  case DATATYPE_MULTI:
    {
      POKEDATA_TYPE_MULTI*  ptm;

      ptm = ( POKEDATA_TYPE_MULTI * )buf;
      for( i = 0 ; i < td->poke_count ; i++ )
      {
        rnd_tmp_get( ptm[ i ].monsno, ptm[ i ].form_no, ptm[ i ].para, &rnd_tmp, heapID );
        //�����|�P�����f�[�^�̃p�����[�^���g�p���āA�|�P�����̌������𐶐�
        rnd = ptm[ i ].pow + ptm[ i ].level + ptm[ i ].monsno + tr_id;
        GFL_STD_RandInit( &rand_c, rnd );
        for( j = 0 ; j < td->tr_type ; j++ )
        {
          rnd = GFL_STD_Rand( &rand_c, 0x10000 );
        }
        rnd = ( rnd << 8 ) + rnd_tmp;
        pow = ptm[ i ].pow * 31 / 255;
        PP_SetupEx( pp,
                    ptm[ i ].monsno,
                    ptm[ i ].level,
                    PTL_SETUP_ID_NOT_RARE,
                    TTL_SETUP_POW_PACK( pow ),
                    rnd );
        PP_Put( pp, ID_PARA_item, ptm[ i ].itemno );
        for( j = 0 ; j < 4 ; j++ )
        {
          PP_SetWazaPos( pp, ptm[ i ].waza[ j ], j );
        }
        pp_set_param( tr_id, pp, ptm[ i ].form_no, ptm[ i ].para );
        PokeParty_Add( pparty, pp );
      }
    }
    break;
  }
  GFL_HEAP_FreeMemory( td );
  GFL_HEAP_FreeMemory( buf );
  GFL_HEAP_FreeMemory( pp );
}

//============================================================================================
/**
 *              �O�����J���Ȃ��֐��S
 */
//============================================================================================

//============================================================================================
/**
 * @brief �|�P�����̓����Ɛ��ʂ����肷�闐���l���Z�o����
 *
 * @param[in]     mons_no �Z�o���郂���X�^�[�i���o�[
 * @param[in]     form_no �Z�o����t�H�����i���o�[
 * @param[in]     para    �����Ɛ��ʃp�����[�^
 * @param[in/out] rnd_tmp ���߂闐���l
 */
//============================================================================================
static  void  rnd_tmp_get( int mons_no, int form_no, int para, u32* rnd_tmp, HEAPID heapID )
{
  int sex     = para & 0x0f;
  int speabi  = ( para & 0xf0 ) >> 4;
  POKEMON_PERSONAL_DATA*  ppd;

  if( para == 0 ){
    return;
  }

  ppd = POKE_PERSONAL_OpenHandle( mons_no, form_no, heapID );

  if( sex ){
    *rnd_tmp = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
    if( sex == 1 ){
      *rnd_tmp += 2;
    }
    else{
      *rnd_tmp -= 2;
    }
  }

  if( speabi == 1 ){
    *rnd_tmp &= 0xfffffffe;
  }
  else if( speabi == 2 ){
    *rnd_tmp |= 0x00000001;
  }

  POKE_PERSONAL_CloseHandle( ppd );
}

//============================================================================================
/**
 * @brief PP_Setup��̃p�����[�^�ݒ�
 *
 * @param[in] pp      POKEMON_PARAM�\���̂̃|�C���^
 * @param[in] form_no �t�H�����i���o�[
 */
//============================================================================================
static  void  pp_set_param( TrainerID tr_id, POKEMON_PARAM* pp, int form_no, int para )
{
  //�Ȃ��x�̏����l��255
  u8  friend = 255;
  int no;
  u16 mons_no = PP_Get( pp, ID_PARA_monsno, NULL );

  for( no = 0 ; no < PTL_WAZA_MAX ; no++ ){
    //�������������Ă���Ƃ��́A�Ȃ��x��0�ɂ���
    if( PP_Get( pp, ID_PARA_waza1 + no , NULL ) == WAZANO_YATUATARI ){
      friend = 0;
    }
  }
  PP_Put( pp, ID_PARA_friend, friend );

  //�t�H�����i���o�[�Z�b�g
  PP_Put( pp, ID_PARA_form_no, form_no );

  //�����Z�b�g
  { 
    if( ( para & 0xf0 ) == 0x30 )
    { 
      PP_SetTokusei3( pp, mons_no, form_no );
    }
    else if( para & 0xf0 )
    { 
      u16 param = POKEPER_ID_speabi1;
      if( POKETOOL_GetPersonalParam( mons_no, form_no, POKEPER_ID_speabi2 ) )
      { 
        if( ( para & 0xf0 ) == 0x20 )
        { 
          param = POKEPER_ID_speabi2;
        }
      }
      PP_Put( pp, ID_PARA_speabino, POKETOOL_GetPersonalParam( mons_no, form_no, param ) );
    }
  }

  //���i�Z�b�g
  { 
    //����̃g���[�i�[�̓���|�P�����̐��i���Œ�ɂ���
    if( ( tr_id == TRID_BOSS_2_06 ) && ( mons_no == MONSNO_RESIRAMU ) )
    { 
      PP_Put( pp, ID_PARA_seikaku, PTL_SEIKAKU_YOUKI );
    }
    else if( ( tr_id == TRID_BOSS_2_07 ) && ( mons_no == MONSNO_ZEKUROMU ) )
    { 
      PP_Put( pp, ID_PARA_seikaku, PTL_SEIKAKU_OKUBYOU );
    }
    else
    { 
      int rnd = PP_Get( pp, ID_PARA_personal_rnd, NULL );

      //����1�o�C�g�͐��ʂ̂��߂ɕ΂肪����̂ŁA���24bit�Ōv�Z����
      rnd >>= 8;

      PP_Put( pp, ID_PARA_seikaku, rnd % PTL_SEIKAKU_MAX );
    }
  }
}

//============================================================================================
/**
 * @brief �̒l��C�ӂ̒l�ɐݒ肷�邽�߂̕ϐ��p�b�L���O����
 *
 * @param[in] pow �p���[�����l
 */
//============================================================================================
static inline u32 TTL_SETUP_POW_PACK( u8 pow )
{
  return ( (pow&0x1f)|((pow&0x1f)<<5)|((pow&0x1f)<<10)|((pow&0x1f)<<15)|((pow&0x1f)<<20)|((pow&0x1f)<<25) );
}

