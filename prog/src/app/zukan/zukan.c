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

#include "zukan_common.h"
#include "list/zukanlist.h"
#include "search/zukansearch.h"
#include "detail/zukan_detail.h"


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
	ZUKAN_PARAM * prm;

	u16 * list;			// ���X�g�f�[�^
	u16	listMax;		// ���X�g�f�[�^��
	u32	listMode;		// ���X�g���[�h

	ZKNCOMM_LIST_SORT	sort;		// �\�[�g�f�[�^

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

static void FreeListData( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndList( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallSearch( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndSearch( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallDetail( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndDetail( ZUKAN_MAIN_WORK * wk );


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

	MainSeq_CallDetail,	// ���Ăяo��
	MainSeq_EndDetail,	// ���I����

	MainSeq_CallDetail,	// ���z�Ăяo��
	MainSeq_EndDetail,	// ���z�I����
	MainSeq_CallDetail,	// �����Ăяo��
	MainSeq_EndDetail,	// �����I����
	MainSeq_CallDetail,	// �p�Ăяo��
	MainSeq_EndDetail,	// �p�I����

	MainSeq_CallSearch,		// �����Ăяo��
	MainSeq_EndSearch,		// �����I����
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
	ZUKAN_MAIN_WORK * wk;

	OS_Printf( "�����������@�}�ӏ����J�n�@����������\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_SYS, 0x8000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_MAIN_WORK), HEAPID_ZUKAN_SYS );
	GFL_STD_MemClear( wk, sizeof(ZUKAN_MAIN_WORK) );

	wk->prm = pwk;

	wk->listMax = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
	ZKNCOMM_ResetSortData( wk->prm->savedata, &wk->sort );

	if( wk->prm->callMode == ZUKAN_MODE_LIST ){
		wk->listMode = ZKNLIST_CALL_NORMAL;
		wk->seq = SEQ_LIST_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_INFO ){
		wk->seq = SEQ_INFO_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_MAP ){
		wk->seq = SEQ_MAP_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_VOICE ){
		wk->seq = SEQ_VOICE_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_FORM ){
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
	FreeListData( mywk );

	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_SYS );

	OS_Printf( "�����������@�}�ӏ����I���@����������\n" );

	return GFL_PROC_RES_FINISH;
}

static void FreeListData( ZUKAN_MAIN_WORK * wk )
{
	if( wk->list != NULL ){
		GFL_HEAP_FreeMemory( wk->list );
		wk->list = NULL;
	}
}




static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk )
{
	ZUKANLIST_DATA * list;

	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANLIST_DATA) );
	list = wk->work;

	list->gamedata = wk->prm->gamedata;
	list->savedata = wk->prm->savedata;

	list->callMode = wk->listMode;
	list->list     = wk->list;
	list->listMax  = wk->listMax;

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
		if( wk->listMode == ZKNLIST_CALL_NORMAL ){
			wk->prm->retMode = ZUKAN_RET_NORMAL;
			ret = SEQ_PROC_FINISH;
		}else{
			FreeListData( wk );
			wk->listMax  = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
			wk->listMode = ZKNLIST_CALL_NORMAL;
			ret = SEQ_LIST_CALL;
		}
		break;

	case ZKNLIST_RET_EXIT_X:	// �}�ӂ��I�����ă��j���[�����
		wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNLIST_RET_MODE_CHANGE:	// �}�Ӄ��[�h��؂�ւ���
		FreeListData( wk );
		wk->listMax = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
		ret = SEQ_LIST_CALL;
		break;

	case ZKNLIST_RET_INFO:		// �ڍ׉�ʂ�
		ret = SEQ_INFO_CALL;
		break;

	case ZKNLIST_RET_SEARCH:	// ������ʂ�
		if( wk->listMode == ZKNLIST_CALL_SEARCH ){
			FreeListData( wk );
			wk->listMax = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
		}
		ret = SEQ_SEARCH_CALL;
		break;

	default:
		wk->prm->retMode = ZUKAN_RET_NORMAL;
		ret = SEQ_PROC_FINISH;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}

static int MainSeq_CallSearch( ZUKAN_MAIN_WORK * wk )
{
	ZUKANSEARCH_DATA * search;

	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANSEARCH_DATA) );
	search = wk->work;

	search->gamedata = wk->prm->gamedata;
	search->savedata = wk->prm->savedata;
	search->sort     = &wk->sort;
	search->list     = NULL;
	search->listMax  = 0;
	GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan_search), &ZUKANSEARCH_ProcData, wk->work );

	return SEQ_SEARCH_END;
}

static int MainSeq_EndSearch( ZUKAN_MAIN_WORK * wk )
{
	ZUKANSEARCH_DATA * search;
	int	ret;

	search = wk->work;

	switch( search->retMode ){
	case ZKNSEARCH_RET_START:		// �������s
		FreeListData( wk );
		wk->list     = search->list;
		wk->listMax  = search->listMax;
		wk->listMode = ZKNLIST_CALL_SEARCH;
		ret = SEQ_LIST_CALL;
		break;
	
	case ZKNSEARCH_RET_CANCEL:	// �L�����Z��
		ret = SEQ_LIST_CALL;
		break;

	case ZKNSEARCH_RET_EXIT_X:	// �}�ӂ��I�����ă��j���[�����
	default:
		wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
		ret = SEQ_PROC_FINISH;
		break;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		�}�ӏڍ׉��
 *
 * @param		wk     ZUKAN_MAIN_WORK
 *
 * @return	���̃��C���V�[�P���X
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CallDetail( ZUKAN_MAIN_WORK * wk )
{
  ZUKAN_DETAIL_PARAM*   detail;

  GAMEDATA*             gamedata       = wk->prm->gamedata;
  ZUKAN_SAVEDATA*       zukan_savedata = GAMEDATA_GetZukanSave( gamedata );
  u16                   monsno         = ZUKANSAVE_GetDefaultMons( zukan_savedata );

	detail = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKAN_DETAIL_PARAM) );
  
  detail->gamedata = wk->prm->gamedata;
  detail->type     = ZUKAN_DETAIL_TYPE_INFO;
  detail->list     = wk->list;
  detail->num      = wk->listMax;
  detail->no       = 0;
  {
    u16 i;
    for( i=0; i<detail->num; i++ )
    {
      if( detail->list[i] == monsno )
      {
        detail->no = i;
        break;
      }
    }
  }

	GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan_detail), &ZUKAN_DETAIL_ProcData, detail );

  wk->work = detail;	
  return SEQ_INFO_END;
}

static int MainSeq_EndDetail( ZUKAN_MAIN_WORK * wk )
{
	ZUKAN_DETAIL_PARAM*   detail;

  GAMEDATA*             gamedata       = wk->prm->gamedata;
  ZUKAN_SAVEDATA*       zukan_savedata = GAMEDATA_GetZukanSave( gamedata );
  u16                   monsno;

	int	                  ret;

	detail = wk->work;

  {
    monsno = detail->list[detail->no];
    ZUKANSAVE_SetDefaultMons( zukan_savedata, monsno );
  }

  switch( detail->ret )
  {
  case ZUKAN_DETAIL_RET_CLOSE:
    {
		  wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
      ret = SEQ_PROC_FINISH;
    }
    break;
  case ZUKAN_DETAIL_RET_RETURN:
    {
      wk->prm->retMode = ZUKAN_RET_NORMAL;
      ret = SEQ_LIST_CALL;
    }
    break;
  }
		
	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}

