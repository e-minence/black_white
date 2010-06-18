//======================================================================
/**
 * @file	scrcmd_postman.c
 * @brief	�X�N���v�g�R�}���h�F�ӂ����Ȃ�������̔z�B��
 * @date	2010.02.12
 * @author  tamada GAMEFREAK inc.
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

//#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_postman.h"
#include "field_mystery_gift.h"

#include "msg/script/msg_postman.h"
#include "savedata/mystery_data.h"
#include "pm_define.h"  //TEMOTI_POKEMAX
#include "poke_tool/poke_tool.h"  //POKEMON_PARAM
#include "savedata/myitem_savedata.h"  //MYITEM_
#include "net_app/mystery.h"  //MYSTERY_PokemonCreate
#include "print/wordset.h"   //WORDSET_

#include "fldmmdl.h"
#include "fieldmap.h" //FIELDMAP_GetMMdlSys
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"

#include "savedata/intrude_save.h"  //GPower

#include "evt_lock.h" //EVTLOCK_SetEvtLock
#include "item/itemsym.h" //ITEM_RIBATHITIKETTO

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef BOOL (* MP_FUNC_ADD_CHECK)( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );
typedef void (* MP_FUNC_ADD)( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );
typedef u16 (* MP_FUNC_SUCCESS_MSG)( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work );
typedef u16 (* MP_FUNC_FAILURE_MSG)( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work );
typedef u16 (* MP_FUNC_GET_TYPE)( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
typedef struct {
  int gift_type;
  MP_FUNC_ADD_CHECK add_check_func;
  MP_FUNC_ADD       add_func;
  MP_FUNC_GET_TYPE    get_type;
  MP_FUNC_SUCCESS_MSG set_success_words;
  MP_FUNC_FAILURE_MSG set_failure_words;
}POSTMAN_FUNC_TABLE;


//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static MMDL * searchPOSTMANOBJ( FIELDMAP_WORK * fieldWork );
static int searchEventDataPostmanObj( GAMEDATA * gamedata );
static BOOL isLibertyTicketCard( const GIFT_PACK_DATA * gpd, u8 gift_type );

//--------------------------------------------------------------
//--------------------------------------------------------------
static const POSTMAN_FUNC_TABLE PostmanFuncTable[ MYSTERYGIFT_TYPE_MAX ];

static BOOL MP_Command_AddCheck(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );
static u16 MP_Command_SuccessMsg( u8 gift_type, GIFT_PACK_DATA *gpd, SCRCMD_WORK * work );
static u16 MP_Command_FailureMsg( u8 gift_type, GIFT_PACK_DATA *gpd, SCRCMD_WORK * work );
static BOOL MP_Command_Receive(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );
static u16 MP_Command_GetType(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd );
static u16 MP_Command_ObjStatus( FIELDMAP_WORK * fieldmap );
static u16 MP_Command_ObjID( FIELDMAP_WORK * fieldmap, GAMEDATA * gamedata );

//======================================================================
//
//    �X�N���v�g�R�}���h����
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief 
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @return  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPostmanCommand( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK * work = wk;
  u16 req = SCRCMD_GetVMWorkValue( core, wk );
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( wk );
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  GIFT_PACK_DATA aGpd;

  //2010412�s�v�c�ȑ��蕨�Z�[�u�f�[�^�擾�֐���alloc,free���ɕς��܂����̂Œu�������܂���nagihashi
  MYSTERY_DATA * fd = MYSTERY_DATA_Load( GAMEDATA_GetSaveControlWork( gamedata ), MYSTERYDATA_LOADTYPE_NORMAL, heapID );
  int index;
  GIFT_PACK_DATA * gpd = FIELD_MYSTERYDATA_GetGiftData( fd, &index, &aGpd );
  u8 gift_type = FIELD_MYSTERYDATA_GetGiftType( fd );

  if ( PostmanFuncTable[gift_type].gift_type != gift_type )
  {
    GF_ASSERT( 0 ); //���蕨�f�[�^�̎�ނ����ꂽ�H
    MYSTERY_DATA_UnLoad( fd );
    return VMCMD_RESULT_CONTINUE;
  }
  switch ( req )
  {
  case SCR_POSTMAN_REQ_EXISTS:
    if ( gift_type != MYSTERYGIFT_TYPE_NONE )
    { 
      *ret_wk = TRUE;
    }
    else
    {
      *ret_wk = FALSE;
    }
    break;
  case SCR_POSTMAN_REQ_ENABLE:
    *ret_wk = MP_Command_AddCheck( gift_type, work, gamedata, gpd );
    break;

  case SCR_POSTMAN_REQ_OK_MSG:
    *ret_wk = MP_Command_SuccessMsg( gift_type, gpd, work );
    break;

  case SCR_POSTMAN_REQ_NG_MSG:
    *ret_wk = MP_Command_FailureMsg( gift_type, gpd, work );
    break;

  case SCR_POSTMAN_REQ_RECEIVE:
    { //��ɓ���鏈��
      if ( MP_Command_Receive( gift_type, work, gamedata, gpd ) == TRUE )
      {
        FIELD_MYSTERYDATA_SetReceived( fd, index );
        if (isLibertyTicketCard( gpd, gift_type ) == TRUE )
        {
          //�r�N�e�B�z�z�`�P�b�g�������ꍇ�A�C�x���g���b�N����������
          MISC * misc = GAMEDATA_GetMiscWork( gamedata );
          MYSTATUS * mystatus = GAMEDATA_GetMyStatus( gamedata );
          EVTLOCK_SetEvtLock( misc, EVT_LOCK_NO_VICTYTICKET, mystatus );
        }
      }
    }
    *ret_wk = 0;
    break;
  case SCR_POSTMAN_REQ_OBJID:
    {
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( SCRCMD_WORK_GetGameSysWork( work ) );
      *ret_wk = MP_Command_ObjID( fieldmap, gamedata );
    }
    break;
  case SCR_POSTMAN_REQ_OBJSTAT:
    {
      FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( SCRCMD_WORK_GetGameSysWork( work ) );
      *ret_wk = MP_Command_ObjStatus( fieldmap );
    }
    break;
  case SCR_POSTMAN_REQ_TYPE:
    {
      *ret_wk = MP_Command_GetType( gift_type, work, gamedata, gpd );
    }
    break;

  default:
    GF_ASSERT( 0 );
  }
  MYSTERY_DATA_UnLoad( fd );
  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��邩�H�`�F�b�N
//--------------------------------------------------------------
static BOOL MP_Command_AddCheck(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  MP_FUNC_ADD_CHECK func;
  func = PostmanFuncTable[gift_type].add_check_func;
  if ( gpd != NULL && func )
  {
    return func( work, gamedata, gpd );
  }
  else
  {
    return FALSE;
  }
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎�������b�Z�[�W
//--------------------------------------------------------------
static u16 MP_Command_SuccessMsg( u8 gift_type, GIFT_PACK_DATA *gpd, SCRCMD_WORK * work )
{
  MP_FUNC_SUCCESS_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = PostmanFuncTable[gift_type].set_success_words;
  if ( gpd != NULL && func )
  {
    msg_id = func( wordset, gpd, work );
    return msg_id;
  }
  else
  {
    return msg_postman_08;  //������̃G���[���p
  }
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎��Ȃ��������b�Z�[�W
//--------------------------------------------------------------
static u16 MP_Command_FailureMsg( u8 gift_type, GIFT_PACK_DATA *gpd, SCRCMD_WORK * work )
{
  MP_FUNC_FAILURE_MSG func;
  u16 msg_id;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  WORDSET *wordset    = SCRIPT_GetWordSet( sc );

  func = PostmanFuncTable[gift_type].set_failure_words;
  if ( gpd != NULL && func )
  {
    msg_id = func( wordset, gpd, work );
    return msg_id;
  }
  else
  {
    return msg_postman_09;  //������̃G���[���p
  }
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨���󂯎�菈��
//--------------------------------------------------------------
static BOOL MP_Command_Receive(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{ //��ɓ���鏈��
  MP_FUNC_ADD func;
  func = PostmanFuncTable[gift_type].add_func;
  if ( gpd != NULL && func )
  {
    func( work, gamedata, gpd );
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F���蕨�̕��ގ擾
//--------------------------------------------------------------
static u16 MP_Command_GetType(
    u8 gift_type, SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  MP_FUNC_GET_TYPE func;
  func = PostmanFuncTable[gift_type].get_type;
  if ( gpd != NULL && func )
  {
    return func( work, gamedata, gpd );
  }
  return 0;
}

//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F�z�B����OBJID�擾
//--------------------------------------------------------------
static u16 MP_Command_ObjID( FIELDMAP_WORK * fieldmap, GAMEDATA * gamedata )
{
  MMDL * mmdl = searchPOSTMANOBJ( fieldmap );
  if ( mmdl != NULL )
  {
    return MMDL_GetOBJID( mmdl );
  }
  else
  {
    int obj_id = searchEventDataPostmanObj( gamedata );
    if ( obj_id >= 0 )
    {
      return obj_id;
    }
  }
  GF_ASSERT_MSG( 0, "�z�B�������݂��Ȃ��I�I\n" );
  return 0;
}
//--------------------------------------------------------------
/// �X�N���v�g�R�}���h�F�z�B��OBJ�����邩�ǂ����H�`�F�b�N
//--------------------------------------------------------------
static u16 MP_Command_ObjStatus( FIELDMAP_WORK * fieldmap )
{
  MMDL * mmdl = searchPOSTMANOBJ( fieldmap );
  if ( mmdl == NULL )
  {
    return SCR_POSTMAN_OBJ_NONE;
  }
  else 
  {
    return SCR_POSTMAN_OBJ_EXISTS;
  }
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �z�B��OBJ����
 *
 * ���ݑ��݂��Ă��铮�샂�f���f�[�^����z�B�����������|�C���^��Ԃ�
 */
//--------------------------------------------------------------
static MMDL * searchPOSTMANOBJ( FIELDMAP_WORK * fieldWork )
{
  u32 no = 0;
	MMDL *mmdl;
  MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		if( MMDL_GetOBJCode(mmdl) == DELIVERY ){
			return( mmdl );
		}
	}
  return NULL;
}
//--------------------------------------------------------------
/**
 * @brief �z�B��OBJ�f�[�^����
 *
 * �C�x���g�f�[�^����A�z�B��OBJ��������obj_id��Ԃ�
 */
//--------------------------------------------------------------
static int searchEventDataPostmanObj( GAMEDATA * gamedata )
{
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );
  const MMDL_HEADER * tbl = EVENTDATA_GetNpcData( evdata );
  int max = EVENTDATA_GetNpcCount( evdata );
  int i;

  if ( tbl == NULL || max == 0 ) return -1;
  
  for ( i = 0; i < max; i ++ )
  {
    if ( tbl[i].obj_code == DELIVERY ) return tbl[i].id;
  }
  return -1;
}
//--------------------------------------------------------------
/**
 * @brief �r�N�e�B�z�z�̕s�v�c�J�[�h�f�[�^���̃`�F�b�N
 */
//--------------------------------------------------------------
static BOOL isLibertyTicketCard( const GIFT_PACK_DATA * gpd, u8 gift_type )
{
  if ( gift_type != MYSTERYGIFT_TYPE_ITEM ) return FALSE;
  if ( gpd->event_id != MYSTERY_DATA_EVENT_LIBERTY ) return FALSE;
  if ( gpd->data.item.itemNo != ITEM_RIBATHITIKETTO	) return FALSE;
  return TRUE;
}
//======================================================================
//  �|�P�����擾�p�֐�
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static POKEMON_PARAM * createPokemon( SCRCMD_WORK * work, GIFT_PACK_DATA * gpd )
{
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  POKEMON_PARAM * pp;
  pp = MYSTERY_CreatePokemon( gpd, heapID, gamedata );
  return pp;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL PFuncCheckPokemon( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gamedata );
  return ( PokeParty_GetPokeCount( party ) < TEMOTI_POKEMAX );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void PFuncAddPokemon( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  POKEPARTY*   party = GAMEDATA_GetMyPokemon( gamedata );
  POKEMON_PARAM * pp = createPokemon( work, gpd );

  if ( PokeParty_GetPokeCount( party ) >= TEMOTI_POKEMAX || pp == NULL )
  {
    return;
  }

  PokeParty_Add( party, pp );
   //�ꉞ�}�ӓo�^
  ZUKANSAVE_SetPokeGet( GAMEDATA_GetZukanSave( gamedata ), pp );
  GFL_HEAP_FreeMemory( pp );
}

//--------------------------------------------------------------
/// �|�P�����̃^�C�v�擾
//--------------------------------------------------------------
static u16 PFuncGetTypePokemon(SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  POKEMON_PARAM * pp = createPokemon( work, gpd );
  BOOL tamago_flag;

  if ( pp == NULL )
  { //������̃G���[����
    return SCR_POSTMAN_TYPE_POKEMON;
  }

  tamago_flag = PP_Get( pp, ID_PARA_tamago_flag, NULL );

  GFL_HEAP_FreeMemory( pp );
  if ( tamago_flag == TRUE ) {
    return SCR_POSTMAN_TYPE_TAMAGO;
  } else {
    return SCR_POSTMAN_TYPE_POKEMON;
  }
}

//--------------------------------------------------------------
/**
 * @brief �|�P�����󂯎�萬�����b�Z�[�W�̐���
 * @attention ������̏ꍇMONSNO=0�̖��O��\������悤�ɂ��Ă���
 */
//--------------------------------------------------------------
static u16  PFuncSetSuccessPokemonWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  MYSTATUS * mystatus = GAMEDATA_GetMyStatus( SCRCMD_WORK_GetGameData( work ) );
  POKEMON_PARAM * pp = createPokemon( work, gpd );
  u16 msg_id = msg_postman_06;

  WORDSET_RegisterPlayerName( wordset, 0, mystatus );
  if ( pp == NULL ) {
    GF_ASSERT( 0 );
    WORDSET_RegisterPokeMonsNameNo( wordset, 1, 0 );
  } else {
    if ( PP_Get( pp, ID_PARA_tamago_flag, NULL ) == TRUE ) {
      TAMADA_Printf("Mystery Present TAMAGO!");
      msg_id = msg_postman_12;
    } else {
      WORDSET_RegisterPokeMonsName( wordset, 1, pp );
    }
    GFL_HEAP_FreeMemory( pp );
  }
  return msg_id;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetFailurePokemonWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  return msg_postman_07;
}

//======================================================================
//  �A�C�e���擾�p�֐�
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL PFuncCheckItem( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  //2010.06.04  tamada
  //�s���ȑ��蕨�z�z�΍�Ƃ��āA�ǂ�ȃA�C�e���ł��A�莝������t�ł��A
  //�󂯎��悤�ɂ���B�i�����łȂ��ƃJ�[�h�������Ȃ��j
  return TRUE;
#if 0
  u16 item_no;
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  item_no = MYSTERY_CreateItem(gpd);

  return MYITEM_AddCheck( myitem, item_no, 1, heapID );
#endif
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void PFuncAddItem( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  MYITEM_PTR myitem = GAMEDATA_GetMyItem( gamedata );

  u16 item_no = gpd->data.item.itemNo;
  BOOL result;

  if ( item_no == ITEM_DUMMY_DATA || item_no > ITEM_DATA_MAX )
  {
    GF_ASSERT( 0 );
    return;
  }
  result = MYITEM_AddItem( myitem, item_no, 1, heapID );
  //2010.06.04  tamada  �s�����蕨�΍�F�莝���ɉ������Ȃ��Ă�Assert�ɂ��Ȃ�
  //GF_ASSERT( result );
}
//--------------------------------------------------------------
/// �A�C�e���̃^�C�v�擾
//--------------------------------------------------------------
static u16 PFuncGetTypeItem(SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  if (isLibertyTicketCard( gpd, gpd->gift_type ) == TRUE ) {
    return SCR_POSTMAN_TYPE_LIBERTY_TICKET;
  } else {
    return SCR_POSTMAN_TYPE_NORMAL_ITEM;
  }
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetSuccessItemWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  MYSTATUS * mystatus = GAMEDATA_GetMyStatus( SCRCMD_WORK_GetGameData( work ) );
  u16 item_no = gpd->data.item.itemNo;
  WORDSET_RegisterPlayerName( wordset, 0, mystatus );
  WORDSET_RegisterItemName( wordset, 1, item_no );
  return msg_postman_08;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16  PFuncSetFailureItemWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  u16 item_no = gpd->data.item.itemNo;
  WORDSET_RegisterItemName( wordset, 0, item_no );

  return msg_postman_09;
}

//======================================================================
/// GPower�擾�p�֐�
//======================================================================
//--------------------------------------------------------------
/// GPower�擾�o���邩�H�`�F�b�N
//--------------------------------------------------------------
static BOOL PFuncCheckGPower( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  //GPower�͖������Ɏ擾����
  return TRUE;
}

//--------------------------------------------------------------
/// GPower�擾
//--------------------------------------------------------------
static void PFuncAddGPower( SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  INTRUDE_SAVE_WORK * intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( gamedata ) );
  GPOWER_ID gpower_id = gpd->data.gpower.type;
  ISC_SAVE_SetDistributionGPower( intsave, gpower_id );
}

//--------------------------------------------------------------
/// GPower�擾
//--------------------------------------------------------------
static u16 PFuncGetTypeGPower(SCRCMD_WORK * work, GAMEDATA * gamedata, GIFT_PACK_DATA * gpd )
{
  return SCR_POSTMAN_TYPE_GPOWER;
}

//--------------------------------------------------------------
/// GPower�擾�F�������b�Z�[�W
//--------------------------------------------------------------
static u16  PFuncSetSuccessGPowerWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  GAMEDATA * gamedata = SCRCMD_WORK_GetGameData( work );
  INTRUDE_SAVE_WORK * intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( gamedata ) );
  MYSTATUS * mystatus = GAMEDATA_GetMyStatus( gamedata );
  GPOWER_ID gpower_id = gpd->data.gpower.type;
  WORDSET_RegisterPlayerName( wordset, 0, mystatus );
  WORDSET_RegisterGPowerName( wordset, 1, gpower_id );
  return msg_postman_10;
}

//--------------------------------------------------------------
/// GPower�擾�F���s
//--------------------------------------------------------------
static u16  PFuncSetFailureGPowerWords( WORDSET * wordset, GIFT_PACK_DATA * gpd, SCRCMD_WORK * work )
{
  //���̃��b�Z�[�W��\�����邱�Ƃ͂Ȃ��͂�
  return msg_postman_11;
}

//======================================================================
//
//
//
//======================================================================
//--------------------------------------------------------------
//  ���蕨�̎�ޖ��̕���p�e�[�u��
//--------------------------------------------------------------
static const POSTMAN_FUNC_TABLE PostmanFuncTable[ MYSTERYGIFT_TYPE_MAX ] = {
  { 
    MYSTERYGIFT_TYPE_NONE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
  },
  { 
    MYSTERYGIFT_TYPE_POKEMON,
    PFuncCheckPokemon,
    PFuncAddPokemon,
    PFuncGetTypePokemon,
    PFuncSetSuccessPokemonWords,  //msg_postman_06 or msg_postman_12
    PFuncSetFailurePokemonWords,  //msg_postman_07
  },
  { 
    MYSTERYGIFT_TYPE_ITEM,
    PFuncCheckItem,
    PFuncAddItem,
    PFuncGetTypeItem,
    PFuncSetSuccessItemWords, //msg_postman_08
    PFuncSetFailureItemWords, //msg_postman_09
  },
  { 
    MYSTERYGIFT_TYPE_POWER,
    PFuncCheckGPower,
    PFuncAddGPower,
    PFuncGetTypeGPower,
    PFuncSetSuccessGPowerWords, //msg_postman_10
    PFuncSetFailureGPowerWords, //msg_postman_11
  },
};

