������������������������������������������������������������������������������
��                                                                          ��
��  DS�v���e�N�g                                                            ��
��                                                                          ��
��  Mar  5, 2010                                                            ��
��                                                                          ��
��  Copyright NetAgent Co., Ltd.                                            ��
��                                                                          ��
������������������������������������������������������������������������������

���͂��߂�

  DS�v���e�N�g�́A�j���e���h�[DS �Ή��̃}�W�R���A�y�сA�G�~�����[�^��ł� DS 
  �A�v���P�[�V�����̗��p�𐧌����邽�߂̃~�h���E�F�A�v���O�����ł��B

�@�y���ӁzVersion 1.22���t�H���_�\�����ς���Ă���܂��̂ł����ӂ��������B

���C���X�g�[�����@
  
  �p�b�P�[�W��C�ӂ̏ꏊ�ɓW�J���ADSProtect/include/AMProtect.h �y�� 
  DSProtect/lib/ARM9-TS/Rom/AMProtectR2a.a �����g�p�̊J�����ɓ������Ă��������B
  
  �y���Ӂz�p�b�P�[�W�� TwlSDK �ɏ㏑���R�s�[���Ȃ��ł��������B
  
  �T���v���v���O�������r���h����ꍇ�ɂ́A�p�b�P�[�W�̃f�B���N�g���\�����ێ���
  ���܂܁ADSProtect/build/demos/DSProtect �ɂĈȉ��̃R�}���h�����s���Ă��������B
  
  % make TWLSDK_PLATFORM=NITRO NITRO_FINALROM=TRUE

  NitroSDK���g�p���ăr���h����ꍇ�͈ȉ��̃R�}���h�����s���Ă��������B

  % make -f Makefile.ntr NITRO_FINALROM=TRUE

�@�y���ӁzCodeWarrior�������g���̕��́ACodeWarrior�p�v���W�F�N�g�t�@�C������
          ���p���������BCodeWarrior�p�v���W�F�N�g�t�@�C���́A
          DSProtect/build/demos/DSProtect/CW/ �ȉ��ɔz�u����Ă��܂��B
          �Ȃ��ATwlSDK�p�v���W�F�N�g�t�@�C���́ATwlSDK 5.1 �p�̐ݒ�ɂȂ��Ă�
          ��܂��BTwlSDK 5.2 �����g���̕��́ATwl�T�|�[�g�y�[�W���Z�p���y�[
          �W�L�ڂ̈ڍs���@���Q�Ƃ̂����A�v���W�F�N�g�ݒ��ύX���Ă��������B

���v���e�N�V�����ɂ���

  �P��ނ̃}�W�R�����o���C�u�����𓯍����Ă��܂��B
  ���C�u�����̎������@�Ȃǂ́A�uDS�v���e�N�g ���[�U�[�Y�}�j���A���v�����m�F��
  �������B


���t�@�C�����C�A�E�g

  DSProtect
  ��bin
  ����ARM9-TS
  ��  ��rom
  ��      NITRO_Overlay.srl             �c  DS�p�T���v��ROM
  ��      NITRO_Overlay_thumb.srl       �c  DS�p�T���v��ROM (thumb ��)
  ��      TWL_Hybrid_Overlay.srl        �c  DSi�p Hybrid �T���v��ROM
  ��      TWL_Hybrid_Overlay_thumb.srl  �c  DSi�p Hybrid �T���v��ROM (thumb ��)
  ��build
  ����demos
  ��  ��DSProtect                       �c  �T���v���v���O����
  ��      �d�d
  ��docs
  ����DSProtect
  �b    index.html                      �c  DS�v���e�N�g ���[�U�[�Y�}�j���A��
  �b    �d�d
  ��include
  ��  AMProtect.h                       �c  �w�b�_�t�@�C��
  ��lib
  ����ARM9-TS
  ��  ��rom
  ��      AMProtectR2a.a                �c  ���C�u�����t�@�C��
  ��man
  ����ja_JP
  ��  ��tools                           �c �e��c�[���̃��t�@�����X�}�j���A���B
  �b      �d�d
  ��tools                                
    ��bin                               �c �e��c�[���B�ڂ����́uDS�v���e�N�g
        �d�d                               ���[�U�[�Y�}�j���A�� 5.�����c�[����
                                           ���āv�y�у��t�@�����X�}�j���A��
                                           ���������������B


����������
  ���������́uDS�v���e�N�g���[�U�[�Y�}�j���A���v�́u���������v���������������B

