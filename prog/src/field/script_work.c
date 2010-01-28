//============================================================================================
/**
 * @file	script_work.c
 * @brief	�X�N���v�g�O���[�o�����[�N��`
 * @date  2010.01.26
 * @author  tamada
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_local.h"

#include "scrcmd_work.h"
#include "eventwork_def.h"

#include "field/zonedata.h"

#include "print/wordset.h"    //WORDSET

#include "trainer_eye_data.h"   //EV_TRAINER_EYE_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG
#include "battle/battle.h"

#include "musical/musical_event.h"  //MUSICAL_EVENT_WORK


//============================================================================================
//  define
//============================================================================================
enum
{
	WORDSET_SCRIPT_SETNUM = 32,		///<�f�t�H���g�o�b�t�@��
	WORDSET_SCRIPT_BUFLEN = 64,		///<�f�t�H���g�o�b�t�@���i�������j
  SCR_MSG_BUF_SIZE	= (512),				///<���b�Z�[�W�o�b�t�@�T�C�Y
  //SCR_MSG_BUF_SIZE =	(1024)				//���b�Z�[�W�o�b�t�@�T�C�Y
};


#define SCRIPT_MAGIC_NO		(0x3643f)

#define SCRIPT_MSG_NON (0xffffffff)

#define SCRIPT_NOT_EXIST_ID (0xffff)

enum {
  TEMP_WORK_START = SCWK_AREA_START,
  TEMP_WORK_END = USERWK_AREA_END,
  TEMP_WORK_SIZE = TEMP_WORK_END - TEMP_WORK_START,

  USERWK_SIZE = USERWK_AREA_END - USERWK_AREA_START,
};

//============================================================================================
//	struct
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�O���[�o�����[�N��`
 *
 * �e���z�}�V�����܂����ő��݂���X�N���v�g�p���[�N
 */
//--------------------------------------------------------------
struct _TAG_SCRIPT_WORK
{
  //��������������������������������������������������������
  //script.c ��Private�ȃ����o
  //��������������������������������������������������������
	u32 magic_no;			//�C�x���g�̃��[�N���X�N���v�g���䃏�[�N���𔻕�
	
  //��������������������������������������������������������
  //SCRIPT_WORK�������Ɉ����ŏ���������AReadOnly�ŊO�����J����郁���o
  //��������������������������������������������������������
	u16 start_scr_id;			//���C���̃X�N���v�gID
	MMDL *target_obj;

	HEAPID main_heapID;

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


  void *msgWin;           //���j���[�n���p���b�Z�[�W�E�B���h�E

  void *mw;				//�r�b�g�}�b�v���j���[���[�N

	void * subproc_work; //�T�u�v���Z�X�Ƃ̂��Ƃ�Ɏg�p���郏�[�N�ւ�*

	u8 anm_count;			//�A�j���[�V�������Ă��鐔
	
  u8 sound_se_flag;   ///< �T�E���hSE�Đ��`�F�b�N�t���O

	//�g���[�i�[�������
	EV_TRAINER_EYE_HITDATA eye_hitdata[TRAINER_EYE_HITMAX];
	
	u16 scrTempWork[TEMP_WORK_SIZE];		//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)

  MUSICAL_EVENT_WORK *musicalEventWork;

};

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================


//�C�x���g���[�N�A�X�N���v�g���[�N�A�N�Z�X�֐�
static u16 * getTempWork( SCRIPT_WORK * sc, u16 work_no );

static void initFldParam(SCRIPT_FLDPARAM * fparam, GAMESYS_WORK * gsys);

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


//============================================================================================
//
//  �X�N���v�g���[�N
//
//============================================================================================
//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�p�O���[�o�����[�N�̐���
 * @param main_heapID
 * @param gsys
 * @param event
 * @param	scr_id		�X�N���v�gID
 * @param	ret_wk		�X�N���v�g���ʂ������郏�[�N�̃|�C���^
 * @return	SCRIPT_WORK			SCRIPT�^�̃|�C���^
 *
 * �X�N���v�g�R�}���h�S�ʂ���O���[�o���A�N�Z�X�\�ȃf�[�^��ێ�����
 */
//--------------------------------------------------------------
SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID,
    GAMESYS_WORK * gsys, GMEVENT * event, u16 scr_id, void* ret_wk)
{
  SCRIPT_WORK * sc;

	sc = GFL_HEAP_AllocClearMemory( main_heapID, sizeof(SCRIPT_WORK) );
	sc->magic_no = SCRIPT_MAGIC_NO;
	sc->main_heapID = main_heapID;

	sc->gsys = gsys;
  sc->gmevent = event;
	sc->start_scr_id  = scr_id;	//���C���̃X�N���v�gID
	sc->target_obj = NULL;
	sc->ret_script_wk = ret_wk;	//�X�N���v�g���ʂ������郏�[�N
  
  //���b�Z�[�W�֘A
  sc->wordset = WORDSET_CreateEx(
    WORDSET_SCRIPT_SETNUM, WORDSET_SCRIPT_BUFLEN, main_heapID );
  sc->msg_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );
  sc->tmp_buf = GFL_STR_CreateBuffer( SCR_MSG_BUF_SIZE, main_heapID );

  //�t�B�[���h�p�����[�^����
	initFldParam( &sc->fld_param, gsys );
  //�~���[�W�J���̐���C�x���g�œn�����
  sc->musicalEventWork = NULL;

  //�`�F�b�N�r�b�g������
  SCREND_CHK_ClearBits();

  return sc;
}

//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�p�O���[�o�����[�N�̍폜
 */
//--------------------------------------------------------------
void SCRIPTWORK_Delete( SCRIPT_WORK * sc )
{
  sc->magic_no = 0;
  WORDSET_Delete( sc->wordset );
  GFL_STR_DeleteBuffer( sc->msg_buf );
  GFL_STR_DeleteBuffer( sc->tmp_buf );

  GFL_HEAP_FreeMemory( sc );
}

//============================================================================================
//	�X�N���v�g���䃏�[�N�̃����o�[�A�N�Z�X
//============================================================================================
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
 * @brief �P��Z�b�g�ێ��|�C���^�̎擾
 * @param	sc		    SCRIPT_WORK�̃|�C���^
 */
//--------------------------------------------------------------
WORDSET * SCRIPT_GetWordSet( SCRIPT_WORK * sc )
{
  return sc->wordset;
}
//--------------------------------------------------------------
/**
 * @brief MSG�o�b�t�@�̎擾
 * @param	sc		    SCRIPT_WORK�̃|�C���^
 */
//--------------------------------------------------------------
STRBUF * SCRIPT_GetMsgBuffer( SCRIPT_WORK * sc )
{
  return sc->msg_buf;
}

//--------------------------------------------------------------
/**
 * @brief MSG�W�J�p�e���|�����o�b�t�@�̎擾
 * @param	sc		    SCRIPT_WORK�̃|�C���^
 */
//--------------------------------------------------------------
STRBUF * SCRIPT_GetMsgTempBuffer( SCRIPT_WORK * sc )
{
  return sc->tmp_buf;
}
//--------------------------------------------------------------
/**
 * @brief SE����p�t���O�̎擾
 */
//--------------------------------------------------------------
u8 * SCRIPT_GetSoundSeFlag( SCRIPT_WORK * sc )
{
  return &sc->sound_se_flag;
}
//--------------------------------------------------------------
/**
 * @brief �X�N���v�g�ϐ��F�A�j���J�E���g�̎擾
 */
//--------------------------------------------------------------
u8 * SCRIPT_GetAnimeCount( SCRIPT_WORK * sc )
{
  return &sc->anm_count;
}
//--------------------------------------------------------------
/**
 * @brief   �J�n���̃X�N���v�gID�擾
 * @param	sc		    SCRIPT_WORK�̃|�C���^
 * @return  u16   �X�N���v�gID
 */
//--------------------------------------------------------------
u16 SCRIPT_GetStartScriptID( const SCRIPT_WORK * sc )
{
  return sc->start_scr_id;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void * SCRIPT_GetFLDMENUFUNC( SCRIPT_WORK * sc )
{
  return sc->mw;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void SCRIPT_SetFLDMENUFUNC( SCRIPT_WORK * sc, void * mw )
{
  sc->mw = mw;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
MMDL * SCRIPT_GetTargetObj( SCRIPT_WORK * sc )
{
  return sc->target_obj;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void SCRIPT_SetTargetObj( SCRIPT_WORK * sc, MMDL * obj )
{
	sc->target_obj = obj;		//�b�������Ώ�OBJ�̃|�C���^�Z�b�g
	if( obj != NULL ){
    u16 *objid;
    objid = getTempWork( sc, SCWK_TARGET_OBJID );
		*objid = MMDL_GetOBJID( obj ); //�b�������Ώ�OBJID�̃Z�b�g
	}
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
//--------------------------------------------------------------
//--------------------------------------------------------------
void * SCRIPT_GetMemberWork_Musical( SCRIPT_WORK * sc )
{
  GF_ASSERT_MSG( sc->musicalEventWork != NULL , "�~���[�W�J�����[�N��NULL�I" );
  return sc->musicalEventWork;
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



//============================================================================================
//	�X�N���v�g���[�N�E�t���O�֘A
//============================================================================================
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

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void * SCRIPT_BackupUserWork( SCRIPT_WORK * sc )
{
  u16 * backup_work = GFL_HEAP_AllocClearMemory( sc->main_heapID, sizeof(u16) * USERWK_SIZE );
  GFL_STD_MemCopy16( getTempWork( sc, USERWK_AREA_START ), backup_work, sizeof(u16) * USERWK_SIZE );
  return backup_work;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void SCRIPT_RestoreUserWork( SCRIPT_WORK * sc, void *backup_work )
{
  GFL_STD_MemCopy16( backup_work, getTempWork( sc, USERWK_AREA_START ), sizeof(u16) * USERWK_SIZE );
}

//============================================================================================
//
//	�g���[�i�[�t���O�֘A
//	�E�X�N���v�gID����A�g���[�i�[ID���擾���āA�t���O�`�F�b�N
//
//============================================================================================
//ID_TRAINER_2VS2_OFFSET, ID_TRAINER_OFFSET
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"

#define GET_TRAINER_FLAG(id)	( TR_FLAG_START+(id) )

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
  SCRIPT_WORK *sc =  SCRIPT_GetScriptWorkFromEvent( event );
	EV_TRAINER_EYE_HITDATA *eye = &sc->eye_hitdata[tr_no];
  GF_ASSERT( tr_no < TRAINER_EYE_HITMAX );
  TRAINER_EYE_HITDATA_Set(eye, mmdl, range, dir, scr_id, tr_id, tr_type, tr_no );
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�������̎擾
 */
//--------------------------------------------------------------
void * SCRIPT_GetTrainerEyeData( SCRIPT_WORK * sc, u32 tr_no )
{
  GF_ASSERT( tr_no < TRAINER_EYE_HITMAX );
  return &sc->eye_hitdata[tr_no];
}


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
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  //OS_Printf( "check 2v2 TRID=%d, type = %d\n", tr_id, rule );

  switch( rule ){
  case BTL_RULE_SINGLE:
    return( 0 );
  case BTL_RULE_DOUBLE:
  case BTL_RULE_TRIPLE: //kari
    return( 1 );
  }
  
//  GF_ASSERT( 0 );
  return( 1 );
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


