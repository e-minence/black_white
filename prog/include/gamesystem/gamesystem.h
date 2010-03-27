//============================================================================================
/**
 * @file	gamemsystem.h
 * @brief	�|�P�����Q�[���V�X�e���i�������ʏ�Q�[���̃g�b�v���x���j
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.21
 *
 */
//============================================================================================

#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_comm.h"
#include "gamesystem/iss_sys.h"
#include "field/fieldmap_proc.h"  //FIELDMAP_WORK

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct _GAMESYS_WORK GAMESYS_WORK;

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct _GMEVENT_CONTROL GMEVENT;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[�����C���v���Z�X��`�f�[�^
 */
//------------------------------------------------------------------
extern const GFL_PROC_DATA GameMainProcData;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[�����v���Z�X�̌Ăяo���i����j
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @param	ov_id		�I�[�o�[���C�w��ID
 * @param	procdata	�v���Z�X�f�[�^�ւ̃|�C���^
 * @param	pwk			��ʃv���Z�X����n�����[�N
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
/**
 * @brief	�Q�[�����v���Z�X�̌Ăяo���i�R�[���j
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @param	ov_id		�I�[�o�[���C�w��ID
 * @param	procdata	�v���Z�X�f�[�^�ւ̃|�C���^
 * @param	pwk			��ʃv���Z�X����n�����[�N
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v�v���Z�X�̌Ăяo��
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_CallFieldProc(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�̎擾
 * @param	gsys			�Q�[�����䃏�[�N�ւ̃|�C���^
 * @return	GMEVENT			�C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GMEVENT * GAMESYSTEM_GetEvent(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�C�x���g�̃Z�b�g
 * @param	gsys	�Q�[�����䃏�[�N�ւ̃|�C���^
 * @param	event	�C�x���g�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT * event);

//------------------------------------------------------------------
/**
 * @brief	�v���Z�X�N�������ǂ�����Ԃ�
 * @param	gsys	�Q�[�����䃏�[�N�ւ̃|�C���^
 * @return	GFL_PROC_MAIN_STATUS
 */
//------------------------------------------------------------------
extern GFL_PROC_MAIN_STATUS GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�Q�[���V�X�e���p�q�[�vID�̎擾
 * @param	gsys	�Q�[�����䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern HEAPID GAMESYSTEM_GetHeapID(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^�擾
 * @param	gsys	�Q�[�����䃏�[�N�ւ̃|�C���^
 * @return	void*	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 *
 * GAMESYS_WORK����FIELD_MAIN_WORK���擾�ł��邱�Ƃɂ�
 * ����肪����i�O���[�o���ϐ��ɋ߂��j���A���ɕp�ɂ�
 * �Z�b�g�ŃA�N�Z�X����邽�߁A���֐����l�����Ēǉ��B
 */
//------------------------------------------------------------------
extern FIELDMAP_WORK * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v���[�N�̃|�C���^�Z�b�g
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�}�b�v���[�N�ւ̃|�C���^
 *
 * �t�B�[���h�}�b�v�𐶐����镔���ȊO����̃A�N�Z�X�͖����͂��I
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�}�b�v���[�N�̃|�C���^�`�F�b�N
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @retval BOOL TRUE=�t�B�[���h�}�b�v���[�N���Z�b�g����Ă���B
 */
//------------------------------------------------------------------
extern BOOL GAMESYSTEM_CheckFieldMapWork( const GAMESYS_WORK *gsys );

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�ݒ�
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @param   error_req		TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
extern void GAMESYSTEM_SetFieldCommErrorReq( GAMESYS_WORK *gsys, BOOL error_req );

//==================================================================
/**
 * �t�B�[���h�ł̒ʐM�G���[��ʕ\�����N�G�X�g�擾
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @retval  TRUE:�G���[��ʕ\���B�@FALSE:���N�G�X�g���N���A
 */
//==================================================================
extern BOOL GAMESYSTEM_GetFieldCommErrorReq( const GAMESYS_WORK *gsys );

//--------------------------------------------------------------
//	ISS�V�X�e���擾
//--------------------------------------------------------------
extern ISS_SYS * GAMESYSTEM_GetIssSystem( GAMESYS_WORK * gsys );

//------------------------------------------------------------------
/**
 * @brief	�Q�[���f�[�^�̎擾
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @return	GAMEDATA	�Q�[���f�[�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	�����v���C���[�f�[�^�̎擾
 * @param	gsys		�Q�[�����䃏�[�N�ւ̃|�C���^
 * @return	PLAYER_WORK	�����v���C���[�f�[�^�ւ̃|�C���^
 *
 * ���͗��֐��̂���GAMESYS_WORK���璼�ڎ擾�ɂ��Ă��邪
 * �݌v���GAMESYS_WORK��GAMEDATA��PLAYER_WORK�ɂ���ׂ�����
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys);

//==================================================================
/**
 * �Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^���擾����
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @retval  GAME_COMM_SYS_PTR		�Q�[���ʐM�Ǘ����[�N�ւ̃|�C���^
 */
//==================================================================
extern GAME_COMM_SYS_PTR GAMESYSTEM_GetGameCommSysPtr(GAMESYS_WORK *gsys);

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̎擾
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @retval  TRUE:�펞�ʐMON�A�@FALSE:�펞�ʐMOFF
 */
//--------------------------------------------------------------
extern BOOL GAMESYSTEM_GetAlwaysNetFlag(const GAMESYS_WORK * gsys);

//--------------------------------------------------------------
/**
 * @brief   �펞�ʐM�t���O�̐ݒ�
 * @param   gsys		�Q�[������V�X�e���ւ̃|�C���^
 * @param	is_on			TRUE�Ȃ�Ώ펞�ʐM���[�hON FALSE�Ȃ�Ώ펞�ʐM���[�hOFF
 */
//--------------------------------------------------------------
extern void GAMESYSTEM_SetAlwaysNetFlag( GAMESYS_WORK * gsys, BOOL is_on );

//==================================================================
/**
 * �펞�ʐM�t���O���`�F�b�N������ŁA�펞�ʐM���N������
 *
 * @param   gsys		
 */
//==================================================================
extern void GAMESYSTEM_CommBootAlways( GAMESYS_WORK *gsys );

#ifdef PM_DEBUG
extern GAMESYS_WORK * DEBUG_GameSysWorkPtrGet(void);
#endif

#ifdef	__cplusplus
} /* extern "C" */
#endif
