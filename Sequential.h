//
// Created by crist on 1/10/2024.
//

#ifndef SUBIR_SEQUENTIAL_H
#define SUBIR_SEQUENTIAL_H
#ifndef SEQUENTIALFILE_H
#define SEQUENTIALFILE_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <map>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include "Record.h"

using namespace std;

class SequentialFile{
    string filename;
    string aux_file;
    pair<long, char> head;
    long n;
public:
    SequentialFile(string _filename){
        this->filename = path + _filename + ".bin";
        this->aux_file = path + _filename + "_aux.bin";

        ifstream file(filename, ios::binary);

        if (!file) {
            file.close();
            ofstream fdata(filename, ios::app | ios::binary);
            pair<long, char> header(-1, 'd');
            head = header;
            fdata.write((char*)&header, sizeof(header));
            fdata.close();
        }else{
            file.seekg(0, ios::beg);
            file.read((char*)&head, sizeof(head));
            file.close();
        }
    };

    ~SequentialFile(){}

    long filesize(fstream& file){
        long size;
        file.seekg(0, ios::end);
        size = file.tellg();
        return size;
    }

    void ask_rebuild(){
        long data_size, aux_size;

        fstream data(filename, ios::in | ios::binary);
        fstream aux(aux_file, ios::in | ios::binary);

        data_size = (filesize(data) - sizeof(head)) / sizeof(Record);
        aux_size = filesize(aux) / sizeof(Record);

        if(aux_size > log2(data_size))
            rebuild();

        data.close();
        aux.close();
    }

    bool add(Record record){
        fstream fdata(filename, ios::in | ios::out | ios::binary);
        fstream faux(aux_file, ios::app | ios::in | ios::out | ios::binary);

        if(head.first == -1){
            record.next.first = head.first;
            record.next.second = head.second;

            head.first = faux.tellg();
            head.second = 'a';

            faux.write((char*)&record, sizeof(Record));

            fdata.seekg(0, ios::beg);
            fdata.write((char*)&head, sizeof(head));

            fdata.close();
            faux.close();
            return true;
        }
        auto key = record.getKey();
        Record curr_record, next_record;
        pair<long, char> curr_pos = head;

        if(curr_pos.second == 'd'){
            fdata.seekg(curr_pos.first, ios::beg);
            fdata.read((char*)&curr_record, sizeof(Record));
        }else if(curr_pos.second == 'a'){
            faux.seekg(curr_pos.first, ios::beg);
            faux.read((char*)&curr_record, sizeof(Record));
        }

        do{
            if(curr_pos.first == -1 or record.getKey() == curr_record.getKey()){
                fdata.close();
                faux.close();
                return false;
            }else if(key < curr_record.getKey()){
                faux.seekg(0, ios::end);
                record.next.first = head.first;
                record.next.second = head.second;

                head.first = faux.tellg();
                head.second = 'a';

                faux.write((char*)&record, sizeof(Record));

                fdata.seekg(0, ios::beg);
                fdata.write((char*)&head, sizeof(head));

                fdata.close();
                faux.close();
                return true;
            }
            else if(curr_record.getKey() < key and curr_record.next.first == -1){
                record.next = curr_pos;

                record.next = curr_record.next;

                faux.seekg(0, ios::end);
                curr_record.next.first = faux.tellg();
                curr_record.next.second = 'a';

                faux.write((char*)&record, sizeof(Record));

                if(curr_pos.second == 'd'){
                    fdata.close();
                    faux.close();
                    ofstream f(filename, ios::in | ios::out | ios::binary);
                    f.seekp(curr_pos.first, ios::beg);
                    f.write((char*)&curr_record, sizeof(curr_record));
                    f.close();
                }else if(curr_pos.second == 'a'){
                    fdata.close();
                    faux.close();
                    ofstream f(aux_file, ios::in | ios::out | ios::binary);
                    f.seekp(curr_pos.first, ios::beg);
                    f.write((char*)&curr_record, sizeof(curr_record));
                    f.close();
                }

                return true;
            }else if(curr_record.getKey() < key) {
                if (curr_record.next.second == 'd') {
                    fdata.seekg(curr_record.next.first, ios::beg);
                    fdata.read((char *) &next_record, sizeof(Record));
                } else if (curr_record.next.second == 'a') {
                    faux.seekg(curr_record.next.first, ios::beg);
                    faux.read((char *) &next_record, sizeof(Record));
                }
                if (key < next_record.getKey()) {
                    record.next = curr_record.next;
                    faux.seekg(0, ios::end);
                    curr_record.next.first = faux.tellg();
                    curr_record.next.second = 'a';
                    faux.write((char *) &record, sizeof(Record));

                    if (curr_pos.second == 'd') {
                        fdata.close();
                        faux.close();
                        ofstream f(filename, ios::in | ios::out | ios::binary);
                        f.seekp(curr_pos.first, ios::beg);
                        f.write((char *) &curr_record, sizeof(curr_record));
                        f.close();
                    } else if (curr_pos.second == 'a') {
                        fdata.close();
                        faux.close();
                        ofstream f(aux_file, ios::in | ios::out | ios::binary);
                        f.seekp(curr_pos.first, ios::beg);
                        f.write((char *) &curr_record, sizeof(curr_record));
                        f.close();
                    }
                    return true;
                } else {
                    curr_pos = curr_record.next;
                    curr_record = next_record;
                }
            }
        }while(true);
    };

    vector<Record> inorder(){
        vector<Record> found;

        pair<long, char> curr_pos = head;

        fstream fdata(filename, ios::in | ios::out | ios::binary);
        fstream faux(aux_file, ios::in | ios::out | ios::binary);
        Record record;

        while(curr_pos.first != -1){
            if(curr_pos.second == 'd'){
                fdata.seekg(curr_pos.first, ios::beg);
                fdata.read((char*)&record, sizeof(Record));
                found.push_back(record);
                curr_pos = record.next;
            }else if(curr_pos.second == 'a'){
                faux.seekg(curr_pos.first, ios::beg);
                faux.read((char*)&record, sizeof(Record));
                found.push_back(record);
                curr_pos = record.next;
            }
        }

        fdata.close();
        faux.close();

        return found;
    }

    template<class T>
    vector<Record> search(T key){
        vector<Record> found_records;

        pair<long, char> curr_pos = head;
        fstream fdata(filename, ios::in | ios::out | ios::binary);
        fstream faux(aux_file, ios::in | ios::out | ios::binary);
        Record record;

        while(curr_pos.first != -1){
            if(curr_pos.second == 'd'){
                fdata.seekg(curr_pos.first, ios::beg);
                fdata.read((char*)&record, sizeof(Record));
                if(record.getKey() == key)
                    found_records.push_back(record);
                curr_pos = record.next;
            }else if(curr_pos.second == 'a'){
                faux.seekg(curr_pos.first, ios::beg);
                faux.read((char*)&record, sizeof(Record));
                if(record.getKey() == key)
                    found_records.push_back(record);
                curr_pos = record.next;
            }
        }

        fdata.close();
        faux.close();

        return found_records;
    };

    template<class T>
    vector<Record> rangeSearch(T begin, T end){
        rebuild();

        ifstream file(filename, ios::binary);

        vector<Record> results;
        Record curr_record;
        file.seekg(sizeof(head), ios::beg);

        while(file.read((char*)&curr_record, sizeof(Record)) && curr_record.getKey() <= end){
            if(curr_record.getKey() >= begin)
                results.push_back(curr_record);
        }

        file.close();
        return results;
    };

    template<class T>
    bool remove(T key){
        pair<long, char> curr_pos = head;
        Record curr_record, next_record;

        fstream fdata(filename, ios::in | ios::out | ios::binary);
        fstream faux(aux_file, ios::app | ios::in | ios::out | ios::binary);

        if(curr_pos.second == 'd'){
            fdata.seekg(curr_pos.first, ios::beg);
            fdata.read((char*)&curr_record, sizeof(Record));
        }else if(curr_pos.second == 'a'){
            faux.seekg(curr_pos.first, ios::beg);
            faux.read((char*)&curr_record, sizeof(Record));
        }

        if(key == curr_record.getKey()){
            head = curr_record.next;
            fdata.seekg(0, ios::beg);
            fdata.write((char*)&head, sizeof(head));
            fdata.close();
            faux.close();
            return true;
        }

        do{
            if(curr_record.next.first != -1){
                if(curr_record.next.second == 'd'){
                    fdata.seekg(curr_record.next.first, ios::beg);
                    fdata.read((char*)&next_record, sizeof(Record));
                }else if(curr_record.next.second == 'a'){
                    faux.seekg(curr_record.next.first, ios::beg);
                    faux.read((char*)&next_record, sizeof(Record));
                }

                if(key == next_record.getKey()){
                    curr_record.next = next_record.next;
                    fdata.close();
                    faux.close();

                    if (curr_pos.second == 'd') {
                        ofstream f(filename, ios::in | ios::out | ios::binary);
                        f.seekp(curr_pos.first, ios::beg);
                        f.write((char *) &curr_record, sizeof(curr_record));
                        f.close();
                    } else if (curr_pos.second == 'a') {
                        ofstream f(aux_file, ios::in | ios::out | ios::binary);
                        f.seekp(curr_pos.first, ios::beg);
                        f.write((char *) &curr_record, sizeof(curr_record));
                        f.close();
                    }
                    return true;
                }else{
                    curr_pos = curr_record.next;
                    curr_record = next_record;
                }

            }else{
                fdata.close();
                faux.close();
                return false;
            }
        }while(true);
    };

private:
    void rebuild(){
        pair<long, char> curr_pos = head;
        pair<long, char> header = {sizeof(header), 'd'};

        ofstream file("rebuild.bin", ios::binary);

        file.seekp(0, ios::beg);
        file.write((char*)&header, sizeof(header));

        fstream fdata(filename, ios::in | ios::out | ios::binary);
        fstream faux(aux_file, ios::in | ios::out | ios::binary);
        Record record;

        while(curr_pos.first != -1){
            if(curr_pos.second == 'd'){
                fdata.seekg(curr_pos.first, ios::beg);
                fdata.read((char*)&record, sizeof(Record));
                curr_pos = record.next;
                if(curr_pos.first == -1)
                    record.next = {-1, 'd'};
                else
                    record.next = {(long)file.tellp() + sizeof(Record), 'd'};
                file.write((char*)&record, sizeof(Record));
            }else if(curr_pos.second == 'a'){
                faux.seekg(curr_pos.first, ios::beg);
                faux.read((char*)&record, sizeof(Record));
                curr_pos = record.next;
                if(curr_pos.first == -1)
                    record.next = {-1, 'd'};
                else
                    record.next = {(long)file.tellp() + sizeof(Record), 'd'};
                file.write((char*)&record, sizeof(Record));
            }
        }
        fdata.close();
        faux.close();
        file.close();

        string old_name = "rebuild.bin";
        string new_name = filename;

        auto removing_aux = std::remove(aux_file.c_str());
        auto removing_data = std::remove(filename.c_str());
        auto renaming = std::rename(old_name.c_str(), new_name.c_str());
    }

};

#endif // SEQUENTIALFILE_H
#endif //SUBIR_SEQUENTIAL_H
