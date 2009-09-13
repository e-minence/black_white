//======================================================================
/**
 * @file	script.c
 * @bfief	�X�N���v�g���䃁�C������
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

#include "field/zonedata.h"

#include "script_message.naix"
#include "print/wordset.h"

#include "arc/fieldmap/zone_id.h"

#include "script.h"

#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "eventwork_def.h"
#include "trainer_eye_data.h"   //EV_TRAINER_EYE_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG

#include "arc/fieldmap/script_seq.naix"
#include "system/main.h"  //HEAPID_PROC

#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"
#include "../../../resource/fldmapdata/script/init_scr_def.h" //SCRID_INIT_SCRIPT

//======================================================================
//  define
//======================================================================
#define TRAINER_EYE_HITMAX (2) ///<�g���[�i�[�����f�[�^�ő吔

enum
{
	WORDSET_SCRIPT_SETNUM = 8,		//�f�t�H���g�o�b�t�@��
	WORDSET_SCRIPT_BUFLEN = 64,		//�f�t�H���g�o�b�t�@���i�������j
};

//#define SCR_MSG_BUF_SIZE	(1024)				//���b�Z�[�W�o�b�t�@�T�C�Y
#define SCR_MSG_BUF_SIZE	(512)				//���b�Z�[�W�o�b�t�@�T�C�Y

#define SCRIPT_MAGIC_NO		(0x3643f)

#define SCRIPT_MSG_NON (0xffffffff)

#define SCRIPT_NOT_EXIST_ID (0xffff)

enum {
  SCWK_AREA_SIZE = SCWK_AREA_END - SCWK_AREA_START,
};

//======================================================================
//	struct
//======================================================================
//�֐��|�C���^�^
typedef void (*SCRIPT_EVENTFUNC)(GMEVENT *fsys);

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�\����
 */
//--------------------------------------------------------------
typedef struct {

	u8 vm_machine_count;	//�ǉ��������z�}�V���̐�
	VMHANDLE *vm[VMHANDLE_MAX];	//���z�}�V���ւ̃|�C���^
	
	SCRIPT_EVENTFUNC next_func;		//�X�N���v�g�I�����ɌĂяo�����֐�

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

	HEAPID heapID;
  HEAPID temp_heapID;

	GAMESYS_WORK *gsys;
  GMEVENT *gmevent; //�X�N���v�g�����s���Ă���GMEVENT*
	SCRIPT_FLDPARAM fld_param;

	u16 *ret_script_wk;			//�X�N���v�g���ʂ������郏�[�N�̃|�C���^

  //��������������������������������������������������������
  //script.c���Ŏg�p���Ȃ��A�O�����J�p�����o
  //  public R/W
  //��������������������������������������������������������

	WORDSET* wordset;				//�P��Z�b�g
	STRBUF* msg_buf;				//���b�Z�[�W�o�b�t�@�|�C���^
	STRBUF* tmp_buf;				//�e���|�����o�b�t�@�|�C���^


	u8 MsgIndex;			//���b�Z�[�W�C���f�b�N�X
	u8 anm_count;			//�A�j���[�V�������Ă��鐔
	
	u8 win_open_flag;		//��b�E�B���h�E�\���t���O(0=��\���A1=�\��)

	BOOL win_flag;			///<�퓬���ʕێ��p���[�N
	
  void *mw;				//�r�b�g�}�b�v���j���[���[�N

	void * subproc_work; //�T�u�v���Z�X�Ƃ̂��Ƃ�Ɏg�p���郏�[�N�ւ�*
	void * pWork;					//���[�N�ւ̔ėp�|�C���^

	MMDL *dummy_obj;

	//�g���[�i�[�������
	EV_TRAINER_EYE_HITDATA eye_hitdata[TRAINER_EYE_HITMAX];
	
	u16 scrTempWork[SCWK_AREA_SIZE];		//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)
	

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
//	�B���A�C�e���f�[�^
//======================================================================
#ifndef SCRIPT_PL_NULL
typedef struct{
	u16 itemno;									//�A�C�e���i���o�[
	u8	num;									//��
	u8	response;								//�����x
	u16	sp;										//����(���g�p)
	u16	index;									//�t���O�C���f�b�N
}HIDE_ITEM_DATA;

//#include "../fielddata/script/hide_item.dat"	//�B���A�C�e���f�[�^
#endif

//======================================================================
//	�v���g�^�C�v�錾
//======================================================================
static SCRIPT_WORK * SCRIPTWORK_Create( HEAPID heapID, HEAPID temp_heapID,
    GAMESYS_WORK * gsys, u16 scr_id, MMDL *obj, void* ret_wk);
static void SCRIPTWORK_Delete( SCRIPT_WORK * sc );
static void SCRIPTSYS_AddVM(SCRIPTSYS * scr_sys, VMHANDLE * vm, VMHANDLE_ID vm_id);
static void SCRIPTSYS_RemoveVM( SCRIPTSYS *scr_sys, VMHANDLE_ID vm_id );

static VMHANDLE * SCRVM_Create( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id );
static void SCRVM_Delete( VMHANDLE* core );
static u16 loadScriptData( SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, HEAPID heapID );
static void loadScriptDataDirect(
	SCRCMD_WORK *work, VMHANDLE* core, u32 scr_id, u32 msg_id, HEAPID heapID );

static void EventDataIDJump( VMHANDLE * core, u16 ev_id );



//�C�x���g
static GMEVENT * FldScript_CreateControlEvent( SCRIPT_WORK *sc );
static void FldScript_CallControlEvent( SCRIPT_WORK *sc, GMEVENT *event );
static GMEVENT * FldScript_ChangeControlEvent( SCRIPT_WORK *sc, GMEVENT *event );

//�C�x���g���[�N�A�X�N���v�g���[�N�A�N�Z�X�֐�
static void * SCRIPT_GetSubMemberWork( SCRIPT_WORK * sc, u32 id );
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no );

#ifndef SCRIPT_PL_NULL
u16 * GetEventWorkAdrs( FLDCOMMON_WORK* fsys, u16 work_no );
u16 GetEventWorkValue( FLDCOMMON_WORK* fsys, u16 work_no );
BOOL SetEventWorkValue( FLDCOMMON_WORK* fsys, u16 work_no, u16 value );

u16 GetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no );
BOOL SetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no, u16 obj_code );
#endif

//�C�x���g�t���O

//�g���[�i�[�֘A
#ifndef SCRIPT_PL_NULL
u16 GetTrainerIdByScriptId( u16 scr_id );
BOOL GetTrainerLRByScriptId( u16 scr_id );
BOOL CheckTrainer2vs2Type( u16 tr_id );
#endif

//�B���A�C�e��
#ifndef SCRIPT_PL_NULL
u16 GetHideItemFlagNoByScriptId( u16 scr_id );
u16 GetHideItemFlagIndexByScriptId( u16 scr_id );
u8 GetHideItemResponseByScriptId( u16 scr_id );
static BOOL HideItemParamSet( SCRIPT_WORK* sc, u16 scr_id );
void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys );
#endif

//����X�N���v�g
static BOOL SCRIPT_SearchMapInitScript( GAMESYS_WORK * gsys, HEAPID heapID, u8 key);
static u16 searchSpecialScript( const u8 * p, u8 key );
static u16 searchSceneScript( GAMESYS_WORK * gsys, const u8 * p, u8 key );

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
		HEAPID temp_heapID, const SCRIPT_FLDPARAM *fparam )
{
	GMEVENT *event;
	SCRIPT_WORK *sc;

  sc = SCRIPTWORK_Create( HEAPID_PROC, temp_heapID, gsys, scr_id, obj, NULL );
	sc->fld_param = *fparam;
	event = FldScript_CreateControlEvent( sc );
	return( event );
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
void SCRIPT_CallScript( GMEVENT *event,
	u16 scr_id, MMDL *obj, void *ret_script_wk, HEAPID heapID )
{
  SCRIPT_WORK *sc = SCRIPTWORK_Create( HEAPID_PROC, heapID,
      GMEVENT_GetGameSysWork(event), scr_id, obj, ret_script_wk);
	FldScript_CallControlEvent( sc, event );
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
void SCRIPT_ChangeScript( GMEVENT *event,
		u16 scr_id, MMDL *obj, HEAPID heapID )
{
  SCRIPT_WORK *sc = SCRIPTWORK_Create( HEAPID_PROC, heapID,
      GMEVENT_GetGameSysWork(event), scr_id, obj, NULL);
	FldScript_ChangeControlEvent( sc, event );
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

//--------------------------------------------------------------
/**
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^
 * @param	ret_wk		�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @return	SCRIPT_WORK			SCRIPT�^�̃|�C���^
 */
//--------------------------------------------------------------
static SCRIPT_WORK * SCRIPTWORK_Create( HEAPID heapID, HEAPID temp_heapID,
    GAMESYS_WORK * gsys, u16 scr_id, MMDL *obj, void* ret_wk)
{
  SCRIPT_WORK * sc;

	sc = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCRIPT_WORK) );
	sc->magic_no = SCRIPT_MAGIC_NO;
	sc->heapID = heapID;
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
    //objid = SCRIPT_GetSubMemberWork( sc, ID_EVSCR_WK_TARGET_OBJID );
		*objid = MMDL_GetOBJID( obj ); //�b�������Ώ�OBJID�̃Z�b�g
	}
  sc->scr_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCRIPTSYS) );

  //���b�Z�[�W�֘A
  sc->wordset = WORDSET_CreateEx(
    WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, sc->heapID );
  sc->msg_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, sc->heapID );
  sc->tmp_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, sc->heapID );

#ifndef SCRIPT_PL_NULL
	//�B���A�C�e���̃X�N���v�gID��������(���Ƃňړ�����\��)
	if( (scr_id >= ID_HIDE_ITEM_OFFSET) &&
		(scr_id <= ID_HIDE_ITEM_OFFSET_END) ){
		HideItemParamSet( sc, scr_id );
	}
#endif

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
	
	work = SCRCMD_WORK_Create( &head, sc->heapID, sc->temp_heapID );
	
	init.stack_size = 0x0100;
	init.reg_size = 0x040;
	init.command_table = ScriptCmdTbl;
	init.command_max  = ScriptCmdMax;
	
	core = VM_Create( sc->heapID, &init );	//���z�}�V��������
	
	VM_Init( core, work );
  {
    u16 local_scr_id;
    //�X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^�ǂݍ���
    local_scr_id = loadScriptData( work, core, zone_id, scr_id, sc->heapID );
    VM_Start( core, core->pScript ); //���z�}�V���ɃR�[�h�ݒ�
    EventDataIDJump( core, local_scr_id );
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


	//GAMESYS_WORK wb
	case ID_EVSCR_WK_GAMESYS_WORK:
		return &sc->gsys;
	//HEAPID wb
	case ID_EVSCR_WK_HEAPID:
		return &sc->heapID;
  //TEMP_HEAPID wb
  case ID_EVSCR_WK_TEMP_HEAPID:
    return &sc->temp_heapID;
	//SCRIPT_FLDPARAM wb
	case ID_EVSCR_WK_FLDPARAM:
		return &sc->fld_param;
  //GMEVENT   
  case ID_EVSCR_WK_GMEVENT:
    return &sc->gmevent;
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

/*
#ifndef SCRIPT_PL_NULL
	//���ʃX�N���v�g�؂�ւ��t���O�̃|�C���^
	case ID_EVSCR_COMMON_SCR_FLAG:
		return &sc->common_scr_flag;
	//�ǉ��������z�}�V���̐��̃|�C���^
	case ID_EVSCR_VMHANDLE_COUNT:
		return &sc->vm_machine_count;
	case ID_EVSCR_VM_MAIN:
		return &sc->vm[VMHANDLE_MAIN];
	//���z�}�V��(�T�u)�̃|�C���^
	case ID_EVSCR_VM_SUB1:
		return &sc->vm[VMHANDLE_SUB1];
	//�C�x���g�N�����̎�l���̌���
	case ID_EVSCR_PLAYER_DIR:
		return &sc->player_dir;
	//�C�x���g�E�B���h�E���[�N�̃|�C���^
	case ID_EVSCR_EVWIN:
		return &sc->ev_win;
	//��b�E�B���h�E�r�b�g�}�b�v�f�[�^�̃|�C���^
	case ID_EVSCR_MSGWINDAT:
		return &sc->MsgWinDat;
	//�ҋ@�A�C�R���̃|�C���^
	case ID_EVSCR_WAITICON:
		return &sc->waiticon;
	//�t�B�[���h�G�t�F�N�g�ւ̃|�C���^
	case ID_EVSCR_EOA:
		return &sc->eoa;
	//���@�`�ԃ��|�[�gTCB�̃|�C���^
	case ID_EVSCR_PLAYER_TCB:
		return &sc->player_tcb;
	//�R�C���E�B���h�E�r�b�g�}�b�v�f�[�^�̃|�C���^
	case ID_EVSCR_COINWINDAT:
		return &sc->CoinWinDat;
	//�����E�B���h�E�r�b�g�}�b�v�f�[�^�̃|�C���^
	case ID_EVSCR_GOLDWINDAT:
		return &sc->GoldWinDat;
	//���|�[�g���\���E�B���h�E���䃏�[�N�ւ̃|�C���^
	case ID_EVSCR_REPORTWIN:
		return &sc->riw;

#endif
*/
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

//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N��"next_func"�Ƀ��j���[�Ăяo�����Z�b�g
 * @param	fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @return	none
 * �t�B�[���h���j���[�Ăяo������I
 * ���ꂪ�c�������炭�Ȃ�̂ŁA�ėp�I�Ɏg���Ȃ��悤�ɂ��Ă���I
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void SCRIPT_SetNextScript( FLDCOMMON_WORK *fsys )
{
	SCRIPT_WORK *sc = FieldEvent_GetSpecialWork( fsys->event );
	
	if( FieldMenuCallCheck( fsys ) == TRUE ){	//�u�Ȃ��̂΂���v�`�F�b�N 2006/10/24 by nakahiro
		sc->scr_sys->next_func = FieldMenuEvChg;
	}
	return;
}
#endif

//--------------------------------------------------------------
/**
 * ID�W�����v
 * @param	core	VMHANDLE�^�̃|�C���^
 * @param	id		�X�N���v�gID
 * @return	none
 */
//--------------------------------------------------------------
static void EventDataIDJump( VMHANDLE * core, u16 ev_id )
{
	core->adrs += (ev_id * 4);			//ID���i�߂�(adrs��long�Ȃ̂�*4)
	core->adrs += VMGetU32( core );		//���x���I�t�Z�b�g���i�߂�
	return;
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
			
			//�f�o�b�N����
			//debug_script_flag = 0;
			
      SCRIPTWORK_Delete( sc );		//�X�N���v�g���[�N
			
			//�X�N���v�g�I�����ɌĂяo�����֐����s
			if( func != NULL ){
				func( event );
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
 * �X�N���v�g�C�x���g�R�[��
 * @param	sc	SCRIPT_WORK*
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static void FldScript_CallControlEvent( SCRIPT_WORK *sc, GMEVENT *event )
{
	GMEVENT *sc_event;
	sc_event = FldScript_CreateControlEvent( sc );
	GMEVENT_CallEvent( event, sc_event );
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�C�x���g�`�F���W
 * @param	sc	SCRIPT_WORK*
 * @param	
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * FldScript_ChangeControlEvent( SCRIPT_WORK *sc, GMEVENT *event )
{
	EVENT_SCRIPT_WORK *ev_sc;
	
	GMEVENT_Change( event,
		FldScriptEvent_ControlScript, sizeof(EVENT_SCRIPT_WORK) );
	ev_sc = GMEVENT_GetEventWork( event );
  FldScriptEvent_InitWork( ev_sc, sc );

	return event;
}

//======================================================================
//	�C�x���g���[�N
//======================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no )
{
  GF_ASSERT( work_no >= SCWK_AREA_START );
  GF_ASSERT( work_no < SCWK_AREA_END );
  return &(sc->scrTempWork[ work_no - SCWK_AREA_START ]);
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
  GAMEDATA * gdata = GAMESYSTEM_GetGameData( sc->gsys );
	u16* res = SCRIPT_GetEventWork( sc, gdata, work_no );
	if( res == NULL ){ return FALSE; }
	*res = value;
	return TRUE;
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
#ifndef SCRIPT_PL_NULL
void LocalEventFlagClear( EVENTWORK * eventwork )
{
	int i;
	EVENTWORK* event;

	//�C�x���g���[�N�̃|�C���^�擾
	event = SaveData_GetEventWork(fsys->savedata);

	//���[�J���t���O�̃N���A"8bit * 8 = 64��"
	//"0"�͖����ȃi���o�[�Ȃ̂�1��n���Ď擾���Ă���
	MI_CpuClear8( EventWork_GetEventFlagAdrs(event,1), LOCAL_FLAG_AREA_MAX );

	//���[�J�����[�N�̃N���A"32��"
	MI_CpuClear8( EventWork_GetEventWorkAdrs(event,LOCAL_WORK_START), 2*LOCAL_WORK_MAX );

}
#endif

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
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET + 1);		//1�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET + 1);		//1�I���W��
	}
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
#ifndef SCRIPT_PL_NULL
BOOL CheckTrainer2vs2Type( u16 tr_id )
{
	if( TT_TrainerDataParaGet(tr_id, ID_TD_fight_type) == FIGHT_TYPE_1vs1 ){	//1vs1
		OS_Printf( "trainer_type = 1vs1 " );
		return 0;
		//return SCR_EYE_TR_TYPE_SINGLE;
	}

	OS_Printf( "trainer_type = 2vs2 " );
	return 1;
	//return SCR_EYE_TR_TYPE_DOUBLE;
}
#else
BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id )
{
  #if 0 //wb ���󖳌�
	if( TT_TrainerDataParaGet(tr_id, ID_TD_fight_type) == FIGHT_TYPE_1vs1 ){	//1vs1
		OS_Printf( "trainer_type = 1vs1 " );
		return 0;
		//return SCR_EYE_TR_TYPE_SINGLE;
	}

	OS_Printf( "trainer_type = 2vs2 " );
	return 1;
	//return SCR_EYE_TR_TYPE_DOUBLE;
  #else
  return 0; //���ŃV���O���Œ�
  #endif
}
#endif

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

//======================================================================
//	�B���A�C�e���֘A
//	�E�X�N���v�gID����A�B���A�C�e���t���O���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlag( fsys, GetHideItemFlagNoByScriptId(scr_id) );
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�i���o�[���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "�t���O�i���o�["
 */
//--------------------------------------------------------------
u16 SCRIPT_GetHideItemFlagNoByScriptID( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET + FH_FLAG_START);
}

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���t���O�C���f�b�N�X���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "�t���O�C���f�b�N�X"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
u16 GetHideItemFlagIndexByScriptId( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET);
}
#endif

//--------------------------------------------------------------
/**
 * 0���ŉB���t���O����������
 * @param   fsys	FLDCOMMON_WORK�^�̃|�C���^
 * @retval  none
 * �a�������̃`�F�b�N�������Ă��܂������A
 * ����̌`��OK�������ł��B(08.06.25)
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static u16 oneday_hide_item1[][2] = {		//�|�S��
	{ ZONE_ID_D24R0103, 52 },
	{ ZONE_ID_D24R0104, 53 },
	{ ZONE_ID_D24R0105, 54 },
	{ ZONE_ID_D24R0105, 55 },
};

#define ONEDAY_HIDE_ITEM1_MAX	( NELEMS(oneday_hide_item1) )

static u16 oneday_hide_item2[] = {			//�\�m�I�̉ԉ�
	58,59,219,220,221,222,
};

#define ONEDAY_HIDE_ITEM2_MAX	( NELEMS(oneday_hide_item2) )
#endif

#ifndef SCRIPT_PL_NULL
void HideItemFlagOneDayClear( FLDCOMMON_WORK* fsys )
{
	u8 index;

	//�|�S����2�ӏ��̉B���A�C�e�����������邱�Ƃ�����
	index = ( gf_rand() % ONEDAY_HIDE_ITEM1_MAX );
	OS_Printf( "0_0 index = %d\n", index );
	if( fsys->location->zone_id != oneday_hide_item1[index][0] ){
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item1[index][1]) );
	}

	index = ( gf_rand() % ONEDAY_HIDE_ITEM1_MAX );
	OS_Printf( "0_1 index = %d\n", index );
	if( fsys->location->zone_id != oneday_hide_item1[index][0] ){
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item1[index][1]) );
	}

	//�\�m�I�̉ԉ���2�ӏ��̉B���A�C�e�����������邱�Ƃ�����
	if( fsys->location->zone_id != ZONE_ID_D13R0101 ){
		index = ( gf_rand() % ONEDAY_HIDE_ITEM2_MAX );
		OS_Printf( "1_0 index = %d\n", index );
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item2[index]) );

		index = ( gf_rand() % ONEDAY_HIDE_ITEM2_MAX );
		OS_Printf( "1_1 index = %d\n", index );
		ResetEventFlag( fsys, (FH_FLAG_START + oneday_hide_item2[index]) );
	}
	return;
}
#endif

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A�B���A�C�e���̔������擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "����"
 * �B���A�C�e���̃X�N���v�g����������ĂԂ悤�ɂ���I
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
u8 GetHideItemResponseByScriptId( u16 scr_id )
{
	int i;
	u16	index;
	const HIDE_ITEM_DATA* data;

	data	= &hide_item_data[0];
	index	= GetHideItemFlagIndexByScriptId(scr_id);			//�t���O�C���f�b�N�X�擾

	//�T�[�`����
	for( i=0; i < HIDE_ITEM_DATA_MAX ;i++ ){
		if( data[i].index == index ){
			break;
		}
	}

	//������Ȃ�����
	if( i >= HIDE_ITEM_DATA_MAX ){
		GF_ASSERT( (0) && "�B���A�C�e���f�[�^�ɊY������f�[�^������܂���I" );
		return 0;
	}

	return data[i].response;
}
#endif

//--------------------------------------------------------------
/**
 * �B���A�C�e���p�����[�^�����[�N�ɃZ�b�g
 *
 * @param   sc			SCRIPT�^�̃|�C���^
 * @param   scr_id		�X�N���v�gID
 *
 * @retval  "0=�Z�b�g���s�A1=�Z�b�g����"
 *
 * �g�p���Ă���I
 * SCWK_PARAM0
 * SCWK_PARAM1
 * SCWK_PARAM2
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static BOOL HideItemParamSet( SCRIPT_WORK* sc, u16 scr_id )
{
	int i;
	u16	index;
	const HIDE_ITEM_DATA* data;
	u16* param0 = getTempWork( sc, SCWK_PARAM0 );
	u16* param1 = getTempWork( sc, SCWK_PARAM1 );
	u16* param2 = getTempWork( sc, SCWK_PARAM2 );

	data	= &hide_item_data[0];
	index	= GetHideItemFlagIndexByScriptId(scr_id);		//�t���O�C���f�b�N�X�擾

	//�T�[�`����
	for( i=0; i < HIDE_ITEM_DATA_MAX ;i++ ){
		if( data[i].index == index ){
			break;
		}
	}

	//������Ȃ�����
	if( i >= HIDE_ITEM_DATA_MAX ){
		GF_ASSERT( (0) && "�B���A�C�e���f�[�^�ɊY������f�[�^������܂���I" );
		return 0;
	}

	*param0 = data[i].itemno;						//�A�C�e���i���o�[
	*param1 = data[i].num;							//��
	*param2 = GetHideItemFlagNoByScriptId(scr_id);	//�t���O�i���o�[

	return 1;
}
#endif

#define DEBUG_HIDE_ITEM_LIST	//�f�o�b�N�L��
//--------------------------------------------------------------
/**
 * ��ʓ��ɂ���B���A�C�e�����������Ċm�ۂ������X�g�ɓo�^
 *
 * @param   fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param   heapid		�q�[�vID
 *
 * @retval  "���X�g�̃A�h���X"
 *
 * ���������Y�ꂸ�ɁI
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
HIDE_ITEM_LIST * HideItem_CreateList( FLDCOMMON_WORK * fsys, int heapid )
{
	HIDE_ITEM_LIST* list;
	const BG_TALK_DATA* bg;
	int	hero_gx,hero_gz,i,max,count,tmp;
	int l,r,u,d;

	count = 0;

	//BG�f�[�^���擾
	max = EventData_GetNowBgTalkDataSize( fsys );
	max++;		//�I���R�[�h������̂�+1

	//�������m��
	list = sys_AllocMemory( heapid, sizeof(HIDE_ITEM_LIST) * max );

	//BG�f�[�^�����݂��Ă��Ȃ�������
	if( max == 1 ){
		//�I���R�[�h�Z�b�g
		list[0].response= HIDE_LIST_RESPONSE_NONE;
		list[0].gx		= 0xffff;
		list[0].gz		= 0xffff;
		return list;
	}

	//BG�f�[�^�擾
	bg	= EventData_GetNowBgTalkData( fsys );
	if( bg == NULL ){
		//�I���R�[�h�Z�b�g
		list[0].response= HIDE_LIST_RESPONSE_NONE;
		list[0].gx		= 0xffff;
		list[0].gz		= 0xffff;
		return list;
	}

	//��l���̈ʒu�擾
	hero_gx = Player_NowGPosXGet( fsys->player );
	hero_gz = Player_NowGPosZGet( fsys->player );

	//�����͈͂��Z�b�g(3D�͈ӎ������̊ȈՔ�)
	l = hero_gx - HIDE_LIST_SX;
	r = hero_gx + HIDE_LIST_SX;
	u = hero_gz - HIDE_LIST_TOP;
	d = hero_gz + HIDE_LIST_BOTTOM;

	//�␳
	if( l < 0 ){ l = 0; }
	//if( r < 0 ){ 0 };
	if( u < 0 ){ u = 0; }
	//if( d < 0 ){ 0 };

#ifdef DEBUG_HIDE_ITEM_LIST
	OS_Printf( "\n�������͈́�\n" );
	OS_Printf( "l = %d\n", l );
	OS_Printf( "r = %d\n", r );
	OS_Printf( "u = %d\n", u );
	OS_Printf( "d = %d\n", d );
#endif

	//BG�f�[�^���T�[�`��������
	for( i=0; i < max ;i++ ){

		//�B���A�C�e���^�C�v�ŁA�܂����肵�Ă��Ȃ�������
		if( (bg[i].type == BG_TALK_TYPE_HIDE) &&
			(CheckEventFlag(fsys, GetHideItemFlagNoByScriptId(bg[i].id)) == 0) ){

			//�����͈͓��ɂ��邩�`�F�b�N
			if( (bg[i].gx >= l) &&
				(bg[i].gx <= r) &&
				(bg[i].gz >= u) &&
				(bg[i].gz <= d) ){

				//�X�N���v�gID����A�B���A�C�e���̔������擾
				list[count].response= GetHideItemResponseByScriptId( bg[i].id );

#if 0
				//���������W�����̂܂ܑ��
				list[count].gx		= bg[i].gx;
				list[count].gz		= bg[i].gz;

				//���[�J�����W
				list[count].gx		= (bg[i].gx % 32);
				list[count].gz		= (bg[i].gz % 32);
#endif	

				//�����0,0�Ƃ��Ď擾�������W
				//(��l���̈ʒu����̍��������߂�)
				tmp = ( hero_gx - bg[i].gx );
				list[count].gx = abs(HIDE_LIST_SX - tmp);
				tmp = ( hero_gz - bg[i].gz );
				list[count].gz = abs(HIDE_LIST_TOP - tmp);

#ifdef DEBUG_HIDE_ITEM_LIST
				OS_Printf( "\n����l���̈ʒu��\n" );
				OS_Printf( "hero_gx = %d\n", hero_gx );
				OS_Printf( "hero_gz = %d\n", hero_gz );

				OS_Printf( "\n���������B���A�C�e���̈ʒu��\n" );
				OS_Printf( "bg[i].gx = %d\n", bg[i].gx );
				OS_Printf( "bg[i].gz = %d\n", bg[i].gz );

				OS_Printf( "\n�������0,0�Ƃ��Ď擾�������W��\n" );
				OS_Printf( "list[count].gx = %d\n", list[count].gx );
				OS_Printf( "list[count].gz = %d\n", list[count].gz );
#endif

				count++;
			}
		}
	}

	//�I���R�[�h�Z�b�g
	list[count].response= HIDE_LIST_RESPONSE_NONE;
	list[count].gx		= 0xffff;
	list[count].gz		= 0xffff;

	return list;
}
#endif


//======================================================================
//	����X�N���v�g�֘A
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n �X�N���v�g�����ݒ�̎��s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_CallGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID )
{
  SCRIPT_CallSpecialScript( gsys, heapID, SCRID_INIT_SCRIPT );
}

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
}

//------------------------------------------------------------------
/**
 * @brief	����X�N���v�g�������Ď��s
 * @param	gsys      GAMESYS_WORK�^�̃|�C���^
 * @param	key			����X�N���v�gID
 * @return	"TRUE=����X�N���v�g���s�AFALSE=�������Ȃ�"
 */
//------------------------------------------------------------------
static BOOL SCRIPT_SearchMapInitScript( GAMESYS_WORK * gsys, HEAPID heapID, u8 key)
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
  return SCRIPT_SearchMapInitScript( gsys, heapID, SP_SCRID_FIELD_INIT );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL SCRIPT_CallFieldRecoverScript( GAMESYS_WORK * gsys, HEAPID heapID )
{
  return SCRIPT_SearchMapInitScript( gsys, heapID, SP_SCRID_FIELD_RECOVER );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
BOOL SCRIPT_CallZoneChangeScript( GAMESYS_WORK * gsys, HEAPID heapID)
{
  return SCRIPT_SearchMapInitScript( gsys, heapID, SP_SCRID_ZONE_CHANGE );
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

  scr_id = searchSceneScript( gsys, p, SP_SCRID_SCENE_CHANGE );
	if( scr_id == SCRIPT_NOT_EXIST_ID ){
		return NULL;
  }
  {
    SCRIPT_FLDPARAM fparam;
    FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
    fparam.fieldMap = fieldmap;
    fparam.msgBG = FIELDMAP_GetFldMsgBG(fieldmap);
    event = SCRIPT_SetEventScript( gsys, scr_id, NULL, heapID, &fparam );
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
static u16 searchSceneScript( GAMESYS_WORK * gsys, const u8 * p, u8 key )
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

	ev = GAMEDATA_GetEventWork( GAMESYSTEM_GetGameData(gsys) );
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
		OS_Printf( "work1 = %d\n", work1 );

		//��r����l(���[�N��)�擾
		work2 = ( *p + ( *(p+1)<<8 ) );
		p += 2;									//��r����l������(���[�N��)
		OS_Printf( "work2 = %d\n", work2 );

		//�����`�F�b�N
		//if( EVENTWORK_GetEventWorkAdrs(ev,work1) == EVENTWORK_GetEventWorkAdrs(ev,work2) ){
		if( *(EVENTWORK_GetEventWorkAdrs(ev,work1)) == work2 ){
			return ( *p + ( *(p+1)<<8 ) );
		}

		p += 2;									//�X�N���v�gID������
	}
	return SCRIPT_NOT_EXIST_ID;								//����������Ȃ�����
}


