/*
 *  @file   event_debug_numinput.c
 *  @brief  �f�o�b�O���l����
 *  @author Miyuki Iwasawa
 *  @date   09.11.25
 */

#pragma once

//�I�[�o�[���CID extern
FS_EXTERN_OVERLAY( d_numinput );

typedef enum {  
  D_NUMINPUT_ENCEFF = 0,
  D_NUMINPUT_DUMMY,
  D_NUMINPUT_MODE_MAX
}D_NUMINPUT_MODE;

//--------------------------------------------------------------
/**
 * ���l���̓f�o�b�O���j���[�C�x���g����
 * @param wk  DEBUG_MENU_EVENT_WORK*
 * @retval  BOOL  TRUE=�C�x���g�p��
 */
//--------------------------------------------------------------
extern GMEVENT* EVENT_DMenuNumInput( GAMESYS_WORK* gsys, void* work );

