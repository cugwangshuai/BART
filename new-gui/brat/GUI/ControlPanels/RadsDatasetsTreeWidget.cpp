/*
* This file is part of BRAT
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include "new-gui/brat/stdafx.h"


#include "new-gui/Common/QtUtils.h"
#include "DataModels/Workspaces/RadsDataset.h"
#include "DataModels/Workspaces/Workspace.h"
#include "ApplicationPaths.h"

#include "RadsDatasetsTreeWidget.h"


Q_DECLARE_METATYPE( CRadsDataset* )

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget Items Class
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////



CRadsDatasetsTreeWidgetItem::CRadsDatasetsTreeWidgetItem( const std::vector< CRadsMission > &missions, CRadsDataset *dataset, QTreeWidget *view )
	: qobject_base_t( view )
	, base_t( view )
{
	assert__( view );

	view->blockSignals( true );

	// I. Datasets column

	const QString dataset_name = dataset->GetName().c_str();

	setText(0, dataset_name );
	setToolTip(0, dataset_name );
	setIcon( 0, QIcon( ":/images/OSGeo/dataset.png" ) );
	setFlags( flags() | Qt::ItemIsEditable );
	setData( 0, Qt::UserRole, QVariant::fromValue( dataset ) );

	view->addTopLevelItem( this );		//this is necessary before what follows


	// II. Missions column


	// II. 1. Missions combo box

	mCombo = new QComboBox;

	// ...fill with missions

	auto const &selected_user_missions = dataset->Missions();
	int index = 0, selected_index = -1;
	for ( auto const &mission : missions )
	{
		mCombo->addItem(mission.mName.c_str());
		bool selected = std::find( selected_user_missions.begin(), selected_user_missions.end(), mission ) != selected_user_missions.end();
		if ( selected )
			selected_index = index;
		index++;
	}
	mCombo->setCurrentIndex( selected_index );

	// ...aesthetics

	QString sheet =
		"\
		QComboBox,										\
		QLineEdit,										\
		QListView {										\
			selection-background-color: rgb(0, 0, 0);	\
			selection-color: white;						\
			border: 1px black;							\
			border-radius: 5px;							\
			padding: 2px 2px;							\
			font: bold;									\
			}											\
		QComboBox::drop-down {							\
			height: 12px;								\
			subcontrol-origin: padding;					\
			subcontrol-position: center right;			\
			background: none;							\
		}												\
		";
//width: 11px;								\

	mCombo->setStyleSheet( sheet );
	mCombo->setSizeAdjustPolicy( QComboBox::AdjustToContents );
	mCombo->view()->setMinimumHeight( missions.size() * ( mCombo->fontMetrics().lineSpacing() + 1 ) );

	//...align center
	mCombo->setEditable(true);				//otherwise there is no edit widget
	mCombo->lineEdit()->setReadOnly(true);
	mCombo->lineEdit()->setAlignment( Qt::AlignCenter );
	for( int i = 0; i < mCombo->count(); i++ )
		mCombo->setItemData( i, Qt::AlignCenter, Qt::TextAlignmentRole );

	//...connect embedded combo signal
	connect( mCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( currentIndexChanged( int ) ) );

	//...allow parent view to ignore wheel events
	mCombo->installEventFilter( view );
	mCombo->setFocusPolicy( Qt::StrongFocus );
	mCombo->lineEdit()->installEventFilter( view );


	// II. 2. Put missions combo in column 1

	mComboFrame = new QFrame;
	LayoutWidgets( Qt::Horizontal, { mCombo }, mComboFrame, 4, 4, 4, 4, 4 );
	view->setItemWidget( this, 1, mComboFrame );
	SetMissionToolTip();

	view->blockSignals( false );
}


//virtual 
bool CRadsDatasetsTreeWidgetItem::operator< ( const QTreeWidgetItem &o ) const
{
	if ( treeWidget()->sortColumn() == 0 )
		return base_t::operator< ( o );

	const auto &rads_o = dynamic_cast< const CRadsDatasetsTreeWidgetItem& >( o );

	const QString &s1 = mCombo->currentText();
	const QString &s2 = rads_o.mCombo->currentText();

	return s1 < s2;
}


void CRadsDatasetsTreeWidgetItem::SetMissionToolTip()
{
	QString tip;
	const CRadsDataset *rads_dataset = data( 0, Qt::UserRole ).value< CRadsDataset* >();
	if ( rads_dataset )
	{
		const QString mission_name = CurrentMission();
		if ( rads_dataset->HasMission( q2a( mission_name ) ) )
		{
			if ( rads_dataset->IsEmpty() )
			{
				tip = "No files found for mission " + mission_name;
			}
			else
			{
				tip = n2q( rads_dataset->Size() ) + " files found for mission " + mission_name;
			}
		}
	}
	SetToolTip( 1, tip );
}


void CRadsDatasetsTreeWidgetItem::SetToolTip( int column, const QString &atoolTip )
{
	setToolTip( column, atoolTip );
	setData( column, Qt::ToolTipRole, atoolTip );

	if ( column == 1 )
	{
		mCombo->setToolTip( atoolTip );
		mCombo->lineEdit()->setToolTip( atoolTip );
		mComboFrame->setToolTip( atoolTip );
	}
}


//slot
void CRadsDatasetsTreeWidgetItem::currentIndexChanged( int index )
{
	// Cannot SetMissionToolTip here, because only answering this signal
	//	will the dataset be assigned with the mission

	emit itemChanged( this, index );
}





/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										CRadsDatasetsTreeWidget
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Assumes a checked and valid all_available_missions
//
CRadsDatasetsTreeWidget::CRadsDatasetsTreeWidget( const std::vector< CRadsMission > &all_available_missions, QWidget *parent )	//parent = nullptr 
	: base_t( parent )
	, mAllAvailableMissions( all_available_missions )
{
    setHeaderLabels( QStringList() << tr("Dataset") << tr("Mission") );
    setHeaderHidden( false );
	header()->setStretchLastSection( true );
	header()->setSectionResizeMode( QHeaderView::Interactive );
	header()->setDefaultAlignment( Qt::AlignCenter );

	//setRootIsDecorated( false );	the problem with this: the highest level items have no node icon, can only be expanded/collapsed by double-clicking
	setDragEnabled( false );

	setSortingEnabled( true );

	mSortAscending = AddMenuAction( "Sort &Ascending" );
	mSortDescending = AddMenuAction( "Sort &Descending" );

    connect( mSortAscending,		SIGNAL(triggered()),	this, SLOT( HandleSortAscending() ) );
    connect( mSortDescending,		SIGNAL(triggered()),	this, SLOT( HandleSortDescending() ) );
}


QTreeWidgetItem* CRadsDatasetsTreeWidget::AddDatasetToTree( const QString &dataset_name )
{
	assert__( mWDataset );

	CRadsDataset *wkspcd = mWDataset->GetDataset< CRadsDataset >( q2a( dataset_name ) );			assert__( wkspcd );

	CRadsDatasetsTreeWidgetItem *dataset_item = new CRadsDatasetsTreeWidgetItem( mAllAvailableMissions, wkspcd, this );
	connect( dataset_item, &CRadsDatasetsTreeWidgetItem::itemChanged, this, &CRadsDatasetsTreeWidget::currentIndexChanged );	//specialized RADS tree item signal, from missions combo
    
	resizeColumnToContents( 0 );

	return dataset_item;
}


// Tree item events to be processed by the container tree
//
bool CRadsDatasetsTreeWidget::eventFilter( QObject *o, QEvent *e ) 
{
	if ( qobject_cast< QComboBox* >( o ) )
	{
		switch( e->type() )
		{
			// Prevent mouse wheel on combo

			case QEvent::Wheel:
				e->ignore();
				return true;
				break;

			default:            
				return false;
				break;
		}
	}
	else
	if ( qobject_cast< QLineEdit* >( o ) )
	{
		switch( e->type() )
		{
			// Prevent arrow keys on combo line edit

			case QEvent::KeyPress:
			case QEvent::KeyRelease:
				e->ignore();
				return true;
				break;

			// Allow selection by clicking in combo line edit

			case QEvent::MouseButtonPress:
			case QEvent::MouseButtonRelease:
				e->ignore();
				return true;
				break;

			default:            
				return false;
				break;
		}
	}

	return base_t::eventFilter( o, e );
}



void CRadsDatasetsTreeWidget::HandleSortAscending()
{
	sortByColumn( 0, Qt::AscendingOrder );
}


void CRadsDatasetsTreeWidget::HandleSortDescending()
{
	sortByColumn( 0, Qt::DescendingOrder );
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_RadsDatasetsTreeWidget.cpp"