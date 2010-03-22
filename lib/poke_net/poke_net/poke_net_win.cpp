//============================================================================
//                         Pokemon Network Library
//============================================================================
#include "poke_net.h"


#ifdef ___POKE_NET_BUILD_WINDOWS___


extern "C" {
POKE_NET_CONTROL POKE_NET_Control;
}

//====================================
//         �f�o�b�O�v�����g
//====================================
void POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL _level ,char *_format ,...)
{
	va_list ap;

	if( _level > POKE_NET_CURRENT_DEBUGLEVEL )return;
	
	va_start(ap, _format);
	vprintf(_format, ap);
	va_end(ap);
}


//====================================
//         �f�o�b�O�v�����g
// [10/03/16]�ǉ��Bunicode�ŁB
//====================================
void POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL _level ,wchar_t *_format ,...)
{
	va_list ap;

	if( _level > POKE_NET_CURRENT_DEBUGLEVEL )return;
	
	va_start(ap, _format);
	vwprintf(_format, ap);
	va_end(ap);
}

//====================================
//           �������m��
//====================================
void *POKE_NET_AllocFunc( long _size )
{
	char *addr;

	addr = new char [_size];
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memalloc %d addr:%08X\n" ,_size ,addr);

	return(addr);
}

//====================================
//              ���
//====================================
void POKE_NET_FreeFunc( void *_ptr  )
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memfree %08X\n" ,_ptr);
	delete [] ((char *)_ptr);
}

//====================================
//      �~���[�e�N�X���b�N
//====================================
void POKE_NET_LockMutex()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL2 ,"#POKE_NET lock mutex %08X\n" ,POKE_NET_Control.MutexHandle);
	WaitForSingleObject(POKE_NET_Control.MutexHandle ,INFINITE);
}

//====================================
//       �~���[�e�N�X�A�����b�N
//====================================
void POKE_NET_UnlockMutex()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL2 ,"#POKE_NET unlock %08X\n" ,POKE_NET_Control.MutexHandle);
	ReleaseMutex(POKE_NET_Control.MutexHandle);
}

//====================================
//      �~���[�e�N�X�쐬
//====================================
BOOL POKE_NET_CreateMutex()
{
	POKE_NET_Control.MutexHandle = CreateMutex(0 ,FALSE ,0);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create mutex : %08X\n" ,POKE_NET_Control.MutexHandle);
	return(TRUE);
}

//====================================
//      �~���[�e�N�X���
//====================================
BOOL POKE_NET_ReleaseMutex()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET close mutex : %08X\n" ,POKE_NET_Control.MutexHandle);
	CloseHandle(POKE_NET_Control.MutexHandle);
	POKE_NET_Control.MutexHandle = NULL;
	return(TRUE);
}

//====================================
//        �@����L������
//====================================
BOOL POKE_NET_ExInitialize()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET exInitialize\n");
	POKE_NET_Control.ThreadLevel = THREAD_PRIORITY_NORMAL;

	// ���P�[���ݒ�
	setlocale(LC_ALL, "");

	return(TRUE);
}

//====================================
//         �@����L���
//====================================
void POKE_NET_ExRelease()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET exreleased\n");
}

//====================================
//        �X���b�h�������b�v
//====================================
void POKE_NET_ThreadFunctionWrap(void *_Param)
{
	POKE_NET_ThreadFunction();
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET exit thread %08X\n" ,POKE_NET_Control.ThreadID);
	_endthread();
}

//====================================
//        �X���b�h�쐬
//====================================
BOOL POKE_NET_CreateThread()
{
	POKE_NET_Control.ThreadID = _beginthread(POKE_NET_ThreadFunctionWrap ,NULL ,NULL);
	SetThreadPriority((HANDLE)POKE_NET_Control.ThreadID ,POKE_NET_Control.ThreadLevel);

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create thread %08X\n" ,POKE_NET_Control.ThreadID);
	if( POKE_NET_Control.ThreadID == -1 ){
		return(FALSE);
	}
	return(TRUE);
}

//=====================================
// IP�A�h���X�����񂪐��������`�F�b�N
//=====================================
BOOL CheckCorrectIP(char *_Data)
{
	long NCount = 0;
	long Mode = 0;

	while(*_Data){
		if( (*_Data >= '0') && (*_Data <= '9') ){
			NCount++;
			if( NCount >= 4 )return(FALSE);
		}else if( *_Data == '.' ){
			if( NCount == 0 )return(FALSE);
			NCount = 0;
			Mode++;
			if( Mode >= 4 )return(FALSE);
		}else return(FALSE);
	
		_Data++;
	}
	if( (Mode == 3) && (NCount != 0) )return(TRUE);

	return(FALSE);
}

//====================================
//           URL����IP�擾
//====================================
BOOL GetIPfromURL(char *_URL ,char *_IP)
{
	struct hostent *HostTmp;
	struct in_addr *IAddress;
	long IP[4];

	if( CheckCorrectIP(_URL) == FALSE ){
		if( HostTmp = gethostbyname(_URL) ){
			IAddress=(LPIN_ADDR)HostTmp->h_addr; //h_addr_list[0];
			strcpy(_IP ,inet_ntoa(*IAddress));
		}else{
			return(FALSE);
		}
	}else{
		sscanf(_URL ,"%d.%d.%d.%d" ,&IP[0] ,&IP[1] ,&IP[2] ,&IP[3]);
		sprintf(_IP ,"%d.%d.%d.%d" ,IP[0] ,IP[1] ,IP[2] ,IP[3]);
	}

	return(TRUE);
}


// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___

//====================================
//    SSL�n���h�V�F�C�N���؃R�[���o�b�N
//====================================
static int Verify_Callback(int _ok, X509_STORE_CTX* _pStore)
{
	char issuer_name[255] = {0};
	char subject_name[255] = {0}; 

	if(!_ok)
	{
		X509* pCert = X509_STORE_CTX_get_current_cert(_pStore);
		int depth	= X509_STORE_CTX_get_error_depth(_pStore);
		int err		= X509_STORE_CTX_get_error(_pStore);

		X509_NAME_oneline(X509_get_issuer_name(pCert), issuer_name, sizeof(issuer_name));
		X509_NAME_oneline(X509_get_subject_name(pCert), subject_name, sizeof(subject_name));

		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
			"POKE_NET_SVR::Verify_Callback >> Failed. >>\n issuer:%s\n subject:%s\n error:%s\n depth:%d\n",
			issuer_name,
			subject_name,
			X509_verify_cert_error_string(err),
			depth
		);
	}

	return _ok;
}

#endif
// ___COMMUNICATES_BY_USING_SSL___


//====================================
//       �Z�b�e�B���O��
//====================================
BOOL POKE_NET_Setting()
{
	unsigned long argp = 1;
	char iptmp[16];

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET network setting\n");

	// �\�P�b�g�쐬
	POKE_NET_Control.Socket = socket(AF_INET, SOCK_STREAM, 0);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create socket %08X\n" ,POKE_NET_Control.Socket);
	if( POKE_NET_Control.Socket == INVALID_SOCKET ){
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_NETWORKSETTING;
		return(FALSE);
	}

	// �\�P�b�g��񓯊��ɐݒ�
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET socket mode to nonblocking %08X\n" ,POKE_NET_Control.Socket);
	if( ioctlsocket( POKE_NET_Control.Socket , FIONBIO , &argp ) == SOCKET_ERROR ){
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_IOCTRLSOCKET;
		return(FALSE);
	}

	// URL����IP�A�h���X�擾
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET gethostbyname %s" ,POKE_NET_Control.URL);
	if( GetIPfromURL(POKE_NET_Control.URL ,iptmp) == FALSE ){
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_NETWORKSETTING;
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ," ... unknown\n");
		return(FALSE);
	}
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ," (%s):%d\n" ,iptmp ,POKE_NET_Control.PortNo);

	// IP�A�h���X�o�^
	POKE_NET_Control.IP = inet_addr(iptmp);

	// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___
	const char SSL_SERVER_CERT_FILENAME[] = "pkgdstest_cert.pem";
	//const char SSL_SERVER_CERT_FILENAME[] = "root.pem";

	// SSL�̏�����
	SSL_library_init(); 
	POKE_NET_Control.m_pSSL_CTX = SSL_CTX_new(SSLv23_client_method());

	// �ݒ�
	SSL_CTX_set_mode(POKE_NET_Control.m_pSSL_CTX, SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER);

	//// �ؖ����`�F�C���̏���ǂݍ���
	//// �����v��Ȃ�
	////if(SSL_CTX_use_certificate_file(m_pSSL_CTX, SSL_SERVER_CERT_FILENAME, SSL_FILETYPE_PEM) != 1)
	//if(SSL_CTX_use_certificate_chain_file(POKE_NET_Control.m_pSSL_CTX, SSL_SERVER_CERT_FILENAME) != 1)
	//{
	//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
	//		"POKE_NET_SVR SSL_CTX_use_certificate_chain_file failed %s.\n", 
	//		SSL_SERVER_CERT_FILENAME
	//	);
	//}
	//else
	//{
	//	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
	//		"POKE_NET_SVR SSL_CTX_use_certificate_chain_file success %s.\n",
	//		SSL_SERVER_CERT_FILENAME
	//	);
	//}

	// �M���o����ؖ����̑g�ݍ���
	if(SSL_CTX_load_verify_locations(POKE_NET_Control.m_pSSL_CTX, SSL_SERVER_CERT_FILENAME, NULL) != 1)
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
			"POKE_NET_SVR load failed %s.\n", 
			SSL_SERVER_CERT_FILENAME
		);
	}
	else
	{
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
			"POKE_NET_SVR load success %s.\n",
			SSL_SERVER_CERT_FILENAME
		);
	}

	// ���؎�ނ̊��蓖��
	// ���؂��t�B���^����R�[���o�b�N�̊��蓖��
	// ��SSL_VERIFY_NONE
	// <server>
	// �@�ؖ����̗v�����N���C�A���g�Ɉ�ؑ��M���ꂸ�A�N���C�A���g�͏ؖ����𑗐M���܂���B
	// <client>
	// �@�T�[�o����󂯎�����ؖ����͑S�Č��؂���܂����A���؂Ɏ��s���Ă��n���h�V�F�C�N���~���܂���B
	// �����̃t���O�͑��̃t���O�Ƒg�ݍ��킹�Ă͑ʖځB
	// ��SSL_VERIFY_PEER
	// <server>
	//   �ؖ����̗v�����N���C�A���g�ɑ��M����܂��B�N���C�A���g�͖����\�B
	//   �ؖ���������Ԃ��ꂽ�ꍇ�ɂ͂��̏ؖ����͌��؂���܂��B���؂Ɏ��s������n���h�V�F�C�N�͒�~����܂��B
	// <client>
	//   �T�[�o���ؖ����𑗐M����ƁA���̏ؖ��������؂���܂��B���؂Ɏ��s������n���h�V�F�C�N�͒�~����܂��B
	//   �T�[�o���ؖ����𑗐M���Ȃ��ꍇ�͓������[�h���g�p����Ă���ꍇ�̂݁B����̓f�t�H���g�ł͖����B
	// ��SSL_VERIFY_FAIL_IF_NO_PEER_CERT
	// <server>
	//   SSL_VERIFY_PEER�ƍ��킹�Ďg�p����K�v������܂��B
	//   �N���C�A���g���ؖ�����񋟂��Ȃ��ꍇ�A�n���h�V�F�C�N�͒�~����܂��B
	// <client>
	//   �T�[�o���[�h�ȊO�ł͖�������܂��B
	// ��SSL_VERIFY_CLIENT_ONCE
	// <server>
	//   SSL_VERIFY_PEER�ƍ��킹�Ďg�p����K�v������܂��B
	//   �l�S�V�G�[�V�������ēx�s���Ă���ꍇ�ɂ́A�N���C�A���g�ؖ�����v�����܂���B�ŏ��̂ݗv���B
	// <client>
	//   �T�[�o���[�h�ȊO�ł͖�������܂��B
	SSL_CTX_set_verify(POKE_NET_Control.m_pSSL_CTX, SSL_VERIFY_NONE, Verify_Callback);
//	SSL_CTX_set_verify(POKE_NET_Control.m_pSSL_CTX, SSL_VERIFY_PEER, Verify_Callback);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
		"POKE_NET_SVR SSL_CTX_set_verify success.\n"
	);

	// �ؖ����`�F�C���̌��ؐ��̐ݒ�(�f�t�H���g=9)
	SSL_CTX_set_verify_depth(POKE_NET_Control.m_pSSL_CTX, 9);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
		"POKE_NET_SVR SSL_CTX_set_verify_depth success.\n"
	);

#endif

	return(TRUE);
}


//====================================
//          �\�P�b�g���
//====================================
void POKE_NET_CloseSocket()
{
	if( POKE_NET_Control.Socket != INVALID_SOCKET ){
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET close socket %08X\n" ,POKE_NET_Control.Socket);
		shutdown(POKE_NET_Control.Socket ,2);
		closesocket(POKE_NET_Control.Socket);
		POKE_NET_Control.Socket = INVALID_SOCKET;
	}

	// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___
	// SSL�̏I��
	if(	POKE_NET_Control.m_pSSL != NULL)
	{
		SSL_shutdown(POKE_NET_Control.m_pSSL); 
		SSL_free(POKE_NET_Control.m_pSSL); 
	}

	if(	POKE_NET_Control.m_pSSL != NULL)
	{
		SSL_CTX_free(POKE_NET_Control.m_pSSL_CTX);
	}
#endif

}

//====================================
//          �ڑ���
//====================================
BOOL POKE_NET_Connecting()
{
//	unsigned long address;
	SOCKADDR_IN sockaddrtmp;
	long errcode;
#ifdef ___COMMUNICATES_BY_USING_SSL___
	int ret;
#endif
	unsigned long timetmp;
	unsigned long starttime;

	memcpy(&sockaddrtmp.sin_addr ,&POKE_NET_Control.IP ,sizeof(unsigned long));
	sockaddrtmp.sin_port = htons(POKE_NET_Control.PortNo);
	sockaddrtmp.sin_family = AF_INET;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect to %s socket:%08X\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket);
	starttime = GetTickCount();
	for(;;){
		if( connect(POKE_NET_Control.Socket ,(LPSOCKADDR)&sockaddrtmp ,sizeof(SOCKADDR_IN)) == SOCKET_ERROR ){
			errcode = WSAGetLastError();
			if( (errcode == WSAEWOULDBLOCK) || (errcode == WSAEALREADY)  || (errcode == WSAEINVAL) ){
				// �u���b�L���O���G���[
				if( POKE_NET_Control.ConnectTimeOut != 0 ){
					// �^�C���A�E�g�ݒ肠��
					timetmp = GetTickCount();
					if( (signed)(timetmp - starttime) >= (POKE_NET_Control.ConnectTimeOut*1000) ){
						// �^�C���A�E�g
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect timeout socket:%08X\n" ,POKE_NET_Control.Socket);
						POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_CONNECTTIMEOUT;
						return(FALSE);
					}
				}
			}else if( errcode == WSAEISCONN ){
				// �ڑ�����Ă���

				// SSL�֌W
#ifdef ___COMMUNICATES_BY_USING_SSL___
				POKE_NET_Control.m_pSSL = SSL_new(POKE_NET_Control.m_pSSL_CTX);
				errcode = SSL_set_fd(POKE_NET_Control.m_pSSL, POKE_NET_Control.Socket);
				if(errcode <= 0)
				{
					POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
						"#POKE_NET POKE_NET_Control() failed. errcode = %d, get_error() = %d\n",
						errcode,
						SSL_get_error(POKE_NET_Control.m_pSSL, errcode)
					);
					ERR_print_errors_fp(stderr);
				}
				else
				{
					POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, "#POKE_NET SSL_set_fd() Succeeded.\n");
				}

				// SSL�ʐM�̊J�n
				while((ret = SSL_connect(POKE_NET_Control.m_pSSL)) <= 0)
				{
					errcode = SSL_get_error(POKE_NET_Control.m_pSSL, ret);

					if(errcode == SSL_ERROR_WANT_READ || errcode == SSL_ERROR_WANT_WRITE)
					{
						continue;
					}
					else
					{
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
							"#POKE_NET SSL_connect() failed. errcode = %d\n",
							errcode
						);
						ERR_print_errors_fp(stderr);

						return FALSE;
					}
				}
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, "#POKE_NET SSL_connect() Succeeded.\n");

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, "#POKE_NET ---------- SSL�ʐM��� ----------\n");
				// session
				//SSL_SESSION* pSSI = SSL_get_session(POKE_NET_Control.m_pSSL);

				// �g�p����Í����������擾
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, 
					"  �Í�������=%s\n",
					SSL_get_cipher(POKE_NET_Control.m_pSSL)
				);

				// ��M�����T�[�o�ؖ��������擾
				X509* pServerCert = SSL_get_peer_certificate(POKE_NET_Control.m_pSSL);

				// �T�[�o�ؖ����̏��L�ҏ��̎擾
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, 
					"  Server certificate::Subject=%s\n",
					X509_NAME_oneline(X509_get_subject_name(pServerCert), 0, 0)
				);

				// �T�[�o�ؖ����̔��s�ҏ��̎擾
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, 
					"  Server certificate::Issuer=%s\n",
					X509_NAME_oneline(X509_get_issuer_name(pServerCert), 0, 0)
				);

				// �F�،��ʂ̃X�e�[�^�X���擾
				// X509_V_OK = 0
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, 
					"#POKE_NET verify_result=%d\n",
					SSL_get_verify_result(POKE_NET_Control.m_pSSL)
				);

				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1, "#POKE_NET ---------- SSL�ʐM��� �����܂�--\n");

				// X509���
				X509_free(pServerCert);
#endif
				break;
			}else{
				// �R�l�N�g���s
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_CONNECT;
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect error socket:%08X\n" ,POKE_NET_Control.Socket);
				return(FALSE);
			}
		}else{
			// �ڑ�����Ă���

			break;
		}

		POKE_NET_LockMutex();
		if( POKE_NET_Control.AbortFlag == TRUE ){
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect abort:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ABORTED;
			POKE_NET_UnlockMutex();
			return(FALSE);
		}
		POKE_NET_UnlockMutex();

		POKE_NET_Sleep(1);
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connected socket:%08X\n" ,POKE_NET_Control.Socket);
	return(TRUE);
}

//====================================
//          ���M��
//====================================
BOOL POKE_NET_Sending()
{
	int ret;
	fd_set fdset; 
	struct timeval timeout;
	int sret;
	long sizetmp;
	long offset;
	unsigned long starttime;
	unsigned long timetmp;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send to %s socket:%08X size:%d\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket ,POKE_NET_Control.SendSize);
	starttime = GetTickCount();
	offset = 0;
	for(;;){
		sizetmp = POKE_NET_Control.SendSize - offset;									// �c��T�C�Y�v�Z
		if( sizetmp > POKE_NET_Control.SendSize )sizetmp = POKE_NET_Control.SendSize;	// ��x�ɑ���ʂ���������␳

		// �u���b�L���O���Ȃ��ŏ������߂�f�[�^�����邩���ׂ�
		FD_ZERO( &fdset ); 
		FD_SET( POKE_NET_Control.Socket , &fdset ); 
		timeout.tv_sec = 0; 
		timeout.tv_usec = 0; 
		sret = select( POKE_NET_Control.Socket+1 ,NULL ,&fdset ,NULL ,&timeout ); 
		if( sret && (sret != -1) ){
#ifdef ___COMMUNICATES_BY_USING_SSL___
			// SSL�֌W
			ret = SSL_write(POKE_NET_Control.m_pSSL, (char *)&POKE_NET_Control.SendBuffer[offset], sizetmp);	// ���M
			if(ret <= 0)
			{
				long errcode = SSL_get_error(POKE_NET_Control.m_pSSL, ret);
				if(errcode != SSL_ERROR_WANT_READ && errcode != SSL_ERROR_WANT_WRITE)
				{
					// < 0:���M�G���[
					// = 0:�ؒf���ꂽ

					// ��M���s���N�������ꍇ��Disconnect����TRUE
					// -----------------------------------------
					// = ���O
					// -----------------------------------------
					POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
						"#POKE_NET send error socket:0x%08X, ret = %d, SSL_ERROR = %d\n" ,
						POKE_NET_Control.Socket,
						ret,
						errcode
					);

					ERR_print_errors_fp(stderr);

					POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_SENDTIMEOUT;
					return(FALSE);
				}
			}
#else
			ret = send(POKE_NET_Control.Socket ,(char *)&POKE_NET_Control.SendBuffer[offset] ,sizetmp ,0 );	// ���M
			if( (ret == SOCKET_ERROR) || (ret == 0) ){
				// ���M�G���[�A�������͐ؒf���ꂽ
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send error socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_SENDTIMEOUT;
				return(FALSE);
			}
#endif

			// �������p�ʕ␳
			offset += ret;
			if( offset == POKE_NET_Control.SendSize ){
				// ���ׂẴf�[�^�𑗂�I����
				break;
			}
		}

		if( POKE_NET_Control.SendTimeOut != 0 ){
			// �^�C���A�E�g�ݒ肠��
			timetmp = GetTickCount();
			if( (signed)(timetmp - starttime) >= (POKE_NET_Control.SendTimeOut*1000) ){
				// �^�C���A�E�g
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send timeout socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_SENDTIMEOUT;
				return(FALSE);
			}
		}

		POKE_NET_LockMutex();
		if( POKE_NET_Control.AbortFlag == TRUE ){
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send abort socket:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ABORTED;
			POKE_NET_UnlockMutex();
			return(FALSE);
		}
		POKE_NET_UnlockMutex();

		POKE_NET_Sleep(1);
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET sent socket:%08X\n" ,POKE_NET_Control.Socket);
	return(TRUE);
}

//====================================
//          ��M��
//====================================
BOOL POKE_NET_Receiving()
{
	int ret;
	fd_set fdset; 
	struct timeval timeout;
	int sret;
	long offset;
	long size;
	unsigned long starttime;
	unsigned long timetmp;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv from %s socket:%08X\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket);
	starttime = GetTickCount();
	offset = 0;
	size = -1;
	for(;;){
		// �u���b�L���O���Ȃ��œǂݍ��߂�f�[�^�����邩���ׂ�
		FD_ZERO( &fdset ); 
		FD_SET( POKE_NET_Control.Socket , &fdset ); 
		timeout.tv_sec = 0; 
		timeout.tv_usec = 0; 
		sret = select( POKE_NET_Control.Socket+1 ,&fdset ,NULL ,NULL ,&timeout ); 
		if( sret && (sret != -1) ){
			// �ǂނׂ��f�[�^������(�������̓N���[�Y����Ă���)
			// ��M

#ifdef ___COMMUNICATES_BY_USING_SSL___
			// SSL�֌W
			ret = SSL_read(POKE_NET_Control.m_pSSL, (char *)&POKE_NET_Control.RecvBuffer[offset], POKE_NET_Control.RecvSize-offset);

			if(ret <= 0)
			{
				long errcode = SSL_get_error(POKE_NET_Control.m_pSSL, ret);
				if(errcode != SSL_ERROR_WANT_READ && errcode != SSL_ERROR_WANT_WRITE)
				{
					// ��M���s���N�������ꍇ��Disconnect����TRUE

					// -----------------------------------------
					// = ���O
					// -----------------------------------------
					POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,
						"#POKE_NET recv error socket:0x%08X, ret = %d, SSL_ERROR = %d\n" ,
						POKE_NET_Control.Socket,
						ret,
						errcode
					);

					fprintf(stderr, "err:");
					ERR_print_errors_fp(stderr);
					fprintf(stderr, "\n");

					POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVRESPONSE;
					return(FALSE);
				}
			}
#else
			ret = recv(POKE_NET_Control.Socket ,(char *)&POKE_NET_Control.RecvBuffer[offset] ,POKE_NET_Control.RecvSize-offset ,0 );

			if( ret == SOCKET_ERROR ){
				// �G���[�F��M�G���[
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv error socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVRESPONSE;
				return(FALSE);
			}else if( ret == 0 ){
				// �ؒf���ꂽ�F��M�G���[
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv disconnected socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVRESPONSE;
				return(FALSE);
			}
#endif
			else{
				// ���o�C�g���󂯎����
				offset += ret;
				if( (offset >= sizeof(long)) && (size == -1) ){
					// �e�ʂ��󂯎�����F����̂ݗp�ʃZ�b�g
					memcpy(&size ,POKE_NET_Control.RecvBuffer ,sizeof(long));
				}
				if( (offset == size) && (size != -1) ){
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
					//// �w��e�ʎ󂯎�����F������
					//POKE_NET_OptDecode(
					//	(unsigned char *)&POKE_NET_Control.RecvBuffer[sizeof(long)] ,
					//	offset - sizeof(long) ,
					//	(unsigned char *)&POKE_NET_Control.RecvBuffer[sizeof(long)]);

					// �T�C�Y�␳
					POKE_NET_Control.RecvSize = size - sizeof(long);
					break;
				}
			}
		}

		if( POKE_NET_Control.RecvTimeOut != 0 ){
			// �^�C���A�E�g�ݒ肠��
			timetmp = GetTickCount();
			if( (signed)(timetmp - starttime) >= (POKE_NET_Control.RecvTimeOut*1000) ){
				// �^�C���A�E�g
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv timeout socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVTIMEOUT;
				return(FALSE);
			}
		}

		POKE_NET_LockMutex();
		if( POKE_NET_Control.AbortFlag == TRUE ){
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv abort socket:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ABORTED;
			POKE_NET_UnlockMutex();
			return(FALSE);
		}
		POKE_NET_UnlockMutex();

		POKE_NET_Sleep(1);
	}

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recved socket:%08X size:%d\n" ,POKE_NET_Control.Socket ,POKE_NET_Control.RecvSize);
	return(TRUE);
}


//====================================
//          ��M��
//====================================
void POKE_NET_Sleep(long _ms)
{
	Sleep(_ms);
}
//=====================================
//		SVL�擾���C�����[�`��
//=====================================
BOOL POKE_NET_GetSvl()
{
	return(TRUE);
}

//=====================================
//		SSL ������
//=====================================
void POKE_NET_SSL_Initialize()
{
}

//=====================================
//		SSL �N���A����
//=====================================
void POKE_NET_SSL_Clear()
{
#ifdef ___COMMUNICATES_BY_USING_SSL___
	POKE_NET_Control.m_pSSL = NULL;
	POKE_NET_Control.m_pSSL_CTX = NULL;
#endif
}

#endif	// ___POKE_NET_BUILD_WINDOWS___
