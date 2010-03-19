//======================================================================
/**
 * @file  scr_seq_def.h
 * @bfief  �X�N���v�g�R�}���h�̃}�N����`�t�@�C��
 * @author  GAME FREAK inc.
 *
 */
//======================================================================

//�X�N���v�g�Ŏg�p����w�b�_�[�t�@�C��
  .include  "usescript.h"

//�f�[�^�����R�ȋ��E�ɃA���C�����邩�̐ݒ�
  .option alignment off

//�X�N���v�g�R�}���h�e�[�u���p�}�N���̒�`
  .include "scrcmd_table/def_table_macro.h"
//�X�N���v�g��`�e�[�u��
  .include "../../../prog/src/field/scrcmd_table.cdat"

//======================================================================
// �C�x���g�f�[�^�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _EVENT_DATA
 *  @brief  �C�x���g�f�[�^�@�e�[�u���錾
 *  @param adrs �錾����f�[�^���x���A�h���X
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA  adrs
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  @def  _EVENT_DATA_END
 *  @brief  �C�x���g�f�[�^�@�e�[�u���錾�I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA_END
  .short  EV_DATA_END_CODE
  .endm

//--------------------------------------------------------------
/**
 *  @def  _SP_EVENT_DATA_END
 *  @brief  ����X�N���v�g�I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _SP_EVENT_DATA_END
  .short   SP_SCRID_NONE
  .endm


//=============================================================================
/**
 *	����X�N���v�g�֘A
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief ����X�N���v�g�o�^�F�V�[�������C�x���g�̓o�^
 * @param adrs  �V�[�������C�x���g�e�[�u���̃A�h���X
 *
 * @note
 * �V�[���X�N���v�g�i�t�B�[���h�Ń��[�N�̒l���w��l�̏ꍇ�ɋ�����������X�N���v�g�j��
 * �o�^����
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_LABEL	adrs
	.short	SP_SCRID_SCENE_CHANGE
	.long	((\adrs-.)-4)
	.endm

//--------------------------------------------------------------
/**
 * @brief �V�[�������C�x���g�̗v�f�L�q
 * @param wk      �Q�Ƃ��郏�[�N�w��ID
 * @param num     ��L���[�N�̔��������Ƃ���l
 * @param scr_id  �X�N���v�g���w�肷��ID
 *
	�N�������L�q
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_DATA		wk,num,scr_id
	.short	\wk
	.short	\num
	.short	\scr_id
	.endm

//--------------------------------------------------------------
/**
 * @brief �V�[�������C�x���g�̗v�f�I�[
 *
 * _SCENE_CHANGE_LABEL�Ŏw�肵���V�[�������C�x���g�e�[�u���̍Ō�ɂ�
 * �K��_SCENE_CHANGE_END���L�q����K�v������
 */
//--------------------------------------------------------------
	.macro	_SCENE_CHANGE_END
	.short	0
	.endm

//--------------------------------------------------------------
/**
 *	@brief  ����X�N���v�g�o�^�F�]�[���؂�ւ�����
 *	@param  scr_id  �o�^����X�N���v�gID
 *
 *	@note
 *	�]�[���؂�ւ��̃^�C�~���O�ŌĂ΂�܂��B
 *	�����ă}�b�v���܂������^�C�~���O�A
 *	�o�����E������ԂȂǃt�B�[���h�}�b�v���Ȃ��^�C�~���O
 *	�̗����ŌĂ΂�܂��B
 *	��҂̍ۂ́A�g�p�ł���R�}���h�̐����ɋC�����邱�ƁI
 */
//--------------------------------------------------------------
	.macro	_ZONE_CHANGE_LABEL	scr_id
	.short	SP_SCRID_ZONE_CHANGE
	.short	\scr_id
	.short	0		//4byte�ɍ��킹�邽�߂Ƀ_�~�[
	.endm

//--------------------------------------------------------------
/**
 *	@brief  ����X�N���v�g�o�^�F�}�b�v���񏉊�������
 *	@param  scr_id  �o�^����X�N���v�gID
 *	@note
 *	�}�b�v���ł��āA�n�a�i���z�u���ꂽ���ƌĂ΂��B
 *	���̃}�b�v�ɓ������Ƃ��ɌĂ΂�܂��B
 */
//--------------------------------------------------------------
	.macro	_FIELD_INIT_LABEL	scr_id
	.short	SP_SCRID_FIELD_INIT
	.short	\scr_id
	.short	0		//4byte�ɍ��킹�邽�߂Ƀ_�~�[
	.endm

//--------------------------------------------------------------
/**
 *	@brief  ����X�N���v�g�o�^�F�}�b�v���A����������
 *	@param  scr_id  �o�^����X�N���v�gID
 *	@note
 *	�}�b�v���ł��āA�n�a�i���z�u���ꂽ���ƌĂ΂��B
 *	�}�b�v���A�����i���j���[��o�g������̕��A�j�̂��тɂ�΂�܂��B
 */
//--------------------------------------------------------------
	.macro	_FIELD_RECOVER_LABEL	scr_id
	.short	SP_SCRID_FIELD_RECOVER
	.short	\scr_id
	.short	0		//4byte�ɍ��킹�邽�߂Ƀ_�~�[
	.endm


//======================================================================
//
//
//  �C�x���g�J�n�E�I��
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �ʏ�C�x���g�J�n�錾
 */
//--------------------------------------------------------------
  .macro  EVENT_START label
\label:
  .short  EV_SEQ_COMMON_PROC_FIELD_EVENT_START
  .endm

//--------------------------------------------------------------
/**
 * �ʏ�C�x���g�I���錾
 */
//--------------------------------------------------------------
  .macro  EVENT_END
  .short  EV_SEQ_CHECK_SCR_END
  .short  EV_SEQ_COMMON_PROC_FIELD_EVENT_END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  INIT_EVENT_START  label
\label:
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  INIT_EVENT_END
  .short  EV_SEQ_END
  .endm


//======================================================================
//  ��{�R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _NOP
 *  @brief  �������Ȃ�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _NOP
  .short  EV_SEQ_NOP
  .endm

//--------------------------------------------------------------
/**
 *  @def  _DUMMY
 *  @brief  �_�~�[
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _DUMMY
  .short  EV_SEQ_DUMMY
  .endm

//--------------------------------------------------------------
/**
 *  @def  _END
 *  @brief  �X�N���v�g�̏I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TIME_WAIT
 *  @brief  �E�F�C�g
 *  @param time �҂��t���[����
 */
//--------------------------------------------------------------
#define _TIME_WAIT( time ) _ASM_TIME_WAIT time

  .macro  _ASM_TIME_WAIT time, ret_wk
  .short  EV_SEQ_TIME_WAIT
  .short  \time
  .endm


//--------------------------------------------------------------
/**
 *  _CALL �R�[��
 *  @param adrs �Ăяo�����x���A�h���X
 */
//--------------------------------------------------------------
  .macro  _CALL adrs
  .short  EV_SEQ_CALL
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _RET �R�[�����Ăяo�����A�h���X�ɖ߂�
 */
//--------------------------------------------------------------
  .macro  _RET
  .short  EV_SEQ_RET
  .endm

//--------------------------------------------------------------
/**
 * @def _DEBUG_PRINT_WK
 * @brief ���[�N�̒l���o�͂���
 * @param work �l���o�͂��郏�[�N
 */
//--------------------------------------------------------------
#define _DEBUG_PRINT_WK( work ) \
    _ASM_DEBUG_PRINT_WK  work

  .macro  _ASM_DEBUG_PRINT_WK work
  .short  EV_SEQ_DEBUG_PRINT_WK
  .short  \work
  .endm

//======================================================================
//    ���艉�Z�p����
//  ���X�N���v�g�R���p�C�����g�p���Ă��܂��B
//  �@�X�N���v�g�S���҂͎g�p���Ȃ��ŉ������B
//======================================================================
//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  _CMP_STACK  cond
  .short  EV_SEQ_CMP_STACK
  .short   \cond
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_WORK  wk
  .short  EV_SEQ_PUSH_WORK
  .short  \wk
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_VALUE  value
  .short  EV_SEQ_PUSH_VALUE
  .short  \value
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _PUSH_FLAG  flag
  .short  EV_SEQ_PUSH_FLAG
  .short  \flag
  .endm
//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _POP_WORK  wk
  .short  EV_SEQ_POP_WORK
  .short  \wk
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _POP
  .short  EV_SEQ_POP
  .endm


//======================================================================
//  �f�[�^���[�h�E�X�g�A�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * _LD_REG_VAL ���z�}�V���̔ėp���W�X�^��1byte�̒l���i�[
 * @param r �l���i�[���郌�W�X�^
 * @param val �i�[����l
 */
//--------------------------------------------------------------
  .macro  _LD_REG_VAL  r,val
  .short  EV_SEQ_LD_REG_VAL
  .byte  \r
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_WDATA ���z�}�V���̔ėp���W�X�^��4byte�̒l���i�[
 * @param r �l���i�[���郌�W�X�^
 * @param val �i�[����l
 */
//--------------------------------------------------------------
  .macro  _LD_REG_WDATA  r,wdata
  .short  EV_SEQ_LD_REG_WDATA
  .byte  \r
  .long  \wdata
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_ADR ���z�}�V���̔ėp���W�X�^�ɃA�h���X���i�[
 * @param reg �A�h���X���i�[���郌�W�X�^
 * @@aram ADDRESS �i�[����A�h���X
 */
//--------------------------------------------------------------
  .macro  _LD_REG_ADR  reg,ADDRESS
  .short  EV_SEQ_LD_REG_ADR
  .byte  \reg
  .long  \ADDRESS
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_VAL �A�h���X�̒��g�ɒl����
 * @param adrs �l��������A�h���X
 * @param val �������l
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_VAL  adrs,val
  .short  EV_SEQ_LD_ADR_VAL
  .long  \adrs
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_REG �A�h���X�̒��g�ɉ��z�}�V���̔ėp���W�X�^�̒l����
 * @param adrs �l��������A�h���X
 * @param val �������l���i�[���ꂽ���W�X�^
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_REG  adrs,reg
  .short  EV_SEQ_LD_ADR_REG
  .long  \adrs
  .byte  \reg
  .endm

//--------------------------------------------------------------
/**
 * _LD_REG_REG ���z�}�V���̔ėp���W�X�^�̒l��ėp���W�X�^�ɃR�s�[
 * @param r1 �R�s�[�惌�W�X�^
 * @param r2 �R�s�[�����W�X�^
 */
//--------------------------------------------------------------
  .macro  _LD_REG_REG  r1,r2
  .short  EV_SEQ_LD_REG_REG
  .byte  \r1,\r2
  .endm

//--------------------------------------------------------------
/**
 * _LD_ADR_ADR �A�h���X�̒��g�ɃA�h���X�̒��g����
 * @param r1 �����A�h���X
 * @param r2 ������A�h���X
 */
//--------------------------------------------------------------
  .macro  _LD_ADR_ADR  adr1,adr2
  .short  EV_SEQ_LD_ADR_ADR
  .long  \adr1
  .long  \adr2
  .endm

//======================================================================
//  ��r����
//======================================================================
//--------------------------------------------------------------
/**
 *  _CP_REG_REG ���z�}�V���̔ėp���W�X�^���r
 *  @param r1 ��r���郌�W�X�^
 *  @param r2 r1�Ɣ�r���郌�W�X�^
 */
//--------------------------------------------------------------
  .macro  _CP_REG_REG  r1,r2
  .short  EV_SEQ_CP_REG_REG
  .byte  \r1,\r2
  .endm

//--------------------------------------------------------------
/**
 *  _CP_REG_VAL ���z�}�V���̔ėp���W�X�^�ƒl���r
 *  @param r1 ��r���郌�W�X�^
 *  @param val ��r����l
 */
//--------------------------------------------------------------
  .macro  _CP_REG_VAL  r1,val
  .short  EV_SEQ_CP_REG_VAL
  .byte  \r1,\val
  .endm

//--------------------------------------------------------------
/**
 *  _CP_REG_ADR ���z�}�V���̔ėp���W�X�^�ƃA�h���X�̒��g���r
 *  @param r1 ��r���郌�W�X�^
 *  @param val ��r����l
 */
//--------------------------------------------------------------
  .macro  _CP_REG_ADR  r1,adrs
  .short  EV_SEQ_CP_REG_ADR
  .byte  \r1
  .long  \adrs
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_REG �A�h���X�̒��g�Ɖ��z�}�V���̔ėp���W�X�^���r
 *  @param adrs ��r����A�h���X
 *  @param r1 ��r���郌�W�X�^
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_REG  adrs,r1
  .short  EV_SEQ_CP_ADR_REG
  .long  \adrs
  .byte  \r1
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_VAL �A�h���X�̒��g�ƒl���r
 *  @param adrs ��r����A�h���X
 *  @param val ��r����l
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_VAL  adrs,val
  .short  EV_SEQ_CP_ADR_VAL
  .long  \adrs
  .byte  \val
  .endm

//--------------------------------------------------------------
/**
 *  _CP_ADR_ADR �A�h���X�̒��g�ƃA�h���X�̒��g���r
 *  @param adr1 ��r����A�h���X����1
 *  @param adr2 ��r����A�h���X����2
 */
//--------------------------------------------------------------
  .macro  _CP_ADR_ADR  adr1,adr2
  .short  EV_SEQ_CP_ADR_ADR
  .long  \adr1
  .long  \adr2
  .endm

//--------------------------------------------------------------
/**
 *  _CMPVAL ���[�N�ƒl�̔�r
 *  @param wk ��r���郏�[�N
 *  @param val ��r����l
 */
//--------------------------------------------------------------
  .macro  _CMPVAL  wk,val
  .short  EV_SEQ_CP_WK_VAL
  .short  \wk
  .short  \val
  .endm

//--------------------------------------------------------------
/**
 *  _CMPWK ���[�N�ƃ��[�N�̔�r
 *  @param ��r���郏�[�N����1
 *  @param ��r���郏�[�N����2
 */
//--------------------------------------------------------------
  .macro  _CMPWK  wk1,wk2
  .short  EV_SEQ_CP_WK_WK
  .short  \wk1
  .short  \wk2
  .endm

#if 0 //wb null
//--------------------------------------------------------------
/**
 * �f�o�b�O�p�F���[�N���\��
 */
//--------------------------------------------------------------
  .macro  _DEBUG_WATCH_WORK  wk
  .short  EV_SEQ_DEBUG_WATCH_VALUE
  .short  \wk
  .endm
#endif //wb null

//======================================================================
//  ���z�}�V���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _VM_ADD ���z�}�V���ǉ�(�؂�ւ��͂����A����œ��삵�܂��I)
 *  @param id ���삳���鉼�z�}�V��ID
 */
//--------------------------------------------------------------
  .macro  _VM_ADD id
  .short  EV_SEQ_VM_ADD
  .short  \id
  .endm

//--------------------------------------------------------------
/**
 * _CHG_COMMON_SCR ���[�J���X�N���v�g���E�F�C�g��Ԃɂ��āA
 * ���ʃX�N���v�g�𓮍삳���܂�
 * @param id ���삳����X�N���v�gID
 */
//--------------------------------------------------------------
  .macro  _CHG_COMMON_SCR id
  .short  EV_SEQ_CHG_COMMON_SCR
  .short  \id
  .endm

//--------------------------------------------------------------
/**
 * @def _GLOBAL_CALL
 * @brief �t�@�C�����܂������X�N���v�g�̌Ăяo���i_CHG_COMMON_SCR�̕ʖ��j
 * @param id ���삳����X�N���v�gID
 */
//--------------------------------------------------------------
  .macro  _GLOBAL_CALL  id
  .short  EV_SEQ_CHG_COMMON_SCR
  .short  \id
  .endm

//--------------------------------------------------------------
/**
 *  _CHG_LOCAL_SCR ���ʃX�N���v�g���I�����āA���[�J���X�N���v�g���ĊJ�����܂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _CHG_LOCAL_SCR
  .short  EV_SEQ_CHG_LOCAL_SCR
  .endm

//--------------------------------------------------------------
/**
 * @def _RESERVE_SCRIPT
 * @brief �X�N���v�g�̗\��
 * @param scr_id  �X�N���v�g�w��ID
 *
 * �V�[���C�x���g�����^�C�~���O�ŋN������X�N���v�g��\��o�^����
 */
//--------------------------------------------------------------
#define _RESERVE_SCRIPT( scr_id ) \
    _ASM_RESERVE_SCRIPT scr_id

  .macro  _ASM_RESERVE_SCRIPT scr_id
  .short  EV_SEQ_RESERVE_SCRIPT
  .short  \scr_id
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_CONTINUE_RECOVER
 * @brief �R���e�B�j���[�����ǂ����̔���
 * @param ret_wk    TRUE�̂Ƃ��A�R���e�B�j���[�^�C�~���O
 *
 * FIELD_RECOVER_LABEL����̃X�N���v�g�ȊO�ł͏��FALSE�ɂȂ�
 */
//--------------------------------------------------------------
#define _CHECK_CONTINUE_RECOVER( ret_wk ) \
    _ASM_CHECK_CONTINUE_RECOVER ret_wk

    .macro  _ASM_CHECK_CONTINUE_RECOVER ret_wk
    .short  EV_SEQ_CHECK_CONTINUE_RECOVER
    .short  \ret_wk
    .endm

//======================================================================
//  ���򖽗�
//======================================================================
//--------------------------------------------------------------
/**
 *  _JUMP �W�����v
 *  @param adrs ��ѐ�̃A�h���X
 */
//--------------------------------------------------------------
  .macro  _JUMP adrs
  .short  EV_SEQ_JUMP
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IF_JUMP IF�W�����v
 *  @param cond ��r�R�}���h LT,EQ,GT...
 *  @param adrs cond�ƈ�v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_JUMP cond,adrs
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IF_CALL IF�R�[��
 *  @param cond ��r�R�}���h LT,EQ,GT...
 *  @param adrs cond�ƈ�v���Ă����ۂɌĂяo���A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_CALL cond,adrs
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFVAL_JUMP �l���r�A�����ǂ���̏ꍇ����(�W�����v)
 *  @param wk ��r���郏�[�N
 *  @param cond ��r�R�}���h LT,RQ,GT...
 *  @param val wk�Ɣ�r����l
 *  @param adrs cond�ƈ�v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IFVAL_JUMP wk,cond,val,adrs
  _CMPVAL  \wk,\val
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFVAL_CALL �l���r�A�����ǂ���̏ꍇ����(�R�[��)
 *  @param wk ��r���郏�[�N
 *  @param cond ��r�R�}���h LT,RQ,GT...
 *  @param val wk�Ɣ�r����l
 *  @param adrs cond�ƈ�v���Ă����ۂɌĂяo���A�h���X
 */
//--------------------------------------------------------------
  .macro  _IFVAL_CALL wk,cond,val,adrs
  _CMPVAL  \wk,\val
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFWK_JUMP ���[�N���r�A�����ǂ���̏ꍇ����(�W�����v)
 *  @param wk1 ��r���郏�[�N1
 *  @param cond ��r�R�}���h LT,RQ,GT...
 *  @param wk2 �Ɣ�r���郏�[�N2
 *  @param adrs cond�ƈ�v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IFWK_JUMP wk1,cond,wk2,adrs
  _CMPWK  \wk1,\wk2
  .short  EV_SEQ_IF_JUMP
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _IFWK_CALL ���[�N���r�A�����ǂ���̏ꍇ����(�R�[��)
 *  @param wk1 ��r���郏�[�N1
 *  @param cond ��r�R�}���h LT,RQ,GT...
 *  @param wk2 �Ɣ�r���郏�[�N2
 *  @param adrs cond�ƈ�v���Ă����ۂɌĂяo���A�h���X
 */
//--------------------------------------------------------------
  .macro  _IFWK_CALL wk1,cond,wk2,adrs
  _CMPWK  \wk1,\wk2
  .short  EV_SEQ_IF_CALL
  .byte  \cond
  .long  ((\adrs-.)-4)
  .endm


//======================================================================
//  �C�x���g�t���O�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _FLAG_SET �t���O�̃Z�b�g
 *  @param num �Z�b�g����t���O�i���o�[
 */
//--------------------------------------------------------------
#define   _FLAG_SET( num ) _ASM_FLAG_SET num

  .macro  _ASM_FLAG_SET num
  .short  EV_SEQ_FLAG_SET
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _FLAG_RESET �t���O�̃��Z�b�g
 *  @param ���Z�b�g����t���O�i���o�[
 */
//--------------------------------------------------------------
#define _FLAG_RESET( num ) _ASM_FLAG_RESET num

  .macro  _ASM_FLAG_RESET num
  .short  EV_SEQ_FLAG_RESET
  .short  \num
  .endm


//--------------------------------------------------------------
/**
 *  _FLAG_CHECK �t���O�`�F�b�N
 *  @param �`�F�b�N����t���O�i���o�[
 *  @param ���ʂ��i�[���郏�[�N
 */
//--------------------------------------------------------------
#define _FLAG_CHECK( num, wk ) _ASM_FLAG_CHECK num,wk
  
  .macro  _ASM_FLAG_CHECK num,wk
  .short  EV_SEQ_FLAG_CHECK
  .short  \num
  .short  \wk
  .endm

//======================================================================
//  ���[�N����֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _ADD_WK ���[�N�ɒl�𑫂�
 *  @param �l�𑫂����[�N
 *  @param num �����l
 */
//--------------------------------------------------------------
  .macro  _ASM_ADD_WK wk,num
  .short  EV_SEQ_ADD_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _SUB_WK ���[�N����l������
 *  @param �l���������[�N
 *  @param num �����l
 */
//--------------------------------------------------------------
  .macro  _ASM_SUB_WK wk,num
  .short  EV_SEQ_SUB_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _ASM_MUL_WK ���[�N�ɒl��������
 *  @param ����Ώۂ̃��[�N
 *  @param num ������l
 */
//--------------------------------------------------------------
  .macro  _ASM_MUL_WK wk,num
  .short  EV_SEQ_MUL_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _ASM_DIV_WK ���[�N��l�Ŋ���
 *  @param ����Ώۂ̃��[�N
 *  @param num ����l
 */
//--------------------------------------------------------------
  .macro  _ASM_DIV_WK wk,num
  .short  EV_SEQ_DIV_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _ASM_MOD_WK ���[�N�̏�]���Ƃ�
 *  @param ����Ώۂ̃��[�N
 *  @param num ����l
 */
//--------------------------------------------------------------
  .macro  _ASM_MOD_WK wk,num
  .short  EV_SEQ_MOD_WK
  .short  \wk
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _LDVAL ���[�N�ɒl����
 *  @param wk �l���i�[���郏�[�N
 *  @param val �i�[����l
 */
//--------------------------------------------------------------
#define _LDVAL( wk, val ) _ASM_LDVAL wk,val

  .macro  _ASM_LDVAL  wk,val
  .short  EV_SEQ_LD_WK_VAL
  .short  \wk
  .short  \val
  .endm

//--------------------------------------------------------------
/**
 *  _LDWK ���[�N�Ƀ��[�N�̒l����
 *  @param wk1 ����惏�[�N
 *  @param wk2 �������l���i�[���ꂽ���[�N
 */
//--------------------------------------------------------------
#define _LDWK( wk1, wk2 ) _ASM_LDWK wk1,wk2

  .macro  _ASM_LDWK  wk1,wk2
  .short  EV_SEQ_LD_WK_WK
  .short  \wk1
  .short  \wk2
  .endm

//--------------------------------------------------------------
/**
 *  _LDWKVAL ���[�N�ɒl�����[�N�̒l����
 *  @param wk1 ����惏�[�N
 *  @param wk2 �������l�������̓��[�N
 */
//--------------------------------------------------------------
  .macro  _ASM_LDWKVAL  wk1,wk2
  .short  EV_SEQ_LD_WK_WKVAL
  .short  \wk1
  .short  \wk2
  .endm

//======================================================================
//  �L�[���͊֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _AB_KEYWAIT �L�[�҂�
 *  @param none
 */
//--------------------------------------------------------------
#define _AB_KEYWAIT() _ASM_AB_KEYWAIT

  .macro  _ASM_AB_KEYWAIT
  .short  EV_SEQ_ABKEYWAIT
  .endm


//--------------------------------------------------------------
/**
 *  _LAST_KEYWAIT ���b�Z�[�W�Ō�̃L�[�҂�
 *
 *  ��AB�L�[�����łȂ��A�\���L�[�ł������邱�Ƃ��ł���
 */
//--------------------------------------------------------------
#define _LAST_KEYWAIT() _ASM_LAST_KEYWAIT

  .macro  _ASM_LAST_KEYWAIT
  .short  EV_SEQ_LASTKEYWAIT
  .endm

//--------------------------------------------------------------
/**
 *  _AB_KEYWAIT_MARK ���O�ɏo���Ă��郁�b�Z�[�W�E�B���h�E��
 *  �L�[�҂��L���i���j��\�������ăL�[�҂�
 *  @param none
 *  @note ���ӁI�҂��L���\���͌���GMM���ł��܂��BME�Đ���A�T�u�E�B���\����̂ݎg�p���܂��B
 *  �ڍׂ̓X�N���v�g�֘A���[�����Q�Ƃ��Ă��������B
 */
//--------------------------------------------------------------
#define _AB_KEYWAIT_MARK() _ASM_AB_KEYWAIT_MARK

  .macro  _ASM_AB_KEYWAIT_MARK
  .short  EV_SEQ_ABKEYWAIT_MARK
  .endm

//======================================================================
//  �t�B�[���h�@�V�X�e���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 * @def _SYSWIN_MSG
 * @brief �V�X�e���E�B���h�E�ł̃��b�Z�[�W�\��
 * @param msg_id  �\�����郁�b�Z�[�WID
 * @param pos     �E�B���h�E�\���ʒu�̎w��iPOS_UP, POS_DOWN, POS_DEFAULT�j
 */
//--------------------------------------------------------------
#define _SYSWIN_MSG( msg_id, pos )  _ASM_SYSWIN_MSG msg_id, pos

  .macro  _ASM_SYSWIN_MSG msg_id,up_down
  .short  EV_SEQ_SYSWIN_MSG
  .short  \msg_id
  .short  \up_down
  .endm

//--------------------------------------------------------------
/**
 *  @def  _SYSWIN_MSG_ALLPUT
 *  @brief  �W�J���b�Z�[�W���ꊇ�\��
 *  @param  msg_id  �\�����郁�b�Z�[�Wid
 *  @param  pos     �E�B���h�E�\���ʒu�̎w��iPOS_UP, POS_DOWN, POS_DEFAULT�j
 */
//--------------------------------------------------------------
#define _SYSWIN_MSG_ALLPUT( msg_id, pos ) \
    _ASM_SYSWIN_MSG_ALLPUT msg_id, pos

//--------------------------------------------------------------
/**
 *  _SYSWIN_MSG_ALLPUT_DOWN �W�J���b�Z�[�W���ꊇ�\��(1byte) �E�B���h�E��
 *  @param  msg_id  �\�����郁�b�Z�[�Wid
 *
 *  _SYSWIN_MSG_ALLPUT�ɓ������A_SYSWIN_MSG_ALLPUT_DOWN�͔p�~�\��ł��B
 */
//--------------------------------------------------------------
#define _SYSWIN_MSG_ALLPUT_DOWN( msg_id ) \
    _ASM_SYSWIN_MSG_ALLPUT msg_id, POS_DOWN
  
  .macro  _ASM_SYSWIN_MSG_ALLPUT msg_id, up_down
  .short  EV_SEQ_SYSWIN_MSG_ALLPUT
  .short  \msg_id
  .short  \up_down
  .endm

//--------------------------------------------------------------
/**
 *  @def    _SYSWIN_CLOSE
 *  @brief  �V�X�e���E�B���h�E����    �i�^�C���A�C�R���������ɕ��܂��j
 *  @param none
 */
//--------------------------------------------------------------
#define _SYSWIN_CLOSE() _ASM_SYSWIN_CLOSE

  .macro  _ASM_SYSWIN_CLOSE
  .short  EV_SEQ_SYSWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def    _SYSWIN_TIMEICON
 *  @brief  �V�X�e���E�B���h�E�^�C���A�C�R����\��
 *  @param flag 
            SCR_SYSWIN_TIMEICON_ON  (0)   �\��
            SCR_SYSWIN_TIMEICON_OFF (1)   ��\��
 */
//--------------------------------------------------------------
#define _SYSWIN_TIMEICON( flag ) _ASM_SYSWIN_TIMEICON flag

  .macro  _ASM_SYSWIN_TIMEICON flag
  .short  EV_SEQ_SYSWIN_TIMEICON
  .short  \flag
  .endm

//======================================================================
//  �t�B�[���h�@�����o���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG
 *  @brief  �����o���E�B���h�E�`��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG( msg_id , obj_id ) \
    _ASM_BALLOONWIN_OBJMSG msg_id, obj_id

  .macro _ASM_BALLOONWIN_OBJMSG msg_id, obj_id
  .short  EV_SEQ_BALLOONWIN_OBJMSG
  .short 0x0400
  .short \msg_id
  .short \obj_id
  .short WIN_NONE
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_TALKOBJ_MSG
 *  @brief  �����o���E�B���h�E�`��@�b���|��OBJ��p
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_MSG( msg_id ) \
    _ASM_BALLOONWIN_TALKOBJ_MSG msg_id
  
  .macro _ASM_BALLOONWIN_TALKOBJ_MSG msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_MSG
  .short 0x0400
  .short \msg_id
  .short WIN_NONE
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 * @def _BALLOONWIN_TALKOBJ_MSG_ARC
 * @brief �����o���E�B���h�E�`�� �b������OBJ��p�A���b�Z�[�W�A�[�J�C�u�w��t��
 *  @param  arc_id  ���b�Z�[�W�A�[�J�C�u�w��ID
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_MSG_ARC( arc_id, msg_id ) \
    _ASM_BALLOONWIN_TALKOBJ_MSG_ARC arc_id, msg_id

  .macro  _ASM_BALLOONWIN_TALKOBJ_MSG_ARC arc_id, msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_MSG
  .short \arc_id
  .short \msg_id
  .short WIN_NONE
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_POS
 *  @brief  �����o���E�B���h�E�`��@�ʒu�w�肠��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu usescript.h��POS_��`���Q��
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG_POS( msg_id , obj_id, pos ) \
    _ASM_BALLOONWIN_OBJMSG_POS msg_id, obj_id, pos
  
  .macro _ASM_BALLOONWIN_OBJMSG_POS msg_id, obj_id, pos
  .short  EV_SEQ_BALLOONWIN_OBJMSG
  .short 0x0400
  .short \msg_id
  .short \obj_id
  .short \pos
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_MF
 *  @brief  �����o���E�B���h�E�`��@�v���C���[���ʃ��b�Z�[�W�w��
 *  @param msg_id_m �\�����郁�b�Z�[�WID�@�j��
 *  @param msg_id_f �\�����郁�b�Z�[�WID�@����
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu usescript.h��POS_��`���Q��
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG_MF( msg_id_m, msg_id_f, obj_id, pos ) \
    _ASM_BALLOONWIN_OBJMSG_MF msg_id_m, msg_id_f, obj_id, pos

  .macro _ASM_BALLOONWIN_OBJMSG_MF msg_id_m, msg_id_f, obj_id, pos
  .short  EV_SEQ_BALLOONWIN_OBJMSG_MF
  .short 0x0400
  .short \msg_id_m
  .short \msg_id_f
  .short \obj_id
  .short \pos
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_WB
 *  @brief  �����o���E�B���h�E�`��@�o�[�W������
 *  @param msg_id_w �\�����郁�b�Z�[�WID�A�z���C�g��
 *  @param msg_id_b �\�����郁�b�Z�[�WID�A�u���b�N��
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu usescript.h��POS_��`���Q��
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG_WB( msg_id_w, msg_id_b, obj_id, pos ) \
    _ASM_BALLOONWIN_OBJMSG_WB msg_id_w, msg_id_b, obj_id, pos

  .macro _ASM_BALLOONWIN_OBJMSG_WB msg_id_w, msg_id_b, obj_id, pos
  .short  EV_SEQ_BALLOONWIN_OBJMSG_WB
  .short 0x0400
  .short \msg_id_w
  .short \msg_id_b
  .short \obj_id
  .short \pos
  .short BALLOONWIN_TYPE_NORMAL
  .endm

//--------------------------------------------------------------
/**
 *  @def  _GIZAWIN_OBJMSG_POS
 *  @brief  �����o���E�B���h�E�`��@�M�U�M�U�@�ʒu�w�肠��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu usescript.h��POS_��`���Q��
 */
//--------------------------------------------------------------
#define _GIZAWIN_OBJMSG_POS( msg_id , obj_id, pos ) \
    _ASM_GIZAWIN_OBJMSG_POS msg_id, obj_id, pos
  
  .macro _ASM_GIZAWIN_OBJMSG_POS msg_id, obj_id, pos
  .short  EV_SEQ_BALLOONWIN_OBJMSG
  .short 0x0400
  .short \msg_id
  .short \obj_id
  .short \pos
  .short BALLOONWIN_TYPE_GIZA
  .endm

//--------------------------------------------------------------
/**
 *  _BALLOONWIN_CLOSE �����o���E�B���h�E����
 *  @param none
 */
//--------------------------------------------------------------
#define _BALLOONWIN_CLOSE() _ASM_BALLOONWIN_CLOSE

  .macro  _ASM_BALLOONWIN_CLOSE
  .short  EV_SEQ_BALLOONWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  _GIZAWIN_CLOSE �����o���E�B���h�E����
 *  @param none
 */
//--------------------------------------------------------------
#define _GIZAWIN_CLOSE() _ASM_BALLOONWIN_CLOSE

//======================================================================
//  �������E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_OPEN
 *  @brief �������E�B���h�E��\������@�E�l�\��
 *  @param x �\��x�ʒu(�L�����P��)
 *  @param y �\��x�ʒu(�L�����P��)
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_OPEN( x, y ) \
    _ASM_GOLD_WIN_OPEN x, y

  .macro _ASM_GOLD_WIN_OPEN x, y
  .short EV_SEQ_GOLD_WIN_OPEN
  .short \x
  .short \y
  .endm

//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_CLOSE
 *  @brief �������E�B���h�E����������
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_CLOSE() \
    _ASM_GOLD_WIN_CLOSE

  .macro _ASM_GOLD_WIN_CLOSE
  .short EV_SEQ_GOLD_WIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def _GOLD_WIN_UPDATE
 *  @brief �������E�B���h�E���X�V����
 *  @param x �\��x�ʒu(�L�����P��)
 *  @param y �\��x�ʒu(�L�����P��)
 *  @param none
 */
//--------------------------------------------------------------
#define _GOLD_WIN_UPDATE( x, y ) \
    _ASM_GOLD_WIN_UPDATE x, y

  .macro _ASM_GOLD_WIN_UPDATE x, y
  .short EV_SEQ_GOLD_WIN_UPDATE
  .short \x
  .short \y
  .endm

//======================================================================
//  ���b�Z�[�W�E�B���h�E����
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _MSGWIN_CLOSE
 *  @brief ��ނ��킸�\�����Ă��郁�b�Z�[�W�E�B���h�E�����B
 *  @param none
 *  @note �Y���E�B���h�E�́A
 *  �V�X�e���E�B���h�E�A�����o���E�B���h�E�A�v���[���E�B���h�E�ABG�E�B���h�E
 */
//--------------------------------------------------------------
#define _MSGWIN_CLOSE() _ASM_MSGWIN_CLOSE

  .macro _ASM_MSGWIN_CLOSE
  .short EV_SEQ_MSGWIN_CLOSE
  .endm

//======================================================================
//  �v���[���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _PLAINWIN_MSG_UP
 *  @brief �v���[���E�B���h�E����ɕ\��
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _PLAINWIN_MSG_UP( msg_id ) _ASM_PLAINWIN_MSG msg_id, POS_UP

//--------------------------------------------------------------
/**
 *  @def _PLAINWIN_MSG_DOWN
 *  @brief �v���[���E�B���h�E�����ɕ\��
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _PLAINWIN_MSG_DOWN( msg_id ) _ASM_PLAINWIN_MSG msg_id, POS_DOWN
  
  .macro _ASM_PLAINWIN_MSG msg_id, up_down
  .short EV_SEQ_PLAINWIN_MSG
  .short \msg_id
  .byte \up_down
  .endm

//--------------------------------------------------------------
/**
 *  @def _PLAINWIN_CLOSE
 *  @brief �v���[���E�B���h�E�����
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _PLAINWIN_CLOSE() _ASM_PLAINWIN_CLOSE
  
  .macro _ASM_PLAINWIN_CLOSE
  .short EV_SEQ_PLAINWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def _GIZAPLAINWIN_MSG_POS
 *  @brief �M�U�v���[���E�B���h�E��\��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param pos POS_UP,POS_DOWN
 */
//--------------------------------------------------------------
#define _GIZAPLAINWIN_MSG_POS( msg_id, pos ) _ASM_GIZAPLAINWIN_MSG_POS msg_id, pos
  
  .macro _ASM_GIZAPLAINWIN_MSG_POS msg_id, pos
  .short EV_SEQ_PLAINWIN_GIZA_MSG
  .short \msg_id
  .byte \pos
  .endm

//--------------------------------------------------------------
/**
 *  @def _GIZAPLAINWIN_CLOSE
 *  @brief �M�U�v���[���E�B���h�E�����
 *  @param none
 */
//--------------------------------------------------------------
#define _GIZAPLAINWIN_CLOSE() _ASM_PLAINWIN_CLOSE

//======================================================================
//  �T�u�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _SUBWIN_MSG
 *  @brief �T�u�E�B���h�E��\��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param pos_x �\������X���W�i�L�����P��
 *  @param pos_y �\������Y���W�i�L�����P��
 *  @param win_id ���ʂׂ̈ɓo�^����C�ӂ�ID
 */
//--------------------------------------------------------------
#define _SUBWIN_MSG( msg_id, pos_x, pos_y, win_id ) \
    _ASM_SUBWIN_MSG msg_id, pos_x, pos_y, win_id
  
  .macro _ASM_SUBWIN_MSG  msg_id, pos_x, pos_y, win_id
  .short EV_SEQ_SUBWIN_MSG
  .short \msg_id
  .byte \pos_x
  .byte \pos_y
  .short \win_id
  .endm

//--------------------------------------------------------------
/**
 *  @def _SUBWIN_CLOSE
 *  @brief �T�u�E�B���h�E�����
 *  @param win_id _SUBWIN_MSG()�Ŏw�肵��win_id
 */
//--------------------------------------------------------------
#define _SUBWIN_CLOSE( win_id ) _ASM_SUBWIN_CLOSE win_id
  
  .macro _ASM_SUBWIN_CLOSE win_id
  .short EV_SEQ_SUBWIN_CLOSE
  .short \win_id
  .endm

//======================================================================
//  BG�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _BGWIN_MSG
 *  @brief BG�E�B���h�E��\������
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param bg_type �\������^�C�v TYPE_INFO,TYPE_TOWN,TYPE_POST,TYPE_ROAD
 */
//--------------------------------------------------------------
#define _BGWIN_MSG( msg_id, bg_type ) _ASM_BGWIN_MSG msg_id, bg_type
  
  .macro _ASM_BGWIN_MSG msg_id, bg_type
  .short EV_SEQ_BGWIN_MSG
  .short \msg_id
  .short \bg_type
  .endm

//--------------------------------------------------------------
/**
 *  @def _BGWIN_CLOSE
 *  @brief BG�E�B���h�E�����
 *  @param none 
 */
//--------------------------------------------------------------
#define _BGWIN_CLOSE()  _ASM_BGWIN_CLOSE
  
  .macro _ASM_BGWIN_CLOSE
  .short EV_SEQ_BGWIN_CLOSE
  .endm

//======================================================================
//  ����E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def _SPWIN_MSG
 *  @brief ����E�B���h�E��\������
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param x �E�B���h�E����X���W �L�����P��
 *  @param y �E�B���h�E����Y���W �L�����P��
 *  @param type �\������^�C�v SPWIN_LETTER SPWIN_BOOK��
 */
//--------------------------------------------------------------
#define _SPWIN_MSG( msg_id, x, y, type ) _ASM_SPWIN_MSG msg_id, x, y, type
  
  .macro _ASM_SPWIN_MSG msg_id, x, y, type
  .short EV_SEQ_SPWIN_MSG
  .short \msg_id
  .byte \x
  .byte \y
  .short \type
  .endm

//--------------------------------------------------------------
/**
 *  @def _SPWIN_CLOSE
 *  @brief ����E�B���h�E�����
 *  @param none 
 */
//--------------------------------------------------------------
#define _SPWIN_CLOSE()  _ASM_SPWIN_CLOSE
  
  .macro _ASM_SPWIN_CLOSE
  .short EV_SEQ_SPWIN_CLOSE
  .endm


//======================================================================
//  ���샂�f��
//======================================================================
//--------------------------------------------------------------
/**
 *  _ANIME_LABEL �A�j���[�V�����f�[�^���x��
 *  @param label �f�[�^���x����
 */
//--------------------------------------------------------------
  .macro  _ANIME_LABEL label
  .align  4
  \label:
  .endm

//--------------------------------------------------------------
/**
 *  _ANIME_DATA �A�j���[�V�����f�[�^(fldmmdl_code.h�Q��)
 *  @param code �A�j���[�V�����R�[�h
 *  @param num code���s��
 */
//--------------------------------------------------------------
  .macro  _ANIME_DATA  code,num
  .short  \code
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _OBJ_ANIME �t�B�[���hOBJ �A�j���[�V�����R�}���h
 *  @param obj_id �A�j���[�V�������s��OBJ ID
 *  @param adrs �A�j���[�V�����R�}���h�f�[�^���x��
 *
 *  @note ���ӁI �����~���Z�b�g���������́A
 *  _OBJ_PAUSE_ALL��������x�Ă΂Ȃ��Ƃ����Ȃ��I
 */
//--------------------------------------------------------------
#define _OBJ_ANIME( obj_id, adrs ) _ASM_OBJ_ANIME obj_id, adrs

  .macro  _ASM_OBJ_ANIME obj_id, adrs
  .short  EV_SEQ_OBJ_ANIME
  .short  \obj_id 
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _OBJ_ANIME_WAIT �A�j���[�V�����R�}���h�I���҂�
 *  @param none
 */
//--------------------------------------------------------------
#define _OBJ_ANIME_WAIT() _ASM_OBJ_ANIME_WAIT

  .macro  _ASM_OBJ_ANIME_WAIT
  .short  EV_SEQ_OBJ_ANIME_WAIT
  .endm
  
//--------------------------------------------------------------
/**
 *  _MOVE_CODE_GET ����R�[�h�擾(fldmmdl_code.h)
 *  @param ret_wk ����R�[�h���i�[���郏�[�N
 *  @param obj_id ����R�[�h���擾����OBJ ID
 */
//--------------------------------------------------------------
#define  _MOVE_CODE_GET(  ret_wk,obj_id ) _ASM_MOVE_CODE_GET ret_wk, obj_id

  .macro  _ASM_MOVE_CODE_GET  ret_wk, obj_id
  .short  EV_SEQ_MOVE_CODE_GET 
  .short  \ret_wk
  .short  \obj_id
  .endm
  
//--------------------------------------------------------------
/**
 *  _MOVE_CODE_CHANGE ����R�[�h��ύX(fldmmdl_code.h)
 *  @param obj_id ����R�[�h��ύX����OBJ ID
 *  @param code �ύX���铮��R�[�h
 */
//--------------------------------------------------------------
#define  _MOVE_CODE_CHANGE( obj_id, code ) _ASM_MOVE_CODE_CHANGE obj_id, code

  .macro  _ASM_MOVE_CODE_CHANGE obj_id, code
  .short  EV_SEQ_MOVE_CODE_CHANGE
  .short  \obj_id
  .short  \code
  .endm

//--------------------------------------------------------------
/**
 * @def _OBJ_POS_GET
 * @brief OBJ�̈ʒu�擾
 * @param obj_id �擾����OBJ ID
 * @param x X���W�i�[��
 * @param z Z���W�i�[��
 */
//--------------------------------------------------------------
#define _OBJ_POS_GET( obj_id,x,z ) _ASM_OBJ_POS_GET obj_id, x, z

  .macro  _ASM_OBJ_POS_GET  obj_id, x, z
  .short  EV_SEQ_OBJ_POS_GET
  .short  \obj_id 
  .short  \x
  .short  \z
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_POS_GET
 * @brief ��l���̈ʒu�擾
 * @param x X���W�i�[��
 * @param z Z���W�i�[��
 */
//--------------------------------------------------------------
#define _PLAYER_POS_GET( x, z ) _ASM_PLAYER_POS_GET x, z

  .macro  _ASM_PLAYER_POS_GET x, z
  .short  EV_SEQ_PLAYER_POS_GET
  .short  \x
  .short  \z
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_DIR_GET
 * @brief �v���C���[�̕����擾
 * @param ret_wk    �������󂯎�郏�[�N
 *
 * @note  �����ɂ��Ă�prog/include/field/field_dir.h���Q��
 * @todo  �O���b�h�}�b�v�݂̂Ń��[���}�b�v�̂��Ƃ͍l������Ă��Ȃ�
 */
//--------------------------------------------------------------
#define _PLAYER_DIR_GET( ret_wk ) \
    _ASM_PLAYER_DIR_GET ret_wk

    .macro  _ASM_PLAYER_DIR_GET ret_wk
    .short  EV_SEQ_PLAYER_DIR_GET
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_OBJID
 * @brief �w�肵�����W�ɂ���I�u�W�FID���擾����
 * @param ret_id    �I�u�W�FID���󂯎�郏�[�N
 * @param ret_valid �I�u�W�F�����݂��邩�ǂ������󂯎�郏�[�N
 * @param gx        x���W[grid]
 * @param gy        y���W[grid]
 * @param gz        z���W[grid]
 * @return �I�u�W�F�����݂���ꍇ,   ret_valid = TRUE, ret_id = �I�u�W�FID
 *         �I�u�W�F�����݂��Ȃ��ꍇ, ret_valid = FALSE
 */
//--------------------------------------------------------------
#define _GET_OBJID( ret_id, ret_valid, gx, gy, gz ) \
    _ASM_GET_OBJID ret_id, ret_valid, gx, gy, gz

    .macro  _ASM_GET_OBJID ret_id, ret_valid, gx, gy, gz 
    .short  EV_SEQ_GET_OBJID
    .short  \ret_id
    .short  \ret_valid
    .short  \gx
    .short  \gy
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_FRONT_OBJID
 * @brief �v���C���[�̑O��1�O���b�h�ɂ���I�u�W�FID���擾����
 * @param ret_id    �I�u�W�FID���󂯎�郏�[�N
 * @param ret_valid �I�u�W�F�����݂��邩�ǂ������󂯎�郏�[�N
 * @return �I�u�W�F�����݂���ꍇ,   ret_valid = TRUE, ret_id = �I�u�W�FID
 *         �I�u�W�F�����݂��Ȃ��ꍇ, ret_valid = FALSE
 */
//--------------------------------------------------------------
#define _GET_FRONT_OBJID( ret_id, ret_valid ) \
    _ASM_GET_FRONT_OBJID ret_id, ret_valid

    .macro  _ASM_GET_FRONT_OBJID ret_id, ret_valid
    .short  EV_SEQ_GET_FRONT_OBJID
    .short  \ret_id
    .short  \ret_valid
    .endm

//--------------------------------------------------------------
/**
 * _GET_OBJ_EV_FLAG �w���OBJ�Ɏw�肳��Ă���C�x���g�t���O���擾
 * @param id OBJ ID
 * @param ret_id �t���O�i�[��
 */
//--------------------------------------------------------------
#define _GET_OBJ_EV_FLAG( id, ret_id ) \
  _ASM_GET_OBJ_EV_FLAG id, ret_id

  .macro _ASM_GET_OBJ_EV_FLAG id, ret_id
  .short EV_SEQ_OBJ_EV_FLAG_GET   
  .short \id
  .short \ret_id
  .endm

//--------------------------------------------------------------
/**
 * _OBJ_ADD_EX OBJ��ǉ��B
 * @param x �\������O���b�hX���W
 * @param z �\������O���b�hZ���W
 * @param dir ���� DIR_UP��
 * @param id ���ʗpOBJ ID
 * �����ʗpID�� MMDL_ID_EVENTOBJ_00 ���g���܂��B�����A���̃R�}���h�Œǉ�����OBJ�͓����ɂP�l�܂�
 * @param code �\���R�[�hBOY1��
 * @param move ����R�[�hMV_RND��
 */
//--------------------------------------------------------------
#define _OBJ_ADD_EX( x,z,dir,id,code,move ) _ASM_OBJ_ADD_EX x, z, dir, id, code, move

.macro _ASM_OBJ_ADD_EX x, z, dir, id, code, move
  .short EV_SEQ_OBJ_ADD
  .short \x
  .short \z
  .short \dir
  .short \id
  .short \code
  .short \move
.endm

//--------------------------------------------------------------
/**
 * _OBJ_ADD �]�[�����Ŕz�u����Ă���OBJ�ꗗ���A�w���ID������OBJ��ǉ�
 * @param id �z�uOBJ���Œǉ�����OBJ ID
 * @note ���ӁI�ǉ�����OBJ�̓����~�������܂܂�Ă��܂��B�t�B�[���h�������X�N���v�g���Œǉ�����ꍇ�̓|�[�Y�������Ă�ł��������B
 */
//--------------------------------------------------------------
#define _OBJ_ADD( id ) \
  _ASM_OBJ_ADD id

  .macro _ASM_OBJ_ADD id
  .short EV_SEQ_OBJ_ADD_EV
  .short \id
  .endm

//--------------------------------------------------------------
/**
 * _OBJ_DEL �w���OBJ_ID��������OBJ���폜�B
 * @param id �z�uOBJ���Œǉ�����OBJ ID
 * @attention OBJ���폜����݂̂̃R�}���h�BOBJ�����t���O���̑���͖����B
 */
//--------------------------------------------------------------
#define _OBJ_DEL( id )  \
    _ASM_OBJ_DEL id

  .macro _ASM_OBJ_DEL id
  .short EV_SEQ_OBJ_DEL
  .short \id
  .endm

//--------------------------------------------------------------
/**
 * _OBJ_POS_CHANGE �w���OBJ_ID��������OBJ�̍��W�ʒu��ύX�B
 * @param id ���W�ύX����OBJ ID
 * @param gx �O���b�hX���W
 * @param gy �O���b�hY���W
 * @param gz �O���b�hZ���W
 * @param dir ���� DIR_UP��
 */
//--------------------------------------------------------------
#define _OBJ_POS_CHANGE( id, gx, gy, gz, dir )  \
    _ASM_OBJ_POS_CHANGE id, gx, gy, gz, dir

	.macro	_ASM_OBJ_POS_CHANGE	id,gx,gy,gz,dir
	.short	EV_SEQ_OBJ_POS_CHANGE
	.short	\id
	.short	\gx
	.short	\gy
	.short	\gz
	.short	\dir
	.endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_REQUEST
 * @brief ���@�Ƀ��N�G�X�g���o��
 * @param reqbit FIELD_PLAYER_REQBIT_NORMAL�� C:\home\pokemon_wb\prog\src\field\field_player_code.h
 */
//--------------------------------------------------------------
#define _PLAYER_REQUEST( reqbit ) _ASM_PLAYER_REQUEST reqbit

  .macro  _ASM_PLAYER_REQUEST reqbit
  .short  EV_SEQ_PLAYER_REQUEST
  .short  \reqbit
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_UPDOWN_EFFECT
 * @brief ���@�̍����ω����o
 * @param type  �@���������0�̎����_����A1�̎��t�Z�����ʒu����
 * @param frame   ����������
 * @param length  �ړ������i3D���W�P�ʁj
 * @param way     �ړ����� 0=��  1=��
 *
 * @note
 * ���@�̓������𑀍삵�Ė߂��������Ȃ����ߎg�p�ӏ��͗v���ӁI
 */
//--------------------------------------------------------------
#define _PLAYER_UPDOWN_EFFECT( type, frame, length, way ) \
    _ASM_PLAYER_UPDOWN type, frame, length, way

  .macro  _ASM_PLAYER_UPDOWN type, frame, length, way
  .short  EV_SEQ_PLAYER_UPDOWN
  .short  \type
  .short  \frame
  .short  \length
  .short  \way
  .endm

//--------------------------------------------------------------
/**
 * @def _POS_PLAYER_TURN
 * @brief ���@�̍����ω����o
 * @param type  �@���������
 * @param frame   ����������
 * @param length
 */
//--------------------------------------------------------------
#define _POS_PLAYER_TURN() _ASM_POS_PLAYER_TURN

  .macro  _ASM_POS_PLAYER_TURN
  .short  EV_SEQ_POS_PLAYER_TURN
  .endm

//--------------------------------------------------------------
/**
 * @def _PLAYER_FORM_GET
 * @brief ���@�̓���`�Ԃ��擾
 * @param ret_wk �`�Ԋi�[��
 * SCR_PLAYER_FORM_NORMAL�@��
 * SCR_PLAYER_FORM_CYCLE�@���]��
 * SCR_PLAYER_FORM_SWIM�@�g���
 */
//--------------------------------------------------------------
#define _PLAYER_FORM_GET( ret_wk ) _ASM_PLAYER_FORM_GET ret_wk

  .macro _ASM_PLAYER_FORM_GET ret_wk
  .short EV_SEQ_GET_PLAYER_FORM
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @brief   ���@�̃X�N���v�g�p���[�����P�[�V�����擾�@�I���[���}�b�v��p�I
 * @param index   �߂�l�P�@�C���f�b�N�X
 * @param front   �߂�l�Q  �t�����g�C���f�b�N�X
 * @param side    �߂�l�R  �T�C�h�C���f�b�N�X
 */
//--------------------------------------------------------------
#define _GET_PLAYER_RAIL_LOC( index, front, side ) \
  _ASM_GET_PLAYER_RAIL_LOC index, front, side

  .macro  _ASM_GET_PLAYER_RAIL_LOC index, front, side
  .short  EV_SEQ_GET_PLAYER_RAIL_LOC
  .short  \index
  .short  \front
  .short  \side
  .endm

//--------------------------------------------------------------
/**
 * @brief   ���샂�f���̃X�N���v�g�p���[�����P�[�V�����擾�@�I���[���}�b�v��p�I
 * @param id ���W�ύX����OBJ ID
 * @param index   �߂�l�P�@�C���f�b�N�X
 * @param front   �߂�l�Q  �t�����g�C���f�b�N�X
 * @param side    �߂�l�R  �T�C�h�C���f�b�N�X
 */
//--------------------------------------------------------------
#define _GET_OBJ_RAIL_LOC( id, index, front, side ) \
  _ASM_GET_OBJ_RAIL_LOC id, index, front, side

  .macro  _ASM_GET_OBJ_RAIL_LOC id, index, front, side
  .short  EV_SEQ_GET_OBJ_RAIL_LOC
  .short  \id
  .short  \index
  .short  \front
  .short  \side
  .endm
  
//======================================================================
//  ���샂�f���@�C�x���g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _TURN_HERO_SITE �b��������OBJ�����@�����֐U�����
 *  @param none
 */
//--------------------------------------------------------------
#define _TURN_HERO_SITE() _ASM_TURN_HERO_SITE

.macro  _ASM_TURN_HERO_SITE
.short  EV_SEQ_OBJ_TURN
.endm

//======================================================================
//  �͂��A�������@����
//======================================================================
//--------------------------------------------------------------
/**
 *  _YES_NO_WIN �͂��A�������I��
 *  @param ret_wk �͂��A�������I�����ʊi�[��
*  @retval SCR_YES (0)�̂Ƃ��A�u�͂��v��I��
*  @retval SCR_NO (1)�̂Ƃ��A�u�������v�܂��̓L�����Z��
 */
//--------------------------------------------------------------
#define _YES_NO_WIN( ret_wk ) _ASM_YES_NO_WIN ret_wk

  .macro  _ASM_YES_NO_WIN ret_wk
  .short  EV_SEQ_YES_NO_WIN
  .short  \ret_wk
  .endm

//======================================================================
//  WORDSET�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _PLAYER_NAME ��l�������w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 */
//--------------------------------------------------------------
#define _PLAYER_NAME( idx ) _ASM_PLAYER_NAME idx

  .macro  _ASM_PLAYER_NAME idx
  .short  EV_SEQ_PLAYER_NAME
  .byte   \idx
  .endm

//--------------------------------------------------------------
/**
 *  _ITEM_NAME �A�C�e�������w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 *  @param itemno ���O���擾����A�C�e���i���o�[
 */
//--------------------------------------------------------------
#define _ITEM_NAME( idx, itemno ) _ASM_ITEM_NAME idx, itemno
  
  .macro _ASM_ITEM_NAME idx, itemno
  .short EV_SEQ_ITEM_NAME
  .byte \idx
  .short \itemno
  .endm

//--------------------------------------------------------------
/**
 *	�Z�}�V���̋Z�����w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 *  @param itemno ���O���擾����A�C�e���i���o�[
 */
//--------------------------------------------------------------
#define _ITEM_WAZA_NAME( idx, itemno ) _ASM_ITEM_WAZA_NAME idx, itemno

	.macro	_ASM_ITEM_WAZA_NAME	idx,itemno
	.short	EV_SEQ_ITEM_WAZA_NAME
	.byte	\idx
	.short	\itemno
	.endm

//--------------------------------------------------------------
/**
 *	�Z�����w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 *  @param wazano ���O���擾����Z�ԍ�
 */
//--------------------------------------------------------------
#define _WAZA_NAME( idx, waza ) _ASM_WAZA_NAME idx,waza

	.macro	_ASM_WAZA_NAME	idx,waza
	.short	EV_SEQ_WAZA_NAME
	.byte	\idx
	.short	\waza
	.endm

//--------------------------------------------------------------
/**
 * @def _POKETYPE_NAME
 * @brief �|�P�����E�킴�̃^�C�v�����w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 *  @param poketype ���O���擾����^�C�v�w��ID
 *  @note
 *  ����poketype�ɓn����`���� C:\home\pokemon_wb\prog\include\poke_tool\poketype_def.h�Q��
 */
//--------------------------------------------------------------
#define _POKETYPE_NAME( idx, poketype ) \
    _ASM_POKETYPE_NAME idx, poketype

    .macro  _ASM_POKETYPE_NAME idx, poketype
    .short  EV_SEQ_POKETYPE_NAME
    .byte   \idx
    .short  \poketype
    .endm

//--------------------------------------------------------------
/**
 * @def _MONS_NAME
 * @brief ������i���o�[����|�P���������w��^�O�ɃZ�b�g
 *  @param idx �Z�b�g����^�O�i���o�[
 *  @param monsno ���O���擾����|�P�����̂�����i���o�[
 */
//--------------------------------------------------------------
#define _MONS_NAME( idx, monsno ) \
    _ASM_MONS_NAME idx, monsno

    .macro  _ASM_MONS_NAME idx, monsno
    .short  EV_SEQ_MONS_NAME
    .byte   \idx
    .short  \monsno
    .endm

//--------------------------------------------------------------
/**
 * @def _PLACE_NAME
 * @brief �n�����w���^�O�ɃZ�b�g
 * @param idx     �Z�b�g����^�O�i���o�[
 * @param zone_id �Z�b�g����n���̃]�[��ID
 */
//--------------------------------------------------------------
#define _PLACE_NAME( idx, zone_id ) \
    _ASM_PLACE_NAME idx, zone_id

    .macro  _ASM_PLACE_NAME idx, zone_id
    .short  EV_SEQ_PLACE_NAME
    .byte   \idx
    .short  \zone_id
    .endm

//--------------------------------------------------------------
/**
 *  �|�P�b�g�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pocket_id   �o�b�O�̃|�P�b�g�����w��ID
 */
//--------------------------------------------------------------
#define _POCKET_NAME( idx, pocket_id )  _ASM_POCKET_NAME idx, pocket_id

  .macro  _ASM_POCKET_NAME idx, pocket_id
  .short  EV_SEQ_POCKET_NAME
  .byte   \idx
  .short  \pocket_id
  .endm

//--------------------------------------------------------------
/**
 *  �莝���|�P�����̎푰�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pos �Z�b�g����|�P�������w��
 */
//--------------------------------------------------------------
#define _TEMOTI_MONS_NAME( idx, pos ) \
    _ASM_TEMOTI_MONS_NAME idx, pos

  .macro _ASM_TEMOTI_MONS_NAME idx, pos
  .short EV_SEQ_TEMOTI_MONS_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  �莝���|�P�����̃j�b�N�l�[�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pos �Z�b�g����|�P�������w��
 */
//--------------------------------------------------------------
#define _TEMOTI_NICK_NAME( idx, pos ) \
    _ASM_TEMOTI_NICK_NAME idx, pos

  .macro _ASM_TEMOTI_NICK_NAME idx, pos
  .short EV_SEQ_TEMOTI_NICK_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  ��ĉ��̃|�P�����̎푰�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pos �Z�b�g����|�P�������w��
 */
//--------------------------------------------------------------
#define _SODATEYA_MONS_NAME( idx, pos ) \
    _ASM_SODATEYA_MONS_NAME idx, pos

  .macro _ASM_SODATEYA_MONS_NAME idx, pos
  .short EV_SEQ_SODATEYA_MONS_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  ��ĉ��̃|�P�����̃j�b�N�l�[�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pos �Z�b�g����|�P�������w��
 */
//--------------------------------------------------------------
#define _SODATEYA_NICK_NAME( idx, pos ) \
    _ASM_SODATEYA_NICK_NAME idx, pos

  .macro _ASM_SODATEYA_NICK_NAME idx, pos
  .short EV_SEQ_SODATEYA_NICK_NAME
  .byte  \idx
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ���l���w��^�O�ɃZ�b�g
 * @param  idx �Z�b�g����^�O�i���o�[
 * @param  num �Z�b�g���鐔�l
 * @param keta ����
 */
//--------------------------------------------------------------
#define _NUMBER( idx, num, keta ) \
    _ASM_NUMBER idx, num, keta

  .macro _ASM_NUMBER idx, num, keta
  .short EV_SEQ_NUMBER
  .byte  \idx
  .short \num
  .short \keta
  .endm

//--------------------------------------------------------------
/**
 *  �~���[�W�J���֌W�̃��[�h���w��^�O�ɃZ�b�g(���̃��b�p�[���g���Ă��������B
 * @param type �Z�b�g������(0:���ږ� 1:�O�b�Y��
 * @param idx �Z�b�g����^�O�i���o�[
 * @param val �Z�b�g�����ޔԍ�(���ږ�:���� �O�b�Y��:�O�b�Y�ԍ�
 */
//--------------------------------------------------------------
#define _MUSICAL_WORD( type, idx, val ) \
    _ASM_MUSICAL_WORD type, idx, val

  .macro _ASM_MUSICAL_WORD type, idx, val
  .short EV_SEQ_MUSICAL_WORD
  .byte  \type
  .byte  \idx
  .short \val
  .endm

//--------------------------------------------------------------
/**
 * �~���[�W�J�� ROM�ɓ����Ă��鉉�ږ������[�h�Z�b�g�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 */
//--------------------------------------------------------------
#define _MUSICAL_WORD_PROGRAM_TITLE( idx ) \
    _ASM_MUSICAL_WORD_PROGRAM_TITLE idx

  .macro _ASM_MUSICAL_WORD_PROGRAM_TITLE idx
  .short EV_SEQ_MUSICAL_WORD
  .byte  0
  .byte  \idx
  .short 0
  .endm

//--------------------------------------------------------------
/**
 * �~���[�W�J�� �O�b�Y�������[�h�Z�b�g�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param no  �O�b�Y�ԍ�
 */
//--------------------------------------------------------------
#define _MUSICAL_WORD_GOODS_NAME( idx, no ) \
    _ASM_MUSICAL_WORD_GOODS_NAME idx, no

  .macro _ASM_MUSICAL_WORD_GOODS_NAME idx, no
  .short EV_SEQ_MUSICAL_WORD
  .byte  1
  .byte  \idx
  .short \no
  .endm

//--------------------------------------------------------------
/**
 * �ȈՉ�b�P����o�b�t�@�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param word  �ȈՉ�b���[�h
 */
//--------------------------------------------------------------
#define _EASY_TALK_WORD_NAME( idx, word ) \
    _ASM_EASY_TALK_WORD_NAME idx, word

  .macro _ASM_EASY_TALK_WORD_NAME idx, word
  .short EV_SEQ_EASY_TALK_WORD_NAME
  .byte \idx
  .short \word
  .endm

//--------------------------------------------------------------
/**
 * �������o�b�t�@�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param code  ���R�[�h
 */
//--------------------------------------------------------------
#define _COUNTRY_NAME( idx, code ) \
    _ASM_COUNTRY_NAME idx, code

  .macro _ASM_COUNTRY_NAME idx, code
  .short EV_SEQ_COUNTRY_NAME
  .byte \idx
  .short \code
  .endm

//--------------------------------------------------------------
/**
 * ������o�b�t�@�ɃZ�b�g
 * @param idx     �Z�b�g����^�O�i���o�[
 * @param hobbyID �ID ( HOBBY_ID_xxxx )
 *  
 * ���ID �� resource/research_radar/data/hobby_id.h �Œ�`
 */
//--------------------------------------------------------------
#define _HOBBY_NAME( idx, hobbyID ) \
    _ASM_HOBBY_NAME idx, hobbyID

  .macro _ASM_HOBBY_NAME idx, hobbyID
  .short EV_SEQ_HOBBY_NAME
  .byte  \idx
  .short \hobbyID
  .endm
  
//--------------------------------------------------------------
/**
 *  GPower�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param gpower_id   GPOWER_ID
 */
//--------------------------------------------------------------
#define _GPOWER_NAME( idx, gpower_id )  _ASM_GPOWER_NAME idx, gpower_id

  .macro  _ASM_GPOWER_NAME idx, gpower_id
  .short  EV_SEQ_GPOWER_NAME
  .byte   \idx
  .short  \gpower_id
  .endm

//--------------------------------------------------------------
/**
 *  �g���[�i�[��ʂ��w��^�O�ɃZ�b�g
 * @param idx   �Z�b�g����^�O�i���o�[
 * @param tr_id �Z�b�g����g���[�i�[���
 */
//--------------------------------------------------------------
#define _TRAINER_TYPE_NAME( idx, tr_id ) \
    _ASM_TRAINER_TYPE_NAME idx, tr_id

  .macro  _ASM_TRAINER_TYPE_NAME idx, tr_id
  .short  EV_SEQ_TRAINER_TYPE_NAME
  .byte   \idx
  .short  \tr_id
  .endm
  
//--------------------------------------------------------------
/**
 * �񓚂��o�b�t�@�ɃZ�b�g
 * @param idx      �Z�b�g����^�O�i���o�[
 * @param answerID ��ID ( ANSWER_ID_xxxx )
 *  
 * ���ID �� resource/research_radar/data/answer_id.h �Œ�`
 */
//--------------------------------------------------------------
#define _ANSWER_NAME( idx, answerID ) \
    _ASM_ANSWER_NAME idx, answerID

  .macro _ASM_ANSWER_NAME idx, answerID
  .short EV_SEQ_ANSWER_NAME
  .byte  \idx
  .short \answerID
  .endm

//--------------------------------------------------------------
/**
 * �ʎZ�w����Ⴂ�񐔁x���o�b�t�@�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 */
//--------------------------------------------------------------
#define _SURETIGAI_COUNT( idx ) \
    _ASM_SURETIGAI_COUNT idx

  .macro _ASM_SURETIGAI_COUNT idx
  .short EV_SEQ_SURETIGAI_COUNT
  .byte  \idx
  .endm

//--------------------------------------------------------------
/**
 * �ʎZ�w����Ⴂ�ł�����󂯂��񐔁x���o�b�t�@�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 */
//--------------------------------------------------------------
#define _SURETIGAI_THANKS_RECEIVE_COUNT( idx ) \
    _ASM_SURETIGAI_THANKS_RECEIVE_COUNT idx

  .macro _ASM_SURETIGAI_THANKS_RECEIVE_COUNT idx
  .short EV_SEQ_SURETIGAI_THANKS_RECEIVE_COUNT
  .byte  \idx
  .endm

//======================================================================
//  �����g���[�i�[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _EYE_TRAINER_MOVE_SET
 *  @brief  �����F�g���[�i�[�ړ��Ăяo��
 *  @param pos �ړ�������g���[�i�[ �f�[�^�ԍ�0,1
 *
 *  @note
 *  �g���[�i�[�����X�N���v�g����Ă΂�Ă���B
 *  �ʏ�X�N���v�g�ł̎g�p�͋֎~�Ƃ���B
 */
//--------------------------------------------------------------
#define _EYE_TRAINER_MOVE_SET( pos ) \
    _ASM_EYE_TRAINER_MOVE_SET pos

  .macro  _ASM_EYE_TRAINER_MOVE_SET  pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SET
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  @def  _EYE_TRAINER_MOVE_SINGLE
 *  @brief  �����F�g���[�i�[�ړ��@�V���O��
 *  @param pos �ړ�������g���[�i�[ �f�[�^�ԍ�0,1
 *
 *  @note
 *  �g���[�i�[�����X�N���v�g����Ă΂�Ă���B
 *  �ʏ�X�N���v�g�ł̎g�p�͋֎~�Ƃ���B
 */
//--------------------------------------------------------------
#define _EYE_TRAINER_MOVE_SINGLE( pos ) \
    _ASM_EYE_TRAINER_MOVE_SINGLE pos

  .macro  _ASM_EYE_TRAINER_MOVE_SINGLE pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SINGLE
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  @def  _EYE_TRAINER_MOVE_DOUBLE
 *  @brief  �����F�g���[�i�[�ړ��@�_�u��
 *  @param none
 *
 *  @note
 *  �g���[�i�[�����X�N���v�g����Ă΂�Ă���B
 *  �ʏ�X�N���v�g�ł̎g�p�͋֎~�Ƃ���B
 */
//--------------------------------------------------------------
#define _EYE_TRAINER_MOVE_DOUBLE()  \
    _ASM_EYE_TRAINER_MOVE_DOUBLE

  .macro  _ASM_EYE_TRAINER_MOVE_DOUBLE
  .short  EV_SEQ_EYE_TRAINER_MOVE_DOUBLE
  .endm

//--------------------------------------------------------------
/**
 *  @def  _EYE_TRAINER_TYPE_GET
 *  @brief  �����F�g���[�i�[�^�C�v�擾
 *  @param ret_wk �g���[�i�[�^�C�v�i�[�� SCR_EYE_TR_TYPE_SINGLE�����i�[�����
 *
 *  @note
 *  �g���[�i�[�����X�N���v�g����Ă΂�Ă���B
 *  �ʏ�X�N���v�g�ł̎g�p�͋֎~�Ƃ���B
 */
//--------------------------------------------------------------
#define _EYE_TRAINER_TYPE_GET( ret_wk ) \
    _ASM_EYE_TRAINER_TYPE_GET ret_wk

  .macro  _ASM_EYE_TRAINER_TYPE_GET ret_wk
  .short  EV_SEQ_EYE_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  @def  _EYE_TRAINER_ID_GET
 *  @brief  �����F�g���[�i�[ID�擾
 *  @param  pos     �ʒu�w��iSCR_EYE_TR_0 / SCR_EYE_TR_1 )
 *  @param  ret_wk
 *
 *  @note
 *  �g���[�i�[�����X�N���v�g����Ă΂�Ă���B
 *  �ʏ�X�N���v�g�ł̎g�p�͋֎~�Ƃ���B
 */
//--------------------------------------------------------------
#define _EYE_TRAINER_ID_GET( pos, ret_wk ) \
  _ASM_EYE_TRAINER_ID_GET  pos,ret_wk
  .macro  _ASM_EYE_TRAINER_ID_GET  pos,ret_wk
  .short  EV_SEQ_EYE_TRAINER_ID_GET
  .short  \pos
  .short  \ret_wk
  .endm

//======================================================================
//  �g���[�i�[�o�g���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _TRAINER_ID_GET
 *  @brief  �g���[�i�[ID�擾
 *  @param wk ID�i�[�惏�[�N
 */
//--------------------------------------------------------------
#define _TRAINER_ID_GET( wk ) \
  _ASM_TRAINER_ID_GET wk

  .macro  _ASM_TRAINER_ID_GET  wk
  .short  EV_SEQ_TRAINER_ID_GET
  .short  \wk
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TRAINER_BTL_SET
 *  @brief  �g���[�i�[�퓬�Ăяo��
 *  @param tr_id_0 �g���[�i�[ID0
 *  @param tr_id_1 �g���[�i�[ID1
 */
//--------------------------------------------------------------
#define _TRAINER_BTL_SET( tr_id0, tr_id1 ) \
    _ASM_TRAINER_BTL_SET tr_id0, tr_id1
  
  .macro  _ASM_TRAINER_BTL_SET  tr_id_0,tr_id_1
  .short  EV_SEQ_TRAINER_BTL_SET
  .short  \tr_id_0
  .short  \tr_id_1
  .short  SCR_BATTLE_MODE_NONE
  .endm

//--------------------------------------------------------------
/**
 *  @brief  �g���[�i�[�퓬�Ăяo���i����w�肠��j
 *  @param tr_id_0 �g���[�i�[ID0
 *  @param tr_id_1 �g���[�i�[ID1
 *  @note �퓬��͕K���u�����v�Ŗ߂��Ă��܂��B�퓬���_TRAINER_WIN()�͕K�v�ɂȂ�܂��B
 */
//--------------------------------------------------------------
#define _TRAINER_BTL_SP_SET( tr_id0, tr_id1, flags ) \
    _ASM_TRAINER_BTL_SP_SET tr_id0, tr_id1, flags
  
  .macro  _ASM_TRAINER_BTL_SP_SET  tr_id_0,tr_id_1 flags
  .short  EV_SEQ_TRAINER_BTL_SET
  .short  \tr_id_0
  .short  \tr_id_1
  .short  \flags
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TRAINER_MULTI_BTL_SET
 *  @brief  �g���[�i�[�퓬�i�}���`�o�g���j�Ăяo��
 *  @param partner_id �p�[�g�i�[�̃g���[�i�[ID
 *  @param tr_id_0 �ΐ푊��(��)�̃g���[�i�[ID
 *  @param tr_id_1 �ΐ푊��(�E)�̃g���[�i�[ID
 */
//--------------------------------------------------------------
#define _TRAINER_MULTI_BTL_SET( partner_id, tr_id_0, tr_id_1 ) \
  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1

  .macro  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1
  .short  EV_SEQ_TRAINER_MULTI_BTL_SET
  .short  \partner_id
  .short  \tr_id_0
  .short  \tr_id_1
  .short  SCR_BATTLE_MODE_NONE
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_MSG_SET �g���[�i�[��b�Ăяo��
 *  @param tr_id ���b�Z�[�W�\���p�g���[�i�[ID
 *  @param kind_id ��b���
 */
//--------------------------------------------------------------
#define _TRAINER_MSG_SET( tr_id,kind_id ) \
  _ASM_TRAINER_MSG_SET  tr_id,kind_id

  .macro  _ASM_TRAINER_MSG_SET  tr_id,kind_id
  .short  EV_SEQ_TRAINER_MSG_SET
  .short  \tr_id
  .short  \kind_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_TALK_TYPE_GET �g���[�i�[��b�̎�ގ擾
 *  @param wk1 ��b��ފi�[�惏�[�N �퓬�O
 *  @param wk2 ��b��ފi�[�惏�[�N �퓬��
 *  @param wk2 ��b��ފi�[�惏�[�N �|�P�����P��
 */
//--------------------------------------------------------------
#define _TRAINER_TALK_TYPE_GET(  wk1,wk2,wk3 ) \
  _ASM_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3

  .macro  _ASM_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
  .short  EV_SEQ_TRAINER_TALK_TYPE_GET
  .short  \wk1
  .short  \wk2
  .short  \wk3
  .endm

//--------------------------------------------------------------
/**
 *  _REVENGE_TRAINER_TALK_TYPE_GET �Đ�g���[�i�[��b�̎�ގ擾
 *  @param wk1 ��b��ފi�[�惏�[�N �퓬�O
 *  @param wk2 ��b��ފi�[�惏�[�N �퓬��
 *  @param wk2 ��b��ފi�[�惏�[�N �|�P�����P��
 */
//--------------------------------------------------------------
#define _REVENGE_TRAINER_TALK_TYPE_GET(  wk1,wk2,wk3 ) \
  _ASM_REVENGE_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3

  .macro  _ASM_REVENGE_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
  .short  EV_SEQ_REVENGE_TRAINER_TALK_TYPE_GET
  .short  \wk1
  .short  \wk2
  .short  \wk3
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_TYPE_GET �g���[�i�[�^�C�v�擾
 *  @param ret_wk �^�C�v�i�[�惏�[�N
 */
//--------------------------------------------------------------
#define _TRAINER_TYPE_GET( ret_wk ) \
  _ASM_TRAINER_TYPE_GET  ret_wk

  .macro  _ASM_TRAINER_TYPE_GET  ret_wk
  .short  EV_SEQ_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_BGM_SET �g���[�i�[BGM�Z�b�g
 *  @param tr_id �g���[�i�[ID
 */
//--------------------------------------------------------------
#define _TRAINER_BGM_SET(  tr_id ) \
  _ASM_TRAINER_BGM_SET  tr_id

  .macro  _ASM_TRAINER_BGM_SET  tr_id
  .short  EV_SEQ_TRAINER_BGM_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 * @def _TRAINER_WIN
 * @brief �g���[�i�[�폟��
 *
 * @note
 * ���ۂɂ̓t�F�[�h���������s���Ă��܂��񂪁A
 * ����̊g�����l���ăg���[�i�[�폈���ɓ���Ă����ĉ������B
 */
//--------------------------------------------------------------
#define _TRAINER_WIN() _ASM_TRAINER_WIN_CALL

  .macro  _ASM_TRAINER_WIN_CALL
    _ASM_TRAINER_WIN
    //_BLACK_IN() //�����Ńu���b�N�C�����Ăԗ\��
  .endm

  .macro  _ASM_TRAINER_WIN
  .short  EV_SEQ_TRAINER_WIN
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE �g���[�i�[�s�k
 *  @param none
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE() \
  _ASM_TRAINER_LOSE

  .macro  _ASM_TRAINER_LOSE
  .short  EV_SEQ_TRAINER_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TRAINER_LOSE_CHECK
 *  @brief  �g���[�i�[�s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[��
 *  
 *  @retval SCR_BATTLE_WIN  ����
 *  @retval SCR_BATTLE_LOSE �s�k
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE_CHECK( ret_wk ) \
    _ASM_TRAINER_LOSE_CHECK ret_wk

  .macro  _ASM_TRAINER_LOSE_CHECK ret_wk
  .short  EV_SEQ_TRAINER_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _WILD_BTL_SET  �쐶��Ăяo��(��`��)
 * @param mons_no �G���J�E���g�������������X�^�[No 
 * @param mons_lv �G���J�E���g�������������X�^�[���x��
 *
 * ��`���Ɠ`���n�ł̓o�g���J�n���̃G���J�E���g���b�Z�[�W���ς��܂�
 * ��`���u��������яo���Ă����v ���b�Z�[�W�ω���091029���_�Ŗ�����
 */
//--------------------------------------------------------------
#define _WILD_BTL_SET( mons_no, mons_lv ) \
    _ASM_WILD_BTL_SET mons_no, mons_lv, SCR_WILD_BTL_FLAG_NONE 

//--------------------------------------------------------------
/**
 * _WILD_BTL_SET_LEGEND  �쐶��Ăяo��(�`���n)
 * @param mons_no �G���J�E���g�������������X�^�[No 
 * @param mons_lv �G���J�E���g�������������X�^�[���x��
 *
 * ��`���Ɠ`���n�ł̓o�g���J�n���̃G���J�E���g���b�Z�[�W���ς��܂�
 * �`���n�u���������ꂽ�v ���b�Z�[�W�ω���091029���_�Ŗ�����
 */
//--------------------------------------------------------------
#define _WILD_BTL_SET_LEGEND( mons_no, mons_lv ) \
    _ASM_WILD_BTL_SET mons_no, mons_lv, SCR_WILD_BTL_FLAG_LEGEND

  .macro  _ASM_WILD_BTL_SET mons_no, mons_lv, flags
  .short  EV_SEQ_WILD_BTL_SET
  .short  \mons_no
  .short  \mons_lv
  .short  \flags
  .endm

//--------------------------------------------------------------
/**
 * _CAPTURE_DEMO_SET  �ߊl�f���Z�b�g
 */
//--------------------------------------------------------------
#define _CAPTURE_DEMO_SET( ) \
    _ASM_CAPTURE_DEMO_SET 

  .macro  _ASM_CAPTURE_DEMO_SET
  .short  EV_SEQ_CAPTURE_DEMO_SET
  .endm

//--------------------------------------------------------------
/**
 * @def _WILD_WIN
 * @brief �쐶�폟������
 */
//--------------------------------------------------------------
#define _WILD_WIN() _ASM_WILD_WIN_CALL

  .macro  _ASM_WILD_WIN_CALL
    _ASM_WILD_WIN
    //_BLACK_IN() //�����Ńu���b�N�C�����Ăԗ\��
  .endm

  .macro  _ASM_WILD_WIN
  .short  EV_SEQ_WILD_WIN
  .endm

//--------------------------------------------------------------
/**
 * _WILD_LOSE  �쐶��s�k����
 * @param none
 */
//--------------------------------------------------------------
#define _WILD_LOSE() \
  _ASM_WILD_LOSE

  .macro  _ASM_WILD_LOSE
  .short  EV_SEQ_WILD_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  @def  _WILD_LOSE_CHECK
 *  @brief  �쐶�|�P������ł̔s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[��
 *  
 *  @retval SCR_BATTLE_WIN  ����
 *  @retval SCR_BATTLE_LOSE �s�k
 */
//--------------------------------------------------------------
#define  _WILD_LOSE_CHECK(  ret_wk ) \
  _ASM_WILD_LOSE_CHECK  ret_wk

  .macro  _ASM_WILD_LOSE_CHECK  ret_wk
  .short  EV_SEQ_WILD_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  @def  _WILD_RETRY_CHECK
 *  @brief  �쐶�|�P������ł̍Đ�R�[�h�`�F�b�N
 *  @param ret_wk ���ʊi�[��
 *  
 *  @retval SCR_WILD_BTL_RET_CAPTURE  �߂܂���
 *  @retval SCR_WILD_BTL_RET_ESCAPE   ������
 *  @retval SCR_WILD_BTL_RET_WIN      �|����
 *  
 *  @note ���̃R�}���h�͎g�p�ꏊ�����X�g�A�b�v���Ă��܂��B�g���ꍇ�͐ē��}�Ɏg�p�ꏊ�������Ă��������B
 */
//--------------------------------------------------------------
#define  _WILD_RETRY_CHECK(  ret_wk ) \
  _ASM_WILD_RETRY_CHECK  ret_wk

  .macro  _ASM_WILD_RETRY_CHECK  ret_wk
  .short  EV_SEQ_WILD_BTL_RETRY_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  @def _LOOK_ELBOARD
 *  @brief �d���f��������
 *  @param frame �J�����ړ��ɗv����t���[����
 */
//--------------------------------------------------------------
#define _LOOK_ELBOARD( frame ) \
    _ASM_LOOK_ELBOARD frame

  .macro  _ASM_LOOK_ELBOARD frame
  .short  EV_SEQ_LOOK_ELBOARD
  .short \frame
  .endm


#if 0
//--------------------------------------------------------------
/**
 *  _SEACRET_POKE_RETRY_CHECK �B���|�P�����Đ�s�`�F�b�N
 *  @param ret_wk ���ʊi�[�� 
 */
//--------------------------------------------------------------
  .macro  _SEACRET_POKE_RETRY_CHECK ret_wk
  .short  EV_SEQ_SEACRET_POKE_RETRY_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _HAIFU_POKE_RETRY_CHECK �z�z�|�P�����Đ�s�`�F�b�N
 *  @param ret_wk ���ʊi�[��
 */
//--------------------------------------------------------------
  .macro  _HAIFU_POKE_RETRY_CHECK ret_wk
  .short  EV_SEQ_HAIFU_POKE_RETRY_CHECK
  .short  \ret_wk
  .endm
#endif

//--------------------------------------------------------------
/**
 *  @def  _2VS2_BATTLE_CHECK
 *  @brief  �莝���`�F�b�N 2vs2���\���擾
 *  @param ret_wk ���ʊi�[��
 *  @return TRUE == 2vs2 ���ł���
 */
//--------------------------------------------------------------
#define _2VS2_BATTLE_CHECK( ret_wk ) \
    _ASM_2VS2_BATTLE_CHECK ret_wk

  .macro  _ASM_2VS2_BATTLE_CHECK ret_wk
  .short  EV_SEQ_2VS2_BATTLE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _DEBUG_BTL_SET �f�o�b�N�퓬�Ăяo��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _DEBUG_BTL_SET
  .short  EV_SEQ_DEBUG_BTL_SET
  .endm

//--------------------------------------------------------------
/**
 *  _BATTLE_RESULT_GET �퓬���ʂ��擾
 *  @param ret_wk ���ʊi�[��
 */
//--------------------------------------------------------------
  .macro  _BATTLE_RESULT_GET ret_wk
  .short  EV_SEQ_BATTLE_RESULT_GET
  .short  \ret_wk
  .endm

//======================================================================
//  �g���[�i�[�t���O
//======================================================================
//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_SET �g���[�i�[�t���O�̃Z�b�g
 *  @param tr_id �Z�b�g����g���[�i�[ID
 */
//--------------------------------------------------------------
#define _TRAINER_FLAG_SET( tr_id ) \
    _ASM_TRAINER_FLAG_SET tr_id

  .macro  _ASM_TRAINER_FLAG_SET tr_id
  .short  EV_SEQ_TRAINER_FLAG_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_RESET �g���[�i�[�t���O�̃��Z�b�g
 *  @param tr_id ���Z�b�g����g���[�i�[ID
 */
//--------------------------------------------------------------
#define _TRAINER_FLAG_RESET( tr_id ) \
    _ASM_TRAINER_FLAG_RESET tr_id

  .macro  _ASM_TRAINER_FLAG_RESET tr_id
  .short  EV_SEQ_TRAINER_FLAG_RESET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  @def  _TRAINER_FLAG_CHECK
 *  @brief  �g���[�i�[�t���O�`�F�b�N
 *  @param  tr_id   �`�F�b�N����g���[�i�[ID
 *  @param  ret_wk  �l���󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _TRAINER_FLAG_CHECK( tr_id, ret_wk )  \
    _ASM_TRAINER_FLAG_CHECK tr_id, ret_wk

  .macro  _ASM_TRAINER_FLAG_CHECK tr_id, ret_wk
  .short  EV_SEQ_TRAINER_FLAG_CHECK
  .short  \tr_id
  .short  \ret_wk
  .endm


//======================================================================
//  �T�E���h BGM
//======================================================================
//--------------------------------------------------------------
/**
  * _BGM_PLAY BGM��炷
  * @param no BGM Number
  */
//--------------------------------------------------------------
#define _BGM_PLAY(no) _ASM_BGM_PLAY no

   .macro  _ASM_BGM_PLAY no
  .short  EV_SEQ_BGM_PLAY
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_STOP BGM���~�߂�
 *  @param none
 */
//--------------------------------------------------------------
#define _BGM_STOP() _ASM_BGM_STOP

  .macro  _ASM_BGM_STOP
  .short  EV_SEQ_BGM_STOP
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_PLAYER_PAUSE �V�[�P���X���ꎞ��~�܂��͍ĊJ
 *  @param flag (1=��~�A0=�ĊJ)
 */
//--------------------------------------------------------------
#define _BGM_PLAYER_PAUSE(flag) _ASM_BGM_PLAYER_PAUSE

  .macro  _ASM_BGM_PLAYER_PAUSE flag
  .short  EV_SEQ_BGM_PLAYER_PAUSE
  .byte  \flag
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_PLAY_CHECK �a�f�l�Ȃ��Ă��邩�`�F�b�N
 *  @param  music   BGM�i���o�[
 *  @param  ret     TRUE�Ȃ��Ă���
 */
//--------------------------------------------------------------
#define _BGM_PLAY_CHECK(music, ret) _ASM_BGM_PLAY_CHECK music, ret

  .macro  _ASM_BGM_PLAY_CHECK music, ret
  .short  EV_SEQ_BGM_PLAY_CHECK
  .short music
  .short ret
  .endm


//--------------------------------------------------------------
/**
 *  _BGM_FADEOUT BGM�t�F�[�h�A�E�g
 *  @param vol    = �{�����[��
 *  @param frame  = ���t���[�������ăt�F�[�h���邩
 */
//--------------------------------------------------------------
#define _BGM_FADEOUT(vol, frame)  _ASM_BGM_FADEOUT vol, frame

  .macro  _ASM_BGM_FADEOUT vol,frame
  .short  EV_SEQ_BGM_FADEOUT
  .short  \vol
  .short  \frame
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_FADEIN BGM�t�F�[�h�C��
 *  @param frame = ���t���[�������ăt�F�[�h���邩
 */
//--------------------------------------------------------------
#define _BGM_FADEIN(frame)  _ASM_BGM_FADEIN frame

  .macro  _ASM_BGM_FADEIN frame
  .short  EV_SEQ_BGM_FADEIN
  .short  \frame
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_NOW_MAP_RECOVER ���݂̃}�b�v��BGM��炷
 *  @param none
 */
//--------------------------------------------------------------
#define _BGM_NOW_MAP_RECOVER()  _ASM_BGM_NOW_MAP_RECOVER

  .macro  _ASM_BGM_NOW_MAP_RECOVER
  .short  EV_SEQ_BGM_NOW_MAP_RECOVER
  .endm

//--------------------------------------------------------------
/**
  * _START_TEMP_EVENT_BGM �ꎞ�I�ȃC�x���gBGM���Đ�����
  * @param no BGM Number
  *
  * �t�B�[���hBGM��ޔ����A�C�x���gBGM���Đ����܂��B
  * ���̏�ԂŃo�g�����畜�A����ƁA�t�B�[���hBGM���Đ�����܂��B
  *
  * �t�B�[���hBGM �� �C�x���gBGM �� �o�g�� �� �t�B�[���hBGM
  * �̗��ꂪ�K�v�ȏ�ʂł̂ݎg�p���Ă��������B
 */
//--------------------------------------------------------------
#define _START_TEMP_EVENT_BGM( no ) \
        _ASM_START_TEMP_EVENT_BGM no

  .macro  _ASM_START_TEMP_EVENT_BGM no
  .short  EV_SEQ_START_TEMP_EVENT_BGM
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_WAIT 
 */
//--------------------------------------------------------------
#define _BGM_WAIT() _ASM_BGM_WAIT

  .macro  _ASM_BGM_WAIT
  .short  EV_SEQ_BGM_WAIT
  .endm

//--------------------------------------------------------------
/**
 * @def _START_EVENT_BGM
 * @brief �X�N���v�g�ȈՃ}�N���F�C�x���gBGM�J�n
 * @param bgmno
 *
 * @note
 * �����I�ɂ� _BGM_PLAY()���Ă�ł���
 */
//--------------------------------------------------------------
#define _START_EVENT_BGM( bgmno ) \
    _ASM_START_EVENT_BGM bgmno

  .macro _ASM_START_EVENT_BGM bgmno
//  _ASM_BGM_STOP
  _ASM_BGM_PLAY \bgmno
  .endm

//--------------------------------------------------------------
/**
 * @def _END_EVENT_BGM
 * @brief �X�N���v�g�ȈՃ}�N���F�C�x���gBGM�I��
 *
 * @note
 * �����I�ɂ́A_BGM_NOW_MAP_RECOVER()���Ă�ł���
 */
//--------------------------------------------------------------
#define _END_EVENT_BGM() \
    _ASM_END_EVENT_BGM

  .macro  _ASM_END_EVENT_BGM
  //_ASM_BGM_FADEOUT  0, 30
  //_ASM_BGM_STOP
  _ASM_BGM_NOW_MAP_RECOVER
  .endm

//======================================================================
//  �T�E���h ISS
//======================================================================

//--------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�@�w��X�C�b�`�� ON �ɂ���
 * @def _ISS_SWITCH_ON 
 * @param idx ON �ɂ���X�C�b�`�ԍ�(0�`8)
 */
//--------------------------------------------------------------
#define _ISS_SWITCH_ON( idx ) \
    _ASM_ISS_SWITCH_ON idx

  .macro  _ASM_ISS_SWITCH_ON idx
  .short  EV_SEQ_ISS_SWITCH_ON
  .short  \idx
  .endm

//--------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�@�w��X�C�b�`�� OFF �ɂ���
 * @def _ISS_SWITCH_OFF
 * @param idx OFF �ɂ���X�C�b�`�ԍ�(0�`8)
 */
//--------------------------------------------------------------
#define _ISS_SWITCH_OFF( idx ) \
    _ASM_ISS_SWITCH_OFF idx

  .macro  _ASM_ISS_SWITCH_OFF idx
  .short  EV_SEQ_ISS_SWITCH_OFF
  .short  \idx
  .endm

//--------------------------------------------------------------
/**
 * @brief �X�C�b�`ISS�@�w��X�C�b�`�� ON/OFF �𒲂ׂ�
 * @def _ISS_SWITCH_CHECK
 * @param ret_wk ���茋�ʂ̊i�[��
 * @param idx    OFF �ɂ���X�C�b�`�ԍ�(0�`8)
 * @return �w��X�C�b�`�� ON �Ȃ� TRUE, �����łȂ���� FALSE
 */
//--------------------------------------------------------------
#define _ISS_SWITCH_CHECK( ret_wk, idx ) \
    _ASM_ISS_SWITCH_CHECK ret_wk, idx

  .macro  _ASM_ISS_SWITCH_CHECK ret_wk, idx
  .short  EV_SEQ_ISS_SWITCH_CHECK
  .short  \ret_wk
  .short  \idx
  .endm


//======================================================================
//  �T�E���h SE
//======================================================================
//--------------------------------------------------------------
/**
 *  _SE_PLAY SE��炷
 *  @param no �Đ�����SE�i���o�[
 */
//-------------------------------------------------------------
#define   _SE_PLAY( no )  \
    _ASM_SE_PLAY no

.macro  _ASM_SE_PLAY  no
.short  EV_SEQ_SE_PLAY
.short  \no
.endm

//--------------------------------------------------------------
/**
 *  _SE_STOP SE���~�߂�
 *  @param none
 */
//--------------------------------------------------------------
#define   _SE_STOP() \
  .short  EV_SEQ_SE_STOP

.macro  _ASM_SE_STOP
.short  EV_SEQ_SE_STOP
.endm
//--------------------------------------------------------------
/**
 *  _SE_WAIT SE�I���҂�
 *  @param none
 */
//--------------------------------------------------------------
#define   _SE_WAIT()  _ASM_SE_WAIT

.macro  _ASM_SE_WAIT
.short  EV_SEQ_SE_WAIT
.endm

//======================================================================
//  �T�E���h ME
//======================================================================
//--------------------------------------------------------------
/**
 *  _ME_PLAY ME��炷
 *  @param no �Đ�����BGM�i���o�[
 */
//--------------------------------------------------------------
#define _ME_PLAY( no ) _ASM_ME_PLAY no

.macro  _ASM_ME_PLAY  no
  .short  EV_SEQ_ME_PLAY
  .short  \no
.endm

//--------------------------------------------------------------
/**
 *  _ME_WAIT ME�I���҂�
 *  @param none
 */
//--------------------------------------------------------------
#define _ME_WAIT() _ASM_ME_WAIT

.macro  _ASM_ME_WAIT
  .short  EV_SEQ_ME_WAIT
.endm

//======================================================================
//  �T�E���h ����
//======================================================================
//--------------------------------------------------------------
/**
 * �|�P���������Đ�
 *
 * @param monsno  �|�P�����i���o�[
 * @param formno  �t�H�����i���o�[�i���ɂȂ��ꍇ�A�O���w��j
 */
//--------------------------------------------------------------
#define _VOICE_PLAY( monsno, formno ) \
    _ASM_VOICE_PLAY monsno, formno

  .macro  _ASM_VOICE_PLAY monsno, formno
  .short  EV_SEQ_VOICE_PLAY
  .short  \monsno
  .byte  0
  .byte  0
  .endm

//--------------------------------------------------------------
/**
 * �|�P���������Đ�
 */
//--------------------------------------------------------------
#define _VOICE_WAIT() \
    _ASM_VOICE_WAIT

  .macro  _ASM_VOICE_WAIT
  .short  EV_SEQ_VOICE_WAIT
  .endm

//======================================================================
//  ���j���[
//======================================================================
//--------------------------------------------------------------
/**
 *  _BMPMENU_INIT BMP���j���[�@������
 *  @param x ���j���[����X���W �L�����P��
 *  @param y ���j���[����Y���W �L�����P��
 *  @param cursor �J�[�\�������ʒu
 *  @param cancel B�{�^���L�����Z���L���t���O 1=�L�� 0=����
 *  @param ret_wk ���ʊi�[�惏�[�NID
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT( x,y,cursor,cancel,ret_wk ) \
    _ASM_BMPMENU_INIT x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT
  .byte  \x
  .byte  \y
  .short \cursor
  .byte  \cancel
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_INIT_EX BMP���j���[�@������(�ǂݍ���ł���gmm�t�@�C�����g�p����)
 *  @param x ���j���[����X���W �L�����P��
 *  @param y ���j���[����Y���W �L�����P��
 *  @param cursor �J�[�\�������ʒu
 *  @param cancel B�{�^���L�����Z���L���t���O 1=�L�� 0=����
 *  @param ret_wk ���ʊi�[�惏�[�NID
 *  @note ���W�w��͒�`�����g���܂��BMENU_X1,MENU_Y1 �ڍׂ�user_define.h���Q�ƁB
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT_EX( x,y,cursor,cancel,ret_wk ) \
   _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT_EX
  .byte  \x
  .byte  \y
  .short \cursor
  .byte  \cancel
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_INIT_EX BMP���j���[�@������(�E�l�A�ǂݍ���ł���gmm�t�@�C�����g�p����)
 *  @param x ���j���[����X���W �L�����P��
 *  @param y ���j���[����Y���W �L�����P��
 *  @param cursor �J�[�\�������ʒu
 *  @param cancel B�{�^���L�����Z���L���t���O 1=�L�� 0=����
 *  @param ret_wk ���ʊi�[�惏�[�NID
 *  @note ���W�w��͒�`�����g���܂��BMENU_X1,MENU_Y1 �ڍׂ�user_define.h���Q�ƁB
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT_RIGHT( x,y,cursor,cancel,ret_wk ) \
   _ASM_BMPMENU_INIT_RIGHT x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT_RIGHT x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT_EX_RIGHT
  .byte  \x
  .byte  \y
  .short \cursor
  .byte  \cancel
  .short  \ret_wk
  .endm


//--------------------------------------------------------------
/**
 *  _BMPMENU_MAKE_LIST BMP���j���[�@���X�g�쐬
 *  @param msg_id ���b�Z�[�WID
 *  @param param ���b�Z�[�W���I�����ꂽ�ۂɌ��ʂƂ��ĕԂ��l
 */
//--------------------------------------------------------------
#define _BMPMENU_MAKE_LIST( msg_id, param ) \
    _ASM_BMPMENU_MAKE_LIST msg_id, SCR_BMPMENU_EXMSG_NULL, param

//--------------------------------------------------------------
/**
 *  _BMPMENU_MAKE_LIST BMP���j���[�@���X�g�쐬�g����
 *  @param msg_id ���b�Z�[�WID
 *  @param ex_msg_id  �e���ڂ̐������b�Z�[�WID
 *  @param param ���b�Z�[�W���I�����ꂽ�ۂɌ��ʂƂ��ĕԂ��l
 *
 *  ex_msg���e���ڑI�����ɃE�B���h�E�ɕ`�悵�܂�
 *  09.12.08���� SYSWIN����
 */
//--------------------------------------------------------------
#define _BMPMENU_MAKE_LIST_EX( msg_id, ex_msg_id, param ) \
    _ASM_BMPMENU_MAKE_LIST msg_id, ex_msg_id, param

  .macro  _ASM_BMPMENU_MAKE_LIST msg_id,ex_msg_id,param
  .short  EV_SEQ_BMPMENU_MAKE_LIST
  .short  \msg_id
  .short  \ex_msg_id 
  .short  \param
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_START BMP���j���[�@�J�n
 *  @param none
 */
//--------------------------------------------------------------
#define _BMPMENU_START() _ASM_BMPMENU_START

  .macro  _ASM_BMPMENU_START
  .short  EV_SEQ_BMPMENU_START
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_START BMP���j���[�@�J�n(x�{�^���ɂ�钆�f����)
 *  @param none
 */
//--------------------------------------------------------------
#define _BMPMENU_START_BREAKABLE() _ASM_BMPMENU_START_BREAKABLE

  .macro  _ASM_BMPMENU_START_BREAKABLE
  .short  EV_SEQ_BMPMENU_START_BREAKABLE
  .endm

//======================================================================
//  ��ʃt�F�[�h
//======================================================================
//--------------------------------------------------------------
//  �t�F�[�h���[�h(fade.h)
//--------------------------------------------------------------
#define DISP_FADE_BLACKOUT_MAIN (0x01) ///<���C����ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_BLACKOUT_SUB  (0x02) ///<�T�u��ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_BLACKOUT (0x03) ///<����ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_WHITEOUT_MAIN (0x04) ///<���C����ʃt�F�[�h �z���C�g�A�E�g
#define DISP_FADE_WHITEOUT_SUB  (0x08) ///<�T�u��ʃt�F�[�h �z���C�g�A�E�g
#define DISP_FADE_WHITEOUT (0x0c) ///<����ʃt�F�[�h �z���C�g�A�E�g

//--------------------------------------------------------------
/**
 *  _DISP_FADE_START ��ʃt�F�[�h�X�^�[�g
 *  @param mode ��ʃt�F�[�h���[�h 
 *  @param start_evy �����P�x(0=���̐F�`16=�w��P�x�j
 *  @param end_evy �I���P�x(0=���̐F�`16=�w��P�x)
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
#define _DISP_FADE_START( mode,start_evy,end_evy,speed ) _ASM_DISP_FADE_START mode,start_evy,end_evy,speed

  .macro  _ASM_DISP_FADE_START mode,start_evy,end_evy,speed
  .short EV_SEQ_DISP_FADE_START
  .short \mode
  .short \start_evy
  .short \end_evy
  .short \speed
  .endm


//--------------------------------------------------------------
/**
 *  _DISP_FADE_CHECK ��ʃt�F�[�h�I���`�F�b�N
 *  @param none
 */
//--------------------------------------------------------------
#define _DISP_FADE_END_CHECK()  _ASM_DISP_FADE_END_CHECK

  .macro  _ASM_DISP_FADE_END_CHECK
  .short EV_SEQ_DISP_FADE_CHECK
  .endm


//--------------------------------------------------------------
/**
 *  @def  _WHITE_OUT
 *  @brief  �z���C�g�A�E�g
 *  @param none
 *  @note _DISP_FADE_END_CHECK()�ŏI���҂������Ă�������
 */
//--------------------------------------------------------------
#define _WHITE_OUT() \
    _DISP_FADE_START(DISP_FADE_WHITEOUT,0,16,DISP_FADE_SPEED)

//--------------------------------------------------------------
/**
 *  @def  _WHITE_IN
 *  @brief  �z���C�g�C��
 *  @param none
 */
//--------------------------------------------------------------
#define _WHITE_IN() \
    _DISP_FADE_START(DISP_FADE_WHITEOUT,16,0,DISP_FADE_SPEED)

//--------------------------------------------------------------
/**
 *  @def  _BLACK_OUT
 *  @brief  �u���b�N�A�E�g
 *  @param  none
 *  @note _DISP_FADE_END_CHECK()�ŏI���҂������Ă�������
 */
//--------------------------------------------------------------
#define _BLACK_OUT() \
    _DISP_FADE_START(DISP_FADE_BLACKOUT,0,16,DISP_FADE_SPEED)
  
//--------------------------------------------------------------
/**
 *  @def  _BLACK_IN
 *  @brief  �u���b�N�C��
 *  @param  none
 */
//--------------------------------------------------------------
#define _BLACK_IN() \
    _DISP_FADE_START(DISP_FADE_BLACKOUT,16,0,DISP_FADE_SPEED)

//======================================================================
//  �A�C�e���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *	�A�C�e����������
 *	@param item_no ������A�C�e���i���o�[
 *	@param num �������
 *	@param ret_wk ���ʑ���� TRUE=���� FALSE=���s
 *	@note ���̃R�}���h�̓A�C�e����ǉ����邾���̏����ł��B
 *	�X�N���v�g�ǂ̐l�͓��胁�b�Z�[�W�����p�b�P�[�W���ꂽ _ITEM_EVENT_KEYWAIT ���g���Ă��������B
 */
//--------------------------------------------------------------
#define _ADD_ITEM( item_no, num, ret_wk )  _ASM_ADD_ITEM item_no,num,ret_wk

	.macro	_ASM_ADD_ITEM	item_no,num,ret_wk
	.short	EV_SEQ_ADD_ITEM
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	�A�C�e�������炷
 *	@param item_no ���炷�A�C�e���i���o�[
 *	@param num ���炷��
 *	@param ret_wk ���ʑ���� TRUE=���� FALSE=���s
 */
//--------------------------------------------------------------
#define _SUB_ITEM( item_no,num,ret_wk ) _ASM_SUB_ITEM item_no,num,ret_wk

	.macro	_ASM_SUB_ITEM item_no,num,ret_wk
	.short	EV_SEQ_SUB_ITEM
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	�A�C�e�����������邩�`�F�b�N
 *	@param item_no ������A�C�e���i���o�[
 *	@param num �������
 *	@param ret_wk ���ʑ���� TRUE=�\ FALSE=�s�\
 */
//--------------------------------------------------------------
#define _ADD_ITEM_CHK( item_no, num, ret_wk ) _ASM_ADD_ITEM_CHK item_no,num,ret_wk

	.macro	_ASM_ADD_ITEM_CHK	item_no,num,ret_wk
	.short	EV_SEQ_ADD_ITEM_CHK
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	�o�b�O�̃A�C�e���`�F�b�N
 *	@param item_no �`�F�b�N����A�C�e���i���o�[
 *	@param num �`�F�b�N�����
 *	@param ret_wk ���ʑ���� TRUE=���� FALSE=���݂��Ȃ�
 */
//--------------------------------------------------------------
#define _ITEM_CHK( item_no, num, ret_wk ) _ASM_ITEM_CHK item_no,num,ret_wk

	.macro	_ASM_ITEM_CHK		item_no,num,ret_wk
	.short	EV_SEQ_ITEM_CHK
	.short	\item_no
	.short	\num
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	�o�b�O�̃A�C�e�����擾
 *	@param item_no �擾����A�C�e���i���o�[
 *	@param ret_wk ���ʑ����
 */
//--------------------------------------------------------------
#define _GET_ITEM_NUM( item_no, ret_wk ) _ASM_GET_ITEM_NUM		item_no,ret_wk

	.macro	_ASM_GET_ITEM_NUM		item_no,ret_wk
	.short	EV_SEQ_GET_ITEM_NUM
	.short	\item_no
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 *	�Z�}�V���̃A�C�e���i���o�[���`�F�b�N
 *	@param item_no �`�F�b�N����A�C�e���i���o�[
 *	@param ret_wk ���ʑ����
 *	@retval TRUE=�Z�}�V���ł���
 *	@retval FALSE=�Z�}�V���ł͂Ȃ�
 */
//--------------------------------------------------------------
#define _WAZA_ITEM_CHK( item_no,ret_wk ) _ASM_WAZA_ITEM_CHK item_no,ret_wk

	.macro	_ASM_WAZA_ITEM_CHK		item_no,ret_wk
	.short	EV_SEQ_WAZA_ITEM_CHK
	.short	\item_no
	.short	\ret_wk
	.endm

//--------------------------------------------------------------
/**
 * @def _GET_POCKET_ID
 * @brief �|�P�b�gID�̎擾����
 * @param item_no   �A�C�e���i���o�[
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 *
 * item_no�Ŏw�肵���A�C�e���ɑΉ������o�b�O�̃|�P�b�gID���擾����
 */
//--------------------------------------------------------------
#define _GET_POCKET_ID( item_no, ret_wk )   \
    _ASM_GET_POCKET_ID item_no, ret_wk


    .macro  _ASM_GET_POCKET_ID item_no, ret_wk
    .short  EV_SEQ_GET_POCKET_ID
    .short  \item_no
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ITEM_TYPE
 * @brief �w��A�C�e���̎�ނ��擾����
 * @param item_no   �A�C�e���i���o�[
 * @param ret_wk    ���ʂ��󂯎�郏�[�N  itemtype_def.h�Q��
 *
 * item_no�Ŏw�肵���A�C�e���ɑΉ������A�C�e���^�C�v���擾����
 */
//--------------------------------------------------------------
#define _GET_ITEM_TYPE( item_no, ret_wk )   \
    _ASM_GET_ITEM_TYPE item_no, ret_wk


    .macro  _ASM_GET_ITEM_TYPE item_no, ret_wk
    .short  EV_SEQ_GET_ITEM_TYPE
    .short  \item_no
    .short  \ret_wk
    .endm
    
//--------------------------------------------------------------
/**
 * @def _GET_EFFECT_ENCOUNT_ITEM
 * @brief �G�t�F�N�g�G���J�E���g�A�C�e���C�x���g�@�A�C�e��No���擾
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _GET_EFFECT_ENCOUNT_ITEM( ret_wk )   \
    _ASM_GET_EFFECT_ENCOUNT_ITEM ret_wk


    .macro  _ASM_GET_EFFECT_ENCOUNT_ITEM ret_wk
    .short  EV_SEQ_GET_EFFECT_ENCOUNT_ITEM
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * �t�B�[���h�̃A�C�e���C�x���g�Ăяo��
 * @param no   �A�C�e���i���o�[
 * @param num ������l
 */
//--------------------------------------------------------------
.macro _FLD_ITEM_EVENT no,num
  _ASM_LDVAL SCWK_REG0,\no
  _ASM_LDVAL SCWK_REG1,\num
  _CALL ev_fld_item2
.endm

//======================================================================
//  ��b�C�x���g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALK_START_SE_PLAY
 *  ��b�C�x���g�J�n����SE�Đ�
 *  @param none
 */
//--------------------------------------------------------------
#define _TALK_START_SE_PLAY() _ASM_TALK_START_SE_PLAY

  .macro  _ASM_TALK_START_SE_PLAY
  _ASM_SE_PLAY( SEQ_SE_MESSAGE )
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_START
 *  @brief  OBJ�ɑ΂��Ęb���|���J�n(��l���ɑ΂��ĐU������L��)
 *
 * @note
 *  �����I�ɂ�_TALK_START_SE_PLAY()��_TURN_HERO_SITE()���Ăяo���̂Ɠ��@�\�B
 *  ����̉�b�֘A�̋@�\�g���ɔ����āA������Ăяo���悤�ɂ��Ă����B
 */
//--------------------------------------------------------------
#define _TALK_OBJ_START() _ASM_TALK_OBJ_START

.macro  _ASM_TALK_OBJ_START
    _ASM_TALK_START_SE_PLAY
    _ASM_TURN_HERO_SITE
.endm

//--------------------------------------------------------------
/**
 * @def _TALK_START_TURN_NOT
 * @brief  OBJ�ɑ΂��Ęb���|���J�n(��l���ɑ΂��ĐU������Ȃ��j
 *
 * @note
 *  �����I�ɂ�_TALK_START_SE_PLAY()���Ăяo���̂Ɠ��@�\�B
 *  ����̉�b�֘A�̋@�\�g���ɔ����āA������Ăяo���悤�ɂ��Ă����B
 */
//--------------------------------------------------------------
#define _TALK_START_TURN_NOT() _TALK_START_SE_PLAY()


//======================================================================
//  �~���[�W�J���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _MUSICAL_CALL �~���[�W�J���F�~���[�W�J���Ăяo��
 *  @param pos �I���ʒu
 *  @param mode ���[�h (0:��ʐM 1:�ʐM 2:�~���[�W�J���V���b�g
 */
//--------------------------------------------------------------
#define _MUSICAL_CALL( pos, mode ) \
    _ASM_MUSICAL_CALL pos, mode

  .macro  _ASM_MUSICAL_CALL pos, mode
  .short EV_SEQ_MUSICAL_CALL
  .short \pos
  .byte \mode
  .endm

//--------------------------------------------------------------
/**
 *  _MUSICAL_CALL �~���[�W�J���F�~���[�W�J���V���b�g�Ăяo��
 */
//--------------------------------------------------------------
#define _MUSICAL_CALL_MUSICAL_SHOT() \
    _ASM_MUSICAL_CALL_MUSICAL_SHOT

  .macro  _ASM_MUSICAL_CALL_MUSICAL_SHOT
  .short EV_SEQ_MUSICAL_CALL
  .short 0
  .byte 2
  .endm

//--------------------------------------------------------------
/**
 *  _MUSICAL_FITTING_CALL �~���[�W�J���F�������Ăяo��
 *  @param pos �I���ʒu
 */
//--------------------------------------------------------------
#define _MUSICAL_FITTING_CALL( pos ) \
    _ASM_MUSICAL_FITTING_CALL pos

  .macro  _ASM_MUSICAL_FITTING_CALL pos
  .short EV_SEQ_MUSICAL_FITTING_CALL
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE �~���[�W�J���F�~���[�W�J�����l�擾(�ėp
 *      ��{�I�ɉ��ɂ��郉�b�p�[���Ă�ł�������
 *  @param type ���(�Q���񐔁E�g�b�v�񐔁E�ŏI�]���_�E�ŏI�R���f�B�V����
 *  @param val  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE( type, val, retVal ) \
    _ASM_GET_MUSICAL_VALUE type, val, retVal

  .macro  _ASM_GET_MUSICAL_VALUE type, val, retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte \type
  .short \val
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_JOIN_NUM �~���[�W�J���F�Q���񐔎擾
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_JOIN_NUM( retVal ) \
    _ASM_GET_MUSICAL_VALUE_JOIN_NUM retVal

  .macro  _ASM_GET_MUSICAL_VALUE_JOIN_NUM retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 0
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_TOP_NUM �~���[�W�J���F�g�b�v��
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_TOP_NUM( retVal ) \
    _ASM_GET_MUSICAL_VALUE_TOP_NUM retVal

  .macro  _ASM_GET_MUSICAL_VALUE_TOP_NUM retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 1
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_LAST_POINT �~���[�W�J���F�ŏI�]���_
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_LAST_POINT( retVal ) \
    _ASM_GET_MUSICAL_VALUE_LAST_POINT retVal

  .macro  _ASM_GET_MUSICAL_VALUE_LAST_POINT retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 2
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_LAST_CONDITION �~���[�W�J���F�ŏI�R���f�B�V����
 *                                      �ō��������̏ꍇ��MUSICAL_CONDITION_NONE��Ԃ��܂�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_LAST_CONDITION( retVal ) \
    _ASM_GET_MUSICAL_VALUE_LAST_CONDITION retVal

  .macro  _ASM_GET_MUSICAL_VALUE_LAST_CONDITION retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 3
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_SELECT_PROGRAM �~���[�W�J���F�I�����ڔԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_SELECT_PROGRAM( retVal ) \
    _ASM_GET_MUSICAL_VALUE_SELECT_PROGRAM retVal

  .macro  _ASM_GET_MUSICAL_VALUE_SELECT_PROGRAM retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 4
  .short 0
  .short \retVal
  .endm
  
//--------------------------------------------------------------
/**
 *  _SET_MUSICAL_VALUE_SELECT_PROGRAM �~���[�W�J���F�I�����ڔԍ��ݒ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _SET_MUSICAL_VALUE_SELECT_PROGRAM( setVal ) \
    _ASM_SET_MUSICAL_VALUE_SELECT_PROGRAM setVal

  .macro  _ASM_SET_MUSICAL_VALUE_SELECT_PROGRAM setVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 5
  .short \setVal
  .short 0
  .endm
  
//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_IS_EQUIP_ANY �~���[�W�J���F�����O�b�Y�����邩�`�F�b�N
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_IS_EQUIP_ANY( retVal ) \
    _ASM_GET_MUSICAL_VALUE_IS_EQUIP_ANY retVal

  .macro  _ASM_GET_MUSICAL_VALUE_IS_EQUIP_ANY retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 6
  .short 0
  .short \retVal
  .endm

  
//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_IS_EQUIP_ANY �~���[�W�J���F�~���[�W�J���V���b�g���L���H
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_IS_ENABLE_SHOT( retVal ) \
    _ASM_GET_MUSICAL_VALUE_IS_ENABLE_SHOT retVal

  .macro  _ASM_GET_MUSICAL_VALUE_IS_ENABLE_SHOT retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 7
  .short 0
  .short \retVal
  .endm


//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_LAST_CONDITION_MIN �~���[�W�J���F�ŏI�R���f�B�V����(�Ⴂ
 *                  �ŒႪ�����̏ꍇ�A��`���̌�납��D��I�ɕԂ��܂��B
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_LAST_CONDITION_MIN( retVal ) \
    _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_MIN retVal

  .macro  _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_MIN retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 8
  .short 0
  .short \retVal
  .endm


//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_LAST_CONDITION_MAX �~���[�W�J���F�ŏI�R���f�B�V����(����
 *                  �ō��������̏ꍇ�A��`���̂ɗD��I�ɕԂ��܂��B
 *                  ���ׂē����_�������ꍇ�AMUSICAL_CONDITION_BALANCE
 *                  �����������Ă��Ȃ������ꍇ�AMUSICAL_CONDITION_NONE
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_LAST_CONDITION_MAX( retVal ) \
    _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_MAX retVal

  .macro  _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_MAX retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 9
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_LAST_CONDITION_2ND �~���[�W�J���F�ŏI�R���f�B�V����(�Q��
 *                  �Q�ʂ������̏ꍇ�AMUSICAL_CONDITION_NONE
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_LAST_CONDITION_2ND( retVal ) \
    _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_2ND retVal

  .macro  _ASM_GET_MUSICAL_VALUE_LAST_CONDITION_2ND retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 10
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_COUNT_MUSICAL_POKE �~���[�W�J���F�Q���\�ȃ|�P�����̐�
 *  @param ret_val  �߂�l  (�O�Ȃ�S���Q���ł��Ȃ��I
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_COUNT_MUSICAL_POKE( retVal ) \
    _ASM_GET_MUSICAL_VALUE_COUNT_MUSICAL_POKE retVal

  .macro  _ASM_GET_MUSICAL_VALUE_COUNT_MUSICAL_POKE retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 11
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_CHECK_MUSICAL_POKE �~���[�W�J���F�Q���|�P�������Q���\���H
 *  @param ret_val  �߂�l  (�O�ł��Ȃ��I:�P�ł���I
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_CHECK_MUSICAL_POKE( pos, retVal ) \
    _ASM_GET_MUSICAL_VALUE_CHECK_MUSICAL_POKE pos, retVal

  .macro  _ASM_GET_MUSICAL_VALUE_CHECK_MUSICAL_POKE pos, retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 12
  .short \pos
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP �~���[�W�J���F�����������Ă��邩�H(�������L��
 *  @param ret_val  �߂�l  (�O�Ȃ�:�P����
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP( retVal ) \
    _ASM_GET_MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP retVal

  .macro  _ASM_GET_MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte 13
  .short 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE �~���[�W�J���F�~���[�W�J�����l�擾(�t�@���p
 *      ��{�I�ɉ��ɂ��郉�b�p�[���Ă�ł�������
 *  @param pos  �����ʒu(0�`4
 *  @param type ���(�����ځE����Msg�E�v���[���gMsg�E�v���[���g��ށE�v���[���g�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE( pos, type, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE pos, type, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE pos, type, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte \type
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE_TYPE �~���[�W�J���F�t�@��������
 *  @param pos  �����ʒu(0�`4
 *  @param ret_val  �߂�l(0�ŋ��Ȃ��E����ȊO��OBJID
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE_TYPE( pos, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE_TYPE pos, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE_TYPE pos, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE_CHEER_MSG �~���[�W�J���F�t�@���������b�Z�[�W
 *  @param pos  �����ʒu(0�`4
 *  @param ret_val  �߂�l(���b�Z�[�WID
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE_CHEER_MSG( pos, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE_CHEER_MSG pos, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE_CHEER_MSG pos, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte 1
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE_GIFT_MSG �~���[�W�J���F�t�@�����蕨���b�Z�[�W
 *  @param pos  �����ʒu(0�`4
 *  @param ret_val  �߂�l(���b�Z�[�WID
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE_GIFT_MSG( pos, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE_GIFT_MSG pos, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE_GIFT_MSG pos, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte 2
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE_GIFT_TYPE �~���[�W�J���F�t�@�����蕨���
 *  @param pos  �����ʒu(0�`4
 *  @param ret_val  �߂�l(0�����E1�O�b�Y�E2�ʏ�A�C�e��
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE_GIFT_TYPE( pos, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE_GIFT_TYPE pos, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE_GIFT_TYPE pos, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte 3
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_FAN_VALUE_GIFT_NUMBER �~���[�W�J���F�t�@�����蕨�ԍ�
 *  @param pos  �����ʒu(0�`4
 *  @param ret_val  �߂�l(�O�b�Y(�A�C�e��)�ԍ�
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_FAN_VALUE_GIFT_NUMBER( pos, retVal ) \
    _ASM_GET_MUSICAL_FAN_VALUE_GIFT_NUMBER pos, retVal

  .macro  _ASM_GET_MUSICAL_FAN_VALUE_GIFT_NUMBER pos, retVal
  .short EV_SEQ_GET_MUSICAL_FAN_VALUE
  .short \pos
  .byte 4
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_MUSICAL_WAITROOM_VALUE �~���[�W�J���F�~���[�W�J�����l�擾(�T�����p
 *  @param type ���(����J
 *  @param val  �擾�p�ԍ� (����J
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_MUSICAL_WAITROOM_VALUE( type, val, retVal ) \
    _ASM_GET_MUSICAL_WAITROOM_VALUE type, val, retVal

  .macro  _ASM_GET_MUSICAL_WAITROOM_VALUE type, val, retVal
  .short EV_SEQ_GET_MUSICAL_VALUE
  .byte \type
  .short \val
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _ADD_MUSICAL_GOODS �~���[�W�J���F�O�b�Y�ǉ�
 *  @param no �O�b�Y�ԍ�
 */
//--------------------------------------------------------------
#define _ADD_MUSICAL_GOODS( no ) \
    _ASM_ADD_MUSICAL_GOODS no

  .macro  _ASM_ADD_MUSICAL_GOODS no
  .short EV_SEQ_ADD_MUSICAL_GOODS
  .short \no
  .endm

//--------------------------------------------------------------
/**
 *  _RESET_MUSICAL_FAN_GIFT_FLG �~���[�W�J���F�t�@�����蕨�t���O���Ƃ�
 *  @param pos  �����ʒu(0�`4
 */
//--------------------------------------------------------------
#define _RESET_MUSICAL_FAN_GIFT_FLG( pos ) \
    _ASM_RESET_MUSICAL_FAN_GIFT_FLG pos

  .macro  _ASM_RESET_MUSICAL_FAN_GIFT_FLG pos, retVal
  .short EV_SEQ_RESET_MUSICAL_FAN_GIFT_FLG
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 *  _SELECT_MUSCAL_POKE �~���[�W�J���F�~���[�W�J���Q���|�P�I��
 *  @param decide  �߂�l(�O�L�����Z���F�P����
 *  @param pos     �߂�l(�Q���ԍ�
 */
//--------------------------------------------------------------
#define _SELECT_MUSCAL_POKE( decide , pos ) \
    _ASM_SELECT_MUSCAL_POKE  decide , pos

  .macro  _ASM_SELECT_MUSCAL_POKE decide, pos
  .short EV_SEQ_SELECT_MUSICAL_POKE
  .short \decide
  .short \pos
  .endm


//--------------------------------------------------------------
/**
 *  _MUSICAL_TOOLS �~���[�W�J���F�ėp�c�[��(�ʐM�N���Ƃ�
 *  @param val1  �擾�p�ԍ�
 *  @param val2  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _MUSICAL_TOOLS( type, val1, val2, ret_val ) \
    _ASM_MUSICAL_TOOLS  type, val1, val2, ret_val 

  .macro  _ASM_MUSICAL_TOOLS  type, val1, val2, ret_val 
  .short EV_SEQ_MUSICAL_TOOLS
  .short \type
  .short \val1
  .short \val2
  .short \ret_val
  .endm



//======================================================================
//  ���|�[�g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @def _REPORT_CALL
 * @brief   �Z�[�u�@�\�Ăяo��
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 *
 * ���ʃX�N���v�g�̃��|�[�g�@�\�Ăяo�����痘�p����Ă���B
 * �ʏ�̃X�N���v�g���璼�ڌĂяo�����Ƃ͂Ȃ��B
 */
//--------------------------------------------------------------
#define _REPORT_CALL(ret_wk)  \
    _ASM_REPORT_CALL  ret_wk

  .macro  _ASM_REPORT_CALL  ret_wk
  .short  EV_SEQ_REPORT_CALL
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _REPORT_CHECK
 * @brief �Z�[�u�󋵂̊m�F
 * @param ret_exist  [out]�Z�[�u�f�[�^�����݂��邩�ǂ���
 * @param ret_new    [out]���݂̃v���C�f�[�^���V�K�f�[�^���ǂ���
 * @param ret_volume [out]�Z�[�u����f�[�^��
 */
//--------------------------------------------------------------
#define _REPORT_CHECK( ret_exist, ret_new, ret_volume )  \
    _ASM_REPORT_CHECK  ret_exist, ret_new, ret_volume 

  .macro  _ASM_REPORT_CHECK ret_exist, ret_new, ret_volume 
  .short  EV_SEQ_REPORT_CHECK
  .short  \ret_exist
  .short  \ret_new
  .short  \ret_volume 
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_FIELD_COMM_NO
 * @brief �t�B�[���h�̒ʐM��Ԃ�Ԃ�
 *
 * �R�������� _FIELD_COMM_EXIT_EVENT_CALL �̒��Ŏg���܂�
 * �����Ƃ��ĒP�̂ł̌Ăяo���͂����A�R�����������Ă�ł�������
 */
//--------------------------------------------------------------
#define _GET_FIELD_COMM_NO( ret_work ) \
    _ASM_GET_FIELD_COMM_NO  ret_work

  .macro  _ASM_GET_FIELD_COMM_NO  ret_work
  .short  EV_SEQ_GET_FIELD_COMM_NO
  .short  \ret_work
  .endm

//--------------------------------------------------------------
/**
 * @def _FIELD_COMM_EXIT
 * @brief �t�B�[���h�}�b�v�̒ʐM���I������
 *
 * �R�������� _FIELD_COMM_EXIT_EVENT_CALL ���쐻���Ă���܂��B
 * �����Ƃ��ĒP�̂ł̌Ăяo���͂����A�R�����������Ă�ł�������
 */
//--------------------------------------------------------------
#define _FIELD_COMM_EXIT( ret_work ) \
    _ASM_FIELD_COMM_EXIT  ret_work

  .macro  _ASM_FIELD_COMM_EXIT  ret_work
  .short  EV_SEQ_FIELD_COMM_EXIT
  .short  \ret_work
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_AVAILABLE_WIRELESS
 * @brief �ʐM�\�ȏ�Ԃ��ǂ����̎擾
 * @param ret_work  TRUE�̎��A�ʐM�\
 *
 * DSi�{�̐ݒ�ł̒ʐMON/OFF��Ԃ��擾����BDS�̏ꍇ�͏��TRUE�ƂȂ�
 */
//--------------------------------------------------------------
#define _GET_AVAILABLE_WIRELESS( ret_work ) \
    _ASM_GET_AVAILABLE_WIRELESS ret_work

  .macro  _ASM_GET_AVAILABLE_WIRELESS ret_work
  .short  EV_SEQ_GET_AVAILABLE_WIRELESS
  .short  \ret_work
  .endm

//--------------------------------------------------------------
/**
 * @def _REBOOT_BEACON_SEARCH
 * @brief   �t�B�[���h�̃r�[�R���T�[�`���ĊJ����
 *
 * �ꕔ�̒ʐM��t�I�������ł̂ݎg�p�����p�R�}���h�ł��B
 * �R���t�B�O���u�펞�ڑ�ON�v�łȂ��Ƌ@�\���܂���B
 * �ʐM���~���Ă����ԂłȂ���΋@�\���܂���B
 */
//--------------------------------------------------------------
#define _REBOOT_BEACON_SEARCH() \
    _ASM_REBOOT_BEACON_SEARCH

    .macro  _ASM_REBOOT_BEACON_SEARCH
    .short  EV_SEQ_REBOOT_BEACON_SEARCH
    .endm

//======================================================================
//  �v���O�����Ǘ��f�[�^�̎擾�E�Z�b�g
//======================================================================
//--------------------------------------------------------------
/**
 * @def _GET_ROM_VERSION
 * @brief �����o�[�W�����̎擾
 * @retval  VERSION_WHITE �z���C�g�o�[�W����
 * @retval  VERSION_BLACK �u���b�N�o�[�W����
 */
//--------------------------------------------------------------
#define _GET_ROM_VERSION( ret_wk )  \
    _ASM_GET_ROM_VERSION ret_wk

    .macro _ASM_GET_ROM_VERSION ret_wk
    .short  EV_SEQ_GET_ROM_VERSION
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 *  @def  _CHG_LANGID
 *  @brief  ����ID�؂�ւ� �Ђ炪��<->����
 *  @param none
 */
//--------------------------------------------------------------
#define   _CHG_LANGID() _ASM_CHG_LANGID

  .macro  _ASM_CHG_LANGID
  .short  EV_SEQ_CHG_LANGID
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_LANG_ID
 * @brief ���ꃂ�[�hID�̎擾
 * @param ret_wk
 * @retval  0   �Ђ炪�ȃ��[�h
 * @retval  1   �������[�h
 */
//--------------------------------------------------------------
#define _GET_LANG_ID(ret_wk) \
    _ASM_GET_LANG_ID ret_wk

  .macro  _ASM_GET_LANG_ID
  .short  EV_SEQ_GET_LANG_ID
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  @def  _GET_RND
 *  @brief  �����_�����擾
 *  @param ret_wk �����i�[��
 *  @param num ��������l
 *
 *  @return �����i�l�͈̔͂͂O�` num-1 �܂ł̊ԁj
 */
//--------------------------------------------------------------
#define _GET_RND( ret_wk, num ) _ASM_GET_RND ret_wk, num

  .macro  _ASM_GET_RND ret_wk, num
  .short  EV_SEQ_GET_RND
  .short  \ret_wk
  .short  \num
  .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
  .macro  _ASM_GET_NOW_MSG_ARCID  work
  .short  EV_SEQ_GET_NOW_MSG_ARCID
  .short  \work
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TIMEZONE
 * @brief ���ԑт̎擾
 * @param ret_wk    �`�F�b�N���ʂ��󂯎�郏�[�N
 *
 * �߂�l��prog/include/system/timezone.h���Q��
 *
 * ���ԑтƑΉ����鎞�Ԃ͋G�߂ɂ��ς��܂��̂Œ��ӂ��K�v�ł��B
 */
//--------------------------------------------------------------
#define _GET_TIMEZONE( ret_wk ) \
    _ASM_GET_TIMEZONE ret_wk

  .macro  _ASM_GET_TIMEZONE ret_wk
  .short  EV_SEQ_GET_TIMEZONE
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TRAINER_CARD_RANK
 * @brief �g���[�i�[�J�[�h�����N�̎擾
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _GET_TRAINER_CARD_RANK( ret_wk ) \
    _ASM_GET_TRAINER_CARD_RANK ret_wk

  .macro  _ASM_GET_TRAINER_CARD_RANK ret_wk
  .short  EV_SEQ_GET_TRAINER_CARD_RANK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_WEEK
 * @brief �j���̎擾
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 *
 * �߂�l��prog\src\field\script_def.h���Q��
 */
//--------------------------------------------------------------
#define _GET_WEEK( ret_wk ) \
    _ASM_GET_WEEK ret_wk

  .macro  _ASM_GET_WEEK ret_wk
  .short  EV_SEQ_GET_WEEK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_DATE
 * @brief ���t�̎擾
 * @param ret_month �����󂯎�郏�[�N
 * @param ret_day   �����󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _GET_DATE( ret_month, ret_day ) \
    _ASM_GET_DATE ret_month, ret_day

  .macro  _ASM_GET_DATE ret_month, ret_day
  .short  EV_SEQ_GET_DATE
  .short  \ret_month
  .short  \ret_day
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TIME
 * @brief ���ݎ��Ԃ̎擾
 * @param ret_hour    �����󂯎�郏�[�N
 * @param ret_minute  �����󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _GET_TIME( ret_hour, ret_minute ) \
    _ASM_GET_TIME ret_hour, ret_minute

  .macro  _ASM_GET_TIME ret_hour, ret_minute
  .short  EV_SEQ_GET_TIME
  .short  \ret_hour
  .short  \ret_minute
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_SEASON_ID
 * @brief �G�߂̎擾
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 * @return  �G�ߎw��ID�iprog/include/gamesystem/pm_season.h�Q�Ɓj
 */
//--------------------------------------------------------------
#define _GET_SEASON_ID( ret_wk )  \
    _ASM_GET_SEASON_ID ret_wk

    .macro _ASM_GET_SEASON_ID ret_wk
    .short  EV_SEQ_GET_SEASON_ID
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZONE_ID
 * @brief ���@�̂���]�[��ID���擾
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 * @return  �]�[��ID
 */
//--------------------------------------------------------------
#define _GET_ZONE_ID( ret_wk )  \
    _ASM_GET_ZONE_ID ret_wk

    .macro _ASM_GET_ZONE_ID ret_wk
    .short  EV_SEQ_GET_ZONE_ID
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_BIRTHDAY
 * @brief �a�����̎擾
 * @param ret_month �����󂯎�郏�[�N
 * @param ret_day   �����󂯎�郏�[�N
 *
 * DS�ɓo�^�����a�������擾����
 */
//--------------------------------------------------------------
#define _GET_BIRTHDAY( ret_month, ret_day ) \
    _ASM_GET_BIRTHDAY ret_month, ret_day

    .macro _ASM_GET_BIRTHDAY ret_month, ret_day
    .short  EV_SEQ_GET_BIRTHDAY
    .short  \ret_month
    .short  \ret_day
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_MY_SEX
 * @brief �����̐��ʂ̎擾
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 * @retval  PM_MALE     �j��
 * @retval  PM_FEMALE   ����
 */
//--------------------------------------------------------------
#define _GET_MY_SEX( ret_wk ) \
    _ASM_GET_MY_SEX ret_wk

    .macro _ASM_GET_MY_SEX ret_wk
    .short  EV_SEQ_GET_MY_SEX
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZUKAN_COUNT
 * @brief ������t���O�̃J�E���g
 * @param zukan_count_type  ZUKANCTRL_MODE_SEE / ZUKANCTRL_MODE_GET
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 */
//--------------------------------------------------------------
#define _GET_ZUKAN_COUNT( zukan_count_type, ret_wk ) \
    _ASM_GET_ZUKAN_COUNT zukan_count_type, ret_wk

    .macro _ASM_GET_ZUKAN_COUNT zukan_count_type, ret_wk
    .short  EV_SEQ_GET_ZUKAN_COUNT
    .short  \zukan_count_type
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_ZUKAN_FLAG
 * @brief ������t���O���Z�b�g����
 * @param zukan_count_type  ZUKANCTRL_MODE_SEE / ZUKANCTRL_MODE_GET
 * @param monsno    �Z�b�g�Ώۃ|�P�����̃i���o�[
 */
//--------------------------------------------------------------
#define _SET_ZUKAN_FLAG( zukan_count_type, monsno ) \
    _ASM_SET_ZUKAN_FLAG zukan_count_type, monsno

    .macro _ASM_SET_ZUKAN_FLAG zukan_count_type, monsno
    .short  EV_SEQ_SET_ZUKAN_FLAG
    .short  \zukan_count_type
    .short  \monsno
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZUKAN_FLAG
 * @brief ������t���O���擾����
 * @param zukan_count_type  ZUKANCTRL_MODE_SEE / ZUKANCTRL_MODE_GET
 * @param monsno    �擾�Ώۃ|�P�����̃i���o�[
 */
//--------------------------------------------------------------
#define _GET_ZUKAN_FLAG( zukan_count_type, monsno, ret_wk ) \
    _ASM_GET_ZUKAN_FLAG zukan_count_type, monsno, ret_wk

    .macro _ASM_GET_ZUKAN_FLAG zukan_count_type, monsno, ret_wk
    .short  EV_SEQ_GET_ZUKAN_FLAG
    .short  \zukan_count_type
    .short  \monsno
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZENKOKU_ZUKAN_FLAG
 * @brief �S���}�Ӄ��[�h�̊m�F
 * @param ret_wk
 * @retval  TRUE�̎��A�S���}�Ӄ��[�h
 * @retval  FALSE�̎��A�n���}�Ӄ��[�h
 */
//--------------------------------------------------------------
#define _GET_ZENKOKU_ZUKAN_FLAG( ret_wk ) \
    _ASM_GET_ZENKOKU_ZUKAN_FLAG ret_wk

    .macro  _ASM_GET_ZENKOKU_ZUKAN_FLAG ret_wk
    .short  EV_SEQ_GET_ZUKAN_ZENKOKU_FLAG
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_ZENKOKU_ZUKAN_FLAG
 * @brief �S���}�Ӄ��[�h�̃Z�b�g
 */
//--------------------------------------------------------------
#define _SET_ZENKOKU_ZUKAN_FLAG() \
    _ASM_SET_ZENKOKU_ZUKAN_FLAG

    .macro  _ASM_SET_ZENKOKU_ZUKAN_FLAG ret_wk
    .short  EV_SEQ_SET_ZUKAN_ZENKOKU_FLAG
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_ZUKAN_GRAPHIC_FLAG
 * @brief ������o�[�W�����A�b�v�F�t�H�����ڍ׉�ʒǉ�
 */
//--------------------------------------------------------------
#define _SET_ZUKAN_GRAPHIC_FLAG() \
    _ASM_SET_ZUKAN_GRAPHIC_FLAG

    .macro  _ASM_SET_ZUKAN_GRAPHIC_FLAG ret_wk
    .short  EV_SEQ_SET_ZUKAN_GRAPHIC_FLAG
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_BADGE_FLAG
 * @brief �o�b�W�t���O�̎擾
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 * @param badge_id  �擾����o�b�W��ID
 */
//--------------------------------------------------------------
#define _GET_BADGE_FLAG( ret_wk, badge_id ) \
    _ASM_GET_BADGE_FLAG ret_wk, badge_id

    .macro  _ASM_GET_BADGE_FLAG ret_wk, badge_id
    .short  EV_SEQ_GET_BADGE_FLAG
    .short  \ret_wk
    .short  \badge_id
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_BADGE_FLAG
 * @brief �o�b�W�t���O�̃Z�b�g
 * @param badge_id  �Z�b�g����o�b�W��ID
 */
//--------------------------------------------------------------
#define _SET_BADGE_FLAG( badge_id ) \
    _ASM_SET_BADGE_FLAG badge_id

    .macro  _ASM_SET_BADGE_FLAG badge_id
    .short  EV_SEQ_SET_BADGE_FLAG
    .short  \badge_id
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_BADGE_COUNT
 * @brief �o�b�W���̎擾
 * @param ret_wk    ���ʂ��󂯂郏�[�N
 */
//--------------------------------------------------------------
#define _GET_BADGE_COUNT( ret_wk ) \
    _ASM_GET_BADGE_COUNT ret_wk

    .macro  _ASM_GET_BADGE_COUNT ret_wk
    .short  EV_SEQ_GET_BADGE_COUNT
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_STARTMENU_FLAG
 * @brief   �^�C�g������̃X�^�[�g���j���[���ڂ��I�[�v������
 * @param type    ���j���[���ڎw��iscript_def.h��SCR_STARTMENU_FLAG_�`���Q�Ɓj
 */
//--------------------------------------------------------------
#define _SET_STARTMENU_FLAG( type ) \
    _ASM_SET_STARTMENU_FLAG type
    .macro  _ASM_SET_STARTMENU_FLAG type
    .short  EV_SEQ_SET_START_MENU_FLAG
    .short  \type
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_SEATEMPLE_STEP_WALK
 * @brief   �C���Ձ@���s���̎擾
 * @param ret_wk  �����i�[��
 */
//--------------------------------------------------------------
#define _GET_SEATEMPLE_STEP_WALK( ret_wk ) \
    _ASM_GET_SEATEMPLE_STEP_WALK ret_wk
    .macro  _ASM_GET_SEATEMPLE_STEP_WALK ret_wk
    .short  EV_SEQ_GET_SEATEMPLE_STEP_WALK
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_STARTMENU_FLAG
 * @brief   �^�C�g������̃X�^�[�g���j���[���ڂ̏�Ԏ擾
 * @param type    ���j���[���ڎw��iscript_def.h��SCR_STARTMENU_FLAG_�`���Q�Ɓj
 * @param ret_wk  �߂�l�iTRUE/FALSE��Ԃ��j
 */
//--------------------------------------------------------------
#define _GET_STARTMENU_FLAG( type, ret_wk ) \
    _ASM_GET_STARTMENU_FLAG type, ret_wk
    .macro  _ASM_GET_STARTMENU_FLAG type, ret_wk
    .short  EV_SEQ_GET_START_MENU_FLAG
    .short  \type
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _CHK_STARTMENU_VIEW_FLAG
 * @brief   �^�C�g������̃X�^�[�g���j���[���ڂ̏�Ԏ擾�m�d�v�������Ă��邩�H
 * @param type    ���j���[���ڎw��iscript_def.h��SCR_STARTMENU_FLAG_�`���Q�Ɓj
 * @param ret_wk  �߂�l�iTRUE/FALSE��Ԃ��jTRUE�ŕ\���ς�
 */
//--------------------------------------------------------------
#define _CHK_STARTMENU_VIEW_FLAG( type, ret_wk ) \
    _ASM_CHK_STARTMENU_VIEW_FLAG type, ret_wk
    .macro  _ASM_CHK_STARTMENU_VIEW_FLAG type, ret_wk
    .short  EV_SEQ_CHK_START_MENU_VIEW_FLAG
    .short  \type
    .short  \ret_wk
    .endm
    

//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��߂��̐ݒ�
 * @param warp_id ���[�v��ѐ�w��
 *
 * �S�Ŏ��A�e���|�[�g�g�p���̖߂��w��������Ȃ��B
 * �w��\�Ȓ�`�ɂ��Ă�resource/fldmapdata/warpdata/warpdata.h���Q�ƁB
 */
//--------------------------------------------------------------
#define _SET_WARP_ID( warp_id )  \
    _ASM_SET_WARP_ID warp_id

    .macro  _ASM_SET_WARP_ID warp_id
    .short  EV_SEQ_SET_WARP_ID
    .short  \warp_id
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_SP_LOCATION_HERE
 * @brief ����ڑ���̐ݒ�F���݈ʒu
 *
 * @note
 * �O���b�h�}�b�v�ɂ����Ή����Ă��Ȃ�
 */
//--------------------------------------------------------------
#define _SET_SP_LOCATION_HERE() \
      _ASM_SET_SP_LOCATION_HERE

    .macro _ASM_SET_SP_LOCATION_HERE
    .short  EV_SEQ_SET_SP_LOCATION_HERE
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_SP_LOCATION_DIRECT
 * @brief ����ڑ���̐ݒ�F���ڎw��
 * @param zone_id �}�b�vID�w��
 * @param dir �����w��
 * @param gx  X�ʒu�w��i�O���b�h�P�ʁj
 * @param gy  Y�ʒu�w��i�O���b�h�P�ʁj
 * @param gz  Z�ʒu�w��i�O���b�h�P�ʁj
 *
 * @note
 * �O���b�h�}�b�v�ɂ����Ή����Ă��Ȃ�
 */
//--------------------------------------------------------------
#define _SET_SP_LOCATION_DIRECT( zone_id, dir, gx, gy, gz ) \
      _ASM_SET_SP_LOCATION_DIRECT zone_id, dir, gx, gy, gz 

    .macro _ASM_SET_SP_LOCATION_DIRECT zone_id, dir, gx, gy, gz 
    .short  EV_SEQ_SET_SP_LOCATION_DIRECT
    .short  \zone_id
    .short  \dir
    .short  \gx, \gy, \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _CHANGE_MAPREPLACE_FLAG
 * @brief �}�b�v�u�������C�x���g�̃t���O����
 * @param id            �}�b�v�u�������̎w��ID�iprog/src/field/map_replace.h�Q�Ɓj
 * @param on_off        �u��������ON/OFF�w��
 * @param direct_flag   �������f�t���O�iTRUE/FALSE)
 *
 * @note
 * �}�b�v�u�������V�X�e�����Q�Ƃ���C�x���g�t���O�̑�����s���X�N���v�g�R�}���h�B
 * �������f�t���O��TRUE�Ɏw�肵�Ă��A�t�B�[���h���甲���ď�ʓ]�����s��Ȃ�����͔��f���Ȃ��̂Œ��ӁB
 */
//--------------------------------------------------------------
#define _CHANGE_MAPREPLACE_FLAG( id, on_off, direct_flag ) \
    _ASM_CHANGE_MAPREPLACE_FLAG id, on_off, direct_flag

    .macro  _ASM_CHANGE_MAPREPLACE_FLAG id, on_off, direct_flag
    .short  EV_SEQ_CHANGE_MAP_REPLACE_FLAG
    .short  \id
    .short  \on_off
    .short  \direct_flag
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_WIRELESS_SAVEMODE
 * @brief ���C�����X�Z�[�u���[�h�̎擾
 */
//--------------------------------------------------------------
#define _GET_WIRELESS_SAVEMODE( ret_wk ) \
      _ASM_GET_WIRELESS_SAVEMODE ret_wk

    .macro  _ASM_GET_WIRELESS_SAVEMODE ret_wk
    .short  EV_SEQ_GET_WIRELESS_SAVEMODE
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_SHOES
 * @brief �V���[�Y�̎擾
 */
//--------------------------------------------------------------
#define _GET_SHOES() _ASM_GET_SHOES

    .macro  _ASM_GET_SHOES
    .short  EV_SEQ_GET_SHOES
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_BOX_WALLPAPER
 * @brief �{�b�N�X�̒ǉ��ǎ��擾
 * @param wallpaper_flag  script_def.h��SCR_BOX_EX_WALLPAPER?���g�p
 */
//--------------------------------------------------------------
#define _GET_BOX_WALLPAPER( flag )  \
    _ASM_GET_BOX_WALLPAPER flag

    .macro  _ASM_GET_BOX_WALLPAPER flag
    .short  EV_SEQ_GET_BOX_WALLPAPER
    .short  \flag
    .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_PLAYER_VIEW_CHANGE
 * @brief �v���C���[�̃g���[�i�[�^�C�v���ύX����Ă��邩�ǂ������`�F�b�N����.
 *        �ύX����Ă��Ȃ��ꍇ, 
 *        �� �ύX�ς݂ɐݒ肵, 
 *        �� �f�t�H���g�Őݒ肳�ꂽ�g���[�i�[�^�C�v���擾����.
 * @param ret_wk_chng �g���[�i�[�^�C�v�ύX��Ԃ̊i�[��
 * @param ret_wk_type �v���C���[�̃g���C�i�[�^�C�v�̊i�[��
 *
 * ��ret_wk_chng
 * ��TRUE  �g���[�i�[�^�C�v�ύX�ς�
 * ��FALSE �g���[�i�[�^�C�v�ύX���ĂȂ�
 */
//--------------------------------------------------------------
#define _CHECK_PLAYER_VIEW_CHANGE( ret_wk_chng, ret_wk_type )  \
    _ASM_CHECK_PLAYER_VIEW_CHANGE ret_wk_chng, ret_wk_type

    .macro  _ASM_CHECK_PLAYER_VIEW_CHANGE ret_wk_chng, ret_wk_type
    .short  EV_SEQ_CHECK_PLAYER_VIEW_CHANGE
    .short  \ret_wk_chng
    .short  \ret_wk_type
    .endm

//--------------------------------------------------------------
/**
 * @def _SET_GYM_VICTORY_INFO
 * @brief �W�����[�_�[�폟�����̏��Z�b�g
 * @param gym_id  BADGE_ID_??���w�肵�Ă�������
 */
//--------------------------------------------------------------
#define _SET_GYM_VICTORY_INFO( gym_id ) \
      _ASM_SET_GYM_VICTORY_INFO gym_id

    .macro  _ASM_SET_GYM_VICTORY_INFO gym_id
    .short  EV_SEQ_SET_GYM_VICTORY_INFO
    .short  \gym_id
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_GYM_VICTORY_INFO
 * @brief �W�����[�_�[�폟�����̎莝���|�P�����푰�����Z�b�g����
 * @param gym_id  BADGE_ID_??���w�肵�Ă�������
 * @param ret_wk  �W�����[�_�[�폟�����̎莝���|�P������
 */
//--------------------------------------------------------------
#define _GET_GYM_VICTORY_INFO( gym_id, ret_wk ) \
    _ASM_GET_GYM_VICTORY_INFO gym_id, ret_wk

    .macro _ASM_GET_GYM_VICTORY_INFO gym_id, ret_wk
    .short  EV_SEQ_GET_GYM_VICTORY_INFO
    .short  \gym_id
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def   _SET_FAVORITE_POKEMON
 * @brief �D���ȃ|�P�����̎w��i�o�g�����R�[�_�[�p�j
 * @param pos   �w�肷��|�P�����̈ʒu
 */
//--------------------------------------------------------------
#define _SET_FAVORITE_POKEMON( pos ) \
    _ASM_SET_FAVORITE_POKEMON pos
    .macro  _ASM_SET_FAVORITE_POKEMON pos
    .short  EV_SEQ_SET_FAVORITE_POKEMON
    .short  \pos
    .endm

//--------------------------------------------------------------
/**
 * @def	_GET_BREEDER_JUDGE_RESULT
 * @brief �|�P�����̒l�̕]���p�����[�^�擾
 * @param pos   �w�肷��|�P�����̈ʒu
 * @param	mode	(script_def.h��SCR_JUDGE_MODE_�`���Q�Ɓj
 * @param	ret_wk		�擾�����l
 */
//--------------------------------------------------------------
#define	_GET_BREEDER_JUDGE_RESULT( pos, mode, ret_wk ) \
		_ASM_GET_BREEDER_JUDGE_RESULT pos, mode, ret_wk

		.macro	_ASM_GET_BREEDER_JUDGE_RESULT pos, mode, ret_wk
		.short	EV_SEQ_GET_BREEDER_JUDGE_RESULT
		.short	\pos
		.short	\mode
		.short	\ret_wk
		.endm

//--------------------------------------------------------------
/**
 * @def _WAZAMACHINE_LOT
 * @brief �ێ��킴�}�V�����璊�I����
 * @param	ret_wk		���I���ꂽ�Z�i���o�[�i�O�̎��A�ΏۂȂ��j
 *
 * �킴�}�V���i���o�[�łȂ��A�Z�i���o�[��Ԃ�
 */
//--------------------------------------------------------------
#define _WAZAMACHINE_LOT( ret_wk ) \
    _ASM_WAZAMACHINE_LOT ret_wk

    .macro  _ASM_WAZAMACHINE_LOT ret_wk
    .short  EV_SEQ_WAZAMACHINE_LOT
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZUKAN_HYOUKA
 * @brief ������]���p���b�Z�[�WID�擾
 * @param mode      ������]�����[�h�iscript_def.h��SCR_ZUKAN_HYOUKA_MODE_�`���Q�Ɓj
 * @param	ret_msgid		�擾����MSGID
 * @param ret_count   �|�P������
 */
//--------------------------------------------------------------
#define _GET_ZUKAN_HYOUKA( mode, ret_msgid, ret_count ) \
    _ASM_GET_ZUKAN_HYOUKA mode, ret_msgid, ret_count

    .macro  _ASM_GET_ZUKAN_HYOUKA mode, ret_msgid, ret_count
    .short  EV_SEQ_GET_ZUKAN_HYOUKA
    .short  \mode
    .short  \ret_msgid
    .short  \ret_count
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_ZUKAN_COMPLETE
 * @brief �����񊮐��`�F�b�N
 * @param ret_wk    TRUE�̂Ƃ��A����
 * @param mode      ������]�����[�h�iscript_def.h��SCR_ZUKAN_HYOUKA_MODE_�`���Q�Ɓj
 *
 * @li  SCR_ZUKAN_HYOUKA_MODE_GLOBAL    �S���}�ӂł̃`�F�b�N
 * @li  SCR_ZUKAN_HYOUKA_MODE_LOCAL_GET ���[�J���}�ӂł̃`�F�b�N
 * @li  ���̑���SCR_ZUKAN_HYOUKA_MODE_�`�ɂ͑Ή����Ă��Ȃ�
 */
//--------------------------------------------------------------
#define _GET_ZUKAN_COMPLETE( ret_wk, mode ) \
    _ASM_GET_ZUKAN_COMPLETE ret_wk, mode

    .macro  _ASM_GET_ZUKAN_COMPLETE ret_wk, mode
    .short  EV_SEQ_GET_ZUKAN_COMPLETE
    .short  \ret_wk
    .short  \mode
    .endm

//======================================================================
//
//    
//
//======================================================================
//--------------------------------------------------------------
/**
 * @def _CGEAR_ON_DEMO
 * @brief C�M�A�N���f��
 */
//--------------------------------------------------------------
#define _CGEAR_ON_DEMO()  \
    _ASM_CGEAR_ON_DEMO

    .macro  _ASM_CGEAR_ON_DEMO
    .short  EV_SEQ_CALL_CGEAR_GET_DEMO
    .short  0
    .endm

//--------------------------------------------------------------
/**
 * @def _GOTO_GAMECLEAR_DEMO
 * @brief �Q�[���N���A�f���ւ̈ڍs
 * @param mode  ���ݖ��g�p�F�O�����Ă����Ă�������
 */
//--------------------------------------------------------------
#define _GOTO_GAMECLEAR_DEMO( mode ) \
    _ASM_GOTO_GAMECLEAR_DEMO mode

    .macro  _ASM_GOTO_GAMECLEAR_DEMO mode
    .short  EV_SEQ_GOTO_GAMECLEAR
    .short  \mode
    .endm


//--------------------------------------------------------------
/**
 * @def _CHNAGE_SUBSCREEN
 * @brief �T�u�X�N���[����ύX
 * @param mode  �T�u�X�N���[���̃��[�h
 * mode 
 *  SCR_EV_SUBSCREEN_MODE_SAVE    �Z�[�u��ʂɂ���
 */
//--------------------------------------------------------------
#define _CHNAGE_SUBSCREEN( mode ) \
    _ASM_CHNAGE_SUBSCREEN mode

    .macro  _ASM_CHNAGE_SUBSCREEN mode
    .short  EV_SEQ_CHANGE_SUBSCREEN
    .short  \mode
    .endm

//--------------------------------------------------------------
/**
 * @def _BACK_SUBSCREEN_NORMAL
 * @brief �T�u�X�N���[����ʏ��Ԃɖ߂�
 */
//--------------------------------------------------------------
#define _BACK_SUBSCREEN_NORMAL() \
    _ASM_BACK_SUBSCREEN_NORMAL

    .macro  _ASM_BACK_SUBSCREEN_NORMAL
    .short  EV_SEQ_BACK_SUBSCREEN_NORMAL
    .endm




//======================================================================
//
//
//    �|�P�����֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �莝���|�P������
 */
//--------------------------------------------------------------
#define _TEMOTI_POKEMON_KAIFUKU() \
    _ASM_TEMOTI_POKEMON_KAIFUKU

  .macro  _ASM_TEMOTI_POKEMON_KAIFUKU
  .short  EV_SEQ_POKEMON_RECOVER
  .endm

//--------------------------------------------------------------
/**
 * @def _PARTY_POKE_NAME_INPUT
 * @brief �莝���|�P�����̖��O���͉�ʂ��Ăяo��
 * @param ret_wk ���͌��ʂ��󂯎�郏�[�N
 * @param index  ���O��ݒ肷��莝���|�P�����̃C���f�b�N�X���w��
 * @retval  TRUE�̂Ƃ��A���͂������O���m�肵���ꍇ
 * @retval  FALSE�̂Ƃ��A���͂��L�����Z�������ꍇ
 */
//--------------------------------------------------------------
#define _PARTY_POKE_NAME_INPUT( ret_wk, index ) \
    _ASM_PARTY_POKE_NAME_INPUT ret_wk, index

  .macro  _ASM_PARTY_POKE_NAME_INPUT ret_wk, index
  .short  EV_SEQ_PARTY_POKE_NAME_INPUT
  .short  \ret_wk
  .short  \index
  .endm

//--------------------------------------------------------------
/**
 * @def _PARTY_POKE_EGG_BIRTH
 * @brief �莝���̃^�}�S��z��������
 * @param ret_wk ���܂ꂽ�|�P�����̎莝���C���f�b�N�X���󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _PARTY_POKE_EGG_BIRTH( ret_wk ) \
    _ASM_PARTY_POKE_EGG_BIRTH ret_wk

  .macro  _ASM_PARTY_POKE_EGG_BIRTH ret_wk
  .short  EV_SEQ_PARTY_POKE_EGG_BIRTH
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_POKERUS
 * @brief �莝���Ƀ|�P���X�����邩�ǂ����̃`�F�b�N
 * @param ret_wk    �`�F�b�N���ʂ��󂯎�郏�[�N
 * @retval  TRUE�̂Ƃ��A�|�P���X���������|�P�������莝���ɂ���
 * @retval  FALSE�̂Ƃ��A���������|�P�����͂��Ȃ�
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_POKERUS( ret_wk ) \
    _ASM_CHECK_TEMOTI_POKERUS ret_wk

  .macro  _ASM_CHECK_TEMOTI_POKERUS ret_wk
  .short  EV_SEQ_CHECK_TEMOTI_POKERUS
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_HP
 * @brief �莝���|�P������HP�`�F�b�N
 * @param ret_wk  �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @retval  TRUE    �`�F�b�N�����|�P������HP�����^��
 * @retval  FALSE   �`�F�b�N�����|�P������HP�͌����Ă���
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_HP( ret_wk, pos ) \
    _ASM_CHECK_TEMOTI_HP ret_wk, pos


  .macro  _ASM_CHECK_TEMOTI_HP ret_wk, pos
  .short  EV_SEQ_CHECK_TEMOTI_HP
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_EGG
 * @brief �莝���|�P������EGG�`�F�b�N
 * @param ret_wk  �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @retval  TRUE    �`�F�b�N�����|�P�������^�}�S�̏ꍇ
 * @retval  FALSE   �`�F�b�N�����|�P�������^�}�S�łȂ��ꍇ
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_EGG( ret_wk, pos ) \
    _ASM_CHECK_TEMOTI_EGG ret_wk, pos


  .macro  _ASM_CHECK_TEMOTI_EGG ret_wk, pos
  .short  EV_SEQ_CHECK_TEMOTI_EGG
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_COUNT
 * @brief �莝���|�P�����̐����擾
 * @param ret_wk �`�F�b�N���ʂ��󂯎�郏�[�N
 	!!����!!
 	�Ԃ�l�́u�P�v����n�܂�܂��B�莝���̃|�W�V�����w��́u0�v�n�܂�Ȃ̂ō��킹�Ďg���ꍇ�͒��ӁB
 * @param count_mode �������w��(POKECOUNT_MODE_XXXX)
 		�莝���|�P�������J�E���g���[�h
 		POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
 		POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
 		POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
 		POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������) 
 		POKECOUNT_MODE_ONLY_DAME_EGG (4)  // �ʖڃ^�}�S�̐�
    POKECOUNT_MODE_EMPTY         (5)  // �󂢂Ă��鐔
 	* @retval u16 �莝���|�P�����̐�
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_COUNT( ret_wk, count_mode ) \
    _ASM_GET_PARTY_POKE_COUNT ret_wk, count_mode

  .macro _ASM_GET_PARTY_POKE_COUNT ret_wk, count_mode
  .short EV_SEQ_GET_PARTY_POKE_COUNT
  .short \ret_wk
  .short \count_mode
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_COUNT_BY_MONSNO
 * @brief �w�胂���X�^�[�i���o�[�̃|�P���莝���ɉ��C���邩��Ԃ�(�^�}�S�͏��O)
 * @param monsno  �Ώۃ����X�^�[�i���o�[
 * @param ret_wk  ���ʂ��󂯎�郏�[�N
 * @retval  0�`6
 */
//--------------------------------------------------------------  
#define _GET_PARTY_POKE_COUNT_BY_MONSNO(monsno, ret_wk) \
    _ASM_GET_PARTY_POKE_COUNT_BY_MONSNO monsno, ret_wk

  .macro  _ASM_GET_PARTY_POKE_COUNT_BY_MONSNO monsno, ret_wk
  .short  EV_SEQ_GET_PARTY_POKE_COUNT_BY_MONSNO
  .short  \monsno
  .short  \ret_wk
  .endm  

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_FRONT_POKEMON
 * @brief �莝���̐擪�|�P����������ʒu���擾
 * @param ret_wk �`�F�b�N���ʂ��󂯎�郏�[�N�i0�`5�̐��l�ŕԂ��܂��j
 * @param count_mode �������w��(POKECOUNT_MODE_XXXX)
 		POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S������
 		POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P����
 	* @retval u16 �ʒu
  *
  * _GET_PARTY_POKE_COUNT�Ɠ����������Ƃ邪�A�g���Ȃ����̂�����̂Œ��ӁI
 */
//--------------------------------------------------------------
#define _GET_PARTY_FRONT_POKEMON( ret_wk, count_mode ) \
    _ASM_GET_PARTY_FRONT_POKEMON ret_wk, count_mode

    .macro  _ASM_GET_PARTY_FRONT_POKEMON ret_wk, count_mode
    .short  EV_SEQ_GET_PARTY_FRONT_POKEMON
    .short  \ret_wk
    .short  \count_mode
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_POKE_PARAM
 * @brief
 * @param ret_wk      ���ʂ��󂯎�郏�[�N
 * @param para_id     �擾�������p�����[�^�w��ID�iscript_def.h��SCR_POKEPARA_�`�j
 * @param pos         �����X�^�[�i���o�[�𒲂ׂ�莝���|�P�����ԍ�
 *
 * @note
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_PARAM( ret_wk, pos, para_id ) \
    _ASM_GET_PARTY_POKE_PARAM ret_wk, pos, para_id

    .macro  _ASM_GET_PARTY_POKE_PARAM ret_wk, pos, para_id
    .short  EV_SEQ_GET_PARTY_POKE_PARAMETER
    .short  \ret_wk
    .short  \pos
    .short  \para_id
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_MONSNO
 * @brief �w�肵���莝���|�P�����̃����X�^�[�i���o�[���擾����
 * @param ret_wk ���ʂ��󂯎�郏�[�N
 * @param pos    �����X�^�[�i���o�[�𒲂ׂ�莝���|�P�����ԍ�
 * @retval u16   �w��|�P�����̃����X�^�[�i���o�[
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_MONSNO( ret_wk, pos ) \
    _ASM_GET_PARTY_POKE_MONSNO ret_wk, pos

  .macro _ASM_GET_PARTY_POKE_MONSNO ret_wk, pos
  .short EV_SEQ_GET_PARTY_POKE_MONSNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POKE_FORMNO
 * @brief �w�肵���莝���|�P�����̌`��i���o�[���擾����
 * @param ret_wk ���ʂ��󂯎�郏�[�N
 * @param pos    �`��i���o�[�𒲂ׂ�莝���|�P�����ԍ�
 * @retval u16   �w��|�P�����̌`��i���o�[
 */
//--------------------------------------------------------------
#define _GET_PARTY_POKE_FORMNO( ret_wk, pos ) \
    _ASM_GET_PARTY_POKE_FORMNO ret_wk, pos

  .macro _ASM_GET_PARTY_POKE_FORMNO ret_wk, pos
  .short EV_SEQ_GET_PARTY_POKE_FORMNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_POKEMON_TO_PARTY
 * @brief �|�P�������莝���ɒǉ�����
 * @param ret_wk ���ʂ��󂯎�郏�[�N
 * @param monsno  �����X�^�[�i���o�[
 * @param formno  �t�H�[���i���o�[
 * @param tokusei ����
 * @param level   ���x��
 * @param itemno  �����A�C�e��
 * @param 
 * @retval BOOL �ǉ��ł�����TRUE
 */
//--------------------------------------------------------------
#define _ADD_POKEMON_TO_PARTY( ret_wk, monsno, formno, tokusei, level, itemno ) \
    _ASM_ADD_POKEMON_TO_PARTY ret_wk, monsno, formno, tokusei, level, itemno

  .macro _ASM_ADD_POKEMON_TO_PARTY ret_wk, monsno, formno, tokusei, level, itemno
  .short EV_SEQ_ADD_POKEMON_TO_PARTY
  .short \ret_wk
  .short \monsno
  .short \formno
  .short \tokusei
  .short \level
  .short \itemno
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_POKEMON_TO_BOX
 * @brief �|�P�������{�b�N�X�ɒǉ�����
 * @param ret_wk ���ʂ��󂯎�郏�[�N
 * @param monsno  �����X�^�[�i���o�[
 * @param formno  �t�H�[���i���o�[
 * @param tokusei ����
 * @param level   ���x��
 * @param itemno  �����A�C�e��
 * @param 
 * @retval BOOL �ǉ��ł�����TRUE
 */
//--------------------------------------------------------------
#define _ADD_POKEMON_TO_BOX( ret_wk, monsno, formno, tokusei, level, itemno ) \
    _ASM_ADD_POKEMON_TO_BOX ret_wk, monsno, formno, tokusei, level, itemno

  .macro _ASM_ADD_POKEMON_TO_BOX ret_wk, monsno, formno, tokusei, level, itemno
  .short EV_SEQ_ADD_POKEMON_TO_BOX
  .short \ret_wk
  .short \monsno
  .short \formno
  .short \tokusei
  .short \level
  .short \itemno
  .endm
  
//--------------------------------------------------------------
/**
 * @def _CHECK_POKE_WAZA
 * @brief �w�肳�ꂽ�Z���o���Ă��邩���ׂ�
 * @param ret_wk �`�F�b�N���ʂ��i�[���郏�[�N 0=�o���Ă��Ȃ� 1=�o���Ă���
 * @param wazano �`�F�b�N����Z�i���o�[
 * @param tno ���ׂ�莝���|�P�����̈�ԍ�
 */
//--------------------------------------------------------------
#define _CHECK_POKE_WAZA( ret_wk, wazano, tno ) \
    _ASM_CHECK_POKE_WAZA ret_wk, wazano, tno

  .macro _ASM_CHECK_POKE_WAZA ret_wk, wazano, tno
  .short EV_SEQ_CHK_POKE_WAZA
  .short \ret_wk
  .short \wazano
  .short \tno
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_POKE_WAZA_GROUP 
 * @brief �w�肳�ꂽ�Z���o���Ă��邩���ׂ�i�莝���S�́j
 * @param ret_wk �`�F�b�N���ʂ��i�[���郏�[�N �Z���o���Ă���莝���ʒu�B 6=�o���Ă��Ȃ�
 * @param wazano �`�F�b�N����Z�i���o�[
 */
//--------------------------------------------------------------
#define _CHECK_POKE_WAZA_GROUP( ret_wk, wazano ) \
    _ASM_CHECK_POKE_WAZA_GROUP ret_wk, wazano

  .macro _ASM_CHECK_POKE_WAZA_GROUP ret_wk, wazano
  .short EV_SEQ_CHK_POKE_WAZA_GROUP
  .short \ret_wk
  .short \wazano
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_NATSUKI
 * @brief �莝���|�P�����̂Ȃ��x�`�F�b�N
 * @param ret_wk  �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @return        �`�F�b�N�����|�P�����̂Ȃ��x
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_NATSUKI( ret_wk, pos ) \
    _ASM_CHECK_GET_TEMOTI_NATSUKI ret_wk, pos

  .macro  _ASM_CHECK_GET_TEMOTI_NATSUKI ret_wk, pos
  .short  EV_SEQ_GET_TEMOTI_NATSUKI
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _SET_TEMOTI_NATSUKI
 * @brief �莝���|�P�����̂Ȃ��x���Z�b�g
 * @param pos     �Ȃ��x�𑀍삷��|�P�����̈ʒu�i�O�`�T�j
 * @param value   �ݒ肵�����Ȃ��x
 * @param mode    �R�}���h�̌Ăяo�����[�h(0:�Z�b�g,1:���Z,2:���Z)
 */
//--------------------------------------------------------------
#define _SET_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_SET_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_SET_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_SET_TEMOTI_NATSUKI
  .short  \pos
  .short  \value
  .short   0
  .endm

//--------------------------------------------------------------
/**
 * @def _ADD_TEMOTI_NATSUKI
 * @brief �莝���|�P�����̂Ȃ��x�����Z
 * @param pos     �Ȃ��x�𑀍삷��|�P�����̈ʒu�i�O�`�T�j
 * @param value   ���Z�������l
 * @param mode    �R�}���h�̌Ăяo�����[�h(0:�Z�b�g,1:���Z,2:���Z)
 */
//--------------------------------------------------------------
#define _ADD_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_ADD_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_ADD_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_SET_TEMOTI_NATSUKI
  .short  \pos
  .short  \value
  .short  1
  .endm

//--------------------------------------------------------------
/**
 * @def _SUB_TEMOTI_NATSUKI
 * @brief �莝���|�P�����̂Ȃ��x�����Z
 * @param pos     �Ȃ��x�𑀍삷��|�P�����̈ʒu�i�O�`�T�j
 * @param value   ���Z�������l
 * @param mode    �R�}���h�̌Ăяo�����[�h(0:�Z�b�g,1:���Z,2:���Z)
 */
//--------------------------------------------------------------
#define _SUB_TEMOTI_NATSUKI( pos, value ) \
    _ASM_CHECK_SUB_TEMOTI_NATSUKI pos, value

  .macro  _ASM_CHECK_SUB_TEMOTI_NATSUKI pos,value
  .short  EV_SEQ_SET_TEMOTI_NATSUKI
  .short  \pos
  .short  \value
  .short  2
  .endm

//--------------------------------------------------------------
/**
 * @def _SELECT_TEMOTI_POKE
 * @brief �ėp�莝���|�P�����I��
 * @param decide �I�����ʂ̊i�[�� (TRUE�FFALSE �I��������)
 * @param ret_wk �I�����ʂ̊i�[�� (�O�`�T �I���ʒu)
 * @param value  �\�����[�N (�O�����Ă����Ă�������
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _SELECT_TEMOTI_POKE( decide , ret_wk , value ) \
    _ASM_SELECT_TEMOTI_POKE decide, ret_wk, value

  .macro  _ASM_SELECT_TEMOTI_POKE decide, ret_wk, value
  .short  EV_SEQ_PARTY_POKE_SELECT
  .short  \decide
  .short  \ret_wk
  .short  \value
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_WAZA_NUM
 * @brief �莝���|�P�����̋Z���擾
 * @param ret_wk �I�����ʂ̊i�[�� 
 * @param pos    �Ώۂ̈ʒu
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_WAZA_NUM( ret_wk, pos ) \
    _ASM_GET_TEMOTI_WAZA_NUM ret_wk, pos

  .macro  _ASM_GET_TEMOTI_WAZA_NUM ret_wk, pos
  .short  EV_SEQ_GET_TEMOTI_WAZA_NUM
  .short  \ret_wk
  .short  \pos
  .endm


//--------------------------------------------------------------
/**
 * @def _SELECT_WAZA_TEMOTI_POKE
 * @brief �ėp�莝���|�P�����̋Z�I��
 * @param decide �I�����ʂ̊i�[�� (TRUE�FFALSE �I��������)
 * @param ret_wk �I�����ʂ̊i�[�� (�O�`�R �I���ʒu)
 * @param poke_pos �|�P�����̈ʒu
 * @param value  �\�����[�N (�O�����Ă����Ă�������
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _SELECT_WAZA_TEMOTI_POKE( decide , ret_wk , poke_pos , value ) \
    _ASM_SELECT_WAZA_TEMOTI_POKE decide, ret_wk, poke_pos, value

  .macro  _ASM_SELECT_WAZA_TEMOTI_POKE decide, ret_wk, poke_pos, value
  .short  EV_SEQ_PARTY_POKE_WAZA_SELECT
  .short  \decide
  .short  \ret_wk
  .short  \poke_pos
  .short  \value
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_WAZA_ID
 * @brief �ėp�莝���|�P�����̋Z�ԍ��擾
 * @param ret_wk �I�����ʂ̊i�[�� (�ZID)
 * @param poke_pos �|�P�����̈ʒu
 * @param waza_pos �Z�̈ʒu
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_WAZA_ID( ret_wk , poke_pos , waza_pos ) \
    _ASM_GET_TEMOTI_WAZA_ID ret_wk, poke_pos, waza_pos

  .macro  _ASM_GET_TEMOTI_WAZA_ID ret_wk, poke_pos, waza_pos
  .short  EV_SEQ_GET_TEMOTI_WAZA_ID
  .short  \ret_wk
  .short  \poke_pos
  .short  \waza_pos
  .endm

//--------------------------------------------------------------
/**
 * @def _SET_TEMOTI_WAZA
 * @brief �莝���|�P�����̋Z�Z�b�g(�ǉ��E�u�������E�Y��Ή�
 * @param poke_pos �|�P�����̈ʒu
 * @param waza_pos �Z�̈ʒu(5�ŋZ�������o�����Œǉ�
 * @param waza_id  �Z�̔ԍ�(0�ŋZ��Y���
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _SET_TEMOTI_WAZA( poke_pos , waza_pos , waza_id ) \
    _ASM_SET_TEMOTI_WAZA poke_pos, waza_pos, waza_id 

  .macro  _ASM_SET_TEMOTI_WAZA poke_pos, waza_pos, waza_id 
  .short  EV_SEQ_SET_TEMOTI_WAZA
  .short  \poke_pos
  .short  \waza_pos
  .short  \waza_id
  .endm


//--------------------------------------------------------------
/**
 * @def _SKILLTEACH_CHECK_PARTY
 * @brief �킴���ڂ��F�K�p�`�F�b�N
 * @param mode    �Z�o�����[�h�iscript_def.h��SCR_SKILLTEACH_MODE�Q�Ɓj
 * @param ret_wk  script_def.h��SCR_SKILLTEACH_CHECK_RESULT_���Q��
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_OK          �ΏۂƂȂ�|�P����������
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG  �ΏۂƂȂ�|�P���������Ȃ�
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG  �ΏۂƂȂ�|�P�����͂��邪�Ȃ�������Ȃ�
 *
 * �ŋ��Z�i���邢�̓h���S���Z�j���o���邱�Ƃ��ł���莝�����ǂ�����
 * �`�F�b�N����
 */
//--------------------------------------------------------------
#define _SKILLTEACH_CHECK_PARTY( mode, ret_wk ) \
    _ASM_SKILLTEACH_CHECK_PARTY mode, ret_wk

    .macro  _ASM_SKILLTEACH_CHECK_PARTY mode, ret_wk
    .short  EV_SEQ_SKILLTEACH_CHECK_PARTY
    .short  \mode
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SKILLTEACH_CHECK_POKEMON
 * @brief �킴���ڂ��F�K�p�|�P�����̃`�F�b�N
 * @param mode    �Z�o�����[�h�iscript_def.h��SCR_SKILLTEACH_MODE�Q�Ɓj
 * @param pos     �I�������|�P�����̈ʒu
 * @param ret_wk  script_def.h��SCR_SKILLTEACH_CHECK_RESULT_���Q��
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_OK          �o����������
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG  �o���Ȃ��|�P������I��
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG  �Ȃ�������Ȃ��|�P������I��
 * @retval  SCR_SKILLTEACH_CHECK_RESULT_ALREADY_NG  ���łɎ����Ă���|�P������I��
 *
 * �ŋ��Z�i���邢�̓h���S���Z�j���o���邱�Ƃ��ł���莝�����ǂ�����
 * �`�F�b�N����
 */
//--------------------------------------------------------------
#define _SKILLTEACH_CHECK_POKEMON( mode, pos, ret_wk ) \
    _ASM_SKILLTEACH_CHECK_POKEMON mode, pos, ret_wk

    .macro  _ASM_SKILLTEACH_CHECK_POKEMON mode, pos, ret_wk
    .short  EV_SEQ_SKILLTEACH_CHECK_POKEMON
    .short  \mode
    .short  \pos
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SKILLTEACH_SELECT_POKEMON
 * @brief �|�P�����I���F�킴���ڂ��p
 * @param mode    �Z�o�����[�h�iscript_def.h��SCR_SKILLTEACH_MODE�Q�Ɓj
 * @param ret_decide  �I���������ǂ����̖߂�l�iTRUE/FALSE�j
 * @param ret_wk    �I�������|�P�����̈ʒu
 */
//--------------------------------------------------------------
#define _SKILLTEACH_SELECT_POKEMON( mode, ret_decide, ret_wk ) \
    _ASM_SKILLTEACH_SELECT_POKEMON mode, ret_decide, ret_wk

    .macro  _ASM_SKILLTEACH_SELECT_POKEMON mode, ret_decide, ret_wk
    .short  EV_SEQ_SKILLTEACH_SELECT_POKEMON
    .short  \mode
    .short  \ret_decide
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _SKILLTEACH_GET_WAZAID
 * @brief
 * @param mode    �Z�o�����[�h�iscript_def.h��SCR_SKILLTEACH_MODE�Q�Ɓj
 * @param sel_pos �I�������|�P�����̈ʒu 
 * @param ret_wk  �ΏۂƂȂ�Z��ID
 */
//--------------------------------------------------------------
#define _SKILLTEACH_GET_WAZAID( mode, sel_pos, ret_wk ) \
    _ASM_SKILLTEACH_GET_WAZAID mode, sel_pos, ret_wk

    .macro  _ASM_SKILLTEACH_GET_WAZAID mode, sel_pos, ret_wk
    .short  EV_SEQ_SKILLTEACH_GET_WAZAID
    .short  \mode
    .short  \sel_pos
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_MEZAMERU_POWER_TYPE
 * @brief �u�߂��߂�p���[�v�̃^�C�v���擾����
 * @param ret_wk    �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @retval  PokeType  �|�P�����̃^�C�v�w��iprog/include/poke_tool/poketype_def.h�Q�Ɓj
 */
//--------------------------------------------------------------
#define _GET_MEZAMERU_POWER_TYPE( ret_wk, pos ) \
    _ASM_GET_MEZAMERU_POWER_TYPE ret_wk, pos

  .macro  _ASM_GET_MEZAMERU_POWER_TYPE ret_wk, pos
  .short  EV_SEQ_GET_MEZAMERU_POWER_TYPE
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def   _GET_TEMOTI_POKE_TYPE
 * @brief �莝���|�P�����̃^�C�v���擾����
 * @param ret_wk1 �`�F�b�N���ʂ��󂯎�郏�[�N���̂P
 * @param ret_wk2 �`�F�b�N���ʂ��󂯎�郏�[�N���̂Q
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @retval  PokeType  �|�P�����̃^�C�v�w��iprog/include/poke_tool/poketype_def.h�Q�Ɓj
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_POKE_TYPE( ret_wk1, ret_wk2, pos ) \
    _ASM_GET_TEMOTI_POKE_TYPE ret_wk1, ret_wk2, pos

    .macro  _ASM_GET_TEMOTI_POKE_TYPE ret_wk1, ret_wk2, pos
    .short  EV_SEQ_GET_TEMOTI_POKE_TYPE
    .short  \ret_wk1
    .short  \ret_wk2
    .short  \pos
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_PARAM_EXP
 * @brief �莝���|�P�����̓w�͒l���v���󂯎��
 * @param ret_wk    �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_PARAM_EXP( ret_wk , pos ) \
    _ASM_GET_TEMOTI_PARAM_EXP ret_wk , pos

  .macro  _ASM_GET_TEMOTI_PARAM_EXP ret_wk , pos
  .short  EV_SEQ_GET_PARAM_EXP
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_POKE_OWNER
 * @brief �莝���|�P�����̐e�����������`�F�b�N
 * @param ret_wk  �`�F�b�N���ʂ��󂯎�郏�[�N
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @retval  TRUE    �`�F�b�N�����|�P�����̐e�͎���
 * @retval  FALSE   �`�F�b�N�����|�P�����̐e�͑��l
 */
//--------------------------------------------------------------
#define _CHECK_POKE_OWNER( ret_wk, pos ) \
    _ASM_CHECK_POKE_OWNER ret_wk, pos

  .macro  _ASM_CHECK_POKE_OWNER ret_wk, pos
  .short  EV_SEQ_CHECK_POKE_OWNER
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _CHANGE_FORM_NO
 * @brief �莝���|�P�����̃t�H�����`�F���W
 * @param pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @param formno    �t�H�����i���o�[
 */
//--------------------------------------------------------------
#define _CHANGE_FORM_NO( pos, formno ) _ASM_CHANGE_FORM_NO pos, formno

  .macro  _ASM_CHANGE_FORM_NO pos, formno
  .short  EV_SEQ_CHG_FORM_NO
  .short  \pos
  .short  \formno
  .endm
  

//--------------------------------------------------------------
/**
 * @def _CHANGE_ROTOM_FORM_NO
 * @brief �莝���|�P�����̃t�H�����`�F���W
 * @param poke_pos     �`�F�b�N����|�P�����̈ʒu�i�O�`�T�j
 * @param waza_pos      �Z�̈ʒu�i0�`3�j
 * @param formno    �t�H�����i���o�[
 */
//--------------------------------------------------------------
#define _CHANGE_ROTOM_FORM_NO( poke_pos, waza_pos, formno ) _ASM_CHANGE_ROTOM_FORM_NO poke_pos, waza_pos, formno

  .macro  _ASM_CHANGE_ROTOM_FORM_NO poke_pos, waza_pos, formno
  .short  EV_SEQ_CHG_ROTOM_FORM_NO
  .short  \poke_pos
  .short  \waza_pos
  .short  \formno
  .endm
  
//--------------------------------------------------------------
/**
 * @def _GET_PARTY_POS
 * @brief �w�胂���X�^�[�i���o�[�̃|�P���莝���̂ǂ��ɂ��邩�𒲂ׂ�
 * @param monsno  �T�������X�^�[�i���o�[
 * @param ret_wk  TRUE�Ȃ甭�� FALSE�Ȃ疢����
 * @param pos     ���������|�P�����̈ʒu�i�O�`�T�j
 */
//--------------------------------------------------------------
#define _GET_PARTY_POS( monsno, ret_wk, pos ) _ASM_GET_PARTY_POS monsno, ret_wk, pos

  .macro  _ASM_GET_PARTY_POS monsno, ret_wk, pos
  .short  EV_SEQ_GET_PARTY_POS_BY_MONSNO
  .short  \monsno
  .short  \ret_wk
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_TEMOTI_GETPLACE
 * @brief �w��莝���|�P�����̕ߊl�ꏊID���`�F�b�N
 *
 * @param ret_wk      TRUE  �`�F�b�N�^�C�v�ƈ�v�@FALSE�@�s��v
 * @param poke_pos    �莝���|�W�V����
 * @param check_type  �ߊl�ꏊ�`�F�b�N�^�C�v�@POKE_GET_PLACE_CHECK_�`
 *        prog/src/field/script_def.h
 */
//--------------------------------------------------------------
#define _CHECK_TEMOTI_GETPLACE( ret_wk, poke_pos, check_type ) _ASM_CHECK_TEMOTI_GETPLACE ret_wk, poke_pos, check_type
  

  .macro  _ASM_CHECK_TEMOTI_GETPLACE ret_wk, poke_pos, check_type
  .short  EV_SEQ_CHECK_TEMOTI_GETPLACE
  .short  \ret_wk
  .short  \poke_pos
  .short  \check_type
  .endm

//--------------------------------------------------------------
/**
 * @def _GET_TEMOTI_GETDATE
 * @brief �w��莝���|�P�����̕ߊl���t���擾
 *
 * @param ret_year   �ߊl�N�̊i�[��
 * @param ret_month  �ߊl���̊i�[��
 * @param ret_day    �ߊl���̊i�[��
 * @param poke_pos  �莝���|�W�V����
 */
//--------------------------------------------------------------
#define _GET_TEMOTI_GETDATE( ret_year, ret_month, ret_day, poke_pos ) _ASM_GET_TEMOTI_GETDATE ret_year, ret_month, ret_day, poke_pos

  .macro  _ASM_GET_TEMOTI_GETDATE ret_year, ret_month, ret_day, poke_pos
  .short  EV_SEQ_GET_TEMOTI_GETPLACE
  .short  \ret_year
  .short  \ret_month
  .short  \ret_day
  .short  \poke_pos
  .endm

//======================================================================
// ����
//======================================================================
//--------------------------------------------------------------
/**
 * @def _ADD_GOLD
 * @brief �����𑝂₷
 * @param value ���₷���z(�~)
 */
//--------------------------------------------------------------
#define _ADD_GOLD( value ) \
    _ASM_ADD_GOLD value

  .macro _ASM_ADD_GOLD value
  .short EV_SEQ_ADD_GOLD
  .short \value
  .endm

//--------------------------------------------------------------
/**
 * @def _SUBTRACT_GOLD
 * @brief ���������炷
 * @param value ���炷���z(�~)
 */
//--------------------------------------------------------------
#define _SUBTRACT_GOLD( value ) \
    _ASM_SUBTRACT_GOLD value

  .macro _ASM_SUBTRACT_GOLD value
  .short EV_SEQ_SUBTRACT_GOLD
  .short \value
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_GOLD
 * @brief �w����z�������Ă��邩�ǂ����`�F�b�N����
 * @param ret_wk ���茋�ʂ��󂯎�郏�[�N
 * @param value  ���肷����z(�~)
 * @retval TRUE  �w����z�������Ă���ꍇ
 * @retval FALSE �w����z�������Ă��Ȃ��ꍇ
 */
//--------------------------------------------------------------
#define _CHECK_GOLD( ret_wk, value ) \
    _ASM_CHECK_GOLD ret_wk, value

  .macro _ASM_CHECK_GOLD ret_wk, value
  .short EV_SEQ_CHECK_GOLD
  .short \ret_wk
  .short \value
  .endm


//======================================================================
// �{�b�N�X�֘A
//======================================================================

//--------------------------------------------------------------
/**
 * @def _GET_BOX_POKE_COUNT
 * @brief �{�b�N�X���̃|�P���������擾����
 * @param ret_wk �|�P���������󂯎�郏�[�N
 * @param mode �J�E���g���[�h( POKECOUNT_MODE_XXXX )
 */
//--------------------------------------------------------------
#define _GET_BOX_POKE_COUNT( ret_wk, mode ) \
    _ASM_GET_BOX_POKE_COUNT ret_wk, mode

  .macro _ASM_GET_BOX_POKE_COUNT ret_wk, mode
  .short EV_SEQ_GET_BOX_POKE_COUNT
  .short \ret_wk
  .short \mode
  .endm
    
  
//======================================================================
// �}�b�v�J�ڊ֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �}�b�v�J�ڃR�}���h (BGM�ύX�Ȃ�)
 * @param zone_id   �ړ���]�[��ID�w��
 * @param gx        �ړ���w�ʒu�w��i�O���b�h�P�ʁj
 * @param gz        �ړ���y�ʒu�w��i�O���b�h�P�ʁj
 * @param dir       �ړ���̌���
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_BGM_KEEP( zone_id, gx, gz, dir )  \
    _ASM_MAP_CHANGE_BGM_KEEP zone_id, gx, gz, dir

  .macro  _ASM_MAP_CHANGE_BGM_KEEP zone_id, gx, gz, dir
  .short  EV_SEQ_MAP_CHANGE_BGM_KEEP
  .short  \zone_id
  .short  \gx
  .short  \gz
  .short  \dir
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�J�ڃR�}���h�i�����j
 * @param zone_id   �ړ���]�[��ID�w��
 * @param gx        �ړ���w�ʒu�w��i�O���b�h�P�ʁj
 * @param gz        �ړ���x�ʒu�w��i�O���b�h�P�ʁj
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_SAND_STREAM( zone_id, gx, gz )  \
    _ASM_MAP_CHANGE_SAND_STREAM zone_id, gx, gz

  .macro  _ASM_MAP_CHANGE_SAND_STREAM zone_id, gx, gz
  .short  EV_SEQ_MAP_CHANGE_SAND_STREAM
  .short  \zone_id
  .short  \gx
  .short  \gz
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�J�ڃR�}���h
 * @param zone_id   �ړ���]�[��ID�w��
 * @param gx        �ړ���w�ʒu�w��i�O���b�h�P�ʁj
 * @param gz        �ړ���y�ʒu�w��i�O���b�h�P�ʁj
 * @param dir       �ړ���̌���
 */
//--------------------------------------------------------------
#define _MAP_CHANGE( zone_id, gx, gz, dir )  \
    _ASM_MAP_CHANGE zone_id, gx, gz, dir

  .macro  _ASM_MAP_CHANGE zone_id, gx, gz, dir
  .short  EV_SEQ_MAP_CHANGE
  .short  \zone_id
  .short  \gx
  .short  \gz
  .short  \dir
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�J�ڃR�}���h ( ���[�v )
 * @param zone_id   �ړ���]�[��ID�w��
 * @param gx        �ړ���w�ʒu�w��i�O���b�h�P�ʁj
 * @param gz        �ړ���y�ʒu�w��i�O���b�h�P�ʁj
 * @param dir       �ړ���̌���
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_WARP( zone_id, gx, gz, dir )  \
    _ASM_MAP_CHANGE_WARP zone_id, gx, gz, dir

  .macro  _ASM_MAP_CHANGE_WARP zone_id, gx, gz, dir
  .short  EV_SEQ_MAP_CHANGE_WARP
  .short  \zone_id
  .short  \gx
  .short  \gz
  .short  \dir
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�J�ڃR�}���h(���j�I�����[��)
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_TO_UNION() \
    _ASM_MAP_CHANGE_TO_UNION 

  .macro  _ASM_MAP_CHANGE_TO_UNION 
  .short  EV_SEQ_MAP_CHANGE_TO_UNION
  .endm

//--------------------------------------------------------------
/**
 * �t�F�[�h�Ȃ��}�b�v�J�ڃR�}���h
 * @param zone_id   �ړ���]�[��ID�w��
 * @param gx        �ړ���w�ʒu�w��i�O���b�h�P�ʁj
 * @param gy        �ړ���x�ʒu�w��i�O���b�h�P�ʁj
 * @param gz        �ړ���y�ʒu�w��i�O���b�h�P�ʁj
 * @param dir       �ړ���̌���
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_NO_FADE( zone_id, gx, gy, gz, dir )  \
    _ASM_MAP_CHANGE_NO_FADE zone_id, gx, gy, gz, dir

  .macro  _ASM_MAP_CHANGE_NO_FADE zone_id, gx, gy, gz, dir
  .short  EV_SEQ_MAP_CHANGE_NO_FADE
  .short  \zone_id
  .short  \gx
  .short  \gy
  .short  \gz
  .short  \dir
  .endm  

//======================================================================
//
//
//  ����󋵃M�~�b�N�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �W���R�}���h�@�d�C�W��������
 */
//--------------------------------------------------------------
#define _GYM_ELEC_INIT( evt1, evt2 ) \
    _ASM_GYM_ELEC_INIT evt1, evt2

  .macro  _ASM_GYM_ELEC_INIT evt1, evt2
  .short  EV_SEQ_GYM_ELEC_INIT
  .short  \evt1
  .short  \evt2
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�d�C�W���X�C�b�`
 * @param sw_idx   �X�C�b�`�C���f�b�N�X0�`3
 */
//--------------------------------------------------------------
#define _GYM_ELEC_PUSH_SW( sw_idx ) \
    _ASM_GYM_ELEC_PUSH_SW sw_idx 

  .macro  _ASM_GYM_ELEC_PUSH_SW sw_idx
  .short  EV_SEQ_GYM_ELEC_PUSH_SW
  .short  \sw_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�d�C�W���J�v�Z���g���[�i�[�폟���㏈��
 * @param cap_idx   ��l�ڂɏ����̂Ƃ��F1�@��l�ڂɏ����̂Ƃ��F2
 */
//--------------------------------------------------------------
#define _GYM_ELEC_SET_TR_ENC_FLG( cap_idx ) \
    _ASM_GYM_ELEC_SET_TR_ENC_FLG cap_idx 

  .macro  _ASM_GYM_ELEC_SET_TR_ENC_FLG cap_idx
  .short  EV_SEQ_GYM_ELEC_SET_TR_ENC_FLG
  .short  \cap_idx
  .endm

  

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�m�[�}���W��������
 * @param rm_idx   �M�~�b�N�̂��镔���C���f�b�N�X0�`1
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_INIT() \
    _ASM_GYM_NORMAL_INIT 

  .macro  _ASM_GYM_NORMAL_INIT
  .short  EV_SEQ_GYM_NORMAL_INIT
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�m�[�}���W�����b�N����
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_UNROCK() \
    _ASM_GYM_NORMAL_UNROCK 

  .macro  _ASM_GYM_NORMAL_UNROCK
  .short  EV_SEQ_GYM_NORMAL_UNROCK
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�m�[�}���W�����b�N�����`�F�b�N
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_CHK_UNROCK(ret_wk) \
    _ASM_GYM_NORMAL_CHK_UNROCK ret_wk

  .macro  _ASM_GYM_NORMAL_CHK_UNROCK ret_wk
  .short  EV_SEQ_GYM_NORMAL_CHK_UNROCK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�m�[�}���W���{�I�ړ�
 * @param wall_idx   �{�I�C���f�b�N�X�@���݂�0�Œ�
 */
//--------------------------------------------------------------
#define _GYM_NORMAL_MOVE_WALL(wall_idx) \
    _ASM_GYM_NORMAL_MOVE_WALL wall_idx

  .macro  _ASM_GYM_NORMAL_MOVE_WALL wall_idx
  .short  EV_SEQ_GYM_NORMAL_MOVE_WALL
  .short  \wall_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�A���`�W��������
 */
//--------------------------------------------------------------
#define _GYM_ANTI_INIT() \
    _ASM_GYM_ANTI_INIT

  .macro  _ASM_GYM_ANTI_INIT
  .short  EV_SEQ_GYM_ANTI_INIT
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�A���`�W���X�C�b�`�I��
 * @param sw_idx   �X�C�b�`�C���f�b�N�X0�`8
 * @note  0,1,2�F ���ꂼ��ΐ����̓�����{�^���A3,6:�΂̃n�Y���{�^���@4,7:���̃n�Y���{�^�� 5,8:���̃n�Y���{�^��
 */
//--------------------------------------------------------------
#define _GYM_ANTI_SW_ON(sw_idx) \
    _ASM_GYM_ANTI_SW_ON sw_idx

  .macro  _ASM_GYM_ANTI_SW_ON sw_idx
  .short  EV_SEQ_GYM_ANTI_SW_ON
  .short  \sw_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�A���`�W���X�C�b�`�I�t
 * @param sw_idx   �X�C�b�`�C���f�b�N�X0�`8
 * @note  0,1,2�F ���ꂼ��ΐ����̓�����{�^���A3,6:�΂̃n�Y���{�^���@4,7:���̃n�Y���{�^�� 5,8:���̃n�Y���{�^��
 */
//--------------------------------------------------------------
#define _GYM_ANTI_SW_OFF(sw_idx) \
    _ASM_GYM_ANTI_SW_OFF sw_idx

  .macro  _ASM_GYM_ANTI_SW_OFF sw_idx
  .short  EV_SEQ_GYM_ANTI_SW_OFF
  .short  \sw_idx
  .endm
  
//--------------------------------------------------------------
/**
 * �W���R�}���h�@�A���`�W���X�C�b�`�I��
 * @param door_idx   �h�A�C���f�b�N�X0�`2
 */
//--------------------------------------------------------------
#define _GYM_ANTI_OPEN_DOOR(door_idx) \
    _ASM_GYM_ANTI_OPEN_DOOR door_idx

  .macro  _ASM_GYM_ANTI_OPEN_DOOR door_idx
  .short  EV_SEQ_GYM_ANTI_OPEN_DOOR
  .short  \door_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@���W��������
 */
//--------------------------------------------------------------
#define _GYM_INSECT_INIT() \
    _ASM_GYM_INSECT_INIT

  .macro  _ASM_GYM_INSECT_INIT
  .short  EV_SEQ_GYM_INSECT_INIT
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@���W���X�C�b�`�I��
 * @param sw_idx   �X�C�b�`�C���f�b�N�X0�`7
 */
//--------------------------------------------------------------
#define _GYM_INSECT_SW_ON(sw_idx) \
    _ASM_GYM_INSECT_SW_ON sw_idx

  .macro  _ASM_GYM_INSECT_SW_ON sw_idx
  .short  EV_SEQ_GYM_INSECT_SW_ON
  .short  \sw_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@���W���|�[������
 * @param pl_idx   �|�[���C���f�b�N�X0�`9
 */
//--------------------------------------------------------------
#define _GYM_INSECT_PL_ON(pl_idx) \
    _ASM_GYM_INSECT_PL_ON pl_idx

  .macro  _ASM_GYM_INSECT_PL_ON pl_idx
  .short  EV_SEQ_GYM_INSECT_PL_ON
  .short  \pl_idx
  .endm
  
//--------------------------------------------------------------
/**
 * �W���R�}���h�@���W���g���[�i�[�g���b�v
 * @param trp_evt_idx   �C���f�b�N�X0�`1
 */
//--------------------------------------------------------------
#define _GYM_INSECT_TR_TRAP_ON(trp_evt_idx) \
    _ASM_GYM_INSECT_TR_TRAP_ON trp_evt_idx

  .macro  _ASM_GYM_INSECT_TR_TRAP_ON trp_evt_idx
  .short  EV_SEQ_GYM_INSECT_TR_TRAP_ON
  .short  \trp_evt_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�n�ʃW�����t�g�ړ�
 * @param lift_idx   �C���f�b�N�X0�`5
 */
//--------------------------------------------------------------
#define _GYM_GROUND_MV_LIFT(lift_idx) \
    _ASM_GYM_GROUND_MV_LIFT lift_idx

  .macro  _ASM_GYM_GROUND_MV_LIFT lift_idx
  .short  EV_SEQ_GYM_GROUND_MV_LIFT
  .short  \lift_idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�n�ʃW���o�����t�g�ړ�
 * @param exit   �W������o��Ƃ�1�@�W���ɓ���Ƃ�0
 */
//--------------------------------------------------------------
#define _GYM_GROUND_EXIT_LIFT(exit) \
    _ASM_GYM_GROUND_EXIT_LIFT exit

  .macro  _ASM_GYM_GROUND_EXIT_LIFT exit
  .short  EV_SEQ_GYM_GROUND_EXIT_LIFT
  .short  \exit
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�n�ʃW���G���g�����X�o�����t�g�ړ�
 * @param exit   �W���ɍs���Ƃ�1�@�W������߂�Ƃ�0
 */
//--------------------------------------------------------------
#define _GYM_GROUND_ENT_EXIT_LIFT(exit) \
    _ASM_GYM_GROUND_ENT_EXIT_LIFT exit

  .macro  _ASM_GYM_GROUND_ENT_EXIT_LIFT exit
  .short  EV_SEQ_GYM_GROUND_ENT_EXIT_LIFT
  .short  \exit
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�X�W���X�C�b�`�A�j��
 * @param idx   �M�~�b�N�C���f�b�N�X0�`2
 */
//--------------------------------------------------------------
#define _GYM_ICE_SW_ANM(idx) _ASM_GYM_ICE_SW_ANM idx

  .macro  _ASM_GYM_ICE_SW_ANM idx
  .short  EV_SEQ_GYM_ICE_SW_ANM
  .short  \idx
  .endm
  
//--------------------------------------------------------------
/**
 * �W���R�}���h�@�X�W���ǃA�j��
 * @param idx   �M�~�b�N�C���f�b�N�X0�`2
 */
//--------------------------------------------------------------
#define _GYM_ICE_WALL_ANM(idx) _ASM_GYM_ICE_WALL_ANM idx

  .macro  _ASM_GYM_ICE_WALL_ANM idx
  .short  EV_SEQ_GYM_ICE_WALL_ANM
  .short  \idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�X�W�����[���Ƀ`�F���W
 * @param idx   �M�~�b�N�C���f�b�N�X0�`2
 */
//--------------------------------------------------------------
#define _GYM_ICE_CHG_RAIL(idx) _ASM_GYM_ICE_CHG_RAIL idx

  .macro  _ASM_GYM_ICE_CHG_RAIL idx
  .short  EV_SEQ_GYM_ICE_CHG_RAIL
  .short  \idx
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�h���S���W��������
 */
//--------------------------------------------------------------
#define _GYM_DRAGON_INIT() \
    _ASM_GYM_DRAGON_INIT

  .macro  _ASM_GYM_DRAGON_INIT
  .short  EV_SEQ_GYM_DRAGON_INIT
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�h���S���W���M�~�b�N����
 */
//--------------------------------------------------------------
#define _GYM_DRAGON_CALL_GMK(head, arm) _ASM_GYM_DRAGON_CALL_GMK head, arm

  .macro  _ASM_GYM_DRAGON_CALL_GMK head, arm
  .short  EV_SEQ_GYM_DRAGON_CALL_GMK
  .short  \head
  .short  \arm
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�h���S���W�����@��э~��
 */
//--------------------------------------------------------------
#define _GYM_DRAGON_JUMP_DOWN(dir) _ASM_GYM_DRAGON_JUMP_DOWN dir

  .macro  _ASM_GYM_DRAGON_JUMP_DOWN dir
  .short  EV_SEQ_GYM_DRAGON_JUMP_DOWN
  .short  \dir
  .endm

//--------------------------------------------------------------
/**
 * �W���R�}���h�@�h���S���W���r�ړ�
 */
//--------------------------------------------------------------
#define _GYM_DRAGON_MOVE_ARM(head, arm) _ASM_GYM_DRAGON_MOVE_ARM head, arm

  .macro  _ASM_GYM_DRAGON_MOVE_ARM head, arm
  .short  EV_SEQ_GYM_DRAGON_MOVE_ARM
  .short  \head
  .short  \arm
  .endm  
  
//--------------------------------------------------------------
/**
 * �W���R�}���h�@�h���S���W����ړ�
 */
//--------------------------------------------------------------
#define _GYM_DRAGON_MOVE_HEAD(head) _ASM_GYM_DRAGON_MOVE_HEAD head

  .macro  _ASM_GYM_DRAGON_MOVE_HEAD head
  .short  EV_SEQ_GYM_DRAGON_MOVE_HEAD
  .short  \head
  .endm

//======================================================================
//
//  ���f���A�j���֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 * @def _POKECEN_RECOVER_ANIME
 * @brief �|�P�Z���񕜃A�j��
 * @param poke_count    �莝���̃|�P�����̐�
 */
//--------------------------------------------------------------
#define _POKECEN_RECOVER_ANIME( poke_count )  \
  _ASM_POKECEN_RECOVER_ANIME poke_count

  .macro  _ASM_POKECEN_RECOVER_ANIME poke_count
  .short  EV_SEQ_POKECEN_RECOVER_ANIME
  .short  \poke_count
  .endm

//--------------------------------------------------------------
/**
 * @def _POKECEN_PC_ON
 * @brief �|�P�Z��PC�N���A�j��
 */
//--------------------------------------------------------------
#define _POKECEN_PC_ON()  \
  _ASM_POKECEN_PC_ON

  .macro  _ASM_POKECEN_PC_ON
  .short  EV_SEQ_POKECEN_PC_ON
  .endm

//--------------------------------------------------------------
/**
 * @def _POKECEN_PC_RUN
 * @brief �|�P�Z��PC�N�����A�j��
 */
//--------------------------------------------------------------
#define _POKECEN_PC_RUN()  \
  _ASM_POKECEN_PC_RUN

  .macro  _ASM_POKECEN_PC_RUN
  .short  EV_SEQ_POKECEN_PC_RUN
  .endm

//--------------------------------------------------------------
/**
 * @def _POKECEN_PC_OFF
 * @brief �|�P�Z��PC��~�A�j��
 */
//--------------------------------------------------------------
#define _POKECEN_PC_OFF()  \
  _ASM_POKECEN_PC_OFF

  .macro  _ASM_POKECEN_PC_OFF
  .short  EV_SEQ_POKECEN_PC_OFF
  .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_CREATE
 * @brief �z�u���f���A�j���R���g���[���[�̐���
 * @param ret_wk  anm_id�������Ƃ邽�߂̃��[�N
 * @param bm_id   �z�u���f���̎�ގw��ID�iscript_def.h���Q�Ɓj
 * @param gx      �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz      �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 *
 * ret_wk�Ŏ󂯎�����A�j���w��ID���g���Ĕz�u���f���A�j���e�R�}���h
 * �i_BMODEL_ANIME_DELETE/_BMODEL_ANIME_SET/_BMODEL_ANIME_WAIT�j��
 * �Ăяo���B
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_CREATE( ret_wk, bm_id, gx, gz ) \
    _ASM_BMODEL_ANIME_CREATE ret_wk, bm_id, gx, gz

    .macro _ASM_BMODEL_ANIME_CREATE ret_wk, bm_id, gx, gz
    .short  EV_SEQ_BMODEL_ANIME_CREATE
    .short  \ret_wk
    .short  \bm_id
    .short  \gx
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_DELETE
 * @brief �z�u���f���A�j���R���g���[���[�̍폜
 * @param anm_id  �z�u���f�����w�肷�邽�߂�ID
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_DELETE( anm_id ) \
     _ASM_BMODEL_ANIME_DELETE anm_id

     .macro _ASM_BMODEL_ANIME_DELETE anm_id
     .short EV_SEQ_BMODEL_ANIME_DELETE
     .short \anm_id
     .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_SET
 * @brief �z�u���f���A�j���̃Z�b�g
 * @param anm_id    �z�u���f�����w�肷�邽�߂�ID
 * @param anm_type  �A�j���[�V�����̎�ގw��
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_SET( anm_id, anm_type ) \
    _ASM_BMODEL_ANIME_SET anm_id, anm_type

    .macro _ASM_BMODEL_ANIME_SET anm_id, anm_type
    .short  EV_SEQ_BMODEL_ANIME_SET
    .short  \anm_id
    .short  \anm_type
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_LOOP_SET
 * @brief �z�u���f�����[�v�A�j���̃Z�b�g
 * @param anm_id    �z�u���f�����w�肷�邽�߂�ID
 * @param anm_type  �A�j���[�V�����̎�ގw��
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_LOOP_SET( anm_id, anm_type ) \
    _ASM_BMODEL_ANIME_LOOP_SET anm_id, anm_type

    .macro _ASM_BMODEL_ANIME_LOOP_SET anm_id, anm_type
    .short  EV_SEQ_BMODEL_ANIME_LOOP_SET
    .short  \anm_id
    .short  \anm_type
    .endm    

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_STOP
 * @brief �z�u���f���A�j���̒�~
 * @param anm_id  �z�u���f�����w�肷�邽�߂�ID
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_STOP( anm_id ) \
    _ASM_BMODEL_ANIME_STOP anm_id

    .macro _ASM_BMODEL_ANIME_STOP anm_id
    .short  EV_SEQ_BMODEL_ANIME_STOP
    .short  \anm_id
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_ANIME_WAIT
 * @brief �z�u���f���A�j���̏I���҂�
 * @param anm_id  �z�u���f�����w�肷�邽�߂�ID
 */
//--------------------------------------------------------------
#define _BMODEL_ANIME_WAIT( anm_id ) \
    _ASM_BMODEL_ANIME_WAIT anm_id

    .macro _ASM_BMODEL_ANIME_WAIT anm_id
    .short  EV_SEQ_BMODEL_ANIME_WAIT
    .short  \anm_id
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_DIRECT_ANIME_SET_FINISHED
 * @brief �z�u���f���A�j����Ԃ��I����ԂŌŒ肷��
 * @param bm_id   �z�u���f���̎�ގw��ID�iscript_def.h���Q�Ɓj
 * @param gx      �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz      �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 *
 * @note
 * �E�O���[�o���ȃA�j����Ԃ𑀍삷��̂ŁA�}�b�v���̓�����ނ̂��ׂĂ̔z�u���f���ɓK�p�����
 * �E��Ԃ͕ێ�����Ȃ����߁A�}�b�v�č\�����ɖ���Ăяo���K�v������
 *
 * �o�b�W�`�F�b�N�Q�[�g���J���������ɂ��邽�߂̃R�}���h�ŁA���ł͎g�������Ȃ������B
 */
//--------------------------------------------------------------
#define _BMODEL_DIRECT_ANIME_SET_FINISHED( bm_id, gx, gz )  \
    _ASM_BMODEL_DIRECT_ANIME_SET_FINISHED bm_id, gx, gz

    .macro  _ASM_BMODEL_DIRECT_ANIME_SET_FINISHED bm_id, gx, gz
    .short  EV_SEQ_BMODEL_DIRECT_ANIME_SET_FINISHED
    .short  \bm_id
    .short  \gx
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_DIRECT_ANIME_SET_LOOP
 * @brief �z�u���f���A�j����Ԃ����[�v��ԂŌŒ肷��
 * @param bm_id   �z�u���f���̎�ގw��ID�iscript_def.h���Q�Ɓj
 * @param gx      �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz      �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 *
 * @note
 * �E�O���[�o���ȃA�j����Ԃ𑀍삷��̂ŁA�}�b�v���̓�����ނ̂��ׂĂ̔z�u���f���ɓK�p�����
 * �E��Ԃ͕ێ�����Ȃ����߁A�}�b�v�č\�����ɖ���Ăяo���K�v������
 *
 */
//--------------------------------------------------------------
#define _BMODEL_DIRECT_ANIME_SET_LOOP( bm_id, gx, gz )  \
    _ASM_BMODEL_DIRECT_ANIME_SET_LOOP bm_id, gx, gz

    .macro  _ASM_BMODEL_DIRECT_ANIME_SET_LOOP bm_id, gx, gz
    .short  EV_SEQ_BMODEL_DIRECT_ANIME_SET_LOOP
    .short  \bm_id
    .short  \gx
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_CHANGE_VIEW_FLAG
 * @brief �z�u���f���̕\��ON/OFF
 * @param bm_id   �z�u���f���̎�ގw��ID�iscript_def.h���Q�Ɓj
 * @param gx      �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz      �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 * @param flag    ON/OFF�w��
 *
 */
//--------------------------------------------------------------
#define _BMODEL_CHANGE_VIEW_FLAG( bm_id, gx, gz, flag ) \
    _ASM_BMODEL_CHANGE_VIEW_FLAG bm_id, gx, gz, flag

    .macro  _ASM_BMODEL_CHANGE_VIEW_FLAG bm_id, gx, gz, flag
    .short  EV_SEQ_BMODEL_DIRECT_CHANGE_VIEW_FLAG
    .short  \bm_id
    .short  \gx
    .short  \gz
    .short  \flag
    .endm

//--------------------------------------------------------------
/**
 * @def _BMODEL_DIRECT_CHANGE_MODEL_ID
 * @brief �z�u���f���̃��f��ID��ύX
 * @param search_id   �z�u���f���̎�ގw��ID�iscript_def.h���Q�Ɓj
 * @param gx          �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz          �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 * @param model_id    ���f���w��ID
 *
 * ���f���w��ID�𒼐ڃX�N���v�g����L�q����\��͂Ȃ��B
 * �����_GET_TRAIN_MODEL_ID()�ł̂ݎg�p�\��
 */
//--------------------------------------------------------------
#define _BMODEL_DIRECT_CHANGE_MODEL_ID( search_id, gx, gz, model_id ) \
    _ANM_BMODEL_DIRECT_CHANGE_MODEL_ID search_id, gx, gz, model_id

    .macro  _ANM_BMODEL_DIRECT_CHANGE_MODEL_ID search_id, gx, gz, model_id
    .short  EV_SEQ_BMODEL_DIRECT_CHANGE_MODEL_ID
    .short  \search_id
    .short  \gx
    .short  \gz
    .short  \model_id
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_TRAIN_MODEL_ID
 * @brief   �g���C���^�E���̓d�ԃ��f��ID���擾
 * @param index   �d�Ԃ̎�ގw��i�O�`�X�j
 * @param ret_wk  �d�Ԃ̃��f��ID���󂯎�郏�[�N
 *
 * �g���C���^�E���ł̂ݎg�p����R�}���h�B
 */
//--------------------------------------------------------------
#define _GET_TRAIN_MODEL_ID( index, ret_wk ) \
    _ASM_GET_TRAIN_MODEL_ID index, ret_wk

    .macro  _ASM_GET_TRAIN_MODEL_ID index, ret_wk
    .short  EV_SEQ_GET_TRAIN_MODEL_ID
    .short  \index
    .short  \ret_wk
    .endm

//======================================================================
//
//
// ��ĉ��֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * ��ĉ��̃|�P�����Ƀ^�}�S�����܂ꂽ���ǂ����̃`�F�b�N
 * 
 * @param ret_wk ���ʂ̊i�[��
 * @return       �^�}�S������ꍇ TRUE
 */
//--------------------------------------------------------------
#define _CHECK_SODATEYA_HAVE_EGG( ret_wk ) \
    _ASM_CHECK_SODATEYA_HAVE_EGG ret_wk

  .macro _ASM_CHECK_SODATEYA_HAVE_EGG ret_wk
  .short EV_SEQ_CHECK_SODATEYA_HAVE_EGG
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��̃^�}�S���󂯎��
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_EGG() \
    _ASM_GET_SODATEYA_EGG

  .macro _ASM_GET_SODATEYA_EGG
  .short EV_SEQ_GET_SODATEYA_EGG
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��̃^�}�S���폜����
 */
//--------------------------------------------------------------
#define _DELETE_SODATEYA_EGG() \
    _ASM_DELETE_SODATEYA_EGG

  .macro _ASM_DELETE_SODATEYA_EGG
  .short EV_SEQ_DELETE_SODATEYA_EGG
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����̐����擾����
 *
 * @param ret_wk ���ʂ̊i�[��
 * @return       ��ĉ��ɗa���Ă���|�P�����̐�
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKEMON_NUM( ret_wk ) \
    _ASM_GET_SODATEYA_POKEMON_NUM ret_wk

  .macro _ASM_GET_SODATEYA_POKEMON_NUM ret_wk
  .short EV_SEQ_GET_SODATEYA_POKEMON_NUM
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���2�̂̃|�P�����̑������`�F�b�N����
 *
 * @param ret_wk ���ʂ̊i�[��
 * @return 
 */
//--------------------------------------------------------------
#define _SODATEYA_LOVE_CHECK( ret_wk ) \
    _ASM_SODATEYA_LOVE_CHECK ret_wk

  .macro _ASM_SODATEYA_LOVE_CHECK ret_wk
  .short EV_SEQ_SODATEYA_LOVE_CHECK
  .short \ret_wk
  .endm
//--------------------------------------------------------------
/**
 * �|�P��������ĉ��ɗa����
 *
 * @param pos �a����|�P�����̎莝���ԍ�
 */
//--------------------------------------------------------------
#define _POKE_TO_SODATEYA( pos ) \
    _ASM_POKE_TO_SODATEYA pos

  .macro _ASM_POKE_TO_SODATEYA pos
  .short EV_SEQ_POKE_TO_SODATEYA
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * �|�P��������ĉ�����󂯎��
 *
 * @param pos �󂯎��|�P�����̈�ĉ����ԍ�(0 or 1)
 */
//--------------------------------------------------------------
#define _POKE_FROM_SODATEYA( pos ) \
    _ASM_POKE_FROM_SODATEYA pos

  .macro _ASM_POKE_FROM_SODATEYA pos
  .short EV_SEQ_POKE_FROM_SODATEYA
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����̃����X�^�[�i���o�[���擾����
 *
 * @param ret_wk �擾���������X�^�[�i���o�[�̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����̃����X�^�[�i���o�[
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_MONSNO( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_MONSNO ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_MONSNO ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_MONSNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����̌`��i���o�[���擾����
 *
 * @param ret_wk �擾�����`��i���o�[�̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����̌`��i���o�[
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_FORMNO( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_FORMNO ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_FORMNO ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_FORMNO
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����̌��݂̃��x�����擾����
 *
 * @param ret_wk �擾�������x���̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����̃��x��
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_LV( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_LV ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_LV ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_LV
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����ɂ���, �����������x�����擾����
 *
 * @param ret_wk �擾�����l�̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����������������x��
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_GROWUP_LV( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_GROWUP_LV ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_GROWUP_LV ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_GROWUP_LV
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����ɂ���, ������藿�����擾����
 *
 * @param ret_wk �擾�����l�̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����̈�����藿��
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_CHARGE( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_CHARGE ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_CHARGE ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_CHARGE
  .short \ret_wk
  .short \pos
  .endm

//--------------------------------------------------------------
/**
 * �莝���|�P�����I����ʂ��Ăяo��
 *
 * @param ret_wk �I�����ʂ̊i�[��
 * @retval u16   �I������
 */
//--------------------------------------------------------------
#define _SODATEYA_SELECT_POKEMON( ret_wk ) \
    _ASM_SODATEYA_SELECT_POKEMON ret_wk

  .macro _ASM_SODATEYA_SELECT_POKEMON ret_wk
  .short EV_SEQ_SODATEYA_SELECT_POKEMON
  .short \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * ��ĉ��ɗa���Ă���|�P�����ɂ���, ���ʂ��擾����
 *
 * @param ret_wk �擾�����l�̊i�[��
 * @param pos    �擾�Ώۃ|�P�����̈�ĉ����C���f�b�N�X
 * @retval u16   �w��|�P�����̐���
 *               �I�X = 0
 *               ���X = 1
 *               ���ʂȂ� = 2
 */
//--------------------------------------------------------------
#define _GET_SODATEYA_POKE_SEX( ret_wk, pos ) \
    _ASM_GET_SODATEYA_POKE_SEX ret_wk, pos

  .macro _ASM_GET_SODATEYA_POKE_SEX ret_wk, pos
  .short EV_SEQ_GET_SODATEYA_POKE_SEX
  .short \ret_wk
  .short \pos
  .endm

//======================================================================
//  �T�u�v���Z�X�R�[���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�v���Z�X�𐶐�����
 */
//--------------------------------------------------------------
#define _FIELD_OPEN() _ASM_FIELD_OPEN

  .macro _ASM_FIELD_OPEN
  .short EV_SEQ_FIELD_OPEN
  .endm

//--------------------------------------------------------------
/**
 * �t�B�[���h�v���Z�X���I������
 */
//--------------------------------------------------------------
#define _FIELD_CLOSE() _ASM_FIELD_CLOSE

  .macro _ASM_FIELD_CLOSE
  .short EV_SEQ_FIELD_CLOSE
  .endm

//--------------------------------------------------------------
/**
 * �T�u�v���Z�X���[�N���������
 *
 * ���g�p�͋ʓc����̋����ł��B
 * �@�v���Z�X�R�[���͌����Ƃ��āA�Ăяo���`�߂�l�擾�܂ł�One�R�}���h�Ŏ�������݌v�ɂ��邱�ƁI
 */
//--------------------------------------------------------------
#define _FREE_SUBPROC_WORK() _ASM_SUBPROC_WORK

  .macro _ASM_SUBPROC_WORK
  .short EV_SEQ_FREE_SUB_PROC_WORK
  .endm

//--------------------------------------------------------------
/**
 *  �o�b�O�v���Z�X���Ăяo��
 *  @param  mode  �Ăяo�����[�h script_def.h SCR_BAGMODE_SELL��
 *  @param  ret_mode  �o�b�O�̏I�����[�h�@script_def.h SCR_PROC_RETMODE_EXIT ��
 *  @param  ret_item  �I���A�C�e��No(0�Ȃ�I���A�C�e������)
 *
 *  �T�u�v���Z�X���[�N��������Ȃ��̂ŁA�g�p��� _FREE_SUBPROC_WORK()���Ăяo������
 */
//--------------------------------------------------------------
#define _CALL_BAG_PROC( mode, ret_mode, ret_item ) \
    _ASM_CALL_BAG_PROC mode, ret_mode, ret_item

  .macro  _ASM_CALL_BAG_PROC mode, ret_mode, ret_item
  .short  EV_SEQ_CALL_BAG_PROC
  .short  \mode
  .short  \ret_mode
  .short  \ret_item
  .endm

//--------------------------------------------------------------
/**
 *  @brief �{�b�N�X�v���Z�X���Ăяo��
 *  @param ret_end_mode �{�b�N�X�I�����[�h���󂯎�郏�[�N
 *  @param mode         �{�b�N�X�Ăяo�����[�h( BOX_MODE_xxxx )
 *
 *  ���Ăяo�����[�h prog/include/app/box_mode.h
 */
//--------------------------------------------------------------
#define _CALL_BOX_PROC( ret_end_mode, mode ) \
    _ASM_CALL_BOX_PROC ret_end_mode, mode

  .macro  _ASM_CALL_BOX_PROC ret_end_mode, mode
  .short  EV_SEQ_CALL_BOX_PROC
  .short  \ret_end_mode
  .short  \mode
  .endm

//--------------------------------------------------------------
/**
 *  @brief ���[���{�b�N�X�v���Z�X���Ăяo��
 */
//--------------------------------------------------------------
#define _CALL_MAILBOX_PROC() \
    _ASM_CALL_MAILBOX_PROC

  .macro  _ASM_CALL_MAILBOX_PROC
  .short  EV_SEQ_CALL_MAILBOX_PROC
  .endm

//--------------------------------------------------------------
/**
 * @def _CALL_GEONET_PROC
 * @brief   �W�I�l�b�g�n���V�̌Ăяo��
 *
 * �W�I�l�b�g�̒n���V��ʂ��Ăяo���܂�
 */
//--------------------------------------------------------------
#define _CALL_GEONET_PROC() _ASM_CALL_GEONET_PROC

  .macro  _ASM_CALL_GEONET_PROC
  .short  EV_SEQ_CALL_GEONET_PROC
  .endm

//--------------------------------------------------------------
/**
 * @def _CALL_ZUKAN_AWARD
 * @brief   �}�ӊ����\���󁕘H���}�\���A�v���Ăяo��
 *
 * �}�ӌĂяo��ID
 *  �n���}�� SCR_ZUKAN_AWARD_CHIHOU
 *  �S���}�� SCR_ZUKAN_AWARD_ZENKOKU
 */
//--------------------------------------------------------------
//�}��
#define _CALL_ZUKAN_AWARD( demo_id, pattern ) \
    _ASM_ZUKAN_AWARD_PROC_CALL demo_id, pattern

//�H���}
#define _CALL_SUBWAY_ROUTE_MAP() \
    _ASM_ZUKAN_AWARD_PROC_CALL SCR_ZUKAN_AWARD_SUBWAY_ROUTE_MAP, 0 

  .macro  _ASM_CALL_ZUKAN_AWARD demo_id, pattern
  .short  EV_SEQ_CALL_ZUKAN_AWARD
  .short  \demo_id
  .short  \pattern
  .endm

//--------------------------------------------------------------
/**
 * @def
 * @brief
 * @param ret_wk
 */
//--------------------------------------------------------------
#define _CALL_FIRST_POKE_SELECT( ret_wk ) \
    _ASM_FIRST_POKE_SELECT_CALL ret_wk

  .macro  _ASM_CALL_FIRST_POKE_SELECT ret_wk
  .short  EV_SEQ_CALL_FIRST_POKE_SELECT
  .short  \ret_wk
  .endm

//======================================================================
//  �V���b�v�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @def _SHOP_CALL
 * @brief �ȈՃC�x���g�R�}���h�F�V���b�v�C�x���g�Ăяo��
 * @param shop_id
 */
//--------------------------------------------------------------
#define _SHOP_CALL( ) _ASM_SHOP_CALL  SCR_SHOPID_NULL,TRUE 

//--------------------------------------------------------------
/**
 * @def _SHOP_CALL_GREETING_LESS
 * @brief �ȈՃC�x���g�R�}���h�F�V���b�v�C�x���g�Ăяo��
 * @param shop_id
 * _SHOP_CALL�Ƃ̈Ⴂ�͘b���������́u��������Ⴂ�܂��v�̈��A����
 * ���Œ�V���b�v�ABP�V���b�v�ȂǊe�V���b�v�Ɉ��A�����o�[�W���������݂��܂��B
 */
//--------------------------------------------------------------
#define _SHOP_CALL_GREETING_LESS( ) _ASM_SHOP_CALL  SCR_SHOPID_NULL,FALSE

//--------------------------------------------------------------
/**
 * @def _FIX_SHOP_CALL
 * @brief �ȈՃC�x���g�R�}���h�F�Œ�V���b�v�C�x���g�Ăяo��
 * @param shop_id
 */
//--------------------------------------------------------------
#define _FIX_SHOP_CALL( shop_id ) _ASM_SHOP_CALL shop_id,TRUE
#define _FIX_SHOP_CALL_GREETING_LESS( shop_id ) _ASM_SHOP_CALL shop_id,FALSE

//--------------------------------------------------------------
/**
 * @def _BP_ITEM_SHOP_CALL
 * @brief �ȈՃC�x���g�R�}���h�FBP�V���b�v�C�x���g�Ăяo��
 */
//--------------------------------------------------------------
#define _BP_ITEM_SHOP_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BP_ITEM,TRUE
#define _BP_ITEM_SHOP_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BP_ITEM,FALSE

//--------------------------------------------------------------
/**
 * @def _BP_WAZA_SHOP_CALL
 * @brief �ȈՃC�x���g�R�}���h�FBP���U�V���b�v�C�x���g�Ăяo��
 */
//--------------------------------------------------------------
#define _BP_WAZA_SHOP_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BP_WAZA,TRUE
#define _BP_WAZA_SHOP_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BP_WAZA,FALSE

//--------------------------------------------------------------
/**
 * @def _BLACK_CITY_SHOP_CALL
 * @brief �ȈՃC�x���g�R�}���h�F�u���b�N�V�e�B�V���b�v�C�x���g�Ăяo��
 */
//--------------------------------------------------------------
#define _BLACK_CITY_SHOP0_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY0,TRUE
#define _BLACK_CITY_SHOP0_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY0,FALSE
#define _BLACK_CITY_SHOP1_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY1,TRUE
#define _BLACK_CITY_SHOP1_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY1,FALSE
#define _BLACK_CITY_SHOP2_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY2,TRUE
#define _BLACK_CITY_SHOP2_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY2,FALSE
#define _BLACK_CITY_SHOP3_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY3,TRUE
#define _BLACK_CITY_SHOP3_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY3,FALSE
#define _BLACK_CITY_SHOP4_CALL( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY4,TRUE
#define _BLACK_CITY_SHOP4_CALL_GREETING_LESS( ) _ASM_SHOP_CALL SCR_SHOPID_BLACK_CITY4,FALSE

//--------------------------------------------------------------
/**
 * @def _CALL_SHOP_PROC_BUY
 * @brief �V���b�v�C�x���g�Ăяo��
 * @param shop_id SCR_SHOPID_NULL ���w�肷��ƕϓ��V���b�v�A����ȊO�Ȃ�Œ�V���b�v
 *
 * shop_scr.ev��p�@�]�[���X�N���v�g���璼�ڌĂяo���Ȃ����ƁI
 */
//--------------------------------------------------------------
#define _CALL_SHOP_PROC_BUY( shop_id, ret_mode )  _ASM_CALL_SHOP_PROC_BUY shop_id,ret_mode

  .macro  _ASM_CALL_SHOP_PROC_BUY shop_id, ret_mode
  .short  EV_SEQ_CALL_SHOP_PROC_BUY
  .short  \shop_id
  .short  \ret_mode
  .endm

//======================================================================
// WiFi�N���u�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @brief    GSID�������������`�F�b�N����
 */
//--------------------------------------------------------------
#define _WIFI_CHECK_MY_GSID( ret_work ) _ASM_WIFI_CHECK_MY_GSID ret_work

  .macro  _ASM_WIFI_CHECK_MY_GSID ret_work
  .short  EV_SEQ_WIFI_CHECK_MY_GSID
  .short  \ret_work
  .endm

//--------------------------------------------------------------------
/**
 * @brief   �F�����蒠�ɓo�^����Ă���l����Ԃ� 
 */
//--------------------------------------------------------------------
#define _WIFI_GET_FRIEND_NUM( ret_work ) _ASM_WIFI_GET_FRIEND_NUM ret_work

  .macro  _ASM_WIFI_GET_FRIEND_NUM ret_work
  .short  EV_SEQ_WIFI_GET_FRIEND_NUM
  .short  \ret_work
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi�N���u�C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_CLUB_EVENT_CALL( ) _ASM_WIFI_CLUB_EVENT_CALL

  .macro  _ASM_WIFI_CLUB_EVENT_CALL
  .short  EV_SEQ_WIFI_CLUB_EVENT_CALL
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi GTS�C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_GTS_EVENT_CALL( ) _ASM_WIFI_GTS_EVENT_CALL

  .macro  _ASM_WIFI_GTS_EVENT_CALL
  .short  EV_SEQ_WIFI_GTS_EVENT_CALL
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi GTS�l�S�V�G�[�V�����C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_GTS_NEGO_EVENT_CALL( ) _ASM_WIFI_GTS_NEGO_EVENT_CALL

  .macro  _ASM_WIFI_GTS_NEGO_EVENT_CALL
  .short  EV_SEQ_WIFI_GTS_NEGO_EVENT_CALL
  .endm


//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi GeoNet�C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_GEO_NET_CALL( ) _ASM_WIFI_GEO_NET_CALL

  .macro  _ASM_WIFI_GEO_NET_CALL
  .short  EV_SEQ_WIFI_GEO_NET_CALL
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi �����_���}�b�`�C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_RANDOM_MATCH_EVENT_CALL( regulation, party ) \
  _ASM_WIFI_RANDOM_MATCH_EVENT_CALL regulation, party

  .macro  _ASM_WIFI_RANDOM_MATCH_EVENT_CALL regulation, party
  .short  EV_SEQ_WIFI_RANDOM_MATCH_EVENT_CALL
  .short  \regulation
  .short  \party
  .endm

//--------------------------------------------------------------------
/**
 * @brief   Wi-Fi �o�g�����R�[�_�[�C�x���g���Ăяo��
 */
//--------------------------------------------------------------------
#define _WIFI_BATTLE_RECORDER_EVENT_CALL( mode ) _ASM_WIFI_BATTLE_RECORDER_EVENT_CALL mode

  .macro  _ASM_WIFI_BATTLE_RECORDER_EVENT_CALL mode
  .short  EV_SEQ_WIFI_BATTLE_RECORDER_EVENT_CALL
  .short  \mode
  .endm

//--------------------------------------------------------------
/**
 * @def _BTL_UTIL_PARTY_SELECT_CALL
 * @brief �p�[�e�B�I���E�B���h�E�Ăяo��
 *
 * @param regulation  poke_tool/regulation_def.h�ɒ�`���ꂽ�R�[�h
 * @param ret_wk  �ǂ̃p�[�e�B��I�����������擾 btl_util_scr_local.h
 *                SCR_BTL_PARTY_SELECT_TEMOTI  �莝����I��
 *                SCR_BTL_PARTY_SELECT_BTLBOX  �o�g���{�b�N�X��I��
 *                SCR_BTL_PARTY_SELECT_NG      �ǂ�����K�����Ȃ�
 *                SCR_BTL_PARTY_SELECT_CANCEL  �L�����Z��
 */
//--------------------------------------------------------------
#define _BTL_UTIL_PARTY_SELECT_CALL( regulation, ret_wk )  \
    _ASM_BTL_UTIL_PARTY_SELECT_CALL  regulation, ret_wk

  .macro  _ASM_BTL_UTIL_PARTY_SELECT_CALL regulation, ret_wk 
  .short  EV_SEQ_BTL_UTIL_PARTY_SELECT
  .short  \regulation
  .short  \ret_wk
  .endm

//======================================================================
//  ��`�Z�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �Ȃ݂̂�J�n
 * @param none
 */
//--------------------------------------------------------------
#define _NAMINORI() _ASM_NAMINORI
  
  .macro _ASM_NAMINORI
  .short EV_SEQ_NAMINORI
  .endm

//--------------------------------------------------------------
/**
 * ��o��J�n
 * @param no �Z���g���莝���|�P�����ʒu�ԍ�
 */
//--------------------------------------------------------------
#define _TAKINOBORI( no ) _ASM_TAKINOBORI no
  
  .macro _ASM_TAKINOBORI no
  .short EV_SEQ_TAKINOBORI
  .short \no
  .endm

//--------------------------------------------------------------
/**
 * @def _IAIGIRI_EFFECT
 * @brief �������؂�G�t�F�N�g�\��
 */
//--------------------------------------------------------------
#define _IAIGIRI_EFFECT() \
    _ASM_IAIGIRI_EFFECT
  
  .macro _ASM_IAIGIRI_EFFECT
  .short EV_SEQ_IAIGIRI_EFFECT
  .endm

//--------------------------------------------------------------
/**
 * @brief �t�B�[���h�Z�F�_�C�r���O
 * �t�B�[���h�Z�X�N���v�g�ƊC���Ղł̂ݎg�p���Ă��܂��B
 * ���̉ӏ��ł͎g�p���Ȃ��ł��������B
 *
 *  param �㏸�Ɖ��~  SCR_EV_DIVING_MAPCHANGE_DOWN�@SCR_EV_DIVING_MAPCHANGE_UP
 */
//--------------------------------------------------------------
#define _DIVING( up_down ) _ASM_DIVING up_down
  .macro  _ASM_DIVING up_down
  .short  EV_SEQ_DIVING
  .short  \up_down
  .endm

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @def _BEACON_SET_REQ_ITEM_GET
 * @brief �r�[�R���Z�b�g���N�G�X�g�֘A
 * @param itemno  �擾�����A�C�e���̃i���o�[
 */
//--------------------------------------------------------------
#define _BEACON_SET_REQ_ITEM_GET( itemno ) \
    _ASM_BEACON_SET_REQ SCR_BEACON_SET_REQ_ITEM_GET, itemno
  
  .macro _ASM_BEACON_SET_REQ beacon_id, value
  .short EV_SEQ_BEACON_SET_REQUEST
  .short \beacon_id
  .short \value 
  .endm

//--------------------------------------------------------------
/**
 * @def _CHECK_GPOWER_FINISH
 * @brief G�p���[�t�B�j�b�V���`�F�b�N
 */
//--------------------------------------------------------------
#define _CHECK_GPOWER_FINISH( ret_gpower, ret_next ) \
    _ASM_CHECK_GPOWER_FINISH ret_gpower, ret_next
  
  .macro _ASM_CHECK_GPOWER_FINISH ret_gpower, ret_next 
  .short EV_SEQ_CHECK_GPOWER_FINISH
  .short \ret_gpower
  .short \ret_next
  .endm

//--------------------------------------------------------------
/**
 * @def _GPOWER_USE_EFFECT
 * @brief �������؂�G�t�F�N�g�\��
 */
//--------------------------------------------------------------
#define _GPOWER_USE_EFFECT() \
    _ASM_GPOWER_USE_EFFECT
  
  .macro _ASM_GPOWER_USE_EFFECT
  .short EV_SEQ_GPOWER_USE_EFFECT
  .endm


//======================================================================
//
//
//  �}�b�v�t�F�[�h�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�u���b�N�C��
 * @note �Ó]��ɃA�v���N���ȂǂŁA�}�b�v���j�󂳂��ꍇ�͂�����̃t�F�[�h���g���܂��B
 	* _BLACK_IN()�͉�ʂ��Â����邾���̎��ɂ����g���܂���B
 */
//--------------------------------------------------------------
#define _MAP_FADE_BLACK_IN() _ASM_MAP_FADE_BLACK_IN

  .macro  _ASM_MAP_FADE_BLACK_IN
  .short  EV_SEQ_MAP_FADE_BLACK_IN
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�G�߃C���i�R���e�B�j���[��p�j
 * @note
 * ����ȃR���e�B�j���[�������s�����߂̃X�N���v�g�R�}���h�B
 * �ʏ�͎g�p�֎~�ł��B
 * ���ݎ����̋G�߂�\�����Ă���t�F�[�h�C�����܂��B
 */
//--------------------------------------------------------------
#define _MAP_FADE_SEASON_IN_FORCE() _ASM_MAP_FADE_SEASON_IN_FORCE

  .macro  _ASM_MAP_FADE_SEASON_IN_FORCE
  .short  EV_SEQ_MAP_FADE_SEASON_IN_FORCE
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�u���b�N�C���i�R���e�B�j���[��p�j
 * @note
 * ����ȃR���e�B�j���[�������s�����߂̃X�N���v�g�R�}���h�B
 * �ʏ�͎g�p�֎~�ł��B
 */
//--------------------------------------------------------------
#define _MAP_FADE_BLACK_IN_FORCE() _ASM_MAP_FADE_BLACK_IN_FORCE

  .macro  _ASM_MAP_FADE_BLACK_IN_FORCE
  .short  EV_SEQ_MAP_FADE_BLACK_IN_FORCE
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�u���b�N�A�E�g
 * @note �Ó]��ɃA�v���N���ȂǂŁA�}�b�v���j�󂳂��ꍇ�͂�����̃t�F�[�h���g���܂��B
 * _BLACK_OUT()�͉�ʂ��Â����邾���̎��ɂ����g���܂���B
 */
//--------------------------------------------------------------
#define _MAP_FADE_BLACK_OUT() _ASM_MAP_FADE_BLACK_OUT

  .macro  _ASM_MAP_FADE_BLACK_OUT
  .short  EV_SEQ_MAP_FADE_BLACK_OUT
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�z���C�g�C��
 */
//--------------------------------------------------------------
#define _MAP_FADE_WHITE_IN() _ASM_MAP_FADE_WHITE_IN

  .macro  _ASM_MAP_FADE_WHITE_IN
  .short  EV_SEQ_MAP_FADE_WHITE_IN
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�z���C�g�A�E�g
 */
//--------------------------------------------------------------
#define _MAP_FADE_WHITE_OUT() _ASM_MAP_FADE_WHITE_OUT

  .macro  _ASM_MAP_FADE_WHITE_OUT
  .short  EV_SEQ_MAP_FADE_WHITE_OUT
  .endm

//--------------------------------------------------------------
/**
 *  _DISP_FADE_CHECK�@�}�b�v�J�ڗp�t�F�[�h�I���`�F�b�N
 *  @param none
 */
//--------------------------------------------------------------
#define _MAP_FADE_END_CHECK()  _ASM_MAP_FADE_END_CHECK

  .macro  _ASM_MAP_FADE_END_CHECK
  .short EV_SEQ_MAP_FADE_CHECK
  .endm


//======================================================================
//
//  �ȈՃR�}���h
//  ���X�N���v�g�R�}���h��g�ݍ��킹������
//
//======================================================================
.include  "easy_event_def.h"

//--------------------------------------------------------------
/**
 * �ȈՃ��b�Z�[�W�\�� BG�p	�I�I�I�I�I�I�g�p�֎~�I�I�I�I�I�I�I
 	* _EASY_BGWIN_MSG()�ɒu�������Ă����܂��B�����Ȃ莟��폜�\��
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _EASY_MSG( msg_id ) \
  _ASM_EASY_MSG msg_id

//--------------------------------------------------------------
/**
 * �ȈՐ����o�����b�Z�[�W�\�� �b���|��OBJ�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _EASY_BALLOONWIN_TALKOBJ_MSG( msg_id ) \
  _ASM_EASY_BALLOONWIN_TALKOBJ_MSG msg_id

//--------------------------------------------------------------
/**
 * �Ȉ�BG�E�B���h�E���b�Z�[�W�\��
 * @param msg_id �\�����郁�b�Z�[�WID
 * @param bg_type �\������^�C�v TYPE_INFO,TYPE_TOWN,TYPE_POST,TYPE_ROAD
                  TYPE_INFO �� �����Ȍf����
                  TYPE_TOWN �� �X�Ŕ�
                  TYPE_POST �� �{�݊Ŕ�
                  TYPE_ROAD �� ���H�Ŕ�
 */
//--------------------------------------------------------------
#define _EASY_BGWIN_MSG( msg_id, bg_type ) \
  _ASM_EASY_BGWIN_MSG msg_id, bg_type

//--------------------------------------------------------------
/**
 * �ȈՃC�x���g����C�x���g
 *
 * @param itemno        �擾����A�C�e���i���o�[
 * @param num           �擾����A�C�e���̐�
 * @param flag          �A�C�e���擾�������ǂ����̔���p�t���O
 * @param before_msg    ����O�̃��b�Z�[�W
 * @param finish_msg    �����̃��b�Z�[�W
 * @param after_msg     �����A�Ăјb�������Ƃ��̃��b�Z�[�W
 *
 */
//--------------------------------------------------------------
#define _EASY_TALK_ITEM_EVENT( itemno, num, flag, before_msg, finish_msg, after_msg) \
    _ASM_EASY_TALK_ITEM_EVENT itemno, num, flag, before_msg, finish_msg, after_msg

//--------------------------------------------------------------
/**
 * �C�x���g����C�x���g�u���v�҂�����
 *
 * @param itemno        �擾����A�C�e���i���o�[
 * @param num           �擾����A�C�e���̐�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_KEYWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_KEYWAIT itemno, num

//--------------------------------------------------------------
/**
 * �C�x���g����C�x���g�u���v�҂��Ȃ�
 *
 * @param itemno        �擾����A�C�e���i���o�[
 * @param num           �擾����A�C�e���̐�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_NOWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_NOWAIT itemno, num

//--------------------------------------------------------------
/**
 * �C�x���g����t�B�[���h�C�x���g�u���v�҂��Ȃ�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_FIELD(itemno, num)   \
    _ASM_ITEM_EVENT_FIELD itemno, num

//--------------------------------------------------------------
/**
 * �B���A�C�e������t�B�[���h�C�x���g�u���v�҂��Ȃ�
 *
 * @note
 * �������������B���A�C�e���X�N���v�g�ł̂ݎg�p���܂�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_HIDEITEM( itemno, num, flag_no ) \
  _ASM_ITEM_EVENT_HIDEITEM itemno, num, flag_no

//--------------------------------------------------------------
/**
 * @brief �C�x���g���萬���C�x���g�u���v�҂�����
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_SUCCESS_KEYWAIT( itemno, num ) \
    _ASM_ITEM_EVENT_SUCCESS_KEYWAIT itemno, num

//--------------------------------------------------------------
/**
 * @brief �C�x���g���萬���C�x���g�u���v�҂��Ȃ�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_SUCCESS_NOWAIT( itemno, num ) \
    _ASM_ITEM_EVENT_SUCCESS_NOWAIT itemno, num

//--------------------------------------------------------------
/**
 * �ȈՖ������b�Z�[�W�R�}���h
 *
 * @param monsno
 * @param msg_id
 */
//--------------------------------------------------------------
#define _EASY_TALK_POKEVOICE( monsno, msg_id ) \
    _ASM_EASY_TALK_POKEVOICE monsno, msg_id

//--------------------------------------------------------------
/**
 * @def _REPORT_EVENT_CALL
 * @brief �ȈՃC�x���g�R�}���h�F�Z�[�u�Ăяo��
 * @param ret_wk �Z�[�u���ʂ��󂯎�郏�[�N
 * @return �Z�[�u�����ꍇ     SCR_REPORT_OK
 *         �L�����Z�������ꍇ SCR_REPORT_CANCEL
 */
//--------------------------------------------------------------
#define _REPORT_EVENT_CALL( ret_wk )  \
    _ASM_REPORT_EVENT_CALL  ret_wk

//--------------------------------------------------------------
/**
 * @def _REPORT_EVENT_CALL_WIRELESS
 * @brief �ȈՃC�x���g�R�}���h�F�Z�[�u�Ăяo��( ���C�����X�Z�[�u���[�h�̃`�F�b�N�L )
 * @param ret_wk
 * @return �Z�[�u�����ꍇ             SCR_REPORT_OK
 *         �L�����Z�������ꍇ         SCR_REPORT_CANCEL
 *         �Z�[�u���Ȃ��ݒ肾�����ꍇ SCR_REPORT_OK
 */
//--------------------------------------------------------------
#define _REPORT_EVENT_CALL_WIRELESS( ret_wk )  \
    _ASM_REPORT_EVENT_CALL_WIRELESS  ret_wk

//--------------------------------------------------------------
/**
 * @def _FIELD_COMM_EXIT_EVENT_CALL
 * @brief �ȈՃC�x���g�R�}���h�F�t�B�[���h�ʐM�ؒf�C�x���g�Ăяo��
 * @param ret_wk  �ؒf�����̖߂�l���擾
 *                SCR_FIELD_COMM_EXIT_OK:�ؒf�I�� SCR_FIELD_COMM_EXIT_NG:�Ȃ�炩�̃G���[������
 */
//--------------------------------------------------------------
#define _FIELD_COMM_EXIT_EVENT_CALL( ret_wk )  \
    _ASM_FIELD_COMM_EXIT_EVENT_CALL  ret_wk

//--------------------------------------------------------------
/**
 * @def _BTL_UTIL_PARTY_REGULATION_NG_MSG_CALL
 * @brief �ȈՃC�x���g�R�}���h�F
 *        �ΐ�n��t�@�p�[�e�B�����M�����[�V�����ɔ�K�����������̈�A�̃��b�Z�[�W�\��
 *
 * @param regulation  poke_tool/regulation_def.h�ɒ�`���ꂽ�R�[�h
 */
//--------------------------------------------------------------
#define _BTL_UTIL_PARTY_REGULATION_NG_MSG_CALL( regulation )  \
    _ASM_BTL_UTIL_PARTY_REGULATION_NG_MSG_CALL  regulation


//======================================================================
//
//
//    �J��������֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �J��������J�n
 */
//--------------------------------------------------------------
#define _CAMERA_START() _ASM_CAMERA_START
  
  .macro _ASM_CAMERA_START
  .short EV_SEQ_CAMERA_START
  .endm

//--------------------------------------------------------------
/**
 * �J��������I��
 */
//--------------------------------------------------------------
#define _CAMERA_END() _ASM_CAMERA_END
  
  .macro _ASM_CAMERA_END
  .short EV_SEQ_CAMERA_END
  .endm  

//--------------------------------------------------------------
/**
 * �J�����p�[�W
 */
//--------------------------------------------------------------
#define _CAMERA_PURGE() _ASM_CAMERA_PURGE
  
  .macro _ASM_CAMERA_PURGE
  .short EV_SEQ_CAMERA_PURGE
  .endm

//--------------------------------------------------------------
/**
 * �J�����o�C���h
 */
//--------------------------------------------------------------
#define _CAMERA_BIND() _ASM_CAMERA_BIND
  
  .macro _ASM_CAMERA_BIND
  .short EV_SEQ_CAMERA_BIND
  .endm

//--------------------------------------------------------------
/**
 * �J�����ړ�
 * @param pitch     �s�b�`
 * @param yaw       ���[
 * @param dist      ����
 * @param x         �����_�w
 * @param y         �����_�x
 * @param z         �����_�y
 * @param frame     �ړ��t���[��
 */
//--------------------------------------------------------------
#define _CAMERA_MOVE(pitch, yaw, dist, x, y, z, frame) \
    _ASM_CAMERA_MOVE pitch, yaw, dist, x, y, z, frame
  
  .macro _ASM_CAMERA_MOVE pitch, yaw, dist, x, y, z, frame
  .short EV_SEQ_CAMERA_MOVE
  .short \pitch
  .short \yaw
  .long \dist
  .long \x
  .long \y
  .long \z
  .short \frame
  .endm

//--------------------------------------------------------------
/**
 * �J�����ړ��h�c�w��
 * @param id        �p�����[�^ID    
 * @param frame     �ړ��t���[��
 */
//--------------------------------------------------------------
#define _CAMERA_MOVE_BY_ID(id, frame) _ASM_CAMERA_MOVE_BY_ID id, frame
  
  .macro _ASM_CAMERA_MOVE_BY_ID id, frame
  .short EV_SEQ_CAMERA_MOVE_BY_ID
  .short \id
  .short \frame
  .endm  

//--------------------------------------------------------------
/**
 * �J�������A�ړ�
 * @param frame   �ړ��t���[��
 */
//--------------------------------------------------------------
#define _CAMERA_RECV_MOVE(frame) _ASM_CAMERA_RECV_MOVE frame
  
  .macro _ASM_CAMERA_RECV_MOVE frame
  .short EV_SEQ_CAMERA_RECV_MOVE
  .short \frame
  .endm

//--------------------------------------------------------------
/**
 * �f�t�H���g�J�����ւ̈ړ�
 * @param frame   �ړ��t���[��
 */
//--------------------------------------------------------------
#define _CAMERA_DEF_MOVE(frame) _ASM_CAMERA_DEF_MOVE frame
  
  .macro _ASM_CAMERA_DEF_MOVE frame
  .short EV_SEQ_CAMERA_DEF_MOVE
  .short \frame
  .endm  

//--------------------------------------------------------------
/**
 * �J�����ړ��҂�
 */
//--------------------------------------------------------------
#define _CAMERA_WAIT_MOVE() _ASM_CAMERA_WAIT_MOVE
  
  .macro _ASM_CAMERA_WAIT_MOVE
  .short EV_SEQ_CAMERA_WAIT_MOVE
  .endm

//--------------------------------------------------------------
/**
 * �J�����h��@�i�T�C���J�[�u�h��j
 * @param w   ���U�ꕝ
 * @param h   �c�h�ꕝ
 * @param sync    1�����ɂ�����V���N���@1�ȏ�
 * @param time    �U����
 * 
 * @note w,h�͓����ɒl���Z�b�g���ē��������Ƃ��ł��܂����A�ǂ�����T�C���J�[�u�ω����܂��̂ŁA
 * w�ɒl����ꂽ�ꍇ��h=0
 * h�ɒl����ꂽ�ꍇ��w=0
 *�ŃZ�b�g���邱�Ƃ������Ȃ�Ǝv���܂��B�i�܂�A�c�������h��͂Ȃ����낤�Ƃ������Ƃł��j
 */
//--------------------------------------------------------------
#define _SHAKE_CAMERA(w, h, sync, time) _ASM_SHAKE_CAMERA w, h, sync, time
  
  .macro _ASM_SHAKE_CAMERA w, h, sync, time
  .short EV_SEQ_SHAKE_CAMERA
  .short \w
  .short \h
  .short \sync
  .short \time
  .endm
  

//======================================================================
//
//
//    �N���֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �N���R�}���h�@�~�j���m���X�ݒu
 */
//--------------------------------------------------------------
#define _INTRUDE_MINIMONO_SETTING() _ASM_INTRUDE_MINIMONO_SETTING

  .macro  _ASM_INTRUDE_MINIMONO_SETTING
  .short  EV_SEQ_INTRUDE_MINIMONO_SETTING
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�~�b�V�����J�n
 */
//--------------------------------------------------------------
#define _INTRUDE_MISSION_START() _ASM_INTRUDE_MISSION_START

  .macro  _ASM_INTRUDE_MISSION_START
  .short  EV_SEQ_INTRUDE_MISSION_START
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�p���X���}�b�v�Z�b�e�B���O
 */
//--------------------------------------------------------------
#define _INTRUDE_CONNECT_MAP_SETTING() _ASM_INTRUDE_CONNECT_MAP_SETTING

  .macro  _ASM_INTRUDE_CONNECT_MAP_SETTING
  .short  EV_SEQ_INTRUDE_CONNECT_MAP_SETTING
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@���m���X��ʌĂяo��
 */
//--------------------------------------------------------------
#define _CALL_MONOLITH_PROC() _ASM_CALL_MONOLITH_PROC

  .macro  _ASM_CALL_MONOLITH_PROC
  .short  EV_SEQ_CALL_MONOLITH_PROC
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�~�b�V�����I����⃊�X�g�v�����N�G�X�g
 */
//--------------------------------------------------------------
#define _INTRUDE_MISSION_CHOICE_LIST_REQ() _ASM_INTRUDE_MISSION_CHOICE_LIST_REQ

  .macro  _ASM_INTRUDE_MISSION_CHOICE_LIST_REQ
  .short  EV_SEQ_INTRUDE_MISSION_CHOICE_LIST_REQ
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�V���{���|�P�����Z�b�e�B���O
 */
//--------------------------------------------------------------
#define _SYMBOL_POKE_SET() _ASM_SYMBOL_POKE_SET

  .macro  _ASM_SYMBOL_POKE_SET
  .short  EV_SEQ_SYMBOL_POKE_SET
  .endm

//--------------------------------------------------------------
/**
 *  �N�����̌��݂̕ϑ��󋵎擾
 *  @param ret_wk �^�C�v�i�[�惏�[�N
 */
//--------------------------------------------------------------
#define _GET_DISGUISE_CODE( ret_wk ) \
  _ASM_GET_DISGUISE_CODE  ret_wk

  .macro  _ASM_GET_DISGUISE_CODE  ret_wk
  .short  EV_SEQ_GET_DISGUISE_CODE
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�p���XIN���̕ϑ��C�x���g
 */
//--------------------------------------------------------------
#define _PALACE_IN_DISGUISE() _ASM_PALACE_IN_DISGUISE

  .macro  _ASM_PALACE_IN_DISGUISE
  .short  EV_SEQ_PALACE_IN_DISGUISE
  .endm

//--------------------------------------------------------------
/**
 *  �~�b�V�����ɃG���g���[���Ă��邩�擾
 *  @param ret_wk �^�C�v�i�[�惏�[�N
 * 
 * TRUE:�G���g���[���Ă���@FALSE:�G���g���[���Ă��Ȃ�
 */
//--------------------------------------------------------------
#define _GET_MISSION_ENTRY( ret_wk ) \
  _ASM_GET_MISSION_ENTRY  ret_wk

  .macro  _ASM_GET_MISSION_ENTRY  ret_wk
  .short  EV_SEQ_GET_MISSION_ENTRY
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �N���R�}���h�@�~�b�V�����G���g���[��A�J�n�܂ł̑҂��C�x���g
 */
//--------------------------------------------------------------
#define _MISSION_START_WAIT_EVENT() _ASM_MISSION_START_WAIT_EVENT

  .macro  _ASM_MISSION_START_WAIT_EVENT
  .short  EV_SEQ_MISSION_START_WAIT_EVENT
  .endm

//--------------------------------------------------------------
/**
 * �V���{���|�P�����o�g��
 * @param  �o�g������(TRUE:�ߊl�����@FALSE:�ߊl���Ȃ�����)
 */
//--------------------------------------------------------------
#define _SYMBOL_POKE_BATTLE( ret_wk ) _ASM_SYMBOL_POKE_BATTLE ret_wk

  .macro  _ASM_SYMBOL_POKE_BATTLE     ret_wk
  .short  EV_SEQ_SYMBOL_POKE_BATTLE
  .short  \ret_wk
  .endm

//======================================================================
//
//
//  �J�b�g�C���֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�J�b�g�C��
 * @param no      �J�b�g�C���i���o�[    fldci_id_def.h�Q��
 */
//--------------------------------------------------------------
#define _CALL_FLD3D_CUTIN(no) _ASM_CALL_FLD3D_CUTIN no

  .macro  _ASM_CALL_FLD3D_CUTIN no
  .short  EV_SEQ_CALL_FLD3D_CUTIN
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 * �|�P�����J�b�g�C��
 * @param pos         �莝���ʒu
 */
//--------------------------------------------------------------
#define _CALL_POKE_CUTIN(pos) _ASM_CALL_POKE_CUTIN pos

  .macro  _ASM_CALL_POKE_CUTIN pos
  .short  EV_SEQ_CALL_POKE_CUTIN
  .short  \pos
  .endm

//--------------------------------------------------------------
/**
 * NPC������ƂԃJ�b�g�C��
 * @param obj_id         �Ώ�OBJID
 */
//--------------------------------------------------------------
#define _CALL_NPCFLY_CUTIN(obj_id) _ASM_CALL_NPCFLY_CUTIN obj_id

  .macro  _ASM_CALL_NPCFLY_CUTIN obj_id
  .short  EV_SEQ_CALL_NPCFLY_CUTIN
  .short  \obj_id
  .endm  
  
//======================================================================
// �l�V���֘A
//======================================================================
//--------------------------------------------------------------
/**
 * @def _FOURKINGS_WALKEVENT
 * @brief �l�V���@�����A�j���[�V�����C�x���g�Ăяo��
 * @param fourking_id 1�`4  �]�[��ID�̐��l�@ZONE_ID_C09R[fourking_id]01
 */
//--------------------------------------------------------------
#define _FOURKINGS_WALKEVENT( fourking_id ) _ASM_FOURKINGS_WALKEVENT fourking_id

  .macro  _ASM_FOURKINGS_WALKEVENT fourking_id
  .short  EV_SEQ_FOURKINGS_WALKEVENT
  .short  \fourking_id
  .endm

//--------------------------------------------------------------
/**
 * @def _FOURKINGS_SET_TOP_CAMERA
 * @brief �l�V���@����̃J�����w��
 * @param fourking_id 1�`4  �]�[��ID�̐��l�@ZONE_ID_C09R[fourking_id]01
 */
//--------------------------------------------------------------
#define _FOURKINGS_SET_TOP_CAMERA( fourking_id ) _ASM_FOURKINGS_SET_TOP_CAMERA fourking_id

  .macro  _ASM_FOURKINGS_SET_TOP_CAMERA fourking_id
  .short  EV_SEQ_FOURKINGS_SET_TOP_CAMERA
  .short  \fourking_id
  .endm

//--------------------------------------------------------------
/**
 * @def _FOURKINGS_SET_SOUND_SYSTEM
 * @brief �l�V���@�����@���V�X�e���ݒ�
 * @param fourking_id 1�`4  �]�[��ID�̐��l�@ZONE_ID_C09R[fourking_id]01
 */
//--------------------------------------------------------------
#define _FOURKINGS_SET_SOUND_SYSTEM( fourking_id ) _ASM_FOURKINGS_SET_SOUND_SYSTEM fourking_id

  .macro  _ASM_FOURKINGS_SET_SOUND_SYSTEM fourking_id
  .short  EV_SEQ_FOURKINGS_SET_SOUND_SYSTEM
  .short  \fourking_id
  .endm



//======================================================================
//
//
//  �V���D�@�֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * �V���D���[�h�J�n
 */
//--------------------------------------------------------------
#define _PL_BOAT_START() _ASM_PL_BOAT_START

  .macro  _ASM_PL_BOAT_START
  .short  EV_SEQ_PL_BOAT_START
  .endm

//--------------------------------------------------------------
/**
 * �V���D���[�h�I��
 */
//--------------------------------------------------------------
#define _PL_BOAT_END() _ASM_PL_BOAT_END

  .macro  _ASM_PL_BOAT_END
  .short  EV_SEQ_PL_BOAT_END
  .endm

//--------------------------------------------------------------
/**
 * �V���D���g���[�i���擾
 * @param type�@    �����^�C�v  PL_TR_SEARCH_TYPE_�`�@pl_boat_def.h �Q��
 * @param ret_wk    �������g���[�i�[��
 */
//--------------------------------------------------------------
#define _PL_BOAT_GET_TR_NUM(type,ret_wk) _ASM_PL_BOAT_GET_TR_NUM type, ret_wk

  .macro  _ASM_PL_BOAT_GET_TR_NUM type, ret_wk
  .short  EV_SEQ_PL_BOAT_GET_TR_NUM
  .short  \type
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �V���D�����ԍX�V
 * @param add_time    ���Z���ԁi�b�j
 * @param whistle     ���Z���ʌ�A�D�J���Ȃ鎞�Ԃ̏ꍇ�A�D�J��炷���ǂ����@0�F�炳�Ȃ��@1�F�炷
 */
//--------------------------------------------------------------
#define _PL_BOAT_ADD_TIME(add_time, whistle) _ASM_PL_BOAT_ADD_TIME add_time, whistle

  .macro  _ASM_PL_BOAT_ADD_TIME add_time, whistle
  .short  EV_SEQ_PL_BOAT_ADD_TIME
  .short  \add_time
  .short  \whistle
  .endm

//--------------------------------------------------------------
/**
 * �V���D�Q�[���I��
 */
//--------------------------------------------------------------
#define _PL_BOAT_GAME_END() _ASM_PL_BOAT_GAME_END

  .macro  _ASM_PL_BOAT_GAME_END
  .short  EV_SEQ_PL_BOAT_GAME_END
  .endm  


//--------------------------------------------------------------
/**
 * �V���D���������擾
 * @param room_idx    �����C���f�b�N�X
 * @param info_kind   �擾���
 * @param param       ���擾�p�����[�^
 * @param ret_wk      �i�[�o�b�t�@
 */
//--------------------------------------------------------------
#define _PL_BOAT_GET_ROOM_INFO(room_idx,info_kind,param,ret_wk) \
    _ASM_PL_BOAT_GET_ROOM_INFO room_idx,info_kind,param,ret_wk

  .macro  _ASM_PL_BOAT_GET_ROOM_INFO room_idx,info_kind,param,ret_wk
  .short  EV_SEQ_PL_BOAT_GET_ROOM_INFO
  .short  \room_idx
  .short  \info_kind
  .short  \param
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �V���D���������Z�b�g
 * @param room_idx    �����C���f�b�N�X
 * @param info_kind   �擾���
 * @param param       ���擾�p�����[�^
 * @param set_val      �Z�b�g���
 */
//--------------------------------------------------------------
#define _PL_BOAT_SET_ROOM_INFO(room_idx,info_kind,param,set_val) \
    _ASM_PL_BOAT_SET_ROOM_INFO room_idx,info_kind,param,set_val

  .macro  _ASM_PL_BOAT_SET_ROOM_INFO room_idx,info_kind,param,set_val
  .short  EV_SEQ_PL_BOAT_SET_ROOM_INFO
  .short  \room_idx
  .short  \info_kind
  .short  \param
  .short  \set_val
  .endm

//--------------------------------------------------------------
/**
 * �V���D�b�f���R�[��
 * @param ret_wk
 */
//--------------------------------------------------------------
#define _PL_BOAT_CALL_DEMO(ret_wk) _ASM_PL_BOAT_CALL_DEMO ret_wk

  .macro  _ASM_PL_BOAT_CALL_DEMO ret_wk
  .short  EV_SEQ_PL_BOAT_CALL_DEMO
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * �}�b�v�t�F�[�h�a�f�������@�A�v������NOFADE�ŕʃ}�b�v�ɔ�ԂƂ��ȂǂɎg�p����
 */
//--------------------------------------------------------------
#define _MAP_FADE_INT_BG() _ASM_MAP_FADE_INIT_BG

  .macro  _ASM_MAP_FADE_INIT_BG
  .short  EV_SEQ_MAP_FADE_INIT_BG
  .endm
 
//======================================================================
//  3D�f��
//======================================================================
//--------------------------------------------------------------
/**
 * @def _CALL_3D_DEMO
 * @brief �f���Ăяo��
 * @param demo_no   �f��ID�w��i���o�[
 *
 * @note  �t�F�[�h�����͕ʓr�Ăяo���K�v������
 */
//--------------------------------------------------------------
#define _CALL_3D_DEMO( demo_no )  \
    _ASM_CALL_3D_DEMO demo_no

  .macro  _ASM_DEMO_SCENE demo_no
  .short  EV_SEQ_DEMO_SCENE
  .short  \demo_no
  .endm
  
//======================================================================
//  �e���r�g�����V�[�o�[�f��
//======================================================================
//--------------------------------------------------------------
/**
 * @def _CALL_TVT_DEMO
 * @brief �f���Ăяo��
 * @param demo_no   �f��ID�w��i���o�[
 */
//--------------------------------------------------------------
#define _CALL_TVT_DEMO( demo_no )  \
    _ASM_CALL_TVT_DEMO demo_no

  .macro  _ASM_TVT_DEMO demo_no
  .short  EV_SEQ_CALL_TVT_DEMO
  .short  \demo_no
  .endm

//======================================================================
//  �ړ��|�P����
//======================================================================

//--------------------------------------------------------------
/**
 * @def _ADD_MOVE_POKEMON
 * @brief �ړ��|�P�����ǉ�
 * @param move_poke    ���ʂ��󂯎�郏�[�N
 */
//--------------------------------------------------------------
#define _ADD_MOVE_POKEMON( move_poke )   \
    _ASM_ADD_MOVE_POKEMON move_poke

    .macro  _ASM_ADD_MOVE_POKEMON move_poke
    .short  EV_SEQ_ADD_MOVE_POKEMON
    .short  \move_poke
    .endm

//======================================================================
//  �����֘A
//======================================================================

//--------------------------------------------------------------
/**
 * @def _FIELD_POKE_TRADE
 * @brief �Q�[�������������s����
 * @param trade_no  ���炤�|�P�������w�� (FLD_TRADE_POKE_xxxx)
 * @param party_pos ������|�P�������w�� (�p�[�e�B���C���f�b�N�X0�`5)
 *
 * �@�����f���̍Đ�
 * �A�莝���|�P�����̏�������
 * �B�}�ӓo�^                 �����s���܂��B
 *
 * trade_no �ɓn���l��, resource/fld_trade/fld_trade_list.h �Œ�`.
 */
//--------------------------------------------------------------
#define _FIELD_POKE_TRADE( trade_no, party_pos ) \
    _ASM_FIELD_POKE_TRADE trade_no, party_pos

    .macro  _ASM_FIELD_POKE_TRADE trade_no, party_pos
    .short  EV_SEQ_FIELD_POKE_TRADE
    .short  \trade_no
    .short  \party_pos
    .endm

//--------------------------------------------------------------
/**
 * @def _FIELD_POKE_TRADE_CHECK
 * @brief �Q�[�����������\���ǂ������`�F�b�N����
 * @param ret_wk    ���ʂ��󂯎�郏�[�N
 * @param trade_no  ���炤�|�P�������w�� (FLD_TRADE_POKE_xxxx)
 * @param party_pos ������|�P�������w�� (�p�[�e�B���C���f�b�N�X0�`5)
 *
 * @retval FLD_TRADE_ENABLE         �����\
 *         FLD_TRADE_DISABLE_EGG    �����s�\(�^�}�S)
 *         FLD_TRADE_DISABLE_MONSNO �����s�\(�����X�^�[�Ⴂ)
 *         FLD_TRADE_DISABLE_SEX    �����s�\(���ʈႢ)
 *
 *  �߂�l�� prog/src/field/script_def.h �Œ�`.
 */
//--------------------------------------------------------------
#define _FIELD_POKE_TRADE_CHECK( ret_wk, trade_no, party_pos ) \
    _ASM_FIELD_POKE_TRADE_CHECK ret_wk, trade_no, party_pos

    .macro  _ASM_FIELD_POKE_TRADE_CHECK ret_wk, trade_no, party_pos
    .short  EV_SEQ_FIELD_POKE_TRADE_CHECK
    .short  \ret_wk
    .short  \trade_no
    .short  \party_pos
    .endm
  
//======================================================================
// �Z�v���o���֘A
//======================================================================
  
//--------------------------------------------------------------
/**
 * @def _CHECK_WAZA_REMAIND
 * @brief �v���o���Z�̗L�����`�F�b�N����
 * @param ret_wk ���ʂ��󂯎�郏�[�N
 * @param pos    ���肷��|�P�������莝���ʒu�Ŏw��
 * @return �w�肵���|�P�����Ɏv���o����Z������ꍇ TRUE,
 *         �����łȂ���� FALSE
 */
//--------------------------------------------------------------
#define _CHECK_WAZA_REMAIND( ret_wk, pos ) \
    _ASM_CHECK_WAZA_REMAIND ret_wk, pos

  .macro _ASM_CHECK_WAZA_REMAIND ret_wk, pos
  .short EV_SEQ_CHECK_WAZA_REMAIND
  .short \ret_wk
  .short \pos
  .endm 

//--------------------------------------------------------------
/**
 * @def _CALL_WAZA_REMAIND_PROC
 * @brief �Z�v���o���v���Z�X���Ăяo��
 * @param ret_wk ���ʂ̊i�[��
 * @param pos    �ΏۂƂȂ�|�P�������莝���ʒu�Ŏw��
 * @return �Z���v���o�����ꍇ SCR_WAZAOSHIE_RET_SET
 *         �L�����Z�������ꍇ SCR_WAZAOSHIE_RET_CANCEL
 */
//--------------------------------------------------------------
#define _CALL_WAZA_REMAIND_PROC( ret_wk, pos ) \
    _ASM_CALL_WAZA_REMAIND_PROC ret_wk, pos

  .macro _ASM_CALL_WAZA_REMAIND_PROC ret_wk, pos
  .short EV_SEQ_CALL_WAZA_REMAIND_PROC
  .short \ret_wk
  .short \pos
  .endm

//======================================================================
//  �ΐ�n��t���[�e�B���e�B
//======================================================================
//--------------------------------------------------------------
/**
 * @def _SET_REGULATION_OUT_POKE_NAME
 * @brief �f���Ăяo��
 * @param demo_no   �f��ID�w��i���o�[
 */
//--------------------------------------------------------------
#define _SET_REGULATION_OUT_POKE_NAME( regulation, ret_wk )  \
    _ASM_SET_REGULATION_OUT_POKE_NAME regulation, ret_wk

  .macro  _ASM_SET_REGULATION_OUT_POKE_NAME regulation, ret_wk
  .short  EV_SEQ_BTL_UTIL_SET_REGULATION_OUT_POKE_NAME
  .short  \regulation
  .short  \ret_wk
  .endm

//======================================================================
//  �|�P�������[�O �t�����g
//======================================================================
//--------------------------------------------------------------
/**
 * @def _LEAGUE_FRONT_LIFT_DOWN
 * @brief �f���Ăяo��
 * @param demo_no   �f��ID�w��i���o�[
 */
//--------------------------------------------------------------
#define _LEAGUE_FRONT_LIFT_DOWN() \
    _ASM_LEAGUE_FRONT_LIFT_DOWN

  .macro  _ASM_LEAGUE_FRONT_LIFT_DOWN
  .short  EV_SEQ_LEAGUE_FRONT_LIFT_DOWN
  .endm

//======================================================================
//  �o�g���T�u�E�F�C
//======================================================================
//--------------------------------------------------------------
/**
 * @def _BSUBWAY_WORK_CLEAR
 * @brief �o�g���T�u�E�F�C�@���[�N�N���A
 * @param none
 */
//--------------------------------------------------------------
#define _BSUBWAY_WORK_CLEAR() \
    _ASM_BSUBWAY_WORK_CLEAR

  .macro  _ASM_BSUBWAY_WORK_CLEAR
  .short  EV_SEQ_BSUBWAY_WORK_CLEAR
  .endm

//--------------------------------------------------------------
/**
 * @def _BSUBWAY_WORK_CREATE
 * @brief �o�g���T�u�E�F�C�@���[�N�쐬
 * @param init ��������� BSWAY_PLAY_NEW��
 * @param mode �o�g�����[�h BSWAY_MODE_SINGLE��
 */
//--------------------------------------------------------------
#define _BSUBWAY_WORK_CREATE( init, mode ) \
    _ASM_BSUBWAY_WORK_CREATE init, mode

  .macro  _ASM_BSUBWAY_WORK_CREATE init, mode
  .short  EV_SEQ_BSUBWAY_WORK_CREATE
  .short  \init
  .short  \mode
  .endm
 
//--------------------------------------------------------------
/**
 * @def _BSUBWAY_WORK_RELEASE
 * @brief �o�g���T�u�E�F�C�@���[�N�J��
 * @param none
 */
//--------------------------------------------------------------
#define _BSUBWAY_WORK_RELEASE() \
    _ASM_BSUBWAY_WORK_RELEASE

  .macro  _ASM_BSUBWAY_WORK_RELEASE
  .short  EV_SEQ_BSUBWAY_WORK_RELEASE
  .endm
 
//--------------------------------------------------------------
/**
 * @def _BSUBWAY_TOOL
 * @brief �o�g���T�u�E�F�C�@�R�}���h�c�[���Ăяo��
 * @param cmd �Ăяo���R�}���h�@BSWAY_TOOL_CHK_ENTRY_POKE_NUM��
 * @param param �R�}���h�ɓn���p�����[�^
 * @param ret_wk ���ʊi�[��
 */
//--------------------------------------------------------------
#define _BSUBWAY_TOOL( cmd, param0, param1, ret_wk ) \
    _ASM_BSUBWAY_TOOL cmd, param0, param1, ret_wk

  .macro  _ASM_BSUBWAY_TOOL cmd, param0, param1, ret_wk
  .short  EV_SEQ_BSUBWAY_TOOL
  .short \cmd
  .short \param0
  .short \param1
  .short \ret_wk
  .endm


//======================================================================
//
//  �ړ��|�P�����@�C�x���g
//
//======================================================================
//--------------------------------------------------------------
/**
 * @def _EV_MOVEPOKE_CREATE
 * @brief �ړ��|�P����  �Ǘ����[�N�@����
 * @param id      ����ID
 */
//--------------------------------------------------------------
#define _EV_MOVEPOKE_CREATE( id ) \
    _ASM_EV_MOVEPOKE_CREATE id

  .macro  _ASM_EV_MOVEPOKE_CREATE id
  .short  EV_SEQ_EV_MOVEPOKE_CREATE
  .short \id
  .endm


//--------------------------------------------------------------
/**
 * @def _EV_MOVEPOKE_DELETE
 * @brief �ړ��|�P����  �Ǘ����[�N�@�j��
 */
//--------------------------------------------------------------
#define _EV_MOVEPOKE_DELETE() \
    _ASM_EV_MOVEPOKE_DELETE

  .macro  _ASM_EV_MOVEPOKE_DELETE
  .short  EV_SEQ_EV_MOVEPOKE_DELETE
  .endm

//--------------------------------------------------------------
/**
 * @def _EV_MOVEPOKE_START_ANIME
 * @brief �ړ��|�P����  �Ǘ����[�N�@�A�j���[�V�����J�n
 * @param anime_index �A�j���[�V�����i���o�[
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_INSIDE (0)   ���C�J�~�@�J�U�J�~�@�o��
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_OUTSIDE (1)  ���C�J�~�@�J�U�J�~�@�ޏ�
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_INSIDE  (2)     �c�`�m�J�~�@�o��P
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_OUTSIDE (3)     �c�`�m�J�~  �o��Q
 */
//--------------------------------------------------------------
#define _EV_MOVEPOKE_START_ANIME( anime_index ) \
    _ASM_EV_MOVEPOKE_START_ANIME anime_index

  .macro  _ASM_EV_MOVEPOKE_START_ANIME anime_index
  .short  EV_SEQ_EV_MOVEPOKE_START_ANIME
  .short  \anime_index
  .endm

//--------------------------------------------------------------
/**
 * @def _EV_MOVEPOKE_CHECK_ANIME_END
 * @brief �ړ��|�P����  �Ǘ����[�N�@�A�j���[�V�����I���҂�
 */
//--------------------------------------------------------------
#define _EV_MOVEPOKE_CHECK_ANIME_END() \
    _ASM_EV_MOVEPOKE_CHECK_ANIME_END

  .macro  _ASM_EV_MOVEPOKE_CHECK_ANIME_END
  .short  EV_SEQ_EV_MOVEPOKE_CHECK_ANIME_END
  .endm

 
//======================================================================
//
//  �p���p�[�N�֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 * @def _PALPARK_CALL
 * @brief �p���p�[�N�Ăяo��
 * @param none
 */
//--------------------------------------------------------------
#define _PALPARK_CALL() \
    _ASM_PALPARK_CALL

  .macro  _ASM_PALPARK_CALL
  .short  EV_SEQ_CALL_PALPARK
  .endm

//--------------------------------------------------------------
/**
 *  _GET_PALPARK_VALUE 
 *  @brief �p���p�[�N�F���l�擾(�ėp
 *      ��{�I�ɉ��ɂ��郉�b�p�[���Ă�ł�������
 *  @param type ���(�O��Q�[���X�e�[�g�E�n�C�X�R�A
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_PALPARK_VALUE( type, retVal ) \
    _ASM_GET_PALPARK_VALUE type, retVal

  .macro  _ASM_GET_PALPARK_VALUE type, retVal
  .short EV_SEQ_GET_PALPARK_VALUE
  .byte \type
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_PALPARK_VALUE_FINISH_STATE 
 *  @brief �p���p�[�N�F�O��I�����(0:�ߊl���� 1:�ߊl����+�n�C�X�R�A 2:�ߊl���� 3:�G���[
 *         palpark_scr_local.h�Q��
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_PALPARK_VALUE_FINISH_STATE( retVal ) \
    _ASM_GET_PALPARK_VALUE_FINISH_STATE retVal

  .macro  _ASM_GET_PALPARK_VALUE_FINISH_STATE retVal
  .short EV_SEQ_GET_PALPARK_VALUE
  .byte 0
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _GET_PALPARK_VALUE 
 *  @brief �p���p�[�N�F�n�C�X�R�A
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _GET_PALPARK_VALUE_HIGHSCORE( retVal ) \
    _ASM_GET_PALPARK_VALUE_HIGHSCORE retVal

  .macro  _ASM_GET_PALPARK_VALUE_HIGHSCORE retVal
  .short EV_SEQ_GET_PALPARK_VALUE
  .byte 1
  .short \retVal
  .endm

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @def  _POSTMAN_REQUEST
 * @brief �s�v�c�ȑ��蕨�z�B���p�R�}���h
 * @param req     ���N�G�X�gID
 * @param ret_wk  �߂�l
 *
 * @note
 * �s�v�c�Ȃ�������̐�p�ł��B�ʏ�͎g�p���Ȃ��ł��������B
 */
//--------------------------------------------------------------
#define _POSTMAN_REQUEST( req, ret_wk ) \
    _ASM_POSTMAN_REQUEST req, ret_wk

    .macro  _ASM_POSTMAN_REQUEST req, ret_wk
    .short  EV_SEQ_POSTMAN_REQUEST
    .short  \req
    .short  \ret_wk
    .endm

//--------------------------------------------------------------
/**
 * @def  _PDW_POSTMAN_REQUEST
 * @brief PDW�z�B���p�R�}���h
 * @param req     ���N�G�X�gID
 * @param ret_wk  �߂�l
 *
 * @note
 * �p���X�ɔz�u�����PDW�z�B����p�ł��B�ʏ�͎g�p���Ȃ��ł��������B
 */
//--------------------------------------------------------------
#define _PDW_POSTMAN_REQUEST( req, ret_wk ) \
    _ASM_PDW_POSTMAN_REQUEST req, ret_wk

    .macro  _ASM_PDW_POSTMAN_REQUEST req, ret_wk
    .short  EV_SEQ_PDW_POSTMAN_REQUEST
    .short  \req
    .short  \ret_wk
    .endm

//======================================================================
//
//    WFBC�֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 *  _WFBC_TALKSTART 
 *  @brief WFBC:��b�J�n
 */
//--------------------------------------------------------------
#define _WFBC_TALKSTART() \
    _ASM_WFBC_TALKSTART

  .macro  _ASM_WFBC_TALKSTART
  .short EV_SEQ_WFBC_TALKSTART
  .endm


//--------------------------------------------------------------
/**
 *  _WFBC_TALKEND
 *  @brief WFBC:��b�I��
 */
//--------------------------------------------------------------
#define _WFBC_TALKEND() \
    _ASM_WFBC_TALKEND

  .macro  _ASM_WFBC_TALKEND
  .short EV_SEQ_WFBC_TALKEND
  .endm


//--------------------------------------------------------------
/**
 *  _WFBC_GET_DATA
 *  @brief WFBC:�e����̎擾
 *
 *  @param data_type  �擾���������̃^�C�v�@prog/src/field/scrcmd_wfbc_define.h
 *  @param ret_val    �߂�l  �f�[�^
 *
 *  data_type
      WFBC_GET_PARAM_BATTLE_TRAINER_ID (0)    // �o�g���g���[�i�[ID
      WFBC_GET_PARAM_IS_TAKES_ID (1)          // �X�ɂ��������邩
      WFBC_GET_PARAM_IS_RIREKI (2)          // �O�ɂ���BC�̊X�̗��������邩
      WFBC_GET_PARAM_PEOPLE_NUM (3)         // �l���̎擾 **zone_chnage�̃^�C�~���O�ŌĂ��OK**
      WFBC_GET_PARAM_BC_NPC_WIN_NUM (4)     // BC�@NPC������
      WFBC_GET_PARAM_BC_NPC_WIN_TARGET (5)  // BC�@NPC�����ڕW��
      WFBC_GET_PARAM_WF_IS_POKECATCH (6)  // WF�@�����C�x���g�@�|�P�����͂��邩�H
      WFBC_GET_PARAM_WF_ITEM  (7)         // WF �����C�x���g�@���J���A�C�e���̎擾
      WFBC_GET_PARAM_WF_POKE  (8)         // WF �����C�x���g�@�T���|�P�����i���o�[�̎擾
      WFBC_GET_PARAM_OBJ_ID  (9)         // �b�������OBJID�@�i0�`29�j�iWFBC�Ŏ����z�u�����30�l�̐l�����ʂ���ID�j
      WFBC_GET_PARAM_BC_CHECK_BATTLE (10)  // BC�@����ƃo�g���\���`�F�b�N
      WFBC_GET_PARAM_MMDL_ID  (11)         // �b�������MMDL OBJID (�t�B�[���h��̓��샂�f�������ʂ���ID)
 */
//--------------------------------------------------------------
#define _WFBC_GET_DATA( data_type, retVal ) \
    _ASM_WFBC_GET_DATA data_type, retVal

  .macro  _ASM_WFBC_GET_DATA data_type, retVal
  .short EV_SEQ_WFBC_GET_DATA
  .short \data_type
  .short \retVal
  .endm

//--------------------------------------------------------------
/**
 *  _WFBC_SET_WORDSET_RIREKI_PLAYER_NAME
 *  @brief WFBC:�����ɓ����Ă���l�̖��O�����[�h�Z�b�g�ɐݒ�
 *
 *  @param idx  �i�[���郏�[�h�Z�b�g�̃C���f�b�N�X
 */
//--------------------------------------------------------------
#define _WFBC_SET_WORDSET_RIREKI_PLAYER_NAME( idx ) \
    _ASM_WFBC_SET_WORDSET_RIREKI_PLAYER_NAME idx

  .macro  _ASM_WFBC_SET_WORDSET_RIREKI_PLAYER_NAME idx
  .short EV_SEQ_WFBC_SET_WORDSET_RIREKI_PLAYER_NAME
  .short \idx
  .endm

//--------------------------------------------------------------
/**
 *  _WFBC_CHECK_WF_TARGETPOKE
 *  @brief WFBC:WF �ڕW�|�P����������̂��H�`�F�b�N
 *  @param  ret_if1 �푰NO����v,������Ȃ��A�����n��WF �Ȃ� TRUE
 *  @param  ret_if2 �߂܂�����������  �Ȃ�@TRUE
 *  @param  ret_temoti_pos  �q�b�g�����|�P�����̎莝���ʒu
 */
//--------------------------------------------------------------
#define _WFBC_CHECK_WF_TARGETPOKE( ret_if1, ret_if2, ret_temoti_pos ) \
    _ASM_WFBC_CHECK_WF_TARGETPOKE ret_if1, ret_if2, ret_temoti_pos

  .macro  _ASM_WFBC_CHECK_WF_TARGETPOKE ret_if1, ret_if2, ret_temoti_pos
  .short EV_SEQ_WFBC_CHECK_WF_TARGETPOKE
  .short \ret_if1
  .short \ret_if2
  .short \ret_temoti_pos
  .endm

//--------------------------------------------------------------
/**
 *  _WFBC_ADD_BC_NPC_WIN_TARGET
 *  @brief WFBC:BC NPC�����������Z
 */
//--------------------------------------------------------------
#define _WFBC_ADD_BC_NPC_WIN_TARGET() \
    _ASM_WFBC_ADD_BC_NPC_WIN_TARGET

  .macro  _ASM_WFBC_ADD_BC_NPC_WIN_TARGET
  .short EV_SEQ_WFBC_ADD_BC_NPC_WIN_TARGET
  .endm

//--------------------------------------------------------------
/**
 *  _WFBC_PALACE_TALK
 *  @brief WFBC:��b�J�n
 */
//--------------------------------------------------------------
#define _WFBC_PALACE_TALK() \
    _ASM_WFBC_PALACE_TALK

  .macro  _ASM_WFBC_PALACE_TALK
  .short EV_SEQ_WFBC_PALACE_TALK
  .endm


//--------------------------------------------------------------
/**
 *  _WFBC_GET_AUTO_NPC_MESSAGE
 *  @brief WFBC:�����z�u�l���̃��b�Z�[�W���擾����
 *  
 *  @param  ret     �߂�l
 *  @param  msg_idx ���b�Z�[�W�̃C���f�b�N�X(1�`4)
 */
//--------------------------------------------------------------
#define _WFBC_GET_AUTO_NPC_MESSAGE( ret, msg_idx ) \
    _ASM_WFBC_GET_AUTO_NPC_MESSAGE ret, msg_idx

  .macro  _ASM_WFBC_GET_AUTO_NPC_MESSAGE ret, msg_idx
  .short EV_SEQ_WFBC_GET_AUTO_NPC_MESSAGE
  .short \ret
  .short \msg_idx
  .endm

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @def _FEELING_NAME
 * @brief �����f�f�C�x���g�F���O���Z�b�g
 * @param idx
 */
//--------------------------------------------------------------
#define _FEELING_NAME( idx )  \
    _ASM_FEELING_NAME idx

    .macro  _ASM_FEELING_NAME idx
    .short  EV_SEQ_SET_FEELING_NAME
    .short  \idx
    .endm

//--------------------------------------------------------------
/**
 * @def _GET_FEELING_COUNT
 * @brief �����f�f�C�x���g�F�����f�f�����l�����擾
 * @param ret_wk
 */
//--------------------------------------------------------------
#define _GET_FEELING_COUNT( ret_wk ) \
    _ASM_GET_FEELING_COUNT ret_wk

    .macro  _ASM_GET_FEELING_COUNT ret_wk
    .short  EV_SEQ_GET_FEELING_COUNT
    .short  \ret_wk
    .endm

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
    .macro  _ELEVATOR_LABEL   adrs
  .align  4
  \adrs:
    .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
    .macro  _ELEVATOR_DATA  msgid, zone_id, x, y, z
    .short  \msgid, \zone_id, \x, \y, \z
    .endm

//--------------------------------------------------------------
//--------------------------------------------------------------
    .macro  _ELEVATOR_DATA_END
    .short  0xffff
    .endm

//--------------------------------------------------------------
/**
 * @def _ELEVATOR_ENTRY_LIST
 * @brief �G���x�[�^�[�f�[�^�̓o�^
 * @param list_label  �G���x�[�^�[�f�[�^�w�胉�x��
 * @note  �G���x�[�^�[�X�N���v�g��p�R�}���h�B�ʏ�͎g��Ȃ��ł��������B
 */
//--------------------------------------------------------------
#define _ELEVATOR_ENTRY_LIST( list_label ) \
    _ASM_ELEVATOR_ENTRY_LIST list_label 

    .macro  _ASM_ELEVATOR_ENTRY_LIST list_label 
    .short  EV_SEQ_ELEVATOR_ENTRY_LIST
    .long  ((\list_label-.)-4)
    .endm

//--------------------------------------------------------------
/**
 * @def _ELEVATOR_MAKE_LIST
 * @brief �G���x�[�^�[�t���A�I�����X�g�ǉ�����
 * @note  �G���x�[�^�[�X�N���v�g��p�R�}���h�B�ʏ�͎g��Ȃ��ł��������B
 */
//--------------------------------------------------------------
#define _ELEVATOR_MAKE_LIST( ) \
    _ASM_ELEVATOR_MAKE_LIST 

    .macro  _ASM_ELEVATOR_MAKE_LIST list_label
    .short  EV_SEQ_ELEVATOR_MAKE_LIST
    .endm

//--------------------------------------------------------------
/**
 * @def _ELEVATOR_MAP_CHANGE
 * @brief �G���x�[�^�[�p�}�b�v�J�ڏ���
 * @param select_id   �I���ʒu
 * @note  �G���x�[�^�[�X�N���v�g��p�R�}���h�B�ʏ�͎g��Ȃ��ł��������B
 */
//--------------------------------------------------------------
#define _ELEVATOR_MAP_CHANGE( select_id ) \
    _ASM_ELEVATOR_MAP_CHANGE select_id

    .macro  _ASM_ELEVATOR_MAP_CHANGE select_id
    .short  EV_SEQ_ELEVATOR_MAP_CHANGE
    .short  \select_id
    .endm

//======================================================================
//
//  �ʃM�~�b�N�֘A
//
//======================================================================

//--------------------------------------------------------------
/**
 * @brief �W�F�b�g�o�b�W�`�F�b�N�Q�[�g�M�~�b�N������ 
 */
//--------------------------------------------------------------
#define _JET_BADGE_GATE_GIMMICK_INIT( ) \
    _ASM_JET_BADGE_GATE_GIMMICK_INIT

  .macro  _ASM_JET_BADGE_GATE_GIMMICK_INIT
  .short  EV_SEQ_JET_BADGE_GATE_GIMMICK_INIT
  .endm


//--------------------------------------------------------------
/**
 * @brief C03�������@��ʑJ�ڃJ��������
 * @param id        �p�����[�^ID    
 * @param frame     �ړ��t���[��
 */
//--------------------------------------------------------------
#define _C03CENTER_CAMERA_MOVE( id, frame ) \
    _ASM_C03CENTER_CAMERA_MOVE id, frame

  .macro  _ASM_C03CENTER_CAMERA_MOVE id, frame
  .short EV_SEQ_CAMERA_MOVE_BY_ID
  .short \id
  .short \frame
  .short EV_SEQ_C03CENTER_PLAYERWAY
  .short \frame
  .endm
  

//--------------------------------------------------------------
/**
 * @brief C03 �Q�W�\���J�n
 */
//--------------------------------------------------------------
#define _CROWD_PEOPLE_START() \
    _ASM_CROWD_PEOPLE_START

  .macro  _ASM_CROWD_PEOPLE_START
  .short EV_SEQ_CROWD_PEOPLE_START
  .endm

//--------------------------------------------------------------
/**
 * @brief D20�C��\��ON
 */
//--------------------------------------------------------------
#define _SEATEMPLE_START() \
    _ASM_SEATEMPLE_START

  .macro  _ASM_SEATEMPLE_START
  .short EV_SEQ_SEATEMPLE_START
  .endm

//--------------------------------------------------------------
/**
 * @brief �V�C�ύX
 * @param weather_id  WEATHER_NO_RAIKAMI�F���C�J�~  WEATHER_NO_KAZAKAMI�F�J�U�J�~ 
 * prog/include/field/weather_no.h�@�Q��
 */
//--------------------------------------------------------------
#define _WEATHER_CHANGE( weather_id ) \
    _ASM_WEATHER_CHANGE weather_id

  .macro  _ASM_WEATHER_CHANGE weather_id
  .short EV_SEQ_WEATHER_CHANGE
  .short \weather_id
  .endm


//--------------------------------------------------------------
/**
 * @brief �|�P����ID�����@������ID�ƈقȂ�ID�̎�ސ����擾�i�ő�50�j
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _SEARCH_POKEID_NUM( ret ) _ASM_SEARCH_POKEID_NUM ret

  .macro  _ASM_SEARCH_POKEID_NUM ret
  .short EV_SEQ_SEARCH_POKEID_NUM
  .short \ret
  .endm

//======================================================================
//
//    �o�X�g�A�b�v�V���b�g�֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ��A�b�v�J�n
 * @note      �����́Asrc/field/fld_faceup_def.h���Q�Ƃ̂���
 * @param back_idx    �w�i�w��          FLD_FACEUP_BG�`���w��
 * @param char_idx    �L�����N�^�[�w��  FLD_FACEUP_CHAR�`���w��
 */
//--------------------------------------------------------------
#define _START_FACEUP( back_idx, char_idx ) _ASM_START_FACEUP back_idx, char_idx

  .macro  _ASM_START_FACEUP back_idx, char_idx
  .short EV_SEQ_FACEUP_SATRT
  .short \back_idx
  .short \char_idx
  .endm

//--------------------------------------------------------------
/**
 * @brief ��A�b�v�I��
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _END_FACEUP() _ASM_END_FACEUP

  .macro  _ASM_END_FACEUP
  .short EV_SEQ_FACEUP_END
  .endm
  
//--------------------------------------------------------------
/**
 * @brief ��A�b�v ��j�ύX
 * @note      �����́Asrc/field/fld_faceup_def.h���Q�Ƃ̂���
 * @param char_idx    �L�����N�^�[�w��  FLD_FACEUP_CHAR�`���w��   
 */
//--------------------------------------------------------------
#define _CHANGE_FACEUP( char_idx ) _ASM_CHANGE_FACEUP char_idx

  .macro  _ASM_CHANGE_FACEUP char_idx
  .short EV_SEQ_FACEUP_CHG
  .short \char_idx
  .endm


//======================================================================
//
//    ���A�֘A
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief ���A�֘A ���������������擾
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_GET_COUNTRY_NUM( ret ) _ASM_UN_GET_COUNTRY_NUM ret

  .macro  _ASM_UN_GET_COUNTRY_NUM ret
  .short EV_SEQ_UN_GET_COUNTRY_NUM
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �L���f�[�^��
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_GET_VALID_DATA_NUM( ret ) _ASM_UN_GET_VALID_DATA_NUM ret

  .macro  _ASM_UN_GET_VALID_DATA_NUM ret
  .short EV_SEQ_UN_GET_VALID_DATA_NUM
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A ��b�t���O�Z�b�g
 * @param obj_idx   OBJ�C���f�b�N�X0�`4
 */
//--------------------------------------------------------------
#define _UN_SET_TALK_FLG( obj_idx ) _ASM_UN_SET_TALK_FLG obj_idx

  .macro _ASM_UN_SET_TALK_FLG obj_idx
  .short EV_SEQ_UN_SET_TALK_FLG
  .short \obj_idx
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A ��b�t���O�`�F�b�N
 * @param obj_idx   OBJ�C���f�b�N�X0�`4
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_CHK_TALK_FLG( obj_idx, ret ) _ASM_UN_CHK_TALK_FLG obj_idx, ret

  .macro _ASM_UN_CHK_TALK_FLG obj_idx, ret
  .short EV_SEQ_UN_CHK_TALK_FLG
  .short \obj_idx
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �����̐��i���Z�b�g
 * @param nature_idx ���i�R�[�h
 * @note prog/include/savedata/nature_def.h�@�Q��
 */
//--------------------------------------------------------------
#define _UN_SET_MY_NATURE( nature_idx ) _ASM_UN_SET_MY_NATURE nature_idx

  .macro _ASM_UN_SET_MY_NATURE nature_idx
  .short EV_SEQ_UN_SET_MY_NATURE
  .short \nature_idx
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �����̎���Z�b�g
 * @param favorite_idx    ��R�[�h
 * @note  resource\research_radar\data\hobby_id.h�@�Q��
 */
//--------------------------------------------------------------
#define _UN_SET_MY_FAVORITE( favorite_idx ) _ASM_UN_SET_MY_FAVORITE favorite_idx

  .macro _ASM_UN_SET_MY_FAVORITE favorite_idx
  .short EV_SEQ_UN_SET_MY_FAVORITE
  .short \favorite_idx
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �����̎���擾
 * @param ret       ����
 */
//--------------------------------------------------------------
#define _UN_GET_MY_FAVORITE( ret ) _ASM_UN_GET_MY_FAVORITE ret

  .macro _ASM_UN_GET_MY_FAVORITE ret
  .short EV_SEQ_UN_GET_MY_FAVORITE
  .short \ret
  .endm
  
//--------------------------------------------------------------
/**
 * @brief ���A�֘A �����ɍs���O�̏��Z�b�g
 * @param country_code  ���R�[�h
 * @param floor         �t���A
 */
//--------------------------------------------------------------
#define _UN_SET_COUNTRY_INFO( country_code, floor ) _ASM_UN_SET_COUNTRY_INFO country_code, floor

  .macro _ASM_UN_SET_COUNTRY_INFO country_code, floor
  .short EV_SEQ_UN_SET_COUNTRY_INFO
  .short \country_code
  .short \floor
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �������@OBJ�R�[�h�擾
 * @param idx       �����̒���OBJ�C���f�b�N�X0�`4
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_GET_ROOM_OBJ_CODE( idx, ret ) _ASM_UN_GET_ROOM_OBJ_CODE idx, ret

  .macro _ASM_UN_GET_ROOM_OBJ_CODE idx, ret
  .short EV_SEQ_UN_GET_ROOM_OBJ_CODE
  .short \idx
  .short \ret
  .endm 

//--------------------------------------------------------------
/**
 * @brief ���A�֘A ���������擾
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_GET_ROOM_INFO( infotype, ret ) _ASM_UN_GET_ROOM_INFO infotype, ret

  .macro _ASM_UN_GET_ROOM_INFO infotype, ret
  .short EV_SEQ_UN_GET_ROOM_INFO
  .short \infotype
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �������@OBJ���b�Z�[�W�擾
 * @param idx       �����̒���OBJ�C���f�b�N�X0�`4
 * @param idx       �擾�������̂͏��񃁃b�Z�[�W���H  TRUE�ŏ��񃁃b�Z�[�W
 * @param ret       ��������
 */
//--------------------------------------------------------------
#define _UN_GET_ROOM_OBJ_MSG( idx, first, ret ) _ASM_UN_GET_ROOM_OBJ_MSG idx, first, ret

  .macro _ASM_UN_GET_ROOM_OBJ_MSG idx, first, ret
  .short EV_SEQ_UN_GET_ROOM_OBJ_MSG
  .short \idx
  .short \first
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ���A�֘A �t���A�I���A�v���R�[��
 * @param in_floor    ���݃t���A    �G���g�����X�̂Ƃ��́@1�ȉ����w��
 * @param out_floor   �ړ���t���A�i�[���[�N
 * @param country�@   �ړ��捑�R�[�h���[�N
 * @param ret         ���ʁ@TRUE �ňړ�
 */
//--------------------------------------------------------------
#define _UN_CALL_FLOOR_SEL_APP( in_floor, out_floor, country, ret ) \
    _ASM_UN_CALL_FLOOR_SEL_APP in_floor, out_floor, country, ret

  .macro _ASM_UN_CALL_FLOOR_SEL_APP in_floor, out_floor, country, ret
  .short EV_SEQ_UN_CALL_FLOOR_SEL_APP
  .short \in_floor
  .short \out_floor
  .short \country
  .short \ret
  .endm
  
//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @def _SXY_MOVE_BG_EVENT
 * @brief BG�C�x���g�̈ʒu��ύX����
 * @param id  BG�C�x���g���w�肷�邽�߂�ID
 * @param gx  X�ʒu�i�O���b�h�P�ʁj
 * @param gy  Y�ʒu�i�O���b�h�P�ʁj
 * @param gz  Z�ʒu�i�O���b�h�P�ʁj
 */
//--------------------------------------------------------------
#define _SXY_MOVE_BG_EVENT( bg_id, gx, gy, gz ) \
    _ASM_SXY_MOVE_BG_EVENT bg_id, gx, gy, gz

    .macro  _ASM_SXY_MOVE_BG_EVENT bg_id, gx, gy, gz
    .short  EV_SEQ_MOVE_BG_EVENT_POS
    .short  \bg_id
    .short  \gx
    .short  \gy
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _SXY_MOVE_EXIT_EVENT
 * @brief �o�����C�x���g�̈ʒu��ύX����
 * @param id  �o�����C�x���g���w�肷�邽�߂�ID
 * @param gx  X�ʒu�i�O���b�h�P�ʁj
 * @param gy  Y�ʒu�i�O���b�h�P�ʁj
 * @param gz  Z�ʒu�i�O���b�h�P�ʁj
 */
//--------------------------------------------------------------
#define _SXY_MOVE_EXIT_EVENT( exit_id, gx, gy, gz ) \
    _ASM_SXY_MOVE_EXIT_EVENT exit_id, gx, gy, gz

    .macro  _ASM_SXY_MOVE_EXIT_EVENT exit_id, gx, gy, gz
    .short  EV_SEQ_MOVE_EXIT_EVENT_POS
    .short  \exit_id
    .short  \gx
    .short  \gy
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _SXY_MOVE_NPC_POS
 * @brief NPC�̏����ʒu��ύX����
 * @param npc_id  NPC���w�肷�邽�߂�ID
 * @param dir     �����̎w��
 * @param gx      X�ʒu�i�O���b�h�P�ʁj
 * @param gy      Y�ʒu�i�O���b�h�P�ʁj
 * @param gz      Z�ʒu�i�O���b�h�P�ʁj
 *
 * _ZONE_CHANGE_LABEL�̃^�C�~���O�ȊO�Ŏg�p���Ă��A
 * ���̃R�}���h�ŕύX�����f�[�^���Q�Ƃ���Ȃ����߈Ӗ����Ȃ��B
 */
//--------------------------------------------------------------
#define _SXY_MOVE_NPC_POS( npc_id, dir, gx, gy, gz ) \
    _ASM_SXY_MOVE_NPC_POS npc_id, dir, gx, gy, gz

    .macro  _ASM_SXY_MOVE_NPC_POS npc_id, dir, gx, gy, gz
    .short  EV_SEQ_MOVE_NPC_DATA_POS
    .short  \npc_id
    .short  \dir
    .short  \gx
    .short  \gy
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @brief �ȈՉ�b�A�v���R�[��
 * @param mode
 * @param word1
 * @param word2
 * @param ret       ���ʁ@�ҏW����߂��ꍇFALSE �ҏW�����ꍇTRUE
 */
//--------------------------------------------------------------
#define _CALL_EASY_TALK_APP( mode, word1, word2, ret ) \
      _ASM_CALL_EASY_TALK_APP mode, word1, word2, ret

  .macro _ASM_CALL_EASY_TALK_APP mode, word1, word2, ret
  .short EV_SEQ_CALL_EASY_TALK_APP
  .short \mode
  .short \word1
  .short \word2
  .short \ret
  .endm

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�J�n
 */
//--------------------------------------------------------------
#define _TH_START() _ASM_TH_START
  
  .macro _ASM_TH_START
  .short EV_SEQ_TH_START
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�I��
 */
//--------------------------------------------------------------
#define _TH_END() _ASM_TH_END
  
  .macro _ASM_TH_END
  .short EV_SEQ_TH_END
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�v���C���[�h�Z�b�g�i�V���O���n�q�_�u���j
 * @param   mode
 */
//--------------------------------------------------------------
#define _TH_SET_PLAY_MODE( mode ) _ASM_TH_SET_PLAY_MODE mode
  
  .macro _ASM_TH_SET_PLAY_MODE mode
  .short EV_SEQ_TH_SET_PLAY_MODE
  .short \mode
  .endm
  
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�|�P�����I��
 * @param   reg_type      REG_SUBWAY_SINGLE or REG_SUBWAY_DOUBLE
 * @param   party_type    SCR_BTL_PARTY_SELECT_TEMOTI or SCR_BTL_PARTY_SELECT_BTLBOX
 * @param   ret     �I������  FALSE�F�L�����Z��
 */
//--------------------------------------------------------------
#define _TH_SEL_POKE( reg_type, party_type, ret ) _ASM_TH_SEL_POKE reg_type, party_type, ret
  
  .macro _ASM_TH_SEL_POKE reg_type, party_type, ret
  .short EV_SEQ_TH_SEL_POKE
  .short \reg_type
  .short \party_type
  .short \ret
  .endm
 
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�ΐ푊��Z�b�g
 * @param   round   �ΐ�񐔁@0�`4
 * poaram   obj_id  �Z�b�g�����g���[�i�[�̂n�a�i�h�c���i�[����o�b�t�@
 */
//--------------------------------------------------------------
#define _TH_SET_TR( round, obj_id ) _ASM_TH_SET_TR round, obj_id
  
  .macro _ASM_TH_SET_TR round, obj_id
  .short EV_SEQ_TH_SET_TR
  .short \round
  .short \obj_id
  .endm
  
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�퓬�O���b�Z�[�W�\��
 */
//--------------------------------------------------------------
#define _TH_DISP_BEFORE_MSG() _ASM_TH_DISP_BEFORE_MSG
  
  .macro _ASM_TH_DISP_BEFORE_MSG
  .short EV_SEQ_TH_DISP_BEFORE_MSG
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�퓬�Ăяo��
 */
//--------------------------------------------------------------
#define _TH_CALL_BATTLE() _ASM_TH_CALL_BATTLE
  
  .macro _ASM_TH_CALL_BATTLE
  .short EV_SEQ_TH_CALL_BATTLE
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�_�E�����[�h�f�[�^�ŗV�Ԃ��H
 * @param   flg       TRUE: �_�E�����[�h�f�[�^ FALSE:�q�n�l�f�[�^
 */
//--------------------------------------------------------------
#define _TH_SET_DL_FLG( flg ) _ASM_TH_SET_Dl_FLG flg
  
  .macro _ASM_TH_SET_DL_FLG flg
  .short EV_SEQ_TH_SET_DL_FLG
  .short \flg
  .endm
  
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�����N�Z�b�g
 * @param  rank  �Z�b�g���郉���N
 */
//--------------------------------------------------------------
#define _TH_SET_RANK( rank ) _ASM_TH_SET_RANK rank
  
  .macro _ASM_TH_SET_RANK rank
  .short EV_SEQ_TH_SET_RANK
  .short \rank
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�����N�擾
 * @param  rank �����N�i�[�o�b�t�@
 */
//--------------------------------------------------------------
#define _TH_GET_RANK( rank ) _ASM_TH_GET_RANK rank
  
  .macro _ASM_TH_GET_RANK rank
  .short EV_SEQ_TH_GET_RANK
  .short \rank
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�r�[�R���T�[�`
 * @param  ret    �T�[�`����
 */
//--------------------------------------------------------------
#define _TH_SEARCH_BEACON( ret ) _ASM_TH_SEARCH_BEACON ret
  
  .macro _ASM_TH_SEARCH_BEACON ret
  .short EV_SEQ_TH_SEARCH_BEACON
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�̓_
 * @param rank    �̓_���ʂ̃����N�i�[�o�b�t�@
 * @param point   ���_�i�[�o�b�t�@
 */
//--------------------------------------------------------------
#define _TH_CALC_BTL_RESULT( rank, point ) _ASM_TH_CALC_BTL_RESULT rank, point
  
  .macro _ASM_TH_CALC_BTL_RESULT rank, point
  .short EV_SEQ_TH_CALC_BTL_RESULT
  .short \rank
  .short \point
  .endm
  
//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�_�E�����[�h�f�[�^�^�C�v�擾
 * @param type     �^�C�v�i�[���[�N
 * @note          �߂�l��
 * TH_DL_DATA_TYPE_NONE �F�f�[�^�Ȃ�
 * TH_DL_DATA_TYPE_SINGLE �F�V���O��
 * TH_DL_DATA_TYPE_DOUBLE �F�_�u��
 */
//--------------------------------------------------------------
#define _TH_GET_DL_DATA_TYPE( type ) _ASM_TH_GET_DL_DATA_TYPE type
  
  .macro _ASM_TH_GET_DL_DATA_TYPE type
  .short EV_SEQ_TH_GET_DL_DATA_TYPE
  .short \type
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�_�E�����[�h�f�[�^���N���A
 * @param none
 */
//--------------------------------------------------------------
#define _TH_CLEAR_DL_DATA() _ASM_TH_CLEAR_DL_DATA
  
  .macro _ASM_TH_CLEAR_DL_DATA
  .short EV_SEQ_TH_CLEAR_DL_DATA
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�����L���O�m�F�A�v���R�[��
 * @param dl
 * @param none
 */
//--------------------------------------------------------------
#define _TH_CALL_RANK_APP( dl ) _ASM_TH_CALL_RANK_APP dl
  
  .macro _ASM_TH_CALL_RANK_APP dl
  .short EV_SEQ_TH_CALL_RANK_APP
  .short \dl
  .endm

//--------------------------------------------------------------
/**
 * @brief �g���C�A���n�E�X�@�����L���O�f�[�^��
 * @param state   �X�e�[�g�i�[�o�b�t�@  trial_house_scr_def.h�Q��
 * @param none
 */
//--------------------------------------------------------------
#define _TH_GET_RDAT_STATE( state ) _ASM_TH_GET_RDAT_STATE state
  
  .macro _ASM_TH_GET_RDAT_STATE state
  .short EV_SEQ_TH_GET_RDAT_STATE
  .short \state
  .endm
  
//--------------------------------------------------------------
/**
 *  _PDW_COMMON_TOOLS PDW�ėp�c�[��
 *  @param val1  �擾�p�ԍ�
 *  @param val2  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _PDW_COMMON_TOOLS( type, val1, val2, ret_val ) \
    _ASM_PDW_COMMON_TOOLS type, val1, val2, ret_val 
  
  .macro _ASM_PDW_COMMON_TOOLS type, val1, val2, ret_val 
  .short EV_SEQ_PDW_COMMON_TOOLS
  .short \type
  .short \val1
  .short \val2
  .short \ret_val
  .endm


//--------------------------------------------------------------
/**
 *  _PDW_FURNITURE_TOOLS PDW�Ƌ�j���[�F�ėp�c�[��
 *  @param val1  �擾�p�ԍ�
 *  @param val2  �擾�p�ԍ�
 *  @param ret_val  �߂�l
 */
//--------------------------------------------------------------
#define _PDW_FURNITURE_TOOLS( type, val1, val2, ret_val ) \
    _ASM_PDW_FURNITURE_TOOLS type, val1, val2, ret_val 
  
  .macro _ASM_PDW_FURNITURE_TOOLS type, val1, val2, ret_val 
  .short EV_SEQ_PDW_FURNITURE_TOOLS
  .short \type
  .short \val1
  .short \val2
  .short \ret_val
  .endm


//--------------------------------------------------------------
/**
 *  _PDW_SET_FURNITURE_WORD PDW�Ƌ�j���[�F�Ƌ�Z�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param val �Z�b�g����Ƌ�ԍ�
 */
//--------------------------------------------------------------
#define _PDW_SET_FURNITURE_WORD( idx, val ) \
    _ASM_PDW_SET_FURNITURE_WORD idx, val

  .macro _ASM_PDW_SET_FURNITURE_WORD idx, val
  .short EV_SEQ_PDW_SET_FURNITURE_WORD
  .short \idx
  .short \val
  .endm

//--------------------------------------------------------------
/**
 *  _MACHINE_PASSWORD     �p�X���[�h�������킹
 * @param no    ���ԍ��@1�@or 2
 * @param word1   �P�ڃ��[�h�R�[�h
 * @param word2   �Q�ڃ��[�h�R�[�h
 * @param ret     ���ʁ@TRUE�Ő���
 */
//--------------------------------------------------------------
#define _MACHINE_PASSWORD( no, word1, word2, ret ) \
    _ASM_MACHINE_PASSWORD no, word1, word2, ret

  .macro _ASM_MACHINE_PASSWORD no, word1, word2, ret
  .short EV_SEQ_MACHINE_PASSWORD
  .short \no
  .short \word1
  .short \word2
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 *  _GET_QUIZ     �N�C�Y�擾
 * @param quiz  �N�C�Y
 * @param hint  �q���g
 * @param ans   �����ȈՉ�b�P��R�[�h
 */
//--------------------------------------------------------------
#define _GET_QUIZ( quiz, hint, ans ) \
    _ASM_GET_QUIZ quiz, hint, ans

  .macro _ASM_GET_QUIZ quiz, hint, ans
  .short EV_SEQ_GET_QUIZ
  .short \quiz
  .short \hint
  .short \ans
  .endm  

//--------------------------------------------------------------
/**
 *  _ITEMJUDGE     �Ӓ�m
 *  @param  item_no   �Ӓ肷��A�C�e��
 *  @param  obj_type  �Ӓ肷��l  
 *                    SCR_ITEM_JUDGE_OBJTYPE_GOURMET      (0)   // �O����
 *                    SCR_ITEM_JUDGE_OBJTYPE_STONE_MANIA  (1)   // �΃}�j�A
 *                    SCR_ITEM_JUDGE_OBJTYPE_RICH         (2)   // ��x��
 *  @param  ret_money ���z�i�[���[�N      
 *
 *  ret_money��0�Ȃ狻���Ȃ�
 */
//--------------------------------------------------------------
#define _ITEMJUDGE( item_no, obj_type, ret_money ) \
    _ASM_ITEMJUDGE item_no, obj_type, ret_money

  .macro _ASM_ITEMJUDGE item_no, obj_type, ret_money
  .short EV_SEQ_ITEMJUDGE_CHECK
  .short \item_no
  .short \obj_type
  .short \ret_money
  .endm  




//======================================================================
// ����Ⴂ�������֘A
//======================================================================

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�������Ƃ��Ă̒������J�n����
 *
 * @param req [in] �J�n���钲���˗�ID ( RESEARCH_REQ_ID_xxxx )
 * @param q1  [in] �������鎿��ID ( QUESTION_ID_xxxx )
 * @param q2  [in] �������鎿��ID ( QUESTION_ID_xxxx )
 * @param q3  [in] �������鎿��ID ( QUESTION_ID_xxxx )
 */
//--------------------------------------------------------------
#define _START_RESEARCH( req, q1, q2, q3 ) \
    _ASM_START_RESEARCH req, q1, q2, q3

  .macro _ASM_START_RESEARCH req, q1, q2, q3 
  .short EV_SEQ_START_RESEARCH
  .short \req
  .short \q1
  .short \q2
  .short \q3
  .endm

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�������Ƃ��Ă̒������I������
 */
//--------------------------------------------------------------
#define _FINISH_RESEARCH() \
    _ASM_FINISH_RESEARCH

  .macro _ASM_FINISH_RESEARCH
  .short EV_SEQ_FINISH_RESEARCH
  .endm

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�������Ƃ��Ē������̒����˗�ID���擾����
 *
 * @param ret [out] �������̒����˗�ID�̊i�[�� ( ���݂��Ȃ��ꍇ, RESEARCH_REQ_ID_NONE )
 */
//--------------------------------------------------------------
#define _GET_RESEARCH_REQUEST_ID( ret ) \
    _ASM_GET_RESEARCH_REQUEST_ID ret

  .macro _ASM_GET_RESEARCH_REQUEST_ID ret
  .short EV_SEQ_GET_RESEARCH_REQUEST_ID
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�������Ƃ��Ē������̎���ID���擾����
 *
 * @param ret_q1 [out] �������̎���ID�̊i�[�� ( ���݂��Ȃ��ꍇ, QUESTION_ID_DUMMY )
 * @param ret_q2 [out] �������̎���ID�̊i�[�� ( ���݂��Ȃ��ꍇ, QUESTION_ID_DUMMY )
 * @param ret_q3 [out] �������̎���ID�̊i�[�� ( ���݂��Ȃ��ꍇ, QUESTION_ID_DUMMY )
 */
//--------------------------------------------------------------
#define _GET_RESEARCH_QUESTION_ID( ret_q1, ret_q2, ret_q3 ) \
    _ASM_GET_RESEARCH_QUESTION_ID ret_q1, ret_q2, ret_q3

  .macro _ASM_GET_RESEARCH_QUESTION_ID ret_q1, ret_q2, ret_q3 
  .short EV_SEQ_GET_RESEARCH_QUESTION_ID
  .short \ret_q1
  .short \ret_q2
  .short \ret_q3
  .endm

//--------------------------------------------------------------
/**
 * @brief ����ɑ΂���, �ł������h�̉�ID���擾����
 *
 * @param qID [in]  ����ID ( QUESTION_ID_xxxx )
 * @param ret [out] ��ID�̊i�[��
 */
//--------------------------------------------------------------
#define _GET_MAJORITY_ANSWER_OF_QUESTION( qID, ret ) \
    _ASM_GET_MAJORITY_ANSWER_OF_QUESTION qID, ret

  .macro _ASM_GET_MAJORITY_ANSWER_OF_QUESTION qID, ret
  .short EV_SEQ_GET_MAJORITY_ANSWER_OF_QUESTION
  .short \qID
  .short \ret
  .endm 

//--------------------------------------------------------------
/**
 * @brief ����ɑ΂���, ���܂܂ł̉񓚐l�����擾����
 *
 * @param qID [in]  ����ID ( QUESTION_ID_xxxx )
 * @param ret [out] �񓚐l���̊i�[��
 */
//--------------------------------------------------------------
#define _GET_TOTAL_COUNT_OF_QUESTION( qID, ret ) \
    _ASM_GET_TOTAL_COUNT_OF_QUESTION qID, ret

  .macro _ASM_GET_TOTAL_COUNT_OF_QUESTION qID, ret
  .short EV_SEQ_GET_TOTAL_COUNT_OF_QUESTION
  .short \qID
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ����ɑ΂���, �����J�n���̉񓚐l�����擾����
 *
 * @param qID [in]  ����ID ( QUESTION_ID_xxxx )
 * @param ret [out] �񓚐l���̊i�[��
 */
//--------------------------------------------------------------
#define _GET_START_COUNT_OF_QUESTION( qID, ret ) \
    _ASM_GET_START_COUNT_OF_QUESTION qID, ret

  .macro _ASM_GET_START_COUNT_OF_QUESTION qID, ret
  .short EV_SEQ_GET_START_COUNT_OF_QUESTION
  .short \qID
  .short \ret
  .endm 

//--------------------------------------------------------------
/**
 * @brief �������̒����ɂ���, �o�߂�������[h]���擾����
 *
 * @param ret [out] �o�ߎ��Ԃ̊i�[��
 *
 * ���߂�l�͍ő�24!!
 */
//--------------------------------------------------------------
#define _GET_RESEARCH_PASSED_TIME( ret ) \
    _ASM_GET_RESEARCH_PASSED_TIME ret

  .macro _ASM_GET_RESEARCH_PASSED_TIME ret
  .short EV_SEQ_GET_RESEARCH_PASSED_TIME
  .short \ret
  .endm 

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�w���A���b�Z�[�W�x���͉�ʂ��Ăяo��
 *
 * @param ret [out] ���͂��m�肵�����ǂ������󂯎�郏�[�N ( TRUE: �m�� )
 */
//--------------------------------------------------------------
#define _CALL_CROSS_COMM_HELLO_MSG_INPUT( ret ) \
    _ASM_CALL_CROSS_COMM_HELLO_MSG_INPUT ret

  .macro _ASM_CALL_CROSS_COMM_HELLO_MSG_INPUT ret
  .short EV_SEQ_CALL_CROSS_COMM_HELLO_MSG_INPUT
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ�w���烁�b�Z�[�W�x���͉�ʂ��Ăяo��
 *
 * @param ret [out] ���͂��m�肵�����ǂ������󂯎�郏�[�N ( TRUE: �m�� )
 */
//--------------------------------------------------------------
#define _CALL_CROSS_COMM_THANKS_MSG_INPUT( ret ) \
    _ASM_CALL_CROSS_COMM_THANKS_MSG_INPUT ret

  .macro _ASM_CALL_CROSS_COMM_THANKS_MSG_INPUT ret
  .short EV_SEQ_CALL_CROSS_COMM_THANKS_MSG_INPUT
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief ����Ⴂ������, �����̉񓚂�ݒ肷��
 *
 * @param qID [in] ����ID ( QUESTION_ID_xxxx )
 * @param aIdx[in] ���Ԗڂ̉񓚂��H
 */
//--------------------------------------------------------------
#define _SET_MY_ANSWER( qID, aIdx ) \
    _ASM_SET_MY_ANSWER qID, aIdx

  .macro _ASM_SET_MY_ANSWER qID, aIdx
  .short EV_SEQ_SET_MY_ANSWER
  .short \qID
  .short \aIdx
  .endm

//--------------------------------------------------------------
/**
 * @brief ���[�N�̒l���o�͂���
 *
 * @param print_no [in] �o��No.
 * @param value    [in] �o�͂��郏�[�N
 */
//--------------------------------------------------------------
#define _DEBUG_PRINT( print_no, value ) \
    _ASM_DEBUG_PRINT print_no, value

  .macro _ASM_DEBUG_PRINT print_no, value
  .short EV_SEQ_DEBUG_PRINT
  .short \print_no
  .short \value
  .endm

//--------------------------------------------------------------
/**
 * @brief �������̑��������N���擾����
 *
 * @param ret [out] ���������N�̊i�[�� ( RESEARCH_TEAM_RANK_ 0�`5 )
 */
//--------------------------------------------------------------
#define _GET_RESEARCH_TEAM_RANK( ret ) \
    _ASM_GET_RESEARCH_TEAM_RANK ret

  .macro _ASM_GET_RESEARCH_TEAM_RANK ret
  .short EV_SEQ_GET_RESEARCH_TEAM_RANK
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief �������̑��������N��1�グ��
 */
//--------------------------------------------------------------
#define _RESEARCH_TEAM_RANK_UP() \
    _ASM_RESEARCH_TEAM_RANK_UP

  .macro _ASM_RESEARCH_TEAM_RANK_UP
  .short EV_SEQ_RESEARCH_TEAM_RANK_UP
  .endm

//--------------------------------------------------------------
/**
 * @brief �莝���̔z�z�|�P�̈ʒu��Ԃ�
 *
 * @param monsno    �Ώۃ����X�^�[�i���o�[
 * @param skill_flg �ŗL�Z�������Ă��Ȃ����Ƃ������ɉ�����ꍇ��TRUE
 * @param pos       �莝���|�P�����̈ʒu    
 * @param ret       �����𖞂������|�P�����������ꍇ��TRUE
 */
//--------------------------------------------------------------
#define _GET_EVT_POKE_POS( monsno, skill_flg, pos, ret ) \
    _ASM_GET_EVT_POKE_POS monsno, skill_flg, pos, ret

  .macro _ASM_GET_EVT_POKE_POS monsno, skill_flg, pos, ret
  .short EV_SEQ_GET_EVT_POKE_POS
  .short \monsno
  .short \skill_flg
  .short \pos
  .short \ret
  .endm

//--------------------------------------------------------------
/**
 * @brief �{�[���ړ��i�g�p�ꏊ�͓���|�P�C�x���g����j
 *
 * @param type      �{�[���A�j���^�C�v SCR_BALL_ANM_TYPE_�`�@script_def.h�Q��
 * @param tx        �ړI�w���W
 * @param ty        �ړI�x���W 0.5�O���b�h�T�C�Y���̎w��i2��ݒ肷���1�O���b�h�j
 * @param tz        �ړI�y���W
 * @param height    �A�j�����̃{�[������
 * @param sync      �K�v�V���N��
 */
//--------------------------------------------------------------
#define _MOVE_BALL( type, tx, ty, tz, height, sync ) \
    _ASM_MOVE_BALL type, tx, ty, tz, height, sync

  .macro _ASM_MOVE_BALL type, tx, ty, tz, height, sync
  .short EV_SEQ_MOVE_BALL
  .short \type
  .short \tx
  .short \ty
  .short \tz
  .short \height
  .short \sync
  .endm

//--------------------------------------------------------------
/**
 * @brief �{�[���A�j���J�n�i�g�p�ꏊ�͓���|�P�C�x���g����j
 *
 * @param type      �{�[���A�j���^�C�v SCR_BALL_ANM_TYPE_�`�@script_def.h�Q��
 * @param tx        �w���W
 * @param ty        �x���W 0.5�O���b�h�T�C�Y���̎w��i2��ݒ肷���1�O���b�h�j
 * @param tz        �y���W
 */
//--------------------------------------------------------------
#define _START_BALL_ANM( type, tx, ty, tz  ) \
    _ASM_START_BALL_ANM type, tx, ty, tz

  .macro _ASM_START_BALL_ANM type, tx, ty, tz
  .short EV_SEQ_START_BALL_ANM
  .short \type
  .short \tx
  .short \ty
  .short \tz
  .endm

//--------------------------------------------------------------
/**
 * @brief �{�[���A�j���I���҂��i�g�p�ꏊ�͓���|�P�C�x���g����j
 *
 * @param type      �{�[���A�j���^�C�v SCR_BALL_ANM_TYPE_�`�@script_def.h�Q��
 */
//--------------------------------------------------------------
#define _WAIT_BALL_ANM( type  ) \
    _ASM_WAIT_BALL_ANM type

  .macro _ASM_WAIT_BALL_ANM type
  .short EV_SEQ_WAIT_BALL_ANM
  .short \type
  .endm

//--------------------------------------------------------------
/**
 * @brief �{�[���A�j�����A�|�P������\���A��\�����Ă����^�C�~���O�܂ő҂i�g�p�ꏊ�͓���|�P�C�x���g����j
 *
 * @param type      �{�[���A�j���^�C�v SCR_BALL_ANM_TYPE_�`�@script_def.h�Q��
 */
//--------------------------------------------------------------
#define _WAIT_BALL_POKE_APP( type  ) \
    _ASM_WAIT_BALL_POKE_APP type

  .macro _ASM_WAIT_BALL_POKE_APP type
  .short EV_SEQ_WAIT_BALL_POKE_APP
  .short \type
  .endm

//--------------------------------------------------------------
/**
 * @brief �w��n�a�i���w��V���N�����Ďw��l���㏸������
 *
 * @param obj_id      �Ώۂn�a�i�h�c
 * @param height      �㏸�l  �i���O���b�h�P�ʁj
 * @param sync        �K�v�V���N��
 */
//--------------------------------------------------------------
#define _EVENT_RISE_OBJ( obj_id, height, sync  ) \
    _ASM_EVENT_RISE_OBJ obj_id, height, sync

  .macro _ASM_EVENT_RISE_OBJ obj_id, height, sync
  .short EV_SEQ_EVENT_RISE_OBJ
  .short \obj_id
  .short \height
  .short \sync
  .endm

//--------------------------------------------------------------
/**
 * @brief �w��n�a�i���C�x���g��]������
 *
 * @param obj_id      �Ώۂn�a�i�h�c
 */
//--------------------------------------------------------------
#define _EVENT_ROTATE_OBJ( obj_id ) \
    _ASM_EVENT_ROTATE_OBJ obj_id

  .macro _ASM_EVENT_ROTATE_OBJ obj_id
  .short EV_SEQ_EVENT_ROTATE_OBJ
  .short \obj_id
  .endm

//--------------------------------------------------------------
/**
 * @brief �|�P�����E�B���h�E���o���i�p�b�`�[���̃u�`��Ή��j
 *
 * @param monsno      �����X�^�[�i���o�[
 * @param form        �t�H�����i���o�[
 * @param sex         ����
 * @param rare        ���A�J���[�@0�F�ʏ�@1�F���A
 */
//--------------------------------------------------------------
#define _DISP_POKE_WIN( monsno, form, sex, rare ) \
    _ASM_DISP_POKE_WIN monsno, form, sex, rare

  .macro _ASM_DISP_POKE_WIN monsno, form, sex, rare
  .short EV_SEQ_DISP_POKE_WIN
  .short \monsno
  .short \form
  .short \sex
  .short \rare
  .endm

//--------------------------------------------------------------
/**
 * @brief �莝���C�x���g�|�P�����ɃC�x���g�I���t���O���Z�b�g����
 *
 * @param monsno      �����X�^�[�i���o�[
 * @param pos    �莝���̈ʒu
 */
//--------------------------------------------------------------
#define _SET_EVT_POKE_AFT_FLG( monsno, pos ) \
    _ASM_SET_EVT_POKE_AFT_FLG monsno, pos

  .macro _ASM_SET_EVT_POKE_AFT_FLG monsno, pos
  .short EV_SEQ_SET_EVT_POKE_AFT_FLG
  .short \monsno
  .short \pos
  .endm

