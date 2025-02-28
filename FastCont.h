#pragma once
#include <cstdint>
#include <cstring>
#include <iostream>
using namespace std;

template <class T, class id_data_type>
struct FastContElement {
    T data;
    id_data_type id;
};

template <class T, class id_data_type = uint32_t>
class FastCont {
    uint32_t alloc_size;
    id_data_type rollingID;
    FastContElement<T, id_data_type> *p;
    bool memory_leak_safety; // izbriše podatke na destruktorju, privzeto = true
    uint32_t _size;

public:
    ~FastCont();
    FastCont();
    FastCont(bool);

    template <typename... Args>
    FastCont(T, Args...);
    template <typename... Args>
    void handleInfArgs(T, Args...);
    void handleInfArgs();

    uint32_t size() { return _size; };

    id_data_type push_back(T);
    void pop_back();

    void remove_index(uint32_t);
    void remove_id(id_data_type);
    void insert(T, uint32_t);

    void clear(); // izbriše podatke
    void reset(); // resetira zaporedni ID
    void reserve_n_spots(uint32_t);
    void set_memory_leak_safety(bool);

    T *at_index(uint32_t);
    T *at_id(id_data_type);

    id_data_type get_id_at_index(uint32_t);
    int64_t find_and_return_index(T); // -1 če ne najde

    void force_import(id_data_type, T); // nastavi ID in podatke
    void set_rollingID(id_data_type new_roll) { rollingID = new_roll; }
};

#include "FastCont.cpp"