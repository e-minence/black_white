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
#if 0
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

    


    if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_L){
        OSTick tick;
        u64 time;
        MATHCRC16Table localTable;
        int i;
        int testsize = 364+288*2;
        u8* pDummyBuff = GFL_HEAP_AllocMemory(ctrl->debug_heap_id, testsize);

        OS_InitTick();
        OS_SetTick((OSTick)0);
        MATH_CRC16CCITTInitTable( &localTable );
        
        for(i=0;i<100;i++){
            MATH_CalcCRC16CCITT( &localTable, pDummyBuff, testsize );
            pDummyBuff[i%testsize]++;
            MATH_CalcCRC16CCITT( &localTable, pDummyBuff, testsize );
        }
        tick = OS_GetTick();
        time = OS_TicksToMilliSeconds(tick);
        OS_TPrintf("���� %d\n",time);
        GFL_HEAP_FreeMemory(pDummyBuff);
    }


#if 0
//    TEST_UI_Main();  //UI TEST
    
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


//----------------------------------------------������̗��L�[�V�F�A�����O

#include "key_yt_common.h"
#include "key_title.h"
#include "key_game.h"

#include "testmode.h"

#define	TCB_WORK_SIZE		(0x10000)
#define	ACTIVE_TCB_MAX		(144)

typedef	void	(*ytFunc)(GAME_PARAM *);

static	const	char	*GraphicFileTable[]={
	"src/sample_graphic/yossyegg.narc",
	"src/sample_sound/yossy_egg.narc",
	"src/sample_graphic/spa.narc",
};

static	void	game_init(GAME_PARAM *gp);

extern	void	KEY_YT_Init3DTest(GAME_PARAM *gp);
extern	void	KEY_YT_Main3DTest(GAME_PARAM *gp);
extern	void	KEY_YT_InitBlockOut(GAME_PARAM *gp);
extern	void	KEY_YT_MainBlockOut(GAME_PARAM *gp);

//============================================================================================
//
//
//		Job�e�[�u��
//
//
//============================================================================================

static	ytFunc	YT_JobTable[]={
	KEY_YT_InitTitle,
	KEY_YT_MainTitle,
	KEY_YT_InitGame,
	KEY_YT_MainGame,
};

//============================================================================================
//
//
//		�v���Z�X�̒�`
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏�����
 *
 * �����Ńq�[�v�̐�����e�평�����������s���B
 * �����i�K�ł�mywk��NULL�����AGFL_PROC_AllocWork���g�p�����
 * �ȍ~�͊m�ۂ������[�N�̃A�h���X�ƂȂ�B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAME_PARAM	*gp;

	GFL_HEAP_CreateHeap(GFL_HEAPID_APP,HEAPID_OHNO_DEBUG,0x180000);
	gp=GFL_PROC_AllocWork(proc, sizeof(GAME_PARAM), HEAPID_OHNO_DEBUG);

	GFL_STD_MemClear(gp,sizeof(GAME_PARAM));

	gp->heapID=HEAPID_OHNO_DEBUG;

	KEY_YT_JobNoSet(gp,KEY_YT_InitTitleNo);

	game_init(gp);

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAME_PARAM	*gp=(GAME_PARAM *)mywk;

	YT_JobTable[gp->job_no](gp);

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̏I������
 *
 * �P�ɏI�������ꍇ�A�e�v���Z�X�ɏ������Ԃ�B
 * GFL_PROC_SysSetNextProc���Ă�ł����ƁA�I���セ�̃v���Z�X��
 * �������J�ڂ���B
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	return GFL_PROC_RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief		�Q�[�����Ƃ̏���������
 */
//------------------------------------------------------------------
static	void	game_init(GAME_PARAM *gp)
{
	//TCB������
	//�^�X�N���[�N�m��
	gp->tcb_work=GFL_HEAP_AllocMemory(GFL_HEAPID_APP,TCB_WORK_SIZE);
	gp->tcbsys=GFL_TCB_Init(ACTIVE_TCB_MAX,gp->tcb_work);

	//DMA�V�X�e��������
	GFL_DMA_Init(GFL_HEAPID_APP);

	//ARC�V�X�e��������
	GFL_ARC_Init(&GraphicFileTable[0],1);

	//FADE�V�X�e��������
	GFL_FADE_Init(GFL_HEAPID_APP);

	//�^�b�`�p�l��������
	GFL_UI_TP_Init(GFL_HEAPID_APP);

	//�Z���A�N�^�[������
	gp->clact = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(KEY_YT_CLACT) );

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

	//�T�E���h�f�[�^���[�h
//	GFL_SOUND_LoadArchiveData("src/sample_sound/yossy_egg.sdat");
	GFL_SOUND_LoadArchiveData(1,0);
	GFL_SOUND_LoadGroupData(GROUP_SE);
	GFL_SOUND_LoadGroupData(GROUP_JINGLE);
	gp->mus_level[MUS_LEVEL_JINGLE]=GFL_SOUND_SaveHeapState();
}

//------------------------------------------------------------------
/**
 * @brief		gp->job_no�ւ̑��
 */
//------------------------------------------------------------------
void	KEY_YT_JobNoSet(GAME_PARAM *gp,int job_no)
{
	gp->job_no=job_no;
	gp->seq_no=0;
}






#define COMMENT_OHNO 0
#if COMMENT_OHNO

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
#endif  //COMMENT_OHNO

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugOhnoMainProcData = {
	DebugOhnoMainProcInit,
	DebugOhnoMainProcMain,
	DebugOhnoMainProcEnd,
};

