//============================================================================================
/**
 * @file  poke_tool.c
 * @bfief �|�P�����p�����[�^�c�[���Q�iWB����Łj
 * @author  HisashiSogabe
 * @date  08.11.12
 */
//============================================================================================
#include    <gflib.h>

#include    "waza_tool/wazano_def.h"
#include    "poke_tool/poke_personal.h"
#include    "poke_tool/poke_tool.h"
#include    "poke_tool/monsno_def.h"
#include    "poke_tool/tokusyu_def.h"
#include    "waza_tool/wazadata.h"
#include    "print/global_msg.h"
#include    "print/str_tool.h"
#include    "system/gfl_use.h"

#include    "poke_tool_def.h"
#include    "poke_personal_local.h"
#include    "arc_def.h"
#include    "savedata/mail.h"

//�T�C�Y�����ꂽ��WIFI�s���`�F�b�N��ʂ�Ȃ��\��������̂ŁA
//���؋��̂݃R���p�C���G���[�`�F�b�N�����Ă���
#ifdef DEBUG_ONLY_FOR_toru_nagihashi
SDK_COMPILER_ASSERT( sizeof(POKEMON_PARAM) == 220 );
#endif

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  // �e���C�|�P�������Ȃǂ̏����Ŏg���񂷕�����o�b�t�@�T�C�Y�i�C�O�ł��l�����Ē��߂Ɂj
  STRBUFFER_LEN = 32,

  // �Z�o���e�[�u���Ɋւ����`
  LEVELUPWAZA_OBOE_MAX = 22,  ///<�i�Z20�{�I�[�R�[�h:1+4�o�C�g���E�␳:1�ju16�̔z��ŃX�^�e�B�b�N�Ɋm��
//  LEVELUPWAZA_OBOE_END = 0xffff,
//  LEVELUPWAZA_LEVEL_MASK = 0xfe00,
//  LEVELUPWAZA_WAZA_MASK = 0x01ff,
//  LEVELUPWAZA_LEVEL_SHIFT = 9,

  PRAND_TOKUSEI_SHIFT = 16,   ///< �������F�Ƃ�������ނ����肷��Bit��Index
  PRAND_TOKUSEI_MASK = (1 << PRAND_TOKUSEI_SHIFT ),
};

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void  calc_renew_core( POKEMON_PARAM *pp );

static  BOOL  pp_decordAct( POKEMON_PARAM *pp );
static  BOOL  ppp_decordAct( POKEMON_PASO_PARAM *ppp );
static  void  pp_encode_act( POKEMON_PARAM *pp );
static  void  ppp_encode_act( POKEMON_PASO_PARAM *ppp );

static  u32   pp_getAct( POKEMON_PARAM *pp, int id, void *buf);
static  u32   ppp_getAct( POKEMON_PASO_PARAM *ppp, int id, void *buf);
static  void  pp_putAct( POKEMON_PARAM *pp, int paramID, u32 arg );
static  void  ppp_putAct( POKEMON_PASO_PARAM *ppp, int paramID, u32 arg );
static  void  pp_addAct( POKEMON_PARAM *pp, int id, int value);
static  void  ppp_addAct( POKEMON_PASO_PARAM *ppp, int id, int value);

static  void  round_calc( u8 *data, int value, int max );
static  u16   rand_next( u32 *code );
static  u16   calc_abi_seikaku( u8 chr, u16 para, u8 cond );
static  void  load_grow_table( int para, u32 *GrowTable );
static  u32   get_growtbl_param( int para, int level );
static  BOOL  pppAct_check_nickname( POKEMON_PASO_PARAM* ppp );
static POKEMON_PERSONAL_DATA* Personal_Load( u16 monsno, u16 formno );
static u8 Personal_GetTokuseiCount( POKEMON_PERSONAL_DATA* ppd );
static void change_monsno_sub_tokusei( POKEMON_PASO_PARAM* ppp, u16 new_monsno, u16 old_monsno );
static void change_monsno_sub_sex( POKEMON_PASO_PARAM* ppp, u16 new_monsno, u16 old_monsno );
static  PokeType  get_type_from_item( u16 item );
static void PokeParaWazaDelPos(POKEMON_PARAM *pp,u32 pos);

//============================================================================================
/**
 *  �X�^�e�B�b�N�ϐ�
 */
//============================================================================================
static  u32 GrowTable[ GROW_TBL_SIZE ];         //�����Ȑ��e�[�u��
static  STRCODE StrBuffer[STRBUFFER_LEN];       //������o�b�t�@
static  STRCODE StrBufferSub[STRBUFFER_LEN];    //������o�b�t�@
static  POKEMON_PERSONAL_DATA PersonalDataWork; //�|�P�����P�̕��̃p�[�\�i���f�[�^�\����
static  u16  PersonalLatestMonsNo;              //���O�ɓǂݍ��񂾃p�[�\�i���f�[�^�����i�����X�^�[�i���o�[�j
static  u16  PersonalLatestFormNo;              //���O�ɓǂݍ��񂾃p�[�\�i���f�[�^�����i�t�H�����i���o�[�j


//=============================================================================================
/**
 * �V�X�e���������i�v���O�����N����ɂP�x�����Ăяo���j
 *
 * @param   heapID    �V�X�e���������p�q�[�vID
 *
 */
//=============================================================================================
void POKETOOL_InitSystem( HEAPID heapID )
{
  POKE_PERSONAL_InitSystem( heapID );
  PersonalLatestMonsNo = MONSNO_MAX;
}


//============================================================================================
/**
 * @brief POKEMON_PASO_PARAM�\���̂̃T�C�Y��Ԃ�
 *
 * @retval  POKEMON_PASO_PARAM�\���̂̃T�C�Y
 */
//============================================================================================
u32   POKETOOL_GetPPPWorkSize( void )
{
  return sizeof( POKEMON_PASO_PARAM );
}

//============================================================================================
/**
 *  POKEMON_PARAM�\���̂̃T�C�Y��Ԃ�
 *
 * @retval  POKEMON_PARAM�\���̂̃T�C�Y
 */
//============================================================================================
u32   POKETOOL_GetWorkSize( void )
{
  return sizeof( POKEMON_PARAM );
}


//============================================================================================
/**
 * �|�P�����p�����[�^����֐��n
 */
//============================================================================================


//=============================================================================================
/**
 * �|�P�����p�����[�^���̂𐶐�
 *
 * @param   mons_no       �|�P�����i���o�[
 * @param   level         ���x��
 * @param   id            ID
 * @param   heapID        ���̊m�ۗp�̃q�[�vID
 *
 * @retval  POKEMON_PARAM*
 */
//=============================================================================================
POKEMON_PARAM* PP_Create( u16 mons_no, u16 level, u64 id, HEAPID heapID )
{
  POKEMON_PARAM* pp = GFL_HEAP_AllocClearMemory( heapID, POKETOOL_GetWorkSize() );

  if( pp )
  {
    PP_Setup( pp, mons_no, level, id );
  }

  return pp;
}
//=============================================================================================
/**
 * �|�P�����p�\�R���p�����[�^����A�|�P�����p�����[�^���\�z����
 *
 * @param   ppp
 * @param   heapID
 *
 * @retval  POKEMON_PARAM*
 */
//=============================================================================================
POKEMON_PARAM* PP_CreateByPPP( const POKEMON_PASO_PARAM* ppp, HEAPID heapID )
{
  POKEMON_PARAM* pp;
  BOOL fast_flag;
  u32 i=0;

  fast_flag = PPP_FastModeOn( (POKEMON_PASO_PARAM*)ppp );
  {
    u16 mons_no = PPP_Get( ppp, ID_PARA_monsno, NULL );
    u16 level = PPP_Get( ppp, ID_PARA_level, NULL );
    u32 ID = PPP_Get( ppp, ID_PARA_id_no, NULL );

    pp = PP_Create( mons_no, level, ID, heapID );
  }
  PPP_FastModeOff( (POKEMON_PASO_PARAM*)ppp, fast_flag );

  pp->ppp=*ppp;

//�R���f�B�V�����Z�b�g
  PP_Put(pp,ID_PARA_condition,i);
//HP������
  PP_Put(pp,ID_PARA_hp,i);
  PP_Put(pp,ID_PARA_hpmax,i);
  //���[���f�[�^
#if 0
  {
    MAIL_DATA* mail_data = MailData_CreateWork(heapID);
    PP_Put(pp,ID_PARA_mail_data, mail_data);
    sys_FreeMemoryEz(mail_data);
  }
#endif
  PP_Renew(pp);

  return pp;
}


//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̗̈������������
 *
 * @param[in] pp  ����������|�P�����p�����[�^�\���̂̃|�C���^
 */
//============================================================================================
void  PP_Clear( POKEMON_PARAM *pp )
{
  MI_CpuClearFast( pp, sizeof( POKEMON_PARAM ) );

// �p�����[�^���Í���
  POKETOOL_encode_data( &pp->ppp.paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, pp->ppp.checksum );
  POKETOOL_encode_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );
}
//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̗̈������������
 *
 * @param[in] ppp ����������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 */
//============================================================================================
void  PPP_Clear( POKEMON_PASO_PARAM *ppp )
{
  MI_CpuClearFast( ppp, sizeof( POKEMON_PASO_PARAM ) );

// �������Z�b�g�����Ƃ���ňÍ���
  POKETOOL_encode_data( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, ppp->checksum );
}

//=============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂̓��e���\�z
 *
 * @param[out] pp       �\�z��\���̃A�h���X
 * @param[in]  mons_no  �|�P�����i���o�[
 * @param[in]  level    ���x��
 * @param[in]  id       ID
 */
//=============================================================================================
void PP_Setup( POKEMON_PARAM *pp, u16 mons_no, u16 level, u64 ID )
{
  PP_SetupEx( pp, mons_no, level, ID, PTL_SETUP_POW_AUTO, PTL_SETUP_RND_AUTO );
}

//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂Ƀ|�P�����p�����[�^���\�z�y�g���Łz
 *
 * @param[out]  pp       �p�����[�^�𐶐�����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]   mons_no  �|�P�����i���o�[
 * @param[in]   level    ���x��
 * @param[in]   ID       ID�w��iPTL_SETUP_ID_AUTO �Ń����_���j
 * @param[in]   pow      �̒l�w��iPTL_SETUP_POW_AUTO �Ń����_���^�ڂ�����poke_tool.h �Q�Ɓj
 * @param[in]   rnd      �������w��iPTL_SETUP_RND_AUTO�Ń����_���^�ڂ�����poke_tool.h �Q�Ɓj
 */
//============================================================================================
void  PP_SetupEx( POKEMON_PARAM *pp, u16 mons_no, u16 level, u64 ID, PtlSetupPow pow, u64 rnd )
{
  u32     i;
//���[���ƃJ�X�^���{�[���Ή����܂��ł�
#ifdef DEBUG_ONLY_FOR_sogabe
#warning MAIL_DATA and CB_CORE nothing
#endif
//  MAIL_DATA *mail_data;
//  CB_CORE   cb_core;

  PP_Clear( pp );

  PPP_SetupEx( ( POKEMON_PASO_PARAM * )&pp->ppp, mons_no, level, ID, pow, rnd );

//�����������܂�����APCP���ĈÍ���
  POKETOOL_encode_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), 0 );
  POKETOOL_encode_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );

//���x���Z�b�g
  PP_Put( pp, ID_PARA_level, level );

#if 0
//���[���f�[�^
  mail_data = MailData_CreateWork( HEAPID_BASE_SYSTEM );
  PP_Put( pp, ID_PARA_mail_data, mail_data );
  GFL_HEAP_FreeMemory( mail_data );

//�J�X�^���{�[��ID
  i = 0;
  PP_Put( pp, ID_PARA_cb_id, (u8 *)&i );

//�J�X�^���{�[��
  MI_CpuClearFast( &cb_core, sizeof( CB_CORE ) );
  PP_Put( pp, ID_PARA_cb_core, ( CB_CORE * )&cb_core );
#endif

  PP_Renew( pp );
}

//=============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂̓��e���\�z
 *
 * @param[out]  pp      �\�z��\���̃A�h���X
 * @param[in]   mons_no �|�P�����i���o�[
 * @param[in]   level   ���x��
 * @param[in]   id      ID
 *
 */
//=============================================================================================
void  PPP_Setup( POKEMON_PASO_PARAM *ppp, u16 mons_no, u16 level, u64 id )
{
  PPP_SetupEx( ppp, mons_no, level, id, PTL_SETUP_POW_AUTO, PTL_SETUP_RND_AUTO );
}

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂Ƀ|�P�����p�����[�^���\�z�y�g���Łz
 *
 * @param[out]  ppp     �p�����[�^�𐶐�����{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in]   mons_no �|�P�����i���o�[
 * @param[in]   level   ���x��
 * @param[in]   id      ID�w��iPTL_SETUP_ID_AUTO �Ń����_���j
 * @param[in]   pow     �̒l�w��iPTL_SETUP_POW_AUTO �Ń����_���^�ڂ�����poke_tool.h �Q�Ɓj
 * @param[in]   rnd     �������w��iPTL_SETUP_RND_AUTO�Ń����_���^�ڂ�����poke_tool.h �Q�Ɓj
 */
//============================================================================================
void  PPP_SetupEx( POKEMON_PASO_PARAM *ppp, u16 mons_no, u16 level, u64 id, PtlSetupPow pow, u64 rnd )
{
  POKEMON_PERSONAL_DATA* ppd;
  u32   val;
  BOOL  flag;
  u16   sum;

  PPP_Clear( ppp );

  flag = PPP_FastModeOn( ppp );

// �p�[�\�i���f�[�^�����[�h���Ă���
  ppd = Personal_Load( mons_no, PTL_FORM_NONE );

//ID�i���o�[�Z�b�g
  if( id == PTL_SETUP_ID_AUTO )
  {
    id = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
  }
  //�g���[�i�[�̎����|�P�����͌��������Œ�ɂ��ă��A�͏o���Ȃ��悤�ɂ���̂ŁAID�Œ���������
  else if( id == PTL_SETUP_ID_NOT_RARE )
  {
    id = (u32)(rnd+1);
    while( POKETOOL_CheckRare( id, rnd ) ){ id = (id+1)&0xffffffff; }
  }
  PPP_Put( ppp, ID_PARA_id_no, (int)id );

//�������Z�b�g
  if( rnd == PTL_SETUP_RND_AUTO )
  {
    rnd = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
      __GFL_STD_MtRand();
  }
  else if( rnd == PTL_SETUP_RND_RARE )
  {
    rnd = id;
  }
  PPP_Put( ppp, ID_PARA_personal_rnd, (u32)rnd );


#ifdef DEBUG_ONLY_FOR_sogabe
#warning CasetteLanguage Nothing
#endif

//���R�[�h
//  PPP_Put( ppp, ID_PARA_country_code, ( u8 * )&CasetteLanguage );

//�|�P�����i���o�[�Z�b�g
  PPP_Put( ppp, ID_PARA_monsno, mons_no );

//�j�b�N�l�[���Z�b�g
  GFL_MSG_GetStringRaw( GlobalMsg_PokeName, mons_no, StrBuffer, NELEMS(StrBuffer) );
  PPP_Put( ppp, ID_PARA_nickname_raw, (u32)StrBuffer );

//�o���l�Z�b�g
  val = POKETOOL_GetMinExp( mons_no, PTL_FORM_NONE, level );
  PPP_Put( ppp, ID_PARA_exp, val );

//�F�D�l�Z�b�g
  val = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_friend );
  PPP_Put( ppp, ID_PARA_friend, val );

//�ߊl�f�[�^�Z�b�g
  PPP_Put( ppp, ID_PARA_get_level, level );

#ifdef DEBUG_ONLY_FOR_sogabe
#warning CasetteVersion Nothing
#endif
//  PPP_Put( ppp, ID_PARA_get_cassette, ( u8 * )&CasetteVersion );
  //�f�t�H���g�̓����X�^�[�{�[���ɂ��Ă���
  PPP_Put( ppp, ID_PARA_get_ball, ITEM_MONSUTAABOORU );

//�p���[�����Z�b�g
  if( pow == PTL_SETUP_POW_AUTO )
  {
    PPP_Put( ppp, ID_PARA_hp_rnd,     GFL_STD_MtRand(32) );
    PPP_Put( ppp, ID_PARA_pow_rnd,    GFL_STD_MtRand(32) );
    PPP_Put( ppp, ID_PARA_def_rnd,    GFL_STD_MtRand(32) );
    PPP_Put( ppp, ID_PARA_spepow_rnd, GFL_STD_MtRand(32) );
    PPP_Put( ppp, ID_PARA_spedef_rnd, GFL_STD_MtRand(32) );
    PPP_Put( ppp, ID_PARA_agi_rnd,    GFL_STD_MtRand(32) );
  }
  else
  {
    PPP_Put( ppp, ID_PARA_hp_rnd,     PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_HP) );
    PPP_Put( ppp, ID_PARA_pow_rnd,    PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_ATK) );
    PPP_Put( ppp, ID_PARA_def_rnd,    PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_DEF) );
    PPP_Put( ppp, ID_PARA_spepow_rnd, PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_SPATK) );
    PPP_Put( ppp, ID_PARA_spedef_rnd, PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_SPDEF) );
    PPP_Put( ppp, ID_PARA_agi_rnd,    PTL_SETUP_POW_UNPACK(pow, PTL_ABILITY_AGI) );
  }

// �Ƃ������Z�b�g
  {
    u16 param = POKEPER_ID_speabi1;
    if( Personal_GetTokuseiCount(ppd) == 2 ){
      if( rnd & PRAND_TOKUSEI_MASK ){
        param = POKEPER_ID_speabi2;
      }
    }
    val = POKE_PERSONAL_GetParam( ppd, param );
    PPP_Put( ppp, ID_PARA_speabino, val );
  }

//���ʃZ�b�g
  val = PPP_GetSex( ppp );
  PPP_Put( ppp, ID_PARA_sex, val );

//���i�Z�b�g
  val = GFUser_GetPublicRand( PTL_SEIKAKU_MAX );
  PPP_Put( ppp, ID_PARA_seikaku, val );

//�Z�Z�b�g
  PPP_SetWazaDefault( ppp );

  PPP_FastModeOff( ppp, flag );
}

//=============================================================================================
/**
 * �����X�^�[�i���o�[������������B
 * �K�v�ɉ����Čv�Z�������K�v�̂���p�����[�^�����킹�ĕύX����i�i�����Ȃǂ̎g�p��z��j
 *
 * @param   pp
 * @param   monsno
 */
//=============================================================================================
void PP_ChangeMonsNo( POKEMON_PARAM* pp, u16 monsno )
{
  u8 fast_flag = PP_FastModeOn( pp );
  {
    PPP_ChangeMonsNo( &pp->ppp, monsno );
    PP_Renew( pp );
  }
  PP_FastModeOff( pp, fast_flag );
}
//=============================================================================================
/**
 * �����X�^�[�i���o�[������������B
 * �K�v�ɉ����Čv�Z�������K�v�̂���p�����[�^�����킹�ĕύX����i�i�����Ȃǂ̎g�p��z��j
 *
 * @param   ppp
 * @param   mons_no   ����������̃����X�^�[�i���o�[
 */
//=============================================================================================
void PPP_ChangeMonsNo( POKEMON_PASO_PARAM* ppp, u16 next_monsno )
{
  u8 fast_flag = PPP_FastModeOn( ppp );
  {
    u16 old_monsno = PPP_Get( ppp, ID_PARA_monsno, NULL );
    if( old_monsno != next_monsno )
    {
      u16 form_no = PPP_Get( ppp, ID_PARA_form_no, NULL );
      BOOL chg_nickname = pppAct_check_nickname( ppp );

      PPP_Put( ppp, ID_PARA_monsno, next_monsno );
      change_monsno_sub_tokusei( ppp, next_monsno, old_monsno );
      change_monsno_sub_sex( ppp, next_monsno, old_monsno );
      //�f�t�H���g���̂܂܂Ȃ�A�j�b�N�l�[��������������
      if( chg_nickname == FALSE )
      {
        GFL_MSG_GetStringRaw( GlobalMsg_PokeName, next_monsno, StrBuffer, NELEMS(StrBuffer) );
        PPP_Put( ppp, ID_PARA_nickname_raw, (u32)StrBuffer );
      }
    }
  }
  PPP_FastModeOff( ppp, fast_flag );
}

//=============================================================================================
/**
 * �t�H�����i���o�[������������B
 * �K�v�ɉ����Čv�Z�������K�v�̂���p�����[�^�����킹�ĕύX����
 *
 * @param   pp
 * @param   formno
 */
//=============================================================================================
BOOL PP_ChangeFormNo( POKEMON_PARAM* pp, u16 formno )
{
  BOOL  ret;
  u8 fast_flag = PP_FastModeOn( pp );
  {
    ret = PPP_ChangeFormNo( &pp->ppp, formno );
    if( ret == TRUE )
    {
      PP_Renew( pp );
    }
  }
  PP_FastModeOff( pp, fast_flag );

  return ret;
}
//=============================================================================================
/**
 * �t�H�����i���o�[������������B
 * �K�v�ɉ����Čv�Z�������K�v�̂���p�����[�^�����킹�ĕύX����
 *
 * @param   ppp
 * @param   next_formno   ����������̃t�H�����i���o�[
 */
//=============================================================================================
BOOL PPP_ChangeFormNo( POKEMON_PASO_PARAM* ppp, u16 next_formno )
{
  BOOL  ret = FALSE;
  u8 fast_flag = PPP_FastModeOn( ppp );
  {
    u16 old_formno = PPP_Get( ppp, ID_PARA_form_no, NULL );
    if( old_formno != next_formno )
    {
      u16 form_no = PPP_Get( ppp, ID_PARA_form_no, NULL );
      u16 mons_no = PPP_Get( ppp, ID_PARA_monsno, NULL );

      //�|�P�������ŗL�̃`�F�b�N����ق�

      PPP_Put( ppp, ID_PARA_form_no, next_formno );
      change_monsno_sub_tokusei( ppp, mons_no, mons_no );
      change_monsno_sub_sex( ppp, mons_no, mons_no );

      ret = TRUE;
    }
  }
  PPP_FastModeOff( ppp, fast_flag );

  return ret;
}

//--------------------------------------------------------------
/**
 * @brief   ���g�����t�H�����`�F���W����
 *
 * @param   pp          �Z�b�g����POKEMON_PARAM�\���̂ւ̃|�C���^
 * @param   new_form_no     �V�����t�H�����ԍ�
 * @param   del_waza_pos    �Ή��Z���o��������ׁA�Z���폜����ʒu
 *                (�Ή��Z�̍폜���s��ꂽ�ꍇ�A�����ɐV�����Ή��Z����������)
 *
 * @retval  TRUE:�t�H�����`�F���W����
 * @retval  FALSE:���g���ł͂Ȃ�
 */
//--------------------------------------------------------------
BOOL PP_ChangeRotomFormNo(POKEMON_PARAM *pp, int new_form_no, int del_waza_pos)
{
  int monsno;

  monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  if(monsno != MONSNO_ROTOMU){
    return FALSE;
  }

  {
    int i, s, new_wazano, wazano;
    const u16 rotom_waza_tbl[] = {
      0,
      WAZANO_OOBAAHIITO,
      WAZANO_HAIDOROPONPU,
      WAZANO_HUBUKI,
      WAZANO_EASURASSYU,
      WAZANO_RIIHUSUTOOMU,
    };

    new_wazano = rotom_waza_tbl[new_form_no];

    //�Ή��Z��S�ĖY�ꂳ����
    for(i = 0; i < 4; i++)
    {
      wazano = PP_Get(pp, ID_PARA_waza1 + i, NULL);
      for(s = 1; s < NELEMS(rotom_waza_tbl); s++)
      {
        if(wazano != 0 && wazano == rotom_waza_tbl[s])
        {
          if(new_wazano != 0)
          {
            //�V�����t�H�����̋Z�Ɠ���ւ���
            PP_SetWazaPos( pp, new_wazano, i );
            new_wazano = 0;
            break;
          }
          else
          { //�Z�Y��
            PokeParaWazaDelPos(pp, i);
            i--;
            break;
          }
        }
      }
    }

    //�Ή��Z���o���Ă��Ȃ������ꍇ�͂����ŋZ����ւ�
    if(new_wazano != 0)
    {
      for(i = 0; i < 4; i++)
      {
        if(PP_Get(pp, ID_PARA_waza1 + i, NULL) == 0)
        {
          PP_SetWazaPos( pp, new_wazano, i );
          break;
        }
      }
      if(i == 4)
      {
        PP_SetWazaPos( pp, new_wazano, del_waza_pos );
      }
    }

    //�Ή��Z��Y�ꂳ�������ʎ莝���̋Z��0�ɂȂ��Ă�����d�@�V���b�N���o��������
    if(PP_Get(pp, ID_PARA_waza1, NULL) == 0)
    {
      PP_SetWazaPos( pp, WAZANO_DENKISYOKKU, 0 );
    }
  }

  PP_ChangeFormNo( pp, new_form_no );

  return TRUE;
}

//============================================================================================
/**
 * �w��ʒu�̋Z���N���A���āA�V�t�g����
 *
 * @param[in] pp    �N���A����\���̂̃|�C���^
 * @param[in] pos   �N���A�ʒu
 */
//============================================================================================
static void PokeParaWazaDelPos(POKEMON_PARAM *pp,u32 pos)
{
  u32 i;
  u16 waza_no;
  u8  pp_now;
  u8  pp_count;

  for(i=pos;i<3;i++){
    waza_no  = PP_Get(pp,ID_PARA_waza1+i+1,NULL);
    pp_now   = PP_Get(pp,ID_PARA_pp1+i+1,NULL);
    pp_count = PP_Get(pp,ID_PARA_pp_count1+i+1,NULL);

    PP_Put(pp,ID_PARA_waza1+i,waza_no);
    PP_Put(pp,ID_PARA_pp1+i,pp_now);
    PP_Put(pp,ID_PARA_pp_count1+i,pp_count);
  }
  waza_no  = 0;
  pp_now   = 0;
  pp_count = 0;
  PP_Put(pp,ID_PARA_waza4,waza_no);
  PP_Put(pp,ID_PARA_pp4,pp_now);
  PP_Put(pp,ID_PARA_pp_count4,pp_count);
}

//----------------------------------------------------------------------------------
/**
 * �����X�^�[�i���o�[����������� �Ƃ����� �ω�����
 * �iPPP_ChangeMonsNo�̉������j
 *
 * @param   ppp
 * @param   next_monsno
 * @param   old_monsno
 */
//----------------------------------------------------------------------------------
static void change_monsno_sub_tokusei( POKEMON_PASO_PARAM* ppp, u16 next_monsno, u16 old_monsno )
{
  u32 form_no = PPP_Get( ppp, ID_PARA_form_no, NULL );
  POKEMON_PERSONAL_DATA* ppd = Personal_Load( next_monsno, form_no );
  u32 rnd = PPP_Get( ppp, ID_PARA_personal_rnd, NULL );
  u16 param = POKEPER_ID_speabi1;
  if( PPP_Get( ppp, ID_PARA_tokusei_3_flag, NULL ) )
  { 
    param = POKEPER_ID_speabi3;
  }
  else if( Personal_GetTokuseiCount(ppd) == 2 ){
    if( rnd & 0x200 ){
      param = POKEPER_ID_speabi2;
    }
  }

  {
    u32 val = POKE_PERSONAL_GetParam( ppd, param );
    PPP_Put( ppp, ID_PARA_speabino, val );
  }
}
//----------------------------------------------------------------------------------
/**
 * �����X�^�[�i���o�[����������� ���� �ω�����
 * �iPPP_ChangeMonsNo�̉������j
 *
 * @param   ppp
 * @param   new_monsno
 * @param   old_monsno
 */
//----------------------------------------------------------------------------------
static void change_monsno_sub_sex( POKEMON_PASO_PARAM* ppp, u16 next_monsno, u16 old_monsno )
{
  u32 form_no = PPP_Get( ppp, ID_PARA_form_no, NULL );
  POKEMON_PERSONAL_DATA* ppd = Personal_Load( next_monsno, form_no );
  u32 next_param, old_param;

  next_param = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
  ppd = Personal_Load( old_monsno, form_no );
  old_param = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );

  if( PokePersonal_SexVecTypeGet( next_param ) != POKEPER_SEXTYPE_FIX
  ||  (old_param == POKEPER_SEX_UNKNOWN)
  ){
    u32 rnd = PPP_Get( ppp, ID_PARA_personal_rnd, NULL );
    u8 sex = POKETOOL_GetSex( next_monsno, form_no, rnd );
    PPP_Put( ppp, ID_PARA_sex, sex );
  }
}



//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂ւ̃A�N�Z�X�����������[�h��
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂ւ̃|�C���^
 *
 * @retval  FALSE:���łɍ��������[�h�ɂȂ��Ă���@TRUE:���������[�h�ֈڍs
 */
//============================================================================================
BOOL  PP_FastModeOn( POKEMON_PARAM *pp )
{
  BOOL  ret;

  ret = FALSE;

  if( pp->ppp.pp_fast_mode == 0 ){
    ret = TRUE;
    GF_ASSERT_MSG( pp->ppp.ppp_fast_mode == 0, "ppp�����������[�h�ɂȂ��Ă��܂�\n" );
    pp->ppp.pp_fast_mode = 1;
    pp->ppp.ppp_fast_mode = 1;
    POKETOOL_decord_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );
    POKETOOL_decord_data( &pp->ppp.paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, pp->ppp.checksum );
  }

  return ret;
}
//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂ւ̃A�N�Z�X��ʏ탂�[�h��
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂ւ̃|�C���^
 * @param[in] flag  ���[�h�ڍs�\�����f���邽�߂̃t���O�iFALSE:�ڍs�s�ATRUE:�ڍs����j
 *
 * @retval  FALSE:���łɒʏ탂�[�h�ɂȂ��Ă���@TRUE:�ʏ탂�[�h�ֈڍs
 */
//============================================================================================
BOOL  PP_FastModeOff( POKEMON_PARAM *pp, BOOL flag )
{
  BOOL  ret;

  ret = FALSE;

  if( ( pp->ppp.pp_fast_mode == 1 ) && ( flag == TRUE ) ){
    ret = TRUE;
    pp->ppp.pp_fast_mode = 0;
    pp->ppp.ppp_fast_mode = 0;

    POKETOOL_encode_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );
    pp->ppp.checksum = POKETOOL_make_checksum( &pp->ppp.paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX );
    POKETOOL_encode_data( &pp->ppp.paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, pp->ppp.checksum );
  }

  return ret;
}


//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂ւ̃A�N�Z�X�����������[�h��
 *
 * @param[in] ppp �{�b�N�X�|�P�����p�����[�^�\���̂ւ̃|�C���^
 *
 * @retval  FALSE:���łɍ��������[�h�ɂȂ��Ă���@TRUE:���������[�h�ֈڍs
 */
//============================================================================================
BOOL  PPP_FastModeOn( POKEMON_PASO_PARAM *ppp )
{
  BOOL  ret;

  ret = FALSE;

  if( ppp->ppp_fast_mode == 0 ){
    ret = TRUE;
    ppp->ppp_fast_mode = 1;
    POKETOOL_decord_data( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, ppp->checksum );
  }

  return ret;
}

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂ւ̃A�N�Z�X��ʏ탂�[�h��
 *
 * @param[in] ppp   �{�b�N�X�|�P�����p�����[�^�\���̂ւ̃|�C���^
 * @param[in] flag  ���[�h�ڍs�\�����f���邽�߂̃t���O�iFALSE:�ڍs�s�ATRUE:�ڍs����j
 *
 * @retval  FALSE:���łɒʏ탂�[�h�ɂȂ��Ă���@TRUE:�ʏ탂�[�h�ֈڍs
 */
//============================================================================================
BOOL  PPP_FastModeOff( POKEMON_PASO_PARAM *ppp, BOOL flag )
{
  BOOL  ret;

  ret = FALSE;

  if( ( ppp->ppp_fast_mode == 1 ) && ( flag == TRUE ) ){
    ret = TRUE;
    ppp->ppp_fast_mode = 0;


    ppp->checksum = POKETOOL_make_checksum( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX );
    POKETOOL_encode_data( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, ppp->checksum );
  }

  return ret;
}
//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂���C�ӂŒl���擾�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[in] pp  �擾����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id  �擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]  buf �擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return    �擾�����f�[�^
 */
//============================================================================================
u32 PP_Get( const POKEMON_PARAM *pp, int id, void *buf )
{
  u32 ret;

  pp_decordAct( (POKEMON_PARAM*)pp );

  ret = pp_getAct( (POKEMON_PARAM*)pp, id, buf );

  pp_encode_act( (POKEMON_PARAM*)pp );

  return ret;
}

//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[out]  ppp     �i�[�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] paramID   �i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] arg     �i�[�������f�[�^�l�^�z�񓙂̃A�h���X�Ƃ��Ă����p�iparamID�̎w��ɂ��j
 */
//============================================================================================
void  PP_Put( POKEMON_PARAM *pp, int paramID, u32 arg )
{
  if( pp_decordAct( pp ) == TRUE )
  {
    pp_putAct( pp, paramID, arg );
  }

  pp_encode_act( pp );
}
//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[out]  ppp     �i�[�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] paramID   �i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] arg     �i�[�������f�[�^�l�^�z�񓙂̃A�h���X�Ƃ��Ă����p�iparamID�̎w��ɂ��j
 */
//============================================================================================
void  PPP_Put( POKEMON_PASO_PARAM *ppp, int paramID, u32 arg )
{
  if( ppp_decordAct( ppp ) == TRUE )
  {
    ppp_putAct( ppp, paramID, arg );
  }
  ppp_encode_act( ppp );
}

//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂ɔC�ӂŒl�����Z�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[in] pp    ���Z�������|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id    ���Z�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] value ���Z�������f�[�^�̃|�C���^
 */
//============================================================================================
void  PP_Add( POKEMON_PARAM *pp, int id, int value )
{
  if( pp_decordAct( pp ) == TRUE ){
    pp_addAct( pp, id, value );
  }

  pp_encode_act( pp );
}

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂���C�ӂŒl���擾�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[in] pp  �擾����{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id  �擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]  buf �擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return    �擾�����f�[�^
 */
//============================================================================================
u32 PPP_Get( const POKEMON_PASO_PARAM *ppp, int id, void *buf )
{
  u32 ret;

  ppp_decordAct( (POKEMON_PASO_PARAM*)ppp );

  ret = ppp_getAct( (POKEMON_PASO_PARAM*)ppp, id, buf );

  ppp_encode_act( (POKEMON_PASO_PARAM*)ppp );

  return ret;
}

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂ɔC�ӂŒl�����Z�i�Í��^�����^�`�F�b�N�T���`�F�b�N����j
 *
 * @param[in] ppp   ���Z�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id    ���Z�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] value ���Z�������f�[�^�̃|�C���^
 */
//============================================================================================
void  PPP_Add( POKEMON_PASO_PARAM *ppp, int id, int value )
{
  if( ppp_decordAct( ppp ) == TRUE ){
    ppp_addAct( ppp, id, value );
  }

  ppp_encode_act( ppp );
}


//==============================================================================
/**
 * @brief   POKEMON_PARAM���烌�A���ǂ����𔻒肷��
 *
 * @param[in] pp    �|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @retval  FALSE:���A����Ȃ�  TRUE:���A
 */
//==============================================================================
BOOL  PP_CheckRare( const POKEMON_PARAM *pp )
{
  return PPP_CheckRare( &pp->ppp );
}

//==============================================================================
/**
 * @brief   POKEMON_PASO_PARAM���烌�A���ǂ����𔻒肷��
 *
 * @param[in] ppp   �{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @retval  FALSE:���A����Ȃ�  TRUE:���A
 */
//==============================================================================
BOOL  PPP_CheckRare( const POKEMON_PASO_PARAM *ppp )
{
  u32 id  = PPP_Get( ppp, ID_PARA_id_no,      NULL );
  u32 rnd = PPP_Get( ppp, ID_PARA_personal_rnd, NULL );

  return POKETOOL_CheckRare( id, rnd );
}
//==============================================================================
/**
 * @brief   �h�c�ƌ��������烌�A���ǂ����𔻒肷��
 *
 * @param   id    ID
 * @param   rnd   ������
 *
 * @retval  FALSE:���A����Ȃ�  TRUE:���A
 */
//==============================================================================
BOOL  POKETOOL_CheckRare( u32 id, u32 rnd )
{
  return ( ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0xffff ) ^ ( ( rnd & 0xffff0000 ) >> 16 ) ^ ( rnd & 0xffff ) ) < 8 );
}


//============================================================================================
/**
 *  �|�P�����p�����[�^����|�P�����̐��ʂ��擾
 *
 * @param[in] pp    �|�P�����p�����[�^�\����
 *
 * @retval  PTL_SEX_MALE:���@PTL_SEX_FEMALE:���@PTL_SEX_UNK:���ʕs��
 */
//============================================================================================
u8  PP_GetSex( const POKEMON_PARAM *pp )
{
  return ( PPP_GetSex( ( POKEMON_PASO_PARAM * )&pp->ppp ) );
}
//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^����|�P�����̐��ʂ��擾
 *
 * @param[in] pp    �|�P�����p�����[�^�\����
 *
 * @retval  PTL_SEX_MALE:���@PTL_SEX_FEMALE:���@PTL_SEX_UNK:���ʕs��
 */
//============================================================================================
u8  PPP_GetSex( const POKEMON_PASO_PARAM *ppp )
{
  u16 mons_no;
  int form_no;
  u32 rnd;
  int flag;

  flag = PPP_FastModeOn( (POKEMON_PASO_PARAM*)ppp );
  mons_no = PPP_Get( ppp, ID_PARA_monsno, 0 );
  form_no = PPP_Get( ppp, ID_PARA_form_no, 0 );
  rnd = PPP_Get( ppp, ID_PARA_personal_rnd, 0 );
  PPP_FastModeOff( (POKEMON_PASO_PARAM*)ppp, flag );

  return  POKETOOL_GetSex( mons_no, form_no, rnd );
}
//============================================================================================
/**
 *  �|�P�����̐��ʂ��擾�i�����Ńp�[�\�i���f�[�^�����[�h����j
 *
 * @param[in] mons_no ���ʂ��擾����|�P�����i���o�[
 * @param[in] form_no ���ʂ��擾����|�P�����̃t�H�����i���o�[
 * @param[in] rnd   ���ʂ��擾����|�P�����̌�����
 *
 * @retval  PTL_SEX_MALE:���@PTL_SEX_FEMALE:���@PTL_SEX_UNK:���ʕs��
 */
//============================================================================================
u8 POKETOOL_GetSex( u16 mons_no, u16 form_no, u32 personal_rnd )
{
  u8 sex_param;

  POKEMON_PERSONAL_DATA* ppd = Personal_Load( mons_no, form_no );
  sex_param = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );

// ���ʌŒ�̃P�[�X
  switch( sex_param ){
  case POKEPER_SEX_MALE:
    return PTL_SEX_MALE;
  case POKEPER_SEX_FEMALE:
    return PTL_SEX_FEMALE;
  case POKEPER_SEX_UNKNOWN:
    return PTL_SEX_UNKNOWN;
  }

// �������ɂ�萫�ʂ����܂�P�[�X
  {
    u8 rnd_sex_bit = ( personal_rnd & 0xff );

    if( rnd_sex_bit >= sex_param )
    {
      return PTL_SEX_MALE;
    }
    else
    {
      return PTL_SEX_FEMALE;
    }
  }
}

//=============================================================================================
/**
 * �|�P�����̏�Ԉُ���擾
 *
 * @param[in]   pp �|�P�����p�����[�^�\����
 *
 * @retval  PokeSick    ��Ԉُ�ID�i�ُ킪�����ꍇ POKESICK_NULL�j
 */
//=============================================================================================
PokeSick PP_GetSick( const POKEMON_PARAM* pp )
{
  return PP_Get( pp, ID_PARA_condition, NULL );
}
//=============================================================================================
/**
 * �|�P�����̏�Ԉُ��ݒ�
 *
 * @param   pp      �|�P�����p�����[�^�\����
 * @param   sick    ��Ԉُ�ID�i�ُ�𖳂����ꍇ POKESICK_NULL�j
 *
 */
//=============================================================================================
void PP_SetSick( POKEMON_PARAM* pp, PokeSick sick )
{
  PP_Put( pp, ID_PARA_condition, sick );
}



//============================================================================================
/**
 *  �����X�^�[�i���o�[�ƃ��x������f�t�H���g�Z���Z�b�g����B
 *
 * @param[in] pp  �Z�b�g����|�P�����f�[�^�\���̂̃|�C���^
 */
//============================================================================================
void  PP_SetWazaDefault( POKEMON_PARAM *pp )
{
  PPP_SetWazaDefault( &pp->ppp );
}

//============================================================================================
/**
 *  �����X�^�[�i���o�[�ƃ��x������f�t�H���g�Z���Z�b�g����B
 *
 * @param[in] ppp �Z�b�g����|�P�����f�[�^�\���̂̃|�C���^
 */
//============================================================================================
void  PPP_SetWazaDefault( POKEMON_PASO_PARAM *ppp )
{
  static  POKEPER_WAZAOBOE_CODE wot[ POKEPER_WAZAOBOE_TABLE_ELEMS ];  //�Z�o���e�[�u��

  BOOL  flag;
  int i;
  u16 mons_no;
  u16 waza_no;
  int form_no;
  u16 ret;
  u8  level;

  flag = PPP_FastModeOn( ppp );

  mons_no = PPP_Get( ppp, ID_PARA_monsno, 0 );
  form_no = PPP_Get( ppp, ID_PARA_form_no, 0 );
  level = PPP_CalcLevel( ppp );
  POKE_PERSONAL_LoadWazaOboeTable( mons_no, form_no, wot );

// �ŏ��ɑS�N���A���Ă���
  for(i=0; i<PTL_WAZA_MAX; i++)
  {
    // @@@ ���U�i���o�[�����l=0 �Ƃ����O��̋L�q�B������V���{��������B
    PPP_Put( ppp, ID_PARA_waza1 + i,    0 );
    PPP_Put( ppp, ID_PARA_pp1 + i,      0 );
    PPP_Put( ppp, ID_PARA_pp_count1 + i,  0 );
  }

  i = 0;
  while( !POKEPER_WAZAOBOE_IsEndCode(wot[i]) )
  {
    if( POKEPER_WAZAOBOE_GetLevel(wot[i]) <= level )
    {
      waza_no = POKEPER_WAZAOBOE_GetWazaID( wot[i] );
      ret = PPP_SetWaza( ppp, waza_no );
      if( ret == PTL_WAZASET_FAIL ){
        PPP_SetWazaPush( ppp, waza_no );
      }
    }
    else{
      break;
    }
    i++;
  }

  PPP_FastModeOff( ppp, flag );
}

//============================================================================================
/**
 *  �����Ă���ꏊ�ɋZ��ǉ�����B�����Ă��Ȃ��ꍇ�͂��̎|��Ԃ��B
 *
 *  @param[in]  pp    �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 *  @param[in]  wazano  �Z�b�g����Z�i���o�[
 *
 *  @retvl  wazano        ����I��
 *      PTL_WAZASET_SAME  ���łɊo���Ă���Z�Ɠ����Z���o���悤�Ƃ���
 *      PTL_WAZASET_FAIL  �ꏊ���󂢂Ă��Ȃ�
 */
//============================================================================================
u16 PP_SetWaza( POKEMON_PARAM *pp, u16 wazano )
{
  return PPP_SetWaza( &pp->ppp, wazano );
}

//============================================================================================
/**
 *  �����Ă���ꏊ�ɋZ��ǉ�����B�����Ă��Ȃ��ꍇ�͂��̎|��Ԃ��B
 *
 *  @param[in]  ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 *  @param[in]  wazano  �Z�b�g����Z�i���o�[
 *
 *  @retvl  wazano      ����I��
 *      POKETOOL_WAZASET_SAME ���łɊo���Ă���Z�Ɠ����Z���o���悤�Ƃ���
 *      POKETOOL_WAZASET_FAIL   �ꏊ���󂢂Ă��Ȃ�
 */
//============================================================================================
u16 PPP_SetWaza( POKEMON_PASO_PARAM *ppp, u16 wazano )
{
  int i;
  u8  pp;
  u16 waza;
  u16 ret;
  BOOL  flag;

  ret = PTL_WAZASET_FAIL;

  flag = PPP_FastModeOn( ppp );

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ ){
    if( ( waza = PPP_Get( ppp, ID_PARA_waza1 + i, NULL ) ) == 0 ){
      PPP_SetWazaPos( ppp, wazano, i );
      ret = wazano;
      break;
    }
    else{
      //�����Z���o�����Ⴞ��
      if( waza == wazano ){
        ret = PTL_WAZASET_SAME;
        break;
      }
    }
  }

  PPP_FastModeOff( ppp, flag );

  return  ret;
}

//============================================================================================
/**
 *  �����o�����ɋZ���o����B
 *
 * @param[in] pp    �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �o����Z
 */
//============================================================================================
void  PP_SetWazaPush( POKEMON_PARAM *pp, u16 wazano )
{
  PPP_SetWazaPush( &pp->ppp, wazano );
}

//============================================================================================
/**
 *  �����o�����ɋZ���o����B
 *
 * @param[in] ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �o����Z
 */
//============================================================================================
void  PPP_SetWazaPush( POKEMON_PASO_PARAM *ppp, u16 wazano )
{
  u16 waza[PTL_WAZA_MAX];
  u8  pp[PTL_WAZA_MAX];
  u8  ppcnt[PTL_WAZA_MAX];
  BOOL  flag;
  int i;

  u16 LAST_IDX = PTL_WAZA_MAX - 1;

  flag = PPP_FastModeOn( ppp );

  for( i = 0 ; i < LAST_IDX ; i++ ){
    waza[i] = PPP_Get( ppp, ID_PARA_waza2 + i,   NULL );
    pp[i] =   PPP_Get( ppp, ID_PARA_pp2 + i,     NULL );
    ppcnt[i] =  PPP_Get( ppp, ID_PARA_pp_count2 + i, NULL );
  }

  waza[LAST_IDX] = wazano;
  pp[LAST_IDX] = WAZADATA_GetParam( wazano, WAZAPARAM_BASE_PP );
  ppcnt[LAST_IDX] = 0;

  for( i=0 ; i < PTL_WAZA_MAX ; i++ ){
    PPP_Put( ppp, ID_PARA_waza1 + i,   waza[i] );
    PPP_Put( ppp, ID_PARA_pp1 + i,     pp[i] );
    PPP_Put( ppp, ID_PARA_pp_count1 + i, ppcnt[i] );
  }

  PPP_FastModeOff( ppp, flag );
}

//=============================================================================================
/**
 * �o���Ă���S�Ẵ��U��PP���ő�l�܂ŉ񕜂�����
 *
 * @param   pp
 */
//=============================================================================================
void PP_RecoverWazaPPAll( POKEMON_PARAM* pp )
{
  BOOL flag = PP_FastModeOn( pp );
  {
    u32 waza, i;
    for(i=0; i<PTL_WAZA_MAX; ++i)
    {
      waza = PP_Get( pp, ID_PARA_waza1+i, NULL );
      if( waza != WAZANO_NULL )
      {
        u32 upsPP = PP_Get( pp, ID_PARA_pp_count1+i, NULL );
        u32 maxPP = WAZADATA_GetMaxPP( waza, upsPP );
        PP_Put( pp, ID_PARA_pp1+i, maxPP );
      }
    }
  }
  PP_FastModeOff( pp, flag );
}

//============================================================================================
/**
 *  �ꏊ���w�肵�ċZ���Z�b�g����B
 *
 * @param[in] pp    �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �Z�b�g����Z�i���o�[
 * @param[in] pos   �Z���Z�b�g����ꏊ
 */
//============================================================================================
void  PP_SetWazaPos( POKEMON_PARAM *pp, u16 wazano, u8 pos )
{
  PPP_SetWazaPos( &pp->ppp, wazano, pos );
}

//============================================================================================
/**
 *  �ꏊ���w�肵�ċZ���Z�b�g����B
 *
 * @param[in] ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �Z�b�g����Z�i���o�[
 * @param[in] pos   �Z���Z�b�g����ꏊ
 */
//============================================================================================
void  PPP_SetWazaPos( POKEMON_PASO_PARAM *ppp, u16 wazano, u8 pos )
{
  GF_ASSERT(pos<PTL_WAZA_MAX);
  {
    u8 maxPP = 0;

    PPP_Put( ppp, ID_PARA_waza1 + pos, wazano );
    PPP_Put( ppp, ID_PARA_pp_count1 + pos, 0 );

    if( wazano != WAZANO_NULL ){
      maxPP = WAZADATA_GetMaxPP( wazano, 0 );
    }

    PPP_Put( ppp, ID_PARA_pp1 + pos, maxPP );
  }
}

//============================================================================================
/**
 *  �ꏊ���w�肵�ċZ���Z�b�g����B  PP���p��
 *
 * @param[in] ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �Z�b�g����Z�i���o�[
 */
//============================================================================================
u16  PP_SetWazaPPCont( POKEMON_PARAM *pp, u16 wazano )
{ 
  return PPP_SetWazaPPCont( &pp->ppp, wazano );
}

//============================================================================================
/**
 *  �����Ă���ꏊ�ɋZ��ǉ�����B�����Ă��Ȃ��ꍇ�͂��̎|��Ԃ��B  PP���p��
 *
 *  @param[in]  ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 *  @param[in]  wazano  �Z�b�g����Z�i���o�[
 *
 *  @retvl  wazano      ����I��
 *      POKETOOL_WAZASET_SAME ���łɊo���Ă���Z�Ɠ����Z���o���悤�Ƃ���
 *      POKETOOL_WAZASET_FAIL   �ꏊ���󂢂Ă��Ȃ�
 */
//============================================================================================
u16  PPP_SetWazaPPCont( POKEMON_PASO_PARAM *ppp, u16 wazano )
{ 
  int i;
  u8  pp;
  u16 waza;
  u16 ret;
  BOOL  flag;

  ret = PTL_WAZASET_FAIL;

  flag = PPP_FastModeOn( ppp );

  for( i = 0 ; i < PTL_WAZA_MAX ; i++ ){
    if( ( waza = PPP_Get( ppp, ID_PARA_waza1 + i, NULL ) ) == 0 ){
      PPP_SetWazaPosPPCont( ppp, wazano, i );
      ret = wazano;
      break;
    }
    else{
      //�����Z���o�����Ⴞ��
      if( waza == wazano ){
        ret = PTL_WAZASET_SAME;
        break;
      }
    }
  }

  PPP_FastModeOff( ppp, flag );

  return  ret;
}
//============================================================================================
/**
 *  �ꏊ���w�肵�ċZ���Z�b�g����B  PP���p��
 *
 * @param[in] pp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �Z�b�g����Z�i���o�[
 * @param[in] pos   �Z���Z�b�g����ꏊ
 */
//============================================================================================
void  PP_SetWazaPosPPCont( POKEMON_PARAM *pp, u16 wazano, u8 pos )
{ 
  PPP_SetWazaPosPPCont( &pp->ppp, wazano, pos );
}

//============================================================================================
/**
 *  �ꏊ���w�肵�ċZ���Z�b�g����B  PP���p��
 *
 * @param[in] ppp   �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] wazano  �Z�b�g����Z�i���o�[
 * @param[in] pos   �Z���Z�b�g����ꏊ
 */
//============================================================================================
void  PPP_SetWazaPosPPCont( POKEMON_PASO_PARAM *ppp, u16 wazano, u8 pos )
{ 
  //����A�킴�}�V����������g����̂ŁA
  //�킴�}�V�����g�������邱�ƂŁAPP�̉񕜂��ł��Ă��܂�
  //���̂��߈ȑO��PP�������p���@2010/3/23 nagihashi
  GF_ASSERT(pos<PTL_WAZA_MAX);
  {
    u8 srcPP  = 0;
    u8 dstPP  = 0;
    BOOL is_exits;

    //�ȑO�̋Z���擾
    is_exits  = PPP_Get( ppp, ID_PARA_waza1 + pos, NULL ) != WAZANO_NULL;
    srcPP     = PPP_Get( ppp, ID_PARA_pp1 + pos, NULL );

    //�V�K�̋Z�ݒ�
    PPP_Put( ppp, ID_PARA_waza1 + pos, wazano );
    PPP_Put( ppp, ID_PARA_pp_count1 + pos, 0 );

    //PP�Đݒ菈��
    if( wazano != WAZANO_NULL ){
      dstPP = WAZADATA_GetMaxPP( wazano, 0 );

      //�Z���㏑�����Ă����ꍇ�APP�������p��
      if( is_exits )
      { 
        dstPP = MATH_CLAMP( dstPP, 0, srcPP );
      }
    }

    PPP_Put( ppp, ID_PARA_pp1 + pos, dstPP );
  }
}

//============================================================================================
/**
 *  ���̃��x���Ŋo����Z���Z�b�g����i���x���A�b�v���ɌĂ΂�邱�Ƒz��j
 *
 * @param[in]     pp      �Z�b�g����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in/out] index   �Z�o���e�[�u���Q�ƃC���f�b�N�X
 * @param[in]     heapID  �Z�o���e�[�u���m�ۗp�q�[�v�̃q�[�vID
 *
 * @retval  �Z�b�g�����Z�i���o�[�i PTL_WAZAOBOE_NONE:�o���Ȃ����� PTL_WAZAOBOE_FULL�r�b�g�������Ă���:�莝���Z�������ς��j
 */
//============================================================================================
WazaID  PP_CheckWazaOboe( POKEMON_PARAM *pp, int* index, HEAPID heapID )
{
  POKEPER_WAZAOBOE_CODE*  wot = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID( heapID ),
                                                      POKEPER_WAZAOBOE_TABLE_ELEMS * sizeof( POKEPER_WAZAOBOE_CODE ) );
  WazaID  ret = PTL_WAZAOBOE_NONE;
  u16     mons_no;
  int     form_no;
  u8      level;

  mons_no = PP_Get( pp, ID_PARA_monsno, NULL );
  form_no = PP_Get( pp, ID_PARA_form_no, NULL );
  level = PP_Get( pp, ID_PARA_level, NULL );
  POKE_PERSONAL_LoadWazaOboeTable( mons_no, form_no, wot );

  while( !POKEPER_WAZAOBOE_IsEndCode( wot[ index[ 0 ] ] ) ){
    if( POKEPER_WAZAOBOE_GetLevel( wot[ index[ 0 ] ] ) == level )
    {
      ret = PP_SetWaza( pp, POKEPER_WAZAOBOE_GetWazaID( wot[ index[ 0 ] ] ) );
      if( ret == PTL_WAZASET_FAIL )
      {
        ret = POKEPER_WAZAOBOE_GetWazaID( wot[ index[ 0 ] ] ) | PTL_WAZAOBOE_FULL;
        index[ 0 ]++;
        break;
      }
      else if( ret != PTL_WAZASET_SAME )
      {
        index[ 0 ]++;
        break;
      }
    }
    index[ 0 ]++;
  }

  GFL_HEAP_FreeMemory( wot );

  return  ret;
}

//============================================================================================
/**
 *  �|�P�����f�[�^����|�P�����̃��x�����擾
 *
 * @param[in] pp  �擾�������|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return  �擾�������x��
 */
//============================================================================================
u32 PP_CalcLevel( const POKEMON_PARAM *pp )
{
  return PPP_CalcLevel( &pp->ppp );
}
//============================================================================================
/**
 *  �{�b�N�X�|�P�����f�[�^����|�P�����̃��x�����擾
 *
 * @param[in] ppp �擾�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return  �擾�������x��
 */
//============================================================================================
u32 PPP_CalcLevel( const POKEMON_PASO_PARAM *ppp )
{
  int mons_no;
  int form_no;
  u32 exp;
  BOOL  flag;

  flag = PPP_FastModeOn( (POKEMON_PASO_PARAM*)ppp );

  mons_no = PPP_Get( ppp, ID_PARA_monsno, 0 );
  form_no = PPP_Get( ppp, ID_PARA_form_no, 0 );
  exp = PPP_Get( ppp, ID_PARA_exp, 0 );

  PPP_FastModeOff( (POKEMON_PASO_PARAM*)ppp, flag );

  return POKETOOL_CalcLevel( mons_no, form_no, exp );
}

//=============================================================================================
/**
 * �|�P�����f�[�^�ƃv���C���[�f�[�^�̐e��v�`�F�b�N
 *
 * @param   pp        �|�P�����f�[�^
 * @param   player    �v���C���[�f�[�^
 *
 * @retval  BOOL      �v���C���[�f�[�^���|�P�����̐e�Ȃ�TRUE
 */
//=============================================================================================
BOOL PP_IsMatchOya( const POKEMON_PARAM* pp, const MYSTATUS* player )
{
  return PPP_IsMatchOya( &pp->ppp, player );
}
//=============================================================================================
/**
 * �{�b�N�X�|�P�����f�[�^�ƃv���C���[�f�[�^�̐e��v�`�F�b�N
 *
 * @param   ppp       �{�b�N�X�|�P�����f�[�^
 * @param   player    �v���C���[�f�[�^
 *
 * @retval  BOOL      �v���C���[�f�[�^���|�P�����̐e�Ȃ�TRUE
 */
//=============================================================================================
BOOL PPP_IsMatchOya( const POKEMON_PASO_PARAM* ppp, const MYSTATUS* player )
{
  u8 fastFlag = PPP_FastModeOn( (POKEMON_PASO_PARAM*)ppp );
  BOOL result = FALSE;

  {
    u32 playerID = MyStatus_GetID( player );
    u32 oyaID = PPP_Get( ppp, ID_PARA_id_no, NULL );

    u32 playerSex = MyStatus_GetMySex( player );
    u32 oyaSex = PPP_Get( ppp, ID_PARA_oyasex, NULL );

    MyStatus_CopyNameStrCode( player, StrBuffer, NELEMS(StrBuffer) );
    PPP_Get( ppp, ID_PARA_oyaname_raw, StrBufferSub );

    if( (playerID == oyaID)
    &&  (playerSex == oyaSex)
    &&  (STRTOOL_Comp(StrBuffer, StrBufferSub))
    ){
      result = TRUE;
    }
  }

  PPP_FastModeOff( (POKEMON_PASO_PARAM*)ppp, fastFlag );

  return result;
}


//============================================================================================
/**
 *  �|�P�����i���o�[�A�o���l����|�P�����̃��x�����v�Z
 *  �i�����Ńp�[�\�i���f�[�^�����[�h����j
 *
 * @param[in] mons_no �擾�������|�P�����i���o�[
 * @param[in] form_no �擾�������|�P�����̃t�H�����i���o�[
 * @param[in] exp   �擾�������|�P�����̌o���l
 *
 * @return  �擾�������x��
 */
//============================================================================================
u32 POKETOOL_CalcLevel( u16 mons_no, u16 form_no, u32 exp )
{
  u16 growType, level;

  POKEMON_PERSONAL_DATA* ppd = Personal_Load( mons_no, form_no );
  growType = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_grow );
  load_grow_table( growType, &GrowTable[0] );

  for( level = 1 ; level < 101 ; level++ ){
    if( GrowTable[ level ] > exp ) break;
  }

  return level - 1;
}

//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂���o���l�f�[�^���擾
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂̃|�C���^
 */
//============================================================================================
u32 PP_GetMinExp( const POKEMON_PARAM *pp )
{
  return PPP_GetMinExp( &pp->ppp );
}

//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂���o���l�f�[�^���擾
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂̃|�C���^
 */
//============================================================================================
u32 PPP_GetMinExp( const POKEMON_PASO_PARAM *ppp )
{
  return POKETOOL_GetMinExp(
              PPP_Get( ppp, ID_PARA_monsno, NULL ),
              PPP_Get( ppp, ID_PARA_form_no, NULL ),
              PPP_Get( ppp, ID_PARA_level, NULL )
  );
}

//============================================================================================
/**
 *  �|�P�����i���o�[�ƃ��x������A���̃��x���ɂȂ邽�߂̍ŏ��o���l���擾
 *
 * @param[in] mons_no   �擾���郂���X�^�[�i���o�[
 * @param[in] form_no   �擾���郂���X�^�[�̃t�H�����i���o�[
 * @param[in] level   ���x���w��
 *
 * @retval  u32   �w�背�x���ɒB���邽�߂̍ŏ��o���l
 */
//============================================================================================
u32 POKETOOL_GetMinExp( u16 mons_no, u16 form_no, u16 level )
{
  return  get_growtbl_param( POKETOOL_GetPersonalParam(mons_no, form_no, POKEPER_ID_grow), level );
}



//============================================================================================
/**
 *  �|�P�����̐��i���擾�i������POKEMON_PARAM)
 *
 * @param[in] pp  �擾�������|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return  �擾�������i
 */
//============================================================================================
u8  PP_GetSeikaku( const POKEMON_PARAM *pp )
{
  return  PPP_GetSeikaku( &pp->ppp );
}
//============================================================================================
/**
 *  �|�P�����̐��i���擾�i������POKEMON_PASO_PARAM)
 *
 * @param[in] ppp �擾�������|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return  �擾�������i
 */
//============================================================================================
u8  PPP_GetSeikaku( const POKEMON_PASO_PARAM *ppp )
{
  return PPP_Get( ppp, ID_PARA_seikaku, 0 );
}

//=============================================================================================
/**
 * �����PPP�f�[�^����ɁAPP�f�[�^�����X�V����
 *
 * @param   pp    �X�V����PP�f�[�^�|�C���^
 */
//=============================================================================================
void  PP_Renew( POKEMON_PARAM *pp )
{
  int   level;
  BOOL  flag;

  flag = PP_FastModeOn( pp );

//���x���Z�b�g
  level = PP_CalcLevel( pp );
  PP_Put( pp, ID_PARA_level, level );

  calc_renew_core( pp );

  PP_FastModeOff( pp, flag );
}

//=============================================================================================
/**
 * POKEMON_PASO_PARAM�̃|�C���^���擾
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂̃|�C���^
 */
//=============================================================================================
POKEMON_PASO_PARAM  *PP_GetPPPPointer( POKEMON_PARAM *pp )
{
  return &pp->ppp;
}

//=============================================================================================
/**
 * POKEMON_PASO_PARAM�̃|�C���^���擾
 *
 * @param[in] pp  �|�P�����p�����[�^�\���̂̃|�C���^
 */
//=============================================================================================
const POKEMON_PASO_PARAM  *PP_GetPPPPointerConst( const POKEMON_PARAM *pp )
{
  return &pp->ppp;
}

//============================================================================================
/**
 *  �w�肳�ꂽ�p�����[�^�ɂȂ�悤�Ɍ��������v�Z����
 *
 * @param[in] mons_no     �����X�^�[�i���o�[
 * @param[in] form_no     �t�H�����[�i���o�[�i�s�v�Ȃ� PTR_FORM_NONE�j
 * @param[in] sex         ����( PTL_SEX_MALE or PTL_SEX_FEMALE or PTL_SEX_UNKNOWN )
 *
 * @retval  �v�Z����������
 */
//============================================================================================
u32  POKETOOL_CalcPersonalRand( u16 mons_no, u16 form_no, u8 sex )
{
  u32 rnd = 8;  // ID == 0 �̎��Ƀ��A�ɂȂ�Ȃ��悤�����l��8�ɂ��Ă���

  // ���� : ���� 1byte �𒲐�
  {
    POKEMON_PERSONAL_DATA* ppd = Personal_Load( mons_no, form_no );
    u8 sex_param = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );
    if( PokePersonal_SexVecTypeGet( sex_param ) != POKEPER_SEXTYPE_FIX)
    {
      if( sex == PTL_SEX_MALE ){
        rnd = sex_param;
      }else{
        if( sex_param ){
          rnd = sex_param - 1;
        }else{
          rnd = 0;  // ffffffff�ɂȂ��Ă��܂�Ȃ��悤�O�̂���
        }
      }
    }
  }

  return rnd;
}

//============================================================================================
/**
 *  �w�肳�ꂽ�p�����[�^�ɂȂ�悤�Ɍ��������v�Z����i�����A���A�w��\�Łj
 *
 * @param[in] id          �g���[�i�[ID
 * @param[in] mons_no     ���������v�Z���郂���X�^�[�i���o�[
 * @param[in] form_no     �t�H�����[�i���o�[�i�s�v�Ȃ� PTR_FORM_NONE�j
 * @param[in] sex         ����
 * @param[in] tokusei     �����i 0 or 1 �Ŏw�� �j�i����ɓ����Por�����Q�ɂȂ邩���w�肷�邽�߂̂��̂ł�
 * @param[in] rare_flag   ���A�ɂ��邩�ǂ���( FALSE:���A�ł͂Ȃ��@TRUE:���A�ɂ��� �j
 *
 * @retval  �v�Z����������
 */
//============================================================================================
u32   POKETOOL_CalcPersonalRandEx( u32 id, u16 mons_no, u16 form_no, u8 sex, u8 tokusei, BOOL rare_flag )
{
  u32 rnd;

  //�����i���o�[�ł͂Ȃ��̂ŁA2�ȏ�̓A�T�[�g�ɂ���
  GF_ASSERT( tokusei < 2 );

  if( rare_flag )
  {
    u32 mask = ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0x0000ffff ) );

    rnd = POKETOOL_CalcPersonalRand( mons_no, form_no, sex );
    if( ( rnd & 0x00000001 ) != tokusei )
    {
      if( sex == PTL_SEX_MALE )
      {
        rnd++;
      }
      else
      {
        //�������O�̂Ƃ��́A�v�����悤�Ȓl�ɂȂ�Ȃ���������߂Ă��炤�����Ȃ�
        if( rnd )
        {
          rnd--;
        }
      }
    }
    rnd |= ( mask ^ ( rnd & 0x0000ffff ) ) << 16;
  }
  else
  {
    POKEMON_PERSONAL_DATA* ppd = Personal_Load( mons_no, form_no );
    u8 per_sex = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );

    rnd = ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0x0000ffff ) ) & 0xff00;
    rnd = ( rnd ^ 0xff00 ) << 16;

    if( PokePersonal_SexVecTypeGet( per_sex ) != POKEPER_SEXTYPE_FIX )
    {
      rnd |= per_sex;
      if( sex == PTL_SEX_MALE )
      {
        if( ( rnd & 1 ) != tokusei )
        {
          rnd++;
        }
      }
      else
      {
        rnd--;
        if( ( rnd & 1 ) != tokusei )
        {
          rnd--;
        }
      }
    }
  }
  return rnd;
}

//============================================================================================
/**
 *  �w�肳�ꂽ�p�����[�^�ɂȂ�悤�Ɍ��������v�Z����i���ʁA�����A���A�w�肨��т��ꂼ��̕s����w��\�Łj
 *
 * @param[in] id          �g���[�i�[ID
 * @param[in] mons_no     ���������v�Z���郂���X�^�[�i���o�[
 * @param[in] form_no     �t�H�����[�i���o�[�i�s�v�Ȃ� PTR_FORM_NONE�j
 * @param[in] sex         ����(�I�X���X�̂���|�P������PTL_SEX_UNKNOWN���w�肷��ƕs��ɂȂ�j
 * @param[in] tokusei     �����i 0 or 1 �Ŏw�� �j�i����ɓ����Por�����Q�ɂȂ邩���w�肷�邽�߂̂��̂ł�
 * @param[in] rare_flag   ���A�ɂ��邩�ǂ���( FALSE:���A�ł͂Ȃ��@TRUE:���A�ɂ��� �j
 *
 * @retval  �v�Z����������
 */
//============================================================================================
u32   POKETOOL_CalcPersonalRandSpec( u32 id, u16 mons_no, u16 form_no, PtlSexSpec sex, PtlTokuseiSpec tokusei, PtlRareSpec rare_flag )
{
  u32 rnd;

  //�����i���o�[�ł͂Ȃ��̂ŁA2�ȏ�̓A�T�[�g�ɂ���
  GF_ASSERT( sex        < PTL_SEX_SPEC_MAX );
  GF_ASSERT( tokusei    < PTL_TOKUSEI_SPEC_MAX );
  GF_ASSERT( rare_flag  < PTL_RARE_SPEC_MAX );

  if( sex == PTL_SEX_SPEC_UNKNOWN )
  { 
    rnd = GFUser_GetPublicRand(GFL_STD_RAND_MAX);
  }
  else
  { 
    rnd = POKETOOL_CalcPersonalRand( mons_no, form_no, sex );
  }

  switch( rare_flag ){ 
  case PTL_RARE_SPEC_TRUE:
    { 
      u32 rare_mask = ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0x0000ffff ) );
      rnd &= 0x000000ff;
      if( tokusei != PTL_TOKUSEI_SPEC_BOTH )
      { 
        if( ( rnd & 0x00000001 ) != tokusei )
        {
          if( sex == PTL_SEX_FEMALE )
          {
            //�������O�̂Ƃ��́A�v�����悤�Ȓl�ɂȂ�Ȃ���������߂Ă��炤�����Ȃ�
            if( rnd )
            {
              rnd--;
            }
          }
          else
          {
            rnd++;
          }
        }
      }
      else
      { 
        if( sex == PTL_SEX_FEMALE )
        {
          //�������O�̂Ƃ��́A�v�����悤�Ȓl�ɂȂ�Ȃ���������߂Ă��炤�����Ȃ�
          if( rnd )
          {
            rnd--;
            if( ( GFUser_GetPublicRand( 2 ) ) && ( rnd ) )
            { 
              rnd--;
            }
          }
        }
        else
        {
          rnd++;
          if( GFUser_GetPublicRand( 2 ) )
          { 
            rnd++;
          }
        }
      }
      rnd |= ( rare_mask ^ ( rnd & 0x0000ffff ) ) << 16;
    }
    break;
  case PTL_RARE_SPEC_FALSE:
    { 
      u32 rare_false_rnd = ( ( ( id & 0xffff0000 ) >> 16 ) ^ ( id & 0x0000ffff ) ) & 0xff00;
      rare_false_rnd = ( rare_false_rnd ^ 0xff00 ) << 16;
      rnd &= 0x000000ff;
      rnd = rare_false_rnd | rnd;
    }
    /* fallthru */
  case PTL_RARE_SPEC_BOTH:
    if( tokusei != PTL_TOKUSEI_SPEC_BOTH )
    { 
      if( ( rnd & 0x00000001 ) != tokusei )
      {
        if( sex == PTL_SEX_FEMALE )
        {
          //�������O�̂Ƃ��́A�v�����悤�Ȓl�ɂȂ�Ȃ���������߂Ă��炤�����Ȃ�
          if( rnd )
          {
            rnd--;
          }
        }
        else
        {
          rnd++;
        }
      }
    }
    else
    { 
      if( sex == PTL_SEX_FEMALE )
      {
        //�������O�̂Ƃ��́A�v�����悤�Ȓl�ɂȂ�Ȃ���������߂Ă��炤�����Ȃ�
        if( rnd )
        {
          rnd--;
          if( ( GFUser_GetPublicRand( 2 ) ) && ( rnd ) )
          { 
            rnd--;
          }
        }
      }
      else
      {
        rnd++;
        if( GFUser_GetPublicRand( 2 ) )
        { 
          rnd++;
        }
      }
    }
    break;
  }

  return rnd;
}

//=============================================================================================
/**
 * ���U�u�߂��߂�p���[�v�̎��s���^�C�v�擾
 *
 * @param   pp
 *
 * @retval  PokeType
 */
//=============================================================================================
PokeType POKETOOL_GetMezaPa_Type( const POKEMON_PARAM* pp )
{
  static const u8 typeTbl[] = {
    POKETYPE_KAKUTOU, POKETYPE_HIKOU, POKETYPE_DOKU,   POKETYPE_JIMEN,
    POKETYPE_IWA,     POKETYPE_MUSHI, POKETYPE_GHOST,  POKETYPE_HAGANE,
    POKETYPE_HONOO,   POKETYPE_MIZU,  POKETYPE_KUSA,   POKETYPE_DENKI,
    POKETYPE_ESPER,   POKETYPE_KOORI, POKETYPE_DRAGON, POKETYPE_AKU,
  };

  u32 val=0;
  if( PP_Get(pp, ID_PARA_hp_rnd, NULL) & 1){ val += 1; }
  if( PP_Get(pp, ID_PARA_pow_rnd, NULL) & 1){ val += 2; }
  if( PP_Get(pp, ID_PARA_def_rnd, NULL) & 1){ val += 4; }
  if( PP_Get(pp, ID_PARA_agi_rnd, NULL) & 1){ val += 8; }
  if( PP_Get(pp, ID_PARA_spepow_rnd, NULL) & 1){ val += 16; }
  if( PP_Get(pp, ID_PARA_spedef_rnd, NULL) & 1){ val += 32; }

  val = val * 15 / 63;
  while( val > NELEMS(typeTbl) ){ // �e�[�u���T�C�Y���z���邱�Ƃ͗L�蓾�Ȃ��n�Y�����O�̂���
    val -= NELEMS(typeTbl);
  }

  return typeTbl[ val ];
}
//=============================================================================================
/**
 * ���U�u�߂��߂�p���[�v�̎��s���З͂��擾
 *
 * @param   pp
 *
 * @retval  PokeType
 */
//=============================================================================================
u32 POKETOOL_GetMezaPa_Power( const POKEMON_PARAM* pp )
{
  u32 pow=0;
  if( (PP_Get(pp, ID_PARA_hp_rnd, NULL) % 4)    > 1){ pow += 1; }
  if( (PP_Get(pp, ID_PARA_pow_rnd, NULL) % 4)   > 1){ pow += 2; }
  if( (PP_Get(pp, ID_PARA_def_rnd, NULL) % 4)   > 1){ pow += 4; }
  if( (PP_Get(pp, ID_PARA_agi_rnd, NULL) % 4)   > 1){ pow += 8; }
  if( (PP_Get(pp, ID_PARA_spepow_rnd, NULL) % 4)> 1){ pow += 16; }
  if( (PP_Get(pp, ID_PARA_spedef_rnd, NULL) % 4)> 1){ pow += 32; }

  pow = 30 + (pow * 40 / 63);
  return pow;
}

//=============================================================================================
/**
 * �Z�}�V���ŋZ���o���邩�`�F�b�N
 *
 * @param[in] pp          �|�P�����p�����[�^�\����
 * @param[in] machine_no  �`�F�b�N����Z�}�V���i���o�[
 *
 * @retval  FALSE:�o���Ȃ��@TRUE:�o����
 */
//=============================================================================================
BOOL  PP_CheckWazaMachine( const POKEMON_PARAM *pp, int machine_no )
{
  return PPP_CheckWazaMachine( &pp->ppp, machine_no );
}

//=============================================================================================
/**
 * �Z�}�V���ŋZ���o���邩�`�F�b�N
 *
 * @param[in] ppp
 * @param[in] machine_no  �`�F�b�N����Z�}�V���i���o�[
 *
 * @retval  FALSE:�o���Ȃ��@TRUE:�o����
 */
//=============================================================================================
BOOL  PPP_CheckWazaMachine( const POKEMON_PASO_PARAM *ppp, int machine_no )
{
  u16 mons_no = PPP_Get( ppp, ID_PARA_monsno, NULL );
  u16 form_no = PPP_Get( ppp, ID_PARA_form_no, NULL );

  return POKETOOL_CheckWazaMachine( mons_no, form_no, machine_no );
}

//=============================================================================================
/**
 * �Z�}�V���ŋZ���o���邩�`�F�b�N
 *
 * @param[in] mons_no     �`�F�b�N����|�P�����i���o�[
 * @param[in] form_no     �`�F�b�N����|�P�����̃t�H�����i���o�[
 * @param[in] machine_no  �`�F�b�N����Z�}�V���i���o�[
 *
 * @retval  FALSE:�o���Ȃ��@TRUE:�o����
 */
//=============================================================================================
BOOL  POKETOOL_CheckWazaMachine( u16 mons_no, u16 form_no, int machine_no )
{
  PokePersonalParamID perID;
  u32 machine_bit;

  if( mons_no == MONSNO_TAMAGO )
  {
    return FALSE;
  }

  machine_bit = ( 1 << ( machine_no % 32 ) );
  perID = POKEPER_ID_machine1 + ( machine_no / 32 );

  return ( ( POKETOOL_GetPersonalParam( mons_no, form_no, perID ) & machine_bit ) != 0 );
}

//=============================================================================================
/**
 * �Z�����ŋZ���o���邩�`�F�b�N
 *
 * @param[in] pp          �|�P�����p�����[�^�\����
 * @param[in] waza_oshie_no  �`�F�b�N����킴�����w��i���o�[
 *
 * @retval  FALSE:�o���Ȃ��@TRUE:�o����
 */
//=============================================================================================
BOOL PP_CheckWazaOshie( const POKEMON_PARAM * pp, int waza_oshie_no )
{
  u16 mons_no = PP_Get( pp, ID_PARA_monsno, NULL );
  u16 form_no = PP_Get( pp, ID_PARA_form_no, NULL );

  return POKETOOL_CheckWazaOshie( mons_no, form_no, waza_oshie_no );
}

//=============================================================================================
/**
 * �Z�����ŋZ���o���邩�`�F�b�N
 *
 * @param[in] mons_no        �`�F�b�N����|�P�����i���o�[
 * @param[in] form_no        �`�F�b�N����|�P�����̃t�H�����i���o�[
 * @param[in] waza_oshie_no  �`�F�b�N����킴�����w��i���o�[
 *
 * @retval  FALSE:�o���Ȃ��@TRUE:�o����
 */
//=============================================================================================
BOOL  POKETOOL_CheckWazaOshie( u16 mons_no, u16 form_no, int waza_oshie_no )
{
  PokePersonalParamID perID;
  u32 waza_oshie_bit;

  if( mons_no == MONSNO_TAMAGO )
  {
    return FALSE;
  }

  waza_oshie_bit = ( 1 << ( waza_oshie_no % 32 ) );
  perID = POKEPER_ID_waza_oshie1;

  return ( ( POKETOOL_GetPersonalParam( mons_no, form_no, perID ) & waza_oshie_bit ) != 0 );
}
//=============================================================================================
/**
 * �|�P�����̍D�݂̖����`�F�b�N
 *
 * @param[in] pp     �|�P�����p�����[�^�\����
 * @param[in] taste  �`�F�b�N���閡
 *
 * @retval  PTL_TASTE_LIKE:�D���@PTL_TASTE_DISLIKE:�����@PTL_TASTE_NORMAL:����
 */
//=============================================================================================
PtlTasteJudge PP_CheckDesiredTaste( const POKEMON_PARAM *pp, PtlTaste taste )
{
  return PPP_CheckDesiredTaste( &pp->ppp, taste );
}

//=============================================================================================
/**
 * �|�P�����̍D�݂̖����`�F�b�N
 *
 * @param[in] ppp    �|�P�����p�����[�^�\����
 * @param[in] taste  �`�F�b�N���閡
 *
 * @retval  PTL_TASTE_LIKE:�D���@PTL_TASTE_DISLIKE:�����@PTL_TASTE_NORMAL:����
 */
//=============================================================================================
PtlTasteJudge PPP_CheckDesiredTaste( const POKEMON_PASO_PARAM *ppp, PtlTaste taste )
{
  u8 seikaku = PPP_Get( ppp, ID_PARA_seikaku, NULL );

  return POKETOOL_CheckDesiredTaste( seikaku, taste );
}

//=============================================================================================
/**
 * �|�P�����̍D�݂̖����`�F�b�N
 *
 * @param[in] seikaku ���i
 * @param[in] taste   �`�F�b�N���閡
 *
 * @retval  PTL_TASTE_LIKE:�D���@PTL_TASTE_DISLIKE:�����@PTL_TASTE_NORMAL:����
 */
//=============================================================================================
#include "taste.cdat"
PtlTasteJudge POKETOOL_CheckDesiredTaste( u8 seikaku, PtlTaste taste )
{
  return desired_taste_tbl[ seikaku ][ taste ];
}

//=============================================================================================
/**
 * �|�P�����ɑ�3����������i��3�����t���O�����Ă邽�ߐ�p�֐��ɂ���j
 *
 * @param[in] pp      �|�P�����p�����[�^�\����
 * @param[in] mons_no ��3���������郂���X�^�[�i���o�[
 * @param[in] form_no ��3����������t�H�����i���o�[
 */
//=============================================================================================
void  PP_SetTokusei3( POKEMON_PARAM* pp, int mons_no, int form_no )
{ 
  PPP_SetTokusei3( &pp->ppp, mons_no, form_no );
}

//=============================================================================================
/**
 * �|�P�����ɑ�3����������i��3�����t���O�����Ă邽�ߐ�p�֐��ɂ���j
 *
 * @param[in] ppp �|�P�����p�����[�^�\����
 * @param[in] mons_no ��3���������郂���X�^�[�i���o�[
 * @param[in] form_no ��3����������t�H�����i���o�[
 */
//=============================================================================================
void  PPP_SetTokusei3( POKEMON_PASO_PARAM* ppp, int mons_no, int form_no )
{ 
  u32 tokusei = POKETOOL_GetPersonalParam( mons_no, form_no, POKEPER_ID_speabi3 );
  if(tokusei){  //@todo ��check �]�䕔����ɗv�m�F
    PPP_Put( ppp, ID_PARA_speabino, tokusei );
    PPP_Put( ppp, ID_PARA_tokusei_3_flag, 1 );
  }
}

//--------------------------------------------------------------------------
/**
 * PPP�f�[�^������PP�Ǝ��f�[�^�����Čv�Z
 *
 * @param[io]   pp
 *
 */
//--------------------------------------------------------------------------
static void  calc_renew_core( POKEMON_PARAM *pp )
{
  int oldhpmax, hp, hpmax;
  int pow, def, agi, spepow, spedef;
  int hp_rnd, pow_rnd, def_rnd, agi_rnd, spepow_rnd, spedef_rnd;
  int hp_exp, pow_exp, def_exp, agi_exp, spepow_exp, spedef_exp;
  int monsno;
  int level;
  int form_no;
  int speabi1,speabi2,rnd;
  POKEMON_PERSONAL_DATA* ppd;

  level =     PP_Get( pp, ID_PARA_level,      0);
  oldhpmax =    PP_Get( pp, ID_PARA_hpmax,      0);
  hp =      PP_Get( pp, ID_PARA_hp,     0);
  hp_rnd =    PP_Get( pp, ID_PARA_hp_rnd,   0);
  hp_exp =    PP_Get( pp, ID_PARA_hp_exp,   0);
  pow_rnd =   PP_Get( pp, ID_PARA_pow_rnd,    0);
  pow_exp =   PP_Get( pp, ID_PARA_pow_exp,    0);
  def_rnd =   PP_Get( pp, ID_PARA_def_rnd,    0);
  def_exp =   PP_Get( pp, ID_PARA_def_exp,    0);
  agi_rnd =   PP_Get( pp, ID_PARA_agi_rnd,    0);
  agi_exp =   PP_Get( pp, ID_PARA_agi_exp,    0);
  spepow_rnd =  PP_Get( pp, ID_PARA_spepow_rnd, 0);
  spepow_exp =  PP_Get( pp, ID_PARA_spepow_exp, 0);
  spedef_rnd =  PP_Get( pp, ID_PARA_spedef_rnd, 0);
  spedef_exp =  PP_Get( pp, ID_PARA_spedef_exp, 0);
  form_no =   PP_Get( pp, ID_PARA_form_no,    0);

  monsno = PP_Get( pp, ID_PARA_monsno, 0 );

  ppd = Personal_Load( monsno, form_no );

  if( monsno == MONSNO_NUKENIN ){
    hpmax = 1;
  }
  else{
    hpmax = ( ( 2 * ppd->basic_hp + hp_rnd + hp_exp / 4 ) * level / 100 + level + 10 );
  }

  PP_Put( pp, ID_PARA_hpmax, hpmax );

  pow = ( ( 2 * ppd->basic_pow + pow_rnd + pow_exp / 4 ) * level / 100 + 5 );
  pow = calc_abi_seikaku( PP_GetSeikaku( pp ), pow, PTL_ABILITY_ATK );
  PP_Put( pp, ID_PARA_pow, pow );

  def = ( ( 2 * ppd->basic_def + def_rnd + def_exp / 4 ) * level / 100 + 5 );
  def = calc_abi_seikaku( PP_GetSeikaku( pp ), def, PTL_ABILITY_DEF );
  PP_Put( pp, ID_PARA_def, def );

  agi = ( ( 2 * ppd->basic_agi + agi_rnd + agi_exp / 4 ) * level / 100 + 5 );
  agi = calc_abi_seikaku( PP_GetSeikaku( pp ), agi, PTL_ABILITY_AGI );
  PP_Put( pp, ID_PARA_agi, agi );

  spepow = ( ( 2 * ppd->basic_spepow + spepow_rnd + spepow_exp / 4 ) * level / 100 + 5 );
  spepow = calc_abi_seikaku( PP_GetSeikaku( pp ), spepow, PTL_ABILITY_SPATK );
  PP_Put( pp, ID_PARA_spepow, spepow );

  spedef = ( ( 2 * ppd->basic_spedef + spedef_rnd + spedef_exp / 4 ) * level / 100 + 5 );
  spedef = calc_abi_seikaku( PP_GetSeikaku( pp ), spedef, PTL_ABILITY_SPDEF );
  PP_Put( pp, ID_PARA_spedef, spedef );

  if( ( hp == 0 ) && ( oldhpmax != 0 ) ){
    ;
  }
  else{
    if( monsno == MONSNO_NUKENIN ){
      hp = 1;
    }
    else if( hp == 0 ){
      hp = hpmax;
    }
    else{
      if( ( hpmax - oldhpmax ) < 0 ){
        if( hp > hpmax ){
          hp = hpmax;
        }
      }
      else{
        hp += ( hpmax - oldhpmax );
      }
    }
  }

  if( hp ){
    PP_Put( pp, ID_PARA_hp, hp );
  }
}



//--------------------------------------------------------------------------
/**
 *  �|�P�����p�����[�^�\���̂𕜍����ă`�F�b�N�T�����`�F�b�N���ĕs���Ȃ�_���^�}�S�ɂ���
 *
 * @param[in] pp    ��������|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return    FALSE:�s���f�[�^�@TRUE:����f�[�^
 */
//--------------------------------------------------------------------------
static  BOOL  pp_decordAct( POKEMON_PARAM *pp )
{
  BOOL  ret = TRUE;
  if( pp->ppp.pp_fast_mode == 0 ){
    POKETOOL_decord_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );
    ret = ppp_decordAct( &pp->ppp );
  }
  return ret;
}
//--------------------------------------------------------------------------
/**
 *  �|�P�����p�����[�^�\���̂𕜍����ă`�F�b�N�T�����`�F�b�N���ĕs���Ȃ�_���^�}�S�ɂ���
 *
 * @param[in] ppp   ��������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 *
 * @return    FALSE:�s���f�[�^�@TRUE:����f�[�^
 */
//--------------------------------------------------------------------------
static  BOOL  ppp_decordAct( POKEMON_PASO_PARAM *ppp )
{
  BOOL  ret = TRUE;
  u16   sum;

  if( ppp->ppp_fast_mode == 0 ){
    POKETOOL_decord_data( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, ppp->checksum );
    sum = POKETOOL_make_checksum( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX );
    if( sum != ppp->checksum ){
      GF_ASSERT_MSG( ( sum == ppp->checksum ), "checksum Crash!\n" );
      ppp->fusei_tamago_flag = 1;
      ret = FALSE;
    }
  }
  return ret;
}
//--------------------------------------------------------------------------
/**
 *  �|�P�����p�����[�^�\���̂��Í�������
 *
 * @param[io] ppp   ��������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
static  void  pp_encode_act( POKEMON_PARAM *pp )
{
  if( pp->ppp.pp_fast_mode == 0 ){
    POKETOOL_encode_data( &pp->pcp, sizeof( POKEMON_CALC_PARAM ), pp->ppp.personal_rnd );
    ppp_encode_act( &pp->ppp );
  }
}

//--------------------------------------------------------------------------
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂��Í�������
 *
 * @param[io] ppp   ��������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 */
//--------------------------------------------------------------------------
static  void  ppp_encode_act( POKEMON_PASO_PARAM *ppp )
{
  if( ppp->ppp_fast_mode == 0 ){
    ppp->checksum = POKETOOL_make_checksum( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX );
    POKETOOL_encode_data( ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX, ppp->checksum );
  }
}

//--------------------------------------------------------------------------
/**
 *  �|�P�����p�����[�^�\���̂���C�ӂŒl���擾
 *
 * @param[in] pp  �擾����|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id  �擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]  buf �擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return    �擾�����f�[�^
 */
//--------------------------------------------------------------------------
static  u32 pp_getAct( POKEMON_PARAM *pp, int id, void *buf )
{
  u32 ret = 0;

//���[���ƃJ�X�^���{�[���Ή����܂��ł�
#ifdef DEBUG_ONLY_FOR_sogabe
#warning CB_CORE nothing
#endif
  switch( id ){
  case ID_PARA_condition:
    ret = pp->pcp.condition;
    break;
  case ID_PARA_level:
    ret = pp->pcp.level;
    break;
  case ID_PARA_cb_id:
    ret = pp->pcp.cb_id;
    break;
  case ID_PARA_hp:
    ret = pp->pcp.hp;
    break;
  case ID_PARA_hpmax:
    ret = pp->pcp.hpmax;
    break;
  case ID_PARA_pow:
    ret = pp->pcp.pow;
    break;
  case ID_PARA_def:
    ret = pp->pcp.def;
    break;
  case ID_PARA_agi:
    ret = pp->pcp.agi;
    break;
  case ID_PARA_spepow:
    ret = pp->pcp.spepow;
    break;
  case ID_PARA_spedef:
    ret = pp->pcp.spedef;
    break;
  case ID_PARA_mail_data:
    MailData_Copy( ( MAIL_DATA * )pp->pcp.mail_data, ( MAIL_DATA * )buf );
    ret = TRUE;
    break;
  case ID_PARA_cb_core:
//    CB_Tool_CoreData_Copy( &pp->pcp.cb_core, ( CB_CORE * )buf );
    ret = TRUE;
    break;
  default:
    ret = ppp_getAct( ( POKEMON_PASO_PARAM * )&pp->ppp, id, buf );
    break;
  }
  return  ret;
}

//--------------------------------------------------------------------------
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂���C�ӂŒl���擾
 *
 * @param[in] pp  �擾����{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id  �擾�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[out]  buf �擾�������f�[�^���z��̎��Ɋi�[��̃A�h���X���w��
 *
 * @return    �擾�����f�[�^
 */
//--------------------------------------------------------------------------
static  u32 ppp_getAct( POKEMON_PASO_PARAM *ppp, int id, void *buf )
{
  u32 ret = 0;

  POKEMON_PASO_PARAM1 *ppp1;
  POKEMON_PASO_PARAM2 *ppp2;
  POKEMON_PASO_PARAM3 *ppp3;
  POKEMON_PASO_PARAM4 *ppp4;

  ppp1 = ( POKEMON_PASO_PARAM1 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA1 );
  ppp2 = ( POKEMON_PASO_PARAM2 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA2 );
  ppp3 = ( POKEMON_PASO_PARAM3 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA3 );
  ppp4 = ( POKEMON_PASO_PARAM4 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA4 );

  switch( id ){
    default:
      ret = 0;
      break;
//PARAM
    case ID_PARA_personal_rnd:
      ret = ppp->personal_rnd;
      break;
    case ID_PARA_pp_fast_mode:
      ret = ppp->pp_fast_mode;
      break;
    case ID_PARA_ppp_fast_mode:
      ret = ppp->ppp_fast_mode;
      break;
    case ID_PARA_fusei_tamago_flag:
      ret = ppp->fusei_tamago_flag;
      break;
    case ID_PARA_checksum:
      ret = ppp->checksum;
      break;
    case ID_PARA_poke_exist:
      ret = ( ppp1->monsno != 0 );
      break;
    case ID_PARA_tamago_exist:
      if( ppp->fusei_tamago_flag ){
        ret = ppp->fusei_tamago_flag;
      }
      else{
        ret = ppp2->tamago_flag;
      }
      break;
    case ID_PARA_monsno_egg:
      ret = ppp1->monsno;
      if( ret == 0){
        break;
      }
      else if( ( ppp2->tamago_flag ) || ( ppp->fusei_tamago_flag ) ){
        ret = MONSNO_TAMAGO;
      }
      break;
    case ID_PARA_level:
      ret = POKETOOL_CalcLevel( ppp1->monsno, ppp2->form_no, ppp1->exp );
      break;
//PARAM1
    case ID_PARA_monsno:
      if( ppp->fusei_tamago_flag ){
        ret = MONSNO_TAMAGO;
      }
      else{
        ret = ppp1->monsno;
      }
      break;
    case ID_PARA_item:
      ret = ppp1->item;
      break;
    case ID_PARA_id_no:
      ret = ppp1->id_no;
      break;
    case ID_PARA_exp:
      ret = ppp1->exp;
      break;
    case ID_PARA_friend:
      ret = ppp1->friend;
      break;
    case ID_PARA_speabino:
      ret = ppp1->speabino;
      break;
    case ID_PARA_mark:
      ret = ppp1->mark;
      break;
    case ID_PARA_country_code:
      ret = ppp1->country_code;
      break;
    case ID_PARA_hp_exp:
      ret = ppp1->hp_exp;
      break;
    case ID_PARA_pow_exp:
      ret = ppp1->pow_exp;
      break;
    case ID_PARA_def_exp:
      ret = ppp1->def_exp;
      break;
    case ID_PARA_agi_exp:
      ret = ppp1->agi_exp;
      break;
    case ID_PARA_spepow_exp:
      ret = ppp1->spepow_exp;
      break;
    case ID_PARA_spedef_exp:
      ret = ppp1->spedef_exp;
      break;
    case ID_PARA_style:
      ret = ppp1->style;
      break;
    case ID_PARA_beautiful:
      ret = ppp1->beautiful;
      break;
    case ID_PARA_cute:
      ret = ppp1->cute;
      break;
    case ID_PARA_clever:
      ret = ppp1->clever;
      break;
    case ID_PARA_strong:
      ret = ppp1->strong;
      break;
    case ID_PARA_fur:
      ret = ppp1->fur;
      break;
    case ID_PARA_sinou_champ_ribbon:        //�V���I�E�`�����v���{��
    case ID_PARA_sinou_battle_tower_ttwin_first:  //�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
    case ID_PARA_sinou_battle_tower_ttwin_second: //�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
    case ID_PARA_sinou_battle_tower_2vs2_win50:   //�V���I�E�o�g���^���[�^���[�_�u��50�A��
    case ID_PARA_sinou_battle_tower_aimulti_win50:  //�V���I�E�o�g���^���[�^���[AI�}���`50�A��
    case ID_PARA_sinou_battle_tower_siomulti_win50: //�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
    case ID_PARA_sinou_battle_tower_wifi_rank5:   //�V���I�E�o�g���^���[Wifi�����N�T����
    case ID_PARA_sinou_syakki_ribbon:       //�V���I�E����������{��
    case ID_PARA_sinou_dokki_ribbon:        //�V���I�E�ǂ������{��
    case ID_PARA_sinou_syonbo_ribbon:       //�V���I�E�����ڃ��{��
    case ID_PARA_sinou_ukka_ribbon:         //�V���I�E���������{��
    case ID_PARA_sinou_sukki_ribbon:        //�V���I�E���������{��
    case ID_PARA_sinou_gussu_ribbon:        //�V���I�E���������{��
    case ID_PARA_sinou_nikko_ribbon:        //�V���I�E�ɂ������{��
    case ID_PARA_sinou_gorgeous_ribbon:       //�V���I�E�S�[�W���X���{��
    case ID_PARA_sinou_royal_ribbon:        //�V���I�E���C�������{��
    case ID_PARA_sinou_gorgeousroyal_ribbon:    //�V���I�E�S�[�W���X���C�������{��
    case ID_PARA_sinou_ashiato_ribbon:        //�V���I�E�������ƃ��{��
    case ID_PARA_sinou_record_ribbon:       //�V���I�E���R�[�h���{��
    case ID_PARA_sinou_history_ribbon:        //�V���I�E�q�X�g���[���{��
    case ID_PARA_sinou_legend_ribbon:       //�V���I�E���W�F���h���{��
    case ID_PARA_sinou_red_ribbon:          //�V���I�E���b�h���{��
    case ID_PARA_sinou_green_ribbon:        //�V���I�E�O���[�����{��
    case ID_PARA_sinou_blue_ribbon:         //�V���I�E�u���[���{��
    case ID_PARA_sinou_festival_ribbon:       //�V���I�E�t�F�X�e�B�o�����{��
    case ID_PARA_sinou_carnival_ribbon:       //�V���I�E�J�[�j�o�����{��
    case ID_PARA_sinou_classic_ribbon:        //�V���I�E�N���V�b�N���{��
    case ID_PARA_sinou_premiere_ribbon:       //�V���I�E�v���~�A���{��
    case ID_PARA_sinou_amari_ribbon:        //���܂�
      {
        u64 bit = 1;
        ret = ( ( ppp1->sinou_ribbon & ( bit << id - ID_PARA_sinou_champ_ribbon ) ) != 0 );
      }
      break;
//PARAM2
    case ID_PARA_waza1:
    case ID_PARA_waza2:
    case ID_PARA_waza3:
    case ID_PARA_waza4:
      ret = ppp2->waza[ id-ID_PARA_waza1 ];
      break;
    case ID_PARA_pp1:
    case ID_PARA_pp2:
    case ID_PARA_pp3:
    case ID_PARA_pp4:
      ret = ppp2->pp[ id-ID_PARA_pp1 ];
      break;
    case ID_PARA_pp_count1:
    case ID_PARA_pp_count2:
    case ID_PARA_pp_count3:
    case ID_PARA_pp_count4:
      ret = ppp2->pp_count[ id - ID_PARA_pp_count1 ];
      break;
    case ID_PARA_pp_max1:
    case ID_PARA_pp_max2:
    case ID_PARA_pp_max3:
    case ID_PARA_pp_max4:
      {
        WazaID waza = ppp2->waza[ id-ID_PARA_pp_max1 ];
        if( waza != WAZANO_NULL ){
          u8 maxup_cnt = ppp2->pp_count[ id-ID_PARA_pp_max1 ];
          ret = WAZADATA_GetMaxPP( waza, maxup_cnt );
        }else{
          ret = 0;
        }
      }
      break;
    case ID_PARA_hp_rnd:
      ret = ppp2->hp_rnd;
      break;
    case ID_PARA_pow_rnd:
      ret = ppp2->pow_rnd;
      break;
    case ID_PARA_def_rnd:
      ret = ppp2->def_rnd;
      break;
    case ID_PARA_agi_rnd:
      ret = ppp2->agi_rnd;
      break;
    case ID_PARA_spepow_rnd:
      ret = ppp2->spepow_rnd;
      break;
    case ID_PARA_spedef_rnd:
      ret = ppp2->spedef_rnd;
      break;
    case ID_PARA_tamago_flag:
      if( ppp->fusei_tamago_flag ){
        ret = ppp->fusei_tamago_flag;
      }
      else{
        ret = ppp2->tamago_flag;
      }
      break;
    case ID_PARA_nickname_flag:
      ret = ppp2->nickname_flag;
      break;
    case ID_PARA_stylemedal_normal:
    case ID_PARA_stylemedal_super:
    case ID_PARA_stylemedal_hyper:
    case ID_PARA_stylemedal_master:
    case ID_PARA_beautifulmedal_normal:
    case ID_PARA_beautifulmedal_super:
    case ID_PARA_beautifulmedal_hyper:
    case ID_PARA_beautifulmedal_master:
    case ID_PARA_cutemedal_normal:
    case ID_PARA_cutemedal_super:
    case ID_PARA_cutemedal_hyper:
    case ID_PARA_cutemedal_master:
    case ID_PARA_clevermedal_normal:
    case ID_PARA_clevermedal_super:
    case ID_PARA_clevermedal_hyper:
    case ID_PARA_clevermedal_master:
    case ID_PARA_strongmedal_normal:
    case ID_PARA_strongmedal_super:
    case ID_PARA_strongmedal_hyper:
    case ID_PARA_strongmedal_master:
    case ID_PARA_champ_ribbon:
    case ID_PARA_winning_ribbon:
    case ID_PARA_victory_ribbon:
    case ID_PARA_bromide_ribbon:
    case ID_PARA_ganba_ribbon:
    case ID_PARA_marine_ribbon:
    case ID_PARA_land_ribbon:
    case ID_PARA_sky_ribbon:
    case ID_PARA_country_ribbon:
    case ID_PARA_national_ribbon:
    case ID_PARA_earth_ribbon:
    case ID_PARA_world_ribbon:
      {
        u64 bit = 1;
        ret = ( ( ppp2->old_ribbon & ( bit << id - ID_PARA_stylemedal_normal ) ) !=0 );
      }
      break;
    case ID_PARA_event_get_flag:
      ret = ppp2->event_get_flag;
      break;
    case ID_PARA_sex:
      //�K���p�����[�^����v�Z���ĕԂ��悤����
      ret = POKETOOL_GetSex( ppp1->monsno, ppp2->form_no, ppp->personal_rnd );
      //�Čv�Z�������̂������Ă���
      ppp2->sex = ret;
      //�`�F�b�N�T�����Čv�Z
      ppp->checksum = POKETOOL_make_checksum( &ppp->paradata, sizeof( POKEMON_PASO_PARAM1 ) * POKE_PARA_BLOCK_MAX );
      break;
    case ID_PARA_form_no:
      ret = ppp2->form_no;
      break;
    case ID_PARA_seikaku:
      ret = ppp2->seikaku;
      break;
    case ID_PARA_tokusei_3_flag:
      ret = ppp2->tokusei_3_flag;
      break;
    case ID_PARA_dummy_p2_3:
      ret = ppp2->dummy_p2_3;
      break;
//PARAM3
    case ID_PARA_nickname:
      if( ppp->fusei_tamago_flag ){
        GFL_MSG_GetString( GlobalMsg_PokeName, MONSNO_DAMETAMAGO, (STRBUF*)buf );
      }
      else{
        GFL_STR_SetStringCode( (STRBUF*)buf, ppp3->nickname );
      }
      break;

    case ID_PARA_nickname_raw:
      if( ppp->fusei_tamago_flag ){
        GFL_MSG_GetStringRaw( GlobalMsg_PokeName, MONSNO_DAMETAMAGO, (STRCODE*)buf, NELEMS(ppp3->nickname) );
      }
      else{
        STRTOOL_Copy( ppp3->nickname, (STRCODE*)buf, NELEMS(ppp3->nickname) );
      }
      break;

    case ID_PARA_pref_code:
      ret = ppp3->pref_code;
      break;
    case ID_PARA_get_cassette:
      ret = ppp3->get_cassette;
      break;
    case ID_PARA_trial_stylemedal_normal:
    case ID_PARA_trial_stylemedal_super:
    case ID_PARA_trial_stylemedal_hyper:
    case ID_PARA_trial_stylemedal_master:
    case ID_PARA_trial_beautifulmedal_normal:
    case ID_PARA_trial_beautifulmedal_super:
    case ID_PARA_trial_beautifulmedal_hyper:
    case ID_PARA_trial_beautifulmedal_master:
    case ID_PARA_trial_cutemedal_normal:
    case ID_PARA_trial_cutemedal_super:
    case ID_PARA_trial_cutemedal_hyper:
    case ID_PARA_trial_cutemedal_master:
    case ID_PARA_trial_clevermedal_normal:
    case ID_PARA_trial_clevermedal_super:
    case ID_PARA_trial_clevermedal_hyper:
    case ID_PARA_trial_clevermedal_master:
    case ID_PARA_trial_strongmedal_normal:
    case ID_PARA_trial_strongmedal_super:
    case ID_PARA_trial_strongmedal_hyper:
    case ID_PARA_trial_strongmedal_master:
    case ID_PARA_amari_ribbon:
      {
        u64 bit = 1;
        ret = ( ( ppp3->new_ribbon & ( bit << id - ID_PARA_trial_stylemedal_normal ) ) != 0 );
      }
      break;
//PARAM4
    case ID_PARA_oyaname:
      GFL_STR_SetStringCode( (STRBUF*)buf, ppp4->oyaname );
      break;

    case ID_PARA_oyaname_raw:
      STRTOOL_Copy( ppp4->oyaname, (STRCODE*)buf, NELEMS(ppp4->oyaname) );
      break;

    case ID_PARA_get_year:              //�߂܂����N
      ret = ppp4->get_year;
      break;
    case ID_PARA_get_month:             //�߂܂�����
      ret = ppp4->get_month;
      break;
    case ID_PARA_get_day:             //�߂܂�����
      ret = ppp4->get_day;
      break;
    case ID_PARA_birth_year:            //���܂ꂽ�N
      ret = ppp4->birth_year;
      break;
    case ID_PARA_birth_month:           //���܂ꂽ��
      ret = ppp4->birth_month;
      break;
    case ID_PARA_birth_day:             //���܂ꂽ��
      ret = ppp4->birth_day;
      break;
    case ID_PARA_get_place:             //�߂܂����ꏊ
      ret = ppp4->get_place;
      break;
    case ID_PARA_birth_place:           //���܂ꂽ�ꏊ
      ret = ppp4->birth_place;
      break;
    case ID_PARA_pokerus:             //�|�P���X
      ret = ppp4->pokerus;
      break;
    case ID_PARA_get_ball:              //�߂܂����{�[��
      ret = ppp4->get_ball;
      break;
    case ID_PARA_get_level:             //�߂܂������x��
      ret = ppp4->get_level;
      break;
    case ID_PARA_oyasex:              //�e�̐���
      ret = ppp4->oyasex;
      break;
    case ID_PARA_get_ground_id:           //�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
      ret = ppp4->get_ground_id;
      break;
    case ID_PARA_dummy_p4_1:            //���܂�
      ret = ppp4->dummy_p4_1;
      break;

    case ID_PARA_power_rnd:
      ret = ( ppp2->hp_rnd    <<  0 )|
          ( ppp2->pow_rnd   <<  5 )|
          ( ppp2->def_rnd   << 10 )|
          ( ppp2->agi_rnd   << 15 )|
          ( ppp2->spepow_rnd  << 20 )|
          ( ppp2->spedef_rnd  << 25 );
      break;
    //�����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N
    case ID_PARA_nidoran_nickname:
      if( ( ( ppp1->monsno == MONSNO_NIDORAN_F ) || ( ppp1->monsno == MONSNO_NIDORAN_M ) ) &&
          ( ppp2->nickname_flag == 0 ) ){
        ret = FALSE;
      }
      else{
        ret = TRUE;
      }
      break;
    case ID_PARA_type1:
    case ID_PARA_type2:
      if( ( ppp1->monsno == MONSNO_ARUSEUSU ) && ( ppp1->speabino == TOKUSYU_MARUTITAIPU ) )
      {
          ret = get_type_from_item( ppp1->item );
      }
      else
      {
        ret = POKETOOL_GetPersonalParam( ppp1->monsno, ppp2->form_no, POKEPER_ID_type1 + ( id - ID_PARA_type1 ) );
      }
      break;
  }
  return  ret;
}

//--------------------------------------------------------------------------
/**
 *  �����ς݃|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[
 *
 * @param[out]  pp      �i�[�������|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] paramID   �i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] arg     �i�[�������f�[�^�^�z��A�h���X���Ƃ��Ă����p�iparamID�ɂ��j
 */
//--------------------------------------------------------------------------
static  void  pp_putAct( POKEMON_PARAM *pp, int paramID, u32 arg )
{
  switch( paramID ){
  case ID_PARA_condition:
    pp->pcp.condition = arg;
    break;
  case ID_PARA_level:
    pp->pcp.level = arg;
    break;
  case ID_PARA_cb_id:
    pp->pcp.cb_id = arg;
    break;
  case ID_PARA_hp:
    pp->pcp.hp = arg;
    break;
  case ID_PARA_hpmax:
    pp->pcp.hpmax = arg;
    break;
  case ID_PARA_pow:
    pp->pcp.pow = arg;
    break;
  case ID_PARA_def:
    pp->pcp.def = arg;
    break;
  case ID_PARA_agi:
    pp->pcp.agi = arg;
    break;
  case ID_PARA_spepow:
    pp->pcp.spepow = arg;
    break;
  case ID_PARA_spedef:
    pp->pcp.spedef = arg;
    break;
  case ID_PARA_mail_data:
    MailData_Copy( ( MAIL_DATA * )arg, ( MAIL_DATA * )pp->pcp.mail_data );
    break;
  case ID_PARA_cb_core:
//�J�X�^���{�[�������Ȃ��ł�
#ifdef DEBUG_ONLY_FOR_sogabe
#warning CB_CORE Nothing
#endif
//    CB_Tool_CoreData_Copy( ( CB_CORE * )buf, &pp->pcp.cb_core );
    break;

  default:
    ppp_putAct( ( POKEMON_PASO_PARAM * )&pp->ppp, paramID, arg );
    break;
  }
}

//----------------------------------------------------------------------------------
/**
 * �w�͒l�i�[���̒��ߏ����i�ʒl�A���v�l�Ƃ��ɏ���𒴂��Ȃ��悤�Ɂj
 *
 * @param   ppp1
 * @param   currentExp  ���݂̓w�͒l
 * @param   nextExp     �i�[���N�G�X�g�̂������w�͒l
 *
 * @retval  u32         �i�[����w�͒l
 */
//----------------------------------------------------------------------------------
static inline u32 adjustExp( POKEMON_PASO_PARAM1* ppp1, u32 currentExp, u32 nextExp )
{
  u32 sum, margin;

  sum = ppp1->hp_exp
      + ppp1->pow_exp
      + ppp1->def_exp
      + ppp1->agi_exp
      + ppp1->spepow_exp
      + ppp1->spedef_exp;

  margin = PARA_EXP_TOTAL_MAX - sum;
  if( nextExp > margin ){
    nextExp = margin;
  }

  if( nextExp > PARA_EXP_MAX ){
    nextExp = PARA_EXP_MAX;
  }

  return nextExp;
}

//--------------------------------------------------------------------------
/**
 *  �����ς݃{�b�N�X�|�P�����p�����[�^�\���̂ɔC�ӂŒl���i�[
 *
 * @param[out]  pp      �i�[�������|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] paramID   �i�[�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] arg     �i�[�������f�[�^�^�z��A�h���X���Ƃ��Ă����p�iparamID�ɂ��j
 */
//--------------------------------------------------------------------------
static  void  ppp_putAct( POKEMON_PASO_PARAM *ppp, int paramID, u32 arg )
{
  POKEMON_PASO_PARAM1 *ppp1;
  POKEMON_PASO_PARAM2 *ppp2;
  POKEMON_PASO_PARAM3 *ppp3;
  POKEMON_PASO_PARAM4 *ppp4;

  ppp1 = ( POKEMON_PASO_PARAM1 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA1 );
  ppp2 = ( POKEMON_PASO_PARAM2 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA2 );
  ppp3 = ( POKEMON_PASO_PARAM3 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA3 );
  ppp4 = ( POKEMON_PASO_PARAM4 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA4 );

  switch( paramID ){
//PARAM
    case ID_PARA_personal_rnd:
      ppp->personal_rnd = arg;
      break;
    case ID_PARA_pp_fast_mode:
      GF_ASSERT_MSG( ( 0 ), "fast_mode�֕s���ȏ�������\n" );
      ppp->pp_fast_mode = arg;
      break;
    case ID_PARA_ppp_fast_mode:
      GF_ASSERT_MSG( ( 0 ), "fast_mode�֕s���ȏ�������\n" );
      ppp->ppp_fast_mode = arg;
      break;
    case ID_PARA_fusei_tamago_flag:
      ppp->fusei_tamago_flag = arg;
      break;
    case ID_PARA_checksum:
      ppp->checksum = arg;
      break;
//PARAM1
    case ID_PARA_monsno:
      ppp1->monsno = arg;
      break;
    case ID_PARA_item:
      ppp1->item = arg;
      break;
    case ID_PARA_id_no:
      ppp1->id_no = arg;
      break;
    case ID_PARA_exp:
      ppp1->exp = arg;
      break;
    case ID_PARA_friend:
      ppp1->friend = arg;
      break;
    case ID_PARA_speabino:
      ppp1->speabino = arg;
      break;
    case ID_PARA_mark:
      ppp1->mark = arg;
      break;
    case ID_PARA_country_code:
      ppp1->country_code = arg;
      break;
    case ID_PARA_hp_exp:
      ppp1->hp_exp = adjustExp( ppp1, ppp1->hp_exp, arg );
      break;
    case ID_PARA_pow_exp:
      ppp1->pow_exp = adjustExp( ppp1, ppp1->pow_exp, arg );
      break;
    case ID_PARA_def_exp:
      ppp1->def_exp = adjustExp( ppp1, ppp1->def_exp, arg );
      break;
    case ID_PARA_agi_exp:
      ppp1->agi_exp = adjustExp( ppp1, ppp1->agi_exp, arg );
      break;
    case ID_PARA_spepow_exp:
      ppp1->spepow_exp = adjustExp( ppp1, ppp1->spepow_exp, arg );
      break;
    case ID_PARA_spedef_exp:
      ppp1->spedef_exp = adjustExp( ppp1, ppp1->spedef_exp, arg );
      break;
    case ID_PARA_style:
      ppp1->style = arg;
      break;
    case ID_PARA_beautiful:
      ppp1->beautiful = arg;
      break;
    case ID_PARA_cute:
      ppp1->cute = arg;
      break;
    case ID_PARA_clever:
      ppp1->clever = arg;
      break;
    case ID_PARA_strong:
      ppp1->strong = arg;
      break;
    case ID_PARA_fur:
      ppp1->fur = arg;
      break;
    case ID_PARA_sinou_champ_ribbon:        //�V���I�E�`�����v���{��
    case ID_PARA_sinou_battle_tower_ttwin_first:  //�V���I�E�o�g���^���[�^���[�^�C�N�[������1���
    case ID_PARA_sinou_battle_tower_ttwin_second: //�V���I�E�o�g���^���[�^���[�^�C�N�[������2���
    case ID_PARA_sinou_battle_tower_2vs2_win50:   //�V���I�E�o�g���^���[�^���[�_�u��50�A��
    case ID_PARA_sinou_battle_tower_aimulti_win50:  //�V���I�E�o�g���^���[�^���[AI�}���`50�A��
    case ID_PARA_sinou_battle_tower_siomulti_win50: //�V���I�E�o�g���^���[�^���[�ʐM�}���`50�A��
    case ID_PARA_sinou_battle_tower_wifi_rank5:   //�V���I�E�o�g���^���[Wifi�����N�T����
    case ID_PARA_sinou_syakki_ribbon:       //�V���I�E����������{��
    case ID_PARA_sinou_dokki_ribbon:        //�V���I�E�ǂ������{��
    case ID_PARA_sinou_syonbo_ribbon:       //�V���I�E�����ڃ��{��
    case ID_PARA_sinou_ukka_ribbon:         //�V���I�E���������{��
    case ID_PARA_sinou_sukki_ribbon:        //�V���I�E���������{��
    case ID_PARA_sinou_gussu_ribbon:        //�V���I�E���������{��
    case ID_PARA_sinou_nikko_ribbon:        //�V���I�E�ɂ������{��
    case ID_PARA_sinou_gorgeous_ribbon:       //�V���I�E�S�[�W���X���{��
    case ID_PARA_sinou_royal_ribbon:        //�V���I�E���C�������{��
    case ID_PARA_sinou_gorgeousroyal_ribbon:    //�V���I�E�S�[�W���X���C�������{��
    case ID_PARA_sinou_ashiato_ribbon:        //�V���I�E�������ƃ��{��
    case ID_PARA_sinou_record_ribbon:       //�V���I�E���R�[�h���{��
    case ID_PARA_sinou_history_ribbon:        //�V���I�E�q�X�g���[���{��
    case ID_PARA_sinou_legend_ribbon:       //�V���I�E���W�F���h���{��
    case ID_PARA_sinou_red_ribbon:          //�V���I�E���b�h���{��
    case ID_PARA_sinou_green_ribbon:        //�V���I�E�O���[�����{��
    case ID_PARA_sinou_blue_ribbon:         //�V���I�E�u���[���{��
    case ID_PARA_sinou_festival_ribbon:       //�V���I�E�t�F�X�e�B�o�����{��
    case ID_PARA_sinou_carnival_ribbon:       //�V���I�E�J�[�j�o�����{��
    case ID_PARA_sinou_classic_ribbon:        //�V���I�E�N���V�b�N���{��
    case ID_PARA_sinou_premiere_ribbon:       //�V���I�E�v���~�A���{��
    case ID_PARA_sinou_amari_ribbon:        //���܂�
      {
        u64 bit = 1 << ( paramID - ID_PARA_sinou_champ_ribbon );
        if( arg ){
          ppp1->sinou_ribbon |= bit;
        }
        else{
          ppp1->sinou_ribbon &= ( bit ^ 0xffffffff );
        }
      }
      break;
//PARAM2
    case ID_PARA_waza1:
    case ID_PARA_waza2:
    case ID_PARA_waza3:
    case ID_PARA_waza4:
      ppp2->waza[ paramID - ID_PARA_waza1 ] = arg;
      break;
    case ID_PARA_pp1:
    case ID_PARA_pp2:
    case ID_PARA_pp3:
    case ID_PARA_pp4:
      ppp2->pp[ paramID - ID_PARA_pp1 ] = arg;
      break;
    case ID_PARA_pp_count1:
    case ID_PARA_pp_count2:
    case ID_PARA_pp_count3:
    case ID_PARA_pp_count4:
      ppp2->pp_count[ paramID - ID_PARA_pp_count1 ] = arg;
      break;
    case ID_PARA_pp_max1:
    case ID_PARA_pp_max2:
    case ID_PARA_pp_max3:
    case ID_PARA_pp_max4:
      OS_Printf("�s���ȏ�������\n");
      break;
    case ID_PARA_hp_rnd:
      ppp2->hp_rnd = arg;
      break;
    case ID_PARA_pow_rnd:
      ppp2->pow_rnd = arg;
      break;
    case ID_PARA_def_rnd:
      ppp2->def_rnd = arg;
      break;
    case ID_PARA_agi_rnd:
      ppp2->agi_rnd = arg;
      break;
    case ID_PARA_spepow_rnd:
      ppp2->spepow_rnd = arg;
      break;
    case ID_PARA_spedef_rnd:
      ppp2->spedef_rnd = arg;
      break;
    case ID_PARA_tamago_flag:
      ppp2->tamago_flag = arg;
      break;
    case ID_PARA_stylemedal_normal:
    case ID_PARA_stylemedal_super:
    case ID_PARA_stylemedal_hyper:
    case ID_PARA_stylemedal_master:
    case ID_PARA_beautifulmedal_normal:
    case ID_PARA_beautifulmedal_super:
    case ID_PARA_beautifulmedal_hyper:
    case ID_PARA_beautifulmedal_master:
    case ID_PARA_cutemedal_normal:
    case ID_PARA_cutemedal_super:
    case ID_PARA_cutemedal_hyper:
    case ID_PARA_cutemedal_master:
    case ID_PARA_clevermedal_normal:
    case ID_PARA_clevermedal_super:
    case ID_PARA_clevermedal_hyper:
    case ID_PARA_clevermedal_master:
    case ID_PARA_strongmedal_normal:
    case ID_PARA_strongmedal_super:
    case ID_PARA_strongmedal_hyper:
    case ID_PARA_strongmedal_master:
    case ID_PARA_champ_ribbon:
    case ID_PARA_winning_ribbon:
    case ID_PARA_victory_ribbon:
    case ID_PARA_bromide_ribbon:
    case ID_PARA_ganba_ribbon:
    case ID_PARA_marine_ribbon:
    case ID_PARA_land_ribbon:
    case ID_PARA_sky_ribbon:
    case ID_PARA_country_ribbon:
    case ID_PARA_national_ribbon:
    case ID_PARA_earth_ribbon:
    case ID_PARA_world_ribbon:
      {
        u64 bit = 1 << ( paramID - ID_PARA_stylemedal_normal );
        if( arg ){
          ppp2->old_ribbon |= bit;
        }
        else{
          ppp2->old_ribbon &= ( bit ^ 0xffffffff );
        }
      }
      break;
    case ID_PARA_event_get_flag:
      ppp2->event_get_flag = arg;
      break;
    case ID_PARA_sex:
      //ppp2->sex=buf8[0];
      //�K���p�����[�^����v�Z���đ������
      ppp2->sex = POKETOOL_GetSex( ppp1->monsno, ppp2->form_no, ppp->personal_rnd );
      GF_ASSERT_MSG( ( arg == ppp2->sex ), "monsno=%d, Disagreement personal_rnd(%d) <> ID_PARA_sex(%d)\n", ppp1->monsno, ppp2->sex, arg);
      break;
    case ID_PARA_form_no:
      ppp2->form_no = arg;
      break;
    case ID_PARA_seikaku:
      ppp2->seikaku = arg;
      break;
    case ID_PARA_tokusei_3_flag:
      ppp2->tokusei_3_flag = arg;
      break;
    case ID_PARA_dummy_p2_3:
      ppp2->dummy_p2_3 = arg;
      break;

    case ID_PARA_nickname_flag:
      GF_ASSERT_MSG(0, "�j�b�N�l�[���t���O�𒼐ڃZ�b�g���邱�Ƃ͂ł��܂���\n");
      break;
//PARAM3
    case ID_PARA_nickname:
      GFL_STR_GetStringCode( (STRBUF*)arg, ppp3->nickname, NELEMS(ppp3->nickname) );
      ppp2->nickname_flag = pppAct_check_nickname( ppp );
      break;
    case ID_PARA_nickname_raw:
      STRTOOL_Copy( (const STRCODE*)arg, ppp3->nickname, NELEMS(ppp3->nickname) );
      ppp2->nickname_flag = pppAct_check_nickname( ppp );
      break;

    case ID_PARA_pref_code:
      ppp3->pref_code = arg;
      break;
    case ID_PARA_get_cassette:
      ppp3->get_cassette = arg;
      break;
    case ID_PARA_trial_stylemedal_normal:
    case ID_PARA_trial_stylemedal_super:
    case ID_PARA_trial_stylemedal_hyper:
    case ID_PARA_trial_stylemedal_master:
    case ID_PARA_trial_beautifulmedal_normal:
    case ID_PARA_trial_beautifulmedal_super:
    case ID_PARA_trial_beautifulmedal_hyper:
    case ID_PARA_trial_beautifulmedal_master:
    case ID_PARA_trial_cutemedal_normal:
    case ID_PARA_trial_cutemedal_super:
    case ID_PARA_trial_cutemedal_hyper:
    case ID_PARA_trial_cutemedal_master:
    case ID_PARA_trial_clevermedal_normal:
    case ID_PARA_trial_clevermedal_super:
    case ID_PARA_trial_clevermedal_hyper:
    case ID_PARA_trial_clevermedal_master:
    case ID_PARA_trial_strongmedal_normal:
    case ID_PARA_trial_strongmedal_super:
    case ID_PARA_trial_strongmedal_hyper:
    case ID_PARA_trial_strongmedal_master:
    case ID_PARA_amari_ribbon:
      {
        u64 bit = 1 << ( paramID - ID_PARA_trial_stylemedal_normal );
        if( arg ){
          ppp3->new_ribbon |= bit;
        }
        else{
          ppp3->new_ribbon &= ( bit ^ 0xffffffffffffffff );
        }
      }
      break;
//PARAM4
    case ID_PARA_oyaname:
      GFL_STR_GetStringCode( (STRBUF*)arg, ppp4->oyaname, NELEMS(ppp4->oyaname) );
      break;
    case ID_PARA_oyaname_raw:
      STRTOOL_Copy( (const STRCODE*)arg, ppp4->oyaname, NELEMS(ppp4->oyaname) );
      break;

    case ID_PARA_get_year:              //�߂܂����N
      ppp4->get_year = arg;
      break;
    case ID_PARA_get_month:             //�߂܂�����
      ppp4->get_month = arg;
      break;
    case ID_PARA_get_day:             //�߂܂�����
      ppp4->get_day = arg;
      break;
    case ID_PARA_birth_year:            //���܂ꂽ�N
      ppp4->birth_year = arg;
      break;
    case ID_PARA_birth_month:           //���܂ꂽ��
      ppp4->birth_month = arg;
      break;
    case ID_PARA_birth_day:             //���܂ꂽ��
      ppp4->birth_day = arg;
      break;
    case ID_PARA_get_place:             //�߂܂����ꏊ
      ppp4->get_place = arg;
      break;
    case ID_PARA_birth_place:           //���܂ꂽ�ꏊ
      ppp4->birth_place = arg;
      break;
    case ID_PARA_pokerus:             //�|�P���X
      ppp4->pokerus = arg;
      break;
    case ID_PARA_get_ball:              //�߂܂����{�[��
      ppp4->get_ball = arg;
      break;
    case ID_PARA_get_level:             //�߂܂������x��
      ppp4->get_level = arg;
      break;
    case ID_PARA_oyasex:              //�e�̐���
      ppp4->oyasex = arg;
      break;
    case ID_PARA_get_ground_id:           //�߂܂����ꏊ�̒n�`�A�g���r���[�g�i�~�m�b�`�p�j
      ppp4->get_ground_id = arg;
      break;
    case ID_PARA_dummy_p4_1:            //���܂�
      ppp4->dummy_p4_1 = arg;
      break;

    case ID_PARA_power_rnd:
      ppp2->hp_rnd   = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_HP );
      ppp2->pow_rnd  = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_ATK );
      ppp2->def_rnd  = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_DEF );
      ppp2->spepow_rnd = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_SPATK );
      ppp2->spedef_rnd = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_SPDEF );
      ppp2->agi_rnd  = PTL_SETUP_POW_UNPACK( arg, PTL_ABILITY_AGI );
      break;

    //�����X�^�[�i���o�[���j�h�����̎���nickname_flag�������Ă��邩�`�F�b�N
    case ID_PARA_nidoran_nickname:
    case ID_PARA_type1:
    case ID_PARA_type2:
      OS_TPrintf("�s���ȏ�������\n");
      break;
  }
}



//============================================================================================
/**
 *  �|�P�����p�����[�^�\���̂ɔC�ӂŒl�����Z
 *
 * @param[in] pp    ���Z�������|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id    ���Z�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] value ���Z�������f�[�^�̃|�C���^
 */
//============================================================================================
static  void  pp_addAct( POKEMON_PARAM *pp, int id, int value )
{
  switch( id ){
    case ID_PARA_hp:
      if( ( pp->pcp.hp + value ) > pp->pcp.hpmax ){
        pp->pcp.hp = pp->pcp.hpmax;
      }
      else if( ( pp->pcp.hp + value ) < 0 ){
        pp->pcp.hp = 0;
      }
      else{
        pp->pcp.hp += value;
      }
      break;
    case ID_PARA_condition:
    case ID_PARA_level:
    case ID_PARA_cb_id:
    case ID_PARA_hpmax:
    case ID_PARA_pow:
    case ID_PARA_def:
    case ID_PARA_agi:
    case ID_PARA_spepow:
    case ID_PARA_spedef:
    case ID_PARA_mail_data:
      GF_ASSERT_MSG( (0), "Add�ł��Ȃ��p�����[�^�ł�\n" );
      break;
    default:
      ppp_addAct( ( POKEMON_PASO_PARAM * )&pp->ppp, id, value );
      break;
  }
}

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̂ɔC�ӂŒl�����Z
 *
 * @param[in] ppp   ���Z�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] id    ���Z�������f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 * @param[in] value ���Z�������f�[�^�̃|�C���^
 */
//============================================================================================
static  void  ppp_addAct( POKEMON_PASO_PARAM *ppp, int id, int value )
{
  POKEMON_PASO_PARAM1 *ppp1;
  POKEMON_PASO_PARAM2 *ppp2;
  POKEMON_PASO_PARAM3 *ppp3;
  POKEMON_PASO_PARAM4 *ppp4;

  ppp1 = ( POKEMON_PASO_PARAM1 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA1 );
  ppp2 = ( POKEMON_PASO_PARAM2 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA2 );
  ppp3 = ( POKEMON_PASO_PARAM3 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA3 );
  ppp4 = ( POKEMON_PASO_PARAM4 * )POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA4 );

  switch( id ){
    case ID_PARA_exp:
      {
        u32 max_exp = POKETOOL_GetMinExp( ppp1->monsno, ppp2->form_no, PTL_LEVEL_MAX );
        if( ( ppp1->exp + value ) > max_exp )
        {
          ppp1->exp = max_exp;
        }
        else
        {
          ppp1->exp += value;
        }
      }
      break;
    case ID_PARA_friend:
      round_calc( &ppp1->friend, value, PTL_FRIEND_MAX );
      break;
    case ID_PARA_hp_exp:
    case ID_PARA_pow_exp:
    case ID_PARA_def_exp:
    case ID_PARA_agi_exp:
    case ID_PARA_spepow_exp:
    case ID_PARA_spedef_exp:
      GF_ASSERT_MSG( ( 0 ), "�s���ȉ��Z�F�w�͒l�́A���v�l�̏�������܂��Ă���̂ŁA�ʂ̉��Z�̓T�|�[�g���܂���\n");
      break;
    case ID_PARA_style:
      round_calc( &ppp1->style, value, PTL_STYLE_MAX );
      break;
    case ID_PARA_beautiful:
      round_calc( &ppp1->beautiful, value, PTL_BEAUTIFUL_MAX );
      break;
    case ID_PARA_cute:
      round_calc( &ppp1->cute, value, PTL_CUTE_MAX );
      break;
    case ID_PARA_clever:
      round_calc( &ppp1->clever, value, PTL_CLEVER_MAX );
      break;
    case ID_PARA_strong:
      round_calc( &ppp1->strong, value, PTL_STRONG_MAX );
      break;
    case ID_PARA_fur:
      round_calc( &ppp1->fur, value, PTL_FUR_MAX );
      break;
    case ID_PARA_pp1:
    case ID_PARA_pp2:
    case ID_PARA_pp3:
    case ID_PARA_pp4:
      round_calc( &ppp2->pp[ id - ID_PARA_pp1 ],
             value,
             WAZADATA_GetMaxPP( ppp2->waza[ id - ID_PARA_pp1 ], ppp2->pp_count[ id - ID_PARA_pp1 ] ) );
      break;
    case ID_PARA_pp_count1:
    case ID_PARA_pp_count2:
    case ID_PARA_pp_count3:
    case ID_PARA_pp_count4:
      round_calc( &ppp2->pp_count[ id - ID_PARA_pp_count1 ], value, PTL_WAZAPP_COUNT_MAX );
      break;
    default:
      GF_ASSERT_MSG( ( 0 ), "�s���ȉ��Z\n" );
      break;
  }
}
//--------------------------------------------------------------------------
/**
 * ���l�v�Z��A0�ȏ�`�w�����l�ȉ��ɂȂ�悤�Ɋۂ߂�
 *
 * @param[io]   data    �v�Z�O���l���[�N
 * @param[in]   value   ���Z�i���Z�j���������l
 * @param[in]   max     ����l
 */
//--------------------------------------------------------------------------
static  void  round_calc( u8 *data, int value, int max )
{
  int work=*data;

  if( ( work + value ) > max ){
    work = max;
  }
  if( ( work + value ) < 0 ){
    work = 0;
  }
  else{
    work += value;
  }

  *data = ( u8 )work;
}



//--------------------------------------------------------------------------
/**
 * �Í�����
 *
 * @param[io] data  �Í�������f�[�^�̃|�C���^
 * @param[in] size  �Í�������f�[�^�̃T�C�Y
 * @param[in] code  �Í����L�[�̏����l
 */
//--------------------------------------------------------------------------
void  POKETOOL_encode_data( void *data, u32 size, u32 code )
{
  int i;
  u16 *data_p = ( u16 * )data;

  //�Í��́A�����Í��L�[�Ń}�X�N
  for( i = 0 ; i < size / 2 ; i++ ){
    data_p[i] ^= rand_next( &code );
  }
}
//--------------------------------------------------------------------------
/**
 *  ��������
 *
 * @param[io] data  ��������f�[�^�̃|�C���^
 * @param[in] size  ��������f�[�^�̃T�C�Y
 * @param[in] code  �Í����L�[�̏����l
 */
//--------------------------------------------------------------------------
//static  void  POKETOOL_decord_data( void *data, u32 size, u32 code )
//{
//  POKETOOL_encode_data( data, size, code );
//}

//--------------------------------------------------------------------------
/**
 *  �����Í��L�[�������[�`��
 *
 * @param[io] code  �Í��L�[�i�[���[�N�̃|�C���^
 *
 * @return  �Í��L�[�i�[���[�N�̏��2�o�C�g���Í��L�[�Ƃ��ĕԂ�
 */
//--------------------------------------------------------------------------
static  u16 rand_next( u32 *code )
{
    code[0] = code[0] *1103515245L + 24691;
    return ( u16 )( code[0] / 65536L ) ;
}

//--------------------------------------------------------------------------
/**
 *  �|�P�����p�����[�^�̃`�F�b�N�T���𐶐�
 *
 * @param[in] data  �|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] size  �`�F�b�N�T���𐶐�����f�[�^�̃T�C�Y
 *
 * @return  ���������`�F�b�N�T��
 */
//--------------------------------------------------------------------------
u16 POKETOOL_make_checksum( const void *data, u32 size )
{
  const u16 *data_p = ( u16 * )data;
  u16 sum = 0;
  int i;

  for( i = 0 ; i < size / 2 ; i++ ){
    sum += data_p[i];
  }

  return sum;
}

//--------------------------------------------------------------------------
/**
 * �����ς�PPP�ɂ��āA�j�b�N�l�[�����t�����Ă��邩����
 *
 * @param   ppp     �����ς�PPP
 *
 * @retval  BOOL    nickname�t�B�[���h�Ƀf�t�H���g���������Ă��Ȃ��ꍇ��TRUE
 */
//--------------------------------------------------------------------------
static BOOL pppAct_check_nickname( POKEMON_PASO_PARAM* ppp )
{
  POKEMON_PASO_PARAM1 *ppp1;
  POKEMON_PASO_PARAM3 *ppp3;

  ppp1 = (POKEMON_PASO_PARAM1*)POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA1 );
  ppp3 = (POKEMON_PASO_PARAM3*)POKETOOL_ppp_get_param_block( ppp, ppp->personal_rnd, ID_POKEPARA3 );

  GFL_MSG_GetStringRaw( GlobalMsg_PokeName, ppp1->monsno, StrBuffer, NELEMS(StrBuffer) );
  return !STRTOOL_Comp( StrBuffer, ppp3->nickname );
}

//============================================================================================
/**
 *  ���i�ɂ��p�����[�^�ω��l�e�[�u��
 */
//============================================================================================
static  const s8  SeikakuAbiTbl[][5]={
//    atk def sat sdf agi
  {  0,  0,  0,  0,  0, },  // ����΂��
  {  1, -1,  0,  0,  0, },  // ���݂�����
  {  1,  0,  0,  0, -1, },  // �䂤����
  {  1,  0, -1,  0,  0, },  // �������ς�
  {  1,  0,  0, -1,  0, },  // ��񂿂�
  { -1,  1,  0,  0,  0, },  // ���ԂƂ�
  {  0,  0,  0,  0,  0, },  // ���Ȃ�
  {  0,  1,  0,  0, -1, },  // �̂�
  {  0,  1, -1,  0,  0, },  // ���ς�
  {  0,  1,  0, -1,  0, },  // �̂��Ă�
  { -1,  0,  0,  0,  1, },  // �����т傤
  {  0, -1,  0,  0,  1, },  // ��������
  {  0,  0,  0,  0,  0, },  // �܂���
  {  0,  0, -1,  0,  1, },  // �悤��
  {  0,  0,  0, -1,  1, },  // �ނ��Ⴋ
  { -1,  0,  1,  0,  0, },  // �Ђ�����
  {  0, -1,  1,  0,  0, },  // �����Ƃ�
  {  0,  0,  1,  0, -1, },  // �ꂢ����
  {  0,  0,  0,  0,  0, },  // �Ă��
  {  0,  0,  1, -1,  0, },  // ���������
  { -1,  0,  0,  1,  0, },  // �����₩
  {  0, -1,  0,  1,  0, },  // ���ƂȂ���
  {  0,  0,  0,  1, -1, },  // �Ȃ܂���
  {  0,  0, -1,  1,  0, },  // ���񂿂傤
  {  0,  0,  0,  0,  0, },  // ���܂���
};

//============================================================================================
/**
 *  ���i�ɂ��p�����[�^�ω��v�Z���[�`��
 *
 * @param[in] chr   �v�Z���鐫�i
 * @param[in] para  �v�Z����p�����[�^�l
 * @param[in] cond  ���o���p�����[�^�ω��l�e�[�u���̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return  �v�Z����
 */
//============================================================================================
static  u16 calc_abi_seikaku( u8 chr, u16 para, u8 cond )
{
  u16 ret;

  if( cond >= PTL_ABILITY_MAX )
  {
    GF_ASSERT(0);
    return para;
  }

  switch( SeikakuAbiTbl[ chr ][ cond - 1 ]){
    case 1:
      ret = para * 110;
      ret /= 100;
      break;
    case -1:
      ret = para * 90;
      ret /= 100;
      break;
    default:
      ret = para;
      break;
  }
  return  ret;
}

//============================================================================================
/**
 *  ���i�ɂ��p�����[�^�ω��l���擾
 *
 * @param[in] chr   �擾���鐫�i
 * @param[in] cond  ���o���p�����[�^�ω��l�e�[�u���̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return  0 or 1 or -1
 */
//============================================================================================
PtlSeikakuChgValue  POKETOOL_GetSeikakuChangeValue( u8 chr, u8 cond )
{ 
  GF_ASSERT( cond != PTL_ABILITY_HP );
  GF_ASSERT( cond < PTL_ABILITY_MAX );

  return ( SeikakuAbiTbl[ chr ][ cond - 1 ] );
}

//============================================================================================
/**
 *  �|�P���������e�[�u���f�[�^���擾
 *
 * @param[in] para    �擾���鐬���e�[�u���̃C���f�b�N�X�i0�`7�j
 * @param[out]  GrowTable �擾���������e�[�u���̊i�[��
 */
//============================================================================================
static  void  load_grow_table( int para, u32 *GrowTable )
{
  GF_ASSERT_MSG( para < 8, "load_grow_table:TableIndexOver!\n" );
  GFL_ARC_LoadData( GrowTable, ARCID_GROW_TBL, para );
}

//============================================================================================
/**
 *  �|�P���������e�[�u���f�[�^����C�ӂŌo���l�f�[�^���擾
 *
 * @param[in] para    �擾���鐬���e�[�u���̃C���f�b�N�X�i0�`7�j
 * @param[in] level   �擾���鐬���e�[�u���̃��x���i0�`100�j
 *
 * @return  �擾�����o���l�f�[�^
 */
//============================================================================================
static  u32 get_growtbl_param( int para, int level )
{
  GF_ASSERT_MSG( para < 8, "get_growtbl_param:TableIndexOver!\n" );
  GF_ASSERT_MSG( level <= 101, "get_growtbl_param:Level Over!\n" );

  load_grow_table( para, &GrowTable[0] );

  return GrowTable[ level ];
}

//=============================================================================================
/**
 *  �����A�C�e������^�C�v���擾
 *
 * @param[in] item �����A�C�e��
 *
 * @retval  PokeType
 */
//=============================================================================================
static  PokeType  get_type_from_item( u16 item )
{
  PokeType  type;

  switch( item ){
  case ITEM_HINOTAMAPUREETO:
    type = POKETYPE_HONOO;
    break;
  case ITEM_SIZUKUPUREETO:
    type = POKETYPE_MIZU;
    break;
  case ITEM_IKAZUTIPUREETO:
    type = POKETYPE_DENKI;
    break;
  case ITEM_MIDORINOPUREETO:
    type = POKETYPE_KUSA;
    break;
  case ITEM_TURARANOPUREETO:
    type = POKETYPE_KOORI;
    break;
  case ITEM_KOBUSINOPUREETO:
    type = POKETYPE_KAKUTOU;
    break;
  case ITEM_MOUDOKUPUREETO:
    type = POKETYPE_DOKU;
    break;
  case ITEM_DAITINOPUREETO:
    type = POKETYPE_JIMEN;
    break;
  case ITEM_AOZORAPUREETO:
    type = POKETYPE_HIKOU;
    break;
  case ITEM_HUSIGINOPUREETO:
    type = POKETYPE_ESPER;
    break;
  case ITEM_TAMAMUSIPUREETO:
    type = POKETYPE_MUSHI;
    break;
  case ITEM_GANSEKIPUREETO:
    type = POKETYPE_IWA;
    break;
  case ITEM_MONONOKEPUREETO:
    type = POKETYPE_GHOST;
    break;
  case ITEM_RYUUNOPUREETO:
    type = POKETYPE_DRAGON;
    break;
  case ITEM_KOWAMOTEPUREETO:
    type = POKETYPE_AKU;
    break;
  case ITEM_KOUTETUPUREETO:
    type = POKETYPE_HAGANE;
    break;
  default:
    type = POKETYPE_NORMAL;
    break;
  }
  return type;
}


//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̃A�h���X�擾�p�e�[�u���l
 */
//============================================================================================

//============================================================================================
/**
 *  �{�b�N�X�|�P�����p�����[�^�\���̓��̃����o�̃A�h���X���擾
 *
 * @param[in] ppp �擾�������{�b�N�X�|�P�����p�����[�^�\���̂̃|�C���^
 * @param[in] rnd �\���̓���ւ��̃L�[
 * @param[in] id  ���o�����������o�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return  �擾�����A�h���X
 */
//============================================================================================
void  *POKETOOL_ppp_get_param_block( POKEMON_PASO_PARAM *ppp, u32 rnd, u8 id )
{
  enum {
    POS1 = ( sizeof( POKEMON_PASO_PARAM1 ) * 0 ),
    POS2 = ( sizeof( POKEMON_PASO_PARAM1 ) * 1 ),
    POS3 = ( sizeof( POKEMON_PASO_PARAM1 ) * 2 ),
    POS4 = ( sizeof( POKEMON_PASO_PARAM1 ) * 3 ),
  };

  static  const u8 PokeParaAdrsTbl[32][4] =
  {
    { POS1, POS2, POS3, POS4 },
    { POS1, POS2, POS4, POS3 },
    { POS1, POS3, POS2, POS4 },
    { POS1, POS4, POS2, POS3 },
    { POS1, POS3, POS4, POS2 },
    { POS1, POS4, POS3, POS2 },
    { POS2, POS1, POS3, POS4 },
    { POS2, POS1, POS4, POS3 },
    { POS3, POS1, POS2, POS4 },
    { POS4, POS1, POS2, POS3 },
    { POS3, POS1, POS4, POS2 },
    { POS4, POS1, POS3, POS2 },
    { POS2, POS3, POS1, POS4 },
    { POS2, POS4, POS1, POS3 },
    { POS3, POS2, POS1, POS4 },
    { POS4, POS2, POS1, POS3 },
    { POS3, POS4, POS1, POS2 },
    { POS4, POS3, POS1, POS2 },
    { POS2, POS3, POS4, POS1 },
    { POS2, POS4, POS3, POS1 },
    { POS3, POS2, POS4, POS1 },
    { POS4, POS2, POS3, POS1 },
    { POS3, POS4, POS2, POS1 },
    { POS4, POS3, POS2, POS1 },
    { POS1, POS2, POS3, POS4 },
    { POS1, POS2, POS4, POS3 },
    { POS1, POS3, POS2, POS4 },
    { POS1, POS4, POS2, POS3 },
    { POS1, POS3, POS4, POS2 },
    { POS1, POS4, POS3, POS2 },
    { POS2, POS1, POS3, POS4 },
    { POS2, POS1, POS4, POS3 },
  };

  void  *ret;

  rnd = ( rnd & 0x0003e000 ) >> 13;

  GF_ASSERT_MSG( rnd <= ID_POKEPARADATA62, "POKETOOL_ppp_get_param_block:RND Index Over!" );
  GF_ASSERT_MSG( id <= ID_POKEPARA4, "POKETOOL_ppp_get_param_block:ID Index Over!" );

  ret = &ppp->paradata[ PokeParaAdrsTbl[ rnd ][ id ] ];

  return  ret;
}

//============================================================================================
/**
 * �|�P�����p�[�\�i������֐��n
 */
//============================================================================================

//----------------------------------------------------------------------------------
/**
 * ���[�J�����[�N�Ƀp�[�\�i���f�[�^�ǂݍ��݁i���O�Ɠ����f�[�^�Ȃ炻�̂܂܉������Ȃ��j
 *
 * @param   monsno
 * @param   formno
 *
 * @retval  POKEMON_PERSONAL_DATA*    �ǂݍ��ݐ惏�[�N�|�C���^
 */
//----------------------------------------------------------------------------------
static POKEMON_PERSONAL_DATA* Personal_Load( u16 monsno, u16 formno )
{
  if( (monsno != PersonalLatestMonsNo) || (formno != PersonalLatestFormNo) ){
    PersonalLatestFormNo = formno;
    PersonalLatestMonsNo = monsno;
    POKE_PERSONAL_LoadData( monsno, formno, &PersonalDataWork );
  }
  return &PersonalDataWork;
}

//----------------------------------------------------------------------------------
/**
 * �Ƃ����������J�E���g
 *
 * @param   ppd
 *
 * @retval  u8
 */
//----------------------------------------------------------------------------------
static u8 Personal_GetTokuseiCount( POKEMON_PERSONAL_DATA* ppd )
{
  if( POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi2 ) ){
    return 2;
  }
  return 1;
}

//============================================================================================
/**
 *  �|�P�����p�[�\�i���\���̃f�[�^����C�ӂŃf�[�^���擾
 *  �i�ꔭ�Ńf�[�^�����o�������Ƃ��Ɏg�p���Ă��������A
 *    �p�[�\�i���f�[�^������������o�������Ƃ���Open,Get,Close���������y���ł��j
 *
 * @param[in] mons_no �擾�������|�P�����i���o�[
 * @param[in] form_no �擾�������|�P�����̃t�H�����i���o�[
 * @param[in] param �擾�����f�[�^�̃C���f�b�N�X�ipoke_tool.h�ɒ�`�j
 *
 * @return  �擾�����f�[�^
 */
//============================================================================================
u32 POKETOOL_GetPersonalParam( u16 mons_no, u16 form_no, PokePersonalParamID param )
{
  POKEMON_PERSONAL_DATA* ppd = Personal_Load( mons_no, form_no );
  return POKE_PERSONAL_GetParam( ppd, param );
}

//==================================================================
/**
 * �w�肵�����x���Ƀ|�P�����p�����[�^��␳���܂�
 *
 * @param   pp      �Ώۂ̃|�P�����ւ̃|�C���^
 * @param   level   ���x��
 */
//==================================================================
void POKETOOL_MakeLevelRevise(POKEMON_PARAM *pp, u32 level)
{
  u32 level_exp;
  u32 monsno, form_no;

  monsno = PP_Get(pp, ID_PARA_monsno, NULL);
  form_no = PP_Get(pp, ID_PARA_form_no, NULL);

  level_exp = POKETOOL_GetMinExp( monsno, form_no, level );
  PP_Put(pp, ID_PARA_exp, level_exp);
  PP_Renew( pp );
}

//==================================================================
/**
 * �|�P�����p�����[�^�\���̂��R�s�[
 *
 * @param   src �R�s�[��
 * @param   dst �R�s�[��
 */
//==================================================================
void POKETOOL_CopyPPtoPP( POKEMON_PARAM* pp_src, POKEMON_PARAM* pp_dst )
{
  GF_ASSERT( pp_src != NULL );
  GF_ASSERT( pp_dst != NULL );
  *pp_dst = *pp_src;
}

//==================================================================
/**
 * �A�C�e���i���o�[����^�C�v���擾
 *
 * @param   item �A�C�e���i���o�[
 *
 * @retval  PokeType
 */
//==================================================================
PokeType  POKETOOL_GetPokeTypeFromItem( u16 item )
{ 
  return get_type_from_item( item );
}
