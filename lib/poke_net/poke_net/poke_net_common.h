/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net_common.h

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Syachi Pokemon GDS WiFi ���C�u����

*/
//===========================================================================
#ifndef ___POKE_NET_GDS_COMMON___
#define ___POKE_NET_GDS_COMMON___


#include "poke_net_dscomp.h"				// LINUX/WINDOWS/DS��typedef�z��
#include "gds_header/gds_define.h"
#include "gds_header/gds_profile_local.h"	// �v���t�B�[���֌W
#include "gds_header/gds_battle_rec_sub.h"	// �o�g���r�f�I�f�[�^�֌W
#include "gds_header/gds_battle_rec.h"		// �o�g���r�f�I�f�[�^�֌W
#include "gds_header/gds_musical_sub.h"		// �~���[�W�J���V���b�g�֌W
#include "gds_header/gds_musical.h"			// �~���[�W�J���V���b�g�֌W
#include "gds_header/gds_min_max.h"


//===============================================
//                ���ԃp�b�N
//===============================================
#define POKE_NET_TIMETOU64(Y,M,D,h,m,s)	(u64)((((u64)Y) << 40) | (((u64)M) << 32) | (((u64)D) << 24) | (((u64)h) << 16) | (((u64)m) << 8) | (((u64)s) << 0))


//===============================================
//!          �f�c�r���N�G�X�g�R�[�h
//===============================================
enum POKE_NET_GDS_REQCODE
{
	// ------------------------- �ȉ��f�o�b�O���N�G�X�g ----------------------
	POKE_NET_GDS_REQCODE_DEBUG_START							= 0,	// ���ȉ��f�o�b�O�Ŏg�p���郊�N�G�X�g
	POKE_NET_GDS_REQCODE_DEBUG_MESSAGE							= 0,	//!< �f�o�b�O���b�Z�[�W

	POKE_NET_GDS_REQCODE_DEBUG_END								= 9999,	// ��9999�܂ł̓f�o�b�O�Ŏd�l���郊�N�G�X�g

#ifndef ___POKE_NET_BUILD_DS___
	// -------------------------- �ȉ��Ǘ��҃��N�G�X�g -----------------------
	POKE_NET_GDS_REQCODE_ADMIN_START							= 10000,// ��10000�ȍ~�͊Ǘ��҂��g�p�ł��郊�N�G�X�g
	POKE_NET_GDS_REQCODE_ADMIN_SVR_STATUS						= 10000,// �T�[�o�[�X�e�[�^�X�擾
	POKE_NET_GDS_REQCODE_ADMIN_SVR_COMMAND,								// �T�[�o�[�ւ̖���

	POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_STATUS				= 11000,// �~���[�W�J���V���b�g��Ԏ擾
	POKE_NET_GDS_REQCODE_ADMIN_MUSICALSHOT_DELETE,						// �~���[�W�J���V���b�g�폜

	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_STATUS				= 12000,// �o�g���f�[�^��Ԏ擾
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_DELETE,						// �o�g���f�[�^�폜
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_RANKHISTORY,					// �o�g���f�[�^�����L���O�����擾
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFLAG,					// �o�g���f�[�^�C�x���g�o�^�v��
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EVENTFREEWORD,				// �C�x���g�f�[�^�̉���
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_GETPARAM,						// �p�����[�^�擾
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_SETPARAM,						// �p�����[�^�ݒ�
	POKE_NET_GDS_REQCODE_ADMIN_BATTLEDATA_EXECDECISION,					// �m�菈�����s(����)

	POKE_NET_GDS_REQCODE_ADMIN_ILLEGALCHECK_START				= 13000,
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

	POKE_NET_GDS_REQCODE_ADMIN_DHOCHECKLOG_GET					= 14000,// �s���`�F�b�N���O:�擾

	POKE_NET_GDS_REQCODE_ADMIN_END								= 19999,// ��19999�܂ł͊Ǘ��҂��g�p�ł��郊�N�G�X�g
#endif

	// ------------------------- �ȉ����[�U�[���N�G�X�g ----------------------
	POKE_NET_GDS_REQCODE_USER_START								= 20000,// ��20000�ȍ~�͈�ʃ��[�U�[���g���郊�N�G�X�g
	POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST						= 21000,//!< �~���[�W�J���V���b�g�A�b�v���[�h
	POKE_NET_GDS_REQCODE_MUSICALSHOT_GET,								//!< �~���[�W�J���V���b�g�擾

	POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST						= 22000,//!< �o�g���f�[�^�A�b�v���[�h
	POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH,								//!< �o�g���f�[�^����
	POKE_NET_GDS_REQCODE_BATTLEDATA_GET,								//!< �o�g���f�[�^�擾
	POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE,							//!< �o�g���f�[�^���C�ɓ���o�^

	POKE_NET_GDS_REQCODE_LAST
};


// �F�؃g�[�N����
#define	POKE_NET_GDS_REQUESTCOMMON_AUTH_SVLTOKEN_LENGTH (304)

//===============================================
//! �|�P����WiFi���C�u���� ���N�G�X�g�F�؏��
//===============================================
typedef struct 
{
	s32		PID;								//!< �v���t�@�C���h�c
	u8		ROMCode;							//!< �o�[�W�����R�[�h
	u8		LangCode;							//!< ����R�[�h
	char	SvlToken[POKE_NET_GDS_REQUESTCOMMON_AUTH_SVLTOKEN_LENGTH];	//!< �T�[�r�X���P�[�^�g�[�N��
	u16		Dummy;
} 
POKE_NET_REQUESTCOMMON_AUTH;


//===============================================
//!  �|�P����WiFi���C�u���� ���N�G�X�g�w�b�_
//===============================================
typedef struct 
{
	unsigned short ReqCode;						//!< ���N�G�X�g�R�[�h(POKE_NET_GDS_REQUEST_REQCODE_xxxx)
	unsigned short Option;						//!< ���N�G�X�g�I�v�V����
	POKE_NET_REQUESTCOMMON_AUTH Auth;			//!< ���[�U�[�F�؏��
	unsigned char Param[0];						//!< �p�����[�^(�e���N�G�X�g�̍\����)
}
POKE_NET_REQUEST;


//===============================================
//!  �|�P����WiFi���C�u���� ���X�|���X�w�b�_
//===============================================
typedef struct
{
	unsigned short ReqCode;						//!< ���N�G�X�g�R�[�h(POKE_NET_GDS_REQUEST_REQCODE_xxxx)
	unsigned short Result;						//!< ���X�|���X����(POKE_NET_GDS_RESPONSE_RESULT_xxxx)
	unsigned char Param[0];						//!< �p�����[�^(�e���X�|���X�̍\����)
}
POKE_NET_RESPONSE;



//===============================================
//       ���N�G�X�g/���X�|���X�\����
//===============================================

//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//      �f�o�b�O���N�G�X�g/���X�|���X
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//------------------------------------
//         �f�o�b�O���b�Z�[�W
//------------------------------------
//! �f�c�r�f�o�b�O���b�Z�[�W����
enum POKE_NET_GDS_RESPONSE_RESULT_DEBUG_MESSAGE 
{
	POKE_NET_GDS_RESPONSE_RESULT_DEBUG_MESSAGE_SUCCESS ,				//!< �o�^����
	POKE_NET_GDS_RESPONSE_RESULT_DEBUG_MESSAGE_ERROR_UNKNOWN			//!< ���̑��G���[
};

//! �f�c�r�f�o�b�O���b�Z�[�W���N�G�X�g
typedef struct
{
	char Message[1024];						//!< �f�o�b�O���b�Z�[�W
}
POKE_NET_GDS_REQUEST_DEBUG_MESSAGE;

//! �f�c�r�f�o�b�O���b�Z�[�W���X�|���X
typedef struct
{
	char ResultMessage[1024];				//!< �f�o�b�O���b�Z�[�W
}
POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE;


//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//        ���[�U�[���N�G�X�g/���X�|���X
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//------------------------------------
//     �~���[�W�J���V���b�g�o�^
//------------------------------------

//! �f�c�r�~���[�W�J���V���b�g�o�^����
enum POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST 
{
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_SUCCESS,				//!< �o�^����
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_AUTH,				//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ALREADY,			//!< ���łɓo�^����Ă���
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGAL,			//!< �s���ȃf�[�^
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGALPOKEMON,	//!< �s���ȃ|�P�����ԍ�
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_ILLEGALPROFILE,	//!< �s���ȃ��[�U�[�v���t�B�[��
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_ERROR_UNKNOWN			//!< ���̑��G���[
};

//! �f�c�r�~���[�W�J���V���b�g�o�^���N�G�X�g
typedef struct 
{
	MUSICAL_SHOT_SEND Data;							//!< �~���[�W�J���V���b�g���M�f�[�^
}
POKE_NET_GDS_REQUEST_MUSICALSHOT_REGIST;

//! �f�c�r�~���[�W�J���V���b�g�o�^���X�|���X
typedef struct 
{
	u64 Code;										//!< �o�^���ꂽ�~���[�W�J���V���b�g�̃R�[�h
} 
POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST;


//------------------------------------
//     �~���[�W�J���V���b�g�擾
//------------------------------------

// �N���C�A���g���󂯎��~���[�W�J���V���b�g�ő吔
#define	POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET_MAX	(5)				

//! �f�c�r�~���[�W�J���V���b�g�擾����
enum POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_SUCCESS,				//!< �擾����
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_AUTH,			//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_ILLEGALPOKEMON,	//!< �|�P�����ԍ��G���[
	POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_ERROR_UNKNOWN			//!< ���̑��G���[
};

//! �f�c�r�~���[�W�J���V���b�g�擾�^�C�v
enum POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE 
{
	POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE_POKEMON,				//!< �|�P�����ԍ��ōŐV�擾(SearchParam�Ƀ|�P�����ԍ�)
	POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE_CODE,					//!< �R�[�h���ڎw��Ŏ擾
	POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE_PID					//!< PID�ɂ��擾(�Ǘ��p)
};

//! �f�c�r�~���[�W�J���V���b�g�擾���N�G�X�g
typedef struct
{
	unsigned short SearchType;										//!< �����^�C�v(POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE_xxxx)
	unsigned short SearchOpt;										//!< �����I�v�V����(���ݖ��g�p)
	u64 SearchParam;												//!< �����p�����[�^
} 
POKE_NET_GDS_REQUEST_MUSICALSHOT_GET;

//! �f�c�r�~���[�W�J���V���b�g�擾���X�|���X���e
typedef struct 
{
	long PID;														//!< �A�b�v���[�h�҂̃v���t�@�C���h�c
	u64 Code;														//!< �~���[�W�J���V���b�g�̊Ǘ��R�[�h
	MUSICAL_SHOT_RECV Data;											//!< �~���[�W�J���V���b�g��M�f�[�^
} 
POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA;

//! �f�c�r�~���[�W�J���V���b�g�擾���X�|���X
typedef struct 
{
	long HitCount;													//!< �q�b�g����
	POKE_NET_GDS_RESPONSE_MUSICALSHOT_RECVDATA Data[0];				//!< �~���[�W�J���V���b�g��M�f�[�^
} 
POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET;


//------------------------------------
//       �o�g���f�[�^�o�^
//------------------------------------

//! �f�c�r�o�g���f�[�^�o�^����
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST 
{
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS ,					//!< �o�^����
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_AUTH ,					//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ALREADY ,				//!< ���łɓo�^����Ă���
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL ,				//!< �s���ȃf�[�^
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGAL_RANKINGTYPE,	//!< �s���ȃ����L���O���
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPROFILE ,		//!< �s���ȃ��[�U�[�v���t�B�[��
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_UNKNOWN ,				//!< ���̑��G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_MINE ,	//!< �����̃|�P�����ŃG���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_ERROR_ILLEGALPOKEMON_OTHER 	//!< ����̃|�P�����ŃG��
};

//! �f�c�r�o�g���f�[�^�o�^���N�G�X�g
typedef struct
{
	BATTLE_REC_SEND Data;													//!< �o�g���f�[�^���M�f�[�^
} 
POKE_NET_GDS_REQUEST_BATTLEDATA_REGIST;

//! �f�c�r�o�g���f�[�^�o�^���X�|���X
typedef struct
{
	u64 Code;																//!< �o�^���ꂽ�o�g���f�[�^�̃R�[�h
} 
POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST;


//------------------------------------
//      �o�g���f�[�^���X�g�擾
//------------------------------------

//! �f�c�r�o�g���f�[�^���X�g�擾����
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH 
{
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS ,			//!< �擾����
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_AUTH ,			//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALPARAM ,	//!< �����p�����[�^�G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_ILLEGALRANKINGTYPE ,//!< �����p�����[�^�G���[(�����L���O��ʂ��s��)
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_ERROR_UNKNOWN		//!< ���̑��G���[
};

//! �f�c�r�o�g���f�[�^���X�g�擾�^�C�v
enum POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION,				//!< �����w��Ŏ擾
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_LATEST,			//!< �ŐV30�������L���O
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_SUBWAY,			//!< �o�g���T�u�E�F�C�����L���O
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_COMM,			//!< �ʐM�ΐ탉���L���O
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_PID,						// PID�ɂ��擾(�Ǘ��p)
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_EVENT					// �C�x���g�̈�
};

//! �f�c�r�o�g���f�[�^���X�g�擾���N�G�X�g
typedef struct
{
	unsigned short SearchType;											//!< �����^�C�v(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_xxxx)
	unsigned short SearchOpt;											//!< �����I�v�V����(���ݖ��g�p)
	BATTLE_REC_SEARCH_SEND ConditionParam;								//!< ���������p�����[�^
	u64 SearchParam;													//!< �����p�����[�^
}
POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH;

// �N���C�A���g���󂯎��o�g���f�[�^�w�b�_�ő吔
#define	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_MAXNUM	(30)


//! �f�c�r�o�g���f�[�^���X�g�擾���X�|���X���e
typedef struct 
{
	long PID;															//!< �A�b�v���[�h�҂̃v���t�@�C���h�c
	u64 Code;															//!< �o�g���f�[�^�R�[�h
	BATTLE_REC_OUTLINE_RECV Data;										//!< �o�g���f�[�^�w�b�_
}
POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA;

//! �f�c�r�o�g���f�[�^���X�g�擾���X�|���X
typedef struct 
{
	long HitCount;														//!< �q�b�g����
	POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH_RECVDATA Data[0];				//!< �o�g���f�[�^���X�g
}
POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH;


//------------------------------------
//      �o�g���f�[�^�擾
//------------------------------------

//! �f�c�r�o�g���f�[�^�擾����
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS,				//!< �擾����
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_AUTH,				//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALCODE,		//!< �R�[�h�G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_ILLEGALVERSION,	//!< �o�[�W�����G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_ERROR_UNKNOWN			//!< ���̑��G���[
};

//! �f�c�r�o�g���f�[�^�擾���N�G�X�g
typedef struct 
{
	u64 Code;															//!< �o�g���f�[�^�R�[�h
	u32 ServerVersion;													//!< �擾�Ώۃo�g���f�[�^�̃T�[�o�[�o�[�W����
}
POKE_NET_GDS_REQUEST_BATTLEDATA_GET;

//! �f�c�r�o�g���f�[�^�擾���X�|���X
typedef struct
{
	long PID;															//!< �A�b�v���[�h�҂̃v���t�@�C���h�c
	u64 Code;															//!< �o�g���f�[�^�R�[�h
	BATTLE_REC_RECV Data;												//!< �o�g���f�[�^
} 
POKE_NET_GDS_RESPONSE_BATTLEDATA_GET;


//------------------------------------
//      �o�g���f�[�^���C�ɓ���o�^
//------------------------------------

//! �f�c�r�o�g���f�[�^���C�ɓ���o�^����
enum POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE 
{
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS ,				//!< ���C�ɓ���o�^����
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_AUTH ,			//!< ���[�U�[�F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_ILLEGALCODE ,	//!< �R�[�h�G���[
	POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_ERROR_UNKNOWN			//!< ���̑��G���[
};

//! �f�c�r�o�g���f�[�^���C�ɓ���o�^���N�G�X�g
typedef struct 
{
	u64 Code;																//!< �o�g���f�[�^�R�[�h
}
POKE_NET_GDS_REQUEST_BATTLEDATA_FAVORITE;

// ���X�|���X�̓R�[�h�݂̂Ȃ̂ŁA�\���̂̒�`����


#ifndef ___POKE_NET_BUILD_DS___
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//        �Ǘ��҃��N�G�X�g/���X�|���X
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//------------------------------------
//        �T�[�o�[��Ԏ擾
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_SUCCESS,		// �擾����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_STATUS_ERROR_UNKNOWN	// ���̑��G���[
};

enum 
{
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_STOPPED,	// ��~���
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_ACTIVE,	// �ғ����
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_SHUTDOWN,	// �V���b�g�_�E��
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_UNKNOWN	// �s�����(�����Ȃ�)
};


// �ő�T�[�o�[��
#define POKE_NET_GDS_RESPONSE_ADMIN_SVR_MAXNUM	(256)		


enum 
{
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_ADMIN,	// �Ǘ��T�[�o�[
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_SLAVE	// �ʏ�T�[�o�[
};

// -- ���N�G�X�g�\���� --
// �Ȃ�

// -- ���X�|���X�\���� --
typedef struct 
{
	long			Type;				// �T�[�o�[��(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_SVRTYPE_xxxx)
	long			Status;				// �X�e�[�^�X(POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA_STATUS_xxxxx)
	long			MaxConnectCount;	// �ő�R�l�N�V�����\��
	long			ConnectCount;		// �R�l�N�V������
	u64				LastUpdateTime;		// �ŏI�X�e�[�^�X�X�V����(0xYYYYMMDDHHMMSS)
	unsigned long	IP;					// �T�[�o�[IP�A�h���X
	long			Request;			// �T�[�o�[�֑����Ă��郊�N�G�X�g�R�[�h
}
POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA;


typedef struct 
{
	long ServerCount;										// �T�[�o�[��
	POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUSDATA Status[0];	// �e�T�[�o�[�̃X�e�[�^�X
}
POKE_NET_GDS_RESPONSE_ADMIN_SVR_STATUS;


//------------------------------------
//        �T�[�o�[�ւ̖���
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_SUCCESS,				// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_AUTH,			// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALIP,		// �s���ȃT�[�o�[�h�o�A�h���X
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALTYPE,	// �ʏ�T�[�o�[�ȊO�ɂ͖��߂ł��Ȃ�
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ILLEGALSTATUS,	// ���߂ł��Ȃ��X�e�[�^�X���
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_ALREADYREQUEST,// ���łɗv���������Ă���T�[�o�[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_SVR_COMMAND_ERROR_UNKNOWN		// ���̑��G���[
};

enum POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_REQCODE 
{
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_NOREQ,						// �v���Ȃ�
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_START,						// �J�n�v��
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_STOP,						// ��~�v��
	POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_SHUTDOWN						// �V���b�g�_�E���v��
};

// -- ���N�G�X�g�\���� --
typedef struct
{
	short Command;														// �R�}���h�ԍ�(POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND_xxx)
	short Option;														// �I�v�V����(���ݖ��g�p)
	unsigned long IP;													// �J�n����T�[�o�[��IP�A�h���X
} 
POKE_NET_GDS_REQUEST_ADMIN_SVR_COMMAND;


// -- ���X�|���X�\���� --
// �Ȃ�


//------------------------------------
//  �~���[�W�J���V���b�g�F�S�̏��
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_STATUS 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_STATUS_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_STATUS_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_STATUS_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

typedef struct 
{
	long TotalCount;													// �o�^����
	// ����ǉ��\��
} 
POKE_NET_GDS_RESPONSE_ADMIN_MUSICALSHOT_STATUS;


//------------------------------------
//  �~���[�W�J���V���b�g�F�폜
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE_SUCCESS,				// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE_ERROR_ILLEGALCODE,	// �R�[�h�G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE_ERROR_AUTH,			// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_MUSICALSHOT_DELETE_ERROR_UNKNOWN			// ���̑��G���[
};

typedef struct
{
	u64 Code;																	// �폜�������R�[�h
} 
POKE_NET_GDS_REQUEST_ADMIN_MUSICALSHOT_DELETE;

// - ���X�|���X -
// ��`����


//------------------------------------
//  �o�g���f�[�^�F�S�̏��
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_SUCCESS,				// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_ERROR_AUTH,			// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_STATUS_ERROR_UNKNOWN			// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

typedef struct 
{
//	long TotalRankingCount;									// ���݂܂ł̃����L���O�m���
	long TotalRegistCount;									// �o�^����
	long TotalEventCount;									// �C�x���g�o�^����
	// ����ǉ��\��
} 
POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_STATUS;


//------------------------------------
//  �o�g���f�[�^�F�폜
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_SUCCESS,				// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_ILLEGALCODE,		// �R�[�h�G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_AUTH,			// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_DELETE_ERROR_UNKNOWN			// ���̑��G���[
};

enum POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA
{
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_ALL						= 0xFF,	// �S��
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_REGIST					= 0x01,	// �o�^�G���A
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_EVENT						= 0x02,	// �C�x���g�G���A

	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_TODAYRANKING_COMM			= 0x04,	// �����̏W�v���ʐM�ΐ탉���L���O
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_YESTERDAYRANKING_COMM		= 0x08,	// �m�肳�ꂽ����̒ʐM�ΐ탉���L���O�G���A

	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_TODAYRANKING_SUBWAY		= 0x10,	// �����̏W�v���o�g���T�u�E�F�C�����L���O
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_YESTERDAYRANKING_SUBWAY	= 0x20	// �m�肳�ꂽ����̃T�u�E�F�C�����L���O�G���A
};

typedef struct 
{
	long TargetArea;										// �^�[�Q�b�g�G���A(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE_AREA_xxxx)
	u64 Code;												// �폜�������R�[�h
} 
POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_DELETE;


// - ���X�|���X -
// ��`����


//------------------------------------
//  �o�g���f�[�^�F�����L���O�����擾
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_RANKHISTORY_ERROR_UNKNOWN	// ���̑��G���[
};

enum POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE 
{
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_RANKING_SUBWAY,	// �o�g���T�u�E�F�C�����L���O
	POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_RANKING_COMM		// �ʐM�ΐ탉���L���O
};

// - �����L���O�������N�G�X�g -
typedef struct 
{															
	long Type;												// �^�C�v(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_xxxx)
	u32 ServerVersion;										// �擾�Ώۃo�g���f�[�^�̃T�[�o�[�o�[�W����
} 
POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY;

// - ���X�|���X -

// - �����L���O���f�[�^��� -
typedef struct 
{											
	s32 PID;												// �v���t�@�C���h�c
	u64 Code;												// �o�g���f�[�^�R�[�h
	u64 Point;												// �l���|�C���g
	u64 RegistTime;											// �o�^����(0xYYYYMMDDHHMMSS)
	u32 FavoriteCount;										// ���C�ɓ���o�^��
	BATTLE_REC_OUTLINE_RECV Data;							// �o�g���f�[�^�w�b�_
}
POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM;

// - �����L���O��� -
typedef struct
{																		
	long Count;															// �q�b�g��
	POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM List[0];	// �e�f�[�^
} 
POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING;

// - �����L���O�������X�|���X -
typedef struct
{														
	long Type;											// �^�C�v(POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_RANKHISTORY_TYPE_xxxx)
	long HitCount;										// �q�b�g���������L���O��
	char RankingList[0];								// �����L���O���X�g
														// (sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_RANKING) +
														//	sizeof(POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY_PARAM) * ���ꂼ��̃����L���O����Count) * HitCount
}
POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_RANKHISTORY;


//------------------------------------
//  �o�g���f�[�^�F�C�x���g�o�^�v��
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_SUCCESS,				// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_ILLEGALCODE,		// �R�[�h�G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_AUTH,				// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFLAG_ERROR_UNKNOWN			// ���̑��G���[
};

typedef struct
{
	u64 Code;																		// �C�x���g�̈�ֈړ����������f�[�^�̃R�[�h
}
POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EVENTFLAG;

// - ���X�|���X -
// ��`����


//------------------------------------
// �o�g���f�[�^�F�C�x���g�f�[�^�̉���
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFREEWORD
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFREEWORD_SUCCESS,
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFREEWORD_NOTFOUND,
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EVENTFREEWORD_ERROR_UNKNOWN
};

typedef struct
{
	u64	Code;
	u8	Message_flag;
	STRCODE Event_self_introduction[EVENT_SELF_INTRO];
	u8	Secure;
}
POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_EVENTFREEWORD;

// - ���X�|���X -
// ��`����


//------------------------------------
//  �o�g���f�[�^�F�Ǘ��p�����[�^�擾
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_SUCCESS,			// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_GETPARAM_ERROR_UNKNOWN	// �G���[
};

// ���ʂɂ��p�����[�^��(���ʐ�+���̑�/�ŐV)
#define	POKE_NET_GDS_BATTLEDATA_GETPARAM_WEIGHT_MAXNUM	(30+2)

// ���ʂɂ��p�����[�^��
#define	POKE_NET_GDS_BATTLEDATA_GETPARAM_SCALING_MAXNUM	(10+1)


// - ���N�G�X�g -
// ��`����


typedef struct 
{
	long Weight[POKE_NET_GDS_BATTLEDATA_GETPARAM_WEIGHT_MAXNUM];			// �|�C���g�d�݂Â�
	long Scaling[POKE_NET_GDS_BATTLEDATA_GETPARAM_SCALING_MAXNUM];			// ���ʂɂ��|�C���g�{��
	s64 InitPoint;															// �����|�C���g�����l
	s64 Bias;																// ���R��臒l
} 
POKE_NET_GDS_RESPONSE_ADMIN_BATTLEDATA_GETPARAM;


//------------------------------------
//  �o�g���f�[�^�F�Ǘ��p�����[�^�ݒ�
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_SUCCESS,			// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_PARAMS,	// �p�����[�^�G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_SETPARAM_ERROR_UNKNOWN	// �G���[
};

// ���ʂɂ��p�����[�^��(���ʐ�+���̑�/�ŐV)
#define	POKE_NET_GDS_BATTLEDATA_SETPARAM_WEIGHT_MAXNUM	(30+2)

// ���ʂɂ��p�����[�^��
#define	POKE_NET_GDS_BATTLEDATA_SETPARAM_SCALING_MAXNUM	(10+1)

// - ���N�G�X�g -
typedef struct
{
	long Weight[POKE_NET_GDS_BATTLEDATA_SETPARAM_WEIGHT_MAXNUM];			// �|�C���g�d�݂Â�
	long Scaling[POKE_NET_GDS_BATTLEDATA_SETPARAM_SCALING_MAXNUM];			// ���ʂɂ��|�C���g�{��
	s64 InitPoint;															// �����|�C���g�����l
	s64 Bias;																// ���R��臒l
}
POKE_NET_GDS_REQUEST_ADMIN_BATTLEDATA_SETPARAM;

// - ���X�|���X -
// ��`����


//------------------------------------
//  �o�g���f�[�^�F�����m�菈��
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_SUCCESS,			// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_BATTLEDATA_EXECDECISION_ERROR_UNKNOWN	// �G���[
};

// - ���N�G�X�g -
// ��`����

// - ���X�|���X -
// ��`����


// ***************************************************
// *                                                 *
// **            �s���`�F�b�N�֘A                   **
// **                ��������                       **


//------------------------------------
// �s���`�F�b�N:�������R�[�h���擾
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_GET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_GET_ERROR_AUTH ,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_ROMCODELIST_MAXNUM (10)
#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEMOLENGTH_MAX (0x40)	// 64�o�C�g

// - ���X�|���X -
typedef struct
{
	u16 m_RomCode;
	STRCODE m_Memo[POKE_NET_GDS_ADMIN_ILLEGALCHECK_MEMOLENGTH_MAX];
}
POKE_NET_GDS_ADMIN_ILLEGAL_ROMCODE_LIST_ITEM;

typedef struct
{								
	long m_Count;
	POKE_NET_GDS_ADMIN_ILLEGAL_ROMCODE_LIST_ITEM m_List[0];
} 
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_ROMCODELIST_GET;


//------------------------------------
// �s���`�F�b�N:�������R�[�h���Z�b�g
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_SET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_SET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_SET_ERROR_AUTH ,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_ROMCODELIST_SET_ERROR_UNKNOWN	// ���̑��G���[
};
	
// - ���N�G�X�g -
typedef struct
{								
	long m_Count;
	POKE_NET_GDS_ADMIN_ILLEGAL_ROMCODE_LIST_ITEM m_List[POKE_NET_GDS_ADMIN_ILLEGALCHECK_ROMCODELIST_MAXNUM];
} 
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_ROMCODELIST_SET;

// - ���X�|���X -
// ��`����


//------------------------------------
// �s���`�F�b�N:�m�f���[�h�̐ݒ�擾
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_ERROR_AUTH ,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_NGWORD_MAXNUM (100)

// - ���X�|���X -
typedef struct
{								
	long Count;			// �m�f���[�h�R�[�h��
	long Codes[0];		// �m�f���[�h�R�[�h
} 
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_NGWORD_GET;


//------------------------------------
// �s���`�F�b�N:�m�f���[�h�̐ݒ�Z�b�g
//------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_ERROR_AUTH ,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_NGWORD_SET_ERROR_UNKNOWN	// ���̑��G���[
};
	
// - ���N�G�X�g -
typedef struct 
{													
	long Count;														// �m�f���[�h�R�[�h��
	long Codes[POKE_NET_GDS_ADMIN_ILLEGALCHECK_NGWORD_MAXNUM];		// �m�f���[�h�R�[�h
} 
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_NGWORD_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_MAXNUM (1000)

// - ���X�|���X -
typedef struct
{
	int m_Count;
	STRCODE m_TitleList[0][MUSICAL_PROGRAM_NAME_MAX];  // �~���[�W�J���^�C�g���̃��X�g((���{18�E�C�O36�{EOM) * m_Count)
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_GET;


//------------------------------------------
// �s���`�F�b�N:�~���[�W�J���V���b�g�̋��^�C�g�����X�g�̐ݒ�
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
typedef struct
{
	int m_Count;
	STRCODE m_TitleList[POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_MAXNUM][MUSICAL_PROGRAM_NAME_MAX];  // �~���[�W�J���^�C�g���̃��X�g((���{18�E�C�O36�{EOM) * m_Count)
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MUSICALTITLELIST_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_MAXNUM (1000)

// - ���X�|���X -
typedef struct
{
	int m_Count;
	u16 m_DisablePokeNoList[0];  // �s���|�P�����ԍ����X�g(m_Count * 2)
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_GET;


//------------------------------------------
// �s���`�F�b�N:�~���[�W�J���V���b�g�̕s���|�P�����ԍ����X�g�̐ݒ�
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
typedef struct
{
	int m_Count;
	u16 m_DisablePokeNoList[POKE_NET_GDS_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_MAXNUM];  // �s���|�P�����ԍ����X�g(m_Count * 2)
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MUSICALDISABLEPOKENO_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�e��ő�l�p�����[�^�̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_GET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_GET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_GET_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

// - ���X�|���X -
typedef struct
{
	u16 m_PokeNoMax;
	u32 m_TRAINER_STATUS_ai_bit_MAX;
	u16 m_TRAINER_STATUS_tr_id_MAX;
	u16 m_TRAINER_STATUS_tr_type_MAX;
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_MAXLIST_GET;


//------------------------------------------
// �s���`�F�b�N:�e��ő�l�p�����[�^�̐ݒ�
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_SET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_SET_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_MAXLIST_SET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
typedef struct
{
	u16 m_PokeNoMax;
	u32 m_TRAINER_STATUS_ai_bit_MAX;
	u16 m_TRAINER_STATUS_tr_id_MAX;
	u16 m_TRAINER_STATUS_tr_type_MAX;
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_MAXLIST_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_MAXNUM (100)

// - ���X�|���X -
typedef struct
{
	int m_Count;
	u16 m_BattleModeValList[0];  // �����ꂽ�o�g�����[�h�l���X�g(m_Count * 2)
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_GET;


//------------------------------------------
// �s���`�F�b�N:�����ꂽ�o�g�����[�h�l���X�g�̐ݒ�
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET_ERROR_AUTH,		// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
typedef struct
{
	int m_Count;
	u16 m_BattleModeValList[POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_MAXNUM];  // �����ꂽ�o�g�����[�h�l���X�g(m_Count * 2)
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_BATTLEMODEVALUELIST_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

#define POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_MAXNUM (100)

// - ���X�|���X -
typedef struct
{
	int m_Count;
	u8 m_BattleVersionList[0];  // �����ꂽ�o�g���o�[�W�������X�g
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_GET;


//------------------------------------------
// �s���`�F�b�N:�����ꂽ�o�g���o�[�W�������X�g�̐ݒ�
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET_SUCCESS,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET_ERROR_AUTH,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
typedef struct
{
	int m_Count;
	u8 m_BattleVersionList[POKE_NET_GDS_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_MAXNUM];  // �����ꂽ�o�g���o�[�W�������X�g
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_BATTLEVERSIONLIST_SET;

// - ���X�|���X -
// ��`����


//------------------------------------------
// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̎擾
//------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET_ERROR_AUTH ,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET_ERROR_UNKNOWN	// ���̑��G���[
};

// - ���N�G�X�g -
// ��`����

// - ���X�|���X -
typedef struct 
{
	STRCODE Name[7][16][PLW_TRAINER_NAME_SIZE];
}
POKE_NET_GDS_RESPONSE_ADMIN_ILLEGALCHECK_DFTRAINERNAME_GET;


//---------------------------------------------
// �s���`�F�b�N:�f�t�H���g�g���C�i�[���̃Z�b�g
//---------------------------------------------
enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET_SUCCESS ,		// ����
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET_ERROR_AUTH ,	// �F�؃G���[
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET_ERROR_UNKNOWN	// ���̑��G���[
};


// - ���N�G�X�g -
typedef struct 
{
	STRCODE Name[7][16][PLW_TRAINER_NAME_SIZE];
}
POKE_NET_GDS_REQUEST_ADMIN_ILLEGALCHECK_DFTRAINERNAME_SET;


// - ���X�|���X -
// ��`����



//------------------------------------
// �s���`�F�b�N���O:�擾
//------------------------------------
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_TIMECAPA	(20)	// ����������̑傫��
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_CONTENTCAPA	(256)	// ���e������̑傫��
#define POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_MAXRESPONSE	(30)	// �ő剽���܂ł̃f�[�^��ԓ����邩

enum POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET 
{
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET_SUCCESS ,
	POKE_NET_GDS_RESPONSE_RESULT_ADMIN_DHOCHECKLOG_GET_ERROR_UNKNOWN
};

// - ���N�G�X�g -
typedef struct 
{
	long	PageNo;		// �y�[�W�ԍ�
} 
POKE_NET_GDS_REQUEST_ADMIN_DHOCHECKLOG_GET;

// - ���X�|���X -
typedef struct
{
	char Time[POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_TIMECAPA];
	char Content[POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_CONTENTCAPA];
}
POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM;

typedef struct
{
	long	HitCount;
	POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET_PARAM	List[0];
} 
POKE_NET_GDS_RESPONSE_ADMIN_DHOCHECKLOG_GET;


// **                �����܂�                       **
// **            �s���`�F�b�N�֘A                   **
// *                                                 *
// ***************************************************




#endif // #ifndef ___POKE_NET_BUILD_DS___

#endif // #ifndef ___POKE_NET_GDS_COMMON___
