## cserver

This project is a simple web server that handles file transfers using sockets in
C.

It's my first implementation of a web server for a computer networks class,
designed to help me understand the basics of networking and socket programming.

## Table of Contents

- [Setup](#setup)
- [Tasks](#tasks)
- [Usage](#usage)
- [Acknowledgments](#acknowledgments)
- [Demo Video](#low-quality-demo)

## Setup

1. **Clone the repository**:

   - HTTPS `https://github.com/SamVeras/cserver.git`
   - SSH `git@github.com:SamVeras/cserver.git`.

2. **Initialize and update submodules**:

   - Run `git submodule init` and `git submodule update`.

3. **Install [Task](https://taskfile.dev/)** (if not already installed).

4. **Install [Doxygen](https://www.doxygen.nl)** (optional, but required to build
   docs).

5. **Install tree** (optional but required if you want to generate the index directory structure):

   - It should be available on most, if not all, Linux distributions.
   - For example, on Debian-based systems, you can install it with: `sudo apt install tree`
   - If tree is missing, the index folder directory generation will fail.

6. **Build server**: Run `task build` to compile the server executable.

7. **Build documentation**: Run `task docs` to build the Doxygen documentation.

8. **Quick start**: `server --port 8080`.

The server will only serve files from the `/data` folder, which is server's
document root.

## Tasks

This project uses [Task](https://taskfile.dev/) to define tasks which can be run
from the command line.\
These tasks are defined in the `Taskfile.yml` file and can be run with the
`task` command.

The following tasks are defined:

- `build`: Compile the server executable, linking object files.
  - Depends on `objects`
  - Compiles object files into the server executable
- `objects`: Compile source files into object files.
  - Compiles source files into object files
- `docs`: Generate doxygen documentation.
  - Generates doxygen documentation
  - Depends on source files, header files, and Doxyfile
- `showdocs`: Generate and open the Doxygen documentation.
  - Depends on `docs`
  - Opens the generated doxygen documentation in the default web browser
- `clean`: Clean build folder of all object files.
  - Deletes all object files in the build folder

## Usage

Run the server with the following options:

- `-p, --port PORT`\
  Choose a specific port to bind to.\
  The port must be in the range `[1024, 65535]`, or `0` for a random port.\
  Defaults to `0`.

- `-b, --buffer BUFF_SIZE`\
  Set the buffer size for file transfer in bytes. Must be a positive value.\
  Defaults to `1024`.

- `-l, --log-level LOGLEVEL`\
  Set the log level for messages. Only messages with this level or higher will
  be shown.\
  Must be in the range `[0, 5]`, where `TRACE = 0`, `DEBUG = 1`, `INFO = 2`,
  `WARN = 3`, `ERROR = 4`, and `FATAL = 5`.\
  Defaults to `INFO` (`2`).

- `-c, --backlog MAXCONNECT`\
  Set the maximum number of connections in the queue.\
  Must be a positive value.\
  Defaults to `5`.

- `-f, --log-file LOGFILE`
  Specify the name of the file to write logs to. The file will be created if it
  doesn’t exist.\
  If specified, the file name must not be empty.\
  Defaults to `"server.log"`.

- `-r, --root ROOTDIR`\
  Set the root directory for serving files.\
  Defaults to `"data"`.

- `-i, --favicon FAVICONFILE`\
  Set the name of the favicon file.\
  Must be a valid file in the root directory.\
  Defaults to `"favicon32.png"`.

## Acknowledgments

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) by Brian
  "Beej Jorgensen" Hall was super helpful for learning sockets in C.
- [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) by
  [jothepro](https://github.com/jothepro) improved the look of the Doxygen docs
  in this project.
- The `man` command on Linux (originally created by Ken Thompson and Dennis
  Ritchie) was an essential reference throughout development.
- [Doxygen](https://www.doxygen.nl), created by [Dimitri van
  Heesch](https://github.com/doxygen/), was used to generate the project
  documentation.
- [Git](https://git-scm.com/), created by [Linus
  Torvalds](https://github.com/torvalds/), made version control for this project
  a breeze.

### Low Quality Demo

https://github.com/user-attachments/assets/5ab0111e-6b22-46d1-b7e8-e196895f3477
