//======================================================================
/**
 * @file  event_fldmmdl_control.h
 * @brief
 * @date  2009.06.10
 */
//======================================================================

#pragma once

extern GMEVENT * EVENT_ObjPauseAll(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);
extern GMEVENT * EVENT_ObjAnime( GAMESYS_WORK * gys, FIELDMAP_WORK * fieldmap,
    u16 obj_id, void * anm_tbl );
extern GMEVENT * EVENT_PlayerOneStepAnime( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

// �l�W��܂Ŏg�p�@�n�C�W�����v
extern GMEVENT * EVENT_HighJump( GAMESYS_WORK * gsys, MMDL* mmdl, const VecFx32* cp_start, const VecFx32* cp_end );


// ���@�̈���ړ��A�j���𑦊J�n����
extern GFL_TCB* PlayerOneStepAnimeStart( FIELDMAP_WORK* fieldmap );
// ���@�̈���ړ��A�j���̏I����҂�
extern BOOL CheckPlayerOneStepAnimeEnd( GFL_TCB* tcb );
