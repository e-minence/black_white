//============================================================================================
/**
 * @file	mcs_resident.c
 * @brief	MCS�F�풓�Ď�
 * @author	
 * @date	
 */
//============================================================================================
#ifdef PM_DEBUG

#include "gflib.h"
#include "system/main.h"
#include "arc_def.h"
#include <isdbglib.h>

#include "sound/pm_sndsys.h"
#include "debug/gf_mcs.h"

#define GFL_MCS_RESIDENT_ID	(9876543)

enum {
	MCSRSDCOMM_REQHEAPSTATUS = 1,
	MCSRSDCOMM_HEAPSTATUS,
};

typedef struct {
	u32 comm;
	u32 param;
}MCSRSDCOMM_HEADER;

#define MCSRSDCOMM_HEADER_SIZE (sizeof(MCSRSDCOMM_HEADER) - sizeof(u32))

void	GFL_MCS_Resident(void);
static u32	GetExistMemoryBlocksInfo( HEAPID heapID, u32 pBuf );

#define SENDBUF_SIZE (0x10000)
#define RECVBUF_SIZE (0x10000)
u8 MCSRSD_sendBuffer[SENDBUF_SIZE];
static u8 MCSRSD_recvBuffer[RECVBUF_SIZE];
//============================================================================================
/**
 *
 * @brief	�V�X�e���N�� & �I��
 *
 */
//============================================================================================
void	GFL_MCS_Resident(void)
{
	if(GFL_MCS_Read(GFL_MCS_RESIDENT_ID, MCSRSD_recvBuffer, RECVBUF_SIZE) == TRUE){
		MCSRSDCOMM_HEADER* recv = (MCSRSDCOMM_HEADER*)MCSRSD_recvBuffer;
		switch(recv->comm){
		case MCSRSDCOMM_REQHEAPSTATUS:
			{
				MCSRSDCOMM_HEADER* commHeader = (MCSRSDCOMM_HEADER*)MCSRSD_sendBuffer;
				u32 sendSize;
				int i;

				commHeader->comm = MCSRSDCOMM_HEAPSTATUS;

				// �q�[�v���쐬���]��
				for(i=0; i<HEAPID_CHILD_MAX-1; i++){
					ISDPrintSetBlockingMode(1);
					sendSize = GetExistMemoryBlocksInfo( i, (u32)&commHeader->param);
					if(sendSize){
						sendSize += MCSRSDCOMM_HEADER_SIZE;	// �w�b�_��񕪉��Z
						OS_Printf("trans heapInfo ... size %x, heapID %d\n", sendSize, i);

						GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);
						ISDPrintSetBlockingMode(0);
					}
				}
			}
			break;
		}
	}
}

//============================================================================================
/**
 *
 * @brief	�q�[�v���擾
 *		GFL�ɂ���֐��i�f�o�b�O�E�C���h�E�ɏo�͂���֐��j�A�\����
 *		�����̂܂܃R�s�[���Ă���̂Ő������ɏ�ɒ��ӂ��邱�ƁB
 *		MCS�p�̃v���O������GFL�ɓ�������̂ɂ͒�R�����邽��
 *		�����Ă��̕��@�Ŏ�������
 *
 */
//============================================================================================
//----------------------------------------------------------------
/**
 *  ���[�U�[�������u���b�N�w�b�_��`�i�T�C�Y:MEMHEADER_USERINFO_SIZE = 26 �j
 */
//----------------------------------------------------------------
#define FILENAME_LEN  (18)
#define HEAPNAME_LEN  (32)
typedef struct {
  char  filename[ FILENAME_LEN ];		// �Ăяo����\�[�X��
  u16   lineNum;
}DEBUG_MEMHEADER;

typedef struct {
	u32		adrs;												// �m�ۃA�h���X
	u32		size;												// �m�ۃT�C�Y
  u16   lineNo;											// �Ăяo����\�[�X���C��
  char  filename[ FILENAME_LEN+1 ];	// �Ăяo����\�[�X��
	u8		pad[3];
}MCSDATA_HEAPBLOCKINFO;

typedef struct {
  u32		heapSize;										// �q�[�v�S�̃T�C�Y
	u32		totalFreeSize;							// �c��̃q�[�v�T�C�Y
	u32		allocatableSize;						// ��x�Ɋm�ۂł���ő�T�C�Y
	u16		heapID;											// �q�[�vID
	u16		numMemoryBlocks;						// ���݂̃u���b�N�m�ې�
	u16		numMemoryTotal;							// �f�[�^�����i�u���b�N���{�q�q�[�v���j
	u8		pad[2];
  char  name[ HEAPNAME_LEN ];				// �q�[�v��
	MCSDATA_HEAPBLOCKINFO	blockInfo;	// �e�u���b�N���i�ȉ��u���b�N�����j
}MCSDATA_HEAPSTATUS;

static int numMemoryTotal;

//�q�q�[�v�p�錾
static const char* heapNameTable[HEAPID_CHILD_MAX] = { 
	//�e�q�[�v
	"GFL_HEAPID_SYSTEM",
	"GFL_HEAPID_APP",
	//�풓�q�[�v
	"HEAPID_BG",
	"HEAPID_WORLD",
	"HEAPID_PROC",
	"HEAPID_APP_CONTROL",
	"HEAPID_COMMUNICATION",
  //�A�v���P�[�V�����q�[�v
  "HEAPID_TITLE",
  "HEAPID_STARTMENU",
  "HEAPID_NETWORK_FIX",
  "HEAPID_NETWORK",
  "HEAPID_ITEMMENU",
  "HEAPID_WIFI",
  "HEAPID_IRC",
  "HEAPID_LAYOUT",
  "HEAPID_BTL_SYSTEM",
  "HEAPID_BTL_VIEW",
  "HEAPID_BTL_NET",
  "HEAPID_FIELDMAP",
  "HEAPID_TITLE_DEMO",
  "HEAPID_POKELIST",
  "HEAPID_TRADE",
  "HEAPID_BALLOON",
  "HEAPID_COMPATI",
  "HEAPID_COMPATI_CONTROL",
  "HEAPID_CONFIG",
  "HEAPID_MYSTERYGIFT",
  "HEAPID_NAME_INPUT",
  "HEAPID_IRCBATTLE",
  "HEAPID_WIFICLUB",
  "HEAPID_WIFIP2PMATCH",
  "HEAPID_WIFIP2PMATCHEX",
  "HEAPID_WIFI_FOURMATCHVCHAT",
  "HEAPID_WIFI_FOURMATCH",
  "HEAPID_PALACE",
  "HEAPID_TRCARD_SYS",
  "HEAPID_TR_CARD",
  "HEAPID_OEKAKI",
  "HEAPID_PMS_INPUT_SYSTEM",
  "HEAPID_PMS_INPUT_VIEW",
  "HEAPID_MUSICAL_PROC",
  "HEAPID_MUSICAL_STRM",
  "HEAPID_MUSICAL_DRESSUP",
  "HEAPID_MUSICAL_STAGE",
  "HEAPID_MUSICAL_LOBBY",
  "HEAPID_WFLOBBY",
  "HEAPID_WFLBY_ROOM",
  "HEAPID_CODEIN",
  "HEAPID_WFLBY_ROOMGRA",
  "HEAPID_WIFINOTE",
  "HEAPID_CGEAR",
  "HEAPID_DEBUGWIN",
  "HEAPID_IRCAURA",
  "HEAPID_IRCRHYTHM",
  "HEAPID_SEL_MODE",
  "HEAPID_IRCCOMPATIBLE_SYSTEM",
  "HEAPID_IRCCOMPATIBLE",
  "HEAPID_IRCRESULT",
  "HEAPID_NEWSDRAW",
  "HEAPID_WLDTIMER",
  "HEAPID_DEBUG_MAKEPOKE",
  "HEAPID_BUCKET",
  "HEAPID_UNION",
  "HEAPID_POKE_STATUS",
  "HEAPID_IRCRANKING",
  "HEAPID_TOWNMAP",
  "HEAPID_BALANCE_BALL",
  "HEAPID_FOOTPRINT",
  "HEAPID_ANKETO",
  "HEAPID_WORLDTRADE",
  "HEAPID_MICTEST",
  //�f�o�b�O�p�q�[�v
  "HEAPID_WATANABE_DEBUG",
  "HEAPID_SOGABE_DEBUG",
  "HEAPID_TAMADA_DEBUG",
  "HEAPID_OHNO_DEBUG",
  "HEAPID_TAYA_DEBUG",
  "HEAPID_TAYA_DEBUG_SUB",
  "HEAPID_NAKAHIRO_DEBUG",
  "HEAPID_TOMOYA_DEBUG",
  "HEAPID_MATSUDA_DEBUG",
  "HEAPID_GOTO_DEBUG",
  "HEAPID_ARIIZUMI_DEBUG",
  "HEAPID_NAGI_DEBUG",
  "HEAPID_NAGI_DEBUG_SUB",
  "HEAPID_OBATA_DEBUG",
};

//----------------------------------------------------------------------------------
/**
 * �������u���b�N�w�b�_�ɕۑ�����Ă���t�@�C�������o�b�t�@�ɃR�s�[
 *
 * @param   dst      �R�s�[��o�b�t�@
 * @param   header   �������u���b�N�w�b�_
 *
 * @retval  BOOL     �������t�@�C�������擾�ł�����TRUE
 *
 * @li NitroSystem Allocator �o�R�Ń������m�ۂ���Ă���P�[�X�Ȃǂł�
 *			�t�@�C�������擾�ł��Ȃ��\��������
 */
//----------------------------------------------------------------------------------
static BOOL GetFileName( char* dst, const char* src )
{
  int i, len;

  // �I�[�R�[�h�i�V�ŗ̈�߂����ς��t�@�C�����Ɏg���Ă邽�߁A
  // �Ō��NULL������ǉ����Ă��K�v������
  for(i=0; i<FILENAME_LEN; ++i){
    if( src[i] == '\0' ){ break; }
    dst[i] = src[i];
  }
  dst[i] = '\0';
	len = i;

  // �����Ȃ�I�[�R�[�h�Ȃ琳�����t�@�C�����ł͂Ȃ��Ɣ���
  if( len == 0 ){
    return FALSE;
  }
  // ��ASCII�R�[�h���������t�@�C�����ł͂Ȃ��Ɣ���
  for(i = 0; i<len; ++i){
    if( ((u8)dst[i] < 0x20) || ((u8)dst[i]) > 0x7e){
      return FALSE;
    }
  }
  // �̈��t���g���؂��Ă��Ȃ� AND �����R�[�h '.' ���܂܂�Ă��Ȃ��Ȃ�t�@�C�����ł͂Ȃ�
  if( len < FILENAME_LEN ){
    for(i=0; i<len; ++i){
      if( (u8)(dst[i]) == '.' ){ break; }
    }
    if( i == len ){
      return FALSE;
    }
  }
  return TRUE;
}

//----------------------------------------------------------------------------------
static void HeapConflictVisitorFunc(void* memBlock, NNSFndHeapHandle heapHandle, u32 param)
{
	MCSDATA_HEAPBLOCKINFO** inParam = (MCSDATA_HEAPBLOCKINFO**)param;
	MCSDATA_HEAPBLOCKINFO* blockInfo = *inParam;

  void* memory = (u8*)memBlock + MEMHEADER_SIZE;
  DEBUG_MEMHEADER* header = (DEBUG_MEMHEADER*)GFI_HEAP_GetMemheaderUserinfo( memory );

	blockInfo->adrs = (u32)memory;
	blockInfo->size = NNS_FndGetSizeForMBlockExpHeap(memBlock)+0x10;

  if( GetFileName(blockInfo->filename, header->filename) == FALSE ){
    strcpy( blockInfo->filename, "SYSTEM ALLOC" );
    blockInfo->lineNo = 0;
  }else{
    blockInfo->lineNo = header->lineNum;
  }
	(*inParam)++;
	numMemoryTotal++;		// �q�q�[�v�̓u���b�N���Ɋ܂߂��Ȃ��̂ŕʘg�ŃJ�E���g����
}

//----------------------------------------------------------------------------------
static u32	GetExistMemoryBlocksInfo( HEAPID heapID, u32 pBuf )
{
	MCSDATA_HEAPSTATUS* heapStatus = (MCSDATA_HEAPSTATUS*)pBuf;
	MCSDATA_HEAPBLOCKINFO* param = &heapStatus->blockInfo;
  u32 sendSize;

  heapStatus->heapSize = GFI_HEAP_GetHeapTotalSize(heapID);
	if(heapStatus->heapSize == 0){ return 0; }

	heapStatus->totalFreeSize = GFI_HEAP_GetHeapFreeSize(heapID);
	heapStatus->allocatableSize = GFI_HEAP_GetHeapAllocatableSize(heapID);
	heapStatus->heapID = heapID;
	heapStatus->numMemoryBlocks = GFI_HEAP_GetBlockCount(heapID);
	strcpy( heapStatus->name, heapNameTable[heapID] );

  //�g���q�[�v����m�ۂ����������u���b�N�S�āiAlloc���ꂽ���́j�ɑ΂��A�w�肵���֐����Ă΂���
	numMemoryTotal = 0;
  NNS_FndVisitAllocatedForExpHeap
		(GFI_HEAP_GetHandle(heapID), HeapConflictVisitorFunc, (u32)&param);

	heapStatus->numMemoryTotal = numMemoryTotal;
	sendSize = (sizeof(MCSDATA_HEAPSTATUS) - sizeof(MCSDATA_HEAPBLOCKINFO)) 
						+  sizeof(MCSDATA_HEAPBLOCKINFO) * numMemoryTotal;
  if(sendSize > (SENDBUF_SIZE - MCSRSDCOMM_HEADER_SIZE)){
		OS_Printf("�q�[�v���ʂ��������ē]���ł��܂��� HEAPID = &d", heapID);
		return 0;		// �I�[�o�[�t���[
	}
	//GFL_HEAP_DEBUG_PrintExistMemoryBlocks(heapID);
	return sendSize;
}

#endif

