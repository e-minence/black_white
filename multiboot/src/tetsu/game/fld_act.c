//============================================================================================
/**
 * @file	fld_act.c
 * @brief	�t�B�[���h�A�N�g�V�X�e��
 */
//============================================================================================
#include "gflib.h"

#include "setup.h"
#include "gfl_use.h"

//���jGFL_G3D_SYSTEM, GFL_BBD��ł̓���

//------------------------------------------------------------------
/**
 *
 * @brief	�^�錾
 *
 */
//------------------------------------------------------------------
#define FLD_BBDACT_RESMAX	(64)
#define FLD_BBDACT_ACTMAX	(256)
#define WORK_SIZ			(8)

typedef struct {
	FLD_ACTSYS*	fldActSys;
	u16			work[WORK_SIZ];
}FLD_ACT_WORK;

struct _FLD_ACTSYS {
	HEAPID					heapID;
	GFL_G3D_CAMERA*			g3Dcamera;
	GFL_G3D_LIGHTSET*		g3Dlightset;	//g3Dlight Lib �n���h��
	u16						cameraRotate;
	GFL_BBDACT_SYS*			bbdActSys;
	GFL_BBDACT_RESUNIT_ID	bbdActResUnitID;
	u16						bbdActResCount;
	GFL_BBDACT_ACTUNIT_ID	bbdActActUnitID;
	u16						bbdActActCount;
	FLD_ACT_WORK			actWork[FLD_BBDACT_ACTMAX];
};

#include "src/sample_graphic/fld_act.naix"

static void	initActWork( FLD_ACTSYS* fldActSys, FLD_ACT_WORK* actWork );
static void	calcCameraRotate( FLD_ACTSYS* fldActSys );
//------------------------------------------------------------------
/**
 *
 * @brief	�Z�b�g�A�b�v�f�[�^
 *
 */
//------------------------------------------------------------------
static GFL_BBDACT_RESUNIT_ID testResUnitID;
static GFL_BBDACT_ACTUNIT_ID testActUnitID;

static const GFL_BBDACT_RESDATA testResTable[] = {
{ ARCID_FLDACT, NARC_fld_act_achamo_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_artist_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_badman_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_beachgirl_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_idol_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_lady_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_oldman1_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_policeman_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_rivel_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
{ ARCID_FLDACT, NARC_fld_act_waiter_nsbtx, GFL_BBD_TEXFMT_PAL16, GFL_BBD_TEXSIZ_32x512, 32, 32 },
};

 

static const GFL_BBDACT_ANM walkLAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkRAnm[] = {
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkUAnm[] = {
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM walkDAnm[] = {
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 10, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm1[] = {
	{ 0, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 1, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 2, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 3, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 4, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 5, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 6, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 7, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 8, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 9, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 10, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ 11, GFL_BBDACT_ANMFLIP_OFF, GFL_BBDACT_ANMFLIP_OFF, 4 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm2[] = {
	{ 1, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 2, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 16 },
	{ 3, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 24 },
	{ 4, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 32 },
	{ 5, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 48 },
	{ 6, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 7, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 8, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 9, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 10, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ GFL_BBDACT_ANMCOM_LOOP, 0, 0, GFL_BBDACT_ANMLOOPPARAM( 4, 5 ) },
	{ 11, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 64 },
	{ GFL_BBDACT_ANMCOM_CHG, 0, 0, 0 },
};
static const GFL_BBDACT_ANM testAnm3[] = {
	{ 0, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 16, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 32, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ 64, GFL_BBDACT_ANMFLIP_ON, GFL_BBDACT_ANMFLIP_OFF, 8 },
	{ GFL_BBDACT_ANMCOM_JMP, 0, 0, 1 },
};

enum {
	ACTWALK_UP = 0,
	ACTWALK_DOWN,
	ACTWALK_LEFT,
	ACTWALK_RIGHT,
};

static const GFL_BBDACT_ANM* testAnmTable[] = { walkUAnm, walkDAnm, walkLAnm, walkRAnm };




//���C�g�����ݒ�f�[�^
static const GFL_G3D_LIGHT_DATA light0Tbl[] = {
	{ 0, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 1, {{  (FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 2, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
	{ 3, {{ -(FX16_ONE-1), -(FX16_ONE-1), -(FX16_ONE-1) }, GX_RGB(31,31,31) } },
};
static const GFL_G3D_LIGHTSET_SETUP light0Setup = { light0Tbl, NELEMS(light0Tbl) };


static void testFunc( GFL_BBDACT_SYS* bbdActSys, int actIdx, void* work )
{
	GFL_BBD_SYS*	bbdSys = GFL_BBDACT_GetBBDSystem( bbdActSys );
	FLD_ACT_WORK*	actWork = (FLD_ACT_WORK*)work;

	if( actWork->work[0] == 0 ){
		u32	timer = GFUser_GetPublicRand(8);
		u16	anm = GFUser_GetPublicRand(4);
		u8	alpha = GFUser_GetPublicRand(16)+16;

		actWork->work[0] = (timer+2) * 30;
		actWork->work[1] = GFUser_GetPublicRand( 0x10000 );
		
		//GFL_BBDACT_SetAnimeIdx( bbdActSys, actIdx, anm );
		//GFL_BBD_SetObjectAlpha( bbdSys, actIdx, &alpha );
	} else {
		actWork->work[0]--;
	}
	{
		VecFx32 nowTrans, vecMove, rotVec;
		u16	 theta = actWork->work[1];
		fx32 speed = FX32_ONE;

		{
			u16	 dir = actWork->work[1] - actWork->fldActSys->cameraRotate;
			if( (dir > 0x2000)&&(dir < 0x6000)){
				//OS_Printf("�E�@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_RIGHT );
			} else if( (dir >= 0x6000)&&(dir <= 0xa000)){
				//OS_Printf("��@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_UP );
			} else if( (dir > 0xa000)&&(dir < 0xe000)){
				//OS_Printf("���@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_LEFT );
			} else {
				//OS_Printf("���@rotate = %x\n", dir );
				GFL_BBDACT_SetAnimeIdxContinue( bbdActSys, actIdx, ACTWALK_DOWN );
			}
		}
		GFL_BBD_GetObjectTrans( bbdSys, actIdx, &nowTrans );

		vecMove.x = FX_SinIdx( theta );
		vecMove.y = 0;
		vecMove.z = FX_CosIdx( theta );
		VEC_MultAdd( speed, &vecMove, &nowTrans, &nowTrans );
		if( CheckGroundOutRange( &nowTrans ) == TRUE ){
			GFL_BBD_SetObjectTrans( bbdSys, actIdx, &nowTrans );
		}
	}
}

static void testSetUp( FLD_ACTSYS* fldActSys )
{
	GFL_BBDACT_ACTDATA* actData;
	GFL_BBDACT_ACTUNIT_ID actUnitID;
	int		i, objIdx;
	VecFx32	trans;
	u8		alpha;
	BOOL	drawEnable;
	u16		setActNum = FLD_BBDACT_ACTMAX;

	//���\�[�X�Z�b�g�A�b�v
	fldActSys->bbdActResCount = NELEMS(testResTable);
	fldActSys->bbdActResUnitID = GFL_BBDACT_AddResourceUnit(	fldActSys->bbdActSys, 
																testResTable, 
																fldActSys->bbdActResCount );
	//�A�N�^�[�Z�b�g�A�b�v
	fldActSys->bbdActActCount = FLD_BBDACT_ACTMAX;
	actData = GFL_HEAP_AllocClearMemory( fldActSys->heapID, 
										fldActSys->bbdActActCount*sizeof(GFL_BBDACT_ACTDATA) );

	for( i=0; i<setActNum; i++ ){
		actData[i].resID = GFUser_GetPublicRand( 10 );
		actData[i].sizX = FX16_ONE*8-1;
		actData[i].sizY = FX16_ONE*8-1;
		actData[i].trans.x = (GFUser_GetPublicRand( 32 ) - ( 32/2 )) * FX32_ONE*16;
		actData[i].trans.y = FX32_ONE*7;
		actData[i].trans.z = (GFUser_GetPublicRand( 32 ) - ( 32/2 )) * FX32_ONE*16;
		actData[i].alpha = 31;//GFUser_GetPublicRand( 31 ) + 1;
		actData[i].drawEnable = TRUE;
		actData[i].func = testFunc;
		actData[i].work = &fldActSys->actWork[i];
	}
	fldActSys->bbdActActUnitID = GFL_BBDACT_AddAct( fldActSys->bbdActSys, 
													testResUnitID, 
													actData,
													fldActSys->bbdActActCount );
	for( i=0; i<setActNum; i++ ){
		GFL_BBDACT_SetAnimeTable( fldActSys->bbdActSys, fldActSys->bbdActResUnitID+i, 
									testAnmTable, NELEMS(testAnmTable) );
		GFL_BBDACT_SetAnimeIdxOn( fldActSys->bbdActSys, fldActSys->bbdActActUnitID+i, 0 );
	}

	GFL_HEAP_FreeMemory( actData );
}

static void testRelease( FLD_ACTSYS* fldActSys )
{
	u16	setActNum = FLD_BBDACT_ACTMAX;

	GFL_BBDACT_RemoveAct( fldActSys->bbdActSys, testActUnitID, setActNum );
	GFL_BBDACT_RemoveResourceUnit( fldActSys->bbdActSys, testResUnitID, NELEMS(testResTable) );
}

//BBD�pVRAM�]���֐�
static void	g3d_trans_BBD( GFL_BBDACT_TRANSTYPE type, u32 dst, u32 src, u32 siz )
{
	NNS_GFD_DST_TYPE transType;

	if( type == GFL_BBDACT_TRANSTYPE_DATA ){
		transType = NNS_GFD_DST_3D_TEX_VRAM;
	} else {
		transType = NNS_GFD_DST_3D_TEX_PLTT;
	}
	NNS_GfdRegisterNewVramTransferTask( transType, dst, (void*)src, siz );
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g�V�X�e���쐬
 */
//------------------------------------------------------------------
FLD_ACTSYS*	CreateFieldActSys( GFL_G3D_CAMERA* g3Dcamera, HEAPID heapID )
{
	FLD_ACTSYS* fldActSys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_ACTSYS) );
	int	i;

	fldActSys->heapID = heapID;
	fldActSys->g3Dcamera = g3Dcamera;
	fldActSys->g3Dlightset = GFL_G3D_LIGHT_Create( &light0Setup, heapID );
	fldActSys->bbdActSys = GFL_BBDACT_CreateSys( FLD_BBDACT_RESMAX, FLD_BBDACT_ACTMAX, g3d_trans_BBD, heapID );

	calcCameraRotate( fldActSys );
	for( i=0; i<FLD_BBDACT_ACTMAX; i++ ){ initActWork( fldActSys, &fldActSys->actWork[i] ); }

	//testSetUp( fldActSys );	//�e�X�g

	return fldActSys;
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g�V�X�e���j��
 */
//------------------------------------------------------------------
void	DeleteFieldActSys( FLD_ACTSYS* fldActSys )
{
	//testRelease( fldActSys );	//�e�X�g
	GFL_G3D_LIGHT_Delete( fldActSys->g3Dlightset );
	GFL_BBDACT_DeleteSys( fldActSys->bbdActSys );
	GFL_HEAP_FreeMemory( fldActSys ); 
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g����֐�
 */
//------------------------------------------------------------------
void	MainFieldActSys( FLD_ACTSYS* fldActSys )
{
	calcCameraRotate( fldActSys );
	GFL_BBDACT_Main( fldActSys->bbdActSys );
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g�`��֐�
 */
//------------------------------------------------------------------
void	DrawFieldActSys( FLD_ACTSYS* fldActSys )
{
	GFL_BBDACT_Draw( fldActSys->bbdActSys, fldActSys->g3Dcamera,  fldActSys->g3Dlightset);
}

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�A�N�g���[�N������
 */
//------------------------------------------------------------------
static void	initActWork( FLD_ACTSYS* fldActSys, FLD_ACT_WORK* actWork )
{
	int i;

	actWork->fldActSys = fldActSys;

	for( i=0; i<WORK_SIZ; i++ ){
		actWork->work[i] = 0;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�J������]�Z�o(�r���{�[�h���̂��̂ɂ͊֌W�Ȃ��B�A�j�������̕ύX������̂ɎQ��)
 */
//------------------------------------------------------------------
static void	calcCameraRotate( FLD_ACTSYS* fldActSys )
{
	VecFx32 vec, camPos, target;

	GFL_G3D_CAMERA_GetPos( fldActSys->g3Dcamera, &camPos );
	GFL_G3D_CAMERA_GetTarget( fldActSys->g3Dcamera, &target );

	VEC_Subtract( &target, &camPos, &vec );

	fldActSys->cameraRotate = FX_Atan2Idx( -vec.z, vec.x ) - 0x4000;
}

