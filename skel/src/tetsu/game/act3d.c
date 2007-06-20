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
//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _SCENE_ACT {
	GFL_G3D_SCENE*	g3Dscene;
	int				actID;
	int				actCount;
};

typedef struct { 
	int			seq;
	VecFx32		rotateVec;
	u8			animeCmd;
	u8			animeCmdNow;
	u8			animeID;
	u8			accesoryID;
	u8			accesoryIDNow;
	BOOL		busyFlag;
}ACT_WORK;//setup.c�Ŏw�肳��Ă���T�C�Y�Ɏ��߂�悤�ɒ���

static void movePlayer( GFL_G3D_SCENEOBJ* sceneObj, void* work );

//��]�}�g���N�X�ϊ�
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
 * @brief	�R�c�f�[�^
 */
//------------------------------------------------------------------
//g3Dscene �I�u�W�F�N�g�ݒ�e�[�u���f�[�^
static const GFL_G3D_SCENEOBJ_DATA g3DsceneObjData[] = {
	{	G3DOBJ_HUMAN2, 0, 1, 31, FALSE, TRUE, 
		{	{ 0, 0, 0 },
			{ FX32_ONE/4, FX32_ONE/4, FX32_ONE/4 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},movePlayer,
	},
};

//---------------------
//g3Dscene �A�N�Z�T���[�ݒ�e�[�u���f�[�^
static const GFL_G3D_SCENEACCESORY_DATA g3DsceneAccesoryData1[] = {
	{	G3DOBJ_ACCE_SWORD, 31,
		{	{ 0, 0, 0 },
			{ FX32_ONE/2, FX32_ONE/2, FX32_ONE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},28	//�E��jointID
	},
	{	G3DOBJ_ACCE_SHIELD, 31,
		{	{ -FX32_ONE*8, FX32_ONE*4, 0 },
			{ FX32_ONE/2, FX32_ONE/2, FX32_ONE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},20	//����jointID
	},
};

static const GFL_G3D_SCENEACCESORY_DATA g3DsceneAccesoryData2[] = {
	{	G3DOBJ_ACCE_BOW, 31,
		{	{ 0, 0, 0 },
			{ FX32_ONE/2, FX32_ONE/2, FX32_ONE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},20	//����jointID
	},
};

static const GFL_G3D_SCENEACCESORY_DATA g3DsceneAccesoryData3[] = {
	{	G3DOBJ_ACCE_STAFF, 31,
		{	{ 0, 0, 0 },
			{ FX32_ONE/2, FX32_ONE/2, FX32_ONE/2 },
			{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },
		},28	//�E��jointID
	},
};

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g����
 */
//------------------------------------------------------------------
SCENE_ACT* Create3Dact( GFL_G3D_SCENE* g3Dscene, HEAPID heapID )
{
	SCENE_ACT* sceneAct = GFL_HEAP_AllocMemory( heapID, sizeof(SCENE_ACT) );

	//���@�쐬
	sceneAct->actID = GFL_G3D_SCENEOBJ_Add
						( g3Dscene, g3DsceneObjData, NELEMS(g3DsceneObjData) );
	sceneAct->g3Dscene = g3Dscene;
	sceneAct->actCount = NELEMS(g3DsceneObjData);

	return sceneAct;
}
	
//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g�j��
 */
//------------------------------------------------------------------
void	Delete3Dact( SCENE_ACT* sceneAct )
{
	GFL_G3D_SCENEOBJ_Remove( sceneAct->g3Dscene, sceneAct->actID, sceneAct->actCount ); 
	GFL_HEAP_FreeMemory( sceneAct );
}

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
 * @brief		�R�c�I�u�W�F�N�g���W�̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void	Get3DactTrans( SCENE_ACT* sceneAct, u32 idx, VecFx32* trans )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, 
														sceneAct->actID + idx );
	GFL_G3D_SCENEOBJ_GetPos( g3DsceneObj, trans );
}

void	Set3DactTrans( SCENE_ACT* sceneAct, u32 idx, VecFx32* trans )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, 
														sceneAct->actID + idx );
	GFL_G3D_SCENEOBJ_SetPos( g3DsceneObj, trans );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g��]�̎擾�Ɛݒ�
 */
//------------------------------------------------------------------
void	Get3DactRotate( SCENE_ACT* sceneAct, u32 idx, VecFx32* rotate )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, 
														sceneAct->actID + idx );
	ACT_WORK* work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork( g3DsceneObj );

	*rotate = work->rotateVec;
}

void	Set3DactRotate( SCENE_ACT* sceneAct, u32 idx, VecFx32* rotate )
{
	GFL_G3D_SCENEOBJ* g3DsceneObj = GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene, 
														sceneAct->actID + idx );
	ACT_WORK* work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork( g3DsceneObj );
	MtxFx33			rotateMtx;

	work->rotateVec = *rotate;
	rotateCalc( &work->rotateVec, &rotateMtx );
	GFL_G3D_SCENEOBJ_SetRotate( g3DsceneObj, &rotateMtx );
}

//------------------------------------------------------------------
/**
 * @brief		�R�c�I�u�W�F�N�g����R���g���[��
 */
//------------------------------------------------------------------
//------------------------------------------------------------------
//
//	�v���[���[�L����
//
//------------------------------------------------------------------
void	Set3DactPlayerChrAnimeCmd( SCENE_ACT* sceneAct, u32 idx, u8 animeCmd )
{
	ACT_WORK* work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork
						( GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene,sceneAct->actID + idx )); 
	work->animeCmd = animeCmd;
}

void	Set3DactPlayerChrAccesory( SCENE_ACT* sceneAct, u32 idx, u8 accesoryID )
{
	ACT_WORK* work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork
						( GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene,sceneAct->actID + idx )); 
	work->accesoryID = accesoryID;
}

BOOL	Check3DactPlayerBusy( SCENE_ACT* sceneAct, u32 idx )
{
	ACT_WORK* work = (ACT_WORK*)GFL_G3D_SCENEOBJ_GetWork
						( GFL_G3D_SCENEOBJ_Get( sceneAct->g3Dscene,sceneAct->actID + idx )); 
	return work->busyFlag;
}

//------------------------------------------------------------------
enum {
	ACTANMSEQ_PLAYER_SET = 0,
	ACTANMSEQ_PLAYER_STAY,
	ACTANMSEQ_PLAYER_WALK,
	ACTANMSEQ_PLAYER_RUN,
	ACTANMSEQ_PLAYER_ATTACK,
	ACTANMSEQ_PLAYER_SHOOT,
	ACTANMSEQ_PLAYER_SPELL,
	ACTANMSEQ_PLAYER_SITDOWN,
	ACTANMSEQ_PLAYER_STANDUP,
	ACTANMSEQ_PLAYER_TAKE,
};

typedef struct {
	int animeID; 
	int animeSeq; 
	int start; 
	int end; 
	int wait; 
}PLAYER_ANMIDATA_TBL;

static const PLAYER_ANMIDATA_TBL anmDataTable[] = {
	{ HUMAN2_ANM_STAY,		ACTANMSEQ_PLAYER_STAY,		0,	0,	2 },	//ACTANM_PLAYER_STAY
	{ HUMAN2_ANM_WALK,		ACTANMSEQ_PLAYER_WALK,		0,	0,	2 },	//ACTANM_PLAYER_WALK
	{ HUMAN2_ANM_RUN,		ACTANMSEQ_PLAYER_RUN,		0,	0,	4 },	//ACTANM_PLAYER_RUN
	{ HUMAN2_ANM_ATTACK,	ACTANMSEQ_PLAYER_ATTACK,	0,	0,	2 },	//ACTANM_PLAYER_ATTACK
	{ HUMAN2_ANM_SHOOT,		ACTANMSEQ_PLAYER_SHOOT,		0,	0,	2 },	//ACTANM_PLAYER_SHOOT
	{ HUMAN2_ANM_SPELL,		ACTANMSEQ_PLAYER_SPELL,		0,	0,	2 },	//ACTANM_PLAYER_SPELL
	{ HUMAN2_ANM_SIT,		ACTANMSEQ_PLAYER_SITDOWN,	0,	20,	1 },	//ACTANM_PLAYER_SITDOWN
	{ HUMAN2_ANM_SIT,		ACTANMSEQ_PLAYER_STANDUP,	20,	0,	1 },	//ACTANM_PLAYER_STANDUP
	{ HUMAN2_ANM_SIT,		ACTANMSEQ_PLAYER_TAKE,		0,	0,	1 },	//ACTANM_PLAYER_TAKE
};

static void movePlayer( GFL_G3D_SCENEOBJ* sceneObj, void* work )
{
	ACT_WORK*	pw = (ACT_WORK*)work;
	int			anmID;
	fx32		start, end, wait;

	if( pw->animeCmd != pw->animeCmdNow ){	//�A�j���X�V�`�F�b�N
		anmID = anmDataTable[pw->animeCmdNow].animeID;
		GFL_G3D_SCENEOBJ_DisableAnime( sceneObj, anmID );

		pw->seq = anmDataTable[pw->animeCmd].animeSeq;
		anmID = anmDataTable[pw->animeCmd].animeID;

		GFL_G3D_SCENEOBJ_EnableAnime( sceneObj, anmID );
		start = FX32_ONE * anmDataTable[pw->animeCmd].start;
		GFL_G3D_SCENEOBJ_SetAnimeFrame( sceneObj, anmID, (int*)&start );

		pw->animeCmdNow = pw->animeCmd;
	}
	anmID	= anmDataTable[pw->animeCmdNow].animeID;
	end		= FX32_ONE * anmDataTable[pw->animeCmd].end;
	wait	= FX32_ONE * anmDataTable[pw->animeCmd].wait;

	switch( pw->seq ){
	case ACTANMSEQ_PLAYER_STAY:
	case ACTANMSEQ_PLAYER_WALK:
	case ACTANMSEQ_PLAYER_RUN:
		GFL_G3D_SCENEOBJ_LoopAnimeFrame( sceneObj, anmID, wait ); 
		pw->busyFlag = FALSE;
		break;
	case ACTANMSEQ_PLAYER_ATTACK:
	case ACTANMSEQ_PLAYER_SHOOT:
	case ACTANMSEQ_PLAYER_SPELL:
	case ACTANMSEQ_PLAYER_TAKE:
	case ACTANMSEQ_PLAYER_STANDUP:
		if( GFL_G3D_SCENEOBJ_IncAnimeFrame( sceneObj, anmID, wait ) == FALSE ){
			pw->animeCmd = ACTANM_CMD_STAY;
		} else {
			pw->busyFlag = TRUE;
		}
		break;
	case ACTANMSEQ_PLAYER_SITDOWN:
		{
			fx32 nowFrm;
			GFL_G3D_SCENEOBJ_GetAnimeFrame( sceneObj, anmID, (int*)&nowFrm );
			if( nowFrm < end ){
				GFL_G3D_SCENEOBJ_IncAnimeFrame( sceneObj, anmID, wait );
				pw->busyFlag = TRUE;
			} else {
				pw->busyFlag = FALSE;
			}
		}
		break;
	}

	if( pw->accesoryID != pw->accesoryIDNow ){	//�A�N�Z�T���[�X�V�`�F�b�N
		MtxFx33	rotate;
		VecFx32	rotVec = { 0, 0, 0 };
		BOOL result;

		//�A�N�Z�T���[���O��(�ݒ肳��ĂȂ��ꍇ�͂Ȃɂ����Ȃ��ŋA���Ă���)
		GFL_G3D_SCENEOBJ_ACCESORY_Remove( sceneObj );

		switch( pw->accesoryID ){
		case 0:	//��Ԃ�
			break;
		case 1:	//�n���}�[
			result = GFL_G3D_SCENEOBJ_ACCESORY_Add( sceneObj,
							g3DsceneAccesoryData1, NELEMS(g3DsceneAccesoryData1) );
			GF_ASSERT( result == TRUE );
			rotVec.z = 0x4000;
			rotateCalc( &rotVec, &rotate );
			GFL_G3D_SCENEOBJ_ACCESORY_SetRotate( sceneObj, 0, &rotate );
			rotVec.y = 0xc000;
			rotVec.z = 0x0000;
			rotateCalc( &rotVec, &rotate );
			GFL_G3D_SCENEOBJ_ACCESORY_SetRotate( sceneObj, 1, &rotate );
			break;
		case 2:	//�|
			result = GFL_G3D_SCENEOBJ_ACCESORY_Add( sceneObj,
							g3DsceneAccesoryData2, NELEMS(g3DsceneAccesoryData2) );
			GF_ASSERT( result == TRUE );
			rotVec.z = 0x4000;
			rotateCalc( &rotVec, &rotate );
			GFL_G3D_SCENEOBJ_ACCESORY_SetRotate( sceneObj, 0, &rotate );
			break;
		case 3:	//��
			result = GFL_G3D_SCENEOBJ_ACCESORY_Add( sceneObj,
							g3DsceneAccesoryData3, NELEMS(g3DsceneAccesoryData3) );
			GF_ASSERT( result == TRUE );
			rotVec.z = 0x4000;
			rotateCalc( &rotVec, &rotate );
			GFL_G3D_SCENEOBJ_ACCESORY_SetRotate( sceneObj, 0, &rotate );
			break;
		}
		pw->accesoryIDNow = pw->accesoryID;	//�A�N�Z�T���[�X�V
	}
}


