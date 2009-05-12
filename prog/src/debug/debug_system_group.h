//======================================================================
/**
 * @file	debug_system_group.h
 * @brief	�f�o�b�O�V�X�e���̃f�t�H���g�O���[�v�o�^
 * @author	ariizumi
 * @data	09/05/08
 */
//======================================================================

#ifndef DEBUG_SYSTEM_GROUP_H__
#define DEBUG_SYSTEM_GROUP_H__

#define DEBUGWIN_GROUPID_SYSTEM (254)
#define DEBUGWIN_GROUPID_RTC (253)

void DEBUGWIN_AddSystemGroup( const HEAPID heapId );
void DEBUGWIN_RemoveSystemGroup(void);

#endif //DEBUG_SYSTEM_GROUP_H__
