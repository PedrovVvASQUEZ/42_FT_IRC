# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: pgrellie <pgrellie@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/17 17:18:02 by pgrellie          #+#    #+#              #
#    Updated: 2025/10/14 18:31:24 by pgrellie         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

COMP = c++
FLAGS = -std=c++98 -Wall -Wextra -Werror

SRCS_DIR = srcs
HEADER_DIR = headers
OBJS_DIR = objs

SOURCES = main.cpp\
			Server.cpp\
			Clients.cpp\
			Channels.cpp\
			CommandHandler.cpp\
			Messages.cpp\

SRCS = $(addprefix $(SRCS_DIR)/, $(SOURCES))
OBJS = $(SRCS:$(SRCS_DIR)/%.cpp=$(OBJS_DIR)/%.o)

HEADERS = Server.hpp\
			Clients.hpp\
			Channels.hpp\
			CommandHandler.hpp\
			Messages.hpp\

HDRS = $(addprefix $(HEADER_DIR)/, $(HEADERS))

all: $(NAME)

$(NAME): $(OBJS)
	$(COMP) $(FLAGS) -o $(NAME) $(OBJS)
	@echo "IRC Server compiled"

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp $(HDRS) | $(OBJS_DIR)
	$(COMP) $(FLAGS) -I$(HEADER_DIR) -c $< -o $@

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

clean:
	rm -rf $(OBJS_DIR)
	@echo "Object files cleaned"

fclean: clean
	rm -rf $(NAME)
	@echo "Objects N Executable cleaned"

re: fclean all

.PHONY: all clean fclean re