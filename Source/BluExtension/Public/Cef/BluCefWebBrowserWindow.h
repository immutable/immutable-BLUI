#pragma once

#include "BluCefWebBrowserWindowBase.h"

class BLUEXTENSION_API FBluCefWebBrowserWindow : public FBluCefWebBrowserWindowBase
{
public:
	FBluCefWebBrowserWindow(CefRefPtr<CefBrowser> NewCefBrowser);

	/** IBluWebBrowserWindow: @Interface @Begin */
	virtual FIntPoint GetViewportSize() const override;

	virtual TSharedPtr<SViewport> CreateWidget() override;
	/** IBluWebBrowserWindow: @Interface @End */

	void UpdateBrush();

protected:
	UBluEye* GetBluEye() const;

protected:
	FSlateBrush Brush;
};