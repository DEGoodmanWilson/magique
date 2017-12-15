// ga2 - C++ genetic algorithm library
// Copyright (C) 2001 Donald E. Goodman
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
// ga2Chromosome.cpp: implementation of the ga2Chromosome class.
//
//////////////////////////////////////////////////////////////////////

#include <iostream>
#include "ga2Chromosome.h"
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/**
 * \param initialSize Specifies the size of the chromosome for pre-allocation.
 *
 * This function simply creates an instance of a chromosome of specified size.
 */
ga2Chromosome::ga2Chromosome(int initialSize) : _size(initialSize)
{
    _genes.reserve(_size);
    _isEvaluated = false;
    _crossSite = -1;
}

/**
 * This function create an instance of a chromosome that has not been
 * pre-allocated. Use ga2Chromosome::setSize() to allocate genes.
 */
ga2Chromosome::ga2Chromosome()
{
    _isEvaluated = false;
    _crossSite = -1;
}

/**
 * Destructor. Duh.
 */
ga2Chromosome::~ga2Chromosome()
{
    if (!_genes.empty())
    {
        _genes.clear();
    }
    if (!_geneMinRanges.empty())
    {
        _geneMinRanges.clear();
    }
    if (!_geneMaxRanges.empty())
    {
        _geneMaxRanges.clear();
    }
}

/**
 * \param doTrunc Should the random values be truncated to integer values?
 *
 * Randomly initializes all genes within the ranges set with
 * ga2Chromosome::setMinRanges() and ga2Chromosome::setMaxRanges().
 * \bug Uses rand() to generate random values. Not the best way
 * to do it.
 */
bool ga2Chromosome::randomInit(bool doTrunc)
{
    //if max and min ranges havent been set, or have been set improperly, return false
    if ((_geneMaxRanges.size() != _size) || (_geneMinRanges.size() != _size))
    {
        return false;
    }

    //if there is already a chromosome, delete all its genes
    if (_genes.size())
    {
        _genes.clear();
    }

    //randomly assign values to the genes within the min and max ranges
    int i;
    float f;
    for (i = 0; i < _size; ++i)
    {
        ga2Gene range = _geneMaxRanges[i] - _geneMinRanges[i];
        if (doTrunc)
        {
            f = (((float) rand() / (float) RAND_MAX) * (range + 1)) + _geneMinRanges[i];
            f = (int) f; //trunc it down to size
        }
        else
        { //no rounding
            f = (((float) rand() / (float) RAND_MAX) * range) + _geneMinRanges[i];
        }
        _genes.push_back(f);
    }

    //lastly, trip the not evaluated flag
    _isEvaluated = false;
    return true;
}

/**
 * \param func The evaluation function to use. To declare such a function, it
 * should take the form: \code double myEvalFunc( std::vector<ga2Gene> genes ) \endcode
 * The return value should be the fitness of the chromosome as represented by
 * the vector of genes.
 *
 * Sets the function to use to evaluate this chromosome. Obviously, different
 * functions may be used for different chromosomes, but the default behaviour
 * of ga2Population is to assign one function across all chromosomes.
 * \bug I cannot remember if it is important or not that higher return values
 * from the fitness function should correspond to better fitness or not. If
 * in doubt (like I am right now), make higher return values correspond to
 * better fitness.
 */
void ga2Chromosome::setEvalFunc(std::function<double(std::vector<ga2Gene>)> func)
{
    _evalFunc = func;
}

/**
 * \param start Begining of slice
 * \param end Ending of slice
 *
 * This function emulates Python's array slicing behaviour. It returns a
 * chromosome composed of all genes from the originating chromosome, from
 * start to end. %c.grabSlice(0, c.getSize()) will copy the genes in
 * chromosome c (but the genes only! Use ga2Chromosome::operator=() to make
 * an exact copy, including non-gene members.). %grabSlice(20, 25) will return
 * a chromosome with five genes, from c[20] to c[24].
 * \bug To properly emulate Python, the range should be inclusive, but is not.
 * There is much internal code that depends on this function, so fixing this
 * could be hairy. Won't be a problem in the Python port of this code =)
 */
ga2Chromosome ga2Chromosome::grabSlice(int start, int end)
{
    ga2Chromosome retval(end - start);
    retval._evalFunc = this->_evalFunc;
    int i;
    for (i = start; i < end; ++i)
    {
        retval._genes.push_back(this->_genes[i]);
        retval._geneMaxRanges.push_back(this->_geneMaxRanges[i]);
        retval._geneMinRanges.push_back(this->_geneMinRanges[i]);
    }
    return retval;
}

/**
 * Evaluate this chromosome by calling the fitness function (assiged with
 * ga2Chromosome::setEvalFunc()) If the fitness function has not been set,
 * returns 0. Useful for forcing the chromosome to re-evaluate itself.
 * \bug It is impossible to differentiate a fitness of 0 from an error caused
 * by lack of fitness function. An exception should be raised or an error code
 * set or something.
 */
double ga2Chromosome::evaluate(void)
{
    if (_evalFunc == NULL)
    {
        return 0; //there has to be a better way to deal with this case.
    }
    else
    {
        _isEvaluated = true;
        _fitness = _evalFunc(_genes);
        return _fitness;
    }
}

/**
 * Returns the fitness of the chromosome. If it has yet to be determined,
 * the fitness function set with ga2Chromosome::setEvalFunc() is called.
 * If the fitness function has not been set, returns 0.
 * Multiple calls will not impose an undue speed penalty, as the
 * fitness value is cached, and the fitness function is only ever called
 * a second time if the chromosome is changed in some way. This function
 * is preferable to ga2Chromosome::evaluate() for most purposes.
 * \bug It is impossible to differentiate a fitness of 0 from an error caused
 * by lack of fitness function. An exception should be raised or an error code
 * set or something.
 */
double ga2Chromosome::getFitness(void) const
{
    return _fitness;
}

/**
 * \param ranges A vector containing the upper bounds to consider, one value
 * per gene.
 *
 * Each gene can be given a range of values to cover. Default is [0,1]. This
 * function is to tell the chromosome what upper bounds to use for each gene.
 * \bug Unknown errors could result if the parameter ranges and the chromosome
 * size are not the same!
 */
void ga2Chromosome::setMaxRanges(std::vector<ga2Gene> ranges)
{
    if (ranges.size() != getSize())
    {
        return;
    }
    _geneMaxRanges.clear();
    int i;
    for (i = 0; i < ranges.size(); ++i)
    {
        _geneMaxRanges.push_back(ranges[i]);
    }
}

/**
 * \param ranges A vector containing the lower bounds to consider, one value
 * per gene.
 *
 * Each gene can be given a range of values to cover. Default is [0,1]. This
 * function is to tell the chromosome what lower bounds to use for each gene.
 * \bug Unknown errors could result if the parameter ranges and the chromosome
 * size are not the same!
 */
void ga2Chromosome::setMinRanges(std::vector<ga2Gene> ranges)
{
    if (ranges.size() != getSize())
    {
        return;
    }
    _geneMinRanges.clear();
    int i;
    for (i = 0; i < ranges.size(); ++i)
    {
        _geneMinRanges.push_back(ranges[i]);
    }
}

/**
 * \param genes a vector of ga2Gene to use as this chromosome's genes.
 *
 * There are two ways to initialise a chromosome's genes: one is with
 * ga2Chromosome::randomInit(). The other is to set them all by hand.
 * Useful for GA related experiments and whatnot. This function can, of course
 * be called any time.
 */
void ga2Chromosome::setGenes(std::vector<ga2Gene> genes)
{
    if (genes.size() != getSize())
    {
        return;
    }
    _genes.clear();
    int i;
    for (i = 0; i < genes.size(); ++i)
    {
        _genes.push_back(genes[i]);
    }
    _isEvaluated = false;
}

/**
 * Concatenates two chromosomes together into one. Combined with
 * ga2Chromosome::slice(), can be used to create a variety of crossover
 * operations (in addition to those provided).
 */
ga2Chromosome &ga2Chromosome::operator+(const ga2Chromosome &a)
{
    this->_size += a.getSize();
    _evalFunc = a._evalFunc;

    int i;
    for (i = 0; i < a._genes.size(); ++i)
    {
        this->_genes.push_back(a._genes[i]);
        this->_geneMaxRanges.push_back(a._geneMaxRanges[i]);
        this->_geneMinRanges.push_back(a._geneMinRanges[i]);
    }
    this->_isEvaluated = false;
    return *this;
};

ga2Chromosome &ga2Chromosome::operator+(ga2Chromosome &&a)
{
    this->_size += a.getSize();
    _evalFunc = a._evalFunc;

    int i;
    for (i = 0; i < a._genes.size(); ++i)
    {
        this->_genes.push_back(a._genes[i]);
        this->_geneMaxRanges.push_back(a._geneMaxRanges[i]);
        this->_geneMinRanges.push_back(a._geneMinRanges[i]);
    }
    return *this;
};

bool ga2Chromosome::operator<(const ga2Chromosome &a) const
{
    return (this->_fitness < a._fitness);
}


/**
 * Exactly duplicates a chromosome in memory.
 */
//ga2Chromosome &ga2Chromosome::operator=(const ga2Chromosome &a)
//{
//    if (this != &a)
//    {
//        _crossSite = a._crossSite;
//        _evalFunc = a._evalFunc;
//        _fitness = a._fitness;
//        _isEvaluated = a._isEvaluated;
//        _parent[0] = a._parent[0];
//        _parent[1] = a._parent[1];
//        _size = a._size;
//        int i;
//        _geneMaxRanges.clear();
//        _geneMinRanges.clear();
//        _genes.clear();
//        for (i = 0; i < _size; ++i)
//        {
//            _geneMaxRanges.push_back(a._geneMaxRanges[i]);
//            _geneMinRanges.push_back(a._geneMinRanges[i]);
//            _genes.push_back(a._genes[i]);
//        }
//    }
//    return *this;
//}

/**
 * Checks for equivalency, looking only at the contents of the
 * genes, and not any of the other members (like fitness for example; one of
 * the chromosomes may not have been evaluated yet).
 */
bool ga2Chromosome::operator==(const ga2Chromosome &a)
{
    int i;
    for (i = 0; i < a._size; ++i)
    {
        if (this->_genes[i] != a._genes[i])
        {
            return false;
        }
    }
    return true;
}

/**
 * Prints the useful contents of the chromosome to a stream. Can be
 * used to serialise/marshall/pickle a chromosome.
 * \bug Does not bother to evaluate a chromosome if it has not already been.
 */
std::ostream &operator<<(std::ostream &out, const ga2Chromosome &chromo)
{
    int i;

    //first, output the fitness
    if (chromo._isEvaluated)
    {
        out << chromo._fitness << "\n";
    }
    else
    {
        out << "0.0\n";
    }
    //and finally, the chromosome itself
    for (i = 0; i < chromo._genes.size() - 1; ++i)
    {
        out << chromo._genes[i] << " ";
    }
    out << chromo._genes[i] << "\n";
    return out;
}

/**
 * Reads a chromosome from a stream.
 * \bug The chromosome must have already been allocated with new, and the
 * proper size set, either in the constructor or with ga2Chromosome::setSize().
 * Also, a fitness value of 0.0 is interpreted to mean that the chromosome
 * has not yet been evaluated.
 */
std::istream &operator>>(std::istream &in, ga2Chromosome &chromo)
{
    int i;
    ga2Gene f;
    //get the fitness
    in >> chromo._fitness;
    //if a fitness is specifed (ie, non-zero) set the evaluated flag to true
    if (chromo._fitness)
    {
        chromo._isEvaluated = true;
    }
    else
    {
        chromo._isEvaluated = false;
    }
    chromo._genes.clear();
    for (i = 0; i < chromo._size; ++i)
    {
        in >> f;
        chromo._genes.push_back(f);
    }
    return in;
};

/**
 * Prints a chromosome's genes (only) into a stream. Don't know why I created
 * this function, as it looks useless (see
 * ga2Chromosome::printAsSpaceDelimetedString()).
 */
void ga2Chromosome::printAsString(std::ostream &out)
{
    int i, j;
    for (i = 0; i < _size; ++i)
    {
        j = _genes[i];
        out << j;
    }
}

/**
 * Prints a chromosome's genes (only) into a stream with space between
 * each gene value.
 */
void ga2Chromosome::printAsSpaceDelimitedString(std::ostream &out)
{
    float i, j;
    for (i = 0; i < _size - 1; ++i)
    {
        j = _genes[i];
        out << j << " ";
    }
    j = _genes[i];
    out << j;
}
