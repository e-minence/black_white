cd %PROJECT_ROOT:/=\%
find . -name "*.h" -or -name "*.c" -or -name "*.cpp" -or -name "*.dat" -or -name "*.naix" -or -name "*.sadl" -or -name "*.ev" | ctags -R --c-kinds=+x --fields=+iaS --extra=+q  -h +.sadl --langmap=c:+.sadl --totals -L -



