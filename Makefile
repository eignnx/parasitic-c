CC=zig cc
CC_FLAGS=-g -std=c11 -Werror -Wpedantic -Wall

# The most recent stable version of the compiler
MOST_STABLE_VERSION=0.0.3
NEXT_STABLE_VERSION=0.0.4
PCC=.\paracc-v$(MOST_STABLE_VERSION).exe

PARACC_PARAC_SRCS=paracc.parac parse.parac lex.parac cstr_buf.parac list.parac
PARACC_C_SRCS=cheats.h

head: tgt/paracc-HEAD.exe

tgt/paracc-HEAD.exe: $(PARACC_PARAC_SRCS) $(PARACC_C_SRCS) tgt
	$(PCC) .\paracc.parac
	$(MV) .\paracc.c tgt
	$(MV) .\parse.c tgt
	$(MV) .\lex.c tgt
	$(MV) .\cstr_buf.c tgt
	$(MV) .\list.c tgt
	$(CP) .\cheats.h tgt
	$(CC) $(CC_FLAGS) tgt/paracc.c -o $@

tgt:
	$(MKDIR) tgt

test: tgt/paracc-HEAD.exe
	tgt\paracc-HEAD.exe --test

ctest: bootstrap/paracc-from-c.exe
	bootstrap\paracc-from-c.exe --test

release: $(PARACC_PARAC_SRCS) $(PARACC_C_SRCS)
	$(MKDIR) releases\$(NEXT_STABLE_VERSION)
	$(PCC) .\paracc.parac
	$(MV) .\paracc.c releases\$(NEXT_STABLE_VERSION)
	$(MV) .\parse.c releases\$(NEXT_STABLE_VERSION)
	$(MV) .\lex.c releases\$(NEXT_STABLE_VERSION)
	$(MV) .\cstr_buf.c releases\$(NEXT_STABLE_VERSION)
	$(MV) .\list.c releases\$(NEXT_STABLE_VERSION)
	$(CP) .\cheats.h releases\$(NEXT_STABLE_VERSION)
	$(CC) $(CC_FLAGS) releases\$(NEXT_STABLE_VERSION)\paracc.c -o paracc-v$(NEXT_STABLE_VERSION).exe

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