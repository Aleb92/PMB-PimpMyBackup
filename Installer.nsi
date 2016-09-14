!define VERSION "1.0.0"
!define NAME "NOME_FIGO"
Name "${NAME} ${VERSION}"
OutFile "${NAME} Installer - ${VERSION}.exe"

Page license
Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles
