//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		wb_attr_data.h
 *	@brief  WB��p�@�A�g���r���[�g���
 *	@author	GAME FREAK inc.
 *	@date		2010.03.02
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�A�g���r���[�g�E�����f�[�^�@�O�p�`���t���O
//=====================================
typedef enum {
  WB_NORMALVTX_TRYANGLE_ONE=0,    // �O�p�`�P�̃f�[�^
  WB_NORMALVTX_TRYANGLE_TWO=1,    // �O�p�`�Q�̃f�[�^

  WB_NORMALVTX_TRYANGLE_FLAG_MAX,

} WB_NORMALVTX_TRYANGLE_FLAG;

//-----------------------------------------------------------------------------
/**
 *  WB_NORMALVTXST_TR1  
 *  WB_NORMALVTXST_TR2  
 *
 *    ���̃u���b�N���ɁAWB_NORMALVTXST_TR1�̃O���b�h�����Ȃ��ꍇ�ɂ́A
 *    ���ׂẴO���b�h��WB_NORMALVTXST_TR1�̌`���ŏo�͂���܂��B
 *
 *    ���̃u���b�N���ɁA�P�ł��O�p�`���Q�K�v�ȃO���b�h(WB_NORMALVTXST_TR2)��
 *    �������ꍇ�ɂ́A���ׂẴO���b�h��WB_NORMALVTXST_TR2�̌`���ŏo�͂��܂��B
 *
*/
//-----------------------------------------------------------------------------
/// �����f�[�^�擾�p
// 3�p�`�P
typedef struct {
  u16 tryangleFlag:1;
	u16	vecN1_x:15;         //�P�ڂ̎O�p�`�̖@���x�N�g��
	u16	vecN1_D;            // ax+by+cz+d =0 ��D�l

	u32		attr:31;          //�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	u32		tryangleType:1;   //�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2
} WB_NORMALVTXST_TR1;

// 3�p�`�P
typedef struct {
  u16 tryangleFlag:1;     // WB_NORMALVTX_TRYANGLE_FLAG
	u16	vecN1_x:15;         //�P�ڂ̎O�p�`�̖@���x�N�g��
	u16	vecN2_x;            //�Q�ڂ̎O�p�`�̖@���x�N�g��
	u16	vecN1_D;            // ax+by+cz+d =0 ��D�l
	u16	vecN2_D;       

	u32		attr:31;          //�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	u32		tryangleType:1;   //�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2
} WB_NORMALVTXST_TR2;

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief  �g���C�A���O���t���O�擾
 *
 *	@param	cp_data    �f�[�^
 *
 *	@retval WB_NORMALVTX_TRYANGLE_FLAG
 */
//-----------------------------------------------------------------------------
static inline WB_NORMALVTX_TRYANGLE_FLAG WB_NORMALVTXST_GetTryangleFlag( const void* cp_data )
{
  const WB_NORMALVTXST_TR1* cp_tr = cp_data;
  return cp_tr->tryangleFlag;
}

#ifdef _cplusplus
}	// extern "C"{
#endif



