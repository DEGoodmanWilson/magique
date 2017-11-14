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
// ga2Chromosome.h: interface for the ga2Chromosome class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GA2CHROMOSOME_H__
#define __GA2CHROMOSOME_H__

#include <iostream>
#include <vector>
#include <functional>
#include "ga2Gene.h"

///A class representing a chromosome i.e., a member of the population.
/**
 * The ga2Chromosome class represents a single chromosome in a population.
 * A chromosome contains some number of genes, and can be sliced up and put
 * back together. A population contains a number of chromosomes.
 */
class ga2Chromosome
{
	int _size;
	std::vector< ga2Gene > _genes;
	//I'd like not to have to set these ranges for every friggen chromosome...
	std::vector< float > _geneMinRanges;
	std::vector< float > _geneMaxRanges;
	double _fitness;
	bool _isEvaluated;
	std::function<double (std::vector<ga2Gene>)> _evalFunc;
	int _parent[2];
	int _crossSite;
public:
	///The constructor
	ga2Chromosome();
	///Constructor that initializes the size of the chromosome. Probably more useful.
	ga2Chromosome( int initialSize );
	///The destructor
	virtual ~ga2Chromosome();
	///Randomly initialises the chromosome.
	bool randomInit(bool doTrunc);
	///Grabs an arbitrary slice of a chromosome.
	ga2Chromosome grabSlice(int start, int end);
	///Sets the evaluation function that gets called.
	void setEvalFunc(std::function<double(std::vector<ga2Gene>)> func);
	///Evaluate this chromosome.
	double evaluate(void);
	///Returns the size of the chromosome.
	/**
	 * Returns the size of the chromosome as set by the constructor
	 * or by calling ga2Chromosome::setSize()
	 */
	int getSize(void) const {return _size;};
	///Sets the size of the chromosome.
	void setSize(int newSize) {_size = newSize; _genes.reserve(_size);};
	///Returns the fitness of a chromosome.
	double getFitness(void);
	///Sets the minimum values for each gene.
	void setMinRanges(std::vector<ga2Gene> ranges);
	///Sets the maximum values for each gene.
	void setMaxRanges(std::vector<ga2Gene> ranges);
	///Set the value of all genes.
	void setGenes(std::vector<ga2Gene> genes);
	///Returns the maximum values for each gene.
	/**
	 * \param index The index of the gene in question.
	 *
	 * Returns the upper bound set for a particular gene in the chromosome.
	 */
	float getMaxRanges(int index) {return _geneMaxRanges[index];};
	///Returns the minimum values for each gene.
	/**
	 * \param index The index of the gene in question.
	 *
	 * Returns the lower bound set for a particular gene in the chromosome.
	 */
	float getMinRanges(int index) {return _geneMinRanges[index];};
	///Returns the gene located at index.
	/**
	 * \param index Index of the gene to set
	 * \param value Value to set gene to
	 *
	 * Sets the value of a particular gene.
	 * \bug Does not check against upper and lower bounds for that gene.
	 */
	void setGene(int index, ga2Gene value) {_genes[index] = value; _isEvaluated = false;};
	///Returns a vector of the chromosome's genes.
	/**
	 * Returns a vector containing the chromosome's genes.
	 */
	std::vector<ga2Gene> getGenes(void) {return _genes;};
	///Set a chromosome's parent from the previous generation.
	/**
	 * Used by ga2Population
	 */
	void setParent(int index, int value) {_parent[index] = value;};
	///Get a chromosome's parent from the previous generation.
	/**
	 * Used by ga2Population
	 */
	int getParent(int index) {return _parent[index];};
	///Set the crossover site.
	/**
	 * Used by ga2Population
	 */
	void setCrossSite(int val) {_crossSite = val;};
	///Get the crossover site.
	/**
	 * Used by ga2Population
	 */
	int getCrossSite(void) {return _crossSite;};
	///Print a chromosome as a string.
	void printAsString(std::ostream &out);
	///Print a chromosome as a more machine-friendly string.
	void printAsSpaceDelimitedString(std::ostream &out);
	///Concatenate two chromosomes or chromosome fragments together.
	ga2Chromosome& operator+(ga2Chromosome &&a);
	ga2Chromosome& operator+(const ga2Chromosome &a);

	///Set the values of one chromosome to those of another.
	ga2Chromosome& operator=(const ga2Chromosome &a);
	///Compare two chromosomes for equality.
	bool operator==(const ga2Chromosome &a);
	///Print the contents of a chromosome.
	friend std::ostream& operator<< ( std::ostream &out, const ga2Chromosome &chromo );
	///Read in a chromosome.
	friend std::istream& operator>> ( std::istream &in, ga2Chromosome &chromo );
};

#endif
