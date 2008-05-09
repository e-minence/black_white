//============================================================================================
/**
 * @file	act3d.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"
#include "include\system\gfl_use.h"

//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _SCENE_ACTSYS {
	GAME_SYSTEM*	gs;
	GFL_G3D_SCENE*	g3Dscene;
	u16				g3DutilUnitIdx;
	int				actID;
	int				actCount;
};

struct _SCENE_ACT {
	GFL_G3D_SCENE*	g3Dscene;
	u16				g3DutilUnitIdx;
	int				actID;
	int				actCount;

	int				seq;

	VecFx32			rotateVec;
	u8				animeCmd;
	u8				animeCmdNow;
	u8				animeID;
	u8				accesoryID;
	u8				accesoryIDNow;
	BOOL			busyFlag;
	BOOL			anmSetFlag;
};

#define ACT_FREEWORK_SIZ (15)
typedef struct { 
	SCENE_ACT*		sceneAct;
	u32				work[ ACT_FREEWORK_SIZ ];
}ACT_WORK;//setup.c�Ŏw�肳��Ă���T�C�Y�Ɏ��߂�悤�ɒ���

enum {
	ACTANMSEQ_INC_LOOP = 0,
	ACTANMSEQ_INC_END,
	ACTANMSEQ_INC_END_NEXT,
	ACTANMSEQ_INC_END_PREV,

	ACTANMSEQ_DEC_LOOP,
	ACTANMSEQ_DEC_END,
	ACTANMSEQ_DEC_END_NEXT,
	ACTANMSEQ_DEC_END_PREV,
};

typedef struct {
	int animeID; 
	int animeSeq; 
	int start; 
	int end; 
	int rate; 
}ANMIDATA_TBL;

#define SYNC_WAIT (1)

#include "graphic_data/test_graphic/test3d.naix"
//------------------------------------------------------------------
/**
 *
 *
 *
 * @brief	�V�X�e������ы��ʃ��\�[�X�̃Z�b�g�A�b�v�Ɣj��
 *
 *
 *
 */
//------------------------------------------------------------------
//�R�c�O���t�B�b�N���\�[�X�e�[�u��
enum {
	G3DRES_COMMON_NPC_RIZ_BCA = 0,
};

static const GFL_G3D_UTIL_RES g3DresTblCommon[] = {
	{ ARCID_SAMPLE, NARC_haruka_riz_nsbca,  GFL_G3D_UTIL_RESARC },
};

//---------------------
//�R�c�I�u�W�F�N�g�A�j���[�V������`�e�[�u��
enum {
	G3DANM_COMMON_NPC_RIZ,
};

static const GFL_G3D_UTIL_ANM g3DanmTblCommon[] = {
	{ G3DRES_COMMON_NPC_RIZ_BCA, 0 },
};

//---------------------
//g3Dscene �����ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3DsetupCommon = {
	g3DresTblCommon, NELEMS(g3DresTblCommon),
	NULL, 0,
};

SCENE_ACTSYS*	SceneActSys;

SCENE_ACTSYS*	Create3DactSys( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SCENE_ACTSYS* sceneActSys = GFL_HEAP_AllocMemory( heapID, sizeof(SCENE_ACTSYS) );
	sceneActSys->g3Dscene = g3Dscene;

	//�R�c�f�[�^�Z�b�g�A�b�v
	sceneActSys->g3DutilUnitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3DsetupCommon );

	SceneActSys = sceneActSys;
	return sceneActSys;
}

void			Delete3DactSys( SCENE_ACTSYS* sceneActSys )
{
	GFL_G3D_SCENE_DelG3DutilUnit( sceneActSys->g3Dscene, sceneActSys->g3DutilUnitIdx );
	GFL_HEAP_FreeMemory( sceneActSys );
}

//------------------------------------------------------------------
/**
 *
 *
 *
 * @brief	���[�J�����ʊ֐�
 *
 *
 *
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief		��]�}�g���N�X�ϊ�
 */
//------------------------------------------------------------------
static inline void rotateCalc( VecFx32* rotSrc, MtxFx33* rotDst )
{
	MtxFx33 tmp;

	MTX_RotX33(	rotDst, FX_SinIdx((u16)rotSrc->x), FX_CosIdx((u16)rotSrc->x) );

	MTX_RotY33(	&tmp, FX_SinIdx((u16)rotSrc->y), FX_CosIdx((u16)rotSrc->y) );
	MTX_Concat33( rotDst, &tmp, rotDst );

	MTX_RotZ33(	&tmp, FX_SinIdx((u16)rotSrc->z), FX_CosIdx((u16)rotSrc->z) );
	MTX_Concat33( rotDst, &tmp, rotDst );
}

//------------------------------------------------------------------
/**
 * @brief		���[�N������
 */
//------------------------------------------------------------------
static void initActWork( SCENE_ACT*	sceneAct )
{
	ACT_WORK*	work;
	int i, j;

	sceneAct->animeCmd		= 0;
	sceneAct->animeCmdNow	= sceneAct->animeCmd;
	sceneAct->animeID		= 0;
	sceneAct->accesoryID	= 0;
	sceneAct->accesoryIDNow	= sceneAct->accesoryID;
	sceneAct->busyFlag		= FALSE;
	sceneAct->anmSetFlag	= TRUE;

	for( i=0; i<sceneAct->actCount; i++ ){
		work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork
					( GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID+i ) ); 
		work->sceneAct = sceneAct;
		for( j=0; j<ACT_FREEWORK_SIZ; j++ ){
			work->work[j] = 0;
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief		���ʃA�j�����֘A�t����
 */
//------------------------------------------------------------------
static void bindCommonAnm
	( u16 objUnitIdx, u16 objIdx, const GFL_G3D_UTIL_ANM* anmTbl, u32 anmCount )
{
	//���ʃ��\�[�X����A�j���ݒ�
	GFL_G3D_UTIL_SetObjAnotherUnitAnime(	GFL_G3D_SCENE_GetG3Dutil( SceneActSys->g3Dscene ), 
											objUnitIdx, objIdx,
											SceneActSys->g3DutilUnitIdx, anmTbl, anmCount );
	//�A�j���[�V�����̏����L���ݒ�(0�Ԃ̃A�j���[�V������L���ɂ���)
	GFL_G3D_OBJECT_EnableAnime
		( GFL_G3D_SCENE_GetG3DutilObjHandle( SceneActSys->g3Dscene, objUnitIdx + objIdx ), 0 );
}

//------------------------------------------------------------------
/**
 * @brief		�A�j���R���g���[��
 */
//------------------------------------------------------------------
static void setAnimetion( SCENE_ACT* sceneAct, int anmID, fx32 start )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	int i;

	if( anmID != sceneAct->animeID ){
		for( i=0; i<sceneAct->actCount; i++ ){
			g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
			GFL_G3D_SCENEOBJ_DisableAnime( g3DsceneObj, sceneAct->animeID );
			GFL_G3D_SCENEOBJ_EnableAnime( g3DsceneObj, anmID );
		}
		sceneAct->animeID = anmID;
	}
	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetAnimeFrame( g3DsceneObj, anmID, (int*)&start );
	}
}

static void setAnimeFrame( SCENE_ACT* sceneAct, int anmID, fx32 frame )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	int i;

	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetAnimeFrame( g3DsceneObj, anmID, (int*)&frame );
	}
}

static BOOL incAnimetion( SCENE_ACT* sceneAct, int anmID, fx32 end, fx32 rate )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	fx32	nowFrm, setFrm;
	int		i;
	BOOL	result = TRUE;;

	g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID );	//0�̂ݎQ��
	if( GFL_G3D_SCENEOBJ_GetAnimeFrame( g3DsceneObj, anmID, (int*)&nowFrm ) == FALSE ){
		return FALSE;
	}
	setFrm = nowFrm + rate;
	if( setFrm >= end ){
		setFrm = end;
		result = FALSE;
	}
	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetAnimeFrame( g3DsceneObj, anmID, (int*)&setFrm );
	}
	return result;
}

static BOOL decAnimetion( SCENE_ACT* sceneAct, int anmID, fx32 end, fx32 rate )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	fx32	nowFrm, setFrm;
	int		i;
	BOOL	result = TRUE;;

	g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID );	//0�̂ݎQ��
	if( GFL_G3D_SCENEOBJ_GetAnimeFrame( g3DsceneObj, anmID, (int*)&nowFrm ) == FALSE ){
		return FALSE;
	}
	setFrm = nowFrm - rate;
	if( setFrm <= end ){
		setFrm = end;
		result = FALSE;
	}
	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetAnimeFrame( g3DsceneObj, anmID, (int*)&setFrm );
	}
	return result;
}

//------------------------------------------------------------------
static void animeControl( SCENE_ACT* sceneAct, const ANMIDATA_TBL* anmDataTable )
{
	int			anmID;
	fx32		start, end, rate;

	if( sceneAct->anmSetFlag == TRUE ){	//�A�j���X�V�`�F�b�N
		anmID = anmDataTable[ sceneAct->animeCmd ].animeID;
		start = FX32_ONE * anmDataTable[ sceneAct->animeCmd ].start;
		sceneAct->seq = anmDataTable[ sceneAct->animeCmd ].animeSeq;
		setAnimetion( sceneAct, anmID, start );
		sceneAct->animeCmdNow = sceneAct->animeCmd;
		sceneAct->anmSetFlag = FALSE;
	}
	start	= FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].start;
	end		= FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].end;
	rate	= FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].rate;

	switch( sceneAct->seq ){

	case ACTANMSEQ_INC_LOOP:
		if( incAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			setAnimeFrame( sceneAct, sceneAct->animeID, start );
		}
		sceneAct->busyFlag = FALSE;
		break;

	case ACTANMSEQ_INC_END:
		if( incAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->busyFlag = FALSE;
		} else {
			sceneAct->busyFlag = TRUE;
		}
		break;

	case ACTANMSEQ_INC_END_NEXT:
		if( incAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->animeCmdNow++;
			anmID = anmDataTable[ sceneAct->animeCmdNow ].animeID;
			start = FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].start;
			sceneAct->seq = anmDataTable[ sceneAct->animeCmdNow ].animeSeq;
			setAnimetion( sceneAct, anmID, start );
		}
		sceneAct->busyFlag = TRUE;
		break;

	case ACTANMSEQ_INC_END_PREV:
		if( incAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->animeCmdNow--;
			anmID = anmDataTable[ sceneAct->animeCmdNow ].animeID;
			start = FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].start;
			sceneAct->seq = anmDataTable[ sceneAct->animeCmdNow ].animeSeq;
			setAnimetion( sceneAct, anmID, start );
		}
		sceneAct->busyFlag = TRUE;
		break;

	case ACTANMSEQ_DEC_LOOP:
		if( decAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			setAnimeFrame( sceneAct, sceneAct->animeID, start );
		}
		sceneAct->busyFlag = FALSE;
		break;

	case ACTANMSEQ_DEC_END:
		if( decAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->busyFlag = FALSE;
		} else {
			sceneAct->busyFlag = TRUE;
		}
		break;

	case ACTANMSEQ_DEC_END_NEXT:
		if( decAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->animeCmdNow++;
			anmID = anmDataTable[ sceneAct->animeCmdNow ].animeID;
			start = FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].start;
			sceneAct->seq = anmDataTable[ sceneAct->animeCmdNow ].animeSeq;
			setAnimetion( sceneAct, anmID, start );
		}
		sceneAct->busyFlag = TRUE;
		break;

	case ACTANMSEQ_DEC_END_PREV:
		if( decAnimetion( sceneAct, sceneAct->animeID, end, rate ) == FALSE ){
			sceneAct->animeCmdNow--;
			anmID = anmDataTable[ sceneAct->animeCmdNow ].animeID;
			start = FX32_ONE * anmDataTable[ sceneAct->animeCmdNow ].start;
			sceneAct->seq = anmDataTable[ sceneAct->animeCmdNow ].animeSeq;
			setAnimetion( sceneAct, anmID, start );
		}
		sceneAct->busyFlag = TRUE;
		break;
	}
}

//------------------------------------------------------------------
/**
 *
 *
 *
 * @brief	�O���[�o�����ʊ֐�
 *
 *
 *
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g�n���h���|�C���^�擾
 */
//------------------------------------------------------------------
GFL_G3D_SCENEOBJ*	Get3DactHandle( SCENE_ACT* sceneAct, u32 idx )
{
	return GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + idx );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g���[�N�|�C���^�擾
 */
//------------------------------------------------------------------
void*	Get3DactWork( SCENE_ACT* sceneAct, u32 idx )
{
	return GFL_G3D_SCENEOBJ_GetWork( GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, idx ) );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g�`��ON/OFF
 */
//------------------------------------------------------------------
void	Set3DactDrawSw( SCENE_ACT* sceneAct, BOOL sw )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	int i;

	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetDrawSW( g3DsceneObj, &sw );
	}
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g���W�̐ݒ�
 */
//------------------------------------------------------------------
void	Set3DactTrans( SCENE_ACT* sceneAct, VecFx32* trans )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	int i;

	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, trans );
	}
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g��]�̐ݒ�
 */
//------------------------------------------------------------------
void	Set3DactRotate( SCENE_ACT* sceneAct, VecFx32* rotate )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	MtxFx33 rotateMtx;
	int i;

	sceneAct->rotateVec = *rotate;
	rotateCalc( &sceneAct->rotateVec, &rotateMtx );

	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetRotate( g3DsceneObj, &rotateMtx );
	}
}

//------------------------------------------------------------------
/**
 * @brief		���u�����h�̐ݒ�
 */
//------------------------------------------------------------------
void	Set3DactBlendAlpha( SCENE_ACT* sceneAct, u8* alpha )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj;
	int i;

	for( i=0; i<sceneAct->actCount; i++ ){
		g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, sceneAct->actID + i );
		GFL_G3D_SCENEOBJ_SetBlendAlpha( g3DsceneObj, alpha );
	}
}


//------------------------------------------------------------------
/**
 * @brief		�A�j���ݒ�
 */
//------------------------------------------------------------------
BOOL	Set3DactChrAnimeCmd( SCENE_ACT* sceneAct, u8 animeCmd )
{
	if( animeCmd != sceneAct->animeCmd ){	//�A�j���X�V�`�F�b�N
		sceneAct->animeCmd = animeCmd;
		sceneAct->anmSetFlag = TRUE;
		return TRUE; 
	} else {
		return FALSE; 
	}
}

void	Set3DactChrAnimeForceCmd( SCENE_ACT* sceneAct, u8 animeCmd )
{
	sceneAct->animeCmd = animeCmd;
	sceneAct->anmSetFlag = TRUE;
}

BOOL	Check3DactBusy( SCENE_ACT* sceneAct )
{
	return sceneAct->busyFlag;
}

//------------------------------------------------------------------
/**
 * @brief		�A�N�Z�T���[�R���g���[��
 */
//------------------------------------------------------------------
void	Set3DactChrAccesory( SCENE_ACT* sceneAct, u8 accesoryID )
{
	sceneAct->accesoryID = accesoryID;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
/**
 *
 *
 *
 * @brief	�m�o�b
 *
 *
 *
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
static void moveRiz( GFL_G3D_SCENEOBJ* sceneObj, void* work );
//------------------------------------------------------------------
/**
 * @brief	�Z�b�g�A�b�v
 */
//------------------------------------------------------------------
//�R�c�O���t�B�b�N���\�[�X�e�[�u��
enum {
	G3DRES_NPC_RIZ_BMD = 0,
};

static const GFL_G3D_UTIL_RES g3DNPCresTbl[] = {
	{ ARCID_SAMPLE, NARC_haruka_riz_nsbmd,  GFL_G3D_UTIL_RESARC },
};

//---------------------
enum {
	NPC_ANM = 0,
};

//�R�c�I�u�W�F�N�g�A�j���[�V������`�e�[�u��
static const GFL_G3D_UTIL_ANM g3DNPCanmTbl[] = {
	{ GFL_G3D_UTIL_NORESOURCE_ID, 0 },
};

//---------------------
//�R�c�I�u�W�F�N�g��`�e�[�u��
enum {
	G3DOBJ_NPC_RIZ = 0,
};

static const GFL_G3D_UTIL_OBJ g3DNPCobjTbl[] = {
	{ G3DRES_NPC_RIZ_BMD,0,G3DRES_NPC_RIZ_BMD,g3DNPCanmTbl,NELEMS(g3DNPCanmTbl)},
};

//---------------------
//g3Dscene �ݒ�e�[�u���f�[�^
static const GFL_G3D_UTIL_SETUP g3DNPCsetup = {
	g3DNPCresTbl, NELEMS(g3DNPCresTbl),
	g3DNPCobjTbl, NELEMS(g3DNPCobjTbl),
};

//------------------------------------------------------------------
//g3Dscene �I�u�W�F�N�g�ݒ�e�[�u���f�[�^
static const GFL_G3D_SCENEOBJ_DATA g3DsceneObjNPCData[] = {
	{
		G3DOBJ_NPC_RIZ, 0, 1, 31, TRUE, FALSE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE*2, FX32_ONE*2, FX32_ONE*2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},moveRiz,
	},
};

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g����
 */
//------------------------------------------------------------------
SCENE_ACT* Create3DactNPC( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SCENE_ACT*	sceneAct = GFL_HEAP_AllocMemory( heapID, sizeof(SCENE_ACT) );
	u16			objUnitIdx;
	int			i;

	//�R�c�f�[�^�Z�b�g�A�b�v
	sceneAct->g3DutilUnitIdx = GFL_G3D_SCENE_AddG3DutilUnit( g3Dscene, &g3DNPCsetup );
	objUnitIdx = GFL_G3D_SCENE_GetG3DutilUnitObjIdx( g3Dscene, sceneAct->g3DutilUnitIdx );

	//�m�o�b�L�����쐬
	sceneAct->actCount = NELEMS(g3DsceneObjNPCData);
	sceneAct->actID = GFL_G3D_SCENEOBJ_Add
					( g3Dscene, g3DsceneObjNPCData, NELEMS(g3DsceneObjNPCData), objUnitIdx );
	sceneAct->g3Dscene = g3Dscene;

	initActWork( sceneAct );

	//�A�j�����֘A�t����
	bindCommonAnm( objUnitIdx, G3DOBJ_NPC_RIZ, &g3DanmTblCommon[ G3DANM_COMMON_NPC_RIZ ], 1 );
	
	Set3DactDrawSw( sceneAct, TRUE );
	Set3DactChrAnimeCmd( sceneAct, 0 );

	return sceneAct;
}
	
//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	Delete3DactNPC( SCENE_ACT* sceneAct )
{
	GFL_G3D_SCENEOBJ_Remove( sceneAct->g3Dscene, sceneAct->actID, sceneAct->actCount ); 
	GFL_G3D_SCENE_DelG3DutilUnit( sceneAct->g3Dscene, sceneAct->g3DutilUnitIdx );
	GFL_HEAP_FreeMemory( sceneAct );
}

//------------------------------------------------------------------
static const ANMIDATA_TBL rizAnmDataTable[] = {
	{ NPC_ANM,		ACTANMSEQ_INC_END_NEXT,	 140,  150,	 1 * SYNC_WAIT },	//STAY1
	{ NPC_ANM,		ACTANMSEQ_DEC_END_PREV,	 150,  140,	 1 * SYNC_WAIT },	//STAY2
	{ NPC_ANM,		ACTANMSEQ_INC_LOOP,		1000, 1200,	 8 * SYNC_WAIT },	//WALK
	{ NPC_ANM,		ACTANMSEQ_INC_END,		   0,  150,	 4 * SYNC_WAIT },	//WAKEUP
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 800,  900,	 2 * SYNC_WAIT },	//SLEEP
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 150,  250,	 8 * SYNC_WAIT },	//CRY
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 250,  300,	 4 * SYNC_WAIT },	//FLYUPSTART1
	{ NPC_ANM,		ACTANMSEQ_INC_END_NEXT,	 300,  350,	 8 * SYNC_WAIT },	//FLYUPSTART2
	{ NPC_ANM,		ACTANMSEQ_INC_END_NEXT,	 350,  400,	 4 * SYNC_WAIT },	//FLYUP
	{ NPC_ANM,		ACTANMSEQ_DEC_END_PREV,	 400,  350,	 4 * SYNC_WAIT },	//FLYUP
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 600,  650,	 8 * SYNC_WAIT },	//FLY
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 400,  550,	 8 * SYNC_WAIT },	//ATTACK
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 650,  700,	 8 * SYNC_WAIT },	//FLYDOWN
	{ NPC_ANM,		ACTANMSEQ_INC_END,		 700,  800,	 8 * SYNC_WAIT },	//FLYDOWNEND
};

//------------------------------------------------------------------
static void moveRiz( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	ACT_WORK*	actWork = (ACT_WORK*)work;
	SCENE_ACT*	sceneAct = actWork->sceneAct;
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get(sceneAct->g3Dscene, sceneAct->actID);
//-------------------
#if 0
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		u32	anmPat = GFUser_GetPublicRand( NELEMS(rizAnmDataTable) );
		Set3DactChrAnimeCmd( sceneAct, anmPat );
	}
#endif
	if( actWork->work[0] == 0 ){
		u32	timer = GFUser_GetPublicRand( 8 );

		actWork->work[0] = (timer+2) * 30;
		actWork->work[1] = GFUser_GetPublicRand( 0x10000 );
	} else {
		actWork->work[0]--;
	}
	{
		VecFx32 nowTrans, nextTrans, vecMove, rotVec;
		u16	 theta = actWork->work[1];
		fx32 speed = 1 * FX32_ONE;

		GFL_G3D_SCENEOBJ_GetPos( g3DsceneObj, &nowTrans );

		vecMove.x = FX_SinIdx( theta );
		vecMove.y = 0;
		vecMove.z = FX_CosIdx( theta );

		//�͈͊O�R���g���[���i�{���͊O���֐��ł��j
		if( DEBUG_CheckGroundMove( &nowTrans, &vecMove, &nextTrans ) == TRUE ){
			Set3DactTrans( sceneAct, &nextTrans );
		} else {
			actWork->work[0] = 0;
		}
		rotVec.x = 0;
		rotVec.y = actWork->work[1];
		rotVec.z = 0;

		Set3DactRotate( sceneAct, &rotVec );
	}
	Set3DactChrAnimeCmd( sceneAct, 2 );
//-------------------
	animeControl( sceneAct, rizAnmDataTable );
}


