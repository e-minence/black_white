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
  BTL_RECFIELD_ACTION,        ///< �ʏ�s���I��
  BTL_RECFIELD_ROTATION,      ///< ���[�e�[�V����
//  BTL_RECFIELD_POKE_CHANGE,   ///< �^�[���r���̃|�P��������ւ�
}BtlRecFieldType;

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
  u8  numClients : 3;   ///< �������񂾃N���C�A���g��
  u8  type       : 4;   ///< �������ݒ��̃f�[�^�^�C�v
  u8  fError     : 1;   ///< �G���[�����t���O

  u8  buffer[ BTL_RECTOOL_BUFSIZE ];   ///< �������݃o�b�t�@

}BTL_RECTOOL;

extern void BTL_RECTOOL_Init( BTL_RECTOOL* recTool );
extern void BTL_RECTOOL_PutSelActionData( BTL_RECTOOL* recTool, u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction );
extern void* BTL_RECTOOL_FixSelActionData( BTL_RECTOOL* recTool, u32* dataSize );
extern void BTL_RECTOOL_PutRotationData( BTL_RECTOOL* recTool, u8 clientID, BtlRotateDir dir );
extern void* BTL_RECTOOL_FixRotationData( BTL_RECTOOL* recTool, u32* dataSize );



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


typedef struct {

  const u8*   recordData;
  u32         readPtr;
  u32         dataSize;

  u8          buf[64];

}BTL_RECREADER;


extern void BTL_RECREADER_Init( BTL_RECREADER* wk, const void* recordData, u32 dataSize );
extern const BTL_ACTION_PARAM* BTL_RECREADER_ReadAction( BTL_RECREADER* wk, u8 clientID, u8 *numAction );

