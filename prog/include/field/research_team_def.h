/////////////////////////////////////////////////////////////////////
/**
 * @brief  ����Ⴂ������ �萔��`
 * @file   research_team_def.h
 * @author obata
 * @date   2010.03.08
 */
/////////////////////////////////////////////////////////////////////
#pragma once

// 1�̒����˗��Œ��ׂ鎿��̍ő吔
#define MAX_QNUM_PER_RESEARCH_REQ (3)

// �����������N
#define RESEARCH_MEMBER_RANK_C  (0)
#define RESEARCH_MEMBER_RANK_B  (1)
#define RESEARCH_MEMBER_RANK_A  (2)
#define RESEARCH_MEMBER_RANK_S  (3)
#define RESEARCH_MEMBER_RANK_SS (4)

///�����������N
#define RESEARCH_TEAM_RANK_0 (0)
#define RESEARCH_TEAM_RANK_1 (1)
#define RESEARCH_TEAM_RANK_2 (2)
#define RESEARCH_TEAM_RANK_3 (3)
#define RESEARCH_TEAM_RANK_4 (4)
#define RESEARCH_TEAM_RANK_5 (5)
#define RESEARCH_TEAM_RANK_MAX (6)

// �����^�C�v
#define RESEARCH_REQ_TYPE_COUNT (0) // �l������
#define RESEARCH_REQ_TYPE_TIME  (1) // ���Ԓ���
#define RESEARCH_REQ_TYPE_NUM   (2) // ����

// �����˗�ID
#define RESEARCH_REQ_ID_NONE                 (0) // �_�~�[ ( �˗����󂯂Ă��Ȃ� )
#define RESEARCH_REQ_ID_WB_count             (1) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_WB_time              (2) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_FIRST_POKE_count     (3) //�u����񂾁@��������̃|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_FIRST_POKE_time      (4) //�u����񂾁@��������̃|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_PLAY_TIME_count      (5) //�u�����΂񂨂����@�v���C������́H�v( �l�� )
#define RESEARCH_REQ_ID_PLAY_TIME_time       (6) //�u�����΂񂨂����@�v���C������́H�v( ���� )
#define RESEARCH_REQ_ID_SEX_count            (7) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_SEX_time             (8) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_JOB_count            (9) //�u�����΂񂨂����@�����Ƃ́H�v( �l�� )
#define RESEARCH_REQ_ID_JOB_time            (10) //�u�����΂񂨂����@�����Ƃ́H�v( ���� )
#define RESEARCH_REQ_ID_HOBBY_count         (11) //�u�����΂񂨂����@����݂́H�v( �l�� )
#define RESEARCH_REQ_ID_HOBBY_time          (12) //�u�����΂񂨂����@����݂́H�v( ���� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_count    (13) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_time     (14) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_count (15) //�u�����ȁ@�|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_time  (16) //�u�����ȁ@�|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_POKE_TYPE_count     (17) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( �l�� )
#define RESEARCH_REQ_ID_POKE_TYPE_time      (18) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( ���� )
#define RESEARCH_REQ_ID_PLACE_count         (19) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_PLACE_time          (20) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( ���� )
#define RESEARCH_REQ_ID_PRECIOUS_count      (21) //�u�������ɂ��Ă�����̂Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_PRECIOUS_time       (22) //�u�������ɂ��Ă�����̂Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_PARTNER_count       (23) //�u�ЂƂ݂̂�傭�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_PARTNER_time        (24) //�u�ЂƂ݂̂�傭�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SEASON_count        (25) //�u�����Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_SEASON_time         (26) //�u�����Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_ART_count           (27) //�u��������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_ART_time            (28) //�u��������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_MUSIC_count         (29) //�u�����΂񂷂��ȁ@���񂪂��́H�v( �l�� )
#define RESEARCH_REQ_ID_MUSIC_time          (30) //�u�����΂񂷂��ȁ@���񂪂��́H�v( ���� )
#define RESEARCH_REQ_ID_MOVIE_count         (31) //�u���������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_MOVIE_time          (32) //�u���������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SCHOOL_count        (33) //�u�����΂񂷂��ȁ@���������̂�����́H�v( �l�� )
#define RESEARCH_REQ_ID_SCHOOL_time         (34) //�u�����΂񂷂��ȁ@���������̂�����́H�v( ���� )
#define RESEARCH_REQ_ID_STUDY_count         (35) //�u�ׂ񂫂傤�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_STUDY_time          (36) //�u�ׂ񂫂傤�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_STORTS_count        (37) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( �l�� )
#define RESEARCH_REQ_ID_STORTS_time         (38) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( ���� )
#define RESEARCH_REQ_ID_DAY_OFF_count       (39) //�u���イ���Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_DAY_OFF_time        (40) //�u���イ���Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_DREAM_count         (41) //�u�����΂�Ȃ肽�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_DREAM_time          (42) //�u�����΂�Ȃ肽�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_GYM_LEADER_count    (43) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( �l�� )
#define RESEARCH_REQ_ID_GYM_LEADER_time     (44) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( ���� )
#define RESEARCH_REQ_ID_FUN_count           (45) //�u�|�P�����̂������낢�Ƃ���́H�v( �l�� )
#define RESEARCH_REQ_ID_FUN_time            (46) //�u�|�P�����̂������낢�Ƃ���́H�v( ���� )
#define RESEARCH_REQ_ID_NUM                 (47) // ����
#define RESEARCH_REQ_ID_MAX                 (RESEARCH_REQ_ID_NUM-1) // �ő�l

// �A���P�[�gID
#define RESEARCH_QUESTIONNAIRE_ID_YOU           (0) // �ǂ�ȂЂƂ��@���邩
#define RESEARCH_QUESTIONNAIRE_ID_FAVARITE_POKE (1) // �����ȃ|�P����
#define RESEARCH_QUESTIONNAIRE_ID_WISH          (2) // �݂�Ȃ́@�肻��
#define RESEARCH_QUESTIONNAIRE_ID_PARTNER       (3) // �����ȂЂ�
#define RESEARCH_QUESTIONNAIRE_ID_TASTE         (4) // �݂�Ȃ́@���̂�
#define RESEARCH_QUESTIONNAIRE_ID_HOBBY         (5) // �݂�Ȃ́@�����
#define RESEARCH_QUESTIONNAIRE_ID_SCHOOL        (6) // ����������������
#define RESEARCH_QUESTIONNAIRE_ID_PLAY          (7) // �ǂ�ȁ@�����т�������
#define RESEARCH_QUESTIONNAIRE_ID_POKEMON       (8) // �|�P����
#define RESEARCH_QUESTIONNAIRE_ID_DUMMY         (9) // �_�~�[
