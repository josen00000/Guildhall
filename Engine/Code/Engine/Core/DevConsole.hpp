#pragma once
#include <string>
#include <queue>
#include <vector>
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"

class RenderContext;
class Camera;
class BitmapFont;
struct Vertex_PCU;

enum SelectMode {
	START_SELECT,
	PAUSE_SELECT,
	END_SELECT
};

struct ColoredLine {
	ColoredLine( Rgba8 c, std::string t)
		:color(c)
		,text(t)
	{
	};
	Rgba8 color;
	std::string text;
};

class DevConsole
{
public:
	explicit DevConsole( BitmapFont* font, Camera* camera );
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();
	void Update( float deltaSeconds );

	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString);
	void StartDevConcole();
	void EndDevConcole();
	void SetIsOpen( bool isOpen );
	bool IsOpen() const;
	static DevConsole* InitialDevConsole( BitmapFont* font, Camera* camera );

	void Render( RenderContext& renderer ) const;
	void AddVertForInput() const;
	void AddVertForContent() const;
	
	// Input & control
	void CheckIfConsoleOpen();
	void ProcessInput();
	void ProcessCharacterInput();
	void ProcessControlInput();
	void AddCharToInput( char c );
	void DeleteCharFromInput( bool isBefore );
	void ClearInput();
	bool HasInput();

	// Command
	void SubmitCommand();
	void ExecuteCommand( std::string comd );
	static bool CheckIfCommandExist( std::string comd, int& index );
	static bool CheckIfCommandExist( std::string comd );
	static void AddCommandToCommandList( std::string comd, EventCallbackFunctionPtr funcPtr );
	static bool CheckIfCommandEqual( std::string a, std::string b );
	void ExecuteQuitFunction();
	void LogErrorMessage();

	// Caret
	void ResetCaretIndex();
	void UpdateCaretIndex( int deltaIndex );
	void SetCaretIndex( int index );
	void RenderCaret() const;
	void StartSelect();
	void PauseSelect();
	void EndSelect();
	void RenderSelectArea() const;
	void UpdateAbleRenderCaret( float deltaSeconds );

	// History
	void StartDisplayHistory();
	void EndDisplayHistory();
	void UpdateHistoryIndex( int deltaIndex );
	void RecordCommandInHistory( std::string comd );
	bool CheckIfCommandExistInHistory( std::string comd, int& index );
	void DeleteCommandInHistory( int index );
	void LoadHistory();
	void SaveHistoryToFile();

	// ClipBoard
	bool SendSelectedStringToClipBoard();
	bool ReceiveStringFromClipBoard();
	void InputCopyedString( std::string copyed );

	// Accessor
	int GetMaxLinesNum() const;
	float GetLineAlignmentY() const;

private:

public:
	bool m_ableRenderCaret = true;
	bool m_historyMode	= false;
	SelectMode m_selectMode	= END_SELECT;
	bool m_ableQuit = false;
	int m_caretIndex;
	int m_historyCommandIndex;
	Vec2 m_caretPos;
	float m_lineHeight = 1.f;
	Rgba8 m_defaultColor = Rgba8::WHITE;
	BitmapFont* m_font	= nullptr;
	Camera* m_camera	= nullptr;
	std::string m_inputs;
	std::vector<ColoredLine> m_lines;
	mutable std::vector<Vertex_PCU> m_vertices;
	std::vector<std::string> m_commandsHistory;
	static std::map<std::string, std::string> s_commands;

private:
	bool m_isOpen = false; 
	int m_startSelectIndex;
};

