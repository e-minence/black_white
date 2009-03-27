
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
#include "gf_mcs.h"

//============================================================================================
/**
 *	�\���̐錾
 */
//============================================================================================

typedef struct
{
	HEAPID				heapID;
    u8*					mcsWorkMem;
    NNSMcsDeviceCaps	deviceCaps;
    NNSMcsRecvCBInfo	recvCBInfo;
	void*				printBuffer;
	void*				recvBuf;
	void*				recvBuf2;
}MCS_WORK;

static	MCS_WORK	*mw = NULL;

#define	MCS_SEND_IDLE	( 1 )

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
	if(	mw ) return TRUE;

	//�Ǘ��\���̃��������m��
	mw = GFL_HEAP_AllocMemory( heapID, sizeof( MCS_WORK ) );

	mw->heapID = heapID;

	// mcs�̏�����
	mw->mcsWorkMem = GFL_HEAP_AllocMemory( mw->heapID, NNS_MCS_WORKMEM_SIZE ); // MCS�̃��[�N�p���������m��
	NNS_McsInit( mw->mcsWorkMem );

	GFUser_VIntr_CreateTCB( MCS_VBlankIntr, NULL, 0 );

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

	GFL_HEAP_FreeMemory( mw->recvBuf2 );
	GFL_HEAP_FreeMemory( mw->recvBuf );
	GFL_HEAP_FreeMemory( mw->printBuffer );

	// NNS_McsPutString�ɂ��o��
	(void)NNS_McsPutString("device close\n");

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
	GF_ASSERT( ch < MCS_CH_MAX );
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


