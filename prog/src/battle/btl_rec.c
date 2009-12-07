//=============================================================================================
/**
 * @file  btl_rec.c
 * @brief �|�P����WB �o�g���V�X�e�� �^��f�[�^����
 * @author  taya
 *
 * @date  2009.12.06  �쐬
 */
//=============================================================================================


#include "btl_common.h"
#include "btl_rec.h"





static inline u8 MakeRecFieldTag( BtlRecFieldType type, u8 numClient )
{
  GF_ASSERT(numClient < 16);
  GF_ASSERT(type < 16);

  return (type << 4) | (numClient);
}
static inline void ReadRecFieldTag( u8 tagCode, BtlRecFieldType* type, u8* numClient )
{
  *type = ((tagCode >> 4) & 0x0f);
  *numClient = tagCode & 0x0f;
}


static inline u8 MakeClientActionTag( u8 clientID, u8 numAction )
{
  GF_ASSERT(clientID < 8);
  GF_ASSERT(numAction < 32);

  return (clientID << 5) | (numAction);
}
static inline void ReadClientActionTag( u8 tagCode, u8* clientID, u8* numAction )
{
  *clientID  = ((tagCode>>5) & 0x07);
  *numAction = (tagCode & 0x01f);
}

static inline u8 MakeRotationTag( u8 clientID, BtlRotateDir dir )
{
  GF_ASSERT(clientID < 8);
  GF_ASSERT(dir < 32);

  return (clientID << 5) | (dir);
}
static inline void ReadRotationTag( u8 tagCode, u8* clientID, BtlRotateDir* dir )
{
  *clientID  = ((tagCode>>5) & 0x07);
  *dir = (tagCode & 0x01f);
}


/*===========================================================================================*/
/*                                                                                           */
/* �N���C�A���g�p                                                                            */
/*                                                                                           */
/* �ΐ펞�F�^��f�[�^�̒~�ρ^�Đ����F�^��f�[�^�̓ǂݎ��                                    */
/*                                                                                           */
/*===========================================================================================*/

enum {
  BTL_REC_SIZE = 4096,
};


struct _BTL_REC {
//  u16  bufSize;
  u16  writePtr;
  u8   fSizeOver;
  u8   dmy;
  const void* readData;
  u8   buf[ BTL_REC_SIZE ];
};


//=============================================================================================
/**
 * �C���X�^���X����
 *
 * @param   heapID
 *
 * @retval  BTL_REC*
 */
//=============================================================================================
BTL_REC* BTL_REC_Create( HEAPID heapID )
{
  BTL_REC* wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_REC) );
  return wk;
}
//=============================================================================================
/**
 * �C���X�^���X�j��
 *
 * @param   wk
 */
//=============================================================================================
void BTL_REC_Delete( BTL_REC* wk )
{
  GFL_HEAP_FreeMemory( wk );
}
//=============================================================================================
/**
 * �f�[�^��������
 *
 * @param   wk
 * @param   data
 * @param   size
 */
//=============================================================================================
void BTL_REC_Write( BTL_REC* wk, const void* data, u32 size )
{
  if( wk->fSizeOver == FALSE )
  {
    u32 endPtr = wk->writePtr + size;
    if( endPtr <= sizeof(wk->buf) )
    {
      GFL_STD_MemCopy( data, &wk->buf[wk->writePtr], size );
      wk->writePtr = endPtr;
    }
    else
    {
      wk->fSizeOver = TRUE;
    }
  }
}
//=============================================================================================
/**
 * �Ō�܂Ő���ɏ������܂�Ă��邩�`�F�b�N
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_REC_IsCorrect( const BTL_REC* wk )
{
  return !(wk->fSizeOver);
}

//=============================================================================================
/**
 * �������݃f�[�^�擾
 *
 * @param   wk
 * @param   size
 *
 * @retval  const void*
 */
//=============================================================================================
const void* BTL_REC_GetDataPtr( const BTL_REC* wk, u32* size )
{
  *size = wk->writePtr;
  return wk->buf;
}

//=============================================================================================
/**
 * �ǂݍ��݊J�n
 *
 * @param   wk
 * @param   recordData
 * @param   dataSize
 */
//=============================================================================================
void BTL_RECREADER_Init( BTL_RECREADER* wk, const void* recordData, u32 dataSize )
{
  wk->recordData = recordData;
  wk->dataSize = dataSize;
  wk->readPtr = 0;
}

//=============================================================================================
/**
 *
 *
 * @param   wk
 * @param   clientID      �ΏۃN���C�A���gID
 * @param   type          [out] �߂�l�f�[�^���
 *
 * @retval  const void*   �P�����̃f�[�^�擪�|�C���^
 */
//=============================================================================================
//=============================================================================================
/**
 * �A�N�V�����f�[�^�P���ǂݍ���
 *
 * @param   wk
 * @param   clientID    �ΏۃN���C�A���gID
 * @param   numAction   [out] �A�N�V�����f�[�^��
 *
 * @retval  const BTL_ACTION_PARAM*   �ǂݍ��񂾃A�N�V�����f�[�^�擪
 */
//=============================================================================================
const BTL_ACTION_PARAM* BTL_RECREADER_ReadAction( BTL_RECREADER* wk, u8 clientID, u8 *numAction )
{
  BtlRecFieldType type;
  u8 numClient, readClientID, readNumAction;
  u32 i;

  while( 1 )
  {
    ReadRecFieldTag( wk->recordData[wk->readPtr++], &type, &numClient );
    if( (wk->readPtr >= wk->dataSize) ){ break; }
    if( type != BTL_RECFIELD_ACTION )
    {
      wk->readPtr += numClient;
    }
    else
    {
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[wk->readPtr++], &readClientID, &readNumAction );
        if( (wk->readPtr >= wk->dataSize) ){ break; }
        if( readClientID != clientID )
        {
          wk->readPtr += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        }
        else
        {
          const BTL_ACTION_PARAM* returnPtr = (const BTL_ACTION_PARAM*)(&wk->recordData[wk->readPtr]);
          wk->readPtr += (sizeof(BTL_ACTION_PARAM) * readNumAction);

          *numAction = readNumAction;
          return returnPtr;
        }
      }
    }
    if( (wk->readPtr >= wk->dataSize) ){ break; }
  }


  GF_ASSERT_MSG(0, "�s���ȃf�[�^�ǂݎ�� clientID=%d, type=%d", clientID, type);
  return NULL;
}





/*===========================================================================================*/
/*                                                                                           */
/* �T�[�o�[�p                                                                                */
/*                                                                                           */
/* �N���C�A���g�ɑ��M����f�[�^�𐶐����邽�߂̃c�[����                                      */
/*                                                                                           */
/*===========================================================================================*/

//=============================================================================================
/**
 * RECTOOL ������
 *
 * @param   recTool
 */
//=============================================================================================
void BTL_RECTOOL_Init( BTL_RECTOOL* recTool )
{
  GFL_STD_MemClear( recTool, sizeof(BTL_RECTOOL) );
}

//=============================================================================================
/**
 * RECTOOL �A�N�V�����I���f�[�^���P�N���C�A���g���A�ǉ�
 *
 * @param   recTool
 * @param   clientID
 * @param   action
 * @param   numAction
 */
//=============================================================================================
void BTL_RECTOOL_PutSelActionData( BTL_RECTOOL* recTool, u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction )
{
  // �w�b�_�L�q�p�� 1byte �󂯂Ă���
  if( recTool->writePtr == 0 ){
    recTool->type = BTL_RECFIELD_ACTION;
    recTool->writePtr++;
  }

  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( (recTool->clientBit & (1 << clientID)) == 0 )
  {
    u32 endPtr = recTool->writePtr + ((sizeof(BTL_ACTION_PARAM) * numAction) + 1);
    if( endPtr <= sizeof(recTool->buffer) )
    {
      recTool->clientBit |= (1 << clientID);
      recTool->numClients++;

      recTool->buffer[ recTool->writePtr ] = MakeClientActionTag( clientID, numAction );
      GFL_STD_MemCopy( action, &recTool->buffer[recTool->writePtr+1], sizeof(BTL_ACTION_PARAM)*numAction );
      recTool->writePtr = endPtr;
    }
    else
    {
      GF_ASSERT(0);   // �f�[�^�e�ʂ���������
      recTool->fError = 1;
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "client_%d �̃f�[�^�������݂��Q�x�������Ă���", clientID);
  }
}
//=============================================================================================
/**
 * RECTOOL �A�N�V�����I���f�[�^�̏������ݏ������I��
 *
 * @param   recTool
 * @param   dataSize  [out] �N���C�A���g�ɑ��M����f�[�^�T�C�Y������
 *
 * @retval  void*   ����I���Ȃ�N���C�A���g�ɑ��M����f�[�^�|�C���^�^�G���[����NULL
 */
//=============================================================================================
void* BTL_RECTOOL_FixSelActionData( BTL_RECTOOL* recTool, u32* dataSize )
{
  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( recTool->fError == 0 )
  {
    recTool->buffer[0] = MakeRecFieldTag( recTool->type, recTool->numClients );
    *dataSize = recTool->writePtr;
    return recTool->buffer;
  }
  return NULL;
}

//=============================================================================================
/**
 * [RECTOOL] ���[�e�[�V�����I���f�[�^���P�N���C�A���g���A�ǉ�
 *
 * @param   recTool
 * @param   clientID
 * @param   dir
 */
//=============================================================================================
void BTL_RECTOOL_PutRotationData( BTL_RECTOOL* recTool, u8 clientID, BtlRotateDir dir )
{
  // �w�b�_�L�q�p�� 1byte �󂯂Ă���
  if( recTool->writePtr == 0 ){
    recTool->type = BTL_RECFIELD_ROTATION;
    recTool->writePtr++;
  }

  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( (recTool->clientBit & (1 << clientID)) == 0 )
  {
    if( recTool->writePtr < sizeof(recTool->buffer) )
    {
      recTool->clientBit |= (1 << clientID);
      recTool->numClients++;
      recTool->buffer[ recTool->writePtr++ ] = MakeRotationTag( clientID, dir );
    }
    else
    {
      GF_ASSERT(0);   // �f�[�^�e�ʂ���������
      recTool->fError = 1;
    }
  }
  else
  {
    GF_ASSERT_MSG(0, "client_%d �̃f�[�^�������݂��Q�x�������Ă���", clientID);
  }

}

//=============================================================================================
/**
 * [RECTOOL] ���[�e�[�V�����I���f�[�^�̏������݂��I��
 *
 * @param   recTool
 * @param   dataSize  [out] �N���C�A���g�ɑ��M����f�[�^�T�C�Y������
 *
 * @retval  void*   ����I���Ȃ�N���C�A���g�ɑ��M����f�[�^�|�C���^�^�G���[����NULL
 */
//=============================================================================================
void* BTL_RECTOOL_FixRotationData( BTL_RECTOOL* recTool, u32* dataSize )
{
  GF_ASSERT(recTool->type == BTL_RECFIELD_ROTATION);

  if( recTool->fError == 0 )
  {
    recTool->buffer[0] = MakeRecFieldTag( recTool->type, recTool->numClients );
    *dataSize = recTool->writePtr;
    return recTool->buffer;
  }
  return NULL;
}

