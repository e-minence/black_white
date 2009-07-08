////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   field_place_name.c
 * @brief  �n���\���E�B���h�E
 * @author obata_toshihiro
 * @date   2009.07.08
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "field_place_name.h"


//===================================================================================
/**
 * �萔
 */
//===================================================================================

//------------------
// �e��Ԃ̓��쎞��
//------------------
#define PROCESS_TIME_APPEAR    (10)	// �\�����
#define PROCESS_TIME_WAIT      (60) // �ҋ@���
#define PROCESS_TIME_DISAPPEAR (10) // �ޏo���

//-----------------
// �V�X�e���̏��
//-----------------
typedef enum
{
	FIELD_PLACE_NAME_STATE_HIDE,		// ��\��
	FIELD_PLACE_NAME_STATE_APPEAR,		// �o����
	FIELD_PLACE_NAME_STATE_WAIT,		// �ҋ@(�\��)��
	FIELD_PLACE_NAME_STATE_DISAPPEAR,	// �ޏo��
	FIELD_PLACE_NAME_STATE_MAX,
}
FIELD_PLACE_NAME_STATE;



//===================================================================================
/**
 * �V�X�e���E���[�N
 */
//===================================================================================
struct _FIELD_PLACE_NAME
{ 
	FIELD_PLACE_NAME_STATE state;			// ���
	u16	                   stateCount;		// ��ԃJ�E���^
	u32	                   currentZoneID;	// ���ݕ\�����̒n���ɑΉ�����]�[��ID
	u32                    nextZoneID;		// ���ɕ\������n���ɑΉ�����]�[��ID
};


//===================================================================================
/**
 * ����J�֐��̃v���g�^�C�v�錾
 */
//===================================================================================
// ��Ԃ�ύX����
static void SetState( FIELD_PLACE_NAME* p_sys, FIELD_PLACE_NAME_STATE next_state );
// �e��Ԏ��̓���
static void Process_HIDE( FIELD_PLACE_NAME* p_sys );
static void Process_APPEAR( FIELD_PLACE_NAME* p_sys );
static void Process_WAIT( FIELD_PLACE_NAME* p_sys );
static void Process_DISAPPEAR( FIELD_PLACE_NAME* p_sys );


//===================================================================================
/**
 * ���J�֐��̎���( �V�X�e���̉ғ��Ɋւ���֐� )
 */
//===================================================================================
//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e�����쐬����
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �n���\���V�X�e���E���[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------------------------
FIELD_PLACE_NAME* FIELD_PLACE_NAME_Create( HEAPID heap_id )
{
	FIELD_PLACE_NAME* p_sys;

	// �V�X�e���E���[�N�쐬
	p_sys = (FIELD_PLACE_NAME*)GFL_HEAP_AllocClearMemory( heap_id, sizeof( FIELD_PLACE_NAME ) );

	// �쐬�����V�X�e����Ԃ�
	return p_sys;
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e���̓��쏈��
 *
 * @param p_sys �������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Process( FIELD_PLACE_NAME* p_sys )
{
	// ��ԃJ�E���^���X�V
	p_sys->stateCount++;

	// ��Ԃɉ�����������s��
	switch( p_sys->state )
	{
		case FIELD_PLACE_NAME_STATE_HIDE:		Process_HIDE( p_sys );		break;
		case FIELD_PLACE_NAME_STATE_APPEAR:		Process_APPEAR( p_sys );	break;
		case FIELD_PLACE_NAME_STATE_WAIT:		Process_WAIT( p_sys );		break;
		case FIELD_PLACE_NAME_STATE_DISAPPEAR:	Process_DISAPPEAR( p_sys );	break;
	}
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���E�B���h�E�̕`�揈��
 *
 * @param p_sys �`��ΏۃV�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Draw( FIELD_PLACE_NAME* p_sys )
{
	char* state;
	switch( p_sys->state )
	{
		case FIELD_PLACE_NAME_STATE_HIDE:		state = "HIDE";	break;
		case FIELD_PLACE_NAME_STATE_APPEAR:		state = "APPEAR";	break;
		case FIELD_PLACE_NAME_STATE_WAIT:		state = "WAIT";	break;
		case FIELD_PLACE_NAME_STATE_DISAPPEAR:	state = "DISAPPER";	break;
		default:								state = "error"; break;
	}
	OBATA_Printf( "state = %s,  stateCount = %d\n", state, p_sys->stateCount );
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���\���V�X�e����j������
 *
 * @param p_sys �j������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Delete( FIELD_PLACE_NAME* p_sys )
{
	// �V�X�e���E���[�N���
	GFL_HEAP_FreeMemory( p_sys );
} 


//===================================================================================
/**
 * ���J�֐��̎���( �V�X�e���̐���Ɋւ���֐� )
 */
//===================================================================================

//------------------------------------------------------------------------------------
/**
 * @brief �]�[���̐؂�ւ���ʒm��, �V�����n����\������
 *
 * @param p_sys        �]�[���؂�ւ���ʒm����V�X�e��
 * @param next_zone_id �V�����]�[��ID
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_ZoneChange( FIELD_PLACE_NAME* p_sys, u32 next_zone_id )
{
	// �w�肳�ꂽ�]�[��ID�����ɕ\�����ׂ����̂Ƃ��ċL��
	p_sys->nextZoneID = next_zone_id;

	// �o���� or �ҋ@�� �Ȃ�, �����I�ɑޏo������
	if( ( p_sys->state == FIELD_PLACE_NAME_STATE_APPEAR ) ||
		( p_sys->state == FIELD_PLACE_NAME_STATE_WAIT ) )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_DISAPPEAR );
	}
}

//------------------------------------------------------------------------------------
/**
 * @brief �n���E�B���h�E�̕\���������I�ɏI������
 *
 * @param p_sys �\�����I������V�X�e��
 */
//------------------------------------------------------------------------------------
void FIELD_PLACE_NAME_Hide( FIELD_PLACE_NAME* p_sys )
{
	SetState( p_sys, FIELD_PLACE_NAME_STATE_HIDE );
}



//===================================================================================
/**
 * ����J�֐��̎���
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief ��Ԃ�ύX����
 *
 * @param p_sys      ��Ԃ�ύX����V�X�e��
 * @param next_state �ݒ肷����
 */
//-----------------------------------------------------------------------------------
static void SetState( FIELD_PLACE_NAME* p_sys, FIELD_PLACE_NAME_STATE next_state )
{
	// ��Ԃ�ύX��, ��ԃJ�E���^������������
	p_sys->state      = next_state;
	p_sys->stateCount = 0;

	// �o����ԂɈڍs���鎞��, �\�����̃]�[��ID���X�V����
	if( next_state == FIELD_PLACE_NAME_STATE_APPEAR )
	{
		p_sys->currentZoneID = p_sys->nextZoneID;
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief ��\�����( FIELD_PLACE_NAME_STATE_HIDE )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_HIDE( FIELD_PLACE_NAME* p_sys )
{
	// �]�[���̐؂�ւ��ʒm���󂯂Ă�����, �o����ԂɈڍs����
	if( p_sys->currentZoneID != p_sys->nextZoneID )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_APPEAR );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �o�����( FIELD_PLACE_NAME_STATE_APPEAR )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_APPEAR( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, �ҋ@��Ԃֈڍs����
	if( PROCESS_TIME_APPEAR < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_WAIT );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ҋ@( FIELD_PLACE_NAME_STATE_WAIT )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_WAIT( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, �ޏo��Ԃֈڍs����
	if( PROCESS_TIME_WAIT < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_DISAPPEAR );
	}
}

//-----------------------------------------------------------------------------------
/**
 * @brief �ޏo���( FIELD_PLACE_NAME_STATE_DISAPPEAR )���̓���
 *
 * @param p_sys �������V�X�e��
 */
//-----------------------------------------------------------------------------------
static void Process_DISAPPEAR( FIELD_PLACE_NAME* p_sys )
{
	// ��莞�Ԃ��o�߂�����, ��\����Ԃֈڍs����
	if( PROCESS_TIME_DISAPPEAR < p_sys->stateCount )
	{
		SetState( p_sys, FIELD_PLACE_NAME_STATE_HIDE );
	}
}
