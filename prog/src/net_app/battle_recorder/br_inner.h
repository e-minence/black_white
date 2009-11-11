//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_inner.h
 *	@brief	�o�g�����R�[�_�[	�v���C�x�[�g�w�b�_
 *	@author	Toru=Nagihashi
 *	@date		2009.11.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	BG�ʐݒ�
//=====================================
enum 
{
	BG_FRAME_M_BACK	= 0,
	BG_FRAME_S_BACK	= 0,
};

//-------------------------------------
///	PLT�ݒ�
//=====================================
enum 
{
	PLT_BG_M_BACK	= 0,
	PLT_BG_S_BACK	= 0,
};

//-------------------------------------
///	�v���Z�XID
//=====================================
typedef enum 
{
	BR_PROCID_START,		//�N�����
	BR_PROCID_MENU,			//���j���[���
	BR_PROCID_RECORD,		//�^��L�^���
	BR_PROCID_BTLSUBWAY,//�o�g���T�u�E�F�C���щ��
	BR_PROCID_RNDMATCH,	//�����_���}�b�`���щ��
	BR_PROCID_BV_RANK,	//�o�g���r�f�I30����ʁi�ŐV�A�ʐM�ΐ�A�T�u�E�F�C�j
	BR_PROCID_BV_SEARCH,//�ڂ����T�����
	BR_PROCID_BV_CODEIN,//�o�g���r�f�I�i���o�[���͉��
	BR_PROCID_BV_SEND,	//�o�g���r�f�I���M���
	BR_PROCID_MUSICAL_LOOK,	//�~���[�W�J���V���b�g	�ʐ^��������
	BR_PROCID_MUSICAL_SEND,	//�~���[�W�J���V���b�g	�ʐ^�𑗂���

} BR_PROCID;


//@todo���Ƃ�MENU�v���b�N�Ɉړ�
//-------------------------------------
///	���j���[�C���f�b�N�X
//		�����ł̃��j���[�Ƃ͏��ʂ��؂�ւ炸�A
//		���j���[�������Ă������ƂŐi��ł����ӏ��ł��B
//		�i�����͓���܂��j
//=====================================
typedef enum
{
	BR_BROWSE_MENUID_TOP,					//�u���E�U���[�h�@�������j���[

	BR_BROWSE_MENUID_BTLVIDEO,		//�u���E�U���[�h�@�o�g���r�f�I���j���[,

	BR_BROWSE_MENUID_OTHER_RECORD,//�u���E�U���[�h�@���ꂩ�̋L�^���j���[
	BR_BROWSE_MENUID_DELETE_RECORD,//�u���E�U���[�h�@�L�^���������j���[
	BR_BROWSE_MENUID_DELETE_OTHER,//�u���E�U���[�h�@�N���̋L�^���������j���[

	//BR_BROWSE_MENUID_BTLSUBWAY,		//�u���E�U���[�h�@�o�g���T�u�E�F�C���j���[	
	//BR_BROWSE_MENUID_RNDMATCH,		//�u���E�U���[�h�@�����_���}�b�`���j���[
	
	BR_BTLVIDEO_MENUID_TOP,					//�O���[�o���o�g���r�f�I���[�h�@�������j���[

	BR_BTLVIDEO_MENUID_LOOK,				//�O���[�o���o�g���r�f�I���[�h�@���郁�j���[
	BR_BTLVIDEO_MENUID_RANK,				//�O���[�o���o�g���r�f�I���[�h�@�����L���O�ŒT�����j���[

	BR_MUSICAL_MENUID_TOP,					//�O���[�o���~���[�W�J���V���b�g���[�h�@�������j���[

	BR_MENUID_YESNO,								//�ėp	�͂��A������
	BR_MENUID_MAX
} BR_MENUID;
