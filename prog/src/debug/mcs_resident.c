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

#include "sound/sound_manager.h"
#include "sound/pm_sndsys.h"
#include "debug/gf_mcs.h"

#include "debug/mcs_resident.h"

#define GFL_MCS_RESIDENT_ID	(9876543)

enum {
	MCSRSDCOMM_REQHEAPSTATUS = 1,
	MCSRSDCOMM_HEAPSTATUS,
	MCSRSDCOMM_SNDHEAPSTATUS,
	MCSRSDCOMM_REQVRAMSTATUS,
	MCSRSDCOMM_VRAMIMGSTATUS,
	MCSRSDCOMM_VRAMPLTSTATUS,
	MCSRSDCOMM_DEFAULTINFO,
};

typedef struct {
	u32 comm;
	u32 param;
}MCSRSDCOMM_HEADER;

#define MCSRSDCOMM_HEADER_SIZE (sizeof(MCSRSDCOMM_HEADER) - sizeof(u32))

#define SENDBUF_SIZE (0x08000)
#define RECVBUF_SIZE (0x08000)
static u8 MCSRSD_sendBuffer[SENDBUF_SIZE];
static u8 MCSRSD_recvBuffer[RECVBUF_SIZE];

#define TIMER_LEN (60)//(32)
static u16	tickStack[TIMER_LEN];
static u16	tickStackP = 0;
static u16	tickMax = 0;
extern OSTick DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now;    // ���݂̃`�b�N
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
			GFL_MCS_Resident_SendHeapStatus();
			break;
		case MCSRSDCOMM_REQVRAMSTATUS:
			GFL_MCS_Resident_SendTexVramStatus();
			break;
		}
	}
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y){
		//GFL_MCS_Resident_SendTexVramStatus();
	}
#if 0
	{
		if(tickStackP >= BUF_LEN){ tickStackP = 0; }

		if(DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now >= 0xf000){
			tickStack[tickStackP] = 0xf000;
		} else {
			tickStack[tickStackP] = (u16)DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now;
		}
		if(tickStack[tickStackP] > tickMax){ tickMax = tickStack[tickStackP]; }

		tickStackP++;
	}

	if(PMSND_IsLoading() == FALSE){
		if(tickStackP >= TIMER_LEN){
			MCSRSDCOMM_HEADER* commHeader = (MCSRSDCOMM_HEADER*)MCSRSD_sendBuffer;
			int i;
			u32 param = 0;
			for(i=0; i<tickStackP; i++){ param += tickStack[i]; }
			param /= tickStackP;
			param &= 0x0000ffff;			// ����16bit�ɃA�x���[�W�i�[
			param |= (tickMax << 16);	// ���16bit�ɍő�l�i�[
	
			commHeader->comm = MCSRSDCOMM_DEFAULTINFO;
			commHeader->param = param;
			GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sizeof(MCSRSDCOMM_HEADER));

			tickStackP = 0;
			tickMax = 0;
		}
	}
#else
	{
		if(tickStackP >= TIMER_LEN){ tickStackP = 0; tickMax = 0; }

		if(DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now >= 0xf000){
			tickStack[tickStackP] = 0xf000;
		} else {
			tickStack[tickStackP] = (u16)DEBUG_DEBUG_MAIN_TIME_AVERAGE_Now;
		}
		if(tickStack[tickStackP] > tickMax){ tickMax = tickStack[tickStackP]; }

		tickStackP++;
	}

	if(tickStackP == TIMER_LEN){
		MCSRSDCOMM_HEADER* commHeader = (MCSRSDCOMM_HEADER*)MCSRSD_sendBuffer;
		int i;
		u32 param = 0;
		for(i=0; i<TIMER_LEN; i++){ param += tickStack[i]; }
		param /= TIMER_LEN;
		param &= 0x0000ffff;			// ����16bit�ɃA�x���[�W�i�[
		param |= (tickMax << 16);	// ���16bit�ɍő�l�i�[

		commHeader->comm = MCSRSDCOMM_DEFAULTINFO;
		commHeader->param = param;
		GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sizeof(MCSRSDCOMM_HEADER));
	}
#endif
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

#define BGMPLAYER_NUM (6)	// ref. sound/sound_manager.h
typedef struct {
  u32		heapSize;											// �q�[�v�S�̃T�C�Y
	u32		heapRemainsAfterSys;					// �V�X�e���t�@�C�����[�h��̎c�q�[�v�T�C�Y
	u32		heapRemainsAfterPlayer;				// �v���[���[�ݒ��̎c�q�[�v�T�C�Y
	u32		heapRemainsAfterPresetSE;			// �풓SE�ݒ��̎c�q�[�v�T�C�Y
	u32		sePlayerNum;									// �ݒ�SE�v���[���[��
	u32		totalFreeSize;								// �c��̃q�[�v�T�C�Y
	u32		playerSoundID[BGMPLAYER_NUM];	// ���ݎg�p����Ă���BGM�v���[���[���e�isoundID�j
}MCSDATA_SNDHEAPSTATUS;

static int numMemoryTotal;

/**
 * @brief �����񐶐��}�N��
 * heap_define.h���ŕ�����z��Ƃ��ēW�J����邽�߂̃}�N����`
 */
#define HEAP_DEFINE( heap_id )  #heap_id ,

//�q�q�[�v�p�錾
static const char* heapNameTable[HEAPID_CHILD_MAX] = { 
	//�e�q�[�v
	"GFL_HEAPID_SYSTEM",
	"GFL_HEAPID_APP",
	//�풓�q�[�v
	"HEAPID_BG",		//2

  //��L�ȊO�̃q�[�v���͉��L�t�@�C�����}�N���W�J���Đ���
#include "system/heap_define.h"

};


static u32	GetExistMemoryBlocksInfo( HEAPID heapID, u32 pBuf );

BOOL	GFL_MCS_Resident_SendHeapStatus(void)
{
	MCSRSDCOMM_HEADER* commHeader = (MCSRSDCOMM_HEADER*)MCSRSD_sendBuffer;
	u32 sendSize;
	int i;

	commHeader->comm = MCSRSDCOMM_HEAPSTATUS;
	commHeader->param = 0;
	for(i=0; i<HEAPID_CHILD_MAX-1; i++){
		if(GFI_HEAP_GetHeapTotalSize(i) != 0){ commHeader->param++; }
	}
	sendSize = sizeof(MCSRSDCOMM_HEADER_SIZE) + sizeof(u32);
	GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);

	// �q�[�v���쐬���]��
	for(i=0; i<HEAPID_CHILD_MAX-1; i++){
		sendSize = GetExistMemoryBlocksInfo( i, (u32)MCSRSD_sendBuffer);
		if(sendSize){
			GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);
			//OS_Printf("send Heap Status %d, %d\n", i, sendSize);
		}
	}

	commHeader->comm = MCSRSDCOMM_SNDHEAPSTATUS;
	{
		MCSDATA_SNDHEAPSTATUS* shs = (MCSDATA_SNDHEAPSTATUS*)&commHeader->param;
		shs->heapSize = PMSND_GetSndHeapSize();
		shs->heapRemainsAfterSys = PMSND_GetSndHeapRemainsAfterSys();
		shs->heapRemainsAfterPlayer = PMSND_GetSndHeapRemainsAfterPlayer();
		shs->heapRemainsAfterPresetSE = PMSND_GetSndHeapRemainsAfterPresetSE();
		shs->sePlayerNum = PMSND_GetSEPlayerNum();
		shs->totalFreeSize = PMSND_GetSndHeapFreeSize();
		for(i=0; i<BGMPLAYER_NUM; i++){
			shs->playerSoundID[i] = SOUNDMAN_GetHierarchyPlayerSoundIdxByPlayerID(i);	
		}
	}
	sendSize = sizeof(MCSRSDCOMM_HEADER_SIZE) + sizeof(MCSDATA_SNDHEAPSTATUS);
	GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);

	return TRUE;
}

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
    STD_StrCpy( blockInfo->filename, "SYSTEM ALLOC" );
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
	STD_StrCpy( heapStatus->name, heapNameTable[heapID] );

  //�g���q�[�v����m�ۂ����������u���b�N�S�āiAlloc���ꂽ���́j�ɑ΂��A�w�肵���֐����Ă΂���
	numMemoryTotal = 0;
  NNS_FndVisitAllocatedForExpHeap
		(GFI_HEAP_GetHandle(heapID), HeapConflictVisitorFunc, (u32)&param);

	heapStatus->numMemoryTotal = numMemoryTotal;
	sendSize = (sizeof(MCSDATA_HEAPSTATUS) - sizeof(MCSDATA_HEAPBLOCKINFO)) 
						+  sizeof(MCSDATA_HEAPBLOCKINFO) * numMemoryTotal;
  if(sendSize > SENDBUF_SIZE){
		OS_Printf("�q�[�v���ʂ��������ē]���ł��܂��� HEAPID = &d", heapID);
		return 0;		// �I�[�o�[�t���[
	}
	//GFL_HEAP_DEBUG_PrintExistMemoryBlocks(heapID);
	return sendSize;
}


//============================================================================================
/**
 *
 * @brief	�e�N�X�`��VRAM���擾
 *
 */
//============================================================================================
typedef struct {
	u16		type;			// VRAM����U��^�C�v
	u16		size1;		// �u���b�N1�T�C�Y(>>12)
	u16		size2;		// �u���b�N2�T�C�Y(>>12)
	u8		pad[2];
}MCSDATA_TEXVRAMSTATUS;

//----------------------------------------------------------------------------
// LCDC�X�y�[�X���z�u�f�[�^�e�[�u��
//----------------------------------------------------------------------------
#define TEX_ADRSDEF_A		((u16)(HW_LCDC_VRAM_A >> 12))
#define TEX_ADRSDEF_B		((u16)(HW_LCDC_VRAM_B >> 12))
#define TEX_ADRSDEF_C		((u16)(HW_LCDC_VRAM_C >> 12))
#define TEX_ADRSDEF_D		((u16)(HW_LCDC_VRAM_D >> 12))
#define TEX_SIZEDEF_A		((u16)(HW_VRAM_A_SIZE >> 12))
#define TEX_SIZEDEF_B		((u16)(HW_VRAM_B_SIZE >> 12))
#define TEX_SIZEDEF_C		((u16)(HW_VRAM_C_SIZE >> 12))
#define TEX_SIZEDEF_D		((u16)(HW_VRAM_D_SIZE >> 12))
#define TEX_SIZEDEF_AB	((u16)((HW_VRAM_A_SIZE + HW_VRAM_B_SIZE) >> 12))
#define TEX_SIZEDEF_BC	((u16)((HW_VRAM_B_SIZE + HW_VRAM_C_SIZE) >> 12))
#define TEX_SIZEDEF_CD	((u16)((HW_VRAM_C_SIZE + HW_VRAM_D_SIZE) >> 12))
#define TEX_SIZEDEF_ABC	((u16)((HW_VRAM_A_SIZE + HW_VRAM_B_SIZE + HW_VRAM_C_SIZE) >> 12))
#define TEX_SIZEDEF_BCD	((u16)((HW_VRAM_D_SIZE + HW_VRAM_C_SIZE + HW_VRAM_D_SIZE) >> 12))
#define TEX_SIZEDEF_ABCD ((u16)((HW_VRAM_A_SIZE+HW_VRAM_B_SIZE+HW_VRAM_C_SIZE+HW_VRAM_D_SIZE)>>12))
static const struct {
	u16     blk1adrs;	// 12 bit shift
	u16     blk1size;	// 12 bit shift
	u16     blk2adrs;	// 12 bit shift
	u16     blk2size;	// 12 bit shift
} sTexImgStartAddrTable[16] = {
	{0, 0, 0, 0},																										// GX_VRAM_TEX_NONE
	{TEX_ADRSDEF_A, TEX_SIZEDEF_A,	0, 0},													// GX_VRAM_TEX_0_A
	{TEX_ADRSDEF_B, TEX_SIZEDEF_B,	0, 0},													// GX_VRAM_TEX_0_B
	{TEX_ADRSDEF_A,	TEX_SIZEDEF_AB,	0, 0},													// GX_VRAM_TEX_01_AB
	{TEX_ADRSDEF_C,	TEX_SIZEDEF_C,	0, 0},													// GX_VRAM_TEX_0_C
	{TEX_ADRSDEF_A, TEX_SIZEDEF_A,	TEX_ADRSDEF_C, TEX_SIZEDEF_C},	// GX_VRAM_TEX_01_AC
	{TEX_ADRSDEF_B, TEX_SIZEDEF_BC,	0, 0},													// GX_VRAM_TEX_01_BC
	{TEX_ADRSDEF_A, TEX_SIZEDEF_ABC, 0, 0},													// GX_VRAM_TEX_012_ABC
	{TEX_ADRSDEF_D, TEX_SIZEDEF_D,	0, 0},													// GX_VRAM_TEX_0_D
	{TEX_ADRSDEF_A,	TEX_SIZEDEF_A,	TEX_ADRSDEF_D, TEX_SIZEDEF_D},	// GX_VRAM_TEX_01_AD
	{TEX_ADRSDEF_B,	TEX_SIZEDEF_B,	TEX_ADRSDEF_D, TEX_SIZEDEF_D},	// GX_VRAM_TEX_01_BD
	{TEX_ADRSDEF_A,	TEX_SIZEDEF_AB,	TEX_ADRSDEF_D, TEX_SIZEDEF_D},	// GX_VRAM_TEX_012_ABD
	{TEX_ADRSDEF_C, TEX_SIZEDEF_CD,	0, 0},													// GX_VRAM_TEX_01_CD
	{TEX_ADRSDEF_A, TEX_SIZEDEF_A,	TEX_ADRSDEF_C, TEX_SIZEDEF_CD},	// GX_VRAM_TEX_012_ACD
	{TEX_ADRSDEF_B, TEX_SIZEDEF_BCD, 0, 0},													// GX_VRAM_TEX_012_BCD
	{TEX_ADRSDEF_A, TEX_SIZEDEF_ABCD, 0, 0},												// GX_VRAM_TEX_0123_ABCD
};

#define TEXPLT_ADRSDEF_E		((u16)(HW_LCDC_VRAM_E >> 12))
#define TEXPLT_ADRSDEF_F		((u16)(HW_LCDC_VRAM_F >> 12))
#define TEXPLT_ADRSDEF_G		((u16)(HW_LCDC_VRAM_G >> 12))
#define TEXPLT_SIZEDEF_E		((u16)(HW_VRAM_E_SIZE >> 12))
#define TEXPLT_SIZEDEF_F		((u16)(HW_VRAM_F_SIZE >> 12))
#define TEXPLT_SIZEDEF_G		((u16)(HW_VRAM_G_SIZE >> 12))
#define TEXPLT_SIZEDEF_EF		((u16)((HW_VRAM_E_SIZE + HW_VRAM_G_SIZE) >> 12))
#define TEXPLT_SIZEDEF_FG		((u16)((HW_VRAM_F_SIZE + HW_VRAM_G_SIZE) >> 12))
#define TEXPLT_SIZEDEF_EFG	((u16)((HW_VRAM_E_SIZE + HW_VRAM_F_SIZE + HW_VRAM_G_SIZE) >> 12))

static const struct {
	u16     blk1adrs;	// 12 bit shift
	u16     blk1size;	// 12 bit shift
} sTexPltStartAddrTable[16] = {
	{0, 0},																				// GX_VRAM_TEXPLTT_NONE
	{TEXPLT_ADRSDEF_F,		TEXPLT_SIZEDEF_F},			// GX_VRAM_TEXPLTT_0_F
	{TEXPLT_ADRSDEF_G,		TEXPLT_SIZEDEF_G},			// GX_VRAM_TEXPLTT_0_G
	{TEXPLT_ADRSDEF_F,		TEXPLT_SIZEDEF_FG},			// GX_VRAM_TEXPLTT_01_FG
	{TEXPLT_ADRSDEF_E,		TEXPLT_SIZEDEF_E},			// GX_VRAM_TEXPLTT_0123_E
	{TEXPLT_ADRSDEF_E,		TEXPLT_SIZEDEF_EF},			// GX_VRAM_TEXPLTT_01234_EF
	{0, 0},																				// dummy
	{TEXPLT_ADRSDEF_E,	TEXPLT_SIZEDEF_EFG},			// GX_VRAM_TEXPLTT_012345_EFG
};

BOOL	GFL_MCS_Resident_SendTexVramStatus(void)
{
	MCSRSDCOMM_HEADER* commHeader = (MCSRSDCOMM_HEADER*)MCSRSD_sendBuffer;
	MCSDATA_TEXVRAMSTATUS* vStatus;
	u32 sendSize;

	// VRAM�C���[�W�]��
	{
		GXVRamTex stTexImg = (GXVRamTex)(0);
		u32 blk1adrs, blk2adrs, blk1size, blk2size;

		commHeader->comm = MCSRSDCOMM_VRAMIMGSTATUS;
		vStatus = (MCSDATA_TEXVRAMSTATUS*)&commHeader->param;

		//�e�N�X�`���X���b�g��LCDC�X�y�[�X�Ɋ���U�� + ���݂̊���U���Ԃ��擾
		stTexImg = GX_ResetBankForTex();
		// ��`�f�[�^�����̂܂�bitfield�Ƃ��Ďg�p�ł���̂ł��̂܂ܑ��
		vStatus->type = stTexImg;

		// VRAM���쐬���]��
		blk1adrs = (u32)sTexImgStartAddrTable[vStatus->type].blk1adrs;
		blk1size = (u32)sTexImgStartAddrTable[vStatus->type].blk1size;
		blk2adrs = (u32)sTexImgStartAddrTable[vStatus->type].blk2adrs;
		blk2size = (u32)sTexImgStartAddrTable[vStatus->type].blk2size;

		vStatus->size1 = blk1size;
		vStatus->size2 = blk2size;
		sendSize = sizeof(MCSRSDCOMM_HEADER_SIZE) + sizeof(MCSDATA_TEXVRAMSTATUS);
		GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);

		if(vStatus->size1){
			//OS_Printf("texvram_img send 1\n");
			GFL_MCS_Write(GFL_MCS_RESIDENT_ID, (void*)(blk1adrs << 12), (blk1size << 12));
		}
		if(vStatus->size2){
			//OS_Printf("texvram_img send 2\n");
			GFL_MCS_Write(GFL_MCS_RESIDENT_ID, (void*)(blk2adrs << 12), (blk2size << 12));
		}
		// �e�N�X�`���X���b�g�̕��A
		GX_SetBankForTex(stTexImg);
	}
	// VRAM�p���b�g�]��
	{
		GXVRamTexPltt stTexPlt = (GXVRamTexPltt)(0);
		u32 blk1adrs, blk1size;

		commHeader->comm = MCSRSDCOMM_VRAMPLTSTATUS;
		vStatus = (MCSDATA_TEXVRAMSTATUS*)&commHeader->param;

		//�p���b�g�X���b�g��LCDC�X�y�[�X�Ɋ���U�� + ���݂̊���U���Ԃ��擾
		stTexPlt = GX_ResetBankForTexPltt();
		// bitfield�f�[�^�Ƃ��ĕϊ�����B
		switch(stTexPlt){
		default:
		case GX_VRAM_TEXPLTT_NONE:
			vStatus->type = 0;
			break;
		case GX_VRAM_TEXPLTT_0_F:
			vStatus->type = 1;
			break;
		case GX_VRAM_TEXPLTT_0_G:
			vStatus->type = 2;
			break;
		case GX_VRAM_TEXPLTT_01_FG:
			vStatus->type = 3;
			break;
		case GX_VRAM_TEXPLTT_0123_E:
			vStatus->type = 4;
			break;
		case GX_VRAM_TEXPLTT_01234_EF:
			vStatus->type = 5;
			break;
		case GX_VRAM_TEXPLTT_012345_EFG:
			vStatus->type = 7;
			break;
		}
		// VRAM���쐬���]��
		blk1adrs = (u32)sTexPltStartAddrTable[vStatus->type].blk1adrs;
		blk1size = (u32)sTexPltStartAddrTable[vStatus->type].blk1size;

		vStatus->size1 = blk1size;
		vStatus->size2 = 0;
		sendSize = sizeof(MCSRSDCOMM_HEADER_SIZE) + sizeof(MCSDATA_TEXVRAMSTATUS);
		GFL_MCS_Write(GFL_MCS_RESIDENT_ID, MCSRSD_sendBuffer, sendSize);
		if(vStatus->size1){
			//OS_Printf("texvram_plt send 1\n");
			GFL_MCS_Write(GFL_MCS_RESIDENT_ID, (void*)(blk1adrs << 12), (blk1size << 12));
		}
		// �p���b�g�X���b�g�̕��A
		GX_SetBankForTexPltt(stTexPlt);
	}
	return TRUE;
}


#endif

