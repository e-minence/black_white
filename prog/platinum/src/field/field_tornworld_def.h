//******************************************************************************
/**
 * 
 * @file	field_tornworld_def.h
 * @brief	�j�ꂽ���E�@define �w�b�_�[ �X�N���v�g�Q�ƗL��
 * @author	kagaya
 * @data	05.07.20
 */
//******************************************************************************
#ifndef FLD_TORNWORLD_DEF_H
#define FLD_TORNWORLD_DEF_H

//--------------------------------------------------------------
///	SYS_WORK_TW_EVSEQ�B
///	TWEVSEQ		TWEVSEQ_�K�w_�C�x���g�ԍ�
//--------------------------------------------------------------
///�V���i�j�ꂽ���E����J�n�B����I����A����
#define TWEVSEQ_01_00 (0)
///�V���i�ړ�������B����� ����
#define TWEVSEQ_01_01	(1)
///�V���i���ړ������B��w���t�g�ړ��C�x���g�I���Ŏ���
#define TWEVSEQ_01_02	(2)
///�G�����b�g�U���C�x���g�B�@�U���C�x���g�I���Ŏ���
#define TWEVSEQ_02_00	(3)
///�O�w�V���i�ǖʉ�b�B�@��b�I����Ŏ���
#define TWEVSEQ_03_00	(4)
///�O�w�V���i�ǖʉ�b�I����B�l�w�A�J�M�o��C�x���g�����Ŏ��ցB
#define TWEVSEQ_03_01	(5)
///�l�w�A�J�M�o��C�x���g�I���B�V���i�X�g�b�p�[��ԁB�◎�Ƃ������㎟��
#define TWEVSEQ_04_00	(6)
///���w�◎�Ƃ�������̃V���i�ړ���B���w�Ɉړ�����Ǝ���
#define TWEVSEQ_07_00	(7)
///�V���i�Ƌ��ɔ��w���B�B�V���i�A�A�J�M��b�㎟��
#define TWEVSEQ_08_00	(8)
///���w�A�V���i�A�A�J�M��b��B�A�J�M�퓬��Ɏ���
#define TWEVSEQ_08_01	(9)
///���w�@�A�J�M�퓬��B�M���e�B�i�퓬�I����Ɏ���
#define TWEVSEQ_08_02	(10)
///���w�@�M���e�B�i�퓬�I����B
#define TWEVSEQ_08_03	(11)

///�ő�,������
#define TWEVSEQ_ANY		(12)

///�j�ꂽ���E���N���A����
#define TWEVSEQ_CLEAR	(TWEVSEQ_08_03)

//--------------------------------------------------------------
///	�j�ꂽ���E OBJID�
//--------------------------------------------------------------
#define TWOBJID					(0x80)

//--------------------------------------------------------------
///	���wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0101_SIRONA_00	(TWOBJID+0)	//�V���i�C�x���g�p
#define TW_OBJID_0101_SIRONA_01	(TWOBJID+1)	//�V���i���t�g�z�u

//--------------------------------------------------------------
///	��Q�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0102_SIRONA	(TWOBJID+0)	//�V���i���t�g�C�x���g�p
#define TW_OBJID_0102_SPPOKE2	(TWOBJID+1)	//�G�����b�g�C�x���g

//--------------------------------------------------------------
///	��O�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0103_SIRONA	(TWOBJID+0)	//�V���i�ǈē�

//--------------------------------------------------------------
///	��l�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0104_AKAGI		(TWOBJID+0)	//�A�J�M

//--------------------------------------------------------------
///	��Z�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0107_ROCK_0	(TWOBJID+0)	//������0
#define TW_OBJID_0107_ROCK_1	(TWOBJID+1)	//������1
#define TW_OBJID_0107_ROCK_2	(TWOBJID+2)	//������2

//--------------------------------------------------------------
///	�掵�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0108_ROCK_0	(TWOBJID+0)	//������0
#define TW_OBJID_0108_ROCK_1	(TWOBJID+1)	//������1
#define TW_OBJID_0108_ROCK_2	(TWOBJID+2)	//������2
#define TW_OBJID_0108_EM		(TWOBJID+3)	//�G�����b�g
#define TW_OBJID_0108_YUKU		(TWOBJID+4)	//���N�V�[
#define TW_OBJID_0108_AGU		(TWOBJID+5)	//�A�O�m��
#define TW_OBJID_0108_SIRONA	(TWOBJID+6)	//�V���i

//--------------------------------------------------------------
///	�攪�K�wOBJID
//--------------------------------------------------------------
#define TW_OBJID_0109_SIRONA	(TWOBJID+0) //�V���i
#define TW_OBJID_0109_AKAGI		(TWOBJID+1)	//�A�J�M

//--------------------------------------------------------------
///	�攪�K�w�[��OBJID
//--------------------------------------------------------------
#define TW_OBJID_0110_GIRA		(TWOBJID+0)	//�M���e�B�i
#define TW_OBJID_0110_SIRONA	(TWOBJID+1)
#define TW_OBJID_0110_AKAGI		(TWOBJID+2)
#define TW_OBJID_0110_HOLE		(TWOBJID+3)
#define TW_OBJID_0110_NONSIRONA (TWOBJID+4)

//--------------------------------------------------------------
///	�攪�K�w���̂QOBJID
//--------------------------------------------------------------
#define TW_OBJID_0111_HOLE		(TWOBJID+0)
#define TW_OBJID_0111_ITEM		(TWOBJID+1)

#endif	//FLD_TORNWORLD_DEF_H
