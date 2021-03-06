/*
!@
MIT License

Copyright (c) 2020 Skylicht Technology CO., LTD

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files
(the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This file is part of the "Skylicht Engine".
https://github.com/skylicht-lab/skylicht-engine
!#
*/

#pragma once

#include "CXMLColumn.h"
#include "CXMLSpreadsheet.h"
#include "Serializable/CValueProperty.h"

namespace Skylicht
{
	class CXMLTableData
	{
	protected:
		std::vector<CXMLColumn*> m_column;

		CXMLSpreadsheet::SSheet *m_sheet;

	public:
		CXMLTableData(CXMLSpreadsheet::SSheet *sheet);

		virtual ~CXMLTableData();

		void addColumn(EPropertyDataType type, const char *name);

		void insertColumn(EPropertyDataType type, const char *name, u32 position);

		void removeColumn(CXMLColumn *col);

		void removeColumn(u32 index);

		inline CXMLColumn* getColumn(u32 index)
		{
			return m_column[index];
		}

		u32 getNumColumn()
		{
			return m_column.size();
		}

		u32 getNumRow()
		{
			return m_sheet->Rows.size();
		}

		template<class T>
		void freeData(std::vector<T*>& data)
		{
			for (T *i : data)
				delete i;
			data.clear();
		}

		template<class T>
		u32 fetchData(std::vector<T*>& data, u32 fromRow, int count)
		{
			std::list<CXMLSpreadsheet::SRow*>::iterator i = m_sheet->Rows.begin(), end = m_sheet->Rows.end();
			while (i != end)
			{
				CXMLSpreadsheet::SRow* row = (*i);
				if (row->Index < fromRow)
				{
					++i;
					continue;
				}

				if (count > 0)
				{
					if (row->Index >= fromRow + (u32)count)
						break;
				}

				T *obj = new T();
				CObjectSerizable *objectSerizable = dynamic_cast<CObjectSerizable*>(obj);
				if (objectSerizable == NULL)
				{
					delete obj;
					return 0;
				}

				std::list<CXMLSpreadsheet::SCell*>::iterator j = row->Cells.begin(), e = row->Cells.end();
				while (j != e)
				{
					CXMLSpreadsheet::SCell* cell = (*j);
					if (cell->Col < m_column.size())
					{
						CXMLColumn *col = m_column[cell->Col];
						{
							CValueProperty *value = objectSerizable->getProperty(col->getName().c_str());
							if (value == NULL)
								continue;

							switch (value->getType())
							{
							case String:
								((CStringProperty*)value)->setValue(cell->Value.c_str());
								break;
							case Integer:
								((CIntProperty*)value)->setValue(cell->NumberInt);
								break;
							case Float:
								((CFloatProperty*)value)->setValue(cell->NumberFloat);
								break;
							case DateTime:
								((CDateTimeProperty*)value)->setValue(cell->Time);
								break;
							default:
								break;
							}
						}
					}

					++j;
				}

				data.push_back(obj);

				++i;
			}

			return data.size();
		}
	};
}