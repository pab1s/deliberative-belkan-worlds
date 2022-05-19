/**
 * @file jugador.hpp
 * @author Pablo Olivares Martínez
 * @brief Cabecera de la clase ComportamientoJugador. Ésta establece el
 * comportamiento de un agente deliberativo en el mapa de los "Mundos de Belkan".
 * @version 0.1
 * @date 2022-05-16
 */
#ifndef COMPORTAMIENTOJUGADOR_H
#define COMPORTAMIENTOJUGADOR_H

#include <list>
#include <set>

#include "comportamientos/comportamiento.hpp"

/// @brief Enumerado con los distintos estados del agente para los niveles 3 y 4.
enum Plan { recargar, continuar, descubrir, ninguno };

/**
 * @brief Struct con la información de una casilla del mapa en un momento dado.
 */
struct estado {
    int fila;
    int columna;
    int orientacion;
    bool bikini;
    bool zapatillas;
};

/**
 * @brief Struct con la información de una casilla del mapa
 * durante la construcción de un camino a un objetivo.
 */
struct nodo {
    estado st;
    list<Action> secuencia;
    int heur, coste;
};

/**
 * @class ComportamientoJugador
 * @brief Implementación del comportamiento deliberativo del agente.
 */
class ComportamientoJugador : public Comportamiento {
   public:
    /**
     * @brief Constructor de la clase ComportamientoJugador.
     * @param size Se trata del tamaño del mapa.
     */
    ComportamientoJugador(unsigned int size) : Comportamiento(size) {
        // Inicializar Variables de Estado
        ultimaAccion = actIDLE;
        hay_plan = false;
        actual.fila = 0;
        actual.columna = 0;
        actual.orientacion = 0;
        actual.bikini = false;
        actual.zapatillas = false;
        voyARecargar = false;
        elegido = false;
        recargando = false;
        bien_situado = false;
        primerInstante = true;
        elPlan = descubrir;
        nivel = 0;
        vector<unsigned char> vacio(mapaResultado.size(), '?');

        for (unsigned int i = 0; i < mapaResultado.size(); i++) {
            mapaSuperficie.push_back(vacio);
        }
    }

    /**
     * @brief Constructor de la clase ComportamientoJugador.
     * @param mapaR Mapa que se va a copiar.
     */
    ComportamientoJugador(std::vector<std::vector<unsigned char>> mapaR)
        : Comportamiento(mapaR) {
        ultimaAccion = actIDLE;
        hay_plan = false;
        actual.fila = 0;
        actual.columna = 0;
        actual.orientacion = 0;
        actual.bikini = false;
        actual.zapatillas = false;
        voyARecargar = false;
        elegido = false;
        recargando = false;
        bien_situado = false;
        primerInstante = true;
        elPlan = descubrir;
        nivel = 0;
        vector<unsigned char> vacio(mapaResultado.size(), '?');

        for (unsigned int i = 0; i < mapaResultado.size(); i++) {
            mapaSuperficie.push_back(vacio);
        }
    }

    /**
     * @brief Constructor de copia de la clase ComportamientoJugador.
     * @param comport Comportamiento que se va a copiar.
     */
    ComportamientoJugador(const ComportamientoJugador &comport)
        : Comportamiento(comport) {}

    /**
     * @brief Destructor por defecto de ComportamientoJugador.
     */
    ~ComportamientoJugador() {}

    /**
     * @brief think
     * Método que se ejecuta cada vez que se llama al agente. Se encarga de
     * actualizar el mapa, el estado del agente y de decidir la acción a
     * realizar.
     * @param sensores Valores de los sensores del agente.
     * @return Action Acción a realizar por el agente.
     */
    Action think(Sensores sensores);

    /**
     * @brief interact
     * Realiza la interacción con el entorno por parte del agente.
     * @param accion Acción a realizar.
     * @param valor
     * @return int
     */
    int interact(Action accion, int valor);

    /**
     * @brief Pinta sobre el mapa del juego el plan obtenido.
     * @param st Estado actual del agente.
     * @param plan Plan obtenido en la búsqueda del camino.
     */
    void VisualizaPlan(const estado &st, const list<Action> &plan);

    /**
     * @brief Realiza una copia de una instancia de ComportamientoJugador.
     * @return ComportamientoJugador* 
     */
    ComportamientoJugador *clone() { return new ComportamientoJugador(*this); }

   private:
    // Declarar Variables de Estado
    // Enteros constantes del programa 
    const int VISION_DEPTH = 3;
    // Indica la anterior acción
    Action ultimaAccion;
    // Indica si el agente tiene un plan o no y cual es
    Plan elPlan;
    // Estado actual del agente
    estado actual;
    // Objetivo actual del agente
    estado objetivo_actual;
    // Lista de objetivos del agente
    list<estado> objetivos;
    // Plan trazado por la búsqueda del camino
    list<Action> plan;
    // Mapa que almacena el estado de la superficie en dicho momento
    vector<vector<unsigned char>> mapaSuperficie;
    // Contador de objetivos
    int contadorObjs = 0;
    // Indica si el agente tiene un plan, si va a recargar o si está recargando
    bool hay_plan, voyARecargar, recargando;
    // Indica si es la primera acción del agente y si ya ha decidido su siguiente acción
    bool primerInstante, elegido;
    // Indica si el agente está bien situado
    bool bien_situado;
    // Indica el nivel
    int nivel;

    // Métodos privados de la clase
    /**
     * @brief Función de costeutilizado por A* para calcular el coste de un nodo
     * a partir del terreno y la superficie.
     * @param n Nodo a evaluar.
     * @return int Coste de la acción+1. El motivo de sumarle 1 es para no poner coste
     * 0 cuando la casilla == '?' pero costando menos que ninguna.
     */
    int g(const nodo &n);

    /**
     * @brief Función heurística utilizada por A* para calcular el coste de un nodo
     * utilizando la distancia de Chebyshev. También habría sido interesante usar
     * la heurística Octile.
     * @param n Nodo a evaluar.
     * @return int Coste de la heurística.
     */
    int h(const nodo &n);

    /**
     * @brief Función que encuentra un camino desde el origen hasta el destino
     * especificado según el nivel.
     * @param level Nivel del mapa.
     * @param origen Estado de origen.
     * @param destino Estado objetivo.
     * @param plan Plan establecido tras la ejecución.
     * @return true Se ha encontrado plan.
     * @return false No se ha encontrado plan.
     */
    bool pathFinding(int level, const estado &origen,
                     const list<estado> &destino, list<Action> &plan);
    
    /**
     * @brief Implementación de la busqueda en profundidad. 
     * Entran los puntos origen y destino y devuelve la secuencia 
     * de acciones en plan, una lista de acciones.
     * @param origen Estado de origen.
     * @param destino Estado de destino.
     * @param plan Plan obtenido tras la ejecución.
     * @return true Se ha encontrado plan.
     * @return false No se ha encontrado plan.
     */
    bool pathFinding_Profundidad(const estado &origen, const estado &destino,
                                 list<Action> &plan);
    
    /**
     * @brief Implementación de la busqueda en anchura. 
     * Entran los puntos origen y destino y devuelve la secuencia 
     * de acciones en plan, una lista de acciones.
     * @param origen Estado de origen.
     * @param destino Estado de destino.
     * @param plan Plan obtenido tras la ejecución.
     * @return true Se ha encontrado plan.
     * @return false No se ha encontrado plan.
     */
    bool pathFinding_Anchura(const estado &origen, const estado &destino,
                             list<Action> &plan);
    
    /**
     * @brief Implementación de la busqueda A*. 
     * Entran los puntos origen y destino y devuelve la secuencia 
     * de acciones en plan, una lista de acciones.
     * @param origen Estado de origen.
     * @param destino Estado de destino.
     * @param plan Plan obtenido tras la ejecución.
     * @return true Se ha encontrado plan.
     * @return false No se ha encontrado plan.
     */
    bool pathFinding_A_Star(const estado &origen, const estado &destino,
                            list<Action> &plan);
    
    /**
     * @brief Función que calcula el coste de realizar una acción en una casilla.
     * @param celda Casilla con el terreno a evaluar.
     * @param accion Acción a realizar.
     * @param bikini Booleano que indica si el agente lleva bikini o no.
     * @param zapatillas Booleano que indica si el agente lleva zapatillas o no.
     * @return int Coste de la acción.
     */
    int costeAccion(unsigned char celda, Action accion, bool bikini,
                    bool zapatillas);
    
    /**
     * @brief Función que establece el coste a las entidades dinámicas
     * que nos encontremos por el mapa.
     * @param accion Acción a realizar.
     * @param casilla Casilla a la que se va a mover.
     * @return int Coste de la acción.
     */
    int costeAccionSuperficie(Action accion, char casilla);
    
    /**
     * @brief Función que calcula el mejor destino a partir de la posición del
     * agente. Este se realiza dividiendo el mapa en cuadrantes, obtiene el que 
     * tenga más casillas '?' y vuelve a aplicarlo a este nuevo cuadrante. La última casilla
     * obtenida es la que se devuelve.
     * @param origen Posición del agente.
     * @return estado Objetivo calculado.
     */
    estado calcularDestino(const estado &origen);

    /**
     * @brief Función que busca una casilla de recarga en el mapa y la establece 
     * como destino si la hay.
     * @param origen Posición del agente.
     * @param destino Destino calculado.
     * @return true Ha encontrado al menos una casilla de recarga.
     * @return false No ha encontrado ninguna casilla de recarga.
     */
    bool irABateria(const estado &origen, estado &destino);

    /**
     * @brief Busca la casilla sin descubrir más cercana al agente.
     * @param origen Posición del agente.
     * @param destino Casilla a la que se va a mover.
     * @return true Ha encontrado al menos una casilla sin descubrir a la que puede llegar.
     * @return false No ha encontrado ninguna casilla sin descubrir a la que pueda llegar.
     */
    bool irAMasCercano(const estado &origen, estado &destino);

    /**
     * @brief mapearVision
     * Función que mapea la vision del agente.
     * @param mapa Mapa que queremos pintar.
     * @param vision Sensores que usaremos para registrar el campo de vision.
     * @param orientacion Orientación del agente para pintar el mapa.
     * @param f Fila en la que se encuentra el agente.
     * @param c Columna en la que se encuentra el agente.
     */
    void mapearVision(vector<vector<unsigned char>> &mapa,
                      vector<unsigned char> vision, int orientacion, int f,
                      int c);
    
    /**
     * @brief mapearSuperficie 
     * Función que mapea la vision del agente.
     * @param mapa Mapa que queremos pintar.
     * @param vision Superficie que usaremos para registrar el campo de vision.
     * @param orientacion Orientación del agente para pintar el mapa.
     * @param f Fila en la que se encuentra el agente.
     * @param c Columna en la que se encuentra el agente.
     */
    void mapearSuperficie(vector<vector<unsigned char>> &mapa,
                          vector<unsigned char> vision, int orientacion, int f,
                          int c);
    
    /**
     * @brief calcularPerDesc
     * Función que calcula el porcentaje de mapa descubierto.
     * @return float Porcentaje de mapa descubierto.
     */
    float calcularPerDesc();

    /**
     * @brief Comprueba si la casilla que hay delante es un obstaculo. Si es un
     *  obstaculo devuelve true. Si no es un obstaculo, devuelve false y
     *  modifica st con la posición de la casilla del avance.
     * @param st Casilla a evaluar.
     * @return true Es un obstaculo.
     * @return false No es un obstaculo.
     */
    bool HayObstaculoDelante(estado &st);

    /**
     * @brief Sacar por la consola la secuencia del plan obtenido.
     * @param plan Plan a pintar.
     */
    void PintaPlan(list<Action> plan);

    /**
     * @brief Actualiza el estado de los objetos que se encuentran en el mapa.
     * @param e Estado del agente.
     */
    void actualizarObjetos(estado &e);

    /**
     * @brief Evalua si el terreno al que busca acceder es favorable.
     * @param casilla Casilla a evaluar.
     * @return true El terreno es favorable.
     * @return false El terreno es desfavorable.
     */
    bool evaluarTerreno(char casilla);

    /**
     * @brief Comprueba si el agente ha sido desplazado con la colisión y en el
     * caso afirmativo, intenta obtener su posición en el mapa.
     * @param st Supuesto estado actual del agente.
     * @param vision Campo de vision del agente.
     * @return true La posición ha sido recalculada con éxito.
     * @return false La posición no ha podido ser recalculada.
     */
    bool comprobarEmpujon(estado& st,  vector<unsigned char> vision);
};
#endif
