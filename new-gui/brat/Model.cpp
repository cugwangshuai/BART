#include "stdafx.h"

#include "new-gui/Common/QtUtilsIO.h"
#include "new-gui/Common/ApplicationPaths.h"
#include "Workspaces/Workspace.h"
#include "Model.h"


//static 
CModel* CModel::smInstance = nullptr;



//static 
CModel& CModel::CreateInstance( const CApplicationPaths &brat_paths )
{
	assert__( !smInstance );

	smInstance = new CModel( brat_paths );
	return *smInstance;
}


//explicit 
CModel::CModel( const CApplicationPaths &brat_paths ) 
	: mBratPaths( brat_paths )
{}

//virtual 
CModel::~CModel()
{
	mTree.Clear();

	assert__( smInstance );
	delete smInstance;
	smInstance = nullptr;
}	


//static 
CWorkspace* CModel::RootWorkspace( CTreeWorkspace &tree )
{
	return tree.GetRootData();
}

CWorkspace* CModel::RootWorkspace()
{
	return RootWorkspace( mTree );
}


//static 
template< class WKSPC >
WKSPC* CModel::Workspace( CTreeWorkspace &tree )
{
	CWorkspace* wks = RootWorkspace( tree );
	if ( !wks )								   	//admissible when no workspace loaded
		return nullptr;

	std::string workspaceKey = tree.ChildKey( WKSPC::NAME );

	return dynamic_cast< WKSPC* >( tree.FindWorkspace( workspaceKey ) );
}


CWorkspace* CModel::CreateWorkspace( const std::string& name, const std::string& path, std::string &error_msg )
{
    return mTree.CreateReset( mBratPaths.mInternalDataDir, name, path, error_msg );
}

CWorkspace* CModel::CreateTree( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
    return tree.LoadReset( smInstance->mBratPaths.mInternalDataDir, path, error_msg );
}


//static 
CWorkspace* CModel::LoadWorkspace( CTreeWorkspace &tree, const std::string& path, std::string &error_msg )
{
	CWorkspace *wks = CreateTree( tree, path, error_msg );
	CWorkspace *failed_wks = nullptr;
	if ( !tree.LoadConfig( 
		failed_wks,
		Workspace< CWorkspaceDataset >( tree ),
		Workspace< CWorkspaceOperation >( tree ),
		Workspace< CWorkspaceDisplay >( tree ), 
		error_msg 
		) )
	{
		error_msg += ( "\nUnable to load workspace '" + ( failed_wks ? failed_wks->GetName() : "" ) + "'." );
		delete wks;
		wks = nullptr;
	}

	return wks;
}

CWorkspace* CModel::LoadWorkspace( const std::string& path, std::string &error_msg )
{
	return LoadWorkspace( mTree, path, error_msg );
}


void CModel::Reset()
{
	mTree.Clear();
}



bool CModel::LoadImportFormulas( const std::string& path, std::vector< std::string > &v, bool predefined, bool user, std::string &error_msg )
{
	CTreeWorkspace import_tree;
	CWorkspace* wks = CreateTree( import_tree, path, error_msg );
	if ( !wks )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	const CWorkspaceFormula* wksFormula =  import_tree.LoadConfigFormula(
		Workspace< CWorkspaceDataset >( import_tree ),
		Workspace< CWorkspaceOperation >( import_tree ),
		Workspace< CWorkspaceDisplay >( import_tree ), 
		error_msg 
		);

	if ( !wksFormula )
	{
		assert__( !error_msg.empty() );
		return false;
	}

	wksFormula->GetFormulaNames( v, predefined, user );

	import_tree.Clear();

	return true;
}


bool CModel::ImportWorkspace( const std::string& path, 
	bool datasets, bool formulas, bool operations, bool views, const std::vector< std::string > &vformulas, std::string &error_msg )
{
	bool bOk = false;               Q_UNUSED( bOk );

	CWorkspace *wks = RootWorkspace();			assert__( wks != nullptr );

	//wxGetApp().m_tree.GetImportBitSet()->m_bitSet.reset();

	std::string wks_path = wks->GetPath();	//wxGetApp().GetCurrentWorkspace()->GetPathName();
	//wxFileName currentWksPath;
	//currentWksPath.AssignDir( wks_path );
	//currentWksPath.MakeAbsolute();

	if ( !SaveWorkspace( error_msg ) )	// TODO SaveWorkspace failures not verified in v3
		return false;

	mTree.ResetImportBits();
	mTree.SetImportBits(
	{
		{ IMPORT_DATASET_INDEX, datasets },
		{ IMPORT_FORMULA_INDEX, formulas },
		{ IMPORT_OPERATION_INDEX, operations },
		{ IMPORT_DISPLAY_INDEX, views }
	} );

	//CWorkspace* import_wks = new CWorkspace( dlg.mPath );		//CWorkspace* wks = new CWorkspace( dlg.GetWksName()->GetValue(), dlg.mPath, false );

	//wxGetApp().CreateTree( wks, wxGetApp().m_treeImport );
	CTreeWorkspace import_tree;	 			//import_tree.Clear();		// TODO see if this is necessary on a new tree
	import_tree.SetCtrlDatasetFiles( mTree.GetImportBit( IMPORT_DATASET_INDEX ) );
	//wxGetApp().m_treeImport.SetCtrlDatasetFiles( wxGetApp().m_tree.GetImportBitSet()->m_bitSet.test( IMPORT_DATASET_INDEX ) );

	//-----------------------------------------
	//wxGetApp().SetCurrentTree( &( wxGetApp().m_treeImport ) );
	//-----------------------------------------

	//bOk = wxGetApp().m_treeImport.LoadConfig();
	CWorkspace* import_wks = LoadWorkspace( import_tree, path, error_msg );

	if ( !import_wks || !error_msg.empty() )			//TODO TEST THIS ERROR RECOVERY
	{
		//wxGetApp().m_treeImport.DeleteTree();
		mTree.Clear();

		bool oldValue = mTree.GetCtrlDatasetFiles();
		mTree.SetCtrlDatasetFiles( false );

		LoadWorkspace( wks_path, error_msg );

		mTree.SetCtrlDatasetFiles( oldValue );

		return false;
	}

	//-----------------------------------------
	//wxGetApp().SetCurrentTree( &( wxGetApp().m_tree ) );
	//-----------------------------------------

	// Retrieve formula to Import
	CWorkspaceFormula* wksFormula = Workspace< CWorkspaceFormula >( mTree );
	//CWorkspaceFormula* wksFormula = wxGetApp().GetCurrentWorkspaceFormula();

	//dlg.GetCheckedFormulas( wksFormula->GetFormulasToImport() );
	wksFormula->SetFormulasToImport( vformulas );

	std::string keyToFind;
	bool result = mTree.Import( 
		&import_tree, 
		Workspace< CWorkspaceDataset >( mTree ), 
		Workspace< CWorkspaceDisplay >( mTree ), 
		Workspace< CWorkspaceOperation >( mTree ), 
		keyToFind, 
		error_msg );

	if ( result )
	{
		SaveWorkspace( error_msg );

		CWorkspaceOperation* wksOpImport = Workspace< CWorkspaceOperation >( import_tree );
		CWorkspaceOperation* wksOp = Workspace< CWorkspaceOperation >( mTree );
		//CWorkspaceOperation* wksOp = wxGetApp().GetCurrentWorkspaceOperation();
		if ( wksOp != nullptr && wksOpImport != nullptr )
		{
			//CDirTraverserCopyFile traverserCopyFile( wksOp->GetPath(), "nc" );
			////path name given to wxDir is locked until wxDir object is deleted
			//wxDir dir;
			//dir.Open( wksOpImport->GetPathName() );
			//dir.Traverse( traverserCopyFile );
			TraverseDirectory( t2q( wksOpImport->GetPath() ), t2q( wksOp->GetPath() ), { "*.nc" } );
		}
	}

	//wxGetApp().m_treeImport.DeleteTree();
	import_tree.Clear();
	mTree.Clear();

	bool oldValue = mTree.GetCtrlDatasetFiles();
	mTree.SetCtrlDatasetFiles( false );

	result = LoadWorkspace( wks_path, error_msg );

	mTree.SetCtrlDatasetFiles( oldValue );

	return result;
}





///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

void CModel::XYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CPlot* plot = dynamic_cast<CPlot*>( *itGroup );
		if ( plot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateXYPlot( plot, size, pos );
	}
	*/
}
void CModel::ZFXYPlot()
{
	/*
	CObArray::iterator itGroup;

	wxSize size = wxDefaultSize;
	wxPoint pos = wxDefaultPosition;

	for ( itGroup = m_plots.begin(); itGroup != m_plots.end(); itGroup++ )
	{
		CZFXYPlot* zfxyplot = dynamic_cast<CZFXYPlot*>( *itGroup );
		if ( zfxyplot == nullptr )
		{
			continue;
		}

		CWindowHandler::GetSizeAndPosition( size, pos );

		CreateZFXYPlot( zfxyplot, size, pos );
	}
	*/
}






///////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////

#include "moc_Model.cpp"
