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
typedef struct _RESEARCH_RADAR_COMMON_WORK RRC_WORK;


//===============================================================================
// �������E�j��
//===============================================================================
// ����
extern RRC_WORK* RRC_CreateWork( HEAPID heapID, GAMESYS_WORK* gameSystem );
// �j��
extern void RRC_DeleteWork( RRC_WORK* work );


//===============================================================================
// ������
//===============================================================================
// �p���b�g�t�F�[�h ( �u���b�N�E�A�E�g ) ���J�n����
extern void RRC_StartPaletteFadeBlackOut( RRC_WORK* work );
// �p���b�g�t�F�[�h ( �u���b�N�E�C�� ) ���J�n����
extern void RRC_StartPaletteFadeBlackIn( RRC_WORK* work );

// �p���b�g�A�j���[�V�������X�V����
extern void RRC_UpdatePaletteAnime( RRC_WORK* work );
// �p���b�g�A�j���[�V�������J�n����
extern void RRC_StartPaletteAnime( 
    RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���[�V�������~���� ( �ʎw�� )
extern void RRC_StopPaletteAnime( 
    RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���[�V�������~���� ( �S�w�� )
extern void RRC_StopAllPaletteAnime( RRC_WORK* work );
// �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �ʎw�� )
extern void RRC_ResetPalette(
    RRC_WORK* work, COMMON_PALETTE_ANIME_INDEX index );
// �p���b�g�A�j���ő��삵���p���b�g�����Z�b�g���� ( �S�w�� )
extern void RRC_ResetAllPalette( RRC_WORK* work );


//===============================================================================
// ���擾
//===============================================================================
// �q�[�vID���擾����
extern HEAPID RRC_GetHeapID( const RRC_WORK* work );
// �Q�[���V�X�e�����擾����
extern GAMESYS_WORK* RRC_GetGameSystem( const RRC_WORK* work );
// �Q�[���f�[�^���擾����
extern GAMEDATA* RRC_GetGameData( const RRC_WORK* work );
// �Z���A�N�^�[���j�b�g���擾����
extern GFL_CLUNIT* RRC_GetClactUnit( 
    const RRC_WORK* work, COMMON_CLUNIT_INDEX unitIdx );
// �Z���A�N�^�[���[�N���擾����
extern GFL_CLWK* RRC_GetClactWork( 
    const RRC_WORK* work, COMMON_CLWK_INDEX workIdx );
// �p���b�g�t�F�[�h�V�X�e�����擾����
extern PALETTE_FADE_PTR RRC_GetPaletteFadeSystem( const RRC_WORK* work );
// �q�b�g�e�[�u�����擾����
extern const GFL_UI_TP_HITTBL* RRC_GetHitTable( const RRC_WORK* work );
// ���݂̉�ʂ��擾����
extern RADAR_SEQ RRC_GetNowSeq( const RRC_WORK* work );
// ���O�̉�ʂ��擾����
extern RADAR_SEQ RRC_GetPrevSeq( const RRC_WORK* work );
// ��ʑJ�ڂ̃g���K���擾����
extern SEQ_CHANGE_TRIG RRC_GetSeqChangeTrig( const RRC_WORK* work );
// �����I���t���O���擾����
extern BOOL RRC_GetForceReturnFlag( const RRC_WORK* work );

//===============================================================================
// ���ݒ�
//===============================================================================
// ���݂̉�ʂ�o�^����
extern void RRC_SetNowSeq( RRC_WORK* work, RADAR_SEQ seq );
// ��ʑJ�ڂ̃g���K��o�^����
extern void RRC_SetSeqChangeTrig( RRC_WORK* work, SEQ_CHANGE_TRIG trig );
// �����I���t���O�𗧂Ă�
extern void RRC_SetForceReturnFlag( RRC_WORK* work );
