//===============================================================================
/*
 *  @file event_debug_livecomm.h
 *  @brief  ����Ⴂ�ʐM�f�o�b�O���j���[�C�x���g
 *  @author Miyuki Iwasawa
 *  @date   10.04.19
 */
//===============================================================================

#pragma once

#ifdef PM_DEBUG

FS_EXTERN_OVERLAY( d_livecomm );


extern GMEVENT* DEBUG_EVENT_LiveComm( GAMESYS_WORK* gsys, void* parent_work );
extern GMEVENT* DEBUG_EVENT_LiveCommFromField( GAMESYS_WORK* gsys,void* parent_work );





#endif  //PM_DEBUG


