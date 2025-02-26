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
    uint32_t _size;

public:
    FastCont();
    ~FastCont();
    template<typename... Args> FastCont(T, Args...);
    template <typename... Args> void handleInfArgs(T, Args...);
    void handleInfArgs();

    uint32_t size() { return _size; };

    id_data_type push_back(T);
    void pop_back();

    void remove_index(uint32_t);
    void remove_id(id_data_type);

    T *at_index(uint32_t);
    T *at_id(id_data_type);
    
    //...//
};

#include "FastCont.cpp"