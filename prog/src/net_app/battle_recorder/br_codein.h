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

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define CODE_BLOCK_MAX		( 3 )		///< ���̓u���b�N�ő吔

//-------------------------------------
///	
//=====================================
typedef enum
{
  CODEIN_SELECT_NONE,
  CODEIN_SELECT_CANCEL,
  CODEIN_SELECT_DECIDE,
} CODEIN_SELECT;


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
	
	int		  	word_len;					    ///< ���͕�����	
	int		  	block[ CODE_BLOCK_MAX + 1 ];///< ���̓u���b�N�@xx-xxxx-xxx �Ƃ�
	
	int			  end_state;					///< �I�����̏��
	STRBUF*		strbuf;						///< �󗓂Ȃ�o�b�t�@�����łȂ���΃f�t�H���g�l

  GFL_CLUNIT  *p_unit;
  BR_RES_WORK *p_res;
} CODEIN_PARAM;

//-------------------------------------
///	�R�[�h�C�����[�N�s���S�^
//=====================================
typedef struct _CODEIN_WORK CODEIN_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//�V�X�e��
extern CODEIN_WORK * CODEIN_Init( const CODEIN_PARAM *cp_param, HEAPID heapID );
extern void CODEIN_Exit( CODEIN_WORK *wk );
extern void CODEIN_Main( CODEIN_WORK *wk );
extern CODEIN_SELECT CODEIN_GetSelect( const CODEIN_WORK *wk );

//�����쐬
extern CODEIN_PARAM*	CodeInput_ParamCreate( int heap_id, int word_len, GFL_CLUNIT *p_unit, BR_RES_WORK *p_res, int block[] );
extern void	 CodeInput_ParamDelete( CODEIN_PARAM* codein_param );


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
static inline void CodeIn_BlockDataMake_4_4_4( int block[] )
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
static inline void CodeIn_BlockDataMake_2_5_5( int block[] )
{
	block[ 0 ] = 2;
	block[ 1 ] = 5;
	block[ 2 ] = 5;
}
