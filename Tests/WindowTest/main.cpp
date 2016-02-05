#include "..\YupeiLibrary\NativeWindows.h"
#include "..\YupeiLibrary\Graphics.h"
#include "..\YupeiLibrary\Panel.h"
#include "..\YupeiLibrary\Shapes.h"
#include <CommCtrl.h>
using namespace Yupei;

int WINAPI wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	auto window = make_unique<NativeWindow>();
	D2D1_GRADIENT_STOP stops[] = {
		{0.f,D2D1::ColorF(D2D1::ColorF::Yellow)},
		{1.f,D2D1::ColorF(D2D1::ColorF::Blue) }
	};
	auto brush = reinterpret_cast<Yupei::Graphics::Direct2D::DrawingContext*>(window->GetContext())->
		GetLinearGradientBrush(stops, 
		2, 
		D2D1::Point2F(20, 20), 
		D2D1::Point2F(400, 500));
	StackPanel* stackPanel = new Yupei::StackPanel();
	window->SetContent(stackPanel);
	auto rect = new Yupei::Rectangle(160.f,200.f);
	auto rect2 = new Yupei::Rectangle(160.f, 200.f);
	rect->SetFill(brush);
	rect2->SetFill(brush);
	stackPanel->AddVisualChild(rect);
	stackPanel->AddLogicalChild(rect);
	stackPanel->AddVisualChild(rect2);
	stackPanel->AddLogicalChild(rect2);
	auto trans = new Graphics::RotateTransform(20.f, Graphics::Point{ 0.f,0.f });
	rect->SetLayoutTransform(trans);
	//rect2->SetLayoutTransform(trans);
	/*window->WindowPaintEvent += [rect,trans](Object* sender, Yupei::WindowPaintArgs* args)
	{
		trans->angle = trans->angle >= 720.f ? 0.f: trans->angle + 3.f;
	};*/
	window->Run();
	delete brush;
	return 0;
}
