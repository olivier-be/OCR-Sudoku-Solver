# Compilateur, assemleur, éditeur des liens
export CC = gcc
export AS = gcc
export LD = gcc
export AR = ar

# Paramètres globaux de compilation (C, assembleur, liens)
export CFLAGS = -W -Wall -Wextra -pedantic -std=c99 -g3
export SFLAGS = -W -Wall -Wextra
export LFLAGS = -L$(LIB_DIR)

# Dossier des fichier intermédiaires et binaires
export OBJ_DIR = ./obj
export BIN_DIR = ./bin
export LIB_DIR = ./lib

# Dossier des fichier sources des sous projets
export SOLVER_DIR = ./solver
export GUI_DIR = ./gui
export AI_DIR = ./network
export UTILS_DIR = ./utils

# Ensembles des dossier accessibles
VPATH = \
	$(shell find $(AI_DIR) -type d) 		\
	$(shell find $(GUI_DIR) -type d) 		\
	$(shell find $(SOLVER_DIR) -type d)		\
	$(shell find $(UTILS_DIR) -type d)

###############################################################################
#                                                                             #
#                                                                             #
#                               UTILS                                         #
#                                                                             #
#                                                                             #
###############################################################################
UTILS_OBJ_DIR = $(OBJ_DIR)/utils
UTILS_LIB = $(LIB_DIR)/libutils.a

UTILS_SRC = $(shell find $(UTILS_DIR) -name *.c)
UTILS_OBJ = $(addprefix $(UTILS_OBJ_DIR)/, $(notdir $(UTILS_SRC:.c=.c.o)))

$(UTILS_LIB): $(UTILS_OBJ)
	@echo "AR 			$@"
	@$(AR) -rcs $@ $^

$(UTILS_OBJ_DIR)/%.c.o: %.c
	@echo "CC			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) -I$(UTILS_DIR)

###############################################################################
#                                                                             #
#                                                                             # 
#                                SOLVER                                       #
#                                                                             #
#                                                                             #
###############################################################################
SOLVER_OBJ_DIR = $(OBJ_DIR)/solver
SOLVER_LIB = $(LIB_DIR)/libsolver.a
SOLVER_EXE = $(BIN_DIR)/solver

SOLVER_SRC = $(shell find $(SOLVER_DIR) -name *.c)
SOLVER_LIB_SRC = $(filter-out $(SOLVER_DIR)/main.c, $(SOLVER_SRC))

SOLVER_LIB_OBJ = $(addprefix $(SOLVER_OBJ_DIR)/, \
				 $(notdir $(SOLVER_LIB_SRC:.c=.c.o)))

$(SOLVER_LIB): $(SOLVER_LIB_OBJ)
	@echo "AR 			$@"
	@$(AR) -rcs $@ $^

$(SOLVER_EXE): $(SOLVER_LIB_OBJ)
	@echo "LD 			$@"
	@$(LD) -o $@ $^ $(SOLVER_DIR)/main.c $(LFLAGS) -lutils

$(SOLVER_OBJ_DIR)/%.c.o: %.c
	@echo "CC 			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) 


###############################################################################
#                                                                             #
#                                                                             #
#                                  AI                                         #
#                                                                             #
#                                                                             #
###############################################################################
AI_OBJ_DIR = $(OBJ_DIR)/ai
AI_EXE = $(BIN_DIR)/network

AI_SRC = $(IA_DIR)/network.c
AI_OBJ = $(addprefix $(AI_OBJ_DIR)/, $(notdir $(AI_SRC:.c=.c.o)))

$(AI_EXE): $(AI_OBJ)
	@echo "LD 			$@"
	@$(LD) -o $@ $^ -lm

$(AI_OBJ_DIR)/%.c.o: %.c
	@echo "CC 			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) -I$(AI_DIR) -I$(UTILS_DIR)

###############################################################################
#                                                                             #
#                                                                             #
#                                   GUI                                       #
#                                                                             #
#                                                                             #
###############################################################################
GUI_OBJ_DIR = $(OBJ_DIR)/gui
GUI_EXE = $(BIN_DIR)/gui

GUI_SRC = $(shell find $(GUI_DIR) -name *.c)
GUI_OBJ = $(addprefix $(GUI_OBJ_DIR)/, $(notdir $(GUI_SRC:.c=.c.o)))

$(GUI_EXE): $(GUI_OBJ)
	@echo "LD			$@"
	@$(LD) -o $@ $^ $(LFLAGS) `pkg-config --libs gtk+-3.0` -lsolver \
		-lutils `pkg-config --libs gdk-3.0` -lm

$(GUI_OBJ_DIR)/%.c.o: %.c
	@echo "CC			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) `pkg-config --cflags gtk+-3.0` \
		`pkg-config --cflags gdk-3.0` -I$(UTILS_DIR)

###############################################################################
#                                                                             #
#                                                                             #
#                                 RULES                                       #
#                                                                             #
#                                                                             #
###############################################################################
all: dirs $(UTILS_LIB) $(SOLVER_LIB) $(AI_EXE) $(GUI_EXE) $(SOLVER_EXE)

clean:
	@echo "CLEAN"
	@rm -rf $(OBJ_DIR)

mrproper:
	@echo "MRPROPER"
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) $(GUI_OBJ_DIR) $(IMAGE_OBJ_DIR)\
		$(SOLVER_OBJ_DIR) $(AI_OBJ_DIR) $(UTILS_OBJ_DIR)

