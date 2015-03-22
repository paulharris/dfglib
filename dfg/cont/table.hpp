#pragma once

#include "../dfgDefs.hpp"
#include "../dfgAssert.hpp"
#include "../dfgBase.hpp"
#include "../str.hpp"
#include <algorithm>
#include <vector>
#include <numeric>
#include <map>
#include <deque>

DFG_ROOT_NS_BEGIN { DFG_SUB_NS(cont) {

    // Container for table which may have non equal row sizes.
    template <class Elem_T, class Index_T = size_t>
    class DFG_CLASS_NAME(Table)
    {
    public:
        static const auto s_nInvalidIndex = NumericTraits<Index_T>::maxValue;

        // Note: This was std::deque before. Clean up of deque-table constructed from ~50 MB file took tens-of-seconds/minutes
        //       while the same table with std::vector implementation cleaned up almost instantly.
        typedef std::vector<Elem_T> ElementContainerT;

        typedef typename ElementContainerT::iterator iterator;
        typedef typename ElementContainerT::const_iterator const_iterator;
        typedef Elem_T value_type;

        iterator begin() { return m_contElems.begin(); }
        const_iterator begin() const { return m_contElems.begin(); }
        const_iterator cbegin() const { return begin(); }

        iterator end() { return m_contElems.end(); }
        const_iterator end() const { return m_contElems.end(); }
        const_iterator cend() const { return end(); }

        void pushBackOnRow(Index_T nRow, Elem_T elem)
        {
            if (nRow == NumericTraits<Index_T>::maxValue) // Make sure that nRow + 1 won't overflow.
                return;

            if (!isValidIndex(m_rowSizes, nRow))
            {
                m_rowSizes.resize(nRow + 1, 0);
            }
            DFG_ASSERT_UB(isValidIndex(m_rowSizes, nRow));
            if (nRow + 1 == m_rowSizes.size()) // Adding element as the last item on the last row.
            {
                m_contElems.push_back(std::move(elem));
                m_rowSizes.back()++;
            }
            else
            {
                const auto nPos = privateGetLinearPosition(nRow + 1, 0);
                auto iterNewPos = m_contElems.begin() + nPos;
                m_contElems.insert(iterNewPos, std::move(elem));
                m_rowSizes[nRow]++;
            }
        }

        void setElement(Index_T nRow, Index_T nCol, Elem_T elem)
        {
            if (nRow == NumericTraits<Index_T>::maxValue || nCol == NumericTraits<Index_T>::maxValue) // Make sure that nRow + 1 nor nCol + 1 won't overflow.
                return;
            if (!isValidIndex(m_rowSizes, nRow))
                m_rowSizes.resize(nRow + 1, 0);
            if (nRow == m_rowSizes.size() - 1 && nCol == m_rowSizes[nRow]) // If new item is next on row, do pushBackOnRow.
                pushBackOnRow(nRow, std::move(elem));
            else // Case: not next item on row.
            {
                if (m_rowSizes[nRow] <= nCol)
                {
                    const auto nOldRowSize = m_rowSizes[nRow];
                    const auto nPos = privateGetLinearPositionVirtual(nRow, nOldRowSize);
                    const auto iterAddPos = m_contElems.begin() + nPos;
                    const auto nSizeInc = nCol - nOldRowSize + 1;
                    m_rowSizes[nRow] = nCol + 1;
                    m_contElems.insert(iterAddPos, nSizeInc, Elem_T());
                    m_contElems[nPos + nSizeInc - 1] = std::move(elem);
                }
                else // case: item already exists, replace.
                {
                    const auto nPos = privateGetLinearPosition(nRow, nCol);
                    m_contElems[nPos] = std::move(elem);
                }
            }
        }

        // If no element at (nRow, nCol) does not exist, returns s_nInvalidIndex.
        Index_T privateGetLinearPosition(Index_T nRow, Index_T nCol) const
        {
            if (!isValidTableIndex(nRow, nCol))
                return s_nInvalidIndex;
            const auto nPos = nCol + std::accumulate(m_rowSizes.cbegin(), m_rowSizes.cbegin() + nRow, Index_T(0));
            return nPos;
        }

        // Like privateGetLinearPosition, but if element at (nRow, nCol) does not exist, returns the index that given element would have if added to table.
        Index_T privateGetLinearPositionVirtual(Index_T nRow, Index_T nCol) const
        {
            const auto nPos = nCol + std::accumulate(m_rowSizes.cbegin(), m_rowSizes.cbegin() + nRow, Index_T(0));
            return nPos;
        }

        bool isValidTableIndex(Index_T row, Index_T col) const
        {
            return isValidIndex(m_rowSizes, row) && (col >= 0 && col < m_rowSizes[row]);
        }

        // requires
        //        - isValidTableIndex(row, col) == true
        const Elem_T& operator()(Index_T row, Index_T col) const
        {
            DFG_ASSERT_UB(isValidTableIndex(row, col));
            return m_contElems[privateGetLinearPosition(row, col)];
        }

        Index_T getColumnCountOnRow(Index_T nRow) const
        {
            return isValidIndex(m_rowSizes, nRow) ? m_rowSizes[nRow] : 0;
        }

        // Returns true iff. this is empty or all rows have equal size.
        bool hasSingleColumnCount() const
        {
            const auto nFirstRowSize = (!m_rowSizes.empty()) ? m_rowSizes[0] : 0;
            return m_rowSizes.empty() || std::all_of(m_rowSizes.cbegin(), m_rowSizes.cend(), [=](const Index_T nRowSize) {return nRowSize == nFirstRowSize; });
        }

        Index_T getCellCount() const
        {
            return m_contElems.size();
        }

        Index_T getRowCount() const
        {
            return m_rowSizes.size();
        }

        bool empty() const
        {
            return getCellCount() == 0;
        }

        void clear()
        {
            m_contElems.clear();
            m_rowSizes.clear();
        }

        // TODO: test
        // If there not enough elements to make the last row full, it will have size less than rowLength.
        template <class Iterable_T>
        void setElements(const Iterable_T& data, const Index_T rowLength)
        {
            if (rowLength <= 0)
            {
                clear();
                return;
            }
            m_contElems.assign(data.cbegin(), data.cend());
            const auto countOfFullRows = m_contElems.size() / rowLength;
            m_rowSizes.assign(countOfFullRows, rowLength);
            const auto excessElems = m_contElems.size() % rowLength;
            if (excessElems > 0)
                m_rowSizes.push_back(excessElems);
            DFG_ASSERT(std::accumulate(m_rowSizes.cbegin(), m_rowSizes.cend(), size_t(0)) == m_contElems.size());
        }

        ElementContainerT m_contElems;
        std::vector<Index_T> m_rowSizes;
    };

    // Class for efficiently storing big table of small strings with no embedded nulls.
    template <class Char_T, class Index_T = uint32>
    class DFG_CLASS_NAME(TableSz)
    {
    public:
        typedef std::pair<Index_T, const Char_T*> IndexPtrPair;

        DFG_CLASS_NAME(TableSz)() : m_nBlockSize(2048)
        {
        }

        void setBlockSize(size_t nBlockSize)
        {
            m_nBlockSize = nBlockSize;
        }

        template <class Str_T>
        bool setElement(size_t nRow, size_t nCol, const Str_T& sSrc)
        {
            return addString(DFG_MODULE_NS(str)::toCstr(sSrc), DFG_MODULE_NS(str)::strLen(sSrc), static_cast<Index_T>(nRow), static_cast<Index_T>(nCol));
        }

        // Sets element to (nRow, nCol). This does not invalidate any previous pointers return by operator()(), but after this call all pointers returned for (nRow, nCol)
        // will point to the previous element, not current.
        // If element at (nRow, nCol) already exists, it is overwritten.
        // Note: Even in case of overwrite, previous item is not cleared from string storage (this is implementation detail that is not part of the interface, i.e. it is not to be relied on).
        bool addString(const Char_T* const psz, const size_t nLength, const Index_T nRow, const Index_T nCol)
        {
            auto& bufferCont = m_charBuffers[nCol];
            if (bufferCont.empty() || bufferCont.back().capacity() - bufferCont.back().size() < nLength + 1)
            {
                bufferCont.push_back(std::vector<Char_T>());
                bufferCont.back().reserve(m_nBlockSize);
            }

            auto& currentBuffer = bufferCont.back();
            const auto nBeginIndex = currentBuffer.size();

            const auto nFreeSpace = currentBuffer.capacity() - currentBuffer.size();
            // Make sure that there's enough space in the buffer.
            // This should trigger only if nLength > m_nBlockSize.
            // Note that reallocation is not allowed because it would invalidate existing data pointers.
            if (nLength >= nFreeSpace)
                return false;
            currentBuffer.insert(currentBuffer.end(), psz, psz + nLength + 1);
            const Char_T* const pData = &currentBuffer[nBeginIndex];

            if (!isValidIndex(m_colToRows, nCol))
                m_colToRows.resize(nCol + 1);
            auto& rowsInCol = m_colToRows[nCol];

            rowsInCol.push_back(IndexPtrPair(nRow, pData));

            // Sort the row vector. Since the pre-add vector should have been sorted already, only the
            // last item needs to be moved to the right position.
            if (rowsInCol.size() > 1)
            {
                for (size_t i = rowsInCol.size() - 1; i >= 1; --i)
                {
                    if (rowsInCol[i].first < rowsInCol[i - 1].first)
                        std::swap(rowsInCol[i], rowsInCol[i - 1]);
                    else  // case: previous is less than or equal to newly added item.
                    {
                        if (rowsInCol[i].first == rowsInCol[i - 1].first) // If duplicate, erase previous.
                        {
                            // Note: the actual data is not cleared from char buffer.
                            rowsInCol.erase(rowsInCol.begin() + (i - 1));
                        }
                        break;
                    }
                }
            }

            return true;
        }

        template <class Str_T>
        bool addString(const Str_T& s, const Index_T nRow, const Index_T nCol)
        {
            return addString(DFG_MODULE_NS(str)::toCstr(s), DFG_MODULE_NS(str)::strLen(s), nRow, nCol);
        }

        template <class Func_T>
        void forEachFwdColumnIndex(Func_T&& func)
        {
            for (auto iter = m_charBuffers.cbegin(), iterEnd = m_charBuffers.cend(); iter != iterEnd; ++iter)
                func(iter->first);
        }

        // Functor is given two parameters: row index and null terminated string for cell in (row, nCol).
        template <class Func_T>
        void forEachFwdRowInColumn(const Index_T nCol, Func_T&& func)
        {
            if (!isValidIndex(m_colToRows, nCol))
                return;

            for (auto iter = m_colToRows[nCol].begin(), iterEnd = m_colToRows[nCol].end(); iter != iterEnd; ++iter)
                func(iter->first, iter->second);
        }

        // const-overload.
        template <class Func_T>
        void forEachFwdRowInColumn(const Index_T nCol, Func_T&& func) const
        {
            if (!isValidIndex(m_colToRows, nCol))
                return;

            for (auto iter = m_colToRows[nCol].begin(), iterEnd = m_colToRows[nCol].end(); iter != iterEnd; ++iter)
                func(iter->first, iter->second);
        }

        // Returns either pointer to null terminated string or nullptr, if no element exists.
        // Note: Returned pointer remains valid even if adding new strings. For behaviour in case of 
        //       overwriting item at (row, col), see documentation for AddString.
        const Char_T* operator()(Index_T row, Index_T col) const
        {
            if (!isValidIndex(m_colToRows, col))
                return nullptr;
            const auto& colToRowCont = m_colToRows[col];
            const IndexPtrPair searchItem(row, nullptr);
            const auto pred = [](const IndexPtrPair& a, const IndexPtrPair& b) {return a.first < b.first; };
            auto iter = std::lower_bound(colToRowCont.begin(), colToRowCont.end(), searchItem, pred);
            return (iter != colToRowCont.end() && iter->first == row) ? iter->second : nullptr;
        }

        /*
        Storage implementation:
            -m_charBuffers stores null terminated strings in blocks of contiguous memory for each column.
            -m_colToRows[nCol] gives list of (row,psz) pairs ordered by row in column nCol.
            If table has cell at (row,col), it can be accessed by finding row from m_colToRows[nCol].
            Since m_colToRows[nCol] is ordered by row, it can be searched with binary search.
        */
        std::map<Index_T, std::deque<std::vector<char>>> m_charBuffers;
        std::vector<std::vector<IndexPtrPair>> m_colToRows;
        size_t m_nBlockSize;
    };

}} // module cont