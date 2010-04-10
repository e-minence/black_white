//======================================================================
/**
 * @file  fieldobj_code.h
 * @brief  �t�B�[���h���샂�f���Ŏg�p����V���{��,�R�[�h�錾�B
 * �A�Z���u���\�[�X�ǂݍ��݃A�� enum�֎~
 * @author  kagaya
 * @date  05.07.20
 */
//======================================================================
#ifndef MMDL_CODE_H_FILE
#define MMDL_CODE_H_FILE

//======================================================================
//  ����
//======================================================================
#if 0 //field_dir.h��
#define DIR_UP      (0)          ///<�����
#define DIR_DOWN    (1)          ///<������
#define DIR_LEFT    (2)          ///<������
#define DIR_RIGHT    (3)          ///<�E����
#define DIR_LEFTUP    (4)          ///<�������
#define DIR_RIGHTUP    (5)          ///<�E�����
#define DIR_LEFTDOWN  (6)          ///<��������
#define DIR_RIGHTDOWN  (7)          ///<�E������
#define DIR_MAX      (8)          ///<�����ő�
#define DIR_NOT      (9)          ///<����������
#define DIR_MAX4    (DIR_RIGHT+1)    ///<�l�����ő�
#endif

//======================================================================
//  ����ID
//======================================================================
//--------------------------------------------------------------
//  �t�B�[���h���샂�f�� ����ID
//--------------------------------------------------------------
#define MMDL_ID_PLAYER   (0xff)  ///<���@��pID
#define MMDL_ID_PAIR     (0xfe)  ///<�A�������pID
#define MMDL_ID_GIMMICK  (0xfd)  ///<�W�����̎d�|����pID
#define MMDL_ID_COMMACTOR (0xfc) ///<�ʐM�A�N�^�[��pID
#define MMDL_ID_EVENTOBJ_00 (0xfb) ///<�X�N���v�g�C�x���g���Œǉ������OBJID

//--------------------------------------------------------------
//  �G�C���A�XID ��:PL script_def.h
//--------------------------------------------------------------
#define SP_SCRID_ALIES (0xffff)

//======================================================================
//  �t�B�[���h���샂�f�� ����R�[�h
//======================================================================
//--------------------------------------------------------------
//  �t�B�[���h���샂�f�� ����R�[�h
//--------------------------------------------------------------
#define MV_DMY      0x00    ///<���얳��
#define MV_PLAYER    0x01    ///<���@��p
#define MV_DIR_RND    0x02    ///<�����_���ɕ����؂�ւ�
#define MV_RND      0x03    ///<�����_���Ɉړ�
#define MV_RND_V    0x04    ///<�����_���ɏc�ړ�
#define MV_RND_H    0x05    ///<�����_���ɉ��ړ�
#define MV_RND_UL    0x06    ///<�����_���ɏ㍶�����؂�ւ�
#define MV_RND_UR    0x07    ///<�����_���ɏ�E�����؂�ւ�
#define MV_RND_DL    0x08    ///<�����_���ɉ��������؂�ւ�
#define MV_RND_DR    0x09    ///<�����_���ɉ��E�����؂�ւ�
#define MV_RND_UDL    0x0a    ///<�����_���ɏ㉺�������؂�ւ�
#define MV_RND_UDR    0x0b    ///<�����_���ɏ㉺�E�����؂�ւ�
#define MV_RND_ULR    0x0c    ///<�����_���ɏ㍶�E�����؂�ւ�
#define MV_RND_DLR    0x0d    ///<�����_���ɉ����E�����؂�ւ�
#define MV_UP      0x0e    ///<��Œ�
#define MV_DOWN      0x0f    ///<���Œ�
#define MV_LEFT      0x10    ///<���Œ�
#define MV_RIGHT    0x11    ///<�E�Œ�
#define MV_SPIN_L    0x12    ///<����]
#define MV_SPIN_R    0x13    ///<�E��]
#define MV_RT2      0x14    ///<���[�g2
#define MV_RTURLD       0x15        ///<���[�g3 ��E����
#define MV_RTRLDU       0x16        ///<���[�g3 �E������
#define MV_RTDURL       0x17        ///<���[�g3 ����E��
#define MV_RTLDUR       0x18        ///<���[�g3 ������E
#define MV_RTULRD       0x19        ///<���[�g3 �㍶�E��
#define MV_RTLRDU       0x1a        ///<���[�g3 ���E����
#define MV_RTDULR       0x1b        ///<���[�g3 ���㍶�E
#define MV_RTRDUL       0x1c        ///<���[�g3 �E���㍶
#define MV_RTLUDR       0x1d        ///<���[�g3 ���㉺�E
#define MV_RTUDRL       0x1e        ///<���[�g3 �㉺�E��
#define MV_RTRLUD       0x1f        ///<���[�g3 �E���㉺
#define MV_RTDRLU       0x20        ///<���[�g3 ���E����
#define MV_RTRUDL       0x21        ///<���[�g3 �E�㉺��
#define MV_RTUDLR       0x22        ///<���[�g3 �㉺���E
#define MV_RTLRUD       0x23        ///<���[�g3 ���E�㉺
#define MV_RTDLRU       0x24        ///<���[�g3 �����E��
#define MV_RTUL         0x25        ///<���[�g4 �㍶
#define MV_RTDR         0x26        ///<���[�g4 ���E
#define MV_RTLD         0x27        ///<���[�g4 ����
#define MV_RTRU         0x28        ///<���[�g4 �E��
#define MV_RTUR         0x29        ///<���[�g4 ��E
#define MV_RTDL         0x2a        ///<���[�g4 ����
#define MV_RTLU         0x2b        ///<���[�g4 ����
#define MV_RTRD         0x2c        ///<���[�g4 �E��
#define MV_RND_UD    0x2d    ///<�����_���ɏ㉺�؂�ւ�
#define MV_RND_LR    0x2e    ///<�����_���ɍ��E�؂�ւ�
#define MV_SEED      0x2f    ///<�؂̎�
#define MV_PAIR      0x30    ///<�A�����
#define MV_REWAR    0x31    ///<�Đ�
#define MV_TR_PAIR    0x32    ///<�g���[�i�[�A�����
#define MV_HIDE_SNOW  0x33    ///<�B�ꖪ�@��
#define MV_HIDE_SAND  0x34    ///<�B�ꖪ�@��
#define MV_HIDE_GRND  0x35    ///<�B�ꖪ�@�y
#define MV_HIDE_KUSA  0x36    ///<�B�ꖪ�@��
#define MV_COPYU    0x37    ///<���̂܂ˏ�
#define MV_COPYD    0x38    ///<���̂܂ˉ�
#define MV_COPYL    0x39    ///<���̂܂ˍ�
#define MV_COPYR    0x3a    ///<���̂܂ˉE
#define MV_COPYLGRASSU  0x3b    ///<���̂܂ˏ�
#define MV_COPYLGRASSD  0x3c    ///<���̂܂ˉ�
#define MV_COPYLGRASSL  0x3d    ///<���̂܂ˍ�
#define MV_COPYLGRASSR  0x3e    ///<���̂܂ˉE
#define MV_ALONGW_L    0x3f    ///<�ǉ�����(m
#define MV_ALONGW_R    0x40    ///<�ǉ����E(m
#define MV_ALONGW_LRL  0x41    ///<�ǉ������荶(m
#define MV_ALONGW_LRR  0x42    ///<�ǉ�������E(m
#define MV_RND_H_LIM  0x43 ///<�����_���ɉ��ړ� �ړ��`�F�b�N�ړ������̂�
#define MV_COMM_ACTOR 0x44 ///<�ʐM�A�N�^�[����
#define MV_RAIL_DMY 0x45 ///<���[���}�b�v�@����Ȃ�
#define MV_RAIL_DIR_RND_ALL    0x46    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_UL    0x47    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_UR    0x48    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_DL    0x49    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_DR    0x4a    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_UDL    0x4b    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_UDR    0x4c    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_ULR    0x4d    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_DLR    0x4e    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_UD    0x4f    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_DIR_RND_LR    0x50    ///<���[���}�b�v�@�����_���ɕ����؂�ւ�
#define MV_RAIL_RND      0x51    ///<�����_���Ɉړ�    (�ړ���������)
#define MV_RAIL_RND_V    0x52    ///<�����_���ɏc�ړ�  (�ړ���������)
#define MV_RAIL_RND_H    0x53    ///<�����_���ɉ��ړ�  (�ړ���������) 
#define MV_RAIL_RT2    0x54    ///<���[�g2����
#define MV_CODE_MAX    0x55    ///<�R�[�h�ő�i�v���O���������p
#define MV_CODE_NOT    0xff    ///<�񓮍�R�[�h(�v���O���������p

/*�@���Ή�
#define MV_SLEEP  ///<����@�_�b�V���ŋN����
#define MV_DIVE    ///<�f����

#define MV_HIDE_SKY      ///<�B�ꖪ�@��
#define MV_HIDE_SWIM    ///<�B�ꖪ�@�C
#define MV_HIDE_SNOWMAN    ///<�B�ꖪ�@�Ⴞ���
*/

/*
 �T�u����őΉ��������
�@1:�Ƃ܂��Ă���낫��� ok
�@�@�����_���ړ�
�@�@���[�g�Q�C�R�C�S�@
�@2�F�Ƃ܂��Ă��邭�� ok
�@�@�����_���ړ�
�@�@���[�g�Q�C�R�C�S�@
  
�@3�F���邭��ړ� ok
�@�@�����_���ړ�
�@�@���[�g�Q�C�R�C�S�@
�@4�F������ ok
�@�@���[�g�S�@
�@5�F���j��
�@�@���[�g�Q?
 * */

//======================================================================
//  �t�B�[���h���샂�f�� �C�x���g�^�C�v
//  �^�C�v�𑝂₵���ۂ͈ȉ��̕ύX���K�v�B
//  �Efldmmdl_move_1����`�̃f�[�^�e�[�u���̍X�V
//  �E�g���[�i�[�����`�F�b�N�̃g���[�i�[�`�F�b�N�X�V
//======================================================================
//--------------------------------------------------------------
///  ���샂�f�� �C�x���g�^�C�v�@���ɖ���
//--------------------------------------------------------------
#define EV_TYPE_NORMAL 0x00

//--------------------------------------------------------------
/// ���샂�f�� �C�x���g�^�C�v�@�g���[�i�[
/// Param0�Ɏ�������
//--------------------------------------------------------------
#define EV_TYPE_TR 0x01
#define EV_TYPE_TRAINER (0x01) //old

//--------------------------------------------------------------
///  ���샂�f���@�C�x���g�^�C�v�@�g���[�i�[�S���ʌ��m
///  Param0�Ɏ�������
//--------------------------------------------------------------
#define EV_TYPE_TR_EYEALL 0x02
#define EV_TYPE_TRAINER_EYEALL (0x02) //old

//--------------------------------------------------------------
///  ���샂�f���@�C�x���g�^�C�v�@�A�C�e��
//--------------------------------------------------------------
#define EV_TYPE_ITEM 0x03

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�g���[�i�[����낫���
/// Param0�Ɏ��������B
/// Param1�ɂ���낫�����J�n����O���b�h���B
//--------------------------------------------------------------
#define EV_TYPE_TR_KYORO 0x04 ///<�g���[�i�[�^�C�v�@����낫���
#define EV_TYPE_TRAINER_KYORO (0x04)

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�g���[�i�[��~��]
/// Param0�Ɏ��������B
/// Param1�ɉ�]���J�n����J�n����O���b�h���B
//--------------------------------------------------------------
#define EV_TYPE_TR_SPINS_L 0x05 ///<�g���[�i�[�^�C�v��~��]������
#define EV_TYPE_TR_SPINS_R 0x06 ///<�g���[�i�[�^�C�v��~��]�E���
#define EV_TYPE_TR_SPINM_L 0x07 ///<�g���[�i�[�^�C�v�ړ���]������
#define EV_TYPE_TR_SPINM_R 0x08 ///<�g���[�i�[�^�C�v�ړ���]�E���

//old
#define EV_TYPE_TRAINER_SPIN_STOP_L (0x05)
#define EV_TYPE_TRAINER_SPIN_STOP_R (0x06)
#define EV_TYPE_TRAINER_SPIN_MOVE_L (0x07)
#define EV_TYPE_TRAINER_SPIN_MOVE_R (0x08)

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@���b�Z�[�WID�w��
//--------------------------------------------------------------
#define EV_TYPE_MSG 0x09

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�����������Ɏ��@�������瓦����
/// Param0�Ɏ��������B
/// �S�_�ړ��n�̂݁B�����q�b�g�`�F�b�N�Ώۂł͂Ȃ��B
//--------------------------------------------------------------
#define EV_TYPE_TR_ESC 0x0a
#define EV_TYPE_TRAINER_ESCAPE (0x0a) //old

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�_�b�V���ɔ������ĕ����ύX
/// Param0�Ɏ��������B
/// �Q�����؂�ւ��A�R�����؂�ւ��ASPIN�^�C�v����
//--------------------------------------------------------------
#define EV_TYPE_TR_D_ALT 0x0b
#define EV_TYPE_TRAINER_DASH_ALTER (0x0b) //old

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�_�b�V���ɔ������ē���
/// Param0�Ɏ��������B
/// Param1�Ɉړ�����O���b�h���B
//--------------------------------------------------------------
#define EV_TYPE_TR_D_REA 0x0c
#define EV_TYPE_TRAINER_DASH_REACT (0x0c) //old

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�_�b�V���ɔ������Ĉړ����x�A�b�v
/// Param0�Ɏ��������B
//--------------------------------------------------------------
#define EV_TYPE_TR_D_ACC 0x0d
#define EV_TYPE_TRAINER_DASH_ACCEL (0x0d) //old

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�g���[�i�[����낫���@�_�b�V���ő��xUp��
/// Param0�Ɏ��������B
/// Param1�ɂ���낫�����J�n����O���b�h���B
//--------------------------------------------------------------
#define EV_TYPE_TR_D_KYORO (0x0e)
#define EV_TYPE_TRAINER_DASH_ACCEL_KYORO (0x0e) //old

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�g���[�i�[��~��] �_�b�V���ő��xUp��
/// Param0�Ɏ��������B
/// Param1�ɉ�]���J�n����J�n����O���b�h���B
//--------------------------------------------------------------
#define EV_TYPE_TR_D_SPINS_L 0x0f ///<�g���[�i�[�^�C�v��~��]������
#define EV_TYPE_TR_D_SPINS_R 0x10 ///<�g���[�i�[�^�C�v��~��]�E���
#define EV_TYPE_TR_D_SPINM_L 0x11 ///<�g���[�i�[�^�C�v�ړ���]������
#define EV_TYPE_TR_D_SPINM_R 0x12 ///<�g���[�i�[�^�C�v�ړ���]�E���

//old
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_L (0x0f)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_STOP_R (0x10)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_L (0x11)
#define EV_TYPE_TRAINER_DASH_ACCEL_SPIN_MOVE_R (0x12)

//--------------------------------------------------------------
/// ���샂�f���@�C�x���g�^�C�v�@�ő吔
//--------------------------------------------------------------
#define EV_TYPE_MAX (0x13)

//======================================================================
//  �t�B�[���h���샂�f�� �`��X�e�[�^�X
//======================================================================
//--------------------------------------------------------------
///  �t�B�[���h���샂�f�� �`��X�e�[�^�X�@��{
//--------------------------------------------------------------
#define DRAW_STA_STOP     0x00    ///<��~
#define DRAW_STA_WALK_32F 0x01    ///<�ړ� 32�t���[��
#define DRAW_STA_WALK_16F 0x02    ///<�ړ� 16�t���[��
#define DRAW_STA_WALK_8F  0x03    ///<�ړ� 8�t���[��
#define DRAW_STA_WALK_4F  0x04    ///<�ړ� 4�t���[��
#define DRAW_STA_WALK_2F  0x05    ///<�ړ� 2�t���[��
#define DRAW_STA_WALK_6F  0x06    ///<�ړ� 6�t���[��
#define DRAW_STA_WALK_3F  0x07    ///<�ړ� 3�t���[��
#define DRAW_STA_WALK_7F  0x08    ///<�ړ� 7�t���[��
#define DRAW_STA_WALK_12F 0x09    ///<�ړ� 10�t���[��
#define DRAW_STA_MAX      0x0a    ///<�`��X�e�[�^�X�ő�

///�`��X�e�[�^�X�l������
#define DRAW_STA_DIR4_MAX (DRAW_STA_MAX*DIR_MAX4)

//--------------------------------------------------------------
//  ��{+���Z��p�X�e�[�^�X
//--------------------------------------------------------------
//�P���
#define DRAW_STA_ACT0 (DRAW_STA_MAX)
#define DRAW_STA_ACT0_ANMNO_MAX ((DRAW_STA_MAX*DIR_MAX4)+1)

//�Q���
#define DRAW_STA_ACT1 (DRAW_STA_ACT0+1)
#define DRAW_STA_ACT1_ANMNO_MAX (DRAW_STA_ACT0_ANMNO_MAX+1)

//--------------------------------------------------------------
///  PC��t��p�X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_PC_BOW (DRAW_STA_ACT0)
#define DRAW_STA_PCWOMAN_DIR4_MAX (DRAW_STA_DIR4_MAX+1)

//--------------------------------------------------------------
///  �g����l����p�X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_SWIMHERO_MAX (4)

//--------------------------------------------------------------
/// �A�C�e���Q�b�g�p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_ITEMGET_STOP 0x00 ///<��~
#define DRAW_STA_ITEMGET_ANM 0x01 ///<�A�j��
#define DRAW_STA_ITEMGET_MAX 0x02 ///<�f�[�^�C���f�b�N�X�ő�

//--------------------------------------------------------------
/// PC�a���A�j���p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_PCAZUKE_STOP 0x00 ///<��~
#define DRAW_STA_PCAZUKE_ANM0 0x01 ///<�a���A�j��
#define DRAW_STA_PCAZUKE_ANM1 0x02 ///<�󂯎��A�j��
#define DRAW_STA_PCAZUKE_MAX 0x03 ///<�ő�

//--------------------------------------------------------------
/// ���@���|�[�g�p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_SAVEHERO 0x00
#define DRAW_STA_SAVEHERO_MAX 0x01

//--------------------------------------------------------------
/// �t�B�[���h���샂�f�� ���@��p�`��X�e�[�^�X
/// ��{�����n��DRAW_STA_WALK_7F�܂œ���
//--------------------------------------------------------------
#define DRAW_STA_HERO_DASH_4F (DRAW_STA_MAX) ///<�ړ��@�_�b�V�� 4�t���[��
#define DRAW_STA_HERO_DASH_6F (DRAW_STA_MAX+1) ///<�ړ��@�_�b�V�� 6�t���[��
#define DRAW_STA_HERO_MAX (DRAW_STA_HERO_DASH_6F+1) ///���@�ő�

//�`��X�e�[�^�X4������ ���@
#define DRAW_STA_HERO_DIR4_MAX (DRAW_STA_HERO_MAX*DIR_MAX4)

//--------------------------------------------------------------
/// �t�B�[���h���샂�f�� ���]�Ԏ��@��p�`��X�e�[�^�X
//--------------------------------------------------------------
#if 0 //wb null
///���@�@�W�����v��
#define DRAW_STA_TAKE_OFF_16F  (DRAW_STA_MAX)
///���@�@�W�����v��
#define DRAW_STA_TAKE_OFF_8F  (DRAW_STA_MAX+1)
///�`��X�e�[�^�X�ő�@���@
#define DRAW_STA_MAX_CYCLEHERO  (DRAW_STA_TAKE_OFF_8F+1)
#endif

//--------------------------------------------------------------
///  �t�B�[���h���샂�f�� ���@�ނ��p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_FISH_START   0x00
#define DRAW_STA_FISH_END     0x01
#define DRAW_STA_FISH_ON      0x02
#define DRAW_STA_FISH_HIT     0x03
#define DRAW_STA_FISH_MAX     0x04

///4�������ő�
#define DRAW_STA_FISH_DIR4_MAX (DRAW_STA_FISH_MAX*DIR_MAX4)

//--------------------------------------------------------------
///  �t�B�[���h���샂�f�� ���@���ΐ�p�`��X�e�[�^�X
//--------------------------------------------------------------
#if 0 //wb null
#define DRAW_STA_BANZAI   0x00
#define DRAW_STA_BANZAI_UKE  0x01
#define DRAW_STA_MAX_BANZAI (DRAW_STA_BANZAI_UKE+1)
#endif

//--------------------------------------------------------------
///  �t�B�[���h���샂�f�� �V���E���`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_SHINMU_A_STOP_DOWN 0x00
#define DRAW_STA_SHINMU_A_GUTARI (DRAW_STA_MAX)
#define DRAW_STA_SHINMU_A_FLY_UP (DRAW_STA_MAX+1)
#define DRAW_STA_SHINMU_A_FLY (DRAW_STA_MAX+2)
#define DRAW_STA_SHINMU_A_MAX (DRAW_STA_MAX+3)

#define DRAW_STA_SHINMU_A_ANMNO_STOP   0x00
#define DRAW_STA_SHINMU_A_ANMNO_GUTARI 0x01
#define DRAW_STA_SHINMU_A_ANMNO_FLY_UP 0x02
#define DRAW_STA_SHINMU_A_ANMNO_FLY    0x03
#define DRAW_STA_SHINMU_A_ANMNO_MAX    0x04

#define DRAW_STA_SHINMU_B_STOP 0x00
#define DRAW_STA_SHINMU_B_HOERU     (DRAW_STA_MAX)
#define DRAW_STA_SHINMU_B_TURN      (DRAW_STA_MAX+1)
#define DRAW_STA_SHINMU_B_MAX (DRAW_STA_MAX+2)

#define DRAW_STA_SHINMU_B_ANMNO_STOP_U   0x00
#define DRAW_STA_SHINMU_B_ANMNO_STOP_D   0x01
#define DRAW_STA_SHINMU_B_ANMNO_HOERU    0x02
#define DRAW_STA_SHINMU_B_ANMNO_TURN     0x03
#define DRAW_STA_SHINMU_B_ANMNO_MAX      0x04

//--------------------------------------------------------------
///  �t�B�[���h���샂�f�� �����̑���p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_SPIDER_STOP 0x00
#define DRAW_STA_SPIDER_REMOVE 0x01
#define DRAW_STA_SPIDER_MAX 0x02

//--------------------------------------------------------------
/// �t�B�[���h���샂�f�� �����f�B�A��p�`��X�e�[�^�X
//--------------------------------------------------------------
#define DRAW_STA_MELODYER_STOP 0x00
#define DRAW_STA_MELODYER_SPIN (DRAW_STA_MAX)
#define DRAW_STA_MELODYER_SPIN_POSE (DRAW_STA_MAX+1)
#define DRAW_STA_MELODYER_SHAKE (DRAW_STA_MAX+2)
#define DRAW_STA_MELODYER_MAX (DRAW_STA_MAX+3)

#define DRAW_STA_MELODYER_ANMNO_STOP 0x00
#define DRAW_STA_MELODYER_ANMNO_SPIN 0x01
#define DRAW_STA_MELODYER_ANMNO_SPIN_POSE 0x02
#define DRAW_STA_MELODYER_ANMNO_SHAKE 0x03
#define DRAW_STA_MELODYER_ANMNO_MAX 0x04

//======================================================================
//  �t�B�[���h���샂�f�� �A�j���[�V�����R�}���h�R�[�h
//======================================================================
//--------------------------------------------------------------
//  �t�B�[���h���샂�f�� �A�j���[�V�����R�}���h�R�[�h
//--------------------------------------------------------------
#define AC_DIR_U 0x00 ///<��Ɍ��� [rail]
#define AC_DIR_D 0x01 ///<���Ɍ��� [rail]
#define AC_DIR_L 0x02 ///<���Ɍ��� [rail]
#define AC_DIR_R 0x03 ///<�E�Ɍ��� [rail]
#define AC_WALK_U_32F 0x04 ///<��Ɉړ� 32�t���[��
#define AC_WALK_D_32F 0x05 ///<���Ɉړ� 32�t���[��
#define AC_WALK_L_32F 0x06 ///<���Ɉړ� 32�t���[��
#define AC_WALK_R_32F 0x07 ///<�E�Ɉړ� 32�t���[��
#define AC_WALK_U_16F 0x08 ///<��Ɉړ� 16�t���[�� [rail]
#define AC_WALK_D_16F 0x09 ///<���Ɉړ� 16�t���[�� [rail]
#define AC_WALK_L_16F 0x0a ///<���Ɉړ� 16�t���[�� [rail]
#define AC_WALK_R_16F 0x0b ///<�E�Ɉړ� 16�t���[�� [rail]
#define AC_WALK_U_8F 0x0c ///<��Ɉړ� 8�t���[�� [rail]
#define AC_WALK_D_8F 0x0d ///<���Ɉړ� 8�t���[�� [rail]
#define AC_WALK_L_8F 0x0e ///<���Ɉړ� 8�t���[�� [rail]
#define AC_WALK_R_8F 0x0f ///<�E�Ɉړ� 8�t���[�� [rail]
#define AC_WALK_U_4F 0x10 ///<��Ɉړ� 4�t���[�� [rail]
#define AC_WALK_D_4F 0x11 ///<���Ɉړ� 4�t���[�� [rail]
#define AC_WALK_L_4F 0x12 ///<���Ɉړ� 4�t���[�� [rail]
#define AC_WALK_R_4F 0x13 ///<�E�Ɉړ� 4�t���[�� [rail]
#define AC_WALK_U_2F 0x14 ///<��Ɉړ� 2�t���[�� [rail]
#define AC_WALK_D_2F 0x15 ///<���Ɉړ� 2�t���[�� [rail]
#define AC_WALK_L_2F 0x16 ///<���Ɉړ� 2�t���[�� [rail]
#define AC_WALK_R_2F 0x17 ///<�E�Ɉړ� 2�t���[�� [rail]
#define AC_STAY_WALK_U_32F 0x18 ///<������ɂ��̏���� 32�t���[��
#define AC_STAY_WALK_D_32F 0x19 ///<�������ɂ��̏���� 32�t���[��
#define AC_STAY_WALK_L_32F 0x1a ///<�������ɂ��̏���� 32�t���[��
#define AC_STAY_WALK_R_32F 0x1b ///<�E�����ɂ��̏���� 32�t���[��
#define AC_STAY_WALK_U_16F 0x1c ///<������ɂ��̏���� 16�t���[�� [rail]
#define AC_STAY_WALK_D_16F 0x1d ///<�������ɂ��̏���� 16�t���[�� [rail]
#define AC_STAY_WALK_L_16F 0x1e ///<�������ɂ��̏���� 16�t���[�� [rail]
#define AC_STAY_WALK_R_16F 0x1f ///<�E�����ɂ��̏���� 16�t���[�� [rail]
#define AC_STAY_WALK_U_8F 0x20 ///<������ɂ��̏���� 8�t���[�� [rail]
#define AC_STAY_WALK_D_8F 0x21 ///<�������ɂ��̏���� 8�t���[�� [rail]
#define AC_STAY_WALK_L_8F 0x22 ///<�������ɂ��̏���� 8�t���[�� [rail]
#define AC_STAY_WALK_R_8F 0x23 ///<�E�����ɂ��̏���� 8�t���[�� [rail]
#define AC_STAY_WALK_U_4F 0x24 ///<������ɂ��̏���� 4�t���[��
#define AC_STAY_WALK_D_4F 0x25 ///<�������ɂ��̏���� 4�t���[��
#define AC_STAY_WALK_L_4F 0x26 ///<�������ɂ��̏���� 4�t���[��
#define AC_STAY_WALK_R_4F 0x27 ///<�E�����ɂ��̏���� 4�t���[��
#define AC_STAY_WALK_U_2F 0x28 ///<������ɂ��̏���� 2�t���[�� [rail]
#define AC_STAY_WALK_D_2F 0x29 ///<�������ɂ��̏���� 2�t���[�� [rail]
#define AC_STAY_WALK_L_2F 0x2a ///<�������ɂ��̏���� 2�t���[�� [rail]
#define AC_STAY_WALK_R_2F 0x2b ///<�E�����ɂ��̏���� 2�t���[�� [rail]
#define AC_STAY_JUMP_U_16F 0x2c ///<������ɂ��̏�W�����v 16�t���[��
#define AC_STAY_JUMP_D_16F 0x2d ///<�������ɂ��̏�W�����v 16�t���[��
#define AC_STAY_JUMP_L_16F 0x2e ///<�������ɂ��̏�W�����v 16�t���[��
#define AC_STAY_JUMP_R_16F 0x2f ///<�E�����ɂ��̏�W�����v 16�t���[��
#define AC_STAY_JUMP_U_8F 0x30 ///<������ɂ��̏�W�����v 8�t���[��
#define AC_STAY_JUMP_D_8F 0x31 ///<�������ɂ��̏�W�����v 8�t���[��
#define AC_STAY_JUMP_L_8F 0x32 ///<�������ɂ��̏�W�����v 8�t���[��
#define AC_STAY_JUMP_R_8F 0x33 ///<�E�����ɂ��̏�W�����v 8�t���[��
#define AC_JUMP_U_1G_8F 0x34 ///<������ɃW�����v 1grid 8�t���[�� [rail]
#define AC_JUMP_D_1G_8F 0x35 ///<�������ɃW�����v 1grid 8�t���[�� [rail]
#define AC_JUMP_L_1G_8F 0x36 ///<�������ɃW�����v 1grid 8�t���[�� [rail]
#define AC_JUMP_R_1G_8F 0x37 ///<�E�����ɃW�����v 1grid 8�t���[�� [rail]
#define AC_JUMP_U_2G_16F 0x38 ///<������ɃW�����v 2grid 16�t���[�� [rail]
#define AC_JUMP_D_2G_16F 0x39 ///<�������ɃW�����v 2grid 16�t���[�� [rail]
#define AC_JUMP_L_2G_16F 0x3a ///<�������ɃW�����v 2grid 16�t���[�� [rail]
#define AC_JUMP_R_2G_16F 0x3b ///<�E�����ɃW�����v 2grid 16�t���[�� [rail]
#define AC_WAIT_1F 0x3c ///<1�t���[���҂� [rail]
#define AC_WAIT_2F 0x3d ///<2�t���[���҂� [rail]
#define AC_WAIT_4F 0x3e ///<4�t���[���҂� [rail]
#define AC_WAIT_8F 0x3f ///<8�t���[���҂� [rail]
#define AC_WAIT_15F 0x40 ///<15�t���[���҂� [rail]
#define AC_WAIT_16F 0x41 ///<16�t���[���҂� [rail]
#define AC_WAIT_32F 0x42 ///<32�t���[���҂� [rail]
#define AC_WARP_UP 0x43 ///<��Ƀ��[�v
#define AC_WARP_DOWN 0x44 ///<���Ƀ��[�v
#define AC_VANISH_ON 0x45 ///<��\���ɂ��� [rail]
#define AC_VANISH_OFF 0x46 ///<�\���ɂ��� [rail]
#define AC_DIR_PAUSE_ON 0x47 ///<�����Œ肷�� [rail]
#define AC_DIR_PAUSE_OFF 0x48 ///<�����Œ肵�Ȃ� [rail]
#define AC_ANM_PAUSE_ON 0x49 ///<�A�j���|�[�Y [rail]
#define AC_ANM_PAUSE_OFF 0x4a ///<�A�j���|�[�Y������ [rail]
#define AC_MARK_GYOE 0x4b ///<!�}�[�N���o�� [rail]
#define AC_WALK_U_6F 0x4c ///<������Ɉړ� 6�t���[��
#define AC_WALK_D_6F 0x4d ///<�������Ɉړ� 6�t���[��
#define AC_WALK_L_6F 0x4e ///<�������Ɉړ� 6�t���[��
#define AC_WALK_R_6F 0x4f ///<�E�����Ɉړ� 6�t���[��
#define AC_WALK_U_3F 0x50 ///<������Ɉړ� 3�t���[��
#define AC_WALK_D_3F 0x51 ///<�������Ɉړ� 3�t���[��
#define AC_WALK_L_3F 0x52 ///<�������Ɉړ� 3�t���[��
#define AC_WALK_R_3F 0x53 ///<�E�����Ɉړ� 3�t���[��
#define AC_WALK_U_1F 0x54 ///<������Ɉړ� 1�t���[��(�V�X�e�����Ή� �g�p�֎~
#define AC_WALK_D_1F 0x55 ///<�������Ɉړ� 1�t���[��(�V�X�e�����Ή� �g�p�֎~
#define AC_WALK_L_1F 0x56 ///<�������Ɉړ� 1�t���[��(�V�X�e�����Ή� �g�p�֎~
#define AC_WALK_R_1F 0x57 ///<�E�����Ɉړ� 1�t���[��(�V�X�e�����Ή� �g�p�֎~
#define AC_DASH_U_4F 0x58 ///<������Ɉړ� 4�t���[��(���@��p [rail]
#define AC_DASH_D_4F 0x59 ///<�������Ɉړ� 4�t���[��(���@��p [rail]
#define AC_DASH_L_4F 0x5a ///<�������Ɉړ� 4�t���[��(���@��p [rail]
#define AC_DASH_R_4F 0x5b ///<�E�����Ɉړ� 4�t���[��(���@��p [rail]
#define AC_JUMPHI_L_1G_16F 0x5c ///<�������ɍ����W�����v 1grid 16�t���[��(���@��p
#define AC_JUMPHI_R_1G_16F 0x5d ///<�E�����ɍ����W�����v 1grid 16�t���[��(���@��p
#define AC_JUMPHI_L_3G_32F 0x5e ///<�������ɍ����W�����v 3grid 32�t���[��(���@��p
#define AC_JUMPHI_R_3G_32F 0x5f ///<�E�����ɍ����W�����v 3grid 32�t���[��(���@��p
#define AC_WALK_U_7F 0x60 ///<������Ɉړ� 7�t���[��
#define AC_WALK_D_7F 0x61 ///<�������Ɉړ� 7�t���[��
#define AC_WALK_L_7F 0x62 ///<�������Ɉړ� 7�t���[��
#define AC_WALK_R_7F 0x63 ///<�E�����Ɉړ� 7�t���[��
#define AC_ACT0_STOP 0x64 ///<���Z(����OBJ��p
#define AC_HIDE_PULLOFF 0x65 ///<�B�ꖪ�E��(MV_HIDE��p
#define AC_HERO_BANZAI 0x66 ///<PC���荷�o(���@��p
#define AC_MARK_SAISEN 0x67 ///<�Đ�}�[�N�\��
#define AC_HERO_BANZAI_UKE 0x68 ///<PC���荷�o�󂯎��(���@��p
#define AC_WALKGL_U_8F 0x69 ///<���ǈړ� �� 8�t���[��
#define AC_WALKGL_D_8F 0x6a ///<���ǈړ� �� 8�t���[��
#define AC_WALKGL_L_8F 0x6b ///<���ǈړ� �� 8�t���[��
#define AC_WALKGL_R_8F 0x6c ///<���ǈړ� �E 8�t���[��
#define AC_WALKGR_U_8F 0x6d ///<�E�ǈړ� �� 8�t���[��
#define AC_WALKGR_D_8F 0x6e ///<�E�ǈړ� �� 8�t���[��
#define AC_WALKGR_L_8F 0x6f ///<�E�ǈړ� �� 8�t���[��
#define AC_WALKGR_R_8F 0x70 ///<�E�ǈړ� �E 8�t���[��
#define AC_WALKGU_U_8F 0x71 ///<��ǈړ� �� 8�t���[��
#define AC_WALKGU_D_8F 0x72 ///<��ǈړ� �� 8�t���[��
#define AC_WALKGU_L_8F 0x73 ///<��ǈړ� �� 8�t���[��
#define AC_WALKGU_R_8F 0x74 ///<��ǈړ� �E 8�t���[��
#define AC_JUMP_U_3G_24F 0x75 ///<������ɃW�����v 3grid 24�t���[��
#define AC_JUMP_D_3G_24F 0x76 ///<������ɃW�����v 3grid 24�t���[��
#define AC_JUMP_L_3G_24F 0x77 ///<������ɃW�����v 3grid 24�t���[��
#define AC_JUMP_R_3G_24F 0x78 ///<������ɃW�����v 3grid 24�t���[��
#define AC_DASHGL_U_4F 0x79 ///<���ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGL_D_4F 0x7a ///<���ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGL_L_4F 0x7b ///<���ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGL_R_4F 0x7c ///<���ǈړ� �E 4�t���[��(���@��p
#define AC_DASHGR_U_4F 0x7d ///<�E�ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGR_D_4F 0x7e ///<�E�ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGR_L_4F 0x7f ///<�E�ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGR_R_4F 0x80 ///<�E�ǈړ� �E 4�t���[��(���@��p
#define AC_DASHGU_U_4F 0x81 ///<��ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGU_D_4F 0x82 ///<��ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGU_L_4F 0x83 ///<��ǈړ� �� 4�t���[��(���@��p
#define AC_DASHGU_R_4F 0x84 ///<��ǈړ� �E 4�t���[��(���@��p
#define AC_JUMPGL_U_1G_8F 0x85 ///<���� ������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGL_D_1G_8F 0x86 ///<���� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGL_L_1G_8F 0x87 ///<���� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGL_R_1G_8F 0x88 ///<���� �E�����ɃW�����v 1grid 8�t���[��
#define AC_JUMPGR_U_1G_8F 0x89 ///<�E�� ������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGR_D_1G_8F 0x8a ///<�E�� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGR_L_1G_8F 0x8b ///<�E�� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGR_R_1G_8F 0x8c ///<�E�� �E�����ɃW�����v 1grid 8�t���[��
#define AC_JUMPGU_U_1G_8F 0x8d ///<��� ������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGU_D_1G_8F 0x8e ///<��� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGU_L_1G_8F 0x8f ///<��� �������ɃW�����v 1grid 8�t���[��
#define AC_JUMPGU_R_1G_8F 0x90 ///<��� �E�����ɃW�����v 1grid 8�t���[��
#define AC_WALKGU_U_4F 0x91 ///<��ǈړ� �� 4�t���[��
#define AC_WALKGU_D_4F 0x92 ///<��ǈړ� �� 4�t���[��
#define AC_WALKGU_L_4F 0x93 ///<��ǈړ� �� 4�t���[��
#define AC_WALKGU_R_4F 0x94 ///<��ǈړ� �E 4�t���[��
#define AC_WALKGU_U_2F 0x95 ///<��ǈړ� �� 2�t���[��
#define AC_WALKGU_D_2F 0x96 ///<��ǈړ� �� 2�t���[��
#define AC_WALKGU_L_2F 0x97 ///<��ǈړ� �� 2�t���[��
#define AC_WALKGU_R_2F 0x98 ///<��ǈړ� �E 2�t���[��
#define AC_MARK_GYOE_TWAIT 0x99 ///<!�}�[�N���o�� �O���t�B�b�N�]���҂���
#define AC_HERO_ITEMGET 0x9a ///<���@�A�C�e���Q�b�g�A�j�� [rail]
#define AC_SANDWALK_U_16F 0x9b ///<��Ɉړ� 16F�ړ��A�\���A�j����2F�A�j��
#define AC_SANDWALK_D_16F 0x9c ///<���Ɉړ� 16F�ړ��A�\���A�j����2F�A�j��
#define AC_SANDWALK_L_16F 0x9d ///<���Ɉړ� 16F�ړ��A�\���A�j����2F�A�j��
#define AC_SANDWALK_R_16F 0x9e ///<�E�Ɉړ� 16F�ړ��A�\���A�j����2F�A�j��
#define AC_MARK_HATENA 0x9f ///<�H�}�[�N���o��
#define AC_MARK_ONPU 0xa0 ///<��}�[�N���o��
#define AC_MARK_TENTEN 0xa1 ///<�c�}�[�N���o��
#define AC_HERO_CUTIN 0xa2 ///<���@�J�b�g�C���A�j��
#define AC_DASH_U_6F 0xa3 ///<������Ɉړ� 6�t���[��(���@��p
#define AC_DASH_D_6F 0xa4 ///<�������Ɉړ� 6�t���[��(���@��p
#define AC_DASH_L_6F 0xa5 ///<�������Ɉړ� 6�t���[��(���@��p
#define AC_DASH_R_6F 0xa6 ///<�E�����Ɉړ� 6�t���[��(���@��p
#define AC_WALK_U_12F 0xa7 ///<��Ɉړ� 10�t���[��
#define AC_WALK_D_12F 0xa8 ///<��Ɉړ� 10�t���[��
#define AC_WALK_L_12F 0xa9 ///<��Ɉړ� 10�t���[��
#define AC_WALK_R_12F 0xaa ///<��Ɉړ� 10�t���[��
#define AC_SPIDER_REMOVE 0xab ///<�����̑��r���A�j�� SPIDER��p
#define AC_SHIN_MU_HOERU 0xac ///<�V�����@�i����A�j��
#define AC_SHIN_MU_FLY_L 0xad ///<�V�����@��ԁ@��
#define AC_SHIN_MU_FLY_UPPER 0xae ///<�V�����@�㏸
#define AC_SHIN_MU_GUTARI 0xaf ///<�V�����@��������A�j��
#define AC_SHIN_MU_TURN 0xb0 ///<�V�����@�U�����
#define AC_MELODYER_SPIN 0xb1 ///<�����f�B�A ��]
#define AC_MELODYER_SPIN_POSE 0xb2 ///<�����f�B�A ��]+�|�[�Y
#define AC_MELODYER_SHAKE 0xb3 ///<�����f�B�A �㉺�h��
#define AC_SHIN_MU_OFFS_CLEAR 0xb4 ///<�V���� �`��I�t�Z�b�g���N���A
#define AC_ACT0_LOOP 0xb5 ///<���Z(����OBJ��p
#define AC_ACT1_STOP 0xb6 ///<���Z(����OBJ��p
#define AC_ACT1_LOOP 0xb7 ///<���Z(����OBJ��p
#define AC_NINJA_ACT0 0xb8 ///<���Z(NINJA��p �o���A�j��
#define ACMD_MAX 0xb9 ///<�A�j���[�V�����R�}���h�ő吔

#define ACMD_END 0xfe ///<�A�j���[�V�����R�}���h�I��
#define ACMD_NOT 0xff ///<��A�j���[�V�����R�}���h

//�ŋ��n�@����Ղ����O�Œu������
#define AC_PC_BOW (AC_ACT0_STOP) ///<PC�����V
#define AC_RIVEL_ACT0 (AC_ACT0_LOOP) ///<���C�o����p �ܐ�A�j��
#define AC_SUPPORT_ACT0 (AC_ACT0_STOP) ///<�T�|�[�g��p �X�q�A�j��
#define AC_N_ACT0 (AC_ACT0_STOP) ///<N��p ����A�j��
#define AC_N_ACT1 (AC_ACT1_STOP) ///<N��p ��A�j��
#define AC_SAGE1_ACT0 (AC_ACT0_STOP) ///<SAGE1��p ����A�j��
#define AC_NINJA_ACT1 (AC_ACT1_LOOP) ///<NINJA��p �����A�j��
#define AC_CHAMPION_ACT0 (AC_ACT0_STOP) ///<CHAMPION��p �����h��A�j��

#endif //MMDL_CODE_H_FILE
