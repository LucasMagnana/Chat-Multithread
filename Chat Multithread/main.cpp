#include <iostream>
#include "Serveur.h"

using namespace std;

int main()
{
    Serveur serveur(25565);
    serveur.initialisation();
    serveur.demarrage();
    return 0;
}
