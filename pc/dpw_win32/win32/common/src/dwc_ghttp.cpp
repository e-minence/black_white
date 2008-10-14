#include "dpw_typedef.h"
#include "dpw_pcstub.h"

// the original header
//---------------------------------------------------------
#include "dwc_ghttp.h"

// �v���g�^�C�v�錾
//---------------------------------------------------------
static GHTTPBool GHTTPCompletedCallback( GHTTPRequest   request,
										 GHTTPResult	result,
										 char *			buffer,
										 GHTTPByteCount	bufferLen,
										 void *			param);

static void GHTTPProgressCallback(GHTTPRequest   request, 
                                  GHTTPState     state, 
                                  const char *   buffer, 
                                  GHTTPByteCount bufferLen, 
                                  GHTTPByteCount bytesReceived, 
                                  GHTTPByteCount totalSize, 
                                  void *         param );

static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result);

//=============================================================================
/*!
 *	@brief	���C�u�����̏�����
 *
 *	@param	gamename	[in] �Q�[����
 *
 *	@retval	TRUE		����
 *	@retval FALSE   	���s
 */
//=============================================================================
BOOL	DWC_InitGHTTP( const char* gamename ){
#pragma unused(gamename)

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_InitGHTTP\n");

    //
	// GHTTP �̊J�n
	//
	ghttpStartup();

	return TRUE;

}

//=============================================================================
/*!
 *	@brief	���C�u�����̃V���b�g�_�E��
 *
 *	@retval	TRUE		����
 *	@retval	FALSE   	���s
 */
//=============================================================================
BOOL	DWC_ShutdownGHTTP( void ){

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_ShutdownGHTTP\n");

	//
	// GHTTP �̏I��
	//
	ghttpCleanup();

	return TRUE;

}


//=============================================================================
/*!
 *	@brief	�f�[�^�ʐM
 *
 *	@retval	TRUE		����
 *	@retval	FALSE   	���s
 */
//=============================================================================
BOOL	DWC_ProcessGHTTP( void ){

    if (DWCi_IsError()) return FALSE;

	//
	// GHTTP �̃��X�|���X�҂�
	//
	ghttpThink();

	return TRUE;

}

//=============================================================================
/*!
 *	@brief	HTTP���N�G�X�g�̃R�[���o�b�N
 *
 *	@retval	GHTTPTrue		�o�b�t�@�J��
 *	@retval	GHTTPFalse   	�o�b�t�@�J������
 */
//=============================================================================
static GHTTPBool GHTTPCompletedCallback( GHTTPRequest   request,
										 GHTTPResult	result,
										 char *			buffer,
										 GHTTPByteCount	bufferLen,
										 void *			param)
{
#pragma unused(request)

    DWCGHTTPParam* parameter = (DWCGHTTPParam*)param;

	// �R�[���o�b�N�֐��̃|�C���^���擾
	DWCGHTTPCompletedCallback callback = (DWCGHTTPCompletedCallback)parameter->completedCallback;

    BOOL buffer_clear = parameter->buffer_clear;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "GHTTPCompleteCallback result : %d\n", result);

    if(callback){
        if(result == GHTTPSuccess){
            callback( buffer, (int)bufferLen, (DWCGHTTPResult)result, parameter->param );
        }else{
            DWCi_HandleGHTTPError((DWCGHTTPResult)result);
            callback( NULL, 0, (DWCGHTTPResult)result, parameter->param );
        }
    }
    else{
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "Callback is NULL\n");
    }

	DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
    
    if(!buffer_clear){
        return GHTTPFalse;
    }

	return GHTTPTrue;
}

//=============================================================================
/*!
 *	@brief	HTTP���N�G�X�g���̃R�[���o�b�N
 */
//=============================================================================
static void GHTTPProgressCallback(GHTTPRequest   request, 
                                  GHTTPState     state, 
                                  const char *   buffer, 
                                  GHTTPByteCount bufferLen, 
                                  GHTTPByteCount bytesReceived, 
                                  GHTTPByteCount totalSize, 
                                  void *         param )
{
#pragma unused(request)

    DWCGHTTPParam* parameter = (DWCGHTTPParam*)param;

	// �R�[���o�b�N�֐��̃|�C���^���擾
	DWCGHTTPProgressCallback callback = (DWCGHTTPProgressCallback)parameter->progressCallback;

    if(callback){
        callback( (DWCGHTTPState)state, buffer, (int)bufferLen, (int)bytesReceived, (int)totalSize, parameter->param );
    }

}

//=============================================================================
/*!
 *	@brief	post����I�u�W�F�N�g�̍쐬
 *
 *	@param	post	  [in] �쐬�����I�u�W�F�N�g
 *
 */
//=============================================================================
void    DWC_GHTTPNewPost( DWCGHTTPPost* post ){

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GHTTPNewPost\n");
    
    *post = ghttpNewPost();

    if (post == NULL){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
    }
}

//=============================================================================
/*!
 *	@brief	Addpost�p
 *
 *	@param	post	  [in] Add�����I�u�W�F�N�g
 *	@param	key 	  [in] post����f�[�^��key��
 *	@param	buf 	  [in] post����f�[�^�̓��e
 *
 *	@retval	TRUE		����
 *	@retval	FALSE   	���s
 */
//=============================================================================
BOOL    DWC_GHTTPPostAddString( DWCGHTTPPost* post, const char* key, const char* value ){
    
    if(!ghttpPostAddString(*post, key, value)){
        DWC_Printf(DWC_REPORTFLAG_WARNING, "DWC_GHTTPPostAddString FALSE\n");
        return FALSE;
    }
    else {
        return TRUE;
    }
}

//=============================================================================
/*!
 *	@brief	�f�[�^�̑��M
 *
 *	@param	url		[in] ���X�g�擾��̂t�q�k
 *	@param	post	[in] post����I�u�W�F�N�g
 *	@param	proc	[in] ���X�|���X����������R�[���o�b�N
 *	@param	param	[in] �R�[���o�b�N�p�p�����[�^
 *
 *	@retval	req		 0�ȏ� ���N�G�X�g���ʎq�i�����j
 *                  -1�ȉ� ���N�G�X�g�G���[�l
 */
//=============================================================================
int 	DWC_PostGHTTPData( const char* url, GHTTPPost* post, DWCGHTTPCompletedCallback completedCallback, void* param){

   	GHTTPRequest req;
    DWCGHTTPParam* parameter = NULL;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_PostGHTTPData\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;
    
    parameter = (DWCGHTTPParam*)DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParam));
    
    if(!parameter){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, parameter->param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }
    
    parameter->param = param;
    parameter->completedCallback = completedCallback;
    parameter->progressCallback = NULL;
    parameter->buffer_clear = TRUE;

	req = ghttpPost( url, *post, GHTTPFalse, 
                      GHTTPCompletedCallback, (void *)parameter );
   
    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, parameter->param );
        DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
    }

	return (int)req;

}

//=============================================================================
/*!
 *	@brief	�f�[�^�̎�M
 *
 *	@param	url		[in] ���X�g�擾��̂t�q�k
 *	@param	proc	[in] ���X�|���X����������R�[���o�b�N
 *	@param	param	[in] �R�[���o�b�N�p�p�����[�^
 *
 *	@retval	req		 0�ȏ� ���N�G�X�g���ʎq�i�����j
 *                  -1�ȉ� ���N�G�X�g�G���[�l
 */
//=============================================================================
int 	DWC_GetGHTTPData( const char* url, DWCGHTTPCompletedCallback completedCallback, void* param ){
    
   	GHTTPRequest req;
    DWCGHTTPParam* parameter = NULL;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GetGHTTPData\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;
    
    parameter = (DWCGHTTPParam*)DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParam));
    
    if(!parameter){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }

    parameter->param = param;
    parameter->completedCallback = completedCallback;
    parameter->progressCallback  = NULL;
    parameter->buffer_clear = TRUE;

    req = ghttpGet( url, GHTTPFalse, GHTTPCompletedCallback, (void *)parameter );

    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, parameter->param );
        DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
    }
    
	return (int)req;

}

//=============================================================================
/*!
 *	@brief	�f�[�^�̎�M(�o�b�t�@�T�C�Y�w��AProgressCallback�w��)
 *
 *	@param	url		            [in] ���X�g�擾��̂t�q�k
 *	@param	bufferlen           [in] ��M�o�b�t�@�̃T�C�Y
 *	@param	progressCallback	[in] �������ɌĂ΂��R�[���o�b�N
 *  @param  completedCallback   [in] ���X�|���X����������R�[���o�b�N
 *	@param	param	            [in] �R�[���o�b�N�p�p�����[�^
 *
 *	@retval	req		 0�ȏ� ���N�G�X�g���ʎq�i�����j
 *                  -1�ȉ� ���N�G�X�g�G���[�l
 */
//=============================================================================
int 	DWC_GetGHTTPDataEx( const char* url, int bufferlen, BOOL buffer_clear, DWCGHTTPProgressCallback progressCallback, DWCGHTTPCompletedCallback completedCallback, void* param ){

    char* buffer = NULL;
   	GHTTPRequest req;
    DWCGHTTPParam* parameter = NULL;

    DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_GetGHTTPDataEx\n");

    if (DWCi_IsError()) return DWC_GHTTP_IN_ERROR;
    
    parameter = (DWCGHTTPParam*)DWC_Alloc(DWC_ALLOCTYPE_BASE, sizeof(DWCGHTTPParam));
    
    if(!parameter){
        DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
        DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
        completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
        return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
    }

    if(bufferlen > 0){
        buffer = (char*)DWC_Alloc(DWC_ALLOCTYPE_BASE, (u32)bufferlen);

        if(!buffer){
            DWCi_HandleGHTTPError((DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY);
            DWC_Printf(DWC_REPORTFLAG_DEBUG, "DWC_Alloc Error\n");
            completedCallback( NULL, 0, (DWCGHTTPResult)DWC_GHTTP_INSUFFICIENT_MEMORY, param );
            DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
            return (int)DWC_GHTTP_INSUFFICIENT_MEMORY;
        }
    }

    parameter->param = param;
    parameter->completedCallback = completedCallback;
    parameter->progressCallback  = progressCallback;
    parameter->buffer_clear = buffer_clear;

    if(parameter->progressCallback)
        req = ghttpGetEx( url, NULL, buffer, bufferlen, NULL, GHTTPFalse, GHTTPFalse,
                          GHTTPProgressCallback, GHTTPCompletedCallback, (void *)parameter );
    else
        req = ghttpGetEx( url, NULL, buffer, bufferlen, NULL, GHTTPFalse,
                          GHTTPFalse, NULL, GHTTPCompletedCallback, (void *)parameter );
        
    if(req < 0){
        DWCi_HandleGHTTPError((DWCGHTTPResult)req);
        completedCallback( NULL, 0, (DWCGHTTPResult)req, parameter->param );
        DWC_Free(DWC_ALLOCTYPE_BASE, (void*)parameter, 0);
        if(bufferlen > 0)
            DWC_Free(DWC_ALLOCTYPE_BASE, (void*)buffer, 0);
    }
    
	return (int)req;

}

//=============================================================================
/*!
 *	@brief	HTTP���N�G�X�g�̃L�����Z��
 *
 *	@param	req		[in] ��~����HTTP���N�G�X�g��ID(DWC_GetGHTTPDataEx�̕Ԃ�l)
 *
 *	@retval	�Ȃ�
 */
//=============================================================================
void DWC_CancelGHTTPRequest(int reqid)
{
	GHTTPRequest req;
	
	req = (GHTTPRequest)reqid;
	ghttpCancelRequest(req); 
}

//=============================================================================
/*!
 *	@brief	�ʐM��Ԃ̊m�F
 *
 *	@param	req		[in] ���N�G�X�g���ʎq
 *
 *	@retval	state	�ʐM���
 *  @retval False   ���s
 */
//=============================================================================
DWCGHTTPState DWC_GetGHTTPState( int req )
{
    if(req < 0){
        return DWCGHTTPFalse;
    }
    else{
        return (DWCGHTTPState)(ghttpGetState(req));
    }
}

//=============================================================================
/*!
 *	@brief	GHTTP�Ή��G���[�����֐�
 *
 *	@param	result	[in] GHTTP�̏�������
 *
 *	@retval	state	GHTTP�̏������ʌ^�i���������̂܂ܕԂ��j
 */
//=============================================================================
static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result)
{
    int errorCode = DWC_ECODE_SEQ_ETC + DWC_ECODE_GS_HTTP;
    DWCError dwcError = DWC_ERROR_NETWORK;

    if (result == DWC_GHTTP_SUCCESS) return DWC_GHTTP_SUCCESS;

    DWC_Printf(DWC_REPORTFLAG_ERROR, "Main, DWCGHTTP error %d\n", result);

    // Possible Error values returned from GHTTP functions.
    switch (result){
	case DWC_GHTTP_FAILED_TO_OPEN_FILE:
        errorCode += DWC_ECODE_TYPE_OPEN_FILE;
        break;
	case DWC_GHTTP_INVALID_POST:
        errorCode += DWC_ECODE_TYPE_INVALID_POST;
        break;
	case DWC_GHTTP_INSUFFICIENT_MEMORY:
        errorCode += DWC_ECODE_TYPE_BUFF_OVER;
        break;
	case DWC_GHTTP_INVALID_FILE_NAME:
	case DWC_GHTTP_INVALID_BUFFER_SIZE:
	case DWC_GHTTP_INVALID_URL:
        errorCode += DWC_ECODE_TYPE_REQ_INVALID;
        break;
	case DWC_GHTTP_UNSPECIFIED_ERROR:
        errorCode += DWC_ECODE_TYPE_UNSPECIFIED;
        break;
        
    // The result of an HTTP request.
	case DWC_GHTTP_OUT_OF_MEMORY:
    case DWC_GHTTP_MEMORY_ERROR:
        dwcError = DWC_ERROR_FATAL;
        errorCode += DWC_ECODE_TYPE_ALLOC;
        break;
	case DWC_GHTTP_BUFFER_OVERFLOW:
        errorCode += DWC_ECODE_TYPE_BUFF_OVER;
        break;
	case DWC_GHTTP_PARSE_URL_FAILED:
        errorCode += DWC_ECODE_TYPE_PARSE_URL;
        break;
	case DWC_GHTTP_HOST_LOOKUP_FAILED:
        errorCode += DWC_ECODE_TYPE_DNS;
        break;
    case DWC_GHTTP_SOCKET_FAILED:
        errorCode += DWC_ECODE_TYPE_SOCKET;
        break;
	case DWC_GHTTP_CONNECT_FAILED:
	case DWC_GHTTP_FILE_NOT_FOUND:
	case DWC_GHTTP_SERVER_ERROR:
        errorCode += DWC_ECODE_TYPE_SERVER;
        break;
	case DWC_GHTTP_BAD_RESPONSE:
        errorCode += DWC_ECODE_TYPE_BAD_RESPONSE;
        break;
	case DWC_GHTTP_REQUEST_REJECTED:
	case DWC_GHTTP_UNAUTHORIZED:
	case DWC_GHTTP_FORBIDDEN:
        errorCode += DWC_ECODE_TYPE_REJECTED;
        break;
	case DWC_GHTTP_FILE_WRITE_FAILED:
	case DWC_GHTTP_FILE_READ_FAILED:
        errorCode += DWC_ECODE_TYPE_FILE_RW;
        break;
	case DWC_GHTTP_FILE_INCOMPLETE:
        errorCode += DWC_ECODE_TYPE_INCOMPLETE;
        break;
	case DWC_GHTTP_FILE_TOO_BIG:
        errorCode += DWC_ECODE_TYPE_TO_BIG;
        break;
	case DWC_GHTTP_ENCRYPTION_ERROR:
        errorCode += DWC_ECODE_TYPE_ENCRYPTION;
        break;

    default:
        break;
    }

    DWCi_SetError(dwcError, errorCode);
    return result;
}
