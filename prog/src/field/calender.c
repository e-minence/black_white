//////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �J�����_�[���擾�V�X�e��
 * @author obata
 * @date   2009.0805 
 *
 */
////////////////////////////////////////////////////////////////////////////// 
#include "calender.h" 


//=============================================================================
/**
 * @brief �J�����_�[�E�V�X�e���E���[�N
 */
//=============================================================================
struct _CALENDER
{
  CALENDER_ENCOUNT_INFO* pEncountInfo;  // �o�����ԍ��f�[�^
  CALENDER_HATCH_INFO*   pHatchInfo;    // �z���ω��ԍ��f�[�^
  CALENDER_WEATHER_INFO* pWeatherInfo;  // �V�C�f�[�^
};


//=============================================================================
/**
 * @brief �V�X�e���̍쐬�E�j��
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �J�����_�[�V�X�e�����쐬����
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����J�����_�[
 */
//-----------------------------------------------------------------------------
CALENDER* CALENDER_Create( HEAPID heap_id )
{
  CALENDER* p_sys;

  // �V�X�e�����[�N�쐬
  p_sys = GFL_HEAP_AllocMemory( heap_id, sizeof( CALENDER ) );

  // �e��f�[�^��ǂݍ���
  p_sys->pEncountInfo = CALENDER_ENCOUNT_INFO_Create( heap_id );
  p_sys->pHatchInfo   = CALENDER_HATCH_INFO_Create( heap_id );
  p_sys->pWeatherInfo = CALENDER_WEATHER_INFO_Create( heap_id );

  // �쐬�����J�����_�[��Ԃ�
  return p_sys;
}

//-----------------------------------------------------------------------------
/**
 * @brief �J�����_�[�V�X�e����j������
 *
 * @param p_sys �j������J�����_�[
 */
//-----------------------------------------------------------------------------
void CALENDER_Delete( CALENDER* p_sys )
{
  // �e��f�[�^��j��
  CALENDER_ENCOUNT_INFO_Delete( p_sys->pEncountInfo );
  CALENDER_HATCH_INFO_Delete( p_sys->pHatchInfo );
  CALENDER_WEATHER_INFO_Delete( p_sys->pWeatherInfo );

  // �{�̂�j��
  GFL_HEAP_FreeMemory( p_sys );
}


//=============================================================================
/**
 * @brief �J�����_�[�ւ̖₢���킹
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 * @brief �w����ɂ�����, �w��]�[���̓V�C���擾����
 *
 * @param p_sys   �J�����_�[
 * @param p_date  ���t�w��
 * @param zone_id �]�[���w��
 *
 * @return �V�C�ԍ�( WEATHER_NO_XXXX )
 *         �w��]�[�����o�^����Ă��Ȃ��ꍇ, �����l WEATHER_NO_NONE ���Ԃ�
 */
//----------------------------------------------------------------------------- 
u16 CALENDER_GetWeather( CALENDER* p_sys, const RTCDate* p_date, u16 zone_id )
{
  return CALENDER_WEATHER_INFO_GetWeather( p_sys->pWeatherInfo, p_date, zone_id );
}

//-----------------------------------------------------------------------------
/**
 * @breif �w����̏o�����ω��ԍ����擾����
 *
 * @param p_sys  �J�����_�[
 * @param p_date ���t�w��
 *
 * @return �o�����ω��ԍ�( ENCOUNT_NO_x )
 */
//-----------------------------------------------------------------------------
u8 CALENDER_GetEncountNo( CALENDER* p_sys, const RTCDate* p_date )
{
  return CALENDER_ENCOUNT_INFO_GetEncountNo( p_sys->pEncountInfo, p_date );
}

//-----------------------------------------------------------------------------
/**
 * @brief �w����̛z���ω��ԍ����擾����
 *
 * @param p_sys  �J�����_�[
 * @param p_date ���t�w��
 *
 * @return �z���ω��ԍ�( HATCH_NO_x )
 */
//-----------------------------------------------------------------------------
u8 CALENDER_GetHatchNo( CALENDER* p_sys, const RTCDate* p_date )
{
  return CALENDER_HATCH_INFO_GetHatchNo( p_sys->pHatchInfo, p_date );
}
