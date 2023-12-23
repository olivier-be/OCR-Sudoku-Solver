SRC = $(wildcard *.c)
OBJ = $(addprefix $(OBJ_DIR)/gui_, $(notdir $(SRC:.c=.c.o)))

all: $(GUI_EXE)

$(GUI_EXE): $(OBJ)
	@echo "LD			$@"
	@$(LD) -o $@ $^ $(LFLAGS) `pkg-config --libs gtk+-3.0` -lsolver \
        -lutils `pkg-config --libs gdk-3.0` -lm -lnetwork

$(OBJ_DIR)/gui_%.c.o: %.c
	@echo "CC			$@"
	@$(CC) -o $@ -c $< $(CFLAGS) `pkg-config --cflags gtk+-3.0` \
        `pkg-config --cflags gdk-3.0` -I$(UTILS_DIR) -I$(SOLVER_DIR) \
		-I$(AI_DIR)

