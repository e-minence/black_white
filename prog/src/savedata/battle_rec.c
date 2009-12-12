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
#include "gds_profile_types.h"

#include "battle_rec_local.h"

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/* Globals                                                                  */
/*--------------------------------------------------------------------------*/
BATTLE_REC_SAVEDATA * brs=NULL;

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void RecHeaderCreate(SAVE_CONTROL_WORK *sv, BATTLE_REC_HEADER *head, const BATTLE_REC_WORK *rec, int rec_mode, int counter);
static BOOL BattleRec_DataInitializeCheck(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *src);
static  BOOL BattleRecordCheckData(SAVE_CONTROL_WORK *sv, const BATTLE_REC_SAVEDATA * src);
static  void  BattleRec_Decoded(void *data,u32 size,u32 code);
static void PokeParty_to_RecPokeParty( const POKEPARTY *party, REC_POKEPARTY *rec_party );
static void RecPokeParty_to_PokeParty( REC_POKEPARTY *rec_party, POKEPARTY *party, HEAPID heapID );
static void store_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static void restore_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec, HEAPID heapID );
static void store_ClientStatus( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static void restore_ClientStatus( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );
static void store_TrainerData( const BSP_TRAINER_DATA* bspTrainer, BTLREC_TRAINER_STATUS* recTrainer );
static void restore_TrainerData( BSP_TRAINER_DATA* bspTrainer, const BTLREC_TRAINER_STATUS* recTrainer );
static BOOL store_OperationBuffer( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static BOOL restore_OperationBuffer( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );
static BOOL store_SetupSubset( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec );
static BOOL restore_SetupSubset( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec );



//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^���쐬����
 *
 * @param   heapID  �������m�ۂ��邽�߂̃q�[�vID
 */
//------------------------------------------------------------------
void BattleRec_Init(HEAPID heapID)
{
  if(brs != NULL){
    GFL_HEAP_FreeMemory(brs);
    brs = NULL;
  }

  brs = GFL_HEAP_AllocClearMemory(heapID,SAVESIZE_EXTRA_BATTLE_REC);//sizeof(BATTLE_REC_SAVEDATA));
  BattleRec_WorkInit(brs);
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

//--------------------------------------------------------------
/**
 * @brief   �F�؃L�[���������퓬�^�惏�[�N�̃A�h���X���擾
 *
 * @retval  brs�Ɋi�[����Ă��郏�[�N�A�h���X(�F�؃L�[����)
 */
//--------------------------------------------------------------
void * BattleRec_RecWorkAdrsGet( void )
{
  u8 *work;

  GF_ASSERT(brs);

  work = (u8*)brs;
  return &work[sizeof(EX_CERTIFY_SAVE_KEY)];
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃��[�h
 *
 * @param sv    �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heapID  �f�[�^�����[�h���郁�������m�ۂ��邽�߂̃q�[�vID
 * @param result  ���[�h���ʂ��i�[���郏�[�NRECLOAD_RESULT_NULL,OK,NG,ERROR
 * @param bp    ���[�h�����f�[�^���琶������BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param num   ���[�h����f�[�^�i���o�[�iLOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c�j
 *
 * @retval  TRUE
 */
//------------------------------------------------------------------
BOOL BattleRec_Load( SAVE_CONTROL_WORK *sv, HEAPID heapID, LOAD_RESULT *result, int num )
{
  BATTLE_REC_WORK *rec;
  BATTLE_REC_HEADER *head;

  //���łɓǂݍ��܂�Ă���f�[�^������Ȃ�A�j������
  if(brs){
    BattleRec_WorkInit(brs);
  }
  else{
    BattleRec_Init(heapID);
  }

  //�f�[�^��brs�Ƀ��[�h
  *result = SaveControl_Extra_LoadWork(
    sv, SAVE_EXTRA_ID_REC_MINE + num, heapID, brs, sizeof(BATTLE_REC_SAVEDATA));

  //brs�ɓW�J���ꂽ�̂ŃZ�[�u�V�X�e���͔j��
  SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);

  if(*result != LOAD_RESULT_OK){
    *result = RECLOAD_RESULT_ERROR;
    return TRUE;
  }

  rec = &brs->rec;
  head = &brs->head;

  //����
  BattleRec_Decoded(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    rec->crc.crc16ccitt_hash + ((rec->crc.crc16ccitt_hash ^ 0xffff) << 16));

  //�ǂݏo�����f�[�^�ɘ^��f�[�^�������Ă��邩�`�F�b�N
  #if 1
  if(BattleRec_DataInitializeCheck(sv, brs) == TRUE){
    OS_TPrintf("�^��f�[�^��������Ԃ̂��̂ł�\n");
    *result = RECLOAD_RESULT_NULL;  //�������f�[�^�ׁ̈A�f�[�^�Ȃ�
    return TRUE;
  }

  //�f�[�^�̐������`�F�b�N
  if(BattleRecordCheckData(sv, brs) == FALSE){
  #ifdef OSP_REC_ON
    OS_TPrintf("�s���Ș^��f�[�^\n");
  #endif
    *result = RECLOAD_RESULT_NG;
    return TRUE;
  }
  #endif

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
  if(*result != LOAD_RESULT_OK){
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
  #ifdef OSP_REC_ON
    OS_TPrintf("�s���Ș^��f�[�^\n");
  #endif
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
SAVE_RESULT Local_BattleRecSave(SAVE_CONTROL_WORK *sv, BATTLE_REC_SAVEDATA *work, int num, u16 *seq, HEAPID heap_id)
{
  SAVE_RESULT result;

  switch(*seq){
  case 0:
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
    sys_SoftResetNG(SOFTRESET_TYPE_VIDEO);
    sys_SioErrorNG_PtrSet(HEAPID_WORLD);
#endif

    //�Z�[�u�Ώۂ̊O���Z�[�u�̈�̃Z�[�u�V�X�e�����쐬(�Z�[�u���[�N�̎��̂�brs��n��)
    SaveControl_Extra_SystemSetup(sv, SAVE_EXTRA_ID_REC_MINE + num, heap_id, brs, SAVESIZE_EXTRA_BATTLE_REC);

    SaveControl_Extra_SaveAsyncInit(sv, SAVE_EXTRA_ID_REC_MINE + num);
    do{
      result = SaveControl_Extra_SaveAsyncMain(sv, SAVE_EXTRA_ID_REC_MINE + num);
    }while(result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);

    //�O���Z�[�u�����B�Z�[�u�V�X�e����j��
    SaveControl_Extra_UnloadWork(sv, SAVE_EXTRA_ID_REC_MINE + num);

#if 0 //��check�@���쐬�@�ʏ�Z�[�u�͊O���Ƃ̌q���肪�������肵�Ă���
    if(result == SAVE_RESULT_OK){
      //result = SaveData_Save(sv);
      SaveData_DivSave_Init(sv, SVBLK_ID_MAX);
      (*seq)++;
      return SAVE_RESULT_CONTINUE;
    }
#endif

#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
    sys_SoftResetOK(SOFTRESET_TYPE_VIDEO);
#endif
    return result;
  case 1:
#if 0 //��check�@���쐬�@�ʏ�Z�[�u�͊O���Ƃ̌q���肪�������肵�Ă���
    result = SaveData_DivSave_Main(sv);
    if(result == SAVE_RESULT_OK || result == SAVE_RESULT_NG){
      (*seq) = 0;
      sys_SioErrorNG_PtrFree();
      sys_SoftResetOK(SOFTRESET_TYPE_VIDEO);
    }
    return result;
#else
    break;
#endif
  }
  return SAVE_RESULT_CONTINUE;
}

//------------------------------------------------------------------
/**
 * �ΐ�^��f�[�^�̃Z�[�u
 *
 * @param sv        �Z�[�u�f�[�^�\���̂ւ̃|�C���^
 * @param heap_id   �Z�[�u�V�X�e���쐬�Ɉꎞ�I�Ɏg�p����q�[�vID
 * @param   rec_mode    �^�惂�[�h(RECMODE_???)
 * @param   fight_count   ����ڂ�
 * @param num   ���[�h����f�[�^�i���o�[�iLOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c�j
 * @param   work0   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work1   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 *
 * @retval  �Z�[�u����(SAVE_RESULT_OK or SAVE_RESULT_NG ���Ԃ�܂ŏꍇ�͖��t���[���Ăё����Ă�������)
 */
//------------------------------------------------------------------
SAVE_RESULT BattleRec_Save(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int rec_mode, int fight_count, int num, u16 *work0, u16 *work1)
{
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
    rec->crc.crc16ccitt_hash = GFL_STD_CrcCalc(rec,
      sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);

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
    result = Local_BattleRecSave(sv, brs, num, work1, heap_id);
    return result;
  }

  return SAVE_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   �w��ʒu�̘^��f�[�^���폜(������)���ăZ�[�u���s
 *
 * @param   sv
 * @param   heap_id   �폜�p�e���|����(�^��f�[�^��W�J�ł��邾���̃q�[�v���K�v�ł�)
 * @param   num
 * @param   work0   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 * @param   work1   �Z�[�u�i�s�𐧌䂷�郏�[�N�ւ̃|�C���^(�ŏ���0�N���A������ԂŌĂ�ŉ�����)
 *
 * @retval  SAVE_RESULT_CONTINUE  �Z�[�u�����p����
 * @retval  SAVE_RESULT_LAST    �Z�[�u�����p�����A�Ō�̈�O
 * @retval  SAVE_RESULT_OK      �Z�[�u����I��
 * @retval  SAVE_RESULT_NG      �Z�[�u���s�I��
 *
 * �������̓I�t���C���ōs���̂ŕ����Z�[�u�ł͂Ȃ��A�ꊇ�Z�[�u�ɂ��Ă��܂��B
 *    ��check�@WB�ł͏펞�ʐM�ׁ̈A�폜�������Z�[�u�ɕύX����\�� 2009.11.18(��)
 */
//--------------------------------------------------------------
SAVE_RESULT BattleRec_SaveDataErase(SAVE_CONTROL_WORK *sv, HEAPID heap_id, int num)
{
  SAVE_RESULT result;
  LOAD_RESULT load_result;
  BATTLE_REC_SAVEDATA *all;

  load_result = SaveControl_Extra_Load(sv, SAVE_EXTRA_ID_REC_MINE + num, heap_id);
  all = SaveControl_Extra_DataPtrGet(sv, SAVE_EXTRA_ID_REC_MINE + num, 0);
  BattleRec_WorkInit(all);

  SaveControl_Extra_SaveAsyncInit(sv, SAVE_EXTRA_ID_REC_MINE + num);
  do{
    result = SaveControl_Extra_SaveAsyncMain(sv, SAVE_EXTRA_ID_REC_MINE + num);
  }while(result == SAVE_RESULT_CONTINUE || result == SAVE_RESULT_LAST);

  SaveControl_Extra_Unload(sv, SAVE_EXTRA_ID_REC_MINE + num);
  return result;
}

//--------------------------------------------------------------
/**
 * @brief   �^�惂�[�h����N���C�A���g���Ǝ莝�����̏�����擾
 *
 * @param   rec_mode    �^�惂�[�h(RECMODE_???)
 * @param   client_max    �N���C�A���g�������
 * @param   temoti_max    �莝���ő吔�����
 */
//--------------------------------------------------------------
void BattleRec_ClientTemotiGet(int rec_mode, int *client_max, int *temoti_max)
{
  switch(rec_mode){
  case RECMODE_TOWER_MULTI:
  case RECMODE_FACTORY_MULTI:
  case RECMODE_FACTORY_MULTI100:
  case RECMODE_STAGE_MULTI:
  case RECMODE_CASTLE_MULTI:
  case RECMODE_ROULETTE_MULTI:
  case RECMODE_COLOSSEUM_MULTI:
    *client_max = BTL_CLIENT_MAX;
    *temoti_max = TEMOTI_POKEMAX / 2;
    break;
  default:
    *client_max = BTL_CLIENT_MAX / 2;
    *temoti_max = TEMOTI_POKEMAX;
    break;
  }
}

//--------------------------------------------------------------
/**
 * @brief   �^��f�[�^�{�̂����Ƀw�b�_�[�f�[�^���쐬
 *
 * @param   head    �w�b�_�[�f�[�^�����
 * @param   rec     �^��f�[�^�{��
 * @param   rec_mode  �^�惂�[�h(RECMODE_???)
 * @param   counter   ����ڂ�
 */
//--------------------------------------------------------------
static void RecHeaderCreate(SAVE_CONTROL_WORK *sv, BATTLE_REC_HEADER *head, const BATTLE_REC_WORK *rec, int rec_mode, int counter)
{
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
  int client, temoti, client_max, temoti_max, n, get_client, my_client;
  const REC_POKEPARA *para;
  const u8  stand_table[2][4]={{0,2,3,1},{3,1,0,2}};
  const u8  tower_table[4]={0,2,1,3};

  GFL_STD_MemClear(head, sizeof(BATTLE_REC_HEADER));

  BattleRec_ClientTemotiGet(rec_mode, &client_max, &temoti_max);

  n = 0;

  //�����̃N���C�A���g�i���o�[�擾
  if(rec->rbp.fight_type&FIGHT_TYPE_SIO){
    if(rec->rbp.fight_type&FIGHT_TYPE_TOWER){
      my_client=rec->rbp.comm_id*2;
    }
    else{
      my_client=rec->rbp.comm_id;
    }
  }
  else{
    my_client=0;
  }

  for(client = 0; client < client_max; client++){
    if((rec->rbp.fight_type&FIGHT_TYPE_MULTI)&&((rec->rbp.fight_type&FIGHT_TYPE_TOWER)==0)){
      for(get_client=0;get_client<client_max;get_client++){
        if(rec->rbp.comm_stand_no[get_client]==
           stand_table[rec->rbp.comm_stand_no[my_client]&1][client]){
          break;
        }
      }
    }
    else if((rec->rbp.fight_type&FIGHT_TYPE_MULTI)&&(rec->rbp.fight_type&FIGHT_TYPE_TOWER)){
      get_client= tower_table[client];
    }
    else{
      get_client=client;
      if(my_client&1){
        get_client^=1;
      }
    }
    for(temoti = 0; temoti < temoti_max; temoti++){
      para = &(rec->rec_party[get_client].member[temoti]);
      if(para->tamago_flag == 0 && para->fusei_tamago_flag == 0){
        head->monsno[n] = para->monsno;
        head->form_no[n] = para->form_no;
      }
      n++;
    }
  }

  //���M�����[�V�����f�[�^�Z�b�g
  switch(rec_mode){
  case RECMODE_COLOSSEUM_SINGLE_STANDARD:   //MIX�̓V���O���ƈꏏ
  case RECMODE_COLOSSEUM_DOUBLE_STANDARD:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_STANDARD));
    break;
  case RECMODE_COLOSSEUM_SINGLE_FANCY:
  case RECMODE_COLOSSEUM_DOUBLE_FANCY:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_FANCY));
    break;
  case RECMODE_COLOSSEUM_SINGLE_LITTLE:
  case RECMODE_COLOSSEUM_DOUBLE_LITTLE:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_LITTLE));
    break;
  case RECMODE_COLOSSEUM_SINGLE_LIGHT:
  case RECMODE_COLOSSEUM_DOUBLE_LIGHT:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_LIGHT));
    break;
  case RECMODE_COLOSSEUM_SINGLE_DOUBLE:
  case RECMODE_COLOSSEUM_DOUBLE_DOUBLE:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_DOUBLE));
    break;
  case RECMODE_COLOSSEUM_SINGLE_ETC:
  case RECMODE_COLOSSEUM_DOUBLE_ETC:
    head->regulation = *(Data_GetRegulation(sv, REGULATION_NO_ETC));
    break;
  case RECMODE_COLOSSEUM_SINGLE:    //��������
  case RECMODE_COLOSSEUM_DOUBLE:
  default:
    head->regulation = *(Data_GetNoLimitRegulation());
    break;
  }

  head->battle_counter = counter;
  head->mode = rec_mode;
#endif
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

#if 0 //��check�@�O���Z�[�u�f�[�^�̏������`�F�b�N�����쐬 2009.11.18(��)
  if(SaveData_GetExtraInitFlag(sv) == FALSE){
    return TRUE;  //�O���Z�[�u�f�[�^������������Ă��Ȃ��̂Ńf�[�^�����Ɣ��肷��
  }
#endif

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
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
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
  #ifdef OSP_REC_ON
    OS_TPrintf("�w�b�_�[��CRC�n�b�V���s��\n");
  #endif
    return FALSE;
  }

  //�{�̑S�̂�CRC�n�b�V���v�Z
  hash = GFL_STD_CrcCalc(rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE);
  if (hash != rec->crc.crc16ccitt_hash) {
  #ifdef OSP_REC_ON
    OS_TPrintf("�^��f�[�^�{�̂�CRC�n�b�V���s��\n");
  #endif
    return FALSE;
  }

  //�|�P�����p�����[�^�̕s���`�F�b�N
  {
    int client, temoti, wazano;
    const REC_POKEPARA *para;

    for(client = 0; client < BTL_CLIENT_MAX; client++){
      for(temoti = 0; temoti < TEMOTI_POKEMAX; temoti++){
        para = &(rec->rec_party[client].member[temoti]);
      #if 0
        //�_���^�}�S
        if(para->fusei_tamago_flag == 1){
          OS_TPrintf("�_���^�}�S���������Ă���\n");
          return FALSE;
        }
      #endif
        //�s���ȃ|�P�����ԍ�
        if(para->monsno > MONSNO_MAX){
        #ifdef OSP_REC_ON
          OS_TPrintf("�s���ȃ|�P�����ԍ�\n");
        #endif
          return FALSE;
        }
        //�s���ȃA�C�e���ԍ�
        if(para->item > ITEM_DATA_MAX){
        #ifdef OSP_REC_ON
          OS_TPrintf("�s���ȃA�C�e���ԍ�\n");
        #endif
          return FALSE;
        }
        //�s���ȋZ�ԍ�
        for(wazano = 0; wazano < WAZA_TEMOTI_MAX; wazano++){
          if(para->waza[wazano] > WAZANO_MAX){
          #ifdef OSP_REC_ON
            OS_TPrintf("�s���ȋZ�ԍ�\n");
          #endif
            return FALSE;
          }
        }
      }
    }
  }

  return TRUE;
#else
  return TRUE;
#endif
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
 *  BATTLE_PARAM�\���̕ۑ�����
 *
 * @param[in] bp      BATTLE_PARAM�\���̂ւ̃|�C���^
 */
//============================================================================================
void BattleRec_BattleParamRec(BATTLE_PARAM *bp)
{
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
  int i;
  BATTLE_REC_WORK *rec;
  REC_BATTLE_PARAM *rbp;

  if(brs==NULL){
    return;
  }

  rec = &brs->rec;
  rbp = &rec->rbp;

  rbp->fight_type = bp->fight_type;
  rbp->win_lose_flag = bp->win_lose_flag;
//  rbp->btr = bp->btr;
  rbp->bg_id = bp->bg_id;
  rbp->ground_id = bp->ground_id;
  rbp->place_id = bp->place_id;
  rbp->zone_id = bp->zone_id;
  rbp->time_zone = bp->time_zone;
  rbp->shinka_place_mode = bp->shinka_place_mode;
  rbp->contest_see_flag = bp->contest_see_flag;
  rbp->mizuki_flag = bp->mizuki_flag;
  rbp->get_pokemon_client = bp->get_pokemon_client;
  rbp->weather = bp->weather;
  rbp->level_up_flag = bp->level_up_flag;
  rbp->battle_status_flag = bp->battle_status_flag;
  rbp->safari_ball = bp->safari_ball;
  rbp->regulation_flag = bp->regulation_flag;
  rbp->rand = bp->rand;
  rbp->comm_id = bp->comm_id;
  rbp->dummy = bp->dummy;
  rbp->total_turn = bp->total_turn;
  for(i = 0; i < BTL_CLIENT_MAX; i++){
    rbp->trainer_id[i] = bp->trainer_id[i];
    rbp->trainer_data[i] = bp->trainer_data[i];
    if(bp->server_version[i] == 0){
      rbp->server_version[i] = BTL_NET_SERVER_VERSION;
    }
    else{
      rbp->server_version[i] = bp->server_version[i];
    }
    rbp->comm_stand_no[i] = bp->comm_stand_no[i];
    rbp->voice_waza_param[i] = bp->voice_waza_param[i];
  }

  //-- REC_BATTLE_PARAM�ł͂Ȃ��ꏊ�ɕۑ�����f�[�^���Z�b�g --//
  for(i=0;i<BTL_CLIENT_MAX;i++){
    PokeParty_to_RecPokeParty(bp->poke_party[i], &rec->rec_party[i]);
    MyStatus_Copy(bp->my_status[i],&rec->my_status[i]);
    rbp->voice_waza_param[i]=Snd_PerapVoiceWazaParamGet(bp->poke_voice[i]);
  }
  CONFIG_Copy(bp->config,&rec->config);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �T�[�o�[�o�[�W�����X�V����
 *
 * @param   id_no       ID
 * @param   server_version    �T�[�o�[�o�[�W����
 */
//--------------------------------------------------------------
void BattleRec_ServerVersionUpdate(int id_no, u32 server_version)
{
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
  BATTLE_REC_WORK *rec;
  REC_BATTLE_PARAM *rbp;

  if(brs==NULL){
    return;
  }

  rec = &brs->rec;
  rbp = &rec->rbp;
  rbp->server_version[id_no] = server_version;
//  OS_TPrintf("sio server_version = %x\n", rbp->server_version[id_no]);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ������ROM�̃T�[�o�[�o�[�W�������������T�[�o�[�o�[�W�������L�^����Ă��邩�m�F
 *
 * @retval  TRUE:�S�Ď����Ɠ������ȉ��̃T�[�o�[�o�[�W����
 * @retval  FALSE:������ROM���������T�[�o�[�o�[�W�������L�^����Ă���
 */
//--------------------------------------------------------------
BOOL BattleRec_ServerVersionCheck(void)
{
#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
  int i;
  BATTLE_REC_WORK *rec;
  REC_BATTLE_PARAM *rbp;

  if(brs == NULL){
    return TRUE;
  }

  rec = &brs->rec;
  rbp = &rec->rbp;
  for(i = 0; i < BTL_CLIENT_MAX; i++){
    if(rbp->server_version[i] > BTL_NET_SERVER_VERSION){
      return FALSE;
    }
  }
  return TRUE;
#else
  return TRUE;
#endif
}

//============================================================================================
/**
 *  BATTLE_PARAM�\���̐�������
 *
 * @param[in] bp      BATTLE_PARAM�\���̂ւ̃|�C���^
 * @param[in] sv      �Z�[�u�̈�ւ̃|�C���^
 */
//============================================================================================
void BattleRec_BattleParamCreate(BATTLE_PARAM *bp,SAVE_CONTROL_WORK *sv)
{
  int i;
  BATTLE_REC_WORK *rec = &brs->rec;

#if 0 //��check�@���쐬�@�^��f�[�^�̎d�l�����܂��Ă��� 2009.11.18(��)
  bp->fight_type      = rec->rbp.fight_type;
//  bp->win_lose_flag   = rec->rbp.win_lose_flag;
//  bp->btr         = rec->rbp.btr;
  bp->bg_id       = rec->rbp.bg_id;
  bp->ground_id     = rec->rbp.ground_id;
  bp->place_id      = rec->rbp.place_id;
  bp->zone_id       = rec->rbp.zone_id;
  bp->time_zone     = rec->rbp.time_zone;
  bp->shinka_place_mode = rec->rbp.shinka_place_mode;
  bp->contest_see_flag  = rec->rbp.contest_see_flag;
  bp->mizuki_flag     = rec->rbp.mizuki_flag;
  bp->get_pokemon_client  = rec->rbp.get_pokemon_client;
  bp->weather       = rec->rbp.weather;
//  bp->level_up_flag   = rec->rbp.level_up_flag;
  bp->battle_status_flag  = rec->rbp.battle_status_flag|BATTLE_STATUS_FLAG_REC_BATTLE;
  bp->safari_ball     = rec->rbp.safari_ball;
  bp->regulation_flag   = rec->rbp.regulation_flag;
  bp->rand        = rec->rbp.rand;
  bp->comm_id       = rec->rbp.comm_id;
//  bp->total_turn      = rec->rbp.total_turn;

  bp->win_lose_flag   = 0;
  bp->level_up_flag   = 0;

  ZukanWork_Copy(SaveData_GetZukanWork(sv),bp->zw);

  for(i=0;i<BTL_CLIENT_MAX;i++){
    bp->trainer_id[i]=rec->rbp.trainer_id[i];
    bp->trainer_data[i]=rec->rbp.trainer_data[i];
    bp->server_version[i]=rec->rbp.server_version[i];
    bp->comm_stand_no[i]=rec->rbp.comm_stand_no[i];
    RecPokeParty_to_PokeParty(&rec->rec_party[i], bp->poke_party[i]);
    MyStatus_Copy(&rec->my_status[i],bp->my_status[i]);
    bp->voice_waza_param[i]=rec->rbp.voice_waza_param[i];
  }
//  CONFIG_Copy(&rec->config,bp->config);
  CONFIG_Copy(SaveData_GetConfig(sv), bp->config);
  bp->config->window_type = rec->config.window_type;
  if(bp->config->window_type >= TALK_WINDOW_MAX){
    bp->config->window_type = 0;  //����ŉ�b�E�B���h�E�������鎖���l���ăP�A�����Ă���
  }
#endif
}

//--------------------------------------------------------------
/**
 * @brief   POKEPARTY��REC_POKEPARTY�ɕϊ�����
 *
 * @param   party     �ϊ����f�[�^�ւ̃|�C���^
 * @param   rec_party   �ϊ���̃f�[�^�����
 */
//--------------------------------------------------------------
static void PokeParty_to_RecPokeParty( const POKEPARTY *party, REC_POKEPARTY *rec_party )
{
  int i;
  POKEMON_PARAM *pp;

  GFL_STD_MemClear(rec_party, sizeof(REC_POKEPARTY));

  rec_party->PokeCountMax = PokeParty_GetPokeCountMax(party);
  rec_party->PokeCount = PokeParty_GetPokeCount(party);

  for(i=0; i < rec_party->PokeCount; i++){
    pp = PokeParty_GetMemberPointer(party, i);
    POKETOOL_PokePara_to_RecPokePara(pp, &rec_party->member[i]);
  }
}

//--------------------------------------------------------------
/**
 * @brief   REC_POKEPARTY��POKEPARTY�ɕϊ�����
 *
 * @param   rec_party   �ϊ����f�[�^�ւ̃|�C���^
 * @param   party     �ϊ���̃f�[�^�����
 */
//--------------------------------------------------------------
static void RecPokeParty_to_PokeParty( REC_POKEPARTY *rec_party, POKEPARTY *party, HEAPID heapID )
{
  int i;
  POKEMON_PARAM *pp;
  u8 cb_id_para = 0;

  pp = GFL_HEAP_AllocClearMemory( GFL_HEAP_LOWID(heapID), POKETOOL_GetWorkSize() );
  TAYA_Printf("RecParty pokeCnt=%d\n", rec_party->PokeCount);

  PokeParty_Init(party, rec_party->PokeCountMax);
  for(i = 0; i < rec_party->PokeCount; i++){
    POKETOOL_RecPokePara_to_PokePara(&rec_party->member[i], pp);
    PP_Put(pp, ID_PARA_cb_id, cb_id_para);  //�J�X�^���{�[���͏o�Ȃ��悤�ɂ���
    PokeParty_Add(party, pp);
  }

  GFL_HEAP_FreeMemory( pp );
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

BTLREC_SETUP_SUBSET*  BattleRec_GetSetupSubsetPtr( void )
{
  GF_ASSERT(brs);
  return &(brs->rec.setupSubset);
}

















//--------------------------------------------------------------
/**
 * @brief   �퓬�^��f�[�^����A�w�b�_�[��ʓr�m�ۂ������[�N�ɃR�s�[����
 *
 * @param   heap_id �q�[�vID
 *
 * @retval  �m�ۂ��ꂽ�w�b�_�[�f�[�^�ւ̃|�C���^
 *
 * BattleRec_Load���g�p���ăf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
BATTLE_REC_HEADER_PTR BattleRec_HeaderAllocCopy(HEAPID heap_id)
{
  BATTLE_REC_HEADER_PTR head;

  GF_ASSERT(brs != NULL);

  head = GFL_HEAP_AllocMemory(heap_id, sizeof(BATTLE_REC_HEADER));
  GFL_STD_MemCopy32(&brs->head, head, sizeof(BATTLE_REC_HEADER));
  return head;
}

//--------------------------------------------------------------
/**
 * @brief   �퓬�^��f�[�^����AGDS�v���t�B�[����ʓr�m�ۂ������[�N�ɃR�s�[����
 *
 * @param   src   �퓬�^��Z�[�u�f�[�^�ւ̃|�C���^
 * @param   heap_id �q�[�vID
 *
 * @retval  �m�ۂ��ꂽGDS�v���t�B�[���f�[�^�ւ̃|�C���^
 *
 * BattleRec_Load���g�p����brs�Ƀf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
GDS_PROFILE_PTR BattleRec_GDSProfileAllocCopy(HEAPID heap_id)
{
  GDS_PROFILE_PTR profile;

  GF_ASSERT(brs != NULL);

  profile = GFL_HEAP_AllocMemory(heap_id, sizeof(GDS_PROFILE));
  GFL_STD_MemCopy32(&brs->profile, profile, sizeof(GDS_PROFILE));
  return profile;

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
  return &brs->profile;
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
 * @brief   �ǂݍ���ł���^��f�[�^�ɑ΂��āA�w��f�[�^�ŏ㏑������
 *
 * @param   num         LOADDATA_MYREC�ALOADDATA_DOWNLOAD0�ALOADDATA_DOWNLOAD1�c
 * @param   gpp     GDS�v���t�B�[��
 * @param   head    �^��w�b�_
 * @param   rec     �^��{��
 * @param bp      ���̂܂܍Đ�����ꍇ��BATTLE_PARAM�\���̂ւ̃|�C���^��n��
 * @param   sv      �Z�[�u�f�[�^�ւ̃|�C���^
 *
 * BattleRec_Load���g�p���ăf�[�^�����[�h���Ă���K�v������܂��B
 */
//--------------------------------------------------------------
void BattleRec_DataSet(GDS_PROFILE_PTR gpp, BATTLE_REC_HEADER_PTR head, BATTLE_REC_WORK_PTR rec, BATTLE_PARAM *bp, SAVE_CONTROL_WORK *sv)
{
  GF_ASSERT(brs != NULL);
  GFL_STD_MemCopy(head, &brs->head, sizeof(BATTLE_REC_HEADER));
  GFL_STD_MemCopy(rec, &brs->rec, sizeof(BATTLE_REC_WORK));
  GFL_STD_MemCopy(gpp, &brs->profile, sizeof(GDS_PROFILE));

  //����
  BattleRec_Decoded(&brs->rec, sizeof(BATTLE_REC_WORK) - GDS_CRC_SIZE,
    brs->rec.crc.crc16ccitt_hash + ((brs->rec.crc.crc16ccitt_hash ^ 0xffff) << 16));

  if(bp != NULL){
    BattleRec_BattleParamCreate(bp,sv);
  }
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
    return head->form_no[param];

  case RECHEAD_IDX_COUNTER:
    if ( head->battle_counter > REC_COUNTER_MAX ){
      return REC_COUNTER_MAX;
    }
    return head->battle_counter;

  case RECHEAD_IDX_MODE:
    if(head->mode >= RECMODE_MAX){
      return RECMODE_COLOSSEUM_SINGLE;
    }
    return head->mode;
  case RECHEAD_IDX_DATA_NUMBER:
    return head->data_number;
  case RECHEAD_IDX_SECURE:
    return head->secure;
  }

  GF_ASSERT(0); //�s����INDEX
  return 0;
}

//--------------------------------------------------------------
/**
 * @brief   �퓬�^��w�b�_��Alloc����
 *
 * @param   heap_id   �q�[�vID
 *
 * @retval  GDS�v���t�B�[�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
BATTLE_REC_HEADER_PTR BattleRec_Header_AllocMemory(HEAPID heap_id)
{
  BATTLE_REC_HEADER_PTR brhp;

  brhp = GFL_HEAP_AllocMemory(heap_id, sizeof(BATTLE_REC_HEADER));
  GFL_STD_MemClear(brhp, sizeof(BATTLE_REC_HEADER));
  return brhp;
}

//--------------------------------------------------------------
/**
 * @brief   �퓬�^��w�b�_�����
 *
 * @param   brhp    GDS�v���t�B�[���ւ̃|�C���^
 */
//--------------------------------------------------------------
void BattleRec_Header_FreeMemory(BATTLE_REC_HEADER_PTR brhp)
{
  GFL_HEAP_FreeMemory(brhp);
}













//=============================================================================================
/**
 * �o�g���Z�b�g�A�b�v�p�����[�^��^��Z�[�u�f�[�^�ɕϊ����Ę^��Z�[�u�o�b�t�@�Ɋi�[����
 *
 * @param   setup   �o�g���Z�b�g�A�b�v�p�����[�^
 */
//=============================================================================================
void BattleRec_StoreSetupParam( const BATTLE_SETUP_PARAM* setup )
{
  BATTLE_REC_WORK  *rec = &brs->rec;

  store_Party( setup, rec );
  store_ClientStatus( setup, rec );
  store_OperationBuffer( setup, rec );
  store_SetupSubset( setup, rec );
}

//=============================================================================================
/**
 * �^��Z�[�u�o�b�t�@����o�g���Z�b�g�A�b�v�p�����[�^�𕜌�����
 *
 * @param   setup   [out] ������
 */
//=============================================================================================
void BattleRec_RestoreSetupParam( BATTLE_SETUP_PARAM* setup, HEAPID heapID )
{
  BATTLE_REC_WORK  *rec = &brs->rec;

  TAYA_Printf("*** Rec Info ***\n");
  TAYA_Printf(" recWorkSize = %d byte\n", sizeof(BATTLE_REC_WORK));
  TAYA_Printf("   ->setupSubset  = %d byte\n", sizeof(rec->setupSubset) );
  TAYA_Printf("   ->opBuffer     = %d byte\n", sizeof(rec->opBuffer) );
  TAYA_Printf("   ->clientStatus = %d byte\n", sizeof(rec->clientStatus) );
  TAYA_Printf("   ->rec_party    = %d byte\n", sizeof(rec->rec_party) );


  restore_Party( setup, rec, heapID );
  restore_ClientStatus( setup, rec );
  restore_OperationBuffer( setup, rec );
  restore_SetupSubset( setup, rec );
}

//----------------------------------------------------------------------------------
/**
 * �p�[�e�B�f�[�^�F�^��Z�[�u�f�[�^�����Ċi�[
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void store_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  u32 i;
  for(i=0; i<BTL_CLIENT_NUM; ++i)
  {
    if( setup->party[i] ){
      PokeParty_to_RecPokeParty( setup->party[i], &(rec->rec_party[i]) );
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �p�[�e�B�f�[�^�F�^��Z�[�u�f�[�^����
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void restore_Party( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec, HEAPID heapID )
{
  u32 i;
  for(i=0; i<BTL_CLIENT_NUM; ++i)
  {
    if( setup->party[i] ){
      RecPokeParty_to_PokeParty( &(rec->rec_party[i]), setup->party[i], heapID );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * �N���C�A���g�X�e�[�^�X�F�^��Z�[�u�f�[�^�����Ċi�[
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void store_ClientStatus( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  u32 i;
  for(i=0; i<NELEMS(setup->playerStatus); ++i)
  {
    if( setup->playerStatus[i] ){
      MyStatus_Copy( setup->playerStatus[i], &rec->clientStatus[i].player );
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_PLAYER;
    }
    else if( setup->tr_data[i] ){
      store_TrainerData( setup->tr_data[i], &rec->clientStatus[i].trainer );
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_TRAINER;
    }
    else{
      rec->clientStatus[i].type = BTLREC_CLIENTSTATUS_NONE;
    }
  }
}
//----------------------------------------------------------------------------------
/**
 * �N���C�A���g�X�e�[�^�X�F�^��Z�[�u�f�[�^���畜��
 *
 * @param   setup
 * @param   rec
 */
//----------------------------------------------------------------------------------
static void restore_ClientStatus( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  u32 i;
  for(i=0; i<NELEMS(setup->playerStatus); ++i)
  {
    switch( rec->clientStatus[i].type ){
    case BTLREC_CLIENTSTATUS_PLAYER:
      GF_ASSERT(setup->playerStatus[i]!=NULL);
      MyStatus_Copy( &rec->clientStatus[i].player, (MYSTATUS*)setup->playerStatus[i] );
      break;

    case BTLREC_CLIENTSTATUS_TRAINER:
      GF_ASSERT(setup->tr_data[i]!=NULL);
      restore_TrainerData( setup->tr_data[i], &rec->clientStatus[i].trainer );
      break;
    }
  }
}

/**
 *  �g���[�i�[�f�[�^�F�^��Z�[�u�f�[�^�p�ɕϊ����Ċi�[
 */
static void store_TrainerData( const BSP_TRAINER_DATA* bspTrainer, BTLREC_TRAINER_STATUS* recTrainer )
{
  u32 i;

  recTrainer->tr_id     = bspTrainer->tr_id;
  recTrainer->tr_type   = bspTrainer->tr_type;
  recTrainer->ai_bit    = bspTrainer->ai_bit;
  recTrainer->win_word  = bspTrainer->win_word;
  recTrainer->lose_word = bspTrainer->lose_word;

  for(i=0; i<NELEMS(recTrainer->use_item); ++i){
    recTrainer->use_item[i] = bspTrainer->use_item[i];
  }

  GFL_STR_GetStringCode( bspTrainer->name, recTrainer->name, NELEMS(recTrainer->name) );
}
/**
 *  �g���[�i�[�f�[�^�F�^��Z�[�u�f�[�^���畜��
 */
static void restore_TrainerData( BSP_TRAINER_DATA* bspTrainer, const BTLREC_TRAINER_STATUS* recTrainer )
{
  u32 i;

  bspTrainer->tr_id     = recTrainer->tr_id;
  bspTrainer->tr_type   = recTrainer->tr_type;
  bspTrainer->ai_bit    = recTrainer->ai_bit;
  bspTrainer->win_word  = recTrainer->win_word;
  bspTrainer->lose_word = recTrainer->lose_word;

  for(i=0; i<NELEMS(bspTrainer->use_item); ++i){
    bspTrainer->use_item[i] = recTrainer->use_item[i];
  }

  GFL_STR_SetStringCode( bspTrainer->name, recTrainer->name );
}
//----------------------------------------------------------------------------------
/**
 * ����o�b�t�@�F�^��Z�[�u�f�[�^�p�ɕϊ����Ċi�[
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL store_OperationBuffer( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  if( (setup->recBuffer != NULL)
  &&  (setup->recDataSize < sizeof(rec->opBuffer.buffer))
  ){
    rec->opBuffer.size = setup->recDataSize;
    GFL_STD_MemCopy( setup->recBuffer, rec->opBuffer.buffer, setup->recDataSize );
    TAYA_Printf("Store Operation Buffer %dbyte\n", rec->opBuffer.size);
    return TRUE;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * ����o�b�t�@�F�^��Z�[�u�f�[�^���畜��
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL restore_OperationBuffer( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  setup->recDataSize = rec->opBuffer.size;
  TAYA_Printf("Restore Operation Buffer %dbyte\n", setup->recDataSize );
  GFL_STD_MemCopy( rec->opBuffer.buffer, setup->recBuffer, setup->recDataSize );
  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�p�����[�^�����f�[�^��^��Z�[�u�f�[�^�ϊ����Ċi�[
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL store_SetupSubset( const BATTLE_SETUP_PARAM* setup, BATTLE_REC_WORK* rec )
{
  rec->setupSubset.fieldSituation = setup->fieldSituation;
  rec->setupSubset.randomContext = setup->recRandContext;
  rec->setupSubset.musicDefault = setup->musicDefault;
  rec->setupSubset.musicPinch = setup->musicPinch;

  rec->setupSubset.competitor = setup->competitor;
  rec->setupSubset.rule = setup->rule;
  rec->setupSubset.fMultiMode = setup->multiMode;

  CONFIG_Copy( setup->configData, &rec->setupSubset.config );

  return TRUE;
}
//----------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�p�����[�^����
 *
 * @param   setup
 * @param   rec
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL restore_SetupSubset( BATTLE_SETUP_PARAM* setup, const BATTLE_REC_WORK* rec )
{
  setup->fieldSituation = rec->setupSubset.fieldSituation;
  setup->recRandContext = rec->setupSubset.randomContext;
  setup->musicDefault = rec->setupSubset.musicDefault;
  setup->musicPinch = rec->setupSubset.musicPinch;

  setup->competitor = rec->setupSubset.competitor;
  setup->rule = rec->setupSubset.rule;
  setup->multiMode = rec->setupSubset.fMultiMode;

  CONFIG_Copy( &rec->setupSubset.config, (CONFIG*)(setup->configData) );

  return TRUE;
}

