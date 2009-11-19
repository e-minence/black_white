//======================================================================
/**
 * @file    easy_event_def.h
 * @brief   �ȈՃC�x���g��`�t�@�C��
 * @date    2009.09.08
 * @author  tamada GAMEFREAK inc.
 *
 *
 * ���ۂɃX�N���v�g���Ŏg�p�����`��scr_seq_def.h�ɋL�q����Ă���B
 * ������̒�`�𒼐ڌĂяo���Ȃ����ƁI�I
 */
//======================================================================

//--------------------------------------------------------------
/**
 * ���|�[�g�Ăяo��
 *
 * @param ret_wk      ���ʂ��󂯎�邽�߂̃��[�N
 */
//--------------------------------------------------------------
  .macro _ASM_REPORT_EVENT_CALL ret_wk
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _PUSH_WORK  SCWK_PARAM2
  _PUSH_WORK  SCWK_PARAM3
  _PUSH_WORK  SCWK_PARAM4
  _PUSH_WORK  SCWK_PARAM5


  _CHG_COMMON_SCR SCRID_REPORT_EVENT_CALL
  _ASM_LDWKVAL  \ret_wk, SCWK_PARAM0


  _POP_WORK   SCWK_PARAM5
  _POP_WORK   SCWK_PARAM4
  _POP_WORK   SCWK_PARAM3
  _POP_WORK   SCWK_PARAM2
  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0

  .endm

//--------------------------------------------------------------
/**
 * �t�B�[���h�ʐM�ؒf�C�x���g�Ăяo��
 *
 * @param ret_wk      ���ʂ��󂯎�邽�߂̃��[�N
 */
//--------------------------------------------------------------
  .macro _ASM_FIELD_COMM_EXIT_EVENT_CALL ret_wk
  _PUSH_WORK  SCWK_PARAM0

  _CHG_COMMON_SCR SCRID_FIELD_COMM_EXIT_EVENT_CALL
  _ASM_LDWKVAL  \ret_wk, SCWK_PARAM0

  _POP_WORK   SCWK_PARAM0
  .endm

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
 * SCWK_PARAM0�`SCWK_PARAM6���g�p���Ĉ�����n���Ă���
 */
//--------------------------------------------------------------
  .macro  _ASM_EASY_TALK_ITEM_EVENT itemno, num, flag, before_msg, finish_msg, after_msg
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _PUSH_WORK  SCWK_PARAM2
  _PUSH_WORK  SCWK_PARAM3
  _PUSH_WORK  SCWK_PARAM4
  _PUSH_WORK  SCWK_PARAM5
  _PUSH_WORK  SCWK_PARAM6

  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _ASM_LDWKVAL  SCWK_PARAM2, \flag
  _ASM_LDWKVAL  SCWK_PARAM3, \before_msg
  _ASM_LDWKVAL  SCWK_PARAM4, \finish_msg
  _ASM_LDWKVAL  SCWK_PARAM5, \after_msg
  _ASM_GET_NOW_MSG_ARCID  SCWK_PARAM6

  _CHG_COMMON_SCR SCRID_EASY_ITEM_EVENT
  
  _POP_WORK  SCWK_PARAM6
  _POP_WORK  SCWK_PARAM5
  _POP_WORK  SCWK_PARAM4
  _POP_WORK  SCWK_PARAM3
  _POP_WORK  SCWK_PARAM2
  _POP_WORK  SCWK_PARAM1
  _POP_WORK  SCWK_PARAM0
  .endm

//--------------------------------------------------------------
// �C�x���g����C�x���g�u���v�҂�����
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_KEYWAIT itemno, num
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_KEYWAIT
  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm


//--------------------------------------------------------------
// �C�x���g����C�x���g�u���v�҂��Ȃ�
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_NOWAIT  itemno, num
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_NOWAIT
  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm

//======================================================================
//   �ȈՃ��b�Z�[�W�R�}���h
//======================================================================
//--------------------------------------------------------------
/**
 * �ȈՐ����o�����b�Z�[�W�\�� �b���|��OBJ�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
  .macro _ASM_EASY_BALLOONWIN_TALKOBJ_MSG msg_id
  _ASM_TALK_START_SE_PLAY
  _ASM_TURN_HERO_SITE   
  _ASM_BALLOONWIN_TALKOBJ_OPEN \msg_id 
  _ASM_LAST_KEYWAIT      
  _ASM_BALLOONWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 * �ȈՃ��b�Z�[�W�\�� BG�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
  .macro  _ASM_EASY_MSG msg_id
  _ASM_TALK_START_SE_PLAY
  _ASM_SYSWIN_OPEN WIN_DOWN
  _ASM_TALKMSG_ALLPUT \msg_id
  _ASM_LAST_KEYWAIT
  _ASM_TALKWIN_CLOSE
  .endm


//--------------------------------------------------------------
/**
 * �ȈՖ������b�Z�[�W�R�}���h
 *
 * @param monsno
 * @param msg_id
 */
//--------------------------------------------------------------
    .macro  _ASM_EASY_TALK_POKEVOICE monsno, msg_id
    _ASM_TALK_START_SE_PLAY
    _ASM_TURN_HERO_SITE   
    _ASM_VOICE_PLAY \monsno
    _ASM_BALLOONWIN_TALKOBJ_OPEN \msg_id 
    _ASM_VOICE_WAIT
    _ASM_LAST_KEYWAIT
    _ASM_BALLOONWIN_CLOSE
    .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_OPEN_MF
 *  @brief  �����o���E�B���h�E�`��@�o�[�W������
 *  @param msg_id_m �\�����郁�b�Z�[�WID�A�z���C�g��
 *  @param msg_id_f �\�����郁�b�Z�[�WID�A�u���b�N��
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu WIN_UP,WIN_DONW,WIN_NONE
 */
//--------------------------------------------------------------
  .macro _ASM_BALLOONWIN_OBJMSG_OPEN_MF msg_id_m, msg_id_f, obj_id, pos
  _PUSH_WORK SCWK_ANSWER
  _ASM_GET_MY_SEX SCWK_ANSWER
	IF $SCWK_ANSWER == PM_MALE THEN
    _ASM_BALLOONWIN_OBJMSG_OPEN_POS msg_id_m, obj_id, pos
  ELSE
    _ASM_BALLOONWIN_OBJMSG_OPEN_POS msg_id_f, obj_id, pos
  ENDIF
  _POP_WORK SCWK_ANSWER
  .endm

//--------------------------------------------------------------
/**
 *  @def  _BALLOONWIN_OBJMSG_OPEN_WB
 *  @brief  �����o���E�B���h�E�`��@�o�[�W������
 *  @param msg_id_w �\�����郁�b�Z�[�WID�A�z���C�g��
 *  @param msg_id_b �\�����郁�b�Z�[�WID�A�u���b�N��
 *  @param obj_id �����o�����o���Ώ�OBJ ID
 *  @param pos �����o���E�B���h�E�ʒu WIN_UP,WIN_DONW,WIN_NONE
 */
//--------------------------------------------------------------
  .macro _ASM_BALLOONWIN_OBJMSG_OPEN_WB msg_id_w, msg_id_b, obj_id, pos
  _PUSH_WORK SCWK_ANSWER
  _ASM_GET_ROM_VERSION SCWK_ANSWER
	IF $SCWK_ANSWER == VERSION_WHITE THEN
    _ASM_BALLOONWIN_OBJMSG_OPEN_POS msg_id_w, obj_id, pos
  ELSE
    _ASM_BALLOONWIN_OBJMSG_OPEN_POS msg_id_b, obj_id, pos
  ENDIF
  _POP_WORK SCWK_ANSWER
  .endm

//======================================================================
//  �V���b�v
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �V���b�v�Ăяo��
 *
 * @param shop_id      �V���b�vID
 */
//--------------------------------------------------------------
  .macro _ASM_SHOP_CALL shop_id,greeting_f
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1

  _ASM_LDWKVAL  SCWK_PARAM0, \shop_id
  _ASM_LDWKVAL  SCWK_PARAM1, \greeting_f
  _CHG_COMMON_SCR SCRID_SHOP_COUNTER

  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm


//======================================================================
//    �ȈՃA�v������
//======================================================================
//--------------------------------------------------------------
/**
 * @brief CGEAR�N���f���i���j
 */
//--------------------------------------------------------------
  .macro  _ASM_CGEAR_ON_DEMO
  _ASM_DISP_FADE_START  DISP_FADE_WHITEOUT, 0, 16, DISP_FADE_SPEED
  _ASM_DISP_FADE_END_CHECK
  _ASM_FIELD_CLOSE

  _ASM_SET_CGEAR_FLAG TRUE

  _ASM_FIELD_OPEN
  _ASM_DISP_FADE_START  DISP_FADE_WHITEOUT, 16, 0, DISP_FADE_SPEED
  _ASM_DISP_FADE_END_CHECK
  .endm


