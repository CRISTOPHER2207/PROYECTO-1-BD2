#ifndef RECORD_H_INCLUDED
#define RECORD_H_INCLUDED

#include <iostream>
#include <fstream>
#include <map>
#include <chrono>
#include <cctype>
#include <vector>
#include <initializer_list>
#include <cstdio>
#include <unistd.h>
#include <cstring>
#include <cmath>


using namespace std;

extern string KEY;

struct Record
{
    int id;               // ID del Pokémon
    char name[30];        // Nombre del Pokémon
    char type1[15];       // Tipo 1 del Pokémon
    char type2[15];       // Tipo 2 del Pokémon (puede estar vacío)
    float height;         // Altura del Pokémon
    float weight;         // Peso del Pokémon
    int base_exp;         // Experiencia base del Pokémon
    int attack;           // Valor de ataque
    int defense;          // Valor de defensa

    long left = -1, right = -1;
    int height_node = 0;  // Altura del nodo (si estás usando AVL u otro árbol)
    pair<long, char> next = {-1, 'd'};

    // Función para obtener la clave en base a una columna dada
    int getKey(string key = KEY) const {
        if (key == "id") return id;
        else if (key == "attack") return attack;
        else if (key == "defense") return defense;
        else if (key == "base_exp") return base_exp;
        else return -1; // En caso de que no haya coincidencia
    }

    // Función de serialización para guardar en un archivo
    void serialize(ofstream& file) const {
        file.write(reinterpret_cast<const char*>(&id), sizeof(int));
        file.write(name, sizeof(name));
        file.write(type1, sizeof(type1));
        file.write(type2, sizeof(type2));
        file.write(reinterpret_cast<const char*>(&height), sizeof(float));
        file.write(reinterpret_cast<const char*>(&weight), sizeof(float));
        file.write(reinterpret_cast<const char*>(&base_exp), sizeof(int));
        file.write(reinterpret_cast<const char*>(&attack), sizeof(int));
        file.write(reinterpret_cast<const char*>(&defense), sizeof(int));
    }

    // Función de deserialización para leer desde un archivo
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&id), sizeof(int));
        file.read(name, sizeof(name));
        file.read(type1, sizeof(type1));
        file.read(type2, sizeof(type2));
        file.read(reinterpret_cast<char*>(&height), sizeof(float));
        file.read(reinterpret_cast<char*>(&weight), sizeof(float));
        file.read(reinterpret_cast<char*>(&base_exp), sizeof(int));
        file.read(reinterpret_cast<char*>(&attack), sizeof(int));
        file.read(reinterpret_cast<char*>(&defense), sizeof(int));
    }
};
#endif // RECORD_H_INCLUDED
