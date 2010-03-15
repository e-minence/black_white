////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �f�o�b�O���j���[�w���l���́x ( ����Ⴂ�����֘A )
 * @file   debug_numinput_research.c
 * @author obata
 * @date   2010.03.13
 */
//////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "gamesystem/game_data.h" // for GAMEDATA
#include "event_debug_numinput.h" // for DEBUG_NUMINPUT_XXXX

// @todo: �萔�̒�`�𐮗�
///����̃o���G�[�V������
#define QUESTIONNAIRE_ITEM_NUM       (30)
///�񓚂̃o���G�[�V������(���񓚂�����)
#define QUESTIONNAIRE_ANSWER_NUM     (150)

///�{���̒����l���ő吔
#define QUESTIONNAIRE_TODAY_COUNT_MAX   (999)
///���܂ł̒����l���ő吔
#define QUESTIONNAIRE_TOTAL_COUNT_MAX   (99999)

///���█�̖{���̒����l���ő吔
#define QUESTIONNAIRE_TODAY_ANSWER_COUNT_MAX   (99)
///���█�̍��܂ł̒����l���ő吔
#define QUESTIONNAIRE_TOTAL_ANSWER_COUNT_MAX   (9999)

//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� �����̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �擾���鎿���ID ( QUESTION_ID_xxxx )
 *
 * @return ����ɑ΂���, �����̉񓚐l��
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID ); 
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� �����̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �ݒ肷�鎿���ID ( QUESTION_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �擾���鎿���ID ( QUESTION_ID_xxxx )
 *
 * @return ����ɑ΂���, ���܂܂ł̉񓚐l��
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID ); 
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�����l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID �ݒ肷�鎿���ID ( QUESTION_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� �����̂���Ⴂ�񓚐l�� �� Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID �擾���鎿���ID ( ANSWER_ID_xxxx )
 *
 * @return ����ɑ΂���, �����̉񓚐l��
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID ); 
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� �����̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID �ݒ肷�鎿���ID ( ANSWER_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief ���l�擾�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID �擾����񓚂�ID ( ANSWER_ID_xxxx )
 *
 * @return ����ɑ΂���, ���܂܂ł̉񓚐l��
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID ); 
//------------------------------------------------------------------------------
/**
 * @brief ���l�ݒ�֐� ( ���l���� �� ���܂܂ł̂���Ⴂ�񓚐l�� �� Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   �ݒ肷��񓚂�ID ( ANSWER_ID_xxxx )
 * @param count      �ݒ肷�鐔�l
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count );
