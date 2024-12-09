#pragma once

#include "Widgets/SCompoundWidget.h"

class FBluWebBrowserViewport;
class IBluWebBrowserWindow;

class BLUEXTENSION_API SBluWebBrowserView : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SBluWebBrowserView)
		{
		}

	SLATE_END_ARGS()

	void Construct(const FArguments& NewArgs, const TSharedPtr<IBluWebBrowserWindow> NewWebBrowserWindow);

protected:
	/** Interface for dealing with a web browser window. */
	TSharedPtr<IBluWebBrowserWindow> WebBrowserWindow;

	/** Viewport interface for rendering the web page. */
	TSharedPtr<FBluWebBrowserViewport> BrowserViewport;

	/** The implementation dependent widget that renders the browser contents. */
	TSharedPtr<SViewport> BrowserWidget;
};