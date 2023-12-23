LIB_SRC = network.c train_network.c save_network.c
SRC = network.c train_network.c  save_network.c main.c
CFLAGS += `pkg-config --cflags SDL2_image`
LFLAGS += `pkg-config --libs sdl2` `pkg-config --libs SDL2_image` -lm

LIB_OBJ = $(addprefix $(OBJ_DIR)/network_, $(notdir $(LIB_SRC:.c=.c.o)))
OBJ = $(addprefix $(OBJ_DIR)/network_, $(notdir $(SRC:.c=.c.o)))

all: $(AI_LIB) $(AI_EXE) $(AI_HEXA_EXE)

$(AI_LIB): $(LIB_OBJ)
	@echo "AR                       $@"
	@$(AR) -rcs $@ $^

$(AI_EXE): $(OBJ)
	@echo "LD                       $@"
	@$(LD) -o $@ $^ $(LFLAGS) -lutils

$(OBJ_DIR)/network_%.c.o: %.c
	@echo "CC                       $@"
	@$(CC) -o $@ -c $< $(CFLAGS) -I$(UTILS_DIR)

