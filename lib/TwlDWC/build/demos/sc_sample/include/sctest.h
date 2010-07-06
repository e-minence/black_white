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
 * Return value:  ����������
 */
BOOL ScInitialize();

/**
 * ���C������
 *
 * Param:  host �z�X�g���[�h�œ��삷�邩
 * Return value:  ����������
 */
BOOL ScMain( BOOL host );

/**
 * �I������
 */
void ScFinalize();

/**
 * DWC����̃f�[�^��M�R�[���o�b�N�֐��B
 *
 * Param:  aid �f�[�^���M��aid
 * Param:  buffer ��M�o�b�t�@�A�h���X
 * Param:  size ��M�T�C�Y
 * Param:  param �����f�[�^
 */
void ScReceiveDataEvent( u8 aid, u8* buffer, int size, void* param );

#ifdef __cplusplus
}
#endif

#endif  // SCTEST_H
