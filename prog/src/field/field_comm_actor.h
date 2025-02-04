//======================================================================
/**
 * @file	field_comm_actor.h
 * @brief	
 * @author
 * @date
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "fldmmdl.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================
///FIELD_COMM_ACTOR_CTRL
typedef struct _TAG_FIELD_COMM_ACTOR_CTRL FIELD_COMM_ACTOR_CTRL;

//======================================================================
//  extern 
//======================================================================
extern FIELD_COMM_ACTOR_CTRL * FIELD_COMM_ACTOR_CTRL_Create(
    u16 max, MMDLSYS *mmdlsys, HEAPID heapID, BOOL dash_flag );
extern void FIELD_COMM_ACTOR_CTRL_Delete( FIELD_COMM_ACTOR_CTRL *act_ctrl );
extern void FIELD_COMM_ACTOR_CTRL_AddActor(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id, u16 code,
    const u16 *watch_dir, const VecFx32 *watch_pos, const BOOL *watch_vanish );
extern void FIELD_COMM_ACTOR_CTRL_DeleteActro(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id );

extern BOOL FIELD_COMM_ACTOR_CTRL_SearchGridPos(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, s16 gx, s16 gy, s16 gz,
    u32 *outID, u32 *no );

extern MMDL * FIELD_COMM_ACTOR_CTRL_GetMMdl(
    FIELD_COMM_ACTOR_CTRL *act_ctrl, u32 id );

extern void MMDL_MoveCommActor_Init( MMDL *fmmdl );
extern void MMDL_MoveCommActor_Delete( MMDL *fmmdl );
extern void MMDL_MoveCommActor_Move( MMDL *fmmdl );

