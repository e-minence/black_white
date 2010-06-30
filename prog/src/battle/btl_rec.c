//=============================================================================================
/**
 * @file  btl_rec.c
 * @brief �|�P����WB �o�g���V�X�e�� �^��f�[�^����
 * @author  taya
 *
 * @date  2009.12.06  �쐬
 */
//=============================================================================================

#include <gflib.h>
#include "savedata\battle_rec.h"

#include "btl_common.h"
#include "btl_util.h"
#include "btl_rec.h"




static inline u8 MakeRecFieldTag( BtlRecFieldType type, u8 numClient, u8 fChapter )
{
  GF_ASSERT(numClient < 16);
  GF_ASSERT(type < 8);
  GF_ASSERT(fChapter < 2);

  return ((fChapter&1) << 7) | ((type&0x07) << 4) | ((numClient)&0x0f);
}
static inline void ReadRecFieldTag( u8 tagCode, BtlRecFieldType* type, u8* numClient, u8* fChapter )
{
  *numClient = tagCode & 0x0f;
  *type = ((tagCode >> 4) & 0x07);
  *fChapter = ((tagCode>>7)&0x01);
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
  HEADER_TIMING_CODE = 0,
  HEADER_FIELD_TAG,
  HEADER_SIZE,
};

struct _BTL_REC {
//  u16  bufSize;
  u16  writePtr;
  u8   fSizeOver;
  u8   dmy;
  const void* readData;
  u8   buf[ BTLREC_OPERATION_BUFFER_SIZE ];
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
    const u8* pData = (const u8*)data;
    u32 endPtr;

    // HEADER_TIMING_CODE �����͕s�v�Ȃ̂ŃX�L�b�v
    ++pData;
    --size;

    endPtr = wk->writePtr + size;
    if( endPtr <= sizeof(wk->buf) )
    {
      GFL_STD_MemCopy( pData, &wk->buf[wk->writePtr], size );
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
 * �^�C�~���O�R�[�h�擾
 *
 * @param   data
 *
 * @retval  BtlRecTiming
 */
//=============================================================================================
BtlRecTiming BTL_REC_GetTimingCode( const void* data )
{
  const u8* pByte = (const void*)data;
  return (*pByte);
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
  u32 i;

  wk->recordData = recordData;
  wk->dataSize = dataSize;
  wk->fError = FALSE;

  for(i=0; i<NELEMS(wk->readPtr); ++i){
    wk->readPtr[i] = 0;
  }
}
//=============================================================================================
/**
 * �ǂݍ��݈ʒu�����Z�b�g
 *
 * @param   wk
 */
//=============================================================================================
void BTL_RECREADER_Reset( BTL_RECREADER* wk )
{
  u32 i;
  for(i=0; i<NELEMS(wk->readPtr); ++i){
    wk->readPtr[i] = 0;
  }
}
//=============================================================================================
/**
 * �o�g���J�n�`���v�^�[���ł���Ă��邩�`�F�b�N�i���ǂݍ��݃|�C���^�����̕������߂�j
 *
 * @param   wk
 *
 * @retval  BOOL    �ł���Ă�����TRUE
 */
//=============================================================================================
BOOL BTL_RECREADER_CheckBtlInCmd( BTL_RECREADER* wk, u8 clientID )
{
  BtlRecFieldType type;
  u8 numClient, fChapter;

  u32* rp;

  rp = &wk->readPtr[ clientID ];
  ReadRecFieldTag( wk->recordData[ (*rp) ], &type, &numClient, &fChapter );

  if( type == BTL_RECFIELD_BTLSTART )
  {
    (*rp)++;
    return fChapter;
  }
  return FALSE;
}
//=============================================================================================
/**
 * �A�N�V�����f�[�^�P���ǂݍ���
 *
 * @param   wk
 * @param   clientID    �ΏۃN���C�A���gID
 * @param   numAction   [out] �A�N�V�����f�[�^��
 * @param   fChapter    [out] �`���v�^�[�i��؂�j�R�[�h�����ߍ��܂�Ă�����TRUE
 *
 * @retval  const BTL_ACTION_PARAM*   �ǂݍ��񂾃A�N�V�����f�[�^�擪
 */
//=============================================================================================
const BTL_ACTION_PARAM* BTL_RECREADER_ReadAction( BTL_RECREADER* wk, u8 clientID, u8 *numAction, u8* fChapter )
{
  BtlRecFieldType type;
  u8 numClient, readClientID, readNumAction;
  u32* rp;
  u32 i;

  if( wk->fError )
  {
    BTL_ACTION_PARAM* actionParam = (BTL_ACTION_PARAM*)(wk->readBuf[ clientID ]);
    BTL_ACTION_SetRecPlayError( actionParam );
    *numAction = 1;
    *fChapter = FALSE;
    return actionParam;
  }

  rp = &wk->readPtr[ clientID ];

  BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActStart, *rp );
  while( (*rp) < wk->dataSize )
  {
    ReadRecFieldTag( wk->recordData[ (*rp) ], &type, &numClient, fChapter );
    (*rp)++;

    if( (*rp) > wk->dataSize ){
      break;
    }
    if( type == BTL_RECFIELD_ACTION )
    {
      const BTL_ACTION_PARAM* returnPtr = NULL;
      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_SeekClient, numClient);
      if( numClient==0 || (numClient>BTL_CLIENT_MAX) ){
        break;
      }
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[ (*rp)++ ], &readClientID, &readNumAction );

        BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_CheckMatchClient, readClientID, clientID, readNumAction );

        if( ((*rp) >= wk->dataSize) ){ break; }
        if( readClientID != clientID )
        {
          (*rp) += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        }
        else
        {
          returnPtr = (const BTL_ACTION_PARAM*)(&wk->recordData[(*rp)]);
          GFL_STD_MemCopy( returnPtr, wk->readBuf[clientID], readNumAction * sizeof(BTL_ACTION_PARAM) );
          returnPtr = (const BTL_ACTION_PARAM*)(wk->readBuf[ clientID ]);
          BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActParam, (*rp), returnPtr->gen.cmd, returnPtr->fight.waza);
          (*rp) += (sizeof(BTL_ACTION_PARAM) * readNumAction);
          *numAction = readNumAction;
        }
      }
      if( returnPtr ){
        return returnPtr;
      }
    }
    else if( type == BTL_RECFIELD_TIMEOVER )
    {
      BTL_ACTION_PARAM* actionParam = (BTL_ACTION_PARAM*)(wk->readBuf[ clientID ]);

      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadTimeOverCmd, (*rp) );
      BTL_ACTION_SetRecPlayOver( actionParam );
      *numAction = 1;
      *fChapter = FALSE;
      return actionParam;
    }
    else if( type == BTL_RECFIELD_BTLSTART )
    {
      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActSkip, numClient, (*rp) );
    }
    else{
      break;
    }
    if( ((*rp) >= wk->dataSize) ){ break; }
  }


  GF_ASSERT_MSG(0, "�s���ȃf�[�^�ǂݎ�� clientID=%d, type=%d, readPtr=%d, datSize=%d",
            clientID, type, (*rp), wk->dataSize);

  wk->fError = TRUE;
  {
    BTL_ACTION_PARAM* actionParam = (BTL_ACTION_PARAM*)(wk->readBuf[ clientID ]);
    BTL_ACTION_SetRecPlayError( actionParam );
    *numAction = 1;
    *fChapter = FALSE;
    return actionParam;
  }
}

//=============================================================================================
/**
 * �L�^����Ă���`���v�^���𐔂���
 *
 * @param   wk
 *
 * @retval  u32
 */
//=============================================================================================
u32 BTL_RECREADER_GetTurnCount( const BTL_RECREADER* wk )
{
  u32 p = 0;
  u32 turnCount = 0;

  BtlRecFieldType type;
  u8 fChapter, numClient, readClientID, readNumAction;

  while( p < wk->dataSize )
  {
    ReadRecFieldTag( wk->recordData[p++], &type, &numClient, &fChapter );
    if( fChapter ){
      ++turnCount;
    }
    if( (p >= wk->dataSize) ){ break; }
    if( type != BTL_RECFIELD_ACTION )
    {
      p += numClient;
    }
    else
    {
      u32 i;
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( wk->recordData[p++], &readClientID, &readNumAction );
        p += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        if( (p >= wk->dataSize) ){ break; }
      }
    }
  }
  return turnCount;
}
//=============================================================================================
/**
 * �Ō�܂œǂݍ��܂ꂽ������
 *
 * @param   wk
 *
 * @retval  BOOL
 */
//=============================================================================================
BOOL BTL_RECREADER_IsReadComplete( const BTL_RECREADER* wk, u8 clientID )
{
  if( wk->fError == FALSE )
  {
    const u32* rp = &wk->readPtr[ clientID ];
    BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_CheckReadComp, clientID, (*rp), wk->dataSize );
    return ( (*rp) == wk->dataSize );
  }
  return FALSE;
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
 * @param   fChapter    �`���v�^�[�i�^�[���擪�j�t���O
 */
//=============================================================================================
void BTL_RECTOOL_Init( BTL_RECTOOL* recTool, BOOL fChapter )
{
  GFL_STD_MemClear( recTool, sizeof(BTL_RECTOOL) );
  recTool->fChapter = fChapter;
}
//=============================================================================================
/**
 * RECTOOL �o�g���J�n�`���v�^�f�[�^�𐶐�
 *
 * @param   recTool
 * @param   dataSize    ���M�f�[�^�T�C�Y
 *
 * @retval  void*   ���M�f�[�^�|�C���^
 */
//=============================================================================================
void* BTL_RECTOOL_PutBtlInTiming( BTL_RECTOOL* recTool, u32* dataSize, BOOL fChapter )
{
  recTool->buffer[ HEADER_TIMING_CODE ] = BTL_RECTIMING_BtlIn;
  recTool->buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( BTL_RECFIELD_BTLSTART, 0, fChapter );
  *dataSize = HEADER_SIZE;
  return recTool->buffer;
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
  // �w�b�_�L�q�p�ɋ󂯂Ă���
  if( recTool->writePtr == 0 ){
    recTool->type = BTL_RECFIELD_ACTION;
    recTool->writePtr = HEADER_SIZE;
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
void* BTL_RECTOOL_FixSelActionData( BTL_RECTOOL* recTool, BtlRecTiming timingCode, u32* dataSize )
{
  GF_ASSERT(recTool->type == BTL_RECFIELD_ACTION);

  if( recTool->fError == 0 )
  {
    recTool->buffer[ HEADER_TIMING_CODE ] = timingCode;
    recTool->buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( recTool->type, recTool->numClients, recTool->fChapter );
    *dataSize = recTool->writePtr;
    return recTool->buffer;
  }
  return NULL;
}

//=============================================================================================
/**
 * RECTOOL �^�C���I�[�o�[�ʒm�R�[�h�̏������ݏ���
 *
 * @param   recTool
 * @param   dataSize
 *
 * @retval  void*
 */
//=============================================================================================
void* BTL_RECTOOL_PutTimeOverData( BTL_RECTOOL* recTool, u32* dataSize )
{
  recTool->buffer[ HEADER_TIMING_CODE ] = BTL_RECTIMING_None;
  recTool->buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( BTL_RECFIELD_TIMEOVER, 0, FALSE );
  *dataSize = HEADER_SIZE;

  BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_TimeOverCmdWrite );
  return recTool->buffer;
}


//=============================================================================================
/**
 * [�R�}���h�������`�F�b�N�p] �N���C�A���g���󂯎�����A�N�V�����I���f�[�^�� RECTOOL �ɏ����߂�
 *
 * @param   recTool
 * @param   data
 * @param   dataSize
 */
//=============================================================================================
void BTL_RECTOOL_RestoreStart( BTL_RECTOOL* recTool, const void* data, u32 dataSize )
{
  GF_ASSERT(dataSize<sizeof(recTool->buffer));

  GFL_STD_MemCopy( data, recTool->buffer, dataSize );
  recTool->writePtr = dataSize;
  recTool->clientBit = 0;
  recTool->numClients = 0;
  recTool->type = 0;
  recTool->fChapter = 0;
  recTool->fError = 0;
}

//=============================================================================================
/**
 * [�R�}���h�������`�F�b�N�p] �N���C�A���g���󂯎�����A�N�V�����I���f�[�^�ǂݎ��
 *
 * @param   recTool
 * @param   rp          [io]
 * @param   clientID    [out]
 * @param   posIdx      [out]
 * @param   action      [out]
 *
 * @retval  BOOL    ����ȏ�ǂ߂Ȃ��Ȃ�����FALSE
 */
//=============================================================================================
BOOL BTL_RECTOOL_RestoreFwd( BTL_RECTOOL* recTool, u32* rp, u8* clientID, u8* numAction, BTL_ACTION_PARAM* action )
{
  if( (*rp) == 0 ){
    (*rp) = HEADER_SIZE;  // �w�b�_�����X�L�b�v
  }

  if( (*rp) < recTool->writePtr )
  {
    u32 dataSize;

    ReadClientActionTag( recTool->buffer[ (*rp) ], clientID, numAction );

    GF_ASSERT((*clientID) < BTL_CLIENT_MAX);
    GF_ASSERT_MSG((*numAction) <= BTL_POSIDX_MAX, "numAction=%d", (*numAction));

    dataSize = sizeof(BTL_ACTION_PARAM) * (*numAction);
    ++(*rp);
    GFL_STD_MemCopy( &(recTool->buffer[(*rp)]), action, dataSize );
    (*rp) += dataSize;


    return TRUE;
  }

  return FALSE;
}
