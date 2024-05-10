#include <concepts>
#include <iostream>
#include <sstream>
#include <array>

template <typename T>
concept Number = std::integral<T> || std::floating_point<T>;

using index = std::size_t;

template <typename T>
concept Index = std::same_as<T, index>; //for variadic functions

template <Number T, index... dimensions>
class Matrix;

template <Number T, index... dimensions>
std::ostream& operator<<(std::ostream& os, Matrix<T, dimensions...> const& m) {
    os << m.to_string();
    return os;
}

// zero-dimensional matrix
template<Number T>
class Matrix<T> {
    T value;
public:
    /*implicit*/ Matrix(T t) : value {t} {}
    T operator[](this Matrix const& m) {
        return m.value;
    }
    T& operator[](this Matrix& m) {
        return m.value;
    }
    std::string to_string(this Matrix const& m) {
        std::stringstream ss;
        ss << m.value;
        return ss.str();
    }
    template<Index... indices>
    std::string to_string_impl(this Matrix const& m, indices... i) {
        std::stringstream ss;
        ss << "m[";
        const char* separator = "";
        (((ss << separator << i), separator = ", "), ...);
        ss << "] = " << m.value << '\n';
        return ss.str();
    }
};

template<Number T, index dimension, index... rest_of_dimensions>
class Matrix<T, dimension, rest_of_dimensions...> {
    using Col = Matrix<T, rest_of_dimensions...>;
    std::array<Col, dimension> values;
public:
    /*implicit*/ Matrix(std::array<Col, dimension> el) : values{std::move(el)} {}
    template<Index... RestOfIndices>
        requires (sizeof...(RestOfIndices) == sizeof...(rest_of_dimensions))
    T operator[](this Matrix const& m, index i, RestOfIndices... r) {
        return m.values[i][r...];
    }
    template<Index... RestOfIndices>
        requires (sizeof...(RestOfIndices) == sizeof...(rest_of_dimensions))
    T& operator[](this Matrix& m, index i, RestOfIndices... r) {
        return m.values[i][r...];
    }
    std::string to_string(this Matrix const& m) {
        return m.to_string_impl();
    }
    template<Index... Indices>
    std::string to_string_impl(this Matrix const& m, Indices... i) {
        std::stringstream ss;
        for (index j = 0; j < dimension; ++j) {
            ss << m.values[j].to_string_impl(i..., j);
        }
        return ss.str();
    }
};

int main() {
    constexpr index num_rows = 3;
    constexpr index num_cols = 2;
    Matrix<int, num_rows, num_cols> m{{
        Matrix<int, num_cols>{{1, 2}},
        Matrix<int, num_cols>{{3, 4}},
        Matrix<int, num_cols>{{5, 6}}
    }};
    std::cout << m;
    return 0;
}
