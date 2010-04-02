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
#define RESEARCH_REQ_TYPE_DUMMY (3) // �_�~�[

// �����˗�ID
#define RESEARCH_REQ_ID_NONE                  (0) // �_�~�[ ( �˗����󂯂Ă��Ȃ� )
#define RESEARCH_REQ_ID_WB_time               (1) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_FIRST_POKE_time       (2) //�u����񂾁@��������̃|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_PLAY_TIME_time        (3) //�u�����΂񂨂����@�v���C������́H�v( ���� )
#define RESEARCH_REQ_ID_SEX_time              (4) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_JOB_time              (5) //�u�����΂񂨂����@�����Ƃ́H�v( ���� )
#define RESEARCH_REQ_ID_HOBBY_time            (6) //�u�����΂񂨂����@����݂́H�v( ���� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_time       (7) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_time    (8) //�u�����ȁ@�|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_POKE_TYPE_time        (9) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( ���� )
#define RESEARCH_REQ_ID_PLACE_time           (10) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( ���� )
#define RESEARCH_REQ_ID_PRECIOUS_time        (11) //�u�������ɂ��Ă�����̂Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_PARTNER_time         (12) //�u�ЂƂ݂̂�傭�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SEASON_time          (13) //�u�����Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_ART_time             (14) //�u��������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_MUSIC_time           (15) //�u�����΂񂷂��ȁ@���񂪂��́H�v( ���� )
#define RESEARCH_REQ_ID_MOVIE_time           (16) //�u���������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SCHOOL_time          (17) //�u�����΂񂷂��ȁ@���������̂�����́H�v( ���� )
#define RESEARCH_REQ_ID_STUDY_time           (18) //�u�ׂ񂫂傤�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_STORTS_time          (19) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( ���� )
#define RESEARCH_REQ_ID_DAY_OFF_time         (20) //�u���イ���Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_DREAM_time           (21) //�u�����΂�Ȃ肽�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_GYM_LEADER_time      (22) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( ���� )
#define RESEARCH_REQ_ID_FUN_time             (23) //�u�|�P�����̂������낢�Ƃ���́H�v( ���� )
#define RESEARCH_REQ_ID_WB_count             (24) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_FIRST_POKE_count     (25) //�u����񂾁@��������̃|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_PLAY_TIME_count      (26) //�u�����΂񂨂����@�v���C������́H�v( �l�� )
#define RESEARCH_REQ_ID_SEX_count            (27) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_JOB_count            (28) //�u�����΂񂨂����@�����Ƃ́H�v( �l�� )
#define RESEARCH_REQ_ID_HOBBY_count          (29) //�u�����΂񂨂����@����݂́H�v( �l�� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_count     (30) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_count  (31) //�u�����ȁ@�|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_POKE_TYPE_count      (32) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( �l�� )
#define RESEARCH_REQ_ID_PLACE_count          (33) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_PRECIOUS_count       (34) //�u�������ɂ��Ă�����̂Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_PARTNER_count        (35) //�u�ЂƂ݂̂�傭�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_SEASON_count         (36) //�u�����Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_ART_count            (37) //�u��������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_MUSIC_count          (38) //�u�����΂񂷂��ȁ@���񂪂��́H�v( �l�� )
#define RESEARCH_REQ_ID_MOVIE_count          (39) //�u���������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_SCHOOL_count         (40) //�u�����΂񂷂��ȁ@���������̂�����́H�v( �l�� )
#define RESEARCH_REQ_ID_STUDY_count          (41) //�u�ׂ񂫂傤�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_STORTS_count         (42) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( �l�� )
#define RESEARCH_REQ_ID_DAY_OFF_count        (43) //�u���イ���Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_DREAM_count          (44) //�u�����΂�Ȃ肽�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_GYM_LEADER_count     (45) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( �l�� )
#define RESEARCH_REQ_ID_FUN_count            (46) //�u�|�P�����̂������낢�Ƃ���́H�v( �l�� )
#define RESEARCH_REQ_ID_NUM                  (47) // ����
#define RESEARCH_REQ_ID_MAX (RESEARCH_REQ_ID_NUM-1) // �ő�l
#define RESEARCH_REQ_ID_MIN_TYPE_COUNT (RESEARCH_REQ_ID_WB_count)  // �l�������̈˗�ID�̍ŏ��l
#define RESEARCH_REQ_ID_MAX_TYPE_COUNT (RESEARCH_REQ_ID_FUN_count) // �l�������̈˗�ID�̍ő�l
#define RESEARCH_REQ_ID_MIN_TYPE_TIME  (RESEARCH_REQ_ID_WB_time)   // ���Ԓ����̈˗�ID�̍ŏ��l
#define RESEARCH_REQ_ID_MAX_TYPE_TIME  (RESEARCH_REQ_ID_FUN_time)  // ���Ԓ����̈˗�ID�̍ő�l

// �˗��̒B����
#define RESEARCH_REQ_STATE_NULL             (0) // �˗����󂯂Ă��Ȃ�
#define RESEARCH_REQ_STATE_ACHIEVE          (1) // �˗���B��
#define RESEARCH_REQ_STATE_FALSE_NORM_COUNT (2) // �l�������ɂ�����, �m���}�l����B�����Ă��Ȃ�
#define RESEARCH_REQ_STATE_FALSE_NORM_TIME  (3) // ���Ԓ����ɂ�����, �m���}���Ԃ�B�����Ă��Ȃ�
#define RESEARCH_REQ_STATE_FALSE_ZERO_COUNT (4) // ���Ԓ����ɂ�����, �P�l���������Ă��Ȃ�

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
