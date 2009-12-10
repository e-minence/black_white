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
#if 0
#include "common.h"
#include "system/arc_util.h"
#include "include/msgdata/msg.naix"
#include "include/gflib/strbuf_family.h"
#include "fieldsys.h"
#include "field_event.h"
#include "ev_mapchange.h"
#include "ev_time.h"
#include "savedata/zukanwork.h"
#include "savedata/get_poke.h"
#include "savedata/mail.h"
#include "savedata/mail_util.h"
#include "poketool/poke_memo.h"
#include "itemtool/itemsym.h"
#include "itemtool/item.h"
#include "zonedata.h"
#include "fielddata/maptable/zone_id.h"
#include "fld_trade.h"
#include "fld_trade_local.h"
#include "onlyone_poke.h"
#endif

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

FS_EXTERN_OVERLAY(pokemon_trade);

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
*/
//-----------------------------------------------------------------------------

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

static void SetPokemonParam( POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 level, u32 trade_no, u32 zone_id, int heapID );

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

	GF_ASSERT( trade_no < FLD_TRADE_NUM );
	
	work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(FLD_TRADE_WORK) );
	work->heapID   = heap_id;
	work->trade_no = trade_no;

	// �����f�[�^�ǂݍ���
	//work->p_pokedata = ArcUtil_Load( ARC_FIELD_TRADE_POKE, trade_no, FALSE, heap_id, ALLOC_TOP );
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

#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@�|�P�����a����(���l���`�̃|�P������Ⴄ)
 *
 *	@param	heapID		�q�[�vID
 *	@param	trade_no	�����ԍ�
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
void FLD_KeepPokemonAdd( FIELDSYS_WORK* fsys, u8 trade_no ,u8 level,u16 zoneID)
{
	POKEPARTY* party;
	POKEMON_PARAM* pp;
	FLD_TRADE_POKEDATA* p_data;
	
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	
	// �����f�[�^�ǂݍ���
	p_data = ArcUtil_Load( ARC_FIELD_TRADE_POKE, trade_no, FALSE, HEAPID_WORLD, ALLOC_BOTTOM );

	//�p�����[�^����
	SetPokemonParam( pp, p_data, level , trade_no, zoneID ,TRMEMO_FLDKEEP_PLACESET,HEAPID_WORLD);

	// �}�ӂɓo�^
	SaveData_GetPokeRegister( fsys->savedata, pp );

	//�莝���̈�ԍŌ�ɒǉ�
	party = SaveData_GetTemotiPokemon( fsys->savedata );
	PokeParty_Add(party, pp);

	//���[���|�P�����������ꍇ�A���[���f�[�^���Z�b�g
	if(trade_no == FLD_TRADE_ONISUZUME){
		MAIL_DATA* dat;
		STRBUF* str;
		POKEMON_PARAM* tp = PokeParty_GetMemberPointer( party, PokeParty_GetPokeCount(party)-1);
		
		str = GetTradeMsgData( HEAPID_WORLD, FLD_TRADE_GET_OYA_GMM(trade_no) );

		dat = MailData_MailEventDataMake(pp,
				ItemMailDesignGet( p_data->item ),p_data->oya_sex,str,p_data->mons_id);
		PP_Put(tp,ID_PARA_mail_data,dat);

		GFL_STR_DeleteBuffer(str);
		sys_FreeMemoryEz(dat);
	}
	
	sys_FreeMemoryEz( p_data );
	sys_FreeMemoryEz( pp );
}
#endif


#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@���[���|�P�����C�x���g��p���[���f�[�^�𐶐�(�I�j�X�Y���̃��[����p)
 *
 *	@return	���[�N
 *
 *	@com	��������Alloc���ĕԂ��̂ŁA�Ăяo������������邱�ƁI
 */
//-----------------------------------------------------------------------------
MAIL_DATA* FLD_MakeKeepPokemonEventMail( void )
{
	POKEMON_PARAM* pp;
	FLD_TRADE_POKEDATA* p_data;
	STRBUF* str;
	MAIL_DATA* dat;
	
	pp = PokemonParam_AllocWork( HEAPID_WORLD );
	
	// �����f�[�^�ǂݍ���
	p_data = ArcUtil_Load( ARC_FIELD_TRADE_POKE, FLD_TRADE_ONISUZUME, FALSE, HEAPID_WORLD, ALLOC_BOTTOM );

	//�p�����[�^����
	SetPokemonParam( pp, p_data, 20 ,FLD_TRADE_ONISUZUME ,
		ZONE_ID_R35R0101 ,TRMEMO_FLDKEEP_PLACESET,HEAPID_WORLD);

	str = GetTradeMsgData( HEAPID_WORLD, FLD_TRADE_GET_OYA_GMM(FLD_TRADE_ONISUZUME) );

	dat = MailData_MailEventDataMake(pp,
			ItemMailDesignGet( p_data->item ),p_data->oya_sex,str,p_data->mons_id);
	
	GFL_STR_DeleteBuffer(str);
	
	sys_FreeMemoryEz( p_data );
	sys_FreeMemoryEz( pp );

	return dat;
}
#endif


#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	�t�B�[���h�@�a����|�P�����ԋp�s�`�F�b�N
 *
 *	�莝���̃|�P�������a����|�P�����ƈ�v���邩��A�A�C�e���̗L�����`�F�b�N
 *
 *	@param	trade_no	�����ԍ�
 *	@param	pos			�`�F�b�N����莝���̃|�P�����ʒu
 *
 *	@retval	KEEP_POKE_RERR_NONE		�Ԃ���
 *	@retval	KEEP_POKE_RERR_NGPOKE	�Ⴄ�|�P����������Ԃ��Ȃ�
 *	@retval	KEEP_POKE_RERR_ITEM		�A�C�e���������Ă邩��Ԃ��Ȃ�
 *	@retval	KEEP_POKE_RERR_CBALL	�J�X�^���{�[�����Z�b�g����Ă邩��Ԃ��Ȃ�
 */
//-----------------------------------------------------------------------------
u16 FLD_KeepPokemonReturnCheck( FIELDSYS_WORK* fsys, u8 trade_no ,u8 pos)
{
	POKEPARTY* party;
	POKEMON_PARAM* pp;
	
	party = SaveData_GetTemotiPokemon( fsys->savedata );
	pp = PokeParty_GetMemberPointer(party,pos);

	//�a�������|�P�������ǂ����`�F�b�N
	if(OnlyonePoke_KeepPokeCheck(pp,trade_no) == FALSE){
		return KEEP_POKE_RERR_NGPOKE;
	}

	//�z���g�ɕԂ��Ă������ǂ����`�F�b�N
	
	//�{�[���J�v�Z�����Z�b�g����Ă��邩�ǂ���
	{
		u8 cb_id;

		cb_id = PokeParaGet( pp, ID_PARA_cb_id, NULL);
		if(cb_id != 0){
			return KEEP_POKE_RERR_CBALL;
		}
	}
	//������莝�����Q�C�ȏア�邩�ǂ����H
	{
		int i,max,num = 0;
		POKEMON_PARAM* pp;
		max = PokeParty_GetPokeCount(party);
		for(i = 0;i < max;i++){
			pp = PokeParty_GetMemberPointer(party,i);
			if(	PokeParaGet(pp,ID_PARA_fusei_tamago_flag,NULL) == TRUE || 
				PokeParaGet(pp,ID_PARA_hp,NULL) == 0 ||
				PokeParaGet(pp,ID_PARA_tamago_flag,NULL)){
				continue;
			}
			num++;
		}
		if(num < 2){
			return KEEP_POKE_RERR_ONLYONE;
		}
	}
	
	//�A�C�e���������Ă��邩�ǂ����H
	{
		u16 itemno;
		
		itemno = PokeParaGet( pp, ID_PARA_item, NULL );
		if(itemno != ITEM_DUMMY_DATA){
			return KEEP_POKE_RERR_ITEM;
		}
	}
	return 0;
}
#endif



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

#if 0
//----------------------------------------------------------------------------
/**
 *	@brief	����
 *
 *	@param	p_fsys		�t�B�[���h�V�X�e��
 *	@param	work		�������[�N
 *	@param	party_pos	�p�[�e�B�[�|�W�V����
 */
//-----------------------------------------------------------------------------
void FLD_Trade( FIELDSYS_WORK * p_fsys, FLD_TRADE_WORK* work, int party_pos )
{
	POKEPARTY * party = SaveData_GetTemotiPokemon( p_fsys->savedata );

	// �������s
	PokeParty_SetMemberData( party, party_pos, work->p_pp );

	// �}�ӂɓo�^
	SaveData_GetPokeRegister( p_fsys->savedata, work->p_pp );
}
#endif


#if 0
void FLD_TradeDemoDataMake( FIELDSYS_WORK * p_fsys, FLD_TRADE_WORK* work, int party_pos, DEMO_TRADE_PARAM* p_demo, POKEMON_PARAM* sendPoke, POKEMON_PARAM* recvPoke )
{
	POKEPARTY * party = SaveData_GetTemotiPokemon( p_fsys->savedata );
	POKEMON_PARAM* p_chg_poke;
	STRBUF* str;
	u32 lev;		// ���x��
	int time_zone;

	// ��������|�P����
	p_chg_poke = PokeParty_GetMemberPointer( party, party_pos );
	lev = PokeParaGet( p_chg_poke, ID_PARA_level, NULL );
	
	// �����|�P�����̍쐬
	SetPokemonParam( work->p_pp, work->p_pokedata, lev, work->trade_no,
		p_fsys->location->zone_id ,TRMEMO_OFFTRADE_PLACESET,work->heapID);

	// �f���p�̃|�P�����p�����[�^�Ɋi�[
	PokeCopyPPtoPP( p_chg_poke, sendPoke );
	PokeCopyPPtoPP( work->p_pp, recvPoke );

	// �f���f�[�^�쐬
	p_demo->sendPoke	= PPPPointerGet( sendPoke );
	p_demo->recvPoke	= PPPPointerGet( recvPoke );
	p_demo->partner	= work->p_myste;
	p_demo->seqFlag	= DEMO_TRADE_SEQ_FULL;
	p_demo->config		= SaveData_GetConfig( p_fsys->savedata );
	time_zone = EVTIME_GetTimeZone( p_fsys );
	if( (time_zone == TIMEZONE_MORNING) || 
		(time_zone == TIMEZONE_NOON) ){
		p_demo->bgType		= DEMO_TRADE_BGTYPE_MORNING;
	}else if( time_zone == TIMEZONE_EVENING ){
		p_demo->bgType		= DEMO_TRADE_BGTYPE_EVENING;
	}else{
		p_demo->bgType		= DEMO_TRADE_BGTYPE_NIGHT;
	}
}
#endif




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
 *	@param	pp		    �|�P�����p����
 *	@param	data		  �f�[�^
 *	@param	level			���x��
 *	@param	trade_no	trade�i���o�[
 *	@param	zoneID		�g���[�i�[�����ɋL�ڂ���]�[��ID
 *	@param	heapID		heapID
 */
//-----------------------------------------------------------------------------
static void SetPokemonParam( POKEMON_PARAM* pp, FLD_TRADE_POKEDATA* data, 
	                           u32 level, u32 trade_no, u32 zone_id, int heapID )
{
	STRBUF* strbuf;
	u32 placeid; 
  u32 personal_rnd;

  // �w��̐��ʂɂȂ�悤�Ȍ������𐶐�
  personal_rnd = POKETOOL_CalcPersonalRand( data->monsno, data->formno, data->sex );
	
	// �����X�^�[�i���o�[�@���x���@�ő̗����@ID�ݒ�
	PP_SetupEx( pp, 
              data->monsno, 
              level, 
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

#if 0
	// �g���[�i�[�����ݒ�
	placeid = ZoneData_GetPlaceNameID( zone_id );
	TrainerMemoSetPP( pp, NULL, memo, placeid, heapID );
#endif
  // �߂܂����ꏊ
	PP_Put( pp, ID_PARA_get_place, zone_id );
	PP_Put( pp, ID_PARA_new_get_place, zone_id );

  PP_Dump( pp );

	// �v�Z���Ȃ���
	PP_Renew( pp );

	// ���A�ɂȂ�Ȃ��悤�Ƀf�[�^�ݒ肳��Ă��Ȃ������ꍇNG
  GF_ASSERT( PP_CheckRare( pp ) == FALSE );

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
    GF_ASSERT( (speabi==sp1)||(speabi==sp2)||(speabi==sp3) );
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
  val = PP_Get( pp, ID_PARA_new_get_place, NULL ); OBATA_Printf( "new_get_place = %d\n", val );
  OBATA_Printf( "-------------------------------------------------\n" );
}

static void FTP_Dump( const FLD_TRADE_POKEDATA* ftp )
{
  OBATA_Printf( "--------------------------------------------- FTP\n" );
  OBATA_Printf( "monsno = %d\n", ftp->monsno );
  OBATA_Printf( "formno = %d\n", ftp->formno );
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
  GAMESYS_WORK*                gsys;
  u8                        tradeNo;  // �����f�[�^No.
  u8                       partyPos;  // �����ɏo���|�P�����̎莝���C���f�b�N�X
  FLD_TRADE_WORK*         tradeWork;  // �������[�N
  POKEMONTRADE_DEMO_PARAM demoParam;  // �f�� �p�����[�^

} FLD_TRADE_EVWORK;

//----------------------------------------------------------------------------------------
/**
 * @brief �Q�[���������C�x���g�����֐�
 */
//----------------------------------------------------------------------------------------
static GMEVENT_RESULT FieldPokeTradeEvent( GMEVENT* event, int* seq, void* wk )
{
  FLD_TRADE_EVWORK* work = (FLD_TRADE_EVWORK*)wk;

  switch( *seq )
  {
  // �������[�N����
  case 0:
    {
      GAMEDATA*         gdata = GAMESYSTEM_GetGameData( work->gsys );
      FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( work->gsys );
      u16             zone_id = FIELDMAP_GetZoneID( fieldmap );
      POKEPARTY*        party = GAMEDATA_GetMyPokemon( gdata );
      POKEMON_PARAM*       pp = PokeParty_GetMemberPointer( party, work->partyPos );
      u32               level = PP_Get( pp, ID_PARA_level, NULL );
       
      work->tradeWork = FLD_TRADE_WORK_Create( HEAPID_WORLD, work->tradeNo );
      SetPokemonParam( work->tradeWork->p_pp, work->tradeWork->p_pokedata, 
                      level, work->tradeNo, zone_id, HEAPID_WORLD );
    }
    // DEBUG:
    PP_Dump( work->tradeWork->p_pp );
    FTP_Dump( work->tradeWork->p_pokedata );
    ++(*seq);
    break;
  // �����f���Ăяo��
  case 1:
    {
      GMEVENT* demo;
      GAMEDATA*         gdata = GAMESYSTEM_GetGameData( work->gsys );
      POKEPARTY*     my_party = GAMEDATA_GetMyPokemon( gdata );
      FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( work->gsys );
      work->demoParam.gsys     = work->gsys; 
      work->demoParam.pMy      = GAMEDATA_GetMyStatus( gdata );
      work->demoParam.pMyPoke  = PokeParty_GetMemberPointer( my_party, work->partyPos );
      work->demoParam.pNPC     = work->tradeWork->p_myste;
      work->demoParam.pNPCPoke = work->tradeWork->p_pp;
      demo = EVENT_FieldSubProc( work->gsys, fieldmap, 
                                  FS_OVERLAY_ID(pokemon_trade), 
                                  &PokemonTradeDemoProcData, &work->demoParam );
      GMEVENT_CallEvent( event, demo );
    }
    ++(*seq);
    break;
  // �f�[�^�X�V
  case 2:
    // �莝���|�P�㏑��
    {
      GAMEDATA*  gdata = GAMESYSTEM_GetGameData( work->gsys );
      POKEPARTY* party = GAMEDATA_GetMyPokemon( gdata );
      PokeParty_SetMemberData( party, work->partyPos, work->tradeWork->p_pp );
    }
    // �}�ӓo�^
    {
      GAMEDATA*       gdata = GAMESYSTEM_GetGameData( work->gsys );
      ZUKAN_SAVEDATA* zukan = GAMEDATA_GetZukanSave( gdata );
      ZUKANSAVE_SetPokeSee( zukan, work->tradeWork->p_pp );  // ����
      ZUKANSAVE_SetPokeGet( zukan, work->tradeWork->p_pp );  // �߂܂���
    }
    ++(*seq);
    break;
  // �������[�N�j��
  case 3:
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

  // ����
  event = GMEVENT_Create( gsys, NULL, FieldPokeTradeEvent, sizeof(FLD_TRADE_EVWORK) );

  // ������
  work = GMEVENT_GetEventWork( event );
  work->gsys     = gsys;
  work->tradeNo  = trade_no;
  work->partyPos = party_pos; 
  return event;
}
