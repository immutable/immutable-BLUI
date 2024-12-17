#include "Cef/BluCefWebBrowserWindow.h"

#include "Widgets/Images/SImage.h"
#include "Widgets/SViewport.h"

#include "BluEye.h"
#include "RenderHandler.h"

FBluCefWebBrowserWindow::FBluCefWebBrowserWindow(CefRefPtr<CefBrowser> NewCefBrowser) :
	FBluCefWebBrowserWindowBase(NewCefBrowser)
{
}

FIntPoint FBluCefWebBrowserWindow::GetViewportSize() const
{
	if (UBluEye* BluEye = GetBluEye())
	{
		return FIntPoint(BluEye->Settings.Width, BluEye->Settings.Height);
	}
	return FIntPoint::NoneValue;
}

TSharedPtr<SViewport> FBluCefWebBrowserWindow::CreateWidget()
{
	UpdateBrush();

	TSharedRef<SViewport> BrowserWidgetRef =
		SNew(SViewport)
		.EnableGammaCorrection(false)
		[
			SNew(SImage)
			.Image(&Brush)
		];

	return BrowserWidgetRef;
}

void FBluCefWebBrowserWindow::UpdateBrush()
{
	CefRefPtr<CefRenderHandler> CefRenderHandler = InternalCefBrowser->GetHost()->GetClient()->GetRenderHandler();

	if (RenderHandler* BluRenderHandler = static_cast<RenderHandler*>(CefRenderHandler.get()))
	{
		UTexture2D* Texture = BluRenderHandler->ParentUI->GetTexture();

		if (Brush.GetResourceObject() != Texture)
		{
			Brush.SetResourceObject(Texture);

			if (Texture)
			{
				Texture->bForceMiplevelsToBeResident = true;
				Texture->bIgnoreStreamingMipBias = true;
			}
		}
	}
}

UBluEye* FBluCefWebBrowserWindow::GetBluEye() const
{
	CefRefPtr<CefRenderHandler> CefRenderHandler = InternalCefBrowser->GetHost()->GetClient()->GetRenderHandler();

	if (RenderHandler* BluRenderHandler = static_cast<RenderHandler*>(CefRenderHandler.get()))
	{
		return BluRenderHandler->ParentUI;
	}

	return nullptr;
}