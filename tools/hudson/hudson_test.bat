@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

REM main.srl
SET PATH_MAIN_SRL=%PROJECT_PROGDIR%/bin/ARM9-TS/Release/main.srl

REM ���s�^�C���A�E�g(�b) >> hudson�Ŏw��
REM SET TIMEOUT_EXEC=60 

REM �Ώ�DS�̃V���A���i���o�[ >> hudson�Ŏw��
REM SET SERIAL_NO=05093474

REM �I�����o������(���s���������ɐ擪���画��)
SET ABORT_STRING="  **** ASSERTION FAILED ! ****"

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM ================================
REM �u�[�g
REM ================================
loadrun -T %TIMEOUT_EXEC% -a %ABORT_STRING% -s %SERIAL_NO% %PATH_MAIN_SRL%
echo ErrorLevel = %ERRORLEVEL%

REM �^�C���A�E�g����
if %ERRORLEVEL% == 205 goto _TIMEOUT:

REM �w�蕶���񌟒m�ŏI��
if %ERRORLEVEL% == 200 goto _ABORT_STRING:

REM ================================
REM �^�C���A�E�g
REM ================================
:_TIMEOUT
  @echo timeout
  goto _END:
  
REM ================================
REM �w�蕶���񌟒m�I��
REM ================================
:_ABORT_STRING
  REM �G���[�R�[�h���ہX�Ԃ�
  exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:_END
  @echo hudson test is SUCCESS .
  exit 0

