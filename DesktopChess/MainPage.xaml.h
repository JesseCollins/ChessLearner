//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "BoardState.h"

namespace DesktopChess
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void SetupBoard();
		void ResetSquareColors();
		void ArrangePieces();
		void MakeMove(ChessMove move);
	private:
		BoardState m_boardState;
		void OnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e);

		Windows::UI::Xaml::Shapes::Rectangle^ m_squares[64];
		Windows::UI::Xaml::Controls::TextBlock^ m_pieceTextBlocks[64];


		BoardLocation m_selected;
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
