//======================================================================
/**
 * @file	debug_pause.h
 * @brief	�R�}����@�\
 * @author	ariizumi
 * @data	09/03/18
 */
//======================================================================

#ifndef DEBUG_PAUSE_H__
#define DEBUG_PAUSE_H__

#ifdef PM_DEBUG

void DEBUG_PAUSE_Init( void );
//�߂�l��FALSE�Ȃ�t���[���������΂�
const BOOL DEBUG_PAUSE_Update( void );

//�L���̐ݒ�E�擾
void DEBUG_PAUSE_SetEnable( const BOOL isEnable );
const BOOL DEBUG_PAUSE_GetEnable( void );

#endif //PM_DEBUG

#endif //DEBUG_PAUSE_H__
