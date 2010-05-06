//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_g3d_map.h
 *	@brief  g3d_map�@WB�o�[�W����
 *	@author	game freak inc.
 *	@date		2010.03.01
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

//#define	MAPOBJ_SIZE		(0x18000) 	//�u���b�N���I�u�W�F�N�g���f���f�[�^�p�������m�ۃT�C�Y 
//#define	MAPOBJTEX_SIZE	(0x4000) 	//�u���b�N���I�u�W�F�N�g�e�N�X�`���f�[�^�p�u�q�`�l�m�ۃT�C�Y 
#define	FLD_G3D_MAP_MAPPLTT_SIZE	(0x200) 	//�e�N�X�`���p���b�g�p�u�q�`�l�m�ۃT�C�Y(�O������T�C�Y�w��ł���悤�ɏC���\��...) 
#define	FLD_G3D_MAP_MAPLOAD_SIZE	(0x800)		//�����f�[�^���[�h�T�C�Y(ROM��RAM) 
#define	FLD_G3D_MAP_MAPTRANS_SIZE	(0x2000) 	//�����f�[�^�]���T�C�Y(RAM��VRAM) 


#define FLD_G3D_MAP_ATTR_GETMAX	(16)	//�A�g���r���[�g�擾�ő吔

//------------------------------------------------------------------
//�V�X�e���n���h����`
typedef struct _FLD_G3D_MAP FLD_G3D_MAP;

//------------------------------------------------------------------
//�`��^�C�v��`
enum {
	FLD_G3D_MAP_DRAW_NORMAL = 0,
	FLD_G3D_MAP_DRAW_YBILLBOARD,
};
//------------------------------------------------------------------
//���C�g�g�p��`
enum {
	FLD_G3D_MAP_LIGHT_NONE = 0,
	FLD_G3D_MAP_LIGHT0_ON = 1,
	FLD_G3D_MAP_LIGHT1_ON = 2,
	FLD_G3D_MAP_LIGHT2_ON = 4,
	FLD_G3D_MAP_LIGHT3_ON = 8,
};
//------------------------------------------------------------------
//�ǂݍ��݃X�e�[�^�X��`
typedef struct {
	u16			seq;
	u16			loadCount;
	u32			mOffs;
	u32			fOffs;
	u32			fSize;
	u32			tOffs;
	u32			tSize;

	BOOL		mdlLoaded;
	BOOL		texLoaded;
	BOOL		attrLoaded;
}FLD_G3D_MAP_LOAD_STATUS;
//------------------------------------------------------------------
//�ǂݍ��݃V�[�P���X��`
enum {
	FLD_G3D_MAP_LOAD_IDLING = 0,
	FLD_G3D_MAP_LOAD_START,
};
//------------------------------------------------------------------
//�W�I���g�����ڏ������݃I�u�W�F�N�g�֐���`
typedef void (FLD_G3D_MAP_DDOBJ_FUNC)
					( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecView, BOOL lodSt );
//------------------------------------------------------------------
//�W�I���g�����ڏ������݃I�u�W�F�N�g�p�����[�^��`
typedef struct {
	u8						lightMask;
	GXRgb					diffuse;
    GXRgb					ambient;
	GXRgb					specular;
    GXRgb					emission;
	u8						polID;
	u8						alpha;
	u8						drawType;	
	int						scaleVal;
	FLD_G3D_MAP_DDOBJ_FUNC*	func;
}FLD_G3D_MAP_DDOBJ_DATA;
//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�ݒ��`
typedef struct {
	GFL_G3D_OBJ*	g3DobjHQ;	//�I�u�W�F�N�g(High Q)
	GFL_G3D_OBJ*	g3DobjLQ;	//�I�u�W�F�N�g(Low Q)
}FLD_G3D_MAP_OBJ;
//------------------------------------------------------------------
//�W�I���g�����ڏ������݃I�u�W�F�N�g�ݒ��`
typedef struct {
	GFL_G3D_RES*			g3Dres;
	u32						texDataAdrs;
	u32						texPlttAdrs;
	const FLD_G3D_MAP_DDOBJ_DATA*	data;
}FLD_G3D_MAP_DDOBJ;
//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�Ǘ��ݒ��`
#define	GLOBALOBJ_SET_ERROR		(0xffffffff)
typedef struct {
	FLD_G3D_MAP_OBJ*	gobj;
	u32					gobjCount;
	u16*				gobjIDexchange;
	FLD_G3D_MAP_DDOBJ*	gddobj;
	u32					gddobjCount;
	u16*				gddobjIDexchange;
}FLD_G3D_MAP_GLOBALOBJ;

//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�z�u�ݒ��`
typedef struct {
	u32			id;
	VecFx32		trans;
	u16			rotate;
}FLD_G3D_MAP_GLOBALOBJ_ST;

//------------------------------------------------------------------
//�A�g���r���[�g��`
typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}FLD_G3D_MAP_ATTR;

//�A�g���r���[�g����`
typedef struct {
	FLD_G3D_MAP_ATTR	mapAttr[FLD_G3D_MAP_ATTR_GETMAX];
	u32					mapAttrCount;
}FLD_G3D_MAP_ATTRINFO;

//------------------------------------------------------------------
//�}�b�v�f�[�^�t�@�C�������֐���`
typedef BOOL (FLD_G3D_MAP_FILELOAD_FUNC)( FLD_G3D_MAP* g3Dmap, void * exWork );
typedef void (FLD_G3D_MAP_FILEATTR_FUNC)( FLD_G3D_MAP_ATTRINFO* attrInfo,
										const void* mapdata, const VecFx32* posInBlock, 
										const fx32 map_width, const fx32 map_height );
//------------------------------------------------------------------
//�}�b�v�f�[�^�t�@�C�������֐��e�[�u����`
#define MAPFILE_FUNC_DEFAULT	(0xffffffff)	//�e�[�u���̍Ō�ɋL�q�ݒ肵�Ă�������
typedef struct {
	u32							signature;
	FLD_G3D_MAP_FILELOAD_FUNC*	loadFunc; 
	FLD_G3D_MAP_FILEATTR_FUNC*	attrFunc; 
	FLD_G3D_MAP_FILEATTR_FUNC*	attrFuncForEffEnc;  //�G�t�F�N�g�G���J�E���g��p 
}FLD_G3D_MAP_FILE_FUNC;
//------------------------------------------------------------------
//�Z�b�g�A�b�v��`
typedef struct {
	u32								mapDataHeapSize;
	u32								texVramSize;
	const FLD_G3D_MAP_FILE_FUNC*	mapFileFunc;
	void *						externalWork;
  u32               mapLoadSize;

  u16               obj_count;      // �z�u�I�u�W�F�Ǘ���
  u16               ddobj_count;    // DirectDraw�z�u�I�u�W�F�Ǘ���
  fx32              obj_draw_limit; // �`��͈�
  fx32              obj_lod_limit;  // HighQuallity(LOD)�p�`��͈�
  
}FLD_G3D_MAP_SETUP;

//============================================================================================
/**
 * @brief	��{�V�X�e��
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���쐬
 */
//------------------------------------------------------------------
extern FLD_G3D_MAP*	FLD_G3D_MAP_Create( const FLD_G3D_MAP_SETUP* setup, HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���j��
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_Delete( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���C��
 *				���f�[�^�ǂݍ���
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_Main( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��
 *
 *		ex)
 *		{
 *			FLD_G3D_MAP_StartDraw();	//�`��J�n
 *
 *			for( i=0; i<g3Dmap_count; i++ ){
 *				FLD_G3D_MAP_Draw( ...... );		//�e�}�b�v�`��֐�
 *			
 *			FLD_G3D_MAP_EndDraw();		//�`��I��
 *		}
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_StartDraw( void );
extern BOOL	FLD_G3D_MAP_Draw( FLD_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera );
extern void	FLD_G3D_MAP_EndDraw( void );


//============================================================================================
/**
 * @brief	�}�b�v�R���g���[���֐�
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�A�[�J�C�u�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistArc( FLD_G3D_MAP* g3Dmap, const u32 arcID, HEAPID heapID );
extern void	FLD_G3D_MAP_ReleaseArc( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���e�N�X�`�����\�[�X�o�^
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistGlobalTexResource
					( FLD_G3D_MAP* g3Dmap, GFL_G3D_RES* globalResTex );
extern void	FLD_G3D_MAP_ReleaseGlobalTexResource( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g���\�[�X�o�^
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistGlobalObjResource
					( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_GLOBALOBJ* globalObj );
extern void	FLD_G3D_MAP_ReleaseGlobalObjResource( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g�z�u�o�^
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistGlobalObj
					( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST* status, u32 idx );
extern u32	FLD_G3D_MAP_ResistAutoGlobalObj
					( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST* status );
extern void	FLD_G3D_MAP_ReleaseGlobalObj( FLD_G3D_MAP* g3Dmap, u32 idx );
extern FLD_G3D_MAP_GLOBALOBJ_ST * FLD_G3D_MAP_GetGlobalObj
          ( FLD_G3D_MAP* g3Dmap, u32 idx );

//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistGlobalDDobj
					( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST* status, u32 idx );
extern u32	FLD_G3D_MAP_ResistAutoGlobalDDobj
					( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_GLOBALOBJ_ST* status );
extern void	FLD_G3D_MAP_ReleaseGlobalDDobj( FLD_G3D_MAP* g3Dmap, u32 idx );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g�h�c�ϊ�
 */
//------------------------------------------------------------------
extern BOOL	FLD_G3D_MAP_GetGlobalObjectID
					( FLD_G3D_MAP* g3Dmap, const u32 datID, u32* exchangeID );
extern BOOL	FLD_G3D_MAP_GetGlobalDDobjectID
					( FLD_G3D_MAP* g3Dmap, const u32 datID, u32* exchangeID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_SetLoadReq( FLD_G3D_MAP* g3Dmap, const u32 datID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h��~
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_ResetLoadData( FLD_G3D_MAP* g3Dmap );


//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��n�m�^�n�e�e���擾
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_SetDrawSw( FLD_G3D_MAP* g3Dmap, BOOL drawSw );
extern BOOL	FLD_G3D_MAP_GetDrawSw( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�ʒu�ݒ聕�擾
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_SetTrans( FLD_G3D_MAP* g3Dmap, const VecFx32* trans );
extern void	FLD_G3D_MAP_GetTrans( FLD_G3D_MAP* g3Dmap, VecFx32* trans );
//------------------------------------------------------------------
/**
 * @brief	�A�g���r���[�g�擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_InitAttr( FLD_G3D_MAP_ATTRINFO* attrInfo );
extern BOOL FLD_G3D_MAP_IsAttrEnable( FLD_G3D_MAP* g3Dmap );
extern void FLD_G3D_MAP_GetAttr( FLD_G3D_MAP_ATTRINFO* attrInfo,
								FLD_G3D_MAP* g3Dmap, const VecFx32* pos, const fx32 map_width );
extern void FLD_G3D_MAP_GetAttrForEffectEncount( FLD_G3D_MAP_ATTRINFO* attrInfo, FLD_G3D_MAP* g3Dmap,
							const VecFx32* pos, const fx32 map_width );


//============================================================================================
/**
 * @brief	�}�b�v�f�[�^�t�@�C�������p�֐�
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���[�h�X�e�[�^�X�|�C���^�擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_GetLoadStatusPointer( FLD_G3D_MAP* g3Dmap, FLD_G3D_MAP_LOAD_STATUS** ldst );
//------------------------------------------------------------------
/**
 * @brief	���[�h�X�e�[�^�X������
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_ResetLoadStatus( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	���[�h�������|�C���^�擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_GetLoadMemoryPointer( FLD_G3D_MAP* g3Dmap, void** mem );
//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�n���h���擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_GetLoadArcHandle( FLD_G3D_MAP* g3Dmap, ARCHANDLE** handle );
//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�����[�h�f�[�^�h�c�擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_GetLoadDatID( FLD_G3D_MAP* g3Dmap, u32* ID );
//------------------------------------------------------------------
/**
 * @brief	Lord�f�[�^�T�C�Y�̎擾
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_GetLoadDataSize( FLD_G3D_MAP* g3Dmap, u32* size );
//------------------------------------------------------------------
/**
 * @brief	���f�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_CreateResourceMdl( FLD_G3D_MAP* g3Dmap, void* mem );
extern void FLD_G3D_MAP_DeleteResourceMdl( FLD_G3D_MAP* g3Dmap );
extern const GFL_G3D_RES* FLD_G3D_MAP_GetResourceMdl( const FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�e�N�X�`�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_CreateResourceTex( FLD_G3D_MAP* g3Dmap, void* mem );
extern void FLD_G3D_MAP_DeleteResourceTex( FLD_G3D_MAP* g3Dmap );
extern const GFL_G3D_RES* FLD_G3D_MAP_GetResourceTex( const FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�����_�[�쐬
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_MakeRenderObj( FLD_G3D_MAP* g3Dmap );
extern NNSG3dRenderObj* FLD_G3D_MAP_GetRenderObj( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�t�@�C���ǂݍ��݊֐�
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_StartFileLoad( FLD_G3D_MAP* g3Dmap, u32 datID );
extern BOOL FLD_G3D_MAP_ContinueFileLoad( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	���[�J���e�N�X�`��VRAM�]���֐�(�e�N�X�`���ǂݍ��ݏI����ɍs��)
 */
//------------------------------------------------------------------
extern void FLD_G3D_MAP_SetTransVramParam( FLD_G3D_MAP* g3Dmap );
extern BOOL FLD_G3D_MAP_TransVram( FLD_G3D_MAP* g3Dmap );


//============================================================================================
/**
 * @brief	�e�X�g�֐�
 */
//============================================================================================
extern void FLD_G3D_MAP_MakeTestPos( FLD_G3D_MAP* g3Dmap );

#ifdef __cplusplus
}/* extern "C" */
#endif

