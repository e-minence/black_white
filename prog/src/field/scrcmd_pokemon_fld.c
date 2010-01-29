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
#include "pm_define.h"  //TEMOTI_POKEMAX

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
 * @brief ���C�ɓ���|�P�����̃Z�b�g
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSetFavoritePoke( VMHANDLE * core, void *wk )
{

  u16 pos = SCRCMD_GetVMWorkValue( core, wk );  //�|�P�����̈ʒu
  POKEMON_PARAM * pp = SCRCMD_GetTemotiPP( wk, pos );

  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  MISC * misc = GAMEDATA_GetMiscWork( gamedata );
  u16 monsno = PP_Get( pp, ID_PARA_monsno, NULL );
  u16 form_no = PP_Get( pp, ID_PARA_form_no, NULL );
  u16 egg_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
  MISC_SetFavoriteMonsno( misc, monsno, form_no, egg_flag );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief
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
    POKEMON_PARAM * pp = SCRCMD_GetTemotiPP( wk, i );
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
 * @brief
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
 * @brief
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
  pp = SCRCMD_GetTemotiPP( wk, pos );

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
  { 150, msg_hyouka_i09 },
  {   0, msg_hyouka_i00 },  //Sentinel
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
  { 150, msg_hyouka_i19 },
  {   0, msg_hyouka_i10 },  //Sentinel
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
  { 634, msg_hyouka_z17 },
  {   0, msg_hyouka_z01 },  //Sentinel
};

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

  GAMEDATA *gamedata = SCRCMD_WORK_GetGameData( wk );
  ZUKAN_SAVEDATA * zw = GAMEDATA_GetZukanSave( gamedata );
  BOOL zenkoku_flag = ZUKANSAVE_GetZenkokuZukanFlag( zw );
  u32 count = 0;
  int i;
  const ZUKAN_HYOUKA_TABLE * tbl;

  *ret_msgid = msg_hyouka_i01; //�O�̂���
  *ret_count = 0;              //�O�̂���
  switch ( mode )
  {
  case SCR_ZUKAN_HYOUKA_MODE_AUTO:  //�p�\�R��
    if ( zenkoku_flag == FALSE )
    {
      //���ق�������i�S�����[�h�O�j�́u�������v
      count = ZUKANSAVE_GetLocalPokeSeeCount( zw );
      tbl = LocalSeeTable;
    }
    else
    {
      //���񂱂�������́u�߂܂������v
      count = ZUKANSAVE_GetZukanPokeGetCount( zw );
      tbl = GlobalGetTable;
    }
    break;

  case SCR_ZUKAN_HYOUKA_MODE_LOCAL: //���m�i���j
    if ( zenkoku_flag == TRUE )
    {
      //���ق�������i�S�����[�h��j�́u�߂܂������v
      count = ZUKANSAVE_GetLocalPokeGetCount( zw );
      tbl = LocalGetTable;
    }
    else
    {
      //���ق�������i�S�����[�h�O�j�́u�������v
      count = ZUKANSAVE_GetLocalPokeSeeCount( zw );
      tbl = LocalSeeTable;
    }
    break;

  case SCR_ZUKAN_HYOUKA_MODE_GLOBAL:  //���m�i���j
    if ( zenkoku_flag == TRUE )
    {
      //���񂱂�������́u�߂܂������v
      count = ZUKANSAVE_GetZukanPokeGetCount( zw );
      tbl = GlobalGetTable;
    }
    else
    {
      GF_ASSERT( 0 );
      return VMCMD_RESULT_CONTINUE;
    }
    break;

  default:
    GF_ASSERT( 0 );
    return VMCMD_RESULT_CONTINUE;
  }

  *ret_count = count;

  for ( i = 0; ; i ++ )
  {
    if ( tbl[i].range == 0 )
    { //�ԕ��F������e�[�u���ɂȂ��l�������ꍇ
      *ret_msgid = tbl[i].msg_id;
      break;
    }
    if ( count <= tbl[i].range )
    { //�͈͈ȉ��̏ꍇ�A���̑Ή����b�Z�[�WID��Ԃ�
      *ret_msgid = tbl[i].msg_id;
      break;
    }
  }
  
  return VMCMD_RESULT_CONTINUE;
}








