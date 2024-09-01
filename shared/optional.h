#ifndef SHARED_H
#define SHARED_H

template<typename T>
struct Option{
public:
    Option() : mb_hasValue(false){
        memset(&m_value, 0, sizeof(T));
    }
    Option(T value) : m_value(value), mb_hasValue(true){}

    bool hasValue() const {
        return mb_hasValue;
    }

    T* ptr_value() {
        return mb_hasValue ? &m_value : nullptr;
    }
    const T* ptr_value() const {
        return mb_hasValue ? &m_value : nullptr;
    }
private:
    T m_value;
    bool mb_hasValue = false;
};
template<typename T>
Option<T> Some(T value){
    return Option<T>(value);
}
template<typename T>
using None = Option<T>; 

#endif // SHARED_H