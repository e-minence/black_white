//============================================================================================
/**
 * @file	debug_data.c
 * @brief	�Q�[���J�n���̃f�o�b�O�f�[�^�ǉ�����
 * @author	tamada GAME FREAK Inc.
 * @date	09.10.24
 *
 */
//============================================================================================

#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_data.h"

#include "debug_data.h"


#include "poke_tool/monsno_def.h"
#include "item/item.h"  //�f�o�b�O�A�C�e�������p
#include "waza_tool/wazano_def.h"  //�f�o�b�O�|�P�����p
#include "savedata/zukan_savedata.h"		// �}�ӕߊl�t���O�ݒ�

#include "savedata/intrude_save_field.h"
#include "savedata/intrude_save.h"

#include "arc_def.h"  //ARCID_MESSAGE
#include "message.naix" //NARC_message_debugname_dat
#include "msg/msg_debugname.h"  //DEBUG_NAME_RAND_M_000
#include "system/gfl_use.h"   //GFUser_GetPublicRand

#ifdef PM_DEBUG
//============================================================================================
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p�ɓK���Ɏ莝���|�P������Add
 * @param   gamedata  GAMEDATA�ւ̃|�C���^		
 */
//--------------------------------------------------------------
static void DEBUG_MyPokeAdd(GAMEDATA * gamedata, HEAPID heapID)
{
  MYSTATUS *myStatus;
	POKEPARTY *party;
	POKEMON_PARAM *pp;
	const STRCODE *name;
	
	party = GAMEDATA_GetMyPokemon(gamedata);
  myStatus = GAMEDATA_GetMyStatus(gamedata);
  name = MyStatus_GetMyName( myStatus );
  
	pp = PP_Create(MONSNO_MOGURYUU, 100, 123456, heapID);

#ifdef DEBUG_ONLY_for_tamada
	PP_Setup(pp, MONSNO_MUSYAANA, 2, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);
	ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave(gamedata), pp );
#else
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);
	ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave(gamedata), pp );

	PP_Setup(pp, MONSNO_MUSYAANA, 100, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
	PokeParty_Add(party, pp);
	ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave(gamedata), pp );

	PP_Setup(pp, MONSNO_TIRAAMHI, 100, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
  PP_SetWazaPos( pp , WAZANO_TEREPOOTO , 0 );
  PP_SetWazaPos( pp , WAZANO_HURASSYU, 1 );
	PokeParty_Add(party, pp);
	ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave(gamedata), pp );

  PP_Setup(pp, MONSNO_TIRAAMHI, 100, 123456);
  PP_Put( pp , ID_PARA_oyaname_raw , (u32)name );
  PP_Put( pp , ID_PARA_oyasex , MyStatus_GetMySex( myStatus ) );
  PP_SetWazaPos( pp , WAZANO_NAMINORI , 0 );
  PP_SetWazaPos( pp , WAZANO_TAKINOBORI , 1 );
  PP_SetWazaPos( pp , WAZANO_KAIRIKI , 2 );
  PP_SetWazaPos( pp , WAZANO_IAIGIRI , 3 );
	PokeParty_Add(party, pp);
	ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave(gamedata), pp );
#endif

	GFL_HEAP_FreeMemory(pp);
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p�F�K���Ɏ莝���𐶐�����
 * @param	myitem	�莝���A�C�e���\���̂ւ̃|�C���^
 */
//------------------------------------------------------------------
static const ITEM_ST DebugItem[] = {
	{ ITEM_MASUTAABOORU,	111 },
	{ ITEM_MONSUTAABOORU,	222 },
	{ ITEM_SUUPAABOORU,		333 },
	{ ITEM_HAIPAABOORU,		444 },
	{ ITEM_PUREMIABOORU,	555 },
	{ ITEM_DAIBUBOORU,		666 },
	{ ITEM_TAIMAABOORU,		777 },
	{ ITEM_RIPIITOBOORU,	888 },
	{ ITEM_NESUTOBOORU,		999 },
	{ ITEM_GOOZYASUBOORU,	100 },
	{ ITEM_KIZUGUSURI,		123 },
	{ ITEM_NEMUKEZAMASI,	456 },
	{ ITEM_BATORUREKOODAA,  1},  // �o�g�����R�[�_�[
	{ ITEM_TAUNMAPPU,		1 },
	{ ITEM_ZITENSYA,		1 },
	{ ITEM_NANDEMONAOSI,	18 },
	{ ITEM_PIIPIIRIKABAA,	18 },
	{ ITEM_PIIPIIMAKKUSU,	18 },
	{ ITEM_ANANUKENOHIMO, 50 },
	{ ITEM_GOORUDOSUPUREE, 50 },
	{ ITEM_DOKUKESI,		18 },		// �ǂ�����
	{ ITEM_YAKEDONAOSI,		19 },		// �₯�ǂȂ���
	{ ITEM_KOORINAOSI,		20 },		// ������Ȃ���
	{ ITEM_MAHINAOSI,		22 },		// �܂ЂȂ���
	{ ITEM_EFEKUTOGAADO,	54 },		// �G�t�F�N�g�K�[�h
	{ ITEM_KURITHIKATTAA,	55 },		// �N���e�B�J�b�^�[
	{ ITEM_PURASUPAWAA,		56 },		// �v���X�p���[
	{ ITEM_DHIFENDAA,		57 },		// �f�B�t�F���_�[
	{ ITEM_SUPIIDAA,		58 },		// �X�s�[�_�[
	{ ITEM_YOKUATAARU,		59 },		// ���N�A�^�[��
	{ ITEM_SUPESYARUAPPU,	60 },		// �X�y�V�����A�b�v
	{ ITEM_SUPESYARUGAADO,	61 },		// �X�y�V�����K�[�h
	{ ITEM_PIPPININGYOU,	62 },		// �s�b�s�ɂ񂬂傤
	{ ITEM_ENEKONOSIPPO,	63 },		// �G�l�R�̃V�b�|
	{ ITEM_GENKINOKAKERA,	28 },		// ���񂫂̂�����
	{ ITEM_KAIHUKUNOKUSURI,	28 },		// ���񂫂̂�����
	{ ITEM_PIIPIIEIDO,	28 },
	{ ITEM_PIIPIIEIDAA,	28 },
	{ ITEM_DAAKUBOORU,	13 },		// �_�[�N�{�[��
	{ ITEM_HIIRUBOORU,  14 },		// �q�[���{�[��
	{ ITEM_KUIKKUBOORU,	15 },		// �N�C�b�N�{�[��
	{ ITEM_PURESYASUBOORU,	16 },	// �v���V�A�{�[��
	{ ITEM_TOMODATITETYOU,  1},  // �Ƃ������蒠
  {ITEM_PARESUHEGOO, 1}, // �p���X�փS�[
	{ ITEM_MOKOSINOMI,	50},	//���R�V�̂�
	{ ITEM_GOSUNOMI,	50},	//�S�X�̂�
	{ ITEM_RABUTANOMI,	50},	//���u�^�̂�
	
  { ITEM_GAKUSYUUSOUTI,  5},  // �������イ������
  {ITEM_SENSEINOTUME, 5}, // ���񂹂��̃c��
  { ITEM_KODAWARIHATIMAKI,  5},  // �������n�`�}�L
  {ITEM_OMAMORIKOBAN, 5}, // ���܂��肱�΂�

  { ITEM_WAZAMASIN01,  328},		// �킴�}�V���O�P
  {  ITEM_WAZAMASIN02	, 329 },		// �킴�}�V���O�Q
  {  ITEM_WAZAMASIN03	, 330 },		// �킴�}�V���O�R
  {  ITEM_WAZAMASIN04	, 331 },		// �킴�}�V���O�S
  {  ITEM_WAZAMASIN05	, 332 },		// �킴�}�V���O�T
  
};

//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p�Ɏ莝���ǂ�����������
 * @param   gamedata  GAMEDATA�ւ̃|�C���^		
 * @param   heapID    ���p����q�[�vID�̎w��
 */
//--------------------------------------------------------------
static void DEBUG_MYITEM_MakeBag(GAMEDATA * gamedata, HEAPID heapID)
{
	u32	i;
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

	MYITEM_Init( myitem );
	for( i=0; i<NELEMS(DebugItem); i++ ){
		MYITEM_AddItem( myitem, DebugItem[i].id, DebugItem[i].no, heapID );
	}
}

//============================================================================================
//============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static void addIntrudeSecretItem(
    GFL_MSGDATA * msgman, INTRUDE_SAVE_WORK * intsave, u16 item_no, u16 tbl_no)
{
  INTRUDE_SECRET_ITEM_SAVE intrude_item = {
    { 0 },
    ITEM_OMAMORIKOBAN,
    0,
    0
  };
  STRBUF * namebuf;

  namebuf = GFL_MSG_CreateString( msgman, DEBUG_NAME_RAND_M_000 + GFUser_GetPublicRand(8) );
  GFL_STR_GetStringCode( namebuf, intrude_item.name, PERSON_NAME_SIZE + EOM_SIZE );
  intrude_item.item = item_no;
  intrude_item.tbl_no = tbl_no;
  ISC_SAVE_SetItem( intsave, &intrude_item );

  GFL_STR_DeleteBuffer( namebuf );
}
//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p�ɉB����A�C�e����������
 * @param   gamedata  GAMEDATA�ւ̃|�C���^		
 * @param   heapID    ���p����q�[�vID�̎w��
 */
//--------------------------------------------------------------
static void DEBUG_INTRUDE_MakeSecretItem( GAMEDATA * gamedata, HEAPID heapID )
{
  GFL_MSGDATA * msgman;
  INTRUDE_SAVE_WORK * intsave;
    
  msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_debugname_dat, heapID );

  intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( gamedata ) );
  addIntrudeSecretItem( msgman, intsave, ITEM_OMAMORIKOBAN, 0 );
  addIntrudeSecretItem( msgman, intsave, ITEM_WAZAMASIN05 ,1 );
  addIntrudeSecretItem( msgman, intsave, ITEM_KODAWARIHATIMAKI ,0 );

  GFL_MSG_Delete( msgman );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void DEBUG_SetStartData( GAMEDATA * gamedata, HEAPID heapID )
{
  DEBUG_INTRUDE_MakeSecretItem( gamedata, heapID );
  DEBUG_MYITEM_MakeBag( gamedata, heapID );
  DEBUG_MyPokeAdd( gamedata, heapID );
}



#endif  // PM_DEBUG
