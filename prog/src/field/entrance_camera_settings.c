/////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ����o������̃J�������o�f�[�^
 * @file   entrance_camera_settings.c
 * @author obata
 * @date   2010.02.16
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "entrance_camera_settings.h"

#include "field/eventdata_type.h"  // for EXIT_TYPE_xxxx
#include "arc/arc_def.h"           // for ARCID_xxxx
#include "../../resource/fldmapdata/entrance_camera/entrance_camera.naix"  // for NARC_xxxx


// �f�o�b�O�o�̓X�C�b�`
#define DEBUG_PRINT_ON


//--------------------------------------------------------------------------------------- 
/**
 * @brief �w�肵���o������^�C�v�ɊY������f�[�^���擾����
 *
 * @param dest     �擾�����f�[�^�̊i�[��
 * @param exitType �f�[�^�擾�Ώۂ̏o������^�C�v
 */
//---------------------------------------------------------------------------------------
void ENTRANCE_CAMERA_SETTINGS_LoadData( ENTRANCE_CAMERA_SETTINGS* dest, EXIT_TYPE exitType )
{
  // �o������^�C�v�ɑΉ�����J�����f�[�^�̃C���f�b�N�X
  const ARCDATID dataID[ EXIT_TYPE_MAX ] = 
  {
    0, 0, 0, 0, 0, 0, 0,  // EXIT_TYPE_NONE - EXIT_TYPE_INTRUDE
    NARC_entrance_camera_exit_type_sp1_bin,    // EXIT_TYPE_SP1
    NARC_entrance_camera_exit_type_sp2_bin,    // EXIT_TYPE_SP2
    NARC_entrance_camera_exit_type_sp3_bin,    // EXIT_TYPE_SP3
    NARC_entrance_camera_exit_type_sp4_bin,    // EXIT_TYPE_SP4
    NARC_entrance_camera_exit_type_sp5_bin,    // EXIT_TYPE_SP5
    NARC_entrance_camera_exit_type_sp6_bin,    // EXIT_TYPE_SP6
    NARC_entrance_camera_exit_type_sp7_bin,    // EXIT_TYPE_SP7
    NARC_entrance_camera_exit_type_sp8_bin,    // EXIT_TYPE_SP8
    NARC_entrance_camera_exit_type_sp9_bin,    // EXIT_TYPE_SP9
    NARC_entrance_camera_exit_type_sp10_bin,   // EXIT_TYPE_SP10
    NARC_entrance_camera_exit_type_sp11_bin,   // EXIT_TYPE_SP11
    NARC_entrance_camera_exit_type_sp12_bin,   // EXIT_TYPE_SP12
    NARC_entrance_camera_exit_type_sp13_bin,   // EXIT_TYPE_SP13
    NARC_entrance_camera_exit_type_sp14_bin,   // EXIT_TYPE_SP14
    NARC_entrance_camera_exit_type_sp15_bin,   // EXIT_TYPE_SP15
    NARC_entrance_camera_exit_type_sp16_bin,   // EXIT_TYPE_SP16
    NARC_entrance_camera_exit_type_sp17_bin,   // EXIT_TYPE_SP17
    NARC_entrance_camera_exit_type_sp18_bin,   // EXIT_TYPE_SP18
    NARC_entrance_camera_exit_type_sp19_bin,   // EXIT_TYPE_SP19
    NARC_entrance_camera_exit_type_sp20_bin,   // EXIT_TYPE_SP20
    NARC_entrance_camera_exit_type_sp21_bin,   // EXIT_TYPE_SP21
    NARC_entrance_camera_exit_type_sp22_bin,   // EXIT_TYPE_SP22
    NARC_entrance_camera_exit_type_sp23_bin,   // EXIT_TYPE_SP23
    NARC_entrance_camera_exit_type_sp24_bin,   // EXIT_TYPE_SP24
    NARC_entrance_camera_exit_type_sp25_bin,   // EXIT_TYPE_SP25
    NARC_entrance_camera_exit_type_sp26_bin,   // EXIT_TYPE_SP26
    NARC_entrance_camera_exit_type_sp27_bin,   // EXIT_TYPE_SP27
    NARC_entrance_camera_exit_type_sp28_bin,   // EXIT_TYPE_SP28
    NARC_entrance_camera_exit_type_sp29_bin,   // EXIT_TYPE_SP29
    NARC_entrance_camera_exit_type_sp30_bin,   // EXIT_TYPE_SP30
    NARC_entrance_camera_exit_type_sp31_bin,   // EXIT_TYPE_SP31
    NARC_entrance_camera_exit_type_sp32_bin,   // EXIT_TYPE_SP32
    NARC_entrance_camera_exit_type_sp33_bin,   // EXIT_TYPE_SP33
    NARC_entrance_camera_exit_type_sp34_bin,   // EXIT_TYPE_SP34
    NARC_entrance_camera_exit_type_sp35_bin,   // EXIT_TYPE_SP35
    NARC_entrance_camera_exit_type_sp36_bin,   // EXIT_TYPE_SP36
    NARC_entrance_camera_exit_type_sp37_bin,   // EXIT_TYPE_SP37
    NARC_entrance_camera_exit_type_sp38_bin,   // EXIT_TYPE_SP38
    NARC_entrance_camera_exit_type_sp39_bin,   // EXIT_TYPE_SP39
    NARC_entrance_camera_exit_type_sp40_bin,   // EXIT_TYPE_SP40
    NARC_entrance_camera_exit_type_sp41_bin,   // EXIT_TYPE_SP41
    NARC_entrance_camera_exit_type_sp42_bin,   // EXIT_TYPE_SP42
    NARC_entrance_camera_exit_type_sp43_bin,   // EXIT_TYPE_SP43
    NARC_entrance_camera_exit_type_sp44_bin,   // EXIT_TYPE_SP44
    NARC_entrance_camera_exit_type_sp45_bin,   // EXIT_TYPE_SP45
    NARC_entrance_camera_exit_type_sp46_bin,   // EXIT_TYPE_SP46
    NARC_entrance_camera_exit_type_sp47_bin,   // EXIT_TYPE_SP47
    NARC_entrance_camera_exit_type_sp48_bin,   // EXIT_TYPE_SP48
    NARC_entrance_camera_exit_type_sp49_bin,   // EXIT_TYPE_SP49
    NARC_entrance_camera_exit_type_sp50_bin,   // EXIT_TYPE_SP50
  };

  // �����G���[
  GF_ASSERT( exitType < EXIT_TYPE_MAX );

  // �ǂݍ���
  GFL_ARC_LoadData( dest, ARCID_ENTRANCE_CAMERA, dataID[ exitType ] );

  // DEBUG:
#ifdef DEBUG_PRINT_ON
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: frame   = %d\n", dest->frame );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: pitch   = %x\n", dest->pitch );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: yaw     = %x\n", dest->yaw );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: length  = %x\n", dest->length );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetX = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetY = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: offsetZ = %x\n", dest->targetOffsetX );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: validFlag_IN  = %x\n", dest->validFlag_IN );
  OBATA_Printf( "ENTRANCE-CAMERA-SETTINGS: validFlag_OUT = %x\n", dest->validFlag_OUT );
#endif
}
