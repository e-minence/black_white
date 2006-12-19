//==============================================================================
/**
 *
 *@file		areaman.h
 *@brief	VRAM�L�����f�[�^�̈�}�l�[�W��
 *@author	taya
 *@data		2006.10.27
 *
 */
//==============================================================================
#ifndef __AREAMAN_H__
#define __AREAMAN_H__

#include "heapsys.h"

//--------------------------------------------
// �s���S�^��`
//--------------------------------------------
typedef struct _AREAMAN		AREAMAN;


//--------------------------------------------
// 
//--------------------------------------------
typedef	u32		AREAMAN_POS;

#define AREAMAN_POS_NOTFOUND	(0xffffffff)

//------------------------------------------------------------------
/**
 * �̈�}�l�[�W���쐬
 *
 * @param   maxBlock	�Ǘ�����u���b�N��
 * @param   heapID		�q�[�vID
 *
 * @retval  AREAMAN*	�L�����}�l�[�W���{��
 */
//------------------------------------------------------------------
extern AREAMAN*  AREAMAN_Create( u32 maxBlock, u32 heapID );


//------------------------------------------------------------------
/**
 * 
 *
 * @param   charMan		
 *
 * @retval  extern void		
 */
//------------------------------------------------------------------
extern void AREAMAN_Delete( AREAMAN* charMan );




extern AREAMAN_POS AREAMAN_ReserveAuto( AREAMAN* charMan,  u32 blockNum );
extern AREAMAN_POS AREAMAN_ReserveAssignArea( AREAMAN* charMan, u32 startBlock, u32 numAreaBlock, u32 numReserveBlock );
extern BOOL AREAMAN_ReserveAssignPos( AREAMAN* charMan, AREAMAN_POS pos, u32 blockNum );

extern void AREAMAN_Release( AREAMAN* charMan, AREAMAN_POS pos, u32 blockNum );



#ifdef PM_DEBUG
extern void AREAMAN_SetPrintDebug( AREAMAN* man, BOOL flag );
#endif

#endif	// __AREAMAN_H__
