/*===========================================================================*
  Project:  Syachi PokemonGDS Network Library
  File:     poke_net.c

  $NoKeywords: $
 *===========================================================================*/
//===========================================================================

#include "poke_net.h"
#include <string.h>

extern POKE_NET_CONTROL POKE_NET_Control;

//====================================
/*!
	�|�P����WiFi���C�u�����@���

	�|�P����WiFi���C�u�����̎g�p��Ɏ��s����Ȃ���΂Ȃ�܂���B<br>
	�������A���N�G�X�g������ɍēx���N�G�X�g�𔭍s�������ꍇ��<br>
	POKE_NET_Release�ł͂Ȃ�POKE_NET_Clear�����s���܂��B
*/
//====================================
void POKE_NET_Release()
{
	// �f�[�^�N���A
	POKE_NET_Clear();

	// �N���A�ΏۊO�̃f�[�^������
	memset(&POKE_NET_Control.Auth ,0x00 ,sizeof(POKE_NET_REQUESTCOMMON_AUTH));
    POKE_NET_Control.SvlToken = FALSE;
	POKE_NET_Control.URL[0] = (char)NULL;
	POKE_NET_Control.PortNo = 0;

	// �~���[�e�N�X���
	POKE_NET_ReleaseMutex();

	// �@����L���
	POKE_NET_ExRelease();

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET released\n");
}

//====================================
/*!
	�|�P����WiFi���C�u�����@������

	�|�P����WiFi���C�u�����g�p�O�ɌĂяo���K�v������܂��B

	@param[in]	_url	�ڑ���T�[�o�[�t�q�k<br>
						���C�u���������ɕۑ������̂ŏ����������ł��܂��B
	@param[in]	_portno	�ڑ���T�[�o�[�|�[�g�ԍ�
	@param[in]	_auth	�N���C�A���g���<br>
						NULL�̏ꍇ�͑O�ɃZ�b�g���ꂽ�N���C�A���g��񂪎g�p����܂�

	@retval TRUE	�������ɐ������܂���
	@retval FALSE	�������Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_Initialize(const char *_url ,unsigned short _portno ,POKE_NET_REQUESTCOMMON_AUTH *_auth)
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET initialize\n");

	// �p�����[�^
	if( _auth ){
        POKE_NET_Control.Auth = *_auth;
        POKE_NET_Control.SvlToken = FALSE;
	}

#ifdef ___COMMUNICATES_BY_USING_SSL___
    // SSL ������
    POKE_NET_SSL_Initialize();
#endif

    // �@����L������
	if( !POKE_NET_ExInitialize() )return(FALSE);

	// �~���[�e�N�X�쐬
	POKE_NET_CreateMutex();

	// �ڑ���o�^
	strcpy(POKE_NET_Control.URL ,_url);
	POKE_NET_Control.PortNo = _portno;

	// �e�p�����[�^�N���A
	POKE_NET_Control.SendBuffer = NULL;				// ���M�o�b�t�@(���C�u�����ŗp��)
	POKE_NET_Control.Socket = INVALID_SOCKET;		// �\�P�b�g
	POKE_NET_Clear();

	return(TRUE);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�N���A

	���N�G�X�g�I����A�ēx���N�G�X�g��<br>
	�Ăяo���O�Ɏ��s����Ȃ���΂Ȃ�܂���B
*/
//====================================
void POKE_NET_Clear()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET clear\n");

	// �\�P�b�g�N���[�Y
	POKE_NET_CloseSocket();

	// ���M�o�b�t�@���
	if( POKE_NET_Control.SendBuffer ){
		POKE_NET_FreeFunc(POKE_NET_Control.SendBuffer);
		POKE_NET_Control.SendBuffer=NULL;
	}

	// �e�p�����[�^������
	POKE_NET_Control.ConnectTimeOut = POKE_NET_CONNECT_TIMEOUT;	// �R�l�N�g�^�C���A�E�g����(sec)
	POKE_NET_Control.SendTimeOut = POKE_NET_SEND_TIMEOUT;		// ���M�^�C���A�E�g����(sec)
	POKE_NET_Control.RecvTimeOut = POKE_NET_RECV_TIMEOUT;		// ��M�^�C���A�E�g����(sec)
	POKE_NET_Control.SendSize = 0;								// ���M�T�C�Y
	POKE_NET_Control.RecvSize = 0;								// ��M�T�C�Y
	POKE_NET_Control.RecvBuffer = NULL;							// ��M�o�b�t�@(���[�U�[�ŗp��)
	POKE_NET_Control.IP = 0;									// �ڑ���h�o
	POKE_NET_Control.AbortFlag = FALSE;							// �A�{�[�g�v��

	// �X�e�[�^�X���������ςֈڍs
	POKE_NET_Control.Status = POKE_NET_STATUS_INITIALIZED;
	POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_NONE;

	// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___
    POKE_NET_SSL_Clear();
#endif
}

//====================================
//           �X���b�h�֐�
//====================================
void POKE_NET_ThreadFunction()
{
	int flag;
	POKE_NET_STATUS chstat;
		
	// �X���b�h
	for(;;){
		// �X���b�h�I���v���`�F�b�N
		flag = FALSE;
		switch(POKE_NET_Control.Status){
			case POKE_NET_STATUS_INACTIVE:					// ��ғ���
			case POKE_NET_STATUS_INITIALIZED:				// �������ς�
			case POKE_NET_STATUS_ABORTED:					// ���f
			case POKE_NET_STATUS_FINISHED:					// ����I��
			case POKE_NET_STATUS_ERROR:						// �G���[�I��
				flag = TRUE;
				break;
			case POKE_NET_STATUS_REQUEST:					// ���N�G�X�g���s
				// �X���b�h�N�������Ɠ����ɂ��̏�Ԃ͉��������
            	chstat = POKE_NET_STATUS_REQUEST;
	            if( POKE_NET_GetSvl() == FALSE ){
                    if( POKE_NET_Control.LastErrorCode == POKE_NET_LASTERROR_ABORTED ){
                        chstat = POKE_NET_STATUS_ABORTED;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET request aborted\n");
                    } else {
                        chstat = POKE_NET_STATUS_ERROR;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET request error\n");
                    }
                    flag = TRUE;
                } else {
                    chstat = POKE_NET_STATUS_NETSETTING;
                }
				POKE_NET_LockMutex();
				POKE_NET_Control.Status = chstat;
				break;
			case POKE_NET_STATUS_NETSETTING:				// �l�b�g���[�N�ݒ蒆
				if( POKE_NET_Setting() == FALSE ){
					if( POKE_NET_Control.LastErrorCode == POKE_NET_LASTERROR_ABORTED ){
						chstat = POKE_NET_STATUS_ABORTED;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET setting aborted\n");
					}else{
						chstat = POKE_NET_STATUS_ERROR;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET setting error\n");
					}
					flag = TRUE;
				}else{
					chstat = POKE_NET_STATUS_CONNECTING;
				}
				POKE_NET_LockMutex();
				POKE_NET_Control.Status = chstat;
				break;
			case POKE_NET_STATUS_CONNECTING:				// �ڑ���
				if( POKE_NET_Connecting() == FALSE ){
					if( POKE_NET_Control.LastErrorCode == POKE_NET_LASTERROR_ABORTED ){
						chstat = POKE_NET_STATUS_ABORTED;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET connecting aborted\n");
					}else{
						chstat = POKE_NET_STATUS_ERROR;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET connecting error\n");
					}
					flag = TRUE;
				}else{
					chstat = POKE_NET_STATUS_SENDING;
				}
				POKE_NET_LockMutex();
				POKE_NET_Control.Status = chstat;
				break;
			case POKE_NET_STATUS_SENDING:					// ���M��
				if( POKE_NET_Sending() == FALSE ){
					if( POKE_NET_Control.LastErrorCode == POKE_NET_LASTERROR_ABORTED ){
						chstat = POKE_NET_STATUS_ABORTED;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET sending aborted\n");
					}else{
						chstat = POKE_NET_STATUS_ERROR;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET sending error\n");
					}
					flag = TRUE;
				}else{
					chstat = POKE_NET_STATUS_RECEIVING;
				}

				// ���M�o�b�t�@�͂����ŕs�v�ɂȂ�̂ŉ��
				if( POKE_NET_Control.SendBuffer ){
					POKE_NET_FreeFunc(POKE_NET_Control.SendBuffer);
					POKE_NET_Control.SendBuffer=NULL;
				}

				POKE_NET_LockMutex();
				POKE_NET_Control.Status = chstat;
				break;
			case POKE_NET_STATUS_RECEIVING:					// ��M��
				if( POKE_NET_Receiving() == FALSE ){
					if( POKE_NET_Control.LastErrorCode == POKE_NET_LASTERROR_ABORTED ){
						chstat = POKE_NET_STATUS_ABORTED;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET receiving aborted\n");
					}else{
						chstat = POKE_NET_STATUS_ERROR;
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET receiving error\n");
					}
				}else{
					chstat = POKE_NET_STATUS_FINISHED;
					POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET receive finished\n");
				}
				flag = TRUE;
				POKE_NET_LockMutex();
				POKE_NET_Control.Status = chstat;
				break;
		}

		if( flag ){
			// ����������or�G���[�I������
			break;
		}else{
			// �����ŃA�{�[�g����
			if( POKE_NET_Control.AbortFlag == TRUE ){
				// �A�{�[�g�v�������Ă���F�����ŏ����𒆒f����
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET thread aborted\n");
				POKE_NET_Control.Status = POKE_NET_STATUS_ABORTED;
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ABORTED;
				break;
			}
			POKE_NET_UnlockMutex();
		}
		POKE_NET_Sleep(1);
	}

	POKE_NET_UnlockMutex();
}

//====================================
/*!
	�|�P����WiFi���C�u�����@���N�G�X�g���s

	�T�[�o�[�ɑ΂��ă��N�G�X�g�𔭍s���܂��B

	@param[in]	_reqno		���N�G�X�g�ԍ�
	@param[in]	_param		���N�G�X�g���e<br>
							�T�C�Y�̓��N�G�X�g�ԍ����玩���v�Z����܂��B<br>
							���N�G�X�g���e�̓��C�u���������ɕۑ������̂�<br>
							�����������ł��܂��B
	@param[in]	_response	���X�|���X���e���󂯎�郁�����̐擪�A�h���X<br>
							POKE_NET_RESPONSE�\���̂��Ԃ��Ă��܂��B<br>
							Result�����o�ɂ�ReqCode�����o�ɑΉ�����POKE_NET_GDS_RESPONSE_RESULT_xxxx��<br>
							�Ԃ��Ă��Ă���AParam�����o�ɂ�ReqCode�����o�ɑΉ��������X�|���X�\���̂�<br>
							�Ԃ��Ă��܂��B<br>
							�G���[�̏ꍇ�ɂ�Param�����o�ɂ̓f�[�^�������Ă���܂���̂Œ��ӂ��Ă��������B<br>
							<br>
							cf. ReqCode�����o�̒l��POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST�̏ꍇ�ɂ�<br>
							�@Result�����o�ɂ�POKE_NET_GDS_RESPONSE_RESULT_MUSICALSHOT_REGIST_xxxx�̌��ʃR�[�h���Ԃ�<br>
							�@SUCCESS�̏ꍇ�ɂ�Param�����o�ɂ�POKE_NET_GDS_RESPONSE_MUSICALSHOT_REGIST�\���̂��Ԃ��Ă��܂��B


	@retval TRUE	���N�G�X�g���s�ɐ������܂���
	@retval FALSE	���N�G�X�g���s�Ɏ��s���܂���
*/
//====================================
BOOL POKE_NET_Request(unsigned short _reqno ,void *_param ,void *_response ,long _size)
{
	long size;
	POKE_NET_REQUEST *reqhead;

	POKE_NET_LockMutex();
	switch(POKE_NET_Control.Status){
		// ��Ԃ��N���A����
		case POKE_NET_STATUS_INACTIVE:					// ��ғ���
		case POKE_NET_STATUS_REQUEST:					// ���N�G�X�g���s
		case POKE_NET_STATUS_NETSETTING:				// �l�b�g���[�N�ݒ蒆
		case POKE_NET_STATUS_CONNECTING:				// �ڑ���
		case POKE_NET_STATUS_SENDING:					// ���M��
		case POKE_NET_STATUS_RECEIVING:					// ��M��
			// �������A���������Ă��Ȃ���Ԃ̓G���[
			POKE_NET_UnlockMutex();
			POKE_NET_Control.Status = POKE_NET_STATUS_ERROR;
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_NOTINITIALIZED;
			return(FALSE);
		case POKE_NET_STATUS_INITIALIZED:				// �������ς�
			// ����������̓X���[
			break;
		case POKE_NET_STATUS_ABORTED:					// ���f
		case POKE_NET_STATUS_FINISHED:					// ����I��
		case POKE_NET_STATUS_ERROR:						// �G���[�I��
			// ��������ŁA�������������Ă���
			POKE_NET_Clear();
			break;
	}
	POKE_NET_UnlockMutex();

	// ��M�o�b�t�@�̓o�^
	POKE_NET_Control.RecvSize   = POKE_NET_GetResponseMaxSize(_reqno);
	if( POKE_NET_Control.RecvSize == -1 ){
		POKE_NET_Control.Status = POKE_NET_STATUS_ERROR;
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ILLEGALREQUEST;
		return(FALSE);
	}
	POKE_NET_Control.RecvBuffer = (char *)_response;

	// �o�b�t�@�ő�T�C�Y�ƃ��N�G�X�g�ԍ��`�F�b�N
	if( _size != -1 ){
		size = (long)(_size + sizeof(long) + sizeof(POKE_NET_REQUEST));
	}else{
		size = POKE_NET_GetRequestSize(_reqno);
		if( size == -1 ){
			POKE_NET_Control.Status = POKE_NET_STATUS_ERROR;
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ILLEGALREQUEST;
			return(FALSE);
		}
	}

	// ���M�o�b�t�@�m��
	POKE_NET_Control.SendBuffer = (char*)POKE_NET_AllocFunc(size);
	if( POKE_NET_Control.SendBuffer == NULL )return(FALSE);

	// ���M�T�C�Y�o�^
	POKE_NET_Control.SendSize = size;

	// ���M�p�P�b�g�o�^
	memcpy(&POKE_NET_Control.SendBuffer[0] ,&size ,sizeof(long));
	reqhead = (POKE_NET_REQUEST *)&POKE_NET_Control.SendBuffer[sizeof(long)];
	reqhead->ReqCode  = _reqno;
	reqhead->Auth     = POKE_NET_Control.Auth;
	reqhead->Option   = 0;
	memcpy(reqhead->Param ,_param ,size - sizeof(long)- sizeof(POKE_NET_REQUEST));

	// *******************************************************************
	// [10/03/05]
	// SSL�ʐM�𗘗p���邽�߁A�Í����̕K�v�������Ȃ����B
	// <�Y���\�[�X>
	// poke_net_svr_conn.cc line:322�O��	�T�[�o����M
	// poke_net_svr_conn.cc line:617�O��	�T�[�o�����M
	// poke_net.c			line:359�O��	�N���C�A���g���ʑ��M
	// poke_net_win.cpp		line:668�O��	Win����M
	// poke_net_ds.cpp		line:469�O��	DS����M
	// *******************************************************************
	//// �Í���
	//POKE_NET_OptEncode(
	//	(unsigned char *)&POKE_NET_Control.SendBuffer[sizeof(long)] ,
	//	(long)(POKE_NET_Control.SendSize - sizeof(long)) ,
	//	(unsigned char *)&POKE_NET_Control.SendBuffer[sizeof(long)]);

	// ���M�ֈڍs
	POKE_NET_Control.Status = POKE_NET_STATUS_REQUEST;

	// �X���b�h�J�n
	POKE_NET_LockMutex();
	if( POKE_NET_CreateThread() == FALSE ){
		POKE_NET_Control.Status = POKE_NET_STATUS_ERROR;
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_CREATETHREAD;
		POKE_NET_UnlockMutex();
		return(FALSE);
	}
	POKE_NET_UnlockMutex();

	return(TRUE);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�X�e�[�^�X�擾

	���݂̃��C�u�����̏�Ԃ��擾���܂��B<br>
	POKE_NET_LockMutex<br>
	POKE_NET_UnlockMutex<br>
	��p���ă~���[�e�N�X���b�N��ԂŎ擾����K�v������܂��B

	@retval	���݂̏��
*/
//====================================
POKE_NET_STATUS POKE_NET_GetStatus()
{
	return(POKE_NET_Control.Status);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@���f�v��

	���N�G�X�g�����𒆒f����v�����o���܂��B<br>
	���f�v����A�|�P����WiFi���C�u�����̏�Ԃ�<br>
	POKE_NET_GDS_STATUS_ABORTED�ɂȂ�܂ő҂��˂΂Ȃ�܂���B<br>
	�܂��A�X���b�h�����Œ��f�v���𔭍s�����ɂ��ւ�炸<br>
	POKE_NET_GDS_STATUS_FINISHED<br>
	POKE_NET_GDS_STATUS_ERROR<br>
	�Ȃǂ̏�Ԃ֑J�ڂ��邱�Ƃ�����܂��B

	@retval TRUE	�v�����o�����Ƃɐ������܂���
	@retval FALSE	�v�����o�����ƂɎ��s���܂���
*/
//====================================
BOOL POKE_NET_Abort()
{
	POKE_NET_LockMutex();
	if( POKE_NET_Control.Status >= POKE_NET_STATUS_NETSETTING ){
		if( POKE_NET_Control.Status <= POKE_NET_STATUS_RECEIVING ){
			POKE_NET_Control.AbortFlag = TRUE;
		}
	}
	POKE_NET_UnlockMutex();
	return(TRUE);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�ԓ��擾

	���N�G�X�g�ɑ΂���ԓ����擾���܂��B<br>
	��Ԃ�POKE_NET_STATUS_FINISHED�̂Ƃ��̂ݎg�p�ł��܂��B<br>
	�߂�l�̃A�h���X��POKE_NET_Request�֐���<br>
	�w�肵��_response�Ɠ������̂ł��B<br>

	@retval NULL�ȊO	�擾�ɐ������܂���
	@retval NULL		�擾�Ɏ��s���܂���
*/
//====================================
void *POKE_NET_GetResponse()
{
	POKE_NET_LockMutex();
	if( POKE_NET_GetStatus() != POKE_NET_STATUS_FINISHED ){
		POKE_NET_UnlockMutex();
		return(NULL);
	}
	POKE_NET_UnlockMutex();

	return(POKE_NET_Control.RecvBuffer+sizeof(long));
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�ԓ��T�C�Y�擾

	���N�G�X�g�ɑ΂���ԓ��̃T�C�Y���擾���܂��B<br>
	��Ԃ�POKE_NET_STATUS_FINISHED�̂Ƃ��̂ݎg�p�ł��܂��B

	@retval NULL�ȊO	�擾�ɐ������܂���
	@retval NULL		�擾�Ɏ��s���܂���
*/
//====================================
long POKE_NET_GetResponseSize()
{
	POKE_NET_LockMutex();
	if( POKE_NET_GetStatus() != POKE_NET_STATUS_FINISHED ){
		POKE_NET_UnlockMutex();
		return(-1);
	}
	POKE_NET_UnlockMutex();

	return(POKE_NET_Control.RecvSize);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�ŏI�G���[�R�[�h�擾

	�G���[�I�����̍ŏI�G���[�R�[�h���擾���܂��B<br>
	��Ԃ�POKE_NET_STATUS_ERROR�̂Ƃ��̂ݎg�p�ł��܂��B

	@retval POKE_NET_LASTERROR_NONE	�擾�Ɏ��s���܂���
	@retval POKE_NET_LASTERROR_NONE�ȊO	�擾�ɐ������܂���
*/
//====================================
POKE_NET_LASTERROR POKE_NET_GetLastErrorCode()
{
	POKE_NET_LockMutex();
	if( POKE_NET_GetStatus() != POKE_NET_STATUS_ERROR ){
		POKE_NET_UnlockMutex();
		return(POKE_NET_LASTERROR_NONE);
	}
	POKE_NET_UnlockMutex();

	return(POKE_NET_Control.LastErrorCode);
}

//====================================
/*!
	�|�P����WiFi���C�u�����@�X���b�h���x���ݒ�

	�����ŋN������ʐM�X���b�h�̃��x�����w�肵�܂�<br>
	POKE_NET_Initialize�̌�ɌĂ΂��K�v������܂��B<br>
	POKE_NET_Clear�ł͏���������܂���B
*/
//====================================
void POKE_NET_SetThreadLevel(unsigned long _level)
{
	POKE_NET_Control.ThreadLevel = _level;
}
