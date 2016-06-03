#Copyright (C) 2016  Davidson Francis <davidsondfgl@gmail.com>
#              2016  Ádamo Ludwig     <ludwigadamo@gmail.com>
#
#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>

CC=gcc
BINDIR = $(CURDIR)/bin
SRCDIR = $(CURDIR)/src
INCLUDE = $(CURDIR)/include
CFLAGS =  -Wall -Werror
CFLAGS += -I $(INCLUDE) -std=gnu99
MAKEFLAGS += --quiet

.PHONY: server
.PHONY: client

all: server client

server:
	$(CC) $(CFLAGS) $(SRCDIR)/server/*.c -o $(BINDIR)/server

client:
	$(CC) $(CFLAGS) $(SRCDIR)/client/*.c -o $(BINDIR)/client

clean:
	@rm -f $(BINDIR)/server
	@rm -f $(BINDIR)/client
