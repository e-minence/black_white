REM ���E�S��CSV�f�[�^�쐬���E�E�E
wificonv 3 world.txt place_pos_wrd.dat 
REM �A���[���`��CSV�f�[�^�쐬���E�E�E
wificonv 2 ARG.txt place_pos_ARG.dat 
REM �I�[�X�g�����ACSV�f�[�^�쐬���E�E�E
wificonv 2 AUS.txt place_pos_AUS.dat 
REM �u���W��CSV�f�[�^�쐬���E�E�E
wificonv 2 BRA.txt place_pos_BRA.dat 
REM �J�i�_CSV�f�[�^�쐬���E�E�E
wificonv 2 CAN.txt place_pos_CAN.dat 
REM ����CSV�f�[�^�쐬���E�E�E
wificonv 2 CHN.txt place_pos_CHN.dat 
REM �t�B�������hCSV�f�[�^�쐬���E�E�E
wificonv 2 FIN.txt place_pos_FIN.dat 
REM �t�����XCSV�f�[�^�쐬���E�E�E
wificonv 2 FRA.txt place_pos_FRA.dat 
REM �h�C�cCSV�f�[�^�쐬���E�E�E
wificonv 2 DEU.txt place_pos_DEU.dat 
REM �C���hCSV�f�[�^�쐬���E�E�E
wificonv 2 IND.txt place_pos_IND.dat 
REM �C�^���ACSV�f�[�^�쐬���E�E�E
wificonv 2 ITA.txt place_pos_ITA.dat 
REM ���{CSV�f�[�^�쐬���E�E�E
wificonv 2 JPN.txt place_pos_JPN.dat 
REM �m���E�F�[CSV�f�[�^�쐬���E�E�E
wificonv 2 NOR.txt place_pos_NOR.dat 
REM �|�[�����hCSV�f�[�^�쐬���E�E�E
wificonv 2 POL.txt place_pos_POL.dat 
REM ���V�ACSV�f�[�^�쐬���E�E�E
wificonv 2 RUS.txt place_pos_RUS.dat 
REM �X�y�C��CSV�f�[�^�쐬���E�E�E
wificonv 2 ESP.txt place_pos_ESP.dat 
REM �X�E�F�[�f��CSV�f�[�^�쐬���E�E�E
wificonv 2 SWE.txt place_pos_SWE.dat 
REM �C�M���XCSV�f�[�^�쐬���E�E�E
wificonv 2 GBR.txt place_pos_GBR.dat 
REM �A�����JCSV�f�[�^�쐬���E�E�E
wificonv 2 USA.txt place_pos_USA.dat 

ruby wifiearth_gmm.rb world.txt wifi_place_msg_world.gmm
ruby wifiearth_gmm.rb ARG.txt wifi_place_msg_ARG.gmm
ruby wifiearth_gmm.rb AUS.txt wifi_place_msg_AUS.gmm
ruby wifiearth_gmm.rb BRA.txt wifi_place_msg_BRA.gmm
ruby wifiearth_gmm.rb CAN.txt wifi_place_msg_CAN.gmm
ruby wifiearth_gmm.rb CHN.txt wifi_place_msg_CHN.gmm
ruby wifiearth_gmm.rb DEU.txt wifi_place_msg_DEU.gmm
ruby wifiearth_gmm.rb ESP.txt wifi_place_msg_ESP.gmm
ruby wifiearth_gmm.rb FIN.txt wifi_place_msg_FIN.gmm
ruby wifiearth_gmm.rb FRA.txt wifi_place_msg_FRA.gmm
ruby wifiearth_gmm.rb GBR.txt wifi_place_msg_GBR.gmm
ruby wifiearth_gmm.rb IND.txt wifi_place_msg_IND.gmm
ruby wifiearth_gmm.rb ITA.txt wifi_place_msg_ITA.gmm
ruby wifiearth_gmm.rb JPN.txt wifi_place_msg_JPN.gmm
ruby wifiearth_gmm.rb NOR.txt wifi_place_msg_NOR.gmm
ruby wifiearth_gmm.rb POL.txt wifi_place_msg_POL.gmm
ruby wifiearth_gmm.rb RUS.txt wifi_place_msg_RUS.gmm
ruby wifiearth_gmm.rb SWE.txt wifi_place_msg_SWE.gmm
ruby wifiearth_gmm.rb USA.txt wifi_place_msg_USA.gmm

ruby wifiearth_cdat.rb aiueo.txt sort_list.cdat
