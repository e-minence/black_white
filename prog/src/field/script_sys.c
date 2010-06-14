//============================================================================================
/**
 * @file	script_sys.c
 * @brief	�X�N���v�g���䃁�C������
 * @author	tamada GAMEFREAK inc.
 * @date	2010.01.25
 *
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "system/vm.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_local.h"

#include "script_def.h"
#include "scrcmd.h"     //ScriptCmdTbl  ScriptCmdMax
#include "scrcmd_work.h"
#include "eventwork_def.h"

#include "field/zonedata.h"

#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "system/main.h"  //HEAPID_PROC

#include "../../../resource/fldmapdata/script/init_scr_def.h" //SCRID_INIT_SCRIPT
#include "../../../resource/fldmapdata/script/common_scr_def.h" //SCRID_DUMMY

// scr_offset.cdat���ŎQ��
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"
#include "script_message.naix"  //NARC_script_message_
#ifdef  PM_DEBUG
#include "debug_message.naix"   //NARC_debug_message_
#endif
#include "arc/fieldmap/script_seq.naix" // NARC_script_seq_

#include "script_debugger.h"
//============================================================================================
//  define
//============================================================================================

#define SCRIPT_MSG_NON (0xffffffff)

#define SCRIPT_NOT_EXIST_ID (0xffff)

#ifdef  PM_DEBUG

 #if defined(DEBUG_ONLY_FOR_tamada) || defined(DEBUG_ONLY_FOR_masafumi_saitou) || defined(DEBUG_ONLY_FOR_mizuguchi_mai) || defined(DEBUG_ONLY_FOR_suginaka_katsunori) || defined(DEBUG_ONLY_FOR_murakami_naoto)
  #define SCRIPT_Printf( ... )  OS_Printf( __VA_ARGS__ )
 #else
  #define SCRIPT_Printf( ... )  ((void)0)
 #endif

#else //PM_DEBUG
 #define SCRIPT_Printf( ... )  ((void)0)
#endif  //PM_DEBUG

//============================================================================================
//	struct
//============================================================================================
//�֐��|�C���^�^
typedef void (*SCRIPT_EVENTFUNC)(GMEVENT *event, void * work);

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�\����
 */
//--------------------------------------------------------------
struct _SCRIPTSYS{

  HEAPID heapID;
	u8 vm_machine_count;	//�ǉ��������z�}�V���̐�
	VMHANDLE *vm[VMHANDLE_ID_MAX];	//���z�}�V���ւ̃|�C���^
	
	SCRIPT_EVENTFUNC next_func;		//�X�N���v�g�I�����ɌĂяo�����֐�
  void * next_func_params;      //�X�N���v�g�I�����ɌĂяo�����֐��ɓn������

};

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================

//--------------------------------------------------------------
//--------------------------------------------------------------
static SCRIPTSYS * SCRIPTSYS_Create( HEAPID heapID );
static void SCRIPTSYS_Delete( SCRIPTSYS * scrsys );
static BOOL SCRIPTSYS_Run( SCRIPTSYS * scrsys );
static VMHANDLE * SCRIPTSYS_GetVM( SCRIPTSYS *scrsys, VMHANDLE_ID vm_id );
static VMHANDLE_ID SCRIPTSYS_AddVM( SCRIPTSYS * scrsys, VMHANDLE * vm );
static void SCRIPTSYS_RemoveVM( SCRIPTSYS *scrsys, VMHANDLE_ID vm_id );
static void SCRIPTSYS_EntryNextFunc( SCRIPTSYS *scrsys, GMEVENT * next_event );
static void SCRIPTSYS_CallNextFunc( SCRIPTSYS *scrsys, GMEVENT * event );
static BOOL SCRIPTSYS_HasNextFunc( const SCRIPTSYS *scrsys );

//�C�x���g
static GMEVENT * createScriptEvent(
    GAMESYS_WORK * gsys, u16 temp_heapID, u16 scr_id, MMDL * obj, void * ret_script_wk );

//--------------------------------------------------------------
//����X�N���v�g
//--------------------------------------------------------------
static BOOL searchMapInitScript( GAMESYS_WORK * gsys, HEAPID heapID, u8 key);
static u16 searchSpecialScript( const u8 * p, u8 key );
static u16 searchSceneScript( GAMEDATA * gamedata, const u8 * p, u8 key );

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMHANDLE * SCRVM_Create(
    HEAPID main_heapID, SCRIPT_WORK *sc, u16 zone_id, u16 scr_id, BOOL is_sp_flag );
static void SCRVM_Delete( VMHANDLE* core );


//==============================================================================================
//
//	�X�N���v�g����
//
//	���X�N���v�g�C�x���g�Z�b�g
//		EventSet_Script(...)
//
//	�������̗���
//		SCRIPT_SetEventScript(...)						�C�x���g�ݒ�
//		  createScriptEvent(...)
//  		  SCRIPTSYS_Create()
//  		  SCRIPTWORK_Create(...)
//		    SCRIPT_AddVMachine(...)						VM�ǉ�
//				��
//		scriptEvent(...)				�C�x���g����֐�
//		    SCRIPTSYS_Run(...)
//				��
//		SCRIPTWORK_Delete(...)
//		SCRIPTSYS_Delete(...)
//
//
//	�����z�}�V��(VMHANDLE)
//		�E���z�}�V���ǉ�(SCRIPT_AddVMachine)
//		�E���z�}�V���̐�(vm_machine_count)��ǉ�
//
//	���X�N���v�g���䃁�C��(scriptEvent)
//		�E���z�}�V���̐�(vm_machine_count)���`�F�b�N���ăC�x���g�I��
//
//==============================================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�Z�b�g
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	none
 */
//--------------------------------------------------------------
GMEVENT * SCRIPT_SetEventScript( GAMESYS_WORK *gsys, u16 scr_id, MMDL *obj,
		HEAPID temp_heapID )
{
	GMEVENT *event;

  event = createScriptEvent( gsys, temp_heapID, scr_id, obj, NULL );
	return event;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�R�[��
 * @param	event		GMEVENT_CONTROL�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @param	ret_script_wk	�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @retval	none
 */
//--------------------------------------------------------------
SCRIPT_WORK * SCRIPT_CallScript( GMEVENT *event,
	u16 scr_id, MMDL *obj, void *ret_script_wk, HEAPID temp_heapID )
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork( event );
	GMEVENT *sc_event;
  EVENT_SCRIPT_WORK * ev_sc;

  sc_event = createScriptEvent( gsys, temp_heapID, scr_id, obj, ret_script_wk );
  ev_sc = GMEVENT_GetEventWork( sc_event );

	GMEVENT_CallEvent( event, sc_event );
  return ev_sc->sc;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�ؑ�
 *
 * @param	event		GMEVENT_CONTROL�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 *
 * @retval	none
 *
 * ���̃C�x���g����X�N���v�g�ւ̐ؑւ��s��
 */
//--------------------------------------------------------------
SCRIPT_WORK * SCRIPT_ChangeScript( GMEVENT *event,
		u16 scr_id, MMDL *obj, HEAPID temp_heapID )
{
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork( event );
	GMEVENT *sc_event;
  EVENT_SCRIPT_WORK * ev_sc;

  sc_event = createScriptEvent( gsys, temp_heapID, scr_id, obj, NULL );
  ev_sc = GMEVENT_GetEventWork( sc_event );

  GMEVENT_ChangeEvent( event, sc_event );
  return ev_sc->sc;
}

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�����s����Ă��邩�ǂ����̃`�F�b�N
 */
//--------------------------------------------------------------
BOOL SCRIPT_GetVMExists( SCRIPT_WORK *sc, VMHANDLE_ID vm_id )
{
  SCRIPTSYS * scrsys = SCRIPT_GetScriptSys( sc );
  if ( scrsys == NULL ) return FALSE;
  return ( SCRIPTSYS_GetVM( scrsys, vm_id ) != NULL);
}

//============================================================================================
//
//
//    �C�x���g�X�N���v�g�֘A
//
//
//============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static SCRIPTSYS * SCRIPTSYS_Create( HEAPID heapID )
{
  SCRIPTSYS * scrsys = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCRIPTSYS) );
  scrsys->heapID = heapID;
  return scrsys;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void SCRIPTSYS_Delete( SCRIPTSYS * scrsys )
{
  GFL_HEAP_FreeMemory( scrsys );
}

//--------------------------------------------------------------
//���z�}�V���R���g���[��
//--------------------------------------------------------------
static BOOL SCRIPTSYS_Run( SCRIPTSYS * scrsys )
{
  int vm_id;
  for( vm_id = 0; vm_id < VMHANDLE_ID_MAX; vm_id++ )
  {
    VMHANDLE * vm = scrsys->vm[ vm_id ];
    if( vm != NULL )
    {
      if( VM_Control(vm) == FALSE )
      {	//���䃁�C��
        scrsys->vm[vm_id] = NULL;
        scrsys->vm_machine_count--;
        SCRVM_Delete( vm );				//�X�N���v�g�폜
      }
    }
  }

  //���z�}�V���̐����`�F�b�N
  if( scrsys->vm_machine_count > 0 )
  {
    return TRUE;
  } 
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief	���z�}�V���ǉ�
 * @param sc      SCRIPT_WORK�ւ̃|�C���^
 * @param vm      �ǉ�����VMHANDLE�ւ̃|�C���^
 * @param vm_id   VMHANDLE_ID (�X�N���v�g�pVM�w��ID�j
 */
//--------------------------------------------------------------
static VMHANDLE_ID SCRIPTSYS_AddVM( SCRIPTSYS * scrsys, VMHANDLE * vm )
{
  int id;
  for ( id = 0; id < VMHANDLE_ID_MAX; id++)
  {
    if ( scrsys->vm[id] == NULL )
    {
      scrsys->vm[id] = vm;
      scrsys->vm_machine_count ++;
      return id;
    }
  }
  GF_ASSERT( 0 );
  return VMHANDLE_ID_MAX;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static VMHANDLE * SCRIPTSYS_GetVM( SCRIPTSYS *scrsys, VMHANDLE_ID vm_id )
{
  GF_ASSERT( vm_id < VMHANDLE_ID_MAX );
  return scrsys->vm[ vm_id ];
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void changeEventFunc( GMEVENT * event, void * work)
{
  GMEVENT * next_event = work;
  GMEVENT_ChangeEvent( event, next_event );
}
//--------------------------------------------------------------
/**
 * @brief �C�x���g�Ăяo���\��
 * �X�N���v�g�I����A�o�^�����C�x���g�ɑJ�ڂ���
 */
//--------------------------------------------------------------
static void SCRIPTSYS_EntryNextFunc( SCRIPTSYS *scrsys, GMEVENT * next_event )
{
  GF_ASSERT(scrsys->next_func == NULL);
  GF_ASSERT(scrsys->next_func_params == NULL);
  scrsys->next_func = changeEventFunc;
  scrsys->next_func_params = (void*)next_event;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void SCRIPTSYS_CallNextFunc( SCRIPTSYS *scrsys, GMEVENT * event )
{
  scrsys->next_func( event, scrsys->next_func_params );
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL SCRIPTSYS_HasNextFunc( const SCRIPTSYS *scrsys )
{
  return (scrsys->next_func != NULL );
}


//============================================================================================
//	�C�x���g
//============================================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static u16 getZoneID( GAMESYS_WORK * gsys )
{
  PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork( gsys );
  return PLAYERWORK_getZoneID( pw );
}
//--------------------------------------------------------------
/**
 * �X�N���v�g����C�x���g
 * @param	event	GMEVENT
 * @param	seq	event seq
 * @param	wk	event wk
 * @retval	GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT scriptEvent(
	GMEVENT *event, int *seq, void *wk )
{
	int i;
	EVENT_SCRIPT_WORK *ev_wk = wk;
	SCRIPT_WORK *sc = ev_wk->sc;
  SCRIPTSYS * scrsys = ev_wk->scrsys;
	
  //���z�}�V���R���g���[��
  if ( SCRIPTSYS_Run( scrsys ) == TRUE )
  {
    return GMEVENT_RES_CONTINUE;
  }

  SCRIPTWORK_Delete( sc );		//�X�N���v�g���[�N�폜
  
  if( SCRIPTSYS_HasNextFunc( scrsys ) == TRUE )
  {
    //�X�N���v�g�I�����ɌĂяo�����֐����s
    SCRIPTSYS_CallNextFunc( scrsys, event );
    SCRIPTSYS_Delete( scrsys );
	  return GMEVENT_RES_CONTINUE;
  }
  else
  { 
    SCRIPTSYS_Delete( scrsys );
    return GMEVENT_RES_FINISH;
  }
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�Z�b�g
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * createScriptEvent(
    GAMESYS_WORK * gsys, u16 temp_heapID, u16 scr_id, MMDL * obj, void * ret_script_wk )
{
  GMEVENT * event;
	EVENT_SCRIPT_WORK *ev_sc;
  u16 zone_id;

	event = GMEVENT_Create( gsys, NULL, scriptEvent, sizeof(EVENT_SCRIPT_WORK) );
	ev_sc = GMEVENT_GetEventWork( event );
  ev_sc->scrsys = SCRIPTSYS_Create( HEAPID_PROC );
  ev_sc->sc = SCRIPTWORK_Create( HEAPID_PROC, gsys, event, scr_id, ret_script_wk, FALSE );
  SCRIPT_SetTargetObj( ev_sc->sc, obj );
	
  zone_id = getZoneID( gsys );
  //���z�}�V���ǉ�
  SCRIPT_AddVMachine( ev_sc->sc, zone_id, scr_id );
		
	return event;
}

//==============================================================================================
//
//
//  �X�N���v�g�E�X�N���v�g�R�}���h�������΂�鐧��֐�
//
//
//==============================================================================================
//--------------------------------------------------------------
/**
 * @brief �C�x���g�Ăяo��
 * �C�x���g�I����A�ĂуX�N���v�g���s�ɖ߂�
 */
//--------------------------------------------------------------
void SCRIPT_CallEvent( SCRIPT_WORK *sc, GMEVENT * call_event )
{
  GMEVENT_CallEvent( SCRIPT_GetEvent( sc ), call_event );
}

//--------------------------------------------------------------
/**
 * @brief �C�x���g�Ăяo���\��
 * �X�N���v�g�I����A�o�^�����C�x���g�ɑJ�ڂ���
 */
//--------------------------------------------------------------
void SCRIPT_EntryNextEvent( SCRIPT_WORK *sc, GMEVENT * next_event )
{
  SCRIPTSYS * scrsys = SCRIPT_GetScriptSys( sc );

  if ( scrsys )
  {
    SCRIPTSYS_EntryNextFunc( scrsys, next_event );
  }
  else
  {
    GF_ASSERT( 0 ); //SCRIPTSYS�����݂��Ȃ����͓��삳���Ȃ�
  }
}

//--------------------------------------------------------------
/**
 * @brief ���z�}�V���̒ǉ�
 */
//--------------------------------------------------------------
VMHANDLE_ID SCRIPT_AddVMachine( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id )
{
  SCRIPTSYS * scrsys = SCRIPT_GetScriptSys( sc );
  if ( scrsys )
  {
    VMHANDLE * vm;
    VMHANDLE_ID get_id;
    vm = SCRVM_Create( scrsys->heapID, sc, zone_id, scr_id, FALSE );
    get_id = SCRIPTSYS_AddVM( scrsys, vm );
    return get_id;
  }
  else
  {
    GF_ASSERT( 0 ); //SCRIPTSYS�����݂��Ȃ����͓��삳���Ȃ�
    return VMHANDLE_ID_MAX;
  }
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g����X�N���v�g���[�N���擾
 * @param	GMEVENT *event
 * @retval  SCRIPT_WORK*
 */
//--------------------------------------------------------------
SCRIPT_WORK* SCRIPT_GetScriptWorkFromEvent( GMEVENT *event )
{
	EVENT_SCRIPT_WORK *ev_sc = GMEVENT_GetEventWork( event );
	return ev_sc->sc;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
SCRIPTSYS * SCRIPT_GetScriptSysFromEvent( GMEVENT *event )
{
	EVENT_SCRIPT_WORK *ev_sc = GMEVENT_GetEventWork( event );
	return ev_sc->scrsys;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
SCRIPTSYS * SCRIPT_GetScriptSys( SCRIPT_WORK * sc )
{
  GMEVENT * event;
  EVENT_SCRIPT_WORK * ev_sc;
  event = SCRIPT_GetEvent( sc );
  if( event == NULL )
  {
    GF_ASSERT( 0 );
    return NULL;
  }
  ev_sc = GMEVENT_GetEventWork( event );
  return ev_sc->scrsys;
}



//============================================================================================
//	�t���O�֘A
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�}�b�v������̃Z�[�u�t���O���N���A����
 * @return	none
 */
//------------------------------------------------------------------
static void clearLocalEventFlags( EVENTWORK * evwork )
{
	//���[�J���t���O�̃N���A
  EVENTWORK_ClearEventFlags( evwork, FH_AREA_START, FH_AREA_END );
	//���[�J�����[�N�̃N���A
  EVENTWORK_ClearEventWorks( evwork, LOCALWORKAREA_START, LOCALWORKAREA_END );
}

//============================================================================================
//
//
//	����X�N���v�g�֘A
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param heapID    �e���|�����Ɏg�p����q�[�v�w��
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_CallGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID )
{
  SCRIPT_CallSpecialScript( gsys, NULL, heapID, SCRID_INIT_SCRIPT );
}

#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param heapID    �e���|�����Ɏg�p����q�[�v�w��
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_CallDebugGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID )
{
  SCRIPT_CallSpecialScript( gsys, NULL, heapID, SCRID_INIT_DEBUG_SCRIPT );
}
#endif  //PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�������̃X�N���v�g���s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param heapID    �e���|�����Ɏg�p����q�[�v�w��
 */
//------------------------------------------------------------------
void SCRIPT_CallGameClearScript( GAMESYS_WORK *gsys, HEAPID heapID )
{
  SCRIPT_CallSpecialScript( gsys, NULL, heapID, SCRID_INIT_GAMECLEAR_SCRIPT );
}

//------------------------------------------------------------------
/**
 * @brief	EVENT�ł͂Ȃ�����X�N���v�g���s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param sc        �X�N���v�g���[�N�ւ̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_CallSpecialScript( GAMESYS_WORK *gsys, SCRIPT_WORK * sc, HEAPID heapID, u16 script_id )
{
  VMHANDLE *core = NULL;
  SCRIPT_WORK *sc_local = NULL;
  u16 zone_id;
  
  if ( sc == NULL )
  {
    sc_local = SCRIPTWORK_Create( HEAPID_PROC, gsys, NULL, script_id, NULL, TRUE );
    sc = sc_local;
  }
  
  zone_id = getZoneID( gsys );
  core = SCRVM_Create( HEAPID_PROC, sc, zone_id, script_id, TRUE );
  while( VM_Control(core) == TRUE ){};
  SCRVM_Delete( core );

  if ( sc_local )
  {
    SCRIPTWORK_Delete( sc_local );
  }
}

//------------------------------------------------------------------
/**
 * @brief	����X�N���v�g�������Ď��s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param	key			����X�N���v�gID
 * @return	"TRUE=����X�N���v�g���s�AFALSE=�������Ȃ�"
 */
//------------------------------------------------------------------
static BOOL searchMapInitScript( GAMESYS_WORK * gsys, HEAPID heapID, u8 key)
{
  u16 scr_id;

  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );
  const u8 * p = EVENTDATA_GetSpecialScriptData(evdata);

  GF_ASSERT(key != SP_SCRID_SCENE_CHANGE);
  scr_id = searchSpecialScript( p, key );
	if( scr_id == SCRIPT_NOT_EXIST_ID )
  {
		return FALSE;
	}

  //����X�N���v�g���s
  SCRIPT_CallSpecialScript( gsys, NULL, heapID, scr_id );
	return TRUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL SCRIPT_CallFieldInitScript( GAMESYS_WORK * gsys, HEAPID heapID )
{
  return searchMapInitScript( gsys, heapID, SP_SCRID_FIELD_INIT );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL SCRIPT_CallFieldRecoverScript( GAMESYS_WORK * gsys, HEAPID heapID )
{
  return searchMapInitScript( gsys, heapID, SP_SCRID_FIELD_RECOVER );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL SCRIPT_CallZoneChangeScript( GAMESYS_WORK * gsys, HEAPID heapID)
{
  EVENTWORK * ev = GAMEDATA_GetEventWork( GAMESYSTEM_GetGameData(gsys) );
  clearLocalEventFlags(ev);
  return searchMapInitScript( gsys, heapID, SP_SCRID_ZONE_CHANGE );
}

//------------------------------------------------------------------
/**
 * @brief   �V�[�������C�x���g�̌���
 * @param gsys
 * @param heapID
 * @retval  GMEVENT �V�[�������C�x���g�ւ̃|�C���^
 * @retval  NULL�̂Ƃ��A�C�x���g�Ȃ�
 */
//------------------------------------------------------------------
GMEVENT * SCRIPT_SearchSceneScript( GAMESYS_WORK * gsys, HEAPID heapID )
{
  u16 scr_id;
  GMEVENT * event;

  GAMEDATA * gamedata = GAMESYSTEM_GetGameData( gsys );
  EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );
  const u8 * p = EVENTDATA_GetSpecialScriptData(evdata);

  scr_id = searchSceneScript( gamedata, p, SP_SCRID_SCENE_CHANGE );
	if( scr_id == SCRIPT_NOT_EXIST_ID )
  {
		return NULL;
  }
  event = SCRIPT_SetEventScript( gsys, scr_id, NULL, heapID );
  return event;
}



//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u16 GETU16( const u8 * p )
{
  return p[0] + ( p[1] << 8 );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u32 GETU32( const u8 * p )
{
  return p[0] + ( p[1] << 8 ) + ( p[2] << 16 ) + ( p[3] << 24 );
}
//------------------------------------------------------------------
/**
 * @brief	�w�b�_�[�f�[�^����w��^�C�v�̂��̂�����
 * @param	key			����X�N���v�gID
 * @return	"�X�N���v�gID"
 */
//------------------------------------------------------------------
static u16 searchSpecialScript( const u8 * p, u8 key )
{
  while (TRUE)
  {
    if ( GETU16(p) == SP_SCRID_NONE )
    {
      return SCRIPT_NOT_EXIST_ID;
    }
		//�w��^�C�v�̓���X�N���v�g����
    if ( GETU16(p) == key )
    {
      p+= sizeof(u16); //����X�N���v�gID������
      return GETU16( p );		//�X�N���v�gID��Ԃ�
    }
    //����X�N���v�gID+�X�N���v�gID+�_�~�[������
    p += (sizeof(u16) + sizeof(u16) + sizeof(u16));
  }
  return SCRIPT_NOT_EXIST_ID;                //����������Ȃ�����
}

//------------------------------------------------------------------
/**
 * @brief	�w�b�_�[�f�[�^����w��^�C�v�̂��̂���������(�V�[���C�x���g)
 * @param	key			����X�N���v�gID
 * @return	"�X�N���v�gID"
 */
//------------------------------------------------------------------
static u16 searchSceneScript( GAMEDATA * gamedata, const u8 * p, u8 key )
{
	u16 work1,work2;
	u32 pos;
	EVENTWORK *ev;

	pos = 0;

	while( TRUE )
  {
		//�I���L�q�`�F�b�N(SP_EVENT_DATA_END)
		if( GETU16( p ) == SP_SCRID_NONE ){
			return SCRIPT_NOT_EXIST_ID;
		}

		//�w��^�C�v�̓���X�N���v�g����
		if( GETU16( p ) == key ){
			p += sizeof(u16);			//����X�N���v�gID������
			pos = GETU32( p );
			p += sizeof(u32);			//�A�h���X������
			break;
		}

		p += ( sizeof(u16) + sizeof(u32) );		//����X�N���v�gID+�A�h���X������
	}

	//������Ȃ�������
	if( pos == 0 ){
		return SCRIPT_NOT_EXIST_ID;
	}

	//����X�N���v�g�e�[�u�����Z�b�g
	p = (p + pos);

	ev = GAMEDATA_GetEventWork( gamedata );
	while ( TRUE ) {
		//�I���L�q�`�F�b�N(SP_SCRIPT_END)
		if( GETU16( p ) == 0 ){
			return SCRIPT_NOT_EXIST_ID;
		}

		//��r���郏�[�N�擾
		work1 = GETU16( p );
		if( work1 == 0 ){
			return SCRIPT_NOT_EXIST_ID;
		};
		p += sizeof(u16);									//���[�N������

		//��r����l(���[�N��)�擾
		work2 = GETU16( p );
		p += sizeof(u16);									//��r����l������(���[�N��)

		//�����`�F�b�N
		if( *(EVENTWORK_GetEventWorkAdrs(ev,work1)) == work2 ){
			return GETU16( p );
		}

		p += sizeof(u16);									//�X�N���v�gID������
	}
	return SCRIPT_NOT_EXIST_ID;								//����������Ȃ�����
}



//============================================================================================
//
//
//      �X�N���v�g�R�A�F���z�}�V�������E�폜
//
//
//============================================================================================
static u16 getScriptIndex( u32 zone_id, u16 scr_id, u16 * scr_idx, u16 * msg_arc, u16 * msg_idx );
static VM_CODE * loadScriptCodeData( u32 scr_id, HEAPID heapID );

//--------------------------------------------------------------
/**
 * @brief	���z�}�V������
 * @param main_heapID
 * @param sc
 * @param zone_id
 * @param scr_id
 * @param is_sp_flag
 * @retval	VMHANDLE
 */
//--------------------------------------------------------------
static VMHANDLE * SCRVM_Create(
    HEAPID main_heapID, SCRIPT_WORK *sc, u16 zone_id, u16 scr_id, BOOL is_sp_flag )
{
	VMHANDLE *core;
	SCRCMD_WORK *work;
	
  {
    SCRCMD_WORK_HEADER head;
    head.zone_id = zone_id;
    head.script_id = scr_id;
    head.sp_flag = is_sp_flag;
    head.gsys = SCRIPT_GetGameSysWork( sc );
    head.gdata = GAMESYSTEM_GetGameData( head.gsys );
    head.mmdlsys = GAMEDATA_GetMMdlSys( head.gdata );
    head.script_work = sc;

    work = SCRCMD_WORK_Create( &head, main_heapID );
  }
	
  {
    VM_INITIALIZER init;
    init.stack_size = 0x0100;
    init.reg_size = 0x040;
    init.command_table = ScriptCmdTbl;
    init.command_max  = ScriptCmdMax;
	
    core = VM_Create( main_heapID, &init );	//���z�}�V��������
  }
	
	VM_Init( core, work );
  {
    u16 scr_idx;
    u16 msg_arc;
    u16 msg_idx;
    u16 local_scr_id;
    //�X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^�ǂݍ���
    local_scr_id = getScriptIndex( zone_id, scr_id, &scr_idx, &msg_arc, &msg_idx );
    core->pScript = loadScriptCodeData( scr_idx, main_heapID );
    if( is_sp_flag ==FALSE && msg_idx != SCRIPT_MSG_NON )
    {
      SCRCMD_WORK_CreateMsgData( work, msg_arc, msg_idx );
    }
    VM_Start( core, core->pScript ); //���z�}�V���ɃR�[�h�ݒ�
    //�X�N���v�g�̐擪�����̓e�[�u���ɂȂ��Ă���̂ŁA
    //�I�t�Z�b�g�Ŏ��ۂ̊J�n�ʒu��ݒ肷��
    core->adrs += (local_scr_id * sizeof(u32));			//ID���i�߂�(adrs��long�Ȃ̂�*4)
    core->adrs += VMGetU32( core );		//���x���I�t�Z�b�g���i�߂�
  }
	return core;
}
//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g���ʍ폜����
 *
 * @param	core		VMHANDLE�^�̃|�C���^
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void SCRVM_Delete( VMHANDLE *core )
{
	SCRCMD_WORK *cmd = VM_GetContext( core );
	SCRCMD_WORK_Delete( cmd );
	GFL_HEAP_FreeMemory( core->pScript );
	VM_Delete( core );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
	u16 scr_id_start;
  u16 scr_id_end;
	u16 scr_arc_id;
  u16 msg_arc;
	u16 msg_idx;
}SCRIPT_ARC_TABLE;

//ScriptArcTable���`����t�@�C��
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset.cdat"

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�gID����X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^��ǂݍ���
 * @param	work
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param zone_id
 * @param	scr_id			�X�N���v�gID
 * @retval	"�X�N���v�gID����I�t�Z�b�g���������l"
 */
//--------------------------------------------------------------
static u16 getScriptIndex( u32 zone_id, u16 scr_id, u16 * scr_idx, u16 * msg_arc, u16 * msg_idx )
{
  int i;
  u16 local_scr_id = scr_id;

#ifdef DEBUG_SCRIPT
  {
    char buf[ZONEDATA_NAME_LENGTH];
    ZONEDATA_DEBUG_GetZoneName(buf, zone_id);
    SCRIPT_Printf( "ZONE:%s SCRID:%d\n", buf, scr_id);
  }
#endif

  for (i = 0; i < NELEMS(ScriptArcTable); i++)
  {
    const SCRIPT_ARC_TABLE * tbl = ScriptArcTable;
    if (scr_id >= tbl[i].scr_id_start)
    {
      GF_ASSERT_MSG(scr_id <= tbl[i].scr_id_end,"id=%d end=%d",scr_id,tbl[i].scr_id_end);
      *scr_idx = tbl[i].scr_arc_id;
      //*msg_arc = ARCID_SCRIPT_MESSAGE;
      *msg_arc = tbl[i].msg_arc;
      *msg_idx = tbl[i].msg_idx;
      local_scr_id -= tbl[i].scr_id_start;
      SCRIPT_Printf( "���ʃX�N���v�g�N�� scr_arc_idx = %d, msg_idx = %d, scr_id = %d\n",
        tbl[i].scr_arc_id, tbl[i].msg_idx, local_scr_id );
      return local_scr_id;
    }
  }

  //���[�J���X�N���v�g��scr_ID���n���ꂽ��
  if( scr_id >= ID_START_SCR_OFFSET )
  {
    u16 idx_script = ZONEDATA_GetScriptArcID( zone_id );
    u16 idx_msg = ZONEDATA_GetMessageArcID( zone_id );
    *scr_idx = idx_script;
    *msg_arc = ARCID_SCRIPT_MESSAGE;
    *msg_idx = idx_msg;
    local_scr_id -= ID_START_SCR_OFFSET;
    SCRIPT_Printf( "�]�[���X�N���v�g�N�� scr_idx = %d, msg_idx = %d\n",
        idx_script, idx_msg );
    return local_scr_id;
  }

  //SCRID_NULL(0)���n���ꂽ��
  *scr_idx = NARC_script_seq_common_scr_bin;
  *msg_arc = ARCID_SCRIPT_MESSAGE;
  *msg_idx = NARC_script_message_common_scr_dat;
  local_scr_id = 0;
  return local_scr_id;
}


//----------------------------------------------------------------
/**
 * �X�N���v�g�f�[�^�Z�b�g
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param	scr_id		�X�N���v�g�f�[�^��ID
 */
//----------------------------------------------------------------
static VM_CODE * loadScriptCodeData( u32 scr_id, HEAPID heapID )
{
	//���ʃX�N���v�g�f�[�^���[�h
  VM_CODE * script = NULL;
#ifdef  PM_DEBUG
  script = SCRDEBUGGER_ReadScriptFile( heapID, scr_id );
#endif
  if ( script == NULL )
  {
    script = GFL_ARC_UTIL_Load( ARCID_SCRSEQ, scr_id, 0, heapID );
  }
  return (VM_CODE *) script;
}

//------------------------------------------------------------------
/**
 * @brief �X�N���v�gID�̃`�F�b�N
 * @param script_id
 * @return  BOOL  FALSE�̂Ƃ��A���s�\�ȃX�N���v�g
 *
 * @attention  �����ւ̔��Y�^�F
 * @attention  �}�b�v�̃X�N���v�g���܂߂āA�����Ȕ͈̓`�F�b�N���s���Ė\�����Ȃ��悤�ɐ�������B
 * @attention ���݂̓o�C�i���������傫�����ƁA�X�N���v�g�擾�\���̑傫�ȉ����ƂȂ邱�Ƃ��猩����B
 */
//------------------------------------------------------------------
BOOL SCRIPT_IsValidScriptID( u16 script_id )
{
  if ( script_id == SCRID_DUMMY ) return FALSE;
  if ( script_id >= ScriptArcTable[0].scr_id_end )
  {
    GF_ASSERT(0);
    return FALSE;
  }

  /* �����ɔ͈̓`�F�b�N������H */

  return TRUE;
}

