//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		snd_strm.c
 *	@brief		�T�E���h	�X�g���[�~���O�Đ�
 *	@author		tomoya takahashi
 *	@data		2008.12.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "sound/snd_strm.h"

//-----------------------------------------------------------------------------
/**
 *					�R�[�f�B���O�K��
 *		���֐���
 *				�P�����ڂ͑啶������ȍ~�͏������ɂ���
 *		���ϐ���
 *				�E�ϐ�����
 *						const�ɂ� c_ ��t����
 *						static�ɂ� s_ ��t����
 *						�|�C���^�ɂ� p_ ��t����
 *						�S�č��킳��� csp_ �ƂȂ�
 *				�E�O���[�o���ϐ�
 *						�P�����ڂ͑啶��
 *				�E�֐����ϐ�
 *						�������Ɓh�Q�h�Ɛ������g�p���� �֐��̈���������Ɠ���
 */
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					�萔�錾
 */
//-----------------------------------------------------------------------------

//-------------------------------------
///	�`�����l��
//=====================================
#define SND_STRM_CHANNEL_MAX		(1)
static const u8 sc_SND_STRM_CHANNEL_TBL[ SND_STRM_CHANNEL_MAX ] = {
	0,
};

//-------------------------------------
///	���g��
//=====================================
static const int sc_SND_STRM_HZ_TBL[ SND_STRM_HZMAX ] = {
	NNS_SND_STRM_TIMER_CLOCK/8000,
	NNS_SND_STRM_TIMER_CLOCK/11025,
	NNS_SND_STRM_TIMER_CLOCK/16000,
	NNS_SND_STRM_TIMER_CLOCK/22050,
	NNS_SND_STRM_TIMER_CLOCK/32000,
};

//-------------------------------------
/// �T�E���h�f�[�^�^�C�v
//=====================================
static const u32 sc_SND_STRM_DATATYPE[ SND_STRM_TYPE_MAX ] = {
	NNS_SND_STRM_FORMAT_PCM8,
	NNS_SND_STRM_FORMAT_PCM16,
};

//-------------------------------------
///	�X�g���[�~���O�ǂݍ��݂P�T�C�Y
//=====================================
#define SND_STRM_ONELOAD_DATASIZ	(0)



//-------------------------------------
///	���̑����
//=====================================
#define	INTERVAL		(16)
#define	STRM_BUF_SIZE	( INTERVAL * 2 * SND_STRM_CHANNEL_MAX * 32 )
#define	SWAV_HEAD_SIZE	(18)

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
 */
//-----------------------------------------------------------------------------
typedef struct
{
	NNSSndStrm			strm;
	u8					FS_strmBuffer[STRM_BUF_SIZE];
	int					FSReadPos;
	u32					strmReadPos;
	u32					strmWritePos;
	BOOL				playing;

	// ���y���
	BOOL				snddata_in;
	u32					arcid;
	u32					dataid;
	ARCHANDLE*			p_handle;

	u8* pStraightData;

	u32					data_siz;
	u32					seek_top;
	u32					type;
	u32					hz;
	
	BOOL        isLoop;
	BOOL        isFinishData;
}STRM_WORK;

//���[�N
static STRM_WORK*	sp_STRM_WORK = NULL;


//32�o�C�g�A���C�����g�Ńq�[�v����̎������킩��Ȃ��̂ŁA�Ƃ肠�����ÓI��
static	u8				strmBuffer[STRM_BUF_SIZE] ATTRIBUTE_ALIGN(32);

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
 */
//-----------------------------------------------------------------------------
static void SND_STRM_StockWaveData( NNSSndStrmCallbackStatus status, int numChannels, void *buffer[], u32	len, NNSSndStrmFormat format, void *arg );
static void SND_STRM_CopyBuffer( STRM_WORK* p_wk,int size );


//----------------------------------------------------------------------------
/**
 *	@brief	�X�g���[�~���O�Đ��V�X�e��	������
 *
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void SND_STRM_Init( u32 heapID )
{
	GF_ASSERT( sp_STRM_WORK == NULL );

	sp_STRM_WORK = GFL_HEAP_AllocMemory( heapID, sizeof(STRM_WORK) );
	GFL_STD_MemClear( sp_STRM_WORK, sizeof(STRM_WORK) );

	// �X�g���[�~���O�`�����l���ݒ�
	NNS_SndInit();
	NNS_SndStrmInit( &sp_STRM_WORK->strm );
	NNS_SndStrmAllocChannel( &sp_STRM_WORK->strm, SND_STRM_CHANNEL_MAX, sc_SND_STRM_CHANNEL_TBL);
	
	sp_STRM_WORK->isLoop = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�g���[�~���O�Đ��V�X�e��	�j��
 */
//-----------------------------------------------------------------------------
void SND_STRM_Exit( void )
{
	GF_ASSERT( sp_STRM_WORK );

	if(sp_STRM_WORK){
		// �X�g���[�~���O�`�����l���j��
		NNS_SndStrmFreeChannel( &sp_STRM_WORK->strm );

		// �������j��
		GFL_HEAP_FreeMemory( sp_STRM_WORK );
		sp_STRM_WORK = NULL;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�g���[�~���O�Đ��V�X�e��	���C��
 */
//-----------------------------------------------------------------------------
void SND_STRM_Main( void )
{
	GF_ASSERT( sp_STRM_WORK );

	if( sp_STRM_WORK->snddata_in ){
		SND_STRM_CopyBuffer( sp_STRM_WORK, SND_STRM_ONELOAD_DATASIZ );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�T�E���h�f�[�^	�ݒ�
 *
 *	@param	arcid		�A�[�NID
 *	@param	dataid		�f�[�^ID
 *	@param	type		�f�[�^�^�C�v
 *	@param	hz			�T���v�����O����
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void SND_STRM_SetUp( u32 arcid, u32 dataid, SND_STRM_TYPE type, SND_STRM_HZ hz, u32 heapID )
{
	GF_ASSERT( sp_STRM_WORK );


	sp_STRM_WORK->snddata_in = TRUE;
	sp_STRM_WORK->arcid		= arcid;
	sp_STRM_WORK->dataid	= dataid;
	sp_STRM_WORK->p_handle	= GFL_ARC_OpenDataHandle( sp_STRM_WORK->arcid, heapID );
	sp_STRM_WORK->data_siz	= GFL_ARC_GetDataSizeByHandle( sp_STRM_WORK->p_handle, sp_STRM_WORK->dataid );
	sp_STRM_WORK->seek_top	= GFL_ARC_GetDataOfsByHandle( sp_STRM_WORK->p_handle, sp_STRM_WORK->dataid );
	sp_STRM_WORK->seek_top	+= SWAV_HEAD_SIZE;
	sp_STRM_WORK->type		= type;
	sp_STRM_WORK->hz		= hz;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�E���h�f�[�^	�ݒ�
 *
 *	@param	type		�f�[�^�^�C�v
 *	@param	hz			�T���v�����O����
 *	@param	heapID	�q�[�vID
 *	@param	data		���̃T���v�����O�f�[�^
 *	@param	size		�T�C�Y
 */
//-----------------------------------------------------------------------------
void SND_STRM_SetUpStraightData( SND_STRM_TYPE type, SND_STRM_HZ hz, u32 heapID, u8* data,u32 size)
{
	GF_ASSERT( sp_STRM_WORK );

	sp_STRM_WORK->snddata_in = TRUE;
	sp_STRM_WORK->p_handle = NULL;
	sp_STRM_WORK->pStraightData = data;
	sp_STRM_WORK->data_siz = size;
	sp_STRM_WORK->type = type;
	sp_STRM_WORK->hz = hz;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�T�E���h�f�[�^�̔j��
 */
//-----------------------------------------------------------------------------
void SND_STRM_Release( void )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );

	// ��~
	NNS_SndStrmStop( &sp_STRM_WORK->strm );
	sp_STRM_WORK->playing = FALSE;

	// �j��
	if( sp_STRM_WORK->p_handle != NULL )
	{
  	GFL_ARC_CloseDataHandle( sp_STRM_WORK->p_handle );
  }

	sp_STRM_WORK->snddata_in = FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief		�T�E���h�f�[�^�̗L�����擾
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL SND_STRM_CheckSetUp( void )
{
	if( !sp_STRM_WORK ){
		return FALSE;
	}
	return sp_STRM_WORK->snddata_in;

}


//----------------------------------------------------------------------------
/**
 *	@brief	�Đ�
 */
//-----------------------------------------------------------------------------
void SND_STRM_Play( void )
{
	BOOL ret;
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );

	// �p�����[�^������
	if(sp_STRM_WORK->pStraightData){
		sp_STRM_WORK->FSReadPos = 0;
	}
	else{
		sp_STRM_WORK->FSReadPos = SWAV_HEAD_SIZE;
	}
	sp_STRM_WORK->strmReadPos=0;
	sp_STRM_WORK->strmWritePos=0;
	sp_STRM_WORK->isFinishData = FALSE;


	// �Z�b�g�A�b�v
	ret = NNS_SndStrmSetup( &sp_STRM_WORK->strm,
													sc_SND_STRM_DATATYPE[sp_STRM_WORK->type],
													&strmBuffer[0],
													STRM_BUF_SIZE,
													sc_SND_STRM_HZ_TBL[sp_STRM_WORK->hz],
													INTERVAL,
													SND_STRM_StockWaveData,
													sp_STRM_WORK);
	GF_ASSERT(ret);

	// �o�b�t�@��Ԃ��N���A
	MI_CpuClearFast( &strmBuffer[0], STRM_BUF_SIZE );

	// �f�[�^��[
	SND_STRM_CopyBuffer( sp_STRM_WORK, STRM_BUF_SIZE );

	NNS_SndStrmStart( &sp_STRM_WORK->strm );

	sp_STRM_WORK->playing = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	��~
 */
//-----------------------------------------------------------------------------
void SND_STRM_Stop( void )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );
	if( sp_STRM_WORK->playing ){
		NNS_SndStrmStop( &sp_STRM_WORK->strm );
		sp_STRM_WORK->playing = FALSE;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Đ������`�F�b�N����
 *
 *	@retval	TRUE	�Đ���
 *	@retval	FALSE	��~��
 */
//-----------------------------------------------------------------------------
BOOL SND_STRM_CheckPlay( void )
{
	GF_ASSERT( sp_STRM_WORK );
	//GF_ASSERT( sp_STRM_WORK->snddata_in );
	return sp_STRM_WORK->playing;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�X�g���[�����Ō�܂ōs�������H
 *          ���ݍŌ�̃o�b�t�@�𑗂�O�Ƀt���O�������܂��B
 *          �Ȃ̂ŃM���M���܂Ńf�[�^�������Ă���ƍĐ�������Ȃ��ꍇ������܂��B
 *          ���[�v�ݒ莞�͕K��FALSE�ł��B
 *
 *	@retval	TRUE	�Ō�܂ōs����
 *	@retval	FALSE	�Đ���
 */
//-----------------------------------------------------------------------------
const BOOL SND_STRM_CheckFinish( void )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );
	
	if( sp_STRM_WORK->playing == TRUE &&
	    sp_STRM_WORK->isLoop == FALSE &&
	    sp_STRM_WORK->isFinishData == TRUE )
	{
  	return TRUE;
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�v�t���O�̐ݒ�
 *
 *	@param	isLoop	���[�v�ݒ�
 */
//-----------------------------------------------------------------------------
void SND_STRM_SetLoopFlg( const BOOL isLoop )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );
	sp_STRM_WORK->isLoop = isLoop;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���[�v�t���O�̎擾
 *
 *	@retval	TRUE	���[�v����
 *	@retval	FALSE	���[�v���Ȃ�
 */
//-----------------------------------------------------------------------------
const BOOL SND_STRM_GetLoopFlg( void )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );
	return sp_STRM_WORK->isLoop;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�����[���ݒ�
 *
 *	@param	volume	�{�����[��
 */
//-----------------------------------------------------------------------------
void SND_STRM_Volume( int volume )
{
	GF_ASSERT( sp_STRM_WORK );
	GF_ASSERT( sp_STRM_WORK->snddata_in );

	NNS_SndStrmSetVolume( &sp_STRM_WORK->strm, volume );
}






//-----------------------------------------------------------------------------
/**
 *			�v���C�x�[�g�֐�
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	�g�`�f�[�^���[����R�[���o�b�N
 *
 *	@param	status
 *	@param	num_channels
 *	@param	p_buffer[]
 *	@param	len
 *	@param	format
 *	@param	p_arg
 */
//-----------------------------------------------------------------------------
static void SND_STRM_StockWaveData( NNSSndStrmCallbackStatus status, int num_channels, void* p_buffer[], u32 len, NNSSndStrmFormat format, void* p_arg )
{
	STRM_WORK *sw=(STRM_WORK *)p_arg;
	int	i;
	u8	*strBuf;

	strBuf = p_buffer[0];

	for(i=0;i<len;i++){
		strBuf[i]=sw->FS_strmBuffer[i+sw->strmReadPos];
	}

	sw->strmReadPos+=len;
	if(sw->strmReadPos>=STRM_BUF_SIZE){
		sw->strmReadPos=0;
	}
	DC_FlushRange(strBuf,len);
}




//----------------------------------------------------------------------------
/**
 *	@brief	�g�`�f�[�^���[����R�[���o�b�N�����荞�݋֎~�ŌĂ΂��̂ŁA���̊֐��Ń��[�h���Ă���
 *
 *	@param	p_wk		���[�N
 *	@param	size		�T�C�Y
 */
//-----------------------------------------------------------------------------
static void SND_STRM_CopyBuffer( STRM_WORK* p_wk,int size )
{
	FSFile	file;
	s32		ret;

	GF_ASSERT( p_wk->snddata_in );

	if(p_wk->pStraightData){
		if(size){
			GFL_STD_MemCopy(&p_wk->pStraightData[p_wk->FSReadPos], &p_wk->FS_strmBuffer[0] , size);
			p_wk->FSReadPos+=size;
		}
		else{
			while(p_wk->strmReadPos!=p_wk->strmWritePos){
        if( sp_STRM_WORK->isFinishData == FALSE )
        {
  				GFL_STD_MemCopy(&p_wk->pStraightData[p_wk->FSReadPos], &p_wk->FS_strmBuffer[p_wk->strmWritePos] , 32);
        }
        else
        {
          GFL_STD_MemClear( &p_wk->FS_strmBuffer[p_wk->strmWritePos] , 32 );
        }
				p_wk->FSReadPos+=32;
				p_wk->strmWritePos+=32;
				if(p_wk->strmWritePos>=STRM_BUF_SIZE){
					p_wk->strmWritePos=0;
				}
				if( p_wk->FSReadPos >= sp_STRM_WORK->data_siz ){
  					p_wk->FSReadPos=0;
  					if( p_wk->isLoop == FALSE )
  					{
              p_wk->isFinishData = TRUE;
            }
//					GFL_ARC_SeekDataByHandle( p_wk->p_handle, sp_STRM_WORK->seek_top );
				}
			}
		}
	}
	else{
		if(size){
			GFL_ARC_LoadDataOfsByHandle( p_wk->p_handle, p_wk->dataid, p_wk->FSReadPos, size, &p_wk->FS_strmBuffer[0] );
			p_wk->FSReadPos+=size;
		}
		else{
			while(p_wk->strmReadPos!=p_wk->strmWritePos){

        if( sp_STRM_WORK->isFinishData == FALSE )
        {
  				GFL_ARC_LoadDataByHandleContinue( p_wk->p_handle, 32, &p_wk->FS_strmBuffer[p_wk->strmWritePos] );
        }
        else
        {
          GFL_STD_MemClear( &p_wk->FS_strmBuffer[p_wk->strmWritePos] , 32 );
        }
				p_wk->FSReadPos+=32;
				p_wk->strmWritePos+=32;
				if(p_wk->strmWritePos>=STRM_BUF_SIZE){
					p_wk->strmWritePos=0;
				}
				if( p_wk->FSReadPos >= sp_STRM_WORK->data_siz ){
					p_wk->FSReadPos=SWAV_HEAD_SIZE;
					GFL_ARC_SeekDataByHandle( p_wk->p_handle, sp_STRM_WORK->seek_top );
					if( p_wk->isLoop == FALSE )
					{
            p_wk->isFinishData = TRUE;
          }
				}
			}
		}
	}
}
