//======================================================================
/**
 * @file bsubway_tr.c
 * @brief �o�g���T�u�E�F�C�@�g���[�i�[�֘A
 * @author  Miyuki Iwasawa
 * @date  06.04.17
 * @note GS���ڐA kaga
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "savedata/battle_rec.h"

#include "gamesystem/btl_setup.h"
#include "poke_tool/poke_tool.h"
#include "tr_tool/tr_tool.h"
#include "battle/battle.h"
#include "pm_define.h"
#include "buflen.h"
#include "sound/pm_sndsys.h"

#include "fldmmdl.h"
#include "field_encount.h"
#include "message.naix"

#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "savedata/bsubway_savedata.h"

//#include "../mystery/fntequ.h"

#define __BSUBWAY_FLD_H_GLOBAL
#include "bsubway_tr.h"
#include "event_bsubway.h"

//�����f�[�^���o����܂ŃC���N���[�h
#include "item/itemsym.h"
#include "poke_tool/monsno_def.h"
#include "waza_tool/wazano_def.h"
//#include "battle/battle_common.h"
#include "tr_tool/trtype_def.h"
#include "battle/bsubway_battle_data.h"
#include "tr_tool/trno_def.h"
//#include "b_tower_fld.dat"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static BOOL set_BSWayPokemonParam(
    BSUBWAY_SCRWORK* wk,BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no,BSUBWAY_POKEMON *pwd,u8 cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID );

static BSUBWAY_TRAINER_ROM_DATA * get_TrainerRomData(
    u16 tr_no, HEAPID heapID );

static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index);
static void  make_TrainerData(
    BATTLE_SETUP_PARAM *bp, BSUBWAY_PARTNER_DATA *tr_data,
    int cnt, BTL_CLIENT_ID client_no, HEAPID heapID );

static STRCODE * PM_strcpy( STRCODE* to_str, const STRCODE* from_str );
static u16 get_Rand( BSUBWAY_SCRWORK *wk );

//======================================================================
//  �g���[�i�[�^�C�v
//======================================================================
//--------------------------------------------------------------
/// �^���[�ɏo������g���[�i�[�^�C�v����OBJ�R�[�h
//--------------------------------------------------------------
static const u16 btower_trtype2objcode[][2] =
{
 {TRTYPE_TANPAN,  BOY2},  ///<����p��������
 {TRTYPE_MINI,  GIRL1},  ///<�~�j�X�J�[�g
 {TRTYPE_SCHOOLB,  BOY1},  ///<���キ������
 {TRTYPE_SCHOOLG, GIRL3 },  ///<���キ������
 {TRTYPE_PRINCE,  BOY4},  ///<���ڂ������
 {TRTYPE_PRINCESS,  GIRL4},  ///<�����傤����
 {TRTYPE_KINDERGARTENM, BABYBOY2},    //���񂶁�
 {TRTYPE_KINDERGARTENW, BABYGIRL2},    //���񂶁�
 {TRTYPE_BACKPACKERM, BACKPACKERM},//�o�b�N�p�b�J�[��
 {TRTYPE_BACKPACKERW, BACKPACKERW},//�o�b�N�p�b�J�[��
 {TRTYPE_WAITER,WAITER}, //�E�G�[�^�[
 {TRTYPE_WAITRESS,WAITRESS}, //�E�G�[�g���X
 {TRTYPE_DAISUKIM,  MIDDLEMAN1},  ///<���������N���u
 {TRTYPE_DAISUKIW,  MIDDLEWOMAN1},  ///<���������N���u
 {TRTYPE_DOCTOR, DOCTOR },   //�h�N�^�[
 {TRTYPE_NURSE,  NURSE},    //�i�[�X
 {TRTYPE_CYCLINGM,  CYCLEM},  ///<�T�C�N�����O��
 {TRTYPE_CYCLINGW,  CYCLEW},  ///<�T�C�N�����O��
 {TRTYPE_GENTLE,  GENTLEMAN},  ///<�W�F���g���}��
 {TRTYPE_MADAM,  LADY},  ///<�}�_��
 {TRTYPE_BREEDERM,  MAN1},  ///<�|�P�����u���[�_�[
 {TRTYPE_BREEDERW,  WOMAN1},  ///<�|�P�����u���[�_�[
 {TRTYPE_SCIENTISTM,  ASSISTANTM},  //���񂫂イ����
 {TRTYPE_SCIENTISTW,  ASSISTANTW},  //���񂫂イ����
 {TRTYPE_ESPM,  ESPM},  ///<�T�C�L�b�J�[
 {TRTYPE_ESPW,  ESPW},  ///<�T�C�L�b�J�[
 {TRTYPE_KARATE,  FIGHTERM},  //����Ă���
 {TRTYPE_BATTLEG,  GIRL2},  ///<�o�g���K�[��
 {TRTYPE_RANGERM,  MAN3},  ///<�|�P���������W���[
 {TRTYPE_RANGERW,  WOMAN3},  ///<�|�P���������W���[
 {TRTYPE_ELITEM,  MAN3},  ///<�G���[�g�g���[�i�[
 {TRTYPE_ELITEW,  WOMAN3},  ///<�G���[�g�g���[�i�[
 {TRTYPE_VETERANM,  OLDMAN1},  ///<�x�e�����g���[�i�[
 {TRTYPE_VETERANW,  OLDWOMAN1},  ///<�x�e�����g���[�i�[
 {TRTYPE_FISHING,  FISHING},  ///<��т�
 {TRTYPE_MOUNT,  MAN1},  ///<��܂��Ƃ�
 {TRTYPE_WORKER1,  WORKMAN},  ///<�����傤����
 {TRTYPE_WORKER2,  WORKMAN},  ///<�����傤����
 {TRTYPE_JUGGLING, CLOWN },   //�N���E��
 {TRTYPE_ARTIST,  OLDMAN1},  ///<���������
 {TRTYPE_POLICE,  POLICEMAN},  ///<���܂�肳��
 {TRTYPE_HEADS,  BADMAN},  ///<�X�L���w�b�Y
 {TRTYPE_BADRIDER, BADRIDER},   //�ڂ���������
 {TRTYPE_CLEANING, CLEANINGM},   //������������
 {TRTYPE_RAIL,RAILMAN }, //�Ăǂ�����
 {TRTYPE_PILOT, PILOT},   //�p�C���b�g
 {TRTYPE_BUSINESS1, BUSINESSMAN},    //�r�W�l�X�}��1
 {TRTYPE_BUSINESS2, BUSINESSMAN},    //�r�W�l�X�}��2
 {TRTYPE_PARASOL,  AMBRELLA},  ///<�p���\�����˂�����
 {TRTYPE_BAKER,     BAKER },   //�x�[�J���[
 {TRTYPE_CHILDCARE, WOMAN3}, //�ق�����
 {TRTYPE_MAID, MAID},    //���C�h
 {TRTYPE_OL, OL},    //�n�k
};

#define TRTYPE2OBJCODE_MAX  (NELEMS(btower_trtype2objcode))

//--------------------------------------------------------------
/**
 * @brief  �g���[�i�[�^�C�v����l��OBJ�R�[�h��Ԃ�
 * @param tr_type �g���[�i�[�^�C�v
 * @retval u16 OBJ�R�[�h
 */
//--------------------------------------------------------------
u16 BSUBWAY_GetTrainerOBJCode( u8 tr_type )
{
  int i;

  for( i = 0; i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == tr_type ){
      return btower_trtype2objcode[i][1];
    }
  }
  return BOY1;
}

//--------------------------------------------------------------
/**
 * @brief  �g���[�i�[�^�C�v���Ή����Ă��邩
 * @param tr_type �g���[�i�[�^�C�v
 * @retval BOOL TRUE=�Ή�
 */
//--------------------------------------------------------------
static BOOL check_TrainerType(u8 tr_type)
{
  int i;

  for( i = 0;i < TRTYPE2OBJCODE_MAX; i++ ){
    if( btower_trtype2objcode[i][0] == tr_type ){
      return( TRUE );
    }
  }
  return( FALSE );
}

//======================================================================
//  �o�g���T�u�E�F�C�֘A
//======================================================================
typedef struct
{
  u16  size;      ///< �z��T�C�Y
  u16  strlen;    ///< �����񒷁iEOM���܂܂Ȃ��j
  u32  magicNumber; ///< ����ɏ������ς݂ł��邱�Ƃ��`�F�b�N���邽�߂̃i���o�[
  STRCODE  buffer[1]; ///< �z��{��
}DEBUG_STRBUF;

//--------------------------------------------------------------
/**
 *  @brief �o�g���T�u�E�F�C�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
 *  @param src BSUBWAY_POKEMON
 *  @param dest POKEMON_PARAM
 *  @retval nothing
 */
//--------------------------------------------------------------
static void make_PokePara(
    const BSUBWAY_POKEMON *src, POKEMON_PARAM *dest )
{
  int i;
  u8 buf8,waza_pp;
  u16 buf16;
  u32 buf32;

  PP_Clear(dest);

  //monsno,level,pow_rnd,idno
  PP_SetupEx( dest, src->mons_no, 50, PTL_SETUP_ID_AUTO,
      (src->power_rnd & 0x3FFFFFFF), src->personal_rnd);

  //form_no
  buf8 = src->form_no;
  PP_Put( dest, ID_PARA_form_no, buf8 );

  //�����A�C�e���ݒ�
  PP_Put( dest,ID_PARA_item, src->item_no );

  //�Z�ݒ�
  for(i = 0;i < 4;i++){
    buf16 = src->waza[i];
    PP_Put(dest,ID_PARA_waza1+i,buf16);
    buf8 = (src->pp_count >> (i*2))&0x03;
    PP_Put(dest,ID_PARA_pp_count1+i, buf8);

    //pp�Đݒ�
    waza_pp = (u8)PP_Get(dest,ID_PARA_pp_max1+i,NULL);
    PP_Put(dest,ID_PARA_pp1+i,waza_pp);
  }

  //ID�ݒ�
  buf32 = src->id_no;
  PP_Put(dest,ID_PARA_id_no,buf32);

  //�o���l�ݒ�
  buf8 = src->hp_exp;
  PP_Put(dest,ID_PARA_hp_exp,buf8);
  buf8 = src->pow_exp;
  PP_Put(dest,ID_PARA_pow_exp,buf8);
  buf8 = src->def_exp;
  PP_Put(dest,ID_PARA_def_exp,buf8);
  buf8 = src->agi_exp;
  PP_Put(dest,ID_PARA_agi_exp,buf8);
  buf8 = src->spepow_exp;
  PP_Put(dest,ID_PARA_spepow_exp,buf8);
  buf8 = src->spedef_exp;
  PP_Put(dest,ID_PARA_spedef_exp,buf8);

  //�����ݒ�
  PP_Put(dest,ID_PARA_speabino, src->tokusei);

  //�Ȃ��ǐݒ�
  PP_Put(dest,ID_PARA_friend,src->natuki);

  { //���O�ݒ�
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        MONS_NAME_SIZE+EOM_SIZE, HEAPID_PROC );
    
#if 0
      GFL_STR_SetStringCode( nick_name, src->nickname );
#else
    {
      int i;
      u16 eomCode = GFL_STR_GetEOMCode();
      STRCODE code[MONS_NAME_SIZE+EOM_SIZE];
      const STRCODE *sz = src->nickname;
      
      for( i = 0; i < (MONS_NAME_SIZE+EOM_SIZE); i++ ){
        code[i] = sz[i];
      }
      
      code[i-1] = eomCode;
      GFL_STR_SetStringCode( nick_name, code );
    }
    
    /*
    {
      u16 eomCode = GFL_STR_GetEOMCode();
      DEBUG_STRBUF *d_strbuf = (DEBUG_STRBUF*)nick_name;
      const STRCODE *sz = src->nickname;
      
      OS_TPrintf( " SetStringCode ... " );
      d_strbuf->strlen = 0;
      
      while( *sz != eomCode )
      {
        if( d_strbuf->strlen >= (d_strbuf->size-1) )
        {
          GF_ASSERT_MSG( 0, "STRBUF overflow: busize=%d, EOMCode=%04x",
              d_strbuf->size, eomCode );
          break;
        }
        
        OS_TPrintf("%04x,", *sz);
        d_strbuf->buffer[ d_strbuf->strlen++ ] = *sz++;
      }
      
      OS_TPrintf( "\n" );
      
      d_strbuf->buffer[d_strbuf->strlen] = eomCode;
    }
    */
#endif

    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }

  PP_Put(dest,ID_PARA_country_code,src->country_code); //�J���g���[�R�[�h

  //�p�����[�^�Čv�Z
  {
    u32 lv = PP_CalcLevel( dest );
    PP_Put( dest, ID_PARA_level,lv );
  }
}

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�v���C���[�h����FIGHT_TYPE��Ԃ�
 * @param play_mode �v���C���[�h
 * @retval u32 FIGHT_TYPE
 */
//--------------------------------------------------------------
#if 0
static u32 get_FightType(u8 play_mode)
{
  switch(play_mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
//    return FIGHT_TYPE_BATTLE_TOWER_1vs1;
    return BTL_RULE_SINGLE;
  case BSWAY_MODE_DOUBLE:
//  return FIGHT_TYPE_BATTLE_TOWER_2vs2;
    return BTL_RULE_DOUBLE;
#if 0 //wb null
  case BSWAY_MODE_MULTI:
    return FIGHT_TYPE_BATTLE_TOWER_AI_MULTI;
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    return FIGHT_TYPE_BATTLE_TOWER_SIO_MULTI;
#endif
  }
  return BTL_RULE_SINGLE;
}
#endif

//--------------------------------------------------------------
/**
 * @brief  �o�g���T�u�E�F�C�@�g���[�i�[�f�[�^����
 * @param heapID HEAPID
 * @retval BSP_TRAINER_DATA*
 */
//--------------------------------------------------------------
static BSP_TRAINER_DATA * create_BSP_TRAINER_DATA( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );
  return tr_data;
}

//--------------------------------------------------------------
/**
 * BATTLE_SETUP_PARAM�쐬
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param gsys GAMESYS_WORK
 * @retval BATTLE_SETUP_PARAM
 */
//--------------------------------------------------------------
BATTLE_SETUP_PARAM * BSUBWAY_SCRWORK_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  int i;
  BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  BTL_CLIENT_ID client;
  POKEPARTY *party;
  POKEMON_PARAM *pp;
  POKEMON_PARAM *entry_pp;
  BSP_TRAINER_DATA *tr_data;
  BSUBWAY_PARTNER_DATA *bsw_partner;
  BSUBWAY_TRAINER *bsw_trainer;
  
  HEAPID heapID = HEAPID_PROC;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  u16 play_mode = wk->play_mode;
  
  BTL_FIELD_SITUATION_SetFromFieldStatus(
      &sit, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
  
  dst = BATTLE_PARAM_Create( heapID );
  
  //������
  switch( play_mode ){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_WIFI:
    BTL_SETUP_Single_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
    BTL_SETUP_Double_Trainer( dst, gdata, &sit, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
    BTL_SETUP_AIMulti_Trainer( dst, gdata, &sit,
        TRID_NULL, TRID_NULL, TRID_NULL, heapID );
    break;
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    {
      GFL_NETHANDLE *netHandle = GFL_NET_HANDLE_GetCurrentHandle();
      int commPos = 0;
      
      if( GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE ){
        commPos = 2; //�ʐM�q�ł���
      }
      
      BTL_SETUP_AIMulti_Comm( dst, gdata,
          netHandle, BTL_COMM_DS, commPos,
          TRID_NULL, TRID_NULL, heapID );
    }
    break;
  default:
    GF_ASSERT( 0 );
  }
  
  BTL_SETUP_AllocRecBuffer( dst, heapID );
  
  { //�v���C���[�ݒ�
    const POKEPARTY *myparty = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
    client = BTL_CLIENT_PLAYER;
    
    //MyStatus
    dst->playerStatus[client] = mystatus;
    
    //�g���[�i�[�f�[�^
    tr_data = dst->tr_data[client];
    
    MyStatus_CopyNameString( mystatus, tr_data->name );
    tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( mystatus );
    
    //�|�P�����p�[�e�B
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      pp = PokeParty_GetMemberPointer( myparty, wk->member[i] );
      POKETOOL_CopyPPtoPP( pp, entry_pp );
      
      if( PP_Get(pp,ID_PARA_level,NULL) != 50 ){
        u32 exp = POKETOOL_GetMinExp(
              PP_Get(pp,ID_PARA_monsno,NULL),
              PP_Get(pp,ID_PARA_form_no,NULL),
              50 );
        
        PP_Put( entry_pp, ID_PARA_exp, exp );
        PP_Renew( entry_pp );
      }
      
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }

  { //�G�g���[�i�[�ݒ�
    client = BTL_CLIENT_ENEMY1;
    
    //�G�g���[�i�[0 �g���[�i�[�f�[�^
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[0];
    bsw_trainer = &bsw_partner->bt_trd;
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->tr_id = 0;
    }else{
      tr_data->tr_id = bsw_trainer->player_id;
    }
    
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }
    
    //�|�P�����p�[�e�B
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode != BTL_MULTIMODE_NONE ) //�}���`
  { //�G�g���[�i�[�Q�ݒ�
    client = BTL_CLIENT_ENEMY2;
    
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[1];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    //���ɐݒ薳��
    
    //�|�P�����p�[�e�B
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode == BTL_MULTIMODE_PA_AA ) //AI�}���`
  { //AI�p�[�g�i�[�ݒ�
    client = BTL_CLIENT_PARTNER;
    KAGAYA_Printf( "�p�[�g�i�[No.%d\n", wk->partner );
    
    //�g���[�i�[�f�[�^
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->five_data[wk->partner];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    //���ɖ���
    
    //�|�P�����p�[�e�B
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
#if 0
  else if( dst->multiMode == BTL_MULTIMODE_PP_AA ) //�ʐM�}���`
  { //�ʐM�}���`�p�[�g�i�[�ݒ�
  }
#endif
  
  BTL_SETUP_SetSubwayMode( dst ); //��ʂ�Z�b�g������ɌĂԎ�
  return dst;
}

#if 0 //old
BATTLE_SETUP_PARAM * BSUBWAY_SCRWORK_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
  int i;
  BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  BTL_CLIENT_ID client;
  POKEPARTY *party;
  POKEMON_PARAM *pp;
  POKEMON_PARAM *entry_pp;
  BSP_TRAINER_DATA *tr_data;
  BSUBWAY_PARTNER_DATA *bsw_partner;
  BSUBWAY_TRAINER *bsw_trainer;
  
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  MYSTATUS *mystatus = GAMEDATA_GetMyStatus( gdata );
  u16 play_mode = wk->play_mode;
  
  dst = BATTLE_PARAM_Create( HEAPID_PROC );
  
  BTL_FIELD_SITUATION_SetFromFieldStatus( &dst->fieldSituation, gdata, GAMESYSTEM_GetFieldMapWork(gsys) );
  
  dst->netHandle = NULL;
  dst->commMode = BTL_COMM_NONE;
  dst->commPos = 0;
  dst->multiMode = BTL_MULTIMODE_NONE;
  dst->recBuffer = NULL;
  dst->fRecordPlay = FALSE;
  dst->competitor = BTL_COMPETITOR_SUBWAY;
  
  dst->commNetIDBit = 0xffff;
  
  dst->party[BTL_CLIENT_PLAYER] = NULL;
  dst->party[BTL_CLIENT_ENEMY1] = NULL;
  dst->party[BTL_CLIENT_PARTNER] = NULL;
  dst->party[BTL_CLIENT_ENEMY2] = NULL;
  dst->playerStatus[BTL_CLIENT_PLAYER] = NULL;
  dst->playerStatus[BTL_CLIENT_ENEMY1] = NULL;
  dst->playerStatus[BTL_CLIENT_PARTNER] = NULL;
  dst->playerStatus[BTL_CLIENT_ENEMY2] = NULL;
  
  dst->gameData     = gdata;
  dst->itemData     = GAMEDATA_GetMyItem( gdata );
  dst->bagCursor    = GAMEDATA_GetBagCursor( gdata );
  dst->zukanData    = GAMEDATA_GetZukanSave( gdata );
  dst->recordData   = GAMEDATA_GetRecordPtr( gdata );
  
  {
    SAVE_CONTROL_WORK *saveCtrl = GAMEDATA_GetSaveControlWork( gdata );
    dst->configData = SaveData_GetConfig( saveCtrl );
  }
  
//dst->commSupport  = GAMEDATA_GetCommPlayerSupportPtr( gdata );
  dst->commSupport  = NULL;
  
  dst->musicDefault = SEQ_BGM_VS_SUBWAY_TRAINER;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;
  dst->result = BTL_RESULT_WIN;
  
  BTL_SETUP_AllocRecBuffer( dst, HEAPID_PROC );
  
  dst->rule = BTL_RULE_SINGLE;
  
  switch( play_mode ){
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_S_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    dst->rule = BTL_RULE_DOUBLE;
    break;
  }
  
  switch( play_mode ){
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_S_MULTI:
    dst->multiMode = BTL_MULTIMODE_PA_AA;
    break;
  case BSWAY_PLAYMODE_COMM_MULTI:
  case BSWAY_PLAYMODE_S_COMM_MULTI:
    dst->multiMode = BTL_MULTIMODE_PP_AA;
    dst->netHandle = GFL_NET_HANDLE_GetCurrentHandle();
    dst->commMode = BTL_COMM_DS;
    dst->competitor = BTL_COMPETITOR_COMM;
//  dst->playerStatus[BTL_CLIENT_PARTNER] = &wk->mystatus_fr;
     
    if( GFL_NET_SystemGetCurrentID() == GFL_NET_NO_PARENTMACHINE ){
      dst->commPos = 0;
    }else{
      dst->commPos = 2;
    }
    break;
  }
  
  //�^��f�[�^�����̂��߁A
  //�ΐ푊���MYSTATUS, POKEPARTY���󂯎��o�b�t�@�Ƃ��Ċm�ۂ��܂� taya
  {
    u32 i;
    u32 heapID = HEAPID_PROC;

    for(i=0; i<BTL_CLIENT_NUM; ++i)
    {
      if( i != BTL_CLIENT_PLAYER )
      {
        if( dst->playerStatus[i] == NULL ){
          dst->playerStatus[i] = MyStatus_AllocWork( heapID );
        }
        if( dst->party[i] == NULL ){
          dst->party[i] = PokeParty_AllocPartyWork( heapID );
        }
      }
    }
  }
  
  { //�v���C���[�ݒ�
    const POKEPARTY *myparty = BSUBWAY_SCRWORK_GetPokePartyUse( wk );
    client = BTL_CLIENT_PLAYER;
    
    //MyStatus
    dst->playerStatus[client] = mystatus;
    
    //�g���[�i�[�f�[�^
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    MyStatus_CopyNameString( mystatus, tr_data->name );
    tr_data->tr_type = TRTYPE_HERO + MyStatus_GetMySex( mystatus );
    
    //�|�P�����p�[�e�B
    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      pp = PokeParty_GetMemberPointer( myparty, wk->member[i] );
      POKETOOL_CopyPPtoPP( pp, entry_pp );
      
      if( PP_Get(pp,ID_PARA_level,NULL) != 50 ){
        u32 exp = POKETOOL_GetMinExp(
              PP_Get(pp,ID_PARA_monsno,NULL),
              PP_Get(pp,ID_PARA_form_no,NULL),
              50 );
        
        PP_Put( entry_pp, ID_PARA_exp, exp );
        PP_Renew( entry_pp );
      }
      
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }

  { //�G�g���[�i�[�ݒ�
    client = BTL_CLIENT_ENEMY1;
    
    //�g���[�i�[�f�[�^
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[0];
    bsw_trainer = &bsw_partner->bt_trd;
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->tr_id = 0;
    }else{
      tr_data->tr_id = bsw_trainer->player_id;
    }
    
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //�ŋ�
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    if( play_mode == BSWAY_MODE_WIFI ){
      tr_data->win_word = *(PMS_DATA*)bsw_trainer->win_word;
      tr_data->lose_word = *(PMS_DATA*)bsw_trainer->lose_word;
    }
    
    //�|�P�����p�[�e�B
//    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode != BTL_MULTIMODE_NONE ) //�}���`
  { //�G�g���[�i�[�Q�ݒ�
    client = BTL_CLIENT_ENEMY2;
    
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->tr_data[1];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //�ŋ�
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    //�|�P�����p�[�e�B
//  dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
  
  if( dst->multiMode == BTL_MULTIMODE_PA_AA ) //AI�}���`
  { //AI�p�[�g�i�[�ݒ�
    client = BTL_CLIENT_PARTNER;
    
    KAGAYA_Printf( "�p�[�g�i�[No.%d\n", wk->partner );

    //�g���[�i�[�f�[�^
    dst->tr_data[client] = create_BSP_TRAINER_DATA( HEAPID_PROC );
    tr_data = dst->tr_data[client];
    
    bsw_partner = &wk->five_data[wk->partner];
    bsw_trainer = &bsw_partner->bt_trd;
    
    tr_data->tr_id = bsw_trainer->player_id;
    tr_data->tr_type = bsw_trainer->tr_type;
//    tr_data->ai_bit = 0xFFFFFFFF;  //�ŋ�
    tr_data->ai_bit = 0x00000087;  //�ŋ�
    
    //�g���[�i�[�f�[�^�@name
    GFL_STR_SetStringCode( tr_data->name, bsw_trainer->name );
    
    //�g���[�i�[�f�[�^�@word
    PMSDAT_Clear( &tr_data->win_word );
    PMSDAT_Clear( &tr_data->lose_word );
    
    //�|�P�����p�[�e�B
//    dst->party[client] = PokeParty_AllocPartyWork( HEAPID_PROC );
    party = dst->party[client];
    PokeParty_Init( party, TEMOTI_POKEMAX );
    
    entry_pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
    
    for( i = 0; i < wk->member_num; i++ ){
      make_PokePara( &(bsw_partner->btpwd[i]), entry_pp );
      PokeParty_Add( party, entry_pp );
    }
    
    GFL_HEAP_FreeMemory( entry_pp );
  }
#if 0
  else if( dst->multiMode == BTL_MULTIMODE_PP_AA ) //�ʐM�}���`
  { //�ʐM�}���`�p�[�g�i�[�ݒ�
  }
#endif
  
  return dst;
}
#endif

//--------------------------------------------------------------
/**
 *
 * @param
 * @retval
 */
//--------------------------------------------------------------
COMM_BTL_DEMO_PARAM * BSUBWAY_SCRWORK_CreateBattleDemoParam(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys )
{
  COMM_BTL_DEMO_PARAM *demo;
  demo = &bsw_scr->comm_btl_demo_param;
  return( demo );
}

//======================================================================
//  �ʐM�֘A�R�}���h
//======================================================================
#if 0
/**
 *  @brief  �o�g���T�u�E�F�C�@�����Ă����v���C���[�f�[�^���󂯎��
 */
u16 BTowerComm_RecvPlayerData(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  u16  ret = 0;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  wk->pare_sex = (u8)recv_buf[0];
  wk->pare_poke[0] = recv_buf[1];
  wk->pare_poke[1] = recv_buf[2];
  wk->pare_stage = recv_buf[3];

  wk->partner = 5+wk->pare_sex;

  OS_Printf("sio multi mem = %d,%d:%d,%d\n",wk->mem_poke[0],wk->mem_poke[1],wk->pare_poke[0],wk->pare_poke[1]);
  if(  wk->mem_poke[0] == wk->pare_poke[0] ||
    wk->mem_poke[0] == wk->pare_poke[1]){
    ret += 1;
  }
  if(  wk->mem_poke[1] == wk->pare_poke[0] ||
    wk->mem_poke[1] == wk->pare_poke[1]){
    ret += 2;
  }
  return ret;
}

/**
 *  @brief  �o�g���T�u�E�F�C�@�����Ă����g���[�i�[�f�[�^���󂯎��
 */
u16  BTowerComm_RecvTrainerData(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  int i;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  if(CommGetCurrentID() == COMM_PARENT_ID){
    return 0;  //�e�͑��M���邾���Ȃ̂Ŏ󂯎��Ȃ�
  }

  MI_CpuCopy8(recv_buf,wk->trainer,BSUBWAY_STOCK_TRAINER_MAX*2);
  OS_Printf("sio multi trainer01 = %d,%d:%d,%d\n",
      wk->trainer[0],wk->trainer[1],wk->trainer[2],wk->trainer[3]);
  OS_Printf("sio multi trainer02 = %d,%d:%d,%d\n",
      wk->trainer[4],wk->trainer[5],wk->trainer[6],wk->trainer[7]);
  OS_Printf("sio multi trainer03 = %d,%d:%d,%d\n",
      wk->trainer[8],wk->trainer[9],wk->trainer[10],wk->trainer[11]);
  OS_Printf("sio multi trainer04 = %d,%d\n",
      wk->trainer[12],wk->trainer[13]);
  return 1;
}

/**
 *  @brief  �o�g���T�u�E�F�C�@�����Ă������^�C�A���邩�ǂ����̌��ʂ��󂯎��
 *
 *  @retval  0  ���^�C�A���Ȃ�
 *  @retval  1  ���^�C�A����
 */
u16  BTowerComm_RecvRetireSelect(FIELDSYS_WORK* fsys,const u16* recv_buf)
{
  int i;
  BSUBWAY_SCRWORK* wk = fsys->btower_wk;

  OS_Printf("sio multi retire = %d,%d\n",wk->retire_f,recv_buf[0]);
  if(wk->retire_f || recv_buf[0]){
    return 1;
  }
  return 0;
}

/**
 *  @brief  �o�g���T�u�E�F�C�@���@���ʂƃ����X�^�[No�𑗐M
 */
void BTowerComm_SendPlayerData(BSUBWAY_SCRWORK* wk,SAVEDATA *sv)
{
  int i;
  POKEPARTY* party;
  POKEMON_PARAM *pp;
  MYSTATUS  *my = SaveData_GetMyStatus(sv);

  wk->send_buf[0] = MyStatus_GetMySex(my);
  party = SaveData_GetTemotiPokemon(sv);
  for(i = 0;i < 2;i++){
    wk->send_buf[1+i] =
      PP_Get(PokeParty_GetMemberPointer(party,wk->member[i]),
            ID_PARA_monsno,NULL);
  }

  //������BSWAY_MODE_COMM_MULTI��p
  OS_Printf( "wk->play_mode = %d\n", wk->play_mode );
  wk->send_buf[3] =
    TowerScoreData_SetStage(wk->scoreSave,BSWAY_MODE_COMM_MULTI,BSWAY_DATA_get);
}

/**
 *  @brief  �o�g���T�u�E�F�C�@�ʐM�}���`�@���I�����g���[�i�[No���q�@�ɑ��M
 */
void BTowerComm_SendTrainerData(BSUBWAY_SCRWORK* wk)
{
  MI_CpuCopy8(wk->trainer,wk->send_buf,BSUBWAY_STOCK_TRAINER_MAX*2);
}

/**
 *  @brief  �o�g���T�u�E�F�C�@�ʐM�}���`�@���^�C�A���邩�ǂ������݂��ɑ��M
 *
 *  @param  retire  TRUE�Ȃ烊�^�C�A
 */
void BTowerComm_SendRetireSelect(BSUBWAY_SCRWORK* wk,u16 retire)
{
  //�����̑I�����ʂ����[�N�ɕۑ�
  wk->retire_f = retire;
  wk->send_buf[0] = retire;
}
#endif

//======================================================================
//  �g���[�i�[�f�[�^
//======================================================================
//--------------------------------------------------------------
//  �g���[�i�[�ԍ��e�[�u���@�m�[�}�� �P�`�R�T�ڂ̃��E���h�P�`�U
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Normal1_6[3][2] =
{
  {  1-1,  50-1}, //stage 1 round 1-6
  { 31-1,  70-1}, //stage 2 round 1-6
  { 51-1, 110-1}, //stage 3 round 1-6
};

//--------------------------------------------------------------
//  �g���[�i�[�ԍ��e�[�u���@�m�[�}�� �P�`�Q�T�ڂ̃��E���h�V
//  �R�T�ڂ͐�p�̔ԍ��ɂȂ�B
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Normal_Round7[2][2] =
{
  {  1-1,  50-1}, //stage 1 round 7
  { 31-1,  70-1}, //stage 2 round 7
};

//--------------------------------------------------------------
//  �g���[�i�[�ԍ��e�[�u���@�X�[�p�[�@�P�`�S�T�ڂ̃��E���h�P�`�U
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super1_6[4][2]=
{
  {111-1,160-1},
  {141-1,180-1},
  {161-1,200-1},
  {181-1,220-1},
};

//--------------------------------------------------------------
//  �g���[�i�[�ԍ��e�[�u���@�X�[�p�[�@�P�`�S�T�ڂ̃��E���h�V
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super7[4][2]=
{
  {161-1,180-1},
  {181-1,200-1},
  {201-1,220-1},
  {221-1,240-1},
};

//--------------------------------------------------------------
//  �g���[�i�[�ԍ��e�[�u���@�X�[�p�[�@�T�T�ڈȍ~�̂P�`�V
//--------------------------------------------------------------
static const u16 TrainerNoRangeTable_Super_Stage5[1][2]=
{
  {201-1,300-1}, //�S������
};

//--------------------------------------------------------------
/**
 * @brief  �g���[�i�[�i���o�[���I
 * @param bsw_scr BSUBWAY_SCRWORK
 * @param stage �X�e�[�W��
 * @param round ���l�ڂ�
 * @param play_mode �v���C���[�h
 * @retval u16 �g���[�i�[�i���o�[
 */
//--------------------------------------------------------------
u16 BSUBWAY_SCRWORK_GetTrainerNo(
    BSUBWAY_SCRWORK* wk, u16 stage, u8 round, int play_mode, u8 pair_no )
{
  u16 no = 0;
  u16 super = 0;
  s16 boss_no = -1;
  const u16 *tbl;
  
  OS_Printf( "BSW GET TRAINER NO stage = %d round = %d\n", stage, round );
  
  if( round == 6 ) //��p�{�X���H
  {
    if( stage == 2 )
    {
      boss_no = 0;  //�m�[�}���{�X
    }
    else if( stage == 6 )
    {
      boss_no = 1;  //�X�[�p�[�{�X
    }
  }
  
  switch( play_mode ) //�X�[�p�[���[�h���H
  {
  case BSWAY_MODE_S_SINGLE:
  case BSWAY_MODE_S_DOUBLE:
  case BSWAY_MODE_S_MULTI:
  case BSWAY_MODE_S_COMM_MULTI:
    super = 1;
  }
  
  if( boss_no != -1 ) //��p�{�X
  {
    switch( play_mode )
    {
    case BSWAY_MODE_SINGLE:
    case BSWAY_MODE_S_SINGLE:
      if( boss_no == 0 )
      {
        no = BSW_TR_DATANO_SINGLE;
      }
      else if( boss_no == 1 )
      {
        no = BSW_TR_DATANO_S_SINGLE;
      }
      break;
    case BSWAY_MODE_DOUBLE:
    case BSWAY_MODE_S_DOUBLE:
      if( boss_no == 0 )
      {
        no = BSW_TR_DATANO_DOUBLE;
      }
      else if( boss_no == 1 )
      {
        no = BSW_TR_DATANO_S_DOUBLE;
      }
      break;
    case BSWAY_MODE_MULTI:
    case BSWAY_MODE_COMM_MULTI:
    case BSWAY_MODE_S_MULTI:
    case BSWAY_MODE_S_COMM_MULTI:
      if( boss_no == 0 )
      {
        if( pair_no == 0 )
        {
          no = BSW_TR_DATANO_MULTI0;
        }
        else
        {
          no = BSW_TR_DATANO_MULTI1;
        }
      }
      else if( boss_no == 1 )
      {
        if( pair_no == 0 )
        {
          no = BSW_TR_DATANO_S_MULTI0;
        }
        else
        {
          no = BSW_TR_DATANO_S_MULTI1;
        }
      }
      break;
    default:
      GF_ASSERT( 0 );
    }
  }
  else if( super == 0 ) //�m�[�}�����[�h
  {
    if( stage < 3 ) //�X�e�[�W�Q�܂�
    {
      if( round < 6 ) //���E���h�P�`�U
      {
        tbl = TrainerNoRangeTable_Normal1_6[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
      else  //���E���h�V
      {
        if( stage < 2 )
        {
          tbl = TrainerNoRangeTable_Normal_Round7[stage];
          no = tbl[1] - tbl[0] + 1;
          no = tbl[0] + (get_Rand(wk) % no);
        }
        else //�X�e�[�W�Q�ȏ�͂��肦�Ȃ�
        {
          GF_ASSERT( 0 );
        }
      }
    }
    else //�X�e�[�W�R�ȏ�͂��肦�Ȃ�
    {
      GF_ASSERT( 0 );
    }
  }
  else //�X�[�p�[���[�h
  {
    if( stage < 4 ) //�X�e�[�W�S�܂�
    {
      if( round < 6 ) //���E���h�P�`�U
      {
        tbl = TrainerNoRangeTable_Super1_6[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
      else //���E���h�V
      {
        tbl = TrainerNoRangeTable_Super7[stage];
        no = tbl[1] - tbl[0] + 1;
        no = tbl[0] + (get_Rand(wk) % no);
      }
    }
    else //�X�[�p�[�X�e�[�W�T�ȍ~
    {
      tbl = TrainerNoRangeTable_Super_Stage5[0];
      no = tbl[1] - tbl[0] + 1;
      no = tbl[0] + (get_Rand(wk) % no);
    }
  }
  
  OS_Printf( "BSW GET TRAINER NO GET NUM = %d\n", no );
  return no;
}

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�@rom�g���[�i�[�f�[�^�W�J
 *  BSUBWAY_TRAINER_ROM_DATA�^���������m�ۂ��ĕԂ��̂ŁA
 *  �Ăяo�����������I�ɉ�����邱��
 *  @param tr_data BSUBWAY_PARTER_DATA
 *  @param tr_no �g���[�i�[�i���o�[
 *  @param heapID HEAPID
 *  @retval BSUBWAY_TRAINER_ROM_DATA*
 *  TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 *  TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_TrainerDataGet
 */
//--------------------------------------------------------------
static BSUBWAY_TRAINER_ROM_DATA * alloc_TrainerRomData(
    BSUBWAY_PARTNER_DATA *tr_data, u16 tr_no, HEAPID heapID )
{
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  GFL_MSGDATA *msgdata;
  STRBUF *name;
  
  msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_btdtrname_dat, heapID );
  
  MI_CpuClear8(tr_data, sizeof(BSUBWAY_PARTNER_DATA));
  trd = get_TrainerRomData(tr_no,heapID);
  
  //�g���[�i�[ID���Z�b�g
  tr_data->bt_trd.player_id = tr_no + 1; //1 origin
  
  //�g���[�i�[�o�����b�Z�[�W
  tr_data->bt_trd.appear_word[0] = 0xFFFF;
  tr_data->bt_trd.appear_word[1] = tr_no*3;
  
  //�g���[�i�[�f�[�^���Z�b�g
  tr_data->bt_trd.tr_type=trd->tr_type;
  
  //GS�f�[�^����̈ڐA�ɂ�鏈��
  //wb�ł͑��݂��Ă��Ȃ��^�C�v����������
  #if 0
  if( check_TrainerType(trd->tr_type) == FALSE ){
    OS_Printf( "BSUBWAY ERROR TRAINER TYPE" );
    tr_data->bt_trd.tr_type = TRTYPE_TANPAN;
  }
  #endif
  
  OS_Printf( "BSUBWAY allocTrainerRomData NameNo = %d\n", tr_no );

  name = GFL_MSG_CreateString( msgdata, tr_no );
  GFL_STR_GetStringCode( name,
      &tr_data->bt_trd.name[0], BUFLEN_PERSON_NAME );
  GFL_STR_DeleteBuffer(name);
  GFL_MSG_Delete( msgdata );
  return trd;
}

//--------------------------------------------------------------
/// �A�C�e���e�[�u��
//  �����|�P��������̓����_���ł��Ă��邪�������[�v�h�~�̂��߁A
//  ������x�܂킵����A�|�P�����̕s��v�݂̂��`�F�b�N���āA
//  �A�C�e�����Œ�Ŏ������邽�߂̃A�C�e���e�[�u��
//--------------------------------------------------------------
static const u16 BattleTowerPokemonItem[]={
  ITEM_HIKARINOKONA,
  ITEM_RAMUNOMI,
  ITEM_TABENOKOSI,
  ITEM_SENSEINOTUME,
};

//--------------------------------------------------------------
/**
 * �|�P�����̐��i���擾
 * ���i�́A��������25�Ŋ������]�肩��Z�o�����
 * @param[in]  rnd  �擾������������
 * @return  �擾�������i
 */
//--------------------------------------------------------------
static u8  get_PokeSeikaku(u32 rnd)
{
  return (u8)(rnd % 25);
}

//--------------------------------------------------------------
/**
 *  @brief  �o�g���T�u�E�F�C�̃|�P�����p�����[�^����
 *  @param pwd  [in/out]�|�P�����p�����[�^�̓W�J�ꏊ
 *  @param poke_no  [in]�^���[rom�f�[�^�|�P�����i���o�[
 *  @param poke_id  [in]�|�P�����ɃZ�b�g����id
 *  @param poke_rnd [in]�Z�b�g���������(0�������n���ꂽ��֐����Ő���)
 *  @param pow_rnd  [in]�|�P�����ɃZ�b�g����pow_rnd�l
 *  @param mem_idx  [in]�����o�[index�B��̖�or��̖�
 *  @param itemfix  [in]TRUE�Ȃ�Œ�A�C�e���BFALSE�Ȃ�rom�f�[�^�̃A�C�e��
 *  @param heapID [in]�e���|�������������m�ۂ���q�[�vID
 *  @retval u32 personal_rnd:�������ꂽ�|�P�����̌������l
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_PokemonParamMake
 */
//--------------------------------------------------------------
static u32 make_PokemonParam(
    BSUBWAY_SCRWORK *bsw_scr, BSUBWAY_POKEMON *pwd,
    u16 poke_no, u32 poke_id, u32 poke_rnd, u8 pow_rnd,
    u8 mem_idx, BOOL itemfix, HEAPID heapID )
{
  int i;
  int  exp;
  u32  personal_rnd;
  u8  friend;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;

  MI_CpuClear8(pwd,sizeof(BSUBWAY_POKEMON));

  //�����f�[�^���[�h
  get_PokemonRomData(&prd_s,poke_no);

  //�����X�^�[�i���o�[
  pwd->mons_no=prd_s.mons_no;

  //�t�H�����i���o�[
  pwd->form_no=prd_s.form_no;

  //��������
  if(itemfix){
    //50��ȏ�܂킵�Ă����t���O��TURE�Ȃ�A
    //�����A�C�e���͌Œ�̂��̂���������
    pwd->item_no=BattleTowerPokemonItem[mem_idx];
  }else{
    //rom�f�[�^�ɓo�^���ꂽ�A�C�e������������
    pwd->item_no=prd_s.item_no;
  }

  //�Ȃ��x��255���f�t�H���g
  friend=255;
  for(i=0;i<WAZA_TEMOTI_MAX;i++){
    pwd->waza[i]=prd_s.waza[i];
    //�������������Ă���Ƃ��́A�Ȃ��x��0�ɂ���
    if(prd_s.waza[i]==WAZANO_YATUATARI){
      friend=0;
    }
  }

  //IDNo
  pwd->id_no=poke_id;

  if(poke_rnd == 0){
    //������
    do{
//    personal_rnd=(gf_rand()|gf_rand()<<16);
      personal_rnd=(get_Rand(bsw_scr)|get_Rand(bsw_scr)<<16);
#if 0
    }while((prd_s.chr!=get_PokeSeikaku(personal_rnd))&&(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#else
    //�v���`�i�̓^���[���C������(08.03.17)(����������frontier_tool.c�ɂ�����̂Œ��ӁI)
    //�f�[�^�̐��i�ƈ�v���Ă��Ȃ�"��������"���A�̎��́A���[�v����
    }while((prd_s.chr!=get_PokeSeikaku(personal_rnd))||(
          POKETOOL_CheckRare(poke_id,personal_rnd)==TRUE));
#endif
    //OS_Printf( "���肵��personal_rnd = %d\n", personal_rnd );
    //OS_Printf( "get_PokeSeikaku = %d\n",get_PokeSeikaku(personal_rnd) );
    //OS_Printf( "���A����Ȃ��� = %d\n", PokeRareGetPara(poke_id,personal_rnd) );
    pwd->personal_rnd=personal_rnd;
  }else{
    pwd->personal_rnd = poke_rnd;  //0�łȂ���Έ����̒l���g�p
    personal_rnd = poke_rnd;
  }


  //�p���[����
  pwd->hp_rnd=pow_rnd;
  pwd->pow_rnd=pow_rnd;
  pwd->def_rnd=pow_rnd;
  pwd->agi_rnd=pow_rnd;
  pwd->spepow_rnd=pow_rnd;
  pwd->spedef_rnd=pow_rnd;

  //�w�͒l
#if 0 //wb null
  exp=0;
  for(i=0;i<6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      exp++;
    }
  }
  if((PARA_EXP_TOTAL_MAX/exp)>255){
    exp=255;
  }else{
    exp=PARA_EXP_TOTAL_MAX/exp;
  }
  for(i = 0;i < 6;i++){
    if(prd_s.exp_bit&No2Bit(i)){
      pwd->exp[i]=exp;
    }
  }
#endif

  //�Z�|�C���g
  pwd->pp_count=0;

  //���R�[�h
  pwd->country_code = 0;

  //����
  i= POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi2);
  if(i){
    if(pwd->personal_rnd&1){
      pwd->tokusei=i;
    }else{
      pwd->tokusei=POKETOOL_GetPersonalParam(pwd->mons_no,0,POKEPER_ID_speabi1);
    }
  }else{
    pwd->tokusei=POKETOOL_GetPersonalParam( pwd->mons_no,0,POKEPER_ID_speabi1);
  }

  //�Ȃ��x
  pwd->natuki=friend;

  //�j�b�N�l�[��
  {
   GFL_MSGDATA *msgdata = GFL_MSG_Create(
       GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat,
       heapID );
    GFL_MSG_GetStringRaw( msgdata,
        pwd->mons_no, pwd->nickname, MONS_NAME_SIZE+EOM_SIZE );
    GFL_MSG_Delete( msgdata );
  }
  return personal_rnd;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�g���[�i�[�f�[�^����
 * �i�����f�[�^��BSUBWAY_PARTNER_DATA�\���̂ɓW�J�j
 * @param tr_data  [in/out]��������BSUBWAY_PARTNAER_DATA�\����
 * @param tr_no [in]�������ɂȂ�g���[�i�[ID
 * @param cnt [in]�g���[�i�[�Ɏ�������|�P�����̐�
 * @param set_poke_no [in]�y�A�g���[�i�[�̃|�P�����iNULL �`�F�b�N�Ȃ��j
 * @param set_item_no [in]�y�A�̃|�P�����̑����A�C�e���iNULL �`�F�b�N�Ȃ��j
 * @param poke [in/out] ���I�|�P������̂̃p�����[�^�i�[��(NULL���ƂȂ��j
 * @param heapID  [in]�q�[�vID
 * @retval TRUE ���I���[�v��50�񒴂��� FALSE ���I���[�v��50��ȓ��ŏI�����
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY ��������tower_tool.c FSRomBattleTowerTrainerDataMake
 * TOWER_AI_MULTI_ONLY �T�����ŁA5�l�O�̎莝���|�P�����𐶐����Ă���
 */
//--------------------------------------------------------------
BOOL BSUBWAY_SCRWORK_MakeRomTrainerData(
    BSUBWAY_SCRWORK *bsw_scr, BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no, int cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  BOOL      ret = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  
  //�g���[�i�[�f�[�^�Z�b�g
  trd = alloc_TrainerRomData(tr_data,tr_no,heapID);

  //�|�P�����f�[�^���Z�b�g
  ret = set_BSWayPokemonParam(
      bsw_scr,trd,tr_no,&tr_data->btpwd[0],cnt,
      set_poke_no,set_item_no,poke,heapID);
  GFL_HEAP_FreeMemory( trd );
  return ret;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�g���[�i�[�̎����|�P�����̃p���[���������肷��
 * @param  tr_no  �g���[�i�[�i���o�[
 * @return  �p���[����
 * b_tower_fld.c��b_tower_ev�Ɉړ�
 */
//--------------------------------------------------------------
static u8 get_PowerRnd(u16 tr_no)
{
  u8  pow_rnd;

  if(tr_no<100){
    pow_rnd=(0x1f/8)*1;
  }
  else if(tr_no<120){
    pow_rnd=(0x1f/8)*2;
  }
  else if(tr_no<140){
    pow_rnd=(0x1f/8)*3;
  }
  else if(tr_no<160){
    pow_rnd=(0x1f/8)*4;
  }
  else if(tr_no<180){
    pow_rnd=(0x1f/8)*5;
  }
  else if(tr_no<200){
    pow_rnd=(0x1f/8)*6;
  }
  else if(tr_no<220){
    pow_rnd=(0x1f/8)*7;
  }
  else{
    pow_rnd=0x1f;
  }
  return pow_rnd;
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C �y�A�g���[�i�[�f�[�^�Đ���
 * �i�Z�[�u���ꂽAI�}���`�p�[�g�i�[�̃f�[�^��
 * BSUBWAY_PARTNER_DATA�\���̂ɓW�J�j
 * @param tr_data [in/out] ��������BSUBWAY_PARTNAER_DATA�\����
 * @param tr_no [in]  �������ɂȂ�g���[�i�[ID
 * @param fixitem [in] TURE�Ȃ�Œ�A�C�e���AFALSE�Ȃ�rom�A�C�e�����Z�b�g
 * @param poke  [in] �|�P�����f�[�^�Đ����ɕK�v�ȍ\���̌^�f�[�^�ւ̃|�C���^
 * @param heapID [in] �q�[�vID
 * @retval nothing
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY AI�}���`�ŋx�ނ̌�A�ĊJ�������ɌĂ΂��
 */
//--------------------------------------------------------------
void BSUBWAY_SCRWORK_MakePartnerRomData(
    BSUBWAY_SCRWORK *wk, BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no,BOOL itemfix,
    const BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  int  i;
  u8  pow_rnd = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;

  //�g���[�i�[�f�[�^�Z�b�g
  trd = alloc_TrainerRomData(tr_data,tr_no,heapID);

  //�|�P�����f�[�^���Z�b�g
  pow_rnd=get_PowerRnd(tr_no);

  for(i = 0;i < BSUBWAY_STOCK_PAREPOKE_MAX;i++){
    make_PokemonParam(
      wk,&(tr_data->btpwd[i]),
      poke->poke_no[i],poke->poke_id,
      poke->poke_rnd[i],pow_rnd,i,itemfix,heapID);
  }
  GFL_HEAP_FreeMemory( trd );
}

//--------------------------------------------------------------
/**
 * �o�g���T�u�E�F�C�̃|�P���������߂�
 * @param trd [in]�g���[�i�[�f�[�^
 * @param tr_no [in]�g���[�i�[�i���o�[
 * @param pwd [in/out]BSUBWAY_POKEMON�\����
 * @param cnt [in]�g���[�i�[�Ɏ�������|�P�����̐�
 * @param set_poke_no [in]�y�A�g���[�i�[�|�P���� (NULL���ƃ`�F�b�N�Ȃ�
 * @param set_item_no [in]�y�A�g���[�i�[�|�P�����A�C�e�� (NULL �`�F�b�N�Ȃ�
 * @param poke [in/out]���I�|�P������̂̃p�����[�^�i�[��(NULL �`�F�b�N�Ȃ�
 * @param heapID [in]�q�[�vID
 * @retval  FALSE  ���I���[�v��50��ȓ��ŏI�����
 * @retval  TRUE  ���I���[�v��50��ȓ��ŏI���Ȃ�����
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 */
//--------------------------------------------------------------
static BOOL set_BSWayPokemonParam(
    BSUBWAY_SCRWORK *wk, BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no, BSUBWAY_POKEMON *pwd, u8 cnt,
    u16 *set_poke_no, u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID )
{
  int  i,j;
  u8  pow_rnd;
  u8  poke_index;
  u32  id;
  int  set_index;
  int  set_index_no[4];
  u32  set_rnd_no[4];
  int  set_count;
  int  loop_count;
  BOOL  ret = 0;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;
  BSUBWAY_POKEMON_ROM_DATA  prd_d;

  //�莝���|�P������MAX��4�̂܂�
  GF_ASSERT(cnt<=4);

  set_count=0;
  loop_count=0;
  while(set_count!=cnt){
//    poke_index = gf_rand()%trd->use_poke_cnt;
    poke_index = get_Rand(wk)%trd->use_poke_cnt;
    set_index=trd->use_poke_table[poke_index];
    get_PokemonRomData(&prd_d,set_index);

    //�����X�^�[�i���o�[�̃`�F�b�N�i����̃|�P�����͎����Ȃ��j
    for(i=0;i<set_count;i++){
      get_PokemonRomData(&prd_s,set_index_no[i]);
      if(prd_s.mons_no==prd_d.mons_no){
        break;
      }
    }
    
    //�y�A��g��ł���g���[�i�[�̎����|�P�����Ƃ̃`�F�b�N
    if(set_poke_no!=NULL){
      //�����X�^�[�i���o�[�̃`�F�b�N�i����̃|�P�����͎����Ȃ��j
      for(i=0;i<cnt;i++){
        if(set_poke_no[i]==prd_d.mons_no){
          break;
        }
      }
      if(i!=cnt){
        continue;
      }
    }

    //50��܂킵�āA���܂�Ȃ��悤�Ȃ�A����A�C�e���`�F�b�N�͂��Ȃ�
    if(loop_count<50){
      //�����A�C�e���̃`�F�b�N�i����̃A�C�e���͎����Ȃ��j
      for(i=0;i<set_count;i++){
        get_PokemonRomData(&prd_s,set_index_no[i]);
        if((prd_s.item_no)&&(prd_s.item_no==prd_d.item_no)){
          break;
        }
      }
      if(i!=set_count){
        loop_count++;
        continue;
      }
      //�y�A��g��ł���g���[�i�[�̎����|�P�����̑����A�C�e���Ƃ̃`�F�b�N
      if(set_item_no!=NULL){
        //�����A�C�e���̃`�F�b�N�i����̃A�C�e���͎����Ȃ��j
        for(i=0;i<cnt;i++){
          if((set_item_no[i]==prd_d.item_no) && (set_item_no[i]!=0)){
            break;
          }
        }
        if(i!=cnt){
          loop_count++;
          continue;
        }
      }
    }

    set_index_no[set_count]=set_index;
    set_count++;
  }

  pow_rnd=get_PowerRnd(tr_no);
//id=(gf_rand()|(gf_rand()<<16));
  id=(get_Rand(wk)|(get_Rand(wk)<<16));

  if(loop_count >= 50){
    ret = TRUE;
  }
  for(i=0;i<set_count;i++){
    set_rnd_no[i] = make_PokemonParam(wk,&(pwd[i]),
      set_index_no[i],id,0,pow_rnd,i,ret,heapID);
  }
  if(poke == NULL){
    return ret;
  }
  //�|�C���^��NULL�łȂ���΁A���I���ꂽ�|�P�����̕K�v�ȃp�����[�^��Ԃ�
  poke->poke_id = id;
  for(i = 0;i< BSUBWAY_STOCK_PAREPOKE_MAX;i++){
    poke->poke_no[i] = set_index_no[i];
    poke->poke_rnd[i] = set_rnd_no[i];
  }
  return ret;
}

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�g���[�i�[�����f�[�^�̓ǂݏo��
 * @param tr_no  [in]�g���[�i�[�i���o�[
 * @param heapID  [in]�������m�ۂ��邽�߂̃q�[�vID
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_TrainerRomDataGet
 */
//--------------------------------------------------------------
static BSUBWAY_TRAINER_ROM_DATA * get_TrainerRomData(
    u16 tr_no, HEAPID heapID )
{
  tr_no += BSW_TR_ARCDATANO_ORG;
  OS_Printf( "BSUBWAY load TrainerRomData Num = %d\n", tr_no );
  return GFL_ARC_UTIL_Load( ARCID_BSW_TR, tr_no, 0, heapID );
}

#if 0 //old gs
static void * get_TrainerRomData( u16 tr_no, HEAPID heapID )
{
#ifdef DEBUG_ONLY_FOR_kagaya
  OS_Printf( "BSUBWAY load TrainerRomData Num = %d\n", tr_no );
#endif
  //AI�}���`����Ȃ̂Ńv���`�i�I
  return GFL_ARC_UTIL_Load( ARCID_PL_BTD_TR, tr_no, 0, heapID );
}
#endif

//--------------------------------------------------------------
/**
 *  �o�g���T�u�E�F�C�|�P���������f�[�^�̓ǂݏo��
 * @param[in]  prd    �ǂݏo�����|�P�����f�[�^�̊i�[��
 * @param[in]  index  �ǂݏo���|�P�����f�[�^�̃C���f�b�N�X
 * TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈��
 * TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_PokemonRomDataGet
 */
//--------------------------------------------------------------
static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd, int index)
{
//  index += BSW_PM_ARCDATANO_ORG;
  OS_Printf( "BSUBWAY load PokemonRomData Num = %d\n", index );
  GFL_ARC_LoadData( (void*)prd, ARCID_BSW_PD, index );
}

#if 0 //old gs
static void get_PokemonRomData(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index)
{
  //�����͒ʐM�͂��肦�Ȃ��̂Ńv���`�i����I(AI�}���`)
  GFL_ARC_LoadData( (void*)prd, ARCID_PL_BTD_PM, index );
}
#endif

//--------------------------------------------------------------
/**
 *  �o�g���^���[�g���[�i�[�f�[�^����
 * @param[in/out]  bp      ��������BATTLE_PARAM�\����
 * @param[in]    tr_data    �������ɂȂ�g���[�i�[�f�[�^
 * @param[in]    cnt      �g���[�i�[�̎����|�P������
 * @param[in]    client_no  ��������client_no
 * @param[in]    heapID    �q�[�vID�iPOKEMON_PARAM�̐����ɕK�v�j
 */
//--------------------------------------------------------------
static void  make_TrainerData(
    BATTLE_SETUP_PARAM *bp, BSUBWAY_PARTNER_DATA *tr_data,
    int cnt, BTL_CLIENT_ID client_no, HEAPID heapID )
{
  int i,j;
  PMS_DATA *pd;
  POKEMON_PARAM  *pp;
  
  //�g���[�i�[ID���Z�b�g
  bp->tr_data[client_no]->tr_id = tr_data->bt_trd.player_id;
  
  //�g���[�i�[�f�[�^���Z�b�g
  bp->tr_data[client_no]->tr_type = tr_data->bt_trd.tr_type;
  
#if 0
  //GS�f�[�^����̈ڐA�ɂ�鏈��
  //wb�ł͑��݂��Ă��Ȃ��^�C�v����������
  if( check_TrainerType(bp->tr_data[client_no]->tr_type) == FALSE ){
    bp->tr_data[client_no]->tr_type = TRTYPE_TANPAN;
  }
#endif

//PM_strcpy( bp->tr_data[client_no].name, &tr_data->bt_trd.name[0] );
  GFL_STR_SetStringCode( bp->tr_data[client_no]->name, tr_data->bt_trd.name );
  
  pd = (PMS_DATA*)&tr_data->bt_trd.win_word[0];
  bp->tr_data[client_no]->win_word = *pd;
  pd = (PMS_DATA*)&tr_data->bt_trd.lose_word[0];
  bp->tr_data[client_no]->lose_word = *pd;
  
  //�|�P�����f�[�^���Z�b�g
  {
    POKEPARTY **party = &bp->party[client_no];

    pp = GFL_HEAP_AllocMemoryLo( heapID, POKETOOL_GetWorkSize() );
    *party = PokeParty_AllocPartyWork( HEAPID_PROC );
    PokeParty_Init( *party, TEMOTI_POKEMAX );
    
    for(i=0;i<cnt;i++){
      make_PokePara( &tr_data->btpwd[i], pp );
      PokeParty_Add( *party, pp );
    }
  
    GFL_HEAP_FreeMemory( pp );
  }
}

//======================================================================
//  �p�[�c
//======================================================================
//--------------------------------------------------------------
/**
 * ������R�s�[
 * @param  to_str    �R�s�[��o�b�t�@
 * @param  from_str  ������f�[�^
 * @return  �o�b�t�@�ɃR�s�[�����f�[�^�Ō��EOM_�ւ̃|�C���^
 *  �f�[�^���Ȃ�(������f�[�^�擪��EOM_������)�ꍇ�A
 *  EOM_���o�b�t�@�擪�ɏ������݁A�����ւ̃|�C���^��Ԃ�
 */
//--------------------------------------------------------------
static STRCODE * PM_strcpy( STRCODE* to_str, const STRCODE* from_str )
{
  while( *from_str != GFL_STR_GetEOMCode() ){
    *to_str = *from_str;
    to_str++;
    from_str++;
  }
  *to_str = GFL_STR_GetEOMCode();
  return to_str;
}

/**
 *  @brief  �Q���ł��Ȃ��|�P���������^�O�W�J
 */
#if 0
WORDSET * BtlTower_SetNgPokeName(SAVEDATA* sv,u16 pokenum,u16 sex,u8 flag,u8* num)
{
  u8  i;
  u16  mons;
  STRBUF *buf,*nbuf;
  ZUKAN_WORK* zukan;
  WORDSET*  wset;
  MSGDATA_MANAGER* man;

  buf = STRBUF_Create( BUFLEN_POKEMON_NAME+BUFLEN_EOM_SIZE, HEAPID_FIELD );
  nbuf = STRBUF_Create( 2, HEAPID_FIELD );
  zukan = SaveData_GetZukanWork(sv);

  man = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, NARC_msg_monsname_dat, HEAPID_FIELD );
  wset = WORDSET_CreateEx(BSUBWAY_EXPOKE_NUM+1,BUFLEN_POKEMON_NAME+BUFLEN_EOM_SIZE,HEAPID_FIELD);

  WORDSET_RegisterNumber( wset,0,pokenum,1,NUMBER_DISPTYPE_LEFT,NUMBER_CODETYPE_DEFAULT);

  for(i = 0;i < BSUBWAY_EXPOKE_NUM;i++){
    mons = BattleTowerExPoke_MonsNoGet(i);
    if(ZukanWork_GetPokeSeeFlag(zukan,mons)){
      MSGMAN_GetString(man,mons,buf);
      //�P��Z�b�g
      //�o�b�t�@ID
      //������
      //���ʃR�[�h
      //�P�^���iTRUE�ŒP���j
      //����R�[�h
      WORDSET_RegisterWord( wset,(*num)+1,buf,sex,flag,PM_LANG );
      (*num)++;
    }
  }

  MSGMAN_Delete(man);
  STRBUF_Delete( nbuf );
  STRBUF_Delete( buf );

  return wset;
}
#endif

//--------------------------------------------------------------
/**
 * �����擾
 * @param bsw_scr BSUBWAY_SCRWORK
 * @retval u16 ����
 * @note �����Œ�̖��c
 */
//--------------------------------------------------------------
static u16 get_Rand( BSUBWAY_SCRWORK *wk )
{
  return( GFUser_GetPublicRand(0xffffffff)/65535 );
}

//======================================================================
//  wb null
//======================================================================
#if 0
static void btwoer_TrainerDataMake(
    u8 play_mode,BSUBWAY_TRAINER* src,BSP_TRAINER_DATA * dest, HEAPID heapID )
{
  MI_CpuClear8(dest,sizeof(BSP_TRAINER_DATA));

  dest->tr_type = src->tr_type;  //�g���[�i�[�^�C�v
//dest->tr_gra = 0;//src->tr_type;  //�g���[�i�[�^�C�v
  dest->ai_bit = 0xFFFFFFFF;  //�ŋ�

  //�g���[�i�[��
  dest->name = GFL_STR_CreateBuffer( PERSON_NAME_SIZE+EOM_SIZE,

//  GFL_STR_CopyBuffer(
  PM_strcpy(dest->name,src->name);

  //�����������b�Z�[�W
  MI_CpuCopy8(src->win_word,&dest->win_word,sizeof(PMS_DATA));
  MI_CpuCopy8(src->lose_word,&dest->lose_word,sizeof(PMS_DATA));

  switch(play_mode){
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
    dest->fight_type = BTL_RULE_SINGLE;
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    dest->fight_type = BTL_RULE_DOUBLE;
    break;
  }
}
#endif

/**
 *  @brief  �g���[�i�[�ΐ�f�[�^����
 */
#if 0
static void btltower_SetTrainerData(BSUBWAY_SCRWORK* wk,BATTLE_PARAM* bp,
    BSUBWAY_PARTNER_DATA* tr,u8 tr_id,u8 tr_client,u8 poke_client )
{
  int i;
  POKEMON_PARAM *pp;

  //�g���[�i�[�f�[�^���Z�b�g
  btower_TrainerDataMake(
      wk->play_mode,&(tr->bt_trd),&(bp->trainer_data[tr_client]));

  //�g���[�i�[ID����
  bp->trainer_id[tr_client] = tr_id;//wk->tr_data[0].bt_trd.tr_type;

  //�ΐ푊��̃|�P�����f�[�^���Z�b�g
  pp = GFL_HEAP_AllocClearMemory( wk->heapID, POKETOOL_GetWorkSize() );

  PokeParty_Init(bp->poke_party[poke_client],wk->member_num);

  for(i = 0;i < wk->member_num;i++){
    make_PokePara(&(tr->btpwd[i]),pp);
    #if 0
    BattleParam_AddPokemon(bp,pp,poke_client);
    #else
    {
      int result;
      result = PokeParty_Add(bp->poke_party[client_no], pp);
    }
    #endif
  }
  GFL_HEAP_FreeMemory( pp );
}
#endif

