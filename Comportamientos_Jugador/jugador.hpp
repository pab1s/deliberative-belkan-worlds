#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include <list>

#include "comportamientos/comportamiento.hpp"

struct estado {
    int fila;
    int columna;
    int orientacion;
    bool bikini;
    bool zapatillas;
};

struct nodo {
    estado st;
    list<Action> secuencia;
    int heur, coste;
};

bool operator ==(const estado &e1,const estado &e2);



class ComportamientoJugador : public Comportamiento {
   public:
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        // Inicializar Variables de Estado
        hay_plan = false;
    }
    ComportamientoJugador(std::vector<std::vector<unsigned char> > mapaR)
        : Comportamiento(mapaR) {
        // Inicializar Variables de Estado
    }
    ComportamientoJugador(const ComportamientoJugador &comport)
        : Comportamiento(comport) {}
    ~ComportamientoJugador() {}

    Action think(Sensores sensores);
    int interact(Action accion, int valor);
    void VisualizaPlan(const estado &st, const list<Action> &plan);
    ComportamientoJugador *clone() { return new ComportamientoJugador(*this); }

   private:
    // Declarar Variables de Estado
    const int MAX_BATERIA = 1000000;
    estado actual;
    estado objetivo_actual;
    list<estado> objetivos;
    list<Action> plan;
    bool hay_plan;

    // Métodos privados de la clase
    int g(const nodo &n);
    int h(const nodo &n);
    bool pathFinding(int level, const estado &origen, const list<estado> &destino, list<Action> &plan);
    bool pathFinding_Profundidad(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_Anchura(const estado &origen, const estado &destino, list<Action> &plan);
    bool pathFinding_A_Star(const estado &origen, const estado &destino, list<Action> &plan);
    void PintaPlan(list<Action> plan);
    bool HayObstaculoDelante(estado &st);
    int costeAccion(unsigned char celda, Action accion, bool bikini, bool zapatillas);
    void actualizarObjetos(nodo &n);
};
#endif
