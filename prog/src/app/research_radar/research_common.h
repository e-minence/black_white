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
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "system/palanm.h"         // for PaletteFadeXxxx


//===============================================================================
// �����ʃ��[�N�̕s���S�^
//===============================================================================
typedef struct _RESEARCH_COMMON_WORK RESEARCH_COMMON_WORK;


//===============================================================================
// �������E�j��
//===============================================================================
extern RESEARCH_COMMON_WORK* RESEARCH_COMMON_CreateWork( HEAPID heapID, GAMESYS_WORK* gameSystem ); // ����
extern void RESEARCH_COMMON_DeleteWork( RESEARCH_COMMON_WORK* work ); // �j��


//===============================================================================
// ������
//===============================================================================
extern void RESEARCH_COMMON_StartPaletteFadeBlackOut( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h ( �u���b�N�E�A�E�g ) ���J�n����
extern void RESEARCH_COMMON_StartPaletteFadeBlackIn( RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h ( �u���b�N�E�C�� ) ���J�n����


//===============================================================================
// ���擾
//===============================================================================
extern HEAPID RESEARCH_COMMON_GetHeapID( const RESEARCH_COMMON_WORK* work ); // �q�[�vID���擾����
extern GAMESYS_WORK* RESEARCH_COMMON_GetGameSystem( const RESEARCH_COMMON_WORK* work ); // �Q�[���V�X�e�����擾����
extern GAMEDATA* RESEARCH_COMMON_GetGameData( const RESEARCH_COMMON_WORK* work ); // �Q�[���f�[�^���擾����
extern GFL_CLUNIT* RESEARCH_COMMON_GetClactUnit( const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx ); // �Z���A�N�^�[���j�b�g���擾����
extern GFL_CLWK* RESEARCH_COMMON_GetClactWork( const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx ); // �Z���A�N�^�[���[�N���擾����
extern PALETTE_FADE_PTR RESEARCH_COMMON_GetPaletteFadeSystem( const RESEARCH_COMMON_WORK* work ); // �p���b�g�t�F�[�h�V�X�e�����擾����
extern const GFL_UI_TP_HITTBL* RESEARCH_COMMON_GetHitTable( const RESEARCH_COMMON_WORK* work ); // �q�b�g�e�[�u�����擾����
