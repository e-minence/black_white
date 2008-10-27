//======================================================================
/**
 *
 * @file	dlplay_child_common.h
 * @brief	�Z�[�u�f�[�^����݂̋��ʃt�@�C��
 *			�o�[�W�����ʃR�[�h�ƁAdlplay_data_main�ȊO�œǂނ�
 * @author	ariizumi
 * @data	
 */
//======================================================================

#ifndef DLPLAY_CHILD_COMMON_H_
#define DLPLAY_CHILD_COMMON_H_

enum DLPLAY_DATA_SAVEPOS
{
	DDS_FIRST,	//1�ԃf�[�^
	DDS_SECOND,	//2�ԃf�[�^
};

struct _DLPLAY_DATA_DATA
{
	int	heapID_;
	u8	mainSeq_;
	u8	subSeq_;

	s32	lockID_;	//�J�[�h���b�N�pID
	MATHCRC16Table	crcTable_;	//CRC�`�F�b�N�p�e�[�u��

	DLPLAY_CARD_TYPE	cardType_;	//�{�̂Ɏh�����Ă���J�[�h�̎��
	u8					savePos_;	//�P�ƂQ�̂ǂ�������ǂ񂾂��H

	DLPLAY_MSG_SYS	*msgSys_;	//�ォ��n���ċ��p
	u8	*pData_;	//�ǂݏo�����f�[�^
	u8	*pDataMirror_;	//�~���[�p�f�[�^
	u8	*pBoxData_;		//�ǂݍ��ݑΏۂ�BOX�f�[�^�A�h���X(�܂��|�P�����f�[�^����Ȃ��I
};	//DLPLAY_DATA��DATA�E�E�E��₱�����E�E�E


#endif //DLPLAY_CHILD_COMMON_H_

