# Compilateur, assemleur, éditeur des liens
export CC = gcc
export AS = gcc
export LD = gcc
export AR = ar

# Paramètres globaux de compilation (C, assembleur, liens)
export CFLAGS = -W -Wall -Wextra -std=c99 -g3 -DBUILD_GUI -mavx2
export SFLAGS = -W -Wall -Wextra
export LFLAGS = -L$(LIB_DIR)

# Dossier des fichier intermédiaires et binaires
export OBJ_DIR = $(shell pwd)/obj
export BIN_DIR = $(shell pwd)/bin
export LIB_DIR = $(shell pwd)/lib

# Dossier des fichier sources des sous projets
export SOLVER_DIR = ../solver
export GUI_DIR = ../gui
export AI_DIR = ../network
export UTILS_DIR = ../utils

# Binaire et lib statiques
export UTILS_LIB = $(LIB_DIR)/libutils.a
export SOLVER_LIB = $(LIB_DIR)/libsolver.a
export SOLVER_EXE = $(BIN_DIR)/solver
export SOLVER_HEXA_EXE = $(BIN_DIR)/solver_hexa
export GUI_EXE = $(BIN_DIR)/gui
export AI_EXE = $(BIN_DIR)/network
export AI_LIB = $(LIB_DIR)/libnetwork.a

all: dirs utils solver gui
utils: dirs FORCE
	@$(MAKE) --no-print-directory -C utils all

solver: dirs utils FORCE
	@$(MAKE) --no-print-directory -C solver all

ai: dirs FORCE
	@$(MAKE) --no-print-directory -C network all

gui: dirs ai solver utils FORCE
	@$(MAKE) --no-print-directory -C gui all

clean:
	@echo "CLEAN"
	@rm -rf $(OBJ_DIR)

mrproper:
	@echo "MRPROPER"
	@rm -rf $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) $(LIB_DIR) 

FORCE:
