//==============================================================================
/**
 * @file    beacon_view.h
 * @brief   ����Ⴂ�ʐM��ԎQ�Ɖ��
 * @author  matsuda
 * @date    2009.12.13(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
typedef struct _BEACON_VIEW * BEACON_VIEW_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern BEACON_VIEW_PTR BEACON_VIEW_Init(GAMESYS_WORK *gsys, FIELD_SUBSCREEN_WORK *subscreen);
extern void BEACON_VIEW_Exit(BEACON_VIEW_PTR view);
extern void BEACON_VIEW_Update(BEACON_VIEW_PTR view);
extern void BEACON_VIEW_Draw(BEACON_VIEW_PTR view);

