//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		  fld_trade.c
 *	@brief		�Q�[��������
 *	@author		tomoya takahashi (obata)
 *	@data		  2006.05.15 (2009.12.09 HGSS���ڐA)
 *
 *	���̃\�[�X�� field_trade�I�[�o�[���C�̈�ɑ����Ă��܂�
 *	���̃\�[�X�ɒ�`���ꂽ�S�֐��� field_trade�I�[�o�[���C�̈�𖾎��I�Ƀ��[�h���Ă���łȂ���
 *	���p�ł��Ȃ��̂Œ��ӂ��Ă��������I
 *
 *	�I�[�o�[���C���[�h���A�����[�h�\��
 *
 *	#include "system/pm_overlay.h"
 *	
 *	FS_EXTERN_OVERLAY(field_trade);
 *	Overlay_Load( FS_OVERLAY_ID( field_trade ), OVERLAY_LOAD_NOT_SYNCHRONIZE );
 *	Overlay_UnloadID( FS_OVERLAY_ID( field_trade ) );
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>
#include "fld_trade_local.h"
#include "fld_trade.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/mystatus.h"
#include "gamesystem/game_event.h"
#include "fieldmap.h"
#include "arc/arc_def.h"  // for ARCID_MESSAGE, ARCID_FLD_TRADE_POKE
#include "arc/message.naix"  // for NARC_message_fld_trade_dat
#include "net_app/pokemontrade.h"  // for PokemonTradeDemoProcData
#include "event_fieldmap_control.h"  // for EVENT_FieldSubProc
#include "system/main.h"  // for HEAPID_PROC
#include "savedata/zukan_savedata.h"  // for ZUKANSAVE_xxxx
#include "../../../resource/fld_trade/fld_trade_list.h"  // for FLD_TRADE_POKE_xxxx
#include "poke_tool/shinka_check.h"
#include "demo/shinka_demo.h"
#include "poke_tool/poke_memo.h"

FS_EXTERN_OVERLAY(pokemon_trade);


//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
struct _FLD_TRADE_WORK 
{
	FLD_TRADE_POKEDATA* p_pokedata;		// �����|�P�����f�[�^
	POKEMON_PARAM*		        p_pp;		// �����|�P����
	MYSTATUS*			         p_myste;		// ��������p�����[�^
	u32                   trade_no;	  // �����i���o�[
	u32                     heapID;
};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static STRBUF* GetTradeMsgData( u32 heapID, u32 idx );

static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, u32 zone_id,
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 trade_no );

static void PP_Dump( const POKEMON_PARAM* pp );
static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp );


//----------------------------------------------------------------------------
/**
 *	@brief	���[�N�m��
 *
 *	@param	heap_id		�q�[�vID
 *	@param	trade_no	�����ԍ�
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
FLD_TRADE_WORK* FLD_TRADE_WORK_Create( u32 heap_id, u32 trade_no )
{
	FLD_TRADE_WORK* work;
	STRBUF* strbuf;
	STRCODE str_arry[ 128 ];

	GF_ASSERT( trade_no < FLD_TRADE_POKE_NUM );
	
	work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FLD_TRADE_WORK) );
	work->heapID   = heap_id;
	work->trade_no = trade_no;

	// �����f�[�^�ǂݍ���
  work->p_pokedata = GFL_ARC_LoadDataAllocOfs( ARCID_FLD_TRADE_POKE, trade_no, 
                                               heap_id, 0, sizeof(FLD_TRADE_POKEDATA) );
  // �����|�P�����f�[�^�쐬
	work->p_pp = GFL_HEAP_AllocMemory( heap_id, sizeof(POKEMON_PARAM) );

	// �e�f�[�^�쐬
	work->p_myste = MyStatus_AllocWork( heap_id );
	MyStatus_Init( work->p_myste );
	strbuf = GetTradeMsgData( heap_id, FLD_TRADE_GET_OYA_GMM( trade_no ) );
	GFL_STR_GetStringCode( strbuf, str_arry, 128 );
	GFL_STR_DeleteBuffer( strbuf );
	MyStatus_SetMyName( work->p_myste, str_arry );
	MyStatus_SetMySex( work->p_myste, work->p_pokedata->oya_sex );
	
	return work;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�N�j��
 *
 *	@param	work		
 */
//-----------------------------------------------------------------------------
void FLD_TRADE_WORK_Delete( FLD_TRADE_WORK* work )
{
	GFL_HEAP_FreeMemory( work->p_pokedata );
	GFL_HEAP_FreeMemory( work->p_pp );
	GFL_HEAP_FreeMemory( work->p_myste );
	GFL_HEAP_FreeMemory( work );
}

//----------------------------------------------------------------------------
/**
 *	@brief	��������|�P�����i���o�[�擾
 *
 *	@param	cwork		���[�N
 *
 *	@return	�|�P�����i���o�[
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetTradeMonsno( const FLD_TRADE_WORK* cwork )
{
	return cwork->p_pokedata->monsno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ɕK�v�ȃ|�P�����i���o�[�擾
 *
 *	@param	cwork		���[�N
 *
 *	@return	�|�P�����i���o�[
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetChangeMonsno( const FLD_TRADE_WORK* cwork )
{
	return cwork->p_pokedata->change_monsno;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�����ɕK�v�ȃ|�P�����̐��ʂ��擾
 *
 *	@param	cwork		���[�N
 *
 *	@return	�K�v�ȃ|�P�����̐��ʁi0:�I�X�@1:���X�@2:��킸�i�������͐��ʂȂ��j
 *          MONS_MALE�AMONS_FEMALE�AMONS_UNK�ɏ������Ă܂��B
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetChangeMonsSex( const FLD_TRADE_WORK* cwork )
{
	return cwork->p_pokedata->change_monssex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�e���ʁ@�擾
 *
 *	@param	cwork		���[�N
 *
 *	@return	����
 */
//-----------------------------------------------------------------------------
u32 FLD_TRADE_WORK_GetTradeMonsOyaSex( const FLD_TRADE_WORK* cwork )
{
	return cwork->p_pokedata->oya_sex;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�擾
 *	�j�����K�v
 */
//-----------------------------------------------------------------------------
static STRBUF* GetTradeMsgData( u32 heapID, u32 str_id )
{
	STRBUF* strbuf;
	
	GFL_MSGDATA* msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, 
                                     ARCID_MESSAGE, NARC_message_fld_trade_dat, 
                                     heapID );
	strbuf = GFL_MSG_CreateString( msg, str_id );
  GFL_MSG_Delete( msg );
	return strbuf;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����p�����Ƀf�[�^�̐ݒ�
 *
 *  @param  gameData
 *	@param	heapID		heapID
 *	@param	zoneID		�g���[�i�[�����ɋL�ڂ���]�[��ID
 *	@param	pp		    �|�P�����p����
 *	@param	data		  �f�[�^
 *	@param	trade_no	trade�i���o�[
 */
//-----------------------------------------------------------------------------
static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, u32 zone_id,
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 trade_no )
{
	STRBUF* strbuf;
	u32 placeid; 
  u32 personal_rnd;

  // �w��̐��ʂɂȂ�悤�Ȍ������𐶐�
  personal_rnd = POKETOOL_CalcPersonalRandEx( data->mons_id, data->monsno, data->formno, data->sex, 0, FALSE );
	
	// �����X�^�[�i���o�[�@���x���@�ő̗����@ID�ݒ�
	PP_SetupEx( pp, 
              data->monsno, 
              data->level, 
              data->mons_id, 
              PTL_SETUP_POW_AUTO, 
              personal_rnd );
  // �t�H�[��
  PP_Put( pp, ID_PARA_form_no, data->formno );

  PP_Dump( pp );

	// �j�b�N�l�[��
	strbuf = GetTradeMsgData( heapID, FLD_TRADE_GET_POKE_GMM(trade_no) );
	PP_Put( pp, ID_PARA_nickname, (u32)strbuf );
	GFL_STR_DeleteBuffer( strbuf );

	// �e�헐���ݒ�
	PP_Put( pp, ID_PARA_hp_rnd,     data->hp_rnd );
	PP_Put( pp, ID_PARA_pow_rnd,    data->at_rnd );
	PP_Put( pp, ID_PARA_def_rnd,    data->df_rnd );
	PP_Put( pp, ID_PARA_agi_rnd,    data->sp_rnd );
	PP_Put( pp, ID_PARA_spepow_rnd, data->sa_rnd );
	PP_Put( pp, ID_PARA_spedef_rnd, data->sd_rnd );

	// ����
  PP_Put( pp, ID_PARA_speabino, data->speabino );

  // ���i
  PP_Put( pp, ID_PARA_seikaku, data->seikaku );

	// �������悳�Ȃ�
	PP_Put( pp, ID_PARA_style,     data->style );
	PP_Put( pp, ID_PARA_beautiful, data->beautiful );
	PP_Put( pp, ID_PARA_cute,      data->cute );
	PP_Put( pp, ID_PARA_clever,    data->clever );
	PP_Put( pp, ID_PARA_strong,    data->strong );
	
	// �A�C�e��
	PP_Put( pp, ID_PARA_item, data->item );

	// �e�̖��O
	strbuf = GetTradeMsgData( heapID, FLD_TRADE_GET_OYA_GMM(trade_no) );
	PP_Put( pp, ID_PARA_oyaname, (u32)strbuf );
	GFL_STR_DeleteBuffer( strbuf );

	// �e�̐���	
	PP_Put( pp, ID_PARA_oyasex, data->oya_sex );

	// ���R�[�h
	PP_Put( pp, ID_PARA_country_code, data->world_code );

  // �g���[�i�[����
  {
    MYSTATUS* mystatus;
    mystatus = GAMEDATA_GetMyStatus( gameData );
    POKE_MEMO_SetTrainerMemoPP( 
        pp, POKE_MEMO_GAME_TRADE, mystatus, POKE_MEMO_PLACE_GAME_TRADE, heapID );
  }

  PP_Dump( pp );

	// �v�Z���Ȃ���
	PP_Renew( pp );

	// ���A�ɂȂ�Ȃ��悤�Ƀf�[�^�ݒ肳��Ă��Ȃ������ꍇNG
  GF_ASSERT( PP_CheckRare( pp ) == FALSE && "���A�|�P�ł��B");

  // ��������̎������Ȃ����̂�������NG
  {
    POKEMON_PERSONAL_DATA* ppd;
    u32 monsno, formno;
    u32 speabi;
    u32 sp1, sp2, sp3;
    monsno = PP_Get( pp, ID_PARA_monsno, NULL );
    formno = PP_Get( pp, ID_PARA_form_no, NULL );
    speabi = PP_Get( pp, ID_PARA_speabino, NULL );
    ppd = POKE_PERSONAL_OpenHandle( monsno, formno, heapID );
    sp1 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi1 );
    sp2 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi2 );
    sp3 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( ppd );
    // �s���ȓ��������o
    if( (speabi!=sp1)||(speabi!=sp2)||(speabi!=sp3) )
    {
      OBATA_Printf( "======================================\n" );
      OBATA_Printf( "�s���ȓ��������o���܂����B\n" );
      OBATA_Printf( "������������ %d or %d or %d �ł��B\n", sp1, sp2, sp3 );
      OBATA_Printf( "�����|�P�����̓����� %d �ɐݒ肵�܂��B\n", sp1 );
      OBATA_Printf( "======================================\n" );
      PP_Put( pp, ID_PARA_speabino, sp1 );
    }
  }
} 


static void PP_Dump( const POKEMON_PARAM* pp )
{
  u32 val; 
  OBATA_Printf( "---------------------------------------------- PP\n" );
  val = PP_Get( pp, ID_PARA_personal_rnd, NULL ); OBATA_Printf( "personal_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_monsno, NULL ); OBATA_Printf( "monsno = %d\n", val );
  val = PP_Get( pp, ID_PARA_form_no, NULL ); OBATA_Printf( "form_no = %d\n", val );
  val = PP_Get( pp, ID_PARA_item, NULL ); OBATA_Printf( "item = %d\n", val );
  val = PP_Get( pp, ID_PARA_id_no, NULL ); OBATA_Printf( "id_no = %d\n", val );
  val = PP_Get( pp, ID_PARA_exp, NULL ); OBATA_Printf( "exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_friend, NULL ); OBATA_Printf( "friend = %d\n", val );
  val = PP_Get( pp, ID_PARA_speabino, NULL ); OBATA_Printf( "speabino = %d\n", val );
  val = PP_Get( pp, ID_PARA_sex, NULL ); OBATA_Printf( "sex = %d\n", val );
  val = PP_Get( pp, ID_PARA_seikaku, NULL ); OBATA_Printf( "seikaku = %d\n", val );
  val = PP_Get( pp, ID_PARA_mark, NULL ); OBATA_Printf( "mark = %d\n", val );
  val = PP_Get( pp, ID_PARA_country_code, NULL ); OBATA_Printf( "country_code = %d\n", val );
  val = PP_Get( pp, ID_PARA_hp_exp, NULL ); OBATA_Printf( "hp_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_pow_exp, NULL ); OBATA_Printf( "pow_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_def_exp, NULL ); OBATA_Printf( "def_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_agi_exp, NULL ); OBATA_Printf( "agi_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_spepow_exp, NULL ); OBATA_Printf( "spepow_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_spedef_exp, NULL ); OBATA_Printf( "spedef_exp = %d\n", val );
  val = PP_Get( pp, ID_PARA_style, NULL ); OBATA_Printf( "style = %d\n", val );
  val = PP_Get( pp, ID_PARA_beautiful, NULL ); OBATA_Printf( "beautiful = %d\n", val );
  val = PP_Get( pp, ID_PARA_cute, NULL ); OBATA_Printf( "cute = %d\n", val );
  val = PP_Get( pp, ID_PARA_clever, NULL ); OBATA_Printf( "clever = %d\n", val );
  val = PP_Get( pp, ID_PARA_strong, NULL ); OBATA_Printf( "strong = %d\n", val );
  val = PP_Get( pp, ID_PARA_fur, NULL ); OBATA_Printf( "fur = %d\n", val );
  val = PP_Get( pp, ID_PARA_hp_rnd, NULL ); OBATA_Printf( "hp_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_pow_rnd, NULL ); OBATA_Printf( "pow_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_def_rnd, NULL ); OBATA_Printf( "def_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_agi_rnd, NULL ); OBATA_Printf( "agi_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_spepow_rnd, NULL ); OBATA_Printf( "spepow_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_spedef_rnd, NULL ); OBATA_Printf( "spedef_rnd = %d\n", val );
  val = PP_Get( pp, ID_PARA_oyasex, NULL ); OBATA_Printf( "oyasex = %d\n", val );
  val = PP_Get( pp, ID_PARA_level, NULL ); OBATA_Printf( "level = %d\n", val );
  val = PP_Get( pp, ID_PARA_get_place, NULL ); OBATA_Printf( "get_place = %d\n", val );
  OBATA_Printf( "-------------------------------------------------\n" );
}

static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp )
{
  OBATA_Printf( "--------------------------------------------- FTP\n" );
  OBATA_Printf( "monsno = %d\n", ftp->monsno );
  OBATA_Printf( "formno = %d\n", ftp->formno );
  OBATA_Printf( "level = %d\n", ftp->level );
  OBATA_Printf( "hp_rnd = %d\n", ftp->hp_rnd );
  OBATA_Printf( "at_rnd = %d\n", ftp->at_rnd );
  OBATA_Printf( "df_rnd = %d\n", ftp->df_rnd );
  OBATA_Printf( "sp_rnd = %d\n", ftp->sp_rnd );
  OBATA_Printf( "sa_rnd = %d\n", ftp->sa_rnd );
  OBATA_Printf( "sd_rnd = %d\n", ftp->sd_rnd );
  OBATA_Printf( "speabino = %d\n", ftp->speabino );
  OBATA_Printf( "seikaku = %d\n", ftp->seikaku );
  OBATA_Printf( "sex = %d\n", ftp->sex );
  OBATA_Printf( "mons_id = %d\n", ftp->mons_id );
  OBATA_Printf( "style = %d\n", ftp->style );
  OBATA_Printf( "beautiful = %d\n", ftp->beautiful );
  OBATA_Printf( "cute = %d\n", ftp->cute );
  OBATA_Printf( "clever = %d\n", ftp->clever );
  OBATA_Printf( "strong = %d\n", ftp->strong );
  OBATA_Printf( "item = %d\n", ftp->item );
  OBATA_Printf( "oya_sex = %d\n", ftp->oya_sex );
  OBATA_Printf( "fur = %d\n", ftp->fur );
  OBATA_Printf( "world_code = %d\n", ftp->world_code );
  OBATA_Printf( "change_monsno = %d\n", ftp->change_monsno );
  OBATA_Printf( "change_monssex = %d\n", ftp->change_monssex );
  OBATA_Printf( "-------------------------------------------------\n" );
}


//========================================================================================
// ���C�x���g
//========================================================================================
typedef struct 
{
  GAMESYS_WORK*           gameSystem;
  GAMEDATA*               gameData;
  POKEPARTY*              pokeParty;  // �莝���|�P�p�[�e�B
  u8                      tradeNo;    // �����f�[�^No.
  u8                      partyPos;   // �����ɏo���|�P�����̎莝���C���f�b�N�X
  FLD_TRADE_WORK*         tradeWork;  // �������[�N
  POKEMONTRADE_DEMO_PARAM tradeDemoParam;   // �����f���̃p�����[�^
  SHINKA_DEMO_PARAM*      shinkaDemoParam;  // �i���f���̃p�����[�^

} FLD_TRADE_EVWORK;

// �V�[�P���X�ԍ�
enum {
  SEQ_INIT,         // �C�x���g������
  SEQ_TRADE,        // �����f��
  SEQ_DATA_UPDATE,  // �f�[�^�X�V
  SEQ_EVOLUTION,    // �i���f��
  SEQ_EXIT,         // �C�x���g�I��
};

//----------------------------------------------------------------------------------------
/**
 * @brief �Q�[���������C�x���g�����֐�
 */
//----------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldPokeTradeEvent( GMEVENT* event, int* seq, void* wk )
{
  FLD_TRADE_EVWORK* work       = (FLD_TRADE_EVWORK*)wk;
  GAMESYS_WORK*     gameSystem = work->gameSystem;
  GAMEDATA*         gameData   = work->gameData;
  POKEPARTY*        pokeParty  = work->pokeParty;
  FIELDMAP_WORK*    fieldmap   = GAMESYSTEM_GetFieldMapWork( gameSystem );
  POKEMON_PARAM*    pokeParam  = PokeParty_GetMemberPointer( pokeParty, work->partyPos );

  switch( *seq ) {
  // �C�x���g������
  case SEQ_INIT:
    // �������[�N����
    {
      u16 zoneID = FIELDMAP_GetZoneID( fieldmap );
       
      work->tradeWork = FLD_TRADE_WORK_Create( HEAPID_PROC, work->tradeNo );
      SetPokemonParam( gameData, HEAPID_PROC, zoneID,
                       work->tradeWork->p_pp, 
                       work->tradeWork->p_pokedata, 
                       work->tradeNo );
    }
    // DEBUG:
    PP_Dump( work->tradeWork->p_pp );
    FTP_Dump( work->tradeWork->p_pokedata );
    *seq = SEQ_TRADE;
    break;

  // �����f���Ăяo��
  case SEQ_TRADE:
    {
      GMEVENT* demo;
      FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );
      work->tradeDemoParam.gamedata = gameData; 
      work->tradeDemoParam.pMy      = GAMEDATA_GetMyStatus( gameData );
      work->tradeDemoParam.pMyPoke  = pokeParam;
      work->tradeDemoParam.pNPC     = work->tradeWork->p_myste;
      work->tradeDemoParam.pNPCPoke = work->tradeWork->p_pp;
      demo = EVENT_FieldSubProc( gameSystem, fieldmap, 
          FS_OVERLAY_ID(pokemon_trade), &PokemonTradeDemoProcData, &work->tradeDemoParam );
      GMEVENT_CallEvent( event, demo );
    }
    *seq = SEQ_EVOLUTION;
    break;

  // �f�[�^�X�V
  case SEQ_DATA_UPDATE:
    // �莝���|�P�㏑��
    PokeParty_SetMemberData( pokeParty, work->partyPos, work->tradeWork->p_pp );
    // �}�ӓo�^
    {
      ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gameData );
      ZUKANSAVE_SetPokeSee( zukan, work->tradeWork->p_pp );  // ����
      ZUKANSAVE_SetPokeGet( zukan, work->tradeWork->p_pp );  // �߂܂���
    }
    *seq = SEQ_EXIT;
    break;

  // �i���f���Ăяo��
  case SEQ_EVOLUTION:
    {
      SHINKA_COND cond;
      u16 afterMonsNo = SHINKA_Check( pokeParty, pokeParam, SHINKA_TYPE_TUUSHIN, 0, &cond, HEAPID_PROC );

      if( afterMonsNo ) {
        GMEVENT* demo;

        // �p�����[�^�𐶐�
        GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
        work->shinkaDemoParam = SHINKADEMO_AllocParam( 
            HEAPID_PROC, gameData, pokeParty, afterMonsNo, work->partyPos, cond, TRUE );
        GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );

        // �f���Ăяo��
        demo = EVENT_FieldSubProc( gameSystem, fieldmap, 
            FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, work->shinkaDemoParam );
        GMEVENT_CallEvent( event, demo );
      }
    }
    *seq = SEQ_EXIT;
    break;
  
  // �C�x���g�I������
  case SEQ_EXIT:
    // �i���f���̌�n��
    if( work->shinkaDemoParam ) {
        GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
      SHINKADEMO_FreeParam( work->shinkaDemoParam );
      GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
    }
    // �������[�N�j��
    FLD_TRADE_WORK_Delete( work->tradeWork );
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------------------
/**
 * @brief �Q�[���������C�x���g�𐶐�����
 *
 * @param gsys �Q�[���V�X�e��
 * @param trade_no  �����f�[�^No.
 * @param party_pos �����ɏo���|�P�����̎莝���C���f�b�N�X
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldPokeTrade( GAMESYS_WORK* gsys, u8 trade_no, u8 party_pos )
{
  GMEVENT* event;
  FLD_TRADE_EVWORK* work;
  GAMEDATA* gameData;
  POKEPARTY* pokeParty;

  gameData = GAMESYSTEM_GetGameData( gsys );
  pokeParty = GAMEDATA_GetMyPokemon( gameData );

  // ����
  event = GMEVENT_Create( gsys, NULL, FieldPokeTradeEvent, sizeof(FLD_TRADE_EVWORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gameSystem = gsys;
  work->gameData = gameData;
  work->pokeParty = pokeParty;
  work->tradeNo = trade_no;
  work->partyPos = party_pos; 
  work->shinkaDemoParam = NULL;
  return event;
}
