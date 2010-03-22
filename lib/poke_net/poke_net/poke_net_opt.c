//============================================================================
//                         Pokemon Network Library
//============================================================================
#include "poke_net_opt.h"
#include "poke_net_common.h"

unsigned char POKE_NET_CIPHER_Table[256] = {
	0x93 ,0x18 ,0x33 ,0x28 ,0x0B ,0x07 ,0x03 ,0x82 ,0x02 ,0x43 ,0x8A ,0x86 ,0xDB ,0x38 ,0x34 ,0x19 ,
	0xD6 ,0xF9 ,0x59 ,0xB2 ,0xAD ,0x6A ,0x7D ,0xBC ,0xEE ,0xE0 ,0x3A ,0x3F ,0xCA ,0x4C ,0x25 ,0x68 ,
	0xF4 ,0xA9 ,0x5B ,0xF7 ,0x22 ,0x60 ,0x5A ,0x6F ,0xFA ,0x1B ,0x79 ,0xE9 ,0x17 ,0xB1 ,0x00 ,0x9C ,
	0xAA ,0x5E ,0x9D ,0xFF ,0xEA ,0xA0 ,0x0D ,0x4B ,0x75 ,0xF6 ,0x61 ,0x85 ,0x5D ,0xBB ,0xDC ,0xFB ,
	0x64 ,0x2E ,0x7A ,0xAB ,0xF1 ,0xE8 ,0x44 ,0x0C ,0xB8 ,0x8F ,0xA8 ,0x0A ,0x8E ,0xBD ,0xE1 ,0x3B ,
	0xFC ,0x3C ,0x9F ,0x1A ,0x56 ,0xC5 ,0xE2 ,0xF5 ,0x47 ,0xD9 ,0xD7 ,0x8C ,0xCD ,0x97 ,0xF0 ,0x7B ,
	0x8B ,0xC3 ,0x4F ,0x45 ,0x04 ,0x90 ,0x81 ,0x1E ,0x6B ,0xC9 ,0xD3 ,0x73 ,0xC6 ,0xE7 ,0x24 ,0xBA ,
	0x32 ,0xF3 ,0xC0 ,0xEC ,0x57 ,0xCC ,0xC4 ,0xB6 ,0xC1 ,0xAE ,0xAF ,0x88 ,0xF2 ,0x84 ,0xCE ,0x4A ,
	0x0F ,0x94 ,0x41 ,0xB4 ,0x74 ,0x2A ,0xD1 ,0x70 ,0x1C ,0xD4 ,0xB0 ,0xC2 ,0x09 ,0x08 ,0x16 ,0x9B ,
	0xB5 ,0x8D ,0x2B ,0xD2 ,0x89 ,0xB7 ,0x99 ,0xA1 ,0x30 ,0x65 ,0x54 ,0x40 ,0x96 ,0x71 ,0xFE ,0xBF ,
	0x31 ,0x06 ,0xE5 ,0x14 ,0xE6 ,0xDA ,0x48 ,0x26 ,0xAC ,0x87 ,0x9A ,0xD8 ,0xA6 ,0xEB ,0x92 ,0xCF ,
	0xFD ,0x77 ,0x1D ,0x21 ,0x9E ,0x36 ,0x35 ,0x53 ,0x3E ,0xD0 ,0xD5 ,0x62 ,0x58 ,0x5F ,0x63 ,0x7C ,
	0x7E ,0x52 ,0x29 ,0x12 ,0x2C ,0x78 ,0x05 ,0x91 ,0x55 ,0xE3 ,0xA2 ,0xB9 ,0xF8 ,0x50 ,0x95 ,0x13 ,
	0x80 ,0x7F ,0x11 ,0x27 ,0xCB ,0x37 ,0x4E ,0x51 ,0x15 ,0xEF ,0xA7 ,0x72 ,0x4D ,0x83 ,0x49 ,0xA4 ,
	0x69 ,0xDE ,0x20 ,0xA3 ,0x67 ,0xDF ,0x10 ,0x42 ,0x39 ,0x6C ,0x2D ,0xC7 ,0x23 ,0xE4 ,0xDD ,0xED ,
	0xBE ,0x66 ,0xB3 ,0x2F ,0x01 ,0x6E ,0x6D ,0xC8 ,0x1F ,0x98 ,0xA5 ,0x46 ,0x76 ,0x5C ,0x3D ,0x0E ,
};


//====================================
//            �G���R�[�h
// _In   : �Í�����
// _Size : �Í������T�C�Y
// _Out  : �Í�����o�b�t�@(_Size*4/3�͕K�v)
//====================================
void POKE_NET_OptEncode(const unsigned char *_In, long _Size, unsigned char *_Out)
{
	long i;
	unsigned char crcode;

	for(i=0;i<_Size;i++){
		if( i )crcode = POKE_NET_CIPHER_Table[(i+_Size+_In[0]) & 0xFF];
		else crcode = 0;

		*_Out++ = (unsigned char)(_In[i] ^ crcode);
	}
}


//====================================
//              �f�R�[�h
// _In   : ��������
// _Size : ���������T�C�Y
// _Out  : ��������o�b�t�@(_Size*4/3�͕K�v)
// return: ��������T�C�Y(-1:�G���[)
//====================================
void POKE_NET_OptDecode(const unsigned char *_In, long _Size, unsigned char *_Out)
{
	long i;
	unsigned char crcode;

	// �����ŕ�����
	for(i=0;i<_Size;i++){
		if( i )crcode = POKE_NET_CIPHER_Table[(i+_Size+_In[0]) & 0xFF];
		else crcode = 0;
		_Out[i] ^= crcode;
	}
}


//====================================
//    ���N�G�X�g�ő�T�C�Y�擾
//====================================
long POKE_NET_GetRequestSize(long _reqno)
{
	long size;

	switch(_reqno)
	{
		case POKE_NET_GDS_REQCODE_DEBUG_MESSAGE:					// �f�o�b�O�p���N�G�X�g
			size = sizeof(POKE_NET_GDS_REQUEST_DEBUG_MESSAGE);
			break;
		case POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST:				// �~���[�W�J���V���b�g�o�^
			size = sizeof(POKE_NET_GDS_REQUEST_MUSICALSHOT_REGIST);
			break;
		case POKE_NET_GDS_REQCODE_MUSICALSHOT_GET:					// �~���[�W�J���V���b�g�擾
			size = sizeof(POKE_NET_GDS_REQUEST_MUSICALSHOT_GET);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:				// �o�g���f�[�^�o�^
			size = sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_REGIST);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:				// �o�g���f�[�^����
			size = sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:					//  �o�g���f�[�^�擾
			size = sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_GET);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:				// �o�g���f�[�^���C�ɓ���o�^
			size = sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_FAVORITE);
			break;

#ifndef ___POKE_NET_BUILD_DS___
		case POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS:					// �T�[�o�[�X�e�[�^�X�擾
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND:				// �T�[�o�[�ւ̖���
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS:			// �~���[�W�J���V���b�g��Ԏ擾
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE:			// �~���[�W�J���V���b�g�폜
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_MUSICALSHOT_DELETE);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS:			// �o�g���f�[�^��Ԏ擾
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE:			// �o�g���f�[�^�폜
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM:		// �p�����[�^�擾
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM:		// �p�����[�^�ݒ�
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_SETPARAM);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY:		// �o�g���f�[�^�����L���O�����擾
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG:		// �o�g���f�[�^�C�x���g�o�^�v��
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EVENTFLAG);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD:	// �o�g���f�[�^�C�x���g
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EVENTFREEWORD);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION:	// �m�菈�����s(����)
			size = 0;
			break;

		// ===========================================
		// �s���`�F�b�N�֘A
		// ===========================================

		// �s���`�F�b�N:�������R�[�h�擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�������R�[�h�Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ROMCODELIST_SET);
			break;

		// �s���`�F�b�N:�m�f���[�h�̐ݒ�擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�m�f���[�h�̐ݒ�Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_NGWORD_SET);
			break;

		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET);
			break;

		// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET);
			break;

		// �s���`�F�b�N:�e��ő�l�p�����[�^�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�e��ő�l�p�����[�^�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MAXLIST_SET);
			break;

		// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET);
			break;

		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET);
			break;


		// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET:
			size = 0;
			break;
		// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̃Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET:	
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET);
			break;
		//// �s���`�F�b�N:�ǂ����ݒ�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_GET:
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�ǂ����ݒ�̃Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_SET:
		//	size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ITEM_SET);
		//	break;
		//// �s���`�F�b�N:�Z�\�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_GET:
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�Z�\�̐ݒ�
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_SET:	
		//	size = 1024*1024;
		//	break;
		//// �s���`�F�b�N:�o������ꏊ�ݒ�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_GET:
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�o������ꏊ�ݒ�̃Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_SET:
		//	size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MEETPLACE_SET);
		//	break;
		//// �s���`�F�b�N:�|�P�������̐ݒ�擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET:	
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�|�P�������̐ݒ�Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET:	
		//	size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET);
		//	break;

		// �s���`�F�b�N���O:�擾
		case POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET:
			size = sizeof(POKE_NET_GDS_REQUEST_ADMIN_DHOCHECKLOG_GET);
			break;

#endif
		default:
			return(-1);
	}

	size += sizeof(long) + sizeof(POKE_NET_REQUEST);

	return(size);
}


//====================================
//    ���X�|���X�ő�T�C�Y�擾
//====================================
long POKE_NET_GetResponseMaxSize(long _resno)
{
	long size;
	long sizetmp;
	long i;
	long list[] = {
		POKE_NET_GDS_REQCODE_DEBUG_MESSAGE,			// �f�o�b�O�p���N�G�X�g

		POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST,	// �~���[�W�J���V���b�g�o�^
		POKE_NET_GDS_REQCODE_MUSICALSHOT_GET,		// �~���[�W�J���V���b�g�擾

		POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST,		// �o�g���f�[�^�o�^
		POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH,		// �o�g���f�[�^����
		POKE_NET_GDS_REQCODE_BATTLEDATA_GET,		// �o�g���f�[�^�擾
		POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE,	// �o�g���f�[�^���C�ɓ���o�^
		-1
	};

#ifndef ___POKE_NET_BUILD_DS___
	long list1[] = {
		POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS ,					// �T�[�o�[�X�e�[�^�X�擾
		POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND ,				// �T�[�o�[�ւ̖���

		POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS ,			// �~���[�W�J���V���b�g��Ԏ擾
		POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE ,			// �~���[�W�J���V���b�g�폜

		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS ,			// �o�g���f�[�^��Ԏ擾
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE ,			// �o�g���f�[�^�폜
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY ,		// �o�g���f�[�^�����L���O�����擾
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG ,		// �o�g���f�[�^�C�x���g�o�^�v��
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD ,	// �o�g���f�[�^�C�x���g
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM ,		// �p�����[�^�擾
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM ,		// �p�����[�^�ݒ�
		POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION ,	// �m�菈�����s(����)

		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_GET,			// �s���`�F�b�N:�������R�[�h�擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_SET,			// �s���`�F�b�N:�������R�[�h�Z�b�g
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_GET,					// �s���`�F�b�N:�m�f���[�h�擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_SET,					// �s���`�F�b�N:�m�f���[�h�Z�b�g
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET,		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̎擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET,		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̃Z�b�g
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET,	// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̎擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET,	// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̃Z�b�g
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_GET,				// �s���`�F�b�N:�e��ő�l�p�����[�^�̎擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_SET,				// �s���`�F�b�N:�e��ő�l�p�����[�^�̃Z�b�g
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET,	// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̎擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET,	// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̃Z�b�g
	
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET,		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̎擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET,		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̃Z�b�g

		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET,			// �s���`�F�b�N:�f�t�H���g�g���C�i�[���擾
		POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET,			// �s���`�F�b�N:�f�t�H���g�g���C�i�[���ݒ�

		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_GET,					// �s���`�F�b�N:�ǂ����擾
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_SET,					// �s���`�F�b�N:�ǂ����Z�b�g
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_GET,				// �s���`�F�b�N:�Z�e�[�u��
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_SET,				// �s���`�F�b�N:�Z�e�[�u��
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_GET,				// �s���`�F�b�N:�o������ꏊ�擾
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_SET,				// �s���`�F�b�N:�o������ꏊ�Z�b�g
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET,			// �s���`�F�b�N:�|�P�������̐ݒ�擾
		//POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET,			// �s���`�F�b�N:�|�P�������̐ݒ�Z�b�g

		POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET ,				// �s���`�F�b�N���O�F�擾
		-1
	};
#endif

	switch(_resno)
	{
		case -1:
			size = -1;
			for(i=0;;i++){
				if( list[i] == -1 )break;
				sizetmp = POKE_NET_GetResponseMaxSize(list[i]);
				if( sizetmp > size )size = sizetmp;
			}
#ifndef ___POKE_NET_BUILD_DS___
			for(i=0;;i++){
				if( list1[i] == -1 )break;
				sizetmp = POKE_NET_GetResponseMaxSize(list1[i]);
				if( sizetmp > size )size = sizetmp;
			}
#endif
			if( size == -1 )return(-1);
			break;
		case POKE_NET_GDS_REQCODE_DEBUG_MESSAGE:					// �f�o�b�O�p���N�G�X�g
			size = sizeof(POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE);
			break;
		case POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST:				// �~���[�W�J���V���b�g�o�^
			size = sizeof(POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST);
			break;
		case POKE_NET_GDS_REQCODE_MUSICALSHOT_GET:					// �~���[�W�J���V���b�g�擾
			size = sizeof(POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET)
				+ sizeof(POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA) * POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET_MAX;
			break;

		case POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST:				// �o�g���f�[�^�o�^
			size = sizeof(POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH:				// �o�g���f�[�^����
			size = sizeof(POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH)
				+ sizeof(POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA) * POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_MAXNUM;
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_GET:					//  �o�g���f�[�^�擾
			size = sizeof(POKE_NET_GDS_RESPONSE_BATTLEDATA_GET);
			break;
		case POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE:				// �o�g���f�[�^���C�ɓ���o�^
			size = 0;
			break;

#ifndef ___POKE_NET_BUILD_DS___
		case POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS:					// �T�[�o�[�X�e�[�^�X�擾
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS)
				+ sizeof(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA) * POKE_NET_GDS_RESPONSE_ADMIN_SVR_MAXNUM;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND:				// �T�[�o�[�ւ̖���
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS:			// �~���[�W�J���V���b�g��Ԏ擾
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_MUSICALSHOT_STATUS);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE:			// �~���[�W�J���V���b�g�폜
			size = 0;
			break;

		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS:					// �o�g���f�[�^��Ԏ擾
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_STATUS);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE:					// �o�g���f�[�^�폜
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY:				// �o�g���f�[�^�����L���O�����擾
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY)
				+ (sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING) + sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM) * 100)
				* 2;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG:				// �o�g���f�[�^�C�x���g�o�^�v��
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD:			// �o�g���f�[�^�C�x���g
			size = 0;
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM:				// �p�����[�^�擾
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_GETPARAM);
			break;
		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM:				// �p�����[�^�ݒ�
			size = 0;
			break;

		case POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION:			// �m�菈�����s(����)
			size = 0;
			break;

		// ===========================================
		// �s���`�F�b�N�֘A
		// ===========================================

		// �s���`�F�b�N:�������R�[�h�擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_GET:
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ROMCODELIST_GET)
				+ sizeof(POKE_NET_GDS_ADMIN_ILLEGAL_ROMCODE_LIST_ITEM) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_ROMCODELIST_MAXNUM;
			break;
		// �s���`�F�b�N:�������R�[�h�Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ROMCODELIST_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�m�f���[�h�̐ݒ�擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_GET:
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_NGWORD_GET)
				+ sizeof(long) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_NGWORD_MAXNUM;
			break;
		// �s���`�F�b�N:�m�f���[�h�̐ݒ�Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_NGWORD_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET:	
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET)
				+ sizeof(STRCODE)
				* MUSICAL_PROGRAM_NAME_MAX
				* POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_MAXNUM;
			break;
		// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET:	
			size = 0;
			break;

		// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET:	
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET)
				+ sizeof(u16) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_MAXNUM;
			break;
		// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�e��ő�l�p�����[�^�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_GET:
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MAXLIST_GET);
			break;
		// �s���`�F�b�N:�e��ő�l�p�����[�^�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MAXLIST_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET:	
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET)
				+ sizeof(u16) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_MAXNUM;
			break;
		// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET:	
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET)
				+ sizeof(u16) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_MAXNUM;
			break;
		// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̐ݒ�
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET:
			size = 0;
			break;

		// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̎擾
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET:
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET);
			break;
		// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̃Z�b�g
		case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET:
			size = 0;
			break;

		//// �s���`�F�b�N:�ǂ����ݒ�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_GET:	
		//	size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ITEM_GET)
		//		+ sizeof(long) * POKE_NET_GDS_ADMIN_ILLEGALCHECK_ITEM_MAXNUM;
		//	break;
		//// �s���`�F�b�N:�ǂ����ݒ�̃Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_ITEM_SET:	
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�Z�\�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_GET:
		//	size = 1024 * 1024;
		//	break;
		//// �s���`�F�b�N:�Z�\�̐ݒ�
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_WAZATABLE_SET:
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�o������ꏊ�ݒ�̎擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_GET:
		//	size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MEETPLACE_GET) + 
		//		sizeof(s16)*POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPOKEMON*POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEETPLACE_MAXPLACE;
		//	break;
		//// �s���`�F�b�N:�o������ꏊ�ݒ�̃Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_MEETPLACE_SET:	
		//	size = 0;
		//	break;
		//// �s���`�F�b�N:�|�P�������̐ݒ�擾
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET:
		//	size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_POKEMONTABLE_GET);
		//	break;
		//// �s���`�F�b�N:�|�P�������̐ݒ�Z�b�g
		//case POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_POKEMONTABLE_SET:
		//	size = 0;
		//	break;

		// �s���`�F�b�N���O:�擾
		case POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET:
			size = sizeof(POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET)
				+ sizeof(POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM)*POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_MAXRESPONSE;
			break;
#endif
		default:
			return(-1);
	}

	// �T�C�Y��M���ƃ��N�G�X�g�\���̕������Z
	size += sizeof(long) + sizeof(POKE_NET_RESPONSE);

	return(size);
}


//====================================
//   �v���C�f�[�^�h�c�̃G���R�[�h
// �T�[�o�[DB��ID���v���C���[�ɓ`����ID
//====================================
u64 POKE_NET_EncodePlayDataID(u64 _id)
{
	long i;//,j;
	long tmp[12];
	long shuffleparam[12] = {5,3,2,4,1,6,9,2,7,8,6,3};
	u64 idtmp;
	int evflag = 0;

	if( _id >= (((u64)900000) * ((u64)1000000)) ){
		// �C�x���g�f�[�^�F���炩����900000000000�������Ă���
		evflag = 1;
		_id -= (((u64)900000) * ((u64)1000000));
	}

	// �e�������o��
	idtmp = _id;
	for(i=0;i<12;i++){
		tmp[i] = (long)(idtmp % 10);
		idtmp /= 10;
	}

	// tmp[0]�͕��ՂƂ���
	for(i=1;i<12;i++){
		if( i == 11 ){
			// 12����
			if( evflag == 1 ){
				// �C�x���g�f�[�^�͐G��Ȃ�
			}else{
				// ��C�x���g�f�[�^��1�`9�ɂȂ�悤�ۂ߂�(0���̒l��0�`8�Ȃ̂�9�Ŋۂ߂�)
				tmp[i] = (tmp[i] + shuffleparam[(tmp[0]+i) % 12]+i) % 9;
			}
		}else{
			tmp[i] = (tmp[i] + shuffleparam[(tmp[0]+i) % 12]+i) % 10;
		}
	}

	// �O�̐��l�𑫂�����
	for(i=1;i<12;i++){
		if( i == 11 ){
			if( evflag == 1 ){
				// �C�x���g�f�[�^�͐�΂�12���ڂ�0�ɂȂ�
			}else{
				// ��C�x���g�f�[�^��1�`9�ɂȂ�悤�ۂ߂�(0���̒l��0�`8�Ȃ̂�9�Ŋۂ߂ĂP���Z)
				tmp[i] = 1 + ((tmp[i] + tmp[i-1]) % 9);
			}
		}else{
			tmp[i] = (tmp[i] + tmp[i-1]) % 10;
		}
	}
	
	// �߂�
	idtmp = 0;
	for(i=0;i<12;i++){
		idtmp = idtmp * (u64)10;
		idtmp = idtmp + (u64)tmp[11-i];
	}

	return(idtmp);
}


//====================================
// �v���C�f�[�^�h�c�̃f�R�[�h
// �v���C���[�ɓ`����ID���T�[�o�[DB��ID
//====================================
u64 POKE_NET_DecodePlayDataID(u64 _id)
{
	long i;//,j;
	long tmp[12];
	long shuffleparam[12] = {5,3,2,4,1,6,9,2,7,8,6,3};
	u64 idtmp;
	long prev;
	long back;
	int evflag = 0;

	if( _id < (((u64)100000) * ((u64)1000000)) ){
		// �C�x���g�f�[�^
		evflag = 1;
	}

	idtmp = _id;
	for(i=0;i<12;i++){
		tmp[i] = (long)(idtmp % 10);
		idtmp /= 10;
	}

	// �O�̐��l����������
	prev = tmp[0];
	for(i=1;i<12;i++){
		back = tmp[i];
		if( i == 11 ){
			if( evflag == 1 ){
				// �C�x���g�f�[�^�F���ɐG��Ȃ�
			}else{
				// ��C�x���g�f�[�^
				tmp[i] = ((tmp[i] - prev + 9) - 1) % 9;
			}
		}else{
			tmp[i] = (tmp[i] - prev + 10) % 10;
		}
		
		prev = back;
	}

	// tmp[0]�͕��ՂƂ���
	for(i=1;i<12;i++){
		if( i == 11 ){
			// 12����
			if( evflag == 1 ){
				// �C�x���g�f�[�^�͐G��Ȃ�
			}else{
				// ��C�x���g�f�[�^��9�Ŋۂ߂��Ă���
				tmp[i] = (tmp[i] - shuffleparam[(tmp[0]+i) % 12] - i + 27) % 9;
			}
		}else{
			tmp[i] = (tmp[i] - shuffleparam[(tmp[0]+i) % 12] - i + 30) % 10;
		}
	}
	
	// �߂�
	idtmp = 0;
	for(i=0;i<12;i++){
		idtmp = idtmp * (u64)10;
		idtmp = idtmp + (u64)tmp[11-i];
	}

	if( evflag ){
		idtmp += (((u64)900000) * ((u64)1000000));
	}
	return(idtmp);
}
