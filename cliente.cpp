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
        recv(s, recvBuff, sizeof(recvBuff), 0);
        cout << recvBuff << endl;

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

                    // Menú del administrador
                    char opcionAdmin;
                    do {
                        opcionAdmin = menuAdministrador();

                        // Enviar opción al servidor
                        sprintf(sendBuff, "%c", opcionAdmin);
                        send(s, sendBuff, sizeof(sendBuff), 0);
                        recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                        switch (opcionAdmin) {
                            case '1': { // Ver listado de películas
                                int numPeliculas;

                                // Solicitar listado de películas
                                sprintf(sendBuff, "2"); // Código para listar películas
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Recibir número de películas
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                sscanf(recvBuff, "%d", &numPeliculas);

                                // Mostrar encabezados
                                mostrarTitulosPeliculas();

                                // Recibir y mostrar cada película
                                for (int i = 0; i < numPeliculas; i++) {
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
                                }
                                break;
                            }

                            case '2': { // Ver listado de usuarios
                                int numUsuarios;

                                // Solicitar listado de usuarios
                                sprintf(sendBuff, "3"); // Código para listar usuarios
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                // Recibir número de usuarios
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                sscanf(recvBuff, "%d", &numUsuarios);

                                printf("\033[1;35m%20s%15s%15s%20s%15s%20s\n", "NOMBRE", "APELLIDO", "EMAIL", "NICKNAME", "PAIS", "CONTRASEÑA\033[0m\n");

                                // Recibir y mostrar cada usuario
                                for (int i = 0; i < numUsuarios; i++) {
                                    ClienteUsuario u;

                                    recv(s, recvBuff, sizeof(recvBuff), 0);

                                    // Parsear datos
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

                                    printf("%20s%15s%15s%20s%15s%15s\n",
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

                                // Recibir confirmación
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                if (strcmp(recvBuff, "1") == 0) {
                                    cout << "\033[1;32mPelícula eliminada correctamente.\033[0m" << endl;
                                } else {
                                    cout << "\033[1;31mError al eliminar la película.\033[0m" << endl;
                                }
                                break;
                            }

                            case '4': { // Añadir película
                                ClientePelicula p = pedirPelicula();

                                // Enviar datos de la película
                                sprintf(sendBuff, "7"); // Código para añadir película
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                sprintf(sendBuff, "%s;%s;%d;%s", p.titulo, p.genero, p.duracion, p.Reparto);
                                send(s, sendBuff, sizeof(sendBuff), 0);

                                // Recibir confirmación
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                if (strcmp(recvBuff, "1") == 0) {
                                    cout << "\033[1;32mPelícula añadida correctamente.\033[0m" << endl;
                                } else {
                                    cout << "\033[1;31mError al añadir la película.\033[0m" << endl;
                                }
                                break;
                            }

                            case '5': { // Añadir usuario
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

                        switch (opcionUsuario) {
                            case '1': { // Ver datos personales
                                cout << "Ver datos personales" << endl;

                                // Solicitar datos completos del usuario
                                sprintf(sendBuff, "3"); // Código para listar usuarios
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                int numUsuarios;
                                recv(s, recvBuff, sizeof(recvBuff), 0);
                                sscanf(recvBuff, "%d", &numUsuarios);

                                bool encontrado = false;
                                for (int i = 0; i < numUsuarios && !encontrado; i++) {
                                    ClienteUsuario u;

                                    recv(s, recvBuff, sizeof(recvBuff), 0);

                                    // Parsear datos
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

                                    // Si es el usuario actual, mostrar datos
                                    if (strcmp(u.Email, usuarioActual.Email) == 0) {
                                        mostrarUsuario(u);
                                        usuarioActual = u; // Actualizar datos del usuario
                                        encontrado = true;
                                    }
                                }

                                if (!encontrado) {
                                    cout << "\033[1;31mError: No se encontraron los datos del usuario.\033[0m" << endl;
                                }
                                break;
                            }

                            case '2': { // Ver listado de películas
                                int numPeliculas;

                                // Solicitar listado de películas
                                sprintf(sendBuff, "2"); // Código para listar películas
                                send(s, sendBuff, sizeof(sendBuff), 0);
                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

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
                                                // Enviar solicitud para registrar visualización
                                                sprintf(sendBuff, "6"); // Código para registrar visualización
                                                send(s, sendBuff, sizeof(sendBuff), 0);
                                                recv(s, recvBuff, sizeof(recvBuff), 0); // Confirmación

                                                // Enviar email y título
                                                sprintf(sendBuff, "%s", usuarioActual.Email);
                                                send(s, sendBuff, sizeof(sendBuff), 0);

                                                sprintf(sendBuff, "%s", peliculaSeleccionada);
                                                send(s, sendBuff, sizeof(sendBuff), 0);

                                                // Recibir confirmación
                                                recv(s, recvBuff, sizeof(recvBuff), 0);
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

    /*ACABA EL PROGRAMA DEL CLIENTE*/

    // Cerrar el socket y limpiar Winsock
    closesocket(s);
    WSACleanup();

    return 0;
}
