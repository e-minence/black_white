/////////////////////////////////////////////////////////////////////
/**
 * @brief  ����Ⴂ������
 * @file   research_team.h
 * @author obata
 * @date   2010.03.31
 */
/////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>


// �����^�C�v���擾����
extern int RESEARCH_TEAM_GetResearchType( u8 requestID );
// ���␔���擾����
extern int RESEARCH_TEAM_GetQuestionNum( u8 requestID );
// ����ID���擾����
extern void RESEARCH_TEAM_GetQuestionID( u8 requestID, u8* destQID );
// �m���}�l�����擾����
extern int RESEARCH_TEAM_GetNormCount( u8 requestID );
// �m���}���Ԃ��擾����
extern int RESEARCH_TEAM_GetNormTime( u8 requestID );
