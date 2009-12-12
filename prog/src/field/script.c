//======================================================================
/**
 * @file	script.c
 * @brief	�X�N���v�g���䃁�C������
 * @author	Satoshi Nohara
 * @date	05.08.04
 *
 * 01.11.07	Sousuke Tamada
 * 03.04.15	Satoshi Nohara
 * 05.04.25 Hiroyuki Nakamura
 */
//======================================================================
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

#include "print/wordset.h"    //WORDSET

#include "trainer_eye_data.h"   //EV_TRAINER_EYE_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG
#include "battle/battle.h"

#include "musical/musical_event.h"  //MUSICAL_EVENT_WORK

#include "system/main.h"  //HEAPID_PROC

#include "../../../resource/fldmapdata/script/init_scr_def.h" //SCRID_INIT_SCRIPT

// scr_offset.cdat���ŎQ��
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"
#include "script_message.naix"
#include "arc/fieldmap/script_seq.naix"

//======================================================================
//  define
//======================================================================
#define TRAINER_EYE_HITMAX (2) ///<�g���[�i�[�����f�[�^�ő吔

enum
{
	WORDSET_SCRIPT_SETNUM = 32,		//�f�t�H���g�o�b�t�@��
	WORDSET_SCRIPT_BUFLEN = 64,		//�f�t�H���g�o�b�t�@���i�������j
};

//#define SCR_MSG_BUF_SIZE	(1024)				//���b�Z�[�W�o�b�t�@�T�C�Y
#define SCR_MSG_BUF_SIZE	(512)				//���b�Z�[�W�o�b�t�@�T�C�Y

#define SCRIPT_MAGIC_NO		(0x3643f)

#define SCRIPT_MSG_NON (0xffffffff)

#define SCRIPT_NOT_EXIST_ID (0xffff)

enum {
  TEMP_WORK_START = SCWK_AREA_START,
  TEMP_WORK_END = USERWK_AREA_END,
  TEMP_WORK_SIZE = TEMP_WORK_END - TEMP_WORK_START,
};

//======================================================================
//	struct
//======================================================================
//�֐��|�C���^�^
typedef void (*SCRIPT_EVENTFUNC)(GMEVENT *event, void * work);

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�\����
 */
//--------------------------------------------------------------
typedef struct {

	u8 vm_machine_count;	//�ǉ��������z�}�V���̐�
	VMHANDLE *vm[VMHANDLE_MAX];	//���z�}�V���ւ̃|�C���^
	
	SCRIPT_EVENTFUNC next_func;		//�X�N���v�g�I�����ɌĂяo�����֐�
  void * next_func_params;      //�X�N���v�g�I�����ɌĂяo�����֐��ɓn������

}SCRIPTSYS;

//--------------------------------------------------------------
//
//--------------------------------------------------------------
struct _TAG_SCRIPT_WORK
{
  //��������������������������������������������������������
  //script.c ��Private�ȃ����o
  //��������������������������������������������������������
	u32 magic_no;			//�C�x���g�̃��[�N���X�N���v�g���䃏�[�N���𔻕�
	
  SCRIPTSYS * scr_sys;

  //��������������������������������������������������������
  //SCRIPT_WORK�������Ɉ����ŏ���������AReadOnly�ŊO�����J����郁���o
  //��������������������������������������������������������
	u16 start_scr_id;			//���C���̃X�N���v�gID
  u16 start_zone_id;
	MMDL *target_obj;

	HEAPID main_heapID;
  HEAPID temp_heapID;

	GAMESYS_WORK *gsys;
  GMEVENT *gmevent; //�X�N���v�g�����s���Ă���GMEVENT*
	SCRIPT_FLDPARAM fld_param;

	u16 *ret_script_wk;			//�X�N���v�g���ʂ������郏�[�N�̃|�C���^

  //��������������������������������������������������������
  //script.c���Ŏg�p���Ȃ��A�O�����J�p�����o
  //  public R/W
  //��������������������������������������������������������
  void *msgWin;           //���j���[�n���p���b�Z�[�W�E�B���h�E

	WORDSET* wordset;				//�P��Z�b�g
	STRBUF* msg_buf;				//���b�Z�[�W�o�b�t�@�|�C���^
	STRBUF* tmp_buf;				//�e���|�����o�b�t�@�|�C���^


	u8 MsgIndex;			//���b�Z�[�W�C���f�b�N�X
	u8 anm_count;			//�A�j���[�V�������Ă��鐔
	
	u8 win_open_flag;		//��b�E�B���h�E�\���t���O(0=��\���A1=�\��)

  u8 sound_se_flag;   ///< �T�E���hSE�Đ��`�F�b�N�t���O

	BOOL win_flag;			///<�퓬���ʕێ��p���[�N
	
  void *mw;				//�r�b�g�}�b�v���j���[���[�N

	void * subproc_work; //�T�u�v���Z�X�Ƃ̂��Ƃ�Ɏg�p���郏�[�N�ւ�*
	void * pWork;					//���[�N�ւ̔ėp�|�C���^

	MMDL *dummy_obj;

	//�g���[�i�[�������
	EV_TRAINER_EYE_HITDATA eye_hitdata[TRAINER_EYE_HITMAX];
	
	u16 scrTempWork[TEMP_WORK_SIZE];		//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)


  MUSICAL_EVENT_WORK *musicalEventWork;

  
  /*
   * ���g�p�E�������͂���Ȃ����[�N�i�O�삩�玝���Ă���
#ifndef SCRIPT_PL_NULL
	u8 common_scr_flag;		//���[�J���A���� �؂�ւ��t���O(0=local�A1=����)
	int player_dir;					//�C�x���g�N�����̎�l���̌���
	//�C�x���g�E�B���h�E
	EV_WIN_WORK* ev_win;			//�C�x���g�E�B���h�E���[�N�ւ̃|�C���^
	//��b�E�B���h�E
	GF_BGL_BMPWIN MsgWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
	
	TCB_PTR player_tcb;				//���@�`�ԃ��|�[�gTCB
	
	GF_BGL_BMPWIN CoinWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
	GF_BGL_BMPWIN GoldWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
	
	REPORT_INFO * riw;				///<���|�[�g���p�E�B���h�E���䃏�[�N
	EOA_PTR eoa;		//��Ƀt�B�[���h�G�t�F�N�g�̃|�C���^�Ƃ��Ďg��
	void * waiticon;				///<�ҋ@�A�C�R���̃|�C���^
#endif
   */
};

//--------------------------------------------------------------
//	�X�N���v�g�C�x���g�p���[�N
//--------------------------------------------------------------
typedef struct
{
	SCRIPT_WORK *sc; //�X�N���v�g�p���[�N
}EVENT_SCRIPT_WORK;

//======================================================================
//	�v���g�^�C�v�錾
//======================================================================
static SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID, HEAPID temp_heapID,
    GAMESYS_WORK * gsys, u16 scr_id, MMDL *obj, void* ret_wk);
static void SCRIPTWORK_Delete( SCRIPT_WORK * sc );
static void SCRIPTSYS_AddVM(SCRIPTSYS * scr_sys, VMHANDLE * vm, VMHANDLE_ID vm_id);
static void SCRIPTSYS_RemoveVM( SCRIPTSYS *scr_sys, VMHANDLE_ID vm_id );

static VMHANDLE * SCRVM_Create( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id );
static void SCRVM_Delete( VMHANDLE* core );
static u16 loadScriptData( SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, HEAPID heapID );
static void loadScriptDataDirect(
	SCRCMD_WORK *work, VMHANDLE* core, u32 scr_id, u32 msg_id, HEAPID heapID );


//�C�x���g
static GMEVENT * FldScript_CreateControlEvent( SCRIPT_WORK *sc );

//�C�x���g���[�N�A�X�N���v�g���[�N�A�N�Z�X�֐�
static void * SCRIPT_GetSubMemberWork( SCRIPT_WORK * sc, u32 id );
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no );

static void initFldParam(SCRIPT_FLDPARAM * fparam, GAMESYS_WORK * gsys);


//����X�N���v�g
static BOOL searchMapInitScript( GAMESYS_WORK * gsys, HEAPID heapID, u8 key);
static u16 searchSpecialScript( const u8 * p, u8 key );
static u16 searchSceneScript( GAMEDATA * gamedata, const u8 * p, u8 key );

//========================================================================
//
//	�X�N���v�g����
//
//	���X�N���v�g�C�x���g�Z�b�g
//		EventSet_Script(...)
//
//	�������̗���
//		SCRIPTWORK_Create(...)
//		SCRIPT_SetEventScript(...)						�C�x���g�ݒ�
//		GMEVENT_ControlScript(...)				�C�x���g����֐�
//				��
//		VMMachineAdd(...)						VM�}�V�[���ǉ�
//
//	�����z�}�V��(VMHANDLE)
//		�E���z�}�V���ǉ�(VMMachineAdd)
//		�E���z�}�V���̐�(vm_machine_count)��ǉ�
//
//	���X�N���v�g���䃁�C��(GMEVENT_ControlScript)
//		�E���z�}�V���̐�(vm_machine_count)���`�F�b�N���ăC�x���g�I��
//
//========================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�Z�b�g
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^(�Ȃ�����NULL)
 * @retval	none
 */
//--------------------------------------------------------------
GMEVENT * SCRIPT_SetEventScript( GAMESYS_WORK *gsys, u16 scr_id, MMDL *obj,
		HEAPID temp_heapID )
{
	GMEVENT *event;
	SCRIPT_WORK *sc;

  sc = SCRIPTWORK_Create( HEAPID_PROC, temp_heapID, gsys, scr_id, obj, NULL );
	event = FldScript_CreateControlEvent( sc );
	return( event );
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
	GMEVENT *sc_event;
	SCRIPT_WORK *sc;

  sc = SCRIPTWORK_Create( HEAPID_PROC, temp_heapID,
      GMEVENT_GetGameSysWork(event), scr_id, obj, ret_script_wk);
	sc_event = FldScript_CreateControlEvent( sc );
	GMEVENT_CallEvent( event, sc_event );

  return sc;
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
	GMEVENT *sc_event;
  SCRIPT_WORK *sc;

  sc = SCRIPTWORK_Create( HEAPID_PROC, temp_heapID,
      GMEVENT_GetGameSysWork(event), scr_id, obj, NULL);
  sc_event = FldScript_CreateControlEvent( sc );
  GMEVENT_ChangeEvent( event, sc_event );
  return sc;
}

//========================================================================
//
//
//  �X�N���v�g�E�X�N���v�g�R�}���h�������΂�鐧��֐�
//
//
//========================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
void SCRIPT_CallEvent( SCRIPT_WORK *sc, GMEVENT * call_event )
{
  GMEVENT_CallEvent( sc->gmevent, call_event );
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
 */
//--------------------------------------------------------------
void SCRIPT_EntryNextEvent( SCRIPT_WORK *sc, GMEVENT * next_event )
{
  SCRIPTSYS * scr_sys = sc->scr_sys;

  GF_ASSERT(scr_sys->next_func == NULL);
  GF_ASSERT(scr_sys->next_func_params == NULL);
  scr_sys->next_func = changeEventFunc;
  scr_sys->next_func_params = (void*)next_event;
}

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
void SCRIPT_AddVMachine( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id, VMHANDLE_ID vm_id )
{
  VMHANDLE * vm = SCRVM_Create( sc, zone_id, scr_id );
  SCRIPTSYS_AddVM( sc->scr_sys, vm, vm_id );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
BOOL SCRIPT_GetVMExists( SCRIPT_WORK *sc, VMHANDLE_ID vm_id )
{
  GF_ASSERT(vm_id < VMHANDLE_MAX);
  return (sc->scr_sys->vm[vm_id] != NULL);
}


//======================================================================
//
//  �X�N���v�g���[�N
//
//======================================================================
//--------------------------------------------------------------
/**
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^
 * @param	ret_wk		�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @return	SCRIPT_WORK			SCRIPT�^�̃|�C���^
 */
//--------------------------------------------------------------
static SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID, HEAPID temp_heapID,
    GAMESYS_WORK * gsys, u16 scr_id, MMDL *obj, void* ret_wk)
{
  SCRIPT_WORK * sc;

	sc = GFL_HEAP_AllocClearMemory( main_heapID, sizeof(SCRIPT_WORK) );
	sc->magic_no = SCRIPT_MAGIC_NO;
	sc->main_heapID = main_heapID;
  sc->temp_heapID = temp_heapID;

	sc->gsys = gsys;
	sc->start_scr_id  = scr_id;	//���C���̃X�N���v�gID
	sc->target_obj = obj;		//�b�������Ώ�OBJ�̃|�C���^�Z�b�g
	sc->ret_script_wk = ret_wk;	//�X�N���v�g���ʂ������郏�[�N
  
  { //wb
    PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork( gsys );
  	sc->start_zone_id = PLAYERWORK_getZoneID( pw );
  }
  
	if( obj != NULL ){
    u16 *objid;
    objid = getTempWork( sc, SCWK_TARGET_OBJID );
		*objid = MMDL_GetOBJID( obj ); //�b�������Ώ�OBJID�̃Z�b�g
	}
  sc->scr_sys = GFL_HEAP_AllocClearMemory( main_heapID, sizeof(SCRIPTSYS) );

  //���b�Z�[�W�֘A
  sc->wordset = WORDSET_CreateEx(
    WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, main_heapID );
  sc->msg_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );
  sc->tmp_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );

  //�t�B�[���h�p�����[�^����
	initFldParam( &sc->fld_param, gsys );
#ifndef SCRIPT_PL_NULL
	//�B���A�C�e���̃X�N���v�gID��������(���Ƃňړ�����\��)
	if( (scr_id >= ID_HIDE_ITEM_OFFSET) &&
		(scr_id <= ID_HIDE_ITEM_OFFSET_END) ){
		HideItemParamSet( sc, scr_id );
	}
#endif
  //�~���[�W�J���̐���C�x���g�œn�����
  sc->musicalEventWork = NULL;

  //�`�F�b�N�r�b�g������
  SCREND_CHK_ClearBits();

  return sc;
}

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
static void SCRIPTWORK_Delete( SCRIPT_WORK * sc )
{
  sc->magic_no = 0;
  WORDSET_Delete( sc->wordset );
  GFL_STR_DeleteBuffer( sc->msg_buf );
  GFL_STR_DeleteBuffer( sc->tmp_buf );

  GFL_HEAP_FreeMemory( sc->scr_sys );

  GFL_HEAP_FreeMemory( sc );
}

//======================================================================
//
//
//  �X�N���v�g���z�}�V���֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	���z�}�V���ǉ�
 * @param sc      SCRIPT_WORK�ւ̃|�C���^
 * @param vm      �ǉ�����VMHANDLE�ւ̃|�C���^
 * @param vm_id   VMHANDLE_ID (�X�N���v�g�pVM�w��ID�j
 */
//--------------------------------------------------------------
static void SCRIPTSYS_AddVM(SCRIPTSYS * scr_sys, VMHANDLE * vm, VMHANDLE_ID vm_id)
{
  GF_ASSERT(vm_id < VMHANDLE_MAX);
  GF_ASSERT(scr_sys->vm[vm_id] == NULL);
  scr_sys->vm[vm_id] = vm;
  scr_sys->vm_machine_count ++;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void SCRIPTSYS_RemoveVM( SCRIPTSYS *scr_sys, VMHANDLE_ID vm_id )
{
  GF_ASSERT(vm_id < VMHANDLE_MAX);
  GF_ASSERT(scr_sys->vm[vm_id] != NULL);
  if( scr_sys->vm_machine_count == 0 ){
    GF_ASSERT_MSG(0, "���z�}�V���̐����s���ł��I" );
    return;
  }
  scr_sys->vm[vm_id] = NULL;
  scr_sys->vm_machine_count--;
}

//--------------------------------------------------------------
/**
 * @brief	���z�}�V������
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	id			�X�N���v�gID
 * @param	start		���߃e�[�u���J�n�A�h���X
 * @param	end			���߃e�[�u���I���A�h���X
 * @retval	none
 */
//--------------------------------------------------------------
static VMHANDLE * SCRVM_Create( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id )
{
	SCRCMD_WORK_HEADER head;
	VMHANDLE *core;
	VM_INITIALIZER init;
	SCRCMD_WORK *work;
	
  head.zone_id = zone_id;
  head.script_id = scr_id;
	head.gsys = sc->gsys;
	head.gdata = GAMESYSTEM_GetGameData( sc->gsys );
	head.mmdlsys = GAMEDATA_GetMMdlSys( head.gdata );
	head.script = sc;
	head.fldMsgBG = sc->fld_param.msgBG;

	work = SCRCMD_WORK_Create( &head, sc->main_heapID, sc->temp_heapID );
	
	init.stack_size = 0x0100;
	init.reg_size = 0x040;
	init.command_table = ScriptCmdTbl;
	init.command_max  = ScriptCmdMax;
	
	core = VM_Create( sc->main_heapID, &init );	//���z�}�V��������
	
	VM_Init( core, work );
  {
    u16 local_scr_id;
    //�X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^�ǂݍ���
    local_scr_id = loadScriptData( work, core, zone_id, scr_id, sc->main_heapID );
    VM_Start( core, core->pScript ); //���z�}�V���ɃR�[�h�ݒ�
    //�X�N���v�g�̐擪�����̓e�[�u���ɂȂ��Ă���̂ŁA
    //�I�t�Z�b�g�Ŏ��ۂ̊J�n�ʒu��ݒ肷��
    core->adrs += (local_scr_id * sizeof(u32));			//ID���i�߂�(adrs��long�Ȃ̂�*4)
    core->adrs += VMGetU32( core );		//���x���I�t�Z�b�g���i�߂�
  }
#ifdef DEBUG_SCRIPT
{
  char buf[ZONEDATA_NAME_LENGTH];
  ZONEDATA_DEBUG_GetZoneName(buf, zone_id);
  OS_Printf( "ZONE:%s SCRID:%d\n", buf, scr_id);
}
#endif
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
	u16 msg_arc_id;
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
static u16 loadScriptData( SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 scr_id, HEAPID heapID )
{
  int i;
  u16 local_scr_id = scr_id;
  const SCRIPT_ARC_TABLE * tbl = ScriptArcTable;

  for (i = 0; i < NELEMS(ScriptArcTable); i++) {
    if (scr_id >= tbl[i].scr_id_start) {
      GF_ASSERT(scr_id <= tbl[i].scr_id_end);
      loadScriptDataDirect(work, core, tbl[i].scr_arc_id, tbl[i].msg_arc_id, heapID);
      local_scr_id -= tbl[i].scr_id_start;
      OS_Printf( "���ʃX�N���v�g�N�� scr_arc_idx = %d, msg_idx = %d, scr_id = %d\n",
        tbl[i].scr_arc_id, tbl[i].msg_arc_id, local_scr_id );
      return local_scr_id;
    }
  }
  //���[�J���X�N���v�g��scr_ID���n���ꂽ��
  if( scr_id >= ID_START_SCR_OFFSET ){
    u16 idx_script = ZONEDATA_GetScriptArcID( zone_id );
    u16 idx_msg = ZONEDATA_GetMessageArcID( zone_id );
    loadScriptDataDirect( work, core, idx_script, idx_msg, heapID );
    local_scr_id -= ID_START_SCR_OFFSET;
    OS_Printf( "�]�[���X�N���v�g�N�� scr_idx = %d, msg_idx = %d\n",
        idx_script, idx_msg );
    return local_scr_id;

  //SCRID_NULL(0)���n���ꂽ��
  }
  loadScriptDataDirect( work, core,
      NARC_script_seq_common_scr_bin,
      NARC_script_message_common_scr_dat, heapID );
  local_scr_id = 0;
  return local_scr_id;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initFldParam(SCRIPT_FLDPARAM * fparam, GAMESYS_WORK * gsys)
{
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  MI_CpuClear32( fparam, sizeof(SCRIPT_FLDPARAM) );
  fparam->fieldMap = fieldmap;
  if (fieldmap != NULL)
  {
    fparam->msgBG = FIELDMAP_GetFldMsgBG(fieldmap);
  }
}

//----------------------------------------------------------------
/**
 * �X�N���v�g�f�[�^�Z�b�g
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param	scr_id		�X�N���v�g�f�[�^��ID
 * @param	msg_id		���b�Z�[�W�f�[�^��ID
 */
//----------------------------------------------------------------
static void loadScriptDataDirect( SCRCMD_WORK *work,
	VMHANDLE *core, u32 scr_id, u32 msg_id, HEAPID heapID )
{
	//���ʃX�N���v�g�f�[�^���[�h
	VM_CODE *script = GFL_ARC_UTIL_Load( ARCID_SCRSEQ, scr_id, 0, heapID );
	core->pScript = (VM_CODE*)script;
	
	if( msg_id != SCRIPT_MSG_NON ){
		SCRCMD_WORK_CreateMsgData( work, msg_id );
	}
}


//======================================================================
//	�X�N���v�g���䃏�[�N�̃����o�[�A�N�Z�X
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  return sc->gmevent;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  return sc->gsys;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
SCRIPT_FLDPARAM * SCRIPT_GetFieldParam( SCRIPT_WORK * sc )
{
  FIELDMAP_WORK * fieldmap;
  GF_ASSERT( sc->magic_no == SCRIPT_MAGIC_NO );
  //�X�N���v�g�R�}���h���s���Ƀt�B�[���h�ւ̏o���肪����ƁA
  //�������ς���ă|�C���^���������l���w���Ă��Ȃ��\��������B
  //�Ȃ̂ŁA�A�N�Z�X�^�C�~���O�ł̏������������s���B
  initFldParam( &sc->fld_param, sc->gsys );
  GF_ASSERT( sc->fld_param.fieldMap != NULL );
  return &sc->fld_param;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃��b�Z�[�W�E�B���h�E�|�C���^�擾
 * @param	sc		    SCRIPT�^�̃|�C���^
 * @param	msgWin		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
void * SCRIPT_GetMsgWinPointer( SCRIPT_WORK *sc )
{
  return sc->msgWin;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃��b�Z�[�W�E�B���h�E�|�C���^�Z�b�g
 * @param	sc		    SCRIPT�^�̃|�C���^
 * @param	msgWin		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
void SCRIPT_SetMsgWinPointer( SCRIPT_WORK *sc, void* msgWin )
{
  sc->msgWin = msgWin;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	sc		SCRIPT�^�̃|�C���^
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
static void * SCRIPT_GetSubMemberWork( SCRIPT_WORK *sc, u32 id )
{
	switch( id ){
	//��b�E�B���h�E���b�Z�[�W�C���f�b�N�X�̃|�C���^
	case ID_EVSCR_MSGINDEX:
		return &sc->MsgIndex;
	//�A�j���[�V�����̐��̃|�C���^
	case ID_EVSCR_ANMCOUNT:
		return &sc->anm_count;
	//��b�E�B���h�E���J�������t���O�̃|�C���^
	case ID_EVSCR_WIN_OPEN_FLAG:
		return &sc->win_open_flag;
	//���C���̃X�N���v�gID
	case ID_EVSCR_SCRIPT_ID:
		return &sc->start_scr_id;
	//�b�������Ώۂ�OBJ�̃|�C���^
	case ID_EVSCR_TARGET_OBJ:
		return &sc->target_obj;
	//�����_�~�[OBJ�̃|�C���^
	case ID_EVSCR_DUMMY_OBJ:
		return &sc->dummy_obj;
	//���ʂ������郏�[�N�̃|�C���^
	case ID_EVSCR_RETURN_SCRIPT_WK:
		return &sc->ret_script_wk;
	//���z�}�V��(���C��)�̃|�C���^
	//�T�u�v���Z�X�Ƃ̂��Ƃ胏�[�N�ւ̃|�C���^
	case ID_EVSCR_SUBPROC_WORK:
		return &sc->subproc_work;
	//���[�N�ւ̔ėp�|�C���^
	case ID_EVSCR_PWORK:
		return &sc->pWork;
	//�퓬���ʃt���O
	case ID_EVSCR_WIN_FLAG:
		return &sc->win_flag;


	//HEAPID wb
	case ID_EVSCR_WK_HEAPID:
		return &sc->main_heapID;
  //TEMP_HEAPID wb
  case ID_EVSCR_WK_TEMP_HEAPID:
    return &sc->temp_heapID;
	//�r�b�g�}�b�v���j���[���[�N�̃|�C���^
	case ID_EVSCR_MENUWORK:
		return &sc->mw;
	//�P��Z�b�g
	case ID_EVSCR_WORDSET:
		return &sc->wordset;
	//���b�Z�[�W�o�b�t�@�̃|�C���^
	case ID_EVSCR_MSGBUF:
		return &sc->msg_buf;
	//�e���|�����o�b�t�@�̃|�C���^
	case ID_EVSCR_TMPBUF:
		return &sc->tmp_buf;


  case ID_EVSCR_TRAINER0:
		return &sc->eye_hitdata[0];
  case ID_EVSCR_TRAINER1:
		return &sc->eye_hitdata[1];
  
  case ID_EVSCR_MUSICAL_EVENT_WORK:
  	GF_ASSERT_MSG( sc->musicalEventWork != NULL , "�~���[�W�J�����[�N��NULL�I" );
    return sc->musicalEventWork;

  case ID_EVSCR_SOUND_SE_FLAG:
    return &sc->sound_se_flag;
      

	};
	
	//�G���[
	GF_ASSERT( (0) && "�����o�[ID���s���ł��I" );
	return NULL;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
void * SCRIPT_GetMemberWork( SCRIPT_WORK *sc, u32 id )
{
	if( sc->magic_no != SCRIPT_MAGIC_NO ){
		GF_ASSERT_MSG(0, "�N��(�m��)���Ă��Ȃ��X�N���v�g�̃��[�N�ɃA�N�Z�X���Ă��܂��I" );
	}
	return SCRIPT_GetSubMemberWork( sc, id );
}


//======================================================================
//	�C�x���g
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static void FldScriptEvent_InitWork( EVENT_SCRIPT_WORK *ev_wk, SCRIPT_WORK * sc )
{
	MI_CpuClear8( ev_wk, sizeof(EVENT_SCRIPT_WORK) );
	ev_wk->sc = sc;
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
static GMEVENT_RESULT FldScriptEvent_ControlScript(
	GMEVENT *event, int *seq, void *wk )
{
	int i;
	EVENT_SCRIPT_WORK *ev_wk = wk;
	SCRIPT_WORK *sc = ev_wk->sc;
	
	switch( *seq ){
	case 0:
    //���[�N�Z�b�g
    sc->gmevent = event; //�C�x���g�Z�b�g���ɏ��������Ă邪�O�̂���
		
    //���z�}�V���ǉ�
    SCRIPT_AddVMachine( sc, sc->start_zone_id, sc->start_scr_id, VMHANDLE_MAIN );
		
		(*seq)++;
		/* FALL THROUGH */

	case 1:
		//���z�}�V���R���g���[��
		for( i = 0; i < VMHANDLE_MAX; i++ ){
	    VMHANDLE *vm = sc->scr_sys->vm[i];
			if( vm != NULL ){
				if( VM_Control(vm) == FALSE ){	//���䃁�C��
          SCRIPTSYS_RemoveVM( sc->scr_sys, i );
					SCRVM_Delete( vm );				//�X�N���v�g�폜
				}
			}
		}

		//���z�}�V���̐����`�F�b�N
		if( sc->scr_sys->vm_machine_count <= 0 ){
			SCRIPT_EVENTFUNC func = sc->scr_sys->next_func;		//�ޔ�
      void * func_params = sc->scr_sys->next_func_params;//�ޔ�
			
			//�f�o�b�N����
			//debug_script_flag = 0;
			
      SCRIPTWORK_Delete( sc );		//�X�N���v�g���[�N
			
			//�X�N���v�g�I�����ɌĂяo�����֐����s
			if( func != NULL ){
				func( event, func_params );
			}else{ //FieldEvent���I�����Ȃ��悤��FALSE��Ԃ��I
				return( GMEVENT_RES_FINISH );		//FieldEvent_Set�I���I
			}
		}

		break;
	}

	return( GMEVENT_RES_CONTINUE );
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�Z�b�g
 * @param	sc	SCRIPT_WORK*
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * FldScript_CreateControlEvent( SCRIPT_WORK *sc )
{
	GMEVENT *event;
	EVENT_SCRIPT_WORK *ev_sc;
	
	event = GMEVENT_Create( sc->gsys, NULL,
		FldScriptEvent_ControlScript, sizeof(EVENT_SCRIPT_WORK) );
	ev_sc = GMEVENT_GetEventWork( event );
  FldScriptEvent_InitWork( ev_sc, sc );
	
  ev_sc->sc->gmevent = event;
	return event;
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g����X�N���v�g���[�N���擾
 * @param	GMEVENT *event
 * @retval  SCRIPT_WORK*
 */
//--------------------------------------------------------------
SCRIPT_WORK* SCRIPT_GetEventWorkToScriptWork( GMEVENT *event )
{
	EVENT_SCRIPT_WORK *ev_sc = GMEVENT_GetEventWork( event );
	return ev_sc->sc;
}

//======================================================================
//	�C�x���g���[�N
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no )
{
  GF_ASSERT( work_no >= TEMP_WORK_START );
  GF_ASSERT( work_no < TEMP_WORK_END );
  return &(sc->scrTempWork[ work_no - TEMP_WORK_START ]);
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�A�h���X���擾
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	work_no		���[�N�i���o�[
 *
 * @return	"���[�N�̃A�h���X"
 *
 * @li	work_no < 0x8000	�ʏ�̃Z�[�u���[�N
 * @li	work_no >= 0x8000	�X�N���v�g���䃏�[�N�̒��Ɋm�ۂ��Ă��郏�[�N
 */
//------------------------------------------------------------------
u16 * SCRIPT_GetEventWork( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no )
{
	EVENTWORK *ev;
	ev = GAMEDATA_GetEventWork( gdata );
	
	if( work_no < SVWK_START ){
		return NULL;
	}
	
	if( work_no < SCWK_START ){
		return EVENTWORK_GetEventWorkAdrs(ev,work_no);
	}
	
	//�X�N���v�g���䃏�[�N�̒��ŁAANSWORK�Ȃǂ̃��[�N���m�ۂ��Ă��܂�
  return getTempWork( sc, work_no );
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̒l���擾
 *
 * @param	work_no		���[�N�i���o�[
 *
 * @return	"���[�N�̒l"
 */
//------------------------------------------------------------------
u16 SCRIPT_GetEventWorkValue( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no )
{
	u16 *res = SCRIPT_GetEventWork( sc, gdata, work_no );
	if( res == NULL ){ return work_no; }
	return *res;
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̒l���Z�b�g
 *
 * @param	work_no		���[�N�i���o�[
 * @param	value		�Z�b�g����l
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
BOOL SCRIPT_SetEventWorkValue(
	SCRIPT_WORK *sc, u16 work_no, u16 value )
{
  GAMEDATA * gdata;
	u16* res;
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  gdata = GAMESYSTEM_GetGameData( sc->gsys );
	res = SCRIPT_GetEventWork( sc, gdata, work_no );
	if( res == NULL ){ return FALSE; }
	*res = value;
	return TRUE;
}
//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�ݒ�(�~���[�W�J��
 * @param	sc	  SCRIPT_WORK�^�̃|�C���^
 */
//--------------------------------------------------------------
void SCRIPT_SetMemberWork_Musical( SCRIPT_WORK *sc, void *musEveWork )
{
	if( sc->magic_no != SCRIPT_MAGIC_NO ){
		GF_ASSERT_MSG(0, "�N��(�m��)���Ă��Ȃ��X�N���v�g�̃��[�N�ɃA�N�Z�X���Ă��܂��I" );
	}
	sc->musicalEventWork = musEveWork;
}


//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���擾
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	no			0-15
 * @return	"OBJ�L�����R�[�h"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
u16 GetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no )
{
	GF_ASSERT( (no < OBJCHR_WORK_MAX) && "�������s���ł��I" );
	return GetEventWorkValue( fsys, (OBJCHR_WORK_START+no) );
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�X�N���v�g����w�肷��OBJ�R�[�h���Z�b�g
 *
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	no			0-15
 * @param	obj_code	OBJ�R�[�h
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
BOOL SetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no, u16 obj_code )
{
	GF_ASSERT( (no < OBJCHR_WORK_MAX) && "�������s���ł��I" );
	return SetEventWorkValue( fsys, (OBJCHR_WORK_START+no), obj_code );
}
#endif

//--------------------------------------------------------------
/**
 * �v���O��������X�N���v�g�ւ̈����ƂȂ�p�����[�^���Z�b�g
 * @param	sc SCRIPT_WORK
 * @param	prm0	�p�����[�^�O�iSCWK_PARAM0�j
 * @param	prm1	�p�����[�^�P�iSCWK_PARAM1�j
 * @param	prm2	�p�����[�^�Q�iSCWK_PARAM2�j
 * @param	prm3	�p�����[�^�R�iSCWK_PARAM3�j
 * @retval none
 */
//--------------------------------------------------------------
void SCRIPT_SetScriptWorkParam( SCRIPT_WORK *sc, u16 prm0, u16 prm1, u16 prm2, u16 prm3 )
{
  GF_ASSERT(sc->magic_no == SCRIPT_MAGIC_NO);
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM0, prm0 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM1, prm1 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM2, prm2 );
  SCRIPT_SetEventWorkValue( sc, SCWK_PARAM3, prm3 );
}

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̃|�C���^�[�A�h���X���擾
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * �g���I�������K��NULL�N���A���邱�ƁI
 */
//--------------------------------------------------------------
void** SCRIPT_SetSubProcWorkPointerAdrs( SCRIPT_WORK *sc )
{
  return &sc->subproc_work;
}

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̃|�C���^�[���擾
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * �g���I�������K��NULL�N���A���邱�ƁI
 */
//--------------------------------------------------------------
void * SCRIPT_SetSubProcWorkPointer( SCRIPT_WORK *sc )
{
  return sc->subproc_work;
}

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X�p���[�N�̈�̉��(�|�C���^��NULL�łȂ����Free)
 * @param	sc SCRIPT_WORK
 * @retval none
 */
//--------------------------------------------------------------
void SCRIPT_FreeSubProcWorkPointer( SCRIPT_WORK *sc )
{
  if(sc->subproc_work != NULL){
    GFL_HEAP_FreeMemory(sc->subproc_work);
    sc->subproc_work = NULL;
  }
}

//======================================================================
//	�t���O�֘A
//======================================================================

//------------------------------------------------------------------
/**
 * @brief	�}�b�v������̃Z�[�u�t���O���N���A����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @return	none
 */
//------------------------------------------------------------------
void LocalEventFlagClear( EVENTWORK * event )
{
  enum {
    LOCAL_WORK_MAX = LOCALWORKAREA_END - LOCALWORKAREA_START,

    //LOCAL_FLAG_AREA_MAX = FH_AREA_END - FH_AREA_START,
  };
	int i;
	//�C�x���g���[�N�̃|�C���^�擾
  //

	//���[�J���t���O�̃N���A"8bit * 8 = 64��"
	//"0"�͖����ȃi���o�[�Ȃ̂�1��n���Ď擾���Ă���
	//MI_CpuClear8( EVENTWORK_GetEventFlagAdrs(event,1), LOCAL_FLAG_AREA_MAX );
  EVENTWORK_ClearEventFlags( event, FH_AREA_START, FH_AREA_END );
	//���[�J�����[�N�̃N���A"32��"
  EVENTWORK_ClearEventWorks( event, LOCALWORKAREA_START, LOCALWORKAREA_END );
	//MI_CpuClear8( EVENTWORK_GetEventWorkAdrs(event,LOCALWORKAREA_START), 2*LOCAL_WORK_MAX );

}

//------------------------------------------------------------------
/**
 * @brief	1���o�߂��ƂɃN���A�����t���O���N���A����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @return	none
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void TimeEventFlagClear( EVENTWORK * eventwork )
{
	EVENTWORK* event;

	//�C�x���g���[�N�̃|�C���^�擾
	event = SaveData_GetEventWork(fsys->savedata);

	MI_CpuClear8( EventWork_GetEventFlagAdrs(event,TIMEFLAG_START), TIMEFLAG_AREA_MAX );

	return;
}
#endif

//======================================================================
//	�g���[�i�[�t���O�֘A
//	�E�X�N���v�gID����A�g���[�i�[ID���擾���āA�t���O�`�F�b�N
//======================================================================
#define GET_TRAINER_FLAG(id)	( TR_FLAG_START+(id) )

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�g���[�i�[ID���擾
 *
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "�g���[�i�[ID = �t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id )
{
#if 0 //pl
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET + 1);		//1�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET + 1);		//1�I���W��
	}
#else
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET);		//0�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET);		//0�I���W��
	}
#endif
}

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A���E�ǂ���̃g���[�i�[���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "0=���A1=�E"
 */
//--------------------------------------------------------------
BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return 0;
	}else{
		return 1;
	}
}

//--------------------------------------------------------------
/**
 * �g���[�i�[ID����A�_�u���o�g���^�C�v���擾
 * @param   tr_id		�g���[�i�[ID
 * @retval  "0=�V���O���o�g���A1=�_�u���o�g��"
 */
//--------------------------------------------------------------
BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id )
{
  #ifndef SCRIPT_PL_NULL
	if( TT_TrainerDataParaGet(tr_id,ID_TD_fight_type) == FIGHT_TYPE_1vs1 ){
		OS_Printf( "trainer_type = 1vs1 " );
		//return SCR_EYE_TR_TYPE_SINGLE;
		return 0;
	}

	OS_Printf( "trainer_type = 2vs2 " );
	//return SCR_EYE_TR_TYPE_DOUBLE;
	return 1;
  #else //wb
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  OS_Printf( "check 2v2 TRID=%d, type = %d\n", tr_id, rule );

  switch( rule ){
  case BTL_RULE_SINGLE:
    return( 0 );
  case BTL_RULE_DOUBLE:
  case BTL_RULE_TRIPLE: //kari
    return( 1 );
  }
  
//  GF_ASSERT( 0 );
  return( 1 );
  #endif
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���`�F�b�N����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  return EVENTWORK_CheckEventFlag(ev,GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O���Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  EVENTWORK_SetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	�g���[�i�[�C�x���g�t���O�����Z�b�g����
 *
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	tr_id		�g���[�i�[ID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
	EVENTWORK_ResetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[���������i�[ ���O��SCRIPT_SetEventScript()���N�����Ă�����
 * @param	event SCRIPT_SetEventScript()�߂�l�B
 * @param	mmdl �������q�b�g����FIELD_OBJ_PTR
 * @param	range		�O���b�h�P�ʂ̎�������
 * @param	dir			�ړ�����
 * @param	scr_id		�����q�b�g�����X�N���v�gID
 * @param	tr_id		�����q�b�g�����g���[�i�[ID
 * @param	tr_type		�g���[�i�[�^�C�v�@�V���O���A�_�u���A�^�b�O����
 * @param	tr_no		���ԖڂɃq�b�g�����g���[�i�[�Ȃ̂�
 */
//--------------------------------------------------------------
void SCRIPT_SetTrainerEyeData( GMEVENT *event, MMDL *mmdl,
    s16 range, u16 dir, u16 scr_id, u16 tr_id, int tr_type, int tr_no )
{
  EVENT_SCRIPT_WORK *ev_sc = GMEVENT_GetEventWork( event );
  SCRIPT_WORK *sc = ev_sc->sc;
	EV_TRAINER_EYE_HITDATA *eye = &sc->eye_hitdata[tr_no];
  
  TRAINER_EYE_HITDATA_Set(eye, mmdl, range, dir, scr_id, tr_id, tr_type, tr_no );
}

//======================================================================
//	����X�N���v�g�֘A
//======================================================================
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
  SCRIPT_CallSpecialScript( gsys, heapID, SCRID_INIT_SCRIPT );
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
  SCRIPT_CallSpecialScript( gsys, heapID, SCRID_INIT_DEBUG_SCRIPT );
}
#endif  //PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief	EVENT�ł͂Ȃ�����X�N���v�g���s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_CallSpecialScript( GAMESYS_WORK *gsys, HEAPID heapID, u16 script_id )
{
  VMHANDLE *core = NULL;
  SCRIPT_WORK *sc = SCRIPTWORK_Create( HEAPID_PROC, heapID, gsys, script_id, NULL, NULL);
  
  core = SCRVM_Create( sc, sc->start_zone_id, script_id );
  while( VM_Control(core) == TRUE ){};
  SCRVM_Delete( core );

  SCRIPTWORK_Delete( sc );
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
	if( scr_id == SCRIPT_NOT_EXIST_ID ){
		return FALSE;
	} else {
    //����X�N���v�g���s
    SCRIPT_CallSpecialScript( gsys, heapID, scr_id );
  }
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
  LocalEventFlagClear(ev);
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
	if( scr_id == SCRIPT_NOT_EXIST_ID ){
		return NULL;
  }
  {
    event = SCRIPT_SetEventScript( gsys, scr_id, NULL, heapID );
  }
  return event;
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
    if (*p == SP_SCRID_NONE)
    {
      return SCRIPT_NOT_EXIST_ID;
    }
		//�w��^�C�v�̓���X�N���v�g����
    if (*p == key)
    {
      p+= 2; //����X�N���v�gID������
      return ( *p + ( *(p+1)<<8) );		//�X�N���v�gID��Ԃ�
    }
    p += (2 + 2 + 2);						//����X�N���v�gID+�X�N���v�gID+�_�~�[������
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

	while( TRUE ){
		//�I���L�q�`�F�b�N(SP_EVENT_DATA_END)
		if( *p == SP_SCRID_NONE ){
			return SCRIPT_NOT_EXIST_ID;
		}

		//�w��^�C�v�̓���X�N���v�g����
		if( (*p) == key ){
			p += 2;			//����X�N���v�gID������
			pos = ( *p + ( *(p+1)<<8 ) + ( *(p+2)<<16 ) + ( *(p+3)<<24 ) );
			p += 4;			//�A�h���X������
			break;
		}

		p += (2 + 4);		//����X�N���v�gID+�A�h���X������
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
		if( *p == 0 ){
			return SCRIPT_NOT_EXIST_ID;
		}

		//��r���郏�[�N�擾
		work1 = ( *p + ( *(p+1)<<8 ) );
		if( work1 == 0 ){
			return SCRIPT_NOT_EXIST_ID;
		};
		p += 2;									//���[�N������
		//OS_Printf( "work1 = %d\n", work1 );

		//��r����l(���[�N��)�擾
		work2 = ( *p + ( *(p+1)<<8 ) );
		p += 2;									//��r����l������(���[�N��)
		//OS_Printf( "work2 = %d\n", work2 );

		//�����`�F�b�N
		//if( EVENTWORK_GetEventWorkAdrs(ev,work1) == EVENTWORK_GetEventWorkAdrs(ev,work2) ){
		if( *(EVENTWORK_GetEventWorkAdrs(ev,work1)) == work2 ){
			return ( *p + ( *(p+1)<<8 ) );
		}

		p += 2;									//�X�N���v�gID������
	}
	return SCRIPT_NOT_EXIST_ID;								//����������Ȃ�����
}
