/**
 *  @file mail_util.h
 *  @brief  ���[���Z�[�u�f�[�^�Q�Ɨp�s���S�^��`�����[�e�B���e�B�֐���`
 *          GAMEDATA�\���̂�Z�[�u�f�[�^�ɃA�N�Z�X����ۂ̊֐����܂Ƃ߂Ă��܂��B
 *          ���[���f�[�^�𑀍삵�����ꍇ��mail.h�ŁB
 *          
 *  @author AkitoMori / MiyukiIwasawa
 *  @date 09.11.16
 */

#pragma once

///�Q�ƒ�`
#include "gamesystem/game_data.h"
#include "poke_tool/poke_tool.h"
#include "mail.h"

//------------------------------------------------------------
/**
 * @brief ���[���Z�[�u�f�[�^�u���b�N�Ǘ��\���ւ̕s���S�^�|�C���^
 *
 * ���g�͌����Ȃ����ǃ|�C���^�o�R�ŎQ�Ƃ͂ł���
 */
//------------------------------------------------------------
typedef struct _MAIL_BLOCK MAIL_BLOCK;



//=============================================================
/**
 *  ���[���Z�[�u�f�[�^�u���b�N�A�N�Z�X�n�֐�
 */
//=============================================================
//
/**
 *  @brief  �Z�[�u�f�[�^�u���b�N�ւ̃|�C���^���擾
 */
//extern MAIL_BLOCK* SaveData_GetMailBlock(SAVE_CONTROL_WORK* sv);
extern MAIL_BLOCK* GAMEDATA_GetMailBlock( GAMEDATA *gamedata);

/**
 *  @brief  ���[���Z�[�u�f�[�^�u���b�N�T�C�Y�擾
 *
 *  �����[���f�[�^��ʂ̃T�C�Y�ł͂Ȃ��̂Œ��ӁI
 */
extern int MAIL_GetBlockWorkSize(void);

/**
 *  @brief  ���[���Z�[�u�f�[�^�u���b�N������
 */
extern void MAIL_Init(MAIL_BLOCK* dat);

/**
 *  @brief  �󂢂Ă��郁�[���f�[�^ID���擾
 *
 *  @param  id �ǉ����������[���u���b�NID
 *
 *  @return int �f�[�^��ǉ��ł���ꍇ�͎Q��ID
 *        �ǉ��ł��Ȃ��ꍇ�̓}�C�i�X�l���Ԃ�
 */
extern int MAIL_SearchNullID(MAIL_BLOCK* block,MAILBLOCK_ID id);

/**
 *  @brief  ���[���f�[�^���폜
 *
 *  @param  blockID �u���b�N��ID
 *  @param  dataID  �f�[�^ID
 */
extern void MAIL_DelMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID);

/**
 *  @brief  ���[���f�[�^���Z�[�u�u���b�N�ɒǉ�
 *
 *  �������n����MAIL_DATA�\���̌^�f�[�^�̒��g���Z�[�u�f�[�^�ɔ��f�����̂�
 *  �@�������ȃf�[�^�����Ȃ��悤�ɒ��ӁI
 */
extern void MAIL_AddMailFormWork(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* src);

/**
 *  @brief  �w��u���b�N�ɗL���f�[�^���������邩�Ԃ�
 */
extern int MAIL_GetEnableDataNum(MAIL_BLOCK* block,MAILBLOCK_ID blockID);

/**
 *  @brief  ���[���f�[�^�̃R�s�[���擾
 *
 *  �������Ń��������m�ۂ���̂ŁA�Ăяo�������ӔC�����ė̈���J�����邱��
 */
extern MAIL_DATA* MAIL_AllocMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,HEAPID heapID);

/**
 *  @brief  ���[���f�[�^�̃R�s�[���擾
 *
 *  �����炩���ߊm�ۂ���MAIL_DATA�^�������ɃZ�[�u�f�[�^���R�s�[���Ď擾
 */
extern void MAIL_GetMailData(MAIL_BLOCK* block,MAILBLOCK_ID blockID,int dataID,MAIL_DATA* dest);


//=============================================================
/**
 *  �O���A�����[���f�[�^�֘A
 */
//=============================================================

/**
 *  @brief  �f�U�C��No,��������|�P�����̃|�W�V�����A�Z�[�u�f�[�^���w�肵��
 *      ���[���f�[�^��V�K�쐬��Ԃɏ�����
 *  @param  dat �f�[�^���쐬����MAIL_DATA�\���̌^�ւ̃|�C���^
 *  @param  design_no ���[���̃C���[�WNo
 *  @param  pos   ���[������������|�P�����̎莝�����̃|�W�V����
 *  @param  save  �Z�[�u�f�[�^�ւ̃|�C���^
 */
extern void MAILDATA_CreateFromSaveData( MAIL_DATA* dat,u8 design_no,u8 pos,GAMEDATA *gamedata );




