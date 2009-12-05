//=============================================================================================
/**
 * @file  btl_rec.h
 * @brief �|�P����WB �o�g���V�X�e�� �^��f�[�^����
 * @author  taya
 *
 * @date  2009.12.06  �쐬
 */
//=============================================================================================
#pragma once



typedef struct _BTL_REC   BTL_REC;


extern BTL_REC* BTL_REC_Create( HEAPID heapID );
extern void BTL_REC_Delete( BTL_REC* wk );
extern void BTL_REC_Write( BTL_REC* wk, const void* data, u32 size );
extern BOOL BTL_REC_IsCorrect( const BTL_REC* wk );

