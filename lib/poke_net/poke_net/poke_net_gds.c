/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net_gds.c

  $NoKeywords: $
 *===========================================================================*/
/*! @file
	@brief	Syachi Pokemon GDS WiFi ���C�u����

*/
//===========================================================================

#include "poke_net_gds.h"
#include "poke_net_common.h"	// Windows/Linux/DS���ʃw�b�_


//====================================
/*!
	�|�P���� WiFi GDS ���C�u���������������܂�

	@param[in]	_auth	�N���C�A���g���<br>
						NULL�̏ꍇ�͑O�ɃZ�b�g���ꂽ�N���C�A���g��񂪎g�p����܂��B
	@param[in]	_Url	URL<br>
	@param[in]	_PortNo	�|�[�g�ԍ�<br>

	@retval		TRUE	�������ɐ������܂���
	@retval		FALSE	�������Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_Initialize(POKE_NET_REQUESTCOMMON_AUTH* _auth, const char* _Url, unsigned short _PortNo)
{
	// ������
	return(POKE_NET_Initialize(_Url ,_PortNo ,_auth));
}


//====================================
/*!
	�|�P���� WiFi GDS ���C�u������������܂�

	�P��� POKE_NET_GDS_Initialize() �֐��ɑ΂��ĂP�񂾂�<br>
	���s����Ȃ���΂Ȃ�܂���B
*/
//====================================
void POKE_NET_GDS_Release()
{
	// POKE_NET�ʐM���
	POKE_NET_Release();
}


//====================================
/*!
	�|�P���� WiFi GDS ���C�u�����̌��݂̏�Ԃ��擾���܂�

	@retval	���݂̏��
*/
//====================================
POKE_NET_GDS_STATUS POKE_NET_GDS_GetStatus()
{
	POKE_NET_STATUS stat;

	POKE_NET_LockMutex();					// �~���[�e�N�X���b�N
	stat = POKE_NET_GetStatus();			// ���݂̃X�e�[�^�X�擾
	POKE_NET_UnlockMutex();					// �~���[�e�N�X�A�����b�N

	return((POKE_NET_GDS_STATUS)stat);
}


//====================================
/*!
	�|�P���� WiFi GDS ���C�u�����̍ŏI�G���[�R�[�h���擾���܂�

	@retval	�ŏI�G���[�R�[�h
*/
//====================================
POKE_NET_GDS_LASTERROR POKE_NET_GDS_GetLastErrorCode()
{
	POKE_NET_LASTERROR code;

	code = POKE_NET_GetLastErrorCode();		// ���݂̃X�e�[�^�X�擾

	return((POKE_NET_GDS_LASTERROR)code);
}


//====================================
/*!
	�|�P���� WiFi GDS ���C�u�����̃��N�G�X�g�����𒆒f���܂�

	���̊֐������s�����̂� POKE_NET_GDS_GetStatus() �֐��̖߂�l��<br>
	POKE_NET_GDS_STATUS_ABORTED�ƂȂ�Β��f���������Ă��܂��B<br>

	@retval TRUE	���f�v���ɐ������܂���
	@retval FALSE	���f�v���Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_Abort()
{
	return(POKE_NET_Abort());
}


//====================================
/*!
	���X�|���X���e���i�[���ꂽ�������̐擪�A�h���X��Ԃ��܂�

	POKE_NET_GDS_GetStatus() �֐��̖߂�l(�X�e�[�^�X)��<br>
	POKE_NET_GDS_STATUS_FINISHED�̂Ƃ��̂ݗL���ł��B<br>
	���N�G�X�g���s���Ɏw�肵��_response�����Ƃ͈Ⴄ�A�h���X��<br>
	�Ԃ��Ă��܂��̂Œ��ӂ��Ă��������B

	@retval ���X�|���X�i�[��̃A�h���X
*/
//====================================
void* POKE_NET_GDS_GetResponse()
{
	return(POKE_NET_GetResponse());
}


//====================================
/*!
	�e���N�G�X�g�̃��X�|���X�T�C�Y���擾���܂�

	POKE_NET_GDS_GetStatus() �֐��̖߂�l(�X�e�[�^�X)��<br>
	POKE_NET_GDS_STATUS_FINISHED�̂Ƃ��̂ݗL���ł��B

	@retval ���X�|���X�e��
*/
//====================================
long POKE_NET_GDS_GetResponseSize()
{
	return(POKE_NET_GetResponseSize());
}


//====================================
/*!
	�e���N�G�X�g�̃��X�|���X�ő�T�C�Y���擾���܂�

	�e���N�G�X�g�̃��X�|���X���i�[����̂ɕK�v�ȍő�T�C�Y��Ԃ��܂��B

	@param[in]	_reqno	���N�G�X�g�ԍ�<br>
						-1���w�肵���ꍇ�����Ƃ����X�|���X�T�C�Y�̑傫�����N�G�X�g��<br>
						���X�|���X�T�C�Y��Ԃ��܂��B<br>

	@retval		0�ȏ�	���X�|���X�e��
	@retval		-1		�G���[
*/
//====================================
long POKE_NET_GDS_GetResponseMaxSize(long _reqno)
{
	return(POKE_NET_GetResponseMaxSize(_reqno));
}


//====================================
/*!
	�ʐM�X���b�h�̎��s���x����ݒ肵�܂��B

	�ʐM�X���b�h�̎��s���x����ύX�������ꍇ�A<br>
	POKE_NET_GDS_Initialize() �֐������s������A<br>
	�e���N�G�X�g�����s����O�ɌĂяo���K�v������܂��B<br>

	@param[in]	_level	�X���b�h���x��<br>
						�f�t�H���g�F16
*/
//====================================
void POKE_NET_GDS_SetThreadLevel(unsigned long _level)
{
	POKE_NET_SetThreadLevel(_level);
}


//====================================
/*!
	�~���[�W�J���V���b�g�̃A�b�v���[�h���s���܂�<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST

	@param[in]		_data		�A�b�v���[�h����~���[�W�J���V���b�g�f�[�^<br>
								�f�[�^�̓��C�u�������Ɋm�ۂ���܂��̂�<br>
								���̊֐��I����ɉ���ł��܂��B
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B


	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_MusicalShotRegist(MUSICAL_SHOT_SEND* _data, void* _response)
{
	POKE_NET_GDS_REQUEST_MUSICALSHOT_REGIST req;

	req.Data = *_data;

	// ���N�G�X�g���s
	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST, &req, _response, -1));
}


//====================================
/*!
	�~���[�W�J���V���b�g�̃_�E�����[�h���s���܂�<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_MUSICALSHOT_GET

	@param[in]		_pokemonno	�_�E�����[�h����~���[�W�J���V���b�g�Ɏʂ��Ă���|�P�����ԍ�
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_GET_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_MUSICALSHOT_GET�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_MusicalShotGet(long _pokemonno, void* _response)
{
	POKE_NET_GDS_REQUEST_MUSICALSHOT_GET req;

	req.SearchType  = POKE_NET_GDS_REQUEST_MUSICALSHOT_GETTYPE_POKEMON;
	req.SearchOpt   = 0;
	req.SearchParam = _pokemonno;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_MUSICALSHOT_GET, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̃A�b�v���[�h���s���܂�<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST

	@param[in]		_data		�A�b�v���[�h�������o�g���f�[�^
	@param[in]		_signModule	�����f�[�^
	@param[in]		_signSize	�����f�[�^�T�C�Y<br>
								�w�肷��l��256�𒴂���Ɗ֐��͉����s�킸��FALSE��Ԃ��܂��B
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_REGIST_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_REGIST�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataRegist(BATTLE_REC_SEND* _data, u8* _signModule, long _signSize, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_REGIST req = {0};

	if(_signSize > POKE_NET_GDS_REQUEST_BATTLEDATA_SIGN_MAXSIZE)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL,
			"#POKE_NET Error. The maximum size of the prepared SignModuleBuffer is (%d). \n",
			POKE_NET_GDS_REQUEST_BATTLEDATA_SIGN_MAXSIZE
		);
		return FALSE;
	}

	req.Data = *_data;
	req.SignSize = _signSize;
	memcpy(req.SignModule, _signModule, (u32)_signSize);	// �����f�[�^���R�s�[

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�����킵������<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH

	@param[in]		_condition	��������<br>
								���������ʂ͐V�������̏��ŕԂ��Ă��܂��B<br>
								������������BATTLE_REC_SEARCH_SEND�\���̂̒��̏������ׂĂ�<br>
								�@�u�w��Ȃ��v�Ƃ����ꍇ�S�f�[�^�������ΏۂƂȂ�܂��B<br>
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataSearchCondition(BATTLE_REC_SEARCH_SEND* _condition, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH req;

	req.SearchType     = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_CONDITION;
	req.ConditionParam = *_condition;
	req.SearchType     = 0;
	req.SearchOpt      = 0;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̍ŐV30���擾<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH

	@param[in]		_svrversion	�擾���郉���L���O���o�g���f�[�^�̗L���T�[�o�[�o�[�W����
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataSearchRankingLatest(u32 _svrversion, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH req;

	memset(&req ,0x00 ,sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH));

	req.SearchType = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_LATEST;
	req.ConditionParam.server_version = _svrversion;
	req.SearchParam = 0;
	req.SearchOpt = 0;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̒ʐM�ΐ탉���L���O�擾<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH

	@param[in]		_svrversion	�擾���郉���L���O���o�g���f�[�^�̗L���T�[�o�[�o�[�W����
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataSearchRankingComm(u32 _svrversion ,void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH req;

	memset(&req ,0x00 ,sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH));

	req.SearchType = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_COMM;
	req.ConditionParam.server_version = _svrversion;
	req.SearchParam = 0;
	req.SearchOpt = 0;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̃o�g���T�u�E�F�C�����L���O�擾<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH

	@param[in]		_svrversion	�擾���郉���L���O���o�g���f�[�^�̗L���T�[�o�[�o�[�W����
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_SEARCH_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_SEARCH�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataSearchRankingSubway(u32 _svrversion, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH req;

	memset(&req ,0x00 ,sizeof(POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCH));

	req.SearchType = POKE_NET_GDS_REQUEST_BATTLEDATA_SEARCHTYPE_RANKING_SUBWAY;
	req.ConditionParam.server_version = _svrversion;
	req.SearchParam = 0;
	req.SearchOpt = 0;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̎擾<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_GET

	@param[in]		_code		�o�g���f�[�^�R�[�h
	@param[in]		_svrversion	�擾�Ώۃo�g���f�[�^�̃T�[�o�[�o�[�W����
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_GET_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_GET�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataGet(u64 _code, u32 _svrversion, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_GET req;

	req.Code = _code;
	req.ServerVersion = _svrversion;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_GET, &req, _response, -1));
}


//====================================
/*!
	�o�g���f�[�^�̂��C�ɓ���o�^<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE

	@param[in]		_code		�o�g���f�[�^�R�[�h
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_xxxx�̒l���Ԃ��Ă��܂��B<br>
								���ꂪPOKE_NET_GDS_RESPONSE_RESULT_BATTLEDATA_FAVORITE_SUCCESS�̏ꍇ�A<br>
								Param�����o�ɂ�POKE_NET_GDS_RESPONSE_BATTLEDATA_FAVORITE�\���̂��Ԃ��Ă��܂��B<br>
								����ȊO�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�͕Ԃ��Ă��܂���̂Œ��ӂ��Ă��������B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_BattleDataFavorite(u64 _code, void* _response)
{
	POKE_NET_GDS_REQUEST_BATTLEDATA_FAVORITE req;

	req.Code = _code;

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE, &req, _response, -1));
}


//====================================
/*!
	�f�o�b�O���b�Z�[�W���N�G�X�g<br>
	<br>
	<b>���s���N�G�X�g:</b>POKE_NET_GDS_REQCODE_DEBUG_MESSAGE

	@param[in]		_message	���b�Z�[�W�̐擪�A�h���X<br>
								�f�[�^�̓��C�u�������Ɋm�ۂ���܂��̂�<br>
								���̊֐��I����ɉ���ł��܂��B
	@param[in,out]	_response	���X�|���X���󂯎�郁�����̐擪�A�h���X<br>
								POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
								Param�����o��POKE_NET_GDS_RESPONSE_DEBUG_MESSAGE�\���̂��Ԃ��Ă��܂��B

	@retval			TRUE		���N�G�X�g�̓o�^�ɐ������܂���
	@retval			FALSE		���N�G�X�g�̓o�^�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_GDS_DebugMessageRequest(char* _message, void* _response)
{	
	POKE_NET_GDS_REQUEST_DEBUG_MESSAGE req;

	strcpy(req.Message ,_message);

	return(POKE_NET_Request(POKE_NET_GDS_REQCODE_DEBUG_MESSAGE ,&req ,_response ,-1));
}









