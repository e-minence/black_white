//======================================================================
/**
 * @file  bsubway_scr_def.h
 * @bfief  �o�g���T�u�E�F�C�@�X�N���v�g�֘A�萔��`
 * @author kagaya
 * @date  07.05.28
 * @note �v���`�i���ڐA
 */
//======================================================================
#ifndef __SUBWAY_SCR_DEF_H__
#define __SUBWAY_SCR_DEF_H__

//======================================================================
//  define
//======================================================================
#ifdef DEBUG_ONLY_FOR_kagaya
//#define DEBUG_BSW_CLEAR_1 //��`��1��퓬�݂̂ŃN���A
//#define DEBUG_BSW_COMM_MULTI_BTL_SKIP //��`�ŒʐM�}���`�o�g�����X�L�b�v
//#define DEBUG_BSW_BTL_SKIP //��`�Ő퓬�X�L�b�v
//#define DEBUG_BSW_NORMAL_CLEAR //��`�ő��m�[�}���N���A
//#define DEBUG_BSW_FORCE_BTL_WIN //��`�ŋ����������
//#define DEBUG_BSW_COMM_IGNORE_POKE_OVERLAP //��`�ŒʐM�|�P������薳��
//#define DEBUG_BSW_REGU_NONE //��`�Ń��M������
#endif

#define BSUBWAY_SCRWORK_MAGIC  (0x12345678) ///<�}�W�b�N�i���o�[

#define BSWAY_NULL_POKE  (0xFF)
#define BSWAY_NULL_TRAINER  (0xFFFF)
#define BSWAY_NULL_PARAM  (0)

///�o�g���T�u�E�F�C���������[�h
#define BSWAY_PLAY_NEW (0)
#define BSWAY_PLAY_CONTINUE (1)

///�o�g���T�u�E�F�C�@�N���A���E���h��
#define BSWAY_CLEAR_WINCNT  (7)

//�o�g���T�u�E�F�C�@�A���L�^���E���h��
#define BSWAY_20_RENSHOU_CNT      (20)
#define BSWAY_50_RENSHOU_CNT      (50)
#define BSWAY_100_RENSHOU_CNT    (100)

#define LEADER_SET_1ST  (20+1)  //21�l�ڂ̂Ƃ����݂̘A������20
#define LEADER_SET_2ND  (48+1)  //49�l�ڂ̂Ƃ����݂̘A������48

#define BSWAY_PARTNER_NUM (3)  ///<5�l�O�̐�
#define BSWAY_PARTNER_DATA_START_NO_MAN (300)  ///<5�l�O�f�[�^�̃X�^�[�gindex
#define BSWAY_PARTNER_DATA_START_NO_WOMAN (303) ///<5�l�O�f�[�^�̃X�^�[�gindex

///�o�g���T�u�E�F�C�v���C���[�h�X�N���v�g�p��` BSWAY_PLAYMODE�Ɠ���
#define BSWAY_MODE_SINGLE  (0)
#define BSWAY_MODE_DOUBLE  (1)
#define BSWAY_MODE_MULTI  (2)
#define BSWAY_MODE_COMM_MULTI (3)
#define BSWAY_MODE_WIFI (4)
#define BSWAY_MODE_S_SINGLE (5)
#define BSWAY_MODE_S_DOUBLE (6)
#define BSWAY_MODE_S_MULTI (7)
#define BSWAY_MODE_S_COMM_MULTI (8)
#define BSWAY_MODE_MAX (9)
#define BSWAY_MODE_NULL (10)

///�o�g���T�u�E�F�C�@�p�[�g�i�[�R�[�h
#define BSWAY_PTCODE_OBJ    (2)
#define BSWAY_PTCODE_MINE2  (1)
#define BSWAY_PTCODE_MINE  (0)

///�o�g���T�u�E�F�C WIFI�ڑ����[�h��`
#define BSWAY_WIFI_DL_PLAY    (0)
#define BSWAY_WIFI_UP_PLAY    (1)
#define BSWAY_WIFI_DL_LEADER    (2)

///�ʐM�}���`����M�o�b�t�@�T�C�Y
#define BSWAY_SIO_BUF_LEN  (70)

//���炦��O�b�Y
#define BSWAY_GOODS_COPPER  (GOODS_UG_TROPH_14)
#define BSWAY_GOODS_SILVER  (GOODS_UG_TROPH_15)
#define BSWAY_GOODS_GOLD    (GOODS_UG_TROPH_16)

//���炦��o�g���|�C���g
#define BTLPOINT_VAL_BSUBWAY_LEADER  (20)
#define BTLPOINT_VAL_BSUBWAY_WIFI1  (3)      //�_�E�����[�h
#define BTLPOINT_VAL_BSUBWAY_WIFI2  (4)
#define BTLPOINT_VAL_BSUBWAY_WIFI3  (5)
#define BTLPOINT_VAL_BSUBWAY_WIFI4  (6)
#define BTLPOINT_VAL_BSUBWAY_WIFI5  (7)
#define BTLPOINT_VAL_BSUBWAY_WIFI6  (8)
#define BTLPOINT_VAL_BSUBWAY_WIFI7  (9)
#define BTLPOINT_VAL_BSUBWAY_WIFI8  (10)
#define BTLPOINT_VAL_BSUBWAY_WIFI9  (10)
#define BTLPOINT_VAL_BSUBWAY_WIFI10  (10)
#define BTLPOINT_VAL_BSUBWAY_STAGE1  (3)      //�V���O���A�_�u���AAI�}���`
#define BTLPOINT_VAL_BSUBWAY_STAGE2  (3)
#define BTLPOINT_VAL_BSUBWAY_STAGE3  (4)
#define BTLPOINT_VAL_BSUBWAY_STAGE4  (4)
#define BTLPOINT_VAL_BSUBWAY_STAGE5  (5)
#define BTLPOINT_VAL_BSUBWAY_STAGE6  (5)
#define BTLPOINT_VAL_BSUBWAY_STAGE7  (7) //�����̓v���O�����I��DP����ԈႢ
#define BTLPOINT_VAL_BSUBWAY_STAGE8  (7)
#define BTLPOINT_VAL_BSUBWAY_COMM1  (8)      //�}���`�AWIFI
#define BTLPOINT_VAL_BSUBWAY_COMM2  (9)
#define BTLPOINT_VAL_BSUBWAY_COMM3  (11)
#define BTLPOINT_VAL_BSUBWAY_COMM4  (12)
#define BTLPOINT_VAL_BSUBWAY_COMM5  (14)
#define BTLPOINT_VAL_BSUBWAY_COMM6  (15)
#define BTLPOINT_VAL_BSUBWAY_COMM7  (18)
#define BTLPOINT_VAL_BSUBWAY_COMM8  (18)

///�X�R�A�t���O�A�N�Z�XID
#define BSWAY_SF_SILVER  (0)
#define BSWAY_SF_GOLD  (1)
#define BSWAY_SF_S50    (2)
#define BSWAY_SF_S100  (3)
#define BSWAY_SF_D50    (4)
#define BSWAY_SF_M50    (5)
#define BSWAY_SF_CM50  (6)
#define BSWAY_SF_W50    (7)

//--------------------------------------------------------------
//  �g���[�i�[�f�[�^�A�|�P�����f�[�^
//--------------------------------------------------------------
#define BSW_TR_ARCDATANO_ORG (1) ///<�g���[�i�[�f�[�^�A�[�J�C�u�J�n�ԍ�
#define BSW_PM_ARCDATANO_ORG (1) ///<�|�P�����f�[�^�A�[�J�C�u�J�n�ԍ�

//�g���[�i�[�f�[�^�ԍ�
#define BSW_TR_DATANO_SINGLE (306) //�V���O��
#define BSW_TR_DATANO_S_SINGLE (307) //�X�[�p�[�V���O��
#define BSW_TR_DATANO_DOUBLE (308) //�_�u��
#define BSW_TR_DATANO_S_DOUBLE (309) //�X�[�p�[�_�u��
#define BSW_TR_DATANO_MULTI0 (310) //�}���`0
#define BSW_TR_DATANO_MULTI1 (311) //�}���`1
#define BSW_TR_DATANO_S_MULTI0 (312) //�X�[�p�[�}���`0
#define BSW_TR_DATANO_S_MULTI1 (313) //�X�[�p�[�}���`1

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�V�[������
//--------------------------------------------------------------
//��t����
#define BSWAY_SCENE_RECEIPT_NON (0) //��������
#define BSWAY_SCENE_RECEIPT_CONTINUE (1) //�R���e�j���[
#define BSWAY_SCENE_RECEIPT_CANCEL (2) //�L�����Z��
#define BSWAY_SCENE_RECEIPT_AFTER (3) //�Q�[���N���A
#define BSWAY_SCENE_RECEIPT_ERROR (4) //�G���[

//��ԓ�����
#define BSWAY_SCENE_TRAIN_NON (0) //��������
#define BSWAY_SCENE_TRAIN_FIRST (1) //��ԏ���
#define BSWAY_SCENE_TRAIN_CONTINUE (2) //��Ԓ�

//�z�[������
#define BSWAY_SCENE_HOME_NON (0) //��������
#define BSWAY_SCENE_HOME_GAME_CLEAR (1) //�Q�[���N���A����
#define BSWAY_SCENE_HOME_STOPOVER (2) //�r���w
#define BSWAY_SCENE_HOME_TERMINUS (3) //�z�[���I�_

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�R�}���h�c�[��ID
/// ���[�N�ˑ������R�}���h
//--------------------------------------------------------------
#define BSWTOOL_GET_ZONE_PLAY_MODE (0)
#define BSWTOOL_SYSTEM_RESET (1)
#define BSWTOOL_CLEAR_PLAY_DATA (2)
#define BSWTOOL_IS_SAVE_DATA_ENABLE (3)
#define BSWTOOL_PUSH_NOW_LOCATION (4)
#define BSWTOOL_POP_NOW_LOCATION (5)
#define BSWTOOL_GET_RENSHOU_RECORD (6)
#define BSWTOOL_SET_NG_SCORE (7)
#define BSWTOOL_GET_NOW_ROUND (8)
#define BSWTOOL_INC_ROUND (9)
#define BSWTOOL_GET_RENSHOU_CNT (10)
#define BSWTOOL_GET_NEXT_ROUND (11)
#define BSWTOOL_GET_MINE_OBJ (12)
#define BSWTOOL_OBJ_VANISH (13)
#define BSWTOOL_GET_BOSS_CLEAR_FLAG (14)
#define BSWTOOL_SET_BOSS_CLEAR_FLAG (15)
#define BSWTOOL_GET_SUPPORT_ENCOUNT_END (16)
#define BSWTOOL_SET_SUPPORT_ENCOUNT_END (17)
#define BSWTOOL_SET_TRAIN (18)
#define BSWTOOL_SET_TRAIN_ANM (19)
#define BSWTOOL_SET_TRAIN_VANISH (20)
#define BSWTOOL_GET_DATA_PLAY_MODE (21)
#define BSWTOOL_CHK_MODE_MULTI (22)
#define BSWTOOL_OBJ_HEIGHT_OFF (23)
#define BSWTOOL_OBJ_HEIGHT_ON (24)
#define BSWTOOL_CHK_EXIST_OBJ (25)
#define BSWTOOL_GET_HOME_NPC_MSGID (26)
#define BSWTOOL_GET_WIFI_RANK (27)
#define BSWTOOL_UP_WIFI_RANK (28)
#define BSWTOOL_DOWN_WIFI_RANK (29)
#define BSWTOOL_GET_STAGE_NO (30)
#define BSWTOOL_GET_OBJCODE_PARTNER (31)
#define BSWTOOL_CHK_BTL_BOX_IN (32)
#define BSWTOOL_CHK_DEBUG_ROM (33)
#define BSWTOOL_SET_TTOWN_TRAIN (34)
#define BSWTOOL_SET_OBJ_DIR (35)
#define BSWTOOL_OBJ_PAUSE (36)
#define BSWTOOL_WIFI_HOME_NPC_MSG (37)
#define BSWTOOL_CHK_EXIST_STAGE (38)
#define BSWTOOL_END_NO (39)

#define BSWTOOL_WIFI_START_NO (50)
#define BSWTOOL_WIFI_SET_UPLOAD_FLAG (BSWTOOL_WIFI_START_NO+0)
#define BSWTOOL_WIFI_GET_UPLOAD_FLAG (BSWTOOL_WIFI_START_NO+1)
#define BSWTOOL_WIFI_CONNECT (BSWTOOL_WIFI_START_NO+2)
#define BSWTOOL_WIFI_UPLOAD_SCORE (BSWTOOL_WIFI_START_NO+3)
#define BSWTOOL_WIFI_DOWNLOAD_GAMEDATA (BSWTOOL_WIFI_START_NO+4)
#define BSWTOOL_WIFI_DOWNLOAD_SCDATA (BSWTOOL_WIFI_START_NO+5)
#define BSWTOOL_WIFI_GET_RANK (BSWTOOL_WIFI_START_NO+6)
#define BSWTOOL_WIFI_CHK_DL_DATA (BSWTOOL_WIFI_START_NO+7)
#define BSWTOOL_WIFI_SET_SEL_DL_BTL (BSWTOOL_WIFI_START_NO+8)
#define BSWTOOL_WIFI_CHK_SEL_DL_BTL (BSWTOOL_WIFI_START_NO+9)
#define BSWTOOL_WIFI_CHK_DL_SCDATA (BSWTOOL_WIFI_START_NO+10)
#define BSWTOOL_WIFI_EV_READ_SCDATA (BSWTOOL_WIFI_START_NO+11)
#define BSWTOOL_WIFI_GET_DL_SCDATA_RANK (BSWTOOL_WIFI_START_NO+12)
#define BSWTOOL_WIFI_GET_DL_SCDATA_ROOM (BSWTOOL_WIFI_START_NO+13)
#define BSWTOOL_WIFI_END_NO (BSWTOOL_WIFI_START_NO+14)

#define BSWTOOL_DEBUG_START_NO (80)
#define BSWTOOL_DEBUG_CHK_FLAG (BSWTOOL_DEBUG_START_NO+0)
#define BSWTOOL_DEBUG_PRINT_NO (BSWTOOL_DEBUG_START_NO+1)
#define BSWTOOL_DEBUG_END_NO (BSWTOOL_DEBUG_START_NO+2)

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�R�}���h�c�[��ID
/// ���[�N�ˑ��R�}���h
//--------------------------------------------------------------
#define BSWSUB_START_NO (100)
#define BSWSUB_SELECT_POKE (BSWSUB_START_NO+0)
#define BSWSUB_GET_ENTRY_POKE (BSWSUB_START_NO+1)
#define BSWSUB_CHK_ENTRY_POKE (BSWSUB_START_NO+2)
#define BSWSUB_IS_CLEAR (BSWSUB_START_NO+3)
#define BSWSUB_SET_LOSE_SCORE (BSWSUB_START_NO+4)
#define BSWSUB_SET_CLEAR_SCORE (BSWSUB_START_NO+5)
#define BSWSUB_SAVE_REST_PLAY_DATA (BSWSUB_START_NO+6)
#define BSWSUB_CHOICE_BTL_PARTNER (BSWSUB_START_NO+7)
#define BSWSUB_GET_ENEMY_OBJ (BSWSUB_START_NO+8)
#define BSWSUB_LOCAL_BTL_CALL (BSWSUB_START_NO+9)
#define BSWSUB_GET_PLAY_MODE (BSWSUB_START_NO+10)
#define BSWSUB_SET_PLAY_BOSS_CLEAR (BSWSUB_START_NO+11)
#define BSWSUB_GET_PLAY_BOSS_CLEAR (BSWSUB_START_NO+12)
#define BSWSUB_ADD_BATTLE_POINT (BSWSUB_START_NO+13)
#define BSWSUB_SET_PARTNER_NO (BSWSUB_START_NO+14)
#define BSWSUB_GET_PARTNER_NO (BSWSUB_START_NO+15)
#define BSWSUB_BTL_TRAINER_SET (BSWSUB_START_NO+16)
#define BSWSUB_GET_SELPOKE_IDX (BSWSUB_START_NO+17)
#define BSWSUB_DUMMY00 (BSWSUB_START_NO+18)
#define BSWSUB_RECV_BUF_CLEAR (BSWSUB_START_NO+19)
#define BSWSUB_TRAINER_BEFORE_MSG (BSWSUB_START_NO+20)
#define BSWSUB_SAVE_GAMECLEAR (BSWSUB_START_NO+21)
#define BSWSUB_SET_PLAY_MODE_LOCATION (BSWSUB_START_NO+22)
#define BSWSUB_LOAD_POKEMON_MEMBER (BSWSUB_START_NO+23)
#define BSWSUB_SET_USE_BTL_BOX_FLAG (BSWSUB_START_NO+24)
#define BSWSUB_CHG_WORK_COMM_MULTI_MODE (BSWSUB_START_NO+25)
#define BSWSUB_CHK_S_MODE (BSWSUB_START_NO+26)
#define BSWSUB_DUMMY01 (BSWSUB_START_NO+27)
#define BSWSUB_DUMMY02 (BSWSUB_START_NO+28)
#define BSWSUB_SET_HOME_OBJ (BSWSUB_START_NO+29)
#define BSWSUB_SET_COMM_FLAG (BSWSUB_START_NO+30)
#define BSWSUB_GET_COMM_FLAG (BSWSUB_START_NO+31)
#define BSWSUB_GET_BATTLE_RESULT (BSWSUB_START_NO+32)
#define BSWSUB_SET_HOME_WORK (BSWSUB_START_NO+33)
#define BSWSUB_SET_TRAIN_BGM_ROUND_SW (BSWSUB_START_NO+34)
#define BSWSUB_CHOICE_BTL_SEVEN (BSWSUB_START_NO+35)
#define BSWSUB_GET_MEMBER_POKENO (BSWSUB_START_NO+36) 
#define BSWSUB_SET_COMM_IRC_FLAG (BSWSUB_START_NO+37)
#define BSWSUB_GET_COMM_IRC_FLAG (BSWSUB_START_NO+38)
#define BSWSUB_CHK_REGULATION (BSWSUB_START_NO+39)
#define BSWSUB_PREPAR_BTL_BOX (BSWSUB_START_NO+40)
#define BSWSUB_GET_REGULATION_TYPE (BSWSUB_START_NO+41)
#define BSWSUB_GET_MEMBER_NUM (BSWSUB_START_NO+42)
#define BSWSUB_CALL_BTLREC_EXIST (BSWSUB_START_NO+43) //�^�揈������ŕs�v
#define BSWSUB_CHK_BTLREC_EXIST (BSWSUB_START_NO+44) //�^�揈������ŕs�v
#define BSWSUB_STORE_BTLREC (BSWSUB_START_NO+45)
#define BSWSUB_SAVE_BTLREC (BSWSUB_START_NO+46) //�^�揈������ŕs�v
#define BSWSUB_FREE_BTLPRM (BSWSUB_START_NO+47)
#define BSWSUB_CALL_BTLREC (BSWSUB_START_NO+48)
#define BSWSUB_RESET_STAGE_ROUND (BSWSUB_START_NO+49)
#define BSWSUB_STOP_SHAKE_TRAIN (BSWSUB_START_NO+50)
#define BSWSUB_GET_TRAINER_NUM_ROUND_NOW (BSWSUB_START_NO+51)
#define BSWSUB_END_NO (BSWSUB_START_NO+52)

#define BSWSUB_COMM_START_NO (200)
#define BSWSUB_COMM_START (BSWSUB_COMM_START_NO+0)
#define BSWSUB_COMM_END (BSWSUB_COMM_START_NO+1)
#define BSWSUB_COMM_TIMSYNC (BSWSUB_COMM_START_NO+2)
#define BSWSUB_COMM_ENTRY_PARENT_MENU (BSWSUB_COMM_START_NO+3)
#define BSWSUB_COMM_ENTRY_CHILD_MENU (BSWSUB_COMM_START_NO+4)
#define BSWSUB_COMM_SEND_BUF (BSWSUB_COMM_START_NO+5)
#define BSWSUB_COMM_RECV_BUF (BSWSUB_COMM_START_NO+6)
#define BSWSUB_COMM_GET_CURRENT_ID (BSWSUB_COMM_START_NO+7)
#define BSWSUB_COMM_IRC_ENTRY (BSWSUB_COMM_START_NO+8)
#define BSWSUB_COMM_IRC_ENTRY_RESULT (BSWSUB_COMM_START_NO+9)
#define BSWSUB_COMM_SET_UNION_ACQ (BSWSUB_COMM_START_NO+10)
#define BSWSUB_COMM_GET_OBJCODE_OYA (BSWSUB_COMM_START_NO+11)
#define BSWSUB_COMM_GET_OBJCODE_KO (BSWSUB_COMM_START_NO+12)
#define BSWSUB_COMM_END_NO (BSWSUB_COMM_START_NO+13)

#define BSWSUB_DEBUG_START_NO (300)
#define BSWSUB_DEBUG_SET_SELECT_POKE (BSWSUB_DEBUG_START_NO+1)
#define BSWSUB_DEBUG_END_NO (BSWSUB_DEBUG_START_NO+2)

//�o�g���T�u�E�F�C�R�}���h�@����NULL
#define BSW_NULL (0)

//--------------------------------------------------------------
//  �ʐM�}���`�@�����R�[�h
//--------------------------------------------------------------
#define BSW_COMM_MULTI_RECEIPT_POKE_SELECT     1
#define BSW_COMM_MULTI_RECEIPT_BTL_TRAINER_SET 2
#define BSW_COMM_MULTI_RECEIPT_AUTO_SAVE       3
#define BSW_COMM_MULTI_RECEIPT_SIO_END         4
#define BSW_COMM_MULTI_RECEIPT_SEND_MYST       5

#define BSW_COMM_MULTI_ROUND_BATTLE_START 10
#define BSW_COMM_MULTI_ROUND_MENU_START   11
#define BSW_COMM_MULTI_ROUND_MENU_SELECT  12
#define BSW_COMM_MULTI_ROUND_EXIT_WAIT    13
#define BSW_COMM_MULTI_ROUND_RETIRE_WAIT  14
#define BSW_COMM_MULTI_ROUND_SIO_END      12     

#define BSW_COMM_MULTI_HOME_MENU_SELECT     20
#define BSW_COMM_MULTI_HOME_RETIRE_WAIT     21
#define BSW_COMM_MULTI_HOME_BTL_TRAINER_SET 22
#define BSW_COMM_MULTI_HOME_AUTO_SAVE       23
#define BSW_COMM_MULTI_HOME_SIO_END         24

//--------------------------------------------------------------
//  �ʐM�}���`�@�Q�����j���[����
//--------------------------------------------------------------
#define BSWAY_COMM_PERENT_ENTRY_OK 0 //�e���j���[�@�����o�[���W�܂���
#define BSWAY_COMM_PERENT_ENTRY_CANCEL 1 //�e���j���[�@�L�����Z��
#define BSWAY_COMM_PERENT_ENTRY_ERROR 2 //�e���j���[�@�G���[

#define BSWAY_COMM_CHILD_ENTRY_OK 0 //�q���j���[�@OK
#define BSWAY_COMM_CHILD_ENTRY_NG 1 //�q���j���[�@NG

#define BSWAY_COMM_ROUND_SEL_NEXT   0
#define BSWAY_COMM_ROUND_SEL_RETIRE 1

#define BSWAY_COMM_HOME_SEL_NEXT 0
#define BSWAY_COMM_HOME_SEL_END  1

#define BSWAY_COMM_IRC_RESULT_OK    0
#define BSWAY_COMM_IRC_RESULT_EXIT  1
#define BSWAY_COMM_IRC_RESULT_RETRY 2

//--------------------------------------------------------------
//  �X�N���v�g�@��ѐ���W�w��n
//--------------------------------------------------------------
//���@ �V���O��/�_�u�� ��t��ѐ�w��
#define SB_GATE_ZONE  ZONEID_D31R0201

//���@ �G���x�[�^�[ ��ѐ�
#define SB_EV_ZONE    ZONEID_D31R0202
#define SB_EV_PLAYER_SX      (3)
#define SB_EV_PLAYER_SY      (6)

//���@ �V���O���ʘH ��ѐ�
#define SB_WAYS_ZONE  ZONEID_D31R0203
#define SB_WAYS_PLAYER_SX    (2)
#define SB_WAYS_PLAYER_SY    (1)

//���@ �V���O���o�g�����[�� ������ѐ�
#define SB_ROOMS_ZONE  ZONEID_D31R0205
#define SB_ROOMS_PLAYER_SX    (10)
#define SB_ROOMS_PLAYER_SY    (4)

//���@ �}���`�ʘH ��ѐ�
#define SB_WAYD_ZONE  ZONEID_D31R0204
#define SB_WAYD_PLAYER_SX    (8)
#define SB_WAYD_PLAYER_SY    (3)

//���@ �}���`�o�g�����[�� ��ѐ�
#define SB_ROOMD_ZONE  ZONEID_D31R0206
#define SB_ROOMD_PLAYER_SX    (7)
#define SB_ROOMD_PLAYER_SY    (5)

//���@ �T���� ��ѐ�
#define SB_SALON_ZONE  ZONEID_D31R0207
#define SB_SALON_PLAYER_SX    (8)
#define SB_SALON_PLAYER_SY    (2)

//�ʐM��`(�v���`�i�Œǉ�)
#define BSWAY_COMM_PLAYER_DATA    (0)
#define BSWAY_COMM_TR_DATA      (1)
#define BSWAY_COMM_RETIRE_SELECT  (2)
#define BSWAY_COMM_MYSTATUS_DATA (3)

//D31R0205_HERO kari
#define HERO_OBJ (0)

//--------------------------------------------------
//D31R0205_PCWOMAN2
#define PCWOMAN_OBJ (1)
//kari
#define TRAINER_OBJ (2)
//kari
#define OBJCODE_TRAINBOSS (TBOSS)

//--------------------------------------------------------------
//  ��Ԏ�� FLDEFF_BTRAIN_TYPE�Ɠ���ł��鎖
//--------------------------------------------------------------
#if 0
#define BTRAIN_TYPE_01 (0) //�V���O���g���C��
#define BTRAIN_TYPE_02 (1) //�X�[�p�[�V���O���g���C��
#define BTRAIN_TYPE_03 (2) //�_�u���g���C��
#define BTRAIN_TYPE_04 (3) //�X�[�p�[�_�u���g���C��
#define BTRAIN_TYPE_05 (4) //�}���`�g���C��
#define BTRAIN_TYPE_06 (5) //�X�[�p�[�}���`�g���C��
#define BTRAIN_TYPE_07 (6) //WiFi�g���C��
#endif

//--------------------------------------------------------------
//  ��ԃA�j�� FLDEFF_BTRAIN_ANIME_TYPE�Ɠ���ł��鎖
//--------------------------------------------------------------
#define BTRAIN_ANIME_TYPE_START_GEAR (0) //�M�A�X�e�o���A�j���@������A����
#define BTRAIN_ANIME_TYPE_ARRIVAL (1)  //�r���w�d�Ԑi���A�j��
#define BTRAIN_ANIME_TYPE_ARRIVAL_HOME (2) //�z�[���i���A�d�ԊJ���A�j��
#define BTRAIN_ANIME_TYPE_START_HOME (3)  //�z�[���o���A�j�� ������A����

//--------------------------------------------------------------
//  ��ԍ��W���
//--------------------------------------------------------------
#define BTRAIN_POS_RECEIPT (0) //��t�z�[��
#define BTRAIN_POS_HOME (1) //�r���w�A�I�_
#define BTRAIN_POS_MAX (2) //�ő�

//--------------------------------------------------------------
///
//--------------------------------------------------------------
#define BSW_TRAIN_OBJID_JIKI (0xe0)
#define BSW_TRAIN_OBJID_PARTNER (0xe1)
#define BSW_TRAIN_OBJID_TRAINER_0 (0xe2)
#define BSW_TRAIN_OBJID_TRAINER_1 (0xe3)
#define BSW_HOME_OBJID_NPC_FIRST (0x80)

//--------------------------------------------------------------
//  �f�o�b�O�p�t���O
//--------------------------------------------------------------
#define BSW_DEBUG_FLAG_REGU_OFF (1<<0) //���M������
#define BSW_DEBUG_FLAG_AUTO (1<<1) //�I�[�g
#define BSW_DEBUG_FLAG_BTL_SKIP (1<<2) //�o�g���X�L�b�v

#endif //__SUBWAY_SCR_DEF_H__
