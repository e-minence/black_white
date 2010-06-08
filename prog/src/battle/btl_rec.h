//=============================================================================================
/**
 * @file  btl_rec.h
 * @brief �|�P����WB �o�g���V�X�e�� �N���C�A���g����L�^�f�[�^����
 * @author  taya
 *
 * @date  2009.12.06  �쐬
 */
//=============================================================================================
#pragma once

#include "btl_action.h"

typedef enum {
  BTL_RECFIELD_NULL = 0,
  BTL_RECFIELD_ACTION,          ///< �ʏ�s���I��
  BTL_RECFIELD_BTLSTART,        ///< �o�g���J�n
  BTL_RECFIELD_TIMEOVER,        ///< ���Ԑ����������̂ŏI��
  BTL_RECFIELD_SIZEOVER,        ///< ����ȏ�R�}���h�L���ł��Ȃ��̂ŏI��
}BtlRecFieldType;

/**
 *  �R�}���h�����^�C�~���O
 */
typedef enum {
  BTL_RECTIMING_None = 0,
  BTL_RECTIMING_StartTurn,
  BTL_RECTIMING_PokeInCover,
  BTL_RECTIMING_PokeInChange,
  BTL_RECTIMING_BtlIn,
}BtlRecTiming;

/*===========================================================================================*/
/*                                                                                           */
/* �T�[�o�[�p                                                                                */
/*                                                                                           */
/* �N���C�A���g�ɑ��M����f�[�^�𐶐����邽�߂̃c�[����                                      */
/*                                                                                           */
/*===========================================================================================*/

enum {
  BTL_RECTOOL_BUFSIZE = 60,
};

/**
 *  �T�[�o���L�^�f�[�^�𐶐������邽�߂̃��[�N�̈�
 */
typedef struct {

  u8  writePtr;         ///< �������݃|�C���^
  u8  clientBit;        ///< �N���C�A���g���̏������݊����r�b�g
  u8  numClients;       ///< �������񂾃N���C�A���g��
  u8  type       : 6;   ///< �������ݒ��̃f�[�^�^�C�v
  u8  fChapter   : 1;   ///< �`���v�^�[�t���O
  u8  fError     : 1;   ///< �G���[�����t���O

  u8  buffer[ BTL_RECTOOL_BUFSIZE ];   ///< �������݃o�b�t�@

}BTL_RECTOOL;

extern void BTL_RECTOOL_Init( BTL_RECTOOL* recTool, BOOL fChapter );
extern void BTL_RECTOOL_PutSelActionData( BTL_RECTOOL* recTool, u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction );
extern void* BTL_RECTOOL_PutBtlInChapter( BTL_RECTOOL* recTool, u32* dataSize );
extern void* BTL_RECTOOL_FixSelActionData( BTL_RECTOOL* recTool, BtlRecTiming timingCode, u32* dataSize );
extern void* BTL_RECTOOL_PutTimeOverData( BTL_RECTOOL* recTool, u32* dataSize );
extern void BTL_RECTOOL_PutRotationData( BTL_RECTOOL* recTool, u8 clientID, BtlRotateDir dir );
extern void* BTL_RECTOOL_FixRotationData( BTL_RECTOOL* recTool, u32* dataSize );

extern void BTL_RECTOOL_RestoreStart( BTL_RECTOOL* recTool, const void* data, u32 dataSize );
extern BOOL BTL_RECTOOL_RestoreFwd( BTL_RECTOOL* recTool, u32* rp, u8* clientID, u8* posIdx, BTL_ACTION_PARAM* action );


/*===========================================================================================*/
/*                                                                                           */
/* �N���C�A���g�p                                                                            */
/*                                                                                           */
/* �T�[�o�[���瑗���Ă����f�[�^���i�[���ǂݎ�邽�߂̊Ǘ����W���[��                        */
/*                                                                                           */
/*===========================================================================================*/

typedef struct _BTL_REC   BTL_REC;

extern BTL_REC* BTL_REC_Create( HEAPID heapID );
extern void BTL_REC_Delete( BTL_REC* wk );
extern void BTL_REC_Write( BTL_REC* wk, const void* data, u32 size );
extern BOOL BTL_REC_IsCorrect( const BTL_REC* wk );
extern const void* BTL_REC_GetDataPtr( const BTL_REC* wk, u32* size );
extern BtlRecTiming BTL_REC_GetTimingCode( const void* data );

typedef struct {

  const u8*   recordData;
  u32         dataSize  : 31;
  u32         fError    :  1;
//  u32         readPtr;
  u32         readPtr[ BTL_CLIENT_MAX ];
  u8          readBuf[ BTL_CLIENT_MAX ][64];

}BTL_RECREADER;


extern void BTL_RECREADER_Init( BTL_RECREADER* wk, const void* recordData, u32 dataSize );
extern void BTL_RECREADER_Reset( BTL_RECREADER* wk );
extern BOOL BTL_RECREADER_CheckBtlInChapter( BTL_RECREADER* wk, u8 clientID );
extern const BTL_ACTION_PARAM* BTL_RECREADER_ReadAction( BTL_RECREADER* wk, u8 clientID, u8 *numAction, u8* fChapter );
extern u32 BTL_RECREADER_GetTurnCount( const BTL_RECREADER* wk );
extern BOOL BTL_RECREADER_IsReadComplete( const BTL_RECREADER* wk, u8 clientID );




