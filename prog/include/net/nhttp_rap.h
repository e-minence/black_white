//=============================================================================
/**
 * @file	  nhttp_rap.h
 * @brief	  nhttp��PDW�T�[�o�ɃA�N�Z�X����ׂ̃��b�p�[�֐�
 * @author	k.ohno
 * @date    2009.11.29
 */
//=============================================================================

#pragma once

#include <nitro.h>
#include "gflib.h"
#include <nitroWiFi/nhttp.h>
#include <nitroWiFi/soc.h>
#include "nitrowifidummy.h"
#include "dreamworld_netdata.h"
#include "webresp_defs.h"
#include "net/network_define.h"

//-------------------------------------
///	NHTTP_RAP�n���h��
//=====================================
typedef struct _NHTTP_RAP_WORK NHTTP_RAP_WORK;


typedef enum{
  NHTTPRAP_URL_ACCOUNTINFO,
  NHTTPRAP_URL_POKEMONLIST,
  NHTTPRAP_URL_DOWNLOAD,
  NHTTPRAP_URL_UPLOAD,
  NHTTPRAP_URL_ACCOUNT_CREATE,
  NHTTPRAP_URL_BATTLE_DOWNLOAD,
  NHTTPRAP_URL_BATTLE_UPLOAD,
  NHTTPRAP_URL_POKECHK,
  NHTTPRAP_URL_DOWNLOAD_FINISH,
} NHTTPRAP_URL_ENUM;


//typedef void (Callback_NHTTPError)(void* pUserWork, int code, int type, int ret );


extern BOOL NHTTP_RAP_ConectionCreate(NHTTPRAP_URL_ENUM urlno,NHTTP_RAP_WORK* pWork);
extern NHTTPConnectionHandle NHTTP_RAP_GetHandle(NHTTP_RAP_WORK* pWork);
extern NHTTPError NHTTP_RAP_StartConnect(NHTTP_RAP_WORK* pWork);
extern NHTTPError NHTTP_RAP_Process(NHTTP_RAP_WORK* pWork);
extern void* NHTTP_RAP_GetRecvBuffer(NHTTP_RAP_WORK* pWork);

extern NHTTP_RAP_WORK* NHTTP_RAP_Init(HEAPID heapID,u32 profileid, DWCSvlResult* pSvl);
extern void NHTTP_RAP_End(NHTTP_RAP_WORK* pWork);

//------------------------------------------------------------------------------
/**
 * @brief   �F�؃L�[�쐬�J�n
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @retval  TRUE�Ȃ琬��
 */
//------------------------------------------------------------------------------
extern BOOL NHTTP_RAP_SvlGetTokenStart(NHTTP_RAP_WORK* pWork);

//------------------------------------------------------------------------------
/**
 * @brief   �F�؃L�[�쐬�����C��
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @retval  TRUE�Ȃ�F�؃L�[�����Ƃ���
 */
//------------------------------------------------------------------------------
extern BOOL NHTTP_RAP_SvlGetTokenMain(NHTTP_RAP_WORK* pWork);

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s�������̈�m��
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @param   HEAPID heapID,          �̈�m�ۂ�ID
 * @param   int size                 �|�P����*��+367�o�C�g ���炢�K�v�ł�
 * @param   NHTTP_POKECHK_ENUM type network_define.h�ɂ���s�������^�C�v
 * @retval  none
 */
//------------------------------------------------------------------------------
extern void NHTTP_RAP_PokemonEvilCheckCreate(NHTTP_RAP_WORK* pWork, HEAPID heapID, int size, NHTTP_POKECHK_ENUM type);
//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s�������@�|�P�����ǉ�
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @param   pData   �|�P�����f�[�^
 * @param   int size                �|�P�����\���̂̃T�C�Y
 * @retval  none
 */
//------------------------------------------------------------------------------
extern void NHTTP_RAP_PokemonEvilCheckAdd(NHTTP_RAP_WORK* pWork,const void* pData, int size);
//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �R�l�N�V�����쐬
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @retval  BOOL TRUE�Ȃ�R�l�N�V�������쐬�ł���
 */
//------------------------------------------------------------------------------
extern BOOL NHTTP_RAP_PokemonEvilCheckConectionCreate(NHTTP_RAP_WORK* pWork);
//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �̈�j��
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 */
//------------------------------------------------------------------------------
extern void NHTTP_RAP_PokemonEvilCheckDelete(NHTTP_RAP_WORK* pWork);

//------------------------------------------------------------------------------
/**
 * @brief   �|�P�����s������ �̈�j��
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 */
//------------------------------------------------------------------------------
extern void NHTTP_RAP_RESPONSE_PokemonEvilCheck(NHTTP_RAP_WORK* pWork);

extern void NHTTP_RAP_PokemonEvilCheckReset(NHTTP_RAP_WORK* pWork,NHTTP_POKECHK_ENUM type);





//------------------------------------------------------------------------------
/**
 * @brief   ��M�����p�[�Z���g��Ԃ�
 * @param   NHTTP_RAP_WORK* pWork,  ���[�N
 * @retval    0-100
 */
//------------------------------------------------------------------------------
extern int NHTTP_RAP_ProcessPercent(NHTTP_RAP_WORK* pWork);

//�G���[���N�������̉񕜏���
extern void NHTTP_RAP_ErrorClean(NHTTP_RAP_WORK* pWork);


#if PM_DEBUG
void NHTTP_DEBUG_GPF_HEADER_PRINT(gs_response* prep);
#else
inline void NHTTP_DEBUG_GPF_HEADER_PRINT(gs_response* prep){}
#endif

