//==============================================================================
/**
 * @file    battle_rec.c
 * @brief   �퓬�^��Z�[�u
 * @author  matsuda
 * @date    2009.09.01(��)
 */
//==============================================================================
#include <gflib.h>


#include "system/main.h"
#include "savedata/save_tbl.h"
#include "savedata/mystatus.h"
#include "savedata/config.h"
#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "savedata/battle_rec.h"
#include "battle/battle.h"

#include "../battle/btl_common.h"
#include "../battle/btl_net.h"
#include "../poke_tool/poke_tool_def.h"

#include "mystatus_local.h"
#include "net_app/gds/gds_profile_local.h"

#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "savedata/gds_profile.h"
#include "battle/btl_common.h"
#include "savedata\battle_rec.h"
#include "net_app/gds/gds_profile_local.h"
#include "battle_rec_local.h"
#include "net_app/gds/gds_battle_mode.h"

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* debug                                                                    */
/*--------------------------------------------------------------------------*/
#ifdef PM_DEBUG

#define DEBUG_BATTLE_REC_PRINT_ON //�S���҂݂̂̃v�����gON

#endif //PM_DEBUG


//�S���҂݂̂̃v�����g
#ifdef DEBUG_BATTLE_REC_PRINT_ON

#if defined( DEBUG_ONLY_FOR_matsuda ) 
#define BATTLE_REC_Printf(...) OS_TPrintf(__VA_ARGS__)
#elif  defined( DEBUG_ONLY_FOR_shimoyamada )
#define BATTLE_REC_Printf(...) OS_TPrintf(__VA_ARGS__)
#elif defined( DEBUG_ONLY_FOR_toru_nagihashi )
#define BATTLE_REC_Printf(...) OS_TFPrintf(0,__VA_ARGS__)
#endif //defined
#endif //DEBUG_BATTLE_REC_PRINT_ON

#ifndef BATTLE_REC_Printf
#define BATTLE_REC_Printf(...)  /*  */
#endif //BATTLE_REC_Printf

/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
BATTLE_REC_SAVEDATA * brs=NULL;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void RecHeaderCreate(SAVE_CONTROL_WORK *sv, BATTLE_REC_HEADER *head, const BATTLE_REC_WORK_PTR rec, int rec_mode, int counter);
static BOOL BattleRec_DataInitializeCheck(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *src);
static  BOOL BattleRecordCheckData(SAVE_CONTROL_WORK *sv, const BATTLE_REC_SAVEDATA * src);
static  void  BattleRec_Decoded(void *data,u32 size,u32 code);



//==============================================================================
//  �f�[�^
//==============================================================================
#include "battle_rec_mode.cdat"
SDK_COMPILER_ASSERT(BATTLE_MODE_MAX == NELEMS(BattleRecModeBitTbl));


//--------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�p���[�N��Alloc����
 *
 * @param   heapID
 *
 * @retval  BATTLE_REC_SAVEDATA *
 */
//--------------------------------------------------------------
static BATTLE_REC_SAVEDATA * _BattleRecSaveAlloc(HEAPID heapID)
{
  BATTLE_REC_SAVEDATA *new_brs;

  new_brs = GFL_HEAP_AllocClearMemory(heapID, SAVESIZE_EXTRA_BATTLE_REC);//sizeof(BATTLE_REC_SAVEDATA));
  BattleRec_WorkInit(new_brs);
  return new_brs;
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^���쐬����
 *
 * @param   heapID  �������m�ۂ��邽�߂̃q�[�vID
 */
//------------------------------------------------------------------
void BattleRec_Init(HEAPID heapID)
{
  BATTLE_REC_Printf("BATTLE_REC_WORK size = %d\n", sizeof(BATTLE_REC_WORK));
  BATTLE_REC_Printf("BATTLE_REC_HEADER size = %d\n", sizeof(BATTLE_REC_HEADER));

  if(brs != NULL){
    GFL_HEAP_FreeMemory(brs);
    brs = NULL;
  }

  brs = _BattleRecSaveAlloc(heapID);
}
//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̔j��
 */
//------------------------------------------------------------------
void BattleRec_Exit(void)
{
  GF_ASSERT(brs);
  GFL_HEAP_FreeMemory(brs);
  brs = NULL;
}
//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̔j��(���[�N�w��)
 *
 * BattleRec_LoadAlloc�Ŋm�ۂ������[�N�̉���p
 */
//------------------------------------------------------------------
void BattleRec_ExitWork(BATTLE_REC_SAVEDATA *wk_brs)
{
  GFL_HEAP_FreeMemory(wk_brs);
}

//------------------------------------------------------------------
/**
 * �Z�[�u�f�[�^�T�C�Y��Ԃ�
 *
 * @retval  int
 */
//------------------------------------------------------------------
u32 BattleRec_GetWorkSize( void )
{
  //�Z�N�^�[2�y�[�W�������T�C�Y���傫���Ȃ�����ExtraSaveDataTable�̊e�^��f�[�^��
  //�y�[�W�I�t�Z�b�g��+3�P�ʂɒ���
  GF_ASSERT(sizeof(BATTLE_REC_SAVEDATA) < SAVE_SECTOR_SIZE * 2);

  return sizeof(BATTLE_REC_SAVEDATA);
}

//--------------------------------------------------------------
/**
 * @brief   ���[�N������
 *
 * @param   rec
 */
//--------------------------------------------------------------
void BattleRec_WorkInit(void *rec)
{
  GFL_STD_MemClear32( rec, sizeof(BATTLE_REC_SAVEDATA) );
}
//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃��������m�ۂ��Ă��邩�`�F�b�N
 *
 * @retval  TRUE:���݂���@FALSE:���݂��Ȃ�
 */
//------------------------------------------------------------------
BOOL BattleRec_DataExistCheck(void)
{
  return (brs!=NULL);
}
//----------------------------------------------------------------------------
/**
 *  @brief  �ΐ�^��f�[�^�̃�������ɘ^��f�[�^���Z�b�g����Ă��邩�`�F�b�N
 *
 *  @return TRUE�^��f�[�^���Z�b�g����Ă���@FALSE�^��f�[�^���Z�b�g����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL BattleRec_DataSetCheck(void)
{
  BATTLE_REC_WORK *rec = &brs->rec;

  //�����������܂�Ă��Ȃ����FALSE
  if( GFL_STD_CODED_CheckSum( rec, sizeof(BATTLE_REC_WORK) ) > 0 )
  {
    return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �ΐ�^��f�[�^�̃�������ɘ^��f�[�^���N���A����
 *
 */
//-----------------------------------------------------------------------------
void BattleRec_DataClear(void)
{
  BattleRec_WorkInit(brs);
}
//----------------------------------------------------------------------------
/**
 *  @brief  ��������̑ΐ�^��f�[�^�𕜍�������
 *          BattleRec_Save�̌�A���̂܂�BRS���g���ꍇ�Ɏg�p���Ă�������
 *
 */
//-----------------------------------------------------------------------------
void BattleRec_DataDecoded(void)
{
  GF_ASSERT(brs != NULL);

  BattleRec_Decoded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
}

void BattleRec_DataCoded(void)
{
  GF_ASSERT(brs != NULL);

  BattleRec_Coded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
      brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
}


//--------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃��[�h
 *
 * @param wk_brs    �^��f�[�^�W�J��
 * @param heapID    �f�[�^�����[�h���郁�������m�ۂ��邽�߂̃q�[�vID
 * @param result    ���[�h���ʂ��i�[���郏�[�NRECLOAD_RESULT_NULL,OK,NG,ERROR
 * @param bp    ���[�h�����f�[�^���琶������BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param num   ���[�h����f�[�^�i���o�[(LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c)
 *
 * @retval  TRUE
 */
//--------------------------------------------------------------
static BOOL _BattleRec_LoadCommon(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *wk_brs, HEAPID heapID, LOAD_RESULT *result, int num)
{
  BATTLE_REC_WORK *rec;
  BATTLE_REC_HEADER *head;

  //�f�[�^��wk_brs�Ƀ��[�h
  *result = SaveControl_Extra_LoadWork(
    sv, SAVE_EXTRA_ID_REC_MINE + num, heapID, wk_brs, SAVESIZE_EXTRA_BATTLE_REC);

  //wk_brs�ɓW�J���ꂽ�̂ŃZ�[�u�V�X�e���͔j��
  SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);

  if(*result == LOAD_RESULT_NULL){
    BATTLE_REC_Printf("�^��f�[�^�����݂��܂���\n");
    *result = RECLOAD_RESULT_NULL;  //�������f�[�^�ׁ̈A�f�[�^�Ȃ�
    return TRUE;
  }
  else if(*result != LOAD_RESULT_OK){
    *result = RECLOAD_RESULT_ERROR;
    return TRUE;
  }

  rec = &wk_brs->rec;
  head = &wk_brs->head;

  //����
  BattleRec_Decoded(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    rec->crc.crc16ccitt_hash + ((rec->crc.crc16ccitt_hash ^ 0xffff) << 16));

  //�ǂݏo�����f�[�^�ɘ^��f�[�^�������Ă��邩�`�F�b�N
  if(BattleRec_DataInitializeCheck(sv, wk_brs) == TRUE){
    BATTLE_REC_Printf("�^��f�[�^��������Ԃ̂��̂ł�\n");
    *result = RECLOAD_RESULT_NULL;  //�������f�[�^�ׁ̈A�f�[�^�Ȃ�
    return TRUE;
  }

  //�f�[�^�̐������`�F�b�N
  if(BattleRecordCheckData(sv, wk_brs) == FALSE){
    BATTLE_REC_Printf("�s���Ș^��f�[�^\n");
    *result = RECLOAD_RESULT_NG;
    return TRUE;
  }

  #if 0
  //�ǂݏo���f�[�^��BATTLE_PARAM�ɃZ�b�g
  if(bp){
    BattleRec_BattleParamCreate(bp,sv);
  }
  #endif

  *result = RECLOAD_RESULT_OK;
  return TRUE;
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃��[�h
 *
 * @param sv    �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heapID  �f�[�^�����[�h���郁�������m�ۂ��邽�߂̃q�[�vID
 * @param result  ���[�h���ʂ��i�[���郏�[�NRECLOAD_RESULT_NULL,OK,NG,ERROR
 * @param bp    ���[�h�����f�[�^���琶������BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param num   ���[�h����f�[�^�i���o�[(LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c)
 *
 * @retval  TRUE
 */
//------------------------------------------------------------------
BOOL BattleRec_Load( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num )
{
  //���łɓǂݍ��܂�Ă���f�[�^������Ȃ�A�j������
  if(brs){
    BattleRec_WorkInit(brs);
  }
  else{
    BattleRec_Init(heapID);
  }

  return _BattleRec_LoadCommon(sv, brs, heapID, result, num);
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃��[�h(�ǂݍ��ݗp�̃��[�N��Alloc���ă��[�h�B�@���d�ǂݍ��ݗp)
 *
 * @param sv    �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heapID  �f�[�^�����[�h���郁�������m�ۂ��邽�߂̃q�[�vID
 * @param result  ���[�h���ʂ��i�[���郏�[�NRECLOAD_RESULT_NULL,OK,NG,ERROR
 * @param bp    ���[�h�����f�[�^���琶������BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param num   ���[�h����f�[�^�i���o�[(LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c)
 *
 * @retval  �^��f�[�^���[�N(���[�h�Ɏ��s���Ă����[�N��Alloc���Ă��܂��B
 *                           �K��BattleRec_ExitWork���g�p���ĉ�����Ă�������)
 */
//------------------------------------------------------------------
BATTLE_REC_SAVEDATA * BattleRec_LoadAlloc( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num )
{
  BATTLE_REC_SAVEDATA *new_brs;

  new_brs = _BattleRecSaveAlloc(heapID);
  _BattleRec_LoadCommon(sv, new_brs, heapID, result, num);
  return new_brs;
}

//------------------------------------------------------------------
/**
 * ���ɑΐ�^��f�[�^�����݂��Ă��邩���ׂ�(BattleRec_Load�֐�����f�[�^�`�F�b�N�̂ݔ����o��������)
 *
 * @param sv    �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heapID  �f�[�^�����[�h���郁�������m�ۂ��邽�߂̃q�[�vID
 * @param result  ���[�h���ʂ��i�[���郏�[�NRECLOAD_RESULT_NULL,OK,NG,ERROR
 * @param bp    ���[�h�����f�[�^���琶������BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param num   ���[�h����f�[�^�i���o�[�iLOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c�j
 *
 * @retval  TRUE:����Ș^��f�[�^�����݂��Ă���
 * @retval  FALSE:����Ș^��f�[�^�͑��݂��Ă��Ȃ�
 */
//------------------------------------------------------------------
BOOL BattleRec_DataOccCheck(SAVE_CONTROL_WORK *sv,HEAPID heapID,LOAD_RESULT *result,int num)
{
  BATTLE_REC_WORK *rec;
  BATTLE_REC_HEADER *head;
  BATTLE_REC_SAVEDATA *all;

  *result = SaveControl_Extra_Load(sv, SAVE_EXTRA_ID_REC_MINE + num, heapID);
  all = SaveControl_Extra_DataPtrGet(sv, SAVE_EXTRA_ID_REC_MINE + num, 0);
  if(*result == LOAD_RESULT_NULL){
    *result = RECLOAD_RESULT_NULL;  //�������f�[�^�ׁ̈A�f�[�^�Ȃ�
    SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
    return FALSE;
  }
  else if(*result != LOAD_RESULT_OK){
    *result = RECLOAD_RESULT_ERROR;
    SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
    return FALSE;
  }

  rec = &all->rec;
  head = &all->head;

  //����
  BattleRec_Decoded(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    rec->crc.crc16ccitt_hash + ((rec->crc.crc16ccitt_hash ^ 0xffff) << 16));

  //�ǂݏo�����f�[�^�ɘ^��f�[�^�������Ă��邩�`�F�b�N
  if(BattleRec_DataInitializeCheck(sv, all) == TRUE){
    *result = RECLOAD_RESULT_NULL;  //�������f�[�^�ׁ̈A�f�[�^�Ȃ�
    SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
    return FALSE;
  }

  //�f�[�^�̐������`�F�b�N
  if(BattleRecordCheckData(sv, all) == FALSE){
    BATTLE_REC_Printf("�s���Ș^��f�[�^\n");
    *result = RECLOAD_RESULT_NG;
    SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
    return FALSE;
  }

  *result = RECLOAD_RESULT_OK;
  SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �ΐ�^��̃Z�[�u�������܂Ƃ߂�����
 *
 * @param   sv    �Z�[�u�f�[�^�ւ̃|�C���^
 * @param   num   LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c
 * @param   seq   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   heap_id �Z�[�u�V�X�e���쐬�Ɉꎞ�I�Ɏg�p����q�[�vID
 *
 * @retval  SAVE_RESULT_CONTINUE  �Z�[�u�p����
 * @retval  SAVE_RESULT_LAST    �Z�[�u�p�����A�Ō�̕���
 * @retval  SAVE_RESULT_OK      �Z�[�u�I���A����
 * @retval  SAVE_RESULT_NG      �Z�[�u�I���A���s
 */
//--------------------------------------------------------------
SAVE_RESULT Local_BattleRecSave(GAMEDATA *gamedata, BATTLE_REC_SAVEDATA *work, int num, u16 *seq, HEAPID heap_id)
{
  SAVE_RESULT result;
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);

  switch(*seq){
  case 0:
    //�Z�[�u�Ώۂ̊O���Z�[�u�̈�̃Z�[�u�V�X�e�����쐬(�Z�[�u���[�N�̎��̂�brs��n��)
    SaveControl_Extra_SystemSetup(sv, SAVE_EXTRA_ID_REC_MINE + num, heap_id, brs, SAVESIZE_EXTRA_BATTLE_REC);

    GAMEDATA_ExtraSaveAsyncStart(gamedata, SAVE_EXTRA_ID_REC_MINE + num);
    (*seq)++;
    break;
  case 1:
    result = GAMEDATA_ExtraSaveAsyncMain(gamedata, SAVE_EXTRA_ID_REC_MINE + num);

    if(result == SAVE_RESULT_OK || result == SAVE_RESULT_NG){
      //�O���Z�[�u�����B�Z�[�u�V�X�e����j��
      SaveControl_Extra_UnloadWork(sv, SAVE_EXTRA_ID_REC_MINE + num);
      return result;
    }
    break;
  }
  return SAVE_RESULT_CONTINUE;
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃Z�[�u
 *
 * @param sv        �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heap_id   �Z�[�u�V�X�e���쐬�Ɉꎞ�I�Ɏg�p����q�[�vID
 * @param   rec_mode    �^�惂�[�h(BATTLE_MODE_???)
 * @param   fight_count   ����ڂ�
 * @param num   ���[�h����f�[�^�i���o�[�iLOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c�j
 * @param   work0   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work1   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 *
 * @retval  �Z�[�u����(SAVE_RESULT_OK or SAVE_RESULT_NG ���Ԃ�܂ŏꍇ�͖��t���[���Ăё����Ă�������)
 */
//------------------------------------------------------------------
SAVE_RESULT BattleRec_Save(GAMEDATA *gamedata, HEAPID heap_id, BATTLE_MODE rec_mode, int fight_count, int num, u16 *work0, u16 *work1)
{
  SAVE_CONTROL_WORK *sv = GAMEDATA_GetSaveControlWork(gamedata);
  BATTLE_REC_HEADER *head;
  BATTLE_REC_WORK *rec;
  SAVE_RESULT result;
  u16 test_crc;

  switch(*work0){
  case 0:
    //�f�[�^���Ȃ��Ƃ��́A�������Ȃ�
    if(brs==NULL){
      return  SAVE_RESULT_NG;
    }
    head = &brs->head;
    rec = &brs->rec;

    //�^��f�[�^�{�̂����Ƀw�b�_�f�[�^�쐬
    RecHeaderCreate(sv, head, rec, rec_mode, fight_count);

    //CRC�쐬
    head->magic_key = REC_OCC_MAGIC_KEY;
    head->crc.crc16ccitt_hash = GFL_STD_CrcCalc(head,
      sizeof(BATTLE_REC_HEADER) - GDS_CRC_SIZE - DATANUMBER_SIZE);
    rec->magic_key = REC_OCC_MAGIC_KEY;
    BattleRec_CalcCrcRec( rec );

    //CRC���L�[�ɂ��ĈÍ���
    test_crc = rec->crc.crc16ccitt_hash;
    BattleRec_Coded(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
      rec->crc.crc16ccitt_hash + ((rec->crc.crc16ccitt_hash ^ 0xffff) << 16));
    //�����Ɉ���������ꍇ��CRC�̊J�n�ʒu���\���̏��4�o�C�g�A���C�����g����Ă��Ȃ�
    GF_ASSERT(rec->crc.crc16ccitt_hash == test_crc);

    *work1 = 0;
    (*work0)++;
    break;
  case 1:
    result = Local_BattleRecSave(gamedata, brs, num, work1, heap_id);
    return result;
  }

  return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �^�惂�[�h����N���C�A���g���Ǝ莝�����̏�����擾
 *
 * @param   rec_mode      �^�惂�[�h��bit��`(BattleRecModeBitTbl�̓��e��)
 * @param   client_max    �N���C�A���g�������
 * @param   temoti_max    �莝���ő吔�����
 */
//--------------------------------------------------------------
void BattleRec_ClientTemotiGet(u16 mode_bit, int *client_max, int *temoti_max)
{
  int i;

  for(i = 0; i < BATTLE_MODE_MAX; i++){
    if(BattleRecModeBitTbl[i] == mode_bit){
      switch(i){
      case BATTLE_MODE_COLOSSEUM_MULTI_FREE:
      case BATTLE_MODE_COLOSSEUM_MULTI_50:
      case BATTLE_MODE_COLOSSEUM_MULTI_FREE_SHOOTER:
      case BATTLE_MODE_COLOSSEUM_MULTI_50_SHOOTER:
      case BATTLE_MODE_SUBWAY_MULTI:
        *client_max = 4;
        *temoti_max = TEMOTI_POKEMAX / 2;
        return;
      }
    }
  }
  *client_max = 2;
  *temoti_max = TEMOTI_POKEMAX;
}

//--------------------------------------------------------------
/**
 * @brief   �^��f�[�^�{�̂����Ƀw�b�_�[�f�[�^���쐬
 *
 * @param   head    �w�b�_�[�f�[�^�����
 * @param   rec     �^��f�[�^�{��
 * @param   rec_mode  �^�惂�[�h(BATTLE_MODE_???)
 * @param   counter   ����ڂ�
 */
//--------------------------------------------------------------
static void RecHeaderCreate(SAVE_CONTROL_WORK *sv, BATTLE_REC_HEADER *head, const BATTLE_REC_WORK_PTR rec, int rec_mode, int counter)
{
  int client, temoti, client_max, temoti_max, n;
  const REC_POKEPARA *para;

  GFL_STD_MemClear(head, sizeof(BATTLE_REC_HEADER));

  BattleRec_ClientTemotiGet(BattleRecModeBitTbl[rec_mode], &client_max, &temoti_max);

  n = 0;

  for(client = 0; client < client_max; client++){
    for(temoti = 0; temoti < temoti_max; temoti++){
      para = &(rec->rec_party[client].member[temoti]);
      if(para->tamago_flag == 0 && para->fusei_tamago_flag == 0){
        head->monsno[n] = para->monsno;
        head->form_no_and_sex[n] = ( para->form_no & HEADER_FORM_NO_MASK ) | ( para->sex << HEADER_GENDER_MASK );
      }
      n++;
    }
  }

  GF_ASSERT(rec_mode < BATTLE_MODE_MAX);
  head->battle_counter = counter;
  head->mode = BattleRecModeBitTbl[rec_mode];
  head->server_vesion = BTL_NET_SERVER_VERSION;
}

//--------------------------------------------------------------
/**
 * @brief   �ΐ�^��f�[�^�����������ꂽ�f�[�^�����ׂ�
 *
 * @param   src   �ΐ�^��f�[�^�ւ̃|�C���^
 *
 * @retval  TRUE:����������Ă���f�[�^
 * @retval  FALSE:���炩�̃f�[�^�������Ă���
 */
//--------------------------------------------------------------
static BOOL BattleRec_DataInitializeCheck(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *src)
{
  BATTLE_REC_WORK *rec = &src->rec;
  BATTLE_REC_HEADER *head = &src->head;

  if(rec->magic_key != REC_OCC_MAGIC_KEY || head->magic_key != REC_OCC_MAGIC_KEY){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^�̐������`�F�b�N
 *
 * @param   src   �f�[�^�̐擪�A�h���X
 *
 * @retval  TRUE:�������B�@FALSE:�s��
 */
//--------------------------------------------------------------
static  BOOL BattleRecordCheckData(SAVE_CONTROL_WORK *sv, const BATTLE_REC_SAVEDATA * src)
{
  const BATTLE_REC_WORK *rec = &src->rec;
  const BATTLE_REC_HEADER *head = &src->head;
  u16 hash;

//  if(rec->crc.crc16ccitt_hash == 0 || head->crc.crc16ccitt_hash == 0){
  if(rec->magic_key != REC_OCC_MAGIC_KEY || head->magic_key != REC_OCC_MAGIC_KEY){
    return FALSE;
  }

  //�w�b�_�[��CRC�n�b�V���v�Z
  hash = GFL_STD_CrcCalc(head,
    sizeof(BATTLE_REC_HEADER) -GDS_CRC_SIZE-DATANUMBER_SIZE);
  if(hash != head->crc.crc16ccitt_hash){
    BATTLE_REC_Printf("�w�b�_�[��CRC�n�b�V���s��\n");
    return FALSE;
  }

  //�{�̑S�̂�CRC�n�b�V���v�Z
  hash = GFL_STD_CrcCalc(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);
  if (hash != rec->crc.crc16ccitt_hash) {
    BATTLE_REC_Printf("�^��f�[�^�{�̂�CRC�n�b�V���s��\n");
    return FALSE;
  }

  return TRUE;
}
//============================================================================================
/**
 *  ��������
 *
 * @param[in] data  ��������f�[�^�̃|�C���^
 * @param[in] size  ��������f�[�^�̃T�C�Y
 * @param[in] code  �Í����L�[�̏����l
 */
//============================================================================================
static  void  BattleRec_Decoded(void *data,u32 size,u32 code)
{
  GFL_STD_CODED_Decoded(data,size,code);
}
//============================================================================================
/**
 *  �Í�����
 *
 * @param[in] data  �Í�������f�[�^�̃|�C���^
 * @param[in] size  �Í�������f�[�^�̃T�C�Y
 * @param[in] code  �Í����L�[�̏����l
 */
//============================================================================================
void  BattleRec_Coded(void *data,u32 size,u32 code)
{
  GFL_STD_CODED_Coded(data, size, code);
}


//==============================================================================
//
//  �f�[�^�擾
//
//==============================================================================


BTLREC_OPERATION_BUFFER*  BattleRec_GetOperationBufferPtr( void )
{
  GF_ASSERT(brs);
  return &(brs->rec.opBuffer);
}




//--------------------------------------------------------------
/**
 * @brief   GDS�v���t�B�[���̃|�C���^�擾
 *
 * @param   num         LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c
 *
 * @retval  �v���t�B�[���̃|�C���^
 *
 * BattleRec_Load or BattleRec_Init���g�p����brs�Ƀf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
GDS_PROFILE_PTR BattleRec_GDSProfilePtrGet(void)
{
  GF_ASSERT(brs != NULL);
  return (GDS_PROFILE_PTR)&brs->profile;
}

//--------------------------------------------------------------
/**
 * @brief   �^��{�̂̃|�C���^�擾
 *
 * @retval  �^��{�̂̃|�C���^
 *
 * BattleRec_Load or BattleRec_Init���g�p����brs�Ƀf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
BATTLE_REC_WORK_PTR BattleRec_WorkPtrGet(void)
{
  GF_ASSERT(brs != NULL);
  return &brs->rec;
}

//--------------------------------------------------------------
/**
 * @brief   �^��w�b�_�̃|�C���^�擾
 *
 * @retval  �^��w�b�_�̃|�C���^
 *
 * BattleRec_Load or BattleRec_Init���g�p����brs�Ƀf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
BATTLE_REC_HEADER_PTR BattleRec_HeaderPtrGet(void)
{
  GF_ASSERT(brs != NULL);
  return &brs->head;
}

//--------------------------------------------------------------
/**
 * @brief   �^��w�b�_�̃|�C���^�擾(���[�N�w��)
 *
 * @retval  �^��w�b�_�̃|�C���^
 *
 * BattleRec_LoadAlloc���g�p�������[�N�̃w�b�_�擾�p�ł�
 */
//--------------------------------------------------------------
BATTLE_REC_HEADER_PTR BattleRec_HeaderPtrGetWork(BATTLE_REC_SAVEDATA *wk_brs)
{
  return &wk_brs->head;
}

//--------------------------------------------------------------
/**
 * @brief   �ǂݍ���ł���^��f�[�^�ɑ΂��āA�w��f�[�^�ŏ㏑������
 *
 * @param   num         LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c
 * @param   gpp     GDS�v���t�B�[��
 * @param   head    �^��w�b�_
 * @param   rec     �^��{��
 * @param   sv      �Z�[�u�f�[�^�ւ̃|�C���^
 *
 * BattleRec_Load���g�p���ăf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
void BattleRec_DataSet(GDS_PROFILE_PTR gpp, BATTLE_REC_HEADER_PTR head, BATTLE_REC_WORK_PTR rec, SAVE_CONTROL_WORK *sv)
{
  GF_ASSERT(brs != NULL);
  GFL_STD_MemCopy(head, &brs->head, sizeof(BATTLE_REC_HEADER));
  GFL_STD_MemCopy(rec, &brs->rec, sizeof(BATTLE_REC_WORK));
  GFL_STD_MemCopy(gpp, &brs->profile, sizeof(GDS_PROFILE));

  //����
  BattleRec_Decoded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));
}
//--------------------------------------------------------------
/**
 * @brief   �^��w�b�_�̃p�����[�^�擾
 *
 * @param   head    �^��w�b�_�ւ̃|�C���^
 * @param   index   �f�[�^INDEX(RECHEAD_IDX_???)
 * @param   param   �p�����[�^
 *
 * @retval  �擾�f�[�^
 */
//--------------------------------------------------------------
u64 RecHeader_ParamGet(BATTLE_REC_HEADER_PTR head, int index, int param)
{
  GF_ASSERT(DATANUMBER_SIZE <= sizeof(u64));

  switch(index){
  case RECHEAD_IDX_MONSNO:
    GF_ASSERT(param < HEADER_MONSNO_MAX);
    if(head->monsno[param] > MONSNO_END){
      return 0;
    }
    return head->monsno[param];
  case RECHEAD_IDX_FORM_NO:
    GF_ASSERT(param < HEADER_MONSNO_MAX);
    return head->form_no_and_sex[param] & HEADER_FORM_NO_MASK;

  case RECHEAD_IDX_COUNTER:
    if ( head->battle_counter > REC_COUNTER_MAX ){
      return REC_COUNTER_MAX;
    }
    return head->battle_counter;

  case RECHEAD_IDX_MODE:
    {
      int i;
      for(i = 0; i < BATTLE_MODE_MAX; i++){
        if(head->mode == BattleRecModeBitTbl[i]){
          return i;
        }
      }
    }
    return BATTLE_MODE_COLOSSEUM_SINGLE_FREE;
  case RECHEAD_IDX_DATA_NUMBER:
    return head->data_number;
  case RECHEAD_IDX_SECURE:
    return head->secure;
  case RECHEAD_IDX_GENDER:
    GF_ASSERT(param < HEADER_MONSNO_MAX);
    return ( head->form_no_and_sex[param] & HEADER_GENDER_MASK ) >> HEADER_GENDER_SHIFT;
  case RECHEAD_IDX_SERVER_VER:
    return head->server_vesion;
  }

  GF_ASSERT(0); //�s����INDEX
  return 0;
}

//--------------------------------------------------------------
/**
 * @brief   ������ROM�̃T�[�o�[�o�[�W�������������T�[�o�[�o�[�W�������L�^����Ă��邩�m�F
 *
 * @param   ������ɘ^��ł��邩���m�F�������ꍇ��BATTLE_SETUP_PARAM��commServerVer
 *          �o�g���r�f�I�̃f�[�^���m�F�������ꍇ��RecHeader_ParamGet( header, RECHEAD_IDX_SERVER_VER ,0 )�Ŏ擾�����l������Ă�������
 *
 * @retval  TRUE:dd�����Ɠ������ȉ��̃T�[�o�[�o�[�W�����i�ۑ��A�Đ��\�j
 * @retval  FALSE:������ROM���������T�[�o�[�o�[�W�������L�^����Ă���
 */
//--------------------------------------------------------------
BOOL BattleRec_ServerVersionCheck( u8 version )
{
  return version <= BTL_NET_SERVER_VERSION;
}
//----------------------------------------------------------------------------
/**
 *	@brief  REC_WORK��CRC���Čv�Z
 *	        �i�s�������΍�̂��߂Ƀ|�P��������u��������̂ŁA���̍ۂɎg�p�j
 *
 *	@param	*wk_brs BRS
 */
//-----------------------------------------------------------------------------
void BattleRec_CalcCrcRec( BATTLE_REC_WORK_PTR rec )
{
  rec->crc.crc16ccitt_hash = GFL_STD_CrcCalc(rec,
      sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);
}


FS_EXTERN_OVERLAY(battle_recorder_tools);

//=============================================================================================
/**
 * �^��f�[�^�i�[�E�����Ɏg���c�[���I�[�o�[���C���W���[�������[�h����
 */
//=============================================================================================
void BattleRec_LoadToolModule( void )
{
  GFL_OVERLAY_Load( FS_OVERLAY_ID(battle_recorder_tools) );
}
//=============================================================================================
/**
 * �^��f�[�^�i�[�E�����Ɏg���c�[���I�[�o�[���C���W���[�����A�����[�h����
 */
//=============================================================================================
void BattleRec_UnloadToolModule( void )
{
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(battle_recorder_tools) );
}

