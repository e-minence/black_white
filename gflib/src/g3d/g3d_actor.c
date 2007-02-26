//=============================================================================================
/**
 * @file	g3d_actor.c                                                  
 * @brief	�R�c�A�N�^�[�v���O����
 * @date	
 */
//=============================================================================================
#include "gflib.h"
#include "tcbl.h"

#include "g3d_actor.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
struct _GFL_G3D_ACTOR {
	GFL_G3D_ACTOR_DATA	actData;
	void*				actworkEx;
};

struct _GFL_G3D_ACTSYS {
	GFL_G3D_UTIL*		g3Dutil;
	GFL_TCBLSYS*		g3DactTCBLsys;
	GFL_TCBL**			g3DactTCBLtbl;
	GFL_AREAMAN*		g3DactAreaman;
	u16					g3DactorMax;	
	u32					g3DactWorkSize;
	u16*				g3DactPriTbl;
	HEAPID				heapID;
};


static void actfunc( GFL_TCBL* tcbl, void* work );
static void objDrawSort( GFL_G3D_ACTSYS* g3DactSys );
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
 * @param	g3Dutil		�ˑ�����g3Dutil
 * @param	actMax		�ݒ�\�ȃA�N�^�[��
 * @param	wksiz		�P�A�N�^�[�Ɋ��蓖�Ă郏�[�N�̃T�C�Y
 * @param	heapID		�q�[�v�h�c
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_ACTSYS*
	GFL_G3D_ActorSysCreate
		( GFL_G3D_UTIL* g3Dutil, const u16 actMax, const u32 wksiz, const HEAPID heapID )
{
	GFL_G3D_ACTSYS*	g3DactSys;
	GFL_TCBL*		g3DactTCBL;
	u32	TCBLworkSize = sizeof(GFL_G3D_ACTOR) + wksiz;

	GF_ASSERT( g3Dutil );
	//�Ǘ��̈�m��
	g3DactSys = GFL_HEAP_AllocMemoryClear( heapID, sizeof(GFL_G3D_ACTSYS) );
	//TCBL�N��
	g3DactSys->g3DactTCBLsys = GFL_TCBL_SysInit(heapID, heapID, actMax, TCBLworkSize); 
	//�Ǘ��z��쐬
	g3DactSys->g3DactTCBLtbl = GFL_HEAP_AllocMemoryClear( heapID, TCBL_POINTER_SIZ*actMax );
	//�z��̈�}�l�[�W���쐬
	g3DactSys->g3DactAreaman = GFL_AREAMAN_Create( actMax, heapID );
	//�v���C�I���e�B�[�Ǘ��z��쐬
	g3DactSys->g3DactPriTbl = GFL_HEAP_AllocMemoryClear( heapID, 2*actMax );

	g3DactSys->heapID = heapID;
	g3DactSys->g3Dutil = g3Dutil;
	g3DactSys->g3DactorMax = actMax;
	g3DactSys->g3DactWorkSize = TCBLworkSize;

	return g3DactSys;
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i����j
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorSysMain
		( GFL_G3D_ACTSYS* g3DactSys )  
{
	GFL_TCBL_SysMain( g3DactSys->g3DactTCBLsys );
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e�����C���i�`��j
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorSysDraw
		( GFL_G3D_ACTSYS* g3DactSys )  
{
	GFL_G3D_ACTOR*		g3Dact;
	GFL_G3D_OBJ*		g3Dobj;
	int	i = 0;

	//�`��J�n
	GFL_G3D_DrawStart();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DrawLookAt();
	//�`��v���C�I���e�B�[�ɂ��\�[�g
	objDrawSort( g3DactSys );

	//�e�A�N�^�[�̕`��
	while( ( i<g3DactSys->g3DactorMax )&&( g3DactSys->g3DactPriTbl[i] != 0xffff) ){
		g3Dact = GFL_TCBL_GetWork( g3DactSys->g3DactTCBLtbl[ g3DactSys->g3DactPriTbl[i] ] );
		g3Dobj = GFL_G3D_UtilsysObjHandleGet( g3DactSys->g3Dutil, g3Dact->actData.objID );

		GFL_G3D_ObjDraw( g3Dobj, &g3Dact->actData.status );
		i++;
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	GFL_G3D_DrawEnd();							
}

//--------------------------------------------------------------------------------------------
/**
 * �V�X�e���j��
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorSysDelete
		( GFL_G3D_ACTSYS* g3DactSys )  
{
	GFL_HEAP_FreeMemory( g3DactSys->g3DactPriTbl );
	GFL_AREAMAN_Delete( g3DactSys->g3DactAreaman );
	GFL_HEAP_FreeMemory( g3DactSys->g3DactTCBLtbl );
	GFL_TCBL_SysExit( g3DactSys->g3DactTCBLsys );
	GFL_HEAP_FreeMemory( g3DactSys );
}

//--------------------------------------------------------------------------------------------
/**
 * �A�N�^�[�|�C���^���h�m�c�d�w���擾
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 * @param	idx				�A�N�^�[�z�u�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_ACTOR*
	GFL_G3D_G3DActorGet
		( GFL_G3D_ACTSYS* g3DactSys, u32 idx )
{
	GF_ASSERT( idx < g3DactSys->g3DactorMax );
	return ( GFL_G3D_ACTOR* )GFL_TCBL_GetWork( g3DactSys->g3DactTCBLtbl[ idx ] );
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐��i���[�J���j
 * 
 * ���̊֐����o�R���Ċe�A�N�^�[����֐����Ăяo���A�߂��Ă���
 */
//--------------------------------------------------------------------------------------------
static void actfunc( GFL_TCBL* tcbl, void* work )
{
	GFL_G3D_ACTOR* g3Dact = (GFL_G3D_ACTOR*)work;

	if( g3Dact->actData.func != NULL ){
		g3Dact->actData.func( g3Dact, (void*)((u32)work+sizeof(GFL_G3D_ACTOR)));
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �\�[�g�֐��i���[�J���j
 */
//--------------------------------------------------------------------------------------------
static void objDrawSort( GFL_G3D_ACTSYS* g3DactSys )
{
	int		i;
	u16		idx1,idx2;
	u8		pri1,pri2;
	BOOL	sw;
	int		count = 0;

	for( i=0; i<g3DactSys->g3DactorMax; i++ ){
		if( g3DactSys->g3DactTCBLtbl[i] != NULL ){
			GFL_G3D_ActorDrawSWGet( GFL_TCBL_GetWork(g3DactSys->g3DactTCBLtbl[i]), &sw );
			if( sw == TRUE ){
				g3DactSys->g3DactPriTbl[ count ] = i;
				count++;
			}
		}
	}
	for( i=count; i<g3DactSys->g3DactorMax; i++ ){
		g3DactSys->g3DactPriTbl[i] = 0xffff;
	}
	while( count ){
		for( i=0; i<count-1; i++ ){
			idx1 = g3DactSys->g3DactPriTbl[i];
			idx2 = g3DactSys->g3DactPriTbl[i+1];

			GFL_G3D_ActorDrawPriGet( GFL_TCBL_GetWork(g3DactSys->g3DactTCBLtbl[idx1]), &pri1 );
			GFL_G3D_ActorDrawPriGet( GFL_TCBL_GetWork(g3DactSys->g3DactTCBLtbl[idx2]), &pri2 );

			if( pri1 > pri2 ){
				g3DactSys->g3DactPriTbl[i] = idx2;
				g3DactSys->g3DactPriTbl[i+1] = idx1;
			}
		}
		count--;
	}
}





//=============================================================================================
/**
 *
 *
 * �A�N�^�[
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �쐬
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 * @param	actTbl			�A�N�^�[�ݒ�f�[�^
 * @param	actCount		�A�N�^�[��
 *
 * @return	idx				�A�N�^�[�z�u�擪�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
u32
	GFL_G3D_ActorAdd
		( GFL_G3D_ACTSYS* g3DactSys, const GFL_G3D_ACTOR_DATA* actTbl, const u16 actCount )
{
	GFL_TCBL*		g3DactTCBL;
	GFL_G3D_ACTOR*	g3Dact;
	u32 pos;
	int	i;

	GF_ASSERT( g3DactSys );
	//�쐬INDEX�m��
	pos = GFL_AREAMAN_ReserveAuto( g3DactSys->g3DactAreaman, actCount );
	GF_ASSERT_MSG(( pos != AREAMAN_POS_NOTFOUND ), "cannot reserve resource block" );

	for( i=0; i<actCount; i++ ){
		g3DactTCBL = GFL_TCBL_Create( g3DactSys->g3DactTCBLsys, actfunc,
										g3DactSys->g3DactWorkSize,
										actTbl[i].movePriority );	
		GF_ASSERT( g3DactTCBL );
		g3DactSys->g3DactTCBLtbl[ pos+i ] = g3DactTCBL;

		g3Dact = GFL_TCBL_GetWork( g3DactTCBL );
		g3Dact->actData		= actTbl[i];
		g3Dact->actworkEx	= NULL;
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 *
 * @param	g3DactSys		�V�X�e���|�C���^
 * @param	idx				�A�N�^�[�z�u�擪�h�m�c�d�w
 * @param	resCount		�A�N�^�[��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorDel
		( GFL_G3D_ACTSYS* g3DactSys, u32 idx, const u16 actCount )
{
	GFL_TCBL* g3DactTCBL;
	int	i;

	GF_ASSERT( g3DactSys );
	for( i=0; i<actCount; i++ ){
		g3DactTCBL = g3DactSys->g3DactTCBLtbl[ idx+i ];
		if( g3DactTCBL ){
			GFL_TCBL_Delete( g3DactTCBL );
			//�h�m�c�d�w�e�[�u������폜
			g3DactSys->g3DactTCBLtbl[ idx+i ] = NULL;
		}
	}
	GFL_AREAMAN_Release( g3DactSys->g3DactAreaman, idx, actCount );
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�h�c�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	objID			�I�u�W�F�N�g�h�c�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorObjIDGet
		( GFL_G3D_ACTOR* g3Dact, u16* objID )
{
	*objID = g3Dact->actData.objID;
}

void
	GFL_G3D_ActorObjIDSet
		( GFL_G3D_ACTOR* g3Dact, u16* objID )
{
	g3Dact->actData.objID = *objID;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��v���C�I���e�B�[�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	drawPri			�`��v���C�I���e�B�[�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorDrawPriGet
		( GFL_G3D_ACTOR* g3Dact, u8* drawPri )
{
	*drawPri = g3Dact->actData.drawPriority;
}

void
	GFL_G3D_ActorDrawPriSet
		( GFL_G3D_ACTOR* g3Dact, u8* drawPri )
{
	g3Dact->actData.drawPriority = *drawPri;
}

//--------------------------------------------------------------------------------------------
/**
 * �`��X�C�b�`�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	drawSW			�`��X�C�b�`�̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorDrawSWGet
		( GFL_G3D_ACTOR* g3Dact, BOOL* drawSW )
{
	*drawSW = g3Dact->actData.drawSW;
}

void
	GFL_G3D_ActorDrawSWSet
		( GFL_G3D_ACTOR* g3Dact, BOOL* drawSW )
{
	g3Dact->actData.drawSW = *drawSW;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�ʒu���j�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	trans			�ʒu���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorStatusTransGet
		( GFL_G3D_ACTOR* g3Dact, VecFx32* trans )
{
	*trans = g3Dact->actData.status.trans;
}

void
	GFL_G3D_ActorStatusTransSet
		( GFL_G3D_ACTOR* g3Dact, VecFx32* trans )
{
	g3Dact->actData.status.trans = *trans;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i�X�P�[�����j�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorStatusScaleGet
		( GFL_G3D_ACTOR* g3Dact, VecFx32* scale )
{
	*scale = g3Dact->actData.status.scale;
}

void
	GFL_G3D_ActorStatusScaleSet
		( GFL_G3D_ACTOR* g3Dact, VecFx32* scale )
{
	g3Dact->actData.status.scale = *scale;
}

//--------------------------------------------------------------------------------------------
/**
 * �X�e�[�^�X�i��]���j�̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	rotate			��]���̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorStatusRotateGet
		( GFL_G3D_ACTOR* g3Dact, MtxFx33* rotate )
{
	*rotate = g3Dact->actData.status.rotate;
}

void
	GFL_G3D_ActorStatusRotateSet
		( GFL_G3D_ACTOR* g3Dact, MtxFx33* rotate )
{
	g3Dact->actData.status.rotate = *rotate;
}

//--------------------------------------------------------------------------------------------
/**
 * ����֐��̎擾�ƕύX
 *
 * @param	g3Dact			�A�N�^�[�|�C���^
 * @param	scale			�X�P�[�����̊i�[�������͎Q�ƃ��[�N�|�C���^
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_ActorFuncGet
		( GFL_G3D_ACTOR* g3Dact, GFL_G3D_ACTFUNC** func )
{
	*func = g3Dact->actData.func;
}

void
	GFL_G3D_ActorFuncSet
		( GFL_G3D_ACTOR* g3Dact, GFL_G3D_ACTFUNC** func )
{
	g3Dact->actData.func = *func;
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
