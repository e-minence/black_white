//======================================================================
/**
 * @file	eventwork.c
 * @brief	�Z�[�u�t���O�A���[�N����
 * @author	Tamada
 * @author	Satoshi Nohara
 * @date	05.10.22
 */
//======================================================================
#include "eventwork.h"
#include "eventwork_def.h"

static u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no );
//======================================================================
//	��`
//======================================================================
enum {
  //  �S�Z�[�u�t���O��L���[�N�T�C�Y
  EVENT_FLAG_AREA_MAX =  ( MATH_ROUNDUP(FLAGS_TOTAL_MAX, 8) / 8),

  //  �S�t���O�G���A��L�t���O�T�C�Y
  EVENT_WORK_AREA_MAX = WK_AREA_END - WORKS_TOTAL_MIN,

};

SDK_COMPILER_ASSERT( EVENT_WORK_AREA_MAX < 320 );
SDK_COMPILER_ASSERT( FLAGS_TOTAL_MAX % 8 == 0 );
SDK_COMPILER_ASSERT( EVENT_FLAG_AREA_MAX <= 364 );

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�\���̂̒�`
 */
//------------------------------------------------------------------
struct _EVENTWORK {
	u16 work[EVENT_WORK_AREA_MAX];	//���[�N
	u8	flag[EVENT_FLAG_AREA_MAX];	//�t���O
  u8  time_request;
};

//------------------------------------------------------------------
/**
 * @brief �Z�[�u���Ȃ��t���O
 */
//------------------------------------------------------------------
static u8 EventCtrlFlag[CTRLFLAG_AREA_MAX] = {};

//�V�X�e���t���O�͈͓��`�F�b�N
#define CHECK_SYS_FLAG_RANGE(flag) \
  GF_ASSERT_MSG(flag>=SYS_FLAG_AREA_START && flag<=SYS_FLAG_AREA_END,"ERROR SYS FLAG OUTRANGE")

//======================================================================
//  proto
//======================================================================

//======================================================================
//	EVENTWORK
//======================================================================
//------------------------------------------------------------------
//
//------------------------------------------------------------------
int EVENTWORK_GetWorkSize(void)
{
	return sizeof(EVENTWORK);
}

void EVENTWORK_InitWork( u8 *work )
{
  GFL_STD_MemClear( work, sizeof(EVENTWORK) );
}

//======================================================================
//
//    �t���O����֘A
//
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O���`�F�b�N����
 *
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @retval	"1 = �t���OON"
 * @retval	"0 = �t���OOFF"
 */
//------------------------------------------------------------------
BOOL EVENTWORK_CheckEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p != NULL ){
		if( *p & ( 1 << (flag_no % 8) ) ){ return 1; }
	}
	return 0;
}


//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O���Z�b�g����
 *
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @return	none
 */
//------------------------------------------------------------------
void EVENTWORK_SetEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p == NULL ){ return; }
	*p |= 1 << ( flag_no % 8 );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return;
}


//------------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O�����Z�b�g����
 *
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 *
 * @return	none
 */
//------------------------------------------------------------------
void EVENTWORK_ResetEventFlag( EVENTWORK * ev, u16 flag_no)
{
	u8 * p = EVENTWORK_GetEventFlagAdrs( ev, flag_no );
	if( p == NULL ){ return; }
	*p &= ( 0xff ^ ( 1 << (flag_no % 8) ) );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return;
}

//--------------------------------------------------------------
/**
 * @brief �C�x���g�t���O�̗̈�N���A
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	flag_no		�t���O�i���o�[
 * 
 * @noto
 * �x�^�ȋL�q�Ȃ̂ŏ������x�̖�肪���邩������Ȃ�
 */
//--------------------------------------------------------------
void EVENTWORK_ClearEventFlags( EVENTWORK * ev, u16 start_no, u16 end_no )
{
  int no;

  GF_ASSERT( start_no < end_no );
  GF_ASSERT( ( end_no < SCFLG_START ) || ( SCFLG_START <= start_no ) );

  for ( no = start_no ; no <= end_no; no ++ )
  {
    EVENTWORK_ResetEventFlag( ev, no );
  }
}

//--------------------------------------------------------------
/**
 * @brief	�C�x���g�t���O�̃A�h���X���擾����
 *
 * @param	flag_no		�t���O�i���o�[
 *
 * @retval	"NULL != �t���O�A�h���X"
 * @retval	"NULL = ����`"
 */
//--------------------------------------------------------------
static u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no )
{
	if( flag_no == 0 ){
		return NULL;
	}else if( flag_no < SCFLG_START ){
		if( (flag_no / 8) >= EVENT_FLAG_AREA_MAX ){
			GF_ASSERT_MSG(0, "�t���O�i���o�[(0x%04x)���ő吔�𒴂��Ă��܂��I\n", flag_no );
      return NULL;
		}
		return &ev->flag[flag_no/8];
	}else{

		if( ((flag_no - SCFLG_START) / 8) >= CTRLFLAG_AREA_MAX ){
			GF_ASSERT_MSG(0, "�t���O�i���o�[(0x%04x)���ő吔�𒴂��Ă��܂��I\n", flag_no );
      return NULL;
		}
		return &( EventCtrlFlag[ (flag_no - SCFLG_START) / 8 ] );
	}
}

//======================================================================
//
//
//    ���[�N����֘A
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�A�h���X���擾
 *
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	work_no		���[�N�i���o�[
 *
 * @return	"���[�N�̃A�h���X"
 *
 * @li	work_no < 0x8000	�ʏ�̃Z�[�u���[�N
 * @li	work_no >= 0x8000	�X�N���v�g���䃏�[�N�̒��Ɋm�ۂ��Ă��郏�[�N
 */
//--------------------------------------------------------------
u16 * EVENTWORK_GetEventWorkAdrs( EVENTWORK * ev, u16 work_no )
{
	if( (work_no - SVWK_START) >= EVENT_WORK_AREA_MAX ){
		GF_ASSERT_MSG( 0, "���[�N�i���o�[(0x%04x)���ő吔�𒴂��Ă��܂��I\n", work_no );
	}
	
	return &ev->work[ work_no - SVWK_START ];
}

//--------------------------------------------------------------
/**
 * @brief
 * @param	ev			�C�x���g���[�N�ւ̃|�C���^
 * @param	work_no		���[�N�i���o�[
 */
//--------------------------------------------------------------
void EVENTWORK_ClearEventWorks( EVENTWORK * ev, u16 start_no, u16 end_no )
{
  int no;
  GF_ASSERT( start_no < end_no );
  GF_ASSERT( SVWK_START <= start_no );
  GF_ASSERT( end_no < SVWK_START + EVENT_WORK_AREA_MAX );

  GFL_STD_MemClear(
      EVENTWORK_GetEventWorkAdrs( ev, start_no ),
      sizeof(u16) * (end_no - start_no) );
}



//======================================================================
//
//
//
//======================================================================
//--------------------------------------------------------------
///    ���ԃt���O�̃N���A
//--------------------------------------------------------------
void EVENTWORK_ClearTimeFlags( EVENTWORK * ev )
{
  EVENTWORK_ClearEventFlags( ev, TMFLG_AREA_START, TMFLG_AREA_END );
  ev->time_request = TRUE;
}
//--------------------------------------------------------------
///    ���ԍX�V�������N�G�X�g�̎��o��
//    ������A�B���A�C�e�������p
//--------------------------------------------------------------
BOOL EVENTWORK_PopTimeRequest( EVENTWORK * ev )
{
  BOOL result = ev->time_request;
  ev->time_request = FALSE;
  return result;
}

