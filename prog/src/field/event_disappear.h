////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �ޏ�C�x���g
 * @file   event_disappear.h
 * @author obata
 * @date   2009.08.28
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once

//------
// �ޏ�
//------
// ���[�v
GMEVENT* EVENT_DISAPPEAR_Warp( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap ); 

// ����
GMEVENT* EVENT_DISAPPEAR_FallInSand( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, const VecFx32* stream_pos ); 
