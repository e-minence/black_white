////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �ޏꁕ�o��C�x���g
 * @file   event_appear.h
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
GMEVENT* EVENT_DISAPPEAR_FallInSand( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap ); 

//------
// �o��
//------
// ���[�v
GMEVENT* EVENT_APPEAR_Warp( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );  

// ����
GMEVENT* EVENT_APPEAR_Fall( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );  
