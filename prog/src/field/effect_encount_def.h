/*
 *  @file   effect_encount_def.h
 *  @brief  �G�t�F�N�g�G���J�E���g�@���e������`
 *  @author Miyuki Iwasawa
 *  @date   10.03.04
 */

#pragma once

#define EFFENC_NORMAL_INTERVAL (20)  //�G�t�F�N�g���I�f�t�H���g�C���^�[�o��
#define EFFENC_NORMAL_PROB (10)  //�G�t�F�N�g���I�m���f�t�H���g
#define EFFENC_CAVE_INTERVAL (20)  //�G�t�F�N�g���I���A�C���^�[�o��
#define EFFENC_CAVE_PROB (10)  //�G�t�F�N�g���I�m�����A
#define EFFENC_BRIDGE_INTERVAL (5)  //�G�t�F�N�g���I���C���^�[�o��
#define EFFENC_BRIDGE_PROB (15)  //�G�t�F�N�g���I�m����

//======================================================================
//  define
//======================================================================
typedef enum{
 EFFENC_TYPE_SGRASS_NORMAL,  ///<�Z����(�m�[�}��)
 EFFENC_TYPE_SGRASS_SNOW,  ///<�Z����(����n��)
 EFFENC_TYPE_SGRASS_WARM,  ///<�Z����(��t)
 EFFENC_TYPE_LGRASS,  ///<������
 EFFENC_TYPE_CAVE,    ///<���A
 EFFENC_TYPE_WATER,   ///<�W��
 EFFENC_TYPE_SEA,     ///<�C
 EFFENC_TYPE_BRIDGE,  ///<��
 EFFENC_TYPE_MAX,
}EFFENC_TYPE_ID;

