//=============================================================================================
/**
 * @file	g3d_light.c                                                  
 * @brief	���C�g�ݒ�
 * @date	
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
struct _GFL_G3D_LIGHTSET {
	GFL_G3D_LIGHT	light[4];
};
//=============================================================================================
/**
 *
 *
 * �Ǘ��V�X�e��
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ���C�g�Z�b�g�쐬
 *
 * @param	light		���C�g�ݒ�f�[�^�|�C���^
 * @param	lightCount	���C�g��
 *
 * @return	g3Dlightset	���C�g�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_LIGHTSET*
	GFL_G3D_LIGHT_Create
		( const GFL_G3D_LIGHTSET_SETUP* setUp, HEAPID heapID )
{
	GFL_G3D_LIGHTSET*	g3Dlightset;
	int	i;

	//�Ǘ��̈�m��
	g3Dlightset = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_LIGHTSET) );

	//���C�g�Z�b�g���擾�B�Z�b�g�A�b�v���color=0�͑��݂��Ȃ����C�g�Ƃ݂Ȃ��B
	for( i=0; i<setUp->lightCount; i++ ){
		g3Dlightset->light[ setUp->lightData[i].num ] = setUp->lightData[i].data;
	}
	return g3Dlightset;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�Z�b�g�j��
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LIGHT_Delete
		( GFL_G3D_LIGHTSET* g3Dlightset )
{
	GF_ASSERT( g3Dlightset );

	GFL_HEAP_FreeMemory( g3Dlightset );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�Z�b�g���f
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LIGHT_Switching
		( GFL_G3D_LIGHTSET* g3Dlightset )
{
	GFL_G3D_LIGHT initLight = {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, 0 };
	int	i;

	GF_ASSERT( g3Dlightset );

	//���C�g�ݒ�
	for( i=0; i<4; i++ ){
		if( g3Dlightset->light[i].color ){
			GFL_G3D_SetSystemLight( i, &g3Dlightset->light[i] );
		} else {
			GFL_G3D_SetSystemLight( i, &initLight );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�����̎擾�ƕύX
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 * @param	idx			���C�g�h�m�c�d�w
 * @param	vec			���C�g�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LIGHT_GetVec
		( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec )
{
	GF_ASSERT( g3Dlightset );
	*vec = g3Dlightset->light[ idx ].vec;
}

void
	GFL_G3D_LIGHT_SetVec
		( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec )
{
	GF_ASSERT( g3Dlightset );
	g3Dlightset->light[ idx ].vec = *vec;
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�F�̎擾�ƕύX
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 * @param	idx			���C�g�h�m�c�d�w
 * @param	color		���C�g�F�̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LIGHT_GetColor
		( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color )
{
	GF_ASSERT( g3Dlightset );
	*color = g3Dlightset->light[ idx ].color;
}

void
	GFL_G3D_LIGHT_SetColor
		( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color )
{
	GF_ASSERT( g3Dlightset );
	g3Dlightset->light[ idx ].color = *color;
}


