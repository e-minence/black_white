#ifndef SCTEST_H
#define SCTEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sc/dwc_sc.h>

//----------------------------------------------------------------------------
// define
//----------------------------------------------------------------------------
    
//----------------------------------------------------------------------------
// typedef
//----------------------------------------------------------------------------

typedef struct PlayerData
{
    DWCScLoginCertificatePtr mCertificate;
    DWCScReportPtr mReport;
    u8 mSessionId[DWC_SC_SESSION_GUID_SIZE];
    u8 mConnectionId[DWC_SC_CONNECTION_GUID_SIZE];
    u8 mStatsAuthdata[DWC_SC_AUTHDATA_SIZE];
} PlayerData;

//----------------------------------------------------------------------------
// function
//----------------------------------------------------------------------------

/**
 * ����������
 *
 * @return ����������
 */
BOOL ScInitialize();

/**
 * ���C������
 *
 * @param host �z�X�g���[�h�œ��삷�邩
 * @return ����������
 */
BOOL ScMain( BOOL host );

/**
 * �I������
 */
void ScFinalize();

/**
 * DWC����̃f�[�^��M�R�[���o�b�N�֐��B
 *
 * @param aid �f�[�^���M��aid
 * @param buffer ��M�o�b�t�@�A�h���X
 * @param size ��M�T�C�Y
 * @param param �����f�[�^
 */
void ScReceiveDataEvent( u8 aid, u8* buffer, int size, void* param );

#ifdef __cplusplus
}
#endif

#endif  // SCTEST_H
