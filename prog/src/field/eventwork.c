//======================================================================
/**
 * @file	eventflag.c
 * @bfief	�Z�[�u�t���O�A���[�N����
 * @author	Tamada
 * @author	Satoshi Nohara
 * @date	05.10.22
 */
//======================================================================
#include "eventwork.h"
#include "eventwork_def.h"
//#include "field_script.h"

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

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�𐶐�����
 *
 * @param	heapID		�q�[�vID
 *
 * @return	EVENTWORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
EVENTWORK * EVENTWORK_AllocWork( HEAPID heapID )
{
	EVENTWORK * event;
	event = GFL_HEAP_AllocMemory(heapID, sizeof(EVENTWORK));

	EVENTWORK_Init(event);

	return event;
}

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

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���[�N�̏�����
 * @param	evwk	EVENTWORK�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVENTWORK_Init(EVENTWORK * evwk)
{
	//�s���Ȓl�������Ă���̂ŃN���A
	memset( evwk, 0, sizeof(EVENTWORK) );
#if 0
	SVLD_SetCrc(GMDATA_ID_EVENT_WORK);
#endif //CRC_LOADCHECK
}

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
u8 * EVENTWORK_GetEventFlagAdrs( EVENTWORK * ev, u16 flag_no )
{
	if( flag_no == 0 ){
		return NULL;
	}else if( flag_no < SCFLG_START ){
		if( (flag_no / 8) >= EVENT_FLAG_AREA_MAX ){
			GF_ASSERT( (0) && "�t���O�i���o�[���ő吔�𒴂��Ă��܂��I" );
		}

		return &ev->flag[flag_no/8];
	}else{

		if( ((flag_no - SCFLG_START) / 8) >= CTRLFLAG_AREA_MAX ){
			GF_ASSERT( (0) && "�t���O�i���o�[���ő吔�𒴂��Ă��܂��I" );
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
		GF_ASSERT( (0) && "���[�N�i���o�[���ő吔�𒴂��Ă��܂��I" );
	}
	
	return &ev->work[ work_no - SVWK_START ];
}
