ECHO OFF
MODE COM3 1200
D:
CD\
CLS

:INICIO

@COPY COM3 temp.log
CLS
ECHO Ultima Temperatura Registrada
TYPE temp.log

GOTO INICIO
