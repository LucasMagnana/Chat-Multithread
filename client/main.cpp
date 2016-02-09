#include <stdlib.h>
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <thread>
#include <string>

#define PORT 25565


using namespace std;


void envoi(SOCKET sock, string str_envoi)
{

    while(1)
    {
        string str;
        //cin >> str;
        getline(cin, str);
        str_envoi += " : ";
        str_envoi += str;
        char *chaine = new char [str_envoi.length()+1];
        strcpy (chaine, str_envoi.c_str());
        char taille[4];
        sprintf(taille, "%d", str_envoi.length()+1);

        int sock_err, taille_int;

        sock_err = send(sock, taille, 4, 0);
        if (sock_err == -1) cout << "sa a pas marcher" <<endl;
        //else cout << "envoi reussit" <<endl;

        sock_err = send(sock, chaine, str_envoi.length()+1, 0);
        if (sock_err == -1) cout << "sa a pas marcher" <<endl;
        //else cout << "envoi reussit" <<endl;
    }

}


void reception(SOCKET sock)
{
    while(1)
    {
        //cout << "bite" <<endl;
        char taille[4];
        int sock_err, taille_int;
        char *buffer = (char*)calloc(8, sizeof(char));
        sock_err = recv(sock, taille, 4, 0);
        if (sock_err != -1)
        {
            taille_int = atoi(taille);
            buffer = (char*)realloc(buffer, taille_int);
            if (sock_err = recv(sock, buffer, taille_int,0) != -1) cout << buffer <<endl;
        }
        //else cout << "sa a pas marcher" << endl;
    }
}


int main()
{


    #if defined (WIN32)
        WSADATA WSAData;
        int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);
    #else
        int erreur = 0;
    #endif

    SOCKADDR_IN sin;

    SOCKET sock;

    char IP[100], taille_pseudo[4];

    int taille_int = 0, err;

    string pseudo_str = "";

    FILE *config = NULL;
    config = fopen("config.izi","w");

    if(config != NULL)
    {
        err = fscanf(config, "%d", taille_int);
        if (err < 0)
        {

            cout << "Entrez votre pseudo : ";
            cin >> pseudo_str;
            sprintf(taille_pseudo, "%d", pseudo_str.length()+1);
            cout << taille_pseudo << endl;
            fputs("biteeeeee", config);
            //fputc('\n', config);
            char *pseudo = new char [pseudo_str.length()+1];
            strcpy (pseudo, pseudo_str.c_str());
            fputs(pseudo, config);
        }

        else
        {
            fseek(config, 1, SEEK_CUR);
            char *pseudo = new char [taille_int+1];
            fgets(pseudo, taille_int+1, config);
            cout << pseudo;


        }
    }

    fclose(config);



    std::cout << "Entrez l'IP: ";

    std::cin >> IP;

    if(!erreur)
    {

        /* Création de la socket */
        sock = socket(AF_INET, SOCK_STREAM, 0);

        /* Configuration de la connexion */
        sin.sin_addr.s_addr = inet_addr(IP);
        sin.sin_family = AF_INET;
        sin.sin_port = htons(PORT);

        /* Si le client arrive à se connecter */
        if(connect(sock, (SOCKADDR*)&sin, sizeof(sin)) != SOCKET_ERROR)
            printf("Connexion à %s sur le port %d\n", inet_ntoa(sin.sin_addr), htons(sin.sin_port));
        else
            printf("Impossible de se connecter\n");


    }

    thread thread_envoi(envoi, sock, pseudo_str);
    thread_envoi.detach();
    thread thread_reception(reception, sock);
    thread_reception.join();



}
