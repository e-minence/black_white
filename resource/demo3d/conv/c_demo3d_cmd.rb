#-----------------------------------------------
# demo3d �R�}���h�p�����[�^��`
# �R�}���h����������$cmd_table[]��ǉ�����K�v������܂�
#
# c_demo3d_cmd_check.rb��require���Ďg���܂�
#-----------------------------------------------
#==============================================
#���X�g�l�n�b�V����`
#==============================================
$h_se_default = {"---"=>"DEMO3D_SE_PARAM_DEFAULT"}
$h_brightness_disp = {"main"=>"MASK_MAIN_DISPLAY","sub"=>"MASK_SUB_DISPLAY","double"=>"MASK_DOUBLE_DISPLAY"}

#==============================================
#�R�}���h�p�����[�^��`
#==============================================
$prm_none = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #
] 
$prm_se_play = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #SE���x��
  $_cmd_prm.new( 0, 127, $h_se_default ),       #volume
  $_cmd_prm.new( -128, 127, $h_se_default ),      #pan
  $_cmd_prm.new( -32768, 32768, $h_se_default ),  #pitch
  $_cmd_prm.new( 0, 3, $h_se_default ),  #�v���C���[                
]
$prm_se_stop = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #SE���x��
] 
$prm_se_volume_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #SE���x��
  $_cmd_prm.new( 0, 127, nil ), #�X�^�[�g
  $_cmd_prm.new( 0, 127, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default ), #�v���C���[No
] 
$prm_se_pan_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #SE���x��
  $_cmd_prm.new( -128, 127, nil ), #�X�^�[�g
  $_cmd_prm.new( -128, 127, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default ), #�v���C���[No
] 
$prm_se_pitch_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #SE���x��
  $_cmd_prm.new( -32768, 32767, nil ), #�X�^�[�g
  $_cmd_prm.new( -32768, 32767, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default ), #�v���C���[No
] 
$prm_brightness_req = [
  $_cmd_prm.new( P_LIST, P_LIST, $h_brightness_disp ), #�K�p���
  $_cmd_prm.new( P_FREE, P_FREE, nil ), #�t���[��
  $_cmd_prm.new( -16, 16, nil ), #�J�n�P�x
  $_cmd_prm.new( -16, 16, nil ), #�I���P�x
]
$prm_flash_req = [
  $_cmd_prm.new( P_LIST, P_LIST, $h_brightness_disp ), #�K�p���
  $_cmd_prm.new( -16, 16, nil ), #�J�n�P�x
  $_cmd_prm.new( -16, 16, nil ), #�ő�P�x
  $_cmd_prm.new( -16, 16, nil ), #�I���P�x
  $_cmd_prm.new( 1, P_FREE, nil ), #start�`max�܂ł̃t���[��
  $_cmd_prm.new( 1, P_FREE, nil ), #max�`end�܂ł̃t���[��
]
$prm_motionbl_start = [
  $_cmd_prm.new( 0, 31, nil ), #�V�����u�����h���郿
  $_cmd_prm.new( 0, 31, nil ), #�o�b�t�@�����O����Ă��郿
] 

$cmd_tbl = [
  CDemo3DCmd::new( "SE_PLAY", 5, $prm_se_play),
  CDemo3DCmd::new( "SE_STOP", 1, $prm_se_stop),
  CDemo3DCmd::new( "SE_VOLUME_EFFECT_REQ", 5, $prm_se_volume_effect_req),
  CDemo3DCmd::new( "SE_PAN_EFFECT_REQ", 5, $prm_se_pan_effect_req),
  CDemo3DCmd::new( "SE_PITCH_EFFECT_REQ", 5, $prm_se_pitch_effect_req),
  CDemo3DCmd::new( "BRIGHTNESS_REQ", 4, $prm_brightness_req),
  CDemo3DCmd::new( "FLASH_REQ", 6, $prm_flash_req),
  CDemo3DCmd::new( "MOTIONBL_START", 2, $prm_motionbl_start),
  CDemo3DCmd::new( "MOTIONBL_END", 0, $prm_none),
  CDemo3DCmd::new( "END", 0, $prm_none),
]


