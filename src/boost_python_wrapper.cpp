#ifndef BOOST_SYSTEM_NO_DEPRECATED
#define BOOST_SYSTEM_NO_DEPRECATED 1
#endif

#include <utility>
#include <boost/python.hpp>
#include <boost/python/numeric.hpp>
#include <boost/python/extract.hpp>
#include <armadillo>
#include "Python.h"
#include "numpy/ndarrayobject.h"
#include "include/carmcmc.hpp"

using namespace boost::python;

arma::vec numericToArma(numeric::array na) {
    int npts = extract<int>(na.attr("shape")[0]);
    arma::vec aa(npts);
    for (int i = 0; i < npts; i++) {
        aa[i] = extract<double>(na[i]);
    }
    return aa;
}


numeric::array vectorToNumeric(std::vector<double> v) {
    int nx = v.size();
    int dimens[1] = {nx};
    object nv(handle<>(PyArray_FromDims(1, dimens, PyArray_DOUBLE)));
    for (int i = 0; i < nx; i++) {
        nv[i] = v[i];
    }
    return extract<numeric::array>(nv);
}

numeric::array armaToNumeric2(std::vector<arma::vec> aa) {
    int nx = aa.size();
    int ny = aa[0].n_elem;
    int dimens[2] = {nx, ny};
    object na(handle<>(PyArray_FromDims(2, dimens, PyArray_DOUBLE)));
    for (int i = 0; i < dimens[0]; i++) {
        for (int j = 0; j < dimens[1]; j++) {
            na[i][j] = aa[i][j];
        }
    }
    return extract<numeric::array>(na);
}

boost::python::tuple runWrapper(int sample_size, int burnin,
                                numeric::array x, 
                                numeric::array y, 
                                numeric::array dy, 
                                int p, int nwalkers, int thin=1) {
    
    arma::vec ax(numericToArma(x));
    arma::vec ay(numericToArma(y));
    arma::vec ady(numericToArma(dy));

    std::pair<std::vector<arma::vec>, std::vector<double> > runResults = 
            RunEnsembleCarSampler(sample_size, burnin, ax, ay, ady, p, nwalkers, thin);

    numeric::array convResults = armaToNumeric2(runResults.first);
    numeric::array likeResults = vectorToNumeric(runResults.second);
    return boost::python::make_tuple(likeResults, convResults);
}

BOOST_PYTHON_MODULE(_carmcmc){
    import_array();
    numeric::array::set_module_and_type("numpy", "ndarray");
    
    def("run_mcmc", runWrapper);
};