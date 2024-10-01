//
// Created by crist on 1/10/2024.
//

#ifndef SUBIR_ISAM_H
#define SUBIR_ISAM_H
#pragma once
#include "Record.h" // Asegúrate de que este archivo se encuentra en el mismo directorio.
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>

const std::string DATOS = "files/pokemon.dat";
const std::string FIRST_INDEX = "files/firstindex.dat";
const std::string SECOND_INDEX = "files/secondindex.dat";

class ISAM {
private:
    std::fstream datafile;
    std::fstream firstindex;
    std::fstream secondindex;

    void create_files() {
        datafile.open(DATOS, std::ios::out | std::ios::binary);
        firstindex.open(FIRST_INDEX, std::ios::out | std::ios::binary);
        secondindex.open(SECOND_INDEX, std::ios::out | std::ios::binary);
    }

    void close_files() {
        if (datafile.is_open()) datafile.close();
        if (firstindex.is_open()) firstindex.close();
        if (secondindex.is_open()) secondindex.close();
    }

public:
    void build() {
        std::vector<Record> records = read_csv_to_record(); // Método que debes implementar para leer los CSV.

        create_files(); // Crea y abre los archivos

        // Contador para el índice de segundo nivel
        int secondLevelCounter = 0;

        // Construir archivos
        for (const Record& record : records) {
            datafile.write(reinterpret_cast<const char*>(&record), sizeof(Record));

            // Guardar en el índice de segundo nivel
            if (record.id % 4 == 1) {
                std::streampos pos = datafile.tellp() - static_cast<std::streampos>(sizeof(Record));
                secondindex.write(reinterpret_cast<const char*>(&record.id), sizeof(int));
                secondindex.write(reinterpret_cast<const char*>(&pos), sizeof(std::streampos));

                // Guardar en el índice de primer nivel
                if (record.id % 16 == 1) {
                    firstindex.write(reinterpret_cast<const char*>(&record.id), sizeof(int));
                    firstindex.write(reinterpret_cast<const char*>(&secondLevelCounter), sizeof(int));
                    secondLevelCounter += 4; // Proporcionar espacio para el siguiente índice
                }
            }
        }

        close_files(); // Cierra los archivos
    }

    Record search(int key) {
        if (key < 1) {
            throw std::invalid_argument("Ingrese una llave valida!");
        }

        // Buscar en el primer nivel de índices
        std::vector<int> first_level_indexes = read_all_index();

        std::streampos dataPos;
        for (const auto& index : first_level_indexes) {
            if (index == key) {
                dataPos = physical_read(index);
                return read_record(dataPos);
            }
        }

        // Si no está en el primer nivel, intentar en el segundo
        std::vector<std::pair<int, std::streampos>> second_level_indexes = read_part_index(first_level_indexes.back());

        for (const auto& index : second_level_indexes) {
            if (index.first == key) {
                return read_record(index.second);
            }
        }

        throw std::runtime_error("Registro no encontrado.");
    }

    std::vector<Record> range_search(int key1, int key2) {
        std::vector<Record> result;

        for (int key = key1; key <= key2; ++key) {
            try {
                result.push_back(search(key));
            } catch (const std::runtime_error&) {
                // Ignorar excepciones de registros no encontrados
            }
        }

        return result;
    }

private:
    Record read_record(std::streampos pos) {
        datafile.open(DATOS, std::ios::in | std::ios::binary);
        if (!datafile) throw std::runtime_error("Error al leer datos");
        datafile.seekg(pos, std::ios::beg);

        Record record;
        datafile.read(reinterpret_cast<char*>(&record), sizeof(Record));
        datafile.close();
        return record;
    }

    std::vector<Record> read_csv_to_record() {
        // Implementar la lectura de CSV aquí
        // Deberías usar el parser que ya tienes para crear un vector de registros
        std::vector<Record> records;
        // Ejemplo: records.push_back(Record(...));
        return records;
    }

    std::vector<int> read_all_index() {
        std::ifstream file(FIRST_INDEX, std::ios::binary);
        std::vector<int> indexes;

        while (file.peek() != EOF) {
            int index;
            file.read(reinterpret_cast<char*>(&index), sizeof(int));
            indexes.push_back(index);
        }
        return indexes;
    }

    std::vector<std::pair<int, std::streampos>> read_part_index(int vector_pos) {
        std::ifstream file(SECOND_INDEX, std::ios::binary);
        std::vector<std::pair<int, std::streampos>> indexes;

        file.seekg(vector_pos * sizeof(std::pair<int, std::streampos>));
        for (int i = 0; i < 4; ++i) {
            if (file.peek() == EOF) break;
            int key;
            std::streampos pos;
            file.read(reinterpret_cast<char*>(&key), sizeof(int));
            file.read(reinterpret_cast<char*>(&pos), sizeof(std::streampos));
            indexes.emplace_back(key, pos);
        }
        return indexes;
    }
};

#endif //SUBIR_ISAM_H
