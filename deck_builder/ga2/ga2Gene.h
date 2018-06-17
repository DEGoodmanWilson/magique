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
// ga2Gene.h: just need this one typedef...of course, if you want your genes
//            to be something other than floats, this should be overwritten...
//
//////////////////////////////////////////////////////////////////////

#ifndef __GA2GENE_H__
#define __GA2GENE_H__

/**
 * \typedef typedef float ga2Gene
 * A gene is represented with a float; it is not necessary to change this
 * to use integer or binary values, though if a chromosome is to be composed
 * entirely of integers or binary digits, then changing this may yield some
 * improvement.
 */
typedef uint64_t ga2Gene;

#endif
