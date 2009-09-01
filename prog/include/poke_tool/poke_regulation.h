//============================================================================================
/**
 * @file	poke_regulation.h
 * @brief	���M�����[�V���������p�v���O����
 * @author	k.ohno
 * @date	2006.5.25
 */
//============================================================================================
#pragma once

#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "savedata/regulation.h"

#include "regulation_def.h"


// PokeRegulationMatchPartialPokeParty��PokeRegulationMatchFullPokeParty �̖߂�l
enum{
    POKE_REG_OK,               // ����
    POKE_REG_TOTAL_LV_FAILED,  // �g�[�^����LV���I�[�o�[���Ă���
    POKE_REG_BOTH_POKE,        // �����|�P����������
    POKE_REG_BOTH_ITEM,        // �����A�C�e��������
//--------------------------------------------------------
    POKE_REG_NUM_FAILED,       // �K�v�ȃ|�P�����̐l������������Ă��Ȃ�
    POKE_REG_ILLEGAL_POKE,     // �����ɍ���Ȃ��|�P����������
    POKE_REG_NUM_FAILED2,       // �K�v�ȃ|�P�����̐l������������Ă��Ȃ�2
    POKE_REG_NO_MASTPOKE,
} POKE_REG_RETURN_ENUM;

// �|�P���������M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�  
extern BOOL PokeRegulationCheckPokePara(const REGULATION* pReg, POKEMON_PARAM* pp);
// �|�P�p�[�e�B�����M�����[�V�����Ɋ��S�K�����Ă��邩�ǂ������ׂ� �o�g���ŏI�`�F�b�N�p
extern int PokeRegulationMatchFullPokeParty(const REGULATION* pReg, POKEPARTY * party, u8* sel);
// �|�P�p�[�e�B���Ƀ��M�����[�V�����ɓK�����Ă���
//  �p�[�e�B�[���g�߂邩�ǂ������ׂ� �K���O�̃|�P���������Ă����v  �󂯕t���p
extern int PokeRegulationMatchPartialPokeParty(const REGULATION* pReg, POKEPARTY * party);

// regulation.narc�ɂ���f�[�^��ǂݍ���
extern const REGULATION* PokeRegulation_LoadDataAlloc(int regulation_data_no, HEAPID heapid);
