//=============================================================================================
/**
 * @file	g3d_scene.c                                                  
 * @brief	�R�c�V�[���Ǘ��v���O����
 * @date	
 */
//=============================================================================================
#include "gflib.h"
#include "tcbl.h"

#include "g3d_scene.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
struct _GFL_G3D_SCENEOBJ {
	GFL_G3D_SCENEOBJ_DATA	sceneObjData;
	GFL_G3D_OBJ*			g3Dobj;
	void*					sceneObjWorkEx;
};

struct _GFL_G3D_SCENE {
	GFL_G3D_UTIL*		g3Dutil;
	GFL_TCBLSYS*		g3DsceneObjTCBLsys;
	GFL_TCBL**			g3DsceneObjTCBLtbl;
	GFL_AREAMAN*		g3DsceneObjAreaman;
	u16					g3DsceneObjMax;	
	u32					g3DsceneObjWorkSize;
	u16*				g3DsceneObjPriTbl;
	HEAPID				heapID;
};


static void sceneObjfunc( GFL_TCBL* tcbl, void* work );
static void objDrawSort( GFL_G3D_SCENE* g3Dscene );

//static void getTranslucent( NNSG3dRS* rs );

#define TCBL_POINTER_SIZ	(4)
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
 * �V�X�e���쐬
 *
 * @param	g3Dutil			�ˑ�����g3Dutil
 * @param	sceneObjMax		�z�u�\�ȃI�u�W�F�N�g��
 * @param	sceneObjWkSiz	�P�I�u�W�F�N�g�Ɋ��蓖�Ă郏�[�N�̃T�C�Y
 * @param	heapID			�q�[�v�h�c
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_SCENE*
	GFL_G3D_SCENE_Create
		( GFL_G3D_UTIL* g3Dutil, const u16 sceneObjMax, const u32 sceneObjWkSiz, 
			const HEAPID heapID )
{
	GFL_G3D_SCENE*	g3Dscene;
	GFL_TCBL*		g3DsceneObjTCBL;
	u32	TCBLworkSize = sizeof(GFL_G3D_SCENEOBJ) + sceneObjWkSiz;

	GF_ASSERT( g3Dutil );
	//�Ǘ��̈�m��
	g3Dscene = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_SCENE) );
	//TCBL�N��
	g3Dscene->g3DsceneObjTCBLsys = GFL_TCBL_SysInit(heapID,heapID,sceneObjMax,TCBLworkSize); 
	//�Ǘ��z��쐬
	g3Dscene->g3DsceneObjTCBLtbl = GFL_HEAP_AllocClearMemory(heapID,TCBL_POINTER_SIZ*sceneObjMax);
	//�z��̈�}�l�[�W���쐬
	g3Dscene->g3DsceneObjAreaman = GFL_AREAMAN_Create( sceneObjMax, heapID );
	//�v���C�I���e�B�[�Ǘ��z��쐬
	g3Dscene->g3DsceneObjPriTbl = GFL_HEAP_AllocClearMemory( heapID, 2*sceneObjMax );

	g3Dscene->heapID = heapID;
	g3Dscene->g3Dutil = g3Dutil;
	g3Dscene->g3DsceneObjMax = sceneObjMax;
	g3Dscene->g3DsceneObjWorkSize = TCBLworkSize;

	return g3Dscene;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i����j
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENE_Main
		( GFL_G3D_SCENE* g3Dscene )  
{
	GFL_TCBL_SysMain( g3Dscene->g3DsceneObjTCBLsys );
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i�`��j
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENE_Draw
		( GFL_G3D_SCENE* g3Dscene )  
{
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	GFL_G3D_OBJ*		g3Dobj;
	GFL_G3D_RND*		g3Drnd;
	NNSG3dResMdl*		pMdl;
	int	i = 0;

	//�`��J�n
	GFL_G3D_DRAW_Start();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DRAW_SetLookAt();
	//�`��v���C�I���e�B�[�ɂ��\�[�g
	objDrawSort( g3Dscene );

	//�e�A�N�^�[�̕`��
	while( ( i<g3Dscene->g3DsceneObjMax )&&( g3Dscene->g3DsceneObjPriTbl[i] != 0xffff) ){
		g3DsceneObj = GFL_TCBL_GetWork
						( g3Dscene->g3DsceneObjTCBLtbl[ g3Dscene->g3DsceneObjPriTbl[i] ] );
		g3Dobj = g3DsceneObj->g3Dobj;

		//�������ݒ�
		g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3DsceneObj->g3Dobj );
		pMdl = NNS_G3dRenderObjGetResMdl( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );
#if 0
		{
			u32 MatID;

			NNS_G3dMdlUseMdlAlpha( pMdl );
			for ( MatID = 0; MatID < pMdl->info.numMat; ++MatID ){
				NNS_G3dMdlSetMdlAlpha( pMdl, MatID, g3DsceneObj->sceneObjData.blendAlpha );
			}
		}
#else
		if( g3DsceneObj->sceneObjData.blendAlpha == GFL_G3D_SCENEOBJ_ALPHA_OFF ){
			NNS_G3dMdlUseMdlPolygonID( pMdl );
			NNS_G3dMdlUseMdlAlpha( pMdl );
		} else {
			NNS_G3dGlbPolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE,
									GX_CULL_NONE, i, g3DsceneObj->sceneObjData.blendAlpha, 0 );
			NNS_G3dMdlUseGlbPolygonID( pMdl );	//�ꎞ�I�ɂh�c��ύX�i�������̂��߁j
			NNS_G3dMdlUseGlbAlpha( pMdl );		//���f���Ă���̂̓��ݒ肾��
		}
#endif
		GFL_G3D_DRAW_DrawObjectCullingON( g3Dobj, &g3DsceneObj->sceneObjData.status );
		i++;
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	GFL_G3D_DRAW_End();							
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���j��
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENE_Delete
		( GFL_G3D_SCENE* g3Dscene )  
{
	GFL_HEAP_FreeMemory( g3Dscene->g3DsceneObjPriTbl );
	GFL_AREAMAN_Delete( g3Dscene->g3DsceneObjAreaman );
	GFL_HEAP_FreeMemory( g3Dscene->g3DsceneObjTCBLtbl );
	GFL_TCBL_SysExit( g3Dscene->g3DsceneObjTCBLsys );
	GFL_HEAP_FreeMemory( g3Dscene );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�N�^�[�|�C���^���h�m�c�d�w���擾
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	idx				�A�N�^�[�z�u�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_SCENEOBJ*
	GFL_G3D_SCENEOBJ_Get
		( GFL_G3D_SCENE* g3Dscene, u32 idx )
{
	GF_ASSERT( idx < g3Dscene->g3DsceneObjMax );
	return ( GFL_G3D_SCENEOBJ* )GFL_TCBL_GetWork( g3Dscene->g3DsceneObjTCBLtbl[ idx ] );
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐��i���[�J���j
 * 
 * ���̊֐����o�R���Ċe�z�u�I�u�W�F�N�g����֐����Ăяo���A�߂��Ă���
 */
//--------------------------------------------------------------------------------------------
static void sceneObjfunc( GFL_TCBL* tcbl, void* work )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = (GFL_G3D_SCENEOBJ*)work;

	if( g3DsceneObj->sceneObjData.func != NULL ){
		g3DsceneObj->sceneObjData.func( g3DsceneObj, (void*)((u32)work+sizeof(GFL_G3D_SCENEOBJ)));
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �\�[�g�֐��y�����z�i���[�J���j
 */
//--------------------------------------------------------------------------------------------
static void objDrawSort( GFL_G3D_SCENE* g3Dscene )
{
	int		i;
	u16		idx1,idx2;
	u8		pri1,pri2;
	BOOL	sw;
	int		count = 0;

	for( i=0; i<g3Dscene->g3DsceneObjMax; i++ ){
		if( g3Dscene->g3DsceneObjTCBLtbl[i] != NULL ){
			GFL_G3D_SCENEOBJ_GetDrawSW( GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[i]), &sw );
			if( sw == TRUE ){
				g3Dscene->g3DsceneObjPriTbl[ count ] = i;
				count++;
			}
		}
	}
	for( i=count; i<g3Dscene->g3DsceneObjMax; i++ ){
		g3Dscene->g3DsceneObjPriTbl[i] = 0xffff;
	}
	while( count ){
		for( i=0; i<count-1; i++ ){
			idx1 = g3Dscene->g3DsceneObjPriTbl[i];
			idx2 = g3Dscene->g3DsceneObjPriTbl[i+1];

			GFL_G3D_SCENEOBJ_GetDrawPri(GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[idx1]),&pri1);
			GFL_G3D_SCENEOBJ_GetDrawPri(GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[idx2]),&pri2);

			if( pri1 > pri2 ){
				g3Dscene->g3DsceneObjPriTbl[i] = idx2;
				g3Dscene->g3DsceneObjPriTbl[i+1] = idx1;
			}
		}
		count--;
	}
}





//=============================================================================================
/**
 *
 *
 * �z�u�I�u�W�F�N�g
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �쐬
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	sceneObjTbl		�z�u�I�u�W�F�N�g�ݒ�f�[�^
 * @param	sceneObjCount	�z�u�I�u�W�F�N�g��
 *
 * @return	idx				�z�u�I�u�W�F�N�g�擪�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
u32
	GFL_G3D_SCENEOBJ_Add
		( GFL_G3D_SCENE* g3Dscene, const GFL_G3D_SCENEOBJ_DATA* sceneObjTbl, 
			const u16 sceneObjCount )
{
	GFL_TCBL*			g3DsceneObjTCBL;
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	u32 pos;
	int	i;

	GF_ASSERT( g3Dscene );
	//�쐬INDEX�m��
	pos = GFL_AREAMAN_ReserveAuto( g3Dscene->g3DsceneObjAreaman, sceneObjCount );
	GF_ASSERT_MSG(( pos != AREAMAN_POS_NOTFOUND ), "cannot reserve resource block" );

	for( i=0; i<sceneObjCount; i++ ){
		g3DsceneObjTCBL = GFL_TCBL_Create( g3Dscene->g3DsceneObjTCBLsys, sceneObjfunc,
										g3Dscene->g3DsceneObjWorkSize,
										sceneObjTbl[i].movePriority );	
		GF_ASSERT( g3DsceneObjTCBL );
		g3Dscene->g3DsceneObjTCBLtbl[ pos+i ] = g3DsceneObjTCBL;

		g3DsceneObj = GFL_TCBL_GetWork( g3DsceneObjTCBL );
		g3DsceneObj->sceneObjData	= sceneObjTbl[i];
		g3DsceneObj->g3Dobj	= GFL_G3D_UTIL_GetObjHandle
								( g3Dscene->g3Dutil, g3DsceneObj->sceneObjData.objID );
		g3DsceneObj->sceneObjWorkEx	= NULL;
#if 0
		// �����������R�[���o�b�N�ݒ�
		{
			GFL_G3D_RND* g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3DsceneObj->g3Dobj );
			NNS_G3dRenderObjSetCallBack( GFL_G3D_RENDER_GetRenderObj( g3Drnd ),
						&getTranslucent, NULL, NNS_G3D_SBC_MAT, NNS_G3D_SBC_CALLBACK_TIMING_B);
		}
#endif
		GFL_STD_MemClear( (void*)((u32)g3DsceneObj + sizeof(GFL_G3D_SCENEOBJ)), 
							g3Dscene->g3DsceneObjWorkSize - sizeof(GFL_G3D_SCENEOBJ) );
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 * @param	idx				�z�u�I�u�W�F�N�g�擪�h�m�c�d�w
 * @param	sceneObjCount	�z�u�I�u�W�F�N�g��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_Remove
		( GFL_G3D_SCENE* g3Dscene, u32 idx, const u16 sceneObjCount )
{
	GFL_TCBL* g3DsceneObjTCBL;
	int	i;

	GF_ASSERT( g3Dscene );
	for( i=0; i<sceneObjCount; i++ ){
		g3DsceneObjTCBL = g3Dscene->g3DsceneObjTCBLtbl[ idx+i ];
		if( g3DsceneObjTCBL ){
			GFL_TCBL_Delete( g3DsceneObjTCBL );
			//�h�m�c�d�w�e�[�u������폜
			g3Dscene->g3DsceneObjTCBLtbl[ idx+i ] = NULL;
		}
	}
	GFL_AREAMAN_Release( g3Dscene->g3DsceneObjAreaman, idx, sceneObjCount );
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h��(G3D_OBJ)�̎擾
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_SCENEOBJ_GetG3DobjHandle
		( GFL_G3D_SCENEOBJ* g3DsceneObj )
{
	return g3DsceneObj->g3Dobj;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�h�c�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	objID			�I�u�W�F�N�g�h�c�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID )
{
	*objID = g3DsceneObj->sceneObjData.objID;
}

void
	GFL_G3D_SCENEOBJ_SetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID )
{
	g3DsceneObj->sceneObjData.objID = *objID;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��v���C�I���e�B�[�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	drawPri			�`��v���C�I���e�B�[�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetDrawPri
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri )
{
	*drawPri = g3DsceneObj->sceneObjData.drawPriority;
}

void
	GFL_G3D_SCENEOBJ_SetDrawPri
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri )
{
	g3DsceneObj->sceneObjData.drawPriority = *drawPri;
}

//--------------------------------------------------------------------------------------------
/**
 * ���u�����h�l�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	blendAlpha		���u�����h�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetBlendAlpha
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha )
{
	*blendAlpha = g3DsceneObj->sceneObjData.blendAlpha;
}

void
	GFL_G3D_SCENEOBJ_SetBlendAlpha
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* blendAlpha )
{
	g3DsceneObj->sceneObjData.blendAlpha = *blendAlpha;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��X�C�b�`�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	drawSW			�`��X�C�b�`�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetDrawSW
		( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW )
{
	*drawSW = g3DsceneObj->sceneObjData.drawSW;
}

void
	GFL_G3D_SCENEOBJ_SetDrawSW
		( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW )
{
	g3DsceneObj->sceneObjData.drawSW = *drawSW;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�ʒu���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	trans			�ʒu���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans )
{
	*trans = g3DsceneObj->sceneObjData.status.trans;
}

void
	GFL_G3D_SCENEOBJ_SetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans )
{
	g3DsceneObj->sceneObjData.status.trans = *trans;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�X�P�[�����j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale )
{
	*scale = g3DsceneObj->sceneObjData.status.scale;
}

void
	GFL_G3D_SCENEOBJ_SetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale )
{
	g3DsceneObj->sceneObjData.status.scale = *scale;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i��]���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	rotate			��]���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate )
{
	*rotate = g3DsceneObj->sceneObjData.status.rotate;
}

void
	GFL_G3D_SCENEOBJ_SetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate )
{
	g3DsceneObj->sceneObjData.status.rotate = *rotate;
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐��̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_GetFunc
		( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func )
{
	*func = g3DsceneObj->sceneObjData.func;
}

void
	GFL_G3D_SCENEOBJ_SetFunc
		( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func )
{
	g3DsceneObj->sceneObjData.func = *func;
}





//=============================================================================================
/**
 *
 *
 * �R�[���o�b�N
 *
 *
 */
//=============================================================================================
#if 0
static void getTranslucent( NNSG3dRS* rs )
{
#if 0
    int matID;
    NNSG3dMatAnmResult* matResult;
    NNS_G3D_GET_MATID(NNS_G3dRenderObjGetResMdl(NNS_G3dRSGetRenderObj(rs)),
                      &matID,
                      "lambert2");
    SDK_ASSERT(matID >= 0);

    matResult = NNS_G3dRSGetMatAnmResult(rs);

    if (NNS_G3dRSGetCurrentMatID(rs) == matID)
    {
        matResult->prmPolygonAttr =
            (matResult->prmPolygonAttr & ~REG_G3_POLYGON_ATTR_ALPHA_MASK) |
            (16 << REG_G3_POLYGON_ATTR_ALPHA_SHIFT);
    }
    else
    {
        matResult->flag |= NNS_G3D_MATANM_RESULTFLAG_WIREFRAME;
    }
#else
	NNSG3dMatAnmResult* matResult = NNS_G3dRSGetMatAnmResult(rs);

	matResult->prmPolygonAttr = (matResult->prmPolygonAttr & ~REG_G3_POLYGON_ATTR_ALPHA_MASK) |
								(8 << REG_G3_POLYGON_ATTR_ALPHA_SHIFT);
#endif
}
#endif



#if 0
//=============================================================================================
/**
 *
 *
 * ���̑�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̉�]�s��̍쐬
 *
 * @param	rotSrc	�v�Z�O�̉�]�x�N�g���|�C���^
 * @param	rotDst	�v�Z��̉�]�s��i�[�|�C���^
 *
 * ���̊֐������g�p���A�I�u�W�F�N�g���ɓK�؂ȉ�]�s����쐬�������̂��A�`��ɗ����B
 */
//--------------------------------------------------------------------------------------------
// �w���x���y�̏��ԂŌv�Z
void
	GFL_G3D_UtilObjDrawRotateCalcXY
		( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotX33(	rotDst, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );

	MTX_RotY33(	&tmp, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)rotSrc->z), FX_CosIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

// �w���x���y�̏��ԂŌv�Z�i���΁j
void
	GFL_G3D_UtilObjDrawRotateCalcXY_Rev
		( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotX33(	rotDst, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );

	MTX_RotY33(	&tmp, FX_SinIdx((u16)-rotSrc->x), FX_CosIdx((u16)-rotSrc->x) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_CosIdx((u16)rotSrc->z), FX_SinIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

// �x���w���y�̏��ԂŌv�Z
void
	GFL_G3D_UtilObjDrawRotateCalcYX
		( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotY33(	rotDst, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)rotSrc->z), FX_CosIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst,&tmp, rotDst );
}

// �x���w���y�̏��ԂŌv�Z�i���΁j
void
	GFL_G3D_UtilObjDrawRotateCalcYX_Rev
		( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotY33(	rotDst, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );

	MTX_RotX33(	&tmp, FX_SinIdx((u16)-rotSrc->y), FX_CosIdx((u16)-rotSrc->y) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp,FX_CosIdx((u16)rotSrc->z), FX_SinIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

#endif
