/*
Action ComportamientoJugador::think(Sensores sensores) {
    Action accion = actIDLE;

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
            actual.fila = sensores.posF;
            actual.columna = sensores.posC;
            actual.orientacion = sensores.sentido;
            break;
    }

    actual.fila = sensores.posF;
    actual.columna = sensores.posC;
    actual.orientacion = sensores.sentido;

    cout << "Fila: " << actual.fila << endl;
    cout << "Columna : " << actual.columna << endl;
    cout << "Origen : " << actual.orientacion << endl;

    actualizarObjetos(actual);
    mapearVision(mapaResultado, sensores.terreno, sensores.sentido, sensores.posF, sensores.posC);

    // Capturo los destinos
    cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
    objetivos.clear();
    if (sensores.nivel < 3) {
        for (int i = 0; i < sensores.num_destinos; i++) {
            estado aux;
            aux.fila = sensores.destino[2 * i];
            aux.columna = sensores.destino[2 * i + 1];
            objetivos.push_back(aux);
        }
    } else {

        if (((float)sensores.bateria)/(float)sensores.vida < 0.65 and sensores.vida > 900) {
            elPlan = recargar;
        } if (!evaluarTerreno(sensores.terreno[2]) and plan.front() == actFORWARD) {
            elPlan = continuar;
        } if (mapaResultado[objetivo_actual.fila][objetivo_actual.columna] != '?') {
            elPlan = descubrir;
        }

        switch(elPlan) {
            case continuar:
                cout << "HACE EL CONTINUAR " << endl;
                hay_plan = false;
                objetivos.clear();
                objetivos.push_back(objetivo_actual);
                break;
            case descubrir:
                cout << "HACE EL DESCUBRIR " << endl;
                hay_plan = false;
                objetivos.clear();
                objetivos.push_back(calcularDestino(actual));
                break;
            case recargar:
                cout << "HACE EL RECARGAR " << endl;
                estado aux;
                if (irABateria(actual, aux) and !voyARecargar) {
                    voyARecargar = true;
                    hay_plan = false;
                    objetivos.clear();
                    objetivos.push_back(aux);
                }
                break;
            case ninguno:
                cout << "HACE EL NINGUNO " << endl;
                break;
        }
    }

    if (!hay_plan) {
        hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);
        if (!hay_plan) {
            elPlan = descubrir;
            decidido = true;
        }
    }

    if (((float)sensores.bateria)/(float)sensores.vida < 0.85 and sensores.terreno[0] == 'X') {
        accion = actIDLE;
        voyARecargar = false;
    } else if (hay_plan and plan.size() > 0) {
        accion = plan.front();
        plan.erase(plan.begin());
    } else {
        cout << "No esta implementado" << endl;
    }

    ultimaAccion = accion;
    return accion;
}

Action ComportamientoJugador::think(Sensores sensores) {
    Action accion = actIDLE;

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
            actual.fila = sensores.posF;
            actual.columna = sensores.posC;
            actual.orientacion = sensores.sentido;
            break;
    }

    actual.fila = sensores.posF;
    actual.columna = sensores.posC;
    actual.orientacion = sensores.sentido;

    cout << "Fila: " << actual.fila << endl;
    cout << "Columna : " << actual.columna << endl;
    cout << "Origen : " << actual.orientacion << endl;

    actualizarObjetos(actual);
    mapearVision(mapaResultado, sensores.terreno, sensores.sentido, sensores.posF, sensores.posC);

    // Capturo los destinos
    cout << "sensores.num_destinos : " << sensores.num_destinos << endl;
    //objetivos.clear();
    if (sensores.nivel != 3) {
        for (int i = 0; i < sensores.num_destinos; i++) {
            estado aux;
            aux.fila = sensores.destino[2 * i];
            aux.columna = sensores.destino[2 * i + 1];
            objetivos.push_back(aux);
        }
    } else {
        if (primeraAccion) {
            primeraAccion = false;
            objetivos.push_back(calcularDestino(actual));
        } else {

                estado destinoAux;
                if (((float)sensores.bateria)/(float)sensores.vida < 0.65) {
                    cout << "HACE EL BATERIA " << endl;
                    if (irABateria(actual, destinoAux) and !voyARecargar) {
                        cout << "LO EJECURO "<< endl;
                        voyARecargar = true;
                        hay_plan = false;
                        objetivos.clear();
                        objetivos.push_back(destinoAux);
                    }
                }
                if ((!hay_plan or (!voyARecargar and  mapaResultado[objetivo_actual.fila][objetivo_actual.columna] != '?'))) {
                    cout << "HACE EL DESCUBRIR " << endl;
                    hay_plan = false;
                    destinoAux = calcularDestino(actual);
                    objetivos.clear();
                    objetivos.push_back(destinoAux);
                    cout << "Destino DIF ?: " << mapaResultado[objetivo_actual.fila][objetivo_actual.columna] << endl;
                } else
                if (!recargando and !evaluarTerreno(sensores.terreno[2]) and plan.front() == actFORWARD) {
                    cout << "HACE EL CONTINUAR " << endl;
                    hay_plan = false;
                    objetivos.clear();
                    objetivos.push_back(objetivo_actual);
                }
        }
    }

    if (!hay_plan) {
        hay_plan = pathFinding(sensores.nivel, actual, objetivos, plan);
    }

    if (((float)sensores.bateria)/(float)sensores.vida < 0.85 and sensores.terreno[0] == 'X') {
        accion = actIDLE;
        recargando = true;
        voyARecargar = false;
    } else if (hay_plan and plan.size() > 0) {
        accion = plan.front();
        plan.erase(plan.begin());
    } else {
        cout << "No esta implementado" << endl;
    }

    ultimaAccion = accion;
    return accion;
}
*/