cd %GFL_MAKE_DIR:/=\%
make
if ERRORLEVEL 1 goto MAKE_ERR
xcopy /Y /D /I include\*.h %GFL_PROJECT_DIR:/=\%\include
xcopy /Y /D /I install\ARM9-TS\Release\*.a %GFL_PROJECT_DIR:/=\%\install\ARM9-TS\Release
goto END

:MAKE_ERR
echo GFLib��make�Ɏ��s���܂����I

:END
cd %PROJECT_PROGDIR:/=\%
