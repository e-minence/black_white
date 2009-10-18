//==============================================================================
/**
 * @file    intrude_subdisp.h
 * @brief   �N������ʂ̃w�b�_
 * @author  matsuda
 * @date    2009.10.17(�y)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �^��`
//==============================================================================
typedef struct _INTRUDE_SUBDISP * INTRUDE_SUBDISP_PTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern INTRUDE_SUBDISP_PTR INTRUDE_SUBDISP_Init(GAMESYS_WORK *gsys);
extern void INTRUDE_SUBDISP_Exit(INTRUDE_SUBDISP_PTR unisub);
extern void INTRUDE_SUBDISP_Update(INTRUDE_SUBDISP_PTR unisub);
extern void INTRUDE_SUBDISP_Draw(INTRUDE_SUBDISP_PTR unisub);

