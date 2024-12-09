#include "UserInterface/BluWebBrowserView.h"

#include "BluWebBrowserViewport.h"
#include "SlateOptMacros.h"
#include "Widgets/SViewport.h"

#include "BluWebBrowserWindow.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SBluWebBrowserView::Construct(const FArguments& NewArgs, const TSharedPtr<IBluWebBrowserWindow> NewWebBrowserWindow)
{
	WebBrowserWindow = NewWebBrowserWindow;

	if (WebBrowserWindow.IsValid())
	{
		const TSharedRef<SViewport>& BrowserWidgetRef = static_cast<FBluWebBrowserWindow*>(WebBrowserWindow.Get())->CreateWidget().ToSharedRef();

		ChildSlot
		[
			BrowserWidgetRef
		];

		BrowserViewport = MakeShareable(new FBluWebBrowserViewport(WebBrowserWindow));

		BrowserWidget = BrowserWidgetRef;
		BrowserWidget->SetViewportInterface(BrowserViewport.ToSharedRef());
	}
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION