#ifndef MPDSMODEL_H__
#define MPDSMODEL_H__

#define WM_SIZE_USER_GAMEINFO (112)
#define _BEACON_SIZE_FIX (12)
#define _BEACON_FIXHEAD_SIZE (6)
#define _BEACON_USER_SIZE_MAX (WM_SIZE_USER_GAMEINFO-_BEACON_SIZE_FIX)

//�A���C�����g��1�o�C�g�P�ʂɕύX
//���pop����̂�Y��Ȃ�����
#pragma pack(push,1)
typedef struct
{
	u8	  FixHead[_BEACON_FIXHEAD_SIZE];		 ///< �Œ�Ō��߂��U�o�C�g����
	//GameServiceID
	u8		serviceNo; ///< �ʐM�T�[�r�X�ԍ�
	u8	  debugAloneTest;	  ///< �f�o�b�O�p �����Q�[���ł��r�[�R�����E��Ȃ��悤��
	u8  	connectNum;		   ///< �Ȃ����Ă���䐔  --> �{�e���ǂ�������
	u8	  pause;			   ///< �ڑ����֎~���������Ɏg�p����
	u8		dummy1;
	u8		dummy2;
	u8	  aBeaconDataBuff[_BEACON_USER_SIZE_MAX];
	
}DS_COMM_USERDATAINFO;
#pragma pack(pop)

#endif	//MPDSMODEL_H__