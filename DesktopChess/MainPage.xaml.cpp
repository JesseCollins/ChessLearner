//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "GameAi.h"
#include "XamlHelper.h"

using namespace DesktopChess;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Animation;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Xaml::Shapes;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
	: m_selected(InvalidBoardLocation)
{
	InitializeComponent();

	SetupBoard();
}

SolidColorBrush^ CreateColoredBrush(unsigned char r, unsigned char g, unsigned char b)
{
	auto brush = ref new SolidColorBrush();
	brush->Color = Windows::UI::ColorHelper::FromArgb(0xff, r, g, b);
	return brush;
}

void MainPage::SetupBoard()
{
	for (int i = 0; i < 8; ++i)
	{
		Board->RowDefinitions->Append(ref new RowDefinition());
		Board->ColumnDefinitions->Append(ref new ColumnDefinition());
	}

	for (int x = 0; x < 8;++x)
	{ 
		for (int y = 0; y < 8; ++y)
		{
			auto rect = ref new Rectangle();
			rect->SetValue(Grid::ColumnProperty, x);
			rect->SetValue(Grid::RowProperty, y);
			rect->Tapped += ref new Windows::UI::Xaml::Input::TappedEventHandler(this, &DesktopChess::MainPage::OnTapped);
			Board->Children->InsertAt(0, rect);
			m_squares[x + y * 8] = rect;
		}
	}

	ResetSquareColors();
	ArrangePieces();
}

void MainPage::ResetSquareColors()
{
	SolidColorBrush^ brushes[] = {
		CreateColoredBrush(180, 180, 180),
		CreateColoredBrush(50, 50, 50)
	};

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto rect = m_squares[x + y * 8];
			rect->Fill = brushes[(x + y) % 2];
		}
	}
}

void MainPage::ArrangePieces()
{
	SolidColorBrush^ pieceBrushes[] {
		CreateColoredBrush(255, 255, 255),
		CreateColoredBrush(0, 0, 0)
	};
	static const wchar_t* pieceStrings[] =
	{
		L"",
		L"\u265F",
		L"\u265D",
		L"\u265E",
		L"\u265C",
		L"\u265B",
		L"\u265A"
	};
	

	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y)
		{
			auto piece = m_boardState.Get(x, y);

			if (piece.Type != PieceType::Empty)
			{
				auto tb = ref new TextBlock();
				tb->Text = ref new Platform::String(pieceStrings[(byte)piece.Type]);
				tb->Foreground = pieceBrushes[(int)piece.Side];
				tb->SetValue(Grid::ColumnProperty, x);
				tb->SetValue(Grid::RowProperty, y);
				tb->Style = PieceStyle;

				//auto rt = ref new RotateTransform();
				//tb->RenderTransform = rt;
				//auto rt = static_cast<RotateTransform^>(static_cast<TransformGroup^>(tb->RenderTransform)->Children->GetAt(1));
				//rt->Angle = (double)(rand() % 20) - 10.;
				Board->Children->Append(tb);

				m_pieceTextBlocks[x + y*8] = tb;
			}
		}
	}
}

void DoAnimation(UIElement^ e, double dx, double dy)
{
	auto tt = ref new TranslateTransform();
	e->RenderTransform = tt;

	//double distance = sqrt(dx*dx + dy*dy);

	//auto tt = static_cast<TranslateTransform^>(static_cast<TransformGroup^>(e->RenderTransform)->Children->GetAt(0));

	auto ts = Windows::Foundation::TimeSpan();
	ts.Duration = 4 * 1000 * 1000;

	auto sb = ref new Storyboard();

	auto daX = ref new DoubleAnimation();
	sb->Children->Append(daX);
	Storyboard::SetTarget(daX, tt);
	Storyboard::SetTargetProperty(daX, L"X");
	daX->From = dx;
	daX->To = 0.;
	daX->Duration = ts;
	daX->EasingFunction = ref new ExponentialEase();

	auto daY = ref new DoubleAnimation();
	sb->Children->Append(daY);
	Storyboard::SetTarget(daY, tt);
	Storyboard::SetTargetProperty(daY, L"Y");
	daY->From = dy;
	daY->To = 0.;
	daY->Duration = ts;
	
	daY->EasingFunction = ref new ExponentialEase();

	
	sb->Begin();
	
}

void MainPage::MakeMove(ChessMove move)
{
	m_boardState.Move(move.From, move.To, [&](BoardLocation from, BoardLocation to) {

		if (to == InvalidBoardLocation)
		{
			// Piece is dying
			m_pieceTextBlocks[from.Raw()]->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		}
		else if (from == InvalidBoardLocation)
		{
			// Piece is new
		}
		else
		{
			// Piece is moving
			auto pieceTb = m_pieceTextBlocks[from.Raw()];

			double offsetX = (to.X() - from.X()) * Board->ActualWidth / 8.;
			double offsetY = (to.Y() - from.Y()) * Board->ActualHeight / 8.;

			DoAnimation(pieceTb, -offsetX, -offsetY);

			pieceTb->SetValue(Grid::ColumnProperty, to.X());
			pieceTb->SetValue(Grid::RowProperty, to.Y());

			m_pieceTextBlocks[to.Raw()] = pieceTb;
			m_pieceTextBlocks[from.Raw()] = nullptr;
		}
	});	

	if (m_boardState.IsCheckmate())
	{
		auto msg = ref new Windows::UI::Popups::MessageDialog(L"Wow, that's a checkmate!");
		msg->ShowAsync();
	}
}

void MainPage::OnTapped(Platform::Object ^sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs ^e)
{
	ResetSquareColors();

	auto rect = safe_cast<Rectangle^>(sender);
	int x = (int)rect->GetValue(Grid::ColumnProperty);
	int y = (int)rect->GetValue(Grid::RowProperty);

	BoardLocation tappedLoc(x, y);

	if (m_boardState.CanMove(m_selected, tappedLoc))
	{
		MakeMove({ m_selected, tappedLoc });
		m_selected = InvalidBoardLocation;
	}
	else
	{
		m_selected = tappedLoc;
		int found = 0;
		for (auto to : m_boardState)
		{
			if (m_boardState.CanMove(tappedLoc, to))
			{
				auto toRect = m_squares[to.Raw()];
				toRect->Fill = CreateColoredBrush(100, 150, 100);
				++found;
			}
		}

		if (found > 0)
		{
			rect->Fill = CreateColoredBrush(50, 150, 50);
		}
		else
		{
			rect->Fill = CreateColoredBrush(150, 50, 50);
		}

	}
}


void DesktopChess::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_gameAi = std::make_unique<GameAi>();
	m_gameAi->StartDecideMove(m_boardState);

	if (m_aiTimer)
	{
		m_aiTimer->Stop();
	}

	m_aiTimer = ref new DispatcherTimer();

	m_aiTimer->Interval = MakeTimeSpan(100);
	m_aiTimer->Tick += ref new Windows::Foundation::EventHandler<Platform::Object ^>(this, &DesktopChess::MainPage::OnTick);
	m_aiTimer->Start();
}


void DesktopChess::MainPage::OnTick(Platform::Object ^sender, Platform::Object ^args)
{
	if (m_gameAi->IsFinished())
	{
		
		this->MoveInfoText->Text = MakeString(L"Time: %0.3f", m_gameAi->GetElapsedTime() / 1000.);

		MakeMove(m_gameAi->GetMove());
		
		m_gameAi = nullptr;
		m_aiTimer->Stop();
		m_aiTimer = nullptr;
	}
}
