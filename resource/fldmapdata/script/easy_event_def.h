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

//--------------------------------------------------------------
// �C�x���g����C�x���g�u���v�҂�����
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_FIELD itemno, num
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_FIELD
  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm

//--------------------------------------------------------------
/**
 * @brief �C�x���g���萬���C�x���g�u���v�҂�����
 */
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_SUCCESS_KEYWAIT itemno, num
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_SUCCESS_KEYWAIT
  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm

//--------------------------------------------------------------
/**
 * @brief �C�x���g���萬���C�x���g�u���v�҂��Ȃ�
 */
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_SUCCESS_NOWAIT itemno, num
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_SUCCESS_NOWAIT
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
  _ASM_BALLOONWIN_TALKOBJ_MSG \msg_id 
  _ASM_LAST_KEYWAIT      
  _ASM_BALLOONWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 * �ȈՃ��b�Z�[�W�\�� BG�p
 * @param msg_id �\�����郁�b�Z�[�WID
 */
//--------------------------------------------------------------
  .macro _ASM_EASY_MSG msg_id
  _ASM_TALK_START_SE_PLAY
  _ASM_SYSWIN_MSG_ALLPUT \msg_id,\WIN_DOWN
  _ASM_LAST_KEYWAIT
  _ASM_SYSWIN_CLOSE
  .endm

//--------------------------------------------------------------
/**
 * �Ȉ�BG�E�B���h�E���b�Z�[�W�\��
 * @param msg_id �\�����郁�b�Z�[�WID
 * @param bg_type �\������^�C�v TYPE_INFO,TYPE_TOWN,TYPE_POST,TYPE_ROAD
 */
//--------------------------------------------------------------
  .macro _ASM_EASY_BGWIN_MSG msg_id, bg_type
  _ASM_TALK_START_SE_PLAY
  _ASM_BGWIN_MSG \msg_id, \bg_type
  _ASM_BGWIN_CLOSE
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
    _ASM_BALLOONWIN_TALKOBJ_MSG \msg_id 
    _ASM_VOICE_WAIT
    _ASM_LAST_KEYWAIT
    _ASM_BALLOONWIN_CLOSE
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
  _ASM_MAP_FADE_WHITE_OUT
  _ASM_MAP_FADE_END_CHECK
  _ASM_FIELD_CLOSE

  _ASM_SET_CGEAR_FLAG TRUE

  _ASM_FIELD_OPEN
  _ASM_MAP_FADE_WHITE_IN
  _ASM_MAP_FADE_END_CHECK
  .endm


//--------------------------------------------------------------
/**
 * @brief 3D�f���ȈՌĂяo��
 */
//--------------------------------------------------------------
  .macro  _ASM_CALL_3D_DEMO demo_no
  //_ASM_MAP_FADE_BLACK_OUT
  //_ASM_MAP_FADE_END_CHECK
  _ASM_FIELD_CLOSE
  _ASM_DEMO_SCENE \demo_no
  _ASM_FIELD_OPEN
  //_ASM_MAP_FADE_BLACK_IN
  //_ASM_MAP_FADE_END_CHECK
  .endm

//--------------------------------------------------------------
/**
 * @brief �e���r�g�����V�[�o�[�f���ȈՌĂяo��
 */
//--------------------------------------------------------------
  .macro  _ASM_CALL_TVT_DEMO demo_no
  _ASM_MAP_FADE_BLACK_OUT
  _ASM_MAP_FADE_END_CHECK
  _ASM_FIELD_CLOSE
  _ASM_TVT_DEMO \demo_no
  _ASM_FIELD_OPEN
  _ASM_MAP_FADE_BLACK_IN
  _ASM_MAP_FADE_END_CHECK
  .endm

//--------------------------------------------------------------
/**
 *  �ΐ��t�@�p�[�e�B�I���E�B���h�E�\�� 
 *
 * @param ret_wk      ���ʂ��󂯎�邽�߂̃��[�N
 */
//--------------------------------------------------------------
  .macro _ASM_BTL_UTIL_PARTY_SELECT_CALL regulation, ret_wk
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1

  _ASM_LDWKVAL  SCWK_PARAM1, \regulation
  _CHG_COMMON_SCR SCRID_BTL_UTIL_PARTY_SELECT_CALL
  _ASM_LDWKVAL  \ret_wk, SCWK_PARAM0

  _POP_WORK   SCWK_PARAM1
  _POP_WORK   SCWK_PARAM0
  .endm

//--------------------------------------------------------------
/**
 *  �ΐ��t�@���M�����[�V�����ɍ����p�[�e�B��������Ȃ������Ƃ��̃��b�Z�[�W�\�� 
 *
 * @param ret_wk      ���ʂ��󂯎�邽�߂̃��[�N
 */
//--------------------------------------------------------------
  .macro _ASM_BTL_UTIL_PARTY_REGULATION_NG_MSG_CALL regulation
  _PUSH_WORK  SCWK_PARAM0
  
  _ASM_LDWKVAL  SCWK_PARAM0, \regulation
  _CHG_COMMON_SCR SCRID_BTL_UTIL_PARTY_REGULATION_NG_MSG_CALL
  
  _POP_WORK   SCWK_PARAM0
  .endm

