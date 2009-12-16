//======================================================================
/**
 * @file bsubway_scr.c
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

#include "fldmmdl.h"

#include "message.naix"

#include "bsubway_scr_def.h"
#include "bsubway_scr.h"
#include "bsubway_scr_common.h"
#include "savedata/bsubway_savedata.h"

#include "../mystery/fntequ.h"

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
static BOOL BattleTowerPokemonSetAct(
    BSUBWAY_SCRWORK* wk,BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no,BSUBWAY_POKEMON *pwd,u8 cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM *poke, HEAPID heapID );
static void * BattleTowerTrainerRomDataGet(u16 tr_no,HEAPID heapID);
static void BattleTowerPokemonRomDataGet(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index);

static STRCODE * PM_strcpy( STRCODE* to_str, const STRCODE* from_str );

//======================================================================
//  �g���[�i�[�^�C�v
//======================================================================
//--------------------------------------------------------------
/// �^���[�ɏo������g���[�i�[�^�C�v����OBJ�R�[�h
//--------------------------------------------------------------
static const u16 btower_trtype2objcode[][2] = {
 {TRTYPE_TANPAN,  BOY2},  ///<����p��������
 {TRTYPE_MINI,  GIRL1},  ///<�~�j�X�J�[�g
 {TRTYPE_SCHOOLB,  BOY1},  ///<���キ������
// {TRTYPE_PL_SCHOOLG,  GIRL3},  ///<���キ������
// {TRTYPE_PL_PRINCE,  GORGGEOUSM},  ///<���ڂ������
// {TRTYPE_PL_PRINCESS,  GORGGEOUSW},  ///<�����傤����
// {TRTYPE_CAMPB,  CAMPBOY},  ///<�L�����v�{�[�C
// {TRTYPE_PICNICG,  PICNICGIRL},  ///<�s�N�j�b�N�K�[��
// {TRTYPE_PL_UKIWAB,  BABYBOY1},  ///<������{�[�C
// {TRTYPE_PL_UKIWAG,  BABYGIRL1},  ///<������K�[��
 {TRTYPE_DAISUKIM,  MIDDLEMAN1},  ///<���������N���u
 {TRTYPE_DAISUKIW,  MIDDLEWOMAN1},  ///<���������N���u
// {TRTYPE_PL_WAITER,  WAITER},  ///<�E�G�[�^�[
// {TRTYPE_PL_WAITRESS,  WAITRESS},  ///<�E�G�[�g���X
// {TRTYPE_PL_BREEDERM,  MAN1},  ///<�|�P�����u���[�_�[
// {TRTYPE_PL_BREEDERW,  WOMAN1},  ///<�|�P�����u���[�_�[
// {TRTYPE_PL_CAMERAMAN,  CAMERAMAN},  ///<�J�����}��
// {TRTYPE_PL_REPORTER,  REPORTER},  ///<���|�[�^�[
// {TRTYPE_PL_FARMER,  FARMER},  ///<�ڂ����傤��������
// {TRTYPE_PL_COWGIRL,  COWGIRL},  ///<�J�E�K�[��
// {TRTYPE_PL_CYCLINGM,  CYCLEM},  ///<�T�C�N�����O��
// {TRTYPE_PL_CYCLINGW,  CYCLEW},  ///<�T�C�N�����O��
 {TRTYPE_KARATE,  FIGHTERM},  ///<����Ă���
// {TRTYPE_PL_BATTLEG,  GIRL2},  ///<�o�g���K�[��
// {TRTYPE_PL_VETERAN,  OLDMAN1},  ///<�x�e�����g���[�i�[
// {TRTYPE_PL_MADAM,  LADY},  ///<�}�_��
 {TRTYPE_ESPM,  MAN1},  ///<�T�C�L�b�J�[
// {TRTYPE_PL_ESPW,  MYSTERY},  ///<�T�C�L�b�J�[
// {TRTYPE_PL_RANGERM,  MAN3},  ///<�|�P���������W���[
// {TRTYPE_PL_RANGERW,  WOMAN3},  ///<�|�P���������W���[
// {TRTYPE_ELITEM,  MAN3},  ///<�G���[�g�g���[�i�[
// {TRTYPE_ELITEW,  WOMAN3},  ///<�G���[�g�g���[�i�[
// {TRTYPE_PL_COLDELITEM,  MAN5},  ///<�G���[�g�g���[�i�[���i�����j
// {TRTYPE_PL_COLDELITEW,  WOMAN5},  ///<�G���[�g�g���[�i�[���i�����j
// {TRTYPE_PL_DRAGON,  MAN3},  ///<�h���S������
// {TRTYPE_MUSHI,  BOY3},  ///<�ނ��Ƃ肵�傤�˂�
// {TRTYPE_PL_SHINOBI,  BABYBOY1},  ///<�ɂ񂶂Ⴒ����
// {TRTYPE_PL_JOGGER,  SPORTSMAN},  ///<�W���M���O��
 {TRTYPE_FISHING,  FISHING},  ///<��т�
// {TRTYPE_SAILOR,  SEAMAN},  ///<�ӂȂ̂�
 {TRTYPE_MOUNT,  MAN1},  ///<��܂��Ƃ�
// {TRTYPE_PL_ISEKI,  EXPLORE},  ///<�������}�j�A
// {TRTYPE_GUITARIST,  MAN2},  ///<�M�^���X�g
// {TRTYPE_PL_COLLECTOR,  BIGMAN},  ///<�|�P�����R���N�^�[
// {TRTYPE_PL_HEADS,  BADMAN},  ///<�X�L���w�b�Y
// {TRTYPE_SCIENTIST,  ASSISTANTM},  ///<���񂫂イ����
 {TRTYPE_GENTLE,  GENTLEMAN},  ///<�W�F���g���}��
// {TRTYPE_PL_WORKER,  WORKMAN},  ///<�����傤����
// {TRTYPE_PL_PIERROT,  CLOWN},  ///<�s�G��
// {TRTYPE_POLICE,  POLICEMAN},  ///<���܂�肳��
// {TRTYPE_PL_GAMBLER,  GORGGEOUSM},  ///<�M�����u���[
// {TRTYPE_BIRD,  WOMAN3},  ///<�Ƃ����
// {TRTYPE_PL_PARASOL,  AMBRELLA},  ///<�p���\�����˂�����
// {TRTYPE_SISTER,  WOMAN2},  ///<���ƂȂ̂��˂�����
// {TRTYPE_PL_AROMA,  WOMAN1},  ///<�A���}�Ȃ��˂�����
// {TRTYPE_PL_IDOL,  IDOL},  ///<�A�C�h��
// {TRTYPE_PL_ARTIST,  ARTIST},  ///<���������
// {TRTYPE_PL_POKEGIRL,  PIKACHU},  ///<�|�P������������
};

#define TRTYPE2OBJCODE_MAX  (NELEMS(btower_trtype2objcode))


/**
 *  @brief  �g���[�i�[�^�C�v����l��OBJ�R�[�h��Ԃ�
 */
u16 BtlTower_TrType2ObjCode(u8 tr_type)
{
  int i = 0;

  for(i = 0;i < TRTYPE2OBJCODE_MAX;i++){
    if(btower_trtype2objcode[i][0] == tr_type){
      return btower_trtype2objcode[i][1];
    }
  }
  return BOY1;  
}

static BOOL BtlTower_CheckTrType(u8 tr_type)
{
  int i = 0;

  for(i = 0;i < TRTYPE2OBJCODE_MAX;i++){
    if(btower_trtype2objcode[i][0] == tr_type){
      return( TRUE );
    }
  }
  return( FALSE );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  �o�g���^���[�p�|�P�����f�[�^����POKEMON_PARAM�𐶐�
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_PokeParaMake
 */
//-----------------------------------------------------------------------------
static void BtlTower_PokeParaMake(
    const BSUBWAY_POKEMON *src, POKEMON_PARAM *dest)
{
  int i;
  u8  buf8,waza_pp;
  u16  buf16;
  u32  buf32;
  
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
  
  //NG�l�[���t���O���`�F�b�N
  #if 0
  if( src->ngname_f ){
    //�f�t�H���g�l�[����W�J����
    GFL_MSGDATA *msgdata;
    STRBUF *def_name;

    msgdata= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
        ARCID_MESSAGE, NARC_message_monsname_dat, HEAPID_PROC );
    def_name = GFL_STR_CreateBuffer( HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );
    
    GFL_MSG_GetString( msgdata, src->mons_no, def_name );
    PP_Put( dest, ID_PARA_nickname, (u32)def_name );

    GFL_STR_DeleteBuffer( def_name );
    GFL_MSG_Delete( msgdata );
  }else{
    //�j�b�N�l�[��
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );
	  GFL_STR_SetStringCode( nick_name, src->nickname );
    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }
#else
  if( 1 ){
#if 0
    //�f�t�H���g�l�[����W�J����
    GFL_MSGDATA *msgdata;
    STRBUF *def_name;

    msgdata= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
        ARCID_MESSAGE, NARC_message_monsname_dat, HEAPID_PROC );
    def_name = GFL_STR_CreateBuffer( HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );
    
    GFL_MSG_GetString( msgdata, src->mons_no, def_name );
    PP_Put( dest, ID_PARA_nickname, (u32)def_name );
    
    GFL_STR_DeleteBuffer( def_name );
    GFL_MSG_Delete( msgdata );
#endif
  }else{
    //�j�b�N�l�[��
    STRBUF *nick_name;
    nick_name = GFL_STR_CreateBuffer(
        HEAPID_PROC, MONS_NAME_SIZE+EOM_SIZE );
	  GFL_STR_SetStringCode( nick_name, src->nickname );
    PP_Put( dest, ID_PARA_nickname, (u32)nick_name );
    GFL_STR_DeleteBuffer( nick_name );
  }
#endif

  //�J���g���[�R�[�h
  PP_Put(dest,ID_PARA_country_code,src->country_code);
  
  //�p�����[�^�Čv�Z
  {
    u32 lv = PP_CalcLevel( dest );
    PP_Put( dest, ID_PARA_level,lv );
  }
}

/**
 * @brief  �o�g���^���[�@�v���C���[�h����FIGHT_TYPE��Ԃ�
 */
static u32 btower_GetFightType(u8 play_mode)
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

/**
 *  @brief  �o�g���^���[�@�g���[�i�[�f�[�^����
 */



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
    BtlTower_PokeParaMake(&(tr->btpwd[i]),pp);
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

static BSP_TRAINER_DATA * BSP_TRAINER_DATA_Create( HEAPID heapID )
{
  BSP_TRAINER_DATA* tr_data = GFL_HEAP_AllocClearMemory( heapID, sizeof( BSP_TRAINER_DATA ) );
  tr_data->name =   GFL_STR_CreateBuffer( BUFLEN_PERSON_NAME, heapID );
  return tr_data;
}

//BATTLE_SETUP_PARAM
BATTLE_SETUP_PARAM * BtlTower_CreateBattleParam(
    BSUBWAY_SCRWORK *wk, GAMESYS_WORK *gsys )
{
#if 0
	BATTLE_SETUP_PARAM *dst;
  BTL_FIELD_SITUATION sit;
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( gsys );
  
	dst = BATTLE_PARAM_Create( HEAPID_PROC );
  
  {
    sit.bgType = BATTLE_BG_TYPE_ROOM;
    sit.bgAttr = BATTLE_BG_ATTR_LAWN;
    sit.weather = BTL_WEATHER_NONE;
    sit.timeZone = TIMEZONE_NOON;
    sit.season = 0;
    
    dst->netHandle = NULL;
    dst->commMode = BTL_COMM_NONE;
    dst->commPos = 0;
    dst->netID = 0;
    dst->multiMode = 0;
    dst->recBuffer = NULL;
    dst->fRecordPlay = FALSE;
    
    dst->partyPlayer = NULL;
    dst->partyEnemy1 = NULL;
    dst->partyPartner = NULL;
    dst->partyEnemy2 = NULL;
    
    dst->competitor = BTL_COMPETITOR_TRAINER;
    
    dst->statusPlayer = GAMEDATA_GetMyStatus( gameData );
    dst->itemData     = GAMEDATA_GetMyItem( gameData );
    dst->bagCursor    = GAMEDATA_GetBagCursor( gameData );
    dst->zukanData    = GAMEDATA_GetZukanSave( gameData );
    
    {
      SAVE_CONTROL_WORK *saveCtrl = GAMEDATA_GetSaveControlWork( gameData );
      dst->configData = SaveData_GetConfig( saveCtrl );
    }
    
    MI_CpuCopy8( &sit, &dst->fieldSituation, sizeof(BTL_FIELD_SITUATION) );
  }
  
  BTL_SETUP_SetSubwayMode( dst );

  switch( wk->play_mode )
  {
  case BSWAY_MODE_SINGLE:
  case BSWAY_MODE_WIFI:
    dst->rule = BTL_RULE_SINGLE;
    break;
  case BSWAY_MODE_DOUBLE:
  case BSWAY_MODE_MULTI:
  case BSWAY_MODE_COMM_MULTI:
  case BSWAY_MODE_WIFI_MULTI:
    dst->rule = BTL_RULE_DOUBLE;
    break;  
  }
 
  { //�g���[�i�[�f�[�^�m��
    dst->tr_data[BTL_CLIENT_PLAYER] = BSP_TRAINER_DATA_Create( HEAPID_PROC );
    dst->tr_data[BTL_CLIENT_ENEMY1] = BSP_TRAINER_DATA_Create( HEAPID_PROC );
  }
  
  { //�G�g���[�i�[�Z�b�g
    BTL_CLIENT_ID client = BTL_CLIENT_ENEMY1;
    POKEPARTY **party = &dst->partyEnemy1;
    BSP_TRAINER_DATA *data = dst->tr_data[client];
    BSUBWAY_PARTNER_DATA *b_tr_data = &wk->tr_data[0];
    BSUBWAY_TRAINER *bt_trd = &b_tr_data->bt_trd;
    
    data->tr_id = 0;
    data->tr_type = bt_trd->tr_type;
    data->ai_bit = 0xFFFFFFFF;  //�ŋ�
    
    #if 1
    {
#if 0
      GFL_MSGDATA *msgdata;
      STRBUF *name;
      
      msgdata =  GFL_MSG_Create(
        GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
        NARC_message_btdtrname_dat, HEAPID_PROC );
      name = GFL_MSG_CreateString( msgdata, bt_trd->player_id );
     
	    GFL_STR_CopyBuffer( name, data->name );
      
      GFL_STR_DeleteBuffer(name);
      GFL_MSG_Delete( msgdata );
#endif
    }
    #else
    GFL_STR_SetStringCode( data->name, bt_trd->name );
    #endif
    
    MI_CpuCopy8( bt_trd->win_word, &data->win_word,sizeof(PMS_DATA));
    MI_CpuCopy8( bt_trd->lose_word, &data->lose_word,sizeof(PMS_DATA));

    //�G�|�P�����Z�b�g
    {
      int i;
	    POKEMON_PARAM*  pp;
      
	    pp = GFL_HEAP_AllocMemoryLo( HEAPID_PROC, POKETOOL_GetWorkSize() );
      *party = PokeParty_AllocPartyWork( HEAPID_PROC );
      PokeParty_Init( *party, TEMOTI_POKEMAX );
      
      for( i = 0; i < wk->member_num; i++ ){
        BtlTower_PokeParaMake( &(b_tr_data->btpwd[i]), pp );
        PokeParty_Add( *party, pp );
      }
      GFL_HEAP_FreeMemory( pp );
    }
  }

#if 0  
  BTL_MAIN_GetClientPlayerData 
  registerWords
#endif

  { //�v���C���[�Z�b�g
    BTL_CLIENT_ID client = BTL_CLIENT_PLAYER;
    POKEPARTY **party = &dst->partyPlayer;
    BSP_TRAINER_DATA *data = dst->tr_data[client];
    PLAYER_WORK * player = GAMEDATA_GetMyPlayerWork( gameData );
    
    MyStatus_CopyNameString(
        (const MYSTATUS*)&player->mystatus, data->name );
    
    data->tr_type = TRTYPE_HERO +
      MyStatus_GetMySex((const MYSTATUS*)&player->mystatus );
    
    //�|�P�����Z�b�g
    {
      int i;
	    const POKEMON_PARAM *pp;
      const POKEPARTY *myparty =  GAMEDATA_GetMyPokemon(gameData);
      
      *party = PokeParty_AllocPartyWork( HEAPID_PROC );
      PokeParty_Init( *party, TEMOTI_POKEMAX );
      
      for( i = 0; i < wk->member_num; i++ ){
        pp = PokeParty_GetMemberPointer( myparty,
            wk->member[i] );
        PokeParty_Add( *party, pp );
      }
    }
  }

#if 0
  dst->musicDefault = SEQ_BGM_VS_NORAPOKE;
  dst->musicPinch = SEQ_BGM_BATTLEPINCH;

  dst->result = BTL_RESULT_WIN;

	sv = fsys->savedata;
	party = SaveData_GetTemotiPokemon(sv);
	
  BattleParam_SetParamByGameDataCore(bp,fsys);
	bp->bg_id = BG_ID_ROOM_A;			//��{�w�i�w��
	bp->ground_id = GROUND_ID_FLOOR;	//��{�n�ʎw��

	//�|�P�����f�[�^�Z�b�g
	pp = PokemonParam_AllocWork(wk->heapID);
	
	//�I�񂾎莝���|�P�������Z�b�g
	val8 = 50;
	PokeParty_Init(bp->poke_party[POKEPARTY_MINE],wk->member_num);
	for(i = 0;i < wk->member_num;i++){
		PokeCopyPPtoPP(PokeParty_GetMemberPointer(party,wk->member[i]),pp);

		//���x������
		if(PokeParaGet(pp,ID_PARA_level,NULL) > val8){
			val32 = PokeLevelExpGet(PokeParaGet(pp,ID_PARA_monsno,NULL),val8);

			PokeParaPut(pp,ID_PARA_exp,&val32);
			PokeParaCalc(pp);
		}
		BattleParam_AddPokemon(bp,pp,POKEPARTY_MINE);
	}
	sys_FreeMemoryEz(pp);

	//�g���[�i�[�f�[�^�����i�������j
	BattleParam_TrainerDataMake(bp);

	//�g���[�i�[�f�[�^(enemy1)���Z�b�g
	BattleTowerTrainerDataMake(bp,&(wk->tr_data[0]),wk->member_num,CLIENT_NO_ENEMY,wk->heapID);
	
	switch(wk->play_mode){
	case BTWR_MODE_MULTI:
		//�y�A�f�[�^���Z�b�g
		BattleTowerTrainerDataMake(bp,&(wk->five_data[wk->partner]),wk->member_num,CLIENT_NO_MINE2,wk->heapID);
		//�������͋��ʏ����ŗ���Ă���
	case BTWR_MODE_COMM_MULTI:
		//�g���[�i�[�f�[�^(enemy2)���Z�b�g
		BattleTowerTrainerDataMake(bp,&(wk->tr_data[1]),wk->member_num,CLIENT_NO_ENEMY2,wk->heapID);
		break;
	default:
		break;
	}
#endif
	return dst;
#else
  return NULL;
#endif
}

//============================================================================
/**
 *  �ʐM�֘A�R�}���h
 */
//============================================================================
#if 0
/**
 *  @brief  �o�g���^���[�@�����Ă����v���C���[�f�[�^���󂯎��
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
 *  @brief  �o�g���^���[�@�����Ă����g���[�i�[�f�[�^���󂯎��
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
 *  @brief  �o�g���^���[�@�����Ă������^�C�A���邩�ǂ����̌��ʂ��󂯎��
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
 *  @brief  �o�g���^���[�@���@���ʂƃ����X�^�[No�𑗐M
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
 *  @brief  �o�g���^���[�@�ʐM�}���`�@���I�����g���[�i�[No���q�@�ɑ��M
 */
void BTowerComm_SendTrainerData(BSUBWAY_SCRWORK* wk)
{
  MI_CpuCopy8(wk->trainer,wk->send_buf,BSUBWAY_STOCK_TRAINER_MAX*2);
}

/**
 *  @brief  �o�g���^���[�@�ʐM�}���`�@���^�C�A���邩�ǂ������݂��ɑ��M
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

//============================================================================================
/**
 *  �o�g���^���[�g���[�i�[No�擾�i�����f�[�^�̂��߂̃g���[�i�[ID�擾�j
 *
 * @param[in]  stage    ����
 * @param[in]  round    ���l��
 * @param[in]  play_mode  �ΐ탂�[�h
 *
 * @retval  trainer_id
 */
//============================================================================================
static const u16 TrainerNoRangeTable[][2]={
  {  1-1,100-1},
  { 81-1,120-1},
  {101-1,140-1},
  {121-1,160-1},
  {141-1,180-1},
  {161-1,200-1},
  {181-1,220-1},
  {201-1,300-1},
};

static const u16 TrainerNoRangeTable2[][2]={
  {101-1,120-1},
  {121-1,140-1},
  {141-1,160-1},
  {161-1,180-1},
  {181-1,200-1},
  {201-1,220-1},
  {221-1,240-1},
  {201-1,300-1},
};

#define  TOWER_MASTER_FIRST  (305)    //0�I���W��
#define  TOWER_MASTER_SECOND  (306)

static u16 btower_rand( BSUBWAY_SCRWORK *wk )
{
#if 0
  //�v���`�i�Œǉ����ꂽWIFI�}���`�͒ʏ�̃����_�����g�p
  if( wk->play_mode == BTWR_MODE_WIFI_MULTI ){
    return ( gf_rand() );
  }

  wk->play_rnd_seed = BtlTower_PlayFixRand(wk->play_rnd_seed);

  OS_Printf("btower_rand = %d\n",wk->play_rnd_seed);
  return (wk->play_rnd_seed/65535);
#else
  return( GFUser_GetPublicRand(0xffffffff)/65535 );
#endif
}

/**
 *  @brief  �g���[�i�[�i���o�[���I
 */
u16  BattleTowerTrainerNoGet(
    BSUBWAY_SCRWORK* wk,u8 stage,u8 round,int play_mode)
{
  u16  no;

  OS_Printf( "stage = %d\n", stage );
  OS_Printf( "round = %d\n", round );

  //�^���[�^�C�N�[���̓V���O���̂�
  if(play_mode==BSWAY_MODE_SINGLE){
    //�^���[�^�C�N�[��1���
    if((stage==2)&&(round==6)){
      return TOWER_MASTER_FIRST;
    }
    //�^���[�^�C�N�[��2���
    if((stage==6)&&(round==6)){
      return TOWER_MASTER_SECOND;
    }
  }

  if(stage<7){
    if(round==(7-1)){
      no=(TrainerNoRangeTable2[stage][1]-TrainerNoRangeTable2[stage][0])+1;
//      no=TrainerNoRangeTable2[stage][0]+(gf_rand()%no);
      no=TrainerNoRangeTable2[stage][0]+(btower_rand(wk)%no);
    }
    else{
      no=(TrainerNoRangeTable[stage][1]-TrainerNoRangeTable[stage][0])+1;
//      no=TrainerNoRangeTable[stage][0]+(gf_rand()%no);
      no=TrainerNoRangeTable[stage][0]+(btower_rand(wk)%no);
    }
  }
  else{
    no=(TrainerNoRangeTable[7][1]-TrainerNoRangeTable[7][0])+1;
//    no=TrainerNoRangeTable[7][0]+(gf_rand()%no);
    no=TrainerNoRangeTable[7][0]+(btower_rand(wk)%no);
  }
  return no;
}

//============================================================================================
/**
 *  �o�g���^���[�@rom�g���[�i�[�f�[�^�W�J
 *
 *  BSUBWAY_TRAINER_ROM_DATA�^���������m�ۂ��ĕԂ��̂ŁA
 *  �Ăяo�����������I�ɉ�����邱��
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_TrainerDataGet
 */
//============================================================================================
static BSUBWAY_TRAINER_ROM_DATA * RomTrainerDataAlloc(
    BSUBWAY_PARTNER_DATA *tr_data,u16 tr_no,HEAPID heapID)
{
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  GFL_MSGDATA *msgdata;
  STRBUF *name;
  
  msgdata =  GFL_MSG_Create(
      GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_btdtrname_dat, heapID );
  
  MI_CpuClear8(tr_data, sizeof(BSUBWAY_PARTNER_DATA));
  trd=BattleTowerTrainerRomDataGet(tr_no,heapID);
  
  //�g���[�i�[ID���Z�b�g
  tr_data->bt_trd.player_id=tr_no;
  
  //�g���[�i�[�o�����b�Z�[�W
  tr_data->bt_trd.appear_word[0] = 0xFFFF;
  tr_data->bt_trd.appear_word[1] = tr_no*3;
  
  //�g���[�i�[�f�[�^���Z�b�g
  tr_data->bt_trd.tr_type=trd->tr_type;

  //GS�f�[�^����̈ڐA�ɂ�鏈��
  //wb�ł͑��݂��Ă��Ȃ��^�C�v����������
  #if 1
  if( BtlTower_CheckTrType( trd->tr_type ) == FALSE ){
    tr_data->bt_trd.tr_type = TRTYPE_TANPAN;
  }
  #endif
  
  name = GFL_MSG_CreateString( msgdata, tr_no );
  GFL_STR_GetStringCode( name,
      &tr_data->bt_trd.name[0], BUFLEN_PERSON_NAME );
  GFL_STR_DeleteBuffer(name);
  GFL_MSG_Delete( msgdata );
  return trd;
}

//�����|�P��������̓����_���ł��Ă��邪�������[�v�h�~�̂��߁A
//������x�܂킵����A�|�P�����̕s��v�݂̂��`�F�b�N���āA
//�A�C�e�����Œ�Ŏ������邽�߂̃A�C�e���e�[�u��
static const u16 BattleTowerPokemonItem[]={
  ITEM_HIKARINOKONA,
  ITEM_RAMUNOMI,
  ITEM_TABENOKOSI,
  ITEM_SENSEINOTUME,
};

//============================================================================================
/**
 *	�|�P�����̐��i���擾
 *
 *	���i�́A��������25�Ŋ������]�肩��Z�o�����
 *
 * @param[in]	rnd	�擾������������
 *
 * @return	�擾�������i
 */
//============================================================================================
static u8	PokeSeikakuGetRnd(u32 rnd)
{
	return(u8)(rnd%25);
}

//============================================================================================
/**
 *  @brief  �o�g���^���[�̃|�P�����p�����[�^����
 *
 *  @param[in/out]  pwd  �|�P�����p�����[�^�̓W�J�ꏊ
 *  @param[in]    poke_no  �^���[rom�f�[�^�|�P�����i���o�[
 *  @param[in]    poke_id  �|�P�����ɃZ�b�g����id
 *  @param[in]    poke_rnd  �|�P�����ɃZ�b�g���������(0�������n���ꂽ��֐����Ő���)
 *  @param[in]    pow_rnd  �|�P�����ɃZ�b�g����pow_rnd�l
 *  @param[in]    mem_idx  �����o�[index�B��̖�or��̖�
 *  @param[in]    itemfix  TRUE�Ȃ�Œ�A�C�e���BFALSE�Ȃ�rom�f�[�^�̃A�C�e��
 *  @param[in]    heapID  �e���|�������������m�ۂ���q�[�vID
 *
 *  @return  personal_rnd:�������ꂽ�|�P�����̌������l
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY ��������frontier_tool.c Frontier_PokemonParamMake
 */
//============================================================================================
static u32 BattleTowerPokemonParamMake(
    BSUBWAY_SCRWORK* wk,BSUBWAY_POKEMON* pwd,
    u16 poke_no,u32 poke_id,u32 poke_rnd,u8 pow_rnd,
    u8 mem_idx,BOOL itemfix,HEAPID heapID)
{
  int i;
  int  exp;
  u32  personal_rnd;
  u8  friend;
  BSUBWAY_POKEMON_ROM_DATA  prd_s;
  
  MI_CpuClear8(pwd,sizeof(BSUBWAY_POKEMON));
  
  //�����f�[�^���[�h
  BattleTowerPokemonRomDataGet(&prd_s,poke_no);
  
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
      personal_rnd=(btower_rand(wk)|btower_rand(wk)<<16);
#if 0
    }while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))&&(PokeRareGetPara(poke_id,personal_rnd)==TRUE));
#else
    //�v���`�i�̓^���[���C������(08.03.17)(����������frontier_tool.c�ɂ�����̂Œ��ӁI)
    //�f�[�^�̐��i�ƈ�v���Ă��Ȃ�"��������"���A�̎��́A���[�v����
    }while((prd_s.chr!=PokeSeikakuGetRnd(personal_rnd))||(
          POKETOOL_CheckRare(poke_id,personal_rnd)==TRUE));
#endif
    //OS_Printf( "���肵��personal_rnd = %d\n", personal_rnd );
    //OS_Printf( "PokeSeikakuGetRnd = %d\n",PokeSeikakuGetRnd(personal_rnd) );
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

//============================================================================================
/**
 *  �o�g���^���[�g���[�i�[�f�[�^�����i�����f�[�^��BSUBWAY_PARTNER_DATA�\���̂ɓW�J�j
 *
 * @param[in/out]  tr_data    ��������BSUBWAY_PARTNAER_DATA�\����
 * @param[in]    tr_no    �������ɂȂ�g���[�i�[ID
 * @param[in]    cnt      �g���[�i�[�Ɏ�������|�P�����̐�
 * @param[in]    set_poke_no  �y�A��g��ł���g���[�i�[�̎����|�P�����iNULL���ƃ`�F�b�N�Ȃ��j
 * @param[in]    set_item_no  �y�A��g��ł���g���[�i�[�̎����|�P�����̑����A�C�e���iNULL���ƃ`�F�b�N�Ȃ��j
 * @param[in/out]  poke    ���I���ꂽ�|�P�����̓�̂̃p�����[�^���i�[���ĕԂ��\���̌^�f�[�^�ւ̃|�C���^(NULL���ƃ`�F�b�N�Ȃ��j
 * @param[in]    heapID    �q�[�vID
 *
 * @retval  FALSE  ���I���[�v��50��ȓ��ŏI�����
 * @retval  TRUE  ���I���[�v��50��ȓ��ŏI���Ȃ�����
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY ��������tower_tool.c FSRomBattleTowerTrainerDataMake
 * ��TOWER_AI_MULTI_ONLY �T�����ŁA5�l�O�̎莝���|�P�����𐶐����Ă���
 */
//============================================================================================
BOOL  RomBattleTowerTrainerDataMake(
    BSUBWAY_SCRWORK* wk,BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no,int cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  BOOL      ret = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;
  
  //�g���[�i�[�f�[�^�Z�b�g
  trd = RomTrainerDataAlloc(tr_data,tr_no,heapID);

  //�|�P�����f�[�^���Z�b�g
  ret = BattleTowerPokemonSetAct(
      wk,trd,tr_no,&tr_data->btpwd[0],cnt,
      set_poke_no,set_item_no,poke,heapID);
  GFL_HEAP_FreeMemory( trd );
  return ret;
}

//---------------------------------------------------------------------------------------------
/**
 * �o�g���^���[�g���[�i�[�̎����|�P�����̃p���[���������肷��
 *
 * @param	tr_no	�g���[�i�[�i���o�[
 *
 * @return	�p���[����
 *
 * b_tower_fld.c��b_tower_ev�Ɉړ�
 */
//---------------------------------------------------------------------------------------------
static u8	BattleTowerPowRndGet(u16 tr_no)
{
	u8	pow_rnd;

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

//============================================================================================
/**
 *  �o�g���^���[ �y�A�g���[�i�[�f�[�^�Đ���
 *  �i�Z�[�u���ꂽAI�}���`�p�[�g�i�[�̃f�[�^��BSUBWAY_PARTNER_DATA�\���̂ɓW�J�j
 *
 * @param[in/out]  tr_data    ��������BSUBWAY_PARTNAER_DATA�\����
 * @param[in]    tr_no    �������ɂȂ�g���[�i�[ID
 * @param[in]    fixitem    TURE�Ȃ�Œ�A�C�e�����AFALSE�Ȃ�rom�A�C�e�����Z�b�g����
 * @param[in]    poke    �|�P�����f�[�^�Đ����ɕK�v�ȍ\���̌^�f�[�^�ւ̃|�C���^
 * @param[in]    heapID    �q�[�vID
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY AI�}���`�ŋx�ނ̌�A�ĊJ�������ɌĂ΂��
 */
//============================================================================================
void RomBattleTowerPartnerDataMake(
    BSUBWAY_SCRWORK* wk,BSUBWAY_PARTNER_DATA *tr_data,
    u16 tr_no,BOOL itemfix,
    const BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
{
  int  i;
  u8  pow_rnd = 0;
  BSUBWAY_TRAINER_ROM_DATA  *trd;

  //�g���[�i�[�f�[�^�Z�b�g
  trd = RomTrainerDataAlloc(tr_data,tr_no,heapID);
  
  //�|�P�����f�[�^���Z�b�g
  pow_rnd=BattleTowerPowRndGet(tr_no);

  for(i = 0;i < BSUBWAY_STOCK_PAREPOKE_MAX;i++){
    BattleTowerPokemonParamMake(
      wk,&(tr_data->btpwd[i]),
      poke->poke_no[i],poke->poke_id,
      poke->poke_rnd[i],pow_rnd,i,itemfix,heapID);
  }
  GFL_HEAP_FreeMemory( trd );
}

//============================================================================================
/**
 *  �o�g���^���[�̃|�P���������߂�
 *
 * @param[in]    trd      �g���[�i�[�f�[�^
 * @param[in]    tr_no    �g���[�i�[�i���o�[
 * @param[in/out]  pwd      BSUBWAY_POKEMON�\����
 * @param[in]    cnt      �g���[�i�[�Ɏ�������|�P�����̐�
 * @param[in]    set_poke_no  �y�A��g��ł���g���[�i�[�̎����|�P�����iNULL���ƃ`�F�b�N�Ȃ��j
 * @param[in]    set_item_no  �y�A��g��ł���g���[�i�[�̎����|�P�����̑����A�C�e���iNULL���ƃ`�F�b�N�Ȃ��j
 * @param[in/out]  poke    ���I���ꂽ�|�P�����̓�̂̃p�����[�^���i�[���ĕԂ��\���̌^�f�[�^�ւ̃|�C���^(NULL���ƃ`�F�b�N�Ȃ��j
 * @param[in]    heapID    �q�[�vID
 *
 * @retval  FALSE  ���I���[�v��50��ȓ��ŏI�����
 * @retval  TRUE  ���I���[�v��50��ȓ��ŏI���Ȃ�����
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 */
//============================================================================================
static BOOL BattleTowerPokemonSetAct(
    BSUBWAY_SCRWORK* wk,BSUBWAY_TRAINER_ROM_DATA *trd,
    u16 tr_no,BSUBWAY_POKEMON *pwd,u8 cnt,
    u16 *set_poke_no,u16 *set_item_no,
    BSUBWAY_PAREPOKE_PARAM* poke,HEAPID heapID)
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
    poke_index = btower_rand(wk)%trd->use_poke_cnt;
    set_index=trd->use_poke_table[poke_index];
    BattleTowerPokemonRomDataGet(&prd_d,set_index);

    //�����X�^�[�i���o�[�̃`�F�b�N�i����̃|�P�����͎����Ȃ��j
    for(i=0;i<set_count;i++){
      BattleTowerPokemonRomDataGet(&prd_s,set_index_no[i]);
      if(prd_s.mons_no==prd_d.mons_no){
        break;
      }
    }
    if(i!=set_count){
      continue;
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
        BattleTowerPokemonRomDataGet(&prd_s,set_index_no[i]);
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

  pow_rnd=BattleTowerPowRndGet(tr_no);
//id=(gf_rand()|(gf_rand()<<16));
  id=(btower_rand(wk)|(btower_rand(wk)<<16));

  if(loop_count >= 50){
    ret = TRUE;
  }
  for(i=0;i<set_count;i++){
    set_rnd_no[i] = BattleTowerPokemonParamMake(wk,&(pwd[i]),
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

//---------------------------------------------------------------------------------------------
/**
 *  �o�g���^���[�g���[�i�[�����f�[�^�̓ǂݏo��
 *
 * @param[in]  tr_no  �g���[�i�[�i���o�[
 * @param[in]  heapID  �������m�ۂ��邽�߂̃q�[�vID
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_TrainerRomDataGet
 */
//---------------------------------------------------------------------------------------------
static void * BattleTowerTrainerRomDataGet(u16 tr_no,HEAPID heapID)
{
  //AI�}���`����Ȃ̂Ńv���`�i�I
  return GFL_ARC_UTIL_Load( ARCID_PL_BTD_TR, tr_no, 0, heapID );
}

//---------------------------------------------------------------------------------------------
/**
 *  �o�g���^���[�|�P���������f�[�^�̓ǂݏo��
 *
 * @param[in]  prd    �ǂݏo�����|�P�����f�[�^�̊i�[��
 * @param[in]  index  �ǂݏo���|�P�����f�[�^�̃C���f�b�N�X
 *
 * ��TOWER_AI_MULTI_ONLY �t�B�[���h��ŌĂ΂�鏈�� 
 * ��TOWER_AI_MULTI_ONLY �������� frontier_tool.c Frontier_PokemonRomDataGet
 */
//---------------------------------------------------------------------------------------------
static void BattleTowerPokemonRomDataGet(
    BSUBWAY_POKEMON_ROM_DATA *prd,int index)
{
  //�����͒ʐM�͂��肦�Ȃ��̂Ńv���`�i����I(AI�}���`)
	GFL_ARC_LoadData( (void*)prd, ARCID_PL_BTD_PM, index );
}

//======================================================================
//  POKEMON_PARAM
//======================================================================

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
  while( *from_str != EOM_ ){
    *to_str = *from_str;
    to_str++;
    from_str++;
  }
  *to_str = EOM_;
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


