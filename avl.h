
#ifndef AVL_H
#define AVL_H

#include "record.h"

using namespace std;

class AVLFile
{
private:
    string filename;
    long pos_root;
public:
    AVLFile(string _filename){
        this->pos_root = 0;
        this->filename = _filename + ".bin";

        ifstream file(filename, ios::app | ios::binary);

        if(!file.good()){
            file.close();
            ofstream f(filename, ios::app | ios::binary);
            f.close();
        }

    }

    template<class TK>
    Record find(TK key){
        ifstream file(filename, ios::binary);
        Record record = find(pos_root, key, file);
        file.close();
        return record;
    }

    template<class TK>
    vector<Record> search(TK key){
        ifstream file(filename, ios::binary);
        vector<Record> records = search(pos_root, key, file);
        file.close();
        return records;
    }

    void add(Record record){
        fstream f(this->filename, ios::in | ios::out | ios::binary);
        insert(pos_root, record, f);
        f.close();
    }

    template <class T>
    void remove(T key){
        fstream f(this->filename, ios::in | ios::out | ios::binary);
        remove(pos_root, key, f);
        f.close();
    }

    vector<Record> inorder(){
        return inorder(pos_root);
    }

    template<class TK>
    vector<Record> rangeSearch(TK begin, TK end){
        vector<Record> results = RangeSearch(pos_root, begin, end);
        return results;
    }

private:

    template<class TK>
    vector<Record> RangeSearch(long pos_node, const TK& begin, const TK& end) {
        vector<Record> result;

        if (pos_node != -1) {
            ifstream file(filename, ios::binary);
            Record record;
            file.seekg(pos_node, ios::beg);
            file.read((char*)&record, sizeof(Record));

            if (record.getKey() >= begin) {
                vector<Record> leftSubtree = RangeSearch(record.left, begin, end);
                result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

                if (record.getKey() <= end and record.getKey() != -1) {
                    result.push_back(record);
                }

                if (record.getKey() < end) {
                    vector<Record> rightSubtree = RangeSearch(record.right, begin, end);
                    result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());
                }
            } else {
                // Si el registro actual est� antes del rango, solo exploramos el sub�rbol derecho.
                vector<Record> rightSubtree = RangeSearch(record.right, begin, end);
                result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());
            }

            file.close();
        }

        return result;
    }


    vector<Record> inorder(long pos_node){
        vector<Record> result;

        if (pos_node != -1) {
            ifstream file(filename, ios::binary);
            Record record;
            file.seekg(pos_node, ios::beg);
            file.read((char*)&record, sizeof(Record));

            vector<Record> leftSubtree = inorder(record.left);
            result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

            if(record.id != -1) result.push_back(record);

            vector<Record> rightSubtree = inorder(record.right);
            result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

            file.close();
        }

        return result;
    }


    template<class TK>
    vector<Record> search(long pos_node, TK key, ifstream& file){
        vector<Record> found;

        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        if (record.getKey() == key) {
            found.push_back(record);
        }

        if (key > record.getKey() && record.right != -1) {
            vector<Record> rightResults = search(record.right, key, file);
            found.insert(found.end(), rightResults.begin(), rightResults.end());
        } else if (key < record.getKey() && record.left != -1) {
            vector<Record> leftResults = search(record.left, key, file);
            found.insert(found.end(), leftResults.begin(), leftResults.end());
        }

        return found;
    }



    void insert(long pos_node, Record record, fstream& file){
        file.seekg(0, ios::end);
        if(file.tellg() == 0){
            file.write((char*)&record, sizeof(Record));
            return;
        }

        Record curr_record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&curr_record, sizeof(Record));

        if(record.getKey() < curr_record.getKey()){
            if(curr_record.left == -1){
                file.seekp(0, ios::end);
                curr_record.left = file.tellg();
                file.write((char*)&record, sizeof(Record));
                file.seekg(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else
                insert(curr_record.left, record, file);
        }else if(record.getKey() > curr_record.getKey()){
            if(curr_record.right == -1){
                file.seekp(0, ios::end);
                curr_record.right = file.tellg();
                file.write((char*)&record, sizeof(Record));
                file.seekg(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else
                insert(curr_record.right, record, file);
        }

        updateHeight(pos_node, file);
        balance(pos_node, file);

    }

    template <class T>
    void remove(long pos_node, T key, fstream& file){
        if(pos_node == -1)
            return;
        Record curr_record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&curr_record, sizeof(Record));

        if(key < curr_record.id)
            remove(curr_record.left, key, file);
        else if(key > curr_record.id)
            remove(curr_record.right, key, file);
        else{
            if(curr_record.left == -1 && curr_record.right == -1){
                curr_record.id = -1;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else if(curr_record.left == -1){
                Record right_record;
                file.seekg(curr_record.right, ios::beg);
                file.read((char*)&right_record, sizeof(Record));
                curr_record.id = right_record.id;
                curr_record.right = right_record.right;
                curr_record.left = right_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else if(curr_record.right == -1){
                Record left_record;
                file.seekg(curr_record.left, ios::beg);
                file.read((char*)&left_record, sizeof(Record));
                curr_record.id = left_record.id;
                curr_record.right = left_record.right;
                curr_record.left = left_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else{
                Record left_record;
                file.seekg(curr_record.left, ios::beg);
                file.read((char*)&left_record, sizeof(Record));
                curr_record.id = left_record.id;
                curr_record.right = left_record.right;
                curr_record.left = left_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
                remove(curr_record.left, left_record.id, file);

            }
        }
        updateHeight(pos_node, file);
        balance(pos_node, file);

    }

    /// AVL
    int height(long pos_node, fstream& file){
        if(pos_node == -1)
            return -1;
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        return record.height;
    }

    void updateHeight(long pos_node, fstream& file){
        if (pos_node == -1)
            return;

        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        int left_height = height(record.left, file);
        int right_height = height(record.right, file);

        record.height = max(left_height, right_height) + 1;

        file.seekp(pos_node, ios::beg);
        file.write((char*)&record, sizeof(Record));
    }

    int balancingFactor(long pos_node, fstream& file){
        if(pos_node == -1)
            return 0;
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        return height(record.left, file) - height(record.right, file);
    }

    void balance(long pos_node, fstream& file){
        if(pos_node == -1)
            return;

        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        if(balancingFactor(pos_node, file) == 2){
            if(balancingFactor(record.left, file) < 0){
                left_rota(record.left, file);
            }
            right_rota(pos_node, file);
        }
        else if(balancingFactor(pos_node, file) == -2){
            if(balancingFactor(record.right, file) > 0){
                right_rota(record.right, file);
            }
            left_rota(pos_node, file);
        }
        updateHeight(pos_node, file);
    }

    void left_rota(long pos_node, fstream& file){
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        Record temp;
        long pos_temp = record.right;
        file.seekg(pos_temp, ios::beg);
        file.read((char*)&temp, sizeof(Record));

        record.right = temp.left;
        temp.left = pos_temp;

        file.seekg(pos_node, ios::beg);
        file.write((char*)&temp, sizeof(Record));

        file.seekg(pos_temp, ios::beg);
        file.write((char*)&record, sizeof(Record));

        updateHeight(pos_node, file);
        updateHeight(pos_temp, file);
    }

    void right_rota(long pos_node, fstream& file){
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        Record temp;
        long pos_temp = record.left;
        file.seekg(pos_temp, ios::beg);
        file.read((char*)&temp, sizeof(Record));

        record.left = temp.right;
        temp.right = pos_temp;

        file.seekg(pos_node, ios::beg);
        file.write((char*)&temp, sizeof(Record));

        file.seekg(pos_temp, ios::beg);
        file.write((char*)&record, sizeof(Record));

        updateHeight(pos_node, file);
        updateHeight(pos_temp, file);
    }

};


#endif // AVL_H
