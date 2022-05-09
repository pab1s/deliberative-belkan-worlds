#include "../Comportamientos_Jugador/jugador.hpp"

#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stack>
#include "motorlib/util.h"

struct ordenarNodo {
    bool operator()(const nodo &nodo_izq, const nodo &nodo_der) {
        int f_izq = nodo_izq.coste + nodo_izq.heur;
        int f_der = nodo_der.coste + nodo_der.heur;
        return f_izq > f_der;
    }
};

// Este es el método principal que se piden en la practica.
// Tiene como entrada la información de los sensores y devuelve la acción a
// realizar. Para ver los distintos sensores mirar fichero "comportamiento.hpp"
Action ComportamientoJugador::think(Sensores sensores) {
    Action accion = actIDLE;

    actual.fila = sensores.posF;
    actual.columna = sensores.posC;
    actual.orientacion = sensores.sentido;

    cout << "Fila: " << actual.fila << endl;
    cout << "Col : " << actual.columna << endl;
    cout << "Ori : " << actual.orientacion << endl;

    // Capturo los destinos
    cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
    objetivos.clear();
    for (int i = 0; i < sensores.num_destinos; i++) {
        estado aux;
        aux.fila = sensores.destino[2 * i];
        aux.columna = sensores.destino[2 * i + 1];
        objetivos.push_back(aux);
    }
    if (!hay_plan) {
        hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);
    }

    if (hay_plan and plan.size() > 0) {
        accion = plan.front();
        plan.erase(plan.begin());
    } else {
        cout << "No esta implementado" << endl;
    }

    return accion;
}

// Llama al algoritmo de busqueda que se usara en cada comportamiento del agente
// Level representa el comportamiento en el que fue iniciado el agente.
bool ComportamientoJugador::pathFinding(int level, const estado &origen,
                                        const list<estado> &destino,
                                        list<Action> &plan) {
    objetivo_actual = objetivos.front();
    cout << "fila: " << objetivo_actual.fila << " col:" << objetivo_actual.columna
         << endl;

    switch (level) {
        case 0:
            cout << "Demo\n";
            return pathFinding_Profundidad(origen, objetivo_actual, plan);
            break;

        case 1:
            cout << "Optimo numero de acciones\n";
            return pathFinding_Anchura(origen, objetivo_actual, plan);
            break;
        case 2:
            cout << "Optimo en coste\n";
            return pathFinding_A_Star(origen, objetivo_actual, plan);
            break;
        case 3:
            cout << "Reto 1: Descubrir el mapa\n";
            // Incluir aqui la llamada al algoritmo de busqueda para el Reto 1
            cout << "No implementado aun\n";
            break;
        case 4:
            cout << "Reto 2: Maximizar objetivos\n";
            // Incluir aqui la llamada al algoritmo de busqueda para el Reto 2
            cout << "No implementado aun\n";
            break;
    }
    return false;
}

//---------------------- Implementación de la busqueda en profundidad
//---------------------------

// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla) {
    if (casilla == 'P' or casilla == 'M')
        return true;
    else
        return false;
}

// Comprueba si la casilla que hay delante es un obstaculo. Si es un
// obstaculo devuelve true. Si no es un obstaculo, devuelve false y
// modifica st con la posición de la casilla del avance.
bool ComportamientoJugador::HayObstaculoDelante(estado &st) {
    int fil = st.fila, col = st.columna;

    // calculo cual es la casilla de delante del agente
    switch (st.orientacion) {
        case 0:
            fil--;
            break;
        case 1:
            fil--;
            col++;
            break;
        case 2:
            col++;
            break;
        case 3:
            fil++;
            col++;
            break;
        case 4:
            fil++;
            break;
        case 5:
            fil++;
            col--;
            break;
        case 6:
            col--;
            break;
        case 7:
            fil--;
            col--;
            break;
    }

    // Compruebo que no me salgo fuera del rango del mapa
    if (fil < 0 or fil >= mapaResultado.size()) return true;
    if (col < 0 or col >= mapaResultado[0].size()) return true;

    // Miro si en esa casilla hay un obstaculo infranqueable
    if (!EsObstaculo(mapaResultado[fil][col])) {
        // No hay obstaculo, actualizo el parametro st poniendo la casilla de
        // delante.
        st.fila = fil;
        st.columna = col;
        return false;
    } else {
        return true;
    }
}

struct ComparaEstados {
    bool operator()(const estado &a, const estado &n) const {
        if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
            (a.fila == n.fila and a.columna == n.columna and a.orientacion > n.orientacion) or
            (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini > n.bikini) or
            (a.fila == n.fila and a.columna == n.columna and a.orientacion == n.orientacion and a.bikini == n.bikini and a.zapatillas > n.zapatillas))
            return true;
        else
            return false;
    }
};

// Implementación de la busqueda en profundidad.
// Entran los puntos origen y destino y devuelve la
// secuencia de acciones en plan, una lista de acciones.
bool ComportamientoJugador::pathFinding_Profundidad(const estado &origen,
                                                    const estado &destino,
                                                    list<Action> &plan) {
    // Borro la lista
    cout << "Calculando plan\n";
    plan.clear();
    set<estado, ComparaEstados> Cerrados;  // Lista de Cerrados
    stack<nodo> Abiertos;                  // Lista de Abiertos

    nodo current;
    current.st = origen;
    current.secuencia.empty();

    Abiertos.push(current);

    while (!Abiertos.empty() and (current.st.fila != destino.fila or
                                  current.st.columna != destino.columna)) {
        Abiertos.pop();
        Cerrados.insert(current.st);

        // Generar descendiente de girar a la derecha 90 grados
        nodo hijoTurnR = current;
        hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
        if (Cerrados.find(hijoTurnR.st) == Cerrados.end()) {
            hijoTurnR.secuencia.push_back(actTURN_R);
            Abiertos.push(hijoTurnR);
        }

        // Generar descendiente de girar a la derecha 45 grados
        nodo hijoSEMITurnR = current;
        hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
        if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end()) {
            hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
            Abiertos.push(hijoSEMITurnR);
        }

        // Generar descendiente de girar a la izquierda 90 grados
        nodo hijoTurnL = current;
        hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
        if (Cerrados.find(hijoTurnL.st) == Cerrados.end()) {
            hijoTurnL.secuencia.push_back(actTURN_L);
            Abiertos.push(hijoTurnL);
        }

        // Generar descendiente de girar a la izquierda 45 grados
        nodo hijoSEMITurnL = current;
        hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
        if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end()) {
            hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
            Abiertos.push(hijoSEMITurnL);
        }

        // Generar descendiente de avanzar
        nodo hijoForward = current;
        if (!HayObstaculoDelante(hijoForward.st)) {
            if (Cerrados.find(hijoForward.st) == Cerrados.end()) {
                hijoForward.secuencia.push_back(actFORWARD);
                Abiertos.push(hijoForward);
            }
        }

        // Tomo el siguiente valor de la Abiertos
        if (!Abiertos.empty()) {
            current = Abiertos.top();
        }
    }

    cout << "Terminada la busqueda\n";

    if (current.st.fila == destino.fila and
        current.st.columna == destino.columna) {
        cout << "Cargando el plan\n";
        plan = current.secuencia;
        cout << "Longitud del plan: " << plan.size() << endl;
        PintaPlan(plan);
        // ver el plan en el mapa
        VisualizaPlan(origen, plan);
        return true;
    } else {
        cout << "No encontrado plan\n";
    }

    return false;
}

// Sacar por la consola la secuencia del plan obtenido
void ComportamientoJugador::PintaPlan(list<Action> plan) {
    auto it = plan.begin();
    while (it != plan.end()) {
        if (*it == actFORWARD) {
            cout << "A ";
        } else if (*it == actTURN_R) {
            cout << "D ";
        } else if (*it == actSEMITURN_R) {
            cout << "d ";
        } else if (*it == actTURN_L) {
            cout << "I ";
        } else if (*it == actSEMITURN_L) {
            cout << "I ";
        } else {
            cout << "- ";
        }
        it++;
    }
    cout << endl;
}

// Funcion auxiliar para poner a 0 todas las casillas de una matriz
void AnularMatriz(vector<vector<unsigned char>> &m) {
    for (int i = 0; i < m[0].size(); i++) {
        for (int j = 0; j < m.size(); j++) {
            m[i][j] = 0;
        }
    }
}

// Pinta sobre el mapa del juego el plan obtenido
void ComportamientoJugador::VisualizaPlan(const estado &st,
                                          const list<Action> &plan) {
    AnularMatriz(mapaConPlan);
    estado cst = st;

    auto it = plan.begin();
    while (it != plan.end()) {
        if (*it == actFORWARD) {
            switch (cst.orientacion) {
                case 0:
                    cst.fila--;
                    break;
                case 1:
                    cst.fila--;
                    cst.columna++;
                    break;
                case 2:
                    cst.columna++;
                    break;
                case 3:
                    cst.fila++;
                    cst.columna++;
                    break;
                case 4:
                    cst.fila++;
                    break;
                case 5:
                    cst.fila++;
                    cst.columna--;
                    break;
                case 6:
                    cst.columna--;
                    break;
                case 7:
                    cst.fila--;
                    cst.columna--;
                    break;
            }
            mapaConPlan[cst.fila][cst.columna] = 1;
        } else if (*it == actTURN_R) {
            cst.orientacion = (cst.orientacion + 2) % 8;
        } else if (*it == actSEMITURN_R) {
            cst.orientacion = (cst.orientacion + 1) % 8;
        } else if (*it == actTURN_L) {
            cst.orientacion = (cst.orientacion + 6) % 8;
        } else if (*it == actSEMITURN_L) {
            cst.orientacion = (cst.orientacion + 7) % 8;
        }
        it++;
    }
}

int ComportamientoJugador::interact(Action accion, int valor) { return false; }

bool ComportamientoJugador::pathFinding_Anchura(const estado &origen,
                                                const estado &destino,
                                                list<Action> &plan) {
    // Borro la lista
    cout << "Calculando plan\n";
    plan.clear();
    set<estado, ComparaEstados> Cerrados;  // Lista de Cerrados
    queue<nodo> Abiertos;                  // Lista de Abiertos

    nodo current;
    current.st = origen;
    current.secuencia.empty();

    Abiertos.push(current);

    while (!Abiertos.empty() and (current.st.fila != destino.fila or
                                  current.st.columna != destino.columna)) {
        Abiertos.pop();
        Cerrados.insert(current.st);

        // Generar descendiente de girar a la derecha 90 grados
        nodo hijoTurnR = current;
        hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
        if (Cerrados.find(hijoTurnR.st) == Cerrados.end()) {
            hijoTurnR.secuencia.push_back(actTURN_R);
            Abiertos.push(hijoTurnR);
        }

        // Generar descendiente de girar a la derecha 45 grados
        nodo hijoSEMITurnR = current;
        hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
        if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end()) {
            hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
            Abiertos.push(hijoSEMITurnR);
        }

        // Generar descendiente de girar a la izquierda 90 grados
        nodo hijoTurnL = current;
        hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
        if (Cerrados.find(hijoTurnL.st) == Cerrados.end()) {
            hijoTurnL.secuencia.push_back(actTURN_L);
            Abiertos.push(hijoTurnL);
        }

        // Generar descendiente de girar a la izquierda 45 grados
        nodo hijoSEMITurnL = current;
        hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
        if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end()) {
            hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
            Abiertos.push(hijoSEMITurnL);
        }

        // Generar descendiente de avanzar
        nodo hijoForward = current;
        if (!HayObstaculoDelante(hijoForward.st)) {
            if (Cerrados.find(hijoForward.st) == Cerrados.end()) {
                hijoForward.secuencia.push_back(actFORWARD);
                Abiertos.push(hijoForward);
            }
        }

        if (hijoForward.st.fila == destino.fila and hijoForward.st.columna == destino.columna) {
            Cerrados.insert(hijoForward.st);
            current = hijoForward;
        }

        // Tomo el siguiente valor de la Abiertos si no ha encontrado la solución
        if (!Abiertos.empty() and !(current.st.fila == destino.fila and current.st.columna == destino.columna)) {
            current = Abiertos.front();
        }
    }

    cout << "Terminada la busqueda\n";

    if (current.st.fila == destino.fila and
        current.st.columna == destino.columna) {
        cout << "Cargando el plan\n";
        plan = current.secuencia;
        cout << "Longitud del plan: " << plan.size() << endl;
        PintaPlan(plan);
        // ver el plan en el mapa
        VisualizaPlan(origen, plan);
        return true;
    } else {
        cout << "No encontrado plan\n";
    }

    return false;
}

bool ComportamientoJugador::pathFinding_A_Star(const estado &origen,
                                               const estado &destino,
                                               list<Action> &plan) {
    // Borro la lista
    cout << "Calculando plan\n";
    plan.clear();
    set<estado, ComparaEstados> Cerrados;  // Lista de Cerrados
    priority_queue<nodo, vector<nodo>, ordenarNodo> Abiertos;  // Lista de Abiertos
    nodo current;
    current.st = origen;
    current.secuencia.empty();
    current.coste = 0;
    current.heur = h(current);
    actualizarObjetos(current);
    Abiertos.push(current);

    while (!Abiertos.empty() and (current.st.fila != destino.fila or
                                  current.st.columna != destino.columna)) {
        Abiertos.pop();
        if (Cerrados.find(current.st) == Cerrados.end()) {
            Cerrados.insert(current.st);

            // Generar descendiente de girar a la derecha 90 grados
            nodo hijoTurnR = current;
            hijoTurnR.secuencia.push_back(actTURN_R);
            hijoTurnR.coste += g(hijoTurnR);
            hijoTurnR.st.orientacion = (hijoTurnR.st.orientacion + 2) % 8;
            hijoTurnR.heur = h(hijoTurnR);
            if (Cerrados.find(hijoTurnR.st) == Cerrados.end()) {
                Abiertos.push(hijoTurnR);
            }

            // Generar descendiente de girar a la derecha 45 grados
            nodo hijoSEMITurnR = current;
            hijoSEMITurnR.secuencia.push_back(actSEMITURN_R);
            hijoSEMITurnR.coste += g(hijoSEMITurnR);
            hijoSEMITurnR.st.orientacion = (hijoSEMITurnR.st.orientacion + 1) % 8;
            hijoSEMITurnR.heur = h(hijoSEMITurnR);
            if (Cerrados.find(hijoSEMITurnR.st) == Cerrados.end()) {
                Abiertos.push(hijoSEMITurnR);
            }

            // Generar descendiente de girar a la izquierda 90 grados
            nodo hijoTurnL = current;
            hijoTurnL.secuencia.push_back(actTURN_L);
            hijoTurnL.coste += g(hijoTurnL);
            hijoTurnL.st.orientacion = (hijoTurnL.st.orientacion + 6) % 8;
            hijoTurnL.heur = h(hijoTurnL);
            if (Cerrados.find(hijoTurnL.st) == Cerrados.end()) {
                Abiertos.push(hijoTurnL);
            }

            // Generar descendiente de girar a la izquierda 45 grados
            nodo hijoSEMITurnL = current;
            hijoSEMITurnL.secuencia.push_back(actSEMITURN_L);
            hijoSEMITurnL.coste += g(hijoSEMITurnL);
            hijoSEMITurnL.st.orientacion = (hijoSEMITurnL.st.orientacion + 7) % 8;
            hijoSEMITurnL.heur = h(hijoSEMITurnL);
            if (Cerrados.find(hijoSEMITurnL.st) == Cerrados.end()) {
                Abiertos.push(hijoSEMITurnL);
            }

            // Generar descendiente de avanzar
            nodo hijoForward = current;
            hijoForward.secuencia.push_back(actFORWARD);
            hijoForward.coste += g(hijoForward);
            if (!HayObstaculoDelante(hijoForward.st)) {
                hijoForward.heur = h(hijoForward);
                actualizarObjetos(hijoForward);
                if (Cerrados.find(hijoForward.st) == Cerrados.end()) {
                    Abiertos.push(hijoForward);
                }
            }
        }
        // Tomo el siguiente valor de la Abiertos
        if (!Abiertos.empty()) {
            current = Abiertos.top();
        }
    }

    cout << "Terminada la busqueda\n";

    if (current.st.fila == destino.fila and
        current.st.columna == destino.columna) {
        cout << "Cargando el plan\n";
        plan = current.secuencia;
        cout << "Longitud del plan: " << plan.size() << endl;
        PintaPlan(plan);
        // ver el plan en el mapa
        VisualizaPlan(origen, plan);
        return true;
    } else {
        cout << "No encontrado plan\n";
    }

    return false;
}

int ComportamientoJugador::g(const nodo &n) {
    return costeAccion(mapaResultado.at(n.st.fila).at(n.st.columna), n.secuencia.back(), n.st.bikini, n.st.zapatillas);
}

int ComportamientoJugador::h(const nodo &n) {
    int x = abs(n.st.columna - objetivo_actual.columna);
    int y = abs(n.st.fila - objetivo_actual.fila);
    return max(x, y);
}

int ComportamientoJugador::costeAccion(unsigned char celda, Action accion, bool bikini, bool zapatillas) {
    int bateria = 1;
    switch (accion) {
        case actIDLE:
            bateria = 0;
            break;
        case actFORWARD:
            switch (celda) {
                case 'A':
                    if (bikini)  // Bikini
                        bateria = 10;
                    else
                        bateria = 200;
                    break;
                case 'B':
                    if (zapatillas)  // Zapatillas
                        bateria = 15;
                    else
                        bateria = 100;
                    break;
                case 'T':
                    bateria = 2;
                    break;
                default:
                    bateria = 1;
                    break;
            }  // Fin switch celda
            break;
        case actTURN_L:
        case actTURN_R:
            switch (celda) {
                case 'A':
                    if (bikini)  // Bikini
                        bateria = 5;
                    else
                        bateria = 500;
                    break;
                case 'B':
                    if (zapatillas)  // Zapatillas
                        bateria = 1;
                    else
                        bateria = 3;
                    break;
                case 'T':
                    bateria = 2;
                    break;
                default:
                    bateria = 1;
                    break;
            }  // Fin switch celda
            break;
        case actSEMITURN_L:
        case actSEMITURN_R:
            switch (celda) {
                case 'A':
                    if (bikini)  // Bikini
                        bateria = 2;
                    else
                        bateria = 300;
                    break;
                case 'B':
                    if (zapatillas)  // Zapatillas
                        bateria = 1;
                    else
                        bateria = 2;
                    break;
                default:
                    bateria = 1;
                    break;
            }  // Fin switch celda
            break;
        case actWHEREIS:
            bateria = 200;
    }
    return bateria;
}

bool operator ==(const estado &e1,const estado &e2) {
    return e1.fila == e2.fila and e1.columna == e2.columna and e1.orientacion == e2.orientacion and e1.bikini == e2.bikini and e1.zapatillas == e2.zapatillas;
}

void ComportamientoJugador::actualizarObjetos(nodo& n) {
    if (mapaResultado[n.st.fila][n.st.columna] == 'K') {
        n.st.bikini = true;
        n.st.zapatillas = false;
    } else if (mapaResultado[n.st.fila][n.st.columna] == 'D') {
        n.st.bikini = false;
        n.st.zapatillas = true;
    }
}