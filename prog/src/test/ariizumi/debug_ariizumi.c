//============================================================================================
/**
 * @file	debug_ariizumi.c
 * @brief	�f�o�b�O �L��
 * @author	ariizumi
 * @date	2006.11.29
 */
//============================================================================================
#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "test/ohno/debug_ohno.h"
#include "system/main.h"

//------------------------------------------------------------------
//  �f�o�b�O�p�������֐�
//------------------------------------------------------------------

static GFL_PROC_RESULT DebugAriizumiMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	DEBUG_OHNO_CONTROL * testmode;
	HEAPID			heapID = HEAPID_OHNO_DEBUG;

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x30000 );

	testmode = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), heapID );
	GFL_STD_MemClear(testmode, sizeof(DEBUG_OHNO_CONTROL));
	testmode->debug_heap_id = heapID;

	return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**   �f�o�b�O�p���C��
 */  
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugAriizumiMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
	DEBUG_OHNO_CONTROL * testmode = mywk;
	if(testmode->funcNet){
        if(testmode->funcNet(mywk)){
            return GFL_PROC_RES_FINISH;
        }
    }
    return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**  �f�o�b�O�pExit
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugAriizumiMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
    GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap( HEAPID_OHNO_DEBUG );

    return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugAriizumiMainProcData = {
	DebugAriizumiMainProcInit,
	DebugAriizumiMainProcMain,
	DebugAriizumiMainProcEnd,
};
