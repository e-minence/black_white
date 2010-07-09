//============================================================================
//                         Pokemon Network Library
//============================================================================
#include "poke_net.h"

#ifdef ___POKE_NET_BUILD_DS___

#include "poke_net_ds_ssl.h"
#include <nitro/std.h>
#include <wchar.h>

POKE_NET_CONTROL POKE_NET_Control;

#define POKE_NET_DS_THREADSTACKSIZE	2048
static char POKE_NET_DS_ThreadStack[POKE_NET_DS_THREADSTACKSIZE];
static char POKE_NET_DS_ThreadStackRecv[POKE_NET_DS_THREADSTACKSIZE];

//====================================
//         �v���g�^�C�v�錾
//====================================
BOOL CheckCorrectIP(char *_Data);
BOOL GetIPfromURL(char *_URL ,char *_IP);

//====================================
//         �f�o�b�O�v�����g
//====================================
#ifndef SDK_FINALROM
void POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL _level ,char *_format ,...)
{
	va_list ap;
	char mestmp[1024];

	if( _level > POKE_NET_CURRENT_DEBUGLEVEL )return;

	va_start(ap, _format);
	STD_TVSPrintf(mestmp ,_format, ap);
	va_end(ap);

    OS_TVPrintf(mestmp, ap);
}
void POKE_NET_DebugPrintfW(POKE_NET_DEBUGLEVEL _level ,wchar_t *_format ,...)
{
    const int LEN = 1024;
   	va_list ap;
	wchar_t mestmp[LEN] = L"";
	char buff[LEN] = "";
    STDResult result;

	if( _level > POKE_NET_CURRENT_DEBUGLEVEL )return;

	va_start(ap, _format);
	vswprintf(mestmp, LEN, _format, ap);
	va_end(ap);

//	{
//		int i = 0;
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "POKE_NET_DebugPrintfW >> Memory dump\n");
//		for(i = 0; i < 32; i++)
//		{
//			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "%04x ", mestmp[i]);
//		}
//		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL, "\ndump end.\n");
//	}
	
    result = STD_ConvertStringUnicodeToSjis(buff, NULL, mestmp, NULL, NULL);

	OS_TPrintf(buff);
	
}
#endif

//====================================
//           �������m��
//====================================
void *POKE_NET_AllocFunc( long _size )
{
	char *ptr;
/*
	OSIntrMode old;

	old = OS_DisableInterrupts();
	ptr = (char *)OS_AllocFromMain( (unsigned long)_size );
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memalloc %d addr:%08X\n" ,_size ,ptr);
	OS_RestoreInterrupts( old );
*/
	ptr = DWC_Alloc((DWCAllocType)0 ,(unsigned long)_size);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memalloc %d addr:%08X\n" ,_size ,ptr);

	return(ptr);
}

//====================================
//              ���
//====================================
void POKE_NET_FreeFunc( void *_ptr )
{
/*
	OSIntrMode old;

	if( !_ptr )return;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memfree %08X\n" ,_ptr);

	old = OS_DisableInterrupts();

	OS_FreeToMain( _ptr );
	OS_RestoreInterrupts( old );
*/
	if( !_ptr )return;

	DWC_Free((DWCAllocType)0 ,_ptr ,0);
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET memfree %08X\n" ,_ptr);
}

//====================================
//      �~���[�e�N�X���b�N
//====================================
void POKE_NET_LockMutex()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL2 ,"#POKE_NET lock mutex\n");
	OS_LockMutex( &POKE_NET_Control.MutexHandle );
}

//====================================
//       �~���[�e�N�X�A�����b�N
//====================================
void POKE_NET_UnlockMutex()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL2 ,"#POKE_NET unlock\n");
	OS_UnlockMutex( &POKE_NET_Control.MutexHandle );
}

//====================================
//      �~���[�e�N�X�쐬
//====================================
BOOL POKE_NET_CreateMutex()
{
	OS_InitMutex( &POKE_NET_Control.MutexHandle );
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create mutex\n");
	return(TRUE);
}

//====================================
//      �~���[�e�N�X���
//====================================
BOOL POKE_NET_ReleaseMutex()
{
	// DS��MUTEX�͉�����Ȃ��Ă悢
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET close mutex\n");
	return(TRUE);
}

//====================================
//        �@����L������
//====================================
BOOL POKE_NET_ExInitialize()
{
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET exinialize\n");
	if( OS_IsTickAvailable() == FALSE ){
		OS_InitTick();
	}

	// �X���b�h�f�t�H���g���x��
	POKE_NET_Control.ThreadLevel = 16;

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
static void POKE_NET_ThreadFunctionWrap(void *_arg)
{
#pragma unused(_arg)
	POKE_NET_ThreadFunction();
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET exit thread\n");
}

//====================================
//        �X���b�h�쐬
//====================================
BOOL POKE_NET_CreateThread()
{
	// �X���b�h�쐬
	OS_CreateThread(&POKE_NET_Control.ThreadID ,
		POKE_NET_ThreadFunctionWrap ,
		NULL ,
		POKE_NET_DS_ThreadStack+POKE_NET_DS_THREADSTACKSIZE ,
		POKE_NET_DS_THREADSTACKSIZE ,
		POKE_NET_Control.ThreadLevel
	);
	OS_WakeupThreadDirect(&POKE_NET_Control.ThreadID);

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create thread\n");

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
	SOCHostEnt *hostent;
	SOCInAddr addr;
/*
	OS_TPrintf("IP      = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(SOCL_GetHostID()));
	OS_TPrintf("NetMask = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSNetMask));
	OS_TPrintf("GW IP   = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSGatewayIp));
	OS_TPrintf("DNS[0]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[0]));
	OS_TPrintf("DNS[1]  = %3d.%3d.%3d.%3d\n", CPS_CV_IPv4(CPSDnsIp[1]));
*/
	if( CheckCorrectIP(_URL) == FALSE ){
		if( (hostent = SOC_GetHostByName(_URL)) != NULL ){
			addr.addr = *(unsigned int *)hostent->addrList[0];
			strcpy(_IP ,SOC_InetNtoA(addr));
		}else{
			return(FALSE);
		}
	}else{
		strcpy(_IP ,_URL);
	}

	return(TRUE);
}

//====================================
//       �Z�b�e�B���O��
//====================================
BOOL POKE_NET_Setting()
{
	unsigned long argp = 1;
	char iptmp[16];
	int val;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_NORMAL ,"#POKE_NET network setting\n");

	// �\�P�b�g�쐬
	POKE_NET_Control.Socket = SOC_Socket( SOC_PF_INET, SOC_SOCK_STREAM, 0 );
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create socket %08X\n" ,POKE_NET_Control.Socket);
	if( POKE_NET_Control.Socket == INVALID_SOCKET ){
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_CREATESOCKET;
		return(FALSE);
	}

	// �\�P�b�g��񓯊��ɐݒ�
    // SSL �ł͓����̂݃T�|�[�g
#ifndef ___COMMUNICATES_BY_USING_SSL___
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET socket mode to nonblocking %08X\n" ,POKE_NET_Control.Socket);
	val = SOC_Fcntl(POKE_NET_Control.Socket ,SOC_F_GETFL ,0);
	if( SOC_Fcntl(POKE_NET_Control.Socket ,SOC_F_SETFL ,val | SOC_O_NONBLOCK) < 0 ){
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_IOCTRLSOCKET;
		return(FALSE);
	}
#endif

	// URL����IP�A�h���X�擾
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET gethostbyname %s" ,POKE_NET_Control.URL);
	if( GetIPfromURL(POKE_NET_Control.URL ,iptmp) == FALSE ){
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ," ... unknown\n");
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_NETWORKSETTING;
		return(FALSE);
	}
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ," %s:%d\n" ,iptmp ,POKE_NET_Control.PortNo);

	// IP�A�h���X�o�^
	POKE_NET_Control.IP = CPS_htonl(CPS_Resolve(iptmp));

#ifdef ___COMMUNICATES_BY_USING_SSL___
#ifndef ___ALWAYS_ENABLE_SSL___
    // �A�h���X�� https �Ŏn�܂��Ă���ꍇ�� SSL ���g�p����
    if (STD_CompareString(POKE_NET_Control.URL, "https") == 0)
    {    
#endif

    SslCreateConnection(POKE_NET_Control.URL, &POKE_NET_Control.sslConnection);
    if ((val = SOC_EnableSsl(POKE_NET_Control.Socket, &POKE_NET_Control.sslConnection)) < 0)
    {
		POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ," ... ssl connection failed\n");
		POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_SSL_SETTING;
        return (FALSE);
    }

#ifndef ___ALWAYS_ENABLE_SSL___
    }    
#endif

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
		SOC_Shutdown( POKE_NET_Control.Socket ,SOC_SHUT_RDWR );
		SOC_Close( POKE_NET_Control.Socket );
		POKE_NET_Control.Socket = INVALID_SOCKET;
#ifdef ___COMMUNICATES_BY_USING_SSL___
        SslDestroyConnection(&POKE_NET_Control.sslConnection);
#endif
	}
}

//====================================
//          �ڑ���
//====================================
BOOL POKE_NET_Connecting()
{
	SOCSockAddrIn sockaddrtmp;
	int ret;
	OSTick starttime;
	OSTick timetmp;
	long timtmps;

	sockaddrtmp.len = sizeof( SOCSockAddrIn );
	sockaddrtmp.family = SOC_PF_INET;
	sockaddrtmp.port = SOC_HtoNs( POKE_NET_Control.PortNo );
	sockaddrtmp.addr.addr = POKE_NET_Control.IP;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect to %s socket:%08X\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket);
	starttime = OS_GetTick();
	for(;;){
		if( (ret = SOC_Connect(POKE_NET_Control.Socket ,&sockaddrtmp)) < 0 ){
			if( ret == SOC_EINPROGRESS ){
				// �u���b�L���O��
				if( POKE_NET_Control.ConnectTimeOut != 0 ){
					// �^�C���A�E�g�ݒ肠��
					timetmp = OS_GetTick();
					timtmps = (long)(timetmp/(OS_SYSTEM_CLOCK / 64) - starttime/(OS_SYSTEM_CLOCK / 64));
					if( timtmps >= POKE_NET_Control.ConnectTimeOut ){
						// �^�C���A�E�g
						POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET connect timeout socket:%08X\n" ,POKE_NET_Control.Socket);
						POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_CONNECTTIMEOUT;
						return(FALSE);
					}
				}
			}else if( ret == SOC_EISCONN ){
				// �ڑ�����Ă���
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
	long sizetmp;
	long timtmps;
	long offset;
	OSTick starttime;
	OSTick timetmp;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send to %s socket:%08X size:%d\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket ,POKE_NET_Control.SendSize);
	offset = 0;
	starttime = OS_GetTick();
	for(;;){
		sizetmp = POKE_NET_Control.SendSize - offset;									// �c��T�C�Y�v�Z
		if( sizetmp > POKE_NET_Control.SendSize )sizetmp = POKE_NET_Control.SendSize;	// ��x�ɑ���ʂ���������␳
		ret = SOC_Send(POKE_NET_Control.Socket ,(char *)&POKE_NET_Control.SendBuffer[offset] ,sizetmp ,0 );	// ���M
		if( ret == SOC_EWOULDBLOCK ){
			// ��u���b�L���O
		}else if( ret < 0 ){
			// ���M�G���[�A�������͐ؒf���ꂽ
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET send error socket:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_SENDTIMEOUT;
			return(FALSE);
		}else{
			// �������p�ʕ␳
			offset += ret;
			if( offset == POKE_NET_Control.SendSize ){
				// ���ׂẴf�[�^�𑗂�I����
				break;
			}
		}

		if( POKE_NET_Control.SendTimeOut != 0 ){
			// �^�C���A�E�g�ݒ肠��
			timetmp = OS_GetTick();
			timtmps = (long)(timetmp/(OS_SYSTEM_CLOCK / 64) - starttime/(OS_SYSTEM_CLOCK / 64));
			if( timtmps >= POKE_NET_Control.SendTimeOut ){
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

#ifdef RECEIVE_THREAD
//====================================
//          ��M�X���b�h
//====================================
static void POKE_NET_ThreadRecv(void* _arg)
{
#pragma unused(_arg)
    for(;;) {
        int offset = POKE_NET_Control.RecvOffset;

		POKE_NET_LockMutex();
        if (POKE_NET_Control.RecvFlag == FALSE) {
	        POKE_NET_Control.RecvResult = SOC_Recv(POKE_NET_Control.Socket ,(char *)&POKE_NET_Control.RecvBuffer[offset] ,POKE_NET_Control.RecvSize-offset ,0 );
            POKE_NET_Control.RecvFlag = TRUE;
        }
		POKE_NET_UnlockMutex();
        POKE_NET_Sleep(1);
    }
	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recved thread terminate\n");
}
#endif

//====================================
//          ��M��
//====================================
BOOL POKE_NET_Receiving()
{
	int ret;
	OSTick starttime;
	OSTick timetmp;
	long timtmps;
	long offset;
	long size;

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv from %s socket:%08X\n" ,POKE_NET_Control.URL ,POKE_NET_Control.Socket);
	offset = 0;
	size = -1;
	starttime = OS_GetTick();
    
#ifdef RECEIVE_THREAD
    // ��M�p�X���b�h�쐬
    {
       	// �X���b�h�쐬
	    OS_CreateThread(&POKE_NET_Control.RecvThread ,
		    POKE_NET_ThreadRecv ,
		    NULL ,
		    POKE_NET_DS_ThreadStackRecv+POKE_NET_DS_THREADSTACKSIZE ,
		    POKE_NET_DS_THREADSTACKSIZE ,
		    POKE_NET_Control.ThreadLevel
	    );
        POKE_NET_Control.RecvResult = SOC_EWOULDBLOCK;
        POKE_NET_Control.RecvOffset = 0;
        POKE_NET_Control.RecvFlag = FALSE;

	    POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET create recv thread\n");
	    OS_WakeupThreadDirect(&POKE_NET_Control.RecvThread);
    }
#endif

	for(;;){
		// ��M
#ifndef RECEIVE_THREAD
		ret = SOC_Recv(POKE_NET_Control.Socket ,(char *)&POKE_NET_Control.RecvBuffer[offset] ,POKE_NET_Control.RecvSize-offset ,0 );
#else
   		if( POKE_NET_Control.RecvTimeOut != 0 ){
			// �^�C���A�E�g�ݒ肠��
			timetmp = OS_GetTick();
			timtmps = (long)(timetmp/(OS_SYSTEM_CLOCK / 64) - starttime/(OS_SYSTEM_CLOCK / 64));
			if( timtmps >= POKE_NET_Control.RecvTimeOut ){
				// �^�C���A�E�g
				POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv timeout socket:%08X\n" ,POKE_NET_Control.Socket);
				POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVTIMEOUT;
#ifdef RECEIVE_THREAD
                OS_DestroyThread(&POKE_NET_Control.RecvThread);
#endif
				return(FALSE);
			}
		}

        if (POKE_NET_Control.RecvFlag == FALSE) {
            continue;//goto TIMEOUT;
        }

        ret = POKE_NET_Control.RecvResult;
        //POKE_NET_Control.RecvResult = SOC_EWOULDBLOCK;
#endif
		if( ret == SOC_EWOULDBLOCK ){
			// ��u���b�L���O
		}else if( ret <= 0 ){
			// �G���[�F��M�G���[
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv error socket:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_RECVRESPONSE;
#ifdef RECEIVE_THREAD
            OS_DestroyThread(&POKE_NET_Control.RecvThread);
#endif
			return(FALSE);
		}else{
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
				//	(long)(offset - sizeof(long)) ,
				//	(unsigned char *)&POKE_NET_Control.RecvBuffer[sizeof(long)]);

				// �T�C�Y�Z�b�g
				POKE_NET_Control.RecvSize = (long)(size - sizeof(long));
    			break;
			}
#ifdef RECEIVE_THREAD
            POKE_NET_Control.RecvOffset = offset;
#endif
		}
		POKE_NET_LockMutex();
		if( POKE_NET_Control.AbortFlag == TRUE ){
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recv abort socket:%08X\n" ,POKE_NET_Control.Socket);
			POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_ABORTED;
			POKE_NET_UnlockMutex();
#ifdef RECEIVE_THREAD
            OS_DestroyThread(&POKE_NET_Control.RecvThread);
#endif
			return(FALSE);
		}
		POKE_NET_UnlockMutex();

#ifdef RECEIVE_THREAD
        POKE_NET_Control.RecvFlag = FALSE;
#endif

		POKE_NET_Sleep(1);
	}

#ifdef RECEIVE_THREAD
    OS_DestroyThread(&POKE_NET_Control.RecvThread);
#endif

	POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#POKE_NET recved socket:%08X size:%d\n" ,POKE_NET_Control.Socket ,POKE_NET_Control.RecvSize);
	return(TRUE);
}

//====================================
//          ��M��
//====================================
void POKE_NET_Sleep(long _ms)
{
	OS_Sleep((unsigned long)_ms);
}

//=====================================
//		SVL�擾���C�����[�`��
//=====================================
BOOL POKE_NET_GetSvl()
{
	DWCError		dwcerror;
	DWCErrorType	dwcerrortype;
	int				errorcode;

	DWCSvlResult	result;
	DWCSvlState		state;

    if( POKE_NET_Control.SvlToken == TRUE )return(TRUE);

	// SVL�擾�J�n
    if(DWC_SVLGetTokenAsync("", &result) == FALSE){
        POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"#DWC_SVLGetTokenAsync failed" );
        POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_GETSVL;
        return(FALSE);
    }

	while(1) {
        state = DWC_SVLProcess();
		if(state == DWC_SVL_STATE_SUCCESS) {
            POKE_NET_REQUEST *reqhead;

            // ����
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"Successed to get SVL Status\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"status = %s\n", result.status==TRUE ? "TRUE" : "FALSE");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"svlhost = %s\n", result.svlhost);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"svltoken = %s\n", result.svltoken);
            memcpy( POKE_NET_Control.Auth.SvlToken, result.svltoken, DWC_SVL_TOKEN_LENGTH+1 );
	        reqhead = (POKE_NET_REQUEST *)&POKE_NET_Control.SendBuffer[sizeof(long)];
	        reqhead->Auth = POKE_NET_Control.Auth;
            POKE_NET_Control.SvlToken = TRUE;
            return(TRUE);
		}
		else if(state == DWC_SVL_STATE_ERROR) {
			dwcerror = DWC_GetLastErrorEx(&errorcode, &dwcerrortype);
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"Failed to get SVL Token\n");
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"DWCError = %d, errorcode = %d, DWCErrorType = %d\n", dwcerror, errorcode, dwcerrortype);
            POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_GETSVL;
            return(FALSE);
		}
		else if(state == DWC_SVL_STATE_CANCELED) {
			POKE_NET_DebugPrintf(POKE_NET_DEBUGLEVEL_LEVEL1 ,"SVL canceled.\n");
            POKE_NET_Control.LastErrorCode = POKE_NET_LASTERROR_GETSVL;
            return(FALSE);
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
    SslInit();
}


#endif	// ___POKE_NET_BUILD_DS___
