//=============================================================================================
/**
 * @file	g3d_util.c                                                  
 * @brief	�R�c�`��Ǘ��V�X�e���g�p�v���O����
 * @date	2006/4/26
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================
typedef struct {
	u8			priority;				
	BOOL		drawSW;
}GFL_G3D_OBJSTAT;

typedef struct {
	GFL_G3D_RES**		g3DresTbl;
	GFL_AREAMAN*		g3DresAreaman;
	u16					g3DresCount;

	GFL_G3D_OBJ**		g3DobjTbl;
	GFL_AREAMAN*		g3DobjAreaman;
	u16					g3DobjCount;
	GFL_G3D_OBJSTAT*	g3DobjStat;

	u16*				g3DobjPriTbl;

	HEAPID				heapID;
}GFL_G3D_UTIL;

GFL_G3D_UTIL* g3Dutil = NULL;

//=============================================================================================
/**
 *
 *
 * �R�c���\�[�X����уI�u�W�F�N�g�n���h���̔z��Ǘ����[�e�B���e�B�[
 *
 *
 */
//=============================================================================================
#define HANDLE_POINTER_SIZE (4)
//--------------------------------------------------------------------------------------------
/**
 * �Z�b�g�A�b�v
 *
 * @param	resourceCount	�Ǘ����\�[�X�ő吔	
 * @param	objectCount		�Ǘ��I�u�W�F�N�g�ő吔
 * @param	heapID			�q�[�v�h�c
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilsysInit
		( u32 resourceCount, u32 objectCount, HEAPID heapID )  
{
	int	i;

	GF_ASSERT( !g3Dutil );

	//�Ǘ��̈�m��
	g3Dutil = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_UTIL) );

	//���\�[�X�Ǘ��z��쐬
	g3Dutil->g3DresTbl = GFL_HEAP_AllocMemory( heapID, HANDLE_POINTER_SIZE * resourceCount );
	//���\�[�X�z��̈�}�l�[�W���쐬
	g3Dutil->g3DresAreaman = GFL_AREAMAN_Create( resourceCount, heapID );

	//�I�u�W�F�N�g�Ǘ��z��쐬
	g3Dutil->g3DobjTbl = GFL_HEAP_AllocMemory( heapID, HANDLE_POINTER_SIZE * objectCount );
	//���\�[�X�z��̈�}�l�[�W���쐬
	g3Dutil->g3DobjAreaman = GFL_AREAMAN_Create( objectCount, heapID );

	//�I�u�W�F�N�g�`��X�e�[�^�X�Ǘ��z��쐬�i�I�u�W�F�N�g�Ǘ��z��쐬�ƕ��я��͓����j
	g3Dutil->g3DobjStat = GFL_HEAP_AllocMemory( heapID, sizeof(GFL_G3D_OBJSTAT)*objectCount );
	//�I�u�W�F�N�g�`��v���C�I���e�B�[�Ǘ��z��쐬
	g3Dutil->g3DobjPriTbl = GFL_HEAP_AllocMemory( heapID, sizeof(u16)*objectCount );

	//������
	for( i=0; i<resourceCount; i++ ){
		g3Dutil->g3DresTbl[i] = NULL;
	}
	for( i=0; i<objectCount; i++ ){
		g3Dutil->g3DobjTbl[i] = NULL;

		g3Dutil->g3DobjStat[i].priority = 0;
		g3Dutil->g3DobjStat[i].drawSW = FALSE;
	}
	g3Dutil->g3DresCount = resourceCount;
	g3Dutil->g3DobjCount = objectCount;
	g3Dutil->heapID = heapID;
}

//--------------------------------------------------------------------------------------------
/**
 * �j��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilsysExit
		( void )  
{
	int	i;

	GF_ASSERT( g3Dutil );

	//�I�u�W�F�N�g�`��v���C�I���e�B�[�Ǘ��z��j��
	GFL_HEAP_FreeMemory( g3Dutil->g3DobjPriTbl );
	//�I�u�W�F�N�g�`��X�e�[�^�X�Ǘ��z��j��
	GFL_HEAP_FreeMemory( g3Dutil->g3DobjStat );

	//�I�u�W�F�N�g�z��̈�}�l�[�W���j��
	GFL_AREAMAN_Delete( g3Dutil->g3DobjAreaman );
	//����Y��`�F�b�N
	for( i=0; i<g3Dutil->g3DobjCount; i++ ){
		GF_ASSERT_MSG( !(g3Dutil->g3DobjTbl[i]),"unload resouce exist" );
	}
	//�I�u�W�F�N�g�Ǘ��z��j��
	GFL_HEAP_FreeMemory( g3Dutil->g3DobjTbl );

	//���\�[�X�z��̈�}�l�[�W���j��
	GFL_AREAMAN_Delete( g3Dutil->g3DresAreaman );
	//����Y��`�F�b�N
	for( i=0; i<g3Dutil->g3DresCount; i++ ){
		GF_ASSERT_MSG( !(g3Dutil->g3DresTbl[i]),"unload resouce exist" );
	}
	//���\�[�X�Ǘ��z��j��
	GFL_HEAP_FreeMemory( g3Dutil->g3DresTbl );

	//�Ǘ��̈���
	GFL_HEAP_FreeMemory( g3Dutil );
}





//=============================================================================================
/**
 *
 *
 * �R�c���\�[�X�Ǘ�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X��z��ɒǉ�
 *
 * @param	resTable		���\�[�X�z��|�C���^
 * @param	resCount		���\�[�X��
 *
 * @return	idx				���\�[�X�z�u�擪�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UtilResLoad
		( const GFL_G3D_UTIL_RES* resTable, u16 resCount )  
{
	int	i;
	u32 pos;

	GF_ASSERT( g3Dutil );

	pos = GFL_AREAMAN_ReserveAuto( g3Dutil->g3DresAreaman, resCount );
	GF_ASSERT_MSG(( pos != AREAMAN_POS_NOTFOUND ), "cannnot reserve resouce block" );

	//�z��ɃZ�b�g�A�b�v
	for( i=0; i<resCount; i++ ){
		if( resTable[i].arcType == GFL_G3D_UTIL_RESARC ){
			//�A�[�J�C�u�h�c���
			g3Dutil->g3DresTbl[ pos+i ] = GFL_G3D_ResourceCreateArc
										( (int)resTable[i].arcive, resTable[i].datID ); 
		} else {
			//�A�[�J�C�u�p�X���
			g3Dutil->g3DresTbl[ pos+i ] = GFL_G3D_ResourceCreatePath
										( (const char*)resTable[i].arcive, resTable[i].datID ); 
		}
	}

	//�e�N�X�`���̈ꊇ�]���i�����Ɍ�����������������Ȃ���������Ă݂�j
	{
		int	i;
		BOOL resTypeFlg;

		//�e�N�X�`���f�[�^�A���]��
		GX_BeginLoadTex();

		for( i=0; i<resCount; i++ ){
			//�e�N�X�`�����\�[�X���ǂ����m�F
			resTypeFlg = GFL_G3D_ResourceTypeCheck
							( g3Dutil->g3DresTbl[ pos+i ], GFL_G3D_RES_CHKTYPE_TEX );

			if(( resTable[i].trans == TRUE )&&( resTypeFlg == TRUE )){
				GFL_G3D_VramLoadTexDataOnly( g3Dutil->g3DresTbl[ pos+i ] );
			}
		}
		GX_EndLoadTex();
	
		//�e�N�X�`���p���b�g�A���]��
		GX_BeginLoadTexPltt();
	
		for( i=0; i<resCount; i++ ){
			//�e�N�X�`�����\�[�X���ǂ����m�F
			resTypeFlg = GFL_G3D_ResourceTypeCheck
							( g3Dutil->g3DresTbl[ pos+i ], GFL_G3D_RES_CHKTYPE_TEX );
	
			if(( resTable[i].trans == TRUE )&&( resTypeFlg == TRUE )){
				GFL_G3D_VramLoadTexPlttOnly( g3Dutil->g3DresTbl[ pos+i ] );
			}
		}
		GX_EndLoadTexPltt();
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X��z�񂩂�폜
 *
 * @param	idx				���\�[�X�z�u�擪�h�m�c�d�w
 * @param	resCount		���\�[�X��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilResUnload
		( u16 idx, u16 resCount ) 
{
	int	i;

	for( i=0; i<resCount; i++ ){
		if( g3Dutil->g3DresTbl[ i+idx ] ){
			GFL_G3D_RES* g3Dres = g3Dutil->g3DresTbl[ i+idx ];
			//�e�N�X�`�����\�[�X���ǂ����m�F
			if( GFL_G3D_ResourceTypeCheck( g3Dres, GFL_G3D_RES_CHKTYPE_TEX ) == TRUE ){
				//�u�q�`�l�]���ς݂��ǂ����m�F
				if( GFL_G3D_ObjTexkeyLiveCheck( g3Dres ) == TRUE ){
					//�u�q�`�l���
					GFL_G3D_VramUnloadTex( g3Dres );
				}
			}
			//���\�[�X�j��
			GFL_G3D_ResourceDelete( g3Dres ); 
			//�h�m�c�d�w�e�[�u������폜
			g3Dutil->g3DresTbl[ i+idx ] = NULL;
		}
	}
	GFL_AREAMAN_Release( g3Dutil->g3DresAreaman, idx, resCount );
}

//--------------------------------------------------------------------------------------------
/**
 * ���\�[�X�n���h����z�񂩂�擾
 *
 * @param	idx				���\�[�X�i�[�h�m�c�d�w
 *
 * @return	GFL_G3D_RES*	���\�[�X�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_RES*
	GFL_G3D_UtilResGet
		( u16 idx ) 
{
	if( idx >= g3Dutil->g3DresCount ){
		OS_Panic( "ID over" );
	}
	return g3Dutil->g3DresTbl[idx];
}
	
//--------------------------------------------------------------------------------------------
/**
 * �e�N�X�`�����\�[�X��z�񂩂�]��(���u�q�`�l)
 *
 * @param	idx		���\�[�X�i�[�h�m�c�d�w
 *
 * @return	BOOL	����(����=TRUE)
 */
//--------------------------------------------------------------------------------------------
BOOL 
	GFL_G3D_UtilVramLoadTex
		( u16 idx ) 
{
	if( idx >= g3Dutil->g3DresCount ){
		OS_Panic( "ID over" );
	}
	return GFL_G3D_VramLoadTex( g3Dutil->g3DresTbl[idx] );
}





//=============================================================================================
/**
 *
 *
 * �R�c�I�u�W�F�N�g�Ǘ�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g��z��ɒǉ�
 *
 * @param	objTable		�I�u�W�F�N�g�z��|�C���^
 * @param	objCount		�I�u�W�F�N�g��
 *
 * @return	idx				�I�u�W�F�N�g�z�u�擪�h�m�c�d�w
 */
//--------------------------------------------------------------------------------------------
u16
	GFL_G3D_UtilObjLoad
		( const GFL_G3D_UTIL_OBJ* objTable, u16 objCount )  
{
	int	i;
	u32 pos;

	GF_ASSERT( g3Dutil );

	pos = GFL_AREAMAN_ReserveAuto( g3Dutil->g3DobjAreaman, objCount );
	GF_ASSERT_MSG(( pos != AREAMAN_POS_NOTFOUND ), "cannnot reserve object block" );

	//�z��ɃZ�b�g�A�b�v
	for( i=0; i<objCount; i++ ){
		g3Dutil->g3DobjTbl[ pos+i ] = GFL_G3D_ObjCreate
								( g3Dutil->g3DresTbl[objTable[i].mdlresID], objTable[i].mdldatID,
								  g3Dutil->g3DresTbl[objTable[i].texresID],
								  g3Dutil->g3DresTbl[objTable[i].anmresID], objTable[i].anmdatID );

		GFL_G3D_ObjContSetTrans( (g3Dutil->g3DobjTbl)[ pos+i ], &objTable[i].trans );
		GFL_G3D_ObjContSetScale( (g3Dutil->g3DobjTbl)[ pos+i ], &objTable[i].scale );
		GFL_G3D_ObjContSetRotate( (g3Dutil->g3DobjTbl)[ pos+i ], &objTable[i].rotate );

		g3Dutil->g3DobjStat[ pos+i ].priority = objTable[i].priority;
		g3Dutil->g3DobjStat[ pos+i ].drawSW = objTable[i].drawSW;
	}
	return pos;
}

//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g��z�񂩂�폜
 *
 * @param	idx				�I�u�W�F�N�g�z�u�擪�h�m�c�d�w
 * @param	objCount		�I�u�W�F�N�g��
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilObjUnload
		( u16 idx, u16 objCount ) 
{
	int	i;

	for( i=0; i<objCount; i++ ){
		if( g3Dutil->g3DobjTbl[ i+idx ] ){
			//�I�u�W�F�N�g�j��
			GFL_G3D_ObjDelete( g3Dutil->g3DobjTbl[ i+idx ] ); 
			//�h�m�c�d�w�e�[�u������폜
			g3Dutil->g3DobjTbl[ i+idx ] = NULL;
		}
	}
	GFL_AREAMAN_Release( g3Dutil->g3DobjAreaman, idx, objCount );
}
	
//--------------------------------------------------------------------------------------------
/**
 * �I�u�W�F�N�g�n���h����z�񂩂�擾
 *
 * @param	idx				�I�u�W�F�N�g�i�[�h�m�c�d�w
 *
 * @return	GFL_G3D_OBJ*	�I�u�W�F�N�g�|�C���^
 */
//--------------------------------------------------------------------------------------------
GFL_G3D_OBJ*
	GFL_G3D_UtilObjGet
		( u16 idx ) 
{
	if( idx >= g3Dutil->g3DobjCount ){
		OS_Panic( "ID over" );
	}
	return g3Dutil->g3DobjTbl[idx];
}





//=============================================================================================
/**
 *
 *
 * �`��Ǘ�
 *
 *
 */
//=============================================================================================
//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g���X�g�̃\�[�g
 */
//--------------------------------------------------------------------------------------------
static void objDrawSort( void )
{
	int	i;
	u16	idx1,idx2;
	u8	pri1,pri2;
	u32 count = g3Dutil->g3DobjCount;

	for( i=0; i<count; i++ ){
		g3Dutil->g3DobjPriTbl[i] = i;
	}
	while( count ){
		for( i=0; i<count-1; i++ ){
			idx1 = g3Dutil->g3DobjPriTbl[i];
			idx2 = g3Dutil->g3DobjPriTbl[i+1];

			pri1 = g3Dutil->g3DobjStat[idx1].priority;
			pri2 = g3Dutil->g3DobjStat[idx2].priority;

			if( pri1 > pri2 ){
				g3Dutil->g3DobjPriTbl[i] = idx2;
				g3Dutil->g3DobjPriTbl[i+1] = idx1;
			}
		}
		count--;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * �R�c�I�u�W�F�N�g�̕`��
 *
 */
//--------------------------------------------------------------------------------------------
void
	GFL_G3D_UtilDraw
		( void )
{
	//�`��J�n
	GFL_G3D_DrawStart();
	//�J�����O���[�o���X�e�[�g�ݒ�		
 	GFL_G3D_DrawLookAt();

	objDrawSort();
	{
		int	i;
		u16 idx;
		GFL_G3D_OBJSTAT*	drawStatus;
		GFL_G3D_OBJ*		drawObj;

		for( i=0; i<g3Dutil->g3DobjCount; i++ ){
			idx = g3Dutil->g3DobjPriTbl[i];
			drawStatus = &g3Dutil->g3DobjStat[ idx ];
			drawObj = g3Dutil->g3DobjTbl[ idx ];

			if(( drawObj )&&( drawStatus->drawSW == TRUE )){
				GFL_G3D_ObjDraw( drawObj );
			}
		}
	}
	//�`��I���i�o�b�t�@�X���b�v�j
	GFL_G3D_DrawEnd();							
}





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







