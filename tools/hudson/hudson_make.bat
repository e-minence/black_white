@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

SET PATH_TOOLS=%PROJECT_ROOT%\tools\hudson

REM �f�o�C�X��ON/OFF �͖��Ȃ����삷��B
REM devcon enable *HID*

REM IPMSG�͂Ȃ���hudson��exit 0���E���Ă��܂��B

REM ================================
REM ���[�U�[�l�[���ݒ�
REM ================================
SET USERNAME=hudson
@echo USERNAME = %USERNAME%

REM ================================
REM ���� make
REM ================================
@echo ***** make *****
make

REM ����I������
if %ERRORLEVEL% == 0 goto END:

REM ================================
REM �G���[����
REM ================================
REM clean���Ă���-> �ʒm���x��Ă��܂��̂ň�[�R�����g�A�E�g
REM make clean

REM prog�N���[��������
cd prog
make clean
cd ..
make

REM ����I������
if %ERRORLEVEL% == 0 goto END:

REM ================================
REM �G���[�I��
REM ================================
@echo errorlevel = %ERRORLEVEL%

exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:END
  @echo hudson make end


