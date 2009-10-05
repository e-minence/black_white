//======================================================================
/**
 * @file    field_beacon_message.h
 * @brief   �t�B�[���h�Ńr�[�R���Ń��b�Z�[�W������肷��
 * @author  ariizumi
 * @data    09/10/02
 *
 * ���W���[�����FFIELD_BEACON_MSG
 */
//======================================================================
#include <gflib.h>
#include "buflen.h"
#include "system/main.h"
#include "system/gfl_use.h"

#include "field/field_beacon_message.h"
#include "test/ariizumi/ari_debug.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define
#define FBM_MESSAGE_DATA_NUM (10)
#define FBM_SENDER_STATE_NUM (20)
#define FBM_INVALID_IDX (0xFF)
#define FBM_SEND_CNT_MAX (0x80)


//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef struct _FBM_MESSAGE_DATA FBM_MESSAGE_DATA;
typedef struct _FBM_SENDER_STATE FBM_SENDER_STATE;


//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
//���b�Z�[�W�̕\���f�[�^
struct _FBM_MESSAGE_DATA
{
  u16 word[BEACON_MESSAGE_DATA_WORD_NUM];
  STRCODE name[BEACON_MESSAGE_DATA_NAME_LENGTH];  //8

  u8  senderIdx;
};

//���b�Z�[�W���M�҂̃f�[�^
struct _FBM_SENDER_STATE
{
  u8 macAddress[6];
  u8 cnt;
//  u8 dataIdx;
};

//�r�[�R�����b�Z�[�W�̃f�[�^
struct _FIELD_BEACON_MSG_DATA
{
  FBM_MESSAGE_DATA postMsgData[FBM_MESSAGE_DATA_NUM];
  FBM_SENDER_STATE senderData[FBM_SENDER_STATE_NUM];
  
  FBM_MESSAGE_DATA selfMsgData;
  
  //�f�[�^���������O�o�b�t�@��Ԃɂ��邽�߂ɐ擪Idx������Ă���
  u8 postDataTop;
  u8 senderDataTop;
};

//�r�[�R�����b�Z�[�W�̃V�X�e��
struct _FIELD_BEACON_MSG_SYS
{
  FIELD_BEACON_MSG_DATA *fbmData;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static const BOOL FIELD_BEACON_MSG_CompSenderData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress );
static const u8 FIELD_BEACON_MSG_AddSenderData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress );
static void FIELD_BEACON_MSG_AddMessageData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , const u8 senderIdx );

//--------------------------------------------------------------
//	���b�Z�[�W�f�[�^�̍쐬
//--------------------------------------------------------------
FIELD_BEACON_MSG_DATA* FIELD_BEACON_MSG_CreateData( const HEAPID heapId , MYSTATUS *myStatus )
{
  FIELD_BEACON_MSG_DATA* fbmData = GFL_HEAP_AllocMemory( heapId , sizeof(FIELD_BEACON_MSG_DATA) );
  
  FIELD_BEACON_MSG_ResetData( fbmData , myStatus );
  
  return fbmData;
}

//--------------------------------------------------------------
//	���b�Z�[�W�f�[�^�̔j��
//--------------------------------------------------------------
void FIELD_BEACON_MSG_DeleteData( FIELD_BEACON_MSG_DATA* fbmData )
{
  GFL_HEAP_FreeMemory( fbmData );
}

//--------------------------------------------------------------
//	���b�Z�[�W�f�[�^�̏�����
//--------------------------------------------------------------
void FIELD_BEACON_MSG_ResetData( FIELD_BEACON_MSG_DATA* fbmData  , MYSTATUS *myStatus )
{
  u8 i;
  GFL_STD_MemClear( fbmData , sizeof(FIELD_BEACON_MSG_DATA) );
  
  for( i=0;i<FBM_MESSAGE_DATA_NUM;i++ )
  {
    fbmData->postMsgData[i].senderIdx = FBM_INVALID_IDX;
  }
  /*
  for( i=0;i<FBM_SENDER_STATE_NUM;i++ )
  {
    fbmData->senderData[i].dataIdx = FBM_INVALID_IDX;
  }
  */
  fbmData->selfMsgData.senderIdx = FBM_INVALID_IDX;
  
  fbmData->postDataTop = 0;
  fbmData->senderDataTop = 0;
  //�����̖��O�Z�b�g
  GFL_STD_MemCopy16( MyStatus_GetMyName(myStatus) , fbmData->selfMsgData.name , sizeof(STRCODE)*BEACON_MESSAGE_DATA_NAME_LENGTH );
  
}

//--------------------------------------------------------------
//	�V�X�e���̏�����
//--------------------------------------------------------------
FIELD_BEACON_MSG_SYS* FIELD_BEACON_MSG_InitSystem( FIELD_BEACON_MSG_DATA* fbmData , const HEAPID heapId )
{
  FIELD_BEACON_MSG_SYS* fbmSys = GFL_HEAP_AllocMemory( heapId , sizeof(FIELD_BEACON_MSG_DATA) );

  fbmSys->fbmData = fbmData;
  
  
  return fbmSys;
}

//--------------------------------------------------------------
//	�V�X�e���̊J��
//--------------------------------------------------------------
void FIELD_BEACON_MSG_ExitSystem( FIELD_BEACON_MSG_SYS* fbmSys )
{
  GFL_HEAP_FreeMemory( fbmSys );
}

//--------------------------------------------------------------
//	�r�[�R���Ƀf�[�^���Z�b�g����
//--------------------------------------------------------------
void FIELD_BEACON_MSG_SetBeaconMessage( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon )
{
  const FBM_MESSAGE_DATA *selfMsgData = &fbmSys->fbmData->selfMsgData;
  if( selfMsgData->senderIdx != FBM_INVALID_IDX )
  {
    beacon->beacon_type = GBS_BEACONN_TYPE_MESSAGE;
    GFL_STD_MemCopy16( selfMsgData->word , beacon->bmData.word , sizeof(u16)*BEACON_MESSAGE_DATA_WORD_NUM );
    GFL_STD_MemCopy16( selfMsgData->name , beacon->bmData.name , sizeof(STRCODE)*BEACON_MESSAGE_DATA_NAME_LENGTH );
    beacon->bmData.msgCnt = selfMsgData->senderIdx;
  }
}

//--------------------------------------------------------------
//	�r�[�R���̃`�F�b�N
//--------------------------------------------------------------
void FIELD_BEACON_MSG_CheckBeacon( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress )
{
  if( beacon->beacon_type == GBS_BEACONN_TYPE_MESSAGE )
  {
    const BOOL ret = FIELD_BEACON_MSG_CompSenderData( fbmSys , beacon , macAddress );
    if( ret == TRUE )
    {
      const u8 newIdx = FIELD_BEACON_MSG_AddSenderData( fbmSys , beacon , macAddress );
      FIELD_BEACON_MSG_AddMessageData( fbmSys , beacon , newIdx );
      {
        u8 i;
        OS_TPrintf("AddMessage! [%d][",beacon->bmData.msgCnt);
        for( i=0;i<6;i++ )
        {
          OS_TPrintf("%02x",macAddress[i]);
        }
        OS_TPrintf("]\n");
      }
      
    }
  }

  //�f�o�b�O����
#if DEB_ARI
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
  {
    u16 word[BEACON_MESSAGE_DATA_WORD_NUM];
    word[0] = GFL_STD_MtRand(0x10000);
    word[1] = GFL_STD_MtRand(0x10000);
    word[2] = GFL_STD_MtRand(0x10000);
    word[3] = GFL_STD_MtRand(0x10000);
    FIELD_BEACON_MESSAGE_SetWord( fbmSys->fbmData , word );
    OS_TPrintf("SendMessage[%d]\n",fbmSys->fbmData->selfMsgData.senderIdx);
  }
#endif
}

//--------------------------------------------------------------
//	���M�҂̔�r
//--------------------------------------------------------------
static const BOOL FIELD_BEACON_MSG_CompSenderData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress )
{
  const u8 startIdx = fbmSys->fbmData->senderDataTop;
  u8 idx = fbmSys->fbmData->senderDataTop;
  BOOL isNewData = TRUE;
  
  do
  {
    const FBM_SENDER_STATE *senderData = &fbmSys->fbmData->senderData[idx];
    //���M�ԍ��̔�r
    if( senderData->cnt == beacon->bmData.msgCnt )
    {
      u8 i;
      BOOL isSame = TRUE;
      //�}�b�N�A�h���X�̔�r
      for( i=0;i<6;i++ )
      {
        if( senderData->macAddress[i] != macAddress[i] )
        {
          isSame = FALSE;
          break;
        }
      }
      if( isSame == TRUE )
      {
        isNewData = FALSE;
      }
    }
    
    if( idx == 0 )
    {
      idx = FBM_SENDER_STATE_NUM - 1;
    }
    else
    {
      idx--;
    }
    
  }while( startIdx != idx && isNewData == TRUE );
  
  return isNewData;
}

//--------------------------------------------------------------
//	���M�҂̒ǉ�
//  @return �V�����f�[�^��Idx
//--------------------------------------------------------------
static const u8 FIELD_BEACON_MSG_AddSenderData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , u8 *macAddress )
{
  if( fbmSys->fbmData->senderDataTop == FBM_SENDER_STATE_NUM-1 )
  {
    fbmSys->fbmData->senderDataTop = 0;
  }
  else
  {
    fbmSys->fbmData->senderDataTop++;
  }
  {
    u8 i;
    FBM_SENDER_STATE *senderData = &fbmSys->fbmData->senderData[fbmSys->fbmData->senderDataTop];
    for( i=0;i<6;i++ )
    {
      senderData->macAddress[i] = macAddress[i];
      senderData->cnt = beacon->bmData.msgCnt;
    }
  }
  return fbmSys->fbmData->senderDataTop;
}

//--------------------------------------------------------------
//	���b�Z�[�W�̒ǉ�
//--------------------------------------------------------------
static void FIELD_BEACON_MSG_AddMessageData( FIELD_BEACON_MSG_SYS *fbmSys , GBS_BEACON *beacon , const u8 senderIdx )
{
  if( fbmSys->fbmData->postDataTop == FBM_MESSAGE_DATA_NUM-1 )
  {
    fbmSys->fbmData->postDataTop = 0;
  }
  else
  {
    fbmSys->fbmData->postDataTop++;
  }
  
  {
    const u8 idx = fbmSys->fbmData->postDataTop;
    GFL_STD_MemCopy16( beacon->bmData.word , fbmSys->fbmData->postMsgData[idx].word , sizeof(u16)*BEACON_MESSAGE_DATA_WORD_NUM );
    GFL_STD_MemCopy16( beacon->bmData.name , fbmSys->fbmData->postMsgData[idx].name , sizeof(STRCODE)*BEACON_MESSAGE_DATA_NAME_LENGTH );
    fbmSys->fbmData->postMsgData[idx].senderIdx = senderIdx;
  }
  
}

//--------------------------------------------------------------
//	���M���b�Z�[�W�̍X�V
//--------------------------------------------------------------
//  @param work �ɂ� u16 word[BEACON_MESSAGE_DATA_WORD_NUM]�̐擪�|�C���^��n���Ă�������
void FIELD_BEACON_MESSAGE_SetWord( FIELD_BEACON_MSG_DATA *fbmData , u16 *word )
{
  GFL_STD_MemCopy16( word , fbmData->selfMsgData.word , sizeof(u16)*BEACON_MESSAGE_DATA_WORD_NUM );
  
  if( fbmData->selfMsgData.senderIdx == FBM_INVALID_IDX ||
      fbmData->selfMsgData.senderIdx == FBM_SEND_CNT_MAX )
  {
    fbmData->selfMsgData.senderIdx = 0;
  }
  else
  {
    fbmData->selfMsgData.senderIdx++;
  }
}
