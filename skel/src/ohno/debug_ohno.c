//============================================================================================
/**
 * @file	debug_ohno.c
 * @brief	�f�o�b�O�p�֐�
 * @author	ohno
 * @date	2006.11.29
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "debug_ohno.h"
#include "test_ui.h"
#include "test_rand.h"
#include "test_net.h"
#include "test_beacon.h"
#include "gfl_use.h"

#include "main.h"


//------------------------------------------------------------------
typedef struct {
	u32 debug_heap_id;
	GFL_PROCSYS * psys;
    SKEL_TEST_BEACON_WORK* pBeaconWork;
}DEBUG_OHNO_CONTROL;

static DEBUG_OHNO_CONTROL * DebugOhnoControl;


//------------------------------------------------------------------
// �X���[�v�������ɌĂ΂��֐�
//------------------------------------------------------------------

static void _sleepRelease(void *pWork)
{
    OS_TPrintf(" sleep release \n");
}


static GFL_PROC_RESULT _debugUIProcInit(GFL_PROC * proc, int * seq, void * p_work, void * my_work)
{
	DEBUG_OHNO_CONTROL * ctrl = p_work;

    GFL_UI_SleepReleaseSetFunc(_sleepRelease, NULL);
#if 1
    TEST_NET_Init();
#else
    //�r�[�R���T�m�@
    ctrl->pBeaconWork = TEST_BEACON_Init(ctrl->debug_heap_id);
#endif

    return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT _debugUIProcMain(GFL_PROC * proc, int * seq, void * p_work, void * my_work)
{
	GFL_PROC * subproc;
	DEBUG_OHNO_CONTROL * ctrl = p_work;

#if 1
//    TEST_UI_Main();  //UI TEST

    OS_TPrintf("%d \n",GFUser_GetPublicRand(32));
    OS_TPrintf("%d \n",GFUser_GetPublicRand(32));
    OS_TPrintf("%d \n",GFUser_GetPublicRand(32));
    OS_TPrintf("%d \n",GFUser_GetPublicRand(32));
    OS_TPrintf("%d \n",GFUser_GetPublicRand(32));
    
    
    if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_A){

        RTCDate date;
        RTCTime time;
        u32 seed;
        RTC_GetDateTime(&date, &time);
        seed = date.year + date.month * 0x100 * date.day * 0x10000
		+ time.hour * 0x10000 + (time.minute + time.second) * 0x1000000;

        TEST_RAND_Main(seed);  // RAND TEST
    }
    TEST_NET_Main();
#else
    // �r�[�R���T�m�@
    TEST_BEACON_Main(ctrl->pBeaconWork);
#endif

    
	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT _debugUIProcEnd(GFL_PROC * proc, int * seq, void * p_work, void * my_work)
{
	DEBUG_OHNO_CONTROL * ctrl = p_work;

    TEST_BEACON_End(ctrl->pBeaconWork);
    return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
// UI�e�X�g�p�v���Z�X��`�e�[�u��
//------------------------------------------------------------------
static const GFL_PROC_DATA UITestProcTbl = {
	_debugUIProcInit,
	_debugUIProcMain,
	_debugUIProcEnd,
};

static void VBlankIntr(void)
{
    //---- ���荞�݃`�F�b�N�t���O
    OS_SetIrqCheckFlag(OS_IE_V_BLANK);
}

static	const	char	*_graphicFileTable[]={
	"src/sample_graphic/radar.narc",
};

//------------------------------------------------------------------
//  �f�o�b�O�p���C���֐�
//------------------------------------------------------------------
void DebugOhnoInit(HEAPID heap_id)
{
	DEBUG_OHNO_CONTROL * ctrl;
	ctrl = GFL_HEAP_AllocMemory(heap_id, sizeof(DEBUG_OHNO_CONTROL));
	DebugOhnoControl = ctrl;
	ctrl->debug_heap_id = heap_id;

	GFL_PROC_SysCallProc(NO_OVERLAY_ID, &UITestProcTbl, ctrl);

	//ARC�V�X�e��������
	GFL_ARC_Init(&_graphicFileTable[0],NELEMS(_graphicFileTable));

    // �Z���A�N�^�[�V�X�e��������
	// �܂����̏������s���K�v������܂��B
	{
		static const GFL_CLSYS_INIT	param = {
			// ���C���ƃT�u�̃T�[�t�F�[�X������W��ݒ肵�܂��B
			// �T�[�t�F�[�X�̃T�C�Y�́i256,192�j�ɂ���̂����ʂȂ̂ŁA
			// �����o�ɂ͓���܂���ł����B
			// �㉺�̉�ʂ��Ȃ��Ďg�p����Ƃ��́A
			// �T�u�T�[�t�F�[�X�̍�����W��(0, 192)�Ȃǂɂ���K�v������Ǝv���܂��B
			0, 0,		// ���C���T�[�t�F�[�X�̍�����W�ix,y�j
			0, 256,		// �T�u�T�[�t�F�[�X�̍�����W�ix,y�j
			
			// ���̓t����OAMAttr���g�p����ꍇ�̐ݒ�
			// �ʐM�A�C�R���Ȃǂ�0�`3��Oam�̈���g���Ȃ��Ƃ��Ȃǂ́A
			// OAMAttr�Ǘ����ݒ��ύX����K�v������܂��B
			1, 128-1,		// ���C��OAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
			0, 128,		// �T�uOAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
		};
		GFL_CLACT_Init( &param, GFL_HEAPID_APP );
	}


    //	GFL_UI_TP_Init(ctrl->debug_heap_id);


//    RTC_Init();
    
}


//------------------------------------------------------------------
//------------------------------------------------------------------
void DebugOhnoMain(void)
{
	DEBUG_OHNO_CONTROL * ctrl = DebugOhnoControl;
//	GFL_UI_TP_Main();
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void DebugOhnoExit(void)
{
	DEBUG_OHNO_CONTROL * ctrl = DebugOhnoControl;
//	GFL_UI_TP_Exit();
	GFL_HEAP_FreeMemory(ctrl);
    DebugOhnoControl = NULL;
}


static GFL_PROC_RESULT DebugOhnoMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_OHNO_DEBUG,0x20000);

    DebugOhnoInit(HEAPID_OHNO_DEBUG);
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    DebugOhnoMain();
    return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    DebugOhnoExit();
	return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugOhnoMainProcData = {
	DebugOhnoMainProcInit,
	DebugOhnoMainProcMain,
	DebugOhnoMainProcEnd,
};

