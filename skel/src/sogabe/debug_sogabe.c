//============================================================================================
/**
 * @file	debug_sogabe.c
 * @brief	�f�o�b�O���e�X�g�p�A�v���i�]�䕔����p�j
 * @author	tamada
 * @date	2007.02.01
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"

#include "yt_common.h"
#include "title.h"
#include "game.h"

#include "testmode.h"

#define	TCB_WORK_SIZE		(0x10000)
#define	ACTIVE_TCB_MAX		(144)

typedef	void	(*ytFunc)(GAME_PARAM *);

static	const	char	*GraphicFileTable[]={
	"src/sample_graphic/yossyegg.narc",
};

static	void	game_init(GAME_PARAM *gp);

extern	void	YT_InitTitle(GAME_PARAM *);
extern	void	YT_MainTitle(GAME_PARAM *);

//============================================================================================
//
//
//		Job�e�[�u��
//
//
//============================================================================================

static	ytFunc	YT_JobTable[]={
	YT_InitTitle,
	YT_MainTitle,
//	YT_InitGame,
//	YT_MainGame,
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
static GFL_PROC_RESULT DebugSogabeMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	GAME_PARAM	*gp;

	gp=GFL_PROC_AllocWork(proc, sizeof(GAME_PARAM), GFL_HEAPID_APP);

	gp->job_no=YT_InitTitleNo;

	game_init(gp);

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�̃��C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugSogabeMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
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
static GFL_PROC_RESULT DebugSogabeMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugSogabeMainProcData = {
	DebugSogabeMainProcInit,
	DebugSogabeMainProcMain,
	DebugSogabeMainProcEnd,
};

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
	gp->tcbsys=GFL_TCB_SysInit(ACTIVE_TCB_MAX,gp->tcb_work);

	//ARC�V�X�e��������
	GFL_ARC_sysInit(&GraphicFileTable[0],1);

	//FADE�V�X�e��������
	GFL_FADE_sysInit(GFL_HEAPID_APP);

	//�^�b�`�p�l��������
	GFL_UI_sysInit(GFL_HEAPID_APP);
	GFL_UI_TP_sysInit(GFL_HEAPID_APP);

	//�Z���A�N�^�[������
//	gp->clact_wk = GFL_HEAP_AllocMemory( GFL_HEAPID_APP, sizeof(DEBUG_CLACT) );

	// �Z���A�N�^�[�V�X�e��������
	// �܂����̏������s���K�v������܂��B
	{
		static const CLSYS_INIT	param = {
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
			0, 128,		// ���C��OAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
			0, 128,		// �T�uOAM�}�l�[�W����OamAttr�Ǘ���(�J�nNo,�Ǘ���)
		};
		GFL_CLACT_SysInit( &param, GFL_HEAPID_APP );
	}
}

