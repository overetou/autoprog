#Work title
TARGET=autoprog

#Directories
OBJ_DIR=obj
SRC_DIR=src
INC_DIRS=includes
LIB_DIRS=

#Compilation related indications
CC=gcc
CFLAGS=-Wall -Wextra -Werror
LIBS=

#From there we use the info to compile everything.
SRC_FILE_NAMES = main.c tidy_prototypes.c common.c word_tree.c
SOURCES=$(addprefix $(SRC_DIR)/, $(SRC_FILE_NAMES))
OBJS:=$(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
INC_FLAGS=$(addprefix -I,$(INC_DIRS))
LIBIDR_FLAGS=$(addprefix -L,$(LIB_DIRS))
CFLAGS+=$(INC_FLAGS)
LIBS_FLAGS=$(addprefix -l,$(LIBS))
VPATH=$(SRC_DIR)

.phony: $(TARGET) clean re

$(TARGET): $(OBJS)	
	@$(CC) $(CFLAGS) -MMD $(OBJS) -o $(TARGET) $(LIBIDR_FLAGS) $(LIBS_FLAGS)
	@echo "\033[0;32mCompilation succeeded.\033[0m"

$(OBJS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(INC_DIRS)/* |$(OBJ_DIR) 
	@$(COMPILE.c) $(OUTPUT_OPTIONS) -o $@ $<
	@echo "Compiled 1 file."

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

clean:
	rm -rf $(OBJS) $(TARGET)

re: clean $(TARGET)

debian-deps:
	echo "No dependencies"

test:
	@gcc -Wall -Wextra -Werror test.c src/tidy_prototypes.c src/common.c src/word_tree.c -I includes -o test_exec
	./test_exec