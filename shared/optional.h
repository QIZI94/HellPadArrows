#ifndef R_OPTIONAL_H
#define R_OPTIONAL_H
namespace detail{
	template<typename T>
	struct remove_reference {
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&> {
		using type = T;
	};

	template<typename T>
	struct remove_reference<T&&> {
		using type = T;
	};

	template<typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template<typename T>
	constexpr remove_reference_t<T>&& move(T&& t) noexcept {
		return static_cast<remove_reference_t<T>&&>(t);
	}
} // namespace detail

namespace detail{
	struct NoTypeNoneOption{};
} // namespace detail

#ifdef ROPTION_USE_NAMESPACE
namespace ropt{
#endif
template<typename T>
struct None_t{};

template<typename T>
struct Option{
public:
    constexpr Option() : m_uinitialized(0), mb_hasValue(false){}
	constexpr Option(None_t<T>) : m_uinitialized(0), mb_hasValue(false){}
	constexpr Option(None_t<detail::NoTypeNoneOption>) : m_uinitialized(0), mb_hasValue(false){}
    constexpr explicit Option(T value) : m_value(detail::move(value)), mb_hasValue(true){}
	//constexpr Option(T&& value) : m_value(detail::move(value)), mb_hasValue(true){}

    inline bool hasValue() const {
        return mb_hasValue;
    }

    inline T* ptr_value() {
        return mb_hasValue ? &m_value : nullptr;
    }
    inline const T* ptr_value() const {
        return mb_hasValue ? &m_value : nullptr;
    }

	inline T& valueOr(T& other){
		return mb_hasValue ? m_value : other;
	}

	inline const T& valueOr(const T& other){
		return mb_hasValue ? m_value : other;
	}
	inline const T valueCopyOr(T other) const{
		return valueOr(detail::move(other));
	}
	inline T& valueUnchecked() {
		return m_value;
	}
	inline const T& valueUnchecked() const {
		return m_value;
	}
	inline T valueCopyUnchecked() const {
		return m_value;
	}
	
	
private:
	union{
		char m_uinitialized;
		T m_value;
	};
    bool mb_hasValue = false;
};
template<typename T>
constexpr Option<T> Some(T value){
    return Option<T>(detail::move(value));
}

constexpr None_t<detail::NoTypeNoneOption> None = None_t<detail::NoTypeNoneOption>{};
#ifdef ROPTION_USE_NAMESPACE
} // namespace opt
#endif
#endif // R_OPTIONAL_H