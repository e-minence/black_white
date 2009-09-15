
//============================================================================================
/**
 * @file	gf_mcs.c
 * @brief	MCS�֘A�֐�
 * @author	soga
 * @date	2009.03.06
 */
//============================================================================================

#include <gflib.h>
#include <nnsys/mcs.h>

#include "system\gfl_use.h"
#include "debug/gf_mcs.h"

//============================================================================================
/**
 *	�萔
 */
//============================================================================================
#define	MCS_CH_MAX			( MCS_CHANNEL_END - 1 )
#define	MCS_CH_NUM			( MCS_CHANNEL_END )
#define	MCS_SEND_IDLE		( 1 )
#define	MCS_TIME_OUT	  ( 500 )

#define MCS_PRINTBUF_SIZE (1024)
#define MCS_RECVBUF_SIZE	(0x4000)
#define MCS_SPLIT_SIZE		(MCS_RECVBUF_SIZE	- 0x0800)

#define POKEMON_MCS_ID			(0x58976557)
#define	MCS_SEND_SIZE	( 0x1800 )
//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

typedef struct
{
	HEAPID						heapID;
	u8*								mcsWorkMem;
	NNSMcsDeviceCaps	deviceCaps;
	NNSMcsRecvCBInfo	recvCBInfo;
	void*							printBuffer;
	void*							recvBuf;
	void*							recvBuf2;
	GFL_TCB*					vBlankTask;
}MCS_WORK;

static	MCS_WORK	*mw = NULL;

//============================================================================================
/**
 *	�v���g�^�C�v�錾
 */
//============================================================================================

BOOL	MCS_Init( HEAPID heapID );
void	MCS_Exit( void );
void	MCS_Main( void );
u32		MCS_Read( void *buf, int size );
BOOL	MCS_Write( int ch, const void *buf, int size );
u32		MCS_CheckRead( void );
BOOL	MCS_CheckEnable( void );

static	void	MCS_DataRecvCallback( const void* pRecv, u32 recvSize, u32 userData, u32 offset, u32 totalSize );
static	void	MCS_VBlankIntr( GFL_TCB *tcb, void *work );
//static void MCS_CartIntrFunc( GFL_TCB *tcb, void *work );
static	void	MCS_CartIntrFunc( void );

//============================================================================================
/**
 *	�V�X�e��������
 *
 * @param[in]	heapID	�q�[�vID
 *
 * @retval	FALSE:���평�����@TRUE:���s�i���łɏ������ς݂Ȃǁj
 */
//============================================================================================
BOOL	MCS_Init( HEAPID heapID )
{
	if(	mw )
	{	
		return TRUE;
	}

	//�Ǘ��\���̃��������m��
	mw = GFL_HEAP_AllocMemory( heapID, sizeof( MCS_WORK ) );

	mw->heapID = heapID;

	// mcs�̏�����
	mw->mcsWorkMem = GFL_HEAP_AllocMemory( mw->heapID, NNS_MCS_WORKMEM_SIZE ); // MCS�̃��[�N�p���������m��
	NNS_McsInit( mw->mcsWorkMem );

	mw->vBlankTask = GFUser_VIntr_CreateTCB( MCS_VBlankIntr, NULL, 0 );

	// �J�[�g���b�W���荞�݂�L���ɂ��A�J�[�g���b�W���荞�ݓ���
  // NNS_McsCartridgeInterrupt()���Ă΂��悤�ɂ���
    OS_SetIrqFunction(OS_IE_CARTRIDGE, MCS_CartIntrFunc);
	(void)OS_EnableIrqMask(OS_IE_CARTRIDGE);

	// �f�o�C�X�̃I�[�v��
	if( NNS_McsGetMaxCaps() > 0 && NNS_McsOpen( &mw->deviceCaps ) )
	{
		mw->printBuffer = GFL_HEAP_AllocMemory( mw->heapID, 1024 );        // �v�����g�p�̃o�b�t�@�̊m��
		mw->recvBuf = GFL_HEAP_AllocMemory( mw->heapID, 0x4000 );       // ��M�p�o�b�t�@�̊m��
		mw->recvBuf2 = GFL_HEAP_AllocMemory( mw->heapID, 0x4000 );       // ��M�p�o�b�t�@�̊m��

		NNS_NULL_ASSERT( mw->printBuffer );
		NNS_NULL_ASSERT( mw->recvBuf );
		NNS_NULL_ASSERT( mw->recvBuf2 );

		// OS_Printf�ɂ��o��
		OS_Printf("device open\n");

		// mcs������o�͂̏�����
		NNS_McsInitPrint( mw->printBuffer, 1024 );

		// NNS_McsPrintf�ɂ��o��
		// ���̃^�C�~���O��mcs�T�[�o���ڑ����Ă���΁A�R���\�[���ɕ\������܂��B
		(void)NNS_McsPrintf("device ID %08X\n", mw->deviceCaps.deviceID );

		 // �ǂݎ��p�o�b�t�@�̓o�^
		NNS_McsRegisterStreamRecvBuffer( MCS_CHANNEL0, mw->recvBuf, 0x4000 );
		NNS_McsRegisterStreamRecvBuffer( MCS_CHANNEL1, mw->recvBuf2, 0x4000 );

        // ��M�R�[���o�b�N�֐��̓o�^
//		NNS_McsRegisterRecvCallback( &mw->recvCBInfo, MCS_CHANNEL1, MCS_DataRecvCallback, (u32)&mw->mw );

		if( NNS_McsIsServerConnect() == FALSE )
		{	
			OS_TPrintf("�ڑ��Ɏ��s���Ă��܂�\n");
			OS_TPrintf("�ڑ�����Ƃ��́A[�f�o�C�X]-[�ڑ�]��I��ł�������\n");
			MCS_Exit();
			return TRUE;
		}

		return FALSE;
	}
	OS_Printf("device open fail.\n");
	return TRUE;
}

//============================================================================================
/**
 *	�V�X�e���I��
 */
//============================================================================================
void	MCS_Exit( void )
{
	NNS_McsUnregisterRecvResource( MCS_CHANNEL0 );
	NNS_McsUnregisterRecvResource( MCS_CHANNEL1 );

	GFL_HEAP_FreeMemory( mw->recvBuf2 );
	GFL_HEAP_FreeMemory( mw->recvBuf );
	GFL_HEAP_FreeMemory( mw->printBuffer );

	GFL_TCB_DeleteTask( mw->vBlankTask );

	// NNS_McsPutString�ɂ��o��
	if( NNS_McsIsServerConnect() == TRUE )
	{	
		(void)NNS_McsPutString("device close\n");
		(void)NNS_McsPutString("�Đڑ�����Ƃ��́A[�f�o�C�X]-[�ؒf]��I��Őؒf������\n");
		(void)NNS_McsPutString("[�f�o�C�X]-[�ڑ�]��I��ł�������\n");
	}

	// �f�o�C�X���N���[�Y
	(void)NNS_McsClose();
	
	GFL_HEAP_FreeMemory( mw->mcsWorkMem );

	GFL_HEAP_FreeMemory( mw );
	mw = NULL;
}

//============================================================================================
/**
 *	�V�X�e�����C��
 */
//============================================================================================
void	MCS_Main( void )
{
	if( mw == NULL ) return;
	NNS_McsPollingIdle();
}

//============================================================================================
/**
 *	�ǂݍ��߂�f�[�^���������Ƃ��Ƀf�[�^��ǂ݂Ƃ�
 *
 *	@param[in]	buf		�ǂݍ��񂾃f�[�^���i�[���郏�[�N�ւ̃|�C���^
 *	@param[in]	size	�ǂݍ��ރT�C�Y
 *
 *	@retval readSize ���ۂɓǂݍ��񂾃T�C�Y
 */
//============================================================================================
u32		MCS_Read( void *buf, int size )
{
	int	len;
	u32 readSize = 0;

	// ��M�\�ȃf�[�^�T�C�Y�̎擾
	len = NNS_McsGetStreamReadableSize( MCS_CHANNEL0 );

	if( len > 0 ){
		// �f�[�^�̓ǂݎ��
		NNS_McsReadStream( MCS_CHANNEL0, buf, size, &readSize );
	}
	else{
		//�����f�[�^����M
		// ��M�\�ȃf�[�^�T�C�Y�̎擾
		u32	flag = MCS_SEND_IDLE;
		u32 read;

		readSize = size;
		NNS_McsWriteStream( MCS_CHANNEL0, &flag, 4 );

		while( size ){
			MCS_Main();
			len = NNS_McsGetStreamReadableSize( MCS_CHANNEL1 );
			if( len ){
				NNS_McsReadStream( MCS_CHANNEL1, buf, len, &read );
				size -= len;
				(u8 *)buf += len;
				NNS_McsWriteStream( MCS_CHANNEL0, &flag, 4 );
			}
		}
	}

	return readSize;
}

//============================================================================================
/**
 *	�f�[�^�������o��
 *
 *  @param[in]	ch		�����o���Ɏg�p����`�����l���i���o�[
 *	@param[in]	buf		�����o���f�[�^���i�[����Ă��郏�[�N�ւ̃|�C���^
 *	@param[in]	size	�����o���T�C�Y
 *
 *	@retval�@TRUE:�����o�������@FALSE:�����o�����s
 */
//============================================================================================
BOOL	MCS_Write( int ch, const void *buf, int size )
{
	GF_ASSERT( ch < MCS_CH_NUM );
	GF_ASSERT( size < MCS_SEND_SIZE );
	return NNS_McsWriteStream( MCS_CHANNEL0 + ch, buf, size );
}

//============================================================================================
/**
 *	�ǂݍ��߂�f�[�^�����邩�`�F�b�N
 *
 *	@retval �ǂݍ��߂�f�[�^�T�C�Y
 */
//============================================================================================
u32		MCS_CheckRead( void )
{
	u32	size;

	// ��M�\�ȃf�[�^�T�C�Y�̎擾
	size = NNS_McsGetStreamReadableSize( MCS_CHANNEL0 );

	if( size == 0 ){
	//�T�C�Y���傫���ĕ������M�����f�[�^�̎擾
		size = NNS_McsGetStreamReadableSize( MCS_CHANNEL1 );
		if( size ){
			u32 readSize = 0;
			// �f�[�^�̓ǂݎ��
			NNS_McsReadStream( MCS_CHANNEL1, &size, 4, &readSize );
		}
	}

	return size;
}

//============================================================================================
/**
 *	MCS���N�����Ă��ăf�o�C�X���I�[�v�����Ă��邩�`�F�b�N
 *
 *	@retval TRUE:�N�����@FALSE:��~��
 */
//============================================================================================
BOOL	MCS_CheckEnable( void )
{	
	if( mw == NULL )
	{	
		return FALSE;
	}

	return TRUE;
}

//============================================================================================
/**
 *	MCS���N�����Ă��ăf�o�C�X���I�[�v�����Ă��邩�`�F�b�N
 *
 *	@retval TRUE:�N�����@FALSE:��~��
 */
//============================================================================================
//============================================================================================
/**
 *	���[�J���֐�
 */
//============================================================================================
/*---------------------------------------------------------------------------*
  Name:         DataRecvCallback

  Description:  PC������f�[�^����M�����Ƃ��ɌĂ΂��R�[���o�b�N�֐��ł��B

                �o�^����R�[���o�b�N�֐����ł̓f�[�^�̑���M���s��Ȃ��ł��������B
                �܂��A���荞�݂��֎~����Ă���ꍇ�����邽�߁A
                ���荞�ݑ҂����[�v���s��Ȃ��ł��������B

  Arguments:    recv:       ��M�����f�[�^�̈ꕔ���邢�͑S�����i�[���Ă���
                            �o�b�t�@�ւ̃|�C���^�B
                recvSize:   recv�ɂ���Ď������o�b�t�@�Ɋi�[����Ă���
                            �f�[�^�̃T�C�Y�B
                userData:   NNS_McsRegisterRecvCallback()�̈���userData��
                            �w�肵���l�B
                offset:     ��M�����f�[�^�̑S����recv�ɂ���Ď������o�b�t�@��
                            �i�[����Ă���ꍇ��0�B
                            ��M�����f�[�^�̈ꕔ���i�[����Ă���ꍇ�́A
                            ��M�����f�[�^�S�̂ɑ΂���0����Ƃ����I�t�Z�b�g�ʒu�B
                totalSize:  ��M�����f�[�^�̑S�̂̃T�C�Y�B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void
MCS_DataRecvCallback(
    const void* pRecv,
    u32         recvSize,
    u32         userData,
    u32         offset,
    u32         totalSize
)
{
    MCS_WORK *mw = (MCS_WORK *)userData;

	OS_Printf( " Callback OK!\n");

	// ��M�o�b�t�@�`�F�b�N
	if (pRecv != NULL && recvSize == sizeof(MCS_WORK) && offset == 0)
	{
        mw = (MCS_WORK *)pRecv;
    }
}

/*---------------------------------------------------------------------------*
  Name:         VBlankIntr

  Description:  V�u�����N���荞�݃n���h���ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
static void MCS_VBlankIntr( GFL_TCB *tcb, void *work )
{
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
  NNS_McsVBlankInterrupt();
}

/*---------------------------------------------------------------------------*
  Name:         CartIntrFunc

  Description:  �J�[�h���b�W���荞�݃n���h���ł��B

  Arguments:    �Ȃ��B

  Returns:      �Ȃ��B
 *---------------------------------------------------------------------------*/
//static void MCS_CartIntrFunc( GFL_TCB *tcb, void *work )
static void MCS_CartIntrFunc( void )
{
  OS_SetIrqCheckFlag(OS_IE_CARTRIDGE);
  NNS_McsCartridgeInterrupt();
}











//============================================================================================
/**
 *	
 *
 *
 *
 *
 *
 *
 *
 */
//============================================================================================
//============================================================================================
/**
 *	�萔��`
 */
//============================================================================================
#define MCS_LINK_MAX	(7)	// 1�����N�ɂ�2�`�����l���g��

enum{
	GFL_MCSCOMM_NULL = 0,
	GFL_MCSCOMM_LINK,
	GFL_MCSCOMM_ANNUL,
};

#define MCS_COMMBUF_SIZE	(0x80)	// �R�}���h���N�G�X�g�i�[�o�b�t�@�T�C�Y
//============================================================================================
/**
 *	�\���̒�`
 */
//============================================================================================
// �R�}���h�w�b�_�\����
typedef struct {
	u32 projectID;		// �v���W�F�N�g����ID
	u32 commID;		    // �R�}���hID
  u32 categoryID;		// �J�e�S���[����ID
}GFL_MCS_HEADER;

// PC �� DS �ւ̐ڑ����N�G�X�g���e�\����
typedef struct {
	u32 channelID1;		// PC���Ŋm�ۂ��ꂽ�`�����l��1(�ʐM�o�b�t�@��菬�����f�[�^�ʐM�p)
	u32 channelID2;		// PC���Ŋm�ۂ��ꂽ�`�����l��2(�ʐM�o�b�t�@���傫���f�[�^�ʐM�p)
}GFL_MCS_LINKPARAM;

// PC �� DS �ւ̃����N���N�G�X�g���e�\����
typedef struct {
	u32 projectID;		// �v���W�F�N�g����ID
	u32 categoryID;		// �J�e�S���[����ID
	u32 channelID1;		// PC���Ŋm�ۂ��ꂽ�`�����l��1(�ʐM�o�b�t�@��菬�����f�[�^�ʐM�p)
	u32 channelID2;		// PC���Ŋm�ۂ��ꂽ�`�����l��2(�ʐM�o�b�t�@���傫���f�[�^�ʐM�p)
}GFL_MCS_LINKREQ;

// PC ���� DS �����N���\����
typedef struct {
	u32 categoryID;		// �J�e�S���[����ID
	u32 channelID1;		// �g�p�`�����l��1(�ʐM�o�b�t�@��菬�����f�[�^�ʐM�p)
	u32 channelID2;		// �g�p�`�����l��2(�ʐM�o�b�t�@���傫���f�[�^�ʐM�p)
}GFL_MCS_LINKKEY;

// ��M���i�[�\����
typedef struct {
	u32		size;				// ��M�f�[�^���T�C�Y
	BOOL	split;			// ������M�t���O
	BOOL	getInfo;		// ���O���擾�t���O
}GFL_MCS_READPARAM;

// PC ���� DS �����N�X�e�[�^�X
typedef struct {
	GFL_MCS_LINKKEY			key;
	GFL_MCS_READPARAM		rParam;
}GFL_MCS_LINKSTATUS;

// �`�����l���i��M�p�j
typedef struct {
	u8		recvBuf[MCS_RECVBUF_SIZE];
	BOOL	resist;
}GFL_MCS_CHANNEL;

// �V�X�e�����[�N
typedef struct {
	// ���C�u�����g�p���[�N
	u8										mcsWork[NNS_MCS_WORKMEM_SIZE];
	u8										printBuf[MCS_PRINTBUF_SIZE];
	GFL_MCS_CHANNEL				channel[MCS_CH_NUM];
	NNSMcsDeviceCaps			deviceCaps;
	//GFL_TCB*						vBlankTask;

	// �������䃏�[�N
	BOOL									deviceOpen;
	GFL_MCS_LINKSTATUS		linkStatus[MCS_LINK_MAX];

}GFL_MCS_WORK;

//============================================================================================
/**
 *	�ϐ���`
 */
//============================================================================================
static	GFL_MCS_WORK*		gflMCS = NULL;
static	GFL_MCS_WORK		gflMCSdata;
static	u8							commBuf[MCS_COMMBUF_SIZE];
static	u32							readResult;	// �ǂݎ��֐������p

static int count;

//============================================================================================
/**
 *
 *	�V�X�e��������
 *
 */
//============================================================================================
void	GFL_MCS_Init( void )
{
	int i;

	//�Ǘ��\���̃�������ݒ�
	gflMCS = &gflMCSdata;
	GFL_STD_MemClear( gflMCS, sizeof(GFL_MCS_WORK) );

	// mcs�̏�����
	NNS_McsInit( gflMCS->mcsWork );

	// VBlank���荞��
	//gflMCS->vBlankTask = GFUser_VIntr_CreateTCB( MCS_VBlankIntr, NULL, 0 );

	// �J�[�g���b�W���荞�݂�L���ɂ��A�J�[�g���b�W���荞�ݓ���
  // NNS_McsCartridgeInterrupt()���Ă΂��悤�ɂ���
  OS_SetIrqFunction(OS_IE_CARTRIDGE, MCS_CartIntrFunc);
	(void)OS_EnableIrqMask(OS_IE_CARTRIDGE);

	// �������䃏�[�N�̏�����
	gflMCS->deviceOpen = FALSE;
}

//============================================================================================
/**
 *	�V�X�e�����C��
 */
//============================================================================================
//--------------------------------------------------------------------------------------------
/**
 *	PC�Ƃ̃����N���m�����邽�߂̎�M�f�[�^�擾
 *	 (���ʃ`�����l����MCS_CHANNEL15�Œ�)
 */
//--------------------------------------------------------------------------------------------
static void	GFL_MCS_GetComm( void )
{
	u32 channelID = MCS_CHANNEL15;
	int i;

	// ��M�\�ȃf�[�^�T�C�Y�̎擾
	u32	recvSize = NNS_McsGetStreamReadableSize(channelID);

	if((recvSize > 0)&&(recvSize <= MCS_COMMBUF_SIZE)){ 
		GFL_MCS_HEADER* header;
		NNS_McsReadStream(channelID, &commBuf, recvSize, &readResult);
		header = (GFL_MCS_HEADER*)commBuf;
		OS_Printf("�R�}���h��M... projectID(%x), commID(%x), categoryID(%x)\n", 
							header->projectID, header->commID, header->categoryID);

		if(header->projectID == POKEMON_MCS_ID){
			switch(header->commID){
			case GFL_MCSCOMM_LINK:
				{
					// �����N���N�G�X�g�i�[
					int pLinkSt = GFL_MCS_LINKIDX_INVALID;
					int appActive = 0;

					for(i=0; i<MCS_LINK_MAX; i++){
						if(gflMCS->linkStatus[i].key.categoryID == 0){
							// �����N���i�[�ꏊ�ۑ�
							if(pLinkSt == GFL_MCS_LINKIDX_INVALID){ pLinkSt = i; }
						} else if(gflMCS->linkStatus[i].key.categoryID == header->categoryID){
							// ����A�v����d�N���`�F�b�N
							appActive++;
						}
					}
					if(appActive){
						OS_Printf("�����N�v������M���܂������A���ɓ���A�v���̃����N�����Ă��邩\n");
						OS_Printf("��d�N���ڑ������悤�Ƃ��Ă��܂��B�v���𖳎����܂�\n");
					} else if(pLinkSt == GFL_MCS_LINKIDX_INVALID){
						OS_Printf("�ő�ڑ��ʂ𒴂��郊���N������M���܂����B�v���𖳎����܂�\n");
					} else {
						GFL_MCS_LINKSTATUS* linkStatus = &gflMCS->linkStatus[pLinkSt];
						GFL_MCS_LINKPARAM* param = (GFL_MCS_LINKPARAM*)(commBuf + sizeof(GFL_MCS_HEADER));

						OS_Printf("�����N����M... categoryID(%x), channelID(%d, %d)\n",
											header->categoryID, param->channelID1, param->channelID2);
						linkStatus->key.categoryID = header->categoryID;
						linkStatus->key.channelID1 = param->channelID1;
						linkStatus->key.channelID2 = param->channelID2;
						// ��M�����M���Ƃ��āAPC�֑���Ԃ�
						{
							GFL_MCS_HEADER header;
							header.projectID = POKEMON_MCS_ID;
							header.commID = GFL_MCSCOMM_LINK;
							header.categoryID = linkStatus->key.categoryID;
							NNS_McsWriteStream(MCS_CHANNEL15, &header, sizeof(GFL_MCS_HEADER));
						}
					}
				}
				break;
			case GFL_MCSCOMM_ANNUL:
				// �ؒf���N�G�X�g�i = �����N���j���j
				for(i=0; i<MCS_LINK_MAX; i++){
					GFL_MCS_LINKSTATUS* linkStatus = &gflMCS->linkStatus[i];

					if(linkStatus->key.categoryID == header->categoryID){
						linkStatus->key.categoryID = 0;
						linkStatus->key.channelID1 = MCS_CHANNEL15;
						linkStatus->key.channelID2 = MCS_CHANNEL15;

						linkStatus->rParam.size = 0;
						linkStatus->rParam.split = FALSE;
						linkStatus->rParam.getInfo = FALSE;
						OS_Printf("�����N�ؒf�v������M���܂����BcategoryID(%x)\n", header->categoryID);
					}
				}
				break;
			}
		}
	} else {
		// �R�}���h��M�o�b�t�@���傫���ꍇ�͓ǂݎ̂Ă�
		while(recvSize){
			u32 tmp;
			if(recvSize <= 4){
				NNS_McsReadStream(channelID, &tmp, recvSize, &readResult);
				break;
			}
			NNS_McsReadStream(channelID, &tmp, 4, &readResult);
			recvSize -= 4;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 *	PC�Ƃ̃����N���INDEX���擾
 */
//--------------------------------------------------------------------------------------------
static u16	GFL_MCS_GetLinkIdx( u32 categoryID )
{
	int i;

	for(i=0; i<MCS_LINK_MAX; i++){
		if(gflMCS->linkStatus[i].key.categoryID == categoryID){
			return i;
		}
	}
	return GFL_MCS_LINKIDX_INVALID;
}

//--------------------------------------------------------------------------------------------
/**
 *	���C���֐�
 */
//--------------------------------------------------------------------------------------------
void GFL_MCS_Main( void )
{
	if( !gflMCS ){ return; }
	if( gflMCS->deviceOpen == FALSE ){ return; }

	NNS_McsPollingIdle();

	// �ڑ��m�F
	if(NNS_McsIsServerConnect() == FALSE){
		OS_Printf("MCS�T�[�o�Ƃ̒ʐM���ؒf����܂���\n");
		GFL_MCS_Close();
		return;
	}
	GFL_MCS_GetComm();

	//OS_Printf("MCS�ʐM��(%d)\n",count);
	count++;
}

//============================================================================================
/**
 *	�V�X�e�����荞�ݏ���
 */
//============================================================================================
void	GFL_MCS_VIntrFunc( void )
{
	if( !gflMCS ){ return; }
	if( gflMCS->deviceOpen == FALSE ){ return; }

  NNS_McsVBlankInterrupt();
}

//============================================================================================
/**
 *
 *	�V�X�e���I��
 *
 */
//============================================================================================
void	GFL_MCS_Exit( void )
{
	if(	!gflMCS ){	return; }

	// �f�o�C�X�̋����N���[�Y
	GFL_MCS_Close();

	//GFL_TCB_DeleteTask( gflMCS->vBlankTask );
	gflMCS = NULL;
}

//============================================================================================
/**
 *	�f�o�C�X�I�[�v��
 *
 * @retval	TRUE:���평�����@FALSE:���s
 */
//============================================================================================
BOOL	GFL_MCS_Open( void )
{
	int i;

	// �ڑ��f�o�C�X�̃T�[�`
	if(NNS_McsGetMaxCaps() == 0){
		OS_TPrintf("�ʐM�f�o�C�X��������܂���\n");
		return FALSE;
	}

	// �f�o�C�X�̃I�[�v��
	if(NNS_McsOpen(&gflMCS->deviceCaps) == FALSE){
		OS_TPrintf("�f�o�C�X�̃I�[�v���Ɏ��s���܂���\n");
		return FALSE;
	}
	OS_Printf("device open\n");

	// mcs������o�͂̏�����
	NNS_McsInitPrint( gflMCS->printBuf, MCS_PRINTBUF_SIZE );
	// NNS_McsPrintf�ɂ��o�́Bmcs�T�[�o���ڑ����Ă���΃R���\�[���ɕ\�������
	(void)NNS_McsPrintf("device ID %08X\n", gflMCS->deviceCaps.deviceID );

	if( NNS_McsIsServerConnect() == FALSE ){	
		OS_TPrintf("�ڑ��Ɏ��s���Ă��܂�\n");
		OS_TPrintf("�ڑ�����Ƃ��́AMCS�T�[�o��[�f�o�C�X]-[�ڑ�]��I��ł�������\n");
		(void)NNS_McsClose();
		return FALSE;
	}

	// ��M�`�����l�����I�[�v��
	for(i=0; i<MCS_CH_NUM; i++){
		GFL_MCS_CHANNEL* mcsCH = &gflMCS->channel[i];
		u32 channelID = MCS_CHANNEL0 + i;

		// �ǂݎ��p�o�b�t�@�̓o�^
		NNS_McsRegisterStreamRecvBuffer(channelID, &mcsCH->recvBuf, MCS_RECVBUF_SIZE);
		// ��M�R�[���o�b�N�֐��̓o�^
		//NNS_McsRegisterRecvCallback(&mchCH->recvCBInfo, channelID, MCS_DataRecvCallback, NULL); 

		mcsCH->resist = TRUE;
	}
	// �����N���̏�����
	for(i=0; i<MCS_LINK_MAX; i++){
		GFL_MCS_LINKSTATUS* linkStatus = &gflMCS->linkStatus[i];
		linkStatus->key.categoryID = 0;
		linkStatus->key.channelID1 = MCS_CHANNEL15;
		linkStatus->key.channelID2 = MCS_CHANNEL15;

		linkStatus->rParam.size = 0;
		linkStatus->rParam.split = FALSE;
		linkStatus->rParam.getInfo = FALSE;
	}
	gflMCS->deviceOpen = TRUE;

	count = 0;
	return TRUE;
}

//============================================================================================
/**
 *	�f�o�C�X�N���[�Y
 */
//============================================================================================
void	GFL_MCS_Close( void )
{
	int i;

	if(	!gflMCS ){	return; }
	if( gflMCS->deviceOpen == FALSE ){ return; }

	// ��M�`�����l�����N���[�Y
	for(i=0; i<MCS_CH_NUM; i++){
		GFL_MCS_CHANNEL* mcsCH = &gflMCS->channel[i];
		u32 channelID = MCS_CHANNEL0 + i;

		if(mcsCH->resist == TRUE){
			// �ǂݎ��p�o�b�t�@����уR�[���o�b�N�֐��̓o�^����
			NNS_McsUnregisterRecvResource( channelID );
			mcsCH->resist = FALSE;
		}
	}
	// �f�o�C�X���N���[�Y
	(void)NNS_McsClose();

	gflMCS->deviceOpen = FALSE;
}

//============================================================================================
/**
 *	PC�Ƃ̃����N���m�����Ă��邩�`�F�b�N����
 */
//============================================================================================
BOOL	GFL_MCS_CheckLink( u32 categoryID )
{
	int i;

	for(i=0; i<MCS_LINK_MAX; i++){
		GFL_MCS_LINKSTATUS* linkStatus = &gflMCS->linkStatus[i];

		if(linkStatus->key.categoryID == categoryID){
			return TRUE;
		}
	}
	return FALSE;
}

//============================================================================================
/**
 *	�A�C�h�����O��Ԍ��o�����M
 */
//============================================================================================
static void sendIdle( GFL_MCS_LINKKEY* pKey )
{
	u32	idleFlag = MCS_SEND_IDLE;

	NNS_McsWriteStream(pKey->channelID1, &idleFlag, 4 );
}

static BOOL checkIdle( GFL_MCS_LINKKEY* pKey )
{
	u32 size = NNS_McsGetStreamReadableSize(pKey->channelID1);
	if(size == 4){
		u32 flag;
		NNS_McsReadStream(pKey->channelID1, &flag, 4, &readResult);
		if(flag == MCS_SEND_IDLE){ return TRUE; }
	}
	return FALSE;
}

//============================================================================================
/**
 *	�f�[�^�ǂݍ��݃`�F�b�N
 */
//============================================================================================
static int GFL_MCS_CheckReadable_Core( u16 idx )
{
	GFL_MCS_LINKSTATUS* linkStatus = &gflMCS->linkStatus[idx];
	u32 size;

	// ���Z�b�g
	linkStatus->rParam.size = 0;
	linkStatus->rParam.split = FALSE;
	linkStatus->rParam.getInfo = TRUE;	// ���擾�ς݃t���O�ݒ�

	// ��M�\�ȃf�[�^�T�C�Y�̎擾(�`�����l��1)
	size = NNS_McsGetStreamReadableSize(linkStatus->key.channelID1);
	if( size ){
		//��x�̎�M�Ŏ擾�ł���f�[�^������
		linkStatus->rParam.size = size;
		linkStatus->rParam.split = FALSE;		// ������M�Ȃ�
		return size;
	}
	// ��M�\�ȃf�[�^�T�C�Y�̎擾(�`�����l��2)
	size = NNS_McsGetStreamReadableSize(linkStatus->key.channelID2);
	if( size ){
		// �������M����Ă���f�[�^������
		u32 totalSize;
		// ���T�C�Y�擾
		NNS_McsReadStream(linkStatus->key.channelID2, &totalSize, 4, &readResult);
		linkStatus->rParam.size = totalSize;
		linkStatus->rParam.split = TRUE;		// ������M����
		return totalSize;
	}
	return 0;
}

int		GFL_MCS_CheckReadable( u32 categoryID )
{
	u16 linkIdx;

	if( !gflMCS ){ return 0; }
	if( gflMCS->deviceOpen == FALSE ){ return 0; }

	linkIdx = GFL_MCS_GetLinkIdx(categoryID);
	if(linkIdx == GFL_MCS_LINKIDX_INVALID){ return  0; }

	return GFL_MCS_CheckReadable_Core(linkIdx);
}

//============================================================================================
/**
 *	�f�[�^�ǂݍ���
 */
//============================================================================================
BOOL	GFL_MCS_Read( u32 categoryID, void* pReadBuf, u32 readBufSize )
{
	GFL_MCS_LINKSTATUS* linkStatus;
	u16		linkIdx;
	u32		readSize;
	BOOL	result = FALSE;

	if( !gflMCS ){ return FALSE; }
	if( gflMCS->deviceOpen == FALSE ){ return FALSE; }

	linkIdx = GFL_MCS_GetLinkIdx(categoryID);
	if(linkIdx == GFL_MCS_LINKIDX_INVALID){ return  FALSE; }
	linkStatus = &gflMCS->linkStatus[linkIdx];

	if(linkStatus->rParam.getInfo == FALSE){ 
		// �ǂݎ���񂪎擾����Ă��Ȃ��̂ł����Ŏ擾
		GFL_MCS_CheckReadable_Core(linkIdx);
	}
	readSize = linkStatus->rParam.size;
	if(readSize){
		if(readSize > readBufSize){
			OS_Printf("�ǂݍ��݃o�b�t�@�T�C�Y�𒴂���f�[�^���������Ă��܂�\n");
			return FALSE;
		}
		result = TRUE;

		if(linkStatus->rParam.split == FALSE){
			// �ꊇ��M
			NNS_McsReadStream(linkStatus->key.channelID1, pReadBuf, readSize, &readResult);
		} else {
			// ������M
			u32 recvSize;
			sendIdle(&linkStatus->key);

			while( readSize ){
				NNS_McsPollingIdle();

				recvSize = NNS_McsGetStreamReadableSize(linkStatus->key.channelID2);
				if( recvSize ){
					NNS_McsReadStream(linkStatus->key.channelID2, pReadBuf, recvSize, &readResult);
					readSize -= recvSize;
					(u8*)pReadBuf += recvSize;

					sendIdle(&linkStatus->key);
				}
			}
		}
	}
	// ���Z�b�g
	linkStatus->rParam.size = 0;
	linkStatus->rParam.split = FALSE;
	linkStatus->rParam.getInfo = FALSE;

	return result;
}

//============================================================================================
/**
 *	�f�[�^��������
 */
//============================================================================================
BOOL	GFL_MCS_Write( u32 categoryID, const void* pWriteBuf, u32 writeSize )
{
	GFL_MCS_LINKSTATUS* linkStatus;
	u16		linkIdx;

	if( !gflMCS ){ return FALSE; }
	if( gflMCS->deviceOpen == FALSE ){ return FALSE; }

	linkIdx = GFL_MCS_GetLinkIdx(categoryID);
	if(linkIdx == GFL_MCS_LINKIDX_INVALID){ return  FALSE; }
	linkStatus = &gflMCS->linkStatus[linkIdx];

	if( writeSize <= MCS_SPLIT_SIZE ){
		// �ꊇ���M
		NNS_McsWriteStream(linkStatus->key.channelID1, pWriteBuf, writeSize);
	} else {
		// �������M
		NNS_McsWriteStream(linkStatus->key.channelID2, &writeSize, 4);	// ���T�C�Y���M

		while(writeSize){
			NNS_McsPollingIdle();

			if(checkIdle(&linkStatus->key) == TRUE){
				if(writeSize > MCS_SPLIT_SIZE){
					NNS_McsWriteStream(linkStatus->key.channelID2, pWriteBuf, MCS_SPLIT_SIZE);
					writeSize -= MCS_SPLIT_SIZE;
					(u8*)pWriteBuf += MCS_SPLIT_SIZE;
				} else {
					NNS_McsWriteStream(linkStatus->key.channelID2, pWriteBuf, writeSize);
					writeSize = 0;
				}
			}
		}
	}
	return TRUE;
}



