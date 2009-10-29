//////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �d���f���̃]�[���ˑ��f�[�^
 * @file   elboard_zone_data.h
 * @author obata
 * @date   2009.10.27
 *
 */
//////////////////////////////////////////////////////////////////////////////////////


//====================================================================================
// ���d���f���f�[�^�\����
//====================================================================================
typedef struct
{
  u32 zone_id;        // �]�[��ID
  u32 x;              // x���W
  u32 y;              // y���W
  u32 z;              // z���W
  u32 dir;            // ����
  u32 arcDatID;       // �A�[�J�C�u���C���f�b�N�X(�A�[�J�C�uID�� ARCID_MESSAGE �ŌŒ�)
  u32 msgID_date;     // ���t�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_weather;  // �V�C�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoA;    // �n����(A)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoB;    // �n����(B)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoC;    // �n����(C)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoD;    // �n����(C)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoE;    // �n����(E)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoF;    // �n����(F)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoG;    // �n����(G)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoH;    // �n����(H)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_infoI;    // �n����(I)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmMon;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmTue;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmWed;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmThu;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmFri;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmSat;    // �ꌾCM(�y)�Ɏg�p���郁�b�Z�[�WID
  u32 msgID_cmSun;    // �ꌾCM(��)�Ɏg�p���郁�b�Z�[�WID

} ELBOARD_ZONE_DATA;


//====================================================================================
// ���f�[�^�擾�֐�
//====================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �d���f���]�[���ˑ��f�[�^��ǂݍ���
 * 
 * @param buf    �ǂݍ��񂾃f�[�^�̊i�[��
 * @param arc_id �ǂݍ��ރf�[�^�̃A�[�J�C�uID
 * @param dat_id �ǂݍ��ރf�[�^�̃A�[�J�C�u���C���f�b�N�X
 *
 * @return �ǂݍ��݂��������s��ꂽ�ꍇ TRUE
 */
//-----------------------------------------------------------------------------------
BOOL ELBOARD_ZONE_DATA_Load( ELBOARD_ZONE_DATA* buf, ARCID arc_id, ARCDATID dat_id );
