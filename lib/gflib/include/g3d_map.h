#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define	MAPPLTT_SIZE	(0x200) 	//�e�N�X�`���p���b�g�p�u�q�`�l�m�ۃT�C�Y 
//#define	MAPOBJ_SIZE		(0x18000) 	//�u���b�N���I�u�W�F�N�g���f���f�[�^�p�������m�ۃT�C�Y 
//#define	MAPOBJTEX_SIZE	(0x4000) 	//�u���b�N���I�u�W�F�N�g�e�N�X�`���f�[�^�p�u�q�`�l�m�ۃT�C�Y 
#define	MAPLOAD_SIZE	(0x800)		//�����f�[�^���[�h�T�C�Y(ROM��RAM) 
#define	MAPTRANS_SIZE	(0x2000) 	//�����f�[�^�]���T�C�Y(RAM��VRAM) 

#define MAP_GRIDCOUNT	(32)		//�}�b�v���c���O���b�h��

#define DRAW_LIMIT	(1024 * FX32_ONE)		//�`��͈�
#define LOD_LIMIT	(512 * FX32_ONE)		//HighQuallity(LOD)�p�`��͈�

#define	NON_TEX		(0xffff)
#define	NON_ATTR	(0xffff)

#define GFL_G3D_MAP_ATTR_GETMAX	(16)	//�A�g���r���[�g�擾�ő吔
//------------------------------------------------------------------
//�V�X�e���n���h����`
typedef struct _GFL_G3D_MAP GFL_G3D_MAP;

//------------------------------------------------------------------
//�`��^�C�v��`
enum {
	GFL_G3D_MAP_DRAW_NORMAL = 0,
	GFL_G3D_MAP_DRAW_YBILLBOARD,
};
//------------------------------------------------------------------
//���C�g�g�p��`
enum {
	GFL_G3D_MAP_LIGHT_NONE = 0,
	GFL_G3D_MAP_LIGHT0_ON = 1,
	GFL_G3D_MAP_LIGHT1_ON = 2,
	GFL_G3D_MAP_LIGHT2_ON = 4,
	GFL_G3D_MAP_LIGHT3_ON = 8,
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
}GFL_G3D_MAP_LOAD_STATUS;
//------------------------------------------------------------------
//�ǂݍ��݃V�[�P���X��`
enum {
	GFL_G3D_MAP_LOAD_IDLING = 0,
	GFL_G3D_MAP_LOAD_START,
};
//------------------------------------------------------------------
//�W�I���g�����ڏ������݃I�u�W�F�N�g�֐���`
typedef void (GFL_G3D_MAP_DDOBJ_FUNC)
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
	GFL_G3D_MAP_DDOBJ_FUNC*	func;
}GFL_G3D_MAP_DDOBJ_DATA;
//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�ݒ��`
typedef struct {
	GFL_G3D_OBJ*	g3DobjHQ;	//�I�u�W�F�N�g(High Q)
	GFL_G3D_OBJ*	g3DobjLQ;	//�I�u�W�F�N�g(Low Q)
}GFL_G3D_MAP_OBJ;
//------------------------------------------------------------------
//�W�I���g�����ڏ������݃I�u�W�F�N�g�ݒ��`
typedef struct {
	GFL_G3D_RES*			g3Dres;
	u32						texDataAdrs;
	u32						texPlttAdrs;
	const GFL_G3D_MAP_DDOBJ_DATA*	data;
}GFL_G3D_MAP_DDOBJ;
//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�Ǘ��ݒ��`
#define	GLOBALOBJ_SET_ERROR		(0xffffffff)
typedef struct {
	GFL_G3D_MAP_OBJ*	gobj;
	u32					gobjCount;
	u16*				gobjIDexchange;
	GFL_G3D_MAP_DDOBJ*	gddobj;
	u32					gddobjCount;
	u16*				gddobjIDexchange;
}GFL_G3D_MAP_GLOBALOBJ;

//------------------------------------------------------------------
//�O���[�o���I�u�W�F�N�g�z�u�ݒ��`
typedef struct {
	u32			id;
	VecFx32		trans;
	u16			rotate;
}GFL_G3D_MAP_GLOBALOBJ_ST;

//------------------------------------------------------------------
//�A�g���r���[�g��`
typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}GFL_G3D_MAP_ATTR;

//�A�g���r���[�g����`
typedef struct {
	GFL_G3D_MAP_ATTR	mapAttr[GFL_G3D_MAP_ATTR_GETMAX];
	u32					mapAttrCount;
}GFL_G3D_MAP_ATTRINFO;

//------------------------------------------------------------------
//�}�b�v�f�[�^�t�@�C�������֐���`
typedef BOOL (GFL_G3D_MAP_FILELOAD_FUNC)( GFL_G3D_MAP* g3Dmap, void * exWork );
typedef void (GFL_G3D_MAP_FILEATTR_FUNC)( GFL_G3D_MAP_ATTRINFO* attrInfo,
										const void* mapdata, const VecFx32* posInBlock, 
										const fx32 map_width, const fx32 map_height );
//------------------------------------------------------------------
//�}�b�v�f�[�^�t�@�C�������֐��e�[�u����`
#define MAPFILE_FUNC_DEFAULT	(0xffffffff)	//�e�[�u���̍Ō�ɋL�q�ݒ肵�Ă�������
typedef struct {
	u32							signature;
	GFL_G3D_MAP_FILELOAD_FUNC*	loadFunc; 
	GFL_G3D_MAP_FILEATTR_FUNC*	attrFunc; 
}GFL_G3D_MAP_FILE_FUNC;
//------------------------------------------------------------------
//�Z�b�g�A�b�v��`
typedef struct {
	u32								mapDataHeapSize;
	u32								texVramSize;
	const GFL_G3D_MAP_FILE_FUNC*	mapFileFunc;
	void *						externalWork;

}GFL_G3D_MAP_SETUP;

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
extern GFL_G3D_MAP*	GFL_G3D_MAP_Create( const GFL_G3D_MAP_SETUP* setup, HEAPID heapID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���j��
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_Delete( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���C��
 *				���f�[�^�ǂݍ���
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_Main( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��
 *
 *		ex)
 *		{
 *			GFL_G3D_MAP_StartDraw();	//�`��J�n
 *
 *			for( i=0; i<g3Dmap_count; i++ ){
 *				GFL_G3D_MAP_Draw( ...... );		//�e�}�b�v�`��֐�
 *			
 *			GFL_G3D_MAP_EndDraw();		//�`��I��
 *		}
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_StartDraw( void );
extern void	GFL_G3D_MAP_Draw( GFL_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera );
extern void	GFL_G3D_MAP_EndDraw( void );


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
extern void	GFL_G3D_MAP_ResistArc( GFL_G3D_MAP* g3Dmap, const u32 arcID, HEAPID heapID );
extern void	GFL_G3D_MAP_ReleaseArc( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���e�N�X�`�����\�[�X�o�^
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_ResistGlobalTexResource
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_RES* globalResTex );
extern void	GFL_G3D_MAP_ReleaseGlobalTexResource( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g���\�[�X�o�^
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_ResistGlobalObjResource
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_GLOBALOBJ* globalObj );
extern void	GFL_G3D_MAP_ReleaseGlobalObjResource( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g�z�u�o�^
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_ResistGlobalObj
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST* status, u32 idx );
extern u32	GFL_G3D_MAP_ResistAutoGlobalObj
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST* status );
extern void	GFL_G3D_MAP_ReleaseGlobalObj( GFL_G3D_MAP* g3Dmap, u32 idx );
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_ResistGlobalDDobj
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST* status, u32 idx );
extern u32	GFL_G3D_MAP_ResistAutoGlobalDDobj
					( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_GLOBALOBJ_ST* status );
extern void	GFL_G3D_MAP_ReleaseGlobalDDobj( GFL_G3D_MAP* g3Dmap, u32 idx );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�O���[�o���I�u�W�F�N�g�h�c�ϊ�
 */
//------------------------------------------------------------------
extern BOOL	GFL_G3D_MAP_GetGlobalObjectID
					( GFL_G3D_MAP* g3Dmap, const u32 datID, u32* exchangeID );
extern BOOL	GFL_G3D_MAP_GetGlobalDDobjectID
					( GFL_G3D_MAP* g3Dmap, const u32 datID, u32* exchangeID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_SetLoadReq( GFL_G3D_MAP* g3Dmap, const u32 datID );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�`��n�m�^�n�e�e���擾
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_SetDrawSw( GFL_G3D_MAP* g3Dmap, BOOL drawSw );
extern BOOL	GFL_G3D_MAP_GetDrawSw( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�ʒu�ݒ聕�擾
 */
//------------------------------------------------------------------
extern void	GFL_G3D_MAP_SetTrans( GFL_G3D_MAP* g3Dmap, const VecFx32* trans );
extern void	GFL_G3D_MAP_GetTrans( GFL_G3D_MAP* g3Dmap, VecFx32* trans );
//------------------------------------------------------------------
/**
 * @brief	�A�g���r���[�g�擾
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_InitAttr( GFL_G3D_MAP_ATTRINFO* attrInfo );
extern void GFL_G3D_MAP_GetAttr( GFL_G3D_MAP_ATTRINFO* attrInfo,
								GFL_G3D_MAP* g3Dmap, const VecFx32* pos, const fx32 map_width );
//------------------------------------------------------------------
/**
 * @brief	�t�@�C�����ʐݒ�i���j��������̓f�[�^�t�@�C���̒��Ɏ��ʂ𖄂ߍ���
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_ResistFileType( GFL_G3D_MAP* g3Dmap, u32 fileType );


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
extern void GFL_G3D_MAP_GetLoadStatusPointer( GFL_G3D_MAP* g3Dmap, GFL_G3D_MAP_LOAD_STATUS** ldst );
//------------------------------------------------------------------
/**
 * @brief	���[�h�X�e�[�^�X������
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_ResetLoadStatus( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	���[�h�������|�C���^�擾
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_GetLoadMemoryPointer( GFL_G3D_MAP* g3Dmap, void** mem );
//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�n���h���擾
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_GetLoadArcHandle( GFL_G3D_MAP* g3Dmap, ARCHANDLE** handle );
//------------------------------------------------------------------
/**
 * @brief	�A�[�J�C�u�����[�h�f�[�^�h�c�擾
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_GetLoadDatID( GFL_G3D_MAP* g3Dmap, u32* ID );
//------------------------------------------------------------------
/**
 * @brief	���f�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_CreateResourceMdl( GFL_G3D_MAP* g3Dmap, void* mem );
extern void GFL_G3D_MAP_DeleteResourceMdl( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�e�N�X�`�����\�[�X�ݒ�
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_CreateResourceTex( GFL_G3D_MAP* g3Dmap, void* mem );
extern void GFL_G3D_MAP_DeleteResourceTex( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�����_�[�쐬
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_MakeRenderObj( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�t�@�C���ǂݍ��݊֐�
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_StartFileLoad( GFL_G3D_MAP* g3Dmap, u32 datID );
extern BOOL GFL_G3D_MAP_ContinueFileLoad( GFL_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	���[�J���e�N�X�`��VRAM�]���֐�(�e�N�X�`���ǂݍ��ݏI����ɍs��)
 */
//------------------------------------------------------------------
extern void GFL_G3D_MAP_SetTransVramParam( GFL_G3D_MAP* g3Dmap );
extern BOOL GFL_G3D_MAP_TransVram( GFL_G3D_MAP* g3Dmap );


//============================================================================================
/**
 * @brief	�e�X�g�֐�
 */
//============================================================================================
extern void GFL_G3D_MAP_MakeTestPos( GFL_G3D_MAP* g3Dmap );

#ifdef __cplusplus
}/* extern "C" */
#endif

