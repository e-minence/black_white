//==============================================================================
/**
 * @file  guru2.h
 * @brief ���邮�����
 * @author  AkitoMori(�ڐA�j/ kagaya(�쐬�j
 * @data  09.01.15
 */
//==============================================================================
#ifndef GURU2_H_FILE
#define GURU2_H_FILE

#include <gflib.h>

#include "gamesystem/game_data.h"
#include "net_app/union_app.h"

//#include "savedata/savedata_def.h"
#include "savedata/record.h"

//==============================================================================
//  define
//==============================================================================

// ���邮������̐ڑ��ő�l��
#define NET_GURU2_CONNECT_MAX   ( 5 )

//==============================================================================
//  typedef
//==============================================================================
typedef struct GURU2PROC_WORK GURU2PROC_WORK;

//--------------------------------------------------------------
/// GURU2_PARAM
//--------------------------------------------------------------
// net_app/union_app.h�̒��ɂ���
// GURUGURU_PARENT_WORK���g�p���Ă�������

//==============================================================================
//  extern
//==============================================================================
extern GURU2PROC_WORK * Guru2_WorkInit( GURUGURU_PARENT_WORK *param, u32 heap_id );
extern void Guru2_WorkDelete( GURU2PROC_WORK *g2p );
extern BOOL Guru2_ReceiptRetCheck( GURU2PROC_WORK *g2p );

extern void * EventCmd_Guru2ProcWorkAlloc( GAMEDATA *gamedata );
extern BOOL EventCmd_Guru2Proc( void *wk );

extern GFL_PROC_RESULT Guru2ReceiptProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2ReceiptProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2ReceiptProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

extern GFL_PROC_RESULT Guru2MainProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2MainProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2MainProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

extern GFL_PROC_RESULT Guru2Proc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2Proc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );
extern GFL_PROC_RESULT Guru2Proc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

//--------------------------------------------------------------
/// ���邮������v���b�N�f�[�^
//--------------------------------------------------------------
extern const GFL_PROC_DATA Guru2ProcData;

#endif //GURU2_H_FILE
