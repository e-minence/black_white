//==============================================================================
/**
 * @file	palace_block.c
 * @brief	�p���X�F�u���b�N����
 * @author	matsuda
 * @date	2009.01.14(��)
 */
//==============================================================================
#include <gflib.h>
#include <procsys.h>
#include "system\main.h"
#include "arc_def.h"
#include "palace.naix"
#include "test/performance.h"


//==============================================================================
//	�萔��`
//==============================================================================
///�A�N�^�[�ő吔
#define ACT_MAX			(64)

#define ONE_GRID		(FX32_ONE * 8)

//--------------------------------------------------------------
//	3D
//--------------------------------------------------------------
#define DTCM_SIZE		(0x1000)

static const GFL_G3D_OBJSTATUS status0 = {
	{ 0, 0, 0 },								//���W
	{ FX32_ONE, FX32_ONE, FX32_ONE },		//�X�P�[��
	{ FX32_ONE, 0, 0, 0, FX32_ONE, 0, 0, 0, FX32_ONE },	//��]
};


//==============================================================================
//	�\���̒�`
//==============================================================================
typedef struct{
	VecFx32				trans;
	fx32				cameraHeight;
	u16					cameraLength;
	u16					direction;
	VecFx32				transOffset;
}PB_CAMERA;

typedef struct {
	u16		seq;
	int debug_mode;
	int mode;			///<���̃��j���[��ʂֈ����n�����[�h
	
	GFL_G3D_UTIL*			g3Dutil;
	u16						g3DutilUnitIdx;
	GFL_G3D_OBJSTATUS		gira_status;
	
	GFL_G3D_CAMERA *g3d_camera;
	PB_CAMERA camera;
	
	int block_max;
}TITLE_WORK;


//==============================================================================
//	�v���g�^�C�v�錾
//==============================================================================
GFL_PROC_RESULT PalaceBlockProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT PalaceBlockProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
GFL_PROC_RESULT PalaceBlockProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static void Local_VramSetting(TITLE_WORK *tw);
static void Local_BGFrameSetting(TITLE_WORK *tw);
static void Local_3DSetting(TITLE_WORK *tw);
static void Local_GirathinaLoad(TITLE_WORK *tw);
static void Local_Draw3D(TITLE_WORK *tw);
static void Local_GirathinaFree(TITLE_WORK *tw);
static void _PB_CameraMove(TITLE_WORK *tw);
static VecFx32 _BlockPosGet(int block_no);


//==============================================================================
//	�f�[�^
//==============================================================================
///�^�C�g����ʌĂяo���悤��PROC�f�[�^
const GFL_PROC_DATA PalaceBlockProcData = {
	PalaceBlockProcInit,
	PalaceBlockProcMain,
	PalaceBlockProcEnd,
};

//--------------------------------------------------------------
//	3D
//--------------------------------------------------------------
enum{
	G3DRES_GIRA_BMD = 0,
};

//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_PALACE, NARC_palace_cube_nsbmd, GFL_G3D_UTIL_RESARC },
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
	{
		G3DRES_GIRA_BMD, 			//���f�����\�[�XID
		0, 							//���f���f�[�^ID(���\�[�X����INDEX)
		G3DRES_GIRA_BMD, 			//�e�N�X�`�����\�[�XID
		NULL, 			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP g3Dutil_setup = {
	g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

#define G3DUTIL_RESCOUNT	(NELEMS(g3Dutil_resTbl))
#define G3DUTIL_OBJCOUNT	(NELEMS(g3Dutil_objTbl))


//==============================================================================
//
//	
//
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   
 *
 * @param   proc		
 * @param   seq		
 * @param   pwk		
 * @param   mywk		
 *
 * @retval  
 */
//--------------------------------------------------------------
GFL_PROC_RESULT PalaceBlockProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK *tw;
	
	DEBUG_PerformanceSetActive(TRUE);	//�f�o�b�O�F�p�t�H�[�}���X���[�^�[�L��

	// �e����ʃ��W�X�^������
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);
	GFL_DISP_GX_SetVisibleControlDirect(0);		//�SBG&OBJ�̕\��OFF
	GFL_DISP_GXS_SetVisibleControlDirect(0);
	G2_BlendNone();
	G2S_BlendNone();
	GX_SetVisibleWnd(GX_WNDMASK_NONE);
	GXS_SetVisibleWnd(GX_WNDMASK_NONE);

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PALACE, 0x70000 );
	tw = GFL_PROC_AllocWork( proc, sizeof(TITLE_WORK), HEAPID_PALACE );
	GFL_STD_MemClear(tw, sizeof(TITLE_WORK));
	tw->block_max = 1;

	// �㉺��ʐݒ�
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	GFL_DISP_SetDispOn();

	//VRAM�ݒ� & BG�t���[���ݒ�
	Local_VramSetting(tw);
	Local_BGFrameSetting(tw);
	
	//3D�ݒ�
	Local_3DSetting(tw);
	Local_GirathinaLoad(tw);

	//�p�t�H�[�}���X���[�^�[�̈�OBJ�\��ON	GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ, VISIBLE_ON);
	
	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT PalaceBlockProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK* tw = mywk;
	enum{
		SEQ_MAIN,
	};
	
	if(tw->seq == SEQ_MAIN){
		int trg = GFL_UI_KEY_GetTrg();
		if(trg & PAD_BUTTON_START){
			tw->mode = trg;
			return GFL_PROC_RES_FINISH;
		}
	}
	
	switch(tw->seq){
	case SEQ_MAIN:
		break;
	}
	
	if(GFL_UI_KEY_GetCont() & PAD_BUTTON_X){
		tw->block_max++;
		OS_TPrintf("�u���b�N�̐� = %d\n", tw->block_max);
	}
	else if(GFL_UI_KEY_GetCont() & PAD_BUTTON_Y){
		tw->block_max--;
		OS_TPrintf("�u���b�N�̐� = %d\n", tw->block_max);
	}
	
	_PB_CameraMove(tw);
	Local_Draw3D(tw);
	
	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------
/**
 * PROC Quit
 */
//--------------------------------------------------------------------------
GFL_PROC_RESULT PalaceBlockProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	TITLE_WORK* tw = mywk;
	int i, mode;
	
	mode = tw->mode;
	
	Local_GirathinaFree(tw);
	GFL_G3D_Exit();

	GFL_BG_Exit();

	GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_PALACE);
	
	return GFL_PROC_RES_FINISH;
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   VRAM�o���N�����[�h�ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_VramSetting(TITLE_WORK *tw)
{
	static const GFL_DISP_VRAM vramBank = {
		GX_VRAM_BG_128_B,				// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,			// ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,			// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,				// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,			// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_0_A,				// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_01_FG,			// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_64K,		// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_32K,		// �T�uOBJ�}�b�s���O���[�h
	};

	static const GFL_BG_SYS_HEADER sysHeader = {
		GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
	};
	
	GFL_DISP_SetBank( &vramBank );
	GFL_BG_Init( HEAPID_PALACE );
	GFL_BG_SetBGMode( &sysHeader );
}

//--------------------------------------------------------------
/**
 * @brief   BG�t���[���ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_BGFrameSetting(TITLE_WORK *tw)
{
#if 0
	static const GFL_BG_BGCNT_HEADER bgcnt_frame[] = {	//���C�����BG�t���[��
		{//FRAME_LOGO_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
		{//FRAME_MIST_M
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, 0x4000,
			GX_BG_EXTPLTT_01, BGPRI_MIST, 0, 0, FALSE
		},
	};

	static const GFL_BG_BGCNT_HEADER sub_bgcnt_frame[] = {	//�T�u���BG�t���[��
		{//FRAME_MSG_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, 0x04000,
			GX_BG_EXTPLTT_01, BGPRI_MSG, 0, 0, FALSE
		},
		{//FRAME_LOGO_S
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, 0x8000,
			GX_BG_EXTPLTT_01, BGPRI_TITLE_LOGO, 0, 0, FALSE
		},
	};

	GFL_BG_SetBGControl( FRAME_LOGO_M,   &bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_MIST_M,   &bgcnt_frame[1],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_MSG_S,   &sub_bgcnt_frame[0],   GFL_BG_MODE_TEXT );
	GFL_BG_SetBGControl( FRAME_LOGO_S,   &sub_bgcnt_frame[1],   GFL_BG_MODE_TEXT );

	GFL_BG_FillCharacter( FRAME_LOGO_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_MIST_M, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_MSG_S, 0x00, 1, 0 );
	GFL_BG_FillCharacter( FRAME_LOGO_S, 0x00, 1, 0 );

	GFL_BG_FillScreen( FRAME_LOGO_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_MIST_M, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_MSG_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
	GFL_BG_FillScreen( FRAME_LOGO_S, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );

	GFL_BG_LoadScreenReq( FRAME_LOGO_M );
	GFL_BG_LoadScreenReq( FRAME_MIST_M );
	GFL_BG_LoadScreenReq( FRAME_MSG_S );
	GFL_BG_LoadScreenReq( FRAME_LOGO_S );
#endif

	GFL_STD_MemFill16((void*)HW_BG_PLTT, 0x7fff, 0x20);
	GFL_STD_MemFill16((void*)HW_DB_BG_PLTT, 0x7fff, 0x20);
}

//--------------------------------------------------------------
/**
 * @brief   3D�����ݒ�
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_3DSetting(TITLE_WORK *tw)
{
	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT32K,
						DTCM_SIZE, HEAPID_PALACE, NULL );
	GFL_BG_SetBGControl3D(0);
}

//--------------------------------------------------------------
/**
 * @brief   �M���e�B�i�����[�h
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_GirathinaLoad(TITLE_WORK *tw)
{
	u16 objIdx;
	
	tw->g3Dutil = GFL_G3D_UTIL_Create(G3DUTIL_RESCOUNT, G3DUTIL_OBJCOUNT, HEAPID_PALACE);
	tw->g3DutilUnitIdx = GFL_G3D_UTIL_AddUnit( tw->g3Dutil, &g3Dutil_setup );

	//�A�j���[�V������L���ɂ���
#if 0
	objIdx = GFL_G3D_UTIL_GetUnitObjIdx( tw->g3Dutil, tw->g3DutilUnitIdx );
	GFL_G3D_OBJECT_EnableAnime( GFL_G3D_UTIL_GetObjHandle(tw->g3Dutil, objIdx), 0); 
	GFL_G3D_OBJECT_EnableAnime( GFL_G3D_UTIL_GetObjHandle(tw->g3Dutil, objIdx), 1); 
	OS_TPrintf("3d objIdx = %d\n", objIdx);
#endif

	tw->gira_status = status0;

	//�J�����Z�b�g
	{
		const VecFx32 cam_pos = {0, FX32_ONE * 50, FX32_ONE * 100};
		const VecFx32 target = {0,0,0};
		
		tw->g3d_camera = GFL_G3D_CAMERA_CreateDefault(&cam_pos, &target, HEAPID_PALACE);
		tw->camera.cameraLength = 16;
	}
}

//--------------------------------------------------------------
/**
 * @brief   3D�`��
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_Draw3D(TITLE_WORK *tw)
{
	GFL_G3D_OBJ* g3Dobj;
	u16				objIdx;
	int i;
	
	GFL_G3D_CAMERA_Switching(tw->g3d_camera);
	
	objIdx = GFL_G3D_UTIL_GetUnitObjIdx( tw->g3Dutil, tw->g3DutilUnitIdx );
	g3Dobj = GFL_G3D_UTIL_GetObjHandle( tw->g3Dutil, objIdx + 0 );

	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	{
		GFL_G3D_OBJSTATUS status = status0;
		
		for(i = 0; i < tw->block_max; i++){
			status.trans = _BlockPosGet(i);
			GFL_G3D_DRAW_DrawObject( g3Dobj, &status );
		}
	}
	GFL_G3D_DRAW_End();

#if 0
	GFL_G3D_OBJECT_LoopAnimeFrame( g3Dobj, 0, FX32_ONE/2 );	//BTA	/2=�v���`�i��1/30�A�j���̈�
	GFL_G3D_OBJECT_LoopAnimeFrame( g3Dobj, 1, FX32_ONE/2 );	//BCA
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �M���e�B�i���A�����[�h
 * @param   tw		�^�C�g�����[�N�ւ̃|�C���^
 */
//--------------------------------------------------------------
static void Local_GirathinaFree(TITLE_WORK *tw)
{
	GFL_G3D_UTIL_DelUnit( tw->g3Dutil, tw->g3DutilUnitIdx );
	GFL_G3D_UTIL_Delete( tw->g3Dutil );
	
	//�J�����폜
	GFL_G3D_CAMERA_Delete(tw->g3d_camera);
}

//--------------------------------------------------------------
/**
 * @brief   �J��������
 *
 * @param   tw		
 */
//--------------------------------------------------------------
#define ROTATE_SPEED		(FX32_ONE/8)
#define MOVE_SPEED		(FX32_ONE*2)
static void _PB_CameraMove(TITLE_WORK *tw)
{
	GFL_G3D_CAMERA * g3Dcamera = tw->g3d_camera;
	PB_CAMERA *camera = &tw->camera;
	VecFx32	pos, target, trans;
	VecFx32	vecMove = { 0, 0, 0 };
	VecFx32	vecUD = { 0, 0, 0 };
	BOOL	mvFlag = FALSE;
	int key = GFL_UI_KEY_GetCont();
	
	if( key & PAD_BUTTON_R ){
		camera->direction -= ROTATE_SPEED;
	}
	if( key & PAD_BUTTON_L ){
		camera->direction += ROTATE_SPEED;
	}
	if( key & PAD_BUTTON_B ){
		if( camera->cameraLength > 8 ){
			camera->cameraLength -= 8;
		}
		//vecMove.y = -MOVE_SPEED;
	}
	if( key & PAD_BUTTON_A ){
		if( camera->cameraLength < 4096 ){
			camera->cameraLength += 8;
		}
		//vecMove.y = MOVE_SPEED;
	}
	if( key & PAD_BUTTON_Y ){
		camera->cameraHeight -= MOVE_SPEED;
	}
	if( key & PAD_BUTTON_X ){
		camera->cameraHeight += MOVE_SPEED;
	}
	
	trans = camera->trans;
	trans.x += camera->transOffset.x;
	trans.y += camera->transOffset.y;
	trans.z += camera->transOffset.z;

	VEC_Set( &target,
			trans.x,
			trans.y,// + CAMERA_TARGET_HEIGHT*FX32_ONE,
			trans.z);

	pos.x = trans.x + camera->cameraLength * FX_SinIdx(camera->direction);
	pos.y = trans.y + camera->cameraHeight;
	pos.z = trans.z + camera->cameraLength * FX_CosIdx(camera->direction);

//	GFL_G3D_CAMERA_SetTarget( g3Dcamera, &target );
//	GFL_G3D_CAMERA_SetPos( g3Dcamera, &pos );
}

//--------------------------------------------------------------
/**
 * @brief   �u���b�N�̔z�u���W���擾����
 *
 * @param   block_no		�u���b�N�ԍ�
 *
 * @retval  ���W
 */
//--------------------------------------------------------------
static VecFx32 _BlockPosGet(int block_no)
{
	VecFx32 pos;
	
	pos.x = (block_no % 16) * ONE_GRID;
	pos.y = (block_no / 256) * ONE_GRID;
	pos.z = ((block_no / 16) % 16) * ONE_GRID;
	return pos;
}
