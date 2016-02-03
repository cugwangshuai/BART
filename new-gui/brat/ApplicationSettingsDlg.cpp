#include "new-gui/brat/stdafx.h"

#include "new-gui/Common/QtUtils.h"
#include "new-gui/brat/GUI/ActionsTable.h"
#include "BratApplication.h"
#include "BratSettings.h"
#include "ApplicationSettingsDlg.h"

/*
Receives a CBratSettings ref, wich includes a CApplicationSettings ref as well.
mSettings is set by copy from options.
*/
CApplicationSettingsDlg::CApplicationSettingsDlg( CBratSettings &options, QWidget *parent ) : QDialog( parent ), mSettings( options )
{
    //Add sub widgets and additional contents.
	setupUi( this );
	//QIcon icon;
	//icon.addFile( QString::fromUtf8( ":/settings.png" ), QSize(), QIcon::Normal, QIcon::Off );
	//setWindowIcon( icon );

	// *** Page Selector ***

	static const size_t iwidth = 48, iheight = iwidth, spacing = 12;

	Contents_ListWidget->setViewMode( QListView::IconMode );
	Contents_ListWidget->setIconSize( QSize( iwidth, iheight ) );
	Contents_ListWidget->setMovement( QListView::Static );
	Contents_ListWidget->setMaximumWidth( iwidth + 2*spacing + 10 );	//don't change, empirically found
	Contents_ListWidget->setSpacing( spacing );
	createIcons();
	connect( 
		Contents_ListWidget,SIGNAL( currentItemChanged( QListWidgetItem*, QListWidgetItem* ) ),
		this, SLOT( changePage( QListWidgetItem*, QListWidgetItem* ) ) );
	Contents_ListWidget->setCurrentRow( 0 );


	// *** Pages ***

	//	ApplicationPaths_page

	//DataDirectory_lineEdit->setText( m_bs.GetDataPath() );
 //   ExternalDataDirectory_lineEdit->setText( m_bs.GetLibraryPath() );
 //   ProjectsDirectory_lineEdit->setText( m_bs.GetProjectsPath() );

    // Signal and slots:
    // DataDirectory_lineEdit reacts to signal textChanged with callback
    // 'UpdateDirectoriesActions'
    connect( DataDirectory_lineEdit, SIGNAL( textChanged(const QString&) ), this, SLOT( UpdateDirectoriesActions(const QString&) ) );

	//AutoRelativePaths_checkBox->setChecked( 
	//	( m_bs.GetLibraryPath() == m_bs.GetDataPath() + m_bs.DefaultExternalDataSubDir() ) &&
	//	m_bs.GetProjectsPath() == m_bs.GetDataPath() + m_bs.DefaultProjectsSubDir()
	//);
	on_AutoRelativePaths_checkBox_clicked( AutoRelativePaths_checkBox->isChecked() );


	//	StartupOptions_page

	LoadLastProjectAtAtartup_checkBox->setChecked( mSettings.mLoadLastWorkspaceAtStartUp );

#if defined(DEBUG) || defined(_DEBUG)
	mDesktopManagerSdiCheckbox = new QCheckBox( "Single Document Interface desktop manager" );
	mStartupOptions_groupBox->layout()->addWidget( mDesktopManagerSdiCheckbox );
	mDesktopManagerSdiCheckbox->setChecked( mSettings.mDesktopManagerSdi );
#endif


    //	ApplicationStyles_page - irrelevant for now.

	Styles_listWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	const std::vector< std::string > &styles = CBratSettings::getStyles();
	fillList( Styles_listWidget, styles, (int)CBratSettings::getStyleIndex( mSettings.mAppStyle ), true );

	StyleSheets_listWidget->setSelectionMode( QAbstractItemView::SingleSelection );
	const std::vector< std::string > &style_sheets = CBratSettings::getStyleSheets( false );
	fillList( StyleSheets_listWidget, style_sheets, mSettings.mCustomAppStyleSheet, true );

    DefaultStyle_checkBox->setChecked( mSettings.mUseDefaultStyle );
	NoStyleSheet_checkBox->setChecked( mSettings.mNoStyleSheet );

	Styles_listWidget->setDisabled( DefaultStyle_checkBox->isChecked() );
	StyleSheets_listWidget->setDisabled( NoStyleSheet_checkBox->isChecked() );
	connect( DefaultStyle_checkBox, SIGNAL( clicked( bool ) ), Styles_listWidget, SLOT( setDisabled( bool ) ) );
	connect( NoStyleSheet_checkBox, SIGNAL( clicked( bool ) ), StyleSheets_listWidget, SLOT( setDisabled( bool ) ) );

	StyleSheets_listWidget->item( e_DarkStyle )->setToolTip(" recommended with QCleanlooksStyle " );
	StyleSheets_listWidget->item( e_DarkOrangeStyle )->setToolTip(" recommended with QPlastiqueStyle " );
}


///////////////////////////////////////////////////////////////////////////////////
//
//							*** Page Selector ***
//
///////////////////////////////////////////////////////////////////////////////////

void CApplicationSettingsDlg::createIcons()
{
	// ApplicationPaths_page
	QAction *a = CActionInfo::CreateAction( this, eAction_ApplicationPaths_page );
	QListWidgetItem *configButton = new QListWidgetItem( Contents_ListWidget );
    configButton->setIcon( a->icon() );
	configButton->setText( a->text() );
	configButton->setToolTip( a->toolTip() );
	configButton->setTextAlignment( Qt::AlignHCenter );
	configButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	delete a;

	// StartupOptions_page
	a = CActionInfo::CreateAction( this, eAction_StartupOptions_page );
	QListWidgetItem *updateButton = new QListWidgetItem( Contents_ListWidget );
    updateButton->setIcon( a->icon() );
	updateButton->setText( a->text() );
	updateButton->setToolTip( a->toolTip() );
	updateButton->setTextAlignment( Qt::AlignHCenter );
	updateButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	delete a;

	// ApplicationStyles_page
	a = CActionInfo::CreateAction( this, eAction_ApplicationStyles_page );
	QListWidgetItem *queryButton = new QListWidgetItem( Contents_ListWidget );
    queryButton->setIcon( a->icon() );
	queryButton->setText( a->text() );
	queryButton->setToolTip( a->toolTip() );
	queryButton->setTextAlignment( Qt::AlignHCenter );
	queryButton->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
	delete a;
}

void CApplicationSettingsDlg::changePage( QListWidgetItem *current, QListWidgetItem *previous )
{
	if ( !current )
		current = previous;

	Pages_StackedWidget->setCurrentIndex( Contents_ListWidget->row( current ) );
}



///////////////////////////////////////////////////////////////////////////////////
//
//								*** Pages ***
//
///////////////////////////////////////////////////////////////////////////////////


//	ApplicationPaths_page


void CApplicationSettingsDlg::on_BrowseParseDataDirectory_pushButton_clicked()
{
    QString dir = BrowseDirectory( this, "Select BRAT Data Directory", DataDirectory_lineEdit->text() );
    if ( !dir.isEmpty() )
        DataDirectory_lineEdit->setText( dir );
}

void CApplicationSettingsDlg::on_Browse_ExternalDataPath_pushButton_clicked()
{
    QString dir = BrowseDirectory( this, "Select External Data Directory", ExternalDataDirectory_lineEdit->text() );
    if ( !dir.isEmpty() )
        ExternalDataDirectory_lineEdit->setText( dir );
}

void CApplicationSettingsDlg::on_Browse_ProjectsPath_pushButton_clicked()
{
    QString dir = BrowseDirectory( this, "Select Workspaces Directory", ProjectsDirectory_lineEdit->text() );
    if ( !dir.isEmpty() )
        ProjectsDirectory_lineEdit->setText( dir );
}

//This is a callback. DataDirectory_LineEdit was updated for some reason.
void CApplicationSettingsDlg::UpdateDirectoriesActions(const QString & text)
{
    // Only do anything if AutoRelativePaths is checked as positive:
    // Which would make sense since we are only managing the default part.
	if ( AutoRelativePaths_checkBox->isChecked() )
	{
        // The text for ExternalDataDirectory_lineEdit will be text / DefaultExternalDataSubDir()
        ExternalDataDirectory_lineEdit->setText( text + t2q( mSettings.BratPaths().DefaultExternalDataSubDir() ) );
        // ProjectsDirectory_lineEdit text will be text / DefaultProjectsSubdir()
        ProjectsDirectory_lineEdit->setText( text + t2q( mSettings.BratPaths().DefaultProjectsSubDir() ) );
	}
}

// Only enable other boxes if we don't ise AutoRelativePaths
void CApplicationSettingsDlg::on_AutoRelativePaths_checkBox_clicked( bool checked )
{
	if ( checked )
        UpdateDirectoriesActions(DataDirectory_lineEdit->text());

    ExternalDataDirectory_lineEdit->setEnabled( !checked );
    ProjectsDirectory_lineEdit->setEnabled( !checked );
    Browse_ExternalDataPath_pushButton->setEnabled( !checked );
    Browse_ProjectsPath_pushButton->setEnabled( !checked );
}


//	StartupOptions_page
//	ApplicationStyles_page



///////////////////////////////////////////////////////////////////////////////////
//
//						Validation and Conclusion
//
///////////////////////////////////////////////////////////////////////////////////



bool CApplicationSettingsDlg::ValidateAndAssign()
{
	//	1. Back Services Directories (the dialog does actual changes in application state, but tries to reset them on error)

	//	SetDataPath, SetLibraryPath	 and SetProjectsPath only make changes if input is different from 
	//	current values, so these assignments don't make unnecessary changes and work as validations.
	//	If there is an error with an assignment, it is assumed that no changes are made by m_bs, and
	//	we try to reset the previous directories if the user cancels.

	//	ApplicationPaths_page

    std::string saveDataDirectory = mSettings.BratPaths().mInternalDataDir;
    std::string saveLibraryDirectory = mSettings.BratPaths().WorkspacesPath();

	//if ( !m_bs.SetDataPath( DataDirectory_lineEdit->text().toLatin1() ) )
 //   {
 //       QMessageBox::StandardButton b =
 //               QMessageBox::critical(this, tr("Data Path Error"),
 //                                     "Error assigning the data default directory.",
 //                                     QMessageBox::Ok | QMessageBox::Cancel );
 //       if ( b == QMessageBox::Ok )
 //           return false;
 //   }
 //   if ( !m_bs.SetLibraryPath( ExternalDataDirectory_lineEdit->text().toLatin1() ) )
 //   {
 //       QMessageBox::StandardButton b =
 //               QMessageBox::critical(this, tr("Library Path Error"),
 //                                     "Error assigning the Library default directory.",
 //                                     QMessageBox::Ok | QMessageBox::Cancel );
 //       if ( b == QMessageBox::Ok )
 //           return false;
	//	else
	//		m_bs.SetDataPath( saveDataDirectory.c_str() );
 //   }
 //   if ( !m_bs.SetProjectsPath( ProjectsDirectory_lineEdit->text().toLatin1() ) )
 //   {
 //       QMessageBox::StandardButton b =
 //               QMessageBox::critical(this, tr("Projects Path Error"),
 //                                     "Error assigning the Projects default directory.",
 //                                     QMessageBox::Ok | QMessageBox::Cancel );
 //       if ( b == QMessageBox::Ok )
 //           return false;
	//	else
	//	{
	//		m_bs.SetDataPath( saveDataDirectory.c_str() );
	//		m_bs.SetLibraryPath( saveLibraryDirectory.c_str() );
	//	}
 //   }


	//	2. Application Options (the dialog acts over an options object copy, not changing the application state)

	//	2a. StartupOptions_page

	mSettings.mLoadLastWorkspaceAtStartUp = LoadLastProjectAtAtartup_checkBox->isChecked();

#if defined(DEBUG) || defined(_DEBUG)
	assert__( mDesktopManagerSdiCheckbox );
	mSettings.mDesktopManagerSdi = mDesktopManagerSdiCheckbox->isChecked();
#endif


	//	2b. ApplicationStyles_page

	if ( Styles_listWidget->currentItem() )
		mSettings.mAppStyle = Styles_listWidget->currentItem()->text();
    mSettings.mUseDefaultStyle = DefaultStyle_checkBox->isChecked();
	mSettings.mCustomAppStyleSheet = (EApplicationStyleSheets)StyleSheets_listWidget->currentRow();
	mSettings.mNoStyleSheet = NoStyleSheet_checkBox->isChecked();

    return true;
}

void CApplicationSettingsDlg::accept()
{
    if ( ValidateAndAssign() )
        QDialog::accept();
}



///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_ApplicationSettingsDlg.cpp"
