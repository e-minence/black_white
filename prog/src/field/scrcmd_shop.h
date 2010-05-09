//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�V���b�v�֘A
 * @file   scrcmd_shop.h
 * @author iwasawa
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#pragma once


typedef struct {
  u16 id;       // �ǂ���ID
  u16 price;    // �l�i or BP�|�C���g
}SHOP_ITEM;

// ��x�ɕ��ׂ邱�Ƃ��ł��鏤�i��
#define SHOP_ITEM_MAX   ( 40 )

//--------------------------------------------------------------------
/**
 * @brief �ʏ�V���b�v�Ăяo�� 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk );

