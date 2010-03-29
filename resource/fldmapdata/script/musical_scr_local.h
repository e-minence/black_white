//======================================================================
//                musical_scr_local.h
//
//    �X�N���v�g�t�@�C���F�~���[�W�J���֌W���[�J���萔��`
//
//======================================================================

#define MUSICAL_FITTING_BGM (SEQ_BGM_MSL_DRESSUP)

//�C�x���g�œn���O�b�Y
#define MUSICAL_GOODS_KEEKI (24)
#define MUSICAL_GOODS_KURAUN (55)
#define MUSICAL_GOODS_THIARA (67)
#define MUSICAL_GOODS_YUUSHOUBERUTO (82)
#define MUSICAL_GOODS_OHESOBOTAN (87)
#define MUSICAL_GOODS_NIKONIKOOMEN (86)
#define MUSICAL_GOODS_EREKIGITAA (84)
#define MUSICAL_GOODS_PUREZENTOBAKO (99)
#define MUSICAL_GOODS_FUSAFUSANOHIGE (98)
#define MUSICAL_GOODS_AKAIBOUSI (95)
#define MUSICAL_GOODS_OOKINAFUKURO (96)

//�������Ă���R�}���h�͍T�����̂ݗL���ł��I

//�R���f�B�V�����̒�`
#define MUSICAL_CONDITION_COOL (0)    //�N�[��
#define MUSICAL_CONDITION_CUTE (1)    //�L���[�g
#define MUSICAL_CONDITION_ELEGANT (2) //�G���K���g
#define MUSICAL_CONDITION_UNIQUE (3)  //���j�[�N
#define MUSICAL_CONDITION_NONE (4)  //��ȊO�̂Ƃ��ɋA���Ă���l(���������Ƃ��Ώۂ���������
#define MUSICAL_CONDITION_BALANCE (5)  //�S���ꏏ


//_GET_MUSICAL_FAN_VALUE_GIFT_TYPE �Ŗ߂��Ă���l
#define MUSICAL_GIFT_TYPE_NONE  (0)  //����
#define MUSICAL_GIFT_TYPE_GOODS (1)  //�O�b�Y
#define MUSICAL_GIFT_TYPE_ITEM  (2)  //�A�C�e��

//_GET_MUSICAL_FAN_VALUE_TYPE �ŋ��Ȃ����̒l
//���鎞��OBJID���A���Ă��Ă܂��B
#define MUSICAL_FAL_TYPE_NONE (0)

//�R�R���牺�͊�{���b�p�[���Ă���̂Ńv���O�����ŎQ��
//���͍T������p
//_MUSICAL_WORD �p�^�C�v
#define MUSICAL_WORD_TITLE (0)        //�Z�[�u�ɂ��鉉��
#define MUSICAL_WORD_GOODS (1)        //�O�b�Y��
#define MUSICAL_WORD_TITLE_LOCAL (2)  //�����݉���
#define MUSICAL_WORD_AUDI_TYPE (3)  //�l�C�q�w
#define MUSICAL_WORD_CONDITION (4)  //�R���f�B�V������
#define MUSICAL_WORD_NPC_NAME (5)  //���Q���Җ��O

//_MUSICAL_CALL �p
#define MUSICAL_CALL_LOCAL (0)        //��ʐM
#define MUSICAL_CALL_COMM  (1)        //�ʐM

//_GET_MUSICAL_VALUE �p
#define MUSICAL_VALUE_JOINNUM (0)   //�Q����
#define MUSICAL_VALUE_TOPNUM  (1)   //�g�b�v��
#define MUSICAL_VALUE_LAST_POINT (2)  //�ŏI�]���_
#define MUSICAL_VALUE_LAST_CONDITION (3)  //�ŏI�R���f�B�V����(���_�͂��̑��ŕԂ�
#define MUSICAL_VALUE_PROGRAM_NUMBER (4)  //�I�����ڔԍ�
#define MUSICAL_VALUE_SET_PROGRAM_NUMBER (5)  //�I�����ڔԍ��ݒ�
#define MUSICAL_VALUE_IS_EQUIP_ANY (6)    //�����������Ă��邩�H
#define MUSICAL_VALUE_IS_ENABLE_SHOT (7)  //�~���[�W�J���V���b�g���L�����H
#define MUSICAL_VALUE_LAST_CONDITION_MIN (8) //�ŏI�R���f�B�V����(�Œ�)
#define MUSICAL_VALUE_LAST_CONDITION_MAX (9) //�ŏI�R���f�B�V����(�ō�)(���_�͗D�揇�ʂŐݒ肳�ꂽ���ɕԂ��Ă���
#define MUSICAL_VALUE_LAST_CONDITION_2ND (10) //�ŏI�R���f�B�V����(�Q��)(���_�͂��̑��ŕԂ�
#define MUSICAL_VALUE_COUNT_MUSICAL_POKE (11) //�Q���\�ȃ|�P�����邩�H
#define MUSICAL_VALUE_CHECK_MUSICAL_POKE (12) //Party�Ń|�P���Q���ł��邩�H
#define MUSICAL_VALUE_IS_EQUIP_ANY_DRESSUP (13)    //�����������Ă��邩�H(�����L��
#define MUSICAL_VALUE_IS_ENABLE_DIST_DATA (14)    //�z�M�Ȃ����邩�H
//�S����(�T�����̂ݗL���ł��I)
//���������ꂼ��h���X�A�b�v��E�V���[��̂ݗL���ȕ�������̂ŗL��ȊO�g�p�֎~
#define MUSICAL_VALUE_WR_SELF_IDX  (30) //�����̎Q���ԍ�
#define MUSICAL_VALUE_WR_MAX_POINT (31) //�ō��]���_
#define MUSICAL_VALUE_WR_MIN_POINT (32) //�Œ�]���_
#define MUSICAL_VALUE_WR_POINT     (33) //�l���_
#define MUSICAL_VALUE_WR_GRADE_MSG (34) //�]�����b�Z�[�W�̎擾
#define MUSICAL_VALUE_WR_POS_TO_IDX (35) //�����ʒu�ɑΉ������Q���ԍ�
#define MUSICAL_VALUE_WR_MAX_CONDITION (36) //���ڂ̍����R���f�B�V����
#define MUSICAL_VALUE_WR_POS_TO_RANK (37) //���ʂɑΉ������Q���ԍ�
#define MUSICAL_VALUE_WR_POKE_MAX_POINT_CON (38) //�ł��_�����������R���f�B�V����
#define MUSICAL_VALUE_WR_POS_TO_OBJ_VIEW (39) //�����ʒu�ɉ�����������(�v���C���[�̎��͏���

//_GET_MUSICAL_FAN_VALUE �p
#define MUSICAL_VALUE_FAN_TYPE (0)  //������
#define MUSICAL_VALUE_FAN_CHEER_MSG (1) //�������b�Z�[�W
#define MUSICAL_VALUE_FAN_GIFT_MSG  (2) //�v���[���g���b�Z�[�W
#define MUSICAL_VALUE_FAN_GIFT_TYPE (3) //�v���[���g���
#define MUSICAL_VALUE_FAN_GIFT_NUMBER (4) //�v���[���g�ԍ�

//_MUSICAL_TOOLS �p

#define MUSICAL_TOOL_INIT (0) //�~���[�W�J�������̊J�n
#define MUSICAL_TOOL_EXIT (1) //�~���[�W�J�������̏I��

#define MUSICAL_TOOL_COMM_INIT (10) //�ʐM�J�n
#define MUSICAL_TOOL_COMM_EXIT (11) //�ʐM�I��
#define MUSICAL_TOOL_COMM_PARENT_CONNECT (12) //�e�@�ڑ�
#define MUSICAL_TOOL_COMM_CHILD_CONNECT (13) //�q�@�ڑ�
#define MUSICAL_TOOL_COMM_TIMESYNC (14)   //�ʐM����
#define MUSICAL_TOOL_COMM_WAIT_POST_PROGRAM (15)   //�v���O������M�҂�
#define MUSICAL_TOOL_COMM_WAIT_NET_INIT (16)  //�ʐM�������҂�
#define MUSICAL_TOOL_COMM_WAIT_POST_ALLPOKE (17)  //�|�P��M�҂�
#define MUSICAL_TOOL_COMM_IR_CONNECT (18) //�ԊO���ڑ�
#define MUSICAL_TOOL_COMM_CHECK_ERROR (19) //�G���[�`�F�b�N

#define MUSICAL_TOOL_PRINT (100) //�f�o�b�O�o��

//�ʐM�G���g���[�p�Ԃ�l
#define MUSICAL_COMM_ENTRY_PARENT_OK (0)
#define MUSICAL_COMM_ENTRY_PARENT_CANCEL (1)
#define MUSICAL_COMM_ENTRY_PARENT_ERROR (2)
#define MUSICAL_COMM_ENTRY_CHILD_OK (3)
#define MUSICAL_COMM_ENTRY_CHILD_CANCEL (4)
#define MUSICAL_COMM_ENTRY_IR_OK (5)
#define MUSICAL_COMM_ENTRY_IR_CANCEL (6)

//�^�C�~���O�V���N�p�ԍ�
#define MUSICAL_COMM_SYNC_FIRST (10)
#define MUSICAL_COMM_SYNC_DRESSUP_START (11)