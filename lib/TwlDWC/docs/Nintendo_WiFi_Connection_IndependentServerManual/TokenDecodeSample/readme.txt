========================================================================================
  Nintendo Wi-Fi Connection
    �Ǝ��Q�[���T�[�o�p�F�؃g�[�N���������v���O�����T���v�� Ver0.1
========================================================================================

��������A����ѕK�v��Perl���W���[��

�EPerl v5.8.6�ɂē���m�F���s���Ă��܂��B���LPerl���W���[����������΁A���̃o�[�W������
  �����삷��Ǝv���܂��B
�E���s�ɂ́A���L��Perl���W���[�����K�v�ł��B���ꂼ�ꉺ�L�̃o�[�W�����œ���m�F���s���Ă�
  �܂����A���̃o�[�W�����ł����삷��Ǝv���܂��B

  - Crypt::CBC          v2.15
  - Crypt::Rijndael_PP  v0.04
  - Digest::MD5         v2.33
  - MIME::Base64        v3.05


�����ӓ_

�E������128bit�ł��邱�Ƃɒ��ӂ��Ă��������BCrypt::CBC����Rijndael_PP�𗘗p����ꍇ�A
  Rijndael_PP�͌������f�t�H���g�l��256bit�Ƃ��ď������܂��B�����������邽�߁A���̃T��
  �v���ł́ARijndael_PP.pm��Rijdael_PP_128.pm�Ƃ��ăR�s�[���A���L�̕ύX�����������̂��g
  �p���Ă��܂��B

  ��Rijndael_PP_128.pm �ύX�_�iRijndael_PP.pm v0.04�̏ꍇ�j
    3c3
    < package Crypt::Rijndael_PP;
    ---
    > package Crypt::Rijndael_PP_128;
    2340c2340
    < sub DEFAULT_KEYSIZE() { 256 }
    ---
    > sub DEFAULT_KEYSIZE() { 128 }


���ύX����

Ver0.1  2007/3/13
  Wii�p�̃g�[�N���ɑΉ�

Ver0.0  2006/4/6
  ����

