#pragma once

#include "UObject/Interface.h"

#include "BluWebBrowserWindow.generated.h"

UINTERFACE()
class UBluWebBrowserWindow : public UInterface
{
	GENERATED_BODY()
};

class BLUEXTENSION_API IBluWebBrowserWindow
{
	GENERATED_BODY()

public:
	/** A delegate to allow callbacks when a frame url changes. */
	DECLARE_EVENT_OneParam(IBluWebBrowserWindow, FOnToolTip, FString /* ToolTipText */);

	/** A delegate that is invoked when drag is detected in an area specified as a drag region on the web page. */
	DECLARE_DELEGATE_RetVal_OneParam(bool, FOnDragWindow, const FPointerEvent& /* MouseEvent */)

public:
	/** Checks whether the web browser is valid and ready for use. */
	virtual bool IsValid() const = 0;

	/** Gets the current size of the web browser viewport if available, FIntPoint::NoneValue otherwise. */
	virtual FIntPoint GetViewportSize() const = 0;

	/**
	 * Gets interface to the texture representation of the browser
	 *
	 * @param bIsPopup Whether to return the popup menu texture instead of the main browser window.
	 * @return A slate shader resource that can be rendered
	 */
	virtual FSlateShaderResource* GetTexture(bool bIsPopup = false) = 0;

	/** Get parent SWindow for this window */
	virtual TSharedPtr<SWindow> GetParentWindow() const = 0;

	/** Returns true if the browser can navigate backwards. */
	virtual bool CanGoBack() const = 0;

	/** Returns true if the browser can navigate forwards. */
	virtual bool CanGoForward() const = 0;

	virtual FOnToolTip& OnToolTip() = 0;
	virtual FOnDragWindow& OnDragWindow() = 0;

	/**
	 * Set the desired size of the web browser viewport
	 * 
	 * @param WindowSize Desired viewport size
	 * @param WindowPos Desired viewport position
	 */
	virtual void SetViewportSize(FIntPoint WindowSize, FIntPoint WindowPos = FIntPoint::NoneValue) = 0;

	/** Set parent SWindow for this window */
	virtual void SetParentWindow(TSharedPtr<SWindow> Window) = 0;

	virtual TSharedPtr<SViewport> CreateWidget() = 0;

	/**
	 * Notify the browser of a character event
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param CharacterEvent Character event
	 */
	virtual FReply OnKeyChar(const FGeometry& MyGeometry, const FCharacterEvent& CharacterEvent) = 0;

	/**
	 * Notify the browser that a key has been pressed
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param  KeyEvent  Key event
	 */
	virtual FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) = 0;

	/**
	 * Notify the browser that a key has been released
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param  KeyEvent  Key event
	 */
	virtual FReply OnKeyUp(const FGeometry& MyGeometry, const FKeyEvent& KeyEvent) = 0;

	/**
	 * Notify the browser that a mouse button was pressed within it
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) = 0;

	/**
	 * Notify the browser that a mouse button was released within it
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) = 0;

	/**
	 * Notify the browser that a mouse moved within it
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnMouseMove(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) = 0;

	/**
	 * Notify the browser that a mouse has entered the window
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 */
	virtual void OnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) = 0;

	/**
	 * Notify the browser that a mouse has left the window
	 *
	 * @param MouseEvent Information about the input event
	 */
	virtual void OnMouseLeave(const FPointerEvent& MouseEvent) = 0;

	/**
	 * Called when the mouse wheel is spun
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnMouseWheel(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) = 0;

	/**
	 * Notify the browser of a double click event
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param MouseEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnMouseButtonDoubleClick(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, bool bIsPopup) = 0;

	/**
	 * Called when a touch gesture is performed.
	 *
	 * @param MyGeometry The Geometry of the browser
	 * @param GestureEvent Information about the input event
	 * @param bIsPopup True if the coordinates are relative to a popup menu window, otherwise false.
	 *
	 * @return FReply::Handled() if the mouse event was handled, FReply::Unhandled() otherwise
	 */
	virtual FReply OnTouchGesture(const FGeometry& MyGeometry, const FPointerEvent& GestureEvent, bool bIsPopup) = 0;

	/** Navigate backwards. */
	virtual void GoBack() = 0;

	/** Navigate forwards. */
	virtual void GoForward() = 0;
};