//======================================================================
/**
 * @file	battle_championship.h
 * @brief	���j���[
 * @author	ariizumi
 * @data	09/10/08
 *
 * ���W���[�����FBATTLE_CHAMPINONSHIP
 */
//======================================================================
#pragma once

//======================================================================
//	define
//======================================================================
#pragma mark [> define
//������Ɨ��\�ł����A���̒l��������void*�ɗ^���Ă�������
typedef void* BATTLE_CHAMPIONSHIP_MODE;
#define BATTLE_CHAMPIONSHIP_MODE_MAIN_MENU  ((BATTLE_CHAMPIONSHIP_MODE)0)    //�������j���[
#define BATTLE_CHAMPIONSHIP_MODE_WIFI_MENU  ((BATTLE_CHAMPIONSHIP_MODE)1)    //WIFI���j���[

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto
extern const GFL_PROC_DATA BATTLE_CHAMPIONSHIP_ProcData;

//--------------------------------------------------------------
//	
//--------------------------------------------------------------
FS_EXTERN_OVERLAY( battle_championship );
