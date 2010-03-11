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
#include "battle/battle.h"

//==============================================================================
//  �萔��`
//==============================================================================
enum {
  BTLREC_OPERATION_BUFFER_SIZE = 0xc00,   ///< �S�N���C�A���g�̑�����e�ێ��o�b�t�@�T�C�Y
};

///�^��w�b�_�Ɋi�[����|�P�����ԍ��̍ő�v�f��
#define HEADER_MONSNO_MAX   (12)

#define HEADER_FORM_NO_MASK ( 0x7f )      ///<�t�H�����i���o�[�����o�����߂̃}�X�N�l
#define HEADER_GENDER_MASK  ( 0x80 )      ///<���ʂ����o�����߂̃}�X�N�l
#define HEADER_GENDER_SHIFT ( 7 )         ///<���ʂ����o�����߂̃V�t�g�l

///�f�[�^�i���o�[�̌^
typedef u64 DATA_NUMBER;

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
///�o�g�����[�h
typedef enum{
  //�R���V�A���F�V���O��
  BATTLE_MODE_COLOSSEUM_SINGLE_FREE,
  BATTLE_MODE_COLOSSEUM_SINGLE_50,
  BATTLE_MODE_COLOSSEUM_SINGLE_FREE_SHOOTER,
  BATTLE_MODE_COLOSSEUM_SINGLE_50_SHOOTER,
  //�R���V�A���F�_�u��
  BATTLE_MODE_COLOSSEUM_DOUBLE_FREE,
  BATTLE_MODE_COLOSSEUM_DOUBLE_50,
  BATTLE_MODE_COLOSSEUM_DOUBLE_FREE_SHOOTER,
  BATTLE_MODE_COLOSSEUM_DOUBLE_50_SHOOTER,
  //�R���V�A���F�g���v��
  BATTLE_MODE_COLOSSEUM_TRIPLE_FREE,
  BATTLE_MODE_COLOSSEUM_TRIPLE_50,
  BATTLE_MODE_COLOSSEUM_TRIPLE_FREE_SHOOTER,
  BATTLE_MODE_COLOSSEUM_TRIPLE_50_SHOOTER,
  //�R���V�A���F���[�e�[�V����
  BATTLE_MODE_COLOSSEUM_ROTATION_FREE,
  BATTLE_MODE_COLOSSEUM_ROTATION_50,
  BATTLE_MODE_COLOSSEUM_ROTATION_FREE_SHOOTER,
  BATTLE_MODE_COLOSSEUM_ROTATION_50_SHOOTER,
  //�R���V�A���F�}���`
  BATTLE_MODE_COLOSSEUM_MULTI_FREE,
  BATTLE_MODE_COLOSSEUM_MULTI_50,
  BATTLE_MODE_COLOSSEUM_MULTI_FREE_SHOOTER,
  BATTLE_MODE_COLOSSEUM_MULTI_50_SHOOTER,
  //�n���S
  BATTLE_MODE_SUBWAY_SINGLE,     //WIFI DL�܂�
  BATTLE_MODE_SUBWAY_DOUBLE,
  BATTLE_MODE_SUBWAY_MULTI,      //NPC, COMM, WIFI
  //�����_���}�b�`�F�t���[
  BATTLE_MODE_RANDOM_FREE_SINGLE,
  BATTLE_MODE_RANDOM_FREE_DOUBLE,
  BATTLE_MODE_RANDOM_FREE_TRIPLE,
  BATTLE_MODE_RANDOM_FREE_ROTATION,
  BATTLE_MODE_RANDOM_FREE_SHOOTER,
  //�����_���}�b�`�F���[�e�B���O
  BATTLE_MODE_RANDOM_RATING_SINGLE,
  BATTLE_MODE_RANDOM_RATING_DOUBLE,
  BATTLE_MODE_RANDOM_RATING_TRIPLE,
  BATTLE_MODE_RANDOM_RATING_ROTATION,
  BATTLE_MODE_RANDOM_RATING_SHOOTER,
  //���
  BATTLE_MODE_COMPETITION_SINGLE,
  BATTLE_MODE_COMPETITION_DOUBLE,
  BATTLE_MODE_COMPETITION_TRIPLE,
  BATTLE_MODE_COMPETITION_ROTATION,
  BATTLE_MODE_COMPETITION_SINGLE_SHOOTER,
  BATTLE_MODE_COMPETITION_DOUBLE_SHOOTER,
  BATTLE_MODE_COMPETITION_TRIPLE_SHOOTER,
  BATTLE_MODE_COMPETITION_ROTATION_SHOOTER,

  //--------------------------------------------------------------
  //  ��������
  //--------------------------------------------------------------
  BATTLE_MODE_COLOSSEUM_SINGLE_NO_SHOOTER = 0xef,    //�R���V�A��/�V���O���@�V���[�^�[����
  BATTLE_MODE_COLOSSEUM_SINGLE_SHOOTER = 0xf0,       //�R���V�A��/�V���O���@�V���[�^�[�L
  BATTLE_MODE_COLOSSEUM_DOUBLE_NO_SHOOTER = 0xf1,    //�R���V�A��/�_�u���@�V���[�^�[����
  BATTLE_MODE_COLOSSEUM_DOUBLE_SHOOTER = 0xf2,       //�R���V�A��/�_�u���@�V���[�^�[�L
  BATTLE_MODE_COLOSSEUM_TRIPLE_NO_SHOOTER = 0xf3,    //�R���V�A��/�g���v���@�V���[�^�[����
  BATTLE_MODE_COLOSSEUM_TRIPLE_SHOOTER = 0xf4,       //�R���V�A��/�g���v���@�V���[�^�[�L
  BATTLE_MODE_COLOSSEUM_ROTATION_NO_SHOOTER = 0xf5,  //�R���V�A��/���[�e�[�V�����@�V���[�^�[����
  BATTLE_MODE_COLOSSEUM_ROTATION_SHOOTER = 0xf6,     //�R���V�A��/���[�e�[�V�����@�V���[�^�[�L
  BATTLE_MODE_COLOSSEUM_MULTI_NO_SHOOTER = 0xf7,     //�R���V�A��/���[�e�[�V�����@�V���[�^�[����
  BATTLE_MODE_COLOSSEUM_MULTI_SHOOTER = 0xf8,        //�R���V�A��/���[�e�[�V�����@�V���[�^�[�L
  BATTLE_MODE_RANDOM_SINGLE = 0xf9,                  //�����_���}�b�`/�V���O��
  BATTLE_MODE_RANDOM_DOUBLE = 0xfa,                  //�����_���}�b�`/�_�u��
  BATTLE_MODE_RANDOM_TRIPLE = 0xfb,                  //�����_���}�b�`/�g���v��
  BATTLE_MODE_RANDOM_ROTATION = 0xfc,                //�����_���}�b�`/���[�e�[�V����
  BATTLE_MODE_RANDOM_SHOOTER = 0xfd,                 //�����_���}�b�`/�V���[�^�[
  BATTLE_MODE_COMPETITION = 0xfe,                    //�o�g�����
  BATTLE_MODE_NONE = 0xff,                           //�w��Ȃ�
}BATTLE_MODE;

//�I�[
#define BATTLE_MODE_MAX           (BATTLE_MODE_COMPETITION_ROTATION_SHOOTER + 1)


//--------------------------------------------------------------
//  �^��w�b�_�̃p�����[�^�擾�vINDEX
//--------------------------------------------------------------
#define RECHEAD_IDX_MONSNO      (0)   ///<�|�P�����ԍ�
#define RECHEAD_IDX_FORM_NO     (1)   ///<�t�H�����ԍ�
#define RECHEAD_IDX_COUNTER     (2)   ///<����ڂ�
#define RECHEAD_IDX_MODE      (3)   ///<�^��{��
#define RECHEAD_IDX_DATA_NUMBER   (4)   ///<�f�[�^�i���o�[
#define RECHEAD_IDX_SECURE      (5)   ///<���S�����ς�
#define RECHEAD_IDX_GENDER      (6)   ///<����



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
extern BOOL BattleRec_DataSetCheck(void);
extern void BattleRec_DataClear(void);
extern void * BattleRec_RecWorkAdrsGet( void );
extern BOOL BattleRec_Load( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num );
extern BOOL BattleRec_DataOccCheck(SAVE_CONTROL_WORK *sv,HEAPID heapID,LOAD_RESULT *result,int num);
extern SAVE_RESULT BattleRec_Save(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int rec_mode, int fight_count, int num, u16 *work0, u16 *work1);
extern SAVE_RESULT BattleRec_SaveDataErase(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int num);
extern void BattleRec_ClientTemotiGet(BATTLE_MODE rec_mode, int *client_max, int *temoti_max);
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

extern void BattleRec_ExitWork(BATTLE_REC_SAVEDATA *wk_brs);
extern BATTLE_REC_SAVEDATA * BattleRec_LoadAlloc( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num );
extern BATTLE_REC_HEADER_PTR BattleRec_HeaderPtrGetWork(BATTLE_REC_SAVEDATA *wk_brs);

extern void BattleRec_DataSet(GDS_PROFILE_PTR gpp, BATTLE_REC_HEADER_PTR head, BATTLE_REC_WORK_PTR rec, BATTLE_PARAM *bp, SAVE_CONTROL_WORK *sv);
extern u64 RecHeader_ParamGet(BATTLE_REC_HEADER_PTR head, int index, int param);
extern BATTLE_REC_HEADER_PTR BattleRec_Header_AllocMemory(HEAPID heap_id);
extern void BattleRec_Header_FreeMemory(BATTLE_REC_HEADER_PTR brhp);


extern void BattleRec_LoadToolModule( void );
extern void BattleRec_UnloadToolModule( void );
extern void BattleRec_StoreSetupParam( const BATTLE_SETUP_PARAM* setup );
extern void BattleRec_RestoreSetupParam( BATTLE_SETUP_PARAM* setup, HEAPID heapID );

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

