/////////////////////////////////////////////////////////////////////
/**
 * @brief  ����Ⴂ������
 * @file   research_team.h
 * @author obata
 * @date   2010.03.31
 */
/////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "field/research_team.h" 
#include "field/research_team_def.h"
#include "../app/research_radar/question_id.h"


typedef struct {

  u8 type;      // �����^�C�v
  u8 QNum;      // �����Ώۂ̎��␔
  u8 QID1;      // ����ID1
  u8 QID2;      // ����ID2
  u8 QID3;      // ����ID3
  u8 normCount; // �m���}�l��
  u8 normTime;  // �m���}����

} RESEARCH_REQUEST_DATA;


static const RESEARCH_REQUEST_DATA RequestData[ RESEARCH_REQ_ID_NUM ] = 
{
  { RESEARCH_REQ_TYPE_TIME,  0, QUESTION_ID_DUMMY,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      0,  0 }, // �_�~�[
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_WB,               QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  2 }, //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_FIRST_POKEMON,    QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  2 }, //�u����񂾁@��������̃|�P�����́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_PLAY_TIME,        QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  4 }, //�u�����΂񂨂����@�v���C������́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_SEX,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  4 }, //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_JOB,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  4 }, //�u�����΂񂨂����@�����Ƃ́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_HOBBY,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  4 }, //�u�����΂񂨂����@����݂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_FUN,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  8 }, //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_FAVORITE_POKEMON, QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  8 }, //�u�����ȁ@�|�P�����́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_POKEMON_TYPE,     QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  8 }, //�u�����ȁ@�|�P�����̃^�C�v�́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_PLACE,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1,  8 }, //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_WANT,             QUESTION_ID_PRECIOUS,   QUESTION_ID_DUMMY,      1, 12 }, //�u�������ɂ��Ă�����̂Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  3, QUESTION_ID_PARTNER,          QUESTION_REQUIREMENT,   QUESTION_ID_CHARACTER,  1, 12 }, //�u�ЂƂ݂̂�傭�Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  2, QUESTION_ID_SEASON,           QUESTION_ID_FOOD,       QUESTION_ID_DUMMY,      1, 12 }, //�u�����Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  2, QUESTION_ID_COLOR,            QUESTION_ID_MUSIC,      QUESTION_ID_DUMMY,      1, 12 }, //�u��������Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_MUSIC,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 16 }, //�u�����΂񂷂��ȁ@���񂪂��́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  2, QUESTION_ID_TV,               QUESTION_ID_MOVIE,      QUESTION_ID_DUMMY,      1, 16 }, //�u���������Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_FAVORITE_CLASS,   QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 16 }, //�u�����΂񂷂��ȁ@���������̂�����́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  2, QUESTION_ID_STRONG_CLASS,     QUESTION_ID_WEAK_CLASS, QUESTION_ID_DUMMY,      1, 16 }, //�u�ׂ񂫂傤�Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_SPORTS,           QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 20 }, //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  2, QUESTION_ID_DAY_OFF,          QUESTION_ID_PLAY_SPOT,  QUESTION_ID_DUMMY,      1, 20 }, //�u���イ���Ɂ@���āv( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_DREAM,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 20 }, //�u�����΂�Ȃ肽�����̂́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_GYM_LEADER,       QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 24 }, //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( ���� )
  { RESEARCH_REQ_TYPE_TIME,  1, QUESTION_ID_POKEMON,          QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      1, 24 }, //�u�|�P�����̂������낢�Ƃ���́H�v( ���� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_WB,               QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      5,  0 }, //�u�u���b�N�Ɓ@�z���C�g�@�������̂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_FIRST_POKEMON,    QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,      5,  0 }, //�u����񂾁@��������̃|�P�����́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_PLAY_TIME,        QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     10,  0 }, //�u�����΂񂨂����@�v���C������́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_SEX,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     10,  0 }, //�u���Ƃ��̂��Ɓ@����Ȃ̂��@�������̂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_JOB,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     10,  0 }, //�u�����΂񂨂����@�����Ƃ́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_HOBBY,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     10,  0 }, //�u�����΂񂨂����@����݂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_FUN,              QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     20,  0 }, //�u��������Ɓ@��������@�ɂ񂫂�����̂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_FAVORITE_POKEMON, QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     20,  0 }, //�u�����ȁ@�|�P�����́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_POKEMON_TYPE,     QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     20,  0 }, //�u�����ȁ@�|�P�����̃^�C�v�́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_PLACE,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     20,  0 }, //�u�Ƃ����Ɓ@���Ȃ��@���݂����̂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_WANT,             QUESTION_ID_PRECIOUS,   QUESTION_ID_DUMMY,     30,  0 }, //�u�������ɂ��Ă�����̂Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 3, QUESTION_ID_PARTNER,          QUESTION_REQUIREMENT,   QUESTION_ID_CHARACTER, 30,  0 }, //�u�ЂƂ݂̂�傭�Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_SEASON,           QUESTION_ID_FOOD,       QUESTION_ID_DUMMY,     30,  0 }, //�u�����Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_COLOR,            QUESTION_ID_MUSIC,      QUESTION_ID_DUMMY,     30,  0 }, //�u��������Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_MUSIC,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     40,  0 }, //�u�����΂񂷂��ȁ@���񂪂��́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_TV,               QUESTION_ID_MOVIE,      QUESTION_ID_DUMMY,     40,  0 }, //�u���������Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_FAVORITE_CLASS,   QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     40,  0 }, //�u�����΂񂷂��ȁ@���������̂�����́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_STRONG_CLASS,     QUESTION_ID_WEAK_CLASS, QUESTION_ID_DUMMY,     40,  0 }, //�u�ׂ񂫂傤�Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_SPORTS,           QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     50,  0 }, //�u�����΂�ɂ񂫂̂���@�X�|�[�c�́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 2, QUESTION_ID_DAY_OFF,          QUESTION_ID_PLAY_SPOT,  QUESTION_ID_DUMMY,     50,  0 }, //�u���イ���Ɂ@���āv( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_DREAM,            QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,     50,  0 }, //�u�����΂�Ȃ肽�����̂́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_GYM_LEADER,       QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,    100,  0 }, //�u�����΂�ɂ񂫂̂���@�W�����[�_�[�́H�v( �l�� )
  { RESEARCH_REQ_TYPE_COUNT, 1, QUESTION_ID_POKEMON,          QUESTION_ID_DUMMY,      QUESTION_ID_DUMMY,    100,  0 }, //�u�|�P�����̂������낢�Ƃ���́H�v( �l�� )
};


//--------------------------------------------------------------------
/**
 * @brief �����^�C�v���擾����
 *
 * @param requestID �����˗�ID ( RESEARCH_REQ_ID_xxxx )
 *
 * @return �w�肵���˗�ID�̒����^�C�v ( RESEARCH_REQ_TYPE_xxxx )
 */
//--------------------------------------------------------------------
int RESEARCH_TEAM_GetResearchType( u8 requestID )
{
  GF_ASSERT( RESEARCH_REQ_ID_NONE < requestID );
  GF_ASSERT( requestID < RESEARCH_REQ_ID_NUM );

  return RequestData[ requestID ].type;
}

//--------------------------------------------------------------------
/**
 * @brief ���␔���擾����
 *
 * @param requestID [in] �����˗�ID ( RESEARCH_REQ_ID_xxxx )
 *
 * @return �w�肵���˗��ɊY�����鎿��̐�
 */
//--------------------------------------------------------------------
int RESEARCH_TEAM_GetQuestionNum( u8 requestID )
{
  GF_ASSERT( RESEARCH_REQ_ID_NONE < requestID );
  GF_ASSERT( requestID < RESEARCH_REQ_ID_NUM );

  return RequestData[ requestID ].QNum;
}

//--------------------------------------------------------------------
/**
 * @brief ����ID���擾����
 *
 * @param requestID [in] �����˗�ID ( RESEARCH_REQ_ID_xxxx )
 * @param destQID   [out]�w�肵�������˗��ɊY�����鎿��ID
 */
//--------------------------------------------------------------------
void RESEARCH_TEAM_GetQuestionID( u8 requestID, u8* destQID )
{
  GF_ASSERT( RESEARCH_REQ_ID_NONE < requestID );
  GF_ASSERT( requestID < RESEARCH_REQ_ID_NUM );

  destQID[0] = RequestData[ requestID ].QID1;
  destQID[1] = RequestData[ requestID ].QID2;
  destQID[2] = RequestData[ requestID ].QID3;
}

//--------------------------------------------------------------------
/**
 * @brief �m���}�l�����擾����
 */
//--------------------------------------------------------------------
int RESEARCH_TEAM_GetNormCount( u8 requestID )
{
  GF_ASSERT( RESEARCH_REQ_ID_NONE < requestID );
  GF_ASSERT( requestID < RESEARCH_REQ_ID_NUM );

  return RequestData[ requestID ].normCount;
}

//--------------------------------------------------------------------
/**
 * @brief �m���}���Ԃ��擾����
 */
//--------------------------------------------------------------------
int RESEARCH_TEAM_GetNormTime( u8 requestID )
{
  GF_ASSERT( RESEARCH_REQ_ID_NONE < requestID );
  GF_ASSERT( requestID < RESEARCH_REQ_ID_NUM );

  return RequestData[ requestID ].normTime;
} 
