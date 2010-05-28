//======================================================================
/**
 * @file	scrcmd_pokemon_fld.c
 * @brief	�X�N���v�g�R�}���h�F�|�P�����֘A�F�t�B�[���h�ł����g�p���Ȃ�����
 * @date  2010.01.23
 * @author	tamada GAMEFREAK inc.
 *
 * �T�u�C�x���g�Ō���g�p�������̂ȂǁA�펞�Ăяo���Ȃ��Ă����v�Ȃ��̂�
 * scrcmd_pokemon.c�łȂ�������ɔz�u����
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_pokemon.h"
#include "scrcmd_pokemon_fld.h"

#include "savedata/misc.h"
#include "savedata/tradepoke_after_save.h"
#include "pm_define.h"  //TEMOTI_POKEMAX

#include "waza_tool/wazano_def.h" // for WAZANO_xxxx
#include "event_poke_status.h" // for EvCmdPartyPokeSelect

#include "event_name_input.h" // for EVENT_NameInput_PartyPoke
#include "event_egg_birth.h"  // for EVENT_EggBirth
#include "fld_trade.h"  // for EVENT_FieldPokeTrade

#include "item/item.h"  //ITEM_GetWazaNo

#include "app/pokelist.h" // PL_SP_WAZANO_�`


#include "fieldmap.h"
//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * �|�P������I������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPartyPokeSelect( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��2����
  u16               value = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����(�\���l)
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  
  {
    GMEVENT *event = EVENT_CreatePokeSelect( gsys , ret_decide , ret_wk );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief ���C�ɓ���|�P�����̃Z�b�g
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetFavoritePoke( VMHANDLE * core, void *wk )
{

  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //�|�P�����̈ʒu
  POKEMON_PARAM * pp;

  if ( SCRCMD_GetTemotiPP( wk, pos, &pp ) == TRUE )
  { //�G���[�Ώ�
    GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
    MISC * misc = GAMEDATA_GetMiscWork( gamedata );
    u16 monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    u16 form_no = PP_Get( pp, ID_PARA_form_no, NULL );
    u16 egg_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
    u16 sex = PP_Get( pp, ID_PARA_sex, NULL );

    MISC_SetFavoriteMonsno( misc, monsno, form_no, egg_flag, sex );
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �W�����[�_�[�폟�����̃Z�b�g
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetGymVictoryInfo( VMHANDLE * core, void *wk )
{
  int i;
  u16 monsnos[TEMOTI_POKEMAX];
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );

  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  for ( i = 0 ; i < TEMOTI_POKEMAX; i++ )
  {
    monsnos[i] = 0;
  }
  for (i = 0; i < PokeParty_GetPokeCount( party ); i++)
  {
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
    if ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == FALSE )
    {
      monsnos[i] = PP_Get( pp, ID_PARA_monsno, NULL);
    }
  }
  MISC_SetGymVictoryInfo( GAMEDATA_GetMiscWork(gamedata), badge_id, monsnos );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �W�����[�_�[�폟�����̎擾
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetGymVictoryInfo( VMHANDLE * core, void *wk )
{
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  SCRCMD_WORK*       work = wk;
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET*        wordset = SCRIPT_GetWordSet( sc );

  int i, count;
  u16 badge_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );  //���ʊi�[���[�N
  u16 monsnos[TEMOTI_POKEMAX];

  MISC_GetGymVictoryInfo( GAMEDATA_GetMiscWork(gamedata), badge_id, monsnos );
  for ( i = 0, count = 0; i < TEMOTI_POKEMAX; i++ )
  {
    if (monsnos[i] != 0)
    {
      WORDSET_RegisterPokeMonsNameNo( wordset, i, monsnos[i] );
      count ++;
    }
  }
  *ret_wk = count;
  return VMCMD_RESULT_CONTINUE;
}



//--------------------------------------------------------------
/**
 * @brief   �W���b�W�C�x���g�̔���f�[�^�擾�R�}���h
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetBreederJudgeResult( VMHANDLE * core, void *wk )
{
  u16 max_val;  //0�`31
  u16 total;
  u16 max_bits;
  u32 i;
  u16 temp[6];
  POKEMON_PARAM* pp;
  u16 pos         = SCRCMD_GetVMWorkValue( core, wk );    //�莝���̉��Ԗڂ��������Ă���
  u16 mode        = SCRCMD_GetVMWorkValue( core, wk );    //�擾�������l�̎w��
  u16* ret_wk    = SCRCMD_GetVMWork( core, wk );   //�p���[�����̍��v

  //�|�P�����ւ̃|�C���^�擾
  //
  if (SCRCMD_GetTemotiPP( wk, pos, &pp ) == FALSE )
  { //�G���[�Ώ�
    *ret_wk = 0;
    return VMCMD_RESULT_CONTINUE;
  }

  //�p���[�����擾
  temp[0] = PP_Get( pp, ID_PARA_hp_rnd, NULL );
  temp[1] = PP_Get( pp, ID_PARA_pow_rnd, NULL );
  temp[2] = PP_Get( pp, ID_PARA_def_rnd, NULL );
  temp[3] = PP_Get( pp, ID_PARA_agi_rnd, NULL );
  temp[4] = PP_Get( pp, ID_PARA_spepow_rnd, NULL );
  temp[5] = PP_Get( pp, ID_PARA_spedef_rnd, NULL );

  //�p���[�����̍��v�Ȃ�тɁA�ō��l�𒲂ׂ�
  total = 0;
  max_val = 0;
  for( i=0; i < 6 ;i++ )
  {
    total += temp[i];
    if (max_val < temp[i]){
      max_val = temp[i];
    }
  }
  switch ( mode )
  {
  case SCR_JUDGE_MODE_TOTAL:
    *ret_wk = total;
    break;
  case SCR_JUDGE_MODE_HIGH_VALUE:
    *ret_wk = max_val;
    break;
  case SCR_JUDGE_MODE_HP:
    *ret_wk = temp[0];
    break;
  case SCR_JUDGE_MODE_POW:
    *ret_wk = temp[1];
    break;
  case SCR_JUDGE_MODE_DEF:
    *ret_wk = temp[2];
    break;
  case SCR_JUDGE_MODE_AGI:
    *ret_wk = temp[3];
    break;
  case SCR_JUDGE_MODE_SPPOW:
    *ret_wk = temp[4];
    break;
  case SCR_JUDGE_MODE_SPDEF:
    *ret_wk = temp[5];
    break;
  default:
    GF_ASSERT_MSG( 0, "���Ή���SCR_JUDGE_MODE(%d)�ł�\n", mode );
  }

  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//
//
//
//======================================================================
#include "msg/script/msg_hyouka_scr.h"

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct{
  u16 range;
  u16 msg_id;
} ZUKAN_HYOUKA_TABLE;

static const ZUKAN_HYOUKA_TABLE LocalSeeTable[] = {
  {  14, msg_hyouka_i00 },
  {  24, msg_hyouka_i01 },
  {  39, msg_hyouka_i02 },
  {  59, msg_hyouka_i03 },
  {  89, msg_hyouka_i04 },
  { 114, msg_hyouka_i05 },
  { 134, msg_hyouka_i06 },
  { 144, msg_hyouka_i07 },
  { 149, msg_hyouka_i08 },
  {   0, msg_hyouka_i08 },  //Sentinel
};

static const ZUKAN_HYOUKA_TABLE LocalGetTable[] = {
  {  29, msg_hyouka_i10 },
  {  49, msg_hyouka_i11 },
  {  69, msg_hyouka_i12 },
  {  89, msg_hyouka_i13 },
  { 104, msg_hyouka_i14 },
  { 119, msg_hyouka_i15 },
  { 134, msg_hyouka_i16 },
  { 144, msg_hyouka_i17 },
  { 149, msg_hyouka_i18 },
  {   0, msg_hyouka_i18 },  //Sentinel
};

static const ZUKAN_HYOUKA_TABLE GlobalGetTable[] = {
  {  39, msg_hyouka_z01 },
  {  79, msg_hyouka_z02 },
  { 119, msg_hyouka_z03 },
  { 159, msg_hyouka_z04 },
  { 199, msg_hyouka_z05 },
  { 249, msg_hyouka_z06 },
  { 299, msg_hyouka_z07 },
  { 349, msg_hyouka_z08 },
  { 399, msg_hyouka_z09 },
  { 449, msg_hyouka_z10 },
  { 499, msg_hyouka_z11 },
  { 549, msg_hyouka_z12 },
  { 599, msg_hyouka_z13 },
  { 614, msg_hyouka_z14 },
  { 629, msg_hyouka_z15 },
  { 633, msg_hyouka_z16 },
  {   0, msg_hyouka_z16 },  //Sentinel
};

//--------------------------------------------------------------
//--------------------------------------------------------------
static get_msgid( const ZUKAN_HYOUKA_TABLE * tbl, int count )
{
  int i;
  for ( i = 0; ; i ++ )
  {
    if ( tbl[i].range == 0 )
    { //�ԕ��F������e�[�u���ɂȂ��l�������ꍇ
      return tbl[i].msg_id;
    }
    if ( count <= tbl[i].range )
    { //�͈͈ȉ��̏ꍇ�A���̑Ή����b�Z�[�WID��Ԃ�
      return tbl[i].msg_id;
    }
  }
  GF_ASSERT( 0 );
  return tbl[0].msg_id;  //�O�̂���
}
//--------------------------------------------------------------
/**
 * @brief ������]��
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanHyouka( VMHANDLE * core, void *wk )
{
  u16     mode = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_msgid = SCRCMD_GetVMWork( core, wk );
  u16 * ret_count = SCRCMD_GetVMWork( core, wk );

  HEAPID     heap_id = SCRCMD_WORK_GetHeapID( wk );
  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  ZUKAN_SAVEDATA * zw = GAMEDATA_GetZukanSave( gamedata );
  BOOL zenkoku_flag = ZUKANSAVE_GetZenkokuZukanFlag( zw );
  u32 count = 0;
  int i;

  *ret_msgid = msg_hyouka_i01; //�O�̂���
  *ret_count = 0;              //�O�̂���
  switch ( mode )
  {
  case SCR_ZUKAN_HYOUKA_MODE_LOCAL_GET: //���m�i���j�߂܂���
    {
      //���ق�������i�S�����[�h��j�́u�߂܂������v
      count = ZUKANSAVE_GetLocalPokeGetCount( zw, heap_id );
      if ( ZUKANSAVE_CheckLocalGetComp( zw, heap_id ) == TRUE ) {
        *ret_msgid = msg_hyouka_i19;
      } else {
        *ret_msgid = get_msgid( LocalGetTable, count );
      }
    }
    break;

  case SCR_ZUKAN_HYOUKA_MODE_LOCAL_SEE: //���m�i���j����
    {
      //���ق�������i�S�����[�h�O�j�́u�������v
      count = ZUKANSAVE_GetLocalPokeSeeCount( zw, heap_id );
      if ( ZUKANSAVE_CheckLocalSeeComp( zw, heap_id ) == TRUE ) {
        *ret_msgid = msg_hyouka_i09;
      } else {
        *ret_msgid = get_msgid( LocalSeeTable, count );
      }
    }
    break;

  case SCR_ZUKAN_HYOUKA_MODE_GLOBAL:  //���m�i���j
    GF_ASSERT( zenkoku_flag == TRUE );
    {
      //���񂱂�������́u�߂܂������v
      count = ZUKANSAVE_GetZukanPokeGetCount( zw, heap_id );
      if ( ZUKANSAVE_CheckZenkokuComp( zw ) == TRUE ) {
        *ret_msgid = msg_hyouka_z17;
      } else {
        *ret_msgid = get_msgid( GlobalGetTable, count );
      }
    }
    break;

  default:
    GF_ASSERT( 0 );
    return VMCMD_RESULT_CONTINUE;
  }

  *ret_count = count;
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ������R���v���[�g����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetZukanComplete( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*        work = wk;
  GAMEDATA*       gamedata = SCRCMD_WORK_GetGameData( work );
  ZUKAN_SAVEDATA*       zw = GAMEDATA_GetZukanSave( gamedata );
  GAMESYS_WORK *      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  u16     mode = SCRCMD_GetVMWorkValue( core, wk );
  switch ( mode )
  {
  case SCR_ZUKAN_HYOUKA_MODE_GLOBAL:
    *ret_wk = ZUKANSAVE_CheckZenkokuComp( zw );
    break;
  case SCR_ZUKAN_HYOUKA_MODE_LOCAL_GET:
    *ret_wk = ZUKANSAVE_CheckLocalGetComp( zw, FIELDMAP_GetHeapID( fieldmap ) );
    break;

  case SCR_ZUKAN_HYOUKA_MODE_LOCAL_SEE:
    *ret_wk = ZUKANSAVE_CheckLocalSeeComp( zw, FIELDMAP_GetHeapID( fieldmap ) );
    break;

  default:
    GF_ASSERT_MSG( 0, "�Ή����Ă��Ȃ��]�����[�h\n" );
    break;
  }
  return VMCMD_RESULT_CONTINUE;
}
//======================================================================
//
//    �Z�o���֘A
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  SKILLTEACH_ID_COALESCENCE_KUSA = 0,
  SKILLTEACH_ID_COALESCENCE_HONOO,
  SKILLTEACH_ID_COALESCENCE_MIZU,

  SKILLTEACH_ID_STRONGEST_KUSA,
  SKILLTEACH_ID_STRONGEST_HONOO,
  SKILLTEACH_ID_STRONGEST_MIZU,

  SKILLTEACH_ID_DRAGON,

  SKILLTEACH_ID_MAX,
};

//--------------------------------------------------------------
/**
 * @brief ������Z�i���o�[���擾����
 * @param pp    �ΏۂƂȂ�|�P����
 * @param mode  �Z�o�����[�h�w��
 * @return  u16 �Z�i���o�[
 */
//--------------------------------------------------------------
static u16 get_teach_waza( const POKEMON_PARAM * pp, u16 mode )
{
  int i;
  u16 monsno;

  switch ( mode )
  {
  case SCR_SKILLTEACH_MODE_STRONGEST:
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_KUSA ) )
    {
      return WAZANO_HAADOPURANTO;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_HONOO ) )
    {
      return WAZANO_BURASUTOBAAN;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_MIZU ) )
    {
      return WAZANO_HAIDOROKANON;
    }
    GF_ASSERT(0);
    break;

  case SCR_SKILLTEACH_MODE_DRAGON:
    return WAZANO_RYUUSEIGUN;

  case SCR_SKILLTEACH_MODE_COALESCENCE:
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_KUSA ) )
    {
      return WAZANO_KUSANOTIKAI;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_HONOO ) )
    {
      return WAZANO_HONOONOTIKAI;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_MIZU ) )
    {
      return WAZANO_MIZUNOTIKAI;
    }
    GF_ASSERT(0);
    break;

  default:
    GF_ASSERT(0);
  }
  return WAZANO_BURASUTOBAAN;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 get_teach_limit_friendly( u16 mode )
{
  /* ����A���ׂẴ��[�h�ōő�l�K�v�Ƃ��Ă��� */
  return PTL_FRIEND_MAX;
}

//--------------------------------------------------------------
/**
 * @brief �Z���������邩�ǂ����̃`�F�b�N
 * @param pp    �ΏۂƂȂ�|�P����
 * @param mode  �Z�o�����[�h�w��
 * @return  BOOL  TRUE�̎�����������AFALSE�̎����������Ȃ�
 */
//--------------------------------------------------------------
static BOOL check_teach_mons( const POKEMON_PARAM * pp, u16 mode )
{
  int i;

  switch ( mode )
  {
  case SCR_SKILLTEACH_MODE_STRONGEST:
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_KUSA ) )
    {
      return TRUE;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_HONOO ) )
    {
      return TRUE;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_STRONGEST_MIZU ) )
    {
      return TRUE;
    }
    return FALSE;

  case SCR_SKILLTEACH_MODE_DRAGON:
#if 0
    {
      u16 type1 = PP_Get( pp, ID_PARA_type1, NULL );
      u16 type2 = PP_Get( pp, ID_PARA_type2, NULL );
      if ( type1 == POKETYPE_DRAGON && type2 == POKETYPE_DRAGON )
      {
        return TRUE;
      }
    }
#else
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_DRAGON ) )
    {
      return TRUE;
    }
#endif
    return FALSE;

  case SCR_SKILLTEACH_MODE_COALESCENCE:
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_KUSA ) )
    {
      return TRUE;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_HONOO ) )
    {
      return TRUE;
    }
    if ( PP_CheckWazaOshie( pp, SKILLTEACH_ID_COALESCENCE_MIZU ) )
    {
      return TRUE;
    }
    return FALSE;
  }
  GF_ASSERT(0);
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief �莝���ɋZ�������\���ǂ����̔���
 * @param core
 * @param wk
 * @param mode    �Z�������[�h�w��
 * @param oboeBit �r�b�g�P�ʂłǂ̃|�P�����ɋ������邩�H��Ԃ����߂̃��[�N
 * @param natsuki_check �Ȃ��x���݂邩�ǂ����̎w��iTRUE==����j
 *
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_OK  ��������
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG  �Ώۃ|�P���������Ȃ�
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG  �Ȃ�������Ȃ�
 */
//--------------------------------------------------------------
static u32 checkSkillTeach( VMHANDLE * core, void * wk, u16 mode, u8 * oboeBit, BOOL natsuki_check )
{
  GAMEDATA*           gdata = SCRCMD_WORK_GetGameData( wk );
  POKEPARTY*          party = GAMEDATA_GetMyPokemon( gdata );
  int                   max = PokeParty_GetPokeCount( party );
  BOOL     has_pokemon_flag = FALSE;
  BOOL  enough_natsuki_flag = FALSE;

  int pos;

  * oboeBit = 0;
  for ( pos = 0; pos < max; pos++ )
  {
    POKEMON_PARAM* pp = PokeParty_GetMemberPointer( party, pos );
    if ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == TRUE)
    {
      continue;
    }
    if ( check_teach_mons( pp, mode ) == FALSE )
    {
      continue;
    }
    has_pokemon_flag = TRUE;
    if ( natsuki_check == FALSE || PP_Get( pp, ID_PARA_friend, NULL ) >= get_teach_limit_friendly( mode ) )
    {
      enough_natsuki_flag = TRUE;
      *oboeBit |= ( 1 << pos );
    }
  }

  if ( has_pokemon_flag == FALSE )
  {
    return SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG;
  }
  if ( enough_natsuki_flag == FALSE )
  {
    return SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG;
  }
  return SCR_SKILLTEACH_CHECK_RESULT_OK;
}

//--------------------------------------------------------------
/**
 * @brief �莝���ɋZ��������Ώۂ����邩�H�̃`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSkillTeachCheckParty( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );
  u8 oboeBit;

  *ret_wk = checkSkillTeach( core, wk, mode, &oboeBit, TRUE );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ������ZID���擾����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSkillTeachGetWazaID( VMHANDLE * core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  u16 pos = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );
  POKEMON_PARAM * pp;

  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == TRUE )
  {
    *ret_wk = get_teach_waza( pp, mode );
  }
  else
  {
    *ret_wk = 0;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief 
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSkillTeachCheckPokemon( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  u16 pos = SCRCMD_GetVMWorkValue( core, work );
  u16 * ret_wk = SCRCMD_GetVMWork( core, work );
  POKEMON_PARAM * pp;
  
  *ret_wk = SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG; //�G���[�Ώ��p

  if ( SCRCMD_GetTemotiPP( work, pos, &pp ) == FALSE )
  { //�G���[�Ώ�
    return VMCMD_RESULT_CONTINUE;
  }

  if ( check_teach_mons( pp, mode ) == FALSE )
  {
    //�ΏۂƂȂ�|�P�����łȂ�
    *ret_wk = SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG;
  }
  else if ( SCRCMD_CheckTemotiWaza( pp, get_teach_waza( pp, mode ) ) == TRUE )
  {
    //���łɊo���Ă���
    *ret_wk = SCR_SKILLTEACH_CHECK_RESULT_ALREADY_NG;
  }
  else if ( PP_Get( pp, ID_PARA_friend, NULL ) < get_teach_limit_friendly( mode ) )
  {
    //�Ȃ�������Ȃ�
    *ret_wk = SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG;
  }
  else
  {
    //�����邱�Ƃ��ł���I
    *ret_wk = SCR_SKILLTEACH_CHECK_RESULT_OK;
  }

  return  VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�����Ώۂ̃|�P������I������
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSkillTeachSelectPokemon( VMHANDLE * core, void * wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16                mode = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // �R�}���h��2����
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��3����
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );

  u8 learnBit;
  if ( checkSkillTeach( core, wk, mode, &learnBit, FALSE ) != SCR_SKILLTEACH_CHECK_RESULT_OK )
  {
    *ret_decide = FALSE;
  }
  else
  {
    u16 wazano;
    GMEVENT * event;
    switch ( mode )
    {
    case SCR_SKILLTEACH_MODE_STRONGEST:
      wazano = PL_SP_WAZANO_STRONGEST;
      break;
    case SCR_SKILLTEACH_MODE_COALESCENCE:
      wazano = PL_SP_WAZANO_COALESCENCE;
      break;
    case SCR_SKILLTEACH_MODE_DRAGON:
      wazano = WAZANO_RYUUSEIGUN;
      break;
    default:
      GF_ASSERT( 0 );
      wazano = WAZANO_WARUAGAKI;
      break;
    }
    event = EVENT_CreatePokeSelectWazaOboe( gsys , ret_decide , ret_wk, learnBit, wazano );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//
//
//    ����C�x���g�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �莝���Ƀ|�P���X��Ԃ̃|�P���������邩�H�̃`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckTemotiPokerus( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  *ret_wk = FALSE;

  {
    GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u32 max = PokeParty_GetPokeCount( party );
    int idx;

    for (idx = 0; idx < max; idx ++)
    {
      u32 result;
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, idx );
      result = PP_Get( pp, ID_PARA_pokerus, NULL );
      if (result != 0)
      {
        *ret_wk = TRUE;
        break;
      }
    }
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �u�߂��߂�p���[�v�̃^�C�v���擾����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 * �߂��߂�p���[���킴�}�V���P�O
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetMezameruPowerType( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );  //���ʊi�[���[�N
  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //�|�P�����̈ʒu
  POKEMON_PARAM * pp;
  u8 machine_no;

  //�킴�}�V���P�O���߂��߂�p���[����Ȃ��Ȃ�����Assert
  GF_ASSERT( ITEM_GetWazaNo( ITEM_WAZAMASIN10 ) == WAZANO_MEZAMERUPAWAA );

  machine_no = ITEM_GetWazaMashineNo( ITEM_WAZAMASIN10 );
  if ( SCRCMD_GetTemotiPP( wk, pos, &pp ) == FALSE )
  { //�G���[�Ώ�
    *ret_wk = 0xffff;
  }
  else if (PP_CheckWazaMachine( pp, machine_no ) == FALSE)
  {
    *ret_wk = 0xffff;
  }
  else
  {
    *ret_wk = POKETOOL_GetMezaPa_Type( pp );
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �莝���|�P�����̖��O���͉�ʂ��Ăяo��
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdPartyPokeNameInput( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16*        ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16          index = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����
  u16      fade_flag = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����

  // �C�x���g���Ăяo��
  SCRIPT_CallEvent( scw, EVENT_NameInput_PartyPoke(gsys, ret_wk, index, fade_flag) );
  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * �Z��I������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPartyPokeWazaSelect( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*         ret_decide = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��2����
  u16             pokePos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��3����
  u16               value = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��4���� ��`�I���\(1�� 0�s��)
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  {
    BOOL canSelHiden = ( value == 0 );
    GMEVENT *event = EVENT_CreateWazaSelect( gsys , fieldmap , pokePos , ret_decide , ret_wk , canSelHiden, value );
    SCRIPT_CallEvent( scw, event );
  }
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �莝���̃^�}�S��z��������
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
extern VMCMD_RESULT EvCmdPartyPokeEggBirth( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h��1����
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS*            my = GAMEDATA_GetMyStatus( gdata );
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  int          poke_count = PokeParty_GetPokeCount( party );


  for( i=0; i<poke_count; i++ )
  {
    POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, i );
    u32     tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    u32     fusei_tamago_flag = PP_Get( param, ID_PARA_fusei_tamago_flag, NULL );

    if( tamago_flag && !fusei_tamago_flag )
    {
      // �z���C�x���g
      SCRIPT_CallEvent( scw, EVENT_EggBirth( gsys, fieldmap, param ) );
      *ret_wk = i;
      return VMCMD_RESULT_SUSPEND;
    }
  }
  return VMCMD_RESULT_CONTINUE; 
}

//--------------------------------------------------------------
/**
 * @brief �Q�[�����������\���ǂ������`�F�b�N����
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldPokeTradeCheck( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  u16*             ret_wk = SCRCMD_GetVMWork( core, work );       // �R�}���h������
  u16            trade_no = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������
  u16           party_pos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��O����
  POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM*     poke = PokeParty_GetMemberPointer( party, party_pos );
  FLD_TRADE_WORK*   trade = FLD_TRADE_WORK_Create( heap_id, trade_no );
 
  // �S�`�F�b�N���p�X����Ό����\
  *ret_wk = FLD_TRADE_ENABLE;

  // �^�}�S�`�F�b�N
  if( *ret_wk == FLD_TRADE_ENABLE )
  {
    u32 tamago_flag = PP_Get( poke, ID_PARA_tamago_flag, NULL );
    if( tamago_flag == TRUE )
    {
      *ret_wk = FLD_TRADE_DISABLE_EGG;
    }
  }
  // �����X�^�[No.�`�F�b�N
  if( *ret_wk == FLD_TRADE_ENABLE )
  {
    u32 monsno = PP_Get( poke, ID_PARA_monsno, NULL );
    if( monsno != FLD_TRADE_WORK_GetChangeMonsno(trade) )
    {
      *ret_wk = FLD_TRADE_DISABLE_MONSNO;
    }
  }
  // ���ʃ`�F�b�N
  if( *ret_wk == FLD_TRADE_ENABLE )
  {
    if( FLD_TRADE_WORK_GetChangeMonsSex(trade) != PM_NEUTRAL )
    {
      u32 sex = PP_Get( poke, ID_PARA_sex, NULL );
      if( sex != FLD_TRADE_WORK_GetChangeMonsSex(trade) )
      {
        *ret_wk = FLD_TRADE_DISABLE_SEX;
      }
    } 
  } 

  FLD_TRADE_WORK_Delete( trade );
  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * @brief �Q�[���������C�x���g���Ă�
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldPokeTrade( VMHANDLE *core, void *wk )
{
  int i;
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  u16       trade_no = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u16      party_pos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����
  
  {
    GMEVENT *event = EVENT_FieldPokeTrade( gsys, trade_no, party_pos );
    SCRIPT_CallEvent( scw, event );
  } 
  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �킴�}�V���Ŏ����Ă�����̂𒊑I����
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdWazaMachineLot( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK * work = wk;
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  int max = MYITEM_GetItemPocketNumber( myitem, ITEMPOCKET_WAZA );
  int pos = GFUser_GetPublicRand( max );

  if ( max == 0 )
  {
    *ret_wk = 0;
  }
  else
  {
    ITEM_ST * item_st = MYITEM_PosItemGet( myitem, ITEMPOCKET_WAZA, pos );
    *ret_wk = ITEM_GetWazaNo( item_st->id );
  }

  return VMCMD_RESULT_CONTINUE;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �������̌�p�@�|�P�������Z�[�u
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdTradeAfterSaveSet( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*        work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*       gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*           gdata = GAMESYSTEM_GetGameData( gsys );
  TRPOKE_AFTER_SAVE*  save = GAMEDATA_GetTrPokeAfterSaveData( gdata );
  POKEPARTY * party = GAMEDATA_GetMyPokemon( gdata );
  POKEMON_PARAM*     poke;
  u16     party_pos = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u16     trade_type = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����

  poke = PokeParty_GetMemberPointer( party, party_pos );

  GF_ASSERT( trade_type < TRPOKE_AFTER_SAVE_TYPE_MAX );

  // �����Ώۂ̏���ۑ�����B
  TRPOKE_AFTER_SV_SetData( save, trade_type, poke );

  return VMCMD_RESULT_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u����Ă���|�P�����������[�h�Z�b�g�ɐݒ肷��B
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdTradeAfterPokeNameSetWord( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*        work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*        scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*       gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*           gdata = GAMESYSTEM_GetGameData( gsys );
  TRPOKE_AFTER_SAVE*  save = GAMEDATA_GetTrPokeAfterSaveData( gdata );
  u16     trade_type = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  u16     word_idx = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��2����
  STRBUF* tmpStr;
  WORDSET* wordset;

  GF_ASSERT( trade_type < TRPOKE_AFTER_SAVE_TYPE_MAX );

  tmpStr = SCRIPT_GetMsgTempBuffer(scw);
  wordset = SCRIPT_GetWordSet(scw);

  // �f�[�^�L���`�F�b�N
  if( TRPOKE_AFTER_SV_IsData( save, trade_type ) )
  {
    // �|�P�����������[�h�Z�b�g�ɐݒ�
    GFL_STR_SetStringCode( tmpStr, TRPOKE_AFTER_SV_GetNickName( save, trade_type ) );
  }
  else
  {
    STRCODE str[] = { 0 };
    GF_ASSERT(0);
    str[0] = GFL_STR_GetEOMCode(); // ���I���
    // �f�[�^���Ȃ��B�_�~�[�̖��O��ݒ�B
    GFL_STR_SetStringCode( tmpStr, str );
  }

  WORDSET_RegisterWord( wordset, word_idx, tmpStr, 0, TRUE, PM_LANG );

  return VMCMD_RESULT_CONTINUE;
}



//==============================================================================================
//
//    �N���C�}�b�N�X�f���p�R�}���h
//
//==============================================================================================
static int searchPartyLegend( GAMEDATA * gamedata );
static void exchangeLegendPosition( SCRCMD_WORK * work );
static void getLegendFromBox( SCRCMD_WORK * work, u16 select_pos );
static BOOL searchLegendInBox( GAMEDATA * gamedata, u16 monsno, u16 * traynum, u16 * tray_pos );

//-----------------------------------------------------------------------------
/**
 * @brief   �N���C�}�b�N�X�f���p�R�}���h
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdCrimaxCommand( VMHANDLE * core, void * wk )
{
  u16     cmd_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 *   use_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( wk );

  switch ( cmd_id )
  {
  case SCR_CRIMAX_CMD_TEMOTI_CHECK:
    //�莝���ɂ��邩�̃`�F�b�N
    if (searchPartyLegend( gamedata ) < 0 ) {
      *use_wk = FALSE;
    } else {
      *use_wk = TRUE;
    }
    break;
  case SCR_CRIMAX_CMD_SORT:
    //�擪�Ɏ����Ă��邽�߁A�Ȃ�בւ���
    exchangeLegendPosition( wk );
    break;
  case SCR_CRIMAX_CMD_GET_FROM_BOX:
    //�{�b�N�X���玝���Ă���
    getLegendFromBox( wk, *use_wk );
    exchangeLegendPosition( wk );
    break;
  default:
    GF_ASSERT( 0 ); //���Ή��̃R�}���h
  }
  return VMCMD_RESULT_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @brief �{�b�N�X����`���|�P���������o���A�莝���Ɠ���ւ���
 */
//-----------------------------------------------------------------------------
static void getLegendFromBox( SCRCMD_WORK * work, u16 select_pos )
{
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gamedata );

  u16 traynum, tray_pos;

  GF_ASSERT( PokeParty_GetPokeCount( party ) > select_pos );

  /* ������ */
  {
    u16 monsno;
    if ( GET_VERSION() == VERSION_WHITE ) {
      monsno = MONSNO_651;
    } else {
      monsno = MONSNO_650;
    }
    if ( searchLegendInBox( gamedata, monsno, &traynum, &tray_pos ) == FALSE )
    {
      GF_ASSERT( 0 );
      return;
    }
  }

  
  {
    HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
    BOX_MANAGER * boxman = GAMEDATA_GetBoxManager( gamedata );
    POKEMON_PARAM * legend_pp;
    POKEMON_PARAM * temp_pp = GFL_HEAP_AllocClearMemory( heapID, POKETOOL_GetWorkSize() );

    //�㏑������莝���f�[�^���Ƃ��Ă���
    GFL_STD_MemCopy(
        PokeParty_GetMemberPointer( party, select_pos ), temp_pp, POKETOOL_GetWorkSize() );

    //�`���|�P�����Ŏ莝�����㏑��
    legend_pp = PP_CreateByPPP(
        BOXDAT_GetPokeDataAddress( boxman, traynum, tray_pos ), heapID );
    PokeParty_SetMemberData( party, select_pos, legend_pp );
    GFL_HEAP_FreeMemory( legend_pp );

    //�{�b�N�X�̓`���|�P�̏ꏊ���Ƃ��Ă������莝���f�[�^�ŏ㏑��
    BOXDAT_PutPokemonPos( boxman, traynum, tray_pos, PP_GetPPPPointer(temp_pp) );
    GFL_HEAP_FreeMemory( temp_pp );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �{�b�N�X����w��|�P������T��
 * @param work      
 * @param monsno    �T���|�P�����i���o�[
 * @param traynum   �g���C�w��ID
 * @param tray_pos  �g���C���̃C���f�b�N�X
 * @return  BOOL  TRUE�̂Ƃ��A��������
 *
 * ������ID�����A�w��|�P������T��
 */
//-----------------------------------------------------------------------------
static BOOL searchLegendInBox( GAMEDATA * gamedata, u16 monsno, u16 * traynum, u16 * tray_pos )
{
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gamedata );
  MYSTATUS*  status = GAMEDATA_GetMyStatus( gamedata );
  BOX_MANAGER * boxman = GAMEDATA_GetBoxManager( gamedata );
  int current_tray = BOXDAT_GetCureentTrayNumber( boxman );
  int tray_idx, poke_pos;
  u32 my_id = MyStatus_GetID( status );

  for ( tray_idx = 0; tray_idx < BOX_MAX_TRAY; tray_idx ++ )
  {
    int tray = ( tray_idx + current_tray ) % BOX_MAX_TRAY;
    for ( poke_pos = 0; poke_pos < BOX_MAX_POS; poke_pos ++ )
    {
      if ( BOXDAT_PokeParaGet( boxman, tray, poke_pos, ID_PARA_poke_exist, NULL )
          && BOXDAT_PokeParaGet( boxman, tray, poke_pos, ID_PARA_id_no, NULL ) == my_id
          && BOXDAT_PokeParaGet( boxman, tray, poke_pos, ID_PARA_monsno, NULL ) == monsno
          && BOXDAT_PokeParaGet( boxman, tray, poke_pos, ID_PARA_tamago_flag, NULL ) == FALSE
          )
      {
        *traynum = tray;
        *tray_pos = poke_pos;
        return TRUE;
      }
    }
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 * @brief   �莝���̓`���|�P������擪�Ɏ����Ă���
 */
//-----------------------------------------------------------------------------
static void exchangeLegendPosition( SCRCMD_WORK * work )
{
  GAMEDATA* gamedata = SCRCMD_WORK_GetGameData( work );
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gamedata );
  int idx;
  idx = searchPartyLegend( gamedata );
  if ( idx >= 0 ) {
    PokeParty_ExchangePosition( party, idx, 0, SCRCMD_WORK_GetHeapID( work ) );
  } else {
    GF_ASSERT( 0 );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief �莝���̓`���|�P������T��
 * @retval  0-5 ���������ʒu
 * @retval  -1  ������Ȃ�����
 */
//-----------------------------------------------------------------------------
static int searchPartyLegend( GAMEDATA * gamedata )
{
  POKEPARTY*  party = GAMEDATA_GetMyPokemon( gamedata );
  MYSTATUS*  status = GAMEDATA_GetMyStatus( gamedata );
  int idx, max;
  u32 my_id;
  u16 monsno;

  if ( GET_VERSION() == VERSION_WHITE ) {
    monsno = MONSNO_651;
  } else {
    monsno = MONSNO_650;
  }

  max = PokeParty_GetPokeCount( party );
  my_id = MyStatus_GetID( status );

  for ( idx = 0; idx < max; idx++ )
  {
    POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, idx );
    if ( PP_Get( pp, ID_PARA_id_no, NULL ) == my_id
        && PP_Get( pp, ID_PARA_monsno, NULL ) == monsno
        && PP_Get( pp, ID_PARA_tamago_flag, NULL ) == FALSE )
    {
      return idx;
    }
  }
  return -1;
}







