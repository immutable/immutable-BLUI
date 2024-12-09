#include "BluWebBrowserViewport.h"

#include "Interface/BluWebBrowserWindow.h"

FBluWebBrowserViewport::FBluWebBrowserViewport(TSharedPtr<IBluWebBrowserWindow> NewWebBrowserWindow, bool bNewIsPopup) :
	WebBrowserWindow(NewWebBrowserWindow),
	bIsPopup(bNewIsPopup)
{
}

FIntPoint FBluWebBrowserViewport::GetSize() const
{
	return (WebBrowserWindow->GetTexture(bIsPopup) != nullptr) ? FIntPoint(WebBrowserWindow->GetTexture(bIsPopup)->GetWidth(), WebBrowserWindow->GetTexture(bIsPopup)->GetHeight()) : FIntPoint();
}

FSlateShaderResource* FBluWebBrowserViewport::GetViewportRenderTargetTexture() const
{
	return WebBrowserWindow->GetTexture(bIsPopup);
}

void FBluWebBrowserViewport::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float DeltaTime)
{
	if (!bIsPopup)
	{
		const float DPI = (WebBrowserWindow->GetParentWindow().IsValid() ? WebBrowserWindow->GetParentWindow()->GetNativeWindow()->GetDPIScaleFactor() : 1.0f);
		const float DPIScale = AllottedGeometry.Scale / DPI;
		FVector2D AbsoluteSize = AllottedGeometry.GetLocalSize() * DPIScale;
		WebBrowserWindow->SetViewportSize(AbsoluteSize.IntPoint(), AllottedGeometry.GetAbsolutePosition().IntPoint());
	}
}

bool FBluWebBrowserViewport::RequiresVsync() const
{
	return false;
}

FReply FBluWebBrowserViewport::OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent)
{
	return WebBrowserWindow->OnKeyChar(MyGeometry, CharacterEvent);
}

FReply FBluWebBrowserViewport::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	return WebBrowserWindow->OnKeyDown(MyGeometry, KeyEvent);
}

FReply FBluWebBrowserViewport::OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent)
{
	return WebBrowserWindow->OnKeyUp(MyGeometry, KeyEvent);
}

FReply FBluWebBrowserViewport::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return WebBrowserWindow->OnMouseButtonDown(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBluWebBrowserViewport::OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return WebBrowserWindow->OnMouseButtonUp(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBluWebBrowserViewport::OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return WebBrowserWindow->OnMouseMove(MyGeometry, MouseEvent, bIsPopup);
}

void FBluWebBrowserViewport::OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	WebBrowserWindow->OnMouseEnter(MyGeometry, MouseEvent);
}

void FBluWebBrowserViewport::OnMouseLeave(const FPointerEvent& MouseEvent)
{
	WebBrowserWindow->OnMouseLeave(MouseEvent);
}

FReply FBluWebBrowserViewport::OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return WebBrowserWindow->OnMouseWheel(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBluWebBrowserViewport::OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	return WebBrowserWindow->OnMouseButtonDoubleClick(MyGeometry, MouseEvent, bIsPopup);
}

FReply FBluWebBrowserViewport::OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent)
{
	return WebBrowserWindow->OnTouchGesture(MyGeometry, GestureEvent, bIsPopup);
}