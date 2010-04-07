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
$h_bgm_chg_type = {"play"=>"DEMO3D_BGM_PLAY","stop"=>"DEMO3D_BGM_STOP","change"=>"DEMO3D_BGM_CHANGE","push"=>"DEMO3D_BGM_PUSH","pop"=>"DEMO3D_BGM_POP"}

#==============================================
#�v���t�B�b�N�X�E�T�t�B�b�N�X��`
#==============================================
$psfix_fx16 = $_psfix_prm.new( "FX16_CONST( ", " )")
$psfix_msg_arc = $_psfix_prm.new( "NARC_message_", "_dat")

#==============================================
#�R�}���h�p�����[�^��`
#==============================================
$prm_none = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #
] 
$prm_se_play = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #SE���x��
  $_cmd_prm.new( 0, 127, $h_se_default, nil ),       #volume
  $_cmd_prm.new( -128, 127, $h_se_default, nil ),      #pan
  $_cmd_prm.new( -32768, 32768, $h_se_default, nil ),  #pitch
  $_cmd_prm.new( 0, 3, $h_se_default, nil ),  #�v���C���[                
]
$prm_se_stop = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil  ), #SE���x��
  $_cmd_prm.new( 0, 3, $h_se_default, nil ),  #�v���C���[                
] 
$prm_se_volume_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #SE���x��
  $_cmd_prm.new( 0, 127, nil, nil ), #�X�^�[�g
  $_cmd_prm.new( 0, 127, nil, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default, nil ), #�v���C���[No
] 
$prm_se_pan_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #SE���x��
  $_cmd_prm.new( -128, 127, nil, nil ), #�X�^�[�g
  $_cmd_prm.new( -128, 127, nil, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default, nil ), #�v���C���[No
] 
$prm_se_pitch_effect_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #SE���x��
  $_cmd_prm.new( -32768, 32767, nil, nil ), #�X�^�[�g
  $_cmd_prm.new( -32768, 32767, nil, nil ), #�G���h
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #�t���[��
  $_cmd_prm.new( 0, 3, $h_se_default, nil ), #�v���C���[No
] 
$prm_bgm_change_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #BGM���x��
  $_cmd_prm.new( 0, 65535, nil, nil ), #fadeout_frame
  $_cmd_prm.new( 0, 65535, nil, nil ), #fadein_frame
  $_cmd_prm.new( P_LIST, P_LIST, $h_bgm_chg_type, nil ), #type
] 
$prm_brightness_req = [
  $_cmd_prm.new( P_LIST, P_LIST, $h_brightness_disp, nil ), #�K�p���
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #�t���[��
  $_cmd_prm.new( -16, 16, nil, nil ), #�J�n�P�x
  $_cmd_prm.new( -16, 16, nil, nil ), #�I���P�x
]
$prm_flash_req = [
  $_cmd_prm.new( P_LIST, P_LIST, $h_brightness_disp ), #�K�p���
  $_cmd_prm.new( -16, 16, nil, nil ), #�J�n�P�x
  $_cmd_prm.new( -16, 16, nil, nil ), #�ő�P�x
  $_cmd_prm.new( -16, 16, nil, nil ), #�I���P�x
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #start�`max�܂ł̃t���[��
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #max�`end�܂ł̃t���[��
]
$prm_light_color_set = [
  $_cmd_prm.new( 0, 3, nil, nil ), #���C�gNo
  $_cmd_prm.new( 0, 31, nil, nil ), #color R
  $_cmd_prm.new( 0, 31, nil, nil ), #color G
  $_cmd_prm.new( 0, 31, nil, nil ), #color B
]
$prm_talkwin_req = [
  $_cmd_prm.new( P_FREE, P_FREE, nil, $psfix_msg_arc ), #���b�Z�[�W�A�[�J�C�uID
  $_cmd_prm.new( P_FREE, P_FREE, nil, nil ), #���b�Z�[�W���x��
  $_cmd_prm.new( 0, P_FREE, nil, nil ), #���b�Z�[�W�Z�b�g��
  $_cmd_prm.new( 1, P_FREE, nil, nil ), #���b�Z�[�W��\������t���[����
  $_cmd_prm.new( 1, 30, nil, nil ), #�E�B���h�E����X
  $_cmd_prm.new( 1, 21, nil, nil ), #�E�B���h�E����Y
  $_cmd_prm.new( 2, 28, nil, nil ), #�E�B���h�E�T�C�YX
  $_cmd_prm.new( 2, 22, nil, nil ), #�E�B���h�E�T�C�YY
] 
$prm_light_vector_set = [
  $_cmd_prm.new( 0, 3, nil, nil ), #���C�gNo
  $_cmd_prm.new( -7.9, 7.9, nil, $psfix_fx16 ), #vector X
  $_cmd_prm.new( -7.9, 7.9, nil, $psfix_fx16 ), #vector Y
  $_cmd_prm.new( -7.9, 7.9, nil, $psfix_fx16 ), #vector Z
]
$prm_motionbl_start = [
  $_cmd_prm.new( 0, 31, nil, nil ), #�V�����u�����h���郿
  $_cmd_prm.new( 0, 31, nil, nil ), #�o�b�t�@�����O����Ă��郿
] 

$cmd_tbl = [
  CDemo3DCmd::new( "SE_PLAY", $prm_se_play),
  CDemo3DCmd::new( "SE_STOP", $prm_se_stop),
  CDemo3DCmd::new( "SE_VOLUME_EFFECT_REQ", $prm_se_volume_effect_req),
  CDemo3DCmd::new( "SE_PAN_EFFECT_REQ", $prm_se_pan_effect_req),
  CDemo3DCmd::new( "SE_PITCH_EFFECT_REQ", $prm_se_pitch_effect_req),
  CDemo3DCmd::new( "BGM_CHANGE_REQ", $prm_bgm_change_req),
  CDemo3DCmd::new( "BRIGHTNESS_REQ", $prm_brightness_req),
  CDemo3DCmd::new( "FLASH_REQ", $prm_flash_req),
  CDemo3DCmd::new( "LIGHT_COLOR_SET", $prm_light_color_set),
  CDemo3DCmd::new( "LIGHT_VECTOR_SET", $prm_light_vector_set),
  CDemo3DCmd::new( "TALKWIN_REQ", $prm_talkwin_req),
  CDemo3DCmd::new( "MOTIONBL_START", $prm_motionbl_start),
  CDemo3DCmd::new( "MOTIONBL_END", nil ),
  CDemo3DCmd::new( "END", nil),
]


