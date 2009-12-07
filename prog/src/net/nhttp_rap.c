//=============================================================================
/**
 * @file    nhttp_rap.c
 * @brief   nhttp�𗘗p���āAGameSync�ɕK�v��
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date    09/04/30
 */
//=============================================================================

#include "net/nhttp_rap.h"
#include "net_app/gsync.h"
#include "net/dwc_rapcommon.h"

#include "system/main.h"  //HEAPID
#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "net/network_define.h"
#include "savedata/wifilist.h"
#include "msg/msg_d_ohno.h"



static G_SYNC_WORK* _pWork;

typedef void (StateFunc)(G_SYNC_WORK* pState);

/*
#define POSTURL "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsync.cgi"
#define GETURL1 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data1&mac=1"
#define GETURL2 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data2&mac=1"
#define GETURL3 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data3&mac=1"
#define GETURL4 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data4&mac=1"
#define GETURL5 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data5&mac=1"
#define GETURL6 "https://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?name=data6&mac=1"
 */
//#define POSTURL "https://pokemon-ds.basementfactorysystems.com/bindata-test/data2.php"
//#define GETURL1 "https://pokemon-ds.basementfactorysystems.com/bindata-test/data1.php"


const static char ACCOUNT_INFOURL[] = "https://pokemon-ds.basementfactorysystems.com/bindata-test/data1.php";  //GET
const static char  POKEMONLISTURL[] ="https://pokemon-ds.basementfactorysystems.com/bindata-test/data1.php";  //GET
const static char  DOWNLOAD_URL[] ="https://pokemon-ds.basementfactorysystems.com/bindata-test/data1.php";  //GET
const static char  UPLOAD_URL[] ="https://pokemon-ds.basementfactorysystems.com/gs?p=savedata.upload&gsid=";  //POST

typedef struct{
  const char* url;
  int type;
} NHTTPRAP_URLTBL;

static NHTTPRAP_URLTBL urltable[]={
  {ACCOUNT_INFOURL, NHTTP_REQMETHOD_GET},
  {POKEMONLISTURL, NHTTP_REQMETHOD_GET},
  {DOWNLOAD_URL, NHTTP_REQMETHOD_GET},
  {UPLOAD_URL, NHTTP_REQMETHOD_POST},
};


extern CPSCaInfo PDW_CA;

static CPSCaInfo* cainfos[] = {
  &PDW_CA,
};

#define _GET_MAXSIZE  (1024)
#define _URL_BUFFER   (200)


struct _NHTTP_RAP_WORK {
  NHTTPConnectionHandle   handle;
  char getbuffer[_GET_MAXSIZE];
  char urlbuff[_URL_BUFFER];

  HEAPID heapID;
  u32 profileid;
};



static void* AllocForNhttp(u32 size, int alignment)
{
  return DWC_RAPCOMMON_Alloc(0,size,alignment);
}

static void FreeForNhttp(void* ptr)
{
  DWC_RAPCOMMON_Free(0,ptr,0);
}



static int ConnectionCallback( NHTTPConnectionHandle handle, NHTTPConnectionEvent event, void* arg )
{
  (void)handle;
  switch ( event )
  {
  case NHTTP_EVENT_BODY_RECV_FULL:
  case NHTTP_EVENT_BODY_RECV_DONE:
    {
      NHTTPBodyBufArg*    bbArg = (NHTTPBodyBufArg*)arg;
      bbArg->offset = 0;
    }
  case NHTTP_EVENT_POST_SEND:
    {
      NHTTPPostSendArg*   psArg = (NHTTPPostSendArg*)arg;
    }
  }
  return NHTTP_ERROR_NONE;
}



BOOL NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_ENUM urlno,NHTTP_RAP_WORK* pWork)
{
  int     result;
  NHTTPConnectionHandle   handle;
  NHTTPError              err;
  OSTick                  tickStart, tickPrevious;
  u32                     receivedCurrent = 0, receivedPrevious = 0;
  u32                     contentLength;
  u32                     averageSpeed = 0, currentSpeed = 0, maxSpeed = 0;
  char pidbuff[20];

  if(0!=NHTTPStartup(AllocForNhttp, FreeForNhttp, 12)){
    GF_ASSERT(0);
    return FALSE;
  }



  GFL_STD_MemClear(pWork->urlbuff,sizeof(_URL_BUFFER));
  GFL_STD_MemCopy(urltable[urlno].url, pWork->urlbuff, GFL_STD_StrLen(urltable[urlno].url));

  if(urlno==NHTTPRAP_URL_UPLOAD){
    STD_TSNPrintf(pidbuff,20,"%d", pWork->profileid);
    GFL_STD_StrCat(pWork->urlbuff,pidbuff,sizeof(_URL_BUFFER));
  }
  NET_PRINT(" Target URL: %s\n", pWork->urlbuff);
  handle = NHTTPCreateConnection( pWork->urlbuff,
                                  urltable[urlno].type,
                                  pWork->getbuffer, _GET_MAXSIZE,
                                  ConnectionCallback, pWork);
  GF_ASSERT(handle);
  if(handle==NULL){
    return FALSE;
  }
  pWork->handle = handle;

  if ( 0 > ( err = (NHTTPError)NHTTP_SetRootCA( handle, (const char *)cainfos, sizeof(cainfos)/sizeof(CPSCaInfo*) )))
  {
    GF_ASSERT_MSG(0," NHTTP_SetRootCA(%d)\n",err);
    return FALSE;
  }
  if(0!=NHTTP_AddHeaderField(handle, "Accept", "*/*" )){
    GF_ASSERT(0);
    return FALSE;
  }
  if(0!=NHTTP_AddHeaderField(handle, "User-Agent", "Test"  )){
    GF_ASSERT(0);
    return FALSE;
  }

  if(0!=NHTTP_SetBasicAuthorization( handle, "pokemon", "2Phfv9MY")){
    GF_ASSERT(0);
    return FALSE;
  }
  return TRUE;
}


NHTTPConnectionHandle NHTTP_RAP_GetHandle(NHTTP_RAP_WORK* pWork)
{
  return pWork->handle;
}


BOOL NHTTP_RAP_StartConnect(NHTTP_RAP_WORK* pWork)
{
  if ( 0 !=  NHTTPStartConnection(pWork->handle))
  {
    GF_ASSERT(0);
    return FALSE;
  }

  return TRUE;
}


/*
NHTTP_ERROR_SYSTEM  	-1  	�n���h���s���ɂ��G���[�B
�����œn���Ă���R�l�N�V�����n���h�����s���̏ꍇ�ɕԂ�܂��B
NHTTP_ERROR_NONE 	0 	�����B
NHTTP_ERROR_ALLOC 	1 	�A���[�P�[�V�����Ɏ��s���Ă��܂��B
NHTTP_Startup�Őݒ肵���������m�ۊ֐��Ɖ���֐����m�F���Ă��������B
NHTTP_ERROR_TOOMANYREQ 	2 	���N�G�X�g�������ς��ł��B���̃G���[���������邱�Ƃ͂���܂���B
NHTTP_ERROR_SOCKET 	3 	�\�P�b�g�G���[�B
�\�P�b�g�̍쐬�Ɏ��s�����ꍇ�A�܂��̓X�g���[�~���O���M��POST�f�[�^���M�̃T�C�Y�������ɐݒ肵���R�[���o�b�N�����̒l��Ԃ����ꍇ�ɕԂ�܂��B
NHTTP_ERROR_DNS 	4 	DNS�����Ɏ��s���Ă��܂��B
���N�G�X�gURL���Ԉ���Ă���ꍇ��A���݂��Ȃ��ꍇ�ɕԂ�܂��B
���N�G�X�gURL���m�F���Ă��������B
NHTTP_ERROR_CONNECT 	5 	�ڑ������s���Ă��܂��B
DNS������A���N�G�X�gURL�̃T�[�o�[�Ƃ̐ڑ��O�ɒʐM���Ւf���ꂽ�ꍇ��A
���N�G�X�gURL�Ŏw�肵���ڑ��|�[�g�ԍ����Ԉ���Ă���ꍇ�ɕԂ�܂��B
NHTTP_ERROR_BUFFULL 	6 	��M�o�b�t�@����t�ł��B
Body�̃T�C�Y��NHTTP_CreateConnection�Őݒ肵���o�b�t�@�����傫���ꍇ�ɕԂ�܂��B
NHTTP_EVENT_BODY_RECV_FULL�C�x���g�R�[���o�b�N�������M�o�b�t�@���Đݒ肷�邱�Ƃŉ����\�ł��B
NHTTP_ERROR_HTTPPARSE 	7 	HTTP�w�b�_�̉�͂Ɏ��s���Ă��܂��B
�T�[�o�[�����M����HTTP�w�b�_�[���ُ�ȏꍇ�ɕԂ�܂��B
NHTTP_ERROR_CANCELED 	8 	���N�G�X�g���L�����Z�����Ă��܂��B
NHTTP_CancelConnection�Ŏ��s���̃��N�G�X�g�ɃL�����Z����v�������ꍇ�ɕԂ�܂��B
NHTTP_ERROR_SDK 	9 	NHTTP_�X���b�h�̐����Ɏ��s���Ă��܂��B
NHTTP_Startup���s���A�X���b�h�����Ɏ��s�����ꍇ�ɕԂ�܂��B
NHTTP_ERROR_NETWORK 	10 	���M�A��M�A�\�P�b�g�I�����ɖ�肪����ꍇ�ɔ�������G���[�B
���M�A��M�A�I�����ɒʐM���Ւf���ꂽ�ꍇ�ɕԂ�܂��B
NHTTP_ERROR_UNKNOWN 	11 	NHTTP_CreateConnection�Ń��\�b�h�Ɉُ�Ȓl��ݒ肵�Ă���ꍇ�B
�܂��͌��ݎ��s���̃��N�G�X�g�ɑ΂��āA
NHTTP_StartConnection�ōēx���N�G�X�g�J�n��v�������ꍇ�ɔ�������G���[�B
NHTTP_ERROR_DNS_PROXY 	12 	proxy�T�[�o�[��DNS�����Ɏ��s���Ă��܂��B
�{�̐ݒ��proxy�T�[�o�[���Ԉ���Ă���ꍇ��A���݂��Ȃ��ꍇ�ɕԂ�܂��B
�{�̐ݒ��proxy�T�[�o�[���m�F���Ă��������B
NHTTP_ERROR_CONNECT_PROXY 	13 	proxy�T�[�o�[�Ƃ̐ڑ��Ɏ��s���Ă��܂��B
DNS������Aproxy�T�[�o�[�Ɛڑ��O�ɒʐM���Ւf���ꂽ�ꍇ��A
�{�̐ݒ��proxy�T�[�o�[�̃|�[�g�ԍ����ԈႦ�Ă���ꍇ�ɕԂ�܂��B
NHTTP_ERROR_SSL 	14 	SSL�ʐM�Ɏ��s���Ă��܂��B
SSL�ʐM�ŃT�[�o�[�Ƃ̐ڑ������s�����ꍇ�ɕԂ�܂��B
SSL�ʐM�G���[�̏ڍׂ�NHTTP_GetConnectionSSLError()�Ŏ擾�\�ł��B
NHTTP_ERROR_BUSY 	15 	���N�G�X�g�����ݎ��s���ł��邱�Ƃ�\���܂��B
NHTTP_ERROR_SSL_ROOTCA 	16 	���[�gCA�̐ݒ�Ɏ��s���Ă��܂��B
NHTTP_ERROR_SSL_CLIENTCERT 	17 	�N���C�A���g�ؖ����̐ݒ�Ɏ��s���Ă��܂��B


 */
NHTTPError NHTTP_RAP_Process(NHTTP_RAP_WORK* pWork)
{
  int     result;
  NHTTPError              err;
  u32                     receivedCurrent = 0;
  u32                     contentLength;

  if(pWork->handle==NULL){
    return NHTTP_ERROR_NONE;
  }
  
  err = NHTTPGetConnectionError(pWork->handle);
  switch(err){
  case NHTTP_ERROR_BUSY:
    break;
  case NHTTP_ERROR_NONE:  //���łɊ���
  default:
    OS_TPrintf("err = %d\n", err);
    NHTTPDeleteConnection(pWork->handle);
    pWork->handle=NULL;
    NHTTPCleanup();
    return err;
  }
  
  {
    NHTTPConnectionStatus   status = NHTTPGetConnectionStatus(pWork->handle);
    OSTick tickNow;

    if ( NHTTP_ERROR_NONE == NHTTPGetConnectionProgress(pWork->handle, &receivedCurrent, &contentLength))
    {

      NET_PRINT(" %d recv  \n",100*contentLength/receivedCurrent);

    }
  }
  return err;
}



void* NHTTP_RAP_GetRecvBuffer(NHTTP_RAP_WORK* pWork)
{
  return pWork->getbuffer;
}



NHTTP_RAP_WORK* NHTTP_RAP_Init(HEAPID heapID,u32 profileid)
{
  NHTTP_RAP_WORK* pWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(NHTTP_RAP_WORK) );
  pWork->profileid=profileid;
  return pWork;
}


void NHTTP_RAP_End(NHTTP_RAP_WORK* pWork)
{
  GFL_HEAP_FreeMemory(pWork);
}




