������������������������������������������������������������������������������
��                                                                          ��
��  readme.txt                                                              ��
��                                                                          ��
��  DP WiFi Library                                                         ��
��                                                                          ��
������������������������������������������������������������������������������


�y�A�v���P�[�V�����ւ̑g�ݍ��݂ɂ��āz

�@���E�ʐM�������C�u�������g�p���邽�߂ɂ́Alibdpw/common�ȉ��̃w�b�_�[��
libdpw/dpw_tr �ȉ��̃w�b�_�[�y�у��C�u�����t�@�C�����v���W�F�N�g�֊܂߂Ă��������B
�ڂ�����libdpw/demos/tr_sample��Makefile�����Q�Ƃ��������B

�@�o�g���^���[���C�u�������g�p���邽�߂ɂ́Alibdpw/common�ȉ��̃w�b�_�[��
libdpw/dpw_bt �ȉ��̃w�b�_�[�y�у��C�u�����t�@�C�����v���W�F�N�g�֊܂߂Ă��������B
�ڂ�����libdpw/demos/bt_sample��Makefile�����Q�Ƃ��������B

���C�u�����̏��������s��Dpw_Bt_Init�֐��ADpw_Tr_Init�֐��̈�����
�f�o�b�O�p�܂��̓����[�X�p�T�[�o���w�肵�Ă��������B

�yDP�؍���ňȍ~�̌l���o�^�z

�@DP�؍���ł�v���`�i�ȍ~��Dpw_Bt_Init�֐���������Dpw_Tr_Init�֐����Ă�ŏ���
��������A�K����xDpw_Tr_SetProfileAsync�֐���������Dpw_Bt_SetProfileAsync�֐�
�Ōl����o�^���Ă��������B�ǂ���̊֐����@�\�͓���ł��B


�y�v���`�i�ȍ~�̃��[���@�\�z

�@���[���A�h���X�F�؎�(�F�؃��[��)��GTS�ŗa���Ă���|�P�������������ꂽ�Ƃ�
(�����ʒm���[��)��E���[�����T�[�o���ő��M���܂��B���M���郁�[���̃T�u�W�F�N�g
�Ɩ{���͊Ǘ��c�[���ŃC�x���g�A���ꂲ�Ƃɐݒ�ł��܂��B���[���A�h���X��GTS����
���̓o�g���^���[��31���Ԑڑ����Ȃ��Ǝ����I�ɖ����ɂȂ�A���[�������M����Ȃ���
��܂��B���������̌�ڑ�����Ǝ����I�ɗL���ɂȂ�܂��B(���݂̂Ƃ���A�a���Ă�
��|�P������31���ō폜����A���̌�����ʒm���[�����������邱�Ƃ͂���܂����
�ł��̗l�q���m�F���邱�Ƃ͂ł��܂���B)

�@E���[�����T�[�o�ɓo�^����ɂ�Dpw_Tr_SetProfileAsync�֐���������
Dpw_Bt_SetProfileAsync�֐����g�p���܂��B(�ǂ���ł��\���܂���B) ���ۂ̓o�^��
����͉��L�̂悤�ɂȂ�܂��B

1. Dpw_xx_SetProfileAsync�֐��Ɏw�肷��Dpw_Common_Profile�\���̂�mailAddr����
�@ �o�Ƀ��[���A�h���X��ݒ肵�AmailAddrAuthPass�����o��
�@ DPW_MAIL_ADDR_AUTH_START_PASSWORD��ݒ肵�AmailAddrAuthVerification�����o��
�@ 0�`999�̐����������_���ɐݒ肵�܂��B
�@ ���ʂƂ���Dpw_Common_ProfileResult�\���̂�mailAddrAuthResult�����o��
�@ DPW_PROFILE_AUTHRESULT_SEND���Z�b�g����܂��B
2. �T�[�o����xxx-yyyy�`���̃p�X���[�h�����͂��ꂽ���[���A�h���X�֑��M����܂��B
�@ �������Axxx�̓N���C�A���g���w�肵��mailAddrAuthVerification�Ayyyy�̓T�[�o��
�@ �������ꂽ�p�X���[�h�ł��B
3. �N���C�A���g�̓p�X���[�h���͉�ʂ�\�����A�p�X���[�h�̓�yyyy�̕����݂̂��
�@ �͂����܂��Bxxx�͏��߂���\�������܂��B(�킩��₷���̂��߂̔z��)
4. �ēxDpw_xx_SetProfileAsync�֐���mailAddrAuthPass�����o�ɐ������p�X���[�h��
�@ �w�肵�AmailAddr�����o�ɑO��Ɠ������[���A�h���X���w�肵�܂��B�w�肳�ꂽ�p
�@ �X���[�h�ƃ��[���A�h���X�������������ꍇ�ADpw_Common_ProfileResult�\���̂�
�@ mailAddrAuthResult�����o��DPW_PROFILE_AUTHRESULT_SUCCESS���Ԃ�܂��B
5. GTS�������̓o�g���^���[�ɐڑ�����Ƃ��͕K��Dpw_xx_SetProfileAsync�֐�����x
�@ �Ă�ł��������B����ɂ�胁�[���A�h���X�̗L��������31���ɉ�������܂��B
�@ ���̂Ƃ�Dpw_Common_ProfileResult�\���̂�mailAddrAuthResult�����o��
�@ DPW_PROFILE_AUTHRESULT_SUCCESS�ȊO�̒l���������ꍇ�̓p�X���[�h�Ȃǂ̃p��
�@ ���[�^�̎w�肪�Ԉ���Ă��܂��B���̏ꍇ�A���[���@�\�͖����ɂȂ�܂��B
�@ (�����̗F�B�R�[�h���ω������Ƃ��ɈȑO�Ɠ������[���A�h���X�ƃp�X���[�h��
�@ �M�����Ƃ��ɂ��̂悤�ɂȂ�\��������܂��B�ʏ�͂��蓾�܂���B)

�@�F�؃��[�������M���������͉��L�̒ʂ�ł��B
1. �l���o�^��(Dpw_xx_SetProfileAsync�֐�)�Ɏw�肷��Dpw_Common_Profile�\��
�@ �̂�mailAddr�����o�Ƀ��[���A�h���X��ݒ肵�AmailAddrAuthPass�����o��
�@ DPW_MAIL_ADDR_AUTH_START_PASSWORD��ݒ肵�AmailAddrAuthVerification�����o��
�@ 0�`999�̐�����ݒ肵�Ă���B
2. �F�؃��[���p�̖{������M�҂�language�����o�Ŏw�肵������ł��炩���ߓo�^��
�@ �Ă���B

�@�����ʒm���[�������M���������͉��L�̒ʂ�ł��B
1. ��M�҂����[���A�h���X��o�^���Ă���B
2. ��M�҂̃��[���A�h���X���L������(31��)���߂��Ă��Ȃ��B
3. ��M�҂����[����M�t���O(mailRecvFlag�����o)��
�@ DPW_PROFILE_MAILRECVFLAG_EXCHANGE���Z�b�g���Ă���B
4. �����ʒm���[���p�̖{������M�҂�language�����o�œo�^��������ł��炩���ߓo�^
�@ ���Ă���B

�@Dpw_xx_SetProfileAsync�֐��Ɏw�肷��Dpw_Common_Profile�\���̂�
mailAddrAuthPass��DPW_MAIL_ADDR_AUTH_DEBUG_PASSWORD(9999)���w�肷��ƃ��[���A
�h���X�̔F�؂������I�ɐ��������܂��B��ɔF�،��ʂ�
DPW_PROFILE_AUTHRESULT_SUCCESS�ɂȂ�܂��B�w�肵�����[���A�h���X���L���ɂȂ�
�܂��B�F�؃��[���͑��M����܂���B

�@���[���͖{���o�^���Ɏw�肵�������R�[�h�ő��M����܂��B���ɖ�肪�Ȃ�����f�t�H
���g����ύX���Ȃ��ł��������B

�@�T�[�o�Ƀ��[���̃t�H�[�}�b�g���ݒ肳��Ă��Ȃ��ꍇ�̓��[�������M����܂���B
�F�؃��[���A�����ʒm���[���̃��[���t�H�[�}�b�g���Ǘ��c�[���ł��炩���ߐݒ肵��
�����Ă��������B�ڂ����ݒ���@��$libdpw/man/dpwadmin_manual.txt���Q�Ƃ��Ă���
�����B

�@���M���ꂽ���[���̖{���̍Ō�ɑ��M�҂����ʂ��邽�߂̃T�|�[�g�pID�������I�ɕt
������܂��B


�y�������Ǘ��z

�@�{���C�u�����́A�ʐM����DWC���C�u�����̃A���P�[�^��p���܂��B
...Async�Ƃ������O�̔񓯊��֐����Ă񂾌�Ƀ��������m�ۂ��A�񓯊��֐����I�������
�������܂��B


�y����m�F�o�[�W�����z

�@�{���C�u�����̓���m�F�����Ă��郉�C�u�����̃o�[�W�����͈ȉ��̒ʂ�ł��B

�ETwlSDK-5_3_patch6-20091125-jp
�ETwlDWC-5_4-test1-20091108-jp
�ECW4DSi_1_1_patch3plus


�y�Ǘ��p�c�[���z

�@$libdpw/tools����NG���[�h�̓o�^��T�[�o�̏�Ԃ�ύX�ł���Ǘ��p�c�[�������^��
��Ă��܂��B
�{�c�[���̓f�o�b�O�T�[�o�ɂ̂ݐڑ��ł��܂��B


�y�f�o�b�O�T�[�o�֐ڑ�������_�~�[DNS�z

�@�����[�X�T�[�o�ւ̐ڑ��������I�ɕύX���A�f�o�b�O�T�[�o�ɐڑ�������_�~�[DNS��
�ݒu���Ă��܂��B
���LURL�Ō��݂̃_�~�[DNS��IP�A�h���X���m�F�ł��܂��B���̃A�h���X��Wi-Fi�ݒ��
DNS�A�h���X�Ƃ��ēo�^���Ă��������B

�@�@http://kiyoshi.servebeer.com/cgi-bin/getipaddr.cgi

�@���̃_�~�[DNS�̃A�h���X�͕s����ɕύX�����\��������܂��B


�y �f�B���N�g���}�b�v �z

$libdpw       DPW �� ROOT �f�B���N�g��
 ��
 ����common         ���ʃ��C�u����
 ��
 ����demos          �T���v��
 ��  ����bt_sample      -  �o�g���^���[���C�u�����̃T���v��
 ��  ����dbs            -  �f�o�b�O�X�N���[�����C�u����
 ��  ����tr_sample      -  ���E�ʐM�������C�u�����̃T���v��
 ��
 ����dpw_bt         �o�g���^���[���C�u����
 ��
 ����dpw_tr         ���E�ʐM�������C�u����
 ��
 ����man            �}�j���A��
 ��
 ����tool           �Ǘ��p�c�[��


�y �X�V���e �z

 beta27 (Apr. 28, 2010)
  �|�o�g���^���[�A�Ǘ��p�c�[���|
    �E�o�g���^���[�ɃA�b�v���[�h����|�P�����f�[�^�\���̂� 56 �o�C�g���� 60 �o�C�g�ɕύX���܂����B
      DpwAdmin.exe �����̕ύX�ɑΉ�����悤�ɕύX���܂����B(version 1.07)
      
 beta26 (Apr. 22, 2010)
  �|�Ǘ��p�c�[���|
    �EDpwAdmin.exe �ɕ\�������v���C���[�f�[�^�A���[�_�[�f�[�^�̖��O����
      UTF16 �G���R�[�h���ꂽ�������\������悤�ɕύX���܂����B(version 1.06)
      �������R�[�h�̃f�[�^�͕����������܂��̂ŁA�ȑO�̃f�[�^�����������ꍇ version 1.05 �ȑO�̃c�[�����g�p���ĉ������B

 beta25 (Apr. 1, 2010)
  �|���E�ʐM�����|
    �EDpw_Tr_Data �\���̂̕ϐ� trainerID �̌^�� u16 ���� u32 �ɕύX���A�\���̂̕ϐ��̏������ꕔ�ύX���܂����B
   
 beta24 (Jan. 29, 2010)
  �|���E�ʐM�����|
    �EDpw_Tr_Data �\���̂ɕϐ���3�o�C�g(u8 favoriete, u8 countryCount, u8 nature)��ǉ����܂����B
    
 beta23 (Dec. 28, 2009)
  �|�o�g���^���[�A���E�ʐM�����|
    �EDpw_Bt_Init�֐��ADpw_Tr_Init�֐��Ɉ�����ǉ����A
      �f�o�b�O�p/�����[�X�p�T�[�o���w�肷��悤�ɕύX���܂����B
    �EDpw_Bt_UploadPlayerAsync�֐��ADpw_Tr_UploadAsync�֐��ADpw_Tr_TradeAsync�֐��Ɉ�����ǉ����A
      �g�[�N�����󂯕t����悤�ɕύX���܂����B

 beta22 (Nov. 9, 2009)
  �|���ʁ|
    �ETwl���Ńv���C�x�[�g�ȃw�b�_���Q�Ƃ���悤�ɂȂ��Ă���
      �R���p�C���ł��܂���ł����̂ł��̖����C�����܂����B

 beta21 (Nov. 5, 2009)
  �|���ʁ|
    �ETwlSDK�ɑΉ����܂����B

 beta20 (Dec. 6, 2007)
  �|���E�ʐM�����|
    �EDpw_Tr_DownloadMatchDataExAsync�֐��y��Dpw_Tr_PokemonSearchDataEx�\���̂�
      �ǉ����A���R�[�h�Ō����ł���悤�ɂ��܂����B

 beta19 (Oct. 12, 2007)
  �|���E�ʐM�����|
    �E���[���@�\��ǉ����܂����B
  �|�Ǘ��p�c�[���|
    �E���[���t�H�[�}�b�g�ݒ�@�\��ǉ����܂����B

 beta18 (Oct. 9, 2007)
  �|���ʁ|
    �E�v���W�F�N�g�g�ݍ��ݗp�\�[�X�R�[�h�C���N���[�h������Ă����̂��C�����܂����B

 beta17 (Oct. 5, 2007)
  �|���E�ʐM�����A�o�g���^���[�|
    �EDpw_Tr_SetProfileAsync�֐��y��Dpw_Bt_SetProfileAsync�֐���ǉ����܂����B
  �|�Ǘ��p�c�[���|
    �E�؍��łɑΉ����܂����B

 beta16 (July. 27, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_ReturnAsync() ��ǉ����܂����B�|�P�������������ۂɂ�Dpw_Tr_ReturnAsync()
      ���g�p���ADpw_Tr_DeleteAsync()�͌����ς݂̃f�[�^���폜����ꍇ�ɂ̂ݎg�p���Ă��������B
  �|�Ǘ��p�c�[���|
    �E�|�P�����R�[�h�ƃ��j�R�[�h�̕ϊ����������Ȃ������̂��C�����܂����B
    �ENG�l�[���t���O�̕\���ɑΉ����܂����B
  �|�o�g���^���[���[���f�[�^�{���c�[���|
    �E�V�K�ǉ����܂����B

 beta15 (July. 7, 2006)
  �|�o�g���^���[�|
    �EDpw_Bt_Player�y��Dpw_Bt_Leader�̐��ʂ����Ă����������r�b�g�t���O�Ƃ��A
      ���ʃt���O��NG�l�[���t���O�����悤�ɂ��܂����B

 beta14 (July. 5, 2006)
  �|���E�ʐM�����|
    �E�T�[�o�[�A�b�v�f�[�g���ɃA�N�Z�X�����ꍇ�ADPW_TR_ERROR_SERVER_TIMEOUT�G���[�ɂȂ�
      �悤�ɂ��܂����B
  �|�o�g���^���[�|
    �E���[�����̍ő�lDPW_BT_ROOM_NUM_MAX��100�ɕύX���܂����B
    �E�T�[�o�[�A�b�v�f�[�g���ɃA�N�Z�X�����ꍇ�ADPW_BT_ERROR_SERVER_TIMEOUT�G���[�ɂȂ�
      �悤�ɂ��܂����B
  �|�Ǘ��p�c�[���|
    �E�ڑ�����T�[�o�[��I���ł���悤�ɂ��܂����B
    �E�o�g���^���[�̃��[�����̍ő�l��100�ɂȂ����ύX�ɑΉ����܂����B

 beta13 (June. 30, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_TradeAsync() �Ő������T�[�o�[����̃��X�|���X���󂯎��Ă��Ȃ������o�O��
      �C�����܂����B

 beta12 (June. 15, 2006)
  �|���E�ʐM�����|
    �EDPW_TR_ERROR_ILLEGAL_DATA, DPW_TR_ERROR_CHEAT_DATA, DPW_TR_ERROR_NG_POKEMON_NAME,
      DPW_TR_ERROR_NG_PARENT_NAME, DPW_TR_ERROR_NG_MAIL_NAME, DPW_TR_ERROR_NG_OWNER_NAME
      ���A Dpw_Tr_Data::name �����̒l�ɂ����Ƃ��ɕԂ�悤�ɂ��܂����B

 beta11 (June. 9, 2006)
  �|���E�ʐM�����|
    �EDPW_TR_ERROR_SERVER_TIMEOUT �̍ۂ̃G���[�\�����ׂ������܂����B
  �|�o�g���^���[�|
    �EDpw_Bt_UploadPlayerAsync() �̈��� win ��0�̂Ƃ��ɃA�T�[�g�ɂȂ��Ă����o�O
      ���C�����܂����B
    �EDPW_BT_ERROR_SERVER_TIMEOUT �̍ۂ̃G���[�\�����ׂ������܂����B

 beta10 (May. 23, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_Data::friend_key���폜���܂����B
  �|�o�g���^���[�|
    �E�T�[�o�[�̏������Ń��[���f�[�^������������Ȃ��Ȃ�܂����B
    �EDpw_Bt_Player::friend_key���폜���܂����B
    �EDpw_Bt_Leader::friend_key���폜���܂����B

 beta9 (May. 22, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_Init() �Ɉ��� friend_key ��ǉ����܂����B
    �EDpw_Tr_Data::friend_key��ǉ����܂����B
  �|�o�g���^���[�|
    �EDpw_Bt_Init() �Ɉ��� friend_key ��ǉ����܂����B
    �EDpw_Bt_Player::friend_key��ǉ����܂����B
    �EDpw_Bt_Leader::friend_key��ǉ����܂����B
    �E�A�b�v�f�[�g���ɁA���[�_�[��₪��l�����Ȃ��ꍇ�����[���̍X�V���s���Ă����o�O��
      �C�����܂����B

 beta8 (May. 19, 2006)
  �|�o�g���^���[�|
    �EDpw_Bt_PokemonData �̃T�C�Y��ύX���܂����B

 beta7 (May. 15, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_Data::gender��ǉ����܂����B
  �|�o�g���^���[�|
    �EDpw_Bt_Leader::gender��ǉ����܂����B
    �EDpw_Bt_Player::gender��ǉ����܂����B
    �EDpw_Bt_Player::playerName��NG�l�[�����܂܂��ꍇ�A�A�b�v�f�[�g���ɒu����������
      �悤�ɂȂ�܂����B
  �|�Ǘ��p�c�[���|
    �E�u���l�[�����X�g�̃A�b�v���[�h��ǉ����܂����B
    �E���p�Ґ��̕\���@�\��ǉ����܂����B
    �E���[���f�[�^�̎擾�@�\��ǉ����܂����B
    �E���[���f�[�^�̕ύX�@�\��ǉ����܂����B
    �E�����̃f�[�^��\������@�\��ǉ����܂����B
    
 beta6 (May. 8, 2006)
  �|���E�ʐM�����|
    �E::DPW_TR_GENDER_NONE �̃R�����g���C�����܂����B
    �EDpw_Tr_PokemonSearchData ��ǉ����܂����B
    �EDpw_Tr_Data::wantSimple �̌^�� Dpw_Tr_PokemonSearchData �֕ύX���܂����B
    �EDpw_Tr_DownloadMatchDataAsync() �̈�����ύX���܂����B
    �EDpw_Tr_Db_InitServer(), Dpw_Tr_Db_UpdateServer(), Dpw_Tr_Db_ShuffleServerData()
      �ŃG���[����������Ă����o�O���C�����܂����B
    �EDpw_Tr_Data::postDate, Dpw_Tr_Data::tradeDate �֓��錎���A1�}�C�i�X���ꂽ�l��
      �Ȃ��Ă����o�O���C�����܂����B
  �|�o�g���^���[�|
    �E::DPW_BT_RANK_NUM��ǉ����܂����B
    �EDpw_Bt_GetRoomNumAsync(), Dpw_Bt_DownloadRoomAsync(), Dpw_Bt_UploadPlayerAsync()
      �ŁA����rank�͈̔̓`�F�b�N������Ă����o�O���C�����܂����B
    �EDpw_Bt_Db_InitServer(), Dpw_Bt_Db_UpdateServer() �ŃG���[����������Ă����o�O��
      �C�����܂����B

 beta5 (April. 27, 2006)
  �|���ʁ|
  �@�EGameSpy�̃T�[�o�[�֐ڑ�����悤�ɕύX���܂����B
    �E�n���h�����Ă��Ȃ����������G���[�������������C�����܂����B
    �E�Ǘ��p�c�[���ilibdpw/tool/DpwAdmin.exe�j��ǉ����܂����B
  �|���E�ʐM�����|
    �EDPW_TR_ERROR_NG_TRAINER_NAME��p�~���ADPW_TR_ERROR_NG_PARENT_NAME��
      DPW_TR_ERROR_NG_OWNER_NAME��ǉ����܂����B
    �E��l�����iDpw_Tr_Data::name�j��NG���[�h���܂܂�Ă����ꍇ�A
      DPW_TR_ERROR_NG_OWNER_NAME���Ԃ�悤�ɂȂ�܂����B
  �|�o�g���^���[�|
    �EDPW_BT_ROOM_NUM_MAX��ǉ����܂����B�i�b��I��20�Ƃ��Ă��܂��B�j

 beta4 (April. 17, 2006)
  �|���ʁ|
    �EDPW���C�u�������g�p����O�ɁAWiFi�R�l�N�V�����ւ̃��O�C�����s���A�������O�A�E�g����
      �������T���v���֒ǉ����܂����B
  �|�o�g���^���[�|
    �EDpw_Bt_Leader::leaderMessage �̃T�C�Y��ύX���܂����B
    �EDpw_Bt_Player::leaderMessage, Dpw_Bt_Player::message �̃T�C�Y��ύX���܂����B
    �EDWC_GetDateTime()�̎g�p����T���v���֒ǉ����܂����B

 beta3 (April. 10, 2006)
  �|���E�ʐM�����|
    �EDpw_Tr_PokemonData �̃T�C�Y��ύX���܂����B
    �EDpw_Tr_Data::comment ���폜���܂����B
    �EDpw_Tr_Data::trainerID ��ǉ����܂����B
    �EDpw_Tr_Data::langCode ��ǉ����܂����B    
    �E����̃^�C�~���O�� Dpw_Tr_CancelAsync() ���ĂԂƎ~�܂��Ă����s����C�����܂����B
  �|�o�g���^���[�|
    �EDpw_Bt_PokemonData �̃T�C�Y��ύX���܂����B
    �EDpw_Bt_Leader::versionCode, Dpw_Bt_Leader::langCode, Dpw_Bt_Leader::countryCode, 
      Dpw_Bt_Leader::localCode ��ǉ����܂����B
    �EDpw_Bt_Player::versionCode, Dpw_Bt_Player::langCode, Dpw_Bt_Player::countryCode,
      Dpw_Bt_Player::localCode ��ǉ����܂����B
    �EDpw_Bt_Room::leader �̃T�C�Y��ύX���܂����B
    �E����̃^�C�~���O�� Dpw_Bt_CancelAsync() ���ĂԂƎ~�܂��Ă����s����C�����܂����B

 beta2 (March. 27, 2006)
  �|���ʁ|
    �EHTTP���C�u�����̃��C���[�ŃG���[���N�������Ƃ��ɁA�G���[���N���A�ł��Ă�
      �Ȃ������s����C�����܂����B
    �E�֐��̃R�����g�ɁA�Ԃ��Ă���G���[�̎�ނ�ǉ����܂����B
    �EDpw_Bt_PokemonData�̑傫����172�o�C�g�ɕύX���܂����B
  �|���E�ʐM�����|
    �EDpw_Tr_DownloadMatchDataAsync() �Ō��������f�[�^�������_���ɕύX���܂�
      ���B�قȂ�PID�ł́A�قȂ錟�����ʂƂȂ�܂��B
    �EDpw_Tr_Data::trainerType �����������悤�ɂ��܂����B
    �EDpw_Tr_Data::mail ���폜���܂����B
    �EDpw_Tr_PokemonData�̑傫����172�o�C�g�ɕύX���܂����B
    �EDpwTrError �ŁANG���[�h�Ɋւ���G���[���ו������܂����B
    �E���E�ʐM�����̌������̕ύX����֐� Dpw_Tr_Db_ShuffleServerData() ��ǉ���
    �@�܂����B

 beta1 (March. 17, 2006)

    �E���ł������[�X���܂����B