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


// proxy class because ublas vectors have ugly interface

template<class Tp>
class c_vector {

    ublas::compressed_vector<Tp> data;

public:
    typedef size_t				   size_type;
    typedef Tp                     value_type;
    typedef value_type &           reference;
    typedef const value_type &     const_reference;

    c_vector(): data() {}

    c_vector(size_type __n): data(__n) {}

    c_vector(size_type __n, const value_type& __value) {
        for (size_type i = 0; i < __n; ++ i)
            data(i) = __value;
    }

    size_type size() const { return data.size(); }

    void resize(size_type __new_size) { data.resize(__new_size); }

    const_reference operator[](size_type __n) const {  return data[__n];  }

    reference operator[](size_type __n) { return data[__n].ref(); }

    reference at(size_type __n) { return data(__n); }

    const_reference at(size_type __n) const { return data(__n); }

};



// Example from here https://stackoverflow.com/questions/8579207/hypercube-with-multidimensional-vectors


template<unsigned N>
struct hcube_info;

template<>
struct hcube_info<1>
{ // base version
    typedef c_vector<int> type;
    static type init(unsigned innerdim, int value = 0){
        return type(innerdim, value);
    }
};

template<unsigned N>
struct hcube_info { // recursive definition, N dimensions
private:
    typedef hcube_info<N-1> base;
    typedef typename base::type btype;

public:
    typedef c_vector<btype> type;
    static type init(unsigned innerdim, int value = 0) {
        return type(innerdim, base::init(innerdim, value));
    }
};

//----------------------

template<unsigned N>
struct hypercube {
private:
    typedef hcube_info<N> info;
    typedef typename info::type vec_type;

public:
    typedef typename vec_type::value_type value_type;
    typedef typename vec_type::size_type size_type;

    explicit hypercube(unsigned innerdim, unsigned value = 0)
            : c(info::init(innerdim, value))
    {
    }

    value_type& operator[](unsigned i){
        return c[i];
    }

    size_type size() const { return c.size(); }

private:
    vec_type c;
};

#endif //TEXTGEN_HYPERCUBE_H
