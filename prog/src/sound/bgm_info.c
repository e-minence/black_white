#include <gflib.h>
#include "bgm_info.h"
#include "arc/arc_def.h"
#include "arc/bgm_info.naix"
#include "../../../resource/sound/bgm_info/iss_type.h"


//=============================================================================================
/**
 *
 * @brief �萔
 * 
 */
//=============================================================================================
#define ARC_ID      (ARCID_BGM_INFO)				// BGM���̃A�[�J�C�u�E�C���f�b�N�X
#define ARC_DATA_ID (NARC_bgm_info_bgm_info_bin)	// �A�[�J�C�u���f�[�^�E�C���f�b�N�X


//=============================================================================================
/**
 *
 * @brief BGM���\����
 * 
 */
//=============================================================================================
struct _BGM_INFO_SYS_WORK
{
	// �f�[�^��
	u8 dataNum;		

	// �ȉ�, �v�f��dataNum�̔z��
	u16* seqIndex;		// �V�[�P���X�ԍ�
	u8*  issType;		// ISS�^�C�v
};


//=============================================================================================
/**
 *
 * @brief ����J�֐��̃v���g�^�C�v�錾
 * 
 */
//=============================================================================================
// BGM����ǂݍ���
void LoadInfo( BGM_INFO_SYS* p_sys, HEAPID heap_id );


//=============================================================================================
/**
 *
 * @brief �V�X�e���쐬�E�j���֐�
 *
 */
//=============================================================================================

//----------------------------------------------------------------------------
/**
 * @brief BGM���Ǘ��V�X�e�����쐬����
 *
 * @param heap_id �q�[�vID
 *
 * @return �V�X�e�����[�N�ւ̃|�C���^
 */
//----------------------------------------------------------------------------
BGM_INFO_SYS* BGM_INFO_CreateSystem( HEAPID heap_id )
{
	BGM_INFO_SYS* p_sys;

	// �V�X�e�����[�N���m��
	p_sys = (BGM_INFO_SYS*)GFL_HEAP_AllocMemory( heap_id, sizeof( BGM_INFO_SYS ) );

	// �f�[�^�ǂݍ���
	LoadInfo( p_sys, heap_id );

	// �쐬�����V�X�e�����[�N��Ԃ�
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 * @brief BGM���Ǘ��V�X�e����j������
 *
 * @param p_sys �j������V�X�e��
 */
//----------------------------------------------------------------------------
void BGM_INFO_DeleteSystem( BGM_INFO_SYS* p_sys )
{
	// �V�X�e�������f�[�^��j��
	GFL_HEAP_FreeMemory( p_sys->seqIndex );
	GFL_HEAP_FreeMemory( p_sys->issType );
	
	// �{�̂�j��
	GFL_HEAP_FreeMemory( p_sys );

	// DEBUG:
	/*
	OBATA_Printf( "-----------------free size = %d\n", sizeof( u16 ) * p_sys->dataNum );
	OBATA_Printf( "-----------------free size = %d\n", sizeof( u8 ) * p_sys->dataNum );
	OBATA_Printf( "-----------------free size = %d\n", sizeof( BGM_INFO_SYS ) );
	*/
}


//====================================================================================
/**
 *
 * @brief �f�[�^�擾�֐�
 *
 */
//====================================================================================

//----------------------------------------------------------------------------
/**
 * @brief �w�肵��BGM��ISS�^�C�v���擾����
 * 
 * @param p_sys   �擾�Ώۂ̃V�X�e��
 * @param seq_idx BGM�̃V�[�P���X�E�C���f�b�N�X( SEQ_XXXX )
 *
 * @return �w��BGM��ISS�^�C�v( �XISS, ���HISS �Ȃ� )
 */
//----------------------------------------------------------------------------
u8 BGM_INFO_GetIssType( BGM_INFO_SYS* p_sys, u16 seq_idx )
{
	int i;

	// �V�[�P���X�ԍ����L�[�Ɍ���
	for( i=0; i<p_sys->dataNum; i++ )
	{
		// �w�肳�ꂽ�V�[�P���X�ԍ�����������, ����ISS�^�C�v��Ԃ�
		if( p_sys->seqIndex[i] == seq_idx )
		{
			return p_sys->issType[i];
		}
	}

	// �w��V�[�P���X�ԍ����e�[�u���ɓo�^����Ă��Ȃ��ꍇ, �ʏ�BGM�Ƃ���
	return ISS_TYPE_NORMAL;
}


//=============================================================================================
/**
 *
 * @brief ����J�֐��̎���
 * 
 */
//=============================================================================================

//----------------------------------------------------------------------------
/**
 * @brief BGM����ǂݍ���
 *
 * @param p_sys   �ǂݍ��񂾃f�[�^�̊i�[��V�X�e��
 * @param heap_id �g�p����q�[�vID
 *
 * [�o�C�i���E�f�[�^�̃t�H�[�}�b�g]
 * [0] �f�[�^��
 * [1] �V�[�P���X�ԍ�
 * [2]
 * [3] ISS�^�C�v
 * �ȉ��A[1]-[3]���f�[�^�̐���������
 */
//---------------------------------------------------------------------------- 
void LoadInfo( BGM_INFO_SYS* p_sys, HEAPID heap_id )
{
	int i;
	int pos = 0;

	// �f�[�^�����擾
	GFL_ARC_LoadDataOfs( &p_sys->dataNum, ARC_ID, ARC_DATA_ID, pos, sizeof( u8 ) );
	pos += 1;

	// �e��f�[�^�̔z����m��
	p_sys->seqIndex = (u16*)GFL_HEAP_AllocMemory( heap_id, sizeof( u16 ) * p_sys->dataNum );
	p_sys->issType  = ( u8*)GFL_HEAP_AllocMemory( heap_id, sizeof(  u8 ) * p_sys->dataNum );

	// �e��f�[�^���擾
	for( i=0; i<p_sys->dataNum; i++ )
	{
		// �V�[�P���X�ԍ�
		GFL_ARC_LoadDataOfs( &p_sys->seqIndex[i], ARC_ID, ARC_DATA_ID, pos, sizeof( u16 ) );
		pos += 2;

		// ISS�^�C�v
		GFL_ARC_LoadDataOfs( &p_sys->issType[i], ARC_ID, ARC_DATA_ID, pos, sizeof( u8 ) );
		pos += 1;
	}

	// DEBUG:
	/*
	OBATA_Printf( "--------------------------------------\n" );
	OBATA_Printf( "dataNum = %d\n", p_sys->dataNum );
	for( i=0; i<p_sys->dataNum; i++ )
	{
		OBATA_Printf( "[%d]: seq=%d, iss=%d\n", i, p_sys->seqIndex[i], p_sys->issType[i] );
	}
	*/
}
