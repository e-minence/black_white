//=============================================================================
/**
 * @file	savedata.h
 * @brief	�Z�[�u�f�[�^�p�w�b�_
 * @author	tamada	GAME FREAK Inc.
 * @date	2005.10.12
 *
 * ���̃w�b�_�̓Z�[�u�f�[�^�̐��������ȂǁA���ڃZ�[�u�f�[�^�ɂ�����镔���ɂ̂ݕK�v�B
 * ����ȊO�̉ӏ��ł�savedata_def.h��SAVEDATA�^�o�R�ŃA�N�Z�X�ł���΂悢�B
 * ���̂��߁A���̃w�b�_���C���N���[�h����ӏ��͌��肳���͂�
 */
//=============================================================================
#pragma	once

#include "savedata_def.h"

///---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef u32 (*FUNC_GET_SIZE)(void);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
typedef void (*FUNC_INIT_WORK)(void *);

typedef u32 DATA_ID;

//---------------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^���e�f�[�^�p�̍\���̒�`
 */
//---------------------------------------------------------------------------
typedef struct {
	DATA_ID gmdataID;				///<�Z�[�u�f�[�^����ID
	FUNC_GET_SIZE get_size;			///<�Z�[�u�f�[�^�T�C�Y�擾�֐�
	FUNC_INIT_WORK	init_work;		///<�Z�[�u�f�[�^�������֐�
}SAVEDATA_TABLE;

//---------------------------------------------------------------------------
///	�Z�[�u�f�[�^��`�\����
//---------------------------------------------------------------------------
typedef struct {
	const SAVEDATA_TABLE * table;
	u32 table_max;
	u32 savearea_top_address;
	u32 savearea_size;
	u32 magic_number;
}GFL_SVLD_PARAM;



//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
//	�Z�[�u�f�[�^����֐�
//---------------------------------------------------------------------------
//�V�X�e���N��
extern void GFL_BACKUP_Init(u32 heap_save_id, u32 heap_temp_id);
//�V�X�e���I��
extern void GFL_BACKUP_Exit(void);
//�t���b�V�����݃`�F�b�N
extern BOOL GFL_BACKUP_IsEnableFlash(const SAVEDATA * sv);

//�Z�[�u���[�h�V�X�e��������
extern SAVEDATA * SaveData_System_Init(u32 heap_save_id, u32 heap_temp_id);

extern SAVEDATA * GFL_SVLD_Create(const GFL_SVLD_PARAM * sv_param);
extern void GFL_SVLD_Delete(SAVEDATA * sv);

//���[�h����
extern BOOL SaveData_Load(SAVEDATA * sv);
//�Z�[�u����
extern SAVE_RESULT SaveData_Save(SAVEDATA * sv);
//��������
extern BOOL SaveData_Erase(SAVEDATA * sv);

//�Z�[�u�f�[�^�������i�������܂Ȃ��j
extern void SaveData_ClearData(SAVEDATA * sv);

//�ŏ��̓ǂݍ��ݎ��̃Z�[�u�f�[�^��Ԏ擾
extern LOAD_RESULT SaveData_GetLoadResult(const SAVEDATA * sv);

//�f�[�^�����݂��邩�ǂ����̔��ʃt���O�擾
extern BOOL SaveData_GetExistFlag(const SAVEDATA * sv);

//�����Z�[�u�F������
extern void SaveData_DivSave_Init(SAVEDATA * sv);
//�����Z�[�u�F���C��
extern SAVE_RESULT SaveData_DivSave_Main(SAVEDATA * sv);
//�����Z�[�u�F�L�����Z��
extern void SaveData_DivSave_Cancel(SAVEDATA * sv);

//�Z�[�u�f�[�^�����߂��i�f�o�b�O�p�j
extern void SaveData_Rewind(SAVEDATA * sv);

//---------------------------------------------------------------------------
//	�Z�[�u�f�[�^�A�N�Z�X�p�֐�
//---------------------------------------------------------------------------
extern void * SaveData_Get(SAVEDATA * sv, DATA_ID gmdataID);
extern const void * SaveData_GetReadOnlyData(const SAVEDATA * sv, DATA_ID gmdataID);


// �C���f�b�N�X����鎞�̂ݗL��
// �ʏ��OFF�Ȃ̂�ON�ɂȂ��Ă�̂��������疳������OFF�ŗǂ�
//#define CREATE_INDEX

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


