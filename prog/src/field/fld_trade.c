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
#include "include/msg/msg_fld_trade.h"

#define DEBUG_MODE

FS_EXTERN_OVERLAY(pokemon_trade);


//=============================================================================
// ���Q�[�����������[�N
//=============================================================================
struct _FLD_TRADE_WORK {

	HEAPID              heapID;
	u32                 tradeNo;	  // �����i���o�[
	FLD_TRADE_POKEDATA* pokeData;		// �����|�P�����f�[�^
	POKEMON_PARAM*		  pokeParam;	// �����|�P����
	MYSTATUS*			      pokeOya;		// ��������p�����[�^

};


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
static STRBUF* GetTradeMsgData( u32 heapID, u32 idx ); 
static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, 
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 tradeNo ); 
#ifdef DEBUG_MODE
static void PP_Dump( const POKEMON_PARAM* pp );
static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp );
#endif


//----------------------------------------------------------------------------
/**
 *	@brief	�Q�[�����������[�N�𐶐�����
 *
 *	@param	heap_id	�q�[�vID
 *	@param	tradeNo	�����ԍ�
 */
//-----------------------------------------------------------------------------
FLD_TRADE_WORK* FLD_TRADE_WORK_Create( u32 heap_id, u32 tradeNo )
{
	FLD_TRADE_WORK* work;
	STRBUF* strbuf;
	STRCODE str_arry[ 128 ];

	GF_ASSERT( tradeNo < FLD_TRADE_POKE_NUM );
	
	work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FLD_TRADE_WORK) );
	work->heapID   = heap_id;
	work->tradeNo = tradeNo;

	// �����f�[�^�ǂݍ���
  work->pokeData = GFL_ARC_LoadDataAllocOfs( ARCID_FLD_TRADE_POKE, tradeNo, 
                                               heap_id, 0, sizeof(FLD_TRADE_POKEDATA) );
  // �����|�P�����f�[�^�쐬
	work->pokeParam = GFL_HEAP_AllocMemory( heap_id, sizeof(POKEMON_PARAM) );

	// �e�f�[�^�쐬
	work->pokeOya = MyStatus_AllocWork( heap_id );
	MyStatus_Init( work->pokeOya );
	strbuf = GetTradeMsgData( heap_id, work->pokeData->str_id_oya_name );
	GFL_STR_GetStringCode( strbuf, str_arry, 128 );
	GFL_STR_DeleteBuffer( strbuf );
	MyStatus_SetMyName( work->pokeOya, str_arry );
	MyStatus_SetMySex( work->pokeOya, work->pokeData->oya_sex );
	
	return work;
}

//----------------------------------------------------------------------------
/**
 *	@brief �Q�[�����������[�N��j������
 *
 *	@param	work		
 */
//-----------------------------------------------------------------------------
void FLD_TRADE_WORK_Delete( FLD_TRADE_WORK* work )
{
	GFL_HEAP_FreeMemory( work->pokeData );
	GFL_HEAP_FreeMemory( work->pokeParam );
	GFL_HEAP_FreeMemory( work->pokeOya );
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
	return cwork->pokeData->monsno;
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
	return cwork->pokeData->change_monsno;
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
	return cwork->pokeData->change_monssex;
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
	return cwork->pokeData->oya_sex;
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
 *	@param	pp		    �|�P�����p����
 *	@param	data		  �f�[�^
 *	@param	tradeNo	trade�i���o�[
 */
//-----------------------------------------------------------------------------
static void SetPokemonParam( GAMEDATA* gameData, HEAPID heapID, 
                             POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 tradeNo )
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

#ifdef DEBUG_MODE
  PP_Dump( pp );
#endif

	// �j�b�N�l�[��
	strbuf = GetTradeMsgData( heapID, data->str_id_nickname );
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
  {
    POKEMON_PERSONAL_DATA* ppd;
    u32 sp1, sp2, sp3;
    u32 tokusei;

    ppd = POKE_PERSONAL_OpenHandle( data->monsno, data->formno, heapID );
    sp1 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi1 );
    sp2 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi2 );
    sp3 = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_speabi3 );
    POKE_PERSONAL_CloseHandle( ppd );

    switch( data->tokusei_idx ) {
    case 0: tokusei = sp1; break;
    case 1: tokusei = sp2; break;
    case 2: tokusei = sp3; break;
    default: 
      GF_ASSERT(0);
      tokusei = sp1;
      break;
    } 
    PP_Put( pp, ID_PARA_speabino, tokusei );
  }

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
	strbuf = GetTradeMsgData( heapID, data->str_id_oya_name );
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

#ifdef DEBUG_MODE
  PP_Dump( pp );
#endif

	// �v�Z���Ȃ���
	PP_Renew( pp );

	// ���A�ɂȂ�Ȃ��悤�Ƀf�[�^�ݒ肳��Ă��Ȃ������ꍇNG
  GF_ASSERT( PP_CheckRare( pp ) == FALSE && "���A�|�P�ł��B");
} 


#ifdef DEBUG_MODE
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
  val = PP_Get( pp, ID_PARA_speabino, NULL ); OBATA_Printf( "tokusei_idx = %d\n", val );
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
#endif

#ifdef DEBUG_MODE
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
  OBATA_Printf( "tokusei_idx = %d\n", ftp->tokusei_idx );
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
#endif


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
    work->tradeWork = FLD_TRADE_WORK_Create( HEAPID_PROC, work->tradeNo );
    SetPokemonParam( gameData, HEAPID_PROC, 
        work->tradeWork->pokeParam, work->tradeWork->pokeData, work->tradeNo );
#ifdef DEBUG_MODE
    PP_Dump( work->tradeWork->pokeParam );
    FTP_Dump( work->tradeWork->pokeData );
#endif
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
      work->tradeDemoParam.pNPC     = work->tradeWork->pokeOya;
      work->tradeDemoParam.pNPCPoke = work->tradeWork->pokeParam;
      demo = EVENT_FieldSubProc( gameSystem, fieldmap, 
          FS_OVERLAY_ID(pokemon_trade), &PokemonTradeDemoProcData, &work->tradeDemoParam );
      GMEVENT_CallEvent( event, demo );
    }
    *seq = SEQ_DATA_UPDATE;
    break;

  // �f�[�^�X�V
  case SEQ_DATA_UPDATE:
    // �莝���|�P�㏑��
    PokeParty_SetMemberData( pokeParty, work->partyPos, work->tradeWork->pokeParam );
    // �}�ӓo�^
    {
      ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gameData );
      ZUKANSAVE_SetPokeSee( zukan, work->tradeWork->pokeParam );  // ����
      ZUKANSAVE_SetPokeGet( zukan, work->tradeWork->pokeParam );  // �߂܂���
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
        //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
        //work->shinkaDemoParam = SHINKADEMO_AllocParam( 
        //    HEAPID_PROC, gameData, pokeParty, afterMonsNo, work->partyPos, cond, TRUE );
        //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
        {
          SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof( SHINKA_DEMO_PARAM ) );
          sdp->gamedata          = gameData;
          sdp->ppt               = pokeParty;
          sdp->after_mons_no     = afterMonsNo;
          sdp->shinka_pos        = work->partyPos;
          sdp->shinka_cond       = cond;
          sdp->b_field           = TRUE;
          sdp->b_enable_cancel   = FALSE;
          work->shinkaDemoParam  = sdp;
        }
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
        //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
      //SHINKADEMO_FreeParam( work->shinkaDemoParam );
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      {
        SHINKA_DEMO_PARAM* sdp = work->shinkaDemoParam;
        GFL_HEAP_FreeMemory( sdp );
      }
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
 * @param tradeNo  �����f�[�^No.
 * @param party_pos �����ɏo���|�P�����̎莝���C���f�b�N�X
 *
 * @return ���������C�x���g
 */
//----------------------------------------------------------------------------------------
GMEVENT* EVENT_FieldPokeTrade( GAMESYS_WORK* gsys, u8 tradeNo, u8 party_pos )
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
  work->tradeNo = tradeNo;
  work->partyPos = party_pos; 
  work->shinkaDemoParam = NULL;
  return event;
}
