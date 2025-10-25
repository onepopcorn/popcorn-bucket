# PATH TO DOSBOX EXECUTABLE
DOSBOX := $(DOSBOX_PATH)

# PATH TO DJGGP COMPILER
# if DJGPP_PATH env variable is set it will use it, if not it will default to gcc.
# DGJPP comes with a setenv bash script, there's no need to set DJGPP_PATH var if
# the setenv script is ran before runtin make
CC := $(if $(DJGPP_PATH),$(DJGPP_PATH)/bin/i586-pc-msdosdjgpp-gcc.exe,gcc)