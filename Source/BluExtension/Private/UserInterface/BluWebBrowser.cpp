#include "UserInterface/BluWebBrowser.h"

#include "Components/Image.h"
#include "SlateOptMacros.h"

#include "Blu/Public/BluEye.h"
#include "BluWebBrowserWindow.h"

#include "UserInterface/BluWebBrowserView.h"

#define LOCTEXT_NAMESPACE "BluWebBrowser"

SBluWebBrowser::~SBluWebBrowser()
{
	if (BluEye)
	{
		BluEye->RemoveFromRoot();
	}
}

void SBluWebBrowser::OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const
{
	SCompoundWidget::OnArrangeChildren(AllottedGeometry, ArrangedChildren);

	FVector2D WidgetSize = AllottedGeometry.GetLocalSize();

	if (WidgetSize.X != CachedWidgetSize.X || WidgetSize.Y != CachedWidgetSize.Y)
	{
		CachedWidgetSize = WidgetSize;

		if (BrowserWindow && BluEye)
		{
			const float DPI = (BrowserWindow->GetParentWindow().IsValid() ? BrowserWindow->GetParentWindow()->GetNativeWindow()->GetDPIScaleFactor() : 1.0f);
			const float DPIScale = AllottedGeometry.Scale / DPI;
			FVector2D AbsoluteSize = AllottedGeometry.GetLocalSize() * DPIScale;
			BluEye->ResizeBrowser(AbsoluteSize.X, AbsoluteSize.Y);
			BrowserWindow->UpdateBrush();
		}
	}
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBluWebBrowser::Construct(const FArguments& Args)
{
	OnUrlChanged = Args._OnUrlChanged;

	if (!BluEye)
	{
		BluEye = NewObject<UBluEye>();

		BluEye->UrlChangeMulticastEventEmitter.AddLambda([this](const FString& Url)
		{
			OnUrlChanged.Execute(FText::FromString(Url));
		});

		BluEye->AddToRoot();

		BluEye->DefaultURL = Args._InitialURL;
		BluEye->bEnabled = true;

		if (Args._InitialWidth != INDEX_NONE)
		{
			BluEye->Settings.Width = Args._InitialWidth;
		}

		if (Args._InitialHeight != INDEX_NONE)
		{
			BluEye->Settings.Height = Args._InitialHeight;
		}

		check(BluEye->Init());
	}

	BrowserWindow = MakeShareable(new FBluCefWebBrowserWindow(BluEye->Browser));

	ChildSlot
	[
		SAssignNew(BrowserView, SBluWebBrowserView, BrowserWindow)
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBluWebBrowser::LoadURL(const FString& URL)
{
	if (BluEye)
	{
		BluEye->LoadURL(URL);
	}
}

TSharedRef<SWidget> UBluWebBrowser::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Web Browser", "Web Browser"))
			];
	}
	else
	{
		S_WebBrowser = SNew(SBluWebBrowser);

		return S_WebBrowser.ToSharedRef();
	}
}

#undef LOCTEXT_NAMESPACE