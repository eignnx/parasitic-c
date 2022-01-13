CC=zig cc
CC_FLAGS=-g -std=c11

PCC=.\paracc-v0.0.0.exe

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

test: tgt/paracc-HEAD.exe
	tgt\paracc-HEAD.exe --test

ctest: bootstrap/paracc-from-c.exe
	bootstrap\paracc-from-c.exe --test

bootstrap: bootstrap/paracc-from-c.exe
	bootstrap\paracc-from-c.exe .\paracc.parac.c
	$(MV) .\paracc.c bootstrap
	$(MV) .\parse.h bootstrap
	$(MV) .\lex.h bootstrap
	$(MV) .\cstr_buf.h bootstrap
	$(CP) .\cheats.h bootstrap
	$(CC) $(CC_FLAGS) bootstrap\paracc.c -o paracc-v0.0.0.exe

bootstrap/paracc-from-c.exe: paracc.parac.c parse.parac.h lex.parac.h cstr_buf.parac.h cheats.h
	$(MKDIR) bootstrap
	$(CC) $(CC_FLAGS) paracc.parac.c -o bootstrap\paracc-from-c.exe

clean:
	$(RMDIR) bootstrap
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