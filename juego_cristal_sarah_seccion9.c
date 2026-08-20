#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <conio.h>
#include <time.h>

#define TIERRA  "\xF0\x9F\x9F\xA9" 
#define VACIO   "  "             
#define CEREZA  "\xF0\x9F\x8D\x92" 
#define JUGADOR "\xF0\x9F\xA4\xA1" 
#define MANZANA "\xF0\x9F\x8D\x8E"
#define PASTEL  "\xF0\x9F\x8E\x82" 
#define ENEMIGO_LENTO "\xF0\x9F\x91\xBE"
#define ENEMIGO_RAPIDO "\xF0\x9F\x98\x88"
#define ALPHAMONSTER "\xF0\x9F\x91\xBB"
#define DIAMANTE "\xF0\x9F\x92\x8E"
#define MAX_ENEMIGOS 12
#define PELOTA "\xF0\x9F\x94\xB4"

typedef struct{
  int posicionFila;
  int posicionColumna;
  int ultimaDirFila;
  int ultimaDirColumna;
  int vidas; 
  int puntos;
  char* emoji;
  int cooldownInmunidad;
} jugador; 

typedef enum {
    TIPO_ROJO,
    TIPO_AZUL,
    TIPO_ALPHA
} TipoEnemigo;

typedef struct {
    int posicionFila;
    int posicionColumna;
    int activo;
    TipoEnemigo tipo;
    int velocidad; 
} Enemigo;

typedef struct {
    int posicionFila;
    int posicionColumna;
    int dirFila;
    int dirColumna;
    int activa; 
} Pelota;

char*** crearMapa(int filas, int columnas);
void imprimirMapa(char*** mapa, int filas, int columnas, int puntos, int vidas, int tiempoModoRojo); 
void limpiarMemoria(char*** mapa, int filas, int columnas);
jugador crearPersonaje(int filaInicial, int columnaInicial);
void controlarPersonaje(char*** mapa, jugador* j, int filas, int columnas, int* nivelCompletado, int* tiempoModoRojo, Enemigo* enemigos, int* cantEnemigos, Pelota* p); 
void moverPersonaje(char*** mapa, jugador* j, int nuevaFila, int nuevaColumna, int filas, int columnas, int* nivelCompletado, int* tiempoModoRojo, Enemigo* enemigos, int* cantEnemigos);
void moverEnemigos(char*** mapa, Enemigo* enemigos, int cantEnemigos, jugador* j, int filas, int columnas, int contadorCiclos);
void generarFilaAlphaModoRojo(char*** mapa, Enemigo* enemigos, int* cantEnemigos, int filas, int columnas);
void moverPelota(char*** mapa, Pelota* p, jugador* j, Enemigo* enemigos, int cantEnemigos, int filas, int columnas);
int menuSeleccionarNivel();
int hayCerezas(char*** mapa, int filas, int columnas);
void pantallaVictoria(int puntos);
int pantallaGameOver(int puntos);

int main() {
    SetConsoleOutputCP(CP_UTF8);
    srand(time(NULL));
    int filasNivel1 = 11, columnasNivel1 = 11;
    int filasNivel2 = 15, columnasNivel2 = 13;
    int filasNivel3 = 19, columnasNivel3 = 15;
    
   int jugarDeNuevo = 1;
   while (jugarDeNuevo) {
    int nivelInicial = menuSeleccionarNivel();
    jugador j;
    int jugadorCreado = 0;
    int gameOver = 0;
    for (int nivel = nivelInicial; nivel <= 3; nivel++) {
            
        int filas, columnas;
        if (nivel == 1) {
            filas = filasNivel1;
            columnas = columnasNivel1;
        } else if (nivel == 2) {
            filas = filasNivel2;
            columnas = columnasNivel2;
        } else {
            filas = filasNivel3;
            columnas = columnasNivel3;
        }

        int tiempoModoRojo = 0;
        int contadorCiclos = 0;
        int esperaEnemigos = 75;
        Pelota pelota = {0, 0, 0, 0, 0};
    
        char*** mapa = crearMapa(filas, columnas);
        
        int filaAleatoria = 1 + rand() % (filas - 2);
        int columnaAleatoria = 1 + rand() % (columnas - 2);
        
      if (!jugadorCreado) {
            j = crearPersonaje(filaAleatoria, columnaAleatoria);
            jugadorCreado = 1;
        } else {
            j.posicionFila = filaAleatoria;
            j.posicionColumna = columnaAleatoria;
        }
        mapa[j.posicionFila][j.posicionColumna] = j.emoji;
        
        int cantEnemigos = 3;
       
        Enemigo enemigos[MAX_ENEMIGOS];
        
        for (int e = 0; e < 3; e++) {
            int ef, ec;
            do {
                ef = 1 + rand() % (filas - 2);
                ec = 1 + rand() % (columnas - 2);
            } while (abs(ef - j.posicionFila) < 3 && abs(ec - j.posicionColumna) < 3); 

            enemigos[e].posicionFila = ef;
            enemigos[e].posicionColumna = ec;
            enemigos[e].activo = 1;
            
            if (e == 0) {
                enemigos[e].tipo = TIPO_ROJO;
                enemigos[e].velocidad = 12;
                mapa[ef][ec] = ENEMIGO_LENTO;
            } else if (e == 1) {
                enemigos[e].tipo = TIPO_AZUL;
                enemigos[e].velocidad = 7;
                mapa[ef][ec] = ENEMIGO_RAPIDO;
            } else {
                enemigos[e].tipo = TIPO_ALPHA;
                enemigos[e].velocidad = 8;
                mapa[ef][ec] = ALPHAMONSTER;
            }
        }
        
        int nivelCompletado = 0; 

        system("cls");
        
        while (_kbhit()) {
    _getch();
}

        while (!nivelCompletado && j.vidas > 0) { 
            imprimirMapa(mapa, filas, columnas, j.puntos, j.vidas, tiempoModoRojo); 
            printf("\n--- NIVEL %d DE 3 ---\n", nivel); 
            if (esperaEnemigos > 0) {
                printf(" PREPARATE! Enemigos congelados por %d segundo(s)...\n", (esperaEnemigos / 50) + 1);
                esperaEnemigos--;
            } else {
                printf("WASD: Mover | ESPACIO: Disparar | Q: Siguiente nivel\n");
            }
            
            controlarPersonaje(mapa, &j, filas, columnas, &nivelCompletado, &tiempoModoRojo, enemigos, &cantEnemigos, &pelota);
            
            if (contadorCiclos % 2 == 0) {
                moverPelota(mapa, &pelota, &j, enemigos, cantEnemigos, filas, columnas);
            }
            
            if (tiempoModoRojo > 0) {
                tiempoModoRojo--;
                
            } else if(esperaEnemigos == 0){
                
                moverEnemigos(mapa, enemigos, cantEnemigos, &j, filas, columnas, contadorCiclos);
            }

            contadorCiclos++; 
            
            Sleep(20); 
        }
            
        limpiarMemoria(mapa, filas, columnas);
        
        if (j.vidas <= 0) {
            gameOver = 1; 
                break; }
            
        system("cls"); 
        if (nivel < 3) {
            printf("\n Nivel completado! Cargando siguiente mapa...\n");
            Sleep(1000);
           }
        }
        if (gameOver) {
            jugarDeNuevo = pantallaGameOver(j.puntos); 
        } else {
            pantallaVictoria(j.puntos);
            jugarDeNuevo = 0; 
        } } return 0; }
        
char*** crearMapa(int filas, int columnas) {
    char*** mapa = (char***) malloc(filas * sizeof(char**));
    if (mapa == NULL) {
        printf("Ocurrio un error en la asignacion de memoria (filas).\n");
        exit(1);
    }

    for (int i = 0; i < filas; i++) {
        mapa[i] = (char**) malloc(columnas * sizeof(char*));
        if (mapa[i] == NULL) { 
            printf("Ocurrio un error en la asignacion de memoria (columnas).\n");
            exit(1);
        }
    }
    
    
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            mapa[i][j] = TIERRA; 
        }
    }

    int centroFila = filas / 2;
    int centroColumna = columnas / 2;
   
    for (int i = 1; i < filas - 1; i++) {
        mapa[i][centroColumna] = VACIO;
    }

    for (int j = 1; j < columnas - 1; j++) {
        mapa[centroFila][j] = VACIO;
    }

   int tunelesExtra = 0;
    while (tunelesExtra < 4) {
        int f = 1 + rand() % (filas - 2);
        int c = 1 + rand() % (columnas - 2);

        if (rand() % 2 == 1) {
            for (int i = 0; i < 4 && (c + i) < columnas - 1; i++) {
                mapa[f][c + i] = VACIO;
            }
        } else {
            for (int i = 0; i < 4 && (f + i) < filas - 1; i++) {
                mapa[f + i][c] = VACIO;
            }
        }
        tunelesExtra++;
    }

    int gruposColocados = 0;
    while (gruposColocados < 5) {
        int cf = 1 + rand() % (filas - 2);
        int cc = 1 + rand() % (columnas - 3);

        if (mapa[cf][cc] == TIERRA && mapa[cf][cc + 1] == TIERRA) {
            mapa[cf][cc] = CEREZA;
            mapa[cf][cc + 1] = CEREZA;
            gruposColocados++;
        }
    }
    int manzanasColocadas = 0;
    while (manzanasColocadas < 4) {
       int mf = 1 + rand() % (filas - 3); 
        int mc = 1 + rand() % (columnas - 2);

        if (mapa[mf][mc] == TIERRA && mapa[mf + 1][mc] == TIERRA) {
            mapa[mf][mc] = MANZANA;
            manzanasColocadas++;
        }
    }


   
    mapa[centroFila][centroColumna] = PASTEL;

    return mapa; 
}

void imprimirMapa(char*** mapa, int filas, int columnas, int puntos, int vidas, int tiempoModoRojo) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD origen = {0, 0};
    SetConsoleCursorPosition(hConsole, origen);
    
    if (tiempoModoRojo > 0) {
       
        SetConsoleTextAttribute(hConsole, BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
    } else {
        
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }
    
    printf("=== MR. DO! - PROTOTIPO ===\n");
    printf("PUNTOS: %d    | VIDAS: %d        \n\n", puntos, vidas);

    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < columnas; j++) {
            printf("%s ", mapa[i][j]);
        }
        printf("\n"); 
    }
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void limpiarMemoria(char*** mapa, int filas, int columnas) {
    for (int i = 0; i < filas; i++) {
        free(mapa[i]);
    }
    free(mapa);
}

jugador crearPersonaje(int filaInicial, int columnaInicial){
    jugador j;
    j.posicionFila = filaInicial; 
    j.posicionColumna = columnaInicial;
    j.vidas = 3;
    j.puntos = 0;
    j.ultimaDirFila = -1;
    j.ultimaDirColumna = 0;
    j.emoji = JUGADOR;
    j.cooldownInmunidad = 0;
    return j;	
}
void generarFilaAlphaModoRojo(char*** mapa, Enemigo* enemigos, int* cantEnemigos, int filas, int columnas) {
    int filaSuperior = 1;
    int columnaCentro = columnas / 2;
    
    int enemigosAGenerar = 3; 

    for (int i = 0; i < enemigosAGenerar && *cantEnemigos < MAX_ENEMIGOS; i++) {
        if (mapa[filaSuperior][columnaCentro] != JUGADOR) {
            enemigos[*cantEnemigos].posicionFila = filaSuperior;
            enemigos[*cantEnemigos].posicionColumna = columnaCentro;
          
            enemigos[*cantEnemigos].tipo = TIPO_AZUL;
            enemigos[*cantEnemigos].activo = 1;
            enemigos[*cantEnemigos].velocidad = 6;
            
            mapa[filaSuperior][columnaCentro] = ENEMIGO_RAPIDO;
            (*cantEnemigos)++;
        }
    }
}


void controlarPersonaje(char*** mapa, jugador* j, int filas, int columnas, int* nivelCompletado, int* tiempoModoRojo, Enemigo* enemigos, int* cantEnemigos, Pelota* p) {
    while (_kbhit()) {
        int nuevaFila = j->posicionFila;
        int nuevaColumna = j->posicionColumna;
        
        char tecla = _getch(); 

        if (tecla == 'w' || tecla == 'W') {
            nuevaFila--;
            j->ultimaDirFila = -1;
            j->ultimaDirColumna = 0;
        }
        if (tecla == 's' || tecla == 'S') {
            nuevaFila++;
            j->ultimaDirFila = 1;
            j->ultimaDirColumna = 0;
        }
        if (tecla == 'a' || tecla == 'A') {
            nuevaColumna--;
            j->ultimaDirFila = 0;
            j->ultimaDirColumna = -1;
        }
        if (tecla == 'd' || tecla == 'D') {
            nuevaColumna++;
            j->ultimaDirFila = 0;
            j->ultimaDirColumna = 1;
        }
        if (tecla == 'q' || tecla == 'Q') *nivelCompletado = 1;

      
        if (tecla == ' ') {
            if (!p->activa) {
                p->posicionFila = j->posicionFila;
                p->posicionColumna = j->posicionColumna;
                p->dirFila = j->ultimaDirFila;
                p->dirColumna = j->ultimaDirColumna;
                p->activa = 1;
            }
        }
        
        if (nuevaFila >= 0 && nuevaFila < filas && nuevaColumna >= 0 && nuevaColumna < columnas) {
            moverPersonaje(mapa, j, nuevaFila, nuevaColumna, filas, columnas, nivelCompletado, tiempoModoRojo, enemigos, cantEnemigos);
        }
    }
}

void moverPersonaje(char*** mapa, jugador* j, int nuevaFila, int nuevaColumna, int filas, int columnas, int* nivelCompletado, int* tiempoModoRojo, Enemigo* enemigos, int* cantEnemigos) {
    
    if (j->cooldownInmunidad > 0) {
        j->cooldownInmunidad--;
    }

  
    for (int e = 0; e < *cantEnemigos; e++) {
        if (enemigos[e].activo && enemigos[e].posicionFila == nuevaFila && enemigos[e].posicionColumna == nuevaColumna) {
           
            if (j->cooldownInmunidad == 0) {
               if (enemigos[e].tipo == TIPO_ALPHA) {
                    j->vidas = 0; 
                } else {
                    j->vidas--;
                    j->cooldownInmunidad = 3; 
                }
            }
           
            break;     }  }
            
    if (strcmp(mapa[nuevaFila][nuevaColumna], CEREZA) == 0) {
        j->puntos += 50;
    } else if (strcmp(mapa[nuevaFila][nuevaColumna], PASTEL) == 0) {
        j->puntos += 1000;
        *tiempoModoRojo = 100;
        generarFilaAlphaModoRojo(mapa, enemigos, cantEnemigos, filas, columnas);
    } else if (strcmp(mapa[nuevaFila][nuevaColumna], DIAMANTE) == 0) {
        j->puntos += 8000;
        *nivelCompletado = 1;
    }

    mapa[j->posicionFila][j->posicionColumna] = VACIO;
    j->posicionFila = nuevaFila;
    j->posicionColumna = nuevaColumna;
    mapa[j->posicionFila][j->posicionColumna] = j->emoji;
    
    if (!hayCerezas(mapa, filas, columnas)) {
        *nivelCompletado = 1;
        return;
    }
    
    int enemigosVivos = 0;
    for (int e = 0; e < *cantEnemigos; e++) {
        if (enemigos[e].activo) enemigosVivos++;
    }
    if (enemigosVivos == 0) {
        *nivelCompletado = 1;
        return;
    }

    for (int i = filas - 2; i >= 0; i--) {
        for (int k = 0; k < columnas; k++) {
            if (mapa[i][k] == MANZANA) {
                int fCaida = i;
                int distanciaCaida = 0;

                while (fCaida + 1 < filas) {
                    if (mapa[fCaida + 1][k] == VACIO) {
                        mapa[fCaida][k] = VACIO;
                        fCaida++;
                        mapa[fCaida][k] = MANZANA;
                        distanciaCaida++;
                    } else {
                    	
                   if (fCaida + 1 == j->posicionFila && k == j->posicionColumna) {
                       if (distanciaCaida > 0) { 
                                    j->vidas--; 
                                }
                    }
                        
                        int aplastoEnemigo = 0;
                        for (int e = 0; e < *cantEnemigos; e++) {
                            if (enemigos[e].activo && enemigos[e].posicionFila == fCaida + 1 && enemigos[e].posicionColumna == k) {
                                enemigos[e].activo = 0;
                                mapa[fCaida][k] = VACIO;
                                fCaida++;
                                mapa[fCaida][k] = MANZANA;
                                j->puntos += 500;
                               if (enemigos[e].tipo == TIPO_ALPHA) {
                                    j->vidas++;
                                }
                                aplastoEnemigo = 1;
                                break;
                            }
                        }
                        if (!aplastoEnemigo) break; 
                    }
                }

                if (distanciaCaida >= 3) {
                    mapa[fCaida][k] = DIAMANTE;   }
         }
        }
   }
}

void moverEnemigos(char*** mapa, Enemigo* enemigos, int cantEnemigos, jugador* j, int filas, int columnas, int contadorCiclos) {
    for (int i = 0; i < cantEnemigos; i++) {
        if (!enemigos[i].activo) continue;

       
        if (contadorCiclos % enemigos[i].velocidad != 0) continue;

        int ef = enemigos[i].posicionFila;
        int ec = enemigos[i].posicionColumna;

        int nf = ef, nc = ec;

        
        if (rand() % 2 == 0) {
            if (j->posicionFila < ef) nf--;
            else if (j->posicionFila > ef) nf++;
            else if (j->posicionColumna < ec) nc--;
            else if (j->posicionColumna > ec) nc++;
        } else {
            if (j->posicionColumna < ec) nc--;
            else if (j->posicionColumna > ec) nc++;
            else if (j->posicionFila < ef) nf--;
            else if (j->posicionFila > ef) nf++;
        }

        if (nf >= 0 && nf < filas && nc >= 0 && nc < columnas) {
            char* casillaDestino = mapa[nf][nc];
            int movimientoValido = 0;

            
            if (enemigos[i].tipo == TIPO_ROJO) {
                
                if (casillaDestino == VACIO || casillaDestino == TIERRA || casillaDestino == JUGADOR || casillaDestino == CEREZA) {
                    movimientoValido = 1;
                }
            } else if (enemigos[i].tipo == TIPO_AZUL) {
                if (casillaDestino == VACIO || casillaDestino == JUGADOR) {
                    movimientoValido = 1;
                }
               
            } else if (enemigos[i].tipo == TIPO_ALPHA) {
            
                if (casillaDestino == VACIO || casillaDestino == MANZANA || casillaDestino == JUGADOR) {
                    movimientoValido = 1;
                }
            }

            if (movimientoValido) {
                mapa[ef][ec] = VACIO; 
                if (nf == j->posicionFila && nc == j->posicionColumna) {
                	if (j->cooldownInmunidad == 0) {
                	if (enemigos[i].tipo == TIPO_ALPHA) {
                        j->vidas = 0;
                    } else {
                        j->vidas--;
						j->cooldownInmunidad = 3; } }
						}
					else {
                    enemigos[i].posicionFila = nf;
                    enemigos[i].posicionColumna = nc;

                    char* emojiMostrar = ENEMIGO_RAPIDO;
                    if (enemigos[i].tipo == TIPO_ROJO) emojiMostrar = ENEMIGO_LENTO;
                    if (enemigos[i].tipo == TIPO_ALPHA) emojiMostrar = ALPHAMONSTER;

                    mapa[nf][nc] = emojiMostrar;  
                }
            }
       }
    }
}

void moverPelota(char*** mapa, Pelota* p, jugador* j, Enemigo* enemigos, int cantEnemigos, int filas, int columnas) {
    if (!p->activa) return;

    
    if (mapa[p->posicionFila][p->posicionColumna] == PELOTA) {
        mapa[p->posicionFila][p->posicionColumna] = VACIO;
    }

   int nf = p->posicionFila + p->dirFila;
    int nc = p->posicionColumna + p->dirColumna;

  for (int e = 0; e < cantEnemigos; e++) {
        if (enemigos[e].activo && enemigos[e].posicionFila == nf && enemigos[e].posicionColumna == nc) {
            enemigos[e].activo = 0;
            j->puntos += 500;
            if (enemigos[e].tipo == TIPO_ALPHA) {
                j->vidas++;    }
            p->activa = 0; 
            if (mapa[nf][nc] != JUGADOR) {
                mapa[nf][nc] = VACIO;
            }
            return;
        }
    }
    
    if (nf == j->posicionFila && nc == j->posicionColumna) {
        p->activa = 0; 
        return;
    }
  
    if (nf < 0 || nf >= filas || nc < 0 || nc >= columnas || mapa[nf][nc] != VACIO) {
        
        p->dirFila = -p->dirFila;
        p->dirColumna = -p->dirColumna;
        
        nf = p->posicionFila + p->dirFila;
        nc = p->posicionColumna + p->dirColumna;

        if (nf < 0 || nf >= filas || nc < 0 || nc >= columnas || mapa[nf][nc] != VACIO) {
            p->activa = 0;
            return;
        }
    }
    
    p->posicionFila = nf;
    p->posicionColumna = nc;

    if (mapa[nf][nc] == VACIO) {
        mapa[nf][nc] = PELOTA;
    }
}

int menuSeleccionarNivel() {
    system("cls");
    printf("===========================================\n");
    printf("         MR. DO! - SELECCION DE NIVEL      \n");
    printf("===========================================\n\n");
    printf(" 1. Nivel 1 (Mapa Peque o  - 11x11)\n");
    printf(" 2. Nivel 2 (Mapa Mediano  - 15x13)\n");
    printf(" 3. Nivel 3 (Mapa Grande   - 19x15)\n\n");
    printf("Selecciona un nivel inicial (1-3): ");

    int opcion = 1;
    scanf("%d", &opcion);
    if (opcion < 1 || opcion > 3) opcion = 1;
    return opcion;
}

int pantallaGameOver(int puntos) {
    system("cls");
    printf("\n===========================================\n");
    printf("                GAME OVER                  \n");
    printf("===========================================\n");
    printf("  Te has quedado sin vidas...\n");
    printf("  Puntuacion final: %d\n", puntos);
    printf("===========================================\n\n");
     printf(" Deseas reintentar? (Presiona 'R' para jugar de nuevo, o 'P' para salir): ");
     
     while (_kbhit()) {
        _getch();
    }
    
    char tecla;
    do {
        tecla = _getch();
    } while (tecla != 'r' && tecla != 'R' && tecla != 'p' && tecla != 'P');

    if (tecla == 'r' || tecla == 'R') {
        return 1;
    }
    return 0;
}


int hayCerezas(char*** mapa, int filas, int columnas) {
    for (int f = 0; f < filas; f++) {
        for (int c = 0; c < columnas; c++) {
            if (strcmp(mapa[f][c], CEREZA) == 0) {
                return 1; 
            }
        }
    }
    return 0; 
}
void pantallaVictoria(int puntos) {
    system("cls");
    printf("\n===========================================\n");
    printf("             VICTORIA TOTAL!             \n");
    printf("===========================================\n");
    printf("   Has completado todos los niveles!\n");
    printf("  Puntuacion final acumulada: %d\n", puntos);
    printf("===========================================\n\n");
    printf("Presiona cualquier tecla para salir...");
    _getch();
} 
