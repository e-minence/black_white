//=============================================================================================
/**
 * @file	gfl_use.c
 * @date	2006.12.06
 * @author	GAME FREAK inc.
 * @brief	���[�U�[��GF���C�u�����g�p�̂��߂̃X�P���g���\�[�X
 *
 */
//=============================================================================================

#include <nitro.h>

#include <gflib.h>
#include <procsys.h>
#include <gf_overlay.h>
#include <tcb.h>
#include <backup_system.h>

//#include "gflib_inter.h"

#include "system\main.h"
#include "system\gfl_use.h"
#include "arc_def.h"
#include "arc_file.h"

#include "textprint.h"

#ifdef PM_DEBUG
#include "test/debug_pause.h"
#include "debug/debugwin_sys.h"
#endif //PM_DEBUG


//=============================================================================================
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
	TCB_HINTR_MAX = 4,
	TCB_VINTR_MAX = 16,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
    GFL_STD_RandContext publicRandContext;
	GFL_TCBSYS *	TCBSysHintr;
	void *		TCBMemHintr;
	GFL_TCBSYS *	TCBSysVintr;
	void *		TCBMemVintr;
}GFL_USE_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static const HEAP_INIT_HEADER hih[]={
	{ HEAPSIZE_SYSTEM,	OS_ARENA_MAIN },
	{ HEAPSIZE_APP,		OS_ARENA_MAIN },
};

static GFL_USE_WORK *	gfl_work = NULL;
static int				GFL_USE_VintrCounter;

static void GFUser_PublicRandInit(void);
static void gfluse_AssertFinish( void );

//=============================================================================================
//
//			�֐�
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����F������
 *
 * InitSystem�̓����ŌĂ�ł���InitFileSystem��gflib�̊O�ł�
 * �ĂׂȂ����߁AInitSystem���Ă�ł���B
 */
//------------------------------------------------------------------
void GFLUser_Init(void)
{
	//InitSystem();
#if 0
#ifdef USE_MAINEXMEMORY
  OS_EnableMainExArena();
#endif

	// �V�X�e��������
	OS_Init();
	OS_InitTick();
	OS_InitAlarm();
	// RTC������
	RTC_Init();
	// GX������
	GX_Init();
	// �Œ菬���_API������
	FX_Init();
	// �t�@�C���V�X�e��������
	InitFileSystem();
#endif

	//�q�[�v�V�X�e��������
	GFL_HEAP_Init(&hih[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0);	//���C���A���[�i
	GFL_HEAP_DTCM_Init( 0x2000 );				//�c�s�b�l�A���[�i

    //STD �W�����C�u�����������i������CRC�j
    GFL_STD_Init(GFL_HEAPID_SYSTEM);
	//�A�[�J�C�u�V�X�e��������
	GFL_ARC_Init(&ArchiveFileTable[0],ARCID_TABLE_MAX);

	//UI�V�X�e��������
	GFL_UI_Boot(GFL_HEAPID_SYSTEM);

	// �A�T�[�g��~�֐��̐ݒ�i�R�[���X�^�b�N�\�����\�Ȃ珈�����s�̂��߁j
	GFL_ASSERT_SetDisplayFunc( NULL, NULL, gfluse_AssertFinish );

	//OVERLAY�V�X�e��������
	GFL_OVERLAY_boot(GFL_HEAPID_SYSTEM, 8, 4, 4);

	//PROC�V�X�e��������
	GFL_PROC_boot(GFL_HEAPID_SYSTEM);
	gfl_work = GFL_HEAP_AllocMemory(GFL_HEAPID_SYSTEM, sizeof(GFL_USE_WORK));
	gfl_work->TCBMemHintr = GFL_HEAP_AllocMemory(
		  GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_HINTR_MAX));
	gfl_work->TCBSysHintr = GFL_TCB_Init(TCB_HINTR_MAX, gfl_work->TCBMemHintr);
	gfl_work->TCBMemVintr = GFL_HEAP_AllocMemory(
		  GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_VINTR_MAX));
	gfl_work->TCBSysVintr = GFL_TCB_Init(TCB_VINTR_MAX, gfl_work->TCBMemVintr);

   // �ėp����������
    GFUser_PublicRandInit();

    //FADE�V�X�e��������
    GFL_FADE_Init(GFL_HEAPID_SYSTEM);

    //�T�E���h�V�X�e��������
    //GFL_SOUND_Init(&sndHeap[0],SOUND_HEAP_SIZE);

	//�o�b�N�A�b�v�V�X�e��������
	GFL_BACKUP_Init(GFL_HEAPID_APP, GFL_HEAPID_APP);

	//�V�X�e���t�H���g������
	GFL_TEXT_CreateSystem( NULL );

	//RTC������
	GFL_RTC_Init();
	
	GFL_USE_VintrCounter = 0;
}


//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����F���C���Ăяo��
 *
 * ���t���[���Ăяo�����K�v�����郁�C���������L�q����
 */
//------------------------------------------------------------------
void GFLUser_Main(void)
{
	GFL_UI_Main();
    GFL_NET_Main();  //�L�[�̏����̌シ���ɒʐM�������������ׂ����ɔz�u
	
#ifdef PM_DEBUG
  //�f�o�b�O�V�X�e��
  DEBUGWIN_UpdateSystem();
	//L�������Ă���Ԓ�~�BL�����Ȃ���R��1�R�}����
	if( DEBUG_PAUSE_Update() == TRUE &&
	    DEBUGWIN_IsActive() == FALSE )
#endif
	{
		GFL_PROC_Main();
		GFL_FADE_Main();
	}
	GFL_RTC_Main();
	//GFL_SOUND_Main();
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����F�`�揈��
 *
 * ���t���[���Ăяo�����K�v������`�揈�����L�q����
 */
//------------------------------------------------------------------
void GFLUser_Display(void)
{
	//GFL_OAM_sysMain(...);
	//GFL_BG_sysMain(...);
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����F�I������
 */
//------------------------------------------------------------------
void GFLUser_Exit(void)
{
	GFL_TEXT_DeleteSystem();
	GFL_BACKUP_Exit();
	//GFL_SOUND_Exit();
	GFL_FADE_Exit();
	GFL_PROC_Exit();
	GFL_TCB_Exit(gfl_work->TCBSysVintr);
	GFL_HEAP_FreeMemory(gfl_work->TCBSysVintr);
	GFL_HEAP_FreeMemory(gfl_work->TCBMemVintr);
	GFL_OVERLAY_Exit();
	GFL_UI_Exit();
	GFL_HEAP_DTCM_Exit();
	GFL_HEAP_Exit();
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FHBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void GFLUser_HIntr(void)
{
	GFL_TCB_Main(gfl_work->TCBSysHintr);
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FHBlank��TCB�̓o�^����
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_HIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
	return GFL_TCB_AddTask(gfl_work->TCBSysHintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FVBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void GFLUser_VIntr(void)
{
	GFL_TCB_Main(gfl_work->TCBSysVintr);
	GFL_BG_VBlankFunc();
	// V�u�����N���ԂŎ��s���܂��B
	// �����A���j�b�g�̕`�悪�s���Ă��Ȃ��̂�
	// ���̊֐������s����ƁA�`�悵�Ă���OBJ�������Ă��܂�����
	// ���荞�݂Ȃ��ŌĂ΂Ȃ��ق����ǂ���������܂���B
		//�J�����܂��� Ariizumi1031
	//GFL_CLACT_VBlankFunc();
    // �ʐM�A�C�R���̕`��̂��߂ɂ���܂��B�ʐM���͍̂s���Ă��܂���
    //GFL_NET_VBlankFunc();

	GFL_DMA_Main();
	GFL_USE_VintrCounter++;
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FVBlank��TCB�̓o�^����
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
	return GFL_TCB_AddTask(gfl_work->TCBSysVintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FVBlank��TCBSYS�̎擾
 */
//------------------------------------------------------------------
GFL_TCBSYS * GFUser_VIntr_GetTCBSYS( void )
{
	return gfl_work->TCBSysVintr;
}

//------------------------------------------------------------------
/**
 * @brief	GFUser �ėp�����쐬
 */
//------------------------------------------------------------------
static void GFUser_PublicRandInit(void)
{
    GFL_STD_RandGeneralInit( &gfl_work->publicRandContext );
}

//------------------------------------------------------------------
/**
 * @brief	GFUser �ėp�����𓾂�
 */
//------------------------------------------------------------------
u32 GFUser_GetPublicRand(u32 range)
{
    return GFL_STD_Rand( &gfl_work->publicRandContext, range );
}

//------------------------------------------------------------------
/**
 * @brief	VBlank�J�E���^�[�擾�ƃ��Z�b�g
 */
//------------------------------------------------------------------
void GFUser_VIntr_ResetVblankCounter( void )
{
	GFL_USE_VintrCounter = 0;
}

int GFUser_VIntr_GetVblankCounter( void )
{
	return GFL_USE_VintrCounter;
}

//--------------------------------------------------------------------------
/**
 * @brief	ArchiveID�Ńt�@�C�������擾����
 */
//--------------------------------------------------------------------------
const char * GFUser_GetFileNameByArcID(ARCID arcID)
{
	GF_ASSERT(arcID < ARCID_TABLE_MAX);
	return ArchiveFileTable[arcID];
}

//--------------------------------------------------------------------------
/**
 * ASSERT��~��ɌĂяo����郆�[�U�֐�
 *
 * �� �f�t�H���g����OS_Halt�ɂ���~���Ă��܂��A
 *    �R�[���X�^�b�N�\��������Ȃ��Ȃǂ̖�肪���邽�ߗp�ӂ��Ă���܂�
 *
 */
//--------------------------------------------------------------------------
static void gfluse_AssertFinish( void )
{
	int key_cont;

	while(1){

		GFL_UI_Main();
		key_cont = GFL_UI_KEY_GetCont();

		// �L�[���������܂Ń��[�v
		if( (key_cont & PAD_BUTTON_L) && (key_cont & PAD_BUTTON_R) &&
			(key_cont & PAD_BUTTON_X) && (key_cont & PAD_BUTTON_Y) ){
			return ;
		}

		// VBlank�҂�
		// (�����L���ɂ���ƁA�f�o�b�J�Œ�~�������Ƃ��ɃR�[���X�^�b�N���\������Ȃ�)
//		OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	}
}

