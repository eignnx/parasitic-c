CC=zig cc
CC_FLAGS=-g -std=c11

PCC=.\paracc0.exe

tgt/paracc-HEAD.exe: tgt/paracc.c tgt/parse.h tgt/lex.h tgt/cstr_buf.h tgt/cheats.h
	$(CC) $(CC_FLAGS) tgt/paracc.c -o $@

tgt/cheats.h: cheats.h tgt
	$(CP) cheats.h tgt

tgt/paracc.c: paracc.parac.c parse.parac.h lex.parac.h cstr_buf.parac.h cheats.h tgt
	$(PCC) .\paracc.parac.c
	$(MV) .\paracc.c tgt

tgt/parse.h: parse.parac.h lex.parac.h cheats.h tgt
	$(PCC) .\parse.parac.h
	$(MV) .\parse.h tgt

tgt/lex.h: lex.parac.h cheats.h tgt
	$(PCC) .\lex.parac.h
	$(MV) .\lex.h tgt

tgt/cstr_buf.h: cstr_buf.parac.h cheats.h tgt
	$(PCC) .\cstr_buf.parac.h
	$(MV) .\cstr_buf.h tgt

tgt:
	$(MKDIR) tgt

clean:
	$(RMDIR) tgt

ifeq ($(OS),Windows_NT)
    RM = cmd /C del /Q /F
    RMDIR = cmd /C rmdir /Q /S
	MV = cmd /C move
	CP = cmd /C copy
	MKDIR = cmd /C mkdir
else
    RM = rm -f
    RMDIR = rm -f -r
	MV = mv
	CP = cp
	MKDIR = mkdir
endif