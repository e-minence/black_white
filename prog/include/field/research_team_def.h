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
#define RESEARCH_MEMBER_RANK_C  (0) // �����NC
#define RESEARCH_MEMBER_RANK_B  (1) // �����NB
#define RESEARCH_MEMBER_RANK_A  (2) // �����NA
#define RESEARCH_MEMBER_RANK_S  (3) // �����NS
#define RESEARCH_MEMBER_RANK_SS (4) // �����NSS

// �����˗�ID
#define RESEARCH_REQ_ID_WB_count             (0) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_WB_time              (1) //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_FIRST_POKE_count     (2) //�u����񂾁@��������̃|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_FIRST_POKE_time      (3) //�u����񂾁@��������̃|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_PLAY_TIME_count      (4) //�u�����΂񂨂����@�v���C������́H�v( �l�� )
#define RESEARCH_REQ_ID_PLAY_TIME_time       (5) //�u�����΂񂨂����@�v���C������́H�v( ���� )
#define RESEARCH_REQ_ID_SEX_count            (6) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( �l�� )
#define RESEARCH_REQ_ID_SEX_time             (7) //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( ���� )
#define RESEARCH_REQ_ID_JOB_count            (8) //�u�����΂񂨂����@�����Ƃ́H�v( �l�� )
#define RESEARCH_REQ_ID_JOB_time             (9) //�u�����΂񂨂����@�����Ƃ́H�v( ���� )
#define RESEARCH_REQ_ID_HOBBY_count         (10) //�u�����΂񂨂����@����݂́H�v( �l�� )
#define RESEARCH_REQ_ID_HOBBY_time          (11) //�u�����΂񂨂����@����݂́H�v( ���� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_count    (12) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_BTL_OR_TRD_time     (13) //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_count (14) //�u�����ȁ@�|�P�����́H�v( �l�� )
#define RESEARCH_REQ_ID_FAVORITE_POKE_time  (15) //�u�����ȁ@�|�P�����́H�v( ���� )
#define RESEARCH_REQ_ID_POKE_TYPE_count     (16) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( �l�� )
#define RESEARCH_REQ_ID_POKE_TYPE_time      (17) //�u�����ȁ@�|�P�����̃^�C�v�́H�v( ���� )
#define RESEARCH_REQ_ID_PLACE_count         (18) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_PLACE_time          (19) //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( ���� )
#define RESEARCH_REQ_ID_PRECIOUS_count      (20) //�u�������ɂ��Ă�����̂Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_PRECIOUS_time       (21) //�u�������ɂ��Ă�����̂Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_PARTNER_count       (22) //�u�ЂƂ݂̂�傭�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_PARTNER_time        (23) //�u�ЂƂ݂̂�傭�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SEASON_count        (24) //�u�����Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_SEASON_time         (25) //�u�����Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_ART_count           (26) //�u��������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_ART_time            (27) //�u��������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_MUSIC_count         (28) //�u�����΂񂷂��ȁ@���񂪂��́H�v( �l�� )
#define RESEARCH_REQ_ID_MUSIC_time          (29) //�u�����΂񂷂��ȁ@���񂪂��́H�v( ���� )
#define RESEARCH_REQ_ID_MOVIE_count         (30) //�u���������Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_MOVIE_time          (31) //�u���������Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_SCHOOL_count        (32) //�u�����΂񂷂��ȁ@���������̂�����́H�v( �l�� )
#define RESEARCH_REQ_ID_SCHOOL_time         (33) //�u�����΂񂷂��ȁ@���������̂�����́H�v( ���� )
#define RESEARCH_REQ_ID_STUDY_count         (34) //�u�ׂ񂫂傤�Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_STUDY_time          (35) //�u�ׂ񂫂傤�Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_STORTS_count        (36) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( �l�� )
#define RESEARCH_REQ_ID_STORTS_time         (37) //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( ���� )
#define RESEARCH_REQ_ID_DAY_OFF_count       (38) //�u���イ���Ɂ@���āv( �l�� )
#define RESEARCH_REQ_ID_DAY_OFF_time        (39) //�u���イ���Ɂ@���āv( ���� )
#define RESEARCH_REQ_ID_DREAM_count         (40) //�u�����΂�Ȃ肽�����̂́H�v( �l�� )
#define RESEARCH_REQ_ID_DREAM_time          (41) //�u�����΂�Ȃ肽�����̂́H�v( ���� )
#define RESEARCH_REQ_ID_GYM_LEADER_count    (42) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( �l�� )
#define RESEARCH_REQ_ID_GYM_LEADER_time     (43) //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( ���� )
#define RESEARCH_REQ_ID_FUN_count           (44) //�u�|�P�����̂������낢�Ƃ���́H�v( �l�� )
#define RESEARCH_REQ_ID_FUN_time            (45) //�u�|�P�����̂������낢�Ƃ���́H�v( ���� )
#define RESEARCH_REQ_ID_NUM                 (46) // ����

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
