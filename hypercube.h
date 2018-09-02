//
// Created by sanya on 8/20/18.
//

#ifndef TEXTGEN_HYPERCUBE_H
#define TEXTGEN_HYPERCUBE_H

#include <vector>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/ublas/storage.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/numeric/ublas/vector_sparse.hpp>


namespace ublas = boost::numeric::ublas;

// typedef ublas::compressed_vector<int> CVector;

template<typename T>
struct index2d {
    T x;
    T y;
    bool operator<(const index2d &other) const {
        if (x < other.x) return true;
        if (other.x < x) return false;
        return y < other.y;
    }
    static int get_dim() { return 2; }
};

template<typename T>
struct index4d {
    T d1;
    T d2;
    T d3;
    T d4;
    bool operator<(const index4d &other) const {
        if (d1 < other.d1) return true;
        if (other.d1 < d1) return false;
        if (d2 < other.d2) return true;
        if (other.d2 < d2) return false;
        if (d3 < other.d3) return true;
        if (other.d3 < d3) return false;
        return d4 < other.d4;
    }
    static int get_dim() { return 4; }
};


template<typename T, typename indexT>
class hcube_t {

    using datamap_t = std::map<indexT, T>;

    datamap_t m_data;

    size_t m_size;

public:

    typedef typename datamap_t::iterator iterator;
    typedef typename datamap_t::const_iterator const_iterator;
    typedef T                      value_type;
    typedef value_type &           reference;
    typedef const value_type &     const_reference;
    typedef  value_type *          pointer;
    typedef  const value_type *    const_pointer;

    static int get_dim() { return indexT::get_dim(); }

    hcube_t(size_t size = 0): m_size(size) {

    }

    void resize(size_t size) {
        m_size = size;
    }

    iterator begin() {
        return m_data.begin();
    }

    iterator end() {
        return m_data.end();
    }

    size_t size() const {
        return m_size;
    }

    size_t data_size() const {
        return m_data.size();
    }

    size_t data_bytes() const {
        return m_data.size() * sizeof(indexT) + sizeof(T);
    }

    reference operator[](const indexT & i) {
        return m_data[i];
    }

};



// proxy class because ublas vectors have ugly interface

template<class Tp>
class c_vector {

    ublas::compressed_vector<Tp> data;

public:
    typedef size_t				   size_type;
    typedef Tp                     value_type;
    typedef value_type &           reference;
    typedef const value_type &     const_reference;
    typedef  value_type *          pointer;
    typedef  const value_type *    const_pointer;
//    typedef __gnu_cxx::__normal_iterator<pointer, c_vector> iterator;
//    typedef __gnu_cxx::__normal_iterator<const_pointer, c_vector> const_iterator;
    typedef typename ublas::compressed_vector<Tp>::iterator iterator;
    typedef typename ublas::compressed_vector<Tp>::const_iterator const_iterator;
    typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
    typedef std::reverse_iterator<iterator>		 reverse_iterator;



    c_vector(): data() {}

    c_vector(size_type __n): data(__n) {}

    c_vector(size_type __n, const value_type& __value): data(__n) {
        for (size_type i = 0; i < __n; ++ i)
            data(i) = __value;
    }

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.end();
    }

    const_iterator cbegin() {
        return data.cbegin();
    }

    const_iterator cend() {
        return data.cend();
    }



    size_type size() const { return data.size(); }
    size_type r_size() const {
        return data.value_data().size();
    }

    void resize(size_type __new_size) { data.resize(__new_size); }

    const_reference operator[](size_type __n) const {  return data[__n];  }

    reference operator[](size_type __n) { return data[__n].ref(); }

    reference at(size_type __n) { return data(__n); }

    const_reference at(size_type __n) const { return data(__n); }

};



// Example from here https://stackoverflow.com/questions/8579207/hypercube-with-multidimensional-vectors


template<size_t N, typename T>
struct hcube_info;

template<typename T>
struct hcube_info<1, T>
{ // base version
    typedef c_vector<T> type;

    static type init(size_t innerdim, int value = 0){
        return type(innerdim);
    }
/*
    size_t real_size(size_t & a) {
        a = type.r_size();
        return a;
    }
*/
};

template<size_t N, typename T>
struct hcube_info { // recursive definition, N dimensions
private:
    typedef hcube_info<N-1, T> base;
    typedef typename base::type btype;

public:
    typedef c_vector<btype> type;
    static type init(size_t innerdim, int value = 0) {
        return type(innerdim, base::init(innerdim, value));
    }
/*
    size_t real_size(type & t, size_t & a) {
        return real_size(t, base::real_size(t, a));
    }
*/
};

//----------------------

template<size_t N, typename T>
struct hypercube {
    typedef hcube_info<N, T> info;
public:
    typedef typename info::type vec_type;
private:
    int proc_vec(vec_type & t, int & sum, size_t dim) {

        if(dim > 1) {
            for(const auto &element : t ) {
                proc_vec(t, sum, dim - 1);
                sum += element.r_size();
            }
        }
        return sum;
    }

public:
    typedef typename vec_type::value_type value_type;
    typedef typename vec_type::size_type size_type;

    explicit hypercube() {
    }


    explicit hypercube(size_t innerdim)
            : c(info::init(innerdim))
    {
    }

    value_type& operator[](size_t i){
        return c[i];
    }

    size_type size() const { return c.size(); }

    void resize(size_type __new_size) { c.resize(__new_size); }

    int real_size() {
        int res = 0;

        proc_vec(c, res, N);

        return res;
    }

    unsigned get_dim() const  { return N; }


private:
    vec_type c;
};

#endif //TEXTGEN_HYPERCUBE_H
