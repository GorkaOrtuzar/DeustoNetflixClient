
#include "menu.h"
#include <iostream>
using namespace std;

char menuPrincipal(){
    char opcion;

    cout<<"\033[1;36m\n--- MENU PRINCIPAL ---\033[0m\n"<<endl;
    cout<<"1. Administrador\n"<<endl;
    cout<<"2. Usuario\n"<<endl;
    cout<<"0. Salir\n"<<endl;
    cout<<"Elige una opción: ";
    cin >> opcion;
    return opcion;
}

char menuAdministrador(){
    char opcion;

    cout<<"\033[1;36m\n--- MENU ADMINISTRADOR ---\033[0m\n"<<endl;
    cout<<"1. Ver listado de películas.\n"<<endl;
    cout<<"2. Ver listado de usuarios\n"<<endl;
    cout<<"3. Eliminar película\n"<<endl;
    cout<<"4. Añadir película\n"<<endl;
    cout<<"5. Añadir usuario\n"<<endl;
    cout<<"0. Salir\n"<<endl;
    cout<<"Elige una opción: ";
    cin>>opcion;
    return opcion;
}

char menuUsuario(){
    char opcion;

    cout<<"\033[1;36m\n--- MENU USUARIO ---\033[0m\n"<<endl;
    cout<<"1. Ver datos personales\n"<<endl;
    cout<<"2. Ver listado de películas\n"<<endl;
    cout<<"3. Ver mis peliculas vistas\n"<<endl;
    cout<<"0. Salir\n"<<endl;
    cout<<"Elige una opción: ";
    cin>>opcion;
    return opcion;
}

char menuSesiones(){
    char opcion;

    cout<<"\033[1;36m\n--- INICIAR SESIÓN / REGISTRARSE ---\033[0m\n"<<endl;
    cout<<"1. Iniciar Sesión\n"<<endl;
    cout<<"2. Registrarse\n"<<endl;
    cout<<"0. Volver\n"<<endl;
    cout<<"Elige una opción: ";
    cin>>opcion;
    return opcion;
}



