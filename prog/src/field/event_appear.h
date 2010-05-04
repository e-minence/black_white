////////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �o��C�x���g
 * @file   event_appear.h
 * @author obata
 * @date   2009.08.28
 *
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once

// ����
extern GMEVENT* EVENT_APPEAR_Fall( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );  

// ���Ȃʂ��̃q��
extern GMEVENT* EVENT_APPEAR_Ananukenohimo(
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, 
    BOOL season_change_flag, int prev_season, int now_season );  

// ���Ȃ��ق�
extern GMEVENT* EVENT_APPEAR_Anawohoru( 
    GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, 
    BOOL season_change_flag, int prev_season, int now_season );  

// �e���|�[�g
extern GMEVENT* EVENT_APPEAR_Teleport( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );  

// ���[�v
extern GMEVENT* EVENT_APPEAR_Warp( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );  

// ���j�I������
extern GMEVENT* EVENT_APPEAR_UnionIn( GMEVENT* parent, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap );
