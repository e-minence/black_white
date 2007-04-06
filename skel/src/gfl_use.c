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
#include "gflib.h"
//#include "gflib_inter.h"

#include "main.h"

#include "gfl_use.h"
#include "procsys.h"
#include "gf_overlay.h"
#include "tcb.h"


//=============================================================================================
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
	TCB_VINTR_MAX = 16,
};

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
	TCBSYS * TCBSysVintr;
	void * TCBMemVintr;
}GFL_USE_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static const HEAP_INIT_HEADER hih[]={
	{ HEAPSIZE_SYSTEM,	OS_ARENA_MAIN },
	{ HEAPSIZE_APP,		OS_ARENA_MAIN },
};

static GFL_USE_WORK * gfl_work = NULL;

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
	InitSystem();
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

	//�A�[�J�C�u�V�X�e��������
	//GFL_ARC_SysInit(...);

	//UI�V�X�e��������
	GFL_UI_Boot(GFL_HEAPID_SYSTEM);

	//OVERLAY�V�X�e��������
	GFL_OVERLAY_boot(GFL_HEAPID_SYSTEM, 8, 4, 4);

	//PROC�V�X�e��������
	GFL_PROC_boot(GFL_HEAPID_SYSTEM);
  gfl_work = GFL_HEAP_AllocMemory(GFL_HEAPID_SYSTEM, sizeof(GFL_USE_WORK));
  gfl_work->TCBMemVintr = GFL_HEAP_AllocMemory(
		  GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_VINTR_MAX));
  gfl_work->TCBSysVintr = GFL_TCB_SysInit(TCB_VINTR_MAX, gfl_work->TCBMemVintr);

    //FADE�V�X�e��������
    GFL_FADE_sysInit(GFL_HEAPID_SYSTEM);

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
	
	GFL_PROC_SysMain();
    GFL_FADE_sysMain();
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
	GFL_UI_Exit();
	GFL_PROC_SysExit();
	GFL_OVERLAY_SysExit();
	GFL_FADE_sysExit();
	GFL_HEAP_DTCM_Exit();
	GFL_HEAP_Exit();
}


//------------------------------------------------------------------
/**
 * @brief	GF���C�u�������p�����FVBlank���荞�ݏ���
 */
//------------------------------------------------------------------
void GFLUser_VIntr(void)
{
	GFL_TCB_SysMain(gfl_work->TCBSysVintr);
	GFL_BG_VBlankFunc();
	// V�u�����N���ԂŎ��s���܂��B
	// �����A���j�b�g�̕`�悪�s���Ă��Ȃ��̂�
	// ���̊֐������s����ƁA�`�悵�Ă���OBJ�������Ă��܂�����
	// ���荞�݂Ȃ��ŌĂ΂Ȃ��ق����ǂ���������܂���B
	GFL_CLACT_SysVblank();
    // �ʐM�A�C�R���̕`��̂��߂ɂ���܂��B�ʐM���͍̂s���Ă��܂���
    GFL_NET_VBlankFunc();
}


