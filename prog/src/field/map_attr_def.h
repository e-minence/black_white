///////////////////////////////////////////////////////////////////
/*
	@file		map_attr_def.h
	@brief	�A�g���r���[�g���x����`

	���̃t�@�C����wb_data3d/tool/attr.bat�R���o�[�^����o�͂���
	wb_data3d/field�t�H���_����蓮�ŃR�s�[���Ă��܂�
*/
///////////////////////////////////////////////////////////////////


#pragma once

#define MATTR_NORMAL              (0x00)		//�ړ��\
#define MATTR_NOT_MOVE            (0x01)		//�ړ��s��
#define MATTR_ZIMEN_S01           (0x02)		//�l�G�ω��n�ʂP
#define MATTR_ZIMEN_01            (0x03)		//�ʏ�n��
#define MATTR_E_GRASS_LOW         (0x04)		//���ނ�G���J�E���g��
#define MATTR_SNOW_GRASS_LOW      (0x05)		//����n�ё��ނ��
#define MATTR_E_GRASS_HIGH        (0x06)		//���ނ�G���J�E���g����
#define MATTR_SNOW_GRASS_HIGH     (0x07)		//����n�ё��ނ狭
#define MATTR_E_LGRASS_LOW        (0x08)		//�������i�G���J�E���g��j
#define MATTR_E_LGRASS_HIGH       (0x09)		//�������i�G���J�E���g���j
#define MATTR_E_ZIMEN_01          (0x0a)		//�n�ʁi�G���J�E���g�j
#define MATTR_DESERT_01           (0x0b)		//����
#define MATTR_E_DESERT_01         (0x0c)		//�����i�G���J�E���g�j
#define MATTR_E_ROOM_01           (0x0d)		//����
#define MATTR_SNOW_01             (0x0e)		//��
#define MATTR_SNOW_02             (0x0f)		//��Q
#define MATTR_ICE_02              (0x10)		//�X��Q
#define MATTR_NORMAL_02           (0x11)		//�ړ��\�Q
#define MATTR_NOT_MOVE_02         (0x12)		//�ړ��s�Q
#define MATTR_NORMAL_03           (0x13)		//�ړ��\�R
#define MATTR_POOL_01             (0x14)		//�����܂�i�l�G�Ȃ��j
#define MATTR_POOL_S01            (0x15)		//�����܂�i�l�G����j
#define MATTR_SHOAL_01            (0x17)		//��
#define MATTR_ICE_01              (0x18)		//�X��
#define MATTR_SAND_01             (0x19)		//���l
#define MATTR_WATERFALL_BACK      (0x1a)		//��̗�����n�`
#define MATTR_MIRROR_01           (0x1b)		//����
#define MATTR_MARSH_01            (0x1c)		//�󂢎���
#define MATTR_KAIRIKI_ANA         (0x1d)		//�����肫��
#define MATTR_ZIMEN_S02           (0x1e)		//�l�G�ω��n�ʂQ
#define MATTR_LAWN_01             (0x1f)		//�Ő�
#define MATTR_BRIDGE_01           (0x20)		//�G�t�F�N�g�G���J�E���g��
#define MATTR_ELECTORIC_FLOOR     (0x30)		//�r���r����
#define MATTR_FLOAT_FLOOR         (0x31)		//���V�Ώ��h��
#define MATTR_ELECTORIC_ROCK      (0x32)		//�d�C��
#define MATTR_UPDOWN_FLOOR        (0x33)		//���@�㉺�h��
#define MATTR_WATER_01            (0x3d)		//�W���i�l�G�ω��Ȃ��j
#define MATTR_WATER_S01           (0x3e)		//�W���i�l�G�ω�����j
#define MATTR_SEA_01              (0x3f)		//�C
#define MATTR_WATERFALL_01        (0x40)		//��
#define MATTR_SHORE_01            (0x41)		//��
#define MATTR_DEEP_MARSH_01       (0x42)		//�[������
#define MATTR_NMOVE_RIGHT         (0x51)		//�ǁi�E�����ֈړ��s�j
#define MATTR_NMOVE_LEFT          (0x52)		//�ǁi�������ֈړ��s�j
#define MATTR_NMOVE_UP            (0x53)		//�ǁi������ֈړ��s�j
#define MATTR_NMOVE_DOWN          (0x54)		//�ǁi�������ֈړ��s�j
#define MATTR_NMOVE_RU            (0x55)		//�ǁi�E�E������ֈړ��s�j
#define MATTR_NMOVE_LU            (0x56)		//�ǁi���E������ֈړ��s�j
#define MATTR_NMOVE_RD            (0x57)		//�ǁi�E�E�������ֈړ��s�j
#define MATTR_NMOVE_LD            (0x58)		//�ǁi���E�������ֈړ��s�j
#define MATTR_JUMP_RIGHT          (0x72)		//�E�����փW�����v�ړ�
#define MATTR_JUMP_LEFT           (0x73)		//�������փW�����v�ړ�
#define MATTR_JUMP_UP             (0x74)		//������փW�����v�ړ�
#define MATTR_JUMP_DOWN           (0x75)		//�������փW�����v�ړ�
#define MATTR_MOVEF_RIGHT         (0x76)		//�E�����֋����ړ�
#define MATTR_MOVEF_LEFT          (0x77)		//�������֋����ړ�
#define MATTR_MOVEF_UP            (0x78)		//������֋����ړ�
#define MATTR_MOVEF_DOWN          (0x79)		//�������֋����ړ�
#define MATTR_DRIFT_SAND_01       (0x7c)		//����
#define MATTR_CURRENT_RIGHT       (0x94)		//�E�����E�C
#define MATTR_CURRENT_LEFT        (0x95)		//�������E�C
#define MATTR_CURRENT_UP          (0x96)		//�㐅���E�C
#define MATTR_CURRENT_DOWN        (0x97)		//�������E�C
#define MATTR_CURRENTR_LEFT       (0x98)		//�����E��E��
#define MATTR_CURRENTR_RIGHT      (0x99)		//�����E��E�E
#define MATTR_CURRENTR_UP         (0x9a)		//�����E��E��
#define MATTR_CURRENTR_DOWN       (0x9b)		//�����E��E��
#define MATTR_SLIP_01             (0xa0)		//����~��
#define MATTR_SLIP_GRASS_LOW      (0xa1)		//����~��̑��ނ�
#define MATTR_SLIP_GRASS_HIGH     (0xa2)		//����~��̑��ނ�(��)
#define MATTR_HYBRID_CHANGE       (0xa3)		//�n�C�u���b�h��芷���p
#define MATTR_SLIP_RIGHT          (0xa4)		//�E��芊��
#define MATTR_SLIP_LEFT           (0xa5)		//����芊��
#define MATTR_SLIP_JUMP_RIGHT     (0xa6)		//�X��W�����v���i�E�����j
#define MATTR_SLIP_JUMP_LEFT      (0xa7)		//�X��W�����v���i�������j
#define MATTR_TURN_RIGHT          (0xa8)		//�E������
#define MATTR_OZE_01              (0xbe)		//����
#define MATTR_OZE_STAIRS          (0xbf)		//�����K�i
#define MATTR_COUNTER_01          (0xd4)		//�J�E���^�[
#define MATTR_MAT_01              (0xd5)		//�}�b�g
#define MATTR_PC_01               (0xd6)		//�p�\�R��
#define MATTR_WORLDMAP_01         (0xd7)		//�}�b�v
#define MATTR_TV_01               (0xd8)		//�e���r
#define MATTR_BOOKSHELF_01        (0xd9)		//�{�I�P
#define MATTR_BOOKSHELF_02        (0xda)		//�{�I�Q
#define MATTR_BOOKSHELF_03        (0xdb)		//�{�I�R
#define MATTR_BOOKSHELF_04        (0xdc)		//�{�I�S
#define MATTR_VASE_01             (0xdd)		//��
#define MATTR_DUST_BOX            (0xde)		//�S�~��
#define MATTR_SHOPSHELF_01        (0xdf)		//�f�p�[�g���V���b�v�̒I�P
#define MATTR_SHOPSHELF_02        (0xe0)		//�f�p�[�g���V���b�v�̒I�Q
#define MATTR_SHOPSHELF_03        (0xe1)		//�f�p�[�g���V���b�v�̒I�R
#define MATTR_NO_ATTR             (0xff)		//�A�g���r���[�g�Ȃ�

