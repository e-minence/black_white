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
typedef enum{
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
extern void PokeRegulation_LoadData(int regulation_data_no, REGULATION *reg);


//�֎~�t���O�̃r�b�g
#define POKEFAILEDBIT_NUM         (0x01)     ///< ��������ĂȂ�
#define POKEFAILEDBIT_LEVEL       (0x02)     ///< ���x���ᔽ�̃|�P����������
#define POKEFAILEDBIT_VETO_POKE     (0x04)   ///< �֎~�|�P����
#define POKEFAILEDBIT_BOTHPOKE    (0x08)    ///< �����|�P����
#define POKEFAILEDBIT_BOTHITEM    (0x10)    ///< �����A�C�e��
#define POKEFAILEDBIT_EGG         (0x20)    ///< ���܂�
#define POKEFAILEDBIT_VETO_ITEM         (0x40)   ///< �֎~�A�C�e��
#define POKEFAILEDBIT_MAST_POKE (0x80)   ///< �K�v�|�P���������Ȃ�

// �|�P���������M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ�  ����������������BIT�ŕԂ��܂�
extern BOOL PokeRegulationCheckPokeParaLookAt(const REGULATION* pReg, POKEMON_PARAM* pp,u32* FailedBit);
// �|�P�p�[�e�B�����M�����[�V�����ɓK�����Ă��邩�ǂ������ׂ� ����������������BIT�ŕԂ��܂�
extern int PokeRegulationMatchLookAtPokeParty(const REGULATION* pReg, POKEPARTY * party, u32* FailedBit);



