//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_net.h
 *	@brief  WIFI�o�g���p�l�b�g���W���[��
 *	@author	Toru=Nagihashi
 *	@date		2009.11.25
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�l�b�g���W���[��
//=====================================
typedef struct _WIFIBATTLEMATCH_NET_WORK WIFIBATTLEMATCH_NET_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
extern WIFIBATTLEMATCH_NET_WORK * WIFIBATTLEMATCH_NET_Init( HEAPID heapID );
extern void WIFIBATTLEMATCH_NET_Exit( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern void WIFIBATTLEMATCH_NET_Main( WIFIBATTLEMATCH_NET_WORK *p_wk );

extern BOOL WIFIBATTLEMATCH_NET_IsError( const WIFIBATTLEMATCH_NET_WORK *cp_wk );

extern void WIFIBATTLEMATCH_NET_StartMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk );
extern BOOL WIFIBATTLEMATCH_NET_WaitMatchMake( WIFIBATTLEMATCH_NET_WORK *p_wk );
