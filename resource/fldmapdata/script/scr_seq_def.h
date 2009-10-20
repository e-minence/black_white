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
  .short  EV_SEQ_COMMON_PROC_FIELD_EVENT_END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  SP_EVENT_START  label
\label:
  .endm

//--------------------------------------------------------------
/**
 */
//--------------------------------------------------------------
  .macro  SP_EVENT_END
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
 *  _CHG_LOCAL_SCR ���ʃX�N���v�g���I�����āA���[�J���X�N���v�g���ĊJ�����܂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _CHG_LOCAL_SCR
  .short  EV_SEQ_CHG_LOCAL_SCR
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

//--------------------------------------------------------------
/**
 *  _ASM_FLAG_CHECK_VM �t���O�`�F�b�N
 *  @param �`�F�b�N����t���O�i���o�[
 */
//--------------------------------------------------------------
  .macro  _ASM_FLAG_CHECK_VM num
  .short  EV_SEQ_FLAG_CHECK_VM
  .short  \num
  .endm

//--------------------------------------------------------------
/*
 *  _IF_FLAGON_JUMP �t���OON�̎��ɕ���(JUMP)
 *  @param num �`�F�b�N����t���O�i���o�[
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGON_JUMP num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_JUMP  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGOFF_JUMP �t���OOFF�̎��ɕ���(JUMP)
 *  @param num �`�F�b�N����t���O�i���o�[
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------

  .macro  _IF_FLAGOFF_JUMP num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_JUMP  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGON_CALL �t���OON�̎��ɕ���(CALL)
 *  @param num �`�F�b�N����t���O�i���o�[
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGON_CALL num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_CALL  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_FLAGOFF_CALL �t���OOFF�̎��ɕ���(CALL)
 *  @param num �`�F�b�N����t���O�i���o�[
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_FLAGOFF_CALL num,adrs
  _ASM_FLAG_CHECK_VM \num
  _IF_CALL  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 * _FLAG_CHECK_WK ���[�N�̒l���t���O�i���o�[�Ƃ��ă`�F�b�N����I
 * @param wk �`�F�b�N����l���i�[���ꂽ���[�N
 * @param ret_wk ���ʂ��i�[���郏�[�N
 */
//--------------------------------------------------------------
  .macro  _FLAG_CHECK_WK wk,ret_wk
  .short  EV_SEQ_FLAG_CHECK_WK
  .short  \wk
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _FLAG_SET_WK ���[�N�̒l���t���O�i���o�[�Ƃ��ăZ�b�g����I
 * @param wk �t���O�i���o�[���i�[���ꂽ���[�N
 */
//--------------------------------------------------------------
  .macro  _FLAG_SET_WK wk
  .short  EV_SEQ_FLAG_SET_WK
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
  .macro  _ADD_WK wk,num
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
  .macro  _SUB_WK wk,num
  .short  EV_SEQ_SUB_WK
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

//======================================================================
//  �t�B�[���h�@��b�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALKMSG �W�J���b�Z�[�W��\��(1byte)
 *  @param  msg_id  �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _TALKMSG( msg_id ) _ASM_TALK_MSG msg_id

  .macro  _ASM_TALK_MSG msg_id
  .short  EV_SEQ_TALKMSG
  .short  \msg_id
  .endm

//--------------------------------------------------------------
/**
 *  _TALKMSG_ALLPUT �W�J���b�Z�[�W��\��(1byte)
 *  @param  msg_id  �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _TALKMSG_ALLPUT( msg_id ) _ASM_TALKMSG_ALLPUT msg_id

  .macro  _ASM_TALKMSG_ALLPUT msg_id
  .short  EV_SEQ_TALKMSG_ALLPUT
  .short  \msg_id
  .endm

//--------------------------------------------------------------
/**
 *  _TALKWIN_OPEN ��b�E�B���h�E�J��
 *  @param none
 */
//--------------------------------------------------------------
#define _TALKWIN_OPEN() _ASM_TALKWIN_OPEN

  .macro  _ASM_TALKWIN_OPEN
  .short  EV_SEQ_TALKWIN_OPEN
  .endm

//--------------------------------------------------------------
/**
 *  _TALKWIN_CLOSE ��b�E�B���h�E����
 *  @param none
 */
//--------------------------------------------------------------
#define _TALKWIN_CLOSE()  _ASM_TALKWIN_CLOSE

  .macro  _ASM_TALKWIN_CLOSE
  .short  EV_SEQ_TALKWIN_CLOSE
  .endm

//======================================================================
//  �t�B�[���h�@�����o���E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_OPEN
 *  @brief  �����o���E�B���h�E�`��
 *  @param msg_id �\�����郁�b�Z�[�WID
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_OBJMSG_OPEN( msg_id , obj_id ) _ASM_BALLOONWIN_OBJMSG_OPEN msg_id, obj_id

  .macro _ASM_BALLOONWIN_OBJMSG_OPEN msg_id, obj_id
  .short  EV_SEQ_BALLOONWIN_OBJMSG_OPEN
  .short 0x0400
  .short \msg_id
  .byte \obj_id
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_TALKOBJ_OPEN
 *  @brief  �����o���E�B���h�E�`��@�b���|��OBJ��p
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_OPEN( msg_id ) _ASM_BALLOONWIN_TALKOBJ_OPEN msg_id

  .macro _ASM_BALLOONWIN_TALKOBJ_OPEN msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_OPEN
  .short 0x0400
  .short \msg_id
  .endm

//--------------------------------------------------------------
/**
 * @def _BALLOONWIN_TALKOBJ_OPEN_ARC
 *
 * @brief �����o���E�B���h�E�`�� �b������OBJ��p�A���b�Z�[�W�A�[�J�C�u�w��t��
 * 
 *  @param  arc_id  ���b�Z�[�W�A�[�J�C�u�w��ID
 *  @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _BALLOONWIN_TALKOBJ_OPEN_ARC( arc_id, msg_id ) \
    _ASM_BALLOONWIN_TALKOBJ_OPEN_ARC arc_id, msg_id

  .macro  _ASM_BALLOONWIN_TALKOBJ_OPEN_ARC arc_id, msg_id
  .short  EV_SEQ_BALLOONWIN_TALKOBJ_OPEN
  .short \arc_id
  .short \msg_id
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
 *  @def _GOLD_WIN_OPEN
 *  @brief �������E�B���h�E��\������
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
 * _OBJ_ADD_EX OBJ��ǉ�
 * @param x �\������O���b�hX���W
 * @param z �\������O���b�hZ���W
 * @param dir ���� DIR_UP��
 * @param id ���ʗpOBJ ID
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
 * _OBJ_DEL_EX OBJ���폜
 * @param id �폜����OBJ ID
 */
//--------------------------------------------------------------
#define _OBJ_DEL_EX( id ) _ASM_OBJ_DEL_EX id

.macro  _ASM_OBJ_DEL_EX id
  .short EV_SEQ_OBJ_DEL
  .short \id
.endm
 
//--------------------------------------------------------------
/**
 * _OBJ_ADD �]�[�����Ŕz�u����Ă���OBJ�ꗗ���A�w���ID������OBJ��ǉ�
 * @param id �z�uOBJ���Œǉ�����OBJ ID
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
 * �I�I�g�p�֎~�ł��I�I�@�^�C�~���O���݂ăR�}���h���폜�\��
 * _OBJ_DEL_EV �w���OBJ_ID��������OBJ���폜�B
 * �폜���A�Ώ�OBJ�Ŏw�肳��Ă���C�x���g�t���O��ON�ɂ���B
 * @param id �z�uOBJ���Œǉ�����OBJ ID
 */
//--------------------------------------------------------------
#define _OBJ_DEL( id )  \
    _ASM_OBJ_DEL id

  .macro _ASM_OBJ_DEL id
  .short EV_SEQ_OBJ_DEL_EV
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
 *  @retval 0�̂Ƃ��A�u�͂��v��I��
 *  @retval 1�̂Ƃ��A�u�������v�܂��̓L�����Z��
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
 * �莝���|�P�����̃j�b�N�l�[�����w��^�O�ɃZ�b�g
 * @param idx �Z�b�g����^�O�i���o�[
 * @param pos �j�b�N�l�[�����擾����莝���|�P��������
 * @param pocket_id   �o�b�O�̃|�P�b�g�����w��ID
 */
//--------------------------------------------------------------
#define _NICK_NAME( idx, pos ) _ASM_NICK_NAME idx, pos

  .macro  _ASM_NICK_NAME idx, pos
  .short  EV_SEQ_NICK_NAME
  .byte   \idx
  .short  \pos
  .endm


//======================================================================
//  �����g���[�i�[�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET �����F�g���[�i�[�ړ��Ăяo��
 *  @param pos �ړ�������g���[�i�[ �f�[�^�ԍ�0,1
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_SET  pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SET
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET �����F�g���[�i�[�ړ��@�V���O��
 *  @param pos �ړ�������g���[�i�[ �f�[�^�ԍ�0,1
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_SINGLE pos
  .short  EV_SEQ_EYE_TRAINER_MOVE_SINGLE
  .short  \pos
  .endm
 
//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_MOVE_SET �����F�g���[�i�[�ړ��@�_�u��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_MOVE_DOUBLE
  .short  EV_SEQ_EYE_TRAINER_MOVE_DOUBLE
  .endm

//--------------------------------------------------------------
/**
 *  _EYE_TRAINER_TYPE_GET �����F�g���[�i�[�^�C�v�擾
 *  @param ret_wk �g���[�i�[�^�C�v�i�[�� SCR_EYE_TR_TYPE_SINGLE�����i�[�����
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_TYPE_GET ret_wk
  .short  EV_SEQ_EYE_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  �����F�g���[�i�[ID�擾
 */
//--------------------------------------------------------------
  .macro  _EYE_TRAINER_ID_GET  pos,ret_wk
  .short  EV_SEQ_EYE_TRAINER_ID_GET
  .short  \pos
  .short  \ret_wk
  .endm

//======================================================================
//  �g���[�i�[�o�g���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _TRAINER_ID_GET �g���[�i�[ID�擾
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
 *  _TRAINER_BTL_SET �g���[�i�[�퓬�Ăяo��
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
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_MULTI_BTL_SET �g���[�i�[�퓬�i�}���`�o�g���j�Ăяo��
 *  @param partner_id �p�[�g�i�[ID
 *  @param tr_id_0 �g���[�i�[ID0
 *  @param tr_id_1 �g���[�i�[ID1
 */
//--------------------------------------------------------------
#define _TRAINER_MULTI_BTL_SET( partner_id, tr_id_0, tr_id_1 ) \
  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1

  .macro  _ASM_TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1
  .short  EV_SEQ_TRAINER_MULTI_BTL_SET
  .short  \partner_id
  .short  \tr_id_0
  .short  \tr_id_1
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
 *  _TRAINER_LOSE �g���[�i�[�s�k
 *  @param none
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE() \
  _ASM_TRAINER_LOSE

  .macro  _ASM_TRAINER_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE_CHECK �g���[�i�[�s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[�� 0=�s�k 1=����
 */
//--------------------------------------------------------------
#define _TRAINER_LOSE_CHECK( ret_wk ) _ASM_TRAINER_LOSE_CHECK ret_wk

  .macro  _ASM_TRAINER_LOSE_CHECK ret_wk
  .short  EV_SEQ_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _NORMAL_LOSE  �s�k����
 * @param none
 */
//--------------------------------------------------------------
#define _NORMAL_LOSE() \
  _ASM_NORMAL_LOSE

  .macro  _ASMNORMAL_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _LOSE_CHECK �s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[�� 0=�s�k 1=����
 */
//--------------------------------------------------------------
#define  _LOSE_CHECK(  ret_wk ) \
  _ASM_LOSE_CHECK  ret_wk

  .macro  _ASM_LOSE_CHECK  ret_wk
  .short  EV_SEQ_LOSE_CHECK
  .short  \ret_wk
  .endm

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

//--------------------------------------------------------------
/**
 *  _2VS2_BATTLE_CHECK �莝���`�F�b�N 2vs2���\���擾
 *  @param ret_wk ���ʊi�[��
 */
//--------------------------------------------------------------
  .macro  _2VS2_BATTLE_CHECK ret_wk
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
  .macro  _TRAINER_FLAG_SET tr_id
  .short  EV_SEQ_TRAINER_FLAG_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_RESET �g���[�i�[�t���O�̃��Z�b�g
 *  @param tr_id ���Z�b�g����g���[�i�[ID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_FLAG_RESET tr_id
  .short  EV_SEQ_TRAINER_FLAG_RESET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_FLAG_CHECK �g���[�i�[�t���O�`�F�b�N
 *  @param tr_id �`�F�b�N����g���[�i�[ID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_FLAG_CHECK tr_id
  .short  EV_SEQ_TRAINER_FLAG_CHECK
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGON_JUMP �t���OON�̎��ɕ���(JUMP)
 *  @param tr_id �g���[�i�[ID
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGON_JUMP tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_JUMP  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGOFF_JUMP �t���OOFF�̎��ɕ���(JUMP)
 *  @param tr_id �g���[�i�[ID
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGOFF_JUMP tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_JUMP  FLGOFF,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGON_CALL �t���OON�̎��ɕ���(CALL)
 *  @param tr_id �g���[�i�[ID
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGON_CALL tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_CALL  FLGON,\adrs
  .endm

//--------------------------------------------------------------
/**
 *  _IF_TR_FLAGOFF_CALL �t���OOFF�̎��ɕ���(CALL)
 *  @param tr_id �g���[�i�[ID
 *  @param adrs ��������v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _IF_TR_FLAGOFF_CALL tr_id,adrs
  _TRAINER_FLAG_CHECK \tr_id
  _IF_CALL  FLGOFF,\adrs
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
  .byte  \cursor
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
 */
//--------------------------------------------------------------
#define _BMPMENU_INIT_EX( x,y,cursor,cancel,ret_wk ) \
   _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk

   .macro  _ASM_BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk
  .short  EV_SEQ_BMPMENU_INIT_EX
  .byte  \x
  .byte  \y
  .byte  \cursor
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
    _ASM_BMPMENU_MAKE_LIST msg_id, param

  .macro  _ASM_BMPMENU_MAKE_LIST msg_id,param
  .short  EV_SEQ_BMPMENU_MAKE_LIST
  .short  \msg_id
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
 *  _DISP_FADE_START ��ʃt�F�[�h�I���`�F�b�N
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
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
#define _WHITE_OUT( speed ) _DISP_FADE_START( DISP_FADE_WHITEOUT_MAIN,0,16,speed )

//--------------------------------------------------------------
/**
 *  @def  _WHITE_IN
 *  @brief  �z���C�g�C��
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
#define _WHITE_IN( speed ) _DISP_FADE_START( DISP_FADE_WHITEOUT_MAIN,16,0,speed )

//--------------------------------------------------------------
/**
 *  @def  _BLACK_OUT
 *  @brief  �u���b�N�A�E�g
 *  @param  speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
#define _BLACK_OUT( speed ) _DISP_FADE_START( DISP_FADE_BLACKOUT_MAIN,0,16,speed )

//--------------------------------------------------------------
/**
 *  @def  _BLACK_IN
 *  @brief  �u���b�N�C��
 *  @param  speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
#define _BLACK_IN( speed ) _DISP_FADE_START( DISP_FADE_BLACKOUT_MAIN,16,0,speed )

//======================================================================
//  �A�C�e���֘A
//======================================================================
//--------------------------------------------------------------
/**
 *	�A�C�e����������
 *	@param item_no ������A�C�e���i���o�[
 *	@param num �������
 *	@param ret_wk ���ʑ���� TRUE=���� FALSE=���s
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
#define _SUB_ITEM( item_no,num,ret_wk ) _ASM_SUB_ITEM item_no,num_ret_wk

	.macro	_ASM_SUB_ITEM item_no, num, ret_wk
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
 *  _MUSICAL_CALL �~���[�W�J���Ăяo��
 *  @param none
 */
//--------------------------------------------------------------
#define _MUSICAL_CALL() _ASM_MUSICAL_CALL

  .macro  _ASM_MUSICAL_CALL
  .short EV_SEQ_MUSICAL_CALL
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
 * @def _FIELD_COMM_EXIT
 * @brief �t�B�[���h�}�b�v�̒ʐM���I������
 */
//--------------------------------------------------------------
#define _FIELD_COMM_EXIT() \
    _ASM_FIELD_COMM_EXIT

  .macro  _ASM_FIELD_COMM_EXIT
  .short  EV_SEQ_FIELD_COMM_EXIT
  .endm

//======================================================================
//  �v���O�����Ǘ��f�[�^�̎擾�E�Z�b�g
//======================================================================
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
 * @param count_mode �������w��(POKECOUNT_MODE_XXXX)
		�莝���|�P�������J�E���g���[�h
		POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
		POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
		POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
		POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������) 
    POKECOUNT_MODE_ONLY_DAME_EGG (4)  // �ʖڃ^�}�S�̐�
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
  .short  EV_SEQ_GET_TEMOTI_NATSUKI
  .short  \ret_wk
  .short  \pos
  .byte   0
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
  .byte   1
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
  .byte   2
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
//======================================================================
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
 * @param gz        �ړ���x�ʒu�w��i�O���b�h�P�ʁj
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
 * �}�b�v�J�ڃR�}���h(���j�I�����[��)
 */
//--------------------------------------------------------------
#define _MAP_CHANGE_TO_UNION() \
    _ASM_MAP_CHANGE_TO_UNION 

  .macro  _ASM_MAP_CHANGE_TO_UNION 
  .short  EV_SEQ_MAP_CHANGE_TO_UNION
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
 * @def _DOOR_ANIME_CREATE
 * @brief �h�A�A�j���R���g���[���[�̐���
 * @param ret_wk  anm_id�������Ƃ邽�߂̃��[�N
 * @param gx      �A�j����������z�u���f����X�ʒu�i�O���b�h�P�ʁj
 * @param gz      �A�j����������z�u���f����Z�ʒu�i�O���b�h�P�ʁj
 *
 * ret_wk�Ŏ󂯎�����A�j���w��ID���g���ăh�A�A�j���e�R�}���h
 * �i_DOOR_ANIME_DELETE/_DOOR_ANIME_SET/_DOOR_ANIME_WAIT�j��
 * �Ăяo���B
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_CREATE( ret_wk, gx, gz ) \
    _ASM_DOOR_ANIME_CREATE ret_wk, gx, gz

    .macro _ASM_DOOR_ANIME_CREATE ret_wk, gx, gz
    .short  EV_SEQ_DOOR_ANIME_CREATE
    .short  \ret_wk
    .short  \gx
    .short  \gz
    .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_DELETE
 * @brief �h�A�A�j���R���g���[���[�̍폜
 * @param anm_id  �z�u���f�����w�肷�邽�߂�ID
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_DELETE( anm_id ) \
     _ASM_DOOR_ANIME_DELETE anm_id

     .macro _ASM_DOOR_ANIME_DELETE anm_id
     .short EV_SEQ_DOOR_ANIME_DELETE
     .short \anm_id
     .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_SET
 * @brief �h�A�A�j���̃Z�b�g
 * @param anm_id    �z�u���f�����w�肷�邽�߂�ID
 * @param anm_type  �A�j���[�V�����̎�ގw��
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_SET( anm_id, anm_type ) \
    _ASM_DOOR_ANIME_SET anm_id, anm_type

    .macro _ASM_DOOR_ANIME_SET anm_id, anm_type
    .short  EV_SEQ_DOOR_ANIME_SET
    .short  \anm_id
    .short  \anm_type
    .endm

//--------------------------------------------------------------
/**
 * @def _DOOR_ANIME_WAIT
 * @brief �h�A�A�j���̏I���҂�
 * @param anm_id  �z�u���f�����w�肷�邽�߂�ID
 */
//--------------------------------------------------------------
#define _DOOR_ANIME_WAIT( anm_id ) \
    _ASM_DOOR_ANIME_WAIT anm_id

    .macro _ASM_DOOR_ANIME_WAIT anm_id
    .short  EV_SEQ_DOOR_ANIME_WAIT
    .short  \anm_id
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

//======================================================================
//
//  �ȈՃR�}���h
//  ���X�N���v�g�R�}���h��g�ݍ��킹������
//
//======================================================================
  .include  "easy_event_def.h"

//--------------------------------------------------------------
/**
 * �ȈՃ��b�Z�[�W�\�� BG�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _EASY_MSG( msg_id )   _ASM_EASY_MSG msg_id


//--------------------------------------------------------------
/**
 * �ȈՐ����o�����b�Z�[�W�\�� �b���|��OBJ�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
#define _EASY_BALLOONWIN_TALKOBJ_MSG( msg_id )  _ASM_EASY_BALLOONWIN_TALKOBJ_MSG msg_id


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
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_KEYWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_KEYWAIT itemno, num

//--------------------------------------------------------------
/**
 * �C�x���g����C�x���g�u���v�҂��Ȃ�
 */
//--------------------------------------------------------------
#define _ITEM_EVENT_NOWAIT(itemno, num)   \
    _ASM_ITEM_EVENT_KEYWAIT itemno, num

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
 * @param ret_wk
 */
//--------------------------------------------------------------
#define _REPORT_EVENT_CALL( ret_wk )  \
    _ASM_REPORT_EVENT_CALL  ret_wk


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
  
  .macro _ASM_CAMERA_MOVE pitch, yaw, dist, x, y, z, fream
  .short EV_SEQ_CAMERA_MOVE
  .short \pitch
  .short \yaw
  .short \dist
  .long \x
  .long \y
  .long \z
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
 * �J�����ړ��҂�
 */
//--------------------------------------------------------------
#define _CAMERA_WAIT_MOVE() _ASM_CAMERA_WAIT_MOVE
  
  .macro _ASM_CAMERA_WAIT_MOVE
  .short EV_SEQ_CAMERA_WAIT_MOVE
  .endm


