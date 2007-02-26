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
	GFL_G3D_SceneCreate
		( GFL_G3D_UTIL* g3Dutil, const u16 sceneObjMax, const u32 sceneObjWkSiz, 
			const HEAPID heapID )
{
	GFL_G3D_SCENE*	g3Dscene;
	GFL_TCBL*		g3DsceneObjTCBL;
	u32	TCBLworkSize = sizeof(GFL_G3D_SCENEOBJ) + sceneObjWkSiz;

	GF_ASSERT( g3Dutil );
	//�Ǘ��̈�m��
	g3Dscene = GFL_HEAP_AllocMemoryClear( heapID, sizeof(GFL_G3D_SCENE) );
	//TCBL�N��
	g3Dscene->g3DsceneObjTCBLsys = GFL_TCBL_SysInit(heapID,heapID,sceneObjMax,TCBLworkSize); 
	//�Ǘ��z��쐬
	g3Dscene->g3DsceneObjTCBLtbl = GFL_HEAP_AllocMemoryClear(heapID,TCBL_POINTER_SIZ*sceneObjMax);
	//�z��̈�}�l�[�W���쐬
	g3Dscene->g3DsceneObjAreaman = GFL_AREAMAN_Create( sceneObjMax, heapID );
	//�v���C�I���e�B�[�Ǘ��z��쐬
	g3Dscene->g3DsceneObjPriTbl = GFL_HEAP_AllocMemoryClear( heapID, 2*sceneObjMax );

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
	GFL_G3D_SceneMain
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
	GFL_G3D_SceneDraw
		( GFL_G3D_SCENE* g3Dscene )  
{
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	GFL_G3D_OBJ*		g3Dobj;
	int	i = 0;

	//�`��J�n
	GFL_G3D_DrawStart();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DrawLookAt();
	//�`��v���C�I���e�B�[�ɂ��\�[�g
	objDrawSort( g3Dscene );

	//�e�A�N�^�[�̕`��
	while( ( i<g3Dscene->g3DsceneObjMax )&&( g3Dscene->g3DsceneObjPriTbl[i] != 0xffff) ){
		g3DsceneObj = GFL_TCBL_GetWork
						( g3Dscene->g3DsceneObjTCBLtbl[ g3Dscene->g3DsceneObjPriTbl[i] ] );
		g3Dobj = GFL_G3D_UtilsysObjHandleGet( g3Dscene->g3Dutil, g3DsceneObj->sceneObjData.objID );

		GFL_G3D_ObjDraw( g3Dobj, &g3DsceneObj->sceneObjData.status );
		i++;
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	GFL_G3D_DrawEnd();							
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���j��
 *
 * @param	g3Dscene		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SceneDelete
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
	GFL_G3D_SceneObjGet
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
 * �\�[�g�֐��i���[�J���j
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
			GFL_G3D_SceneObjDrawSWGet( GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[i]), &sw );
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

			GFL_G3D_SceneObjDrawPriGet(GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[idx1]),&pri1);
			GFL_G3D_SceneObjDrawPriGet(GFL_TCBL_GetWork(g3Dscene->g3DsceneObjTCBLtbl[idx2]),&pri2);

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
	GFL_G3D_SceneObjAdd
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
		g3DsceneObj->sceneObjWorkEx	= NULL;
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
	GFL_G3D_SceneObjDel
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
 * �I�u�W�F�N�g�h�c�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	objID			�I�u�W�F�N�g�h�c�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SceneObjIDGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16* objID )
{
	*objID = g3DsceneObj->sceneObjData.objID;
}

void
	GFL_G3D_SceneObjIDSet
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
	GFL_G3D_SceneObjDrawPriGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri )
{
	*drawPri = g3DsceneObj->sceneObjData.drawPriority;
}

void
	GFL_G3D_SceneObjDrawPriSet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u8* drawPri )
{
	g3DsceneObj->sceneObjData.drawPriority = *drawPri;
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
	GFL_G3D_SceneObjDrawSWGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, BOOL* drawSW )
{
	*drawSW = g3DsceneObj->sceneObjData.drawSW;
}

void
	GFL_G3D_SceneObjDrawSWSet
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
	GFL_G3D_SceneObjStatusTransGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* trans )
{
	*trans = g3DsceneObj->sceneObjData.status.trans;
}

void
	GFL_G3D_SceneObjStatusTransSet
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
	GFL_G3D_SceneObjStatusScaleGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, VecFx32* scale )
{
	*scale = g3DsceneObj->sceneObjData.status.scale;
}

void
	GFL_G3D_SceneObjStatusScaleSet
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
	GFL_G3D_SceneObjStatusRotateGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, MtxFx33* rotate )
{
	*rotate = g3DsceneObj->sceneObjData.status.rotate;
}

void
	GFL_G3D_SceneObjStatusRotateSet
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
	GFL_G3D_SceneObjFuncGet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func )
{
	*func = g3DsceneObj->sceneObjData.func;
}

void
	GFL_G3D_SceneObjFuncSet
		( GFL_G3D_SCENEOBJ* g3DsceneObj, GFL_G3D_SCENEOBJFUNC** func )
{
	g3DsceneObj->sceneObjData.func = *func;
}

	








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
