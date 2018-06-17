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
// ga2.h: a bunch of global constants. probably should not be touched.
//
//////////////////////////////////////////////////////////////////////

#ifndef __GA2_H__
#define __GA2_H__

#define GA2_CROSSOVER_ONEPOINT 1
#define GA2_CROSSOVER_UNIFORM 2

#define GA2_REPLACE_GENERATIONAL 1
#define GA2_REPLACE_STEADYSTATE 2
#define GA2_REPLACE_STEADYSTATENODUPLICATES 3

#define GA2_SELECT_ROULETTE 1
#define GA2_SELECT_RANKED 2

#include "ga2Gene.h"
#include "ga2Chromosome.h"
#include "ga2Population.h"

#endif //__GA2_H__
