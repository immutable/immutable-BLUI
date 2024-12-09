#include "UserInterface/BluWebBrowser.h"

#include "Components/Image.h"
#include "SlateOptMacros.h"

#include "Blu/Public/BluEye.h"
#include "BluWebBrowserWindow.h"

#include "UserInterface/BluWebBrowserView.h"

#define LOCTEXT_NAMESPACE "BluWebBrowser"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBluWebBrowser::Construct(const FArguments& Args, TSharedPtr<IBluWebBrowserWindow> BrowserWindow)
{
	ChildSlot
	[
		SAssignNew(BrowserView, SBluWebBrowserView, BrowserWindow)
	];
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void UBluWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
}

void UBluWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();
}

void UBluWebBrowser::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BluEye = NewObject<UBluEye>(this);

	BluEye->DefaultURL = InitialURL;
	BluEye->bEnabled = true;

	if (InitialWidth != INDEX_NONE)
	{
		BluEye->Settings.Width = InitialWidth;
	}

	if (InitialHeight != INDEX_NONE)
	{
		BluEye->Settings.Height = InitialHeight;
	}

	check(BluEye->Init());
}

UBluEye* UBluWebBrowser::GetBluEye() const
{
	return BluEye;
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
		TSharedPtr<FBluWebBrowserWindow> BrowserWindow(new FBluWebBrowserWindow(BluEye->Browser));

		S_WebBrowser = SNew(SBluWebBrowser, BrowserWindow);

		return S_WebBrowser.ToSharedRef();
	}
}

#undef LOCTEXT_NAMESPACE
