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
 * ���|�[�g�Ăяo��( ���C�����X�Z�[�u���[�h�̃`�F�b�N�L )
 *
 * @param ret_wk      ���ʂ��󂯎�邽�߂̃��[�N
 */
//--------------------------------------------------------------
  .macro _ASM_REPORT_EVENT_CALL_WIRELESS ret_wk  
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _PUSH_WORK  SCWK_PARAM2
  _PUSH_WORK  SCWK_PARAM3
  _PUSH_WORK  SCWK_PARAM4
  _PUSH_WORK  SCWK_PARAM5


  _CHG_COMMON_SCR SCRID_REPORT_EVENT_CALL_WIRELESS
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
// �B���C�x���g����C�x���g�u���v�҂�����
//--------------------------------------------------------------
  .macro  _ASM_ITEM_EVENT_HIDEITEM itemno, num, flag_no
  _ASM_LDWKVAL  SCWK_PARAM0, \itemno
  _ASM_LDWKVAL  SCWK_PARAM1, \num
  _ASM_LDWKVAL  SCWK_PARAM2, \flag_no
  _CHG_COMMON_SCR SCRID_ITEM_EVENT_HIDEITEM
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
 * @brief 3D�f���ȈՌĂяo��
 */
//--------------------------------------------------------------
  .macro  _ASM_CALL_3D_DEMO demo_no, scene_id
  _ASM_FIELD_CLOSE
  _ASM_DEMO_SCENE \demo_no, \scene_id
  _ASM_FIELD_OPEN
  .endm

//--------------------------------------------------------------
/**
 * @brief �e���r�g�����V�[�o�[�f���ȈՌĂяo��
 */
//--------------------------------------------------------------
  .macro  _ASM_CALL_TVT_DEMO demo_no
  _ASM_FIELD_CLOSE
  _ASM_TVT_DEMO \demo_no
  _ASM_FIELD_OPEN
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

//======================================================================
//  �\���󁕘H���}�A�v���Ăяo��
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �\���󁕘H���}�A�v���Ăяo��
 *
 * @param demo_id
 * @param pattern
 */
//--------------------------------------------------------------
  .macro _ASM_ZUKAN_AWARD_PROC_CALL demo_id,pattern
  
  _MAP_FADE_BLACK_OUT()
  _MAP_FADE_END_CHECK()
  _FIELD_CLOSE()
  
  _ASM_CALL_ZUKAN_AWARD \demo_id,\pattern

  _FIELD_OPEN()
  _MAP_FADE_BLACK_IN()
  _MAP_FADE_END_CHECK()
 
  .endm

//======================================================================
//  
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �ŏ��̂R�̑I��
 *
 * @param ret_wk
 */
//--------------------------------------------------------------
  .macro _ASM_FIRST_POKE_SELECT_CALL ret_wk
  
  _ASM_FIELD_CLOSE
  _ASM_CALL_FIRST_POKE_SELECT \ret_wk
  _ASM_FIELD_OPEN
 
  .endm

//======================================================================
//�ϗ��ԃg���[�i�[�֘A  
//======================================================================
//--------------------------------------------------------------
/**
 * @brief �ϗ��ԃg���[�i�[ �o�^ID�擾(0�`7�̓o�^ID tr_id�Ƃ͕�)
 *
 * @param ret_wk
 */
//--------------------------------------------------------------
  .macro _ASM_GET_WHEEL_TRAINER_ENTRY_ID ret_wk
  
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  
  _GET_SEASON_ID( SCWK_PARAM0 )
  _GET_MY_SEX( SCWK_PARAM1 )

  _ASM_LDWKVAL  \ret_wk, SCWK_PARAM0
  _ASM_MUL_WK   \ret_wk,2
  _ASM_ADD_WK   \ret_wk,SCWK_PARAM1
  
  _POP_WORK  SCWK_PARAM1
  _POP_WORK  SCWK_PARAM0
  .endm

//--------------------------------------------------------------
/**
 * @brief �ϗ��ԃg���[�i�[���b�Z�[�W
 *
 * @param ret_wk
 */
//--------------------------------------------------------------
  .macro _ASM_WHEEL_TRAINER_MSG msg_id, id, f_2nd
  
  _PUSH_WORK  SCWK_PARAM0
  _PUSH_WORK  SCWK_PARAM1
  _PUSH_WORK  SCWK_PARAM2

  _ASM_LDWKVAL  SCWK_PARAM0, \msg_id
  _ASM_LDWKVAL  SCWK_PARAM1, \id
  _ASM_LDWKVAL  SCWK_PARAM2, \f_2nd
  
  _ASM_MUL_WK SCWK_PARAM1,10
  _ASM_MUL_WK SCWK_PARAM2,5
  _ASM_ADD_WK SCWK_PARAM0,SCWK_PARAM1
  _ASM_ADD_WK SCWK_PARAM0,SCWK_PARAM2
 
  _ASM_PLAYER_NAME  0
  _ASM_BALLOONWIN_TALKOBJ_MSG SCWK_PARAM0
 
  _POP_WORK  SCWK_PARAM2
  _POP_WORK  SCWK_PARAM1
  _POP_WORK  SCWK_PARAM0
  .endm

