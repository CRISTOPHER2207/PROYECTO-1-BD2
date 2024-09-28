#ifndef AVLARCHIVOS_FINAL_H
#define AVLARCHIVOS_FINAL_H

#include <iostream>
#include <fstream>
#include <cstring> // Para uso de strncpy
#include <stdexcept> // Para manejo de excepciones
using namespace std;

class AVL {
public:
    struct RecordAVL {
        int id; // ID del Pokémon
        char name[20]; // Nombre
        char type1[10]; // Tipo 1
        char type2[10]; // Tipo 2
        int total; // Total de stats
        int hp; // Puntos de vida
        int attack; // Ataque
        int defense; // Defensa
        int spAtk; // Ataque especial
        int spDef; // Defensa especial
        int speed; // Velocidad
        int generation; // Generación
        bool legendary; // Si es legendario o no

        RecordAVL() {}

        RecordAVL(int _id, const std::string& _name, const std::string& _type1, const std::string& _type2,
                  int _total, int _hp, int _attack, int _defense, int _spAtk, int _spDef, int _speed,
                  int _generation, bool _legendary) {
            id = _id;
            strncpy(name, _name.c_str(), sizeof(name));
            strncpy(type1, _type1.c_str(), sizeof(type1));
            strncpy(type2, _type2.c_str(), sizeof(type2));
            total = _total;
            hp = _hp;
            attack = _attack;
            defense = _defense;
            spAtk = _spAtk;
            spDef = _spDef;
            speed = _speed;
            generation = _generation;
            legendary = _legendary;
        }
    };

    struct NodeBT {
        RecordAVL data; // Datos del registro
        long left; // Índice del hijo izquierdo
        long right; // Índice del hijo derecho
        long height; // Altura del nodo

        NodeBT() : left(-1), right(-1), height(1) {} // Inicializa hijo izquierdo y derecho como -1 y altura como 1

        NodeBT(RecordAVL record) : data(record), left(-1), right(-1), height(1) {}
    };

private:
    long root; // Raíz del árbol
    string filename; // Nombre del archivo donde se almacenan los nodos

    long sizeC() {
        ifstream file(filename, ios::binary);
        file.seekg(0, ios::end);
        long nBytes = file.tellg();
        file.close();
        return nBytes;
    }

    int sizeNode() {
        return sizeof(NodeBT); // Tamaño del nodo
    }

public:
    AVL(string filename) : filename(filename), root(-1) {}

    RecordAVL find(int id) {
        return find(root, id);
    }

    void insert(RecordAVL record) {
        insert(root, record, true);
    }

private:
    RecordAVL find(long nodepos, int id) {
        ifstream file(filename, ios::binary);
        if (nodepos == -1) throw std::out_of_range("No se encontró el Pokémon con ID: " + to_string(id));

        NodeBT temp;
        file.seekg(nodepos * sizeof(NodeBT));
        file.read((char*)&temp, sizeof(NodeBT));
        file.close();

        if (id < temp.data.id) {
            return find(temp.left, id);
        } else if (id > temp.data.id) {
            return find(temp.right, id);
        } else {
            return temp.data; // Retorna el registro encontrado
        }
    }

    void insert(long node, RecordAVL value, bool sit) {
        fstream file(filename, ios::binary | ios::in | ios::out | ios::ate);
        if (node == -1) {
            NodeBT temp(value);
            NodeBT aux = getNode(node).first;
            (sit) ? aux.left = sizeC() / sizeof(NodeBT) : aux.right = sizeC() / sizeof(NodeBT);
            file.seekp(0, ios::end);
            file.write((char*)&temp, sizeof(NodeBT));
            file.seekp(node * sizeof(NodeBT));
            file.write((char*)&aux, sizeof(NodeBT));
            file.close();
            return;
        }

        NodeBT temp;
        file.seekg(node * sizeof(NodeBT));
        file.read((char*)&temp, sizeof(NodeBT));

        if (temp.data.id > value.id) {
            insert(temp.left, value, true);
        } else if (value.id < temp.data.id) {
            insert(temp.right, value, false);
        }

        updateHeight(temp, node);
        balance(node);
        file.close();
    }

    void updateHeight(NodeBT& node, long pos) {
        int leftHeight = height(node.left);
        int rightHeight = height(node.right);
        node.height = (leftHeight > rightHeight ? leftHeight : rightHeight) + 1;

        fstream file(filename, ios::binary | ios::in | ios::out | ios::ate);
        file.seekp(pos * sizeof(NodeBT));
        file.write((char*)&node, sizeof(NodeBT));
        file.close();
    }

    int height(long node) {
        return node == -1 ? 0 : getNode(node).first.height;
    }

    void balance(long node) {
        pair<NodeBT, long> temp = getNode(node);
        int balanceFactor = balancingFactor(temp.first);

        if (balanceFactor > 1) {
            pair<NodeBT, long> temp2 = getNode(temp.first.left);
            if (balancingFactor(temp2.first) < 0) {
                left_rota(temp2.first, temp2.second);
            }
            right_rota(temp.first, temp.second);
        } else if (balanceFactor < -1) {
            pair<NodeBT, long> temp2 = getNode(temp.first.right);
            if (balancingFactor(temp2.first) > 0) {
                right_rota(temp2.first, temp2.second);
            }
            left_rota(temp.first, temp.second);
        }
    }

    pair<NodeBT, long> getNode(long pos) {
        NodeBT temp;
        ifstream file(filename, ios::binary);
        file.seekg(pos * sizeof(NodeBT));
        file.read((char*)&temp, sizeof(NodeBT));
        file.close();
        return {temp, pos};
    }

    int balancingFactor(NodeBT node) {
        return height(node.left) - height(node.right);
    }

    void left_rota(NodeBT& node, long pos) {
        pair<NodeBT, long> temp = getNode(node.right);
        node.right = temp.first.left;
        temp.first.left = pos;

        updateHeight(node, pos);
        updateHeight(temp.first, temp.second);

        fstream file(filename, ios::binary | ios::in | ios::out | ios::ate);
        file.seekp(pos * sizeof(NodeBT));
        file.write((char*)&node, sizeof(NodeBT));
        file.seekp(temp.second * sizeof(NodeBT));
        file.write((char*)&temp.first, sizeof(NodeBT));
        file.close();
    }

    void right_rota(NodeBT& node, long pos) {
        pair<NodeBT, long> temp = getNode(node.left);
        node.left = temp.first.right;
        temp.first.right = pos;

        updateHeight(node, pos);
        updateHeight(temp.first, temp.second);

        fstream file(filename, ios::binary | ios::in | ios::out | ios::ate);
        file.seekp(pos * sizeof(NodeBT));
        file.write((char*)&node, sizeof(NodeBT));
        file.seekp(temp.second * sizeof(NodeBT));
        file.write((char*)&temp.first, sizeof(NodeBT));
        file.close();
    }

public:
    void display() {
        display(root);
    }

private:
    void display(long node) {
        if (node != -1) {
            pair<NodeBT, long> currentNode = getNode(node);
            display(currentNode.first.left);
            cout << "ID: " << currentNode.first.data.id << ", Name: " << currentNode.first.data.name << endl;
            display(currentNode.first.right);
        }
    }
};

#endif // AVLARCHIVOS_FINAL_H
