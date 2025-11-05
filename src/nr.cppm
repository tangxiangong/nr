module;

#include <format>
#include <initializer_list>
#include <memory>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

export module nr;

export void hello() { std::println("Hello from C++23!"); }

export struct NRError {
    std::string message;
    std::string file;
    size_t line;
    NRError(std::string msg, std::string f, const size_t l)
        : message(std::move(msg)), file(std::move(f)), line(l) {}
};

export template <>
struct std::formatter<NRError> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const NRError& err, FormatContext& ctx) const {
        return std::format_to(ctx.out(),
                              "ERROR: {}\n    in file: {} at line: {}",
                              err.message, err.file, err.line);
    }
};

export template <typename T>
class NRVector {
   private:
    // size of array
    size_t m_len = 0;
    std::unique_ptr<T[]> m_data = nullptr;

   public:
    NRVector() = default;
    explicit NRVector(size_t);           // zero-based array
    NRVector(size_t, const T&);          // initialize to constant value
    NRVector(size_t, const T*);          // initialize to array
    NRVector(const NRVector&);           // copy constructor
    NRVector(std::initializer_list<T>);  // initialize to initializer list
    ~NRVector() = default;
    using value_type = T;
    NRVector& operator=(const NRVector&);  // copy assignment
    inline T& operator[](size_t);
    inline const T& operator[](size_t) const;
    [[nodiscard]] inline size_t size() const;
    void resize(size_t);
    void assign(size_t, const T&);  // resize and assign constant value
};

export template <typename T>
struct std::formatter<NRVector<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const NRVector<T>& vec, FormatContext& ctx) const {
        std::string result = std::format("NRVector (length={})\n[", vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::format("{}", vec[i]);
        }
        result += "]";
        return std::format_to(ctx.out(), "{}", result);
    }
};

template <typename T>
NRVector<T>::NRVector(size_t n)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {}

template <typename T>
NRVector<T>::NRVector(size_t n, const T& val)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {
    for (size_t i = 0; i < n; ++i) m_data[i] = val;
}

template <typename T>
NRVector<T>::NRVector(size_t n, const T* arr)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {
    if (arr == nullptr && n > 0)
        throw NRError("Null pointer exception in NRVector constructor",
                      __FILE__, __LINE__);
    if (n > 0 && arr != nullptr) {
        for (size_t i = 0; i < n; ++i) m_data[i] = arr[i];
    }
}

template <typename T>
NRVector<T>::NRVector(const NRVector& vec)
    : m_len(vec.m_len),
      m_data(vec.m_len == 0 ? nullptr : std::make_unique<T[]>(vec.m_len)) {
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_len; ++i) m_data[i] = vec[i];
    }
}

template <typename T>
NRVector<T>::NRVector(std::initializer_list<T> init)
    : m_len(init.size()),
      m_data(init.size() == 0 ? nullptr : std::make_unique<T[]>(init.size())) {
    size_t i = 0;
    for (const auto& val : init) {
        m_data[i++] = val;
    }
}

template <class T>
NRVector<T>& NRVector<T>::operator=(const NRVector<T>& rhs) {
    if (this != &rhs) {
        if (m_len != rhs.m_len) {
            m_data.reset();
            m_len = rhs.m_len;
            m_data = m_len > 0 ? std::make_unique<T[]>(m_len) : nullptr;
        }
        if (m_data != nullptr) {
            for (size_t i = 0; i < m_len; ++i) m_data[i] = rhs[i];
        }
    }
    return *this;
}

template <class T>
inline T& NRVector<T>::operator[](const size_t idx) {
    if (idx >= m_len) {
        throw std::out_of_range("NRVector subscript out of bounds");
    }
    return m_data[idx];
}

template <class T>
inline const T& NRVector<T>::operator[](const size_t idx) const {
    if (idx >= m_len) {
        throw std::out_of_range("NRVector subscript out of bounds");
    }
    return m_data[idx];
}

template <class T>
inline size_t NRVector<T>::size() const {
    return m_len;
}

template <class T>
void NRVector<T>::resize(size_t new_len) {
    if (new_len != m_len) {
        m_data.reset();
        m_len = new_len;
        m_data = m_len > 0 ? std::make_unique<T[]>(m_len) : nullptr;
    }
}

template <class T>
void NRVector<T>::assign(size_t new_len, const T& a) {
    if (new_len != m_len) {
        m_data.reset();
        m_len = new_len;
        m_data = m_len > 0 ? std::make_unique<T[]>(m_len) : nullptr;
    }
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_len; ++i) m_data[i] = a;
    }
}
