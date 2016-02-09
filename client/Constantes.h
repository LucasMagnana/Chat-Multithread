#ifndef CONSTANTES_H_INCLUDED
#define CONSTANTES_H_INCLUDED

#define taille_case 71
#define nb_cases_hauteur 12
#define nb_cases_largeur 19
#define case_finale_x 9
#define case_finale_y 11
#define nb_obstacle_min 13
#define nb_obstacle_max 19
#define PORT 25565
#define TAILLE 10

#define prix_tourelle 100
#define prix_obstacle_cloutee 150


enum{VIDE, TOURELLE, ZOMBIE, CLOUS, OBSTACLE, HERO_DEFENSE=6, HERO_ZOMBIE=7, OBSTACLE_CLOUTEE}; ///ATENTION ZOMBIE + CLOUS = 5
enum{FULL_FOG,HALF_FOG,VISION};
enum{JOUR, NUIT};
enum{SOLO, SERVEUR, CLIENT};
enum{UP,DOWN,LEFT,RIGHT};


enum{CONTROL_ZOMBIE,CONTROL_DEFENSE,CONTROL_SOLO_DEFENSE};





#endif // CONSTANTES_H_INCLUDED
