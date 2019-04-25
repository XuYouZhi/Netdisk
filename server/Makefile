######### Makefile COMMON ########  
EXTENSION = c
CC = gcc
EXE =server
SUBDIR = src
OBJ = obj

#LDFLAGS +=
#CFLAGS += -g -Wall
CXX_SOURCES = $(foreach dir,$(SUBDIR), $(wildcard $(dir)/*.$(EXTENSION)))
CXX_OBJECTS = $(patsubst  %.$(EXTENSION), $(OBJ)/%.o, $(notdir $(CXX_SOURCES)))

$(EXE): $(CXX_OBJECTS)
	$(CC) $(CXX_OBJECTS) -o $(EXE)	-lmysqlclient 
$(OBJ)/%.o: $(SUBDIR)/%.$(EXTENSION)
	$(CC) -c $(CPPFLAGS) $< -o $@
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEP_FILES)
endif

.PHONY:clean
clean:
	-rm  -f  $(CXX_OBJECTS) $(EXE)
