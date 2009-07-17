//======================================================================
/**
 * @file  scr_seq_def.def
 * @bfief  �X�N���v�g�R�}���h�̃}�N����`�t�@�C��
 * @author  HisashiSogabe
 * @date  2005.05.24
 *
 * 05.08.04 Satoshi Nohara
 *
 * �Z�V�[�P���X�p�̃}�N����`�t�@�C�������ɂ��Ă���
 */
//======================================================================
#define  __ASM_NO_DEF_

//�X�N���v�g�Ŏg�p����w�b�_�[�t�@�C��
  .include  "usescript.h"
//  .include  "..\..\..\include\pl_bugfix.h"

//�f�[�^�����R�ȋ��E�ɃA���C�����邩�̐ݒ�
  .option alignment off

//======================================================================
//  macro
//======================================================================
  .macro  INIT_CMD
DEF_CMD_COUNT  =  0
  .endm
    
  .macro  DEF_CMD symname
\symname  =  DEF_CMD_COUNT
DEF_CMD_COUNT  =  ( DEF_CMD_COUNT + 1 )
  .endm

//======================================================================
//  ���߃V���{���錾
//======================================================================
  INIT_CMD
  //��{�V�X�e������
  DEF_CMD EV_SEQ_NOP
  DEF_CMD EV_SEQ_DUMMY
  DEF_CMD EV_SEQ_END
  DEF_CMD EV_SEQ_TIME_WAIT
  DEF_CMD EV_SEQ_CALL
  DEF_CMD EV_SEQ_RET
  
  //�f�[�^���[�h�E�X�g�A�֘A
  DEF_CMD EV_SEQ_LD_REG_VAL
  DEF_CMD EV_SEQ_LD_REG_WDATA
  DEF_CMD EV_SEQ_LD_REG_ADR
  DEF_CMD EV_SEQ_LD_ADR_VAL
  DEF_CMD EV_SEQ_LD_ADR_REG
  DEF_CMD EV_SEQ_LD_REG_REG
  DEF_CMD EV_SEQ_LD_ADR_ADR
  
  //��r����
  DEF_CMD EV_SEQ_CP_REG_REG
  DEF_CMD EV_SEQ_CP_REG_VAL
  DEF_CMD EV_SEQ_CP_REG_ADR
  DEF_CMD EV_SEQ_CP_ADR_REG
  DEF_CMD EV_SEQ_CP_ADR_VAL
  DEF_CMD EV_SEQ_CP_ADR_ADR
  DEF_CMD EV_SEQ_CP_WK_VAL
  DEF_CMD EV_SEQ_CP_WK_WK
  
  //���z�}�V���֘A
  DEF_CMD EV_SEQ_VM_ADD
  DEF_CMD EV_SEQ_CHG_COMMON_SCR
  DEF_CMD EV_SEQ_CHG_LOCAL_SCR
  
  //���򖽗�
  DEF_CMD EV_SEQ_JUMP
  DEF_CMD EV_SEQ_IF_JUMP
  DEF_CMD EV_SEQ_IF_CALL
  DEF_CMD EV_SEQ_OBJ_ID_JUMP
  DEF_CMD EV_SEQ_BG_ID_JUMP
  DEF_CMD EV_SEQ_PLAYER_DIR_JUMP
  
  //�C�x���g���[�N�֘A
  DEF_CMD EV_SEQ_FLAG_SET
  DEF_CMD EV_SEQ_FLAG_RESET
  DEF_CMD EV_SEQ_FLAG_CHECK
  DEF_CMD EV_SEQ_FLAG_CHECK_WK
  DEF_CMD EV_SEQ_FLAG_SET_WK
  
  //���[�N����֘A
  DEF_CMD EV_SEQ_ADD_WK
  DEF_CMD EV_SEQ_SUB_WK
  DEF_CMD EV_SEQ_LD_WK_VAL
  DEF_CMD EV_SEQ_LD_WK_WK
  DEF_CMD EV_SEQ_LD_WK_WKVAL
  
  //�L�[���͊֘A
  DEF_CMD EV_SEQ_ABKEYWAIT
  
  //�t�B�[���h�@���b�Z�[�W�\��
  DEF_CMD EV_SEQ_TALKMSG_ALLPUT
  
  //�t�B�[���h�@���b�Z�[�W�E�B���h�E
  DEF_CMD EV_SEQ_TALKWIN_OPEN
  DEF_CMD EV_SEQ_TALKWIN_CLOSE
  
  //���샂�f��
  DEF_CMD EV_SEQ_OBJ_ANIME      
  DEF_CMD EV_SEQ_OBJ_ANIME_WAIT
  DEF_CMD EV_SEQ_MOVE_CODE_GET
  DEF_CMD EV_SEQ_OBJ_POS_GET
  DEF_CMD EV_SEQ_PLAYER_POS_GET
  DEF_CMD EV_SEQ_OBJ_ADD
  DEF_CMD EV_SEQ_OBJ_DEL

  //���샂�f���@�C�x���g�֘A
  DEF_CMD EV_SEQ_OBJ_PAUSE_ALL
  DEF_CMD EV_SEQ_TALK_OBJ_PAUSE_ALL
  DEF_CMD EV_SEQ_OBJ_PAUSE_CLEAR_ALL
  DEF_CMD EV_SEQ_OBJ_TURN
  
  //�͂��A�������@����
  DEF_CMD EV_SEQ_YES_NO_WIN
  
  //WORDSET�֘A
  DEF_CMD EV_SEQ_PLAYER_NAME
  
  //�����g���[�i�[�֘A
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_SET
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_SINGLE
  DEF_CMD EV_SEQ_EYE_TRAINER_MOVE_DOUBLE
  DEF_CMD EV_SEQ_EYE_TRAINER_TYPE_GET
  DEF_CMD EV_SEQ_EYE_TRAINER_ID_GET
  
  //�g���[�i�[�o�g���֘A
  DEF_CMD EV_SEQ_TRAINER_ID_GET
  DEF_CMD EV_SEQ_TRAINER_BTL_SET
  DEF_CMD EV_SEQ_TRAINER_MULTI_BTL_SET
  DEF_CMD EV_SEQ_TRAINER_MSG_SET
  DEF_CMD EV_SEQ_TRAINER_TALK_TYPE_GET
  DEF_CMD EV_SEQ_REVENGE_TRAINER_TALK_TYPE_GET
  DEF_CMD EV_SEQ_TRAINER_TYPE_GET
  DEF_CMD EV_SEQ_TRAINER_BGM_SET
  DEF_CMD EV_SEQ_LOSE
  DEF_CMD EV_SEQ_LOSE_CHECK
  DEF_CMD EV_SEQ_SEACRET_POKE_RETRY_CHECK
  DEF_CMD EV_SEQ_HAIFU_POKE_RETRY_CHECK
  DEF_CMD EV_SEQ_2VS2_BATTLE_CHECK
  DEF_CMD EV_SEQ_DEBUG_BTL_SET
  DEF_CMD EV_SEQ_BATTLE_RESULT_GET
  
  //�g���[�i�[�t���O�֘A
  DEF_CMD EV_SEQ_TRAINER_FLAG_SET
  DEF_CMD EV_SEQ_TRAINER_FLAG_RESET
  DEF_CMD EV_SEQ_TRAINER_FLAG_CHECK
  
  //BGM
  DEF_CMD EV_SEQ_BGM_PLAY
  DEF_CMD EV_SEQ_BGM_STOP
  DEF_CMD EV_SEQ_BGM_PLAYER_PAUSE
  DEF_CMD EV_SEQ_BGM_PLAY_CHECK
  DEF_CMD EV_SEQ_BGM_FADEOUT
  DEF_CMD EV_SEQ_BGM_FADEIN
  DEF_CMD EV_SEQ_BGM_NOW_MAP_PLAY
  
  //SE
  DEF_CMD EV_SEQ_SE_PLAY
  DEF_CMD EV_SEQ_SE_STOP
  DEF_CMD EV_SEQ_SE_WAIT
  
  //ME
  DEF_CMD EV_SEQ_ME_PLAY
  DEF_CMD EV_SEQ_ME_WAIT
  
  //���j���[
  DEF_CMD EV_SEQ_BMPMENU_INIT
  DEF_CMD EV_SEQ_BMPMENU_INIT_EX
  DEF_CMD EV_SEQ_BMPMENU_MAKE_LIST
  DEF_CMD EV_SEQ_BMPMENU_START

  //���̑�
  DEF_CMD EV_SEQ_CHG_LANGID
  DEF_CMD EV_SEQ_GET_RND

//======================================================================
// �C�x���g�f�[�^�֘A
//======================================================================
#define EV_DATA_END_CODE  (0xfd13)  //�K���Ȓl

//--------------------------------------------------------------
/**
 *  _EVENT_DATA �C�x���g�f�[�^�@�e�[�u���錾
 *  @param adrs �錾����f�[�^���x���A�h���X
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA  adrs
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _EVENT_DATA_END �C�x���g�f�[�^�@�e�[�u���錾�I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EVENT_DATA_END
  .short  EV_DATA_END_CODE
  .endm

//--------------------------------------------------------------
/**
 *  _SP_EVENT_DATA_END ����X�N���v�g�I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _SP_EVENT_DATA_END
  .byte   SP_SCRID_NONE
  .endm

//======================================================================
//  ��{�R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 *  _NOP �������Ȃ�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _NOP
  .short  EV_SEQ_NOP
  .endm

//--------------------------------------------------------------
/**
 *  _DUMMY �_�~�[
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _DUMMY
  .short  EV_SEQ_DUMMY
  .endm

//--------------------------------------------------------------
/**
 *  _END �X�N���v�g�̏I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _END
  .short  EV_SEQ_END
  .endm

//--------------------------------------------------------------
/**
 *  _TIME_WAIT �E�F�C�g
 *  @param time �҂��t���[����
 *  @param ret_wk �҂����Ԋi�[�惏�[�NID
 */
//--------------------------------------------------------------
  .macro  _TIME_WAIT time,ret_wk
  .short  EV_SEQ_TIME_WAIT
  .short  \time
  .short  \ret_wk
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
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _RET
  .short  EV_SEQ_RET
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

//--------------------------------------------------------------
/**
 *  _OBJ_ID_JUMP �b������OBJ��ID���r���W�����v
 *  @param act ��r����OBJID
 *  @param adrs ��v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _OBJ_ID_JUMP act,adrs
  .short  EV_SEQ_OBJ_ID_JUMP
  .byte  \act
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _BG_ID�@�b���|��BG��ID���r���W�����v
 *  @param act ��r����BGID
 *  @param adrs ��v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _BG_ID_JUMP act,adrs
  .short  EV_SEQ_BG_ID_JUMP
  .byte  \act
  .long  ((\adrs-.)-4)
  .endm

//--------------------------------------------------------------
/**
 *  _PLAYER_DIR_JUMP �C�x���g�N�����̎�l���̌�����r�W�����v
 *  (���݂̌����ł͂Ȃ��̂Œ��ӁI)
 *  @param dir ���� DIR_UP��
 *  @param adrs ��v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _PLAYER_DIR_JUMP dir,adrs
  .short  EV_SEQ_PLAYER_DIR_JUMP
  .byte  \dir
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
  .macro  _FLAG_SET num
  .short  EV_SEQ_FLAG_SET
  .short  \num
  .endm

//--------------------------------------------------------------
/*
 *  _ARRIVE_FLAG_SET �����t���O�Z�b�g
 * @param num �Z�b�g����t���O�i���o�[
 */
//--------------------------------------------------------------
  .macro  _ARRIVE_FLAG_SET num
  _FLAG_SET  (\num + SYS_FLAG_ARRIVE_START)
  .endm

//--------------------------------------------------------------
/**
 *  _FLAG_RESET �t���O�̃��Z�b�g
 *  @param ���Z�b�g����t���O�i���o�[
 */
//--------------------------------------------------------------
  .macro  _FLAG_RESET num
  .short  EV_SEQ_FLAG_RESET
  .short  \num
  .endm

//--------------------------------------------------------------
/**
 *  _FLAG_CHECK �t���O�`�F�b�N
 *  @param �`�F�b�N����t���O�i���o�[
 */
//--------------------------------------------------------------
  .macro  _FLAG_CHECK num
  .short  EV_SEQ_FLAG_CHECK
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
  _FLAG_CHECK \num
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
  _FLAG_CHECK \num
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
  _FLAG_CHECK \num
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
  _FLAG_CHECK \num
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
  .macro  _LDVAL  wk,val
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
  .macro  _LDWK  wk1,wk2
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
  .macro  _LDWKVAL  wk1,wk2
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
  .macro  _AB_KEYWAIT
  .short  EV_SEQ_ABKEYWAIT
  .endm

//======================================================================
//  �t�B�[���h�@���b�Z�[�W�\��
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALKMSG_ALLPUT �W�J���b�Z�[�W��\��(1byte)
 *  @param  msg_id  �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
  .macro  _TALKMSG_ALLPUT msg_id
  .short  EV_SEQ_TALKMSG_ALLPUT
  .byte  \msg_id
  .endm

//======================================================================
//  �t�B�[���h�@���b�Z�[�W�E�B���h�E
//======================================================================
//--------------------------------------------------------------
/**
 *  _TALKWIN_OPEN ��b�E�B���h�E�J��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TALKWIN_OPEN
  .short  EV_SEQ_TALKWIN_OPEN
  .endm

//--------------------------------------------------------------
/**
 *  _TALKWIN_CLOSE ��b�E�B���h�E����
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TALKWIN_CLOSE
  .short  EV_SEQ_TALKWIN_CLOSE
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
  .macro  _OBJ_ANIME  obj_id,adrs
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
  .macro  _OBJ_ANIME_WAIT
  .short  EV_SEQ_OBJ_ANIME_WAIT
  .endm
  
//--------------------------------------------------------------
/**
 *  _MOVE_CODE_GET ����R�[�h�擾(fldmmdl_code.h)
 *  @param ret_wk ����R�[�h���i�[���郏�[�N
 *  @param obj_id ����R�[�h���擾����OBJ ID
 */
//--------------------------------------------------------------
  .macro  _MOVE_CODE_GET  ret_wk,obj_id
  .short  EV_SEQ_MOVE_CODE_GET
  .short  \ret_wk
  .short  \obj_id
  .endm

//--------------------------------------------------------------
/**
 * _OBJ_POS_GET OBJ�̈ʒu�擾
 * @parma obj_id �擾����OBJ ID
 * @param x X���W�i�[��
 * @param z Z���W�i�[��
 */
//--------------------------------------------------------------
   .macro  _OBJ_POS_GET  obj_id,x,z
  .short  EV_SEQ_OBJ_POS_GET
  .short  \obj_id
  .short  \x
  .short  \z
  .endm 

//--------------------------------------------------------------
/**
 * _PLAYER_POS_GET ��l���̈ʒu�擾
 * @param x X���W�i�[��
 * @param z Z���W�i�[��
 */
//--------------------------------------------------------------
  .macro  _PLAYER_POS_GET  x,z
  .short  EV_SEQ_PLAYER_POS_GET
  .short  \x
  .short  \z
  .endm
 
//--------------------------------------------------------------
/**
 * _OBJ_ADD OBJ��ǉ�
 * @param x �\������O���b�hX���W
 * @param z �\������O���b�hZ���W
 * @param dir ���� DIR_UP��
 * @param id ���ʗpOBJ ID
 * @param code �\���R�[�hBOY1��
 * @param move ����R�[�hMV_RND��
 */
//--------------------------------------------------------------
  .macro _OBJ_ADD x,z,dir,id,code,move
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
 * _OBJ_ADD OBJ���폜
 * @param id �폜����OBJ ID
 */
//--------------------------------------------------------------
  .macro _OBJ_DEL id
  .short EV_SEQ_OBJ_DEL
  .short \id
  .endm

//======================================================================
//  ���샂�f���@�C�x���g�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _OBJ_PAUSE_ALL �t�B�[���hOBJ�����~
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _OBJ_PAUSE_ALL
  .short  EV_SEQ_OBJ_PAUSE_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_PAUSE_ALL OBJ��b�C�x���g�p�@�����~
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TALK_OBJ_PAUSE_ALL
  .short  EV_SEQ_TALK_OBJ_PAUSE_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _OBJ_PAUSE_CLEAR_ALL OBJ����ĊJ
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _OBJ_PAUSE_CLEAR_ALL
  .short  EV_SEQ_OBJ_PAUSE_CLEAR_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _TURN_HERO_SITE �b��������OBJ�����@�����֐U�����
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TURN_HERO_SITE
  .short  EV_SEQ_OBJ_TURN
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_START OBJ�ɑ΂��Ęb���|���J�n(��l���ɑ΂��ĐU������L��)
 *  @param none
 */
//--------------------------------------------------------------
  .macro _TALK_OBJ_START
//  _SE_PLAY SEQ_SE_DP_SELECT
//  _SE_PLAY SEQ_SE_DP_SELECT11
//  _SE_PLAY SEQ_SE_DP_TALK2
  _OBJ_PAUSE_ALL
  _TURN_HERO_SITE
  .endm

//--------------------------------------------------------------
/**
 *  _EVENT_START POS,SCENE_CHANGE_LABEL�ɑ΂��ĊJ�n
 *  (TALK_START���g�p����ƁA��b�J�n�̉����Ȃ��Ă��܂��̂ŕ�����)
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EVENT_START
  _OBJ_PAUSE_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _EVENT_END POS,SCENE_CHANGE_LABEL�ɑ΂��ďI��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _EVENT_END
  _OBJ_PAUSE_CLEAR_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_START_TURN_NOT OBJ�ɑ΂��Ęb���|���J�n
 *  (��l���ɑ΂��ĐU���������)
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TALK_OBJ_START_TURN_NOT
//  _SE_PLAY SEQ_SE_DP_SELECT
//  _SE_PLAY SEQ_SE_DP_SELECT11
  _OBJ_PAUSE_ALL
  .endm

//--------------------------------------------------------------
/**
 *  _TALK_OBJ_END OBJ�ɑ΂��Ęb���|���I��
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TALK_OBJ_END
  _OBJ_PAUSE_CLEAR_ALL
  .endm

//======================================================================
// switch�֘A
//======================================================================
//--------------------------------------------------------------
/**
 *  _SWITCH switch �` case �\��
 *  �X�N���v�g���䃏�[�N�Ŋm�ۂ���郏�[�N(SCWK_REG0)���g�p���Ă���̂ŁA
 *  FLAG_CHANGE,INIT_CHANGE�Ȃǂł͎g�p�ł��܂���I
 *  SCENE_CHANGE�͎g�p�ł��܂��B
 *
 *  @param wk �Q�Ƃ��郏�[�N
 *
 *  _SWITCH SCWK_ANSWER
 *  _CASE_JUMP 0,jump0
 *  _CASE_JUMP 1,jump1
 *  _CASE_JUMP 2,jump2
 *  :
 */
//--------------------------------------------------------------
  .macro  _SWITCH wk
  _LDWK  SCWK_REG0,\wk
  .endm

//--------------------------------------------------------------
/**
 *  _CASE_JUMP switch �` case �\��
 *  �X�N���v�g���䃏�[�N�Ŋm�ۂ���郏�[�N(SCWK_REG0)���g�p���Ă���̂ŁA
 *  FLAG_CHANGE,INIT_CHANGE�Ȃǂł͎g�p�ł��܂���I
 *  SCENE_CHANGE�͎g�p�ł��܂��B
 *
 *  @param val _SWITCH�ŎQ�Ƃ�������wk�Ɣ�r����l
 *  @param adr val�ƈ�v���Ă����ۂ̃W�����v��A�h���X
 */
//--------------------------------------------------------------
  .macro  _CASE_JUMP  val,adr
  _CMPVAL  SCWK_REG0,\val
  _IF_JUMP  EQ,\adr
  .endm

#if 0 //wb null
//�X�N���v�g�E�B���h�E�\������B�L�����Z��
  .macro  _CASE_CANCEL  adr
  _CMPVAL  SCWK_REG0,EV_WIN_B_CANCEL
  _IF_JUMP  EQ,\adr
  .endm

//�ҋ@�A�C�R���\��
  .macro  _ADD_WAITICON
  .short  EV_SEQ_WAITICON_ADD
  .endm

//�ҋ@�A�C�R������
  .macro  _DEL_WAITICON
  .short  EV_SEQ_WAITICON_DEL
  .endm
#endif

//======================================================================
//  �͂��A�������@����
//======================================================================
//--------------------------------------------------------------
/**
 *  _YES_NO_WIN �͂��A�������I��
 *  @param ret_wk �͂��A�������I�����ʊi�[��
 */
//--------------------------------------------------------------
  .macro  _YES_NO_WIN ret_wk
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
  .macro  _PLAYER_NAME  idx
  .short  EV_SEQ_PLAYER_NAME
  .byte  \idx
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
  .macro  _TRAINER_ID_GET  wk
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
  .macro  _TRAINER_BTL_SET  tr_id_0,tr_id_1
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
  .macro  _TRAINER_MULTI_BTL_SET  partner_id, tr_id_0, tr_id_1
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
  .macro  _TRAINER_MSG_SET  tr_id,kind_id
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
  .macro  _TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
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
  .macro  _REVENGE_TRAINER_TALK_TYPE_GET  wk1,wk2,wk3
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
  .macro  _TRAINER_TYPE_GET  ret_wk
  .short  EV_SEQ_TRAINER_TYPE_GET
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_BGM_SET �g���[�i�[BGM�Z�b�g
 *  @param tr_id �g���[�i�[ID
 */
//--------------------------------------------------------------
  .macro  _TRAINER_BGM_SET  tr_id
  .short  EV_SEQ_TRAINER_BGM_SET
  .short  \tr_id
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE �g���[�i�[�s�k
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _TRAINER_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _TRAINER_LOSE_CHECK �g���[�i�[�s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[�� 0=�s�k 1=����
 */
//--------------------------------------------------------------
  .macro  _TRAINER_LOSE_CHECK ret_wk
  .short  EV_SEQ_LOSE_CHECK
  .short  \ret_wk
  .endm

//--------------------------------------------------------------
/**
 * _NORMAL_LOSE  �s�k����
 * @param none
 */
//--------------------------------------------------------------
  .macro  _NORMAL_LOSE
  .short  EV_SEQ_LOSE
  .endm

//--------------------------------------------------------------
/**
 *  _LOSE_CHECK �s�k�`�F�b�N
 *  @param ret_wk ���ʊi�[�� 0=�s�k 1=����
 */
//--------------------------------------------------------------
  .macro  _LOSE_CHECK  ret_wk
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
   .macro  _BGM_PLAY no
  .short  EV_SEQ_BGM_PLAY
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_STOP BGM���~�߂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _BGM_STOP
  .short  EV_SEQ_BGM_STOP
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_PLAYER_PAUSE �V�[�P���X���ꎞ��~�܂��͍ĊJ
 *  @param flag (1=��~�A0=�ĊJ)
 */
//--------------------------------------------------------------
  .macro  _BGM_PLAYER_PAUSE flag
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
  .macro  _BGM_FADEOUT vol,frame
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
  .macro  _BGM_FADEIN frame
  .short  EV_SEQ_BGM_FADEIN
  .short  \frame
  .endm

//--------------------------------------------------------------
/**
 *  _BGM_NOW_MAP_PLAY ���݂̃}�b�v��BGM��炷
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _BGM_NOW_MAP_PLAY
  .short  EV_SEQ_BGM_NOW_MAP_PLAY
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
  .macro  _SE_PLAY no
  .short  EV_SEQ_SE_PLAY
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _SE_STOP SE���~�߂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _SE_STOP
  .short  EV_SEQ_SE_STOP
  .endm

//--------------------------------------------------------------
/**
 *  _SE_WAIT SE�I���҂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _SE_WAIT
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
  .macro  _ME_PLAY no
  .short  EV_SEQ_ME_PLAY
  .short  \no
  .endm

//--------------------------------------------------------------
/**
 *  _ME_WAIT ME�I���҂�
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _ME_WAIT
  .short  EV_SEQ_ME_WAIT
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
   .macro  _BMPMENU_INIT x,y,cursor,cancel,ret_wk
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
   .macro  _BMPMENU_INIT_EX x,y,cursor,cancel,ret_wk
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
  .macro  _BMPMENU_MAKE_LIST msg_id,param
  .short  EV_SEQ_BMPMENU_MAKE_LIST16
  .short  \msg_id
  .short  \param
  .endm

//--------------------------------------------------------------
/**
 *  _BMPMENU_START BMP���j���[�@�J�n
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _BMPMENU_START
  .short  EV_SEQ_BMPMENU_START
  .endm

//======================================================================
//  ��ʃt�F�[�h
//======================================================================
//--------------------------------------------------------------
//  �t�F�[�h���[�h(fade.h)
//--------------------------------------------------------------
#define DISP_FADE_BLACKOUT_MAIN (0x01) //���C����ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_BLACKOUT_SUB  (0x02) //�T�u��ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_BLACKOUT (0x03) //����ʃt�F�[�h �u���b�N�A�E�g
#define DISP_FADE_WHITEOUT_MAIN (0x04) //���C����ʃt�F�[�h �z���C�g�A�E�g
#define DISP_FADE_WHITEOUT_SUB  (0x08) //�T�u��ʃt�F�[�h �z���C�g�A�E�g
#define DISP_FADE_WHITEOUT (0x0c) //����ʃt�F�[�h �z���C�g�A�E�g

//--------------------------------------------------------------
/**
 *  _DISP_FADE_START ��ʃt�F�[�h�X�^�[�g
 *  @param mode ��ʃt�F�[�h���[�h 
 *  @param start_evy �����P�x(0=���̐F�`16=�w��P�x�j
 *  @param end_evy �I���P�x(0=���̐F�`16=�w��P�x)
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
  .macro _DISP_FADE_START mode,start_evy,end_evy,speed
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
  .macro _DISP_FADE_END_CHECK
  .short EV_SEQ_DISP_FADE_CHECK
  .endm

//--------------------------------------------------------------
/**
 *  _WHITE_OUT �z���C�g�A�E�g
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
  .macro _WHITE_OUT speed
  _DISP_FADE_START DISP_FADE_WHITEOUT_MAIN,0,16,\speed
  .endm

//--------------------------------------------------------------
/**
 *  _WHITE_IN �z���C�g�C��
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
  .macro _WHITE_IN speed
  _DISP_FADE_START DISP_FADE_WHITEOUT_MAIN,16,0,\speed
  .endm

//--------------------------------------------------------------
/**
 *  _BLACK_OUT �u���b�N�A�E�g
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
  .macro _BLACK_OUT speed
  _DISP_FADE_START DISP_FADE_BLACKOUT_MAIN,0,16,\speed
  .endm

//--------------------------------------------------------------
/**
 *  _WHITE_IN �u���b�N�C��
 *  @param speed �t�F�[�h�X�s�[�h 0�`
 */
//--------------------------------------------------------------
  .macro _BLACK_IN speed
  _DISP_FADE_START DISP_FADE_BLACKOUT_MAIN,16,0,\speed
  .endm

//======================================================================
//  ���̑�
//======================================================================
//--------------------------------------------------------------
/**
 *  _CHG_LANGID ����ID�؂�ւ� �Ђ炪��<->����
 *  @param none
 */
//--------------------------------------------------------------
  .macro  _CHG_LANGID
  .short  EV_SEQ_CHG_LANGID
  .endm

//--------------------------------------------------------------
/*
 *  _GET_RND �����_�����擾
 *  @param ret_wk �����i�[��
 *  @param num ��������l
 */
//--------------------------------------------------------------
   .macro  _GET_RND ret_wk,num
  .short  EV_SEQ_GET_RND
  .short  \ret_wk
  .short  \num
  .endm
