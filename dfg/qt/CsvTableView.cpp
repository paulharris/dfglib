#include "../buildConfig.hpp" // To get rid of C4996 "Function call with parameters that may be unsafe" in MSVC.
#include "qtIncludeHelpers.hpp"
#include "CsvTableView.hpp"
#include "CsvItemModel.hpp"
#include "CsvTableViewActions.hpp"

DFG_BEGIN_INCLUDE_QT_HEADERS
#include <QMenu>
#include <QFileDialog>
#include <QUndoStack>
#include <QHeaderView>
DFG_END_INCLUDE_QT_HEADERS

#include <set>
#include <unordered_set>
#include "../alg.hpp"

using namespace DFG_MODULE_NS(qt);

DFG_CLASS_NAME(CsvTableView)::DFG_CLASS_NAME(CsvTableView)(QWidget* pParent) : BaseClass(pParent)
{
	auto pVertHdr = verticalHeader();
	if (pVertHdr)
		pVertHdr->setDefaultSectionSize(20); // Default row height seems to be 30, which looks somewhat bloated. Make it smaller.

	{
		auto pAction = new QAction(tr("Move first row to header"), this);
		//pAction->setShortcut(tr(""));
		connect(pAction, &QAction::triggered, this, &ThisClass::moveFirstRowToHeader);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Move header to first row"), this);
		//pAction->setShortcut(tr(""));
		connect(pAction, &QAction::triggered, this, &ThisClass::moveHeaderToFirstRow);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Open file..."), this);
		connect(pAction, &QAction::triggered, this, &ThisClass::openFromFile);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Save to file..."), this);
		connect(pAction, &QAction::triggered, this, &ThisClass::saveToFile);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(this);
		pAction->setSeparator(true);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Clear selected cell(s)"), this);
		pAction->setShortcut(tr("Del"));
		connect(pAction, &QAction::triggered, this, &ThisClass::clearSelected);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Insert row here"), this);
		pAction->setShortcut(tr("Ins"));
		connect(pAction, &QAction::triggered, this, &ThisClass::insertRowHere);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Insert row after current"), this);
		pAction->setShortcut(tr("Shift+Ins"));
		connect(pAction, &QAction::triggered, this, &ThisClass::insertRowAfterCurrent);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Insert column"), this);
		pAction->setShortcut(tr("Ctrl+Alt+Ins"));
		connect(pAction, &QAction::triggered, this, &ThisClass::insertColumn);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Insert column after current"), this);
		pAction->setShortcut(tr("Ctrl+Alt+Shift+Ins"));
		connect(pAction, &QAction::triggered, this, &ThisClass::insertColumnAfterCurrent);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Delete selected row(s)"), this);
		pAction->setShortcut(tr("Shift+Del"));
		connect(pAction, &QAction::triggered, this, &ThisClass::deleteSelectedRow);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Delete current column"), this);
		pAction->setShortcut(tr("Ctrl+Del"));
		connect(pAction, SIGNAL(triggered()), this, SLOT(deleteCurrentColumn()));
		addAction(pAction);
	}

	{
		auto pAction = new QAction(this);
		pAction->setSeparator(true);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Invert selections"), this);
		pAction->setShortcut(tr("Ctrl+I"));
		connect(pAction, &QAction::triggered, this, &ThisClass::invertSelection);
		addAction(pAction);
	}

	/* Not Implemented
	{
		auto pAction = new QAction(tr("Move row up"), this);
		pAction->setShortcut(tr("Alt+Up"));
		connect(pAction, &QAction::triggered, this, &ThisClass::moveRowUp);
		addAction(pAction);
	}

	{
	auto pAction = new QAction(tr("Move row down"), this);
	pAction->setShortcut(tr("Alt+Down"));
	connect(pAction, &QAction::triggered, this, &ThisClass::moveRowDown);
	addAction(pAction);
	}
	*/

	{
		auto pAction = new QAction(this);
		pAction->setSeparator(true);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Cut"), this);
		pAction->setShortcut(tr("Ctrl+X"));
		connect(pAction, &QAction::triggered, this, &ThisClass::cut);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Copy"), this);
		pAction->setShortcut(tr("Ctrl+C"));
		connect(pAction, &QAction::triggered, this, &ThisClass::copy);
		addAction(pAction);
	}

	{
		auto pAction = new QAction(tr("Paste"), this);
		pAction->setShortcut(tr("Ctrl+V"));
		connect(pAction, &QAction::triggered, this, &ThisClass::paste);
		addAction(pAction);
	}
}

void DFG_CLASS_NAME(CsvTableView)::contextMenuEvent(QContextMenuEvent* pEvent)
{
	DFG_UNUSED(pEvent);

	QMenu menu;
	menu.addActions(actions());
	menu.exec(QCursor::pos());

	//BaseClass::contextMenuEvent(pEvent);
	/*
	QMenu menu;

	auto actionDelete_current_column = new QAction(this);
	actionDelete_current_column->setObjectName(QStringLiteral("actionDelete_current_column"));
	auto actionRename_column = new QAction(this);
	actionRename_column->setObjectName(QStringLiteral("actionRename_column"));
	auto actionMove_column_left = new QAction(this);
	actionMove_column_left->setObjectName(QStringLiteral("actionMove_column_left"));
	auto actionMove_column_right = new QAction(this);
	actionMove_column_right->setObjectName(QStringLiteral("actionMove_column_right"));
	auto actionCopy_column = new QAction(this);
	actionCopy_column->setObjectName(QStringLiteral("actionCopy_column"));
	auto actionPaste_column = new QAction(this);
	actionPaste_column->setObjectName(QStringLiteral("actionPaste_column"));
	auto actionDelete_selected_row_s = new QAction(this);
	actionDelete_selected_row_s->setObjectName(QStringLiteral("actionDelete_selected_row_s"));

	actionRename_column->setText(QApplication::translate("MainWindow", "Rename column", 0, QApplication::UnicodeUTF8));
	actionRename_column->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0, QApplication::UnicodeUTF8));
	actionMove_column_left->setText(QApplication::translate("MainWindow", "Move column left", 0, QApplication::UnicodeUTF8));
	actionMove_column_left->setShortcut(QApplication::translate("MainWindow", "Alt+Left", 0, QApplication::UnicodeUTF8));
	actionMove_column_right->setText(QApplication::translate("MainWindow", "Move column right", 0, QApplication::UnicodeUTF8));
	actionMove_column_right->setShortcut(QApplication::translate("MainWindow", "Alt+Right", 0, QApplication::UnicodeUTF8));
	actionCopy_column->setText(QApplication::translate("MainWindow", "Copy column", 0, QApplication::UnicodeUTF8));
	actionCopy_column->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0, QApplication::UnicodeUTF8));
	actionPaste_column->setText(QApplication::translate("MainWindow", "Paste column", 0, QApplication::UnicodeUTF8));
	actionPaste_column->setShortcut(QApplication::translate("MainWindow", "Ctrl+Shift+V", 0, QApplication::UnicodeUTF8));

	menu.addAction();

	menu.exec(QCursor::pos());
	*/
}

void DFG_CLASS_NAME(CsvTableView)::setModel(QAbstractItemModel* pModel)
{
	DFG_ASSERT_WITH_MSG(dynamic_cast<QAbstractProxyModel*>(pModel) == nullptr, "View currently does not work with proxy models.");
	BaseClass::setModel(pModel);
}

DFG_CLASS_NAME(CsvItemModel)* DFG_CLASS_NAME(CsvTableView)::csvModel()
{
	auto pModel = model();
	auto pCsvModel = dynamic_cast<CsvModel*>(pModel);
	if (pCsvModel)
		return pCsvModel;
	auto pProxyModel = dynamic_cast<QAbstractProxyModel*>(pModel);
	return (pProxyModel) ? dynamic_cast<CsvModel*>(pProxyModel->sourceModel()) : nullptr;
}

int DFG_CLASS_NAME(CsvTableView)::getFirstSelectedViewRow()
{
	const auto& contItems = getRowsOfSelectedItems(nullptr);
	return (!contItems.empty()) ? *contItems.begin() : model()->rowCount();
}

std::vector<int> DFG_CLASS_NAME(CsvTableView)::getRowsOfCol(const int nCol, QAbstractProxyModel* pProxy)
{
	std::vector<int> vec(model()->rowCount());
	if (!pProxy)
	{
		DFG_MODULE_NS(alg)::generateAdjacent(vec, 0, 1);
	}
	else
	{
		for (int i = 0; i<int(vec.size()); ++i)
			vec[i] = pProxy->mapToSource(pProxy->index(i, nCol)).row();
	}
	return vec;
}

QModelIndexList DFG_CLASS_NAME(CsvTableView)::getSelectedItemIndexes(QAbstractProxyModel* pProxy) const
{
	QModelIndexList listSelected = selectedIndexes();
	if (pProxy)
	{
		for (int i = 0; i<listSelected.size(); ++i)
			listSelected[i] = pProxy->mapToSource(listSelected[i]);
	}
	return listSelected;
}

std::vector<int> DFG_CLASS_NAME(CsvTableView)::getRowsOfSelectedItems(QAbstractProxyModel* pProxy, const bool bSort) const
{
	QModelIndexList listSelected = getSelectedItemIndexes(pProxy);

	std::set<int> setRows;
	std::vector<int> vRows;
	for (QModelIndexList::const_iterator iter = listSelected.begin(); iter != listSelected.end(); ++iter)
	{
		if (iter->isValid())
		{
			if (setRows.find(iter->row()) != setRows.end())
				continue;
			setRows.insert(iter->row());
			vRows.push_back(iter->row());
		}
	}

	DFG_ASSERT(setRows.size() == vRows.size());
	if (bSort)
		std::copy(setRows.begin(), setRows.end(), vRows.begin());
	return vRows;
}

QModelIndex DFG_CLASS_NAME(CsvTableView)::getFirstSelectedItem(QAbstractProxyModel* pProxy)
{
	QModelIndexList listSelected = selectedIndexes();
	if (listSelected.empty())
		return QModelIndex();
	if (pProxy)
		return pProxy->mapToSource(listSelected[0]);
	else
		return listSelected[0];

}

void DFG_CLASS_NAME(CsvTableView)::invertSelection()
{
	const auto pModel = model();
	if (pModel == nullptr)
		return;
	for (int r = 0; r<pModel->rowCount(); ++r)
	{
		for (int c = 0; c<pModel->columnCount(); ++c)
		{
			QModelIndex index = pModel->index(r, c);
			selectionModel()->select(index, QItemSelectionModel::Toggle);
		}
	}
}

bool DFG_CLASS_NAME(CsvTableView)::isRowMode() const
{
	return false;
}

bool DFG_CLASS_NAME(CsvTableView)::saveToFile()
{
	auto sPath = QFileDialog::getSaveFileName(this,
											tr("Open file"),
											QString()/*dir*/,
											tr("CSV files (*.csv);;All files (*.*)"),
											nullptr/*selected filter*/,
											0/*options*/);

	if (sPath.isEmpty())
		return false;

	auto pModel = csvModel();

	if (!pModel)
		return false;

	const auto bSuccess = pModel->saveToFile(sPath);
	if (!bSuccess)
		QMessageBox::information(nullptr, tr("Save failed"), tr("Failed to save to path %1").arg(sPath));

	return bSuccess;
}

bool DFG_CLASS_NAME(CsvTableView)::openFromFile()
{
	auto sPath = QFileDialog::getOpenFileName(this,
		tr("Open file"),
		QString()/*dir*/,
		tr("CSV files (*.csv);;All files (*.*)"),
												nullptr/*selected filter*/,
												0/*options*/);
	if (sPath.isEmpty())
		return false;

	auto pModel = csvModel();
	if (pModel)
	{
		const auto bSuccess = pModel->openFile(sPath);
		if (!bSuccess)
			QMessageBox::information(nullptr, "", QString("Failed to open file '%1'").arg(sPath));
		return bSuccess;
	}
	else
		return false;
}

template <class T, class Param0_T>
bool DFG_CLASS_NAME(CsvTableView)::executeAction(Param0_T&& p0)
{
	if (m_spUndoStack)
	{
		QUndoCommand* insertCommand = new T(std::forward<Param0_T>(p0));
		m_spUndoStack->push(insertCommand);
	}
	else
		T(std::forward<Param0_T>(p0)).redo();

	return true;
}

template <class T, class Param0_T, class Param1_T>
bool DFG_CLASS_NAME(CsvTableView)::executeAction(Param0_T&& p0, Param1_T&& p1)
{
	if (m_spUndoStack)
	{
		QUndoCommand* insertCommand = new T(std::forward<Param0_T>(p0), std::forward<Param1_T>(p1));
		m_spUndoStack->push(insertCommand);
	}
	else
		T(std::forward<Param0_T>(p0), std::forward<Param1_T>(p1)).redo();

	return true;
}

template <class T, class Param0_T, class Param1_T, class Param2_T>
bool DFG_CLASS_NAME(CsvTableView)::executeAction(Param0_T&& p0, Param1_T&& p1, Param2_T&& p2)
{
	if (m_spUndoStack)
	{
		QUndoCommand* insertCommand = new T(std::forward<Param0_T>(p0), std::forward<Param1_T>(p1), std::forward<Param2_T>(p2));
		m_spUndoStack->push(insertCommand);
	}
	else
		T(std::forward<Param0_T>(p0), std::forward<Param1_T>(p1), std::forward<Param2_T>(p2)).redo();

	return true;
}

bool DFG_CLASS_NAME(CsvTableView)::clearSelected()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionDelete)>(*this, getProxyModelPtr(), false /*false = not row mode*/);
}

bool DFG_CLASS_NAME(CsvTableView)::insertRowHere()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionInsertRow)>(this, DFG_SUB_NS_NAME(undoCommands)::InsertRowTypeBefore);
}

bool DFG_CLASS_NAME(CsvTableView)::insertRowAfterCurrent()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionInsertRow)>(this, DFG_SUB_NS_NAME(undoCommands)::InsertRowTypeAfter);
}

bool DFG_CLASS_NAME(CsvTableView)::insertColumn()
{
	const auto nCol = currentIndex().column();
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionInsertColumn)>(csvModel(), nCol);
}

bool DFG_CLASS_NAME(CsvTableView)::insertColumnAfterCurrent()
{
	const auto nCol = currentIndex().column();
	if (nCol >= 0)
		return executeAction<DFG_CLASS_NAME(CsvTableViewActionInsertColumn)>(csvModel(), nCol + 1);
	else
		return false;
}

bool DFG_CLASS_NAME(CsvTableView)::cut()
{
	copy();
	clearSelected();
	return true;
}

bool DFG_CLASS_NAME(CsvTableView)::copy()
{
	auto vViewRows = getRowsOfSelectedItems(nullptr, false);
	auto vRows = getRowsOfSelectedItems(m_spProxyModel.get(), false);
	auto pModel = csvModel();
	if (vRows.empty() || !pModel)
		return false;
	QString str;
	// Not sorting because it's probably more intuitive to get
	// items in that order in which they were shown.
	//std::sort(vRows.begin(), vRows.end());
	//std::sort(vViewRows.begin(), vViewRows.end());
	DFG_ASSERT(vViewRows.size() == vRows.size());
	QItemSelection selection;
	const bool bRowMode = isRowMode();
	for (size_t i = 0; i<vRows.size(); ++i)
	{
		if (!bRowMode)
		{
			std::unordered_set<int> setIgnoreCols;
			QAbstractItemModel* pEffectiveModel = getProxyModelPtr();
			if (pEffectiveModel == nullptr)
				pEffectiveModel = pModel;

			for (int col = 0; col < pModel->columnCount(); ++col)
			{
				if (!selectionModel()->isSelected(pEffectiveModel->index(vViewRows[i], col)))
					setIgnoreCols.insert(col);
				else
					selection.select(pModel->index(vViewRows[i], col), pModel->index(vViewRows[i], col));
			}
			pModel->rowToString(vRows[i], str, '\t', &setIgnoreCols);
		}
		else
		{
			pModel->rowToString(vRows[i], str, '\t');
			selection.select(pModel->index(vViewRows[i], 0), pModel->index(vViewRows[i], 0));
		}
		str.push_back('\n');

	}
	if (bRowMode)
		selectionModel()->select(selection, QItemSelectionModel::Select | QItemSelectionModel::Rows);
	else
		selectionModel()->select(selection, QItemSelectionModel::Select);

	QApplication::clipboard()->setText(str);

	return true;
}

bool DFG_CLASS_NAME(CsvTableView)::paste()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionPaste)>(this);
}

bool DFG_CLASS_NAME(CsvTableView)::deleteCurrentColumn()
{
	const auto curIndex = currentIndex();
	const int nRow = curIndex.row();
	const int nCol = curIndex.column();
	const auto rv = deleteCurrentColumn(nCol);
	selectionModel()->setCurrentIndex(model()->index(nRow, nCol), QItemSelectionModel::NoUpdate);
	return rv;
}

bool DFG_CLASS_NAME(CsvTableView)::deleteCurrentColumn(const int nCol)
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionDeleteColumn)>(this, nCol);
}

bool DFG_CLASS_NAME(CsvTableView)::deleteSelectedRow()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionDelete)>(*this, getProxyModelPtr(), true /*row mode*/);
}

bool DFG_CLASS_NAME(CsvTableView)::moveFirstRowToHeader()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionMoveFirstRowToHeader)>(this);
}

bool DFG_CLASS_NAME(CsvTableView)::moveHeaderToFirstRow()
{
	return executeAction<DFG_CLASS_NAME(CsvTableViewActionMoveFirstRowToHeader)>(this, true);
}