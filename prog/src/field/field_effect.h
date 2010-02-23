//======================================================================
/**
 * @file	field_effect.h
 * @brief	�t�B�[���h�G�t�F�N�g
 * @authaor	kagaya
 * @date	2008.12.11
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"

#include "field/fieldmap_proc.h"
#include "field_g3dobj.h"

#include "arc/fieldmap/fldeff.naix"

//======================================================================
//  define
//======================================================================
#define FLDEFF_TASK_WORK_SIZE (160) ///<FLDEFF_TASK�Ŏg�p���郏�[�N�T�C�Y
#define FLDEFF_TASK_MAX (94) //64(�Œ���K�v)+30(�M�~�b�N)

//--------------------------------------------------------------
/// FLDEFF_PROCID
//--------------------------------------------------------------
typedef enum
{
  FLDEFF_PROCID_SHADOW = 0, ///<�e
  FLDEFF_PROCID_KEMURI, ///<�y���G�t�F�N�g
  FLDEFF_PROCID_GRASS, ///<���G�t�F�N�g
  FLDEFF_PROCID_NAMIPOKE, ///<�g���|�P����
  FLDEFF_PROCID_GYOE, ///<�т�����}�[�N
  FLDEFF_PROCID_FOOTMARK, ///<����
  FLDEFF_PROCID_REFLECT, ///<�f�荞��
  FLDEFF_PROCID_RIPPLE, ///<���g��
  FLDEFF_PROCID_SPLASH, ///<����
  FLDEFF_PROCID_NAMIPOKE_EFFECT, ///<�g���|�P�����G�t�F�N�g
  FLDEFF_PROCID_IWAKUDAKI, ///<��ӂ�
  FLDEFF_PROCID_D06DENKI, ///<D06�d�C���A
  FLDEFF_PROCID_IAIGIRI, ///<�������؂�
  FLDEFF_PROCID_FISHING_LURE, ///<�ނ�C�x���g�̃��A�[
  FLDEFF_PROCID_BTRAIN, ///<�o�g���g���C��
  FLDEFF_PROCID_HIDE, ///<���샂�f���B�ꖪ
  FLDEFF_PROCID_BUBBLE, ///<���A

  //��������G�t�F�N�g�G���J�E���g�p
  FLDEFF_PROCID_ENC_SGRASS, ///<�G�t�F�N�g�G���J�E���g�p�@�Z����
  FLDEFF_PROCID_ENC_LGRASS, ///<�G�t�F�N�g�G���J�E���g�p�@������
  FLDEFF_PROCID_ENC_CAVE,   ///<�G�t�F�N�g�G���J�E���g�p�@���A
  FLDEFF_PROCID_ENC_WATER,  ///<�G�t�F�N�g�G���J�E���g�p�@�W��
  FLDEFF_PROCID_ENC_SEA,    ///<�G�t�F�N�g�G���J�E���g�p�@�C
  FLDEFF_PROCID_ENC_BRIDGE, ///<�G�t�F�N�g�G���J�E���g�p�@��
  
  FLDEFF_PROCID_MAX, ///<�ő�
}FLDEFF_PROCID;

//======================================================================
//  struct
//======================================================================
///FLDEFF_CTRL
typedef struct _TAG_FLDEFF_CTRL FLDEFF_CTRL;
///FLDEFF_TASK
typedef struct _TAG_FLDEFF_TASK FLDEFF_TASK;
///FLDEFF_TASKSYS
typedef struct _TAG_FLDEFF_TASKSYS FLDEFF_TASKSYS;

///FLDEFF_TASK �������֐�
typedef void (*FLDEFF_TASK_INIT)( FLDEFF_TASK*, void *wk );
///FLDEFF_TASK �폜
typedef void (*FLDEFF_TASK_DELETE)( FLDEFF_TASK*, void *wk );
///FLDEFF_TASK �X�V
typedef void (*FLDEFF_TASK_UPDATE)( FLDEFF_TASK*, void *wk );
///FLDEFF_TASK �`��
typedef void (*FLDEFF_TASK_DRAW)( FLDEFF_TASK*, void *wk );

//--------------------------------------------------------------
///	�G�t�F�N�g�T�u�v���Z�X�������֐��^��`�B
/// �����FFLDEFF_CTRL* �߂�l�F�֐��Ŏg�p���郏�[�N *
//--------------------------------------------------------------
typedef void * (*FLDEFF_PROCEFF_INIT)( FLDEFF_CTRL *, HEAPID heapID );

//--------------------------------------------------------------
///	�G�t�F�N�g�T�u�v���Z�X�폜�֐��^��`�B
/// �����Fvoid * = FLDEFF_PROCEFF_INIT�̖߂�l
//--------------------------------------------------------------
typedef void (*FLDEFF_PROCEFF_DELETE)( FLDEFF_CTRL*, void * );

//--------------------------------------------------------------
/// FLDEFF_PROCEFF_DATA
//--------------------------------------------------------------
typedef struct
{
  u32 id; ///<proc_init,proc_del���Ăяo���ׂ̎���ID
  FLDEFF_PROCEFF_INIT proc_init; ///<id���w�肵���ۂɌĂ΂�鏉�����֐�
	FLDEFF_PROCEFF_DELETE proc_del; ///<id���w�肵���ۂɌĂ΂��폜�֐�
}FLDEFF_PROCEFF_DATA;

//--------------------------------------------------------------
/// FLDEFF_TASK_HEADER
//--------------------------------------------------------------
typedef struct
{
  u32 work_size; ///<�^�X�N�Ŏg�p���郏�[�N�̃T�C�Y
  FLDEFF_TASK_INIT proc_init; ///<�^�X�N�ǉ����ɌĂяo����鏉�����֐�
  FLDEFF_TASK_DELETE proc_delete; ///<�^�X�N�폜���ɌĂяo�����폜�֐�
  FLDEFF_TASK_UPDATE proc_update; ///<�X�V���ɌĂяo�����X�V�֐�
  FLDEFF_TASK_DRAW proc_draw; ///<�`�掞�ɌĂяo�����`��֐�
}FLDEFF_TASK_HEADER;

//======================================================================
//  extern
//======================================================================
//FLDEFF_CTRL
extern FLDEFF_CTRL * FLDEFF_CTRL_Create(
    FIELDMAP_WORK *fwork, u32 reg_max, HEAPID heapID );
extern void FLDEFF_CTRL_Delete( FLDEFF_CTRL *fectrl );
extern void FLDEFF_CTRL_Update( FLDEFF_CTRL *fectrl );
extern void FLDEFF_CTRL_Draw( FLDEFF_CTRL *fectrl );

//FLDEFF_CTRL �Q��
extern FIELDMAP_WORK * FLDEFF_CTRL_GetFieldMapWork( FLDEFF_CTRL *fectrl );
extern FLD_G3DOBJ_CTRL * FLDEFF_CTRL_GetFldG3dOBJCtrl( FLDEFF_CTRL *fectrl );
extern ARCHANDLE * FLDEFF_CTRL_GetArcHandleEffect( FLDEFF_CTRL *fectrl );
extern u8 FLDEFF_CTRL_GetSeasonID( FLDEFF_CTRL *fectrl );
extern BOOL FLDEFF_CTRL_GetAreaInOutSwitch( FLDEFF_CTRL *fectrl );
extern BOOL FLDEFF_CTRL_GetHasSeasonDiff( FLDEFF_CTRL *fectrl );

//FLDEFF_CTRL effect process
extern void FLDEFF_CTRL_RegistEffect(
    FLDEFF_CTRL *fectrl, const FLDEFF_PROCID *id, u32 count );
extern void FLDEFF_CTRL_DeleteEffect(
     FLDEFF_CTRL *fectrl, FLDEFF_PROCID id );
extern BOOL FLDEFF_CTRL_CheckRegistEffect(
    const FLDEFF_CTRL *fectrl, FLDEFF_PROCID id );
extern void * FLDEFF_CTRL_GetEffectWork(
    FLDEFF_CTRL *fectrl, FLDEFF_PROCID id );

//FLDEFF_CTRL task
extern FLDEFF_TASK * FLDEFF_CTRL_AddTask(
    FLDEFF_CTRL *fectrl, const FLDEFF_TASK_HEADER *head,
    const VecFx32 *pos, int add_param, const void *add_ptr, int pri );
extern void FLDEFF_CTRL_SetTaskParam( FLDEFF_CTRL *fectrl, u32 max );

//FLDEFF_TASKSYS
extern FLDEFF_TASKSYS * FLDEFF_TASKSYS_Init(
    HEAPID heapID, GFL_TCBSYS *tcbsys, int max );
extern void FLDEFF_TASKSYS_Delete( FLDEFF_TASKSYS *tasksys );
extern void FLDEFF_TASKSYS_Draw( FLDEFF_TASKSYS *tasksys );
extern FLDEFF_TASK * FLDEFF_TASKSYS_AddTask(
    FLDEFF_TASKSYS *tasksys, const FLDEFF_TASK_HEADER *head,
    const VecFx32 *pos, int add_param, const void *add_ptr, int pri );

//FLDEFF_TASK
extern void FLDEFF_TASK_CallInit( FLDEFF_TASK *task );
extern void FLDEFF_TASK_CallDelete( FLDEFF_TASK *task );
extern void FLDEFF_TASK_CallUpdate( FLDEFF_TASK *task );
extern void FLDEFF_TASK_CallDraw( FLDEFF_TASK *task );
extern u32 FLDEFF_TASK_GetAddParam( const FLDEFF_TASK *task );
extern const void * FLDEFF_TASK_GetAddPointer( const FLDEFF_TASK *task );
extern void FLDEFF_TASK_GetPos( const FLDEFF_TASK *task, VecFx32 *pos );
extern void FLDEFF_TASK_SetPos( FLDEFF_TASK *task, const VecFx32 *pos );
extern void * FLDEFF_TASK_GetWork( FLDEFF_TASK *task );
extern HEAPID FLDEFF_TASK_GetHeapID( const FLDEFF_TASK *task );

//kari
extern FLDEFF_PROCEFF_DATA DATA_FLDEFF_ProcEffectDataTbl[FLDEFF_PROCID_MAX+1];

extern const FLDEFF_PROCID DATA_FLDEFF_RegistEffectGroundTbl[];
extern const u32 DATA_FLDEFF_RegistEffectGroundTblNum;
