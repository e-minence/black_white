//=============================================================================================
/**
 * @file	hand_tokusei_common.h
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]���L
 * @author	taya
 *
 * @date	2008.11.11	�쐬
 */
//=============================================================================================
#ifndef __HAND_TOKUSEI_COMMON_H__
#define __HAND_TOKUSEI_COMMON_H__

#include "../../btl_common.h"
#include "../../btl_event_factor.h"


//--------------------------------------------------------------
/**
 *	�Ƃ������C�x���g�n���h���o�^�֐��̌^��`
 */
//--------------------------------------------------------------
typedef BTL_EVENT_FACTOR* (*pTokuseiEventAddFunc)( u16 pri, u8 pokeID );


//--------------------------------------------------------------
/**
 *	�Ƃ������C�x���g�n���h���o�^�֐��Q
 */
//--------------------------------------------------------------
extern BTL_EVENT_FACTOR*  HAND_TOK_ADD_Ikaku( u16 pri, u8 pokeID );
extern BTL_EVENT_FACTOR*  HAND_TOK_ADD_ClearBody( u16 pri, u8 pokeID );
extern BTL_EVENT_FACTOR*  HAND_TOK_ADD_Seisinryoku( u16 pri, u8 pokeID );
extern BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukutsuno( u16 pri, u8 pokeID );



#endif
