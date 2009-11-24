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

// ����
GMEVENT* EVENT_DISAPPEAR_FallInSand( GMEVENT* parent, 
                                     GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, 
                                     const VecFx32* stream_pos ); 
// ���Ȃʂ��̃q��
GMEVENT* EVENT_DISAPPEAR_Ananukenohimo( GMEVENT* parent, 
                                        GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap ); 
// ���Ȃ��ق�
GMEVENT* EVENT_DISAPPEAR_Anawohoru( GMEVENT* parent, 
                                    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );
// �e���|�[�g
GMEVENT* EVENT_DISAPPEAR_Teleport( GMEVENT* parent, 
                                   GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );
