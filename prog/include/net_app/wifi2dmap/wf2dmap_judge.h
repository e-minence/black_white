#ifdef __cplusplus
extern "C" {
#endif
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_judge.h
 *	@brief		�R�}���h���f���W���[�������N�G�X�g�R�}���h�o�b�t�@
 *	@author		tomoya takahashi
 *	@data		2007.03.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WF2DMAP_JUDGE_H__
#define __WF2DMAP_JUDGE_H__

#include "net_app/wifi2dmap/wf2dmap_common.h"
#include "net_app/wifi2dmap/wf2dmap_obj.h"
#include "net_app/wifi2dmap/wf2dmap_map.h"




//-----------------------------------------------------------------------------
/**
 *			�R�}���h�W���b�W���W���[��
 */
//-----------------------------------------------------------------------------
extern BOOL WF2DMAP_JUDGESysCmdJudge( const WF2DMAP_MAPSYS* cp_mapsys, const WF2DMAP_OBJSYS* cp_objsys, const WF2DMAP_REQCMD* cp_req, WF2DMAP_ACTCMD* p_act );

#endif		// __WF2DMAP_JUDGE_H__

#ifdef __cplusplus
}/* extern "C" */
#endif
