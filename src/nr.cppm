module;

#include <cstddef>
#include <format>
#include <initializer_list>
#include <limits>
#include <memory>
#include <print>
#include <stdexcept>
#include <string>
#include <utility>

export module nr;

export void hello() { std::println("Hello from C++23!"); }

// Helper function to check for multiplication overflow
inline bool will_multiply_overflow(size_t a, size_t b) noexcept {
    if (a == 0 || b == 0) return false;
    return a > std::numeric_limits<size_t>::max() / b;
}

export struct Error {
    std::string message;
    std::string file;
    size_t line;
    Error(std::string msg, std::string f, const size_t l)
        : message(std::move(msg)), file(std::move(f)), line(l) {}
};

export template <>
struct std::formatter<Error> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Error& err, FormatContext& ctx) const {
        return std::format_to(ctx.out(),
                              "ERROR: {}\n    in file: {} at line: {}",
                              err.message, err.file, err.line);
    }
};

export template <typename T>
class Vector {
   private:
    // size of array
    size_t m_len = 0;
    std::unique_ptr<T[]> m_data = nullptr;

   public:
    Vector() = default;
    explicit Vector(size_t);           // zero-based array
    Vector(size_t, const T&);          // initialize to constant value
    Vector(size_t, const T*);          // initialize to array
    Vector(const Vector&);             // copy constructor
    Vector(Vector&&) noexcept;         // move constructor
    Vector(std::initializer_list<T>);  // initialize to initializer list
    ~Vector() = default;
    using value_type = T;
    Vector& operator=(const Vector&);      // copy assignment
    Vector& operator=(Vector&&) noexcept;  // move assignment
    T& operator[](size_t);
    const T& operator[](size_t) const;
    [[nodiscard]] size_t size() const noexcept;
    void resize(size_t);
    void assign(size_t, const T&);  // resize and assign constant value
};

export template <typename T>
struct std::formatter<Vector<T>> : std::formatter<std::string> {
    template <typename FormatContext>
    auto format(const Vector<T>& vec, FormatContext& ctx) const {
        std::string result = std::format("Vector (length={})\n[", vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) result += ", ";
            result += std::format("{}", vec[i]);
        }
        result += "]";
        return std::format_to(ctx.out(), "{}", result);
    }
};

template <typename T>
Vector<T>::Vector(size_t n)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {}

template <typename T>
Vector<T>::Vector(size_t n, const T& val)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {
    for (size_t i = 0; i < n; ++i) m_data[i] = val;
}

template <typename T>
Vector<T>::Vector(size_t n, const T* arr)
    : m_len(n), m_data(n == 0 ? nullptr : std::make_unique<T[]>(n)) {
    if (arr == nullptr && n > 0)
        throw Error("Null pointer exception in Vector constructor", __FILE__,
                    __LINE__);
    if (n > 0 && arr != nullptr) {
        for (size_t i = 0; i < n; ++i) m_data[i] = arr[i];
    }
}

template <typename T>
Vector<T>::Vector(const Vector& vec)
    : m_len(vec.m_len),
      m_data(vec.m_len == 0 ? nullptr : std::make_unique<T[]>(vec.m_len)) {
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_len; ++i) m_data[i] = vec[i];
    }
}

template <typename T>
Vector<T>::Vector(Vector&& other) noexcept
    : m_len(other.m_len), m_data(std::move(other.m_data)) {
    other.m_len = 0;
}

template <typename T>
Vector<T>::Vector(std::initializer_list<T> init)
    : m_len(init.size()),
      m_data(init.size() == 0 ? nullptr : std::make_unique<T[]>(init.size())) {
    size_t i = 0;
    for (const auto& val : init) {
        m_data[i++] = val;
    }
}

template <typename T>
Vector<T>& Vector<T>::operator=(const Vector<T>& rhs) {
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

template <typename T>
Vector<T>& Vector<T>::operator=(Vector<T>&& rhs) noexcept {
    if (this != &rhs) {
        m_data = std::move(rhs.m_data);
        m_len = rhs.m_len;
        rhs.m_len = 0;
    }
    return *this;
}

template <typename T>
T& Vector<T>::operator[](const size_t idx) {
    if (idx >= m_len) {
        throw std::out_of_range("Vector subscript out of bounds");
    }
    return m_data[idx];
}

template <typename T>
const T& Vector<T>::operator[](const size_t idx) const {
    if (idx >= m_len) {
        throw std::out_of_range("Vector subscript out of bounds");
    }
    return m_data[idx];
}

template <typename T>
size_t Vector<T>::size() const noexcept {
    return m_len;
}

template <typename T>
void Vector<T>::resize(size_t new_len) {
    if (new_len != m_len) {
        m_data.reset();
        m_len = new_len;
        m_data = m_len > 0 ? std::make_unique<T[]>(m_len) : nullptr;
    }
}

template <typename T>
void Vector<T>::assign(size_t new_len, const T& a) {
    if (new_len != m_len) {
        m_data.reset();
        m_len = new_len;
        m_data = m_len > 0 ? std::make_unique<T[]>(m_len) : nullptr;
    }
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_len; ++i) m_data[i] = a;
    }
}

template <typename T>
class Matrix {
   private:
    size_t m_rows = 0;
    size_t m_cols = 0;
    std::unique_ptr<T[]> m_data = nullptr;

   public:
    Matrix() = default;
    Matrix(size_t, size_t);                // Zero-based array
    Matrix(size_t, size_t, const T&);      // Initialize to constant
    Matrix(size_t, size_t, const T*);      // Initialize to array
    Matrix(const Matrix&);                 // Copy constructor
    Matrix(Matrix&&) noexcept;             // Move constructor
    Matrix& operator=(const Matrix&);      // Copy assignment
    Matrix& operator=(Matrix&&) noexcept;  // Move assignment
    using value_type = T;                  // make T available externally
    T& operator[](const size_t,
                  const size_t);  // subscripting: pointer to row i
    const T& operator[](const size_t, const size_t) const;
    [[nodiscard]] size_t nrows() const noexcept;
    [[nodiscard]] size_t ncols() const noexcept;
    void resize(size_t, size_t);  // resize (contents not preserved)
    void assign(size_t, size_t,
                const T&);  // resize and assign a constant value
    ~Matrix() = default;
};

template <typename T>
Matrix<T>::Matrix(size_t rows, size_t cols)
    : m_rows(rows),
      m_cols(cols),
      m_data(rows * cols == 0 ? nullptr : std::make_unique<T[]>(rows * cols)) {
    if (will_multiply_overflow(rows, cols))
        throw std::overflow_error("Matrix size overflow");
}

template <typename T>
Matrix<T>::Matrix(size_t rows, size_t cols, const T& a)
    : m_rows(rows),
      m_cols(cols),
      m_data(rows * cols == 0 ? nullptr : std::make_unique<T[]>(rows * cols)) {
    if (will_multiply_overflow(rows, cols))
        throw std::overflow_error("Matrix size overflow");
    if (m_data != nullptr) {
        for (size_t i = 0; i < rows * cols; ++i) m_data[i] = a;
    }
}

template <typename T>
Matrix<T>::Matrix(size_t rows, size_t cols, const T* a)
    : m_rows(rows),
      m_cols(cols),
      m_data(rows * cols == 0 ? nullptr : std::make_unique<T[]>(rows * cols)) {
    if (will_multiply_overflow(rows, cols))
        throw std::overflow_error("Matrix size overflow");
    if (a == nullptr && rows * cols > 0)
        throw Error("Null pointer exception in Matrix constructor", __FILE__,
                    __LINE__);
    if (m_data != nullptr) {
        for (size_t i = 0; i < rows * cols; ++i) m_data[i] = a[i];
    }
}

template <typename T>
Matrix<T>::Matrix(const Matrix<T>& rhs)
    : m_rows(rhs.m_rows),
      m_cols(rhs.m_cols),
      m_data(m_rows * m_cols == 0 ? nullptr
                                  : std::make_unique<T[]>(m_rows * m_cols)) {
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_rows * m_cols; ++i) m_data[i] = rhs.m_data[i];
    }
}

template <typename T>
Matrix<T>::Matrix(Matrix<T>&& other) noexcept
    : m_rows(other.m_rows),
      m_cols(other.m_cols),
      m_data(std::move(other.m_data)) {
    other.m_rows = 0;
    other.m_cols = 0;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(const Matrix<T>& rhs) {
    if (this != &rhs) {
        if (m_rows != rhs.m_rows || m_cols != rhs.m_cols) {
            m_data.reset();
            m_rows = rhs.m_rows;
            m_cols = rhs.m_cols;
            m_data = m_rows * m_cols > 0
                         ? std::make_unique<T[]>(m_rows * m_cols)
                         : nullptr;
        }
        if (m_data != nullptr) {
            for (size_t i = 0; i < m_rows * m_cols; ++i)
                m_data[i] = rhs.m_data[i];
        }
    }
    return *this;
}

template <typename T>
Matrix<T>& Matrix<T>::operator=(Matrix<T>&& rhs) noexcept {
    if (this != &rhs) {
        m_data = std::move(rhs.m_data);
        m_rows = rhs.m_rows;
        m_cols = rhs.m_cols;
        rhs.m_rows = 0;
        rhs.m_cols = 0;
    }
    return *this;
}

template <typename T>
size_t Matrix<T>::nrows() const noexcept {
    return m_rows;
}

template <typename T>
size_t Matrix<T>::ncols() const noexcept {
    return m_cols;
}

template <typename T>
T& Matrix<T>::operator[](const size_t i, const size_t j) {
    if (i >= m_rows || j >= m_cols) {
        throw std::out_of_range("Matrix subscript out of bounds");
    }
    return m_data[i * m_cols + j];
}

template <typename T>
const T& Matrix<T>::operator[](const size_t i, const size_t j) const {
    if (i >= m_rows || j >= m_cols) {
        throw std::out_of_range("Matrix subscript out of bounds");
    }
    return m_data[i * m_cols + j];
}

template <typename T>
void Matrix<T>::resize(size_t new_rows, size_t new_cols) {
    if (new_rows != m_rows || new_cols != m_cols) {
        if (will_multiply_overflow(new_rows, new_cols))
            throw std::overflow_error("Matrix size overflow");
        m_data.reset();
        m_rows = new_rows;
        m_cols = new_cols;
        m_data = m_rows * m_cols > 0 ? std::make_unique<T[]>(m_rows * m_cols)
                                     : nullptr;
    }
}

template <typename T>
void Matrix<T>::assign(size_t new_rows, size_t new_cols, const T& a) {
    if (new_rows != m_rows || new_cols != m_cols) {
        if (will_multiply_overflow(new_rows, new_cols))
            throw std::overflow_error("Matrix size overflow");
        m_data.reset();
        m_rows = new_rows;
        m_cols = new_cols;
        m_data = m_rows * m_cols > 0 ? std::make_unique<T[]>(m_rows * m_cols)
                                     : nullptr;
    }
    if (m_data != nullptr) {
        for (size_t i = 0; i < m_rows * m_cols; ++i) m_data[i] = a;
    }
}
