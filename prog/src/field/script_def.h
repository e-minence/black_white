//======================================================================
/**
 * @file  script_def.h
 * @brief �X�N���v�g��`�t�@�C��
 *
 * �X�N���v�g���ƃv���O�����Ƃŋ��ʂ��ĎQ�Ƃ���K�v�������`���W�񂷂�w�b�_�t�@�C���B
 * �X�N���v�g����enum�Ȃ�C���L�̕��@���߂��ł��Ȃ����߁Adefine���g�p���邱�ƁB
 */
//======================================================================
#pragma once

//======================================================================
//
//  �X�N���v�g�w�b�_�֘A
//
//  �X�N���v�g���ł̓}�N���ɖ��ߍ��܂�Ă��邽�߁A���ڂ����̒l��
//  �ӎ��I�Ɏg�p���邱�Ƃ͂Ȃ��B
//======================================================================
//--------------------------------------------------------------
//����X�N���v�g��`
//--------------------------------------------------------------
#define SP_SCRID_NONE       (0)
#define SP_SCRID_SCENE_CHANGE   (1)     ///<����X�N���v�g�e�[�u���ł̃V�[�������C�x���g�f�[�^�̎w��ID
#define SP_SCRID_ZONE_CHANGE    (2)     ///<����X�N���v�g�e�[�u���ł̃]�[���؂�ւ��X�N���v�g�̎w��ID
#define SP_SCRID_FIELD_RECOVER  (3)     ///<����X�N���v�g�e�[�u���ł̃t�B�[���h���A�X�N���v�g�w��ID
#define SP_SCRID_FIELD_INIT     (4)     ///<����X�N���v�g�e�[�u���ł̃t�B�[���h�������X�N���v�g�w��ID

///�C�x���g�f�[�^�e�[�u�������𔻒肷�邽�߂̒l
#define EV_DATA_END_CODE  (0xfd13)  //�K���Ȓl

//======================================================================
//
//    OBJ�֘A
//
//======================================================================
//--------------------------------------------------------------
//�G�C���A�X���ʗp�X�N���v�gID
#define SP_SCRID_ALIES        (0xffff)

//�A�����OBJ���擾����OBJID��`
#define SCR_OBJID_MV_PAIR     (0xf2)

//�����_�~�[OBJ���擾����OBJID��`
#define SCR_OBJID_DUMMY       (0xf1)

//======================================================================
//    �g���[�i�[�֘A�̒�`
//======================================================================
//�g���[�i�[��������^�C�v��`
#define SCR_EYE_TR_TYPE_SINGLE    (0) ///<�P�Ɠ���
#define SCR_EYE_TR_TYPE_DOUBLE    (1) ///<�_�u������
#define SCR_EYE_TR_TYPE_TAG       (2) ///<�^�b�O����i�ЂƂ肸�����j

//�g���[�i�[�̔ԍ���`
#define SCR_EYE_TR_0        (0)
#define SCR_EYE_TR_1        (1)

//�g���[�i�[�̐퓬���[���^�C�v��`
#define SCR_TR_BTL_RULE_SINGLE    0   ///<�V���O��
#define SCR_TR_BTL_RULE_DOUBLE    1   ///<�_�u��
#define SCR_TR_BTL_RULE_TRIPLE    2   ///<�g���v��
#define SCR_TR_BTL_RULE_ROTATION  3   ///<���[�e�[�V����

#define SCR_TR_SPTYPE_NONE        0   ///<�g���[�i�[����^�C�v�F�Ȃ�
#define SCR_TR_SPTYPE_RECOVER     1   ///<�g���[�i�[����^�C�v�F��
#define SCR_TR_SPTYPE_ITEM        1   ///<�g���[�i�[����^�C�v�F�A�C�e��

//======================================================================
//    �e�R�}���h�ˑ��̒�`
//======================================================================
//--------------------------------------------------------------
//fld_menu.c
//--------------------------------------------------------------
#define SCR_REPORT_OK       (0)     //�Z�[�u����
#define SCR_REPORT_CANCEL     (1)     //�Z�[�u���Ă��Ȃ�

///YES_NO_WIN�̖߂�l
#define SCR_YES   (0) //YES_NO_WIN�̖߂�l �u�͂��v��Ԃ���
#define SCR_NO    (1) //YES_NO_WIN�̖߂�l �u�������v��Ԃ���

//�V���b�v��ID��`
#define SCR_SHOPID_NULL        ( 0xFF )  //�ϓ��V���b�v�Ăяo��ID
#define SCR_SHOPID_BP_ITEM     ( 0xFE )  //BP�A�C�e���V���b�v�Ăяo��ID
#define SCR_SHOPID_BP_WAZA     ( 0xFD )  //BP�Z�}�V���V���b�v�Ăяo��ID
#define SCR_SHOPID_BLACK_CITY  ( 0xFC )  //�u���b�N�V�e�B�V���b�v <---���g�p�ɂȂ�܂�
#define SCR_SHOPID_BLACK_CITY4 ( 0xFB )  //�u���b�N�V�e�B�V���b�v�Ăяo��ID:0
#define SCR_SHOPID_BLACK_CITY3 ( 0xFA )  //�u���b�N�V�e�B�V���b�v�Ăяo��ID:1
#define SCR_SHOPID_BLACK_CITY2 ( 0xF9 )  //�u���b�N�V�e�B�V���b�v�Ăяo��ID:2
#define SCR_SHOPID_BLACK_CITY1 ( 0xF8 )  //�u���b�N�V�e�B�V���b�v�Ăяo��ID:3
#define SCR_SHOPID_BLACK_CITY0 ( 0xF7 )  //�u���b�N�V�e�B�V���b�v�Ăяo��ID:4

//�T�u�v���Z�X(�A�v��)�̏I���R�[�h��`
#define SCR_PROC_RETMODE_EXIT    (0) //�ꔭ�I��
#define SCR_PROC_RETMODE_NORMAL  (1) //�ʏ�I��

//�o�b�O�v���Z�X�Ăяo���R�[�h��`
#define SCR_BAG_MODE_SELL   (0)
#define SCR_BAG_MODE_SELECT (1)
#define SCR_BAG_MODE_FIELD  (2)

//�t�B�[���h�ʐM�ؒf�����C�x���g�̌���
#define SCR_FIELD_COMM_EXIT_OK      (0) //����ɒʐM�ؒf�������I�����
#define SCR_FIELD_COMM_EXIT_CANCEL  (1) //�ؒf�������L�����Z��
#define SCR_FIELD_COMM_EXIT_ERROR   (2) //�ؒf�������ɂȂ�炩�̃G���[

//--------------------------------------------------------------
/// BmpMenu�̐������b�Z�[�W����ID
//--------------------------------------------------------------
#define SCR_BMPMENU_EXMSG_NULL   (0xFFFF)

//--------------------------------------------------------------
//  �o�g���֘A
//--------------------------------------------------------------
///TRAINER_LOSE_CHECK,NORMAL_LOSE_CHECK
#define SCR_BATTLE_RESULT_LOSE (0) //����
#define SCR_BATTLE_RESULT_WIN  (1) //����

#define SCR_BATTLE_MODE_NONE    (0) //���Ƀ��[�h�w��Ȃ�
#define SCR_BATTLE_MODE_NOLOSE  (1) //�����Ȃ����[�h

///�쐶�|�P�����퓬�Ăяo�����Ɏw��ł���t���O
#define SCR_WILD_BTL_FLAG_NONE    (0x0000)  //�t���O����
#define SCR_WILD_BTL_FLAG_LEGEND  (0x0001)  //�`���퓬(�G���J�E���g���b�Z�[�W���ω�����)
#define SCR_WILD_BTL_FLAG_RARE      (0x0002)  //�������A�|�P�퓬  �v�a�ł͖�����
#define SCR_WILD_BTL_FLAG_ITEMNONE  (0x0004)  //�����A�C�e����������
#define SCR_WILD_BTL_FLAG_SPEABI3   (0x0008)  //��3�����K�p
#define SCR_WILD_BTL_FLAG_NO_RARE   (0x0010)  //���A�֎~
#define SCR_WILD_BTL_FLAG_MALE      (0x0020)  //�I�X�Ɛ퓬
#define SCR_WILD_BTL_FLAG_FEMALE    (0x0040)  //���X�Ɛ퓬


///�쐶�|�P������@�Đ�R�[�h�`�F�b�N
#define SCR_WILD_BTL_RET_CAPTURE  (0) //�߂܂���
#define SCR_WILD_BTL_RET_ESCAPE   (1) //������
#define SCR_WILD_BTL_RET_WIN      (2) //�|����

//--------------------------------------------------------------
//�j���擾�֐��̖߂�l
//  _GET_WEEK
//--------------------------------------------------------------
#define RTC_WEEK_SUNDAY   0
#define RTC_WEEK_MONDAY   1
#define RTC_WEEK_TUESDAY  2
#define RTC_WEEK_WEDNESDAY  3
#define RTC_WEEK_THURSDAY 4
#define RTC_WEEK_FRIDAY   5
#define RTC_WEEK_SATURDAY 6

//--------------------------------------------------------------
//�o�b�W�t���O�w��ID�̒�`
//  _GET_BADGE_FLAG/_SET_BADGE_FLAG
//--------------------------------------------------------------
#define BADGE_ID_01   0
#define BADGE_ID_02   1
#define BADGE_ID_03   2
#define BADGE_ID_04   3
#define BADGE_ID_05   4
#define BADGE_ID_06   5
#define BADGE_ID_07   6
#define BADGE_ID_08   7

//--------------------------------------------------------------
// �莝���|�P�������J�E���g���[�h
//--------------------------------------------------------------
#define POKECOUNT_MODE_TOTAL         (0)  // �莝���̐�
#define POKECOUNT_MODE_NOT_EGG       (1)  // �^�}�S�������莝���̐�
#define POKECOUNT_MODE_BATTLE_ENABLE (2)  // �킦��(�^�}�S�ƕm����������)�|�P������
#define POKECOUNT_MODE_ONLY_EGG      (3)  // �^�}�S�̐�(�ʖڃ^�}�S������)
#define POKECOUNT_MODE_ONLY_DAME_EGG (4)  // �ʖڃ^�}�S�̐�
#define POKECOUNT_MODE_EMPTY         (5)  // �󂢂Ă��鐔

//--------------------------------------------------------------
//
//--------------------------------------------------------------
#define SCR_POKEPARA_MONSNO           (5) ///< �|�P�����i���o�[
#define SCR_POKEPARA_ITEMNO           (6) ///< �ێ��A�C�e���i���o�[
#define SCR_POKEPARA_COUNTRY_CODE     (12)  ///< ���R�[�h
#define SCR_POKEPARA_HAIHU_FLAG       (109) ///<�C�x���g�z�z�t���O
#define SCR_POKEPARA_SEX              (110) ///<����
#define SCR_POKEPARA_FORMNO           (111) ///<�t�H�����i���o�[
#define SCR_POKEPARA_SEIKAKU          (112) ///<���i
#define SCR_POKEPARA_GET_ROM          (119) ///<�߂܂�������
#define SCR_POKEPARA_GET_YEAR         (143) ///<�ߊl�������̔N
#define SCR_POKEPARA_GET_MONTH        (144) ///<�ߊl�������̌�
#define SCR_POKEPARA_GET_DAY          (145) ///<�ߊl�������̓�
#define SCR_POKEPARA_BIRTH_YEAR       (146) ///<���܂ꂽ�N
#define SCR_POKEPARA_BIRTH_MONTH      (147) ///<���܂ꂽ��
#define SCR_POKEPARA_BIRTH_DAY        (148) ///<���܂ꂽ��
#define SCR_POKEPARA_GET_PLACE        (149) ///<�ߊl�����ꏊ
#define SCR_POKEPARA_BIRTH_PLACE      (150) ///<���܂ꂽ�ꏊ
#define SCR_POKEPARA_GET_LEVEL        (153) ///<�ߊl�������̃��x��
#define SCR_POKEPARA_OYA_SEX          (154) ///<�e�̐���
#define SCR_POKEPARA_LEVEL            (158) ///<���݂̃��x��

//--------------------------------------------------------------
// �|�P�����ߊl�ꏊ�@�`�F�b�N
//--------------------------------------------------------------
#define POKE_GET_PLACE_CHECK_WF         (0)  // �z���C�g�t�H���X�g�ŕߊl�����̂��`�F�b�N
#define POKE_GET_PLACE_CHECK_MAX        (1)  // �z���C�g�t�H���X�g�ŕߊl�����̂��`�F�b�N

//--------------------------------------------------------------
//  �����񑀍샂�[�h�w��
//--------------------------------------------------------------
#define ZUKANCTRL_MODE_SEE     (0)   ///<�����񑀍샂�[�h�F������
#define ZUKANCTRL_MODE_GET     (1)   ///<�����񑀍샂�[�h�F�߂܂�����

//--------------------------------------------------------------
//  �z�u���f���p�A�j���w��
//--------------------------------------------------------------
#define SCR_BMID_DOOR           (1)       ///<�z�u���f�����ށF�h�A
#define SCR_BMID_SANDSTREAM     (2)       ///<�z�u���f�����ށF����
#define SCR_BMID_PCMACHINE      (3)       ///<�z�u���f�����ށF�񕜃}�V��
#define SCR_BMID_PC             (4)       ///<�z�u���f�����ށF�p�\�R��
#define SCR_BMID_PCEV_DOOR      (5)       ///<�z�u���f�����ށF�|�P�Z���G���x�[�^�[�F�h�A
#define SCR_BMID_PCEV_FLOOR     (6)       ///<�z�u���f�����ށF�|�P�Z���G���x�[�^�[�F�t���A
#define SCR_BMID_WARP           (7)       ///<�z�u���f�����ށF���[�v
#define SCR_BMID_EVENT          (8)       ///<�z�u���f�����ށF���[�v
#define SCR_BMID_PPMACHINE      (9)       ///<�z�u���f�����ށF�p���p�[�N�}�V��

#define  SCR_BMANM_DOOR_OPEN    (0)       ///<�h�A�A�j���F�J��
#define  SCR_BMANM_DOOR_CLOSE   (1)       ///<�h�A�A�j���F����

#define  SCR_BMANM_PCEV_OPEN    (0)       ///<�|�P�Z���G���x�[�^�A�j���F�J��
#define  SCR_BMANM_PCEV_CLOSE   (1)       ///<�|�P�Z���G���x�[�^�A�j���F����

#define  SCR_BMANM_PCEV_UP      (0)       ///<�|�P�Z���G���x�[�^�A�j���F�オ��
#define  SCR_BMANM_PCEV_DOWN    (1)       ///<�|�P�Z���G���x�[�^�A�j���F������

#define  SCR_BMANM_PPM_MOV    (0)       ///<�p���p�[�N�}�V���A�j���F�N��
#define  SCR_BMANM_PPM_LOOP    (1)       ///<�p���p�[�N�}�V���A�j���F���[�v

//--------------------------------------------------------------
/// ���b�Z�[�W�E�B���h�E�㉺�w��
//--------------------------------------------------------------
#define SCRCMD_MSGWIN_UPLEFT    (0)
#define SCRCMD_MSGWIN_DOWNLEFT  (1)
#define SCRCMD_MSGWIN_DEFAULT   (2)
#define SCRCMD_MSGWIN_UPRIGHT   (3)
#define SCRCMD_MSGWIN_DOWNRIGHT (4)
#define SCRCMD_MSGWIN_UP_AUTO   (5)
#define SCRCMD_MSGWIN_DOWN_AUTO (6)

#define SCRCMD_MSGWIN_UP    SCRCMD_MSGWIN_UPLEFT
#define SCRCMD_MSGWIN_DOWN  SCRCMD_MSGWIN_DOWNLEFT
#define SCRCMD_MSGWIN_NON   SCRCMD_MSGWIN_DEFAULT 


//--------------------------------------------------------------
/// �V�X�e�����b�Z�[�W�E�B���h�E�@�^�C���A�C�R���@�X�C�b�`
//--------------------------------------------------------------
#define SCR_SYSWIN_TIMEICON_ON  (0)
#define SCR_SYSWIN_TIMEICON_OFF (1)

//--------------------------------------------------------------
/// BG�E�B���h�E�^�C�v
//--------------------------------------------------------------
#define SCRCMD_BGWIN_TYPE_INFO (0)
#define SCRCMD_BGWIN_TYPE_TOWN (1)
#define SCRCMD_BGWIN_TYPE_POST (2)
#define SCRCMD_BGWIN_TYPE_ROAD (3)

//--------------------------------------------------------------
/// ����E�B���h�E�^�C�v
//--------------------------------------------------------------
#define SCRCMD_SPWIN_TYPE_LETTER (0)
#define SCRCMD_SPWIN_TYPE_BOOK (1)

//--------------------------------------------------------------
/// �o�g�����R�[�_�[�Ăяo�����[�h�w��
//--------------------------------------------------------------
#define SCRCMD_BTL_RECORDER_MODE_VIDEO   (0)
#define SCRCMD_BTL_RECORDER_MODE_MUSICAL (1)

//--------------------------------------------------------------
/// �ΐ펞�p�[�e�B�^�C�v�w��
//--------------------------------------------------------------
#define SCR_BTL_PARTY_SELECT_TEMOTI   (0) //�莝��
#define SCR_BTL_PARTY_SELECT_BTLBOX   (1) //�o�g���{�b�N�X
#define SCR_BTL_PARTY_SELECT_CANCEL   (2) //�I���L�����Z��
#define SCR_BTL_PARTY_SELECT_NG       (3) //���M�����[�V����NG

//--------------------------------------------------------------
/// �Q�[���������\�`�F�b�N�߂�l
//--------------------------------------------------------------
#define FLD_TRADE_ENABLE         (0)  // ������
#define FLD_TRADE_DISABLE_EGG    (1)  // �����s��(�^�}�S�̂���)
#define FLD_TRADE_DISABLE_MONSNO (2)  // �����s��(�Ⴄ�����X�^�[�̂���)
#define FLD_TRADE_DISABLE_SEX    (3)  // �����s��(�Ⴄ���ʂ̂���)

//--------------------------------------------------------------
/// ���C�����X�Z�[�u���[�h
//--------------------------------------------------------------
#define WIRELESS_SAVEMODE_OFF (0)
#define WIRELESS_SAVEMODE_ON  (1)

//--------------------------------------------------------------
/// �Z�v���o������
//--------------------------------------------------------------
#define SCR_WAZAOSHIE_RET_SET    (0)
#define SCR_WAZAOSHIE_RET_CANCEL (1)

//--------------------------------------------------------------
/// �Z���ڂ����[�h�w��
//--------------------------------------------------------------
#define SCR_SKILLTEACH_MODE_DRAGON    (0)
#define SCR_SKILLTEACH_MODE_STRONGEST (1)
#define SCR_SKILLTEACH_MODE_COALESCENCE (2)

#define SCR_SKILLTEACH_CHECK_RESULT_OK         (0)
#define SCR_SKILLTEACH_CHECK_RESULT_POKEMON_NG (1)
#define SCR_SKILLTEACH_CHECK_RESULT_NATSUKI_NG (2)
#define SCR_SKILLTEACH_CHECK_RESULT_ALREADY_NG (3)

//--------------------------------------------------------------
/// �{�b�N�X�I�����[�h
//--------------------------------------------------------------
#define SCR_BOX_END_MODE_MENU   (0)  // �I�����j���[�֖߂�
#define SCR_BOX_END_MODE_C_GEAR (1)  // C-gear�܂Ŗ߂�

//--------------------------------------------------------------
//  �W���b�W�̔���Ŏ擾���鍀��
//--------------------------------------------------------------
#define SCR_JUDGE_MODE_TOTAL      (0)
#define SCR_JUDGE_MODE_HIGH_VALUE (1)
#define SCR_JUDGE_MODE_HP         (2)
#define SCR_JUDGE_MODE_POW        (3)
#define SCR_JUDGE_MODE_DEF        (4)
#define SCR_JUDGE_MODE_AGI        (5)
#define SCR_JUDGE_MODE_SPPOW      (6)
#define SCR_JUDGE_MODE_SPDEF      (7)

//--------------------------------------------------------------
//  ������]���̃��[�h�w��
//--------------------------------------------------------------
#define SCR_ZUKAN_HYOUKA_MODE_AUTO        (0)   ///<�������ʁi�p�\�R���j
#define SCR_ZUKAN_HYOUKA_MODE_GLOBAL      (1)   ///<��ɑS��������i���m�E���j
#define SCR_ZUKAN_HYOUKA_MODE_LOCAL_SEE   (2)   ///<�n��������E�����i���m�E���j
#define SCR_ZUKAN_HYOUKA_MODE_LOCAL_GET   (3)   ///<�n��������E�����i���m�E���j

//--------------------------------------------------------------
/// �{�b�N�X�ǉ��ǎ��w��  _SET_BOX_WALLPAPER()�Ɏg�p
//--------------------------------------------------------------
#define SCR_BOX_EX_WALLPAPER1   (1)
#define SCR_BOX_EX_WALLPAPER2   (2)

//--------------------------------------------------------------
//  �s�v�c�ȑ��蕨�@�z�B��
//--------------------------------------------------------------
#define SCR_POSTMAN_REQ_EXISTS  (0) ///<���݃`�F�b�N
#define SCR_POSTMAN_REQ_ENABLE  (1) ///<�󂯎��邩�H�`�F�b�N
#define SCR_POSTMAN_REQ_OK_MSG  (2) ///<�󂯎�������b�Z�[�WID
#define SCR_POSTMAN_REQ_NG_MSG  (3) ///<�󂯎��Ȃ����b�Z�[�WID
#define SCR_POSTMAN_REQ_RECEIVE (4) ///<�󂯎�菈��
#define SCR_POSTMAN_REQ_OBJID   (5) ///<�z�B����OBJID���擾
#define SCR_POSTMAN_REQ_OBJSTAT (6) ///<�z�B��OBJ�̏�Ԏ擾


#define SCR_POSTMAN_OBJ_NONE    (0) ///<�z�B��OBJ:���݂��Ȃ�
#define SCR_POSTMAN_OBJ_EXISTS  (1) ///<�z�B��OBJ:���݂���

//--------------------------------------------------------------
//  PDW�z�B��
//--------------------------------------------------------------
#define SCR_PDW_POSTMAN_REQ_EXISTS      (0) ///<���݃`�F�b�N
#define SCR_PDW_POSTMAN_REQ_ENABLE      (1) ///<�󂯎��邩�H�`�F�b�N
#define SCR_PDW_POSTMAN_REQ_OK_INFO     (2) ///<�󂯎����\��
#define SCR_PDW_POSTMAN_REQ_RECEIVE     (3) ///<�󂯎�菈��
#define SCR_PDW_POSTMAN_REQ_NG_COUNT    (4) ///<�󂯎��Ȃ��J�E���g�擾
#define SCR_PDW_POSTMAN_REQ_NG_ITEM     (5) ///<�󂯎��Ȃ��ǂ������Z�b�g

//--------------------------------------------------------------
/// �r�[�R�����M���N�G�X�gID
//--------------------------------------------------------------
#define SCR_BEACON_SET_REQ_ITEM_GET           (0) ///<�t�B�[���h�ŃA�C�e�����E����
#define SCR_BEACON_SET_REQ_FSKILL_USE         (1) ///<�t�B�[���h�Z�g�p�n
#define SCR_BEACON_SET_REQ_UNION_COUNTER      (2) ///<���j�I���J�E���^�[�����J�n�J�n
#define SCR_BEACON_SET_REQ_TRIAL_HOUSE_START  (3) ///<�g���C�A���n�E�X����J�n
#define SCR_BEACON_SET_REQ_TRIAL_HOUSE_RANK   (4) ///<�g���C�A���n�E�X�����N�Z�b�g
#define SCR_BEACON_SET_REQ_POKE_SHIFTER_START (5) ///<�|�P�V�t�^�[����J�n
#define SCR_BEACON_SET_REQ_SUBWAY_START       (6) ///<�T�u�E�F�C����J�n
#define SCR_BEACON_SET_REQ_SUBWAY_WIN         (7) ///<�T�u�E�F�C����
#define SCR_BEACON_SET_REQ_SUBWAY_TROPHY_GET  (8) ///<�T�u�E�F�C�g���t�B�[�Q�b�g

//--------------------------------------------------------------
//    �^�C�g������̃X�^�[�g���j���[����
//--------------------------------------------------------------
#define SCR_STARTMENU_FLAG_HUSHIGI   (0)    ///<�u�s�v�c�ȑ��蕨�v
#define SCR_STARTMENU_FLAG_BATTLE    (1)    ///<�u�o�g�����v
#define SCR_STARTMENU_FLAG_GAMESYNC  (2)    ///<�u�Q�[���V���N�ݒ�v
#define SCR_STARTMENU_FLAG_MACHINE   (3)    ///<�u�]���}�V���������v

//--------------------------------------------------------------
//  �ړ��|�P����
//  src/field/event_movepoke.h�̒�`�ƈ�v���Ă���K�v������B
//--------------------------------------------------------------
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_INSIDE (0)
#define SCR_EV_MOVEPOKE_ANIME_RAI_KAZA_OUTSIDE (1)
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_INSIDE  (2)
#define SCR_EV_MOVEPOKE_ANIME_TUCHI_OUTSIDE (3)


//--------------------------------------------------------------
//  �T�u�X�N���[������
//--------------------------------------------------------------
#define SCR_EV_SUBSCREEN_MODE_SAVE  (0) // �Z�[�u��ʂɂ���
#define SCR_EV_SUBSCREEN_MODE_MAX  (1)


//--------------------------------------------------------------
//  �C��_�a����
//--------------------------------------------------------------
#define SCR_EV_DIVING_MAPCHANGE_DOWN    (0)
#define SCR_EV_DIVING_MAPCHANGE_UP      (1)
#define SCR_EV_DIVING_MAPCHANGE_MAX     (2)

//--------------------------------------------------------------
///�@�}�ӊ����܏�&�H���}�\���A�v���N���R�[�h (����A�v�����Ăяo��)
//--------------------------------------------------------------
#define SCR_ZUKAN_AWARD_CHIHOU    (0)  // �n���}�ӊ���
#define SCR_ZUKAN_AWARD_ZENKOKU   (1)  // �S���}�ӊ���
#define SCR_ZUKAN_AWARD_SUBWAY_ROUTE_MAP  (2)  // �H���}

//--------------------------------------------------------------
/// ���@����`��
//--------------------------------------------------------------
#define SCR_PLAYER_FORM_NORMAL (0)
#define SCR_PLAYER_FORM_CYCLE (1)
#define SCR_PLAYER_FORM_SWIM (2)
#define SCR_PLAYER_FORM_DIVING (3)

//--------------------------------------------------------------
/// �{�[���A�j���^�C�v�w��p
//--------------------------------------------------------------
#define SCR_BALL_ANM_TYPE_OUT   (0)
#define SCR_BALL_ANM_TYPE_IN   (1)


//--------------------------------------------------------------
/// �Ӓ�m�p
//--------------------------------------------------------------
#define SCR_ITEM_JUDGE_OBJTYPE_GOURMET      (0)   // �O����
#define SCR_ITEM_JUDGE_OBJTYPE_STONE_MANIA  (1)   // �΃}�j�A
#define SCR_ITEM_JUDGE_OBJTYPE_RICH         (2)   // ��x��

//--------------------------------------------------------------
//    �V���{���G���J�E���g�F���擾�p
//--------------------------------------------------------------
#define SCR_SYMMAP_INFO_IS_MINE         (0)   ///<�����̃p���X�}�b�v���H
#define SCR_SYMMAP_INFO_IS_KEEPZONE     (1)   ///<�L�[�v�]�[�����H
#define SCR_SYMMAP_INFO_IS_ENTRANCE     (2)   ///<�������}�b�v���H


//--------------------------------------------------------------
// �u�a������v�f�[�^�`�F�b�N�̖߂�l
//--------------------------------------------------------------
#define SCR_DENDOU_DATA_NULL (0) // �f�[�^�Ȃ�
#define SCR_DENDOU_DATA_OK   (1) // ����f�[�^����
#define SCR_DENDOU_DATA_NG   (2) // �ُ�f�[�^����

//--------------------------------------------------------------
//    �Q�[���N���A�w��    _GOTO_GAMECLEAR_DEMO()�R�}���h�p
//    prog/src/field/event_gameclear.h��GAMECLEAR_MODE�ƈ�v���Ă���K�v������
//--------------------------------------------------------------
#define SCR_GAMECLEAR_MODE_FIRST      (0)     ///<����Q�[���N���A
#define SCR_GAMECLEAR_MODE_DENDOU     (1)     ///<2��ڈȍ~�A�a������

//--------------------------------------------------------------
//    c01�W���J�[�e����
//--------------------------------------------------------------
#define SCR_GYM01_CURTAIN_NOTOPEN     (0)     ///<�J�[�e���󂢂ĂȂ�
#define SCR_GYM01_CURTAIN_OPEN_ONE    (1)     ///<�J�[�e��1�J���Ă��� 
#define SCR_GYM01_CURTAIN_OPEN_TWO    (3)     ///<�J�[�e��2�J���Ă��� 
#define SCR_GYM01_CURTAIN_OPEN_THREE  (7)     ///<�J�[�e��3�J���Ă��� 


