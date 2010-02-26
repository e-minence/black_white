/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �������[�_�[ ���ʃw�b�_
 * @file   research_common.h
 * @author obata
 * @date   2010.02.06
 */
///////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "research_common_index.h"
#include "gamesystem/gamesystem.h"  // for GAMESYS_WORK


//=============================================================================== 
// ���萔
//=============================================================================== 
#define PRINT_TARGET (2)  // �f�o�b�O�o�͐�

#define DOT_PER_CHARA     (8)                      // 1�L���� = 8�h�b�g
#define ONE_CHARA_SIZE    (0x20)                   // 1�L�����N�^�[�f�[�^�̃T�C�Y [Byte]
#define ONE_PALETTE_SIZE  (0x20)                   // 1�p���b�g(16�F��)�̃T�C�Y [Byte]
#define FULL_PALETTE_SIZE (ONE_PALETTE_SIZE * 16)  // 16�p���b�g�̃T�C�Y [Byte]

//----------
// ��SUB-BG
//----------
// �w�i��
#define SUB_BG_BACK (GFL_BG_FRAME0_S)  // BG �t���[��
#define SUB_BG_BACK_PRIORITY      (3)  // �\���D�揇��
#define SUB_BG_BACK_FIRST_PLT_IDX (0)  // �擪�p���b�g�ԍ�
#define SUB_BG_BACK_PLT_NUM       (1)  // �g�p�p���b�g��
// ���[�_�[��
#define SUB_BG_RADAR (GFL_BG_FRAME1_S)    // BG �t���[��
#define SUB_BG_RADAR_PRIORITY        (2)  // �\���D�揇��
#define SUB_BG_RADAR_FIRST_PLT_IDX   (0)  // �擪�p���b�g�ԍ�
#define SUB_BG_RADAR_PLT_NUM         (6)  // �g�p�p���b�g��
#define SUB_BG_RADAR_PLT_ANIME_FRAME (10) // �p���b�g�A�j���[�V�����̍X�V�Ԋu[frame]

//-----------
// ��MAIN-BG
//-----------
// �w�i��
#define MAIN_BG_BACK (GFL_BG_FRAME1_M)  // �w�iBG��
#define MAIN_BG_BACK_PRIORITY      (3)  // �w�iBG�ʂ̕\���D�揇��
#define MAIN_BG_BACK_FIRST_PLT_IDX (0)  // �w�iBG�ʂ̐擪�p���b�g�ԍ�
#define MAIN_BG_BACK_PLT_NUM       (1)  // �w�iBG�ʂ̃p���b�g��


// VRAM-Bank �ݒ�
extern const GFL_DISP_VRAM VRAMBankSettings;

// BG���[�h�ݒ�
extern const GFL_BG_SYS_HEADER BGSysHeader2D;
extern const GFL_BG_SYS_HEADER BGSysHeader3D;

// �Z���A�N�^�[�V�X�e��
extern const GFL_CLSYS_INIT ClactSystemInitData;

// ���ʃ��[�N
typedef struct _RESEARCH_COMMON_WORK RESEARCH_COMMON_WORK;

// ���ʃ��[�N �����E�j��
RESEARCH_COMMON_WORK* RESEARCH_COMMON_CreateWork( HEAPID heapID, GAMESYS_WORK* gameSystem );
void RESEARCH_COMMON_DeleteWork( RESEARCH_COMMON_WORK* work ); 

// ���ʃ��[�N ����OBJ �����E��Еt��
void RESEARCH_COMMON_SetupCommonOBJ( RESEARCH_COMMON_WORK* work );
void RESEARCH_COMMON_CleanUpCommonOBJ( RESEARCH_COMMON_WORK* work );

// ���ʃ��[�N �擾
HEAPID RESEARCH_COMMON_GetHeapID( const RESEARCH_COMMON_WORK* work );
GAMESYS_WORK* RESEARCH_COMMON_GetGameSystem( const RESEARCH_COMMON_WORK* work );
GAMEDATA* RESEARCH_COMMON_GetGameData( const RESEARCH_COMMON_WORK* work );
GFL_CLUNIT* RESEARCH_COMMON_GetClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx );
GFL_CLWK* RESEARCH_COMMON_GetClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx );
