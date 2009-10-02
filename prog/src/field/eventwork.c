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
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�\���̂̒�`
 */
//------------------------------------------------------------------
struct _EVENTWORK {
	u16 work[EVENT_WORK_AREA_MAX];	//���[�N
	u8	flag[EVENT_FLAG_AREA_MAX];	//�t���O
};

//�Z�[�u���Ȃ��t���O
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

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�𐶐�����
 *
 * @param	heapID		�q�[�vID
 *
 * @return	EVENTWORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
#if 0
EVENTWORK * EVENTWORK_AllocWork( HEAPID heapID )
{
	EVENTWORK * event;
	event = GFL_HEAP_AllocMemory(heapID, sizeof(EVENTWORK));

	EVENTWORK_Init(event);

	return event;
}
#endif

#if 0
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N���J������
 *
 * @param	evwk 	EVENTWORK
 *
 * @return	nothing
 */
//------------------------------------------------------------------
void EVENTWORK_FreeWork( EVENTWORK *evwk )
{
	GFL_HEAP_FreeMemory( evwk );
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̏�����
 * @param	evwk	EVENTWORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
#if 0
void EVENTWORK_Init(EVENTWORK * evwk)
{
	//�s���Ȓl�������Ă���̂ŃN���A
	memset( evwk, 0, sizeof(EVENTWORK) );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
}
#endif

//------------------------------------------------------------------
/**
 * @brief	�Z�[�u�f�[�^����C�x���g���[�N�ւ̃|�C���^���擾����
 *
 * @param	sv	�Z�[�u�f�[�^�ւ̃|�C���^
 *
 * @return	�Z�[�u�f�[�^����EVENTWORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
#if 0
EVENTWORK * SaveData_GetEventWork(SAVEDATA * sv)
{
	EVENTWORK * event;
	event = (EVENTWORK *)SaveData_Get(sv, GMDATA_ID_EVENT_WORK);
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_EVENT_WORK)
	SVLD_CheckCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
	return event;
}
#endif

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

//======================================================================
//  �C�x���g���[�N�@�V�X�e���t���O�֘A
//======================================================================
#if 0
//--------------------------------------------------------------
/**
 * �V�X�e���t���O�@�Z�b�g
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval nothing
 */
//--------------------------------------------------------------
static void sysflag_Set( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  EVENTWORK_SetEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * �V�X�e���t���O�@���Z�b�g
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval nothing
 */
//--------------------------------------------------------------
static void sysflag_Reset( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  EVENTWORK_ResetEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * �V�X�e���t���O�@�`�F�b�N
 * @param ev EVENT_WORK
 * @param flag SYSTEM_FLAG
 * @retval BOOL TRUE=flag on
 */
//--------------------------------------------------------------
static BOOL sysflag_Check( EVENTWORK *ev, u16 flag )
{
  CHECK_SYS_FLAG_RANGE( flag );
  return EVENTWORK_CheckEventFlag( ev, flag );
}

//--------------------------------------------------------------
/**
 * �V�X�e���t���O�@���ʏ���
 * @param ev  EVENTWORK
 * @param mode SYS_FLAG_MODE
 * @param flag_id
 * @retval BOOL SYS_FLAG_MODE_CHECK���ATRUE��flag on
 */
//--------------------------------------------------------------
static BOOL sysflag_Common( EVENTWORK *ev, SYS_FLAG_MODE mode, u16 flag )
{
  switch( mode ){
  case SYS_FLAG_MODE_SET:
    sysflag_Set( ev, flag );
    break;
  case SYS_FLAG_MODE_RESET:
    sysflag_Reset( ev, flag );
    break;
  case SYS_FLAG_MODE_CHECK:
    return sysflag_Check( ev, flag );
  default:
 		GF_ASSERT_MSG( 0,"FLAG %d, NOT DEFINED MODE %d\n", flag, mode );
  }
  return FALSE;
}

//======================================================================
//  �V�X�e���t���O�@��`�Z�֘A
//======================================================================
//--------------------------------------------------------------
/**
 * �V�X�e���t���O�@��`�Z�@�����肫
 * @param ev EVENTWORK
 * @param mode SYS_FLAG_MODE
 * @retval BOOL
 */
//--------------------------------------------------------------
BOOL EVENTWORK_SYS_FLAG_Kairiki( EVENTWORK *ev, SYS_FLAG_MODE mode )
{
  return sysflag_Common( ev, mode, SYS_FLAG_KAIRIKI );
}
#endif //#if 0
