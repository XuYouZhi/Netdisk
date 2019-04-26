SRCS:=$(wildcard *.c)
OBJS:= $(SRCS:%.c=%.o)
ELF:=thread_pool_server
$(ELF):$(OBJS)
	gcc $^ -o $@ -pthread
clean:
	rm -rf $(ELF) $(OBJS)
