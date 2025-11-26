#ifndef SHARED_H
#define SHARED_H

template<typename T>
struct Option{
public:
    Option() : mb_hasValue(false){
        memset(&m_value, 0, sizeof(T));
    }
    constexpr Option(T value) : m_value(value), mb_hasValue(true){}

    bool hasValue() const {
        return mb_hasValue;
    }

    T* ptr_value() {
        return mb_hasValue ? &m_value : nullptr;
    }
    const T* ptr_value() const {
        return mb_hasValue ? &m_value : nullptr;
    }

	T& valueOr(T& other){
		return mb_hasValue ? m_value : other;
	}

	const T& valueOr(const T& other){
		return mb_hasValue ? m_value : other;
	}
	const T valueCopyOr(T other) const{
		mb_hasValue ? m_value : other;
	}
	T& valueUnchecked() {
		return m_value;
	}
	const T& valueUnchecked() const {
		return m_value;
	}
	T valueCopyUnchecked() const {
		return m_value;
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