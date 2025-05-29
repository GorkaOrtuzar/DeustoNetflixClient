// IMPORTANT: Winsock Library ("ws2_32") should be linked
//Un socket es un mecanismo de comunicación entre 2 procesos cualesquiera
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include "menu.h"
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 6000
using namespace std;

// Estructura para representar un usuario en el cliente
struct ClienteUsuario {
    char Nombre[20];
    char Apellido[20];
    char Email[20];
    char NickName[20];
    char Pais[20];
    char Contrasenia[30];
};

// Estructura para representar una película en el cliente
struct ClientePelicula {
    char titulo[20];
    char genero[20];
    int duracion;
    char Reparto[20];
};

// Función para mostrar los datos de un usuario
void mostrarUsuario(ClienteUsuario u) {
    cout << "\033[1;32m----- DATOS PERSONALES ----\033[0m" << endl;
    cout << "Nombre: " << u.Nombre << endl;
    cout << "Apellido: " << u.Apellido << endl;
    cout << "Email: " << u.Email << endl;
    cout << "NickName: " << u.NickName << endl;
    cout << "País: " << u.Pais << endl;
    cout << "Contraseña: " << u.Contrasenia << endl;
}

// Función para mostrar los títulos de las columnas de películas
void mostrarTitulosPeliculas() {
    printf("\033[1;35m%20s%30s%20s%30s\n", "TITULO", "GÉNERO", "DURACIÓN", "REPARTO\033[0m");
}

// Función para mostrar una película
void mostrarPelicula(ClientePelicula p) {
    printf("%20s%30s%20d%30s\n", p.titulo, p.genero, p.duracion, p.Reparto);
}

// Función para pedir los datos de una película
ClientePelicula pedirPelicula() {
    ClientePelicula p;

    cout << "Introduce el título: ";
    cin.ignore();
    cin.getline(p.titulo, 20);

    cout << "Introduce el género: ";
    cin.getline(p.genero, 20);

    cout << "Introduce la duración (en minutos): ";
    cin >> p.duracion;
    cin.ignore();

    cout << "Introduce el reparto: ";
    cin.getline(p.Reparto, 20);

    return p;
}

// Función para pedir el título de una película
char* pedirTitulo() {
    static char titulo[50];
    cout << "Introduce el título: ";
    cin.ignore();
    cin.getline(titulo, 50);
    return titulo;
}

// Función para registrar un nuevo usuario
ClienteUsuario registrarUsuario() {
    ClienteUsuario u;

    cout << "Introduce el nombre: ";
    cin.ignore();
    cin.getline(u.Nombre, 20);

    cout << "Introduce el apellido: ";
    cin.getline(u.Apellido, 20);

    cout << "Introduce el email: ";
    cin.getline(u.Email, 20);

    cout << "Introduce tu NickName: ";
    cin.getline(u.NickName, 20);

    cout << "Introduce tu País: ";
    cin.getline(u.Pais, 20);

    cout << "Introduce tu Contraseña: ";
    cin.getline(u.Contrasenia, 30);

    return u;
}

// Función para iniciar sesión
bool iniciarSesion(ClienteUsuario* usuario) {
    char email[20], password[30];

    cout << "Introduce tu email: ";
    cin.ignore();
    cin.getline(email, 20);

    cout << "Introduce tu contraseña: ";
    cin.getline(password, 30);

    // Guardar los datos para enviarlos al servidor
    strcpy(usuario->Email, email);
    strcpy(usuario->Contrasenia, password);

    return true;
}

int main(int argc, char *argv[]) {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in server;
    char sendBuff[512], recvBuff[512];

    // Inicializar Winsock
    printf("\nInitialising Winsock...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return -1;
    }
    printf("Initialised.\n");

    // Crear socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        WSACleanup();
        return -1;
    }
    printf("Socket created.\n");

    // Configurar dirección del servidor
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);

    // Conectar al servidor
    if (connect(s, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Connection error: %d", WSAGetLastError());
        closesocket(s);
        WSACleanup();
        return -1;
    }
    printf("Connection stablished with: %s (%d)\n", inet_ntoa(server.sin_addr),
           ntohs(server.sin_port));

    /*EMPIEZA EL PROGRAMA DEL CLIENTE*/

    char opcion;
    ClienteUsuario usuarioActual;
    bool usuarioEncontrado = false;

    do {
        opcion = menuPrincipal();

        // Enviar opción al servidor
        sprintf(sendBuff, "%c", opcion);
        send(s, sendBuff, sizeof(sendBuff), 0);

        // Recibir confirmación del servidor
        memset(recvBuff, 0, sizeof(recvBuff));
        recv(s, recvBuff, sizeof(recvBuff), 0);
        printf("DEBUG CLIENTE: Confirmación recibida: '%s' (longitud: %d)\n", recvBuff, (int)strlen(recvBuff));
        fflush(stdout);
        // Solo mostrar si parece válida
        if (strstr(recvBuff, "Comando recibido") != NULL) {
            cout << recvBuff << endl;
        }
        switch (opcion) {
            case '1': { // Menú administrador
                char email[50], contrasenia[50];
                int adminEncontrado = 0;

                cout << "Introduce tu email: ";
                cin.ignore();
                cin.getline(email, 50);

                cout << "Introduce tu contraseña: ";
                cin.getline(contrasenia, 50);

                // Enviar credenciales del administrador
                sprintf(sendBuff, "%s", email);
                send(s, sendBuff, sizeof(sendBuff), 0);

                sprintf(sendBuff, "%s", contrasenia);
                send(s, sendBuff, sizeof(sendBuff), 0);

                // Recibir resultado de autenticación
                recv(s, recvBuff, sizeof(recvBuff), 0);
                sscanf(recvBuff, "%d", &adminEncontrado);

                if (adminEncontrado) {
                    cout << "\033[1;32mAutenticación exitosa como administrador.\033[0m" << endl;

                    // Menú del administrador - CON DEBUG
                    char opcionAdmin;
                    do {
                        opcionAdmin = menuAdministrador();
                        printf("DEBUG CLIENTE: Opción seleccionada: '%c'\n", opcionAdmin);

                        if (opcionAdmin == '0') {
                                // Enviar la opción '0' al servidor para que sepa que sales
                                memset(sendBuff, 0, sizeof(sendBuff));
                                sprintf(sendBuff, "%c", opcionAdmin);
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                break;  // Ahora sí, salir
                            }

                        // Enviar opción al servidor
                        memset(sendBuff, 0, sizeof(sendBuff));
                        sprintf(sendBuff, "%c", opcionAdmin);
                        printf("DEBUG CLIENTE: Enviando opción al servidor: '%s'\n", sendBuff);
                        fflush(stdout);
                        if (send(s, sendBuff, sizeof(sendBuff), 0) == SOCKET_ERROR) {
                            printf("ERROR enviando opción al servidor\n");fflush(stdout);
                            break;
                        }

                        printf("DEBUG CLIENTE: Esperando confirmación del servidor...\n");
                        fflush(stdout);
                        // Recibir confirmación del servidor
                        memset(recvBuff, 0, sizeof(recvBuff));
                        int bytes_recibidos = recv(s, recvBuff, sizeof(recvBuff), 0);

                        printf("DEBUG CLIENTE: Confirmación recibida - Bytes: %d, Contenido: '%s'\n",
                               bytes_recibidos, recvBuff);
                        fflush(stdout);
                        if (bytes_recibidos <= 0) {
                            printf("ERROR recibiendo confirmación del servidor\n");
                            break;
                        }

                        printf("DEBUG CLIENTE: Entrando en switch con opción '%c'\n", opcionAdmin);
                        fflush(stdout);
                        switch (opcionAdmin) {
                            case '1': { // Ver listado de películas - CLIENTE CON DEBUG
                                printf("DEBUG: Cliente solicitando películas...\n");
                                fflush(stdout);
                                // 1. Recibir número de películas del servidor
                                memset(recvBuff, 0, sizeof(recvBuff));
                                int bytes_recibidos = recv(s, recvBuff, sizeof(recvBuff), 0);

                                printf("DEBUG: Bytes recibidos del número de películas: %d\n", bytes_recibidos);
                                printf("DEBUG: Contenido recibido: '%s'\n", recvBuff);
                                fflush(stdout);
                                if (bytes_recibidos <= 0) {
                                    cout << "Error recibiendo número de películas" << endl;
                                    break;
                                }

                                int numPeliculas = atoi(recvBuff);
                                printf("DEBUG: Número de películas a recibir: %d\n", numPeliculas);
                                fflush(stdout);
                                if (numPeliculas <= 0) {
                                    cout << "No hay películas registradas." << endl;
                                    break;
                                }

                                // 2. Mostrar encabezados de la tabla
                                mostrarTitulosPeliculas();

                                // 3. Recibir cada película una por una
                                for (int i = 0; i < numPeliculas; i++) {
                                    printf("DEBUG: Esperando película %d/%d...\n", i+1, numPeliculas);
                                    fflush(stdout);
                                    // Limpiar buffer antes de recibir
                                    memset(recvBuff, 0, sizeof(recvBuff));

                                    // Recibir datos de la película
                                    bytes_recibidos = recv(s, recvBuff, sizeof(recvBuff), 0);
                                    printf("DEBUG: Película %d - Bytes recibidos: %d\n", i+1, bytes_recibidos);
                                    printf("DEBUG: Película %d - Datos: '%s'\n", i+1, recvBuff);
                                    fflush(stdout);
                                    if (bytes_recibidos <= 0) {
                                        cout << "Error recibiendo película " << i+1 << endl;
                                        break;
                                    }

                                    // 4. Enviar confirmación al servidor (ACK)
                                    printf("DEBUG: Enviando confirmación para película %d...\n", i+1);
                                    fflush(stdout);
                                    memset(sendBuff, 0, sizeof(sendBuff));
                                    strcpy(sendBuff, "OK");
                                    int bytes_enviados = send(s, sendBuff, sizeof(sendBuff), 0);
                                    printf("DEBUG: Confirmación enviada - Bytes: %d\n", bytes_enviados);
                                    fflush(stdout);
                                    // 5. Parsear los datos recibidos
                                    ClientePelicula p;
                                    char *token = strtok(recvBuff, ";");
                                    if (token) {
                                        strncpy(p.titulo, token, sizeof(p.titulo) - 1);
                                        p.titulo[sizeof(p.titulo) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(p.genero, token, sizeof(p.genero) - 1);
                                        p.genero[sizeof(p.genero) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        p.duracion = atoi(token);
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(p.Reparto, token, sizeof(p.Reparto) - 1);
                                        p.Reparto[sizeof(p.Reparto) - 1] = '\0';
                                    }

                                    // 6. Mostrar la película
                                    printf("DEBUG: Mostrando película: %s\n", p.titulo);
                                    fflush(stdout);
                                    mostrarPelicula(p);
                                }

                                printf("DEBUG: Listado de películas completado\n");
                                fflush(stdout);
                                break;
                            }

                            case '2': { // Ver listado de usuarios - CLIENTE
                                // 1. Recibir número de usuarios del servidor
                                memset(recvBuff, 0, sizeof(recvBuff));
                                if (recv(s, recvBuff, sizeof(recvBuff), 0) <= 0) {
                                    cout << "Error recibiendo número de usuarios" << endl;
                                    break;
                                }

                                int numUsuarios = atoi(recvBuff);
                                printf("Número de usuarios: %d\n", numUsuarios);
                                fflush(stdout);
                                if (numUsuarios <= 0) {
                                    cout << "No hay usuarios registrados." << endl;
                                    break;
                                }

                                // 2. Mostrar encabezados de la tabla
                                printf("\033[1;35m%20s%15s%20s%20s%15s%30s\033[0m\n",
                                       "NOMBRE", "APELLIDO", "EMAIL", "NICKNAME", "PAIS", "CONTRASEÑA");
                                fflush(stdout);
                                // 3. Recibir cada usuario uno por uno
                                for (int i = 0; i < numUsuarios; i++) {
                                    // Limpiar buffer antes de recibir
                                    memset(recvBuff, 0, sizeof(recvBuff));

                                    // Recibir datos del usuario
                                    if (recv(s, recvBuff, sizeof(recvBuff), 0) <= 0) {
                                        cout << "Error recibiendo usuario " << i+1 << endl;
                                        break;
                                    }

                                    // 4. Enviar confirmación al servidor (ACK)
                                    memset(sendBuff, 0, sizeof(sendBuff));
                                    strcpy(sendBuff, "OK");
                                    send(s, sendBuff, strlen(sendBuff), 0);

                                    // 5. Parsear los datos recibidos
                                    ClienteUsuario u;
                                    char *token = strtok(recvBuff, ";");
                                    if (token) {
                                        strncpy(u.Nombre, token, sizeof(u.Nombre) - 1);
                                        u.Nombre[sizeof(u.Nombre) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(u.Apellido, token, sizeof(u.Apellido) - 1);
                                        u.Apellido[sizeof(u.Apellido) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(u.Email, token, sizeof(u.Email) - 1);
                                        u.Email[sizeof(u.Email) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(u.NickName, token, sizeof(u.NickName) - 1);
                                        u.NickName[sizeof(u.NickName) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(u.Pais, token, sizeof(u.Pais) - 1);
                                        u.Pais[sizeof(u.Pais) - 1] = '\0';
                                    }

                                    token = strtok(NULL, ";");
                                    if (token) {
                                        strncpy(u.Contrasenia, token, sizeof(u.Contrasenia) - 1);
                                        u.Contrasenia[sizeof(u.Contrasenia) - 1] = '\0';
                                    }

                                    // 6. Mostrar el usuario en formato tabla
                                    printf("%20s%15s%20s%20s%15s%30s\n",
                                           u.Nombre, u.Apellido, u.Email, u.NickName, u.Pais, u.Contrasenia);
                                }
                                break;
                            }

                            case '3': { // Eliminar película
                                char *titulo = pedirTitulo();

                                // Enviar título a eliminar
                                sprintf(sendBuff, "8"); // Código para eliminar película
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                sprintf(sendBuff, "%s", titulo);
                                send(s, sendBuff, sizeof(sendBuff), 0);

                                // Limpiar buffer antes de recibir
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(s, recvBuff, sizeof(recvBuff), 0);

                                // Debug: ver qué se está recibiendo realmente
                                printf("Respuesta recibida: '%s' (longitud: %d)\n", recvBuff, (int)strlen(recvBuff));
                                fflush(stdout);
                                // Comparación más robusta
                                if (recvBuff[0] == '1') {  // Comparar solo el primer carácter
                                    cout << "\033[1;32mPelícula eliminada correctamente.\033[0m" << endl;
                                } else {
                                    cout << "\033[1;31mError al eliminar la película.\033[0m" << endl;
                                }
                                break;
                            }

                            case '4': { // Añadir película
                                ClientePelicula p = pedirPelicula();

                                // Enviar código para añadir película
                                sprintf(sendBuff, "7");
                                send(s, sendBuff, strlen(sendBuff) + 1, 0); // Usar strlen en lugar de sizeof
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar datos de la película
                                memset(sendBuff, 0, sizeof(sendBuff));
                                sprintf(sendBuff, "%s;%s;%d;%s", p.titulo, p.genero, p.duracion, p.Reparto);
                                send(s, sendBuff, strlen(sendBuff) + 1, 0); // Usar strlen en lugar de sizeof

                                // Recibir confirmación
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(s, recvBuff, sizeof(recvBuff), 0);

                                if (strncmp(recvBuff, "1", 1) == 0) {
                                    cout << "\033[1;32mPelícula añadida correctamente.\033[0m" << endl;
                                } else {
                                    cout << "\033[1;31mError al añadir la película.\033[0m" << endl;
                                }
                                break;
                            }

                            case '5': { // Añadir usuario
                                ClienteUsuario u = registrarUsuario();

                                // Enviar código para registrar usuario
                                sprintf(sendBuff, "4");
                                send(s, sendBuff, strlen(sendBuff) + 1, 0); // Usar strlen en lugar de sizeof
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar datos del usuario
                                memset(sendBuff, 0, sizeof(sendBuff));
                                sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                    u.Nombre, u.Apellido, u.Email, u.NickName, u.Pais, u.Contrasenia);
                                send(s, sendBuff, strlen(sendBuff) + 1, 0); // Usar strlen en lugar de sizeof

                                // Recibir confirmación
                                memset(recvBuff, 0, sizeof(recvBuff));
                                recv(s, recvBuff, sizeof(recvBuff), 0);

                                if (strncmp(recvBuff, "1", 1) == 0) {
                                    cout << "\033[1;32mUsuario añadido correctamente.\033[0m" << endl;
                                } else {
                                    cout << "\033[1;31mError al añadir el usuario.\033[0m" << endl;
                                }
                                break;
                            }

                            case '0': // Salir
                                break;

                            default:
                                cout << "\033[1;31mERROR! La opción seleccionada no es correcta\033[0m" << endl;
                                break;
                        }

                        printf("DEBUG CLIENTE: Fin del switch, volviendo al menú\n");
                        fflush(stdout);
                    } while (opcionAdmin != '0');

                } else {
                    cout << "\033[1;31mCredenciales incorrectas para el administrador\033[0m" << endl;
                }
                break;
            }

            case '2': { // Menú usuario
                char opcionSesion;

                do {
                    opcionSesion = menuSesiones();

                    switch (opcionSesion) {
                        case '1': { // Iniciar sesión
                            if (iniciarSesion(&usuarioActual)) {
                                // Enviar solicitud de autenticación
                                sprintf(sendBuff, "1"); // Código para autenticar usuario
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar credenciales
                                sprintf(sendBuff, "%s", usuarioActual.Email);
                                send(s, sendBuff, sizeof(sendBuff), 0);

                                sprintf(sendBuff, "%s", usuarioActual.Contrasenia);
                                send(s, sendBuff, sizeof(sendBuff), 0);

                                // Recibir resultado de autenticación
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                int resultado;
                                sscanf(recvBuff, "%d", &resultado);

                                if (resultado == 1) {
                                    cout << "\033[1;32mInicio de sesión exitoso. Bienvenido!\033[0m" << endl;
                                    usuarioEncontrado = true;
                                    opcionSesion = '0'; // Salir del submenu
                                } else {
                                    cout << "\033[1;31mError: Email o contraseña incorrectos.\033[0m" << endl;
                                }
                            }
                            break;
                        }


                        case '2': { // Registrarse
                            ClienteUsuario u = registrarUsuario();

                            // Enviar datos del usuario
                            sprintf(sendBuff, "4"); // Código para registrar usuario
                            send(s, sendBuff, sizeof(sendBuff), 0);
                            recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                            sprintf(sendBuff, "%s;%s;%s;%s;%s;%s",
                                    u.Nombre, u.Apellido, u.Email, u.NickName, u.Pais, u.Contrasenia);
                            send(s, sendBuff, sizeof(sendBuff), 0);

                            // Recibir confirmación
                            recv(s, recvBuff, sizeof(recvBuff), 0);
                            if (strcmp(recvBuff, "1") == 0) {
                                cout << "\033[1;32mUsuario registrado correctamente. Ya puede iniciar sesión.\033[0m" << endl;
                            } else {
                                cout << "\033[1;31mError al registrar el usuario.\033[0m" << endl;
                            }
                            break;
                        }

                        case '0': // Volver
                            break;

                        default:
                            cout << "\033[1;31mERROR! La opción seleccionada no es correcta\033[0m" << endl;
                            break;
                    }
                } while (opcionSesion != '0');

                // Si el login fue exitoso, mostrar menú de usuario
                if (usuarioEncontrado) {
                    char opcionUsuario;
                    do {
                        opcionUsuario = menuUsuario();

                        printf("DEBUG CLIENTE: Enviando opción de usuario '%c'\n", opcionUsuario);
                        fflush(stdout);
                        sprintf(sendBuff, "%c", opcionUsuario);
                           send(s, sendBuff, strlen(sendBuff) + 1 , 0);

                           // Recibir confirmación del servidor
                           memset(recvBuff, 0, sizeof(recvBuff));
                           recv(s, recvBuff, sizeof(recvBuff), 0);
                           printf("DEBUG CLIENTE: Confirmación recibida: %s\n", recvBuff);
                           fflush(stdout);
                           switch (opcionUsuario) {
                        case '1': { // Ver datos personales
                            cout << "Ver datos personales" << endl;

                            // YA SE RECIBIÓ CONFIRMACIÓN ARRIBA

                            // Recibir datos del usuario
                            recv(s, recvBuff, sizeof(recvBuff), 0);

                            // Parsear datos
                            ClienteUsuario u;
                            char *token = strtok(recvBuff, ";");
                            if (token) strcpy(u.Nombre, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Apellido, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Email, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.NickName, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Pais, token);

                            token = strtok(NULL, ";");
                            if (token) strcpy(u.Contrasenia, token);

                            // Mostrar todos los datos
                            mostrarUsuario(u);
                            break;
                        }
                            case '2': { // Ver listado de películas
                                int numPeliculas;

                                // Recibir número de películas
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                sscanf(recvBuff, "%d", &numPeliculas);

                                // Mostrar encabezados
                                mostrarTitulosPeliculas();

                                // Crear array para guardar las películas
                                ClientePelicula peliculas[100];
                                int contPeliculas = 0;

                                // Recibir y mostrar cada película
                                for (int i = 0; i < numPeliculas && contPeliculas < 100; i++) {
                                    ClientePelicula p;

                                    recv(s, recvBuff, sizeof(recvBuff), 0);

                                    // Parsear datos
                                    char *token = strtok(recvBuff, ";");
                                    if (token) strcpy(p.titulo, token);

                                    token = strtok(NULL, ";");
                                    if (token) strcpy(p.genero, token);

                                    token = strtok(NULL, ";");
                                    if (token) p.duracion = atoi(token);

                                    token = strtok(NULL, ";");
                                    if (token) strcpy(p.Reparto, token);

                                    mostrarPelicula(p);

                                    // Guardar película en el array
                                    peliculas[contPeliculas++] = p;
                                }

                                // Preguntar qué película quiere ver
                                cout << "\nIntroduce el título de la película que deseas ver (o 'salir' para volver): ";
                                char peliculaSeleccionada[50];
                                cin.ignore();
                                cin.getline(peliculaSeleccionada, 50);

                                if (strcmp(peliculaSeleccionada, "salir") != 0) {
                                    bool encontrada = false;

                                    for (int i = 0; i < contPeliculas && !encontrada; i++) {
                                        if (strcmp(peliculas[i].titulo, peliculaSeleccionada) == 0) {
                                            encontrada = true;

                                            cout << "\nReproduciendo '" << peliculaSeleccionada << "'..." << endl;
                                            cout << "Género: " << peliculas[i].genero
                                                 << ", Duración: " << peliculas[i].duracion
                                                 << " min, Reparto: " << peliculas[i].Reparto << endl << endl;

                                            cout << "Presiona Enter para continuar...";
                                            cin.get();

                                            // Preguntar si desea registrar la película como vista
                                            cout << "¿Quieres registrar esta película como vista? (s/n): ";
                                            char opcionRegistro;
                                            cin >> opcionRegistro;
                                            cin.ignore();

                                            if (opcionRegistro == 's' || opcionRegistro == 'S') {
                                            	memset(sendBuff, 0, sizeof(sendBuff));
                                            	sprintf(sendBuff, "6");
                                                send(s, sendBuff, strlen(sendBuff), 0);

                                            	// Recibir confirmación "OK"
                                            	memset(recvBuff, 0, sizeof(recvBuff));
                                            	recv(s, recvBuff, sizeof(recvBuff), 0);
                                            	printf("DEBUG CLIENTE: Confirmación recibida: '%s'\n", recvBuff);

                                                // Enviar email
                                            	memset(sendBuff, 0, sizeof(sendBuff));
                                                sprintf(sendBuff, "%s", usuarioActual.Email);
                                            	send(s, sendBuff, strlen(sendBuff), 0);

                                            	// Enviar título
                                                memset(sendBuff, 0, sizeof(sendBuff));
                                            	sprintf(sendBuff, "%s", peliculaSeleccionada);
                                            	send(s, sendBuff, strlen(sendBuff), 0);  // ← Cambiar sizeof por strlen

                                            	// Recibir resultado final
                                            	memset(recvBuff, 0, sizeof(recvBuff));
                                            	recv(s, recvBuff, sizeof(recvBuff), 0);
                                            	printf("DEBUG CLIENTE: Resultado recibido: '%s'\n", recvBuff);

                                            	if (strcmp(recvBuff, "1") == 0) {
                                            	   cout << "\033[1;32mPelícula registrada como vista correctamente.\033[0m" << endl;
                                            	} else {
                                            	   cout << "\033[1;31mError al registrar la visualización.\033[0m" << endl;
                                            	  }
                                            }
                                        }
                                    }

                                    if (!encontrada) {
                                        cout << "\033[1;31mPelícula no encontrada en el catálogo.\033[0m" << endl;
                                    }
                                }
                                break;
                            }

                            case '3': { // Ver mis películas vistas
                                cout << "\033[1;36m--- PELÍCULAS VISTAS ---\033[0m" << endl;

                                // Solicitar películas vistas
                                sprintf(sendBuff, "9"); // Código para listar películas vistas
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Enviar email del usuario
                                sprintf(sendBuff, "%s", usuarioActual.Email);
                                send(s, sendBuff, sizeof(sendBuff), 0);

                                // Recibir número de películas vistas
                                int numPeliculasVistas;
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                sscanf(recvBuff, "%d", &numPeliculasVistas);

                                if (numPeliculasVistas > 0) {
                                    printf("\033[1;35m%20s%30s\n", "TÍTULO", "GÉNERO\033[0m");
                                    fflush(stdout);
                                    // Recibir y mostrar cada película vista
                                    for (int i = 0; i < numPeliculasVistas; i++) {
                                        recv(s, recvBuff, sizeof(recvBuff), 0);

                                        // Parsear datos
                                        char titulo[100], genero[100];
                                        char *token = strtok(recvBuff, ";");
                                        if (token) strcpy(titulo, token);

                                        token = strtok(NULL, ";");
                                        if (token) strcpy(genero, token);

                                        printf("%20s%30s\n", titulo, genero);
                                        fflush(stdout);
                                    }
                                } else {
                                    cout << "No has visto ninguna película aún." << endl;
                                }

                                cout << "\nPresiona Enter para volver al menú...";
                                cin.ignore();
                                cin.get();
                                break;
                            }

                            case '0': // Salir
                                break;

                            default:
                                cout << "\033[1;31mERROR! La opción seleccionada no es correcta\033[0m" << endl;
                                break;
                        }
                    } while (opcionUsuario != '0');
                }
                break;
            }

            case '0': // Salir del programa
                cout << "HASTA LA PROXIMA" << endl;
                break;

            default:
                cout << "\033[1;31mERROR! La opción seleccionada no es correcta\033[0m" << endl;
                break;
        }
    } while (opcion != '0');
    cout << "Gracias por venir"<<endl;

    /*ACABA EL PROGRAMA DEL CLIENTE*/

    // Cerrar el socket y limpiar Winsock
    closesocket(s);
    WSACleanup();

    return 0;
}
