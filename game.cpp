#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <iostream>
#include <string.h>
#include <time.h> 

using namespace std; 

// notes 
  // Para habilitar WSL: Ubuntu
  // echo 0 | sudo tee /proc/sys/kernel/yama/ptrace_scope
  // Para correr C++
  // g++ ./game.cpp -o game && ./game

// TODO 
  // Faltan algunos mensajes 


//* ===================================================
//*             Variables y Constantes 
//* ===================================================

const int DIM       = 12;
int cantParedes     = 30;
int cantObstaculos  = 30;
int cantPastillas   = 5;
int cantEnemigos    = 1;
int cantBurbujas    = 0;
int puntosObtenidos = 0;

int filAvatar  = 0;
int colAvatar  = 0;

int filFantasma  = 0;
int colFantasma  = 0;

char avatar       = 'C';  // Avatar del pj
char avatarMuerto = 'X';  //  <-- temporal 
char burbuja      = '*';  // Puntos
char paredIndest  = '0';  // paredIndest indestructible 
char vacio        = ' '; 
char pastilla     = '@';  // x2 a los mov enemigos
char fantasma     = 'B';

struct TipoObstaculo { 
  char obstaculo[3]; // [0] -> 3 vidas (obs1), [1] -> 2 vidas (obs2), [2] -> 1 vida (obs1)
  int vidaMaxima;
};
char obs1       = '/';  // Obstaculo intacto (fase 1)
char obs2       = '>';  // Obstaculo dañado  (fase 2)
char obs3       = '-';  // Obstaculo fragil  (fase 3)
TipoObstaculo pared = {{obs1, obs2, obs3}, 3};

int vidaElemento[DIM][DIM]; // almacena los puntos de vida de cada obstaculo  

bool bonusMov = false; 
const int TURNOS       = 3;
const int TURNOS_BONUS = 6;

//* ===================================================
//*   Declaracion de Funciones 
//* ===================================================

void crearMapa(char mapa[][DIM]);
void limpiarMapa(char mapa[][DIM]);
void mostarMapa(char mapa[][DIM], int tam);
void mostrarMapaObs(int mapaObs[][DIM], int tam); // <-- temporal
void mostrarContador();                           // <-- temporal

void cargarElemento(char mapa[][DIM], char elemento, int cant);
void cargarFantasma(char mapa[][DIM], char e, int cant);
void destruirParedes(char mapa[][DIM]);

void encontrarAvatar(char mapa[][DIM]);
void leerEntrada(char str[], int dim);
void procesarMovimientoJugador(char mapa[][DIM], char mov, bool &gameOver);

void encontrarFantasma(char mapa[][DIM]);
bool puedeMoverse(char mapa[][DIM], int fil, int col, char direccion);
void procesarMovimientoMaquina(char mapa[][DIM], char movMaquina, bool bonusMov, int &turnosMaquina, bool &gameOver);


//* ==================================================
//*             *** Programa Principal ***
//* ==================================================

int main(){
  srand(time(NULL)); 

  char mapa[DIM][DIM];

  char inputUser[10];
  char mov;
  bool finalizo      = false;
  bool gameOver      = false;
  printf("--- NUEVA PARTIDA ---\n");
  crearMapa(mapa);

  do  {
    bool entradaValida = true;

    // system("clear"); //TODO - eliminar 
    printf("\n");

    mostrarContador();
    mostarMapa(mapa, DIM);
    // mostrarMapaObs(vidaElemento, DIM); // DEBUG 

    //* Turno Usuario
    printf("\nUtilice WASD para moverse, o puede digitar - Rendirse- para terminar: \n");
    // printf("--- Turno Usuario \n"); // DEBUG 

    leerEntrada(inputUser, 10);
    // printf("Escribiste: %s\n\n", inputUser); // DEBUG 

    if (strcmp(inputUser, "W") == 0 || strcmp(inputUser, "w") == 0) {
      encontrarAvatar(mapa);
      procesarMovimientoJugador(mapa, 'W', gameOver);
    }
    else if (strcmp(inputUser, "A") == 0 || strcmp(inputUser, "a") == 0) {
      encontrarAvatar(mapa);
      procesarMovimientoJugador(mapa, 'A', gameOver);
    }
    else if (strcmp(inputUser, "S") == 0 || strcmp(inputUser, "s") == 0) {
      encontrarAvatar(mapa);
      procesarMovimientoJugador(mapa, 'S', gameOver);
    }
    else if (strcmp(inputUser, "D") == 0 || strcmp(inputUser, "d") == 0) {
      encontrarAvatar(mapa);
      procesarMovimientoJugador(mapa, 'D', gameOver);
    }
    else if (strcmp(inputUser, "Rendirse") == 0 || strcmp(inputUser, "Q") == 0) { //TODO- Borrar el caso para 'Q'
      finalizo = true;
    } else {
        printf("\nEntrada inválida, vuelva a intentarlo: \n");
        entradaValida = false;
      }
      
    //* Turno de la Maquina
    printf("--- Turno de la Maquina \n"); // DEBUG 
    if (entradaValida) {
      int turnosMaquina;

      if (!bonusMov) {
        turnosMaquina = TURNOS; // 
      } else {
        turnosMaquina = TURNOS_BONUS;
      }
      
      int i = 0;
      while (i < turnosMaquina) {
        encontrarFantasma(mapa);

        char direcciones[] = {'W', 'S', 'A', 'D'};
        char movMaquina;
        int intentos = 0;
        bool movValido;
        
        do {
          movMaquina = direcciones[rand() % 4];
          // printf("(%d) Mov Maquina: %c : ", i, movMaquina); // DEBUG 
          movValido = puedeMoverse(mapa, filFantasma, colFantasma, movMaquina);
          intentos++;
        } while (!movValido && intentos < 10); // Pongo un maximo de intentos para la Maquina para evitar loops infinitos 

        procesarMovimientoMaquina(mapa, movMaquina, bonusMov, turnosMaquina, gameOver);

        i++;
      }

      bonusMov = false;
    }

    if (cantBurbujas == 0) {
      finalizo = true;
    }
    
    if (gameOver) {
      finalizo = true;
    }
  } while (!finalizo);

  if (gameOver) {
    printf("\nPerdiste, consumiste solo %d burbujas \n", puntosObtenidos);
    mostarMapa(mapa, DIM);
    printf("\n");
    // mostrarMapaObs(vidaElemento, DIM); // DEBUG
  }
  
  printf("Puntos obtenidos %d: ", puntosObtenidos);
  printf("\n");

  // "Win"
  if (cantBurbujas == 0) {
    printf("Lo lograste, consumiste todas las burbujas!\n");
  } 
  
  return 0;
}


//* ===================================================
//*   Funciones auxiliares para el Mapa 
//* ===================================================

void crearMapa(char mapa[][DIM]){
  limpiarMapa(mapa);

  cargarElemento(mapa, avatar, 1);
  cargarElemento(mapa, paredIndest, cantParedes);
  cargarElemento(mapa, obs1, cantObstaculos);
  cargarElemento(mapa, pastilla, cantPastillas);
  cargarFantasma(mapa, fantasma, cantEnemigos);
  
  // Completo los lugares vacios con burbujas
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      if (mapa[i][j] == vacio) {
        mapa[i][j] = burbuja;
        cantBurbujas++;
      }
    }
  }

  // Testeo
  // cargarElemento(mapa, burbuja, 5);
  // cantBurbujas = 5;
  // printf("Cantidad de burbujas generadas: %d", cantBurbujas); // DEBUG 
}

void limpiarMapa(char mapa[][DIM]){
   for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
        mapa[i][j] = vacio;
    }
  }
}

// Muestra todos los elementos en el mapa
void mostarMapa(char mapa[][DIM], int tam){
  // Borede superior
  for (int i = 0; i < tam*2+1; i++) {
    printf("_");
  }
  printf("\n");

  for (int i = 0; i < tam; i++) {
    for (int j = 0; j < DIM; j++) {
      if (j != DIM -1) {
        printf("|%c", mapa[i][j]);
      } else {
        printf("|%c|", mapa[i][j]);
      }
    }
    printf("\n");
  }

  // Borede inferior
  for (int i = 0; i < tam*2+1; i++) {
    printf("-");
  }
  printf("\n");
}

// Muestra solamente los obstaculos y sus puntos de vida
void mostrarMapaObs(int mapaObs[][DIM], int tam){
  for (int i = 0; i < tam; i++) {
    for (int j = 0; j < DIM; j++) {
      if (j != DIM - 1) {
        printf("|%d", mapaObs[i][j]);
      } else {
        printf("|%d|", mapaObs[i][j]);
      }
    }
    printf("\n");
  }
}

// Muesta los puntos obtenidos del usuario la momento
void mostrarContador(){
  for (int i = 0; i < 7; i++) {
    printf("-");
  }
  printf(" PUNTOS: %d ", puntosObtenidos);
  for (int i = 0; i < 7; i++) {
    printf("-");
  }
  printf("\n");
}


//* ===================================================
//*   Funciones auxiliares para el Jugador
//* ===================================================

void leerEntrada(char str[], int dim){
  int i = 0;

  str[0] = getchar();
  while (str[i] != '\n' && i < dim - 1) {
    i++;
    str[i] = getchar();
  }
  
  char ultimoCaracter = str[i];

  str[i] = '\0';

  if (ultimoCaracter != '\n') 
    while (getchar() != '\n');
}

void encontrarAvatar(char mapa[][DIM]){
  int fil = 0, col = 0;
  bool encontrado = false;

  while (!encontrado && fil < DIM) {
    col = 0;

    while (!encontrado && col < DIM)  {
      if (mapa[fil][col] == avatar) {
        filAvatar = fil;
        colAvatar = col;
        encontrado = true;
      }

      if (!encontrado) { 
        col++;
      }
    }

    if (!encontrado) { 
      fil++;
    }
  }
}

// Movimientos posible del Usuario
void procesarMovimientoJugador(char mapa[][DIM], char mov, bool &gameOver){
  switch (mov)  {
    case 'W': 
      if ((filAvatar - 1) >= 0 && mapa[filAvatar - 1][colAvatar] != paredIndest) {
        // Caso: 'Enemigo'
        if (mapa[filAvatar - 1][colAvatar] == fantasma) {
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar--;
          mapa[filAvatar][colAvatar] = avatarMuerto; // Para indicar la posicion donde murio/perdio
          gameOver = true;
          break;     
        } 
        // Caso: 'Puntos'
        else if (mapa[filAvatar - 1][colAvatar] == burbuja) {
          cantBurbujas--;
          puntosObtenidos++;
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar--;
          mapa[filAvatar][colAvatar] = avatar;
        } 
        // Caso: 'Obstaculos'
        else if (mapa[filAvatar - 1][colAvatar] == pared.obstaculo[0] || 
                 mapa[filAvatar - 1][colAvatar] == pared.obstaculo[1] || 
                 mapa[filAvatar - 1][colAvatar] == pared.obstaculo[2] ) {

            int &vidaActual = vidaElemento[filAvatar - 1][colAvatar]; 
            // printf("DEBUG vida antes: %d\n", vidaActual);  //DEBUG 
            vidaActual--; // le resto una vida a la pared
            // printf("DEBUG vida despues: %d, idx: %d, char: %c\n", vidaActual, 3 - vidaActual, pared.obstaculo[3 - vidaActual]); //DEBUG 

            if (vidaActual <= 0) {
              mapa[filAvatar][colAvatar] = vacio;
              filAvatar--;
              mapa[filAvatar][colAvatar] = avatar; 
            } else {
              mapa[filAvatar - 1][colAvatar] = pared.obstaculo[pared.vidaMaxima - vidaActual];  // Cambio el aspecto
            }
        }
        // Caso: Pastilla
        else if (mapa[filAvatar -1][colAvatar] == pastilla){
          // printf("Agarre una pastila! \n"); // DEBUG 
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar--;
          mapa[filAvatar][colAvatar] = avatar;
          destruirParedes(mapa);
          bonusMov = true;  
        }
        // Caso: 'Vacio'
        else {
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar--;
          mapa[filAvatar][colAvatar] = avatar; 
        }
      } 
      break;

    case 'S': 
      if ((filAvatar + 1) < DIM && mapa[filAvatar + 1][colAvatar] != paredIndest) {
        // Caso: 'Enemigo'
        if (mapa[filAvatar + 1][colAvatar] == fantasma) {
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar++;
          mapa[filAvatar][colAvatar] = avatarMuerto; 
          gameOver = true;
          break;
        
        } 
        // Caso: 'Puntos'
        else if (mapa[filAvatar + 1][colAvatar] == burbuja) {
          cantBurbujas--;
          puntosObtenidos++;
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar++;
          mapa[filAvatar][colAvatar] = avatar;

        } 
        // Caso: 'Obstaculos'
        else if (mapa[filAvatar + 1][colAvatar] == pared.obstaculo[0] || 
                 mapa[filAvatar + 1][colAvatar] == pared.obstaculo[1] || 
                 mapa[filAvatar + 1][colAvatar] == pared.obstaculo[2] ){
          int &vidaActual = vidaElemento[filAvatar + 1][colAvatar];
          // printf("DEBUG vida antes: %d\n", vidaActual);  // DEBUG 
          vidaActual--;
          // printf("DEBUG vida despues: %d, idx: %d, char: %c\n", vidaActual, 3 - vidaActual, pared.obstaculo[3 - vidaActual]); // DEBUG 
        
          if (vidaActual <= 0) {
            mapa[filAvatar][colAvatar] = vacio;
            filAvatar++;
            mapa[filAvatar][colAvatar] = avatar;
          } else {
            mapa[filAvatar + 1][colAvatar] = pared.obstaculo[pared.vidaMaxima - vidaActual];
          }
        
        } 
        // Caso: Pastilla
        else if (mapa[filAvatar + 1][colAvatar] == pastilla){
          // printf("Agarre una pastila! \n"); // DEBUG 
           mapa[filAvatar][colAvatar] = vacio;
          filAvatar++;
          mapa[filAvatar][colAvatar] = avatar;
          destruirParedes(mapa);
          bonusMov = true;  
        }
        // Caso: 'Vacio'
        else {
          mapa[filAvatar][colAvatar] = vacio;
          filAvatar++;
          mapa[filAvatar][colAvatar] = avatar; 
          }
      } 
      break;

    case 'A': 
      if ((colAvatar - 1) >= 0 && mapa[filAvatar][colAvatar - 1] != paredIndest) {
        // Caso: 'Enemigo'
        if (mapa[filAvatar][colAvatar - 1] == fantasma) {
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar--;
          mapa[filAvatar][colAvatar] = avatarMuerto; 
          gameOver = true;
          break;

        } 
        // Caso: 'Puntos'          
        else if (mapa[filAvatar][colAvatar - 1] == burbuja) {
          cantBurbujas--;
          puntosObtenidos++;
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar--;
          mapa[filAvatar][colAvatar] = avatar;
          
        } 
        // Caso: 'Obstaculos'
        else if (mapa[filAvatar][colAvatar - 1] == pared.obstaculo[0]  || 
                 mapa[filAvatar][colAvatar - 1] == pared.obstaculo[1]  || 
                 mapa[filAvatar][colAvatar - 1] == pared.obstaculo[2]  ){ 
          int &vidaActual = vidaElemento[filAvatar][colAvatar - 1];
          // printf("DEBUG vida antes: %d\n", vidaActual);  // DEBUG 
          vidaActual--;
          // printf("DEBUG vida despues: %d, idx: %d, char: %c\n", vidaActual, 3 - vidaActual, pared.obstaculo[3 - vidaActual]); // DEBUG 

          if (vidaActual <= 0) {
            mapa[filAvatar][colAvatar] = vacio;
            colAvatar--;
            mapa[filAvatar][colAvatar] = avatar;
          } else {
            mapa[filAvatar][colAvatar - 1] = pared.obstaculo[pared.vidaMaxima - vidaActual];
          }

        }
        // Caso: Pastilla
        else if(mapa[filAvatar][colAvatar - 1] == pastilla){
          // printf("Agarre una pastila! \n"); // DEBUG 
           mapa[filAvatar][colAvatar] = vacio;
          colAvatar--;
          mapa[filAvatar][colAvatar] = avatar;
          destruirParedes(mapa);
          bonusMov = true;  
        }
        // Caso: 'Vacio'
        else {
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar--;
          mapa[filAvatar][colAvatar] = avatar; 
        }
      } 
      break;

    case 'D': 
      if ((colAvatar + 1) < DIM && mapa[filAvatar][colAvatar + 1] != paredIndest) {
        // Caso: 'Enemigo'
        if (mapa[filAvatar][colAvatar + 1] == fantasma) {
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar++;
          mapa[filAvatar][colAvatar] = avatarMuerto; 
          gameOver = true;
          break;

        } 
        // Caso: 'Puntos'          
        else if (mapa[filAvatar][colAvatar + 1] == burbuja) {
          cantBurbujas--;
          puntosObtenidos++;
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar++;
          mapa[filAvatar][colAvatar] = avatar;

        } 
        // Caso: 'Obstaculos'
        else if (mapa[filAvatar][colAvatar + 1] == pared.obstaculo[0] ||
                 mapa[filAvatar][colAvatar + 1] == pared.obstaculo[1] ||
                 mapa[filAvatar][colAvatar + 1] == pared.obstaculo[2] ){
          int &vidaActual = vidaElemento[filAvatar][colAvatar + 1];
          // printf("DEBUG vida antes: %d\n", vidaActual);  // DEBUG 
          vidaActual--;
          // printf("DEBUG vida despues: %d, idx: %d, char: %c\n", vidaActual, 3 - vidaActual, pared.obstaculo[3 - vidaActual]); // DEBUG 

          if (vidaActual <= 0) {
            mapa[filAvatar][colAvatar] = vacio;
            colAvatar++;
            mapa[filAvatar][colAvatar] = avatar;
          } else {
            mapa[filAvatar][colAvatar + 1] = pared.obstaculo[pared.vidaMaxima - vidaActual];
          }

        } 
        // Caso: Pastilla
        else if(mapa[filAvatar][colAvatar + 1] == pastilla){
          // printf("Agarre una pastila! \n"); // DEBUG 
           mapa[filAvatar][colAvatar] = vacio;
          colAvatar++;
          mapa[filAvatar][colAvatar] = avatar;
          destruirParedes(mapa);
          bonusMov = true;  
        }
        // Caso: 'Vacio'
        else {
          mapa[filAvatar][colAvatar] = vacio;
          colAvatar++;
          mapa[filAvatar][colAvatar] = avatar; 
        }
      } 
      break;
    
    case 'Q': //TODO Borrar 
      break;
  }
}


//* ===================================================
//*   Funciones auxiliares para la Maquina
//* ===================================================

void encontrarFantasma(char mapa[][DIM]){
  int fil = 0, col = 0;
  bool encontrado = false;

  while (!encontrado && fil < DIM) {
    col = 0;
    
    while (!encontrado && col < DIM) {
      if (mapa[fil][col] == fantasma){
        filFantasma = fil;
        colFantasma = col;
        // printf("\n");   // DEBUG
        // printf("Pos Fantastma Fila: %d\n", filFantasma);   // DEBUG
        // printf("Pos Fantastma Columna: %d\n", colFantasma);// DEBUG
        encontrado = true;
      }

      if (!encontrado) { 
          col++;
      }
    }

    if (!encontrado) { 
      fil++;
    }
  }
}

// Detecta si el movimiento es efectivo o no
bool puedeMoverse(char mapa[][DIM], int fil, int col, char direccion) {
  int filDestino = fil;
  int colDestino = col;

  switch (direccion) {
    case 'W': filDestino--; break;
    case 'S': filDestino++; break;
    case 'A': colDestino--; break;
    case 'D': colDestino++; break;
    default:  return false;
  }

  // Fuera de los límites del mapa
  if (filDestino < 0 || filDestino >= DIM || colDestino < 0 || colDestino >= DIM) {
    return false;
  }

  char celda = mapa[filDestino][colDestino];

  // Paredes y obstáculos
  if (celda == paredIndest ||
      celda == pared.obstaculo[0] ||
      celda == pared.obstaculo[1] ||
      celda == pared.obstaculo[2]) {
    return false;
  }

  return true;
}

// Movimientos posible de la Maquina
void procesarMovimientoMaquina(char mapa[][DIM], char movMaquina, bool bonusMov, int &turnosMaquina, bool &gameOver){

  switch (movMaquina)  {
    case 'W':
      // printf("Se mueve hacia arriba \n"); // DEBUG 
      // Paredes y obstaculos 
      if ((filFantasma - 1) >= 0 && 
              mapa[filFantasma - 1][colFantasma] != paredIndest &&
              mapa[filFantasma - 1][colFantasma] != pared.obstaculo[0] &&
              mapa[filFantasma - 1][colFantasma] != pared.obstaculo[1] &&
              mapa[filFantasma - 1][colFantasma] != pared.obstaculo[2]) {

        // Avatar
        if (mapa[filFantasma -1][colFantasma] == avatar) {
          mapa[filFantasma][colFantasma] = vacio;
          filFantasma--;
          mapa[filFantasma][colFantasma] = avatarMuerto;
          gameOver = true;
          break;     
        } 
        // Punto
        else if (mapa[filFantasma - 1][colFantasma] == burbuja) {
          mapa[filFantasma][colFantasma] = burbuja;
          filFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;

        }
        // Pastilla
        else if (mapa[filFantasma - 1][colFantasma] == pastilla) {
          mapa[filFantasma][colFantasma] = pastilla;
          filFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;
        }
        // Vacio
        else {
          mapa[filFantasma][colFantasma] = vacio;
          filFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;
        }
      } 
      // Si no se puede mover genera un turno extra
      else {
      printf("Turno extra \n"); // DEBUG 
      turnosMaquina++;
    } 
      break;

    case 'S':
      // printf("Se mueve hacia abajo \n"); // DEBUG 
      if ((filFantasma + 1) < DIM &&
              mapa[filFantasma + 1][colFantasma] != paredIndest &&
              mapa[filFantasma + 1][colFantasma] != pared.obstaculo[0] &&
              mapa[filFantasma + 1][colFantasma] != pared.obstaculo[1] &&
              mapa[filFantasma + 1][colFantasma] != pared.obstaculo[2]) {

        // Avatar
        if (mapa[filFantasma + 1][colFantasma] == avatar) {
          mapa[filFantasma][colFantasma] = vacio;
          filFantasma++;
          mapa[filFantasma][colFantasma] = avatarMuerto;
          gameOver = true;
          break;     
        } 
        // Punto
        else if (mapa[filFantasma + 1][colFantasma] == burbuja) {
          mapa[filFantasma][colFantasma] = burbuja;
          filFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;

        }
        // Pastilla
        else if (mapa[filFantasma + 1][colFantasma] == pastilla) {
          mapa[filFantasma][colFantasma] = pastilla;
          filFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;
        }
        // Vacio
        else {
          mapa[filFantasma][colFantasma] = vacio;
          filFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;
        }
      } 
      // Si no se puede mover genera un turno extra
      else {
        printf("Turno extra \n"); // DEBUG 
        turnosMaquina++;
      }  
      break;
    case 'A':
      // printf("mueve hacia izquierda \n"); // DEBUG 
        if ((colFantasma - 1) >= 0 &&
          mapa[filFantasma][colFantasma - 1] != paredIndest &&
          mapa[filFantasma][colFantasma - 1] != pared.obstaculo[0] &&
          mapa[filFantasma][colFantasma - 1] != pared.obstaculo[1] &&
          mapa[filFantasma][colFantasma - 1] != pared.obstaculo[2]) {

        // Avatar
        if (mapa[filFantasma][colFantasma - 1] == avatar) {
          mapa[filFantasma][colFantasma] = vacio;
          colFantasma--;
          mapa[filFantasma][colFantasma] = avatarMuerto;
          gameOver = true;
          break;     
        } 
        // Punto
        else if (mapa[filFantasma][colFantasma - 1] == burbuja) {
          mapa[filFantasma][colFantasma] = burbuja;
          colFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;

        }
        // Pastilla
        else if (mapa[filFantasma][colFantasma - 1] == pastilla) {
          mapa[filFantasma][colFantasma] = pastilla;
          colFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;
        }
        // Vacio
        else {
          mapa[filFantasma][colFantasma] = vacio;
          colFantasma--;
          mapa[filFantasma][colFantasma] = fantasma;
        }
      } 
      // Si no se puede mover genera un turno extra
      else {
        printf("Turno extra \n"); // DEBUG 
        turnosMaquina++;
      }  
    break;
    case 'D':
      // printf("Se mueve hacia derecha \n"); // DEBUG 
       if ((colFantasma + 1) < DIM &&
          mapa[filFantasma][colFantasma + 1] != paredIndest &&
          mapa[filFantasma][colFantasma + 1] != pared.obstaculo[0] &&
          mapa[filFantasma][colFantasma + 1] != pared.obstaculo[1] &&
          mapa[filFantasma][colFantasma + 1] != pared.obstaculo[2]) {

        // Avatar
        if (mapa[filFantasma][colFantasma + 1] == avatar) {
          mapa[filFantasma][colFantasma] = vacio;
          colFantasma++;
          mapa[filFantasma][colFantasma] = avatarMuerto;
          gameOver = true;
          break;     
        } 
        // Punto
        else if (mapa[filFantasma][colFantasma + 1] == burbuja) {
          mapa[filFantasma][colFantasma] = burbuja;
          colFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;

        }
        // Pastilla
        else if (mapa[filFantasma][colFantasma + 1] == pastilla) {
          mapa[filFantasma][colFantasma] = pastilla;
          colFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;
        }
        // Vacio
        else {
          mapa[filFantasma][colFantasma] = vacio;
          colFantasma++;
          mapa[filFantasma][colFantasma] = fantasma;
        }
      } 
      // Si no se puede mover genera un turno extra
      else {
        printf("Turno extra \n"); // DEBUG 
        turnosMaquina++;
      }  
      break;
  
  default:
    break;
  }
}


//* ===================================================
//*   Otras funciones auxiliares 
//* ===================================================

// Carga al mapa una cantidad de elementos (avatar, burbuja, paredIndest, obs1 = (pared.obstaculo[0]), pastilla)
void cargarElemento(char mapa[][DIM], char elemento, int cant){
  while (cant > 0) {
    int i = rand() % DIM;
    int j = rand() % DIM;

    if (mapa[i][j] == vacio){

      if (elemento == pared.obstaculo[0]) {
        mapa[i][j] = pared.obstaculo[0];
        vidaElemento[i][j] = 3; 

      } else {
        mapa[i][j] = elemento;
        vidaElemento[i][j] = 0; 
      }
      cant--;
    }    
  }
}

// Los enemigos son cargados sobre una burbuja o pastilla
void cargarFantasma(char mapa[][DIM], char e, int cant){
  while (cant > 0)  {
    int i = rand() % DIM;
    int j = rand() % DIM;

    if (mapa[i][j] == burbuja || mapa[i][j] == pastilla) {
      mapa[i][j] = fantasma;
      cant--;
    }
  }
}

void destruirParedes(char mapa[][DIM]) {
  int paredesDestruidas = 0;
  while (paredesDestruidas < 5) {
    int filRand = rand() % DIM;
    int colRand = rand() % DIM;

    if (mapa[filRand][colRand] == paredIndest) {
      mapa[filRand][colRand] = vacio;
      paredesDestruidas++;
      // printf("(%d) Destrui una pared en [%d][%d]\n", paredesDestruidas, filRand, colRand); // DEBUG 
    }
  }
}