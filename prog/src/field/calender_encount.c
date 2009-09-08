#include "calender_encount.h"
#include "arc/arc_def.h"
#include "arc/calender.naix"


//==================================================================================
/**
 * @brief �V�X�e�����[�N
 */
//==================================================================================
struct _CALENDER_ENCOUNT_INFO
{
  u16 dataNum;    // �f�[�^��
  u8* month;      // ���z��
  u8* day;        // ���z��
  u8* encountNo;  // �o�����ԍ��z��
};


//==================================================================================
/**
 * @brief ����J�֐��̃v���g�^�C�v�錾
 */
//==================================================================================

// �w�肵��2�o�C�g�����g���G���f�B�A���Ƃ��ĉ��߂�, �l���擾����
static u16 GetU16( u8* data, int pos );

// �f�[�^��ǂݍ���
static void LoadData( CALENDER_ENCOUNT_INFO* p_info, HEAPID heap_id );


//==================================================================================
/**
 * @brief ���J�֐��̒�`
 */
//================================================================================== 

//---------------------------------------------------------------------------------
/**
 * @brief �G���J�E���g�E�f�[�^���쐬����
 *
 * @param heap_id �g�p����q�[�vID
 *
 * @return �쐬�����G���J�E���g�E�f�[�^
 */
//---------------------------------------------------------------------------------
CALENDER_ENCOUNT_INFO* CALENDER_ENCOUNT_INFO_Create( HEAPID heap_id )
{
  CALENDER_ENCOUNT_INFO* p_info;

  // ���[�N���쐬
  p_info = (CALENDER_ENCOUNT_INFO*)GFL_HEAP_AllocMemory( heap_id, sizeof( CALENDER_ENCOUNT_INFO ) );

  // �f�[�^��ǂݍ���
  LoadData( p_info, heap_id );

  // �쐬�����f�[�^��Ԃ�
  return p_info;
}

//---------------------------------------------------------------------------------
/**
 * @brief �w����̏o�����ω��ԍ����擾����
 *
 * @param p_info �f�[�^
 * @parma p_date ���t
 *
 * @return �w�肵�����t�̏o�����ω��ԍ�
 */
//---------------------------------------------------------------------------------
u8 CALENDER_ENCOUNT_INFO_GetEncountNo( CALENDER_ENCOUNT_INFO* p_info, const RTCDate* p_date )
{
  int i;
  u8 encount_no = ENCOUNT_NO_0;

  // �w���������
  for( i=0; i<p_info->dataNum; i++ )
  {
    // ����
    if( ( p_info->month[i] == p_date->month ) &&
        ( p_info->day[i]   == p_date->day ) )
    { 
      encount_no = p_info->encountNo[i];
      break;
    }
  }

  // �s���l��r��
  if( ENCOUNT_NO_MAX <= encount_no )
  {
    OBATA_Printf( "-----------------------------------\n" );
    OBATA_Printf( "�s���ȏo�����ω��ԍ�(%d)�𔭌�!!\n", encount_no );
    OBATA_Printf( "-----------------------------------\n" );
    encount_no = ENCOUNT_NO_0;
  }

  // DEBUG:
  OBATA_Printf( "%d/%d : encountNo = %d\n", p_date->month, p_date->day, encount_no );

  // �w����̃f�[�^�����݂��Ȃ� ==> �f�t�H���g�l0�Ԃ�Ԃ�
  return encount_no;
}

//---------------------------------------------------------------------------------
/**
 * @brief �G���J�E���g�E�f�[�^��j������
 *
 * @param p_info �j������f�[�^
 */ 
//---------------------------------------------------------------------------------
void CALENDER_ENCOUNT_INFO_Delete( CALENDER_ENCOUNT_INFO* p_info )
{
  // �e�z���j��
  GFL_HEAP_FreeMemory( p_info->month );
  GFL_HEAP_FreeMemory( p_info->day );
  GFL_HEAP_FreeMemory( p_info->encountNo );

  // �{�̂�j��
  GFL_HEAP_FreeMemory( p_info );
}


//==================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//==================================================================================

//---------------------------------------------------------------------------- 
/**
 * @brief data + pos �̈ʒu����n�܂�2�o�C�g��, 
 *        ���g���G���f�B�A���ŕ���u16�̃f�[�^�Ƃ��ĉ��߂��l���擾����
 *
 * @param data �f�[�^�J�n�ʒu
 * @param pos  �J�n�ʒu�I�t�Z�b�g
 *
 * @return u16
 */
//---------------------------------------------------------------------------- 
static u16 GetU16( u8* data, int pos )
{
	u16 lower = (u16)( data[ pos ] );
	u16 upper = (u16)( data[ pos + 1 ] );
	u16 value = ( upper << 8 ) | lower;

	return value;
}

//---------------------------------------------------------------------------------
/**
 * @brief �f�[�^��ǂݍ���
 *
 * @param p_info  �ǂݍ��񂾃f�[�^�̊i�[��
 * @param heap_id �g�p����q�[�vID
 */
//---------------------------------------------------------------------------------
static void LoadData( CALENDER_ENCOUNT_INFO* p_info, HEAPID heap_id )
{
  int i;
  u8* data;
  int pos = 0; 

  // �f�[�^�ǂݍ���
  data = (u8*)GFL_ARC_LoadDataAlloc( ARCID_CALENDER, NARC_calender_encount_bin, heap_id );

  // �f�[�^�����擾
  p_info->dataNum = GetU16( data, pos );
  pos += 2;

  // �o�b�t�@�m��
  p_info->month     = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->dataNum );
  p_info->day       = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->dataNum );
  p_info->encountNo = (u8*)GFL_HEAP_AllocMemory( heap_id, sizeof( u8 ) * p_info->dataNum );

  // ���E���E�o�����ω��ԍ����f�[�^�������擾
  for( i=0; i<p_info->dataNum; i++ )
  {
    p_info->month[i]     = data[ pos++ ];
    p_info->day[i]       = data[ pos++ ];
    p_info->encountNo[i] = data[ pos++ ];
  }

  // ��n��
  GFL_HEAP_FreeMemory( data );


  // DEBUG:
  OBATA_Printf( "dataNum = %d\n", p_info->dataNum );
  for( i=0; i<p_info->dataNum; i++ )
  {
    OBATA_Printf( "%d/%d = %d\%\n", p_info->month[i], p_info->day[i], p_info->encountNo[i] );
  } 
}
