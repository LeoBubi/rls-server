CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -std=c17 -I include
LDFLAGS = -lcrypt

SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/core/*.c) $(wildcard $(SRCDIR)/io/*.c) $(wildcard $(SRCDIR)/pty/*.c) $(wildcard $(SRCDIR)/utils/*.c)

OBJDIR = obj
OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

TARGET = rls-server

.PHONY: clean cleaninstall

#---------------------------------------------#

# Create obj directory if it doesn't exist
$(shell mkdir -p $(OBJDIR)/core $(OBJDIR)/io $(OBJDIR)/pty $(OBJDIR)/utils)

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Clean up object files, target executable, and obj directory
clean:
	rm -rfv $(OBJDIR) $(TARGET)

# Clean up object files after installation
cleaninstall:
	@rm -rf $(OBJDIR)