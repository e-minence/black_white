#ifdef __cplusplus
extern "C" {
#endif
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifi_2dchar.h
 *	@brief		wifi�QD�L�����N�^�ǂݍ��݃V�X�e��
 *	@author		tomoya takahshi
 *	@data		2007.02.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __WIFI_2DCHAR_H__
#define __WIFI_2DCHAR_H__

#include <gflib.h>
#include "system/palanm.h"
#include "wifi_2dcharcon.h"
#include "net_app/wifi2dmap/wifi_2dcommon.h"

enum unionchar_enum{
	WF_2DC_BOY2,
	WF_2DC_TRAINERM,	//�G���[�g�g���[�i�[
	WF_2DC_RANGERM,	//�|�P���������W���[
	WF_2DC_BREEDERM,	//	�|�P�����u���[�_�[
	WF_2DC_ASSISTANTM,	//	���񂫂イ����
	WF_2DC_MOUNTMAN,	//	��܂��Ƃ�
	WF_2DC_BADMAN,	//	�X�L���w�b�Y
	WF_2DC_BABYBOY1,	//	�悤������
	WF_2DC_GIRL2,	//	�~�j�X�J�[�g
	WF_2DC_TRAINERW,	//	�G���[�g�g���[�i�[
	WF_2DC_RANGERW,	//	�|�P���������W���[
	WF_2DC_BREEDERW,	//	�|�P�����u���[�_�[
	WF_2DC_ASSISTANTW,	//	���񂫂イ����
	WF_2DC_AMBRELLA,	//	�p���\�����˂�����
	WF_2DC_NURSE,	//	�i�[�X
	WF_2DC_BABYGIRL1,	//	�悤������
  WF_2DC_UNICHAR_NUM,
  WB_HERO    = (WF_2DC_UNICHAR_NUM),
  WB_HEROINE,
  WF_2DC_CHARNUM,
};



//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	����^�C�v
// �����I�ɂ́A�Z���A�j�����\�[�X�̎��
//=====================================
typedef enum {
	// �I�[�\�h�b�N�X�A�j��
	WF_2DC_MOVERUN = WF2DC_C_MOVERUN,		// �����{�U������{����A�j���i��l���̂݁j
	WF_2DC_MOVENORMAL = WF2DC_C_MOVENORMAL,	// �����{�U������̂݃A�j��
	WF_2DC_MOVETURN = WF2DC_C_MOVETURN,	// �U������̂݃A�j��
	
	// �g���A�j��
	
	// �A�j���[�V�����^�C�v��
	WF_2DC_MOVENUM = WF2DC_C_MOVENUM,
	
} WF_2DC_MOVETYPE;

//-------------------------------------
///	�A�j���[�V�����^�C�v
//�@����^�C�v���I�[�\�h�b�N�X�A�j���̏ꍇ�ɂ̂�
//�@�g�p�ł���t���O�ł��B
//	
//=====================================
typedef enum {
	WF_2DC_ANMWAY = WF2DC_C_ANMWAY,	// �����ς��A�j��	1�t���[���Ő؂�ւ��܂�
	WF_2DC_ANMROTA = WF2DC_C_ANMROTA,	// ��]�A�j��		���[�v

	// WF_2DC_MOVETURN���[�h�łȂ��Ǝw��ł��܂���
	WF_2DC_ANMWALK = WF2DC_C_ANMWALK,	// �����A�j��		1��8�t���[��
	WF_2DC_ANMTURN = WF2DC_C_ANMTURN,	// �U������A�j��	2�t���[��

	// WF_2DC_MOVERUN���[�h�łȂ��Ǝw��ł��܂���
	WF_2DC_ANMRUN = WF2DC_C_ANMRUN,	// ����A�j��		1��4�t���[��

	//  �Ǖ����A�j��
	WF_2DC_ANMWALLWALK = WF2DC_C_ANMWALLWALK,	// �Ǖ����A�j��	1��16�t���[��

	// ����������
	WF_2DC_ANMSLOWWALK = WF2DC_C_ANMSLOWWALK,	// 1��16�t���[��

	// ��������
	WF_2DC_ANMHIGHWALK2 = WF2DC_C_ANMHIGHWALK2,	// 1��2�t���[��
	WF_2DC_ANMHIGHWALK4 = WF2DC_C_ANMHIGHWALK4,	// 1��4�t���[��
	
	// ���������Ƃ��͂���
	WF_2DC_ANMNONE = WF2DC_C_ANMNONE,
	
	WF_2DC_ANMNUM = WF2DC_C_ANMNUM,
} WF_2DC_ANMTYPE;


//-------------------------------------
///	���W�擾�^�C�v
typedef enum {
	WF_2DC_GET_X,
	WF_2DC_GET_Y
} WF_2DC_MAT;



//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
/// 2D�L�����N�^�Ǘ��V�X�e��
typedef struct _WF_2DCSYS	WF_2DCSYS;

/// 2D�L�����N�^���[�N
typedef struct _WF_2DCWK	WF_2DCWK;

//-------------------------------------
///	���[�N�o�^�f�[�^
typedef struct {
	s16 x;
	s16 y;
	s16 pri;
	s16 bgpri;
} WF_2DC_WKDATA;


//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
// �V�X�e���̐���
extern WF_2DCSYS* WF_2DC_SysInit( GFL_CLUNIT* p_unit, PALETTE_FADE_PTR p_pfd, u32 objnum, u32 heap );
extern void WF_2DC_SysExit( WF_2DCSYS* p_sys );

// ���\�[�X�o�^
extern void WF_2DC_ResSet( WF_2DCSYS* p_sys, u32 view_type, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
extern void WF_2DC_ResDel( WF_2DCSYS* p_sys, u32 view_type );
extern BOOL WF_2DC_ResCheck( const WF_2DCSYS* cp_sys, u32 view_type );
extern void WF_2DC_AllResDel( WF_2DCSYS* p_sys );

// ���j�I�����\�[�X�o�^
// ���j�I�����\�[�X�́Aboy1 boy3 man3 badman explore fighter gorggeousm mystery girl1 girl2 woman2 woman3 idol lady cowgirl gorggeousw �̂P�U��
extern void WF_2DC_UnionResSet( WF_2DCSYS* p_sys, u32 draw_type, WF_2DC_MOVETYPE movetype, u32 heap );
extern void WF_2DC_UnionResDel( WF_2DCSYS* p_sys );

// �A�̃��\�[�X�ǂݍ���
// �A���\�[�X�͎�l���i�j���ǂ��炩�j�̃��\�[�X��ǂݍ��񂾌�ɓo�^���Ă��������B
// ��l���p�̃J���[�p���b�g���g�p���ĕ\�����܂��B
// �܂��A�A�̃��\�[�X��j������O�Ɏ�l���̃��\�[�X��j�����Ȃ��悤�ɂ��肢�������܂��B
extern void WF_2DC_ShadowResSet( WF_2DCSYS* p_sys, u32 draw_type, u32 shadow_pri, u32 heap );
extern void WF_2DC_ShadowResDel( WF_2DCSYS* p_sys );

// �A�N�^�[�쐬
extern WF_2DCWK* WF_2DC_WkAdd( WF_2DCSYS* p_sys, const WF_2DC_WKDATA* cp_data, u32 view_type, u32 heap );
extern void WF_2DC_WkDel( WF_2DCWK* p_wk );

// ���[�N����n
extern GFL_CLWK* WF_2DC_WkClWkGet( WF_2DCWK* p_wk );
extern const GFL_CLWK* WF_2DC_WkConstClWkGet( const WF_2DCWK* cp_wk );
extern void WF_2DC_WkMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y );
extern s16 WF_2DC_WkMatrixGet( WF_2DCWK* p_wk, WF_2DC_MAT x_y );
extern void WF_2DC_WkDrawPriSet( WF_2DCWK* p_wk, u16 pri );
extern u16 WF_2DC_WkDrawPriGet( const WF_2DCWK* cp_wk );
extern void WF_2DC_WkAnmAddFrame( WF_2DCWK* p_wk, fx32 frame );
extern void WF_2DC_WkDrawFlagSet( WF_2DCWK* p_wk, BOOL flag );
extern BOOL WF_2DC_WkDrawFlagGet( const WF_2DCWK* cp_wk );
extern WF_2DC_MOVETYPE WF_2DC_WkMoveTypeGet( const WF_2DCWK* cp_wk );
extern WF_2DC_ANMTYPE WF_2DC_WkAnmTypeGet( const WF_2DCWK* cp_wk );
extern void WF_2DC_WkAnmFrameSet( WF_2DCWK* p_wk, u16 frame );
extern u16 WF_2DC_WkAnmFrameGet( const WF_2DCWK* cp_wk );
extern void WF_2DC_WkShadowMatrixSet( WF_2DCWK* p_wk, s16 x, s16 y );

// Pattern�A�j���i�����A�U������Ȃǂ͌��܂������������̂Łj
// WF_2DC_ANMWAY�́A1�t���[���ŏI���܂��B
// �I���`�F�b�N������K�v�͂���܂���
extern void WF_2DC_WkPatAnmStart( WF_2DCWK* p_wk, WF_2DC_ANMTYPE anmtype, WF_COMMON_WAY anmway );
extern BOOL WF_2DC_WkPatAnmEndCheck( const WF_2DCWK* cp_wk );
extern void WF_2DC_WkPatAnmAddFrame( WF_2DCWK* p_wk );// Pattern�A�j���X�V����

#endif		// __WIFI_2DCHAR_H__

#ifdef __cplusplus
} /* extern "C" */
#endif
