@echo off
REM Build script for C++ HTTP Server

echo Creating obj directory if not exists...
if not exist obj mkdir obj

echo Compiling source files...
g++ -std=c++17 -Wall -I./include -c src/main.cpp -o obj/main.o
g++ -std=c++17 -Wall -I./include -c src/server.cpp -o obj/server.o
g++ -std=c++17 -Wall -I./include -c src/http.cpp -o obj/http.o
g++ -std=c++17 -Wall -I./include -c src/router.cpp -o obj/router.o
g++ -std=c++17 -Wall -I./include -c src/response.cpp -o obj/response.o
g++ -std=c++17 -Wall -I./include -c src/database.cpp -o obj/database.o
g++ -std=c++17 -Wall -I./include -c src/users.cpp -o obj/users.o

echo Linking...
g++ obj/main.o obj/server.o obj/http.o obj/router.o obj/response.o obj/database.o obj/users.o -o server.exe -lboost_system -lsqlite3 -lpthread

echo Build complete!
echo Run the server with: server.exe 