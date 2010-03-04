//======================================================================
/**
 * @file	bmp_taskmenu.h
 * @brief	�^�X�N�o�[�p ���ʃ��j���[
 * @author	ariizumi
 * @data	09/07/27
 *
 * ���W���[�����FBMP_TASKMENU
 */
//======================================================================

#pragma once
#include "print/printsys.h"

//���莞�_�ŃA�j��
#define APP_TASKMENU_ANM_CNT (16)
#define APP_TASKMENU_ANM_INTERVAL (4)

//�f�t�H���g��
#define APP_TASKMENU_PLATE_WIDTH (13)	
#define APP_TASKMENU_PLATE_HEIGHT (3)
//�f�t�H���g��(�͂��E�������p
#define APP_TASKMENU_PLATE_WIDTH_YN_WIN (8)	
#define APP_TASKMENU_PLATE_HEIGHT_YN_WIN (APP_TASKMENU_PLATE_HEIGHT)

//�f�t�H���g�����F�J���[
#define APP_TASKMENU_ITEM_MSGCOLOR	(PRINTSYS_LSB_Make(0xe,0xf,0x3))

//�v���[�g�̃A�j���Bsin�g���̂�0�`0xFFFF�̃��[�v
#define APP_TASKMENU_ANIME_VALUE (0x400)

//�P���܂ǂ̎��
typedef enum
{	
	APP_TASKMENU_WIN_TYPE_NORMAL,	//�ʏ�̉���������
	APP_TASKMENU_WIN_TYPE_RETURN,	//���ǂ�L�������������i�����j������Ȃ�
}APP_TASKMENU_WIN_TYPE;

//�^�X�N���j���[�p���\�[�X�ێ����[�N
typedef struct _APP_TASKMENU_RES APP_TASKMENU_RES;

//�^�X�N���j���[ ���[�N
typedef struct _APP_TASKMENU_WORK APP_TASKMENU_WORK;

//�^�X�N���j���[ �P���P�܂ǃ��[�N
typedef struct _APP_TASKMENU_WIN_WORK APP_TASKMENU_WIN_WORK;

//�ʒu�w�肪���ォ�E����
typedef enum
{
  ATPT_LEFT_UP,     //����
  ATPT_RIGHT_DOWN,  //�E��
  
}APP_TASKMENU_POS_TYPE;

//�^�X�N���j���[ ���������[�N �P����
typedef struct
{
  STRBUF  *str;											//���j���[�ɕ\�����镶����
  PRINTSYS_LSB msgColor;						//�����F�B�f�t�H���g�ł悢�Ȃ��APP_TASKMENU_ITEM_MSGCOLOR
  APP_TASKMENU_WIN_TYPE type;       //�߂�}�[�N�̕\��
}APP_TASKMENU_ITEMWORK;

//�^�X�N���j���[ ���������[�N
typedef struct
{
  int heapId;
  
  u8  itemNum;
  APP_TASKMENU_ITEMWORK *itemWork;
  
  //�ʒu
  APP_TASKMENU_POS_TYPE posType;
  u8  charPosX; //�E�B���h�E�J�n�ʒu(�L�����P��
  u8  charPosY;

	//��	�i�ȑO�͕��̃p�����[�^�͂���܂���ł����B
	//			�ȑO�Ƃ̌݊����̂��߁A0���n��Ɠ����ŁA�f�t�H���g�l�ɂ��܂��B
	//			�݊����̂��߂����ł��̂ŁA0�͍���n���Ȃ��悤�ɂ��Ă��������j
	u8	w;	//�L�����P��
	u8	h;	//�L�����P��
}APP_TASKMENU_INITWORK;
//��APP_TASKMENU_INITWORK��OpenMenu��J�����Ă���肠��܂���B

//���\�[�X�ǂݍ���
extern APP_TASKMENU_RES* APP_TASKMENU_RES_Create( u8 frame, u8 plt, GFL_FONT *fontHandle, PRINT_QUE *printQue, HEAPID heapID );
extern void APP_TASKMENU_RES_Delete( APP_TASKMENU_RES *wk );

//�c���j���[
extern APP_TASKMENU_WORK* APP_TASKMENU_OpenMenu( APP_TASKMENU_INITWORK *initWork, const APP_TASKMENU_RES *res );
extern APP_TASKMENU_WORK* APP_TASKMENU_OpenMenuEx( APP_TASKMENU_INITWORK *initWork, const APP_TASKMENU_RES *res );
extern void APP_TASKMENU_CloseMenu( APP_TASKMENU_WORK *work );
extern void APP_TASKMENU_UpdateMenu( APP_TASKMENU_WORK *work );
extern const BOOL APP_TASKMENU_IsFinish( APP_TASKMENU_WORK *work );
extern const u8 APP_TASKMENU_GetCursorPos( APP_TASKMENU_WORK *work );
extern const void APP_TASKMENU_SetActive( APP_TASKMENU_WORK *work, BOOL isActive );

//�����j���[�ȂǂŎg�p����A�P����
//�����j���[�̍ۂ́A�u�����Ă��v�u��߂�v�ȂǁA�����E�B���h�E���쐬���Ă�������
extern APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_Create( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, HEAPID heapID );
extern APP_TASKMENU_WIN_WORK * APP_TASKMENU_WIN_CreateEx( const APP_TASKMENU_RES *res, const APP_TASKMENU_ITEMWORK *item, u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
extern void APP_TASKMENU_WIN_Delete( APP_TASKMENU_WIN_WORK *wk );
extern void APP_TASKMENU_WIN_Update( APP_TASKMENU_WIN_WORK *wk );
extern void APP_TASKMENU_WIN_SetActive( APP_TASKMENU_WIN_WORK *wk, BOOL isActive );
extern void APP_TASKMENU_WIN_SetDecide( APP_TASKMENU_WIN_WORK *wk, BOOL isDecide );
extern BOOL APP_TASKMENU_WIN_IsDecide( const APP_TASKMENU_WIN_WORK *wk );
extern const BOOL APP_TASKMENU_WIN_IsFinish( const APP_TASKMENU_WIN_WORK *work );
extern const BOOL APP_TASKMENU_WIN_IsTrg( const APP_TASKMENU_WIN_WORK *wk );
extern void APP_TASKMENU_WIN_ResetDecide( APP_TASKMENU_WIN_WORK *wk );
