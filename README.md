# NUKO

[Live version](https://cirnosperfectmultiplayerlobby.herokuapp.com)

A multiplayer first person shooter written in C and SDL2 for my SDD Major Project.

## Prerequisites

- SDL2
- NodeJS

## Compiling

Compiling for web using emscripten

```
make -f web.mk
```

Compiling natively using gcc

```
make -f linux.mk
```

## Hosting a server

Hosting on web

```
cd ./build/web
npm install
node .
```

This will open a server on https://localhost:8000
