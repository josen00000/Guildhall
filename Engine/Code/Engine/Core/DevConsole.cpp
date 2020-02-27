﻿#include <iostream>
#include <fstream>
#include <windows.h>
#include "DevConsole.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;

std::map<std::string, std::string> DevConsole::s_commands;

DevConsole::DevConsole( BitmapFont* font, Camera* camera )
	:m_font(font)
	,m_camera(camera)
{

}

void DevConsole::Startup()
{
	LoadHistory();
}

void DevConsole::BeginFrame()
{
}

void DevConsole::EndFrame()
{
	m_vertices.clear();
}

void DevConsole::Shutdown()
{
	SaveHistoryToFile();
}

void DevConsole::Update( float deltaSeconds )
{
	CheckIfConsoleOpen();
	if( !m_isOpen ){ return; }

	ProcessInput();
	
	UpdateAbleRenderCaret( deltaSeconds );
	if( m_historyMode ) {
		m_inputs = m_commandsHistory[m_historyCommandIndex];
	}
}

void DevConsole::ProcessInput()
{
	
	ProcessControlInput();
	ProcessCharacterInput();
}

void DevConsole::ProcessCharacterInput()
{
	char c;
	while( g_theInputSystem->PopCharacter( &c ) ) {
		if( m_historyMode ) {
			m_historyMode = false;
		}
		AddCharToInput( c );
	}
}

void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	ColoredLine tem = ColoredLine(textColor, devConsolePrintString);
	m_lines.push_back(tem);
}

void DevConsole::StartDevConcole()
{
	m_isOpen = true;
	g_theInputSystem->ClearCharacters();
	m_inputs.clear();
}

void DevConsole::EndDevConcole()
{
	m_isOpen = false;
	EndSelect();
	ClearInput();
	g_theInputSystem->ClearCharacters();
}

void DevConsole::Render( RenderContext& renderer ) const
{
	if( !m_isOpen ) { return; }
	g_theRenderer->BeginCamera( *m_camera );

	// render input
	// render content
	// render caret
	// render select area ( if possible )

	AddVertForInput();
	AddVertForContent();
	//const Texture* temTexture = m_font->GetTexture();
	//g_theRenderer->BindTexture( temTexture );
	renderer.DrawVertexVector( m_vertices );


	g_theRenderer->BindTexture( nullptr );

	if( m_ableRenderCaret ) {
		RenderCaret();
	}
	
	if( m_selectMode != END_SELECT ) {
		RenderSelectArea();
	}


}

void DevConsole::RenderCaret() const
{
	float cellWidth = m_font->GetCellWidth( m_lineHeight, 1.f );
	AABB2 cameraBox = m_camera->GetCameraBox();
	
	Vec2 start = cameraBox.mins + Vec2( cellWidth * m_caretIndex, 0.f );
	Vec2 end = cameraBox.mins + Vec2( cellWidth * m_caretIndex, m_lineHeight );
	g_theRenderer->DrawLine( start, end, 0.2f, Rgba8::WHITE );
}

void DevConsole::StartSelect()
{
	m_selectMode = START_SELECT;
	m_startSelectIndex = m_caretIndex;
}

void DevConsole::PauseSelect()
{
	m_selectMode = PAUSE_SELECT;
}

void DevConsole::EndSelect()
{
	m_selectMode = END_SELECT;
}

void DevConsole::RenderSelectArea() const
{
	AABB2 cameraBox = m_camera->GetCameraBox();
	float cellWidth = m_font->GetCellWidth( m_lineHeight , 1.f );
	Rgba8 selectColor = Rgba8( 176, 224, 230, 50 );

	Vec2 boxMin = Vec2( cameraBox.mins + Vec2( cellWidth * m_caretIndex, 0.f ) );
	Vec2 boxMax = Vec2( cameraBox.mins + Vec2( cellWidth * m_startSelectIndex, m_lineHeight ) );
	AABB2 selectBox = AABB2( boxMin, boxMax );
	g_theRenderer->DrawAABB2D( selectBox, selectColor );
}

void DevConsole::UpdateAbleRenderCaret( float deltaSeconds )
{
	static float timer = 0;
	timer += deltaSeconds;
	int tem = (int)(timer / 0.5f);
	int odd = tem % 2;
	if( odd != 0 ) {
		m_ableRenderCaret = true;
	}
	else {
		m_ableRenderCaret = false;
	}
}

void DevConsole::StartDisplayHistory()
{
	m_historyMode = true;
	m_historyCommandIndex = (int)m_commandsHistory.size() - 1;
	if( m_historyCommandIndex < 0 ){
		EndDisplayHistory();
	}
}

void DevConsole::EndDisplayHistory()
{
	m_historyMode = false;
}

void DevConsole::AddVertForInput() const
{
	// Render input line
	Vec2 alignment = Vec2::ZERO;
	m_font->AddVertsForTextInBox2D( m_vertices, m_camera->GetCameraBox(), m_lineHeight, m_inputs, m_defaultColor, 1, alignment );
	// Render marker
	
}

void DevConsole::AddVertForContent() const
{
	int maxDisplayedLinesNum = GetMaxLinesNum();
	
	float deltaY = (float)1 / maxDisplayedLinesNum ;
	Vec2 alignment = Vec2( 0.f, deltaY );
	for( int lineIndex = (int)(m_lines.size() - 1); lineIndex > (int)m_lines.size() - maxDisplayedLinesNum; lineIndex-- ) {
		if( lineIndex < 0 ) { break; }
		ColoredLine tem = m_lines[lineIndex];
		m_font->AddVertsForTextInBox2D( m_vertices, m_camera->GetCameraBox(), m_lineHeight, tem.text, tem.color, 1.f, alignment );
		alignment.y += deltaY;
	}
}

void DevConsole::CheckIfConsoleOpen()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_TILDE ) ) {
		if( !g_theConsole->IsOpen() ) {
			g_theConsole->StartDevConcole();
		}
		else {
			g_theConsole->EndDevConcole();
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ) {
		if( g_theConsole->IsOpen() ) {
			if( g_theConsole->m_historyMode ) {
				g_theConsole->EndDisplayHistory();
			}
			if( g_theConsole->HasInput() ) {
				g_theConsole->ClearInput();
			}
			else {
				g_theConsole->EndDevConcole();
			}
		}
	}
}

void DevConsole::ProcessControlInput()
{
	
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		if( !g_theConsole->m_historyMode ) {
			g_theConsole->StartDisplayHistory();
		}
		else {
			g_theConsole->UpdateHistoryIndex( -1 );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		if( g_theConsole->m_historyMode ) {
			g_theConsole->UpdateHistoryIndex( 1 );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_LEFT_ARROW ) ) {
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_SHIFT ) ) {
			if( g_theConsole->m_selectMode == END_SELECT ) {
				g_theConsole->StartSelect();
			}
		}
		else if( g_theConsole->m_selectMode == PAUSE_SELECT ) {
			g_theConsole->EndSelect();
		}
		g_theConsole->UpdateCaretIndex( -1 );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_RIGHT_ARROW ) ) {
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_SHIFT ) ) {
			if( g_theConsole->m_selectMode == END_SELECT ) {
				g_theConsole->StartSelect();
			}
		}
		else if( g_theConsole->m_selectMode == PAUSE_SELECT ) {
			g_theConsole->EndSelect();
		}
		g_theConsole->UpdateCaretIndex( 1 );
	}
	else {
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DELETE ) ) {
			g_theConsole->EndDisplayHistory();
			g_theConsole->DeleteCharFromInput( false );
		}
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_BACK ) ) {
			g_theConsole->EndDisplayHistory();
			g_theConsole->DeleteCharFromInput( true );
		}
		else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ENTER ) ) {
			g_theConsole->EndDisplayHistory();
			g_theConsole->SubmitCommand();
		}
	}

	// Release event
	if( g_theInputSystem->WasKeyJustReleased( KEYBOARD_BUTTON_ID_SHIFT ) ) {
		if( g_theConsole->m_selectMode == START_SELECT ) {
			g_theConsole->PauseSelect();
		}
	}
}

void DevConsole::AddCharToInput( char c )
{
	if( m_selectMode != END_SELECT ){
		DeleteCharFromInput( true );
	}
	m_inputs.insert( ( m_inputs.begin() + m_caretIndex ) , c );
	UpdateCaretIndex( 1 );
}

void DevConsole::DeleteCharFromInput( bool isBefore )
{
	if( m_selectMode != END_SELECT ) {
		if( m_caretIndex < m_startSelectIndex ) {
			m_inputs.erase( m_caretIndex, m_startSelectIndex - m_caretIndex );
			SetCaretIndex( m_caretIndex );
		}
		else {
			m_inputs.erase( m_startSelectIndex, m_caretIndex - m_startSelectIndex );
			SetCaretIndex( m_startSelectIndex );
		}
		EndSelect();
		return;
	}
	if( isBefore ) {
		if( m_caretIndex == 0 ){ return; }
		m_inputs.erase( m_inputs.begin() + m_caretIndex - 1 );
		UpdateCaretIndex( -1 );
	}
	else {
		if( m_caretIndex == m_inputs.size() ){ return; }
		m_inputs.erase( m_inputs.begin() + m_caretIndex );
	}
}

void DevConsole::ClearInput()
{
	m_inputs.clear();
	ResetCaretIndex();
}

bool DevConsole::HasInput()
{
	if( m_inputs.size() > 0 ){
		return true;
	}
	else{
		return false;
	}
}

void DevConsole::SubmitCommand()
{
	std::string command = m_inputs;
	ClearInput();
	EndSelect();
	if ( CheckIfCommandExist( command ) ){
		RecordCommandInHistory( command );
	}
	ExecuteCommand( command	);
}

void DevConsole::ExecuteCommand( std::string comd )
{
	EventArgs tempEventArgs;
// 	if( command == "help" ){
// 		ExecuteHelpFunction();
// 	}
// 	else if( command == "quit" ) {
// 		ExecuteQuitFunction();
// 	}
// 	else if( command == "test" ) {
// 		PrintString( Rgba8::WHITE, "test.");
// 	}
// 	else {
// 		PrintString( Rgba8::RED, "Wrong Command!" );
// 	}
// 	
// 	
	if( comd == "test" || comd == "test1" || comd == "test2" ){
		PrintString( m_defaultColor, comd + "test for history" );
		return;
	}
	if( CheckIfCommandExist( comd ) ){
		g_theEventSystem->FireTheEvent( comd, tempEventArgs );	
	}
	else{
		LogErrorMessage();
	}
}

void DevConsole::AddCommandToCommandList( std::string comd, std::string desc, EventCallbackFunctionPtr funcPtr )
{
	if( !CheckIfCommandExist( comd ) ) {
		s_commands[comd] = desc;
	}
	if( comd == "test" || comd == "test1" || comd == "test2" ){ return;}
	g_theEventSystem->SubscribeTheEvent( comd, funcPtr );
}

void DevConsole::ExecuteQuitFunction()
{
	m_ableQuit = true;
}

void DevConsole::LogErrorMessage()
{
	PrintString( m_defaultColor, "Command Invalid! -help to read all commands " );
}

DevConsole* DevConsole::InitialDevConsole( BitmapFont* font, Camera* camera )
{
	DevConsole* devConcole = new DevConsole( font, camera );
	return devConcole;
}

void DevConsole::ResetCaretIndex()
{
	m_caretIndex = 0;
}

void DevConsole::UpdateCaretIndex( int deltaIndex )
{
	m_caretIndex += deltaIndex;
	m_caretIndex = ClampInt( 0, (int)m_inputs.size(), m_caretIndex );
}

void DevConsole::SetCaretIndex( int index )
{
	m_caretIndex = index;
}

void DevConsole::UpdateHistoryIndex( int deltaIndex )
{
	m_historyCommandIndex += deltaIndex;
	m_historyCommandIndex = ClampInt( 0, ( (int)m_commandsHistory.size() - 1 ), m_historyCommandIndex );
}

void DevConsole::RecordCommandInHistory( std::string comd )
{
	int index;
	if( CheckIfCommandExistInHistory( comd, index ) ){
		DeleteCommandInHistory(  index );
	}
		m_commandsHistory.push_back( comd );
	
}

bool DevConsole::CheckIfCommandExistInHistory( std::string comd, int& index )
{
	for( index = 0; index < m_commandsHistory.size(); index++ ) {
		if( m_commandsHistory[index] == comd ){
			return true;
		}
	}
	
	return false;
}

bool DevConsole::CheckIfCommandExist( std::string comd )
{
	auto it = s_commands.find( comd );
	if( it != s_commands.end() ) {
		return true;
	}
	else {
		return false;
	}
}


void DevConsole::DeleteCommandInHistory( int index )
{
	m_commandsHistory.erase( m_commandsHistory.begin() + index );
}

void DevConsole::LoadHistory()
{
	std::ifstream myFile;
	myFile.open( "./Data/DevConsole/History.txt" );
	if( myFile.is_open() ){
		std::string line;
		while ( getline( myFile, line ) )
		{
			RecordCommandInHistory( line );
		}
		myFile.close();
	}

}

void DevConsole::SaveHistoryToFile()
{
	std::ofstream myFile;
	myFile.open( "./Data/DevConsole/History.txt" );
	if( !myFile.fail() ){
		for( int comIndex = 0; comIndex < m_commandsHistory.size(); comIndex++ ){
			myFile << m_commandsHistory[comIndex] << std::endl;
		}
		myFile.close();
	}
	else {
		ERROR_AND_DIE(" can not open file. ");
	}
}

bool DevConsole::SendSelectedStringToClipBoard()
{
	// get selected string
	//send string to clip board
	std::string selectedString;
	if( m_selectMode != END_SELECT ){
		if( m_startSelectIndex < m_caretIndex ){
			selectedString = m_inputs.substr( m_startSelectIndex, m_caretIndex - m_startSelectIndex );
		}
		else if( m_startSelectIndex > m_caretIndex ) {
			selectedString = m_inputs.substr( m_caretIndex, m_startSelectIndex - m_caretIndex );
		}
		else{
			return false;
		}

		OpenClipboard( NULL );
		EmptyClipboard();
		HGLOBAL hGlob = GlobalAlloc( GMEM_MOVEABLE, selectedString.size() + 1 );
		if( !hGlob ){ 
			CloseClipboard();
			return false;
		}
		memcpy( GlobalLock( hGlob ), selectedString.c_str(), selectedString.size() + 1 );
		GlobalUnlock( hGlob );
		SetClipboardData( CF_TEXT, hGlob );
		CloseClipboard();
		GlobalFree( hGlob );
		return true;
	}
	else {
		return false;
	}
}

bool DevConsole::ReceiveStringFromClipBoard()
{
	// get string form clipboard
	// input string to input

	OpenClipboard( NULL );
	HGLOBAL hGlob =  GetClipboardData( CF_TEXT );
	if( hGlob == NULL ){ return false; }
	char* stringPtr = static_cast<char*>( GlobalLock( hGlob ) );

	std::string copyedString = std::string( stringPtr );
	InputCopyedString( copyedString );
	return true;
}

void DevConsole::InputCopyedString( std::string copyed )
{
	for( int charIndex = 0; charIndex < copyed.size(); charIndex++ ) {
		AddCharToInput( copyed[charIndex ] );
	} 
}

int DevConsole::GetMaxLinesNum() const
{
	return (int)( m_camera->GetCameraHeight() / m_lineHeight );
}

float DevConsole::GetLineAlignmentY() const
{
	return 0;
}

void DevConsole::SetIsOpen( bool isOpen )
{
	m_isOpen = isOpen;
}

bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

