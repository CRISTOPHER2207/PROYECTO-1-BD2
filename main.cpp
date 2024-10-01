#include "Parser.h"

void writeRecordsToCSV(const vector<Record>& records, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error abriendo el archivo CSV." << endl;
        return;
    }

    // Escribir encabezados de columnas
    file << "id,name,type1,type2,total,hp,attack,defense\n";

    // Escribir cada Record en una línea del archivo CSV
    for (const auto& record : records) {
        file << record.id << ","
             << record.name << ","
             << record.type1 << ","
             << record.type2 << ","
             << record.total << ","
             << record.hp << ","
             << record.attack << ","
             << record.defense << "\n";
    }

    file.close();
}

int main() {
    // Supongamos que tienes un archivo CSV llamado "pokemon.csv"
    // y quieres crear una estructura AVL con él.
    // Insertar un nuevo registro

    // Realizar una consulta SELECT
    //Sequential avl("pokemonseq");
    //readCSVFile("Pokemon.csv", avl);
    //Dinamic avl("pokemonhash");
    //readCSVFile("Pokemon.csv", avl);

    std::ifstream consultaFile("consulta.txt");
    if (!consultaFile.is_open()) {
        std::cerr << "No se pudo abrir el archivo consulta.txt" << std::endl;
        return 1;
    }

    // Leer la consulta del archivo
    std::string consulta;
    std::getline(consultaFile, consulta);
    consultaFile.close();

    // Mostrar la consulta leída (o procesarla)
    std::cout << "Consulta leída: " << consulta << std::endl;

    //string selectCommand = "SELECT * FROM pokemonavll  WHERE id == 140 USING AVL";
    Parser selectParser(consulta);
    vector<Record> foundRecords;
    selectParser.parse(foundRecords);


    for(const auto& record : foundRecords){
        cout << "ID: " << record.id << ", Name: " << record.name<< ", Type1: " << record.type1<< ", Type2: " << record.type2 << endl;
    }

    // Mostrar los registros encontrados
    writeRecordsToCSV(foundRecords, "records.csv");
    cout << "Datos exportados a records.csv" << endl;
    return 0;
}
