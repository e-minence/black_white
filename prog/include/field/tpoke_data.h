//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		tpoke_data.h
 *	@brief  �A������|�P�������  OBJCODE �傫��
 *	@author	tomoya takahashi
 *	@date		2010.03.23
 *
 *	���W���[�����FTPOKE_DATA
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/game_data.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�A������|�P�������
//=====================================
typedef struct _TPOKE_DATA TPOKE_DATA;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

// �f�[�^�ǂݍ��݁E�j��
extern TPOKE_DATA* TPOKE_DATA_Create( HEAPID heapID );
extern void TPOKE_DATA_Delete( TPOKE_DATA* p_wk );

// �I�u�W�F�R�[�h�擾
extern u16 TPOKE_DATA_GetObjCode( const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno );
// �������擾
extern BOOL TPOKE_DATA_IsSizeBig( const GAMEDATA* cp_gdata, const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno );
extern BOOL TPOKE_DATA_IsSizeNormal( const GAMEDATA* cp_gdata, const TPOKE_DATA* cp_wk, u16 monsno, u16 sex, u16 formno );


#ifdef _cplusplus
}	// extern "C"{
#endif



