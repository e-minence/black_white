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
struct _GFL_G3D_LIGHTING {
	u16			num;
	u16			color;
	VecFx16		vec;
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
 * ���C�g�쐬
 *
 * @param	num			�g�p���C�g�i���o�[
 * @param	color		���C�g�̐F
 * @param	vec			���C�g�����x�N�g��
 *
 * @return	g3Dlight	���C�g�n���h��
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_LIGHTING*
	GFL_G3D_LightCreate
		( const u16 num, const u16 color, const VecFx16* vec, HEAPID heapID )
{
	GFL_G3D_LIGHTING*	g3Dlight;
	//�Ǘ��̈�m��
	g3Dlight = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_LIGHTING) );

	g3Dlight->num	= num;
	g3Dlight->color	= color;
	g3Dlight->vec	= *vec;

	return g3Dlight;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����j��
 *
 * @param	g3Dlight	���C�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LightDelete
		( GFL_G3D_LIGHTING* g3Dlight )
{
	GF_ASSERT( g3Dlight );

	GFL_HEAP_FreeMemory( g3Dlight );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g���f
 *
 * @param	g3Dlight	���C�g�n���h��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LightSwitching
		( GFL_G3D_LIGHTING* g3Dlight )
{
	GF_ASSERT( g3Dlight );

	//���C�g�ݒ�
	GFL_G3D_sysLightSet( g3Dlight->num, &g3Dlight->vec, g3Dlight->color );
}

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�����̎擾�ƕύX
 *
 * @param	g3Dlight	���C�g�n���h��
 * @param	vec			���C�g�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LightVecGet
		( GFL_G3D_LIGHTING* g3Dlight, VecFx16* vec )
{
	GF_ASSERT( g3Dlight );
	*vec = g3Dlight->vec;
}

void
	GFL_G3D_LightVecSet
		( GFL_G3D_LIGHTING* g3Dlight, VecFx16* vec )
{
	GF_ASSERT( g3Dlight );
	g3Dlight->vec = *vec;
}

//--------------------------------------------------------------------------------------------
/**
 * �J�����F�̎擾�ƕύX
 *
 * @param	g3Dlight	���C�g�n���h��
 * @param	color		���C�g�F�̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_LightColorGet
		( GFL_G3D_LIGHTING* g3Dlight, u16* color )
{
	GF_ASSERT( g3Dlight );
	*color = g3Dlight->color;
}

void
	GFL_G3D_LightColorSet
		( GFL_G3D_LIGHTING* g3Dlight, u16* color )
{
	GF_ASSERT( g3Dlight );
	g3Dlight->color = *color;
}


