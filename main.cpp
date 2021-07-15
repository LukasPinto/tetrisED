#include <cstdlib>
#include <time.h>
#include <sstream>
#include <string>
#include "miniwin.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
using namespace miniwin;

const int TAMANHO = 25; // constante
const int FILAS = 20;
const int COLUMNAS = 10;
//ED tablero
typedef int Tablero[COLUMNAS][FILAS];

struct Coord { int x, y; };
//ED Pieza
struct Pieza {
   Coord orig;     // bloque central (posición absoluta)
   Coord perif[3]; // bloques periféricos (posición relativa)
   int color;

    // n entre 0 y 3 (0 = central, 1-3 = perif.)
};

Coord posicion(int n, const Pieza& P)  {//retorna las coordenadas de cada cuadrado de una pieza
   
   Coord ret = { P.orig.x, P.orig.y };
   if (n != 0) {
      ret.x += P.perif[n-1].x;
      ret.y += P.perif[n-1].y;
   }
   return ret;
}

void cuadrado(int x, int y) {
   rectangulo_lleno(20 + 1 + x * TAMANHO,
                    20 + 1 + y * TAMANHO,
                    20 + x * TAMANHO + TAMANHO,
                    20 + y * TAMANHO + TAMANHO);
}
//interfaz 
void Pintar_Pieza(const Pieza& P) {
   color(P.color);//color para de la pieza
   for (int i = 0; i < 4; i++) {//pinta la pieza en la interfaz grafica
      Coord c = posicion(i,P);
      cuadrado(c.x, c.y);//funcion que pinta la pieza en dicha coordenada
   }
}
//pieza
Coord RotarA_Derecha(const Coord& c) {// multiplica por -1 la coordenada y,lo que significa que lo rota a la derecha
   Coord ret = { -c.y, c.x };
   return ret;
}
//pieza
void RotarA_Derecha(Pieza& P) {//funcion que aplica la rotacion a la pieza, pasando por cada cuadradito
   for (int i = 0; i < 3; i++) {
      P.perif[i] = RotarA_Derecha(P.perif[i]);
   }
}
//tablero
void Vacia_tablero(Tablero& T) {//op del tablero, que se encarga de vacior el tablero.
   for (int i = 0; i < COLUMNAS; i++) {
      for (int j = 0; j < FILAS; j++) {
         T[i][j] = NEGRO; // Esto significa casilla vacía NEGRO = 0
      }
   }
}
//interfaz
void Pintar_Tablero(const Tablero& T) {
   for (int i = 0; i < COLUMNAS; i++) {
      for (int j = 0; j < FILAS; j++) {
         color(T[i][j]);//funcion que recorre cada casilla del tablero y se encarga de pintar el tablero
         cuadrado(i, j);
      }
   }
}
//tablero
//se agrega la pieza al tablero
void Tablero_Agrega_Pieza(Tablero& T, const Pieza& P) {
   for (int i = 0; i < 4; i++) {
      Coord c = posicion(i,P);//se coloca la pieza en sus respectivas coordenadas
      T[c.x][c.y] = P.color;//en la matriz del tablero se colocan los numero del color de la pieza
   }
}
//tablero
bool tablero_colicion(const Tablero& T, const Pieza& P) {/*para comprobar que la pieza se encuentra dentro de la matriz, 
primero se reemplaza la pieza y si su coordenada x o y es mayor a las filas o columnas, significa que esta fuera*/
/*al mismo tiempo esta funcion tambien se encarga de verificar las colisiones con otras piezas*/
/*retorna true si colisiona o si se sale de el trablero, y false si no lo hace*/
   for (int i = 0; i < 4; i++) {
      Coord c = posicion(i,P);
      // Comprobar límites
      if (c.x < 0 || c.x >= COLUMNAS) {
         return true;
      }
      if (c.y < 0 || c.y >= FILAS) {
         return true;
      }
      // verificacion de coliscion con otra pieza
      if (T[c.x][c.y] != NEGRO) {
         return true;
      }
   }
   return false;
}
//piezas
const Coord perifs[7][3] = {
/*coordenadas de los cuadrados perifericos de todas las piezas*/
   { { 1,0 }, { 0,1 }, { 1,1 } }, // cuadrado
   { { 1,0 }, {-1,1 }, { 0,1 } }, // S
   { { 0,1 }, { 1,1 }, {-1,0 } }, // Z
   { { 0,1 }, { 0,-1}, { 1,1 } }, // L
   { { 0,1 }, { 0,-1}, {-1,1 } }, // Lr
   { {-1,0 }, { 1,0 }, { 0,1 } }, // T
   { { 0,1 }, { 0,-1}, { 0,2 } }, // I
};
//pieza
void Nueva_Pieza(Pieza& P) {// se genera una nueva pieza y un nuevo color, utilizando un numero al azar
  P.orig.x = 12;
  P.orig.y = 2;
  P.color = 1 + rand() % 6;//colores al azar desde el 1 al 5, el 0 es el negro
  // Pieza al azar
  int r = rand() % 7;
  for (int i = 0; i < 3; i++) {
     P.perif[i] = perifs[r][i];
  }
}
//juego
bool Fila_llena_Tablero(const Tablero& T, int fila) {// funcion que verifica que se hizo tetris, 
   for (int i = 0; i < COLUMNAS; i++) {
      if (T[i][fila] == NEGRO) return false;//si alguna casilla de la fila es negro, no hay tetris
   }
   return true;
}
//tablero, se encarga de bajar todo el tablero una posicion despues de verificar si se hizo tetris
void tetris(Tablero& T, int fila) {
   // Copiar de abajo a arriba
   for (int j = fila; j > 0; j--) {
      for (int i = 0; i < COLUMNAS; i++) {
         T[i][j] = T[i][j-1];
      }
   }
   // Vaciar la de arriba
   for (int i = 0; i < COLUMNAS; i++) {
      T[i][0] = NEGRO;
   }
}
//tablero/juego, verifica si se llego al tope del tablero y ademas se encarga de actualizar el tablero en cada iteracion
/*ademas retorna un contador que se utilizara para el puntaje*/
int Cuenta_Lineas_Tablero(Tablero& T) {
   int fila = FILAS - 1, cont = 0;
   while (fila >= 0) {
      if (Fila_llena_Tablero(T, fila)) {
         tetris(T, fila);
         cont++;
      } else {
         fila--;
      }
   }
   return cont;
}


//funcion que actualiza la interfaz
void Re_Pintar(const Tablero& T, const Pieza& p, const Pieza& sig,
             int puntos, int nivel)
{
   const int ancho = TAMANHO * COLUMNAS;
   const int alto  = TAMANHO * FILAS;
   borra();
   Pintar_Tablero(T);
   color_rgb(128, 128, 128);
   linea(20, 20, 20, 20 + alto);
   linea(20, 20 + alto,20 + ancho, 20 + alto);
   linea(20 + ancho, 20 + alto, 20 + ancho, 20);
   linea(20 + ancho, 620, 20 + ancho, 550);
   linea(20,620,20+ancho,620);
   linea(20,550,20+ancho,550);
   linea(20,550,20,620);
   linea(20+ancho+40,550,20+ancho+160,550);
   linea(20+ancho+160,550,20+ancho+160,620);
   linea(20+ancho+40,550,20+ancho+40,620);
   linea(20+ancho+40,620,20+ancho+160,620);
   
   texto(20 , 550, " arriba:        rotar pieza");
   texto(20 , 565, " izquierda: mover a la izquierda");
   texto(20 , 580, " derecha:      mover a la derecha");
   texto(20 , 595, " abajo:             bajar más rapido");
  texto(ancho+65, 200, "Siguiente Pieza: ");
   color(BLANCO);
   char puntos_aux[5];
   char niveles[5];
   itoa(nivel+1,niveles,10);
   itoa(puntos,puntos_aux,10);
   texto(20+ancho+45, 555, "Nivel:");
   texto(20+ancho+45, 590, "Puntos");
   texto(20+ancho+100, 555, niveles);
   texto(20+ancho+100, 590, puntos_aux);
   Pintar_Pieza(p);
   Pintar_Pieza(sig);
   refresca();
}
//juego
const int puntos_limite[10] = {
   10, 20, 30, 40, 50, 70, 80, 90, 100, 110
};
//juego
const int tics_nivel[10] = {
   33, 25, 20, 18, 16, 14, 12, 10, 8, 2
};
//pantalla de fin del juego
void game_over() {
   color(BLANCO);
   texto(140, 240, "GAME OVER!");
   refresca();
   espera(1000);
   vcierra();
}


int main() {
   vredimensiona(TAMANHO * COLUMNAS + 250, TAMANHO * FILAS + 250);
   srand(time(0)); // Inicializar los números al azar (poner la semilla)

   int tic = 0, puntos = 0, nivel = 0;
// se crea todo lo necesario para la ,ejecucion del juego
   Tablero T;
   Vacia_tablero(T);
   Pieza c, sig;
   Nueva_Pieza(c);
   Nueva_Pieza(sig);
   c.orig.x = 5;
   sig.orig.x=13;
   sig.orig.y=10;

   Re_Pintar(T, c, sig, puntos, nivel);

   int t = tecla();
   //pantalla de inicio del juego
   while(t==NINGUNA){
      texto(65, 240, "presione cualquier tecla ");
      t=tecla();
   }

//ejecucion del juego
   while (t != ESCAPE) {
      // 0. Copiar la pieza actual
      Pieza copia = c;
      

      if (t == NINGUNA && tic > tics_nivel[nivel]) {
         tic = 0;
         t = ABAJO;
      }

      if (t == int('N')) {
         nivel++;
      }

      
      if (t == ABAJO) {//baja la pieza una fila
         c.orig.y++;
      } else if (t == ARRIBA) {//se rota la pieza 
         RotarA_Derecha(c);
      //dependiendo la flecha se mueve el origen hacia ese lado(izquierda o derecha)
      } else if (t == DERECHA) {
         c.orig.x++;
      } else if (t == IZQUIERDA) {
         c.orig.x--;
      }
      // 2. verifica si hay colisiones en cada ciclo
      if (tablero_colicion(T, c)) {
         c = copia;//en caso de haber colision se inserta la pieza en el tablero y se genera una nueva y la siguiente
         if (t == ABAJO) {
            Tablero_Agrega_Pieza(T, c);
            int cont = Cuenta_Lineas_Tablero(T);//dependiendo el nivel hay una nueva velocidad de bajada de la pieza
            puntos += cont * cont;
            if (puntos > puntos_limite[nivel]) {
               nivel++;
            }
            c = sig;
            Nueva_Pieza(sig);
            c.orig.x = 5;
            
            if (tablero_colicion(T, c)) {//si la pieza queda fuera de el tablero se termina el juego
               game_over();
            }
         }
      }

      // Re_Pintarr
      if (t != NINGUNA) {
            sig.orig.x=13;//genera la pieza siguiente en cierta coordenada para poder mostrarla 
         sig.orig.y=10;
         Re_Pintar(T, c, sig, puntos, nivel);
         sig.orig.x=0;//luego de insertar la pieza actual, la pieza siguiente pasa a ser la que esta en juego
         sig.orig.y=1;
      }

      espera(30);
      tic++;//velocidad a la que se ejecuta el juego
      t = tecla();//variable que detecta la tecla
      
   }
   vcierra();//cerrar ventana
   return 0;
}
