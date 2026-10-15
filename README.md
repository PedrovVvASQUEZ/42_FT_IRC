# ft_irc

`ft_irc` is a lightweight IRC server written in C++98 as a 42 school project. It accepts multiple TCP clients, registers them with a server password, and provides basic IRC-style private messaging and channel management.

## Features

- Non-blocking IPv4 TCP server using `poll()` to monitor client connections.
- Password-protected client registration with `PASS`, `NICK`, and `USER`.
- Support for up to 100 connected clients.
- Channel creation and membership management with `JOIN` and `PART`.
- Private and channel messaging with `PRIVMSG`; `NOTICE` is also supported.
- Channel operator actions: `KICK`, `INVITE`, and `MODE`.
- Channel topics with `TOPIC`.
- Ping replies with `PING` / `PONG`, and client disconnection with `QUIT`.
- Channel modes: invite-only (`i`), operator-managed topics (`t`), password key (`k`), user limit (`l`), and channel operator status (`o`).
- Graceful shutdown on `SIGINT`, `SIGQUIT`, or `SIGTERM`.

This is a learning project implementing a subset of IRC behavior; it is not intended to be a full production IRC daemon. It uses plain TCP and does not provide TLS encryption.

## Requirements

- A C++ compiler with C++98 support
- GNU Make
- A POSIX-compatible system (the server uses POSIX sockets and `poll()`)

No third-party libraries are required.

## Build

From the project root, run:

```sh
make
```

This builds the `ircserv` executable. To remove object files, run `make clean`; to remove all generated build files, run `make fclean`. Use `make re` to rebuild from scratch.

## Run

Start the server with a listening port and connection password:

```sh
./ircserv <port> <password>
```

For example:

```sh
./ircserv 6667 secret
```

The port must be between 1 and 65535, and the password must not be empty. The server binds to all local IPv4 interfaces on the selected port.

## Connect and try it

Connect with any IRC client configured for the server address, port, and password. For a quick local test, use `nc` (netcat):

```sh
nc 127.0.0.1 6667
```

Then enter the registration commands, replacing `secret` with the password passed to the server:

```text
PASS secret
NICK alice
USER alice 0 * :Alice Example
```

After registration, try the following IRC commands:

```text
JOIN #general
PRIVMSG #general :Hello, everyone!
TOPIC #general :A sample channel
PART #general
QUIT :Goodbye
```

The first client to join a new channel becomes its operator. Channel modes can be set by an operator, for example `MODE #general +t` or `MODE #general +k channelkey`.

## Project structure

```text
headers/       C++ class declarations
srcs/          Server entry point and implementation
Makefile       Build rules for the ircserv executable
```

The implementation is divided into the following components:

- `Server` manages the listening socket, event loop, connected clients, and channels.
- `Clients` stores client identity, registration state, and incoming data buffers.
- `Channels` tracks channel members, operators, topics, invitation state, and modes.
- `Messages` parses incoming IRC-style lines and their parameters.
- `CommandHandler` dispatches supported commands and generates protocol replies.