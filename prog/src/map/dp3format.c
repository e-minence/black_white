//=============================================================================
/**
 * @file	dp3format.h
 * @brief   �}�b�v�G�f�B�^�[�̏o�̓t�@�C����`
 * @author	ohno_katsunmi@gamefreak.co.jp
 * @date    2008.12.11
 */
//=============================================================================

#include <gflib.h>
#include "map/dp3format.h"


//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̖@���x�N�g���\���̃|�C���^�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval     PositionSt
 */
//-----------------------------------------------------------------------------

NormalVtxSt* DP3MAP_GetNormalVtxSt(const int index,const NormalVtxFormat* pNormalVtx)
{
    u8* pData = (u8*)pNormalVtx;
	return (NormalVtxSt*)(pData + sizeof(NormalVtxFormat) + index * sizeof(NormalVtxSt));
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̖@���x�N�g���𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @retval      none
 */
//-----------------------------------------------------------------------------

void DP3MAP_GetNormalVtxPosition(const int index,const NormalVtxFormat* pNormalVtx,VecFx16* vc1 ,VecFx16* vc2)
{
    NormalVtxSt* norVtxSt = DP3MAP_GetNormalVtxSt(index, pNormalVtx);

    GF_ASSERT((pNormalVtx->width*pNormalVtx->height) > index );
    VEC_Fx16Set( vc1, norVtxSt->vecN1_x, norVtxSt->vecN1_y, -norVtxSt->vecN1_z);
    VEC_Fx16Set( vc2, norVtxSt->vecN2_x, norVtxSt->vecN2_y, -norVtxSt->vecN2_z);

//    VEC_Fx16Set( vc1, norVtxSt[index].vecN1_x, norVtxSt[index].vecN1_y, -norVtxSt[index].vecN1_z);
//    VEC_Fx16Set( vc2, norVtxSt[index].vecN2_x, norVtxSt[index].vecN2_y, -norVtxSt[index].vecN2_z);
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̓��ϒl�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  ���ςP
 * @param[out]  ���ςQ
 * @retval      none
 */
//-----------------------------------------------------------------------------

void DP3MAP_GetNormalVtxInnerProduct(const int index,const NormalVtxFormat* pNormalVtx,fx16* ip1 ,fx16* ip2)
{
    NormalVtxSt* norVtxSt = DP3MAP_GetNormalVtxSt(index, pNormalVtx);

    GF_ASSERT((pNormalVtx->width*pNormalVtx->height) > index );
    *ip1 = norVtxSt->vecN1_D;
    *ip2 = norVtxSt->vecN2_D;

//    *ip1 = norVtxSt[index].vecN1_D;
//	  *ip2 = norVtxSt[index].vecN2_D;
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̃A�g���r���[�gBIT�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  ���ςP
 * @param[out]  ���ςQ
 * @retval      none
 */
//-----------------------------------------------------------------------------

u32 DP3MAP_GetNormalVtxAttrBit(const int index,const NormalVtxFormat* pNormalVtx)
{
    NormalVtxSt* norVtxSt = DP3MAP_GetNormalVtxSt(index, pNormalVtx);

    GF_ASSERT((pNormalVtx->width*pNormalVtx->height) > index );
	return norVtxSt->attr;	
//    return norVtxSt[index].attr;
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ�tryangleType�𓾂�
 * @param[in]   index    
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �O�p�`�̌`�̃^�C�v  �_ = 0  �^ = 1
 */
//-----------------------------------------------------------------------------

u32 DP3MAP_GetNormalVtxTriangleType(const int index,const NormalVtxFormat* pNormalVtx)
{
    NormalVtxSt* norVtxSt = DP3MAP_GetNormalVtxSt(index, pNormalVtx);

    GF_ASSERT((pNormalVtx->width*pNormalVtx->height) > index );
    return norVtxSt->tryangleType;

//    return norVtxSt[index].tryangleType;
}

//-----------------------------------------------------------------------------
/**
 * @brief       �^����ꂽ�ʒu�̖@���x�N�g���Ɠ��ς�Ԃ�
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[in]   posInBlock*   ������W
 * @param[in]   map_width     �l�`�o�̒���
 * @param[in]   map_height    �l�`�o�̍���
 * @param[out]  nomalOut      �@���x�N�g��
 * @retval      ����
 */
//-----------------------------------------------------------------------------

fx32 DP3MAP_GetNormalVector( const NormalVtxFormat* pNormalVtx, const VecFx32* posInBlock,
                             const fx32 map_width, const fx32 map_height, VecFx32* normalOut )
{
	fx32			grid_w, grid_x, grid_z;
	u32				grid_idx;
	VecFx32			pos, vecN;
	fx32			by, valD;
	NormalVtxSt*	nvs;
	u32				attrAdrs = (u32)pNormalVtx;

	VEC_Set( &pos, posInBlock->x + map_width/2, posInBlock->y, posInBlock->z + map_width/2 );
	//�O���b�h�����擾
	grid_w = map_width / pNormalVtx->width;	//�}�b�v�����O���b�h���ŕ���
	grid_idx = ( pos.z / grid_w ) * pNormalVtx->width + ( pos.x / grid_w );
	grid_x = pos.x % grid_w;
	grid_z = pos.z % grid_w;

	//���擾(���̎������Ⴄ�̂Ŗ@���x�N�g����Z���])
	nvs = (NormalVtxSt*)(attrAdrs + sizeof(NormalVtxFormat) + grid_idx * sizeof(NormalVtxSt));

	//�O���b�h���O�p�`�̔���
	if( nvs->tryangleType == 0 ){
		//0-2-1,3-1-2�̃p�^�[��
		if( grid_x + grid_z < grid_w ){
			VEC_Set( normalOut, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Set( normalOut, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	} else {
		//2-3-0,1-0-3�̃p�^�[��
		if( grid_x > grid_z ){
			VEC_Set( normalOut, nvs->vecN1_x, nvs->vecN1_y, -nvs->vecN1_z );
			valD = nvs->vecN1_D;
		} else {
			VEC_Set( normalOut, nvs->vecN2_x, nvs->vecN2_y, -nvs->vecN2_z );
			valD = nvs->vecN2_D;
		}
	}
    return valD;
}

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̔z�u�f�[�^�\���̃|�C���^�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval     PositionSt
 */
//-----------------------------------------------------------------------------

PositionSt* DP3MAP_GetLayoutPositionSt(const int index,const LayoutFormat* pLayout)
{
    PositionSt* objStatus = (PositionSt*)&pLayout->posData;
    GF_ASSERT(pLayout->count > index );
    return &objStatus[index];
}

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̔z�u�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @param[out]  VecFx32* �Ԃ� VecFx32
 * @retval  none
 */
//-----------------------------------------------------------------------------

void DP3MAP_GetLayoutPosition(const int index,const LayoutFormat* pLayout,VecFx32* vc )
{
    PositionSt* objStatus = (PositionSt*)&pLayout->posData;

    GF_ASSERT(pLayout->count > index );
    VEC_Set( vc, objStatus[index].xpos, objStatus[index].ypos, -objStatus[index].zpos );
}

//-----------------------------------------------------------------------------
/**
 * @brief      index�Ԗڂ̊p�x�𓾂�
 * @param[in]  index    
 * @param[in]  LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �p�x
 */
//-----------------------------------------------------------------------------

u16 DP3MAP_GetLayoutRotate(const int index,const LayoutFormat* pLayout )
{
    PositionSt* objStatus = (PositionSt*)&pLayout->posData;

    GF_ASSERT(pLayout->count > index );
    return objStatus[index].rotate;
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̃r���{�[�h�t���O�𓾂�
 * @param[in]   index
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      TRUE�Ȃ�r���{�[�h
 */
//-----------------------------------------------------------------------------

BOOL DP3MAP_GetLayoutBillboard(const int index,const LayoutFormat* pLayout )
{
    PositionSt* objStatus = (PositionSt*)&pLayout->posData;

    GF_ASSERT(pLayout->count > index );
    return objStatus[index].billboard;
}

//-----------------------------------------------------------------------------
/**
 * @brief       index�Ԗڂ̔z�u��ID�𓾂�
 * @param[in]   index
 * @param[in]   LayoutFormat* ���C�A�E�g�o�C�i���̐擪�|�C���^
 * @retval      �z�u��ID
 */
//-----------------------------------------------------------------------------

u8 DP3MAP_GetLayoutResourceID(const int index,const LayoutFormat* pLayout )
{
    PositionSt* objStatus = (PositionSt*)&pLayout->posData;

    GF_ASSERT(pLayout->count > index );
    return objStatus[index].resourceID;
}



