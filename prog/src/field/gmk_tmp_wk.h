//======================================================================
/**
 * @file	gmk_tmp_work.h
 * @brief	�M�~�b�N�e���|�������[�N
 * @author  Saito
 *
 * @todo  �e���|�������[�N���g�p����ۂ́A���̃w�b�_�[�t�@�C���𖾎��I�ɃC���N���[�h���Ă�������
 * @todo  �\���̓������m�ہA�m�ۃ������|�C���^�擾�A�����������3�ł�
 *
 */
//======================================================================
#pragma once

#include "field/fieldmap_proc.h"    //for FIELDMAP_WORK

extern void GMK_TMP_WK_AllocWork
      (FIELDMAP_WORK *fieldWork, const u32 inAssignID, const HEAPID inHeapID, const u32 inSize);
extern void GMK_TMP_WK_FreeWork
      (FIELDMAP_WORK *fieldWork, const u32 inAssignID);
extern void *GMK_TMP_WK_GetWork(FIELDMAP_WORK *fieldWork, const u32 inAssignID);



