@echo off
REM =================================================================
REM hudson�p��make�o�b�`
REM �p�r    : prompt.bat cmd hudson_make.bat ��hudson����Ăяo��
REM �����  : hosaka_genya
REM ���t    : 2009/11/24
REM =================================================================

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
@echo errorlevel = %ERRORLEVEL%

REM �I������
if %ERRORLEVEL% == 0 goto END:

REM ================================
REM �G���[����
REM ================================
REM make clean���čēxmake
@echo ***** make clean *****
make clean
make
REM �G���[�R�[�h���ہX�Ԃ��iclean�ł�make���ʂ�Ȃ�������hudson�ɒʒm)
exit %ERRORLEVEL%

REM ================================
REM ����I��
REM ================================
:END
  @echo hudson make end


