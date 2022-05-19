#include "../Comportamientos_Jugador/jugador.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <queue>
#include <set>
#include <stack>

#include "motorlib/util.h"

using namespace std;

struct ordenarNodo {
    bool operator()(const nodo &nodo_izq, const nodo &nodo_der) {
        int f_izq = nodo_izq.coste + nodo_izq.heur;
        int f_der = nodo_der.coste + nodo_der.heur;
        return f_izq > f_der;
    }
};

struct ComparaEstados {
    bool operator()(const estado &a, const estado &n) const {
        if ((a.fila > n.fila) or (a.fila == n.fila and a.columna > n.columna) or
            (a.fila == n.fila and a.columna == n.columna and
             a.orientacion > n.orientacion) or
            (a.fila == n.fila and a.columna == n.columna and
             a.orientacion == n.orientacion and a.bikini > n.bikini) or
            (a.fila == n.fila and a.columna == n.columna and
             a.orientacion == n.orientacion and a.bikini == n.bikini and
             a.zapatillas > n.zapatillas))
            return true;
        else
            return false;
    }
};

int distanciaChebyshev(const estado &origen, const estado &destino) {
    int distancia = 0;
    int distancia_x = abs(origen.fila - destino.fila);
    int distancia_y = abs(origen.columna - destino.columna);
    if (distancia_x > distancia_y) {
        distancia = distancia_x;
    } else {
        distancia = distancia_y;
    }
    return distancia;
}

Action ComportamientoJugador::think(Sensores sensores) {
    Action accion = actIDLE;
    nivel = sensores.nivel;

    if (sensores.nivel != 4) {
        bien_situado = true;
        actual.fila = sensores.posF;
        actual.columna = sensores.posC;
        actual.orientacion = sensores.sentido;
    }

    // Capturo los destinos
    cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
    if (sensores.nivel != 3) {
        objetivos.clear();
        for (int i = 0; i < sensores.num_destinos; i++) {
            estado aux;
            aux.fila = sensores.destino[2 * i];
            aux.columna = sensores.destino[2 * i + 1];
            objetivos.push_back(aux);
        }
    }

    if (sensores.nivel == 4 and primerInstante) {
        ultimaAccion = actWHEREIS;
        primerInstante = false;
        bien_situado = true;
        return ultimaAccion;
    } else if (sensores.nivel == 3) {
        actual.fila = sensores.posF;
        actual.columna = sensores.posC;
        actual.orientacion = sensores.sentido;

        if (((float)sensores.bateria) / (float)sensores.vida < 1 and
            sensores.terreno[0] == 'X') {
            ultimaAccion = actIDLE;
            voyARecargar = false;
            return actIDLE;
        }
        if (((float)sensores.bateria) / (float)sensores.vida < 0.65 and
            sensores.vida > 900) {
            elPlan = recargar;
            elegido = true;
        }
        if (!evaluarTerreno(sensores.terreno[2]) and
            plan.front() == actFORWARD) {
            elPlan = continuar;
            elegido = true;
        }
        if ((!voyARecargar) and
            (!hay_plan or
             mapaResultado[objetivo_actual.fila][objetivo_actual.columna] !=
                 '?')) {
            elPlan = descubrir;
            elegido = true;
        }
        if (!elegido) {
            elPlan = ninguno;
        }

        switch (elPlan) {
            estado sigObjetivo;
            case continuar:
                hay_plan = false;
                objetivos.clear();
                objetivos.push_back(objetivo_actual);
                break;
            case descubrir:
                hay_plan = false;
                objetivos.clear();
                if (calcularPerDesc() < 0.75 or
                    !irAMasCercano(actual, sigObjetivo)) {
                    sigObjetivo = calcularDestino(actual);
                }
                objetivos.push_back(sigObjetivo);
                break;
            case recargar:
                if (irABateria(actual, sigObjetivo) and !voyARecargar) {
                    voyARecargar = true;
                    hay_plan = false;
                    objetivos.clear();
                    objetivos.push_back(sigObjetivo);
                }
                break;
            default:
                break;
        }
    } else if (sensores.nivel == 4 and !primerInstante) {
        if (sensores.colision) {
            if (!comprobarEmpujon(actual, sensores.terreno)) {
                bien_situado = false;
                elegido = false;
                hay_plan = false;
                ultimaAccion = actWHEREIS;
                return actWHEREIS;
            }
        }

        switch (ultimaAccion) {
            case actFORWARD:
                if (!sensores.colision) {
                    switch (actual.orientacion) {
                        case 0:  // Norte
                            actual.fila--;
                            break;
                        case 1:  // Noreste
                            actual.fila--;
                            actual.columna++;
                            break;
                        case 2:  // Este
                            actual.columna++;
                            break;
                        case 3:  // Sureste
                            actual.fila++;
                            actual.columna++;
                            break;
                        case 4:  // Sur
                            actual.fila++;
                            break;
                        case 5:  // Suroeste
                            actual.fila++;
                            actual.columna--;
                            break;
                        case 6:  // Oeste
                            actual.columna--;
                            break;
                        case 7:  // Noroeste
                            actual.fila--;
                            actual.columna--;
                            break;
                    }
                }
                break;
            case actTURN_L:
                actual.orientacion = (actual.orientacion + 6) % 8;
                break;
            case actSEMITURN_L:
                actual.orientacion = (actual.orientacion + 7) % 8;
                break;
            case actTURN_R:
                actual.orientacion = (actual.orientacion + 2) % 8;
                break;
            case actSEMITURN_R:
                actual.orientacion = (actual.orientacion + 1) % 8;
                break;
            case actWHEREIS:
                bien_situado = true;
                actual.fila = sensores.posF;
                actual.columna = sensores.posC;
                actual.orientacion = sensores.sentido;
                break;
        }

        if (((float)sensores.bateria) / (float)sensores.vida < 1 and
            sensores.terreno[0] == 'X') {
            ultimaAccion = actIDLE;
            voyARecargar = false;
            return actIDLE;
        }
        if (((float)sensores.bateria) / (float)sensores.vida < 0.65 and
            sensores.vida > 900) {
            elPlan = recargar;
            elegido = true;
        }
        if ((!evaluarTerreno(sensores.terreno[2]) or sensores.superficie[2] != '_') and
            plan.front() == actFORWARD) {
            elPlan = continuar;
            elegido = true;
        }
        if ((!voyARecargar) and
            (!hay_plan or (objetivo_actual.fila == actual.fila and
                           objetivo_actual.columna == actual.columna))) {
            elPlan = descubrir;
            elegido = true;
            if (objetivo_actual.fila == actual.fila and
                objetivo_actual.columna == actual.columna) {
                for (auto objetivo : objetivos) {
                    if (objetivo.fila == objetivo_actual.fila and
                        objetivo.columna == objetivo_actual.columna) {
                        contadorObjs = (contadorObjs + 1) % 3;
                    }
                }
            }
        }
        if (!elegido) {
            elPlan = ninguno;
        }

        switch (elPlan) {
            estado sigObjetivo;
            case continuar:
                hay_plan = false;
                objetivos.clear();
                objetivos.push_back(objetivo_actual);
                break;
            case descubrir:
                hay_plan = false;
                for (int i = 0; i < contadorObjs; ++i) {
                    objetivos.pop_front();
                }
                sigObjetivo = objetivos.front();
                objetivos.push_back(objetivo_actual);
                break;
            case recargar:
                if (irABateria(actual, sigObjetivo) and !voyARecargar) {
                    voyARecargar = true;
                    hay_plan = false;
                    objetivos.clear();
                    objetivos.push_back(sigObjetivo);
                }
                break;
            default:
                break;
        }
    }

    cout << "Fila: " << actual.fila << endl;
    cout << "Columna : " << actual.columna << endl;
    cout << "Origen : " << actual.orientacion << endl;

    if (bien_situado or sensores.nivel != 4) {
        mapearVision(mapaResultado, sensores.terreno, actual.orientacion,
                     actual.fila, actual.columna);
        mapearSuperficie(mapaSuperficie, sensores.superficie,
                         actual.orientacion, actual.fila, actual.columna);
        actualizarObjetos(actual);
    }

    if (!hay_plan and bien_situado) {
        hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);
    }

    if (hay_plan and plan.size() > 0) {
        accion = plan.front();
        plan.erase(plan.begin());
    } else {
        cout << "No esta implementado" << endl;
    }
    cout << "Objetivo: " << objetivos.front().fila << " "
         << objetivos.front().columna << endl;
    elegido = false;
    ultimaAccion = accion;
    cout << "Accion : " << accion << endl;
    return accion;
}

bool ComportamientoJugador::pathFinding(int level, const estado &origen,
                                        const list<estado> &destino,
                                        list<Action> &plan) {
    objetivo_actual = objetivos.front();
    cout << "Objetivo actual fila: " << objetivo_actual.fila
         << " columna:" << objetivo_actual.columna << endl;

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
            return pathFinding_A_Star(origen, objetivo_actual, plan);
            break;
        case 4:
            cout << "Reto 2: Maximizar objetivos\n";
            return pathFinding_A_Star(origen, objetivo_actual, plan);
            break;
    }
    return false;
}

//---------------------- Implementación de la busqueda en profundidad
//---------------------------

int ComportamientoJugador::interact(Action accion, int valor) { return false; }
// Dado el codigo en caracter de una casilla del mapa dice si se puede
// pasar por ella sin riegos de morir o chocar.
bool EsObstaculo(unsigned char casilla) {
    if (casilla == 'P' or casilla == 'M')
        return true;
    else
        return false;
}

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

        if (hijoForward.st.fila == destino.fila and
            hijoForward.st.columna == destino.columna) {
            Cerrados.insert(hijoForward.st);
            current = hijoForward;
        }

        // Tomo el siguiente valor de la Abiertos si no ha encontrado la
        // solución
        if (!Abiertos.empty() and !(current.st.fila == destino.fila and
                                    current.st.columna == destino.columna)) {
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

int ComportamientoJugador::g(const nodo &n) {
    return costeAccion(mapaResultado.at(n.st.fila).at(n.st.columna),
                       n.secuencia.back(), n.st.bikini, n.st.zapatillas) +
           costeAccionSuperficie(n.secuencia.back(),
                                 mapaSuperficie.at(n.st.fila).at(n.st.columna));
}

int ComportamientoJugador::h(const nodo &n) {
    int x = abs(n.st.columna - objetivo_actual.columna);
    int y = abs(n.st.fila - objetivo_actual.fila);
    return max(x, y);  // Chebyshev distance
    // return (int)(0.414*min(x, y) + max(x, y)); // Octile Heuristic
}

bool ComportamientoJugador::pathFinding_A_Star(const estado &origen,
                                               const estado &destino,
                                               list<Action> &plan) {
    // Borro la lista
    cout << "Calculando plan\n";
    plan.clear();
    set<estado, ComparaEstados> Cerrados;  // Lista de Cerrados
    priority_queue<nodo, vector<nodo>, ordenarNodo>
        Abiertos;  // Lista de Abiertos
    nodo current;
    current.st = origen;
    current.secuencia.empty();
    current.coste = 0;
    current.heur = h(current);
    actualizarObjetos(current.st);
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
            hijoSEMITurnR.st.orientacion =
                (hijoSEMITurnR.st.orientacion + 1) % 8;
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
            hijoSEMITurnL.st.orientacion =
                (hijoSEMITurnL.st.orientacion + 7) % 8;
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
                actualizarObjetos(hijoForward.st);
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
        cout << "Coste total: " << current.coste << endl;
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

int ComportamientoJugador::costeAccion(unsigned char celda, Action accion,
                                       bool bikini, bool zapatillas) {
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
                case '?':
                    bateria = 0;
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
                case '?':
                    bateria = 1;
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
                case '?':
                    bateria = 1;
                    break;
                default:
                    bateria = 1;
                    break;
            }  // Fin switch celda
            break;
        case actWHEREIS:
            bateria = 200;
    }

    if (nivel == 3 or nivel == 4) {
        ++bateria;
    }

    return bateria;
}

int ComportamientoJugador::costeAccionSuperficie(Action accion, char casilla) {
    int bateria = 0;
    if (accion == actFORWARD) {
        switch (casilla) {
            case 'l':
                bateria = 500;
                break;
            case 'a':
                bateria = 500;
                break;
            case '_':
                bateria = 0;
                break;
        }
    }
    return bateria;
}

void ComportamientoJugador::actualizarObjetos(estado &e) {
    if (mapaResultado[e.fila][e.columna] == 'K') {
        e.bikini = true;
        e.zapatillas = false;
    } else if (mapaResultado[e.fila][e.columna] == 'D') {
        e.bikini = false;
        e.zapatillas = true;
    }
}

estado ComportamientoJugador::calcularDestino(const estado &origen) {
    estado destino;
    int nSectores = 4;
    int maxIndex = 0;
    int x_rel = 0, y_rel = 0, x_aux = 0, y_aux = 0;
    int tam = mapaResultado.size() / 2;
    vector<int> nCasillasVacias(nSectores, 0);
    while (tam > 1) {
        for (int i = 0; i < nSectores; i++) {
            for (int j = 0; j < tam; j++) {
                for (int k = 0; k < tam; k++) {
                    x_aux = i % 2 == 0 ? x_rel + j : x_rel + tam + j;
                    y_aux = i < nSectores / 2 ? y_rel + k : y_rel + tam + k;
                    if (mapaResultado[x_aux][y_aux] == '?') {
                        nCasillasVacias[i]++;
                    }
                }
            }
        }
        maxIndex = max_element(nCasillasVacias.begin(), nCasillasVacias.end()) -
                   nCasillasVacias.begin();
        x_rel = maxIndex % 2 == 0 ? x_rel : x_rel + tam;
        y_rel = maxIndex < nSectores / 2 ? y_rel : y_rel + tam;
        tam = tam / 2;
        fill(nCasillasVacias.begin(), nCasillasVacias.end(), 0);
    }

    destino.fila = x_rel;
    destino.columna = y_rel;
    destino.orientacion = origen.orientacion;

    return destino;
}

bool ComportamientoJugador::irABateria(const estado &origen, estado &destino) {
    destino.fila = mapaResultado.size();
    destino.columna = mapaResultado.size();
    bool encontrado = false;
    for (int i = 0; i < mapaResultado.size(); i++) {
        for (int j = 0; j < mapaResultado.size(); j++) {
            if (mapaResultado[i][j] == 'X') {
                estado aux;
                aux.fila = i;
                aux.columna = j;
                if (!encontrado or distanciaChebyshev(origen, aux) <
                                       distanciaChebyshev(origen, destino)) {
                    destino = aux;
                    encontrado = true;
                }
            }
        }
    }
    return encontrado;
}

bool ComportamientoJugador::irAMasCercano(const estado &origen,
                                          estado &destino) {
    destino.fila = mapaResultado.size();
    destino.columna = mapaResultado.size();
    bool encontrado = false;
    for (int i = 0; i < mapaResultado.size(); i++) {
        for (int j = 0; j < mapaResultado.size(); j++) {
            if (mapaResultado[i][j] == '?') {
                estado aux;
                aux.fila = i;
                aux.columna = j;
                if ((!encontrado or distanciaChebyshev(origen, aux) <
                                        distanciaChebyshev(origen, destino)) and
                    i >= 3 and j >= 3 and i < mapaResultado.size() - 3 and
                    j < mapaResultado.size() - 3) {
                    destino = aux;
                    encontrado = true;
                }
            }
        }
    }
    return encontrado;
}

void ComportamientoJugador::mapearVision(vector<vector<unsigned char>> &mapa,
                                         vector<unsigned char> vision,
                                         int orientacion, int f, int c) {
    int n = 0, aux = 0, x = 0, y = 0;
    for (int i = 0; i >= -VISION_DEPTH; i--) {
        for (int j = i; j <= -i; j++) {
            if (orientacion % 2 == 0) {
                x = i;
                y = j;
            } else {
                if (j <= 0) {
                    x = i;
                    y = j - i;
                } else {
                    x = i + j;
                    y = -i;
                }
            }
            for (int k = 0; k < orientacion / 2; k++) {
                aux = (-1) * x;
                x = y;
                y = aux;
            }
            mapa[f + x][c + y] = vision[n];
            n++;
        }
    }
}

void ComportamientoJugador::mapearSuperficie(
    vector<vector<unsigned char>> &mapa, vector<unsigned char> superficie,
    int orientacion, int f, int c) {
    int n = 0, aux = 0, x = 0, y = 0;
    for (int i = 0; i < mapaSuperficie.size(); ++i) {
        fill(mapaSuperficie[i].begin(), mapaSuperficie[i].end(), '?');
    }

    for (int i = 0; i >= -VISION_DEPTH; i--) {
        for (int j = i; j <= -i; j++) {
            if (orientacion % 2 == 0) {
                x = i;
                y = j;
            } else {
                if (j <= 0) {
                    x = i;
                    y = j - i;
                } else {
                    x = i + j;
                    y = -i;
                }
            }
            for (int k = 0; k < orientacion / 2; k++) {
                aux = (-1) * x;
                x = y;
                y = aux;
            }
            mapa[f + x][c + y] = superficie[n];
            n++;
        }
    }
}

bool ComportamientoJugador::evaluarTerreno(char casilla) {
    if (casilla == 'T' or casilla == 'S' or casilla == 'G' or casilla == 'X' or
        casilla == 'D' or casilla == 'K' /*or (casilla == 'B' and actual.zapatillas) or
        (casilla == 'A' and actual.bikini)*/) {
        return true;
    } else {
        return false;
    }
}

float ComportamientoJugador::calcularPerDesc() {
    float porcentaje = 0, contador = 0;

    for (int i = 0; i < mapaResultado.size(); i++) {
        for (int j = 0; j < mapaResultado.size(); j++) {
            if (mapaResultado[i][j] != '?') {
                contador++;
            }
        }
    }
    porcentaje = contador / (mapaResultado.size() * mapaResultado.size());
    return porcentaje;
}

bool ComportamientoJugador::comprobarEmpujon(estado &st,
                                             vector<unsigned char> vision) {
    estado posiblesPos[8];
    int aux = 0, n = 0, numOcurr = 0, orientacion = 0;
    int x = -1, y = 0;
    bool comprobacion = true;

    for (int i = 0; i < 8; i++) {
        x = -1;
        y = i % 2 == 0 ? 0 : 1;

        for (int k = 0; k < i / 2; k++) {
            aux = (-1) * x;
            x = y;
            y = aux;
        }

        posiblesPos[i].fila = st.fila + x;
        posiblesPos[i].columna = st.columna + y;
    }

    for (int l = 0; l < 8; l++) {
        comprobacion = true;
        n = 0;
        for (int i = 0; i >= -VISION_DEPTH; i--) {
            for (int j = i; j <= -i; j++) {
                if (st.orientacion % 2 == 0) {
                    x = i;
                    y = j;
                } else {
                    if (j <= 0) {
                        x = i;
                        y = j - i;
                    } else {
                        x = i + j;
                        y = -i;
                    }
                }
                for (int k = 0; k < st.orientacion / 2; k++) {
                    aux = (-1) * x;
                    x = y;
                    y = aux;
                }
                if (!(mapaResultado[st.fila + x][st.columna + y] == vision[n] or
                    mapaResultado[st.fila + x][st.columna + y] == '?'))
                    comprobacion = false;
                n++;
            }
            if (comprobacion) {
                ++numOcurr;
                orientacion = l;
            }
        }
    }

    if (numOcurr == 1) {
        st = posiblesPos[orientacion];
        return true;
    }
    return false;
}
