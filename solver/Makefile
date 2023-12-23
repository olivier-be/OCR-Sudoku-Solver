LIB_SRC = solver.c hexa.c
SRC_HEXA = hexa.c mainhexa.c
SRC = solver.c main.c

LIB_OBJ = $(addprefix $(OBJ_DIR)/solver_, $(notdir $(LIB_SRC:.c=.c.o)))
OBJ = $(addprefix $(OBJ_DIR)/solver_, $(notdir $(SRC:.c=.c.o)))
OBJ_HEXA = $(addprefix $(OBJ_DIR)/solver_, $(notdir $(SRC_HEXA:.c=.c.o)))

all: $(SOLVER_LIB) $(SOLVER_EXE) $(SOLVER_HEXA_EXE)

$(SOLVER_LIB): $(LIB_OBJ)
	@echo "AR 			$@"
	@$(AR) -rcs $@ $^

$(SOLVER_EXE): $(OBJ)
	@echo "LD 			$@"
	@$(LD) -o $@ $^ $(LFLAGS) -lutils

$(SOLVER_HEXA_EXE): $(OBJ_HEXA)
	@echo "LD			$@"
	@$(LD) -o $@ $^ $(LFLAGS) -lutils

$(OBJ_DIR)/solver_%.c.o: %.c
	@echo "CC			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) -I$(UTILS_DIR)
