/**
 *  @file   event_fishing.h
 *  @brief  �ނ�C�x���g
 *  @author Miyuki Iwasawa
 *  @datge  09.12.04
 */


/*
 *  @brief  �ނ肪�ł���|�W�V�������`�F�b�N
 *
 *  @param  outPos  ���W���v��Ȃ����NULL�ŗǂ�
 */
extern BOOL FieldFishingCheckPos( GAMEDATA* gdata, FIELDMAP_WORK* fieldmap, VecFx32* outPos );

//------------------------------------------------------------------
/*
 *  @brief  �ނ�C�x���g�N��
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_FieldFishing( FIELDMAP_WORK* fieldmap, GAMESYS_WORK* gsys );



