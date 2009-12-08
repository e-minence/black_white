//==============================================================================
/**
 * @file    battle_rec.h
 * @brief   �퓬�^��Z�[�u�f�[�^�̃w�b�_
 * @author  matsuda
 * @date    2009.09.01(��)
 */
//==============================================================================
#pragma once

#include "savedata/gds_profile.h" //GDS_PROFILE_PTR��`
#include "net_app/gds/gds_battle_rec_sub.h"

//==============================================================================
//  �萔��`
//==============================================================================
enum {
  BTLREC_OPERATION_BUFFER_SIZE = 0xc00,   ///< �S�N���C�A���g�̑�����e�ێ��o�b�t�@�T�C�Y
};



//���[�h�f�[�^�p�C���f�b�N�X
#define LOADDATA_MYREC    (0)
#define LOADDATA_DOWNLOAD0  (1)
#define LOADDATA_DOWNLOAD1  (2)
#define LOADDATA_DOWNLOAD2  (3)

///�^��f�[�^���[�h���̌��ʒl
#define RECLOAD_RESULT_NULL   (0) ///<�f�[�^�Ȃ�
#define RECLOAD_RESULT_OK   (1) ///<����ǂݍ���
#define RECLOAD_RESULT_NG   (2) ///<�f�[�^�s��
#define RECLOAD_RESULT_ERROR  (3) ///<���[�h�œǂݏo�����s(�Z�[�u�f�[�^���j�󂳂�Ă���ꍇ�Ȃ�)

#define REC_COUNTER_MAX     ( 9999 )  ///< �A���L�^�ő�l


//--------------------------------------------------------------
//  �퓬���[�h(�{�݈ꗗ)
//--------------------------------------------------------------
///�o�g���^���[ / �V���O��
#define RECMODE_TOWER_SINGLE    (GT_BATTLE_MODE_TOWER_SINGLE)
///�o�g���^���[ / �_�u��
#define RECMODE_TOWER_DOUBLE    (GT_BATTLE_MODE_TOWER_DOUBLE)
///�o�g���^���[ / WIFI�_�E�����[�h
#define RECMODE_TOWER_WIFI_DL   (GT_BATTLE_MODE_TOWER_SINGLE)

///�o�g���t�@�N�g���[ / �V���O��
#define RECMODE_FACTORY_SINGLE    (GT_BATTLE_MODE_FACTORY50_SINGLE)
///�o�g���t�@�N�g���[ / �_�u��
#define RECMODE_FACTORY_DOUBLE    (GT_BATTLE_MODE_FACTORY50_DOUBLE)

///�o�g���t�@�N�g���[ / �V���O��100
#define RECMODE_FACTORY_SINGLE100 (GT_BATTLE_MODE_FACTORY100_SINGLE)
///�o�g���t�@�N�g���[ / �_�u��100
#define RECMODE_FACTORY_DOUBLE100 (GT_BATTLE_MODE_FACTORY100_DOUBLE)

///�o�g���X�e�[�W / �V���O��
#define RECMODE_STAGE_SINGLE    (GT_BATTLE_MODE_STAGE_SINGLE)
///�o�g���X�e�[�W / �_�u��
#define RECMODE_STAGE_DOUBLE    (GT_BATTLE_MODE_STAGE_DOUBLE)

///�o�g���L���b�X�� / �V���O��
#define RECMODE_CASTLE_SINGLE   (GT_BATTLE_MODE_CASTLE_SINGLE)
///�o�g���L���b�X�� / �_�u��
#define RECMODE_CASTLE_DOUBLE   (GT_BATTLE_MODE_CASTLE_DOUBLE)

///�o�g�����[���b�g / �V���O��
#define RECMODE_ROULETTE_SINGLE   (GT_BATTLE_MODE_ROULETTE_SINGLE)
///�o�g�����[���b�g / �_�u��
#define RECMODE_ROULETTE_DOUBLE   (GT_BATTLE_MODE_ROULETTE_DOUBLE)

///�R���V�A��(�ʐM�ΐ�) / �V���O��(��������)
#define RECMODE_COLOSSEUM_SINGLE      (GT_BATTLE_MODE_COLOSSEUM_SINGLE)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(�X�^���_�[�h�J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_STANDARD (GT_BATTLE_MODE_COLOSSEUM_SINGLE_STANDARD)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(�t�@���V�[�J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_FANCY    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_FANCY)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(���g���J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_LITTLE   (GT_BATTLE_MODE_COLOSSEUM_SINGLE_LITTLE)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(���C�g�J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_LIGHT    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_LIGHT)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(�_�u���J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_DOUBLE   (GT_BATTLE_MODE_COLOSSEUM_SINGLE_DOUBLE)
///�R���V�A��(�ʐM�ΐ�) / �V���O��(���̑��̃J�b�v)
#define RECMODE_COLOSSEUM_SINGLE_ETC    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_ETC)

///�R���V�A��(�ʐM�ΐ�) / �_�u��
#define RECMODE_COLOSSEUM_DOUBLE      (GT_BATTLE_MODE_COLOSSEUM_DOUBLE)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(�X�^���_�[�h�J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_STANDARD (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_STANDARD)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(�t�@���V�[�J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_FANCY    (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_FANCY)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(���g���J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_LITTLE   (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_LITTLE)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(���C�g�J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_LIGHT    (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_LIGHT)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(�_�u���J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_DOUBLE   (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_DOUBLE)
///�R���V�A��(�ʐM�ΐ�) / �_�u��(���̑��̃J�b�v)
#define RECMODE_COLOSSEUM_DOUBLE_ETC    (GT_BATTLE_MODE_COLOSSEUM_DOUBLE_ETC)

///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(��������)
#define RECMODE_COLOSSEUM_MIX       (GT_BATTLE_MODE_COLOSSEUM_SINGLE)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(�X�^���_�[�h�J�b�v)
#define RECMODE_COLOSSEUM_MIX_STANDARD    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_STANDARD)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(�t�@���V�[�J�b�v)
#define RECMODE_COLOSSEUM_MIX_FANCY     (GT_BATTLE_MODE_COLOSSEUM_SINGLE_FANCY)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(���g���J�b�v)
#define RECMODE_COLOSSEUM_MIX_LITTLE    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_LITTLE)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(���C�g�J�b�v)
#define RECMODE_COLOSSEUM_MIX_LIGHT     (GT_BATTLE_MODE_COLOSSEUM_SINGLE_LIGHT)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(�_�u���J�b�v)
#define RECMODE_COLOSSEUM_MIX_DOUBLE    (GT_BATTLE_MODE_COLOSSEUM_SINGLE_DOUBLE)
///�R���V�A��(�ʐM�ΐ�) / �~�b�N�X(���̑��̃J�b�v)
#define RECMODE_COLOSSEUM_MIX_ETC     (GT_BATTLE_MODE_COLOSSEUM_SINGLE_ETC)

//�ȉ��A�}���`���[�h
#define RECMODE_TOWER_MULTI     (GT_BATTLE_MODE_TOWER_MULTI)  ///<�o�g���^���[ / �}���`
#define RECMODE_FACTORY_MULTI   (GT_BATTLE_MODE_FACTORY50_MULTI)  ///<�o�g���t�@�N�g���[ / �}���`
#define RECMODE_FACTORY_MULTI100  (GT_BATTLE_MODE_FACTORY100_MULTI)///<�o�g���t�@�N�g���[ / �}���`100
#define RECMODE_STAGE_MULTI     (GT_BATTLE_MODE_STAGE_MULTI)  ///<�o�g���X�e�[�W / �}���`
#define RECMODE_CASTLE_MULTI    (GT_BATTLE_MODE_CASTLE_MULTI) ///<�o�g���L���b�X�� / �}���`
#define RECMODE_ROULETTE_MULTI    (GT_BATTLE_MODE_ROULETTE_MULTI) ///<�o�g�����[���b�g / �}���`
#define RECMODE_COLOSSEUM_MULTI   (GT_BATTLE_MODE_COLOSSEUM_MULTI)///<�R���V�A��(�ʐM�ΐ�) / �}���`

//�I�[
#define RECMODE_MAX           (GT_BATTLE_MODE_ROULETTE_MULTI + 1)


//--------------------------------------------------------------
//  �^��w�b�_�̃p�����[�^�擾�vINDEX
//--------------------------------------------------------------
#define RECHEAD_IDX_MONSNO      (0)   ///<�|�P�����ԍ�
#define RECHEAD_IDX_FORM_NO     (1)   ///<�t�H�����ԍ�
#define RECHEAD_IDX_COUNTER     (2)   ///<����ڂ�
#define RECHEAD_IDX_MODE      (3)   ///<�^��{��
#define RECHEAD_IDX_DATA_NUMBER   (4)   ///<�f�[�^�i���o�[
#define RECHEAD_IDX_SECURE      (5)   ///<���S�����ς�



//--------------------------------------------------------------
/**
 *  �퓬�^��Z�[�u�f�[�^�̕s���S�^�\���̐錾
 */
//--------------------------------------------------------------
typedef struct _BATTLE_REC_SAVEDATA   BATTLE_REC_SAVEDATA;

//--------------------------------------------------------------
/**
 *  �퓬�^��{�̂̕s���S�^�\���̐錾
 */
//--------------------------------------------------------------
typedef struct _BATTLE_REC_WORK *   BATTLE_REC_WORK_PTR;

//--------------------------------------------------------------
/**
 *  �퓬�^��w�b�_�̕s���S�^�\���̐錾
 */
//--------------------------------------------------------------
typedef struct _BATTLE_REC_HEADER *   BATTLE_REC_HEADER_PTR;



//==============================================================================
//  ��check�@make��ʂ��ׂ̎b���`
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   �퓬�J�n�p�����[�^
 *      336 byte
 */
//--------------------------------------------------------------
typedef struct{
  u8 work[336];
}BATTLE_PARAM;

//----------------------------------------------------------
/**
 * @brief �o�g�����M�����[�V�����f�[�^�^��`  fushigi_data.h�Q�Ƃ̈׊O�����J��
 */
//----------------------------------------------------------
typedef struct {
  STRCODE cupName[12];
  u16 totalLevel;
  u8 num;
  u8 level;
    s8 height;
    s8 weight;
    u8 evolution:1;    //
    u8 bLegend:1;
    u8 bBothMonster:1;
    u8 bBothItem:1;
    u8 bFixDamage:1;
}AAA_REGULATION;

//----------------------------------------------------------------------------
/**
  *  �s���S�^��`
 */
//----------------------------------------------------------------------------
typedef struct _BTLREC_OPERATION_BUFFER   BTLREC_OPERATION_BUFFER;  ///< �N���C�A���g������e�ێ��o�b�t�@
typedef struct _BTLREC_SETUP_SUBSET       BTLREC_SETUP_SUBSET;      ///< �o�g���Z�b�g�A�b�v�p�����[�^�̃T�u�Z�b�g


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern u32 BattleRec_GetWorkSize( void );
extern void BattleRec_WorkInit(void *rec);
extern void BattleRec_Init(HEAPID heapID);
extern void BattleRec_Exit(void);
extern BOOL BattleRec_DataExistCheck(void);
extern void * BattleRec_RecWorkAdrsGet( void );
extern BOOL BattleRec_Load( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num );
extern BOOL BattleRec_DataOccCheck(SAVE_CONTROL_WORK *sv,HEAPID heapID,LOAD_RESULT *result,int num);
extern SAVE_RESULT BattleRec_Save(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int rec_mode, int fight_count, int num, u16 *work0, u16 *work1);
extern SAVE_RESULT BattleRec_SaveDataErase(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int num);
extern void BattleRec_ClientTemotiGet(int rec_mode, int *client_max, int *temoti_max);
extern void BattleRec_BattleParamRec(BATTLE_PARAM *bp);
extern void BattleRec_ServerVersionUpdate(int id_no, u32 server_version);
extern BOOL BattleRec_ServerVersionCheck(void);
extern void BattleRec_BattleParamCreate(BATTLE_PARAM *bp,SAVE_CONTROL_WORK *sv);
extern BATTLE_REC_HEADER_PTR BattleRec_HeaderAllocCopy(HEAPID heap_id);
extern GDS_PROFILE_PTR BattleRec_GDSProfileAllocCopy(HEAPID heap_id);
extern GDS_PROFILE_PTR BattleRec_GDSProfilePtrGet(void);
extern BATTLE_REC_WORK_PTR BattleRec_WorkPtrGet(void);
extern BATTLE_REC_HEADER_PTR BattleRec_HeaderPtrGet(void);
extern BTLREC_OPERATION_BUFFER* BattleRec_GetOperationBufferPtr( void );
extern BTLREC_SETUP_SUBSET*     BattleRec_GetSetupSubsetPtr( void );

extern void BattleRec_DataSet(GDS_PROFILE_PTR gpp, BATTLE_REC_HEADER_PTR head, BATTLE_REC_WORK_PTR rec, BATTLE_PARAM *bp, SAVE_CONTROL_WORK *sv);
extern u64 RecHeader_ParamGet(BATTLE_REC_HEADER_PTR head, int index, int param);
extern BATTLE_REC_HEADER_PTR BattleRec_Header_AllocMemory(HEAPID heap_id);
extern void BattleRec_Header_FreeMemory(BATTLE_REC_HEADER_PTR brhp);

//==============================================================================
//  �ꕔ�֐���battle_rec_ov.c�Ɉړ������ׁA�O�����J����K�v�ɂȂ�������
//==============================================================================
extern SAVE_RESULT Local_BattleRecSave(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *work, int num, u16 *seq, HEAPID heap_id);
extern void BattleRec_Coded(void *data,u32 size,u32 code);

//==============================================================================
//  battl_rec_ov.c
//      ��battle_recorder�I�[�o�[���C�̈�ɔz�u����Ă���̂Œ���!
//==============================================================================
extern void BattleRecTool_ErrorStrCheck(BATTLE_REC_SAVEDATA *src, BATTLE_PARAM *bp, int heap_id);
extern SAVE_RESULT BattleRec_GDS_RecvData_Save(SAVE_CONTROL_WORK *sv, int num, u8 secure, u16 *work0, u16 *work1);
extern void BattleRec_GDS_SendData_Conv(SAVE_CONTROL_WORK *sv);
extern SAVE_RESULT BattleRec_GDS_MySendData_DataNumberSetSave(SAVE_CONTROL_WORK *sv, u64 data_number, u16 *work0, u16 *work1);
extern SAVE_RESULT BattleRec_SecureSetSave(SAVE_CONTROL_WORK *sv, int num);

#ifdef PM_DEBUG
extern void DEBUG_BattleRec_SecureFlagSet(SAVE_CONTROL_WORK *sv);
#endif

