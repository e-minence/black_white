//=============================================================================
/**
 * @file    nhttp_rap.c
 * @brief   nhttp�𗘗p���āAGameSync�ɕK�v��
 * @author  ohno_katsumi@gamefreak.co.jp
 * @date    09/04/30
 */
//=============================================================================

#include <dwc.h>
#include "net/nhttp_rap.h"
//#include "net_app/gsync.h"
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


//#include <nitroWiFi/soc.h>
extern SOCCaInfo DWCca_nintendo_class2;
static SOCCaInfo* nintendoCA[] = {
    &DWCca_nintendo_class2,
};


#define _EVILSERVER_PROTO (0)  //�g�[�N���������o���ĂȂ����ɂP


//static G_SYNC_WORK* _pWork;

//typedef void (StateFunc)(G_SYNC_WORK* pState);

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

#ifdef DEBUG_SERVER

const static char ACCOUNT_INFOURL[] = "https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=account.playstatus&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char POKEMONLISTURL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=sleepily.bitlist&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char DOWNLOAD_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=savedata.download&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char DOWNLOADEND_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=savedata.download.finish&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //POST
const static char UPLOAD_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=savedata.upload&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //POST
const static char ACCOUNT_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=account.createdata&tok=%s\0"; //POST
const static char BTL_DL_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=worldbattle.download&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0"; //GET
const static char BTL_UP_URL[] ="https://pokemon-ds.basementfactorysystems.com/dsio/gw?p=worldbattle.upload&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0"; //POST

//const static char POKECHK_URL[] ="http://125.206.241.227/pokemon/validate"; //POST
const static char POKECHK_URL[] ="https://pkvldttest.nintendo.co.jp/pokemon/validate"; //POST

#else


const static char ACCOUNT_INFOURL[] = "https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=account.playstatus&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char POKEMONLISTURL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=sleepily.bitlist&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char DOWNLOAD_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=savedata.download&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //GET
const static char DOWNLOADEND_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=savedata.download.finish&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //POST
const static char UPLOAD_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=savedata.upload&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0";  //POST
const static char ACCOUNT_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=account.createdata&tok=%s\0"; //POST
const static char BTL_DL_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=worldbattle.download&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0"; //GET
const static char BTL_UP_URL[] ="https://pokemon-ds-pub.basementfactorysystems.com/dsio/gw?p=worldbattle.upload&gsid=%d&rom=%d&langcode=%d&dreamw=%d&tok=%s\0"; //POST

//@todo �܂��{�Ԃ̃A�h���X�񋟂�����
const static char POKECHK_URL[] ="https://pkvldttest.nintendo.co.jp/pokemon/validate"; //POST

#endif

//const static char ACCOUNT_URL[] ="http://wbext.gamefreak.co.jp:10610/cgi-bin/cgeartest/gsyncget.cgi?p=account.createdata\0"; //POST




  //rom, langcode, dreamw 

typedef struct{
  const char* url;
  int type;
} NHTTPRAP_URLTBL;

static NHTTPRAP_URLTBL urltable[]={
  {ACCOUNT_INFOURL, NHTTP_REQMETHOD_GET},
  {POKEMONLISTURL, NHTTP_REQMETHOD_GET},
  {DOWNLOAD_URL, NHTTP_REQMETHOD_GET},
  {UPLOAD_URL, NHTTP_REQMETHOD_POST},
  {ACCOUNT_URL, NHTTP_REQMETHOD_POST},
  {BTL_DL_URL, NHTTP_REQMETHOD_GET},
  {BTL_UP_URL, NHTTP_REQMETHOD_POST},
  {POKECHK_URL, NHTTP_REQMETHOD_POST},
  {DOWNLOADEND_URL, NHTTP_REQMETHOD_POST},
};


extern CPSCaInfo PDW_CA;

static CPSCaInfo* cainfos[] = {
  &PDW_CA,
};

#define _GET_MAXSIZE  (1024)
#define _URL_BUFFER   (400)


struct _NHTTP_RAP_WORK {
  u8* pData;
  int length;
  DWCSvlResult* pSvl;
  NHTTPConnectionHandle   handle;
  char getbuffer[_GET_MAXSIZE];
  char urlbuff[_URL_BUFFER];
  HEAPID heapID;
  u32 profileid;
};

static NHTTP_RAP_WORK* pNHTTPWork;  //���̕ϐ��̓G���[�̍ۂɎg�p�����


static void* AllocForNhttp(u32 size, int alignment)
{
  return DWC_RAPCOMMON_Alloc(DWC_ALLOCTYPE_NHTTP,size,alignment);
}

static void FreeForNhttp(void* ptr)
{
  DWC_RAPCOMMON_Free(DWC_ALLOCTYPE_NHTTP,ptr,0);
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

  if(0!=NHTTPStartup(AllocForNhttp, FreeForNhttp, 12)){
    GF_ASSERT(0);
    return FALSE;
  }
  if(pWork==NULL){
    return FALSE;
  }

  GFL_STD_MemClear(pWork->urlbuff,sizeof(_URL_BUFFER));

  if(urlno==NHTTPRAP_URL_ACCOUNT_CREATE){
    STD_TSNPrintf(pWork->urlbuff, _URL_BUFFER, urltable[urlno].url, pWork->pSvl->svltoken);
  }
  else{
    STD_TSNPrintf(pWork->urlbuff, _URL_BUFFER, urltable[urlno].url, pWork->profileid, PM_VERSION, PM_LANG, NET_DREAMWORLD_VER, pWork->pSvl->svltoken);
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
  if(0!=NHTTP_AddHeaderField(handle, "User-Agent", "Nintendo-DS"  )){
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
  if(pWork){
    return pWork->handle;
  }
  return NULL;
}


BOOL NHTTP_RAP_StartConnect(NHTTP_RAP_WORK* pWork)
{
  if(pWork){
    if ( NHTTP_ERROR_NONE !=  NHTTPStartConnection(pWork->handle))
    {
      GF_ASSERT(0);
      return FALSE;
    }
    return TRUE;
  }
  return FALSE;
}
  


void NHTTP_RAP_ErrorClean(NHTTP_RAP_WORK* pWork)
{
  if(pWork){
    if(pWork->handle){
      NHTTPDeleteConnection(pWork->handle);
      pWork->handle=NULL;
      NHTTPCleanup();
      NET_PRINT("NHTTPCleanup\n");
    }
  }
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

  if(pWork==NULL){
    return NHTTP_ERROR_SYSTEM;
  }
  if(pWork->handle==NULL){
    return NHTTP_ERROR_NONE;
  }
  
  err = NHTTPGetConnectionError(pWork->handle);

  if( err == NHTTP_ERROR_BUSY )
  { 
    u32 receivedCurrent;
    u32 contentLength;
    if ( NHTTP_ERROR_NONE == NHTTPGetConnectionProgress(pWork->handle, &receivedCurrent, &contentLength))
    {
//      NET_PRINT(" %d recv  \n",100*contentLength/receivedCurrent);
    }
  }
  else
  { 
    NHTTPError  error;
    error = NHTTP_GetConnectionError( pWork->handle );

    NET_PRINT("err = %d  response %d status %d connect_err%d\n", err,NHTTP_GetResultCode( pWork->handle ),NHTTPGetConnectionStatus(pWork->handle), error);

    if( error == NHTTP_ERROR_NONE )
    {
      char *res;
      BOOL ret = NHTTP_GetHeaderAll( pWork->handle, &res );
      NET_PRINT( "%s\nok? %d\n", res, ret);
    }
    else{
//      int errorCode;
//      DWCErrorType ErrorType;
//      int ret;
  //    ret = DWC_GetLastErrorEx( &errorCode, &ErrorType );  //�A���Ă��Ȃ�����
      GFL_NET_StateSetWifiError( 
                error,
                DWC_ETYPE_SHOW_ERROR,
                error,             ERRORCODE_NHTTP );
      GFL_NET_StateSetError(error);
    }

    NHTTP_RAP_ErrorClean(pWork);
    return err;
  }
  return err;
}

//����%��Ԃ�
int NHTTP_RAP_ProcessPercent(NHTTP_RAP_WORK* pWork)
{
  u32 receivedCurrent;
  u32 contentLength;

  if(pWork==NULL){
    return 0;
  }

  if(pWork->handle){
    if ( NHTTP_ERROR_NONE == NHTTPGetConnectionProgress(pWork->handle, &receivedCurrent, &contentLength))
    {
      return ((100 * contentLength) / receivedCurrent);
    }
  }
  return 0;
}




void* NHTTP_RAP_GetRecvBuffer(NHTTP_RAP_WORK* pWork)
{
  if(pWork==NULL){
    return NULL;
  }
  return pWork->getbuffer;
}



NHTTP_RAP_WORK* NHTTP_RAP_Init(HEAPID heapID,u32 profileid, DWCSvlResult* pSvl)
{
  NHTTP_RAP_WORK* pWork = GFL_HEAP_AllocClearMemory( heapID, sizeof(NHTTP_RAP_WORK) );
  pNHTTPWork = pWork;
  pWork->profileid=profileid;
  pWork->pSvl = pSvl;
  return pWork;
}


void NHTTP_RAP_End(NHTTP_RAP_WORK* pWork)
{
  if(pWork){
    NHTTP_RAP_PokemonEvilCheckDelete(pWork);
    GFL_HEAP_FreeMemory(pWork);
  }
}


#if PM_DEBUG
void NHTTP_DEBUG_GPF_HEADER_PRINT(gs_response* prep)
{

  OS_TPrintf("�������ʃR�[�h       %d\n",prep->ret_cd);
  OS_TPrintf("�f�[�^���o�C�g�T�C�Y %d\n",prep->body_size);
  OS_TPrintf("�ڍ׃G���[�R�[�h     %d\n",prep->desc_cd);
  OS_TPrintf("%s\n",prep->desc_msg);
}
#endif



BOOL NHTTP_RAP_SvlGetTokenStart(NHTTP_RAP_WORK* pWork)
{
  //����Ȃ��Ȃ���
  return TRUE;
}


BOOL NHTTP_RAP_SvlGetTokenMain(NHTTP_RAP_WORK* pWork)
{
  //����Ȃ��Ȃ���
  return TRUE;
}



void NHTTP_RAP_PokemonEvilCheckReset(NHTTP_RAP_WORK* pWork,NHTTP_POKECHK_ENUM type)
{
  u16 typeu16 = type;

  if(pWork==NULL){
    return;
  }
  pWork->length = 0;
  while( pWork->pSvl->svltoken[pWork->length] != 0 )
  { 
    pWork->pData[pWork->length] = (u8)pWork->pSvl->svltoken[pWork->length];
    pWork->length++;
  }
  pWork->pData[pWork->length++] = 0;
  NET_PRINT("[%d] %s\n", pWork->length, pWork->pData);
 


  pWork->pData[pWork->length++] = (u8)(typeu16 >> 8);
  NET_PRINT("[%d] %d\n", 1, pWork->pData[pWork->length-1]);
  pWork->pData[pWork->length++] = (u8)(typeu16 & 0xFF);
  NET_PRINT("[%d] %d\n", 1, pWork->pData[pWork->length-1]);
}


void NHTTP_RAP_PokemonEvilCheckCreate(NHTTP_RAP_WORK* pWork, HEAPID heapID, int size, NHTTP_POKECHK_ENUM type)
{
  if(pWork==NULL){
    return;
  }
  pWork->pData = GFL_NET_Align32Alloc(heapID, size + DWC_SVL_TOKEN_LENGTH + 4);
  GFL_STD_MemClear( pWork->pData, size + DWC_SVL_TOKEN_LENGTH + 4 );

  NHTTP_RAP_PokemonEvilCheckReset(pWork, type);
}



void NHTTP_RAP_PokemonEvilCheckAdd(NHTTP_RAP_WORK* pWork, const void* pData, int size)
{
  if(pWork==NULL){
    return;
  }
  GFL_STD_MemCopy(pData, &pWork->pData[pWork->length], size);
  pWork->length += size;
  NET_PRINT("[%d] \n", size);
}



BOOL NHTTP_RAP_PokemonEvilCheckConectionCreate(NHTTP_RAP_WORK* pWork)
{
//POKECHK_URL
  int                     result;
  const NHTTPRAP_URL_ENUM urlno = NHTTPRAP_URL_POKECHK;
  NHTTPConnectionHandle   handle;
  NHTTPError              err;
  OSTick                  tickStart, tickPrevious;
  u32                     receivedCurrent = 0, receivedPrevious = 0;
  u32                     contentLength;
  u32                     averageSpeed = 0, currentSpeed = 0, maxSpeed = 0;

  if(pWork==NULL){
    return FALSE;
  }
  
  if(0!=NHTTPStartup(AllocForNhttp, FreeForNhttp, 12)){
    GF_ASSERT(0);
    return FALSE;
  }



  GFL_STD_MemClear(pWork->urlbuff,sizeof(_URL_BUFFER));
  STD_StrCpy(pWork->urlbuff, urltable[urlno].url );

  
  NET_PRINT(" Target URL: %s\n", pWork->urlbuff);
  NET_PRINT("type :%d\n", urltable[urlno].type );
  handle = NHTTPCreateConnection( urltable[urlno].url,
                                  urltable[urlno].type,
                                  pWork->getbuffer, _GET_MAXSIZE,
                                  ConnectionCallback, NULL);
  if(handle==NULL){
    NHTTPError  error = NHTTP_GetError();
    GF_ASSERT_MSG( handle, "Connection��NULL!, error=%d\n", error );
    return FALSE;
  }
  pWork->handle = handle;



  if ( 0 > ( err = (NHTTPError)NHTTP_SetRootCA( handle, (const char *)nintendoCA, sizeof(nintendoCA)/sizeof(CPSCaInfo*) )))
  {
    GF_ASSERT_MSG(0," NHTTP_SetRootCA(%d)\n",err);
    return FALSE;
  }

  NHTTP_AddPostDataRaw( handle, &(pWork->pData[0]), pWork->length );
  NET_PRINT("data svltoken = %s\n", pWork->pData);
  NET_PRINT("Post Byte size %d\n", pWork->length);
  {
    int i;
    for( i = 0; i < pWork->length; i++ )
    { 
      NET_PRINT( "%x ", pWork->pData[i] );
    }
      NET_PRINT( "\nend\n" );
  }


  
  return TRUE;
}


void NHTTP_RAP_PokemonEvilCheckDelete(NHTTP_RAP_WORK* pWork)
{
  if(pWork){
    if(pWork->pData){
      GFL_NET_Align32Free(pWork->pData);
    }
    pWork->pData = NULL;
    pWork->length = 0;
  }
  pNHTTPWork = NULL;
}



