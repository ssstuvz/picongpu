/**
 * Copyright 2013 Felix Schmitt, Heiko Burau, Rene Widera
 *
 * This file is part of libPMacc.
 *
 * libPMacc is free software: you can redistribute it and/or modify
 * it under the terms of of either the GNU General Public License or
 * the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libPMacc is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License and the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * and the GNU Lesser General Public License along with libPMacc.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STRIDEMAPPING_H
#define	STRIDEMAPPING_H

#include "types.h"
#include <stdexcept>
#include "dimensions/DataSpace.hpp"
#include "mappings/kernel/StrideMappingMethods.hpp"
#include "dimensions/DataSpaceOperations.hpp"

namespace PMacc
{

template<uint32_t areaType, uint32_t stride, class baseClass>
class StrideMapping;

template<
uint32_t areaType,
uint32_t stride,
template<unsigned, class> class baseClass,
unsigned DIM,
class SuperCellSize_
>
class StrideMapping<areaType, stride, baseClass<DIM, SuperCellSize_> > : public baseClass<DIM, SuperCellSize_>
{
public:
    typedef baseClass<DIM, SuperCellSize_> BaseClass;

    enum
    {
        AreaType = areaType, Dim = BaseClass::Dim, Stride = stride
    };


    typedef typename BaseClass::SuperCellSize SuperCellSize;

    HINLINE StrideMapping(BaseClass base) : BaseClass(base), offset()
    {
    }

    /**
     * Generates cuda gridDim information for kernel call.
     *
     * @return dim3 with gridDim information
     */
    HINLINE DataSpace<DIM> getGridDim()
    {
        return this->reduce((StrideMappingMethods<areaType, DIM>::getGridDim(*this) - offset + (int)Stride - 1) / (int)Stride);
    }

    /**
     * Returns index of current logical block, depending on current cuda block id.
     *
     * @param _blockIdx current cuda block id (blockIdx)
     * @return current logical block index
     */
    DINLINE DataSpace<DIM> getSuperCellIndex(const DataSpace<DIM>& realSuperCellIdx)
    {
        const DataSpace<DIM> blockId((extend(realSuperCellIdx) * (int)Stride) + offset);
        return StrideMappingMethods<areaType, DIM>::shift(*this, blockId);
    }

    HDINLINE DataSpace<DIM> getOffset()
    {
        return offset;
    }

    HDINLINE void setOffset(const DataSpace<DIM> offset)
    {
        this->offset = offset;
    }

    HINLINE bool next()
    {
        int linearOffset = DataSpaceOperations<Dim>::map(DataSpace<DIM>::create(stride), offset);
        linearOffset++;
        offset = DataSpaceOperations<Dim>::map(DataSpace<DIM>::create(stride), linearOffset);

        return linearOffset < DataSpace<DIM>::create(stride).productOfComponents();
    }

private:
    PMACC_ALIGN(offset, DataSpace<DIM>);

};


} // namespace PMacc



#endif	/* STRIDEMAPPING_H */

