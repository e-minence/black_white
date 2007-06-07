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
	GFL_G3D_SCENEOBJ_DATA		sceneObjData;
	GFL_G3D_SCENE*				g3Dscene;
	void*						sceneObjWorkEx;
	GFL_G3D_SCENEACCESORY_DATA*	accesory;
	u16							accesoryCount;
};

struct _GFL_G3D_SCENE {
	GFL_G3D_UTIL*			g3Dutil;
	GFL_TCBLSYS*			g3DsceneObjTCBLsys;
	GFL_TCBL**				g3DsceneObjTCBLtbl;
	GFL_AREAMAN*			g3DsceneObjAreaman;
	u16						g3DsceneObjMax;	
	u32						g3DsceneObjWorkSize;
	u16*					g3DsceneObjPriTbl;
	BOOL					GFL_PTC_Enable;
	BOOL					GFL_PTC_DrawSW;
	HEAPID					heapID;
};


static void sceneObjfunc( GFL_TCBL* tcbl, void* work );
static void objDrawSort( GFL_G3D_SCENE* g3Dscene );

//static void getTranslucent( NNSG3dRS* rs );

#define TCBL_POINTER_SIZ		(4)
//=============================================================================================
/**
 *
 *
 * �Ǘ��V�X�e��
 *
 *	g3Dutil���\�[�X�z��𗘗p���A�z������h�m�c�d�w�Q�Ƃ���`�ŏ������s���B
 *	�Ȃ̂ŕ`�挳���\�[�X�͂��ׂĂ�����ŃZ�b�g�A�b�v����Ă���K�v������B
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
 * @param	GFL_PTC_Enable	�p�[�e�B�N���V�X�e���̋N���t���O
 * @param	heapID			�q�[�v�h�c
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_SCENE*
	GFL_G3D_SCENE_Create
		( GFL_G3D_UTIL* g3Dutil, const u16 sceneObjMax, const u32 sceneObjWkSiz, 
			const u16 sceneAccesoryMax, BOOL GFL_PTC_Enable, const HEAPID heapID )
{
	GFL_G3D_SCENE*	g3Dscene;
	GFL_TCBL*		g3DsceneObjTCBL;
	u32	TCBLworkSize = sizeof(GFL_G3D_SCENEOBJ) + sceneObjWkSiz;

	GF_ASSERT( g3Dutil );
	//�V�[���Ǘ��̈�m��
	g3Dscene = GFL_HEAP_AllocClearMemory( heapID, sizeof(GFL_G3D_SCENE) );

	//�z�u�I�u�W�F�N�gTCBL�N���i����v���C�I���e�B�[�Ȃǂ�TCBL�Ǘ�����j
	g3Dscene->g3DsceneObjTCBLsys = GFL_TCBL_Init(heapID,heapID,sceneObjMax,TCBLworkSize); 
	//�z�u�I�u�W�F�N�g�Ǘ��z��쐬
	g3Dscene->g3DsceneObjTCBLtbl = GFL_HEAP_AllocClearMemory(heapID,TCBL_POINTER_SIZ*sceneObjMax);
	//�z�u�I�u�W�F�N�g�z��̈�}�l�[�W���쐬
	g3Dscene->g3DsceneObjAreaman = GFL_AREAMAN_Create( sceneObjMax, heapID );
	//�z�u�I�u�W�F�N�g�v���C�I���e�B�[�Ǘ��z��쐬
	g3Dscene->g3DsceneObjPriTbl = GFL_HEAP_AllocClearMemory( heapID, 2*sceneObjMax );

	g3Dscene->heapID = heapID;
	g3Dscene->g3Dutil = g3Dutil;
	g3Dscene->g3DsceneObjMax = sceneObjMax;
	g3Dscene->g3DsceneObjWorkSize = TCBLworkSize;
	g3Dscene->GFL_PTC_Enable = GFL_PTC_Enable;
	
	if( g3Dscene->GFL_PTC_Enable == TRUE ){
		GFL_PTC_Init(heapID);
		g3Dscene->GFL_PTC_DrawSW = TRUE;
	}
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
	GFL_TCBL_Main( g3Dscene->g3DsceneObjTCBLsys );
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
	GFL_G3D_OBJ			*g3Dobj, *g3Dobj_Accesory;
	GFL_G3D_RND*		g3Drnd;
	NNSG3dResMdl*		pMdl;
	int	i = 0;
	int	j;
	BOOL cullResult; 

	//�`��J�n
	GFL_G3D_DRAW_Start();

	if(( g3Dscene->GFL_PTC_Enable == TRUE )&&( g3Dscene->GFL_PTC_DrawSW == TRUE )){
		GFL_PTC_Main();
	}
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DRAW_SetLookAt();
	//�`��v���C�I���e�B�[�ɂ��\�[�g
	objDrawSort( g3Dscene );

	//�e�A�N�^�[�̕`��
	while( ( i<g3Dscene->g3DsceneObjMax )&&( g3Dscene->g3DsceneObjPriTbl[i] != 0xffff) ){
		//��v�\���̃|�C���^�擾
		g3DsceneObj = GFL_TCBL_GetWork
						( g3Dscene->g3DsceneObjTCBLtbl[ g3Dscene->g3DsceneObjPriTbl[i] ] );
		g3Dobj = GFL_G3D_UTIL_GetObjHandle( g3Dscene->g3Dutil, g3DsceneObj->sceneObjData.objID );

		//�������ݒ�
		g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3Dobj );
		pMdl = NNS_G3dRenderObjGetResMdl( GFL_G3D_RENDER_GetRenderObj( g3Drnd ) );
		if( g3DsceneObj->sceneObjData.blendAlpha == GFL_G3D_SCENEOBJ_ALPHA_OFF ){
			NNS_G3dMdlUseMdlPolygonID( pMdl );
			NNS_G3dMdlUseMdlAlpha( pMdl );
		} else {
			NNS_G3dGlbPolygonAttr(	GX_LIGHTMASK_NONE, GX_POLYGONMODE_MODULATE,
									GX_CULL_NONE, i, g3DsceneObj->sceneObjData.blendAlpha, 0 );
			NNS_G3dMdlUseGlbPolygonID( pMdl );	//�ꎞ�I�ɂh�c��ύX�i�������̂��߁j
			NNS_G3dMdlUseGlbAlpha( pMdl );		//���f���Ă���̂̓��ݒ肾��
		}

		if( g3DsceneObj->sceneObjData.cullingFlag == TRUE ){
			//�{�̂̕`��
			cullResult = GFL_G3D_DRAW_DrawObjectCullingON
							( g3Dobj, &g3DsceneObj->sceneObjData.status );
		} else {
			//�{�̂̕`��
			GFL_G3D_DRAW_DrawObject( g3Dobj, &g3DsceneObj->sceneObjData.status );
			cullResult = TRUE;
		}
		//�A�N�Z�T���[�̕`��i�ꕔ�p�����[�^�[�͖{�̂̂��̂����p���j
		if( cullResult == TRUE ){
			for( j=0; j<g3DsceneObj->accesoryCount; j++ ){
				g3Dobj_Accesory = GFL_G3D_UTIL_GetObjHandle
								( g3Dscene->g3Dutil, g3DsceneObj->accesory[j].objID );
				GFL_G3D_DRAW_DrawAccesory( g3Dobj, g3Dobj_Accesory,
						&g3DsceneObj->accesory[j].status, g3DsceneObj->accesory[j].jntID );
			}
		}
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
	GF_ASSERT( g3Dscene );

	if( g3Dscene->GFL_PTC_Enable == TRUE ){
		GFL_PTC_Exit();
	}
	GFL_HEAP_FreeMemory( g3Dscene->g3DsceneObjPriTbl );
	GFL_AREAMAN_Delete( g3Dscene->g3DsceneObjAreaman );
	GFL_HEAP_FreeMemory( g3Dscene->g3DsceneObjTCBLtbl );
	GFL_TCBL_Exit( g3Dscene->g3DsceneObjTCBLsys );
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
	GF_ASSERT( g3Dscene );
	GF_ASSERT( idx < g3Dscene->g3DsceneObjMax );
	return ( GFL_G3D_SCENEOBJ* )GFL_TCBL_GetWork( g3Dscene->g3DsceneObjTCBLtbl[ idx ] );
}

//--------------------------------------------------------------------------------------------
/**
 * �p�[�e�B�N���`��̂n�m/�n�e�e(�p�[�e�B�N���V�X�e�����N������Ă���ꍇ�̂ݗL��)
 *
 * @param	BOOL			�p�[�e�B�N���`��X�C�b�`
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENE_SetDrawParticleSW
		( GFL_G3D_SCENE* g3Dscene, BOOL sw )
{
	GF_ASSERT( g3Dscene );
	g3Dscene->GFL_PTC_DrawSW = sw;
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
		g3DsceneObj->g3Dscene = g3Dscene;
		g3DsceneObj->sceneObjData = sceneObjTbl[i];
		g3DsceneObj->sceneObjWorkEx	= NULL;
		g3DsceneObj->accesoryCount = 0;
		g3DsceneObj->accesory = NULL;
#if 0
		// �����������R�[���o�b�N�ݒ�
		{
			GFL_G3D_OBJ* g3Dobj = GFL_G3D_UTIL_GetObjHandle( g3Dscene->g3Dutil, 
																g3DsceneObj->sceneObjData.objID );
			GFL_G3D_RND* g3Drnd = GFL_G3D_OBJECT_GetG3Drnd( g3Dobj );

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
	GFL_TCBL*			g3DsceneObjTCBL;
	GFL_G3D_SCENEOBJ*	g3DsceneObj;
	int	i;

	GF_ASSERT( g3Dscene );
	for( i=0; i<sceneObjCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( g3Dscene, idx+i );

		GFL_G3D_SCENEOBJ_ACCESORY_Remove( g3DsceneObj );	//�A�N�Z�T���[�j��

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
	return GFL_G3D_UTIL_GetObjHandle(	g3DsceneObj->g3Dscene->g3Dutil, 
										g3DsceneObj->sceneObjData.objID );
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

//--------------------------------------------------------------------------------------------
/**
 *
 * �A�N�Z�T���[�֘A
 *�@�j���Ɋւ��Ă͔z�u�I�u�W�F�N�g���폜�����ۂɂ������ŏ�����B
 */
//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
/**
 * �A�N�Z�T���[�o�^
 *
 * @param	g3DsceneObj			�z�u�I�u�W�F�N�g�|�C���^
 * @param	accesoryTbl			�A�N�Z�T���[�ݒ�f�[�^�e�[�u���|�C���^
 * @param	accesoryCount		�A�N�Z�T���[��
 *
 * @return	BOOL
 */
//--------------------------------------------------------------------------------------------
BOOL
	GFL_G3D_SCENEOBJ_ACCESORY_Add
		( GFL_G3D_SCENEOBJ* g3DsceneObj, const GFL_G3D_SCENEACCESORY_DATA* accesoryTbl, 
			const u16 accesoryCount )
{
	int	i;

	if( g3DsceneObj->accesoryCount ) return FALSE;	//��d�o�^�s��

	g3DsceneObj->accesory = GFL_HEAP_AllocClearMemory( g3DsceneObj->g3Dscene->heapID, 
								sizeof(GFL_G3D_SCENEACCESORY_DATA) * accesoryCount );
	g3DsceneObj->accesoryCount = accesoryCount;

	//�A�N�Z�T���[�ݒ�z����R�s�[
	for( i=0; i<accesoryCount; i++ ){
		g3DsceneObj->accesory[i] = accesoryTbl[i];
	}
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	g3DsceneObj			�z�u�I�u�W�F�N�g�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_ACCESORY_Remove
		( GFL_G3D_SCENEOBJ* g3DsceneObj )
{
	if( !g3DsceneObj->accesory ) return;

	GFL_HEAP_FreeMemory( g3DsceneObj->accesory );
	g3DsceneObj->accesory = NULL;
	g3DsceneObj->accesoryCount = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�h�c�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	accesoryIdx		�A�N�Z�T���[�C���f�b�N�X
 * @param	objID			�I�u�W�F�N�g�h�c�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_ACCESORY_GetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, u16* objID )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	*objID = g3DsceneObj->accesory[accesoryIdx].objID;
}

void
	GFL_G3D_SCENEOBJ_ACCESORY_SetObjID
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, u16* objID )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	g3DsceneObj->accesory[accesoryIdx].objID = *objID;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�ʒu���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	accesoryIdx		�A�N�Z�T���[�C���f�b�N�X
 * @param	trans			�ʒu���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_ACCESORY_GetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* trans )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	*trans = g3DsceneObj->accesory[accesoryIdx].status.trans;
}

void
	GFL_G3D_SCENEOBJ_ACCESORY_SetPos
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* trans )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	g3DsceneObj->accesory[accesoryIdx].status.trans = *trans;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�X�P�[�����j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	accesoryIdx		�A�N�Z�T���[�C���f�b�N�X
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_ACCESORY_GetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* scale )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	*scale = g3DsceneObj->accesory[accesoryIdx].status.scale;
}

void
	GFL_G3D_SCENEOBJ_ACCESORY_SetScale
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, VecFx32* scale )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	g3DsceneObj->accesory[accesoryIdx].status.scale = *scale;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i��]���j�̎擾�ƕύX
 *
 * @param	g3DsceneObj		�z�u�I�u�W�F�N�g�|�C���^
 * @param	accesoryIdx		�A�N�Z�T���[�C���f�b�N�X
 * @param	rotate			��]���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_SCENEOBJ_ACCESORY_GetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, MtxFx33* rotate )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	*rotate = g3DsceneObj->accesory[accesoryIdx].status.rotate;
}

void
	GFL_G3D_SCENEOBJ_ACCESORY_SetRotate
		( GFL_G3D_SCENEOBJ* g3DsceneObj, u16 accesoryIdx, MtxFx33* rotate )
{
	GF_ASSERT( g3DsceneObj->accesoryCount );
	GF_ASSERT( accesoryIdx < g3DsceneObj->accesoryCount );
	g3DsceneObj->accesory[accesoryIdx].status.rotate = *rotate;
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
