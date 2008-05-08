//============================================================================================
/**
 * @file	
 * @brief	
 * @date	
 */
//============================================================================================
typedef struct _GFL_BBD_SYS		GFL_BBD_SYS;

typedef enum {
	GFL_BBD_TEXFMT_PAL16 = 0,
	GFL_BBD_TEXFMT_PAL256,
	GFL_BBD_TEXFMT_PAL4,
	GFL_BBD_TEXFMT_ERROR,
}GFL_BBD_TEXFMT;

typedef enum {
	GFL_BBD_TEXSIZ_8x8 = 0,
	GFL_BBD_TEXSIZ_16x8,
	GFL_BBD_TEXSIZ_16x16,
	GFL_BBD_TEXSIZ_32x16,
	GFL_BBD_TEXSIZ_32x32,
	GFL_BBD_TEXSIZ_64x32,
	GFL_BBD_TEXSIZ_64x64,
	GFL_BBD_TEXSIZ_128x64,
	GFL_BBD_TEXSIZ_128x128,
	GFL_BBD_TEXSIZ_256x128,
	GFL_BBD_TEXSIZ_256x256,
	GFL_BBD_TEXSIZ_512x256,
	GFL_BBD_TEXSIZ_512x512,
	GFL_BBD_TEXSIZ_1024x512,
	GFL_BBD_TEXSIZ_1024x1024,
	GFL_BBD_TEXSIZ_32x512,
	GFL_BBD_TEXSIZ_32x1024,
	GFL_BBD_TEXSIZ_ERROR,
}GFL_BBD_TEXSIZ;

typedef struct {
	u16							resCountMax;
	u16							objCountMax;
	VecFx32						scale;
	GXRgb						diffuse;
    GXRgb						ambient;
	GXRgb						specular;
    GXRgb						emission;
	u8							polID;
}GFL_BBD_SETUP;

extern const GFL_BBD_SETUP defaultSetup;

#define GFL_BBD_NON_ALPHA	(31)
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

//------------------------------------------------------------------
//	�r���{�[�h���\�[�X�ǉ�
//		�P�j�A�[�J�C�u�h�c�ɂ��ǂݍ���(return resIdx)
extern int GFL_BBD_AddResourceArc( GFL_BBD_SYS* billboardSys, int arcID, int datID,
									u8 texFmt, u8 texSiz, u8 celSizX, u8 celSizY ); 
//		�Q�j�A�[�J�C�u�t�@�C���p�X�ɂ��ǂݍ���(return resIdx)
extern int GFL_BBD_AddResourcePath( GFL_BBD_SYS* billboardSys, const char* path, int datID,
									u8 texFmt, u8 texSiz, u8 celSizX, u8 cellSizY ); 
//	�r���{�[�h���\�[�X�j��
//		�P�j���\�[�X�h�c�w�ɂ��j��
extern void	GFL_BBD_RemoveResource( GFL_BBD_SYS* billboardSys, int resIdx );
//		�Q�j�S�j��
extern void	GFL_BBD_RemoveResourceAll( GFL_BBD_SYS* billboardSys );

//	�r���{�[�h�I�u�W�F�N�g�ǉ�  (return objIdx)
extern int	GFL_BBD_AddObject
		( GFL_BBD_SYS* billboardSys, int resIdx, const fx16 sizX, const fx16 sizY, 
			const VecFx32* trans, const u8 alpha );
//	�r���{�[�h�I�u�W�F�N�g�j��
extern void	GFL_BBD_RemoveObject( GFL_BBD_SYS* billboardSys, int objIdx );
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
extern void	GFL_BBD_SetObjectDrawSw
				( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* drawSw );
extern void	GFL_BBD_SetObjectFlipS
			( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipS );
extern void	GFL_BBD_SetObjectFlipT
			( GFL_BBD_SYS* billboardSys, int objIdx, const BOOL* flipT );

//------------------------------------------------------------------
/**
 *
 * @brief	�r���{�[�h�`��
 *
 */
//------------------------------------------------------------------
extern void	GFL_BBD_Draw( GFL_BBD_SYS* billboardSys, GFL_G3D_CAMERA* g3Dcamera );


//------------------------------------------------------------------
/**
 *
 * @brief	�e�X�g�֐�
 *
 */
//------------------------------------------------------------------
extern void GFL_BBD_TestObject( GFL_BBD_SYS* billboardSys, int objIdx );

