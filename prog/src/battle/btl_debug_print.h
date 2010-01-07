//=============================================================================================
/**
 * @file  btl_debug_print.h
 * @brief �|�P����WB �o�g���V�X�e�� �f�o�b�OPrint�@�\
 * @author  taya
 *
 * @date  2010.01.07  �쐬
 */
//=============================================================================================
#pragma once


typedef enum {
  BTL_PRINTTYPE_UNKNOWN,    ///< �^�C�v���m��
  BTL_PRINTTYPE_SERVER,     ///< �^�C�v�T�[�o�[
  BTL_PRINTTYPE_CLIENT,     ///< �^�C�v�T�[�o�[�ȊO
  BTL_PRINTTYPE_STANDALONE, ///< �^�C�v�X�^���h�A�������s
}BtlPrintType;

/**
 *  �����񃊃e����ID
 */
typedef enum {
  DBGSTR_SETUP_DONE = 0,
  DBGSTR_DEBUGFLAG_BIT,
  DBGSTR_ADAPTER_SEND_START,
  DBGSTR_ADAPTER_RECV_DONE,
  DBGSTR_ADAPTER_RECVED_A_CLIENT,
  DBGSTR_CLIENT_RETURN_CMD_START,
  DBGSTR_CLIENT_RETURN_CMD_DONE,
  DBGSTR_SelectAction_Done,
  DBGSTR_CLIENT_SelectActionSkip,
  DBGSTR_CLIENT_SelectActionDone,
  DBGSTR_CLIENT_ReturnSeqDone,
  DBGSTR_CLIENT_WazaLockInfo,
  DBGSTR_MAIN_SwapPartyMember,
  DBGSTR_NET_DecideClientID,
  DBGSTR_NET_SendSucceed,
  DBGSTR_NET_RecvedServerParam,
  DBGSTR_NET_RecvedPartyData,
  DBGSTR_NET_PartyDataComplete,
  DBGSTR_NET_SendCmdDone,
  DBGSTR_NET_RecvedAllClientsData,
  DBGSTR_NET_ReturnToServerTrying,
  DBGSTR_NET_RecvedClientData,
  DBGSTR_CALCDMG_BaseDamage,
  DBGSTR_CALCDMG_RangeHosei,
  DBGSTR_CALCDMG_WeatherHosei,
  DBGSTR_CALCDMG_RAW_DAMAGE,
  DBGSTR_CALCDMG_Critical,
  DBGSTR_CALCDMG_RandomHosei,
  DBGSTR_CALCDMG_TypeMatchHosei,
  DBGSTR_CALCDMG_TypeAffInfo,
  DBGSTR_CALCDMG_DamageResult,
  DBGSTR_CALCDMG_DamageMarume,

}BtlDebugStrID;


extern void BTL_DEBUGPRINT_SetType( BtlPrintType type );
extern void BTL_DEBUGPRINT_PrintHeader( const char* fileName, u32 line );
extern void BTL_DEBUGPRINT_PrintDump( const char* caption, const void* data, u32 size );
extern const char* BTL_DEBUGPRINT_GetFormatStr( BtlDebugStrID strID );

