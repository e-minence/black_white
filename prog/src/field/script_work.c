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

#include "trainer_eye_data.h"   //SCR_TRAINER_HITDATA
#include "field/eventdata_system.h" //EVENTDATA_GetSpecialScriptData

#include "fieldmap.h"   //FIELDMAP_GetFldMsgBG
#include "battle/battle.h"

#include "musical/musical_event.h"  //MUSICAL_EVENT_WORK

#include "script_trainer.h" //SCRIPT_GetTrainerHitData
#include "scrcmd_work.h"

//============================================================================================
//  define
//============================================================================================
enum
{
	WORDSET_SCRIPT_SETNUM = 32,		///<�f�t�H���g�o�b�t�@��
	WORDSET_SCRIPT_BUFLEN = 64,		///<�f�t�H���g�o�b�t�@���i�������j
  //SCR_MSG_BUF_SIZE	= (512),				///<���b�Z�[�W�o�b�t�@�T�C�Y
  SCR_MSG_BUF_SIZE =	(1024)				//���b�Z�[�W�o�b�t�@�T�C�Y
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
	GFL_FONT* font;			    //�t�H���g�|�C���^�@�C��_�a�Í��t�H���g�Ɏg�p


  void *mw;				//�r�b�g�}�b�v���j���[���[�N

	void * subproc_work; //�T�u�v���Z�X�Ƃ̂��Ƃ�Ɏg�p���郏�[�N�ւ�*

  u8 sound_se_flag;   ///< �T�E���hSE�Đ��`�F�b�N�t���O

	//�g���[�i�[�������
	SCR_TRAINER_HITDATA trainer_hitdata[TRAINER_EYE_HITMAX];
	
	u16 scrTempWork[TEMP_WORK_SIZE];		//���[�N(ANSWORK,TMPWORK�Ȃǂ̑���)

  FIELD_SAVEANIME* bg_saveanime;
  
  EV_MOVEPOKE_WORK* movepoke;

  // �Đ�g���[�i�[�e���|����
  void* rebattle_trainer;

  void* scrcmd_global;
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

  sc->scrcmd_global = SCRCMD_GLOBAL_Create( sc, main_heapID );

  //�t�B�[���h�p�����[�^����
	initFldParam( &sc->fld_param, gsys );

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

  SCRCMD_GLOBAL_Delete( sc->scrcmd_global );

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
 * @brief �X�N���v�g�R�}���h�p���[�N�ւ̃|�C���^�擾
 */
//--------------------------------------------------------------
void * SCRIPT_GetScrCmdGlobal( SCRIPT_WORK * sc )
{
  GF_ASSERT( sc->magic_no );
  return sc->scrcmd_global;
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

//----------------------------------------------------------------------------
/**
 *	@brief  �t�H���g�n���h����ݒ�    �i�C��_�a�@�Í����b�Z�[�W�Ɏg�p�j
 *
 *	@param	sc
 *	@param	fontHandle 
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetFontHandle( SCRIPT_WORK * sc, GFL_FONT* fontHandle )
{
  sc->font = fontHandle;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�H���g�n���h���̎擾  �i�C��_�a�@�Í����b�Z�[�W�Ɏg�p�j
 *
 *	@param	sc 
 *
 *	@return �t�H���g�n���h��
 */
//-----------------------------------------------------------------------------
GFL_FONT* SCRIPT_GetFontHandle( const SCRIPT_WORK * sc )
{
  return sc->font;
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

  if ( work_no < SCWK_END ){
    //�X�N���v�g���䃏�[�N�̒��ŁAANSWORK�Ȃǂ̃��[�N���m�ۂ��Ă��܂�
    return getTempWork( sc, work_no );
  }
	
  return NULL;
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
  GFL_HEAP_FreeMemory( backup_work );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�uBG�A�j���[�V�������[�N�̊Ǘ�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�uBG�A�j���[�V�������[�N�̕ۑ�
 *
 *	@param	sc    ���[�N
 *	@param	wk    �Z�[�uBG�A�j���[�V�������[�N
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetSaveAnimeWork( SCRIPT_WORK * sc, FIELD_SAVEANIME * wk )
{
  sc->bg_saveanime = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ۑ����Ă���Z�[�uBG�A�j���[�V�������[�N�擾
 *
 *	@param	sc  ���[�N
 */
//-----------------------------------------------------------------------------
FIELD_SAVEANIME * SCRIPT_GetSaveAnimeWork( SCRIPT_WORK * sc )
{
  return sc->bg_saveanime;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����A�j���[�V�������[�N�̊Ǘ�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����A�j���[�V�������[�N�̐ݒ�
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetMovePokeWork( SCRIPT_WORK * sc, EV_MOVEPOKE_WORK * wk )
{
  sc->movepoke = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ړ��|�P�����A�j���[�V�������[�N�̎擾
 */
//-----------------------------------------------------------------------------
EV_MOVEPOKE_WORK * SCRIPT_GetMovePokeWork( SCRIPT_WORK * sc )
{
  return sc->movepoke;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[���[�N�̕ۑ�
 */
//-----------------------------------------------------------------------------
void SCRIPT_SetReBattleTrainerData( SCRIPT_WORK * sc, void * wk )
{
  sc->rebattle_trainer = wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Đ�g���[�i�[���[�N�擾
 *	@return ���[�N
 */
//-----------------------------------------------------------------------------
void * SCRIPT_GetReBattleTrainerData( SCRIPT_WORK * sc )
{
  return sc->rebattle_trainer;
}




//--------------------------------------------------------------
/**
 * �g���[�i�[�������̎擾
 */
//--------------------------------------------------------------
SCR_TRAINER_HITDATA * SCRIPT_GetTrainerHitData( SCRIPT_WORK * sc, u32 tr_no )
{
  GF_ASSERT( tr_no < TRAINER_EYE_HITMAX );
  return &sc->trainer_hitdata[tr_no];
}


