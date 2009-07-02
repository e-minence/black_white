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
#include "script_offset_id.h" //�X�N���v�gID�I�t�Z�b�g
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "eventwork_def.h"

#include "arc/fieldmap/script_seq.naix"

//======================================================================
//	�f�o�b�N
//======================================================================
//VM_CODE* sp_script;

//======================================================================
//  define
//======================================================================
#define TRAINER_EYE_HITMAX (2) ///<�g���[�i�[�����f�[�^�ő吔

enum
{
	WORDSET_SCRIPT_SETNUM = 8,		//�f�t�H���g�o�b�t�@��
	WORDSET_SCRIPT_BUFLEN = 64,		//�f�t�H���g�o�b�t�@���i�������j
};

#define SCR_MSG_BUF_SIZE	(1024)				//���b�Z�[�W�o�b�t�@�T�C�Y

//======================================================================
//	struct
//======================================================================
//�֐��|�C���^�^
typedef void (*SCRIPT_EVENTFUNC)(GMEVENT *fsys);

//--------------------------------------------------------------
//  �g���[�i�[�����f�[�^�\����
//--------------------------------------------------------------
typedef struct {
	s16 range;				//��������
	u16 dir;					//�ړ�����
	u16 scr_id;				//�X�N���v�gID
	u16 tr_id;				//�g���[�i�[ID
	int tr_type;			//�g���[�i�[�^�C�v
	MMDL *mmdl;
  GMEVENT *ev_eye_move;
}EV_TRAINER_EYE_HITDATA;

//--------------------------------------------------------------
//	�X�N���v�g���䃏�[�N�\����
//--------------------------------------------------------------
struct _TAG_SCRIPT_WORK
{
	u32 magic_no;			//�C�x���g�̃��[�N���X�N���v�g���䃏�[�N���𔻕�
	
	u8 seq;					//�X�N���v�g�V�[�P���X
	u8 MsgIndex;			//���b�Z�[�W�C���f�b�N�X
	u8 anm_count;			//�A�j���[�V�������Ă��鐔
	u8 common_scr_flag;		//���[�J���A���� �؂�ւ��t���O(0=local�A1=����)
	
	u8 win_open_flag;		//��b�E�B���h�E�\���t���O(0=��\���A1=�\��)
	u8 vm_machine_count;	//�ǉ��������z�}�V���̐�
	u16 script_id;			//���C���̃X�N���v�gID
	
	BOOL win_flag;			///<�퓬���ʕێ��p���[�N
	
#ifndef SCRIPT_PL_NULL
	//�C�x���g�E�B���h�E
	EV_WIN_WORK* ev_win;			//�C�x���g�E�B���h�E���[�N�ւ̃|�C���^
	//��b�E�B���h�E
	GF_BGL_BMPWIN MsgWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
#endif
	
#ifndef SCRIPT_PL_NULL
	BMPMENU_WORK* mw;				//�r�b�g�}�b�v���j���[���[�N
#else
  void *mw;
#endif

#ifndef SCRIPT_PL_NULL
	int player_dir;					//�C�x���g�N�����̎�l���̌���
	FIELD_OBJ_PTR target_obj;		//�b�������Ώۂ�OBJ�̃|�C���^
	FIELD_OBJ_PTR dummy_obj;		//�����_�~�[OBJ�̃|�C���^
#else
	int player_dir;					//�C�x���g�N�����̎�l���̌���
	MMDL *target_obj;
	MMDL *dummy_obj;
#endif
	u16 *ret_script_wk;			//�X�N���v�g���ʂ������郏�[�N�̃|�C���^
	VMHANDLE *vm[VMHANDLE_MAX];	//���z�}�V���ւ̃|�C���^

	WORDSET* wordset;				//�P��Z�b�g
	STRBUF* msg_buf;				//���b�Z�[�W�o�b�t�@�|�C���^
	STRBUF* tmp_buf;				//�e���|�����o�b�t�@�|�C���^
  
#ifndef SCRIPT_PL_NULL
	void * waiticon;				///<�ҋ@�A�C�R���̃|�C���^
#endif
	
	//�g���[�i�[�������
	EV_TRAINER_EYE_HITDATA eye_hitdata[TRAINER_EYE_HITMAX];
	
	u16 work[EVSCR_WORK_MAX];		//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)
	
	SCRIPT_EVENTFUNC next_func;		//�X�N���v�g�I�����ɌĂяo�����֐�
	
	void * subproc_work; //�T�u�v���Z�X�Ƃ̂��Ƃ�Ɏg�p���郏�[�N�ւ�*
	void * pWork;					//���[�N�ւ̔ėp�|�C���^

#ifndef SCRIPT_PL_NULL
	EOA_PTR eoa;		//��Ƀt�B�[���h�G�t�F�N�g�̃|�C���^�Ƃ��Ďg��
#endif

#ifndef SCRIPT_PL_NULL
	TCB_PTR player_tcb;				//���@�`�ԃ��|�[�gTCB
	
	GF_BGL_BMPWIN CoinWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
	GF_BGL_BMPWIN GoldWinDat;		//�r�b�g�}�b�v�E�B���h�E�f�[�^
	
	REPORT_INFO * riw;				///<���|�[�g���p�E�B���h�E���䃏�[�N
#endif
	
	//WB
	GAMESYS_WORK *gsys;
	HEAPID heapID;
	FLDMSGBG *msgBG;
  u32 zone_id;

	SCRIPT_FLDPARAM fld_param;
};

//--------------------------------------------------------------
//	�X�N���v�g�C�x���g�p���[�N
//--------------------------------------------------------------
typedef struct
{
	SCRIPT_WORK *sc; //�X�N���v�g�p���[�N
	SCRCMD_WORK *cmd_wk; //�X�N���v�g�R�}���h�p���[�N
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
static SCRIPT_WORK* EvScriptWork_Alloc( HEAPID heapID );
static void script_del( VMHANDLE* core );
static void EvScriptWork_Init( SCRIPT_WORK *sc, GAMESYS_WORK *gsys,
	u16 scr_id, MMDL *obj, void* ret_wk);

static void InitScript(
	SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, u8 type, HEAPID heapID );
static u16 SetScriptDataSub( SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, HEAPID heapID );
static void SetScriptData(
	SCRCMD_WORK *work, VMHANDLE* core, int index, u32 dat_id, HEAPID heapID );
static void SetZoneScriptData( FLDCOMMON_WORK* fsys, VMHANDLE* core );

//SCRIPT�̃����o�[�A�N�Z�X
static void EventDataIDJump( VMHANDLE * core, u16 ev_id );

static void * LoadZoneScriptFile(int zone_id);
static u32 LoadZoneMsgNo(int zone_id);

//�C�x���g
static GMEVENT * FldScript_CreateControlEvent( SCRIPT_WORK *sc );
static void FldScript_CallControlEvent( SCRIPT_WORK *sc, GMEVENT *event );
static GMEVENT * FldScript_ChangeControlEvent( SCRIPT_WORK *sc, GMEVENT *event );

//�C�x���g���[�N�A�X�N���v�g���[�N�A�N�Z�X�֐�
#ifndef SCRIPT_PL_NULL
u16 * GetEventWorkAdrs( FLDCOMMON_WORK* fsys, u16 work_no );
u16 GetEventWorkValue( FLDCOMMON_WORK* fsys, u16 work_no );
BOOL SetEventWorkValue( FLDCOMMON_WORK* fsys, u16 work_no, u16 value );

u16 GetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no );
BOOL SetEvDefineObjCode( FLDCOMMON_WORK* fsys, u16 no, u16 obj_code );
#endif

//�C�x���g�t���O
#ifndef SCRIPT_PL_NULL
BOOL CheckEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);
void SetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);
void ResetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no);

void LocalEventFlagClear( FLDCOMMON_WORK* fsys );
void TimeEventFlagClear( FLDCOMMON_WORK* fsys );
#endif

//�g���[�i�[�֘A
#ifndef SCRIPT_PL_NULL
u16 GetTrainerIdByScriptId( u16 scr_id );
BOOL GetTrainerLRByScriptId( u16 scr_id );
BOOL CheckTrainer2vs2Type( u16 tr_id );
BOOL CheckEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );
void SetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );
void ResetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id );
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
#ifndef SCRIPT_PL_NULL
void GameStartScriptInit( FLDCOMMON_WORK* fsys );
void SpScriptStart( FLDCOMMON_WORK* fsys, u16 scr_id );
BOOL SpScriptSearch( FLDCOMMON_WORK* fsys, u8 key );
static u16 SpScriptSearch_Sub( const u8 * p, u8 key );
#endif

//======================================================================
//	�O���[�o���ϐ�
//======================================================================
#define SCRIPT_MAGIC_NO		(0x3643f)

//========================================================================
//
//	�X�N���v�g����
//
//	���X�N���v�g�C�x���g�Z�b�g
//		EventSet_Script(...)
//
//	�������̗���
//		EvScriptWork_Alloc(...)					���[�N�m��
//		EvScriptWork_Init(...)					�����ݒ�
//		FieldEvent_Set(...)						�C�x���g�ݒ�
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
		HEAPID heapID, const SCRIPT_FLDPARAM *fparam )
{
	GMEVENT *event;
	
	SCRIPT_WORK *sc = EvScriptWork_Alloc( heapID );		//���[�N�m��
	sc->fld_param = *fparam;
	EvScriptWork_Init( sc, gsys, scr_id, obj, NULL );	//�����ݒ�
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
  
  eye->range = range;
	eye->dir = dir;
	eye->scr_id = scr_id;
	eye->tr_id = tr_id;
	eye->tr_type = tr_type;
	eye->mmdl = mmdl;
  eye->ev_eye_move = NULL;
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
	SCRIPT_WORK *sc = EvScriptWork_Alloc( heapID );	//���[�N�m��
	EvScriptWork_Init( sc, GMEVENT_GetGameSysWork(event), scr_id, obj, ret_script_wk );	//�����ݒ�
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
	SCRIPT_WORK *sc = EvScriptWork_Alloc( heapID );	//���[�N�m��
	EvScriptWork_Init( sc, GMEVENT_GetGameSysWork(event), scr_id, obj, NULL );	//�����ݒ�
	FldScript_ChangeControlEvent( sc, event );
}


//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g���䃁�C��
 *
 * @param	event		GMEVENT_CONTROL�^
 *
 * @retval	"FALSE = �X�N���v�g���s��"
 * @retval	"TRUE = �X�N���v�g���s�I��"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static BOOL GMEVENT_ControlScript(GMEVENT_CONTROL * event)
{
	int i;
	fsysFunc func;
	VMHANDLE* core = NULL;
	SCRIPT_WORK * sc = FieldEvent_GetSpecialWork( event );
	FLDCOMMON_WORK* fsys = FieldEvent_GetFieldSysWork( event );

	switch(sc->seq){
	case 0:
		//���z�}�V���ǉ�
		sc->vm[VMHANDLE_MAIN] = VMMachineAdd( fsys, sc->script_id );
		//VMMachineAdd( fsys, id, &ScriptCmdTbl[0], &ScriptCmdTbl[EVCMD_MAX] );
		sc->vm_machine_count = 1;

		//MSGMAN_Create�̌�ɏ���
		//sc->wordset = WORDSET_Create( HEAPID_WORLD );
		sc->wordset = WORDSET_CreateEx(WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, HEAPID_WORLD);
		sc->msg_buf = STRBUF_Create( SCR_MSG_BUF_SIZE, HEAPID_WORLD );
		sc->tmp_buf = STRBUF_Create( SCR_MSG_BUF_SIZE, HEAPID_WORLD );
    
		sc->seq++;
		/* FALL THROUGH */
		
	case 1:
		//���z�}�V���R���g���[��
		for( i=0; i < VMHANDLE_MAX; i++ ){

			core = sc->vm[i];

			if( core != NULL ){

				if( VM_Control(core) == FALSE ){				//���䃁�C��

					script_del(core);							//�X�N���v�g�폜

					if( sc->vm_machine_count == 0 ){
						GF_ASSERT( (0) && "���z�}�V���̐����s���ł��I" );
					}

					sc->vm[i] = NULL;
					sc->vm_machine_count--;
				}
			}
		}

		//���z�}�V���̐����`�F�b�N
		if( sc->vm_machine_count <= 0 ){

			func = sc->next_func;								//�ޔ�

			WORDSET_Delete( sc->wordset );
			STRBUF_Delete( sc->msg_buf );
			STRBUF_Delete( sc->tmp_buf );

			//�f�o�b�N����
			//debug_script_flag = 0;
			sc->magic_no = 0;

			sys_FreeMemoryEz( sc );								//�X�N���v�g���[�N

			//�X�N���v�g�I�����ɌĂяo�����֐����s
			if( func != NULL ){
				func(fsys);
				return FALSE;				//FieldEvent���I�����Ȃ��悤��FALSE��Ԃ��I
			}

			return TRUE;										//FieldEvent_Set�I���I
		}
		break;
	};
	return FALSE;
}
#endif

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g���䃏�[�N�m��
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 *
 * @retval	"�X�N���v�g���䃏�[�N�̃A�h���X"
 */
//--------------------------------------------------------------
static SCRIPT_WORK * EvScriptWork_Alloc( HEAPID heapID )
{
	SCRIPT_WORK *sc;
	
	sc = GFL_HEAP_AllocClearMemory( heapID, sizeof(SCRIPT_WORK) );
	
	//�f�o�b�N����
	//debug_script_flag = 1;
	
	sc->magic_no = SCRIPT_MAGIC_NO;
	sc->heapID = heapID;
	return sc;
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
static void script_del( VMHANDLE *core )
{
	#ifndef SCRIPT_PL_NULL
	MSGMAN_Delete( core->msgman );
	sys_FreeMemoryEz( core->pScript );	//�X�N���v�g�f�[�^
	sys_FreeMemoryEz( core );
	return;
	#else
	SCRCMD_WORK *cmd = VM_GetContext( core );
	SCRCMD_WORK_Delete( cmd );
	GFL_HEAP_FreeMemory( core->pScript );
	VM_Delete( core );
	#endif
}

//-----------------ON=---------------------------------------------
/**
 * @brief	�X�N���v�g���䃏�[�N�����ݒ�
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	sc			SCRIPT�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @param	obj			�b�������Ώ�OBJ�̃|�C���^
 * @param	ret_wk		�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 *
 * @retval	"�X�N���v�g���䃏�[�N�̃A�h���X"
 */
//--------------------------------------------------------------
static void EvScriptWork_Init( SCRIPT_WORK *sc,
	GAMESYS_WORK *gsys,	u16 scr_id, MMDL *obj, void *ret_wk )
{
	u16 *objid;
	
	sc->gsys = gsys;
	objid = SCRIPT_GetSubMemberWork( sc, ID_EVSCR_WK_TARGET_OBJID );
	
	//DIR_NOT = -1
	#ifndef SCRIPT_PL_NULL
	sc->player_dir = Player_DirGet( fsys->player ); //�N�����̎��@�����Z�b�g
	#endif
	sc->target_obj = obj;		//�b�������Ώ�OBJ�̃|�C���^�Z�b�g
	sc->script_id  = scr_id;	//���C���̃X�N���v�gID
	sc->ret_script_wk = ret_wk;	//�X�N���v�g���ʂ������郏�[�N
  
  { //wb
    PLAYER_WORK *pw = GAMESYSTEM_GetMyPlayerWork( gsys );
  	sc->zone_id = PLAYERWORK_getZoneID( pw );
  }
  
	if( obj != NULL ){
		*objid = MMDL_GetOBJID( obj ); //�b�������Ώ�OBJID�̃Z�b�g
	}

#ifndef SCRIPT_PL_NULL
	//�B���A�C�e���̃X�N���v�gID��������(���Ƃňړ�����\��)
	if( (scr_id >= ID_HIDE_ITEM_OFFSET) &&
		(scr_id <= ID_HIDE_ITEM_OFFSET_END) ){
		HideItemParamSet( sc, scr_id );
	}
	
	#ifdef DEBUG_SCRIPT
	OS_Printf( "ZONE:%s SCRID:%d\n",
		ZoneData_GetZoneName(fsys->location->zone_id), scr_id);
	#endif
#endif
}

//--------------------------------------------------------------
/**
 * @brief	���z�}�V���ǉ�
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	id			�X�N���v�gID
 * @param	start		���߃e�[�u���J�n�A�h���X
 * @param	end			���߃e�[�u���I���A�h���X
 * @retval	none
 */
//--------------------------------------------------------------
VMHANDLE * SCRIPT_AddVMachine(
		GAMESYS_WORK *gsys,
		SCRIPT_WORK *sc,
		HEAPID heapID,
		u16 scr_id )
{
	SCRCMD_WORK_HEADER head;
	VMHANDLE *core;
	VM_INITIALIZER init;
	SCRCMD_WORK *work;
	
	head.gsys = gsys;
	head.gdata = GAMESYSTEM_GetGameData( gsys );
	head.fldmmdlsys = GAMEDATA_GetMMdlSys( head.gdata );
	head.script = sc;
	
	work = SCRCMD_WORK_Create( &head, heapID );
	
	init.stack_size = 0x0100;
	init.reg_size = 0x0100;
	init.command_table = ScriptCmdTbl;
	
	#if 0	//command_max��const�̈׏������o���Ȃ�
	init.command_max  = ScriptCmdMax;
	#else
	{
		u32 *max = (u32*)(&init.command_max);
		*max = ScriptCmdMax;
	}
	#endif
	
	core = VM_Create( heapID, &init );	//���z�}�V��������
	
	VM_Init( core, work );
	InitScript( work, core, sc->zone_id, scr_id, 0, heapID );//���[�J���X�N���v�g������
	return core;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g�ݒ菉����
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param	id			�X�N���v�gID
 * @param	type		"���g�p"
 * @retval	none
 */
//--------------------------------------------------------------
static void InitScript(
	SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, u8 type, HEAPID heapID )
{
	u16 scr_id;
	scr_id = SetScriptDataSub( work, core, zone_id, id, heapID ); //�X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^�ǂݍ���
	VM_Start( core, core->pScript ); //���z�}�V���ɃR�[�h�ݒ�
	EventDataIDJump( core, scr_id );
//	VM_SetWork(core,(void *)fsys->event); //�R�}���h�ȂǂŎQ�Ƃ��郏�[�N�Z�b�g
	return;
}

//--------------------------------------------------------------
/**
 * @brief	�X�N���v�gID����X�N���v�g�f�[�^�A���b�Z�[�W�f�[�^��ǂݍ���
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param	id			�X�N���v�gID
 * @retval	"�X�N���v�gID����I�t�Z�b�g���������l"
 */
//--------------------------------------------------------------
static u16 SetScriptDataSub( SCRCMD_WORK *work, VMHANDLE* core, u32 zone_id, u16 id, HEAPID heapID )
{
	u16 scr_id = id;
	
	if( scr_id >= ID_COMMON_SCR_OFFSET ){		//���ʃX�N���v�gID
		SetScriptData( work, core,
			NARC_script_seq_common_scr_bin,
			NARC_script_message_common_scr_dat,
			heapID );
		scr_id -= ID_COMMON_SCR_OFFSET;
	}else if( scr_id >= ID_START_SCR_OFFSET ){ //���[�J���X�N���v�gID
		#ifndef SCRIPT_PL_NULL
		SetZoneScriptData( fsys, core );	//�]�[���X�N���v�g�f�[�^�Z�b�g
		scr_id -= ID_START_SCR_OFFSET;
		#else //wb ��
    {
#if 0
      u32 idx_script = NARC_script_seq_dummy_scr_bin;
      u32 idx_msg = NARC_message_common_scr_dat;
      
      switch( zone_id ){
      case ZONE_ID_T01:
        idx_script = NARC_script_seq_t01_bin;
        idx_msg = NARC_message_t01_dat;
        break;
      case ZONE_ID_MAPSPRING:
      case ZONE_ID_MAPSUMMER:
      case ZONE_ID_MAPAUTUMN:
      case ZONE_ID_MAPWINTER:
        idx_script = NARC_script_seq_c99_bin;
        idx_msg = NARC_message_c99_dat;
        break;
      }
#else
      u16 idx_script = ZONEDATA_GetScriptArcID( zone_id );
      u16 idx_msg = ZONEDATA_GetMessageArcID( zone_id );
      
      OS_Printf( "�]�[���X�N���v�g�N�� scr_idx = %d, msg_idx = %d\n",
          idx_script, idx_msg );
      
#endif
	  	SetScriptData( work, core, idx_script, idx_msg, heapID );
		  scr_id -= ID_START_SCR_OFFSET;
    }
		#endif
	}else{										//SCRID_NULL(0)���n���ꂽ��
		SetScriptData( work, core,
			NARC_script_seq_dummy_scr_bin,
			NARC_script_message_common_scr_dat,
			heapID );
		scr_id = 0;
	}
	
	return scr_id;
}

#if 0 //SCRIPT_PL_NULL
static u16 SetScriptDataSub( SCRCMD_WORK *work, VMHANDLE* core, u16 id )
{
	u16 scr_id = id;
	

	//�X�N���b�`�X�N���v�g��ID���n���ꂽ��
	if( scr_id >= ID_SCRATCH_OFFSET ){
		SetScriptData(
			fsys, core,
			NARC_scr_seq_scratch_bin,
			NARC_msg_scratch_counter_dat );
		scr_id -= ID_SCRATCH_OFFSET;
	//�t�����e�B�A���уX�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_SEISEKI_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_seiseki_bin, NARC_msg_bf_seiseki_dat );
		scr_id -= ID_SEISEKI_OFFSET;

	//�h�T����X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_DOSA_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_dosa_bin, NARC_msg_circuit_trainer_dat );
		scr_id -= ID_DOSA_OFFSET;

	//�T�|�[�g�X�N���v�g��ID���n���ꂽ��
	}else  if( scr_id >= ID_SUPPORT_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_support_bin, NARC_msg_support_dat );
		scr_id -= ID_SUPPORT_OFFSET;

	//�z�B���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_HAITATU_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_haitatu_bin, NARC_msg_haitatu_dat );
		scr_id -= ID_HAITATU_OFFSET;

	//TV�C���^�r���[�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_TV_INTERVIEW_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_tv_interview_bin, NARC_msg_tv_interview_dat );
		scr_id -= ID_TV_INTERVIEW_OFFSET;

	//TV�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_TV_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_tv_bin, NARC_msg_tv_program_dat );
		scr_id -= ID_TV_OFFSET;

	//��`�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_HIDEN_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_hiden_bin, NARC_msg_hiden_dat );
		scr_id -= ID_HIDEN_OFFSET;

	//�]���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_HYOUKA_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_hyouka_scr_bin, NARC_msg_hyouka_dat );
		scr_id -= ID_HYOUKA_SCR_OFFSET;

	//�f�o�b�N�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_DEBUG_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_debug_scr_bin, NARC_msg_common_scr_dat );
		scr_id -= ID_DEBUG_SCR_OFFSET;

	//�R���e�X�g��t�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_CON_RECEPTION_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_con_reception_bin, NARC_msg_con_reception_dat );
		scr_id -= ID_CON_RECEPTION_OFFSET;

	//�A������X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_PAIR_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_pair_scr_bin, NARC_msg_pair_scr_dat );
		scr_id -= ID_PAIR_SCR_OFFSET;

	//�Q�[���J�n�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_INIT_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_init_scr_bin, NARC_msg_common_scr_dat );
		scr_id -= ID_INIT_SCR_OFFSET;

	//��ĉ��X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_SODATEYA_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_sodateya_bin, NARC_msg_sodateya_dat );
		scr_id -= ID_SODATEYA_OFFSET;

	//�|���g�~�j�Q�[���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_PORUTO_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_poruto_scr_bin, NARC_msg_pgamestart_dat );
		scr_id -= ID_PORUTO_SCR_OFFSET;

	//�O���[�v�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_GROUP_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_group_bin, NARC_msg_group_dat );
		scr_id -= ID_GROUP_OFFSET;

	//�|�P�Z���n���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_PC_UG_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_pc_ug_bin, NARC_msg_pc_ug_dat );
		scr_id -= ID_PC_UG_OFFSET;

	//�R���V�A���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_BATTLE_ROOM_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_battle_room_bin, NARC_msg_battle_room_dat );
		scr_id -= ID_BATTLE_ROOM_OFFSET;

	//�ʐM�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_CONNECT_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_connect_bin, NARC_msg_connect_dat );
		scr_id -= ID_CONNECT_OFFSET;

	//�|�P�T�[�`���[�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_POKESEARCHER_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_pokesearcher_bin, NARC_msg_bag_dat );
		scr_id -= ID_POKESEARCHER_OFFSET;

	//�Đ�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_SAISEN_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_saisen_bin, NARC_msg_saisen_dat );
		scr_id -= ID_SAISEN_OFFSET;

	//�y���b�v�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_PERAP_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_perap_bin, NARC_msg_perap_dat );
		scr_id -= ID_PERAP_OFFSET;

	//�T�t�@���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_SAFARI_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_safari_bin, NARC_msg_safari_dat );
		scr_id -= ID_SAFARI_OFFSET;

	//�B���A�C�e���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_HIDE_ITEM_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_hide_item_bin, NARC_msg_hide_item_dat );
		scr_id -= ID_HIDE_ITEM_OFFSET;

	//�t�B�[���h�A�C�e���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_FLD_ITEM_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_fld_item_bin, NARC_msg_fld_item_dat );
		scr_id -= ID_FLD_ITEM_OFFSET;

	//2vs2�g���[�i�[�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_TRAINER_2VS2_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_trainer_bin, NARC_msg_common_scr_dat );
		scr_id -= ID_TRAINER_2VS2_OFFSET;

	//�g���[�i�[�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_TRAINER_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_trainer_bin, NARC_msg_common_scr_dat );
		scr_id -= ID_TRAINER_OFFSET;

	//���̂݃X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_KINOMI_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_kinomi_bin, NARC_msg_kinomi_dat );
		scr_id -= ID_KINOMI_OFFSET;

	//BG�X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_BG_ATTR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_bg_attr_bin, NARC_msg_bg_attr_dat );
		scr_id -= ID_BG_ATTR_OFFSET;

	//���ʃX�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_COMMON_SCR_OFFSET ){
		SetScriptData( fsys, core, NARC_scr_seq_common_scr_bin, NARC_msg_common_scr_dat );
		scr_id -= ID_COMMON_SCR_OFFSET;

	//���[�J���X�N���v�g��ID���n���ꂽ��
	}else if( scr_id >= ID_START_SCR_OFFSET ){
		SetZoneScriptData( fsys, core );						//�]�[���X�N���v�g�f�[�^�Z�b�g
		scr_id -= ID_START_SCR_OFFSET;

	//SCRID_NULL(0)���n���ꂽ��
	}else{
		SetScriptData( fsys, core, NARC_scr_seq_dummy_scr_bin, NARC_msg_dummy_scr_dat );
		scr_id = 0;
	}

	return scr_id;
}
#endif

//----------------------------------------------------------------
/**
 * �X�N���v�g�f�[�^�Z�b�g
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	core		VMHANDLE�^�̃|�C���^
 * @param	index		�X�N���v�g�f�[�^��ID
 * @param	dat_id		���b�Z�[�W�f�[�^��ID
 */
//----------------------------------------------------------------
#define SCRIPT_MSG_NON (0xffffffff)

static void SetScriptData( SCRCMD_WORK *work,
	VMHANDLE *core, int index, u32 dat_id, HEAPID heapID )
{
	//���ʃX�N���v�g�f�[�^���[�h
	VM_CODE *script = GFL_ARC_UTIL_Load( ARCID_SCRSEQ, index, 0, heapID );
	core->pScript = (VM_CODE*)script;
	
#ifndef SCRIPT_PL_NULL
	//���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
	script->msgman = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, dat_id, heapID );
	core->msgman = MSGMAN_Create(
		MSGMAN_TYPE_DIRECT, ARC_MSG, dat_id, HEAPID_WORLD );
#else
	if( dat_id != SCRIPT_MSG_NON ){
		SCRCMD_WORK_CreateMsgData( work, dat_id );
	}
#endif
}

//----------------------------------------------------------------
/**
 * �]�[���X�N���v�g�f�[�^�Z�b�g
 * @param	ID		�X�N���v�g�f�[�^ID
 */
//----------------------------------------------------------------
static void SetZoneScriptData( FLDCOMMON_WORK* fsys, VMHANDLE* core )
{
#ifndef SCRIPT_PL_NULL
	//�]�[��ID���烍�[�h����X�N���v�g�o�C�i�����擾
	VM_CODE* script = LoadZoneScriptFile(fsys->location->zone_id);
	core->pScript	= (VM_CODE*)script;

	//���b�Z�[�W�f�[�^�}�l�[�W���[�쐬
	core->msgman = MSGMAN_Create( MSGMAN_TYPE_DIRECT, ARC_MSG, 
									LoadZoneMsgNo(fsys->location->zone_id), HEAPID_WORLD );
	
	return;
#else
	GF_ASSERT( 0 );
#endif
}


//======================================================================
//	�X�N���v�g���䃏�[�N�̃����o�[�A�N�Z�X
//======================================================================
//--------------------------------------------------------------
/**
 * �X�N���v�g���䃏�[�N�̃����o�[�A�h���X�擾
 * @param	sc		SCRIPT�^�̃|�C���^
 * @param	id		�擾���郁���oID(script.h�Q��)
 * @return	"�A�h���X"
 */
//--------------------------------------------------------------
void * SCRIPT_GetSubMemberWork( SCRIPT_WORK *sc, u32 id )
{
	EV_TRAINER_EYE_HITDATA *eye;
	
	switch( id ){
	//��b�E�B���h�E���b�Z�[�W�C���f�b�N�X�̃|�C���^
	case ID_EVSCR_MSGINDEX:
		return &sc->MsgIndex;
	//�A�j���[�V�����̐��̃|�C���^
	case ID_EVSCR_ANMCOUNT:
		return &sc->anm_count;
	//���ʃX�N���v�g�؂�ւ��t���O�̃|�C���^
	case ID_EVSCR_COMMON_SCR_FLAG:
		return &sc->common_scr_flag;
	//��b�E�B���h�E���J�������t���O�̃|�C���^
	case ID_EVSCR_WIN_OPEN_FLAG:
		return &sc->win_open_flag;
	//�ǉ��������z�}�V���̐��̃|�C���^
	case ID_EVSCR_VMHANDLE_COUNT:
		return &sc->vm_machine_count;
	//���C���̃X�N���v�gID
	case ID_EVSCR_SCRIPT_ID:
		return &sc->script_id;
	//�C�x���g�N�����̎�l���̌���
	case ID_EVSCR_PLAYER_DIR:
		return &sc->player_dir;
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
	case ID_EVSCR_VM_MAIN:
		return &sc->vm[VMHANDLE_MAIN];
	//���z�}�V��(�T�u)�̃|�C���^
	case ID_EVSCR_VM_SUB1:
		return &sc->vm[VMHANDLE_SUB1];
	//�T�u�v���Z�X�Ƃ̂��Ƃ胏�[�N�ւ̃|�C���^
	case ID_EVSCR_SUBPROC_WORK:
		return &sc->subproc_work;
	//���[�N�ւ̔ėp�|�C���^
	case ID_EVSCR_PWORK:
		return &sc->pWork;
	//�퓬���ʃt���O
	case ID_EVSCR_WIN_FLAG:
		return &sc->win_flag;
	//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)�̃|�C���^
	//�X�N���v�g�ƃv���O�����̃f�[�^�����p
	case ID_EVSCR_WK_PARAM0:				
	case ID_EVSCR_WK_PARAM1:
	case ID_EVSCR_WK_PARAM2:
	case ID_EVSCR_WK_PARAM3:
	//�X�N���v�g�ł̃e���|����
	case ID_EVSCR_WK_TEMP0:					
	case ID_EVSCR_WK_TEMP1:
	case ID_EVSCR_WK_TEMP2:
	case ID_EVSCR_WK_TEMP3:
	//�X�N���v�g�����ł̏����p
	case ID_EVSCR_WK_REG0:					
	case ID_EVSCR_WK_REG1:
	case ID_EVSCR_WK_REG2:
	case ID_EVSCR_WK_REG3:
	//�X�N���v�g�ɓ�����Ԃ��ėp���[�N
	case ID_EVSCR_WK_ANSWER:
	//�b�������Ώ�OBJID
	case ID_EVSCR_WK_TARGET_OBJID:
		return &sc->work[ id - ID_EVSCR_WK_START ];
	//GAMESYS_WORK wb
	case ID_EVSCR_WK_GAMESYS_WORK:
		return &sc->gsys;
	//HEAPID wb
	case ID_EVSCR_WK_HEAPID:
		return &sc->heapID;
	//SCRIPT_FLDPARAM wb
	case ID_EVSCR_WK_FLDPARAM:
		return &sc->fld_param;
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
	//����(0)�F��������
	case ID_EVSCR_TR0_RANGE:
		eye = &sc->eye_hitdata[0];
		return &eye->range;
	//����(0)�F����
	case ID_EVSCR_TR0_DIR:
		eye = &sc->eye_hitdata[0];
		return &eye->dir;
		GF_ASSERT( 0 );
		break;
	//����(0)�F�X�N���v�gID
	case ID_EVSCR_TR0_SCR_ID:
		eye = &sc->eye_hitdata[0];
		return &eye->scr_id;
	//����(0)�F�g���[�i�[ID
	case ID_EVSCR_TR0_ID:
		eye = &sc->eye_hitdata[0];
		return &eye->tr_id;
	//����(0)�F�g���[�i�[�^�C�v
	case ID_EVSCR_TR0_TYPE:
		eye = &sc->eye_hitdata[0];
		return &eye->tr_type;
	//����(0)�F�g���[�i�[OBJ
	case ID_EVSCR_TR0_FLDOBJ:
		eye = &sc->eye_hitdata[0];
		return &eye->mmdl;
	//����(0)�FTCB
	case ID_EVSCR_TR0_TCB:
		eye = &sc->eye_hitdata[0];
		return &eye->ev_eye_move;
	//����(1)�F��������
	case ID_EVSCR_TR1_RANGE:
		eye = &sc->eye_hitdata[1];
		return &eye->range;
	//����(1)�F����
	case ID_EVSCR_TR1_DIR:
		eye = &sc->eye_hitdata[1];
		return &eye->dir;
	//����(1)�F�X�N���v�gID
	case ID_EVSCR_TR1_SCR_ID:
		eye = &sc->eye_hitdata[1];
		return &eye->scr_id;
	//����(1)�F�g���[�i�[ID
	case ID_EVSCR_TR1_ID:
		eye = &sc->eye_hitdata[1];
		return &eye->tr_id;
	//����(1)�F�g���[�i�[�^�C�v
	case ID_EVSCR_TR1_TYPE:
		eye = &sc->eye_hitdata[1];
		return &eye->tr_type;
	//����(1)�F�g���[�i�[OBJ
	case ID_EVSCR_TR1_FLDOBJ:
		eye = &sc->eye_hitdata[1];
		return &eye->mmdl;
	//����(1)�FTCB
	case ID_EVSCR_TR1_TCB:
		eye = &sc->eye_hitdata[1];
		return &eye->ev_eye_move;

#ifndef SCRIPT_PL_NULL
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
		GF_ASSERT( (0) &&
		"�N��(�m��)���Ă��Ȃ��X�N���v�g�̃��[�N�ɃA�N�Z�X���Ă��܂��I" );
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
		sc->next_func = FieldMenuEvChg;
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

//--------------------------------------------------------------
/**
 *
 * @param	zone_id		�]�[��ID
 * @retval	""
 */
//--------------------------------------------------------------
static void * LoadZoneScriptFile(int zone_id)
{
#ifndef SCRIPT_PL_NULL
	return ArchiveDataLoadMalloc(ARC_SCRIPT, ZoneData_GetScriptArchiveID(zone_id), HEAPID_WORLD);
#else
	return NULL;
#endif
}

#if 0	//pl..
void * LoadZoneMsgFile(int zone_id);
void * LoadZoneMsgFile(int zone_id)
{
	return ArchiveDataLoadMalloc(ARC_SCRIPT_MSG, ZoneData_GetMsgArchiveID(zone_id), HEAPID_WORLD);
}
#endif

//--------------------------------------------------------------
/**
 * �}�b�v�Ǘ��\�ɓo�^����Ă���A���b�Z�[�W�t�@�C���̃A�[�J�C�uID���擾
 *
 * @param	zone_id		�]�[��ID
 *
 * @retval	"�A�[�J�C�uID"
 */
//--------------------------------------------------------------
static u32 LoadZoneMsgNo(int zone_id)
{
#ifndef SCRIPT_PL_NULL
	return ZoneData_GetMsgArchiveID(zone_id);
#else
	return 0;
#endif
}

//======================================================================
//	�C�x���g
//======================================================================
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
	VMHANDLE *core = NULL;
	EVENT_SCRIPT_WORK *ev_wk = wk;
	SCRIPT_WORK *sc = ev_wk->sc;
	
	switch( *seq ){
	case 0:
		//���z�}�V���ǉ�
		sc->vm[VMHANDLE_MAIN] = SCRIPT_AddVMachine(
				sc->gsys, sc, sc->heapID, sc->script_id );
		sc->vm_machine_count = 1;
		
		//���b�Z�[�W�֘A
		sc->wordset = WORDSET_CreateEx(
			WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, sc->heapID );
		sc->msg_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, sc->heapID );
		sc->tmp_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, sc->heapID );
		
		(*seq)++;
	case 1:
		//���z�}�V���R���g���[��
		for( i = 0; i < VMHANDLE_MAX; i++ ){
			core = sc->vm[i];
			if( core != NULL ){
				if( VM_Control(core) == FALSE ){	//���䃁�C��
					script_del( core );				//�X�N���v�g�폜
					
					if( sc->vm_machine_count == 0 ){
						GF_ASSERT( (0) && "���z�}�V���̐����s���ł��I" );
					}
					
					sc->vm[i] = NULL;
					sc->vm_machine_count--;
				}
			}
		}

		//���z�}�V���̐����`�F�b�N
		if( sc->vm_machine_count <= 0 ){
			SCRIPT_EVENTFUNC func = sc->next_func;		//�ޔ�
			
			WORDSET_Delete( sc->wordset );
			GFL_STR_DeleteBuffer( sc->msg_buf );
			GFL_STR_DeleteBuffer( sc->tmp_buf );

			//�f�o�b�N����
			//debug_script_flag = 0;
			
			sc->magic_no = 0;
			GFL_HEAP_FreeMemory( sc );		//�X�N���v�g���[�N
			
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
	MI_CpuClear8( ev_sc, sizeof(EVENT_SCRIPT_WORK) );
	
	ev_sc->sc = sc;
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
	MI_CpuClear8( ev_sc, sizeof(EVENT_SCRIPT_WORK) );

	ev_sc->sc = sc;
	return event;
}

//======================================================================
//	�C�x���g���[�N
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�A�h���X���擾
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
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
	return SCRIPT_GetMemberWork(
		sc, (ID_EVSCR_WK_START + work_no - SCWK_START) );
}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̒l���擾
 *
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
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
 * @param	fsys		FLDCOMMON_WORK�ւ̃|�C���^
 * @param	work_no		���[�N�i���o�[
 * @param	value		�Z�b�g����l
 *
 * @return	"TRUE=�Z�b�g�o�����AFALSE=�Z�b�g�o���Ȃ�����"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
BOOL SCRIPT_SetEventWorkValue(
	SCRIPT_WORK *sc, u16 work_no, u16 value )
{
	u16* res = GetEventWorkAdrs( fsys, work_no );
	if( res == NULL ){ return FALSE; }
	*res = value;
	return TRUE;
}
#endif

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
 * @brief	�C�x���g�t���O���`�F�b�N����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
BOOL CheckEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no)
{
	return EventWork_CheckEventFlag( SaveData_GetEventWork(fsys->savedata),	flag_no );
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O���Z�b�g����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 * @return	none
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void SetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no)
{
	EventWork_SetEventFlag( SaveData_GetEventWork(fsys->savedata),flag_no );
	return;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O�����Z�b�g����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 * @return	none
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void ResetEventFlag( FLDCOMMON_WORK* fsys, u16 flag_no)
{
	EventWork_ResetEventFlag( SaveData_GetEventWork(fsys->savedata), flag_no );
	return;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�}�b�v������̃Z�[�u�t���O���N���A����
 * @param	fsys		FLDCOMMON_WORK�̃|�C���^
 * @return	none
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void LocalEventFlagClear( FLDCOMMON_WORK* fsys )
{
	int i;
	EVENTWORK* event;

#if 0
	//�N���A�O
	for( i=0; i < 80 ;i++ ){
		OS_Printf( "flag %d= %d\n", i, CheckEventFlag(fsys,i) );
	}
	for( i=0x4000; i < 0x4020 ;i++ ){
		OS_Printf( "work %d= %d\n", i, GetEventWorkValue(fsys,i) );
	}
#endif

	//�C�x���g���[�N�̃|�C���^�擾
	event = SaveData_GetEventWork(fsys->savedata);

	//���[�J���t���O�̃N���A"8bit * 8 = 64��"
	//"0"�͖����ȃi���o�[�Ȃ̂�1��n���Ď擾���Ă���
	memset( EventWork_GetEventFlagAdrs(event,1), 0, LOCAL_FLAG_AREA_MAX );

	//���[�J�����[�N�̃N���A"32��"
	memset( EventWork_GetEventWorkAdrs(event,LOCAL_WORK_START), 0, 2*LOCAL_WORK_MAX );

#if 0
	//�N���A��
	for( i=0; i < 80 ;i++ ){
		OS_Printf( "flag %d= %d\n", i, CheckEventFlag(fsys,i) );
	}
	for( i=0x4000; i < 0x4020 ;i++ ){
		OS_Printf( "work %d= %d\n", i, GetEventWorkValue(fsys,i) );
	}
#endif

	return;
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
void TimeEventFlagClear( FLDCOMMON_WORK* fsys )
{
	EVENTWORK* event;

	//�C�x���g���[�N�̃|�C���^�擾
	event = SaveData_GetEventWork(fsys->savedata);

	memset( EventWork_GetEventFlagAdrs(event,TIMEFLAG_START), 0, TIMEFLAG_AREA_MAX );

	return;
}
#endif

//======================================================================
//		�v���O�������X�N���v�g�ėp
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	�v���O��������X�N���v�g�փp�����[�^��n��
 * @param	event	�X�N���v�g���N�����Ă���C�x���g�ւ̃|�C���^
 * @param	prm0	�p�����[�^�O�iSCWK_PARAM0�j
 * @param	prm1	�p�����[�^�P�iSCWK_PARAM1�j
 * @param	prm2	�p�����[�^�Q�iSCWK_PARAM2�j
 * @param	prm3	�p�����[�^�R�iSCWK_PARAM3�j
 */
//--------------------------------------------------------------
void SCRIPT_SetParam(
	SCRIPT_WORK *sc, u16 prm0, u16 prm1, u16 prm2, u16 prm3 )
{
	*(u16*)SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_PARAM0 ) = prm0;
	*(u16*)SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_PARAM1 ) = prm1;
	*(u16*)SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_PARAM2 ) = prm2;
	*(u16*)SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_PARAM3 ) = prm3;
}

//======================================================================
//	�g���[�i�[�t���O�֘A
//	�E�X�N���v�gID����A�g���[�i�[ID���擾���āA�t���O�`�F�b�N
//	BOOL CheckEventFlagTrainer( fsys, GetTrainerIdByScriptId(scr_id) );
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
#ifndef SCRIPT_PL_NULL
u16 GetTrainerIdByScriptId( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET + 1);		//1�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET + 1);		//1�I���W��
	}
}
#else
u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET + 1);		//1�I���W��
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET + 1);		//1�I���W��
	}
}
#endif

//--------------------------------------------------------------
/**
 * �X�N���v�gID����A���E�ǂ���̃g���[�i�[���擾
 * @param   scr_id		�X�N���v�gID
 * @retval  "0=���A1=�E"
 */
//--------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
BOOL GetTrainerLRByScriptId( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return 0;
	}else{
		return 1;
	}
}
#else
BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return 0;
	}else{
		return 1;
	}
}
#endif

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
#ifndef SCRIPT_PL_NULL
BOOL CheckEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id )
{
	return EventWork_CheckEventFlag( 
		SaveData_GetEventWork(fsys->savedata), GET_TRAINER_FLAG(tr_id) );
}
#else
BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  return EVENTWORK_CheckEventFlag(ev,GET_TRAINER_FLAG(tr_id) );
}
#endif

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
void SetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id )
{
#ifndef SCRIPT_PL_NULL
	EventWork_SetEventFlag( SaveData_GetEventWork(fsys->savedata), GET_TRAINER_FLAG(tr_id) );
	return;
#endif
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
#ifndef SCRIPT_PL_NULL
void ResetEventFlagTrainer( FLDCOMMON_WORK* fsys, u16 tr_id )
{
	EventWork_ResetEventFlag( SaveData_GetEventWork(fsys->savedata), GET_TRAINER_FLAG(tr_id) );
	return;
}
#endif


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
#ifndef SCRIPT_PL_NULL
u16 GetHideItemFlagNoByScriptId( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET + FH_FLAG_START);
}
#else //wb
u16 SCRIPT_GetHideItemFlagNoByScriptID( u16 scr_id )
{
	return (scr_id - ID_HIDE_ITEM_OFFSET + FH_FLAG_START);
}
#endif

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
	u16* param0 = SCRIPT_GetSubMemberWork( sc, ID_EVSCR_WK_PARAM0 );
	u16* param1 = SCRIPT_GetSubMemberWork( sc, ID_EVSCR_WK_PARAM1 );
	u16* param2 = SCRIPT_GetSubMemberWork( sc, ID_EVSCR_WK_PARAM2 );

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
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @return	none
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void GameStartScriptInit( FLDCOMMON_WORK* fsys )
{
	SpScriptStart( fsys, SCRID_INIT_SCRIPT );

#ifdef PM_DEBUG
	//�u�f�o�b�N�������v��I�����Ă�����
	if( DebugFlagData.debug_mode == TRUE ){

		//�|�P�Z���n���X�g�b�p�[�폜�A�Ƃ������蒠�C�x���g����
		SpScriptStart( fsys, SCRID_DEBUG_PC_UG );
	}
#endif
	return;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	EVENT�ł͂Ȃ�����X�N���v�g���s
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	scr_id		�X�N���v�gID
 * @return	none
 *
 * ���ӁI
 * SCRIPT���m�ۂ��Ă��Ȃ��̂ŁA
 * SCWK_ANSWER�Ȃǂ̃��[�N�͎g�p���邱�Ƃ��o���Ȃ��I
 * LOCALWORK0�Ȃǂ��g�p����悤�ɂ���I
 *
 * ���ʃX�N���v�g�ɂ��Ă͌���g�p�s�I
 * �Ή��\�肾���A�]�[�����܂��������ɏ����������邩���H
 * ���܂����Ȃ������H
 *
 * �t���O�`�F���W���x���ŋ��ʃX�N���v�g���g�������Ƃ��́A
 * ��t���O�`�F���W���x�����������ƂɂȂ邩���H
 * �Е��́A�]�[���ł��Ƃ������́A�Е��́A���ʃX�N���v�g��ID���w��H�B�B�B
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
void SpScriptStart( FLDCOMMON_WORK* fsys, u16 scr_id )
{
	VMHANDLE*core = VMMachineAdd( fsys, scr_id );
	while( VM_Control( core ) == TRUE );
	script_del(core);	//�X�N���v�g�폜
	return;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	����X�N���v�g�������Ď��s
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	key			����X�N���v�gID
 * @return	"TRUE=����X�N���v�g���s�AFALSE=�������Ȃ�"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
BOOL SpScriptSearch( FLDCOMMON_WORK* fsys, u8 key )
{
	u16 scr_id;
	const u8 * p;

	p = EventData_GetSpecialScript(fsys);
	if (p == NULL) {
		return FALSE;
	}
	//����X�N���v�g����
	if( key == SP_SCRID_SCENE_CHANGE ){
		scr_id = SpScriptSearch_Sub2( fsys, p, key );	//�����`�F�b�N�L��
	}else{
		scr_id = SpScriptSearch_Sub( p, key );			//�����`�F�b�N����
	}

	//OS_Printf( "scr_id = %d\n", scr_id );
	if( scr_id == 0xffff ){
		return FALSE;
	}

	//����X�N���v�g���s
	if( key == SP_SCRID_SCENE_CHANGE ){
		EventSet_Script( fsys, scr_id, NULL );
	}else{
		SpScriptStart( fsys, scr_id );
	}
	return TRUE;
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�w�b�_�[�f�[�^����w��^�C�v�̂��̂�����
 * @param	key			����X�N���v�gID
 * @return	"�X�N���v�gID"
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static u16 SpScriptSearch_Sub(const u8 * p, u8 key )
{
	while( 1 ){
		//�I���L�q�`�F�b�N(SP_EVENT_DATA_END)
		if( *p == SP_SCRID_NONE ){
			return 0xffff;
		}

		//�w��^�C�v�̓���X�N���v�g����
		if( *p == key ){
			p++;								//����X�N���v�gID������
			return ( *p + ( *(p+1)<<8 ) );		//�X�N���v�gID��Ԃ�
		}

		p += (1 + 2 + 2);						//����X�N���v�gID+�X�N���v�gID+�_�~�[������
	}
	return 0xffff;								//����������Ȃ�����
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�w�b�_�[�f�[�^����w��^�C�v�̂��̂���������(�V�[���C�x���g)
 * @param	key			����X�N���v�gID
 * @return	"�X�N���v�gID"
 * 08/04/30
 * LOCALWORK0���V�[���`�F���W�Ŏw�肵�Ă�����X�N���v�g�������Ȃ��B
 * �����𒲂ׂĂ݂���ASP_SCRIPT_END(=0)���`�F�b�N���Ă��鏈�����A
 * ���[�N��u8�ŏ������Ă���̂ŁA
 * LOCALWORK0(=0x4000)�Ȃ̂�1byte������ƁA
 * 0�̂��߁ASP_SCRIPT_END�Ƃ��ď�������Ă��܂��Ă����B
 */
//------------------------------------------------------------------
#ifndef SCRIPT_PL_NULL
static u16 SpScriptSearch_Sub2( FLDCOMMON_WORK* fsys, const u8 * p, u8 key )
{
	u16 work1,work2;
	u32 pos;

	pos = 0;

	while( 1 ){
		//�I���L�q�`�F�b�N(SP_EVENT_DATA_END)
		if( *p == SP_SCRID_NONE ){
			return 0xffff;
		}

		//�w��^�C�v�̓���X�N���v�g����
		if( (*p) == key ){
			p++;			//����X�N���v�gID������
			pos = ( *p + ( *(p+1)<<8 ) + ( *(p+2)<<16 ) + ( *(p+3)<<24 ) );
			p += 4;			//�A�h���X������
			break;
		}

		p += (1 + 4);		//����X�N���v�gID+�A�h���X������
	}

	//������Ȃ�������
	if( pos == 0 ){
		return 0xffff;
	}

	//����X�N���v�g�e�[�u�����Z�b�g
	p = (p + pos);

	while ( TRUE ) {
		//�I���L�q�`�F�b�N(SP_SCRIPT_END)
		if( *p == 0 ){
			return 0xffff;
		}

		//��r�����[�N�擾
		work1 = ( *p + ( *(p+1)<<8 ) );
		if( work1 == 0 ){
			return 0xffff;
		};
		p += 2;									//���[�N������
		//OS_Printf( "work1 = %d\n", work1 );

		//��r���l(���[�N��)�擾
		work2 = ( *p + ( *(p+1)<<8 ) );
		p += 2;									//��r����l������(���[�N��)
		//OS_Printf( "work2 = %d\n", work2 );

		//�����`�F�b�N
		if( GetEventWorkValue(fsys,work1) == GetEventWorkValue(fsys,work2) ){
			return ( *p + ( *(p+1)<<8 ) );
		}

		p += 2;									//�X�N���v�gID������
	}
	return 0xffff;								//����������Ȃ�����
}
#endif


//======================================================================
//
//	���g�p
//
//======================================================================
#if 0
static void FieldScriptLoad( u16 map, u16 id );
static void FieldScriptFileNameMake( char * path, u16 map, u16 id );

static VMHANDLE field_script;			// ���z�}�V���p����\����
static u8 field_script_status;			// �X�N���v�g��ԕϐ�
//static u8 ForceEventFlag;				// �����C�x���g����t���O
static VM_CODE * pScript = NULL;		// �ʏ�X�N���v�g
//static u16 * ScriptWork = NULL;		// �X�N���v�g���[�N

//	�X�N���v�g��Ԓ�`
enum{
	SCRIPT_ON,
	SCRIPT_WAIT,
	SCRIPT_OFF,
};

static void FieldScriptFileNameMake( char * path, u16 map, u16 id )
{
	strcpy( path, "/data/script/script_map" );

	StrNumSet( &path[23], map, 3 );		// "/data/script/script_map"
	strcat( path, "_" );
	StrNumSet( &path[27], id, 2 );		// "/data/script/script_map000_"
	strcat( path, ".dat" );
}

static void FieldScriptLoad( u16 map, u16 id )
{
	char * path;

	path = (char *)sys_AllocMemory( APP_W_HEAP_AREA, 48 );
	FieldScriptFileNameMake( path, map, id );
	pScript = (VM_CODE *)sys_LoadFile( APP_W_HEAP_AREA, path );
	sys_FreeMemory( APP_W_HEAP_AREA, path );
}

//--------------------------------------------------------------
/**
 * �X�N���v�g�N���`�F�b�N
 *
 * @param	none
 *
 * @retval	"TRUE = �N�����Ă���"
 * @retval	"FALSE = �N�����Ă��Ȃ�"
 */
//--------------------------------------------------------------
u8 CheckScriptStatus(void)
{
	if( field_script_status == SCRIPT_ON ){
		return TRUE;
	}
	return FALSE;
}

#endif


//======================================================================
//
//	VMMachine��TCB��ǉ�
//
//======================================================================

#if 0
//--------------------------------------------------------------
/**
 * @brief	���z�}�V���ǉ�
 *
 * @param	fsys		FLDCOMMON_WORK�^�̃|�C���^
 * @param	id			�X�N���v�gID
 * @param	start		���߃e�[�u���J�n�A�h���X
 * @param	end			���߃e�[�u���I���A�h���X
 *
 * @retval	none
 */
//--------------------------------------------------------------
void VMMachineAddTCB(FLDCOMMON_WORK* fsys, u16 id, const VM_CMD* start, const VM_CMD* end)
{
	SCRIPT_WORK * sc;
	VMHANDLE* core = NULL;

	sc = FieldEvent_GetSpecialWork( fsys->event );

	//core = sys_AllocMemory( HEAPID_FIELD, sizeof(VMHANDLE) );
	core = sys_AllocMemory( HEAPID_WORLD, sizeof(VMHANDLE) );
	if( core == NULL ){
		GF_ASSERT( (0) && "�������m�ۂɎ��s���܂����I" );
	}

	//���z�}�V��������
	//VM_Init( core, &ScriptCmdTbl[0], &ScriptCmdTbl[EVCMD_MAX] );
	VM_Init( core, start, end );

	InitScript( fsys, core, id, 0 );							//���[�J���X�N���v�g������
	
	TCB_Add( VMMachineControlTCB, core, 0 );
	sc->vm_machine_count++;
	return;
}

//--------------------------------------------------------------
/**
 * @brief	TCB���C��
 *
 * @param	tcb		TCB_PTR
 * @param	wk		���[�N�̃A�h���X
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void VMMachineControlTCB( TCB_PTR tcb, void* wk )
{
	VMHANDLE *core;
	FLDCOMMON_WORK* fsys;
	SCRIPT_WORK * sc;

	core = (VMHANDLE *)wk;
	fsys = FieldEvent_GetFieldSysWork( core->event_work );
	sc = FieldEvent_GetSpecialWork( fsys->event );

	if( VM_Control( core ) == FALSE ){

		MSGMAN_Delete( core->msgman );

		sys_FreeMemoryEz( core->pScript );			//�X�N���v�g�f�[�^

		TCB_Delete( tcb );
		sys_FreeMemoryEz( core );

		if( sc->vm_machine_count == 0 ){
			GF_ASSERT( (0) && "���z�}�V���̐����s���ł��I" );
		}

		sc->vm_machine_count--;
	};

	return;
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * @brief	�X�N���v�g���䃏�[�N��VM�|�C���^�e�[�u���̋󂫂�����
 *
 * @param	event		GMEVENT_CONTROL�^
 *
 * @retval	"0xff = �󂫂Ȃ��A����ȊO�͊Ǘ�ID"
 */
//--------------------------------------------------------------
u8 EvScriptWork_VMEmptySearch(GMEVENT_CONTROL * event);
u8 EvScriptWork_VMEmptySearch(GMEVENT_CONTROL * event)
{
	VMHANDLE* core = NULL;
	SCRIPT_WORK * sc = FieldEvent_GetSpecialWork( event );

	for( i=0; i < VMHANDLE_MAX; i++ ){

		core = sc->vm[i];

		if( core == NULL ){
			return i;
		}
	}

	return 0xff;
}
#endif

