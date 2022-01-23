#**********************************************************************
# Author:		John Walker
# Email			jmgwalker@triumf.ca
# Date:			2019/09/25
#********************************************************************** 
INCDIR= $(shell pwd)/include
SRCDIR= $(shell pwd)/src
OBJDIR= $(shell pwd)/obj
BINDIR= $(shell pwd)/bin

VPATH = $(SRCDIR)

CFLAGS=-c -g -Wall `root-config --cflags` -I${INCDIR}
LDFLAGS=`root-config --glibs` -lHistPainter

TARGET1=field_to_csv.cpp
TARGET2=ptf_analysis.cpp

EXECUTABLE1=$(TARGET1:%.cpp=$(BINDIR)/%.app)
EXECUTABLE2=$(TARGET2:%.cpp=$(BINDIR)/%.app)

FILES= $(wildcard $(SRCDIR)/*.cpp)
SOURCES=$(FILES)

OBJECTS = $(FILES:$(SRCDIR)/%.cpp=${OBJDIR}/%.o)

OBJ1=$(TARGET1:%.cpp=${OBJDIR}/%.o) $(OBJECTS)
OBJ2=$(TARGET2:%.cpp=${OBJDIR}/%.o) $(OBJECTS)

all: MESSAGE $(EXECUTABLE1) $(EXECUTABLE2)

MESSAGE:
	@echo '**********************************************************************'
	@echo '* Author:     John Walker                                            *'
	@echo '* Email       jmgwalker@triumf.ca                                    *'
	@echo '* Date:       2019/09/25                                             *'
	@echo '**********************************************************************'

$(EXECUTABLE1): $(OBJECTS) $(OBJ1)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(EXECUTABLE2): $(OBJECTS) $(OBJ2)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

clean:
	- $(RM) $(BINDIR)/* $(OBJDIR)/*
