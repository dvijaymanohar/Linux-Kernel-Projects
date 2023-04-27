# UART access application makefile

CC := gcc
SRCS := $(wildcard *.c)
HDRS := ./uart_io.h
#OBJS := $(SRCS: .c = .o)

UART : ${SRCS}
	${CC} -O $^ -o $@

