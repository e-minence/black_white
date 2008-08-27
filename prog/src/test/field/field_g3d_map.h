#define	MAPPLTT_SIZE	(0x200) 	//�e�N�X�`���p���b�g�p�u�q�`�l�m�ۃT�C�Y 
#define	MAPOBJ_SIZE		(0x18000) 	//�u���b�N���I�u�W�F�N�g���f���f�[�^�p�������m�ۃT�C�Y 
#define	MAPOBJTEX_SIZE	(0x4000) 	//�u���b�N���I�u�W�F�N�g�e�N�X�`���f�[�^�p�u�q�`�l�m�ۃT�C�Y 

#define	MAPLOAD_SIZE	(0x800)		//�����f�[�^���[�h�T�C�Y(ROM��RAM) 
#define	MAPTRANS_SIZE	(0x2000) 	//�����f�[�^�]���T�C�Y(RAM��VRAM) 

#define MAP_GRIDCOUNT	(32)		//�}�b�v���c���O���b�h��

#define DRAW_LIMIT	(1024 * FX32_ONE)		//�`��͈�
#define LOD_LIMIT	(512 * FX32_ONE)		//HighQuallity(LOD)�p�`��͈�

typedef struct _FLD_G3D_MAP FLD_G3D_MAP;
typedef void (FLD_G3D_MAP_DDOBJ_FUNC)
					( u32 texDataAdrs, u32 texPlttAdrs, VecFx16* vecView, BOOL lodSt );

#define	NON_ATTR	(0xffff)

enum {
	DRAW_NORMAL = 0,
	DRAW_YBILLBOARD,
};
enum {
	LIGHT_NONE = 0,
	LIGHT0_ON = 1,
	LIGHT1_ON = 2,
	LIGHT2_ON = 4,
	LIGHT3_ON = 8,
};

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


typedef struct {
	NNSG3dRenderObj*	NNSrnd_H;	//�����_�[(High Q)
	GFL_G3D_RES*		g3Dres_H;	//���\�[�X(High Q)
	NNSG3dRenderObj*	NNSrnd_L;	//�����_�[(Low Q)
	GFL_G3D_RES*		g3Dres_L;	//���\�[�X(High Q)
}FLD_G3D_MAP_OBJ;

typedef struct {
	GFL_G3D_RES*			g3Dres;
	u32						texDataAdrs;
	u32						texPlttAdrs;
	const FLD_G3D_MAP_DDOBJ_DATA*	data;
}FLD_G3D_MAP_DDOBJ;

typedef struct {
	VecFx16 vecN;
	u32		attr;
	fx32	height;
}FLD_G3D_MAP_ATTRINFO;

typedef struct {
	u32	mdlHeapSize;
	u32	texHeapSize;
	u32	attrHeapSize;

}FLD_G3D_MAP_SETUP;

//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�V�X�e���쐬
 */
//------------------------------------------------------------------
extern FLD_G3D_MAP*	FLD_G3D_MAP_Create( FLD_G3D_MAP_SETUP* setup, HEAPID heapID );
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
extern void	FLD_G3D_MAP_Draw( FLD_G3D_MAP* g3Dmap, GFL_G3D_CAMERA* g3Dcamera,
								const FLD_G3D_MAP_OBJ* obj, const FLD_G3D_MAP_DDOBJ* ddobj );
extern void	FLD_G3D_MAP_EndDraw( void );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v�A�[�J�C�u�f�[�^�o�^
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_ResistArc( FLD_G3D_MAP* g3Dmap, const u32 arcID, HEAPID heapID );
extern void	FLD_G3D_MAP_ReleaseArc( FLD_G3D_MAP* g3Dmap );
//------------------------------------------------------------------
/**
 * @brief	�R�c�}�b�v���[�h���N�G�X�g�ݒ�
 */
//------------------------------------------------------------------
extern void	FLD_G3D_MAP_SetLoadReq( FLD_G3D_MAP* g3Dmap,
								const u32 datID, const u32 texID, const u32 attrID );
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
extern void FLD_G3D_MAP_GetAttr( FLD_G3D_MAP_ATTRINFO* attrInfo, const FLD_G3D_MAP* g3Dmap,
									const VecFx32* pos, const fx32 map_width );

