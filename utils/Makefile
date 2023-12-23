SRC = $(wildcard *.c)

OBJ = $(addprefix $(OBJ_DIR)/utils_, $(notdir $(SRC:.c=.c.o)))

all: $(UTILS_LIB)

$(UTILS_LIB): $(OBJ)
	@echo "AR 			$@"
	@$(AR) -rcs $@ $^

$(OBJ_DIR)/utils_%.c.o: %.c
	@echo "CC			$@"
	@$(CC) -o $@ -c $< $(CFLAGS)

