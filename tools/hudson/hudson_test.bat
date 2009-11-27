@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

REM hudson�Œ�`�������ϐ��𗘗p
SET PROJECT_ROOT=%WORKSPACE%
SET PROJECT_PROGDIR=%PROJECT_ROOT%/prog/

REM ##################################################
REM		�p�X�̒ǉ�
REM ##################################################
PATH=c:\tools;%PROJECT_ROOT%;%PROJECT_ROOT%\tools;c:\tools\subversion\bin;c:\cygwin\bin;c:\tools\vim;C:\tools;%NITROSDK_ROOT%\tools\bin;%NITROSYSTEM_ROOT%\tools\bin;%PATH%;

REM main.srl
SET PATH_MAIN_SRL=%PROJECT_PROGDIR%bin/ARM9-TS/Release/main.srl

REM �Ō��Printf����̃^�C���A�E�g(�b) >> hudson�Ŏw��
SET TIMEOUT_DISP=%1 

REM �Ώ�DS�̃V���A���i���o�[ >> hudson�Ŏw��
SET SERIAL_NO=%2

REM wbrom:05093474
REM hosaka:05113644

REM �I�����o������(���s���������ɐ擪���画��)
SET ABORT_STRING="  **** ASSERTION FAILED ! ****"

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

@echo loadpath = %PATH_MAIN_SRL%
@echo TIMEOUT_DISP = %TIMEOUT_DISP%
@echo serial_no = %SERIAL_NO%

REM ================================
REM �u�[�g
REM ================================

REM buryarg %PATH_MAIN_SRL% 1

loadrun -t %TIMEOUT_DISP% -s %SERIAL_NO% -a %ABORT_STRING% %PATH_MAIN_SRL%
echo ErrorLevel = %ERRORLEVEL%

REM �f�o�C�X��������Ȃ�����
if %ERRORLEVEL% == -1 goto _ERROR_END:

REM �^�C���A�E�g����
if %ERRORLEVEL% == 205 goto _END:

REM �w�蕶���񌟒m�ŏI��
if %ERRORLEVEL% == 200 goto _ERROR_END:
  
REM ================================
REM �w�蕶���񌟒m�I��
REM ================================
:_ERROR_END
  REM �G���[�R�[�h���ہX�Ԃ�
  @echo hudson test is FAILED .
  pause
  exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:_END
  @echo hudson test is SUCCESS .
  REM devcon disable *HID*
  pause
  exit 0



