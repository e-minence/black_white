//=============================================================================================
/**
 * @file	sound.c
 * @brief	�T�E���h���[�`��
 * @author	Hisashi Sogabe
 * @date	2007/04/17
 */
//=============================================================================================
#include "gflib.h"

//=============================================================================================
//	�^�錾
//=============================================================================================

//=============================================================================================
//	�萔�錾
//=============================================================================================
#define	SOUND_HEAP_SIZE		(0x40000)
#define	STOP_FADE_FRAME		(1)				//GFL_SOUND_StopBgm�ł̋Ȃ��Ƃ܂�܂ł�FADE_WAIT
#define	SOUND_SEQARC_NOLOAD	(0xffffffff)
#define	SOUND_HANDLE_MAX	(8)

//=============================================================================================
//	���[�N
//=============================================================================================
static	NNSSndArc			arc;
//static	u8					sndHeap[SOUND_HEAP_SIZE];	//�A�v�����ƂɐÓI�Ɋm�ۂ��Ă��炤�悤�ɕύX�i�T�C�Y���ςɂ��邽�߁j
static	NNSSndHeapHandle	heap;
static	NNSSndHandle		sndHandle[SOUND_HANDLE_MAX];
static	int					GFL_SOUND_seqarc_no=SOUND_SEQARC_NOLOAD;
static	int					cur_bgm_no;

//=============================================================================================
//=============================================================================================
//	�ݒ�֐�
//=============================================================================================
//=============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h������
 *
 * @param	sndHeap		�T�E���h�q�[�v�ւ̃|�C���^�i�ÓI�Ɋm�ۂ��邱�Ƃ𐄏��j
 * @param	sndHeapSize	�T�E���h�q�[�v�̃T�C�Y
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_Init( u8 *sndHeap,int sndHeapSize )
{
	int	i;

	NNS_SndInit();
	heap=NNS_SndHeapCreate(sndHeap,sndHeapSize);

	for(i=0;i<SOUND_HANDLE_MAX;i++){
		NNS_SndHandleInit(&sndHandle[i]);
	}

	arc.file_open=FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h���C������
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_Main( void )
{
	NNS_SndMain();
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�I���i�Ă΂�邱�Ƃ͂Ȃ����낤���A�ꉞ�쐬�j
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_Exit( void )
{
	NNS_SndHeapDestroy( heap );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^���[�h�i�A�[�J�C�u�f�[�^�j
 *
 * @param[in]	arcID		�ǂݍ��ރA�[�J�C�u�t�@�C���̎�ރC���f�b�N�X�i���o�[
 * @param[in]	datID		�ǂݍ��ރf�[�^�̃A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadArchiveData( int arcID, int datID )
{
	BOOL	result;

	if(arc.file_open==TRUE){
		FS_CloseFile(&arc.file);
	}

	GFL_ARC_OpenFileTopPosWrite(arcID,datID,&arc.file);

    arc.file_open = TRUE;
    
    /* �Z�b�g�A�b�v */
    result = NNS_SndArcSetup( &arc, heap, FALSE );

    GF_ASSERT( result );
    if ( ! result ) return;

	NNS_SndArcSetCurrent(&arc);

	NNS_SndArcPlayerSetup(heap);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^���[�h�i�V�[�P���X�f�[�^�j
 *
 * @param	seq_no	�V�[�P���X�f�[�^�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadSequenceData( int seq_no )
{
	NNS_SndArcLoadSeq(seq_no,heap);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^���[�h�i�V�[�P���X�A�[�J�C�u�f�[�^�j
 *
 * @param	seqarc_no	�V�[�P���X�A�[�J�C�u�f�[�^�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadSequenceArchiveData( int seqarc_no )
{
	NNS_SndArcLoadSeqArc(seqarc_no,heap);
	GFL_SOUND_seqarc_no=seqarc_no;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^���[�h�i�o���N�f�[�^�j
 *
 * @param	bank_no	�o���N�f�[�^�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadBankData( int bank_no )
{
	NNS_SndArcLoadBank(bank_no,heap);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^���[�h�i�O���[�v�f�[�^�j
 *
 * @param	group_no	�O���[�v�f�[�^�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadGroupData( int group_no )
{
    const NNSSndArcGroupInfo* info;
    const NNSSndArcGroupItem* item;
    int itemNo;

	NNS_SndArcLoadGroup(group_no,heap);
    info = NNS_SndArcGetGroupInfo( group_no );

    GF_ASSERT ( info!=NULL );
    
    for( itemNo = 0; itemNo < info->count ; itemNo++ )
    {
        item = & info->item[ itemNo ];
        
        switch( item->type ) {
        case NNS_SNDARC_SNDTYPE_SEQARC:
			GFL_SOUND_seqarc_no=item->index;
            break;
        default:
            GF_ASSERT(1);
            break;
        }
    }
}

//--------------------------------------------------------------------------------------------
/**
 * �V�[�P���X�i���o�[����v���C���[�i���o�[���擾
 *
 * @param	no	�V�[�P���X�i���o�[
 *
 * @retval	�v���C���[�i���o�[ or 0xff=�擾���s
 */
//--------------------------------------------------------------------------------------------
u8	GFL_SOUND_GetPlayerNo( u16 no )
{
	const NNSSndSeqParam* param;
	
	if( no == 0 ){
		//GF_ASSERT( (0) && "�V�[�P���X�i���o�[���s���Ȃ̂Ńv���C���[�i���o�[�擾���s�I" );
		return 0xff;	//�G���[
	}

	//�T�E���h�A�[�J�C�u���̊e�V�[�P���X�ɑ΂���A�V�[�P���X�p�����[�^�\���̂��擾
	param = NNS_SndArcGetSeqParam( no );
	//OS_Printf( "�v���C���[�i���o�[ = %d\n", param->playerNo );

	if( param == NULL ){
		//GF_ASSERT( (0) && "�V�[�P���X�i���o�[���s���Ȃ̂Ńv���C���[�i���o�[�擾���s�I" );
		OS_Printf( "�V�[�P���X�i���o�[���s���Ȃ̂Ńv���C���[�i���o�[�擾���s�I\n" );
		return 0xff;	//�G���[
	}

	GF_ASSERT(param->playerNo<SOUND_HANDLE_MAX);

	return param->playerNo;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^�Đ��iBGM�j
 *
 * @param	bgm_no	BGM�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_PlayBGM( int bgm_no )
{
	u8	player_no;

	player_no=GFL_SOUND_GetPlayerNo(bgm_no);
	GF_ASSERT(player_no!=0xff);

	NNS_SndArcPlayerStartSeq(&sndHandle[player_no],bgm_no);
	cur_bgm_no=bgm_no;
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^Stop�iBGM�j
 *
 * �����Ă���BGM���~�߂�
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_StopBGM( void )
{
	u8	player_no;

	player_no=GFL_SOUND_GetPlayerNo(cur_bgm_no);
	GF_ASSERT(player_no!=0xff);

	NNS_SndPlayerStopSeq(&sndHandle[player_no],STOP_FADE_FRAME);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^�Đ��iSE�j
 *
 * @param	se_no	SE�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_PlaySE( int se_no )
{
	u8	player_no;

	player_no=GFL_SOUND_GetPlayerNo(se_no);
	GF_ASSERT(player_no!=0xff);

	NNS_SndArcPlayerStartSeq(&sndHandle[player_no],se_no);
}

//--------------------------------------------------------------
/**
 * @brief	player_no���w�肵�čĐ����̃V�[�P���X���~�߂�
 *
 * @param	player_no	player�i���o�[
 */
//--------------------------------------------------------------
void	GFL_SOUND_StopPlayerNo(u8 player_no)
{
	NNS_SndPlayerStopSeqByPlayerNo( player_no, 0 );
}

//--------------------------------------------------------------
/**
 * @brief	player_no���w�肵�čĐ����̃V�[�P���X�̐���Ԃ�
 *
 * @param	player_no	player�i���o�[
 *
 * @retval	"�Đ����̃V�[�P���X�̐�"
 */
//--------------------------------------------------------------
int		GFL_SOUND_CheckPlaySeq( u8 player_no )
{
	//����̓`�F�b�N���Ă��Ȃ��̂Œ��ӁI
	if( player_no < 0 ){
		GF_ASSERT( (0) && "�������s���ł��I" );
	}

	return NNS_SndPlayerCountPlayingSeqByPlayerNo( player_no );
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�q�[�v�K�w���[�h
 *
 * @param	level	���[�h����K�w���x��
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_LoadHeapState(int level)
{
	NNS_SndHeapLoadState(heap,level);
}

//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�q�[�v�K�w�Z�[�u
 *
 * @retval	�ۑ������K�w���x���i-1�ŃG���[�j
 */
//--------------------------------------------------------------------------------------------
int	GFL_SOUND_SaveHeapState(void)
{
	return NNS_SndHeapSaveState(heap);
}

//--------------------------------------------------------------------------------------------
/**
 * �v���C���[�i���o�[���w�肵�Ĉꎞ��~
 *
 * @param	playerNo	�v���C���[�ԍ�
 * @param	flg			�ꎞ��~�t���O ( TRUE = PAUSE )
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_SOUND_PausePlayerNo( int playerNo, BOOL flg )
{
	NNS_SndPlayerPauseByPlayerNo( playerNo, flg );
}

//--------------------------------------------------------------------------------------------
/**
 * ������ύX
 *
 * @param	playerNo	�v���C���[�ԍ�
 * @param	trbitmask	�g���b�N�r�b�g�}�X�N
 * @param	pitch		�����ω��̒l�i-32768�`32767�j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_SOUND_SetPitch( int playerNo, u16 trbitmask, int pitch )
{
	NNS_SndPlayerSetTrackPitch( &sndHandle[playerNo], trbitmask, pitch );
}

//--------------------------------------------------------------------------------------------
/**
 * ������ύX
 *
 * @param	playerNo	�v���C���[�ԍ�
 * @param	trbitmask	�g���b�N�r�b�g�}�X�N
 * @param	pan			��ʕω��̒l�i-128�`127�j
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void GFL_SOUND_SetPan( int playerNo, u16 trbitmask, int pan )
{
	NNS_SndPlayerSetTrackPan( &sndHandle[playerNo], trbitmask, pan );
}

//SEQARC��p���Ă̍Đ��i����͖��T�|�[�g�j
#if 0
//--------------------------------------------------------------------------------------------
/**
 * �T�E���h�f�[�^�Đ��iSE�j
 *
 * @param	se_no	SE�i���o�[
 */
//--------------------------------------------------------------------------------------------
void	GFL_SOUND_PlaySE( int se_no )
{
	GF_ASSERT(GFL_SOUND_seqarc_no!=SOUND_SEQARC_NOLOAD);
	NNS_SndArcPlayerStartSeqArc(&seHandle,GFL_SOUND_seqarc_no,se_no);
}
#endif

