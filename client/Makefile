###############################################################################
# DESSAUX                           MAKEFILE                      O7 Oct 2008 #
#   Julien                                                                    #
###############################################################################

VERSION = DEBUG

TGT = client
SRC = agrawala.c main.c ricart_rpc.c

SRCDIR = ./src
INCDIR = ./include
OBJDIR = ./obj

CPP = gcc -Wall
# -ansi
# -pedantic
LDLIBS = 
CFLAGS += -I$(INCDIR)/

DEBUG_CFLAGS = -g
PROFILE_CFLAGS = -pg
RELEASE_CFLAGS = -O2

ifeq "$(VERSION)" "PROFILE"
    CFLAGS += $(PROFILE_CFLAGS)
endif
ifeq ($(VERSION), DEBUG)
    CFLAGS += $(DEBUG_CFLAGS)
endif
ifeq ($(VERSION), RELEASE)
    CFLAGS += $(RELEASE_CFLAGS)
endif


#----- Suffix rules ----------------------------------------------------------
$(OBJDIR)/%.o : %.c
	$(CPP) -MD $(CFLAGS) -c $< -o $@

#----- Dependencies ----------------------------------------------------------
vpath %.c $(SRCDIR)/

SRCS = $(addprefix $(SRCDIR)/, $(SRC))
OBJ = $(addprefix $(OBJDIR)/, $(addsuffix .o, $(basename $(SRC))))

all: $(TGT)

$(TGT): $(OBJ)
	$(CPP) $(CFLAGS) -o $@ $(OBJ) $(LDLIBS)

#----- Other stuff -----------------------------------------------------------

archive:
	-rm ../$(TGT).tar.gz
	tar cvzf ../$(TGT).tar.gz ./

doc:
	doxygen

clean:
	-rm obj/* $(TGT) *.log core

