//////////////////////////////////////////////////////////////////////////
/**
 * @brief  �S�ڑ��`�F�b�N�C�x���g
 * @file   event_debug_all_connect_check.c
 * @author obata
 * @date   2010.04.21
 */
//////////////////////////////////////////////////////////////////////////
#ifdef PM_DEBUG
#pragma once

FS_EXTERN_OVERLAY( debug_connect_check );

//------------------------------------------------------------------------
/**
 * @brief �S�ڑ��`�F�b�N�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param wk
 *
 * @return ���������C�x���g
 */
//------------------------------------------------------------------------
extern GMEVENT* EVENT_DEBUG_AllConnectCheck( GAMESYS_WORK* gameSystem, void* wk );

//------------------------------------------------------------------------
/**
 * @brief ���}�b�v�̐ڑ��`�F�b�N�C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param wk
 *
 * @return ���������C�x���g
 */
//------------------------------------------------------------------------
extern GMEVENT* EVENT_DEBUG_NowConnectCheck( GAMESYS_WORK* gameSystem, void* wk );

#endif
