@echo off
echo 正在执行清理......

del /f/s/q *.obj
del /f/s/q *.lst
del /f/s/q *.lnp
del /f/s/q *.m51
del /f/s/q *.htm
del /f/s/q *.__i
del /f/s/q *.uvgui.*
del /f/s/q *.

echo 清理完毕！
echo. & pause