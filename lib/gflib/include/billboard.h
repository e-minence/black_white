//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		billboard.h
 *	@brief		�r���{�[�h�A�N�^�[�V�X�e��
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __BILLBOARD_H__
#define __BILLBOARD_H__

#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�e�N�X�`���t�H�[�}�b�g
//=====================================
typedef enum {
	GFL_BBD_TEXFMT_PAL16 = 0,	//16�F�p���b�g
	GFL_BBD_TEXFMT_PAL256,		//256�F�p���b�g
	GFL_BBD_TEXFMT_PAL4,		//4�F�p���b�g
	GFL_BBD_TEXFMT_ERROR,
}GFL_BBD_TEXFMT;

//-------------------------------------
///	�r���{�[�h�e�N�X�`���T�C�Y
//=====================================
typedef enum {
	GFL_BBD_TEXSIZ_8x8 = 0,
	GFL_BBD_TEXSIZ_8x16,
	GFL_BBD_TEXSIZ_8x32,
	GFL_BBD_TEXSIZ_8x64,
	GFL_BBD_TEXSIZ_8x128,
	GFL_BBD_TEXSIZ_8x256,
	GFL_BBD_TEXSIZ_8x512,
	GFL_BBD_TEXSIZ_8x1024,
	GFL_BBD_TEXSIZ_16x8,
	GFL_BBD_TEXSIZ_16x16,
	GFL_BBD_TEXSIZ_16x32,
	GFL_BBD_TEXSIZ_16x64,
	GFL_BBD_TEXSIZ_16x128,
	GFL_BBD_TEXSIZ_16x256,
	GFL_BBD_TEXSIZ_16x512,
	GFL_BBD_TEXSIZ_16x1024,
	GFL_BBD_TEXSIZ_32x8,
	GFL_BBD_TEXSIZ_32x16,
	GFL_BBD_TEXSIZ_32x32,
	GFL_BBD_TEXSIZ_32x64,
	GFL_BBD_TEXSIZ_32x128,
	GFL_BBD_TEXSIZ_32x256,
	GFL_BBD_TEXSIZ_32x512,
	GFL_BBD_TEXSIZ_32x1024,
	GFL_BBD_TEXSIZ_64x8,
	GFL_BBD_TEXSIZ_64x16,
	GFL_BBD_TEXSIZ_64x32,
	GFL_BBD_TEXSIZ_64x64,
	GFL_BBD_TEXSIZ_64x128,
	GFL_BBD_TEXSIZ_64x256,
	GFL_BBD_TEXSIZ_64x512,
	GFL_BBD_TEXSIZ_64x1024,
	GFL_BBD_TEXSIZ_128x8,
	GFL_BBD_TEXSIZ_128x16,
	GFL_BBD_TEXSIZ_128x32,
	GFL_BBD_TEXSIZ_128x64,
	GFL_BBD_TEXSIZ_128x128,
	GFL_BBD_TEXSIZ_128x256,
	GFL_BBD_TEXSIZ_128x512,
	GFL_BBD_TEXSIZ_128x1024,
	GFL_BBD_TEXSIZ_256x8,
	GFL_BBD_TEXSIZ_256x16,
	GFL_BBD_TEXSIZ_256x32,
	GFL_BBD_TEXSIZ_256x64,
	GFL_BBD_TEXSIZ_256x128,
	GFL_BBD_TEXSIZ_256x256,
	GFL_BBD_TEXSIZ_256x512,
	GFL_BBD_TEXSIZ_256x1024,
	GFL_BBD_TEXSIZ_512x8,
	GFL_BBD_TEXSIZ_512x16,
	GFL_BBD_TEXSIZ_512x32,
	GFL_BBD_TEXSIZ_512x64,
	GFL_BBD_TEXSIZ_512x128,
	GFL_BBD_TEXSIZ_512x256,
	GFL_BBD_TEXSIZ_512x512,
	GFL_BBD_TEXSIZ_512x1024,
	GFL_BBD_TEXSIZ_1024x8,
	GFL_BBD_TEXSIZ_1024x16,
	GFL_BBD_TEXSIZ_1024x32,
	GFL_BBD_TEXSIZ_1024x64,
	GFL_BBD_TEXSIZ_1024x128,
	GFL_BBD_TEXSIZ_1024x256,
	GFL_BBD_TEXSIZ_1024x512,
	GFL_BBD_TEXSIZ_1024x1024,
	GFL_BBD_TEXSIZ_ERROR,
}GFL_BBD_TEXSIZ;

//-------------------------------------
///	�r���{�[�h�e�N�X�`���T�C�Y define��`��
//=====================================
#define GFL_BBD_TEXSIZDEF_8x8 0
#define GFL_BBD_TEXSIZDEF_8x16 1
#define GFL_BBD_TEXSIZDEF_8x32 2
#define GFL_BBD_TEXSIZDEF_8x64 3
#define GFL_BBD_TEXSIZDEF_8x128 4
#define GFL_BBD_TEXSIZDEF_8x256 5
#define GFL_BBD_TEXSIZDEF_8x512 6
#define GFL_BBD_TEXSIZDEF_8x1024 7
#define GFL_BBD_TEXSIZDEF_16x8 8
#define GFL_BBD_TEXSIZDEF_16x16 9
#define GFL_BBD_TEXSIZDEF_16x32 10
#define GFL_BBD_TEXSIZDEF_16x64 11
#define GFL_BBD_TEXSIZDEF_16x128 12
#define GFL_BBD_TEXSIZDEF_16x256 13
#define GFL_BBD_TEXSIZDEF_16x512 14
#define GFL_BBD_TEXSIZDEF_16x1024 15
#define GFL_BBD_TEXSIZDEF_32x8 16
#define GFL_BBD_TEXSIZDEF_32x16 17
#define GFL_BBD_TEXSIZDEF_32x32 18
#define GFL_BBD_TEXSIZDEF_32x64 19
#define GFL_BBD_TEXSIZDEF_32x128 20
#define GFL_BBD_TEXSIZDEF_32x256 21
#define GFL_BBD_TEXSIZDEF_32x512 22
#define GFL_BBD_TEXSIZDEF_32x1024 23
#define GFL_BBD_TEXSIZDEF_64x8 24 
#define GFL_BBD_TEXSIZDEF_64x16 25
#define GFL_BBD_TEXSIZDEF_64x32 26
#define GFL_BBD_TEXSIZDEF_64x64 27
#define GFL_BBD_TEXSIZDEF_64x128 28
#define GFL_BBD_TEXSIZDEF_64x256 29
#define GFL_BBD_TEXSIZDEF_64x512 30
#define GFL_BBD_TEXSIZDEF_64x1024 31
#define GFL_BBD_TEXSIZDEF_128x8 32
#define GFL_BBD_TEXSIZDEF_128x16 33
#define GFL_BBD_TEXSIZDEF_128x32 34
#define GFL_BBD_TEXSIZDEF_128x64 35
#define GFL_BBD_TEXSIZDEF_128x128 36
#define GFL_BBD_TEXSIZDEF_128x256 37
#define GFL_BBD_TEXSIZDEF_128x512 38
#define GFL_BBD_TEXSIZDEF_128x1024 39
#define GFL_BBD_TEXSIZDEF_256x8 40
#define GFL_BBD_TEXSIZDEF_256x16 41
#define GFL_BBD_TEXSIZDEF_256x32 42
#define GFL_BBD_TEXSIZDEF_256x64 43
#define GFL_BBD_TEXSIZDEF_256x128 44
#define GFL_BBD_TEXSIZDEF_256x256 45
#define GFL_BBD_TEXSIZDEF_256x512 46
#define GFL_BBD_TEXSIZDEF_256x1024 47
#define GFL_BBD_TEXSIZDEF_512x8 48
#define GFL_BBD_TEXSIZDEF_512x16 49
#define GFL_BBD_TEXSIZDEF_512x32 50
#define GFL_BBD_TEXSIZDEF_512x64 51
#define GFL_BBD_TEXSIZDEF_512x128 52
#define GFL_BBD_TEXSIZDEF_512x256 53
#define GFL_BBD_TEXSIZDEF_512x512 54
#define GFL_BBD_TEXSIZDEF_512x1024 55
#define GFL_BBD_TEXSIZDEF_1024x8 56
#define GFL_BBD_TEXSIZDEF_1024x16 57
#define GFL_BBD_TEXSIZDEF_1024x32 58
#define GFL_BBD_TEXSIZDEF_1024x64 59
#define GFL_BBD_TEXSIZDEF_1024x128 60
#define GFL_BBD_TEXSIZDEF_1024x256 61
#define GFL_BBD_TEXSIZDEF_1024x512 62
#define GFL_BBD_TEXSIZDEF_1024x1024 63

//-------------------------------------
///	���C�g�}�X�N
//=====================================
typedef enum {
	GFL_BBD_LIGHT_NONE = 0,
	GFL_BBD_LIGHTMASK_0,	// = 1
	GFL_BBD_LIGHTMASK_1,	// = 2
	GFL_BBD_LIGHTMASK_01,
	GFL_BBD_LIGHTMASK_2,	// = 4
	GFL_BBD_LIGHTMASK_02,
	GFL_BBD_LIGHTMASK_12,
	GFL_BBD_LIGHTMASK_012,
	GFL_BBD_LIGHTMASK_3,	// = 8
	GFL_BBD_LIGHTMASK_03,
	GFL_BBD_LIGHTMASK_13,
	GFL_BBD_LIGHTMASK_013,
	GFL_BBD_LIGHTMASK_23,
	GFL_BBD_LIGHTMASK_023,
	GFL_BBD_LIGHTMASK_123,
	GFL_BBD_LIGHTMASK_0123,
}GFL_BBD_LIGHTMASK;

//-------------------------------------
///	�A���t�@�Ȃ���`
//=====================================
#define GFL_BBD_NON_ALPHA	(31)

//-------------------------------------
///	���_�^�C�v
//=====================================
typedef enum {
	GFL_BBD_ORIGIN_CENTER,
	GFL_BBD_ORIGIN_TOP,
	GFL_BBD_ORIGIN_BOTTOM,
	GFL_BBD_ORIGIN_LEFT,
	GFL_BBD_ORIGIN_RIGHT,

	GFL_BBD_ORIGIN_TOP_LEFT,
	GFL_BBD_ORIGIN_TOP_RIGHT,
	GFL_BBD_ORIGIN_BOTTOM_LEFT,
	GFL_BBD_ORIGIN_BOTTOM_RIGHT,

	GFL_BBD_ORIGIN_MAX,	// ���W���[�����Ŏg�p
}GFL_BBD_ORIGIN;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�r���{�[�h�V�X�e���s���S�^
//=====================================
typedef struct _GFL_BBD_SYS		GFL_BBD_SYS;

//-------------------------------------
///	�r���{�[�h�V�X�e���ݒ�p�\����
//=====================================
typedef struct {
	u16							resCountMax;
	u16							objCountMax;
	VecFx32						scale;
	GXRgb						diffuse;
    GXRgb						ambient;
	GXRgb						specular;
    GXRgb						emission;
	u8							polID;
	u8							origin;
}GFL_BBD_SETUP;

//-------------------------------------
///	�f�t�H���g�ݒ�p�\����
//=====================================
extern const GFL_BBD_SETUP defaultSetup;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *
 * @brief	�Z�b�g�A�b�v
 *
 */
//------------------------------------------------------------------
//	�r���{�[�h�V�X�e���쐬 (return billboardSys)
extern GFL_BBD_SYS*	GFL_BBD_CreateSys( const GFL_BBD_SETUP* setup, HEAPID heapID );

//	�r���{�[�h�V�X�e���j��
extern void	GFL_BBD_DeleteSys( GFL_BBD_SYS* billboardSys );

//	�r���{�[�h�V�X�e���e��p�����[�^�̎擾�ƕύX
extern void	GFL_BBD_GetScale( GFL_BBD_SYS* billboardSys, VecFx32* scale );
extern void	GFL_BBD_SetScale( GFL_BBD_SYS* billboardSys, VecFx32* scale );
extern void	GFL_BBD_GetDiffuse( GFL_BBD_SYS* billboardSys, GXRgb* diffuse );
extern void	GFL_BBD_SetDiffuse( GFL_BBD_SYS* billboardSys, GXRgb* diffuse );
extern void	GFL_BBD_GetAmbient( GFL_BBD_SYS* billboardSys, GXRgb* ambient );
extern void	GFL_BBD_SetAmbient( GFL_BBD_SYS* billboardSys, GXRgb* ambient );
extern void	GFL_BBD_GetSpecular( GFL_BBD_SYS* billboardSys, GXRgb* specular );
extern void	GFL_BBD_SetSpecular( GFL_BBD_SYS* billboardSys, GXRgb* specular );
extern void	GFL_BBD_GetEmission( GFL_BBD_SYS* billboardSys, GXRgb* emission );
extern void	GFL_BBD_SetEmission( GFL_BBD_SYS* billboardSys, GXRgb* emission );
extern void	GFL_BBD_GetPolID( GFL_BBD_SYS* billboardSys, u8* polID );
extern void	GFL_BBD_SetPolID( GFL_BBD_SYS* billboardSys, u8* polID );
extern void	GFL_BBD_GetOrigin( GFL_BBD_SYS* billboardSys, u8* origin );
extern void	GFL_BBD_SetOrigin( GFL_BBD_SYS* billboardSys, u8 origin  );

//GFL_BBD_TEXSIZ����S��T�̎����l���擾
extern void GFL_BBD_GetTexSize( GFL_BBD_TEXSIZ texSiz, u16 * s, u16 * t );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X
 */
//------------------------------------------------------------------
//	�r���{�[�h���\�[�X�ǉ�
//		�P�j�ǂݍ��ݍς݂̃��\�[�X��o�^(return resIdx)
extern int GFL_BBD_AddResource( GFL_BBD_SYS* billboardSys, GFL_G3D_RES* g3DresTex,
									u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ); 
//		�Q�j�A�[�J�C�u�h�c�ɂ��ǂݍ���(return resIdx)
extern int GFL_BBD_AddResourceArc( GFL_BBD_SYS* billboardSys, int arcID, int datID,
									u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ); 
//		�R�j�A�[�J�C�u�t�@�C���p�X�ɂ��ǂݍ���(return resIdx)
extern int GFL_BBD_AddResourcePath( GFL_BBD_SYS* billboardSys, const char* path, int datID,
									u8 texFmt, u8 texSiz, u8 celSizX, u8 cellSizY ); 
//	�r���{�[�h���\�[�X�j��
//		�P�j���\�[�X�h�c�w�ɂ��j��
extern void	GFL_BBD_RemoveResource( GFL_BBD_SYS* billboardSys, int resIdx );
//		�Q�j�S�j��
extern void	GFL_BBD_RemoveResourceAll( GFL_BBD_SYS* billboardSys );

//	�r���{�[�h���\�[�X�f�[�^�|�C���^�擾
extern u32	GFL_BBD_GetResourceData( GFL_BBD_SYS* billboardSys, int resIdx );
//	�r���{�[�h���\�[�X�f�[�^�J�b�g
extern void	GFL_BBD_CutResourceData( GFL_BBD_SYS* billboardSys, int resIdx );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h���\�[�X�e��p�����[�^�̎擾
 */
//------------------------------------------------------------------
//texDataAdrs
extern void	GFL_BBD_GetResourceTexDataAdrs
				( GFL_BBD_SYS* billboardSys, int resIdx, u32* texDataAdrs );
//texPlttAdrs
extern void	GFL_BBD_GetResourceTexPlttAdrs
				( GFL_BBD_SYS* billboardSys, int resIdx, u32* texPlttAdrs );
//cel�v�Z
enum {
	CEL_OFFS_1D = 0,
	CEL_OFFS_2D,
};
extern void	GFL_BBD_GetResourceCelOffset( GFL_BBD_SYS* billboardSys, int resIdx, 
								const u16 celIdx, u32* dataOffs, u32* celDataSiz, u8 type );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�I�u�W�F�N�g
 */
//------------------------------------------------------------------
//	�r���{�[�h�I�u�W�F�N�g�ǉ�  (return objIdx)
extern int	GFL_BBD_AddObject
		( GFL_BBD_SYS* billboardSys, int resIdx, const fx16 sizX, const fx16 sizY, 
			const VecFx32* trans, const u8 alpha, const GFL_BBD_LIGHTMASK lightMask );
//	�r���{�[�h�I�u�W�F�N�g�j��
extern void	GFL_BBD_RemoveObject( GFL_BBD_SYS* billboardSys, int objIdx );
//	�r���{�[�h�I�u�W�F�N�g�j�� Vram�̂�
extern void	GFL_BBD_RemoveResourceVram( GFL_BBD_SYS* billboardSys, int resIdx );

//------------------------------------------------------------------
/**
 * @brief	�r���{�[�h�I�u�W�F�N�g�e��p�����[�^�̎擾�ƕύX
 */
//------------------------------------------------------------------
extern void	GFL_BBD_GetObjectResIdx
				( GFL_BBD_SYS* billboardSys, int objIdx, u16* resIdx );
extern void	GFL_BBD_SetObjectResIdx
				( GFL_BBD_SYS* billboardSys, int objIdx, const u16* resIdx );
extern void	GFL_BBD_GetObjectTrans
				( GFL_BBD_SYS* billboardSys, int objIdx, VecFx32* trans );
extern void	GFL_BBD_SetObjectTrans
				( GFL_BBD_SYS* billboardSys, int objIdx, const VecFx32* trans );
extern void	GFL_BBD_GetObjectCelIdx
				( GFL_BBD_SYS* billboardSys, int objIdx, u16* celIdx );
extern void	GFL_BBD_SetObjectCelIdx
				( GFL_BBD_SYS* billboardSys, int objIdx, const u16* celIdx );
extern void	GFL_BBD_GetObjectSiz
				( GFL_BBD_SYS* billboardSys, int objIdx, fx16* sizX, fx16* sizY );
extern void	GFL_BBD_SetObjectSiz
				( GFL_BBD_SYS* billboardSys, int objIdx, const fx16* sizX, const fx16* sizY );
extern void	GFL_BBD_GetObjectAlpha
				( GFL_BBD_SYS* billboardSys, int objIdx, u8* alpha );
extern void	GFL_BBD_SetObjectAlpha
				( GFL_BBD_SYS* billboardSys, int objIdx, const u8* alpha );
extern BOOL	GFL_BBD_GetObjectDrawEnable
				( const GFL_BBD_SYS* billboardSys, int objIdx );
extern void	GFL_BBD_SetObjectDrawEnable
				( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* drawEnable );
extern GFL_BBD_LIGHTMASK	GFL_BBD_GetObjectLightMask
				( const GFL_BBD_SYS* billboardSys, int objIdx );
extern void	GFL_BBD_SetObjectLightMask
				( GFL_BBD_SYS* billboardSys, int objIdx, const GFL_BBD_LIGHTMASK* lightMask );
extern void	GFL_BBD_OnObjectLightMask
				( GFL_BBD_SYS* billboardSys, int objIdx, GFL_BBD_LIGHTMASK lightMask );
extern void	GFL_BBD_OffObjectLightMask
				( GFL_BBD_SYS* billboardSys, int objIdx, GFL_BBD_LIGHTMASK lightMask );
extern BOOL	GFL_BBD_GetObjectFlipS
				( const GFL_BBD_SYS* billboardSys, int objIdx );
extern void	GFL_BBD_SetObjectFlipS
				( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipS );
extern BOOL	GFL_BBD_GetObjectFlipT
				( const GFL_BBD_SYS* billboardSys, int objIdx );
extern void	GFL_BBD_SetObjectFlipT
				( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipT );
extern void	GFL_BBD_GetObjectRotate
				( GFL_BBD_SYS* billboardSys, int objIdx, u16 *rotZ );
extern void	GFL_BBD_SetObjectRotate
				( GFL_BBD_SYS* billboardSys, int objIdx, const u16* rotZ );

//------------------------------------------------------------------
/**
 *
 * @brief	�r���{�[�h�`��
 *
 */
//------------------------------------------------------------------
extern void	GFL_BBD_Draw
		( GFL_BBD_SYS* billboardSys, GFL_G3D_CAMERA* g3Dcamera, GFL_G3D_LIGHTSET* g3Dlightset );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif		// __BILLBOARD_H__
