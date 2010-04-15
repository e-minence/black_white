//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_codein.h
 *	@brief  �R�[�h����
 *	@author	Toru=Nagihashi
 *	@date		2009.11.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "br_res.h"
#include "br_util.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_CODE_BLOCK_MAX		( 3 )		///< ���̓u���b�N�ő吔

//-------------------------------------
///	
//=====================================
typedef enum
{
  BR_CODEIN_SELECT_NONE,
  BR_CODEIN_SELECT_CANCEL,
  BR_CODEIN_SELECT_DECIDE,
} BR_CODEIN_SELECT;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�R�[�h�C������
//=====================================
typedef struct 
{
	
	int		  	word_len;					    ///<[in ]���͕�����	
	int		  	block[ BR_CODE_BLOCK_MAX + 1 ]; ///<[in ]���̓u���b�N�@xx-xxxx-xxx �Ƃ�
	
	int			  end_state;				///< [out]�I�����̏��
	STRBUF*		strbuf;						///< [out]�󗓂Ȃ�o�b�t�@�����łȂ���΃f�t�H���g�l
  GFL_CLUNIT  *p_unit;        ///< [in ]OAM�Ǘ�
  BR_RES_WORK *p_res;         ///< [in ]���\�[�X�Ǘ�
  BR_BALLEFF_WORK *p_balleff; ///< [in ]���o���[�N
} BR_CODEIN_PARAM;

//-------------------------------------
///	�R�[�h�C�����[�N�s���S�^
//=====================================
typedef struct _BR_CODEIN_WORK BR_CODEIN_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//�V�X�e��
extern BR_CODEIN_WORK * BR_CODEIN_Init( const BR_CODEIN_PARAM *cp_param, HEAPID heapID );
extern void BR_CODEIN_Exit( BR_CODEIN_WORK *wk );
extern void BR_CODEIN_Main( BR_CODEIN_WORK *wk );
extern BR_CODEIN_SELECT BR_CODEIN_GetSelect( const BR_CODEIN_WORK *wk );
extern void BR_CODEINT_GetNumberBlock( const BR_CODEIN_WORK *wk, int *p_tbl, int tbl_max );
extern u64 BR_CODEIN_GetNumber( const BR_CODEIN_WORK *wk );

//�����쐬
extern BR_CODEIN_PARAM*	BR_CODEIN_ParamCreate( int heap_id, int word_len, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, BR_BALLEFF_WORK *p_balleff, int block[] );
extern void	 BR_CODEIN_ParamDelete( BR_CODEIN_PARAM* codein_param );

//--------------------------------------------------------------
/**
 * @brief	xxxx-xxxx-xxxx �̃u���b�N��`�����
 *
 * @param	block[]	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void BR_CODEIN_BlockDataMake_4_4_4( int block[] )
{
#if 1
	block[ 0 ] = 4;
	block[ 1 ] = 4;
	block[ 2 ] = 4;
#else
	block[ 0 ] = 2;
	block[ 1 ] = 5;
	block[ 2 ] = 5;
#endif
}


//--------------------------------------------------------------
/**
 * @brief	xx-xxxxx-xxxxx �̃u���b�N��`�����
 *
 * @param	block[]	
 *
 * @retval	static inline	
 *
 */
//--------------------------------------------------------------
static inline void BR_CODEIN_BlockDataMake_2_5_5( int block[] )
{
	block[ 0 ] = 2;
	block[ 1 ] = 5;
	block[ 2 ] = 5;
}
