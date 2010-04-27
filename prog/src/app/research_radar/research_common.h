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
#include "research_common_def.h"
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "system/palanm.h"         // for PaletteFadeXxxx


//===============================================================================
// �����ʃ��[�N�̕s���S�^
//===============================================================================
typedef struct _RESEARCH_COMMON_WORK RESEARCH_COMMON_WORK;


//===============================================================================
// �������E�j��
//===============================================================================
// ����
extern RESEARCH_COMMON_WORK* RESEARCH_COMMON_CreateWork( 
    HEAPID heapID, GAMESYS_WORK* gameSystem );
// �j��
extern void RESEARCH_COMMON_DeleteWork( RESEARCH_COMMON_WORK* work );


//===============================================================================
// ������
//===============================================================================
// �p���b�g�t�F�[�h ( �u���b�N�E�A�E�g ) ���J�n����
extern void RESEARCH_COMMON_StartPaletteFadeBlackOut( RESEARCH_COMMON_WORK* work );
// �p���b�g�t�F�[�h ( �u���b�N�E�C�� ) ���J�n����
extern void RESEARCH_COMMON_StartPaletteFadeBlackIn( RESEARCH_COMMON_WORK* work );

// �p���b�g�A�j���[�V�������X�V����
extern void RESEARCH_COMMON_UpdatePaletteAnime( RESEARCH_COMMON_WORK* work );
// �p���b�g�A�j���[�V�������J�n����
extern void RESEARCH_COMMON_StartPaletteAnime( 
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���[�V�������~���� ( �ʎw�� )
extern void RESEARCH_COMMON_StopPaletteAnime( 
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���[�V�������~���� ( �S�w�� )
extern void RESEARCH_COMMON_StopAllPaletteAnime( RESEARCH_COMMON_WORK* work );
// �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �ʎw�� )
extern void RESEARCH_COMMON_ResetPalette(
    RESEARCH_COMMON_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �S�w�� )
extern void RESEARCH_COMMON_ResetAllPalette( RESEARCH_COMMON_WORK* work );


//===============================================================================
// ���擾
//===============================================================================
// �q�[�vID���擾����
extern HEAPID RESEARCH_COMMON_GetHeapID( const RESEARCH_COMMON_WORK* work );
// �Q�[���V�X�e�����擾����
extern GAMESYS_WORK* RESEARCH_COMMON_GetGameSystem( const RESEARCH_COMMON_WORK* work );
// �Q�[���f�[�^���擾����
extern GAMEDATA* RESEARCH_COMMON_GetGameData( const RESEARCH_COMMON_WORK* work );
// �Z���A�N�^�[���j�b�g���擾����
extern GFL_CLUNIT* RESEARCH_COMMON_GetClactUnit( 
    const RESEARCH_COMMON_WORK* work, COMMON_CLUNIT_INDEX unitIdx );
// �Z���A�N�^�[���[�N���擾����
extern GFL_CLWK* RESEARCH_COMMON_GetClactWork( 
    const RESEARCH_COMMON_WORK* work, COMMON_CLWK_INDEX workIdx );
// �p���b�g�t�F�[�h�V�X�e�����擾����
extern PALETTE_FADE_PTR RESEARCH_COMMON_GetPaletteFadeSystem( const RESEARCH_COMMON_WORK* work );
// �q�b�g�e�[�u�����擾����
extern const GFL_UI_TP_HITTBL* RESEARCH_COMMON_GetHitTable( const RESEARCH_COMMON_WORK* work );
// ���݂̉�ʂ��擾����
extern RADAR_SEQ RESEARCH_COMMON_GetNowSeq( const RESEARCH_COMMON_WORK* work );
// ���O�̉�ʂ��擾����
extern RADAR_SEQ RESEARCH_COMMON_GetPrevSeq( const RESEARCH_COMMON_WORK* work );
// ��ʑJ�ڂ̃g���K���擾����
extern SEQ_CHANGE_TRIG RESEARCH_COMMON_GetSeqChangeTrig( const RESEARCH_COMMON_WORK* work );

//===============================================================================
// ���ݒ�
//===============================================================================
// ���݂̉�ʂ�o�^����
extern void RESEARCH_COMMON_SetNowSeq( RESEARCH_COMMON_WORK* work, RADAR_SEQ seq );
// ��ʑJ�ڂ̃g���K��o�^����
extern void RESEARCH_COMMON_SetSeqChangeTrig( RESEARCH_COMMON_WORK* work, SEQ_CHANGE_TRIG trig );
