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

#include <gflib.h>

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
  WB_NORMALVTX_TRYANGLE_TWO_INDEX=2,// �O�p�`�Q�̃f�[�^�̃C���f�b�N�X�i�T�C�Y��WB_NORMALVTXST_TR1�ƈꏏ�j

  WB_NORMALVTX_TRYANGLE_FLAG_MAX,

} WB_NORMALVTX_TRYANGLE_FLAG;

//-----------------------------------------------------------------------------
/**
 *  WB_NORMALVTXST_TR1  
 *  WB_NORMALVTXST_TR2  
*/
//-----------------------------------------------------------------------------
/// �����f�[�^�擾�p
// 3�p�`�@1��
typedef struct {
  u16 tryangleFlag:2;     // WB_NORMALVTX_TRYANGLE_FLAG
	u16	vecN1_x:14;         //�P�ڂ̎O�p�`�̖@���x�N�g���C���f�b�N�X�i���̃C���f�b�N�X����R��x,y,z�̏��Ŋi�[�j
	u16	vecN1_D;            // ax+by+cz+d =0 ��D�l
  //��tryangleFlag == WB_NORMALVTX_TRYANGLE_TWO_INDEX�̂Ƃ��AvecN1_D��WB_NORMALVTXST_TR2�z��̃C���f�b�N�X�Ƃ��Ďg�p

	u32		attr:31;          //�A�g���r���[�g�r�b�g    0-15��value 16-30��flg
	u32		tryangleType:1;   //�O�p�`�̌`�̃^�C�v  �^ = 0 ��:vecN1,�E:vecN2  �_ = 1 �E:vecN1,��:vecN2
} WB_NORMALVTXST_TR1;

// 3�p�` 2��
typedef struct {
  u16 tryangleFlag:2;     // WB_NORMALVTX_TRYANGLE_FLAG  WB_NORMALVTX_TRYANGLE_TWO �Œ�
	u16	vecN1_x:14;         //�P�ڂ̎O�p�`�̖@���x�N�g���P�ڂ̎O�p�`�̖@���x�N�g���C���f�b�N�X�i���̃C���f�b�N�X����R��x,y,z�̏��Ŋi�[�j
	u16	vecN2_x;            //�Q�ڂ̎O�p�`�̖@���x�N�g���P�ڂ̎O�p�`�̖@���x�N�g���C���f�b�N�X�i���̃C���f�b�N�X����R��x,y,z�̏��Ŋi�[�j
	u16	vecN1_D;            // ax+by+cz+d =0 ��D�l
	u16	vecN2_D;       

} WB_NORMALVTXST_TR2;


//-----------------------------------------------------------------------------
/**
 *					�C���f�b�N�X���
*/
//-----------------------------------------------------------------------------
#if 0
#define WB_NORMAL_TBL_MAX ( 9 )
#define WB_PLANE_D_TBL_MAX ( 9 )
extern const fx16 WB_NORMAL_TBL[WB_NORMAL_TBL_MAX];  // fx16*3�P�ʂŖ@�����i�[����Ă��܂��B
extern const fx32 WB_PLANE_D_TBL[WB_PLANE_D_TBL_MAX];
#endif



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

//----------------------------------------------------------------------------
/**
 *	@brief  WB_NORMALVTX_TRYANGLE_TWO_INDEX�̏�񂩂�TR2�C���f�b�N�X���擾
 *
 *	@param	cp_data    �f�[�^
 *
 *	@retval TR2�C���f�b�N�X
 */
//-----------------------------------------------------------------------------
static inline u16 WB_NORMALVTXST_GetTR2Index( const void* cp_data )
{
  const WB_NORMALVTXST_TR1* cp_tr = cp_data;
  GF_ASSERT( cp_tr->tryangleFlag == WB_NORMALVTX_TRYANGLE_TWO_INDEX );
  return cp_tr->vecN1_D;
}

#ifdef _cplusplus
}	// extern "C"{
#endif



