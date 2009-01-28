//=============================================================================================
/**
 * @file	btl_pokeselect.h
 * @brief	�|�P����WB �o�g���V�X�e��	�|�P�����I����ʗp�p�����[�^���̒�`
 * @author	taya
 *
 * @date	2008.09.25	�쐬
 */
//=============================================================================================
#ifndef __BTL_POKESELECT_H__
#define __BTL_POKESELECT_H__

#include "btl_common.h"
#include "btl_main.h"

//------------------------------
/**
 *	�I���ł��Ȃ����R
 */
//------------------------------
typedef enum {
	BTL_POKESEL_CANT_NONE=0,		///< �I���ł��܂�

	BTL_POKESEL_CANT_DEAD,			///< ����ł邩��
	BTL_POKESEL_CANT_FIGHTING,	///< ����Ă邩��
	BTL_POKESEL_CANT_SELECTED,	///< �����^�[���ł����I�΂ꂽ����

}BtlPokeselReason;

//------------------------------------------------------
/**
 *	�|�P�����I����ʗp�p�����[�^
 */
//------------------------------------------------------
typedef struct {

	const BTL_PARTY*	party;				///< �p�[�e�B�f�[�^
	u8								numSelect;		///< �I������|�P������
	u8								prohibit[ BTL_PARTY_MEMBER_MAX ];
	u8								aliveOnly;		///< TRUE���Ɛ����Ă���|�P���������I�ׂȂ�

}BTL_POKESELECT_PARAM;

//------------------------------------------------------
/**
 *	���ʊi�[�p�\����
 */
//------------------------------------------------------
typedef struct {

	u8			selIdx[ BTL_POSIDX_MAX ];	///< �I�������|�P�����̕��я��i���݂̕��я��Ő擪��0�Ɛ�����j
	u8			cnt;											///< �I��������
	u8			max;											///< �I�����鐔

}BTL_POKESELECT_RESULT;



extern void BTL_POKESELECT_PARAM_Init( BTL_POKESELECT_PARAM* param, const BTL_PARTY* party, u8 numSelect, u8 aliveOnly );
extern void BTL_POKESELECT_PARAM_SetProhibitFighting( BTL_POKESELECT_PARAM* param, u8 numCover );
extern void BTL_POKESELECT_PARAM_SetProhibit( BTL_POKESELECT_PARAM* param, BtlPokeselReason reason, u8 idx );

extern void BTL_POKESELECT_RESULT_Init( BTL_POKESELECT_RESULT *result, const BTL_POKESELECT_PARAM* param );
extern void BTL_POKESELECT_RESULT_Push( BTL_POKESELECT_RESULT *result, u8 idx );
extern void BTL_POKESELECT_RESULT_Pop( BTL_POKESELECT_RESULT *result );

extern BOOL BTL_POKESELECT_IsDone( const BTL_POKESELECT_RESULT* result );
extern u8 BTL_POKESELECT_RESULT_GetCount( const BTL_POKESELECT_RESULT* result );
extern u8 BTL_POKESELECT_RESULT_GetLast( const BTL_POKESELECT_RESULT* result );


extern BtlPokeselReason BTL_POKESELECT_CheckProhibit( const BTL_POKESELECT_PARAM* param, const BTL_POKESELECT_RESULT* result, u8 idx );

#endif
