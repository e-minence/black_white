//============================================================================================
/**
 * @file		zukan.c
 * @brief		�}�Ӊ��
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	���W���[�����FZUKAN
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "app/zukan.h"

#include "list/zukanlist.h"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���V�[�P���X
enum {
	SEQ_LIST_CALL = 0,		// ���X�g�Ăяo��
	SEQ_LIST_END,					// ���X�g�I����
	SEQ_INFO_CALL,				// ���Ăяo��
	SEQ_INFO_END,					// ���I����
	SEQ_MAP_CALL,					// ���z�Ăяo��
	SEQ_MAP_END,					// ���z�I����
	SEQ_VOICE_CALL,				// �����Ăяo��
	SEQ_VOICE_END,				// �����I����
	SEQ_FORM_CALL,				// �p�Ăяo��
	SEQ_FORM_END,					// �p�I����
	SEQ_SEARCH_CALL,			// �����Ăяo��
	SEQ_SEARCH_END,				// �����I����
	SEQ_PROC_FINISH,			// �}�ӏI��
};

typedef struct {
	int	seq;
	void * work;
}ZUKAN_MAIN_WORK;

typedef int (*pZUKAN_FUNC)(ZUKAN_MAIN_WORK*);


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
static GFL_PROC_RESULT ZukanProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndList( ZUKAN_MAIN_WORK * wk );


//============================================================================================
//	�O���[�o��
//============================================================================================

// PROC�f�[�^
const GFL_PROC_DATA ZUKAN_ProcData = {
	ZukanProc_Init,
	ZukanProc_Main,
	ZukanProc_End
};

// ���C���V�[�P���X�֐��e�[�u��
static const pZUKAN_FUNC MainSeq[] = {
	MainSeq_CallList,			// ���X�g�Ăяo��
	MainSeq_EndList,			// ���X�g�I����

	NULL,	// ���Ăяo��
	NULL,	// ���I����
	NULL,	// ���z�Ăяo��
	NULL,	// ���z�I����
	NULL,	// �����Ăяo��
	NULL,	// �����I����
	NULL,	// �p�Ăяo��
	NULL,	// �p�I����
	NULL,	// �����Ăяo��
	NULL,	// �����I����
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F������
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZUKAN_PARAM * prm;
	ZUKAN_MAIN_WORK * wk;

	OS_Printf( "�����������@�}�ӏ����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_SYS, 0x1000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_MAIN_WORK), HEAPID_ZUKAN_SYS );
	GFL_STD_MemClear( wk, sizeof(ZUKAN_MAIN_WORK) );

	prm = pwk;

	if( prm->callMode == ZUKAN_MODE_LIST ){
		wk->seq = SEQ_LIST_CALL;
	}else if( prm->callMode == ZUKAN_MODE_INFO ){
		wk->seq = SEQ_INFO_CALL;
	}else if( prm->callMode == ZUKAN_MODE_MAP ){
		wk->seq = SEQ_MAP_CALL;
	}else if( prm->callMode == ZUKAN_MODE_VOICE ){
		wk->seq = SEQ_VOICE_CALL;
	}else if( prm->callMode == ZUKAN_MODE_FORM ){
		wk->seq = SEQ_FORM_CALL;
	}else{
		wk->seq = SEQ_SEARCH_CALL;
	}

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F���C��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZUKAN_MAIN_WORK * wk = mywk;

	if( wk->seq == SEQ_PROC_FINISH ){
		return GFL_PROC_RES_FINISH;
	}

	wk->seq = MainSeq[wk->seq]( wk );

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�v���Z�X�֐��F�I��
 *
 * @param		proc	�v���Z�X�f�[�^
 * @param		seq		�V�[�P���X
 * @param		pwk		�e���[�N
 * @param		mywk	���샏�[�N
 *
 * @return	������
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_SYS );

	OS_Printf( "�����������@�}�ӏ����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}



static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk )
{
	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANLIST_DATA) );
	GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan_list), &ZUKANLIST_ProcData, wk->work );
	return SEQ_LIST_END;
}

static int MainSeq_EndList( ZUKAN_MAIN_WORK * wk )
{
	ZUKANLIST_DATA * list;
	int	ret;

	list = wk->work;

	switch( list->retMode ){
	case ZKNLIST_RET_EXIT:		// �}�ӏI��
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNLIST_RET_EXIT_X:	// �}�ӂ��I�����ă��j���[��
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNLIST_RET_INFO:		// �ڍ׉�ʂ�
//		ret = SEQ_INFO_CALL;
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNLIST_RET_SEARCH:	// ������ʂ�
//		ret = SEQ_SEARCH_CALL;
		ret = SEQ_PROC_FINISH;
		break;

	default:
		ret = SEQ_PROC_FINISH;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}
