///////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�w��ĉ��x
 * @file   event_debug_sodateya.h
 * @author obata
 * @date   2010.04.22
 */
///////////////////////////////////////////////////////////////////////////////
#ifdef PM_DEBUG
#pragma once

FS_EXTERN_OVERLAY( debug_sodateya );

//-----------------------------------------------------------------------------
/**
 * @brief ��ĉ��f�o�b�O���j���[�C�x���g�𐶐�����
 *
 * @parma gameSystem
 * @param wk         FIELDMAP_WORK�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_DebugMenuSodateya( GAMESYS_WORK* gameSystem, void* wk );


#endif // PM_DEBUG
