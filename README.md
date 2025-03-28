# C++ HTTP Server

A modular HTTP server implementation in C++ using Boost.Asio.

## Project Structure

The project is organized into the following modules:

-   **HTTP Module** (`http.hpp/cpp`): Handles HTTP-related functionality

    -   HTTP method resolution
    -   JSON parsing from requests
    -   CORS headers management

-   **Router Module** (`router.hpp/cpp`): Manages request routing

    -   Endpoint resolution
    -   Request handling based on method and endpoint
    -   Calls appropriate handler functions

-   **Response Module** (`response.hpp/cpp`): Handles HTTP responses

    -   JSON response generation
    -   Error response handling
    -   Not found responses
    -   OPTIONS response for CORS preflight

-   **Server Module** (`server.hpp/cpp`): Core server functionality

    -   TCP socket management
    -   HTTP request parsing
    -   Server startup and connection acceptance

-   **Users Module** (`users.hpp/cpp`): User management

    -   Authentication
    -   User registration
    -   User data retrieval

-   **Database Module** (`database.hpp/cpp`): Database operations
    -   Data storage and retrieval

## Building the Project

The project uses a Makefile for building. The following commands are available:

```bash
# Build the project
make

# Run the server
make run

# Clean build artifacts
make clean
```

## Dependencies

-   C++17
-   Boost.Asio
-   SQLite3
-   nlohmann/json (header-only)

## Endpoints

-   `GET /users/get-all`: Get all users
-   `POST /auth/login`: User login
-   `POST /auth/register`: User registration
