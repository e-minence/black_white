//=============================================================================================
/**
 * @file	g3d_light.h
 * @brief	���C�g�ݒ�
 * @date	
 */
//=============================================================================================
#ifndef _G3D_LIGHT_H_
#define _G3D_LIGHT_H_

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef struct _GFL_G3D_LIGHTSET	GFL_G3D_LIGHTSET;

typedef struct {
	const u8				num;
	const GFL_G3D_LIGHT		data;
}GFL_G3D_LIGHT_DATA;

typedef struct {
	const GFL_G3D_LIGHT_DATA*	lightData;
	const u8					lightCount;
}GFL_G3D_LIGHTSET_SETUP;

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
extern GFL_G3D_LIGHTSET*
	GFL_G3D_LIGHT_Create
		( const GFL_G3D_LIGHTSET_SETUP* setUp, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�Z�b�g�j��
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_LIGHT_Delete
		( GFL_G3D_LIGHTSET* g3Dlightset );

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�Z�b�g���f
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 */
//--------------------------------------------------------------------------------------------
extern void
	GFL_G3D_LIGHT_Switching
		( GFL_G3D_LIGHTSET* g3Dlightset );

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�����̎擾�ƕύX
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 * @param	idx			���C�g�h�m�c�d�w
 * @param	vec			���C�g�x�N�g���̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_LIGHT_GetVec( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec );
extern void GFL_G3D_LIGHT_SetVec( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, VecFx16* vec );

//--------------------------------------------------------------------------------------------
/**
 * ���C�g�F�̎擾�ƕύX
 *
 * @param	g3Dlightset	���C�g�Z�b�g�n���h��
 * @param	idx			���C�g�h�m�c�d�w
 * @param	color		���C�g�F�̊i�[�������͎Q�ƃ��[�N�|�C���^	
 */
//--------------------------------------------------------------------------------------------
extern void GFL_G3D_LIGHT_GetColor( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color );
extern void GFL_G3D_LIGHT_SetColor( GFL_G3D_LIGHTSET* g3Dlightset, u8 idx, u16* color );

#endif


