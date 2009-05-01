//======================================================================
/**
 * @file  field_comm_main.c
 * @brief �t�B�[���h�ʐM�̏����̊O�ƒ����Ȃ��֐��Ƃ��Q
 * @author  ariizumi
 * @data  08/11/11
 */
//======================================================================

#ifndef FIELD_COMM_MAIN__
#define FIELD_COMM_MAIN__

#include "field_comm_def.h"
#include "../fieldmap.h"
#include "../field_player.h"
#include "gamesystem/gamesystem.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  enum
//======================================================================

//======================================================================
//  typedef struct
//======================================================================

//======================================================================
//  proto
//======================================================================
extern FIELD_COMM_MAIN* FIELD_COMM_MAIN_InitSystem( HEAPID heapID , HEAPID commHeapID );
extern void FIELD_COMM_MAIN_TermSystem( FIELD_MAIN_WORK *fieldWork, FIELD_COMM_MAIN *commSys );
extern void FIELD_COMM_MAIN_UpdateCommSystem( FIELD_MAIN_WORK *fieldWork , GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor , FIELD_COMM_MAIN *commSys );

extern const BOOL FIELD_COMM_MAIN_CanTalk( FIELD_COMM_MAIN *commSys );
extern const BOOL FIELD_COMM_MAIN_CheckReserveTalk( FIELD_COMM_MAIN *commSys );
extern void FIELD_COMM_MAIN_StartTalk( FIELD_COMM_MAIN *commSys );
extern void FIELD_COMM_MAIN_StartTalkPartner( FIELD_COMM_MAIN *commSys );

extern void FIELD_COMM_MAIN_InitStartCommMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG );
extern void FIELD_COMM_MAIN_TermStartCommMenu( FIELD_COMM_MAIN *commSys );
extern const BOOL  FIELD_COMM_MAIN_LoopStartCommMenu( FIELD_COMM_MAIN *commSys, GAMESYS_WORK *gsys );

extern void FIELD_COMM_MAIN_InitStartInvasionMenu( FIELD_COMM_MAIN *commSys, FLDMSGBG *fldMsgBG );
extern void FIELD_COMM_MAIN_TermStartInvasionMenu( FIELD_COMM_MAIN *commSys );
extern const BOOL FIELD_COMM_MAIN_LoopStartInvasionMenu( GAMESYS_WORK *gsys, FIELD_COMM_MAIN *commSys );

extern void FIELD_COMM_MAIN_Disconnect( FIELD_MAIN_WORK *fieldWork , FIELD_COMM_MAIN *commSys );
extern BOOL FIELD_COMM_MAIN_DisconnectWait( FIELD_MAIN_WORK *fieldWork, FIELD_COMM_MAIN *commSys );

extern COMM_FIELD_SYS_PTR FIELD_COMM_MAIN_GetCommFieldSysPtr( FIELD_COMM_MAIN *commSys );
extern const int FIELD_COMM_MAIN_GetWorkSize(void);
extern FIELD_COMM_MENU** FIELD_COMM_MAIN_GetCommMenuWork( FIELD_COMM_MAIN *commSys );
extern const HEAPID FIELD_COMM_MAIN_GetHeapID( FIELD_COMM_MAIN *commSys );

extern void FIELD_COMM_MAIN_CommFieldSysPtrSet( FIELD_COMM_MAIN *commSys, COMM_FIELD_SYS_PTR comm_field);
extern COMM_FIELD_SYS_PTR FIELD_COMM_MAIN_CommFieldSysAlloc(FIELD_COMM_MAIN *commSys, HEAPID commHeapID);
extern void FIELD_COMM_MAIN_CommFieldSysFree(FIELD_COMM_MAIN *commSys);
extern void FIELD_COMM_MAIN_CommFieldMapInit(COMM_FIELD_SYS_PTR comm_field);


#endif //FIELD_COMM_MAIN__

